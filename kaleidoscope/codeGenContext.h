#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>

namespace llvm
{
namespace orc
{
class KaleidoscopeJIT;
}
} // namespace llvm

namespace kaleidoscope
{
/// CodeGenContext is a structure storing the internal state
/// of the generated IR code, and various LLVM objects which
/// contribute to code-generation.
///
/// This class has ownership of the following objects:
/// - LLVM context
/// - a module.
/// - an IR builder.
/// - a function pass manager.
///
/// The module and function pass manager are not initialized upon CodeGenContext construction,
/// Use InitializeModule() in situations which only demand IR code generation.
/// Or, use InitializeModuleWithJIT() in situations which require both IR code generation and JIT execution.
///
/// MoveModule() facilitates ownership transfer of its module over to the JIT engine.
class CodeGenContext
{
public:
    CodeGenContext();

    /// Print out the generated IR code described in the module thus far.
    void Print();

    /// Get the LLVM context.
    llvm::LLVMContext& GetLLVMContext();

    /// Get the LLVM IR Builder.
    llvm::IRBuilder<>& GetIRBuilder();

    /// Initialize the module.
    void InitializeModule();

    /// Initialize the module with data layout based on JIT.
    void InitializeModuleWithJIT( llvm::orc::KaleidoscopeJIT& io_jit );

    /// Get the LLVM module as a raw ptr.
    llvm::Module* GetModule();

    /// Move the LLVM module as a unique ptr, transfering ownership to an external entity.
    std::unique_ptr< llvm::Module > MoveModule();

    /// Get all the named values in scope.
    std::map< std::string, llvm::Value* >& GetNamedValuesInScope();

    /// Get the function pass manager.
    llvm::legacy::FunctionPassManager* GetFunctionPassManager();

private:
    /// Used internally for setting up optimization passes.
    void InitializePassManager();

    llvm::LLVMContext m_context;   /// Storage of LLVM internals.
    llvm::IRBuilder<> m_irBuilder; /// Helper object for generating instructions.

    /// Manager all the optimization passes.
    std::unique_ptr< llvm::legacy::FunctionPassManager > m_passManager = nullptr;

    /// Top-level container for functions and global variables.
    std::unique_ptr< llvm::Module > m_module = nullptr;

    /// Keeps track of values defined in the scope, mapped to their IR.
    /// Currently, only function parameters are referencable.
    std::map< std::string, llvm::Value* > m_namedValuesInScope;
};

} // namespace kaleidoscope
