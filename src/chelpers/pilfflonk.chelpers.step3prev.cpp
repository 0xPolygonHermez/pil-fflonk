#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
    AltBn128::FrElement tmp_11 = params.cm1_n[0 + i*2];
    AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[2], params.x_n[i]);
    AltBn128::FrElement tmp_1 = E.fr.add(tmp_11, tmp_0);
    AltBn128::FrElement tmp_12 = E.fr.add(tmp_1, params.challenges[3]);
    AltBn128::FrElement tmp_13 = params.cm1_n[1 + i*2];
    AltBn128::FrElement v;
    E.fr.fromString(v, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
    AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[2], v);
    AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, params.x_n[i]);
    AltBn128::FrElement tmp_4 = E.fr.add(tmp_13, tmp_3);
    AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
    params.tmpExp_n[0 + i*2] = E.fr.mul(tmp_12, tmp_5);
    AltBn128::FrElement tmp_14 = params.const_n[1 + i * 3];
    AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], tmp_14);
    AltBn128::FrElement tmp_7 = E.fr.add(tmp_11, tmp_6);
    AltBn128::FrElement tmp_15 = E.fr.add(tmp_7, params.challenges[3]);
    AltBn128::FrElement tmp_16 = params.const_n[2 + i * 3];
    AltBn128::FrElement tmp_8 = E.fr.mul(params.challenges[2], tmp_16);
    AltBn128::FrElement tmp_9 = E.fr.add(tmp_13, tmp_8);
    AltBn128::FrElement tmp_10 = E.fr.add(tmp_9, params.challenges[3]);
    params.tmpExp_n[1 + i*2] = E.fr.mul(tmp_15, tmp_10);
}

void PilFflonkSteps::step3prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_11 = params.cm1_n[0 + i*2];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_11, tmp_0);
     AltBn128::FrElement tmp_12 = E.fr.add(tmp_1, params.challenges[3]);
     AltBn128::FrElement tmp_13 = params.cm1_n[1 + i*2];
     AltBn128::FrElement v;
     E.fr.fromString(v, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[2], v);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, params.x_n[i]);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_13, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     params.tmpExp_n[0 + i*2] = E.fr.mul(tmp_12, tmp_5);
     AltBn128::FrElement tmp_14 = params.const_n[1 + i * 3];
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], tmp_14);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_11, tmp_6);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_7, params.challenges[3]);
     AltBn128::FrElement tmp_16 = params.const_n[2 + i * 3];
     AltBn128::FrElement tmp_8 = E.fr.mul(params.challenges[2], tmp_16);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_13, tmp_8);
     AltBn128::FrElement tmp_10 = E.fr.add(tmp_9, params.challenges[3]);
     params.tmpExp_n[1 + i*2] = E.fr.mul(tmp_15, tmp_10);
}

void PilFflonkSteps::step3prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_11 = params.cm1_n[0 + i*2];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_11, tmp_0);
     AltBn128::FrElement tmp_12 = E.fr.add(tmp_1, params.challenges[3]);
     AltBn128::FrElement tmp_13 = params.cm1_n[1 + i*2];
     AltBn128::FrElement v;
     E.fr.fromString(v, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[2], v);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, params.x_n[i]);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_13, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     params.tmpExp_n[0 + i*2] = E.fr.mul(tmp_12, tmp_5);
     AltBn128::FrElement tmp_14 = params.const_n[1 + i * 3];
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], tmp_14);
     AltBn128::FrElement tmp_7 = E.fr.add(tmp_11, tmp_6);
     AltBn128::FrElement tmp_15 = E.fr.add(tmp_7, params.challenges[3]);
     AltBn128::FrElement tmp_16 = params.const_n[2 + i * 3];
     AltBn128::FrElement tmp_8 = E.fr.mul(params.challenges[2], tmp_16);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_13, tmp_8);
     AltBn128::FrElement tmp_10 = E.fr.add(tmp_9, params.challenges[3]);
     params.tmpExp_n[1 + i*2] = E.fr.mul(tmp_15, tmp_10);
}
