/**
 *  @file   fks_argv.c
 *  @brief  argc,argvの拡張処理(ワイルドカード,レスポンスファイル).
 *  @author Masashi KITAMURA
 *  @date   2006-2018
 *  @note
 *  -	main(int argc,char* argv[]) のargc,argvに対し、
 *  	ワイルドカード指定やレスポンスファイル指定等を展開したargc,argvに変換.
 *  	main()の初っ端、fks_ioMbsInit()したあとくらいに
 *			fks_argv_conv(&argc, &argv);
 *  	のように呼び出す. あるいは WinMain() では,
 *			fks_argv_forWinMain(cmdl, &argc, &argv);
 *
 *  -	メインはdos/win系(のコマンドラインツール)を想定.
 *  	一応 linux gccでのコンパイル可.
 *  	(unix系だとワイルドカードはシェル任せだろうで、メリット少なく)
 *
 *  -	fks_argv_config.h は設定ファイル. アプリごとに書き換えるの想定.
 *  -	設定できる要素は、
 *  	    - ワイルドカード (on/off)
 *  	    - ワイルドカード時の再帰指定(**)の有無 (on/off)
 *  	    - @レスポンスファイル (on/off)
 *  	    - .exe連動 .cfg ファイル 読込 (on/off)
 *  	    - オプション環境変数名の利用
 *
 *  -	引数文字列の先頭が'-'ならばオプションだろうで、その文字列中に
 *  	ワイルドカード文字があっても展開しない.
 */

#include <fks_common.h>
#include <fks_path.h>
#include <fks_io.h>
#include <fks_dirent.h>
#include <fks_argv.h>
#include <fks_argv_config.h>
#include <fks_misc.h>
#include <fks_assert_ex.h>
#include <fks_malloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef FKS_WIN32
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)	    	    // MSのお馬鹿なセキュリティ関数使えを無視.
#endif

/// 定義するとこの名前の環境変数の中身をコマンドライン文字列として使えるようにする.
//#define FKS_ARGV_ENVNAME    "your_app_env_name"

#ifndef FKS_ARGV_USE_WC
#define FKS_ARGV_USE_WC	      1	    	///< ワイルドカード指定があればファイル名に展開する.
#endif

#ifndef FKS_ARGV_USE_WC_REC
#define FKS_ARGV_USE_WC_REC   1	    	///< FKS_ARGV_USE_WC時に、**があれば再帰ワイルドカードにする.
#endif

#ifndef FKS_ARGV_USE_RESFILE
#define FKS_ARGV_USE_RESFILE  0	    	///< @レスポンスファイルを有効にする.
#endif

#ifndef FKS_ARGV_USE_RESFILE_AUTO_CHARENC
#if FKS_ARGV_USE_RESFILE && defined FKS_WIN32
#define	FKS_ARGV_USE_RESFILE_AUTO_CHARENC	1	///< レスポンスファイルの文字エンコードがUTF-8かSJISか自動判別する/しない (use fks_mbc.c)
#else
#define	FKS_ARGV_USE_RESFILE_AUTO_CHARENC	0
#endif
#endif

#ifndef FKS_ARGV_USE_CONFIG
#define FKS_ARGV_USE_CONFIG   0	    	///< .exeを.cfgに置換したパス名から読込.
#endif

#ifndef FKS_ARGV_CONFIG_EXT
#define FKS_ARGV_CONFIG_EXT   ".cfg"  	///< コンフィグファイル入力有の時の拡張子. 拡張子は4文字以内のこと.
#endif

#if 0 //ndef FKS_ARGV_USE_FULLPATH_ARGV0
#define FKS_ARGV_USE_FULLPATH_ARGV0   1	///< argv[0] の実行ファイル名をフルパスにする/しない. win環境のみ.
#endif

//#define FKS_ARGV_TOSLASH    	    	///< 定義すれば、filePath中の \ を / に置換.
//#define FKS_ARGV_TOBACKSLASH	    	///< 定義すれば、filePath中の / を \ に置換.
//#define FKS_ARGV_USE_SLASH_OPT	    	///< 定義すれば、/ もオプション開始文字とみなす.

#if FKS_ARGV_USE_RESFILE_AUTO_CHARENC && defined FKS_WIN32
#include <fks_mbc.h>
#include <fks_io_mbs.h>
#endif


#define T(s)			s


#ifdef __cplusplus
extern "C" {
#endif

// ===========================================================================

enum { FILEPATH_SZ = (FKS_PATH_MAX*2 > 4096) ? FKS_PATH_MAX*2 : 4096 };
enum { FKS_ARGV_VECTOR_CAPA_BASE	= 4096 };

#if FKS_ARGV_USE_WC
static unsigned char	    s_wildMode;     	///< ワイルドカード文字列が設定されていたらon.
#endif

#if (FKS_ARGV_USE_WC || FKS_ARGV_USE_RESFILE) && !FKS_ARGV_USE_CONFIG && !defined(FKS_ARGV_ENVNAME) \
    	&& !defined(FKS_WIN32_GUI) && !defined FKS_ARGV_TOSLASH && !defined FKS_ARGV_TOBACKSLASH
    #define FKS_ARGV_USE_CHK_CHR
#endif


// ===========================================================================

typedef struct fks_argv_Vector {
    char**	    buf;
    size_t		size;
    size_t		capa;
} fks_argv_Vector;

static fks_argv_Vector *fks_argv_Vector_create(size_t size);
static void 	    fks_argv_Vector_push(fks_argv_Vector* pVec, char const* pStr);
static void 	    fks_argv_VectorToArgv(fks_argv_Vector** pVec, int* pArgc, char*** pppArgv);
static void*	    fks_argv_alloc(size_t size);
static char*	    fks_argv_strdup(char const* s);
#define				fks_argv_free(p)	free(p)

#if FKS_ARGV_USE_WC
static size_t  	    fks_argv_Vector_findFname(fks_argv_Vector* pVec, char const* pPathName, int recFlag);
static void 	    fks_argv_wildCard(fks_argv_Vector* pVec);
#endif
#if defined FKS_WIN32_GUI
static int  	    fks_argv_forCmdLine1(char const* pCmdLine, int* pArgc, char*** pppArgv);
#endif
#if defined FKS_ARGV_USE_CHK_CHR
static unsigned     fks_argv_checkWcResfile(int argc, char** argv);
#endif
#ifdef FKS_ARGV_ENVNAME
static void 	    fks_argv_getEnv(char const* envName, fks_argv_Vector* pVec);
#endif
#if FKS_ARGV_USE_CONFIG
static void 	    fks_argv_getCfgFile(char const* exeName, fks_argv_Vector* pVec);
#endif
#if FKS_ARGV_USE_RESFILE || FKS_ARGV_USE_CONFIG
static void 	    fks_argv_getResFile(char const* fname, fks_argv_Vector* pVec, int notFoundOk);
#endif

#if (defined FKS_ARGV_TOSLASH) || (defined FKS_ARGV_TOBACKSLASH)
static void 	    fks_argv_convBackSlash(fks_argv_Vector* pVec);
#endif

#if FKS_ARGV_USE_WC
static int  	    fks_argv_fname_isWildCard(char const* s);
#endif



// ===========================================================================

#if defined FKS_WIN32_GUI

/** winアプリで、WinMain初っ端で、argc,argvを作りたいときに使うのを想定.
 */
void fks_argv_forWinMain(char const* pCmdLine, int* pArgc, char*** pppArgv)
{
    fks_argv_forCmdLine1(pCmdLine, pArgc, pppArgv);
}

#endif



#if defined FKS_WIN32_GUI
/** 1行の文字列pCmdLine からargc,argvを生成. (先に環境変数や.cfgファイルを処理)
 */
static int fks_argv_forCmdLine1(char const* pCmdLine, int* pArgc, char*** pppArgv)
{
    fks_argv_Vector*  pVec;
    char  	    arg[ FILEPATH_SZ + 4 ];
    char const*   s;
    int     	    n;

    FKS_ASSERT(pArgc != 0 && pppArgv != 0);
    if (pArgc == 0 || pppArgv == 0)
    	return -1;

    pVec = fks_argv_Vector_create(64); 	    	    // 作業用のリストを用意.
    if (pVec == 0)
    	return -1;

    // 実行ファイル名を得て、それを初っ端に登録.
    n = GetModuleFileName(NULL, arg, FILEPATH_SZ);
    if (n > 0) {
    	fks_argv_Vector_push(pVec, arg);
    } else {
    	// error.
      #if defined _MSC_VER
    	fks_argv_Vector_push(pVec, _pgmptr);
      #endif
    }
    if (pVec->size == 0)
    	return -1;

    // 環境変数の取得.
  #ifdef FKS_ARGV_ENVNAME
    FKS_ASSERT(strlen(FKS_ARGV_ENVNAME) > 0);
    fks_argv_getEnv(FKS_ARGV_ENVNAME, pVec);
  #endif

    // コンフィグファイルの読込.
  #if FKS_ARGV_USE_CONFIG
    fks_argv_getCfgFile(pVec->buf[0], pVec );
  #endif

  #if FKS_ARGV_USE_WC
    s_wildMode	= 0;
  #endif

    // 1行で渡されるコマンドラインを分割.
    s = pCmdLine;
    while ( (s = fks_pathScanArgStr(arg, FILEPATH_SZ, s, ' ')) != NULL ) {
    	char const* p = arg;
      #if FKS_ARGV_USE_RESFILE
    	if (*p == T('@')) {
    	    fks_argv_getResFile(p+1, pVec, 0);
    	} else
      #endif
    	{
    	  #if FKS_ARGV_USE_WC
    	    s_wildMode |= fks_argv_fname_isWildCard(p);
    	  #endif
    	    fks_argv_Vector_push( pVec, p );
    	}
    }

  #if FKS_ARGV_USE_WC
    if (s_wildMode)
    	fks_argv_wildCard(pVec);	    	    	    // ワイルドカードやディレクトリ再帰してパスを取得.
  #endif
  #if (defined FKS_ARGV_TOSLASH) || (defined FKS_ARGV_TOBACKSLASH)
    fks_argv_convBackSlash(pVec);     	    	    // define設定に従って、\ と / の変換. (基本的には何もしない)
  #endif

    fks_argv_VectorToArgv( &pVec, pArgc, pppArgv );   // 作業リストを argc,argv に変換し、作業リスト自体は開放.

    return 0;
}
#endif



// ===========================================================================

#if (defined FKS_WIN32_GUI) == 0

/** argc,argv をレスポンスファイルやワイルドカード展開して、argc, argvを更新して返す.
 *  @param  pArgc   	argcのアドレス.(argvの数)
 *  @param  pppArgv 	argvのアドレス.
 */
FKS_LIB_DECL (void)
fks_argv_conv(int* pArgc, char*** pppArgv)
{
    int    	    argc;
    char**	    ppArgv;
    fks_argv_Vector*  pVec;
    int     	    i;

    FKS_ASSERT( pArgc != 0 && pppArgv != 0 );
    if (pArgc == 0 || pppArgv == 0)
    	return;

    ppArgv = *pppArgv;
    argc   = *pArgc;
    FKS_ASSERT(argc > 0 && ppArgv != 0);
    if (argc == 0 || ppArgv == 0)
    	return;

  #if defined FKS_ARGV_USE_FULLPATH_ARGV0 && defined _WIN32   	// 古いソース用に、exeのフルパスを設定.
   #if defined _MSC_VER     // vcならすでにあるのでそれを流用.
    ppArgv[0] = _pgmptr;
   #elif defined __GNUC__   // わからないのでモジュール名取得.
    {
    	static char nm[FKS_PATH_MAX];
    	if (GetModuleFileName(NULL, nm, FKS_PATH_MAX) > 0)
    	    ppArgv[0] = nm;
    }
   #endif
  #endif

    if (argc < 2)
    	return;

  #if !FKS_ARGV_USE_CONFIG && !defined(FKS_ARGV_ENVNAME) && !defined(FKS_ARGV_TOSLASH) && !defined(FKS_ARGV_TOBACKSLASH)
   #if !FKS_ARGV_USE_WC && !FKS_ARGV_USE_RESFILE
    return; 	// ほぼ変換無し...
   #else
    if (fks_argv_checkWcResfile(argc, ppArgv) == 0)   // 現状のargc,argvを弄る必要があるか?
    	return;
   #endif
  #endif

    pVec = fks_argv_Vector_create(argc+1+16);    	    // argvが増減するので、作業用のリストを用意.

    //x printf("@4 %d %p(%p)\n", argc, ppArgv, *ppArgv);
    //x printf("   %p: %p %d %d\n", pVec, pVec->buf, pVec->capa, pVec->size);

    // 実行ファイル名の取得.
    if (argc > 0)
    	fks_argv_Vector_push( pVec, ppArgv[0] );	    // Vecに登録.

    // 環境変数の取得.
  #ifdef FKS_ARGV_ENVNAME
    FKS_ASSERT(strlen(FKS_ARGV_ENVNAME) > 0);
    fks_argv_getEnv(FKS_ARGV_ENVNAME, pVec);
  #endif

    // コンフィグファイルの読込.
  #if FKS_ARGV_USE_CONFIG
    fks_argv_getCfgFile( ppArgv[0], pVec );
  #endif

    //x printf("%p %x %#x %p\n",pVec, pVec->capa, pVec->size, pVec->buf);

  #if FKS_ARGV_USE_WC
    s_wildMode	= 0;
  #endif

    // 引数の処理.
    for (i = 1; i < argc; ++i) {
    	char const* p = ppArgv[i];
      #if FKS_ARGV_USE_RESFILE
    	if (i > 0 && *p == T('@')) {
    	    fks_argv_getResFile(p+1, pVec, 0);	    // レスポンスファイル読込.
    	} else
      #endif
    	{
    	  #if FKS_ARGV_USE_WC
    	    s_wildMode |= fks_argv_fname_isWildCard(p);
    	  #endif
    	    fks_argv_Vector_push( pVec, p );  	    // Vecに登録.
    	}
    }

  #if FKS_ARGV_USE_WC
    if (s_wildMode)
    	fks_argv_wildCard(pVec);	    	    	    // ワイルドカードやディレクトリ再帰してパスを取得.
  #endif
  #if (defined FKS_ARGV_TOSLASH) || (defined FKS_ARGV_TOBACKSLASH)
    fks_argv_convBackSlash(pVec);     	    	    // define設定に従って、\ と / の変換. (基本的には何もしない)
  #endif

    fks_argv_VectorToArgv( &pVec, pArgc, pppArgv );   // 作業リストを argc,argv に変換し、作業リスト自体は開放.
}

#endif



// ===========================================================================

static FKS_FORCE_INLINE int fks_argv_isOpt(int c)
{
  #ifdef FKS_ARGV_USE_SLASH_OPT
    return c == T('-') || c == T('/');
  #else
    return c == T('-');
  #endif
}


#if FKS_ARGV_USE_WC

/// ワイルドカード文字が混じっているか?
static int  fks_argv_fname_isWildCard(char const* s) {
  #if defined FKS_WIN32
    unsigned	rc = 0;
    unsigned	c;
    while ((c = *s++) != 0) {
    	if (c == T('*')) {
    	    if (*s == T('*')) {
    	    	return 2;
    	    }
    	    rc = 1;
    	} else if (c == T('?')) {
    	    rc = 1;
    	}
    }
    return rc;
  #else // linux(fnmatch)
    //return strpbrk(s, "*?[]\\") != 0;
    unsigned	rc = 0;
    unsigned	bc = 0;
    unsigned	c;
    while ((c = *s++) != 0) {
    	if (bc == 0) {
    	    if (c == T('*')) {
    	    	if (*s == T('*')) {
    	    	    return 2;
    	    	}
    	    	rc = 1;
    	    } else if (c == T('?')) {
    	    	rc = 1;
    	    } else if (c == T('[')) {
    	    	rc = 1;
    	    	bc = 1;
    	    	if (*s == T(']'))
    	    	    ++s;
    	    }
    	  #if 0
    	    else if (c == T('\\') && *s) {
    	    	++s;
    	    }
    	  #endif
    	} else if (c == T(']')) {
    	    bc = 0;
    	}
    }
    return rc;
  #endif
}


/// リカーシブ指定の**があれば、*一つにする.
static void  fks_argv_fname_removeRecChr(char* d, char const* s)
{
    char  c;
    while ((c = *s++) != 0) {
    	if (c == T('*') && *s == T('*')) {
    	    ++s;
    	}
    	*d++ = c;
    }
    *d = 0;
}

#endif	// FKS_ARGV_USE_WC



#if defined FKS_ARGV_USE_CHK_CHR
/// 引数文字列配列に、レスポンスファイル指定、ワイルドカード指定、リカーシブ指定があるかチェック.
static unsigned fks_argv_checkWcResfile(int argc, char** argv)
{
    int     	i;
    unsigned	rc    = 0;

    for (i = 1; i < argc; ++i) {
    	char const* p = argv[i];
      #if FKS_ARGV_USE_RESFILE
    	if (*p == '@') {
    	    rc |= 4;	// レスポンスファイル指定.
    	} else
      #endif
    	{
    	  #if FKS_ARGV_USE_WC
    	    if (fks_argv_isOpt(*p) == 0) {
    	    	int mode = fks_argv_fname_isWildCard(p);
    	    	s_wildMode |= mode;
    	    	if (mode > 0) {
    	    	    rc |= 1;	// ワイルドカード指定.
    	    	    if (mode == 2)
    	    	    	rc |= 2;
    	    	}
    	    }
    	  #endif
    	}
    }
    return rc;
}
#endif


#ifdef FKS_ARGV_ENVNAME
/// 環境変数があれば、登録.
static void fks_argv_getEnv(char const* envName, fks_argv_Vector* pVec)
{
    char const* env;
    if (envName == 0 || envName[0] == 0)
    	return;
    env = getenv(envName);
    if (env && env[0]) {
    	char	    	arg[ FILEPATH_SZ + 4 ];
    	while ( (env = fks_pathScanArgStr(arg, FILEPATH_SZ, env, ' ')) != NULL ) {
    	    char const* p = arg;
    	  #if FKS_ARGV_USE_WC
    	    s_wildMode |= fks_argv_fname_isWildCard(p);
    	  #endif
    	    fks_argv_Vector_push( pVec, p );
    	}
    }
}
#endif


#if FKS_ARGV_USE_CONFIG
/// コンフィグファイルの読み込み.
static void fks_argv_getCfgFile(char const* exeName, fks_argv_Vector* pVec)
{
    char  name[ FILEPATH_SZ+4 ];

    // 実行ファイル名からコンフィグパス名を生成.
  #ifdef FKS_WIN32
    fks_pathCpy(p, FILEPATH_SZ-10, exeName);
  #else
    fks_pathCpy(name, sizeof name, "~/.");
    fks_pathCat(name, sizeof name, fks_pathBaseName(exeName));
  #endif

	fks_pathSetExt(name, sizeof name, FKS_ARGV_CONFIG_EXT);

    fks_argv_getResFile(name, pVec, 1);
}
#endif


#if FKS_ARGV_USE_RESFILE || FKS_ARGV_USE_CONFIG
/** レスポンスファイルを(argc,argv)を pVec に変換.
 * レスポンスファイルやワイルドカード、リカーシブ等の処理も行う.
 */
static void fks_argv_getResFile(char const* fname, fks_argv_Vector* pVec, int notFoundOk)
{
    size_t		bytes = 0;
    char		line[0x2000];
    char*		buf = (char*)fks_fileLoadMalloc(fname, &bytes);
    char*		bufEnd;
    char const*	cur;
    if (buf == NULL) {
    	if (notFoundOk)
    	    return;
    	fprintf(stderr,"Response-file '%s' is not opened.\n", fname);
    	exit(1);    // return;
    }
    if (bytes == 0) {
		fks_free(buf);
    	return;
    }

 #if FKS_ARGV_USE_RESFILE_AUTO_CHARENC && defined FKS_WIN32
	{
		fks_mbcenc_t srcEnc = fks_mbsAutoCharEncoding(buf, bytes, 0, NULL, 0);
		if (srcEnc->cp != fks_io_mbs_codepage) {
			char*	d;
			size_t	sz = 0;
			fks_mbcenc_t dstEnc = (fks_io_mbs_codepage == FKS_CP_UTF8) ? fks_mbc_utf8
								: (fks_io_mbs_codepage == FKS_CP_SJIS) ? fks_mbc_cp932
								: fks_mbc_dbc;
			d = fks_mbsConvMalloc(dstEnc, srcEnc, buf, bytes, &sz);
			fks_free(buf);
			buf = d;
			bytes = sz;
		}
	}
 #endif

	cur = buf;
	bufEnd = buf + bytes;
    while ((cur = fks_strGetLine(line, sizeof line, cur, bufEnd, 0x0f)) != NULL) {
    	char	arg[FILEPATH_SZ + 4];
    	char* s = buf;
    	while ( (s = fks_pathScanArgStr(arg, FILEPATH_SZ, s, ' ')) != NULL ) {
    	    int c = ((unsigned char*)arg)[0];
    	    if (c == ';' || c == '#' || c == '\0') {	// 空行やコメントの時.
    	    	break;
    	    }
    	    // 再帰検索指定,ワイルドカードの有無をチェック.
    	  #if FKS_ARGV_USE_WC
    	    s_wildMode |= fks_argv_fname_isWildCard(arg);
    	  #endif
    	    fks_argv_Vector_push(pVec, arg );
    	}
    }
	fks_free(buf);
}
#endif


#if FKS_ARGV_USE_WC
/** ワイルドカード、再帰処理.
 */
static void fks_argv_wildCard(fks_argv_Vector* pVec)
{
    char**	    pp;
    char**	    ee;
    fks_argv_Vector*  wk;
    int     	    mode;

    // 再構築.
    wk = fks_argv_Vector_create( pVec->size+1 );
    ee = pVec->buf + pVec->size;
    for (pp = pVec->buf; pp != ee; ++pp) {
    	char const* s = *pp;
      #if FKS_ARGV_USE_WC
    	if (   fks_argv_isOpt(*s) == 0	    	    	// オプション以外の文字列で,
    	    && (pp != pVec->buf)    	    	    	// 初っ端以外([0]は実行ファイル名なので検索させない)のときで,
    	    && ((mode = fks_argv_fname_isWildCard( s )) != 0) // ワイルドカード指定のありのとき.
    	 ){
    	    char  name[FILEPATH_SZ+4];
    	    int recFlag = (mode >> 1) & 1;
    	  #if FKS_ARGV_USE_WC_REC
    	    if (s[0] == T('*') && s[1] == T('*') && fks_pathIsSep(s[2])) {
    	    	recFlag = 1;
    	    	s += 3;
    	    } else
    	  #endif
    	    if (recFlag) {
    	    	fks_argv_fname_removeRecChr(name, s);
    	    	s = name;
    	    }
    	    fks_argv_Vector_findFname(wk, s, recFlag);

    	} else	{
    	    fks_argv_Vector_push( wk, s );
    	}
      #else
    	fks_argv_Vector_push( wk, s );
      #endif
    }

    // 元のリストを開放.
    for (pp = pVec->buf; pp != ee; ++pp) {
    	char* p = *pp;
    	if (p)
    	    fks_argv_free(p);
    }
    fks_argv_free(pVec->buf);

    // 今回生成したものを、pVecに設定.
    pVec->buf  = wk->buf;
    pVec->size = wk->size;
    pVec->capa = wk->capa;

    // 作業に使ったメモリを開放.
    fks_argv_free(wk);
}
#endif


#if (defined FKS_ARGV_TOSLASH) || (defined FKS_ARGV_TOBACKSLASH)
/** ファイル(パス)名中の \ / の変換. -で始まるオプション文字列は対象外.
 *  最近のwin環境ではどちらの指定でもokなので、無理に変換する必要なし.
 *  (オプション中にファイル名があると結局自前で変換せざるえないので、ここでやらないほうが無難かも)
 */
static void fks_argv_convBackSlash(fks_argv_Vector* pVec)
{
    char**	pp;
    char**	ee = pVec->buf + pVec->size;

    for (pp = pVec->buf; pp != ee; ++pp) {
    	char* s = *pp;
    	if (fks_argv_isOpt(*s) == 0) {	    // オプション以外の文字列で,
    	  #if (defined FKS_ARGV_TOSLASH)
    	    fks_pathBackslashToSlash(s);	    // \ を / に置換.
    	  #else
    	    fks_pathSlashToBackslash(s);	    // / を \ に置換.
    	  #endif
    	} else {    	    	    	    // オプションなら、下手に変換しないでおく.
    	    ;
    	}
    }
}
#endif


/** pVecから、(argc,argv)を生成. ppVecは開放する.
 */
static void fks_argv_VectorToArgv(fks_argv_Vector** ppVec, int* pArgc, char*** pppArgv)
{
    fks_argv_Vector*  pVec;
    char**	    		av;
    int     	    	ac;

    FKS_ASSERT( pppArgv != 0 && pArgc != 0 && ppVec != 0 );

    *pppArgv = NULL;
    *pArgc   = 0;

    pVec     = *ppVec;
    if (pVec == NULL)
    	return;

    ac	     = (int)pVec->size;
    if (ac == 0)
    	return;

    *pArgc   = ac;
    av	     = (char**) fks_argv_alloc(sizeof(char*) * (ac + 2));
    *pppArgv = av;

    memcpy(av, pVec->buf, sizeof(char*) * ac);
    av[ac]   = NULL;
    av[ac+1] = NULL;

    fks_argv_free(pVec->buf);
    fks_argv_free(pVec);
    *ppVec   = NULL;
}


// ===========================================================================

#if FKS_ARGV_USE_WC

/** srchNameで指定されたパス名(ワイルドカード文字対応) にマッチするパス名を全て pVec に入れて返す.
 *  recFlag が真なら再帰検索を行う.
 */
static size_t  fks_argv_Vector_findFname(fks_argv_Vector* pVec, char const* srchName, int recFlag)
{
	char**				ppPaths = NULL;
	Fks_DirEnt_Matchs	matchs = {0};
	fks_isize_t			n, i;
	matchs.fname = srchName;
	if (recFlag)
		matchs.flags |= FKS_DE_Recursive;
	n = fks_createDirEntPaths(&ppPaths, NULL, &matchs);
	for (i = 0; i < n; ++i) {
		fks_argv_Vector_push(pVec, ppPaths[i]);
	}
	fks_releaseDirEntPaths(ppPaths);
	return n;
}

#endif


// ===========================================================================

/** argv vector create
 */
static fks_argv_Vector* fks_argv_Vector_create(size_t size)
{
    fks_argv_Vector* pVec = (fks_argv_Vector*)fks_argv_alloc( sizeof(fks_argv_Vector) );
    size    	    	= ((size + FKS_ARGV_VECTOR_CAPA_BASE) / FKS_ARGV_VECTOR_CAPA_BASE) * FKS_ARGV_VECTOR_CAPA_BASE;
    pVec->capa	    	= size;
    pVec->size	    	= 0;
    pVec->buf	    	= (char**)fks_argv_alloc(sizeof(void*) * size);
    return pVec;
}

/** append str to argv vector
 */
static void fks_argv_Vector_push(fks_argv_Vector* pVec, char const* pStr)
{
    FKS_ARG_PTR_ASSERT(1, pVec);
    FKS_ARG_PTR_ASSERT(2, pStr);
    if (pStr && pVec) {
    	size_t	capa = pVec->capa;
    	FKS_PTR_ASSERT(pVec->buf);
    	if (pVec->size >= capa) {   // キャパを超えていたら、メモリを確保しなおす.
    	    char**	    buf;
    	    size_t		newCapa = capa + FKS_ARGV_VECTOR_CAPA_BASE;
    	    //x printf("!  %p: %p %d %d ::%s\n", pVec, pVec->buf, pVec->capa, pVec->size, pStr);
    	    FKS_ASSERT(pVec->size == capa);
    	    pVec->capa	= newCapa;
    	    buf     	= (char**)fks_argv_alloc(sizeof(void*) * pVec->capa);
    	    if (pVec->buf)
    	    	memcpy(buf, pVec->buf, capa*sizeof(void*));
    	    memset(buf+capa, 0, FKS_ARGV_VECTOR_CAPA_BASE*sizeof(void*));
    	    fks_argv_free(pVec->buf);
    	    pVec->buf	= buf;
    	}
    	FKS_ASSERT(pVec->size < pVec->capa);
    	pVec->buf[ pVec->size ] = fks_argv_strdup(pStr);
    	++ pVec->size;
    	//x printf("!!	%p: %p %d %d ::%s\n", pVec, pVec->buf, pVec->capa, pVec->size, pStr);
    }
}


// ===========================================================================

/** malloc
 */
static void* fks_argv_alloc(size_t size)
{
    void* p = calloc(1, size);
    if (p == NULL) {
    	fprintf(stderr, "not enough memory.\n");
    	exit(1);
    }
    return p;
}

/** strdup
 */
static char* fks_argv_strdup(char const* s)
{
    size_t   sz = strlen(s) + 1;
    char*  p	= (char*)malloc(sz * sizeof(char));
    if (p == NULL) {
    	fprintf(stderr, "not enough memory.\n");
    	exit(1);
    }
    return (char*) memcpy(p, s, sz*sizeof(char));
}


#ifdef __cplusplus
}
#endif
