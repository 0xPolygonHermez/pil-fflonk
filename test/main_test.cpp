#include <gtest/gtest.h>

#include "ntt_altbn128.test.hpp"
#include "polinomial.test.hpp" 

// // Test fixture for common setup and teardown
// class MyTestFixture : public ::testing::Test {
// protected:
//     void SetUp() override {
//         // Common setup code for all tests
//     }

//     void TearDown() override {
//         // Common teardown code for all tests
//     }
// }; 

AltBn128::Engine E;
 
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Check the command-line arguments to execute desired test(s) 
    bool bn128 = false;
    bool polinomial = false;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--bn128") {  
            bn128 = true;
        } else if (std::string(argv[i]) == "--polinomial") {  
            polinomial = true;
        }
    } 

    if (!bn128 && !polinomial) {
        return RUN_ALL_TESTS();
    }
  
    int result = 0;
 
    if (bn128) {
        result |= RUN_ALL_TESTS(); // Run all tests and aggregate the results
        result &= ~1; // Clear the lowest bit to ignore test2's result
    }

    if (polinomial) {
        result |= RUN_ALL_TESTS();
        result &= ~2; // Clear the second lowest bit to ignore test1's result
    }

    return result;
}
