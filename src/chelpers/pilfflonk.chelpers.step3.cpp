#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_36 = params.cm1_n[0 + i*3];
     AltBn128::FrElement tmp_18 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_36, tmp_18);
     AltBn128::FrElement tmp_37 = E.fr.add(tmp_19, params.challenges[3]);
     AltBn128::FrElement tmp_38 = params.cm1_n[1 + i*3];
     AltBn128::FrElement v0;
     E.fr.fromString(v0, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_20 = E.fr.mul(params.challenges[2], v0);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_20, params.x_n[i]);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_38, tmp_21);
     AltBn128::FrElement tmp_23 = E.fr.add(tmp_22, params.challenges[3]);
     AltBn128::FrElement tmp_39 = E.fr.mul(tmp_37, tmp_23);
     AltBn128::FrElement tmp_40 = params.cm1_n[2 + i*3];
     AltBn128::FrElement v1;
     E.fr.fromString(v1, "7105417016332174850410570896152637638969286250121046049601916147253836355616");
     AltBn128::FrElement tmp_24 = E.fr.mul(params.challenges[2], v1);
     AltBn128::FrElement tmp_25 = E.fr.mul(tmp_24, params.x_n[i]);
     AltBn128::FrElement tmp_26 = E.fr.add(tmp_40, tmp_25);
     AltBn128::FrElement tmp_27 = E.fr.add(tmp_26, params.challenges[3]);
     params.cm3_n[1 + i*3] = E.fr.mul(tmp_39, tmp_27);
     AltBn128::FrElement tmp_41 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_28 = E.fr.mul(params.challenges[2], tmp_41);
     AltBn128::FrElement tmp_29 = E.fr.add(tmp_36, tmp_28);
     AltBn128::FrElement tmp_42 = E.fr.add(tmp_29, params.challenges[3]);
     AltBn128::FrElement tmp_43 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_30 = E.fr.mul(params.challenges[2], tmp_43);
     AltBn128::FrElement tmp_31 = E.fr.add(tmp_38, tmp_30);
     AltBn128::FrElement tmp_32 = E.fr.add(tmp_31, params.challenges[3]);
     AltBn128::FrElement tmp_44 = E.fr.mul(tmp_42, tmp_32);
     AltBn128::FrElement tmp_45 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_33 = E.fr.mul(params.challenges[2], tmp_45);
     AltBn128::FrElement tmp_34 = E.fr.add(tmp_40, tmp_33);
     AltBn128::FrElement tmp_35 = E.fr.add(tmp_34, params.challenges[3]);
     params.cm3_n[2 + i*3] = E.fr.mul(tmp_44, tmp_35);
}

void PilFflonkSteps::step3_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_36 = params.cm1_n[0 + i*3];
     AltBn128::FrElement tmp_18 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_36, tmp_18);
     AltBn128::FrElement tmp_37 = E.fr.add(tmp_19, params.challenges[3]);
     AltBn128::FrElement tmp_38 = params.cm1_n[1 + i*3];
     AltBn128::FrElement v0;
     E.fr.fromString(v0, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_20 = E.fr.mul(params.challenges[2], v0);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_20, params.x_n[i]);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_38, tmp_21);
     AltBn128::FrElement tmp_23 = E.fr.add(tmp_22, params.challenges[3]);
     AltBn128::FrElement tmp_39 = E.fr.mul(tmp_37, tmp_23);
     AltBn128::FrElement tmp_40 = params.cm1_n[2 + i*3];
     AltBn128::FrElement v1;
     E.fr.fromString(v1, "7105417016332174850410570896152637638969286250121046049601916147253836355616");
     AltBn128::FrElement tmp_24 = E.fr.mul(params.challenges[2], v1);
     AltBn128::FrElement tmp_25 = E.fr.mul(tmp_24, params.x_n[i]);
     AltBn128::FrElement tmp_26 = E.fr.add(tmp_40, tmp_25);
     AltBn128::FrElement tmp_27 = E.fr.add(tmp_26, params.challenges[3]);
     params.cm3_n[1 + i*3] = E.fr.mul(tmp_39, tmp_27);
     AltBn128::FrElement tmp_41 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_28 = E.fr.mul(params.challenges[2], tmp_41);
     AltBn128::FrElement tmp_29 = E.fr.add(tmp_36, tmp_28);
     AltBn128::FrElement tmp_42 = E.fr.add(tmp_29, params.challenges[3]);
     AltBn128::FrElement tmp_43 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_30 = E.fr.mul(params.challenges[2], tmp_43);
     AltBn128::FrElement tmp_31 = E.fr.add(tmp_38, tmp_30);
     AltBn128::FrElement tmp_32 = E.fr.add(tmp_31, params.challenges[3]);
     AltBn128::FrElement tmp_44 = E.fr.mul(tmp_42, tmp_32);
     AltBn128::FrElement tmp_45 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_33 = E.fr.mul(params.challenges[2], tmp_45);
     AltBn128::FrElement tmp_34 = E.fr.add(tmp_40, tmp_33);
     AltBn128::FrElement tmp_35 = E.fr.add(tmp_34, params.challenges[3]);
     params.cm3_n[2 + i*3] = E.fr.mul(tmp_44, tmp_35);
}

void PilFflonkSteps::step3_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_36 = params.cm1_n[0 + i*3];
     AltBn128::FrElement tmp_18 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_19 = E.fr.add(tmp_36, tmp_18);
     AltBn128::FrElement tmp_37 = E.fr.add(tmp_19, params.challenges[3]);
     AltBn128::FrElement tmp_38 = params.cm1_n[1 + i*3];
     AltBn128::FrElement v0;
     E.fr.fromString(v0, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     AltBn128::FrElement tmp_20 = E.fr.mul(params.challenges[2], v0);
     AltBn128::FrElement tmp_21 = E.fr.mul(tmp_20, params.x_n[i]);
     AltBn128::FrElement tmp_22 = E.fr.add(tmp_38, tmp_21);
     AltBn128::FrElement tmp_23 = E.fr.add(tmp_22, params.challenges[3]);
     AltBn128::FrElement tmp_39 = E.fr.mul(tmp_37, tmp_23);
     AltBn128::FrElement tmp_40 = params.cm1_n[2 + i*3];
     AltBn128::FrElement v1;
     E.fr.fromString(v1, "7105417016332174850410570896152637638969286250121046049601916147253836355616");
     AltBn128::FrElement tmp_24 = E.fr.mul(params.challenges[2], v1);
     AltBn128::FrElement tmp_25 = E.fr.mul(tmp_24, params.x_n[i]);
     AltBn128::FrElement tmp_26 = E.fr.add(tmp_40, tmp_25);
     AltBn128::FrElement tmp_27 = E.fr.add(tmp_26, params.challenges[3]);
     params.cm3_n[1 + i*3] = E.fr.mul(tmp_39, tmp_27);
     AltBn128::FrElement tmp_41 = params.const_n[1 + i * 4];
     AltBn128::FrElement tmp_28 = E.fr.mul(params.challenges[2], tmp_41);
     AltBn128::FrElement tmp_29 = E.fr.add(tmp_36, tmp_28);
     AltBn128::FrElement tmp_42 = E.fr.add(tmp_29, params.challenges[3]);
     AltBn128::FrElement tmp_43 = params.const_n[2 + i * 4];
     AltBn128::FrElement tmp_30 = E.fr.mul(params.challenges[2], tmp_43);
     AltBn128::FrElement tmp_31 = E.fr.add(tmp_38, tmp_30);
     AltBn128::FrElement tmp_32 = E.fr.add(tmp_31, params.challenges[3]);
     AltBn128::FrElement tmp_44 = E.fr.mul(tmp_42, tmp_32);
     AltBn128::FrElement tmp_45 = params.const_n[3 + i * 4];
     AltBn128::FrElement tmp_33 = E.fr.mul(params.challenges[2], tmp_45);
     AltBn128::FrElement tmp_34 = E.fr.add(tmp_40, tmp_33);
     AltBn128::FrElement tmp_35 = E.fr.add(tmp_34, params.challenges[3]);
     params.cm3_n[2 + i*3] = E.fr.mul(tmp_44, tmp_35);
}
