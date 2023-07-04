#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm1_2ns[0 + i*3], params.cm1_2ns[1 + i*3]);
     AltBn128::FrElement tmp_8 = E.fr.sub(tmp_0, params.cm1_2ns[2 + i*3]);
     AltBn128::FrElement tmp_1 = E.fr.sub(params.cm1_2ns[2 + i*3], params.publicInputs[0]);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_1);
     AltBn128::FrElement tmp_9 = E.fr.sub(tmp_2, E.fr.set(0));
     AltBn128::FrElement tmp_3 = E.fr.sub(params.cm1_2ns[2 + i*3], params.publicInputs[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(params.const_2ns[1 + i * 2], tmp_3);
     AltBn128::FrElement tmp_10 = E.fr.sub(tmp_4, E.fr.set(0));
     AltBn128::FrElement tmp_5 = E.fr.mul(params.challenges[4], tmp_8);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_5, tmp_9);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.challenges[4], tmp_6);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_7, tmp_10);
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_11);
}

void PilFflonkSteps::step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm1_2ns[0 + i*3], params.cm1_2ns[1 + i*3]);
     AltBn128::FrElement tmp_8 = E.fr.sub(tmp_0, params.cm1_2ns[2 + i*3]);
     AltBn128::FrElement tmp_1 = E.fr.sub(params.cm1_2ns[2 + i*3], params.publicInputs[0]);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_1);
     AltBn128::FrElement tmp_9 = E.fr.sub(tmp_2, E.fr.set(0));
     AltBn128::FrElement tmp_3 = E.fr.sub(params.cm1_2ns[2 + i*3], params.publicInputs[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(params.const_2ns[1 + i * 2], tmp_3);
     AltBn128::FrElement tmp_10 = E.fr.sub(tmp_4, E.fr.set(0));
     AltBn128::FrElement tmp_5 = E.fr.mul(params.challenges[4], tmp_8);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_5, tmp_9);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.challenges[4], tmp_6);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_7, tmp_10);
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_11);
}

void PilFflonkSteps::step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm1_2ns[0 + i*3], params.cm1_2ns[1 + i*3]);
     AltBn128::FrElement tmp_8 = E.fr.sub(tmp_0, params.cm1_2ns[2 + i*3]);
     AltBn128::FrElement tmp_1 = E.fr.sub(params.cm1_2ns[2 + i*3], params.publicInputs[0]);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.const_2ns[0 + i * 2], tmp_1);
     AltBn128::FrElement tmp_9 = E.fr.sub(tmp_2, E.fr.set(0));
     AltBn128::FrElement tmp_3 = E.fr.sub(params.cm1_2ns[2 + i*3], params.publicInputs[1]);
     AltBn128::FrElement tmp_4 = E.fr.mul(params.const_2ns[1 + i * 2], tmp_3);
     AltBn128::FrElement tmp_10 = E.fr.sub(tmp_4, E.fr.set(0));
     AltBn128::FrElement tmp_5 = E.fr.mul(params.challenges[4], tmp_8);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_5, tmp_9);
     AltBn128::FrElement tmp_7 = E.fr.mul(params.challenges[4], tmp_6);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_7, tmp_10);
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_11);
}
