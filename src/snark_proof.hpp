#ifndef SNARK_PROOF_HPP
#define SNARK_PROOF_HPP

#include <map>
#include <string>
#include <iostream>
#include <string.h>
#include <alt_bn128.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

class SnarkProof {
    using FrElement = typename AltBn128::FrElement;
    using G1Point = typename AltBn128::G1Point;
    using G1PointAffine = typename AltBn128::G1PointAffine;

    AltBn128::Engine &E;
    std::string protocol;
    std::string curve;

    std::map <std::string, G1Point> polynomialCommitments;
    std::map <std::string, FrElement> evaluationCommitments;
public :
    SnarkProof(AltBn128::Engine &E, const std::string &protocol);

    void reset();

    void addPolynomialCommitment(const std::string &key, G1Point &polynomialCommitment);

    G1Point getPolynomialCommitment(const std::string &key);

    void addEvaluationCommitment(const std::string &key, FrElement evaluationCommitment);

    FrElement getEvaluationCommitment(const std::string &key);

    json toJson();
};

#endif