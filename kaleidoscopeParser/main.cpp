#include <kaleidoscope/lexer.h>
#include <kaleidoscope/parser.h>

void HandleDefinition()
{
    if ( kaleidoscope::ParseDefinitionExpr() )
    {
        fprintf( stderr, "Parsed a function definition.\n" );
    }
    else
    {
        // Skip token for error recovery.
        kaleidoscope::ParseNextToken();
    }
}

void HandleExtern()
{
    if ( kaleidoscope::ParseExternExpr() )
    {
        fprintf( stderr, "Parsed an extern\n" );
    }
    else
    {
        // Skip token for error recovery.
        kaleidoscope::ParseNextToken();
    }
}

void HandleTopLevelExpression()
{
    // Evaluate a top-level expression into an anonymous function.
    if ( kaleidoscope::ParseTopLevelExpr() )
    {
        fprintf( stderr, "Parsed a top-level expr\n" );
    }
    else
    {
        // Skip token for error recovery.
        kaleidoscope::ParseNextToken();
    }
}

void MainLoop()
{
    while ( true )
    {
        fprintf( stderr, "ready> " );
        switch ( kaleidoscope::ParseCurrentToken() )
        {
        case kaleidoscope::Token_Eof:
            return;
        case ';': // ignore top-level semicolons.
            kaleidoscope::ParseNextToken();
            break;
        case kaleidoscope::Token_Def:
            HandleDefinition();
            break;
        case kaleidoscope::Token_Extern:
            HandleExtern();
            break;
        default:
            HandleTopLevelExpression();
            break;
        }
    }
}

int main()
{
    fprintf( stderr, "ready> " );
    kaleidoscope::ParseNextToken();
    MainLoop();
    return 0;
}
