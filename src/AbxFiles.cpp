/**
 *  @file   AbxFiles.cpp
 *  @brief  get directory entries
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */

#include "AbxFiles.hpp"

#include <fks_path.h>
#include <fks_io.h>
#include <fks_dirent.h>


bool AbxFiles::getPathStats(StrList& filenameList, AbxFiles_Opts const& opts)
{
	Fks_DirEnt_Matchs	mt 		= {0};
	unsigned int 		flags 	= opts.srchAttr_;

	if (opts.recursiveFlg_) {
		flags |= FKS_DE_Recursive;
		mt.isDirMatch 	  	= &dirMatchCheck;
		mt.isDirMatchData 	= (void*)&opts;
	}
	mt.flags = flags;

	if (   opts.fileAttr_
		|| (opts.sizeMin_ <= opts.sizeMax_)
		|| (opts.dateMin_ <= opts.dateMax_)
		|| opts.charCodeChk_
	){
		mt.isMatch 			= &matchCheck;
		mt.isMatchData		= (void*)&opts;
	}

	FPathBuf  fnmWk;
	inputDir_		= opts.inputDir_;
	char*	iname	= fks_pathBaseName(&inputDir_[0]);
	pathStatBody_.reserve(filenameList.size());
	for (StrList::iterator ite = filenameList.begin(); ite != filenameList.end(); ++ite) {
    	char const* p = ite->c_str();
    	if (!fks_pathIsAbs(p)) {	// relative path?
    	    *iname  	= '\0';
    	    inputDir_  += p;
    	    p       	= &inputDir_[0];
    	}
    	fname_ = p;
    	if (fks_pathCheckLastSep(p))
    	    fname_ += "*";

    	fks_pathSetDefaultExt(&fname_[0], fname_.capacity(), opts.dfltExtp_);
		char* fname = &fname_[0];
		if (flags & FKS_DE_Recursive) {
			char* b = fks_pathBaseName(&fname_[0]);
			if (b > &fname_[0])
				b[-1] = 0;
			fks_fileFullpath(&fnmWk[0], fnmWk.capacity(), &fname_[0]);
			if (b > &fname_[0])
				fks_pathCombine(&fnmWk[0], fnmWk.capacity(), b);
			fname = &fnmWk[0];
		}
		mt.fname = fname;
		Fks_DirEntPathStat*	pPathStats = NULL;
		fks_isize_t	n = fks_createDirEntPathStats(&pPathStats, NULL, &mt);
		if (n < 0)
			continue;
		if (n > 0 && pPathStats == NULL) {
			fprintf(stderr, "ERROR: read directories.\n");
			continue;
		}
		pathStatBody_.push_back(pPathStats);
		pathStats_.reserve( size_t(pathStats_.size() + n) );
		for (fks_isize_t i = 0; i < n; ++i) {
			Fks_DirEntPathStat* ps = &pPathStats[i];
			if (ps->path == NULL) {
				fprintf(stderr, "ERROR: pathname is null.\n");
				continue;
			}
			if (ps->stat == NULL) {
				fprintf(stderr, "ERROR: no file status (%s)\n", ps->path);
				continue;
			}
			pathStats_.push_back(ps);
		}
	}

	if (opts.sortType_ || opts.sortRevFlg_) {
		sortPartStats(opts.sortType_, opts.sortRevFlg_, opts.sortLwrFlg_, opts.sortDirFlg_);
	}

	return true;
}


int AbxFiles::matchCheck(void* opts0, Fks_DirEnt const* de)
{
	AbxFiles_Opts const& opts = *(AbxFiles_Opts*)opts0;

 #ifdef FKS_WIN32
	unsigned    fattr = opts.fileAttr_;
	if (fattr) {
		unsigned	w32attr = de->stat->st_native_attr;
		//printf(">>> %04x & %04x = %04x\n", fattr, w32attr, fattr & w32attr);
		if ((fattr & w32attr) == 0)
			return 0;
	}
 #endif
	if (opts.srchAttr_ & SRCH_FILE) {
	 	if (opts.sizeMin_ < opts.sizeMax_) {
			if (de->stat->st_size < opts.sizeMin_ || opts.sizeMax_ < de->stat->st_size)
				return 0;
		}
	}
	if (opts.dateMin_ < opts.dateMax_) {
		if (de->stat->st_mtime < opts.dateMin_ || opts.dateMax_ < de->stat->st_mtime)
			return 0;
	}

	int ccChk = opts.charCodeChk_;
	if (ccChk) {
		if (ccChk ==  1 && !chkKnjs(de->name))
			return 0;
		if (ccChk == -1 &&  chkKnjs(de->name))
			return 0;
		if (ccChk ==  2 && !strchr(de->name,'\\'))
			return 0;
		if (ccChk == -2 &&  strchr(de->name,'\\'))
			return 0;
	}

	return 1;
}

int AbxFiles::chkKnjs(const char *p)
{
	unsigned char c;
	while((c = *(unsigned char *)p++) != 0) {
	    if (c & 0x80)
	    	return 1;
	}
	return 0;
}

int AbxFiles::dirMatchCheck(void* opts0, Fks_DirEnt const* de)
{
	AbxFiles_Opts const& opts = *(AbxFiles_Opts*)opts0;

	return 1;
}

namespace {

static int 	s_direction;
static bool s_icase;
static int 	s_directory;

bool nameDegitCmp(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) {
	if (s_directory) {
		int d = FKS_S_ISDIR(l->stat->st_mode) - FKS_S_ISDIR(r->stat->st_mode);
    	if (d)
    		return d * s_direction > 0;
	}
 #ifdef	USE_LOWER_CMP	//TODO: ignore case cmp for linux,unix .
    int rc = (s_icase) ? fks_pathLowerDigitCmp(l->path, r->path) : fks_pathDigitCmp(l->path, r->path);
 #else
    int rc = fks_pathDigitCmp(l->path, r->path);
 #endif
    return rc * s_direction < 0;
}

bool nameCmp(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) {
	if (s_directory) {
		int d = FKS_S_ISDIR(l->stat->st_mode) - FKS_S_ISDIR(r->stat->st_mode);
    	if (d)
    		return d * s_direction > 0;
	}
 #ifdef	USE_LOWER_CMP	//TODO: ignore case cmp for linux,unix .
    int rc = (s_icase) ? fks_pathLowerCmp(l->path, r->path) : fks_pathCmp(l->path, r->path);
 #else
    return fks_pathCmp(l->path, r->path) * s_direction < 0;
 #endif
}

bool extCmp(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) {
	int n = fks_pathCmp(fks_pathExt(l->path), fks_pathExt(r->path));
	if (n)
		return (s_direction * n) < 0;
	return nameDegitCmp(l, r);
}

bool sizeCmp(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) {
	fks_isize_t  d = l->stat->st_size - r->stat->st_size;
	if (d)
		return (s_direction * d) < 0;
	return nameDegitCmp(l, r);
}

bool dateCmp(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) {
	fks_isize_t  d = l->stat->st_mtime - r->stat->st_mtime;
	if (d)
		return (s_direction * d) < 0;
	return nameDegitCmp(l, r);
}

static bool attrCmp(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) {
 #ifdef FKS_WIN32
	int  la = l->stat->st_native_attr;
	int  ra = r->stat->st_native_attr;
	int  d  = la - ra;
	if (d)
		return (d * s_direction) > 0;
 #else
	int  d = l->stat->st_mode - r->stat->st_mode;
	if (d)
		return (d * s_direction) > 0;
 #endif
	return nameDegitCmp(l, r);
}

typedef bool (*cmp_t)(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r);

}	// namespace


void AbxFiles::sortPartStats(SortType sortType, bool sortRevFlg, bool sortICaseFlg, bool sortDirFlg)
{
	s_direction = sortRevFlg ? -1 : 1;
	s_icase 	= sortICaseFlg;
	s_directory = sortDirFlg;
	cmp_t	cmp;
	switch (sortType) {
	default:
	case ST_NUM :	cmp = nameDegitCmp; break;
	case ST_NAME:	cmp = nameCmp;		break;
	case ST_EXT :	cmp = extCmp;		break;
	case ST_SIZE:	cmp = sizeCmp;		break;
	case ST_DATE:	cmp = dateCmp;		break;
	case ST_ATTR:	cmp = attrCmp;		break;
	}
	std::sort(pathStats_.begin(), pathStats_.end(), cmp);
}
