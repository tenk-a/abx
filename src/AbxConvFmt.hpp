#ifndef AbxConvFmt_HPP
#define AbxConvFmt_HPP

#include "subr.hpp"

class ConvFmt {
public:
    enum { VAR_NMSZ = FPATH_SIZE };
    ConvFmt();

    char const* tmpDir() const { return tmpDir_.c_str(); }
    char const* getVar(unsigned n) const {
    	return (n < 10) ? var_[n].c_str() : "";
    }

    void setTargetNameFmt(char const* tgt) { targetPathFmt_ = tgt; }
    void setNum(FKS_ULLONG num) { num_ = num; }
    void setIgnoreCaseFlag(bool sw) { ignoreCaseFlag_ = sw; }
    void setRecursiveDirFlag(bool sw) { recursiveFlg_ = sw; }
    void setAutoWq(bool sw) { autoWqFlg_ = sw; }
    void setFmtStr(char const* fmtBuf) { fmtBuf_ = fmtBuf; }
	void setCurDir(char const* dir) { curDir_ = dir; }
	void setRelativeBaseDir(char const* dir);

    StrList&	    outBuf() { return outStrList_; }
    StrList const&  outBuf() const { return outStrList_; }
    void    	    clearOutBuf() { StrList().swap(outStrList_); }

	void  setChgPathDir(char const* dir);
	void  setTmpDir(char const* dir);
	void  clearVar();
	bool  setVar(unsigned m, char const* p, size_t l);
	int   write(char const* fpath, struct fks_stat const* st);
	int   writeLine0(char const* s);

private:
	bool  setTgtNameAndCheck(struct fks_stat const* st);
	void  strFmt(char *dst, size_t dstSz, char const* fmt, struct fks_stat const* st);
	char* stpCpy(char *d, char* d_end, char const* s, ptrdiff_t clm, int upLow);
	void  changeSep(char* d, int sepMode);
	char* changeRelative(char* d, char* d_end);

private:
    bool    	    	ignoreCaseFlag_;	// ignore case flag
    bool    	    	autoWqFlg_; 		// Automatically add '" ' to both ends with $f etc.
	bool				first_;
	bool				recursiveFlg_;
    FKS_ULLONG 	    	num_;	    		// Counter for $i
    FKS_ULLONG 	    	numEnd_;    		// End number when serial number is used instead of file.
    char const*     	fmtBuf_;    		// format string buffer.
    char const*     	rawStr_;			// Raw input string.
    FPathBuf	    	var_[10];			// $0..$9 variable
	FPathBuf			fullpath_;			// Fullpath string.
    FPathBuf	    	drive_;				// Drive string.
    FPathBuf	    	dir_;				// Directory string.
    FPathBuf	    	name_;				// filename string.
    FPathBuf	    	ext_;				// Extension string.
    FPathBuf	    	curDir_;			// Current directory string.
    FPathBuf			relativeBaseDir_;	// Base directory for relative path.
    FPathBuf	    	tmpDir_;			// Temporary directory.
    FPathBuf	    	pathDir_;			// Drive + directory.
    FPathBuf	    	chgPathDir_;		// Change pathDir_
    FPathBuf	    	targetPath_;		// target path
    FPathBuf	    	targetPathFmt_;		// target path format
    StrzBuf<OBUFSIZ>	outBuf_;	   		// output buffer (or read .cfg(.res))

    StrList 	    	outStrList_;
};


#endif
