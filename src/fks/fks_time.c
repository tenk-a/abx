/*
 *  @file   fks_time.c
 *  @brief
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>

#ifdef FKS_WIN32
#include "msw/fks_time_w32.hh"
#elif 1 //defined FKS_LINUX
#include "uni/fks_time_uni.hh"
#endif
