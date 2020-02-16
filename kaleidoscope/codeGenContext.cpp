#include <kaleidoscope/codeGenContext.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

namespace kaleidoscope
{
CodeGenContext::CodeGenContext()
    : m_irBuilder( m_context )
    , m_module( "MyModule", m_context )
    , m_passManager( &m_module )
{
    m_passManager.add( llvm::createInstructionCombiningPass() );
    m_passManager.add( llvm::createReassociatePass() );
    m_passManager.add( llvm::createGVNPass() );
    m_passManager.add( llvm::createCFGSimplificationPass() );
    m_passManager.doInitialization();
}

void CodeGenContext::Print()
{
    m_module.print( llvm::errs(), nullptr );
}

llvm::LLVMContext& CodeGenContext::GetLLVMContext()
{
    return m_context;
}

llvm::Module& CodeGenContext::GetLLVMModule()
{
    return m_module;
}

llvm::IRBuilder<>& CodeGenContext::GetIRBuilder()
{
    return m_irBuilder;
}

std::map< std::string, llvm::Value* >& CodeGenContext::GetNamedValuesInScope()
{
    return m_namedValuesInScope;
}

llvm::legacy::FunctionPassManager& CodeGenContext::GetFunctionPassManager()
{
    return m_passManager;
}

} // namespace kaleidoscope
