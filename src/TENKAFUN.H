#ifndef _TENKAFUN_H_
#define _TENKAFUN_H_

#ifndef OS9
/* MS-DOS */
typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef long double    ldouble;
typedef void far *VOID_FP;
typedef byte far *BYTE_FP;
typedef word far *WORD_FP;
#define REGI
#define DIRE
#define __(x)    x
#define C_DIRSP  '\\'

#define	MAKE_FP(s,o)  ((void far *)(((dword)(s)<<16) | ((o)&0xffff)))
#define PTR_OFF(p)    ((word) (p))
#define PTR_SEG(p)    ((word)((dword)(void far *)(p) >> 16))
#define htoi(s)       ((int)strtoul((s),(char **)0,16))

#else
/* os9/09 MW-C*/
typedef char     byte;
typedef unsigned word;
typedef long     dword;
#define REGI     register
#define DIRE     direct
#define __(x)    ()
#define C_DIRSP  '/'

#endif

/*  */
#define global
#define TRACE       fprintf(stderr,"%s (%d)\n",__FILE__,__LINE__)
#define TRACES(s)   fprintf(stderr,"%s (%d) :%s\n",__FILE__,__LINE__,s)
#define C_CR        0x0d
#define C_LF        0x0a
#define eputs(s)    fputs((s),stderr)
#define ep_exit(s)  (fputs((s),stderr),exit(1))

#ifndef toascii /* ctype.h ‚ªinclude‚³‚ê‚Ä‚¢‚È‚¢‚Æ‚« */
  #define toupper(c)  ( ((c) >= 'a' && (c) <= 'z') ? (c) - 0x20 : (c) )
  #define isdigit(c)  ((c) >= '0' && (c) <= '9')
#endif

#endif
