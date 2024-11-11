#include "../include/Utils.hpp"
#include <iostream>

// Object File Compilation
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/LegacyPassManager.h"

// JIT
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"

llvm::Value * LogErrorV(const std::string error) {
    std::cout << "Error: " << error << std::endl;
    return nullptr;
}

int CompileToObjectFile(std::shared_ptr<IRConstructor> irConst, std::string fileName) {
    // Initialize the target registry etc.
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!target) {
        llvm::errs() << error;
        return 1;
    }

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto targetMachine = target->createTargetMachine(
        targetTriple, cpu, features, opt, llvm::Reloc::PIC_);

    irConst->getModule().setDataLayout(targetMachine->createDataLayout());

    std::error_code erroCode;
    llvm::raw_fd_ostream dest(fileName, erroCode, llvm::sys::fs::OF_None);
    
    if (erroCode) {
        llvm::errs() << "Could not open file: " << erroCode.message();
        return 1;
    }

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(irConst->getModule());
    dest.flush();

    llvm::outs() << "Wrote " << fileName << "\n";
    return 0;
} 

llvm::GenericValue RunParsedFunction(std::shared_ptr<IRConstructor> irConst, const std::string functionName) {

    std::string errorStr;
    llvm::ExecutionEngine *executionEngine = 
        llvm::EngineBuilder(std::move(irConst->module))
            .setErrorStr(&errorStr)
            .setOptLevel(llvm::CodeGenOptLevel::None)
            .create();

    if (!executionEngine) {
        llvm::errs() << "Failed to construct ExecutionEngine: " << errorStr << "\n";
    }

    llvm::Function *func = executionEngine->FindFunctionNamed("test");
    if (!func) {
        llvm::errs() << "Function not found in module!\n";
    }

    std::vector<llvm::GenericValue> noArgs;
    return executionEngine->runFunction(func, noArgs);
}