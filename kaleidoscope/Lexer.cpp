#include <kaleidoscope/Lexer.h>

namespace
{
/// Static storage of different types of token values.
static std::string s_identifierValue = "";
static double s_numberValue = 0.0;
}

namespace kaleidoscope
{

int LexerNextToken()
{
    static int lastChar = ' ';

    // Skip all whitespace characters.
    while ( isspace( lastChar ) )
    {
        lastChar = getchar();
    }

    if ( isalpha( lastChar ) )
    {   
        // Handle non-numerical tokens.
        s_identifierValue = lastChar;
        while ( isalnum( lastChar = getchar() ) )
        {
            s_identifierValue += lastChar;
        }
        
        if ( s_identifierValue == "def" )
        {
            return Token_Def;
        }
        else if ( s_identifierValue == "extern" )
        {
            return Token_Extern;
        }

        return Token_Identifier;
    }
    else if ( isdigit( lastChar ) || lastChar == '.' ) 
    {
        // Handle numerical double-precision tokens.
        std::string numberStr;
        do {
            numberStr += lastChar;
            lastChar = getchar();
        } while ( isdigit( lastChar ) || lastChar == '.' );

        s_numberValue = strtod( numberStr.c_str(), 0 );
        return Token_Number;
    }
    else if ( lastChar == EOF )
    {
        return Token_Eof;
    }
    else
    {
        int thisCharInt = (int) lastChar;
        lastChar = getchar(); 
        return thisCharInt;
    }
}

const std::string& LexerGetIdentifierValue()
{
    return s_identifierValue;
}

double LexerGetNumberValue()
{
    return s_numberValue;
}

} // kaleidoscope
