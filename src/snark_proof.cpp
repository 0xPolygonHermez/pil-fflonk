#include <sstream>

#include "snark_proof.hpp"

SnarkProof::SnarkProof(AltBn128::Engine &_E, const std::string &protocol) : E(_E) {
    this->protocol = protocol;
    this->curve = "bn128";
    this->reset();
}

void SnarkProof::reset() {
    this->polynomialCommitments.clear();
    this->evaluationCommitments.clear();
}

void SnarkProof::addPolynomialCommitment(const std::string &key, G1Point &polynomialCommmitment) {
    if (0 != polynomialCommitments.count(key)) {
        std::ostringstream ss;
        cerr << "!!! SnarkProof::addPolynomialCommitment. '" << key << "' already exist in proof";
    }
    this->polynomialCommitments[key] = polynomialCommmitment;
}

AltBn128::G1Point SnarkProof::getPolynomialCommitment(const std::string &key) {
    if (0 == polynomialCommitments.count(key)) {
        std::ostringstream ss;
        cerr << "!!! SnarkProof::addPolynomialCommitment. '" << key << "' does not exist in proof";
    }
    return this->polynomialCommitments[key];
}

void SnarkProof::addEvaluationCommitment(const std::string &key, FrElement evaluationCommitment) {
    if (0 != evaluationCommitments.count(key)) {
        std::ostringstream ss;
        cerr << "!!! SnarkProof::addPolynomialCommitment. '" << key << "' already exist in proof";
    }
    this->evaluationCommitments[key] = evaluationCommitment;
}

AltBn128::FrElement SnarkProof::getEvaluationCommitment(const std::string &key) {
    if (0 == evaluationCommitments.count(key)) {
        std::ostringstream ss;
        cerr << "!!! SnarkProof::addPolynomialCommitment. '" << key << "' does not exist in proof";
    }
    return this->evaluationCommitments[key];
}

json SnarkProof::toJson() {
    json jsonProof;

    jsonProof["polynomials"] = {};
    jsonProof["evaluations"] = {};

    for (auto &[key, point]: this->polynomialCommitments) {
        G1PointAffine tmp;
        E.g1.copy(tmp, point);

        jsonProof["polynomials"][key] = {};

        std::string x = E.f1.toString(tmp.x);
        std::string y = E.f1.toString(tmp.y);

        jsonProof["polynomials"][key].push_back(x);
        jsonProof["polynomials"][key].push_back(y);
        jsonProof["polynomials"][key].push_back("1");
    }

    for (auto &[key, element]: this->evaluationCommitments) {
        jsonProof["evaluations"][key] = E.fr.toString(element);
    }

    jsonProof["protocol"] = this->protocol;
    jsonProof["curve"] = this->curve;

    return jsonProof;
}