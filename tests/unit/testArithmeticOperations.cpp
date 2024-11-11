#include "gtest/gtest.h"

#include "../../include/Parser.hpp"
#include "../../include/Utils.hpp"


bool ArithmeticEval(std::string function, double expected) {
    Parser parser(function);
    parser.parse();
    llvm::GenericValue res = RunParsedFunction(parser.GetIRConstructor(), "test");
    return res.DoubleVal == expected;
}

TEST(ArithmeticTests, Addition) {
    GTEST_ASSERT_TRUE(ArithmeticEval("def test(hello) 5 + 5 + 10", 20.0f));
    GTEST_ASSERT_TRUE(ArithmeticEval("def test(hello) 0 + 0", 0.0f));
    // GTEST_ASSERT_TRUE(ArithmeticEval("def test(hello) 5 + (-5)", 0.0f));
}

TEST(ArithmeticTests, Subtraction) {
    GTEST_ASSERT_TRUE(ArithmeticEval("def test(hello) 5 - 5 - 10", -10.0f));
}

TEST(ArithmeticTests, Multiplication) {
    GTEST_ASSERT_TRUE(ArithmeticEval("def test(hello) 5 * 5 * 10", 250.0f));
    GTEST_ASSERT_TRUE(ArithmeticEval("def test(hello) 0 * 5 * 10", 0.0f));
}
