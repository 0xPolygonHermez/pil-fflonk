#include <iostream>
#include <string.h>
#include <alt_bn128.hpp>
#include <binfile_utils.hpp>
#include "zkey_pilfflonk.hpp"
#include "utils.hpp"
#include "timer.hpp"
#include "pilfflonk_prover.hpp"

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
        cerr << "Usage: " << argv[0] << " <pil.zkey> <fflonkInfo.json> <polynomials.cnst> <polynomials.cmt>" << endl;
        return -1;
    }

    // Get the input arguments
    string zkeyFilename = argv[1];
    string fflonkInfoFileName = argv[2];
    string cnstFilename = argv[3];
    string cmtFilename = argv[4];

    TimerStart(WHOLE_PROCESS);

    // Check required files presence
    bool bError = false;
    if (!fileExists(zkeyFilename))
    {
        cerr << "Error: required file zkeyFilename=" << zkeyFilename << " does not exist" << endl;
        bError = true;
    }

    if (!fileExists(fflonkInfoFileName))
    {
        cerr << "Error: required file zkeyFilename=" << fflonkInfoFileName << " does not exist" << endl;
        bError = true;
    }
    // if (!fileExists(cnstFilename))
    // {
    //     cerr << "Error: required file cnstFilename=" << cnstFilename << " does not exist" << endl;
    //     bError = true;
    // }
    // if (!fileExists(cmtFilename))
    // {
    //     cerr << "Error: required file cmtFilename=" << cmtFilename << " does not exist" << endl;
    //     bError = true;
    // }

    if (bError) return -1; // exitProcess();

    cout << "> Opening zkey file" << endl;
    auto zkey = BinFileUtils::openExisting(zkeyFilename, "zkey", 1);

    // cout << "> Opening constant polynomials file file" << endl;
    // auto cnstPols = BinFileUtils::openExisting(cnstFilename, "cnst", 1);

    // cout << "> Opening committed polynomial file" << endl;
    // auto cmtPols = BinFileUtils::openExisting(cmtFilename, "cmt", 1);

    auto prover = new PilFflonk::PilFflonkProver(AltBn128::Engine::engine, fflonkInfoFileName);
    prover->prove(zkey.get() /*, cnstPols, cmtPols*/);

    TimerStopAndLog(WHOLE_PROCESS);

    cout << "PIL-FFLONK PROVER FINISHED" << endl;
}