/**
 *  @file   AbxMsgStr.hpp
 *  @brief  abx message strings.
 *  @author Masashi KITAMURA (tenka@6809.net)
 *	@license Boost Software License Version 1.0
 */
#ifndef ABX_MSGSTR_HPP_INCLUDED
#define ABX_MSGSTR_HPP_INCLUDED

#include <stddef.h>
#include <stdio.h>

#include <fks_io_mbs.h>


#ifdef __cplusplus
extern "C" {
#endif

#define ABX_VERTION			"abx v3.95(pre v4)"

typedef struct AbxMsgStr {
	char const*		file_open_error;
	char const*		incorrect_command_line_option;
	char const*		file_read_error;
	char const*		cfg_read_error;
	char const*		translation_names_list;
	char const*		key_is_not_defined;
	char const*		cfgfile_has_an_incorrect_dollN_specification;
	char const*		dollN_specification_in_cfgfile_is_incorrect;
	char const*		single_quotation_string_is_broken;
	char const*		there_are_more_par_pair_in_a_certain_line;
	char const*		invalid_par_pair;
	char const*		colon_string_can_not_be_specified;
	char const*		xm_and_Hbegin_can_not_be_used_at_the_same_time;
	char const*		usage;
	char const*		usage_options;
} AbxMsgStr;

extern AbxMsgStr const*	abxMsgStr;
extern AbxMsgStr const 	abxMsgStrEn;

#ifdef __cplusplus
}
#endif

#define	ABXMSG(x)		FKS_MBSO(abxMsgStr->x)

#endif