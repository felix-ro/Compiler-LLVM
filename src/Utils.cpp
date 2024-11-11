#include "../include/Utils.hpp"
#include <iostream>

llvm::Value * LogErrorV(const std::string error) {
    std::cout << "Error: " << error << std::endl;
    return nullptr;
}