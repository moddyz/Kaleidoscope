#pragma once

#include <kaleidoscope/api.h>

#include <cstdio>
#include <stdarg.h>
#include <utility>

namespace kaleidoscope
{
/// Log an error message.
/// \param i_errorMessage message to log as an error.
KALEIDOSCOPE_API
void LogError( const char* i_errorMessage, ... );

/// Log an informative message.
/// \param i_message mesage log as an information.
KALEIDOSCOPE_API
void LogInfo( const char* i_message, ... );

} // namespace kaleidoscope
