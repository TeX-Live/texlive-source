/*1:*/
#line 62 "common.w"

/*3:*/
#line 44 "common.h"

#include <ctype.h>  
#include <stdbool.h>  
#include <stddef.h>  
#include <stdint.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  

/*:3*/
#line 63 "common.w"

#define ctangle false
#define cweave true \

#define and_and 04
#define lt_lt 020
#define gt_gt 021
#define plus_plus 013
#define minus_minus 01
#define minus_gt 031
#define non_eq 032
#define lt_eq 034
#define gt_eq 035
#define eq_eq 036
#define or_or 037
#define dot_dot_dot 016
#define colon_colon 06
#define period_ast 026
#define minus_gt_ast 027 \

#define compress(c) if(loc++<=limit) return c \

#define xisalpha(c) (isalpha((int) (c) ) &&((eight_bits) (c) <0200) ) 
#define xisdigit(c) (isdigit((int) (c) ) &&((eight_bits) (c) <0200) ) 
#define xisspace(c) (isspace((int) (c) ) &&((eight_bits) (c) <0200) ) 
#define xislower(c) (islower((int) (c) ) &&((eight_bits) (c) <0200) ) 
#define xisupper(c) (isupper((int) (c) ) &&((eight_bits) (c) <0200) ) 
#define xisxdigit(c) (isxdigit((int) (c) ) &&((eight_bits) (c) <0200) ) 
#define isxalpha(c) ((c) =='_'||(c) =='$')  \

#define ishigh(c) ((eight_bits) (c) > 0177)  \
 \

#define max_include_depth 10 \

#define max_file_name_length 60
#define cur_file file[include_depth]
#define cur_file_name file_name[include_depth]
#define cur_line line[include_depth]
#define web_file file[0]
#define web_file_name file_name[0] \

#define length(c) (size_t) ((c+1) ->byte_start-(c) ->byte_start) 
#define print_id(c) term_write((c) ->byte_start,length(c) ) 
#define llink link
#define rlink dummy.Rlink
#define root name_dir->rlink \
 \

#define spotless 0
#define harmless_message 1
#define error_message 2
#define fatal_message 3
#define mark_harmless if(history==spotless) history= harmless_message
#define mark_error history= error_message
#define confusion(s) fatal("! This can't happen: ",s)  \
 \

#define show_banner flags['b']
#define show_progress flags['p']
#define show_happiness flags['h']
#define show_stats flags['s']
#define make_xrefs flags['x'] \

#define update_terminal fflush(stdout) 
#define new_line putchar('\n') 
#define term_write(a,b) fflush(stdout) ,fwrite(a,sizeof(char) ,b,stdout)  \

#define buf_size 200
#define longest_name 10000 \

#define long_buf_size (buf_size+longest_name) 
#define max_bytes 100000 \

#define max_names 5000 \

#define max_sections 2000 \

#define lines_dont_match (change_limit-change_buffer!=limit-buffer|| \
strncmp(buffer,change_buffer,(size_t) (limit-buffer) ) !=0)  \

#define if_section_start_make_pending(b)  \
*limit= '!'; \
for(loc= buffer;xisspace(*loc) ;loc++) ; \
*limit= ' '; \
if(*loc=='@'&&(xisspace(*(loc+1) ) ||*(loc+1) =='*') ) change_pending= b \

#define too_long() {include_depth--; \
err_print("! Include file name too long") ;goto restart;} \

#define hash_size 353 \

#define first_chunk(p) ((p) ->byte_start+2) 
#define prefix_length(p) (size_t) ((eight_bits) *((p) ->byte_start) *256+ \
(eight_bits) *((p) ->byte_start+1) ) 
#define set_prefix_length(p,m) (*((p) ->byte_start) = (char) ((m) /256) , \
*((p) ->byte_start+1) = (char) ((m) %256) )  \

#define less 0
#define equal 1
#define greater 2
#define prefix 3
#define extension 4 \

#define bad_extension 5 \

#define flag_change (**argv!='-') 

#line 64 "common.w"

/*2:*/
#line 35 "common.h"

typedef bool boolean;
typedef uint8_t eight_bits;
typedef uint16_t sixteen_bits;
extern boolean program;
extern int phase;

/*:2*//*4:*/
#line 74 "common.h"

extern char section_text[];
extern char*section_text_end;
extern char*id_first;
extern char*id_loc;

/*:4*//*5:*/
#line 92 "common.h"

extern char buffer[];
extern char*buffer_end;
extern char*loc;
extern char*limit;

/*:5*//*6:*/
#line 109 "common.h"

extern int include_depth;
extern FILE*file[];
extern FILE*change_file;
extern char file_name[][max_file_name_length];

extern char change_file_name[];
extern int line[];
extern int change_line;
extern int change_depth;
extern boolean input_has_ended;
extern boolean changing;
extern boolean web_file_open;

/*:6*//*8:*/
#line 129 "common.h"

extern sixteen_bits section_count;
extern boolean changed_section[];
extern boolean change_pending;
extern boolean print_where;

/*:8*//*9:*/
#line 143 "common.h"

typedef struct name_info{
char*byte_start;
struct name_info*link;
union{
struct name_info*Rlink;

char Ilk;
}dummy;
void*equiv_or_xref;
}name_info;
typedef name_info*name_pointer;
typedef name_pointer*hash_pointer;
extern char byte_mem[];
extern char*byte_mem_end;
extern char*byte_ptr;
extern name_info name_dir[];
extern name_pointer name_dir_end;
extern name_pointer name_ptr;
extern name_pointer hash[];
extern hash_pointer hash_end;
extern hash_pointer h;

/*:9*//*11:*/
#line 187 "common.h"

extern int history;

/*:11*//*13:*/
#line 203 "common.h"

extern int argc;
extern char**argv;
extern char C_file_name[];
extern char tex_file_name[];
extern char idx_file_name[];
extern char scn_file_name[];
extern boolean flags[];

/*:13*//*14:*/
#line 217 "common.h"

extern FILE*C_file;
extern FILE*tex_file;
extern FILE*idx_file;
extern FILE*scn_file;
extern FILE*active_file;

/*:14*/
#line 65 "common.w"

/*18:*/
#line 79 "common.w"

boolean program;

/*:18*//*19:*/
#line 88 "common.w"

int phase;

/*:19*//*21:*/
#line 126 "common.w"

char section_text[longest_name+1];
char*section_text_end= section_text+longest_name;
char*id_first;
char*id_loc;

/*:21*//*22:*/
#line 146 "common.w"

char buffer[long_buf_size];
char*buffer_end= buffer+buf_size-2;
char*loc= buffer;
char*limit= buffer;

/*:22*//*25:*/
#line 190 "common.w"

int include_depth;
FILE*file[max_include_depth];
FILE*change_file;
char file_name[max_include_depth][max_file_name_length];

char change_file_name[max_file_name_length];
static char alt_web_file_name[max_file_name_length];
int line[max_include_depth];
int change_line;
int change_depth;
boolean input_has_ended;
boolean changing;
boolean web_file_open= false;

/*:25*//*26:*/
#line 216 "common.w"

static char change_buffer[buf_size];
static char*change_limit;

/*:26*//*37:*/
#line 393 "common.w"

sixteen_bits section_count;
boolean changed_section[max_sections];
boolean change_pending;

boolean print_where= false;

/*:37*//*43:*/
#line 590 "common.w"

char byte_mem[max_bytes];
char*byte_mem_end= byte_mem+max_bytes-1;
name_info name_dir[max_names];
name_pointer name_dir_end= name_dir+max_names-1;

/*:43*//*44:*/
#line 601 "common.w"

char*byte_ptr;
name_pointer name_ptr;

/*:44*//*46:*/
#line 621 "common.w"

name_pointer hash[hash_size];
hash_pointer hash_end= hash+hash_size-1;
hash_pointer h;

/*:46*//*65:*/
#line 1003 "common.w"

int history= spotless;

/*:65*//*73:*/
#line 1126 "common.w"

int argc;
char**argv;
char C_file_name[max_file_name_length];
char tex_file_name[max_file_name_length];
char idx_file_name[max_file_name_length];
char scn_file_name[max_file_name_length];
boolean flags[128];

/*:73*//*83:*/
#line 1269 "common.w"

FILE*C_file;
FILE*tex_file;
FILE*idx_file;
FILE*scn_file;
FILE*active_file;

/*:83*/
#line 66 "common.w"

/*7:*/
#line 123 "common.h"

extern boolean get_line(void);
extern void check_complete(void);
extern void reset_input(void);

/*:7*//*10:*/
#line 166 "common.h"

extern boolean names_match(name_pointer,const char*,size_t,eight_bits);
extern name_pointer id_lookup(const char*,const char*,eight_bits);

extern name_pointer section_lookup(char*,char*,boolean);
extern void init_node(name_pointer);
extern void init_p(name_pointer,eight_bits);
extern void print_prefix_name(name_pointer);
extern void print_section_name(name_pointer);
extern void sprint_section_name(char*,name_pointer);

/*:10*//*12:*/
#line 190 "common.h"

extern int wrap_up(void);
extern void err_print(const char*);
extern void fatal(const char*,const char*);
extern void overflow(const char*);

/*:12*//*15:*/
#line 225 "common.h"

extern void common_init(void);
extern void print_stats(void);

/*:15*//*24:*/
#line 176 "common.w"
static boolean input_ln(FILE*);

/*:24*//*28:*/
#line 237 "common.w"
static void prime_the_change_buffer(void);

/*:28*//*33:*/
#line 334 "common.w"
static void check_change(void);

/*:33*//*55:*/
#line 764 "common.w"

static int web_strcmp(char*,size_t,char*,size_t);
static name_pointer add_section_name(name_pointer,int,char*,char*,boolean);
static void extend_section_name(name_pointer,char*,char*,boolean);

/*:55*//*64:*/
#line 991 "common.w"
static int section_name_cmp(char**,size_t,name_pointer);

/*:64*//*76:*/
#line 1186 "common.w"
static void scan_args(void);

/*:76*/
#line 67 "common.w"


/*:1*//*20:*/
#line 95 "common.w"

void
common_init(void)
{
/*45:*/
#line 605 "common.w"

name_dir->byte_start= byte_ptr= byte_mem;
name_ptr= name_dir+1;
name_ptr->byte_start= byte_mem;
root= NULL;

/*:45*//*47:*/
#line 628 "common.w"

for(h= hash;h<=hash_end;*h++= NULL);

/*:47*/
#line 99 "common.w"

/*74:*/
#line 1139 "common.w"

show_banner= show_happiness= show_progress= make_xrefs= true;

/*:74*/
#line 100 "common.w"

/*84:*/
#line 1276 "common.w"

scan_args();
if(program==ctangle){
if((C_file= fopen(C_file_name,"wb"))==NULL)
fatal("! Cannot open output file ",C_file_name);

}
else{
if((tex_file= fopen(tex_file_name,"wb"))==NULL)
fatal("! Cannot open output file ",tex_file_name);
}

/*:84*/
#line 101 "common.w"

}

/*:20*//*23:*/
#line 156 "common.w"

static boolean input_ln(
FILE*fp)
{
register int c= EOF;
register char*k;
if(feof(fp))return false;
limit= k= buffer;
while(k<=buffer_end&&(c= getc(fp))!=EOF&&c!='\n')
if((*(k++)= c)!=' ')limit= k;
if(k> buffer_end)
if((c= getc(fp))!=EOF&&c!='\n'){
ungetc(c,fp);loc= buffer;err_print("! Input line too long");

}
if(c==EOF&&limit==buffer)return false;

return true;
}

/*:23*//*27:*/
#line 227 "common.w"

static void
prime_the_change_buffer(void)
{
change_limit= change_buffer;
/*29:*/
#line 243 "common.w"

while(true){
change_line++;
if(!input_ln(change_file))return;
if(limit<buffer+2)continue;
if(buffer[0]!='@')continue;
if(xisupper(buffer[1]))buffer[1]= tolower((int)buffer[1]);
if(buffer[1]=='x')break;
if(buffer[1]=='y'||buffer[1]=='z'||buffer[1]=='i'){
loc= buffer+2;
err_print("! Missing @x in change file");

}
}

/*:29*/
#line 232 "common.w"

/*30:*/
#line 260 "common.w"

do{
change_line++;
if(!input_ln(change_file)){
err_print("! Change file ended after @x");

return;
}
}while(limit==buffer);

/*:30*/
#line 233 "common.w"

/*31:*/
#line 270 "common.w"

change_limit= change_buffer+(ptrdiff_t)(limit-buffer);
strncpy(change_buffer,buffer,(size_t)(limit-buffer+1));

/*:31*/
#line 234 "common.w"

}

/*:27*//*32:*/
#line 296 "common.w"

static void
check_change(void)
{
int n= 0;
if(lines_dont_match)return;
change_pending= false;
if(!changed_section[section_count]){
if_section_start_make_pending(true);
if(!change_pending)changed_section[section_count]= true;
}
while(true){
changing= print_where= true;change_line++;
if(!input_ln(change_file)){
err_print("! Change file ended before @y");

change_limit= change_buffer;changing= false;
return;
}
if(limit> buffer+1&&buffer[0]=='@'){
char xyz_code= xisupper(buffer[1])?tolower((int)buffer[1]):buffer[1];
/*34:*/
#line 336 "common.w"

if(xyz_code=='x'||xyz_code=='z'){
loc= buffer+2;err_print("! Where is the matching @y?");

}
else if(xyz_code=='y'){
if(n> 0){
loc= buffer+2;
printf("\n! Hmm... %d ",n);
err_print("of the preceding lines failed to match");

}
change_depth= include_depth;
return;
}

/*:34*/
#line 318 "common.w"

}
/*31:*/
#line 270 "common.w"

change_limit= change_buffer+(ptrdiff_t)(limit-buffer);
strncpy(change_buffer,buffer,(size_t)(limit-buffer+1));

/*:31*/
#line 320 "common.w"

changing= false;cur_line++;
while(!input_ln(cur_file)){
if(include_depth==0){
err_print("! CWEB file ended during a change");

input_has_ended= true;return;
}
include_depth--;cur_line++;
}
if(lines_dont_match)n++;
}
}

/*:32*//*35:*/
#line 356 "common.w"

void
reset_input(void)
{
limit= buffer;loc= buffer+1;buffer[0]= ' ';
/*36:*/
#line 371 "common.w"

if((web_file= fopen(web_file_name,"r"))==NULL){
strcpy(web_file_name,alt_web_file_name);
if((web_file= fopen(web_file_name,"r"))==NULL)
fatal("! Cannot open input file ",web_file_name);
}


web_file_open= true;
if((change_file= fopen(change_file_name,"r"))==NULL)
fatal("! Cannot open change file ",change_file_name);

/*:36*/
#line 361 "common.w"

include_depth= cur_line= change_line= 0;
change_depth= include_depth;
changing= true;prime_the_change_buffer();changing= !changing;
limit= buffer;loc= buffer+1;buffer[0]= ' ';input_has_ended= false;
}

/*:35*//*38:*/
#line 400 "common.w"

boolean get_line(void)
{
restart:
if(changing&&include_depth==change_depth)
/*41:*/
#line 508 "common.w"
{
change_line++;
if(!input_ln(change_file)){
err_print("! Change file ended without @z");

buffer[0]= '@';buffer[1]= 'z';limit= buffer+2;
}
if(limit> buffer){
if(change_pending){
if_section_start_make_pending(false);
if(change_pending){
changed_section[section_count]= true;change_pending= false;
}
}
*limit= ' ';
if(buffer[0]=='@'){
if(xisupper(buffer[1]))buffer[1]= tolower((int)buffer[1]);
if(buffer[1]=='x'||buffer[1]=='y'){
loc= buffer+2;
err_print("! Where is the matching @z?");

}
else if(buffer[1]=='z'){
prime_the_change_buffer();changing= !changing;print_where= true;
}
}
}
}

/*:41*/
#line 405 "common.w"

if(!changing||include_depth> change_depth){
/*40:*/
#line 491 "common.w"
{
cur_line++;
while(!input_ln(cur_file)){
print_where= true;
if(include_depth==0){input_has_ended= true;break;}
else{
fclose(cur_file);include_depth--;
if(changing&&include_depth==change_depth)break;
cur_line++;
}
}
if(!changing&&!input_has_ended)
if(limit-buffer==change_limit-change_buffer)
if(buffer[0]==change_buffer[0])
if(change_limit> change_buffer)check_change();
}

/*:40*/
#line 407 "common.w"

if(changing&&include_depth==change_depth)goto restart;
}
if(input_has_ended)return false;
loc= buffer;*limit= ' ';
if(buffer[0]=='@'&&(buffer[1]=='i'||buffer[1]=='I')){
loc= buffer+2;*limit= '"';
while(*loc==' '||*loc=='\t')loc++;
if(loc>=limit){
err_print("! Include file name not given");

goto restart;
}
if(include_depth>=max_include_depth-1){
err_print("! Too many nested includes");

goto restart;
}
include_depth++;
/*39:*/
#line 445 "common.w"
{
char temp_file_name[max_file_name_length];
char*cur_file_name_end= cur_file_name+max_file_name_length-1;
char*kk,*k= cur_file_name;
size_t l;

if(*loc=='"'){
loc++;
while(*loc!='"'&&k<=cur_file_name_end)*k++= *loc++;
if(loc==limit)k= cur_file_name_end+1;
}else
while(*loc!=' '&&*loc!='\t'&&*loc!='"'&&k<=cur_file_name_end)*k++= *loc++;
if(k> cur_file_name_end)too_long();

*k= '\0';
if((cur_file= fopen(cur_file_name,"r"))!=NULL){
cur_line= 0;print_where= true;
goto restart;
}
if((kk= getenv("CWEBINPUTS"))!=NULL){

if((l= strlen(kk))> max_file_name_length-2)too_long();
strcpy(temp_file_name,kk);
}
else{
#ifdef CWEBINPUTS
if((l= strlen(CWEBINPUTS))> max_file_name_length-2)too_long();
strcpy(temp_file_name,CWEBINPUTS);
#else
l= 0;
#endif 
}
if(l> 0){
if(k+l+2>=cur_file_name_end)too_long();

for(;k>=cur_file_name;k--)*(k+l+1)= *k;
strcpy(cur_file_name,temp_file_name);
cur_file_name[l]= '/';
if((cur_file= fopen(cur_file_name,"r"))!=NULL){
cur_line= 0;print_where= true;
goto restart;
}
}
include_depth--;err_print("! Cannot open include file");goto restart;
}

/*:39*/
#line 426 "common.w"

}
return true;
}

/*:38*//*42:*/
#line 540 "common.w"

void
check_complete(void){
if(change_limit!=change_buffer){
strncpy(buffer,change_buffer,(size_t)(change_limit-change_buffer+1));
limit= buffer+(ptrdiff_t)(change_limit-change_buffer);
changing= true;change_depth= include_depth;loc= buffer;
err_print("! Change file entry did not match");

}
}

/*:42*//*48:*/
#line 633 "common.w"

name_pointer
id_lookup(
const char*first,
const char*last,
eight_bits t)
{
const char*i= first;
int h;
size_t l;
name_pointer p;
if(last==NULL)for(last= first;*last!='\0';last++);
l= (size_t)(last-first);
/*49:*/
#line 656 "common.w"

h= (int)((eight_bits)*i);
while(++i<last)h= (h+h+(int)((eight_bits)*i))%hash_size;


/*:49*/
#line 646 "common.w"

/*50:*/
#line 664 "common.w"

p= hash[h];
while(p&&!names_match(p,first,l,t))p= p->link;
if(p==NULL){
p= name_ptr;
p->link= hash[h];hash[h]= p;
}

/*:50*/
#line 647 "common.w"

if(p==name_ptr)/*51:*/
#line 676 "common.w"
{
if(byte_ptr+l> byte_mem_end)overflow("byte memory");
if(name_ptr>=name_dir_end)overflow("name");
strncpy(byte_ptr,first,l);
(++name_ptr)->byte_start= byte_ptr+= l;
init_p(p,t);
}

/*:51*/
#line 648 "common.w"

return p;
}

/*:48*//*52:*/
#line 708 "common.w"

void
print_section_name(
name_pointer p)
{
char*ss,*s= first_chunk(p);
name_pointer q= p+1;
while(p!=name_dir){
ss= (p+1)->byte_start-1;
if(*ss==' '&&ss>=s)p= q->link,q= p;
else ss++,p= name_dir,q= NULL;
term_write(s,(size_t)(ss-s));
s= p->byte_start;
}
if(q)term_write("...",3);
}

/*:52*//*53:*/
#line 725 "common.w"

void
sprint_section_name(
char*dest,
name_pointer p)
{
char*ss,*s= first_chunk(p);
name_pointer q= p+1;
while(p!=name_dir){
ss= (p+1)->byte_start-1;
if(*ss==' '&&ss>=s)p= q->link,q= p;
else ss++,p= name_dir;
strncpy(dest,s,(size_t)(ss-s)),dest+= ss-s;
s= p->byte_start;
}
*dest= '\0';
}

/*:53*//*54:*/
#line 743 "common.w"

void
print_prefix_name(
name_pointer p)
{
char*s= first_chunk(p);
size_t l= prefix_length(p);
term_write(s,l);
if(s+l<(p+1)->byte_start)term_write("...",3);
}

/*:54*//*56:*/
#line 769 "common.w"

static int web_strcmp(
char*j,
size_t j_len,
char*k,
size_t k_len)
{
char*j1= j+j_len,*k1= k+k_len;
while(k<k1&&j<j1&&*j==*k)k++,j++;
if(k==k1)if(j==j1)return equal;
else return extension;
else if(j==j1)return prefix;
else if(*j<*k)return less;
else return greater;
}

/*:56*//*57:*/
#line 798 "common.w"

static name_pointer
add_section_name(
name_pointer par,
int c,
char*first,
char*last,
boolean ispref)
{
name_pointer p= name_ptr;
char*s= first_chunk(p);
size_t name_len= (size_t)(last-first+(int)ispref);
if(s+name_len> byte_mem_end)overflow("byte memory");
if(name_ptr+1>=name_dir_end)overflow("name");
(++name_ptr)->byte_start= byte_ptr= s+name_len;
if(ispref){
*(byte_ptr-1)= ' ';
name_len--;
name_ptr->link= name_dir;
(++name_ptr)->byte_start= byte_ptr;
}
set_prefix_length(p,name_len);
strncpy(s,first,name_len);
p->llink= p->rlink= NULL;
init_node(p);
return par==NULL?(root= p):c==less?(par->llink= p):(par->rlink= p);
}

/*:57*//*58:*/
#line 826 "common.w"

static void
extend_section_name(
name_pointer p,
char*first,
char*last,
boolean ispref)
{
char*s;
name_pointer q= p+1;
size_t name_len= (size_t)(last-first+(int)ispref);
if(name_ptr>=name_dir_end)overflow("name");
while(q->link!=name_dir)q= q->link;
q->link= name_ptr;
s= name_ptr->byte_start;
name_ptr->link= name_dir;
if(s+name_len> byte_mem_end)overflow("byte memory");
(++name_ptr)->byte_start= byte_ptr= s+name_len;
strncpy(s,first,name_len);
if(ispref)*(byte_ptr-1)= ' ';
}

/*:58*//*59:*/
#line 854 "common.w"

name_pointer
section_lookup(
char*first,char*last,
boolean ispref)
{
int c= less;
name_pointer p= root;
name_pointer q= NULL;
name_pointer r= NULL;
name_pointer par= NULL;

size_t name_len= (size_t)(last-first+1);
/*60:*/
#line 878 "common.w"

while(p){
c= web_strcmp(first,name_len,first_chunk(p),prefix_length(p));
if(c==less||c==greater){
if(r==NULL)
par= p;
p= (c==less?p->llink:p->rlink);
}else{
if(r!=NULL){
fputs("\n! Ambiguous prefix: matches <",stdout);

print_prefix_name(p);
fputs(">\n and <",stdout);
print_prefix_name(r);
err_print(">");
return name_dir;
}
r= p;
p= p->llink;
q= r->rlink;
}
if(p==NULL)
p= q,q= NULL;
}

/*:60*/
#line 868 "common.w"

/*61:*/
#line 903 "common.w"

if(r==NULL)
return add_section_name(par,c,first,last+1,ispref);

/*:61*/
#line 869 "common.w"

/*62:*/
#line 911 "common.w"

switch(section_name_cmp(&first,name_len,r)){

case prefix:
if(!ispref){
fputs("\n! New name is a prefix of <",stdout);

print_section_name(r);
err_print(">");
}
else if(name_len<prefix_length(r))set_prefix_length(r,name_len);

case equal:break;
case extension:if(!ispref||first<=last)
extend_section_name(r,first,last+1,ispref);
break;
case bad_extension:
fputs("\n! New name extends <",stdout);

print_section_name(r);
err_print(">");
break;
default:
fputs("\n! Section name incompatible with <",stdout);

print_prefix_name(r);
fputs(">,\n which abbreviates <",stdout);
print_section_name(r);
err_print(">");
}
return r;

/*:62*/
#line 870 "common.w"

}

/*:59*//*63:*/
#line 959 "common.w"

static int section_name_cmp(
char**pfirst,
size_t len,
name_pointer r)
{
char*first= *pfirst;
name_pointer q= r+1;
char*ss,*s= first_chunk(r);
int c= less;
boolean ispref;
while(true){
ss= (r+1)->byte_start-1;
if(*ss==' '&&ss>=r->byte_start)ispref= true,q= q->link;
else ispref= false,ss++,q= name_dir;
switch(c= web_strcmp(first,len,s,(size_t)(ss-s))){
case equal:if(q==name_dir)
if(ispref){
*pfirst= first+(ptrdiff_t)(ss-s);
return extension;
}else return equal;
else return(q->byte_start==(q+1)->byte_start)?equal:prefix;
case extension:
if(!ispref)return bad_extension;
first+= ss-s;
if(q!=name_dir){len-= (int)(ss-s);s= q->byte_start;r= q;continue;}
*pfirst= first;return extension;
default:return c;
}
}
}

/*:63*//*66:*/
#line 1013 "common.w"

void
err_print(
const char*s)
{
*s=='!'?printf("\n%s",s):printf("%s",s);
if(web_file_open)/*67:*/
#line 1032 "common.w"

{char*k,*l;
if(changing&&include_depth==change_depth)
printf(". (l. %d of change file)\n",change_line);
else if(include_depth==0)printf(". (l. %d)\n",cur_line);
else printf(". (l. %d of include file %s)\n",cur_line,cur_file_name);
l= (loc>=limit?limit:loc);
if(l> buffer){
for(k= buffer;k<l;k++)
if(*k=='\t')putchar(' ');
else putchar(*k);
new_line;
for(k= buffer;k<l;k++)putchar(' ');
}
for(k= l;k<limit;k++)putchar(*k);
if(*limit=='|')putchar('|');
putchar(' ');
}

/*:67*/
#line 1019 "common.w"

update_terminal;mark_error;
}

/*:66*//*68:*/
#line 1065 "common.w"

int wrap_up(void){
if(show_progress)new_line;
if(show_stats)
print_stats();
/*69:*/
#line 1075 "common.w"

switch(history){
case spotless:
if(show_happiness)puts("(No errors were found.)");break;
case harmless_message:
puts("(Did you see the warning message above?)");break;
case error_message:
puts("(Pardon me, but I think I spotted something wrong.)");break;
case fatal_message:default:
puts("(That was a fatal error, my friend.)");
}

/*:69*/
#line 1070 "common.w"

if(history> harmless_message)return EXIT_FAILURE;
else return EXIT_SUCCESS;
}

/*:68*//*70:*/
#line 1093 "common.w"
void
fatal(
const char*s,const char*t)
{
if(*s)err_print(s);
err_print(t);
history= fatal_message;exit(wrap_up());
}

/*:70*//*71:*/
#line 1104 "common.w"
void
overflow(
const char*t)
{
printf("\n! Sorry, %s capacity exceeded",t);fatal("","");
}


/*:71*//*75:*/
#line 1157 "common.w"

static void
scan_args(void)
{
char*dot_pos;
char*name_pos;
register char*s;
boolean found_web= false,found_change= false,found_out= false;


strcpy(change_file_name,"/dev/null");
while(--argc> 0){
if((**(++argv)=='-'||**argv=='+')&&*(*argv+1))/*80:*/
#line 1247 "common.w"

for(dot_pos= *argv+1;*dot_pos> '\0';dot_pos++)
flags[(eight_bits)*dot_pos]= flag_change;

/*:80*/
#line 1169 "common.w"

else{
s= name_pos= *argv;dot_pos= NULL;
while(*s)
if(*s=='.')dot_pos= s++;
else if(*s=='/')dot_pos= NULL,name_pos= ++s;
else s++;
if(!found_web)/*77:*/
#line 1195 "common.w"

{
if(s-*argv> max_file_name_length-5)
/*82:*/
#line 1263 "common.w"
fatal("! Filename too long\n",*argv);


/*:82*/
#line 1198 "common.w"

if(dot_pos==NULL)
sprintf(web_file_name,"%s.w",*argv);
else{
strcpy(web_file_name,*argv);
*dot_pos= '\0';
}
sprintf(alt_web_file_name,"%s.web",*argv);
sprintf(tex_file_name,"%s.tex",name_pos);
sprintf(idx_file_name,"%s.idx",name_pos);
sprintf(scn_file_name,"%s.scn",name_pos);
sprintf(C_file_name,"%s.c",name_pos);
found_web= true;
}

/*:77*/
#line 1177 "common.w"

else if(!found_change)/*78:*/
#line 1213 "common.w"

{
if(strcmp(*argv,"-")!=0){
if(s-*argv> max_file_name_length-4)
/*82:*/
#line 1263 "common.w"
fatal("! Filename too long\n",*argv);


/*:82*/
#line 1217 "common.w"

if(dot_pos==NULL)
sprintf(change_file_name,"%s.ch",*argv);
else strcpy(change_file_name,*argv);
}
found_change= true;
}

/*:78*/
#line 1178 "common.w"

else if(!found_out)/*79:*/
#line 1225 "common.w"

{
if(s-*argv> max_file_name_length-5)
/*82:*/
#line 1263 "common.w"
fatal("! Filename too long\n",*argv);


/*:82*/
#line 1228 "common.w"

if(dot_pos==NULL){
sprintf(tex_file_name,"%s.tex",*argv);
sprintf(idx_file_name,"%s.idx",*argv);
sprintf(scn_file_name,"%s.scn",*argv);
sprintf(C_file_name,"%s.c",*argv);
}else{
strcpy(tex_file_name,*argv);
strcpy(C_file_name,*argv);
if(make_xrefs){
*dot_pos= '\0';
sprintf(idx_file_name,"%s.idx",*argv);
sprintf(scn_file_name,"%s.scn",*argv);
}
}
found_out= true;
}

/*:79*/
#line 1179 "common.w"

else/*81:*/
#line 1251 "common.w"

{
if(program==ctangle)
fatal(
"! Usage: ctangle [options] webfile[.w] [{changefile[.ch]|-} [outfile[.c]]]\n"
,"");

else fatal(
"! Usage: cweave [options] webfile[.w] [{changefile[.ch]|-} [outfile[.tex]]]\n"
,"");
}

/*:81*/
#line 1180 "common.w"

}
}
if(!found_web)/*81:*/
#line 1251 "common.w"

{
if(program==ctangle)
fatal(
"! Usage: ctangle [options] webfile[.w] [{changefile[.ch]|-} [outfile[.c]]]\n"
,"");

else fatal(
"! Usage: cweave [options] webfile[.w] [{changefile[.ch]|-} [outfile[.tex]]]\n"
,"");
}

/*:81*/
#line 1183 "common.w"

}

/*:75*/
