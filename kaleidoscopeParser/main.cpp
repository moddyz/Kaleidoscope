#include <kaleidoscope/codeGenContext.h>
#include <kaleidoscope/lexer.h>
#include <kaleidoscope/parser.h>

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>

static kaleidoscope::CodeGenContext s_codeGenContext;

void HandleDefinition()
{
    auto expr = kaleidoscope::ParseDefinitionExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( s_codeGenContext );
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
        kaleidoscope::ParseNextToken();
    }
}

void HandleExtern()
{
    std::unique_ptr< kaleidoscope::PrototypeAST > expr = kaleidoscope::ParseExternExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( s_codeGenContext );
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
        kaleidoscope::ParseNextToken();
    }
}

void HandleTopLevelExpression()
{
    // Evaluate a top-level expression into an anonymous function.
    std::unique_ptr< kaleidoscope::FunctionAST > expr = kaleidoscope::ParseTopLevelExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( s_codeGenContext );
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
