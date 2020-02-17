#pragma once

/* Tools for parsing the kaleidoscope language into an AST (abstract syntax tree) */

#include <kaleidoscope/ast.h>

namespace kaleidoscope
{
/// Parser will parse a block of text into an abstract syntax tree.
class Parser
{
public:
    KALEIDOSCOPE_API
    explicit Parser( const std::string& i_text );

    /// Get the current token.
    /// \return current token.
    KALEIDOSCOPE_API
    int ParseCurrentToken();

    /// Parse the next token into the current buffer.
    /// \return previously stored token.
    KALEIDOSCOPE_API
    int ParseNextToken();

    /// Parse an 'extern' function declaration, with no body (basically a prototype).
    /// \returns function prototype expression.
    KALEIDOSCOPE_API
    std::unique_ptr< PrototypeAST > ParseExternExpr();

    /// Parses a function definition.
    /// \returns parsed function definition.
    KALEIDOSCOPE_API
    std::unique_ptr< FunctionAST > ParseDefinitionExpr();

    /// Parse a top-level expression which is evaluated on the fly.
    /// \returns function exprssion.
    KALEIDOSCOPE_API
    std::unique_ptr< FunctionAST > ParseTopLevelExpr();

private:
    /// No default constructor.
    Parser();

    /// Internal parsing utilities.
    std::unique_ptr< ExprAST > parseExpr();
    std::unique_ptr< ExprAST > parseNumericExpr();
    std::unique_ptr< ExprAST > parseParenthesisExpr();
    std::unique_ptr< ExprAST > parseIdentifierExpr();
    std::unique_ptr< ExprAST > parsePrimaryExpr();
    std::unique_ptr< ExprAST > parseBinaryOperatorRHS( int i_precendence, std::unique_ptr< ExprAST > io_lhs );
    std::unique_ptr< PrototypeAST > parsePrototypeExpr();
    int parseCurrentTokenPrecendence();

    Lexer m_lexer;            /// Internal lexer for token translation.
    int   m_currentToken = 0; /// The current token.
};

} // namespace kaleidoscope
