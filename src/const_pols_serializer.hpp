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
    static void readConstPolsCoefsSection(BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);

    static void readConstPolsEvalsExtSection(BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);

    static AltBn128::FrElement *readBuffer(BinFileUtils::BinFile *fd, int idSection);

public:
    AltBn128::FrElement *coefs;
    AltBn128::FrElement *evalsExt;

    ~ConstPolsSerializer();

    static ConstPolsSerializer *readConstPolsFile(BinFileUtils::BinFile *fd);
};

#endif
