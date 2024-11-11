#ifndef UTILS_H
#define UTILS_H

#include "../include/IRConstructor.hpp"

#include <string>
#include "llvm/IR/Value.h"

llvm::Value* LogErrorV(const std::string error);

int CompileToObjectFile(std::shared_ptr<IRConstructor> irConst, std::string fileName);
#endif // UTILS_H