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
#include <fks_dirent.h>


typedef std::vector<struct Fks_DirEntPathStat const*>	PathStats;


enum SearchAttrType {
	SRCH_DIR	= FKS_DE_Dir,
	SRCH_FILE	= FKS_DE_File,
	SRCH_DOTDOT = FKS_DE_DotOrDotDot,
	SRCH_HIDDEN	= FKS_DE_Hidden,
	SRCH_RDONLY	= FKS_DE_ReadOnly,
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
					, srchAttr_(0), fileAttr_(0)
					, charCodeChk_(0), sortType_(ST_NONE)
					, sortRevFlg_(false), sortLwrFlg_(false)
					, recursiveFlg_(false)
	{
		dateMin_.tv_sec = FKS_TIME_MAX;
		dateMin_.tv_nsec = FKS_TIME_MAX;
		dateMax_.tv_sec = 0;
		dateMax_.tv_nsec = 0;
	}

public:
	FPathBuf	    inputDir_; 	    // Input directory.
    FPathBuf	    dfltExt_;	    // Default extension buffer.
    char const*     dfltExtp_;	    // Default extension pointer.

    fks_isize_t	    sizeMin_; 	    // Minimum file size.
    fks_isize_t	    sizeMax_;		// Maximum file size.
    fks_timespec    dateMin_; 	    // Minimum file date-time.
    fks_timespec    dateMax_;		// Maximum file date-time.

	unsigned		srchAttr_;		// attribute for search
    unsigned	    fileAttr_; 	    // file attribute.(raw)
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

	static int matchCheck(void* opts0, struct Fks_DirEnt const* de);
	static int dirMatchCheck(void* opts0, struct Fks_DirEnt const* de);
    static int chkKnjs(const char *p);

private:
	FPathBuf	    fname_;	    		// name work buffer.
	FPathBuf	    inputDir_;			// input directory.
	PathStats		pathStats_;			// path&stat list.
	PathStats		pathStatBody_;		// path&stat data buffer.
};

#endif
