/**
 *  @file   abx.cpp
 *  @brief  �t�@�C�����������A�Y���t�@�C�����𕶎���ɖ���(�o�b�`����)
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1995-2017
 *  @note
 *      license
 *          Boost Software License Version 1.0
 *          see license.txt
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <process.h>
#include <algorithm>
#include <string>
#include <list>
#include <set>
//#define __STDC_LIMIT_MACROS
//#include <inttypes.h>
#ifdef _WIN32
 #include <windows.h>
#endif

#include "subr.hpp"
#include "StrzBuf.hpp"

#ifdef ENABLE_MT_X
#include "abxmt.h"
#endif



/*---------------------------------------------------------------------------*/

#define APP_HELP_TITLE      "abx v3.91(pre v4) ̧�ٖ�������,�Y��̧�ٖ��𕶎���ɖ���(�ޯ�����).\n"	\
							"  https://github.com/tenk-a/abx.git      Boost Software License Version 1.0\n"
#define APP_HELP_CMDLINE    "usage : %s [��߼��] ['�ϊ�������'] ̧�ٖ� [=�ϊ�������]\n"
#define APP_HELP_OPTS       "��߼��:                        ""�ϊ�����:            �ϊ���:\n"                       \
                            " -x[-]    �ޯ����s -x-���Ȃ�   "" $f ���߽(�g���q�t)   d:\\dir\\dir2\\filename.ext\n"  \
                            " -xm[N]   N�X���b�h���s.0����  "" $g ���߽(�g���q��)   d:\\dir\\dir2\\filename\n"      \
                            " -r[-]    �ިڸ�؍ċA          "" $v ��ײ��            d\n"                            \
                            " -a[nrhsd] �w��̧�ّ����Ō���  "" $p �ިڸ��(��ײ�ޕt) d:\\dir\\dir2\n"                \
                            "          n:��� s:���� h:�B�� "" $d �ިڸ��(��ײ�ޖ�) \\dir\\dir2\n"                  \
                            "          r:ذ�޵�ذ d:�ިڸ�� "" $c ̧��(�g���q�t)    filename.ext\n"                 \
                            " -z[N-M]  ����N�`M��FILE������ "" $x ̧��(�g���q��)    filename\n"                     \
                            " -d[A-B]  ���tA�`B��FILE������ "" $e �g���q            ext\n"                          \
                            " -s[neztam][r] ���(����)       "" $w �����إ�ިڸ��    (���ϐ�TMP�̓��e)\n"          \
                            "          n:�� e:�g���q z:���� "" $z ����(10�i10��)    1234567890 ��$Z�Ȃ�16�i8��\n"   \
                            "          t:���t a:���� r:�~�� "" $j ����              1993-02-14\n"                   \
                            "          m:��(��)             "" $i �A�Ԑ���          ��$I�Ȃ�16�i��\n"               \
                            " -n[-]    ̧�ٌ������Ȃ� -n-�L "" $$ $  $[ <  $` '  $n ���s  $t ���\n"                 \
                            " -u[-]    $c|$C��̧�ٖ��召����"" $# #  $] >  $^ \"  $s ��  $l �����͂̂܂�\n"       \
                            " -l[-]    @���͂Ŗ��O�͍s�P��  ""------------------------------------------------\n"   \
                            " -ci[N:M] N:$i�̊J�n�ԍ�(M:�I) ""-p<DIR>  $p�̋����ύX   ""-ct<FILE> FILE���V�Ȃ�\n" \
                            " +CFGFILE .CFĢ�َw��         ""-e<EXT>  ��̫�Ċg���q   ""-ck[-] ���{�ꖼ�̂݌���\n"  \
                            " @RESFILE ڽ��ݽ̧��           ""-o<FILE> �o��̧�َw��   ""-cy[-] \\���܂ޑS�p������\n"\
                            " :�ϊ���  CFG�Œ�`�����ϊ�    ""-i<DIR>  �����ިڸ��    ""-y     $cxfgdpw��\"�t��\n"  \
                            " :        �ϊ����ꗗ��\��     ""-w<DIR>  TMP�ިڸ��     ""-t[N]  �ŏ���N�̂ݏ���\n" \


enum { OBUFSIZ  = 0x80000 };    /* ��`�t�@�C�����̃T�C�Y               */
enum { FMTSIZ   = 0x80000 };    /* ��`�t�@�C�����̃T�C�Y               */


typedef std::list<std::string>  StrList;
typedef StrzBuf<FIL_NMSZ>       FnameBuf;



/*--------------------- �G���[�����t���̕W���֐� ---------------------------*/

/** exit�I������ printf
 */
volatile void err_printfE(char const* fmt, ...) {
    va_list app;
    va_start(app, fmt);
    /*  fprintf(stdout, "%s %5d : ", src_name, src_line);*/
    vfprintf(stderr, fmt, app);
    va_end(app);
    exit(1);
}


/** �G���[������Α�exit�� fopen()
 */
FILE *fopenE(char const* name, char const* mod) {
    FILE *fp = fopen(name,mod);
    if (fp == NULL) {
        err_printfE("�t�@�C�� %s ���I�[�v���ł��܂���\n", name);
    }
    setvbuf(fp, NULL, _IOFBF, 1024*1024);
    return fp;
}

#if 0
/** �G���[������Α�exit�� fwrite()
 */
size_t  fwriteE(void const* buf, size_t sz, size_t num, FILE *fp) {
    size_t l = fwrite(buf, sz, num, fp);
    if (ferror(fp)) {
        err_printfE("�t�@�C�������݂ŃG���[����\n");
    }
    return l;
}
#endif

/** �G���[������Α�exit�� fread()
 */
size_t  freadE(void* buf, size_t sz, size_t num, FILE *fp) {
    size_t l = fread(buf, sz, num, fp);
    if (ferror(fp)) {
        err_printfE("�t�@�C���Ǎ��݂ŃG���[����\n");
    }
    return l;
}


/*---------------------------------------------------------------------------*/

class ConvFmt;

enum SortType {
    ST_NONE = 0x00,
    ST_NAME = 0x01,     // ���O�Ń\�[�g.
    ST_EXT  = 0x02,     // �g���q.
    ST_SIZE = 0x04,     // �T�C�Y.
    ST_DATE = 0x08,     // ���t/����.
    ST_ATTR = 0x10,     // �t�@�C������.
    ST_NUM  = 0x20,     // ���l��r�̖��O.
    //ST_MASK = 0x7F,   // �\�[�g���}�X�N
};

enum FileAttr {
    FA_Norm   = 0x100,
    FA_RdOnly = 0x001,
    FA_Hidden = 0x002,
    FA_Sys    = 0x004,
    FA_Volume = 0x008,
    FA_Dir    = 0x010,
    FA_Arcive = 0x020,
    FA_MASK   = 0x03f,
    FA_MASK_NOARC = 0xDF/*0x1f*/,
};

class FSrh {
public:
    FSrh()
        : recFlg_(false)
        , normalFlg_(true)
        , topFlg_(false)
        , nonFileFind_(false)
        , zenFlg_(false)
        , sortRevFlg_(false)
        , uplwFlg_(false)
        , sortType_(ST_NONE)
        , fattr_(FA_MASK)
        , knjChk_(0)
        , topN_(0)
        , topCnt_(0)
        , szMin_(0)
        , szMax_(0)
        , dateMin_(0)
        , dateMax_(0)
        , outFp_(NULL)
        , convFmt_(NULL)
        , membFunc_(NULL)
        , fpath_()
        , fname_()
    {
    }

    FSrh( unsigned atr, bool recFlg, bool zenFlg,
             size_t topN, SortType sortType, bool sortRevFlg, bool upLwFlg, int knjChk,
             size_t szmin, size_t szmax,
             unsigned short dtmin, unsigned short dtmax,
             FILE* outFp, ConvFmt* pConvFmt,
             int (ConvFmt::*fun)(char const* apath, FIL_FIND const* aff))
        : recFlg_(recFlg)
        , normalFlg_(false)
        , topFlg_(topN != 0)
        , nonFileFind_(false)
        , zenFlg_(zenFlg)
        , sortRevFlg_(sortRevFlg)
        , uplwFlg_(upLwFlg)
        , sortType_(sortType)
        //, fattr_(FA_MASK)
        , knjChk_(knjChk)
        , topN_(topN)
        , topCnt_(0)
        , szMin_(szmin)
        , szMax_(szmax)
        , dateMin_(dtmin)
        , dateMax_(dtmax)
        , outFp_(outFp)
        , convFmt_(pConvFmt)
        , membFunc_(fun)
        , fpath_()
        , fname_()
    {
        if (atr & FA_Norm) {
            normalFlg_ = true;
        }
        atr &= FA_MASK/*0xff*/;
        fattr_ = atr;
        //FIL_SetZenMode(zenFlg);
    }

    int findAndDo(char const* path, bool nonFileFind)
    {
        nonFileFind_ = nonFileFind;
        FIL_SetZenMode(zenFlg_);
        /*printf("%lu(%lx)-%lu(%lx)\n",szmin,szmin,szmax,szmax);*/
        /*printf("date %04x-%04x\n",dtmin,dtmax);*/
        FIL_FullPath(path, &fpath_[0]);
        char *p = STREND(&fpath_[0]);
        if (p[-1] == ':' || p[-1] == '\\' || p[-1] == '/')
            fpath_ += "*";
        p = FIL_BaseName(fpath_.c_str());
        fname_ = p;
        if (nonFileFind_) {   /* �t�@�C���������Ȃ��ꍇ */
            FIL_FIND ff;
            memset(&ff, 0, sizeof ff);
            if (   (knjChk_==0)
                || (knjChk_==1  && chkKnjs(fname_.c_str()))
                || (knjChk_==2  && strchr(fname_.c_str(),'\\'))
                || (knjChk_==-1 && !chkKnjs(fname_.c_str()))
                || (knjChk_==-2 && !strchr(fname_.c_str(),'\\')) )
            {
                (convFmt_->*membFunc_)(fpath_.c_str(), &ff);
            }
            return 0;
        }
        /* �t�@�C����������ꍇ */
        *p = 0;
        if (sortType_ != ST_NONE) {  /* �\�[�g���� */
            return findAndDo_subSort();
        }
        return findAndDo_sub();
    }

private:
    static int chkKnjs(const char *p)
    {
        unsigned char c;
        while((c = *(unsigned char *)p++) != 0) {
            if (c & 0x80)
                return 1;
        }
        return 0;
    }


    struct FileFindNameCmp {
        bool operator()(FIL_FIND const& l, FIL_FIND const& r) const {
         #ifdef _WIN32
            return _stricmp(l.name, r.name) < 0;
         #else
            return strcmp(l.name, r.name) < 0;
         #endif
        }

    private:
        FSrh const*     pFSrh_;
    };
    typedef std::set<FIL_FIND, FileFindNameCmp> FileFindDirTree;


   #ifdef _WIN32
    #define FNAME_GET_C(c, p) do {                              \
            (c) = *(unsigned char*)((p)++);                     \
            if (IsDBCSLeadByte(BYTE(c)) && *(p))                \
                (c) = ((c) << 8) | *(unsigned char*)((p)++);    \
        } while (0)
   #else
    #define FNAME_GET_C(c, p)   ((c) = *((p)++))
   #endif

    static int fnameCmp(char const* l, char const* r) {
        #ifdef _WIN32
            int rc = _stricmp(l,r);
            if (rc != 0)
                return rc;
            return strcmp(l,r);
        #else
            return strcmp(l,r);
        #endif
    }

    /** �t�@�C�����̑召��r. ���l���������ꍇ�A�����Ⴂ�̐��l���m�̑召�𔽉f
    *   �召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
    *   �ȊO�͒P���ɕ������r.
    */
    static int fnameNDigitCmp(const char* l, const char* r, size_t len) {
        const char* e = l + len;
        if (e < l)
            e = (const char*)-1;
        while (l < e) {
            unsigned    lc;
            unsigned    rc;

            FNAME_GET_C(lc, l);
            FNAME_GET_C(rc, r);

            if (lc <= 0x80 && isdigit(lc) && rc <= 0x80 && isdigit(rc)) {
                uint64_t   lv = uint64_t(strtoull(l - 1, (char**)&l, 10));
                uint64_t   rv = uint64_t(strtoull(r - 1, (char**)&r, 10));
                int64_t	   n  = int64_t(lv - rv);
                if (n == 0)
                    continue;
                return (n < 0) ? -1 : 1;
            }

            if (lc < 0x80)
                lc = tolower(lc);
            if (rc < 0x80)
                rc = tolower(rc);

            int64_t n  = int64_t(lc - rc);
            if (n == 0) {
                if (lc == 0)
                    return 0;
                continue;
            }

            if ((lc == '/' && rc == '\\') || (lc == '\\' && rc == '/')) {
                continue;
            }

            return (n < 0) ? -1 : 1;
        }
        return 0;
    }
    #undef FNAME_GET_C

    int  fileStatCmp(FIL_FIND const* f1, FIL_FIND const* f2) const {
        int n = 0;

        if (sortType_ == ST_NUM) {                      /* ���������͐��l�Ŕ�r���閼�O�\�[�g */
            n = fnameNDigitCmp(f1->name, f2->name, (size_t)-1);
            if (sortRevFlg_)
                return -n;
            return n;
        }
        if (sortType_ <= ST_NAME) {                        /* ���O�Ń\�[�g */
            n = fnameCmp(f1->name, f2->name);
            if (sortRevFlg_)
                return -n;
            return n;
        }

        if (sortType_ == ST_EXT) {                     /* �g���q */
            char const* p = strrchr(f1->name, '.');
            p = (p == NULL) ? "" : p;
            char const* q = strrchr(f2->name, '.');
            q = (q == NULL) ? "" : q;
            n = fnameCmp(p,q);

        } else if (sortType_ == ST_SIZE) {              /* �T�C�Y */
            long t = f1->size - f2->size;
            n = (t > 0) ? 1 : (t < 0) ? -1 : 0;

        } else if (sortType_ == ST_DATE) {              /* ���� */
            int64_t t = f1->time_write - f2->time_write;
            n = (t > 0) ? 1 : (t < 0) ? -1 : 0;
        } else if (sortType_ == ST_ATTR) {              /* ���� */
            /* �A�[�J�C�u�����͎ז��Ȃ̂ŃI�t���� */
            n = ((int)f2->attrib & FA_MASK_NOARC) - ((int)f1->attrib & FA_MASK_NOARC);
        }

        if (n == 0) {
            n = fnameCmp(f1->name, f2->name);
            //if (sortRevFlg_)
            //    n = -n;
        }
        if (sortRevFlg_)
            return -n;
        return n;
    }

    struct FileFindCmp {
        FileFindCmp(FSrh const* pFSrh=NULL) : pFSrh_(pFSrh) {}

        bool operator()(FIL_FIND const& l, FIL_FIND const& r) const {
            return pFSrh_->fileStatCmp(&l, &r) < 0;
        }

    private:
        FSrh const*     pFSrh_;
    };
    typedef std::set<FIL_FIND, FileFindCmp>     FileFindTree;

    struct DoOne {
        DoOne(FSrh* pFSrh) : pFSrh_(pFSrh) {}

        void operator()(FIL_FIND const& ff) {
            if (pFSrh_->topFlg_) {
                if (pFSrh_->topCnt_ == 0)       /* �擪 N�݂̂̏����̂Ƃ� */
                    return;
                else
                    --pFSrh_->topCnt_;
            }
            char *t = STREND(&pFSrh_->fpath_[0]);
            strcpy(t, ff.name);
            (pFSrh_->convFmt_->*(pFSrh_->membFunc_))(pFSrh_->fpath_.c_str(), &ff);
            *t = 0;
        }

    private:
        FSrh*       pFSrh_;
    };

    struct DoOneDir {
        DoOneDir(FSrh* pFSrh) : pFSrh_(pFSrh) {}

        void operator()(FIL_FIND const& ff) {
            char *t = STREND(&pFSrh_->fpath_[0]);
            strcpy(t, ff.name);
            strcat(t, "\\");
            pFSrh_->findAndDo_subSort();
            *t = 0;
        }

    private:
        FSrh*     pFSrh_;
    };


    int findAndDo_subSort() {
        FIL_FIND_HANDLE hdl = 0;
        FIL_FIND        ff = {0};
        char*           t;

        if (topFlg_) {
            topCnt_ = topN_;
        }

        {
            FileFindTree    tree(FileFindCmp(this));
            t    = STREND(&fpath_[0]);
            strcpy(t, fname_.c_str());
            hdl = FIL_FINDFIRST(fpath_.c_str(), fattr_, &ff);
            if (FIL_FIND_HANDLE_OK(hdl)) {
                do {
                    *t = '\0';
                    if (normalFlg_ == 0 && (fattr_ & ff.attrib) == 0)
                        continue;
                    if ((fattr_ & FA_Dir) == 0 && (ff.attrib & FA_Dir))   /* �f�B���N�g�������łȂ��̂Ƀf�B���N�g�������������΂� */
                        continue;
                    if(  (ff.name[0] != '.')
                      && (  (szMin_ > szMax_) || ((int)szMin_ <= ff.size && ff.size <= (int)szMax_) )
                      && (  (dateMin_ > dateMax_) || (dateMin_ <= ff.wr_date && ff.wr_date <= dateMax_) )
                      && (  (knjChk_==0) || (knjChk_==1 && chkKnjs(ff.name)) || (knjChk_==2 && strchr(ff.name,'\\'))
                                             || (knjChk_==-1&& !chkKnjs(ff.name))|| (knjChk_==-2&& !strchr(ff.name,'\\'))  )
                      )
                    {
                        tree.insert(ff);
                    }
                } while (FIL_FINDNEXT(hdl, &ff) == 0);
                FIL_FINDCLOSE(hdl);
            }
            std::for_each(tree.begin(), tree.end(), DoOne(this));
        }

        if (recFlg_ /*&& nonFileFind_ == 0*/) {
            FileFindDirTree     dirTree;
            strcpy(t,"*.*");
            hdl = FIL_FINDFIRST(fpath_.c_str(), FA_Dir, &ff);
            if (FIL_FIND_HANDLE_OK(hdl)) {
                do {
                    *t = '\0';
                    if ((ff.attrib & FA_Dir) && strcmp(ff.name, ".") && strcmp(ff.name, "..")) {
                        dirTree.insert(ff);
                    }
                } while (FIL_FINDNEXT(hdl, &ff) == 0);
                FIL_FINDCLOSE(hdl);
            }
            std::for_each(dirTree.begin(), dirTree.end(), DoOneDir(this));
        }
        return 0;
    }



    int findAndDo_sub() {
        FIL_FIND_HANDLE hdl;
        FIL_FIND        ff = {0};

        if (topFlg_) {
            topCnt_ = topN_;
        }
        char *t = STREND(&fpath_[0]);
        strcpy(t, fname_.c_str());
        hdl = FIL_FINDFIRST(fpath_.c_str(), fattr_, &ff);
        if (FIL_FIND_HANDLE_OK(hdl)) {
            do {
                *t = '\0';
                if (normalFlg_ == 0 && (fattr_ & ff.attrib) == 0)
                    continue;
                if ((fattr_ & FA_Dir) == 0 && (ff.attrib & FA_Dir))   /* �f�B���N�g�������łȂ��̂Ƀf�B���N�g�������������΂� */
                    continue;
                if(  (ff.name[0] != '.')
                  && (  (szMin_ > szMax_) || ((int)szMin_ <= ff.size && ff.size <= (int)szMax_) )
                  && (  (dateMin_ > dateMax_) || (dateMin_ <= ff.wr_date && ff.wr_date <= dateMax_) )
                  && (  (knjChk_==0) || (knjChk_==1 && chkKnjs(ff.name)) || (knjChk_==2 && strchr(ff.name,'\\'))
                                         || (knjChk_==-1&& !chkKnjs(ff.name))|| (knjChk_==-2&& !strchr(ff.name,'\\'))  )
                  )
                {
                    strcpy(t, ff.name);
                    (convFmt_->*membFunc_)(fpath_.c_str(), &ff);
                    *t = 0;
                    if (topFlg_ && --topCnt_ == 0) {    /* �擪 N�݂̂̏����̂Ƃ� */
                        return 0;
                    }
                }
            } while (FIL_FINDNEXT(hdl, &ff) == 0);
            FIL_FINDCLOSE(hdl);
        }

        if (recFlg_) {
            strcpy(t,"*.*");
            hdl = FIL_FINDFIRST(fpath_.c_str(), 0x10, &ff);
            if (FIL_FIND_HANDLE_OK(hdl)) {
                do {
                    *t = '\0';
                    if ((ff.attrib & FA_Dir) && ff.name[0] != '.') {
                        strcpy(t, ff.name);
                        strcat(t, "\\");
                        findAndDo_sub();
                    }
                } while (FIL_FINDNEXT(hdl, &ff) == 0);
                FIL_FINDCLOSE(hdl);
            }
        }
        return 0;
    }


private:
    bool            recFlg_;        // 1:�ċA���� 0:���Ȃ�
    bool            normalFlg_;     // ɰ�٥̧�ق�ϯ� 1:���� 0:���Ȃ�
    bool            topFlg_;
    bool            nonFileFind_;   // 1:�t�@�C���������Ȃ� 0:����
    bool            zenFlg_;
    bool            sortRevFlg_;
    bool            uplwFlg_;
    SortType        sortType_;
    unsigned        fattr_;         // ����̧�ّ���
    int             knjChk_;
    size_t          topN_;
    size_t          topCnt_;
    size_t          szMin_;
    size_t          szMax_;
    unsigned short  dateMin_;
    unsigned short  dateMax_;
    FILE*           outFp_;
    ConvFmt*        convFmt_;
    int  (ConvFmt::*membFunc_)(char const* path, FIL_FIND const* ff);
    FnameBuf        fpath_;
    FnameBuf        fname_;
};



/*---------------------------------------------------------------------------*/

class ConvFmt {
public:
    enum { VAR_NMSZ = FIL_NMSZ };

    ConvFmt()
        : upLwrFlg_(false)
        , autoWqFlg_(false)
        , num_(0)
        , numEnd_(0)
        , fmtBuf_(NULL)
        , lineBuf_(NULL)
        , drv_()
        , dir_()
        , name_()
        , ext_()
        , tmpDir_()
        , pathDir_()
        , chgPathDir_()
        , tgtnm_()
        , tgtnmFmt_()
        //, var_()
        , obuf_()
    {
        FIL_GetTmpDir(&tmpDir_[0]); /* �e���|�����f�B���N�g�����擾 */
    }

    void setChgPathDir(char const* dir) {
        FIL_FullPath(dir, &chgPathDir_[0]);
        char* p = STREND(&chgPathDir_[0]);
        if (p[-1] == '\\' || p[-1] == '/') {
            p[-1] = '\0';
        }
    }

    char const* tmpDir() const { return tmpDir_.c_str(); }

    void setTmpDir(char const* dir) {
        tmpDir_ = dir;
        FIL_GetTmpDir(&tmpDir_[0]); /* �e���|�����f�B���N�g�����擾 */
    }

    void setTgtnmFmt(char const* tgt) {
        tgtnmFmt_ = tgt;
    }

    void clearVar() {
        for (int l = 0; l < 10; l++)
            var_[l].clear();
    }

    char const* getVar(unsigned n) const {
        return (n < 10) ? var_[n].c_str() : "";
    }

    bool setVar(unsigned m, char const* p, size_t l) {
        if (l < 1 || l >= (var_[0].capacity() - 1))
            return false;
        var_[m].assign(p, p + l);
        return true;
    }

    void setNum(size_t num) { num_ = num; }

    void setUpLwrFlag(bool sw) { upLwrFlg_ = sw; }

    void setAutoWq(bool sw) { autoWqFlg_ = sw; }

    void setLineBuf(char const* lineBuf) { lineBuf_ = lineBuf; }

    void setFmtStr(char const* fmtBuf) {
        fmtBuf_ = fmtBuf;
    }


    int write(char const* fpath, FIL_FIND const* ff) {
        splitPath(fpath);

        StrFmt(&tgtnm_[0], tgtnmFmt_.c_str(), tgtnm_.capacity(), ff);           // ����̃^�[�Q�b�g����ݒ�
        if (tgtnmFmt_.empty() || FIL_FdateCmp(tgtnm_.c_str(), fpath) < 0) { 	// ���t��r���Ȃ����A����ꍇ�̓^�[�Q�b�g���Â����
            StrFmt(&obuf_[0], &fmtBuf_[0], obuf_.capacity(), ff);
		 #if 1
			outBuf_.push_back(obuf_.c_str());
		 #else
            fprintf(fp, "%s", obuf_.c_str());
         #endif
        }
        ++num_;
        return 0;
    }


    int writeLine0(char const* s) {
        char* p = &obuf_[0];
        char c;
        while ((c = (*p++ = *s++)) != '\0') {
            if (c == '$') {
                --p;
                c = *s++;
                if (c == '$') {
                    *p++ = '$';
                } else if (c >= '1' && c <= '9') {
                    p = STPCPY(p, var_[c-'0'].c_str());
                } else {
                    fprintf(stderr,"Incorrect '$' format : '$%c'\n",c);
                    /*fprintfE(stderr,"���X�|���X���� $�w�肪��������(%c)\n",c);*/
                    exit(1);
                }
            }
        }
	 #if 1
		strcat(&obuf_[0], "\n");
		outBuf_.push_back(obuf_.c_str());
	 #else
        fprintf(fp, "%s\n", obuf_.c_str());
	 #endif
        return 0;
    }

	StrList&		outBuf() { return outBuf_; }
	StrList const&	outBuf() const { return outBuf_; }
	void			clearOutBuf() { StrList().swap(outBuf_); }

private:
    char *stpCpy(char *d, char const* s, ptrdiff_t clm, int flg) {
        unsigned char c;
        size_t        n = 0;
        if (upLwrFlg_ == 0) {
            strcpy(d,s);
            n = strlen(s);
            d = d + n;
        } else if (flg == 0) {  /* �啶���� */
            while ((c = *(unsigned char *)s++) != '\0') {
                if (islower(c))
                    c = (unsigned char)toupper(c);
                *d++ = (char)c;
                n++;
                if (ISKANJI(c) && *s && FIL_GetZenMode()) {
                    *d++ = *s++;
                    n++;
                }
            }
        } else {        /* �������� */
            while ((c = *(unsigned char *)s++) != '\0') {
                if (isupper(c))
                    c = (unsigned char)tolower(c);
                *d++ = (char)c;
                n++;
                if (ISKANJI(c) && *s && FIL_GetZenMode()) {
                    *d++ = *s++;
                    n++;
                }
            }
        }
        clm -= (ptrdiff_t)n;
        while (clm > 0) {
            *d++ = ' ';
            --clm;
        }
        *d = '\0';
        return d;
    }

    void splitPath(char const* fpath) {
        FIL_SplitPath(&fpath[0], &drv_[0], &dir_[0], &name_[0], &ext_[0]);

        FIL_DelLastDirSep(&dir_[0]);  /* �f�B���N�g�����̌���'\'���͂��� */
        pathDir_ = drv_;
        pathDir_ += dir_;
        if (!chgPathDir_.empty()) {
            pathDir_ = chgPathDir_;
        }
        /* �g���q�� '.' ���͂��� */
        if (ext_[0] == '.') {
            memmove(&ext_[0], &ext_[1], strlen(&ext_[1])+1);
        }
    }


    void StrFmt(char *dst, char const* fmt, int sz, FIL_FIND const* ff) {
        char    buf[FIL_NMSZ*4] = {0};
        char    *b;
        int     f,n;
        char    drv[2];
        drv[0] = drv_[0];
        drv[1] = 0;

        char const* s = fmt;
        char*       p = dst;
        char*       pe = p + sz;
        char        c;
        while ((c = (*p++ = *s++)) != '\0' && p < pe) {
            if (c == '$') {
                --p;
                n = -1;
                c = *s++;
                if (c == '+') { /* +NN �͌����w�肾 */
                    n = strtoul(s,(char**)&s,10);
                    if (s == NULL || *s == 0)
                        break;
                    if (n >= FIL_NMSZ)
                        n = FIL_NMSZ;
                    c = *s++;
                }
                f = islower(c);
                switch (toupper(c)) {
                case 'S':   *p++ = ' ';     break;
                case 'T':   *p++ = '\t';    break;
                case 'N':   *p++ = '\n';    break;
                case '$':   *p++ = '$';     break;
                case '#':   *p++ = '#';     break;
                case '[':   *p++ = '<';     break;
                case ']':   *p++ = '>';     break;
                case '`':   *p++ = '\'';    break;
                case '^':   *p++ = '"';     break;

                case 'L':   p = stpCpy(p,lineBuf_,n,f);  break;
                case 'V':   p = stpCpy(p,drv,n,f);       break;
                case 'D':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p, dir_.c_str(), n, f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;
                case 'X':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p,name_.c_str(),n,f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;
                case 'E':
                    p = stpCpy(p,ext_.c_str(),n,f);
                    break;
                case 'W':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p, tmpDir_.c_str(), n, f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;
                case 'P':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p,pathDir_.c_str(),n,f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;

                case 'C':
                    b = buf;
                    if (autoWqFlg_) *b++ = '"';
                    b = stpCpy(b,name_.c_str(),0,f);
                    if (!ext_.empty()) {
                        b = STPCPY(b,".");
                        b = stpCpy(b,ext_.c_str(),0,f);
                    }
                    if (autoWqFlg_) *b++ = '"';
                    *b = 0;
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'F':
                    b = buf;
                    if (autoWqFlg_) *b++ = '"';
                    b = stpCpy(b,drv_.c_str(),0,f);
                    b = stpCpy(b,dir_.c_str(),0,f);
                    b = STPCPY(b,"\\");
                    b = stpCpy(b,name_.c_str(),0,f);
                    if (!ext_.empty()) {
                        b = STPCPY(b,".");
                        b =stpCpy(b,ext_.c_str(),0,f);
                    }
                    if (autoWqFlg_) *b++ = '"';
                    *b = 0;
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'G':
                    b = buf;
                    if (autoWqFlg_) *b++ = '"';
                    b = stpCpy(b,drv_.c_str(),0,f);
                    b = stpCpy(b,dir_.c_str(),0,f);
                    b = STPCPY(b,"\\");
                    b = stpCpy(b,name_.c_str(),0,f);
                    if (autoWqFlg_) *b++ = '"';
                    *b = '\0';
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'O':
                    stpCpy(buf, tgtnm_.c_str(), 0, f);
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'Z':
                    if (f) {
                        if (n < 0)
                            n = 10;
                        p += sprintf(p, "%*d", n, ff->size);
                    } else {
                        if (n < 0)
                            n = 8;
                        p += sprintf(p, "%*X", n, ff->size);
                    }
                    break;

                case 'I':
                    if (f) {
                        if (n < 0)
                            n = 1;
                        p += sprintf(p, "%0*d", n, unsigned(num_));
                    } else {
                        if (n < 0)
                            n = 1;
                        p += sprintf(p, "%0*X", n, unsigned(num_));
                    }
                    break;

                case 'J':
                    {   int y = 0, m = 0, d = 0;
                     #if defined _MSC_VER && _MSC_VER >= 1400
                        struct tm* ltm = _localtime64(&ff->time_write);
                     #else
                        struct tm* ltm = localtime(&ff->time_write);
                     #endif
                        if (ltm) {
                            y = ltm->tm_year + 1900;
                            m = ltm->tm_mon  + 1;
                            d = ltm->tm_mday;
                        }
                        if (n < 0)
                            n = 10;
                        if (n >= 10) {
                            sprintf(buf, "%04d-%02d-%02d", y, m, d);
                        } else if (n >= 8) {
                            sprintf(buf, "%02d-%02d-%02d", y %100, m, d);
                        } else {
                            sprintf(buf, "%02d-%02d", m, d);
                        }
                        p += sprintf(p, "%-*s", n, buf);
                    }
                    break;
                default:
                    if (c >= '1' && c <= '9') {
                        p = STPCPY(p, var_[c-'0'].c_str());
                    } else {
                        fprintf(stderr, "Incorrect '$' format : '$%c'\n",c);
                        /*fprintfE(stderr,".cfg �� $�w�肪��������(%c)\n",c);*/
                        // exit(1);
                    }
                }
            }
        }
    }

private:
    bool                upLwrFlg_;
    bool                autoWqFlg_;     /* $f���Ŏ����ŗ��[��"��t�����郂�[�h. */
    size_t              num_;           /* $i �Ő�������ԍ� */
    size_t              numEnd_;        /* �A�Ԃ��t�@�C�����̕�����̑���ɂ���w��������ꍇ�̏I���A�h���X */
    char const*         fmtBuf_;        /* �ϊ�����������߂� */
    char const*         lineBuf_;
    FnameBuf            var_[10];
    FnameBuf            drv_;
    FnameBuf            dir_;
    FnameBuf            name_;
    FnameBuf            ext_;
    FnameBuf            tmpDir_;
    FnameBuf            pathDir_;
    FnameBuf            chgPathDir_;
    FnameBuf            tgtnm_;
    FnameBuf            tgtnmFmt_;
    StrzBuf<OBUFSIZ>    obuf_;          /* .cfg(.res) �ǂݍ��݂�A�o�͗p�̃o�b�t�@ */

	StrList				outBuf_;
};



/*--------------------------------------------------------------------------*/
class Opts {
public:
    ConvFmt&        rConvFmt_;
    bool            recFlg_;                /* �ċA�̗L�� */
    bool            zenFlg_;                /* MS�S�p�Ή� */
    bool            batFlg_;                /* �o�b�`���s */
    bool            batEx_;                 /* -b�̗L�� */
    bool            linInFlg_;              /* RES���͂��s�P�ʏ���*/
    bool            autoWqFlg_;
    bool            upLwrFlg_;
    bool            sortRevFlg_;
    bool            sortUpLw_;
    int            noFindFile_;            /* �t�@�C���������Ȃ� */
    int             knjChk_;                /* MS�S�p���݃`�F�b�N */
    unsigned        fattr_;                 /* �t�@�C������ */
    SortType        sortType_;              /* �\�[�g */
    size_t          topN_;                  /* ������ */
    char *          iname_;                 /* ���̓t�@�C���� */
    char const*     dfltExtp_;              /* �f�t�H���g�g���q */
    size_t          szmin_;                 /* szmin > szmax�̂Ƃ���r���s��Ȃ�*/
    size_t          szmax_;
    uint16_t		dtmin_;                 /* dtmin > dtmax�̂Ƃ���r���s��Ȃ�*/
    uint16_t  		dtmax_;
 #ifdef ENABLE_MT_X
	unsigned		nthread_;
 #endif
    size_t          renbanStart_;           /* �A�Ԃ̊J�n�ԍ�. ����0 */
    size_t          renbanEnd_;             /* �A�Ԃ̊J�n�ԍ�. ����0 */
    FnameBuf        outname_;               /* �o�̓t�@�C���� */
    FnameBuf        ipath_;                 /* ���̓p�X�� */
    FnameBuf        dfltExt_;               /* �f�t�H���g�g���q */
    FnameBuf        chgPathDir_;
    FnameBuf        exename_;

public:
    Opts(ConvFmt& rConvFmt)
        : rConvFmt_(rConvFmt)
        , recFlg_(false)
        , zenFlg_(true)
        , batFlg_(false)
        , batEx_(false)
        , linInFlg_(false)
        , autoWqFlg_(false)
        , upLwrFlg_(false)
        , sortRevFlg_(false)
        , sortUpLw_(false)
        , noFindFile_(0)
        , knjChk_(0)
        , fattr_(0)
        , sortType_(ST_NONE)
        , topN_(0)
        //, iname_(&ipath_[0])
        , dfltExtp_(NULL)
        , szmin_(~size_t(0))
        , szmax_(0)
        , dtmin_(0xFFFFU)
        , dtmax_(0)
	   #ifdef ENABLE_MT_X
		, nthread_(0)
	   #endif
        , renbanStart_(0)
        , renbanEnd_(0)
        , outname_()
        , ipath_()
        , dfltExt_()
        , chgPathDir_()
        , exename_()
    {
        iname_ = &ipath_[0];
    }

    void setExename(char const* exename) {
        exename_ = exename;
     #ifdef _WIN32
        StrLwrN(&exename_[0], exename_.size());
     #endif
    }

    bool scan(char* s) {
        char* p = s + 1;
        int  c = *(unsigned char*)p++;
        c = toupper(c);
        switch (c) {
        case 'X':
            batFlg_ = (*p != '-');
			//mt check
		 #ifdef ENABLE_MT_X
			if (batFlg_) {
				if (*p == 'm' || *p == 'M') {
					nthread_ = strtol(p+1, NULL, 0);
					++nthread_;
				}
			}
		 #endif
            break;

        case 'R':
            recFlg_ = (*p != '-');
            break;

        case 'U':
            upLwrFlg_ = (*p != '-');
            break;

        case 'N':
            noFindFile_ = (*p != '-');
            if (*p == 'd' || *p == 'D')	// -nd ����@�\���Ȃ�
                noFindFile_ = 2;
            break;

        case 'J':
            zenFlg_ = (*p != '-');
            break;

        case 'B':
            batEx_ = (*p != '-');
            break;

        case 'L':
            linInFlg_  = (*p != '-');
            break;

        case 'T':
            if (*p == 0) {
                topN_ = 1;
            } else {
                topN_ = strtol(p,NULL,0);
            }
            break;

        case 'C':
            c = toupper(*p);
            if (c == '-') {
                knjChk_ = 0;
            } else if (c == 'K') {
                knjChk_ = 1;
                if (p[1] == '-')
                    knjChk_ = -1;
            } else if (c == 'Y') {
                knjChk_ = 2;
                if (p[1] == '-')
                    knjChk_ = -2;
            } else if (c == 'T' /*|| c == 'F'*/) {  // 'F'�͋��݊�
                rConvFmt_.setTgtnmFmt(p + 1);
            } else if (c == 'I') {
                renbanStart_ = strtol(p+1, (char**)&p, 0);
                if (*p) {
                    renbanEnd_ = strtol(p+1, (char**)&p, 0);
                } else {
                    renbanEnd_ = 0;
                }
            } else {
                goto ERR_OPTS;
            }
            break;

        case 'Y':
            autoWqFlg_ = (*p != '-');
            break;

        case 'E':
            dfltExt_ = p;
            dfltExtp_ = dfltExt_.c_str();
            if (*p == '$' && p[1] >= '1' && p[1] <= '9' && p[2] == 0) {
                dfltExt_ = rConvFmt_.getVar(p[1]-'0');
            }
            /*dfltExt_[3] = 0;*/
            break;

        case 'O':
            if (*p == 0)
                goto ERR_OPTS;
            outname_ = p;
            break;

        case 'I':
            if (*p == 0)
                goto ERR_OPTS;
            FIL_FullPath(p, &ipath_[0]);
            p = STREND(&ipath_[0]);
            if (p[-1] != '\\' && p[-1] != '/') {
                *p++ = '\\';
                *p = '\0';
            }
            iname_ = p;
            break;

        case 'P':
            if (*p == 0)
                goto ERR_OPTS;
            rConvFmt_.setChgPathDir(p);
            break;

        case 'W':
            rConvFmt_.setTmpDir(p);
            break;

        case 'A':
            strupr(p);
            while (*p) {
                switch(*p) {
                case 'N': fattr_ |= FA_Norm;   break;
                case 'R': fattr_ |= FA_RdOnly; break;
                case 'H': fattr_ |= FA_Hidden; break;
                case 'S': fattr_ |= FA_Sys;    break;
                case 'V': fattr_ |= FA_Volume; break;
                case 'D': fattr_ |= FA_Dir;    break;
                case 'A': fattr_ |= FA_Arcive; break;
                }
                ++p;
            }
            break;

        case 'S':
            c = 0;
            sortType_ = ST_NAME;
            strupr(p);
            while (*p) {
                switch(*p) {
                case '-': sortType_ = ST_NONE; break;
                case 'N': sortType_ = ST_NAME; break;
                case 'E': sortType_ = ST_EXT;  break;
                case 'Z': sortType_ = ST_SIZE; break;
                case 'T': sortType_ = ST_DATE; break;
                case 'A': sortType_ = ST_ATTR; break;
                case 'M': sortType_ = ST_NUM;  break;
                case 'R': sortRevFlg_ = true;   break;
                }
                ++p;
            }
            break;

        case 'Z':
            szmin_ = (*p == '-') ? 0 : size_t( strtoull(p, &p, 0) );
            if (*p == 'K' || *p == 'k')         p++, szmin_ *= 1024UL;
            else if (*p == 'M' || *p == 'm')    p++, szmin_ *= 1024UL*1024UL;
            if (*p) { /* *p == '-' */
                szmax_ = 0xffffffffUL;
                p++;
                if (*p) {
                    szmax_ = size_t( strtoull(p,&p,0) );
                    if (*p == 'K' || *p == 'k')         p++, szmax_ *= 1024UL;
                    else if (*p == 'M' || *p == 'm')    p++, szmax_ *= 1024UL*1024UL;
                }
                if (szmax_ < szmin_)
                    goto ERR_OPTS;
            } else {
                szmax_ = szmin_;
            }
            break;

        case 'D':
            if (*p == 0) {
                dtmax_ = dtmin_ = 0;
            } else {
                unsigned long t;
                int y,m,d;
                if (*p == '-') {
                    dtmin_ = 0;
                } else {
                    t = strtoul(p,&p,10);
                    y = (int)((t/10000) % 100); y = (y >= 80) ? (y-80) : (y+100-80);
                    m = (int)((t / 100) % 100); if (m == 0 || 12 < m) goto ERR_OPTS;
                    d = (int)(t % 100);         if (d == 0 || 31 < d) goto ERR_OPTS;
                    dtmin_ = uint16_t((y<<9)|(m<<5)|d);
                }
                if (*p) {
                    p++;
                    dtmax_ = 0xFFFFU;
                    if (*p) {
                        t = strtoul(p,&p,10);
                        y = (int)(t/10000)%100; y = (y>=80) ? (y-80) : (y+100-80);
                        m = (int)(t/100) % 100; if (m==0 || 12 < m) goto ERR_OPTS;
                        d = (int)(t % 100);     if (d==0 || 31 < d) goto ERR_OPTS;
                        dtmax_ = uint16_t((y<<9)|(m<<5)|d);
                        if (dtmax_ < dtmin_)
                            goto ERR_OPTS;
                    }
                } else {
                    dtmax_ = dtmin_;
                }
            }
            break;

        case '?':
        case '\0':
            return usage();

        default:
      ERR_OPTS:
          #if 1
            err_printfE("�R�}���h���C���ł̃I�v�V�����w�肪�������� : %s\n", s);
          #else
            err_printfE("Incorrect command line option : %s\n", s);
          #endif
        }
        return true;
    }


    bool usage() {
        printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename_.c_str());
        printf("%s", APP_HELP_OPTS);
        return false;
    }
};


/*---------------------------------------------------------------------------*/

class ResCfgFile {
public:
    ResCfgFile(Opts& rOpts, ConvFmt& rConvFmt, StrList& rFileNameList, StrList& rBeforeList, StrList& rAfterList, char* fmtBuf)
        : rOpts_(rOpts)
        , rConvFmt_(rConvFmt)
        , rFileNameList_(rFileNameList)
        , rBeforeStrList_(rBeforeList)
        , rAfterStrList_(rAfterList)
        , fmtBuf_(fmtBuf)
        //, resP_(&resOBuf_[0])
        , varIdx_(1)
        //, varNo_[10]
        , resName_()
        , resOBuf_()
    {
        memset(varNo_, 0, sizeof(varNo_));
        resP_ = &resOBuf_[0];
    }

    /** ���X�|���X�t�@�C������
     */
    bool GetResFile(char const* name) {
        size_t  l;

        if (name[0] == 0) {                     /* �t�@�C�������Ȃ���ΕW������ */
            l = fread(&resOBuf_[0], 1, resOBuf_.capacity(), stdin);
        } else {
            resName_ = name;
            FIL_AddExt(&resName_[0], "abx");
            FILE* fp = fopenE(resName_.c_str(), "rt");
            l = freadE(&resOBuf_[0], 1, resOBuf_.capacity(), fp);
            fclose(fp);
        }
        resOBuf_[l] = 0;
        if (l == 0)
            return true;

        resP_ = &resOBuf_[0];
        return getFmts();  /* ���ۂ̃t�@�C�����e�̏��� */
    }

    /** ��`�t�@�C������
     */
    bool GetCfgFile(char *name, char *key) {
        FIL_FullPath(name, &resName_[0]);
        FILE*   fp = fopenE(resName_.c_str(),"r");
        size_t  l  = freadE(&resOBuf_[0], 1, resOBuf_.capacity(), fp);
        fclose(fp);
        resOBuf_[l] = 0;
        if (l == 0)
            return false;

        if (key[1] == 0) /* ':'�����̎w��̂Ƃ� */
            printf("':�ϊ���'�ꗗ\n");
        /*l = 1;*/
        /*   */
        strupr(key);
        resP_ = &resOBuf_[0];
        /* ���s+':'+�ϊ�����T�� */
        while ((resP_ = strstr(resP_, "\n:")) != NULL) {
            resP_ ++;
            char* p = getLine();
            if (p)
                p = strtok(p, " \t\r");
            /*printf("cmp %s vs %s\n",key,p);*/
            if (p == NULL || *p == 0)
                continue;
            strupr(p);
            if (key[1]) {
                /* �ϊ�����������΃��X�|���X�Ɠ������������� */
                if (keyStrEqu(key, p)) {
                    if ((p = strstr(resP_, "\n:")) != NULL) {
                        *p = '\0';
                    }
                    return getFmts();
                }
            } else {    /* �����L�[���Ȃ���΁A�ꗗ�\�� */
                printf("\t%s\n",p);
            }
        }
        if (key[1])
            err_printfE("%s �ɂ� %s �͒�`����Ă��Ȃ�\n", resName_.c_str(), key);
        exit(1);
        return false;
    }

private:
    /** resOBuf_�ɒ������e�L�X�g���P�s����
     * �s���̉��s�͍폜. resOBuf_�͔j��
     */
    char *getLine(void) {
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

    /** $���� �ϐ��̐ݒ�
     */
    char *setDoll(char *p0) {
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
            err_printfE(".cfg �t�@�C���� $�m �w��ł����������̂����� : $%s\n",p0);

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
            err_printfE(".cfg �t�@�C���� $�m=������w�� �܂��� $�m:�l{..}�w��ł����������̂����� : $%s\n",p0);
        }
      RET:
        return p;
    }

    /** s ���󔒂ŋ�؂�ꂽ�P��(�t�@�C����)��name �ɃR�s�[����.
     * ������ "file name" �̂悤��"�������"���폜���ւ�ɊԂ̋󔒂��c��.
     */
    char const* getFileNameStr(char *d, char const* s) {
        int f = 0;

        s = StrSkipSpc(s);
        while (*s) {
            if (*s == '"')
                f ^= 1;
            else if (f == 0 && (*(unsigned char *)s <= ' '))
                break;
            else
                *d++ = *s;
            s++;
        }
        *d = 0;
        return s;
    }

    /** �ϊ��t�H�[�}�b�g������擾
     */
    bool getFmts() {
        #define ISSPC(c)    ((unsigned char)c <= ' ')
        char    name[FIL_NMSZ];
        char*   p;
        char*   q;
        char*   d;
        enum Mode { MD_Body, MD_Bgn, MD_End, MD_TameBody };
        Mode    mode;

        d = fmtBuf_;
        mode = MD_Body;
        while ( (p = getLine()) != NULL ) {
            q = (char*)StrSkipSpc(p);
            if (strnicmp(q, "#begin", 6) == 0 && ISSPC(p[6])) {
                mode = MD_Bgn;
                continue;
            } else if (strnicmp(q, "#body", 5) == 0 && ISSPC(p[5])) {
                mode = MD_Body;
                continue;
            } else if (strnicmp(q, "#end", 4) == 0 && ISSPC(p[4])) {
                mode = MD_End;
                continue;
            }
            switch (mode) {
            case MD_Body: /* #body */
                while (p && *p) {
                    p = (char*)StrSkipSpc(p);  /* �󔒃X�L�b�v */
                    switch (*p) {
                    case '\0':
                    case '#':
                        goto NEXT_LINE;
                    case '\'':
                        if (p[1] == 0) {
                            err_printfE("���X�|���X�t�@�C��(��`�t�@�C����)��'�ϊ�������'�w�肪��������\n");
                        }
                        p++;
                        d = strchr(p, '\'');
                        if (d) {
                            *d = '\0';
                            d = STPCPY(&fmtBuf_[0], p);
                        }
                        break;
                    case '=':
                        d = &fmtBuf_[0];
                      #if 0
                        if (p[1]) {
                            d = STPCPY(d, p+1);
                            *d++ = '\n';
                            *d   = '\0';
                        }
                      #endif
                        mode = MD_TameBody;
                        goto NEXT_LINE;
                    case '-':               /* �I�v�V���������� */
                        q = (char*)StrSkipNotSpc(p);
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
                    case '$':               /* $�ϐ��� */
                        p = setDoll(p+1);
                        break;
                    default:
                        if (rOpts_.linInFlg_) { /* �s�P�ʂŃt�@�C�������擾 */
                            rFileNameList_.push_back(p);
                            goto NEXT_LINE;
                        } else {            /* �󔒋�؂�Ńt�@�C�������擾 */
                            p = (char*)getFileNameStr(name, p);
                            rFileNameList_.push_back(name);
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
            case MD_TameBody: /* = �o�b�t�@���ߖ{�� */
                d = STPCPY(d, p);
                *d++ = '\n';
                *d   = '\0';
                break;
            }
        }
        return true;
        #undef ISSPC
    }


    bool keyStrEqu(char *key, char *lin) {
        char    *k,*f;
        size_t  l;

        rConvFmt_.clearVar();

        k = key;
        f = lin;
        for (; ;) {
          NEXT:
            if (*k == *f) {
                if (*k == '\0')
                    return true;   /* �}�b�`������ */
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
                            err_printfE("%s �̂��錟���s��{..}��10�ȏ゠�� %s\n", resName_.c_str(),lin);
                        }
                        rConvFmt_.setVar(varIdx_, f, l);
                        ++varIdx_;
                        k += l;
                        f = strchr(f,'}');
                        if (f == NULL) {
                  ERR1:
                            err_printfE("%s ��{..}�̎w�肪�������� %s\n",resName_.c_str(), lin);
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
        return false;          /* �}�b�`���Ȃ����� */
    }


private:
    Opts&               rOpts_;
    ConvFmt&            rConvFmt_;
    StrList&            rFileNameList_;
    StrList&            rBeforeStrList_;
    StrList&            rAfterStrList_;
    char*               fmtBuf_;
    char*               resP_;
    int                 varIdx_;
    int                 varNo_[10];
    FnameBuf            resName_;
    StrzBuf<OBUFSIZ>    resOBuf_;
};



class App {
public:
    App()
        : outFp_(NULL)
        , filenameList_()
        , beforeStrList_()
        , afterStrList_()
        , abxName_()
        , opts_(convFmt_)
        , resCfgFile_(opts_, convFmt_, filenameList_, beforeStrList_, afterStrList_, &fmtBuf_[0])
        , fmtBuf_()
    {
    }

    int main(int argc, char *argv[]) {
        opts_.setExename(FIL_BaseName(argv[0]));    /*�A�v���P�[�V������*/
        if (argc < 2) {
            opts_.usage();
            return 1;
        }

        abxName_ = argv[0];
        FIL_ChgExt(&abxName_[0], "cfg");

        if (scanOpts(argc, argv) == false)
            return 1;
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


private:

    bool scanOpts(int argc, char *argv[]) {
        /* �R�}���h���C���̃I�v�V����/�t�@�C����/�ϊ�������, �擾 */
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

            } else if (*p == '@') {
                resCfgFile_.GetResFile(p+1);

            } else if (*p == '+') {
                ++p;
                if (*p == '\\' || *p == '/' || p[1] == ':') {
                    FIL_FullPath(p, &abxName_[0]);
                } else {
                    char fbuf[FIL_NMSZ];
                    strcpy(fbuf, argv[0]);
                    strcpy(FIL_BaseName(fbuf), p);
                    FIL_FullPath(fbuf, &abxName_[0]);
                }
                FIL_AddExt(&abxName_[0], "cfg");

            } else if (*p == ':') {
                if (p[1] == '#') {
                    err_printfE(":#�Ŏn�܂镶����͎w��ł��܂���i%s�j\n",p);
                }
                if (resCfgFile_.GetCfgFile(&abxName_[0], p) == false)
                    return false;
            } else if (*p == '$') {
                if (p[1] >= '1' && p[1] <= '9' && p[2] == '=') {
                    unsigned    no  = p[1] - '0';
                    char const* s   = p + 3;
                    convFmt_.setVar(no, s, strlen(s));
                }
            } else {
                filenameList_.push_back(p);
            }
        }

	   #ifdef ENABLE_MT_X
		if (opts_.nthread_ && (!beforeStrList_.empty() || !afterStrList_.empty())) {
			err_printfE("-xm �w��� #begin,#end �w��͓����Ɏw��ł��܂���\n");
		}
	   #endif

        /* �o�b�`���s�̂Ƃ� */
        if (opts_.batFlg_) {
		 #if 1
			TmpFile_make2(&tmpFName_[0], FIL_NMSZ, "abx_", ".bat");
printf("tmpfname=%s\n", &tmpFName_[0]);
			opts_.outname_  = tmpFName_;
		 #else
            opts_.outname_  = convFmt_.tmpDir();
            opts_.outname_ += "\\_abx_tmp.bat";
         #endif
        }

        if (opts_.fattr_ == 0) {     /* �f�t�H���g�̃t�@�C���������� */
            opts_.fattr_ = FA_Norm|FA_RdOnly|FA_Hidden|FA_Sys|FA_Volume|FA_Arcive;  //0x127
        }
        convFmt_.setUpLwrFlag(opts_.upLwrFlg_);

        /* �ϊ������񒲐� */
        if (fmtBuf_.empty()) {
            if (opts_.recFlg_)
                fmtBuf_ = "$F\n";
            else
                fmtBuf_ = "$c\n";
        }
        if (strchr(fmtBuf_.c_str(), '\n') == NULL)
            fmtBuf_ += "\n";
        convFmt_.setFmtStr(fmtBuf_.c_str());

        convFmt_.setAutoWq(opts_.autoWqFlg_);       /* $f���Ŏ����ŗ��[��"��t�����郂�[�h. */

        return true;
    }


    bool genText() {
        if (opts_.batEx_) {                 /* �o�b�`���s�p�ɐ擪�� echo off ��u�� */
            convFmt_.writeLine0("@echo off");
        }

        /* ���O�o�̓e�L�X�g */
        for (StrList::iterator ite = beforeStrList_.begin(); ite != beforeStrList_.end(); ++ite) {
            convFmt_.writeLine0(ite->c_str());
        }

        /* -u && -s �Ȃ�΁A�w��t�@�C�������������� */
        if (opts_.upLwrFlg_ && opts_.sortType_) {
            for (StrList::iterator ite = filenameList_.begin(); ite != filenameList_.end(); ++ite) {
                StrLwrN(&(*ite)[0], ite->size());
            }
        }

        /* ���s */
        FSrh    fsrh_(  opts_.fattr_, opts_.recFlg_, opts_.zenFlg_, opts_.topN_,
                        opts_.sortType_, opts_.sortRevFlg_, opts_.upLwrFlg_, opts_.knjChk_,
                        opts_.szmin_, opts_.szmax_, opts_.dtmin_, opts_.dtmax_, outFp_, &convFmt_, &ConvFmt::write
                    );
        if (opts_.renbanEnd_ == 0) {
            for (StrList::iterator ite = filenameList_.begin(); ite != filenameList_.end(); ++ite) {
                char const* p = ite->c_str();
                convFmt_.setLineBuf(p);
                if (*p != '\\' && *p != '/' && p[1] != ':') {   /* ���΃p�X�̂Ƃ� */
                    *opts_.iname_ = '\0';
                    opts_.ipath_ += p;
                    p = opts_.ipath_.c_str();
                } else {                                        /* �t���p�X�̂Ƃ� */
                }
                abxName_ = p;
                char const* s = STREND(p);
                if (*s == '/' || *s == '\\')
                    abxName_ += "*";
                FIL_AddExt(&abxName_[0], opts_.dfltExtp_);      /* �f�t�H���g�g���q�t�� */
                /* ���ۂ̃t�@�C�������Ƃ̐��� */
                fsrh_.findAndDo(abxName_.c_str(), opts_.noFindFile_ != 0);
           }
        } else {
            opts_.noFindFile_ = 1;
            /* �A�Ԑ����ł̏����l�ݒ� */
            for (size_t num = opts_.renbanStart_; num <= opts_.renbanEnd_; ++num) {
                convFmt_.setNum(num);
                sprintf(&abxName_[0], "%u", unsigned(num));
                convFmt_.setLineBuf(&abxName_[0]);
                /* ���ۂ̃t�@�C�������Ƃ̐��� */
                fsrh_.findAndDo(abxName_.c_str(), opts_.noFindFile_ != 0);
            }
        }

        /* ����o�̓e�L�X�g */
        for (StrList::iterator ite = afterStrList_.begin(); ite != afterStrList_.end(); ++ite) {
            convFmt_.writeLine0(ite->c_str());
        }

        if (opts_.batEx_)  /* �o�b�`���s�p�Ƀt�@�C������:END��t������ */
            convFmt_.writeLine0(":END");

        return true;
    }

    bool outputText() {
        /* �o�̓t�@�C���ݒ� */
        if (!opts_.outname_.empty()) {
            outFp_ = fopenE(opts_.outname_.c_str(), "wt");
        } else {
            outFp_ = stdout;
        }

		StrList const& outBuf = convFmt_.outBuf();
        for (StrList::const_iterator ite = outBuf.begin(); ite != outBuf.end(); ++ite) {
			fprintf(outFp_, ite->c_str());
        }

        if (!opts_.outname_.empty()) {
            fclose(outFp_);
            outFp_ = NULL;
        }
        return true;
    }

    bool execBat() {
        /* �o�b�`���s�̂Ƃ� */
        if (opts_.batFlg_) {
		 #ifdef ENABLE_MT_X
			if (opts_.nthread_) {
				StrList& 					buf = convFmt_.outBuf();
				std::vector<std::string>	cmds(buf.size());
				std::copy(buf.begin(), buf.end(), cmds.begin());
				convFmt_.clearOutBuf();
				mtCmd(cmds, opts_.nthread_-1);
			} else
		 #endif
			{
			 #if 1
				system(opts_.outname_.c_str());
	         #else
	            char* p = getenv("COMSPEC");
	            spawnl( _P_WAIT, p, p, "/c", opts_.outname_.c_str(), NULL);
			 #endif
			}
        }
        return true;
    }


private:
    FILE*           outFp_;
    StrList         filenameList_;
    StrList         beforeStrList_;
    StrList         afterStrList_;
    FnameBuf        abxName_;           /* ���O work */
    ConvFmt         convFmt_;
    Opts            opts_;
    ResCfgFile      resCfgFile_;
    StrzBuf<FMTSIZ> fmtBuf_;            /* �ϊ�����������߂� */
	StrzBuf<FIL_NMSZ> tmpFName_;
};


/** start application
 */
int main(int argc, char *argv[]) {
    static App app;
    return app.main(argc, argv);
}
