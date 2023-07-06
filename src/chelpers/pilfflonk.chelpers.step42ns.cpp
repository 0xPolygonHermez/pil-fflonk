#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_15 = E.fr.mul(params.const_2ns[0 + i * 3], tmp_0);
     AltBn128::FrElement tmp_16 = params.cm1_2ns[0 + i*2];
     AltBn128::FrElement tmp_17 = params.const_2ns[1 + i * 3];
     AltBn128::FrElement tmp_1 = E.fr.mul(params.challenges[2], tmp_17);
     AltBn128::FrElement tmp_2 = E.fr.add(tmp_16, tmp_1);
     AltBn128::FrElement tmp_18 = E.fr.add(tmp_2, params.challenges[3]);
     AltBn128::FrElement tmp_19 = params.cm1_2ns[1 + i*2];
     AltBn128::FrElement tmp_20 = params.const_2ns[2 + i * 3];
     AltBn128::FrElement tmp_3 = E.fr.mul(params.challenges[2], tmp_20);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_19, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_18, tmp_5);
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], params.x_2ns[i]);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_16, tmp_6);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_7, params.challenges[3]);
     AltBn128::FrElement tmp_8 = E.fr.mul(params.challenges[2], params.constValues[0]);
     AltBn128::FrElement tmp_9 = E.fr.mul(tmp_8, params.x_2ns[i]);
     AltBn128::FrElement tmp_10 = E.fr.add(tmp_19, tmp_9);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_10, params.challenges[3]);
     AltBn128::FrElement tmp_23 = E.fr.mul(tmp_22, tmp_11);
     AltBn128::FrElement tmp_12 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%32)*1], tmp_21);
     AltBn128::FrElement tmp_13 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_23);
     AltBn128::FrElement tmp_24 = E.fr.sub(tmp_12, tmp_13);
     AltBn128::FrElement tmp_14 = E.fr.mul(params.challenges[4], tmp_15);
     AltBn128::FrElement tmp_25 = E.fr.add(tmp_14, tmp_24);
     params.q_2ns[i] = tmp_25;
}

void PilFflonkSteps::step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_15 = E.fr.mul(params.const_2ns[0 + i * 3], tmp_0);
     AltBn128::FrElement tmp_16 = params.cm1_2ns[0 + i*2];
     AltBn128::FrElement tmp_17 = params.const_2ns[1 + i * 3];
     AltBn128::FrElement tmp_1 = E.fr.mul(params.challenges[2], tmp_17);
     AltBn128::FrElement tmp_2 = E.fr.add(tmp_16, tmp_1);
     AltBn128::FrElement tmp_18 = E.fr.add(tmp_2, params.challenges[3]);
     AltBn128::FrElement tmp_19 = params.cm1_2ns[1 + i*2];
     AltBn128::FrElement tmp_20 = params.const_2ns[2 + i * 3];
     AltBn128::FrElement tmp_3 = E.fr.mul(params.challenges[2], tmp_20);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_19, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_18, tmp_5);
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], params.x_2ns[i]);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_16, tmp_6);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_7, params.challenges[3]);
     AltBn128::FrElement tmp_8 = E.fr.mul(params.challenges[2], params.constValues[0]);
     AltBn128::FrElement tmp_9 = E.fr.mul(tmp_8, params.x_2ns[i]);
     AltBn128::FrElement tmp_10 = E.fr.add(tmp_19, tmp_9);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_10, params.challenges[3]);
     AltBn128::FrElement tmp_23 = E.fr.mul(tmp_22, tmp_11);
     AltBn128::FrElement tmp_12 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%32)*1], tmp_21);
     AltBn128::FrElement tmp_13 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_23);
     AltBn128::FrElement tmp_24 = E.fr.sub(tmp_12, tmp_13);
     AltBn128::FrElement tmp_14 = E.fr.mul(params.challenges[4], tmp_15);
     AltBn128::FrElement tmp_25 = E.fr.add(tmp_14, tmp_24);
     params.q_2ns[i] = tmp_25;
}

void PilFflonkSteps::step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_15 = E.fr.mul(params.const_2ns[0 + i * 3], tmp_0);
     AltBn128::FrElement tmp_16 = params.cm1_2ns[0 + i*2];
     AltBn128::FrElement tmp_17 = params.const_2ns[1 + i * 3];
     AltBn128::FrElement tmp_1 = E.fr.mul(params.challenges[2], tmp_17);
     AltBn128::FrElement tmp_2 = E.fr.add(tmp_16, tmp_1);
     AltBn128::FrElement tmp_18 = E.fr.add(tmp_2, params.challenges[3]);
     AltBn128::FrElement tmp_19 = params.cm1_2ns[1 + i*2];
     AltBn128::FrElement tmp_20 = params.const_2ns[2 + i * 3];
     AltBn128::FrElement tmp_3 = E.fr.mul(params.challenges[2], tmp_20);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_19, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_18, tmp_5);
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], params.x_2ns[i]);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_16, tmp_6);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_7, params.challenges[3]);
     AltBn128::FrElement tmp_8 = E.fr.mul(params.challenges[2], params.constValues[0]);
     AltBn128::FrElement tmp_9 = E.fr.mul(tmp_8, params.x_2ns[i]);
     AltBn128::FrElement tmp_10 = E.fr.add(tmp_19, tmp_9);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_10, params.challenges[3]);
     AltBn128::FrElement tmp_23 = E.fr.mul(tmp_22, tmp_11);
     AltBn128::FrElement tmp_12 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%32)*1], tmp_21);
     AltBn128::FrElement tmp_13 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_23);
     AltBn128::FrElement tmp_24 = E.fr.sub(tmp_12, tmp_13);
     AltBn128::FrElement tmp_14 = E.fr.mul(params.challenges[4], tmp_15);
     AltBn128::FrElement tmp_25 = E.fr.add(tmp_14, tmp_24);
     params.q_2ns[i] = tmp_25;
}
