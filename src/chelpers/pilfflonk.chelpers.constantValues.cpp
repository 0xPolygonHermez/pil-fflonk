#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

u_int64_t PilFflonkSteps::getNumConstValues() { return 1; }

void PilFflonkSteps::setConstValues(AltBn128::Engine &E, StepsParams &params) {
     params.constValues[0] = E.fr.set(1);
}