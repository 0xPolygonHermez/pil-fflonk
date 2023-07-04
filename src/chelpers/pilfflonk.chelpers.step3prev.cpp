#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_4 = params.cm1_n[0 + i*2];
     AltBn128::FrElement tmp_5 = params.const_n[1 + i * 3];
     AltBn128::FrElement tmp_0 = E.fr.sub(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_1 = E.fr.mul(tmp_0, tmp_5);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_1, params.challenges[1]);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_6, params.challenges[2]);
     AltBn128::FrElement tmp_7 = params.cm1_n[1 + i*2];
     AltBn128::FrElement tmp_8 = params.const_n[2 + i * 3];
     AltBn128::FrElement tmp_2 = E.fr.sub(tmp_7, params.challenges[1]);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, tmp_8);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_3, params.challenges[1]);
     params.tmpExp_n[1 + i*2] = E.fr.add(tmp_9, params.challenges[2]);
}

void PilFflonkSteps::step3prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_4 = params.cm1_n[0 + i*2];
     AltBn128::FrElement tmp_5 = params.const_n[1 + i * 3];
     AltBn128::FrElement tmp_0 = E.fr.sub(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_1 = E.fr.mul(tmp_0, tmp_5);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_1, params.challenges[1]);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_6, params.challenges[2]);
     AltBn128::FrElement tmp_7 = params.cm1_n[1 + i*2];
     AltBn128::FrElement tmp_8 = params.const_n[2 + i * 3];
     AltBn128::FrElement tmp_2 = E.fr.sub(tmp_7, params.challenges[1]);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, tmp_8);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_3, params.challenges[1]);
     params.tmpExp_n[1 + i*2] = E.fr.add(tmp_9, params.challenges[2]);
}

void PilFflonkSteps::step3prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_4 = params.cm1_n[0 + i*2];
     AltBn128::FrElement tmp_5 = params.const_n[1 + i * 3];
     AltBn128::FrElement tmp_0 = E.fr.sub(tmp_4, params.challenges[1]);
     AltBn128::FrElement tmp_1 = E.fr.mul(tmp_0, tmp_5);
     AltBn128::FrElement tmp_6 = E.fr.add(tmp_1, params.challenges[1]);
     params.tmpExp_n[0 + i*2] = E.fr.add(tmp_6, params.challenges[2]);
     AltBn128::FrElement tmp_7 = params.cm1_n[1 + i*2];
     AltBn128::FrElement tmp_8 = params.const_n[2 + i * 3];
     AltBn128::FrElement tmp_2 = E.fr.sub(tmp_7, params.challenges[1]);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, tmp_8);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_3, params.challenges[1]);
     params.tmpExp_n[1 + i*2] = E.fr.add(tmp_9, params.challenges[2]);
}
