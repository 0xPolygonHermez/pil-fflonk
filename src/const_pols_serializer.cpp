// #include <stdexcept>
// #include "const_pols_serializer.hpp"

// using namespace std;

// PilFflonkZkey::~PilFflonkZkey()
// {
//     delete[] coefs;
//     delete[] evalsExt;
// }

// PilFflonkZkey *loadPilFflonkZkey(BinFileUtils::BinFile *fd)
// {
//     auto constPolsSerializer = new ConstPolsSerializer();

//     readConstPolsCoefsSection(fd, pilFflonkZkey);

//     readConstPolsEvalsExtSection(fd, pilFflonkZkey);

//     return pilFflonkZkey;
// }

// void readConstPolsCoefsSection(BinFileUtils::BinFile *fd, PilFflonkZkey *pilFflonkZkey)
// {
//     fd->startReadSection(ZKEY_PF_F_SECTION);
//     //coefs = readBuffer(fd, sections[CONST_POLS_FILE_COEFS_SECTION]);
//     fd->endReadSection();
// }

// void readConstPolsEvalsExtSection(BinFileUtils::BinFile *fd, PilFflonkZkey *pilFflonkZkey)
// {
//     fd->startReadSection(ZKEY_PF_OPENINGPOINTS_SECTION);
//     //evalsExt = readBuffer(fd, sections[CONST_POLS_FILE_EVALS_EXT_SECTION]);
//     fd->endReadSection();
// }

// // async function readBuffer(fd, section, Fr) {
// //     const size = section[0].size;
// //     const partialBuffer = new BigBuffer(size);
// //     const nElements = size / Fr.n8;

// //     partialBuffer.set(await fd.read(size), 0);

// //     let p=0;
// //     for (let i = 0; i < nElements; i++) {
// //         const element = partialBuffer.slice(i * Fr.n8, (i + 1) * Fr.n8);
        
// //         partialBuffer.set(Fr.fromRprLE(element), i * Fr.n8);
// //         p += Fr.n8;
// //     }

// //     return partialBuffer;
// // }