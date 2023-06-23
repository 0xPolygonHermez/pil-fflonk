#ifndef SHPLONK_HPP
#define SHPLONK_HPP

#include <iostream>
#include <string.h>
#include <binfile_utils.hpp>
#include <nlohmann/json.hpp>
#include "polynomial/polynomial.hpp"
#include "polynomial/cpolynomial.hpp"
#include "zkey_pilfflonk.hpp"
#include "zkey.hpp"
#include "shplonk.hpp"
#include "keccak_256_transcript.hpp"
#include <alt_bn128.hpp>
#include "fft.hpp"

using json = nlohmann::json;

using namespace std;

namespace ShPlonk {

    struct ShPlonkCommit {
        Polynomial<AltBn128::Engine>* pol;
        AltBn128::G1Point commit;
    };

    struct ShPlonkOpen{
        std::map <std::string, AltBn128::G1Point> polynomialCommitments;
        std::map <std::string, AltBn128::FrElement> evaluationCommitments;
        AltBn128::FrElement challengeXiSeed;
    };

    class ShPlonkProver {

        AltBn128::Engine &E;

        using FrElement = typename AltBn128::FrElement;
        using G1Point = typename AltBn128::G1Point;
        using G1PointAffine = typename AltBn128::G1PointAffine;

        std::string protocol;
        
        Keccak256Transcript<AltBn128::Engine> *transcript;

    public:
        ShPlonkProver(AltBn128::Engine &_E, const std::string &protocol);
        
        std::map<std::string, ShPlonkCommit *> commit(u_int32_t stage, ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, G1PointAffine *PTau, bool multiExp);

        ShPlonkOpen open(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, G1PointAffine *PTau, FrElement previousChallenge);

    protected:
        Polynomial<AltBn128::Engine>* computeRi(u_int32_t degree, FrElement* rootsRi, Polynomial<AltBn128::Engine> * polynomialFi);

        std::map<std::string, Polynomial<AltBn128::Engine> *> computeR(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, FrElement *> rootsMap, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF);

        Polynomial<AltBn128::Engine>* computeZT(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, FrElement *> rootsMap);

        Polynomial<AltBn128::Engine>* computeL(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeY, FrElement challengeAlpha, std::map<std::string, FrElement *> rootsMap, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR, Polynomial<AltBn128::Engine> * polynomialW);

        Polynomial<AltBn128::Engine>* computeZTS2(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, FrElement *> rootsMap);

        Polynomial<AltBn128::Engine>* computeW(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeAlpha, FrElement challengeXi, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR);

        Polynomial<AltBn128::Engine>* computeWp(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeY, FrElement challengeAlpha, std::map<std::string, FrElement *> rootsMap, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsF, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsR, Polynomial<AltBn128::Engine> * polynomialW);

        FrElement computeChallengeXiSeed(std::map<std::string, G1Point> commits, FrElement previousChallenge);

        FrElement computeChallengeAlpha(FrElement challengeXiSeed, std::map <std::string, FrElement> evaluationsMap);
        
        FrElement computeChallengeY(FrElement challengeAlpha, G1Point W);

        std::map<std::string, AltBn128::FrElement*> calculateRoots(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeXiSeed);

        AltBn128::FrElement getMontgomeryBatchedInverse(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, FrElement challengeXi, FrElement challengeY, std::map<std::string, FrElement *> rootsMap);

        std::map<std::string, AltBn128::FrElement> calculateEvaluations(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, FrElement challengeXi);

        AltBn128::G1Point sumCommits(u_int32_t nCommits, G1Point *commits);

        Polynomial<AltBn128::Engine>* sumPolynomials(u_int32_t nPols, Polynomial<AltBn128::Engine>** polynomials);

        std::map<std::string , ShPlonkCommit *> prepareCommits(ZkeyPilFflonk::PilFflonkZkeyHeader *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits);

        int find(std::string* arr, u_int32_t n, std::string x);
        int find(u_int32_t* arr, u_int32_t n, u_int32_t x);

    };
}

#endif