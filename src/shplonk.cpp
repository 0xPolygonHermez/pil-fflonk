#include <sstream>

#include "shplonk.hpp"

namespace ShPlonk {

    ShPlonkProver::ShPlonkProver(AltBn128::Engine &_E, const std::string &protocol) : E(_E) {
        this->protocol = protocol;

        transcript = new Keccak256Transcript<AltBn128::Engine>(_E);
        // this->reset();
    }

    void ShPlonkProver::computeR(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk) {
        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }
        
        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            std::string log = "> Computing R" + std::to_string(i) + " polynomial"; 
            LOG_TRACE(log);

            FrElement* evals = new FrElement[degrees[i]];

            for(u_int32_t j = 0; j < degrees[i]; ++j) {
                evals[i] = polynomialsShPlonk["f" + std::to_string(i)]->fastEvaluate(rootsMap["f" + std::to_string(i)][j]);
            }

            polynomialsShPlonk["R" + std::to_string(i)] = Polynomial<AltBn128::Engine>::lagrangePolynomialInterpolation(rootsMap["f" + std::to_string(i)], evals, degrees[i]);
            
            // Check the degree of R0(X) < (degree - 1)
            if (polynomialsShPlonk["R" + std::to_string(i)]->getDegree() > (degrees[i] - 1))
            {
                throw std::runtime_error("Polynomial is not well calculated");
            }

            delete[] evals;
        }

        delete[] degrees;
    }

    void ShPlonkProver::computeZT(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk)
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

        polynomialsShPlonk["ZT"] = Polynomial<AltBn128::Engine>::zerofierPolynomial(arr, nRoots);

        delete[] arr;
        delete[] degrees;
    }

    void ShPlonkProver::computeL(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk)
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
           evalRiY[i] = polynomialsShPlonk["R" + std::to_string(i)]->fastEvaluate(challengeY);
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
            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsShPlonk["f" + std::to_string(i)]);
            fTmp->subScalar(evalRiY[i]);
            fTmp->mulScalar(preL[i]);
            polynomialL->add(*fTmp);

        }
       
        LOG_TRACE("> Computing ZT polynomial");
        computeZT(zkeyFflonk);

        FrElement evalZTY = polynomialsShPlonk["ZT"]->fastEvaluate(challengeY);
        polynomialsShPlonk["W"]->mulScalar(evalZTY);
        polynomialL->sub(*polynomialsShPlonk["W"]);

        if (polynomialL->getDegree() > maxDegree)
        {
            throw std::runtime_error("L Polynomial is not well calculated");
        }

        delete[] mulL;
        delete[] preL;
        delete[] evalRiY;
        delete[] degrees;

        polynomialsShPlonk["Wp"] = polynomialL;
        
    }

    void ShPlonkProver::computeZTS2(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk)
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


        polynomialsShPlonk["ZTS2"] = Polynomial<AltBn128::Engine>::zerofierPolynomial(arr, nRoots);

        delete[] degrees;
        delete[] arr;
    }

    void ShPlonkProver::computeW(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk)
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

            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsShPlonk["f" + std::to_string(i)]);
            fTmp->sub(*polynomialsShPlonk["R" + std::to_string(i)]);
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

        polynomialsShPlonk["W"] = polynomialW;
        
    }

    void ShPlonkProver::computeWp(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk) {
        LOG_TRACE("> Computing L polynomial");
        computeL(zkeyFflonk);

        LOG_TRACE("> Computing ZTS2 polynomial");
        computeZTS2(zkeyFflonk);

        FrElement ZTS2Y = polynomialsShPlonk["ZTS2"]->fastEvaluate(challengeY);
        E.fr.inv(ZTS2Y, ZTS2Y);
        polynomialsShPlonk["Wp"]->mulScalar(ZTS2Y);

        LOG_TRACE("> Computing W' = L / ZTS2 polynomial");
        polynomialsShPlonk["Wp"]->divByZerofier(1, challengeY);

        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < zkeyFflonk->f.size(); ++i) {
            if(zkeyFflonk->f[i]->degree > maxDegree) {
                maxDegree = zkeyFflonk->f[i]->degree;
            }
        }

        maxDegree -= 1;

        if (polynomialsShPlonk["Wp"]->getDegree() >= maxDegree)
        {
            throw std::runtime_error("Degree of L(X)/(ZTS2(y)(X-y)) is not correct");
        }
    }

    void ShPlonkProver::computeChallengeXiSeed(FrElement previousChallenge)
    {    
        transcript->reset();
        // TODO NULL FIX
        // if(previousChallenge != nullptr) {
        //     transcript->addScalar(previousChallenge);
        // }

        for (const auto& x : nonConstantCommits) {
            transcript->addPolCommitment(x.second);
        }

        challengeXiSeed = transcript->getChallenge();
    }


    void ShPlonkProver::computeChallengeAlpha()
    {    
        u_int32_t nEvaluations = evaluationCommitments.size();

        std::string* evaluationsNamesOrdered = new std::string[nEvaluations];

        int index = 0;
        for(auto const& x : evaluationCommitments) {
            evaluationsNamesOrdered[index++] = x.first;
        }

        std::sort(evaluationsNamesOrdered, evaluationsNamesOrdered + nEvaluations);

        transcript->reset();
        transcript->addScalar(challengeXiSeed);

        for (u_int32_t i = 0; i < nEvaluations; ++i) {
            transcript->addScalar(evaluationCommitments[evaluationsNamesOrdered[i]]);
        }

        delete[] evaluationsNamesOrdered;

        challengeAlpha = transcript->getChallenge();
    }

    void ShPlonkProver::computeChallengeY(G1Point W)
    {    
        transcript->reset();
        transcript->addScalar(challengeAlpha);
        transcript->addPolCommitment(W);
        challengeY = transcript->getChallenge();
    }

    void ShPlonkProver::calculateRoots(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk) {
        u_int32_t* degrees = new u_int32_t[zkeyFflonk->f.size()]; 

        for(auto const& x : zkeyFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

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
    }

    void ShPlonkProver::getMontgomeryBatchedInverse(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk)
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

        evaluationCommitments["inv"] = mulAccumulator;
    }    

    void ShPlonkProver::calculateEvaluations(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials) {
        
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
                evaluationCommitments[polName + wPower] = polynomials[polName]->fastEvaluate(openValue);
            }
        }

        delete[] initialOpenValues;
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

    void ShPlonkProver::prepareCommits(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits) {
        u_int32_t nPols = zkeyFflonk->f.size();
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

            polynomialCommitments["f" + std::to_string(i)] = sumCommits(zkeyFflonk->f[i]->nStages, commitsStage);

            if(zkeyFflonk->f[i]->nStages > 1 || zkeyFflonk->f[i]->nStages != 0) {
                nonConstantCommits["f" + std::to_string(i)] = polynomialCommitments["f" + std::to_string(i)]; 
            }

            polynomialsShPlonk["f" + std::to_string(i)] = sumPolynomials(zkeyFflonk->f[i]->nStages, polynomialsStage);

            if(polynomialsShPlonk["f" + std::to_string(i)]->getDegree() > zkeyFflonk->f[i]->degree) {
                throw std::runtime_error("Polynomial f" + std::to_string(i) + " degree is greater than expected");
            }   
            
            delete[] polynomialsStage;
            delete[] commitsStage;
        }
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

    void ShPlonkProver::open(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, G1PointAffine *PTau, FrElement previousChallenge) {

        prepareCommits(zkeyFflonk, commits);

        // Compute challenge xi seed
        computeChallengeXiSeed(previousChallenge);

        // Calculate roots
        calculateRoots(zkeyFflonk);

        challengeXi = E.fr.one();
        for(u_int32_t i = 0; i < zkeyFflonk->powerW; ++i) {
            challengeXi = E.fr.mul(challengeXi, challengeXiSeed);
        }

        calculateEvaluations(zkeyFflonk, polynomials);        

        computeChallengeAlpha();

        computeR(zkeyFflonk);
        
        computeW(zkeyFflonk);

        LOG_TRACE("> Computing W multi exponentiation");
        G1Point W;
        u_int64_t* lengthsW = new u_int64_t[1]{polynomialsShPlonk["W"]->getDegree() + 1};
        E.g1.multiMulByScalar(W, PTau, (uint8_t *)polynomialsShPlonk["W"], sizeof(polynomialsShPlonk["W"][0]), polynomialsShPlonk["W"]->getDegree() + 1, 1, lengthsW);
        polynomialCommitments["W"] = W;

        computeChallengeY(W);

        computeWp(zkeyFflonk);
        G1Point Wp;
        u_int64_t* lengthsWp = new u_int64_t[1]{polynomialsShPlonk["Wp"]->getDegree() + 1};
        E.g1.multiMulByScalar(Wp, PTau, (uint8_t *)polynomialsShPlonk["Wp"], sizeof(polynomialsShPlonk["Wp"][0]), polynomialsShPlonk["Wp"]->getDegree() + 1, 1, lengthsWp);
        polynomialCommitments["Wp"] = Wp;

        getMontgomeryBatchedInverse(zkeyFflonk);

        delete[] lengthsW;
        delete[] lengthsWp;
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
