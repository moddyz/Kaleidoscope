#include <kaleidoscope/codeGenContext.h>
#include <kaleidoscope/lexer.h>
#include <kaleidoscope/logger.h>
#include <kaleidoscope/parser.h>

#include <fstream>
#include <iostream>

using namespace kaleidoscope;

void HandleDefinition( Parser& io_parser, CodeGenContext& io_codeGenContext )
{
    std::unique_ptr< FunctionAST > expr = io_parser.ParseDefinitionExpr();
    if ( expr != nullptr )
    {
        expr->GenerateCode( io_codeGenContext );
    }
    else
    {
        io_parser.ParseNextToken();
    }
}

void HandleExtern( Parser& io_parser, CodeGenContext& io_codeGenContext )
{
    std::unique_ptr< PrototypeAST > expr = io_parser.ParseExternExpr();
    if ( expr != nullptr )
    {
        expr->GenerateCode( io_codeGenContext );
    }
    else
    {
        io_parser.ParseNextToken();
    }
}

void HandleTopLevelExpression( Parser& io_parser, CodeGenContext& io_codeGenContext )
{
    std::unique_ptr< FunctionAST > expr = io_parser.ParseTopLevelExpr();
    if ( expr != nullptr )
    {
        expr->GenerateCode( io_codeGenContext );
    }
    else
    {
        io_parser.ParseNextToken();
    }
}

int main( int i_argc, char** i_argv )
{
    if ( i_argc != 2 )
    {
        LogError( "usage: kaleidoscopeCompiler <sourceFile>" );
        return -1;
    }

    std::string   sourceFile( i_argv[ 1 ] );
    std::ifstream fileStream( sourceFile );
    if ( !fileStream )
    {
        LogError( "Failed to load file: %s", sourceFile.c_str() );
    }

    CodeGenContext codeGenContext;
    std::string    lineString;
    size_t         lineNumber = 0;
    LogInfo( "Compiling '%s'...", sourceFile.c_str() );
    while ( std::getline( fileStream, lineString ) )
    {
        LogInfo( "%s", lineString.c_str() );

        // Depending on token,
        Parser parser( lineString );
        switch ( parser.ParseCurrentToken() )
        {
        case Token_Eof:
            break;
        case ';': // ignore top-level semicolons.
            parser.ParseNextToken();
            break;
        case Token_Def:
            HandleDefinition( parser, codeGenContext );
            break;
        case Token_Extern:
            HandleExtern( parser, codeGenContext );
            break;
        default:
            HandleTopLevelExpression( parser, codeGenContext );
            break;
        }
    }

    LogInfo( "Successfully compiled '%s', generated IR:", sourceFile.c_str() );
    codeGenContext.Print();

    return 0;
}
