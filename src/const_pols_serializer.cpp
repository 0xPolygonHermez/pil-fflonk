#include <stdexcept>
#include "const_pols_serializer.hpp"

ConstPolsSerializer::~ConstPolsSerializer()
{
    delete[] coefs;
    delete[] evalsExt;
}

ConstPolsSerializer *ConstPolsSerializer::readConstPolsFile(BinFileUtils::BinFile *fd)
{
    auto constPolsSerializer = new ConstPolsSerializer();

    readConstPolsCoefsSection(fd, constPolsSerializer);

    readConstPolsEvalsExtSection(fd, constPolsSerializer);

    return constPolsSerializer;
}

void ConstPolsSerializer::readConstPolsCoefsSection(BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->coefs = readBuffer(fd, CONST_POLS_FILE_COEFS_SECTION);
}

void ConstPolsSerializer::readConstPolsEvalsExtSection(BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->evalsExt = readBuffer(fd, CONST_POLS_FILE_EVALS_EXT_SECTION);
}

AltBn128::FrElement *ConstPolsSerializer::readBuffer(BinFileUtils::BinFile *fd, int idSection)
{
    uint64_t size = fd->getSectionSize(idSection);
    uint64_t nElements = size / sizeof(AltBn128::FrElement);

    AltBn128::FrElement *buffer = new AltBn128::FrElement[nElements];

    memcpy(&buffer[0], fd->getSectionData(idSection), size);

    return &buffer[0];
}