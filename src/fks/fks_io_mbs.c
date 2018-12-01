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
fks_codepage_t fks_priv_sourcecode_codepage = 0;
#else
fks_codepage_t fks_priv_sourcecode_codepage = FKS_CP_UTF8;
#endif

#ifdef __cplusplus

Fks_IoSrccodeToOutStr::Fks_IoSrccodeToOutStr(char const* msg)
	: Fks_IoCPConvStr(msg, fks_priv_sourcecode_codepage, fks_io_mbs_output_codepage)
{
}

Fks_IoMbsToOutStr::Fks_IoMbsToOutStr(char const* msg, bool outUtf8Flag)
	: Fks_IoCPConvStr(msg, fks_io_mbs_codepage, (outUtf8Flag) ? FKS_CP_UTF8 : fks_io_mbs_output_codepage)
{
}

Fks_IoCPConvStr::Fks_IoCPConvStr(char const* msg, fks_codepage_t icp, fks_codepage_t ocp)
{
	size_t l = strlen(msg) * 4 + 1;
	this->str  = sbuf_;
	if (l > SBUF_SZ)
		this->str = (char*)fks_calloc(1, l);
	if (this->str)
		fks_mbsConvCP(ocp, this->str, l, icp, msg);
}

Fks_IoCPConvStr::~Fks_IoCPConvStr()
{
	if (this->str != sbuf_)
		fks_free(this->str);
}
#endif
