#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step2prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_12 = params.cm1_n[1 + i*4];
     AltBn128::FrElement tmp_13 = params.cm1_n[2 + ((i + 1)%256)*4];
     AltBn128::FrElement tmp_14 = E.fr.mul(params.cm1_n[1 + i*4], params.cm1_n[2 + ((i + 1)%256)*4]);
     AltBn128::FrElement tmp_15 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_16 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_17 = params.cm1_n[3 + i*4];
     AltBn128::FrElement tmp_18 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[0], tmp_15);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_0, tmp_16);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[0], tmp_1);
     AltBn128::FrElement tmp_3 = E.fr.add(tmp_2, tmp_17);
     AltBn128::FrElement tmp_4 = E.fr.sub(tmp_3, params.challenges[1]);
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_4, tmp_18);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_5, params.challenges[1]);
     AltBn128::FrElement tmp_19 = params.cm1_n[0 + i*4];
     AltBn128::FrElement tmp_6 = E.fr.mul(tmp_12, params.challenges[0]);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_6, tmp_13);
     AltBn128::FrElement tmp_8 = E.fr.mul(tmp_7, params.challenges[0]);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_8, tmp_14);
     AltBn128::FrElement tmp_10 = E.fr.sub(tmp_9, params.tmpExp_n[0 + i*2]);
     AltBn128::FrElement tmp_11 = E.fr.mul(tmp_10, tmp_19);
     params.cm3_n[1 + i*3] = E.fr.add(tmp_11, params.tmpExp_n[0 + i*2]);
}

void PilFflonkSteps::step2prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_12 = params.cm1_n[1 + i*4];
     AltBn128::FrElement tmp_13 = params.cm1_n[2 + ((i + 1)%256)*4];
     AltBn128::FrElement tmp_14 = E.fr.mul(params.cm1_n[1 + i*4], params.cm1_n[2 + ((i + 1)%256)*4]);
     AltBn128::FrElement tmp_15 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_16 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_17 = params.cm1_n[3 + i*4];
     AltBn128::FrElement tmp_18 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[0], tmp_15);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_0, tmp_16);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[0], tmp_1);
     AltBn128::FrElement tmp_3 = E.fr.add(tmp_2, tmp_17);
     AltBn128::FrElement tmp_4 = E.fr.sub(tmp_3, params.challenges[1]);
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_4, tmp_18);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_5, params.challenges[1]);
     AltBn128::FrElement tmp_19 = params.cm1_n[0 + i*4];
     AltBn128::FrElement tmp_6 = E.fr.mul(tmp_12, params.challenges[0]);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_6, tmp_13);
     AltBn128::FrElement tmp_8 = E.fr.mul(tmp_7, params.challenges[0]);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_8, tmp_14);
     AltBn128::FrElement tmp_10 = E.fr.sub(tmp_9, params.tmpExp_n[0 + i*2]);
     AltBn128::FrElement tmp_11 = E.fr.mul(tmp_10, tmp_19);
     params.cm3_n[1 + i*3] = E.fr.add(tmp_11, params.tmpExp_n[0 + i*2]);
}

void PilFflonkSteps::step2prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_12 = params.cm1_n[1 + i*4];
     AltBn128::FrElement tmp_13 = params.cm1_n[2 + ((i + 1)%256)*4];
     AltBn128::FrElement tmp_14 = E.fr.mul(params.cm1_n[1 + i*4], params.cm1_n[2 + ((i + 1)%256)*4]);
     AltBn128::FrElement tmp_15 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_16 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_17 = params.cm1_n[3 + i*4];
     AltBn128::FrElement tmp_18 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[0], tmp_15);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_0, tmp_16);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[0], tmp_1);
     AltBn128::FrElement tmp_3 = E.fr.add(tmp_2, tmp_17);
     AltBn128::FrElement tmp_4 = E.fr.sub(tmp_3, params.challenges[1]);
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_4, tmp_18);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_5, params.challenges[1]);
     AltBn128::FrElement tmp_19 = params.cm1_n[0 + i*4];
     AltBn128::FrElement tmp_6 = E.fr.mul(tmp_12, params.challenges[0]);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_6, tmp_13);
     AltBn128::FrElement tmp_8 = E.fr.mul(tmp_7, params.challenges[0]);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_8, tmp_14);
     AltBn128::FrElement tmp_10 = E.fr.sub(tmp_9, params.tmpExp_n[0 + i*2]);
     AltBn128::FrElement tmp_11 = E.fr.mul(tmp_10, tmp_19);
     params.cm3_n[1 + i*3] = E.fr.add(tmp_11, params.tmpExp_n[0 + i*2]);
}
