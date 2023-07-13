#ifndef SHPLONK_HPP
#define SHPLONK_HPP

#include <iostream>
#include <string.h>
#include <vector>
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

        std::map<std::string, Polynomial<AltBn128::Engine> *> polynomialsShPlonk;

        std::map<std::string, AltBn128::FrElement> evaluationCommitments;
        
        std::vector<u_int32_t> openingPoints;

        std::vector<FrElement> inverseElements;

    public:
        std::map <std::string, AltBn128::G1Point> polynomialCommitments;

        void addPolynomialShPlonk(const std::string &key, Polynomial<AltBn128::Engine>* pol);
        
        Polynomial<AltBn128::Engine> * getPolynomialShPlonk(const std::string &key);

        ShPlonkProver(AltBn128::Engine &_E, PilFflonkZkey::PilFflonkZkey *zkey);

        ~ShPlonkProver();

        void reset();

        void commit(u_int32_t stage, G1PointAffine *PTau, std::map<std::string, AltBn128::FrElement *> ptrShPlonk, bool multiExp);

        json open(G1PointAffine *PTau, std::map<std::string, AltBn128::FrElement *> ptrShPlonk, FrElement challengeXiSeed);

        json toJson();

        FrElement getChallengeXi();
    protected:
        void computeR();

        void computeZT();

        void computeL(AltBn128::FrElement *reservedBuffer, AltBn128::FrElement *tmpBuffer);

        void computeZTS2();

        void computeW(AltBn128::FrElement *reservedBuffer, AltBn128::FrElement *tmpBuffer);

        void computeWp(AltBn128::FrElement *reservedBuffer, AltBn128::FrElement *tmpBuffer);

        void computeChallengeXiSeed(FrElement previousChallenge);

        void computeChallengeAlpha();
        
        void computeChallengeY(G1Point W);

        void calculateOpeningPoints();

        void calculateRoots();

        void getMontgomeryBatchedInverse();

        void computeLiSingleOpeningPoint(u_int32_t i);

        void computeLiMultipleOpeningPoints(u_int32_t i);

        void computeLiTwoOpeningPoints(u_int32_t i);

        void calculateEvaluations();

        void prepareCommits(std::map<std::string, AltBn128::FrElement *> ptrShPlonk);

        void sumCommits(u_int32_t nCommits, std::string* polynomialsNames, std::string dstName);

        void sumPolynomials(u_int32_t nPols, std::string* polynomialsNames, std::map<std::string, AltBn128::FrElement *> ptrShPlonk, std::string dstName);

        int find(std::string* arr, u_int32_t n, std::string x);
        int find(u_int32_t* arr, u_int32_t n, u_int32_t x);

        AltBn128::G1Point multiExponentiation(G1PointAffine *PTau, Polynomial<AltBn128::Engine> *polynomial, u_int32_t nx, u_int64_t x[]);
        
        FrElement *polynomialFromMontgomery(Polynomial<AltBn128::Engine> *polynomial);

    };
}

#endif
