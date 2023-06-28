#ifndef PILFFLONK_STEPS_HPP
#define PILFFLONK_F_STEPS_HPP

#include "../pilfflonk_prover.hpp"
#include <alt_bn128.hpp>

class PilFflonkSteps : public Steps
{
public:
    void step2prev_first(StepsParams &params, uint64_t i);
    void step2prev_i(StepsParams &params, uint64_t i);
    void step2prev_last(StepsParams &params, uint64_t i);

    void step3prev_first(StepsParams &params, uint64_t i);
    void step3prev_i(StepsParams &params, uint64_t i);
    void step3prev_last(StepsParams &params, uint64_t i);

    void step3_first(StepsParams &params, uint64_t i);
    void step3_i(StepsParams &params, uint64_t i);
    void step3_last(StepsParams &params, uint64_t i);

    void step42ns_first(StepsParams &params, uint64_t i);
    void step42ns_i(StepsParams &params, uint64_t i);
    void step42ns_last(StepsParams &params, uint64_t i);
};

#endif // STARKS_RECURSIVE_1_STEPS_HPP
