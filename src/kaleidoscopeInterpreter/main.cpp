#include <kaleidoscope/KaleidoscopeJIT.h>
#include <kaleidoscope/codeGenContext.h>
#include <kaleidoscope/lexer.h>
#include <kaleidoscope/parser.h>

#include <llvm/IR/Function.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>

using namespace kaleidoscope;

typedef double ( *GetDoubleFn )();

void HandleDefinition( Parser& io_parser, CodeGenContext& io_codeGenContext, llvm::orc::KaleidoscopeJIT& io_jit )
{
    std::unique_ptr< FunctionAST > expr = io_parser.ParseDefinitionExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( io_codeGenContext );
        if ( value != nullptr )
        {
            fprintf( stderr, "Parsed a function definition.\n" );
            value->print( llvm::errs() );
            io_jit.addModule( std::move( io_codeGenContext.MoveModule() ) );
            io_codeGenContext.InitializeModuleWithJIT( io_jit );
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
            io_codeGenContext.AddFunction( expr );
        }
    }
    else
    {
        // Skip token for error recovery.
        io_parser.ParseNextToken();
    }
}

void HandleTopLevelExpression( Parser&                     io_parser,
                               CodeGenContext&             io_codeGenContext,
                               llvm::orc::KaleidoscopeJIT& io_jit )
{
    // Evaluate a top-level expression into an anonymous function.
    std::unique_ptr< FunctionAST > expr = io_parser.ParseTopLevelExpr();
    if ( expr != nullptr )
    {
        llvm::Value* value = expr->GenerateCode( io_codeGenContext );
        if ( value != nullptr )
        {
            // JIT compile the module.
            llvm::orc::VModuleKey moduleKey = io_jit.addModule( std::move( io_codeGenContext.MoveModule() ) );
            io_codeGenContext.InitializeModuleWithJIT( io_jit );

            // Find anonymous symbol
            llvm::JITSymbol exprSymbol = io_jit.findSymbol( "__anon_expr" );
            assert( exprSymbol );

            // Get the expression's symbol address, and cast it to a function ptr which
            // takes no arguments and returns a double on invocation.
            llvm::Expected< uintptr_t > addr = exprSymbol.getAddress();
            assert( addr );
            GetDoubleFn functionPtr = ( GetDoubleFn )( uintptr_t ) addr.get();

            // Evaluate function ptr.
            fprintf( stderr, "Evaluated to %f\n", functionPtr() );

            // Delete module from jit.
            io_jit.removeModule( moduleKey );
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
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    CodeGenContext codeGenContext;

    llvm::orc::KaleidoscopeJIT jit;
    codeGenContext.InitializeModuleWithJIT( jit );

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
            HandleDefinition( parser, codeGenContext, jit );
            break;
        case Token_Extern:
            HandleExtern( parser, codeGenContext );
            break;
        default:
            HandleTopLevelExpression( parser, codeGenContext, jit );
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
