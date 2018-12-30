/**
 *  @file   fks_argv.h
 *  @brief  Extended argv (Recursive search, Response file etc.)
 *  @author Masashi KITAMURA
 *  @date   2006-2018
 *  @note
 *  -	int main(int argc,char* argv[]) {
 *			fks_ioMbsInit(1,0);	// internal: utf-8	output: defalut codepage
 *			fks_argv_conv(&argc, &argv);
 *	-	int wmain(int argc, wchar_t* wargv[]) {
 *			char** argv;
 *			fks_ioMbsInit(1,1);	// internal: utf-8	output: utf-8
 *			argv = fks_convArgWcsToMbs(&argc, wargv);
 *			fks_argv_conv(&argc, &argv);
 */

#ifndef FKS_ARGV_H_INCLUDED
#define FKS_ARGV_H_INCLUDED

#include <fks_common.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (void) fks_argv_conv(int* pArgc, char*** pppArgv);

#if defined FKS_WIN32 && !defined(_CONSOLE)	    // win-gui ŠÂ‹«—p.
  void fks_argv_forWinMain(const char* pCmdLine, int* pArgc, char*** pppArgv);
#endif


#ifdef __cplusplus
}
#endif

#endif	// FKS_ARGV_H_INCLUDED
