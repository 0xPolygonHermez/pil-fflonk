#ifndef STEPS_HPP
#define STEPS_HPP

#include <alt_bn128.hpp>
#include "constant_pols_fflonk.hpp"

struct StepsParams
{
    AltBn128::FrElement *pols;
    ConstantPolsFflonk *pConstPols;
    ConstantPolsFflonk *pConstPols2ns;
    AltBn128::FrElement* challenges;
    AltBn128::FrElement* x_n;
    AltBn128::FrElement* x_2ns;
    AltBn128::FrElement* publicInputs;
    AltBn128::FrElement* q_2ns;
};

class Steps
{
public:

    virtual AltBn128::FrElement publics_first(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub) = 0;
    virtual AltBn128::FrElement publics_i(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub) = 0;
    virtual AltBn128::FrElement publics_last(AltBn128::Engine &E, StepsParams &params, uint64_t i, uint64_t pub) = 0;

    virtual void step2prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step2prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step2prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;

    virtual void step3prev_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step3prev_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step3prev_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;

    virtual void step3_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step3_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step3_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;

    virtual void step42ns_first(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step42ns_i(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
    virtual void step42ns_last(AltBn128::Engine &E, StepsParams &params, uint64_t i) = 0;
};

#endif // STEPS
