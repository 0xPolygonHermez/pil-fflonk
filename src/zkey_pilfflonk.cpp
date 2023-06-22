#include <stdexcept>

#include "zkey_pilfflonk.hpp"

namespace Zkey {
    PilFflonkZkeyHeader::PilFflonkZkeyHeader() {
        this->protocolId = Zkey::FFLONK_PROTOCOL_ID;
    }

    PilFflonkZkeyHeader::~PilFflonkZkeyHeader() {
        mpz_clear(qPrime);
        mpz_clear(rPrime);
    }

    PilFflonkZkeyHeader* PilFflonkZkeyHeader::loadPilFflonkZkeyHeader(BinFileUtils::BinFile *f) {
        auto pilFflonkZkeyHeader = new PilFflonkZkeyHeader();

        f->startReadSection(Zkey::ZKEY_FF_HEADER_SECTION);

        pilFflonkZkeyHeader->n8q = f->readU32LE();
        mpz_init(pilFflonkZkeyHeader->qPrime);
        mpz_import(pilFflonkZkeyHeader->qPrime, pilFflonkZkeyHeader->n8q, -1, 1, -1, 0, f->read(pilFflonkZkeyHeader->n8q));

        pilFflonkZkeyHeader->n8r = f->readU32LE();
        mpz_init(pilFflonkZkeyHeader->rPrime);
        mpz_import(pilFflonkZkeyHeader->rPrime, pilFflonkZkeyHeader->n8r, -1, 1, -1, 0, f->read(pilFflonkZkeyHeader->n8r));

        pilFflonkZkeyHeader->power = f->readU32LE();
        pilFflonkZkeyHeader->powerZK = f->readU32LE();
        pilFflonkZkeyHeader->powerW = f->readU32LE();
        pilFflonkZkeyHeader->nPublics = f->readU32LE();

        pilFflonkZkeyHeader->X2 = f->read(pilFflonkZkeyHeader->n8q * 4);

        f->endReadSection();

        f->startReadSection(Zkey::ZKEY_PF_F_SECTION);

        u_int32_t lenF = f->readU32LE();

        pilFflonkZkeyHeader->f = new std::map<u_int32_t, shPlonkPol *>();
        for(uint32_t i = 0; i < lenF; i++) {
            shPlonkPol fi;

            fi.index = f->readU32LE();
            fi.degree = f->readU32LE();

            fi.nOpeningPoints = f->readU32LE();
            fi.openingPoints = new u_int32_t[fi.nOpeningPoints];
            for(uint32_t j = 0; j < fi.nOpeningPoints; j++) {
                fi.openingPoints[j] = f->readU32LE();
            }

            fi.nPols = f->readULE32();
            fi.pols = new u_int32_t[fi.nPols];
            for(uint32_t j = 0; j < fi.nPols; j++) {
                fi.pols[j] = f->readString();
            }

            fi.nStages = f->readU32LE();
            for(uint32_t j = 0; j < fi.nStages; j++) {
                shPlonkStage stage;
                stage.stage = f->readU32LE();
                stage.nPols = f->readU32LE();
                stage.pols = new u_int32_t[stage.nPols];
                for(uint32_t k = 0; k < stage.nPols; k++) {
                    shPlonkStagePol pol;
                    pol.name = f->readString();
                    pol.degree = f->readU32LE();

                    stage.pols[k] = pol;
                }
                fi.stages[j] = stage;
            }

            (*pilFflonkZkeyHeader->f)[fi.index] = fi;
        }   

        f->endReadSection();

        f->startReadSection(Zkey::ZKEY_PF_F_COMMITMENTS_SECTION);

        u_int32_t lenCommits = f->readU32LE();

        pilFflonkZkeyHeader->committedConstants = new std::map<std::string, FrElement *>();
        for(u_int32_t i = 0; i < lenCommits; ++i) {
            std::string name = f->readString();
            (*pilFflonkZkeyHeader->committedConstants)[name] = f->read(pilFflonkZkeyHeader->n8q * 2);
        }

        f->endReadSection();

        f->startReadSection(Zkey::ZKEY_PF_POLSMAP_SECTION);

        pilFflonkZkeyHeader->polsMap = new std::map<std::string, std::map< u_int32_t,std::string> >();

        u_int32_t lenPolsMap = f->readU32LE();

        for(u_int32_t i = 0; i < lenPolsMap; ++i) {
           std::string name = f->readString();
            (*pilFflonkZkeyHeader->polsMap)[name] = new std::map<u_int32_t, std::string>();

            u_int32_t lenSubkeys = f->readU32LE();
            for(u_int32_t j = 0; j < lenSubkeys; ++j) {
                u_int32_t index = f->readU32LE();
                (*pilFflonkZkeyHeader->polsMap)[name][index] = f->readString();
            }
        }

        f->endReadSection();

        f->startReadSection(Zkey::ZKEY_PF_POLSOPENINGS_SECTION);

        u_int32_t lenPolsOpenings = f->readU32LE();

        pilFflonkZkeyHeader->polsOpenings = new std::map<std::string, u_int32_t >();
        for(u_int32_t i = 0; i < lenPolsOpenings; ++i) {
            std::string name = f->readString();
            u_int32_t open = f->readU32LE();

            (*pilFflonkZkeyHeader->polsOpenings)[name] = open;            
        }

        f->endReadSection();

        f->startReadSection(Zkey::ZKEY_PF_POLSNAMESSTAGE_SECTION);

        const lenPolsNamesStage = f->readU32LE();
        pilFflonkZkeyHeader->polsNamesStage = new std::map<u_int32_t, std::string* >();

        for(u_int32_t i = 0; i < lenPolsNamesStage; ++i) {
            u_int32_t stage = f->readU32LE();

            u_int32_t lenPolsStage = f->readU32LE();
            for(u_int32_t j = 0; j < lenPolsStage; ++j) {
                (*pilFflonkZkeyHeader->polsNamesStage)[stage][j] = f->readString();
            }        
        }

        f->endReadSection();

        f->startReadSection(Zkey::ZKEY_PF_OMEGAS_SECTION);

        const lenOmegas = f->readU32LE();

        pilFflonkZkeyHeader->omegas = new std::map<std::string, u_int32_t >();
        for(u_int32_t i = 0; i < lenOmegas; ++i) {
            std::string name = f->readString();
            FrElement omega = f->read(pilFflonkZkeyHeader->n8q);
            (*pilFflonkZkeyHeader->omegas)[name] = omega;
        }


        f->endReadSection();

        return pilFflonkZkeyHeader;
    }

    
}
