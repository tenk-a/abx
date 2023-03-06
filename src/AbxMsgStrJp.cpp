/**
 *  @file   abx_msgstrJp.cpp
 *  @brief  abx message strings for Japanese.
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *      Boost Software License Version 1.0
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
    "usage> abx [オプション] ファイル… =変換文字列\n"
    "       abx [オプション] ファイル… '変換文字列' ファイル…\n"
    "       abx [オプション] ファイル… -k=\"変換文字列\" ファイル…\n"
    ,
    //usage_options
    "オプション:\n"
    " -? -h           このヘルプ\n"
    " -k[$@RFULBby][=\"変換文字列\"]\n"
    "                 $ @   変換指定文字を $ か @ にする\n"
    "                 R F U L B b y パス名変換指定の補助指定のデフォルト設定\n"
 #ifdef FKS_WIN32
    " -x[-]           バッチ実行 -x- しない\n"
 #else
    " -x[-]           shell実行 -x- しない\n"
 #endif
 #ifdef ENABLE_MT_X
    " -xm[N]          Nスレッド実行. 0自動\n"
 #endif
    " -r[-]           ディレクトリ再帰\n"
    " -a[dfh:N]       大枠のファイル属性:詳細os属性値 でファイル検索\n"
    "                 d:ディレクトリ f:ファイル h:隠しファイル属性も対象\n"
    "                 N:osのファイル属性整数値. 大枠かつこの数値にマッチするモノ\n"
 #ifdef FKS_WIN32
    " -a[dfh:flags]   flags:rhsdaDntplcoievVxEPUAQ  該当win32ファイル属性指定\n"
 #endif
    " -z[N-M]         サイズ N～MのFILEを検索\n"
    " -d[A-B]         日付A～BのFILEを検索\n"
    " -s[neztam]      ソート(整列)\n"
    "   [rd]          n:名 m:名&数 e:拡張子 z:サイズ\n"
    "                 t:日付 a:属性 r:降順 d:ディレクトリ優先\n"
    " -n[-]           ファイル検索しない  -n- する\n"
    // " -u[-]        $c|$Cでファイル名大小文字\n"
    " -y              $cxfgdpwでパス名の両端に\"付加(-kyに同じ)\n"
    " -l[-]           レスポンスファイル入力で名前は行単位\n"
    " -ci[N]          N:$iの開始番号\n"
    " -ci[N:M]        ファイル検索せずにN-M連番生成\n"
    " -e<EXT>         デフォルト拡張子\n"
    " -o<FILE>        出力ファイル指定\n"
    " -i<DIR>         検索ディレクトリ\n"
    " -cd<DIR>        相対パス生成時の基準ディレクトリ\n"
    " -w<DIR>         tmpディレクトリ\n"
    " -p<DIR>         $pの強制変更\n"
    " -ct<FILE>       FILE($変数可)より新しい日付ならマッチ\n"
    " -cu<FILE>       -ct 同様で、FILE無時にフォルダ生成\n"
    " -ck[-]          ascii以外の文字のあるファイル名を検索\n"
 #ifdef USE_PATH_DBC
    " -cy[-]          \\を含む全角名検索\n"
 #endif
 #ifdef FKS_WIN32
    " -utf8           utf8出力\n"
 #endif
    " -t[N]           最初のN個のみ処理\n"
    " $N=<STR>        N:1-9  変数設定\n"
    " +CFGFILE        .cfgファイル指定\n"
    " $RESFILE        レスポンスファイル($1～9以外)\n"
    " -$RESFILE       レスポンスファイル\n"
    " :変換名         CFGで定義した変換\n"
    " :               変換名一覧を表示\n"
    "\n"
    ,
    // conversion_orders
    "変換指定文字は -k$$ または -k@ 指定があればその文字になる.\n"
    "指定がなければ $$ か @ のうち先に使われたほうを用いる.\n"
    "変換文字($$ または @):         変換例:\n"
    " $$f  フルパス                 $f\n"
    "     $$Rf 相対パス             $Rf\n"
    "     $$yf 両端に\"付加          $yf\n"
    "     $$bf /で表記              $bf\n"
    "     $$Bf \\で表記              $Bf\n"
    " $$g  拡張子無フルパス         $g\n"
 #ifdef FKS_WIN32
    " $$v  ドライブ                 $v\n"
    " $$p  ディレクトリ(ドライブ付) $p\n"
    " $$d  ディレクトリ(ドライブ無) $d\n"
 #else
    " $$p  ディレクトリ             $p\n"
    " $$d  ディレクトリ             $d\n"
 #endif
    " $$D  直１つ上のディレクトリ   $D\n"
    " $$c  ファイル(拡張子付)       $c\n"
    " $$x  ファイル(拡張子無)       $x\n"
    " $$e  拡張子                   $e\n"
    " $$w  テンポラリ・ディレクトリ $w\n"
    " $$o  -ctで指定されたファイル\n"
    " $$z  サイズ(10進10桁)         $z\n"
    "     桁数でKB等表記     $$+8bz $+8bz\n"
    "     桁数でKiB等表記    $$+8Bz $+8Bz\n"
    " $$Z  サイズ(16進8桁)          $Z\n"
    " $$j  日付                     $j\n"
    "     年上位省略         $$+8j  $+8j\n"
    "     +時間              $$+19j $+19j\n"
    "     +時間(+ミリ秒)     $$+23j $+23j\n"
    " $$J  時間(ラベル化用)   $$+23J $+23J\n"
    " $$a  ファイル属性             $a\n"
 #ifdef FKS_WIN32
    " $$ba unix系風ファイル属性     [ld]rwxrwxrwx\n"
    " $$Ba win32主要ファイル属性    rhsdalcoievxpu\n"
 #endif
    " $$A  ファイル属性(16進数)     $A\n"
 #ifdef FKS_WIN32
    " $$BA win32 32bitファイル属性  rhs-daDntplcoievVxEPU-A--Q------\n"
 #endif
    " $$i  連番生成 10進数          $i\n"
    "                         $$+5i $+5i\n"
    " $$I  連番生成 16進数          $I\n"
    " $$l  生の入力文字列まま\n"
    " $$1～9  変数\n"
    " $$t  タブ\n"
    " $$s  空白\n"
    " $$[  <\n"
    " $$]  >\n"
    " $$^  \"\n"
    " $$`  '\n"
    " $$$$  $$\n"
    " $$@  @\n"
    " $$#  #\n"
    "\n"
    "変換補助文字: 以下#は変換文字.\n"
    " $$+N#  変換文字#で(最低)N桁にする.(0埋か空白補完)\n"
    " $$y#   パスの両端に\"付加\n"
    " $$R#   パス表記を相対パス表記に変換\n"
    " $$F#   パス表記をフルパス表記に変換\n"
    " $$b#   パスの\\を/に置換\n"
    " $$B#   パスの/を\\に置換\n"
    " $$U#   大文字化\n"
    " $$L#   小文字化\n"
    "\n"
    "ex)$$+24Rbyf $$+8Bz $$+16j\n"
    "   $+24Rbyf $+8Bz $+16j\n"
    ,
};


struct AbxMsgStrJpInit {
    AbxMsgStrJpInit() { abxMsgStr = (fks_ioIsJapan()) ? &abxMsgStrJp : &abxMsgStrEn; }
};
AbxMsgStrJpInit s_abxMsgStrJpInit;
//#endif
