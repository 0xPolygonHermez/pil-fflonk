#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step3_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm1_n[0 + i*2], params.cm1_n[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.cm1_n[1 + i*2], params.cm1_n[1 + i*2]);
     params.cm3_n[0 + i*1] = E.fr.add(tmp_0, tmp_1);
}

void PilFflonkSteps::step3_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm1_n[0 + i*2], params.cm1_n[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.cm1_n[1 + i*2], params.cm1_n[1 + i*2]);
     params.cm3_n[0 + i*1] = E.fr.add(tmp_0, tmp_1);
}

void PilFflonkSteps::step3_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.cm1_n[0 + i*2], params.cm1_n[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.cm1_n[1 + i*2], params.cm1_n[1 + i*2]);
     params.cm3_n[0 + i*1] = E.fr.add(tmp_0, tmp_1);
}
