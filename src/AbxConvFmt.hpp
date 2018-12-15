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
    void setAddDoubleQuotation(bool sw) { autoWqFlg_ = sw; }
    void setFmtStr(char const* fmtBuf) { fmtBuf_ = fmtBuf; }
	void setCurDir(char const* dir) { curDir_ = dir; }
	void setRelativeBaseDir(char const* dir);
	void setOdrCh(char c) { odrCh_ = c; }
	char odrCh() const { return odrCh_; }
	void setRelativePathMode(bool mode) { relativePathMode_ = mode; }
	void setSepMode(int mode) { defaultSepMode_ = mode; }
	void setUpLowMode(int mode) { defaultUpLowMode_ = mode; }
	void setNoFindFile(bool sw) { noFindFile_ = sw; }
	void setChgPathDir(char const* dir);
	void setTmpDir(char const* dir);
	bool setVar(unsigned m, char const* p, size_t l);
	void clearVar();

	int  write(char const* fpath, struct fks_stat_t const* st);
	int  writeLine0(char const* s);

    StrList&	    outBufList() { return outStrList_; }
    StrList const&  outBufList() const { return outStrList_; }
    void    	    clearOutBufList() { StrList().swap(outStrList_); }
	char const*		currentOutBuf() const { return &outBuf_[0]; }

private:
	bool  setTgtNameAndCheck(struct fks_stat_t const* st);
	void  strFmt(char *dst, size_t dstSz, char const* fmt, struct fks_stat_t const* st);
	char* stpCpy(char *d, char* d_end, char const* s, ptrdiff_t clm, int upLow);
	void  changeSep(char* d, int sepMode);
	char* changeRelative(char* d, char* d_end);
	char  checkOdrCh(char const* s);
	char* strFmtSize(char* d, char* de, int64_t size, int clm, bool mode);

private:
	bool				noFindFile_;
    bool    	    	ignoreCaseFlag_;	// ignore case flag
    bool    	    	autoWqFlg_; 		// Automatically add '" ' to both ends with $f etc.
	bool				first_;
	bool				recursiveFlg_;
	bool				relativePathMode_;
	uint8_t				defaultSepMode_;
	int8_t				defaultUpLowMode_;
	char				odrCh_;				// '$' or '@'
    FKS_ULLONG 	    	num_;	    		// Counter for $i
    FKS_ULLONG 	    	numEnd_;    		// End number when serial number is used instead of file.
    char const*     	fmtBuf_;    		// format string buffer.
    char const*     	rawStr_;			// Raw input string.
    FPathBuf	    	var_[10];			// $0..$9 variable
	FPathBuf			fullpath_;			// Fullpath string.
	//FPathBuf			relativePath_;		// Relative path string.
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
