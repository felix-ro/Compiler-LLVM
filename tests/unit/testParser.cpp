#include "gtest/gtest.h"

#include "../../include/Parser.hpp"
#include "../../include/Utils.hpp"
#include "../../include/Constants.hpp"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"

TEST(ParserTest, Numeric) {
    const std::string input = "def test(hello) 5 + 5 + 10";
    Parser parser(input);
    parser.parse();

    std::string errorStr;
    llvm::ExecutionEngine *executionEngine = 
        llvm::EngineBuilder(std::move(parser.GetIRConstructor()->module))
            .setErrorStr(&errorStr)
            .setOptLevel(llvm::CodeGenOptLevel::None)
            .create();

    if (!executionEngine) {
        llvm::errs() << "Failed to construct ExecutionEngine: " << errorStr << "\n";
        GTEST_ASSERT_TRUE(false); 
    }

    llvm::Function *func = executionEngine->FindFunctionNamed("test");
    if (!func) {
        llvm::errs() << "Function not found in module!\n";
        GTEST_ASSERT_TRUE(false); 
    }

    std::vector<llvm::GenericValue> noArgs;
    llvm::GenericValue res = executionEngine->runFunction(func, noArgs);
    
    GTEST_ASSERT_EQ(20.0f, res.DoubleVal);
}
