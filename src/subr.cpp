/**
 *  @file   subr.cpp
 *  @brief  �T�u���[�`��
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *  	Boost Software License Version 1.0
 */
#include "subr.hpp"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
//#include <time.h>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif



/*---------------------------------------------------------------------------*/

char const* StrSkipSpc(char const* s)
{
    while (*s && *(unsigned char *)s <= ' ')
    	s++;
    return s;
}

char const* StrSkipNotSpc(char const* s)
{
    while (*(unsigned char *)s > ' ')
    	s++;
    return s;
}


/*---------------------------------------------------------------------------*/

/* �Ƃ肠�����A�A�Z���u���\�[�X�Ƃ̌��ˍ����ŁA�_�~�[�֐���p�� */
static int  FIL_zenFlg = 1; 	    /* 1:MS�S�p�ɑΉ� 0:���Ή� */


void	FIL_SetZenMode(int ff)
{
    FIL_zenFlg = ff;
}

int FIL_GetZenMode(void)
{
    return FIL_zenFlg;
}
