// #include "dump.hpp"
// #include "keccak_wrapper.hpp"

// #include <iostream>
// #include <iomanip>

// namespace Dump
// {
//     void Dump::dump(const std::string &label, u_int32_t element)
//     {
//         std::stringstream ss;
//         ss << element;

//         std::string lchash = getColorLiteHash(ss.str());

//         printf("## %s %s %s\n", label.c_str(), lchash.c_str(), ss.str().c_str());
//     }

//     void Dump::dump(const std::string &label, const u_int8_t *elements, int count)
//     {
//         static const char *dec2hex = "0123456789ABCDEF";
//         std::stringstream ss;
//         for (int i = 0; i < count; ++i)
//         {
//             ss << dec2hex[(elements[i] >> 4) & 0x0F] << dec2hex[elements[i] & 0x0F];
//         }
//         std::string values = ss.str();
//         std::string lchash = getColorLiteHash(values);

//         printf("## %s [%u] %s %s\n", label.c_str(), count, lchash.c_str(), showValues ? values.c_str() : "");
//     }

//     void Dump::dump(const std::string &label, u_int32_t *elements, int count)
//     {
//         std::stringstream ss;
//         for (int i = 0; i < count; ++i)
//         {
//             if (i)
//                 ss << " ";
//             ss << elements[i];
//         }
//         std::string values = ss.str();
//         std::string lchash = getColorLiteHash(values);

//         printf("## %s [%u] %s %s\n", label.c_str(), count, lchash.c_str(), showValues ? values.c_str() : "");
//     }

//     void Dump::dump(const std::string &label, typename AltBn128::FrElement *elements, u_int32_t count)
//     {
//         std::stringstream ss;
//         for (uint32_t i = 0; i < count; ++i)
//         {
//             if (i)
//                 ss << " ";
//             ss << E.fr.toString(elements[i]);
//         }
//         std::string values = ss.str();
//         std::string lchash = getColorLiteHash(values);

//         printf("## %s [%u] %s %s\n", label.c_str(), count, lchash.c_str(), showValues ? values.c_str() : "");
//     }

//     void Dump::dump(const std::string &label, const typename AltBn128::Engine::FrElement &element)
//     {
//         std::string value = E.fr.toString(element);
//         std::string lchash = getColorLiteHash(value);

//         printf("## %s %s %s\n", label.c_str(), lchash.c_str(), value.c_str());
//     }

//     void Dump::dump(const std::string &label, std::vector<typename AltBn128::Engine::FrElement> &elements, u_int64_t offset)
//     {
//         std::stringstream ss;
//         for (u_int64_t i = offset; i < elements.size(); ++i)
//         {
//             if (i)
//                 ss << " ";
//             ss << E.fr.toString(elements[i]);
//         }
//         std::string values = ss.str();
//         std::string lchash = getColorLiteHash(values);

//         printf("## %s [%lu] %s %s\n", label.c_str(), elements.size() - offset, lchash.c_str(), showValues ? values.c_str() : "");
//     }

//     void Dump::dump(const std::string &label, std::vector<typename AltBn128::Engine::G1PointAffine> &points)
//     {
//         std::stringstream ss;
//         for (uint32_t i = 0; i < points.size(); ++i)
//         {
//             if (i)
//                 ss << " ";
//             ss << "[" << E.f1.toString(points[i].x) << "," << E.f1.toString(points[i].y) << "]";
//         }
//         std::string values = ss.str();
//         std::string lchash = getColorLiteHash(values);

//         printf("## %s [%lu] %s %s\n", label.c_str(), points.size(), lchash.c_str(), showValues ? values.c_str() : "");
//     }

//     void Dump::dump(const std::string &label, typename AltBn128::Engine::G1PointAffine &point)
//     {
//         std::stringstream ss;
//         ss << "(" << E.f1.toString(point.x) << "," << E.f1.toString(point.y) << ")";
//         std::string values = ss.str();
//         std::string lchash = getColorLiteHash(values);

//         printf("## %s %s %s\n", label.c_str(), lchash.c_str(), values.c_str());
//     }

//     void Dump::dump(const std::string &label, typename AltBn128::Engine::G1Point &point)
//     {
//         std::string values = E.g1.toString(point);
//         std::string lchash = getColorLiteHash(values);

//         printf("## %s %s %s\n", label.c_str(), lchash.c_str(), values.c_str());
//     }

//     std::string Dump::getColorLiteHash(std::string &data)
//     {
//         std::string lhash = getLiteHash(data);

//         std::stringstream ss;

//         ss << "\e[1;" << (31 + (std::stoi(lhash.substr(1, 1), nullptr, 16) % 6)) << "m" << lhash.substr(0, 3)
//            << "\e[1;" << (31 + (std::stoi(lhash.substr(3, 1), nullptr, 16) % 6)) << "m" << lhash.substr(3, 2)
//            << "\e[1;" << (31 + (std::stoi(lhash.substr(6, 1), nullptr, 16) % 6)) << "m" << lhash.substr(5, 3)
//            << "\e[0m";
//         return ss.str();
//     }

//     std::string Dump::getLiteHash(std::string &data)
//     {
//         std::string hash = getHash(data.c_str(), data.length());
//         return hash.substr(0, 4) + hash.substr(28, 4);
//     }

//     std::string Dump::getHash(const void *data, u_int32_t len)
//     {
//         unsigned char digest[32];
//         int64_t outLen = keccak((void *)data, len, digest, sizeof(digest));

//         std::stringstream ss;
//         for (int64_t i = 0; i < outLen; i++)
//         {
//             ss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<unsigned>(digest[i]);
//         }
//         return ss.str();
//     }

// }