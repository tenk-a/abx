/**
 *	@file	fks_path.h
 *	@brief	file-name functions
 *	@author Masashi Kitamura (tenka@6809.net)
 */
#ifndef FKS_PATH_H_INCLUDED
#define FKS_PATH_H_INCLUDED

#include <fks/fks_config.h>
#include <stddef.h>
#include <string.h>


// ============================================================================
// マクロ関係.

// Win以外(unix系)で ファイル名中の全角の \ 対策をしたい場合は定義.
// これを定義すると環境変数 LANG をみて SJIS,gbk,gb2312,big5ならMBC処理をする.
//#define FKS_PATH_DBC

// ファイル名のサイズ. 利用側のバッファ準備用.
// 元々はパス全体の制限だったが現状 1ファイル名の長さになっている感じ.
// ※ win-api 自体は基本的にパス全体でこの制限を受ける.
// fname.cpp では、fks_pathRelativePath?? のみがこの値を使う. (他は参照しない)
#ifndef FKS_PATH_MAX
 #ifdef _WIN32
  #define FKS_PATH_MAX		260/*_MAX_PATH*/		///< 通常のパス名の長さ.※winnt系の場合1ファイル名の長さ.
 #else
  #define FKS_PATH_MAX		1024					///< 通常のパス名の長さ.
 #endif
#endif

// url向け?だが場合によってはこれがパスの最大長. winの場合 最長約0x8000 . それ以外は適当.
// (※ winでは特殊な指定をしないと_PAX_PATHを超えては使えない)
#ifndef FKS_PATH_MAX_URL
 #ifdef _WIN32
  #define FKS_PATH_MAX_URL 	(0x8000)				///< urlとして扱う場合のパス名サイズ.
 #else	// 適当に計算.
  #define FKS_PATH_MAX_URL 	(6U*4*FKS_PATH_MAX)		///< urlとして扱う場合のパス名サイズ.
 #endif
#endif

#if defined _WIN32 || defined _DOS || defined _MSDOS
#define FKS_PATH_WINDOS
#endif

#if defined FKS_PATH_WINDOS
#define FKS_PATH_SEP_CHR		'\\'
#define FKS_PATH_SEP_STR		"\\"
#define FKS_PATH_SEP_WCS		L"\\"
#define FKS_PATH_SEP_TCS		_T("\\")
#else
#define FKS_PATH_SEP_CHR		'/'
#define FKS_PATH_SEP_STR		"/"
#define FKS_PATH_SEP_WCS		L"/"
#define FKS_PATH_SEP_TCS		"/"
#endif

#ifndef FKS_C_CONST
#ifdef __cplusplus
#define FKS_C_CONST 			// c++の場合は 基本は非constで、const,非const２種類作る.
#else	// c のとき.
#define FKS_C_CONST 	const	// Cの場合で 引数はconst, 返り値は非const にする場合に使う.
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

#ifndef FKS_PATH_SIZE
typedef size_t				FKS_PATH_SIZE;
#define FKS_PATH_SIZE		FKS_PATH_SIZE
#endif

#if defined FKS_PATH_WINDOS
FKS_INL_LIB_DECL (int)		fks_pathIsSep(unsigned c) FKS_NOEXCEPT { return c == '\\' || c == '/'; }
#else
FKS_INL_LIB_DECL (int)		fks_pathIsSep(unsigned c) FKS_NOEXCEPT { return c == '/'; }
#endif

// ============================================================================
// char version

#if !(defined FKS_PATH_WCS_COMPILE)

#ifdef __cplusplus
extern "C" {
#endif

FKS_INL_LIB_DECL (FKS_PATH_SIZE)	fks_pathLen(const char* path) FKS_NOEXCEPT { return (FKS_PATH_SIZE)strlen(path); }

FKS_LIB_DECL (int)		fks_pathIsAbs(const char* path) FKS_NOEXCEPT;										///< 絶対パスか否か(ドライブ名の有無は関係なし)
FKS_LIB_DECL (int)		fks_pathHasDrive(const char* path) FKS_NOEXCEPT;									///< ドライブ名がついているか. (file: や http:もドライブ扱い)

FKS_LIB_DECL (FKS_PATH_SIZE)	fks_pathAdjustSize(const char* str, FKS_PATH_SIZE size) FKS_NOEXCEPT; 		///< (なるべく文字を壊さないで)size文字以内の文字数を返す.
FKS_LIB_DECL (char*) 	fks_pathCpy(char dst[], FKS_PATH_SIZE sz, const char* src) FKS_NOEXCEPT; 			///< ファイル名のコピー.
FKS_LIB_DECL (char*) 	fks_pathCat(char dst[], FKS_PATH_SIZE sz, const char* src) FKS_NOEXCEPT; 			///< ファイル名文字列の連結.

FKS_LIB_DECL (char*) 	fks_pathBaseName(FKS_C_CONST char *adr) FKS_NOEXCEPT;								///< ファイルパス名中のディレクトリを除いたファイル名の位置を返す.
FKS_LIB_DECL (char*) 	fks_pathExt(FKS_C_CONST char *name) FKS_NOEXCEPT;									///< 拡張子の位置を返す.
FKS_LIB_DECL (char*) 	fks_pathSkipDrive(FKS_C_CONST char *name) FKS_NOEXCEPT; 							///< ドライブ名をスキップした位置を返す.
FKS_LIB_DECL (char*) 	fks_pathSkipDriveRoot(FKS_C_CONST char* name) FKS_NOEXCEPT; 						///< ドライブ名とルート指定部分をスキップした位置を返す.

FKS_LIB_DECL (char*) 	fks_pathDelExt(char name[]) FKS_NOEXCEPT;											///< 拡張子を削除する.
FKS_LIB_DECL (char*) 	fks_pathGetNoExt(char dst[], FKS_PATH_SIZE sz, const char *src) FKS_NOEXCEPT;		///< 拡張子を外した名前を取得.
FKS_LIB_DECL (char*) 	fks_pathGetBaseNameNoExt(char d[], FKS_PATH_SIZE l, const char *s) FKS_NOEXCEPT;	///< ディレクトリと拡張子を外した名前を取得.

FKS_LIB_DECL (char*) 	fks_pathSetExt(char dst[], FKS_PATH_SIZE sz, const char* src, const char *ext) FKS_NOEXCEPT;		///< 拡張子を、ext に変更する.
FKS_LIB_DECL (char*) 	fks_pathSetDefaultExt(char dst[], FKS_PATH_SIZE sz, const char* src, const char *ext) FKS_NOEXCEPT;	///< 拡張子がなければ、ext を追加する.
FKS_LIB_DECL (char*) 	fks_pathJoin(char dst[],FKS_PATH_SIZE sz,const char *dir,const char *nm) FKS_NOEXCEPT;				///< ディレクトリ名とファイル名の連結.

FKS_LIB_DECL (char*) 	fks_pathGetDir(char dir[], FKS_PATH_SIZE sz, const char *nm) FKS_NOEXCEPT;		///< ディレクトリ名の取得.
FKS_LIB_DECL (char*) 	fks_pathGetDrive(char drv[], FKS_PATH_SIZE sz, const char *nm) FKS_NOEXCEPT; 	///< ドライブ名を取得.
FKS_LIB_DECL (char*) 	fks_pathGetDriveRoot(char dr[],FKS_PATH_SIZE sz,const char *nm) FKS_NOEXCEPT;	///< ドライブ名を取得.

FKS_LIB_DECL (char*) 	fks_pathCheckPosSep(FKS_C_CONST char* dir, ptrdiff_t pos) FKS_NOEXCEPT;			///< posの位置に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (char*) 	fks_pathCheckLastSep(FKS_C_CONST char* dir) FKS_NOEXCEPT;						///< 最後に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (char*) 	fks_pathDelLastSep(char dir[]) FKS_NOEXCEPT;									///< 文字列の最後に \ か / があれば削除.
FKS_LIB_DECL (char*) 	fks_pathAddSep(char dst[], FKS_PATH_SIZE sz) FKS_NOEXCEPT;						///< 文字列の最後に \ / がなければ追加.

FKS_LIB_DECL (char*) 	fks_pathToUpper(char filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strupr.
FKS_LIB_DECL (char*) 	fks_pathToLower(char filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strlwr.
//FKS_LIB_DECL (char*) 	fks_pathToUpperN(char filename[], size_t n) FKS_NOEXCEPT;						///< 全角２バイト目を考慮した strupr.
//FKS_LIB_DECL (char*) 	fks_pathToLowerN(char filename[], size_t n) FKS_NOEXCEPT;						///< 全角２バイト目を考慮した strlwr.

FKS_LIB_DECL (char*) 	fks_pathBackslashToSlash(char filePath[]) FKS_NOEXCEPT; 						///< filePath中の \ を / に置換.
FKS_LIB_DECL (char*) 	fks_pathSlashToBackslash(char filePath[]) FKS_NOEXCEPT; 						///< filePath中の / を \ に置換.

FKS_LIB_DECL (char*) 	fks_pathFullpath  (char fullpath[], FKS_PATH_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< フルパス生成. os依存.
FKS_LIB_DECL (char*) 	fks_pathFullpathSL(char fullpath[], FKS_PATH_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< フルパス生成. / 区切.
FKS_LIB_DECL (char*) 	fks_pathFullpathBS(char fullpath[], FKS_PATH_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< フルパス生成. \ 区切.
FKS_LIB_DECL (char*) 	fks_pathRelativePath  (char relPath[], FKS_PATH_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< 相対パス生成. os依存.
FKS_LIB_DECL (char*) 	fks_pathRelativePathSL(char relPath[], FKS_PATH_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< 相対パス生成. / 区切.
FKS_LIB_DECL (char*) 	fks_pathRelativePathBS(char relPath[], FKS_PATH_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< 相対パス生成. \ 区切.

FKS_LIB_DECL (int)		fks_pathCmp(const char* l, const char* r) FKS_NOEXCEPT; 						///< ファイル名の大小比較.
FKS_LIB_DECL (int)		fks_pathNCmp(const char* l, const char* r, FKS_PATH_SIZE n) FKS_NOEXCEPT;		///< ファイル名のn文字大小比較.
FKS_LIB_DECL (int)		fks_pathDigitCmp(const char* l, const char* r) FKS_NOEXCEPT;					///< 桁違いの数字を数値として大小比較.
//FKS_LIB_DECL (int) 	fks_pathNDigitCmp(const char* l,const char* r,FKS_PATH_SIZE n) FKS_NOEXCEPT;	///< 桁違いの数字を数値としてn文字大小比較.
//FKS_LIB_DECL (int)	fks_pathLowerCmp(const char* l, const char* r) FKS_NOEXCEPT; 					///< 小文字化してのファイル名の大小比較.
FKS_LIB_DECL (char*) 	fks_pathStartsWith(FKS_C_CONST char* fname, const char* prefix) FKS_NOEXCEPT;	///< fnameがprefixで始まっているか否か.
FKS_LIB_DECL (int)		fks_pathMatchWildCard(const char* pattern, const char* str) FKS_NOEXCEPT;		///< ワイルドカード文字(*?)列比較. マッチしたら真.

/// コマンドライン引数や、;区切りの複数のパス指定から、１要素取得.
FKS_LIB_DECL (char*) 	fks_pathScanArgStr(char arg[],FKS_PATH_SIZE sz,const char *str, unsigned sepChr) FKS_NOEXCEPT;

#ifdef FKS_WIN32
extern int 						_fks_priv_pathUtf8Flag;
#define fks_pathIsUtf8()		_fks_priv_pathUtf8Flag
#define fks_pathSetUtf8(flag)	(_fks_priv_pathUtf8Flag = (flag))
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
FKS_INL_LIB_DECL (const char*)	fks_pathBaseName(const char *p) FKS_NOEXCEPT								{ return fks_pathBaseName((char*)p); }
FKS_INL_LIB_DECL (const char*)	fks_pathExt(const char *name) FKS_NOEXCEPT									{ return fks_pathExt((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_pathSkipDrive(const char *name) FKS_NOEXCEPT							{ return fks_pathSkipDrive((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_pathSkipDriveRoot(const char *name) FKS_NOEXCEPT						{ return fks_pathSkipDriveRoot((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_pathCheckPosSep(const char* dir, ptrdiff_t pos) FKS_NOEXCEPT			{ return fks_pathCheckPosSep((char*)dir,pos); }
FKS_INL_LIB_DECL (const char*)	fks_pathCheckLastSep(const char* dir) FKS_NOEXCEPT							{ return fks_pathCheckLastSep((char*)dir); }
FKS_INL_LIB_DECL (const char*)	fks_pathStartsWith(const char* fname, const char* prefix) FKS_NOEXCEPT		{ return fks_pathStartsWith((char*)fname, prefix); }
FKS_INL_LIB_DECL (char*) 		fks_pathSetExt(char dst[], FKS_PATH_SIZE sz, const char *ext) FKS_NOEXCEPT	{ return fks_pathSetExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (char*) 		fks_pathSetDefaultExt(char dst[], FKS_PATH_SIZE sz, const char *ext) FKS_NOEXCEPT { return fks_pathSetDefaultExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (char*) 		fks_pathJoin(char dst[], FKS_PATH_SIZE sz, const char *nm) FKS_NOEXCEPT		{ return fks_pathJoin(dst, sz, dst, nm); }
FKS_INL_LIB_DECL (char*)		fks_pathScanArgStr(char arg[],FKS_PATH_SIZE sz,const char *str) FKS_NOEXCEPT { return fks_pathScanArgStr(arg,sz,str, ' ');	}
#endif

#endif


// ============================================================================
// wchar_t version
#if defined __cplusplus || defined FKS_PATH_WCS_COMPILE

FKS_INL_LIB_DECL (FKS_PATH_SIZE)	fks_pathLen(const wchar_t* path) FKS_NOEXCEPT { return (FKS_PATH_SIZE)wcslen(path); }

FKS_LIB_DECL (int)			fks_pathIsAbs(const wchar_t* path) FKS_NOEXCEPT;									///< 絶対パスか否か(ドライブ名の有無は関係なし)
FKS_LIB_DECL (int)			fks_pathHasDrive(const wchar_t* path) FKS_NOEXCEPT;									///< ドライブ名がついているか. (file: や http:もドライブ扱い)

FKS_LIB_DECL (FKS_PATH_SIZE)	fks_pathAdjustSize(const wchar_t* str, FKS_PATH_SIZE size) FKS_NOEXCEPT; 		///< (なるべく文字を壊さないで)size文字以内の文字数を返す.
FKS_LIB_DECL (wchar_t*) 	fks_pathCpy(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t* src) FKS_NOEXCEPT; 		///< ファイル名のコピー.
FKS_LIB_DECL (wchar_t*) 	fks_pathCat(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t* src) FKS_NOEXCEPT; 		///< ファイル名文字列の連結.

FKS_LIB_DECL (wchar_t*) 	fks_pathBaseName(FKS_C_CONST wchar_t *adr) FKS_NOEXCEPT;							///< ファイルパス名中のディレクトリを除いたファイル名の位置を返す.
FKS_LIB_DECL (wchar_t*) 	fks_pathExt(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT;								///< 拡張子の位置を返す.
FKS_LIB_DECL (wchar_t*) 	fks_pathSkipDrive(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT; 							///< ドライブ名をスキップした位置を返す.
FKS_LIB_DECL (wchar_t*) 	fks_pathSkipDriveRoot(FKS_C_CONST wchar_t* name) FKS_NOEXCEPT; 						///< ドライブ名とルート指定部分をスキップした位置を返す.

FKS_LIB_DECL (wchar_t*) 	fks_pathDelExt(wchar_t name[]) FKS_NOEXCEPT;											///< 拡張子を削除する.
FKS_LIB_DECL (wchar_t*) 	fks_pathGetNoExt(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t *src) FKS_NOEXCEPT;		///< 拡張子を外した名前を取得.
FKS_LIB_DECL (wchar_t*) 	fks_pathGetBaseNameNoExt(wchar_t d[], FKS_PATH_SIZE l, const wchar_t *s) FKS_NOEXCEPT;	///< ディレクトリと拡張子を外した名前を取得.

FKS_LIB_DECL (wchar_t*) 	fks_pathSetExt(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t* src, const wchar_t *ext) FKS_NOEXCEPT; 			///< 拡張子を、ext に変更する.
FKS_LIB_DECL (wchar_t*) 	fks_pathSetDefaultExt(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t* src, const wchar_t *ext) FKS_NOEXCEPT;	///< 拡張子がなければ、ext を追加する.
FKS_LIB_DECL (wchar_t*) 	fks_pathJoin(wchar_t dst[],FKS_PATH_SIZE sz,const wchar_t *dir,const wchar_t *nm) FKS_NOEXCEPT;					///< ディレクトリ名とファイル名の連結.

FKS_LIB_DECL (wchar_t*) 	fks_pathGetDir(wchar_t dir[], FKS_PATH_SIZE sz, const wchar_t *nm) FKS_NOEXCEPT;	///< ディレクトリ名の取得.
FKS_LIB_DECL (wchar_t*) 	fks_pathGetDrive(wchar_t drv[], FKS_PATH_SIZE sz, const wchar_t *nm) FKS_NOEXCEPT; 	///< ドライブ名を取得.
FKS_LIB_DECL (wchar_t*) 	fks_pathGetDriveRoot(wchar_t dr[],FKS_PATH_SIZE sz,const wchar_t *nm) FKS_NOEXCEPT;	///< ドライブ名を取得.

FKS_LIB_DECL (wchar_t*) 	fks_pathCheckPosSep(FKS_C_CONST wchar_t* dir, int pos) FKS_NOEXCEPT;				///< posの位置に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (wchar_t*) 	fks_pathCheckLastSep(FKS_C_CONST wchar_t* dir) FKS_NOEXCEPT;						///< 最後に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (wchar_t*) 	fks_pathDelLastSep(wchar_t dir[]) FKS_NOEXCEPT;										///< 文字列の最後に \ か / があれば削除.
FKS_LIB_DECL (wchar_t*) 	fks_pathAddSep(wchar_t dst[], FKS_PATH_SIZE sz) FKS_NOEXCEPT;						///< 文字列の最後に \ / がなければ追加.

FKS_LIB_DECL (wchar_t*) 	fks_pathToUpper(wchar_t filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strupr.
FKS_LIB_DECL (wchar_t*) 	fks_pathToLower(wchar_t filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strlwr.
FKS_LIB_DECL (wchar_t*) 	fks_pathBackslashToSlash(wchar_t filePath[]) FKS_NOEXCEPT; 							///< filePath中の \ を / に置換.
FKS_LIB_DECL (wchar_t*) 	fks_pathSlashToBackslash(wchar_t filePath[]) FKS_NOEXCEPT; 							///< filePath中の / を \ に置換.

FKS_LIB_DECL (wchar_t*) 	fks_pathFullpath  (wchar_t fullpath[], FKS_PATH_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;		///< フルパス生成. os依存.
FKS_LIB_DECL (wchar_t*) 	fks_pathFullpathSL(wchar_t fullpath[], FKS_PATH_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;		///< フルパス生成. / 区切.
FKS_LIB_DECL (wchar_t*) 	fks_pathFullpathBS(wchar_t fullpath[], FKS_PATH_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;		///< フルパス生成. \ 区切.
FKS_LIB_DECL (wchar_t*) 	fks_pathRelativePath  (wchar_t relPath[], FKS_PATH_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< 相対パス生成. os依存.
FKS_LIB_DECL (wchar_t*) 	fks_pathRelativePathSL(wchar_t relPath[], FKS_PATH_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< 相対パス生成. / 区切.
FKS_LIB_DECL (wchar_t*) 	fks_pathRelativePathBS(wchar_t relPath[], FKS_PATH_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< 相対パス生成. \ 区切.

FKS_LIB_DECL (int)			fks_pathCmp(const wchar_t* l, const wchar_t* r) FKS_NOEXCEPT; 							///< ファイル名の大小比較.
FKS_LIB_DECL (int)			fks_pathNCmp(const wchar_t* l, const wchar_t* r, FKS_PATH_SIZE n) FKS_NOEXCEPT;			///< ファイル名のn文字大小比較.
FKS_LIB_DECL (int)			fks_pathDigitCmp(const wchar_t* l, const wchar_t* r) FKS_NOEXCEPT;						///< 桁違いの数字を数値として大小比較.
//FKS_LIB_DECL (int) 		fks_pathNDigitCmp(const wchar_t* l,const wchar_t* r,FKS_PATH_SIZE n) FKS_NOEXCEPT;		///< 桁違いの数字を数値としてn文字大小比較.
FKS_LIB_DECL (wchar_t*) 	fks_pathStartsWith(FKS_C_CONST wchar_t* fname, const wchar_t* prefix) FKS_NOEXCEPT;		///< fnameがprefixで始まっているか否か.
FKS_LIB_DECL (int)			fks_pathMatchWildCard(const wchar_t* pattern, const wchar_t* str) FKS_NOEXCEPT;			///< ワイルドカード文字(*?)列比較. マッチしたら真.

/// コマンドライン引数や、;区切りの複数のパス指定から、１要素取得.
FKS_LIB_DECL (wchar_t*) 	fks_pathScanArgStr(wchar_t arg[],FKS_PATH_SIZE sz,const wchar_t *str, unsigned sepChr) FKS_NOEXCEPT;
#endif

#if defined __cplusplus
FKS_INL_LIB_DECL (const wchar_t*)	fks_pathBaseName(const wchar_t *p) FKS_NOEXCEPT										{ return fks_pathBaseName((wchar_t*)p); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_pathExt(const wchar_t *name) FKS_NOEXCEPT										{ return fks_pathExt((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_pathSkipDrive(const wchar_t *name) FKS_NOEXCEPT									{ return fks_pathSkipDrive((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_pathSkipDriveRoot(const wchar_t *name) FKS_NOEXCEPT								{ return fks_pathSkipDriveRoot((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_pathCheckPosSep(const wchar_t* dir, int pos) FKS_NOEXCEPT						{ return fks_pathCheckPosSep((wchar_t*)dir,pos); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_pathCheckLastSep(const wchar_t* dir) FKS_NOEXCEPT								{ return fks_pathCheckLastSep((wchar_t*)dir); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_pathStartsWith(const wchar_t* fname, const wchar_t* prefix) FKS_NOEXCEPT		{ return fks_pathStartsWith((wchar_t*)fname, prefix); }
FKS_INL_LIB_DECL (wchar_t*) 		fks_pathSetExt(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t *ext) FKS_NOEXCEPT	{ return fks_pathSetExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (wchar_t*) 		fks_pathSetDefaultExt(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t *ext) FKS_NOEXCEPT { return fks_pathSetDefaultExt(dst, sz, dst, ext); }
FKS_INL_LIB_DECL (wchar_t*) 		fks_pathJoin(wchar_t dst[], FKS_PATH_SIZE sz, const wchar_t *nm) FKS_NOEXCEPT		{ return fks_pathJoin(dst, sz, dst, nm); }
FKS_INL_LIB_DECL (wchar_t*)			fks_pathScanArgStr(wchar_t arg[],FKS_PATH_SIZE sz,const wchar_t *str) FKS_NOEXCEPT	{ return fks_pathScanArgStr(arg,sz,str, ' ');	}
#endif

#endif		// FKS_PATH_H_INCLUDED.
