// #ifndef ZKEY_CONST_POLS_SERIALIZER_H
// #define ZKEY_CONST_POLS_SERIALIZER_H

// #include <string>
// #include <map>
// #include <gmp.h>
// #include <binfile_utils.hpp>
// #include <alt_bn128.hpp>

// const CONST_POLS_NSECTIONS = 3;
// const CONST_POLS_FILE_COEFS_SECTION = 1;
// const CONST_POLS_FILE_EVALS_EXT_SECTION = 2;

// class ConstPolsSerializer
// {
// public:
//     AltBn128::FrElement *coefs;
//     AltBn128::FrElement *evalsExt;

//     ~ConstPolsSerializer();
// };

// PilFflonkZkey *readConstPolsFile(BinFileUtils::BinFile *fd);

// void readConstPolsCoefsSection(BinFileUtils::BinFile *fd);

// void readConstPolsEvalsExtSection(BinFileUtils::BinFile *fd);

// #endif
