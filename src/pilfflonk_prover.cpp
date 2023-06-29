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

    PilFflonkProver::PilFflonkProver(AltBn128::Engine &_E, std::string fflonkInfoFile) : E(_E)
    {
        fflonkInfo = new FflonkInfo(_E, fflonkInfoFile);
        initialize(NULL);
    }

    PilFflonkProver::PilFflonkProver(AltBn128::Engine &_E, std::string fflonkInfoFile, void* reservedMemoryPtr, uint64_t reservedMemorySize) : E(_E)
    {
        fflonkInfo = new FflonkInfo(_E, fflonkInfoFile);
        initialize(reservedMemoryPtr, reservedMemorySize);
    }

    PilFflonkProver::~PilFflonkProver() {
        this->removePrecomputedData();

        delete transcript;
        delete proof;
    }

    void PilFflonkProver::removePrecomputedData() {
        // DELETE RESERVED MEMORY (if necessary)
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

        delete shPlonkProver;
    }

    void PilFflonkProver::setZkey(BinFileUtils::BinFile *fdZkey, BinFileUtils::BinFile *fdCnstPols) {
        // TODO check constant polynomials size match with definitions
        // TODO check committed polynomials size match with definitions

        try
        {
            if(NULL != zkey) {
                removePrecomputedData();
            }

            cout << "> Creating shPlonk prover" << endl;

            shPlonkProver = new ShPlonk::ShPlonkProver(AltBn128::Engine::engine, fdZkey);

            cout << "> Reading zkey file" << endl;

            if (Zkey::getProtocolIdFromZkey(fdZkey) != Zkey::PILFFLONK_PROTOCOL_ID)
            {
                throw std::invalid_argument("zkey file is not pilfflonk");
            }

            zkey = PilFflonkZkey::loadPilFflonkZkey(fdZkey);

            cout << "> Reading constant polynomials file" << endl;

            cnstPols = loadPolynomialsFromBinFile(fdCnstPols);

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

            // //////////////////////////////////////////////////
            // CONSTANT BIG BUFFER
            // //////////////////////////////////////////////////
            lengthBuffer = 0;

            u_int32_t maxFiDegree = 0;
            for (auto const& [key, f] : zkey->f) maxFiDegree = max(maxFiDegree, f->degree);
            maxFiDegree += 1;

            // Polynomial evaluations
            lengthBuffer += N * fflonkInfo->nConstants;                              // const_n     >> Constant polynomials evaluations
            lengthBuffer += N * factorZK * fflonkInfo->nConstants;                   // const_coefs >> Constant polynomials coefficients
            lengthBuffer += NExt * factorZK * fflonkInfo->nConstants;                // const_2ns   >> Constant polynomials extended evaluations
            lengthBuffer += N * factorZK * fflonkInfo->mapSectionsN.section[cm1_n];    // cm1_coefs   >> Stage1 polynomials coefficients
            lengthBuffer += N * factorZK * fflonkInfo->mapSectionsN.section[cm2_n];    // cm2_coefs   >> Stage2 polynomials coefficients
            lengthBuffer += N * factorZK * fflonkInfo->mapSectionsN.section[cm3_n];    // cm3_coefs   >> Stage3 polynomials coefficients
            lengthBuffer += maxFiDegree * sizeof(G1PointAffine) / sizeof(FrElement); // PTau buf

            bBuffer = new FrElement[lengthBuffer];

            ptr["const_n"]     = &bBuffer[0];
            ptr["const_coefs"] = ptr["const_n"]     + N * fflonkInfo->nConstants;
            ptr["const_2ns"]   = ptr["const_coefs"] + N * factorZK * fflonkInfo->nConstants;
            
            ptr["x_n"]       = ptr["const_2ns"]     + NExt * factorZK * fflonkInfo->nConstants;
            ptr["x_2ns"]     = ptr["x_n"]       + N;
            
            ptr["cm1_coefs"] = ptr["x_2ns"]     + NExt * factorZK;
            ptr["cm2_coefs"] = ptr["cm1_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.section[cm1_n];
            ptr["cm3_coefs"] = ptr["cm2_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.section[cm2_n];

            ptr["publics"]   = ptr["cm3_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.section[cm3_n];
            
            PTau = (G1PointAffine *)(ptr["publics"] + fflonkInfo->nPublics);;

            // //////////////////////////////////////////////////
            // BIG BUFFER
            // //////////////////////////////////////////////////
            lengthBufferCommitted = 0;

            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[cm1_n];               // cm1_n       >> Stage1, committed polynomials evaluations
            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[cm2_n];               // cm2_n       >> Stage2, H1&H2 polynomials evaluations
            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[cm3_n];               // cm3_n       >> Stage3, Z polynomial evaluations
            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[tmpExp_n]; // tmpExp_n    >> Temporal expressions polynomials evaluations
            lengthBufferCommitted += N;                                                // x_n         >> 

            lengthBufferCommitted += NExt * factorZK * fflonkInfo->mapSectionsN.section[cm1_n]; // cm1_2ns     >> Stage1, committed polynomials extended evaluations
            lengthBufferCommitted += NExt * factorZK * fflonkInfo->mapSectionsN.section[cm2_n]; // cm2_2ns     >> Stage2, H1&H2 polynomials extended evaluations
            lengthBufferCommitted += NExt * factorZK * fflonkInfo->mapSectionsN.section[cm3_n]; // cm3_2ns     >> Stage3, Z polynomial extended evaluations
            lengthBufferCommitted += NExt * factorZK * fflonkInfo->qDim;               // q_2ns       >> Stage4, Q polynomial extended evaluations
            lengthBufferCommitted += NExt * factorZK;                                  // x_2ns       >>

            lengthBufferCommitted += fflonkInfo->nPublics;

            bBufferCommitted = new FrElement[lengthBufferCommitted];

            ptrCommitted["cm1_n"]     = &bBufferCommitted[0];
            ptrCommitted["cm2_n"]     = ptrCommitted["cm1_n"]     + N * fflonkInfo->mapSectionsN.section[cm1_n];
            ptrCommitted["cm3_n"]     = ptrCommitted["cm2_n"]     + N * fflonkInfo->mapSectionsN.section[cm2_n];
            ptrCommitted["tmpExp_n"]  = ptrCommitted["cm3_n"]     + N * fflonkInfo->mapSectionsN.section[cm3_n];            
            ptrCommitted["cm1_2ns"]   = ptrCommitted["tmpExp_n"]  + N  * fflonkInfo->mapSectionsN.section[tmpExp_n];
            ptrCommitted["cm2_2ns"]   = ptrCommitted["cm1_2ns"]   + NExt * factorZK * fflonkInfo->mapSectionsN.section[cm1_n];
            ptrCommitted["cm3_2ns"]   = ptrCommitted["cm2_2ns"]   + NExt * factorZK * fflonkInfo->mapSectionsN.section[cm2_n];
            ptrCommitted["q_2ns"]     = ptrCommitted["cm3_2ns"]   + NExt * factorZK * fflonkInfo->mapSectionsN.section[cm3_n];

            int nThreads = omp_get_max_threads() / 2;

            u_int32_t lenPTau = maxFiDegree * sizeof(G1PointAffine);

            ThreadUtils::parset(PTau, 0, lenPTau, nThreads);

            ThreadUtils::parcpy(PTau,
                                (G1PointAffine *)(fdZkey->getSectionData(PilFflonkZkey::ZKEY_PF_PTAU_SECTION)),
                                lenPTau, nThreads);


            transcript = new Keccak256Transcript(E);
            proof = new SnarkProof(E, "pilfflonk");

            pConstPols = new ConstantPolsFflonk(ptr["const_n"], N, fflonkInfo->nConstants);
            pConstPols2ns = new ConstantPolsFflonk(ptr["const_2ns"], NExt * factorZK, fflonkInfo->nConstants);

        }

        catch (const std::exception &e)
        {
            std::cerr << "EXCEPTION: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    /*std::tuple <json, json>*/ void PilFflonkProver::prove(BinFileUtils::BinFile *fdZkey, BinFileUtils::BinFile *fdCnstPols, BinFileUtils::BinFile* fdCmtdPols) {
        this->setZkey(fdZkey, fdCnstPols);

        cout << "> Reading committed polynomials file" << endl;

        cmtdPols = loadPolynomialsFromBinFile(fdCmtdPols);

        return this->prove(fdCmtdPols);
    }

    /*std::tuple<json, json>*/ void PilFflonkProver::prove(BinFileUtils::BinFile* fdCmtdPols)
    {
        if(NULL == zkey) {
            throw std::runtime_error("Zkey data not set");
        }

        try
        {   
           
            cout << "PIL-FFLONK PROVER STARTED" << endl << endl;

            // Initialize vars
            // params = {
            //     pols : &bBufferCommitted,
            //     pConstPols :  pConstPols,
            //     pConstPols2ns :  pConstPols2ns,
            //     challenges : challenges,
            //     x_n : ptr["x_n"],
            //     x_2ns : ptr["x_2ns"],
            //     publicInputs : publicInputs,
            //     q_2ns : ptrCommitted["q_2ns"]
            // }
            
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
            cout << "  Stage 1 pols:    " << fflonkInfo->mapSectionsN.section[cm1_n] << endl;
            cout << "  Stage 2 pols:    " << fflonkInfo->mapSectionsN.section[cm2_n] << endl;
            cout << "  Stage 3 pols:    " << fflonkInfo->mapSectionsN.section[cm3_n] << endl;
            cout << "  Temp exp pols:   " << fflonkInfo->mapSectionsN.section[tmpExp_n]<< endl;
            cout << "-----------------------------" << endl;

            transcript->reset();
            proof->reset();

            // TODO add to precomputed?????
            for (u_int64_t i = 0; i < N; i++) {
                ptr["x_n"][i] = fft->root(nBits, i);
            }

            for (u_int64_t i = 0; i < NExt * factorZK; i++) {
                ptr["x_2ns"][i] = fft->root(nBitsExtZK, i);
            }

            // STAGE 0. Store constants and committed values. Calculate publics
            stage0();

            // STAGE 1. Compute Trace Column Polynomials
            cout << "> STAGE 1. Compute Trace Column Polynomials" << endl;
            stage1();

            // STAGE 2. Compute Inclusion Polynomials
            cout << "> STAGE 2. Compute Inclusion Polynomials" << endl;
            stage2();

            // STAGE 3. Compute Grand Product and Intermediate Polynomials
            cout << "> STAGE 3. Compute Grand Product and Intermediate Polynomials" << endl;
            stage3();

            // STAGE 4. Trace Quotient Polynomials
            cout << "> STAGE 4. Compute Trace Quotient Polynomials" << endl;
            stage4();

            // const [cmts, evaluations, xiSeed] = await open(zkey, PTau, ctx, committedPols, curve, { logger, fflonkPreviousChallenge: ctx.challenges[4], nonCommittedPols: ["Q"] });

            // // Compute xiSeed 
            // FrElement challengeXi = E.fr.exp(xiSeed, zkey.powerW);

            // auto xN = E.fr.exp(challengeXi, N);
            // auto Z = E.fr.sub(xN, E.fr.one());

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
        for (u_int32_t i = 0; i < fflonkInfo->nConstants; i++)
        {
            cout << "··· Preparing '" << cnstPols->names[i] << "' constant polynomial" << endl;

            // Prepare constant polynomial evaluations
            for (u_int64_t j = 0; j < cnstPols->n; j++)
            {
                ptr["const_n"][i + j * fflonkInfo->nConstants] = cnstPols->buffer[i + j * fflonkInfo->nConstants];
            }
        }

        // STEP 0.2 - Prepare committed polynomial evaluations
        for (u_int32_t i = 0; i < cmtdPols->nPols; i++) {
             cout << "··· Preparing '" << cmtdPols->names[i] << "' polynomial" << endl;

             // Prepare committed polynomial evaluations
             for (u_int64_t j = 0; j < cmtdPols->n; j++)
             {
                ptrCommitted["cm1_n"][i + j * fflonkInfo->mapSectionsN.section[cm1_n]] = cmtdPols->buffer[i + j * fflonkInfo->mapSectionsN.section[cm1_n]];
                cout << E.fr.toString(ptrCommitted["cm1_n"][i + j * fflonkInfo->mapSectionsN.section[cm1_n]]) << endl;
             }
        }

        // STEP 0.3 - Prepare public inputs
        for (u_int32_t i = 0; i < fflonkInfo->nPublics; i++)
        {
            //  const publicPol = fflonkInfo->publics[i];

            //  if ("cmP" == publicPol.polType)
            //  {
            //     u_int64_t offset = (fflonkInfo->publics[i].idx * fflonkInfo->mapSectionsN.section[cm1_n] + fflonkInfo->publics[i].polId) * n8r;
            //     ptr["publics"][i] = ptr["cm1_n"][offset];
            //  }
            //  else if ("imP" == publicPol.polType)
            //  {
            //     ptr["publics"][i] = calculateExpAtPoint(ctx, fflonkInfo->publicsCode[i], publicPol.idx);
            //  }
            //  else
            //  {
            //     throw std::runtime_error("Invalid public input type");
            //  }
        }
    }

    void PilFflonkProver::stage1()
    {
        // STEP 1.1 - Compute random challenge
        // TODO Add preprocessed polynomials or public inputs to the transcript ????

        // STEP 1.2 - Compute constant polynomials (coefficients + evaluations) and commit them
        if (fflonkInfo->nConstants > 0)
        {
            extend(0, fflonkInfo->nConstants);

            shPlonkProver->commit(0, PTau, false);
        }

        // STEP 1.3 - Compute commit polynomials (coefficients + evaluations) and commit them
        if (!fflonkInfo->mapSectionsN.section[cm1_n])
             return;

        extend(1, fflonkInfo->mapSectionsN.section[cm1_n]);

        // STEP 1.4 - Commit stage 1 polynomials
        shPlonkProver->commit(1, PTau, true);
    }

    void PilFflonkProver::stage2()
    {
        // STEP 2.1 - Compute random challenges
        cout << "> Computing challenges alpha and beta" << endl;

        // auto cnstCommitPols = Object.keys(zkey).filter(k => k.match(/^f\d/));
        // for (let i = 0; i < cnstCommitPols.length; ++i) {
        //     transcript.addPolCommitment(zkey[cnstCommitPols[i]]);
        //     committedPols[`${cnstCommitPols[i]}`].commit = zkey[cnstCommitPols[i]];
        // }

        for(auto const&[key, commit] : zkey->committedConstants) {
            G1Point C;
            E.g1.copy(C, *((G1PointAffine *)commit));
            transcript->addPolCommitment(C);
        }

        // Add all the publics to the transcript
        for (u_int32_t i = 0; i < fflonkInfo->nPublics; i++)
        {
             transcript->addScalar(ptr["publics"][i]);
        }

        if (0 == transcript->nElements()) {
            transcript->addScalar(E.fr.one());
        }

        // Compute challenge alpha
        challenges[0] = transcript->getChallenge();
        cout << "··· challenges.alpha: " << E.fr.toString(challenges[0]) << endl;

        // Compute challenge beta
        transcript->reset();
        transcript->addScalar(challenges[0]);
        challenges[1] = transcript->getChallenge();
        cout << "··· challenges.beta: " << E.fr.toString(challenges[1]) << endl;

        if (!fflonkInfo->mapSectionsN.section[cm2_n]) return;

            // STEP 2.2 - Compute stage 2 polynomials --> h1, h2
            #pragma omp parallel for
            for (uint64_t i = 0; i < N * factorZK; i++)
            {
                // steps->step2prev_first(E, params, i);
            }

            auto nCm2 = fflonkInfo->mapSectionsN.section[cm1_n];

            for (uint64_t i = 0; i < fflonkInfo->puCtx.size(); i++)
            {
                Polinomial fPol = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].fExpId)]);
                Polinomial tPol = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].tExpId)]);
                Polinomial h1 = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->cm_n[nCm2 + 2*i]);
                Polinomial h2 = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->cm_n[nCm2 + 2*i + 1]);

                // Polinomial::calculateH1H2(h1, h2, fPol, tPol);
            }

            extend(2, fflonkInfo->mapSectionsN.section[cm2_n]);

            // STEP 2.3 - Commit stage 2 polynomials
            shPlonkProver->commit(2, PTau, true);
    }

    void PilFflonkProver::stage3()
    {
        // STEP 3.1 - Compute random challenges
        transcript->reset();

        cout << "> Computing challenges gamma and delta" << endl;

        // Compute challenge gamma
        transcript->addScalar(challenges[1]);

        challenges[2] = transcript->getChallenge();
        cout << "··· challenges.gamma: " << E.fr.toString(challenges[2]) << endl;

        // Compute challenge delta
        transcript->reset();
        transcript->addScalar(challenges[2]);
        challenges[3] = transcript->getChallenge();
        cout << "··· challenges.delta: " << E.fr.toString(challenges[3]) << endl;

        if (!fflonkInfo->mapSectionsN.section[cm3_n]) return;

        // STEP 3.2 - Compute stage 3 polynomials --> Plookup Z, Permutations Z & ConnectionZ polynomials
        auto nPlookups = fflonkInfo->puCtx.size();
        auto nPermutations = fflonkInfo->peCtx.size();
        auto nConnections = fflonkInfo->ciCtx.size();

        #pragma omp parallel for
        for (uint64_t i = 0; i < N; i++)
        {
            // steps->step3prev_first(E, params, i);
        }   

        auto nCm3 = fflonkInfo->mapSectionsN.section[cm1_n] + fflonkInfo->mapSectionsN.section[cm2_n];


        for (uint64_t i = 0; i < nPlookups; i++)
        {
            cout << "··· Calculating z for plookup check " << i << endl;
            Polinomial pNum = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].numId)]);
            Polinomial pDen = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].denId)]);
            Polinomial z = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->cm_n[nCm3 + i]);
            Polinomial::calculateZ(E, z, pNum, pDen);
        }

        for (uint64_t i = 0; i < nPermutations; i++)
        {
            cout << "··· Calculating z for permutation check " << i << endl;
            Polinomial pNum = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->peCtx[i].numId)]);
            Polinomial pDen = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->peCtx[i].denId)]);
            Polinomial z = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->cm_n[nCm3 + nPlookups + i]);
            Polinomial::calculateZ(E, z, pNum, pDen);
        }

        for (uint64_t i = 0; i < nConnections; i++)
        {
            cout << "··· Calculating z for connection check " << i << endl;
            Polinomial pNum = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->ciCtx[i].numId)]);
            Polinomial pDen = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->exp2pol[to_string(fflonkInfo->ciCtx[i].denId)]);
            Polinomial z = fflonkInfo->getPolinomial(bBufferCommitted, fflonkInfo->cm_n[nCm3 + nPlookups + nPermutations + i]);
            Polinomial::calculateZ(E, z, pNum, pDen);
        }

        #pragma omp parallel for
        for (uint64_t i = 0; i < N * factorZK; i++)
        {
            // steps->step3_first(E, params, i);
        }

        extend(3, fflonkInfo->mapSectionsN.section[cm3_n]);
    }

    void PilFflonkProver::stage4()
    {
        // STEP 4.1 - Compute random challenges
        transcript->reset();

        cout << "> Computing challenge a" << endl;

        // Compute challenge a
        transcript->addScalar(challenges[3]);

        challenges[4] = transcript->getChallenge();
        cout << "··· challenges.a: " << E.fr.toString(challenges[4]) << endl;

        // STEP 4.2 - Compute stage 4 polynomial --> Q polynomial
        #pragma omp parallel for
        for (uint64_t i = 0; i < NExt * factorZK; i++)
        {
            // steps->step42ns_first(E, params, i);
        }

        shPlonkProver->polynomialsShPlonk["Q"] = Polynomial<AltBn128::Engine>::fromEvaluations(E, fft, ptrCommitted["q_2ns"], fflonkInfo->qDim * NExt * factorZK);
        shPlonkProver->polynomialsShPlonk["Q"]->divZh(N, 1 << extendBitsTotal);
    }

    void PilFflonkProver::extend(u_int32_t stage, u_int32_t nPols) {
        
        AltBn128::FrElement *buffSrc = stage == 0 ? ptr["const_n"] : ptrCommitted["cm" + std::to_string(stage) + "_n"];
        AltBn128::FrElement *buffDst = stage == 0 ? ptr["const_2ns"] : ptrCommitted["cm" + std::to_string(stage) + "_2ns"];
        AltBn128::FrElement *buffCoefs = stage == 0 ? ptr["const_coefs"] : ptr["cm" + std::to_string(stage) + "_coefs"];

        FrElement* buffer = stage == 0 ? bBuffer : bBufferCommitted;

        NTT_AltBn128 ntt(E, N);
        ntt.INTT(buffCoefs, buffSrc, nBits, nPols, buffer);

        if(stage != 0) {
            for(u_int32_t i = 0; i < nPols; i++) {
                std::string name = (*zkey->polsNamesStage[stage])[i];
                for(u_int32_t j = 0; j < zkey->polsOpenings[name]; ++j) {
                    AltBn128::FrElement b;
                    randombytes_buf((void *)&(b), sizeof(FrElement)-1);

                    buffCoefs[j * nPols + i] = E.fr.add(buffCoefs[j * nPols + i], E.fr.neg(b));
                    buffCoefs[(j + N) * nPols + i] = E.fr.add(buffCoefs[(j + N) * nPols + i], b);
                } 
            }
        }

        // Store coefs to context
        for(u_int32_t i = 0; i < nPols; i++) {
            //     const coefs = getPolFromBuffer(buffCoefs, nPols, N * factorZK, i, Fr);
            //     ctx[zkey.polsNamesStage[stage][i]] = new Polynomial(coefs, ctx.curve, logger);
        }

        NTT_AltBn128 nttExtended(E, 1 << nBitsExtZK);
        nttExtended.NTT(buffDst, buffCoefs, nBitsExtZK, nPols, buffer);
    }

    // function getPolFromBuffer(buff, nPols, N, id, Fr) {
    //     let polBuffer = new BigBuffer(N * Fr.n8);
    //     for (let j = 0; j < N; j++) {
    //         polBuffer.set(buff.slice((id + j * nPols) * Fr.n8, (id + j * nPols + 1) * Fr.n8), j * Fr.n8);
    //     }
    //     return polBuffer
    // }


    BinFilePolsData* PilFflonkProver::loadPolynomialsFromBinFile(BinFileUtils::BinFile *fd)
    {
        BinFilePolsData* polsData = new BinFilePolsData();

        // Read section 2 -> polynomials names
        fd->startReadSection(2);

        auto len = fd->readU32LE();
        polsData->nPols = len;
        polsData->names = new string[len];

        for(u_int32_t i = 0; i < len; i++)
        {
            polsData->names[i] = fd->readString();
        }

        fd->endReadSection();

        // Read section 3 -> polynomials data
        auto lenSection = fd->getSectionSize(3);
        auto lenArray =  (u_int64_t)(lenSection / sizeof(FrElement));
        if(lenSection != lenArray * sizeof(FrElement))
        {
            throw std::runtime_error("Invalid section size");
        }

        polsData->n = lenArray;
        polsData->buffer = new FrElement[lenArray];

        int nThreads = max(omp_get_max_threads() / 2, 128 * 128);

        ThreadUtils::parcpy(polsData->buffer, (FrElement *)fd->getSectionData(3), lenSection, nThreads);

        // u_int8_t* data = (uint8_t *)&polsData->buffer[0];
        // for(int i =0;i< lenSection;i++) {
        //     cout << *(data + 48) << " " << endl;;
        // }
        // cout << endl;
        
//         u_int64_t bytes = 0;
//         u_int8_t data2[32];


//         FrElement element;
//         for(u_int64_t i = 0; i < lenArray; i++)
//         {
//             element = std::any_cast<FrElement>(polsData->buffer[i]);

// //            cout << E.fr.toString(element) << endl;
//             E.fr.toRprBE(element, data2, E.fr.bytes());
//             cout << E.fr.toString(element) << endl;
//         }

        return polsData;
    }
}
