#ifndef POLYNOMIAL
#define POLYNOMIAL

#include <math.h> /* log2 */
#include "zklog.hpp"
#include "exit_process.hpp"
#include <alt_bn128.hpp>

class Polinomial
{
    AltBn128::Engine &E;
private:
    AltBn128::FrElement *_pAddress = NULL;
    uint64_t _degree = 0;
    uint64_t _offset = 0;
    bool _allocated = false;
    std::string _name = "";

public:
    Polinomial(AltBn128::Engine &_E) : E(_E)
    {
        _pAddress = NULL;
        _degree = 0;
        _offset = 0;
        _allocated = false;
    }
    Polinomial(AltBn128::Engine &_E, 
               void *pAddress,
               uint64_t degree,
               uint64_t offset = 0,
               std::string name = "") : E(_E),
                                        _pAddress((AltBn128::FrElement *)pAddress),
                                        _degree(degree),
                                        _offset(offset),
                                        _name(name){};
    Polinomial(AltBn128::Engine &_E, 
               uint64_t degree,
               std::string name = "") : E(_E),
                                        _degree(degree),
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
        _offset = 1; //TODO: Is this correct?
        _allocated = true;
    };

    ~Polinomial()
    {
        if (_allocated)
            free(_pAddress);
    };

    void potConstruct(AltBn128::FrElement *pAddress,
                      uint64_t degree,
                      uint64_t offset = 0)
    {
        _pAddress = pAddress;
        _degree = degree;
        _offset = offset;
        _allocated = false;
    }

    inline AltBn128::FrElement *address(void) { return _pAddress; }
    inline uint64_t degree(void) { return _degree; }
    inline uint64_t length(void) { return _degree; }
    inline uint64_t size(void) { return _degree * sizeof(AltBn128::FrElement); }
    inline uint64_t offset(void) { return _offset; }

    AltBn128::FrElement *operator[](uint64_t i) { return &_pAddress[i * _offset]; };

    std::string toString(uint numElements = 0, uint radix = 10)
    {
        uint64_t elements = (numElements != 0) ? numElements : _degree;
        std::string res = (_name != "") ? _name + ":\n" : "";
        for (uint i = 0; i < elements; i++)
        {
            res += E.fr.toString(_pAddress[i * _offset]);
            if (i != elements - 1) res += " ";
        }
        return res;
    }

    static void copy(Polinomial &a, Polinomial &b, uint64_t size = 0)
    {
        assert(a.degree() == b.degree());
        if (b._offset == 1 && a._offset == 1)
        {
            std::memcpy(a.address(), b.address(), b.size());
        }
        else
        {
#pragma omp parallel for
            for (uint64_t i = 0; i < b.degree(); i++)
            {
                std::memcpy(a[i], b[i], sizeof(AltBn128::FrElement));
            }
        }
    };

    static void copyElement(Polinomial &a, uint64_t idx_a, Polinomial &b, uint64_t idx_b)
    {;
        std::memcpy(a[idx_a], b[idx_b], sizeof(AltBn128::FrElement));
    };

    static void copyElement(Polinomial &a, uint64_t idx_a, std::vector<AltBn128::FrElement> b)
    {
        std::memcpy(a[idx_a], &b[0], sizeof(AltBn128::FrElement));
    };

    inline void addElement(Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, Polinomial &in_b, uint64_t idx_b)
    {
        out[idx_out][0] = E.fr.add(in_a[idx_a][0],in_b[idx_b][0]);
    }

    inline void subElement(Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, Polinomial &in_b, uint64_t idx_b)
    {
        out[idx_out][0] = E.fr.sub(in_a[idx_a][0], in_b[idx_b][0]);
    }

    inline void mulElement(Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, AltBn128::FrElement &b)
    {
        Polinomial polB(E, &b, 1);
        mulElement(out, idx_out, in_a, idx_a, polB, 0);
    }

    inline void mulElement(Polinomial &out, uint64_t idx_out, Polinomial &in_a, uint64_t idx_a, Polinomial &in_b, uint64_t idx_b)
    {
         out[idx_out][0] = E.fr.mul(in_a[idx_a][0], in_b[idx_b][0]);
    };

    inline std::vector<AltBn128::FrElement> toVector(uint64_t idx)
    {
        std::vector<AltBn128::FrElement> result;
        result.assign(&_pAddress[idx * _offset], &_pAddress[idx * _offset] + 1);
        return result;
    }


//     inline uint64_t firstValueU64(uint64_t idx)
//     {
//         return Goldilocks::toU64(_pAddress[idx * _offset]);
//     }

    // static void calculateH1H2_opt1(Polinomial &h1, Polinomial &h2, Polinomial &fPol, Polinomial &tPol, uint64_t pNumber, uint64_t *buffer, uint64_t size_keys, uint64_t size_values)
    // {
    //     vector<int> counter(tPol.degree(), 1);  // this 1 is important, space of the original buffer could be used
    //     vector<bool> touched(size_keys, false); // faster use this than initialize buffer, bitmask could be used
    //     uint32_t pos = 0;

    //     // double time1 = omp_get_wtime();
    //     for (uint64_t i = 0; i < tPol.degree(); i++)
    //     {
    //         uint64_t key = tPol.firstValueU64(i);
    //         uint64_t ind = key % size_keys;
    //         if (!touched[ind])
    //         {
    //             buffer[ind] = pos;
    //             uint32_t offset = size_keys + 3 * pos;
    //             buffer[offset] = key;
    //             buffer[offset + 1] = i;
    //             buffer[offset + 2] = 0;
    //             pos += 1;
    //             touched[ind] = true;
    //         }
    //         else
    //         {
    //             uint64_t pos_ = buffer[ind];
    //             bool exit_ = false;
    //             do
    //             {
    //                 uint32_t offset = size_keys + 3 * pos_;
    //                 if (key == buffer[offset])
    //                 {
    //                     buffer[offset + 1] = i;
    //                     exit_ = true;
    //                 }
    //                 else
    //                 {
    //                     if (buffer[offset + 2] != 0)
    //                     {
    //                         pos_ = buffer[offset + 2];
    //                     }
    //                     else
    //                     {
    //                         buffer[offset + 2] = pos;
    //                         // new offset
    //                         offset = size_keys + 3 * pos;
    //                         buffer[offset] = key;
    //                         buffer[offset + 1] = i;
    //                         buffer[offset + 2] = 0;
    //                         pos += 1;
    //                         exit_ = true;
    //                     }
    //                 }
    //             } while (!exit_);
    //         }
    //     }

    //     // double time2 = omp_get_wtime();

    //     for (uint64_t i = 0; i < fPol.degree(); i++)
    //     {
    //         uint64_t indx = 0;
    //         uint64_t key = fPol.firstValueU64(i);
    //         uint64_t ind = key % size_keys;
    //         if (!touched[ind])
    //         {
    //             zklog.error("Polynomial::calculateH1H2() Number not included: w=" + to_string(i) + " plookup_number=" + to_string(pNumber) + "\nPol:" + Goldilocks::toString(fPol[i], 16));
    //             exitProcess();
    //         }
    //         uint64_t pos_ = buffer[ind];
    //         bool exit_ = false;
    //         do
    //         {
    //             uint32_t offset = size_keys + 3 * pos_;
    //             if (key == buffer[offset])
    //             {
    //                 indx = buffer[offset + 1];
    //                 exit_ = true;
    //             }
    //             else
    //             {
    //                 if (buffer[offset + 2] != 0)
    //                 {
    //                     pos_ = buffer[offset + 2];
    //                 }
    //                 else
    //                 {
    //                     zklog.error("Polynomial::calculateH1H2() Number not included: w=" + to_string(i) + " plookup_number=" + to_string(pNumber) + "\nPol:" + Goldilocks::toString(fPol[i], 16));
    //                     exitProcess();
    //                 }
    //             }
    //         } while (!exit_);
    //         ++counter[indx];
    //     }

    //     // double time3 = omp_get_wtime();
    //     uint64_t id = 0;
    //     for (u_int64_t i = 0; i < tPol.degree(); ++i)
    //     {
    //         if (counter[id] == 0)
    //         {
    //             ++id;
    //         }

    //         counter[id] -= 1;
    //         Polinomial::copyElement(h1, i, tPol, id);

    //         if (counter[id] == 0)
    //         {
    //             ++id;
    //         }
    //         counter[id] -= 1;
    //         Polinomial::copyElement(h2, i, tPol, id);
    //     }
    //     // double time4 = omp_get_wtime();
    //     // std::cout << "holu: " << id << " " << pos << " times: " << time2 - time1 << " " << time3 - time2 << " " << time4 - time3 << " " << h2.dim() << std::endl;
    // }

    // void calculateZ(Polinomial &z, Polinomial &num, Polinomial &den)
    // {
    //     uint64_t size = num.degree();

    //     Polinomial denI(E, size);
    //     Polinomial checkVal(E, 1);
    //     AltBn128::FrElement *pZ = z[0];
    //     E.fr.copy((AltBn128::FrElement *)&pZ[0], E.fr.one());

    //     batchInverse(denI, den);
    //     for (uint64_t i = 1; i < size; i++)
    //     {
    //         Polinomial tmp(E, 1);
    //         Polinomial::mulElement(tmp, 0, num, i - 1, denI, i - 1);
    //         Polinomial::mulElement(z, i, z, i - 1, tmp, 0);
    //     }
    //     Polinomial tmp(E, 1);
    //     Polinomial::mulElement(tmp, 0, num, size - 1, denI, size - 1);
    //     Polinomial::mulElement(checkVal, 0, z, size - 1, tmp, 0);

    //     // zkassert(E.fr.isOne((AltBn128::FrElement &)*checkVal[0]));
    // }

    // void batchInverse(Polinomial &res, Polinomial &src)
    // {
    //     uint64_t size = src.degree();
    //     Polinomial tmp(E, size);
    //     Polinomial z(E, 2);

    //     Polinomial::copyElement(tmp, 0, src, 0);

    //     for (uint64_t i = 1; i < size; i++)
    //     {
    //         Polinomial::mulElement(tmp, i, tmp, i - 1, src, i);
    //     }

    //     E.fr.inv((AltBn128::FrElement *)z[0], tmp[size - 1]);

    //     for (uint64_t i = size - 1; i > 0; i--) 
    //     {
    //         Polinomial::mulElement(z, 1, z, 0, src, i);
    //         Polinomial::mulElement(res, i, z, 0, tmp, i - 1);
    //         Polinomial::copyElement(z, 0, z, 1);
    //     }
    //     Polinomial::copyElement(res, 0, z, 0);
    // }
};

#endif