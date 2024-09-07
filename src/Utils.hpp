#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "llvm/IR/Value.h"

llvm::Value* LogErrorV(const std::string error);

#endif // UTILS_H