/**
 *	@file	abx_usage.cpp
 *	@brief	usage for abx
 *	@author Masashi KITAMURA (tenka@6809.net)
 *	@license	Boost Software License Version 1.0
 */

#include <stddef.h>
#include <fks_common.h>
#include <stdio.h>
#include <subr.hpp>
#include <string>
#include "abx_usage.h"

#ifdef _WIN32
#include <windows.h>
#endif

#if 1
#define APP_HELP_TITLE_JP	"abx v3.94(pre v4) ファイルを検索,ファイル名を文字列に埋込(バッチ生成)\n"	\
							"  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"
#endif
#define APP_HELP_TITLE		"abx v3.94(pre v4) Search file, embed file name in text(gen. bat)\n"	\
							"  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"

#define APP_HELP_CMDLINE	"usage : %s [option]  file(s) 'text conversion' file(s)\n"	\
							"        %s [option]  file(s) =text conversion\n"


static char const* s_helpJp =
		"Option:\n"
		" -x[-]      バッチ実行 -x-しない\n"
	 #ifdef ENABLE_MT_X
		" -xm[N]     Nスレッド実行.0自動\n"
	 #endif
		" -r[-]      ディレクトリ再帰\n"
		" -a[nrhsd]  指定ファイル属性で検索\n"
		"            n:一般 s:システム h:隠し\n"
		"            r:読込専用 d:ディレクトリ\n"
		" -z[N-M]    サイズ N～MのFILEを検索\n"
		" -d[A-B]    日付A～BのFILEを検索\n"
		" -s[neztam] ソート(整列)\n"
		"   [r]      n:名 m:名(数) e:拡張子 z:サイズ\n"
		"            t:日付 a:属性 r:降順\n"
		" -n[-]      ファイル検索しない -n-有 \n"
		// " -u[-]      $c|$Cでファイル名大小文字\n"
		" -y         $cxfgdpwに\"付加\n"
		" -l[-]      @入力で名前は行単位\n"
		" -ci[N:M]   N:$iの開始番号(M:終)\n"
		" +CFGFILE   .CFGファイル指定\n"
		" @RESFILE   レスポンスファイル\n"
		" :変換名    CFGで定義した変換\n"
		" :          変換名一覧を表示\n"
		" -e<EXT>    デフォルト拡張子\n"
		" -o<FILE>   出力ファイル指定\n"
		" -i<DIR>    検索ディレクトリ\n"
		" -cd<DIR>   相対パス生成時のカレントディレクトリ\n"
		" -w<DIR>    TMPディレクトリ\n"
		" -p<DIR>    $pの強制変更\n"
		" -ct<FILE>  FILEより新なら\n"
		" -ck[-]     ascii以外の文字のあるファイル名を検索\n"
	 #ifdef USE_PATH_DBC
		" -cy[-]     \\を含む全角名検索\n"
	 #endif
		" -t[N]      最初のN個のみ処理\n"
		" -? -h      ヘルプ\n"
		"\n"
		"変換文字:                    変換例:\n"
		" $f フルパス(拡張子付)       d:\\dir\\dir2\\filename.ext\n"
		" $g フルパス(拡張子無)       d:\\dir\\dir2\\filename\n"
		" $v ドライブ                 d\n"
		" $p ディレクトリ(ドライブ付) d:\\dir\\dir2\n"
		" $d ディレクトリ(ドライブ無) \\dir\\dir2\n"
		" $D 直上１つのディレクトリ   dir\n"
		" $c ファイル(拡張子付)       filename.ext\n"
		" $x ファイル(拡張子無)       filename\n"
		" $e 拡張子                   ext\n"
		" $w テンポラリ・ディレクトリ (環境変数TMPの内容)\n"
		" $z サイズ(10進10桁)         1234567890\n"
		" $Z サイズ(16進8桁)          ABCDEF1234\n"
		" $j 時間                     1993-02-14   $+8j 93-02-14\n"
		"                       $+23j 2010-04-03 04:02:05.999\n"
		" $J 時間(ラベル化用)   $+23J 2010_04_03_04_02_05_999\n"
		" $a ファイル属性             drhsacen\n"
		" $A ファイル属性(16進数)     0x10\n"
		" $i 連番生成 10進数          $+4j  0255\n"
		" $I 連番生成 16進数          $+3J  00FF\n"
		" $$ $  $[ <  $` '  $n 改行  $t タブ\n"
		" $# #  $] >  $^ \"  $s 空白  $l 生入力のまま\n"
		"\n"
		"変換補助文字: 以下@は変換文字.\n"
		" $+?@   変換文字@で最低?桁にする.(0埋か右詰で空白補完)\n"
		" $R@    フルパス表記を相対パス表記に変換\n"
		" $U@    大文字化\n"
		" $L@    小文字化\n"
		" $b@    パスの\\を/に置換\n"
		" $B@    パスの/を\\に置換\n"
		;


int abx_usage(char const* exename)
{
 #ifdef _WIN32
	if (GetUserDefaultLCID() == 1041) {
	 #ifdef USE_SRC_DBC
		printf(DBStoCONS(APP_HELP_TITLE_JP APP_HELP_CMDLINE), exename, exename);
		printf("%s", DBStoCONS(s_helpJp));
	 #else
		printf(UTF8toCONS(APP_HELP_TITLE_JP APP_HELP_CMDLINE), exename, exename);
		printf("%s", UTF8toCONS(s_helpJp));
	 #endif
	} else {
		printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
	}
 #else
	printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
 #endif
	return 1;
}
