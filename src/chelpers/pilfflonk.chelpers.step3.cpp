#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_44 = params.cm1_n[2 + i*11];
     AltBn128::FrElement tmp_26 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_27 = E.fr.add(tmp_44, tmp_26);
     AltBn128::FrElement tmp_45 = E.fr.add(tmp_27, params.challenges[3]);
     AltBn128::FrElement tmp_46 = params.cm1_n[3 + i*11];
     AltBn128::FrElement tmp_28 = E.fr.mul(params.challenges[2], params.constValues[0]);
     AltBn128::FrElement tmp_29 = E.fr.mul(tmp_28, params.x_n[i]);
     AltBn128::FrElement tmp_30 = E.fr.add(tmp_46, tmp_29);
     AltBn128::FrElement tmp_31 = E.fr.add(tmp_30, params.challenges[3]);
     AltBn128::FrElement tmp_47 = E.fr.mul(tmp_45, tmp_31);
     AltBn128::FrElement tmp_48 = params.cm1_n[4 + i*11];
     AltBn128::FrElement tmp_32 = E.fr.mul(params.challenges[2], params.constValues[1]);
     AltBn128::FrElement tmp_33 = E.fr.mul(tmp_32, params.x_n[i]);
     AltBn128::FrElement tmp_34 = E.fr.add(tmp_48, tmp_33);
     AltBn128::FrElement tmp_35 = E.fr.add(tmp_34, params.challenges[3]);
     params.cm3_n[2 + i*4] = E.fr.mul(tmp_47, tmp_35);
     AltBn128::FrElement tmp_49 = params.const_n[3 + i * 6];
     AltBn128::FrElement tmp_36 = E.fr.mul(params.challenges[2], tmp_49);
     AltBn128::FrElement tmp_37 = E.fr.add(tmp_44, tmp_36);
     AltBn128::FrElement tmp_50 = E.fr.add(tmp_37, params.challenges[3]);
     AltBn128::FrElement tmp_51 = params.const_n[4 + i * 6];
     AltBn128::FrElement tmp_38 = E.fr.mul(params.challenges[2], tmp_51);
     AltBn128::FrElement tmp_39 = E.fr.add(tmp_46, tmp_38);
     AltBn128::FrElement tmp_40 = E.fr.add(tmp_39, params.challenges[3]);
     AltBn128::FrElement tmp_52 = E.fr.mul(tmp_50, tmp_40);
     AltBn128::FrElement tmp_53 = params.const_n[5 + i * 6];
     AltBn128::FrElement tmp_41 = E.fr.mul(params.challenges[2], tmp_53);
     AltBn128::FrElement tmp_42 = E.fr.add(tmp_48, tmp_41);
     AltBn128::FrElement tmp_43 = E.fr.add(tmp_42, params.challenges[3]);
     params.cm3_n[3 + i*4] = E.fr.mul(tmp_52, tmp_43);
}

void PilFflonkSteps::step3_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_44 = params.cm1_n[2 + i*11];
     AltBn128::FrElement tmp_26 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_27 = E.fr.add(tmp_44, tmp_26);
     AltBn128::FrElement tmp_45 = E.fr.add(tmp_27, params.challenges[3]);
     AltBn128::FrElement tmp_46 = params.cm1_n[3 + i*11];
     AltBn128::FrElement tmp_28 = E.fr.mul(params.challenges[2], params.constValues[0]);
     AltBn128::FrElement tmp_29 = E.fr.mul(tmp_28, params.x_n[i]);
     AltBn128::FrElement tmp_30 = E.fr.add(tmp_46, tmp_29);
     AltBn128::FrElement tmp_31 = E.fr.add(tmp_30, params.challenges[3]);
     AltBn128::FrElement tmp_47 = E.fr.mul(tmp_45, tmp_31);
     AltBn128::FrElement tmp_48 = params.cm1_n[4 + i*11];
     AltBn128::FrElement tmp_32 = E.fr.mul(params.challenges[2], params.constValues[1]);
     AltBn128::FrElement tmp_33 = E.fr.mul(tmp_32, params.x_n[i]);
     AltBn128::FrElement tmp_34 = E.fr.add(tmp_48, tmp_33);
     AltBn128::FrElement tmp_35 = E.fr.add(tmp_34, params.challenges[3]);
     params.cm3_n[2 + i*4] = E.fr.mul(tmp_47, tmp_35);
     AltBn128::FrElement tmp_49 = params.const_n[3 + i * 6];
     AltBn128::FrElement tmp_36 = E.fr.mul(params.challenges[2], tmp_49);
     AltBn128::FrElement tmp_37 = E.fr.add(tmp_44, tmp_36);
     AltBn128::FrElement tmp_50 = E.fr.add(tmp_37, params.challenges[3]);
     AltBn128::FrElement tmp_51 = params.const_n[4 + i * 6];
     AltBn128::FrElement tmp_38 = E.fr.mul(params.challenges[2], tmp_51);
     AltBn128::FrElement tmp_39 = E.fr.add(tmp_46, tmp_38);
     AltBn128::FrElement tmp_40 = E.fr.add(tmp_39, params.challenges[3]);
     AltBn128::FrElement tmp_52 = E.fr.mul(tmp_50, tmp_40);
     AltBn128::FrElement tmp_53 = params.const_n[5 + i * 6];
     AltBn128::FrElement tmp_41 = E.fr.mul(params.challenges[2], tmp_53);
     AltBn128::FrElement tmp_42 = E.fr.add(tmp_48, tmp_41);
     AltBn128::FrElement tmp_43 = E.fr.add(tmp_42, params.challenges[3]);
     params.cm3_n[3 + i*4] = E.fr.mul(tmp_52, tmp_43);
}

void PilFflonkSteps::step3_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_44 = params.cm1_n[2 + i*11];
     AltBn128::FrElement tmp_26 = E.fr.mul(params.challenges[2], params.x_n[i]);
     AltBn128::FrElement tmp_27 = E.fr.add(tmp_44, tmp_26);
     AltBn128::FrElement tmp_45 = E.fr.add(tmp_27, params.challenges[3]);
     AltBn128::FrElement tmp_46 = params.cm1_n[3 + i*11];
     AltBn128::FrElement tmp_28 = E.fr.mul(params.challenges[2], params.constValues[0]);
     AltBn128::FrElement tmp_29 = E.fr.mul(tmp_28, params.x_n[i]);
     AltBn128::FrElement tmp_30 = E.fr.add(tmp_46, tmp_29);
     AltBn128::FrElement tmp_31 = E.fr.add(tmp_30, params.challenges[3]);
     AltBn128::FrElement tmp_47 = E.fr.mul(tmp_45, tmp_31);
     AltBn128::FrElement tmp_48 = params.cm1_n[4 + i*11];
     AltBn128::FrElement tmp_32 = E.fr.mul(params.challenges[2], params.constValues[1]);
     AltBn128::FrElement tmp_33 = E.fr.mul(tmp_32, params.x_n[i]);
     AltBn128::FrElement tmp_34 = E.fr.add(tmp_48, tmp_33);
     AltBn128::FrElement tmp_35 = E.fr.add(tmp_34, params.challenges[3]);
     params.cm3_n[2 + i*4] = E.fr.mul(tmp_47, tmp_35);
     AltBn128::FrElement tmp_49 = params.const_n[3 + i * 6];
     AltBn128::FrElement tmp_36 = E.fr.mul(params.challenges[2], tmp_49);
     AltBn128::FrElement tmp_37 = E.fr.add(tmp_44, tmp_36);
     AltBn128::FrElement tmp_50 = E.fr.add(tmp_37, params.challenges[3]);
     AltBn128::FrElement tmp_51 = params.const_n[4 + i * 6];
     AltBn128::FrElement tmp_38 = E.fr.mul(params.challenges[2], tmp_51);
     AltBn128::FrElement tmp_39 = E.fr.add(tmp_46, tmp_38);
     AltBn128::FrElement tmp_40 = E.fr.add(tmp_39, params.challenges[3]);
     AltBn128::FrElement tmp_52 = E.fr.mul(tmp_50, tmp_40);
     AltBn128::FrElement tmp_53 = params.const_n[5 + i * 6];
     AltBn128::FrElement tmp_41 = E.fr.mul(params.challenges[2], tmp_53);
     AltBn128::FrElement tmp_42 = E.fr.add(tmp_48, tmp_41);
     AltBn128::FrElement tmp_43 = E.fr.add(tmp_42, params.challenges[3]);
     params.cm3_n[3 + i*4] = E.fr.mul(tmp_52, tmp_43);
}
