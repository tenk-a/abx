#include <stdio.h>
#include <string.h>
typedef unsigned char byte;

int
main(int argc,byte *argv[])
{
     byte *p;
     static byte buf[4000];

     buf[0] = '\0';
     if (argc < 3) {
          fputs("use:tx <cmd> [cmd_opts] -- <file1> [<file2>]...\n",stdout);
          return 1;
     }
     p = buf;
     while(--argc > 0) {
          ++argv;
          if (strcmp(*argv,"--") == 0)
               break;
          p = stpcpy(p,*argv);
          *(p++) = ' ';
          *(p) = '\0';
     }
     while (--argc > 0) {
          ++argv;
          fputs(buf,stdout);
          fputs(*argv,stdout);
          fputs("\n",stdout);
     }
     return 0;
}
