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

if ! fileExists "${WORKING_DIR}/${NAME}.fflonkinfo.json"; then echo "FflonkInfo file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.fflonkinfo.json ./config/pilfflonk.fflonkinfo.json

if ! fileExists "${WORKING_DIR}/${NAME}.cnst"; then echo "Const file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.cnst ./config/pilfflonk.cnst

if ! fileExists "${WORKING_DIR}/${NAME}.ext.cnst"; then echo "Ext Const file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.ext.cnst ./config/pilfflonk.ext.cnst

if ! fileExists "${WORKING_DIR}/${NAME}.cmmt"; then echo "Commit file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.cmmt ./config/pilfflonk.cmmt

if ! fileExists "${WORKING_DIR}/${NAME}.vkey"; then echo "Verification Key file not found."; exit 1; fi
cp ${WORKING_DIR}/${NAME}.vkey ./config/pilfflonk.vkey


#make file
make -j
