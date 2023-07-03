#ifndef ZKEY_CONST_POLS_SERIALIZER_H
#define ZKEY_CONST_POLS_SERIALIZER_H

#include <string>
#include <map>
#include <gmp.h>
#include <binfile_utils.hpp>
#include <alt_bn128.hpp>

const int CONST_POLS_NSECTIONS = 2;
const int CONST_POLS_FILE_COEFS_SECTION = 1;
const int CONST_POLS_FILE_EVALS_EXT_SECTION = 2;

class ConstPolsSerializer
{
    static void readConstPolsCoefsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);
    static void readConstPolsCoefsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs);

    static void readConstPolsEvalsExtSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);
    static void readConstPolsEvalsExtSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *evalsExt);

    static AltBn128::FrElement *readBuffer(AltBn128::Engine &E, BinFileUtils::BinFile *fd, int idSection);
    static void readBuffer(AltBn128::Engine &E, BinFileUtils::BinFile *fd, int idSection, AltBn128::FrElement *ptrDst);

public:
    AltBn128::FrElement *coefs;
    AltBn128::FrElement *evalsExt;

    ~ConstPolsSerializer();

    static ConstPolsSerializer *readConstPolsFile(AltBn128::Engine &E, BinFileUtils::BinFile *fd);
    static void readConstPolsFile(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs, AltBn128::FrElement *evalsExt);
};

#endif
