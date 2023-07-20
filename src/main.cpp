#include <iostream>
#include <string.h>
#include <alt_bn128.hpp>
#include <binfile_utils.hpp>
#include "zkey_pilfflonk.hpp"
#include "utils.hpp"
#include "timer.hpp"
#include <sys/time.h>
#include "pilfflonk_prover.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>
#include <string>
#include <memory.h>
#include <stdexcept>
#include <thread_utils.hpp>
#include <omp.h>

#define PILFFLONK_PROVER_VERSION "v0.0.1"

using namespace std;

int main(int argc, char **argv)
{
    cout << "PIL-FFLONK " << string(PILFFLONK_PROVER_VERSION) << endl;

    // Test that stderr is properly logged
    cerr << "Error: Checking error channel; ignore this trace" << endl;

#ifdef DEBUG
    cout << "DEBUG defined" << endl;
#endif

    cout << endl;
    if (argc == 2)
    {
        if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[1], "--version") == 0))
        {
            // If requested to only print the version, then exit the program
            return 0;
        }
    }


    // Get the input arguments
    string zkeyFilename = "config/pilfflonk.zkey";
    string shkeyFilename = "config/pilfflonk.shkey.json";
    string fflonkInfoFileName = "config/pilfflonk.fflonkinfo.json";
    string cmtdFilename =  "config/pilfflonk.cmmt";
    string proofFilename =  "runtime/proof.json";
    string publicFilename = "runtime/public.json";

    TimerStart(WHOLE_PROCESS);

    // Check required files presence
    bool bError = false;
    if (!fileExists(zkeyFilename))
    {
        cerr << "Error: zkey file '" << zkeyFilename << "' does not exist" << endl;
        bError = true;
    }

    if (!fileExists(fflonkInfoFileName))
    {
        cerr << "Error: fflonk info file '" << fflonkInfoFileName << "' does not exist" << endl;
        bError = true;
    }

     if (!fileExists(shkeyFilename))
    {
        cerr << "Error: shkey file '" << shkeyFilename << "' does not exist" << endl;
        bError = true;
    }

    if (!fileExists(cmtdFilename))
    {
        cerr << "Error: committed polynomials file '" << cmtdFilename << "' does not exist" << endl;
        bError = true;
    }

    if (bError) return -1; // exitProcess();

    // Example using reserved memory from main.cpp
    // int numElements = 200;
    // auto reservedMemorySize = numElements * sizeof(AltBn128::FrElement);
    // auto reservedMemory = new FrElement[numElements];
    // auto prover = new PilFflonk::PilFflonkProver(AltBn128::Engine::engine,
    //                                              zkeyFilename, fflonkInfoFileName,
    //                                              reservedMemory, reservedMemorySize);

    auto prover = new PilFflonk::PilFflonkProver(AltBn128::Engine::engine,
                                                 zkeyFilename, fflonkInfoFileName);

    auto [proofJson, publicSignalsJson] = prover->prove(cmtdFilename);

    std::ofstream file;
    file.open(proofFilename);
    file << proofJson;
    file.close();

    file.open(publicFilename);
    if(publicSignalsJson.is_null()) {
        file << string("[]");
    } else {
        file << publicSignalsJson;
    }
    file.close();

    TimerStopAndLog(WHOLE_PROCESS);
}
