#ifndef PILFFLONK_SETUP_HPP
#define PILFFLONK_SETUP_HPP

#include <iostream>
#include <string.h>
#include <binfile_utils.hpp>
#include <binfile_writer.hpp>
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
#include "zkey_pilfflonk.hpp"
#include "fflonk_info.hpp"

using json = nlohmann::json;

using namespace std;

namespace PilFflonk
{
    class PilFflonkSetup
    {
        using FrElement = typename AltBn128::Engine::FrElement;
        using G1Point = typename AltBn128::Engine::G1Point;
        using G1PointAffine = typename AltBn128::Engine::G1PointAffine;

        AltBn128::Engine &E;

        PilFflonkZkey::PilFflonkZkey* zkey;
        FflonkInfo* fflonkInfo;

        void parseShKey(json shKeyJson);
        void parseFShKey(json shKeyJson);
        void parsePolsNamesStageShKey(json shKeyJson);
        void parseOmegasShKey(json shKeyJson);

        void parseFflonkInfo(json fflonkInfoJson);

        FrElement* constPolsEvals;
        uint64_t constPolsEvalsSize;
        FrElement* constPolsEvalsExt;
        uint64_t constPolsEvalsExtSize;
        FrElement* constPolsCoefs;
        uint64_t constPolsCoefsSize;

        FrElement* x_n;
        FrElement* x_2ns;

        G1PointAffine *PTau;

        FFT<AltBn128::Engine::Fr> *fft = NULL;

        NTT_AltBn128 *ntt;
        NTT_AltBn128 *nttExtended;

        std::tuple<FrElement*, uint64_t> loadFromFileFr(std::string filename);

        FrElement* polynomialFromMontgomery(Polynomial<AltBn128::Engine> *polynomial);

        G1Point multiExponentiation(Polynomial<AltBn128::Engine> *polynomial);

        Polynomial<AltBn128::Engine>* getPolFromBuffer(FrElement* buff, uint32_t nPols, uint64_t N, int32_t id);

    public:
        PilFflonkSetup(AltBn128::Engine &_E) : E(_E) {};

        ~PilFflonkSetup();

        void generateZkey(std::string shKeyFilename, std::string fflonkInfoFileName,
                          std::string pTauFilename, std::string cnstPolsFilename,
                          std::string cnstPolsExtFilename, std::string zkeyFilename);

    };
}

#endif
