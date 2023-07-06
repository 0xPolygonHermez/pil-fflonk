#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step2prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = params.cm1_n[0 + i*2];
     params.tmpExp_n[0 + i*6] = tmp_0;
     AltBn128::FrElement tmp_1 = params.const_n[1 + i * 4];
     params.tmpExp_n[1 + i*6] = tmp_1;
     AltBn128::FrElement tmp_2 = params.cm1_n[1 + i*2];
     params.tmpExp_n[2 + i*6] = tmp_2;
     AltBn128::FrElement tmp_3 = params.const_n[2 + i * 4];
     params.tmpExp_n[3 + i*6] = tmp_3;
}

void PilFflonkSteps::step2prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = params.cm1_n[0 + i*2];
     params.tmpExp_n[0 + i*6] = tmp_0;
     AltBn128::FrElement tmp_1 = params.const_n[1 + i * 4];
     params.tmpExp_n[1 + i*6] = tmp_1;
     AltBn128::FrElement tmp_2 = params.cm1_n[1 + i*2];
     params.tmpExp_n[2 + i*6] = tmp_2;
     AltBn128::FrElement tmp_3 = params.const_n[2 + i * 4];
     params.tmpExp_n[3 + i*6] = tmp_3;
}

void PilFflonkSteps::step2prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = params.cm1_n[0 + i*2];
     params.tmpExp_n[0 + i*6] = tmp_0;
     AltBn128::FrElement tmp_1 = params.const_n[1 + i * 4];
     params.tmpExp_n[1 + i*6] = tmp_1;
     AltBn128::FrElement tmp_2 = params.cm1_n[1 + i*2];
     params.tmpExp_n[2 + i*6] = tmp_2;
     AltBn128::FrElement tmp_3 = params.const_n[2 + i * 4];
     params.tmpExp_n[3 + i*6] = tmp_3;
}
