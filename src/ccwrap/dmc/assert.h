#pragma once

#include <ccwrap_header.h>

#include <../include/assert.h>

#ifndef __cplusplus
#define static_assert(c, ...)		_Static_assert(c, __VA_ARGS__)
#endif
