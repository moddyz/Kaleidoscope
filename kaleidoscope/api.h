#pragma once

#ifdef _WIN32
#define KALEIDOSCOPE_API __declspec(dllexport)
#else
#define KALEIDOSCOPE_API
#endif

