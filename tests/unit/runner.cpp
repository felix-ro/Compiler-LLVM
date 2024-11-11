#include "gtest/gtest.h"
#include "llvm/Support/TargetSelect.h"

#include <iostream>

#include "../../include/Constants.hpp"

int main(int argc, char ** argv) {
    // This should be run during the setup of the test environment
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    testing::InitGoogleTest();
    RUN_ALL_TESTS();
}