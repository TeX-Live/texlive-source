#define BANNER "This is program vlna, version 1.2, (c) 1995, 2002 Petr Olsak\n" \

#define OK 0
#define WARNING 1
#define IO_ERR 2
#define BAD_OPTIONS 3
#define BAD_PROGRAM 4
#define MAXLEN 120
#define ONE 1
#define ANY 2
#define ONE_NOT -1
#define ANY_NOT -2 \

#define MAXPATT 200
#define MAXBUFF 500
#define BUFI 300
#define FOUND -1
#define NOFOUND -2
#define TEXTMODE 0
#define MATHMODE 1
#define DISPLAYMODE 2
#define VERBMODE 3
/*1:*/
#line 19 "../vlna-1.3/vlna.w"

/*3:*/
#line 39 "../vlna-1.3/vlna.w"

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

/*:3*/
#line 20 "../vlna-1.3/vlna.w"

/*4:*/
#line 56 "../vlna-1.3/vlna.w"

char*prog_name;
int status;

/*:4*//*7:*/
#line 133 "../vlna-1.3/vlna.w"

int isfilter= 0,silent= 0,rmbackup= 0,nomath= 0,noverb= 0,web= 0,latex= 0;
char charsetdefault[]= "KkSsVvZzOoUuAI";
char*charset= charsetdefault;

/*:7*//*15:*/
#line 292 "../vlna-1.3/vlna.w"

typedef struct PATITEM{
char*str;
int flag;
struct PATITEM*next;
}PATITEM;
typedef struct PATTERN{
PATITEM*patt;
void(*proc)();
struct PATTERN*next;
}PATTERN;

/*:15*//*16:*/
#line 344 "../vlna-1.3/vlna.w"

PATITEM*lapi[MAXPATT];
PATTERN*lapt[MAXPATT];
PATTERN*listpatt,*normallist,*commentlist,*pt,*lastpt= NULL;
PATITEM*pi,*lastpi= NULL;
char c;
char buff[MAXBUFF];
int ind;

/*:16*//*20:*/
#line 418 "../vlna-1.3/vlna.w"

char strings[512];
int i;

/*:20*//*25:*/
#line 506 "../vlna-1.3/vlna.w"

char*filename;
long int numline,numchanges;
int mode;

/*:25*//*33:*/
#line 637 "../vlna-1.3/vlna.w"

char tblanks[]= " ~\t";
char blanks[]= " \t";
char blankscr[]= " \t\n";
char tblankscr[]= " ~\t\n";
char nochar[]= "%~\n";
char cr[]= "\n";
char prefixes[]= "[({~";
char dolar[]= "$";
char backslash[]= "\\";
char openbrace[]= "{";
char letters[]= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
PATTERN*vlnkalist,*mathlist,*parcheck,*verblist;

/*:33*/
#line 21 "../vlna-1.3/vlna.w"

/*6:*/
#line 110 "../vlna-1.3/vlna.w"

void printusage()
{
fprintf(stderr,
"usage: vlna [opt] [filenames]\n"
"  opt -f :  filter mode: file1 file2 ... file1->file2\n"
"                         file1       ... file1->stdout\n"
"                                     ... stdin->stdout\n"
"            nofilter: file1 [file2 file3 ...] all are in/out\n"
"      -s :  silent: no messages to stderr\n"
"      -r :  rmbackup: if nofilter, removes temporary files\n"
"      -v charset :  set of lettres to add tie, default: KkSsVvZzOoUuAI\n"
"      -m :  nomath: ignores math modes\n"
"      -n :  noverb: ignores verbatim modes\n"
"      -l :  LaTeX mode\n"
"      -w :  web mode\n");
}

/*:6*//*10:*/
#line 168 "../vlna-1.3/vlna.w"

void ioerr(f)
char*f;
{
fprintf(stderr,"%s: cannot open file %s\n",prog_name,f);
}

/*:10*//*17:*/
#line 357 "../vlna-1.3/vlna.w"

void*myalloc(size)
int size;
{
void*p;
p= malloc(size);
if(p==NULL)
{
fprintf(stderr,"%s, no memory, malloc failed\n",prog_name);
exit(BAD_PROGRAM);
}
return p;
}

/*:17*//*18:*/
#line 375 "../vlna-1.3/vlna.w"

PATTERN*setpattern(proc)
void(*proc)();
{
PATTERN*pp;
pp= myalloc(sizeof(PATTERN));
pp->proc= proc;
pp->next= NULL;
pp->patt= NULL;
if(lastpt!=NULL)lastpt->next= pp;
lastpt= pp;
lastpi= NULL;
return pp;
}

/*:18*//*19:*/
#line 394 "../vlna-1.3/vlna.w"

void setpi(str,flag)
char*str;
int flag;
{
PATITEM*p;
p= myalloc(sizeof(PATITEM));
p->str= str;p->flag= flag;
p->next= NULL;
if(lastpi==NULL)lastpt->patt= p;
else lastpi->next= p;
lastpi= p;
}

/*:19*//*22:*/
#line 429 "../vlna-1.3/vlna.w"

PATTERN*normalpattern(proc,str)
void(*proc)();
char*str;
{
PATTERN*pp;
int j= 0;
pp= setpattern(proc);
while(str[j]){
if(str[j]=='.'){
j++;
if(str[j]!='.'){
setpi(blankscr,ANY);
continue;
}
}
setpi(&strings[(unsigned char)str[j]*2],ONE);
j++;
}
return pp;
}

/*:22*//*23:*/
#line 467 "../vlna-1.3/vlna.w"

int match(p)
PATITEM*p;
{
int m;
if(strchr(p->str,c)!=NULL)m= 1;
else m= -1;
switch(m*p->flag){
case ANY:return 0;
case ONE:if(p->next==NULL)return FOUND;
return 1;
case ONE_NOT:return NOFOUND;
case ANY_NOT:/*24:*/
#line 486 "../vlna-1.3/vlna.w"

switch(m= match(p->next)){
case NOFOUND:return NOFOUND;
case FOUND:return FOUND;
default:return 1+m;
}

/*:24*/
#line 479 "../vlna-1.3/vlna.w"
;
}
return 0;
}

/*:23*//*36:*/
#line 670 "../vlna-1.3/vlna.w"

void vlnkain()
{
char p;
ind--;
p= buff[ind--];
while(strchr(blanks,buff[ind])!=NULL)ind--;
ind++;
buff[ind++]= '~';
buff[ind++]= p;
numchanges++;
}

/*:36*//*38:*/
#line 709 "../vlna-1.3/vlna.w"

void vlnkacr()
{
char p;
int i,j;
ind--;
p= buff[ind--];
while(strchr(blankscr,buff[ind])!=NULL)ind--;
i= ind;
while(i>=0&&(strchr(blankscr,buff[i])==NULL))i--;
j= i;
while(i>=0&&(strchr(blanks,buff[i])!=NULL))i--;
if(i>=0&&buff[i]=='\n')j= -1;
if(j>=0)buff[j]= '\n';
else numline--;
ind++;
buff[ind++]= '~';
buff[ind++]= p;
numchanges++;
}

/*:38*//*41:*/
#line 775 "../vlna-1.3/vlna.w"

void tielock()
{
c= 1;
}

/*:41*//*42:*/
#line 784 "../vlna-1.3/vlna.w"

void printwarning()
{
if(!silent)
fprintf(stderr,
"~!~ warning: text/math/verb mode mismatch,  file: %s,  line: %ld\n",
filename,numline-(c=='\n'?1:0));
status= WARNING;
}

/*:42*//*44:*/
#line 809 "../vlna-1.3/vlna.w"

void mathin()
{
if(mode!=TEXTMODE)printwarning();
mode= MATHMODE;
normallist= listpatt= mathlist;
}
void mathout()
{
if(mode!=MATHMODE)printwarning();
mode= TEXTMODE;
normallist= listpatt= vlnkalist;
}

/*:44*//*45:*/
#line 827 "../vlna-1.3/vlna.w"

void onedollar()
{
if(buff[ind-3]=='\\'||(buff[ind-3]=='$'&&buff[ind-4]!='\\'))return;
if(mode==DISPLAYMODE)printwarning();
else{
if(mode==TEXTMODE)mathin();
else mathout();
}
}

/*:45*//*47:*/
#line 846 "../vlna-1.3/vlna.w"

void checkmode()
{
if(mode!=TEXTMODE){
printwarning();
mode= TEXTMODE;
normallist= listpatt= vlnkalist;
}
}

/*:47*//*49:*/
#line 872 "../vlna-1.3/vlna.w"

void displayin()
{
if(mode!=TEXTMODE)printwarning();
mode= DISPLAYMODE;normallist= listpatt= parcheck;
}
void displayout()
{
if(mode!=DISPLAYMODE)printwarning();
mode= TEXTMODE;normallist= listpatt= vlnkalist;
}
void twodollars()
{
if(buff[ind-3]=='\\')return;
if(mode==DISPLAYMODE)displayout();
else displayin();
}

/*:49*//*51:*/
#line 921 "../vlna-1.3/vlna.w"

int prevmode;
PATTERN*prevlist,*verboutlist[4];
char verbchar[2];
void verbinchar()
{
prevmode= mode;
verbchar[0]= c;
c= 1;
listpatt= normallist= verboutlist[0];
prevlist= listpatt->next;
listpatt->next= NULL;
mode= VERBMODE;
}

/*:51*//*52:*/
#line 942 "../vlna-1.3/vlna.w"

void verbin()
{
int i;
i= 0;
prevmode= mode;
switch(c){
case't':i= 1;break;
case'm':i= 2;break;
case'<':;
case'd':i= 3;
if(buff[ind-3]=='@')return;
break;
}
listpatt= normallist= verboutlist[i];
prevlist= listpatt->next;
if(c!='<'&&c!='d')listpatt->next= NULL;
mode= VERBMODE;
}

/*:52*//*53:*/
#line 962 "../vlna-1.3/vlna.w"

void verbout()
{
if(mode!=VERBMODE)return;
if(web&&buff[ind-2]=='@'&&buff[ind-3]=='@')return;
mode= prevmode;
normallist->next= prevlist;
switch(mode){
case DISPLAYMODE:normallist= listpatt= parcheck;break;
case MATHMODE:normallist= listpatt= mathlist;break;
case TEXTMODE:normallist= listpatt= vlnkalist;break;
}
}

/*:53*//*55:*/
#line 984 "../vlna-1.3/vlna.w"

void tieoff()
{
normallist= NULL;
}
void tieon()
{
normallist= vlnkalist;
}

/*:55*/
#line 22 "../vlna-1.3/vlna.w"

/*26:*/
#line 515 "../vlna-1.3/vlna.w"

void tie(input,output)
FILE*input,*output;
{
int ap;
register int k,m,n;
int ic;
PATTERN*pp;
PATITEM*pi;

/*27:*/
#line 546 "../vlna-1.3/vlna.w"

for(k= 0;k<MAXPATT;k++)lapi[k]= NULL;
c= '\n';
buff[0]= mode= ap= 0;ind= 1;
numline= 1;numchanges= 0;
mode= TEXTMODE;

/*:27*//*35:*/
#line 667 "../vlna-1.3/vlna.w"

listpatt= normallist= vlnkalist;

/*:35*/
#line 525 "../vlna-1.3/vlna.w"
;

while(!feof(input)){
/*30:*/
#line 591 "../vlna-1.3/vlna.w"

pp= listpatt;
while(pp!=NULL){
switch(m= match(pp->patt)){
case FOUND:(*pp->proc)();
case NOFOUND:break;
default:/*31:*/
#line 606 "../vlna-1.3/vlna.w"

pi= pp->patt;
while(m--)pi= pi->next;
n= ap;k= 0;
while(n){
if(lapi[k]==pi)break;
if(lapi[k++]!=NULL)n--;
}
if(!n){
k= 0;
while(lapi[k]!=NULL)k++;
if(k>=MAXPATT){
fprintf(stderr,"I cannot allocate pp, is anything wrong?\n");
exit(BAD_PROGRAM);
}
lapt[k]= pp;lapi[k]= pi;ap++;
}

/*:31*/
#line 597 "../vlna-1.3/vlna.w"
;
}
pp= pp->next;
}

/*:30*/
#line 528 "../vlna-1.3/vlna.w"
;
if(ap==0&&ind> BUFI&&c!='\\')/*28:*/
#line 561 "../vlna-1.3/vlna.w"

{
buff[ind]= 0;
fputs(&buff[1],output);
ind= 1;
}

/*:28*/
#line 529 "../vlna-1.3/vlna.w"
;
if(ind>=MAXBUFF){
fprintf(stderr,"Operating buffer overflow, is anything wrong?\n");
exit(BAD_PROGRAM);
}
if((ic= getc(input))==EOF)
break;
buff[ind++]= c= ic;
if(c=='\n')numline++,listpatt= normallist;
if(c=='%'&&mode!=VERBMODE&&buff[ind-2]!='\\')listpatt= commentlist;
/*29:*/
#line 574 "../vlna-1.3/vlna.w"

n= ap;k= 0;
while(n){
while(lapi[k]==NULL)k++;
switch(m= match(lapi[k])){
case FOUND:(*lapt[k]->proc)();
case NOFOUND:lapi[k]= NULL;
ap--;break;
default:while(m--)lapi[k]= lapi[k]->next;
}
k++;n--;
}

/*:29*/
#line 539 "../vlna-1.3/vlna.w"
;
}
/*28:*/
#line 561 "../vlna-1.3/vlna.w"

{
buff[ind]= 0;
fputs(&buff[1],output);
ind= 1;
}

/*:28*/
#line 541 "../vlna-1.3/vlna.w"
;
if(!web)checkmode();
if(!silent)/*32:*/
#line 625 "../vlna-1.3/vlna.w"

fprintf(stderr,"~~~ file: %s\t  lines: %ld, changes: %ld\n",
filename,numline,numchanges);

/*:32*/
#line 543 "../vlna-1.3/vlna.w"
;
}

/*:26*/
#line 23 "../vlna-1.3/vlna.w"

/*5:*/
#line 61 "../vlna-1.3/vlna.w"

int main(argc,argv)
int argc;
char**argv;
{
/*9:*/
#line 161 "../vlna-1.3/vlna.w"

FILE*infile,*outfile;
char backup[MAXLEN];
int j;

/*:9*/
#line 66 "../vlna-1.3/vlna.w"
;
prog_name= argv[0];status= OK;
/*8:*/
#line 138 "../vlna-1.3/vlna.w"

while(argc> 1&&argv[1][0]=='-'){
if(argv[1][2]!=0)printusage(),exit(BAD_OPTIONS);
switch(argv[1][1]){
case'f':isfilter= 1;break;
case's':silent= 1;break;
case'r':rmbackup= 1;break;
case'v':if(argc<2)printusage(),exit(BAD_OPTIONS);
argv++;argc--;charset= argv[1];break;
case'm':nomath= 1;break;
case'n':noverb= 1;break;
case'l':latex= 1;break;
case'w':web= 1;break;
default:printusage(),exit(BAD_OPTIONS);

}
argc--;argv++;
}

/*:8*/
#line 68 "../vlna-1.3/vlna.w"
;
if(!silent)fprintf(stderr,BANNER);
/*21:*/
#line 423 "../vlna-1.3/vlna.w"

for(i= 0;i<256;i++){
strings[2*i]= (char)i;strings[2*i+1]= 0;
}

/*:21*//*34:*/
#line 657 "../vlna-1.3/vlna.w"

vlnkalist= setpattern(vlnkain);
setpi(tblankscr,ONE);
setpi(tblanks,ANY);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);

/*:34*//*37:*/
#line 685 "../vlna-1.3/vlna.w"

setpattern(vlnkacr);
setpi(tblankscr,ONE);
setpi(tblanks,ANY);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ANY);
setpi(cr,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);

/*:37*//*39:*/
#line 731 "../vlna-1.3/vlna.w"

setpattern(vlnkain);
setpi(tblankscr,ONE);
setpi(backslash,ONE);
setpi(letters,ONE);
setpi(letters,ANY);
setpi(openbrace,ONE);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);

setpattern(vlnkacr);
setpi(tblankscr,ONE);
setpi(backslash,ONE);
setpi(letters,ONE);
setpi(letters,ANY);
setpi(openbrace,ONE);
setpi(prefixes,ANY);
setpi(charset,ONE);
setpi(blanks,ANY);
setpi(cr,ONE);
setpi(blanks,ANY);
setpi(nochar,ONE_NOT);



/*:39*//*40:*/
#line 764 "../vlna-1.3/vlna.w"

normalpattern(tielock,"\\TeX");
setpi(blankscr,ONE);
normalpattern(tielock,"\\LaTeX");
setpi(blankscr,ONE);

/*:40*//*43:*/
#line 798 "../vlna-1.3/vlna.w"

if(!nomath){
mathlist= setpattern(onedollar);
setpi(dolar,ONE);
setpi(dolar,ONE_NOT);
if(latex){
normalpattern(mathin,"\\begin.{math}");
normalpattern(mathout,"\\end.{math}");
}
}

/*:43*//*46:*/
#line 840 "../vlna-1.3/vlna.w"

parcheck= setpattern(checkmode);
setpi(cr,ONE);
setpi(blanks,ANY);
setpi(cr,ONE);

/*:46*//*48:*/
#line 861 "../vlna-1.3/vlna.w"

if(!nomath){
normalpattern(twodollars,"$$");
if(latex){
normalpattern(displayin,"\\begin.{displaymath");
normalpattern(displayin,"\\begin.{equation");
normalpattern(displayout,"\\end.{displaymath");
normalpattern(displayout,"\\end.{equation");
}
}

/*:48*//*50:*/
#line 893 "../vlna-1.3/vlna.w"

if(!noverb){
verblist= normalpattern(verbinchar,"\\verb");
setpi(blankscr,ANY);
setpi(blankscr,ONE_NOT);
normalpattern(verbin,"\\begtt");
if(latex)normalpattern(verbin,"\\begin.{verbatim");
}
if(web){
normalpattern(verbin,"@<");
normalpattern(verbin,"@d");
}
if(!noverb){
verboutlist[0]= setpattern(verbout);
setpi(verbchar,ONE);
verboutlist[1]= normalpattern(verbout,"\\endtt");
if(latex)verboutlist[2]= normalpattern(verbout,"\\end{verbatim");
}
if(web){
verboutlist[3]= normalpattern(verbout,"@ ");
normalpattern(verbout,"@*");
normalpattern(verbout,"@>|");
}


/*:50*//*54:*/
#line 979 "../vlna-1.3/vlna.w"

lastpt= 0;
commentlist= normalpattern(tieoff,"%.~.-");
normalpattern(tieon,"%.~.+");

/*:54*/
#line 70 "../vlna-1.3/vlna.w"
;
/*11:*/
#line 176 "../vlna-1.3/vlna.w"

if(isfilter)/*12:*/
#line 188 "../vlna-1.3/vlna.w"

{
if(argc> 3)printusage(),exit(BAD_OPTIONS);
infile= stdin;outfile= stdout;
if(argc>=2)infile= fopen(argv[1],"r");
if(infile==NULL)ioerr(argv[1]),exit(IO_ERR);
if(argc==3)outfile= fopen(argv[2],"w");
if(outfile==NULL)ioerr(argv[2]),exit(IO_ERR);
if(argc>=2)filename= argv[1];
else filename= NULL;
tie(infile,outfile);
if(outfile!=stdout)fclose(outfile);
if(infile!=stdin)fclose(infile);
}

/*:12*/
#line 177 "../vlna-1.3/vlna.w"

else/*13:*/
#line 211 "../vlna-1.3/vlna.w"

{
if(argc==1)printusage(),exit(BAD_OPTIONS);
while(argc> 1){
argc--;argv++;
/*14:*/
#line 242 "../vlna-1.3/vlna.w"

infile= NULL;
j= strlen(argv[0])-1;
if(j>=MAXLEN||argv[0][j]=='~'){
if(!silent)fprintf(stderr,"%s: the conflict of file name %s\n",
prog_name,argv[0]);
}
else{
strcpy(backup,argv[0]);
backup[j]= '~';
remove(backup);
j= rename(argv[0],backup);
if(j==0)infile= fopen(backup,"r");
}

/*:14*/
#line 216 "../vlna-1.3/vlna.w"
;
if(infile==NULL){
ioerr(argv[0]);continue;
}
outfile= fopen(argv[0],"w");
if(outfile==NULL){
ioerr(argv[0]);
rename(backup,argv[0]);
status= WARNING;
continue;
}
filename= argv[0];
tie(infile,outfile);
fclose(outfile),fclose(infile);
if(rmbackup)remove(backup);
}
}

/*:13*/
#line 178 "../vlna-1.3/vlna.w"


/*:11*/
#line 71 "../vlna-1.3/vlna.w"
;
return status;
}

/*:5*/
#line 24 "../vlna-1.3/vlna.w"


/*:1*/
