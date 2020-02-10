#pragma once

/* AST (Abstract Syntax Tree) nodes describing the constructs of the Kaleidoscope language */

#include <memory>
#include <string>
#include <vector>

namespace kaleidoscope
{
/// ExprAST is the base class for all expression nodes in the AST.
class ExprAST
{
public:
    virtual ~ExprAST()
    {
    }
};

/// NumericExprAST represents numeric literals, like "1.0".
class NumericExprAST : public ExprAST
{
public:
    NumericExprAST( double i_value )
        : m_value( i_value )
    {
    }

private:
    /// Internal storage for numeric value.
    double m_value = 0.0;
};

/// VariableExprAST represents a variable, like "foo".
class VariableExprAST : public ExprAST
{
public:
    VariableExprAST( const std::string& i_name )
        : m_name( i_name )
    {
    }

private:
    std::string m_name = ""; /// Internal storage for variable name.
};

/// BinaryExprAST represents a binary operation.
/// For example: "foo < 5.0"
class BinaryExprAST : public ExprAST
{
public:
    BinaryExprAST( char                       i_operation,
                   std::unique_ptr< ExprAST > i_lhs,
                   std::unique_ptr< ExprAST > i_rhs )
        : m_operation( i_operation )
        , m_lhs( std::move( i_lhs ) )
        , m_rhs( std::move( i_rhs ) )
    {
    }

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
    CallExprAST( const std::string&                        i_callee,
                 std::vector< std::unique_ptr< ExprAST > > i_arguments )
        : m_callee( i_callee )
        , m_arguments( std::move( i_arguments ) )
    {
    }

private:
    std::string                               m_callee;    // Name of the function being called.
    std::vector< std::unique_ptr< ExprAST > > m_arguments; // Arguments passed into the function.
};

/// PrototypeAST represents a function prototype, capturing its name, and names of arguments.
class PrototypeAST
{
public:
    PrototypeAST( const std::string& i_name, const std::vector< std::string >& i_arguments )
        : m_name( i_name )
        , m_arguments( i_arguments )
    {
    }

private:
    std::string                m_name;      /// Name of the function prototype.
    std::vector< std::string > m_arguments; /// Names of the arguments.
};

/// FunctionAST represents a function definition, composed of a prototype (signature)
/// and body.
class FunctionAST
{
public:
    FunctionAST( std::unique_ptr< PrototypeAST > i_prototype, std::unique_ptr< ExprAST > i_body )
        : m_prototype( std::move( i_prototype ) )
        , m_body( std::move( i_body ) )
    {
    }

private:
    std::unique_ptr< PrototypeAST > m_prototype; /// This function's associated prototype.
    std::unique_ptr< ExprAST >      m_body;      /// Function body.
};

} // namespace kaleidoscope
