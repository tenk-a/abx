/**
 *  @file   AbxConvFmt.cpp
 *  @brief  convert formated path string
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */

#include <stddef.h>
#include <fks_common.h>
#include <fks_path.h>
#include <fks_io.h>
#include <fks_time.h>
#include <stdio.h>
#include <stdlib.h>

#include "AbxConvFmt.hpp"
#include "AbxMsgStr.hpp"
#include "subr.hpp"

#define PUT_C(p,pe,c)		do {if ((p) < (pe)) *(p)++ = (c); } while (0)


ConvFmt::ConvFmt()
	: ignoreCaseFlag_(false)
	, autoWqFlg_(false)
	, first_(true)
	, recursiveFlg_(false)
	, odrCh_('\0')
	, num_(0)
	, numEnd_(0)
	, fmtBuf_(NULL)
	, rawStr_(NULL)
	, drive_()
	, dir_()
	, name_()
	, ext_()
	, curDir_()
	, tmpDir_()
	, pathDir_()
	, chgPathDir_()
	, targetPath_()
	, targetPathFmt_()
	, outBuf_()
	, outStrList_()
	//, var_()
{
	setTmpDir(NULL);
}

void ConvFmt::setChgPathDir(char const* dir) {
	fks_fileFullpath(&chgPathDir_[0], chgPathDir_.capacity(), dir);
	char* p = STREND(&chgPathDir_[0]);
	if (fks_pathIsSep(p[-1])) {
	    p[-1] = '\0';
	}
}

void ConvFmt::setTmpDir(char const* dir) {
	FPathBuf	tmp;
	if (dir == NULL || dir[0] == 0) {
	    dir = &tmp[0];
	 #ifdef FKS_WIN32
	    fks_getTmpEnv(&tmp[0], tmp.capacity());
	    fks_pathDelLastSep(&tmp[0]);
	 #else
	 	fks_pathCpy(&tmp[0], tmp.capacity(), "/tmp");
	 #endif
	}
    fks_fileFullpath(&tmpDir_[0], tmpDir_.capacity(), dir);
    fks_pathDelLastSep(&tmpDir_[0]);
}

void ConvFmt::clearVar() {
	for (int l = 0; l < 10; l++)
	    var_[l].clear();
}

bool ConvFmt::setVar(unsigned m, char const* p, size_t l) {
	if (l < 1 || l >= (var_[0].capacity() - 1))
	    return false;
	var_[m].assign(p, p + l);
	return true;
}

void ConvFmt::setRelativeBaseDir(char const* dir) {
	fks_fileFullpath(&relativeBaseDir_[0], relativeBaseDir_.capacity(), dir);
}

int ConvFmt::write(char const* fpath, fks_stat_t const* st) {
	rawStr_  = fpath;	// non filename for $l

	if (curDir_.empty())
		fks_getcwd(&curDir_[0], curDir_.capacity());
	if (relativeBaseDir_.empty())
		relativeBaseDir_ = curDir_;

	if ((st->st_ex_mode & FKS_S_EX_DOTORDOTDOT) && !recursiveFlg_) {
		fks_pathCpy(&name_[0], name_.capacity(), fpath);
		fks_pathDelBaseName(&name_[0]);
		fks_pathCpy(&name_[0], name_.capacity(), ".");
		char*	fullpath = fks_fileFullpath(&fullpath_[0], fullpath_.capacity(), &name_[0]);
		fks_pathDelBaseName(fullpath);
		fks_pathGetDrive(&drive_[0], drive_.capacity(), fullpath);
		fks_pathCpy(&dir_[0], dir_.capacity(), fullpath);
		fks_pathDelLastSep(&dir_[0]);
		char const* b = fks_pathBaseName(fpath);
		fks_pathCombine(fullpath, fullpath_.capacity(), b);
		fks_pathCpy(&name_[0], name_.capacity(), b);
		ext_[0] = 0;
	} else {
		char*	fullpath = fks_fileFullpath(&fullpath_[0], fullpath_.capacity(), fpath);
		fks_pathGetDrive(&drive_[0], drive_.capacity(), fullpath);
		fks_pathGetDir(&dir_[0], dir_.capacity(), fullpath);
		fks_pathDelLastSep(&dir_[0]);
		fks_pathGetBaseNameNoExt(&name_[0], name_.capacity(), fks_pathBaseName(fullpath));
		fks_pathGetNoDotExt(&ext_[0], ext_.capacity(), fullpath);
	}

	pathDir_ = drive_;
	pathDir_ += dir_;
	if (!chgPathDir_.empty())
		pathDir_ = chgPathDir_;

	bool ok = setTgtNameAndCheck(st);

	if (ok) {
	    strFmt(&outBuf_[0], outBuf_.capacity(), &fmtBuf_[0], st);
	    outStrList_.push_back(&outBuf_[0]);
	    first_ = false;
	}
	++num_;
	return 0;
}

bool ConvFmt::setTgtNameAndCheck(fks_stat_t const* st) {
	if (targetPathFmt_.empty())
		return true;	// If there is no target, update.
	fks_stat_t tgtSt = {0};
	strFmt(&targetPath_[0], targetPath_.capacity(), &targetPathFmt_[0], &tgtSt);
	if (fks_stat(&targetPath_[0], &tgtSt) < 0)
		return true;	// If there is no target file, update.
	fks_time_t	ltm  = tgtSt.st_mtime;
	fks_time_t  rtm  = st->st_mtime;
	if (ltm ==  0 || rtm == 0)
		return true;	// If there is no date of either file, update.
	return ltm < rtm;	// If the target date is old, update.
}

char ConvFmt::checkOdrCh(char const* s) {
	if (odrCh_ != '\0')
		return odrCh_;
	char c;
	while ((c = *s++) != '\0') {
		if (c == '$')
			return c;
		else if (c == '@')
			return c;
	}
 #ifdef FKS_WIN32
 	return '$';
 #else
 	return '@';
 #endif
}

int ConvFmt::writeLine0(char const* s) {
	char* d  = &outBuf_[0];
	char* de = d + outBuf_.capacity() - 1;
	char c;
	char odrCh = checkOdrCh(s);
	while ((c = (*d++ = *s++)) != '\0') {
	    if (c == odrCh) {
	    	--d;
	    	c = *s++;
	    	if (c == odrCh) {
				PUT_C(d, de, odrCh);
	    	} else if (c >= '1' && c <= '9') {
				fks_pathCpy(d, de - d, &var_[c-'0'][0]);
	    	    d = STREND(d);
	    	} else {
	    	    //fprintf(stderr, ABXMSG(incorrect_dollar_format), c);
	    	    //exit(1);
				PUT_C(d, de, odrCh);
				PUT_C(d, de, c);
	    	}
	    }
	}
 #if 1
	strcat(&outBuf_[0], "\n");
	outStrList_.push_back(&outBuf_[0]);
 #else
	fprintf(fp, "%s\n", &outBuf_[0]);
 #endif
	return 0;
}

void ConvFmt::strFmt(char *dst, size_t dstSz, char const* fmt, fks_stat_t const* st) {
	enum { buf_sz = FPATH_SIZE*2 + 15 };
	char		buf[buf_sz + 1] = {0};
	char*		b  = &buf[0];
	char*		be = b + buf_sz;
	char*		tb = b;
	char		drv[2];
	drv[0] = drive_[0];
	drv[1] = 0;

	char const* s  = fmt;
	char*	    d  = dst;
	char*		td = d;
	char*	    q = NULL;
	char*	    de = d + dstSz - 1;
	char 		odrCh = checkOdrCh(s);
	char	    c;
	int 		n;
	while ((c = (*d = *s)) != '\0' && d < de) {
	    if (c == odrCh) {
			char const* sav_s = s;
			char const* ss;
			++s;
	    	bool relative = false;
	    	int  uplow    	= 0;
	    	int  sepMode	= 0;	// 1=to '/'  2=to '\\'
	    	n = -1;
	    	c = *s++;
	    	if (c == '+') { // +NN
	    	    n = strtoul(s,(char**)&ss,10);
	    	    if (!ss)
	    	    	goto SKIP_S;
		    	s = ss;
		    	if (*s == '\0')
	    	    	goto SKIP_S;
	    	    if (n >= FPATH_SIZE)
	    	    	n = FPATH_SIZE;
	    	    c = *s++;
	    	}
			while (c) {
    	    	if (c == 'R') {
					relative = true;
					c = *s++;
    	    	} else if (c == 'F') {
					relative = false;
					c = *s++;
				} else if (c == 'U') {
					uplow = 1;
					c = *s++;
				} else if (c == 'L') {	// u
					uplow = -1;
					c = *s++;
				} else if (c == 'B') {
					sepMode = 2;
					c = *s++;
				} else if (c == 'b') {
					sepMode = 1;
					c = *s++;
				} else {
					break;
				}
			}
	    	switch (c) {
	    	case 's':   PUT_C(d,de,' ');     break;
	    	case 't':   PUT_C(d,de,'\t');    break;
	    	case 'n':   PUT_C(d,de,'\n');    break;
	    	case '$':   PUT_C(d,de,'$');     break;
	    	case '@':   PUT_C(d,de,'@');     break;
	    	case '#':   PUT_C(d,de,'#');     break;
	    	case '[':   PUT_C(d,de,'<');     break;
	    	case ']':   PUT_C(d,de,'>');     break;
	    	case '`':   PUT_C(d,de,'\'');    break;
	    	case '^':   PUT_C(d,de,'"');     break;

	    	case 'l':   d = stpCpy(d, de, rawStr_, n, uplow);	break;
	    	case 'v':   d = stpCpy(d, de, drv, n, uplow);		break;

	    	case 'd':
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    d = stpCpy(d, de, &dir_[0], n, uplow);
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    *d = 0;
	    	    break;

	    	case 'D':
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    q = fks_pathBaseName(&dir_[0]);
	    	    d = stpCpy(d, de, q, n, uplow);
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    *d = 0;
	    	    break;

	    	case 'x':
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    d = stpCpy(d,de, &name_[0], n, uplow);
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    *d = 0;
	    	    break;

	    	case 'e':
	    	    d = stpCpy(d, de, &ext_[0], n, uplow);
	    	    break;

	    	case 'w':
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
				td = d;
	    	    d = stpCpy(d, de, &tmpDir_[0], n, uplow);
	    	    if (relative) d = changeRelative(td, de);
	    	    if (sepMode)  changeSep(td, sepMode);
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    *d = 0;
	    	    break;

	    	case 'p':
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
				td = d;
	    	    d = stpCpy(d, de, &pathDir_[0], n, uplow);
	    	    if (relative) d = changeRelative(td, de);
	    	    if (sepMode)  changeSep(td, sepMode);
	    	    if (autoWqFlg_) PUT_C(d,de,'"');
	    	    *d = 0;
	    	    break;

	    	case 'c':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
	    	    b = stpCpy(b, be, &name_[0], 0, uplow);
	    	    if (!ext_.empty()) {
					fks_pathCpy(b, be - b, ".");
					b = STREND(b);
	    	    	b = stpCpy(b, be, &ext_[0], 0, uplow);
	    	    }
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = 0;
	    	    if (n < 0) n = 1;
	    	    d += snprintf(d, de-d, "%-*s", n, buf);
	    	    break;

	    	case 'f':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
				tb = b;
	    	    b = stpCpy(b, be, &fullpath_[0], 0, uplow);
	    	    if (relative) b = changeRelative(tb, be);
	    	    if (sepMode)  changeSep(tb, sepMode);
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = 0;
	    	    if (n < 0) n = 1;
	    	    d += snprintf(d, de-d, "%-*s", n, buf);
	    	    break;

	    	case 'g':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
				tb = b;
	    	    stpCpy(tb, be, &fullpath_[0], 0, uplow);
	    	    b = fks_pathExt(tb);
	    	    if (b)
	    	    	*b = '\0';
	    	    if (relative) b = changeRelative(tb, be);
	    	    if (sepMode)  changeSep(tb, sepMode);
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = '\0';
	    	    if (n < 0)
	    	    	n = 1;
	    	    else if (n > de-d-1)
	    	    	n = de-d-1;
	    	    d += snprintf(d, de-d, "%-*s", n, buf);
	    	    break;

	    	case 'o':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
				tb = b;
	    	    b = stpCpy(b, be, &targetPath_[0], 0, uplow);
	    	    if (relative && fks_pathIsAbs(tb)) b = changeRelative(tb, be);
	    	    if (sepMode)  changeSep(tb, sepMode);
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = '\0';
	    	    if (n < 0)
	    	    	n = 1;
	    	    else if (n > de-d-1)
	    	    	n = de-d-1;
	    	    d += snprintf(d, de-d, "%-*s", n, buf);
	    	    break;

	    	case 'z':
    	    	if (n < 0)
    	    	    n = 10;
	    	    else if (n > de-d-1)
	    	    	n = de-d-1;
				if (sepMode)
					d = strFmtSize(d, de, st->st_size, n, sepMode == 2);
				else
    	    		d += snprintf(d, de-d, "%*" PRIF_LL "d", n, (PRIF_LLONG)st->st_size);
    	    	break;

	    	case 'Z':
    	    	if (n < 0)
    	    	    n = 8;
	    	    else if (n > de-d-1)
	    	    	n = de-d-1;
    	    	d += snprintf(d, de-d, "%*" PRIF_LL "X", n, (PRIF_ULLONG)st->st_size);
	    	    break;

	    	case 'i':
    	    	if (n < 0)
    	    	    n = 1;
	    	    else if (n > de-d-1)
	    	    	n = de-d-1;
    	    	d += snprintf(d, de-d, "%0*" PRIF_LL "d", n, (PRIF_LLONG)(num_));
	    	    break;

	    	case 'I':
	    	    if (n < 0)
	    	    	n = 1;
	    	    else if (n > de-d-1)
	    	    	n = de-d-1;
    	    	d += snprintf(d, de-d, "%0*" PRIF_LL "X", n, (PRIF_ULLONG)(num_));
	    	    break;

	    	case 'j':
	    	case 'J':
	    	    {
					Fks_DateTime	dt = {0};
					fks_fileTimeToLocalDateTime(st->st_mtime, &dt);
	    	    	if (n < 0)
	    	    	    n = 10;
	    	    	if (n < 8) {			// 5
	    	    	    snprintf(buf, buf_sz, "%02d-%02d", dt.month, dt.day);
					} else if (n < 10) {	// 8
	    	    	    snprintf(buf, buf_sz, "%02d-%02d-%02d", dt.year%100, dt.month, dt.day);
					} else if (n < 13) {	// 10
	    	    	    snprintf(buf, buf_sz, "%04d-%02d-%02d", dt.year, dt.month, dt.day);
					} else if (n < 16) {	// 13
	    	    	    snprintf(buf, buf_sz, "%04d-%02d-%02d %02d", dt.year, dt.month, dt.day, dt.hour);
					} else if (n < 19) {	// 16
	    	    	    snprintf(buf, buf_sz, "%04d-%02d-%02d %02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute);
					} else if (n < 21) {	// 19
	    	    	    snprintf(buf, buf_sz, "%04d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
					} else if (n < 22) {	// 21
	    	    	    snprintf(buf, buf_sz, "%04d-%02d-%02d %02d:%02d:%02d.%1d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, ((dt.milliSeconds+49) / 100)%10);
					} else if (n < 23) {	// 22
	    	    	    snprintf(buf, buf_sz, "%04d-%02d-%02d %02d:%02d:%02d.%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, ((dt.milliSeconds+5) / 10)%100);
					} else {				// 23
	    	    	    snprintf(buf, buf_sz, "%04d-%02d-%02d %02d:%02d:%02d.%03d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second,dt.milliSeconds%1000);
					}
					if (c == 'J') {
						char* t = buf;
						while (*t) {
							if (*t == ' ') 		*t = '_';
							else if (*t == '-') *t = '_';
							else if (*t == ':') *t = '_';
							else if (*t == '.') *t = '_';
							++t;
						}
					}
		    	    if (n > de-d-1)
		    	    	n = de-d-1;
	    	    	d += snprintf(d, de-d, "%-*s", n, buf);
	    	    }
	    	    break;

			case 'a':
			  #ifdef _WIN32
			  	if (sepMode == 0 || sepMode == 2) {
		    	    if (n < 1) n = 14; //9;
				  	unsigned a = st->st_native_attr;
				  	b = buf;
				 #if 1
					if (a & FKS_S_W32_ReadOnly) 		*b++ = 'r';	else *b++='-';
					if (a & FKS_S_W32_Hidden) 			*b++ = 'h';	else *b++='-';
					if (a & FKS_S_W32_System)    		*b++ = 's';	else *b++='-';
					if (a & FKS_S_W32_Directory)		*b++ = 'd';	else *b++='-';
					if (a & FKS_S_W32_Archive) 			*b++ = 'a';	else *b++='-';
					//if (a & FKS_S_W32_SparseFile)		*b++ = 'P';	else *b++='-';
					if (a & FKS_S_W32_ReparsePoint) 	*b++ = 'l';	else *b++='-';
					if (a & FKS_S_W32_Compressed)   	*b++ = 'c';	else *b++='-';
					if (a & FKS_S_W32_Offline) 			*b++ = 'o';	else *b++='-';
					if (a & FKS_S_W32_NoIndexed)		*b++ = 'i';	else *b++='-';
					if (a & FKS_S_W32_Encrypted) 		*b++ = 'e';	else *b++='-';
					if (a & FKS_S_W32_IntegritySystem) 	*b++ = 'v';	else *b++='-';
					if (a & FKS_S_W32_NoScrubData)		*b++ = 'x';	else *b++='-';
					if (a & FKS_S_W32_Pinned)			*b++ = 'p';	else *b++='-';
					if (a & FKS_S_W32_Unpinned)			*b++ = 'u';	else *b++='-';
				 #else
					if (a & FKS_S_W32_ReadOnly) 		*b++ = 'r';	else *b++='-';
					if (a & FKS_S_W32_Hidden) 			*b++ = 'h';	else *b++='-';
					if (a & FKS_S_W32_System)    		*b++ = 's';	else *b++='-';
					if (a & FKS_S_W32_Directory)		*b++ = 'd';	else *b++='-';
					if (a & FKS_S_W32_Archive) 			*b++ = 'a';	else *b++='-';
					if (a & FKS_S_W32_ReparsePoint) 	*b++ = 'l';	else *b++='-';
					if (a & FKS_S_W32_Compressed)   	*b++ = 'c';	else *b++='-';
					if (a & FKS_S_W32_NoIndexed)		*b++ = 'i';	else *b++='-';
					if (a & FKS_S_W32_Encrypted) 		*b++ = 'e';	else *b++='-';
				 #endif
					*b = 0;
					if (n < 9)
						buf[n] = 0;
		    	    if (n > de-d-1)
		    	    	n = de-d-1;
	    	    	d += snprintf(d, de-d, "%-*s", n, buf);
	    	    } else
			  #endif
	    	    {
		    	    if (n < 1) n = 10;
				  	unsigned a = st->st_mode;
				  	b = buf;

					if ((a & FKS_S_IFMT) == FKS_S_IFLNK)		*b++ = 'l';
					else if ((a & FKS_S_IFMT) == FKS_S_IFDIR)	*b++ = 'd';
					else 										*b++ = '-';
					if (a & (1 << 8))			*b++ = 'r'; else *b++ = '-';
					if (a & (1 << 7))			*b++ = 'w'; else *b++ = '-';
					if (a & (1 << 6))			*b++ = 'x'; else *b++ = '-';
					if (a & (1 << 5))			*b++ = 'r'; else *b++ = '-';
					if (a & (1 << 4))			*b++ = 'w'; else *b++ = '-';
					if (a & (1 << 3))			*b++ = 'x'; else *b++ = '-';
					if (a & (1 << 2))			*b++ = 'r'; else *b++ = '-';
					if (a & (1 << 1))			*b++ = 'w'; else *b++ = '-';
					if (a & (1 << 0))			*b++ = 'x'; else *b++ = '-';

					*b = 0;
		    	    if (n > de-d-1)
		    	    	n = de-d-1;
	    	    	d += snprintf(d, de-d, "%-*s", n, buf);
				}
				break;

			case 'A':
			  #ifdef _WIN32
			  	if (sepMode == 2) {
		    	    if (n < 1) n = 32; //8;
				  	unsigned a = st->st_native_attr;
					//a = FKS_S_W32ATTR(a);
				  	b = buf;
					if (a & FKS_S_W32_ReadOnly) 		*b++ = 'r';	else *b++='-';
					if (a & FKS_S_W32_Hidden) 			*b++ = 'h';	else *b++='-';
					if (a & FKS_S_W32_System)    		*b++ = 's';	else *b++='-';
					if (a & FKS_S_W32_Volume)    		*b++ = '1';	else *b++='-';
					if (a & FKS_S_W32_Directory)		*b++ = 'd';	else *b++='-';
					if (a & FKS_S_W32_Archive) 			*b++ = 'a';	else *b++='-';
					if (a & FKS_S_W32_Device) 			*b++ = 'D';	else *b++='-';
					if (a & FKS_S_W32_Normal)   		*b++ = 'n';	else *b++='-';
					if (a & FKS_S_W32_Temporary)   		*b++ = 't';	else *b++='-';
					if (a & FKS_S_W32_SparseFile) 		*b++ = 'p';	else *b++='-';
					if (a & FKS_S_W32_ReparsePoint) 	*b++ = 'l';	else *b++='-';
					if (a & FKS_S_W32_Compressed)   	*b++ = 'c';	else *b++='-';
					if (a & FKS_S_W32_Offline) 			*b++ = 'o';	else *b++='-';
					if (a & FKS_S_W32_NoIndexed)		*b++ = 'i';	else *b++='-';
					if (a & FKS_S_W32_Encrypted) 		*b++ = 'e';	else *b++='-';
					if (a & FKS_S_W32_IntegritySystem) 	*b++ = 'v';	else *b++='-';
					if (a & FKS_S_W32_Virtual) 			*b++ = 'V';	else *b++='-';
					if (a & FKS_S_W32_NoScrubData)		*b++ = 'x';	else *b++='-';
					if (a & FKS_S_W32_EA) 				*b++ = 'E';	else *b++='-';
					if (a & FKS_S_W32_Pinned)			*b++ = 'P';	else *b++='-';
					if (a & FKS_S_W32_Unpinned)			*b++ = 'U';	else *b++='-';
					if (a & 0x00200000)					*b++ = '1'; else *b++='-';
					if (a & FKS_S_W32_RecallOnDataAcs)	*b++ = 'A';	else *b++='-';
					if (a & 0x00800000)					*b++ = '1'; else *b++='-';
					if (a & 0x01000000)					*b++ = '1'; else *b++='-';
					if (a & FKS_S_W32_StrictlySequential) *b++='Q';	else *b++='-';
					if (a & 0x04000000)					*b++ = '1'; else *b++='-';
					if (a & 0x08000000)					*b++ = '1'; else *b++='-';
					if (a & 0x10000000)					*b++ = '1'; else *b++='-';
					if (a & 0x20000000)					*b++ = '1'; else *b++='-';
					if (a & 0x40000000)					*b++ = '1'; else *b++='-';
					if (a & 0x80000000)					*b++ = '1'; else *b++='-';
					*b = 0;
					if (n < 32)
						buf[n] = 0;
		    	    if (n > de-d-1)
		    	    	n = de-d-1;
	    	    	d += snprintf(d, de-d, "%-*s", n, buf);
	    	    } else
			  #endif
	    	    {
					if (n < 0)
						n = 4;
		    	    if (n > de-d-1)
		    	    	n = de-d-1;
					d += snprintf(d, de-d, "%0*x", n, st->st_native_attr);
				}
				break;

	    	default:
				//if (c == odrCh) {
				//	PUT_C(d,de,odrCh);
	    		//} else
	    		if (c >= '1' && c <= '9') {
					td = d;
	    	    	fks_pathCpy(d, de - d, &var_[c-'0'][0]);
	    	    	d = STREND(d);
		    	    if (relative && fks_pathIsAbs(td)) d = changeRelative(td, de);
    	    	    if (sepMode) changeSep(td, sepMode);
	    	    } else {
	 	 SKIP_S:
					for (ss = sav_s; ss < s; ++ss)
						PUT_C(d,de,*ss);
	    	    }
	    	}
	    } else {
			++s;
			++d;
		}
	}
}

#if 1
char* ConvFmt::strFmtSize(char* d, char* de, int64_t size, int n, bool mode)
{
	static char const s_sym[] = " KMGTPEZY";
	int l = 0, m = 0, f = 0, z = 2, k = 1000;
	fks_isize_t sz = size;
	if (mode)
		z = 3, k = 1024;
	do {
		++l;
		sz /= 10;
	} while (sz);
	m = 0;
	f = 0;
	sz = size;
	while (sz && l > n - z && m < 9) {
		f  |= (sz % k) != 0;
		sz = sz / k;
		l -= 3;
		++m;
	}
	//printf("n=%d sz=%d l=%d m=%d\n", n, sz, l, m);
	if (n >= z)
		n -= (m > 0)*z;
	d += snprintf(d, de-d, "%*" PRIF_LL "d", n, (PRIF_LLONG)sz+f);
	if (m) {
		PUT_C(d,de, s_sym[m]);
		if (mode)
			PUT_C(d,de, 'i');
		PUT_C(d,de, 'B');
	}
	return d;
}
#endif

char *ConvFmt::stpCpy(char* d, char* de, char const* s, ptrdiff_t clm, int uplow) {
	fks_pathCpy(d, de - d, s);
	if (uplow > 0) {	// upper.
		fks_pathToUpper(d);
	} else if (uplow < 0) {	// lower.
		fks_pathToLower(d);
	}
	size_t  n = strlen(d);
	d += n;
	clm -= (ptrdiff_t)n;
	while (clm > 0) {
		if (d < de)
		    *d++ = ' ';
	    --clm;
	}
	*d = '\0';
	return d;
}

void ConvFmt::changeSep(char* d, int sepMode)
{
	if (sepMode == 1)
		fks_pathBackslashToSlash(d);
	else if (sepMode == 2)
		fks_pathSlashToBackslash(d);
}

char* ConvFmt::changeRelative(char* d, char* de)
{
	FPathBuf	buf;
	fks_pathRelativePath(&buf[0], buf.capacity(), d, &relativeBaseDir_[0]);
	fks_pathCpy(d, de - d, &buf[0]);
	return STREND(d);
}
