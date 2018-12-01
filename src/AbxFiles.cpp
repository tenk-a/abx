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


AbxFiles_Opts const*	AbxFiles::opts_;



bool AbxFiles::getPathStats(StrList& filenameList, AbxFiles_Opts const& opts)
{
	opts_ = &opts;
	int flags = 0;
	if (opts.recFlg_)
		flags |= FKS_DE_Recursive;
	if (!(opts.fattr_ & FA_Dir))
		flags |= FKS_DE_FileOnly;
	else if (!(opts.fattr_ & FA_MASK_FILEYTPE))
		flags |= FKS_DE_DirOnly;

	Fks_DirEnt_IsMatchCB	isMatch = NULL;
	if ( (opts.fattr_ && !(flags & FKS_DE_DirOnly)) // ((opts.fattr_ & FA_Dir) == 0)
		|| (opts.szmin_ <= opts.szmax_)
		|| (opts.dtmin_ <= opts.dtmax_)
		|| (opts.knjChk_)
	){
		isMatch = &matchCheck;
	}

	FnameBuf fnmWk;
	ipath_			= opts.ipath_;
	char*	iname	= fks_pathBaseName(&ipath_[0]);
	pathStatBody_.reserve(filenameList.size());
	for (StrList::iterator ite = filenameList.begin(); ite != filenameList.end(); ++ite) {
    	char const* p = ite->c_str();
    	    *iname  = '\0';
    	    ipath_  += p;
    	    p       = &ipath_[0];
    	if (!fks_pathIsAbs(p)) {	// relative path?
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
		Fks_DirEntPathStat*	pPathStats = NULL;
		fks_isize_t			n = fks_createDirEntPathStats(&pPathStats, NULL, fname, flags, isMatch);
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
		sortPartStats(opts.sortType_, opts.sortRevFlg_, opts.sortLwrFlg_);
	}
	return true;
}


int AbxFiles::matchCheck(Fks_DirEnt const* de)
{
	AbxFiles_Opts const& opts = *opts_;

 #ifdef FKS_WIN32
	unsigned    fattr = opts.fattr_;
	if (fattr & (FA_MASK_FILEYTPE|FA_Dir)) {
		unsigned	w32attr = de->stat->st_native_attr;
		w32attr				= FKS_S_W32ATTR(w32attr);
		//printf(">>> %04x & %04x = %04x\n", fattr, w32attr, fattr & w32attr);
		if ((fattr & w32attr) == 0)
			return 0;
	}
 #endif
 	if (opts.szmin_ < opts.szmax_) {
		if (de->stat->st_size < opts.szmin_ || opts.szmax_ < de->stat->st_size)
			return 0;
	}
	if (opts.dtmin_ < opts.dtmax_) {
		if (de->stat->st_mtime < opts.dtmin_ || opts.dtmax_ < de->stat->st_mtime)
			return 0;
	}

	int knjChk = opts.knjChk_;
	if (knjChk) {
		if (knjChk ==  1 && !chkKnjs(de->name))
			return 0;
		if (knjChk == -1 &&  chkKnjs(de->name))
			return 0;
		if (knjChk ==  2 && !strchr(de->name,'\\'))
			return 0;
		if (knjChk == -2 &&  strchr(de->name,'\\'))
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


namespace {

/// 数字部分は数値で比較する名前比較 .
struct NameDigitCmp {
	int 	dir_;
	NameDigitCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
	}
};

/// 名前比較 .
struct NameCmp {
	int 	dir_;
	NameCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
	    return fks_pathCmp(l->path, r->path) * dir_ < 0;
	}
};

/// 拡張子比較 .
struct ExtCmp {
	int 	dir_;
	ExtCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
		int n = fks_pathCmp(fks_pathExt(l->path), fks_pathExt(r->path));
		if (n)
			return (dir_ * n) < 0;
	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
	}
};

/// サイズ比較 .
struct SizeCmp {
	int 	dir_;
	SizeCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
		fks_isize_t  d = l->stat->st_size - r->stat->st_size;
		if (d)
			return (dir_ * d) < 0;
	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
	}
};

/// 日付比較 .
struct DateCmp {
	int 	dir_;
	DateCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
		fks_isize_t  d = l->stat->st_mtime - r->stat->st_mtime;
		if (d)
			return (dir_ * d) < 0;
		return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
	}
};

/// 属性比較. ほぼほぼ win 用 .
struct AttrCmp {
	int 	dir_;
	AttrCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
	 #if FKS_WIN32
	    /* アーカイブ属性は邪魔なのでオフする */
		int  la = FKS_S_W32ATTR(l->stat->st_native_attr) & FA_MASK_NOARC;
		int  ra = FKS_S_W32ATTR(r->stat->st_native_attr) & FA_MASK_NOARC;
		int  d  = la - ra;
	 #else
		int  d = l->stat->st_mode - r->stat->st_mode;
	 #endif
		if (d)
			return (dir_ * d) < 0;
	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
	}
};

//#define USE_LOWER_CMP	//TODO: 比較処理実装 .
#ifdef	USE_LOWER_CMP	//TODO: linux,unix で 小文字化ソートしたい場合用 .
// 数字部分は数値で比較する名前比較 .
/// 名前小文字列化比較 .
struct LowerNumCmp {
	int 	dir_;
	LowerNumCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
		int n = fks_pathDigitLowerCmp(l->path, r->path);
		if (n == 0)
			n = fks_pathDigitCmp(l->path, r->path);
	    return n * dir_ < 0;
	}
}

/// 名前小文字列化比較 .
struct LowerNameCmp {
	int 	dir_;
	LowerNameCmp(int dir) : dir_(dir) {}
	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
		int n = fks_pathLowerCmp(l->path, r->path);
		if (n == 0)
			n = fks_pathCmp(l->path, r->path);
	    return n * dir_ < 0;
	}
}
#endif

}	// namespace


void AbxFiles::sortPartStats(SortType sortType, bool sortRevFlg, bool sortLwrFlg)
{
	int dir = sortRevFlg ? -1 : 1;
	switch (sortType) {
	default:
	case ST_NUM :
	 #ifdef USE_LOWER_CMP
		if (sortLwrFlg)
			std::sort(pathStats_.begin(), pathStats_.end(), LowerNumCmp(dir));
		else
	 #endif
			std::sort(pathStats_.begin(), pathStats_.end(), NameDigitCmp(dir));
		break;
	case ST_NAME:
	 #ifdef USE_LOWER_CMP
		if (sortLwrFlg)
			std::sort(pathStats_.begin(), pathStats_.end(), LowerNameCmp(dir));
		else
	 #endif
			std::sort(pathStats_.begin(), pathStats_.end(), NameCmp(dir));
		break;
	case ST_EXT :
		std::sort(pathStats_.begin(), pathStats_.end(), ExtCmp(dir));
		break;
	case ST_SIZE:
		std::sort(pathStats_.begin(), pathStats_.end(), SizeCmp(dir));
		break;
	case ST_DATE:
		std::sort(pathStats_.begin(), pathStats_.end(), DateCmp(dir));
		break;
	case ST_ATTR:
		std::sort(pathStats_.begin(), pathStats_.end(), AttrCmp(dir));
		break;
	}
}
