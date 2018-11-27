/**
 *	@file	abx_usage.c
 *	@brief	usage for abx
 *	@author Masashi KITAMURA (tenka@6809.net)
 *	@license	Boost Software License Version 1.0
 */

#include <stddef.h>
#include <fks/fks_config.h>
#include <stdio.h>
#include <subr.hpp>
#include <string>
#include "abx_usage.h"

#ifdef _WIN32
#include <windows.h>
#endif

#if 1
#define APP_HELP_TITLE_JP	"abx v3.93(pre v4) �t�@�C��������,�t�@�C�����𕶎���ɖ���(�o�b�`����)\n"	\
							"  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"
#endif
#define APP_HELP_TITLE		"abx v3.93(pre v4) Search file, embed file name in text(gen. bat)\n"	\
							"  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"

#define APP_HELP_CMDLINE	"usage : %s [option]  filename(s) 'text conversion'\n"	\
							"        %s [option]  filename(s) =text conversion\n"


static char const* s_helpJp =
		"Option:\n"
		" -x[-]      �o�b�`���s -x-���Ȃ�\n"
		" -xm[N]     N�X���b�h���s.0����\n"
		" -r[-]      �f�B���N�g���ċA\n"
		" -a[nrhsd]  �w��t�@�C�������Ō���\n"
		"            n:��� s:�V�X�e�� h:�B��\n"
		"            r:�Ǎ���p d:�f�B���N�g��\n"
		" -z[N-M]    �T�C�Y N�`M��FILE������\n"
		" -d[A-B]    ���tA�`B��FILE������\n"
		" -s[neztam] �\�[�g(����)\n"
		"   [r]      n:�� m:��(��) e:�g���q z:�T�C�Y\n"
		"            t:���t a:���� r:�~��\n"
		" -n[-]      �t�@�C���������Ȃ� -n-�L \n"
		// " -u[-]      $c|$C�Ńt�@�C�����召����\n"
		" -y         $cxfgdpw��\"�t��\n"
		" -l[-]      @���͂Ŗ��O�͍s�P��\n"
		" -ci[N:M]   N:$i�̊J�n�ԍ�(M:�I)\n"
		" +CFGFILE   .CFG�t�@�C���w��\n"
		" @RESFILE   ���X�|���X�t�@�C��\n"
		" :�ϊ���    CFG�Œ�`�����ϊ�\n"
		" :          �ϊ����ꗗ��\��\n"
		" -e<EXT>    �f�t�H���g�g���q\n"
		" -o<FILE>   �o�̓t�@�C���w��\n"
		" -i<DIR>    �����f�B���N�g��\n"
		" -cd<DIR>   ���΃p�X�������̃J�����g�f�B���N�g��\n"
		" -w<DIR>    TMP�f�B���N�g��\n"
		" -p<DIR>    $p�̋����ύX\n"
		" -ct<FILE>  FILE���V�Ȃ�\n"
		" -ck[-]     ���{�ꖼ�̂݌���\n"
		" -cy[-]     \\���܂ޑS�p������\n"
		" -t[N]      �ŏ���N�̂ݏ���\n"
		"\n"
		"�ϊ�����:                    �ϊ���:\n"
		" $f �t���p�X(�g���q�t)       d:\\dir\\dir2\\filename.ext\n"
		" $g �t���p�X(�g���q��)       d:\\dir\\dir2\\filename\n"
		" $v �h���C�u                 d\n"
		" $p �f�B���N�g��(�h���C�u�t) d:\\dir\\dir2\n"
		" $d �f�B���N�g��(�h���C�u��) \\dir\\dir2\n"
		" $D ����P�̃f�B���N�g��   dir\n"
		" $c �t�@�C��(�g���q�t)       filename.ext\n"
		" $x �t�@�C��(�g���q��)       filename\n"
		" $e �g���q                   ext\n"
		" $w �e���|�����E�f�B���N�g�� (���ϐ�TMP�̓��e)\n"
		" $z �T�C�Y(10�i10��)         1234567890\n"
		" $Z �T�C�Y(16�i8��)          ABCDEF1234\n"
		" $j ����                     1993-02-14 , 2010-04-03 04:02:05 999\n"
		" $J ����(�t�@�C�����p)       2010-04-03_04_02_05_999\n"
		" $i �A�Ԑ��� 10�i��\n"
		" $I �A�Ԑ��� 16�i��\n"
		" $$ $  $[ <  $` '  $n ���s  $t �^�u\n"
		" $# #  $] >  $^ \"  $s ��  $l �����͂̂܂�\n"
		"\n"
		"�ϊ��⏕����: �ȉ�@�͕ϊ�����.\n"
		" $+?@   �ϊ�����@�ōŒ�?���ɂ���.(0�����E�l�ŋ󔒕⊮)\n"
		" $R@    �t���p�X�\�L�𑊑΃p�X�\�L�ɕϊ�\n"
		" $U@    �啶����\n"
		" $u@    ��������\n"
		" $B@    �p�X��\\��/�ɒu��\n"
		" $b@    �p�X��/��\\�ɒu��\n"
		;


int abx_usage(char const* exename)
{
 #ifdef _WIN32
	if (GetUserDefaultLCID() == 1041) {
	 #if 0
		printf(UTF8toCONS(APP_HELP_TITLE_JP APP_HELP_CMDLINE), exename, exename);
		printf("%s", UTF8toCONS(s_helpJp));
	 #else
		printf(DBStoCONS(APP_HELP_TITLE_JP APP_HELP_CMDLINE), exename, exename);
		printf("%s", DBStoCONS(s_helpJp));
	 #endif
	} else {
		printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
	}
 #else
	printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
 #endif
	return 1;
}
