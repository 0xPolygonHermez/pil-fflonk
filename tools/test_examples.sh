#!/bin/bash

# Define working dir
WORKING_DIR=${1:-../pil-stark/tmp}

GENERATE_TESTS=${2:-false}

generate_basic_tests() {
    mocha ${WORKING_DIR}/../test/cfiles/fflonk_gen_all_files.js 
    mocha ${WORKING_DIR}/../test/cfiles/fflonk_gen_simple_files.js 
    mocha ${WORKING_DIR}/../test/cfiles/fflonk_gen_arguments_files.js 
}

if $GENERATE_TESTS; then
    # Generate files
    generate_basic_tests
fi

# Array of arguments
args=("simple1" "simple2"  "simple2p"  "simple3"  "simple4" "simple4p" "fibonacci" "simple_plookup" "simple_permutation" "simple_connection" "permutation" "connection" "plookup" "all")

for arg in "${args[@]}"; do
    # Copy generated files
    ./tools/copy_generated_files.sh $arg

    # Build pfProver
    ./build/pfProver

    # Run main_verifier.js
    verification=$(node ../pil-stark/src/fflonk/main_verifier.js \
        -v config/pilfflonk.vkey \
        -f config/pilfflonk.fflonkinfo.json \
        -o runtime/proof.json \
        -b runtime/public.json || echo $?)

    # Check the exit code
    if echo "$verification" | grep -q "FAIL!"; then
        echo "Verification failed for $arg. Stopping the script."
        exit 1
    else
        echo "Verification succeeded for $arg."
    fi

done

echo "All tests were successful!"