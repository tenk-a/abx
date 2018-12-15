/**
 *  @file   abx_msgstrJp.cpp
 *  @brief  abx message strings for Japanese.
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *  	Boost Software License Version 1.0
 */
#include "AbxMsgStr.hpp"

//#ifdef ABX_USE_JAPAN

static AbxMsgStr const abxMsgStrJp = {
	//file_open_error
	"ファイル・オープン失敗. : %s\n",
	//incorrect_command_line_option
	"オプションがおかしい. : %s\n",
	//file_read_error
	"ファイル読込エラー. : %s\n",
	//cfg_read_error
	".cfgファイル読込エラー. : %s\n",
	//conversion_names_list
	":変換名 一覧.\n",
	//key_is_not_defined
	"%s が定義されていない. : %s\n",
	//cfgfile_has_an_incorrect_dollN_specification
	".cfgファイルで$N指定がおかしい.: $%s\n",
	//dollN_specification_in_cfgfile_is_incorrect
	".cfgファイル中の $N=文字列 か $N:M{..} 指定がおかしい. : $%s\n",
	//single_quotation_string_is_broken
	"'文字列' が壊れている.\n",
	//there_are_more_par_pair_in_a_certain_line
	"%s のある検索行に{..}が10個以上ある : %s\n",
	//invalid_par_pair
	"%s で{..}の指定がおかしい : %s\n",
	//colon_hash_string_can_not_be_specified
	":#で始まる文字列は指定できない : %s\n",
	//xm_and_Hbegin_can_not_be_used_at_the_same_time
    "-xm と #begin,#end は同時には使用できない\n",
	//incorrect_dollar_format
	"%c指定がおかしい(%c%c)\n",
	//usage
	ABX_VERTION "  ファイルを検索,ファイル名を文字列に埋込(バッチ生成)\n"
	"usage> abx [オプション] ファイル… '変換文字列' ファイル…\n"
	"       abx [オプション] ファイル名 =変換文字列\n"
	,
	//usage_options
	"オプション:\n"
	" -? -h      このヘルプ\n"
	" -x[-]      バッチ実行 -x- しない\n"
 #ifdef ENABLE_MT_X
	" -xm[N]     Nスレッド実行. 0自動\n"
 #endif
	" -r[-]      ディレクトリ再帰\n"
	" -a[dfh:N]  大枠のファイル属性:詳細os属性値 でファイル検索\n"
	"            d:ディレクトリ f:ファイル h:隠しファイル属性も対象\n"
    "            N:osのファイル属性整数値. 大枠かつこの数値にマッチするモノ\n"
	" -z[N-M]    サイズ N～MのFILEを検索\n"
	" -d[A-B]    日付A～BのFILEを検索\n"
	" -s[neztam] ソート(整列)\n"
	"   [rd]     n:名 m:名(数) e:拡張子 z:サイズ\n"
	"            t:日付 a:属性 r:降順 d:ディレクトリ優先\n"
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
	"\n"
	"変換文字(@ 部分は$も可):     変換例:\n"
	" @f フルパス(拡張子付)       d:\\dir\\dir2\\filename.ext\n"
	" @g フルパス(拡張子無)       d:\\dir\\dir2\\filename\n"
	" @v ドライブ                 d\n"
	" @p ディレクトリ(ドライブ付) d:\\dir\\dir2\n"
	" @d ディレクトリ(ドライブ無) \\dir\\dir2\n"
	" @D 直上１つのディレクトリ   dir\n"
	" @c ファイル(拡張子付)       filename.ext\n"
	" @x ファイル(拡張子無)       filename\n"
	" @e 拡張子                   ext\n"
	" @w テンポラリ・ディレクトリ (環境変数TMPの内容)\n"
	" @z サイズ(10進10桁)         1234567890\n"
	" @Z サイズ(16進8桁)          ABCDEF1234\n"
	" @j 時間                     1993-02-14   @+8j 93-02-14\n"
	"                       @+23j 2010-04-03 04:02:05.999\n"
	" @J 時間(ラベル化用)   @+23J 2010_04_03_04_02_05_999\n"
//	" @a ファイル属性             drhsacen\n"
//	" @A ファイル属性(16進数)     0x10\n"
	" @i 連番生成 10進数          @+4j  0254\n"
	" @I 連番生成 16進数          @+2J  FE\n"
	" @@ @  @[ <  @` '  @n 改行  @t タブ\n"
	" @# #  @] >  @^ \"  @s 空白  @l 生入力のまま\n"
	"\n"
	"変換補助文字: 以下#は変換文字.\n"
	" @+?#   変換文字@で最低?桁にする.(0埋か右詰で空白補完)\n"
	" @R#    フルパス表記を相対パス表記に変換\n"
	" @U#    大文字化\n"
	" @L#    小文字化\n"
	" @b#    パスの\\を/に置換\n"
	" @B#    パスの/を\\に置換\n"
	,
};


struct AbxMsgStrJpInit {
	AbxMsgStrJpInit() { abxMsgStr = (fks_ioIsJapan()) ? &abxMsgStrJp : &abxMsgStrEn; }
};
AbxMsgStrJpInit s_abxMsgStrJpInit;
//#endif
