/* tc 用 _dos_findfirst/next 関数
 *
 * LSI-C(MS-C)にあってＴＣにない_dos_findfirst(),_dos_findnext()を、以下の
 * PDSのｿｰｽよりぬきだし、関数名、構造体名をあわせ,errorのあるとき、
 * errnoに値をｾｯﾄするようにしました。
 * 1991.08.26  M.Kitamura  (NIF:NBB00541)
 */
/*---------------------------------------------------------------------------*/
/* dosdir.c --- DOS依存のディレクトリアクセス関数 */
/* $Header: DOSDIR.Cv  1.2  90/11/08 00:19:44  H.Kuno(NIF:MGG02367)  Exp $
 *
 *	DOSに依存したパス名変換やディレクトリアクセス関数群
 *
 * 1st coded by 久野 NIF:MGG02367   Sat, Sep  1 1990  00:15
 *
 *　このソースは public domain としますので、著作者に断りなく、
 * 営利目的もふくめて自由に利用、配布、引用、改変を行ってください。
 */
/*---------------------------------------------------------------------------*/

#include <dos.h>
#include <errno.h>
#include "dos2.h"

#define PTR_OFF(p)	((unsigned short) (p))
#define PTR_SEG(p)	((unsigned short)((unsigned long)(void far*)(p) >> 16))

/***********************************************************************
 * ファイル検索を開始する(DOS #4e)
 *
 *	<<<戻り値>>>
 * 0     ... OK
 * other ... MS-DOS Error code
 *
 *	<<<引数>>>
 * szPathName ... 検索パス名
 * nAttrib    ... 検索ファイルアトリビュート
 * pfind      ... 検索バッファ
 *
 * ＭＳ－Ｃライブラリコンパチ（名前の衝突回避のため関数名・構造体名は
 * 変えてあります ※）の仕様です。intdosx() を使用して処理系の違いの吸収
 * を計っています。（intdosxも処理系依存なので一部条件コンパイルがあり
 * ます）
 * ※  TC用に関数名、構造体名をMS-C(LSI-C)のものにあわせなおしました。
 **********************************************************************/
unsigned
_dos_findfirst(char* szPathName, unsigned nAttrib, struct find_t *pfind)
{
    union REGS reg;
    struct SREGS sreg;

	/* set DTA */
    reg.h.ah = 0x1a;
    reg.x.dx = PTR_OFF(pfind);
    sreg.ds  = PTR_SEG(pfind);
    intdosx(&reg, &reg, &sreg);
	/* findfirst */
    reg.h.ah = 0x4e;
    reg.x.cx = nAttrib;
    reg.x.dx = PTR_OFF(szPathName);
    sreg.ds  = PTR_SEG(szPathName);
    intdosx(&reg, &reg, &sreg);
	if (pfind->name[0] == 0x05)
            pfind->name[0] = 0xe5; /* DOS 3.1以前のバグ対策 */
#if defined(LSI_C)
    return (reg.x.flags & 1) ? (errno = reg.x.ax) : 0;
#else
    return (reg.x.cflag) ? (errno = reg.x.ax) : 0;
#endif
}

/***********************************************************************
 * dos_findfirst() に続いてファイル検索を行う(DOS #4f)
 *
 *	<<<戻り値>>>
 * 0     ... OK
 * other ... MS-DOS Error code
 *
 *	<<<引数>>>
 * pfind      ... 検索バッファ(_dos_findfirst()で処理済みのもの)
 **********************************************************************/
unsigned
_dos_findnext(struct find_t *pfind)
{
    union REGS reg;
    struct SREGS sreg;

	/* set DTA */
    reg.h.ah = 0x1a;
    reg.x.dx = PTR_OFF(pfind);
    sreg.ds  = PTR_SEG(pfind);
    intdosx(&reg, &reg, &sreg);
	/* findnext */
    reg.h.ah = 0x4f;
    intdos(&reg, &reg);
	if (pfind->name[0] == 0x05)
            pfind->name[0] = 0xe5; /* DOS 3.1以前のバグ対策 */
#if defined(LSI_C)
    return (reg.x.flags & 1) ? (errno = reg.x.ax) : 0;
#else
    return (reg.x.cflag) ? (errno = reg.x.ax) : 0;
#endif
}
