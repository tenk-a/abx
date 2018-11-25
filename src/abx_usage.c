/**
 *  @file   abx_usage.c
 *  @brief  usage for abx
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license	Boost Software License Version 1.0
 */

#include <stdio.h>
#include <abx_usage.h>

#define APP_HELP_TITLE_JP	"abx v3.93(pre v4) ̧�ٖ�������,�Y��̧�ٖ��𕶎���ɖ���(�ޯ�����)\n"	\
    	    	    	    "  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"

#define APP_HELP_TITLE	    "abx v3.93(pre v4) Search file, embed file name in text(gen. bat)\n"    \
    	    	    	    "  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"

#define APP_HELP_CMDLINE    "usage : %s [option]  filename(s) 'text conversion'\n"	\
						    "        %s [option]  filename(s) =text conversion\n"

int abx_usage(char const* exename)
{
 #if _WIN32
	if (GetUserDefaultLCID() == 1041) {
		printf(APP_HELP_TITLE_JP APP_HELP_CMDLINE, exename, exename);
	} else {
		printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
	}
 #else
	printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename, exename);
 #endif
	printf("%s", 
		"Option:\n"
		" -x[-]      �o�b�`���s -x-���Ȃ�   \n"	
		" -xm[N]     N�X���b�h���s.0����  \n"	
		" -r[-]      �f�B���N�g���ċA          \n"	
		" -a[nrhsd]  �w��t�@�C�������Ō���   \n"	
		"            n:��� s:�V�X�e�� h:�B�� \n"	
		"            r:�Ǎ���p d:�f�B���N�g�� \n"	
		" -z[N-M]    �T�C�Y N�`M��FILE������ \n"	
		" -d[A-B]    ���tA�`B��FILE������ \n"	
		" -s[neztam] �\�[�g(����)            \n"	
		"   [r]      n:�� e:�g���q z:�T�C�Y \n"	
		"            t:���t a:���� r:�~�� \n"	
		"            m:��(��)             \n"	
		" -n[-]      �t�@�C���������Ȃ� -n-�L \n"	
		" -u[-]      $c|$C�Ńt�@�C�����召����\n"	
		" -l[-]      @���͂Ŗ��O�͍s�P��\n"	
		" -ci[N:M]   N:$i�̊J�n�ԍ�(M:�I)\n"	
		" +CFGFILE   .CFG�t�@�C���w��\n"	
		" @RESFILE   ���X�|���X�t�@�C��\n"	
		" :�ϊ���    CFG�Œ�`�����ϊ�\n"	
		" :          �ϊ����ꗗ��\��\n"	
		" -p<DIR>    $p�̋����ύX\n"
		" -e<EXT>    �f�t�H���g�g���q\n"
		" -o<FILE>   �o�̓t�@�C���w��\n"
		" -i<DIR>    �����f�B���N�g��\n"
		" -w<DIR>    TMP�f�B���N�g��\n"
		" -ct<FILE>  FILE���V�Ȃ�\n" 
		" -ck[-]     ���{�ꖼ�̂݌���\n"  
		" -cy[-]     \\���܂ޑS�p������\n"
		" -y         $cxfgdpw��\"�t��\n"  
		" -t[N]      �ŏ���N�̂ݏ���\n" 
		"\n"
		"�ϊ�����:             �ϊ���:\n"
		" $f �t���p�X(�g���q�t)      d:\\dir\\dir2\\filename.ext\n" 
		" $g �t���p�X(�g���q��)      d:\\dir\\dir2\\filename\n"	 
		" $v �h���C�u                d\n"    	    	    	    
		" $p �f�B���N�g��(�h���C�u�t) d:\\dir\\dir2\n"	    	    
		" $d �f�B���N�g��(�h���C�u��) \\dir\\dir2\n" 
		" $D ����P�̃f�B���N�g��  dir\n"
		" $c �t�@�C��(�g���q�t)      filename.ext\n" 	    	    
		" $x �t�@�C��(�g���q��)      filename\n"     	    	    
		" $e �g���q                  ext\n"  	    	    	    
		" $w �e���|�����E�f�B���N�g�� (���ϐ�TMP�̓��e)\n"  	    
		" $z �T�C�Y(10�i10��)        1234567890\n"
		" $Z �T�C�Y(16�i8��)         ABCDEF1234\n"
		" $j ����                    1993-02-14 , 2010-04-03 04:02:05\n"   	    	    
		" $J ����(�t�@�C�����p)      2010-04-03_04.02.05.999\n"   	    	    
		" $i �A�Ԑ��� 10�i��\n"
		" $I �A�Ԑ��� 16�i��\n"
		" $$ $  $[ <  $` '  $n ���s  $t ���\n" 	    	    
		" $# #  $] >  $^ \"  $s ��  $l �����͂̂܂�\n"	 
		"�ϊ��⏕����: �ȉ�@�͕ϊ�����. ??�͑�p����(����)\n"
        " $+??@  �ϊ�����@�ł̍Œጅ��??\n"
        " $R@    �t���p�X�\�L�ϊ�@ �𑊑΃p�X�\�L�ɕϊ�\n"
        " $U@    �啶����\n"
        " $u@    ��������\n"
        " $B@    �p�X��\\��/�ɒu��\n"
        " $b@    �p�X��/��\\�ɒu��\n"
	);
	return 1;
}
