#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace kaleidoscope
{
/// CodeGenContext is a structure storing the internal state
/// for the course of a code generation session.
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
    /// Storage of llvm internals.
    llvm::LLVMContext m_context;

    /// Helper object for generating instructions.
    llvm::IRBuilder<> m_irBuilder;

    /// Top-level container for functions and global variables.  Owns all memory for generated IR.
    llvm::Module m_module;

    /// Keeps track of values defined in the scope, mapped to their IR.
    /// Currently, only function parameters are referencable.
    std::map< std::string, llvm::Value* > m_namedValuesInScope;
};

} // namespace kaleidoscope
