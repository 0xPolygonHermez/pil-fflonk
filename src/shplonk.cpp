#include <sstream>
#include "timer.hpp"
#include "zklog.hpp"
#include "shplonk.hpp"

namespace ShPlonk {

    ShPlonkProver::ShPlonkProver(AltBn128::Engine &_E, PilFflonkZkey::PilFflonkZkey *zkey) : E(_E) {
        zkeyPilFflonk = zkey;

        transcript = new Keccak256Transcript(_E);

        //Calculate evaluations size
        nEvaluations = 0;
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            nEvaluations += zkeyPilFflonk->f[i]->nPols;
        }

        evaluationsNames = new std::string[nEvaluations];

        //Calculate evaluations names
        int index = 0;
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            
            u_int32_t openingPoint = zkeyPilFflonk->f[i]->openingPoints[0];
            std::string wPower = openingPoint == 0 ? "" : (openingPoint == 1 ? "w" : "w" + std::to_string(openingPoint));

            for(u_int32_t k = 0; k < zkeyPilFflonk->f[i]->nPols; ++k) {
                std::string polName = zkeyPilFflonk->f[i]->pols[k];
                evaluationsNames[index++] = polName + wPower;
            }
        }

        this->reset();
    }

    ShPlonkProver::~ShPlonkProver() {
        this->reset();

        delete transcript;
        delete zkeyPilFflonk;
    }  

    void ShPlonkProver::reset() {
        this->polynomialsShPlonk.clear();
        this->rootsMap.clear();
        this->evaluationCommitments.clear();
        this->polynomialCommitments.clear();
        this->openingPoints.clear();
    }

    void ShPlonkProver::addPolynomialShPlonk(const std::string &key, Polynomial<AltBn128::Engine> *pol) {
        polynomialsShPlonk[key] = pol;
    }

    Polynomial<AltBn128::Engine> * ShPlonkProver::getPolynomialShPlonk(const std::string &key) {
        return this->polynomialsShPlonk[key];
    }

    void ShPlonkProver::computeR() {
        TimerStart(SHPLONK_COMPUTE_R_POLYNOMIALS);
        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }
        
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            FrElement* evals = new FrElement[degrees[i]];

            for(u_int32_t j = 0; j < degrees[i]; ++j) {
                evals[j] = polynomialsShPlonk["f" + std::to_string(i)]->fastEvaluate(rootsMap["f" + std::to_string(i)][j]);
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
        TimerStopAndLog(SHPLONK_COMPUTE_R_POLYNOMIALS);
    }

    void ShPlonkProver::computeZT()
    {
        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }
        
        u_int32_t nRoots = 0;
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            nRoots += degrees[i];
        }

        FrElement* arr = new FrElement[nRoots];

        u_int32_t index = 0;
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            for(u_int32_t j = 0; j < degrees[i]; ++j) {
                arr[index++] = rootsMap["f" + std::to_string(i)][j];
            }

        }

        polynomialsShPlonk["ZT"] = Polynomial<AltBn128::Engine>::zerofierPolynomial(arr, nRoots);

        delete[] arr;
        delete[] degrees;
    }

    void ShPlonkProver::computeL(AltBn128::FrElement *reservedBuffer, AltBn128::FrElement *tmpBuffer)
    {
        TimerStart(SHPLONK_COMPUTE_L_POLYNOMIAL);

        FrElement* mulL = new FrElement[zkeyPilFflonk->f.size()];
        FrElement* preL = new FrElement[zkeyPilFflonk->f.size()];
        FrElement* evalRiY = new FrElement[zkeyPilFflonk->f.size()];

        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            FrElement mulLi = E.fr.one();
            for (u_int32_t j = 0; j < degrees[i]; j++)
            {
                mulLi = E.fr.mul(mulLi, E.fr.sub(challengeY, rootsMap["f" + std::to_string(i)][j]));
            }
            mulL[i] = mulLi;
        }

       
        FrElement alpha = E.fr.one();
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            preL[i] = alpha;
            for(u_int32_t j = 0; j < zkeyPilFflonk->f.size(); ++j) {
                if(i != j) {
                    preL[i] = E.fr.mul(preL[i], mulL[j]);
                }
            }
            alpha = E.fr.mul(alpha,challengeAlpha);
        }

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
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
        polynomialsShPlonk["Wp"] = new Polynomial<AltBn128::Engine>(E, reservedBuffer, lengthBuffer);
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsShPlonk["f" + std::to_string(i)], tmpBuffer);
            fTmp->subScalar(evalRiY[i]);
            fTmp->mulScalar(preL[i]);
            polynomialsShPlonk["Wp"]->add(*fTmp);

        }
       
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

        TimerStopAndLog(SHPLONK_COMPUTE_L_POLYNOMIAL);      
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

    void ShPlonkProver::computeW(AltBn128::FrElement *reservedBuffer, AltBn128::FrElement *tmpBuffer)
    {

        TimerStart(SHPLONK_COMPUTE_W_POLYNOMIAL);

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

        Polynomial<AltBn128::Engine> * polynomialW = new Polynomial<AltBn128::Engine>(E, reservedBuffer, lengthBuffer);
              
        FrElement* initialOpenValues = new FrElement[openingPoints.size()];
        for(u_int32_t i = 0; i < openingPoints.size(); ++i) {
            initialOpenValues[i] = challengeXi;
            for(u_int32_t j = 0; j < openingPoints[i]; ++j) {
                initialOpenValues[i] = E.fr.mul(initialOpenValues[i], zkeyPilFflonk->omegas["w1_1d1"]);
            }
        } 

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            u_int32_t openingPoint = zkeyPilFflonk->f[i]->openingPoints[0];
 
            auto fTmp = Polynomial<AltBn128::Engine>::fromPolynomial(E, *polynomialsShPlonk["f" + std::to_string(i)], tmpBuffer);
            fTmp->sub(*polynomialsShPlonk["R" + std::to_string(i)]);
            fTmp->mulScalar(alpha);

            auto found = std::find(openingPoints.begin(), openingPoints.end(), openingPoint);
            if (found == openingPoints.end()) throw std::runtime_error("Opening point not found");
            FrElement openValue = initialOpenValues[std::distance(openingPoints.begin(), found)];

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
        TimerStopAndLog(SHPLONK_COMPUTE_W_POLYNOMIAL);
        
    }

    void ShPlonkProver::computeWp(AltBn128::FrElement *reservedBuffer, AltBn128::FrElement *tmpBuffer) {
        TimerStart(SHPLONK_COMPUTE_WP_POLYNOMIAL);

        computeL(reservedBuffer, tmpBuffer);
        computeZTS2();

        FrElement ZTS2Y = polynomialsShPlonk["ZTS2"]->fastEvaluate(challengeY);
        E.fr.inv(ZTS2Y, ZTS2Y);
        polynomialsShPlonk["Wp"]->mulScalar(ZTS2Y);
        polynomialsShPlonk["Wp"]->divByZerofier(1, challengeY);

        u_int64_t maxDegree = 0; 
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            if(zkeyPilFflonk->f[i]->degree > maxDegree) {
                maxDegree = zkeyPilFflonk->f[i]->degree;
            }
        }

        maxDegree -= 1;

        if (polynomialsShPlonk["Wp"]->getDegree() > maxDegree)
        {
            throw std::runtime_error("Degree of L(X)/(ZTS2(y)(X-y)) is not correct");
        }

        TimerStopAndLog(SHPLONK_COMPUTE_WP_POLYNOMIAL);
    }

    void ShPlonkProver::computeChallengeXiSeed(FrElement previousChallenge)
    {     
        transcript->reset();
        transcript->addScalar(previousChallenge);

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            if(zkeyPilFflonk->f[i]->nStages > 1 || zkeyPilFflonk->f[i]->stages[0].stage != 0) {
                G1Point commit = polynomialCommitments["f" + std::to_string(zkeyPilFflonk->f[i]->index)]; 
                transcript->addPolCommitment(commit);
            }
        }

        challengeXiSeed = transcript->getChallenge();
    }


    void ShPlonkProver::computeChallengeAlpha()
    {    
        transcript->reset();
        transcript->addScalar(challengeXiSeed);
        
        for (u_int32_t i = 0; i < nEvaluations; ++i) {
            if(evaluationsNames[i] != "Q") {
                transcript->addScalar(evaluationCommitments[evaluationsNames[i]]);
            }
        }

        delete[] evaluationsNames;

        challengeAlpha = transcript->getChallenge();
    }

    void ShPlonkProver::computeChallengeY(G1Point W)
    {    
        transcript->reset();
        transcript->addScalar(challengeAlpha);
        transcript->addPolCommitment(W);
        challengeY = transcript->getChallenge();
    }

    void ShPlonkProver::calculateOpeningPoints() {
     
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            for(u_int32_t j = 0; j < zkeyPilFflonk->f[i]->nOpeningPoints; ++j) {
                auto it = std::find(openingPoints.begin(), openingPoints.end(), zkeyPilFflonk->f[i]->openingPoints[j]);
                if(it == openingPoints.end()) {
                    openingPoints.push_back(zkeyPilFflonk->f[i]->openingPoints[j]);
                }
            }
        }        
    }

    void ShPlonkProver::calculateRoots() {
        TimerStart(SHPLONK_CALCULATE_ROOTS);
        u_int32_t* degrees = new u_int32_t[zkeyPilFflonk->f.size()]; 

        for(auto const& x : zkeyPilFflonk->f) {
            degrees[x.second->index] = x.second->nPols;
        }

        std::map<std::string, FrElement *> omegasMap;

        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); i++) {
            omegasMap["w" + std::to_string(degrees[i])] = new FrElement[degrees[i]];
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

        for (auto const &x : omegasMap) {
            delete[] x.second;
        }

        delete[] degrees;
        TimerStopAndLog(SHPLONK_CALCULATE_ROOTS);
    }

    void ShPlonkProver::getMontgomeryBatchedInverse()
    {   
        TimerStart(SHPLONK_CALCULATE_MONTGOMERY_BATCHED_INVERSE);
        std::vector<FrElement> inverseElements;
     
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
                 inverseElements.push_back(mulLi);
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

                if(degrees[i] > 1) {
                    FrElement den1 = E.fr.set(degrees[i]);
                    if(degrees[i] > 2) {
                        for (u_int64_t j = 0; j < (degrees[i] - 2); j++)
                        {
                            den1 = E.fr.mul(den1, rootsMap["f" + std::to_string(i)][0]);
                        }
                    }
                    
                    for (uint j = 0; j < degrees[i]; j++) {
                        FrElement den2 = rootsMap["f" + std::to_string(i)][((degrees[i] - 1) * j) % degrees[i]];
                        FrElement den3 = E.fr.sub(challengeY, rootsMap["f" + std::to_string(i)][j]);

                        inverseElements.push_back(E.fr.mul(E.fr.mul(den1, den2), den3));
                    }
                }
            }
        }
        
        

        FrElement mulAccumulator = E.fr.one();
        for (u_int32_t i = 0; i < inverseElements.size(); i++)
        {            
            mulAccumulator = E.fr.mul(mulAccumulator, inverseElements[i]);
        }
      
        E.fr.inv(mulAccumulator, mulAccumulator);

        delete[] degrees;

        evaluationCommitments["inv"] = mulAccumulator;

        TimerStopAndLog(SHPLONK_CALCULATE_MONTGOMERY_BATCHED_INVERSE);
    }    

    void ShPlonkProver::calculateEvaluations() {
        TimerStart(SHPLONK_CALCULATE_EVALUATIONS);
        FrElement* initialOpenValues = new FrElement[openingPoints.size()];
        for(u_int32_t i = 0; i < openingPoints.size(); ++i) {
            initialOpenValues[i] = challengeXi;
            for(u_int32_t j = 0; j < openingPoints[i]; ++j) {
                initialOpenValues[i] = E.fr.mul(initialOpenValues[i], zkeyPilFflonk->omegas["w1_1d1"]);
            }
        }

        //Calculate evaluations
        for(u_int32_t i = 0; i < zkeyPilFflonk->f.size(); ++i) {
            
            u_int32_t openingPoint = zkeyPilFflonk->f[i]->openingPoints[0];
            std::string wPower = openingPoint == 0 ? "" : (openingPoint == 1 ? "w" : "w" + std::to_string(openingPoint));

            auto found = std::find(openingPoints.begin(), openingPoints.end(), openingPoint);
            if (found == openingPoints.end()) throw std::runtime_error("Opening point not found");
            FrElement openValue = initialOpenValues[std::distance(openingPoints.begin(), found)];

            for(u_int32_t k = 0; k < zkeyPilFflonk->f[i]->nPols; ++k) {
                std::string polName = zkeyPilFflonk->f[i]->pols[k];
                if(polynomialsShPlonk[polName] == nullptr) throw std::runtime_error("Polynomial not found");
                evaluationCommitments[polName + wPower] = polynomialsShPlonk[polName]->fastEvaluate(openValue);
            }
        }

        delete[] initialOpenValues;

        TimerStopAndLog(SHPLONK_CALCULATE_EVALUATIONS);
    }

    void ShPlonkProver::sumCommits(u_int32_t nCommits, std::string* polynomialsNames, std::string dstName) {
        // Initialize commit to zero in G1 curve 
        G1Point commit = E.g1.zero();

        // Add all the commits
        for(u_int32_t i = 0; i < nCommits; ++i) {
            E.g1.add(commit, polynomialCommitments[polynomialsNames[i]], commit);  
        }

        polynomialCommitments[dstName] = commit;
    }

    void ShPlonkProver::sumPolynomials(u_int32_t nPols, std::string* polynomialsNames, std::map<std::string, AltBn128::FrElement *> ptrShPlonk, std::string dstName) {
        if(nPols == 1) {
            polynomialsShPlonk[dstName] = polynomialsShPlonk[polynomialsNames[0]];
            return;            
        }
    
        u_int64_t maxDegree = 0;
        for(u_int32_t i = 0; i < nPols; ++i) {
            polynomialsShPlonk[polynomialsNames[i]]->fixDegree();
            if(polynomialsShPlonk[polynomialsNames[i]]->getDegree() > maxDegree) {
                maxDegree = polynomialsShPlonk[polynomialsNames[i]]->getDegree();
            }
        }

        u_int64_t lengthBuffer = std::pow(2, (u_int64_t)log2(maxDegree - 1) + 1);
        
        polynomialsShPlonk[dstName] = new Polynomial<AltBn128::Engine>(E, ptrShPlonk[dstName], lengthBuffer);

        #pragma omp parallel for
        for (u_int64_t i = 0; i <= maxDegree; i++) {
            FrElement coef = E.fr.zero();
            for (u_int32_t j = 0; j < nPols; ++j) {
                if(polynomialsShPlonk[polynomialsNames[j]] != nullptr && polynomialsShPlonk[polynomialsNames[j]]->getDegree() > 0 && i <= polynomialsShPlonk[polynomialsNames[j]]->getDegree()) {
                    coef = E.fr.add(coef, polynomialsShPlonk[polynomialsNames[j]]->coef[i]);
                }
            }
            polynomialsShPlonk[dstName]->coef[i] = coef;
        }
        polynomialsShPlonk[dstName]->fixDegree();
    }

    void ShPlonkProver::prepareCommits(std::map<std::string, AltBn128::FrElement *> ptrShPlonk) {
        u_int32_t nPols = zkeyPilFflonk->f.size();
        for(u_int64_t i = 0; i < nPols; ++i) {

            std::string* polynomialsNames = new std::string[zkeyPilFflonk->f[i]->nStages];

            for(u_int64_t j = 0; j < zkeyPilFflonk->f[i]->nStages; ++j) {
        
                std::string index = "f" + std::to_string(zkeyPilFflonk->f[i]->index) + "_" + std::to_string(zkeyPilFflonk->f[i]->stages[j].stage);
                polynomialsNames[j] = index;

                if(polynomialsShPlonk.find(index) == polynomialsShPlonk.end()) throw std::runtime_error("Polynomial " + index + " is not provided");
                if(polynomialCommitments.find(index) == polynomialCommitments.end()) throw std::runtime_error("Commit " + index + " is not provided");
            }
            
            sumCommits(zkeyPilFflonk->f[i]->nStages, polynomialsNames, "f" + std::to_string(zkeyPilFflonk->f[i]->index));
            sumPolynomials(zkeyPilFflonk->f[i]->nStages, polynomialsNames, ptrShPlonk,  "f" + std::to_string(zkeyPilFflonk->f[i]->index));

            if(polynomialsShPlonk["f" + std::to_string(zkeyPilFflonk->f[i]->index)]->getDegree() > zkeyPilFflonk->f[i]->degree) {
                throw std::runtime_error("Polynomial f" + std::to_string(i) + " degree is greater than expected");
            }   
            
            delete[] polynomialsNames;
        }
    }

    void ShPlonkProver::commit(u_int32_t stage, G1PointAffine *PTau, std::map<std::string, AltBn128::FrElement *> ptrShPlonk, bool multiExp) {
        
        if(NULL == zkeyPilFflonk) {
            throw std::runtime_error("Zkey data not set");
        }

        for (auto it = zkeyPilFflonk->f.begin(); it != zkeyPilFflonk->f.end(); ++it) {
            PilFflonkZkey::ShPlonkPol* pol = it->second;

            u_int32_t* stages = new u_int32_t[pol->nStages];
            for(u_int32_t i = 0; i < pol->nStages; ++i) {
                stages[i] = pol->stages[i].stage;
            }
 
            int stagePos = find(stages, pol->nStages, stage);

            if(stagePos != -1) {
                PilFflonkZkey::ShPlonkStage* stagePol = &pol->stages[stagePos];
                
                u_int64_t* lengths = new u_int64_t[pol->nPols]{};

                CPolynomial<AltBn128::Engine> *fi = new CPolynomial(E, pol->nPols);
                for(u_int32_t j = 0; j < stagePol->nPols; ++j) {
                    std::string name = stagePol->pols[j].name;
                    int index = find(pol->pols, pol->nPols, name);
                    if (index == -1) {
                            throw std::runtime_error("Polynomial " + std::string(name) + " missing");
                    }
                    
                    if (polynomialsShPlonk[name] == nullptr) {
                        throw std::runtime_error("Polynomial " + std::string(name) + " is not provided");
                    }

                    lengths[index] = polynomialsShPlonk[name]->getDegree() + 1;
                    fi->addPolynomial(index, polynomialsShPlonk[name]);
                }

                std::string index = "f" + std::to_string(pol->index) + "_" + std::to_string(stage);

                polynomialsShPlonk[index] = fi->getPolynomial(ptrShPlonk[index]);            

                // Check degree
                if (polynomialsShPlonk[index]->getDegree() > pol->degree) 
                {
                    throw std::runtime_error("CPolynomial is not well calculated");
                }

                if(multiExp) {
                    G1Point Fi = multiExponentiation(PTau, polynomialsShPlonk[index], pol->nPols, lengths);
                    zklog.info("Commit " + index + ": " + E.g1.toString(Fi));
                    polynomialCommitments[index] = Fi;
                }
                delete[] lengths;
            }

            delete[] stages;
        }
    }

    json ShPlonkProver::open(G1PointAffine *PTau, std::map<std::string, AltBn128::FrElement *> ptrShPlonk, FrElement previousChallenge) {
        TimerStart(SHPLONK_OPEN);

        if(NULL == zkeyPilFflonk) {
            throw std::runtime_error("Zkey data not set");
        }
        
        for(auto const&[key, commit] : zkeyPilFflonk->committedConstants) {
            G1Point C;
            E.g1.copy(C, *((G1PointAffine *)commit));
            polynomialCommitments[key] = C;
        }

        prepareCommits(ptrShPlonk);

        // Compute challenge xi seed
        computeChallengeXiSeed(previousChallenge);

        zklog.info("Challenge xi seed: " + E.fr.toString(challengeXiSeed));

        // Calculate opening points
        calculateOpeningPoints();

        // Calculate roots
        calculateRoots();

        challengeXi = E.fr.one();
        for(u_int32_t i = 0; i < zkeyPilFflonk->powerW; ++i) {
            challengeXi = E.fr.mul(challengeXi, challengeXiSeed);
        }

        zklog.info("Challenge xi: " + E.fr.toString(challengeXi));

        calculateEvaluations();        

        computeChallengeAlpha();

        zklog.info("Challenge alpha: " + E.fr.toString(challengeAlpha));

        computeR();
        
        computeW(ptrShPlonk["W"], ptrShPlonk["tmp"]);

        u_int64_t* lengthsW = new u_int64_t[1]{polynomialsShPlonk["W"]->getDegree() + 1};
        G1Point W = multiExponentiation(PTau, polynomialsShPlonk["W"], 1, lengthsW);
        polynomialCommitments["W"] = W;

        zklog.info("Commit W: " + E.g1.toString(polynomialCommitments["W"]));

        computeChallengeY(W);

        zklog.info("Challenge Y: " + E.fr.toString(challengeY));

        computeWp(ptrShPlonk["Wp"], ptrShPlonk["tmp"]);
        u_int64_t* lengthsWp = new u_int64_t[1]{polynomialsShPlonk["Wp"]->getDegree() + 1};
        G1Point Wp = multiExponentiation(PTau, polynomialsShPlonk["Wp"], 1, lengthsWp);

        polynomialCommitments["Wp"] = Wp;

        zklog.info("Commit Wp: " + E.g1.toString(polynomialCommitments["Wp"]));

        getMontgomeryBatchedInverse();

        zklog.info("Batched Inverse shplonk: " + E.fr.toString(evaluationCommitments["inv"]));

        delete[] lengthsW;
        delete[] lengthsWp;

        TimerStopAndLog(SHPLONK_OPEN);
        return toJson();
    }

    AltBn128::G1Point ShPlonkProver::multiExponentiation(G1PointAffine *PTau, Polynomial<AltBn128::Engine> *polynomial, u_int32_t nx, u_int64_t x[])
    {
        TimerStart(SHPLONK_CALCULATE_MSM);
        G1Point value;
        FrElement *pol = this->polynomialFromMontgomery(polynomial);
        E.g1.multiMulByScalar(value, PTau, (uint8_t *)pol, sizeof(pol[0]), polynomial->getDegree() + 1, nx, x);
        TimerStopAndLog(SHPLONK_CALCULATE_MSM);
        return value;
    }

        AltBn128::FrElement *ShPlonkProver::polynomialFromMontgomery(Polynomial<AltBn128::Engine> *polynomial)
    {
        const u_int64_t length = polynomial->getDegree() + 1;
        FrElement *result = new FrElement[length];
        int nThreads = omp_get_max_threads() / 2;
        ThreadUtils::parset(result, 0, length * sizeof(FrElement), nThreads);

#pragma omp parallel for
        for (u_int32_t index = 0; index < length; ++index)
        {
            E.fr.fromMontgomery(result[index], polynomial->coef[index]);
        }

        return result;
    }


    int ShPlonkProver::find(std::string* arr, u_int32_t n, std::string x) {
        for(u_int32_t i = 0; i < n; ++i) {
            if(arr[i] == x) {
                return int(i);
            }
        }

        return -1;
    }

    int ShPlonkProver::find(u_int32_t* arr, u_int32_t n, u_int32_t x) {
        for(u_int32_t i = 0; i < n; ++i) {
            if(arr[i] == x) {
                return int(i);
            }
        }

        return -1;
    }

    AltBn128::FrElement ShPlonkProver::getChallengeXi() {
        return challengeXi;
    }

    json ShPlonkProver::toJson() {
        json jsonProof;

        jsonProof["polynomials"] = {};
        jsonProof["evaluations"] = {};

        for (auto &[key, point]: this->polynomialCommitments) {
            G1PointAffine tmp;
            E.g1.copy(tmp, point);

            jsonProof["polynomials"][key] = {};

            std::string x = E.f1.toString(tmp.x);
            std::string y = E.f1.toString(tmp.y);

            jsonProof["polynomials"][key].push_back(x);
            jsonProof["polynomials"][key].push_back(y);
            jsonProof["polynomials"][key].push_back("1");
        }

        for (auto &[key, element]: this->evaluationCommitments) {
            jsonProof["evaluations"][key] = E.fr.toString(element);
        }

        jsonProof["protocol"] = "pilfflonk";
        jsonProof["curve"] = "bn128";

        return jsonProof;
    }

}
