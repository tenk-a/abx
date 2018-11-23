/**
 *	@file	fname.h
 *	@brief	file-name functions
 *	@author Masashi Kitamura (tenka@6809.net)
 */
#ifndef FKS_FNAME_H_INCLUDED
#define FKS_FNAME_H_INCLUDED

#include <fks/fks_config.h>
#include <stddef.h>
#include <string.h>

#ifndef FKS_NOEXCEPT
 #ifdef __cplusplus
  #define FKS_NOEXCEPT		throw()	//noexcept
 #else
  #define FKS_NOEXCEPT
 #endif
#endif
#ifndef FKS_LIB_DECL
 #define FKS_LIB_DECL(t)		t
#endif
#ifndef FKS_INL_LIB_DECL
 #define FKS_INL_LIB_DECL(t)	static inline t
#endif


// ============================================================================
// �}�N���֌W.

// Win�ȊO(unix�n)�� �t�@�C�������̑S�p�� \ �΍���������ꍇ�͒�`.
// ������`����Ɗ��ϐ� LANG ���݂� SJIS,gbk,gb2312,big5�Ȃ�MBC����������.
//#define FKS_USE_FNAME_MBC

#if defined _WIN32 || defined _DOS || defined _MSDOS
#define FKS_FNAME_WINDOS
#endif

// �t�@�C�����̃T�C�Y. ���p���̃o�b�t�@�����p.
// ���X�̓p�X�S�̂̐��������������� 1�t�@�C�����̒����ɂȂ��Ă��銴��.
// �� win-api ���̂͊�{�I�Ƀp�X�S�̂ł��̐������󂯂�.
// fname.cpp �ł́Afks_fnameRelativePath?? �݂̂����̒l���g��. (���͎Q�Ƃ��Ȃ�)
#ifndef FKS_FNAME_MAX_PATH
 #ifdef _WIN32
  #define FKS_FNAME_MAX_PATH	260/*_MAX_PATH*/		///< �ʏ�̃p�X���̒���.��winnt�n�̏ꍇ1�t�@�C�����̒���.
 #else
  #define FKS_FNAME_MAX_PATH	1024					///< �ʏ�̃p�X���̒���.
 #endif
#endif

// url����?�����ꍇ�ɂ���Ă͂��ꂪ�p�X�̍ő咷. win�̏ꍇ �Œ���0x8000 . ����ȊO�͓K��.
// (�� win�ł͓���Ȏw������Ȃ���_PAX_PATH�𒴂��Ă͎g���Ȃ�)
#ifndef FKS_FNAME_MAX_URL
 #ifdef _WIN32
  #define FKS_FNAME_MAX_URL 	(0x8000)					///< url�Ƃ��Ĉ����ꍇ�̃p�X���T�C�Y.
 #else	// �K���Ɍv�Z.
  #define FKS_FNAME_MAX_URL 	(6U*4*FKS_FNAME_MAX_PATH)	///< url�Ƃ��Ĉ����ꍇ�̃p�X���T�C�Y.
 #endif
#endif

#if defined FKS_FNAME_WINDOS
#define FKS_FNAME_SEP_CHR		'\\'
#define FKS_FNAME_SEP_STR		"\\"
#define FKS_FNAME_SEP_WCS		L"\\"
#define FKS_FNAME_SEP_TCS		_T("\\")
#else
#define FKS_FNAME_SEP_CHR		'/'
#define FKS_FNAME_SEP_STR		"/"
#define FKS_FNAME_SEP_WCS		L"/"
#define FKS_FNAME_SEP_TCS		"/"
#endif

#ifndef FKS_C_CONST
#ifdef __cplusplus
#define FKS_C_CONST 			// c++�̏ꍇ�� ��{�͔�const�ŁAconst,��const�Q��ލ��.
#else	// c �̂Ƃ�.
#define FKS_C_CONST 	const	// C�̏ꍇ�� ������const, �Ԃ�l�͔�const �ɂ���ꍇ�Ɏg��.
#endif
#endif


#ifndef FKS_FNAME_SIZE
typedef size_t			FKS_FNAME_SIZE;
#define FKS_FNAME_SIZE	FKS_FNAME_SIZE
#endif

#if defined FKS_FNAME_WINDOS
FKS_INL_LIB_DECL (int)		fks_fnameIsSep(unsigned c) FKS_NOEXCEPT { return c == '\\' || c == '/'; }
#else
FKS_INL_LIB_DECL (int)		fks_fnameIsSep(unsigned c) FKS_NOEXCEPT { return c == '/'; }
#endif

// ============================================================================
// char version

#if !(defined FKS_FNAME_WCS_COMPILE)

#ifdef __cplusplus
extern "C" {
#endif

FKS_INL_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameLen(const char* path) FKS_NOEXCEPT { return (FKS_FNAME_SIZE)strlen(path); }

FKS_LIB_DECL (int)		fks_fnameIsAbs(const char* path) FKS_NOEXCEPT;									///< ��΃p�X���ۂ�(�h���C�u���̗L���͊֌W�Ȃ�)
FKS_LIB_DECL (int)		fks_fnameHasDrive(const char* path) FKS_NOEXCEPT;								///< �h���C�u�������Ă��邩. (file: �� http:���h���C�u����)

FKS_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameAdjustSize(const char* str, FKS_FNAME_SIZE size) FKS_NOEXCEPT; ///< (�Ȃ�ׂ��������󂳂Ȃ���)size�����ȓ��̕�������Ԃ�.
FKS_LIB_DECL (char*) 	fks_fnameCpy(char dst[], FKS_FNAME_SIZE sz, const char* src) FKS_NOEXCEPT; 		///< �t�@�C�����̃R�s�[.
FKS_LIB_DECL (char*) 	fks_fnameCat(char dst[], FKS_FNAME_SIZE sz, const char* src) FKS_NOEXCEPT; 		///< �t�@�C����������̘A��.

FKS_LIB_DECL (char*) 	fks_fnameBaseName(FKS_C_CONST char *adr) FKS_NOEXCEPT;							///< �t�@�C���p�X�����̃f�B���N�g�����������t�@�C�����̈ʒu��Ԃ�.
FKS_LIB_DECL (char*) 	fks_fnameExt(FKS_C_CONST char *name) FKS_NOEXCEPT;								///< �g���q�̈ʒu��Ԃ�.
FKS_LIB_DECL (char*) 	fks_fnameSkipDrive(FKS_C_CONST char *name) FKS_NOEXCEPT; 						///< �h���C�u�����X�L�b�v�����ʒu��Ԃ�.
FKS_LIB_DECL (char*) 	fks_fnameSkipDriveRoot(FKS_C_CONST char* name) FKS_NOEXCEPT; 					///< �h���C�u���ƃ��[�g�w�蕔�����X�L�b�v�����ʒu��Ԃ�.

FKS_LIB_DECL (char*) 	fks_fnameDelExt(char name[]) FKS_NOEXCEPT;											///< �g���q���폜����.
FKS_LIB_DECL (char*) 	fks_fnameGetNoExt(char dst[], FKS_FNAME_SIZE sz, const char *src) FKS_NOEXCEPT;		///< �g���q���O�������O���擾.
FKS_LIB_DECL (char*) 	fks_fnameGetBaseNameNoExt(char d[], FKS_FNAME_SIZE l, const char *s) FKS_NOEXCEPT;	///< �f�B���N�g���Ɗg���q���O�������O���擾.

FKS_LIB_DECL (char*) 	fks_fnameSetExt(char dst[], FKS_FNAME_SIZE sz, const char* src, const char *ext) FKS_NOEXCEPT; 			///< �g���q���Aext �ɕύX����.
FKS_LIB_DECL (char*) 	fks_fnameSetDefaultExt(char dst[], FKS_FNAME_SIZE sz, const char* src, const char *ext) FKS_NOEXCEPT;	///< �g���q���Ȃ���΁Aext ��ǉ�����.
FKS_LIB_DECL (char*) 	fks_fnameJoin(char dst[],FKS_FNAME_SIZE sz,const char *dir,const char *nm) FKS_NOEXCEPT;				///< �f�B���N�g�����ƃt�@�C�����̘A��.

FKS_LIB_DECL (char*) 	fks_fnameGetDir(char dir[], FKS_FNAME_SIZE sz, const char *nm) FKS_NOEXCEPT;	///< �f�B���N�g�����̎擾.
FKS_LIB_DECL (char*) 	fks_fnameGetDrive(char drv[], FKS_FNAME_SIZE sz, const char *nm) FKS_NOEXCEPT; 	///< �h���C�u�����擾.
FKS_LIB_DECL (char*) 	fks_fnameGetDriveRoot(char dr[],FKS_FNAME_SIZE sz,const char *nm) FKS_NOEXCEPT;	///< �h���C�u�����擾.

FKS_LIB_DECL (char*) 	fks_fnameCheckPosSep(FKS_C_CONST char* dir, int pos) FKS_NOEXCEPT;				///< pos�̈ʒu��\��/������΂��̃A�h���X���Ȃ����NULL��Ԃ�.
FKS_LIB_DECL (char*) 	fks_fnameCheckLastSep(FKS_C_CONST char* dir) FKS_NOEXCEPT;						///< �Ō��\��/������΂��̃A�h���X���Ȃ����NULL��Ԃ�.
FKS_LIB_DECL (char*) 	fks_fnameDelLastSep(char dir[]) FKS_NOEXCEPT;									///< ������̍Ō�� \ �� / ������΍폜.
FKS_LIB_DECL (char*) 	fks_fnameAddSep(char dst[], FKS_FNAME_SIZE sz) FKS_NOEXCEPT;					///< ������̍Ō�� \ / ���Ȃ���Βǉ�.

FKS_LIB_DECL (char*) 	fks_fnameToUpper(char filename[]) FKS_NOEXCEPT;									///< �S�p�Q�o�C�g�ڂ��l������ strupr.
FKS_LIB_DECL (char*) 	fks_fnameToLower(char filename[]) FKS_NOEXCEPT;									///< �S�p�Q�o�C�g�ڂ��l������ strlwr.
//FKS_LIB_DECL (char*) 	fks_fnameToUpperN(char filename[], size_t n) FKS_NOEXCEPT;						///< �S�p�Q�o�C�g�ڂ��l������ strupr.
//FKS_LIB_DECL (char*) 	fks_fnameToLowerN(char filename[], size_t n) FKS_NOEXCEPT;						///< �S�p�Q�o�C�g�ڂ��l������ strlwr.

FKS_LIB_DECL (char*) 	fks_fnameBackslashToSlash(char filePath[]) FKS_NOEXCEPT; 						///< filePath���� \ �� / �ɒu��.
FKS_LIB_DECL (char*) 	fks_fnameSlashToBackslash(char filePath[]) FKS_NOEXCEPT; 						///< filePath���� / �� \ �ɒu��.

FKS_LIB_DECL (char*) 	fks_fnameFullpath  (char fullpath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< �t���p�X����. os�ˑ�.
FKS_LIB_DECL (char*) 	fks_fnameFullpathSL(char fullpath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< �t���p�X����. / ���.
FKS_LIB_DECL (char*) 	fks_fnameFullpathBS(char fullpath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	 	///< �t���p�X����. \ ���.
FKS_LIB_DECL (char*) 	fks_fnameRelativePath  (char relPath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< ���΃p�X����. os�ˑ�.
FKS_LIB_DECL (char*) 	fks_fnameRelativePathSL(char relPath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< ���΃p�X����. / ���.
FKS_LIB_DECL (char*) 	fks_fnameRelativePathBS(char relPath[], FKS_FNAME_SIZE sz, const char* path, const char* curDir) FKS_NOEXCEPT;	///< ���΃p�X����. \ ���.

FKS_LIB_DECL (int)		fks_fnameCmp(const char* l, const char* r) FKS_NOEXCEPT; 						///< �t�@�C�����̑召��r.
FKS_LIB_DECL (int)		fks_fnameNCmp(const char* l, const char* r, FKS_FNAME_SIZE n) FKS_NOEXCEPT;		///< �t�@�C������n�����召��r.
FKS_LIB_DECL (int)		fks_fnameDigitCmp(const char* l, const char* r) FKS_NOEXCEPT;					///< ���Ⴂ�̐����𐔒l�Ƃ��đ召��r.
//FKS_LIB_DECL (int) 	fks_fnameNDigitCmp(const char* l,const char* r,FKS_FNAME_SIZE n) FKS_NOEXCEPT;	///< ���Ⴂ�̐����𐔒l�Ƃ���n�����召��r.
FKS_LIB_DECL (char*) 	fks_fnameEquLong(FKS_C_CONST char* fname, const char* baseName) FKS_NOEXCEPT;	///< fname��baseName�Ŏn�܂��Ă��邩�ۂ�.
FKS_LIB_DECL (int)		fks_fnameMatchWildCard(const char* pattern, const char* str) FKS_NOEXCEPT;		///< ���C���h�J�[�h����(*?)���r. �}�b�`������^.

/// �R�}���h���C��������A;��؂�̕����̃p�X�w�肩��A�P�v�f�擾.
FKS_LIB_DECL (char*) 	fks_fnameScanArgStr(char arg[],FKS_FNAME_SIZE sz,const char *str, unsigned sepChr) FKS_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
FKS_INL_LIB_DECL (const char*)	fks_fnameBaseName(const char *p) FKS_NOEXCEPT							{ return fks_fnameBaseName((char*)p); }
FKS_INL_LIB_DECL (const char*)	fks_fnameExt(const char *name) FKS_NOEXCEPT								{ return fks_fnameExt((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_fnameSkipDrive(const char *name) FKS_NOEXCEPT						{ return fks_fnameSkipDrive((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_fnameSkipDriveRoot(const char *name) FKS_NOEXCEPT					{ return fks_fnameSkipDriveRoot((char*)name); }
FKS_INL_LIB_DECL (const char*)	fks_fnameCheckPosSep(const char* dir, int pos) FKS_NOEXCEPT				{ return fks_fnameCheckPosSep((char*)dir,pos); }
FKS_INL_LIB_DECL (const char*)	fks_fnameCheckLastSep(const char* dir) FKS_NOEXCEPT						{ return fks_fnameCheckLastSep((char*)dir); }
FKS_INL_LIB_DECL (const char*)	fks_fnameEquLong(const char* fname, const char* baseName) FKS_NOEXCEPT	{ return fks_fnameEquLong((char*)fname, baseName); }
FKS_INL_LIB_DECL (char*)		fks_fnameScanArgStr(char arg[],FKS_FNAME_SIZE sz,const char *str) FKS_NOEXCEPT { return fks_fnameScanArgStr(arg,sz,str, ' ');	}
#endif

#endif


// ============================================================================
// wchar_t version
#if defined __cplusplus || defined FKS_FNAME_WCS_COMPILE

FKS_INL_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameLen(const wchar_t* path) FKS_NOEXCEPT { return (FKS_FNAME_SIZE)wcslen(path); }

FKS_LIB_DECL (int)			fks_fnameIsAbs(const wchar_t* path) FKS_NOEXCEPT;									///< ��΃p�X���ۂ�(�h���C�u���̗L���͊֌W�Ȃ�)
FKS_LIB_DECL (int)			fks_fnameHasDrive(const wchar_t* path) FKS_NOEXCEPT;								///< �h���C�u�������Ă��邩. (file: �� http:���h���C�u����)

FKS_LIB_DECL (FKS_FNAME_SIZE)	fks_fnameAdjustSize(const wchar_t* str, FKS_FNAME_SIZE size) FKS_NOEXCEPT; 		///< (�Ȃ�ׂ��������󂳂Ȃ���)size�����ȓ��̕�������Ԃ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameCpy(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src) FKS_NOEXCEPT; 	///< �t�@�C�����̃R�s�[.
FKS_LIB_DECL (wchar_t*) 	fks_fnameCat(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src) FKS_NOEXCEPT; 	///< �t�@�C����������̘A��.

FKS_LIB_DECL (wchar_t*) 	fks_fnameBaseName(FKS_C_CONST wchar_t *adr) FKS_NOEXCEPT;							///< �t�@�C���p�X�����̃f�B���N�g�����������t�@�C�����̈ʒu��Ԃ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameExt(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT;								///< �g���q�̈ʒu��Ԃ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSkipDrive(FKS_C_CONST wchar_t *name) FKS_NOEXCEPT; 						///< �h���C�u�����X�L�b�v�����ʒu��Ԃ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSkipDriveRoot(FKS_C_CONST wchar_t* name) FKS_NOEXCEPT; 					///< �h���C�u���ƃ��[�g�w�蕔�����X�L�b�v�����ʒu��Ԃ�.

FKS_LIB_DECL (wchar_t*) 	fks_fnameDelExt(wchar_t name[]) FKS_NOEXCEPT;												///< �g���q���폜����.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetNoExt(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t *src) FKS_NOEXCEPT;		///< �g���q���O�������O���擾.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetBaseNameNoExt(wchar_t d[], FKS_FNAME_SIZE l, const wchar_t *s) FKS_NOEXCEPT;	///< �f�B���N�g���Ɗg���q���O�������O���擾.

FKS_LIB_DECL (wchar_t*) 	fks_fnameSetExt(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src, const wchar_t *ext) FKS_NOEXCEPT; 		///< �g���q���Aext �ɕύX����.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSetDefaultExt(wchar_t dst[], FKS_FNAME_SIZE sz, const wchar_t* src, const wchar_t *ext) FKS_NOEXCEPT;	///< �g���q���Ȃ���΁Aext ��ǉ�����.
FKS_LIB_DECL (wchar_t*) 	fks_fnameJoin(wchar_t dst[],FKS_FNAME_SIZE sz,const wchar_t *dir,const wchar_t *nm) FKS_NOEXCEPT;				///< �f�B���N�g�����ƃt�@�C�����̘A��.

FKS_LIB_DECL (wchar_t*) 	fks_fnameGetDir(wchar_t dir[], FKS_FNAME_SIZE sz, const wchar_t *nm) FKS_NOEXCEPT;		///< �f�B���N�g�����̎擾.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetDrive(wchar_t drv[], FKS_FNAME_SIZE sz, const wchar_t *nm) FKS_NOEXCEPT; 	///< �h���C�u�����擾.
FKS_LIB_DECL (wchar_t*) 	fks_fnameGetDriveRoot(wchar_t dr[],FKS_FNAME_SIZE sz,const wchar_t *nm) FKS_NOEXCEPT;	///< �h���C�u�����擾.

FKS_LIB_DECL (wchar_t*) 	fks_fnameCheckPosSep(FKS_C_CONST wchar_t* dir, int pos) FKS_NOEXCEPT;				///< pos�̈ʒu��\��/������΂��̃A�h���X���Ȃ����NULL��Ԃ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameCheckLastSep(FKS_C_CONST wchar_t* dir) FKS_NOEXCEPT;						///< �Ō��\��/������΂��̃A�h���X���Ȃ����NULL��Ԃ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameDelLastSep(wchar_t dir[]) FKS_NOEXCEPT;									///< ������̍Ō�� \ �� / ������΍폜.
FKS_LIB_DECL (wchar_t*) 	fks_fnameAddSep(wchar_t dst[], FKS_FNAME_SIZE sz) FKS_NOEXCEPT;						///< ������̍Ō�� \ / ���Ȃ���Βǉ�.

FKS_LIB_DECL (wchar_t*) 	fks_fnameToUpper(wchar_t filename[]) FKS_NOEXCEPT;									///< �S�p�Q�o�C�g�ڂ��l������ strupr.
FKS_LIB_DECL (wchar_t*) 	fks_fnameToLower(wchar_t filename[]) FKS_NOEXCEPT;									///< �S�p�Q�o�C�g�ڂ��l������ strlwr.
FKS_LIB_DECL (wchar_t*) 	fks_fnameBackslashToSlash(wchar_t filePath[]) FKS_NOEXCEPT; 						///< filePath���� \ �� / �ɒu��.
FKS_LIB_DECL (wchar_t*) 	fks_fnameSlashToBackslash(wchar_t filePath[]) FKS_NOEXCEPT; 						///< filePath���� / �� \ �ɒu��.

FKS_LIB_DECL (wchar_t*) 	fks_fnameFullpath  (wchar_t fullpath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< �t���p�X����. os�ˑ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameFullpathSL(wchar_t fullpath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< �t���p�X����. / ���.
FKS_LIB_DECL (wchar_t*) 	fks_fnameFullpathBS(wchar_t fullpath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< �t���p�X����. \ ���.
FKS_LIB_DECL (wchar_t*) 	fks_fnameRelativePath  (wchar_t relPath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< ���΃p�X����. os�ˑ�.
FKS_LIB_DECL (wchar_t*) 	fks_fnameRelativePathSL(wchar_t relPath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< ���΃p�X����. / ���.
FKS_LIB_DECL (wchar_t*) 	fks_fnameRelativePathBS(wchar_t relPath[], FKS_FNAME_SIZE sz, const wchar_t* path, const wchar_t* curDir) FKS_NOEXCEPT;	///< ���΃p�X����. \ ���.

FKS_LIB_DECL (int)			fks_fnameCmp(const wchar_t* l, const wchar_t* r) FKS_NOEXCEPT; 							///< �t�@�C�����̑召��r.
FKS_LIB_DECL (int)			fks_fnameNCmp(const wchar_t* l, const wchar_t* r, FKS_FNAME_SIZE n) FKS_NOEXCEPT;		///< �t�@�C������n�����召��r.
FKS_LIB_DECL (int)			fks_fnameDigitCmp(const wchar_t* l, const wchar_t* r) FKS_NOEXCEPT;						///< ���Ⴂ�̐����𐔒l�Ƃ��đ召��r.
//FKS_LIB_DECL (int) 		fks_fnameNDigitCmp(const wchar_t* l,const wchar_t* r,FKS_FNAME_SIZE n) FKS_NOEXCEPT;	///< ���Ⴂ�̐����𐔒l�Ƃ���n�����召��r.
FKS_LIB_DECL (wchar_t*) 	fks_fnameEquLong(FKS_C_CONST wchar_t* fname, const wchar_t* baseName) FKS_NOEXCEPT;		///< fname��baseName�Ŏn�܂��Ă��邩�ۂ�.
FKS_LIB_DECL (int)			fks_fnameMatchWildCard(const wchar_t* pattern, const wchar_t* str) FKS_NOEXCEPT;		///< ���C���h�J�[�h����(*?)���r. �}�b�`������^.

/// �R�}���h���C��������A;��؂�̕����̃p�X�w�肩��A�P�v�f�擾.
FKS_LIB_DECL (wchar_t*) 	fks_fnameScanArgStr(wchar_t arg[],FKS_FNAME_SIZE sz,const wchar_t *str, unsigned sepChr) FKS_NOEXCEPT;
#endif

#if defined __cplusplus
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameBaseName(const wchar_t *p) FKS_NOEXCEPT							{ return fks_fnameBaseName((wchar_t*)p); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameExt(const wchar_t *name) FKS_NOEXCEPT								{ return fks_fnameExt((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameSkipDrive(const wchar_t *name) FKS_NOEXCEPT						{ return fks_fnameSkipDrive((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameSkipDriveRoot(const wchar_t *name) FKS_NOEXCEPT					{ return fks_fnameSkipDriveRoot((wchar_t*)name); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameCheckPosSep(const wchar_t* dir, int pos) FKS_NOEXCEPT				{ return fks_fnameCheckPosSep((wchar_t*)dir,pos); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameCheckLastSep(const wchar_t* dir) FKS_NOEXCEPT						{ return fks_fnameCheckLastSep((wchar_t*)dir); }
FKS_INL_LIB_DECL (const wchar_t*)	fks_fnameEquLong(const wchar_t* fname, const wchar_t* baseName) FKS_NOEXCEPT	{ return fks_fnameEquLong((wchar_t*)fname, baseName); }
FKS_INL_LIB_DECL (wchar_t*)			fks_fnameScanArgStr(wchar_t arg[],FKS_FNAME_SIZE sz,const wchar_t *str) FKS_NOEXCEPT { return fks_fnameScanArgStr(arg,sz,str, ' ');	}
#endif

#endif		// FKS_FNAME_H_INCLUDED.
