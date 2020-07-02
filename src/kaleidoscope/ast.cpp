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
    llvm::Function* calleeFunc = io_context.GetFunction( m_callee );
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
                                                       io_context.GetModule() );

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
    PrototypeAST& prototype = *m_prototype;
    io_context.AddFunction( m_prototype );
    llvm::Function* function = io_context.GetFunction( prototype.GetName() );
    if ( function == nullptr )
    {
        return nullptr;
    }

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
        io_context.GetFunctionPassManager()->run( *function );
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

llvm::Value* IfExprAST::GenerateCode( CodeGenContext& io_context )
{
    llvm::Value* ifValue = m_if->GenerateCode( io_context );
    if ( ifValue == nullptr )
    {
        LogError( "Failed to generate code for if condition." );
        return nullptr;
    }

    // Compare if condition != 0.0.
    ifValue = io_context.GetIRBuilder().CreateFCmpONE(
        ifValue,
        llvm::ConstantFP::get( io_context.GetLLVMContext(), llvm::APFloat( 0.0 ) ),
        "ifcond" );

    // Get the function which this condition resides in.
    llvm::Function*   function        = io_context.GetIRBuilder().GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBasicBlock  = llvm::BasicBlock::Create( io_context.GetLLVMContext(), "then", function );
    llvm::BasicBlock* elseBasicBlock  = llvm::BasicBlock::Create( io_context.GetLLVMContext(), "else" );
    llvm::BasicBlock* mergeBasicBlock = llvm::BasicBlock::Create( io_context.GetLLVMContext(), "ifcont" );

    // Create branch into 'then' and 'else'
    io_context.GetIRBuilder().CreateCondBr( ifValue, thenBasicBlock, elseBasicBlock );

    // Generate code for 'then'
    io_context.GetIRBuilder().SetInsertPoint( thenBasicBlock );

    llvm::Value* thenValue = m_then->GenerateCode( io_context );
    if ( thenValue == nullptr )
    {
        LogError( "Failed to generate code for 'then'." );
        return nullptr;
    }

    // Direct 'then' into merge block.
    io_context.GetIRBuilder().CreateBr( mergeBasicBlock );

    // Generating the 'then' code can change the current block.  Fetch the most up to date block for insertion.
    thenBasicBlock = io_context.GetIRBuilder().GetInsertBlock();

    // Add else block to the function.
    function->getBasicBlockList().push_back( elseBasicBlock );
    io_context.GetIRBuilder().SetInsertPoint( elseBasicBlock );

    llvm::Value* elseValue = m_else->GenerateCode( io_context );
    if ( elseValue == nullptr )
    {
        LogError( "Failed to generate code for 'else'." );
        return nullptr;
    }

    // Direct 'else' into merge block.
    io_context.GetIRBuilder().CreateBr( mergeBasicBlock );

    // Generating the 'else' code can change the current block.  Fetch the most up to date block for insertion.
    elseBasicBlock = io_context.GetIRBuilder().GetInsertBlock();

    // Emit merge block.
    function->getBasicBlockList().push_back( mergeBasicBlock );
    io_context.GetIRBuilder().SetInsertPoint( mergeBasicBlock );

    // Create a PHI node which will consume different values depending on which control was executed.
    llvm::PHINode* phiNode =
        io_context.GetIRBuilder().CreatePHI( llvm::Type::getDoubleTy( io_context.GetLLVMContext() ), 2, "iftmp" );

    phiNode->addIncoming( thenValue, thenBasicBlock );
    phiNode->addIncoming( elseValue, elseBasicBlock );

    return phiNode;
}

llvm::Value* ForExprAST::GenerateCode( CodeGenContext& io_context )
{
    // Generate code for start expression.
    llvm::Value* startVariable = m_start->GenerateCode( io_context );
    if ( startVariable == nullptr )
    {
        LogError( "Failed to generate code for loop start." );
        return nullptr;
    }

    llvm::IRBuilder<>& builder     = io_context.GetIRBuilder();
    llvm::LLVMContext& llvmContext = io_context.GetLLVMContext();

    // Get insertion point by querying parent, because this for-loop could be within another condition or block.
    llvm::Function*   function            = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* preHeaderBasicBlock = builder.GetInsertBlock();
    llvm::BasicBlock* loopBasicBlock      = llvm::BasicBlock::Create( io_context.GetLLVMContext(), "loop", function );

    // Direct parent block into loop.
    builder.CreateBr( loopBasicBlock );

    // Start insertion point into loop body.
    builder.SetInsertPoint( loopBasicBlock );

    // PHI node to store start value.
    llvm::PHINode* currentVariable =
        builder.CreatePHI( llvm::Type::getDoubleTy( io_context.GetLLVMContext() ), 2, m_variableName.c_str() );
    currentVariable->addIncoming( startVariable, preHeaderBasicBlock );

    // The start variable may shadow an existing variable, so cache the old variable.
    // Insert a new variable to be available in scope.
    std::map< std::string, llvm::Value* >&                namedValues = io_context.GetNamedValuesInScope();
    std::map< std::string, llvm::Value* >::const_iterator oldValueIt  = namedValues.find( m_variableName );
    namedValues[ m_variableName ] = currentVariable;

    // Emit code for body.
    if ( m_body->GenerateCode( io_context ) == nullptr )
    {
        LogError( "Failed to generate code for loop body." );
        return nullptr;
    }

    // Optional step expression.
    llvm::Value* stepValue = nullptr;
    if ( m_step != nullptr )
    {
        stepValue = m_step->GenerateCode( io_context );
        if ( stepValue == nullptr )
        {
            LogError( "Failed to generate code for loop step expression." );
            return nullptr;
        }
    }
    else
    {
        stepValue = llvm::ConstantFP::get( io_context.GetLLVMContext(), llvm::APFloat( 1.0 ) );
    }

    // Next variable = current variable + step value.
    llvm::Value* nextVariable = builder.CreateFAdd( currentVariable, stepValue, "nextvar" );

    // Compute the end condition.
    llvm::Value* endCondition = m_end->GenerateCode( io_context );
    if ( endCondition == nullptr )
    {
        LogError( "Failed to generate code for loop end expression." );
    }

    // Convert condition to boolean value.
    endCondition =
        builder.CreateFCmpONE( endCondition, llvm::ConstantFP::get( llvmContext, llvm::APFloat( 0.0 ) ), "loopcond" );

    // Create after-loop block
    llvm::BasicBlock* loopEndBasicBlock = builder.GetInsertBlock();
    llvm::BasicBlock* afterBasicBlock   = llvm::BasicBlock::Create( llvmContext, "afterloop", function );

    // Create condition to check
    builder.CreateCondBr( endCondition, loopBasicBlock, afterBasicBlock );

    // Set code insertion point to afterLoop basic block.
    builder.SetInsertPoint( afterBasicBlock );

    // Assign nextVariable to currentVariable.
    currentVariable->addIncoming( nextVariable, loopEndBasicBlock );

    if ( oldValueIt != namedValues.end() )
    {
        namedValues[ m_variableName ] = oldValueIt->second;
    }
    else
    {
        // Does not shadow a previous variable, so erase it from scope.
        namedValues.erase( m_variableName );
    }

    return llvm::Constant::getNullValue( llvm::Type::getDoubleTy( llvmContext ) );
}

} // namespace kaleidoscope
