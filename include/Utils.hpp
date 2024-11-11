#ifndef UTILS_H
#define UTILS_H

#include "../include/IRConstructor.hpp"

#include <string>

#include "llvm/IR/Value.h"
#include "llvm/ExecutionEngine/GenericValue.h"

llvm::Value* LogErrorV(const std::string error);

int CompileToObjectFile(std::shared_ptr<IRConstructor> irConst, std::string fileName);

llvm::GenericValue RunParsedFunction(std::shared_ptr<IRConstructor> irConst, const std::string functionName);
#endif // UTILS_H