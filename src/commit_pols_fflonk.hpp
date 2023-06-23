#ifndef COMMIT_POLS_FFLONK_HPP
#define COMMIT_POLS_FFLONK_HPP

#include <cstdint>
#include <alt_bn128.hpp>
#include "zkassert.hpp"

class CommitPolFFlonk
{
private:
    AltBn128::FrElement *_pAddress;
    uint64_t _degree;
    uint64_t _index;
    uint64_t _nCommitedPols;

public:
    CommitPolFFlonk() {}

    CommitPolFFlonk(AltBn128::FrElement *pAddress, uint64_t degree, uint64_t index, uint64_t nCommitedPols) : _pAddress(pAddress), _degree(degree), _index(index), _nCommitedPols(nCommitedPols){};
    AltBn128::FrElement &operator[](uint64_t i) { return _pAddress[i * _nCommitedPols]; };
    AltBn128::FrElement *operator=(AltBn128::FrElement *pAddress)
    {
        _pAddress = pAddress;
        return _pAddress;
    };

    AltBn128::FrElement *address(void) { return _pAddress; }
    uint64_t degree(void) { return _degree; }
    uint64_t index(void) { return _index; }
};

class CompressorCommitPolsFflonk
{
public:
    CommitPolFFlonk* a;

private:
    void *_pAddress;
    uint64_t _degree;
    uint64_t _nCommitedPols;

public:
    CompressorCommitPolsFflonk(void *pAddress, uint64_t degree, uint64_t nCommitedPols) : _pAddress(pAddress), _degree(degree), _nCommitedPols(nCommitedPols){
        a = new CommitPolFFlonk[nCommitedPols];

        for (uint64_t i = 0; i < nCommitedPols; i++) {
            a[i] = CommitPolFFlonk((AltBn128::FrElement *)((uint8_t *)pAddress + sizeof(AltBn128::FrElement)*i), degree, i, nCommitedPols);
        }
    };
    
    uint64_t numPols(void) { return _nCommitedPols; }

    void *address(void) { return _pAddress; }
    uint64_t degree(void) { return _degree; }
    uint64_t size(void) { return _degree * _nCommitedPols * sizeof(AltBn128::FrElement); }
    ~CompressorCommitPolsFflonk() {
        delete[] a;
    };
};

class CommitPolsFflonk
{
public:
    CompressorCommitPolsFflonk Compressor;

private:
    void *_pAddress;
    uint64_t _degree;
    uint64_t _nCommitedPols;

public:
    CommitPolsFflonk(void *pAddress, uint64_t degree, uint64_t nCommitedPols) : Compressor(pAddress, degree, nCommitedPols),
                                                        _pAddress(pAddress), _degree(degree), _nCommitedPols(nCommitedPols) {}

    uint64_t numPols(void) { return _nCommitedPols; }

    void *address(void) { return _pAddress; }
    uint64_t degree(void) { return _degree; }
    uint64_t size(void) { return _degree * _nCommitedPols * sizeof(AltBn128::FrElement); }

    AltBn128::FrElement &getElement(uint64_t pol, uint64_t evaluation)
    {
        zkassert((pol < numPols()) && (evaluation < degree()));
        return ((AltBn128::FrElement *)_pAddress)[pol + evaluation * numPols()];
    }
};

#endif 
