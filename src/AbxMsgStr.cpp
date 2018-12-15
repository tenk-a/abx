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
	"usage : abx [option]  file(s) 'conversion_string' file(s)\n"	\
	"        abx [option]  file(s) =conversion_string\n"
	,
	//usage_options
	"options:\n"
	" -? -h -x -xm[N] -r -a[dnrhs] -z[N-M] -d[A-B] -s[neztam][r] -n[-] -y -l[-] -ci[N]\n"
	" -e<EXT> -o<FILE> -i<DIR> -cd<DIR> -w<DIR> -p<DIR> -ct<FILE> -ck[-] -t[N]\n"
	" +CFGFILE @RESFILE :CONVERSION-NAME\n"
	,
};

AbxMsgStr const*	abxMsgStr = &abxMsgStrEn;

#ifdef __cplusplus
}
#endif
