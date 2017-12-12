#ifndef __CWRAP_STDDEF_H
#define __CWRAP_STDDEF_H

#pragma once
#include "ccwrap_header.h"

#if __BORLANDC__ >= 0x700
#include <../crtl/_stddef.h>
#else
#include __CCWRAP_NATIVE_C_HEADER_PATH(_stddef.h)
#endif

#endif
