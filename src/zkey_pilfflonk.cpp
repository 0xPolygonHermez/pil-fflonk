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
        pilFflonkZkey->powerW = fdZKey->readU32LE();
        pilFflonkZkey->nPublics = fdZKey->readU32LE();

        pilFflonkZkey->X2 = fdZKey->read(pilFflonkZkey->n8q * 4);

        fdZKey->endReadSection();

        readFSection(fdZKey, pilFflonkZkey);

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

    void readPolsNamesStageSection(BinFileUtils::BinFile *fdZKey, PilFflonkZkey *pilFflonkZkey)
    {
        fdZKey->startReadSection(ZKEY_PF_POLSNAMESSTAGE_SECTION);

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

    void readConstPols(BinFileUtils::BinFile *fdZKey, AltBn128::FrElement *evals, AltBn128::FrElement *coefs, AltBn128::FrElement *evalsExt, AltBn128::FrElement *x_n, AltBn128::FrElement *x_2ns){
        
        readConstPolsEvalsSection(fdZKey, evals);

        readConstPolsCoefsSection(fdZKey, coefs);

        readConstPolsEvalsExtSection(fdZKey, evalsExt);

        readXnSection(fdZKey, x_n);

        readX2nsSection(fdZKey, x_2ns);
    }

    void readConstPolsEvalsSection(BinFileUtils::BinFile *fdZKey, AltBn128::FrElement *evals)
    {
        readBuffer(fdZKey, ZKEY_PF_CONST_POLS_EVALS_SECTION, evals);
    }

    void readConstPolsCoefsSection(BinFileUtils::BinFile *fdZKey, AltBn128::FrElement *coefs)
    {
        readBuffer(fdZKey, ZKEY_PF_CONST_POLS_COEFS_SECTION, coefs);
    }


    void readConstPolsEvalsExtSection(BinFileUtils::BinFile *fdZKey, AltBn128::FrElement *evalsExt)
    {
        readBuffer(fdZKey, ZKEY_PF_CONST_POLS_EVALS_EXT_SECTION, evalsExt);
    }


    void readXnSection(BinFileUtils::BinFile *fdZKey, AltBn128::FrElement *x_n)
    {
        readBuffer(fdZKey, ZKEY_PF_X_N_SECTION, x_n);
    }


    void readX2nsSection(BinFileUtils::BinFile *fdZKey, AltBn128::FrElement *x_2ns)
    {
        readBuffer(fdZKey, ZKEY_PF_X_EXT_SECTION, x_2ns);
    }

    void readBuffer(BinFileUtils::BinFile *fdZKey, int idSection, AltBn128::FrElement *ptrDst)
    {
        uint64_t size = fdZKey->getSectionSize(idSection);

        AltBn128::FrElement *buffer = ptrDst;

        ThreadUtils::parcpy(&buffer[0], (FrElement *)fdZKey->getSectionData(idSection), size, omp_get_num_threads() / 2);
    }
}