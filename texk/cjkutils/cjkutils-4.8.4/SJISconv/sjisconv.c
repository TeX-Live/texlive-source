#define banner  \
"sjisconv (CJK ver. 4.8.4)" \

/*2:*/
#line 96 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/SJISconv/sjisconv.w"

#include <stdio.h> 
#include <stdlib.h> 


int main(int argc,char*argv[])
{int ch;

fprintf(stdout,"\\def\\CJKpreproc{%s}",banner);

ch= fgetc(stdin);

while(!feof(stdin))
{if((ch>=0x81&&ch<=0x9F)||(ch>=0xE0&&ch<=0xEF))
{fprintf(stdout,"\177%c\177",ch);

ch= fgetc(stdin);
if(!feof(stdin))
fprintf(stdout,"%d\177",ch);
}
else
fputc(ch,stdout);

ch= fgetc(stdin);
}
exit(EXIT_SUCCESS);
return 0;
}/*:2*/
