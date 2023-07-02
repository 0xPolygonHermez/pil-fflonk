#ifndef PILFFLONK_PROVER_HPP
#define PILFFLONK_PROVER_HPP

#include <iostream>
#include <string.h>
#include <binfile_utils.hpp>
#include <nlohmann/json.hpp>
#include <sodium.h>
#include "zkey_pilfflonk.hpp"
#include "shplonk.hpp"
#include "polynomial/polynomial.hpp"
#include "polinomial.hpp"
#include "zkey.hpp"
#include "steps.hpp"
#include "snark_proof.hpp"
#include "fflonk_info.hpp"
#include "keccak_256_transcript.hpp"
#include "commit_pols_fflonk.hpp"
#include "constant_pols_fflonk.hpp"
#include "chelpers/pilfflonk_steps.hpp"
#include "ntt_bn128.hpp"
#include <alt_bn128.hpp>
#include "fft.hpp"
#include "utils.hpp"

using json = nlohmann::json;

using namespace std;

namespace PilFflonk {
    struct BinFilePolsData{
        u_int64_t  n;
        u_int32_t  nPols;
        string*    names;
        AltBn128::FrElement* buffer;
    };

    class PilFflonkProver {
        using FrElement = typename AltBn128::Engine::FrElement;
        using G1Point = typename AltBn128::Engine::G1Point;
        using G1PointAffine = typename AltBn128::Engine::G1PointAffine;

        AltBn128::Engine &E;
        std::string curveName;


        FFT<AltBn128::Engine::Fr> *fft = NULL;

        PilFflonkZkey::PilFflonkZkey *zkey;

        u_int32_t n8r;
        u_int64_t N;
        u_int64_t NExt;
        u_int32_t nBits;
        u_int32_t nBitsExt;
        u_int32_t extendBits;
        u_int64_t sDomain;
        u_int64_t sDomainExt;

        u_int32_t extendBitsZK;
        u_int32_t factorZK;
        u_int32_t extendBitsTotal;
        u_int32_t nBitsExtZK;

        FrElement challenges[5];

        FflonkInfo* fflonkInfo;

        BinFilePolsData* cnstPols;
        BinFilePolsData* cmtdPols;


        void *pConstPolsAddress;
        void *pConstPolsAddress2ns;
        ConstantPolsFflonk* pConstPols;
        ConstantPolsFflonk* pConstPols2ns;
        
        uint64_t constPolsSize;

        void *pCommittedPolsAddress;

        ShPlonk::ShPlonkProver* shPlonkProver;

        PilFflonkSteps pilFflonkSteps;

        u_int64_t lengthBufferCommitted;
        u_int64_t lengthBuffer;

        FrElement *bBufferCommitted;
        FrElement *bBuffer;
        G1PointAffine *PTau;

        std::map<std::string, AltBn128::FrElement *> ptrCommitted;
        std::map<std::string, AltBn128::FrElement *> ptr;

        StepsParams params;

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

        // std::map<std::string, u_int32_t *> mapBuffers;
        // std::map<std::string, FrElement *> buffers;
        // std::map<std::string, Polynomial<Engine> *> polynomials;
        // std::map<std::string, Evaluations<Engine> *> evaluations;

        // std::map <std::string, FrElement> toInverse;
        // std::map <std::string, FrElement> challenges;      
        // std::map<std::string, FrElement *> roots;
        // FrElement blindingFactors[BLINDINGFACTORSLENGTH];

        Keccak256Transcript *transcript;
        SnarkProof *proof;
    public:
        PilFflonkProver(AltBn128::Engine &E, std::string fflonkInfoFile);
        PilFflonkProver(AltBn128::Engine &E, std::string fflonkInfoFile, void* reservedMemoryPtr, uint64_t reservedMemorySize);

        ~PilFflonkProver();

        void setZkey(BinFileUtils::BinFile *fdZkey, std::string constPolsFilename);

        /*tuple <json, json>*/ void prove(BinFileUtils::BinFile *fdZkey, std::string constPolsFilename, std::string committedPolsFilename);
        /*tuple <json, json>*/ void prove(std::string committedPolsFilename);

    protected:
        void initialize(void* reservedMemoryPtr, uint64_t reservedMemorySize = 0);

        void removePrecomputedData();

        void stage1();

        void stage2();

        void stage3();

        void stage4();

        void extend(u_int32_t stage, u_int32_t nPols);
    };
}

#endif
