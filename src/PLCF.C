/*
 * plcf
 * 91/09/21  ver.1.00
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#ifdef TURBOC
  #include "dos2.h"
#endif
#include <jctype.h>
#include "tenkafun.h"
#include "plcf.h"

/*---------------------------------------------------------------------------*/
static char *
fnamcpy(char * p, char * s)
{
    while (*s) {
        if (iskanji(*s)) {
            *(p++) = *(s++);
            if (*s)
                *(p++) = *(s++);
        } else if (*s == '/') {
            *(p++) = C_DIRSP;
        } else
            *(p++) = *(s++);
    }
    *p = '\0';
    return p;
}

static void
divifname(char * src, char * dfn, char * dir, char * fn, char * ext)
   /*    src : source
    *    dfn : ﾌｧｲﾙ名(ﾃﾞｨﾚｸﾄﾘ名込み,拡張子なし) を格納するchar配列へのﾎﾟｲﾝﾀ
    *    dir : ﾃﾞｨﾚｸﾄﾘ名 を格納するchar配列へのﾎﾟｲﾝﾀ
    *    fn  : ﾌｧｲﾙ名(ﾃﾞｨﾚｸﾄﾘ名,拡張子なし) を格納するchar配列へのﾎﾟｲﾝﾀ
    *    ext : 拡張子名 を格納するchar配列へのﾎﾟｲﾝﾀ
    */
{
    char *ep;
    char *np;
    char *s;

    for (ep = NULL, np = s = src; *s != '\0'; ++s) {
        if (iskanji(*s)) {
            if (*(++s) == '\0')
                break;
        } else if (*s == ':' || *s == C_DIRSP) {
            np = s + 1;
        } else if (*s == '.') {
            ep = s + 1;
        }
    }
    if (ep == NULL)
        ep = s;
    for (s = src; s < ep - 1;)
        *(dfn++) = *(s++);
    *dfn = '\0';
    for (s = src; s < np - 1;)
        *(dir++) = *(s++);
    *dir = '\0';
    for (s = np; s < ep - 1;)
        *(fn++) = *(s++);
    *fn = '\0';
    for (s = ep; *s != '\0';)
        *(ext++) = *(s++);
    *ext = '\0';
}


static void
one(char * cmd, char * str, FILE * ofp)
{
    #define BU_MAX 3000
    static char buf[BU_MAX + 1000];
    static char dfn[300 + 10];
    static char dir[300 + 10];
    static char  fn[300 + 10];
    static char ext[300 + 10];
    char *p;
    char c;
    int  i, n;

    n = strlen(cmd);
    if (n > 300) {
        eputs("文字列が長すぎます:");
        eputs(cmd);
        eputs("\n");
        exit(1);
    }
    if (strlen(str) > 300) {
        eputs("文字列が長すぎます:");
        eputs(str);
        eputs("\n");
        return;
    }
    divifname(str, dfn, dir, fn, ext);
 /*
    fprintf(stderr,"  *:%s x:%s $d:%s $f:%s $e:%s\n",str,dfn,dir,fn,ext);
  */
    p = buf;
    for (i = 0; i < BU_MAX && --n >= 0;) {
        c = *(cmd++);
        if (c == '\0')
            break;
        else if (c == '*') {
            goto J1;
        } else if (c == '$') {
            c = *(cmd++);
            switch (toupper(c)) {
            case '\0':
                goto LOOPOUT;
            case 'C':
              J1:
                p = stpcpy(p, str);
                i += strlen(str);
                break;
            case 'P':
                p = stpcpy(p, dir);
                i += strlen(dir);
                break;
            case 'X':
                p = stpcpy(p, dfn);
                i += strlen(dfn);
                break;
            case 'F':
                p = stpcpy(p, fn);
                i += strlen(fn);
                break;
            case 'E':
                p = stpcpy(p, ext);
                i += strlen(ext);
                break;
            case 'N':
                *(p++) = '\n';
                ++i;
                break;
            case 'T':
                *(p++) = '\t';
                break;
            case 'S':
                *(p++) = ' ';
                break;
            default:
                *(p++) = c;
                ++i;
            }
        } else {
            *(p++) = c;
            ++i;
        }
    }
 LOOPOUT:
    *(p++) = '\n';
    *p = '\0';

    if (i >= BU_MAX) {
        eputs("\n変換後の行が長すぎ、途中で切りました:");
        eputs(buf);
    }
    fputs_e(buf, ofp);
}



/*-------------------------------- main -------------------------------------*/

static void
usage()
{
    eputs("use: plcf [-opts] strings...\n");
    eputs("  ｵﾌﾟｼｮﾝ以外の文字列を、その文字列中に'*'等があればその変換方法にしたがって,\n");
    eputs("  入力行と交換して出力します\n");
    eputs(" 変換文字:\n");
    eputs("   *,$c  入力行そのものと交換\n");
    eputs("   $x    入力行をﾌｧｲﾙ名とし,その各調子を除いたものと交換\n");
    eputs("   $p    入力行をﾌｧｲﾙ名とし,そのﾃﾞｨﾚｸﾄﾘ名と交換\n");
    eputs("   $f    入力行をﾌｧｲﾙ名とし,そのﾃﾞｨﾚｸﾄﾘ,各調子を除いたﾌｧｲﾙ名と交換\n");
    eputs("   $e    入力行をﾌｧｲﾙ名とし,その各調子名と交換\n");
    eputs("   $s,$t,$n  それぞれ, 空白(' '),TAB('\\t'),改行('\\n')に置換\n");
    eputs("   $[文字] [文字]がx,p,f,e,n,t,sでなければ$を省き[文字]はそのまま\n");
    eputs(" opts:\n");
    eputs("   -?    ﾍﾙﾌﾟ\n");
    eputs("   --    ｵﾌﾟｼｮﾝの終わりを指定. 文字列として'-'で始まるものを指定可能にする\n");
    eputs("   -o=<path>  出力を<path>にする\n");
    eputs("   -s=<path>  入力を<path>にする\n");
    eputs("   -f=<path>  入力する行(文字列)を<path>に該当するﾌｧｲﾙ名にする(省略時:*.*)\n");
    eputs(" ｵﾌﾟｼｮﾝ以外の文字列が現れるとそれより後の文字列を出力する文字列とします\n");
    eputs(" -o,s,fの'='は省略できます\n");
    exit(0);
}


int
main(int argc, char * argv[])
{
    static char cmd[1024];
    static char buf[310];
    FILE *fp, *ofp;
    char *o_name, *f_name;
    char o_flg, f_flg;
    word c, i;
    char *p;
    struct find_t ft;

    cmd[0] = buf[0] = '\0';
    f_flg = o_flg = 0;
    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            p = argv[i] + 1;
            if (*p == '\0')
                usage();
            while ((c = toupper(*p)) != '\0') {
                ++p;
                switch (c) {
                case 'F':
                    f_flg = 2;
                    if (*p == '=')
                        ++p;
                    if (*p != 0)
                        f_name = p;
                    else
                        f_name = "*.*";
                    goto LOOPOUT1;
                case 'S':
                    if (*p == '=')
                        ++p;
                    if (*p != 0) {
                        f_flg = 1;
                        f_name = p;
                    } else
                        f_flg = 0;
                    goto LOOPOUT1;
                case 'O':
                    if (*p == '=')
                        ++p;
                    if (*p != '\0') {
                        o_flg = 1;
                        o_name = p;
                    } else
                        o_flg = 0;
                    goto LOOPOUT1;
                case '-':
                    ++i;
                    goto J1;
                case '?':
                    usage();
                default:
                    ep_exit("ｵﾌﾟｼｮﾝ指定がおかしい\n");
                }
            }
        } else {
 J1:
            for (; i < argc; ++i) {
                strcat(cmd, argv[i]);
                strcat(cmd, " ");
            }
 /*
            fprintf(stderr,"%s\n",cmd);
  */
            break;
        }
 LOOPOUT1:;
    }

    if (cmd[0] == '\0')
        ep_exit("文字列を指定してください\n");
    if (o_flg)
        ofp = fopen_e(o_name, "w");
    else
        ofp = stdout;
    p = fnamcpy(buf, f_name);
    switch (f_flg) {
    case 0:
        fp = stdin;
        goto J2;
    case 1:
        fp = fopen_e(buf, "r");
 J2:
        while ((fgets(buf, 300, fp)) != NULL) {
            p = strchr(buf, '\n');
            if (p != NULL)
                *p = '\0';
            one(cmd, buf, ofp);
        }
        fclose(fp);
        break;
    case 2:
        if (*(p - 1) == C_DIRSP)
            strcat(buf, "*.*");
        if (_dos_findfirst(buf, _A_NORMAL | _A_RDONLY, &ft) == 0) {
            one(cmd, ft.name, ofp);
            while (_dos_findnext(&ft) == 0)
                one(cmd, ft.name, ofp);
        }
    }
    if (o_flg)
        fclose(ofp);
    return 0;
}
