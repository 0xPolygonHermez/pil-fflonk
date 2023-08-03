#include <gtest/gtest.h>
#include <alt_bn128.hpp>

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
    
    return RUN_ALL_TESTS();
}
