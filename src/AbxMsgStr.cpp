/**
 *  @file   AbxMsgStr.cpp
 *  @brief  abx message strings.
 *  @author Masashi KITAMURA (tenka@6809.net)
 *	@license Boost Software License Version 1.0
 */
#include "AbxMsgStr.hpp"

#ifdef __cplusplus
extern "C" {
#endif

AbxMsgStr const abxMsgStrEn = {
	//file_open_error
	"File open error : %s\n",
	//incorrect_command_line_option
	"Incorrect command line option : %s\n",
	//file_read_error
	"File read error : %s\n",
	//cfg_read_error
	"Cfg-file read error : %s\n",
	//conversion_names_list
	":CONVERSION-NAME list\n",
	//key_is_not_defined
	"%s is not defined in %s\n",
	//cfgfile_has_an_incorrect_dollN_specification
	".cfg-file has an incorrect $N specification.: $%s\n",
	//dollN_specification_in_cfgfile_is_incorrect
	"$N=STRING or $N:M{..} specification in .cfg-file is incorrect. : $%s\n",
	//single_quotation_string_is_broken
	"'string' is broken.\n",
	//there_are_more_par_pair_in_a_certain_line
	"There are ten or more {..} in a certain line in %s : %s\n",
	//invalid_par_pair
	"Invalid {..} designation in %s : %s\n",
	//colon_hash_string_can_not_be_specified
	"\":#STRING\" can not be specified. : %s\n",
	//xm_and_Hbegin_can_not_be_used_at_the_same_time
	"-xm and #begin(#end) can not be used at the same time.\n",
	//incorrect_dollar_format
	"Incorrect '%c' format : '%c%c'\n",
	//usage
	ABX_VERTION " Search file, embed file name in text(gen. bat)\n"
	"  https://github.com/tenk-a/abx.git  (build: " __DATE__ ")\n"
	"usage : abx [options] file(s) =conversion_string\n"
	"        abx [options] file(s) 'conversion_string' file(s)\n"
	"        abx [options] file(s) -k=\"conversion_string\" file(s)\n"
	,
	"options:\n"
	" -? -h           help\n"
	" -k[@$RFULBby][=\"conversion_string\"]\n"
	"                 @ $   Make conversion specification character @ or $\n"
	"                 R F U L B b y  Default setting of Conversion assistance.\n"
 #ifdef FKS_WIN32
	" -x[-]           Execute batch. -x- not\n"
 #else
	" -x[-]           Execute shell script. -x- not\n"
 #endif
 #ifdef ENABLE_MT_X
	" -xm[N]          N thread execution. 0=auto\n"
 #endif
	" -r[-]           Directory recursion\n"
	" -a[dfh:N]       Match file attribute\n"
	"                 d:Directory f:File h:Hidden\n"
    "                 N:File attribute value\n"
 #ifdef FKS_WIN32
    " -a[dfh:flags]   flags:rhsdaDntplcoievVxEPUAQ  win32 attribute\n"
 #endif
	" -z[N-M]         Match size(N~M bytes)\n"
	" -d[A-B]         Match date(A~B)\n"
	" -s[neztam]      sort\n"
	"   [rd]          n:Name m:Name&Number e:Extention z:Size\n"
	"                 t:Date a:Attribute r:Reverse d:Directory first\n"
	" -n[-]           Not file find\n"
	" -y              add \" ($cxfgdpw) (same -ky)\n"
	" -l[-]           Response file input name by row\n"
	" -ci[N]          N:Start number of $i\n"
	" -ci[N:M]        Generate N-M serial number without searching for files\n"
	" -e<EXT>         Default extension\n"
	" -o<FILE>        Output file\n"
	" -i<DIR>         Input directory\n"
	" -cd<DIR>        Relative path reference position\n"
	" -w<DIR>         Temporary directory\n"
	" -p<DIR>         Forced setting to @p\n"
	" -ct<FILE>       Match if newer than FILE\n"
	" -ck[-]          Search file names with characters other than ascii\n"
 #ifdef USE_PATH_DBC
	" -cy[-]          Matches files with characters including '\\'n"
 #endif
 #ifdef FKS_WIN32
    " -utf8           Output utf8\n"
 #endif
	" -t[N]           Only the first N items\n"
    " @N=<STR>        N:1-9  Assign to variable\n"
	" +CFGFILE        .cfg file\n"
	" @RESFILE        Respons file\n"
	" :CONVERSION     Conversion defined by .cfg\n"
	" :               CONVERSION list\n"
	"\n"
	,
	//conversion_orders
 #ifdef FKS_WIN32
    "Conversion specification character can be\n"
    "chosen with -k@ or -k$.\n"
    "Those who used before if it does not exist.\n"
 #else
    "Conversion specification character is @@.\n"
    "If -k$ is specified $\n"
 #endif
	"Conversion specification:      Example:\n"
	" @@f  Full path                @f\n"
	"     @@Rf Relative path        @Rf\n"
	"     @@yf Add \"                @yf\n"
	"     @@bf Delimiter /          @bf\n"
	"     @@Bf Delimiter \\          @Bf\n"
	" @@g  Full path without .ext   @g\n"
 #ifdef FKS_WIN32
	" @@v  Drive                    @v\n"
	" @@p  Directory(with drive)    @p\n"
	" @@d  Directory(without drive) @d\n"
 #else
	" @@p  Directory                @p\n"
	" @@d  Directory                @d\n"
 #endif
	" @@D  One directory above      @D\n"
	" @@c  File name                @c\n"
	" @@x  File name without .ext   @x\n"
	" @@e  Extension                @e\n"
	" @@w  Temporary directory      @w\n"
	" @@z  Size(bytes)              @z\n"
	"     KB,MB,GB...        @@+8bz @+8bz\n"
	"     KiB,MiB,GiB...     @@+8Bz @+8Bz\n"
	" @@Z  Size(hex)                @Z\n"
	" @@j  Date                     @j\n"
	"                        @@+8j  @+8j\n"
	"     +time              @@+19j @+19j\n"
	"                        @@+23j @+23j\n"
	" @@J  Date(for label)    @@+23J @+23J\n"
	" @@a  File attribute           @a\n"
 #ifdef FKS_WIN32
	" @@ba Unix like                [ld]rwxrwxrwx\n"
	" @@Ba Win32 file attributes    rhsdalcoievxpu\n"
 #endif
	" @@A  File attribute hex value @A\n"
 #ifdef FKS_WIN32
	" @@BA win32 32bit file attr.   rhs-daDntplcoievVxEPU-A--Q------\n"
 #endif
	" @@i  Serial number            @i\n"
	"                         @@+5i @+5i\n"
	" @@I  Serial number(hex)       @I\n"
    " @N=<STR>   N:1-9  Variable\n"
	" @@l  Raw input string\n"
	" @@t  Tab\n"
	" @@s  Space\n"
	" @@[  <\n"
	" @@]  >\n"
	" @@^  \"\n"
	" @@`  '\n"
	" @@@@  @@\n"
	" @@$  $\n"
	" @@#  #\n"
	"\n"
	"Conversion assistance: (# Is conversion specification).\n"
	" @@+N#  (Minimum) N digits.\n"
	" @@y#   Add \" to both ends of the path\n"
	" @@R#   Relative path\n"
	" @@F#   Full path\n"
	" @@b#   Delimiter /\n"
	" @@B#   Delimiter \\\n"
	" @@U#   Uppercase\n"
	" @@L#   Lowercase\n"
	"\n"
	"ex)@@+24Rbyf @@+8Bz @@+16j\n"
	"   @+24Rbyf @+8Bz @+16j\n"
	,
};

AbxMsgStr const*	abxMsgStr = &abxMsgStrEn;

#ifdef __cplusplus
}
#endif
