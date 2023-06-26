#include "pilfflonk_prover.hpp"
#include "zkey.hpp"
#include <math.h>


namespace PilFflonk
{
    void PilFflonkProver::initialize(void* reservedMemoryPtr, uint64_t reservedMemorySize)
    {
        zkey = NULL;
        this->reservedMemoryPtr = (FrElement *)reservedMemoryPtr;
        this->reservedMemorySize = reservedMemorySize;

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

        // delete transcript;
        //delete proof;
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

            zkey = PilFflonkZkey::loadPilFflonkZkey(zkeyBinfile);

            ctx.N = 1 << zkey->power;
            ctx.nBits = zkey->power;
            //ctx.extendBits = Math.ceil(fflonkInfo.qDeg + 1);
            ctx.nBitsExt = zkey->power + ctx.extendBits;
            ctx.Next = 1 << ctx.nBitsExt;
    
            cout << "> Starting fft" << endl;

            domainSize = ctx.N;
            fft = new FFT<AltBn128::Engine::Fr>(domainSize * 4);

            mpz_t altBbn128r;
            mpz_init(altBbn128r);
            mpz_set_str(altBbn128r, "21888242871839275222246405745257275088548364400416034343698204186575808495617", 10);

            if (mpz_cmp(zkey->rPrime, altBbn128r) != 0)
            {
                throw std::invalid_argument("zkey curve not supported");
            }

            sDomain = domainSize * sizeof(FrElement);

            ////////////////////////////////////////////////////
            // PRECOMPUTED BIG BUFFER
            ////////////////////////////////////////////////////
            // Precomputed 1 > polynomials buffer
            // uint64_t lengthPrecomputedBigBuffer = 0;
            // lengthPrecomputedBigBuffer += zkey->domainSize * 1 * 8; // Polynomials QL, QR, QM, QO, QC, Sigma1, Sigma2 & Sigma3
            // lengthPrecomputedBigBuffer += zkey->domainSize * 8 * 1; // Polynomial  C0
            // // Precomputed 2 > evaluations buffer
            // lengthPrecomputedBigBuffer += zkey->domainSize * 4 * 8; // Evaluations QL, QR, QM, QO, QC, Sigma1, Sigma2, Sigma3
            // lengthPrecomputedBigBuffer += zkey->domainSize * 4 * zkey->nPublic; // Evaluations Lagrange1
            // // Precomputed 3 > ptau buffer
            // lengthPrecomputedBigBuffer += zkey->domainSize * 9 * sizeof(G1PointAffine) / sizeof(FrElement); // PTau buffer

            // precomputedBigBuffer = new FrElement[lengthPrecomputedBigBuffer];

            // polPtr["Sigma1"] = &precomputedBigBuffer[0];
            // polPtr["Sigma2"] = polPtr["Sigma1"] + zkey->domainSize;
            // polPtr["Sigma3"] = polPtr["Sigma2"] + zkey->domainSize;
            // polPtr["QL"]     = polPtr["Sigma3"] + zkey->domainSize;
            // polPtr["QR"]     = polPtr["QL"] + zkey->domainSize;
            // polPtr["QM"]     = polPtr["QR"] + zkey->domainSize;
            // polPtr["QO"]     = polPtr["QM"] + zkey->domainSize;
            // polPtr["QC"]     = polPtr["QO"] + zkey->domainSize;
            // polPtr["C0"]     = polPtr["QC"] + zkey->domainSize;

            // evalPtr["Sigma1"] = polPtr["C0"] + zkey->domainSize * 8;
            // evalPtr["Sigma2"] = evalPtr["Sigma1"] + zkey->domainSize * 4;
            // evalPtr["Sigma3"] = evalPtr["Sigma2"] + zkey->domainSize * 4;
            // evalPtr["QL"]     = evalPtr["Sigma3"] + zkey->domainSize * 4;
            // evalPtr["QR"]     = evalPtr["QL"] + zkey->domainSize * 4;
            // evalPtr["QM"]     = evalPtr["QR"] + zkey->domainSize * 4;
            // evalPtr["QO"]     = evalPtr["QM"] + zkey->domainSize * 4;
            // evalPtr["QC"]     = evalPtr["QO"] + zkey->domainSize * 4;
            // evalPtr["lagrange"] = evalPtr["QC"] + zkey->domainSize * 4;

            // PTau = (G1PointAffine *)(evalPtr["lagrange"] + zkey->domainSize * 4 * zkey->nPublic);

            // // Read Q selectors polynomials and evaluations
            // LOG_TRACE("... Loading QL, QR, QM, QO, & QC polynomial coefficients and evaluations");

            // // Reserve memory for Q's polynomials
            // polynomials["QL"] = new Polynomial<Engine>(E, polPtr["QL"], zkey->domainSize);
            // polynomials["QR"] = new Polynomial<Engine>(E, polPtr["QR"], zkey->domainSize);
            // polynomials["QM"] = new Polynomial<Engine>(E, polPtr["QM"], zkey->domainSize);
            // polynomials["QO"] = new Polynomial<Engine>(E, polPtr["QO"], zkey->domainSize);
            // polynomials["QC"] = new Polynomial<Engine>(E, polPtr["QC"], zkey->domainSize);

            // int nThreads = omp_get_max_threads() / 2;

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

            // ThreadUtils::parset(PTau, 0, sizeof(G1PointAffine) * zkey->domainSize * 9, nThreads);

            // // domainSize * 9 = SRS length in the zkey saved in setup process.
            // // it corresponds to the maximum SRS length needed, specifically to commit C2
            // ThreadUtils::parcpy(this->PTau,
            //                     (G1PointAffine *)fdZkey->getSectionData(Zkey::ZKEY_FF_PTAU_SECTION),
            //                     (zkey->domainSize * 9) * sizeof(G1PointAffine), nThreads);

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

            // transcript = new Keccak256Transcript<Engine>(E);
            //proof = new SnarkProof(E, "fflonk");

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
//             LOG_TRACE("FFLONK PROVER STARTED");

//             this->buffWitness = buffWitness;

//             if(NULL != wtnsHeader) {
//                 if (mpz_cmp(zkey->rPrime, wtnsHeader->prime) != 0)
//                 {
//                     throw std::invalid_argument("Curve of the witness does not match the curve of the proving key");
//                 }

//                 if (wtnsHeader->nVars != zkey->nVars - zkey->nAdditions)
//                 {
//                     std::ostringstream ss;
//                     ss << "Invalid witness length. Circuit: " << zkey->nVars << ", witness: " << wtnsHeader->nVars << ", "
//                     << zkey->nAdditions;
//                     throw std::invalid_argument(ss.str());
//                 }
//             }

//             std::ostringstream ss;
//             LOG_TRACE("----------------------------");
//             LOG_TRACE("  FFLONK PROVE SETTINGS");
//             ss.str("");
//             ss << "  Curve:         " << curveName;
//             LOG_TRACE(ss);
//             ss.str("");
//             ss << "  Circuit power: " << zkeyPower;
//             LOG_TRACE(ss);
//             ss.str("");
//             ss << "  Domain size:   " << zkey->domainSize;
//             LOG_TRACE(ss);
//             ss.str("");
//             ss << "  Vars:          " << zkey->nVars;
//             LOG_TRACE(ss);
//             ss.str("");
//             ss << "  Public vars:   " << zkey->nPublic;
//             LOG_TRACE(ss);
//             ss.str("");
//             ss << "  Constraints:   " << zkey->nConstraints;
//             LOG_TRACE(ss);
//             ss.str("");
//             ss << "  Additions:     " << zkey->nAdditions;
//             LOG_TRACE(ss);
//             LOG_TRACE("----------------------------");

// // cout << endl << "PIL-FFLONK PROVER STARTED" << endl;

//     // const int protocolId = Zkey::getProtocolIdFromZkey(zkey.get());

//     // cout << "-----------------------------" << endl;
//     // cout << "  PIL-FFLONK PROVE SETTINGS" << endl;
//     // // cout << "  Curve:         ${curve.name}" << endl;
//     // //cout << "  Domain size:   ${domainSize} (2^${power})" << endl;
//     // //cout << "  Extended Bits:   ${ctx.extendBits}" << endl;
//     // // cout << "  Domain size ext: ${domainSizeExt} (2^${power + ctx.extendBits})" << endl;
//     // // cout << "  Const  pols:   ${fflonkInfo.nConstants}" << endl;
//     // // cout << "  Stage 1 pols:   ${fflonkInfo.mapSectionsN.cm1_n}" << endl;
//     // // cout << "  Stage 2 pols:   ${fflonkInfo.mapSectionsN.cm2_n}" << endl;
//     // // cout << "  Stage 3 pols:   ${fflonkInfo.mapSectionsN.cm3_n}" << endl;
//     // // cout << "  Temp exp pols: ${fflonkInfo.mapSectionsN.tmpExp_n}" << endl;
//     // cout << "-----------------------------" << endl;



//             transcript->reset();
//             proof->reset();

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
}