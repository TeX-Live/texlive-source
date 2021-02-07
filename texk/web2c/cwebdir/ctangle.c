/*1:*/
#line 66 "ctangle.w"

/*4:*/
#line 46 "common.h"

#include <ctype.h>  
#include <stdbool.h>  
#include <stddef.h>  
#include <stdint.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  

/*:4*/
#line 67 "ctangle.w"

#define banner "This is CTANGLE (Version 4.0)" \

#define ctangle 0
#define cweave 1 \

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

#define xisalpha(c) (isalpha((eight_bits) c) &&((eight_bits) c<0200) ) 
#define xisdigit(c) (isdigit((eight_bits) c) &&((eight_bits) c<0200) ) 
#define xisspace(c) (isspace((eight_bits) c) &&((eight_bits) c<0200) ) 
#define xislower(c) (islower((eight_bits) c) &&((eight_bits) c<0200) ) 
#define xisupper(c) (isupper((eight_bits) c) &&((eight_bits) c<0200) ) 
#define xisxdigit(c) (isxdigit((eight_bits) c) &&((eight_bits) c<0200) )  \

#define length(c) (size_t) ((c+1) ->byte_start-(c) ->byte_start) 
#define print_id(c) term_write((c) ->byte_start,length((c) ) ) 
#define llink link
#define rlink dummy.Rlink
#define root name_dir->rlink \

#define chunk_marker 0 \

#define spotless 0
#define harmless_message 1
#define error_message 2
#define fatal_message 3
#define mark_harmless {if(history==spotless) history= harmless_message;}
#define mark_error history= error_message
#define confusion(s) fatal("! This can't happen: ",s)  \
 \

#define max_include_depth 10 \

#define max_file_name_length 1024
#define cur_file file[include_depth]
#define cur_file_name file_name[include_depth]
#define cur_line line[include_depth]
#define web_file file[0]
#define web_file_name file_name[0] \

#define show_banner flags['b']
#define show_progress flags['p']
#define show_stats flags['s']
#define show_happiness flags['h']
#define make_xrefs flags['x'] \

#define update_terminal fflush(stdout) 
#define new_line putchar('\n') 
#define putxchar putchar
#define term_write(a,b) fflush(stdout) ,fwrite(a,sizeof(char) ,b,stdout) 
#define C_printf(c,a) fprintf(C_file,c,a) 
#define C_putc(c) putc(c,C_file)  \

#define max_bytes 1000000 \

#define max_toks 1000000
#define max_names 10239 \

#define max_sections 4000
#define max_texts 10239
#define longest_name 10000
#define stack_size 500
#define buf_size 1000
#define long_buf_size (buf_size+longest_name)  \

#define equiv equiv_or_xref \

#define section_flag max_texts \

#define string 02
#define join 0177
#define output_defs_flag (2*024000-1)  \

#define cur_end cur_state.end_field
#define cur_byte cur_state.byte_field
#define cur_name cur_state.name_field
#define cur_repl cur_state.repl_field
#define cur_section cur_state.section_field \

#define section_number 0201
#define identifier 0202 \

#define normal 0
#define num_or_id 1
#define post_slash 2
#define unbreakable 3
#define verbatim 4 \

#define max_files 256
#define translit_length 10 \

#define ignore 0
#define ord 0302
#define control_text 0303
#define translit_code 0304
#define output_defs_code 0305
#define format_code 0306
#define definition 0307
#define begin_C 0310
#define section_name 0311
#define new_section 0312 \

#define constant 03 \

#define isxalpha(c) ((c) =='_'||(c) =='$')  \

#define ishigh(c) ((eight_bits) (c) > 0177)  \
 \

#define compress(c) if(loc++<=limit) return(c)  \

#define macro 0
#define app_repl(c) {if(tok_ptr==tok_mem_end) overflow("token") ;*tok_ptr++= c;} \


#line 68 "ctangle.w"

/*3:*/
#line 37 "common.h"

typedef bool boolean;
typedef uint8_t eight_bits;
typedef uint16_t sixteen_bits;
extern boolean program;
extern int phase;

/*:3*//*5:*/
#line 74 "common.h"

extern char section_text[];
extern char*section_text_end;
extern char*id_first;
extern char*id_loc;

/*:5*//*6:*/
#line 88 "common.h"

extern char buffer[];
extern char*buffer_end;
extern char*loc;
extern char*limit;

/*:6*//*7:*/
#line 103 "common.h"

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
extern name_info name_dir[];
extern name_pointer name_dir_end;
extern name_pointer name_ptr;
extern char*byte_ptr;
extern name_pointer hash[];
extern hash_pointer hash_end;
extern hash_pointer h;

/*:7*//*9:*/
#line 147 "common.h"

extern int history;

/*:9*//*11:*/
#line 167 "common.h"

extern int include_depth;
extern FILE*file[];
extern FILE*change_file;
extern char C_file_name[];
extern char tex_file_name[];
extern char idx_file_name[];
extern char scn_file_name[];
extern char file_name[][max_file_name_length];

extern char change_file_name[];
extern int line[];
extern int change_line;
extern int change_depth;
extern boolean input_has_ended;
extern boolean changing;
extern boolean web_file_open;

/*:11*//*13:*/
#line 191 "common.h"

extern sixteen_bits section_count;
extern boolean changed_section[];
extern boolean change_pending;
extern boolean print_where;

/*:13*//*14:*/
#line 204 "common.h"

extern int argc;
extern char**argv;
extern boolean flags[];

/*:14*//*15:*/
#line 216 "common.h"

extern FILE*C_file;
extern FILE*tex_file;
extern FILE*idx_file;
extern FILE*scn_file;
extern FILE*active_file;

/*:15*/
#line 69 "ctangle.w"

/*19:*/
#line 124 "ctangle.w"

typedef struct{
eight_bits*tok_start;
sixteen_bits text_link;
}text;
typedef text*text_pointer;

/*:19*//*31:*/
#line 272 "ctangle.w"

typedef struct{
eight_bits*end_field;
eight_bits*byte_field;
name_pointer name_field;
text_pointer repl_field;
sixteen_bits section_field;
}output_state;
typedef output_state*stack_pointer;

/*:31*/
#line 70 "ctangle.w"

/*20:*/
#line 131 "ctangle.w"

text text_info[max_texts];
text_pointer text_info_end= text_info+max_texts-1;
text_pointer text_ptr;
eight_bits tok_mem[max_toks];
eight_bits*tok_mem_end= tok_mem+max_toks-1;
eight_bits*tok_ptr;

/*:20*//*26:*/
#line 200 "ctangle.w"

text_pointer last_unnamed;

/*:26*//*32:*/
#line 288 "ctangle.w"

output_state cur_state;

output_state stack[stack_size+1];
stack_pointer stack_ptr;
stack_pointer stack_end= stack+stack_size;

/*:32*//*37:*/
#line 364 "ctangle.w"

int cur_val;

/*:37*//*42:*/
#line 456 "ctangle.w"

eight_bits out_state;
boolean protect;

/*:42*//*45:*/
#line 488 "ctangle.w"

name_pointer output_files[max_files];
name_pointer*cur_out_file,*end_output_files,*an_output_file;
char cur_section_name_char;
char output_file_name[longest_name+1];

/*:45*//*52:*/
#line 588 "ctangle.w"

boolean output_defs_seen= 0;

/*:52*//*57:*/
#line 697 "ctangle.w"

char translit[128][translit_length];

/*:57*//*62:*/
#line 776 "ctangle.w"

eight_bits ccode[256];

/*:62*//*66:*/
#line 836 "ctangle.w"

boolean comment_continues= 0;

/*:66*//*68:*/
#line 875 "ctangle.w"

name_pointer cur_section_name;
int no_where;

/*:68*//*82:*/
#line 1189 "ctangle.w"

text_pointer cur_text;
eight_bits next_control;

/*:82*//*90:*/
#line 1350 "ctangle.w"

extern sixteen_bits section_count;

/*:90*/
#line 71 "ctangle.w"

/*8:*/
#line 126 "common.h"

extern boolean names_match(name_pointer,const char*,size_t,eight_bits);
extern name_pointer id_lookup(const char*,const char*,char);

extern name_pointer section_lookup(char*,char*,int);
extern void init_node(name_pointer);
extern void init_p(name_pointer,eight_bits);
extern void print_prefix_name(name_pointer);
extern void print_section_name(name_pointer);
extern void sprint_section_name(char*,name_pointer);

/*:8*//*10:*/
#line 150 "common.h"

extern int wrap_up(void);
extern void err_print(const char*);
extern void fatal(const char*,const char*);
extern void overflow(const char*);

/*:10*//*12:*/
#line 185 "common.h"

extern boolean get_line(void);
extern void check_complete(void);
extern void reset_input(void);

/*:12*//*16:*/
#line 224 "common.h"

extern void common_init(void);
extern void print_stats(void);

/*:16*//*29:*/
#line 233 "ctangle.w"

static void store_two_bytes(sixteen_bits);

/*:29*//*34:*/
#line 312 "ctangle.w"

static void push_level(name_pointer);
static void pop_level(int);

/*:34*//*38:*/
#line 371 "ctangle.w"

static void get_output(void);

/*:38*//*43:*/
#line 464 "ctangle.w"

static void flush_buffer(void);

/*:43*//*48:*/
#line 519 "ctangle.w"

static void phase_two(void);

/*:48*//*53:*/
#line 591 "ctangle.w"

static void output_defs(void);
static void out_char(eight_bits);

/*:53*//*64:*/
#line 800 "ctangle.w"

static eight_bits skip_ahead(void);
static boolean skip_comment(boolean);

/*:64*//*69:*/
#line 887 "ctangle.w"

static eight_bits get_next(void);

/*:69*//*83:*/
#line 1193 "ctangle.w"

static void scan_repl(eight_bits);

/*:83*//*91:*/
#line 1357 "ctangle.w"

static void scan_section(void);

/*:91*//*99:*/
#line 1460 "ctangle.w"

static void phase_one(void);

/*:99*//*101:*/
#line 1478 "ctangle.w"

static void skip_limbo(void);

/*:101*/
#line 72 "ctangle.w"


/*:1*//*2:*/
#line 81 "ctangle.w"

int main(
int ac,
char**av)
{
argc= ac;argv= av;
program= ctangle;
/*21:*/
#line 139 "ctangle.w"

text_info->tok_start= tok_ptr= tok_mem;
text_ptr= text_info+1;text_ptr->tok_start= tok_mem;


/*:21*//*23:*/
#line 149 "ctangle.w"

init_node(name_dir);

/*:23*//*27:*/
#line 203 "ctangle.w"
last_unnamed= text_info;text_info->text_link= 0;

/*:27*//*46:*/
#line 498 "ctangle.w"

cur_out_file= end_output_files= output_files+max_files;

/*:46*//*58:*/
#line 700 "ctangle.w"

{
int i;
for(i= 0;i<128;i++)sprintf(translit[i],"X%02X",(unsigned int)(128+i));
}

/*:58*//*63:*/
#line 779 "ctangle.w"
{
int c;
for(c= 0;c<256;c++)ccode[c]= ignore;
ccode[' ']= ccode['\t']= ccode['\n']= ccode['\v']= ccode['\r']= ccode['\f']
= ccode['*']= new_section;
ccode['@']= '@';ccode['=']= string;
ccode['d']= ccode['D']= definition;
ccode['f']= ccode['F']= ccode['s']= ccode['S']= format_code;
ccode['c']= ccode['C']= ccode['p']= ccode['P']= begin_C;
ccode['^']= ccode[':']= ccode['.']= ccode['t']= ccode['T']= 
ccode['q']= ccode['Q']= control_text;
ccode['h']= ccode['H']= output_defs_code;
ccode['l']= ccode['L']= translit_code;
ccode['&']= join;
ccode['<']= ccode['(']= section_name;
ccode['\'']= ord;
}

/*:63*//*78:*/
#line 1110 "ctangle.w"
section_text[0]= ' ';

/*:78*/
#line 88 "ctangle.w"

common_init();
if(show_banner)puts(banner);
phase_one();
phase_two();
return wrap_up();
}

/*:2*//*24:*/
#line 155 "ctangle.w"

boolean names_match(
name_pointer p,
const char*first,
size_t l,
eight_bits t)
{(void)t;
if(length(p)!=l)return 0;
return!strncmp(first,p->byte_start,l);
}

/*:24*//*25:*/
#line 171 "ctangle.w"

void
init_node(
name_pointer node)
{
node->equiv= (void*)text_info;
}
void
init_p(name_pointer p,eight_bits t){(void)p;(void)t;}

/*:25*//*30:*/
#line 236 "ctangle.w"

void
store_two_bytes(
sixteen_bits x)
{
if(tok_ptr+2> tok_mem_end)overflow("token");
*tok_ptr++= x>>8;
*tok_ptr++= x&0377;
}

/*:30*//*35:*/
#line 316 "ctangle.w"

static void
push_level(
name_pointer p)
{
if(stack_ptr==stack_end)overflow("stack");
*stack_ptr= cur_state;
stack_ptr++;
if(p!=NULL){
cur_name= p;cur_repl= (text_pointer)p->equiv;
cur_byte= cur_repl->tok_start;cur_end= (cur_repl+1)->tok_start;
cur_section= 0;
}
}

/*:35*//*36:*/
#line 335 "ctangle.w"

static void
pop_level(
int flag)
{
if(flag&&cur_repl->text_link<section_flag){
cur_repl= cur_repl->text_link+text_info;
cur_byte= cur_repl->tok_start;cur_end= (cur_repl+1)->tok_start;
return;
}
stack_ptr--;
if(stack_ptr> stack)cur_state= *stack_ptr;
}

/*:36*//*39:*/
#line 374 "ctangle.w"

static void
get_output(void)
{
sixteen_bits a;
restart:if(stack_ptr==stack)return;
if(cur_byte==cur_end){
cur_val= -((int)cur_section);
pop_level(1);
if(cur_val==0)goto restart;
out_char(section_number);return;
}
a= *cur_byte++;
if(out_state==verbatim&&a!=string&&a!=constant&&a!='\n')
C_putc(a);
else if(a<0200)out_char(a);
else{
a= (a-0200)*0400+*cur_byte++;
switch(a/024000){
case 0:cur_val= a;out_char(identifier);break;
case 1:if(a==output_defs_flag)output_defs();
else/*40:*/
#line 406 "ctangle.w"

{
a-= 024000;
if((a+name_dir)->equiv!=(void*)text_info)push_level(a+name_dir);
else if(a!=0){
fputs("\n! Not present: <",stdout);
print_section_name(a+name_dir);err_print(">");

}
goto restart;
}

/*:40*/
#line 395 "ctangle.w"

break;
default:cur_val= a-050000;if(cur_val> 0)cur_section= cur_val;
out_char(section_number);
}
}
}

/*:39*//*44:*/
#line 467 "ctangle.w"

static void
flush_buffer(void)
{
C_putc('\n');
if(cur_line%100==0&&show_progress){
printf(".");
if(cur_line%500==0)printf("%d",cur_line);
update_terminal;
}
cur_line++;
}

/*:44*//*49:*/
#line 522 "ctangle.w"

static void
phase_two(void){
web_file_open= 0;
cur_line= 1;
/*33:*/
#line 301 "ctangle.w"

stack_ptr= stack+1;cur_name= name_dir;cur_repl= text_info->text_link+text_info;
cur_byte= cur_repl->tok_start;cur_end= (cur_repl+1)->tok_start;cur_section= 0;

/*:33*/
#line 527 "ctangle.w"

/*51:*/
#line 584 "ctangle.w"

if(!output_defs_seen)
output_defs();

/*:51*/
#line 528 "ctangle.w"

if(text_info->text_link==0&&cur_out_file==end_output_files){
fputs("\n! No program text was specified.",stdout);mark_harmless;

}
else{
if(cur_out_file==end_output_files){
if(show_progress)
printf("\nWriting the output file (%s):",C_file_name);
}
else{
if(show_progress){
fputs("\nWriting the output files:",stdout);

printf(" (%s)",C_file_name);
update_terminal;
}
if(text_info->text_link==0)goto writeloop;
}
while(stack_ptr> stack)get_output();
flush_buffer();
writeloop:/*50:*/
#line 561 "ctangle.w"

for(an_output_file= end_output_files;an_output_file> cur_out_file;){
an_output_file--;
sprint_section_name(output_file_name,*an_output_file);
fclose(C_file);
C_file= fopen(output_file_name,"wb");
if(C_file==0)fatal("! Cannot open output file ",output_file_name);

if(show_progress){printf("\n(%s)",output_file_name);update_terminal;}
cur_line= 1;
stack_ptr= stack+1;
cur_name= (*an_output_file);
cur_repl= (text_pointer)cur_name->equiv;
cur_byte= cur_repl->tok_start;
cur_end= (cur_repl+1)->tok_start;
while(stack_ptr> stack)get_output();
flush_buffer();
}

/*:50*/
#line 549 "ctangle.w"

if(show_happiness){
if(show_progress)new_line;
fputs("Done.",stdout);
}
}
}

/*:49*//*54:*/
#line 595 "ctangle.w"

static void
output_defs(void)
{
sixteen_bits a;
push_level(NULL);
for(cur_text= text_info+1;cur_text<text_ptr;cur_text++)
if(cur_text->text_link==0){
cur_byte= cur_text->tok_start;
cur_end= (cur_text+1)->tok_start;
C_printf("%s","#define ");
out_state= normal;
protect= 1;
while(cur_byte<cur_end){
a= *cur_byte++;
if(cur_byte==cur_end&&a=='\n')break;
if(out_state==verbatim&&a!=string&&a!=constant&&a!='\n')
C_putc(a);

else if(a<0200)out_char(a);
else{
a= (a-0200)*0400+*cur_byte++;
if(a<024000){
cur_val= a;out_char(identifier);
}
else if(a<050000){confusion("macro defs have strange char");}
else{
cur_val= a-050000;cur_section= cur_val;out_char(section_number);
}

}
}
protect= 0;
flush_buffer();
}
pop_level(0);
}

/*:54*//*55:*/
#line 638 "ctangle.w"

static void
out_char(
eight_bits cur_char)
{
char*j,*k;
restart:
switch(cur_char){
case'\n':if(protect&&out_state!=verbatim)C_putc(' ');
if(protect||out_state==verbatim)C_putc('\\');
flush_buffer();if(out_state!=verbatim)out_state= normal;break;
/*59:*/
#line 706 "ctangle.w"

case identifier:
if(out_state==num_or_id)C_putc(' ');
j= (cur_val+name_dir)->byte_start;
k= (cur_val+name_dir+1)->byte_start;
while(j<k){
if((eight_bits)(*j)<0200)C_putc(*j);

else C_printf("%s",translit[(eight_bits)(*j)-0200]);
j++;
}
out_state= num_or_id;break;

/*:59*/
#line 649 "ctangle.w"

/*60:*/
#line 719 "ctangle.w"

case section_number:
if(cur_val> 0)C_printf("/*%d:*/",cur_val);
else if(cur_val<0)C_printf("/*:%d*/",-cur_val);
else if(protect){
cur_byte+= 4;
cur_char= '\n';
goto restart;
}else{
sixteen_bits a;
a= 0400**cur_byte++;
a+= *cur_byte++;
C_printf("\n#line %d \"",a);

cur_val= *cur_byte++;
cur_val= 0400*(cur_val-0200)+*cur_byte++;
for(j= (cur_val+name_dir)->byte_start,k= (cur_val+name_dir+1)->byte_start;
j<k;j++){
if(*j=='\\'||*j=='"')C_putc('\\');
C_putc(*j);
}
C_printf("%s","\"\n");
}
break;

/*:60*/
#line 650 "ctangle.w"

/*56:*/
#line 668 "ctangle.w"

case plus_plus:C_putc('+');C_putc('+');out_state= normal;break;
case minus_minus:C_putc('-');C_putc('-');out_state= normal;break;
case minus_gt:C_putc('-');C_putc('>');out_state= normal;break;
case gt_gt:C_putc('>');C_putc('>');out_state= normal;break;
case eq_eq:C_putc('=');C_putc('=');out_state= normal;break;
case lt_lt:C_putc('<');C_putc('<');out_state= normal;break;
case gt_eq:C_putc('>');C_putc('=');out_state= normal;break;
case lt_eq:C_putc('<');C_putc('=');out_state= normal;break;
case non_eq:C_putc('!');C_putc('=');out_state= normal;break;
case and_and:C_putc('&');C_putc('&');out_state= normal;break;
case or_or:C_putc('|');C_putc('|');out_state= normal;break;
case dot_dot_dot:C_putc('.');C_putc('.');C_putc('.');out_state= normal;
break;
case colon_colon:C_putc(':');C_putc(':');out_state= normal;break;
case period_ast:C_putc('.');C_putc('*');out_state= normal;break;
case minus_gt_ast:C_putc('-');C_putc('>');C_putc('*');out_state= normal;
break;

/*:56*/
#line 651 "ctangle.w"

case'=':case'>':C_putc(cur_char);C_putc(' ');
out_state= normal;break;
case join:out_state= unbreakable;break;
case constant:if(out_state==verbatim){
out_state= num_or_id;break;
}
if(out_state==num_or_id)C_putc(' ');out_state= verbatim;break;
case string:if(out_state==verbatim)out_state= normal;
else out_state= verbatim;break;
case'/':C_putc('/');out_state= post_slash;break;
case'*':if(out_state==post_slash)C_putc(' ');

default:C_putc(cur_char);out_state= normal;break;
}
}

/*:55*//*65:*/
#line 804 "ctangle.w"

static eight_bits
skip_ahead(void)
{
eight_bits c;
while(1){
if(loc> limit&&(get_line()==0))return(new_section);
*(limit+1)= '@';
while(*loc!='@')loc++;
if(loc<=limit){
loc++;c= ccode[(eight_bits)*loc];loc++;
if(c!=ignore||*(loc-1)=='>')return(c);
}
}
}

/*:65*//*67:*/
#line 839 "ctangle.w"

static boolean skip_comment(
boolean is_long_comment)
{
char c;
while(1){
if(loc> limit){
if(is_long_comment){
if(get_line())return(comment_continues= 1);
else{
err_print("! Input ended in mid-comment");

return(comment_continues= 0);
}
}
else return(comment_continues= 0);
}
c= *(loc++);
if(is_long_comment&&c=='*'&&*loc=='/'){
loc++;return(comment_continues= 0);
}
if(c=='@'){
if(ccode[(eight_bits)*loc]==new_section){
err_print("! Section name ended in mid-comment");loc--;

return(comment_continues= 0);
}
else loc++;
}
}
}

/*:67*//*70:*/
#line 890 "ctangle.w"

static eight_bits
get_next(void)
{
static int preprocessing= 0;
eight_bits c;
while(1){
if(loc> limit){
if(preprocessing&&*(limit-1)!='\\')preprocessing= 0;
if(get_line()==0)return(new_section);
else if(print_where&&!no_where){
print_where= 0;
/*85:*/
#line 1222 "ctangle.w"

store_two_bytes(0150000);
if(changing&&include_depth==change_depth){
id_first= change_file_name;
store_two_bytes((sixteen_bits)change_line);
}else{
id_first= cur_file_name;
store_two_bytes((sixteen_bits)cur_line);
}
id_loc= id_first+strlen(id_first);
{int a_l= id_lookup(id_first,id_loc,0)-name_dir;app_repl((a_l/0400)+0200);
app_repl(a_l%0400);}

/*:85*/
#line 902 "ctangle.w"

}
else return('\n');
}
c= *loc;
if(comment_continues||(c=='/'&&(*(loc+1)=='*'||*(loc+1)=='/'))){
skip_comment(comment_continues||*(loc+1)=='*');

if(comment_continues)return('\n');
else continue;
}
loc++;
if(xisdigit(c)||c=='.')/*73:*/
#line 971 "ctangle.w"
{
id_first= loc-1;
if(*id_first=='.'&&!xisdigit(*loc))goto mistake;
if(*id_first=='0'){
if(*loc=='x'||*loc=='X'){
loc++;while(xisxdigit(*loc))loc++;goto found;
}
}
while(xisdigit(*loc))loc++;
if(*loc=='.'){
loc++;
while(xisdigit(*loc))loc++;
}
if(*loc=='e'||*loc=='E'){
if(*++loc=='+'||*loc=='-')loc++;
while(xisdigit(*loc))loc++;
}
found:while(*loc=='u'||*loc=='U'||*loc=='l'||*loc=='L'
||*loc=='f'||*loc=='F')loc++;
id_loc= loc;
return(constant);
}

/*:73*/
#line 914 "ctangle.w"

else if(c=='\''||c=='"'
||((c=='L'||c=='u'||c=='U')&&(*loc=='\''||*loc=='"'))
||((c=='u'&&*loc=='8')&&(*(loc+1)=='\''||*(loc+1)=='"')))
/*74:*/
#line 999 "ctangle.w"
{
char delim= c;
id_first= section_text+1;
id_loc= section_text;*++id_loc= delim;
if(delim=='L'||delim=='u'||delim=='U'){
if(delim=='u'&&*loc=='8'){*++id_loc= *loc++;}
delim= *loc++;*++id_loc= delim;
}
while(1){
if(loc>=limit){
if(*(limit-1)!='\\'){
err_print("! String didn't end");loc= limit;break;

}
if(get_line()==0){
err_print("! Input ended in middle of string");loc= buffer;break;

}
else if(++id_loc<=section_text_end)*id_loc= '\n';

}
if((c= *loc++)==delim){
if(++id_loc<=section_text_end)*id_loc= c;
break;
}
if(c=='\\'){
if(loc>=limit)continue;
if(++id_loc<=section_text_end)*id_loc= '\\';
c= *loc++;
}
if(++id_loc<=section_text_end)*id_loc= c;
}
if(id_loc>=section_text_end){
fputs("\n! String too long: ",stdout);

term_write(section_text+1,25);
err_print("...");
}
id_loc++;
return(string);
}

/*:74*/
#line 918 "ctangle.w"

else if(isalpha(c)||isxalpha(c)||ishigh(c))
/*72:*/
#line 964 "ctangle.w"
{
id_first= --loc;
while(isalpha((eight_bits)*++loc)||isdigit((eight_bits)*loc)
||isxalpha((eight_bits)*loc)||ishigh((eight_bits)*loc));
id_loc= loc;return(identifier);
}

/*:72*/
#line 920 "ctangle.w"

else if(c=='@')/*75:*/
#line 1044 "ctangle.w"
{
c= ccode[(eight_bits)*loc++];
switch(c){
case ignore:continue;
case translit_code:err_print("! Use @l in limbo only");continue;

case control_text:while((c= skip_ahead())=='@');

if(*(loc-1)!='>')
err_print("! Double @ should be used in control text");

continue;
case section_name:
cur_section_name_char= *(loc-1);
/*77:*/
#line 1092 "ctangle.w"
{
char*k;
/*79:*/
#line 1112 "ctangle.w"

k= section_text;
while(1){
if(loc> limit&&get_line()==0){
err_print("! Input ended in section name");

loc= buffer+1;break;
}
c= *loc;
/*80:*/
#line 1136 "ctangle.w"

if(c=='@'){
c= *(loc+1);
if(c=='>'){
loc+= 2;break;
}
if(ccode[(eight_bits)c]==new_section){
err_print("! Section name didn't end");break;

}
if(ccode[(eight_bits)c]==section_name){
err_print("! Nesting of section names not allowed");break;

}
*(++k)= '@';loc++;
}

/*:80*/
#line 1121 "ctangle.w"

loc++;if(k<section_text_end)k++;
if(xisspace(c)){
c= ' ';if(*(k-1)==' ')k--;
}
*k= c;
}
if(k>=section_text_end){
fputs("\n! Section name too long: ",stdout);

term_write(section_text+1,25);
printf("...");mark_harmless;
}
if(*k==' '&&k> section_text)k--;

/*:79*/
#line 1094 "ctangle.w"

if(k-section_text> 3&&strncmp(k-2,"...",3)==0)
cur_section_name= section_lookup(section_text+1,k-3,1);
else cur_section_name= section_lookup(section_text+1,k,0);
if(cur_section_name_char=='(')
/*47:*/
#line 502 "ctangle.w"

{
for(an_output_file= cur_out_file;
an_output_file<end_output_files;an_output_file++)
if(*an_output_file==cur_section_name)break;
if(an_output_file==end_output_files){
if(cur_out_file> output_files)
*--cur_out_file= cur_section_name;
else{
overflow("output files");
}
}
}

/*:47*/
#line 1100 "ctangle.w"

return(section_name);
}

/*:77*/
#line 1058 "ctangle.w"

case string:/*81:*/
#line 1158 "ctangle.w"
{
id_first= loc++;*(limit+1)= '@';*(limit+2)= '>';
while(*loc!='@'||*(loc+1)!='>')loc++;
if(loc>=limit)err_print("! Verbatim string didn't end");

id_loc= loc;loc+= 2;
return(string);
}

/*:81*/
#line 1059 "ctangle.w"

case ord:/*76:*/
#line 1071 "ctangle.w"

id_first= loc;
if(*loc=='\\'){
if(*++loc=='\'')loc++;
}
while(*loc!='\''){
if(*loc=='@'){
if(*(loc+1)!='@')
err_print("! Double @ should be used in ASCII constant");

else loc++;
}
loc++;
if(loc> limit){
err_print("! String didn't end");loc= limit-1;break;

}
}
loc++;
return(ord);

/*:76*/
#line 1060 "ctangle.w"

default:return(c);
}
}

/*:75*/
#line 921 "ctangle.w"

else if(xisspace(c)){
if(!preprocessing||loc> limit)continue;

else return(' ');
}
else if(c=='#'&&loc==buffer+1)preprocessing= 1;
mistake:/*71:*/
#line 942 "ctangle.w"

switch(c){
case'+':if(*loc=='+')compress(plus_plus);break;
case'-':if(*loc=='-'){compress(minus_minus);}
else{if(*loc=='>'){if(*(loc+1)=='*'){loc++;compress(minus_gt_ast);}
else compress(minus_gt);}}break;
case'.':if(*loc=='*'){compress(period_ast);}
else if(*loc=='.'&&*(loc+1)=='.'){
loc++;compress(dot_dot_dot);
}
break;
case':':if(*loc==':')compress(colon_colon);break;
case'=':if(*loc=='=')compress(eq_eq);break;
case'>':if(*loc=='='){compress(gt_eq);}
else if(*loc=='>')compress(gt_gt);break;
case'<':if(*loc=='='){compress(lt_eq);}
else if(*loc=='<')compress(lt_lt);break;
case'&':if(*loc=='&')compress(and_and);break;
case'|':if(*loc=='|')compress(or_or);break;
case'!':if(*loc=='=')compress(non_eq);break;
}

/*:71*/
#line 928 "ctangle.w"

return(c);
}
}

/*:70*//*84:*/
#line 1196 "ctangle.w"

static void
scan_repl(
eight_bits t)
{
sixteen_bits a;
if(t==section_name){/*85:*/
#line 1222 "ctangle.w"

store_two_bytes(0150000);
if(changing&&include_depth==change_depth){
id_first= change_file_name;
store_two_bytes((sixteen_bits)change_line);
}else{
id_first= cur_file_name;
store_two_bytes((sixteen_bits)cur_line);
}
id_loc= id_first+strlen(id_first);
{int a_l= id_lookup(id_first,id_loc,0)-name_dir;app_repl((a_l/0400)+0200);
app_repl(a_l%0400);}

/*:85*/
#line 1202 "ctangle.w"
}
while(1)switch(a= get_next()){
/*86:*/
#line 1235 "ctangle.w"

case identifier:a= id_lookup(id_first,id_loc,0)-name_dir;
app_repl((a/0400)+0200);
app_repl(a%0400);break;
case section_name:if(t!=section_name)goto done;
else{
/*87:*/
#line 1268 "ctangle.w"
{
char*try_loc= loc;
while(*try_loc==' '&&try_loc<limit)try_loc++;
if(*try_loc=='+'&&try_loc<limit)try_loc++;
while(*try_loc==' '&&try_loc<limit)try_loc++;
if(*try_loc=='=')err_print("! Missing `@ ' before a named section");



}

/*:87*/
#line 1241 "ctangle.w"

a= cur_section_name-name_dir;
app_repl((a/0400)+0250);
app_repl(a%0400);
/*85:*/
#line 1222 "ctangle.w"

store_two_bytes(0150000);
if(changing&&include_depth==change_depth){
id_first= change_file_name;
store_two_bytes((sixteen_bits)change_line);
}else{
id_first= cur_file_name;
store_two_bytes((sixteen_bits)cur_line);
}
id_loc= id_first+strlen(id_first);
{int a_l= id_lookup(id_first,id_loc,0)-name_dir;app_repl((a_l/0400)+0200);
app_repl(a_l%0400);}

/*:85*/
#line 1245 "ctangle.w"
break;
}
case output_defs_code:if(t!=section_name)err_print("! Misplaced @h");

else{
output_defs_seen= 1;
a= output_defs_flag;
app_repl((a/0400)+0200);
app_repl(a%0400);
/*85:*/
#line 1222 "ctangle.w"

store_two_bytes(0150000);
if(changing&&include_depth==change_depth){
id_first= change_file_name;
store_two_bytes((sixteen_bits)change_line);
}else{
id_first= cur_file_name;
store_two_bytes((sixteen_bits)cur_line);
}
id_loc= id_first+strlen(id_first);
{int a_l= id_lookup(id_first,id_loc,0)-name_dir;app_repl((a_l/0400)+0200);
app_repl(a_l%0400);}

/*:85*/
#line 1254 "ctangle.w"

}
break;
case constant:case string:
/*88:*/
#line 1279 "ctangle.w"

app_repl(a);
while(id_first<id_loc){
if(*id_first=='@'){
if(*(id_first+1)=='@')id_first++;
else err_print("! Double @ should be used in string");

}
app_repl(*id_first++);
}
app_repl(a);break;

/*:88*/
#line 1258 "ctangle.w"

case ord:
/*89:*/
#line 1295 "ctangle.w"
{
int c= (eight_bits)*id_first;
if(c=='\\'){
c= *++id_first;
if(c>='0'&&c<='7'){
c-= '0';
if(*(id_first+1)>='0'&&*(id_first+1)<='7'){
c= 8*c+*(++id_first)-'0';
if(*(id_first+1)>='0'&&*(id_first+1)<='7'&&c<32)
c= 8*c+*(++id_first)-'0';
}
}
else switch(c){
case't':c= '\t';break;
case'n':c= '\n';break;
case'b':c= '\b';break;
case'f':c= '\f';break;
case'v':c= '\v';break;
case'r':c= '\r';break;
case'a':c= '\7';break;
case'?':c= '?';break;
case'x':
if(xisdigit(*(id_first+1)))c= *(++id_first)-'0';
else if(xisxdigit(*(id_first+1))){
++id_first;
c= toupper((eight_bits)*id_first)-'A'+10;
}
if(xisdigit(*(id_first+1)))c= 16*c+*(++id_first)-'0';
else if(xisxdigit(*(id_first+1))){
++id_first;
c= 16*c+toupper((eight_bits)*id_first)-'A'+10;
}
break;
case'\\':c= '\\';break;
case'\'':c= '\'';break;
case'\"':c= '\"';break;
default:err_print("! Unrecognized escape sequence");

}
}

app_repl(constant);
if(c>=100)app_repl('0'+c/100);
if(c>=10)app_repl('0'+(c/10)%10);
app_repl('0'+c%10);
app_repl(constant);
}
break;

/*:89*/
#line 1260 "ctangle.w"

case definition:case format_code:case begin_C:if(t!=section_name)goto done;
else{
err_print("! @d, @f and @c are ignored in C text");continue;

}
case new_section:goto done;

/*:86*/
#line 1207 "ctangle.w"

case')':app_repl(a);
if(t==macro)app_repl(' ');
break;
default:app_repl(a);
}
done:next_control= (eight_bits)a;
if(text_ptr> text_info_end)overflow("text");
cur_text= text_ptr;(++text_ptr)->tok_start= tok_ptr;
}

/*:84*//*92:*/
#line 1360 "ctangle.w"

static void
scan_section(void)
{
name_pointer p;
text_pointer q;
sixteen_bits a;
section_count++;no_where= 1;
if(*(loc-1)=='*'&&show_progress){
printf("*%d",section_count);update_terminal;
}
next_control= 0;
while(1){
/*93:*/
#line 1399 "ctangle.w"

while(next_control<definition)

if((next_control= skip_ahead())==section_name){
loc-= 2;next_control= get_next();
}

/*:93*/
#line 1374 "ctangle.w"

if(next_control==definition){
/*94:*/
#line 1406 "ctangle.w"
{
while((next_control= get_next())=='\n');
if(next_control!=identifier){
err_print("! Definition flushed, must start with identifier");

continue;
}
app_repl(((a= id_lookup(id_first,id_loc,0)-name_dir)/0400)+0200);

app_repl(a%0400);
if(*loc!='('){
app_repl(string);app_repl(' ');app_repl(string);
}
scan_repl(macro);
cur_text->text_link= 0;
}

/*:94*/
#line 1376 "ctangle.w"

continue;
}
if(next_control==begin_C){
p= name_dir;break;
}
if(next_control==section_name){
p= cur_section_name;
/*95:*/
#line 1431 "ctangle.w"

while((next_control= get_next())=='+');
if(next_control!='='&&next_control!=eq_eq)
continue;

/*:95*/
#line 1384 "ctangle.w"

break;
}
return;
}
no_where= print_where= 0;
/*96:*/
#line 1436 "ctangle.w"

/*97:*/
#line 1441 "ctangle.w"

store_two_bytes((sixteen_bits)(0150000+section_count));


/*:97*/
#line 1437 "ctangle.w"

scan_repl(section_name);
/*98:*/
#line 1445 "ctangle.w"

if(p==name_dir||p==0){
(last_unnamed)->text_link= cur_text-text_info;last_unnamed= cur_text;
}
else if(p->equiv==(void*)text_info)p->equiv= (void*)cur_text;

else{
q= (text_pointer)p->equiv;
while(q->text_link<section_flag)
q= q->text_link+text_info;
q->text_link= cur_text-text_info;
}
cur_text->text_link= section_flag;


/*:98*/
#line 1439 "ctangle.w"


/*:96*/
#line 1390 "ctangle.w"

}

/*:92*//*100:*/
#line 1463 "ctangle.w"

static void
phase_one(void){
phase= 1;
section_count= 0;
reset_input();
skip_limbo();
while(!input_has_ended)scan_section();
check_complete();
phase= 2;
}

/*:100*//*102:*/
#line 1481 "ctangle.w"

static void
skip_limbo(void)
{
char c;
while(1){
if(loc> limit&&get_line()==0)return;
*(limit+1)= '@';
while(*loc!='@')loc++;
if(loc++<=limit){
c= *loc++;
if(ccode[(eight_bits)c]==new_section)break;
switch(ccode[(eight_bits)c]){
case translit_code:/*103:*/
#line 1510 "ctangle.w"

while(xisspace(*loc)&&loc<limit)loc++;
loc+= 3;
if(loc> limit||!xisxdigit(*(loc-3))||!xisxdigit(*(loc-2))
||(*(loc-3)>='0'&&*(loc-3)<='7')||!xisspace(*(loc-1)))
err_print("! Improper hex number following @l");

else{
unsigned int i;
char*beg;
sscanf(loc-3,"%x",&i);
while(xisspace(*loc)&&loc<limit)loc++;
beg= loc;
while(loc<limit&&(xisalpha(*loc)||xisdigit(*loc)||*loc=='_'))loc++;
if(loc-beg>=translit_length)
err_print("! Replacement string in @l too long");

else{
strncpy(translit[i-0200],beg,(size_t)(loc-beg));
translit[i-0200][loc-beg]= '\0';
}
}

/*:103*/
#line 1494 "ctangle.w"
break;
case format_code:case'@':break;
case control_text:if(c=='q'||c=='Q'){
while((c= skip_ahead())=='@');
if(*(loc-1)!='>')
err_print("! Double @ should be used in control text");

break;
}
default:err_print("! Double @ should be used in limbo");

}
}
}
}

/*:102*//*104:*/
#line 1536 "ctangle.w"

void
print_stats(void){
puts("\nMemory usage statistics:");
printf("%ld names (out of %ld)\n",
(ptrdiff_t)(name_ptr-name_dir),(long)max_names);
printf("%ld replacement texts (out of %ld)\n",
(ptrdiff_t)(text_ptr-text_info),(long)max_texts);
printf("%ld bytes (out of %ld)\n",
(ptrdiff_t)(byte_ptr-byte_mem),(long)max_bytes);
printf("%ld tokens (out of %ld)\n",
(ptrdiff_t)(tok_ptr-tok_mem),(long)max_toks);
}

/*:104*/
