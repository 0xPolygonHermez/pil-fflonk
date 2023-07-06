#ifndef COMPARE_FE_HPP
#define COMPARE_FE_HPP

#include <alt_bn128.hpp>


struct CompareFe
{
    AltBn128::Engine& E;
    
    CompareFe(AltBn128::Engine& _E) : E(_E) {}
    
    bool operator()(const AltBn128::FrElement& a, const AltBn128::FrElement& b) const
    {
        // std::istringstream iss1(E.fr.toString(a));
        // std::istringstream iss2(E.fr.toString(b));
        // long long n1, n2;
        // iss1 >> n1;
        // iss2 >> n2;

        // // Perform the comparison
        // return n1 < n2;

        return E.fr.lt(a, b);
    }
};

#endif

