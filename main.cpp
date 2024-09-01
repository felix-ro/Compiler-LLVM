#include <string>
#include <iostream>

#include "parser.hpp"


int main(int argc, char **argv) {
    const std::string input = "def test(hello) 5 + 5 \n ";
    Parser parser(input);
    return parser.parse();
}