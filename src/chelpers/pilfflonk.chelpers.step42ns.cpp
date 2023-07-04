#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

AltBn128::FrElement PilFflonkSteps::step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_8 = E.fr.mul(params.const_2ns[0 + i * 3], tmp_0);
     AltBn128::FrElement tmp_9 = params.cm1_2ns[1 + i*2];
     AltBn128::FrElement tmp_10 = params.const_2ns[2 + i * 3];
     AltBn128::FrElement tmp_1 = E.fr.sub(tmp_9, params.challenges[1]);
     AltBn128::FrElement tmp_2 = E.fr.mul(tmp_1, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_2, params.challenges[1]);
     AltBn128::FrElement tmp_12 = E.fr.add(tmp_11, params.challenges[2]);
     AltBn128::FrElement tmp_13 = params.cm1_2ns[0 + i*2];
     AltBn128::FrElement tmp_14 = params.const_2ns[1 + i * 3];
     AltBn128::FrElement tmp_3 = E.fr.sub(tmp_13, params.challenges[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(tmp_3, tmp_14);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_15, params.challenges[2]);
     AltBn128::FrElement tmp_5 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%64)*1], tmp_12);
     AltBn128::FrElement tmp_6 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_16);
     AltBn128::FrElement tmp_17 = E.fr.sub(tmp_5, tmp_6);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.challenges[4], tmp_8);
     AltBn128::FrElement tmp_18 = E.fr.add(tmp_7, tmp_17);
     return tmp_18;
}

void PilFflonkSteps::step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_8 = E.fr.mul(params.const_2ns[0 + i * 3], tmp_0);
     AltBn128::FrElement tmp_9 = params.cm1_2ns[1 + i*2];
     AltBn128::FrElement tmp_10 = params.const_2ns[2 + i * 3];
     AltBn128::FrElement tmp_1 = E.fr.sub(tmp_9, params.challenges[1]);
     AltBn128::FrElement tmp_2 = E.fr.mul(tmp_1, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_2, params.challenges[1]);
     AltBn128::FrElement tmp_12 = E.fr.add(tmp_11, params.challenges[2]);
     AltBn128::FrElement tmp_13 = params.cm1_2ns[0 + i*2];
     AltBn128::FrElement tmp_14 = params.const_2ns[1 + i * 3];
     AltBn128::FrElement tmp_3 = E.fr.sub(tmp_13, params.challenges[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(tmp_3, tmp_14);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_15, params.challenges[2]);
     AltBn128::FrElement tmp_5 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%64)*1], tmp_12);
     AltBn128::FrElement tmp_6 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_16);
     AltBn128::FrElement tmp_17 = E.fr.sub(tmp_5, tmp_6);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.challenges[4], tmp_8);
     AltBn128::FrElement tmp_18 = E.fr.add(tmp_7, tmp_17);
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_18);
}

void PilFflonkSteps::step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_8 = E.fr.mul(params.const_2ns[0 + i * 3], tmp_0);
     AltBn128::FrElement tmp_9 = params.cm1_2ns[1 + i*2];
     AltBn128::FrElement tmp_10 = params.const_2ns[2 + i * 3];
     AltBn128::FrElement tmp_1 = E.fr.sub(tmp_9, params.challenges[1]);
     AltBn128::FrElement tmp_2 = E.fr.mul(tmp_1, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_2, params.challenges[1]);
     AltBn128::FrElement tmp_12 = E.fr.add(tmp_11, params.challenges[2]);
     AltBn128::FrElement tmp_13 = params.cm1_2ns[0 + i*2];
     AltBn128::FrElement tmp_14 = params.const_2ns[1 + i * 3];
     AltBn128::FrElement tmp_3 = E.fr.sub(tmp_13, params.challenges[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(tmp_3, tmp_14);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_15, params.challenges[2]);
     AltBn128::FrElement tmp_5 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%64)*1], tmp_12);
     AltBn128::FrElement tmp_6 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_16);
     AltBn128::FrElement tmp_17 = E.fr.sub(tmp_5, tmp_6);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.challenges[4], tmp_8);
     AltBn128::FrElement tmp_18 = E.fr.add(tmp_7, tmp_17);
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_18);
}
