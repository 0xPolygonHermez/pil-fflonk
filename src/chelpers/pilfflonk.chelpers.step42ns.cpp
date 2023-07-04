#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_12 = E.fr.mul(params.const_2ns[0 + i * 1], tmp_0);
     AltBn128::FrElement tmp_13 = params.cm1_2ns[3 + i*6];
     AltBn128::FrElement tmp_14 = params.cm1_2ns[3 + i*6];
     AltBn128::FrElement tmp_15 = params.cm1_2ns[5 + i*6];
     AltBn128::FrElement tmp_1 = E.fr.mul(params.challenges[0], tmp_13);
     AltBn128::FrElement tmp_2 = E.fr.add(tmp_1, tmp_14);
     AltBn128::FrElement tmp_3 = E.fr.sub(tmp_2, params.challenges[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(tmp_3, tmp_15);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, params.challenges[2]);
     AltBn128::FrElement tmp_18 = params.cm1_2ns[2 + i*6];
     AltBn128::FrElement tmp_19 = params.cm1_2ns[2 + i*6];
     AltBn128::FrElement tmp_20 = params.cm1_2ns[4 + i*6];
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_18, params.challenges[0]);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_5, tmp_19);
     AltBn128::FrElement tmp_7 = E.fr.sub(tmp_6, params.challenges[1]);
     AltBn128::FrElement tmp_8 = E.fr.mul(tmp_7, tmp_20);
     AltBn128::FrElement tmp_21 = E.fr.add(tmp_8, params.challenges[1]);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_21, params.challenges[2]);
     AltBn128::FrElement tmp_9 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%2048)*1], tmp_17);
     AltBn128::FrElement tmp_10 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_22);
     AltBn128::FrElement tmp_23 = E.fr.sub(tmp_9, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.mul(params.challenges[4], tmp_12);
     AltBn128::FrElement tmp_24 = E.fr.add(tmp_11, tmp_23);
     params.q_2ns[i] = tmp_24;
}

void PilFflonkSteps::step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_12 = E.fr.mul(params.const_2ns[0 + i * 1], tmp_0);
     AltBn128::FrElement tmp_13 = params.cm1_2ns[3 + i*6];
     AltBn128::FrElement tmp_14 = params.cm1_2ns[3 + i*6];
     AltBn128::FrElement tmp_15 = params.cm1_2ns[5 + i*6];
     AltBn128::FrElement tmp_1 = E.fr.mul(params.challenges[0], tmp_13);
     AltBn128::FrElement tmp_2 = E.fr.add(tmp_1, tmp_14);
     AltBn128::FrElement tmp_3 = E.fr.sub(tmp_2, params.challenges[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(tmp_3, tmp_15);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, params.challenges[2]);
     AltBn128::FrElement tmp_18 = params.cm1_2ns[2 + i*6];
     AltBn128::FrElement tmp_19 = params.cm1_2ns[2 + i*6];
     AltBn128::FrElement tmp_20 = params.cm1_2ns[4 + i*6];
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_18, params.challenges[0]);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_5, tmp_19);
     AltBn128::FrElement tmp_7 = E.fr.sub(tmp_6, params.challenges[1]);
     AltBn128::FrElement tmp_8 = E.fr.mul(tmp_7, tmp_20);
     AltBn128::FrElement tmp_21 = E.fr.add(tmp_8, params.challenges[1]);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_21, params.challenges[2]);
     AltBn128::FrElement tmp_9 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%2048)*1], tmp_17);
     AltBn128::FrElement tmp_10 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_22);
     AltBn128::FrElement tmp_23 = E.fr.sub(tmp_9, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.mul(params.challenges[4], tmp_12);
     AltBn128::FrElement tmp_24 = E.fr.add(tmp_11, tmp_23);
     params.q_2ns[i] = tmp_24;
}

void PilFflonkSteps::step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm3_2ns[0 + i*1], E.fr.set(1));
     AltBn128::FrElement tmp_12 = E.fr.mul(params.const_2ns[0 + i * 1], tmp_0);
     AltBn128::FrElement tmp_13 = params.cm1_2ns[3 + i*6];
     AltBn128::FrElement tmp_14 = params.cm1_2ns[3 + i*6];
     AltBn128::FrElement tmp_15 = params.cm1_2ns[5 + i*6];
     AltBn128::FrElement tmp_1 = E.fr.mul(params.challenges[0], tmp_13);
     AltBn128::FrElement tmp_2 = E.fr.add(tmp_1, tmp_14);
     AltBn128::FrElement tmp_3 = E.fr.sub(tmp_2, params.challenges[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(tmp_3, tmp_15);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, params.challenges[2]);
     AltBn128::FrElement tmp_18 = params.cm1_2ns[2 + i*6];
     AltBn128::FrElement tmp_19 = params.cm1_2ns[2 + i*6];
     AltBn128::FrElement tmp_20 = params.cm1_2ns[4 + i*6];
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_18, params.challenges[0]);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_5, tmp_19);
     AltBn128::FrElement tmp_7 = E.fr.sub(tmp_6, params.challenges[1]);
     AltBn128::FrElement tmp_8 = E.fr.mul(tmp_7, tmp_20);
     AltBn128::FrElement tmp_21 = E.fr.add(tmp_8, params.challenges[1]);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_21, params.challenges[2]);
     AltBn128::FrElement tmp_9 = E.fr.mul(params.cm3_2ns[0 + ((i + 8)%2048)*1], tmp_17);
     AltBn128::FrElement tmp_10 = E.fr.mul(params.cm3_2ns[0 + i*1], tmp_22);
     AltBn128::FrElement tmp_23 = E.fr.sub(tmp_9, tmp_10);
     AltBn128::FrElement tmp_11 = E.fr.mul(params.challenges[4], tmp_12);
     AltBn128::FrElement tmp_24 = E.fr.add(tmp_11, tmp_23);
     params.q_2ns[i] = tmp_24;
}
