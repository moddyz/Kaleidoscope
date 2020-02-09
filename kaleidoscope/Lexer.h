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
    Token_Numeric     = -5
};

/// Return the next Token from standard input.
int GetNextToken();

/// Get the value of the last Identifier token.
const std::string& GetIdentifierValue();

/// Get the value of the last Numeric token.
double GetNumericValue();

} // namespace kaleidoscope
