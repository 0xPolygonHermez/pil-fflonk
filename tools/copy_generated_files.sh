#!/bin/bash

fileExists () {
    local fileName="$1"
    if [ -f "$fileName" ]; then
        return 0  # File exists
    else
        return 1  # File does not exist
    fi
}

NAME=${1:-all}
WORKING_DIR=${2:-../pil-stark/tmp}

CIRCOM_VERIFIER=${3:-false}
CIRCOM_NAME=${4:-plonktest.bn128}

CIRCOM_HEADER="#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored \"-Wunused-variable\"\n#pragma GCC push_options\n#pragma GCC optimize (\"O0\")\n#include <stdio.h>\n#include <iostream>\n#include <assert.h>\n#include <cassert>\n"

# Copy the chelpers files
if ! fileExists "${WORKING_DIR}/${NAME}.chelpers.constValues.cpp"; then echo "Const Values file chelpers not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.chelpers.constValues.cpp ./src/chelpers/pilfflonk.chelpers.constantValues.cpp

if ! fileExists "${WORKING_DIR}/${NAME}.chelpers.publics.cpp"; then echo "Public file chelpers not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.chelpers.publics.cpp ./src/chelpers/pilfflonk.chelpers.publics.cpp

if ! fileExists "${WORKING_DIR}/${NAME}.chelpers.step2.cpp"; then echo "Step2 file chelpers not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.chelpers.step2.cpp ./src/chelpers/pilfflonk.chelpers.step2.cpp

if ! fileExists "${WORKING_DIR}/${NAME}.chelpers.step3prev.cpp"; then echo "Step3Prev file chelpers not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.chelpers.step3prev.cpp ./src/chelpers/pilfflonk.chelpers.step3prev.cpp

if ! fileExists "${WORKING_DIR}/${NAME}.chelpers.step3.cpp"; then echo "Step3 file chelpers not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.chelpers.step3.cpp ./src/chelpers/pilfflonk.chelpers.step3.cpp

if ! fileExists "${WORKING_DIR}/${NAME}.chelpers.step42ns.cpp"; then echo "Step42ns file chelpers not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.chelpers.step42ns.cpp ./src/chelpers/pilfflonk.chelpers.step42ns.cpp

# Copy files to config
if ! fileExists "${WORKING_DIR}/${NAME}.zkey"; then echo "Zkey file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.zkey ./config/pilfflonk.zkey

if ! fileExists "${WORKING_DIR}/${NAME}.shkey.json"; then echo "Shkey file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.shkey.json ./config/pilfflonk.shkey.json

if ! fileExists "${WORKING_DIR}/${NAME}.fflonkinfo.json"; then echo "FflonkInfo file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.fflonkinfo.json ./config/pilfflonk.fflonkinfo.json

if ! fileExists "${WORKING_DIR}/${NAME}.cmmt"; then echo "Commit file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.cmmt ./config/pilfflonk.cmmt

if ! fileExists "${WORKING_DIR}/${NAME}.vkey"; then echo "Verification Key file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.vkey ./config/pilfflonk.vkey

if ${CIRCOM_VERIFIER}; then 
    if ! fileExists "${WORKING_DIR}/${CIRCOM_NAME}_cpp/${CIRCOM_NAME}.cpp"; then echo "Circom verifier file not found."; exit 1; fi
    cp ${WORKING_DIR}/${CIRCOM_NAME}_cpp/${CIRCOM_NAME}.cpp src/witness/verifier.cpp
    sed -i '1d;2d;3d;4d;5d' src/witness/verifier.cpp
    sed -i "1s/^/#include \"circom.pilfflonk.hpp\"\n#include \"calcwit.pilfflonk.hpp\"\nnamespace CircomPilFflonk\n{\n/" src/witness/verifier.cpp
    sed -i "1s/^/$CIRCOM_HEADER/" src/witness/verifier.cpp
    echo -e "}\n#pragma GCC diagnostic pop" >> src/witness/verifier.cpp

    cp ${WORKING_DIR}/${CIRCOM_NAME}_cpp/${CIRCOM_NAME}.dat ./config/verifier.dat
fi

#make file
make -j
