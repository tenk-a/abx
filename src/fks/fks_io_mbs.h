/*
 *  @file   fks_inOutMbs.c
 *  @brief  multi byte character routine
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_IO_MBS_H_INCLUDED
#define FKS_IO_MBS_H_INCLUDED

#include <fks/fks_common.h>
#include <fks/fks_types.h>
//#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (void)         fks_ioMbsInit(int inUtf8flag FKS_ARG_INI(0), int outUtf8flag FKS_ARG_INI(0));
FKS_LIB_DECL (void)         fks_ioMbsOutputInit(int outUtf8flag);
FKS_LIB_DECL (fks_isize_t)  fks_wcsFromMbs(wchar_t d[], size_t dl, char const* s, size_t sl);
FKS_LIB_DECL (fks_isize_t)  fks_mbsFromWcs(char d[], size_t dl, wchar_t const* s, size_t sl);
FKS_LIB_DECL (size_t)       fks_ioMbsToOutput(char d[], size_t dl, char const* s);
FKS_LIB_DECL (size_t)       fks_ioMbsFromOutput(char d[], size_t dl, char const* s);
//FKS_LIB_DECL (char*)		fks_ioMbsConvMalloc(char const* s, size_t sl, size_t* pDstSz FKS_ARG_INI(0));

FKS_LIB_DECL (char**)       fks_convArgWcsToMbs(int argc, wchar_t * srcArgv[]);

#if 1 //def FKS_USE_JAPAN
FKS_LIB_DECL (int)          fks_ioIsJapan(void);
#endif

#ifdef FKS_WIN32
typedef int			fks_codepage_t;
extern fks_codepage_t fks_io_mbs_codepage;
extern fks_codepage_t fks_io_mbs_env_codepage;
extern fks_codepage_t fks_io_mbs_output_codepage;
#else	// kari
typedef ptrdiff_t	fks_codepage_t;
#endif

FKS_LIB_DECL (size_t)		fks_ioMbsConvCP(fks_codepage_t dcp, char d[], size_t dl, fks_codepage_t scp, char const* s, size_t sl);
FKS_LIB_DECL (char*)		fks_ioMbsConvCpMalloc(fks_codepage_t dcp, fks_codepage_t scp, char const* s, size_t sl, size_t* pDstSz FKS_ARG_INI(0));

#ifdef __cplusplus
}
#endif

#ifdef FKS_WIN32
#ifdef __cplusplus
class Fks_IoCPConvStr {
public:
    Fks_IoCPConvStr(char const* msg, fks_codepage_t icp, fks_codepage_t ocp);
    ~Fks_IoCPConvStr();

    char*   str;
private:
    enum {SBUF_SZ = 130};
    char	mlcFlg_;
    char    sbuf_[SBUF_SZ];
};
struct Fks_IoSrccodeToOutStr : public Fks_IoCPConvStr {
    Fks_IoSrccodeToOutStr(char const* msg, bool outUtf8Flag=false);
};
struct Fks_IoMbsToOutStr : public Fks_IoCPConvStr {
    Fks_IoMbsToOutStr(char const* msg, bool outUtf8Flag=false);
};

#define FKS_OUT_S(s,utf8)           Fks_IoMbsToOutStr((s),(utf8)).str
#define FKS_MBS_S(s)                Fks_IoMbsToOutStr(s).str
#define FKS_UTF8_S(s)               Fks_IoMbsToOutStr((s),true).str
#define FKS_SRCCODE_S(s)            Fks_IoSrccodeToOutStr(s).str
#define FKS_SRCCODE_TO_UTF8_S(s)    Fks_IoSrccodeToOutStr((s), true).str
#endif
#else
#define FKS_OUT_S(s,utf8)           (s)
#define FKS_MBS_S(s)                (s)
#define FKS_UTF8_S(s)               (s)
#define FKS_SRCCODE_S(s)            (s)
#define FKS_SRCCODE_TO_UTF8_S(s)    (s)
#endif

#endif  // FKS_IO_MBS_H_INCLUDED
