/**
 *  @file   fks_path.h
 *  @brief  file-name functions
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_PATH_H_INCLUDED
#define FKS_PATH_H_INCLUDED

#include <fks/fks_common.h>
#include <stddef.h>
#include <string.h>


// ============================================================================
// macros

#ifndef FKS_PATH_MAX
 #ifdef _WIN32
  #define FKS_PATH_CH_MAX   260/*_MAX_PATH*/
  #define FKS_PATH_MAX      (FKS_PATH_CH_MAX * 8/*6*/)
 #else
  #define FKS_PATH_MAX      1024
 #endif
#endif

#ifndef FKS_PATH_MAX_URL
 #ifdef _WIN32
  #define FKS_PATH_CH_MAX_URL  (0x8000)
  #define FKS_PATH_MAX_URL  (FKS_PATH_CH_MAX_URL * 4)
 #else
  #define FKS_PATH_MAX_URL  (6U*4*FKS_PATH_MAX)
 #endif
#endif

#if defined _WIN32 || defined _DOS || defined _MSDOS
#define FKS_PATH_WINDOS
#endif

#if defined FKS_PATH_WINDOS
#define FKS_PATH_SEP_CHR        '\\'
#define FKS_PATH_SEP_STR        "\\"
#define FKS_PATH_SEP_WCS        L"\\"
#define FKS_PATH_SEP_TCS        _T("\\")
#else
#define FKS_PATH_SEP_CHR        '/'
#define FKS_PATH_SEP_STR        "/"
#define FKS_PATH_SEP_WCS        L"/"
#define FKS_PATH_SEP_TCS        "/"
#endif

#ifndef FKS_C_CONST
#ifdef __cplusplus
#define FKS_C_CONST
#else   // for c
#define FKS_C_CONST     const
#endif
#endif

#if defined FKS_WIN32 // || defined FKS_MAC
 #define FKS_PATH_IGNORECASE
 #if !defined FKS_PATH_UTF8 && !defined FKS_PATH_DBC
  //#define FKS_PATH_DBC
  #define FKS_PATH_UTF8
 #endif
#else
 #if !defined FKS_PATH_ASCII && !defined FKS_PATH_DBC && !defined FKS_PATH_UTF8
  #define FKS_PATH_UTF8
 #endif
#endif
#if defined FKS_PATH_UTF8 && defined FKS_PATH_DBC
 #error Both FKS_PATH_UTF8 and FKS_PATH_DBC are not defined
#endif

#if defined FKS_PATH_WINDOS
FKS_INL_LIB_DECL (int)      fks_pathIsSep(unsigned c) FKS_NOEXCEPT { return c == '\\' || c == '/'; }
#else
FKS_INL_LIB_DECL (int)      fks_pathIsSep(unsigned c) FKS_NOEXCEPT { return c == '/'; }
#endif

// ============================================================================
// char version

#if !(defined FKS_PATH_WCS_COMPILE)

#ifdef __cplusplus
extern "C" {
#endif

//FKS_INL_LIB_DECL (size_t)    fks_pathLen(char const* path) FKS_NOEXCEPT { return (size_t)strlen(path); }
FKS_LIB_DECL (size_t)    fks_pathLen(char const* path) FKS_NOEXCEPT;

FKS_LIB_DECL (int)      fks_pathIsAbs(char const* path) FKS_NOEXCEPT;
FKS_LIB_DECL (int)      fks_pathHasDrive(char const* path) FKS_NOEXCEPT;

FKS_LIB_DECL (size_t)    fks_pathAdjustSize(char const* str, size_t size) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathCpy(char dst[], size_t sz, char const* src) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathCat(char dst[], size_t sz, char const* src) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathCombine(char dst[],size_t sz,char const *dir,char const *nm) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathBaseName(FKS_C_CONST char *adr) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathExt(FKS_C_CONST char *name) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathSkipDrive(FKS_C_CONST char *name) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathSkipDriveRoot(FKS_C_CONST char* name) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathDelBaseName(char name[]) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathDelExt(char name[]) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathGetNoExt(char dst[], size_t sz, char const *src) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathGetBaseNameNoExt(char d[], size_t l, char const *s) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathSetExt(char dst[], size_t sz, char const* src, char const *ext) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathSetDefaultExt(char dst[], size_t sz, char const* src, char const *ext) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathGetNoDotExt(char ext[], size_t sz, char const* src) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathGetDir(char dir[], size_t sz, char const *nm) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathGetDrive(char drv[], size_t sz, char const *nm) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathGetDriveRoot(char dr[],size_t sz,char const *nm) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathCheckPosSep(FKS_C_CONST char* dir, ptrdiff_t pos) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathCheckLastSep(FKS_C_CONST char* dir) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathSkipRootCheckLastSep(FKS_C_CONST char* dir) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathDelLastSep(char dir[]) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathAddSep(char dst[], size_t sz) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathToUpper(char filename[]) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathToLower(char filename[]) FKS_NOEXCEPT;
//FKS_LIB_DECL (char*)  fks_pathToUpperN(char filename[], size_t n) FKS_NOEXCEPT;
//FKS_LIB_DECL (char*)  fks_pathToLowerN(char filename[], size_t n) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathBackslashToSlash(char filePath[]) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathSlashToBackslash(char filePath[]) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathFullpath  (char fullpath[], size_t sz, char const* path, char const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathFullpathSL(char fullpath[], size_t sz, char const* path, char const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathFullpathBS(char fullpath[], size_t sz, char const* path, char const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathRelativePath  (char relPath[], size_t sz, char const* path, char const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathRelativePathSL(char relPath[], size_t sz, char const* path, char const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathRelativePathBS(char relPath[], size_t sz, char const* path, char const* curDir) FKS_NOEXCEPT;

FKS_LIB_DECL (int)      fks_pathCmp(char const* l, char const* r) FKS_NOEXCEPT;
FKS_LIB_DECL (int)      fks_pathNCmp(char const* l, char const* r, size_t n) FKS_NOEXCEPT;
FKS_LIB_DECL (int)      fks_pathDigitCmp(char const* l, char const* r) FKS_NOEXCEPT;
//FKS_LIB_DECL (int)    fks_pathNDigitCmp(char const* l,char const* r,size_t n) FKS_NOEXCEPT;
//FKS_LIB_DECL (int)    fks_pathLowerCmp(char const* l, char const* r) FKS_NOEXCEPT;
FKS_LIB_DECL (char*)    fks_pathStartsWith(FKS_C_CONST char* fname, char const* prefix) FKS_NOEXCEPT;
FKS_LIB_DECL (int)      fks_pathMatchSpec(char const* str, char const* pattern) FKS_NOEXCEPT;

FKS_LIB_DECL (char*)    fks_pathScanArgStr(char arg[],size_t sz,char const *str, unsigned sepChr) FKS_NOEXCEPT;

#ifdef FKS_WIN32
extern int                      _fks_priv_pathUtf8Flag;
#define fks_pathIsUtf8()        _fks_priv_pathUtf8Flag
#define fks_pathSetUtf8(flag)   (_fks_priv_pathUtf8Flag = (flag))
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
FKS_INL_LIB_DECL (char const*)  fks_pathBaseName(char const *p) FKS_NOEXCEPT                                { return fks_pathBaseName((char*)p); }
FKS_INL_LIB_DECL (char const*)  fks_pathExt(char const *name) FKS_NOEXCEPT                                  { return fks_pathExt((char*)name); }
FKS_INL_LIB_DECL (char const*)  fks_pathSkipDrive(char const *name) FKS_NOEXCEPT                            { return fks_pathSkipDrive((char*)name); }
FKS_INL_LIB_DECL (char const*)  fks_pathSkipDriveRoot(char const *name) FKS_NOEXCEPT                        { return fks_pathSkipDriveRoot((char*)name); }
FKS_INL_LIB_DECL (char const*)  fks_pathCheckPosSep(char const* dir, ptrdiff_t pos) FKS_NOEXCEPT            { return fks_pathCheckPosSep((char*)dir,pos); }
FKS_INL_LIB_DECL (char const*)  fks_pathCheckLastSep(char const* dir) FKS_NOEXCEPT                          { return fks_pathCheckLastSep((char*)dir); }
FKS_INL_LIB_DECL (char const*)  fks_pathStartsWith(char const* fname, char const* prefix) FKS_NOEXCEPT      { return fks_pathStartsWith((char*)fname, prefix); }
FKS_INL_LIB_DECL (char*)        fks_pathSetExt(char dst[], size_t sz, char const *ext) FKS_NOEXCEPT  { return fks_pathSetExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (char*)        fks_pathSetDefaultExt(char dst[], size_t sz, char const *ext) FKS_NOEXCEPT { return fks_pathSetDefaultExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (char*)        fks_pathCombine(char dst[], size_t sz, char const *nm) FKS_NOEXCEPT     { return fks_pathCombine(dst, sz, dst, nm); }
FKS_INL_LIB_DECL (char*)        fks_pathScanArgStr(char arg[],size_t sz,char const *str) FKS_NOEXCEPT { return fks_pathScanArgStr(arg,sz,str, ' ');  }
#endif

#endif


// ============================================================================
// wchar_t version
#if defined __cplusplus || defined FKS_PATH_WCS_COMPILE

//FKS_INL_LIB_DECL (size_t)    fks_pathLen(wchar_t const* path) FKS_NOEXCEPT { return (size_t)wcslen(path); }
FKS_LIB_DECL (size_t) fks_pathLen(wchar_t const* path) FKS_NOEXCEPT;

FKS_LIB_DECL (int)          fks_pathIsAbs(wchar_t const* path) FKS_NOEXCEPT;
FKS_LIB_DECL (int)          fks_pathHasDrive(wchar_t const* path) FKS_NOEXCEPT;

FKS_LIB_DECL (size_t)    fks_pathAdjustSize(wchar_t const* str, size_t size) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathCpy(wchar_t dst[], size_t sz, wchar_t const* src) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathCat(wchar_t dst[], size_t sz, wchar_t const* src) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathCombine(wchar_t dst[],size_t sz,wchar_t const *dir,wchar_t const *nm) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathBaseName(FKS_C_CONST wchar_t *adr) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathExt(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathSkipDrive(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathSkipDriveRoot(FKS_C_CONST wchar_t* name) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathDelExt(wchar_t name[]) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathGetNoExt(wchar_t dst[], size_t sz, wchar_t const *src) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathGetBaseNameNoExt(wchar_t d[], size_t l, wchar_t const *s) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathSetExt(wchar_t dst[], size_t sz, wchar_t const* src, wchar_t const *ext) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathSetDefaultExt(wchar_t dst[], size_t sz, wchar_t const* src, wchar_t const *ext) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathGetNoDotExt(wchar_t ext[], size_t sz, wchar_t const* src) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathGetDir(wchar_t dir[], size_t sz, wchar_t const *nm) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathGetDrive(wchar_t drv[], size_t sz, wchar_t const *nm) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathGetDriveRoot(wchar_t dr[],size_t sz,wchar_t const *nm) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathCheckPosSep(FKS_C_CONST wchar_t* dir, ptrdiff_t pos) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathCheckLastSep(FKS_C_CONST wchar_t* dir) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathSkipRootCheckLastSep(FKS_C_CONST wchar_t* dir) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathDelLastSep(wchar_t dir[]) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathAddSep(wchar_t dst[], size_t sz) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathToUpper(wchar_t filename[]) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathToLower(wchar_t filename[]) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathBackslashToSlash(wchar_t filePath[]) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathSlashToBackslash(wchar_t filePath[]) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathFullpath  (wchar_t fullpath[], size_t sz, wchar_t const* path, wchar_t const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathFullpathSL(wchar_t fullpath[], size_t sz, wchar_t const* path, wchar_t const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathFullpathBS(wchar_t fullpath[], size_t sz, wchar_t const* path, wchar_t const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathRelativePath  (wchar_t relPath[], size_t sz, wchar_t const* path, wchar_t const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathRelativePathSL(wchar_t relPath[], size_t sz, wchar_t const* path, wchar_t const* curDir) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathRelativePathBS(wchar_t relPath[], size_t sz, wchar_t const* path, wchar_t const* curDir) FKS_NOEXCEPT;

FKS_LIB_DECL (int)          fks_pathCmp(wchar_t const* l, wchar_t const* r) FKS_NOEXCEPT;
FKS_LIB_DECL (int)          fks_pathNCmp(wchar_t const* l, wchar_t const* r, size_t n) FKS_NOEXCEPT;
FKS_LIB_DECL (int)          fks_pathDigitCmp(wchar_t const* l, wchar_t const* r) FKS_NOEXCEPT;
//FKS_LIB_DECL (int)        fks_pathNDigitCmp(wchar_t const* l,wchar_t const* r,size_t n) FKS_NOEXCEPT;
FKS_LIB_DECL (wchar_t*)     fks_pathStartsWith(FKS_C_CONST wchar_t* fname, wchar_t const* prefix) FKS_NOEXCEPT;
FKS_LIB_DECL (int)          fks_pathMatchSpec(wchar_t const* str, wchar_t const* pattern) FKS_NOEXCEPT;

FKS_LIB_DECL (wchar_t*)     fks_pathScanArgStr(wchar_t arg[],size_t sz,wchar_t const *str, unsigned sepChr) FKS_NOEXCEPT;
#endif

#if defined __cplusplus
FKS_INL_LIB_DECL (wchar_t const*)   fks_pathBaseName(wchar_t const *p) FKS_NOEXCEPT                                     { return fks_pathBaseName((wchar_t*)p); }
FKS_INL_LIB_DECL (wchar_t const*)   fks_pathExt(wchar_t const *name) FKS_NOEXCEPT                                       { return fks_pathExt((wchar_t*)name); }
FKS_INL_LIB_DECL (wchar_t const*)   fks_pathSkipDrive(wchar_t const *name) FKS_NOEXCEPT                                 { return fks_pathSkipDrive((wchar_t*)name); }
FKS_INL_LIB_DECL (wchar_t const*)   fks_pathSkipDriveRoot(wchar_t const *name) FKS_NOEXCEPT                             { return fks_pathSkipDriveRoot((wchar_t*)name); }
FKS_INL_LIB_DECL (wchar_t const*)   fks_pathCheckPosSep(wchar_t const* dir, ptrdiff_t pos) FKS_NOEXCEPT                 { return fks_pathCheckPosSep((wchar_t*)dir,pos); }
FKS_INL_LIB_DECL (wchar_t const*)   fks_pathCheckLastSep(wchar_t const* dir) FKS_NOEXCEPT                               { return fks_pathCheckLastSep((wchar_t*)dir); }
FKS_INL_LIB_DECL (wchar_t const*)   fks_pathStartsWith(wchar_t const* fname, wchar_t const* prefix) FKS_NOEXCEPT        { return fks_pathStartsWith((wchar_t*)fname, prefix); }
FKS_INL_LIB_DECL (wchar_t*)         fks_pathSetExt(wchar_t dst[], size_t sz, wchar_t const *ext) FKS_NOEXCEPT    { return fks_pathSetExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (wchar_t*)         fks_pathSetDefaultExt(wchar_t dst[], size_t sz, wchar_t const *ext) FKS_NOEXCEPT { return fks_pathSetDefaultExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (wchar_t*)         fks_pathCombine(wchar_t dst[], size_t sz, wchar_t const *nm) FKS_NOEXCEPT    { return fks_pathCombine(dst, sz, dst, nm); }
FKS_INL_LIB_DECL (wchar_t*)         fks_pathScanArgStr(wchar_t arg[],size_t sz,wchar_t const *str) FKS_NOEXCEPT  { return fks_pathScanArgStr(arg,sz,str, ' ');   }
#endif

#endif      // FKS_PATH_H_INCLUDED.
