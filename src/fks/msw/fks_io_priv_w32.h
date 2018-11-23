#ifndef FKS_IO_PRIV_W32_H_INCLUDED
#define FKS_IO_PRIV_W32_H_INCLUDED

#include <fks/fks_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FKS_ISALPHA(c)			(((c) <= 'z') && ((c) >= 'A') && ((c) >= 'a' || (c) <= 'Z'))

#if 1	// nano sec.
#define FKS_W32FTIME_TO_TIME(w64time) 	(w64time)
#define fks_time_to_w32ftime(tm)		(tm)
#else	// micro sec.
#define FKS_W32FTIME_TO_TIME(w64time) 	((w64time) / 1000)	///< nano sec. -> micro sec.
#define fks_time_to_w32ftime(tm)		((tm) * 1000)		///< micro sec. -> nano sec.
#endif

/// u32[2] -> uint64_t
#ifdef FKS_BYTE_ALIGN
#define FKS_U32X2P_TO_U64(u32x2p)		(*(uint64_t*)(u32x2p))
#elif (defined FKS_BIG_ENDIAN)
#define FKS_U32X2P_TO_U64(u32x2p)		(((uint64_t)(((uint32_t*)(u32x2p))[0]) << 32) | ((uint32_t*)(u32x2p))[1])
#else
#define FKS_U32X2P_TO_U64(u32x2p)		(((uint64_t)(((uint32_t*)(u32x2p))[1]) << 32) | ((uint32_t*)(u32x2p))[0])
#endif

/// win32-file-attributes to stat::st_mode
#define FKS_W32FATTER_TO_STATMODE(w32atr) 													\
		(	( (((w32atr) & FILE_ATTRIBUTE_READONLY ) ? FKS_S_IFBLK|0555 : FKS_S_IFBLK|0777)	\
			 |(((w32atr) & FILE_ATTRIBUTE_DIRECTORY) ? FKS_S_IFDIR      : FKS_S_IFREG))		\
		  & (  ((w32atr) & FILE_ATTRIBUTE_HIDDEN) ? ~7 : ~0 )								\
		)

/// stat::st_mode to win32-file-attributes
#define FKS_STATMODE_TO_W32FATTR(mode)												\
	(  (((mode) & FKS_S_IFDIR ) ? FILE_ATTRIBUTE_DIRECTORY	 : 0)					\
	 | (((mode) & FKS_S_IWRITE)== 0 ? FILE_ATTRIBUTE_READONLY : 0)					\
	 | ((((mode) & FKS_S_IFDIR)| !((mode)&FKS_S_IWRITE))?0:FILE_ATTRIBUTE_NORMAL)	\
	)


/* ======================================================================== */

extern int _fks_priv_mbswcs_codepage;

#define FKS_CODEPAGE_DEFAULT		_fks_priv_mbswcs_codepage	//0
#define FKS_WCS_FROM_MBS(d,dl,s,sl)	MultiByteToWideChar(FKS_CODEPAGE_DEFAULT,0,(s),(sl),(d),(dl))
#define FKS_MBS_FROM_WCS(d,dl,s,sl)	WideCharToMultiByte(FKS_CODEPAGE_DEFAULT,0,(s),(sl),(d),(dl),0,0)

#define FKS_PRIV_FH2WH(fh)			(HANDLE)(fh)
#define FKS_PRIV_FH_CONV(fh)		fks_priv_fh_conv(fh)
FKS_FAST_DECL (fks_fh_t)			fks_priv_fh_conv(fks_fh_t hdl) FKS_NOEXCEPT;

enum { FKS_LONGNAME_FROM_CS_LEN = 248/*260*/ };
#if 1 //def FKS_HAS_INLINE_ALLOCA
#define FKS_INNR_FPATH_MAX			FKS_LONGNAME_FROM_CS_LEN
#define FKS_ALLOCA(l)				fks_alloca(l)
#else
#define FKS_INNR_FPATH_MAX			4096
#define FKS_ALLOCA(l)				0
#endif
// * Note on optimization because it depends on the memory of the local scope array variable remaining.
#define FKS_LONGFNAME_FROM_CS(d, s) do {										\
		wchar_t		fks_buf[FKS_INNR_FPATH_MAX+1];								\
		const char*	fks_s  = (s);												\
		wchar_t*	fks_d  = fks_buf;											\
		size_t		fks_sl, fks_dl;												\
		fks_dl = fks_priv_longfname_from_cs_subr1(fks_s, &fks_sl);				\
		if (fks_dl >= FKS_INNR_FPATH_MAX)										\
			fks_d = (wchar_t*)FKS_ALLOCA((fks_dl+6)*sizeof(wchar_t));			\
		(d) = fks_priv_longfname_from_cs_subr2(fks_d, fks_dl, fks_s, fks_sl);	\
	} while (0)

//FKS_LIB_DECL(size_t)   	fks_priv_longfname_from_cs_subr1(char const* s, size_t* pLen) FKS_NOEXCEPT;
//FKS_LIB_DECL(wchar_t*)	fks_priv_longfname_from_cs_subr2(wchar_t* d, size_t dl, char const* s, size_t sl) FKS_NOEXCEPT;


#ifdef __cplusplus
}
#endif

#endif
