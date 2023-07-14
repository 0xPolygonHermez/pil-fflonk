#!/bin/bash -x

NAME=${1:-all}
WORKING_DIR=${2:-../pil-stark/tmp}

# Copy the chelpers files
cp ${WORKING_DIR}/${NAME}.chelpers.constValues.cpp ./src/chelpers/pilfflonk.chelpers.constantValues.cpp
cp ${WORKING_DIR}/${NAME}.chelpers.publics.cpp ./src/chelpers/pilfflonk.chelpers.publics.cpp
cp ${WORKING_DIR}/${NAME}.chelpers.step2.cpp ./src/chelpers/pilfflonk.chelpers.step2.cpp
cp ${WORKING_DIR}/${NAME}.chelpers.step3prev.cpp ./src/chelpers/pilfflonk.chelpers.step3prev.cpp
cp ${WORKING_DIR}/${NAME}.chelpers.step3.cpp ./src/chelpers/pilfflonk.chelpers.step3.cpp
cp ${WORKING_DIR}/${NAME}.chelpers.step42ns.cpp ./src/chelpers/pilfflonk.chelpers.step42ns.cpp

# Copy files to config
cp ${WORKING_DIR}/${NAME}.zkey ./config/pilfflonk.zkey
cp ${WORKING_DIR}/${NAME}.fflonkinfo.json ./config/pilfflonk.fflonkinfo.json
cp ${WORKING_DIR}/${NAME}.cnst ./config/pilfflonk.cnst
cp ${WORKING_DIR}/${NAME}.ext.cnst ./config/pilfflonk.ext.cnst
cp ${WORKING_DIR}/${NAME}.cmmt ./config/pilfflonk.cmmt
cp ${WORKING_DIR}/${NAME}.vkey ./config/pilfflonk.vkey


#make file
make -j
