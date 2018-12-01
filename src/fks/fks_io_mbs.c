/*
 *  @file   fks_io_mbs.c
 *  @brief  input/output multi byte character routine
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>
#include <fks_io_mbs.h>

#ifdef FKS_WIN32
#include "msw/fks_io_mbs_w32.hh"
#elif defined FKS_LINUX // || defined FKS_BSD
#include "linux/fks_io_mbs_linux.hh"
#endif

#ifdef __cplusplus
Fks_IoMbs2Out::Fks_IoMbs2Out(char const* msg)
{
	size_t l = strlen(msg) * fks_ioMbcLenMaxO() + 1;
	this->p  = sbuf_;
	if (l > SBUF_SZ)
		this->p = (char*)fks_calloc(1, l);
	if (this->p)
		fks_ioMbsToOutput(this->p, l, msg);
}

Fks_IoMbs2Out::~Fks_IoMbs2Out()
{
	if (p != sbuf_)
		fks_free(p);
}
#endif
