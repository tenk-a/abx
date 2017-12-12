/**
 *  @file   stdnoreturn.h
 *  @brief  _Noreturn   (B.22)
 */
#pragma once

#include <ccwrap_header.h>

#if 0
#ifdef _Noreturn
#define noreturn				_Noreturn
#define __noreturn_is_defined   1
// use _Noreturn -> __declspec(noreturn) -> __declspec(_Noreturn)
#endif
#else
 #define noreturn               __declspec(noreturn)
 #define __noreturn_is_defined  1
 #undef _Noreturn				// use _Noreturn -> __declspec(noreturn)->__declspec(__declspec(noreturn))
#endif
