#pragma once

#include <kaleidoscope/api.h>

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
    Token_Numeric    = -5,

    /// Control flow
    Token_If = -6;
    Token_Then = -7;
    Token_Else = -8;
};

/// The Lexer consumes text and produces identifiable and relevant tokens to then be consumed by the parser.
class Lexer
{
public:
    /// Ctor.
    KALEIDOSCOPE_API
    explicit Lexer( const std::string& i_text );

    /// Get the next token.
    KALEIDOSCOPE_API
    int GetNextToken();

    /// Get the value of the last Identifier token.
    KALEIDOSCOPE_API
    const std::string& GetIdentifierValue();

    /// Get the value of the last Numeric token.
    KALEIDOSCOPE_API
    double GetNumericValue();

private:
    /// Hide private ctor.
    Lexer();

    /// Private function for consuming and returning the next character.
    char getChar();

    std::string m_text;                  /// Text to read.
    size_t      m_position        = 0;   /// Position in the text to consume the next character (via getChar).
    char        m_lastChar        = ' '; /// The last character which was consumed
    std::string m_identifierValue = "";  /// Cached Identifier value.
    double      m_numberValue     = 0.0; /// Cached numerical value.
};

} // namespace kaleidoscope
