/**
 *  @file   abx_usage.c
 *  @brief  usage for abx
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license	Boost Software License Version 1.0
 */

#include <stdio.h>
#include <abx_usage.h>

#define APP_HELP_TITLE_JP	"abx v3.93(pre v4) ﾌｧｲﾙ名を検索,該当ﾌｧｲﾙ名を文字列に埋込(ﾊﾞｯﾁ生成)\n"	\
    	    	    	    "  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"

#define APP_HELP_TITLE	    "abx v3.93(pre v4) Search file, embed file name in text(gen. bat)\n"    \
    	    	    	    "  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"

#define APP_HELP_CMDLINE    "usage : %s [option]  filename(s) 'text conversion'\n"	\
						    "        %s [option]  filename(s) =text conversion\n"

int abx_usage(char const* exename)
{
 #if _WIN32
	if (GetUserDefaultLCID() == 1041) {
		printf(APP_HELP_TITLE_JP APP_HELP_CMDLINE, exename, exename);
	} else {
		printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
	}
 #else
	printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
 #endif
	printf("%s", 
		"Option:\n"
		" -x[-]      バッチ実行 -x-しない   \n"	
		" -xm[N]     Nスレッド実行.0自動  \n"	
		" -r[-]      ディレクトリ再帰          \n"	
		" -a[nrhsd]  指定ファイル属性で検索   \n"	
		"            n:一般 s:システム h:隠し \n"	
		"            r:読込専用 d:ディレクトリ \n"	
		" -z[N-M]    サイズ N～MのFILEを検索 \n"	
		" -d[A-B]    日付A～BのFILEを検索 \n"	
		" -s[neztam] ソート(整列)            \n"	
		"   [r]      n:名 e:拡張子 z:サイズ \n"	
		"            t:日付 a:属性 r:降順 \n"	
		"            m:名(数)             \n"	
		" -n[-]      ファイル検索しない -n-有 \n"	
		" -u[-]      $c|$Cでファイル名大小文字\n"	
		" -l[-]      @入力で名前は行単位\n"	
		" -ci[N:M]   N:$iの開始番号(M:終)\n"	
		" +CFGFILE   .CFGファイル指定\n"	
		" @RESFILE   レスポンスファイル\n"	
		" :変換名    CFGで定義した変換\n"	
		" :          変換名一覧を表示\n"	
		" -p<DIR>    $pの強制変更\n"
		" -e<EXT>    デフォルト拡張子\n"
		" -o<FILE>   出力ファイル指定\n"
		" -i<DIR>    検索ディレクトリ\n"
		" -w<DIR>    TMPディレクトリ\n"
		" -ct<FILE>  FILEより新なら\n" 
		" -ck[-]     日本語名のみ検索\n"  
		" -cy[-]     \\を含む全角名検索\n"
		" -y         $cxfgdpwに\"付加\n"  
		" -t[N]      最初のN個のみ処理\n" 
		"\n"
		"変換文字:             変換例:\n"
		" $f フルパス(拡張子付)      d:\\dir\\dir2\\filename.ext\n" 
		" $g フルパス(拡張子無)      d:\\dir\\dir2\\filename\n"	 
		" $v ドライブ                d\n"    	    	    	    
		" $p ディレクトリ(ドライブ付) d:\\dir\\dir2\n"	    	    
		" $d ディレクトリ(ドライブ無) \\dir\\dir2\n" 
		" $D 直上１つのディレクトリ  dir\n"
		" $c ファイル(拡張子付)      filename.ext\n" 	    	    
		" $x ファイル(拡張子無)      filename\n"     	    	    
		" $e 拡張子                  ext\n"  	    	    	    
		" $w テンポラリ・ディレクトリ (環境変数TMPの内容)\n"  	    
		" $z サイズ(10進10桁)        1234567890\n"
		" $Z サイズ(16進8桁)         ABCDEF1234\n"
		" $j 時間                    1993-02-14 , 2010-04-03 04:02:05\n"   	    	    
		" $J 時間(ファイル名用)      2010-04-03_04.02.05.999\n"   	    	    
		" $i 連番生成 10進数\n"
		" $I 連番生成 16進数\n"
		" $$ $  $[ <  $` '  $n 改行  $t ﾀﾌﾞ\n" 	    	    
		" $# #  $] >  $^ \"  $s 空白  $l 生入力のまま\n"	 
		"変換補助文字: 以下@は変換文字. ??は代用文字(数字)\n"
        " $+??@  変換文字@での最低桁数??\n"
        " $R@    フルパス表記変換@ を相対パス表記に変換\n"
        " $U@    大文字化\n"
        " $u@    小文字化\n"
        " $B@    パスの\\を/に置換\n"
        " $b@    パスの/を\\に置換\n"
	);
	return 1;
}
