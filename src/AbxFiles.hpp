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

#include <fks_types.h>

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
    ST_NONE = 0x00,		// none.
    ST_NAME = 0x01, 	// file name.
    ST_EXT  = 0x02, 	// file extension.
    ST_SIZE = 0x04, 	// file size.
    ST_DATE = 0x08, 	// file date-time.
    ST_ATTR = 0x10, 	// file attribute.
    ST_NUM  = 0x20, 	// Numeric name comparison.
};


struct AbxFiles_Opts {
	AbxFiles_Opts() : inputDir_(), dfltExt_(), dfltExtp_(NULL)
					, sizeMin_(FKS_ISIZE_MAX), sizeMax_(0)
					, dateMin_(FKS_TIME_MAX), dateMax_(0)
					, fileAttr_(0), charCodeChk_(0), sortType_(ST_NONE)
					, sortRevFlg_(false), sortLwrFlg_(false)
					, recursiveFlg_(false)
	{
	}

public:
	FPathBuf	    inputDir_; 	    // Input directory.
    FPathBuf	    dfltExt_;	    // Default extension buffer.
    char const*     dfltExtp_;	    // Default extension pointer.

    fks_isize_t	    sizeMin_; 	    // Minimum file size.
    fks_isize_t	    sizeMax_;		// Maximum file size.
    fks_time_t	    dateMin_; 	    // Minimum file date-time.
    fks_time_t	    dateMax_;		// Maximum file date-time.

    unsigned	    fileAttr_; 	    // file attribute.
    int     	    charCodeChk_;   // character code check.

    SortType	    sortType_;	    // sort type
    bool    	    sortRevFlg_;	// reverse
    bool    	    sortLwrFlg_;	// lower(ignore case)
	bool			sortDirFlg_;	// directory first
    bool			recursiveFlg_;	// recursive
};


class AbxFiles {
public:
	AbxFiles() {}

	PathStats const&	pathStats() const { return pathStats_; }

	bool getPathStats(StrList& filenameList, AbxFiles_Opts const& opts);

private:
	void sortPartStats(SortType sortType, bool sortRevFlg, bool sortICaseFlg, bool sortDirFlg);

	static int matchCheck(struct Fks_DirEnt const* de);
    static int chkKnjs(const char *p);

private:
	FPathBuf	    fname_;	    		// name work buffer.
	FPathBuf	    inputDir_;			// input directory.
	PathStats		pathStats_;			// path&stat list.
	PathStats		pathStatBody_;		// path&stat data buffer.

	static AbxFiles_Opts const*	opts_;	// options.
};

#endif
