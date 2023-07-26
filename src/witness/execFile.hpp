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
public:
    uint64_t nAdds;
    uint64_t nSMap;

    std::unique_ptr<BinFileUtils::BinFile> execFile;

    FrElement *p_adds;
    FrElement *p_sMap;

    ExecFile(std::string execFileName, uint64_t nCommittedPols)
    {
        execFile = BinFileUtils::openExisting(execFileName, "exec", 1);
        auto fdExec = execFile.get();

        nAdds = fdExec->readU32LE();
        nSMap = fdExec->readU32LE();

        uint64_t lenAdds = nAdds * 4 * sizeof(FrElement);
        uint64_t lenSMap = nSMap * nCommittedPols * sizeof(FrElement);

        ThreadUtils::parcpy(p_adds, (FrElement *)fdExec->read(lenAdds), lenAdds, omp_get_num_threads() / 2);

        ThreadUtils::parcpy(p_sMap, (FrElement *)fdExec->read(lenSMap), lenSMap, omp_get_num_threads() / 2);
    }
    ~ExecFile()
    {
        delete[] p_adds;
        delete[] p_sMap;
    }
};
#endif