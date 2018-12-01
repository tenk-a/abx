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

#ifdef FKS_SRC_DBC
static fks_codepage_t fks_priv_sourcecode_codepage = 0;
#else
static fks_codepage_t fks_priv_sourcecode_codepage = FKS_CP_UTF8;
#endif

#ifdef __cplusplus
Fks_IoMbs2Out::Fks_IoMbs2Out(char const* msg)
{
	size_t l = strlen(msg) * 4 + 1;
	this->p  = sbuf_;
	if (l > SBUF_SZ)
		this->p = (char*)fks_calloc(1, l);
	if (this->p)
		fks_mbsConvCP(fks_io_mbs_output_codepage, this->p, l, fks_priv_sourcecode_codepage, msg);
}

Fks_IoMbs2Out::~Fks_IoMbs2Out()
{
	if (p != sbuf_)
		fks_free(p);
}
#endif
