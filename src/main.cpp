#include <string>
#include <iostream>

#include "Parser.hpp"


int main(int argc, char **argv) {
    const std::string input = "def test(x) (1+2+x)*(x+(1+2))";
    Parser parser(input);
    return parser.parse();
}