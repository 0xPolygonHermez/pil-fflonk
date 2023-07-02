#include <alt_bn128.hpp>
#include "../constant_pols_fflonk.hpp"
#include "pilfflonk_steps.hpp"

AltBn128::FrElement PilFflonkSteps::publics_first(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub) {
  cout << "PUB " << pub << endl;
   if (pub == 0) {
     AltBn128::FrElement tmp_0 = params.pols[1 + i*2]; 
     cout << "HOLA" << " " <<E.fr.toString(params.pols[1 + i*2]) << endl;
     cout << "ADEU" << " "<< E.fr.toString(tmp_0) << endl;
     return tmp_0;
   }
}

AltBn128::FrElement PilFflonkSteps::publics_i(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub) {
   if (pub == 0) {
     AltBn128::FrElement tmp_0 = params.pols[1 + i*2];
     return tmp_0;
   }
}

AltBn128::FrElement PilFflonkSteps::publics_last(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub) {
   if (pub == 0) {
     AltBn128::FrElement tmp_0 = params.pols[1 + i*2];
     return tmp_0;
   }
}
