#include <kaleidoscope/KaleidoscopeJIT.h>
#include <kaleidoscope/codeGenContext.h>

#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

namespace kaleidoscope
{
CodeGenContext::CodeGenContext()
    : m_irBuilder( m_context )
{
}

void CodeGenContext::Print()
{
    assert( m_module != nullptr );
    m_module->print( llvm::errs(), nullptr );
}

void CodeGenContext::InitializePassManager()
{
    assert( m_module != nullptr );
    m_passManager = std::make_unique< llvm::legacy::FunctionPassManager >( m_module.get() );
    m_passManager->add( llvm::createInstructionCombiningPass() );
    m_passManager->add( llvm::createReassociatePass() );
    m_passManager->add( llvm::createGVNPass() );
    m_passManager->add( llvm::createCFGSimplificationPass() );
    m_passManager->doInitialization();
}

void CodeGenContext::InitializeModule()
{
    assert( m_module == nullptr );
    m_module = std::make_unique< llvm::Module >( "MyModule", m_context );
    InitializePassManager();
}

void CodeGenContext::InitializeModuleWithJIT( llvm::orc::KaleidoscopeJIT& io_jit )
{
    assert( m_module == nullptr );
    m_module = std::make_unique< llvm::Module >( "MyModule", m_context );
    m_module->setDataLayout( io_jit.getTargetMachine().createDataLayout() );
    InitializePassManager();
}

llvm::LLVMContext& CodeGenContext::GetLLVMContext()
{
    return m_context;
}

llvm::IRBuilder<>& CodeGenContext::GetIRBuilder()
{
    return m_irBuilder;
}

llvm::Module* CodeGenContext::GetModule()
{
    assert( m_module != nullptr );
    return m_module.get();
}

std::unique_ptr< llvm::Module > CodeGenContext::MoveModule()
{
    assert( m_module != nullptr );
    return std::move( m_module );
}

std::map< std::string, llvm::Value* >& CodeGenContext::GetNamedValuesInScope()
{
    return m_namedValuesInScope;
}

llvm::legacy::FunctionPassManager* CodeGenContext::GetFunctionPassManager()
{
    assert( m_passManager != nullptr );
    return m_passManager.get();
}

} // namespace kaleidoscope
