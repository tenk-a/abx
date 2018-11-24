/*
 *  @file   fks_io.cpp
 *  @brief  System level I/O functions and types.
 *	@author	Masashi Kitamura (tenka@6809.net)
 *	@license Boost Software Lisence Version 1.0
 */

#include <fks/fks_config.h>

#ifdef FKS_WIN32
#include "msw/fks_io_w32.cc"
#elif defined FKS_LINUX // || defined FKS_BSD
#include "linux/fks_io_linux.cc"
#endif
