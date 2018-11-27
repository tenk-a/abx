/**
 *  @file   AbxFiles.hpp
 *  @brief  get directory entries
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */
#ifndef ABXFILES_H_INCLUDED
#define ABXFILES_H_INCLUDED

#include "subr.hpp"
#include <vector>

#include <fks/fks_types.h>

typedef std::vector<struct Fks_DirEntPathStat const*>	PathStats;

enum FileAttr {
    FA_RdOnly 	= 0x01,		//FILE_ATTRIBUTE_READONLY,
    FA_Hidden 	= 0x02, 	//FILE_ATTRIBUTE_HIDDEN,
    FA_Sys    	= 0x04, 	//FILE_ATTRIBUTE_SYSTEM,
    //FA_Volume = 0x08,		// DOS FAT
    FA_Dir    	= 0x10, 	//FILE_ATTRIBUTE_DIRECTORY,
    FA_Arcive 	= 0x20, 	//FILE_ATTRIBUTE_ARCHIVE,
	//FA_Device = 0x40, 	//FILE_ATTRIBUTE_DEVICE,
	FA_Norm   	= 0x80, 	//FILE_ATTRIBUTE_NORMAL,
	FA_Temp	  	= 0x100,	// FILE_ATTRIBUTE_TEMPORARY,
	//FA_Sparse = 0x200, 	// FILE_ATTRIBUTE_SPARSE_FILE
	//FA_Reparse= 0x400, 	// FILE_ATTRIBUTE_SPARSE_FILE
	FA_Comp	  	= 0x800, 	// FILE_ATTRIBUTE_COMPRESSED,
	//FA_Offline= 0x1000,	// FILE_ATTRIBUTE_OFFLINE
	//FA_No_indexed=0x2000,	// FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
    FA_Encrypt	= 0x4000,	//FILE_ATTRIBUTE_ENCRYPTED,
	//FA_IntegritySys=0x8000,	// FILE_ATTRIBUTE_INTEGRITY_STREAM
	FA_Virtual	= 0x10000,		// FILE_ATTRIBUTE_VIRTUAL
	//FA_NoScrub	= 0x20000,	// FILE_ATTRIBUTE_NO_SCRUB_DATA
	//FA_EA		= 0x40000,	// FILE_ATTRIBUTE_NO_SCRUB_DATA
	//FA_MASK_NOARC = FA_Norm|FA_RdOnly|FA_Hidden|FA_Sys|FA_Volume|FA_Dir,
	FA_MASK_FILEYTPE = FA_RdOnly|FA_Hidden|FA_Sys|FA_Norm|FA_Arcive,
	FA_MASK_NOARC = FA_RdOnly|FA_Hidden|FA_Sys|FA_Dir|FA_Norm|FA_Temp|FA_Comp|FA_Encrypt|FA_Virtual,
	FA_MASK   = FA_MASK_NOARC | FA_Arcive,
};


enum SortType {
    ST_NONE = 0x00,
    ST_NAME = 0x01, 	// 名前でソート.
    ST_EXT  = 0x02, 	// 拡張子.
    ST_SIZE = 0x04, 	// サイズ.
    ST_DATE = 0x08, 	// 日付/時間.
    ST_ATTR = 0x10, 	// ファイル属性.
    ST_NUM  = 0x20, 	// 数値比較の名前.
    //ST_MASK = 0x7F,	// ソート情報マスク
};


struct AbxFiles_Opts {
	AbxFiles_Opts() : ipath_(), dfltExt_(), dfltExtp_(NULL)
					, szmin_(FKS_ISIZE_MAX), szmax_(0)
					, dtmin_(FKS_TIME_MAX), dtmax_(0)
					, fattr_(0), knjChk_(0), sortType_(ST_NONE)
					, sortRevFlg_(false), sortLwrFlg_(false)
					, recFlg_(false)
	{
	}

public:
	FnameBuf	    ipath_; 	    	    /* 入力パス名 */
    FnameBuf	    dfltExt_;	    	    /* デフォルト拡張子 */
    char const*     dfltExtp_;	    	    /* デフォルト拡張子 */

    fks_isize_t	    szmin_; 	    	    /* szmin > szmaxのとき比較を行わない*/
    fks_isize_t	    szmax_;
    fks_time_t	    dtmin_; 	    	    /* dtmin > dtmaxのとき比較を行わない*/
    fks_time_t	    dtmax_;

    unsigned	    fattr_; 	    	    /* ファイル属性 */
    int     	    knjChk_;	    	    /* MS全角存在チェック */

    SortType	    sortType_;	    	    /* ソート */
    bool    	    sortRevFlg_;
    bool    	    sortLwrFlg_;
    bool			recFlg_;
};


class AbxFiles {
public:
	AbxFiles() {}

	PathStats const&	pathStats() const { return pathStats_; }

	bool getPathStats(StrList& filenameList, AbxFiles_Opts const& opts);

private:
	void sortPartStats(SortType sortType, bool sortRevFlg, bool sortLwrFlg);

	static int matchCheck(struct Fks_DirEnt const* de);
    static int chkKnjs(const char *p);

private:
	FnameBuf	    fname_;	    	/* 名前 work */
	FnameBuf	    ipath_;
	PathStats		pathStats_;
	PathStats		pathStatBody_;

	static AbxFiles_Opts const*		opts_;
};

#endif
