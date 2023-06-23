#ifndef CONSTANT_POLS_FFLONK_HPP
#define CONSTANT_POLS_FFLONK_HPP

#include <cstdint>
#include <alt_bn128.hpp>
#include "zkassert.hpp"

class ConstantPolsFflonk
{
private:
    void *_pAddress;
    uint64_t _degree;
    uint64_t _numPols;

public:
    ConstantPolsFflonk(void *pAddress, uint64_t degree, uint64_t numPols) : _pAddress(pAddress),_degree(degree), _numPols(numPols){};

    inline uint64_t numPols(void) { return _numPols; }

    void *address(void) { return _pAddress; }
    uint64_t degree(void) { return _degree; }
    uint64_t size(void) { return _degree * _numPols * sizeof(AltBn128::FrElement); }

    inline AltBn128::FrElement &getElement(uint64_t pol, uint64_t evaluation)
    {
        return ((AltBn128::FrElement *)_pAddress)[pol + evaluation * _numPols];
    }
};

#endif
