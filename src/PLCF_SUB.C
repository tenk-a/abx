#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tenkafun.h"
#include "plcf.h"

#if 0
void
unlink_e(nam)
     byte *nam;
{
     if (unlink(nam)) {
          fprintf(stderr,"\n%s ‚ªíœ‚Å‚«‚Ü‚¹‚ñ\n",nam);
          exit(errno);
     }
}
#endif

#if 0
void
rename_e(old_nam,new_nam)
     byte *old_nam;
     byte *new_nam;
{
     if (rename(old_nam,new_nam)) {
          fprintf(stderr,"%s ‚ğ %s ‚É•Ï–¼‚Å‚«‚Ü‚¹‚ñ\n",old_nam,new_nam);
          exit(errno);
     }
}
#endif

FILE *
fopen_e(name,mode)
     byte *name;
     byte *mode;
{
     FILE *fp;

     fp = fopen(name,mode);
     if (fp == NULL) {
          fprintf(stderr,"\n %s ‚ğµ°Ìßİ‚Å‚«‚Ü‚¹‚ñ\n",name);
          exit(errno);
     }
     return fp;
}

void
fputs_e(buf,fp)
     byte *buf;
     FILE *fp;
{
     int k;

     k = fputs(buf,fp);
     if (k < 0)
          ep_exit("o—Í‚ÅƒGƒ‰[‚ª”­¶‚µ‚½\n");
}

#if 0
void
chg_ext(onam,inam,ind)
     byte *onam;
     byte *inam;
     byte *ind;
{
     byte *p;

#ifndef OS9
     strncpy(onam,inam,80);
     if ((p = strrchr(onam,'.')) != NULL)
          ++p;
     else {
          p = onam;
          while(*(p++))
               ;
          *(p - 1) = '.';
     }
     if (ind != NULL)
          strcpy(p,ind);
     else
          *(p - 1) = '\0';
#else
     sprintf(onam,"%s.%s",inam,ind);
#endif
}
#endif
