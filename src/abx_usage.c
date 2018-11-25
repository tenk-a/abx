/**
 *  @file   abx_usage.c
 *  @brief  usage for abx
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license	Boost Software License Version 1.0
 */

#include <stdio.h>
#include <abx_usage.h>


#define APP_HELP_TITLE	    "abx v3.93(pre v4) ̧�ٖ�������,�Y��̧�ٖ��𕶎���ɖ���(�ޯ�����)\n"   	    	    \
    	    	    	    "  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"
#define APP_HELP_CMDLINE    "usage : %s [��߼��] ['�ϊ�������'] ̧�ٖ� [=�ϊ�������]\n"
#define APP_HELP_OPTS	    "��߼��:                        ""�ϊ�����:            �ϊ���:\n"	    	    	    \
    	    	    	    " -x[-]    �ޯ����s -x-���Ȃ�   "" $f ���߽(�g���q�t)   d:\\dir\\dir2\\filename.ext\n"  \
    	    	    	    " -xm[N]   N�X���b�h���s.0����  "" $g ���߽(�g���q��)   d:\\dir\\dir2\\filename\n"	    \
    	    	    	    " -r[-]    �ިڸ�؍ċA          "" $v ��ײ��            d\n"    	    	    	    \
    	    	    	    " -a[nrhsd] �w��̧�ّ����Ō���  "" $p �ިڸ��(��ײ�ޕt) d:\\dir\\dir2\n"	    	    \
    	    	    	    "          n:��� s:���� h:�B�� "" $d �ިڸ��(��ײ�ޖ�) \\dir\\dir2\n"  	    	    \
    	    	    	    "          r:ذ�޵�ذ d:�ިڸ�� "" $c ̧��(�g���q�t)    filename.ext\n" 	    	    \
    	    	    	    " -z[N-M]  ����N�`M��FILE������ "" $x ̧��(�g���q��)    filename\n"     	    	    \
    	    	    	    " -d[A-B]  ���tA�`B��FILE������ "" $e �g���q            ext\n"  	    	    	    \
    	    	    	    " -s[neztam][r] ���(����)       "" $w �����إ�ިڸ��    (���ϐ�TMP�̓��e)\n"  	    \
    	    	    	    "          n:�� e:�g���q z:���� "" $z ����(10�i10��)    1234567890 ��$Z�Ȃ�16�i8��\n"   \
    	    	    	    "          t:���t a:���� r:�~�� "" $j ����              1993-02-14\n"   	    	    \
    	    	    	    "          m:��(��)             "" $i �A�Ԑ���          ��$I�Ȃ�16�i��\n"	    	    \
    	    	    	    " -n[-]    ̧�ٌ������Ȃ� -n-�L "" $$ $  $[ <  $` '  $n ���s  $t ���\n" 	    	    \
    	    	    	    " -u[-]    $c|$C��̧�ٖ��召����"" $# #  $] >  $^ \"  $s ��  $l �����͂̂܂�\n"	    \
    	    	    	    " -l[-]    @���͂Ŗ��O�͍s�P��  ""------------------------------------------------\n"   \
    	    	    	    " -ci[N:M] N:$i�̊J�n�ԍ�(M:�I) ""-p<DIR>  $p�̋����ύX   ""-ct<FILE> FILE���V�Ȃ�\n" \
    	    	    	    " +CFGFILE .CFĢ�َw��         ""-e<EXT>  ��̫�Ċg���q   ""-ck[-] ���{�ꖼ�̂݌���\n"  \
    	    	    	    " @RESFILE ڽ��ݽ̧��           ""-o<FILE> �o��̧�َw��   ""-cy[-] \\���܂ޑS�p������\n"\
    	    	    	    " :�ϊ���  CFG�Œ�`�����ϊ�    ""-i<DIR>  �����ިڸ��    ""-y     $cxfgdpw��\"�t��\n"  \
    	    	    	    " :        �ϊ����ꗗ��\��     ""-w<DIR>  TMP�ިڸ��     ""-t[N]  �ŏ���N�̂ݏ���\n" \

int abx_usage(char const* exename)
{
	printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename);
	printf("%s", APP_HELP_OPTS);
	return 1;
}
