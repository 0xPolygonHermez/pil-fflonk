#include "../constant_pols_fflonk.hpp"
#include "pilfflonk_steps.hpp"

   
void PilFflonkSteps::step3_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.pols[0 + i*2], params.pols[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.pols[1 + i*2], params.pols[1 + i*2]);
     params.pols[128 + i*1] = E.fr.add(tmp_0, tmp_1);
}

void PilFflonkSteps::step3_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.pols[0 + i*2], params.pols[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.pols[1 + i*2], params.pols[1 + i*2]);
     params.pols[128 + i*1] = E.fr.add(tmp_0, tmp_1);
}

void PilFflonkSteps::step3_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.pols[0 + i*2], params.pols[0 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.mul(params.pols[1 + i*2], params.pols[1 + i*2]);
     params.pols[128 + i*1] = E.fr.add(tmp_0, tmp_1);
}
