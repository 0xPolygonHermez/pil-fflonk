#ifndef PILFFLONK_PROVER_HPP
#define PILFFLONK_PROVER_HPP

#include <iostream>
#include <string.h>
#include <binfile_utils.hpp>
#include <nlohmann/json.hpp>
#include "zkey_pilfflonk.hpp"
#include "polynomial/polynomial.hpp"
#include "zkey.hpp"
#include "snark_proof.hpp"
#include "keccak_256_transcript.hpp"
#include <alt_bn128.hpp>
#include "fft.hpp"

using json = nlohmann::json;

using namespace std;

namespace PilFflonk {

    struct Ctx {
        u_int64_t N;
        u_int64_t Next;
        u_int32_t nBits;
        u_int32_t nBitsExt;
        u_int32_t extendBits;

        std::map<std::string, FrElement> challenges;
    };

    class PilFflonkProver {
        AltBn128::Engine &E;
        FFT<AltBn128::Engine::Fr> *fft = NULL;

        ZkeyPilFflonk::PilFflonkZkey *zkey;
        u_int32_t zkeyPower;
        std::string curveName;
        size_t sDomain;

        FrElement *reservedMemoryPtr;
        uint64_t reservedMemorySize;

        Ctx ctx;

        // FrElement *precomputedBigBuffer;
        // G1PointAffine *PTau;

        // u_int64_t lengthNonPrecomputedBigBuffer;
        // FrElement *nonPrecomputedBigBuffer;

        // u_int32_t *mapBuffersBigBuffer;

        // FrElement *buffInternalWitness;
        // FrElement *buffWitness;

        // Zkey::Addition<Engine> *additionsBuff;

        // u_int64_t lengthBatchInversesBuffer;

        // FrElement *inverses;
        // FrElement *products;

        // // This is the length of the buffer that must be zeroed after each proof (starting from buffers["A"] pointer)
        // u_int64_t buffersLength;

        // std::map<std::string, FrElement *> polPtr;
        // std::map<std::string, FrElement *> evalPtr;

        // std::map<std::string, u_int32_t *> mapBuffers;
        // std::map<std::string, FrElement *> buffers;
        // std::map<std::string, Polynomial<Engine> *> polynomials;
        // std::map<std::string, Evaluations<Engine> *> evaluations;

        // std::map <std::string, FrElement> toInverse;
        // std::map <std::string, FrElement> challenges;
        // std::map<std::string, FrElement *> roots;
        // FrElement blindingFactors[BLINDINGFACTORSLENGTH];

        // Keccak256Transcript *transcript;
        SnarkProof *proof;
    public:
        PilFflonkProver(AltBn128::Engine &E);
        PilFflonkProver(AltBn128::Engine &E, void* reservedMemoryPtr, uint64_t reservedMemorySize);

        ~PilFflonkProver();

        void setZkey(BinFileUtils::BinFile *fdZkey);

        /*tuple <json, json>*/ void prove(BinFileUtils::BinFile *fdZkey);
        /*tuple <json, json>*/ void prove();

    protected:
        void initialize(void* reservedMemoryPtr, uint64_t reservedMemorySize = 0);

        void removePrecomputedData();

        // void calculateAdditions();

        // FrElement getWitness(u_int64_t idx);

        // void round1();

        // void round2();

        // void round3();

        // void round4();

        // void round5();

        // //ROUND 1 functions
        // void computeWirePolynomials();

        // void computeWirePolynomial(std::string polName, FrElement blindingFactors[]);

        // void computeT0();

        // void computeC1();

        // //ROUND 2 functions
        // void computeZ();

        // void computeT1();

        // void computeT2();

        // void computeC2();

        // //ROUND 4 functions
        // void computeR0();

        // void computeR1();

        // void computeR2();

        // void computeF();

        // void computeZT();

        // //ROUND 5 functions
        // void computeL();

        // void computeZTS2();

        // void batchInverse(FrElement *elements, u_int64_t length);

        // FrElement *polynomialFromMontgomery(Polynomial<Engine> *polynomial);

        // FrElement getMontgomeryBatchedInverse();

        // void computeLiS0();

        // void computeLiS1();

        // void computeLiS2();

        // G1Point multiExponentiation(Polynomial<Engine> *polynomial);

        // G1Point multiExponentiation(Polynomial<Engine> *polynomial, u_int32_t nx, u_int64_t x[]);
    };
}

#endif