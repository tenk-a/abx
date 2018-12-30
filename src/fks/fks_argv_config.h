/**
 *  @file   fks_argv_config.h
 *  @brief  argc,argvの拡張処理(ワイルドカード,レスポンスファイル).
 *  @author Masashi KITAMURA
 *  @date   2006-2018
 *  @note
 *		fks_argv の設定ファイル.
 */

#ifndef FKS_ARGV_CONFIG_H_INCLUDED
#define FKS_ARGV_CONFIG_H_INCLUDED

//[] ワイルドカード指定を 1=有効  0=無効  未定義=デフォルト設定(1)
//#define FKS_ARGV_USE_WC     	1


//[] ワイルドカードon時に、ワイルドカード文字 ** があれば再帰検索に
//  	1=する 0=しない 未定義=デフォルト設定(1)
//#define EXARGV_USE_WC_REC 	1


//[] @レスポンスファイルを
//  	1=有効	 0=無効  未定義=デフォルト設定(0)
#define FKS_ARGV_USE_RESFILE  1

//[] レスポンスファイルの文字エンコードがUTF-8かSJISか自動判別
//  	1=有効	0=無効	未定義=デフォルト(win:1, 以外:0)
//#define	FKS_ARGV_USE_RESFILE_AUTO_CHARENC	1

//[] 簡易コンフィグ(レスポンス)ファイル入力を
//  	1=有効	0=無効	未定義=デフォルト(0)
//   有効時は、win/dosなら .exe を .cfg に置換したパス名.
//  	       以外なら unix 系だろうで ~/.実行ファイル名.cfg
//#define FKS_ARGV_USE_CONFIG 	0

//[] コンフィグファイル入力有効のとき、これを定義すれば、
//  	コンフィグファイルの拡張子をこれにする.
//#define FKS_ARGV_CONFIG_EXT 	".cfg"	// .conf


//[] 定義すると、この名前の環境変数をコマンドライン文字列として利用.
//#define FKS_ARGV_ENVNAME    "your_app_env_name"


//[] win環境のみ. argv[0] の実行ファイル名をフルパス化
//  	1=有効	0=無効	    未定義=デフォルト(0)
//   ※bcc,dmc,watcomは元からフルパスなので何もしません. のでvc,gcc向.
//#define FKS_ARGV_USE_FULLPATH_ARGV0


//[] 定義すれば、filePath中の \ を / に置換.
//#define FKS_ARGV_TOSLASH


//[] 定義すれば、filePath中の / を \ に置換.
#define FKS_ARGV_TOBACKSLASH


//[] 定義すれば、/ もオプション開始文字とみなす.
//#define FKS_ARGV_USE_SLASH_OPT

#endif
