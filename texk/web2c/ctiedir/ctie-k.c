#define version_number "1.1"
#define banner "This is CTIE, Version 1.1"
#define copyright  \
"Copyright 2002,2003 Julian Gilbey.  All rights reserved.  There is no warranty.\n\
Run with the --version option for other important information." \
 \

#define kpse_find_cweb(name) kpse_find_file(name,kpse_cweb_format,true)  \

#define buf_size 1024
#define max_file_index 32 \

#define xisupper(c) (isupper(c) &&((unsigned char) c<0200) )  \
 \

#define spotless 0
#define troublesome 1
#define fatal 2 \

#define max_include_files 20 \

#define max_file_name_length 1024 \

#define too_long() {total_include_files--;free(new_inc) ; \
err_print(i,"! Include file name too long") ;goto restart;} \

#define fatal_error(i,s,t) { \
fprintf(stderr,"\n%s",s) ; \
err_print(i,t) ; \
history= fatal; \
exit(wrap_up() ) ; \
} \
 \

#define none (-1)  \

/*2:*/
#line 89 "./ctie.w"

/*10:*/
#line 172 "./ctie.w"

#include <stdio.h> 


#line 126 "./ctie-k.ch"
/*:10*//*39:*/
#line 715 "./ctie.w"

#include <errno.h> 


/*:39*//*72:*/
#line 454 "./ctie-k.ch"

#include <kpathsea/kpathsea.h> 

/*:72*/
#line 90 "./ctie.w"

/*12:*/
#line 211 "./ctie.w"

#define search 0
#define test 1
#define reading 2
#define ignore 3
typedef int in_file_modes;
#define unknown 0
#define master 1
#define chf 2
typedef int file_types;


/*:12*//*13:*/
#line 229 "./ctie.w"

#define normal 0
#define pre 1
#define post 2
typedef int out_md_type;


/*:13*//*14:*/
#line 239 "./ctie.w"

typedef int file_index;


/*:14*//*15:*/
#line 246 "./ctie.w"

typedef struct _indsc{
char file_name[max_file_name_length];
long line;
FILE*the_file;
struct _indsc*parent;
}include_description;


/*:15*//*16:*/
#line 259 "./ctie.w"

typedef struct _idsc{
string file_name;
char buffer[buf_size];
in_file_modes mode;
long line;
file_types type_of_file;
include_description*current_include;
char*buffer_end;
char*limit;
char*loc;
FILE*the_file;
int dont_match;
}input_description;


/*:16*/
#line 91 "./ctie.w"

/*30:*/
#line 584 "./ctie.w"

#line 223 "./ctie-k.ch"
void err_print P2H(file_index,char*);
#line 586 "./ctie.w"

/*:30*//*35:*/
#line 673 "./ctie.w"

#line 243 "./ctie-k.ch"
int wrap_up P1H(void);
#line 675 "./ctie.w"

/*:35*//*37:*/
#line 696 "./ctie.w"

#line 249 "./ctie-k.ch"
void pfatal_error P2H(char*,char*);
#line 698 "./ctie.w"

/*:37*//*69:*/
#line 1232 "./ctie.w"

#line 392 "./ctie-k.ch"
void usage_help P1H(void);
void print_version_and_exit P2H(string,string);
#line 1235 "./ctie.w"


/*:69*/
#line 92 "./ctie.w"

/*9:*/
#line 162 "./ctie.w"

int history= spotless;


/*:9*//*17:*/
#line 286 "./ctie.w"

int total_include_files= 0;

/*:17*//*18:*/
#line 292 "./ctie.w"

file_index actual_input,test_input,no_ch;
file_types prod_chf= unknown;
out_md_type out_mode;

/*:18*//*20:*/
#line 309 "./ctie.w"

input_description*input_organisation[max_file_index+1];


/*:20*//*24:*/
#line 406 "./ctie.w"

boolean input_has_ended= false;


/*:24*//*41:*/
#line 742 "./ctie.w"

FILE*out_file;
string out_name;


#line 273 "./ctie-k.ch"
/*:41*//*68:*/
#line 1217 "./ctie.w"

string CTIEHELP[]= {
"Usage: ctie -[mc] outfile master changefile(s)",
"  Create a new master file or change file from the given",
"  master (C)WEB file and changefiles.",
"  All filenames are taken literally; no suffixes are added.",
"",
"-m  create a new master file from original (C)WEB and change file(s)",
"-c  create a master change file for original (C)WEB file from changefile(s)",
"--help      display this help and exit",
"--version   display version information and exit",
NULL
};


/*:68*//*73:*/
#line 459 "./ctie-k.ch"

string file_open_mode;
#line 1278 "./ctie.w"


/*:73*/
#line 93 "./ctie.w"

/*31:*/
#line 589 "./ctie.w"

#line 230 "./ctie-k.ch"
void err_print P2C(file_index,i,char*,s)

#line 592 "./ctie.w"
{
char*k,*l;
fprintf(stderr,*s=='!'?"\n%s":"%s",s);
if(i>=0)/*32:*/
#line 611 "./ctie.w"

{
register input_description*inp_desc= input_organisation[i];
register include_description*inc_desc= inp_desc->current_include;

if(inc_desc!=NULL){
fprintf(stderr,". (l. %ld of include file %s",inc_desc->line,
inc_desc->file_name);
fprintf(stderr," included from l. %ld of %s file %s)\n",
inp_desc->line,
inp_desc->type_of_file==master?"master":"change",
inp_desc->file_name);
}
else
fprintf(stderr,". (l. %ld of %s file %s)\n",inp_desc->line,
inp_desc->type_of_file==master?"master":"change",
inp_desc->file_name);
l= (inp_desc->loc>=inp_desc->limit?inp_desc->limit:inp_desc->loc);
if(l> inp_desc->buffer){
for(k= inp_desc->buffer;k<l;k++)
if(*k=='\t')putc(' ',stderr);
else putc(*k,stderr);
putc('\n',stderr);
for(k= inp_desc->buffer;k<l;k++)
putc(' ',stderr);
}
for(k= l;k<inp_desc->limit;k++)
putc(*k,stderr);
putc('\n',stderr);
}


/*:32*/
#line 595 "./ctie.w"

else putc('\n',stderr);
fflush(stderr);
history= troublesome;
}

/*:31*//*38:*/
#line 699 "./ctie.w"

#line 256 "./ctie-k.ch"
void pfatal_error P2C(char*,s,char*,t)
#line 702 "./ctie.w"
{
char*strerr= strerror(errno);

fprintf(stderr,"\n%s%s",s,t);
if(strerr)fprintf(stderr," (%s)\n",strerr);
else fprintf(stderr,"\n");
history= fatal;
exit(wrap_up());
}


/*:38*/
#line 94 "./ctie.w"

/*21:*/
#line 328 "./ctie.w"

#line 143 "./ctie-k.ch"
boolean get_line P2C(file_index,i,boolean,do_includes)
#line 331 "./ctie.w"
{
register input_description*inp_desc= input_organisation[i];
register FILE*fp;

if(inp_desc->mode==ignore)return false;

restart:
if(inp_desc->current_include!=NULL){
register include_description*inc_desc= inp_desc->current_include;

fp= inc_desc->the_file;
/*26:*/
#line 426 "./ctie.w"

{
register int c;
register char*k;

if(feof(fp))
/*27:*/
#line 452 "./ctie.w"

{
include_description*temp= inc_desc->parent;

fclose(fp);
free(inc_desc);
total_include_files--;
inp_desc->current_include= temp;
goto restart;
}


/*:27*/
#line 432 "./ctie.w"


inp_desc->limit= k= inp_desc->buffer;
while(k<=inp_desc->buffer_end&&(c= getc(fp))!=EOF&&c!='\n')
if((*(k++)= c)!=' ')inp_desc->limit= k;
if(k> inp_desc->buffer_end)
if((c= getc(fp))!=EOF&&c!='\n'){
ungetc(c,fp);inp_desc->loc= inp_desc->buffer;
err_print(i,"! Input line too long");

}
if(c==EOF&&inp_desc->limit==inp_desc->buffer)
/*27:*/
#line 452 "./ctie.w"

{
include_description*temp= inc_desc->parent;

fclose(fp);
free(inc_desc);
total_include_files--;
inp_desc->current_include= temp;
goto restart;
}


/*:27*/
#line 444 "./ctie.w"


inc_desc->line++;
}

/*:26*/
#line 342 "./ctie.w"

}
else{
fp= inp_desc->the_file;
/*22:*/
#line 366 "./ctie.w"

{
register int c;
register char*k;

if(feof(fp))
/*23:*/
#line 394 "./ctie.w"

{
inp_desc->mode= ignore;
inp_desc->limit= NULL;
if(inp_desc->type_of_file==master)input_has_ended= true;
fclose(fp);
return false;
}


/*:23*/
#line 372 "./ctie.w"


inp_desc->limit= k= inp_desc->buffer;
while(k<=inp_desc->buffer_end&&(c= getc(fp))!=EOF&&c!='\n')
if((*(k++)= c)!=' ')inp_desc->limit= k;
if(k> inp_desc->buffer_end)
if((c= getc(fp))!=EOF&&c!='\n'){
ungetc(c,fp);inp_desc->loc= inp_desc->buffer;
err_print(i,"! Input line too long");

}
if(c==EOF&&inp_desc->limit==inp_desc->buffer)
/*23:*/
#line 394 "./ctie.w"

{
inp_desc->mode= ignore;
inp_desc->limit= NULL;
if(inp_desc->type_of_file==master)input_has_ended= true;
fclose(fp);
return false;
}


/*:23*/
#line 384 "./ctie.w"


/*25:*/
#line 414 "./ctie.w"

inp_desc->line++;
if(inp_desc->type_of_file==master&&inp_desc->line%100==0){
if(inp_desc->line%500==0)printf("%ld",inp_desc->line);
else putchar('.');
fflush(stdout);
}


/*:25*/
#line 387 "./ctie.w"

}


/*:22*/
#line 346 "./ctie.w"

}

if(do_includes)
/*28:*/
#line 467 "./ctie.w"

{
inp_desc->loc= inp_desc->buffer;
*inp_desc->limit= ' ';
if(*inp_desc->buffer=='@'&&
(inp_desc->buffer[1]=='i'||inp_desc->buffer[1]=='I')){
inp_desc->loc= inp_desc->buffer+2;
*inp_desc->limit= '"';
while(*inp_desc->loc==' '||*inp_desc->loc=='\t')
inp_desc->loc++;
if(inp_desc->loc>=inp_desc->limit){
err_print(i,"! Include file name not given");

goto restart;
}
if(total_include_files>=max_include_files){
err_print(i,"! Too many nested includes");

goto restart;
}
total_include_files++;
/*29:*/
#line 507 "./ctie.w"

{
include_description*new_inc;
#line 166 "./ctie-k.ch"
char*file_name_end;
string fullname;
char*k;
#line 514 "./ctie.w"

new_inc= (include_description*)malloc(sizeof(include_description));
if(new_inc==NULL)
fatal_error(i,"! No memory for new include descriptor","");
new_inc->line= 0;
k= new_inc->file_name;
file_name_end= k+max_file_name_length-1;

if(*inp_desc->loc=='"'){
inp_desc->loc++;
while(*inp_desc->loc!='"'&&k<=file_name_end)
*k++= *inp_desc->loc++;
if(inp_desc->loc==inp_desc->limit)
k= file_name_end+1;
}else
while(*inp_desc->loc!=' '&&*inp_desc->loc!='\t'&&
*inp_desc->loc!='"'&&k<=file_name_end)*k++= *inp_desc->loc++;
if(k> file_name_end)too_long();

*k= '\0';
#line 174 "./ctie-k.ch"
fullname= kpse_find_cweb(new_inc->file_name);
if(fullname)
new_inc->the_file= fopen(fullname,"r");
if(fullname!=NULL&&new_inc->the_file!=NULL){
free(fullname);
#line 535 "./ctie.w"
new_inc->parent= inp_desc->current_include;
inp_desc->current_include= new_inc;
goto restart;
}
#line 563 "./ctie.w"
total_include_files--;
free(new_inc);
#line 212 "./ctie-k.ch"
if(fullname){
free(fullname);
err_print(i,"! Cannot open include file");
}else
err_print(i,"! Cannot find include file");
#line 566 "./ctie.w"

goto restart;
}



/*:29*/
#line 488 "./ctie.w"
;
}
}


/*:28*/
#line 351 "./ctie.w"

return true;
}


/*:21*//*34:*/
#line 663 "./ctie.w"

#line 237 "./ctie-k.ch"
int wrap_up P1H(void)
#line 665 "./ctie.w"
{
/*36:*/
#line 682 "./ctie.w"

switch(history){
case spotless:
printf("\n(No errors were found.)\n");break;
case troublesome:
printf("\n(Pardon me, but I think I spotted something wrong.)\n");break;
case fatal:printf("(That was a fatal error, my friend.)\n");
}


/*:36*/
#line 666 "./ctie.w"
;
if(history> spotless)return 1;
else return 0;
}

/*:34*//*44:*/
#line 791 "./ctie.w"

#line 342 "./ctie-k.ch"
boolean lines_dont_match P2C(file_index,i,file_index,j)
#line 794 "./ctie.w"
{
register input_description*iptr= input_organisation[i],
*jptr= input_organisation[j];

if(iptr->limit-iptr->buffer!=jptr->limit-jptr->buffer)
return true;
return strncmp(iptr->buffer,jptr->buffer,iptr->limit-iptr->buffer);
}


/*:44*//*45:*/
#line 808 "./ctie.w"

#line 349 "./ctie-k.ch"
void init_change_file P1C(file_index,i)
#line 811 "./ctie.w"
{
register input_description*inp_desc= input_organisation[i];
char ccode;

inp_desc->limit= inp_desc->buffer;
/*46:*/
#line 827 "./ctie.w"

while(1){
if(!get_line(i,false))return;
if(inp_desc->limit<inp_desc->buffer+2)continue;
if(inp_desc->buffer[0]!='@')continue;
ccode= inp_desc->buffer[1];
if(xisupper(ccode))ccode= tolower(ccode);
if(ccode=='x')break;
if(ccode=='y'||ccode=='z'||ccode=='i'){
inp_desc->loc= inp_desc->buffer+2;
err_print(i,"! Missing @x in change file");

}
}

/*:46*/
#line 816 "./ctie.w"

/*47:*/
#line 844 "./ctie.w"

do{
if(!get_line(i,true)){
err_print(i,"! Change file ended after @x");

return;
}
}while(inp_desc->limit==inp_desc->buffer);


/*:47*/
#line 817 "./ctie.w"

inp_desc->dont_match= 0;
}


/*:45*//*48:*/
#line 857 "./ctie.w"

#line 356 "./ctie-k.ch"
void put_line P1C(file_index,j)
#line 860 "./ctie.w"
{
char*ptr= input_organisation[j]->buffer;
char*lmt= input_organisation[j]->limit;

while(ptr<lmt)putc(*ptr++,out_file);
putc('\n',out_file);
}


/*:48*//*49:*/
#line 872 "./ctie.w"

#line 363 "./ctie-k.ch"
boolean e_of_ch_module P1C(file_index,i)
#line 875 "./ctie.w"
{
register input_description*inp_desc= input_organisation[i];

if(inp_desc->limit==NULL){
err_print(i,"! Change file ended without @z");

return true;
}else if(inp_desc->limit>=inp_desc->buffer+2)
if(inp_desc->buffer[0]=='@'&&
(inp_desc->buffer[1]=='Z'||inp_desc->buffer[1]=='z'))
return true;
return false;
}


/*:49*//*50:*/
#line 893 "./ctie.w"

#line 370 "./ctie-k.ch"
boolean e_of_ch_preamble P1C(file_index,i)
#line 896 "./ctie.w"
{
register input_description*inp_desc= input_organisation[i];

if(inp_desc->limit>=inp_desc->buffer+2&&inp_desc->buffer[0]=='@')
if(inp_desc->buffer[1]=='Y'||inp_desc->buffer[1]=='y'){
if(inp_desc->dont_match> 0){
inp_desc->loc= inp_desc->buffer+2;
fprintf(stderr,"\n! Hmm... %d ",inp_desc->dont_match);
err_print(i,"of the preceding lines failed to match");
}
return true;
}
return false;
}



/*:50*//*61:*/
#line 1105 "./ctie.w"

#line 376 "./ctie-k.ch"
void usage_error P1H(void)
#line 1107 "./ctie.w"
{
/*62:*/
#line 1118 "./ctie.w"

#line 382 "./ctie-k.ch"
{
extern KPSEDLL string kpathsea_version_string;
printf("%s (%s)\n",banner,kpathsea_version_string);
}
#line 1120 "./ctie.w"
printf("%s\n",copyright);


/*:62*/
#line 1108 "./ctie.w"
;
fprintf(stderr,"Usage: ctie -[mc] outfile master changefile(s)\n");
fprintf(stderr,"Type ctie --help for more information\n");
exit(1);
}


/*:61*/
#line 95 "./ctie.w"

/*3:*/
#line 101 "./ctie.w"

#line 32 "./ctie-k.ch"
int main P2C(int,argc,string*,argv)
#line 104 "./ctie.w"
{
#line 38 "./ctie-k.ch"
/*5:*/
#line 84 "./ctie-k.ch"

kpse_set_program_name(argv[0],"ctie");

/*:5*/
#line 38 "./ctie-k.ch"
;
/*19:*/
#line 300 "./ctie.w"

actual_input= 0;
out_mode= normal;

/*:19*/
#line 39 "./ctie-k.ch"
;
#line 106 "./ctie.w"
/*63:*/
#line 1135 "./ctie.w"

{
if(argc> max_file_index+5-1)usage_error();
no_ch= -1;
while(--argc> 0){
argv++;
if(strcmp("-help",*argv)==0||strcmp("--help",*argv)==0)
/*66:*/
#line 1202 "./ctie.w"

usage_help();



/*:66*/
#line 1142 "./ctie.w"
;
if(strcmp("-version",*argv)==0||strcmp("--version",*argv)==0)
/*67:*/
#line 1208 "./ctie.w"

{
print_version_and_exit("CTIE",version_number);

}


/*:67*/
#line 1144 "./ctie.w"
;
if(**argv=='-')/*64:*/
#line 1158 "./ctie.w"

if(prod_chf!=unknown)usage_error();
else
switch(*(*argv+1)){
case'c':case'C':prod_chf= chf;break;
case'm':case'M':prod_chf= master;break;
default:usage_error();
}


/*:64*/
#line 1145 "./ctie.w"

else/*65:*/
#line 1172 "./ctie.w"

{
if(no_ch==(-1)){
out_name= *argv;
}else{
register input_description*inp_desc;

inp_desc= (input_description*)malloc(sizeof(input_description));
if(inp_desc==NULL)
fatal_error(-1,"! No memory for input descriptor","");

inp_desc->mode= search;
inp_desc->line= 0;
inp_desc->type_of_file= chf;
inp_desc->limit= inp_desc->buffer;
inp_desc->buffer[0]= ' ';
inp_desc->loc= inp_desc->buffer+1;
inp_desc->buffer_end= inp_desc->buffer+buf_size-2;
inp_desc->file_name= *argv;
inp_desc->current_include= NULL;
input_organisation[no_ch]= inp_desc;
}
no_ch++;
}


/*:65*/
#line 1146 "./ctie.w"

}
if(no_ch<=0||prod_chf==unknown)usage_error();
}


/*:63*/
#line 106 "./ctie.w"

/*62:*/
#line 1118 "./ctie.w"

#line 382 "./ctie-k.ch"
{
extern KPSEDLL string kpathsea_version_string;
printf("%s (%s)\n",banner,kpathsea_version_string);
}
#line 1120 "./ctie.w"
printf("%s\n",copyright);


/*:62*/
#line 107 "./ctie.w"
;
/*42:*/
#line 277 "./ctie-k.ch"

{
string fullname;

fullname= kpse_find_cweb(input_organisation[0]->file_name);
if(fullname)
input_organisation[0]->the_file= fopen(fullname,"r");

if(fullname==NULL||input_organisation[0]->the_file==NULL){
if(fullname){
pfatal_error("! Cannot open master file ",
input_organisation[0]->file_name);
}else{
fatal_error(-1,"! Cannot find master file ",
input_organisation[0]->file_name);
}
}
else free(fullname);


#line 759 "./ctie.w"
printf("(%s)\n",input_organisation[0]->file_name);
input_organisation[0]->type_of_file= master;
get_line(0,true);
}


/*:42*/
#line 108 "./ctie.w"

/*43:*/
#line 313 "./ctie-k.ch"

{
file_index i;
string fullname;

i= 1;
while(i<no_ch){
fullname= kpse_find_cweb(input_organisation[i]->file_name);
if(fullname)
input_organisation[i]->the_file= fopen(fullname,"r");

if(fullname==NULL||input_organisation[i]->the_file==NULL){
if(fullname){
pfatal_error("! Cannot open change file ",
input_organisation[i]->file_name);
}else{
fatal_error(-1,"! Cannot find change file ",
input_organisation[i]->file_name);
}
}
else free(fullname);


#line 780 "./ctie.w"
printf("(%s)\n",input_organisation[i]->file_name);
init_change_file(i);
i++;
}
}


/*:43*/
#line 109 "./ctie.w"

/*40:*/
#line 729 "./ctie.w"

{
out_file= fopen(out_name,"w");
if(out_file==NULL){
pfatal_error("! Cannot open/create output file","");

}
}


/*:40*/
#line 110 "./ctie.w"

/*59:*/
#line 1074 "./ctie.w"

actual_input= 0;
input_has_ended= false;
while(input_has_ended==false||actual_input!=0)
/*51:*/
#line 917 "./ctie.w"

{
file_index test_file;

/*52:*/
#line 934 "./ctie.w"

{
register input_description*inp_desc;
while(actual_input> 0&&e_of_ch_module(actual_input)){
inp_desc= input_organisation[actual_input];
if(inp_desc->type_of_file==master){

fatal_error(-1,"! This can't happen: change file is master file","");

}
inp_desc->mode= search;
init_change_file(actual_input);
while((input_organisation[actual_input]->mode!=reading
&&actual_input> 0))
actual_input--;
}
}


/*:52*/
#line 921 "./ctie.w"

if(input_has_ended&&actual_input==0)break;
/*53:*/
#line 960 "./ctie.w"

test_input= none;
test_file= actual_input;
while(test_input==none&&test_file<no_ch-1){
test_file++;
switch(input_organisation[test_file]->mode){
case search:
if(lines_dont_match(actual_input,test_file)==false){
input_organisation[test_file]->mode= test;
test_input= test_file;
}
break;
case test:
if(lines_dont_match(actual_input,test_file)){

input_organisation[test_file]->dont_match++;
}
test_input= test_file;
break;
case reading:
break;
case ignore:
break;
}
}


/*:53*/
#line 923 "./ctie.w"

/*54:*/
#line 993 "./ctie.w"

if(prod_chf==chf){
while(1){
/*55:*/
#line 1007 "./ctie.w"

if(out_mode==normal){
if(test_input!=none){
fprintf(out_file,"@x\n");
out_mode= pre;
}else break;
}


/*:55*/
#line 996 "./ctie.w"

/*56:*/
#line 1021 "./ctie.w"

if(out_mode==pre){
if(test_input==none){
fprintf(out_file,"@y\n");
out_mode= post;
}else{
if(input_organisation[actual_input]->type_of_file==master)
put_line(actual_input);
break;
}
}


/*:56*/
#line 997 "./ctie.w"

/*57:*/
#line 1040 "./ctie.w"

if(out_mode==post){
if(input_organisation[actual_input]->type_of_file==chf){
if(test_input==none)put_line(actual_input);
break;
}else{
fprintf(out_file,"@z\n\n");
out_mode= normal;
}
}


/*:57*/
#line 998 "./ctie.w"

}
}else
if(test_input==none)put_line(actual_input);


/*:54*/
#line 924 "./ctie.w"

/*58:*/
#line 1055 "./ctie.w"

get_line(actual_input,true);
if(test_input!=none){
get_line(test_input,true);
if(e_of_ch_preamble(test_input)==true){
get_line(test_input,true);
input_organisation[test_input]->mode= reading;
actual_input= test_input;
test_input= none;
}
}


/*:58*/
#line 925 "./ctie.w"

}


/*:51*/
#line 1078 "./ctie.w"

if(out_mode==post)
fprintf(out_file,"@z\n");


/*:59*/
#line 111 "./ctie.w"

/*60:*/
#line 1087 "./ctie.w"

{
file_index i;

for(i= 1;i<no_ch;i++){
if(input_organisation[i]->mode!=ignore){
input_organisation[i]->loc= input_organisation[i]->buffer;
err_print(i,"! Change file entry did not match");

}
}
}


/*:60*/
#line 112 "./ctie.w"

exit(wrap_up());
}

#line 57 "./ctie-k.ch"
/*:3*/
#line 96 "./ctie.w"


/*:2*//*70:*/
#line 1237 "./ctie.w"

#line 399 "./ctie-k.ch"
void usage_help P1H(void)
#line 1239 "./ctie.w"
{
string*message= CTIEHELP;

while(*message){
fputs(*message,stdout);
putchar('\n');
++message;
}
putchar('\n');
exit(0);
}


/*:70*//*71:*/
#line 1252 "./ctie.w"

#line 416 "./ctie-k.ch"
void print_version_and_exit P2C(string,name,string,version)
{
extern KPSEDLL string kpathsea_version_string;
printf("%s %s\n",name,version);
puts(kpathsea_version_string);

puts("Copyright (C) 2002,2003 Julian Gilbey.");
puts("Kpathsea is copyright (C) 1999 Free Software Foundation, Inc.");

puts("There is NO warranty.  This is free software.");
puts("Redistribution of this software is covered by the terms of");
puts("both the CTIE copyright and the GNU General Public Licence.");
puts("For more information about these matters, see the files");
puts("named COPYING and the CTIE source.");
puts("Primary authors of CTIE: Julian Gilbey.");
puts("Kpathsea written by Karl Berry and others.\n");

exit(0);
}
#line 1265 "./ctie.w"


#line 450 "./ctie-k.ch"
/*:71*/
