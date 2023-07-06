#include <alt_bn128.hpp>
#include "pilfflonk_steps.hpp"

AltBn128::FrElement* PilFflonkSteps::setConstValues(AltBn128::Engine &E) {
     AltBn128::FrElement * constValues = new AltBn128::FrElement[2];
     AltBn128::FrElement v0;
     E.fr.fromString(v0, "5266228460530200451425464971825753823072228272503274930591399474110020095489");
     constValues[0] = v0;
     AltBn128::FrElement v1;
     E.fr.fromString(v1, "7105417016332174850410570896152637638969286250121046049601916147253836355616");
     constValues[1] = v1;
     return constValues;
}