#include <iostream>

extern "C" {
    double test(double);
}

int main() {
    std::cout << "(3 + 3) * (3 + 3) = " << test(3.0) << std::endl;
}

// clang++ testOutput.cpp output.o -o main