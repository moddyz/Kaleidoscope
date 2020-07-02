#include <cstdio>
#include <stdarg.h>

namespace kaleidoscope
{
/// Utility method for logging to a standard stream.
void Log( FILE* io_stream, const char* i_messageFormat, const char* i_message, va_list i_args )
{
    constexpr size_t bufferSize = 256;
    char             buffer[ bufferSize ];
    vsnprintf( buffer, bufferSize, i_message, i_args );
    fprintf( io_stream, i_messageFormat, buffer );
}

void LogError( const char* i_errorMessage, ... )
{
    va_list args;
    va_start( args, i_errorMessage );
    Log( stderr, "[ERROR] %s\n", i_errorMessage, args );
    va_end( args );
}

void LogInfo( const char* i_message, ... )
{
    va_list args;
    va_start( args, i_message );
    Log( stdout, "[INFO] %s\n", i_message, args );
    va_end( args );
}
} // namespace kaleidoscope
