#include <string>
#include <iostream>

#include "../include/Parser.hpp"
#include "../include/Utils.hpp"


int main(int argc, char **argv) {
    const std::string input = "def test(x) (1+2+x)*(x+(1+2))";
    Parser parser(input);
    parser.parse();
    CompileToObjectFile(parser.GetIRConstructor(), "build/output.o");
}