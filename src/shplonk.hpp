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


    class ShPlonkProver {

        AltBn128::Engine &E;

        using FrElement = typename AltBn128::FrElement;
        using G1Point = typename AltBn128::G1Point;
        using G1PointAffine = typename AltBn128::G1PointAffine;

        std::string protocol;
        
        Keccak256Transcript<AltBn128::Engine> *transcript;

        FrElement challengeXiSeed;
        FrElement challengeXi;
        FrElement challengeAlpha;
        FrElement challengeY;

        std::map<std::string, FrElement *> rootsMap;

        std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsShPlonk;

        std::map<std::string, AltBn128::FrElement> evaluationCommitments;

        std::map <std::string, AltBn128::G1Point> polynomialCommitments;

        std::map <std::string, AltBn128::G1Point> nonConstantCommits;


    public:
        ShPlonkProver(AltBn128::Engine &_E, const std::string &protocol);
        
        std::map<std::string, ShPlonkCommit *> commit(u_int32_t stage, PilFflonkZkey::PilFflonkZkey *zkeyFflonk, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, G1PointAffine *PTau, bool multiExp);

        void open(PilFflonkZkey::PilFflonkZkey *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials, G1PointAffine *PTau, FrElement previousChallenge);

    protected:
        void computeR(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void computeZT(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void computeL(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void computeZTS2(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void computeW(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void computeWp(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void computeChallengeXiSeed(FrElement previousChallenge);

        void computeChallengeAlpha();
        
        void computeChallengeY(G1Point W);

        void calculateRoots(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void getMontgomeryBatchedInverse(PilFflonkZkey::PilFflonkZkey *zkeyFflonk);

        void calculateEvaluations(PilFflonkZkey::PilFflonkZkey *zkeyFflonk, std::map<std::string, Polynomial<AltBn128::Engine> *> polynomials);

        void prepareCommits(PilFflonkZkey::PilFflonkZkey *zkeyFflonk, std::map<std::string, ShPlonkCommit *> commits);

        AltBn128::G1Point sumCommits(u_int32_t nCommits, G1Point *commits);

        Polynomial<AltBn128::Engine>* sumPolynomials(u_int32_t nPols, Polynomial<AltBn128::Engine>** polynomials);

        int find(std::string* arr, u_int32_t n, std::string x);
        int find(u_int32_t* arr, u_int32_t n, u_int32_t x);

    };
}

#endif