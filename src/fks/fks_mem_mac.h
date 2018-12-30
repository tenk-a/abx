/**
 *	@file	fks_mem_mac.h
 *	@brief	macro for memory access
 *	@author	Masashi Kitamura (tenka@6809.net)
 */

#ifndef FKS_MEM_MAC_H_INCLUDED
#define FKS_MEM_MAC_H_INCLUDED

#include <fks_common.h>
#include <stdint.h>

/// 8bit������㉺�ɂȂ���16�r�b�g���ɂ���
#define FKS_BB(a,b) 		((((uint8_t)(a))<<8)|(uint8_t)(b))

/// 16bit������㉺�ɂȂ���32�r�b�g���ɂ���
#define FKS_WW(a,b) 		(((uint32_t)((uint16_t)(a))<<16)|(uint16_t)(b))

/// 32bit������㉺�ɂȂ���64�r�b�g���ɂ���
#define FKS_DD(a,b)			((((uint64_t)(a))<<32)|(uint32_t)(b))

/// 8bit��4����ʂ��珇�ɂȂ���32�r�b�g���ɂ���
#define FKS_BBBB(a,b,c,d)	(((uint32_t)((uint8_t)(a))<<24)|(((uint8_t)(b))<<16)|(((uint8_t)(c))<<8)|((uint8_t)(d)))

#ifdef BIG_ENDIAN
#define FKS_CC(a,b)			FKS_BB(a,b)			///< 2�o�C�g�������int16�Ƃ��Ĉ������ꍇ�́A�萔��r�p�}�N��
#define FKS_CCCC(a,b,c,d)	FKS_BBBB(a,b,c,d)	///< 4�o�C�g�������int32�Ƃ��Ĉ������ꍇ�́A�萔��r�p�}�N��
#else	// LITTLE_ENDIAN
#define FKS_CC(a,b)			FKS_BB(b,a)			///< 2�o�C�g�������int16�Ƃ��Ĉ������ꍇ�́A�萔��r�p�}�N��
#define FKS_CCCC(a,b,c,d)	FKS_BBBB(d,c,b,a)	///< 4�o�C�g�������int32�Ƃ��Ĉ������ꍇ�́A�萔��r�p�}�N��
#endif

#define FKS_GLB(a)			((uint8_t)(a))						///< a�� int16�^�Ƃ��ĉ��ʃo�C�g�̒l���擾
#define FKS_GHB(a)			((uint8_t)(((uint16_t)(a))>>8))		///< a�� int16�^�Ƃ��ď�ʃo�C�g�̒l���擾
#define FKS_GLLB(a) 		((uint8_t)(a))						///< a�� int32�^�Ƃ��čŉ��ʃo�C�g�̒l���擾
#define FKS_GLHB(a) 		((uint8_t)(((uint16_t)(a))>>8))		///< a�� int32�^�Ƃ��ĉ�2�o�C�g�ڂ̒l���擾
#define FKS_GHLB(a) 		((uint8_t)(((uint32_t)(a))>>16))	///< a�� int32�^�Ƃ��ĉ�3�o�C�g�ڂ̒l���擾
#define FKS_GHHB(a) 		((uint8_t)(((uint32_t)(a))>>24))	///< a�� int32�^�Ƃ��ĉ�4�o�C�g�ڂ̒l���擾
#define FKS_GLW(a)			((uint16_t)(a))						///< int32�^�Ƃ��Ă� a �̉�16�r�b�g�̒l���擾
#define FKS_GHW(a)			((uint16_t)(((uint32_t)(a))>>16))	///< int32�^�Ƃ��Ă� a �̏�16�r�b�g�̒l���擾


#define	FKS_PEEKB(a)		(*(const uint8_t *)(a))			///< �A�h���X a ���� 1�o�C�g�ǂݍ���
#define	FKS_POKEB(a,b)		(*(uint8_t *)(a) = (b))			///< �A�h���X a �� 1�o�C�g��������

//	�C���e��/���g���G���f�B�A���E�f�[�^�ɃA�N�Z�X����ꍇ
#if !defined FKS_BIG_ENDIAN && defined FKS_ENABLE_BYTE_ALIGN 	// X86 �́A�A���C�����g���C�ɂ���K�v���Ȃ��̂Œ��ڃA�N�Z�X
#define	FKS_PEEKiW(a)		(*(const uint16_t *)(a))
#define	FKS_PEEKiD(a)		(*(const uint32_t *)(a))
#define FKS_PEEKiB3(s)		((*(const uint16_t*)(s)) | ((*(const uint8_t*)((s)+2))<<16))
#define	FKS_POKEiW(a,b)		(*(uint16_t *)(a) = (b))
#define	FKS_POKEiD(a,b)		(*(uint32_t *)(a) = (b))
#define FKS_POKEiB3(a,b)	(FKS_POKEB((a)+2, FKS_GHLB(b)), FKS_POKEiW(a,FKS_GLW(b)))
#else					//�A���C�����g�΍�ŁA1�o�C�g�ÂA�N�Z�X
#define	FKS_PEEKiW(a)		( FKS_PEEKB(a) | (FKS_PEEKB((const char *)(a)+1)<< 8) )
#define	FKS_PEEKiD(a)		( FKS_PEEKiW(a) | (FKS_PEEKiW((const char *)(a)+2) << 16) )
#define	FKS_POKEiW(a,b)		(FKS_POKEB((a),FKS_GLB(b)), FKS_POKEB((char *)(a)+1,FKS_GHB(b)))
#define	FKS_POKEiD(a,b)		(FKS_POKEiW((a),FKS_GLW(b)), FKS_POKEiW((char *)(a)+2,FKS_GHW(b)))
#define FKS_PEEKiB3(s)		FKS_BBBB(0, ((const uint8_t*)(s))[2], ((const uint8_t*)(s))[1], ((const uint8_t*)(s))[0])
#define FKS_POKEiB3(a,b)	(FKS_POKEB((a)+2, FKS_GHLB(b)), FKS_POKEB((a)+1,FKS_GLHB(b)), FKS_POKEB((a), FKS_GLLB(b)))
#endif

//		���g���[��/�r�b�O�G���f�B�A���E�f�[�^�ɃA�N�Z�X����ꍇ
#if defined FKS_BIG_ENDIAN && defined FKS_ENABLE_BYTE_ALIGN
#define	FKS_PEEKmW(a)		(*(const uint16_t *)(a))
#define	FKS_PEEKmD(a)		(*(const uint32_t *)(a))
#define FKS_PEEKmB3(s)		((*(const uint16_t*)(s) << 8) | ((*(const uint8_t*)((s)+2))))
#define	FKS_POKEmW(a,b)		(*(uint16_t *)(a) = (b))
#define	FKS_POKEmD(a,b)		(*(uint32_t *)(a) = (b))
#define FKS_POKEmB3(a,b)	(FKS_POKEmW((a)+0, (b)>>8), FKS_POKEiB((a)+2,FKS_GLLB(b)))
#else
#define	FKS_PEEKmW(a)		( (FKS_PEEKB(a)<<8) | FKS_PEEKB((const char *)(a)+1) )
#define	FKS_PEEKmD(a)		( (FKS_PEEKmW(a)<<16) | FKS_PEEKmW((const char *)(a)+2) )
#define FKS_PEEKmB3(s)		FKS_BBBB(0, ((const uint8_t*)(s))[0], ((const uint8_t*)(s))[1], ((const uint8_t*)(s))[2])
#define	FKS_POKEmW(a,b)		(FKS_POKEB((a),FKS_GHB(b)), FKS_POKEB((char *)(a)+1,FKS_GLB(b)))
#define	FKS_POKEmD(a,b)		(FKS_POKEmW((a),FKS_GHW(b)), FKS_POKEmW((char *)(a)+2,FKS_GLW(b)))
#define FKS_POKEmB3(a,b)	(FKS_POKEB((a)+0, FKS_GHLB(b)), FKS_POKEB((a)+1,FKS_GLHB(b)), FKS_POKEB((a)+2, FKS_GLLB(b)))
#endif

//		����CPU�́A�f�t�H���g�̃G���f�B�A���ŃA�N�Z�X����ꍇ(�o�C�g�P�ʂ̃A�h���X�\)
#ifdef BIG_ENDIAN
#define	FKS_PEEKW(a)		FKS_PEEKmW(a)
#define	FKS_PEEKB3(a)		FKS_PEEKmB3(a)
#define	FKS_PEEKD(a)		FKS_PEEKmD(a)
#define	FKS_POKEW(a,b)		FKS_POKEmW(a,b)
#define	FKS_POKEB3(a,b)		FKS_POKEmB3(a,b)
#define	FKS_POKED(a,b)		FKS_POKEmD(a,b)
#else /* LITTLE_ENDIAN */
#define	FKS_PEEKW(a)		FKS_PEEKiW(a)
#define	FKS_PEEKB3(a)		FKS_PEEKiB3(a)
#define	FKS_PEEKD(a)		FKS_PEEKiD(a)
#define	FKS_POKEW(a,b)		FKS_POKEiW(a,b)
#define	FKS_POKEB3(a,b)		FKS_POKEiB3(a,b)
#define	FKS_POKED(a,b)		FKS_POKEiD(a,b)
#endif

// -----------------------------------------------
#define FKS_STREND(p)			((p)+ strlen(p))
/// ty�^��0�� �A�h���X d���� sz/sizeof(ty)��������
#define FKS_MEMCLR_TY(d, sz, ty) do {ty *d__ = (ty *)(d); size_t c___ = (sz)/sizeof(ty); do {*d___++ = 0;} while(--c___);} while(0)
/// ty�^��s�� �A�h���X d���� sz/sizeof(ty)��������
#define FKS_MEMSET_TY(d, s, sz, ty) do {ty *d___ = (ty *)(d); ty s___  = (ty)(s);   size_t c___ = (sz)/sizeof(ty); do {*d___++ = s___;} while(--c___);} while(0)
/// ty�^�̃|�C���^�Ƃ���s����d�� sz/sizeof(ty)�R�s�[����
#define FKS_MEMCPY_TY_TR(d, s, sz, ty, EXPR) do {ty *d___ = (ty *)(d); ty *s___ = (ty *)(s); size_t c___ = ((size_t)(sz)/sizeof(ty)); do {(EXPR);} while(--c___);} while(0)
#define FKS_MEMCPY_TY(d, s, sz, ty) MEMCPY_TY_TR(d,s,sz,ty, (*d___++ = *s___++))
/// ty�^�̃|�C���^�Ƃ���s����d�֌�납�� sz/sizeof(ty)�R�s�[����
#define FKS_MEMRCPY_TY(d,s, sz, ty) do {ty *d___ = (ty *)(d); ty *s___ = (ty *)(s); size_t c___ = ((size_t)(sz)/sizeof(ty)); while (c___-- > 0) {d___[c___] = s___[c___]; } } while (0)

#endif	// MEM_MAC_H
