#include <sstream>

#include "shplonk.hpp"

namespace ShPlonk {

    ShPlonkProver::ShPlonkProver(AltBn128::Engine &_E, BinFileUtils::BinFile *zkeyBinfile) : E(_E) {

        transcript = new Keccak256Transcript(_E);

        zkeyPilFflonk = PilFflonkZkey::loadPilFflonkZkey(zkeyBinfile);
    }

    void ShPlonkProver::computeR() {
        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
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

    void ShPlonkProver::computeZT()
    {
        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
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

    void ShPlonkProver::computeL()
    {
        LOG_TRACE("··· Computing L polynomial");

        FrElement* mulL = new FrElement[rootsMap.size()];
        FrElement* preL = new FrElement[rootsMap.size()];
        FrElement* evalRiY = new FrElement[rootsMap.size()];

        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
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
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            if(zkeyPilFflonk->f[i]->degree > maxDegree) {
                maxDegree = zkeyPilFflonk->f[i]->degree;
            }
        }

        u_int64_t lengthBuffer = std::pow(2, ((u_int64_t)log2(maxDegree - 1)) + 1);

        // COMPUTE L(X)
        polynomialsShPlonk["Wp"] = new Polynomial<AltBn128::Engine>(E, lengthBuffer);
        for(u_int32_t i = 0; i < rootsMap.size(); ++i) {
            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsShPlonk["f" + std::to_string(i)]);
            fTmp->subScalar(evalRiY[i]);
            fTmp->mulScalar(preL[i]);
            polynomialsShPlonk["Wp"]->add(*fTmp);

        }
       
        LOG_TRACE("> Computing ZT polynomial");
        computeZT();

        FrElement evalZTY = polynomialsShPlonk["ZT"]->fastEvaluate(challengeY);
        polynomialsShPlonk["W"]->mulScalar(evalZTY);
        polynomialsShPlonk["Wp"]->sub(*polynomialsShPlonk["W"]);

        if (polynomialsShPlonk["Wp"]->getDegree() > maxDegree)
        {
            throw std::runtime_error("L Polynomial is not well calculated");
        }

        delete[] mulL;
        delete[] preL;
        delete[] evalRiY;
        delete[] degrees;        
    }

    void ShPlonkProver::computeZTS2()
    {
        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }
        
        u_int32_t nRoots = 0;
        for(u_int32_t i = 1; i < zkeyPilFflonk->f.size(); ++i) {
            nRoots += degrees[i];
        }

        FrElement* arr = new FrElement[nRoots];

        u_int32_t index = 0;
        for(u_int32_t i = 1; i < zkeyPilFflonk->f.size(); ++i) {
            for(u_int32_t j = 0; j < degrees[i]; ++j) {
                arr[index++] = rootsMap["f" + std::to_string(i)][j];
            }

        }


        polynomialsShPlonk["ZTS2"] = Polynomial<AltBn128::Engine>::zerofierPolynomial(arr, nRoots);

        delete[] degrees;
        delete[] arr;
    }

    void ShPlonkProver::computeW()
    {

        LOG_TRACE("··· Computing W polynomial");

        FrElement alpha = E.fr.one();

        u_int64_t nTotalRoots = 0;
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            nTotalRoots += zkeyPilFflonk->f[i]->nPols * zkeyPilFflonk->f[i]->nOpeningPoints;
        }

        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            u_int64_t fiDegree = zkeyPilFflonk->f[i]->degree + nTotalRoots - zkeyPilFflonk->f[i]->nPols * zkeyPilFflonk->f[i]->nOpeningPoints;
            if(fiDegree > maxDegree) {
                maxDegree = fiDegree;
            }
        }

        u_int64_t lengthBuffer = std::pow(2, ((u_int64_t)log2(maxDegree - 1)) + 1);

        Polynomial<AltBn128::Engine> * polynomialW = new Polynomial<AltBn128::Engine>(E, lengthBuffer);
              
        FrElement* initialOpenValues = new FrElement[zkeyPilFflonk->openingPoints.size()];
        for(u_int32_t i = 0; i < zkeyPilFflonk->openingPoints.size(); ++i) {
            initialOpenValues[i] = challengeXi;
            for(u_int32_t j = 0; j < zkeyPilFflonk->openingPoints[i]; ++j) {
                initialOpenValues[i] = E.fr.mul(initialOpenValues[i], zkeyPilFflonk->omegas["w1_1d1"]);
            }
        }

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            u_int32_t openingPoint = zkeyPilFflonk->f[i]->openingPoints[0];

            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsShPlonk["f" + std::to_string(i)]);
            fTmp->sub(*polynomialsShPlonk["R" + std::to_string(i)]);
            fTmp->mulScalar(alpha);

            auto it = zkeyPilFflonk->openingPoints.find(openingPoint);
            if (it == zkeyPilFflonk->openingPoints.end()) throw std::runtime_error("Opening point not found");
            FrElement openValue = initialOpenValues[it->first];

            fTmp->divByZerofier(zkeyPilFflonk->f[i]->nPols, openValue);
           
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

    void ShPlonkProver::computeWp() {
        LOG_TRACE("> Computing L polynomial");
        computeL();

        LOG_TRACE("> Computing ZTS2 polynomial");
        computeZTS2();

        FrElement ZTS2Y = polynomialsShPlonk["ZTS2"]->fastEvaluate(challengeY);
        E.fr.inv(ZTS2Y, ZTS2Y);
        polynomialsShPlonk["Wp"]->mulScalar(ZTS2Y);

        LOG_TRACE("> Computing W' = L / ZTS2 polynomial");
        polynomialsShPlonk["Wp"]->divByZerofier(1, challengeY);

        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            if(zkeyPilFflonk->f[i]->degree > maxDegree) {
                maxDegree = zkeyPilFflonk->f[i]->degree;
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
        transcript->addScalar(previousChallenge);

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

    void ShPlonkProver::calculateRoots() {
        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

        std::map<std::string, FrElement *> omegasMap;

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); i++) {
            omegasMap["w" + std::to_string(degrees[i])][0] = E.fr.one();
            for (u_int32_t j = 1; j < degrees[i]; j++)
            {   
                std::string omega = "w" + std::to_string(degrees[i]);
                omegasMap["w" + std::to_string(degrees[i])][j] = E.fr.mul(omegasMap["w" + std::to_string(degrees[i])][j - 1], zkeyPilFflonk->omegas[omega]);
            }

            rootsMap["f" + std::to_string(i)] = new FrElement[degrees[i]];

            // This code only works when opening by opening points
            u_int32_t openingPoint = zkeyPilFflonk->f[i]->openingPoints[0];
            if(openingPoint == 0) {
                rootsMap["f" + std::to_string(i)][0] = E.fr.one();
            } else {
                std::string omega = "w" + std::to_string(degrees[i]) + "_" + std::to_string(openingPoint) + "d" + std::to_string(degrees[i]);
                rootsMap["f" + std::to_string(i)][0] = zkeyPilFflonk->omegas[omega];
            }

            for(u_int32_t j = 0; j < zkeyPilFflonk->powerW / degrees[i]; ++j) {
                rootsMap["f" + std::to_string(i)][0] = E.fr.mul(rootsMap["f" + std::to_string(i)][0], challengeXiSeed);
            }

            for (u_int32_t j = 1; j < degrees[i]; j++)
            {
                rootsMap["f" + std::to_string(i)][j] = E.fr.mul(rootsMap["f" + std::to_string(i)][0], omegasMap["w" + std::to_string(degrees[i])][j]);
            }
        }

        delete[] degrees;
    }

    void ShPlonkProver::getMontgomeryBatchedInverse()
    {
        std::map <std::string, FrElement> inverseElements;

        std::ostringstream ss;
        //   · denominator needed in step 8 and 9 of the verifier to multiply by 1/Z_H(xi)
        FrElement xiN = challengeXi;
        for (u_int64_t i = 0; i < zkeyPilFflonk->power; i++)
        {
            xiN = E.fr.square(xiN);
        }

        inverseElements["zh"] = E.fr.sub(xiN, E.fr.one());

        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

        
        std::map<std::string, bool> isAddedMulLi;

        for(u_int32_t i = 1; i < zkeyPilFflonk->f.size(); ++i) {
            std::string nPols = std::to_string(zkeyPilFflonk->f[i]->nPols);

            std::string concatenatedOpenings;

            for(u_int32_t j = 0; j < zkeyPilFflonk->f[i]->nOpeningPoints; ++j) {
                concatenatedOpenings += std::to_string(zkeyPilFflonk->f[i]->openingPoints[j]);
            }

            std::string wName = zkeyPilFflonk->f[i]->openingPoints[0] == 0 
                ? nPols + "_" + concatenatedOpenings
                : nPols + "_" + std::to_string(zkeyPilFflonk->f[i]->openingPoints[0]) + "d" + nPols + "_" +  concatenatedOpenings;

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

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            std::string nPols = std::to_string(zkeyPilFflonk->f[i]->nPols);

            std::string concatenatedOpenings;

            for(u_int32_t j = 0; j < zkeyPilFflonk->f[i]->nOpeningPoints; ++j) {
                concatenatedOpenings += std::to_string(zkeyPilFflonk->f[i]->openingPoints[j]);
            }

            std::string wName = zkeyPilFflonk->f[i]->openingPoints[0] == 0 
                ? nPols + "_" + concatenatedOpenings
                : nPols + "_" + std::to_string(zkeyPilFflonk->f[i]->openingPoints[0]) + "d" + nPols + "_" +  concatenatedOpenings;

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

    void ShPlonkProver::calculateEvaluations() {
        
        FrElement* initialOpenValues = new FrElement[zkeyPilFflonk->openingPoints.size()];
        for(u_int32_t i = 0; i < zkeyPilFflonk->openingPoints.size(); ++i) {
            initialOpenValues[i] = challengeXi;
            for(u_int32_t j = 0; j < zkeyPilFflonk->openingPoints[i]; ++j) {
                initialOpenValues[i] = E.fr.mul(initialOpenValues[i], zkeyPilFflonk->omegas["w1_1d1"]);
            }
        }

        //Calculate evaluations
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            
            u_int32_t openingPoint = zkeyPilFflonk->f[i]->openingPoints[0];
            std::string wPower = openingPoint == 0 ? "" : (openingPoint == 1 ? "w" : "w" + std::to_string(openingPoint));

            auto it = zkeyPilFflonk->openingPoints.find(openingPoint);
            if (it == zkeyPilFflonk->openingPoints.end()) throw std::runtime_error("Opening point not found");
            FrElement openValue = initialOpenValues[it->first];

            
            for(u_int32_t k = 0; k < zkeyPilFflonk->f[i]->nPols; ++k) {
                std::string polName = zkeyPilFflonk->f[i]->pols[k];
                if(polynomialsShPlonk[polName] == nullptr) throw std::runtime_error("Polynomial not found");
                evaluationCommitments[polName + wPower] = polynomialsShPlonk[polName]->fastEvaluate(openValue);
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

    void ShPlonkProver::prepareCommits() {
        u_int32_t nPols = zkeyPilFflonk->f.size();
        for(u_int64_t i = 0; i < nPols; ++i) {

            Polynomial<AltBn128::Engine>** polynomialsStage = new Polynomial<AltBn128::Engine>*[zkeyPilFflonk->f[i]->nStages];
            for (size_t j = 0; j < zkeyPilFflonk->f[i]->nStages; ++j) {
                u_int64_t lengthBuffer = std::pow(2, ((u_int64_t)log2(zkeyPilFflonk->f[i]->degree - 1)) + 1);
                polynomialsStage[j] = new Polynomial<AltBn128::Engine>(E, lengthBuffer);
            }
            G1Point* commitsStage = new G1Point[zkeyPilFflonk->f[i]->nStages];

            for(u_int64_t j = 0; j < zkeyPilFflonk->f[i]->nStages; ++j) {
        
                std::string index = "f" + std::to_string(zkeyPilFflonk->f[i]->index) + "_" + std::to_string(zkeyPilFflonk->f[i]->stages[j].stage);
                if(polynomialsShPlonk.find(index) == polynomialsShPlonk.end()) throw std::runtime_error("Polynomial " + index + " is not provided");
                if(polynomialCommitments.find(index) == polynomialCommitments.end()) throw std::runtime_error("Commit " + index + " is not provided");

                polynomialsStage[j] = polynomialsShPlonk[index];
                commitsStage[j] = polynomialCommitments[index];
            }

            polynomialCommitments["f" + std::to_string(i)] = sumCommits(zkeyPilFflonk->f[i]->nStages, commitsStage);

            if(zkeyPilFflonk->f[i]->nStages > 1 || zkeyPilFflonk->f[i]->nStages != 0) {
                nonConstantCommits["f" + std::to_string(i)] = polynomialCommitments["f" + std::to_string(i)]; 
            }

            polynomialsShPlonk["f" + std::to_string(i)] = sumPolynomials(zkeyPilFflonk->f[i]->nStages, polynomialsStage);

            if(polynomialsShPlonk["f" + std::to_string(i)]->getDegree() > zkeyPilFflonk->f[i]->degree) {
                throw std::runtime_error("Polynomial f" + std::to_string(i) + " degree is greater than expected");
            }   
            
            delete[] polynomialsStage;
            delete[] commitsStage;
        }
    }

    void ShPlonkProver::commit(u_int32_t stage, G1PointAffine *PTau, bool multiExp) {
        
        cout << "COMMITS" << endl;

        for (auto it = zkeyPilFflonk->f.begin(); it != zkeyPilFflonk->f.end(); ++it) {
            PilFflonkZkey::ShPlonkPol* pol = it->second;

            u_int32_t* stages = new u_int32_t[pol->nStages];
            for(u_int32_t i = 0; i < pol->nStages; ++i) {
                stages[i] = pol->stages[i].stage;
            }

            cout << "STAGES " << stages[0] << endl;

            if(find(stages, pol->nStages, stage) != -1) {
                PilFflonkZkey::ShPlonkStage* stagePol = &pol->stages[stage];
                
                u_int64_t* lengths = new u_int64_t[pol->nPols]{};

                CPolynomial<AltBn128::Engine> *fi = new CPolynomial(E, pol->nPols);
                for(u_int32_t j = 0; j < stagePol->nPols; ++j) {
                    std::string name = stagePol->pols[j].name;
                    int index = find(pol->pols, pol->nPols, name);
                    if (index != -1) {
                            throw std::runtime_error("Polynomial " + std::string(name) + " missing");
                    }
                    
                    if (polynomialsShPlonk[name] == nullptr) {
                        throw std::runtime_error("Polynomial " + std::string(name) + " is not provided");
                    }

                    lengths[index] = polynomialsShPlonk[name]->getDegree() + 1;
                    fi->addPolynomial(index, polynomialsShPlonk[name]);
                }

                std::string index = "f" + std::to_string(pol->index) + "_" + std::to_string(stage);

                Polynomial<AltBn128::Engine>* cPol = fi->getPolynomial();

                // Check degree
                if (cPol->getDegree() > pol->degree) 
                {
                    throw std::runtime_error("CPolynomial is not well calculated");
                }

                polynomialsShPlonk[index] = cPol;

                if(multiExp) {
                    G1Point Fi;
                    E.g1.multiMulByScalar(Fi, PTau, (uint8_t *)cPol, sizeof(cPol[0]), fi->getDegree() + 1, pol->nPols, lengths);
                    polynomialCommitments[index] = Fi;
                }

                delete[] lengths;
                delete[] fi;

            }

            delete[] stages;
        }
    }

    void ShPlonkProver::open(G1PointAffine *PTau, FrElement previousChallenge) {

        prepareCommits();

        // Compute challenge xi seed
        computeChallengeXiSeed(previousChallenge);

        // Calculate roots
        calculateRoots();

        challengeXi = E.fr.one();
        for(u_int32_t i = 0; i < zkeyPilFflonk->powerW; ++i) {
            challengeXi = E.fr.mul(challengeXi, challengeXiSeed);
        }

        calculateEvaluations();        

        computeChallengeAlpha();

        computeR();
        
        computeW();

        LOG_TRACE("> Computing W multi exponentiation");
        G1Point W;
        u_int64_t* lengthsW = new u_int64_t[1]{polynomialsShPlonk["W"]->getDegree() + 1};
        E.g1.multiMulByScalar(W, PTau, (uint8_t *)polynomialsShPlonk["W"], sizeof(polynomialsShPlonk["W"][0]), polynomialsShPlonk["W"]->getDegree() + 1, 1, lengthsW);
        polynomialCommitments["W"] = W;

        computeChallengeY(W);

        computeWp();
        G1Point Wp;
        u_int64_t* lengthsWp = new u_int64_t[1]{polynomialsShPlonk["Wp"]->getDegree() + 1};
        E.g1.multiMulByScalar(Wp, PTau, (uint8_t *)polynomialsShPlonk["Wp"], sizeof(polynomialsShPlonk["Wp"][0]), polynomialsShPlonk["Wp"]->getDegree() + 1, 1, lengthsWp);
        polynomialCommitments["Wp"] = Wp;

        getMontgomeryBatchedInverse();

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
