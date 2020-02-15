#include <cstdio>
#include <stdarg.h>

namespace kaleidoscope
{
void LogError( const char* i_errorMessage, ... )
{
    constexpr size_t bufferSize = 256;
    char             buffer[ bufferSize ];
    va_list          args;
    va_start( args, i_errorMessage );
    vsnprintf( buffer, bufferSize, i_errorMessage, args );
    va_end( args );
    fprintf( stderr, "Error: %s\n", buffer );
}

} // namespace kaleidoscope
