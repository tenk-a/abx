/**
 *	@file	fname.h
 *	@brief	file-name functions
 *	@author Masashi Kitamura (tenka@6809.net)
 */
#ifndef FKS_FNAME_H_INCLUDED
#define FKS_FNAME_H_INCLUDED

#include <fks/fks_config.h>
#include <stddef.h>
#include <string.h>

#ifndef FKS_NOEXCEPT
 #ifdef __cplusplus
  #define FKS_NOEXCEPT		throw()	//noexcept
 #else
  #define FKS_NOEXCEPT
 #endif
#endif
#ifndef FKS_LIB_DECL
 #define FKS_LIB_DECL(t)		t
#endif
#ifndef FKS_INL_LIB_DECL
 #define FKS_INL_LIB_DECL(t)	static inline t
#endif


// ============================================================================
// マクロ関係.

// Win以外(unix系)で ファイル名中の全角の \ 対策をしたい場合は定義.
// これを定義すると環境変数 LANG をみて SJIS,gbk,gb2312,big5ならMBC処理をする.
//#define FKS_USE_FNAME_MBC

#if defined _WIN32 || defined _DOS || defined _MSDOS
#define FKS_FNAME_WINDOS
#endif

// ファイル名のサイズ. 利用側のバッファ準備用.
// 元々はパス全体の制限だったが現状 1ファイル名の長さになっている感じ.
// ※ win-api 自体は基本的にパス全体でこの制限を受ける.
// fname.cpp では、fks_fnameRelativePath?? のみがこの値を使う. (他は参照しない)
#ifndef FKS_FNAME_MAX_PATH
 #ifdef _WIN32
  #define FKS_FNAME_MAX_PATH	260/*_MAX_PATH*/		///< 通常のパス名の長さ.※winnt系の場合1ファイル名の長さ.
 #else
  #define FKS_FNAME_MAX_PATH	1024					///< 通常のパス名の長さ.
 #endif
#endif

// url向け?だが場合によってはこれがパスの最大長. winの場合 最長約0x8000 . それ以外は適当.
// (※ winでは特殊な指定をしないと_PAX_PATHを超えては使えない)
#ifndef FKS_FNAME_MAX_URL
 #ifdef _WIN32
  #define FKS_FNAME_MAX_URL 	(0x8000)					///< urlとして扱う場合のパス名サイズ.
 #else	// 適当に計算.
  #define FKS_FNAME_MAX_URL 	(6U*4*FKS_FNAME_MAX_PATH)	///< urlとして扱う場合のパス名サイズ.
 #endif
#endif

#if defined FKS_FNAME_WINDOS
#define FKS_FNAME_SEP_CHR		'\\'
#define FKS_FNAME_SEP_STR		"\\"
#define FKS_FNAME_SEP_WCS		L"\\"
#define FKS_FNAME_SEP_TCS		_T("\\")
#else
#define FKS_FNAME_SEP_CHR		'/'
#define FKS_FNAME_SEP_STR		"/"
#define FKS_FNAME_SEP_WCS		L"/"
#define FKS_FNAME_SEP_TCS		"/"
#endif

#ifndef FKS_C_CONST
#ifdef __cplusplus
#define FKS_C_CONST 			// c++の場合は 基本は非constで、const,非const２種類作る.
#else	// c のとき.
#define FKS_C_CONST 	const	// Cの場合で 引数はconst, 返り値は非const にする場合に使う.
#endif
#endif


#ifndef FKS_FNAME_SIZE
typedef size_t			FKS_FNAME_SIZE;
#define FKS_FNAME_SIZE	FKS_FNAME_SIZE
#endif

#if defined FKS_FNAME_WINDOS
FKS_INL_LIB_DECL (int)		fks_fnameIsSep(unsigned c) FKS_NOEXCEPT { return c == '\\' || c == '/'; }
#else
FKS_INL_LIB_DECL (int)		fks_fnameIsSep(unsigned c) FKS_NOEXCEPT { return c == '/'; }
#endif

// ============================================================================
// char version

#if !(defined FKS_FNAME_WCS_COMPILE)

#ifdef __cplusplus
extern "C" {
#endif

FKS_INL_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameLen(const char* path) FKS_NOEXCEPT { return (FKS_FNAME_SIZE)strlen(path); }

FKS_LIB_DECL (int)		fks_fnameIsAbs(const char* path) FKS_NOEXCEPT;									///< 絶対パスか否か(ドライブ名の有無は関係なし)
FKS_LIB_DECL (int)		fks_fnameHasDrive(const char* path) FKS_NOEXCEPT;								///< ドライブ名がついているか. (file: や http:もドライブ扱い)

FKS_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameAdjustSize(const char* str, FKS_FNAME_SIZE size) FKS_NOEXCEPT; ///< (なるべく文字を壊さないで)size文字以内の文字数を返す.
FKS_LIB_DECL (char*) 	fks_fnameCpy(char dst[], FKS_FNAME_SIZE sz, const char* src) FKS_NOEXCEPT; 		///< ファイル名のコピー.
FKS_LIB_DECL (char*) 	fks_fnameCat(char dst[], FKS_FNAME_SIZE sz, const char* src) FKS_NOEXCEPT; 		///< ファイル名文字列の連結.

FKS_LIB_DECL (char*) 	fks_fnameBaseName(FKS_C_CONST char *adr) FKS_NOEXCEPT;							///< ファイルパス名中のディレクトリを除いたファイル名の位置を返す.
FKS_LIB_DECL (char*) 	fks_fnameExt(FKS_C_CONST char *name) FKS_NOEXCEPT;								///< 拡張子の位置を返す.
FKS_LIB_DECL (char*) 	fks_fnameSkipDrive(FKS_C_CONST char *name) FKS_NOEXCEPT; 						///< ドライブ名をスキップした位置を返す.
FKS_LIB_DECL (char*) 	fks_fnameSkipDriveRoot(FKS_C_CONST char* name) FKS_NOEXCEPT; 					///< ドライブ名とルート指定部分をスキップした位置を返す.

FKS_LIB_DECL (char*) 	fks_fnameDelExt(char name[]) FKS_NOEXCEPT;											///< 拡張子を削除する.
FKS_LIB_DECL (char*) 	fks_fnameGetNoExt(char dst[], FKS_FNAME_SIZE sz, const char *src) FKS_NOEXCEPT;		///< 拡張子を外した名前を取得.
FKS_LIB_DECL (char*) 	fks_fnameGetBaseNameNoExt(char d[], FKS_FNAME_SIZE l, const char *s) FKS_NOEXCEPT;	///< ディレクトリと拡張子を外した名前を取得.

FKS_LIB_DECL (char*) 	fks_fnameSetExt(char dst[], FKS_FNAME_SIZE sz, const char* src, const char *ext) FKS_NOEXCEPT; 			///< 拡張子を、ext に変更する.
FKS_LIB_DECL (char*) 	fks_fnameSetDefaultExt(char dst[], FKS_FNAME_SIZE sz, const char* src, const char *ext) FKS_NOEXCEPT;	///< 拡張子がなければ、ext を追加する.
FKS_LIB_DECL (char*) 	fks_fnameJoin(char dst[],FKS_FNAME_SIZE sz,const char *dir,const char *nm) FKS_NOEXCEPT;				///< ディレクトリ名とファイル名の連結.

FKS_LIB_DECL (char*) 	fks_fnameGetDir(char dir[], FKS_FNAME_SIZE sz, const char *nm) FKS_NOEXCEPT;	///< ディレクトリ名の取得.
FKS_LIB_DECL (char*) 	fks_fnameGetDrive(char drv[], FKS_FNAME_SIZE sz, const char *nm) FKS_NOEXCEPT; 	///< ドライブ名を取得.
FKS_LIB_DECL (char*) 	fks_fnameGetDriveRoot(char dr[],FKS_FNAME_SIZE sz,const char *nm) FKS_NOEXCEPT;	///< ドライブ名を取得.

FKS_LIB_DECL (char*) 	fks_fnameCheckPosSep(FKS_C_CONST char* dir, int pos) FKS_NOEXCEPT;				///< posの位置に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (char*) 	fks_fnameCheckLastSep(FKS_C_CONST char* dir) FKS_NOEXCEPT;						///< 最後に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (char*) 	fks_fnameDelLastSep(char dir[]) FKS_NOEXCEPT;									///< 文字列の最後に \ か / があれば削除.
FKS_LIB_DECL (char*) 	fks_fnameAddSep(char dst[], FKS_FNAME_SIZE sz) FKS_NOEXCEPT;					///< 文字列の最後に \ / がなければ追加.

FKS_LIB_DECL (char*) 	fks_fnameToUpper(char filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strupr.
FKS_LIB_DECL (char*) 	fks_fnameToLower(char filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strlwr.
//FKS_LIB_DECL (char*) 	fks_fnameToUpperN(char filename[], size_t n) FKS_NOEXCEPT;						///< 全角２バイト目を考慮した strupr.
//FKS_LIB_DECL (char*) 	fks_fnameToLowerN(char filename[], size_t n) FKS_NOEXCEPT;						///< 全角２バイト目を考慮した strlwr.

FKS_LIB_DECL (char*) 	fks_fnameBackslashToSlash(char filePath[]) FKS_NOEXCEPT; 						///< filePath中の \ を / に置換.
FKS_LIB_DECL (char*) 	fks_fnameSlashToBackslash(char filePath[]) FKS_NOEXCEPT; 						///< filePath中の / を \ に置換.

FKS_LIB_DECL (char*) 	fks_fnameFullpath  (char fullpath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< フルパス生成. os依存.
FKS_LIB_DECL (char*) 	fks_fnameFullpathSL(char fullpath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< フルパス生成. / 区切.
FKS_LIB_DECL (char*) 	fks_fnameFullpathBS(char fullpath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< フルパス生成. \ 区切.
FKS_LIB_DECL (char*) 	fks_fnameRelativePath  (char relPath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< 相対パス生成. os依存.
FKS_LIB_DECL (char*) 	fks_fnameRelativePathSL(char relPath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< 相対パス生成. / 区切.
FKS_LIB_DECL (char*) 	fks_fnameRelativePathBS(char relPath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< 相対パス生成. \ 区切.

FKS_LIB_DECL (int)		fks_fnameCmp(const char* l, const char* r) FKS_NOEXCEPT; 						///< ファイル名の大小比較.
FKS_LIB_DECL (int)		fks_fnameNCmp(const char* l, const char* r, FKS_FNAME_SIZE n) FKS_NOEXCEPT;		///< ファイル名のn文字大小比較.
FKS_LIB_DECL (int)		fks_fnameDigitCmp(const char* l, const char* r) FKS_NOEXCEPT;					///< 桁違いの数字を数値として大小比較.
//FKS_LIB_DECL (int) 	fks_fnameNDigitCmp(const char* l,const char* r,FKS_FNAME_SIZE n) FKS_NOEXCEPT;	///< 桁違いの数字を数値としてn文字大小比較.
FKS_LIB_DECL (char*) 	fks_fnameEquLong(FKS_C_CONST char* fname, const char* baseName) FKS_NOEXCEPT;	///< fnameがbaseNameで始まっているか否か.
FKS_LIB_DECL (int)		fks_fnameMatchWildCard(const char* pattern, const char* str) FKS_NOEXCEPT;		///< ワイルドカード文字(*?)列比較. マッチしたら真.

/// コマンドライン引数や、;区切りの複数のパス指定から、１要素取得.
FKS_LIB_DECL (char*) 	fks_fnameScanArgStr(char arg[],FKS_FNAME_SIZE sz,const char *str, unsigned sepChr) FKS_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
FKS_INL_LIB_DECL (const char*)	fks_fnameBaseName(const char *p) FKS_NOEXCEPT							{ return fks_fnameBaseName((char*)p); }
FKS_INL_LIB_DECL (const char*)	fks_fnameExt(const char *name) FKS_NOEXCEPT								{ return fks_fnameExt((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_fnameSkipDrive(const char *name) FKS_NOEXCEPT						{ return fks_fnameSkipDrive((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_fnameSkipDriveRoot(const char *name) FKS_NOEXCEPT					{ return fks_fnameSkipDriveRoot((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_fnameCheckPosSep(const char* dir, int pos) FKS_NOEXCEPT				{ return fks_fnameCheckPosSep((char*)dir,pos); }
FKS_INL_LIB_DECL (const char*)	fks_fnameCheckLastSep(const char* dir) FKS_NOEXCEPT						{ return fks_fnameCheckLastSep((char*)dir); }
FKS_INL_LIB_DECL (const char*)	fks_fnameEquLong(const char* fname, const char* baseName) FKS_NOEXCEPT	{ return fks_fnameEquLong((char*)fname, baseName); }
FKS_INL_LIB_DECL (char*)		fks_fnameScanArgStr(char arg[],FKS_FNAME_SIZE sz,const char *str) FKS_NOEXCEPT { return fks_fnameScanArgStr(arg,sz,str, ' ');	}
#endif

#endif


// ============================================================================
// wchar_t version
#if defined __cplusplus || defined FKS_FNAME_WCS_COMPILE

FKS_INL_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameLen(const wchar_t* path) FKS_NOEXCEPT { return (FKS_FNAME_SIZE)wcslen(path); }

FKS_LIB_DECL (int)			fks_fnameIsAbs(const wchar_t* path) FKS_NOEXCEPT;									///< 絶対パスか否か(ドライブ名の有無は関係なし)
FKS_LIB_DECL (int)			fks_fnameHasDrive(const wchar_t* path) FKS_NOEXCEPT;								///< ドライブ名がついているか. (file: や http:もドライブ扱い)

FKS_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameAdjustSize(const wchar_t* str, FKS_FNAME_SIZE size) FKS_NOEXCEPT; 		///< (なるべく文字を壊さないで)size文字以内の文字数を返す.
FKS_LIB_DECL (wchar_t*) 	fks_fnameCpy(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src) FKS_NOEXCEPT; 	///< ファイル名のコピー.
FKS_LIB_DECL (wchar_t*) 	fks_fnameCat(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src) FKS_NOEXCEPT; 	///< ファイル名文字列の連結.

FKS_LIB_DECL (wchar_t*) 	fks_fnameBaseName(FKS_C_CONST wchar_t *adr) FKS_NOEXCEPT;							///< ファイルパス名中のディレクトリを除いたファイル名の位置を返す.
FKS_LIB_DECL (wchar_t*) 	fks_fnameExt(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT;								///< 拡張子の位置を返す.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSkipDrive(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT; 						///< ドライブ名をスキップした位置を返す.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSkipDriveRoot(FKS_C_CONST wchar_t* name) FKS_NOEXCEPT; 					///< ドライブ名とルート指定部分をスキップした位置を返す.

FKS_LIB_DECL (wchar_t*) 	fks_fnameDelExt(wchar_t name[]) FKS_NOEXCEPT;												///< 拡張子を削除する.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetNoExt(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t *src) FKS_NOEXCEPT;		///< 拡張子を外した名前を取得.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetBaseNameNoExt(wchar_t d[], FKS_FNAME_SIZE l, const wchar_t *s) FKS_NOEXCEPT;	///< ディレクトリと拡張子を外した名前を取得.

FKS_LIB_DECL (wchar_t*) 	fks_fnameSetExt(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src, const wchar_t *ext) FKS_NOEXCEPT; 		///< 拡張子を、ext に変更する.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSetDefaultExt(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src, const wchar_t *ext) FKS_NOEXCEPT;	///< 拡張子がなければ、ext を追加する.
FKS_LIB_DECL (wchar_t*) 	fks_fnameJoin(wchar_t dst[],FKS_FNAME_SIZE sz,const wchar_t *dir,const wchar_t *nm) FKS_NOEXCEPT;				///< ディレクトリ名とファイル名の連結.

FKS_LIB_DECL (wchar_t*) 	fks_fnameGetDir(wchar_t dir[], FKS_FNAME_SIZE sz, const wchar_t *nm) FKS_NOEXCEPT;		///< ディレクトリ名の取得.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetDrive(wchar_t drv[], FKS_FNAME_SIZE sz, const wchar_t *nm) FKS_NOEXCEPT; 	///< ドライブ名を取得.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetDriveRoot(wchar_t dr[],FKS_FNAME_SIZE sz,const wchar_t *nm) FKS_NOEXCEPT;	///< ドライブ名を取得.

FKS_LIB_DECL (wchar_t*) 	fks_fnameCheckPosSep(FKS_C_CONST wchar_t* dir, int pos) FKS_NOEXCEPT;				///< posの位置に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (wchar_t*) 	fks_fnameCheckLastSep(FKS_C_CONST wchar_t* dir) FKS_NOEXCEPT;						///< 最後に\か/があればそのアドレスをなければNULLを返す.
FKS_LIB_DECL (wchar_t*) 	fks_fnameDelLastSep(wchar_t dir[]) FKS_NOEXCEPT;									///< 文字列の最後に \ か / があれば削除.
FKS_LIB_DECL (wchar_t*) 	fks_fnameAddSep(wchar_t dst[], FKS_FNAME_SIZE sz) FKS_NOEXCEPT;						///< 文字列の最後に \ / がなければ追加.

FKS_LIB_DECL (wchar_t*) 	fks_fnameToUpper(wchar_t filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strupr.
FKS_LIB_DECL (wchar_t*) 	fks_fnameToLower(wchar_t filename[]) FKS_NOEXCEPT;									///< 全角２バイト目を考慮した strlwr.
FKS_LIB_DECL (wchar_t*) 	fks_fnameBackslashToSlash(wchar_t filePath[]) FKS_NOEXCEPT; 						///< filePath中の \ を / に置換.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSlashToBackslash(wchar_t filePath[]) FKS_NOEXCEPT; 						///< filePath中の / を \ に置換.

FKS_LIB_DECL (wchar_t*) 	fks_fnameFullpath  (wchar_t fullpath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< フルパス生成. os依存.
FKS_LIB_DECL (wchar_t*) 	fks_fnameFullpathSL(wchar_t fullpath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< フルパス生成. / 区切.
FKS_LIB_DECL (wchar_t*) 	fks_fnameFullpathBS(wchar_t fullpath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< フルパス生成. \ 区切.
FKS_LIB_DECL (wchar_t*) 	fks_fnameRelativePath  (wchar_t relPath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< 相対パス生成. os依存.
FKS_LIB_DECL (wchar_t*) 	fks_fnameRelativePathSL(wchar_t relPath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< 相対パス生成. / 区切.
FKS_LIB_DECL (wchar_t*) 	fks_fnameRelativePathBS(wchar_t relPath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< 相対パス生成. \ 区切.

FKS_LIB_DECL (int)			fks_fnameCmp(const wchar_t* l, const wchar_t* r) FKS_NOEXCEPT; 							///< ファイル名の大小比較.
FKS_LIB_DECL (int)			fks_fnameNCmp(const wchar_t* l, const wchar_t* r, FKS_FNAME_SIZE n) FKS_NOEXCEPT;		///< ファイル名のn文字大小比較.
FKS_LIB_DECL (int)			fks_fnameDigitCmp(const wchar_t* l, const wchar_t* r) FKS_NOEXCEPT;						///< 桁違いの数字を数値として大小比較.
//FKS_LIB_DECL (int) 		fks_fnameNDigitCmp(const wchar_t* l,const wchar_t* r,FKS_FNAME_SIZE n) FKS_NOEXCEPT;	///< 桁違いの数字を数値としてn文字大小比較.
FKS_LIB_DECL (wchar_t*) 	fks_fnameEquLong(FKS_C_CONST wchar_t* fname, const wchar_t* baseName) FKS_NOEXCEPT;		///< fnameがbaseNameで始まっているか否か.
FKS_LIB_DECL (int)			fks_fnameMatchWildCard(const wchar_t* pattern, const wchar_t* str) FKS_NOEXCEPT;		///< ワイルドカード文字(*?)列比較. マッチしたら真.

/// コマンドライン引数や、;区切りの複数のパス指定から、１要素取得.
FKS_LIB_DECL (wchar_t*) 	fks_fnameScanArgStr(wchar_t arg[],FKS_FNAME_SIZE sz,const wchar_t *str, unsigned sepChr) FKS_NOEXCEPT;
#endif

#if defined __cplusplus
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameBaseName(const wchar_t *p) FKS_NOEXCEPT							{ return fks_fnameBaseName((wchar_t*)p); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameExt(const wchar_t *name) FKS_NOEXCEPT								{ return fks_fnameExt((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameSkipDrive(const wchar_t *name) FKS_NOEXCEPT						{ return fks_fnameSkipDrive((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameSkipDriveRoot(const wchar_t *name) FKS_NOEXCEPT					{ return fks_fnameSkipDriveRoot((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameCheckPosSep(const wchar_t* dir, int pos) FKS_NOEXCEPT				{ return fks_fnameCheckPosSep((wchar_t*)dir,pos); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameCheckLastSep(const wchar_t* dir) FKS_NOEXCEPT						{ return fks_fnameCheckLastSep((wchar_t*)dir); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameEquLong(const wchar_t* fname, const wchar_t* baseName) FKS_NOEXCEPT	{ return fks_fnameEquLong((wchar_t*)fname, baseName); }
FKS_INL_LIB_DECL (wchar_t*)			fks_fnameScanArgStr(wchar_t arg[],FKS_FNAME_SIZE sz,const wchar_t *str) FKS_NOEXCEPT { return fks_fnameScanArgStr(arg,sz,str, ' ');	}
#endif

#endif		// FKS_FNAME_H_INCLUDED.
