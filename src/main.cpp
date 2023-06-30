#include <iostream>
#include <string.h>
#include <alt_bn128.hpp>
#include <binfile_utils.hpp>
#include "zkey_pilfflonk.hpp"
#include "utils.hpp"
#include "timer.hpp"
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

    if (argc != 5) {
        cerr << "Invalid number of parameters: " << argc << endl;
        cerr << "Usage: " << argv[0] << " <pil.zkey> <fflonkInfo.json> <polynomials.cnst> <polynomials.cmtd>" << endl;
        return -1;
    }

    // Get the input arguments
    string zkeyFilename = argv[1];
    string fflonkInfoFileName = argv[2];
    string cnstFilename = argv[3];
    string cmtdFilename = argv[4];

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

    if (!fileExists(cnstFilename))
    {
        cerr << "Error: constant polynomials file '" << cnstFilename << "' does not exist" << endl;
        bError = true;
    }

    if (!fileExists(cmtdFilename))
    {
        cerr << "Error: committed polynomials file '" << cmtdFilename << "' does not exist" << endl;
        bError = true;
    }

    if (bError) return -1; // exitProcess();

    cout << "> Opening zkey file" << endl;
    auto zkey = BinFileUtils::openExisting(zkeyFilename, "zkey", 1);

    auto prover = new PilFflonk::PilFflonkProver(AltBn128::Engine::engine, fflonkInfoFileName);

    prover->prove(zkey.get(), cnstFilename, cmtdFilename);

    TimerStopAndLog(WHOLE_PROCESS);

    cout << "PIL-FFLONK PROVER FINISHED" << endl;
}