/*
	ファイル関係
 */

#include <string.h>
#include "fil.h"

#define ISKANJI(c)	((unsigned)((c)^0x20) - 0xa1 < 0x3cU)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static int	FIL_zenFlg = 1;			/* 1:MS全角に対応 0:未対応 */
static int	FIL_wcFlg  = 0x08;


void	FIL_SetZenMode(int ff)
{
	FIL_zenFlg = ff;
}

int	FIL_GetZenMode(void)
{
	return FIL_zenFlg;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void	FIL_SetWcMode(int ff)
{
	FIL_wcFlg = ff;
}

int	FIL_GetWcMode(void)
{
	return FIL_wcFlg;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

char *FIL_BaseName(char *adr)
{
	char *p;

	p = adr;
	while (*p != '\0') {
		if (*p == ':' || *p == '/' || *p == '\\')
			adr = p + 1;
		if (FIL_zenFlg && ISKANJI((*(unsigned char *)p)) && *(p+1) )
			p++;
		p++;
	}
	return adr;
}

char *FIL_ChgExt(char filename[], char *ext)
{
	char *p;

	p = FIL_BaseName(filename);
	p = strrchr( p, '.');
	if (p == NULL) {
		if (ext) {
			strcat(filename,".");
			strcat( filename, ext);
		}
	} else {
		if (ext)
			strcpy(p+1, ext);
		else
			*p = 0;
	}
	return filename;
}

char *FIL_AddExt(char filename[], char *ext)
{
	if (strrchr(FIL_BaseName(filename), '.') == NULL) {
		if (ext) {
			strcat(filename,".");
			strcat(filename, ext);
		}
	}
	return filename;
}



