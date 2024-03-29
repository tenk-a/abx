/**
 *  @file   abx.cpp
 *  @brief  Search file, embed file name in text(gen. bat)
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1995-2018
 *  @license Boost Software License Version 1.0
 *  @note
 *  add -xm multi thread version by misakichi (https://github.com/misakichi)
 */

#include <fks/fks_common.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subr.hpp"
#include "StrzBuf.hpp"
#include "AbxConvFmt.hpp"
#include "AbxFiles.hpp"
#include "AbxMsgStr.hpp"

#ifdef ENABLE_MT_X
#include "abxmt.hpp"
#endif

#include <fks/fks_dirent.h>
#include <fks/fks_path.h>
#include <fks/fks_io.h>
#include <fks/fks_time.h>
#include <fks/fks_misc.h>
#include <fks/fks_mbc.h>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;


/** error check fopen
 */
FILE *fopenX(char const* name, char const* mod) {
    FILE *fp = fopen(name,mod);
    if (fp == NULL) {
        fprintf(stderr, ABXMSG(file_open_error), name);
        return NULL;
    }
    setvbuf(fp, NULL, _IOFBF, 1024*1024);
    return fp;
}


/*--------------------------------------------------------------------------*/
class Opts {
public:
    ConvFmt&        rConvFmt_;
    StrzBuf<FMTSIZ>& rFmtBuf_;

    AbxFiles_Opts   filesOpts_;

    //bool          recursiveFlg_;
    //bool          zenFlg_;                // MS Zenkaku.
    bool            execBatFlg_;
    bool            addBatHeaderFooterFlg_; // -b
    bool            lineIsFilenameFlg_;     // -l
    bool            ignoreCaseFlag_;        // -u
    bool            forceUtf8OutFlag_;      // -utf8
    char            needOptPathMode_;       //
    char            odrChr_;                // $ or @
    int             noFindFile_;            // -n
    size_t          topN_;                  // -t
    size_t          sirialNumStart_;        // -ciN
    size_t          sirialNumEnd_;          // -ci?:M
    char const*     resFileRq_;
    FPathBuf        outName_;
    FPathBuf        exeName_;
    FPathBuf        tmpDir_;
 #ifdef ENABLE_MT_X
    unsigned        threadSize_;
 #endif

public:
    Opts(ConvFmt& rConvFmt, StrzBuf<FMTSIZ>& rFmtBuf);
    bool scan(char* s);
    bool usage();

    void setExename(char const* exename) {
        exeName_ = exename;
     #ifdef _WIN32
        fks_pathToLower(&exeName_[0]);
     #endif
    }

    bool needOptPath() const { return needOptPathMode_ != 0; }
    void setNeedOptPath(char const* p);

private:
    fks_timespec  parseDateTime(char* &p, bool maxFlag);
    fks_isize_t parseSize(char* &p);
    char*       parseAttr(char* p);
};

Opts::Opts(ConvFmt& rConvFmt, StrzBuf<FMTSIZ>& rFmtBuf)
    : rConvFmt_(rConvFmt)
    , rFmtBuf_(rFmtBuf)
    //, zenFlg_(true)
    , execBatFlg_(false)
    , addBatHeaderFooterFlg_(false)
    , lineIsFilenameFlg_(false)
    , ignoreCaseFlag_(false)
    , needOptPathMode_(0)
    , odrChr_('$')
    , noFindFile_(0)
    , topN_(0)
    , sirialNumStart_(0)
    , sirialNumEnd_(0)
    , resFileRq_(NULL)
    , outName_()
    , exeName_()
   #ifdef ENABLE_MT_X
    , threadSize_(0)
   #endif
{
}

bool Opts::scan(char* s) {
    char* p = s + 1;
    int  c = *(unsigned char*)p++;
    switch (c) {
    case 'x':
        execBatFlg_ = (*p != '-');
        //mt check
     #ifdef ENABLE_MT_X
        if (execBatFlg_) {
            if (*p == 'm') {
                threadSize_ = strtol(p+1, NULL, 0);
                ++threadSize_;
            }
        }
     #endif
        break;

    case 'r':
        filesOpts_.recursiveFlg_ = (*p != '-');
        break;

    case 'u':
        if (strcmp(p-1, "utf8") == 0) {
            //fks_ioMbsOutputInit(1);
            forceUtf8OutFlag_ = true;
        } else if (*p == '-') {
            ignoreCaseFlag_ = 0;
        } else if (*p == '\0') {
            ignoreCaseFlag_ = 1;
        } else {
            goto ERR_OPTS;
        }
        break;

    case 'n':
        noFindFile_ = (*p != '-');
        if (*p == 'd')
            noFindFile_ = 2;
        break;

    //case 'j':
    //  zenFlg_ = (*p != '-');
    //  break;

    case 'b':
        addBatHeaderFooterFlg_ = (*p != '-');
        break;

    case 'l':
        lineIsFilenameFlg_  = (*p != '-');
        break;

    case 't':
        if (*p == 0) {
            topN_ = 1;
        } else {
            topN_ = strtol(p,NULL,0);
        }
        break;

    case 'k':
        while ((c = *p++) != '\0') {
            if ((c == '$' && *p == '@') || (c == '@' && *p == '$')) {
                ++p;
                rConvFmt_.setOdrCh('\0');
            } else if (c == '$') {
                rConvFmt_.setOdrCh('$');
                odrChr_ = '$';
            } else if (c == '@') {
                rConvFmt_.setOdrCh('@');
                odrChr_ = '@';
            } else if (c == 'R') {
                rConvFmt_.setRelativePathMode(true);
            } else if (c == 'F') {
                rConvFmt_.setRelativePathMode(false);
            } else if (c == 'B' || c == '\\') {
                rConvFmt_.setSepMode(2);
            } else if (c == 'b' || c == '/') {
                rConvFmt_.setSepMode(1);
            } else if (c == 'U') {
                rConvFmt_.setUpLowMode(1);
            } else if (c == 'L') {
                rConvFmt_.setUpLowMode(-1);
            } else if (c == 'y') {
                rConvFmt_.setAddDoubleQuotation(true);
            } else if ((c == '=' || c == ':') && *p) {
                rFmtBuf_ = p;
                break;
            } else {
                goto ERR_OPTS;
            }
        }
        break;

    case 'c':
        c = *p;
        if (c == '-') {
            filesOpts_.charCodeChk_ = 0;
        } else if (c == 'd') {
            ++p;
            if (*p)
                rConvFmt_.setRelativeBaseDir(p);
            else
                needOptPathMode_ = 'c';
        } else if (c == 'k') {
            filesOpts_.charCodeChk_ = 1;
            if (p[1] == '-')
                filesOpts_.charCodeChk_ = -1;
        } else if (c == 'y') {
            filesOpts_.charCodeChk_ = 2;
            if (p[1] == '-')
                filesOpts_.charCodeChk_ = -2;
        } else if (c == 't') {
            ++p;
            if (*p)
                rConvFmt_.setTargetNameFmt(p, false);
            else
                needOptPathMode_ = 't';
        } else if (c == 'u') {
            ++p;
            if (*p)
                rConvFmt_.setTargetNameFmt(p, true);
            else
                needOptPathMode_ = 'u';
        } else if (c == 'i') {
            sirialNumStart_ = strtol(p+1, (char**)&p, 0);
            if (*p) {
                sirialNumEnd_ = strtol(p+1, (char**)&p, 0);
            } else {
                sirialNumEnd_ = 0;
            }
        } else {
            goto ERR_OPTS;
        }
        break;

    case 'y':
        rConvFmt_.setAddDoubleQuotation(*p != '-');
        break;

    case 'e':
        if (*p == '.')
            ++p;
        filesOpts_.dfltExt_ = p;
        filesOpts_.dfltExtp_ = filesOpts_.dfltExt_.c_str();
        if ((*p == '$' || *p == '@') && p[1] >= '1' && p[1] <= '9' && p[2] == 0) {
            filesOpts_.dfltExt_ = rConvFmt_.getVar(p[1]-'0');
        }
        /*filesOpts_.dfltExt_[3] = 0;*/
        break;

    case 'o':
        if (*p) {
            outName_ = p;
        } else {
            needOptPathMode_ = 'o';
        }
        break;

    case 'i':
        if (*p) {
            fks_fileFullpath(&filesOpts_.inputDir_[0], filesOpts_.inputDir_.capacity(), p);
            fks_pathAddSep(&filesOpts_.inputDir_[0], filesOpts_.inputDir_.capacity());
        } else {
            needOptPathMode_ = 'i';
        }
        break;

    case 'p':
        if (*p) {
            rConvFmt_.setChgPathDir(p);
        } else {
            needOptPathMode_ = 'p';
        }
        break;

    case 'w':
        if (*p) {
            //rConvFmt_.setTmpDir(p);
            tmpDir_ = p;
        } else {
            needOptPathMode_ = 'w';
        }
        break;

    case 'a':
        p = parseAttr(p);
        if (!p)
            goto ERR_OPTS;
        break;

    case 's':
        c = 0;
        filesOpts_.sortType_ = ST_NAME;
        while (*p) {
            switch(*p) {
            case '-': filesOpts_.sortType_ = ST_NONE; break;
            case 'n': filesOpts_.sortType_ = ST_NAME; break;
            case 'e': filesOpts_.sortType_ = ST_EXT;  break;
            case 'z': filesOpts_.sortType_ = ST_SIZE; break;
            case 't': filesOpts_.sortType_ = ST_DATE; break;
            case 'a': filesOpts_.sortType_ = ST_ATTR; break;
            case 'm': filesOpts_.sortType_ = ST_NUM;  break;
            case 'r': filesOpts_.sortRevFlg_ = true;  break;
            case 'd': filesOpts_.sortDirFlg_ = true;  break;
            default:  goto ERR_OPTS;
            }
            ++p;
        }
        break;

    case 'z':
        filesOpts_.sizeMin_ = (*p == '-') ? 0 : parseSize(p);
        if (*p) { /* *p == '-' */
            filesOpts_.sizeMax_ = FKS_ISIZE_MAX;
            if (*++p)
                filesOpts_.sizeMax_ = parseSize(p);
            if (filesOpts_.sizeMax_ < filesOpts_.sizeMin_)
                goto ERR_OPTS;
        } else {
            filesOpts_.sizeMax_ = filesOpts_.sizeMin_;
        }
        break;

    case 'd':
        if (*p == 0) {
            filesOpts_.dateMax_.tv_sec  = 0;
            filesOpts_.dateMax_.tv_nsec = 0;
            filesOpts_.dateMin_.tv_sec  = 0;
            filesOpts_.dateMin_.tv_nsec = 0;
        } else {
            if (*p == '-') {
                filesOpts_.dateMin_.tv_sec = 0;
                filesOpts_.dateMin_.tv_nsec = 0;
                ++p;
            } else {
                filesOpts_.dateMin_ = parseDateTime(p, false);
                if (filesOpts_.dateMin_.tv_sec < 0)
                    goto ERR_OPTS;
                filesOpts_.dateMax_.tv_sec  = FKS_TIME_MAX;
                filesOpts_.dateMax_.tv_nsec = FKS_TIME_MAX;
                if (*p == '-')
                    ++p;
            }
            if (*p) {
                filesOpts_.dateMax_ = parseDateTime(p, true);
                if (filesOpts_.dateMax_.tv_sec < 0)
                    goto ERR_OPTS;
                if (fks_timespecCmp(&filesOpts_.dateMax_, &filesOpts_.dateMin_) < 0)
                    goto ERR_OPTS;
            }
        }
        break;

    case '@':
        resFileRq_ = p;
        break;

    case '?':
    case 'h':
    case 'H':
    case '\0':
        return usage();

    default:
  ERR_OPTS:
        fprintf(stderr, ABXMSG(incorrect_command_line_option), s);
        return false;
    }
    return true;
}

void Opts::setNeedOptPath(char const* p)
{
    switch (needOptPathMode_) {
    case 'c':
        rConvFmt_.setRelativeBaseDir(p);
        break;

    case 't':
        rConvFmt_.setTargetNameFmt(p, false);
        break;

    case 'u':
        rConvFmt_.setTargetNameFmt(p, true);
        break;

    case 'o':
        outName_ = p;
        break;

    case 'i':
        fks_fileFullpath(&filesOpts_.inputDir_[0], filesOpts_.inputDir_.capacity(), p);
        fks_pathAddSep(&filesOpts_.inputDir_[0], filesOpts_.inputDir_.capacity());
        break;

    case 'p':
        rConvFmt_.setChgPathDir(p);
        break;

    case 'w':
        //rConvFmt_.setTmpDir(p);
        tmpDir_ = p;
        break;

    default:
        break;
    }
    needOptPathMode_ = 0;
}

bool Opts::usage()
{
    printf("%s", ABXMSG(usage));
    printf("%s", ABXMSG(usage_options));

    fks_stat_t  st;
    memset(&st,0,sizeof st);
    static const Fks_DateTime dateTime = { 2001, 3, 0, 21, 23, 56, 39, 987, 654, 321 };
    st.st_mtimespec = fks_localDateTimeToTimespec(&dateTime);
    st.st_size = 0x100021;
    st.st_mode = 0777;
    st.st_native_mode = 0x025ffff7;
    rConvFmt_.setNoFindFile(true);
    rConvFmt_.setNum(253);
    rConvFmt_.setOdrCh('$');
    //rConvFmt_.setTargetNameFmt("$g.bak");
    rConvFmt_.setIgnoreCaseFlag(false);
    rConvFmt_.setRecursiveDirFlag(false);
    rConvFmt_.setAddDoubleQuotation(false);
    rConvFmt_.setRelativePathMode(0);
    rConvFmt_.setSepMode(0);
    rConvFmt_.setUpLowMode(0);
    rConvFmt_.setTmpDir(&tmpDir_[0]);
    rConvFmt_.setChgPathDir("");
    rConvFmt_.clearVar();
 #ifdef FKS_WIN32
    #define SMP_DIR     "d:/dir1/dir2"
    std::string fmt(FKS_SRCCODE_TO_UTF8_S(abxMsgStr->conversion_orders));
 #else
    #define SMP_DIR     "/dir1/dir2"
    std::string fmt = abxMsgStr->conversion_orders;
 #endif
    rConvFmt_.setFmtStr(fmt.c_str());
    rConvFmt_.setRelativeBaseDir("");
    rConvFmt_.setCurDir(SMP_DIR "/dir4");
    rConvFmt_.write(SMP_DIR "/dir3/file.ext", &st);
    //printf("%s", rConvFmt_.currentOutBuf());
 #ifdef FKS_WIN32
    printf("%s", FKS_MBS_S(rConvFmt_.outBufList().begin()->c_str()));
 #else
    printf("%s", rConvFmt_.outBufList().begin()->c_str());
 #endif
    return false;
}

static bool isDateSep(int c)
{
    return (c == '-' || c == '/' || c == '.' || c == ':' || c == '_');
}

static bool isTimeSep(int c)
{
    return (c == ':' || c == ' ' || c == '.' || c == '/' || c == '_');
}

fks_timespec Opts::parseDateTime(char* &p, bool /*maxFlag*/)
{
    static uint8_t const dayLimTbl[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    unsigned y = 0, m = 0, d = 0, h = 0, min=0, sec=0, milli=0;
    //if (maxFlag)
    //  m = 12, d = 31, h = 23, min=59, sec=59, milli=999;
    y = strtoul(p, &p, 10);
    if (y < 10000 && isDateSep(*p)) {
        m = strtoul(p+1, &p, 10);
        if (isDateSep(*p) && isdigit(p[1])) {
            d = strtoul(p+1, &p, 10);
        }
    } else if (y >= 10000) {
        unsigned t = y;
        y = (int)(t / 10000); y = (y < 70) ? 2000 + y : (y < 100) ? y + 1900 : y;
        m = (int)((t / 100) % 100);
        d = (int)(t % 100);
    }
    fks_timespec er = { -1, 0xffffffff };
    if (m == 0 || 12 < m) return er;
    unsigned dayLim = dayLimTbl[m];
    if (m == 2 && (y % 4) == 0 && (y % 100) != 0)
        dayLim = 29;
    if (d == 0 || dayLim < d) return er;
    if (isTimeSep(*p) && isdigit(p[1])) {
        h = strtoul(p + 1, &p, 10);
        if (isTimeSep(*p) && isdigit(p[1])) {
            h = strtoul(p + 1, &p, 10);
            if (isTimeSep(*p) && isdigit(p[1])) {
                min = strtoul(p + 1, &p, 10);
                if (isTimeSep(*p) && isdigit(p[1])) {
                    sec = strtoul(p + 1, &p, 10);
                    if (isTimeSep(*p) && isdigit(p[1]))
                        milli = strtoul(p + 1, &p, 10);
                }
            }
        }
    }

    Fks_DateTime dt = {y,m,0,d,h,min,sec,milli,0,0};
    return fks_localDateTimeToTimespec(&dt);
}

fks_isize_t Opts::parseSize(char* &p)
{
    uint64_t    sz = uint64_t( strtoull(p, &p, 0) );
    uint64_t    k  = 1000;
    if (*p == 'i') {
        k = 1024;
        ++p;
    }
    if (*p == 'K' || *p == 'k')         p++, sz *= k;
    else if (*p == 'M' || *p == 'm')    p++, sz *= k*k;
    else if (*p == 'G' || *p == 'g')    p++, sz *= k*k*k;
    else if (*p == 'T' || *p == 't')    p++, sz *= k*k*k*k;
    else if (*p == 'P' || *p == 'p')    p++, sz *= k*k*k*k*k;
    return (fks_isize_t)sz;
}

char*   Opts::parseAttr(char* p)
{
    while (*p && *p != ':') {
        switch(*p) {
        case 'd': filesOpts_.srchAttr_ |= SRCH_DIR;     break;
        case 'f': filesOpts_.srchAttr_ |= SRCH_FILE;    break;
        case 'h': filesOpts_.srchAttr_ |= SRCH_HIDDEN;  break;
        case 'p': filesOpts_.srchAttr_ |= SRCH_DOTDOT;  break;
        //case 'r': filesOpts_.srchAttr_ |= SRCH_RDONLY; break;
        case 'a': filesOpts_.srchAttr_ |= SRCH_DIR|SRCH_FILE|SRCH_HIDDEN|SRCH_DOTDOT; break;
        default:  return NULL; //goto ERR_OPTS;
        }
        ++p;
    }
    if (!*p)
        return p;
    ++p;
    if (isdigit(*(unsigned char*)p)) {
        filesOpts_.fileAttr_ = strtoul(p, &p, 16);
    } else {
     #ifdef FKS_WIN32
        while (*p) {
            switch(*p) {
            case 'r': filesOpts_.fileAttr_ |= FKS_S_W32_ReadOnly;           break;
            case 'h': filesOpts_.fileAttr_ |= FKS_S_W32_Hidden;             break;
            case 's': filesOpts_.fileAttr_ |= FKS_S_W32_System;             break;
            case 'v': filesOpts_.fileAttr_ |= FKS_S_W32_Volume;             break;
            case 'd': filesOpts_.fileAttr_ |= FKS_S_W32_Directory;          break;
            case 'a': filesOpts_.fileAttr_ |= FKS_S_W32_Archive;            break;
            case 'D': filesOpts_.fileAttr_ |= FKS_S_W32_Device;             break;
            case 'n': filesOpts_.fileAttr_ |= FKS_S_W32_Normal;             break;
            case 't': filesOpts_.fileAttr_ |= FKS_S_W32_Temporary;          break;
            case 'S': filesOpts_.fileAttr_ |= FKS_S_W32_SparseFile;         break;
            case 'R': filesOpts_.fileAttr_ |= FKS_S_W32_ReparsePoint;       break;
            case 'c': filesOpts_.fileAttr_ |= FKS_S_W32_Compressed;         break;
            case 'o': filesOpts_.fileAttr_ |= FKS_S_W32_Offline;            break;
            case 'N': filesOpts_.fileAttr_ |= FKS_S_W32_NoIndexed;          break;
            case 'e': filesOpts_.fileAttr_ |= FKS_S_W32_Encrypted;          break;
            case 'I': filesOpts_.fileAttr_ |= FKS_S_W32_IntegritySystem;    break;
            case 'V': filesOpts_.fileAttr_ |= FKS_S_W32_Virtual;            break;
            case 'B': filesOpts_.fileAttr_ |= FKS_S_W32_NoScrubData;        break;
            case 'E': filesOpts_.fileAttr_ |= FKS_S_W32_EA;                 break;
            case 'P': filesOpts_.fileAttr_ |= FKS_S_W32_Pinned;             break;
            case 'U': filesOpts_.fileAttr_ |= FKS_S_W32_Unpinned;           break;
            case 'A': filesOpts_.fileAttr_ |= FKS_S_W32_RecallOnDataAcs;    break;
            case 'Z': filesOpts_.fileAttr_ |= FKS_S_W32_StrictlySequential; break;
            default:  return NULL; //goto ERR_OPTS;
            }
            ++p;
        }
     #endif
    }
    return p;
}


// ----------------------------------------------------------------------------

class ResCfgFile {
public:
    ResCfgFile(Opts& rOpts, ConvFmt& rConvFmt, StrList& rFileNameList, StrList& rBeforeList, StrList& rAfterList, StrzBuf<FMTSIZ>& rFmtBuf);
    bool getResFile(char const* name, bool chg=false);
    bool getCfgFile(char *name, char *key);

private:
    char *getLine(void);
    char *setDollNumVar(char *p0);
    char const* getFileNameStr(char *d, size_t dl, char const* s);
    bool getFmts();
    bool keyStrEqu(char *key, char *lin);
    bool loadFile(char const* name, std::vector<char>& buf, bool errMsg=true);

private:
    Opts&               rOpts_;
    ConvFmt&            rConvFmt_;
    StrList&            rFileNameList_;
    StrList&            rBeforeStrList_;
    StrList&            rAfterStrList_;
    StrzBuf<FMTSIZ>&    rFmtBuf_;
    int                 varIdx_;
    int                 varNo_[10 + 1];
    FPathBuf            resName_;
    char*               resP_;
    std::vector<char>   resFileBuf_;
};


ResCfgFile::ResCfgFile(Opts& rOpts, ConvFmt& rConvFmt, StrList& rFileNameList, StrList& rBeforeList, StrList& rAfterList, StrzBuf<FMTSIZ>& rFmtBuf)
    : rOpts_(rOpts)
    , rConvFmt_(rConvFmt)
    , rFileNameList_(rFileNameList)
    , rBeforeStrList_(rBeforeList)
    , rAfterStrList_(rAfterList)
    , rFmtBuf_(rFmtBuf)
    //, resP_(&resFileBuf_[0])
    , varIdx_(1)
    //, varNo_[10]
    , resName_()
    , resFileBuf_()
{
    memset(varNo_, 0, sizeof(varNo_));
    resP_ = NULL; // &resFileBuf_[0];
}

bool ResCfgFile::loadFile(char const* name, std::vector<char>& buf, bool errMsg) {
    if (fks_fileLoad(name, buf) == false) {
        if (errMsg)
            fprintf(stderr, ABXMSG(file_read_error), name);
        return false;
    }
    if (buf.empty())
        return true;
    fks::ConvLineFeed(buf);
 #ifdef FKS_WIN32
    std::vector<char> buf2;
    fks::ConvCharEncoding(buf2, fks_mbc_utf8, buf, fks::AutoCharEncoding(buf));
    buf.swap(buf2);
 #endif
    buf.push_back('\0');
    return true;
}

/** Input response file
 */
bool ResCfgFile::getResFile(char const* name, bool chg) {
    if (chg)
        fks_pathSetExt(&resName_[0], resName_.capacity(), name, "abx");
    else
        fks_pathSetDefaultExt(&resName_[0], resName_.capacity(), name, "abx");
    if (loadFile(&resName_[0], resFileBuf_, !chg) == false)
        return false;
    if (resFileBuf_.empty())
        return true;
    resP_ = &resFileBuf_[0];
    return getFmts();
}

/** Input .cfg file
 */
bool ResCfgFile::getCfgFile(char *name, char *key) {
    fks_fileFullpath(&resName_[0], resName_.capacity(), name);
    if (loadFile(&resName_[0], resFileBuf_) == false)
        return false;

    if (key[1] == 0) /* Only ':' */
        printf(ABXMSG(conversion_names_list));

    resP_ = &resFileBuf_[0];
    // Find LF+':'+conversionName
    while ((resP_ = strstr(resP_, "\n:")) != NULL) {
        resP_ ++;
        char* p = getLine();
        if (p)
            p = strtok(p, " \t\r");
        /*printf("cmp %s vs %s\n",key,p);*/
        if (p == NULL || *p == 0)
            continue;
        //strupr(p);
        if (key[1]) {
            // If the conversion name is found, the same processing as the response file is performed.
            if (keyStrEqu(key, p)) {
                if ((p = strstr(resP_, "\n:")) != NULL) {
                    *p = '\0';
                }
                return getFmts();
            }
        } else {    // If there is no conversion name, list display.
            printf("\t%s\n",p);
        }
    }
    if (key[1]) {
        fprintf(stderr, ABXMSG(key_is_not_defined), key, resName_.c_str());
    }
    return false;
}


/** One line input from resFileBuf_.
 * Delete CR/LF. resFileBuf_ is destroyed.
 */
char *ResCfgFile::getLine(void) {
    char *p;

    p = resP_;
    if (*p == 0)
        return NULL;
    resP_ = strchr(p, '\n');
    if (resP_ != NULL) {
        if (resP_[-1] == '\r')
            resP_[-1] = 0;
        *resP_++ = 0;
    } else {
        resP_ = STREND(p);
    }
    return p;
}

/** Set $N variable.
 */
char *ResCfgFile::setDollNumVar(char *p0) {
    size_t l = 0;
    char*  p = p0;
    int m = *p++;
    if (m < '1' || m > '9')
        goto ERR;
    m -= '0';
    if (*p == '=') {
        ++p;
        l = strcspn(p,"\t\n ");
        if (rConvFmt_.setVar(m, p, l) == false)
            goto ERR;
        p += l+1;
        goto RET;

  ERR:
        fprintf(stderr, ABXMSG(cfgfile_has_an_incorrect_dollN_specification), p0);

        exit(1);
    } else if (*p++ == ':') {
        int i;
        int n = *p++;
        if (n < '1' || n > '9')
            goto ERR2;
        n -= '0';
        if (*p++ != '{')
            goto ERR2;
        i = 0;
        do {
            l = strcspn(p,"|}");
            if (i == varNo_[n]) {
                if (rConvFmt_.setVar(m, p, l) == false)
                    goto ERR;
                p = strchr(p,'}');
                if (p == NULL)
                    goto ERR2;
                p++;
                goto RET;
            }
            i++;
            p += l + 1;
        } while (p[-1] == '|');
  ERR2:
        fprintf(stderr, ABXMSG(dollN_specification_in_cfgfile_is_incorrect), p0);
        exit(1);
    }
  RET:
    return p;
}

/** Get filename with blank separator
 */
char const* ResCfgFile::getFileNameStr(char *d, size_t dl, char const* s) {
    int   f = 0;
    char* e = d + dl-1;

    s = fks_skipSpc(s);
    while (*s) {
        if (*s == '"')
            f ^= 1;
        else if (f == 0 && (*(unsigned char *)s <= ' '))
            break;
        else if (d < e)
            *d++ = *s;
        s++;
    }
    *d = 0;
    return s;
}

/** Get conversion-formats.
 */
bool ResCfgFile::getFmts() {
    #define ISSPC(c)    ((unsigned char)c <= ' ')
    FPathBuf    name;
    enum Mode { MD_Body, MD_Bgn, MD_End, MD_TameBody };
    Mode    mode = MD_Body;
    char*   d;
    char*   p;
    while ( (p = getLine()) != NULL ) {
        char* q = (char*)fks_skipSpc(p);
        if (strncmp(q, "#begin", 6) == 0 && ISSPC(p[6])) {
            mode = MD_Bgn;
            continue;
        } else if (strncmp(q, "#body", 5) == 0 && ISSPC(p[5])) {
            mode = MD_Body;
            continue;
        } else if (strncmp(q, "#end", 4) == 0 && ISSPC(p[4])) {
            mode = MD_End;
            continue;
        }
        switch (mode) {
        case MD_Body: /* #body */
            while (p && *p) {
                p = (char*)fks_skipSpc(p);
                switch (*p) {
                case '\0':
                case '#':
                    goto NEXT_LINE;
                case '\'':
                    if (p[1] == 0) {
                        fprintf(stderr, ABXMSG(single_quotation_string_is_broken));
                        return false;
                    }
                    p++;
                    d = strchr(p, '\'');
                    if (d) {
                        *d = '\0';
                        fks_pathCpy(&rFmtBuf_[0], rFmtBuf_.capacity(), p);
                    }
                    break;
                case '=':
                    mode = MD_TameBody;
                    goto NEXT_LINE;
                case '-':               /* -options */
                    q = (char*)fks_skipNotSpc(p);
                    if (*q) {
                        *q++ = 0;
                    } else {
                        q = NULL;
                    }
                    if (rOpts_.scan(p) == false) {
                        return false;
                    }
                    p = q;
                    break;
                case '$':               /* $N var */
                    p = setDollNumVar(p+1);
                    break;
                default:
                    if (rOpts_.lineIsFilenameFlg_) {
                        rFileNameList_.push_back(p);
                        goto NEXT_LINE;
                    } else {    // Get filename with blank separator.
                        p = (char*)getFileNameStr(&name[0], name.capacity(), p);
                        rFileNameList_.push_back(&name[0]);
                    }
                }
            }
          NEXT_LINE:;
            break;
        case MD_Bgn: /* #begin */
            rBeforeStrList_.push_back(p);
            break;
        case MD_End: /* #end  */
            rAfterStrList_.push_back(p);
            break;
        case MD_TameBody:
            fks_pathCat(&rFmtBuf_[0], rFmtBuf_.capacity(), p);
            fks_pathCat(&rFmtBuf_[0], rFmtBuf_.capacity(), "\n");
            break;
        }
    }
    return true;
    #undef ISSPC
}


bool ResCfgFile::keyStrEqu(char *key, char *lin) {
    char    *k,*f;
    size_t  l;

    rConvFmt_.clearVar();

    k = key;
    f = lin;
    for (; ;) {
      NEXT:
        if (*k == *f) {
            if (*k == '\0')
                return true;   /* match */
            k++;
            f++;
            continue;
        }
        if (*f == '{') {
            f++;
            varNo_[varIdx_] = 0;
            do {
                l = strcspn(f,"|}");
                /*printf("l=%d\n",l);*/
                if (l < 1 || l >= ConvFmt::VAR_NMSZ-1) {
                    goto ERR1;
                }
                if (memcmp(k,f,l) == 0) {
                    if (varIdx_ >= 10) {
                        fprintf(stderr, ABXMSG(there_are_more_par_pair_in_a_certain_line), resName_.c_str(),lin);
                        exit(1);
                    }
                    rConvFmt_.setVar(varIdx_, f, l);
                    ++varIdx_;
                    k += l;
                    f = strchr(f,'}');
                    if (f == NULL) {
              ERR1:
                        fprintf(stderr, ABXMSG(invalid_par_pair),resName_.c_str(), lin);
                        exit(1);
                    }
                    f++;
                    goto NEXT;
                }
                f += l + 1;
                varNo_[varIdx_]++;
            } while (f[-1] == '|');
            varNo_[varIdx_] = 0;
        }
        break;
    }
    return false;          /* not match */
}



// ----------------------------------------------------------------------------

class App {
public:
    App();

    ~App() {
        // Do not deliberately release memory to leave it to end processing by OS.
    }

    int main(int argc, char *argv[]);

private:
    bool scanOpts(int argc, char *argv[]);
    bool genText();
    bool outputText();
    bool execBat();
    void initPath();

private:
    FILE*               outFp_;
    StrList             filenameList_;
    StrList             beforeStrList_;
    StrList             afterStrList_;
    FPathBuf            fileName_;
    ConvFmt             convFmt_;
    Opts                opts_;
    ResCfgFile          resCfgFile_;
    AbxFiles            files_;
    StrzBuf<FPATH_SIZE> tmpFName_;
    StrzBuf<FMTSIZ>     fmtBuf_;
};

App::App()
    : outFp_(NULL)
    , filenameList_()
    , beforeStrList_()
    , afterStrList_()
    , fileName_()
    , convFmt_()
    , opts_(convFmt_, fmtBuf_)
    , resCfgFile_(opts_, convFmt_, filenameList_, beforeStrList_, afterStrList_, fmtBuf_)
    , files_()
    , tmpFName_()
    , fmtBuf_()
{
}

int App::main(int argc, char *argv[]) {
    opts_.setExename(fks_pathBaseName(argv[0]));
    initPath();

    if (argc < 2) {
        opts_.usage();
        return 1;
    }

    if (fks_fileExist(&fileName_[0]))
        resCfgFile_.getResFile(&fileName_[0], true);

    if (scanOpts(argc, argv) == false)
        return 1;

    if (!opts_.noFindFile_ && !opts_.sirialNumEnd_) {   // Find File
        if (files_.getPathStats(filenameList_, opts_.filesOpts_) == false)
            return 1;
    } else {    // raw string mode.
        //TODO: sort
    }

    if (genText() == false)
        return 1;

    if (outputText() == false)
        return 1;

    if (execBat() == false)
        return 1;

    if (tmpFName_[0])
        remove(&tmpFName_[0]);
    return 0;
}

/** Scan command line and get file name and options.
 */
bool App::scanOpts(int argc, char *argv[]) {
    int f = 0;
    for (int i = 1; i < argc; ++i) {
        char* p = argv[i];
       LLL1:
        if (f) {
            if (!fmtBuf_.empty())
                fmtBuf_ += " ";
            fmtBuf_ += p;
            if (f == 1) {
                p = strrchr(&fmtBuf_[0], '\'');
                if (p) {
                    f = 0;
                    *p = 0;
                }
            }
        } else if (*p == '\'') {
            f ^= 1;
            p++;
            if (*p)
                goto LLL1;

        } else if (*p == '=') {
            f = 2;
            p++;
            if (*p)
                goto LLL1;

        } else if (*p == '-') {
            if (opts_.scan(p) == false)
                return false;
            if (opts_.resFileRq_) {
                if (resCfgFile_.getResFile(opts_.resFileRq_) == false)
                    return false;
                opts_.resFileRq_ = NULL;
            }
        } else if ((*p == '$' || *p == '@') && (p[1] >= '1' && p[1] <= '9' && p[2] == '=')) {
            unsigned    no  = p[1] - '0';
            char const* s   = p + 3;
            convFmt_.setVar(no, s, strlen(s));
        } else if (*p == '@') {
            if (resCfgFile_.getResFile(p+1) == false)
                return false;

        } else if (*p == '+') {
            ++p;
            if (fks_pathIsSep(*p) || fks_pathHasDrive(p)){
                fks_fileFullpath(&fileName_[0], fileName_.capacity(), p);
            } else {
                FPathBuf fbuf;
                fks_pathCpy(&fbuf[0], fbuf.capacity(), argv[0]);
                fks_pathDelBaseName(&fbuf[0]);
                fks_pathCat(&fbuf[0], fbuf.capacity(), p);
                fks_fileFullpath(&fileName_[0], fileName_.capacity(), &fbuf[0]);
            }
            fks_pathSetDefaultExt(&fileName_[0], fileName_.capacity(), &fileName_[0], "cfg");

        } else if (*p == ':') {
            if (p[1] == '#') {
                fprintf(stderr, ABXMSG(colon_hash_string_can_not_be_specified), p); // :#STRING is NG
                return false;
            }
            if (resCfgFile_.getCfgFile(&fileName_[0], p) == false)
                return false;

        } else if (opts_.needOptPath()) {
            opts_.setNeedOptPath(p);
        } else {
            filenameList_.push_back(p);
        }
    }

   #ifdef ENABLE_MT_X
    if (opts_.threadSize_ && (!beforeStrList_.empty() || !afterStrList_.empty())) {
        fprintf(stderr, ABXMSG(xm_and_Hbegin_can_not_be_used_at_the_same_time));
        return false;
    }
   #endif

    convFmt_.setTmpDir(&opts_.tmpDir_[0]);

    // Run batch.
    if (opts_.execBatFlg_) {
        tmpFName_ = opts_.tmpDir_;
        fks_tmpFile(&tmpFName_[0], tmpFName_.capacity(), "abx_", ".bat");
        //printf("tmpfname=%s\n", &tmpFName_[0]);
        opts_.outName_  = tmpFName_;
    }

    // Set default if file attribute is not specified
    if (!(opts_.filesOpts_.srchAttr_ & SRCH_DIR)) {
        opts_.filesOpts_.srchAttr_ |= SRCH_FILE;
    }
    convFmt_.setIgnoreCaseFlag(opts_.ignoreCaseFlag_);

    // default format.
    if (fmtBuf_.empty()) {
        char c = convFmt_.odrCh();
        if (c == 0)
            c = '$';
        fmtBuf_ = c;
        if (opts_.filesOpts_.recursiveFlg_) {
            fmtBuf_ += "f\n";
        } else {
            fmtBuf_ += "c\n";
        }
    }
    if (strchr(fmtBuf_.c_str(), '\n') == NULL)
        fmtBuf_ += "\n";

    convFmt_.setFmtStr(fmtBuf_.c_str());

    return true;
}

void App::initPath() {
 #ifdef FKS_WIN32
    fks_getTmpEnv(&opts_.tmpDir_[0], opts_.tmpDir_.capacity());
    fks_getExePath(&fileName_[0], fileName_.capacity());
    fks_pathSetExt(&fileName_[0], fileName_.capacity(), "cfg");
 #else // defined FKS_LINUX
    char* home = getenv("HOME");
    if (home && *home == '/') {
        fileName_ = home;
        fks_pathDelLastSep(&fileName_[0]);
        opts_.tmpDir_ = fileName_;
        fileName_ += "/.abx/abx.cfg";
        opts_.tmpDir_ += "/.abx/tmp";
        //printf("tmpDir=%s\n", &opts_.tmpDir_[0]);
        fks_recursiveMkDir(&opts_.tmpDir_[0]);
    } else {
        opts_.tmpDir_ = "/tmp";
        //printf("!tmpDir=%s\n", &opts_.tmpDir_[0]);
    }
 #endif
}


bool App::genText() {

    if (opts_.addBatHeaderFooterFlg_)
        convFmt_.writeLine0("@echo off");

    // #begin text.
    for (StrList::iterator ite = beforeStrList_.begin(); ite != beforeStrList_.end(); ++ite) {
        convFmt_.writeLine0(ite->c_str());
    }

    convFmt_.setRecursiveDirFlag(opts_.filesOpts_.recursiveFlg_);
    FKS_ULLONG topN = (opts_.topN_) ? opts_.topN_ : (FKS_ULLONG)(FKS_LLONG)-1;
    if (!opts_.noFindFile_ && !opts_.sirialNumEnd_) {   //  File-by-file processing.
        convFmt_.setNum(opts_.sirialNumStart_);
        for (PathStats::const_iterator ite = files_.pathStats().begin(); ite != files_.pathStats().end() && topN > 0; ++ite, --topN) {
            Fks_DirEntPathStat const* ps = *ite;
            convFmt_.write(ps->path, ps->stat);
       }
    } else {
        fks_stat_t  st   = { 0 };
        opts_.noFindFile_ = 1;
        convFmt_.setNoFindFile(opts_.noFindFile_ != 0);
        convFmt_.setNum(opts_.sirialNumStart_);
        if (opts_.sirialNumEnd_) {  // Processing by sequential number generation.
            char*   path = &fileName_[0];
            for (FKS_ULLONG num = opts_.sirialNumStart_; num <= opts_.sirialNumEnd_ && topN > 0; ++num, --topN) {
                convFmt_.setNum(num);
                snprintf(path, fileName_.capacity(), "%" PRIF_LL "u", (PRIF_ULLONG)(num));
                convFmt_.write(path, &st);
            }
        } else {    // Processing with raw string.
            for (StrList::iterator ite = filenameList_.begin(); ite != filenameList_.end() && topN > 0; ++ite, --topN) {
                char const* path = ite->c_str();
                convFmt_.write(path, &st);
            }
        }
    }

    // #end text
    for (StrList::iterator ite = afterStrList_.begin(); ite != afterStrList_.end(); ++ite) {
        convFmt_.writeLine0(ite->c_str());
    }

    if (opts_.addBatHeaderFooterFlg_)
        convFmt_.writeLine0(":END");

    return true;
}

bool App::outputText() {
    if (!opts_.outName_.empty()) {
        outFp_ = fopenX(opts_.outName_.c_str(), "wt");
        if (!outFp_) {
            return false;
        }
    } else {
        outFp_ = stdout;
    }

 #ifdef FKS_WIN32
    bool            utf8flg     = opts_.forceUtf8OutFlag_;
    StrList const&  outBufList  = convFmt_.outBufList();
    for (StrList::const_iterator ite = outBufList.begin(); ite != outBufList.end(); ++ite) {
        fprintf(outFp_, "%s", FKS_OUT_S(ite->c_str(), utf8flg));
    }
 #else
    StrList const&  outBufList  = convFmt_.outBufList();
    for (StrList::const_iterator ite = outBufList.begin(); ite != outBufList.end(); ++ite) {
        fprintf(outFp_, "%s", ite->c_str());
    }
 #endif

    if (!opts_.outName_.empty()) {
        fclose(outFp_);
        outFp_ = NULL;
    }
    return true;
}

bool App::execBat() {
    if (opts_.execBatFlg_) {
     #ifndef FKS_WIN32  // FKS_LINUX
        fks_chmod(&opts_.outName_[0], 0740);
     #endif
     #ifdef ENABLE_MT_X
        if (opts_.threadSize_) {
            StrList&                    buf = convFmt_.outBufList();
            std::vector<std::string>    cmds(buf.size());
            std::copy(buf.begin(), buf.end(), cmds.begin());
            convFmt_.clearOutBufList();
            mtCmd(cmds, opts_.threadSize_-1);
        } else
     #endif
        {
         #if 1
            system(opts_.outName_.c_str());
         #else
            char* p = getenv("COMSPEC");
            spawnl( _P_WAIT, p, p, "/c", opts_.outName_.c_str(), NULL);
         #endif
        }
    }
    return true;
}


/** Program start
 */
#if defined(FKS_USE_LONGFNAME) && defined(FKS_HAS_WMAIN)
int wmain(int argc, wchar_t *wargv[]) {
    static App app;
    fks_ioMbsInit(1,0);
    char** argv = fks_convArgWcsToMbs(argc, wargv);
    return app.main(argc, argv);
}
#else
int main(int argc, char *argv[]) {
    static App app;
    return app.main(argc, argv);
}
#endif
