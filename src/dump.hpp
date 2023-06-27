// #ifndef __DUMP__DUMP_H__
// #define __DUMP__DUMP_H__

// #include <string>
// #include <vector>
// #include <alt_bn128.hpp>

// namespace Dump
// {
//     class Dump
//     {
//     protected:
//         AltBn128::Engine &E;

//     public:
//         Dump(AltBn128::Engine &engine) : E(engine), showValues(false){};

//         void dump(const std::string &label, const u_int8_t *elements, int count);

//         void dump(const std::string &label, u_int32_t *elements, int count);

//         void dump(const std::string &label, u_int32_t element);

//         void dump(const std::string &label, typename AltBn128::FrElement *elements, u_int32_t count);

//         void dump(const std::string &label, std::vector<typename AltBn128::FrElement> &elements, u_int64_t offset = 0);

//         void dump(const std::string &label, const typename AltBn128::FrElement &element);

//         void dump(const std::string &label, std::vector<typename AltBn128::G1PointAffine> &points);

//         void dump(const std::string &label, typename AltBn128::G1PointAffine &point);

//         void dump(const std::string &label, typename AltBn128::G1Point &point);

//         std::string getColorLiteHash(std::string &data);

//         bool showValues;

//     protected:
//         void setShowValues(bool value) { showValues = value; };

//         std::string getLiteHash(std::string &data);

//         std::string getHash(const void *data, u_int32_t len);
//     };
// }

// #endif