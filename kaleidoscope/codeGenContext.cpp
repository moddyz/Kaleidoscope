#include <kaleidoscope/codeGenContext.h>

namespace kaleidoscope
{
CodeGenContext::CodeGenContext()
    : m_irBuilder( m_context )
    , m_module( "MyModule", m_context )
{
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

} // namespace kaleidoscope
