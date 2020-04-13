/**
 *  @file   fks_mem_mac.h
 *  @brief  macro for memory access
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */

#ifndef FKS_MEM_MAC_H_INCLUDED
#define FKS_MEM_MAC_H_INCLUDED

#include <fks/fks_common.h>
#include <stdint.h>

/// 8bit数二つを上下につなげて16ビット数にする
#define FKS_BB(a,b)         ((((uint8_t)(a))<<8)|(uint8_t)(b))

/// 16bit数二つを上下につなげて32ビット数にする
#define FKS_WW(a,b)         (((uint32_t)((uint16_t)(a))<<16)|(uint16_t)(b))

/// 32bit数二つを上下につなげて64ビット数にする
#define FKS_DD(a,b)         ((((uint64_t)(a))<<32)|(uint32_t)(b))

/// 8bit数4つを上位から順につなげて32ビット数にする
#define FKS_BBBB(a,b,c,d)   (((uint32_t)((uint8_t)(a))<<24)|(((uint8_t)(b))<<16)|(((uint8_t)(c))<<8)|((uint8_t)(d)))

#ifdef FKS_BIG_ENDIAN
#define FKS_CC(a,b)         FKS_BB(a,b)         ///< 2バイト文字列をint16として扱った場合の、定数比較用マクロ
#define FKS_CCCC(a,b,c,d)   FKS_BBBB(a,b,c,d)   ///< 4バイト文字列をint32として扱った場合の、定数比較用マクロ
#else   // LITTLE_ENDIAN
#define FKS_CC(a,b)         FKS_BB(b,a)         ///< 2バイト文字列をint16として扱った場合の、定数比較用マクロ
#define FKS_CCCC(a,b,c,d)   FKS_BBBB(d,c,b,a)   ///< 4バイト文字列をint32として扱った場合の、定数比較用マクロ
#endif

#define FKS_GLB(a)          ((uint8_t)(a))                      ///< aを int16型として下位バイトの値を取得
#define FKS_GHB(a)          ((uint8_t)(((uint16_t)(a))>>8))     ///< aを int16型として上位バイトの値を取得
#define FKS_GLLB(a)         ((uint8_t)(a))                      ///< aを int32型として最下位バイトの値を取得
#define FKS_GLHB(a)         ((uint8_t)(((uint16_t)(a))>>8))     ///< aを int32型として下2バイト目の値を取得
#define FKS_GHLB(a)         ((uint8_t)(((uint32_t)(a))>>16))    ///< aを int32型として下3バイト目の値を取得
#define FKS_GHHB(a)         ((uint8_t)(((uint32_t)(a))>>24))    ///< aを int32型として下4バイト目の値を取得
#define FKS_GLW(a)          ((uint16_t)(a))                     ///< int32型としての a の下16ビットの値を取得
#define FKS_GHW(a)          ((uint16_t)(((uint32_t)(a))>>16))   ///< int32型としての a の上16ビットの値を取得


#define FKS_PEEKB(a)        (*(const uint8_t *)(a))         ///< アドレス a から 1バイト読み込む
#define FKS_POKEB(a,b)      (*(uint8_t *)(a) = (b))         ///< アドレス a に 1バイト書き込む

//  インテル/リトルエンディアン・データにアクセスする場合
#if !defined FKS_BIG_ENDIAN && defined FKS_ENABLE_BYTE_ALIGN    // X86 は、アライメントを気にする必要がないので直接アクセス
#define FKS_PEEKiW(a)       (*(const uint16_t *)(a))
#define FKS_PEEKiD(a)       (*(const uint32_t *)(a))
#define FKS_PEEKiB3(s)      ((*(const uint16_t*)(s)) | ((*(const uint8_t*)((s)+2))<<16))
#define FKS_POKEiW(a,b)     (*(uint16_t *)(a) = (b))
#define FKS_POKEiD(a,b)     (*(uint32_t *)(a) = (b))
#define FKS_POKEiB3(a,b)    (FKS_POKEB((a)+2, FKS_GHLB(b)), FKS_POKEiW(a,FKS_GLW(b)))
#else                   //アライメント対策で、1バイトづつアクセス
#define FKS_PEEKiW(a)       ( FKS_PEEKB(a) | (FKS_PEEKB((const char *)(a)+1)<< 8) )
#define FKS_PEEKiD(a)       ( FKS_PEEKiW(a) | (FKS_PEEKiW((const char *)(a)+2) << 16) )
#define FKS_POKEiW(a,b)     (FKS_POKEB((a),FKS_GLB(b)), FKS_POKEB((char *)(a)+1,FKS_GHB(b)))
#define FKS_POKEiD(a,b)     (FKS_POKEiW((a),FKS_GLW(b)), FKS_POKEiW((char *)(a)+2,FKS_GHW(b)))
#define FKS_PEEKiB3(s)      FKS_BBBB(0, ((const uint8_t*)(s))[2], ((const uint8_t*)(s))[1], ((const uint8_t*)(s))[0])
#define FKS_POKEiB3(a,b)    (FKS_POKEB((a)+2, FKS_GHLB(b)), FKS_POKEB((a)+1,FKS_GLHB(b)), FKS_POKEB((a), FKS_GLLB(b)))
#endif

//      モトローラ/ビッグエンディアン・データにアクセスする場合
#if defined FKS_BIG_ENDIAN && defined FKS_ENABLE_BYTE_ALIGN
#define FKS_PEEKmW(a)       (*(const uint16_t *)(a))
#define FKS_PEEKmD(a)       (*(const uint32_t *)(a))
#define FKS_PEEKmB3(s)      ((*(const uint16_t*)(s) << 8) | ((*(const uint8_t*)((s)+2))))
#define FKS_POKEmW(a,b)     (*(uint16_t *)(a) = (b))
#define FKS_POKEmD(a,b)     (*(uint32_t *)(a) = (b))
#define FKS_POKEmB3(a,b)    (FKS_POKEmW((a)+0, (b)>>8), FKS_POKEiB((a)+2,FKS_GLLB(b)))
#else
#define FKS_PEEKmW(a)       ( (FKS_PEEKB(a)<<8) | FKS_PEEKB((const char *)(a)+1) )
#define FKS_PEEKmD(a)       ( (FKS_PEEKmW(a)<<16) | FKS_PEEKmW((const char *)(a)+2) )
#define FKS_PEEKmB3(s)      FKS_BBBB(0, ((const uint8_t*)(s))[0], ((const uint8_t*)(s))[1], ((const uint8_t*)(s))[2])
#define FKS_POKEmW(a,b)     (FKS_POKEB((a),FKS_GHB(b)), FKS_POKEB((char *)(a)+1,FKS_GLB(b)))
#define FKS_POKEmD(a,b)     (FKS_POKEmW((a),FKS_GHW(b)), FKS_POKEmW((char *)(a)+2,FKS_GLW(b)))
#define FKS_POKEmB3(a,b)    (FKS_POKEB((a)+0, FKS_GHLB(b)), FKS_POKEB((a)+1,FKS_GLHB(b)), FKS_POKEB((a)+2, FKS_GLLB(b)))
#endif

//      そのCPUの、デフォルトのエンディアンでアクセスする場合(バイト単位のアドレス可能)
#ifdef BIG_ENDIAN
#define FKS_PEEKW(a)        FKS_PEEKmW(a)
#define FKS_PEEKB3(a)       FKS_PEEKmB3(a)
#define FKS_PEEKD(a)        FKS_PEEKmD(a)
#define FKS_POKEW(a,b)      FKS_POKEmW(a,b)
#define FKS_POKEB3(a,b)     FKS_POKEmB3(a,b)
#define FKS_POKED(a,b)      FKS_POKEmD(a,b)
#else /* LITTLE_ENDIAN */
#define FKS_PEEKW(a)        FKS_PEEKiW(a)
#define FKS_PEEKB3(a)       FKS_PEEKiB3(a)
#define FKS_PEEKD(a)        FKS_PEEKiD(a)
#define FKS_POKEW(a,b)      FKS_POKEiW(a,b)
#define FKS_POKEB3(a,b)     FKS_POKEiB3(a,b)
#define FKS_POKED(a,b)      FKS_POKEiD(a,b)
#endif

// -----------------------------------------------
#define FKS_STREND(p)           ((p)+ strlen(p))
/// ty型の0を アドレス dから sz/sizeof(ty)個書き込む
#define FKS_MEMCLR_TY(d, sz, ty) do {ty *d__ = (ty *)(d); size_t c___ = (sz)/sizeof(ty); do {*d___++ = 0;} while(--c___);} while(0)
/// ty型のsを アドレス dから sz/sizeof(ty)個書き込む
#define FKS_MEMSET_TY(d, s, sz, ty) do {ty *d___ = (ty *)(d); ty s___  = (ty)(s);   size_t c___ = (sz)/sizeof(ty); do {*d___++ = s___;} while(--c___);} while(0)
/// ty型のポインタとしてsからdへ sz/sizeof(ty)個コピーする
#define FKS_MEMCPY_TY_TR(d, s, sz, ty, EXPR) do {ty *d___ = (ty *)(d); ty *s___ = (ty *)(s); size_t c___ = ((size_t)(sz)/sizeof(ty)); do {(EXPR);} while(--c___);} while(0)
#define FKS_MEMCPY_TY(d, s, sz, ty) MEMCPY_TY_TR(d,s,sz,ty, (*d___++ = *s___++))
/// ty型のポインタとしてsからdへ後ろから sz/sizeof(ty)個コピーする
#define FKS_MEMRCPY_TY(d,s, sz, ty) do {ty *d___ = (ty *)(d); ty *s___ = (ty *)(s); size_t c___ = ((size_t)(sz)/sizeof(ty)); while (c___-- > 0) {d___[c___] = s___[c___]; } } while (0)

#endif  // MEM_MAC_H
