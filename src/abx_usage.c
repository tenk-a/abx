/**
 *  @file   abx_usage.c
 *  @brief  usage for abx
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license	Boost Software License Version 1.0
 */

#include <stdio.h>
#include <abx_usage.h>


#define APP_HELP_TITLE	    "abx v3.93(pre v4) ﾌｧｲﾙ名を検索,該当ﾌｧｲﾙ名を文字列に埋込(ﾊﾞｯﾁ生成)\n"   	    	    \
    	    	    	    "  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"
#define APP_HELP_CMDLINE    "usage : %s [ｵﾌﾟｼｮﾝ] ['変換文字列'] ﾌｧｲﾙ名 [=変換文字列]\n"
#define APP_HELP_OPTS	    "ｵﾌﾟｼｮﾝ:                        ""変換文字:            変換例:\n"	    	    	    \
    	    	    	    " -x[-]    ﾊﾞｯﾁ実行 -x-しない   "" $f ﾌﾙﾊﾟｽ(拡張子付)   d:\\dir\\dir2\\filename.ext\n"  \
    	    	    	    " -xm[N]   Nスレッド実行.0自動  "" $g ﾌﾙﾊﾟｽ(拡張子無)   d:\\dir\\dir2\\filename\n"	    \
    	    	    	    " -r[-]    ﾃﾞｨﾚｸﾄﾘ再帰          "" $v ﾄﾞﾗｲﾌﾞ            d\n"    	    	    	    \
    	    	    	    " -a[nrhsd] 指定ﾌｧｲﾙ属性で検索  "" $p ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ付) d:\\dir\\dir2\n"	    	    \
    	    	    	    "          n:一般 s:ｼｽﾃﾑ h:隠し "" $d ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ無) \\dir\\dir2\n"  	    	    \
    	    	    	    "          r:ﾘｰﾄﾞｵﾝﾘｰ d:ﾃﾞｨﾚｸﾄﾘ "" $c ﾌｧｲﾙ(拡張子付)    filename.ext\n" 	    	    \
    	    	    	    " -z[N-M]  ｻｲｽﾞN～MのFILEを検索 "" $x ﾌｧｲﾙ(拡張子無)    filename\n"     	    	    \
    	    	    	    " -d[A-B]  日付A～BのFILEを検索 "" $e 拡張子            ext\n"  	    	    	    \
    	    	    	    " -s[neztam][r] ｿｰﾄ(整列)       "" $w ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ    (環境変数TMPの内容)\n"  	    \
    	    	    	    "          n:名 e:拡張子 z:ｻｲｽﾞ "" $z ｻｲｽﾞ(10進10桁)    1234567890 ※$Zなら16進8桁\n"   \
    	    	    	    "          t:日付 a:属性 r:降順 "" $j 時間              1993-02-14\n"   	    	    \
    	    	    	    "          m:名(数)             "" $i 連番生成          ※$Iなら16進数\n"	    	    \
    	    	    	    " -n[-]    ﾌｧｲﾙ検索しない -n-有 "" $$ $  $[ <  $` '  $n 改行  $t ﾀﾌﾞ\n" 	    	    \
    	    	    	    " -u[-]    $c|$Cでﾌｧｲﾙ名大小文字"" $# #  $] >  $^ \"  $s 空白  $l 生入力のまま\n"	    \
    	    	    	    " -l[-]    @入力で名前は行単位  ""------------------------------------------------\n"   \
    	    	    	    " -ci[N:M] N:$iの開始番号(M:終) ""-p<DIR>  $pの強制変更   ""-ct<FILE> FILEより新なら\n" \
    	    	    	    " +CFGFILE .CFGﾌｧｲﾙ指定         ""-e<EXT>  ﾃﾞﾌｫﾙﾄ拡張子   ""-ck[-] 日本語名のみ検索\n"  \
    	    	    	    " @RESFILE ﾚｽﾎﾟﾝｽﾌｧｲﾙ           ""-o<FILE> 出力ﾌｧｲﾙ指定   ""-cy[-] \\を含む全角名検索\n"\
    	    	    	    " :変換名  CFGで定義した変換    ""-i<DIR>  検索ﾃﾞｨﾚｸﾄﾘ    ""-y     $cxfgdpwに\"付加\n"  \
    	    	    	    " :        変換名一覧を表示     ""-w<DIR>  TMPﾃﾞｨﾚｸﾄﾘ     ""-t[N]  最初のN個のみ処理\n" \

int abx_usage(char const* exename)
{
	printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename);
	printf("%s", APP_HELP_OPTS);
	return 1;
}
