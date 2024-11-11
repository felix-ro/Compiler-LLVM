#include "gtest/gtest.h"
#include "../../include/Parser.hpp"


TEST(ParserTest, Numeric) {
    const std::string input = "def test(hello) 5 + 5 + 10";
    Parser parser(input);
    parser.parse();
}