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

    if (argc < 2)
    {
        cerr << "Invalid number of parameters: " << argc << endl;
        cerr << "Usage: " << argv[0] << " <powers.ptau>" << endl;
        return -1;
    }

    string pTauFilename = argv[1];
    string zkeyFilename = argc >= 3 ? argv[2] : "config/pilfflonk.zkey";
    string shKeyFilename = argc >= 4 ? argv[3] : "config/pilfflonk.shkey.json";
    string fflonkInfoFileName = argc >= 5 ? argv[4] : "config/pilfflonk.fflonkinfo.json";
    string cnstPolsFilename =  argc == 6 ? argv[5] :"config/pilfflonk.const";


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

    if (bError) return -1;

    PilFflonkSetup* setup = new PilFflonkSetup(AltBn128::Engine::engine);
    setup->generateZkey(shKeyFilename,
                        fflonkInfoFileName,
                        pTauFilename,
                        cnstPolsFilename,
                        zkeyFilename);

    TimerStopAndLog(SETUP_PROCESS);
}