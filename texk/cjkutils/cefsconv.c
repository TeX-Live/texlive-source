#define banner  \
"cefsconv (CJK ver. 4.8.3)" \

/*2:*/
#line 111 "./cjkutils-4.8.3/CEFconv/cefsconv.w"

#include <ctype.h> 
#include <stdio.h> 
#include <stdlib.h> 


int main(int argc,char*argv[])
{int ch,i;
unsigned char in[16];
unsigned char out[32];
unsigned char*inp,*outp;

fprintf(stdout,"\\def\\CNSpreproc{%s}",banner);

ch= fgetc(stdin);

while(!feof(stdin))
{if((ch>=0x81&&ch<=0x9F)||(ch>=0xE0&&ch<=0xEF))
{fprintf(stdout,"\177%c\177",ch);

ch= fgetc(stdin);
if(!feof(stdin))
fprintf(stdout,"%d\177",ch);
}
else if(ch=='&')

{inp= in;
outp= out;
*inp= ch;
*(++inp)= fgetc(stdin);

if(*inp=='C'&&!feof(stdin))
{*(++inp)= fgetc(stdin);
if(*inp=='0'&&!feof(stdin))
{*(outp++)= 'B';
*(outp++)= 'g';
*(outp++)= '5';
}
else if(*inp>='1'&&*inp<='7'&&!feof(stdin))
{*(outp++)= 'C';
*(outp++)= 'N';
*(outp++)= 'S';
*(outp++)= *inp;
}
else if((*inp=='X'||*inp=='Y')&&!feof(stdin))
{*(outp++)= 'C';
*(outp++)= 'E';
*(outp++)= 'F';
*(outp++)= *inp;
}
else
goto no_macro;
}
else if(*inp=='U'&&!feof(stdin))
{*(outp++)= 'U';
*(outp++)= 'T';
*(outp++)= 'F';
*(outp++)= '8';
}
else
goto no_macro;

*(++inp)= fgetc(stdin);
if(*inp!='-'||feof(stdin))
goto no_macro;

*(outp++)= '\177';
*(outp++)= '\177';
*(outp++)= '\"';
*(outp++)= '0';

*(++inp)= fgetc(stdin);
if(isxdigit(*inp)&&*inp<0x80&&!feof(stdin))
*(outp++)= toupper(*inp);
else
goto no_macro;

*(++inp)= fgetc(stdin);
if(isxdigit(*inp)&&*inp<0x80&&!feof(stdin))
*(outp++)= toupper(*inp);
else
goto no_macro;

*(outp++)= '\177';
*(outp++)= '\"';
*(outp++)= '0';

*(++inp)= fgetc(stdin);
if(isxdigit(*inp)&&*inp<0x80&&!feof(stdin))
*(outp++)= toupper(*inp);
else
goto no_macro;

*(++inp)= fgetc(stdin);
if(isxdigit(*inp)&&*inp<0x80&&!feof(stdin))
*(outp++)= toupper(*inp);
else
goto no_macro;

*(outp++)= '\177';
*outp= '\0';

*(++inp)= fgetc(stdin);
if(*inp!=';'||feof(stdin))
goto no_macro;

outp= out;
fprintf(stdout,"\17772\177");
while(*outp)
fputc(*(outp++),stdout);

ch= fgetc(stdin);
continue;

no_macro:
ch= *inp;
i= inp-in;
inp= in;
while(i--)
fputc(*(inp++),stdout);
continue;
}
else
fputc(ch,stdout);

ch= fgetc(stdin);
}
exit(EXIT_SUCCESS);
return 0;
}/*:2*/
