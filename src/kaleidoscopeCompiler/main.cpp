#include <kaleidoscope/codeGenContext.h>
#include <kaleidoscope/lexer.h>
#include <kaleidoscope/logger.h>
#include <kaleidoscope/parser.h>

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

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
    if ( i_argc != 3 )
    {
        LogError( "usage: kaleidoscopeCompiler <sourceFile> <objectFile>" );
        return -1;
    }

    std::string   sourceFile( i_argv[ 1 ] );
    std::ifstream fileStream( sourceFile );
    if ( !fileStream )
    {
        LogError( "Failed to load file: %s", sourceFile.c_str() );
        return -1;
    }

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    // Find target architecture from current machine.
    std::string         targetTriple = llvm::sys::getDefaultTargetTriple();
    std::string         err;
    const llvm::Target* targetArch = llvm::TargetRegistry::lookupTarget( targetTriple, err );
    if ( targetArch == nullptr )
    {
        LogError( "Failed to find target architecture: %s, %s", targetTriple.c_str(), err.c_str() );
        return -1;
    }

    // Create target machine.
    std::string cpu      = "generic";
    std::string features = "";

    llvm::TargetOptions                  targetOptions;
    llvm::Optional< llvm::Reloc::Model > model;
    llvm::TargetMachine*                 targetMachine =
        targetArch->createTargetMachine( targetTriple, cpu, features, targetOptions, model );

    CodeGenContext codeGenContext;
    codeGenContext.InitializeModule( targetTriple, targetMachine );
    std::string lineString;
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

    // Compiling to object code.
    std::error_code      errorCode;
    std::string          objectFile( i_argv[ 2 ] );
    llvm::raw_fd_ostream dest( objectFile.c_str(), errorCode, llvm::sys::fs::OF_None );
    if ( errorCode )
    {
        LogError( "Could not open file: %s", errorCode.message().c_str() );
        return -1;
    }

    llvm::legacy::PassManager            passManager;
    llvm::TargetMachine::CodeGenFileType fileType = llvm::TargetMachine::CGFT_ObjectFile;

    if ( targetMachine->addPassesToEmitFile( passManager, dest, nullptr, fileType ) )
    {
        LogError( "TargetMachine can't emit a file of this type" );
        return -1;
    }

    passManager.run( *codeGenContext.GetModule() );
    dest.flush();

    LogInfo( "Wrote object file: '%s'", objectFile.c_str() );

    return 0;
}
