#include "pilfflonk_prover.hpp"
#include <stdio.h>
#include "zkey.hpp"
#include <math.h>


namespace PilFflonk
{
    void PilFflonkProver::initialize(void* reservedMemoryPtr, uint64_t reservedMemorySize)
    {
        zkey = NULL;
        // this->reservedMemoryPtr = (FrElement *)reservedMemoryPtr;
        // this->reservedMemorySize = reservedMemorySize;

        curveName = "bn128";
    }

    PilFflonkProver::PilFflonkProver(AltBn128::Engine &_E, std::string fflonkInfoFile) : E(_E), fflonkInfo(_E, fflonkInfoFile)
    {
        initialize(NULL);
    }

    PilFflonkProver::PilFflonkProver(AltBn128::Engine &_E, std::string fflonkInfoFile, void* reservedMemoryPtr, uint64_t reservedMemorySize) : E(_E), fflonkInfo(_E, fflonkInfoFile)
    {
        initialize(reservedMemoryPtr, reservedMemorySize);
    }

    PilFflonkProver::~PilFflonkProver() {
        this->removePrecomputedData();

        delete transcript;
        delete proof;
    }

    void PilFflonkProver::removePrecomputedData() {
        // // DELETE RESERVED MEMORY (if necessary)
        // delete[] precomputedBigBuffer;
        // delete[] mapBuffersBigBuffer;
        // delete[] buffInternalWitness;

        // if(NULL == reservedMemoryPtr) {
        //     delete[] inverses;
        //     delete[] products;
        //     delete[] nonPrecomputedBigBuffer;
        // }

        //delete fft;

        // mapBuffers.clear();

        // for (auto const &x : roots) delete[] x.second;
        // roots.clear();

        // delete polynomials["QL"];
        // delete polynomials["QR"];
        // delete polynomials["QM"];
        // delete polynomials["QO"];
        // delete polynomials["QC"];
        // delete polynomials["Sigma1"];
        // delete polynomials["Sigma2"];
        // delete polynomials["Sigma3"];
        // delete polynomials["C0"];

        // delete evaluations["QL"];
        // delete evaluations["QR"];
        // delete evaluations["QM"];
        // delete evaluations["QO"];
        // delete evaluations["QC"];
        // delete evaluations["Sigma1"];
        // delete evaluations["Sigma2"];
        // delete evaluations["Sigma3"];
        // delete evaluations["lagrange"];
    }

    void PilFflonkProver::setZkey(BinFileUtils::BinFile *zkeyBinfile) {
        try
        {
            if(NULL != zkey) {
                removePrecomputedData();
            }

            cout << "> Reading zkey file" << endl;

            if (Zkey::getProtocolIdFromZkey(zkeyBinfile) != Zkey::PILFFLONK_PROTOCOL_ID)
            {
                throw std::invalid_argument("zkey file is not pilfflonk");
            }

// TODO TODO TODO ADD fflonkInfo
            zkey = PilFflonkZkey::loadPilFflonkZkey(zkeyBinfile);

            n8r = sizeof(FrElement);
            N = 1 << zkey->power;
            nBits = zkey->power;
            extendBits = ceil(fflonkInfo->qDeg + 1);
            nBitsExt = nBits + extendBits;
            NExt = 1 << nBitsExt;
            sDomain = N * n8r;
            sDomainExt = NExt * n8r;
    
            // ZK data
            extendBitsZK = zkey->powerZK - nBits;
            factorZK = (1 << extendBitsZK);
            extendBitsTotal = extendBits + extendBitsZK;
            nBitsExtZK = nBits + extendBitsTotal;

            cout << "> Starting fft" << endl;

            fft = new FFT<AltBn128::Engine::Fr>(N * 4);

            mpz_t altBbn128r;
            mpz_init(altBbn128r);
            mpz_set_str(altBbn128r, "21888242871839275222246405745257275088548364400416034343698204186575808495617", 10);

            if (mpz_cmp(zkey->rPrime, altBbn128r) != 0)
            {
                throw std::invalid_argument("zkey curve not supported");
            }




        // G1PointAffine *PTau;

        // std::map<std::string, FrElement *> ptr;
        // std::map<std::string, FrElement *> ptrConst;

            // //////////////////////////////////////////////////
            // CONSTANT BIG BUFFER
            // //////////////////////////////////////////////////
            lengthBufferConst = 0;

            u_int32_t maxFiDegree = 0;
            for (auto const& [key, f] : zkey->f) maxFiDegree = max(maxFiDegree, f->degree);
            maxFiDegree += 1;

            // Polynomial evaluations
            lengthBufferConst += N * fflonkInfo->nConstants;                              // const_n     >> Constant polynomials evaluations
            lengthBufferConst += N * factorZK * fflonkInfo->nConstants;                   // const_coefs >> Constant polynomials coefficients
            lengthBufferConst += NExt * factorZK * fflonkInfo->nConstants;                // const_2ns   >> Constant polynomials extended evaluations
            lengthBufferConst += maxFiDegree * sizeof(G1PointAffine) / sizeof(FrElement); // PTau buf

            bBufferConst = new FrElement[lengthBufferConst];

            ptr["const_n"]     = &bBufferConst[0];
            ptr["const_coefs"] = ptr["const_n"]     + N * fflonkInfo->nConstants;
            ptr["const_2ns"]   = ptr["const_coefs"] + N * factorZK * fflonkInfo->nConstants;
            PTau = (G1PointAffine *)(ptr["const_2ns"] + NExt * factorZK * fflonkInfo->nConstants);

            // //////////////////////////////////////////////////
            // BIG BUFFER
            // //////////////////////////////////////////////////
            lengthBuffer = 0;

            lengthBuffer += N * fflonkInfo->mapSectionsN.cm1_n;               // cm1_n       >> Stage1, committed polynomials evaluations
            lengthBuffer += N * fflonkInfo->mapSectionsN.cm2_n;               // cm2_n       >> Stage2, H1&H2 polynomials evaluations
            lengthBuffer += N * fflonkInfo->mapSectionsN.cm3_n;               // cm3_n       >> Stage3, Z polynomial evaluations
            lengthBuffer += N * fflonkInfo->fflonkInfo.mapSectionsN.tmpExp_n; // tmpExp_n    >> Temporal expressions polynomials evaluations
            lengthBuffer += N;                                                // x_n         >> 

            lengthBuffer += N * factorZK * fflonkInfo->mapSectionsN.cm1_n;    // cm1_coefs   >> Constant polynomials coefficients
            lengthBuffer += N * factorZK * fflonkInfo->mapSectionsN.cm2_n;    // cm2_coefs   >> Constant polynomials coefficients
            lengthBuffer += N * factorZK * fflonkInfo->mapSectionsN.cm3_n;    // cm3_coefs   >> Constant polynomials coefficients

            lengthBuffer += NExt * factorZK * fflonkInfo->mapSectionsN.cm1_n; // cm1_2ns     >> Stage1, committed polynomials extended evaluations
            lengthBuffer += NExt * factorZK * fflonkInfo->mapSectionsN.cm2_n; // cm2_2ns     >> Stage2, H1&H2 polynomials extended evaluations
            lengthBuffer += NExt * factorZK * fflonkInfo->mapSectionsN.cm3_n; // cm3_2ns     >> Stage3, Z polynomial extended evaluations
            lengthBuffer += NExt * factorZK * fflonkInfo->qDim;               // q_2ns       >> Stage4, Q polynomial extended evaluations
            lengthBuffer += NExt * factorZK;                                  // x_2ns       >>

            bBuffer = new FrElement[lengthBuffer];

            ptr["cm1_n"]     = &bBuffer[0];
            ptr["cm2_n"]     = ptr["cm1_n"]     + N * fflonkInfo->mapSectionsN.cm1_n;
            ptr["cm3_n"]     = ptr["cm2_n"]     + N * fflonkInfo->mapSectionsN.cm2_n;
            ptr["tmpExp_n"]  = ptr["cm3_n"]     + N * fflonkInfo->mapSectionsN.cm3_n;
            ptr["x_n"]       = ptr["tmpExp_n"]  + N * fflonkInfo->fflonkInfo.mapSectionsN.tmpExp_n;

            ptr["cm1_coefs"] = ptr["x_n"]       + N;
            ptr["cm2_coefs"] = ptr["cm1_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.cm1_n;
            ptr["cm3_coefs"] = ptr["cm2_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.cm2_n;

            ptr["cm1_2ns"]   = ptr["cm3_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.cm2_n;
            ptr["cm2_2ns"]   = ptr["cm1_2ns"]   + NExt * factorZK * fflonkInfo->mapSectionsN.cm1_n;
            ptr["cm3_2ns"]   = ptr["cm2_2ns"]   + NExt * factorZK * fflonkInfo->mapSectionsN.cm2_n;
            ptr["q_2ns"]     = ptr["cm3_2ns"]   + NExt * factorZK * fflonkInfo->mapSectionsN.cm3_n;
            ptr["x_2ns"]     = ptr["q_2ns"]     + NExt * factorZK * fflonkInfo->qDim;

            int nThreads = omp_get_max_threads() / 2;

            ThreadUtils::parset(PTau, 0, sizeof(G1PointAffine) * maxFiDegree, nThreads);

            ThreadUtils::parcpy(PTau,
                                (G1PointAffine *)(zkeyBinfile->getSectionData(PilFflonkZkey::ZKEY_PF_PTAU_SECTION)),
                                sizeof(G1PointAffine) * maxFiDegree, nThreads);

            // // Read Q selectors polynomials and evaluations
            // LOG_TRACE("... Loading QL, QR, QM, QO, & QC polynomial coefficients and evaluations");

            // // Reserve memory for Q's polynomials
            // polynomials["QL"] = new Polynomial<Engine>(E, polPtr["QL"], zkey->domainSize);
            // polynomials["QR"] = new Polynomial<Engine>(E, polPtr["QR"], zkey->domainSize);
            // polynomials["QM"] = new Polynomial<Engine>(E, polPtr["QM"], zkey->domainSize);
            // polynomials["QO"] = new Polynomial<Engine>(E, polPtr["QO"], zkey->domainSize);
            // polynomials["QC"] = new Polynomial<Engine>(E, polPtr["QC"], zkey->domainSize);


            // // Read Q's polynomial coefficients from zkey file
            // ThreadUtils::parcpy(polynomials["QL"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QL_SECTION),
            //                     sDomain, nThreads);
            // ThreadUtils::parcpy(polynomials["QR"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QR_SECTION),
            //                     sDomain, nThreads);
            // ThreadUtils::parcpy(polynomials["QM"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QM_SECTION),
            //                     sDomain, nThreads);
            // ThreadUtils::parcpy(polynomials["QO"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QO_SECTION),
            //                     sDomain, nThreads);
            // ThreadUtils::parcpy(polynomials["QC"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QC_SECTION),
            //                     sDomain, nThreads);

            // polynomials["QL"]->fixDegree();
            // polynomials["QR"]->fixDegree();
            // polynomials["QM"]->fixDegree();
            // polynomials["QO"]->fixDegree();
            // polynomials["QC"]->fixDegree();

            // std::ostringstream ss;
            // ss << "... Reading Q selector evaluations ";

            // // Reserve memory for Q's evaluations
            // evaluations["QL"] = new Evaluations<Engine>(E, evalPtr["QL"], zkey->domainSize * 4);
            // evaluations["QR"] = new Evaluations<Engine>(E, evalPtr["QR"], zkey->domainSize * 4);
            // evaluations["QM"] = new Evaluations<Engine>(E, evalPtr["QM"], zkey->domainSize * 4);
            // evaluations["QO"] = new Evaluations<Engine>(E, evalPtr["QO"], zkey->domainSize * 4);
            // evaluations["QC"] = new Evaluations<Engine>(E, evalPtr["QC"], zkey->domainSize * 4);

            // // Read Q's evaluations from zkey file
            // ThreadUtils::parcpy(evaluations["QL"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QL_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);
            // ThreadUtils::parcpy(evaluations["QR"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QR_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);
            // ThreadUtils::parcpy(evaluations["QM"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QM_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);
            // ThreadUtils::parcpy(evaluations["QO"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QO_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);
            // ThreadUtils::parcpy(evaluations["QC"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_QC_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);

            // // Read Sigma polynomial coefficients and evaluations from zkey file
            // LOG_TRACE("... Loading Sigma1, Sigma2 & Sigma3 polynomial coefficients and evaluations");

            // polynomials["Sigma1"] = new Polynomial<Engine>(E, polPtr["Sigma1"], zkey->domainSize);
            // polynomials["Sigma2"] = new Polynomial<Engine>(E, polPtr["Sigma2"], zkey->domainSize);
            // polynomials["Sigma3"] = new Polynomial<Engine>(E, polPtr["Sigma3"], zkey->domainSize);

            // ThreadUtils::parcpy(polynomials["Sigma1"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_SIGMA1_SECTION),
            //                     sDomain, nThreads);
            // ThreadUtils::parcpy(polynomials["Sigma2"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_SIGMA2_SECTION),
            //                     sDomain, nThreads);
            // ThreadUtils::parcpy(polynomials["Sigma3"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_SIGMA3_SECTION),
            //                     sDomain, nThreads);

            // polynomials["Sigma1"]->fixDegree();
            // polynomials["Sigma2"]->fixDegree();
            // polynomials["Sigma3"]->fixDegree();

            // evaluations["Sigma1"] = new Evaluations<Engine>(E, evalPtr["Sigma1"], zkey->domainSize * 4);
            // evaluations["Sigma2"] = new Evaluations<Engine>(E, evalPtr["Sigma2"], zkey->domainSize * 4);
            // evaluations["Sigma3"] = new Evaluations<Engine>(E, evalPtr["Sigma3"], zkey->domainSize * 4);

            // ThreadUtils::parcpy(evaluations["Sigma1"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_SIGMA1_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);
            // ThreadUtils::parcpy(evaluations["Sigma2"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_SIGMA2_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);
            // ThreadUtils::parcpy(evaluations["Sigma3"]->eval,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_SIGMA3_SECTION) + zkey->domainSize,
            //                     sDomain * 4, nThreads);

            // LOG_TRACE("... Loading C0 polynomial coefficients");
            // polynomials["C0"] = new Polynomial<Engine>(E, polPtr["C0"], zkey->domainSize * 8);
            // ThreadUtils::parcpy(polynomials["C0"]->coef,
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_C0_SECTION),
            //                     sDomain * 8, nThreads);
            // polynomials["C0"]->fixDegree();

            // // Read Lagrange polynomials & evaluations from zkey file
            // LOG_TRACE("... Loading Lagrange evaluations");
            // evaluations["lagrange"] = new Evaluations<Engine>(E, evalPtr["lagrange"], zkey->domainSize * 4 * zkey->nPublic);
            // for(uint64_t i = 0 ; i < zkey->nPublic ; i++) {
            //     ThreadUtils::parcpy(evaluations["lagrange"]->eval + zkey->domainSize * 4 * i,
            //                         (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_LAGRANGE_SECTION) + zkey->domainSize + zkey->domainSize * 5 * i,
            //                         sDomain * 4, nThreads);
            // }
            // LOG_TRACE("... Loading Powers of Tau evaluations");

            // // Load A, B & C map buffers
            // LOG_TRACE("... Loading A, B & C map buffers");

            // u_int64_t byteLength = sizeof(u_int32_t) * zkey->nConstraints;

            // mapBuffersBigBuffer = new u_int32_t[zkey->nConstraints * 3];

            // mapBuffers["A"] = mapBuffersBigBuffer;
            // mapBuffers["B"] = mapBuffers["A"] + zkey->nConstraints;
            // mapBuffers["C"] = mapBuffers["B"] + zkey->nConstraints;

            // buffInternalWitness = new FrElement[zkey->nAdditions];

            // LOG_TRACE("··· Loading additions");
            // additionsBuff = (Zkey::Addition<Engine> *)fdZkey->getSectionData(Zkey::ZKEY_FF_ADDITIONS_SECTION);

            // LOG_TRACE("··· Loading map buffers");
            // ThreadUtils::parset(mapBuffers["A"], 0, byteLength * 3, nThreads);

            // // Read zkey sections and fill the buffers
            // ThreadUtils::parcpy(mapBuffers["A"],
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_A_MAP_SECTION),
            //                     byteLength, nThreads);
            // ThreadUtils::parcpy(mapBuffers["B"],
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_B_MAP_SECTION),
            //                     byteLength, nThreads);
            // ThreadUtils::parcpy(mapBuffers["C"],
            //                     (FrElement *)fdZkey->getSectionData(Zkey::ZKEY_FF_C_MAP_SECTION),
            //                     byteLength, nThreads);

            transcript = new Keccak256Transcript(E);
            proof = new SnarkProof(E, "pilfflonk");

            // roots["w8"] = new FrElement[8];
            // roots["w4"] = new FrElement[4];
            // roots["w3"] = new FrElement[3];
            // roots["S0h0"] = new FrElement[8];
            // roots["S1h1"] = new FrElement[4];
            // roots["S2h2"] = new FrElement[3];
            // roots["S2h3"] = new FrElement[3];

            // lengthBatchInversesBuffer = zkey->domainSize * 2;

            // if(NULL == this->reservedMemoryPtr) {
            //     inverses = new FrElement[zkey->domainSize];
            //     products = new FrElement[zkey->domainSize];
            // } else {
            //     inverses = this->reservedMemoryPtr;
            //     products = inverses + zkey->domainSize;
            // }

            // ////////////////////////////////////////////////////
            // // NON-PRECOMPUTED BIG BUFFER
            // ////////////////////////////////////////////////////
            // // Non-precomputed 1 > polynomials buffer
            // lengthNonPrecomputedBigBuffer = 0;
            // lengthNonPrecomputedBigBuffer += zkey->domainSize * 16 * 1; // Polynomial L (A, B & C will (re)use this buffer)
            // lengthNonPrecomputedBigBuffer += zkey->domainSize * 8  * 1; // Polynomial C1
            // lengthNonPrecomputedBigBuffer += zkey->domainSize * 16 * 1; // Polynomial C2
            // lengthNonPrecomputedBigBuffer += zkey->domainSize * 16 * 1; // Polynomial F
            // lengthNonPrecomputedBigBuffer += zkey->domainSize * 16 * 1; // Polynomial tmp (Z, T0, T1, T1z, T2 & T2z will (re)use this buffer)
            // // Non-precomputed 2 > evaluations buffer
            // lengthNonPrecomputedBigBuffer += zkey->domainSize * 4  * 3; // Evaluations A, B & C
            // lengthNonPrecomputedBigBuffer += zkey->domainSize * 4  * 1; // Evaluations Z
            // // Non-precomputed 3 > buffers buffer
            // buffersLength = 0;
            // buffersLength   += zkey->domainSize * 4  * 3; // Evaluations A, B & C
            // buffersLength   += zkey->domainSize * 16 * 1; // Evaluations tmp (Z, numArr, denArr, T0, T1, T1z, T2 & T2z will (re)use this buffer)
            // lengthNonPrecomputedBigBuffer += buffersLength;

            // if(NULL == this->reservedMemoryPtr) {
            //     nonPrecomputedBigBuffer = new FrElement[lengthNonPrecomputedBigBuffer];
            // } else {
            //     if((lengthBatchInversesBuffer + lengthNonPrecomputedBigBuffer) * sizeof(FrElement) > reservedMemorySize) {
            //         ss.str("");
            //         ss << "Not enough reserved memory to generate a prove. Increase reserved memory size at least to "
            //             << (lengthBatchInversesBuffer + lengthNonPrecomputedBigBuffer) * sizeof(FrElement) << " bytes";
            //         throw std::runtime_error(ss.str());
            //     }

            //     nonPrecomputedBigBuffer = this->reservedMemoryPtr + lengthBatchInversesBuffer;
            // }
            
            // polPtr["L"] = &nonPrecomputedBigBuffer[0];
            // polPtr["C1"]  = polPtr["L"]  + zkey->domainSize * 16;
            // polPtr["C2"]  = polPtr["C1"] + zkey->domainSize * 8;
            // polPtr["F"]   = polPtr["C2"] + zkey->domainSize * 16;
            // polPtr["tmp"] = polPtr["F"]  + zkey->domainSize * 16;
            // // Reuses
            // polPtr["A"]   = polPtr["L"];
            // polPtr["B"]   = polPtr["A"]  + zkey->domainSize;
            // polPtr["C"]   = polPtr["B"]  + zkey->domainSize;
            // polPtr["Z"]   = polPtr["tmp"];
            // polPtr["T0"]  = polPtr["Z"]  + zkey->domainSize * 2;
            // polPtr["T1"]  = polPtr["T0"] + zkey->domainSize * 4;
            // polPtr["T1z"] = polPtr["T1"] + zkey->domainSize * 2;
            // polPtr["T2"]  = polPtr["T1"] + zkey->domainSize * 2;
            // polPtr["T2z"] = polPtr["T2"] + zkey->domainSize * 4;

            // evalPtr["A"] = polPtr["F"];
            // evalPtr["B"] = evalPtr["A"]  + zkey->domainSize * 4;
            // evalPtr["C"] = evalPtr["B"]  + zkey->domainSize * 4;
            // evalPtr["Z"] = evalPtr["C"]  + zkey->domainSize * 4;

            // buffers["A"]   = polPtr["tmp"]  + zkey->domainSize * 16;
            // buffers["B"]   = buffers["A"]  + zkey->domainSize;
            // buffers["C"]   = buffers["B"]  + zkey->domainSize;
            // buffers["tmp"] = buffers["C"]  + zkey->domainSize;

            // // Reuses
            // buffers["Z"]      = buffers["tmp"];
            // buffers["numArr"] = buffers["tmp"];
            // buffers["denArr"] = buffers["numArr"] + zkey->domainSize;
            // buffers["T0"]     = buffers["tmp"];
            // buffers["T1"]     = buffers["tmp"];
            // buffers["T1z"]    = buffers["tmp"] + zkey->domainSize * 2;
            // buffers["T2"]     = buffers["tmp"];
            // buffers["T2z"]    = buffers["tmp"] + zkey->domainSize * 4;
        }

        catch (const std::exception &e)
        {
            std::cerr << "EXCEPTION: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    /*std::tuple <json, json>*/ void PilFflonkProver::prove(BinFileUtils::BinFile *fdZkey) {
        this->setZkey(fdZkey);
        
        return this->prove();
    }

    /*std::tuple<json, json>*/ void PilFflonkProver::prove()
    {
        if(NULL == zkey) {
            throw std::runtime_error("Zkey data not set");
        }

        try
        {
            LOG_TRACE("FFLONK PROVER STARTED");

            // this->buffWitness = buffWitness;

            // if(NULL != wtnsHeader) {
            //     if (mpz_cmp(zkey->rPrime, wtnsHeader->prime) != 0)
            //     {
            //         throw std::invalid_argument("Curve of the witness does not match the curve of the proving key");
            //     }

            //     if (wtnsHeader->nVars != zkey->nVars - zkey->nAdditions)
            //     {
            //         std::ostringstream ss;
            //         ss << "Invalid witness length. Circuit: " << zkey->nVars << ", witness: " << wtnsHeader->nVars << ", "
            //         << zkey->nAdditions;
            //         throw std::invalid_argument(ss.str());
            //     }
            // }

            cout << "-----------------------------" << endl;
            cout << "  PIL-FFLONK PROVE SETTINGS" << endl;
            cout << "  Curve:           " << curveName << endl;
            cout << "  Circuit power:   " << nBits << endl;
            cout << "  Domain size:     " << N << endl;
            cout << "  Extended Bits:   " << extendBits << endl;
            cout << "  Domain size ext: " << NExt << "(2^" << nBits + extendBits << ")" << endl;
            cout << "  Const  pols:     " << fflonkInfo->nConstants << endl;
            cout << "  Stage 1 pols:    " << fflonkInfo->mapSectionsN->cm1_n << endl;
            cout << "  Stage 2 pols:    " << fflonkInfo->mapSectionsN->cm2_n << endl;
            cout << "  Stage 3 pols:    " << fflonkInfo->mapSectionsN->cm3_n << endl;
            cout << "  Temp exp pols:   " << fflonkInfo->mapSectionsN->tmpExp_n << endl;
            cout << "-----------------------------" << endl;

            cout << "PIL-FFLONK PROVER STARTED" << endl << endl;

            transcript->reset();
            proof->reset();

            // TODO add to precomputed?????
            for (u_int64_t i = 0; i < N; i++) {
                ptr["x_n"][i] = fft->root(nBits, i);
            }

            for (u_int64_t i = 0; i < NExt * factorZK; i++) {
                ptr["x_2ns"][i] = fft->root(nBitsExtZK, i);
            }

            //const committedPols = {};

            // STAGE 0. Store constants and committed values. Calculate publics
            stage0();

            // STAGE 1. Compute Trace Column Polynomials
            cout << "> STAGE 1. Compute Trace Column Polynomials" << endl;
            //stage1();

            // STAGE 2. Compute Inclusion Polynomials
            cout << "> STAGE 2. Compute Inclusion Polynomials" << endl;
            //stage2();

            // STAGE 3. Compute Grand Product and Intermediate Polynomials
            cout << "> STAGE 3. Compute Grand Product and Intermediate Polynomials" << endl;
            //stage3();

            // STAGE 4. Trace Quotient Polynomials
            cout << "> STAGE 4. Compute Trace Quotient Polynomials" << endl;
            //stage4();

            // const [cmts, evaluations, xiSeed] = await open(zkey, PTau, ctx, committedPols, curve, { logger, fflonkPreviousChallenge: ctx.challenges[4], nonCommittedPols: ["Q"] });

            // // Compute xiSeed 
            // let challengeXi = curve.Fr.exp(xiSeed, zkey.powerW);

            // const xN = curve.Fr.exp(challengeXi, ctx.N);
            // const Z = curve.Fr.sub(xN, curve.Fr.one);

            // evaluations.invZh = curve.Fr.inv(Z);

            // await pool.terminate();

            // let proof = { polynomials: {}, evaluations: {} };
            // proof.protocol = "pilfflonk";
            // proof.curve = curve.name;
            // Object.keys(cmts).forEach(key => {
            //     proof.polynomials[key] = ctx.curve.G1.toObject(cmts[key]);
            // });

            // Object.keys(evaluations).forEach(key => {
            //     proof.evaluations[key] = ctx.curve.Fr.toObject(evaluations[key]);
            // });

            // proof = stringifyBigInts(proof);

            // // Prepare public inputs
            // let publicSignals = stringifyBigInts(ctx.publics.map(p => ctx.curve.Fr.toObject(p)));

            // return {
            //     proof,
            //     publicSignals,
            // };

            //             // First element in plonk is not used and can be any value. (But always the same).
            //             // We set it to zero to go faster in the exponentiations.
            //             buffWitness[0] = E.fr.zero();

            //             // To divide prime fields the Extended Euclidean Algorithm for computing modular inverses is needed.
            //             // NOTE: This is the equivalent of compute 1/denominator and then multiply it by the numerator.
            //             // The Extended Euclidean Algorithm is expensive in terms of computation.
            //             // For the special case where we need to do many modular inverses, there's a simple mathematical trick
            //             // that allows us to compute many inverses, called Montgomery batch inversion.
            //             // More info: https://vitalik.ca/general/2018/07/21/starks_part_3.html
            //             // Montgomery batch inversion reduces the n inverse computations to a single one
            //             // To save this (single) inverse computation on-chain, will compute it in proving time and send it to the verifier.
            //             // The verifier will have to check:
            //             // 1) the denominator is correct multiplying by himself non-inverted -> a * 1/a == 1
            //             // 2) compute the rest of the denominators using the Montgomery batch inversion
            //             // The inversions are:
            //             //   · denominator needed in step 8 and 9 of the verifier to multiply by 1/Z_H(xi)
            //             //   · denominator needed in step 10 and 11 of the verifier
            //             //   · denominator needed in the verifier when computing L_i^{S1}(X) and L_i^{S2}(X)
            //             //   · L_i i=1 to num public inputs, needed in step 6 and 7 of the verifier to compute L_1(xi) and PI(xi)
            //             // toInverse property is the variable to store the values to be inverted

            //             // Until this point all calculations made are circuit depending and independent from the data, so we could
            //             // this big function into two parts: until here circuit dependent and from here is the proof calculation

            //             double startTime = omp_get_wtime();

            //             LOG_TRACE("> Computing Additions");

            //             int nThreads = omp_get_max_threads() / 2;
            //             // Set 0's to buffers["A"], buffers["B"], buffers["C"] & buffers["Z"]
            //             ThreadUtils::parset(buffers["A"], 0, buffersLength * sizeof(FrElement), nThreads);

            //             calculateAdditions();

            //             // START FFLONK PROVER PROTOCOL

            //             // ROUND 1. Compute C1(X) polynomial
            //             LOG_TRACE("> ROUND 1");
            //             round1();
        }
        catch (const std::exception &e)
        {
            std::cerr << "EXCEPTION: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    void PilFflonkProver::stage0()
    {
        // STEP 0.1 - Prepare constant polynomial evaluations
        for (u_int32_t i = 0; i < cnstPols->nPols; i++) {
            string name = cnstPols->defArray[i].name;
            if (cnstPols->defArray[i].idx >= 0) name += cnstPols.$$defArray[i].idx;

            cout << "··· Preparing '" << name << "' constant polynomial" << endl;

            // Prepare constant polynomial evaluations
            const cnstPolBuffer = cnstPols.$$array[i];
            for (let j = 0; j < cnstPolBuffer.length; j++) {
                ctx.const_n.set(Fr.e(cnstPolBuffer[j]), (i + j * fflonkInfo.nConstants) * n8r);
            }
        }

        // STEP 0.2 - Prepare committed polynomial evaluations
        for (let i = 0; i < cmPols.$$nPols; i++) {
            let name = cmPols.$$defArray[i].name;
            if (cmPols.$$defArray[i].idx >= 0) name += cmPols.$$defArray[i].idx;

            cout << "··· Preparing '" << name << "' polynomial" << endl;

            // Prepare committed polynomial evaluations
            const cmPolBuffer = cmPols.$$array[i];
            for (let j = 0; j < cmPolBuffer.length; j++) {
                ctx.cm1_n.set(Fr.e(cmPolBuffer[j]), (i + j * fflonkInfo.mapSectionsN.cm1_n) * n8r);
            }
        }

        // STEP 0.3 - Prepare public inputs
        ctx.publics = [];
        for (let i = 0; i < fflonkInfo.publics.length; i++) {
            const publicPol = fflonkInfo.publics[i];

            if ("cmP" === publicPol.polType) {
                //const offset = publicPol.polId * sDomain + publicPol.idx * n8r;
                const offset = (fflonkInfo.publics[i].idx * fflonkInfo.mapSectionsN.cm1_n + fflonkInfo.publics[i].polId) * n8r;
                ctx.publics[i] = ctx.cm1_n.slice(offset, offset + n8r);
            } else if ("imP" === publicPol.polType) {
                ctx.publics[i] = calculateExpAtPoint(ctx, fflonkInfo.publicsCode[i], publicPol.idx);
            } else {
                throw new Error(`Invalid public type: ${polType.type}`);
            }
        }
    }

    void PilFflonkProver::stage1()
    {
    }

    void PilFflonkProver::stage2()
    {
    }

    void PilFflonkProver::stage3()
    {
    }

    void PilFflonkProver::stage4()
    {
    }
}