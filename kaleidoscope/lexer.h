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
    Token_Numeric    = -5
};

/// The lexer class consumes a block of text to return
/// identifiable tokens.
class Lexer
{
public:
    /// Ctor.
    explicit Lexer( const std::string& i_text );

    /// Get the next token.
    int GetNextToken();

    /// Get the value of the last Identifier token.
    const std::string& GetIdentifierValue();

    /// Get the value of the last Numeric token.
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
