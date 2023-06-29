#ifndef COMPARE_FE_HPP
#define COMPARE_FE_HPP

#include <alt_bn128.hpp>

struct CompareFe
{
    AltBn128::Engine& E;
    
    CompareFe(AltBn128::Engine& _E) : E(_E) {}
    
    bool operator()(const AltBn128::FrElement& a, const AltBn128::FrElement& b) const
    {
        cout << E.fr.toString(a) << " " << E.fr.toString(b) << " " << E.fr.lt(a,b) << endl;
        return E.fr.lt(a, b);
    }
    
};

#endif

