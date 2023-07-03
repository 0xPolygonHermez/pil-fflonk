#include <stdexcept>
#include "const_pols_serializer.hpp"
#include <thread_utils.hpp>

ConstPolsSerializer::~ConstPolsSerializer()
{
    delete[] coefs;
    delete[] evalsExt;
}

ConstPolsSerializer *ConstPolsSerializer::readConstPolsFile(AltBn128::Engine &E, BinFileUtils::BinFile *fd)
{
    auto constPolsSerializer = new ConstPolsSerializer();

    readConstPolsCoefsSection(E, fd, constPolsSerializer);

    readConstPolsEvalsExtSection(E, fd, constPolsSerializer);

    return constPolsSerializer;
}

void ConstPolsSerializer::readConstPolsCoefsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->coefs = readBuffer(E, fd, CONST_POLS_FILE_COEFS_SECTION);
}

void ConstPolsSerializer::readConstPolsEvalsExtSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->evalsExt = readBuffer(E, fd, CONST_POLS_FILE_EVALS_EXT_SECTION);
}

AltBn128::FrElement *ConstPolsSerializer::readBuffer(AltBn128::Engine &E, BinFileUtils::BinFile *fd, int idSection)
{
    uint64_t size = fd->getSectionSize(idSection);
    uint64_t nElements = size / sizeof(AltBn128::FrElement);
    int nThreads = omp_get_num_threads() / 2;

    AltBn128::FrElement *buffer = new AltBn128::FrElement[nElements];

    ThreadUtils::parcpy(&buffer[0], fd->getSectionData(idSection), size, nThreads);

#pragma omp parallel for
    for (u_int64_t i = 0; i < nElements; i++)
    {
        E.fr.fromRprBE(buffer[i], (uint8_t *)&(buffer[i]), 32);
    }

    return &buffer[0];
}