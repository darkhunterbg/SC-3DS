#pragma once

#include <cstdarg>

#if defined (_3DS)
extern void FatalError(const char* error, ...);
#define EXCEPTION(TEXT, ...)  FatalError(TEXT, ## __VA_ARGS__); 

#elif defined(_WIN32)
#if defined(_DEBUG)
#define EXCEPTION(TEXT, ...) __debugbreak();  /// ## __VA_ARGS__
#else 
#define EXCEPTION(TEXT, ...) 
#endif
#endif

#define GAME_ASSERT(COND, MESSAGE, ...) if(!(COND)) EXCEPTION(MESSAGE, ## __VA_ARGS__); 