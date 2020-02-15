#pragma once

/* Tools for parsing the kaleidoscope language into an AST (abstract syntax tree) */

#include <kaleidoscope/ast.h>

namespace kaleidoscope
{
/// Parser will parse a block of text into an abstract syntax tree.
class Parser
{
public:
    explicit Parser( const std::string& i_text );

    /// Get the current token.
    /// \return current token.
    int ParseCurrentToken();

    /// Parse the next token into the current buffer.
    /// \return previously stored token.
    int ParseNextToken();

    /// Parse an 'extern' function declaration, with no body (basically a prototype).
    /// \returns function prototype expression.
    std::unique_ptr< PrototypeAST > ParseExternExpr();

    /// Parses a function definition.
    /// \returns parsed function definition.
    std::unique_ptr< FunctionAST > ParseDefinitionExpr();

    /// Parse a top-level expression which is evaluated on the fly.
    /// \returns function exprssion.
    std::unique_ptr< FunctionAST > ParseTopLevelExpr();

private:
    /// No default constructor.
    Parser();

    /// Internal parsing utilities.
    std::unique_ptr< ExprAST > ParseExpr();
    std::unique_ptr< ExprAST > ParseNumericExpr();
    std::unique_ptr< ExprAST > ParseParenthesisExpr();
    std::unique_ptr< ExprAST > ParseIdentifierExpr();
    std::unique_ptr< ExprAST > ParsePrimaryExpr();
    std::unique_ptr< ExprAST > ParseBinaryOperatorRHS( int i_precendence, std::unique_ptr< ExprAST > io_lhs );
    std::unique_ptr< PrototypeAST > ParsePrototypeExpr();
    int ParseCurrentTokenPrecendence();

    Lexer m_lexer;            /// Internal lexer for token translation.
    int   m_currentToken = 0; /// The current token.
};

} // namespace kaleidoscope
