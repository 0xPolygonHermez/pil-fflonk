#ifndef POLYNOMIAL
#define POLYNOMIAL

#include <math.h> /* log2 */
#include "zklog.hpp"
#include <iostream>
#include "zkassert.hpp"
#include "compare_fe.hpp"
#include "exit_process.hpp"
#include <alt_bn128.hpp>

class Polinomial
{
private:
    AltBn128::FrElement *_pAddress = NULL;
    uint64_t _degree = 0;
    bool _allocated = false;
    std::string _name = "";

public:
    Polinomial()
    {
        _pAddress = NULL;
        _degree = 0;
        _allocated = false;
    }
    Polinomial(void *pAddress,
               uint64_t degree,
               std::string name = "") : _pAddress((AltBn128::FrElement *)pAddress),
                                        _degree(degree),
                                        _name(name){};
    Polinomial(uint64_t degree,
               std::string name = "") : _degree(degree),
                                        _name(name)
    {
        if (degree == 0)
            return;
        _pAddress = (AltBn128::FrElement *)calloc(_degree, sizeof(AltBn128::FrElement));
        if (_pAddress == NULL)
        {
            zklog.error("Polinomial::Polinomial() failed allocating polinomial with size: " + to_string(_degree * sizeof(AltBn128::FrElement)));
            exitProcess();
        }
        _allocated = true;
    };

    ~Polinomial()
    {
        if (_allocated)
            free(_pAddress);
    };

    void potConstruct(AltBn128::FrElement *pAddress,
                      uint64_t degree)
    {
        _pAddress = pAddress;
        _degree = degree;
        _allocated = false;
    }

    inline AltBn128::FrElement *address(void) { return _pAddress; }
    inline uint64_t degree(void) { return _degree; }
    inline uint64_t length(void) { return _degree; }
    inline uint64_t size(void) { return _degree * sizeof(AltBn128::FrElement); }

    AltBn128::FrElement *operator[](uint64_t i) { return &_pAddress[i]; };

    std::string toString(AltBn128::Engine &E, uint numElements = 0, uint radix = 10)
    {
        uint64_t elements = (numElements != 0) ? numElements : _degree;
        std::string res = (_name != "") ? _name + ":\n" : "";
        for (uint i = 0; i < elements; i++)
        {
            res += E.fr.toString(_pAddress[i]);
            if (i != elements - 1) res += " ";
        }
        return res;
    }

    static void copy(Polinomial &a, Polinomial &b, uint64_t size = 0)
    {
        assert(a.degree() == b.degree());
        memcpy(a.address(), b.address(), b.size());
    };

    static void copyElement(Polinomial &a, uint64_t idx_a, Polinomial &b, uint64_t idx_b)
    {;
        memcpy(a[idx_a], b[idx_b], sizeof(AltBn128::FrElement));
    };

    static void copyElement(Polinomial &a, uint64_t idx_a, AltBn128::FrElement b)
    {
        memcpy(a[idx_a], &b, sizeof(AltBn128::FrElement));
    };

    static inline void addElement(AltBn128::Engine &E, Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, Polinomial &in_b, uint64_t idx_b)
    {
        *out[idx_out] = E.fr.add(*in_a[idx_a],*in_b[idx_b]);
    }

    static inline void subElement(AltBn128::Engine &E, Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, Polinomial &in_b, uint64_t idx_b)
    {
        *out[idx_out] = E.fr.sub(*in_a[idx_a], *in_b[idx_b]);
    }

    static inline void mulElement(AltBn128::Engine &E, Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, AltBn128::FrElement &b)
    {
        Polinomial polB(&b, 1);
        mulElement(E, out, idx_out, in_a, idx_a, polB, 0);
    }

    static inline void mulElement(AltBn128::Engine &E, Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, Polinomial &in_b, uint64_t idx_b)
    {
        *out[idx_out] = E.fr.mul(*in_a[idx_a], *in_b[idx_b]);
    };

    // static void calculateH1H2(AltBn128::Engine &E, Polinomial &h1, Polinomial &h2, Polinomial &fPol, Polinomial &tPol)
    // {
    //     map<AltBn128::FrElement, uint64_t, CompareFe> idx_t(E);
    //     multimap<AltBn128::FrElement, uint64_t, CompareFe> s(E);
    //     multimap<AltBn128::FrElement, uint64_t>::iterator it;
    //     uint64_t i = 0; 

    //     for (uint64_t i = 0; i < tPol.degree(); i++)
    //     {
    //         AltBn128::FrElement key = *tPol[i];
    //         std::pair<AltBn128::FrElement, uint64_t> pr(key, i);

    //         auto const result = idx_t.insert(pr);
    //         if (not result.second)
    //         {
    //             result.first->second = i;
    //         }

    //         s.insert(pr);
    //     }

    //     for (uint64_t i = 0; i < fPol.degree(); i++)
    //     {
    //         AltBn128::FrElement key = *fPol[i];

    //         if (idx_t.find(key) == idx_t.end())
    //         {
    //             zklog.error("Polinomial::calculateH1H2() Number not included: " + E.fr.toString(*fPol[i]));
    //             exitProcess();
    //         }
    //         uint64_t idx = idx_t[key];
    //         s.insert(pair<AltBn128::FrElement, uint64_t>(key, idx));
    //     }

    //     multimap<uint64_t, AltBn128::FrElement> s_sorted;
    //     multimap<uint64_t, AltBn128::FrElement>::iterator it_sorted;

    //     cout << s.size() << endl;
    //     for (it = s.begin(); it != s.end(); it++)
    //     {
    //         cout << E.fr.toString(it->first) << " " << it->second << endl;
    //         s_sorted.insert(make_pair(it->second, it->first));
    //     }

    //     for (it_sorted = s_sorted.begin(); it_sorted != s_sorted.end(); it_sorted++, i++)
    //     {
    //         if ((i & 1) == 0)
    //         {
    //             Polinomial::copyElement(h1, i / 2, it_sorted->second);
    //         }
    //         else
    //         {
    //             Polinomial::copyElement(h2, i / 2, it_sorted->second);
    //         }
    //     }
    // };

    static void calculateZ(AltBn128::Engine &E, Polinomial &z, Polinomial &num, Polinomial &den)
    {
        uint64_t size = num.degree();

        Polinomial denI(size);
        Polinomial checkVal(1);
        AltBn128::FrElement *pZ = z[0];
        pZ[0] = E.fr.one();

        Polinomial::batchInverse(E, denI, den);
        for (uint64_t i = 1; i < size; i++)
        {
            Polinomial tmp(1);
            mulElement(E, tmp, 0, num, i - 1, denI, i - 1);
            mulElement(E, z, i, z, i - 1, tmp, 0);
        }
        Polinomial tmp(1);
        mulElement(E, tmp, 0, num, size - 1, denI, size - 1);
        mulElement(E, checkVal, 0, z, size - 1, tmp, 0);

        zkassert(E.fr.eq((AltBn128::FrElement &)*checkVal[0], E.fr.one()));
    }

    inline static void batchInverse(AltBn128::Engine &E, Polinomial &res, Polinomial &src)
    {
        uint64_t size = src.degree();
        Polinomial tmp(size);
        Polinomial z(2);

        Polinomial::copyElement(tmp, 0, src, 0);

        for (uint64_t i = 1; i < size; i++)
        {
            Polinomial::mulElement(E, tmp, i, tmp, i - 1, src, i);
        }

        E.fr.inv(*z[0], *tmp[size - 1]);

        for (uint64_t i = size - 1; i > 0; i--) 
        {
            Polinomial::mulElement(E, z, 1, z, 0, src, i);
            Polinomial::mulElement(E, res, i, z, 0, tmp, i - 1);
            Polinomial::copyElement(z, 0, z, 1);
        }
        Polinomial::copyElement(res, 0, z, 0);
    }
};

#endif