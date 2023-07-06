#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_18 = params.cm1_n[0 + i*3];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_18, tmp_0);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_1, params.challenges[3]);
     AltBn128::FrElement tmp_20 = params.cm1_n[1 + i*3];
     AltBn128::FrElement v0;
     E.fr.fromString(v0, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[2], v0);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, params.x_n[i]);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_20, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_19, tmp_5);
     AltBn128::FrElement tmp_22 = params.cm1_n[2 + i*3];
     AltBn128::FrElement v1;
     E.fr.fromString(v1, "7105417016332174850410570896152637638969286250121046049601916147253836355616");
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], v1);
     AltBn128::FrElement tmp_7 = E.fr.mul(tmp_6, params.x_n[i]);
     AltBn128::FrElement tmp_8 = E.fr.add(tmp_22, tmp_7);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_8, params.challenges[3]);
     params.cm3_n[1 + i*3] = E.fr.mul(tmp_21, tmp_9);
     AltBn128::FrElement tmp_23 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_10 = E.fr.mul(params.challenges[2], tmp_23);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_18, tmp_10);
     AltBn128::FrElement tmp_24 = E.fr.add(tmp_11, params.challenges[3]);
     AltBn128::FrElement tmp_25 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_12 = E.fr.mul(params.challenges[2], tmp_25);
     AltBn128::FrElement tmp_13 = E.fr.add(tmp_20, tmp_12);
     AltBn128::FrElement tmp_14 = E.fr.add(tmp_13, params.challenges[3]);
     AltBn128::FrElement tmp_26 = E.fr.mul(tmp_24, tmp_14);
     AltBn128::FrElement tmp_27 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_15 = E.fr.mul(params.challenges[2], tmp_27);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_22, tmp_15);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, params.challenges[3]);
     params.cm3_n[2 + i*3] = E.fr.mul(tmp_26, tmp_17);
}

void PilFflonkSteps::step3prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_18 = params.cm1_n[0 + i*3];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_18, tmp_0);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_1, params.challenges[3]);
     AltBn128::FrElement tmp_20 = params.cm1_n[1 + i*3];
     AltBn128::FrElement v0;
     E.fr.fromString(v0, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[2], v0);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, params.x_n[i]);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_20, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_19, tmp_5);
     AltBn128::FrElement tmp_22 = params.cm1_n[2 + i*3];
     AltBn128::FrElement v1;
     E.fr.fromString(v1, "7105417016332174850410570896152637638969286250121046049601916147253836355616");
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], v1);
     AltBn128::FrElement tmp_7 = E.fr.mul(tmp_6, params.x_n[i]);
     AltBn128::FrElement tmp_8 = E.fr.add(tmp_22, tmp_7);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_8, params.challenges[3]);
     params.cm3_n[1 + i*3] = E.fr.mul(tmp_21, tmp_9);
     AltBn128::FrElement tmp_23 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_10 = E.fr.mul(params.challenges[2], tmp_23);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_18, tmp_10);
     AltBn128::FrElement tmp_24 = E.fr.add(tmp_11, params.challenges[3]);
     AltBn128::FrElement tmp_25 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_12 = E.fr.mul(params.challenges[2], tmp_25);
     AltBn128::FrElement tmp_13 = E.fr.add(tmp_20, tmp_12);
     AltBn128::FrElement tmp_14 = E.fr.add(tmp_13, params.challenges[3]);
     AltBn128::FrElement tmp_26 = E.fr.mul(tmp_24, tmp_14);
     AltBn128::FrElement tmp_27 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_15 = E.fr.mul(params.challenges[2], tmp_27);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_22, tmp_15);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, params.challenges[3]);
     params.cm3_n[2 + i*3] = E.fr.mul(tmp_26, tmp_17);
}

void PilFflonkSteps::step3prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_18 = params.cm1_n[0 + i*3];
     AltBn128::FrElement tmp_0 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_1 = E.fr.add(tmp_18, tmp_0);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_1, params.challenges[3]);
     AltBn128::FrElement tmp_20 = params.cm1_n[1 + i*3];
     AltBn128::FrElement v0;
     E.fr.fromString(v0, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_2 = E.fr.mul(params.challenges[2], v0);
     AltBn128::FrElement tmp_3 = E.fr.mul(tmp_2, params.x_n[i]);
     AltBn128::FrElement tmp_4 = E.fr.add(tmp_20, tmp_3);
     AltBn128::FrElement tmp_5 = E.fr.add(tmp_4, params.challenges[3]);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_19, tmp_5);
     AltBn128::FrElement tmp_22 = params.cm1_n[2 + i*3];
     AltBn128::FrElement v1;
     E.fr.fromString(v1, "7105417016332174850410570896152637638969286250121046049601916147253836355616");
     AltBn128::FrElement tmp_6 = E.fr.mul(params.challenges[2], v1);
     AltBn128::FrElement tmp_7 = E.fr.mul(tmp_6, params.x_n[i]);
     AltBn128::FrElement tmp_8 = E.fr.add(tmp_22, tmp_7);
     AltBn128::FrElement tmp_9 = E.fr.add(tmp_8, params.challenges[3]);
     params.cm3_n[1 + i*3] = E.fr.mul(tmp_21, tmp_9);
     AltBn128::FrElement tmp_23 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_10 = E.fr.mul(params.challenges[2], tmp_23);
     AltBn128::FrElement tmp_11 = E.fr.add(tmp_18, tmp_10);
     AltBn128::FrElement tmp_24 = E.fr.add(tmp_11, params.challenges[3]);
     AltBn128::FrElement tmp_25 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_12 = E.fr.mul(params.challenges[2], tmp_25);
     AltBn128::FrElement tmp_13 = E.fr.add(tmp_20, tmp_12);
     AltBn128::FrElement tmp_14 = E.fr.add(tmp_13, params.challenges[3]);
     AltBn128::FrElement tmp_26 = E.fr.mul(tmp_24, tmp_14);
     AltBn128::FrElement tmp_27 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_15 = E.fr.mul(params.challenges[2], tmp_27);
     AltBn128::FrElement tmp_16 = E.fr.add(tmp_22, tmp_15);
     AltBn128::FrElement tmp_17 = E.fr.add(tmp_16, params.challenges[3]);
     params.cm3_n[2 + i*3] = E.fr.mul(tmp_26, tmp_17);
}
