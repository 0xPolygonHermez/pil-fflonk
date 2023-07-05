#include "pilfflonk_prover.hpp"
#include "timer.hpp"
#include <stdio.h>
#include "zkey.hpp"
#include <math.h>
#include "const_pols_serializer.hpp"

namespace PilFflonk
{
    PilFflonkProver::PilFflonkProver(AltBn128::Engine &_E,
                                     std::string zkeyFilename, std::string fflonkInfoFilename,
                                     std::string constPolsFilename, std::string precomputedFilename,
                                     void *reservedMemoryPtr, uint64_t reservedMemorySize) : E(_E)
    {
        this->reservedMemoryPtr = (FrElement *)reservedMemoryPtr;
        this->reservedMemorySize = reservedMemorySize;

        curveName = "bn128";

        setConstantData(zkeyFilename, fflonkInfoFilename, constPolsFilename, precomputedFilename);
    }

    PilFflonkProver::~PilFflonkProver()
    {
        delete[] bBufferConstant;
        ptrConstant.clear();

        if (NULL == reservedMemoryPtr) delete[] bBufferCommitted;
        ptrCommitted.clear();

        delete[] bBufferShPlonk;
        ptrShPlonk.clear();

        delete fft;
        delete zkey;
        delete transcript;
        delete shPlonkProver;
        delete fflonkInfo;
    }

    void PilFflonkProver::setConstantData(std::string zkeyFilename, std::string fflonkInfoFile,
                                          std::string constPolsFilename, std::string precomputedFilename)
    {
        try
        {
            TimerStart(LOAD_ZKEY_TO_MEMORY);

            zklog.info("> Opening zkey data file");
            auto zkeyBinFile = BinFileUtils::openExisting(zkeyFilename, "zkey", 1);
            auto fdZkey = zkeyBinFile.get();

            zklog.info("> Opening precomputed data file");
            auto precomputedBinFile = BinFileUtils::openExisting(precomputedFilename, "pols", 1);
            auto fdPrecomputed = precomputedBinFile.get();

            if (Zkey::getProtocolIdFromZkey(fdZkey) != Zkey::PILFFLONK_PROTOCOL_ID)
            {
                throw std::invalid_argument("zkey file is not pilfflonk");
            }

            zkey = PilFflonkZkey::loadPilFflonkZkey(fdZkey);

            fflonkInfo = new FflonkInfo(E, fflonkInfoFile);

            shPlonkProver = new ShPlonk::ShPlonkProver(AltBn128::Engine::engine, zkey);

            n8r = sizeof(FrElement);
            N = 1 << zkey->power;
            nBits = zkey->power;
            extendBits = ceil(log2(fflonkInfo->qDeg + 1));
            nBitsExt = nBits + extendBits;
            NExt = 1 << nBitsExt;
            sDomain = N * n8r;
            sDomainExt = NExt * n8r;

            // ZK data
            extendBitsZK = zkey->powerZK - zkey->power;
            factorZK = (1 << extendBitsZK);
            extendBitsTotal = extendBits + extendBitsZK;
            nBitsExtZK = nBits + extendBitsTotal;

            fft = new FFT<AltBn128::Engine::Fr>(NExt * factorZK);
            
            transcript = new Keccak256Transcript(E);

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
            for (auto const &[key, f] : zkey->f)
                maxFiDegree = max(maxFiDegree, f->degree);
            maxFiDegree += 1;

            // Polynomial evaluations
            lengthBuffer += N * fflonkInfo->nConstants;                              // const_n     >> Constant polynomials evaluations
            lengthBuffer += NExt * factorZK * fflonkInfo->nConstants;                // const_2ns   >> Constant polynomials extended evaluations
            lengthBuffer += N * factorZK * fflonkInfo->nConstants;                   // const_coefs >> Constant polynomials coefficients
            lengthBuffer += N;                                                       // x_n
            lengthBuffer += NExt * factorZK;                                         // x_2ns

            lengthBuffer += maxFiDegree * sizeof(G1PointAffine) / sizeof(FrElement); // PTau buffer

            bBufferConstant = new FrElement[lengthBuffer];

            ptrConstant["const_n"] = &bBufferConstant[0];
            ptrConstant["const_2ns"] = ptrConstant["const_n"] + N * fflonkInfo->nConstants;

            ptrConstant["x_n"] = ptrConstant["const_2ns"] + NExt * factorZK * fflonkInfo->nConstants;
            ptrConstant["x_2ns"] = ptrConstant["x_n"] + N;

            ptrConstant["const_coefs"] = ptrConstant["x_2ns"] + NExt * factorZK;

            PTau = (G1PointAffine *)(ptrConstant["const_coefs"] + N * factorZK * fflonkInfo->nConstants);

            // //////////////////////////////////////////////////
            // SHPLONK BIG BUFFER
            // //////////////////////////////////////////////////

            u_int32_t nEvals = shPlonkProver->getEvaluations();

            lengthBufferShPlonk = 0;
            lengthBufferShPlonk += nEvals * N * factorZK;

            bBufferShPlonk = new FrElement[lengthBufferShPlonk];

            std::string* evals = shPlonkProver->getEvaluationsNames();
            for(auto i = 0; i < nEvals; ++i) {
                cout << evals[i] << endl;
                // if(i == 0) {
                //     ptrShPlonk[evals[i]] = &bBufferShPlonk[0];
                // } else {
                //     ptrShPlonk[evals[i]] = ptrShPlonk[evals[i-1]] + N * factorZK;
                // }
            }

            // //////////////////////////////////////////////////
            // BIG BUFFER
            // //////////////////////////////////////////////////
            lengthBufferCommitted = 0;

            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[cm1_n];    // cm1_n       >> Stage1, committed polynomials evaluations
            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[cm2_n];    // cm2_n       >> Stage2, H1&H2 polynomials evaluations
            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[cm3_n];    // cm3_n       >> Stage3, Z polynomial evaluations
            lengthBufferCommitted += N * fflonkInfo->mapSectionsN.section[tmpExp_n]; // tmpExp_n    >> Temporal expressions polynomials evaluations

            lengthBufferCommitted += NExt * factorZK * fflonkInfo->mapSectionsN.section[cm1_2ns]; // cm1_2ns     >> Stage1, committed polynomials extended evaluations
            lengthBufferCommitted += NExt * factorZK * fflonkInfo->mapSectionsN.section[cm2_2ns]; // cm2_2ns     >> Stage2, H1&H2 polynomials extended evaluations
            lengthBufferCommitted += NExt * factorZK * fflonkInfo->mapSectionsN.section[cm3_2ns]; // cm3_2ns     >> Stage3, Z polynomial extended evaluations
            lengthBufferCommitted += NExt * factorZK * fflonkInfo->qDim;                          // q_2ns       >> Stage4, Q polynomial extended evaluations
            lengthBufferCommitted += fflonkInfo->nPublics;                                        // publics
            lengthBufferCommitted += N * factorZK * fflonkInfo->mapSectionsN.section[cm1_n];      // cm1_coefs   >> Stage1 polynomials coefficients
            lengthBufferCommitted += N * factorZK * fflonkInfo->mapSectionsN.section[cm2_n];      // cm2_coefs   >> Stage2 polynomials coefficients
            lengthBufferCommitted += N * factorZK * fflonkInfo->mapSectionsN.section[cm3_n];      // cm3_coefs   >> Stage3 polynomials coefficients

            zklog.info("lengthBufferCommitted: " + lengthBufferCommitted);

            if(reservedMemoryPtr == NULL) {
                bBufferCommitted = new FrElement[lengthBufferCommitted];
            } else {
                uint64_t requiredMemorySize = lengthBufferCommitted * sizeof(AltBn128::FrElement);
                if(reservedMemorySize < requiredMemorySize) {
                    uint64_t additionalBytes = requiredMemorySize - reservedMemorySize;
                    string errorMsg = "Insufficient reserved memory size. Additional " + std::to_string(additionalBytes) + " bytes required. Total required " + std::to_string(requiredMemorySize) + " bytes.";
                    zklog.error(errorMsg);

                    throw std::runtime_error(errorMsg);
                }
                bBufferCommitted = reservedMemoryPtr;
            }

            ptrCommitted["cm1_n"] = &bBufferCommitted[0];
            ptrCommitted["cm2_n"] = ptrCommitted["cm1_n"] + N * fflonkInfo->mapSectionsN.section[cm1_n];
            ptrCommitted["cm3_n"] = ptrCommitted["cm2_n"] + N * fflonkInfo->mapSectionsN.section[cm2_n];
            ptrCommitted["tmpExp_n"] = ptrCommitted["cm3_n"] + N * fflonkInfo->mapSectionsN.section[cm3_n];
            ptrCommitted["cm1_2ns"] = ptrCommitted["tmpExp_n"] + N * fflonkInfo->mapSectionsN.section[tmpExp_n];
            ptrCommitted["cm2_2ns"] = ptrCommitted["cm1_2ns"] + NExt * factorZK * fflonkInfo->mapSectionsN.section[cm1_n];
            ptrCommitted["cm3_2ns"] = ptrCommitted["cm2_2ns"] + NExt * factorZK * fflonkInfo->mapSectionsN.section[cm2_n];
            ptrCommitted["q_2ns"] = ptrCommitted["cm3_2ns"] + NExt * factorZK * fflonkInfo->mapSectionsN.section[cm3_n];

            ptrCommitted["publics"] = ptrCommitted["q_2ns"] + NExt * factorZK * fflonkInfo->qDim;
            ptrCommitted["cm1_coefs"] = ptrCommitted["publics"] + fflonkInfo->nPublics;
            ptrCommitted["cm2_coefs"] = ptrCommitted["cm1_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.section[cm1_n];
            ptrCommitted["cm3_coefs"] = ptrCommitted["cm2_coefs"] + N * factorZK * fflonkInfo->mapSectionsN.section[cm2_n];

            int nThreads = omp_get_max_threads() / 2;

            u_int32_t lenPTau = maxFiDegree * sizeof(G1PointAffine);

            ThreadUtils::parset(PTau, 0, lenPTau, nThreads);

            ThreadUtils::parcpy(PTau,
                                (G1PointAffine *)(fdZkey->getSectionData(PilFflonkZkey::ZKEY_PF_PTAU_SECTION)),
                                lenPTau, nThreads);

            TimerStart(LOAD_CONST_POLS_TO_MEMORY);

            u_int64_t constPolsSize = fflonkInfo->nConstants * sizeof(FrElement) * N;

            if (constPolsSize > 0)
            {
                auto pConstPolsAddress = copyFile(constPolsFilename, constPolsSize);
                zklog.info("PilFflonk::PilFflonk() successfully copied " + to_string(constPolsSize) + " bytes from constant file " + constPolsFilename);

#pragma omp parallel for
                for (u_int64_t i = 0; i < fflonkInfo->nConstants * N; i++)
                {
                    E.fr.fromRprLE(ptrConstant["const_n"][i], reinterpret_cast<uint8_t *>(pConstPolsAddress) + i * 32, 32);
                }
            }

            TimerStopAndLog(LOAD_CONST_POLS_TO_MEMORY);

            TimerStart(LOAD_CONST_POLS_ZKEY_TO_MEMORY);

            ConstPolsSerializer::readConstPolsFile(E, fdPrecomputed, ptrConstant["const_coefs"], ptrConstant["const_2ns"], ptrConstant["x_n"], ptrConstant["x_2ns"]);

            TimerStopAndLog(LOAD_CONST_POLS_ZKEY_TO_MEMORY);

            TimerStopAndLog(LOAD_ZKEY_TO_MEMORY);
        }

        catch (const std::exception &e)
        {
            std::cerr << "EXCEPTION: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    std::tuple<json, json> PilFflonkProver::prove(std::string committedPolsFilename)
    {
        if (NULL == zkey)
            throw std::runtime_error("Zkey data not set");

        try
        {
            zklog.info("");
            zklog.info("PIL-FFLONK PROVER STARTED");

            TimerStart(PIL_FFLONK_PROVE);

            // Initialize vars
            StepsParams params = {
                cm1_n : ptrCommitted["cm1_n"],
                cm2_n : ptrCommitted["cm2_n"],
                cm3_n : ptrCommitted["cm3_n"],
                tmpExp_n : ptrCommitted["tmpExp_n"],
                cm1_2ns : ptrCommitted["cm1_2ns"],
                cm2_2ns : ptrCommitted["cm2_2ns"],
                cm3_2ns : ptrCommitted["cm3_2ns"],
                const_n : ptrConstant["const_n"],
                const_2ns : ptrConstant["const_2ns"],
                challenges : challenges,
                x_n : ptrConstant["x_n"],
                x_2ns : ptrConstant["x_2ns"],
                publicInputs : ptrCommitted["publics"],
                q_2ns : ptrCommitted["q_2ns"]
            };

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

            std::ostringstream ss;
            zklog.info("-----------------------------");
            zklog.info("  PIL-FFLONK PROVE SETTINGS");
            zklog.info("  Curve:           " + curveName);
            ss << "  Circuit power:   " << nBits;
            zklog.info(ss.str());
            
            ss.str("");
            ss << "  Domain size:     " << N;
            zklog.info(ss.str());
            ss.str("");
            ss << "  Extended Bits:   " << extendBits;
            zklog.info(ss.str());
            ss.str("");
            ss << "  Domain size ext: " << NExt << "(2^" << nBits + extendBits << ")";
            zklog.info(ss.str());
            ss.str("");
            ss << "  Const  pols:     " << fflonkInfo->nConstants;
            zklog.info(ss.str());
            ss.str("");
            ss << "  Stage 1 pols:    " << fflonkInfo->mapSectionsN.section[cm1_n];
            zklog.info(ss.str());
            ss.str("");
            ss << "  Stage 2 pols:    " << fflonkInfo->mapSectionsN.section[cm2_n];
            zklog.info(ss.str());
            ss.str("");
            ss << "  Stage 3 pols:    " << fflonkInfo->mapSectionsN.section[cm3_n];
            zklog.info(ss.str());
            ss.str("");
            ss << "  Temp exp pols:   " << fflonkInfo->mapSectionsN.section[tmpExp_n];
            zklog.info("-----------------------------");

            transcript->reset();

            TimerStart(LOAD_COMMITTED_POLS_TO_MEMORY);

            u_int64_t cmtdPolsSize = fflonkInfo->mapSectionsN.section[cm1_n] * sizeof(FrElement) * N;

            auto pCommittedPolsAddress = copyFile(committedPolsFilename, cmtdPolsSize);
            zklog.info("PilFflonk::PilFflonk() successfully copied " + to_string(cmtdPolsSize) + " bytes from constant file " + committedPolsFilename);

            TimerStart(CONVERT_COMMITTED_POLS_TO_FR);

#pragma omp parallel for
            for (u_int64_t i = 0; i < fflonkInfo->mapSectionsN.section[cm1_n] * N; i++)
            {
                E.fr.fromRprLE(ptrCommitted["cm1_n"][i], reinterpret_cast<uint8_t *>(pCommittedPolsAddress) + i * 32, 32);
            }

            TimerStopAndLog(CONVERT_COMMITTED_POLS_TO_FR);

            TimerStopAndLog(LOAD_COMMITTED_POLS_TO_MEMORY);

            // STAGE 0. Calculate publics
            // STEP 0.1 - Prepare public inputs

            TimerStart(PIL_FFLONK_CALCULATE_EXPS_PUBLICS);
            json publicSignals(nullptr);
            for (u_int32_t i = 0; i < fflonkInfo->nPublics; i++)
            {
                Publics publicPol = fflonkInfo->publics[i];
                if ("cmP" == publicPol.polType)
                {
                    u_int64_t offset = (fflonkInfo->publics[i].idx * fflonkInfo->mapSectionsN.section[cm1_n] + fflonkInfo->publics[i].polId);
                    ptrCommitted["publics"][i] = ptrCommitted["cm1_n"][offset];
                }
                else if ("imP" == publicPol.polType)
                {
                    ptrCommitted["publics"][i] = pilFflonkSteps.publics_first(E, params, fflonkInfo->publics[i].polId, i);
                }
                else
                {
                    throw std::runtime_error("Invalid public input type");
                }
                publicSignals.push_back(E.fr.toString(ptrCommitted["publics"][i]).c_str());
            }

            TimerStopAndLog(PIL_FFLONK_CALCULATE_EXPS_PUBLICS);

            // STAGE 1. Compute Trace Column Polynomials
            zklog.info("STAGE 1. Compute Trace Column Polynomials");
            stage1(params);

            // STAGE 2. Compute Inclusion Polynomials
            zklog.info("STAGE 2. Compute Inclusion Polynomials");
            stage2(params);

            // STAGE 3. Compute Grand Product and Intermediate Polynomials
            zklog.info("STAGE 3. Compute Grand Product and Intermediate Polynomials");
            stage3(params);

            // STAGE 4. Trace Quotient Polynomials
            zklog.info("STAGE 4. Compute Trace Quotient Polynomials");
            stage4(params);

            json pilFflonkProof = shPlonkProver->open(PTau, challenges[4]);

            FrElement challengeXi = shPlonkProver->getChallengeXi();

            FrElement xN = E.fr.one();
            for (u_int64_t i = 0; i < N; i++)
            {
                xN = E.fr.mul(xN, challengeXi);
            }

            FrElement Z = E.fr.sub(xN, E.fr.one());

            E.fr.inv(Z, Z);
            pilFflonkProof["evaluations"]["invZh"] = E.fr.toString(Z);

            TimerStopAndLog(PIL_FFLONK_PROVE);

            return {pilFflonkProof, publicSignals};
        }
        catch (const std::exception &e)
        {
            std::cerr << "EXCEPTION: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    void PilFflonkProver::stage1(StepsParams &params)
    {
        TimerStart(PIL_FFLONK_STAGE_1);

        TimerStart(PIL_FFLONK_STAGE_1_ADD_CONSTANT_POLS);
        // STEP 1.2 - Compute constant polynomials (coefficients + evaluations) and commit them
        if (fflonkInfo->nConstants > 0)
        {
            addCoefsToContext(0, fflonkInfo->nConstants, ptrConstant["const_coefs"]);

            shPlonkProver->commit(0, PTau, false);
        }
        TimerStopAndLog(PIL_FFLONK_STAGE_1_ADD_CONSTANT_POLS);

        // STEP 1.3 - Compute commit polynomials (coefficients + evaluations) and commit them
        if (fflonkInfo->mapSectionsN.section[cm1_n])
        {
            TimerStart(PIL_FFLONK_STAGE_1_EXTEND);
            extend(1, fflonkInfo->mapSectionsN.section[cm1_n]);
            TimerStopAndLog(PIL_FFLONK_STAGE_1_EXTEND);

            // STEP 1.4 - Commit stage 1 polynomials
            TimerStart(PIL_FFLONK_STAGE_1_COMMIT);
            shPlonkProver->commit(1, PTau, true);
            TimerStopAndLog(PIL_FFLONK_STAGE_1_COMMIT);
        }

        TimerStopAndLog(PIL_FFLONK_STAGE_1);
    }

    void PilFflonkProver::stage2(StepsParams &params)
    {

        TimerStart(PIL_FFLONK_STAGE_2);

        // STEP 2.1 - Compute random challenges
        zklog.info("Computing challenges alpha and beta");
        for (auto const &[key, commit] : zkey->committedConstants)
        {
            G1Point C;
            E.g1.copy(C, *((G1PointAffine *)commit));
            cout << "Commitment " << key << ": " << E.g1.toString(C) << endl;
            transcript->addPolCommitment(C);
        }

        // Add all the publics to the transcript
        for (u_int32_t i = 0; i < fflonkInfo->nPublics; i++)
        {
            transcript->addScalar(ptrCommitted["publics"][i]);
        }

        if (0 == transcript->nElements())
        {
            transcript->addScalar(E.fr.one());
        }

        // Compute challenge alpha
        challenges[0] = transcript->getChallenge();
        zklog.info("Challenge alpha: " + E.fr.toString(challenges[0]));

        // Compute challenge beta
        transcript->reset();
        transcript->addScalar(challenges[0]);
        challenges[1] = transcript->getChallenge();
        zklog.info("Challenge beta: " + E.fr.toString(challenges[1]));

        if (fflonkInfo->mapSectionsN.section[cm2_n])
        {
            // STEP 2.2 - Compute stage 2 polynomials --> h1, h2
            TimerStart(PIL_FFLONK_STAGE_2_CALCULATE_EXPS);
#pragma omp parallel for
            for (uint64_t i = 0; i < N; i++)
            {
                pilFflonkSteps.step2prev_first(E, params, i);
            }
            TimerStopAndLog(PIL_FFLONK_STAGE_2_CALCULATE_EXPS);

            auto nCm2 = fflonkInfo->mapSectionsN.section[cm1_n];

            TimerStart(PIL_FFLONK_STAGE_2_CALCULATE_H1H2);
            for (uint64_t i = 0; i < fflonkInfo->puCtx.size(); i++)
            {
                auto [fName, fIndex, nPols1] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].fExpId)]);
                auto [tName, tIndex, nPols2] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].tExpId)]);

                AltBn128::FrElement *fPol = getPolynomial(ptrCommitted[fName], nPols1, fIndex);
                AltBn128::FrElement *tPol = getPolynomial(ptrCommitted[tName], nPols2, tIndex);

                uint64_t h1Id = find(fflonkInfo->mapSections.section[cm2_n], fflonkInfo->cm_n[nCm2 + 2 * i]);
                uint64_t h2Id = find(fflonkInfo->mapSections.section[cm2_n], fflonkInfo->cm_n[nCm2 + 2 * i + 1]);

                // calculateH1H2(fPol, tPol, h1Id, h2Id);
            }
            TimerStopAndLog(PIL_FFLONK_STAGE_2_CALCULATE_H1H2);

            TimerStart(PIL_FFLONK_STAGE_2_EXTEND);
            extend(2, fflonkInfo->mapSectionsN.section[cm2_n]);
            TimerStopAndLog(PIL_FFLONK_STAGE_2_EXTEND);

            // STEP 2.3 - Commit stage 2 polynomials
            TimerStart(PIL_FFLONK_STAGE_2_COMMIT);
            shPlonkProver->commit(2, PTau, true);
            TimerStopAndLog(PIL_FFLONK_STAGE_2_COMMIT);
        }

        TimerStopAndLog(PIL_FFLONK_STAGE_2);
    }

    void PilFflonkProver::stage3(StepsParams &params)
    {
        TimerStart(PIL_FFLONK_STAGE_3);

        // STEP 3.1 - Compute random challenges
        transcript->reset();

        zklog.info("Computing challenges gamma and delta");

        // Compute challenge gamma
        transcript->addScalar(challenges[1]);

        challenges[2] = transcript->getChallenge();
        zklog.info("Challenge gamma: " + E.fr.toString(challenges[2]));

        // Compute challenge delta
        transcript->reset();
        transcript->addScalar(challenges[2]);
        challenges[3] = transcript->getChallenge();
        zklog.info("Challenge delta: " + E.fr.toString(challenges[3]));

        if (fflonkInfo->mapSectionsN.section[cm3_n])
        {

            // STEP 3.2 - Compute stage 3 polynomials --> Plookup Z, Permutations Z & ConnectionZ polynomials
            auto nPlookups = fflonkInfo->puCtx.size();
            auto nPermutations = fflonkInfo->peCtx.size();
            auto nConnections = fflonkInfo->ciCtx.size();

            TimerStart(PIL_FFLONK_STAGE_3_PREV_CALCULATE_EXPS);
#pragma omp parallel for
            for (uint64_t i = 0; i < N; i++)
            {
                pilFflonkSteps.step3prev_first(E, params, i);
            }
            TimerStopAndLog(PIL_FFLONK_STAGE_3_PREV_CALCULATE_EXPS);

            auto nCm3 = fflonkInfo->mapSectionsN.section[cm1_n] + fflonkInfo->mapSectionsN.section[cm2_n];

            TimerStart(PIL_FFLONK_STAGE_3_CALCULATE_Z);
            for (uint64_t i = 0; i < nPlookups; i++)
            {
                zklog.info("Calculating z for plookup " + to_string(i) + " / " + to_string(nPlookups));
                auto [numName, numIndex, nPols1] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].numId)]);
                auto [denName, denIndex, nPols2] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->puCtx[i].denId)]);

                AltBn128::FrElement *pNum = getPolynomial(ptrCommitted[numName], nPols1, numIndex);
                AltBn128::FrElement *pDen = getPolynomial(ptrCommitted[denName], nPols2, denIndex);

                uint64_t zIndex = find(fflonkInfo->mapSections.section[cm3_n], fflonkInfo->cm_n[nCm3 + i]);
                calculateZ(pNum, pDen, zIndex);
            }

            for (uint64_t i = 0; i < nPermutations; i++)
            {
                zklog.info("Calculating z for permutation " + to_string(i) + " / " + to_string(nPermutations));
                auto [numName, numIndex, nPols1] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->peCtx[i].numId)]);
                auto [denName, denIndex, nPols2] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->peCtx[i].denId)]);

                AltBn128::FrElement *pNum = getPolynomial(ptrCommitted[numName], nPols1, numIndex);
                AltBn128::FrElement *pDen = getPolynomial(ptrCommitted[denName], nPols2, denIndex);

                uint64_t zIndex = find(fflonkInfo->mapSections.section[cm3_n], fflonkInfo->cm_n[nCm3 + nPlookups + i]);
                calculateZ(pNum, pDen, zIndex);
            }

            for (uint64_t i = 0; i < nConnections; i++)
            {
                zklog.info("Calculating z for connection " + to_string(i) + " / " + to_string(nConnections));
                auto [numName, numIndex, nPols1] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->ciCtx[i].numId)]);
                auto [denName, denIndex, nPols2] = find(fflonkInfo->exp2pol[to_string(fflonkInfo->ciCtx[i].denId)]);

                AltBn128::FrElement *pNum = getPolynomial(ptrCommitted[numName], nPols1, numIndex);
                AltBn128::FrElement *pDen = getPolynomial(ptrCommitted[denName], nPols2, denIndex);

                uint64_t zIndex = find(fflonkInfo->mapSections.section[cm3_n], fflonkInfo->cm_n[nCm3 + nPlookups + nPermutations + i]);
                calculateZ(pNum, pDen, zIndex);
            }
            TimerStopAndLog(PIL_FFLONK_STAGE_3_CALCULATE_Z);

            TimerStart(PIL_FFLONK_STAGE_3_CALCULATE_EXPS);
#pragma omp parallel for
            for (uint64_t i = 0; i < N; i++)
            {
                pilFflonkSteps.step3_first(E, params, i);
            }
            TimerStopAndLog(PIL_FFLONK_STAGE_3_CALCULATE_EXPS);

            TimerStart(PIL_FFLONK_STAGE_3_EXTEND);
            extend(3, fflonkInfo->mapSectionsN.section[cm3_n]);
            TimerStopAndLog(PIL_FFLONK_STAGE_3_EXTEND);

            TimerStart(PIL_FFLONK_STAGE_3_COMMIT);
            shPlonkProver->commit(3, PTau, true);
            TimerStopAndLog(PIL_FFLONK_STAGE_3_COMMIT);
        }
        TimerStopAndLog(PIL_FFLONK_STAGE_3);
    }

    void PilFflonkProver::stage4(StepsParams &params)
    {
        TimerStart(PIL_FFLONK_STAGE_4);

        zklog.info("Computing challenges a");

        // STEP 4.1 - Compute random challenges
        transcript->reset();

        // Compute challenge a
        transcript->addScalar(challenges[3]);

        challenges[4] = transcript->getChallenge();
        zklog.info("Challenge a: " + E.fr.toString(challenges[4]));

        // STEP 4.2 - Compute stage 4 polynomial --> Q polynomial

        TimerStart(PIL_FFLONK_STAGE_4_CALCULATE_EXPS);
#pragma omp parallel for
        for (uint64_t i = 0; i < NExt * factorZK; i++)
        {
            pilFflonkSteps.step42ns_first(E, params, i);
        }
        TimerStopAndLog(PIL_FFLONK_STAGE_4_CALCULATE_EXPS);

        TimerStart(PIL_FFLONK_STAGE_4_CALCULATE_Q);
        Polynomial<AltBn128::Engine> *polQ = Polynomial<AltBn128::Engine>::fromEvaluations(E, fft, ptrCommitted["q_2ns"], fflonkInfo->qDim * NExt * factorZK);
        polQ->divZh(N, 1 << extendBitsTotal);
        shPlonkProver->addPolynomialShPlonk("Q", polQ);
        TimerStopAndLog(PIL_FFLONK_STAGE_4_CALCULATE_Q);

        TimerStart(PIL_FFLONK_STAGE_4_COMMIT);
        shPlonkProver->commit(4, PTau, true);
        TimerStopAndLog(PIL_FFLONK_STAGE_4_COMMIT);

        TimerStopAndLog(PIL_FFLONK_STAGE_4);
    }

    void PilFflonkProver::extend(u_int32_t stage, u_int32_t nPols)
    {

        AltBn128::FrElement *buffSrc = ptrCommitted["cm" + std::to_string(stage) + "_n"];       // N
        AltBn128::FrElement *buffDst = ptrCommitted["cm" + std::to_string(stage) + "_2ns"];     // NEXT * FACTORZK
        AltBn128::FrElement *buffCoefs = ptrCommitted["cm" + std::to_string(stage) + "_coefs"]; // N * FACTORZK

        // AltBn128::FrElement* buffer = stage == 0 ? bBuffer : bBufferCommitted;

        int nThreads = omp_get_max_threads() / 2;

        ThreadUtils::parset(buffCoefs, 0, N * factorZK * nPols * sizeof(AltBn128::FrElement), nThreads);
        ThreadUtils::parcpy(buffCoefs, buffSrc, N * nPols * sizeof(AltBn128::FrElement), nThreads);

        TimerStart(EXTEND_INTT);
        NTT_AltBn128 ntt(E, N);
        ntt.INTT(buffCoefs, buffSrc, N, nPols /*, buffer*/);
        TimerStopAndLog(EXTEND_INTT);

        for (u_int32_t i = 0; i < nPols; i++)
        {
            std::string name = (*zkey->polsNamesStage[stage])[i].name;
            u_int32_t openings = (*zkey->polsNamesStage[stage])[i].openings;
            for (u_int32_t j = 0; j < openings; ++j)
            {
                AltBn128::FrElement b;
                // randombytes_buf((void *)&(b), sizeof(FrElement)-1);
                b = E.fr.one();

                buffCoefs[j * nPols + i] = E.fr.add(buffCoefs[j * nPols + i], E.fr.neg(b));
                buffCoefs[(j + N) * nPols + i] = E.fr.add(buffCoefs[(j + N) * nPols + i], b);
            }
        }

        addCoefsToContext(stage, nPols, buffCoefs);

        ThreadUtils::parset(buffDst, 0, NExt * factorZK * nPols * sizeof(AltBn128::FrElement), nThreads);
        ThreadUtils::parcpy(buffDst, buffCoefs, N * factorZK * nPols * sizeof(AltBn128::FrElement), nThreads);

        TimerStart(EXTEND_NTT);
        NTT_AltBn128 nttExtended(E, 1 << nBitsExtZK);
        nttExtended.NTT(buffDst, buffDst, 1 << nBitsExtZK, nPols /*, buffer*/);
        TimerStopAndLog(EXTEND_NTT);
    }

    void PilFflonkProver::addCoefsToContext(u_int32_t stage, u_int32_t nPols, AltBn128::FrElement *buffCoefs)
    {
        // Store coefs to context
        for (u_int32_t i = 0; i < nPols; i++)
        {
            AltBn128::FrElement *coefs = new AltBn128::FrElement[N * factorZK];
            for (u_int32_t j = 0; j < N * factorZK; j++)
            {
                coefs[j] = buffCoefs[i + j * nPols];
            }
            std::string name = (*zkey->polsNamesStage[stage])[i].name;
            shPlonkProver->addPolynomialShPlonk(name, coefs, N * factorZK);
        }
    }

    uint64_t PilFflonkProver::find(std::vector<uint64_t> arr, uint64_t x)
    {
        for (uint64_t i = 0; i < arr.size(); i++)
        {
            if (arr[i] == x)
            {
                return i;
            }
        }
        throw new std::runtime_error("Index " + std::to_string(x) + " not found");
    }

    std::tuple<std::string, uint64_t, uint64_t> PilFflonkProver::find(uint64_t x)
    {
        if (x < fflonkInfo->nCm1 * 2 + fflonkInfo->nCm2 * 2)
        {
            for (uint64_t i = 0; i < fflonkInfo->mapSections.section[cm2_n].size(); i++)
            {
                if (fflonkInfo->mapSections.section[cm2_n][i] == x)
                {
                    return std::make_tuple("cm2_n", i, fflonkInfo->mapSections.section[cm2_n].size());
                }
            }
        }
        else if (x < fflonkInfo->nCm1 * 2 + fflonkInfo->nCm2 * 2 + fflonkInfo->nCm3 * 2)
        {
            for (uint64_t i = 0; i < fflonkInfo->mapSections.section[cm3_n].size(); i++)
            {
                if (fflonkInfo->mapSections.section[cm3_n][i] == x)
                {
                    return std::make_tuple("cm3_n", i, fflonkInfo->mapSections.section[cm3_n].size());
                }
            }
        }
        else
        {
            for (uint64_t i = 0; i < fflonkInfo->tmpExp_n.size(); i++)
            {
                if (fflonkInfo->tmpExp_n[i] == x)
                {
                    return std::make_tuple("tmpExp_n", i, fflonkInfo->tmpExp_n.size());
                }
            }
        }

        throw new std::runtime_error("Index " + std::to_string(x) + " not found");
    }

    AltBn128::FrElement *PilFflonkProver::getPolynomial(AltBn128::FrElement *buffer, uint64_t nPols, uint64_t id)
    {
        AltBn128::FrElement *pol = new AltBn128::FrElement[N];
        for (uint64_t i = 0; i < N; i++)
        {
            pol[i] = buffer[i * nPols + id];
        }
        return pol;
    }

    void PilFflonkProver::calculateZ(AltBn128::FrElement *pNum, AltBn128::FrElement *pDen, uint64_t id)
    {
        AltBn128::FrElement *denI = batchInverse(pDen, N);

        ptrCommitted["cm3_n"][id] = E.fr.one();

        for (uint64_t i = 1; i < N; i++)
        {
            ptrCommitted["cm3_n"][id + fflonkInfo->nCm3 * i] = E.fr.mul(ptrCommitted["cm3_n"][id + fflonkInfo->nCm3 * (i - 1)], E.fr.mul(pNum[i - 1], denI[i - 1]));
        }

        zkassert(E.fr.eq(E.fr.one(), E.fr.mul(ptrCommitted["cm3_n"][id + fflonkInfo->nCm3 * (N - 1)], E.fr.mul(pNum[N - 1], denI[N - 1]))));
    }

    AltBn128::FrElement *PilFflonkProver::batchInverse(AltBn128::FrElement *pol, uint64_t N)
    {
        AltBn128::FrElement *tmp = new FrElement[N];
        tmp[0] = pol[0];

        for (uint64_t i = 1; i < N; i++)
        {
            tmp[i] = E.fr.mul(tmp[i - 1], pol[i]);
        }

        AltBn128::FrElement z0;
        AltBn128::FrElement z1;
        E.fr.inv(z0, tmp[N - 1]);

        AltBn128::FrElement *denI = new FrElement[N];
        for (uint64_t i = N - 1; i > 0; i--)
        {
            z1 = E.fr.mul(z0, pol[i]);
            denI[i] = E.fr.mul(z0, tmp[i - 1]);
            z0 = z1;
        }

        denI[0] = z0;

        delete[] tmp;

        return denI;
    }

    void PilFflonkProver::calculateH1H2(AltBn128::FrElement *fPol, AltBn128::FrElement *tPol, uint64_t h1Id, uint64_t h2Id)
    {
        map<AltBn128::FrElement, uint64_t, CompareFe> idx_t(E);
        multimap<AltBn128::FrElement, uint64_t, CompareFe> s(E);
        multimap<AltBn128::FrElement, uint64_t>::iterator it;

        std::vector<int> counter(N, 1);

        for (uint64_t i = 0; i < N; i++)
        {
            AltBn128::FrElement key = tPol[i];
            idx_t[key] = i + 1;
        }

        for (uint64_t i = 0; i < N; i++)
        {
            AltBn128::FrElement key = fPol[i];
            uint64_t indx = idx_t[key];
            if (indx == 0)
            {
                zklog.error("calculateH1H2() Number not included: " + E.fr.toString(fPol[i]));
                exitProcess();
            }
            ++counter[indx - 1];
        }

        uint64_t id = 0;
        for (u_int64_t i = 0; i < N; ++i)
        {
            if (counter[id] == 0)
            {
                ++id;
            }
            counter[id] -= 1;
        
            ptrCommitted["cm2_n"][h1Id + fflonkInfo->nCm2 * i] = tPol[id];
            if (counter[id] == 0)
            {
                ++id;
            }
            counter[id] -= 1;
            ptrCommitted["cm2_n"][h2Id + fflonkInfo->nCm2 * i] = tPol[id];
        }
    }

}
