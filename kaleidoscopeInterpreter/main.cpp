#include <kaleidoscope/codeGenContext.h>
#include <kaleidoscope/lexer.h>
#include <kaleidoscope/parser.h>

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>

using namespace kaleidoscope;

void HandleDefinition( Parser& io_parser, CodeGenContext& io_codeGenContext )
{
    auto expr = io_parser.ParseDefinitionExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( io_codeGenContext );
        if ( value != nullptr )
        {
            fprintf( stderr, "Parsed a function definition.\n" );
            value->print( llvm::errs() );
            fprintf( stderr, "\n" );
        }
    }
    else
    {
        // Skip token for error recovery.
        io_parser.ParseNextToken();
    }
}

void HandleExtern( Parser& io_parser, CodeGenContext& io_codeGenContext )
{
    std::unique_ptr< PrototypeAST > expr = io_parser.ParseExternExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( io_codeGenContext );
        if ( value != nullptr )
        {
            fprintf( stderr, "Parsed an extern\n" );
            value->print( llvm::errs() );
            fprintf( stderr, "\n" );
        }
    }
    else
    {
        // Skip token for error recovery.
        io_parser.ParseNextToken();
    }
}

void HandleTopLevelExpression( Parser& io_parser, CodeGenContext& io_codeGenContext )
{
    // Evaluate a top-level expression into an anonymous function.
    std::unique_ptr< FunctionAST > expr = io_parser.ParseTopLevelExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( io_codeGenContext );
        if ( value != nullptr )
        {
            fprintf( stderr, "Parsed a top-level expr\n" );
            value->print( llvm::errs() );
            fprintf( stderr, "\n" );
        }
    }
    else
    {
        // Skip token for error recovery.
        io_parser.ParseNextToken();
    }
}

void MainLoop()
{
    CodeGenContext codeGenContext;

    fprintf( stderr, "kaleidoscope> " );
    std::string line;
    while ( std::getline( std::cin, line ) )
    {
        Parser parser( line );
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

        fprintf( stderr, "kaleidoscope> " );
    }
}

int main()
{
    MainLoop();
    return 0;
}
