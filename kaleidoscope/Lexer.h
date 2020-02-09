#pragma once

#include <string>

namespace kaleidoscope
{
/// Token describes the type of token.
enum Token
{
    // End of file.
    Token_Eof = -1,

    // Commands.
    Token_Def    = -2,
    Token_Extern = -3,

    // Primary.
    Token_Identifier = -4,
    Token_Number     = -5
};

/// Return the next Token from standard input.
static int LexerNextToken();

/// Get the value of the last parsed Identifier token.
static const std::string& LexerGetIdentifierValue();

/// Get the value of the last parsed Number token.
static double LexerGetNumberValue();

} // namespace kaleidoscope
