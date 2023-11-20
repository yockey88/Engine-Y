#ifndef YE2_WIN_CRASH_HANDLER_HPP
#define YE2_WIN_CRASH_HANDLER_HPP

#include <Windows.h>

#ifdef YE_DEBUG_BUILD
#define CRASH_HANDLER_EXCEPTION 1

extern DWORD CrashHandlerException(EXCEPTION_POINTERS* ep);
#endif



#endif