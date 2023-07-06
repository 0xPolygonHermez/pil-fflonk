#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_52 = params.cm1_n[0 + i*2];
     params.tmpExp_n[0 + i*6] = tmp_52;
     AltBn128::FrElement tmp_53 = params.const_n[1 + i * 4];
     params.tmpExp_n[1 + i*6] = tmp_53;
     AltBn128::FrElement tmp_54 = params.const_n[1 + ((i + 1)%8) * 4];
     params.tmpExp_n[1 + ((i + 1)%8)*6] = tmp_54;
     AltBn128::FrElement tmp_36 = E.fr.add(params.tmpExp_n[0 + i*6], params.challenges[3]);
     AltBn128::FrElement tmp_37 = E.fr.mul(params.tmpExp_n[1 + ((i + 1)%8)*6], params.challenges[2]);
     AltBn128::FrElement tmp_38 = E.fr.add(params.tmpExp_n[1 + i*6], tmp_37);
     AltBn128::FrElement tmp_39 = E.fr.add(E.fr.set(1), params.challenges[2]);
     AltBn128::FrElement tmp_40 = E.fr.mul(params.challenges[3], tmp_39);
     AltBn128::FrElement tmp_41 = E.fr.add(tmp_38, tmp_40);
     AltBn128::FrElement tmp_42 = E.fr.mul(tmp_36, tmp_41);
     AltBn128::FrElement tmp_43 = E.fr.add(E.fr.set(1), params.challenges[2]);
     params.cm3_n[2 + i*4] = E.fr.mul(tmp_42, tmp_43);
     AltBn128::FrElement tmp_55 = params.cm1_n[1 + i*2];
     params.tmpExp_n[2 + i*6] = tmp_55;
     AltBn128::FrElement tmp_56 = params.const_n[2 + i * 4];
     params.tmpExp_n[3 + i*6] = tmp_56;
     AltBn128::FrElement tmp_57 = params.const_n[2 + ((i + 1)%8) * 4];
     params.tmpExp_n[3 + ((i + 1)%8)*6] = tmp_57;
     AltBn128::FrElement tmp_44 = E.fr.add(params.tmpExp_n[2 + i*6], params.challenges[3]);
     AltBn128::FrElement tmp_45 = E.fr.mul(params.tmpExp_n[3 + ((i + 1)%8)*6], params.challenges[2]);
     AltBn128::FrElement tmp_46 = E.fr.add(params.tmpExp_n[3 + i*6], tmp_45);
     AltBn128::FrElement tmp_47 = E.fr.add(E.fr.set(1), params.challenges[2]);
     AltBn128::FrElement tmp_48 = E.fr.mul(params.challenges[3], tmp_47);
     AltBn128::FrElement tmp_49 = E.fr.add(tmp_46, tmp_48);
     AltBn128::FrElement tmp_50 = E.fr.mul(tmp_44, tmp_49);
     AltBn128::FrElement tmp_51 = E.fr.add(E.fr.set(1), params.challenges[2]);
     params.cm3_n[3 + i*4] = E.fr.mul(tmp_50, tmp_51);
}

void PilFflonkSteps::step3_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_52 = params.cm1_n[0 + i*2];
     params.tmpExp_n[0 + i*6] = tmp_52;
     AltBn128::FrElement tmp_53 = params.const_n[1 + i * 4];
     params.tmpExp_n[1 + i*6] = tmp_53;
     AltBn128::FrElement tmp_54 = params.const_n[1 + ((i + 1)%8) * 4];
     params.tmpExp_n[1 + ((i + 1)%8)*6] = tmp_54;
     AltBn128::FrElement tmp_36 = E.fr.add(params.tmpExp_n[0 + i*6], params.challenges[3]);
     AltBn128::FrElement tmp_37 = E.fr.mul(params.tmpExp_n[1 + ((i + 1)%8)*6], params.challenges[2]);
     AltBn128::FrElement tmp_38 = E.fr.add(params.tmpExp_n[1 + i*6], tmp_37);
     AltBn128::FrElement tmp_39 = E.fr.add(E.fr.set(1), params.challenges[2]);
     AltBn128::FrElement tmp_40 = E.fr.mul(params.challenges[3], tmp_39);
     AltBn128::FrElement tmp_41 = E.fr.add(tmp_38, tmp_40);
     AltBn128::FrElement tmp_42 = E.fr.mul(tmp_36, tmp_41);
     AltBn128::FrElement tmp_43 = E.fr.add(E.fr.set(1), params.challenges[2]);
     params.cm3_n[2 + i*4] = E.fr.mul(tmp_42, tmp_43);
     AltBn128::FrElement tmp_55 = params.cm1_n[1 + i*2];
     params.tmpExp_n[2 + i*6] = tmp_55;
     AltBn128::FrElement tmp_56 = params.const_n[2 + i * 4];
     params.tmpExp_n[3 + i*6] = tmp_56;
     AltBn128::FrElement tmp_57 = params.const_n[2 + ((i + 1)%8) * 4];
     params.tmpExp_n[3 + ((i + 1)%8)*6] = tmp_57;
     AltBn128::FrElement tmp_44 = E.fr.add(params.tmpExp_n[2 + i*6], params.challenges[3]);
     AltBn128::FrElement tmp_45 = E.fr.mul(params.tmpExp_n[3 + ((i + 1)%8)*6], params.challenges[2]);
     AltBn128::FrElement tmp_46 = E.fr.add(params.tmpExp_n[3 + i*6], tmp_45);
     AltBn128::FrElement tmp_47 = E.fr.add(E.fr.set(1), params.challenges[2]);
     AltBn128::FrElement tmp_48 = E.fr.mul(params.challenges[3], tmp_47);
     AltBn128::FrElement tmp_49 = E.fr.add(tmp_46, tmp_48);
     AltBn128::FrElement tmp_50 = E.fr.mul(tmp_44, tmp_49);
     AltBn128::FrElement tmp_51 = E.fr.add(E.fr.set(1), params.challenges[2]);
     params.cm3_n[3 + i*4] = E.fr.mul(tmp_50, tmp_51);
}

void PilFflonkSteps::step3_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_52 = params.cm1_n[0 + i*2];
     params.tmpExp_n[0 + i*6] = tmp_52;
     AltBn128::FrElement tmp_53 = params.const_n[1 + i * 4];
     params.tmpExp_n[1 + i*6] = tmp_53;
     AltBn128::FrElement tmp_54 = params.const_n[1 + ((i + 1)%8) * 4];
     params.tmpExp_n[1 + ((i + 1)%8)*6] = tmp_54;
     AltBn128::FrElement tmp_36 = E.fr.add(params.tmpExp_n[0 + i*6], params.challenges[3]);
     AltBn128::FrElement tmp_37 = E.fr.mul(params.tmpExp_n[1 + ((i + 1)%8)*6], params.challenges[2]);
     AltBn128::FrElement tmp_38 = E.fr.add(params.tmpExp_n[1 + i*6], tmp_37);
     AltBn128::FrElement tmp_39 = E.fr.add(E.fr.set(1), params.challenges[2]);
     AltBn128::FrElement tmp_40 = E.fr.mul(params.challenges[3], tmp_39);
     AltBn128::FrElement tmp_41 = E.fr.add(tmp_38, tmp_40);
     AltBn128::FrElement tmp_42 = E.fr.mul(tmp_36, tmp_41);
     AltBn128::FrElement tmp_43 = E.fr.add(E.fr.set(1), params.challenges[2]);
     params.cm3_n[2 + i*4] = E.fr.mul(tmp_42, tmp_43);
     AltBn128::FrElement tmp_55 = params.cm1_n[1 + i*2];
     params.tmpExp_n[2 + i*6] = tmp_55;
     AltBn128::FrElement tmp_56 = params.const_n[2 + i * 4];
     params.tmpExp_n[3 + i*6] = tmp_56;
     AltBn128::FrElement tmp_57 = params.const_n[2 + ((i + 1)%8) * 4];
     params.tmpExp_n[3 + ((i + 1)%8)*6] = tmp_57;
     AltBn128::FrElement tmp_44 = E.fr.add(params.tmpExp_n[2 + i*6], params.challenges[3]);
     AltBn128::FrElement tmp_45 = E.fr.mul(params.tmpExp_n[3 + ((i + 1)%8)*6], params.challenges[2]);
     AltBn128::FrElement tmp_46 = E.fr.add(params.tmpExp_n[3 + i*6], tmp_45);
     AltBn128::FrElement tmp_47 = E.fr.add(E.fr.set(1), params.challenges[2]);
     AltBn128::FrElement tmp_48 = E.fr.mul(params.challenges[3], tmp_47);
     AltBn128::FrElement tmp_49 = E.fr.add(tmp_46, tmp_48);
     AltBn128::FrElement tmp_50 = E.fr.mul(tmp_44, tmp_49);
     AltBn128::FrElement tmp_51 = E.fr.add(E.fr.set(1), params.challenges[2]);
     params.cm3_n[3 + i*4] = E.fr.mul(tmp_50, tmp_51);
}
