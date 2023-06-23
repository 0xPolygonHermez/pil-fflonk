#include <sstream>

#include "shplonk.hpp"

namespace ShPlonk {

    ShPlonkProver::ShPlonkProver(AltBn128::Engine &_E, const std::string &protocol) : E(_E) {
        this->protocol = protocol;

        transcript = new Keccak256Transcript<AltBn128::Engine>(_E);
        // this->reset();
    }

    Polynomial<AltBn128::Engine> * ShPlonkProver::computeRi(u_int32_t degree, FrElement* rootsRi, Polynomial<AltBn128::Engine> * polynomialFi) {

        FrElement* evals = new FrElement[degree];

        for(u_int32_t i = 0; i < degree; ++i) {
            evals[i] = polynomialFi->fastEvaluate(rootsRi[i]);
        }

        Polynomial<AltBn128::Engine> * polynomialRi = Polynomial<AltBn128::Engine>::lagrangePolynomialInterpolation(rootsRi, evals, degree);
        
        // Check the degree of R0(X) < (degree - 1)
        if (polynomialRi->getDegree() > (degree - 1))
        {
            throw std::runtime_error("Polynomial is not well calculated");
        }

        delete[] evals;

        return polynomialRi;
    }

    std::map<std::string, Polynomial<AltBn128::Engine> *> ShPlonkProver::computeR(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, FrElement *> rootsMap, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF) {
        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }
        
        std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR;

        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            std::string log = "> Computing R" + std::to_string(i) + " polynomial"; 
            LOG_TRACE(log);
            polynomialsR["R" + std::to_string(i)] = computeRi(degrees[i], rootsMap["f" + std::to_string(i)], polynomialsF["f" + std::to_string(i)]);
        }

        delete[] degrees;

        return polynomialsR;
    }

    Polynomial<AltBn128::Engine>* ShPlonkProver::computeZT(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, FrElement *> rootsMap)
    {
        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }
        
        u_int32_t nRoots = 0;
        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            nRoots += degrees[i];
        }

        FrElement* arr = new FrElement[nRoots];

        u_int32_t index = 0;
        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            for(u_int32_t j = 0; j < degrees[i]; ++j) {
                arr[index++] = rootsMap["f" + std::to_string(i)][j];
            }

        }

        Polynomial<AltBn128::Engine> * zt = Polynomial<AltBn128::Engine>::zerofierPolynomial(arr, nRoots);

        delete[] arr;
        delete[] degrees;

        return zt;
    }

    Polynomial<AltBn128::Engine> * ShPlonkProver::computeL(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeY, FrElement challengeAlpha, std::map<std::string, FrElement *> rootsMap, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR, Polynomial<AltBn128::Engine> * polynomialW)
    {
        LOG_TRACE("··· Computing L polynomial");

        FrElement* mulL = new FrElement[rootsMap.size()];
        FrElement* preL = new FrElement[rootsMap.size()];
        FrElement* evalRiY = new FrElement[rootsMap.size()];

        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            FrElement mulLi = E.fr.one();
            for (u_int32_t j = 0; j < degrees[i]; j++)
            {
                mulLi = E.fr.mul(mulLi, E.fr.sub(challengeY, rootsMap["f" + std::to_string(i)][j]));
            }
            mulL[i] = mulLi;
        }

       
        FrElement alpha = E.fr.one();
        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            preL[i] = alpha;
            for(u_int32_t j = 0; j < rootsMap.size(); ++j) {
                if(i != j) {
                    preL[i] = E.fr.mul(preL[i], mulL[j]);
                }
            }
            alpha = E.fr.mul(alpha,challengeAlpha);
        }

        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
           evalRiY[i] = polynomialsR["R" + std::to_string(i)]->fastEvaluate(challengeY);
        }

        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            if(zkeyFflonk->f[i]->degree > maxDegree) {
                maxDegree = zkeyFflonk->f[i]->degree;
            }
        }

        u_int64_t lengthBuffer = std::pow(2, ((u_int64_t)log2(maxDegree - 1)) + 1);

        // COMPUTE L(X)
        Polynomial<AltBn128::Engine>* polynomialL = new Polynomial<AltBn128::Engine>(E, lengthBuffer);
        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsF["f" + std::to_string(i)]);
            fTmp->subScalar(evalRiY[i]);
            fTmp->mulScalar(preL[i]);
            polynomialL->add(*fTmp);

        }
       
        LOG_TRACE("> Computing ZT polynomial");
        Polynomial<AltBn128::Engine>* polynomialZT = computeZT(zkeyFflonk, rootsMap);

        FrElement evalZTY = polynomialZT->fastEvaluate(challengeY);
        polynomialW->mulScalar(evalZTY);
        polynomialL->sub(*polynomialW);

        if (polynomialL->getDegree() > maxDegree)
        {
            throw std::runtime_error("L Polynomial is not well calculated");
        }

        delete[] mulL;
        delete[] preL;
        delete[] evalRiY;
        delete[] degrees;

        return polynomialL;
        
    }

    Polynomial<AltBn128::Engine>* ShPlonkProver::computeZTS2(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, FrElement *> rootsMap)
    {
        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }
        
        u_int32_t nRoots = 0;
        for(u_int32_t i = 1; i < zkeyFflonk->f.size(); ++i) {
            nRoots += degrees[i];
        }

        FrElement* arr = new FrElement[nRoots];

        u_int32_t index = 0;
        for(u_int32_t i = 1; i < zkeyFflonk->f.size(); ++i) {
            for(u_int32_t j = 0; j < degrees[i]; ++j) {
                arr[index++] = rootsMap["f" + std::to_string(i)][j];
            }

        }


        Polynomial<AltBn128::Engine> * zts2 = Polynomial<AltBn128::Engine>::zerofierPolynomial(arr, nRoots);

        delete[] degrees;
        delete[] arr;

        return zts2;
    }

    Polynomial<AltBn128::Engine> * ShPlonkProver::computeW(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeAlpha, FrElement challengeXi, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR)
    {

        LOG_TRACE("··· Computing W polynomial");

        FrElement alpha = E.fr.one();

        u_int64_t nTotalRoots = 0;
        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            nTotalRoots += zkeyFflonk->f[i]->nPols * zkeyFflonk->f[i]->nOpeningPoints;
        }

        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            u_int64_t fiDegree = zkeyFflonk->f[i]->degree + nTotalRoots - zkeyFflonk->f[i]->nPols * zkeyFflonk->f[i]->nOpeningPoints;
            if(fiDegree > maxDegree) {
                maxDegree = fiDegree;
            }
        }

        u_int64_t lengthBuffer = std::pow(2, ((u_int64_t)log2(maxDegree - 1)) + 1);

        Polynomial<AltBn128::Engine> * polynomialW = new Polynomial<AltBn128::Engine>(E, lengthBuffer);
              
        FrElement* initialOpenValues = new FrElement[zkeyFflonk->openingPoints.size()];
        for(u_int32_t i = 0; i < zkeyFflonk->openingPoints.size(); ++i) {
            initialOpenValues[i] = challengeXi;
            for(u_int32_t j = 0; j < zkeyFflonk->openingPoints[i]; ++j) {
                initialOpenValues[i] = E.fr.mul(initialOpenValues[i], zkeyFflonk->omegas["w1_1d1"][0]);
            }
        }

        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            u_int32_t openingPoint = zkeyFflonk->f[i]->openingPoints[0];

            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsF["f" + std::to_string(i)]);
            fTmp->sub(*polynomialsR["R" + std::to_string(i)]);
            fTmp->mulScalar(alpha);

            auto it = zkeyFflonk->openingPoints.find(openingPoint);
            if (it == zkeyFflonk->openingPoints.end()) throw std::runtime_error("Opening point not found");
            FrElement openValue = initialOpenValues[it->first];

            fTmp->divByZerofier(zkeyFflonk->f[i]->nPols, openValue);
           
            polynomialW->add(*fTmp);

            alpha = E.fr.mul(alpha, challengeAlpha);
           
        }

        if (polynomialW->getDegree() > maxDegree - nTotalRoots)
        {
            throw std::runtime_error("W Polynomial is not well calculated");
        }
        
        delete[] initialOpenValues;

        return polynomialW;
        
    }

    Polynomial<AltBn128::Engine> * ShPlonkProver::computeWp(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeY, FrElement challengeAlpha, std::map<std::string, FrElement *> rootsMap, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR, Polynomial<AltBn128::Engine> * polynomialW) {
        LOG_TRACE("> Computing L polynomial");
        Polynomial<AltBn128::Engine>* polynomialL = computeL(zkeyFflonk, challengeY, challengeAlpha, rootsMap, polynomialsF, polynomialsR, polynomialW);

        LOG_TRACE("> Computing ZTS2 polynomial");
        Polynomial<AltBn128::Engine>* polynomialZTS2 = computeZTS2(zkeyFflonk, rootsMap);

        FrElement ZTS2Y = polynomialZTS2->fastEvaluate(challengeY);
        E.fr.inv(ZTS2Y, ZTS2Y);
        polynomialL->mulScalar(ZTS2Y);

        LOG_TRACE("> Computing W' = L / ZTS2 polynomial");
        polynomialL->divByZerofier(1, challengeY);

        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            if(zkeyFflonk->f[i]->degree > maxDegree) {
                maxDegree = zkeyFflonk->f[i]->degree;
            }
        }

        maxDegree -= 1;

        if (polynomialL->getDegree() >= maxDegree)
        {
            throw std::runtime_error("Degree of L(X)/(ZTS2(y)(X-y)) is not correct");
        }

        return polynomialL;
    }

    AltBn128::FrElement ShPlonkProver::computeChallengeXiSeed(std::map<std::string, G1Point> commits, FrElement previousChallenge)
    {    
        transcript->reset();
        // TODO NULL FIX
        // if(previousChallenge != nullptr) {
        //     transcript->addScalar(previousChallenge);
        // }

        for (const auto& x : commits) {
            transcript->addPolCommitment(x.second);
        }

        return transcript->getChallenge();
    }


    AltBn128::FrElement ShPlonkProver::computeChallengeAlpha(FrElement challengeXiSeed, std::map <std::string, FrElement> evaluationsMap)
    {    
        u_int32_t nEvaluations = evaluationsMap.size();

        std::string* evaluationsNamesOrdered = new std::string[nEvaluations];

        int index = 0;
        for(auto const& x : evaluationsMap) {
            evaluationsNamesOrdered[index++] = x.first;
        }

        std::sort(evaluationsNamesOrdered, evaluationsNamesOrdered + nEvaluations);

        transcript->reset();
        transcript->addScalar(challengeXiSeed);

        for (u_int32_t i = 0; i < nEvaluations; ++i) {
            transcript->addScalar(evaluationsMap[evaluationsNamesOrdered[i]]);
        }

        delete[] evaluationsNamesOrdered;

        return transcript->getChallenge();
    }

    AltBn128::FrElement ShPlonkProver::computeChallengeY(FrElement challengeAlpha, G1Point W)
    {    
        transcript->reset();
        transcript->addScalar(challengeAlpha);
        transcript->addPolCommitment(W);
        return transcript->getChallenge();
    }

    std::map<std::string, AltBn128::FrElement*> ShPlonkProver::calculateRoots(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeXiSeed) {
        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

        std::map<std::string, FrElement *> rootsMap;
        std::map<std::string, FrElement *> omegasMap;

        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); i++) {
            omegasMap["w" + std::to_string(degrees[i])][0] = E.fr.one();
            for (u_int32_t j = 1; j < degrees[i]; j++)
            {   
                std::string omega = "w" + std::to_string(degrees[i]);
                omegasMap["w" + std::to_string(degrees[i])][j] = E.fr.mul(omegasMap["w" + std::to_string(degrees[i])][j - 1], *((FrElement *)zkeyFflonk->omegas[omega]));
            }

            rootsMap["f" + std::to_string(i)] = new FrElement[degrees[i]];

            // This code only works when opening by opening points
            u_int32_t openingPoint = zkeyFflonk->f[i]->openingPoints[0];
            if(openingPoint == 0) {
                rootsMap["f" + std::to_string(i)][0] = E.fr.one();
            } else {
                std::string omega = "w" + std::to_string(degrees[i]) + "_" + std::to_string(openingPoint) + "d" + std::to_string(degrees[i]);
                rootsMap["f" + std::to_string(i)][0] = *((FrElement *)zkeyFflonk->omegas[omega]);
            }

            for(u_int32_t j = 0; j < zkeyFflonk->powerW / degrees[i]; ++j) {
                rootsMap["f" + std::to_string(i)][0] = E.fr.mul(rootsMap["f" + std::to_string(i)][0], challengeXiSeed);
            }

            for (u_int32_t j = 1; j < degrees[i]; j++)
            {
                rootsMap["f" + std::to_string(i)][j] = E.fr.mul(rootsMap["f" + std::to_string(i)][0], omegasMap["w" + std::to_string(degrees[i])][j]);
            }
        }

        delete[] degrees;

        return rootsMap;
    }

    AltBn128::FrElement ShPlonkProver::getMontgomeryBatchedInverse(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeXi, FrElement challengeY, std::map<std::string, FrElement *> rootsMap)
    {
        std::map <std::string, FrElement> inverseElements;

        std::ostringstream ss;
        //   · denominator needed in step 8 and 9 of the verifier to multiply by 1/Z_H(xi)
        FrElement xiN = challengeXi;
        for (u_int64_t i = 0; i < zkeyFflonk->power; i++)
        {
            xiN = E.fr.square(xiN);
        }

        inverseElements["zh"] = E.fr.sub(xiN, E.fr.one());

        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

        
        std::map<std::string, bool> isAddedMulLi;

        for(u_int32_t i = 1; i < zkeyFflonk->f.size(); ++i) {
            std::string nPols = std::to_string(zkeyFflonk->f[i]->nPols);

            std::string concatenatedOpenings;

            for(u_int32_t j = 0; j < zkeyFflonk->f[i]->nOpeningPoints; ++j) {
                concatenatedOpenings += std::to_string(zkeyFflonk->f[i]->openingPoints[j]);
            }

            std::string wName = zkeyFflonk->f[i]->openingPoints[0] == 0 
                ? nPols + "_" + concatenatedOpenings
                : nPols + "_" + std::to_string(zkeyFflonk->f[i]->openingPoints[0]) + "d" + nPols + "_" +  concatenatedOpenings;

            if (isAddedMulLi.find(wName) == isAddedMulLi.end()) {
                isAddedMulLi[wName] = true;
                FrElement mulLi = E.fr.one();
                for (u_int32_t j = 0; j < degrees[i]; j++)
                {
                    mulLi = E.fr.mul(mulLi, E.fr.sub(challengeY, rootsMap["f" + std::to_string(i)][j]));
                }
                inverseElements["mulL" + std::to_string(i+1)] = mulLi;
            }
        }

        std::map<std::string, bool> isAddedDen;

        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            std::string nPols = std::to_string(zkeyFflonk->f[i]->nPols);

            std::string concatenatedOpenings;

            for(u_int32_t j = 0; j < zkeyFflonk->f[i]->nOpeningPoints; ++j) {
                concatenatedOpenings += std::to_string(zkeyFflonk->f[i]->openingPoints[j]);
            }

            std::string wName = zkeyFflonk->f[i]->openingPoints[0] == 0 
                ? nPols + "_" + concatenatedOpenings
                : nPols + "_" + std::to_string(zkeyFflonk->f[i]->openingPoints[0]) + "d" + nPols + "_" +  concatenatedOpenings;

            if (isAddedDen.find(wName) == isAddedDen.end()) {
                isAddedDen[wName] = true;
                FrElement den1 = E.fr.set(degrees[i]);
                for (u_int64_t j = 0; j < (degrees[i] - 2); j++)
                {
                    den1 = E.fr.mul(den1, rootsMap["f" + std::to_string(i)][0]);
                }

                for (uint j = 0; j < degrees[i]; j++) {
                    FrElement den2 = rootsMap["f" + std::to_string(i)][((degrees[i] - 1) * j) % degrees[i]];
                    FrElement den3 = E.fr.sub(challengeY, rootsMap["f" + std::to_string(i)][j]);

                    ss.str("");
                    ss << "LiS" << i << "_" << (j + 1) << " ";
                    inverseElements[ss.str()] = E.fr.mul(E.fr.mul(den1, den2), den3);
                }
            }
        }
        
        

        FrElement mulAccumulator = E.fr.one();
        for (auto &[key, value] : inverseElements)
        {
            mulAccumulator = E.fr.mul(mulAccumulator, value);
        }

        E.fr.inv(mulAccumulator, mulAccumulator);

        delete[] degrees;

        return mulAccumulator;
    }    

    std::map <std::string, AltBn128::FrElement> ShPlonkProver::calculateEvaluations(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, FrElement challengeXi) {
        
        std::map <std::string, FrElement > evaluationsMap;

        
        FrElement* initialOpenValues = new FrElement[zkeyFflonk->openingPoints.size()];
        for(u_int32_t i = 0; i < zkeyFflonk->openingPoints.size(); ++i) {
            initialOpenValues[i] = challengeXi;
            for(u_int32_t j = 0; j < zkeyFflonk->openingPoints[i]; ++j) {
                initialOpenValues[i] = E.fr.mul(initialOpenValues[i], zkeyFflonk->omegas["w1_1d1"][0]);
            }
        }

        //Calculate evaluations
        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            
            u_int32_t openingPoint = zkeyFflonk->f[i]->openingPoints[0];
            std::string wPower = openingPoint == 0 ? "" : (openingPoint == 1 ? "w" : "w" + std::to_string(openingPoint));

            auto it = zkeyFflonk->openingPoints.find(openingPoint);
            if (it == zkeyFflonk->openingPoints.end()) throw std::runtime_error("Opening point not found");
            FrElement openValue = initialOpenValues[it->first];

            
            for(u_int32_t k = 0; k < zkeyFflonk->f[i]->nPols; ++k) {
                std::string polName = zkeyFflonk->f[i]->pols[k];
                if(polynomials[polName] == nullptr) throw std::runtime_error("Polynomial not found");
                evaluationsMap[polName + wPower] = polynomials[polName]->fastEvaluate(openValue);
            }
        }

        delete[] initialOpenValues;

        return evaluationsMap;

    }

    AltBn128::G1Point ShPlonkProver::sumCommits(u_int32_t nCommits, G1Point *commits) {
        // Initialize commit to zero in G1 curve 
        G1Point commit = E.g1.zero();

        // Add all the commits
        for(u_int32_t i = 0; i < nCommits; ++i) {
            E.g1.add(commit, commits[i], commit);  
        }

        return commit;
    }

    Polynomial<AltBn128::Engine> * ShPlonkProver::sumPolynomials(u_int32_t nPols, Polynomial<AltBn128::Engine> ** polynomials) {
        if(nPols == 1) {
            return polynomials[0];
        }

        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < nPols; ++i) {
            if(polynomials[i]->getDegree() > maxDegree) {
                maxDegree = polynomials[i]->getDegree();
            }
        }

        u_int64_t lengthBuffer = std::pow(2, ((u_int64_t)log2(maxDegree - 1)) + 1);

        Polynomial<AltBn128::Engine> *polynomial = new Polynomial<AltBn128::Engine>(E, lengthBuffer);
        
        #pragma omp parallel for
        for (u_int64_t i = 0; i < maxDegree + 1; i++) {
            FrElement coef = E.fr.zero();
            for (u_int32_t j = 0; j < nPols; ++j) {
                if(polynomials[j] != nullptr && polynomials[j]->getDegree() > 0 && i <= polynomials[j]->getDegree()) {
                    coef = E.fr.add(coef, polynomials[j]->coef[i]);
                }
            }
            polynomial->coef[i] = coef;
        }

        return polynomial;
    }

    std::map<std::string , ShPlonkCommit *> ShPlonkProver::prepareCommits(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits) {
        u_int32_t nPols = zkeyFflonk->f.size();
        std::map<std::string , ShPlonkCommit *> shPlonkCommits;
        for(u_int64_t i = 0; i < nPols; ++i) {

            Polynomial<AltBn128::Engine>** polynomialsStage = new Polynomial<AltBn128::Engine>*[zkeyFflonk->f[i]->nStages];
            for (size_t j = 0; j < zkeyFflonk->f[i]->nStages; ++j) {
                u_int64_t lengthBuffer = std::pow(2, ((u_int64_t)log2(zkeyFflonk->f[i]->degree - 1)) + 1);
                polynomialsStage[j] = new Polynomial<AltBn128::Engine>(E, lengthBuffer);
            }
            G1Point* commitsStage = new G1Point[zkeyFflonk->f[i]->nStages];

            for(u_int64_t j = 0; j < zkeyFflonk->f[i]->nStages; ++j) {
        
                std::string index = "f" + std::to_string(zkeyFflonk->f[i]->index) + "_" + std::to_string(zkeyFflonk->f[i]->stages[j].stage);
                if(commits[index]->pol == nullptr) throw std::runtime_error("Polynomial " + index + " is not provided");
                // TODO NULL fix
                // if(commits[index]->commit == NULL) throw std::runtime_error("Commit " + index + " is not provided");

                polynomialsStage[j] = commits[index]->pol;
                commitsStage[j] = commits[index]->commit;
            }

            shPlonkCommits["f" + std::to_string(i)]->commit = sumCommits(zkeyFflonk->f[i]->nStages, commitsStage);
            shPlonkCommits["f" + std::to_string(i)]->pol = sumPolynomials(zkeyFflonk->f[i]->nStages, polynomialsStage);

            if(shPlonkCommits["f" + std::to_string(i)]->pol->getDegree() > zkeyFflonk->f[i]->degree) {
                throw std::runtime_error("Polynomial f" + std::to_string(i) + " degree is greater than expected");
            }   
            
            delete[] polynomialsStage;
            delete[] commitsStage;
        }
        
        return shPlonkCommits;
    }

    std::map<std::string, ShPlonkCommit *> ShPlonkProver::commit(u_int32_t stage, ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, G1PointAffine *PTau, bool multiExp) {
        
        std::map<std::string, ShPlonkCommit *> commits;

        for (auto it = zkeyFflonk->f.begin(); it != zkeyFflonk->f.end(); ++it) {
            ZkeyPilFflonk::ShPlonkPol* pol = it->second;

            u_int32_t* stages = new u_int32_t[pol->nStages];
            for(u_int32_t i = 0; i < pol->nStages; ++i) {
                stages[i] = pol->stages[i].stage;
            }

            if(find(stages, pol->nStages, stage) != -1) {
                ZkeyPilFflonk::ShPlonkStage* stagePol = &pol->stages[stage];
                
                u_int64_t* lengths = new u_int64_t[pol->nPols]{};

                CPolynomial<AltBn128::Engine> *fi = new CPolynomial(E, pol->nPols);
                for(u_int32_t j = 0; j < stagePol->nPols; ++j) {
                    std::string name = stagePol->pols[j].name;
                    int index = find(pol->pols, pol->nPols, name);
                    if (index != -1) {
                            throw std::runtime_error("Polynomial " + std::string(name) + " missing");
                    }
                    
                    if (polynomials[name] == nullptr) {
                        throw std::runtime_error("Polynomial " + std::string(name) + " is not provided");
                    }

                    lengths[index] = polynomials[name]->getDegree() + 1;
                    fi->addPolynomial(index, polynomials[name]);
                }

                std::string index = "f" + std::to_string(pol->index) + "_" + std::to_string(stage);

                Polynomial<AltBn128::Engine>* cPol = fi->getPolynomial();

                // Check degree
                if (cPol->getDegree() > pol->degree) 
                {
                    throw std::runtime_error("CPolynomial is not well calculated");
                }

                commits[index]->pol = cPol;

                if(multiExp) {
                    G1Point Fi;
                    E.g1.multiMulByScalar(Fi, PTau, (uint8_t *)cPol, sizeof(cPol[0]), fi->getDegree() + 1, pol->nPols, lengths);
                    commits[index]->commit = Fi;
                }

                delete[] lengths;
                delete[] fi;

            }

            delete[] stages;
        }
        
        return commits; 
    }

    ShPlonkOpen ShPlonkProver::open(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, G1PointAffine *PTau, FrElement previousChallenge) {

        ShPlonkOpen proofCommitments;

        std::map<std::string , ShPlonkCommit *> shPlonkCommits = prepareCommits(zkeyFflonk, commits);

        std::map<std::string, G1Point> nonConstantCommits;

        std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF;


        for(auto const& x : shPlonkCommits) {           
            u_int32_t index = std::stoi(x.first.substr(1));
            if(zkeyFflonk->f[index]->nStages > 1 || zkeyFflonk->f[index]->nStages != 0) {
                nonConstantCommits[x.first] = x.second->commit;
            }

            polynomialsF[x.first] = x.second->pol;
        }

        // Compute challenge xi seed
        FrElement challengeXiSeed = computeChallengeXiSeed(nonConstantCommits, previousChallenge);
        proofCommitments.challengeXiSeed = challengeXiSeed;

        // Calculate roots
        std::map<std::string, FrElement *> rootsMap = calculateRoots(zkeyFflonk, challengeXiSeed);

        FrElement challengeXi = E.fr.one();
        for(u_int32_t i = 0; i < zkeyFflonk->powerW; ++i) {
            challengeXi = E.fr.mul(challengeXi, challengeXiSeed);
        }

        proofCommitments.evaluationCommitments = calculateEvaluations(zkeyFflonk, polynomials, challengeXi);        

        FrElement challengeAlpha = computeChallengeAlpha(challengeXiSeed, proofCommitments.evaluationCommitments);

        std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR = computeR(zkeyFflonk, rootsMap, polynomialsF);
        
        Polynomial<AltBn128::Engine>* polynomialW = computeW(zkeyFflonk, challengeAlpha, challengeXi, polynomialsF, polynomialsR);

        LOG_TRACE("> Computing W multi exponentiation");
        G1Point W;
        u_int64_t* lengthsW = new u_int64_t[1]{polynomialW->getDegree() + 1};
        E.g1.multiMulByScalar(W, PTau, (uint8_t *)polynomialW, sizeof(polynomialW[0]), polynomialW->getDegree() + 1, 1, lengthsW);
        proofCommitments.polynomialCommitments["W"] = W;

        FrElement challengeY = computeChallengeY(challengeAlpha, W);

        Polynomial<AltBn128::Engine>* polynomialWp = computeWp(zkeyFflonk, challengeY, challengeAlpha, rootsMap, polynomialsF, polynomialsR, polynomialW);
        G1Point Wp;
        u_int64_t* lengthsWp = new u_int64_t[1]{polynomialWp->getDegree() + 1};
        E.g1.multiMulByScalar(Wp, PTau, (uint8_t *)polynomialWp, sizeof(polynomialWp[0]), polynomialWp->getDegree() + 1, 1, lengthsWp);
        proofCommitments.polynomialCommitments["Wp"] = Wp;

        proofCommitments.evaluationCommitments["inv"] = getMontgomeryBatchedInverse(zkeyFflonk, challengeXi, challengeY, rootsMap);

        delete[] lengthsW;
        delete[] lengthsWp;

        return proofCommitments;
    }

    int ShPlonkProver::find(std::string* arr, u_int32_t n, std::string x) {
        for(int i = 0; i < int(n); ++i) {
            if(arr[i] == x) {
                return true;
            }
        }

        return -1;
    }

    int ShPlonkProver::find(u_int32_t* arr, u_int32_t n, u_int32_t x) {
        for(int i = 0; i < int(n); ++i) {
            if(arr[i] == x) {
                return i;
            }
        }

        return -1;
    }

}
