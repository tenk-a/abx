/**
 *  @file   abx.c
 *  @brief  �t�@�C�����������A�Y���t�@�C�����𕶎���ɖ���(�o�b�`����)
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1995-2017
 *  @note
 *      license
 *          �����BSD���C�Z���X
 *          see license.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#ifdef __BORLANDC__
#include <dir.h>
#endif
#include <process.h>
#include "subr.h"
#include "tree.h"

#ifdef ENABLE_MT_X
void mtCmd(const char* batPath, int threads);
#endif

#ifdef C16
#define CC_OBUFSIZ      0x4000U     /* ��`�t�@�C�����̃T�C�Y               */
#define CC_FMTSIZ       0x4000U     /* ��`�t�@�C�����̃T�C�Y               */
#else
#define CC_OBUFSIZ      0x80000     /* ��`�t�@�C�����̃T�C�Y               */
#define CC_FMTSIZ       0x80000     /* ��`�t�@�C�����̃T�C�Y               */
#include <time.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif


/*---------------------------------------------------------------------------*/
static char exename[FIL_NMSZ];

volatile void Usage(void)
{
    printf(
      #ifdef C16
        "ABX(msdos) v3.14 ̧�ٖ�������,�Y��̧�ٖ��𕶎���ɖ���(�ޯ�����).  by tenk*\n"
      #else
        "ABX v3.14 ̧�ٖ�������,�Y��̧�ٖ��𕶎���ɖ���(�ޯ�����).  by tenk*\n"
      #endif
	#ifdef ENABLE_MT_X
		"�@�}���`�X���b�h�o�b�`���s�@�\�t�� by k.misakichi\n"
	#endif
		"usage : %s [��߼��] ['�ϊ�������'] ̧�ٖ� [=�ϊ�������]\n" ,exename);
    printf("%s",
        "��߼��:                        ""�ϊ�����:            �ϊ���:\n"
	#ifdef ENABLE_MT_X
		" -x[-/m]�ޯ����s -x-���Ȃ�m��q"" $f ���߽(�g���q�t)   d:\\dir\\dir2\\filename.ext\n"
	#else
		" -x[-]    �ޯ����s   -x-���Ȃ� "" $f ���߽(�g���q�t)   d:\\dir\\dir2\\filename.ext\n"
	#endif
        " -r[-]    �ިڸ�؍ċA          "" $g ���߽(�g���q��)   d:\\dir\\dir2\\filename\n"
        " -n[-]    ̧�ٌ������Ȃ� -n-�L "" $v ��ײ��            d\n"
        " -a[nrhsd] �w��̧�ّ����Ō���  "" $p �ިڸ��(��ײ�ޕt) d:\\dir\\dir2\n"
        "          n:��� s:���� h:�B�� "" $d �ިڸ��(��ײ�ޖ�) \\dir\\dir2\n"
        "          r:ذ�޵�ذ d:�ިڸ�� "" $c ̧��(�g���q�t)    filename.ext\n"
        " -z[N-M]  ����N�`M��FILE������ "" $x ̧��(�g���q��)    filename\n"
        " -d[A-B]  ���tA�`B��FILE������ "" $e �g���q            ext\n"
        " -s[neztam][r] ���(����)       "" $w �����إ�ިڸ��    (���ϐ�TMP�̓��e)\n"
        "          n:�� e:�g���q z:���� "" $z ����(10�i10��)    1234567890 ��$Z�Ȃ�16�i8��\n"
        "          t:���t a:���� r:�~�� "" $j ����              1993-02-14\n"
        "          m:��(��)             "" $i �A�Ԑ���          ��$I�Ȃ�16�i��\n"
        " -u[-]    $c|$C��̧�ٖ��召����"" $$ $  $[ <  $` '  $n ���s  $t ���\n"
        " -l[-]    @���͂Ŗ��O�͍s�P��  "" $# #  $] >  $^ \"  $s ��  $l �����͂̂܂�\n"
        " -ci[N:M] N:$i�̊J�n�ԍ�(M:�I) ""------------------------------------------------\n"
        " -ct<FILE> FILE���V�����Ȃ�  ""-p<DIR>  $p�̋����ύX   ""-ck[-] ���{�ꖼ�̂݌���\n"
        " +CFGFILE .CFĢ�َw��         ""-e<EXT>  ��̫�Ċg���q   ""-cy[-] \\���܂ޑS�p������\n"
        " @RESFILE ڽ��ݽ̧��           ""-o<FILE> �o��̧�َw��   ""-y     $cxfgdpw��\"�t��\n"
        " :�ϊ���  CFG�Œ�`�����ϊ�    ""-i<DIR>  �����ިڸ��    ""-t[N]  �ŏ���N�̂ݏ���\n"
        " :        �ϊ����ꗗ��\��     ""-w<DIR>  TMP�ިڸ��     ""\n"

        /*" -a[nrhsda]�̎w��̂Ȃ���, -anrhsa���w�肳���\n"*/
        /*" -j  �S�p�Ή�(��̫��)          "*/
        /*" -j- �S�p���Ή�                "*/
        /*" -aa   �����ޑ���������        "*/
        /*" -av �{�����[������ϯ�         "*/
        /*" -b       echo off ��t��      "*/
        /* #begin,#end ڽ��ݽ̧�ْ�,�ϊ��O���÷�ďo��""\n"*/

	#ifdef ENABLE_MT_X
		"\n"
		"-xm[threads] �ɂ���\n"
		"threads\n"
		"  �ȗ���/0 �\�Ȍ���_���R�A���ōs��"
		"  n        �X���b�h����n�Ƃ���\n"
	#endif

        );

    exit(0);
}



/*---------------------------------------------------------------------------*/

static int  FSrh_recFlg         = 1;        // 1:�ċA���� 0:���Ȃ�
static int  FSrh_atr            = 0x3f;     // ����̧�ّ���
static int  FSrh_nomalFlg       = 1;        // ɰ�٥̧�ق�ϯ� 1:���� 0:���Ȃ�
static int  FSrh_knjChk         = 0;
static long FSrh_topN           = 0;
static long FSrh_topCnt         = 0;
static int  FSrh_topFlg         = 0;
static int  FSrh_nonFF          = 0;        // 1:�t�@�C���������Ȃ� 0:����

static char FSrh_fpath[FIL_NMSZ * 4];
static char FSrh_fname[FIL_NMSZ+2];

static unsigned long    FSrh_szMin;
static unsigned long    FSrh_szMax;
static unsigned short   FSrh_dateMin;
static unsigned short   FSrh_dateMax;

static int (*FSrh_func)(char *path, FIL_FIND *ff);



static int FSrh_ChkKnjs(const char *p)
{
    unsigned char c;
    while((c = *(unsigned char *)p++) != 0) {
        if (c & 0x80)
            return 1;
    }
    return 0;
}


#if 1  /* -s �\�[�g�֌W */

static int  FSrh_sortFlg    = 0;
static int  FSrh_sortRevFlg = 0;
static int  FSrh_uplwFlg    = 0;


static void *FSrh_New(void/*FIL_FIND*/ *ff)
{
    void/*FIL_FIND*/ *p;
    p = (void*)malloc(sizeof (FIL_FIND));
    if (p == NULL) {
        printfE("������������܂���\n");
    }
    memcpy(p, ff, sizeof(FIL_FIND));
    return p;
}


static void FSrh_Del(void/*FIL_FIND*/ *ff)
{
    free (ff);
}


static int  FSrh_NamCmp(FIL_FIND *f1, FIL_FIND *f2)
{
    return strcmp(f1->name, f2->name);
}


#ifdef _WIN32
#define FNAME_GET_C(c, p) do {                              \
        (c) = *(unsigned char*)((p)++);                     \
        if (IsDBCSLeadByte(c) && *(p))                      \
            (c) = ((c) << 8) | *(unsigned char*)((p)++);    \
    } while (0)
#else
#define FNAME_GET_C(c, p)   ((c) = *((p)++))
#endif

/** �t�@�C�����̑召��r. ���l���������ꍇ�A�����Ⴂ�̐��l���m�̑召�𔽉f
*   �召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
*   �ȊO�͒P���ɕ������r.
*/
static int fnameNDigitCmp(const char* l, const char* r, size_t len)
{
    const char* e = l + len;
    if (e < l)
        e = (const char*)-1;
    while (l < e) {
     #if defined(C16) 
        typedef unsigned long num_t;
        typedef long          dif_t;
     #elif defined(_MSC_VER) || defined(__BORLANDC__)
        typedef unsigned __int64 num_t;
        typedef __int64          dif_t;
     #endif
        dif_t       n;
        unsigned    lc;
        unsigned    rc;

        FNAME_GET_C(lc, l);
        FNAME_GET_C(rc, r);

        if (lc <= 0x80 && isdigit(lc) && rc <= 0x80 && isdigit(rc)) {
         #if defined(C16) 
            num_t   lv = strtoul(l - 1, (char**)&l, 10);
            num_t   rv = strtoul(r - 1, (char**)&r, 10);
         #elif defined(_MSC_VER)
            num_t   lv = _strtoui64(l - 1, (char**)&l, 10);
            num_t   rv = _strtoui64(r - 1, (char**)&r, 10);
         #else
            num_t   lv = strtoull(l - 1, (char**)&l, 10);
            num_t   rv = strtoull(r - 1, (char**)&r, 10);
         #endif
            n = (dif_t)(lv - rv);
            if (n == 0)
                continue;
            return (n < 0) ? -1 : 1;
        }

        if (lc < 0x80)
            lc = tolower(lc);
        if (rc < 0x80)
            rc = tolower(rc);

        n  = (dif_t)(lc - rc);
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


static int  FSrh_Cmp(FIL_FIND *f1, FIL_FIND *f2)
{
    int n = 0;

    if (FSrh_sortFlg & 0x20) {                      /* ���������͐��l�Ŕ�r���閼�O�\�[�g */
        n = fnameNDigitCmp(f1->name, f2->name, (size_t)-1);
        if (FSrh_sortRevFlg)
            return -n;
        return n;
    }
    if (FSrh_sortFlg <= 1) {                        /* ���O�Ń\�[�g */
        n = strcmp(f1->name, f2->name);
        if (FSrh_sortRevFlg)
            return -n;
        return n;
    }

    if (FSrh_sortFlg == 0x02) {                     /* �g���q */
        char *p,*q;
        p = strrchr(f1->name, '.');
        p = (p == NULL) ? "" : p;
        q = strrchr(f2->name, '.');
        q = (q == NULL) ? "" : q;
        n = strcmp(p,q);

    } else if (FSrh_sortFlg == 0x04) {              /* �T�C�Y */
        long t;
        t = f1->size - f2->size;
        n = (t > 0) ? 1 : (t < 0) ? -1 : 0;

    } else if (FSrh_sortFlg == 0x08) {              /* ���� */
      #if defined C16 || defined __BORLANDC__
        long t;
        t = (long)f1->wr_date - (long)f2->wr_date;
        n = (t > 0) ? 1 : (t < 0) ? -1 : 0;
        if (n == 0) {
            t = (long)f1->wr_time - (long)f2->wr_time;
            n = (t > 0) ? 1 : (t < 0) ? -1 : 0;
        }
      #else
        __int64 t;
        t = f1->time_write - f2->time_write;
        n = (t > 0) ? 1 : (t < 0) ? -1 : 0;
      #endif
    } else if (FSrh_sortFlg == 0x10) {              /* ���� */
        /* �A�[�J�C�u�����͎ז��Ȃ̂ŃI�t���� */
        n = ((int)f2->attrib & 0xDF) - ((int)f1->attrib & 0xDF);
    }

    if (n == 0) {
        n = strcmp(f1->name, f2->name);
        if (FSrh_sortRevFlg)
            n = -n;
    }
    if (FSrh_sortRevFlg)
        return -n;
    return n;
}


static void FSrh_DoOne(void *ff)
{
    char *t;

    if (FSrh_topFlg) {
        if (FSrh_topCnt == 0)       /* �擪 N�݂̂̏����̂Ƃ� */
            return;
        else
            --FSrh_topCnt;
    }
    t = STREND(FSrh_fpath);
    strcpy(t, ((FIL_FIND*)ff)->name);
    FSrh_func(FSrh_fpath, ff);
    *t = 0;
}


static int FSrh_FindAndDo_SubSort(void);

static void FSrh_DoOneDir(void *ff)
{
    char *t;

    t = STREND(FSrh_fpath);
    strcpy(t, ((FIL_FIND*)ff)->name);
    strcat(t, "\\");
    FSrh_FindAndDo_SubSort();
    *t = 0;
}


static int FSrh_FindAndDo_SubSort(void)
{
    FIL_FIND_HANDLE hdl;
    FIL_FIND        ff;
    char*           t;
    TREE*           tree;

    if (FSrh_topFlg) {
        FSrh_topCnt = FSrh_topN;
    }
    tree = TREE_Make(FSrh_New, FSrh_Del, (TREE_CMP)FSrh_Cmp, mallocE, freeE);
    t    = STREND(FSrh_fpath);
    strcpy(t,FSrh_fname);
    hdl = FIL_FINDFIRST(FSrh_fpath, FSrh_atr, &ff);
    if (FIL_FIND_HANDLE_OK(hdl)) {
        do {
            *t = '\0';
            if (FSrh_nomalFlg == 0 && (FSrh_atr & ff.attrib) == 0)
                continue;
            if ((FSrh_atr & 0x10) == 0 && (ff.attrib & 0x10))   /* �f�B���N�g�������łȂ��̂Ƀf�B���N�g�������������΂� */
                continue;
            if(  (ff.name[0] != '.')
              && (  (FSrh_szMin > FSrh_szMax) || ((int)FSrh_szMin <= ff.size && ff.size <= (int)FSrh_szMax) )
              && (  (FSrh_dateMin > FSrh_dateMax) || (FSrh_dateMin <= ff.wr_date && ff.wr_date <= FSrh_dateMax) )
              && (  (FSrh_knjChk==0) || (FSrh_knjChk==1 && FSrh_ChkKnjs(ff.name)) || (FSrh_knjChk==2 && strchr(ff.name,'\\'))
                                     || (FSrh_knjChk==-1&& !FSrh_ChkKnjs(ff.name))|| (FSrh_knjChk==-2&& !strchr(ff.name,'\\'))  )
              )
            {
                TREE_Insert(tree, &ff);
            }
        } while (FIL_FINDNEXT(hdl, &ff) == 0);
        FIL_FINDCLOSE(hdl);
    }
    TREE_DoAll(tree, FSrh_DoOne);
    TREE_Clear(tree);

    if (FSrh_recFlg /*&& FSrh_nonFF == 0*/) {
        tree = TREE_Make(FSrh_New, FSrh_Del, (TREE_CMP)FSrh_NamCmp, mallocE, freeE);
        strcpy(t,"*.*");
        hdl = FIL_FINDFIRST(FSrh_fpath, 0x10, &ff);
        if (FIL_FIND_HANDLE_OK(hdl)) {
            do {
                *t = '\0';
                if ((ff.attrib & 0x10) && strcmp(ff.name, ".") && strcmp(ff.name, "..")) {
                    TREE_Insert(tree, &ff);
                }
            } while (FIL_FINDNEXT(hdl, &ff) == 0);
            FIL_FINDCLOSE(hdl);
        }
        TREE_DoAll(tree, FSrh_DoOneDir);
        TREE_Clear(tree);
    }
    return 0;
}

#endif


static int FSrh_FindAndDo_Sub(void)
{
    FIL_FIND_HANDLE hdl;
    FIL_FIND        ff = {0};
    char *t;

    if (FSrh_topFlg) {
        FSrh_topCnt = FSrh_topN;
    }
    t = STREND(FSrh_fpath);
    strcpy(t,FSrh_fname);
    hdl = FIL_FINDFIRST(FSrh_fpath, FSrh_atr, &ff);
    if (FIL_FIND_HANDLE_OK(hdl)) {
        do {
            *t = '\0';
            if (FSrh_nomalFlg == 0 && (FSrh_atr & ff.attrib) == 0)
                continue;
            if ((FSrh_atr & 0x10) == 0 && (ff.attrib & 0x10))   /* �f�B���N�g�������łȂ��̂Ƀf�B���N�g�������������΂� */
                continue;
            if(  (ff.name[0] != '.')
              && (  (FSrh_szMin > FSrh_szMax) || ((int)FSrh_szMin <= ff.size && ff.size <= (int)FSrh_szMax) )
              && (  (FSrh_dateMin > FSrh_dateMax) || (FSrh_dateMin <= ff.wr_date && ff.wr_date <= FSrh_dateMax) )
              && (  (FSrh_knjChk==0) || (FSrh_knjChk==1 && FSrh_ChkKnjs(ff.name)) || (FSrh_knjChk==2 && strchr(ff.name,'\\'))
                                     || (FSrh_knjChk==-1&& !FSrh_ChkKnjs(ff.name))|| (FSrh_knjChk==-2&& !strchr(ff.name,'\\'))  )
              )
            {
                strcpy(t, ff.name);
                FSrh_func(FSrh_fpath, &ff);
                *t = 0;
                if (FSrh_topFlg && --FSrh_topCnt == 0) {    /* �擪 N�݂̂̏����̂Ƃ� */
                    return 0;
                }
            }
        } while (FIL_FINDNEXT(hdl, &ff) == 0);
        FIL_FINDCLOSE(hdl);
    }

    if (FSrh_recFlg) {
        strcpy(t,"*.*");
        hdl = FIL_FINDFIRST(FSrh_fpath, 0x10, &ff);
        if (FIL_FIND_HANDLE_OK(hdl)) {
            do {
                *t = '\0';
                if ((ff.attrib & 0x10) && ff.name[0] != '.') {
                    strcpy(t, ff.name);
                    strcat(t, "\\");
                    FSrh_FindAndDo_Sub();
                }
            } while (FIL_FINDNEXT(hdl, &ff) == 0);
            FIL_FINDCLOSE(hdl);
        }
    }
    return 0;
}



int FSrh_FindAndDo(char *path, int atr, int recFlg, int zenFlg,
                long topn, int sortFlg, int knjChk, int nonFF,
                unsigned long szmin, unsigned long szmax,
                unsigned short dtmin, unsigned short dtmax,
                int (*fun)(char *apath, FIL_FIND *aff))
{
    char *p;

    FSrh_func    = fun;
    FSrh_recFlg  = recFlg;
    FSrh_atr     = atr;
    FSrh_topN    = topn;
    FSrh_topFlg  = (topn != 0);
    FSrh_knjChk  = knjChk;
    FSrh_szMin   = szmin;
    FSrh_szMax   = szmax;
    FSrh_dateMin = dtmin;
    FSrh_dateMax = dtmax;
    FSrh_nonFF   = nonFF;
    FIL_SetZenMode(zenFlg);
    /*printf("%lu(%lx)-%lu(%lx)\n",szmin,szmin,szmax,szmax);*/
    /*printf("date %04x-%04x\n",dtmin,dtmax);*/
    FSrh_nomalFlg = 0;
    if (atr & 0x100) {
        atr &= 0xff;
        FSrh_atr = atr;
        FSrh_nomalFlg = 1;
    }
    FIL_FullPath(path, FSrh_fpath);
    p = STREND(FSrh_fpath);
    if (p[-1] == ':' || p[-1] == '\\' || p[-1] == '/')
        strcat(FSrh_fpath, "*");
    p = FIL_BaseName(FSrh_fpath);
    strncpyZ(FSrh_fname, p, FIL_NMSZ);
    if (FSrh_nonFF) {   /* �t�@�C���������Ȃ��ꍇ */
        FIL_FIND ff;
        memset(&ff, 0, sizeof ff);
        if ( (FSrh_knjChk==0) || (FSrh_knjChk==1 && FSrh_ChkKnjs(FSrh_fname)) || (FSrh_knjChk==2 && strchr(FSrh_fname,'\\'))
                || (FSrh_knjChk==-1&& !FSrh_ChkKnjs(FSrh_fname))|| (FSrh_knjChk==-2&& !strchr(FSrh_fname,'\\')) )
        {
            FSrh_func(FSrh_fpath, &ff);
        }
        return 0;
    }
    /* �t�@�C����������ꍇ */
    *p = 0;
    if (sortFlg) {  /* �\�[�g���� */
        FSrh_sortRevFlg = (sortFlg & 0x80);
        FSrh_sortFlg = sortFlg & 0x7f;
        FSrh_uplwFlg = sortFlg & 0x8000;
        return FSrh_FindAndDo_SubSort();
    }
    return FSrh_FindAndDo_Sub();
}



/*---------------------------------------------------------------------------*/
static char CC_drv[FIL_NMSZ];
static char CC_dir[FIL_NMSZ];
static char CC_name[FIL_NMSZ];
static char CC_ext[FIL_NMSZ];
FILE *      CC_fp;
/*int       CC_lwrFlg = 0;*/
#ifdef C16
int         CC_upLwrFlg = 1;
long        CC_num;                         /* $i �Ő�������ԍ� */
long        CC_numEnd;                      /* �A�Ԃ��t�@�C�����̕�����̑���ɂ���w��������ꍇ�̏I���A�h���X */
#else
int         CC_upLwrFlg = 0;
int         CC_num;                         /* $i �Ő�������ԍ� */
int         CC_numEnd;                      /* �A�Ԃ��t�@�C�����̕�����̑���ɂ���w��������ꍇ�̏I���A�h���X */
#endif
char        CC_tmpDir[FIL_NMSZ];
int         CC_vn = 0;
char        CC_v[10][FIL_NMSZ];
int         CC_no[10];
char        CC_pathDir[FIL_NMSZ];
char        CC_chgPathDir[FIL_NMSZ];
char        CC_fmtBuf[CC_FMTSIZ];           /* �ϊ�����������߂� */
char        CC_obuf[CC_OBUFSIZ+FIL_NMSZ];   /* .cfg(.res) �ǂݍ��݂�A�o�͗p�̃o�b�t�@ */
char        CC_tgtnm[FIL_NMSZ+FIL_NMSZ];
char        CC_tgtnmFmt[FIL_NMSZ+FIL_NMSZ];
char *      CC_lineBuf;
int         CC_auto_wq = 0;                 /* $f���Ŏ����ŗ��[��"��t�����郂�[�h. */



char *CC_StpCpy(char *d, char *s, ptrdiff_t clm, int flg)
{
    unsigned char c;
    size_t        n;

    n = 0;
    if (CC_upLwrFlg == 0) {
        strcpy(d,s);
        n = strlen(s);
        d = d + n;
    } else if (flg == 0) {  /* �啶���� */
        while ((c = *(unsigned char *)s++) != '\0') {
            if (islower(c))
                c = toupper(c);
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
                c = tolower(c);
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



static void CC_SplitPath(char *fpath)
{
    size_t  l;
    char*   p;

    FIL_SplitPath(fpath, CC_drv, CC_dir, CC_name, CC_ext);

  #if 0
 // if (CC_lwrFlg) {
 //     strlwr(CC_drv);
 //     strlwr(CC_dir);
 //     strlwr(CC_name);
 //     strlwr(CC_ext);
 // }
  #endif

  #if 1
    /* �f�B���N�g�����̌���'\'���͂��� */
    l = strlen(CC_dir);
    if (l) {
        p = CC_dir + l - 1;
        if (*p == '\\' || *p == '/') {
            *p = 0;
        }
    }
  #else
    FIL_DelLastDirSep(CC_dir);  /* �f�B���N�g�����̌���'\'���͂��� */
  #endif
    strcpy(CC_pathDir,CC_drv);
    strcat(CC_pathDir,CC_dir);
    if (CC_chgPathDir[0]) {
        strcpy(CC_pathDir, CC_chgPathDir);
    }
    /* �g���q�� '.' ���͂��� */
    if (CC_ext[0] == '.') {
        memmove(CC_ext, CC_ext+1, strlen(CC_ext+1)+1);
    }
}



static void CC_StrFmt(char *dst, const char *src, int sz, FIL_FIND *ff);

int CC_Write(char *fpath, FIL_FIND *ff)
{
    CC_SplitPath(fpath);

    CC_StrFmt(CC_tgtnm, CC_tgtnmFmt, FIL_NMSZ, ff);                 // ����̃^�[�Q�b�g����ݒ�
    if (CC_tgtnmFmt[0] == 0 || FIL_FdateCmp(CC_tgtnm, fpath) < 0) { // ���t��r���Ȃ����A����ꍇ�̓^�[�Q�b�g���Â����
        CC_StrFmt(CC_obuf, CC_fmtBuf, CC_OBUFSIZ, ff);
        fprintf(CC_fp, "%s", CC_obuf);
    }
    CC_num++;
    return 0;
}


static void CC_StrFmt(char *dst, const char *src, int sz, FIL_FIND *ff)
{
    static char buf[FIL_NMSZ*4];
    char c, *b, *p, *pe;
    const char *s;
    int  f,n;
    char drv[2];
    drv[0] = CC_drv[0];
    drv[1] = 0;

    s = src;
    p = dst;
    pe = p + sz;
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

            case 'L':   p = CC_StpCpy(p,CC_lineBuf,n,f);        break;
            case 'V':   p = CC_StpCpy(p,drv,n,f);       break;
            case 'D':
                if (CC_auto_wq) *p++ = '"';
                p = CC_StpCpy(p,CC_dir,n,f);
                if (CC_auto_wq) *p++ = '"';
                *p = 0;
                break;
            case 'X':
                if (CC_auto_wq) *p++ = '"';
                p = CC_StpCpy(p,CC_name,n,f);
                if (CC_auto_wq) *p++ = '"';
                *p = 0;
                break;
            case 'E':
                p = CC_StpCpy(p,CC_ext,n,f);
                break;
            case 'W':
                if (CC_auto_wq) *p++ = '"';
                p = CC_StpCpy(p,CC_tmpDir,n,f);
                if (CC_auto_wq) *p++ = '"';
                *p = 0;
                break;
            case 'P':
                if (CC_auto_wq) *p++ = '"';
                p = CC_StpCpy(p,CC_pathDir,n,f);
                if (CC_auto_wq) *p++ = '"';
                *p = 0;
                break;

            case 'C':
                b = buf;
                if (CC_auto_wq) *b++ = '"';
                b = CC_StpCpy(b,CC_name,0,f);
                if (CC_ext[0]) {
                    b = STPCPY(b,".");
                    b = CC_StpCpy(b,CC_ext,0,f);
                }
                if (CC_auto_wq) *b++ = '"';
                *b = 0;
                if (n < 0) n = 1;
                p += sprintf(p, "%-*s", n, buf);
                break;
            case 'F':
                b = buf;
                if (CC_auto_wq) *b++ = '"';
                b = CC_StpCpy(b,CC_drv,0,f);
                b = CC_StpCpy(b,CC_dir,0,f);
                b = STPCPY(b,"\\");
                b = CC_StpCpy(b,CC_name,0,f);
                if (CC_ext[0]) {
                    b = STPCPY(b,".");
                    b =CC_StpCpy(b,CC_ext,0,f);
                }
                if (CC_auto_wq) *b++ = '"';
                *b = 0;
                if (n < 0) n = 1;
                p += sprintf(p, "%-*s", n, buf);
                break;
            case 'G':
                b = buf;
                if (CC_auto_wq) *b++ = '"';
                b = CC_StpCpy(b,CC_drv,0,f);
                b = CC_StpCpy(b,CC_dir,0,f);
                b = STPCPY(b,"\\");
                b = CC_StpCpy(b,CC_name,0,f);
                if (CC_auto_wq) *b++ = '"';
                *b = '\0';
                if (n < 0) n = 1;
                p += sprintf(p, "%-*s", n, buf);
                break;
            case 'O':
                CC_StpCpy(buf, CC_tgtnm, 0, f);
                if (n < 0) n = 1;
                p += sprintf(p, "%-*s", n, buf);
                break;
            case 'Z':
                if (f) {
                    if (n < 0)
                        n = 10;
                  #ifdef C16
                    p += sprintf(p, "%*ld", n, (ULONG)ff->size);
                  #else
                    p += sprintf(p, "%*d", n, ff->size);
                  #endif
                } else {
                    if (n < 0)
                        n = 8;
                  #ifdef C16
                    p += sprintf(p, "%*LX", n, (ULONG)ff->size);
                  #else
                    p += sprintf(p, "%*X", n, ff->size);
                  #endif
                }
                break;

            case 'I':
                if (f) {
                    if (n < 0)
                        n = 1;
                  #ifdef C16
                    p += sprintf(p, "%0*ld", n, CC_num);
                  #else
                    p += sprintf(p, "%0*d", n, CC_num);
                  #endif
                } else {
                    if (n < 0)
                        n = 1;
                  #ifdef C16
                    p += sprintf(p, "%0*LX", n, CC_num);
                  #else
                    p += sprintf(p, "%0*X", n, CC_num);
                  #endif
                }
                break;

            case 'J':
              #if defined C16 || defined __BORLANDC__
                {   int y,m,d;
                    y = (1980+((unsigned short)ff->wr_date>>9));
                    m = (ff->wr_date>>5) & 0x0f;
                    d = (ff->wr_date   ) & 0x1f;
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
              #else
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
              #endif
                break;
            default:
                if (c >= '1' && c <= '9') {
                    p = STPCPY(p, CC_v[c-'0']);
                } else {
                    fprintf(STDERR, "Incorrect '$' format : '$%c'\n",c);
                    /*fprintfE(STDERR,".cfg �� $�w�肪��������(%c)\n",c);*/
                    // exit(1);
                }
            }
        }
    }
}




int CC_WriteLine0(char *fmtBuf)
{
    char c,*p,*s;

    s = fmtBuf;
    p = CC_obuf;
    while ((c = (*p++ = *s++)) != '\0') {
        if (c == '$') {
            --p;
            c = *s++;
            if (c == '$') {
                *p++ = '$';
            } else if (c >= '1' && c <= '9') {
                p = STPCPY(p, CC_v[c-'0']);
            } else {
                fprintf(STDERR,"Incorrect '$' format : '$%c'\n",c);
                /*fprintfE(STDERR,"���X�|���X���� $�w�肪��������(%c)\n",c);*/
                exit(1);
            }
        }
    }
    fprintf(CC_fp, "%s\n", CC_obuf);
    return 0;
}



/*--------------------------------------------------------------------------*/
static int              Opt_recFlg = 0;                     /* �ċA�̗L�� */
static int              Opt_zenFlg = 1;                     /* MS�S�p�Ή� */
static int              Opt_knjChk = 0;                     /* MS�S�p���݃`�F�b�N */
static int              Opt_atr    = 0;                     /* �t�@�C������ */
static int              Opt_batFlg = 0;                     /* �o�b�`���s */
static int              Opt_batEx  = 0;                     /* -b�̗L�� */
static int              Opt_sort   = 0;                     /* �\�[�g */
static long             Opt_topN   = 0;                     /* ������ */
static int              Opt_nonFF  = 0;                     /* �t�@�C���������Ȃ� */
static int              Opt_linInFlg = 0;                   /* RES���͂��s�P�ʏ���*/
static char             Opt_outname[FIL_NMSZ] = "";         /* �o�̓t�@�C���� */
static char             Opt_ipath[FIL_NMSZ]   = "";         /* ���̓p�X�� */
static char *           Opt_iname = Opt_ipath;              /* ���̓t�@�C���� */
static char             Opt_abxName[FIL_NMSZ] = "";         /* ���O work */
static char             Opt_dfltExt[FIL_NMSZ] = "";         /* �f�t�H���g�g���q */
static char *           Opt_dfltExtp = NULL;                /* �f�t�H���g�g���q */
static unsigned long    Opt_szmin = 0xFFFFFFFFUL;   /* szmin > szmax�̂Ƃ���r���s��Ȃ�*/
static unsigned long    Opt_szmax = 0UL;
static unsigned short   Opt_dtmin = 0xFFFFU;        /* dtmin > dtmax�̂Ƃ���r���s��Ȃ�*/
static unsigned short   Opt_dtmax = 0;
static int              Opt_renbanStart;                    /* �A�Ԃ̊J�n�ԍ�. ����0 */
static int              Opt_renbanEnd;                      /* �A�Ԃ̊J�n�ԍ�. ����0 */
static int              Opt_auto_wq = 0;



void Opts(char *s)
{
    char c,*p;

    p = s;
    p++; c = *p++; c = toupper(c);
    switch (c) {
    case 'X':
        Opt_batFlg = (*p != '-');
		//mt check
	#ifdef ENABLE_MT_X
		if (Opt_batFlg!=0) {
			if (*p == 'm') {
				int threads = strtol(p+1, NULL, 0);
				Opt_batFlg = 0x80000000 | threads;
			}
		}
	#endif
        break;

    case 'R':
        Opt_recFlg = (*p != '-');
        break;

    case 'U':
        CC_upLwrFlg = (*p != '-');
        break;

    case 'N':
        Opt_nonFF = (*p != '-');
        if (*p == 'd' || *p == 'D')
            Opt_nonFF = 2;
        break;

    case 'J':
        Opt_zenFlg = (*p != '-');
        break;

    case 'B':
        Opt_batEx = (*p != '-');
        break;

    case 'L':
        Opt_linInFlg  = (*p != '-');
        break;

  #if 0
 // case 'L':
 //     CC_lwrFlg = (*p != '-');
 //     break;
  #endif

    case 'T':
        if (*p == 0) {
            Opt_topN = 1;
        } else {
            Opt_topN = strtol(p,NULL,0);
        }
        break;

    case 'C':
        c = toupper(*p);
        if (c == '-') {
            Opt_knjChk = 0;
        } else if (c == 'K') {
            Opt_knjChk = 1;
            if (p[1] == '-')
                Opt_knjChk = -1;
        } else if (c == 'Y') {
            Opt_knjChk = 2;
            if (p[1] == '-')
                Opt_knjChk = -2;
        } else if (c == 'T' || c == 'F') {  // 'F'�͋��݊�
            strncpyZ(CC_tgtnmFmt, p+1, FIL_NMSZ);
        } else if (c == 'I') {
            Opt_renbanStart = strtol(p+1, &p, 0);
            if (*p) {
                Opt_renbanEnd = strtol(p+1, &p, 0);
            } else {
                Opt_renbanEnd = 0;
            }
        } else {
            goto ERR_OPTS;
        }
        break;

    case 'Y':
        Opt_auto_wq = (*p != '-');
        break;

    case 'E':
        Opt_dfltExtp = strncpyZ(Opt_dfltExt, p, FIL_NMSZ);
        if (*p == '$' && p[1] >= '1' && p[1] <= '9' && p[2] == 0) {
            strcpy(Opt_dfltExt, CC_v[p[1]-'0']);
        }
        /*Opt_dfltExt[3] = 0;*/
        break;

    case 'O':
        if (*p == 0)
            goto ERR_OPTS;
        strcpy(Opt_outname,p);
        break;

    case 'I':
        if (*p == 0)
            goto ERR_OPTS;
        FIL_FullPath(p, Opt_ipath);
        p = STREND(Opt_ipath);
        if (p[-1] != '\\' && p[-1] != '/') {
            *p++ = '\\';
            *p = '\0';
        }
        Opt_iname = p;
        break;

    case 'P':
        if (*p == 0)
            goto ERR_OPTS;
        FIL_FullPath(p, CC_chgPathDir);
        p = STREND(CC_chgPathDir);
        if (p[-1] == '\\' || p[-1] == '/') {
            p[-1] = '\0';
        }
        break;

    case 'W':
        strcpy(CC_tmpDir,p);
        FIL_GetTmpDir(CC_tmpDir);
        break;

    case 'A':
        strupr(p);
        while (*p) {
            switch(*p) {
            case 'N': Opt_atr |= 0x100; break;
            case 'R': Opt_atr |= 0x001; break;
            case 'H': Opt_atr |= 0x002; break;
            case 'S': Opt_atr |= 0x004; break;
            case 'V': Opt_atr |= 0x008; break;
            case 'D': Opt_atr |= 0x010; break;
            case 'A': Opt_atr |= 0x020; break;
            }
            ++p;
        }
        break;

    case 'S':
        c = 0;
        Opt_sort = 0x01;
        strupr(p);
        while (*p) {
            switch(*p) {
            case '-': Opt_sort = 0x00; break;
            case 'N': Opt_sort = 0x01; break;
            case 'E': Opt_sort = 0x02; break;
            case 'Z': Opt_sort = 0x04; break;
            case 'T': Opt_sort = 0x08; break;
            case 'A': Opt_sort = 0x10; break;
            case 'M': Opt_sort = 0x20; break;
            case 'R': c = 0x80;        break;
            }
            ++p;
        }
        Opt_sort |= c;
        break;

    case 'Z':
        Opt_szmin = (*p == '-') ? 0 : strtoul(p, &p, 0);
        if (*p == 'K' || *p == 'k')         p++, Opt_szmin *= 1024UL;
        else if (*p == 'M' || *p == 'm')    p++, Opt_szmin *= 1024UL*1024UL;
        if (*p) { /* *p == '-' */
            Opt_szmax = 0xffffffffUL;
            p++;
            if (*p) {
                Opt_szmax = strtoul(p,&p,0);
                if (*p == 'K' || *p == 'k')         p++, Opt_szmax *= 1024UL;
                else if (*p == 'M' || *p == 'm')    p++, Opt_szmax *= 1024UL*1024UL;
            }
            if (Opt_szmax < Opt_szmin)
                goto ERR_OPTS;
        } else {
            Opt_szmax = Opt_szmin;
        }
        break;

    case 'D':
        if (*p == 0) {
            Opt_dtmax = Opt_dtmin = 0;
        } else {
            unsigned long t;
            int y,m,d;
            if (*p == '-') {
                Opt_dtmin = 0;
            } else {
                t = strtoul(p,&p,10);
                y = (int)((t/10000) % 100); y = (y >= 80) ? (y-80) : (y+100-80);
                m = (int)((t / 100) % 100); if (m == 0 || 12 < m) goto ERR_OPTS;
                d = (int)(t % 100);         if (d == 0 || 31 < d) goto ERR_OPTS;
                Opt_dtmin = (y<<9)|(m<<5)|d;
            }
            if (*p) {
                p++;
                Opt_dtmax = 0xFFFFU;
                if (*p) {
                    t = strtoul(p,&p,10);
                    y = (int)(t/10000)%100; y = (y>=80) ? (y-80) : (y+100-80);
                    m = (int)(t/100) % 100; if (m==0 || 12 < m) goto ERR_OPTS;
                    d = (int)(t % 100);     if (d==0 || 31 < d) goto ERR_OPTS;
                    Opt_dtmax = (y<<9)|(m<<5)|d;
                    if (Opt_dtmax < Opt_dtmin)
                        goto ERR_OPTS;
                }
            } else {
                Opt_dtmax = Opt_dtmin;
            }
        }
        break;

    case '?':
    case '\0':
        Usage();
        break;

    default:
  ERR_OPTS:
      #if 1
        printfE("�R�}���h���C���ł̃I�v�V�����w�肪�������� : %s\n", s);
      #else
        printfE("Incorrect command line option : %s\n", s);
      #endif
    }
}



/*---------------------------------------------------------------------------*/

SLIST_T *   fileListTop     = NULL;
SLIST_T *   beforeTextList  = NULL;
SLIST_T *   afterTextList   = NULL;
static char Res_nm[FIL_NMSZ];
char *      Res_p           = CC_obuf;



char *Res_GetLine(void)
    /* CC_obuf�ɒ������e�L�X�g���P�s���� */
    /* �s���̉��s�͍폜. CC_obuf�͔j��     */
{
    char *p;

    p = Res_p;
    if (*p == 0)
        return NULL;
    Res_p = strchr(p, '\n');
    if (Res_p != NULL) {
        if (Res_p[-1] == '\r')
            Res_p[-1] = 0;
        *Res_p++ = 0;
    } else {
        Res_p = STREND(p);
    }
    return p;
}

char *Res_SetDoll(char *p0)
{
    int m,n,i;
    char *p;

    p = p0;
    m = *p++;   if (m < '1' || m > '9') goto ERR;
    m -= '0';
    if (*p == '=') {
        size_t l;
        p++;
        l = strcspn(p,"\t\n ");
        if (l < 1 || l >= (sizeof CC_v[0])-1)
            goto ERR;
        memcpy(CC_v[m], p, l);
        CC_v[m][l+1] = 0;
        p += l+1;
        goto RET;

  ERR:
        printfE(".cfg �t�@�C���� $�m �w��ł����������̂����� : $%s\n",p0);

    } else if (*p++ == ':') {
        n = *p++;   if (n < '1' || n > '9') goto ERR2;
        n -= '0';
        if (*p++ != '{') goto ERR2;
        i = 0;
        do {
            size_t l;
            l = strcspn(p,"|}");
            if (l < 1 || l >= (sizeof CC_v[0])-1)
                goto ERR;
            if (i == CC_no[n]) {
                memcpy(CC_v[m], p, l);
                CC_v[m][l+1] = 0;
                p = strchr(p,'}'); if (p == NULL) goto ERR2;
                p++;
                goto RET;
            }
            i++;
            p += l + 1;
        } while (p[-1] == '|');
  ERR2:
        printfE(".cfg �t�@�C���� $�m=������w�� �܂��� $�m:�l{..}�w��ł����������̂����� : $%s\n",p0);
    }
  RET:
    return p;
}



char *Res_GetFileNameStr(char *d, char *s)
    /* s ���󔒂ŋ�؂�ꂽ�P��(�t�@�C����)��name �ɃR�s�[����. */
    /* ������ "file name" �̂悤��"�������"���폜���ւ�ɊԂ̋󔒂��c�� */
{
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



void Res_GetFmts(void)
{
    #define ISSPC(c)    ((unsigned char)c <= ' ')
    char    name[FIL_NMSZ];
    char *p,*d,*q;
    int     mode;

    d = CC_fmtBuf;
    mode = 0;
    while ( (p = Res_GetLine()) != NULL ) {
        q = StrSkipSpc(p);
        if (strnicmp(q, "#begin", 6) == 0 && ISSPC(p[6])) {
            mode = 1;
            continue;
        } else if (strnicmp(q, "#body", 5) == 0 && ISSPC(p[5])) {
            mode = 0;
            continue;
        } else if (strnicmp(q, "#end", 4) == 0 && ISSPC(p[4])) {
            mode = 2;
            continue;
        }
        switch (mode) {
        case 0: /* #body */
            while (p && *p) {
                p = StrSkipSpc(p);  /* �󔒃X�L�b�v */
                switch (*p) {
                case '\0':
                case '#':
                    goto NEXT_LINE;
                case '\'':
                    if (p[1] == 0) {
                        printfE("���X�|���X�t�@�C��(��`�t�@�C����)��'�ϊ�������'�w�肪��������\n");
                    }
                    p++;
                    d = strchr(p, '\'');
                    if (d) {
                        *d = '\0';
                        d = STPCPY(CC_fmtBuf, p);
                    }
                    break;
                case '=':
                    /*memcpy(CC_fmtBuf, Res_p, strlen(Res_p)+1);*/
                    d = CC_fmtBuf;
                  #if 0
                    if (p[1]) {
                        d = STPCPY(d, p+1);
                        *d++ = '\n';
                        *d   = '\0';
                    }
                  #endif
                    mode = 3;
                    goto NEXT_LINE;
                case '-':               /* �I�v�V���������� */
                    q = StrSkipNotSpc(p);
                    if (*q) {
                        *q++ = 0;
                    } else {
                        q = NULL;
                    }
                    Opts(p);
                    p = q;
                    break;
                case '$':               /* $�ϐ��� */
                    p = Res_SetDoll(p+1);
                    break;
                default:
                    if (Opt_linInFlg) { /* �s�P�ʂŃt�@�C�������擾 */
                        SLIST_Add(&fileListTop, p);
                        goto NEXT_LINE;
                    } else {            /* �󔒋�؂�Ńt�@�C�������擾 */
                        p = Res_GetFileNameStr(name, p);
                        SLIST_Add(&fileListTop, name);
                    }
                }
            }
          NEXT_LINE:;
            break;
        case 1: /* #begin */
            SLIST_Add(&beforeTextList, p);
            break;
        case 2: /* #end  */
            SLIST_Add(&afterTextList, p);
            break;
        case 3: /* = �o�b�t�@���ߖ{�� */
            d = STPCPY(d, p);
            *d++ = '\n';
            *d   = '\0';
            break;
        }
    }
}



void GetResFile(char *name)
    /* ���X�|���X�t�@�C������ */
{
    FILE    *fp;
    size_t  l;

    if (name[0] == 0) {                     /* �t�@�C�������Ȃ���ΕW������ */
        l = fread(CC_obuf, 1, CC_FMTSIZ, stdin);
    } else {
        strcpy(Res_nm,name);
        FIL_AddExt(Res_nm, "ABX");
        fp = fopenE(Res_nm,"r");
        l = freadE(CC_obuf, 1, CC_FMTSIZ, fp);
        fclose(fp);
    }
    CC_obuf[l] = 0;
    if (l == 0)
        return;

    Res_p = CC_obuf;
    Res_GetFmts();  /* ���ۂ̃t�@�C�����e�̏��� */
}



#if 0
int Res_StrCmp(char *f, char *p)
{
    return strcmp(f,p);
}
#else

int Res_StrCmp(char *key, char *lin)
{
    char    *k,*f;
    size_t  l;

    for (l = 0; l < 10; l++)
        CC_v[l][0] = 0;
    CC_vn = 1;

    k = key;
    f = lin;
    for (; ;) {
      NEXT:
        if (*k == *f) {
            if (*k == '\0')
                return 0;   /* �}�b�`������ */
            k++;
            f++;
            continue;
        }
        if (*f == '{') {
            f++;
            CC_no[CC_vn] = 0;
            do {
                l = strcspn(f,"|}");
                /*printf("l=%d\n",l);*/
                if (l < 1 || l >= (sizeof CC_v[0])-1)
                    goto ERR1;
                if (memcmp(k,f,l) == 0) {
                    if (CC_vn >= 10) {
                        printfE("%s �̂��錟���s��{..}��10�ȏ゠�� %s\n",
                                Res_nm,lin);
                    }
                    memcpy(CC_v[CC_vn], f, l);
                    CC_v[CC_vn++][l+1] = 0;
                    k += l;
                    f = strchr(f,'}');
                    if (f == NULL) {
              ERR1:
                        printfE("%s ��{..}�̎w�肪�������� %s\n",Res_nm, lin);
                    }
                    f++;
                    goto NEXT;
                }
                f += l + 1;
                CC_no[CC_vn]++;
            } while (f[-1] == '|');
            CC_no[CC_vn] = 0;
        }
        break;
    }
    return -1;          /* �}�b�`���Ȃ����� */

}
#endif



void GetCfgFile(char *name, char *key)
    /* ��`�t�@�C������ */
{
    FILE    *fp;
    size_t  l;
    char    *p;

    FIL_FullPath(name, Res_nm);
    fp = fopenE(Res_nm,"r");
    l = freadE(CC_obuf, 1, CC_FMTSIZ, fp);
    fclose(fp);
    CC_obuf[l] = 0;
    if (l == 0)
        return;

    if (key[1] == 0) /* ':'�����̎w��̂Ƃ� */
        printf("':�ϊ���'�ꗗ\n");
    /*l = 1;*/
    /*   */
    strupr(key);
    Res_p = CC_obuf;
    /* ���s+':'+�ϊ�����T�� */
    while ((Res_p = strstr(Res_p, "\n:")) != NULL) {
        Res_p ++;
        p = Res_GetLine();
        if (p)
            p = strtok(p, " \t\r");
        /*printf("cmp %s vs %s\n",key,p);*/
        if (p == NULL || *p == 0)
            continue;
        strupr(p);
        if (key[1]) {
            /* �ϊ�����������΃��X�|���X�Ɠ������������� */
            if (Res_StrCmp(key, p) == 0) {
                if ((p = strstr(Res_p, "\n:")) != NULL) {
                    *p = '\0';
                }
                Res_GetFmts();
                return;
            }
        } else {    /* �����L�[���Ȃ���΁A�ꗗ�\�� */
            printf("\t%s\n",p);
          #if 0 /* ��� .. */
            if (++l == 23) {
                printf("[more]");
                DOS_KbdIn();
                printf("\b\b\b\b\b\b      \b\b\b\b\b\b");
                l = 0;
            }
          #endif
        }
    }
    if (key[1])
        printf("%s �ɂ� %s �͒�`����Ă��Ȃ�\n", Res_nm, key);
    exit(1);
}



int main(int argc, char *argv[])
{
    int i,f;
    char *p;
    SLIST_T *sl;

    strcpy(exename, FIL_BaseName(argv[0]));     /*�A�v���P�[�V������*/
    strlwr(exename);

    if (argc < 2)
        Usage();

    CC_tmpDir[0] = 0;
    /*GetTmpDir(CC_tmpDir);*/

    FIL_ChgExt(strcpy(Opt_abxName,argv[0]), "cfg");
    for (i = 0; i < 10; i++)
        CC_v[i][0] = 0;
    CC_vn = 1;

    CC_fmtBuf[0] = 0;
    /* �R�}���h���C���̃I�v�V����/�t�@�C����/�ϊ�������, �擾 */
    for (f = 0, i = 1; i < argc; i++) {
        p = argv[i];
       LLL1:
        if (f) {
            if (CC_fmtBuf[0])
                strcat(CC_fmtBuf, " ");
            strcat(CC_fmtBuf, p);
            if (f == 1) {
                p = strrchr(CC_fmtBuf, '\'');
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
            Opts(p);

        } else if (*p == '@') {
            GetResFile(p+1);

        } else if (*p == '+') {
            ++p;
            if (*p == '\\' || *p == '/' || p[1] == ':') {
                FIL_FullPath(p, Opt_abxName);
            } else {
                strcpy(CC_obuf, argv[0]);
                strcpy(FIL_BaseName(CC_obuf), p);
                FIL_FullPath(CC_obuf, Opt_abxName);
            }
            FIL_AddExt(Opt_abxName, "cfg");

        } else if (*p == ':') {
            if (p[1] == '#') {
                printfE(":#�Ŏn�܂镶����͎w��ł��܂���i%s�j\n",p);
            }
            GetCfgFile(Opt_abxName, p);
        } else if (*p == '$') {
            if (p[1] >= '1' && p[1] <= '9' && p[2] == '=') {
                strcpy(CC_v[p[1]-'0'], p+3);
            }
        } else {
            SLIST_Add(&fileListTop, p);
        }
    }

    if (CC_tmpDir[0] == 0)  /* �e���|�����f�B���N�g�����擾 */
        FIL_GetTmpDir(CC_tmpDir);

    if (Opt_atr == 0) {     /* �f�t�H���g�̃t�@�C���������� */
        Opt_atr = 0x127;
    }

    /* �ϊ������񒲐� */
    if (CC_fmtBuf[0] == '\0') {
        if (Opt_recFlg)
            strcpy(CC_fmtBuf, "$F\n");
        else
            strcpy(CC_fmtBuf, "$c\n");
    }
    p = strchr(CC_fmtBuf, '\n');
    if (p == NULL)
        strcat(CC_fmtBuf, "\n");

    /* �A�Ԑ����ł̏����l�ݒ� */
    CC_num     = Opt_renbanStart;
    CC_numEnd  = Opt_renbanEnd;

    CC_auto_wq = Opt_auto_wq;       /* $f���Ŏ����ŗ��[��"��t�����郂�[�h. */

    /* �o�b�`���s�̂Ƃ� */
    if (Opt_batFlg) {
        strcpy(Opt_outname, CC_tmpDir);
        strcat(Opt_outname,"\\_abx_tmp.bat");
    }

	int xmt = (Opt_batFlg & 0x80000000) != 0;
	if (xmt) {
		Opt_batEx = 0;
	}

    /* �o�̓t�@�C���ݒ� */
    if (Opt_outname[0]) {
        CC_fp = fopenE(Opt_outname, "w");
    } else {
        CC_fp = stdout;
    }

    if (Opt_batEx)                  /* �o�b�`���s�p�ɐ擪�� echo off ��u�� */
        fprintf(CC_fp, "@echo off\n");
    /* ���O�o�̓e�L�X�g */
    for (sl = beforeTextList; sl != NULL; sl = sl->link)
        CC_WriteLine0(sl->s);

    /* -u && -s �Ȃ�΁A�w��t�@�C������啶���� */
    if (CC_upLwrFlg && Opt_sort) {
        Opt_sort |= 0x8000; /* �t�@�C���������Č��������t�@�C������啶��������w�� */
        for (sl = fileListTop; sl != NULL; sl = sl->link) {
            strupr(sl->s);
        }
    }

    /* ���s */
    if (CC_numEnd == 0) {
        for (sl = fileListTop; sl != NULL; sl = sl->link) {
            p = sl->s;
            CC_lineBuf = p;
            if (*p != '\\' && *p != '/' && p[1] != ':') {   /* ���΃p�X�̂Ƃ� */
                strcpy(Opt_iname, p);
                strcpy(Opt_abxName, Opt_ipath);
            } else {                                        /* �t���p�X�̂Ƃ� */
                char *s;
                strcpy(Opt_abxName, p);
                s = STREND(p);
                if (*s == '/' || *s == '\\')
                    strcat(Opt_abxName, "*");
            }
            FIL_AddExt(Opt_abxName, Opt_dfltExtp);      /* �f�t�H���g�g���q�t�� */
            /* ���ۂ̃t�@�C�������Ƃ̐��� */
            FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg,
                Opt_zenFlg, Opt_topN, Opt_sort, Opt_knjChk, Opt_nonFF,
                Opt_szmin, Opt_szmax, Opt_dtmin, Opt_dtmax, CC_Write);
        }
    } else {
        Opt_nonFF = 1;
        while (CC_num <= CC_numEnd) {
            sprintf(Opt_abxName, "%d", CC_num);
            CC_lineBuf = p = Opt_abxName;
            /* ���ۂ̃t�@�C�������Ƃ̐��� */
            FSrh_FindAndDo(Opt_abxName, Opt_atr, Opt_recFlg,
                Opt_zenFlg, Opt_topN, Opt_sort, Opt_knjChk, Opt_nonFF,
                Opt_szmin, Opt_szmax, Opt_dtmin, Opt_dtmax, CC_Write);
        }
    }

    /* ����o�̓e�L�X�g */
    for (sl = afterTextList; sl != NULL; sl = sl->link)
        CC_WriteLine0(sl->s);
    if (Opt_batEx)  /* �o�b�`���s�p�Ƀt�@�C������:END��t������ */
        fprintf(CC_fp, ":END\n");

    if (Opt_outname[0])
        fclose(CC_fp);

    /* �o�b�`���s�̂Ƃ� */
    if (Opt_batFlg) {
	#ifdef ENABLE_MT_X
		if (xmt) {
			int threads = Opt_batFlg & 0x7fffffff;
			mtCmd(Opt_outname, threads);
		} else 
	#endif
		{
		#ifdef ENABLE_MT_X
			system(Opt_outname);
		#else
			p = getenv("COMSPEC");
			//x /*i=*/ execl(p,p,"/c",Opt_outname,NULL);
			/*i=*/ spawnl(_P_WAIT, p, p, "/c", Opt_outname, NULL);
			/* �� dos(16)����ȍ~�͎��s����Ȃ�... ��win95���ł͂ǂ�������Ȃ� */
		#endif
		}
    }
	
    return 0;
}
