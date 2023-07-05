#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm1_2ns[1 + ((i + 4)%256)*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.sub(E.fr.set(1), params.const_2ns[1 + i * 2]);
     AltBn128::FrElement tmp_2 = E.fr.mul(tmp_0, tmp_1);
     AltBn128::FrElement tmp_25 = E.fr.sub(tmp_2, E.fr.set(0));
     AltBn128::FrElement tmp_3 = E.fr.sub(params.cm1_2ns[0 + ((i + 4)%256)*2], params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_4 = E.fr.sub(E.fr.set(1), params.const_2ns[1 + i * 2]);
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_3, tmp_4);
     AltBn128::FrElement tmp_26 = E.fr.sub(tmp_5, E.fr.set(0));
     AltBn128::FrElement tmp_6 = E.fr.sub(params.cm1_2ns[1 + i*2], params.publicInputs[0]);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_6);
     AltBn128::FrElement tmp_27 = E.fr.sub(tmp_7, E.fr.set(0));
     AltBn128::FrElement tmp_8 = E.fr.sub(params.cm1_2ns[0 + i*2], params.publicInputs[1]);
     AltBn128::FrElement tmp_9 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_8);
     AltBn128::FrElement tmp_28 = E.fr.sub(tmp_9, E.fr.set(0));
     AltBn128::FrElement tmp_10 = E.fr.sub(params.cm1_2ns[0 + i*2], params.publicInputs[2]);
     AltBn128::FrElement tmp_11 = E.fr.mul(params.const_2ns[1 + i * 2], tmp_10);
     AltBn128::FrElement tmp_29 = E.fr.sub(tmp_11, E.fr.set(0));
     AltBn128::FrElement tmp_12 = E.fr.mul(params.challenges[4], tmp_25);
     AltBn128::FrElement tmp_13 = E.fr.add(tmp_12, tmp_26);
     AltBn128::FrElement tmp_14 = E.fr.mul(params.challenges[4], tmp_13);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_14, tmp_27);
     AltBn128::FrElement tmp_16 = E.fr.mul(params.challenges[4], tmp_15);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, tmp_28);
     AltBn128::FrElement tmp_18 = E.fr.mul(params.challenges[4], tmp_17);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_18, tmp_29);
     AltBn128::FrElement tmp_20 = E.fr.mul(params.challenges[4], tmp_19);
     AltBn128::FrElement tmp_21 = E.fr.mul(params.cm1_2ns[0 + i*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_22 = E.fr.mul(params.cm1_2ns[1 + i*2], params.cm1_2ns[1 + i*2]);
     AltBn128::FrElement tmp_23 = E.fr.add(tmp_21, tmp_22);
     AltBn128::FrElement tmp_24 = E.fr.sub(tmp_23, params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_30 = E.fr.add(tmp_20, tmp_24);
     params.q_2ns[i] = tmp_30;
}

void PilFflonkSteps::step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm1_2ns[1 + ((i + 4)%256)*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.sub(E.fr.set(1), params.const_2ns[1 + i * 2]);
     AltBn128::FrElement tmp_2 = E.fr.mul(tmp_0, tmp_1);
     AltBn128::FrElement tmp_25 = E.fr.sub(tmp_2, E.fr.set(0));
     AltBn128::FrElement tmp_3 = E.fr.sub(params.cm1_2ns[0 + ((i + 4)%256)*2], params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_4 = E.fr.sub(E.fr.set(1), params.const_2ns[1 + i * 2]);
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_3, tmp_4);
     AltBn128::FrElement tmp_26 = E.fr.sub(tmp_5, E.fr.set(0));
     AltBn128::FrElement tmp_6 = E.fr.sub(params.cm1_2ns[1 + i*2], params.publicInputs[0]);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_6);
     AltBn128::FrElement tmp_27 = E.fr.sub(tmp_7, E.fr.set(0));
     AltBn128::FrElement tmp_8 = E.fr.sub(params.cm1_2ns[0 + i*2], params.publicInputs[1]);
     AltBn128::FrElement tmp_9 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_8);
     AltBn128::FrElement tmp_28 = E.fr.sub(tmp_9, E.fr.set(0));
     AltBn128::FrElement tmp_10 = E.fr.sub(params.cm1_2ns[0 + i*2], params.publicInputs[2]);
     AltBn128::FrElement tmp_11 = E.fr.mul(params.const_2ns[1 + i * 2], tmp_10);
     AltBn128::FrElement tmp_29 = E.fr.sub(tmp_11, E.fr.set(0));
     AltBn128::FrElement tmp_12 = E.fr.mul(params.challenges[4], tmp_25);
     AltBn128::FrElement tmp_13 = E.fr.add(tmp_12, tmp_26);
     AltBn128::FrElement tmp_14 = E.fr.mul(params.challenges[4], tmp_13);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_14, tmp_27);
     AltBn128::FrElement tmp_16 = E.fr.mul(params.challenges[4], tmp_15);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, tmp_28);
     AltBn128::FrElement tmp_18 = E.fr.mul(params.challenges[4], tmp_17);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_18, tmp_29);
     AltBn128::FrElement tmp_20 = E.fr.mul(params.challenges[4], tmp_19);
     AltBn128::FrElement tmp_21 = E.fr.mul(params.cm1_2ns[0 + i*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_22 = E.fr.mul(params.cm1_2ns[1 + i*2], params.cm1_2ns[1 + i*2]);
     AltBn128::FrElement tmp_23 = E.fr.add(tmp_21, tmp_22);
     AltBn128::FrElement tmp_24 = E.fr.sub(tmp_23, params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_30 = E.fr.add(tmp_20, tmp_24);
     params.q_2ns[i] = tmp_30;
}

void PilFflonkSteps::step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.sub(params.cm1_2ns[1 + ((i + 4)%256)*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.sub(E.fr.set(1), params.const_2ns[1 + i * 2]);
     AltBn128::FrElement tmp_2 = E.fr.mul(tmp_0, tmp_1);
     AltBn128::FrElement tmp_25 = E.fr.sub(tmp_2, E.fr.set(0));
     AltBn128::FrElement tmp_3 = E.fr.sub(params.cm1_2ns[0 + ((i + 4)%256)*2], params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_4 = E.fr.sub(E.fr.set(1), params.const_2ns[1 + i * 2]);
     AltBn128::FrElement tmp_5 = E.fr.mul(tmp_3, tmp_4);
     AltBn128::FrElement tmp_26 = E.fr.sub(tmp_5, E.fr.set(0));
     AltBn128::FrElement tmp_6 = E.fr.sub(params.cm1_2ns[1 + i*2], params.publicInputs[0]);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_6);
     AltBn128::FrElement tmp_27 = E.fr.sub(tmp_7, E.fr.set(0));
     AltBn128::FrElement tmp_8 = E.fr.sub(params.cm1_2ns[0 + i*2], params.publicInputs[1]);
     AltBn128::FrElement tmp_9 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_8);
     AltBn128::FrElement tmp_28 = E.fr.sub(tmp_9, E.fr.set(0));
     AltBn128::FrElement tmp_10 = E.fr.sub(params.cm1_2ns[0 + i*2], params.publicInputs[2]);
     AltBn128::FrElement tmp_11 = E.fr.mul(params.const_2ns[1 + i * 2], tmp_10);
     AltBn128::FrElement tmp_29 = E.fr.sub(tmp_11, E.fr.set(0));
     AltBn128::FrElement tmp_12 = E.fr.mul(params.challenges[4], tmp_25);
     AltBn128::FrElement tmp_13 = E.fr.add(tmp_12, tmp_26);
     AltBn128::FrElement tmp_14 = E.fr.mul(params.challenges[4], tmp_13);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_14, tmp_27);
     AltBn128::FrElement tmp_16 = E.fr.mul(params.challenges[4], tmp_15);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, tmp_28);
     AltBn128::FrElement tmp_18 = E.fr.mul(params.challenges[4], tmp_17);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_18, tmp_29);
     AltBn128::FrElement tmp_20 = E.fr.mul(params.challenges[4], tmp_19);
     AltBn128::FrElement tmp_21 = E.fr.mul(params.cm1_2ns[0 + i*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_22 = E.fr.mul(params.cm1_2ns[1 + i*2], params.cm1_2ns[1 + i*2]);
     AltBn128::FrElement tmp_23 = E.fr.add(tmp_21, tmp_22);
     AltBn128::FrElement tmp_24 = E.fr.sub(tmp_23, params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_30 = E.fr.add(tmp_20, tmp_24);
     params.q_2ns[i] = tmp_30;
}
