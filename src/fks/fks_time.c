/*
 *  @file   fks_time.c
 *  @brief  
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */

#include <fks/fks_config.h>

#ifdef FKS_WIN32
#include "msw/fks_time_w32.cc"
#elif defined FKS_LINUX // || defined FKS_BSD
#include "linux/fks_time_linux.cc"
#endif