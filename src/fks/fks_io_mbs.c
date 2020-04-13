/*
 *  @file   fks_io_mbs.c
 *  @brief  input/output multi byte character routine
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks/fks_common.h>
#include <fks/fks_io_mbs.h>

#ifdef FKS_WIN32
#include "msw/fks_io_mbs_w32.hh"

#ifdef FKS_SRC_DBC
fks_codepage_t fks_priv_sourcecode_codepage = 0;
#else
fks_codepage_t fks_priv_sourcecode_codepage = FKS_CP_UTF8;
#endif

/*
FKS_LIB_DECL (char*)
fks_ioMbsConvMalloc(fks_codepage_t dcp, fks_codepage_t scp, char const* s, size_t sl, size_t* pDstSz)
{
	return fks_ioMbsConvCpMalloc(fks_io_mbs_env_codepage, 0, s, sl, pDstSz);
}
*/

FKS_LIB_DECL (char*)
fks_ioMbsConvCpMalloc(fks_codepage_t dcp, fks_codepage_t scp, char const* s, size_t sl, size_t* pDstSz)
{
	size_t	sz;
	size_t	dl = (sl + 1) * 4;
	char*	d;
	FKS_PTR_ASSERT(s);
	d  = (char*)fks_calloc(1, dl);
	if (!d) {
		FKS_PTR_ASSERT(d);
		return NULL;
	}

	sz = fks_ioMbsConvCP(dcp, d, dl, scp, s, sl);
	if (sz > 0) {
		if (sz+1 < dl) {
			d = (char*)fks_realloc(d, sz+1);
			d[sz] = 0;
		}
	}
	if (pDstSz)
		*pDstSz = sz;
	return d;
}


#ifdef __cplusplus

Fks_IoSrccodeToOutStr::Fks_IoSrccodeToOutStr(char const* msg, bool outUtf8Flag)
    : Fks_IoCPConvStr(msg, fks_priv_sourcecode_codepage, (outUtf8Flag) ? FKS_CP_UTF8 : fks_io_mbs_output_codepage)
{
}

Fks_IoMbsToOutStr::Fks_IoMbsToOutStr(char const* msg, bool outUtf8Flag)
    : Fks_IoCPConvStr(msg, fks_io_mbs_codepage, (outUtf8Flag) ? FKS_CP_UTF8 : fks_io_mbs_output_codepage)
{
}

Fks_IoCPConvStr::Fks_IoCPConvStr(char const* msg, fks_codepage_t icp, fks_codepage_t ocp)
{
	mlcFlg_ = 0;
    if (icp == ocp) {
        this->str  = (char*)msg;
        return;
    }
    size_t ml = strlen(msg);
    size_t l = ml * 4 + 1;
    this->str  = sbuf_;
    if (l > SBUF_SZ) {
        this->str = (char*)fks_calloc(1, l);
        mlcFlg_ = 1;
    }
    if (this->str)
        fks_ioMbsConvCP(ocp, this->str, l, icp, msg, ml);
}

Fks_IoCPConvStr::~Fks_IoCPConvStr()
{
    if (mlcFlg_)
        fks_free(this->str);
}
#endif

#elif 1 //defined FKS_LINUX
#include "uni/fks_io_mbs_uni.hh"
#endif
