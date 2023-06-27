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
    class ShPlonkProver {

        AltBn128::Engine &E;

        using FrElement = typename AltBn128::FrElement;
        using G1Point = typename AltBn128::G1Point;
        using G1PointAffine = typename AltBn128::G1PointAffine;

        PilFflonkZkey::PilFflonkZkey *zkeyPilFflonk;
        
        Keccak256Transcript *transcript;

        FrElement challengeXiSeed;
        FrElement challengeXi;
        FrElement challengeAlpha;
        FrElement challengeY;

        std::map<std::string, FrElement *> rootsMap;

        std::map<std::string, AltBn128::FrElement> evaluationCommitments;

        std::map <std::string, AltBn128::G1Point> nonConstantCommits;
    public:
        std::map <std::string, AltBn128::G1Point> polynomialCommitments;

        std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsShPlonk;

        ShPlonkProver(AltBn128::Engine &_E, BinFileUtils::BinFile *zkeyBinfile);
        
        void commit(u_int32_t stage, G1PointAffine *PTau, bool multiExp);

        void open(G1PointAffine *PTau, FrElement previousChallenge);

    protected:
        void computeR();

        void computeZT();

        void computeL();

        void computeZTS2();

        void computeW();

        void computeWp();

        void computeChallengeXiSeed(FrElement previousChallenge);

        void computeChallengeAlpha();
        
        void computeChallengeY(G1Point W);

        void calculateRoots();

        void getMontgomeryBatchedInverse();

        void calculateEvaluations();

        void prepareCommits();

        AltBn128::G1Point sumCommits(u_int32_t nCommits, G1Point *commits);

        Polynomial<AltBn128::Engine>* sumPolynomials(u_int32_t nPols, Polynomial<AltBn128::Engine>** polynomials);

        int find(std::string* arr, u_int32_t n, std::string x);
        int find(u_int32_t* arr, u_int32_t n, u_int32_t x);

        AltBn128::G1Point multiExponentiation(G1PointAffine *PTau, Polynomial<AltBn128::Engine> *polynomial, u_int32_t nx, u_int64_t x[]);
        
        FrElement *polynomialFromMontgomery(Polynomial<AltBn128::Engine> *polynomial);

    };
}

#endif