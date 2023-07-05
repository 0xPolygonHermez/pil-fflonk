#include <stdexcept>
#include "zkey_pilfflonk.hpp"

using namespace std;
namespace PilFflonkZkey
{
    PilFflonkZkey::~PilFflonkZkey()
    {
        mpz_clear(qPrime);
        mpz_clear(rPrime);

        f.clear();
        committedConstants.clear();

        for (auto& entry : polsNamesStage) {
            delete entry.second;
        }
        polsNamesStage.clear();

        omegas.clear();
    }

    PilFflonkZkey *loadPilFflonkZkey(BinFileUtils::BinFile *fdZKey)
    {
        auto pilFflonkZkey = new PilFflonkZkey();

        fdZKey->startReadSection(ZKEY_PF_HEADER_SECTION);

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

        readPolsNamesStageSection(fdZKey, pilFflonkZkey);

        readOmegasSection(fdZKey, pilFflonkZkey);

        return pilFflonkZkey;
    }

    void readFSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZKEY_PF_F_SECTION);

        u_int32_t lenF = fdZKey->readU32LE();
        for (uint32_t i = 0; i < lenF; i++)
        {
            ShPlonkPol *fi = new ShPlonkPol();
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
            fi->stages = new ShPlonkStage[fi->nStages];
            for (uint32_t j = 0; j < fi->nStages; j++)
            {
                ShPlonkStage *stage = &(fi->stages[j]);
                stage->stage = fdZKey->readU32LE();
                stage->nPols = fdZKey->readU32LE();
                stage->pols = new ShPlonkStagePol[stage->nPols];
                for (uint32_t k = 0; k < stage->nPols; k++)
                {
                    ShPlonkStagePol *pol = &(stage->pols[k]);
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
        fdZKey->startReadSection(ZKEY_PF_F_COMMITMENTS_SECTION);
        u_int32_t len = fdZKey->readU32LE();

        for (u_int32_t i = 0; i < len; i++)
        {
            std::string name = fdZKey->readString();
            pilFflonkZkey->committedConstants[name] = fdZKey->read(pilFflonkZkey->n8q * 2);
        }

        fdZKey->endReadSection();
    }

    void readPolsNamesStageSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZKEY_PF_POLSNAMESSTAGE_SECTION);

        uint32_t len = fdZKey->readU32LE();

        for (u_int32_t i = 0; i < len; ++i)
        {
            u_int32_t stage = fdZKey->readU32LE();

            pilFflonkZkey->polsNamesStage[stage] = new std::map<u_int32_t, PolsNames>();

            u_int32_t lenPolsStage = fdZKey->readU32LE();

            for (u_int32_t j = 0; j < lenPolsStage; ++j)
            {
                (*(pilFflonkZkey->polsNamesStage[stage]))[j].name = fdZKey->readString();
                (*(pilFflonkZkey->polsNamesStage[stage]))[j].openings = fdZKey->readU32LE();
            }
        }

        fdZKey->endReadSection();
    }

    void readOmegasSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZKEY_PF_OMEGAS_SECTION);

        uint32_t len = fdZKey->readU32LE();

        for (u_int32_t i = 0; i < len; ++i)
        {
            std::string name = fdZKey->readString();
            AltBn128::FrElement omega = *(AltBn128::FrElement *)(fdZKey->read(pilFflonkZkey->n8q));
            pilFflonkZkey->omegas[name] = omega;
        }

        fdZKey->endReadSection();
    }
}
