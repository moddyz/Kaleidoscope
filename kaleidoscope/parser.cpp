#include <kaleidoscope/lexer.h>
#include <kaleidoscope/logger.h>
#include <kaleidoscope/parser.h>

#include <map>

namespace
{
using namespace kaleidoscope;

/// Binary operation precendence.
/// An operator with higher number will be evaluated before one with a lower number.
static std::map< char, int > s_binaryOperationPrecedence = {{'<', 10}, {'+', 20}, {'-', 20}, {'*', 20}};

} // namespace

namespace kaleidoscope
{
Parser::Parser( const std::string& i_text )
    : m_lexer( i_text )
{
    /// Prime the current token.
    ParseNextToken();
}

int Parser::ParseCurrentToken()
{
    return m_currentToken;
}

int Parser::ParseNextToken()
{
    m_currentToken = m_lexer.GetNextToken();
    return m_currentToken;
}

/// Parse the current numeric expression.
/// \return the parsed numeric AST expression.
std::unique_ptr< ExprAST > Parser::parseNumericExpr()
{
    std::unique_ptr< NumericExprAST > numeric = std::make_unique< NumericExprAST >( m_lexer.GetNumericValue() );
    ParseNextToken();
    return std::move( numeric );
}

/// Parse the current parenthesis expression.
/// \return the parsed expression within the parenthesis.
std::unique_ptr< ExprAST > Parser::parseParenthesisExpr()
{
    // Consume '('
    ParseNextToken();

    std::unique_ptr< ExprAST > expr = parseExpr();
    if ( expr == nullptr )
    {
        return nullptr;
    }

    if ( ParseCurrentToken() != ')' )
    {
        LogError( "Expected ')' after expression" );
        return nullptr;
    }

    // Consume ')'
    ParseNextToken();

    return expr;
}

/// Parse the current identifier expression.
/// \return the parsed identifier expression.
std::unique_ptr< ExprAST > Parser::parseIdentifierExpr()
{
    std::string identifier = m_lexer.GetIdentifierValue();

    // Consume current identifier.
    ParseNextToken();

    // If the current token is not a parenthesis, then it is a simple variable.
    if ( ParseCurrentToken() != '(' )
    {
        return std::make_unique< VariableExprAST >( identifier );
    }

    // It is as calling expression, with potential arguments.

    // Consume '('
    ParseNextToken();

    // Collect arguments.
    std::vector< std::unique_ptr< ExprAST > > arguments;
    if ( ParseCurrentToken() != ')' )
    {
        while ( true )
        {
            std::unique_ptr< ExprAST > expression = parseExpr();
            if ( expression != nullptr )
            {
                arguments.push_back( std::move( expression ) );
            }
            else
            {
                LogError( "Unknown error." );
                return nullptr;
            }

            if ( ParseCurrentToken() == ')' )
            {
                break;
            }
            else if ( ParseCurrentToken() != ',' )
            {
                LogError( "Expected ')' or ',' in argument list." );
                return nullptr;
            }

            // Consume ','
            ParseNextToken();
        }
    }

    // Consume ')'
    ParseNextToken();

    return std::make_unique< CallExprAST >( identifier, std::move( arguments ) );
}

/// Entry point for parsing a primary expression (identifier, numeric, or parenthensis)
std::unique_ptr< ExprAST > Parser::parsePrimaryExpr()
{
    int token = ParseCurrentToken();
    switch ( token )
    {
    case Token_Identifier:
        return parseIdentifierExpr();
    case Token_Numeric:
        return parseNumericExpr();
    case '(':
        return parseParenthesisExpr();
    case Token_If:
        return parseIfExpr();
    case Token_For:
        return parseForExpr();
    default:
        LogError( "unknown token when expecting an expression: %c", token );
        return nullptr;
    }
}

/// Get the precendence of the current token.
//
/// \returns -1 if the current token is not a binary operator.  Otherwise the precendence of the current binrary
/// operator token will be returned.
int Parser::parseCurrentTokenPrecendence()
{
    if ( !isascii( ParseCurrentToken() ) )
    {
        return -1;
    }

    std::map< char, int >::const_iterator it = s_binaryOperationPrecedence.find( ParseCurrentToken() );
    if ( it == s_binaryOperationPrecedence.end() )
    {
        return -1;
    }

    return it->second;
}

/// Parses the RHS operand of a binary expression.
/// \returns parsed RHS operand expression.
std::unique_ptr< ExprAST > Parser::parseBinaryOperatorRHS( int i_precendence, std::unique_ptr< ExprAST > io_lhs )
{
    while ( true )
    {
        int leftPrecedence = parseCurrentTokenPrecendence();
        if ( i_precendence > leftPrecedence )
        {
            return io_lhs;
        }

        int leftBinaryOperator = ParseCurrentToken();

        // Consume binary operator.
        ParseNextToken();

        // Parse RHS
        std::unique_ptr< ExprAST > rhs = parsePrimaryExpr();
        if ( rhs == nullptr )
        {
            return nullptr;
        }

        int rightPrecedence = parseCurrentTokenPrecendence();
        if ( leftPrecedence < rightPrecedence )
        {
            // Right binary operator takes precedence.
            // Recurse with RHS, as the LHS input.
            //
            // The reasoning for leftPrecedence + 1 is so that binary operations
            // expressions are consumed *up until* we reach a binary operation with
            // same or less precedence as the current 'left' binary operation.
            rhs = parseBinaryOperatorRHS( leftPrecedence + 1, std::move( rhs ) );
        }

        // Merge LHS / RHS with left binary operator to form one a binary expression.
        io_lhs = std::make_unique< BinaryExprAST >( leftBinaryOperator, std::move( io_lhs ), std::move( rhs ) );

        // Loop back to top, continuing to parse binary expressions.
    }
}

/// Parses a potential binary expression.
/// \returns parsed binary expression.
std::unique_ptr< ExprAST > Parser::parseExpr()
{
    std::unique_ptr< ExprAST > lhs = parsePrimaryExpr();
    if ( lhs == nullptr )
    {
        return nullptr;
    }

    return parseBinaryOperatorRHS( 0, std::move( lhs ) );
}

/// Parses a function prototype.
/// \returns parsed function prototype.
std::unique_ptr< PrototypeAST > Parser::parsePrototypeExpr()
{
    if ( ParseCurrentToken() != Token_Identifier )
    {
        LogError( "Expected function name in prototype\n" );
        return nullptr;
    }

    // Cache function name, then move on by consuming it.
    std::string functionName = m_lexer.GetIdentifierValue();
    ParseNextToken();

    // Parse argument names, until we reach a non-identifier.
    std::vector< std::string > argumentNames;
    while ( ParseNextToken() == Token_Identifier )
    {
        argumentNames.push_back( m_lexer.GetIdentifierValue() );
    }

    if ( ParseCurrentToken() != ')' )
    {
        LogError( "Expected ')' after the end of prototype's argument list.\n" );
        return nullptr;
    }

    // Consume ')'
    ParseNextToken();

    return std::make_unique< PrototypeAST >( functionName, argumentNames );
}

std::unique_ptr< FunctionAST > Parser::ParseDefinitionExpr()
{
    if ( m_lexer.GetIdentifierValue() != "def" )
    {
        LogError( "Expected 'def' at the beginning of function definition.\n" );
        return nullptr;
    }

    // Consume 'def'
    ParseNextToken();

    // Parse function prototype.
    std::unique_ptr< PrototypeAST > prototypeExpr = parsePrototypeExpr();
    if ( prototypeExpr == nullptr )
    {
        return nullptr;
    }

    // Parse potential binary operation expression in function definintion.
    std::unique_ptr< ExprAST > definitionExpr = parseExpr();
    if ( definitionExpr == nullptr )
    {
        return nullptr;
    }

    return std::make_unique< FunctionAST >( std::move( prototypeExpr ), std::move( definitionExpr ) );
}

std::unique_ptr< PrototypeAST > Parser::ParseExternExpr()
{
    if ( m_lexer.GetIdentifierValue() != "extern" )
    {
        LogError( "Expected 'extern' at the beginning of extern prototype.\n" );
        return nullptr;
    }

    // Consume 'extern'.
    ParseNextToken();

    return parsePrototypeExpr();
}

std::unique_ptr< FunctionAST > Parser::ParseTopLevelExpr()
{
    std::unique_ptr< ExprAST > expression = parseExpr();
    if ( expression == nullptr )
    {
        return nullptr;
    }

    // Create an anonymous function prototype, with no arguments to construct our function expression.
    std::unique_ptr< PrototypeAST > prototypeExpr =
        std::make_unique< PrototypeAST >( "__anon_expr", std::vector< std::string >() );
    return std::make_unique< FunctionAST >( std::move( prototypeExpr ), std::move( expression ) );
}

std::unique_ptr< ExprAST > Parser::parseIfExpr()
{
    // Consume the 'if'
    ParseNextToken();

    // Parse the conditional expression.
    std::unique_ptr< ExprAST > conditionExpr = parseExpr();
    if ( conditionExpr == nullptr )
    {
        LogError( "Failed to parse conditional expression." );
        return nullptr;
    }

    if ( ParseCurrentToken() != Token_Then )
    {
        LogError( "Expected 'then' expression." );
        return nullptr;
    }

    // Consume 'then'.
    ParseNextToken();

    // Parse the expression of then.
    std::unique_ptr< ExprAST > thenExpr = parseExpr();
    if ( thenExpr == nullptr )
    {
        LogError( "Failed to parse then expression." );
        return nullptr;
    }

    if ( ParseCurrentToken() != Token_Else )
    {
        LogError( "Expected 'else' expression." );
        return nullptr;
    }

    // Consume 'else'.
    ParseNextToken();

    // Parse the expression of then.
    std::unique_ptr< ExprAST > elseExpr = parseExpr();
    if ( elseExpr == nullptr )
    {
        LogError( "Failed to parse 'else' expression." );
        return nullptr;
    }

    return std::make_unique< IfExprAST >( std::move( conditionExpr ), std::move( thenExpr ), std::move( elseExpr ) );
}

std::unique_ptr< ExprAST > Parser::parseForExpr()
{
    // Consume 'for'
    ParseNextToken();

    // Check that we have an variable identifier.
    if ( ParseCurrentToken() != Token_Identifier )
    {
        LogError( "Expected a variable identifier after 'for'." );
        return nullptr;
    }

    // Cache identifier, then consume it.
    std::string variableName = m_lexer.GetIdentifierValue();
    ParseNextToken();

    // Check for variable value assignment.
    if ( ParseCurrentToken() != '=' )
    {
        LogError( "Expected a '=' after variable identifier." );
        return nullptr;
    }

    // Consume '='.
    ParseNextToken();

    // Parse start expression.
    std::unique_ptr< ExprAST > startExpr = parseExpr();
    if ( startExpr == nullptr )
    {
        LogError( "Failed to parse start expression." );
        return nullptr;
    }

    // Expect a ',' before end check.
    if ( ParseCurrentToken() != ',' )
    {
        LogError( "Expected a ',' after for-loop start expression." );
        return nullptr;
    }

    // Consume ','.
    ParseNextToken();

    // Parse end expression.
    std::unique_ptr< ExprAST > endExpr = parseExpr();
    if ( endExpr == nullptr )
    {
        LogError( "Failed to parse end expression." );
        return nullptr;
    }

    // Optional step value.
    std::unique_ptr< ExprAST > stepExpr;
    if ( ParseCurrentToken() == ',' )
    {
        stepExpr = parseExpr();
        if ( stepExpr == nullptr )
        {
            LogError( "Failed to parse step expression." );
            return nullptr;
        }
    }

    // Expect an 'in' after the end or step expression
    if ( ParseCurrentToken() != Token_In )
    {
        LogError( "Expected 'in' after for-loop end or step expression." );
        return nullptr;
    }

    // Consume 'in'.
    ParseNextToken();

    // Parse body expression.
    std::unique_ptr< ExprAST > bodyExpr = parseExpr();
    if ( bodyExpr == nullptr )
    {
        LogError( "Failed to parse body expression." );
        return nullptr;
    }

    return std::make_unique< ForExprAST >( variableName, startExpr, endExpr, stepExpr, bodyExpr );
}

} // namespace kaleidoscope
