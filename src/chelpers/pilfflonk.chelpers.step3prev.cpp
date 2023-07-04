#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_8 = params.cm1_n[2 + i*6];
     AltBn128::FrElement tmp_9 = params.cm1_n[2 + i*6];
     AltBn128::FrElement tmp_10 = params.cm1_n[4 + i*6];
     AltBn128::FrElement tmp_0 = E.fr.mul(tmp_8, params.challenges[0]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_0, tmp_9);
     AltBn128::FrElement tmp_2 = E.fr.sub(tmp_1, params.challenges[1]);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_3, params.challenges[1]);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_11, params.challenges[2]);
     AltBn128::FrElement tmp_12 = params.cm1_n[3 + i*6];
     AltBn128::FrElement tmp_13 = params.cm1_n[3 + i*6];
     AltBn128::FrElement tmp_14 = params.cm1_n[5 + i*6];
     AltBn128::FrElement tmp_4 = E.fr.mul(params.challenges[0], tmp_12);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, tmp_13);
     AltBn128::FrElement tmp_6 = E.fr.sub(tmp_5, params.challenges[1]);
     AltBn128::FrElement tmp_7 = E.fr.mul(tmp_6, tmp_14);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_7, params.challenges[1]);
     params.tmpExp_n[1 + i*2] = E.fr.add(tmp_15, params.challenges[2]);
}

void PilFflonkSteps::step3prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_8 = params.cm1_n[2 + i*6];
     AltBn128::FrElement tmp_9 = params.cm1_n[2 + i*6];
     AltBn128::FrElement tmp_10 = params.cm1_n[4 + i*6];
     AltBn128::FrElement tmp_0 = E.fr.mul(tmp_8, params.challenges[0]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_0, tmp_9);
     AltBn128::FrElement tmp_2 = E.fr.sub(tmp_1, params.challenges[1]);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_3, params.challenges[1]);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_11, params.challenges[2]);
     AltBn128::FrElement tmp_12 = params.cm1_n[3 + i*6];
     AltBn128::FrElement tmp_13 = params.cm1_n[3 + i*6];
     AltBn128::FrElement tmp_14 = params.cm1_n[5 + i*6];
     AltBn128::FrElement tmp_4 = E.fr.mul(params.challenges[0], tmp_12);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, tmp_13);
     AltBn128::FrElement tmp_6 = E.fr.sub(tmp_5, params.challenges[1]);
     AltBn128::FrElement tmp_7 = E.fr.mul(tmp_6, tmp_14);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_7, params.challenges[1]);
     params.tmpExp_n[1 + i*2] = E.fr.add(tmp_15, params.challenges[2]);
}

void PilFflonkSteps::step3prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_8 = params.cm1_n[2 + i*6];
     AltBn128::FrElement tmp_9 = params.cm1_n[2 + i*6];
     AltBn128::FrElement tmp_10 = params.cm1_n[4 + i*6];
     AltBn128::FrElement tmp_0 = E.fr.mul(tmp_8, params.challenges[0]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_0, tmp_9);
     AltBn128::FrElement tmp_2 = E.fr.sub(tmp_1, params.challenges[1]);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_3, params.challenges[1]);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_11, params.challenges[2]);
     AltBn128::FrElement tmp_12 = params.cm1_n[3 + i*6];
     AltBn128::FrElement tmp_13 = params.cm1_n[3 + i*6];
     AltBn128::FrElement tmp_14 = params.cm1_n[5 + i*6];
     AltBn128::FrElement tmp_4 = E.fr.mul(params.challenges[0], tmp_12);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, tmp_13);
     AltBn128::FrElement tmp_6 = E.fr.sub(tmp_5, params.challenges[1]);
     AltBn128::FrElement tmp_7 = E.fr.mul(tmp_6, tmp_14);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_7, params.challenges[1]);
     params.tmpExp_n[1 + i*2] = E.fr.add(tmp_15, params.challenges[2]);
}
