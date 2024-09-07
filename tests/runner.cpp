#include "gtest/gtest.h"
#include <iostream>

int main(int argc, char ** argv) {
    std::cout << "Starting Tests" << std::endl;
    testing::InitGoogleTest();
    RUN_ALL_TESTS();
}