#ifndef ZKEY_PILFFLONK_H
#define ZKEY_PILFFLONK_H

#include <string>
#include <map>
#include <gmp.h>
#include <binfile_utils.hpp>
#include "zkey.hpp"
#include <alt_bn128.hpp>

namespace ZkeyPilFflonk
{
    const int ZKEY_PF_NSECTIONS = 9;

    const int ZKEY_PF_HEADER_SECTION = 2;
    const int ZKEY_PF_F_SECTION = 3;
    const int ZKEY_PF_OPENINGPOINTS_SECTION = 4; 
    const int ZKEY_PF_F_COMMITMENTS_SECTION = 5;
    const int ZKEY_PF_POLSMAP_SECTION = 6;
    const int ZKEY_PF_POLSOPENINGS_SECTION = 7;
    const int ZKEY_PF_POLSNAMESSTAGE_SECTION = 8;
    const int ZKEY_PF_OMEGAS_SECTION = 9;
    const int ZKEY_PF_PTAU_SECTION = 10;

    struct ShPlonkStagePol
    {
        std::string name;
        u_int32_t degree;
    };

    struct ShPlonkStage
    {
        u_int32_t stage;
        u_int32_t nPols;
        ShPlonkStagePol *pols;
    };

    struct ShPlonkPol
    {
        uint32_t index;
        uint32_t degree;
        uint32_t nOpeningPoints;
        uint32_t *openingPoints;
        uint32_t nPols;
        std::string *pols;
        uint32_t nStages;
        ShPlonkStage *stages;
    };

    class PilFflonkZkey
    {
    public:
        u_int32_t n8q;
        mpz_t qPrime;
        u_int32_t n8r;
        mpz_t rPrime;

        u_int32_t power;
        u_int32_t powerZK;
        u_int32_t powerW;
        u_int32_t nPublics;

        void *X2;

        std::map<u_int32_t, ShPlonkPol *> f;

        std::map<u_int32_t, u_int32_t> openingPoints;

        std::map<std::string, void *> committedConstants;

        std::map<std::string, std::map<u_int32_t, std::string>*> polsMap;

        std::map<std::string, u_int32_t> polsOpenings;

        std::map<u_int32_t, std::map<u_int32_t, std::string>*> polsNamesStage;

        std::map<std::string, FrElement> omegas;

        ~PilFflonkZkey();
    };

    PilFflonkZkey *loadPilFflonkZkey(BinFileUtils::BinFile *f);

    void readFSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey);

    void readOpeningPoints(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey);

    void readFCommitmentsSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey);

    void readPolsMapSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey);

    void readPolsOpeningsSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey);

    void readPolsNamesStageSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey);

    void readOmegasSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey);
}

#endif
