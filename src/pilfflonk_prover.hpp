#ifndef PILFFLONK_PROVER_HPP
#define PILFFLONK_PROVER_HPP

#include <iostream>
#include <string.h>
#include <binfile_utils.hpp>
#include <nlohmann/json.hpp>
#include "compare_fe.hpp"
#include <sodium.h>
#include "zkey_pilfflonk.hpp"
#include "shplonk.hpp"
#include "polynomial/polynomial.hpp"
#include "zkey.hpp"
#include "fflonk_info.hpp"
#include "keccak_256_transcript.hpp"
#include "chelpers/pilfflonk_steps.hpp"
#include "ntt_bn128.hpp"
#include <alt_bn128.hpp>
#include "fft.hpp"
#include "utils.hpp"

using json = nlohmann::json;

using namespace std;

namespace PilFflonk
{
    struct BinFilePolsData
    {
        u_int64_t n;
        u_int32_t nPols;
        string *names;
        AltBn128::FrElement *buffer;
    };

    class PilFflonkProver
    {
        using FrElement = typename AltBn128::Engine::FrElement;
        using G1Point = typename AltBn128::Engine::G1Point;
        using G1PointAffine = typename AltBn128::Engine::G1PointAffine;

        AltBn128::Engine &E;
        std::string curveName;

        FrElement *reservedMemoryPtr;
        uint64_t reservedMemorySize;

        FFT<AltBn128::Engine::Fr> *fft = NULL;

        NTT_AltBn128 *ntt;
        NTT_AltBn128 *nttExtended;

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

        u_int64_t lengthBufferCommitted;
        u_int64_t lengthBufferConstant;
        u_int64_t lengthBufferShPlonk;

        std::map<std::string, u_int64_t> mapBufferCommitted;
        std::map<std::string, u_int64_t> mapBufferConstant;
        std::map<std::string, u_int64_t> mapBufferShPlonk;

        FflonkInfo *fflonkInfo;

        ShPlonk::ShPlonkProver *shPlonkProver;

        Keccak256Transcript *transcript;

        G1PointAffine *PTau;

        PilFflonkSteps pilFflonkSteps;

        FrElement *bBufferCommitted;
        FrElement *bBufferConstant;
        FrElement *bBufferShPlonk;

        std::map<std::string, AltBn128::FrElement *> ptrCommitted;
        std::map<std::string, AltBn128::FrElement *> ptrConstant;
        std::map<std::string, AltBn128::FrElement *> ptrShPlonk;

    public:
        PilFflonkProver(AltBn128::Engine &E,
                        std::string zkeyFilename, std::string fflonkInfoFilename,
                        std::string constPolsFilename, std::string precomputedFilename,
                        void *reservedMemoryPtr = NULL, uint64_t reservedMemorySize = 0);

        ~PilFflonkProver();

        // Set the configuration data that is required once per prover
        void setConstantData(std::string zkeyFilename, std::string fflonkInfoFilename,
                             std::string constPolsFilename, std::string precomputedFilename);

        std::tuple<json, json> prove(std::string committedPolsFilename);

    protected:
        void stage1(StepsParams &params);

        void stage2(StepsParams &params);

        void stage3(StepsParams &params);

        void stage4(StepsParams &params);

        void extend(u_int32_t stage, u_int32_t nPols);

        void addCoefsToContext(u_int32_t stage, u_int32_t nPols, AltBn128::FrElement *buffCoefs);

        AltBn128::FrElement *getPolynomial(uint64_t polId, uint64_t offset);

        void calculateZ(AltBn128::FrElement *pNum, AltBn128::FrElement *pDen, uint64_t id);

        AltBn128::FrElement *batchInverse(AltBn128::FrElement *pol, uint64_t N);

        void calculateH1H2(AltBn128::FrElement *fPol, AltBn128::FrElement *tPol, uint64_t h1Id, uint64_t h2Id);
    };
}

#endif
