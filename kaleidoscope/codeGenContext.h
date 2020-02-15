#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>

namespace kaleidoscope
{
/// CodeGenContext is a structure storing the internal state
/// of the generated IR code, and various LLVM objects which
/// contribute to code-generation.
class CodeGenContext
{
public:
    CodeGenContext();

    /// Print out the generated IR code described in the module thus far.
    void Print();

    /// Get the llvm context.
    llvm::LLVMContext& GetLLVMContext();

    /// Get the llvm module.
    llvm::Module& GetLLVMModule();

    /// Get the llvm IR Builder.
    llvm::IRBuilder<>& GetIRBuilder();

    /// Get all the named values in scope.
    std::map< std::string, llvm::Value* >& GetNamedValuesInScope();

private:
    llvm::LLVMContext                 m_context;     /// Storage of LLVM internals.
    llvm::IRBuilder<>                 m_irBuilder;   /// Helper object for generating instructions.
    llvm::Module                      m_module;      /// Top-level container for functions and global variables.
    llvm::legacy::FunctionPassManager m_passManager; /// Manager all the optimization passes.

    /// Keeps track of values defined in the scope, mapped to their IR.
    /// Currently, only function parameters are referencable.
    std::map< std::string, llvm::Value* > m_namedValuesInScope;
};

} // namespace kaleidoscope
