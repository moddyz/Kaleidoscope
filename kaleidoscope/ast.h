#pragma once

/* AST (Abstract Syntax Tree) nodes describing the constructs of the Kaleidoscope language */

#include <kaleidoscope/api.h>

#include <memory>
#include <string>
#include <vector>

/// Forward declarations for LLVM types.
namespace llvm
{
class Value;
class Function;
} // namespace llvm

namespace kaleidoscope
{
class CodeGenContext;

/// ExprAST is the base class for all expression nodes in the AST.
class ExprAST
{
public:
    KALEIDOSCOPE_API
    virtual ~ExprAST()
    {
    }

    /// Abstract method to generate code.
    KALEIDOSCOPE_API
    virtual llvm::Value* GenerateCode( CodeGenContext& io_context ) = 0;
};

/// NumericExprAST represents numeric literals, like "1.0".
class NumericExprAST : public ExprAST
{
public:
    KALEIDOSCOPE_API
    NumericExprAST( double i_value )
        : m_value( i_value )
    {
    }

    KALEIDOSCOPE_API
    virtual llvm::Value* GenerateCode( CodeGenContext& io_context ) override;

private:
    /// Internal storage for numeric value.
    double m_value = 0.0;
};

/// VariableExprAST represents a variable, like "foo".
class VariableExprAST : public ExprAST
{
public:
    KALEIDOSCOPE_API
    VariableExprAST( const std::string& i_name )
        : m_name( i_name )
    {
    }

    KALEIDOSCOPE_API
    virtual llvm::Value* GenerateCode( CodeGenContext& io_context ) override;

private:
    std::string m_name = ""; /// Internal storage for variable name.
};

/// BinaryExprAST represents a binary operation.
/// For example: "foo < 5.0"
class BinaryExprAST : public ExprAST
{
public:
    KALEIDOSCOPE_API
    BinaryExprAST( char i_operation, std::unique_ptr< ExprAST > i_lhs, std::unique_ptr< ExprAST > i_rhs )
        : m_operation( i_operation )
        , m_lhs( std::move( i_lhs ) )
        , m_rhs( std::move( i_rhs ) )
    {
    }

    KALEIDOSCOPE_API
    virtual llvm::Value* GenerateCode( CodeGenContext& io_context ) override;

private:
    char                       m_operation = ' ';     /// Type of operation.
    std::unique_ptr< ExprAST > m_lhs       = nullptr; /// Left hand side operand.
    std::unique_ptr< ExprAST > m_rhs       = nullptr; /// Right hand side operand.
};

/// CallExprAST represents a function call.
/// For example: "foo( 1.0 )"
class CallExprAST : public ExprAST
{
public:
    KALEIDOSCOPE_API
    CallExprAST( const std::string& i_callee, std::vector< std::unique_ptr< ExprAST > > i_arguments )
        : m_callee( i_callee )
        , m_arguments( std::move( i_arguments ) )
    {
    }

    KALEIDOSCOPE_API
    virtual llvm::Value* GenerateCode( CodeGenContext& io_context ) override;

private:
    std::string                               m_callee;    // Name of the function being called.
    std::vector< std::unique_ptr< ExprAST > > m_arguments; // Arguments passed into the function.
};

/// PrototypeAST represents a function prototype, capturing its name, and names of arguments.
class PrototypeAST
{
public:
    KALEIDOSCOPE_API
    PrototypeAST( const std::string& i_name, const std::vector< std::string >& i_arguments )
        : m_name( i_name )
        , m_arguments( i_arguments )
    {
    }

    /// Returns the function name.
    KALEIDOSCOPE_API
    const std::string& GetName() const;

    /// Generate code for a function.
    KALEIDOSCOPE_API
    llvm::Function* GenerateCode( CodeGenContext& io_context );

private:
    std::string                m_name;      /// Name of the function prototype.
    std::vector< std::string > m_arguments; /// Names of the arguments.
};

/// FunctionAST represents a function definition, composed of a prototype (signature)
/// and body.
class FunctionAST
{
public:
    KALEIDOSCOPE_API
    FunctionAST( std::unique_ptr< PrototypeAST > i_prototype, std::unique_ptr< ExprAST > i_body )
        : m_prototype( std::move( i_prototype ) )
        , m_body( std::move( i_body ) )
    {
    }

    /// Generate code for a function.
    KALEIDOSCOPE_API
    llvm::Function* GenerateCode( CodeGenContext& io_context );

private:
    std::unique_ptr< PrototypeAST > m_prototype; /// This function's associated prototype.
    std::unique_ptr< ExprAST >      m_body;      /// Function body.
};

/// IfExprAST represents a conditional expression.
class IfExprAST : public ExprAST
{
public:
    KALEIDOSCOPE_API
    IfExprAST( std::unique_ptr< ExprAST > i_if, std::unique_ptr< ExprAST > i_then, std::unique_ptr< ExprAST > i_else )
        : m_if( std::move( i_if ) )
        , m_then( std::move( i_then ) )
        , m_else( std::move( i_else ) )
    {
    }

    /// Generate code for a conditional expression.
    KALEIDOSCOPE_API
    llvm::Value* GenerateCode( CodeGenContext& io_context ) override;

private:
    std::unique_ptr< ExprAST > m_if;   /// Conditional statement
    std::unique_ptr< ExprAST > m_then; /// Expression if condition == true.
    std::unique_ptr< ExprAST > m_else; /// Expression if condition == false.
};

/// ForExprAST represents a for loop expression.
class ForExprAST : public ExprAST
{
public:
    KALEIDOSCOPE_API
    ForExprAST( const std::string&         i_variableName,
                std::unique_ptr< ExprAST > i_start,
                std::unique_ptr< ExprAST > i_end,
                std::unique_ptr< ExprAST > i_step,
                std::unique_ptr< ExprAST > i_body )
        : m_variableName( i_variableName )
        , m_start( std::move( i_start ) )
        , m_end( std::move( i_end ) )
        , m_step( std::move( i_step ) )
        , m_body( std::move( i_body ) )
    {
    }

    /// Generate code for a conditional expression.
    KALEIDOSCOPE_API
    llvm::Value* GenerateCode( CodeGenContext& io_context ) override;

private:
    std::string                m_variableName; /// Loop variable name.
    std::unique_ptr< ExprAST > m_start;        /// Initial value expression.
    std::unique_ptr< ExprAST > m_end;          /// Expression to check for loop termination.
    std::unique_ptr< ExprAST > m_step;         /// Increment expression after each iteration of the loop.
    std::unique_ptr< ExprAST > m_body;         /// Expression to evaluate for for each iteration of the loop.
};

} // namespace kaleidoscope
