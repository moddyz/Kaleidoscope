#include <kaleidoscope/lexer.h>
#include <kaleidoscope/logger.h>

namespace
{
/// Static storage of different types of token values.
} // namespace

namespace kaleidoscope
{
Lexer::Lexer( const std::string& i_text )
    : m_text( i_text )
{
}

int Lexer::GetNextToken()
{
    // Skip all whitespace characters.
    while ( isspace( m_lastChar ) )
    {
        m_lastChar = getChar();
    }

    if ( isalpha( m_lastChar ) )
    {
        // Handle non-numerical tokens.
        m_identifierValue = m_lastChar;
        while ( isalnum( m_lastChar = getChar() ) )
        {
            m_identifierValue += m_lastChar;
        }

        if ( m_identifierValue == "def" )
        {
            return Token_Def;
        }
        else if ( m_identifierValue == "extern" )
        {
            return Token_Extern;
        }
        else if ( m_identifierValue == "if" )
        {
            return Token_If;
        }
        else if ( m_identifierValue == "then" )
        {
            return Token_Then;
        }
        else if ( m_identifierValue == "else" )
        {
            return Token_Else;
        }

        return Token_Identifier;
    }
    else if ( isdigit( m_lastChar ) || m_lastChar == '.' )
    {
        // Handle numerical double-precision tokens.
        std::string numberStr;
        do
        {
            numberStr += m_lastChar;
            m_lastChar = getChar();
        } while ( isdigit( m_lastChar ) || m_lastChar == '.' );

        m_numberValue = strtod( numberStr.c_str(), 0 );
        return Token_Numeric;
    }
    else if ( m_lastChar == EOF )
    {
        return Token_Eof;
    }
    else
    {
        int thisCharInt = ( int ) m_lastChar;
        m_lastChar      = getChar();
        return thisCharInt;
    }
}

const std::string& Lexer::GetIdentifierValue()
{
    return m_identifierValue;
}

double Lexer::GetNumericValue()
{
    return m_numberValue;
}

char Lexer::getChar()
{
    if ( m_position >= m_text.size() )
    {
        return EOF;
    }

    return m_text.at( m_position++ );
}

} // namespace kaleidoscope
