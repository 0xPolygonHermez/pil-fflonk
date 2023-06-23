#include <stdexcept>
#include "zkey_pilfflonk.hpp"

using namespace std;
namespace ZkeyPilFflonk
{
    PilFflonkZkey::~PilFflonkZkey()
    {
        mpz_clear(qPrime);
        mpz_clear(rPrime);
    }

    PilFflonkZkey *loadPilFflonkZkey(BinFileUtils::BinFile *fdZKey)
    {
        auto pilFflonkZkey = new PilFflonkZkey();

        fdZKey->startReadSection(ZkeyPilFflonk::ZKEY_PF_HEADER_SECTION);

        pilFflonkZkey->n8q = fdZKey->readU32LE();
        mpz_init(pilFflonkZkey->qPrime);
        mpz_import(pilFflonkZkey->qPrime, pilFflonkZkey->n8q, -1, 1, -1, 0, fdZKey->read(pilFflonkZkey->n8q));

        pilFflonkZkey->n8r = fdZKey->readU32LE();
        mpz_init(pilFflonkZkey->rPrime);
        mpz_import(pilFflonkZkey->rPrime, pilFflonkZkey->n8r, -1, 1, -1, 0, fdZKey->read(pilFflonkZkey->n8r));

        pilFflonkZkey->power = fdZKey->readU32LE();
        pilFflonkZkey->powerZK = fdZKey->readU32LE();
        pilFflonkZkey->powerW = fdZKey->readU32LE();
        pilFflonkZkey->nPublics = fdZKey->readU32LE();

        pilFflonkZkey->X2 = fdZKey->read(pilFflonkZkey->n8q * 4);

        fdZKey->endReadSection();

        readFSection(fdZKey, pilFflonkZkey);

        readFCommitmentsSection(fdZKey, pilFflonkZkey);

        readPolsMapSection(fdZKey, pilFflonkZkey);

        readPolsOpeningsSection(fdZKey, pilFflonkZkey);

        readPolsNamesStageSection(fdZKey, pilFflonkZkey);

        readOmegasSection(fdZKey, pilFflonkZkey);

        return pilFflonkZkey;
    }

    void readFSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZkeyPilFflonk::ZKEY_PF_F_SECTION);

        u_int32_t lenF = fdZKey->readU32LE();
        for (uint32_t i = 0; i < lenF; i++)
        {
            shPlonkPol *fi = new shPlonkPol();
            fi->index = fdZKey->readU32LE();
            fi->degree = fdZKey->readU32LE();

            fi->nOpeningPoints = fdZKey->readU32LE();
            fi->openingPoints = new u_int32_t[fi->nOpeningPoints];
            for (uint32_t j = 0; j < fi->nOpeningPoints; j++)
            {
                fi->openingPoints[j] = fdZKey->readU32LE();
            }

            fi->nPols = fdZKey->readU32LE();

            fi->pols = new string[fi->nPols];
            for (uint32_t j = 0; j < fi->nPols; j++)
            {
                fi->pols[j] = fdZKey->readString();
            }

            fi->nStages = fdZKey->readU32LE();
            fi->stages = new shPlonkStage[fi->nStages];
            for (uint32_t j = 0; j < fi->nStages; j++)
            {
                shPlonkStage *stage = &(fi->stages[j]);
                stage->stage = fdZKey->readU32LE();
                stage->nPols = fdZKey->readU32LE();
                stage->pols = new shPlonkStagePol[stage->nPols];
                for (uint32_t k = 0; k < stage->nPols; k++)
                {
                    shPlonkStagePol *pol = &(stage->pols[k]);
                    pol->name = fdZKey->readString();
                    pol->degree = fdZKey->readU32LE();
                }
            }

            pilFflonkZkey->f[fi->index] = fi;
        }

        fdZKey->endReadSection();
    }

    void readFCommitmentsSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZkeyPilFflonk::ZKEY_PF_F_COMMITMENTS_SECTION);
        u_int32_t len = fdZKey->readU32LE();
        std::cout << "readFCommitmentsSection" << endl;
        std::cout << "len: " << len << endl;

        for (u_int32_t i = 0; i < len; i++)
        {
            std::string name = fdZKey->readString();
            pilFflonkZkey->committedConstants[name] = fdZKey->read(pilFflonkZkey->n8q * 2);
            std::cout << "name: " << name << endl;
            std::cout << "pilFflonkZkey->committedConstants[name]: " << len << endl;
        }

        fdZKey->endReadSection();
    }

    void readPolsMapSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZkeyPilFflonk::ZKEY_PF_POLSMAP_SECTION);

        u_int32_t len = fdZKey->readU32LE();

        for (u_int32_t i = 0; i < len; ++i)
        {
            std::string name = fdZKey->readString();
            pilFflonkZkey->polsMap[name] = new std::map<u_int32_t, std::string>();

            u_int32_t lenSubkeys = fdZKey->readU32LE();

            for (u_int32_t j = 0; j < lenSubkeys; ++j)
            {
                u_int32_t index = fdZKey->readU32LE();
                (*(pilFflonkZkey->polsMap[name]))[index] = fdZKey->readString();
            }
        }

        fdZKey->endReadSection();
    }

    void readPolsOpeningsSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZkeyPilFflonk::ZKEY_PF_POLSOPENINGS_SECTION);

        u_int32_t len = fdZKey->readU32LE();

        for (u_int32_t i = 0; i < len; ++i)
        {
            std::string name = fdZKey->readString();
            u_int32_t open = fdZKey->readU32LE();

            pilFflonkZkey->polsOpenings[name] = open;
        }

        fdZKey->endReadSection();
    }

    void readPolsNamesStageSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZkeyPilFflonk::ZKEY_PF_POLSNAMESSTAGE_SECTION);

        uint32_t len = fdZKey->readU32LE();

        for (u_int32_t i = 0; i < len; ++i)
        {
            u_int32_t stage = fdZKey->readU32LE();

            pilFflonkZkey->polsNamesStage[stage] = new std::map<u_int32_t, std::string>();

            u_int32_t lenPolsStage = fdZKey->readU32LE();

            for (u_int32_t j = 0; j < lenPolsStage; ++j)
            {
                (*(pilFflonkZkey->polsNamesStage[stage]))[j] = fdZKey->readString();
            }
        }

        fdZKey->endReadSection();
    }

    void readOmegasSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZkeyPilFflonk::ZKEY_PF_OMEGAS_SECTION);

        uint32_t len = fdZKey->readU32LE();

        for (u_int32_t i = 0; i < len; ++i)
        {
            std::string name = fdZKey->readString();
            FrElement omega = *(FrElement *)(fdZKey->read(pilFflonkZkey->n8q));
            pilFflonkZkey->omegas[name] = omega;
        }

        fdZKey->endReadSection();
    }
}
