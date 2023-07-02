#include "../constant_pols_fflonk.hpp"
#include "pilfflonk_steps.hpp"

void PilFflonkSteps::step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.pols[16 + i*2], params.pols[16 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.sub(tmp_0, params.pols[17 + i*2]);
     AltBn128::FrElement tmp_2 = tmp_1;
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_2);
}

void PilFflonkSteps::step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.pols[16 + i*2], params.pols[16 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.sub(tmp_0, params.pols[17 + i*2]);
     AltBn128::FrElement tmp_2 = tmp_1;
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_2);
}

void PilFflonkSteps::step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) {
     AltBn128::FrElement tmp_0 = E.fr.mul(params.pols[16 + i*2], params.pols[16 + i*2]);
     AltBn128::FrElement tmp_1 = E.fr.sub(tmp_0, params.pols[17 + i*2]);
     AltBn128::FrElement tmp_2 = tmp_1;
     E.fr.copy((AltBn128::FrElement &)(params.q_2ns[i]), tmp_2);
}
