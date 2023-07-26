#ifndef EXEC_FILE
#define EXEC_FILE

#define EXEC_FILE_SIZE
#include <iostream>
#include <alt_bn128.hpp>
#include <binfile_utils.hpp>
#include "thread_utils.hpp"

#include "../zklog.hpp"

class ExecFile
{
    const int EXEC_PF_NSECTIONS = 3;

    const int EXEC_ADDS_SECTION = 2;
    const int EXEC_SMAP_SECTION = 3;

public:
    uint64_t nAdds;
    uint64_t nSMap;

    std::unique_ptr<BinFileUtils::BinFile> execFile;

    AltBn128::FrElement *p_adds;
    AltBn128::FrElement *p_sMap;

    ExecFile(AltBn128::Engine &E, std::string execFileName, uint64_t nCommittedPols)
    {
        execFile = BinFileUtils::openExisting(execFileName, "exec", 1);

        auto fdExec = execFile.get();

        fdExec->startReadSection(EXEC_ADDS_SECTION);

        nAdds = fdExec->readU64LE();
        uint64_t lenAdds = nAdds * 4 * sizeof(AltBn128::FrElement);
        p_adds = new AltBn128::FrElement[nAdds * 4];
    
        ThreadUtils::parcpy(p_adds, (void *)fdExec->read(lenAdds), lenAdds, omp_get_num_threads() / 2);

        fdExec->endReadSection();

        fdExec->startReadSection(EXEC_SMAP_SECTION);

        nSMap = fdExec->readU64LE();
        uint64_t lenSMap = nSMap * nCommittedPols * sizeof(AltBn128::FrElement);
        p_sMap = new AltBn128::FrElement[nSMap * nCommittedPols];

        ThreadUtils::parcpy(p_sMap, (AltBn128::FrElement *)fdExec->read(lenSMap), lenSMap, omp_get_num_threads() / 2);

        fdExec->endReadSection();
    }
    ~ExecFile()
    {
        delete[] p_adds;
        delete[] p_sMap;
    }
};
#endif