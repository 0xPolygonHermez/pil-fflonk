#include <alt_bn128.hpp>
#include "../constant_pols_fflonk.hpp"
#include "pilfflonk_steps.hpp"

AltBn128::FrElement PilFflonkSteps::step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm3_2ns[0 + i*1], params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.cm1_2ns[1 + i*2], params.cm1_2ns[1 + i*2]);
     AltBn128::FrElement tmp_5 = E.fr.sub(tmp_0, tmp_1);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[4], tmp_5);
     AltBn128::FrElement tmp_3 = E.fr.mul(params.cm1_2ns[0 + i*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_4 = E.fr.sub(tmp_3, params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_2, tmp_4);
     return tmp_6;
}

void PilFflonkSteps::step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm3_2ns[0 + i*1], params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.cm1_2ns[1 + i*2], params.cm1_2ns[1 + i*2]);
     AltBn128::FrElement tmp_5 = E.fr.sub(tmp_0, tmp_1);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[4], tmp_5);
     AltBn128::FrElement tmp_3 = E.fr.mul(params.cm1_2ns[0 + i*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_4 = E.fr.sub(tmp_3, params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_2, tmp_4);
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_6);
}

void PilFflonkSteps::step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm3_2ns[0 + i*1], params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.cm1_2ns[1 + i*2], params.cm1_2ns[1 + i*2]);
     AltBn128::FrElement tmp_5 = E.fr.sub(tmp_0, tmp_1);
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[4], tmp_5);
     AltBn128::FrElement tmp_3 = E.fr.mul(params.cm1_2ns[0 + i*2], params.cm1_2ns[0 + i*2]);
     AltBn128::FrElement tmp_4 = E.fr.sub(tmp_3, params.cm3_2ns[0 + i*1]);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_2, tmp_4);
     E.fr.copy(params.q_2ns[i], tmp_6);
}
