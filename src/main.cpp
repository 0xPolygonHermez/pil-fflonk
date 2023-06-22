#include <iostream>
#include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
#include <alt_bn128.hpp>    

#define PILFFLONK_PROVER_VERSION "v0.0.1"

using namespace std;

int main(int argc, char **argv)
{
    /* CONFIG */

    // Print the zkProver version
    cout << "PIL-FFLONK " << string(PILFFLONK_PROVER_VERSION) << endl;

    // Test that stderr is properly logged
    cerr << "Error: Checking error channel; ignore this trace" << endl;

#ifdef DEBUG
    cout << "DEBUG defined" << endl;
#endif

    if (argc == 2)
    {
        if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[1], "--version") == 0))
        {
            // If requested to only print the version, then exit the program
            return 0;
        }
    }

    cout << endl << "> PIL-FFLONK PROVER STARTED" << endl;

    RawFr fr;

    auto one = fr.set(1);
    auto two = fr.add(one, one);
    if(fr.eq(two, one)) {
        cout << "1 + 1 = 1" << endl;
    } else {
        cout << "1 + 1 != 1" << endl;
    }
//     TimerStart(WHOLE_PROCESS);

//     // Parse the name of the configuration file
//     char *pConfigFile = (char *)"config/config.json";
//     if (argc == 3)
//     {
//         if ((strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "--config") == 0))
//         {
//             pConfigFile = argv[2];
//         }
//     }

//     // Create one instance of Config based on the contents of the file config.json
//     TimerStart(LOAD_CONFIG_JSON);
//     json configJson;
//     file2json(pConfigFile, configJson);
//     Config config;
//     config.load(configJson);
//     config.print();
//     TimerStopAndLog(LOAD_CONFIG_JSON);

//     // Check required files presence
//     bool bError = false;
//     if (!fileExists(config.rom))
//     {
//         cerr << "Error: required file config.rom=" << config.rom << " does not exist" << endl;
//         bError = true;
//     }
//     if (config.generateProof())
//     {
//         if (!fileExists(config.zkevmConstPols))
//         {
//             cerr << "Error: required file config.zkevmConstPols=" << config.zkevmConstPols << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.c12aConstPols))
//         {
//             cerr << "Error: required file config.c12aConstPols=" << config.c12aConstPols << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive1ConstPols))
//         {
//             cerr << "Error: required file config.recursive1ConstPols=" << config.recursive1ConstPols << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive2ConstPols))
//         {
//             cerr << "Error: required file config.recursive2ConstPols=" << config.recursive2ConstPols << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursivefConstPols))
//         {
//             cerr << "Error: required file config.recursivefConstPols=" << config.recursivefConstPols << " does not exist" << endl;
//             bError = true;
//         }

//         if (!fileExists(config.zkevmConstantsTree))
//         {
//             cerr << "Error: required file config.zkevmConstantsTree=" << config.zkevmConstantsTree << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.c12aConstantsTree))
//         {
//             cerr << "Error: required file config.c12aConstantsTree=" << config.c12aConstantsTree << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive1ConstantsTree))
//         {
//             cerr << "Error: required file config.recursive1ConstantsTree=" << config.recursive1ConstantsTree << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive2ConstantsTree))
//         {
//             cerr << "Error: required file config.recursive2ConstantsTree=" << config.recursive2ConstantsTree << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursivefConstantsTree))
//         {
//             cerr << "Error: required file config.recursivefConstantsTree=" << config.recursivefConstantsTree << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.zkevmVerifier))
//         {
//             cerr << "Error: required file config.zkevmVerifier=" << config.zkevmVerifier << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive1Verifier))
//         {
//             cerr << "Error: required file config.recursive1Verifier=" << config.recursive1Verifier << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive2Verifier))
//         {
//             cerr << "Error: required file config.recursive2Verifier=" << config.recursive2Verifier << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive2Verkey))
//         {
//             cerr << "Error: required file config.recursive2Verkey=" << config.recursive2Verkey << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.finalVerifier))
//         {
//             cerr << "Error: required file config.finalVerifier=" << config.finalVerifier << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursivefVerifier))
//         {
//             cerr << "Error: required file config.recursivefVerifier=" << config.recursivefVerifier << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.finalStarkZkey))
//         {
//             cerr << "Error: required file config.finalStarkZkey=" << config.finalStarkZkey << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.storageRomFile))
//         {
//             cerr << "Error: required file config.storageRomFile=" << config.storageRomFile << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.zkevmStarkInfo))
//         {
//             cerr << "Error: required file config.zkevmStarkInfo=" << config.zkevmStarkInfo << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.c12aStarkInfo))
//         {
//             cerr << "Error: required file config.c12aStarkInfo=" << config.c12aStarkInfo << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive1StarkInfo))
//         {
//             cerr << "Error: required file config.recursive1StarkInfo=" << config.recursive1StarkInfo << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive2StarkInfo))
//         {
//             cerr << "Error: required file config.recursive2StarkInfo=" << config.recursive2StarkInfo << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursivefStarkInfo))
//         {
//             cerr << "Error: required file config.recursivefStarkInfo=" << config.recursivefStarkInfo << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.c12aExec))
//         {
//             cerr << "Error: required file config.c12aExec=" << config.c12aExec << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive1Exec))
//         {
//             cerr << "Error: required file config.recursive1Exec=" << config.recursive1Exec << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursive2Exec))
//         {
//             cerr << "Error: required file config.recursive2Exec=" << config.recursive2Exec << " does not exist" << endl;
//             bError = true;
//         }
//         if (!fileExists(config.recursivefExec))
//         {
//             cerr << "Error: required file config.recursivefExec=" << config.recursivefExec << " does not exist" << endl;
//             bError = true;
//         }
//     }
//     if (bError)
//         exitProcess();

//     // Create one instance of the Goldilocks finite field instance
//     Goldilocks fr;

//     // Create one instance of the Poseidon hash library
//     PoseidonGoldilocks poseidon;

//     // Init goldilocks precomputed
//     TimerStart(GOLDILOCKS_PRECOMPUTED_INIT);
//     glp.init();
//     TimerStopAndLog(GOLDILOCKS_PRECOMPUTED_INIT);    

//     /* TOOLS */

//     // Generate Keccak SM script
//     if (config.runKeccakScriptGenerator)
//     {
//         KeccakGenerateScript(config);
//     }

//     /* TESTS */

//     // Test Keccak SM
//     if (config.runKeccakTest)
//     {
//         // Keccak2Test();
//         KeccakSMTest();
//         KeccakSMExecutorTest(fr, config);
//     }

//     // Test Storage SM
//     if (config.runStorageSMTest)
//     {
//         StorageSMTest(fr, poseidon, config);
//     }

//     // Test Binary SM
//     if (config.runBinarySMTest)
//     {
//         BinarySMTest(fr, config);
//     }

//     // Test MemAlign SM
//     if (config.runMemAlignSMTest)
//     {
//         MemAlignSMTest(fr, config);
//     }

//     // Test SHA256
//     if (config.runSHA256Test)
//     {
//         SHA256Test(fr, config);
//     }

//     // Test Blake
//     if (config.runBlakeTest)
//     {
//         Blake2b256_Test(fr, config);
//     }

//     // If there is nothing else to run, exit normally
//     if (!config.runExecutorServer && !config.runExecutorClient && !config.runExecutorClientMultithread &&
//         !config.runStateDBServer && !config.runStateDBTest &&
//         !config.runAggregatorServer && !config.runAggregatorClient && !config.runAggregatorClientMock &&
//         !config.runFileGenBatchProof && !config.runFileGenAggregatedProof && !config.runFileGenFinalProof &&
//         !config.runFileProcessBatch && !config.runFileProcessBatchMultithread && !config.runFileExecute)
//     {
//         exit(0);
//     }

// #if 0
//     BatchMachineExecutor::batchInverseTest(fr);
// #endif

//     // Create output directory, if specified; otherwise, current working directory will be used to store output files
//     if (config.outputPath.size() > 0)
//     {
//         ensureDirectoryExists(config.outputPath);
//     }

//     // Create an instace of the Prover
//     TimerStart(PROVER_CONSTRUCTOR);
//     Prover prover(fr,
//                   poseidon,
//                   config);
//     TimerStopAndLog(PROVER_CONSTRUCTOR);

// #ifdef DATABASE_USE_CACHE

//     /* INIT DB CACHE */
//     Database::dbMTCache.setName("MTCache");
//     Database::dbProgramCache.setName("ProgramCache");
//     Database::dbMTCache.setMaxSize(config.dbMTCacheSize*1024*1024);
//     Database::dbProgramCache.setMaxSize(config.dbProgramCacheSize*1024*1024);

//     if (config.databaseURL != "local") // remote DB
//     {

//         if (config.loadDBToMemCache && (config.runAggregatorClient || config.runExecutorServer || config.runStateDBServer))
//         {
//             TimerStart(DB_CACHE_LOAD);
//             // if we have a db cache enabled
//             if ((Database::dbMTCache.enabled()) || (Database::dbProgramCache.enabled()))
//             {
//                 if (config.loadDBToMemCacheInParallel) {
//                     // Run thread that loads the DB into the dbCache
//                     std::thread loadDBThread (loadDb2MemCache, config);
//                     loadDBThread.detach();
//                 } else {
//                     loadDb2MemCache(config);
//                 }
//             }
//             TimerStopAndLog(DB_CACHE_LOAD);
//         }
//     }
    
// #endif // DATABASE_USE_CACHE

//     /* SERVERS */

//     // Create the StateDB server and run it, if configured
//     StateDBServer *pStateDBServer = NULL;
//     if (config.runStateDBServer)
//     {
//         pStateDBServer = new StateDBServer(fr, config);
//         zkassert(pStateDBServer != NULL);
//         cout << "Launching StateDB server thread..." << endl;
//         pStateDBServer->runThread();
//     }

//     // Create the executor server and run it, if configured
//     ExecutorServer *pExecutorServer = NULL;
//     if (config.runExecutorServer)
//     {
//         pExecutorServer = new ExecutorServer(fr, prover, config);
//         zkassert(pExecutorServer != NULL);
//         cout << "Launching executor server thread..." << endl;
//         pExecutorServer->runThread();
//     }

//     // Create the aggregator server and run it, if configured
//     AggregatorServer *pAggregatorServer = NULL;
//     if (config.runAggregatorServer)
//     {
//         pAggregatorServer = new AggregatorServer(fr, config);
//         zkassert(pAggregatorServer != NULL);
//         cout << "Launching aggregator server thread..." << endl;
//         pAggregatorServer->runThread();
//         sleep(5);
//     }

//     /* FILE-BASED INPUT */

//     // Generate a batch proof from the input file
//     if (config.runFileGenBatchProof)
//     {
//         if (config.inputFile.back() == '/') // Process all input files in the folder
//         {
//             Config tmpConfig = config;
//             // Get files sorted alphabetically from the folder
//             vector<string> files = getFolderFiles(config.inputFile, true);
//             // Process each input file in order
//             for (size_t i = 0; i < files.size(); i++)
//             {
//                 tmpConfig.inputFile = config.inputFile + files[i];
//                 cout << "runFileGenBatchProof inputFile=" << tmpConfig.inputFile << endl;
//                 // Call the prover
//                 runFileGenBatchProof(fr, prover, tmpConfig);
//             }
//         }
//         else
//         {
//             // Call the prover
//             runFileGenBatchProof(fr, prover, config);
//         }
//     }

//     // Generate an aggregated proof from the input file
//     if (config.runFileGenAggregatedProof)
//     {
//         if (config.inputFile.back() == '/') // Process all input files in the folder
//         {
//             Config tmpConfig = config;
//             // Get files sorted alphabetically from the folder
//             vector<string> files = getFolderFiles(config.inputFile, true);
//             // Process each input file in order
//             for (size_t i = 0; i < files.size(); i++)
//             {
//                 tmpConfig.inputFile = config.inputFile + files[i];
//                 cout << "runFileGenAggregatedProof inputFile=" << tmpConfig.inputFile << endl;
//                 // Call the prover
//                 runFileGenAggregatedProof(fr, prover, tmpConfig);
//             }
//         }
//         else
//         {
//             // Call the prover
//             runFileGenAggregatedProof(fr, prover, config);
//         }
//     }

//     // Generate a final proof from the input file
//     if (config.runFileGenFinalProof)
//     {
//         if (config.inputFile.back() == '/') // Process all input files in the folder
//         {
//             Config tmpConfig = config;
//             // Get files sorted alphabetically from the folder
//             vector<string> files = getFolderFiles(config.inputFile, true);
//             // Process each input file in order
//             for (size_t i = 0; i < files.size(); i++)
//             {
//                 tmpConfig.inputFile = config.inputFile + files[i];
//                 cout << "runFileGenFinalProof inputFile=" << tmpConfig.inputFile << endl;
//                 // Call the prover
//                 runFileGenFinalProof(fr, prover, tmpConfig);
//             }
//         }
//         else
//         {
//             // Call the prover
//             runFileGenFinalProof(fr, prover, config);
//         }
//     }

//     // Execute (no proof generation) the input file
//     if (config.runFileProcessBatch)
//     {
//         if (config.inputFile.back() == '/')
//         {
//             Config tmpConfig = config;
//             // Get files sorted alphabetically from the folder
//             vector<string> files = getFolderFiles(config.inputFile, true);
//             // Process each input file in order
//             for (size_t i = 0; i < files.size(); i++)
//             {
//                 tmpConfig.inputFile = config.inputFile + files[i];
//                 cout << "runFileProcessBatch inputFile=" << tmpConfig.inputFile << endl;
//                 // Call the prover
//                 runFileProcessBatch(fr, prover, tmpConfig);
//             }
//         }
//         else
//         {
//             runFileProcessBatch(fr, prover, config);
//         }
//     }

//     // Execute (no proof generation) the input file, in a multithread way
//     if (config.runFileProcessBatchMultithread)
//     {
//         runFileProcessBatchMultithread(fr, prover, config);
//     }

//     // Execute (no proof generation) the input file, in all SMs
//     if (config.runFileExecute)
//     {
//         runFileExecute(fr, prover, config);
//     }

//     /* CLIENTS */

//     // Create the executor client and run it, if configured
//     ExecutorClient *pExecutorClient = NULL;
//     if (config.runExecutorClient)
//     {
//         pExecutorClient = new ExecutorClient(fr, config);
//         zkassert(pExecutorClient != NULL);
//         cout << "Launching executor client thread..." << endl;
//         pExecutorClient->runThread();
//     }

//     // Run the executor client multithread, if configured
//     if (config.runExecutorClientMultithread)
//     {
//         if (pExecutorClient == NULL)
//         {
//             pExecutorClient = new ExecutorClient(fr, config);
//             zkassert(pExecutorClient != NULL);
//         }
//         cout << "Launching executor client threads..." << endl;
//         pExecutorClient->runThreads();
//     }

//     // Run the stateDB test, if configured
//     if (config.runStateDBTest)
//     {
//         cout << "Launching StateDB test thread..." << endl;
//         runStateDBTest(config);
//     }

//     // Create the aggregator client and run it, if configured
//     AggregatorClient *pAggregatorClient = NULL;
//     if (config.runAggregatorClient)
//     {
//         pAggregatorClient = new AggregatorClient(fr, config, prover);
//         zkassert(pAggregatorClient != NULL);
//         cout << "Launching aggregator client thread..." << endl;
//         pAggregatorClient->runThread();
//     }

//     // Create the aggregator client and run it, if configured
//     AggregatorClientMock * pAggregatorClientMock = NULL;
//     if (config.runAggregatorClientMock)
//     {
//         pAggregatorClientMock = new AggregatorClientMock(fr, config);
//         zkassert(pAggregatorClientMock != NULL);
//         cout << "Launching aggregator client mock thread..." << endl;
//         pAggregatorClientMock->runThread();
//     }

//     /* WAIT FOR CLIENT THREADS COMPETION */

//     // Wait for the executor client thread to end
//     if (config.runExecutorClient)
//     {
//         zkassert(pExecutorClient != NULL);
//         pExecutorClient->waitForThread();
//         sleep(1);
//         return 0;
//     }

//     // Wait for the executor client thread to end
//     if (config.runExecutorClientMultithread)
//     {
//         zkassert(pExecutorClient != NULL);
//         pExecutorClient->waitForThreads();
//         cout << "All executor client threads have completed" << endl;
//         sleep(1);
//         return 0;
//     }

//     // Wait for the executor server thread to end
//     if (config.runExecutorServer)
//     {
//         zkassert(pExecutorServer != NULL);
//         pExecutorServer->waitForThread();
//     }

//     // Wait for StateDBServer thread to end
//     if (config.runStateDBServer && !config.runStateDBTest)
//     {
//         zkassert(pStateDBServer != NULL);
//         pStateDBServer->waitForThread();
//     }

//     // Wait for the aggregator client thread to end
//     if (config.runAggregatorClient)
//     {
//         zkassert(pAggregatorClient != NULL);
//         pAggregatorClient->waitForThread();
//         sleep(1);
//         return 0;
//     }

//     // Wait for the aggregator client mock thread to end
//     if (config.runAggregatorClientMock)
//     {
//         zkassert(pAggregatorClientMock != NULL);
//         pAggregatorClientMock->waitForThread();
//         sleep(1);
//         return 0;
//     }

//     // Wait for the aggregator server thread to end
//     if (config.runAggregatorServer)
//     {
//         zkassert(pAggregatorServer != NULL);
//         pAggregatorServer->waitForThread();
//     }

//     // Clean up
//     if (pExecutorClient != NULL)
//     {
//         delete pExecutorClient;
//         pExecutorClient = NULL;
//     }
//     if (pAggregatorClient != NULL)
//     {
//         delete pAggregatorClient;
//         pAggregatorClient = NULL;
//     }
//     if (pExecutorServer != NULL)
//     {
//         delete pExecutorServer;
//         pExecutorServer = NULL;
//     }
//     if (pStateDBServer != NULL)
//     {
//         delete pStateDBServer;
//         pStateDBServer = NULL;
//     }
//     if (pAggregatorServer != NULL)
//     {
//         delete pAggregatorServer;
//         pAggregatorServer = NULL;
//     }

//     TimerStopAndLog(WHOLE_PROCESS);

    cout << "> PIL-FFLONK PROVER FINISHED" << endl;
}