#ifndef ZKEY_CONST_POLS_SERIALIZER_H
#define ZKEY_CONST_POLS_SERIALIZER_H

#include <string>
#include <map>
#include <gmp.h>
#include <binfile_utils.hpp>
#include <alt_bn128.hpp>

const int CONST_POLS_NSECTIONS = 4;
const int CONST_POLS_FILE_COEFS_SECTION = 1;
const int CONST_POLS_FILE_EVALS_EXT_SECTION = 2;
const int X_N_SECTION = 3;
const int X_2NS_SECTION = 4;


class ConstPolsSerializer
{   
    static void readXnSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);
    static void readXnSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs);

    static void readX2nsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);
    static void readX2nsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs);

    static void readConstPolsCoefsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);
    static void readConstPolsCoefsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs);

    static void readConstPolsEvalsExtSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer);
    static void readConstPolsEvalsExtSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *evalsExt);

    static AltBn128::FrElement *readBuffer(AltBn128::Engine &E, BinFileUtils::BinFile *fd, int idSection);
    static void readBuffer(AltBn128::Engine &E, BinFileUtils::BinFile *fd, int idSection, AltBn128::FrElement *ptrDst);

public:
    AltBn128::FrElement *coefs;
    AltBn128::FrElement *evalsExt;
    AltBn128::FrElement *x_n;
    AltBn128::FrElement *x_2ns;

    ~ConstPolsSerializer();

    static ConstPolsSerializer *readConstPolsFile(AltBn128::Engine &E, BinFileUtils::BinFile *fd);
    static void readConstPolsFile(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs, AltBn128::FrElement *evalsExt, AltBn128::FrElement *x_n, AltBn128::FrElement *x_2ns);
};

#endif
