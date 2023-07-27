#include <iostream>
#include <string.h>
#include <alt_bn128.hpp>
#include <binfile_utils.hpp>
#include "zkey_pilfflonk.hpp"
#include "utils.hpp"
#include "timer.hpp"
#include <sys/time.h>
#include "pilfflonk_setup.hpp"
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

#define PILFFLONK_SETUP_VERSION "v0.0.1"

using namespace std;
using namespace PilFflonk;

int main(int argc, char **argv)
{
    cout << "PIL-FFLONK SETUP " << string(PILFFLONK_SETUP_VERSION) << endl;

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

    if (argc != 7)
    {
        cerr << "Invalid number of parameters: " << argc << endl;
        cerr << "Usage: " << argv[0] << " <my.shkey> <fflonkInfo.json> <powers.ptau> <polynomials.cnst> <polynomials.ext.cnst> <my.zkey>" << endl;
        return -1;
    }

    // Get the input arguments
    string shKeyFilename = argv[1];
    string fflonkInfoFileName = argv[2];
    string pTauFilename = argv[3];
    string cnstPolsFilename = argv[4];
    string cnstPolsExtFilename = argv[5];
    string zkeyFilename = argv[6];

    TimerStart(SETUP_PROCESS);

    // Check required files presence
    bool bError = false;
    if (!fileExists(shKeyFilename))
    {
        cerr << "Error: shkey file '" << shKeyFilename << "' does not exist" << endl;
        bError = true;
    }

    if (!fileExists(fflonkInfoFileName))
    {
        cerr << "Error: fflonk info file '" << fflonkInfoFileName << "' does not exist" << endl;
        bError = true;
    }

    if (!fileExists(pTauFilename))
    {
        cerr << "Error: powers of Tau file '" << pTauFilename << "' does not exist" << endl;
        bError = true;
    }

    if (!fileExists(cnstPolsFilename))
    {
        cerr << "Error: constant polynomials file '" << cnstPolsFilename << "' does not exist" << endl;
        bError = true;
    }

    if (!fileExists(cnstPolsExtFilename))
    {
        cerr << "Error: constant ext polynomials file '" << cnstPolsExtFilename << "' does not exist" << endl;
        bError = true;
    }

    if (bError) return -1;

    PilFflonkSetup* setup = new PilFflonkSetup(AltBn128::Engine::engine);
    setup->generateZkey(shKeyFilename,
                        fflonkInfoFileName,
                        pTauFilename,
                        cnstPolsFilename,
                        cnstPolsExtFilename,
                        zkeyFilename);

    TimerStopAndLog(SETUP_PROCESS);
}