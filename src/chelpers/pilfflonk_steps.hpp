#ifndef PILFFLONK_STEPS_HPP
#define PILFFLONK_STEPS_HPP

#include "../steps.hpp"
#include <alt_bn128.hpp>

class PilFflonkSteps : public Steps
{
public:

    AltBn128::FrElement publics_first(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub);
    AltBn128::FrElement publics_i(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub);
    AltBn128::FrElement publics_last(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub);

    void step2prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step2prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step2prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i);

    void step3prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step3prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step3prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i);

    void step3_first(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step3_i(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step3_last(AltBn128::Engine &E, StepsParams &params, uint64_t i);

    void step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i);
    void step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i);
};

#endif // STARKS_RECURSIVE_1_STEPS_HPP
