#include <stdexcept>
#include "const_pols_serializer.hpp"
#include <thread_utils.hpp>

ConstPolsSerializer::~ConstPolsSerializer()
{
    delete[] coefs;
    delete[] evalsExt;
    delete[] x_n;
    delete[] x_2ns;
}

ConstPolsSerializer *ConstPolsSerializer::readConstPolsFile(AltBn128::Engine &E, BinFileUtils::BinFile *fd)
{
    auto constPolsSerializer = new ConstPolsSerializer();

    readConstPolsCoefsSection(E, fd, constPolsSerializer);
    readConstPolsEvalsExtSection(E, fd, constPolsSerializer);
    readXnSection(E, fd, constPolsSerializer);
    readX2nsSection(E, fd, constPolsSerializer);

    return constPolsSerializer;
}

void ConstPolsSerializer::readConstPolsFile(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs, AltBn128::FrElement *evalsExt, AltBn128::FrElement *x_n, AltBn128::FrElement *x_2ns)
{
    readConstPolsCoefsSection(E, fd, coefs);
    readConstPolsEvalsExtSection(E, fd, evalsExt);
    readXnSection(E, fd, x_n);
    readX2nsSection(E, fd, x_2ns);

}

void ConstPolsSerializer::readConstPolsCoefsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->coefs = readBuffer(E, fd, CONST_POLS_FILE_COEFS_SECTION);
}

void ConstPolsSerializer::readConstPolsCoefsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *coefs)
{
    readBuffer(E, fd, CONST_POLS_FILE_COEFS_SECTION, coefs);
}

void ConstPolsSerializer::readConstPolsEvalsExtSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->evalsExt = readBuffer(E, fd, CONST_POLS_FILE_EVALS_EXT_SECTION);
}

void ConstPolsSerializer::readConstPolsEvalsExtSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *evalsExt)
{
    readBuffer(E, fd, CONST_POLS_FILE_EVALS_EXT_SECTION, evalsExt);
}

void ConstPolsSerializer::readXnSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->x_n = readBuffer(E, fd, X_N_SECTION);
}

void ConstPolsSerializer::readXnSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *x_n)
{
    readBuffer(E, fd, X_N_SECTION, x_n);
}


void ConstPolsSerializer::readX2nsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, ConstPolsSerializer *constPolsSerializer)
{
    constPolsSerializer->x_2ns = readBuffer(E, fd, X_2NS_SECTION);
}

void ConstPolsSerializer::readX2nsSection(AltBn128::Engine &E, BinFileUtils::BinFile *fd, AltBn128::FrElement *x_2ns)
{
    readBuffer(E, fd, X_2NS_SECTION, x_2ns);
}


AltBn128::FrElement *ConstPolsSerializer::readBuffer(AltBn128::Engine &E, BinFileUtils::BinFile *fd, int idSection)
{
    uint64_t size = fd->getSectionSize(idSection);
    uint64_t nElements = size / sizeof(AltBn128::FrElement);
    int nThreads = omp_get_num_threads() / 2;

    AltBn128::FrElement *buffer = new AltBn128::FrElement[nElements];

    ThreadUtils::parcpy(&buffer[0], (FrElement *)fd->getSectionData(idSection), size, nThreads);

    return &buffer[0];
}

void ConstPolsSerializer::readBuffer(AltBn128::Engine &E, BinFileUtils::BinFile *fd, int idSection, AltBn128::FrElement *ptrDst)
{
    uint64_t size = fd->getSectionSize(idSection);
    int nThreads = omp_get_num_threads() / 2;

    AltBn128::FrElement *buffer = ptrDst;

    ThreadUtils::parcpy(&buffer[0], (FrElement *)fd->getSectionData(idSection), size, nThreads);
}