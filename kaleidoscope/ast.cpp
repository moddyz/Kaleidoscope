#include <kaleidoscope/ast.h>
#include <kaleidoscope/codeGenContext.h>
#include <kaleidoscope/logger.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>

namespace kaleidoscope
{
llvm::Value* NumericExprAST::GenerateCode( CodeGenContext& io_context )
{
    return llvm::ConstantFP::get( io_context.GetLLVMContext(), llvm::APFloat( m_value ) );
}

llvm::Value* VariableExprAST::GenerateCode( CodeGenContext& io_context )
{
    std::map< std::string, llvm::Value* >::const_iterator valueIt = io_context.GetNamedValuesInScope().find( m_name );
    if ( valueIt == io_context.GetNamedValuesInScope().end() )
    {
        LogError( "Unknown variable name: %s", m_name.c_str() );
        return nullptr;
    }

    return valueIt->second;
}

llvm::Value* BinaryExprAST::GenerateCode( CodeGenContext& io_context )
{
    // Extract Values from operands.
    llvm::Value* leftValue  = m_lhs->GenerateCode( io_context );
    llvm::Value* rightValue = m_rhs->GenerateCode( io_context );
    if ( leftValue == nullptr || rightValue == nullptr )
    {
        return nullptr;
    }

    // Generate appropriate instruction per operand.
    switch ( m_operation )
    {
    case '+':
        return io_context.GetIRBuilder().CreateFAdd( leftValue, rightValue, "addtmp" );
    case '-':
        return io_context.GetIRBuilder().CreateFSub( leftValue, rightValue, "subtmp" );
    case '*':
        return io_context.GetIRBuilder().CreateFMul( leftValue, rightValue, "multmp" );
    case '<':
    {
        llvm::Value* integerValue = io_context.GetIRBuilder().CreateFCmpULT( leftValue, rightValue, "cmptmp" );
        return io_context.GetIRBuilder().CreateUIToFP( integerValue,
                                                       llvm::Type::getDoubleTy( io_context.GetLLVMContext() ),
                                                       "booltmp" );
    }
    default:
        LogError( "Invalid binary operation: %s", m_operation );
        return nullptr;
    }
}

llvm::Value* CallExprAST::GenerateCode( CodeGenContext& io_context )
{
    // Look up function name in global module table.
    llvm::Function* calleeFunc = io_context.GetLLVMModule().getFunction( m_callee );
    if ( calleeFunc == nullptr )
    {
        LogError( "Unknown function '%s' referenced.", m_callee.c_str() );
        return nullptr;
    }

    if ( calleeFunc->arg_size() != m_arguments.size() )
    {
        LogError( "LLVM function arg size %lu != expression size %lu.", calleeFunc->arg_size(), m_arguments.size() );
        return nullptr;
    }

    std::vector< llvm::Value* > argumentValues( calleeFunc->arg_size(), nullptr );
    for ( size_t argIndex = 0; argIndex < calleeFunc->arg_size(); ++argIndex )
    {
        llvm::Value* argumentValue = m_arguments[ argIndex ]->GenerateCode( io_context );
        if ( argumentValue == nullptr )
        {
            LogError( "Invalid argument." );
            return nullptr;
        }

        argumentValues[ argIndex ] = argumentValue;
    }

    return io_context.GetIRBuilder().CreateCall( calleeFunc, argumentValues, "calltmp" );
}

const std::string& PrototypeAST::GetName() const
{
    return m_name;
}

llvm::Function* PrototypeAST::GenerateCode( CodeGenContext& io_context )
{
    // Create Function type from our argument types.
    std::vector< llvm::Type* > argumentTypes( m_arguments.size(),
                                              llvm::Type::getDoubleTy( io_context.GetLLVMContext() ) );
    llvm::FunctionType*        functionType =
        llvm::FunctionType::get( llvm::Type::getDoubleTy( io_context.GetLLVMContext() ), argumentTypes, false );

    // Create new IR Function in our modulefrom functionType.
    llvm::Function* function = llvm::Function::Create( functionType,
                                                       llvm::Function::ExternalLinkage,
                                                       llvm::Twine( m_name ),
                                                       &io_context.GetLLVMModule() );

    // Set argument names to make IR more readable.
    size_t argIndex = 0;
    for ( llvm::Argument& arg : function->args() )
    {
        arg.setName( m_arguments[ argIndex ] );
        argIndex += 1;
    }

    return function;
}

llvm::Function* FunctionAST::GenerateCode( CodeGenContext& io_context )
{
    // Check for existing function generated from previous 'extern' declaration.
    llvm::Function* function = io_context.GetLLVMModule().getFunction( m_prototype->GetName() );

    // Function has not been previously declared, create one from prototype.
    if ( function == nullptr )
    {
        function = m_prototype->GenerateCode( io_context );
    }

    // Error handling.
    if ( function == nullptr )
    {
        LogError( "Failed to create function %s", m_prototype->GetName().c_str() );
        return nullptr;
    }

    // Cannot re-define the same function twice.
    if ( !function->empty() )
    {
        LogError( "Function '%s' cannot be redefined", m_prototype->GetName().c_str() );
        return nullptr;
    }

    // Create a basic block.  Analogous to a scope?
    llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create( io_context.GetLLVMContext(), "entry", function );
    io_context.GetIRBuilder().SetInsertPoint( basicBlock );

    // Clear scope variables, and add function arguments.
    io_context.GetNamedValuesInScope().clear();
    for ( llvm::Argument& arg : function->args() )
    {
        io_context.GetNamedValuesInScope()[ arg.getName() ] = &arg;
    }

    // Create a return value for this block.
    // The return value is based off the body's evaluated expression.
    llvm::Value* returnValue = m_body->GenerateCode( io_context );
    if ( returnValue )
    {
        io_context.GetIRBuilder().CreateRet( returnValue );
        llvm::verifyFunction( *function );
        io_context.GetFunctionPassManager().run( *function );
        return function;
    }
    else
    {
        // Failed to generate body expression.  Delete function.
        function->eraseFromParent();
        return nullptr;
    }

    return function;
}

} // namespace kaleidoscope
