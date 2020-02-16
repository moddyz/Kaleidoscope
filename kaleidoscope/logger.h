#pragma once

#include <cstdio>
#include <stdarg.h>
#include <utility>

namespace kaleidoscope
{
/// Log an error message.
/// \param i_errorMessage message to log as an error.
void LogError( const char* i_errorMessage, ... );

/// Log an informative message.
/// \param i_message mesage log as an information.
void LogInfo( const char* i_message, ... );

} // namespace kaleidoscope
