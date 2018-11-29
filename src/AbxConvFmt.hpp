#ifndef AbxConvFmt_HPP
#define AbxConvFmt_HPP

#include "subr.hpp"

class ConvFmt {
public:
    enum { VAR_NMSZ = FIL_NMSZ };
    ConvFmt();

    char const* tmpDir() const { return tmpDir_.c_str(); }
    char const* getVar(unsigned n) const {
    	return (n < 10) ? var_[n].c_str() : "";
    }

    void setTgtnmFmt(char const* tgt) { tgtnmFmt_ = tgt; }
    void setNum(FKS_ULLONG num) { num_ = num; }
    void setUpLwrFlag(bool sw) { upLwrFlg_ = sw; }
    void setAutoWq(bool sw) { autoWqFlg_ = sw; }
    void setFmtStr(char const* fmtBuf) { fmtBuf_ = fmtBuf; }
	void setCurDir(char const* dir) { curDir_ = dir; }
	void setRelativeBaseDir(char const* dir);

    StrList&	    outBuf() { return outBuf_; }
    StrList const&  outBuf() const { return outBuf_; }
    void    	    clearOutBuf() { StrList().swap(outBuf_); }

	void  setChgPathDir(char const* dir);
	void  setTmpDir(char const* dir);
	void  clearVar();
	bool  setVar(unsigned m, char const* p, size_t l);
	int   write(char const* fpath, struct fks_stat const* st);
	int   writeLine0(char const* s);

private:
	bool  setTgtNameAndCheck(struct fks_stat const* st);
	char* initPathStrings(char const* fpath0);
	void  StrFmt(char *dst, char const* fmt, size_t sz, struct fks_stat const* st);
	char* stpCpy(char *d, char const* s, ptrdiff_t clm, int upLow);
	void  changeSep(char* d, int sepMode);
	char* changeRelative(char* d);

private:
    bool    	    	upLwrFlg_;
    bool    	    	autoWqFlg_; 	/* $f等で自動で両端に"を付加するモード. */
    FKS_ULLONG 	    	num_;	    	/* $i で生成する番号 */
    FKS_ULLONG 	    	numEnd_;    	/* 連番をファイル名の文字列の代わりにする指定をした場合の終了アドレス */
    char const*     	fmtBuf_;    	/* 変換文字列を収める */
    char const*     	lineBuf_;
    FnameBuf	    	var_[10];
	FnameBuf			fullpath_;
    FnameBuf	    	drv_;
    FnameBuf	    	dir_;
    FnameBuf	    	name_;
    FnameBuf	    	ext_;
    FnameBuf	    	curDir_;
    FnameBuf			relativeBaseDir_;
    FnameBuf	    	tmpDir_;
    FnameBuf	    	pathDir_;
    FnameBuf	    	chgPathDir_;
    FnameBuf	    	tgtnm_;
    FnameBuf	    	tgtnmFmt_;
    StrzBuf<OBUFSIZ>	obuf_;	    	/* .cfg(.res) 読み込みや、出力用のバッファ */

    StrList 	    	outBuf_;
};


#endif
