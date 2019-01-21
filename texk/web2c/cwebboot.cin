/*1:*/
#line 58 "cwebdir/common.w"

/*5:*/
#line 102 "cwebdir/common.w"

#include <ctype.h> 

/*:5*//*8:*/
#line 165 "cwebdir/common.w"

#include <stdio.h> 

/*:8*//*22:*/
#line 471 "cwebdir/common.w"

#include <stdlib.h>  

/*:22*//*81:*/
#line 1123 "cwebdir/comm-w2c.ch"

#include <string.h> 
#line 1417 "cwebdir/common.w"

#line 1130 "cwebdir/comm-w2c.ch"
/*:81*//*85:*/
#line 1169 "cwebdir/comm-w2c.ch"

#include <stdbool.h>  
#include <stddef.h>  
#include <stdint.h>  

/*:85*//*88:*/
#line 1211 "cwebdir/comm-w2c.ch"

#ifndef HAVE_GETTEXT
#define HAVE_GETTEXT 0
#endif

#if HAVE_GETTEXT
#include <locale.h> 
#include <libintl.h> 
#else
#define setlocale(A,B) ""
#define bindtextdomain(A,B) ""
#define textdomain(A) ""
#define gettext(A) A
#endif

/*:88*//*90:*/
#line 1301 "cwebdir/comm-w2c.ch"

typedef bool boolean;
#define HAVE_BOOLEAN
#include <kpathsea/kpathsea.h>  
#include <w2c/config.h>  
#include <lib/lib.h>  

#define CWEB
#include "help.h"

/*:90*/
#line 59 "cwebdir/common.w"

#define ctangle 0
#define cweave 1
#define ctwill 2 \

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

#define buf_size 1000
#define longest_name 10000
#define long_buf_size (buf_size+longest_name) 
#define xisspace(c) (isspace((eight_bits) c) &&((eight_bits) c<0200) ) 
#define xisupper(c) (isupper((eight_bits) c) &&((eight_bits) c<0200) )  \

#define max_include_depth 10 \

#define max_file_name_length 1024
#define cur_file file[include_depth]
#define cur_file_name file_name[include_depth]
#define cur_line line[include_depth]
#define web_file file[0]
#define web_file_name file_name[0] \

#define lines_dont_match (change_limit-change_buffer!=limit-buffer|| \
strncmp(buffer,change_buffer,(size_t) (limit-buffer) ) )  \

#define if_section_start_make_pending(b) {*limit= '!'; \
for(loc= buffer;xisspace(*loc) ;loc++) ; \
*limit= ' '; \
if(*loc=='@'&&(xisspace(*(loc+1) ) ||*(loc+1) =='*') ) change_pending= b; \
} \

#define max_sections 10239 \
 \

#define too_long() {include_depth--; \
err_print(_("! Include file name too long") ) ;goto restart;} \

#define max_bytes 1000000 \

#define max_names 10239 \
 \

#define length(c) (size_t) ((c+1) ->byte_start-(c) ->byte_start) 
#define print_id(c) term_write((c) ->byte_start,length((c) ) )  \

#define hash_size 8501 \

#define llink link
#define rlink dummy.Rlink
#define root name_dir->rlink \
 \

#define first_chunk(p) ((p) ->byte_start+2) 
#define prefix_length(p) (int) ((unsigned char) *((p) ->byte_start) *256+ \
(unsigned char) *((p) ->byte_start+1) ) 
#define set_prefix_length(p,m) (*((p) ->byte_start) = (m) /256, \
*((p) ->byte_start+1) = (m) %256)  \

#define less 0
#define equal 1
#define greater 2
#define prefix 3
#define extension 4 \

#define bad_extension 5 \

#define spotless 0
#define harmless_message 1
#define error_message 2
#define fatal_message 3
#define mark_harmless {if(history==spotless) history= harmless_message;}
#define mark_error history= error_message \

#define RETURN_OK 0
#define RETURN_WARN 5
#define RETURN_ERROR 10
#define RETURN_FAIL 20 \

#define confusion(s) fatal(_("! This can't happen: ") ,s)  \
 \

#define show_banner flags['b']
#define show_progress flags['p']
#define show_stats flags['s']
#define show_happiness flags['h']
#define make_xrefs flags['x'] \

#define flag_change (**argv!='-') 
#define update_terminal fflush(stdout)  \

#define new_line putchar('\n') 
#define putxchar putchar
#define term_write(a,b) fflush(stdout) ,fwrite(a,sizeof(char) ,b,stdout) 
#define C_printf(c,a) fprintf(C_file,c,a) 
#define C_putc(c) putc(c,C_file)  \

#define max_banner 50 \

#define PATH_SEPARATOR separators[0]
#define DIR_SEPARATOR separators[1]
#define DEVICE_SEPARATOR separators[2] \

#define _(STRING) gettext(STRING)  \

#define kpse_find_cweb(name) kpse_find_file(name,kpse_cweb_format,true)  \


#line 60 "cwebdir/common.w"

/*2:*/
#line 73 "cwebdir/common.w"

#line 56 "cwebdir/comm-w2c.ch"
int program;
#line 76 "cwebdir/common.w"

/*:2*//*7:*/
#line 159 "cwebdir/common.w"

char buffer[long_buf_size];
char*buffer_end= buffer+buf_size-2;
char*limit= buffer;
char*loc= buffer;

/*:7*//*10:*/
#line 214 "cwebdir/common.w"

int include_depth;
FILE*file[max_include_depth];
FILE*change_file;
char file_name[max_include_depth][max_file_name_length];

char change_file_name[max_file_name_length];
#line 222 "cwebdir/common.w"
int line[max_include_depth];
int change_line;
int change_depth;
boolean input_has_ended;
boolean changing;
boolean web_file_open= 0;

/*:10*//*20:*/
#line 418 "cwebdir/common.w"

#line 264 "cwebdir/comm-w2c.ch"
typedef uint8_t eight_bits;
typedef uint16_t sixteen_bits;
#line 420 "cwebdir/common.w"
sixteen_bits section_count;
boolean changed_section[max_sections];
boolean change_pending;

boolean print_where= 0;

/*:20*//*27:*/
#line 594 "cwebdir/common.w"

typedef struct name_info{
char*byte_start;
/*31:*/
#line 631 "cwebdir/common.w"

struct name_info*link;

/*:31*//*40:*/
#line 730 "cwebdir/common.w"

union{
struct name_info*Rlink;

char Ilk;
}dummy;

/*:40*//*55:*/
#line 703 "cwebdir/comm-w2c.ch"

void*equiv_or_xref;
#line 1064 "cwebdir/common.w"

/*:55*/
#line 597 "cwebdir/common.w"

}name_info;
typedef name_info*name_pointer;
char byte_mem[max_bytes];
char*byte_mem_end= byte_mem+max_bytes-1;
name_info name_dir[max_names];
name_pointer name_dir_end= name_dir+max_names-1;

/*:27*//*29:*/
#line 617 "cwebdir/common.w"

name_pointer name_ptr;
char*byte_ptr;

/*:29*//*32:*/
#line 644 "cwebdir/common.w"

typedef name_pointer*hash_pointer;
name_pointer hash[hash_size];
hash_pointer hash_end= hash+hash_size-1;
hash_pointer h;

/*:32*//*56:*/
#line 1082 "cwebdir/common.w"

int history= spotless;

/*:56*//*67:*/
#line 1220 "cwebdir/common.w"

int argc;
char**argv;
char C_file_name[max_file_name_length];
char tex_file_name[max_file_name_length];
char idx_file_name[max_file_name_length];
#line 863 "cwebdir/comm-w2c.ch"
char scn_file_name[max_file_name_length];
char check_file_name[max_file_name_length];
#line 870 "cwebdir/comm-w2c.ch"
boolean flags[128];
const char*use_language= "";
#line 1228 "cwebdir/common.w"

/*:67*//*77:*/
#line 1370 "cwebdir/common.w"

FILE*C_file;
FILE*tex_file;
FILE*idx_file;
#line 1056 "cwebdir/comm-w2c.ch"
FILE*scn_file;
FILE*check_file;
#line 1063 "cwebdir/comm-w2c.ch"
FILE*active_file;
char*found_filename;
#line 1376 "cwebdir/common.w"

#line 1080 "cwebdir/comm-w2c.ch"
/*:77*/
#line 61 "cwebdir/common.w"

/*3:*/
#line 83 "cwebdir/common.w"
int phase;

/*:3*//*11:*/
#line 240 "cwebdir/common.w"

char change_buffer[buf_size];
char*change_limit;

/*:11*//*86:*/
#line 1185 "cwebdir/comm-w2c.ch"

char cb_banner[max_banner];
string texmf_locale;
#ifndef SEPARATORS
#define SEPARATORS "://"
#endif
char separators[]= SEPARATORS;

/*:86*/
#line 62 "cwebdir/common.w"

/*33:*/
#line 650 "cwebdir/common.w"

#line 432 "cwebdir/comm-w2c.ch"
extern boolean names_match(name_pointer,const char*,size_t,eight_bits);
#line 652 "cwebdir/common.w"

/*:33*//*38:*/
#line 703 "cwebdir/common.w"

#line 462 "cwebdir/comm-w2c.ch"
extern void init_p(name_pointer,eight_bits);
#line 705 "cwebdir/common.w"

/*:38*//*46:*/
#line 852 "cwebdir/common.w"

#line 542 "cwebdir/comm-w2c.ch"
extern void init_node(name_pointer);
#line 854 "cwebdir/common.w"

/*:46*//*53:*/
#line 1017 "cwebdir/common.w"

#line 668 "cwebdir/comm-w2c.ch"
static int section_name_cmp(char**,int,name_pointer);
#line 1019 "cwebdir/common.w"

/*:53*//*57:*/
#line 1092 "cwebdir/common.w"

#line 710 "cwebdir/comm-w2c.ch"
extern void err_print(const char*);
#line 1094 "cwebdir/common.w"

/*:57*//*60:*/
#line 1140 "cwebdir/common.w"

#line 737 "cwebdir/comm-w2c.ch"
extern int wrap_up(void);
extern void print_stats(void);
#line 1143 "cwebdir/common.w"

#line 748 "cwebdir/comm-w2c.ch"
/*:60*//*63:*/
#line 1173 "cwebdir/common.w"

#line 811 "cwebdir/comm-w2c.ch"
extern void fatal(const char*,const char*);
extern void overflow(const char*);
#line 1175 "cwebdir/common.w"

/*:63*//*69:*/
#line 1251 "cwebdir/common.w"

#line 891 "cwebdir/comm-w2c.ch"
static void scan_args(void);
#line 1253 "cwebdir/common.w"

/*:69*//*83:*/
#line 1145 "cwebdir/comm-w2c.ch"

boolean get_line(void);
name_pointer id_lookup(const char*,const char*,char);
name_pointer section_lookup(char*,char*,int);
void check_complete(void);
void common_init(void);
void print_prefix_name(name_pointer);
void print_section_name(name_pointer);
void reset_input(void);
void sprint_section_name(char*,name_pointer);

/*:83*//*84:*/
#line 1158 "cwebdir/comm-w2c.ch"

static boolean input_ln(FILE*);
static int web_strcmp(char*,int,char*,int);
static name_pointer add_section_name(name_pointer,int,char*,char*,int);
static void extend_section_name(name_pointer,char*,char*,int);
static void check_change(void);
static void prime_the_change_buffer(void);

/*:84*//*98:*/
#line 1402 "cwebdir/comm-w2c.ch"

static void cb_usage(const_string str);
static void cb_usagehelp(const_string*message,const_string bug_email);
void cb_show_banner(void);

/*:98*/
#line 63 "cwebdir/common.w"


/*:1*//*4:*/
#line 89 "cwebdir/common.w"

#line 63 "cwebdir/comm-w2c.ch"
void
common_init(void)
#line 92 "cwebdir/common.w"
{
#line 72 "cwebdir/comm-w2c.ch"
/*30:*/
#line 621 "cwebdir/common.w"

name_dir->byte_start= byte_ptr= byte_mem;
name_ptr= name_dir+1;
name_ptr->byte_start= byte_mem;

/*:30*//*34:*/
#line 655 "cwebdir/common.w"

for(h= hash;h<=hash_end;*h++= NULL);

/*:34*//*41:*/
#line 737 "cwebdir/common.w"

root= NULL;

/*:41*/
#line 72 "cwebdir/comm-w2c.ch"

/*91:*/
#line 1316 "cwebdir/comm-w2c.ch"

kpse_set_program_name(argv[0],"cweb");

/*:91*/
#line 73 "cwebdir/comm-w2c.ch"

/*89:*/
#line 1267 "cwebdir/comm-w2c.ch"

setlocale(LC_MESSAGES,setlocale(LC_CTYPE,""));
texmf_locale= kpse_var_expand("${TEXMFLOCALEDIR}");

bindtextdomain("cweb",
bindtextdomain("cweb-tl",
bindtextdomain("web2c-help",
strcmp(texmf_locale,"")?
texmf_locale:"/usr/share/locale")));

free(texmf_locale);
textdomain("cweb");


/*:89*/
#line 74 "cwebdir/comm-w2c.ch"

/*68:*/
#line 1233 "cwebdir/common.w"

#line 1235 "cwebdir/common.w"

/*:68*/
#line 75 "cwebdir/comm-w2c.ch"

/*78:*/
#line 1080 "cwebdir/comm-w2c.ch"

scan_args();
if(program==ctangle){
strcpy(check_file_name,C_file_name);
if(check_file_name[0]!='\0'){
char*dot_pos= strrchr(check_file_name,'.');
if(dot_pos==NULL)strcat(check_file_name,".ttp");
else strcpy(dot_pos,".ttp");
}
if((C_file= fopen(check_file_name,"wb"))==NULL)
fatal(_("! Cannot open output file "),check_file_name);

}
else{
strcpy(check_file_name,tex_file_name);
if(check_file_name[0]!='\0'){
char*dot_pos= strrchr(check_file_name,'.');
if(dot_pos==NULL)strcat(check_file_name,".wtp");
else strcpy(dot_pos,".wtp");
}
if((tex_file= fopen(check_file_name,"wb"))==NULL)
fatal(_("! Cannot open output file "),check_file_name);
}
#line 1388 "cwebdir/common.w"

/*:78*/
#line 76 "cwebdir/comm-w2c.ch"

#line 96 "cwebdir/common.w"
}

/*:4*//*9:*/
#line 172 "cwebdir/common.w"

#line 103 "cwebdir/comm-w2c.ch"
static boolean input_ln(
FILE*fp)
#line 175 "cwebdir/common.w"
{
register int c= EOF;
register char*k;
if(feof(fp))return(0);
limit= k= buffer;
while(k<=buffer_end&&(c= getc(fp))!=EOF&&c!='\n')
#line 110 "cwebdir/comm-w2c.ch"
if((*(k++)= c)!=' '&&c!='\r')limit= k;
#line 182 "cwebdir/common.w"
if(k> buffer_end)
if((c= getc(fp))!=EOF&&c!='\n'){
#line 116 "cwebdir/comm-w2c.ch"
ungetc(c,fp);loc= buffer;err_print(_("! Input line too long"));
#line 185 "cwebdir/common.w"

}
if(c==EOF&&limit==buffer)return(0);

return(1);
}

/*:9*//*12:*/
#line 251 "cwebdir/common.w"

#line 142 "cwebdir/comm-w2c.ch"
static void
prime_the_change_buffer(void)
#line 254 "cwebdir/common.w"
{
change_limit= change_buffer;
/*13:*/
#line 265 "cwebdir/common.w"

while(1){
change_line++;
if(!input_ln(change_file))return;
if(limit<buffer+2)continue;
if(buffer[0]!='@')continue;
#line 149 "cwebdir/comm-w2c.ch"
if(xisupper(buffer[1]))buffer[1]= tolower((eight_bits)buffer[1]);
#line 272 "cwebdir/common.w"
if(buffer[1]=='x')break;
if(buffer[1]=='y'||buffer[1]=='z'||buffer[1]=='i'){
loc= buffer+2;
#line 155 "cwebdir/comm-w2c.ch"
err_print(_("! Missing @x in change file"));
#line 276 "cwebdir/common.w"

}
}

/*:13*/
#line 256 "cwebdir/common.w"
;
/*14:*/
#line 282 "cwebdir/common.w"

do{
change_line++;
if(!input_ln(change_file)){
#line 161 "cwebdir/comm-w2c.ch"
err_print(_("! Change file ended after @x"));
#line 287 "cwebdir/common.w"

return;
}
}while(limit==buffer);

/*:14*/
#line 257 "cwebdir/common.w"
;
/*15:*/
#line 292 "cwebdir/common.w"

{
#line 168 "cwebdir/comm-w2c.ch"
change_limit= change_buffer+(ptrdiff_t)(limit-buffer);
strncpy(change_buffer,buffer,(size_t)(limit-buffer+1));
#line 296 "cwebdir/common.w"
}

/*:15*/
#line 258 "cwebdir/common.w"
;
}

/*:12*//*16:*/
#line 320 "cwebdir/common.w"

#line 176 "cwebdir/comm-w2c.ch"
static void
check_change(void)
#line 323 "cwebdir/common.w"
{
int n= 0;
if(lines_dont_match)return;
change_pending= 0;
if(!changed_section[section_count]){
if_section_start_make_pending(1);
if(!change_pending)changed_section[section_count]= 1;
}
while(1){
changing= 1;print_where= 1;change_line++;
if(!input_ln(change_file)){
#line 183 "cwebdir/comm-w2c.ch"
err_print(_("! Change file ended before @y"));
#line 335 "cwebdir/common.w"

change_limit= change_buffer;changing= 0;
return;
}
if(limit> buffer+1&&buffer[0]=='@'){
#line 189 "cwebdir/comm-w2c.ch"
char xyz_code= xisupper(buffer[1])?tolower((eight_bits)buffer[1]):buffer[1];
#line 341 "cwebdir/common.w"
/*17:*/
#line 358 "cwebdir/common.w"

if(xyz_code=='x'||xyz_code=='z'){
#line 201 "cwebdir/comm-w2c.ch"
loc= buffer+2;err_print(_("! Where is the matching @y?"));
#line 361 "cwebdir/common.w"

}
else if(xyz_code=='y'){
if(n> 0){
loc= buffer+2;
printf("\n! Hmm... %d ",n);
#line 207 "cwebdir/comm-w2c.ch"
err_print(_("of the preceding lines failed to match"));
#line 368 "cwebdir/common.w"

}
change_depth= include_depth;
return;
}

/*:17*/
#line 342 "cwebdir/common.w"
;
}
/*15:*/
#line 292 "cwebdir/common.w"

{
#line 168 "cwebdir/comm-w2c.ch"
change_limit= change_buffer+(ptrdiff_t)(limit-buffer);
strncpy(change_buffer,buffer,(size_t)(limit-buffer+1));
#line 296 "cwebdir/common.w"
}

/*:15*/
#line 344 "cwebdir/common.w"
;
changing= 0;cur_line++;
while(!input_ln(cur_file)){
if(include_depth==0){
#line 195 "cwebdir/comm-w2c.ch"
err_print(_("! CWEB file ended during a change"));
#line 349 "cwebdir/common.w"

input_has_ended= 1;return;
}
include_depth--;cur_line++;
}
if(lines_dont_match)n++;
}
}

/*:16*//*18:*/
#line 378 "cwebdir/common.w"

#line 214 "cwebdir/comm-w2c.ch"
void
reset_input(void)
#line 381 "cwebdir/common.w"
{
limit= buffer;loc= buffer+1;buffer[0]= ' ';
/*19:*/
#line 393 "cwebdir/common.w"

#line 225 "cwebdir/comm-w2c.ch"
if((found_filename= kpse_find_cweb(web_file_name))==NULL||
(web_file= fopen(found_filename,"r"))==NULL){
fatal(_("! Cannot open input file "),web_file_name);
}else if(strlen(found_filename)<max_file_name_length){

if(strcmp(web_file_name,found_filename))
strcpy(web_file_name,found_filename+
((strncmp(found_filename,"./",2)==0)?2:0));
free(found_filename);
}else fatal(_("! Filename too long\n"),found_filename);
#line 399 "cwebdir/common.w"


web_file_open= 1;
#line 241 "cwebdir/comm-w2c.ch"
if((found_filename= kpse_find_cweb(change_file_name))==NULL||
(change_file= fopen(found_filename,"r"))==NULL){
fatal(_("! Cannot open change file "),change_file_name);
}else if(strlen(found_filename)<max_file_name_length){

if(strcmp(change_file_name,found_filename))
strcpy(change_file_name,found_filename+
((strncmp(found_filename,"./",2)==0)?2:0));
free(found_filename);
}else fatal(_("! Filename too long\n"),found_filename);
#line 404 "cwebdir/common.w"

/*:19*/
#line 383 "cwebdir/common.w"
;
include_depth= 0;cur_line= 0;change_line= 0;
change_depth= include_depth;
changing= 1;prime_the_change_buffer();changing= !changing;
limit= buffer;loc= buffer+1;buffer[0]= ' ';input_has_ended= 0;
}

/*:18*//*21:*/
#line 426 "cwebdir/common.w"

#line 271 "cwebdir/comm-w2c.ch"
boolean get_line(void)
#line 428 "cwebdir/common.w"
{
restart:
if(changing&&include_depth==change_depth)
/*25:*/
#line 537 "cwebdir/common.w"
{
change_line++;
if(!input_ln(change_file)){
#line 370 "cwebdir/comm-w2c.ch"
err_print(_("! Change file ended without @z"));
#line 541 "cwebdir/common.w"

buffer[0]= '@';buffer[1]= 'z';limit= buffer+2;
}
if(limit> buffer){
if(change_pending){
if_section_start_make_pending(0);
if(change_pending){
changed_section[section_count]= 1;change_pending= 0;
}
}
*limit= ' ';
if(buffer[0]=='@'){
#line 376 "cwebdir/comm-w2c.ch"
if(xisupper(buffer[1]))buffer[1]= tolower((eight_bits)buffer[1]);
#line 554 "cwebdir/common.w"
if(buffer[1]=='x'||buffer[1]=='y'){
loc= buffer+2;
#line 382 "cwebdir/comm-w2c.ch"
err_print(_("! Where is the matching @z?"));
#line 557 "cwebdir/common.w"

}
else if(buffer[1]=='z'){
prime_the_change_buffer();changing= !changing;print_where= 1;
}
}
}
}

/*:25*/
#line 431 "cwebdir/common.w"
;
if(!changing||include_depth> change_depth){
/*24:*/
#line 520 "cwebdir/common.w"
{
cur_line++;
while(!input_ln(cur_file)){
print_where= 1;
if(include_depth==0){input_has_ended= 1;break;}
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

/*:24*/
#line 433 "cwebdir/common.w"
;
if(changing&&include_depth==change_depth)goto restart;
}
if(input_has_ended)return 0;
loc= buffer;*limit= ' ';
if(buffer[0]=='@'&&(buffer[1]=='i'||buffer[1]=='I')){
loc= buffer+2;*limit= '"';
while(*loc==' '||*loc=='\t')loc++;
if(loc>=limit){
#line 277 "cwebdir/comm-w2c.ch"
err_print(_("! Include file name not given"));
#line 443 "cwebdir/common.w"

goto restart;
}
if(include_depth>=max_include_depth-1){
#line 283 "cwebdir/comm-w2c.ch"
err_print(_("! Too many nested includes"));
#line 448 "cwebdir/common.w"

goto restart;
}
include_depth++;
/*23:*/
#line 474 "cwebdir/common.w"
{
#line 319 "cwebdir/comm-w2c.ch"
char*cur_file_name_end= cur_file_name+max_file_name_length-1;
char*k= cur_file_name;
#line 479 "cwebdir/common.w"

if(*loc=='"'){
loc++;
while(*loc!='"'&&k<=cur_file_name_end)*k++= *loc++;
if(loc==limit)k= cur_file_name_end+1;
}else
while(*loc!=' '&&*loc!='\t'&&*loc!='"'&&k<=cur_file_name_end)*k++= *loc++;
if(k> cur_file_name_end)too_long();

*k= '\0';
#line 326 "cwebdir/comm-w2c.ch"
if((found_filename= kpse_find_cweb(cur_file_name))!=NULL&&
(cur_file= fopen(found_filename,"r"))!=NULL){

if(strlen(found_filename)<max_file_name_length){
if(strcmp(cur_file_name,found_filename))
strcpy(cur_file_name,found_filename+
((strncmp(found_filename,"./",2)==0)?2:0));
free(found_filename);
}else fatal(_("! Filename too long\n"),found_filename);
#line 490 "cwebdir/common.w"
cur_line= 0;print_where= 1;
goto restart;
}
#line 364 "cwebdir/comm-w2c.ch"
include_depth--;err_print(_("! Cannot open include file"));goto restart;
#line 518 "cwebdir/common.w"
}

/*:23*/
#line 452 "cwebdir/common.w"
;
}
return 1;
}

#line 298 "cwebdir/comm-w2c.ch"
/*:21*//*26:*/
#line 569 "cwebdir/common.w"

#line 392 "cwebdir/comm-w2c.ch"
void
check_complete(void){
if(change_limit!=change_buffer){
strncpy(buffer,change_buffer,(size_t)(change_limit-change_buffer+1));
limit= buffer+(ptrdiff_t)(change_limit-change_buffer);
#line 575 "cwebdir/common.w"
changing= 1;change_depth= include_depth;loc= buffer;
#line 402 "cwebdir/comm-w2c.ch"
err_print(_("! Change file entry did not match"));
#line 577 "cwebdir/common.w"

}
}

/*:26*//*35:*/
#line 660 "cwebdir/common.w"

#line 444 "cwebdir/comm-w2c.ch"
name_pointer
id_lookup(
const char*first,
const char*last,
char t)
{
const char*i= first;
#line 668 "cwebdir/common.w"
int h;
int l;
name_pointer p;
if(last==NULL)for(last= first;*last!='\0';last++);
#line 456 "cwebdir/comm-w2c.ch"
l= (int)(last-first);
#line 673 "cwebdir/common.w"
/*36:*/
#line 683 "cwebdir/common.w"

h= (unsigned char)*i;
while(++i<last)h= (h+h+(int)((unsigned char)*i))%hash_size;


/*:36*/
#line 673 "cwebdir/common.w"
;
/*37:*/
#line 691 "cwebdir/common.w"

p= hash[h];
while(p&&!names_match(p,first,l,t))p= p->link;
if(p==NULL){
p= name_ptr;
p->link= hash[h];hash[h]= p;
}

/*:37*/
#line 674 "cwebdir/common.w"
;
if(p==name_ptr)/*39:*/
#line 706 "cwebdir/common.w"
{
#line 469 "cwebdir/comm-w2c.ch"
if(byte_ptr+l> byte_mem_end)overflow(_("byte memory"));
if(name_ptr>=name_dir_end)overflow(_("name"));
#line 709 "cwebdir/common.w"
strncpy(byte_ptr,first,l);
(++name_ptr)->byte_start= byte_ptr+= l;
#line 476 "cwebdir/comm-w2c.ch"
init_p(p,t);
#line 712 "cwebdir/common.w"
}

/*:39*/
#line 675 "cwebdir/common.w"
;
return(p);
}

/*:35*//*42:*/
#line 764 "cwebdir/common.w"

#line 484 "cwebdir/comm-w2c.ch"
void
print_section_name(
name_pointer p)
#line 768 "cwebdir/common.w"
{
char*ss,*s= first_chunk(p);
name_pointer q= p+1;
while(p!=name_dir){
ss= (p+1)->byte_start-1;
if(*ss==' '&&ss>=s){
#line 494 "cwebdir/comm-w2c.ch"
term_write(s,(size_t)(ss-s));p= q->link;q= p;
}else{
term_write(s,(size_t)(ss+1-s));p= name_dir;q= NULL;
#line 777 "cwebdir/common.w"
}
s= p->byte_start;
}
if(q)term_write("...",3);
}

/*:42*//*43:*/
#line 783 "cwebdir/common.w"

#line 505 "cwebdir/comm-w2c.ch"
void
sprint_section_name(
char*dest,
name_pointer p)
#line 788 "cwebdir/common.w"
{
char*ss,*s= first_chunk(p);
name_pointer q= p+1;
while(p!=name_dir){
ss= (p+1)->byte_start-1;
if(*ss==' '&&ss>=s){
p= q->link;q= p;
}else{
ss++;p= name_dir;
}
#line 514 "cwebdir/comm-w2c.ch"
strncpy(dest,s,(size_t)(ss-s)),dest+= ss-s;
#line 799 "cwebdir/common.w"
s= p->byte_start;
}
*dest= '\0';
}

/*:43*//*44:*/
#line 804 "cwebdir/common.w"

#line 522 "cwebdir/comm-w2c.ch"
void
print_prefix_name(
name_pointer p)
#line 808 "cwebdir/common.w"
{
char*s= first_chunk(p);
int l= prefix_length(p);
term_write(s,l);
if(s+l<(p+1)->byte_start)term_write("...",3);
}

/*:44*//*45:*/
#line 825 "cwebdir/common.w"

#line 532 "cwebdir/comm-w2c.ch"
static int web_strcmp(
char*j,
int j_len,
char*k,
int k_len)
#line 829 "cwebdir/common.w"
{
char*j1= j+j_len,*k1= k+k_len;
while(k<k1&&j<j1&&*j==*k)k++,j++;
if(k==k1)if(j==j1)return equal;
else return extension;
else if(j==j1)return prefix;
else if(*j<*k)return less;
else return greater;
}

/*:45*//*47:*/
#line 855 "cwebdir/common.w"

#line 554 "cwebdir/comm-w2c.ch"
static name_pointer
add_section_name(
name_pointer par,
int c,
char*first,
char*last,
int ispref)
#line 863 "cwebdir/common.w"
{
name_pointer p= name_ptr;
char*s= first_chunk(p);
#line 566 "cwebdir/comm-w2c.ch"
int name_len= (int)(last-first)+ispref;
#line 573 "cwebdir/comm-w2c.ch"
if(s+name_len> byte_mem_end)overflow(_("byte memory"));
if(name_ptr+1>=name_dir_end)overflow(_("name"));
#line 869 "cwebdir/common.w"
(++name_ptr)->byte_start= byte_ptr= s+name_len;
if(ispref){
*(byte_ptr-1)= ' ';
name_len--;
name_ptr->link= name_dir;
(++name_ptr)->byte_start= byte_ptr;
}
set_prefix_length(p,name_len);
strncpy(s,first,name_len);
p->llink= NULL;
p->rlink= NULL;
init_node(p);
return par==NULL?(root= p):c==less?(par->llink= p):(par->rlink= p);
}

/*:47*//*48:*/
#line 884 "cwebdir/common.w"

#line 585 "cwebdir/comm-w2c.ch"
static void
extend_section_name(
name_pointer p,
char*first,
char*last,
int ispref)
#line 891 "cwebdir/common.w"
{
char*s;
name_pointer q= p+1;
#line 596 "cwebdir/comm-w2c.ch"
int name_len= (int)(last-first)+ispref;
#line 602 "cwebdir/comm-w2c.ch"
if(name_ptr>=name_dir_end)overflow(_("name"));
#line 896 "cwebdir/common.w"
while(q->link!=name_dir)q= q->link;
q->link= name_ptr;
s= name_ptr->byte_start;
name_ptr->link= name_dir;
#line 608 "cwebdir/comm-w2c.ch"
if(s+name_len> byte_mem_end)overflow(_("byte memory"));
#line 901 "cwebdir/common.w"
(++name_ptr)->byte_start= byte_ptr= s+name_len;
strncpy(s,first,name_len);
if(ispref)*(byte_ptr-1)= ' ';
}

/*:48*//*49:*/
#line 912 "cwebdir/common.w"

#line 617 "cwebdir/comm-w2c.ch"
name_pointer
section_lookup(
char*first,char*last,
int ispref)
#line 917 "cwebdir/common.w"
{
int c= 0;
name_pointer p= root;
name_pointer q= NULL;
name_pointer r= NULL;
name_pointer par= NULL;

#line 626 "cwebdir/comm-w2c.ch"
int name_len= (int)(last-first)+1;
#line 925 "cwebdir/common.w"
/*50:*/
#line 936 "cwebdir/common.w"

while(p){
c= web_strcmp(first,name_len,first_chunk(p),prefix_length(p));
if(c==less||c==greater){
if(r==NULL)
par= p;
p= (c==less?p->llink:p->rlink);
}else{
if(r!=NULL){
#line 632 "cwebdir/comm-w2c.ch"
fputs(_("\n! Ambiguous prefix: matches <"),stdout);
#line 946 "cwebdir/common.w"

print_prefix_name(p);
#line 638 "cwebdir/comm-w2c.ch"
fputs(_(">\n and <"),stdout);
#line 949 "cwebdir/common.w"
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

/*:50*/
#line 926 "cwebdir/common.w"
;
/*51:*/
#line 961 "cwebdir/common.w"

if(r==NULL)
return add_section_name(par,c,first,last+1,ispref);

/*:51*/
#line 927 "cwebdir/common.w"
;
/*52:*/
#line 969 "cwebdir/common.w"

switch(section_name_cmp(&first,name_len,r)){

case prefix:
if(!ispref){
#line 644 "cwebdir/comm-w2c.ch"
fputs(_("\n! New name is a prefix of <"),stdout);
#line 975 "cwebdir/common.w"

print_section_name(r);
err_print(">");
}
else if(name_len<prefix_length(r))set_prefix_length(r,name_len);

case equal:return r;
case extension:if(!ispref||first<=last)
extend_section_name(r,first,last+1,ispref);
return r;
case bad_extension:
#line 650 "cwebdir/comm-w2c.ch"
fputs(_("\n! New name extends <"),stdout);
#line 987 "cwebdir/common.w"

print_section_name(r);
err_print(">");
return r;
default:
#line 656 "cwebdir/comm-w2c.ch"
fputs(_("\n! Section name incompatible with <"),stdout);
#line 993 "cwebdir/common.w"

print_prefix_name(r);
#line 662 "cwebdir/comm-w2c.ch"
fputs(_(">,\n which abbreviates <"),stdout);
#line 996 "cwebdir/common.w"
print_section_name(r);
err_print(">");
return r;
}

/*:52*/
#line 928 "cwebdir/common.w"
;
}

/*:49*//*54:*/
#line 1020 "cwebdir/common.w"

#line 677 "cwebdir/comm-w2c.ch"
static int section_name_cmp(
char**pfirst,
int len,
name_pointer r)
#line 1025 "cwebdir/common.w"
{
char*first= *pfirst;
name_pointer q= r+1;
char*ss,*s= first_chunk(r);
int c;
int ispref;
while(1){
ss= (r+1)->byte_start-1;
if(*ss==' '&&ss>=r->byte_start)ispref= 1,q= q->link;
else ispref= 0,ss++,q= name_dir;
switch(c= web_strcmp(first,len,s,ss-s)){
case equal:if(q==name_dir)
if(ispref){
#line 686 "cwebdir/comm-w2c.ch"
*pfirst= first+(ptrdiff_t)(ss-s);
#line 1039 "cwebdir/common.w"
return extension;
}else return equal;
else return(q->byte_start==(q+1)->byte_start)?equal:prefix;
case extension:
if(!ispref)return bad_extension;
first+= ss-s;
#line 692 "cwebdir/comm-w2c.ch"
if(q!=name_dir){len-= (int)(ss-s);s= q->byte_start;r= q;continue;}
#line 1046 "cwebdir/common.w"
*pfirst= first;return extension;
default:return c;
}
}
}

/*:54*//*58:*/
#line 1095 "cwebdir/common.w"

#line 718 "cwebdir/comm-w2c.ch"
void
err_print(
const char*s)
#line 1099 "cwebdir/common.w"
{
char*k,*l;
printf(*s=='!'?"\n%s":"%s",s);
if(web_file_open)/*59:*/
#line 1115 "cwebdir/common.w"

{if(changing&&include_depth==change_depth)
#line 728 "cwebdir/comm-w2c.ch"
printf(_(". (l. %d of change file)\n"),change_line);
else if(include_depth==0)printf(_(". (l. %d)\n"),cur_line);
else printf(_(". (l. %d of include file %s)\n"),cur_line,cur_file_name);
#line 1120 "cwebdir/common.w"
l= (loc>=limit?limit:loc);
if(l> buffer){
for(k= buffer;k<l;k++)
if(*k=='\t')putchar(' ');
else putchar(*k);
putchar('\n');
for(k= buffer;k<l;k++)putchar(' ');
}
for(k= l;k<limit;k++)putchar(*k);
if(*limit=='|')putchar('|');
putchar(' ');
}

/*:59*/
#line 1102 "cwebdir/common.w"
;
update_terminal;mark_error;
}

/*:58*//*61:*/
#line 1150 "cwebdir/common.w"

#line 767 "cwebdir/comm-w2c.ch"
int wrap_up(void){
if(show_progress)new_line;
#line 1153 "cwebdir/common.w"
if(show_stats)
print_stats();
#line 774 "cwebdir/comm-w2c.ch"
/*62:*/
#line 1160 "cwebdir/common.w"

switch(history){
#line 798 "cwebdir/comm-w2c.ch"
case spotless:
if(show_happiness)puts(_("(No errors were found.)"));break;
case harmless_message:
puts(_("(Did you see the warning message above?)"));break;
case error_message:
puts(_("(Pardon me, but I think I spotted something wrong.)"));break;
case fatal_message:
puts(_("(That was a fatal error, my friend.)"));
#line 1168 "cwebdir/common.w"
}

/*:62*/
#line 774 "cwebdir/comm-w2c.ch"

/*87:*/
#line 1196 "cwebdir/comm-w2c.ch"

if(C_file)fclose(C_file);
if(tex_file)fclose(tex_file);
if(check_file)fclose(check_file);
if(strlen(check_file_name))
remove(check_file_name);

/*:87*/
#line 775 "cwebdir/comm-w2c.ch"

#line 782 "cwebdir/comm-w2c.ch"
switch(history){
case harmless_message:return RETURN_WARN;
case error_message:return RETURN_ERROR;
case fatal_message:return RETURN_FAIL;
default:return RETURN_OK;
}
#line 1158 "cwebdir/common.w"
}

/*:61*//*64:*/
#line 820 "cwebdir/comm-w2c.ch"
void
fatal(
const char*s,const char*t)
#line 1182 "cwebdir/common.w"
{
#line 828 "cwebdir/comm-w2c.ch"
if(*s)fputs(s,stdout);
#line 1184 "cwebdir/common.w"
err_print(t);
history= fatal_message;exit(wrap_up());
}

/*:64*//*65:*/
#line 836 "cwebdir/comm-w2c.ch"
void
overflow(
const char*t)
#line 1193 "cwebdir/common.w"
{
#line 844 "cwebdir/comm-w2c.ch"
printf(_("\n! Sorry, %s capacity exceeded"),t);fatal("","");
#line 1195 "cwebdir/common.w"
}


/*:65*//*70:*/
#line 1254 "cwebdir/common.w"

#line 898 "cwebdir/comm-w2c.ch"
static void
scan_args(void)
#line 1257 "cwebdir/common.w"
{
char*dot_pos;
char*name_pos;
register char*s;
boolean found_web= 0,found_change= 0,found_out= 0;

#line 1264 "cwebdir/common.w"

#line 910 "cwebdir/comm-w2c.ch"

#if defined DEV_NULL
strncpy(change_file_name,DEV_NULL,max_file_name_length-2);
change_file_name[max_file_name_length-2]= '\0';
#elif defined _DEV_NULL
strncpy(change_file_name,_DEV_NULL,max_file_name_length-2);
change_file_name[max_file_name_length-2]= '\0';
#else
strcpy(change_file_name,"/dev/null");
#endif

while(--argc> 0){
#line 1266 "cwebdir/common.w"
if((**(++argv)=='-'||**argv=='+')&&*(*argv+1))/*74:*/
#line 993 "cwebdir/comm-w2c.ch"

{
if(strcmp("-help",*argv)==0||strcmp("--help",*argv)==0)

/*94:*/
#line 1341 "cwebdir/comm-w2c.ch"

cb_usagehelp(program==ctangle?CTANGLEHELP:
program==cweave?CWEAVEHELP:CTWILLHELP,NULL);


/*:94*/
#line 997 "cwebdir/comm-w2c.ch"

if(strcmp("-version",*argv)==0||strcmp("--version",*argv)==0)

/*96:*/
#line 1383 "cwebdir/comm-w2c.ch"

printversionandexit(cb_banner,
program==ctwill?"Donald E. Knuth":"Silvio Levy and Donald E. Knuth",
NULL,NULL);


/*:96*/
#line 1000 "cwebdir/comm-w2c.ch"

if(strcmp("-verbose",*argv)==0||strcmp("--verbose",*argv)==0)

{show_banner= show_progress= show_happiness= 1;continue;}
if(strcmp("-quiet",*argv)==0||strcmp("--quiet",*argv)==0)

{show_banner= show_progress= show_happiness= 0;continue;}
for(dot_pos= *argv+1;*dot_pos> '\0';dot_pos++)
if(*dot_pos=='v'){
show_banner= show_progress= show_happiness= 1;
}else
if(*dot_pos=='q'){
show_banner= show_progress= show_happiness= 0;
}else
if(*dot_pos=='d'){
if(sscanf(++dot_pos,"%u",&kpathsea_debug)!=1)/*75:*/
#line 1352 "cwebdir/common.w"

#line 1043 "cwebdir/comm-w2c.ch"
cb_usage(program==ctangle?"ctangle":program==cweave?"cweave":"ctwill");

#line 1363 "cwebdir/common.w"

#line 1050 "cwebdir/comm-w2c.ch"
/*:75*/
#line 1015 "cwebdir/comm-w2c.ch"

while(isdigit(*dot_pos))dot_pos++;
dot_pos--;
}else
if(*dot_pos=='l'){
use_language= ++dot_pos;
break;
}else
#line 1028 "cwebdir/comm-w2c.ch"
 flags[(eight_bits)*dot_pos]= flag_change;
#line 1350 "cwebdir/common.w"
}

/*:74*/
#line 1266 "cwebdir/common.w"

else{
s= name_pos= *argv;dot_pos= NULL;
#line 931 "cwebdir/comm-w2c.ch"
while(*s){
if(*s=='.')dot_pos= s++;
else if(*s==DIR_SEPARATOR||*s==DEVICE_SEPARATOR||*s=='/')
dot_pos= NULL,name_pos= ++s;
else s++;
}

#line 1274 "cwebdir/common.w"
if(!found_web)/*71:*/
#line 1292 "cwebdir/common.w"

{
if(s-*argv> max_file_name_length-5)
/*76:*/
#line 1050 "cwebdir/comm-w2c.ch"
fatal(_("! Filename too long\n"),*argv);
#line 1365 "cwebdir/common.w"


/*:76*/
#line 1295 "cwebdir/common.w"
;
if(dot_pos==NULL)
sprintf(web_file_name,"%s.w",*argv);
else{
strcpy(web_file_name,*argv);
*dot_pos= 0;
}
#line 1303 "cwebdir/common.w"
sprintf(tex_file_name,"%s.tex",name_pos);
sprintf(idx_file_name,"%s.idx",name_pos);
sprintf(scn_file_name,"%s.scn",name_pos);
sprintf(C_file_name,"%s.c",name_pos);
found_web= 1;
}

/*:71*/
#line 1275 "cwebdir/common.w"

else if(!found_change)/*72:*/
#line 1310 "cwebdir/common.w"

{
#line 968 "cwebdir/comm-w2c.ch"
if(strcmp(*argv,"-")!=0){
#line 1314 "cwebdir/common.w"
if(s-*argv> max_file_name_length-4)
/*76:*/
#line 1050 "cwebdir/comm-w2c.ch"
fatal(_("! Filename too long\n"),*argv);
#line 1365 "cwebdir/common.w"


/*:76*/
#line 1315 "cwebdir/common.w"
;
if(dot_pos==NULL)
sprintf(change_file_name,"%s.ch",*argv);
else strcpy(change_file_name,*argv);
#line 975 "cwebdir/comm-w2c.ch"
}
found_change= 1;
#line 1321 "cwebdir/common.w"
}

/*:72*/
#line 1276 "cwebdir/common.w"

else if(!found_out)/*73:*/
#line 1323 "cwebdir/common.w"

{
if(s-*argv> max_file_name_length-5)
/*76:*/
#line 1050 "cwebdir/comm-w2c.ch"
fatal(_("! Filename too long\n"),*argv);
#line 1365 "cwebdir/common.w"


/*:76*/
#line 1326 "cwebdir/common.w"
;
if(dot_pos==NULL){
sprintf(tex_file_name,"%s.tex",*argv);
sprintf(idx_file_name,"%s.idx",*argv);
sprintf(scn_file_name,"%s.scn",*argv);
sprintf(C_file_name,"%s.c",*argv);
}else{
strcpy(tex_file_name,*argv);
strcpy(C_file_name,*argv);
#line 982 "cwebdir/comm-w2c.ch"
if(make_xrefs){
#line 1336 "cwebdir/common.w"
*dot_pos= 0;
sprintf(idx_file_name,"%s.idx",*argv);
sprintf(scn_file_name,"%s.scn",*argv);
}
}
found_out= 1;
}

#line 992 "cwebdir/comm-w2c.ch"
/*:73*/
#line 1277 "cwebdir/common.w"

else/*75:*/
#line 1352 "cwebdir/common.w"

#line 1043 "cwebdir/comm-w2c.ch"
 cb_usage(program==ctangle?"ctangle":program==cweave?"cweave":"ctwill");

#line 1363 "cwebdir/common.w"

#line 1050 "cwebdir/comm-w2c.ch"
/*:75*/
#line 1278 "cwebdir/common.w"
;
}
}
if(!found_web)/*75:*/
#line 1352 "cwebdir/common.w"

#line 1043 "cwebdir/comm-w2c.ch"
cb_usage(program==ctangle?"ctangle":program==cweave?"cweave":"ctwill");

#line 1363 "cwebdir/common.w"

#line 1050 "cwebdir/comm-w2c.ch"
/*:75*/
#line 1281 "cwebdir/common.w"
;
#line 1283 "cwebdir/common.w"
}

#line 953 "cwebdir/comm-w2c.ch"
/*:70*//*95:*/
#line 1349 "cwebdir/comm-w2c.ch"

static void cb_usage(const_string str)
{
textdomain("cweb-tl");

fprintf(stderr,_("%s: Need one to three file arguments.\n"),str);
fprintf(stderr,_("Try `%s --help' for more information.\n"),str);

textdomain("cweb");

history= fatal_message;exit(wrap_up());
}

static void cb_usagehelp(const_string*message,const_string bug_email)
{
if(!bug_email)
bug_email= "tex-k@tug.org";
textdomain("web2c-help");

while(*message){
printf("%s\n",strcmp("",*message)?_(*message):*message);
++message;
}
textdomain("cweb-tl");

printf(_("\nEmail bug reports to %s.\n"),bug_email);
textdomain("cweb");

history= spotless;exit(wrap_up());
}

/*:95*//*97:*/
#line 1391 "cwebdir/comm-w2c.ch"

void cb_show_banner(void)
{
assert(cb_banner[0]!='\0');
textdomain("cweb-tl");

printf("%s%s\n",_(cb_banner),versionstring);
textdomain("cweb");

}

/*:97*/
