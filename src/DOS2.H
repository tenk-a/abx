/* tc 用 _dos_???? 関数 のﾍｯﾀﾞ
 * ＴＣにないLSI-C(MS-C)のdos.hの_dos_????()系の関数の一部をｻﾎﾟｰﾄﾞします。
 * (MSC,QCをもってるわけでなく、LSI-CとQC本をもとに作成してます)
 *
 * なお、_dos_findfirst(),_dos_findnextは、以下の PDSのｿｰｽよりぬきだし、
 * 関数名、構造体名をもとに合わせなおしました。
 * また、他の関数もこれらを参考に作成しました。
 * たうぜん、このｿｰｽもPDSです。
 * 1991.08.26  M.Kitamura  (NIF:NBB00541)
 */
/*-------------------------------------------------------------------------*/
/* dosdir.h --- DOS依存のディレクトリアクセス関数 */
/* $Header: DOSDIR.Hv  1.1  90/09/12 01:23:44  H.Kuno(NIF:MGG02367)  Exp $
 * 1st coded by 久野 NIF:MGG02367   Sat, Sep  1 1990  00:16
 *
 *　このソースは public domain としますので、著作者に断りなく、
 * 営利目的もふくめて自由に利用、配布、引用、改変を行ってください。
 */
/*-------------------------------------------------------------------------*/

#ifndef _DOS2_H_
#define _DOS2_H_

/* DOSのファイルアトリビュート値 */
#define _A_NORMAL 0x00
#define _A_RDONLY 0x01
#define _A_HIDDEN 0x02
#define _A_SYSTEM 0x04
#define _A_VOLID  0x08
#define _A_SUBDIR 0x10
#define _A_ARCH   0x20
/* ファイル検索のバッファ(dos_findfirst/next()用) */
struct find_t {
	char reserved[21];
	char attrib;
	unsigned wr_time;
	unsigned wr_date;
	long size;
	char name[13];
};

unsigned _dos_findfirst(char *PathName, unsigned Attrib, struct find_t *pfind);
unsigned _dos_findnext(struct find_t *pfind);
unsigned _dos_read(int hno, void far *buf, unsigned siz, unsigned *cnt);
#endif /* dos2.h --- end. */
