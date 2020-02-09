#pragma once

/* Tools for parsing the kaleidoscope language into an AST (abstract syntax tree) */

#include <kaleidoscope/AST.h>

namespace kaleidoscope
{
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

} // namespace kaleidoscope
