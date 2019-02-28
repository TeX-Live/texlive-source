
#line 231 "./tex4ht-c.tex"

/* tex4ht.c (2018-07-03-10:36), generated from tex4ht-c.tex
   Copyright 2009-2018 TeX Users Group
   Copyright 1996-2009 Eitan M. Gurari

#line 1 "./tex4ht-c.tex"

%
% This work may be distributed and/or modified under the
% conditions of the LaTeX Project Public License, either
% version 1.3c of this license or (at your option) any
% later version. The latest version of this license is in
%   http://www.latex-project.org/lppl.txt
% and version 1.3c or later is part of all distributions
% of LaTeX version 2005/12/01 or later.
%
% This work has the LPPL maintenance status "maintained".
%
% The Current Maintainer of this work
% is the TeX4ht Project <http://tug.org/tex4ht>.
%
% If you modify this program, changing the
% version identification would be appreciated.
 */

#line 245 "./tex4ht-c.tex"

/* Compiler options (uncommented | command line), as comments:

       Classic C (CC)             default
#define ANSI                      ansi-c, c++
#define DOS_C
#define HAVE_STRING_H             <string.h>
#define HAVE_DIRENT_H             <dirent.h>
#define HAVE_SYS_NDIR_H           <sys/ndir.h>
#define HAVE_SYS_DIR_H            <sys/dir.h>
#define HAVE_NDIR_H               <ndir.h>
#define HAVE_IO_H                 <io.h>
#define HAVE_UNISTD_H             <unistd.h>
#define WIN32
#define KPATHSEA
#define CDECL                     .......
#define BCC32                     bordland c++

*************************************************
    Tex4ht variables                            *
    (uncommented | command line)                *
----------------------------------------------- */

#line 8844 "./tex4ht-c.tex"

#ifndef LGFNT
#define LGFNT "Font(\"%s\",\"%s\",\"%d\",\"%d\")\n"
#endif


#line 10841 "./tex4ht-c.tex"

#ifndef LGCLS
#define LGCLS "Font_Class(%d,\"%s\"): %s\n"
#endif


#line 11065 "./tex4ht-c.tex"

#ifndef LGPIC
#define LGPIC "--- needs --- %%1.idv[%%2] ==> %%3 ---\n%"
#endif


#line 11102 "./tex4ht-c.tex"

#ifndef LGSEP
#define LGSEP "--- characters ---\n"
#endif


#line 11130 "./tex4ht-c.tex"

#ifndef LGTYP
#define LGTYP ".png"
#endif


#line 13170 "./tex4ht-c.tex"

#ifndef ENVFILE

#endif


#line 13552 "./tex4ht-c.tex"

#ifndef TFMDIR

#endif


#line 13924 "./tex4ht-c.tex"

#ifndef HTFDIR

#endif


/* ******************************************** */


#line 294 "./tex4ht-c.tex"

#ifdef BCC32
#define DOS_WIN32
#define ANSI
#define HAVE_DIRENT_H
#define PLATFORM "ms-win32"
#endif



#line 305 "./tex4ht-c.tex"

#ifdef BCC
#define DOS_C
#define ANSI
#define HAVE_DIRENT_H
#define PLATFORM "ms-dos"
#ifndef PATH_MAX
#define PATH_MAX 256
#endif
#endif



#line 615 "./tex4ht-c.tex"

#ifdef __DJGPP__
#define DOS_WIN
#define ANSI
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif
#endif


#ifdef DOS_C
#define DOS
#endif
#ifdef DOS
#define DOS_WIN32
#define HAVE_STRING_H   <string.h>
#endif
#ifdef WIN32
#define DOS_WIN32
#ifndef KPATHSEA
#define HAVE_STRING_H   <string.h>
#endif
#endif

#line 15685 "./tex4ht-c.tex"

#ifdef DOS_WIN32
#define STRUCT_DIRENT
#endif



#line 441 "./tex4ht-c.tex"

#ifdef KPATHSEA
#ifdef WIN32
#define KWIN32
#endif
#endif



#line 361 "./tex4ht-c.tex"

#ifdef KPATHSEA
#include <kpathsea/config.h>
#endif
#include <stdio.h>   
#include <stdlib.h>  


#line 375 "./tex4ht-c.tex"

#ifdef HAVE_STRING_H
#include <string.h>
#endif


#line 457 "./tex4ht-c.tex"

#ifdef WIN32
#ifdef KPATHSEA
#undef CDECL
#define CDECL                     __cdecl
#else
#include <windows.h>
#endif
#else
#ifdef KPATHSEA
#define CDECL
#endif
#endif


#line 1036 "./tex4ht-c.tex"

#include <limits.h> 


#line 1120 "./tex4ht-c.tex"

#include <signal.h>


#line 13767 "./tex4ht-c.tex"

#ifndef F_OK
#ifdef DOS_WIN32
#define  F_OK 0               
#endif
#ifndef KPATHSEA
#ifndef DOS_WIN32
#define HAVE_UNISTD_H
#endif
#endif
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifndef KPATHSEA
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif


#line 14872 "./tex4ht-c.tex"


#line 1231 "./tex4ht-c.tex"

#ifdef KPATHSEA
#include <kpathsea/c-errno.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/proginit.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-make.h>
#include <kpathsea/variable.h>
#include <signal.h>
#if !defined(_AMIGA) && !defined(WIN32)
#include <sys/time.h>
#endif
#include <fcntl.h>
#include <setjmp.h>
#endif 




#line 15623 "./tex4ht-c.tex"

#ifdef DOS_WIN32
#include <io.h>
#endif


#line 15674 "./tex4ht-c.tex"

#include <sys/stat.h>



#line 15692 "./tex4ht-c.tex"

#ifdef HAVE_DIRENT_H

#line 15707 "./tex4ht-c.tex"

#include <dirent.h>


#else
#ifndef STRUCT_DIRENT
#define STRUCT_DIRECT
#endif

#line 15712 "./tex4ht-c.tex"

#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif


#endif



#line 10877 "./tex4ht-c.tex"

#ifdef DOS
#define HTM
#endif




#line 17267 "./tex4ht-c.tex"

#ifdef DOS
#define PROTOTYP
#endif
#ifdef ANSI
#define PROTOTYP
#endif
#ifdef KWIN32
#define PROTOTYP
#endif




#line 1308 "./tex4ht-c.tex"


#line 369 "./tex4ht-c.tex"

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif


#line 1015 "./tex4ht-c.tex"

#if INT_MAX < 2147483647L  
#define LONG L
#endif


#line 1040 "./tex4ht-c.tex"

#ifdef LONG
#define INTEGER long
#else
#define INTEGER int
#endif
#define U_CHAR char


#line 1396 "./tex4ht-c.tex"

#define m_alloc(typ,n) (typ *) malloc_chk((int) ((n) * sizeof(typ)))


#line 1558 "./tex4ht-c.tex"

#ifndef PATH_MAX
#define PATH_MAX 512
#endif


#line 4276 "./tex4ht-c.tex"

#define IGNORED void


#line 4568 "./tex4ht-c.tex"

struct files_rec{
  FILE *file, *prev_file;
  char* name;
  struct files_rec *next, *prev;
};


#line 4669 "./tex4ht-c.tex"

struct sys_call_rec{
  char* filter;
  struct sys_call_rec *next;
};


#line 4850 "./tex4ht-c.tex"

#define HEIGHT 120


#line 4892 "./tex4ht-c.tex"

#define NULL_MAP (struct map_line_type*) 0


#line 4952 "./tex4ht-c.tex"

#define XRESOLUTION MARGINSP
#ifdef LONG
#define YRESOLUTION 786432L
#else
#define YRESOLUTION 786432
#endif


#line 5084 "./tex4ht-c.tex"

#define MAX_MAP_LINE 500


#line 6038 "./tex4ht-c.tex"

#define idv_int(val)     int_to_dvi((long int) val,4)


#line 6293 "./tex4ht-c.tex"

struct halign_rec{
  char * str;
  int    refs;
};


#line 8636 "./tex4ht-c.tex"

#define design_size_to_pt(n)    ((double)n / (double)(1L<<20))


#line 8715 "./tex4ht-c.tex"

#define new_font   font_tbl[font_tbl_size]


#line 9734 "./tex4ht-c.tex"

#ifdef LONG
#define MARGINSP 344061L        
#else
#define MARGINSP 344061
#endif


#line 10124 "./tex4ht-c.tex"

#define  HTF_ALIAS 10000000


#line 10627 "./tex4ht-c.tex"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef BOOL
#define BOOL int
#endif


#line 10869 "./tex4ht-c.tex"

#define GIF_I      "-%x%s"
#define GIF_II  "-%x-%x%s"
#define GIF_VII       "%s"


#line 10883 "./tex4ht-c.tex"

#ifdef HTM
#define DOS_GIF_FILE
#endif


#line 11310 "./tex4ht-c.tex"

#define BASE  36


#line 11330 "./tex4ht-c.tex"

#define store_bit_I(ch,i)  ch[(i)/8]|=(1<<((i)%8));
#define store_bit_Z(ch,i)  ch[(i)/8]&=~(1<<((i)%8))
#define add_bit(ch,i,b)   ch[(i)/8] |= ((b) << ((i)%8))
#define get_bit(ch,i)     ((ch[(i)/8] >> ((i)%8)) & 1)


#line 11675 "./tex4ht-c.tex"

#define gif_open  span_open
#define gif_alt   span_name
#define gif_class span_size
#define gif_size  span_mag
#define gif_mag   span_ord
#define gif_ord   span_ch
#define gif_end   end_span


#line 13414 "./tex4ht-c.tex"

struct env_c_rec{
  char *option;
  struct env_c_rec  *next;
};


#line 13496 "./tex4ht-c.tex"

#ifndef MAXFDIRS
#define MAXFDIRS 100
#endif


#line 15568 "./tex4ht-c.tex"

#if defined(__DJGPP__)
#define dir_path_slash(str) (is_forward_slash(str)? '/' : '\\')
#else
#define dir_path_slash(str)  '/'
#endif


#line 15633 "./tex4ht-c.tex"

#ifndef  S_ISDIR
#define S_ISDIR(M)  (((M) & _S_IFMT)==_S_IFDIR)   
#endif

#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif

#ifndef _S_IFMT
#define _S_IFMT S_IFMT
#endif


#line 15663 "./tex4ht-c.tex"

#if defined(DOS_WIN32) || !defined(S_ISLNK)
#define LSTAT stat
#else
#define LSTAT lstat
#endif
#define STSTAT stat


#line 15883 "./tex4ht-c.tex"

#define MAX_UNI_CODE 20


#line 16076 "./tex4ht-c.tex"

#define BASE_A 55
#define BASE_a 87


#line 16737 "./tex4ht-c.tex"

#define get_unt(n)  fget_unt(dvi_file,n)


#line 16761 "./tex4ht-c.tex"

#define get_int(n)  fget_int(dvi_file,n)


#line 16827 "./tex4ht-c.tex"

#define eq_str(x,y) (!strcmp(x,y))
#define gt_str(x,y) (strcmp(x,y)>0)


#line 17007 "./tex4ht-c.tex"

#define bad_arg            err_i(0)
#define bad_in_file(name)   err_i_str(1,name)
#define bad_out_file(name)  err_i_str(2,name)
#define bad_special(name)   warn_i_str(3,name)
#define bad_mem             err_i(4)
#define bad_char(chr)       warn_i_int(5,chr)
#define bad_dvi             err_i(7)


#line 17231 "./tex4ht-c.tex"

#if defined(DOS_WIN32) || defined(__MSDOS__)
#define READ_BIN_FLAGS "rb"
#define READ_TEXT_FLAGS "r"
#define WRITE_BIN_FLAGS "wb"
#ifdef WIN32
#define WRITE_TEXT_FLAGS "wb"
#else
#define WRITE_TEXT_FLAGS "w"
#endif
#else
#define READ_BIN_FLAGS "r"
#define READ_TEXT_FLAGS "r"
#define WRITE_BIN_FLAGS "w"
#define WRITE_TEXT_FLAGS "w"
#endif


#line 17279 "./tex4ht-c.tex"

#ifdef PROTOTYP
#define MYVOID void
#define ARG_I(x) x
#define ARG_II(x,y) x,y
#define ARG_III(x,y,z) x,y,z
#define ARG_IV(x,y,z,w) x,y,z,w
#define ARG_V(x,y,z,w,v) x,y,z,w,v
#define ARG_VI(x,y,z,w,v,u) x,y,z,w,v,u
#define ARG_VII(x,y,z,w,v,u,t) x,y,z,w,v,u,t
#else
#define MYVOID
#define ARG_I(x)
#define ARG_II(x,y)
#define ARG_III(x,y,z)
#define ARG_IV(x,y,z,w)
#define ARG_V(x,y,z,w,v)
#define ARG_VI(x,y,z,w,v,u)
#define ARG_VII(x,y,z,w,v,u,t)
#endif



#line 4212 "./tex4ht-c.tex"

struct hcode_repl_typ { char                  *str;
                        struct hcode_repl_typ *next;  };


#line 4738 "./tex4ht-c.tex"

struct count_rec{
   char* str;
   int i, depth, max;
   int* stack;
   struct count_rec* next;
};


#line 4830 "./tex4ht-c.tex"

struct ch_map_rec{
  char* line;
  int max, chars;
};


#line 6669 "./tex4ht-c.tex"

struct stack_entry{
  long  int x_val, y_val;
  INTEGER dx_1, dx_2, dy_1, dy_2;
  BOOL text_on;
  BOOL 
#line 12464 "./tex4ht-c.tex"

accented

;
  
#line 6348 "./tex4ht-c.tex"

struct halign_rec *halign[8];
BOOL halign_on, halign_info, row_no, col_no;


#line 6697 "./tex4ht-c.tex"

int stack_id;
struct group_info * begin;
struct stack_end_entry * end;


#line 7789 "./tex4ht-c.tex"

struct group_path * path_start, * path_end;


#line 12872 "./tex4ht-c.tex"

char *class_open, *class_close,
     *temp_class_open, *temp_class_close;
BOOL temp_class_del, ignore_subclass_del, active_class_del,
    no_left_del, sv_no_left_del;


};


#line 6704 "./tex4ht-c.tex"

struct group_info{
  int stack_id;
  U_CHAR *info;
  struct group_info* next;
};


#line 6762 "./tex4ht-c.tex"

struct stack_end_entry{
  struct stack_end_entry *next;
  U_CHAR *send;
};


#line 7444 "./tex4ht-c.tex"

struct del_stack_entry{
  struct del_stack_entry *next;
  U_CHAR ch;
  int  fnt, id;
};


#line 7530 "./tex4ht-c.tex"

struct send_back_entry{
  struct send_back_entry *next;
  U_CHAR *send;
  int  id;
};


#line 7794 "./tex4ht-c.tex"

struct group_path{
  U_CHAR action;
  U_CHAR *path;
  U_CHAR *info;
  struct group_path * next;
};


#line 8620 "./tex4ht-c.tex"

struct font_entry {
 INTEGER num;
 INTEGER scale;
 INTEGER design_sz;
 
#line 8772 "./tex4ht-c.tex"

INTEGER mag;


 
#line 8776 "./tex4ht-c.tex"

char *family_name, *font_size;


 
#line 9446 "./tex4ht-c.tex"

 INTEGER design_pt;     
 int char_f, char_l;    
 U_CHAR *char_wi;
 U_CHAR *char_hidp;
 int  wtbl_n;
 int  htbl_n;
 int  dtbl_n;
 INTEGER  *wtbl;              
 INTEGER  *htbl;              
 INTEGER  *dtbl;              
 INTEGER  word_sp;            
 INTEGER  it;                 
 INTEGER  ex;                 


 
#line 9954 "./tex4ht-c.tex"

char *name, *gif_on, *ch_str, ch255;  
unsigned U_CHAR **str, *ch, *gif1;


#line 12073 "./tex4ht-c.tex"

unsigned U_CHAR *accent, *accented;
unsigned int *accent_array, *accented_array, accent_N, accented_N;


 
#line 12764 "./tex4ht-c.tex"

char *math, *math_closing;


 
#line 8941 "./tex4ht-c.tex"

INTEGER layout_dir;
unsigned long rgba_color;


};


#line 10274 "./tex4ht-c.tex"

struct html_font_rec{  char* name;
                       int   i;     };


#line 11222 "./tex4ht-c.tex"


struct gif_file_rec{
    U_CHAR                code[4];
    U_CHAR                *name;
    struct gif_file_rec *next;     };



#line 11863 "./tex4ht-c.tex"

struct css_ext_rec{  char* name;
                     struct css_ext_rec *next;     };


#line 11973 "./tex4ht-c.tex"

struct visited_file_rec{  char *name;
       struct visited_file_rec *next;     };


#line 13655 "./tex4ht-c.tex"

struct env_var_rec{  char* base;
                     struct env_var_rec *next;     };


#line 13733 "./tex4ht-c.tex"

struct cache_font_rec{  char* dir;
                        struct cache_file_rec * cache_file;
                        struct cache_font_rec* next;     };
struct cache_file_rec{  struct cache_file_rec* next;
                        U_CHAR *                 file;     };


#line 13854 "./tex4ht-c.tex"

struct htf_com_rec{  char* name;
                     struct htf_com_rec* next;     };


#line 16042 "./tex4ht-c.tex"

struct charset_rec{ int  ch;
                    char* str; };


#line 16454 "./tex4ht-c.tex"

struct htf_4hf_rec { int   ch, type1, type2;
                     char* str; };



#line 528 "./tex4ht-c.tex"

#ifdef WIN32
static U_CHAR dirname[PATH_MAX];
#endif


#line 625 "./tex4ht-c.tex"

static BOOL dos_file_names =
#ifdef DOS_GIF_FILE
  TRUE
#else
  FALSE
#endif
;


#line 1381 "./tex4ht-c.tex"

static FILE*  dot_file;


#line 1572 "./tex4ht-c.tex"

static FILE* dvi_file;


#line 1629 "./tex4ht-c.tex"

static U_CHAR *ext = NULL;


#line 1670 "./tex4ht-c.tex"

static char* job_name;
static int   job_name_n;


#line 1779 "./tex4ht-c.tex"

static U_CHAR *no_root_file;


#line 1840 "./tex4ht-c.tex"

static FILE *out_file  = (FILE *) 0,
     *root_file = (FILE *) 0,
     *cur_o_file = (FILE *) 0;


#line 2166 "./tex4ht-c.tex"

static int version_id;


#line 2213 "./tex4ht-c.tex"

static int  stack_len;


#line 2581 "./tex4ht-c.tex"

static BOOL start_span = FALSE, in_span_ch = FALSE;


#line 2726 "./tex4ht-c.tex"

static BOOL in_trace_char = FALSE, block_start = FALSE;
static int trace_dvi_P = 0, trace_dvi_C = 0,
     trace_dvi_H = 0, trace_dvi_R = 0, trace_dvi_V = 0;
static U_CHAR *trace_dvi_del_P,  *end_trace_dvi_del_P,
     *trace_dvi_del_p,  *end_trace_dvi_del_p,
     *trace_dvi_del_C,  *end_trace_dvi_del_C,
     *trace_dvi_del_c,  *end_trace_dvi_del_c,
     *trace_dvi_del_H,  *end_trace_dvi_del_H,
     *trace_dvi_del_h,  *end_trace_dvi_del_h,
     *trace_dvi_del_R,  *end_trace_dvi_del_R,
     *trace_dvi_del_r,  *end_trace_dvi_del_r,
     *trace_dvi_del_V,  *end_trace_dvi_del_V,
     *trace_dvi_del_v,  *end_trace_dvi_del_v;
static int push_depth=0, push_id=0, push_st[256];


#line 2959 "./tex4ht-c.tex"

static long int x_val = 0, max_x_val = -10000,
     max_y_val = 0, prev_y_val = 0;


#line 3071 "./tex4ht-c.tex"

static INTEGER dx_1 = 0, dx_2 = 0;


#line 3172 "./tex4ht-c.tex"

static INTEGER  dy_1 = 0, dy_2 = 0;
static long int y_val = 0;


#line 3415 "./tex4ht-c.tex"

static U_CHAR *eoln_str = (char *)0;


#line 3436 "./tex4ht-c.tex"

static U_CHAR *space_str = (char *)0;


#line 3447 "./tex4ht-c.tex"

static int ignore_chs=0, ignore_spaces=0, recover_spaces=0;


#line 3600 "./tex4ht-c.tex"

static BOOL text_on = FALSE;


#line 3673 "./tex4ht-c.tex"

static U_CHAR rule_ch = '_';
static BOOL 
#line 3739 "./tex4ht-c.tex"

rule_ch_off

 = FALSE;


#line 3819 "./tex4ht-c.tex"

static int cur_fnt = -1;  


#line 3937 "./tex4ht-c.tex"

static U_CHAR special_hd[10];


#line 4218 "./tex4ht-c.tex"

static struct hcode_repl_typ *hcode_repl
                       = (struct hcode_repl_typ*) 0;


#line 4270 "./tex4ht-c.tex"

static BOOL nomargin = FALSE;
static int next_char = -1;
static U_CHAR  *next_str = (char *) 0;


#line 4357 "./tex4ht-c.tex"

static BOOL keepChar = FALSE;


#line 4563 "./tex4ht-c.tex"

static struct files_rec
   *opened_files = (struct files_rec *) 0, *p;


#line 4677 "./tex4ht-c.tex"

static BOOL system_yes;
static struct sys_call_rec *system_calls = (struct sys_call_rec *) 0;


#line 4747 "./tex4ht-c.tex"

static struct count_rec *counter = (struct count_rec *) 0;


#line 4838 "./tex4ht-c.tex"

static struct ch_map_rec  ch_map[HEIGHT];
static   int max_map_line, min_map_line;


#line 4884 "./tex4ht-c.tex"

static BOOL ch_map_flag = FALSE;


#line 4939 "./tex4ht-c.tex"

static INTEGER xresolution, yresolution;


#line 5039 "./tex4ht-c.tex"

static U_CHAR ok_map = TRUE;


#line 5078 "./tex4ht-c.tex"

static int prevcol = -1, prevrow;
static double prev_x;


#line 5385 "./tex4ht-c.tex"

static BOOL  dvi_flag = FALSE, dvi_page = FALSE;
static FILE *idv_file;


#line 5426 "./tex4ht-c.tex"

static int errCode = 0;


#line 5485 "./tex4ht-c.tex"

static int id_version = -1;


#line 5964 "./tex4ht-c.tex"

static FILE*  log_file;


#line 5973 "./tex4ht-c.tex"

static INTEGER mid_page_y, mid_page_x;


#line 6106 "./tex4ht-c.tex"

static int page_n,  file_n;


#line 6289 "./tex4ht-c.tex"

static struct halign_rec *halign[8];


#line 6374 "./tex4ht-c.tex"

static BOOL new_halign = FALSE;


#line 6600 "./tex4ht-c.tex"

static BOOL group_dvi = FALSE;


#line 6664 "./tex4ht-c.tex"

static int stack_n = 0;
static struct stack_entry* stack;


#line 6798 "./tex4ht-c.tex"

static int ignore_end_group;


#line 7435 "./tex4ht-c.tex"

static struct del_stack_entry  *del_stack;


#line 7460 "./tex4ht-c.tex"

static int ch_id, sv_id, id_latex, back_id_off;


#line 7539 "./tex4ht-c.tex"

static struct send_back_entry *back_token, *back_group;


#line 8442 "./tex4ht-c.tex"

static BOOL pos_dvi = FALSE;
static U_CHAR   *pos_body,     * pos_text,     * pos_line,
       *end_pos_body, * end_pos_text;
static double   pos_x_A, pos_x_B, pos_y_C, pos_y_D, pos_y_E;
static long int base_pos_x, base_pos_y, min_pos_x,
         max_pos_x, min_pos_y, max_pos_y;
static short rect_pos;


#line 8640 "./tex4ht-c.tex"

static struct font_entry*  font_tbl;
static int font_tbl_size = 0;


#line 8709 "./tex4ht-c.tex"

static char*  new_font_name;


#line 8851 "./tex4ht-c.tex"

static U_CHAR *lg_font_fmt = NULL;


#line 9742 "./tex4ht-c.tex"

static double word_sp   = 999999.0, margin_sp;


#line 9976 "./tex4ht-c.tex"

static int ignore_ch = 0;


#line 10020 "./tex4ht-c.tex"

static unsigned  U_CHAR  null_str = '\0';    


#line 10486 "./tex4ht-c.tex"

static short dump_htf_files = 0;
static BOOL dump_env_files = FALSE;


#line 10536 "./tex4ht-c.tex"

static BOOL dumped_env = FALSE;


#line 10693 "./tex4ht-c.tex"

static BOOL verb_ch = FALSE;


#line 10848 "./tex4ht-c.tex"

static U_CHAR *class_fmt = NULL;


#line 11072 "./tex4ht-c.tex"

static U_CHAR *font_gif = NULL;


#line 11109 "./tex4ht-c.tex"

static U_CHAR *begin_char_gif = NULL;


#line 11137 "./tex4ht-c.tex"

static U_CHAR *gif = NULL;


#line 11235 "./tex4ht-c.tex"


static struct gif_file_rec *   gif_file = (struct gif_file_rec *) 0;



#line 11314 "./tex4ht-c.tex"


static U_CHAR xeh[]="0123456789abcdefghijklmnopqrstuvxyz";



#line 11386 "./tex4ht-c.tex"

static BOOL gif_ch = TRUE;
static int design_ch = 0;


#line 11436 "./tex4ht-c.tex"

static int    pause_style = 0, default_font = -1, base_font_size=6533;
static BOOL
  span_name_on = FALSE,
  span_on = FALSE;


#line 11638 "./tex4ht-c.tex"

static BOOL not_notify = FALSE;


#line 11654 "./tex4ht-c.tex"

static U_CHAR * span_name[256], * span_open[256], * span_size[256],
     * span_mag[256],  * span_ch[256],   * end_span[256],
     * span_ord[256],  * gif_id[256];
static U_CHAR class_on[32];


#line 12334 "./tex4ht-c.tex"

static BOOL needs_accent_sym = FALSE,  needs_end_accent = FALSE;
static char  * t_accent_template = (char *) 0,
             * t_accent_first, * t_accent_second,
             * t_accent_third, * t_accent_fourth, * t_accent_fifth,
             * m_accent_template = (char *) 0,
             * m_accent_first, * m_accent_second,
             * m_accent_third, * m_accent_fourth, * m_accent_fifth;


#line 12435 "./tex4ht-c.tex"

static BOOL  needs_accented_sym = 0;
static char  * a_accent_template = (char *) 0,
             * a_accent_first, * a_accent_second,
             * a_accent_third, * a_accent_fourth, * a_accent_fifth;


#line 12451 "./tex4ht-c.tex"

static char  * i_accent_template = (char *) 0,
             * i_accent_first, * i_accent_second,
             * i_accent_third, * i_accent_fourth, * i_accent_fifth;


#line 12553 "./tex4ht-c.tex"

static BOOL math_class_on = FALSE, show_class = FALSE;
static int open_del = 256, math_class, pause_class, ignore_subclass_del;


#line 12561 "./tex4ht-c.tex"

static int sv_group_dvi, sv_trace_dvi_C, sv_in_trace_char, sv_span_on,
    sv_in_span_ch;


#line 12707 "./tex4ht-c.tex"

static U_CHAR *open_class[
#line 12743 "./tex4ht-c.tex"

82   

],
     *close_class[
#line 12743 "./tex4ht-c.tex"

82   

];


#line 13058 "./tex4ht-c.tex"

static BOOL dump_env_search = FALSE;


#line 13421 "./tex4ht-c.tex"

static struct env_c_rec *envChoice
       = (struct env_c_rec*) 0;


#line 13489 "./tex4ht-c.tex"

static U_CHAR  *fontdir[MAXFDIRS];
static int fontdir_count = 0;


#line 13724 "./tex4ht-c.tex"

static struct cache_font_rec *cache_font,  *cur_cache_font;


#line 14140 "./tex4ht-c.tex"

#ifndef KPATHSEA
static BOOL tex4ht_fls = FALSE;
static char *tex4ht_fls_name = (char *) 0;
#endif


#line 14203 "./tex4ht-c.tex"

static U_CHAR *HOME_DIR;


#line 14332 "./tex4ht-c.tex"

#ifndef KPATHSEA
static FILE* cache_files;
#endif


#line 14656 "./tex4ht-c.tex"


#ifdef KPATHSEA
static char * export_str_chars = (char *) 0;
#endif


#line 15062 "./tex4ht-c.tex"

static BOOL dump_htf_search = FALSE;


#line 15876 "./tex4ht-c.tex"

static FILE* put_4ht_file = (FILE *) 0;
static int put_4ht_off = 1;
static char uni_code[MAX_UNI_CODE];
static short uni_code_p = 0;


#line 16047 "./tex4ht-c.tex"

static int charset_n = 0, max_charset_n;
static struct charset_rec *charset;


#line 16215 "./tex4ht-c.tex"

static BOOL u10 = FALSE;


#line 16358 "./tex4ht-c.tex"

static BOOL utf8 = FALSE;


#line 16459 "./tex4ht-c.tex"

static int htf_4hf_n = 0, max_htf_4hf_n;
static struct htf_4hf_rec *htf_4hf;


#line 16633 "./tex4ht-c.tex"

static BOOL special_on = FALSE;


#line 16901 "./tex4ht-c.tex"

static const U_CHAR *warn_err_mssg[]={ 
#line 16842 "./tex4ht-c.tex"


#line 1530 "./tex4ht-c.tex"

"improper command line\ntex4ht [-f<path-separator-ch>]in-file[.dvi]\n"
"   [-.<ext>]            replacement to default file extension name .dvi\n"
"   [-c<tag name>]       choose named segment in env file\n"
"   [-e<env-file>]\n"
"   [-f<path-separator-ch>]        remove path from the file name\n"
"   [-F<ch-code>]        replacement for missing font characters; 0--255; default 0\n"
"   [-g<bitmap-file-ext>]\n"
"   [-h[efFgsvVA]]       trace: e-errors/warnings, f-htf, F-htf search\n"
"                           g-groups, s-specials, v-env, V-env search, A-all\n"
"   [-i<htf-font-dir>]\n"
"   [-l<bookkeeping-file>]\n"
"   [-P(*|<filter>)]     permission for system calls: *-always, filter\n"
"   [-S<image-script>]\n"
"   [-s<css-file-ext>]   default: -s4cs; multiple entries allowed\n"
"   [-t<tfm-font-dir>]\n"
"   [-u10]               base 10 for unicode characters\n"
"   [-utf8]              utf-8 encoding for unicode characters\n"
"   [-v<idv version>]    replacement for the given dvi version\n"
"   [-xs]           ms-dos file names for automatically generated gifs\n"

,                            
"Can't find/open file `%s'\n",                       
"Can't open output file for `%s'\n",                 
"Can't close file `%s' (file is not open)\n",        
"Insufficient memory\n",                              
"Bad character code: %d\n",                           
"Can't find font number %d\n",                        
"Improper dvi file\n",                                
"Improper op while scanning font defs in postamble\n",
"Problem with command line\n",                        
"Font definition repeated in postamble\n",            
"Empty entry in font environment variable\n",         
"Can't access directory `%s\n'",                     
"Too many directories in font environment variable\n",
"Missing fonts, can't proceed\n",                     
"Invalid header in file `%s'\n",                     
"Checksum inconsistent\n",                            
"MAXFONTS too small: %d\n",                           
"Improper signature at end of file `%s.htf'\n",      
"Improper signature at start of file `%s.htf'\n",    
"Improper file `%s.htf'\n",                          
"Couldn't find font `%s.htf' (char codes: ",        
"File `%s.htf' starts/ends with character code %d (instead of %d)\n",
"Implementation problem\n",                           
"Improper groups in \\special{t4ht+}... idv[%d]\n",   
"Too many characters (> %d) for map line: `%c'\n",   
"Extra characters in \\special{t4ht%c...",            
"Page break within a ch map/picture\n",               
"Char code >255 in htf file: %d\n",                   
"Improper char for code in htf file: %c\n",           

#line 1220 "./tex4ht-c.tex"

"Illegal storage address\n", 
"Floating-point\n",          
"Interrupt with Cntr-C\n",   

                           
#ifdef DOS_WIN32
"%c-script too long in tex4ht.env \n",                
#else
"%c-script too long in tex4ht.env (.tex4ht)\n",       
#endif
"Too many rows (> %d) for map: `%c'\n",              
"More than 256 strings in font\n",                    
"\\special{t4ht;%c...}?\n",                           
"\\special{t4ht;|%s}?\n",                             
"\\special{t4ht~!%s}?\n",                             
"\\special{t4ht\"...%s}?\n",                          
"System error 40\n",                                  
"`%c' in \\special{t4ht@...} or \\special{t4ht@-...}?\n",   
"\\special{t4ht~...} without \\special{t4ht~}\n",     
"Ignoring \\special{t4ht.%s}\n",                      
"PUSH for \\special{t4ht<...%s}?\n",                  
"Bad character code (%d) in \\special{t4h~}...\n",    
"Page break in \\special{t4h~}...\n",                 
"tex4ht.fls: Couldn't find file `%s'\n",             
"Improper entry (line %d)\n",                         
"Improper environment variable %s: `%s'\n",           
"Missing %s\n",                                       
"Can't back from file `%s\n'",                        
"\\special{t4ht%s}?\n",                               
"Improper -v option\n",                               

 "" };


#line 17132 "./tex4ht-c.tex"

static BOOL err_context = FALSE;


#line 17149 "./tex4ht-c.tex"

static U_CHAR *err_mark = (char *) 0;


#line 17169 "./tex4ht-c.tex"

static BOOL trace_special = FALSE;



#line 482 "./tex4ht-c.tex"

#ifdef WIN32
static BOOL sigint_handler(ARG_I(DWORD));
#endif


#line 1124 "./tex4ht-c.tex"

static  void

#line 507 "./tex4ht-c.tex"

#ifdef CDECL
CDECL
#endif


sig_err(ARG_I(int));


#line 1402 "./tex4ht-c.tex"

static void* malloc_chk(ARG_I(int));


#line 1418 "./tex4ht-c.tex"

static void* r_alloc(ARG_II(void *, size_t));


#line 1681 "./tex4ht-c.tex"

static void strct( ARG_II(char *, const U_CHAR *) );


#line 1720 "./tex4ht-c.tex"

static void open_o_file( ARG_I(void) );


#line 1820 "./tex4ht-c.tex"

static FILE* open_html_file( ARG_I(char*) );


#line 2798 "./tex4ht-c.tex"

static void set_del( ARG_II(char **, U_CHAR **) );


#line 2989 "./tex4ht-c.tex"

static void try_new_line( ARG_I(void) );


#line 3078 "./tex4ht-c.tex"

static INTEGER move_x( ARG_I(register INTEGER) );


#line 3177 "./tex4ht-c.tex"

static INTEGER move_y( ARG_I(register INTEGER) );


#line 3647 "./tex4ht-c.tex"

static void rule_x( ARG_I(BOOL) );


#line 3877 "./tex4ht-c.tex"

static BOOL tex4ht_special( ARG_II( int*, long int*) );


#line 4962 "./tex4ht-c.tex"

static void init_ch_map( ARG_I(void) );


#line 4999 "./tex4ht-c.tex"

static void insert_ch_map( ARG_II(char,BOOL) );


#line 5289 "./tex4ht-c.tex"

static void dump_ch_map( ARG_I(void) );


#line 5783 "./tex4ht-c.tex"

static void  set_loc( ARG_II(int, long int) );


#line 6002 "./tex4ht-c.tex"

static void idv_char( ARG_I(int) );


#line 6015 "./tex4ht-c.tex"

static void cond_idv_char( ARG_I(int) );


#line 6030 "./tex4ht-c.tex"

static void idv_copy( ARG_I(void) );


#line 6045 "./tex4ht-c.tex"

static void cond_idv_int( ARG_II(long int, int) );


#line 6059 "./tex4ht-c.tex"

static void  int_to_dvi( ARG_II(long int, int) );


#line 6081 "./tex4ht-c.tex"

static void cond_string( ARG_II(int, int) );


#line 6111 "./tex4ht-c.tex"

static INTEGER advance_idv_page( ARG_II( INTEGER,char*) );


#line 6154 "./tex4ht-c.tex"

static void store_mv( ARG_II( int, INTEGER) );


#line 7237 "./tex4ht-c.tex"

static void push_stack( ARG_I(void) );


#line 7284 "./tex4ht-c.tex"

static void pop_stack( ARG_I(void) );


#line 7385 "./tex4ht-c.tex"

static struct del_stack_entry* push_del( ARG_II(char, int) );


#line 7406 "./tex4ht-c.tex"

static struct del_stack_entry* pop_del( ARG_III(char,int,int) );


#line 7618 "./tex4ht-c.tex"

static
struct send_back_entry * rev_list( ARG_I(struct send_back_entry *) );


#line 7672 "./tex4ht-c.tex"

static struct send_back_entry *
   back_insert( ARG_II(struct send_back_entry *, int) );


#line 8546 "./tex4ht-c.tex"

static double pos_dbl( ARG_I(long int *) );


#line 9266 "./tex4ht-c.tex"

static void doGlyphArray( ARG_I(BOOL) );


#line 9314 "./tex4ht-c.tex"

static int search_font_tbl( ARG_I(int) );


#line 10415 "./tex4ht-c.tex"

static int get_html_ch( ARG_I(FILE*) );


#line 10436 "./tex4ht-c.tex"

static FILE* f_open( ARG_II(const char*,const char*) );


#line 10460 "./tex4ht-c.tex"

static void dump_htf( ARG_I(FILE*) );


#line 10512 "./tex4ht-c.tex"

static void dump_env( ARG_I(void) );


#line 10545 "./tex4ht-c.tex"

static void htf_to_lg( ARG_IV(struct html_font_rec*,char*,int,FILE*));


#line 10595 "./tex4ht-c.tex"

static INTEGER get_html_file_id( ARG_IV(FILE*, int, int, int) );


#line 10729 "./tex4ht-c.tex"

static void notify_class_info( ARG_I(int) );


#line 10953 "./tex4ht-c.tex"

static void script( ARG_IV(char *, U_CHAR *, int, U_CHAR *) );


#line 11175 "./tex4ht-c.tex"


static void  dos_gif_file( ARG_III(char *, int, int) );



#line 11344 "./tex4ht-c.tex"

static void put_alt_ch( ARG_II( int, BOOL) );


#line 12347 "./tex4ht-c.tex"

static void get_open_accent(
    ARG_VII(char**, char**, char**, char**, char**, char**, long int*));


#line 12589 "./tex4ht-c.tex"

static int scan_class( ARG_I(int) );


#line 12629 "./tex4ht-c.tex"

static INTEGER set_ch_class( ARG_I(int) );


#line 12684 "./tex4ht-c.tex"

static int math_class_of( ARG_II(int,int) );


#line 13290 "./tex4ht-c.tex"

static char* get_script( ARG_III(char *, const U_CHAR *, int) );


#line 13340 "./tex4ht-c.tex"

static BOOL search_dot_file( ARG_I( int) );


#line 13580 "./tex4ht-c.tex"

static struct env_var_rec * get_env_var( ARG_I(const char *) );


#line 13883 "./tex4ht-c.tex"

static void com_dir( ARG_I(char*) );


#line 14574 "./tex4ht-c.tex"

#ifdef KPATHSEA
static void export_htf( ARG_II(char**, char[]) );
#endif


#line 15304 "./tex4ht-c.tex"

static FILE*  search_in_dot_file( ARG_IV( int, const U_CHAR *, const U_CHAR *,
                                    struct env_var_rec *) );


#line 15349 "./tex4ht-c.tex"

static FILE*  search_file_base( ARG_IV( const U_CHAR *, const U_CHAR *, const U_CHAR *,
                                    struct env_var_rec *) );


#line 15381 "./tex4ht-c.tex"

static char *  abs_addr( ARG_II( const U_CHAR *, const U_CHAR *) );


#line 15422 "./tex4ht-c.tex"

static FILE* search_file( ARG_III(const char *, const U_CHAR *, const U_CHAR *) );


#line 15482 "./tex4ht-c.tex"

static void add_to_cache( ARG_III(const char*,const char*,int) );


#line 15503 "./tex4ht-c.tex"

static FILE* search_file_ext( ARG_III(const char *, const U_CHAR *, const U_CHAR *) );


#line 15578 "./tex4ht-c.tex"

#if defined(__DJGPP__)
   static BOOL is_forward_slash( ARG_I(const char*) );
#endif


#line 15802 "./tex4ht-c.tex"

static FILE* f_open_pathed_filename( ARG_II(const char*,const char*) );


#line 15836 "./tex4ht-c.tex"

static INTEGER put_4ht_ch( ARG_II(int,FILE *) );


#line 15910 "./tex4ht-c.tex"

static void flush_uni( ARG_I(void) );


#line 16573 "./tex4ht-c.tex"

static INTEGER insert_ch( ARG_I(int) );


#line 16611 "./tex4ht-c.tex"

static void put_char( ARG_I(int) );


#line 16644 "./tex4ht-c.tex"

static void print_f( ARG_I(const char*) );


#line 16664 "./tex4ht-c.tex"

static void print_f_4ht( ARG_I(const char*) );


#line 16689 "./tex4ht-c.tex"

static int get_char( ARG_I(void) );


#line 16701 "./tex4ht-c.tex"

static int get_noop( ARG_I(void) );


#line 16717 "./tex4ht-c.tex"

static char* get_str( ARG_I(int) );


#line 16741 "./tex4ht-c.tex"

static long fget_unt( ARG_II(FILE*, int) );


#line 16765 "./tex4ht-c.tex"

static long fget_int( ARG_II(FILE *, int) );


#line 16801 "./tex4ht-c.tex"

static long cond_int( ARG_I(register  INTEGER) );


#line 16908 "./tex4ht-c.tex"

static void warn_i( ARG_I(int) );


#line 16932 "./tex4ht-c.tex"

static void warn_i_int( ARG_II(int,int) );


#line 16948 "./tex4ht-c.tex"

static void warn_i_int_2( ARG_III(int,int,int) );


#line 16964 "./tex4ht-c.tex"

static void warn_i_str( ARG_II(int,const char *) );


#line 16984 "./tex4ht-c.tex"

static void warn_i_str2( ARG_III(int,const char *,const char *) );


#line 17018 "./tex4ht-c.tex"

static void err_i( ARG_I(int) );


#line 17035 "./tex4ht-c.tex"

static void err_i_int( ARG_II(int,int) );


#line 17051 "./tex4ht-c.tex"

static void err_i_str( ARG_II(int,char *) );


#line 17098 "./tex4ht-c.tex"

static void show_err_context( ARG_I(void) );



#line 488 "./tex4ht-c.tex"

#ifdef WIN32



static BOOL  sigint_handler
#ifdef ANSI
#define SEP ,
(  DWORD dwCtrlType
)
#undef SEP
#else
#define SEP ;
(dwCtrlType)  DWORD dwCtrlType
;
#undef SEP
#endif
{
  
#line 502 "./tex4ht-c.tex"

if( dwCtrlType ){ (IGNORED) printf(" "); }


  err_i(32);
  return FALSE;      
}
#endif


#line 1133 "./tex4ht-c.tex"


static void

#line 507 "./tex4ht-c.tex"

#ifdef CDECL
CDECL
#endif


sig_err
#ifdef ANSI
#define SEP ,
(  int s
)
#undef SEP
#else
#define SEP ;
(s)  int s
;
#undef SEP
#endif
{
  (void) signal(s,SIG_IGN);  
  switch( s ){
#ifdef SIGSEGV
    case SIGSEGV: err_i(30);
#endif
    case SIGFPE : err_i(31);
#if defined(SIGINT) && !defined(WIN32)
    case SIGINT : err_i(32);
#endif
  }
  
#line 750 "./tex4ht-c.tex"

#ifdef __DJGPP__
  if (s != SIGINT && s != SIGQUIT)
    exit(EXIT_FAILURE);
#endif


}


#line 1406 "./tex4ht-c.tex"


static void* malloc_chk
#ifdef ANSI
#define SEP ,
( int n
)
#undef SEP
#else
#define SEP ;
( n ) int n
;
#undef SEP
#endif
{      void* p;
   if((p = (void *) malloc( (size_t) n)) == NULL ) bad_mem;
   return p;
}


#line 1422 "./tex4ht-c.tex"


static void* r_alloc
#ifdef ANSI
#define SEP ,
(
      void   *q SEP 
      size_t  n
)
#undef SEP
#else
#define SEP ;
( q, n )
      void   *q SEP 
      size_t  n
;
#undef SEP
#endif
{    void*   p;
   if((p = (void *) realloc( q, (size_t) n)) == NULL) bad_mem;
   return p;
}


#line 1685 "./tex4ht-c.tex"


static void strct
#ifdef ANSI
#define SEP ,
(
     U_CHAR * str1 SEP 
     const U_CHAR * str2

)
#undef SEP
#else
#define SEP ;
( str1, str2 )
     U_CHAR * str1 SEP 
     const U_CHAR * str2

;
#undef SEP
#endif
{   U_CHAR * ch;
   ch = str1 + (int) strlen((char *) str1);
   (IGNORED) strcpy((char *)  ch, str2 );
}


#line 1724 "./tex4ht-c.tex"

static void open_o_file(MYVOID)
{
   
#line 1735 "./tex4ht-c.tex"

    struct files_rec* p;
p = m_alloc(struct files_rec, 1);
if( opened_files != (struct files_rec*) 0 ) opened_files->prev = p;
p->prev = (struct files_rec *) 0;
p->next = opened_files;     opened_files = p;
p->name = no_root_file;
p->file =


   cur_o_file = root_file = open_html_file(no_root_file);
   no_root_file = (char *) 0;
}


#line 1824 "./tex4ht-c.tex"


static FILE* open_html_file
#ifdef ANSI
#define SEP ,
(
     char* name
)
#undef SEP
#else
#define SEP ;
(name)
     char* name
;
#undef SEP
#endif
{   FILE* file;
     char* str;
  str = m_alloc(char, (int) strlen((char *) name) +  1);
  (IGNORED) strcpy((char *) str, (char *) name);
  (IGNORED) printf(" file %s\n", str);
  (IGNORED) fprintf(log_file, "File: %s\n", str);
  if( (file = fopen(str, WRITE_TEXT_FLAGS)) == NULL )  bad_in_file(str);
  free((void *)  str);
  return file;
}


#line 2802 "./tex4ht-c.tex"


static  void set_del
#ifdef ANSI
#define SEP ,
(
     U_CHAR ** del SEP 
     U_CHAR ** end_del
)
#undef SEP
#else
#define SEP ;
( del, end_del )
     U_CHAR ** del SEP 
     U_CHAR ** end_del
;
#undef SEP
#endif
{
  *del = m_alloc(char, 1);       **del = '\0';
  *end_del = m_alloc(char, 1);   **end_del = '\0';
}


#line 2993 "./tex4ht-c.tex"

static void try_new_line(MYVOID)
{        long int  v;
         double    dy;
   dy =  (cur_fnt == -1)? 0.0 : (
#line 3016 "./tex4ht-c.tex"

(
#line 9905 "./tex4ht-c.tex"

design_size_to_pt( 1.7 * (double) font_tbl[cur_fnt].ex )
* (double) font_tbl[cur_fnt].scale

 < 0? -1 : 1)

 * 
#line 9905 "./tex4ht-c.tex"

design_size_to_pt( 1.7 * (double) font_tbl[cur_fnt].ex )
* (double) font_tbl[cur_fnt].scale

) ;
   v = y_val - prev_y_val;
   if( !text_on && (y_val > max_y_val) ){
     if( v > dy/2.5 ){
        
#line 1748 "./tex4ht-c.tex"

if( !no_root_file ){
  
#line 12281 "./tex4ht-c.tex"

if( needs_end_accent && t_accent_template ){
   
#line 2609 "./tex4ht-c.tex"

if( span_on && in_span_ch ){
   if( *end_span[0] ){
       in_span_ch = FALSE;
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       (IGNORED) fprintf(cur_o_file, "%s", end_span[0]);
}  }


   (IGNORED) fprintf(cur_o_file, "%s", t_accent_fifth);
   needs_end_accent = FALSE; 
#line 12424 "./tex4ht-c.tex"

needs_accented_sym--;


}


  if( !
#line 3739 "./tex4ht-c.tex"

rule_ch_off

 ){  put_char('\n'); }
}

  max_x_val = -10000;
        prev_y_val = max_y_val  = stack_n? y_val : 0;
     }
   }else{
      if( v > dy ){ 
#line 1748 "./tex4ht-c.tex"

if( !no_root_file ){
  
#line 12281 "./tex4ht-c.tex"

if( needs_end_accent && t_accent_template ){
   
#line 2609 "./tex4ht-c.tex"

if( span_on && in_span_ch ){
   if( *end_span[0] ){
       in_span_ch = FALSE;
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       (IGNORED) fprintf(cur_o_file, "%s", end_span[0]);
}  }


   (IGNORED) fprintf(cur_o_file, "%s", t_accent_fifth);
   needs_end_accent = FALSE; 
#line 12424 "./tex4ht-c.tex"

needs_accented_sym--;


}


  if( !
#line 3739 "./tex4ht-c.tex"

rule_ch_off

 ){  put_char('\n'); }
}

  max_x_val = x_val;
                    prev_y_val = stack_n? y_val : 0;
      }else if( v < -(dy / 1.4) ) prev_y_val = stack_n? y_val : 0;
}  }


#line 3082 "./tex4ht-c.tex"


static INTEGER move_x
#ifdef ANSI
#define SEP ,
(
      register INTEGER  d
)
#undef SEP
#else
#define SEP ;
( d )
      register INTEGER  d
;
#undef SEP
#endif
{    register long     i, dx;
   x_val += d;
   if( (x_val > max_x_val) && x_val ){
     if( max_x_val == -10000) max_x_val = x_val - d;
     
#line 3237 "./tex4ht-c.tex"

i =  (INTEGER) (  (double) (dx = x_val - max_x_val)
            /         (text_on? word_sp : margin_sp)
            +         0.5 );

#line 3252 "./tex4ht-c.tex"

if( i==0 ){
   i =  (INTEGER) (  (double) dx
            /         word_sp
            +         0.5 );
}


if( i<0 ) i=0;
if( i==0 ){ 
#line 3457 "./tex4ht-c.tex"

         long  curr_pos;
         BOOL  done;
         int ch, cr_fnt;
curr_pos = ftell(dvi_file);
done = FALSE;
while( !done ){
   ch = get_char();
   switch( ch ){
     
     
     
#line 3764 "./tex4ht-c.tex"

case 
#line 17460 "./tex4ht-c.tex"

246 
:   (void) get_char();
case 
#line 17457 "./tex4ht-c.tex"

245 
:   (void) get_char();
case 
#line 17454 "./tex4ht-c.tex"

244 
:   (void) get_char();
case 
#line 17451 "./tex4ht-c.tex"

243 
: {
  for( i=0; i<14; i++ ){ ch = get_char(); }
  for( i=ch + get_char(); i>0; i--) (void) get_char();
  break;
}


     case 
#line 17326 "./tex4ht-c.tex"

141 
:
     case 
#line 17329 "./tex4ht-c.tex"

142 
: { break; }
     default: {
        if( (ch < 
#line 17417 "./tex4ht-c.tex"

171  
) || (ch > 
#line 17423 "./tex4ht-c.tex"

234  
)   ){
           done = TRUE;
        } else {
           
#line 3482 "./tex4ht-c.tex"

         double word_sp;
cr_fnt = ch - 
#line 17417 "./tex4ht-c.tex"

171  
;
cr_fnt = search_font_tbl( cr_fnt );
word_sp = design_size_to_pt( font_tbl[cr_fnt].word_sp )
             * (double) font_tbl[cr_fnt].scale;
i =  (INTEGER) (  (double) dx
            /         (text_on? word_sp : margin_sp)
            +         0.5 );

#line 3252 "./tex4ht-c.tex"

if( i==0 ){
   i =  (INTEGER) (  (double) dx
            /         word_sp
            +         0.5 );
}


if( i>0 ){ i =1; }


}  } }  }
(IGNORED) fseek(dvi_file, curr_pos, 
#line 2174 "./tex4ht-c.tex"

0
);

 }
if( i ){ 
#line 2822 "./tex4ht-c.tex"

if( trace_dvi_H && !ch_map_flag ){
   if( *trace_dvi_del_H != '\0' ){
      (IGNORED) fprintf(cur_o_file, "%s%d", trace_dvi_del_H, (int) dx);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_H);
}

 }
if( !ignore_spaces ){
  
#line 12281 "./tex4ht-c.tex"

if( needs_end_accent && t_accent_template ){
   
#line 2609 "./tex4ht-c.tex"

if( span_on && in_span_ch ){
   if( *end_span[0] ){
       in_span_ch = FALSE;
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       (IGNORED) fprintf(cur_o_file, "%s", end_span[0]);
}  }


   (IGNORED) fprintf(cur_o_file, "%s", t_accent_fifth);
   needs_end_accent = FALSE; 
#line 12424 "./tex4ht-c.tex"

needs_accented_sym--;


}


   while( i-- ) { text_on=TRUE;  put_char(' '); }
} else { recover_spaces = (int) i; }
max_x_val = x_val;


   } else    if( d && text_on  && (x_val != max_x_val) ){
      
#line 3128 "./tex4ht-c.tex"

if( !ignore_spaces ){
   i =  (INTEGER) ( (double) (dx = d) / word_sp + 0.5 );
   if( i<0 ) i=0;
   if( !i ) i = dx>99999L;
   if( i ){ put_char(' '); }
}


   }
   return  d;
}


#line 3181 "./tex4ht-c.tex"


static INTEGER move_y
#ifdef ANSI
#define SEP ,
( register INTEGER d
)
#undef SEP
#else
#define SEP ;
( d ) register INTEGER d
;
#undef SEP
#endif
{  y_val += d;
   
#line 2831 "./tex4ht-c.tex"

if( trace_dvi_V && !ch_map_flag ){
   if( *trace_dvi_del_V != '\0' ){
     (IGNORED) fprintf(cur_o_file, "%s%d", trace_dvi_del_V, d);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_V);
}


   return  d;
}


#line 3652 "./tex4ht-c.tex"


static  void rule_x
#ifdef ANSI
#define SEP ,
(
      BOOL  tag
)
#undef SEP
#else
#define SEP ;
( tag )
      BOOL  tag
;
#undef SEP
#endif
{    long i, right, up;
   up = (INTEGER) get_int(4);
   right = (INTEGER) get_int(4);
   if( ch_map_flag ){ 
#line 5246 "./tex4ht-c.tex"

   long int  sv_x_val, sv_y_val, sv_right, sv;
   int  ch;
sv_x_val = x_val;
sv_y_val = y_val;
sv_right = right;
y_val-=up;
if( right < 0 ){ x_val += right;  right = -right; }
if( up < 0 ){ y_val += up;  up = -up; }
ch = ( (right > xresolution) &&  (up > yresolution) ) ?
       
#line 4861 "./tex4ht-c.tex"

3
 :  ( ( right > up )? 
#line 4855 "./tex4ht-c.tex"

1
 : 
#line 4858 "./tex4ht-c.tex"

2
 );
right += x_val;
up    += sv = y_val;
for( ; x_val < right; x_val += xresolution )
  for( y_val = sv ; y_val < up;  y_val += yresolution )
     insert_ch_map((char) ch, FALSE);
x_val = sv_x_val;
y_val = sv_y_val;
if( sv_x_val + sv_right > max_x_val ) max_x_val = sv_x_val + sv_right;
if( 
#line 4994 "./tex4ht-c.tex"

tag
 ) x_val += sv_right;

 }
   else if( pos_dvi ){
      
#line 8351 "./tex4ht-c.tex"

         long int d;
if( (up > 0) && (right > 0) ){
   if(  *pos_line ){
                      double from_x, from_y;
     
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


     from_x = pos_x_A * (x_val - base_pos_x) + pos_x_B;
     from_y = pos_y_C * (y_val - pos_y_E * up - base_pos_y) + pos_y_D;
     switch (rect_pos){
       case 1: {
              (IGNORED) fprintf(cur_o_file, pos_line,
                   from_x, from_y,
                   pos_x_A * right + pos_x_B  + from_x,
                   pos_y_C * up + pos_y_D  + from_y );
               break; }
       case 2: {
              (IGNORED) fprintf(cur_o_file, pos_line,
                   from_x, from_y,
                   pos_x_A * right + pos_x_B + from_x,
                   from_y,
                   pos_y_C * up + pos_y_D  );
               break; }
       default: {
              (IGNORED) fprintf(cur_o_file, pos_line,
                   from_x, from_y,
                   pos_x_A * right,
                   pos_y_C * up);
               }
      }
   }
   if( x_val < min_pos_x )           min_pos_x = x_val;
   if( (d = x_val + right) > max_pos_x ) max_pos_x = d;
   if( (d = y_val - up) < min_pos_y ) min_pos_y = d;
   if( y_val > max_pos_y )           max_pos_y = y_val;
}


      if( tag ) x_val += right;
   } else if( (up>0) && (right>0) ) {
      
#line 3703 "./tex4ht-c.tex"

if( (x_val + right)  &&
        (    ((x_val + right) > max_x_val)
          || ( !text_on && !ignore_chs )
        )
){
   if( (max_x_val == -10000) || ((x_val + right) <= max_x_val) )
   {  max_x_val = x_val;  }
   i =  (INTEGER) (  (double) (x_val + right - max_x_val)
                   /         (text_on? word_sp : margin_sp)
                   +         0.5 );
   
#line 3728 "./tex4ht-c.tex"

if( i==0 ){
   i =  (INTEGER) (  (double) (x_val + right - max_x_val)
                   /         word_sp
                   +         0.5 );
}


   if( i && !text_on )  try_new_line();
   
#line 2844 "./tex4ht-c.tex"

if( trace_dvi_R && !ch_map_flag ){
   if( *trace_dvi_del_R != '\0' ){
      (IGNORED) fprintf(cur_o_file, "%s%d %d",
         trace_dvi_del_R, (int) x_val, (int) y_val);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_R);
}


   while( i-- ) { text_on=TRUE;
      if( rule_ch && !
#line 3739 "./tex4ht-c.tex"

rule_ch_off

 ){ put_char(rule_ch); }
   }
   
#line 2855 "./tex4ht-c.tex"

if( trace_dvi_R && !ch_map_flag ){
   if( *trace_dvi_del_r != '\0' ){
      (IGNORED) fprintf(cur_o_file, "%s%d %d",
         trace_dvi_del_R, (int) right, (int) up);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_r);
}


   max_x_val = x_val + right;
}


      if( tag ) x_val += right;
   } else {
      
#line 3746 "./tex4ht-c.tex"


#line 2844 "./tex4ht-c.tex"

if( trace_dvi_R && !ch_map_flag ){
   if( *trace_dvi_del_R != '\0' ){
      (IGNORED) fprintf(cur_o_file, "%s%d %d",
         trace_dvi_del_R, (int) x_val, (int) y_val);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_R);
}



#line 2855 "./tex4ht-c.tex"

if( trace_dvi_R && !ch_map_flag ){
   if( *trace_dvi_del_r != '\0' ){
      (IGNORED) fprintf(cur_o_file, "%s%d %d",
         trace_dvi_del_R, (int) right, (int) up);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_r);
}




      if( tag ) x_val += right;
}  }


#line 3881 "./tex4ht-c.tex"


static BOOL tex4ht_special
#ifdef ANSI
#define SEP ,
(
    int      *chr SEP 
    long int *special_n

)
#undef SEP
#else
#define SEP ;
( chr, special_n)
    int      *chr SEP 
    long int *special_n

;
#undef SEP
#endif
{  BOOL     tex4ht;
    int      i;
    long unsigned N;
  tex4ht = FALSE;
  
#line 3930 "./tex4ht-c.tex"

*special_n = (long int) (N = get_unt(*chr - 
#line 17441 "./tex4ht-c.tex"

239 
 + 1));
for(i=4; i--; ){
  special_hd[i] = (unsigned char) (N & 0xFF);
  N = N >> 8; }


  if( *special_n > (long int) 4 ){
    for(i=4; i<9; i++)   special_hd[i]=get_char();
    special_hd[9]='\0';
    
#line 3918 "./tex4ht-c.tex"

tex4ht =           (special_hd[4] == 't') || (special_hd[4] == 'T');
tex4ht = tex4ht &&  special_hd[5] == '4';
tex4ht = tex4ht && ((special_hd[6] == 'h') || (special_hd[6] == 'H'));
tex4ht = tex4ht && ((special_hd[7] == 't') || (special_hd[7] == 'T'));
if( tex4ht && trace_special ){
   
#line 17181 "./tex4ht-c.tex"

{                             long  curr_pos;
                              int n, i;
                              U_CHAR ch;

   curr_pos = ftell(dvi_file);
   print_f("\nSPECIAL:  ");  ch = special_hd[8]; i=60;
   for( n=*special_n - 3; n--;){
      if( !i ){ (IGNORED) putc( '\n', cur_o_file );   i=70; }
      else i--;
      (IGNORED) putc(( (ch>31) && (ch<127))? ch : ' ', cur_o_file);
      ch = get_char();
   }
   (IGNORED) putc( '\n', cur_o_file );
   (IGNORED) fseek(dvi_file, curr_pos, 
#line 2174 "./tex4ht-c.tex"

0
);
}


}


    *chr = special_hd[8];
    tex4ht = tex4ht && ( (*chr == '=') || (*chr == '<') ||
        (*chr == '>') || (*chr == '*') || (*chr == '@') ||
        (*chr == ':') || (*chr == '"') || (*chr == '~') ||
        (*chr == ';') || (*chr == '.') || (*chr == '^') ||
        (*chr == '|') || (*chr == '+') || (*chr == '!') );
    *special_n -= 5;  }
  else{ special_hd[4]='\0'; }
  return  tex4ht;
}


#line 4966 "./tex4ht-c.tex"

static void init_ch_map(MYVOID)
{   int i;
  for( i=0; i<HEIGHT; i++ ){
    ch_map[i].max = 0;  ch_map[i].chars = 0;  ch_map[i].line = NULL;  }
  max_map_line = -1;
  min_map_line = HEIGHT;
}


#line 5003 "./tex4ht-c.tex"


static void insert_ch_map
#ifdef ANSI
#define SEP ,
(
       U_CHAR ch SEP 
       BOOL 
#line 4994 "./tex4ht-c.tex"

tag


)
#undef SEP
#else
#define SEP ;
( ch, 
#line 4994 "./tex4ht-c.tex"

tag
 )
       U_CHAR ch SEP 
       BOOL 
#line 4994 "./tex4ht-c.tex"

tag


;
#undef SEP
#endif
{     int row, col;
   
#line 5045 "./tex4ht-c.tex"

{          double x;
   row = (int) ( (y_val>0? y_val : 0.0) / (double) yresolution + 0.5);
   if( row >= HEIGHT ){
     if( ok_map ){ warn_i_int_2( 34, row, ch); ok_map = FALSE; }
     return; }
   x = (x_val>0? x_val : 0.0 ) / (double) xresolution + 0.75;
   col = (int) x;
   if( (ch > ' ') && (ch != '-') && (ch != '|') ){
      if( row == prevrow ){
         if( (col == prevcol + 1) && (x > prev_x + 0.5) )
            insert_ch_map(' ', TRUE);
         else if( (col > prevcol + 1) && (x < prev_x+0.2)
                                      && ( ch != '&' ))
            col = prevcol + 1;
      }else  prevrow = -1;
      prev_x = x
             + (
#line 9821 "./tex4ht-c.tex"

design_size_to_pt( *(font_tbl[cur_fnt].wtbl
                 +  (int) (
#line 9828 "./tex4ht-c.tex"

*(font_tbl[cur_fnt].char_wi +  (int)
   ( (design_ch? design_ch : ch) - font_tbl[cur_fnt].char_f)% 256)

)) )
   * (double) font_tbl[cur_fnt].scale

)
             / (double) xresolution;
      prevcol = col;
   }else  prevrow = -1;
   prevrow = row;
}


   if(ch != 10){
      if( (ch_map[row].max > MAX_MAP_LINE) || (col > MAX_MAP_LINE) ){
        if( ok_map ){ warn_i_int_2( 25, MAX_MAP_LINE, ch);
                      ok_map = FALSE; }
      }else{  
         if( row < min_map_line ) min_map_line = row;
         if( row > max_map_line ) max_map_line = row;
         if( ch_map[row].max ){ 
#line 5122 "./tex4ht-c.tex"

    int   n;
    char* p;
if( ch_map[row].chars > col ){
      
#line 5173 "./tex4ht-c.tex"

if( 
#line 4994 "./tex4ht-c.tex"

tag
 ){
   if(   *(ch_map[row].line + ch_map[row].max - 1)
      ||  (ch_map[row].chars - col == 1)  ){ 
#line 5218 "./tex4ht-c.tex"

ch_map[row].max += 5;
ch_map[row].line = (char *)
   r_alloc((void *) ch_map[row].line,
           (size_t) ch_map[row].max + 1 );
for( n = 0; n<5; n++ )
   *(ch_map[row].line + ch_map[row].max - n) = 0;
ch_map[row].chars += 5;

 }
   col = (ch_map[row].chars--) - col;
   p = ch_map[row].line + ch_map[row].max;
   while( col ){                     unsigned char temp_ch;
     if( ((unsigned char) (*p)) < 
#line 4864 "./tex4ht-c.tex"

4
 ) col--;
       temp_ch = *(--p);  *(p+1) = temp_ch;  }
} else {
   col = ch_map[row].chars - col;
   p = ch_map[row].line + ch_map[row].max;
   while( col ){
     if( ((unsigned char) (*p)) < 
#line 4864 "./tex4ht-c.tex"

4
 ) col--;  p--;  }
}
*(++p) = ch;

 }
else{ 
#line 5136 "./tex4ht-c.tex"

n = (col - ch_map[row].chars + 8) / 5 * 5;
ch_map[row].chars += n - 
#line 4994 "./tex4ht-c.tex"

tag
;
ch_map[row].max += n;
ch_map[row].line = (char *)
    r_alloc((void *) ch_map[row].line,
            (size_t) ch_map[row].max + 1);
while( n-- )  *(ch_map[row].line + ch_map[row].max - n) = 0;
*(ch_map[row].line + ch_map[row].max
         - (ch_map[row].chars - col) + !
#line 4994 "./tex4ht-c.tex"

tag
 ) = ch;

 }

 }
         else { 
#line 5091 "./tex4ht-c.tex"

   int   n;
   char* p;
ch_map[row].chars = (n = (col + 2 + 5) / 5 * 5) - 
#line 4994 "./tex4ht-c.tex"

tag
;
ch_map[row].max =  n - 1;
ch_map[row].line = p = m_alloc(char, n);
while( n-- ){ *(p++) = 0; }
*(ch_map[row].line + col) = ch;

 }
}  }   }


#line 5297 "./tex4ht-c.tex"

static void dump_ch_map(MYVOID)
{     int   n, i, min, k, extra_sp;
      U_CHAR  *p;
      
  
#line 5327 "./tex4ht-c.tex"

{    int   max;
   min = 100; max = 0;
   for( i=min_map_line; i<=max_map_line; i++ ){
     p = ch_map[i].line;
     n = ch_map[i].max;  if( max < n )  max = n;
     k = 0;  while( n-- ){ if(*(p++)) break;  k++; }
     if( ch_map[i].max && (k < min) ) min = k;        }
   if( (max < 78) && !nomargin ) min = 0;
}


  for( i=min_map_line; i<=max_map_line; i++ ){
    if( ( n = ch_map[i].max) > 0 ){
      p = ch_map[i].line;  k = min;    extra_sp = 0;
      
#line 5340 "./tex4ht-c.tex"

{     U_CHAR  *s;
   s = p + n;
   while( n && !(*s) && !(*(s-1)) ){  n--; s--; }
   if( n && !(*s) && (((unsigned char) *(s-1)) < 
#line 4864 "./tex4ht-c.tex"

4
) ) n--;
}


      while( 1 + n-- ){
        if( --k < 0 ){
          if( extra_sp && (((unsigned char) *p)     < 
#line 4864 "./tex4ht-c.tex"

4
)
                       && (((unsigned char) *(p+1)) < 
#line 4864 "./tex4ht-c.tex"

4
) )
          {  extra_sp--;
          } else { switch( *p ){ 
#line 5348 "./tex4ht-c.tex"

           case 0: { put_char(' '); break; }
    case 
#line 4855 "./tex4ht-c.tex"

1
: { put_char('-'); break; }
    case 
#line 4858 "./tex4ht-c.tex"

2
: { put_char('|'); break; }
case 
#line 4861 "./tex4ht-c.tex"

3
: { put_char('#'); break; }
         case ' ': { extra_sp++; }
          default: { 
#line 5357 "./tex4ht-c.tex"

    BOOL    tag;
    INTEGER count;
tag = TRUE;   count = 0;
do{   if( *p == '<' )       tag   = FALSE;
      else  if( *p == '>' ) tag   = TRUE;
            else           count += tag;
      put_char( *p ); n--;
}while( ((unsigned char) *(++p)) >= 
#line 4864 "./tex4ht-c.tex"

4
 );
if( !count ){ n++; p--; }

  break; }

 } }
        }
        p++;
      }
      free((void *)  ch_map[i].line );
    }
    if( i<max_map_line )  put_char('\n');
  }
  nomargin = FALSE;
}


#line 5787 "./tex4ht-c.tex"


static void  set_loc
#ifdef ANSI
#define SEP ,
(
          int op SEP 
   long   int d

)
#undef SEP
#else
#define SEP ;
( op, d )
          int op SEP 
   long   int d

;
#undef SEP
#endif
{
   idv_char( op + 3 );  int_to_dvi( d, 4 ); file_n += 5;
}


#line 6006 "./tex4ht-c.tex"


static void idv_char
#ifdef ANSI
#define SEP ,
(         int  n
)
#undef SEP
#else
#define SEP ;
( n )         int  n
;
#undef SEP
#endif
{ (IGNORED) putc( n, idv_file ); }


#line 6019 "./tex4ht-c.tex"


static void cond_idv_char
#ifdef ANSI
#define SEP ,
(         int  n
)
#undef SEP
#else
#define SEP ;
( n )         int  n
;
#undef SEP
#endif
{
   if( dvi_flag ){  (IGNORED) putc( n, idv_file );  file_n++;  }
}


#line 6034 "./tex4ht-c.tex"

static void idv_copy(  MYVOID )
{  idv_char( get_char() );  file_n++;  }


#line 6049 "./tex4ht-c.tex"


static void cond_idv_int
#ifdef ANSI
#define SEP ,
(  long   int val SEP 
                             int n
)
#undef SEP
#else
#define SEP ;
( val, n )  long   int val SEP 
                             int n
;
#undef SEP
#endif
{
  if( dvi_flag ){  int_to_dvi((long int)  val, n );   file_n += n;  }
}


#line 6063 "./tex4ht-c.tex"


static void  int_to_dvi
#ifdef ANSI
#define SEP ,
(  long int val SEP 
                                int n

)
#undef SEP
#else
#define SEP ;
( val, n )  long int val SEP 
                                int n

;
#undef SEP
#endif
{         unsigned U_CHAR ch2, ch3, ch4;
  ch4 = (unsigned char) (val & 0xFF);   val = val >> 8;
  ch3 = (unsigned char) (val & 0xFF);   val = val >> 8;
  ch2 = (unsigned char) (val & 0xFF);   val = val >> 8;
  switch( n ){
    case 4: idv_char( (int) val );
    case 3: idv_char( ch2 );
    case 2: idv_char( ch3 );
    case 1: idv_char( ch4 ); }

}


#line 6085 "./tex4ht-c.tex"


static void cond_string
#ifdef ANSI
#define SEP ,
(           int  ch SEP  int n

)
#undef SEP
#else
#define SEP ;
(ch, n)           int  ch SEP  int n

;
#undef SEP
#endif
{  cond_idv_char( ch );
   while( n-- )  cond_idv_char( get_char() );
}


#line 6115 "./tex4ht-c.tex"


static INTEGER advance_idv_page
#ifdef ANSI
#define SEP ,
(
                                            INTEGER    bop_addr SEP 
                                            char*  cur_font

)
#undef SEP
#else
#define SEP ;
( bop_addr, cur_font )
                                            INTEGER    bop_addr SEP 
                                            char*  cur_font

;
#undef SEP
#endif
{                                           int    i;
   if( page_n++ ){
     idv_char(
#line 17329 "./tex4ht-c.tex"

142 
);  file_n++;
     idv_char(
#line 17323 "./tex4ht-c.tex"

140 
);      file_n++;
     idv_char( 
#line 17320 "./tex4ht-c.tex"

139 
 );
     idv_int( page_n );  for( i=36; i--; ) idv_char( 0);
     idv_int( bop_addr );  bop_addr = file_n;  file_n += 45;
     idv_char(
#line 17326 "./tex4ht-c.tex"

141 
);  file_n++;
     for( i=1; i<=cur_font[0]; i++ ){
        idv_char( cur_font[i] );   file_n++;
   } }
   
#line 6147 "./tex4ht-c.tex"

store_mv(
#line 17356 "./tex4ht-c.tex"

151 
, dx_1);
store_mv(
#line 17371 "./tex4ht-c.tex"

156 
, dx_2);
store_mv(
#line 17398 "./tex4ht-c.tex"

165 
, dy_1);
store_mv(
#line 17413 "./tex4ht-c.tex"

170 
, dy_2);


   return  bop_addr;
}


#line 6172 "./tex4ht-c.tex"


static void store_mv
#ifdef ANSI
#define SEP ,
(  int op SEP   INTEGER d

)
#undef SEP
#else
#define SEP ;
(op, d)  int op SEP   INTEGER d

;
#undef SEP
#endif
{
 if( dvi_flag ){
   cond_idv_char(op);  idv_int( (INTEGER) -d);
   cond_idv_char(op);  idv_int( (INTEGER)  d);  file_n += 8; }
}


#line 7241 "./tex4ht-c.tex"

static void push_stack(MYVOID)
{
   stack[stack_n].x_val = x_val;
   stack[stack_n].dx_1  = dx_1;
   stack[stack_n].dx_2  = dx_2;
   stack[stack_n].y_val = y_val;
   stack[stack_n].dy_1  = dy_1;
   stack[stack_n].dy_2  = dy_2;
   
#line 7259 "./tex4ht-c.tex"

stack[stack_n+1].sv_no_left_del = stack[stack_n+1].no_left_del;
stack[stack_n+1].no_left_del = stack[stack_n].no_left_del;


   stack_n++;
if( stack_n > 
#line 6684 "./tex4ht-c.tex"

((int) stack_len + 2)

 ){
  warn_i(40);
}
   
#line 12475 "./tex4ht-c.tex"

stack[stack_n].accented  = FALSE;


}


#line 7288 "./tex4ht-c.tex"

static void pop_stack(MYVOID)
{
   
#line 12468 "./tex4ht-c.tex"

if( stack[stack_n].accented ){
   
#line 12419 "./tex4ht-c.tex"

needs_accented_sym--;


   stack[stack_n].accented=FALSE;
}


   
#line 7264 "./tex4ht-c.tex"

stack[stack_n].no_left_del = stack[stack_n].sv_no_left_del;


   --stack_n;
   x_val = stack[stack_n].x_val;
   dx_1  = stack[stack_n].dx_1;
   dx_2  = stack[stack_n].dx_2;
   y_val = stack[stack_n].y_val;
   dy_1  = stack[stack_n].dy_1;
   dy_2  = stack[stack_n].dy_2;
}


#line 7389 "./tex4ht-c.tex"


static  struct del_stack_entry* push_del
#ifdef ANSI
#define SEP ,
(     U_CHAR ch SEP 
                                                 int cr_fnt
)
#undef SEP
#else
#define SEP ;
(ch, cr_fnt)     U_CHAR ch SEP 
                                                 int cr_fnt
;
#undef SEP
#endif
{                    struct del_stack_entry *p;
    p = m_alloc(struct del_stack_entry,1);
    p->next = del_stack;
    p->ch = ch;
    p->fnt = cr_fnt;
    p->id = ch_id;
    return p;
}


#line 7410 "./tex4ht-c.tex"


static  struct del_stack_entry* pop_del
#ifdef ANSI
#define SEP ,
(
                                                  U_CHAR ch SEP 
                                                  int  id_hide SEP 
                                                  int cr_fnt
)
#undef SEP
#else
#define SEP ;
(ch, id_hide, cr_fnt)
                                                  U_CHAR ch SEP 
                                                  int  id_hide SEP 
                                                  int cr_fnt
;
#undef SEP
#endif
{
   if( del_stack != (struct del_stack_entry*) 0 ){
      if( (cr_fnt ==  del_stack->fnt) &&
          (  *(font_tbl[cr_fnt].math + (ch - font_tbl[cr_fnt].char_f))
             == del_stack->ch) ){
                                             struct del_stack_entry  * p;
         if( !id_hide && !id_latex ){  sv_id = del_stack->id; }
         del_stack = (p = del_stack)->next;  free((void *)  p );
   }  }
   return del_stack;
}


#line 7623 "./tex4ht-c.tex"


static struct send_back_entry *  rev_list
#ifdef ANSI
#define SEP ,
(
                             struct send_back_entry *back_group
)
#undef SEP
#else
#define SEP ;
(back_group)
                             struct send_back_entry *back_group
;
#undef SEP
#endif
{                           struct send_back_entry *p, *q, *t;
    if( back_group->id == -1 ){ return back_group; }
    p = back_group;    q = p->next;
    while( p->id != -1 ){
       t = q->next;  q->next = p;  p = q;  q = t;
    }
    back_group->next = p;
    return  p->next;
}


#line 7677 "./tex4ht-c.tex"


static struct send_back_entry *  back_insert
#ifdef ANSI
#define SEP ,
(
                             struct send_back_entry *back SEP 
                             int    id
)
#undef SEP
#else
#define SEP ;
(back, id)
                             struct send_back_entry *back SEP 
                             int    id
;
#undef SEP
#endif
{
  while( back->id == id ){
                         struct send_back_entry *p;
    print_f( back->send );
    back = (p = back)->next;
    free((void *)  p->send );
    free((void *)  p );
  }
  return back;
}


#line 8550 "./tex4ht-c.tex"


static double pos_dbl
#ifdef ANSI
#define SEP ,
(       long  int *  special_n
)
#undef SEP
#else
#define SEP ;
( special_n )       long  int *  special_n
;
#undef SEP
#endif
{
                U_CHAR ch;
                double v;
                int d;
  v = 0.0; d = 10;
  while(  --(*special_n) > 0 ){
    ch = get_char();
    if( ('0' <= ch ) && (ch <= '9' ) ){
       v += (double) (ch -'0') / d;  d *= 10; }
    else break;
  }
  return v;
}


#line 9270 "./tex4ht-c.tex"


static void doGlyphArray
#ifdef ANSI
#define SEP ,
(
     BOOL yLocs
)
#undef SEP
#else
#define SEP ;
(yLocs)
     BOOL yLocs
;
#undef SEP
#endif
{         int i, glyphCount;
   (void) get_unt(4);
   glyphCount = (INTEGER) get_unt(2);
   for( i = 0; i < glyphCount; ++i ){
     (void) get_int(4);
     if( yLocs ){  (void) get_int(4);   }
   }
   for (i = 0; i < glyphCount; ++i){
     (void) get_unt(2);
   }
}


#line 9318 "./tex4ht-c.tex"


static int search_font_tbl
#ifdef ANSI
#define SEP ,
(
        int cur_fnt
)
#undef SEP
#else
#define SEP ;
( cur_fnt )
        int cur_fnt
;
#undef SEP
#endif
{      int i;
   for( i=0; i<font_tbl_size; i++){
     if( font_tbl[i].num == cur_fnt ){ return i; }
   }
   err_i_int( 6,cur_fnt );
   return 0;
}


#line 10419 "./tex4ht-c.tex"


static int get_html_ch
#ifdef ANSI
#define SEP ,
(   FILE*  file

)
#undef SEP
#else
#define SEP ;
( file )   FILE*  file

;
#undef SEP
#endif
{                        int ch;
  if( (ch = (int) getc(file)) == EOF ) {
     dump_htf( file );
     err_i_str(20, new_font_name);
  }
  return ch;
}


#line 10440 "./tex4ht-c.tex"


static FILE* f_open
#ifdef ANSI
#define SEP ,
(
                          const char*  name  SEP 
                          const char*  flags
)
#undef SEP
#else
#define SEP ;
( name, flags )
                          const char*  name  SEP 
                          const char*  flags
;
#undef SEP
#endif
{                        FILE* file;
  if( (file = fopen(name,flags) ) != NULL ) {
     (IGNORED)  printf("(%s)\n",name);
  }
  return file;
}


#line 10464 "./tex4ht-c.tex"


static void dump_htf
#ifdef ANSI
#define SEP ,
(   FILE*  file

)
#undef SEP
#else
#define SEP ;
( file )   FILE*  file

;
#undef SEP
#endif
{       int i, j, ch, chr=0;
  (IGNORED) fseek(file, 0L, 0);
  i=-1; j=0;
  while( (ch = getc(file)) != EOF  ){
    if( !j ){  chr = ch; }
    j += ch==chr;
    (IGNORED)  putc(ch,stderr);
    if( ch == '\n' ){
       if( (i>-1 ) && (j<4) && (dump_htf_files<2) ){
          (IGNORED) printf("missing delimiter \n");
       }
       (IGNORED) fprintf(stderr,"%d:  ",++i);
       j=0;
} } }


#line 10516 "./tex4ht-c.tex"

static void dump_env( MYVOID )
{
                             int ch;
  if( !dumped_env ){
    dumped_env = TRUE;
    (IGNORED) fseek(dot_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
    (IGNORED) fprintf(stderr,
     "\n----------------------------------------------------\n");
    (IGNORED) fprintf(stderr, "environment file\n");
    (IGNORED) fprintf(stderr,
     "----------------------------------------------------\n");
    while( (ch = getc(dot_file)) != EOF  ){
      (IGNORED)  putc(ch,stderr);
    }
    (IGNORED) fprintf(stderr,
     "----------------------------------------------------\n");
}  }


#line 10549 "./tex4ht-c.tex"


static void htf_to_lg
#ifdef ANSI
#define SEP ,
(

                      struct html_font_rec*   html_font SEP 
                      char* new_font_name SEP 
                      int     fonts_n SEP 
                      FILE* file
)
#undef SEP
#else
#define SEP ;
(html_font,  new_font_name, fonts_n, file)

                      struct html_font_rec*   html_font SEP 
                      char* new_font_name SEP 
                      int     fonts_n SEP 
                      FILE* file
;
#undef SEP
#endif
{
                                              int ch, i;
  for( i = 0  ; i<fonts_n; ){
     if( eq_str(html_font[i].name, new_font_name) ){  break; }
     i++;
  }
  if( i == fonts_n ){
                                              BOOL content;
                                              int prev_ch;
     prev_ch = '\n';  content = FALSE;
     while( (ch = (int) getc(file)) != EOF ) {
       if( (ch != '\n') || (prev_ch != '\n') ){
         (void)  putc( ch, log_file );
         prev_ch = ch;  content = TRUE;
     } }
     if( content && (prev_ch != '\n') ){
        (void)  putc( '\n', log_file );
     }
} }


#line 10599 "./tex4ht-c.tex"


static  INTEGER get_html_file_id
#ifdef ANSI
#define SEP ,
(
                            FILE* file SEP 
                            int first SEP  int last SEP  int n

)
#undef SEP
#else
#define SEP ;
(file, first, last, n)
                            FILE* file SEP 
                            int first SEP  int last SEP  int n

;
#undef SEP
#endif
{                          int ch, i, bound, cnt;
                            INTEGER  diff;
                            char* name;
   name = new_font_name;
   while( *name == (ch = get_html_ch(file)) )  name++;
   
#line 10116 "./tex4ht-c.tex"

if( ( name == new_font_name ) && (n == 19) && (ch=='.') ){
   return HTF_ALIAS;
}


   if( (*name != '\0') || (ch != ' ') )  warn_i_str(n, name);
   bound = first;  diff = 0;
   for( cnt = 0; cnt < 2; cnt++ ){
      while( ch == ' ' )  ch = get_html_ch(file);
      i = 0;
      while( (ch >= '0') && (ch <= '9') ){
         i = i * 10 + ch - '0';  ch = get_html_ch(file);  }
      if( i != bound ){
         
#line 16921 "./tex4ht-c.tex"

(IGNORED) fprintf(stderr,"--- warning --- ");
(IGNORED) fprintf(stderr, warn_err_mssg[22]

, new_font_name, i, bound); show_err_context();
         diff = diff * 1000 + i - bound;    }
      bound = last;  }
   while( ch != '\n' ) ch = get_html_ch(file);
   return diff;
}


#line 10733 "./tex4ht-c.tex"


static void notify_class_info
#ifdef ANSI
#define SEP ,
(   int gif_flag
)
#undef SEP
#else
#define SEP ;
( gif_flag )   int gif_flag
;
#undef SEP
#endif
{                        U_CHAR str[256], *p;
   str[0] = '\0';
   p = gif_open[gif_flag];
   if( p )
     if( *p ) (IGNORED) strct(str,p);
   p = gif_alt[gif_flag];
   if( p )
     if( *p ) (IGNORED) strct(str,p);
   p = gif_class[gif_flag];
   if( p )
     if( *p ) (IGNORED) strct(str,p);
   p = gif_size[gif_flag];
   if( p )
     if( *p ) (IGNORED) strct(str,p);
   p = gif_mag[gif_flag];
   if( p )
     if( *p ) (IGNORED) strct(str,p);
   p = gif_ord[gif_flag];
   if( p )
     if( *p ) (IGNORED) strct(str,p);
   p = gif_end[gif_flag];
   if( p )
     if( *p ) (IGNORED) strct(str,p);
   p = str;
   while( *p ){
     if( *p == '\n' ) *p = ' ';
     p++;
   }
   (IGNORED) fprintf(log_file, class_fmt,
      gif_flag,
      gif_id[gif_flag]? gif_id[gif_flag] : "",
      str);
}


#line 10957 "./tex4ht-c.tex"


static void script
#ifdef ANSI
#define SEP ,
(
      U_CHAR *templt SEP 
      U_CHAR *job SEP 
      int  page SEP 
      U_CHAR *font

)
#undef SEP
#else
#define SEP ;
(templt, job, page, font)
      U_CHAR *templt SEP 
      U_CHAR *job SEP 
      int  page SEP 
      U_CHAR *font

;
#undef SEP
#endif
{    U_CHAR *ch, *p;
      U_CHAR fmt[256];
   job[ (int) strlen((char *) job) - 1 ] ='\0';
   p = ch = templt;
   while( TRUE ){
     if( *ch == '%' ){
       
#line 10991 "./tex4ht-c.tex"

*ch = '\0';  (IGNORED) fprintf(log_file, "%s", p);
*(ch++) = '%';


       
#line 11002 "./tex4ht-c.tex"

p=fmt;  *(p++) = '%';
if( *ch == '\0' ){ job[ (int) strlen((char *) job) ] ='.';  return; }
while( *ch != '%' ){  *(p++) = *(ch ++);  }
*(p+1) = '\0';


       switch( *(++ch) ){
          case '1':{ *p = 's';
                     (IGNORED) fprintf(log_file, fmt, job);  break;}
          case '2':{ *p = 'd';
                     (IGNORED) fprintf(log_file, fmt, page); break;}
          case '3':{ *p = 's';
                     (IGNORED) fprintf(log_file, fmt, font); break;}
          case '%':{ *p = 'c';
                     (IGNORED) fprintf(log_file, fmt, '%');  break;}
           default:{ job[ (int) strlen((char *) job) ] ='.';         return;}
       }
       p = ++ch;
     }else ch++;
} }


#line 11185 "./tex4ht-c.tex"



static void  dos_gif_file
#ifdef ANSI
#define SEP ,
(
        U_CHAR *str SEP 
        int  mag SEP 
        int  design_ch

)
#undef SEP
#else
#define SEP ;
(str, mag, design_ch)
        U_CHAR *str SEP 
        int  mag SEP 
        int  design_ch

;
#undef SEP
#endif
{      int  n, m, i;
        struct gif_file_rec * p, * q;
        U_CHAR *ch;
  m = n = (int) strlen((char *) str);
  if( n > 4 ){
    
#line 11245 "./tex4ht-c.tex"

if( (p = gif_file) != NULL ){
  while( TRUE ){
    if( eq_str( str, p->name ) ) break;
    if( (p = p->next) == gif_file ){ p = NULL;  break; }
} }


    if( p == NULL ){ 
#line 11258 "./tex4ht-c.tex"

p =  m_alloc(struct gif_file_rec, 1);

#line 11278 "./tex4ht-c.tex"

for(i=str[n]; n; ){ i+=str[--n];  if( i > (INT_MAX / 8) ) i/=2; }
if( (n=i % BASE) <10 ) n+= 10 + i%(BASE-20);
*(ch = p->code)= n; n += i;
ch[1]          = n%BASE; n += i;
ch[2]          = n%BASE; n += i;
ch[3]          = n%BASE;



#line 11292 "./tex4ht-c.tex"

if( gif_file ){
   q = gif_file->next;
   while( TRUE ){
     if( (*(q->code) == *ch)   && (*(q->code+1) == ch[1]) &&
         (*(q->code) == ch[2]) && (*(q->code+2) == ch[3])  ){
        ch[3] ++;  ch[2] += ch[3]/ BASE;  ch[3] = ch[3] % BASE;
                   ch[1] += ch[2]/ BASE;  ch[2] = ch[2] % BASE;
                                          ch[1] = ch[1] % BASE;
        q = gif_file;
     } else if( q == gif_file ) break;
     q = q->next;
}  }


(IGNORED) printf("\nRenaming `%s____%s' to `%c%c%c%c____%s'\n",
            str, gif,
                 xeh[(int)(ch[0])], xeh[(int)(ch[1])],
                 xeh[(int)(ch[2])], xeh[(int)(ch[3])]
               , gif);
p->name = m_alloc(char,m+1);
(IGNORED) strcpy((char *)  p->name, (char *) str );
if( gif_file ){  p->next = gif_file->next;   gif_file->next = p;  }
          else   p->next = p;

 }
    gif_file = p;
    for( n=0; n<4; n++ )   str[n] = xeh[(int)(*(p->code + n))];
  }
  str[n++] = xeh[mag<16? 0: mag / 16];
  str[n++] = xeh[mag % 16];
  str[n++] = xeh[design_ch<16? 0: design_ch / 16];
  str[n++] = xeh[design_ch % 16];
  str[n] = '\0';
}



#line 11348 "./tex4ht-c.tex"


static void  put_alt_ch
#ifdef ANSI
#define SEP ,
(
      int  chr SEP 
      BOOL ch_str_flag
)
#undef SEP
#else
#define SEP ;
(chr,ch_str_flag)
      int  chr SEP 
      BOOL ch_str_flag
;
#undef SEP
#endif
{
   if( !ch_str_flag ) put_char( chr );
   else if( chr > 0 ){ 
#line 11365 "./tex4ht-c.tex"

    unsigned U_CHAR * p;
p = font_tbl[cur_fnt].str[chr-1];
if( gif_ch )  print_f( (char *) p );  
else while( *p ){
  switch( *p ){
    case '<':  { while( *p && (*p != '>') )  p++;  break; }
    case '>':
    case '"':  { p++;  break; }
   case '\'':  { p++;  break; }
    default:   { put_char( (int) *p  ) ; p++; }
} }

 }
}


#line 12352 "./tex4ht-c.tex"


static void get_open_accent
#ifdef ANSI
#define SEP ,
(
      char   **all SEP 
      char   **first SEP 
      char   **second SEP 
      char   **third SEP 
      char   **fourth SEP 
      char   **fifth SEP 
      long int    *n
)
#undef SEP
#else
#define SEP ;
(all,first,second,third,fourth,fifth,n)
      char   **all SEP 
      char   **first SEP 
      char   **second SEP 
      char   **third SEP 
      char   **fourth SEP 
      char   **fifth SEP 
      long int    *n
;
#undef SEP
#endif
{                              char *p, *q;
                                int i;
   if( *all  ){ free((void *) *all);  }
   *all = p = get_str( (int) *n );  *n=0;
   i = 2;
   *first = q = p + 1;
   while ( TRUE ){
     if( *q == *p ){
       *q = '\0';
       switch( i ){
         case 2:{  *second = q+1; break; }
         case 3:{  *third  = q+1; break; }
         case 4:{  *fourth = q+1; break; }
         case 5:{  *fifth = q+1; break; }
       }
       if( i++ == 5 ){ break; }
     } else if( !*q ){
       free((void *) *all);  *all = (char *) 0;    break;
     }
     q++;
} }


#line 12593 "./tex4ht-c.tex"


static int scan_class
#ifdef ANSI
#define SEP ,
(
        int flag
)
#undef SEP
#else
#define SEP ;
( flag )
        int flag
;
#undef SEP
#endif
{                                    int math_class;
   math_class = get_char();
   if(   (math_class >= '0' )
      && (math_class < '0' + 
#line 12719 "./tex4ht-c.tex"

79

) ){
      { math_class -= '0';    }
   } else {
        if( flag== 1 ) {
           switch( math_class ){
             case '-': { math_class = 
#line 12719 "./tex4ht-c.tex"

79

;
                         pause_class++;  break; }
             case '+': { math_class = 
#line 12719 "./tex4ht-c.tex"

79

;
                         pause_class--;  break; }
              default: { math_class = 0; }
        } } else if( flag== 2 ) {
           switch( math_class ){
             case 
#line 12726 "./tex4ht-c.tex"

')'

: {
                math_class = 
#line 12734 "./tex4ht-c.tex"

(
#line 12719 "./tex4ht-c.tex"

79

 + 1)

;  break; }
             case 
#line 12730 "./tex4ht-c.tex"

'('

:
                { math_class = 
#line 12738 "./tex4ht-c.tex"

(
#line 12719 "./tex4ht-c.tex"

79

 + 2)

;   break; }
              default: { math_class = 0; }
           }
        } else { math_class = 0; }
   }
   return math_class;
}


#line 12633 "./tex4ht-c.tex"


static  INTEGER set_ch_class
#ifdef ANSI
#define SEP ,
(    int ch
)
#undef SEP
#else
#define SEP ;
(ch)    int ch
;
#undef SEP
#endif
{                              int r_ch;
   r_ch = ch - font_tbl[cur_fnt].char_f;
   if( math_class == 
#line 7369 "./tex4ht-c.tex"

5

 ){
      store_bit_I( font_tbl[cur_fnt].math_closing, r_ch );
      *(font_tbl[cur_fnt].math + r_ch) =
              (char) ((open_del == 256)?  ch : open_del);
   } else {
      store_bit_Z( font_tbl[cur_fnt].math_closing, r_ch );
      *(font_tbl[cur_fnt].math + r_ch) = math_class;
   }
   open_del = ( math_class == 
#line 7373 "./tex4ht-c.tex"

4

 )? ch : 256;
   
#line 9800 "./tex4ht-c.tex"

return (INTEGER)(
    
#line 9837 "./tex4ht-c.tex"

design_size_to_pt( *(font_tbl[cur_fnt].wtbl
                     +  (int) (
#line 9845 "./tex4ht-c.tex"

*(font_tbl[cur_fnt].char_wi +  (int)
   ( ch - font_tbl[cur_fnt].char_f)% 256)

) )
                 )
* (double) font_tbl[cur_fnt].scale


  );


}


#line 12688 "./tex4ht-c.tex"


static  int math_class_of
#ifdef ANSI
#define SEP ,
(    int ch SEP  int cur_fnt
)
#undef SEP
#else
#define SEP ;
(ch,cur_fnt)    int ch SEP  int cur_fnt
;
#undef SEP
#endif
{                           int math_class;
   math_class = ch - font_tbl[cur_fnt].char_f;
   return ((get_bit( font_tbl[cur_fnt].math_closing, math_class))?
                
#line 7369 "./tex4ht-c.tex"

5

 : *( math_class + font_tbl[cur_fnt].math));

}


#line 13225 "./tex4ht-c.tex"

#if defined(__MSDOS__)

#line 13234 "./tex4ht-c.tex"


static char *get_env_dir
#ifdef ANSI
#define SEP ,
(
      U_CHAR *progname

)
#undef SEP
#else
#define SEP ;
(progname)
      U_CHAR *progname

;
#undef SEP
#endif
{    int  i;
      U_CHAR *p;
  if(! progname || ! *progname)  return NULL;               
  i = (int) strlen((char *) progname);
  while( (progname[--i] != (int) dir_path_slash(progname) )
          && (i > 0) ) ;                              
  if(i == 0)  return NULL;                        
  p = (char *) malloc(i+12);
  if(p == NULL)  return NULL;      
  strncpy(p, progname, i+1);                        
  (IGNORED) strcpy((char *) &p[i+1], "tex4ht.env"); 
  return p;
}


#endif


#line 13294 "./tex4ht-c.tex"


static char* get_script
#ifdef ANSI
#define SEP ,
(
     char * name SEP 
     const U_CHAR * inln SEP 
     int x

)
#undef SEP
#else
#define SEP ;
(name, inln,x)
     char * name SEP 
     const U_CHAR * inln SEP 
     int x

;
#undef SEP
#endif
{
   if( !name )
   {                U_CHAR  str[256], *ch;
      (IGNORED) fseek(dot_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
      if( search_dot_file( x ) ){
           
#line 13318 "./tex4ht-c.tex"

ch = str;  str[254] = '\0';
do{                                       int int_ch;
  while((*(ch++) = (char)
                        (int_ch = (int) getc(dot_file))
        )       != '\n'){
    if( int_ch == EOF ){ *(ch-1)='\n';  break; }
    if( str[254] ){ warn_i_int(33, x);  break; }
  }
}while( (int) getc(dot_file) == x );
*ch = '\0';


      } else  {(IGNORED)  strcpy((char *) str, inln); }
      ch = m_alloc(char, (int) strlen((char *) str)+2);
      (IGNORED) strcpy((char *) ch, (char *) str);
      return ch;
   }else return name;
}


#line 13344 "./tex4ht-c.tex"


static BOOL search_dot_file
#ifdef ANSI
#define SEP ,
(
    int ch
)
#undef SEP
#else
#define SEP ;
( ch )
    int ch
;
#undef SEP
#endif
{  int chr;

  while( TRUE ){
    chr = getc(dot_file);
    if( chr == ch ){ return TRUE; }
    if( chr == '<' ) {
      
#line 13367 "./tex4ht-c.tex"

                         U_CHAR match[256];
                         int i;
for( i = 0; (chr != '\n') && (chr != EOF ) ; i++){
  chr = (int) getc(dot_file);
  match[i] = (U_CHAR) chr;
}
match[i-1] = '\0';
if( match[0] != '/' ){
                         BOOL env_skip;
  for( i = 0;
       (match[i] != '>') && (match[i] != '\n') && (match[i] != EOF );
       i++){}
  if( match[i] == '>' ){  match[i] = '\0'; }
  
#line 13435 "./tex4ht-c.tex"

if( envChoice == (struct env_c_rec*) 0  ){
  env_skip = !eq_str(match, "default" );
} else {
               struct env_c_rec *p;
  env_skip = TRUE;
  for( p=envChoice; p!=(struct env_c_rec*) 0 ; p = p->next ){
    if( eq_str(match, p->option ) ){ env_skip = FALSE; }
} }


  if( env_skip ){
     
#line 13387 "./tex4ht-c.tex"

                         U_CHAR cur_block[90];
                         BOOL status;
(IGNORED) strcpy((char *)  cur_block, (char *) match);
status = FALSE;
while( !status && (chr != EOF) ){
  chr = 'x';
  for( i = 0; (chr != '\n') && (chr != EOF ) ; i++){
    chr = (int) getc(dot_file);
    match[i] = (U_CHAR) chr;
  }
  match[i-1] = '\0';
  for(i=0; match[i]!='\0'; i++){
    if( match[i] == '>' ){ break; }
  }
  if( (match[0] == '<') && (match[1] == '/')
                        && (match[i] == '>') ){
    match[i]='\0';
    status = eq_str(match+2, cur_block);
  } else { status = FALSE; }
}


} }


      continue;
    }
    if( chr == '\n' ){ continue; }
    do
       if( chr == EOF ) return FALSE;
    while( (chr = getc(dot_file)) != '\n' );
} }


#line 13584 "./tex4ht-c.tex"


static struct env_var_rec * get_env_var
#ifdef ANSI
#define SEP ,
(
    const char *env_var
)
#undef SEP
#else
#define SEP ;
( env_var )
    const char *env_var
;
#undef SEP
#endif
{                U_CHAR   *TEX4HTTFM,  *from;
                  struct env_var_rec *tfm_dirs, *p;
                  int env_var_len;
   tfm_dirs = (struct  env_var_rec *) 0;
   TEX4HTTFM = getenv( env_var );
   if( TEX4HTTFM ){
      env_var_len = (int) strlen((char *) TEX4HTTFM);
      if ( *TEX4HTTFM == *(TEX4HTTFM + env_var_len - 1 ) ){
         from = TEX4HTTFM + env_var_len - 1;
         *from = '\0';
         do{
           from--;
           if( *from == *TEX4HTTFM ){   char * base;
              *from = '\0';
              base = from + 1;
             
#line 13626 "./tex4ht-c.tex"

{                         U_CHAR *str;
  if( *(from+1) == '~' ){
     if( HOME_DIR ){
         str = m_alloc(char, strlen((char *) HOME_DIR)+strlen((char *) base));
         (IGNORED) sprintf(str,"%s%s", HOME_DIR, base+1);
         if( access(str,F_OK) ) {
            warn_i_str2(49, env_var, str); base = NULL; }
         free((void *)  str);
     } else {
         if( access(base,F_OK) ) {
            warn_i_str2(49, env_var, base); base = NULL; }
     }
   } else {
     if( access(base,F_OK) )  {
        warn_i_str2(49, env_var, base); base = NULL; }
}  }


             if( base ){
               
#line 13618 "./tex4ht-c.tex"

p = m_alloc(struct env_var_rec, 1);
p->next = tfm_dirs;
p->base = base;
tfm_dirs = p;


           } }
         } while (from > TEX4HTTFM );
      } else {  warn_i_str2( 49, env_var, TEX4HTTFM); }
    }
    return tfm_dirs;
}


#line 13887 "./tex4ht-c.tex"


static void com_dir
#ifdef ANSI
#define SEP ,
(
     char* p
)
#undef SEP
#else
#define SEP ;
(p)
     char* p
;
#undef SEP
#endif
{   int i;   U_CHAR   str[256];
  (IGNORED) strcpy((char *)  str, (char *) p+2 );
  i = (int) strlen((char *) str) - 1;
  if( str[i] == '!' )  str[i] = '\0';
}


#line 14580 "./tex4ht-c.tex"

#ifdef KPATHSEA

static void export_htf
#ifdef ANSI
#define SEP ,
(
     char** export_str  SEP 
     char str[]
)
#undef SEP
#else
#define SEP ;
(export_str, str)
     char** export_str  SEP 
     char str[]
;
#undef SEP
#endif
{        int i;
          char* p;
          BOOL found;
  i = (int) strlen((char *) str) - 1;
  while( (i>=0) && (str[i] == '\n') ){  str[i--] = '\0';  }
  while( (i>=0) && (str[i] == ' ') ) {  str[i--] = '\0';  }
  if( (i>=0) && (str[i] == '!') ){      str[i--] = '\0';  }
  if( (i>=0) && ((str[i] == '/') || (str[i] == '\\')) ){
                                        str[i--] = '\0';  }
  i -= 8; found = FALSE;
  while( --i>=0 ){
    if( ((str[i] == '/') || (str[i] == '\\')) && (str[i+1]== 'h')
        && (str[i+2]=='t') && (str[i+3]=='-')
        && (str[i+4]=='f') && (str[i+5]=='o')
        && (str[i+6]=='n') && (str[i+7]=='t')
        && (str[i+8]=='s')
        && ((str[i+9] == '/') || (str[i+9] == '\\'))
      ){
         p = str + i + 10; i=0;
         while( *p ){ str[i++] = *(p++); }
         str[i] = '\0';
         found = TRUE;   break;
  } }
  if( found ){
     *export_str = (char *) r_alloc((void *) *export_str,
     (int) strlen((char *) *export_str) + (int) strlen((char *) str) +  2 );
     if( (int) strlen((char *) *export_str) > 0 ){
        (IGNORED) strcat((char *) *export_str, ",");
     }
     (IGNORED) strcat((char *) *export_str, (char *) str);
  }
}
#endif


#line 15309 "./tex4ht-c.tex"


static FILE*  search_in_dot_file
#ifdef ANSI
#define SEP ,
(
                                                 int   typ SEP 
                                                 const U_CHAR  *name SEP 
                                                 const U_CHAR  *flags SEP 
                                      struct env_var_rec *env_dirs

)
#undef SEP
#else
#define SEP ;
( typ, name, flags, env_dirs)
                                                 int   typ SEP 
                                                 const U_CHAR  *name SEP 
                                                 const U_CHAR  *flags SEP 
                                      struct env_var_rec *env_dirs

;
#undef SEP
#endif
{                                         U_CHAR  *ch, dir[256];
                                          FILE* file;
#ifndef KPATHSEA
   if( cache_files != (FILE *) 0 ){
      
#line 14019 "./tex4ht-c.tex"

                 U_CHAR cache_dir[256], dot_dir[256], *p, *q;
                 BOOL flag;
                 int  n,  ch;
(IGNORED) fseek(cache_files, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
ch = (int) getc(cache_files);
while ( ch != EOF ){
  if ( ch == ' ' ) { 
#line 14036 "./tex4ht-c.tex"

q = cache_dir;
do
  *(q++) = ch = (int) getc(cache_files);
while( (ch !='\n') && (ch != EOF) );
*(q-1 - (*(q-2) == 
#line 15558 "./tex4ht-c.tex"

#if defined(__DJGPP__)
 '\\'
#else
 '/'
#endif

)
#ifdef DOS_WIN32
      - (*(q-2) == '/')
#endif
 ) = '\0';

 }
  else { 
#line 14048 "./tex4ht-c.tex"

p = name;  flag = FALSE;
while( *(p++) == ch ){
  ch = (int) getc(cache_files);
}
if( (*(p-1) == '\0') && ((ch == '\n') || (ch == EOF)) ){ flag = TRUE; }
else{
  while( (ch != '\n') && (ch != EOF) ) {  ch = (int) getc(cache_files); }
}


    if( flag ){
       
#line 14059 "./tex4ht-c.tex"

flag = FALSE;
(IGNORED) fseek(dot_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
while( search_dot_file( typ ) && !flag ){        U_CHAR *q, save_ch;
                                                 int  n, m;
  q = dot_dir;
  do
     *(q++) = ch = (int) getc(dot_file);
  while( (ch !='\n') && (ch != EOF) );
  flag = *(q - 2) = '!';
  q -= (flag? 2 : 1);
  *(q - (*(q-1) == 
#line 15558 "./tex4ht-c.tex"

#if defined(__DJGPP__)
 '\\'
#else
 '/'
#endif

)
#ifdef DOS_WIN32
      - (*(q-1) == '/')
#endif
   ) = '\0';
  if( (n = strlen((char *) dot_dir)) > (m = strlen((char *) cache_dir)) ){ flag = FALSE; }
  else {
    save_ch = *(cache_dir + n);
    *(cache_dir + n) = '\0';
    flag = eq_str(dot_dir,cache_dir) && ( flag || (m == n) );
    *(cache_dir + n) = save_ch;
  }
}


       if( flag ){ 
#line 14085 "./tex4ht-c.tex"

n = (int) strlen((char *) cache_dir);
cache_dir[n] = dir_path_slash(cache_dir);
cache_dir[n+1] = '\0';
(IGNORED) strcat((char *) cache_dir, (char *) name);

 }
  } }
  if ( ch != EOF ){ ch = (int) getc(cache_files); }
}


   }
#endif
   (IGNORED) fseek(dot_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
   while( search_dot_file( typ ) ){
      ch = dir;
      while((*(ch++) = (int) getc(dot_file)) > ' ');
      while(*(ch-1) !=  '\n'){
          *(ch-1) = (int) getc(dot_file);
      }
      *(ch-1) = '\0';
      file = search_file_base(name, dir, flags, env_dirs);
      if( file != NULL ){
#ifndef KPATHSEA
        tex4ht_fls = TRUE;
#endif
        return file;
   }  }
   return NULL;
}


#line 15354 "./tex4ht-c.tex"


static FILE*  search_file_base
#ifdef ANSI
#define SEP ,
(
                                                 const U_CHAR  *name SEP 
                                                 const U_CHAR  *dir SEP 
                                                 const U_CHAR  *flags SEP 
                                      struct env_var_rec *env_dirs

)
#undef SEP
#else
#define SEP ;
( name, dir, flags, env_dirs)
                                                 const U_CHAR  *name SEP 
                                                 const U_CHAR  *dir SEP 
                                                 const U_CHAR  *flags SEP 
                                      struct env_var_rec *env_dirs

;
#undef SEP
#endif
{                                         U_CHAR  *p;
                                          FILE* file;
   if( *dir == '~' ){
     while( TRUE ){
        p = abs_addr(dir, env_dirs? env_dirs->base : NULL);
        file = search_file(name, p, flags);
        free((void *) p);
        if( file || !env_dirs ){  return file; }
        env_dirs = env_dirs->next;
     }
   } else {
     file = search_file(name, dir, flags);
   }
   return file;
}


#line 15385 "./tex4ht-c.tex"


static char *  abs_addr
#ifdef ANSI
#define SEP ,
(
                                                 const U_CHAR  *dir SEP 
                                                 const U_CHAR  *base

)
#undef SEP
#else
#define SEP ;
( dir, base)
                                                 const U_CHAR  *dir SEP 
                                                 const U_CHAR  *base

;
#undef SEP
#endif
{                                         U_CHAR  *p;
   p = m_alloc(char, (int) strlen( dir )            +
                     (base? (int) strlen( base ):0) +
                     (int) strlen((char *)  HOME_DIR  )      + 1 );
   *p = '\0';
   if( (*(dir+1) == '~') && base ){
     if( *base == '~' ){
       (IGNORED) strct(p, HOME_DIR);
       (IGNORED) strct(p, base+1);
     } else {
       (IGNORED) strct(p, base);
     }
     (IGNORED) strct(p, dir+2);
   } else {
     (IGNORED) strct(p, HOME_DIR);
     (IGNORED) strct(p, dir+1);
   }
   return p;
}


#line 15426 "./tex4ht-c.tex"


static FILE* search_file
#ifdef ANSI
#define SEP ,
(
     const char     *name SEP 
     const U_CHAR   *dir SEP 
     const U_CHAR   *flags

)
#undef SEP
#else
#define SEP ;
( name, dir, flags )
     const char     *name SEP 
     const U_CHAR   *dir SEP 
     const U_CHAR   *flags

;
#undef SEP
#endif
{   FILE*  file;
     U_CHAR     str[256];
     int i;
     BOOL subs;
  
#line 15467 "./tex4ht-c.tex"

if( (file = f_open(name, flags)) != NULL ){
   return file; }


      (IGNORED) strcpy((char *) str, dir);
      i = (int) strlen((char *) str) - 1;
      subs = str[i] == '!';
      if( subs )  str[i] = '\0';  else i++;
      
#line 15451 "./tex4ht-c.tex"

(IGNORED) strct(str,
#if defined(__DJGPP__)
 (( dir[i-1] == '/') ||  ( dir[i-1] == '\\'))
 ?  ""
 :  (is_forward_slash(dir)?  "/" : "\\" )
#else
 (dir[i-1] == '/')? "" : "/"
#endif
  );



  
#line 15472 "./tex4ht-c.tex"

(IGNORED) strct(str,name);
if( (file = f_open(str, flags)) != NULL ){
   str[i] = '\0'; add_to_cache(str,name,i);
   return file; }


  str[i] = '\0';
  return  subs?  search_file_ext( name, str, flags):
                NULL;
}


#line 15486 "./tex4ht-c.tex"


static void add_to_cache
#ifdef ANSI
#define SEP ,
(
                         const char*  dir SEP  const char* name SEP  int n

)
#undef SEP
#else
#define SEP ;
(dir,name,n)
                         const char*  dir SEP  const char* name SEP  int n

;
#undef SEP
#endif
{
                         struct cache_font_rec *cur_cache_font;

   
#line 13681 "./tex4ht-c.tex"

{     int found;
   found = FALSE;
   for( cur_cache_font = cache_font;
        cur_cache_font;
        cur_cache_font = cur_cache_font->next )
   { found = found || eq_str(cur_cache_font->dir, dir ) ;
     if( found ) break;  }
   if( !found ){
      cur_cache_font = m_alloc(struct cache_font_rec, 1);
      
#line 13944 "./tex4ht-c.tex"

cur_cache_font->cache_file = (struct cache_file_rec *) 0;


      cur_cache_font->dir = m_alloc(char, n+1);
      (IGNORED) strcpy((char *) cur_cache_font->dir, dir);
      if( !cache_font ){
         cur_cache_font->next = cache_font;
         cache_font = cur_cache_font;
      } else if ( gt_str(cache_font->dir, dir) ) {
         cur_cache_font->next = cache_font;
         cache_font = cur_cache_font;
      } else {
                            struct cache_font_rec *  after_cache_font;
         after_cache_font = cache_font;
         while( after_cache_font->next ){
           if ( gt_str(after_cache_font->next->dir, dir) ) { break; }
           after_cache_font = after_cache_font->next;
         }
         cur_cache_font->next = after_cache_font->next;
         after_cache_font->next = cur_cache_font;
}  }  }


   
#line 13948 "./tex4ht-c.tex"

{
             struct cache_file_rec *file_rec, *prev_file_rec;
  prev_file_rec = (struct cache_file_rec *) 0;
  file_rec = cur_cache_font->cache_file;
  while( file_rec ) {
    if( !gt_str(name,file_rec->file) ) break;
    prev_file_rec = file_rec;
    file_rec = file_rec->next;
  }
  {
                         struct cache_file_rec * file_entry;
                         BOOL flag;
    flag = TRUE;
    if( file_rec ) {
      if( eq_str(name,file_rec->file) ){ flag = FALSE; }
    }
    if( flag ) {
      
#line 13973 "./tex4ht-c.tex"

file_entry = m_alloc(struct cache_file_rec, 1);
file_entry->file = m_alloc(char, strlen(name)+1);
(IGNORED) strcpy((char *) file_entry->file, name);
if( ! cur_cache_font->cache_file ){
  cur_cache_font->cache_file = file_entry;
 file_entry->next = (struct cache_file_rec *) 0;
} else if( !prev_file_rec ){
   file_entry->next = cur_cache_font->cache_file;
   cur_cache_font->cache_file = file_entry;
} else {
   file_entry->next = prev_file_rec->next;
   prev_file_rec->next = file_entry;
}


    }
  }
}


}


#line 15507 "./tex4ht-c.tex"


static FILE* search_file_ext
#ifdef ANSI
#define SEP ,
(
    const char     *name SEP 
    const U_CHAR   *dir SEP 
    const U_CHAR   *flags

)
#undef SEP
#else
#define SEP ;
( name, dir, flags )
    const char     *name SEP 
    const U_CHAR   *dir SEP 
    const U_CHAR   *flags

;
#undef SEP
#endif
{  U_CHAR   str[256];
    FILE*  file;
    int    n;
  n = (int) strlen(dir);
  (IGNORED) sprintf(str,
#if defined(__DJGPP__)
 (( dir[n-1] == '/') ||  ( dir[n-1] == '\\'))
 ?  "%s%s"
 :  (is_forward_slash(dir)?  "%s/%s" : "%s\\%s" )
#else
   (dir[n-1] == '/')? "%s%s" : "%s/%s"
#endif
   , dir, name);
  if( (file = f_open(str,flags)) != NULL ){
     add_to_cache(dir,name,n);
     return file;
  }
  if( (str[n] == 
#line 15558 "./tex4ht-c.tex"

#if defined(__DJGPP__)
 '\\'
#else
 '/'
#endif

)
#ifdef DOS_WIN32
    ||  (str[n] == '/' )
#endif
    ) n++;
  str[n-1] = '\0';
#ifndef NOSUBDIR
#ifdef WIN32
  
#line 537 "./tex4ht-c.tex"

{
    WIN32_FIND_DATA find_file_data;
    HANDLE hnd;
    int proceed;
    (IGNORED) strcpy((char *) dirname, (char *) str);
    strct(dirname, "/*.*");       
    hnd = FindFirstFile(dirname, &find_file_data);
    if (hnd != INVALID_HANDLE_VALUE) {
      
#line 551 "./tex4ht-c.tex"

proceed = 1;
while (proceed) {
  if( !eq_str(find_file_data.cFileName, ".")  &&
      !eq_str(find_file_data.cFileName, "..") )
    {
      (IGNORED) strcpy((char *)  str+n, (char *) find_file_data.cFileName );
      str[n-1] = dir_path_slash(str);
      if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
  if( (file = search_file_ext(name, str, flags)) != NULL ){
    FindClose(hnd);
    return file; }
      }
    }
  proceed = FindNextFile(hnd, &find_file_data);
}
FindClose(hnd);


    }
}


#else
  
#line 15598 "./tex4ht-c.tex"

{      DIR             *dp;
       
#line 15727 "./tex4ht-c.tex"


#ifdef STRUCT_DIRECT
   struct direct
#else
   struct dirent
#endif

   *dirp;
       struct STSTAT     buf;
  if( (dp = opendir( str )) != NULL ){
    while( (dirp = readdir(dp)) != NULL ){
      if( !eq_str(dirp->d_name, ".")  &&
          !eq_str(dirp->d_name, "..") )
      { 
#line 15613 "./tex4ht-c.tex"

(IGNORED) strcpy((char *)  str+n, (char *) dirp->d_name );
str[n-1] = dir_path_slash(str);
if( LSTAT(str, &buf) >= 0 )
   if( S_ISDIR( buf.st_mode ) )
      if( (file = search_file_ext(name, str, flags)) != NULL ){
         (void) closedir(dp);
         return file; }


    } }
    (void) closedir(dp);
} }


#endif
#endif
  return NULL;
}


#line 15584 "./tex4ht-c.tex"

#if defined(__DJGPP__)

static BOOL is_forward_slash
#ifdef ANSI
#define SEP ,
(
                                    const char* str
)
#undef SEP
#else
#define SEP ;
(str)
                                    const char* str
;
#undef SEP
#endif
{
   while( *str ){  if( *(str++) == '/' ) { return TRUE; } }
   return FALSE;
}
#endif


#line 15806 "./tex4ht-c.tex"


static FILE* f_open_pathed_filename
#ifdef ANSI
#define SEP ,
(
                          const char*  name  SEP 
                          const char*  flags
)
#undef SEP
#else
#define SEP ;
( name, flags )
                          const char*  name  SEP 
                          const char*  flags
;
#undef SEP
#endif
{                        FILE* file;
                          U_CHAR *str;
  file = NULL;
  if( *name == '~' ){
     if( HOME_DIR ){
         str = m_alloc(char, strlen((char *) HOME_DIR)+strlen(name));
         (IGNORED) sprintf(str,"%s%s", HOME_DIR, name+1);
         file = f_open(str,flags);
         free((void *)  str);
     }
  } else {  file = f_open( name, flags );   }
  return file;
}


#line 15840 "./tex4ht-c.tex"


static  INTEGER put_4ht_ch
#ifdef ANSI
#define SEP ,
(    int ch  SEP  FILE* htFile
)
#undef SEP
#else
#define SEP ;
(ch,htFile)    int ch  SEP  FILE* htFile
;
#undef SEP
#endif
{
                                 int c;
  c = ch;
  if( ch=='&' ){
     
#line 15906 "./tex4ht-c.tex"

flush_uni();


     if( put_4ht_off ){
        c = putc( ch, htFile );
     } else {
        uni_code[0] = '&';
        uni_code_p = 1;
        put_4ht_file = htFile;
     }
  } else
  if( uni_code_p ){
    if( ch == ';' ){ 
#line 16129 "./tex4ht-c.tex"

if( uni_code[1] != '#' ){
   
#line 15906 "./tex4ht-c.tex"

flush_uni();


   (IGNORED)  putc( ch, htFile );
}
else{
      int i, base, value, digit;
  if( (uni_code[2] == 'x') || (uni_code[2] == 'X') ){
     base =16; i=3;
  } else { base=10; i=2; }
  value = 0;
  for( ; i<uni_code_p; i++ ){
    digit = uni_code[i];
    if( (digit>='0') && (digit<='9') ){  digit -= '0'; }
    else if( (digit>='A') && (digit<='F') ){  digit -= BASE_A; }
    else if( (digit>='a') && (digit<='f') ){  digit -= BASE_a; }
    else { value = -1; break; }
    if( digit >= base ){ value=-1; break; }
    value = value*base + digit;
  }
  if( value<0 ){ 
#line 15906 "./tex4ht-c.tex"

flush_uni();


                 (IGNORED)  putc( ch, htFile );
  } else {
     
#line 16156 "./tex4ht-c.tex"

    int bottom, mid, top;
    BOOL found=FALSE;
bottom = 0; top = charset_n;
while( !found ){
   mid = (bottom + top) / 2;
   if( value == charset[mid].ch ){
      
#line 16182 "./tex4ht-c.tex"

{          U_CHAR *p;
   p = charset[mid].str;
   while( *p != '\0' ){
     if( *p=='\\' ){
       p++;
       if( *p=='\\' ){
         (IGNORED) putc( '\\', htFile );
       } else {
             int i;
         i = *p - '0';
         while( *(++p) != '\\' ){ i = 10*i + *p - '0'; }
         (IGNORED) putc( i, htFile );
     } }
     else {
       (IGNORED) putc( *p, htFile );
       if ( (*p=='&') && u10 ){ 
#line 16256 "./tex4ht-c.tex"

if ( *(p+1) == '#' ){
  p++;
  (IGNORED) putc( '#', htFile );
  if ( (*(p+1) == 'x') || (*(p+1) == 'X') ){
                 int value, digit;
                 U_CHAR *q;
     q = p+2;
     value = 0;
     digit = *(q++);
     while( digit!=0 ){
       if( (digit>='0') && (digit<='9') ){
          value = value*16 + digit - '0';
       }
       else if( (digit>='A') && (digit<='F') ){
          value = value*16 + digit - 'A'+10;
       }
       else if( (digit>='a') && (digit<='f') ){
          value = value*16 + digit - 'a'+10; }
       else {
         if( digit == ';' ){
           
#line 16287 "./tex4ht-c.tex"

              char   uni_10[MAX_UNI_CODE];
              int n;
n = 0;
while( value>0 ){
  uni_10[ n++ ] = value % 10 + '0';
  value /= 10;
}
while( n>0 ){
   (IGNORED) putc(  uni_10[--n], htFile );
}


           p=q-2;
         }
         break;
       }
       digit = *(q++);
     }
} }

 }
     }
     p++;
}  }


      found = TRUE;
   } else if( value < charset[mid].ch ){
      if( bottom == top ){ break; }
      top = mid;
   }
   else {
     if ( bottom < mid ){  bottom = mid; }
     else if ( bottom<top ){ bottom++; }
     else{ break; }
   }
}
if( ! found ){
   if( u10 || utf8 ){ 
#line 16225 "./tex4ht-c.tex"

      short  n;
      long   dec;
      int    ch;
      char   uni_10[MAX_UNI_CODE];
if( (uni_code[2] == 'x') || (uni_code[2] == 'X') ) {
   dec = 0;
   for(  n=3; n<uni_code_p; n++ ){
      ch = uni_code[n];
      dec = 16*dec +
             ((ch > '9')?
                         ( 10 + ((ch > 'Z')? (ch-'a') : (ch-'A')) )
                       : (ch-'0'));
   }
   if( u10 ){ 
#line 16245 "./tex4ht-c.tex"

   if( dec == 0 ){
      uni_code_p = 3;  uni_code[2] = '0';
   } else {
      n = 0;
      while( dec > 0 ){  uni_10[ n++ ] = dec % 10 + '0';   dec /= 10;  }
      uni_code_p = 2;
      while( n>0 ){  uni_code[ uni_code_p++ ] = uni_10[ --n ]; }
   }

 }
   else     { 
#line 16366 "./tex4ht-c.tex"


#line 16316 "./tex4ht-c.tex"

if( dec < 0x80 ){
   uni_code_p = 1;  uni_code[0] = dec;
}
else if( dec < 0x800 ){
   uni_code_p = 2;
   uni_code[0] = (dec >> 6)           | 0xC0;
   uni_code[1] = (dec & 0x3F)         | 0x80;
}
else if( dec < 0x10000 ){
   uni_code_p = 3;
   uni_code[0] = (dec >> 12)          | 0xE0;
   uni_code[1] = ((dec >> 6)  & 0x3F) | 0x80;
   uni_code[2] =  (dec        & 0x3F) | 0x80;
}
else if( dec < 0x200000 ){
   uni_code_p = 4;
   uni_code[0] = (dec >> 18)          | 0xF0;
   uni_code[1] = ((dec >> 12) & 0x3F) | 0x80;
   uni_code[2] = ((dec >>  6) & 0x3F) | 0x80;
   uni_code[3] =  (dec        & 0x3F) | 0x80;
}
else if( dec < 0x4000000 ){
   uni_code_p = 5;
   uni_code[0] = (dec >> 24)          | 0xF8;
   uni_code[1] = ((dec >> 18) & 0x3F) | 0x80;
   uni_code[2] = ((dec >> 12) & 0x3F) | 0x80;
   uni_code[3] = ((dec >>  6) & 0x3F) | 0x80;
   uni_code[4] =  (dec        & 0x3F) | 0x80;
}
else if( dec <= 0x7FFFFFFF ){
   uni_code_p = 6;
   uni_code[0] = (dec >> 30)          | 0xFC;
   uni_code[1] = ((dec >> 24) & 0x3F) | 0x80;
   uni_code[2] = ((dec >> 18) & 0x3F) | 0x80;
   uni_code[3] = ((dec >> 12) & 0x3F) | 0x80;
   uni_code[4] = ((dec >>  6) & 0x3F) | 0x80;
   uni_code[5] =  (dec        & 0x3F) | 0x80;
}


else {
   n = 0;
   while( dec > 0 ){  uni_10[ n++ ] = dec % 10 + '0';   dec /= 10;  }
   uni_code_p = 2;
   while( n>0 ){  uni_code[ uni_code_p++ ] = uni_10[ --n ]; }
}

 }
}

 }
   
#line 15906 "./tex4ht-c.tex"

flush_uni();


   if( !utf8 ){ (IGNORED) putc( ch, htFile ); }
}


} }

 uni_code_p = 0; }
    else if (     ((uni_code_p+1) == MAX_UNI_CODE)
               ||
                  (    ((ch<'0') || (ch>'9'))
                    && ((ch<'a') || (ch>'f'))
                    && ((ch<'A') || (ch>'F'))
                    && (ch!='#')
                    && (ch!='x')
                    && (ch!='X')
                  )
            )
    { 
#line 15906 "./tex4ht-c.tex"

flush_uni();


      c = putc( ch, htFile );
    } else { uni_code[ uni_code_p++ ] = ch; }
  } else { c = putc( ch, htFile ); }
  return  c;
}


#line 15914 "./tex4ht-c.tex"

static void flush_uni( MYVOID )
{
                                   int i;
  for(  i=0; i<uni_code_p; i++ ){
    (IGNORED)  putc( uni_code[i], put_4ht_file );
  }
  uni_code_p = 0;
  put_4ht_file = (FILE *) 0;
}


#line 16577 "./tex4ht-c.tex"


static  INTEGER insert_ch
#ifdef ANSI
#define SEP ,
(    int ch
)
#undef SEP
#else
#define SEP ;
(ch)    int ch
;
#undef SEP
#endif
{
   if( !ignore_chs ){
        BOOL flag;
     
#line 16599 "./tex4ht-c.tex"

flag = FALSE;
if( 
#line 4338 "./tex4ht-c.tex"

next_str

 ) {
  if( eq_str(next_str, "") ) {
    flag = TRUE;
    free((void *) next_str);
    next_str = (char *) 0;
} }


     if( !flag ){
        try_new_line();
        
#line 10649 "./tex4ht-c.tex"


#line 12298 "./tex4ht-c.tex"

if( x_val > needs_end_accent ){
  
#line 12281 "./tex4ht-c.tex"

if( needs_end_accent && t_accent_template ){
   
#line 2609 "./tex4ht-c.tex"

if( span_on && in_span_ch ){
   if( *end_span[0] ){
       in_span_ch = FALSE;
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       (IGNORED) fprintf(cur_o_file, "%s", end_span[0]);
}  }


   (IGNORED) fprintf(cur_o_file, "%s", t_accent_fifth);
   needs_end_accent = FALSE; 
#line 12424 "./tex4ht-c.tex"

needs_accented_sym--;


}


}



#line 2543 "./tex4ht-c.tex"

if( span_on && (default_font != font_tbl[cur_fnt].num) ){
  if( !ch_map_flag && start_span ){
    if( span_name_on ){
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       if( span_open[0] )  if( *span_open[0] )
           (IGNORED) fprintf(cur_o_file, "%s", span_open[0]);
       if( span_name[0] )  if( *span_name[0] )
           (IGNORED) fprintf(cur_o_file,
               span_name[0], font_tbl[cur_fnt].family_name);
       if( span_size[0] )  if( *span_size[0] )
           (IGNORED) fprintf(cur_o_file,
               span_size[0], font_tbl[cur_fnt].font_size);
       if( span_mag[0] )
         if( *span_mag[0]  && (font_tbl[cur_fnt].mag != 100))
           (IGNORED) fprintf(cur_o_file,
                       span_mag[0], font_tbl[cur_fnt].mag);
       if( span_ch[0] )  if( *span_ch[0] )
           (IGNORED) fprintf(cur_o_file, "%s", span_ch[0]);
    }
    start_span = FALSE;
  }
}

 
#line 2897 "./tex4ht-c.tex"

if( trace_dvi_C ){
   if( !ch_map_flag ){
     
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


     if( *trace_dvi_del_C != '\0' ){
        (IGNORED) fprintf(cur_o_file,
            block_start? "%s%s %d B" : "%s%s %d",
            trace_dvi_del_C, font_tbl[cur_fnt].name, ch);
     }
     (IGNORED) fprintf(cur_o_file,"%s", end_trace_dvi_del_C);
   }
   block_start = FALSE;
}



#line 12913 "./tex4ht-c.tex"

if( show_class  && !pause_class && !ignore_subclass_del){
   if( !stack[stack_n].no_left_del && stack[stack_n+1].active_class_del ){
      
#line 12988 "./tex4ht-c.tex"

(IGNORED) print_f( (stack[stack_n+1].temp_class_del)?
                      stack[stack_n+1].temp_class_open
                   :  stack[stack_n+1].class_open);


      if( !stack[stack_n+1].ignore_subclass_del ){
         
#line 12926 "./tex4ht-c.tex"

math_class = math_class_of( ch, cur_fnt );
(IGNORED) print_f( open_class[math_class]);


      }
   } else {
      
#line 12926 "./tex4ht-c.tex"

math_class = math_class_of( ch, cur_fnt );
(IGNORED) print_f( open_class[math_class]);


}  }

 
#line 8194 "./tex4ht-c.tex"

if( pos_dvi ){       long int d;
  if( *pos_text ){
    
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


    (IGNORED) fprintf(cur_o_file, pos_text,
         pos_x_A * (x_val - base_pos_x) + pos_x_B,
         pos_y_C * (y_val - base_pos_y) + pos_y_D);
  }
  if( x_val < min_pos_x )                       min_pos_x = x_val;
  if( (d = x_val + 
#line 9851 "./tex4ht-c.tex"

(int)(
#line 9837 "./tex4ht-c.tex"

design_size_to_pt( *(font_tbl[cur_fnt].wtbl
                     +  (int) (
#line 9845 "./tex4ht-c.tex"

*(font_tbl[cur_fnt].char_wi +  (int)
   ( ch - font_tbl[cur_fnt].char_f)% 256)

) )
                 )
* (double) font_tbl[cur_fnt].scale

)

)  > max_pos_x ) max_pos_x = d;
  if( (d = y_val - 
#line 9875 "./tex4ht-c.tex"

(int)(
#line 9860 "./tex4ht-c.tex"

design_size_to_pt( *(font_tbl[cur_fnt].htbl
                 +  (int) (
#line 9867 "./tex4ht-c.tex"

( *(font_tbl[cur_fnt].char_hidp +  (int)
   ( ch - font_tbl[cur_fnt].char_f)% 256)
  >>  4
) & 0x0F

)) )
   * (double) font_tbl[cur_fnt].scale

)

) < min_pos_y ) min_pos_y = d;
  if( (d = y_val + 
#line 9898 "./tex4ht-c.tex"

(int)(
#line 9884 "./tex4ht-c.tex"

design_size_to_pt( *(font_tbl[cur_fnt].dtbl
                 +  (int) (
#line 9891 "./tex4ht-c.tex"

( *(font_tbl[cur_fnt].char_hidp +  (int)
   ( ch - font_tbl[cur_fnt].char_f)% 256)
) & 0x0F

)) )
* (double) font_tbl[cur_fnt].scale

)

)  > max_pos_y ) max_pos_y = d;
}



#line 12392 "./tex4ht-c.tex"

if( a_accent_template && needs_accented_sym ){
  (IGNORED) fprintf(cur_o_file, "%s%s%s%d%s%d%s",
     a_accent_first,   font_tbl[cur_fnt].family_name,
     a_accent_second, ch, a_accent_third,
     font_tbl[cur_fnt].accented[ch]?
       font_tbl[cur_fnt].accented_array[font_tbl[cur_fnt].accented[ch]-1]
       : 0,
     a_accent_fourth);
}


if( 
#line 4332 "./tex4ht-c.tex"

next_char

 != -1 ) {
   
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


   (IGNORED) put_4ht_ch( 
#line 4332 "./tex4ht-c.tex"

next_char

, cur_o_file );
   
#line 4332 "./tex4ht-c.tex"

next_char

 = -1;
} else if( 
#line 4338 "./tex4ht-c.tex"

next_str

 ) {
   
#line 4347 "./tex4ht-c.tex"

if( keepChar ){
  keepChar=FALSE;
  { 
#line 10678 "./tex4ht-c.tex"

   int gif_flag, chr, r_ch;
    BOOL  ch_str_flag;
r_ch = ch - font_tbl[cur_fnt].char_f;
gif_flag = font_tbl[cur_fnt].gif1[r_ch];
ch_str_flag = get_bit( font_tbl[cur_fnt].ch_str, r_ch);
chr = ((r_ch == 255) && font_tbl[cur_fnt].ch255 )? 256 :
                         *(font_tbl[cur_fnt].ch + r_ch);
if( (gif_flag % 2) || ch_str_flag ){      design_ch = ch;
             { 
#line 10701 "./tex4ht-c.tex"

      U_CHAR  str[256], *p;
      BOOL sv;
      int mag;
sv = special_on;   special_on = TRUE;
if( gif_ch && (gif_flag % 2) ){
   
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


   if( !gif_open[gif_flag] ){
     
#line 10778 "./tex4ht-c.tex"

(IGNORED) sprintf(str,
   "configuration for htf class %d (char %d of %s.htf)",
   gif_flag, ch,font_tbl[cur_fnt].name
  );
warn_i_str(50,str);


#line 10786 "./tex4ht-c.tex"

gif_open[gif_flag] = m_alloc(char,
   
#line 10811 "./tex4ht-c.tex"

29

);
(IGNORED) strcpy((char *) gif_open[gif_flag],
           
#line 10807 "./tex4ht-c.tex"

"<img src=\"+\" alt=\"+++++\" />+"

);
gif_alt[gif_flag] = gif_open[gif_flag]+11;
  *(gif_alt[gif_flag] - 1) = '\0';
gif_class[gif_flag] = gif_open[gif_flag]+19;
  *(gif_class[gif_flag] - 1) = '\0';
gif_size[gif_flag] = gif_open[gif_flag]+20;
  *(gif_size[gif_flag] - 1) = '\0';
gif_mag[gif_flag] = gif_open[gif_flag]+21;
  *(gif_mag[gif_flag] - 1) = '\0';
gif_ord[gif_flag] = gif_open[gif_flag]+22;
  *(gif_ord[gif_flag] - 1) = '\0';
gif_end[gif_flag] = gif_open[gif_flag]+23;
  *(gif_end[gif_flag] - 1) = '\0';
gif_id[gif_flag] = gif_open[gif_flag]+28;
  *(gif_id[gif_flag] - 1) = '\0';


   } else if( !get_bit( class_on, gif_flag ) ) {
      notify_class_info(gif_flag);
      store_bit_I( class_on, gif_flag );
   }
   
#line 11467 "./tex4ht-c.tex"

p= gif_open[gif_flag];
if( p )
if( *p ){
   print_f(p); (IGNORED) strcpy((char *) str, (char *) font_tbl[cur_fnt].name);
   mag = (int) ((double) font_tbl[cur_fnt].scale /
                font_tbl[cur_fnt].design_sz  * 10 );
   
#line 10893 "./tex4ht-c.tex"

if( !dos_file_names ){
   print_f(font_tbl[cur_fnt].name);
   if( mag == 10 )  (IGNORED) sprintf(str, GIF_I,   design_ch, gif);
   else             (IGNORED) sprintf(str, GIF_II,  mag, design_ch, gif);
}


#line 10914 "./tex4ht-c.tex"


if( dos_file_names ){
   dos_gif_file(str, mag, design_ch);
   print_f(str);
   (IGNORED) sprintf(str, GIF_VII, gif);
}



   print_f(str);
   add_bit( font_tbl[cur_fnt].gif_on, r_ch, 1 );
}


   
#line 11500 "./tex4ht-c.tex"

p = gif_alt[gif_flag];
if( p )
  if( *p ){
     print_f(p);  put_alt_ch(chr,ch_str_flag);
}


   
#line 11514 "./tex4ht-c.tex"

p = gif_class[gif_flag];
if( p )
  if( *p ){
    (IGNORED) fprintf(cur_o_file, p,
                   font_tbl[cur_fnt].family_name);  }


   
#line 11538 "./tex4ht-c.tex"

p = gif_size[gif_flag];
if( p )
  if( *p ){
     (IGNORED) fprintf(cur_o_file, p,
                   font_tbl[cur_fnt].font_size);  }


   
#line 11555 "./tex4ht-c.tex"

p = gif_mag[gif_flag];
if( p )
  if( *p && (font_tbl[cur_fnt].mag != 100) ){
     (IGNORED) fprintf(cur_o_file, p, font_tbl[cur_fnt].mag);
}


   
#line 11570 "./tex4ht-c.tex"

p = gif_ord[gif_flag];
if( p )
  if( *p ){
    (IGNORED) fprintf(cur_o_file, p, ch);
}


   
#line 11587 "./tex4ht-c.tex"

p = gif_end[gif_flag];
if( p )
  if( *p ){ print_f( p ); }


} else  { 
#line 10830 "./tex4ht-c.tex"

if( !gif_flag || (gif_flag % 2)  || ch_map_flag ) {
   put_alt_ch(chr,ch_str_flag);  }
else{ 
#line 11723 "./tex4ht-c.tex"


#line 11739 "./tex4ht-c.tex"

if( gif_flag && !get_bit( class_on, gif_flag ) ) {
  notify_class_info(gif_flag);
  store_bit_I( class_on, gif_flag );
}


if( span_on ){
   
#line 11749 "./tex4ht-c.tex"

if( span_open[gif_flag] )
  if( *span_open[gif_flag] ){
     print_f( span_open[gif_flag] );
}


   
#line 11773 "./tex4ht-c.tex"

if( span_name[gif_flag] )
  if( *span_name[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_name[gif_flag],
                        font_tbl[cur_fnt].family_name);
}


   
#line 11783 "./tex4ht-c.tex"

if( span_size[gif_flag] )
  if( *span_size[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_size[gif_flag],
                        font_tbl[cur_fnt].font_size);
}


   
#line 11792 "./tex4ht-c.tex"

if( span_mag[gif_flag] )
  if( *span_mag[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_mag[gif_flag],
                        font_tbl[cur_fnt].mag);
}


   
#line 11801 "./tex4ht-c.tex"

if( span_ord[gif_flag] )
  if( *span_ord[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_ord[gif_flag], chr);
}


   
#line 11765 "./tex4ht-c.tex"

if( span_ch[gif_flag] )
  if( *span_ch[gif_flag] ){
    print_f( span_ch[gif_flag] );
}


}
put_alt_ch(chr,ch_str_flag);
if( span_on ){
   
#line 11756 "./tex4ht-c.tex"

if( end_span[gif_flag] )
  if( *end_span[gif_flag] ){
     print_f( end_span[gif_flag] );
}


}

 }

 }
special_on = sv;

 } design_ch = 0;    }
else { 
#line 10821 "./tex4ht-c.tex"

if( !gif_flag || (gif_flag % 2) || ch_map_flag ) {  put_char(chr);
} else{ 
#line 11704 "./tex4ht-c.tex"


#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }



#line 11739 "./tex4ht-c.tex"

if( gif_flag && !get_bit( class_on, gif_flag ) ) {
  notify_class_info(gif_flag);
  store_bit_I( class_on, gif_flag );
}


if( span_on ){
   
#line 11749 "./tex4ht-c.tex"

if( span_open[gif_flag] )
  if( *span_open[gif_flag] ){
     print_f( span_open[gif_flag] );
}


   
#line 11773 "./tex4ht-c.tex"

if( span_name[gif_flag] )
  if( *span_name[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_name[gif_flag],
                        font_tbl[cur_fnt].family_name);
}


   
#line 11783 "./tex4ht-c.tex"

if( span_size[gif_flag] )
  if( *span_size[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_size[gif_flag],
                        font_tbl[cur_fnt].font_size);
}


   
#line 11792 "./tex4ht-c.tex"

if( span_mag[gif_flag] )
  if( *span_mag[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_mag[gif_flag],
                        font_tbl[cur_fnt].mag);
}


   
#line 11801 "./tex4ht-c.tex"

if( span_ord[gif_flag] )
  if( *span_ord[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_ord[gif_flag], chr);
}


   
#line 11765 "./tex4ht-c.tex"

if( span_ch[gif_flag] )
  if( *span_ch[gif_flag] ){
    print_f( span_ch[gif_flag] );
}


}
put_char(chr);
if( span_on ){
   
#line 11756 "./tex4ht-c.tex"

if( end_span[gif_flag] )
  if( *end_span[gif_flag] ){
     print_f( end_span[gif_flag] );
}


}

 }

 }

 }
}


   print_f_4ht(next_str); free((void *) next_str);
   next_str = (char *) 0;
} else {
   if( verb_ch ){
      
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


      (IGNORED) put_4ht_ch( ch, cur_o_file );
   } else {  
#line 10678 "./tex4ht-c.tex"

   int gif_flag, chr, r_ch;
    BOOL  ch_str_flag;
r_ch = ch - font_tbl[cur_fnt].char_f;
gif_flag = font_tbl[cur_fnt].gif1[r_ch];
ch_str_flag = get_bit( font_tbl[cur_fnt].ch_str, r_ch);
chr = ((r_ch == 255) && font_tbl[cur_fnt].ch255 )? 256 :
                         *(font_tbl[cur_fnt].ch + r_ch);
if( (gif_flag % 2) || ch_str_flag ){      design_ch = ch;
             { 
#line 10701 "./tex4ht-c.tex"

      U_CHAR  str[256], *p;
      BOOL sv;
      int mag;
sv = special_on;   special_on = TRUE;
if( gif_ch && (gif_flag % 2) ){
   
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


   if( !gif_open[gif_flag] ){
     
#line 10778 "./tex4ht-c.tex"

(IGNORED) sprintf(str,
   "configuration for htf class %d (char %d of %s.htf)",
   gif_flag, ch,font_tbl[cur_fnt].name
  );
warn_i_str(50,str);


#line 10786 "./tex4ht-c.tex"

gif_open[gif_flag] = m_alloc(char,
   
#line 10811 "./tex4ht-c.tex"

29

);
(IGNORED) strcpy((char *) gif_open[gif_flag],
           
#line 10807 "./tex4ht-c.tex"

"<img src=\"+\" alt=\"+++++\" />+"

);
gif_alt[gif_flag] = gif_open[gif_flag]+11;
  *(gif_alt[gif_flag] - 1) = '\0';
gif_class[gif_flag] = gif_open[gif_flag]+19;
  *(gif_class[gif_flag] - 1) = '\0';
gif_size[gif_flag] = gif_open[gif_flag]+20;
  *(gif_size[gif_flag] - 1) = '\0';
gif_mag[gif_flag] = gif_open[gif_flag]+21;
  *(gif_mag[gif_flag] - 1) = '\0';
gif_ord[gif_flag] = gif_open[gif_flag]+22;
  *(gif_ord[gif_flag] - 1) = '\0';
gif_end[gif_flag] = gif_open[gif_flag]+23;
  *(gif_end[gif_flag] - 1) = '\0';
gif_id[gif_flag] = gif_open[gif_flag]+28;
  *(gif_id[gif_flag] - 1) = '\0';


   } else if( !get_bit( class_on, gif_flag ) ) {
      notify_class_info(gif_flag);
      store_bit_I( class_on, gif_flag );
   }
   
#line 11467 "./tex4ht-c.tex"

p= gif_open[gif_flag];
if( p )
if( *p ){
   print_f(p); (IGNORED) strcpy((char *) str, (char *) font_tbl[cur_fnt].name);
   mag = (int) ((double) font_tbl[cur_fnt].scale /
                font_tbl[cur_fnt].design_sz  * 10 );
   
#line 10893 "./tex4ht-c.tex"

if( !dos_file_names ){
   print_f(font_tbl[cur_fnt].name);
   if( mag == 10 )  (IGNORED) sprintf(str, GIF_I,   design_ch, gif);
   else             (IGNORED) sprintf(str, GIF_II,  mag, design_ch, gif);
}


#line 10914 "./tex4ht-c.tex"


if( dos_file_names ){
   dos_gif_file(str, mag, design_ch);
   print_f(str);
   (IGNORED) sprintf(str, GIF_VII, gif);
}



   print_f(str);
   add_bit( font_tbl[cur_fnt].gif_on, r_ch, 1 );
}


   
#line 11500 "./tex4ht-c.tex"

p = gif_alt[gif_flag];
if( p )
  if( *p ){
     print_f(p);  put_alt_ch(chr,ch_str_flag);
}


   
#line 11514 "./tex4ht-c.tex"

p = gif_class[gif_flag];
if( p )
  if( *p ){
    (IGNORED) fprintf(cur_o_file, p,
                   font_tbl[cur_fnt].family_name);  }


   
#line 11538 "./tex4ht-c.tex"

p = gif_size[gif_flag];
if( p )
  if( *p ){
     (IGNORED) fprintf(cur_o_file, p,
                   font_tbl[cur_fnt].font_size);  }


   
#line 11555 "./tex4ht-c.tex"

p = gif_mag[gif_flag];
if( p )
  if( *p && (font_tbl[cur_fnt].mag != 100) ){
     (IGNORED) fprintf(cur_o_file, p, font_tbl[cur_fnt].mag);
}


   
#line 11570 "./tex4ht-c.tex"

p = gif_ord[gif_flag];
if( p )
  if( *p ){
    (IGNORED) fprintf(cur_o_file, p, ch);
}


   
#line 11587 "./tex4ht-c.tex"

p = gif_end[gif_flag];
if( p )
  if( *p ){ print_f( p ); }


} else  { 
#line 10830 "./tex4ht-c.tex"

if( !gif_flag || (gif_flag % 2)  || ch_map_flag ) {
   put_alt_ch(chr,ch_str_flag);  }
else{ 
#line 11723 "./tex4ht-c.tex"


#line 11739 "./tex4ht-c.tex"

if( gif_flag && !get_bit( class_on, gif_flag ) ) {
  notify_class_info(gif_flag);
  store_bit_I( class_on, gif_flag );
}


if( span_on ){
   
#line 11749 "./tex4ht-c.tex"

if( span_open[gif_flag] )
  if( *span_open[gif_flag] ){
     print_f( span_open[gif_flag] );
}


   
#line 11773 "./tex4ht-c.tex"

if( span_name[gif_flag] )
  if( *span_name[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_name[gif_flag],
                        font_tbl[cur_fnt].family_name);
}


   
#line 11783 "./tex4ht-c.tex"

if( span_size[gif_flag] )
  if( *span_size[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_size[gif_flag],
                        font_tbl[cur_fnt].font_size);
}


   
#line 11792 "./tex4ht-c.tex"

if( span_mag[gif_flag] )
  if( *span_mag[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_mag[gif_flag],
                        font_tbl[cur_fnt].mag);
}


   
#line 11801 "./tex4ht-c.tex"

if( span_ord[gif_flag] )
  if( *span_ord[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_ord[gif_flag], chr);
}


   
#line 11765 "./tex4ht-c.tex"

if( span_ch[gif_flag] )
  if( *span_ch[gif_flag] ){
    print_f( span_ch[gif_flag] );
}


}
put_alt_ch(chr,ch_str_flag);
if( span_on ){
   
#line 11756 "./tex4ht-c.tex"

if( end_span[gif_flag] )
  if( *end_span[gif_flag] ){
     print_f( end_span[gif_flag] );
}


}

 }

 }
special_on = sv;

 } design_ch = 0;    }
else { 
#line 10821 "./tex4ht-c.tex"

if( !gif_flag || (gif_flag % 2) || ch_map_flag ) {  put_char(chr);
} else{ 
#line 11704 "./tex4ht-c.tex"


#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }



#line 11739 "./tex4ht-c.tex"

if( gif_flag && !get_bit( class_on, gif_flag ) ) {
  notify_class_info(gif_flag);
  store_bit_I( class_on, gif_flag );
}


if( span_on ){
   
#line 11749 "./tex4ht-c.tex"

if( span_open[gif_flag] )
  if( *span_open[gif_flag] ){
     print_f( span_open[gif_flag] );
}


   
#line 11773 "./tex4ht-c.tex"

if( span_name[gif_flag] )
  if( *span_name[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_name[gif_flag],
                        font_tbl[cur_fnt].family_name);
}


   
#line 11783 "./tex4ht-c.tex"

if( span_size[gif_flag] )
  if( *span_size[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_size[gif_flag],
                        font_tbl[cur_fnt].font_size);
}


   
#line 11792 "./tex4ht-c.tex"

if( span_mag[gif_flag] )
  if( *span_mag[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_mag[gif_flag],
                        font_tbl[cur_fnt].mag);
}


   
#line 11801 "./tex4ht-c.tex"

if( span_ord[gif_flag] )
  if( *span_ord[gif_flag] ){
    (IGNORED) fprintf(cur_o_file, span_ord[gif_flag], chr);
}


   
#line 11765 "./tex4ht-c.tex"

if( span_ch[gif_flag] )
  if( *span_ch[gif_flag] ){
    print_f( span_ch[gif_flag] );
}


}
put_char(chr);
if( span_on ){
   
#line 11756 "./tex4ht-c.tex"

if( end_span[gif_flag] )
  if( *end_span[gif_flag] ){
     print_f( end_span[gif_flag] );
}


}

 }

 }

 }
}

#line 12405 "./tex4ht-c.tex"

if( a_accent_template && needs_accented_sym ){
   (IGNORED) fprintf(cur_o_file, "%s", a_accent_fifth);
}



#line 8213 "./tex4ht-c.tex"

if( pos_dvi ){
   print_f(end_pos_text);
}

 
#line 12947 "./tex4ht-c.tex"

if( show_class && !pause_class && !ignore_subclass_del ){
   if( !stack[stack_n].no_left_del && stack[stack_n+1].active_class_del ){
      if( !stack[stack_n+1].ignore_subclass_del ){
         
#line 12962 "./tex4ht-c.tex"

(IGNORED) print_f( close_class[math_class]);


      }
      
#line 12975 "./tex4ht-c.tex"

(IGNORED) print_f( (stack[stack_n+1].temp_class_del)?
                      stack[stack_n+1].temp_class_close
                   :  stack[stack_n+1].class_close);


      stack[stack_n+1].active_class_del = FALSE;
   } else {
      
#line 12962 "./tex4ht-c.tex"

(IGNORED) print_f( close_class[math_class]);


      stack[stack_n+1].active_class_del = FALSE;  
   }
}



#line 2913 "./tex4ht-c.tex"

if( trace_dvi_C && !ch_map_flag ){
   
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


   (IGNORED) fprintf(cur_o_file, "%s%s",
         trace_dvi_del_c, end_trace_dvi_del_c);
}

 
#line 2619 "./tex4ht-c.tex"





#line 12307 "./tex4ht-c.tex"


#line 12281 "./tex4ht-c.tex"

if( needs_end_accent && t_accent_template ){
   
#line 2609 "./tex4ht-c.tex"

if( span_on && in_span_ch ){
   if( *end_span[0] ){
       in_span_ch = FALSE;
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       (IGNORED) fprintf(cur_o_file, "%s", end_span[0]);
}  }


   (IGNORED) fprintf(cur_o_file, "%s", t_accent_fifth);
   needs_end_accent = FALSE; 
#line 12424 "./tex4ht-c.tex"

needs_accented_sym--;


}






        text_on = TRUE;
   } }
   
#line 9800 "./tex4ht-c.tex"

return (INTEGER)(
    
#line 9837 "./tex4ht-c.tex"

design_size_to_pt( *(font_tbl[cur_fnt].wtbl
                     +  (int) (
#line 9845 "./tex4ht-c.tex"

*(font_tbl[cur_fnt].char_wi +  (int)
   ( ch - font_tbl[cur_fnt].char_f)% 256)

) )
                 )
* (double) font_tbl[cur_fnt].scale


  );


}


#line 16615 "./tex4ht-c.tex"


static void put_char
#ifdef ANSI
#define SEP ,
(   int ch
)
#undef SEP
#else
#define SEP ;
( ch )   int ch
;
#undef SEP
#endif
{
  if(    !ignore_chs
      && !( ((ch==' ') || (ch=='\n')) && no_root_file )
 ){
     
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


     if( ch_map_flag ){
        if( special_on || ((ch != '\n') && (ch != ' ')) ){
            
#line 4990 "./tex4ht-c.tex"

insert_ch_map((char) ch, TRUE);

 }
     }else {
        if ( ch == '\n' ){ 
#line 3409 "./tex4ht-c.tex"

if( eoln_str ){ print_f(eoln_str); }
else { (IGNORED) put_4ht_ch( ch, cur_o_file ); }
recover_spaces = 0;

 }
        else if ( ch == ' ' ){ 
#line 3431 "./tex4ht-c.tex"

if( space_str ){ print_f(space_str); }
else { (IGNORED) put_4ht_ch( ch, cur_o_file ); }

 }
        else { (IGNORED) put_4ht_ch( ch, cur_o_file ); }
} }  }


#line 16648 "./tex4ht-c.tex"


static void print_f
#ifdef ANSI
#define SEP ,
(    const char* str
)
#undef SEP
#else
#define SEP ;
(str)    const char* str
;
#undef SEP
#endif
{
  
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


  if( ch_map_flag ){
     while( *str ){ put_char( *str );  str++; }
  }else {
    (IGNORED) print_f_4ht( str );
} }


#line 16668 "./tex4ht-c.tex"


static void print_f_4ht
#ifdef ANSI
#define SEP ,
(    const char* str
)
#undef SEP
#else
#define SEP ;
(str)    const char* str
;
#undef SEP
#endif
{
  
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


  if( ch_map_flag ){
     while( *str ){ put_char( *str );  str++; }
  } else {
     while( *str ){
        (IGNORED) put_4ht_ch( *str, cur_o_file );
        str++;
} } }


#line 16693 "./tex4ht-c.tex"

static int get_char(MYVOID)
{
   return  (int) getc(dvi_file);
}


#line 16705 "./tex4ht-c.tex"

static int get_noop(MYVOID)
{      int ch;
  while(  (ch = get_char())  ==  
#line 17317 "./tex4ht-c.tex"

138 
 ){;}
  return ch;
}


#line 16721 "./tex4ht-c.tex"


static char* get_str
#ifdef ANSI
#define SEP ,
(      int n
)
#undef SEP
#else
#define SEP ;
(n)      int n
;
#undef SEP
#endif
{                 U_CHAR *q, *p;
   p = q = m_alloc(char,n+1);
   while( n-- ) *q++ = get_char();
   *q = '\0';
   return p;
}


#line 16745 "./tex4ht-c.tex"


static long fget_unt
#ifdef ANSI
#define SEP ,
(
    FILE*     file SEP 
    register int  n

)
#undef SEP
#else
#define SEP ;
( file, n )
    FILE*     file SEP 
    register int  n

;
#undef SEP
#endif
{  register long val = 0;
  while( n-- ){ val = (val << 8) + (unsigned INTEGER) getc(file) ;  }
  return val;
}


#line 16769 "./tex4ht-c.tex"


static long fget_int
#ifdef ANSI
#define SEP ,
(
    FILE *file SEP 
    int   n

)
#undef SEP
#else
#define SEP ;
( file, n )
    FILE *file SEP 
    int   n

;
#undef SEP
#endif
{  register long val;
  val = (unsigned INTEGER) getc(file);
  if( val & 0x80 )     val -= 0x100;
  while( --n ){ val = (val << 8) + (unsigned INTEGER) getc(file); }
  return val;
}


#line 16805 "./tex4ht-c.tex"


static long cond_int
#ifdef ANSI
#define SEP ,
(
    register INTEGER  n
)
#undef SEP
#else
#define SEP ;
( n )
    register INTEGER  n
;
#undef SEP
#endif
{  register long val;
    int  ch;
  val = (unsigned int) (ch = get_char());
  cond_idv_char( ch );
  if( val & 0x80 )     val -= 0x100;
  while( --n ){
    val = (val << 8) + (unsigned int) (ch = get_char());
    cond_idv_char( ch );
  }
  return val;
}


#line 16912 "./tex4ht-c.tex"


static  void warn_i
#ifdef ANSI
#define SEP ,
(     int  n
)
#undef SEP
#else
#define SEP ;
(n)     int  n
;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- warning --- ");
   (IGNORED) fprintf(stderr, "%s", warn_err_mssg[n]);
   show_err_context();
}


#line 16936 "./tex4ht-c.tex"


static void warn_i_int
#ifdef ANSI
#define SEP ,
(   int  n SEP  int i

)
#undef SEP
#else
#define SEP ;
(n,i)   int  n SEP  int i

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- warning --- ");
   (IGNORED) fprintf(stderr, warn_err_mssg[n], i);
   show_err_context();
}


#line 16952 "./tex4ht-c.tex"


static void warn_i_int_2
#ifdef ANSI
#define SEP ,
(   int  n SEP  int i SEP  int j

)
#undef SEP
#else
#define SEP ;
(n,i,j)   int  n SEP  int i SEP  int j

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- warning --- ");
   (IGNORED) fprintf(stderr, warn_err_mssg[n], i, j);
   show_err_context();
}


#line 16968 "./tex4ht-c.tex"


static void warn_i_str
#ifdef ANSI
#define SEP ,
(
    int  n SEP 
    const char *str

)
#undef SEP
#else
#define SEP ;
(n,str)
    int  n SEP 
    const char *str

;
#undef SEP
#endif
{
   (IGNORED) fprintf(stderr,"--- warning --- ");
   (IGNORED) fprintf(stderr,warn_err_mssg[n], str);
   show_err_context();
}


#line 16988 "./tex4ht-c.tex"


static void warn_i_str2
#ifdef ANSI
#define SEP ,
(
    int  n SEP 
    const char *str1 SEP 
    const char *str2

)
#undef SEP
#else
#define SEP ;
(n,str1,str2)
    int  n SEP 
    const char *str1 SEP 
    const char *str2

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- warning --- ");
   (IGNORED) fprintf(stderr,warn_err_mssg[n], str1,str2);
   show_err_context();
}


#line 17022 "./tex4ht-c.tex"


static void err_i
#ifdef ANSI
#define SEP ,
(      int  n

)
#undef SEP
#else
#define SEP ;
(n)      int  n

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- error --- ");
   (IGNORED) fprintf(stderr, "%s", warn_err_mssg[n]);
   show_err_context();
   exit(EXIT_FAILURE);
}


#line 17039 "./tex4ht-c.tex"


static void err_i_int
#ifdef ANSI
#define SEP ,
(     int  n SEP   int i

)
#undef SEP
#else
#define SEP ;
(n,i)     int  n SEP   int i

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- error --- ");
   (IGNORED) fprintf(stderr, warn_err_mssg[n], i);
   show_err_context();
   exit(EXIT_FAILURE);
}


#line 17055 "./tex4ht-c.tex"


static void err_i_str
#ifdef ANSI
#define SEP ,
(
     int  n SEP 
     U_CHAR *str

)
#undef SEP
#else
#define SEP ;
(n,str)
     int  n SEP 
     U_CHAR *str

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- error --- ");
   (IGNORED) fprintf(stderr, warn_err_mssg[n], str);
   show_err_context();
   exit(EXIT_FAILURE);
}


#line 17102 "./tex4ht-c.tex"

static void show_err_context(MYVOID)
{                             long  curr_pos;
                              int n, i;
                              U_CHAR ch;
   if( err_context ){
     curr_pos = ftell(dvi_file);
     for(n=6; n--;){
        (IGNORED) putc( '\n', stderr );
        for(i=70; i--;){
          ch = get_char();
          (IGNORED) putc(( (ch>31) && (ch<127))? ch : ' ', stderr);
        }
     }
     (IGNORED) fseek(dvi_file, curr_pos, 
#line 2174 "./tex4ht-c.tex"

0
);
     (IGNORED) putc( '\n', stderr ); (IGNORED) putc( '\n', stderr );
     if( err_mark ){  print_f( err_mark ); }
}  }



int 
#line 507 "./tex4ht-c.tex"

#ifdef CDECL
CDECL
#endif

 main
#ifdef ANSI
#define SEP ,
(
       int  argc SEP 
       U_CHAR **argv
)
#undef SEP
#else
#define SEP ;
(argc, argv)
       int  argc SEP 
       U_CHAR **argv
;
#undef SEP
#endif
{  
#line 1385 "./tex4ht-c.tex"

register INTEGER  i;
int  ch;


#line 2208 "./tex4ht-c.tex"

int unread_pages;


#line 5400 "./tex4ht-c.tex"

long int eof_op_n, begin_postamble;
int dis_pages;


#line 6604 "./tex4ht-c.tex"

int stack_id=0;


#line 11858 "./tex4ht-c.tex"

static struct css_ext_rec * css_ext = (struct css_ext_rec *) 0;
static char css_default[] = "4cs";


#line 12259 "./tex4ht-c.tex"

BOOL in_accenting;


#line 13034 "./tex4ht-c.tex"

char* tex4ht_env_file = (char *) 0;
char* dos_env_file =
#if defined(__MSDOS__)
  
#line 13221 "./tex4ht-c.tex"

get_env_dir(argv[0])

;
#endif
#if !defined(__MSDOS__)
  (char *) 0;
#endif



#line 13859 "./tex4ht-c.tex"

struct htf_com_rec* htf_font_dir = (struct htf_com_rec *) 0;


   
#line 1064 "./tex4ht-c.tex"

#ifdef WIN32
  /* The idea here is to split options apart at spaces: a single argument
     "-foo -bar" becomes the two options "-foo" and "-bar".  We need to
     do this for Windows because mk4ht passes this sort of combined
     option in one string to scripts like htlatex.{unix,bat}.  In the
     Unix case, the shell resplits words when calling t4ht and tex4ht,
     so the program see two options.  But this does not happen with the
     .bat; %4, for instance, remains "-foo -bar".  So we fix it here.  */
  if (argc > 2) {
    int  i, nargc;
    char **nargv, **pnargv, **pargv;

    nargv = (char **) xmalloc (2 * argc * sizeof (char *));
    pnargv = nargv;
    pargv = argv;
    *pnargv++ = xstrdup (*pargv++);
    *pnargv++ = xstrdup (*pargv++);
    nargc = 2;

    for (i=2; i < argc; i++) {
      char *p, *q, *r;
      p = q = *pargv++;
      while (*p == ' ' || *p == '\t') {
        p++;
        q++;
      }
      while (*p != ' ' && *p != '\t' && *p) {
        p++;
        if (*p == '\0') {
          *pnargv++ = xstrdup(q);
          nargc++;
        } else if (*p == ' ' || *p == '\t') {
          r = p;
          while (*p == ' ' || *p == '\t')
            p++;
          if (*p == '-' || *p == '\0') {
            *r = '\0';
            *pnargv++ = xstrdup(q);
            nargc++;
            q = p;
          }
        }
      }
    }

    nargv[nargc] = NULL;
    argv = nargv;
    argc = nargc;
  }
#endif


   
#line 1159 "./tex4ht-c.tex"


#ifdef SIGSEGV
  (void) signal(SIGSEGV,sig_err);
#endif
  (void) signal(SIGFPE,sig_err);
#ifdef WIN32
  
#line 577 "./tex4ht-c.tex"

SetConsoleCtrlHandler((PHANDLER_ROUTINE)sigint_handler, TRUE);


#else
#ifdef SIGINT
  (void) signal(SIGINT,sig_err);    
#endif
#endif


   
#line 1336 "./tex4ht-c.tex"

(IGNORED) printf("----------------------------\n");
#ifndef KPATHSEA
#ifdef PLATFORM
   (IGNORED) printf("tex4ht.c (2018-07-03-10:36 %s)\n",PLATFORM);
#else
   (IGNORED) printf("tex4ht.c (2018-07-03-10:36)\n");
#endif
#else
#ifdef PLATFORM
   (IGNORED) printf("tex4ht.c (2018-07-03-10:36 %s kpathsea)\n",PLATFORM);
#else
   (IGNORED) printf("tex4ht.c (2018-07-03-10:36 kpathsea)\n");
#endif
#endif
for(i=0; i<argc; i++){
    (IGNORED) printf("%s%s ", (i>1)?"\n  " : "", argv[i]); }
(IGNORED) printf("\n");


   
#line 2781 "./tex4ht-c.tex"

set_del( &trace_dvi_del_P, &end_trace_dvi_del_P);
set_del( &trace_dvi_del_p, &end_trace_dvi_del_p);
set_del( &trace_dvi_del_C, &end_trace_dvi_del_C);
set_del( &trace_dvi_del_c, &end_trace_dvi_del_c);
set_del( &trace_dvi_del_H, &end_trace_dvi_del_H);
set_del( &trace_dvi_del_h, &end_trace_dvi_del_h);
set_del( &trace_dvi_del_R, &end_trace_dvi_del_R);
set_del( &trace_dvi_del_r, &end_trace_dvi_del_r);
set_del( &trace_dvi_del_V, &end_trace_dvi_del_V);
set_del( &trace_dvi_del_v, &end_trace_dvi_del_v);


   
#line 1576 "./tex4ht-c.tex"

dvi_file = stdin;


#line 4682 "./tex4ht-c.tex"

{                   U_CHAR   *yes = NULL;
  system_yes =  (system( yes ) != 0);
}


#line 6300 "./tex4ht-c.tex"

{                                         int i;
  for( i=8; i--; ){
    halign[i]         = m_alloc(struct halign_rec, 1);
    halign[i]->str    = m_alloc(char, 1);
    *(halign[i]->str) = '\0';
    halign[i]->refs   = 1;
  }
}


#line 7439 "./tex4ht-c.tex"

del_stack = (struct del_stack_entry  *) 0;


#line 7464 "./tex4ht-c.tex"

back_id_off = 1;  id_latex = 0;


#line 7543 "./tex4ht-c.tex"

back_token = back_group = m_alloc(struct send_back_entry,1);
back_token->id = -1;


#line 8460 "./tex4ht-c.tex"

pos_text = pos_line = end_pos_body = end_pos_text = pos_body =
                      m_alloc(char, (int) 1);
(IGNORED) strcpy((char *) pos_text, "" );


#line 9748 "./tex4ht-c.tex"

margin_sp = (double) MARGINSP;     


#line 11662 "./tex4ht-c.tex"

{   int i;  i=256; while( i-- ) {
     span_name[i] = span_open[i] = span_size[i] =
     span_mag[i]  = span_ch[i]   = end_span[i]  =
     span_ord[i]  = gif_id[i] = NULL;
       if( (i>0) && !(i%2) ) {  store_bit_Z( class_on, i ); }
       else {   store_bit_I( class_on, i ); }
    }
}


#line 12263 "./tex4ht-c.tex"

in_accenting = FALSE;


#line 12753 "./tex4ht-c.tex"

for( math_class=0; math_class<
#line 12719 "./tex4ht-c.tex"

79

; math_class++ ){
  open_class[math_class] = m_alloc(char, 1);
  close_class[math_class] = m_alloc(char, 1);
  *(open_class[math_class]) = *(close_class[math_class]) = '\0';
}


#line 14207 "./tex4ht-c.tex"

HOME_DIR = getenv("HOME");


   
#line 1360 "./tex4ht-c.tex"

{      long   file_len;
  
#line 1443 "./tex4ht-c.tex"

{      int i;
       U_CHAR *p;
       const U_CHAR *in_name = "", *out_name = "";
  
#line 14878 "./tex4ht-c.tex"


#ifdef KPATHSEA
   kpse_set_program_name (argv[0], "tex4ht");
#endif


  
#line 642 "./tex4ht-c.tex"

#if !defined(DOS_GIF_FILE) && !defined(WIN32) && defined(__DJGPP__)
   dos_file_names = !_use_lfn(NULL);
#endif


  if( argc == 1 ){ bad_arg; }
  for(i=1; i<argc; i++) {
    if( *( p=argv[i] ) == '-' ){ 
#line 1468 "./tex4ht-c.tex"

if( (int) strlen((char *)  argv[i] ) == 2 ){
   if( ++i == argc ) bad_arg;
}
switch( *(p+1) ){
  case 'b':{ 
#line 11115 "./tex4ht-c.tex"

begin_char_gif = p+2;

     break; }
  case 'c':{ 
#line 13427 "./tex4ht-c.tex"

struct env_c_rec *temp = (struct env_c_rec*)
                   m_alloc(struct env_c_rec, (int) 1);
temp->option = p+2;
temp->next = envChoice;
envChoice = temp;

  break;}
  case 'e':{ 
#line 13025 "./tex4ht-c.tex"

tex4ht_env_file = p+2;

break; }
  case 'f':{ 
#line 1503 "./tex4ht-c.tex"

p = p + 2;
in_name = p + (int) strlen((char *)  p );
while( *in_name != *p ){ in_name--; }
in_name++;

break; }
  case 'F':{ 
#line 9963 "./tex4ht-c.tex"

char *digit = p+2;
ignore_ch = 0;
while( *digit != '\0' ){
  if( (*digit < '0') || (*digit > '9') ){
    ignore_ch = 0; break;
  }
  ignore_ch = 10 * ignore_ch + (*digit - '0');
  if( ignore_ch > 255 ){ ignore_ch = 0; break; }
  digit++;
}

             break; }
  case 'g':{ 
#line 11143 "./tex4ht-c.tex"

gif = p+2;

      break; }
  case 'h':{ 
#line 1513 "./tex4ht-c.tex"

{
  char trace = *(p+2);
  if (trace == 'A' || trace == 'e') { 
#line 17136 "./tex4ht-c.tex"

err_context = TRUE;

 }
  if (trace == 'A' || trace == 'f') { 
#line 10491 "./tex4ht-c.tex"

dump_htf_files = 1;

 }
  if (trace == 'A' || trace == 'F') { 
#line 15066 "./tex4ht-c.tex"

dump_htf_search = TRUE;

 }
  if (trace == 'A' || trace == 's') { 
#line 17165 "./tex4ht-c.tex"

trace_special = TRUE;

 }
  if (trace == 'A' || trace == 'g') { 
#line 2753 "./tex4ht-c.tex"

trace_dvi_P++;
if( !(   *trace_dvi_del_P || *end_trace_dvi_del_P
      || *trace_dvi_del_p || *end_trace_dvi_del_p
     )
 ){
   trace_dvi_del_P =
            (char *)  r_alloc((void *) trace_dvi_del_P,
                              (size_t) 4);
   (IGNORED) strcpy((char *) trace_dvi_del_P, "[G " );
   end_trace_dvi_del_P =
            (char *)  r_alloc((void *) end_trace_dvi_del_P,
                              (size_t) 2);
   (IGNORED) strcpy((char *) end_trace_dvi_del_P, "]" );
   trace_dvi_del_p =
            (char *)  r_alloc((void *) trace_dvi_del_p,
                              (size_t) 5);
   (IGNORED) strcpy((char *) trace_dvi_del_p, "[/G " );
   end_trace_dvi_del_p =
            (char *)  r_alloc((void *) end_trace_dvi_del_p,
                              (size_t) 2);
   (IGNORED) strcpy((char *) end_trace_dvi_del_p, "]" );
}

 }
  if (trace == 'A' || trace == 'v') { 
#line 10495 "./tex4ht-c.tex"

dump_env_files = TRUE;

 }
  if (trace == 'A' || trace == 'V') { 
#line 13062 "./tex4ht-c.tex"

dump_env_search = TRUE;

 }
  else { bad_arg; }
}

  break; }
  case 'i':{ 
#line 13863 "./tex4ht-c.tex"

   com_dir(p);
   {
                         struct htf_com_rec *q, *t;
   q = m_alloc( struct htf_com_rec, 1);
   q->name = p+2;
   q->next = (struct htf_com_rec *) 0;
   if(  htf_font_dir ){
     t = htf_font_dir;
     while( t->next ){ t = t->next; }
     t->next = q;
   } else {
     htf_font_dir = q;
   }
}

  break; }
  case 'l':{ 
#line 14214 "./tex4ht-c.tex"

#ifndef KPATHSEA
tex4ht_fls_name = p+2;
#endif

  break; }
  case 'P':{ 
#line 4644 "./tex4ht-c.tex"

{     struct sys_call_rec *q;
  q = m_alloc(struct sys_call_rec, 1);
  q->next = system_calls;
  q->filter = p + 2;
  system_calls = q;
}

  break; }
  case 'S':{ 
#line 11077 "./tex4ht-c.tex"

font_gif = p+2;

    break; }
  case 's':{ 
#line 11842 "./tex4ht-c.tex"

struct css_ext_rec * css =  m_alloc(struct css_ext_rec, 1);;
css->name = p + 2;
css->next = css_ext;
css_ext = css;

    break; }
  case 't':{ 
#line 13481 "./tex4ht-c.tex"

com_dir(p);  fontdir[fontdir_count++] = p+2;

  break; }
  case 'u':{ 
#line 16219 "./tex4ht-c.tex"

if( eq_str(p+2, "10") ){ u10 = TRUE; }

#line 16362 "./tex4ht-c.tex"

else if( eq_str(p+2, "tf8") ){ utf8 = TRUE; }


else{ bad_arg;}

  break; }
  case 'v':{ 
#line 5469 "./tex4ht-c.tex"

{          U_CHAR *q;
   q = p + 2;
   id_version = 0;
   while( *q != '\0' ){
     if( id_version != -1 ){
        if( (*q < '0') || (*q > '9') ){
           id_version = -1;
           warn_i(53);
        }
        id_version =  id_version * 10 + *q - '0';
     }
     q++;
}  }

  break; }
  case 'x':{ 
#line 649 "./tex4ht-c.tex"

switch( *(p+2) ){
  case 's':{   dos_file_names = TRUE;  break; }
   default:{ bad_arg; }
}

  break; }
  case '.':{ 
#line 1634 "./tex4ht-c.tex"

ext = p+1;

  break; }
   default:{ bad_arg; }
}

 }
    else in_name = argv[i];
  }
  
#line 11849 "./tex4ht-c.tex"

if( css_ext == (struct css_ext_rec *) 0 ){
  struct css_ext_rec * css =  m_alloc(struct css_ext_rec, 1);;
  css->name = css_default;
  css->next = css_ext;
  css_ext = css;
}


  
#line 1588 "./tex4ht-c.tex"

if( *in_name != '\0' ){ 
#line 1596 "./tex4ht-c.tex"

      BOOL tag;
job_name_n = (int) strlen( in_name );
job_name = m_alloc(char, job_name_n+6);
(IGNORED) strcpy((char *) job_name, in_name);
tag = job_name_n < 3;
if( !tag ){
   tag = !eq_str( job_name+job_name_n-
#line 1614 "./tex4ht-c.tex"

(
  (ext==NULL)? 4 : (int) strlen((char *) ext)
)

,
#line 1620 "./tex4ht-c.tex"

(
  (ext==NULL)? ".dvi" : ext
)

);
}
if( tag ){
   job_name_n+=
#line 1614 "./tex4ht-c.tex"

(
  (ext==NULL)? 4 : (int) strlen((char *) ext)
)

; (IGNORED) strct(job_name, 
#line 1620 "./tex4ht-c.tex"

(
  (ext==NULL)? ".dvi" : ext
)

);
}
if( (dvi_file = fopen(job_name, READ_BIN_FLAGS)) == NULL )
   { 
#line 1655 "./tex4ht-c.tex"

{                             int i;
   for(i=job_name_n-5; i; i--){
     if( job_name[i] == '.' ){
       job_name[i] = '\0';
       job_name_n = i + 
#line 1614 "./tex4ht-c.tex"

(
  (ext==NULL)? 4 : (int) strlen((char *) ext)
)

;
       (IGNORED) strct(job_name, 
#line 1620 "./tex4ht-c.tex"

(
  (ext==NULL)? ".dvi" : ext
)

);
       break;
   } }
   if( (dvi_file = fopen(job_name, READ_BIN_FLAGS)) == NULL ){
      warn_i_str(1, job_name); bad_in_file(job_name);
}  }

 }

#line 5390 "./tex4ht-c.tex"

job_name[job_name_n-1] = 'v';
job_name[job_name_n-2] = 'd';
job_name[job_name_n-3] = 'i';
if( (idv_file = fopen(job_name, WRITE_BIN_FLAGS)) == NULL )
   bad_in_file(job_name);




 }
#ifdef KWIN32
   else if (!isatty(fileno(stdin))) SET_BINARY(fileno(stdin));
#endif


  { 
#line 1700 "./tex4ht-c.tex"

   U_CHAR *name=0;
if( *out_name == '\0' )
  { if( *in_name == '\0' ){ 
#line 1785 "./tex4ht-c.tex"

bad_arg;

 }
    else                  { 
#line 1790 "./tex4ht-c.tex"

int n = (int) strlen((char *)  job_name );
name = m_alloc(char, 6 + n);
(IGNORED) strcpy((char *) name, (char *) job_name);
n -= 4; *(name + n) = '\0';
(IGNORED) strct(name,".html");
#ifdef HTM
name[n+4] ='\0';
#endif

 }
  }
else{ 
#line 1808 "./tex4ht-c.tex"

   int tag = 1;
   int n = (int) strlen( out_name );
name = m_alloc(char, 6 + n);
(IGNORED) strcpy((char *) name, out_name);
while( n-- )   tag = tag && (*(name+n) != '.') ;
if( tag ) (IGNORED) strct(name,".html");
#ifdef HTM
name[n+4] = '\0';
#endif

 }
no_root_file = name;

 }
}


  
#line 13066 "./tex4ht-c.tex"

{                              U_CHAR  str[PATH_MAX],  *TEX4HTENV;
   
#line 13084 "./tex4ht-c.tex"

if( dump_env_search && tex4ht_env_file ){
   (IGNORED) printf("-e: %s?\n", tex4ht_env_file);
}
dot_file = tex4ht_env_file?
   f_open_pathed_filename( tex4ht_env_file, READ_TEXT_FLAGS ) : NULL;


   
#line 13129 "./tex4ht-c.tex"

if( !dot_file ){
   if( dump_env_search ){ (IGNORED) printf("%s?\n", "tex4ht.env"); }
   dot_file = f_open("tex4ht.env", READ_TEXT_FLAGS);
}
#ifndef DOS_WIN32
   if( !dot_file ){
      if( dump_env_search ){ (IGNORED) printf("%s?\n", ".tex4ht"); }
      dot_file = f_open(".tex4ht", READ_TEXT_FLAGS);
      if( dot_file ){
         printf("(%s)\n", ".tex4ht");
   }  }
#endif


   if( !dot_file ){
     
#line 13158 "./tex4ht-c.tex"

TEX4HTENV = getenv("TEX4HTENV");
if( TEX4HTENV ){
   if( dump_env_search ){
        (IGNORED) printf("TEX4HTENV: %s?\n", TEX4HTENV); }
   dot_file = f_open_pathed_filename(TEX4HTENV,READ_TEXT_FLAGS);
} else {
   if( dump_env_search ){
        (IGNORED) printf("getenv(\"TEX4HTENV\")=\n");
}  }


   }
   
#line 13096 "./tex4ht-c.tex"

if( !dot_file ){
   if( HOME_DIR ){
      (IGNORED) sprintf(str,
#line 13177 "./tex4ht-c.tex"

#if defined(__DJGPP__)
  is_forward_slash(HOME_DIR)?  "%s/tex4ht.env" :  "%s\\tex4ht.env"
#else
  "%s/tex4ht.env"
#endif

, HOME_DIR);
      if( dump_env_search ){ (IGNORED) printf("%s?\n", str); }
      dot_file = f_open(str,READ_TEXT_FLAGS);
}  }
#ifndef DOS_WIN32
   if( !dot_file ){
     if( HOME_DIR ){
        (IGNORED) sprintf(str,"%s/.tex4ht", HOME_DIR);
        if( dump_env_search ){ (IGNORED) printf("%s?\n", str); }
        dot_file = f_open(str,READ_TEXT_FLAGS);
   } }
#endif
#if defined(__MSDOS__)
   if( !dot_file ){
      if( dump_env_search ){ (IGNORED) printf("%s?\n", "C:/tex4ht.env"); }
      dot_file = f_open("C:/tex4ht.env",READ_TEXT_FLAGS);
   }
#endif


   
#line 13145 "./tex4ht-c.tex"

#ifdef ENVFILE
   if( !dot_file ){
      if( dump_env_search ){ (IGNORED) printf("ENVFILE: %s?\n", ENVFILE); }
      dot_file = f_open_pathed_filename( ENVFILE,READ_TEXT_FLAGS);
   }
#else
   if( dump_env_search ){
        (IGNORED) printf("tex4ht compiled without ENVFILE\n");
   }
#endif


   
#line 13122 "./tex4ht-c.tex"

if( !dot_file && dos_env_file){
   if( dump_env_search ){ (IGNORED) printf("%s?\n", dos_env_file); }
   dot_file = f_open( dos_env_file, READ_TEXT_FLAGS );
}


   
#line 14887 "./tex4ht-c.tex"


#ifdef KPATHSEA
if( !dot_file ) {                    U_CHAR * envfile;
                             char *arch, *p, str[256];
  
#line 14939 "./tex4ht-c.tex"

p = arch = (char *) kpse_var_value( "SELFAUTOLOC" );
while( *p != '\0' ){
   if( (*p ==   '/') || (*p == '\\') ){
      arch = p;
   }
   p++;
}


  envfile = (char *) 0;
  
#line 14950 "./tex4ht-c.tex"

if( arch ){
  (IGNORED) sprintf(str,"%s%ctex4ht.env", arch+1, *arch);
  if( dump_env_search ){
    (IGNORED) printf("kpse_open_file (\"%s\", ...)?\n", str );
  }
  envfile= kpse_find_file (str, kpse_program_text_format, 0);
}


  if ( !envfile ){ 
#line 14961 "./tex4ht-c.tex"

if( dump_env_search ){
  (IGNORED) printf("kpse_open_file (\"tex4ht.env\", ...)?\n");
}
envfile= kpse_find_file ("tex4ht.env", kpse_program_text_format, 0);

 }
  if ( !envfile ){ 
#line 15012 "./tex4ht-c.tex"

#define KPSEWHICH_CMD "kpsewhich --progname=tex4ht --format=othertext tex4ht.env"
if( dump_env_search ){
  (IGNORED) printf("system(" KPSEWHICH_CMD ")?\n"); /* cpp concatenation */
}
if( system(KPSEWHICH_CMD ">tex4ht.tmp") == 0 ){
   
#line 15042 "./tex4ht-c.tex"

char fileaddr [256];
int loc = 0;
FILE* file =  f_open("tex4ht.tmp", READ_TEXT_FLAGS);
if( file ){
  while( (fileaddr[loc] = getc(file)) >=0  ){
    if( fileaddr[loc] == '\n' ){ fileaddr[loc] = '\0'; break; }
    loc++;
  }
  (IGNORED) fclose(file);
}


   envfile= kpse_find_file (fileaddr, kpse_program_text_format, 0);
   if( envfile ){
      warn_i_str( 50,
          "search support for kpse_find_file--using kpsewhich calls instead");
}  }

 }
  if ( envfile ){
    dot_file = kpse_open_file (envfile,
                              kpse_program_text_format);
    (IGNORED) printf("(%s)\n",  envfile);
  } else if( dump_env_search ){
    p = (char *) kpse_var_value( "TEX4HTINPUTS" );
    if( p ){
       (IGNORED)  printf( "TEX4HTINPUTS = %s\n", p );
    } else {  warn_i_str( 50, "kpathsea variable TEX4HTINPUTS"); }
  }
}
#endif


   if( !dot_file ) { bad_in_file(
#line 13187 "./tex4ht-c.tex"

#ifdef  DOS_WIN32
   "tex4ht.env"
#else
   "tex4ht.env | .tex4ht"
#endif

); } /* give up if no tex4ht.env */
}


  
#line 4626 "./tex4ht-c.tex"

(IGNORED) fseek(dot_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
while ( search_dot_file( 'P' ) ){     struct sys_call_rec *q;
                                      U_CHAR *p, str[256];
  q = m_alloc(struct sys_call_rec, 1);
  q->next = system_calls;
  system_calls = q;
  p = str;
  do
     *(p++) = ch = (int) getc(dot_file);
  while( (ch !='\n') && (ch != EOF) );
  p--;
  *p = '\0';
  q->filter = m_alloc(char, (int) strlen((char *) str)+1);
  (IGNORED) strcpy((char *) q->filter, (char *) str);
}


#line 8857 "./tex4ht-c.tex"

lg_font_fmt = (char *) get_script(lg_font_fmt,LGFNT,'f');


#line 10854 "./tex4ht-c.tex"

class_fmt = (char *) get_script(class_fmt,LGCLS,'c');


#line 11083 "./tex4ht-c.tex"

font_gif = (char *) get_script(font_gif,LGPIC,'s');

#line 11089 "./tex4ht-c.tex"

{    int n;
  n = (int) strlen((char *) font_gif);
  if( font_gif[n-1] != '%' ){ font_gif[n] = '%'; font_gif[n+1] = '\0'; }
}




#line 11121 "./tex4ht-c.tex"

begin_char_gif = (char *) get_script(begin_char_gif,LGSEP,'b');


#line 11149 "./tex4ht-c.tex"

gif = (char *) get_script(gif,LGTYP,'g');
{              int n;
   n = (int) strlen((char *) gif) - 1;
   if( gif[n] == '%' ){  gif[n] = '%'; }
   else if( gif[n] == '\n' ) {  gif[n] = '\0'; }
}


  
#line 1880 "./tex4ht-c.tex"

{      long  curr_pos;
  curr_pos = ftell(dvi_file);
  (IGNORED) fseek(dvi_file, 0, SEEK_END);
  file_len = ftell(dvi_file);
  (IGNORED) fseek(dvi_file, curr_pos, 
#line 2174 "./tex4ht-c.tex"

0
);
  if( (file_len % 4) != 0 )  bad_dvi;
}


  
#line 5953 "./tex4ht-c.tex"

{      U_CHAR str[256];
   (IGNORED) strcpy((char *) str, (char *) job_name);
   str[job_name_n-1] = '\0';
   str[job_name_n-2] = 'g';
   str[job_name_n-3] = 'l';
   if( (log_file = fopen(str, WRITE_TEXT_FLAGS)) == NULL )
                                           bad_in_file(str);
}


  
#line 14437 "./tex4ht-c.tex"

#ifdef KPATHSEA
{           char str [256], *export_str, *postfix;
   export_str = m_alloc(char, 1);
   *export_str = '\0';
   
#line 14525 "./tex4ht-c.tex"

{
      struct htf_com_rec *q;
  q = htf_font_dir;
  while( q != (struct htf_com_rec *) 0 ){
    (IGNORED) strcpy((char *) str, (char *) q->name);
    export_htf( &export_str, str );
    q = q->next;
} }
(IGNORED) fseek(dot_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
while ( search_dot_file( 'i' ) ){
           int ch;
           char* p;
  p = str;
  do {
     ch = (int) getc(dot_file);
     if( ch != EOF) { *(p++) = ch;}
  } while( (ch !='\n') && (ch != EOF) );
  *p = '\0';
  export_htf( &export_str, str );
}
#ifdef HTFDIR
  (IGNORED) strcpy((char *) str, (char *) HTFDIR);
    export_htf( &export_str, str );
#endif

#line 14555 "./tex4ht-c.tex"

{                    U_CHAR * q;
  q = (U_CHAR *) kpse_var_value( "TEX4HTFONTSET" );
  if( q ){
    if( (int) strlen((char *) q) > 0 ){
        export_str = (char *) r_alloc((void *) export_str,
            (int) strlen((char *) export_str) + (int) strlen((char *) q) +  2);
        if( (int) strlen((char *) export_str) > 0 ){
             (IGNORED) strcat((char *) export_str, ",");
        }
        (IGNORED) strcat((char *) export_str, (char *)  q);
} } }




   if( (int) strlen((char *) export_str) > 0 ){
      (IGNORED) strcpy((char *) str, "%%12");
      export_str = (char *) r_alloc((void *) export_str,
          (int) strlen((char *) export_str) + (int) strlen((char *) str) +  1 );
      postfix = str - 1;
      while( *(++postfix) != '\0' ){
        if( (*postfix=='%')     && (*(postfix+1)=='%') &&
            (*(postfix+2)=='1') && (*(postfix+3)=='2') ){
          *postfix='\0'; postfix += 4; break;
      } }
      if( (int) strlen((char *) export_str) != 0 ){
        
#line 14468 "./tex4ht-c.tex"

{                       char *from_ch;
                        int i, n, m;
  n = (int) strlen((char *) str);
  m = (int) strlen((char *) export_str);
  from_ch = export_str + m;
  for( i = 0; i<=m; i++){
    *(from_ch + n) = *(from_ch);
    from_ch--;
  }
  for( i = 0; i<n; i++){
    export_str[i] = str[i];
  }
  (IGNORED) strcat((char *) export_str, (char *) postfix);
}


        
#line 14493 "./tex4ht-c.tex"

{                     U_CHAR * q;
  if( dump_htf_search ) {                       U_CHAR *p, *q;
     
#line 14842 "./tex4ht-c.tex"

p = (U_CHAR *) kpse_var_value( "TEX4HTFONTSET" );
if( p ){
   (IGNORED) printf("given TEX4HTFONTSET = %s\n", p);
}
q = getenv("TEX4HTFONTSET");
if( q ){  (IGNORED) printf(
   "Environmet var TEX4HTFONTSET:  %s\n", q);
}
if( !p && !q ){
   (IGNORED) printf( "Missing TEX4HTFONTSET for kpathsea\n" );
}


  }
  q = (U_CHAR *) kpse_var_value( "TEX4HTFONTSET" );
  if( q ){
     xputenv("TEX4HTFONTSET", export_str);
     if( dump_htf_search ){
        (IGNORED) printf("setting TEX4HTFONTSET={%s}\n", export_str);
     }
  } else if( dump_htf_search ) {
     warn_i_str( 50, "TEX4HTFONTSET for kpathsea" );
  }
}


      }
   }
   
#line 14673 "./tex4ht-c.tex"

{                                  int n;
   n = (int) strlen((char *) export_str);
   if( n > 0 ){
      export_str_chars = m_alloc(char, n+1);
      (IGNORED) strcpy((char *) export_str_chars, (char *) export_str);
}  }


   free((void *) export_str);
   
#line 14790 "./tex4ht-c.tex"

if( dump_htf_search || dump_env_search ) {
                                U_CHAR *p, *q;
   
#line 14820 "./tex4ht-c.tex"

p = kpse_find_file ( "texmf.cnf", kpse_cnf_format, 0);
if( p ){
   (IGNORED) printf( "texmf.cnf = %s\n", p);
} else { warn_i_str(1, "texmf.cnf" ); }
p = (U_CHAR *) kpse_var_value( "TEX4HTINPUTS" );
if( p ){
   (IGNORED) printf("TEX4HTINPUTS = %s\n", p);
}
q = getenv("TEX4HTINPUTS");
if( q ){  (IGNORED) printf(
   "Environment var TEX4HTINPUTS:  %s\n", q);
}
if( !p && !q ){
   (IGNORED) printf( "Missing TEX4HTINPUTS for kpathsea\n" );
}


}


}
#endif


  
#line 2150 "./tex4ht-c.tex"

i=0;
do{
  i++; file_len -= 1;
  (IGNORED) fseek(dvi_file, file_len, 
#line 2174 "./tex4ht-c.tex"

0
);
}   while( (ch=get_char()) == 
#line 17420 "./tex4ht-c.tex"

223 
 );
eof_op_n = file_len;
if( (i<4)
    ||
    ((ch != 
#line 2283 "./tex4ht-c.tex"

2 
) && (ch > 
#line 2292 "./tex4ht-c.tex"

10

))
  )  bad_dvi;
version_id = ch;


#line 2185 "./tex4ht-c.tex"

file_len -= 5;
(IGNORED) fseek(dvi_file, file_len, 
#line 2174 "./tex4ht-c.tex"

0
);
if( get_char() != 
#line 17469 "./tex4ht-c.tex"

249 
 )  bad_dvi;
eof_op_n -= begin_postamble = get_unt(4);
(IGNORED) fseek(dvi_file, begin_postamble, 
#line 2174 "./tex4ht-c.tex"

0
);


#line 2197 "./tex4ht-c.tex"

if( get_char() != 
#line 17466 "./tex4ht-c.tex"

248 
 )  bad_dvi;
(IGNORED) fseek(dvi_file, 16L, 
#line 2177 "./tex4ht-c.tex"

1
);

#line 5968 "./tex4ht-c.tex"

mid_page_y = (INTEGER) get_unt(4) / 2;
mid_page_x = (INTEGER) get_unt(4) / 2;


if( (stack_len = (int) get_unt(2)) < 1)     bad_dvi;

#line 6679 "./tex4ht-c.tex"

stack = m_alloc(struct stack_entry,
#line 6684 "./tex4ht-c.tex"

((int) stack_len + 2)

);

#line 6718 "./tex4ht-c.tex"

{                   int i;
  for( i=
#line 6684 "./tex4ht-c.tex"

((int) stack_len + 2)

-1; i>=0; i--){
    stack[i].begin = (struct group_info *) 0;
    stack[i].end   = (struct stack_end_entry *) 0;
    stack[i].stack_id = -1;
    
#line 6354 "./tex4ht-c.tex"

stack[i].halign_info = FALSE;
stack[i].halign_on = FALSE;


#line 7804 "./tex4ht-c.tex"

stack[i].path_start = (struct group_path *) 0;
stack[i].path_end   = (struct group_path *) 0;


#line 12887 "./tex4ht-c.tex"

stack[i].class_open = stack[i].class_close
                    = (char *) 0;
stack[i].temp_class_open  = m_alloc(char, 1 );
stack[i].temp_class_close = m_alloc(char, 1 );
stack[i].ignore_subclass_del = stack[i].temp_class_del
                          = stack[i].active_class_del
                          = FALSE;


} }




unread_pages = (int) get_unt(2);


#line 2222 "./tex4ht-c.tex"

{      
#line 10256 "./tex4ht-c.tex"

int fonts_n;
struct html_font_rec *html_font=0;


#line 11938 "./tex4ht-c.tex"

struct visited_file_rec * visited_file =
                       (struct visited_file_rec *) 0;


#line 13648 "./tex4ht-c.tex"

#ifndef KPATHSEA
struct env_var_rec *tfm_dirs;
#endif
struct env_var_rec *htf_dirs;


#line 14663 "./tex4ht-c.tex"


#ifdef KPATHSEA

#line 14683 "./tex4ht-c.tex"

int cardinality=0;
char ** fontset=0;


#endif


       BOOL missing_fonts;
#ifndef KPATHSEA
       
#line 14195 "./tex4ht-c.tex"

U_CHAR files_cache[PATH_MAX];


   
#line 14155 "./tex4ht-c.tex"

{                    U_CHAR *p;
   if( !tex4ht_fls_name ){
      tex4ht_fls_name = p = files_cache;
      (IGNORED) fseek(dot_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
      if ( search_dot_file( 'l' ) ){
        do
           *(p++) = ch = (int) getc(dot_file);
        while( (ch !='\n') && (ch != EOF) );
        p--;       *p = '\0';
      } else { (IGNORED) strcpy((char *) p,
(char *)           getenv("TEX4HTWR")?  "~~/tex4ht.fls" : "tex4ht.fls");
      }
   }
   
#line 14174 "./tex4ht-c.tex"

if( *tex4ht_fls_name == '~' ){
   tex4ht_fls_name = abs_addr(tex4ht_fls_name,getenv("TEX4HTWR"));
}


}


   
#line 14338 "./tex4ht-c.tex"

cache_files = f_open(tex4ht_fls_name, READ_BIN_FLAGS);


#endif
   
#line 10261 "./tex4ht-c.tex"

fonts_n = 0;


#line 13728 "./tex4ht-c.tex"

cache_font = (struct cache_font_rec *) 0;
cur_cache_font = (struct cache_font_rec *) 0;


   missing_fonts = FALSE;
   
#line 13568 "./tex4ht-c.tex"

#ifndef KPATHSEA
tfm_dirs = get_env_var("TEX4HTTFM");
#endif
htf_dirs = get_env_var("TEX4HTHTF");


   
#line 14688 "./tex4ht-c.tex"


#ifdef KPATHSEA
if( export_str_chars ){
  
#line 14709 "./tex4ht-c.tex"

{                   U_CHAR   *p;
                    int n;
   cardinality = 1;
   p = (U_CHAR *) export_str_chars;
   while( *p != '\0' ){
     if( *p == ',' ){ cardinality++; }
     p++;
   }
   fontset = m_alloc(char *, cardinality);
   p = (U_CHAR *) export_str_chars;
   fontset[0] = p;
   n=1;
   while( *p != '\0' ){
      if( *p == ',' ){ fontset[n++] = p+1; *p = '\0';  }
      p++;
}  }


}
#endif


   
#line 15935 "./tex4ht-c.tex"

{                              U_CHAR name[256];
                               FILE* file;
   (IGNORED) sprintf(name, "%s.4hf", "unicode");
   
#line 13837 "./tex4ht-c.tex"

   file = NULL;
   
#line 13905 "./tex4ht-c.tex"

{
                               struct htf_com_rec *p;
   p = htf_font_dir;
   while( p ){
      file =  search_file_base(name, p->name, READ_TEXT_FLAGS, htf_dirs);
      if( file ){
#ifndef KPATHSEA
         tex4ht_fls = TRUE;
#endif
         break;
      }
      p = p->next;
   }
}


   if( !file ){
      if( ((file = f_open(name, READ_TEXT_FLAGS)) == NULL) && dot_file )
         file = search_in_dot_file( 'i', name, READ_TEXT_FLAGS, htf_dirs);
#ifdef HTFDIR
      if( !file )  file = search_file_base(name, HTFDIR,
                                               READ_TEXT_FLAGS, htf_dirs);
#endif
      
#line 15073 "./tex4ht-c.tex"


#ifdef KPATHSEA
  if( !file ){ U_CHAR * htfname;
     htfname= kpse_find_file (name, kpse_program_text_format, 0);
     if ( htfname ){
         
#line 14729 "./tex4ht-c.tex"

{                    U_CHAR  * head, * tail, *p;
                     int n;
   
#line 14739 "./tex4ht-c.tex"

n = (int) strlen((char *) htfname);
tail = head = m_alloc(char, n+1);
(IGNORED) strcpy((char *) head, (char *) htfname);
while( n>11 ){
  if( (*tail=='\\') || (*tail=='/') ){
     if( (*tail == *(tail+9)) && (*(tail+1) == 'h')
         && (*(tail+2) == 't') && (*(tail+3) == '-')
         && (*(tail+4) == 'f') && (*(tail+5) == 'o')
         && (*(tail+6) == 'n') && (*(tail+7) == 't')
         && (*(tail+8) == 's') ){
        p = tail + 9;  *(tail + 10) = '\0';  tail += 11;
        while( (*tail != *p) && (*tail != '\0') ){  tail++; }
        break;
  }  }
  tail++; n--;
}


   htfname =  (U_CHAR *) 0;
   
#line 14761 "./tex4ht-c.tex"

for( n = 0 ; (n < cardinality) && !htfname ; n++){
  p = tail;
  while( *p != '\0' ){
                               char * s, *nm;
     s = m_alloc(char, (int) strlen((char *)  head )       +
                       (int) strlen((char *)  fontset[n] ) +
                       (int) strlen((char *)  p )          + 1);
     (IGNORED) strcpy((char *) s, (char *) head);
     (IGNORED) strcat((char *) s, (char *) fontset[n]);
     (IGNORED) strcat((char *) s, (char *) p);
     nm = kpse_find_file (s, kpse_program_text_format, 0);
     free((void *) s);
     if ( nm ){
        htfname = nm; break;
     }
     p++;
     while( (*p != '\\') && (*p != '/')  && (*p != '\0') ){ p++; }
} }


}


         if ( htfname ){
            (IGNORED) printf("(%s)\n",  htfname);
            file= fopen(htfname,READ_TEXT_FLAGS);
     }   }
  }
#endif


   }


   if( file ){
            
#line 15950 "./tex4ht-c.tex"

int chr, delimiter, delimiter_n, line_no, digit, i, j;
U_CHAR in[512], *in_p,  * start[4], *p;
BOOL char_on, err;
int value;


      
#line 16053 "./tex4ht-c.tex"

max_charset_n = 256;
charset =   m_alloc(struct charset_rec, 256);


#line 16465 "./tex4ht-c.tex"

max_htf_4hf_n = 256;
htf_4hf = m_alloc(struct htf_4hf_rec, 256);


      
#line 15958 "./tex4ht-c.tex"

err = FALSE;
line_no = 0;
while( TRUE ){
  line_no++;
  chr = (int) getc(file);
  if( chr == EOF ){ break; }
  if( (chr>32) && (chr<127) ){
     
#line 15994 "./tex4ht-c.tex"

delimiter   = chr;
delimiter_n = 1;
char_on     = TRUE;
in_p = in;
while( TRUE ) {
  chr = (int) getc(file);
  if( (chr == EOF) || (chr=='\n') ){ break; }
  if( chr == delimiter ){
     if( char_on ){ *(in_p++) = '\0'; }
     else{ start[ delimiter_n/2 ] = in_p; }
     char_on = !char_on;
     delimiter_n++;
  } else if (char_on ) {
     *(in_p++) = chr;
  }
  if( delimiter_n==8 ){ break; }
}


     if( delimiter_n == 8 ){
        if( *in != '?' ) {
           if( 
#line 15987 "./tex4ht-c.tex"

   (*in             != '&')
|| (*(in+1)         != '#')
|| ( (*(in+2)       != 'x') && (*(in+2) != 'X'))
|| (*(start[1] - 2) != ';')

 ){ err = TRUE; }
           else {
              
#line 16063 "./tex4ht-c.tex"

value = 0;
for( p=in+3; *p!=';'; p++){
  digit = (int) *p;
  if( (digit>='0') && (digit<='9') ){ digit -= '0'; }
  else if( (digit>='A') && (digit<='F') ){ digit -= BASE_A; }
  else if( (digit>='a') && (digit<='f') ){ digit -= BASE_a; }
  else { digit=0; err = TRUE; }
  value = 16*value + digit;
}


              if( start[3] == (in_p-1) ){
                 if( !err ){ 
#line 16082 "./tex4ht-c.tex"


#line 16112 "./tex4ht-c.tex"

if( (charset_n+1) == max_charset_n){
  max_charset_n += 10;
  charset = (struct charset_rec *) r_alloc((void *) charset,
        (size_t) ((max_charset_n) * sizeof(struct charset_rec) ));
}


p = m_alloc(char, (int) (start[3] - start[2]) );
(IGNORED) strcpy((char *) p, (char *) start[2] );
i = charset_n;
while( i-- > 0 ){
  if( charset[i].ch == value ){
     free((void *) charset[i].str);
     break;
  } else {
     if(   (charset[i].ch < value)
        || ((charset[i].ch > value) && (i==0)) ){
        if( charset[i].ch < value ){ i++; }
        charset_n++;
        for( j=charset_n; j>i; j-- ){
           charset[j].ch  = charset[j-1].ch;
           charset[j].str = charset[j-1].str;
        }
        break;
  }  }
}
if(i == -1){ i = charset_n; }
if( i==charset_n ){ charset_n++; }
charset[i].str = p;
charset[i].ch  = value;

 }
              } else { 
#line 16381 "./tex4ht-c.tex"


#line 16421 "./tex4ht-c.tex"

if( (htf_4hf_n+1) == max_htf_4hf_n){
  max_htf_4hf_n += 10;
  htf_4hf = (struct htf_4hf_rec *) r_alloc((void *) htf_4hf,
        (size_t) ((max_htf_4hf_n) * sizeof(struct htf_4hf_rec) ));
}
p = m_alloc(char, (int) (start[3] - start[2]) );


(IGNORED) strcpy((char *) p, (char *) start[2] );
i = htf_4hf_n;
while( i-- > 0 ){
  if( htf_4hf[i].ch == value ){
     free((void *) htf_4hf[i].str);
     break;
  } else {
     if(   (htf_4hf[i].ch < value)
        || ((htf_4hf[i].ch > value) && (i==0)) ){
        if( htf_4hf[i].ch < value ){ i++; }
        htf_4hf_n++;
        for( j=htf_4hf_n; j>i; j-- ){
           htf_4hf[j].ch = htf_4hf[j-1].ch;
           htf_4hf[j].str = htf_4hf[j-1].str;
           htf_4hf[j].type1  = htf_4hf[j-1].type1;
           htf_4hf[j].type2  = htf_4hf[j-1].type2;
        }
        break;
}  } }
if(i == -1){ i = htf_4hf_n; }
if(i == htf_4hf_n){ htf_4hf_n++; }
htf_4hf[i].str = p;
htf_4hf[i].ch  = value;

#line 16431 "./tex4ht-c.tex"

value = 0;
p = start[1];
while( *p != '\0' ){
   if( (*p < '0') || (*p > '9') ) break;
   value = value * 10 + *p - '0';
   p++;
}
htf_4hf[i].type1  =  value;


#line 16442 "./tex4ht-c.tex"

value = 0;
p = start[3];
while( *p != '\0' ){
   if( (*p < '0') || (*p > '9') ) break;
   value = value * 10 + *p - '0';
   p++;
}
htf_4hf[i].type2  =  value;




 }
     }  } }
     else { err = TRUE; }
     
#line 16020 "./tex4ht-c.tex"

if( err ){
   warn_i_int(48,line_no);
   (IGNORED) printf( "%c", delimiter );
   for( p=in; p != in_p; p++ ){
     if( *p=='\0' ){
       (IGNORED) printf("%c", delimiter);
       if( p != in_p-1 ){ (IGNORED) printf("  %c", delimiter); }
     }
     else { (IGNORED) printf( "%c", *p ); }
   }
   (IGNORED) printf( "\n" );
   err = FALSE;
}


  }
  while( (chr != EOF) && (chr!='\n') ){
     chr = (int) getc(file);
  }
  if( chr == EOF ){ break; }
}


      put_4ht_off = 0;
   } else{ put_4ht_off = 1; 
#line 16058 "./tex4ht-c.tex"

max_charset_n = 0;


#line 16470 "./tex4ht-c.tex"

max_htf_4hf_n = 0;

 }
}


   while( (ch =  get_char()) != 
#line 17469 "./tex4ht-c.tex"

249 
 ){
     
#line 8663 "./tex4ht-c.tex"

#ifdef MAXFONTS
if( (font_tbl_size + 1) < MAXFONTS )
#endif
{
              INTEGER new_font_checksum;
              int font_name_n;
   font_tbl = font_tbl_size?
              (struct font_entry *) r_alloc((void *) font_tbl,
                 (size_t) ((font_tbl_size+1)
                           * sizeof(struct font_entry)))
            : m_alloc(struct font_entry, 1);
   if(       (version_id == 
#line 2292 "./tex4ht-c.tex"

10

)
         &&  (ch == 
#line 17490 "./tex4ht-c.tex"

252

)
   ){
      
#line 8886 "./tex4ht-c.tex"

   unsigned short flags;
new_font.num       = (INTEGER) get_unt(4);
new_font.scale     = (INTEGER) get_unt(4);
new_font.design_sz = new_font.scale;
flags = (INTEGER) get_unt(2);

#line 8913 "./tex4ht-c.tex"

{    int  n, family_name_n, style_name_n;
     U_CHAR *ch;

  font_name_n =  (INTEGER) get_unt(1);
  family_name_n = (INTEGER) get_unt(1);
  style_name_n  = (INTEGER) get_unt(1);
  n =  font_name_n + 1;
  ch = new_font_name = m_alloc(char, n);
  while( --n ){  *ch = (int) get_unt(1); ch++; }
  *ch = '\0';
  n =  family_name_n + 1;

  while( --n ){  (void) get_unt(1); ch++; }
  *ch = '\0';
  n =  style_name_n + 1;

  while( --n ){  (void) get_unt(1); ch++; }
  *ch = '\0';
}


new_font.layout_dir = (flags & 
#line 8951 "./tex4ht-c.tex"

0x0100

) ? 1 : 0;
new_font.rgba_color = (flags & 
#line 8955 "./tex4ht-c.tex"

0x0200

)?
                                   (unsigned long) get_unt(4)
                                   :
                                   0xffffffff;
if( flags & 
#line 8959 "./tex4ht-c.tex"

0x0800

 ){
   int n =  (INTEGER) get_unt(2);  
   int i;
   for (i = 0; i < n; ++i) {       
         (void) get_unt(4);
   }
   for (i = 0; i < n; ++i) {       
      (void) get_int(4);
}  }
(IGNORED) printf("(--- xdv font = %s (not implemented) ---)\n", new_font_name);

#line 9077 "./tex4ht-c.tex"


#line 9298 "./tex4ht-c.tex"

{        int i;
   for( i=font_tbl_size-1; i>0;  i-- )
     if( new_font.num == font_tbl[i].num )  warn_i(10);   }



#line 9098 "./tex4ht-c.tex"

{    
   
/*
   if( font_file == NULL ){
      dump_env();      err_i_str(1,file_name);
      missing_fonts = TRUE;
      new_font.char_f = 2;
      new_font.char_l = 1;
   } else {
*/
      
#line 9119 "./tex4ht-c.tex"

{       
   
#line 9166 "./tex4ht-c.tex"

new_font.char_f = 0;
new_font.char_l = 255;
new_font.wtbl_n = 0;
new_font.htbl_n = 0;
new_font.dtbl_n = 0;


   
   
#line 9191 "./tex4ht-c.tex"

{      U_CHAR *ch, *hidp;
       int i;
   ch = new_font.char_wi = m_alloc(char, new_font.char_l
                                       - new_font.char_f + 1);
   hidp = new_font.char_hidp = m_alloc(char, new_font.char_l
                                       - new_font.char_f + 1);
   for( i = new_font.char_f; i <= new_font.char_l; i++ ){
      *(ch++) = 10;
      *(hidp++) = 26;
}  }


   
#line 9205 "./tex4ht-c.tex"

{       INTEGER *p;         
            int  i;
   p = new_font.wtbl = m_alloc( INTEGER, new_font.wtbl_n);
   for( i = 0; i < new_font.wtbl_n; i++ ){
      *(p++) = 600000;
}  }


   
#line 9216 "./tex4ht-c.tex"

{       INTEGER *p;         
            int  i;
   p = new_font.htbl = m_alloc( INTEGER, new_font.htbl_n);
   for( i = 0; i < new_font.htbl_n; i++ ){
      *(p++) = 600000;
}  }


   
   
   
   
   
   
#line 9140 "./tex4ht-c.tex"


#line 9151 "./tex4ht-c.tex"

new_font.it = 0;

             

#line 9156 "./tex4ht-c.tex"

new_font.word_sp = 350000;

#line 9752 "./tex4ht-c.tex"

if( new_font.word_sp == 0 ) {
        int i;
  for( i = new_font.char_f; i <= new_font.char_l; i++ ){
    new_font.word_sp =
      ( new_font.word_sp
       +
        *(new_font.wtbl + (int)(*(new_font.char_wi + i - new_font.char_f)))
      )  / (new_font.char_f<i? 2:1);
  } }
if( new_font.word_sp == 0 )  new_font.word_sp = MARGINSP; 











}


/*
      (IGNORED) fclose(font_file);
   }
*/
}


new_font_name[font_name_n] = '\0';
new_font.name = m_alloc(char, font_name_n + 1);
(IGNORED) strcpy((char *)  new_font.name, (char *) new_font_name );

#line 8781 "./tex4ht-c.tex"

{    int n, i;
   for( n=0; n<font_name_n; n++ ){
     if(  ( '0' <= new_font_name[n] ) && ( new_font_name[n] <= '9' )){
       break;
     }
   }
   
#line 8803 "./tex4ht-c.tex"

{                       int m;
   for( m=n; m<font_name_n; m++ ){
     if(  ( new_font_name[m] < '0' ) || ( new_font_name[m] > '9' )){
       n=font_name_n;
       break;
}  } }


   new_font.family_name = m_alloc(char, n + 2);
   new_font.font_size  = m_alloc(char, font_name_n - n + 1 );
   for( i=0; i<n; i++ ){
     new_font.family_name[i] = new_font_name[i];
   }
   new_font.family_name[i] = '\0';   i = 0;
   while(  n<font_name_n ){
     new_font.font_size[i++] = new_font_name[n++];
   }
   new_font.font_size[i] = '\0';
}



#line 9088 "./tex4ht-c.tex"

new_font.mag = new_font.scale / (new_font.design_sz / 100);






   } else {
      
#line 8697 "./tex4ht-c.tex"


#line 8723 "./tex4ht-c.tex"

switch( ch ){
  case 
#line 17451 "./tex4ht-c.tex"

243 
:
  case 
#line 17454 "./tex4ht-c.tex"

244 
:
  case 
#line 17457 "./tex4ht-c.tex"

245 
: {
     new_font.num = (INTEGER)
                      get_unt(ch - 
#line 17451 "./tex4ht-c.tex"

243 
 + 1); break; }
  case 
#line 17460 "./tex4ht-c.tex"

246 
: {
     new_font.num = (INTEGER) get_int(4);  break; }
  default: err_i(8);
}


new_font_checksum  = (INTEGER) get_int(4);
new_font.scale     = (INTEGER) get_unt(4);
new_font.design_sz = (INTEGER) get_unt(4);

#line 8737 "./tex4ht-c.tex"

{    int  n, area_ln;
     U_CHAR *ch;
  area_ln = (int) get_unt(1);
  font_name_n = (int) get_unt(1);
  n =  area_ln + font_name_n + 1;
  ch = new_font_name = m_alloc(char, n);
  while( --n ){  *ch = (int) get_unt(1); ch++; }
  *ch = '\0';
}



#line 8753 "./tex4ht-c.tex"


#line 9298 "./tex4ht-c.tex"

{        int i;
   for( i=font_tbl_size-1; i>0;  i-- )
     if( new_font.num == font_tbl[i].num )  warn_i(10);   }



#line 9344 "./tex4ht-c.tex"

{       FILE *font_file;
        U_CHAR  file_name[256];
   
#line 13512 "./tex4ht-c.tex"

{                        
   font_file = NULL;
   (IGNORED) sprintf(file_name, "%s.tfm", new_font_name);
   
#line 14971 "./tex4ht-c.tex"

#ifdef KPATHSEA
{
     U_CHAR * tfmfile;
  tfmfile = kpse_find_file (file_name, kpse_tfm_format, 0);
  if( !tfmfile ){ 
#line 15026 "./tex4ht-c.tex"

char s [256];
(IGNORED) strcpy(s, "kpsewhich ");
(IGNORED) strcat(s, file_name);
(IGNORED) strcat(s, " > tex4ht.tmp ");
if( system(s) == 0 ){
   
#line 15042 "./tex4ht-c.tex"

char fileaddr [256];
int loc = 0;
FILE* file =  f_open("tex4ht.tmp", READ_TEXT_FLAGS);
if( file ){
  while( (fileaddr[loc] = getc(file)) >=0  ){
    if( fileaddr[loc] == '\n' ){ fileaddr[loc] = '\0'; break; }
    loc++;
  }
  (IGNORED) fclose(file);
}


   tfmfile = kpse_find_file (fileaddr, kpse_program_text_format, 0);
}

 }
  if ( tfmfile ){
       (IGNORED) printf("(%s)\n",  tfmfile);
       font_file = kpse_open_file (tfmfile, kpse_tfm_format);
  }
}
#else


      
#line 13668 "./tex4ht-c.tex"

for( cur_cache_font = cache_font;
     cur_cache_font;
     cur_cache_font = cur_cache_font->next )
  if( (font_file = search_file(file_name,
                               cur_cache_font->dir,
                               READ_BIN_FLAGS))
      != NULL)  break;


      if( !font_file ){                                     int  i;
         for( i = fontdir_count; i--; ){
           if( (font_file =  search_file_base(file_name, fontdir[i],
                                         READ_BIN_FLAGS, tfm_dirs))
               != NULL )  break;  }
      }
      if( !font_file ) font_file = f_open(file_name, READ_BIN_FLAGS);
      if( !font_file && dot_file )
          font_file = search_in_dot_file( 't', file_name,
                        READ_BIN_FLAGS, tfm_dirs);
      
#line 13544 "./tex4ht-c.tex"

#ifdef TFMDIR
   if( !font_file )
      font_file = search_file_base(file_name, TFMDIR,
                               READ_BIN_FLAGS, tfm_dirs);
#endif


   
#line 14988 "./tex4ht-c.tex"

#endif


}


   if( font_file == NULL ){
      dump_env();      err_i_str(1,file_name);
      missing_fonts = TRUE;
      new_font.char_f = 2;
      new_font.char_l = 1;
   } else {
      
#line 9385 "./tex4ht-c.tex"

{       
#line 9435 "./tex4ht-c.tex"

 INTEGER  file_length;     
 int    header_length,
     it_correction_table_length,
     lig_kern_table_length,
     kern_table_length,
     extensible_char_table_length,     
     num_font_parameters;


   
#line 9407 "./tex4ht-c.tex"

file_length                    = (INTEGER) fget_int(font_file,2);
header_length                  = (int) fget_int(font_file,2);
new_font.char_f = (int) fget_int(font_file,2);
new_font.char_l = (int) fget_int(font_file,2);
new_font.wtbl_n = (int) fget_int(font_file,2);
new_font.htbl_n = (int) fget_int(font_file,2);
new_font.dtbl_n = (int) fget_int(font_file,2);
it_correction_table_length     = (int) fget_int(font_file,2);
lig_kern_table_length          = (int) fget_int(font_file,2);
kern_table_length              = (int) fget_int(font_file,2);
extensible_char_table_length   = (int) fget_int(font_file,2);
num_font_parameters            = (int) fget_int(font_file,2);
if( file_length != ( 6                + header_length
     - new_font.char_f              + new_font.char_l + 1
     + new_font.wtbl_n              + new_font.htbl_n
     + new_font.dtbl_n              + it_correction_table_length
     + lig_kern_table_length        + kern_table_length
     + extensible_char_table_length + num_font_parameters  )
  ){ err_i_str(15,file_name); }


   
#line 9480 "./tex4ht-c.tex"


#line 9489 "./tex4ht-c.tex"

{      INTEGER checksum;
   checksum = ( INTEGER) fget_int(font_file,4);
   if( checksum && new_font_checksum
                && (checksum  != new_font_checksum) )
    {   warn_i(16);
        (IGNORED) fprintf(stderr,"%s: %d\ndvi file: %d\n",file_name,
                     checksum, new_font_checksum);
}   }



#line 9509 "./tex4ht-c.tex"

new_font.design_pt = ( INTEGER) fget_int(font_file,4); 


(IGNORED) fseek(font_file, (long) ((header_length - 2) * 4), 
#line 2177 "./tex4ht-c.tex"

1
);


   
#line 9521 "./tex4ht-c.tex"

{      U_CHAR *ch, *hidp;
       int i;
   ch = new_font.char_wi = m_alloc(char, new_font.char_l
                                       - new_font.char_f + 1);
   hidp = new_font.char_hidp = m_alloc(char, new_font.char_l
                                       - new_font.char_f + 1);
   for( i = new_font.char_f; i <= new_font.char_l; i++ ){
      *(ch++) = (int) fget_unt(font_file,1);
      *(hidp++) = (int) fget_unt(font_file,1);
      (IGNORED) fseek(font_file, 2L, 
#line 2177 "./tex4ht-c.tex"

1
);
}  }


   
#line 9576 "./tex4ht-c.tex"

{       INTEGER *p;         
            int  i;
   p = new_font.wtbl = m_alloc( INTEGER, new_font.wtbl_n);
   for( i = 0; i < new_font.wtbl_n; i++ ){
      *(p++) = ( INTEGER) fget_int(font_file,4);
}  }


   
#line 9612 "./tex4ht-c.tex"

{       INTEGER *p;         
            int  i;
   p = new_font.htbl = m_alloc( INTEGER, new_font.htbl_n);
   for( i = 0; i < new_font.htbl_n; i++ ){
      *(p++) = ( INTEGER) fget_int(font_file,4);
}  }


#line 9636 "./tex4ht-c.tex"

{       INTEGER *p;         
            int  i;
   p = new_font.dtbl = m_alloc( INTEGER, new_font.dtbl_n);
   for( i = 0; i < new_font.dtbl_n; i++ ){
      *(p++) = ( INTEGER) fget_int(font_file,4);
}  }


   
   
#line 9659 "./tex4ht-c.tex"

                                                   
(IGNORED) fseek(font_file, (long) (it_correction_table_length * 4),
            
#line 2177 "./tex4ht-c.tex"

1
);


   
#line 9669 "./tex4ht-c.tex"

(IGNORED) fseek(font_file, (long) (lig_kern_table_length * 4),
             
#line 2177 "./tex4ht-c.tex"

1
);


   
#line 9678 "./tex4ht-c.tex"

                          
(IGNORED) fseek(font_file, (long) (kern_table_length * 4),
             
#line 2177 "./tex4ht-c.tex"

1
);


   
#line 9688 "./tex4ht-c.tex"

(IGNORED) fseek(font_file, (long) (extensible_char_table_length * 4),
             
#line 2177 "./tex4ht-c.tex"

1
);


   
#line 9703 "./tex4ht-c.tex"


#line 9718 "./tex4ht-c.tex"

new_font.it = ( INTEGER) fget_int(font_file,4);

             

#line 9724 "./tex4ht-c.tex"

new_font.word_sp = ( INTEGER) fget_int(font_file,4);

#line 9752 "./tex4ht-c.tex"

if( new_font.word_sp == 0 ) {
        int i;
  for( i = new_font.char_f; i <= new_font.char_l; i++ ){
    new_font.word_sp =
      ( new_font.word_sp
       +
        *(new_font.wtbl + (int)(*(new_font.char_wi + i - new_font.char_f)))
      )  / (new_font.char_f<i? 2:1);
  } }
if( new_font.word_sp == 0 )  new_font.word_sp = MARGINSP; 





#line 9768 "./tex4ht-c.tex"

(IGNORED) fseek(font_file, 4L, 
#line 2177 "./tex4ht-c.tex"

1
);



#line 9772 "./tex4ht-c.tex"

(IGNORED) fseek(font_file, 4L, 
#line 2177 "./tex4ht-c.tex"

1
);



#line 9161 "./tex4ht-c.tex"

new_font.ex = 450000;


#line 9778 "./tex4ht-c.tex"

new_font.ex = (INTEGER) fget_int(font_file,4);



#line 9784 "./tex4ht-c.tex"





#line 9790 "./tex4ht-c.tex"






}


      (IGNORED) fclose(font_file);
}  }


new_font_name[font_name_n] = '\0';
new_font.name = m_alloc(char, font_name_n + 1);
(IGNORED) strcpy((char *)  new_font.name, (char *) new_font_name );

#line 8781 "./tex4ht-c.tex"

{    int n, i;
   for( n=0; n<font_name_n; n++ ){
     if(  ( '0' <= new_font_name[n] ) && ( new_font_name[n] <= '9' )){
       break;
     }
   }
   
#line 8803 "./tex4ht-c.tex"

{                       int m;
   for( m=n; m<font_name_n; m++ ){
     if(  ( new_font_name[m] < '0' ) || ( new_font_name[m] > '9' )){
       n=font_name_n;
       break;
}  } }


   new_font.family_name = m_alloc(char, n + 2);
   new_font.font_size  = m_alloc(char, font_name_n - n + 1 );
   for( i=0; i<n; i++ ){
     new_font.family_name[i] = new_font_name[i];
   }
   new_font.family_name[i] = '\0';   i = 0;
   while(  n<font_name_n ){
     new_font.font_size[i++] = new_font_name[n++];
   }
   new_font.font_size[i] = '\0';
}



#line 8768 "./tex4ht-c.tex"

new_font.mag = new_font.scale / (new_font.design_sz / 100);






   }
   
#line 9994 "./tex4ht-c.tex"

{      U_CHAR str[256];
       int i, design_n, n_gif;
       
#line 10089 "./tex4ht-c.tex"

int loopBound = 0;
U_CHAR loopName[256];
loopName[0] = '\0';


   n_gif = new_font.char_l - new_font.char_f + 1;
   new_font.ch255 = 0;
   
#line 10024 "./tex4ht-c.tex"

{     int n_gif_bytes;
   n_gif_bytes = (n_gif + 7) / 8;
   new_font.gif_on = m_alloc(char, n_gif_bytes );
   new_font.ch_str = m_alloc(char, n_gif_bytes );
   
#line 12768 "./tex4ht-c.tex"

new_font.math_closing = m_alloc(char, n_gif_bytes );
new_font.math = m_alloc(char, n_gif );


   for( i=n_gif_bytes; i--; ) {
     
#line 12774 "./tex4ht-c.tex"

new_font.math_closing[i] =

 new_font.ch_str[i] = new_font.gif_on[i] = 0;
   }
   new_font.gif1 = m_alloc(unsigned char, n_gif );
   for( i=n_gif; i--; ) {
       
#line 12778 "./tex4ht-c.tex"

new_font.math[i] =

 new_font.gif1[i] = 0;
}  }


   
#line 12078 "./tex4ht-c.tex"

new_font.accent = m_alloc(unsigned char, n_gif );
new_font.accented = m_alloc(unsigned char, n_gif );
new_font.accent_array = (unsigned int *) 0;
new_font.accented_array = (unsigned int *) 0;
new_font.accent_N = new_font.accented_N = 0;
for( i=n_gif; i--; ) {
   new_font.accent[i] = new_font.accented[i] = 0;
}


   new_font.ch = m_alloc(unsigned char, n_gif );
   
#line 10039 "./tex4ht-c.tex"

for( i = new_font.char_f; i <= new_font.char_l ; i++ ){
  new_font.ch[i - new_font.char_f] =
                 (char)  (((31<i) && (i<128))? i : ignore_ch);
}


   new_font.str =  m_alloc(unsigned char*, n_gif);
   new_font.str[0] = &null_str;
   design_n = 0;
      
#line 10053 "./tex4ht-c.tex"

{  char search_font_name [256];
  (IGNORED) strcpy((char *) search_font_name, (char *) new_font.name);
  while( 1 ){                         BOOL flag;
     
#line 10095 "./tex4ht-c.tex"

if( eq_str( new_font_name, loopName) ){
     U_CHAR name[256];
   (IGNORED) sprintf(name, "%s.htf", new_font_name);
    err_i_str(1, name);
} else {
   (IGNORED) strcpy((char *) loopName, (char *) new_font_name);
}
loopBound++;
if( loopBound > 10 ){
   U_CHAR name[256];
   (IGNORED) sprintf(name, "%s.htf", new_font_name);
   err_i_str(1, name);
}


     flag = TRUE;
     for( ; font_name_n; font_name_n-- ){  FILE* file;
                                           int   char_f, char_l;
       new_font_name[font_name_n] = '\0';
       
#line 13828 "./tex4ht-c.tex"

{                              U_CHAR name[256];
   (IGNORED) sprintf(name, "%s.htf", new_font_name);
   
#line 13837 "./tex4ht-c.tex"

   file = NULL;
   
#line 13905 "./tex4ht-c.tex"

{
                               struct htf_com_rec *p;
   p = htf_font_dir;
   while( p ){
      file =  search_file_base(name, p->name, READ_TEXT_FLAGS, htf_dirs);
      if( file ){
#ifndef KPATHSEA
         tex4ht_fls = TRUE;
#endif
         break;
      }
      p = p->next;
   }
}


   if( !file ){
      if( ((file = f_open(name, READ_TEXT_FLAGS)) == NULL) && dot_file )
         file = search_in_dot_file( 'i', name, READ_TEXT_FLAGS, htf_dirs);
#ifdef HTFDIR
      if( !file )  file = search_file_base(name, HTFDIR,
                                               READ_TEXT_FLAGS, htf_dirs);
#endif
      
#line 15073 "./tex4ht-c.tex"


#ifdef KPATHSEA
  if( !file ){ U_CHAR * htfname;
     htfname= kpse_find_file (name, kpse_program_text_format, 0);
     if ( htfname ){
         
#line 14729 "./tex4ht-c.tex"

{                    U_CHAR  * head, * tail, *p;
                     int n;
   
#line 14739 "./tex4ht-c.tex"

n = (int) strlen((char *) htfname);
tail = head = m_alloc(char, n+1);
(IGNORED) strcpy((char *) head, (char *) htfname);
while( n>11 ){
  if( (*tail=='\\') || (*tail=='/') ){
     if( (*tail == *(tail+9)) && (*(tail+1) == 'h')
         && (*(tail+2) == 't') && (*(tail+3) == '-')
         && (*(tail+4) == 'f') && (*(tail+5) == 'o')
         && (*(tail+6) == 'n') && (*(tail+7) == 't')
         && (*(tail+8) == 's') ){
        p = tail + 9;  *(tail + 10) = '\0';  tail += 11;
        while( (*tail != *p) && (*tail != '\0') ){  tail++; }
        break;
  }  }
  tail++; n--;
}


   htfname =  (U_CHAR *) 0;
   
#line 14761 "./tex4ht-c.tex"

for( n = 0 ; (n < cardinality) && !htfname ; n++){
  p = tail;
  while( *p != '\0' ){
                               char * s, *nm;
     s = m_alloc(char, (int) strlen((char *)  head )       +
                       (int) strlen((char *)  fontset[n] ) +
                       (int) strlen((char *)  p )          + 1);
     (IGNORED) strcpy((char *) s, (char *) head);
     (IGNORED) strcat((char *) s, (char *) fontset[n]);
     (IGNORED) strcat((char *) s, (char *) p);
     nm = kpse_find_file (s, kpse_program_text_format, 0);
     free((void *) s);
     if ( nm ){
        htfname = nm; break;
     }
     p++;
     while( (*p != '\\') && (*p != '/')  && (*p != '\0') ){ p++; }
} }


}


         if ( htfname ){
            (IGNORED) printf("(%s)\n",  htfname);
            file= fopen(htfname,READ_TEXT_FLAGS);
     }   }
  }
#endif


   }


   if( file ){ 
#line 8827 "./tex4ht-c.tex"

if( (strlen((char *) new_font.family_name) +
     strlen((char *) new_font.font_size) + 4) == strlen((char *) name) ){
  new_font.family_name = (char *) r_alloc((void *) new_font.family_name,
                         (size_t) (strlen((char *) name)+1));
  (IGNORED) strcat((char *) new_font.family_name, (char *) new_font.font_size);
  *(new_font.font_size)='\0';
}

 }
}


       if( file != NULL){
                                           INTEGER x_char_l;
         
#line 10580 "./tex4ht-c.tex"

x_char_l =
      get_html_file_id(file, new_font.char_f, new_font.char_l, 19);
if( x_char_l != HTF_ALIAS) {
     char_f = (int) (x_char_l / 1000.0 + 0.5) + new_font.char_f;
     x_char_l -= (char_f-new_font.char_f) * 1000 - new_font.char_l;
     char_l = (int) x_char_l;
}


         if( x_char_l == HTF_ALIAS) {
           
#line 10166 "./tex4ht-c.tex"

{                                 int chr;
  font_name_n=0;
  while( (chr = get_html_ch(file)) != '\n' ){
    if( chr != ' ' ) search_font_name[font_name_n++] = chr;
  }
  search_font_name[font_name_n]  = '\0';
  if( eq_str( search_font_name, new_font_name) ){ err_i_str(20, new_font_name); }
  (IGNORED) printf("Searching `%s.htf' for `%s.htf'\n",
                                        search_font_name, new_font.name);
  htf_to_lg(html_font, new_font_name, fonts_n, file);
  new_font_name = (char *)  r_alloc((void *) new_font_name,
                                  (size_t) (font_name_n+1));
  (IGNORED) strcpy((char *) new_font_name, (char *) search_font_name);
  font_name_n = strlen((char *) new_font_name);
}


           (IGNORED) fclose(file);  flag = FALSE; break;
         }
         
#line 10283 "./tex4ht-c.tex"

if( char_f <= new_font.char_l ){      U_CHAR  del;
                                      int  j, n;
   
#line 10297 "./tex4ht-c.tex"

while( char_f < new_font.char_f ){
  while( get_html_ch(file) != '\n' );
  char_f++;   }


   n =  ((char_l < new_font.char_l)? char_l : new_font.char_l)
        - char_f + 1;
   for( i = char_f - new_font.char_f; i < n; i++ ){
      
#line 10312 "./tex4ht-c.tex"

{      int indirect_ch, base=0, value=0, digit, ch1;
  indirect_ch = 0;
  del = get_html_ch(file);   j=0;
  while( (str[j++] = get_html_ch(file)) != del )
    { 
#line 10346 "./tex4ht-c.tex"

if( (digit=str[j-1]) == '\\' )
  if( (indirect_ch = !indirect_ch) != 0) {
    switch( value=get_html_ch(file) ){
      case 'x': { base = 16; value = 0;  j--; break; }
      case 'o': { base = 8;  value = 0;  j--; break; }
      default: {
        if( (value < '0') || (value > '9') ) {
          indirect_ch = !indirect_ch;   str[j-1] = value;
        } else { value -= '0';  base = 10; j--; }
  } } }
  else{ if( value>255 ){
            warn_i_int(28,value);
            value = 32;  dump_htf( file );
        }
        str[j-1] = value;
  }
else if ( indirect_ch ){
  j--;   digit -=  (digit>'9')?  'A'-10 : '0';
  if( (digit<0) || (digit>=base) ){
      warn_i_int(29, str[j]);
      digit = 0; dump_htf( file );
  }
  value = value*base + digit;
} else if ( str[j-1]==10 ){
   dump_htf( file );
   err_i_int(48, i+1);
}

 };
  str[j-1] = '\0';
  while( get_html_ch(file) != del );
  ch1 = 0;
  while( ((ch = (int) get_html_ch(file)) != del) ){
     if( (ch < '0') || (ch > '9') ) break;
     ch1 = ch1 * 10 + ch - '0'; }
  
#line 16480 "./tex4ht-c.tex"

if(
       (*str             == '&')
    && (*(str+1)         == '#')
    && ( (*(str+2)       == 'x') || (*(str+2) == 'X'))
    && (*(str + strlen((char *) str) - 1) == ';')
) {
        char* p;
        int   value = 0;
        BOOL  err = FALSE;
    for( p=str+3; *p!=';'; p++){
      int digit = (int) *p;
      if( (digit>='0') && (digit<='9') ){ digit -= '0'; }
      else if( (digit>='A') && (digit<='F') ){ digit -= BASE_A; }
      else if( (digit>='a') && (digit<='f') ){ digit -= BASE_a; }
      else { digit=0; err = TRUE; }
      value = 16*value + digit;
    }
    if( !err ){
      
#line 16518 "./tex4ht-c.tex"

    int bottom, mid, top;
    BOOL found=FALSE;
bottom = 0; top = htf_4hf_n;
while( !found ){
   mid = (bottom + top) / 2;
   if( value == htf_4hf[mid].ch ){
      
#line 16504 "./tex4ht-c.tex"

if( htf_4hf[mid].type1 == ch1  ){
   ch1 = htf_4hf[mid].type2;
   (IGNORED) strcpy((char *) str, (char *) htf_4hf[mid].str );
}


      found = TRUE;
   } else if( value < htf_4hf[mid].ch ){
      if( bottom == top ){ break; }
      top = mid;
   }
   else {
     if ( bottom < mid ){  bottom = mid; }
     else if ( bottom<top ){ bottom++; }
     else{ break; }
   }
}


      
}   }


  new_font.gif1[i] = ch1 % 256;
  do{
    if( (ch = get_html_ch(file)) == del ){
      
#line 12090 "./tex4ht-c.tex"

ch1 = 0;
while( ((ch = (int) get_html_ch(file)) != del) ){
     if( (ch < '0') || (ch > '9') ){ warn_i_int(48,i);  break; }
     ch1 = ch1 * 10 + ch - '0'; }
new_font.accent_array = new_font.accent_N++?
      (unsigned int *) r_alloc((void *) new_font.accent_array,
                 (size_t) (new_font.accent_N * sizeof(unsigned int)))
            :  m_alloc(unsigned int, 1);
new_font.accent_array[new_font.accent_N - 1] = ch1;
new_font.accent[i] = new_font.accent_N;

  break;
    }
  } while( ch != '\n' );
  if( ch != '\n' ){
    do{
      if( (ch = get_html_ch(file)) == del ){
        
#line 12104 "./tex4ht-c.tex"

ch1 = 0;
while( ((ch = (int) get_html_ch(file)) != del) ){
     if( (ch < '0') || (ch > '9') ){ warn_i_int(48,i);  break; }
     ch1 = ch1 * 10 + ch - '0'; }
new_font.accented_array = new_font.accented_N++?
      (unsigned int *) r_alloc((void *) new_font.accented_array,
                 (size_t) (new_font.accented_N * sizeof(unsigned int)))
            :  m_alloc(unsigned int, 1);
new_font.accented_array[new_font.accented_N - 1] = ch1;
new_font.accented[i] = new_font.accented_N;

  break;
      }
    } while( ch != '\n' );
  }
  if( ch != '\n' )
  { while( get_html_ch(file) != '\n' ); }
}


      
#line 10389 "./tex4ht-c.tex"

add_bit( new_font.ch_str, i, j!=2 );
switch( j ){
  case 1: { new_font.ch[i] = 0;    break; }
  case 2: { new_font.ch[i] = *str; break; }
  default: {                           unsigned U_CHAR  *p;
    new_font.str[design_n] = p = m_alloc(unsigned char, j);
    if( design_n>255 ){ design_n--; warn_i(35);}
    if( i==255 ){
       if( design_n == 255 ){
                new_font.ch[i] = 0;           new_font.ch255 = 1;
       } else { new_font.ch[i] = ++design_n;  }
    } else {    new_font.ch[i] = ++design_n; }
    while( j-- )  p[j] = str[j];  
} }

                     }
   
#line 10303 "./tex4ht-c.tex"

while( char_l > new_font.char_l ){
  while( get_html_ch(file) != '\n' );
  char_l--;   }


}


         
#line 10591 "./tex4ht-c.tex"

(void) get_html_file_id(file, new_font.char_f, new_font.char_l, 18);


         htf_to_lg(html_font,  new_font_name, fonts_n, file);
         
#line 10499 "./tex4ht-c.tex"

if( dump_htf_files ){
   dump_htf_files++;  dump_htf( file );  dump_htf_files--;
}


         (IGNORED) fclose(file);  break;
     } }
     if( flag ){ break; }
  }
  if( font_name_n == 0 ){
     if( errCode == 0 ){ errCode= 21; }
     warn_i_str(21,search_font_name);
     (IGNORED) fprintf(stderr,
               "%d--%d)\n", new_font.char_f, new_font.char_l);
     dump_env();
  } else { 
#line 10505 "./tex4ht-c.tex"

if( dump_env_files ){ dump_env(); }

 }
}


   new_font.str = (unsigned U_CHAR **) r_alloc((void *)   new_font.str,
                     (size_t) ( (design_n?design_n:1) * sizeof(char *)) );
   
#line 10231 "./tex4ht-c.tex"

for( i = fonts_n; i--; )
  if( eq_str(html_font[i].name, new_font_name) ){       int k;
     k = html_font[i].i;
     free((void *)  new_font.gif1 ); new_font.gif1= font_tbl[ k ].gif1;
     free((void *)  new_font.ch );   new_font.ch  = font_tbl[ k ].ch;
     free((void *)  new_font.str );  new_font.str = font_tbl[ k ].str;
     free((void *)  new_font.ch_str );
                               new_font.ch_str = font_tbl[ k ].ch_str;
     
#line 12782 "./tex4ht-c.tex"

free((void *)  new_font.math_closing );
           new_font.math_closing = font_tbl[ k ].math_closing;
free((void *)  new_font.math );
                           new_font.math = font_tbl[ k ].math;


     break;     }
if( i < 0 ){ 
#line 10246 "./tex4ht-c.tex"

html_font = fonts_n? (struct html_font_rec *) r_alloc((void *) html_font,
                 (size_t) ((fonts_n+1) * sizeof(struct html_font_rec) ))
                   :  m_alloc(struct html_font_rec, 1);
html_font[fonts_n].name = m_alloc(char, font_name_n + 1);
(IGNORED) strcpy((char *) html_font[fonts_n].name, (char *) new_font_name);
html_font[fonts_n].i    = font_tbl_size;
fonts_n++;

 }


}


   
#line 11875 "./tex4ht-c.tex"

{    static struct css_ext_rec * search_css_ext;
  for( search_css_ext = css_ext;
       search_css_ext != (struct css_ext_rec *) 0;
       search_css_ext = search_css_ext->next       ){
     int css_name_n = (int) strlen((char *)  new_font.name );
     char * css_file_name = m_alloc(char, css_name_n + 1);
     (IGNORED) strcpy((char *) css_file_name, (char *) new_font.name);
     for( ; css_name_n; css_name_n-- ){
                                               FILE* file;
       css_file_name[css_name_n] = '\0';
       
#line 11892 "./tex4ht-c.tex"

{                              U_CHAR name[256];
   (IGNORED) sprintf(name, "%s.%s", css_file_name,
                                    search_css_ext->name);
   
#line 13837 "./tex4ht-c.tex"

   file = NULL;
   
#line 13905 "./tex4ht-c.tex"

{
                               struct htf_com_rec *p;
   p = htf_font_dir;
   while( p ){
      file =  search_file_base(name, p->name, READ_TEXT_FLAGS, htf_dirs);
      if( file ){
#ifndef KPATHSEA
         tex4ht_fls = TRUE;
#endif
         break;
      }
      p = p->next;
   }
}


   if( !file ){
      if( ((file = f_open(name, READ_TEXT_FLAGS)) == NULL) && dot_file )
         file = search_in_dot_file( 'i', name, READ_TEXT_FLAGS, htf_dirs);
#ifdef HTFDIR
      if( !file )  file = search_file_base(name, HTFDIR,
                                               READ_TEXT_FLAGS, htf_dirs);
#endif
      
#line 15073 "./tex4ht-c.tex"


#ifdef KPATHSEA
  if( !file ){ U_CHAR * htfname;
     htfname= kpse_find_file (name, kpse_program_text_format, 0);
     if ( htfname ){
         
#line 14729 "./tex4ht-c.tex"

{                    U_CHAR  * head, * tail, *p;
                     int n;
   
#line 14739 "./tex4ht-c.tex"

n = (int) strlen((char *) htfname);
tail = head = m_alloc(char, n+1);
(IGNORED) strcpy((char *) head, (char *) htfname);
while( n>11 ){
  if( (*tail=='\\') || (*tail=='/') ){
     if( (*tail == *(tail+9)) && (*(tail+1) == 'h')
         && (*(tail+2) == 't') && (*(tail+3) == '-')
         && (*(tail+4) == 'f') && (*(tail+5) == 'o')
         && (*(tail+6) == 'n') && (*(tail+7) == 't')
         && (*(tail+8) == 's') ){
        p = tail + 9;  *(tail + 10) = '\0';  tail += 11;
        while( (*tail != *p) && (*tail != '\0') ){  tail++; }
        break;
  }  }
  tail++; n--;
}


   htfname =  (U_CHAR *) 0;
   
#line 14761 "./tex4ht-c.tex"

for( n = 0 ; (n < cardinality) && !htfname ; n++){
  p = tail;
  while( *p != '\0' ){
                               char * s, *nm;
     s = m_alloc(char, (int) strlen((char *)  head )       +
                       (int) strlen((char *)  fontset[n] ) +
                       (int) strlen((char *)  p )          + 1);
     (IGNORED) strcpy((char *) s, (char *) head);
     (IGNORED) strcat((char *) s, (char *) fontset[n]);
     (IGNORED) strcat((char *) s, (char *) p);
     nm = kpse_find_file (s, kpse_program_text_format, 0);
     free((void *) s);
     if ( nm ){
        htfname = nm; break;
     }
     p++;
     while( (*p != '\\') && (*p != '/')  && (*p != '\0') ){ p++; }
} }


}


         if ( htfname ){
            (IGNORED) printf("(%s)\n",  htfname);
            file= fopen(htfname,READ_TEXT_FLAGS);
     }   }
  }
#endif


   }


    if( file != NULL ){
      
#line 11913 "./tex4ht-c.tex"


#line 11961 "./tex4ht-c.tex"

BOOL is_visited = FALSE;
struct visited_file_rec * v = visited_file;
while( v != (struct visited_file_rec *) 0 ){
  if(  eq_str(v->name, name) ){
    is_visited = TRUE;  break;
  }
  v = v->next;
}


if( !is_visited ){
  
#line 11953 "./tex4ht-c.tex"

struct visited_file_rec * v =  m_alloc(struct visited_file_rec, 1);
v->name = m_alloc(char,  (int) strlen((char *)  name ) + 1 );
(IGNORED) strcpy((char *) v->name, (char *) name);
v->next = visited_file;
visited_file = v;


   while( 1 ){
                int ch;
       do{
          ch = (int) getc(file);
       } while ( (ch == ' ') || (ch == '\n') || (ch == '\t') );
       if( ch == EOF ){ break; }
       do{
          (void)  putc( ch, log_file );
          ch = (int) getc(file);
       } while ( (ch != '\n') && (ch != EOF) );
       (void)  putc( '\n', log_file );
       if( ch == EOF ){ break; }
}  }


      (IGNORED) fclose(file);  break;
    }
}


     }
     free((void *) css_file_name);
} }


   free((void *)  new_font_name);   font_tbl_size++;
}
#ifdef MAXFONTS
 else err_i_int(17, MAXFONTS);
#endif


   }
   
#line 11943 "./tex4ht-c.tex"

while( visited_file != (struct visited_file_rec *) 0 ){
  struct visited_file_rec * v = visited_file;
  visited_file = visited_file->next;
  free((void *) v->name);
  free((void *) v);
}


#line 14698 "./tex4ht-c.tex"


#ifdef KPATHSEA
if( export_str_chars ){
   free((void *) export_str_chars);
   free((void *) fontset);
}
#endif


#line 16411 "./tex4ht-c.tex"

for( i = 0; i<htf_4hf_n; i++){
   free((void *) htf_4hf[i].str);
}
free((void *) htf_4hf);


   if( missing_fonts ) err_i(14);
#ifndef KPATHSEA
   
#line 14342 "./tex4ht-c.tex"

if( cache_files != (FILE *) 0 ){  (IGNORED) fclose(cache_files); }


   
#line 14105 "./tex4ht-c.tex"

if( tex4ht_fls ){
               FILE *in_file, *out_file;
               U_CHAR temp_file[256];
   
#line 14126 "./tex4ht-c.tex"

(IGNORED) strcpy((char *) temp_file, (char *) job_name);
temp_file[job_name_n] = '\0';
temp_file[job_name_n-1] = 'p';
temp_file[job_name_n-2] = 'm';
temp_file[job_name_n-3] = 't';


   
#line 14222 "./tex4ht-c.tex"

if( (out_file = fopen(temp_file, WRITE_TEXT_FLAGS)) == NULL )
{   bad_in_file(temp_file);
} else {
   if( (in_file = fopen(tex4ht_fls_name, READ_TEXT_FLAGS)) != NULL ){
                                                      int ch;
      while( (ch = getc(in_file)) != EOF ) {
         (IGNORED) putc( ch, out_file );
      }
      (IGNORED) fclose(in_file);
   }
   (IGNORED) fclose(out_file);
}


   if( (out_file = fopen(tex4ht_fls_name, WRITE_TEXT_FLAGS)) == NULL )
   {  bad_in_file(tex4ht_fls_name);
   } else {
      if( (in_file = fopen(temp_file, READ_TEXT_FLAGS)) != NULL ){
        
#line 14238 "./tex4ht-c.tex"

              U_CHAR dir[255], prev_dir[255], file[255], *p;
              int ch;
              BOOL is_dir;
              struct cache_file_rec *file_rec, *prev_file_rec;
cur_cache_font = cache_font;
ch = 'n';  prev_dir[0] = '\0';
while( ch != EOF ){
   ch = getc(in_file);
   is_dir = (ch == ' ');
   p = is_dir? dir : file;
   while( ch != '\n' ) {
     if( ch == EOF ) break;
     if( ch != ' ' ) { *p++ = ch; }
     ch = getc(in_file);
   }
   *p = '\0';
   if( is_dir && (dir[0] != '\0') ){
      
#line 14268 "./tex4ht-c.tex"

while( cur_cache_font != (struct cache_font_rec *)0 ){
 if( gt_str(dir,cur_cache_font->dir) ){
    
#line 14278 "./tex4ht-c.tex"

file_rec = cur_cache_font->cache_file;
if( file_rec ){
   if( !eq_str( prev_dir, cur_cache_font->dir) ){
      (IGNORED) fprintf(out_file, " %s\n", cur_cache_font->dir);
      (IGNORED) strcpy((char *) prev_dir, (char *) dir);
   }
   cur_cache_font->cache_file = (struct cache_file_rec *) 0;
   while( file_rec ) {
     prev_file_rec = file_rec;
     file_rec = file_rec->next;
     (IGNORED) fprintf(out_file, "%s\n",  prev_file_rec->file);
     free((void *) prev_file_rec );
   }
}


 } else break;
 cur_cache_font = cur_cache_font->next;
}


      (IGNORED) fprintf(out_file," %s\n", dir);
      (IGNORED) strcpy((char *) prev_dir, (char *) dir);
   } else if( !is_dir && (file[0] != '\0') ){
      
#line 14298 "./tex4ht-c.tex"

if( cur_cache_font != (struct cache_font_rec *)0 ){
 if( eq_str(dir,cur_cache_font->dir) ){
    file_rec = cur_cache_font->cache_file;
    while( file_rec ) {
      if( gt_str(file_rec->file,file) ){ break; }
      else if( gt_str(file,file_rec->file) ){
          (IGNORED) fprintf(out_file, "%s\n", file_rec->file);
      }
      prev_file_rec = file_rec;
      file_rec = file_rec->next;
      free((void *) prev_file_rec );
    }
    cur_cache_font->cache_file = file_rec;
 }
}


      (IGNORED) fprintf(out_file,"%s\n", file);
   }
}

#line 14318 "./tex4ht-c.tex"

while( cur_cache_font != (struct cache_font_rec *)0 ){
   
#line 14278 "./tex4ht-c.tex"

file_rec = cur_cache_font->cache_file;
if( file_rec ){
   if( !eq_str( prev_dir, cur_cache_font->dir) ){
      (IGNORED) fprintf(out_file, " %s\n", cur_cache_font->dir);
      (IGNORED) strcpy((char *) prev_dir, (char *) dir);
   }
   cur_cache_font->cache_file = (struct cache_file_rec *) 0;
   while( file_rec ) {
     prev_file_rec = file_rec;
     file_rec = file_rec->next;
     (IGNORED) fprintf(out_file, "%s\n",  prev_file_rec->file);
     free((void *) prev_file_rec );
   }
}


   cur_cache_font = cur_cache_font->next;
}




        (IGNORED) fclose(in_file);
      }
      (IGNORED) fclose(out_file);
}  }


#endif
   
#line 10265 "./tex4ht-c.tex"

if( html_font ){
  while( fonts_n-- )  free((void *)  html_font[fonts_n].name);
  free((void *)  html_font );
}


#line 13716 "./tex4ht-c.tex"

while( (cur_cache_font = cache_font) != (struct cache_font_rec *)0 ){
   cache_font = cache_font->next;
   free((void *) cur_cache_font->dir );
   free((void *) cur_cache_font );        }


}


  (IGNORED) fclose(dot_file);
}

#line 2268 "./tex4ht-c.tex"

(IGNORED) fseek(dvi_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
ch = get_noop();
if( ch != 
#line 17475 "./tex4ht-c.tex"

247 
 )   bad_dvi;
if( ((int) get_char()) != version_id ) bad_dvi;
(void) get_unt(4);     
(void) get_unt(4);     
 (void) get_unt(4);
for( i= get_char(); i>0; i-- ) ch = get_char();



#line 2318 "./tex4ht-c.tex"

{
  dis_pages = unread_pages;
  while( unread_pages-- ){
    (IGNORED) printf("[%d", dis_pages - unread_pages);
    
#line 2343 "./tex4ht-c.tex"

x_val = dx_1 = dx_2 = 0;  max_x_val = -10000; 
y_val = max_y_val = prev_y_val = dy_1 = dy_2 = 0;


    if( get_noop() != 
#line 17320 "./tex4ht-c.tex"

139 
 )  bad_dvi;
    for( i = 1; i<45; i++ )
      if( get_char() == EOF )   bad_dvi;
    while( (ch = get_char()) != 
#line 17323 "./tex4ht-c.tex"

140 
 ){
      
#line 2930 "./tex4ht-c.tex"

{       register int ch_1;
   ch_1 = ch;
   
#line 6462 "./tex4ht-c.tex"

if( stack[stack_n].halign_on )
{
   switch( ch ){
       case                
#line 17374 "./tex4ht-c.tex"

157 
: { ; }
       case                
#line 17377 "./tex4ht-c.tex"

158 
: { ; }
       case                
#line 17380 "./tex4ht-c.tex"

159 
: { ; }
       case                
#line 17383 "./tex4ht-c.tex"

160 
: { ; }
       case             
#line 17386 "./tex4ht-c.tex"

161 
: { ; }
       case 
#line 17389 "./tex4ht-c.tex"

162 
: { ; }
       case 
#line 17392 "./tex4ht-c.tex"

163  
: { ; }
       case 
#line 17395 "./tex4ht-c.tex"

164 
: { ; }
       case 
#line 17398 "./tex4ht-c.tex"

165 
: { ; }
       case             
#line 17401 "./tex4ht-c.tex"

166  
: { ; }
       case 
#line 17404 "./tex4ht-c.tex"

167 
: { ; }
       case 
#line 17407 "./tex4ht-c.tex"

168 
: { ; }
       case 
#line 17410 "./tex4ht-c.tex"

169 
: { ; }
       case 
#line 17413 "./tex4ht-c.tex"

170 
: { ; }
       case        
#line 17309 "./tex4ht-c.tex"

132 
: { ; }
       case      
#line 17314 "./tex4ht-c.tex"

137 
: { ; }
       case                  
#line 17429 "./tex4ht-c.tex"

235  
: { ; }
       case                 
#line 17432 "./tex4ht-c.tex"

236  
: { ; }
       case                 
#line 17435 "./tex4ht-c.tex"

237  
: { ; }
       case                     
#line 17438 "./tex4ht-c.tex"

238  
: { ; }
       case                    
#line 17326 "./tex4ht-c.tex"

141 
:
                                              { break; }
     default: { 
#line 6498 "./tex4ht-c.tex"

print_f( stack[stack_n].halign[1]->str );
stack[stack_n].halign_on = FALSE;

 }
}  }


   
#line 3197 "./tex4ht-c.tex"

if( (ch > 127) && (ch < 137) && (ch != 
#line 17309 "./tex4ht-c.tex"

132 
) ){
   ch_1 = (int) get_unt( (ch - (ch>132)) % 4 +1);
}


   
#line 7655 "./tex4ht-c.tex"

if( group_dvi ){
  if( ( ch < 132 ) ||
      ( (ch > 127) && (ch < 137) && (ch != 
#line 17309 "./tex4ht-c.tex"

132 
 ) )
    ){
       ch_id++;
       back_token = back_insert ( back_token, ch_id);
} }


   
#line 2626 "./tex4ht-c.tex"

if( trace_dvi_C && !in_trace_char ){
   if( (ch < 137) && (ch != 
#line 17309 "./tex4ht-c.tex"

132 
) ){
     in_trace_char = TRUE; block_start = TRUE;
} }
else if ( in_trace_char ){
  if( !trace_dvi_C || (ch > 136) || (ch == 
#line 17309 "./tex4ht-c.tex"

132 
) ){
   in_trace_char = FALSE;
} }


   
#line 2587 "./tex4ht-c.tex"

if( span_on && !in_span_ch  && !ignore_chs && !in_accenting
            && (default_font != font_tbl[cur_fnt].num) ){
  if(  (ch < 137) && (ch != 
#line 17309 "./tex4ht-c.tex"

132 
) ){
    in_span_ch = TRUE; start_span = TRUE;
} }
else if ( in_span_ch ){
  if( !span_on ||
     (ch == 
#line 17309 "./tex4ht-c.tex"

132 
) ||
     ((136 < ch) && (ch < 
#line 17332 "./tex4ht-c.tex"

143 
)) ||
     (ch > 
#line 17371 "./tex4ht-c.tex"

156 
)
  ){
    in_span_ch = FALSE;
    if( *end_span[0] ){
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       (IGNORED) fprintf(cur_o_file, "%s", end_span[0]);
    }
} }


   
#line 12212 "./tex4ht-c.tex"

if( in_accenting ){
  
#line 12244 "./tex4ht-c.tex"

                                               long int width;
if( i_accent_template ){
  (IGNORED) fprintf(cur_o_file, "%s", i_accent_second); }
needs_end_accent = (needs_accent_sym == 2 * TRUE);
if( needs_end_accent && t_accent_template )
{  
#line 12201 "./tex4ht-c.tex"

needs_accented_sym++;

 }
else if( m_accent_template )
{  
#line 12201 "./tex4ht-c.tex"

needs_accented_sym++;

  stack[stack_n-1].accented = TRUE; }
needs_accent_sym = FALSE;
width = (INTEGER)( 
#line 9837 "./tex4ht-c.tex"

design_size_to_pt( *(font_tbl[cur_fnt].wtbl
                     +  (int) (
#line 9845 "./tex4ht-c.tex"

*(font_tbl[cur_fnt].char_wi +  (int)
   ( ch - font_tbl[cur_fnt].char_f)% 256)

) )
                 )
* (double) font_tbl[cur_fnt].scale

 );
if( needs_end_accent ){  needs_end_accent = x_val + 9 * width / 10; }
in_accenting = FALSE;


} else if( 
#line 12269 "./tex4ht-c.tex"

needs_accent_sym && (ch < 128)

 ){
  if( needs_accent_sym ){
                                        BOOL needs_end_accent;
    needs_end_accent = (needs_accent_sym == 2 * TRUE);
    if( needs_end_accent && t_accent_template ){
      (IGNORED) fprintf(cur_o_file, "%s%s%s%d%s%d%s",
         t_accent_first,   font_tbl[cur_fnt].family_name,
         t_accent_second, ch, t_accent_third,
         font_tbl[cur_fnt].accent[ch]?
           font_tbl[cur_fnt].accent_array[font_tbl[cur_fnt].accent[ch]-1]
           : 0,
         t_accent_fourth);
    } else if( m_accent_template ){
      (IGNORED) fprintf(cur_o_file, "%s%s%s%d%s%d%s",
         m_accent_first,   font_tbl[cur_fnt].family_name,
         m_accent_second, ch, m_accent_third,
         font_tbl[cur_fnt].accent[ch]?
           font_tbl[cur_fnt].accent_array[font_tbl[cur_fnt].accent[ch]-1]
           : 0,
         m_accent_fourth);
    }
    if( i_accent_template ){
      (IGNORED) fprintf(cur_o_file, "%s", i_accent_first); }
    in_accenting = TRUE;
  }
}


   if( ch < 132 )  {
      x_val += math_class_on? 
#line 12651 "./tex4ht-c.tex"

set_ch_class(ch_1)


                            : insert_ch(ch_1);       
      if(  max_x_val < x_val ) max_x_val = x_val;
   } else switch( ch ) {
      case 133: case 134: case 135: case 136: {
           INTEGER w;
         w = math_class_on?  
#line 12651 "./tex4ht-c.tex"

set_ch_class(ch_1)

 : insert_ch(ch_1);
         max_x_val = ( x_val + w > max_x_val )?  x_val + w : max_x_val;
         break;
      }
      
#line 3036 "./tex4ht-c.tex"

case 
#line 17332 "./tex4ht-c.tex"

143 
: {;}
case 
#line 17335 "./tex4ht-c.tex"

144 
: {;}
case 
#line 17338 "./tex4ht-c.tex"

145 
: {;}
case 
#line 17341 "./tex4ht-c.tex"

146 
: {
   try_new_line();
   (void) move_x((INTEGER) get_int(ch - 
#line 17332 "./tex4ht-c.tex"

143 
 + 1 ));
   break; }


#line 3046 "./tex4ht-c.tex"

case 
#line 17344 "./tex4ht-c.tex"

147 
: {
   (void) move_x( dx_1 ); break; }
case 
#line 17347 "./tex4ht-c.tex"

148 
: {;}
case 
#line 17350 "./tex4ht-c.tex"

149 
: {;}
case 
#line 17353 "./tex4ht-c.tex"

150 
: {;}
case 
#line 17356 "./tex4ht-c.tex"

151 
: {
   try_new_line();
   dx_1 = move_x((INTEGER) get_int(ch - 
#line 17344 "./tex4ht-c.tex"

147 
 ));
   break; }


#line 3058 "./tex4ht-c.tex"

case 
#line 17359 "./tex4ht-c.tex"

152 
: {
   (void) move_x( dx_2 ); break; }
case 
#line 17362 "./tex4ht-c.tex"

153  
: {;}
case 
#line 17365 "./tex4ht-c.tex"

154  
: {;}
case 
#line 17368 "./tex4ht-c.tex"

155 
: {;}
case 
#line 17371 "./tex4ht-c.tex"

156 
: {
   try_new_line();
   dx_2 = move_x((INTEGER) get_int(ch - 
#line 17359 "./tex4ht-c.tex"

152 
 ));
   break; }


#line 3143 "./tex4ht-c.tex"

case 
#line 17374 "./tex4ht-c.tex"

157 
: {;}
case 
#line 17377 "./tex4ht-c.tex"

158 
: {;}
case 
#line 17380 "./tex4ht-c.tex"

159 
: {;}
case 
#line 17383 "./tex4ht-c.tex"

160 
: {
  (void) move_y( (INTEGER) get_int(ch - 
#line 17374 "./tex4ht-c.tex"

157 
 + 1 ));
  break; }


#line 3152 "./tex4ht-c.tex"

case 
#line 17386 "./tex4ht-c.tex"

161 
: { (void) move_y( dy_1 );   break; }
case 
#line 17389 "./tex4ht-c.tex"

162 
: {;}
case 
#line 17392 "./tex4ht-c.tex"

163  
: {;}
case 
#line 17395 "./tex4ht-c.tex"

164 
: {;}
case 
#line 17398 "./tex4ht-c.tex"

165 
: {
  dy_1 = move_y( (INTEGER) get_int(ch - 
#line 17386 "./tex4ht-c.tex"

161 
 ));
  break; }


#line 3162 "./tex4ht-c.tex"

case 
#line 17401 "./tex4ht-c.tex"

166  
: { (void) move_y( dy_2 );   break; }
case 
#line 17404 "./tex4ht-c.tex"

167 
: {;}
case 
#line 17407 "./tex4ht-c.tex"

168 
: {;}
case 
#line 17410 "./tex4ht-c.tex"

169 
: {;}
case 
#line 17413 "./tex4ht-c.tex"

170 
: {
  dy_2 = move_y( (INTEGER) get_int(ch - 
#line 17401 "./tex4ht-c.tex"

166  
 ));
  break; }


#line 3586 "./tex4ht-c.tex"

case 
#line 17309 "./tex4ht-c.tex"

132 
: {
   (void) rule_x( TRUE );   break;
}
case 
#line 17314 "./tex4ht-c.tex"

137 
: {
   (void) rule_x( FALSE );  break;
}


#line 3759 "./tex4ht-c.tex"


#line 3764 "./tex4ht-c.tex"

case 
#line 17460 "./tex4ht-c.tex"

246 
:   (void) get_char();
case 
#line 17457 "./tex4ht-c.tex"

245 
:   (void) get_char();
case 
#line 17454 "./tex4ht-c.tex"

244 
:   (void) get_char();
case 
#line 17451 "./tex4ht-c.tex"

243 
: {
  for( i=0; i<14; i++ ){ ch = get_char(); }
  for( i=ch + get_char(); i>0; i--) (void) get_char();
  break;
}




#line 3776 "./tex4ht-c.tex"

case 
#line 17490 "./tex4ht-c.tex"

252

:
   if(  version_id == 
#line 2292 "./tex4ht-c.tex"

10

 ){
     
#line 9230 "./tex4ht-c.tex"

        unsigned short flags;
for( i=0; i<8; i++ ){ ch = get_char(); }
flags = (INTEGER) get_unt(2);
for( i = (INTEGER) get_unt(1) 
       + (INTEGER) get_unt(1) 
       + (INTEGER) get_unt(1) 
    ; i>0
    ; i-- ){ ch = get_char(); }
if( flags & 
#line 8955 "./tex4ht-c.tex"

0x0200

 ){ (void) get_unt(4); }
if( flags & 
#line 8959 "./tex4ht-c.tex"

0x0800

 ){
   int n =  (INTEGER) get_unt(2);
   int i;
   for (i = 0; i < n; ++i) {
         (void) get_unt(4);
   }
   for (i = 0; i < n; ++i) {
      (void) get_int(4);
}  }


      break;
   }
case 
#line 17496 "./tex4ht-c.tex"

253

:
   if(  version_id == 
#line 2292 "./tex4ht-c.tex"

10

 ){
     
#line 9253 "./tex4ht-c.tex"

doGlyphArray(TRUE);


      break;
   }
case 
#line 17501 "./tex4ht-c.tex"

254

:
   if(  version_id == 
#line 2292 "./tex4ht-c.tex"

10

 ){
     
#line 9258 "./tex4ht-c.tex"

doGlyphArray(FALSE);


      break;
   }


#line 3795 "./tex4ht-c.tex"

default: {
  if( (ch < 
#line 17417 "./tex4ht-c.tex"

171  
) || (ch > 
#line 17423 "./tex4ht-c.tex"

234  
) ) {
     bad_char(ch);
  } else { cur_fnt = ch - 
#line 17417 "./tex4ht-c.tex"

171  
;
         
#line 9309 "./tex4ht-c.tex"

cur_fnt = search_font_tbl( cur_fnt );
word_sp = 
#line 9911 "./tex4ht-c.tex"

design_size_to_pt( font_tbl[cur_fnt].word_sp )
                   * (double) font_tbl[cur_fnt].scale

;

 }
  break;
}


#line 3805 "./tex4ht-c.tex"

case 
#line 17429 "./tex4ht-c.tex"

235  
 :
case 
#line 17432 "./tex4ht-c.tex"

236  
:
case 
#line 17435 "./tex4ht-c.tex"

237  
:
case 
#line 17438 "./tex4ht-c.tex"

238  
    : {
        INTEGER n;
   n = ch - 
#line 17429 "./tex4ht-c.tex"

235  
 + 1;
   cur_fnt = (int)  ((n==4)? get_int(4) : get_unt((int) n));
   
#line 9309 "./tex4ht-c.tex"

cur_fnt = search_font_tbl( cur_fnt );
word_sp = 
#line 9911 "./tex4ht-c.tex"

design_size_to_pt( font_tbl[cur_fnt].word_sp )
                   * (double) font_tbl[cur_fnt].scale

;


   break; }


#line 3841 "./tex4ht-c.tex"

case 
#line 17441 "./tex4ht-c.tex"

239 
: {;}
case 
#line 17443 "./tex4ht-c.tex"

240 
: {;}
case 
#line 17445 "./tex4ht-c.tex"

241 
: {;}
case 
#line 17447 "./tex4ht-c.tex"

242 
: {  
#line 3857 "./tex4ht-c.tex"

long int special_n;
  
#line 12281 "./tex4ht-c.tex"

if( needs_end_accent && t_accent_template ){
   
#line 2609 "./tex4ht-c.tex"

if( span_on && in_span_ch ){
   if( *end_span[0] ){
       in_span_ch = FALSE;
       
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


       (IGNORED) fprintf(cur_o_file, "%s", end_span[0]);
}  }


   (IGNORED) fprintf(cur_o_file, "%s", t_accent_fifth);
   needs_end_accent = FALSE; 
#line 12424 "./tex4ht-c.tex"

needs_accented_sym--;


}


  if( tex4ht_special( &ch, &special_n) )  {    int  sv; sv = ch;
     special_on = TRUE;  
#line 3944 "./tex4ht-c.tex"

try_new_line();
switch( ch ){
  case '*': { 
#line 4585 "./tex4ht-c.tex"

if( special_n ){
  special_n--;
  switch ( get_char() ){
    case '<': { 
#line 4689 "./tex4ht-c.tex"

    U_CHAR name[256];
    int i=0;
    FILE* file;
name[(int) special_n] = '\0';
while(  special_n-- > 0 ){  name[i++] = get_char(); }
file  = f_open(name, READ_TEXT_FLAGS);
if( file ) {
  
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


  while( (ch = getc(file)) >=0  ){
    (IGNORED)  put_4ht_ch(ch,cur_o_file);
  }
  (IGNORED) fclose(file);
} else { warn_i_str( 1, name ); }

    break; }
    case '>': { 
#line 4394 "./tex4ht-c.tex"

if( special_n > 0 ){
  
#line 4504 "./tex4ht-c.tex"

        static struct files_rec *p, *q;
        U_CHAR name[256];
        int i;

#line 4514 "./tex4ht-c.tex"

i = 0;
name[(int) special_n] = '\0';
while(  special_n-- > 0 ){  name[i++] = get_char(); }
for( p = opened_files; p != (struct files_rec*) 0;  p = p->next ){
  if( eq_str(p->name, name) ){ break; }
}


if( p != (struct files_rec*) 0 ){
  
#line 4525 "./tex4ht-c.tex"

for( q = p; q->next != (struct files_rec*) 0;  q = q->next ){ }
if( q != p ){
  q->next = p;
  (p->next)->prev = p->prev;
  if( opened_files == p ){ opened_files = p->next; }
  else { (p->prev)->next = p->next; }
  p->prev = q;
  p->next = (struct files_rec*) 0;
}


}


} else {
  
#line 4470 "./tex4ht-c.tex"


#line 4474 "./tex4ht-c.tex"

        static struct files_rec *p, *q;
for( p = opened_files; p != (struct files_rec*) 0;  p = p->next ){
  if( (p->file == cur_o_file) && p->prev_file ){
    
#line 4486 "./tex4ht-c.tex"

for( q = opened_files; q != (struct files_rec*) 0;  q = q->next ){
  if( q->file == p->prev_file ){
     break;
  }
}
if( q == (struct files_rec*) 0 ){
   warn_i_str(51,q->name);
   break;
}


    cur_o_file = p->prev_file;
    p->prev_file = (FILE *) 0;
    break;
  }
}




}

    break; }
    case '!': { 
#line 4608 "./tex4ht-c.tex"

              U_CHAR name[256], ch;
              int i=0, n;
              struct sys_call_rec *p;
              BOOL flag;
name[(int) special_n] = '\0';
while(  special_n-- > 0 ){  name[i++] = get_char(); }
(IGNORED) printf("System call: %s\n", name);

#line 4654 "./tex4ht-c.tex"

flag = FALSE;
p = system_calls;
while( p ){
  if( (n = (int) strlen((char *) p->filter)) == 1 ) {
      flag = flag || (*(p->filter) == '*');
  } if(  strlen((char *) name) >= (unsigned int) n ) {
      ch = name[n]; name[n] = '\0';
      flag = flag || eq_str(p->filter,name);
      name[n] = ch;
  }
  p = p->next;
}


if( flag ){
  (IGNORED) printf("System return: %d\n",
                    system_yes?  (int) system(name) : -1 );
} else { (IGNORED) printf("No permission for system call\n"); }

    break; }
    case '^': { 
#line 12155 "./tex4ht-c.tex"

special_n--;
switch ( get_char() ){
  case 't': { if( special_n ){
                
#line 12317 "./tex4ht-c.tex"

(IGNORED) get_open_accent(&t_accent_template,
            &t_accent_first, &t_accent_second,
            &t_accent_third, &t_accent_fourth,
            &t_accent_fifth, &special_n);


              } else { 
#line 12193 "./tex4ht-c.tex"

needs_accent_sym = TRUE * 2;

 }
              break;
            }
  case 'm': { if( special_n ){
                
#line 12324 "./tex4ht-c.tex"

(IGNORED) get_open_accent(&m_accent_template,
            &m_accent_first, &m_accent_second,
            &m_accent_third, &m_accent_fourth,
            &m_accent_fifth, &special_n);


              } else { 
#line 12197 "./tex4ht-c.tex"

needs_accent_sym = TRUE;

 }
              break;
            }
  case 'a': { 
#line 12428 "./tex4ht-c.tex"

(IGNORED) get_open_accent(&a_accent_template,
            &a_accent_first, &a_accent_second,
            &a_accent_third, &a_accent_fourth,
            &a_accent_fifth, &special_n);


              break;
            }
  case 'i': { 
#line 12444 "./tex4ht-c.tex"

(IGNORED) get_open_accent(&i_accent_template,
            &i_accent_first, &i_accent_second,
            &i_accent_third, &i_accent_fourth,
            &i_accent_fifth, &special_n);


              break;
            }
   default: { 
#line 17088 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();

 }
}

    break; }
    case '@': { 
#line 6239 "./tex4ht-c.tex"

                                int i;
i = 0;
special_n--;
switch ( get_char() ){
  case '8': { i++; }
  case '7': { i++; }
  case '6': { i++; }
  case '5': { i++; }
  case '4': { i++; }
  case '3': { i++; }
  case '2': { i++; }
  case '1': { 
#line 6313 "./tex4ht-c.tex"

if( halign[i]->refs == 1 ){
   free((void *) halign[i]->str );
} else {
   (halign[i]->refs)--;
   halign[i]         = m_alloc(struct halign_rec, 1);
   halign[i]->refs   = 1;
}
halign[i]->str = get_str( (int) special_n );
special_n=0;

 break; }
  case '/': { if( special_n ){
                
#line 17088 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();


              } else { 
#line 6492 "./tex4ht-c.tex"

if( stack[stack_n].halign_on )
{  
#line 6498 "./tex4ht-c.tex"

print_f( stack[stack_n].halign[1]->str );
stack[stack_n].halign_on = FALSE;

  }

 }
              break;
            }
  case '&': { i++; }
  case '@': { i++;
              if( special_n ){
                
#line 17088 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();


              } else {
                
#line 6370 "./tex4ht-c.tex"

new_halign = i * TRUE;


              }
              break;
            }
   default: { 
#line 17088 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();

 }
}

    break; }
    case '=': { 
#line 4149 "./tex4ht-c.tex"

       char *str, *repl;
       struct hcode_repl_typ *p, *q;
       BOOL flag;
if( special_n ){
   repl = str =  m_alloc(char, (int) special_n + 1);
   while( special_n-- > 0 ){
      *str = get_char(); str++;
   }
   *str = 0;
   
#line 4176 "./tex4ht-c.tex"

if( hcode_repl != (struct hcode_repl_typ*) 0 ){
   if( *(hcode_repl->str) == *repl ){
      p = hcode_repl;
      hcode_repl = hcode_repl->next;
      free((void *) p->str);
      free((void *) p);
   } else {
     p = hcode_repl;
     while( TRUE ){
       q = p->next;
       if( q ==  (struct hcode_repl_typ*) 0 ){ break; }
       if( *(q->str) == *repl ){
          p->next = q->next;
          free((void *) q->str);
          free((void *) q);
          break;
       }
       p = q;
} }  }


   
#line 4199 "./tex4ht-c.tex"

flag = *repl != *(repl+1);
if( !flag ){ flag = *(repl+2) != 0; }
if( flag ){
   p = (struct hcode_repl_typ *) m_alloc(struct hcode_repl_typ, 1);
   p->str = repl;
   p->next = hcode_repl;
   hcode_repl = p;
}


} else {
  
#line 4166 "./tex4ht-c.tex"

while( hcode_repl != (struct hcode_repl_typ*) 0 ){
  p = hcode_repl;
  hcode_repl = hcode_repl->next;
  free((void *) p->str);
  free((void *) p);
}


}

    break; }
     default: { 
#line 17088 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();

 }
  }
} else { 
#line 17093 "./tex4ht-c.tex"

;

 }

  break; }
  case '@': { 
#line 4240 "./tex4ht-c.tex"

     int code, digit;
special_n--;
switch ( code = get_char() ){
  case '%': { 
#line 2511 "./tex4ht-c.tex"

if( special_n>1 ) {
   special_n--;
   if (  get_char() == '%' ) {
      if( special_n>2 ) { 
#line 2648 "./tex4ht-c.tex"

     U_CHAR  type, ch, *p, *q, *pp=0, *qq=0, pre[256], post[256];
special_n -= 2;   type = get_char();  ch = get_char();
p = pre;
while( special_n-- > 0 ) {
  if ( (*(p++)=get_char() ) == ch ) { p--; break; }
}
*p = '\0';
p = post;
while( special_n-- > 0 ) { *(p++)=get_char(); }    *p='\0';


#line 2661 "./tex4ht-c.tex"

p = m_alloc(char, 1 + (int) strlen((char *) pre));
(IGNORED) strcpy((char *) p, (char *) pre );
q = m_alloc(char, 1 + (int) strlen((char *) post));
(IGNORED) strcpy((char *) q, (char *) post );


#line 2670 "./tex4ht-c.tex"

switch ( type ){
   case 'P': {
     pp = trace_dvi_del_P;      trace_dvi_del_P = p;
     qq = end_trace_dvi_del_P;  end_trace_dvi_del_P = q;
     break; }
   case 'C': {
     pp = trace_dvi_del_C;      trace_dvi_del_C = p;
     qq = end_trace_dvi_del_C;  end_trace_dvi_del_C = q;
     break; }
   case 'V': {
     pp = trace_dvi_del_V;      trace_dvi_del_V = p;
     qq = end_trace_dvi_del_V;  end_trace_dvi_del_V = q;
     break; }
   case 'H': {
     pp = trace_dvi_del_H;      trace_dvi_del_H = p;
     qq = end_trace_dvi_del_H;  end_trace_dvi_del_H = q;
     break; }
   case 'R': {
     pp = trace_dvi_del_R;      trace_dvi_del_R = p;
     qq = end_trace_dvi_del_R;  end_trace_dvi_del_R = q;
     break; }
   case 'p': {
     pp = trace_dvi_del_p;      trace_dvi_del_p = p;
     qq = end_trace_dvi_del_p;  end_trace_dvi_del_p = q;
     break; }
   case 'c': {
     pp = trace_dvi_del_c;      trace_dvi_del_c = p;
     qq = end_trace_dvi_del_c;  end_trace_dvi_del_c = q;
     break; }
   case 'v': {
     pp = trace_dvi_del_v;      trace_dvi_del_v = p;
     qq = end_trace_dvi_del_v;  end_trace_dvi_del_v = q;
     break; }
   case 'h': {
     pp = trace_dvi_del_h;      trace_dvi_del_h = p;
     qq = end_trace_dvi_del_h;  end_trace_dvi_del_h = q;
     break; }
   case 'r': {
     pp = trace_dvi_del_r;      trace_dvi_del_r = p;
     qq = end_trace_dvi_del_r;  end_trace_dvi_del_r = q;
     break; }
  default: { ; }
}
free((void *)  pp);
free((void *)  qq);

 }
      else { 
#line 17088 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();

 }
   } else { 
#line 17088 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();

 }
} else if( special_n ) {
  special_n--;
  switch ( get_char() ){
     case 'P': { trace_dvi_P++; break; }
     case 'C': { trace_dvi_C++; break; }
     case 'V': { trace_dvi_V++; break; }
     case 'H': { trace_dvi_H++; break; }
     case 'R': { trace_dvi_R++; break; }
     case 'p': { trace_dvi_P--; break; }
     case 'c': { trace_dvi_C--; break; }
     case 'v': { trace_dvi_V--; break; }
     case 'h': { trace_dvi_H--; break; }
     case 'r': { trace_dvi_R--; break; }
      default: { ; }
} }

    break; }
  case '@': { verb_ch = !verb_ch;  break; }
  case '/': { 
#line 17176 "./tex4ht-c.tex"

trace_special = !trace_special;

  break; }
  case 'e': { 
#line 17153 "./tex4ht-c.tex"

if( err_mark ){ free((void *)  err_mark); }
if( special_n ){
   err_mark = get_str( (int) special_n );  special_n=0;
} else { err_mark = (char *) 0; }

  break; }
  case '!': { 
#line 3262 "./tex4ht-c.tex"

while( recover_spaces-- ){ text_on=TRUE;  put_char(' '); }
recover_spaces = 0;

  break; }
  case '(': { 
#line 3268 "./tex4ht-c.tex"

ignore_spaces++;

  break; }
  case ')': { 
#line 3272 "./tex4ht-c.tex"

ignore_spaces--;

  break; }
  case '[': { 
#line 3290 "./tex4ht-c.tex"

if( special_n ){
                  
#line 3278 "./tex4ht-c.tex"

U_CHAR *unhskip_mark;
long retract_addr;
BOOL unhskip;
int cr_fnt, ch, unskip_depth;


   cr_fnt = cur_fnt;
   unskip_depth = 0;
   unhskip_mark = get_str( (int) special_n );  special_n=0;
   retract_addr = ftell(dvi_file);
   
#line 3304 "./tex4ht-c.tex"

unhskip = TRUE;
while( unhskip ){
  if( (ch = get_char()) >= 128 ) {
  switch( ch ){
    
#line 7080 "./tex4ht-c.tex"

case 
#line 17460 "./tex4ht-c.tex"

246 
: (void) get_char();
case 
#line 17457 "./tex4ht-c.tex"

245 
: (void) get_char();
case 
#line 17454 "./tex4ht-c.tex"

244 
: (void) get_char();
case 
#line 17451 "./tex4ht-c.tex"

243 
: {    int i;
  for( i=14; i; i-- ){  ch = get_char(); }
  i = ch +  get_char();
  (IGNORED) fseek(dvi_file, (long) i, 
#line 2177 "./tex4ht-c.tex"

1
);
  break;  }


    
#line 7151 "./tex4ht-c.tex"

case 
#line 17309 "./tex4ht-c.tex"

132 
:
case 
#line 17314 "./tex4ht-c.tex"

137 
:{
  (IGNORED) fseek(dvi_file, 8L, 
#line 2177 "./tex4ht-c.tex"

1
);
  break;
}


#line 7159 "./tex4ht-c.tex"

case   
#line 17320 "./tex4ht-c.tex"

139 
: {
  (IGNORED) fseek(dvi_file, 44L, 
#line 2177 "./tex4ht-c.tex"

1
);  break; }


#line 7164 "./tex4ht-c.tex"

case 
#line 17332 "./tex4ht-c.tex"

143 
: case 
#line 17335 "./tex4ht-c.tex"

144 
:
case 
#line 17338 "./tex4ht-c.tex"

145 
: case 
#line 17341 "./tex4ht-c.tex"

146 
: {
    (IGNORED) (get_int( ch - 
#line 17332 "./tex4ht-c.tex"

143 
 + 1 ));  break; }
case 
#line 17347 "./tex4ht-c.tex"

148 
:
case 
#line 17350 "./tex4ht-c.tex"

149 
:
case 
#line 17353 "./tex4ht-c.tex"

150 
:
case 
#line 17356 "./tex4ht-c.tex"

151 
: {
    (IGNORED) (get_int( ch - 
#line 17347 "./tex4ht-c.tex"

148 
 + 1));
    break;  }
case 
#line 17362 "./tex4ht-c.tex"

153  
:
case 
#line 17365 "./tex4ht-c.tex"

154  
:
case 
#line 17368 "./tex4ht-c.tex"

155 
:
case 
#line 17371 "./tex4ht-c.tex"

156 
: {
    (IGNORED) (get_int( ch - 
#line 17362 "./tex4ht-c.tex"

153  
 + 1));
    break;  }
case 
#line 17374 "./tex4ht-c.tex"

157 
: case 
#line 17377 "./tex4ht-c.tex"

158 
:
case 
#line 17380 "./tex4ht-c.tex"

159 
: case 
#line 17383 "./tex4ht-c.tex"

160 
: {
    (IGNORED) (get_int( ch - 
#line 17374 "./tex4ht-c.tex"

157 
 + 1));
    break; }
case 
#line 17389 "./tex4ht-c.tex"

162 
:
case 
#line 17392 "./tex4ht-c.tex"

163  
:
case 
#line 17395 "./tex4ht-c.tex"

164 
:
case 
#line 17398 "./tex4ht-c.tex"

165 
: {
    (IGNORED) (get_int( ch - 
#line 17389 "./tex4ht-c.tex"

162 
 + 1));
    break; }
case 
#line 17404 "./tex4ht-c.tex"

167 
:
case 
#line 17407 "./tex4ht-c.tex"

168 
:
case 
#line 17410 "./tex4ht-c.tex"

169 
:
case 
#line 17413 "./tex4ht-c.tex"

170 
: {
    (IGNORED) (get_int( ch - 
#line 17404 "./tex4ht-c.tex"

167 
 + 1));
    break; }


#line 7200 "./tex4ht-c.tex"

case 
#line 17344 "./tex4ht-c.tex"

147 
:
case 
#line 17359 "./tex4ht-c.tex"

152 
:
case 
#line 17386 "./tex4ht-c.tex"

161 
:
case 
#line 17401 "./tex4ht-c.tex"

166  
:
   { break; }


    
#line 3342 "./tex4ht-c.tex"

case 128: case 129: case 130: case 131: case 133:
case 134: case 135: case 136: {
  (void) get_unt( (ch-(ch>132)) % 4 +1);
  break;
}


    
#line 3355 "./tex4ht-c.tex"

case 
#line 17326 "./tex4ht-c.tex"

141 
:
case 
#line 17329 "./tex4ht-c.tex"

142 
: {  break; }


    
#line 3364 "./tex4ht-c.tex"

case 
#line 17441 "./tex4ht-c.tex"

239 
:  case 
#line 17443 "./tex4ht-c.tex"

240 
:
case 
#line 17445 "./tex4ht-c.tex"

241 
:  case 
#line 17447 "./tex4ht-c.tex"

242 
: {  long int i;
  if( tex4ht_special( &ch, &i ) ){    char *mark;
    mark = get_str( (int) i );
    if( i ){
      if( (ch=='@') && eq_str(mark+1,unhskip_mark) ){
         switch( *mark ){
           case '[': { unskip_depth++; break; }
           case ']': {
                unhskip = !(--unskip_depth);
                break;
             }
            default: { ; }
    }  }  }
  }else{ 
#line 7016 "./tex4ht-c.tex"

   U_CHAR *ch;
ch = special_hd + 4;
while( *ch ){   ch++; }
(IGNORED) fseek(dvi_file, (long) i, 
#line 2177 "./tex4ht-c.tex"

1
);

  }
  break;
}


    
#line 7093 "./tex4ht-c.tex"

case  
#line 17429 "./tex4ht-c.tex"

235  
:
case 
#line 17432 "./tex4ht-c.tex"

236  
:
case 
#line 17435 "./tex4ht-c.tex"

237  
:
case     
#line 17438 "./tex4ht-c.tex"

238  
: {
                              INTEGER n;
  n = ch - 
#line 17429 "./tex4ht-c.tex"

235  
 + 1;
  cr_fnt = (int)  ((n==4)? get_int(4) : get_unt((int) n));
  cr_fnt = search_font_tbl( cr_fnt );
  break; }
default: {
  if( (ch < 
#line 17417 "./tex4ht-c.tex"

171  
) || (ch > 
#line 17423 "./tex4ht-c.tex"

234  
) ) {
     if( ch == 
#line 17323 "./tex4ht-c.tex"

140 
 ) { warn_i(46); }
     else { warn_i_int(45,ch); }
  } else { cr_fnt = ch - 
#line 17417 "./tex4ht-c.tex"

171  
;
         cr_fnt = search_font_tbl( cr_fnt );
  }
  break;
}


  }
} }

#line 3320 "./tex4ht-c.tex"

do{
                               long int i;
                               char *mark;
  ch = get_char();
  if(
      ( ch==
#line 17441 "./tex4ht-c.tex"

239 
) ||  ( ch==
#line 17443 "./tex4ht-c.tex"

240 
) ||
      ( ch==
#line 17445 "./tex4ht-c.tex"

241 
) ||  ( ch==
#line 17447 "./tex4ht-c.tex"

242 
)
    )
  {
     if( tex4ht_special( &ch, &i ) ){
         mark = get_str( (int) i );
         if( (ch=='@') && ( *mark=='?') && eq_str(mark+1,unhskip_mark)){
           break;
     }  }
  }
  (IGNORED) fseek(dvi_file, (long) retract_addr, 
#line 2174 "./tex4ht-c.tex"

0
);
} while(FALSE);




   cur_fnt = cr_fnt;
   free((void *)  unhskip_mark);
} else { ignore_chs++;; }

  break; }
  case ']': { 
#line 3390 "./tex4ht-c.tex"

if( special_n ){
   while( special_n-- > 0 ){  (void) get_char(); }
} else { ignore_chs--; }

  break; }
  case '?': { 
#line 3397 "./tex4ht-c.tex"

while( special_n-- > 0 ){  (void) get_char(); }

  break; }
  case '-': {
     if( special_n ) { code = 0; 
#line 4280 "./tex4ht-c.tex"

while( special_n-- > 0 ){
  digit = get_char() - '0';
  if ( (digit < 0) || (digit > 9) ) {  warn_i_int(41,digit+'0') ; }
  else { code = code * 10 + digit; }
}
if ( (code < 0) || (code > 255) ) {  code = '?'; warn_i_int(41,'?') ; }


       put_char( code );
     }  else  {  nomargin = TRUE; }
     break; }
  case '*': { 
#line 4342 "./tex4ht-c.tex"

keepChar=1;

  }
  case '+': { 
#line 4299 "./tex4ht-c.tex"

if( 
#line 4332 "./tex4ht-c.tex"

next_char

 != -1 ) {
   
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


   (IGNORED) put_4ht_ch( 
#line 4332 "./tex4ht-c.tex"

next_char

 , cur_o_file );
   
#line 4332 "./tex4ht-c.tex"

next_char

 = -1;
}
if( 
#line 4338 "./tex4ht-c.tex"

next_str

 ){    print_f(next_str);
  free((void *) next_str);  next_str = (char *) 0; }
next_str = get_str( (int) special_n );  special_n = 0;

#line 4314 "./tex4ht-c.tex"

{     char *front, *back;
      int i;
  back = front = next_str;
  while( *front != '\0' ){
    if( *front == '{' ){
      i = *(++front) - '0';
      while( *(++front) != '}' ){ i = i*10 + (*front - '0'); }
      *front = (char) i;
    }
    *(back++) = *(front++);
  }
  *back = '\0';
}



    break; }
  case '.': { 
#line 3402 "./tex4ht-c.tex"

if( eoln_str ){ free((void *)  eoln_str); }
if( special_n ){
   eoln_str = get_str( (int) special_n );  special_n=0;
} else { eoln_str = (char *) 0; }

  break; }
  case ',': { 
#line 3424 "./tex4ht-c.tex"

if( space_str ){ free((void *) space_str); }
if( special_n ){
   space_str = get_str( (int) special_n );  special_n=0;
} else { space_str = (char *) 0; }

  break; }
  case '_': { 
#line 3678 "./tex4ht-c.tex"

if( !special_n ){ rule_ch = '\0'; }
else            { while( special_n-- > 0 ){ rule_ch = get_char(); }
                }

  break; }
  case 'D': { 
#line 3686 "./tex4ht-c.tex"

          struct files_rec *p;
while( special_n-- > 0 ) (void)  putc( get_char(), log_file );
for( p = opened_files; p != (struct files_rec*) 0;  p = p->next ){
   if( p->file == cur_o_file) {
        (IGNORED) fprintf(log_file, "%d %s\n",
               (int) ftell(cur_o_file), p->name);
        break;
}  }

 break; }
  case 'u': { 
#line 15888 "./tex4ht-c.tex"

special_n--;
switch ( code = get_char() ){
  case '+': { put_4ht_off++; 
#line 15906 "./tex4ht-c.tex"

flush_uni();

 break; }
  case '-': { if( put_4ht_off>0 ){ put_4ht_off--; }
              else { warn_i_str(52, "@u-"); }
              break; }
}

  break; }
   default: { 
#line 4289 "./tex4ht-c.tex"

code -= '0';  
#line 4280 "./tex4ht-c.tex"

while( special_n-- > 0 ){
  digit = get_char() - '0';
  if ( (digit < 0) || (digit > 9) ) {  warn_i_int(41,digit+'0') ; }
  else { code = code * 10 + digit; }
}
if ( (code < 0) || (code > 255) ) {  code = '?'; warn_i_int(41,'?') ; }

 next_char = code;
if( 
#line 4338 "./tex4ht-c.tex"

next_str

 ){    print_f(next_str);
        free((void *) next_str);  next_str = (char *) 0; }

 }
}

  break; }
  case '+': { 
#line 5440 "./tex4ht-c.tex"

while( special_n-- > 0 )  (void) get_char();

  break; }
  case '=': { 
#line 4119 "./tex4ht-c.tex"

while( special_n-- > 0 ){
        int ch;
        BOOL flag;
        struct hcode_repl_typ *q;
   ch = get_char();
   q = hcode_repl;
   flag = FALSE;
   while( q != (struct hcode_repl_typ*) 0 ){
      if( ch == *(q->str) ){ flag = TRUE; break; }
      q = q->next;
   }
   if( flag ){
                   char *chr;
      chr = (q->str) + 1;
      while( *chr != 0 ){ put_char( *chr ); chr++; }
   } else { put_char( ch ); }
}

 break; }
  case '<':
  case '>': { 
#line 4383 "./tex4ht-c.tex"


#line 4541 "./tex4ht-c.tex"

   int  i=0;
   U_CHAR *name;
name =  m_alloc(char, (int) special_n+1);
*(name + (int) special_n) = '\0';
while(  special_n-- > 0 )  *(name + i++) = get_char();



#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }



#line 4555 "./tex4ht-c.tex"

for( p = opened_files; p != (struct files_rec*) 0;  p = p->next )
  { if( eq_str(p->name, name) )  break; }


if( ch == '>' ){ 
#line 4407 "./tex4ht-c.tex"

if( p !=  (struct files_rec*) 0 ){
  out_file = p->file;
  p->prev_file = cur_o_file;
  free((void *) name );
} else {
  if( !(*name) ) out_file = (FILE *) 0;
  else { 
#line 4419 "./tex4ht-c.tex"

p = m_alloc(struct files_rec, 1);
if( opened_files != (struct files_rec*) 0 ) opened_files->prev = p;
p->prev = (struct files_rec *) 0;
p->next = opened_files;     opened_files = p;
p->name = name;
p->file = out_file = open_html_file(name);
p->prev_file = cur_o_file;

 }
}

 }
else           { 
#line 4431 "./tex4ht-c.tex"

if( p == (struct files_rec *)  0 ) bad_special( name );
else { /* if p is null, do nothing more */

#line 4444 "./tex4ht-c.tex"

if( p->prev != (struct files_rec*) 0 ) (p->prev)->next = p->next;
else                                   opened_files = p->next;
if( p->next != (struct files_rec*) 0 ) (p->next)->prev = p->prev;


if( opened_files !=  (struct files_rec*) 0 )
  { if( out_file == p->file )  out_file = opened_files->file; }
else out_file = (FILE *) 0;
(IGNORED) fclose( p->file );   free((void *)  p->name );
free((void *) p );
}

 }
cur_o_file = ( out_file == (FILE *) 0 )? root_file
                                       : out_file;

  break; }
  case '!': { 
#line 4816 "./tex4ht-c.tex"

ch_map_flag = !ch_map_flag;
if( ch_map_flag ){ 
#line 4906 "./tex4ht-c.tex"

init_ch_map();
xresolution = yresolution = 0;
while( special_n-- > 0 ){
  ch = get_char();
  if( (ch >= '0') && (ch <= '9') )
     { yresolution = yresolution * 10 + ch - '0'; }
  else if( (ch == ',') && !xresolution && yresolution )
     { xresolution = yresolution;  yresolution = 0; }
  else { 
#line 4930 "./tex4ht-c.tex"

xresolution = yresolution = 0;

#line 17080 "./tex4ht-c.tex"

warn_i_int( 26, '!');
(IGNORED) putc( ch, stderr);
while( special_n-- )  (IGNORED) putc( get_char(), stderr);



 }
}
if( !xresolution )  xresolution = yresolution;
if( !xresolution ){ xresolution = XRESOLUTION;
                    yresolution = YRESOLUTION; }
else { xresolution = xresolution * (INTEGER) (XRESOLUTION / 100);
       yresolution = yresolution * (INTEGER) (YRESOLUTION / 100);  }

 }
else             { 
#line 5275 "./tex4ht-c.tex"

dump_ch_map();

  }

  break; }
  case '|': { gif_ch = !gif_ch;  break; }
  case ':': { 
#line 4722 "./tex4ht-c.tex"

if( special_n-- ){
        int code, n;
        U_CHAR str [255], *p;
        struct count_rec *q;
  code = get_char();
  while( special_n > 254 ){ (void) get_char(); special_n--; }
  p = str;  n = special_n;
  while( special_n-- ) { *(p++) = get_char(); }
  *p = '\0';
  
#line 4751 "./tex4ht-c.tex"

q = counter;
while( q ){
  if( eq_str(str,q->str) ) break;  q = q->next;
}
if( !q ){
  q = m_alloc(struct count_rec, 1);
  q->i = q->depth = 0;    q->max = 10;
  q->next = counter;  counter = q;
  q->str =  m_alloc(char, (int) n+1);
  (IGNORED) strcpy((char *)  q->str, (char *) str );
  q->stack =  m_alloc(int, q->max);
}


  
#line 4783 "./tex4ht-c.tex"

switch ( code ){
  case '+': {  (q->i)++; break; }
  case '-': {  (q->i)--; break; }
  case '>': {  
#line 4766 "./tex4ht-c.tex"

if( q->depth == q->max ){
   q->max += 10;
   if( (q->stack = (int *) r_alloc( (void *) q->stack,
            (size_t) (q->max * sizeof(int)))) == NULL) bad_mem;
}
q->stack[q->depth++] = q->i;

  break; }
  case '<': {  if( q->depth  ){ 
#line 4775 "./tex4ht-c.tex"

q->depth--;
if( q->max > q->depth + 20 ){ q->max -= 15;
   if( (q->stack = (int *) r_alloc( (void *) q->stack,
            (size_t) (q->max * sizeof(int)))) == NULL) bad_mem;
}

 }
               break; }
  case '!': {  
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


               (IGNORED) fprintf(cur_o_file, "%d", q->i); break; }
  case '|': {  if( q->depth  ){
      
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


      (IGNORED) fprintf(cur_o_file, "%d", q->stack[q->depth - 1] );
            }
      break; }
  default: { ; }
}


}

  break; }
  case ';': { 
#line 11400 "./tex4ht-c.tex"

        int n, code;
        U_CHAR *p, *q;
code = get_char();
n = 1 + ((--special_n>254)? 254 : special_n);
q = p = m_alloc(char, (int) n);
while( special_n > 254 ){ (void) get_char(); special_n--; }
while( special_n-- ) { *(q++) = get_char(); }
*q = '\0';  q = p;
switch ( code ){
  case '8': {  pause_style--; break; }
  case '9': {  pause_style++;  break; }
  case '-': {  default_font = font_tbl[cur_fnt].num;
               base_font_size = font_tbl[cur_fnt].scale / 100;
               break; }
  case '+': {  default_font = -1;                    break; }
  case '%': { 
#line 11814 "./tex4ht-c.tex"

   int f;
f = 0; while( *p ){ f = 10*f + *(p++) - '0'; }

#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


(IGNORED) fprintf(cur_o_file, "%d",
  (font_tbl[cur_fnt].scale / base_font_size - 100) * f / 100 +100
   );

  break; }
  case '=': { 
#line 11823 "./tex4ht-c.tex"


#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


(IGNORED) fprintf(cur_o_file, "%s", font_tbl[cur_fnt].name);
if( font_tbl[cur_fnt].mag != 100 ){
   (IGNORED) fprintf(cur_o_file,"_%d", font_tbl[cur_fnt].mag);
}

  break; }
  case '|': { 
#line 11599 "./tex4ht-c.tex"

{                 int bad_str, m;
                  U_CHAR ch, *t[
#line 11643 "./tex4ht-c.tex"

8
], err_str[256];
bad_str=
#line 11645 "./tex4ht-c.tex"

7
;   (IGNORED) strcpy((char *) err_str, (char *) p);
if( n>
#line 11647 "./tex4ht-c.tex"

10
 ){
   m = 100*( *p-'0' ) + 10*( *(p+1)-'0' )+ *(p+2)-'0';
   if( (m>-1) && (m<256) ){
      ch = *(p + 3);  t[0]=p;
      while( (*p = *(p+4)) != '\0' ){
        if( ch == *p ){ *p = '\0';
          if( bad_str-- > 0 ) t[
#line 11645 "./tex4ht-c.tex"

7
 - bad_str] = p+1;
        }
        p++;
   }  }
   if( !bad_str ){
      if( m==0 ){ span_name_on = n>
#line 11649 "./tex4ht-c.tex"

11
; }
      q = span_open[m];     span_open[m] = t[0];
      span_name[m] = t[1];  span_size[m] = t[2];
      span_mag[m]  = t[3];  span_ord[m]  = t[4];
      span_ch[m]   = t[5];  end_span[m]  = t[6];
      gif_id[m]   = t[7];
      if( not_notify ) {
        store_bit_I( class_on, m );
        not_notify = FALSE;
      } else   store_bit_Z( class_on, m );
   }
}
if( bad_str ){  warn_i_str(37,err_str); }
}

  break; }
  case ',': { 
#line 11634 "./tex4ht-c.tex"

not_notify = TRUE;

  break; }
  default: { warn_i_int( 36, code); }
}
span_on = span_name_on && !pause_style;
if( q ) free((void *)  q);

  break; }
  case '"': { 
#line 8396 "./tex4ht-c.tex"

if( special_n ){
   
#line 8467 "./tex4ht-c.tex"

{                       U_CHAR * p, ch, i;
  ch = get_char();
  p = pos_text = pos_line = end_pos_text
    = end_pos_body = pos_body
    = (char *)  r_alloc((void *) pos_body,(size_t) special_n + 1);
  i = 0;  
#line 8486 "./tex4ht-c.tex"

{                               BOOL after_star=0;
   while(  special_n-- > 0 ){
      if( (*p = get_char()) == ch ){
         *p = '\0'; i++;
              if( i==1 ){ end_pos_body = p + 1;   after_star = FALSE; }
         else if( i==2 ){ pos_text     = p + 1;
                          if( !after_star ){  i--;  after_star = TRUE; }
                        }
         else if( i==3 ){ end_pos_text = p + 1; }
         else if( i==4 ){ pos_line     = p + 1; }
         else           { p++; break; }
      } else { after_star = FALSE; }
      p++;
   }
}


  
#line 8506 "./tex4ht-c.tex"

{                     long int v=0;
                      double w[5];
                      int j;
                      U_CHAR ch, sign;
                      BOOL done;
  for(j=0;j<5;j++){
    
#line 8528 "./tex4ht-c.tex"

done = FALSE;  sign = 1;
if( --special_n > 0 ){
  if( (ch = get_char()) == '-' ){ sign = -1; v=0; }
  else v = ch - '0';
  if( (v<0) || (v>9) ) done = TRUE;
}
if( !done )
   while(  --special_n > 0 ){
     ch = get_char();
     if( ('0' <= ch ) && (ch <= '9' ) ){
        v =  v * 10 + ch - '0'; }
     else{  done = TRUE;  break; }
   }


    if( done ){
      i++;
      w[j] = sign * ((double) v + pos_dbl( &special_n ));
    }
  }
  pos_x_A = w[0];  pos_x_B = w[1];
  pos_y_C = w[2];  pos_y_D = w[3]; pos_y_E = w[4];
}
rect_pos = (special_n == 2);
if( rect_pos ){  special_n -= 2;  rect_pos = get_char() - '0';}


  if( (i != 10) || special_n ){
    warn_i_str(39,pos_text);
    *(pos_text = end_pos_body = pos_line =
      end_pos_text = pos_body) = '\0';
  }
}


} else if( (pos_dvi = !pos_dvi) == TRUE ){
   print_f(pos_body);
   min_pos_x = max_pos_x = base_pos_x = x_val;
   min_pos_y = max_pos_y = base_pos_y = y_val ;
} else {                     U_CHAR   *p;
                             double dim=0.0;
                             BOOL   dim_on;
  
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


  p = end_pos_body;
  while( *p ){
    dim_on = TRUE;
    switch( *p ){
       
#line 8426 "./tex4ht-c.tex"

case 'X': {  dim = pos_x_A * (max_pos_x - base_pos_x) + pos_x_B;
             break; }
case 'x': {  dim = pos_x_A * (base_pos_x - min_pos_x) + pos_x_B;
             break; }
case 'd': {  dim = pos_x_A * (max_pos_x - min_pos_x) + pos_x_B;
             break; }
case 'y': {  dim = pos_y_C * (base_pos_y - min_pos_y - 1) + pos_y_D;
             break; }
case 'Y': {  dim = pos_y_C * (max_pos_y - base_pos_y) + pos_y_D;
             break; }
case 'D': {  dim = pos_y_C * (max_pos_y - min_pos_y) + pos_y_D;
             break; }


        default: { dim_on = FALSE; }
    }
    p++;
    if( dim_on ){    (IGNORED) fprintf(cur_o_file, p, dim);  }
    else        {    (IGNORED) fprintf(cur_o_file, "%s", p); }
    while( * (p++) );
  }
}

 break; }
  case '~': { 
#line 6581 "./tex4ht-c.tex"

if( special_n ){
  
#line 6612 "./tex4ht-c.tex"

              U_CHAR in_ch;
if( (in_ch = get_char()) == '>' ) {
  
#line 6734 "./tex4ht-c.tex"

if( special_n == 1 ){
   special_n--;
   switch( get_char() ){
     case '[': { ignore_end_group++;  break; }
     case ']': { ignore_end_group--;  break; }
      default: { 
#line 17093 "./tex4ht-c.tex"

;

 }
   }
} else {
             struct stack_end_entry *p;
             U_CHAR                   *q;
             int                     j;
   j = get_char() - '0' + stack_n - 1;
   if( --special_n ){
     if (j >= stack_len ) { j = stack_len - 1; }
     p = m_alloc(struct stack_end_entry,1);
     p->next =  stack[ j ].end;
     stack[ j ].end = p;
     q = p->send = m_alloc(char,special_n+1);
     while( --special_n ) *q++ = get_char();
     *q = '\0';
}  }


} else if( in_ch == '!' ) {
  
#line 7809 "./tex4ht-c.tex"

                 struct group_path *p, *t;
                 U_CHAR            *q, str[256];
                 int               n;
p = m_alloc(struct group_path,1);

#line 7832 "./tex4ht-c.tex"

n = 0;
while( --special_n ) {
   str[n] = get_char();
   if( ( str[n] != 'e') && (str[n] != 's') ){ break; }
   n++;
}
if((
    ( str[n] != '<') && (str[n] != '>') &&
    ( str[n] != '/') && (str[n] != '-')
   ) || (n==0) ){
  str[n+1] = '\0';
  err_i_str(38,str);
}
p->action = str[n]; str[n] = '\0';
p->path = m_alloc(char,n+1);
(IGNORED) strcpy((char *) p->path, (char *) str);



#line 7851 "./tex4ht-c.tex"

q = p->info = m_alloc(char,special_n+1);
while( --special_n ) *q++ = get_char();
*q = '\0';


n = stack_n - 1;
if( p->action == '>' ){
   p->next =  stack[ n ].path_end;
   stack[ n ].path_end = p;
} else {
   p->next = (struct group_path *) 0;
   if( stack[n].path_start == (struct group_path *) 0 ) {
      stack[n].path_start = p;
   } else {
      t = stack[n].path_start;
      while( t->next != (struct group_path *) 0 ) { t = t->next; }
      t->next = p;
}  }


} else {
  if( !group_dvi ){ warn_i(42); }
  (IGNORED) fseek(dvi_file, (long) --special_n,
                             
#line 2177 "./tex4ht-c.tex"

1
);
  special_n = 0;
}


} else if( (group_dvi = !group_dvi) == TRUE ){
               long  curr_pos;
               int   ch, sv_stack_n;
               
#line 7114 "./tex4ht-c.tex"

int cr_fnt;


#line 7474 "./tex4ht-c.tex"

BOOL  ch_token;
int  id_hide;


  
#line 7118 "./tex4ht-c.tex"

cr_fnt = cur_fnt;


#line 7479 "./tex4ht-c.tex"

sv_id = 0;    
#line 7470 "./tex4ht-c.tex"

ch_id = 0;


id_hide = 0;    ch_token = TRUE;
while( del_stack != (struct del_stack_entry*) 0 ){
                                  struct del_stack_entry* p;
  del_stack = (p = del_stack)->next;
  free((void *)  p );
}

  stack_id = 0;
  curr_pos = ftell(dvi_file);  sv_stack_n = stack_n;
  
#line 6989 "./tex4ht-c.tex"

while( group_dvi ){
  
#line 7126 "./tex4ht-c.tex"

if( (ch = get_char()) >= 128 ) {
  switch( ch ){
    
#line 7142 "./tex4ht-c.tex"

case 128: case 129: case 130: case 131: case 133:
case 134: case 135: case 136: {
  ch = (int) get_unt( (ch-(ch>132)) % 4 +1);
  
#line 7354 "./tex4ht-c.tex"

ch_id++;
if(!back_id_off ){
   if( !id_hide ){  ch_token = TRUE;  sv_id = ch_id; }
   switch( math_class_of( ch, cr_fnt ) ){
     case 
#line 7373 "./tex4ht-c.tex"

4

: { del_stack = push_del( (char) ch, cr_fnt);
                           break; }
     case 
#line 7369 "./tex4ht-c.tex"

5

: {
        del_stack = pop_del( (char) ch, id_hide, cr_fnt);   break; }
     default:{ ; }
}  }


  break;
}


    
#line 7080 "./tex4ht-c.tex"

case 
#line 17460 "./tex4ht-c.tex"

246 
: (void) get_char();
case 
#line 17457 "./tex4ht-c.tex"

245 
: (void) get_char();
case 
#line 17454 "./tex4ht-c.tex"

244 
: (void) get_char();
case 
#line 17451 "./tex4ht-c.tex"

243 
: {    int i;
  for( i=14; i; i-- ){  ch = get_char(); }
  i = ch +  get_char();
  (IGNORED) fseek(dvi_file, (long) i, 
#line 2177 "./tex4ht-c.tex"

1
);
  break;  }


    
#line 7151 "./tex4ht-c.tex"

case 
#line 17309 "./tex4ht-c.tex"

132 
:
case 
#line 17314 "./tex4ht-c.tex"

137 
:{
  (IGNORED) fseek(dvi_file, 8L, 
#line 2177 "./tex4ht-c.tex"

1
);
  break;
}


#line 7159 "./tex4ht-c.tex"

case   
#line 17320 "./tex4ht-c.tex"

139 
: {
  (IGNORED) fseek(dvi_file, 44L, 
#line 2177 "./tex4ht-c.tex"

1
);  break; }


#line 7164 "./tex4ht-c.tex"

case 
#line 17332 "./tex4ht-c.tex"

143 
: case 
#line 17335 "./tex4ht-c.tex"

144 
:
case 
#line 17338 "./tex4ht-c.tex"

145 
: case 
#line 17341 "./tex4ht-c.tex"

146 
: {
    (IGNORED) (get_int( ch - 
#line 17332 "./tex4ht-c.tex"

143 
 + 1 ));  break; }
case 
#line 17347 "./tex4ht-c.tex"

148 
:
case 
#line 17350 "./tex4ht-c.tex"

149 
:
case 
#line 17353 "./tex4ht-c.tex"

150 
:
case 
#line 17356 "./tex4ht-c.tex"

151 
: {
    (IGNORED) (get_int( ch - 
#line 17347 "./tex4ht-c.tex"

148 
 + 1));
    break;  }
case 
#line 17362 "./tex4ht-c.tex"

153  
:
case 
#line 17365 "./tex4ht-c.tex"

154  
:
case 
#line 17368 "./tex4ht-c.tex"

155 
:
case 
#line 17371 "./tex4ht-c.tex"

156 
: {
    (IGNORED) (get_int( ch - 
#line 17362 "./tex4ht-c.tex"

153  
 + 1));
    break;  }
case 
#line 17374 "./tex4ht-c.tex"

157 
: case 
#line 17377 "./tex4ht-c.tex"

158 
:
case 
#line 17380 "./tex4ht-c.tex"

159 
: case 
#line 17383 "./tex4ht-c.tex"

160 
: {
    (IGNORED) (get_int( ch - 
#line 17374 "./tex4ht-c.tex"

157 
 + 1));
    break; }
case 
#line 17389 "./tex4ht-c.tex"

162 
:
case 
#line 17392 "./tex4ht-c.tex"

163  
:
case 
#line 17395 "./tex4ht-c.tex"

164 
:
case 
#line 17398 "./tex4ht-c.tex"

165 
: {
    (IGNORED) (get_int( ch - 
#line 17389 "./tex4ht-c.tex"

162 
 + 1));
    break; }
case 
#line 17404 "./tex4ht-c.tex"

167 
:
case 
#line 17407 "./tex4ht-c.tex"

168 
:
case 
#line 17410 "./tex4ht-c.tex"

169 
:
case 
#line 17413 "./tex4ht-c.tex"

170 
: {
    (IGNORED) (get_int( ch - 
#line 17404 "./tex4ht-c.tex"

167 
 + 1));
    break; }


#line 7200 "./tex4ht-c.tex"

case 
#line 17344 "./tex4ht-c.tex"

147 
:
case 
#line 17359 "./tex4ht-c.tex"

152 
:
case 
#line 17386 "./tex4ht-c.tex"

161 
:
case 
#line 17401 "./tex4ht-c.tex"

166  
:
   { break; }


    
#line 6871 "./tex4ht-c.tex"

case 
#line 17326 "./tex4ht-c.tex"

141 
: {
   
#line 6891 "./tex4ht-c.tex"

{     struct group_info *p, *last;
  if( (last = p = stack[ stack_n ].begin) != (struct group_info *)0 )
    if( p->stack_id == -1 ){
      
#line 6901 "./tex4ht-c.tex"

while( p ){
  if( p->stack_id != -1 ){ break; }
  p->stack_id = stack_id;
  last = p;
  p = p->next;
}


      
#line 6911 "./tex4ht-c.tex"

while ( stack[ stack_n ].begin != last ){
  p = (stack[ stack_n ].begin) -> next;
  (stack[ stack_n ].begin) -> next = last->next;
  last->next = stack[ stack_n ].begin;
  stack[ stack_n ].begin = p;
}


    }
}


   stack[stack_n].stack_id = stack_id++;
   
#line 7339 "./tex4ht-c.tex"

if( !back_id_off )
{                    struct del_stack_entry *p;
   p = m_alloc(struct del_stack_entry,1);
   p->next = del_stack;
   p->id = p->fnt = -1;
   del_stack = p;
}

    stack_n++;
   if( stack_n > 
#line 6684 "./tex4ht-c.tex"

((int) stack_len + 2)

 ){ warn_i(40); }
   break;
}
case 
#line 17329 "./tex4ht-c.tex"

142 
: {
   stack_n--;  
#line 7325 "./tex4ht-c.tex"

if( !back_id_off ){
   if( !id_hide ){  ch_token = FALSE;
                    sv_id = stack[stack_n].stack_id; }
   while( del_stack != (struct del_stack_entry*) 0 ){
                                     struct del_stack_entry* p;
                                     int id;
     del_stack = (p = del_stack)->next;
     id = p->id;
     free((void *)  p );
     if( id == -1 ) break;
}  }


   stack[stack_n].stack_id = -1;
   break;
}


    
#line 7002 "./tex4ht-c.tex"

case 
#line 17441 "./tex4ht-c.tex"

239 
:  case 
#line 17443 "./tex4ht-c.tex"

240 
:
case 
#line 17445 "./tex4ht-c.tex"

241 
:  case 
#line 17447 "./tex4ht-c.tex"

242 
: {  long int i;
  if( tex4ht_special( &ch, &i ) ){
     if( ch == '~' ){
        
#line 7029 "./tex4ht-c.tex"

if( i==0 ){
  group_dvi = FALSE ;
}else{
  switch( get_char() ){
     case '<': {
        if( i-- ){         U_CHAR ch;
           if( (ch = get_char()) == '*' )
             { 
#line 7554 "./tex4ht-c.tex"

              struct send_back_entry *p, *q, *t=0;
if( back_id_off ){
   while( i-- ){ (IGNORED) get_char();  }
} else {
   p =  m_alloc(struct send_back_entry,1);
   p->send = get_str( (int)( i - 1 ));
   if( ch_token ){
     
#line 7595 "./tex4ht-c.tex"

p->id = sv_id;
if( sv_id >  back_token->id ){
   p->next = back_token;   back_token = p;
} else {
   q = back_token;
   while( sv_id <= q->id ){ t = q;  q = q->next;  }
   p->next = t->next;   t->next = p;
}


   } else {
     p->id = (sv_id<0? 0 : sv_id) +  push_id;
     if( back_group->id < p->id )
        {  p->next = back_group;   back_group = p;  }
     else
        {  q = back_group;
           while( q->id >= p->id ) { t = q;  q = q->next;  }
           p->next = t->next;   t->next = p;
        }
   }
}

 }
           else if( (ch == '[') && (i==1) ){
             i--;  
#line 7493 "./tex4ht-c.tex"

id_hide++;


           }
           else if( (ch == ']') && (i==1) ){
             i--;  
#line 7498 "./tex4ht-c.tex"

id_hide--;


           }
           else if( (ch == '-') && (i==1) ){
             i--;  
#line 7502 "./tex4ht-c.tex"

id_latex++;


           }
           else if( (ch == '+') && (i==1) ){
             i--;  
#line 7507 "./tex4ht-c.tex"

id_latex--;


           }
           else if( (ch == '(') && (i==1) ){
             i--;  
#line 7513 "./tex4ht-c.tex"

back_id_off++;


           }
           else if( (ch == ')') && (i==1) ){
             i--;  
#line 7518 "./tex4ht-c.tex"

back_id_off--;


           }
           else { 
#line 6846 "./tex4ht-c.tex"

          struct group_info *p;
          U_CHAR *q;
          int j;
j = ch - '0' + stack_n - 1;
if (j >= stack_len ) { j = stack_len - 1; }
p = m_alloc(struct group_info,1);
p->next = stack[ j ].begin; stack[ j ].begin = p;
p->stack_id = stack[ j ].stack_id;
q = p->info = m_alloc(char,i+1);
while( --i ) *q++ = get_char();
*q = '\0';

 }
        }
        break; }
     default: { (IGNORED) fseek(dvi_file, (long) --i,
                               
#line 2177 "./tex4ht-c.tex"

1
);  break; }
} }


     } else {
       (IGNORED) fseek(dvi_file, (long) i, 
#line 2177 "./tex4ht-c.tex"

1
);
     }
  }else{ 
#line 7016 "./tex4ht-c.tex"

   U_CHAR *ch;
ch = special_hd + 4;
while( *ch ){   ch++; }
(IGNORED) fseek(dvi_file, (long) i, 
#line 2177 "./tex4ht-c.tex"

1
);

  }
  break;
}


    
#line 7093 "./tex4ht-c.tex"

case  
#line 17429 "./tex4ht-c.tex"

235  
:
case 
#line 17432 "./tex4ht-c.tex"

236  
:
case 
#line 17435 "./tex4ht-c.tex"

237  
:
case     
#line 17438 "./tex4ht-c.tex"

238  
: {
                              INTEGER n;
  n = ch - 
#line 17429 "./tex4ht-c.tex"

235  
 + 1;
  cr_fnt = (int)  ((n==4)? get_int(4) : get_unt((int) n));
  cr_fnt = search_font_tbl( cr_fnt );
  break; }
default: {
  if( (ch < 
#line 17417 "./tex4ht-c.tex"

171  
) || (ch > 
#line 17423 "./tex4ht-c.tex"

234  
) ) {
     if( ch == 
#line 17323 "./tex4ht-c.tex"

140 
 ) { warn_i(46); }
     else { warn_i_int(45,ch); }
  } else { cr_fnt = ch - 
#line 17417 "./tex4ht-c.tex"

171  
;
         cr_fnt = search_font_tbl( cr_fnt );
  }
  break;
}


  }
} else { 
#line 7354 "./tex4ht-c.tex"

ch_id++;
if(!back_id_off ){
   if( !id_hide ){  ch_token = TRUE;  sv_id = ch_id; }
   switch( math_class_of( ch, cr_fnt ) ){
     case 
#line 7373 "./tex4ht-c.tex"

4

: { del_stack = push_del( (char) ch, cr_fnt);
                           break; }
     case 
#line 7369 "./tex4ht-c.tex"

5

: {
        del_stack = pop_del( (char) ch, id_hide, cr_fnt);   break; }
     default:{ ; }
}  }

 }


}


  
#line 6954 "./tex4ht-c.tex"

{                  struct group_info  *first, *second, *temp;
                   int i;
for(i = stack_len; i >= 0; i--){
  first = stack[i].begin;
  if( first ) {
     second = first->next;
     while( second  ){
       temp =  second->next;
       second->next = first;
       first = second;
       second = temp;
     }
     (stack[i].begin)->next = (struct group_info  *) 0;
     stack[i].begin = first;
} } }


#line 7610 "./tex4ht-c.tex"

back_group = rev_list( back_group );
back_token = rev_list( back_token );
back_token = back_insert ( back_token, 0);

#line 7470 "./tex4ht-c.tex"

ch_id = 0;




  (IGNORED) fseek(dvi_file, curr_pos, 
#line 2174 "./tex4ht-c.tex"

0
);
  group_dvi = TRUE;  stack_n = sv_stack_n;    stack_id = 0;
} else { 
#line 6813 "./tex4ht-c.tex"

{              int stack_n;
  for( stack_n=
#line 6684 "./tex4ht-c.tex"

((int) stack_len + 2)

;
       stack_n>0; stack_n--){
    group_dvi = TRUE;    
#line 6792 "./tex4ht-c.tex"

while( stack[stack_n-1].end ){
  
#line 6770 "./tex4ht-c.tex"

          struct stack_end_entry *q, *p, *t;
q = stack[ stack_n-1 ].end;
p = stack[ stack_n-1 ].end = (struct stack_end_entry *) 0;
while( q ){
  t = q->next;  q->next  = p;  p = q;  q = t;
}
while( p ){
  if( ! ignore_end_group ){ print_f( p->send ); }
  free((void *)  p->send );
  q = p;  p = p->next;    free((void *)  q );
}


}


    group_dvi =FALSE;
    
#line 6824 "./tex4ht-c.tex"

while( stack[stack_n-1].begin ){
                               struct group_info *p;
   warn_i_str(44, stack[stack_n-1].begin->info);
   p =  stack[stack_n-1].begin;
   stack[stack_n-1].begin = p->next;
   free((void *)  p );
}
stack[stack_n-1].stack_id = -1;


} }

 }

 break; }
  case '.': { 
#line 1759 "./tex4ht-c.tex"

if( no_root_file ){
   U_CHAR *name;
   name = m_alloc(char, 256);
   (IGNORED) strcpy((char *)  name, (char *) no_root_file );
   free((void *)  no_root_file);
   no_root_file = name;
   name += (size_t) strlen((char *) name);  while( *(--name) != '.' ); name++;
   while( special_n-- ){
       if( (no_root_file+253) == name ) name--;
       *name++ = get_char();
  }
   *name = '\0';
} else {
          U_CHAR str[256], *p;
   p = str;  while( special_n-- ){ *p++ = get_char(); }  *p = '\0';
   warn_i_str(43,str);
}

 break; }
  case '^': { 
#line 12528 "./tex4ht-c.tex"

switch( special_n ){
  case 0:{ if( math_class_on ){
              open_del = 256;   pause_class = ignore_subclass_del = 0;
              math_class_on = FALSE;  
#line 12575 "./tex4ht-c.tex"

group_dvi     = sv_group_dvi;
trace_dvi_C   = sv_trace_dvi_C;
in_trace_char = sv_in_trace_char;
span_on       = sv_span_on;
in_span_ch    = sv_in_span_ch;


           } else { show_class = !show_class; }
           break;
         }
  case 1:{ 
#line 12566 "./tex4ht-c.tex"

sv_group_dvi     = group_dvi;
sv_trace_dvi_C   = trace_dvi_C;
sv_in_trace_char = in_trace_char;
sv_span_on       = span_on;
sv_in_span_ch    = in_span_ch;


           special_n--;
           if( (math_class = scan_class(1)) == 
#line 12719 "./tex4ht-c.tex"

79

 )
             { math_class = 0; }
           else math_class_on = TRUE;
           break;
         }
  case 2:{  
#line 12801 "./tex4ht-c.tex"

special_n -= 2;   math_class = scan_class(0);
stack[stack_n+1].ignore_subclass_del =
   (
#line 12726 "./tex4ht-c.tex"

')'

 == get_char());
stack[stack_n+1].active_class_del = TRUE;
stack[stack_n+1].temp_class_del = FALSE;
stack[stack_n+1].no_left_del = TRUE;
stack[stack_n+1].class_open = open_class[math_class];
stack[stack_n+1].class_close = close_class[math_class];


            break; }
 default:{ 
#line 12657 "./tex4ht-c.tex"

{                                    U_CHAR str[256], *p, ch, **q;
   math_class = scan_class(2);  ch = get_char();
   special_n -= 2;   p = str;
   while( special_n-- > 0 ){
      if(  (*(p++) = get_char()) == ch ){ p--; break; }
   }
   *p = '\0';
   q = (math_class > 
#line 12715 "./tex4ht-c.tex"

78

)? &(
#line 12833 "./tex4ht-c.tex"

stack[stack_n+1].temp_class_open

)
                       : &(open_class[math_class]);
   *q = (char *)  r_alloc((void *) open_class[math_class],
                                 1 + (size_t) strlen((char *) str));
   (IGNORED) strcpy((char *) *q, (char *) str);
   q = (math_class > 
#line 12715 "./tex4ht-c.tex"

78

) ? &(
#line 12829 "./tex4ht-c.tex"

stack[stack_n+1].temp_class_close

)
                         :  &(close_class[math_class]);
   p = *q = (char *)  r_alloc((void *) *q,  1 + (size_t) special_n);
   while( special_n-- > 0 ){ *(p++) = get_char();  }
   *p = '\0';
   if( math_class > 
#line 12715 "./tex4ht-c.tex"

78

){ 
#line 12820 "./tex4ht-c.tex"

stack[stack_n+1].ignore_subclass_del =
  (math_class == 
#line 12734 "./tex4ht-c.tex"

(
#line 12719 "./tex4ht-c.tex"

79

 + 1)

);
stack[stack_n+1].temp_class_del = TRUE;
stack[stack_n+1].active_class_del = TRUE;

 }
}

 }
}

 break; }
}

  special_on = FALSE;
     
#line 17073 "./tex4ht-c.tex"

if( special_n > 0 ){
   warn_i_int( 26, sv);
   while( special_n-- )  (IGNORED) putc( get_char(), stderr);
}


  } else { 
#line 3871 "./tex4ht-c.tex"

while( special_n-- )  (void) get_char();

  }

  break;  }


#line 7219 "./tex4ht-c.tex"

case     
#line 17326 "./tex4ht-c.tex"

141 
: { 
#line 7227 "./tex4ht-c.tex"


#line 6414 "./tex4ht-c.tex"

if( new_halign ){
  
#line 6381 "./tex4ht-c.tex"

stack[stack_n].halign_on = new_halign;
if( stack[stack_n].halign_info )
{                             int j;
   for( j=8; j--; ){
     if( stack[stack_n].halign[j] != halign[j] ){
       if( ! (--(stack[stack_n].halign[j]->refs) ) ){
         free((void *)  stack[stack_n].halign[j]->str );
         free((void *)  stack[stack_n].halign[j] );
       }
       stack[stack_n].halign[j] = halign[j];
       (halign[j]->refs)++;
     }
  }
} else {                                        int j;
  stack[stack_n].halign_info = TRUE;
   for( j=8; j--; ){
     stack[stack_n].halign[j] = halign[j];
     (halign[j]->refs)++;
} }
print_f( stack[stack_n].halign[0]->str );
stack[stack_n].row_no = 0;
new_halign = FALSE;


}
if( stack[stack_n].halign_on )
{
   print_f( stack[stack_n].halign[2]->str );
   if( stack[stack_n].halign_on > TRUE ){
      stack[stack_n].row_no++;
      stack[stack_n].col_no = 0;
      (IGNORED) fprintf(cur_o_file, "%d%s",
            stack[stack_n].row_no, stack[stack_n].halign[6]->str );
}  }
if( stack_n ){
   if( stack[stack_n-1].halign_on )
   {
     print_f( stack[stack_n-1].halign[4]->str );
     if( stack[stack_n-1].halign_on > TRUE ){
        stack[stack_n-1].col_no ++;
        (IGNORED) fprintf(cur_o_file, "%d%s",
             stack[stack_n-1].col_no, stack[stack_n-1].halign[7]->str );
     }
}  }



#line 6922 "./tex4ht-c.tex"

{                        struct group_info *p;
  if( group_dvi &&
      ( (p = stack[stack_n].begin )  != (struct group_info *)0)
    ){
     while( p ){
       if( p->stack_id != stack_id ) break;
       print_f(p->info);
       stack[stack_n].begin = p->next;
       free((void *)  p );
       p = stack[stack_n].begin;
     }
  }
  stack_id++;
}


stack[stack_n].text_on = text_on;
push_stack();  
#line 7647 "./tex4ht-c.tex"

if( group_dvi ) {
   back_group = back_insert ( back_group, push_id);
}



#line 7861 "./tex4ht-c.tex"

{
           
#line 7881 "./tex4ht-c.tex"

struct group_path *start_head, *start_tail,
                  *parent_start_head, *parent_start_tail,
                  *end_head, *end_tail,
                  *parent_end_head, *parent_end_tail,
                  *p, *q;
int place=0;
start_head = start_tail = parent_start_head = parent_start_tail
           = end_head   = end_tail          = parent_end_head
           = parent_end_tail = (struct group_path *) 0;


  if( 
#line 3739 "./tex4ht-c.tex"

rule_ch_off

 ){
    
#line 3272 "./tex4ht-c.tex"

ignore_spaces--;


    
#line 3739 "./tex4ht-c.tex"

rule_ch_off

 = FALSE;
  }
  if( stack_n > 1 ){
    p = stack[stack_n - 2].path_start;
    if(  p !=  (struct group_path *) 0 ){
      
#line 7902 "./tex4ht-c.tex"

while( p !=  (struct group_path *) 0 ){
   
#line 8007 "./tex4ht-c.tex"

if( *(p->path ) == 'e' ) {
  (IGNORED) strcpy((char *) p->path, (char *) p->path+1);
  if( *(p->path) == '\0' ) {
     switch( p->action ){
       case '<':  print_f( p->info );
                  place = 
#line 7978 "./tex4ht-c.tex"

4 
;
                  break;
       case '/':  ignore_chs++;
                  place = 
#line 7972 "./tex4ht-c.tex"

2 
; break;
       case '-':  
#line 3739 "./tex4ht-c.tex"

rule_ch_off

 = TRUE;
                  
#line 3268 "./tex4ht-c.tex"

ignore_spaces++;


                  place = 
#line 7978 "./tex4ht-c.tex"

4 
; break;
     }
  } else {
     place = 
#line 7972 "./tex4ht-c.tex"

2 
;
  }
} else {
  if( *(p->path ) == 's' ) {
     (IGNORED) strcpy((char *) p->path, (char *) p->path+1);
  }
  place = 
#line 7966 "./tex4ht-c.tex"

0 
;
}


   q = p;
   p = p->next;
   q->next = (struct group_path *) 0;
   
#line 7922 "./tex4ht-c.tex"

switch( place ){
 case 
#line 7966 "./tex4ht-c.tex"

0 
:
   if( parent_start_head == (struct group_path *) 0 ){
       parent_start_head = parent_start_tail = q;
   } else {
       parent_start_tail = parent_start_tail->next = q;
   }
   break;
 case 
#line 7972 "./tex4ht-c.tex"

2 
:
   if( start_head == (struct group_path *) 0 ){
       start_head = start_tail = q;
   } else {
       start_tail = start_tail->next = q;
   }
   break;
 case 
#line 7978 "./tex4ht-c.tex"

4 
:
   
#line 7981 "./tex4ht-c.tex"

free((void *)  q->path );
free((void *)  q->info );
free((void *)  q );


   break;
}


}


    }
    p = stack[stack_n - 2].path_end;
    if(  p !=  (struct group_path *) 0 ){
      
#line 7912 "./tex4ht-c.tex"

while( p !=  (struct group_path *) 0 ){
   
#line 8043 "./tex4ht-c.tex"

if( *(p->path ) == 'e' ) {
  (IGNORED) strcpy((char *) p->path, (char *) p->path+1);
  place = 
#line 7975 "./tex4ht-c.tex"

3 
;
} else {
  if( *(p->path ) == 's' ) {
     (IGNORED) strcpy((char *) p->path, (char *) p->path+1);
  }
  place = 
#line 7969 "./tex4ht-c.tex"

1 
;
}


   q = p;
   p = p->next;
   q->next = (struct group_path *) 0;
   
#line 7944 "./tex4ht-c.tex"

switch( place ){
 case 
#line 7969 "./tex4ht-c.tex"

1 
:
   if( parent_end_head == (struct group_path *) 0 ){
       parent_end_head = parent_end_tail = q;
   } else {
       parent_end_tail = parent_end_tail->next = q;
   }
   break;
 case 
#line 7975 "./tex4ht-c.tex"

3 
:
   if( end_head == (struct group_path *) 0 ){
       end_head = end_tail = q;
   } else {
       end_tail = end_tail->next = q;
   }
   break;
 case 
#line 7978 "./tex4ht-c.tex"

4 
:
   
#line 7981 "./tex4ht-c.tex"

free((void *)  q->path );
free((void *)  q->info );
free((void *)  q );


   break;
}


}


    }
    
#line 7893 "./tex4ht-c.tex"

  stack[stack_n - 1].path_start = start_head;
  stack[stack_n - 1].path_end   = end_head;
  stack[stack_n - 2].path_start = parent_start_head;
  stack[stack_n - 2].path_end   = parent_end_head;


} }



#line 2867 "./tex4ht-c.tex"

if( push_depth<256 ) { push_st[push_depth] = push_id++; }
if( trace_dvi_P && !ch_map_flag ){
   
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


   if( *trace_dvi_del_P != '\0' ){
      (IGNORED) fprintf(cur_o_file, "%s%d %d",
         trace_dvi_del_P, push_depth,
         push_st[(push_depth<256)? push_depth:256]);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_P);
}
push_depth++;



#line 12842 "./tex4ht-c.tex"

if( stack[stack_n].active_class_del ){
   if( show_class && !pause_class && !ignore_subclass_del ){
      
#line 12982 "./tex4ht-c.tex"

(IGNORED) print_f( (stack[stack_n].temp_class_del)?
                      stack[stack_n].temp_class_open
                   :  stack[stack_n].class_open);


   }
   ignore_subclass_del =  ignore_subclass_del
                        + stack[stack_n].ignore_subclass_del;
   stack[stack_n+1].no_left_del= FALSE;
}



   break; }
case 
#line 17329 "./tex4ht-c.tex"

142 
: { 
#line 7272 "./tex4ht-c.tex"


#line 12854 "./tex4ht-c.tex"

if( stack[stack_n].active_class_del ){
   ignore_subclass_del =  ignore_subclass_del
                        - stack[stack_n].ignore_subclass_del;
   if( show_class && !pause_class && !ignore_subclass_del ){
      
#line 12969 "./tex4ht-c.tex"

(IGNORED) print_f( (stack[stack_n].temp_class_del)?
                      stack[stack_n].temp_class_close
                   :  stack[stack_n].class_close);


   }
   stack[stack_n].active_class_del = FALSE;
}



#line 8063 "./tex4ht-c.tex"

{
     struct group_path *p, *q;
  if( stack_n > 1 ){
    p = stack[stack_n - 1].path_start;
    if(  p !=  (struct group_path *) 0 ){
      
#line 8078 "./tex4ht-c.tex"

while( p !=  (struct group_path *) 0 ){
   
#line 8104 "./tex4ht-c.tex"

if( *(p->path) != '\0' ) {
   
#line 8134 "./tex4ht-c.tex"

            char str[256];
(IGNORED) strcpy(str, "...."); *(str+3) = p->action;
(IGNORED) strct(str,p->info); warn_i_str(38,str);


} else  {
   switch( p->action ){
     case '/':  ignore_chs--;  break;
      default:  {
          
#line 8134 "./tex4ht-c.tex"

            char str[256];
(IGNORED) strcpy(str, "...."); *(str+3) = p->action;
(IGNORED) strct(str,p->info); warn_i_str(38,str);


          break;
}  }  }


   q = p;
   p = p->next;
   
#line 7981 "./tex4ht-c.tex"

free((void *)  q->path );
free((void *)  q->info );
free((void *)  q );


}


    }
    p = stack[stack_n - 1].path_end;
    if(  p !=  (struct group_path *) 0 ){
      
#line 8087 "./tex4ht-c.tex"

while( p !=  (struct group_path *) 0 ){
   
#line 8120 "./tex4ht-c.tex"

if( *(p->path) != '\0' ) {
   
#line 8134 "./tex4ht-c.tex"

            char str[256];
(IGNORED) strcpy(str, "...."); *(str+3) = p->action;
(IGNORED) strct(str,p->info); warn_i_str(38,str);


} else  {
   switch( p->action ){
     case '>':  print_f( p->info );  break;
      default:  {
          
#line 8134 "./tex4ht-c.tex"

            char str[256];
(IGNORED) strcpy(str, "...."); *(str+3) = p->action;
(IGNORED) strct(str,p->info); warn_i_str(38,str);


          break;
}  }  }


   q = p;
   p = p->next;
   
#line 7981 "./tex4ht-c.tex"

free((void *)  q->path );
free((void *)  q->info );
free((void *)  q );


}


} } }



#line 6792 "./tex4ht-c.tex"

while( stack[stack_n-1].end ){
  
#line 6770 "./tex4ht-c.tex"

          struct stack_end_entry *q, *p, *t;
q = stack[ stack_n-1 ].end;
p = stack[ stack_n-1 ].end = (struct stack_end_entry *) 0;
while( q ){
  t = q->next;  q->next  = p;  p = q;  q = t;
}
while( p ){
  if( ! ignore_end_group ){ print_f( p->send ); }
  free((void *)  p->send );
  q = p;  p = p->next;    free((void *)  q );
}


}



#line 2883 "./tex4ht-c.tex"

push_depth--;
if( trace_dvi_P && !ch_map_flag ){
   
#line 1715 "./tex4ht-c.tex"

if( no_root_file ){  open_o_file(); }


   if( *trace_dvi_del_p != '\0' ){
      (IGNORED) fprintf(cur_o_file, "%s%d %d",
         trace_dvi_del_p,  push_depth,
         push_st[(push_depth<256)? push_depth:256]);
   }
   (IGNORED) fprintf(cur_o_file, "%s", end_trace_dvi_del_p);
}



#line 6443 "./tex4ht-c.tex"


#line 6492 "./tex4ht-c.tex"

if( stack[stack_n].halign_on )
{  
#line 6498 "./tex4ht-c.tex"

print_f( stack[stack_n].halign[1]->str );
stack[stack_n].halign_on = FALSE;

  }



#line 6505 "./tex4ht-c.tex"

if( stack_n ){
  if( stack[stack_n-1].halign_on )
  {
     print_f( stack[stack_n-1].halign[3]->str );
} }



#line 6514 "./tex4ht-c.tex"

if( stack_n-1 ){
  if( stack[stack_n-2].halign_on )
  {
     print_f( stack[stack_n-2].halign[5]->str );
} }


if( stack[stack_n].halign_info )
{                             int j;
   for( j=8; j--; ){
     if( ! (--(stack[stack_n].halign[j]->refs) ) ){
       free((void *)  stack[stack_n].halign[j]->str );
       free((void *)  stack[stack_n].halign[j] );
   }  }
   stack[stack_n].halign_info = FALSE;
}


pop_stack();
if( ((x_val+0.6*word_sp) <  stack[stack_n].x_val) )  put_char(' ');
text_on = stack[stack_n].text_on;

  break; }


   }
}


    }
    
#line 5282 "./tex4ht-c.tex"

if( ch_map_flag ){
   warn_i(27);    init_ch_map(); }


    (IGNORED) printf("]%c",unread_pages % 10 == 0? '\n' : ' ');
    put_char('\n');
} }


put_char('\n');put_char('\n');

#line 4456 "./tex4ht-c.tex"

while( opened_files != (struct files_rec*) 0 )
{
   (IGNORED) fclose( opened_files->file );
   opened_files = opened_files->next;
}


{    
#line 5778 "./tex4ht-c.tex"

INTEGER bop_addr;


#line 5843 "./tex4ht-c.tex"

int stack_depth=0;


#line 5900 "./tex4ht-c.tex"

char cur_font[6];
BOOL visible_cnt=FALSE;


  
#line 5406 "./tex4ht-c.tex"

job_name[job_name_n-3] = '\0';

#line 5448 "./tex4ht-c.tex"

file_n = 14;
(IGNORED) fseek(dvi_file, 0L, 
#line 2174 "./tex4ht-c.tex"

0
);
do{  ch = get_char();
     idv_char( ch );
     file_n++;
}while( ch == 
#line 17317 "./tex4ht-c.tex"

138 
 );

#line 5462 "./tex4ht-c.tex"

ch = get_char();
if( id_version != -1 ){ ch = id_version; }
idv_char( ch );


for( i=12; i ; i-- ){  idv_char( get_char() ); }
i = get_char();
idv_char( (int) i );  while( i-- ) idv_copy();


page_n = 0;

#line 6098 "./tex4ht-c.tex"

x_val = 0;   y_val = 0;   stack_n = 0;
idv_char( 
#line 17320 "./tex4ht-c.tex"

139 
 );
idv_int( page_n + 1 );  for( i=36; i--; ) idv_char( 0);
idv_int( -1 );  bop_addr = file_n;  file_n += 45;
idv_char(
#line 17326 "./tex4ht-c.tex"

141 
);  file_n++;


while( dis_pages ){ 
#line 5526 "./tex4ht-c.tex"

if( (ch = get_char()) < 128 ) { visible_cnt = TRUE;  cond_idv_char( ch );}
else switch( ch ){ 
#line 5533 "./tex4ht-c.tex"

case 128: case 129: case 130: case 131: case 133:
case 134: case 135: case 136: {
  visible_cnt = TRUE;  cond_string( ch, (ch - (ch>132)) % 4 +1 );
  break;
}


#line 5544 "./tex4ht-c.tex"

case   
#line 17320 "./tex4ht-c.tex"

139 
: {
  x_val = 0;   y_val = 0;  stack_n = 0;
  (IGNORED) fseek(dvi_file, 44L, 
#line 2177 "./tex4ht-c.tex"

1
);  break; }
case     
#line 17323 "./tex4ht-c.tex"

140 
: { dis_pages--; }
case           
#line 17317 "./tex4ht-c.tex"

138 
: { break; }


#line 5553 "./tex4ht-c.tex"

case 
#line 17344 "./tex4ht-c.tex"

147 
: {
  cond_idv_char( ch );  x_val += dx_1;  break; }
case 
#line 17359 "./tex4ht-c.tex"

152 
: {
  cond_idv_char( ch );  x_val += dx_2;  break; }
case 
#line 17386 "./tex4ht-c.tex"

161 
: {
  cond_idv_char( ch );  y_val += dy_1;  break; }
case 
#line 17401 "./tex4ht-c.tex"

166  
: {
  cond_idv_char( ch );  y_val += dy_2;  break; }


#line 5565 "./tex4ht-c.tex"

case 
#line 17332 "./tex4ht-c.tex"

143 
: case 
#line 17335 "./tex4ht-c.tex"

144 
:
case 
#line 17338 "./tex4ht-c.tex"

145 
: case 
#line 17341 "./tex4ht-c.tex"

146 
: {
    cond_idv_char( ch );
    x_val += cond_int( ch - 
#line 17332 "./tex4ht-c.tex"

143 
 + 1 );  break; }
case 
#line 17347 "./tex4ht-c.tex"

148 
:
case 
#line 17350 "./tex4ht-c.tex"

149 
:
case 
#line 17353 "./tex4ht-c.tex"

150 
:
case 
#line 17356 "./tex4ht-c.tex"

151 
: {
    cond_idv_char( ch );
    dx_1 = (INTEGER) cond_int( ch - 
#line 17347 "./tex4ht-c.tex"

148 
 + 1);
    x_val += dx_1;   break;  }
case 
#line 17362 "./tex4ht-c.tex"

153  
:
case 
#line 17365 "./tex4ht-c.tex"

154  
:
case 
#line 17368 "./tex4ht-c.tex"

155 
:
case 
#line 17371 "./tex4ht-c.tex"

156 
: {
    cond_idv_char( ch );
    dx_2 = (INTEGER) cond_int( ch - 
#line 17362 "./tex4ht-c.tex"

153  
 + 1);
    x_val += dx_2;   break;  }


#line 5586 "./tex4ht-c.tex"

case 
#line 17374 "./tex4ht-c.tex"

157 
: case 
#line 17377 "./tex4ht-c.tex"

158 
:
case 
#line 17380 "./tex4ht-c.tex"

159 
: case 
#line 17383 "./tex4ht-c.tex"

160 
: {
    cond_idv_char( ch );
    y_val += cond_int( ch - 
#line 17374 "./tex4ht-c.tex"

157 
 + 1);
    break; }
case 
#line 17389 "./tex4ht-c.tex"

162 
:
case 
#line 17392 "./tex4ht-c.tex"

163  
:
case 
#line 17395 "./tex4ht-c.tex"

164 
:
case 
#line 17398 "./tex4ht-c.tex"

165 
: {
    cond_idv_char( ch );
    dy_1 = (INTEGER) cond_int( ch - 
#line 17389 "./tex4ht-c.tex"

162 
 + 1);
    y_val += dy_1;   break; }
case 
#line 17404 "./tex4ht-c.tex"

167 
:
case 
#line 17407 "./tex4ht-c.tex"

168 
:
case 
#line 17410 "./tex4ht-c.tex"

169 
:
case 
#line 17413 "./tex4ht-c.tex"

170 
: {
    cond_idv_char( ch );
    dy_2 = (INTEGER) cond_int( ch - 
#line 17404 "./tex4ht-c.tex"

167 
 + 1);
    y_val += dy_2;   break; }


#line 5613 "./tex4ht-c.tex"

case   
#line 17309 "./tex4ht-c.tex"

132 
:{
  visible_cnt = TRUE; cond_string( ch,4 ); x_val += cond_int(4);
  break;
}
case 
#line 17314 "./tex4ht-c.tex"

137 
:{
  visible_cnt = TRUE; cond_string( ch, 8 );
  break;
}


#line 5646 "./tex4ht-c.tex"

case 
#line 17441 "./tex4ht-c.tex"

239 
:  case 
#line 17443 "./tex4ht-c.tex"

240 
:
case 
#line 17445 "./tex4ht-c.tex"

241 
:  case 
#line 17447 "./tex4ht-c.tex"

242 
: {  long int i;
                                            int special_nr;
  special_nr = ch;
  if( tex4ht_special( &ch, &i ) ){
     if( ch == '+' ){
        
#line 5701 "./tex4ht-c.tex"

if( i==0 ){ if( dvi_flag ){ dvi_flag = 0;  
#line 5833 "./tex4ht-c.tex"

if( !visible_cnt ) {                            U_CHAR  str[256];
   (IGNORED) sprintf(str, "--- empty picture --- %sidv[%d] ---\n",
                          job_name,page_n);
   (IGNORED) printf("%s", str);  (IGNORED) fprintf(log_file, "%s",str); }
while( stack_depth-- > 0 ){
  idv_char(
#line 17329 "./tex4ht-c.tex"

142 
);  file_n++; }

 } }
else{
  if( dvi_flag ){ 
#line 5760 "./tex4ht-c.tex"

cond_idv_char( special_nr );
cond_idv_int( i, special_nr - 
#line 17441 "./tex4ht-c.tex"

239 
 + 1 );
while( i-- )  cond_idv_char( get_char() );
visible_cnt = TRUE;

 }
  else switch( get_char() ){
     case '+': { 
#line 5740 "./tex4ht-c.tex"

{    U_CHAR str[256], *ch;
   ch = str;   while( --i )  *(ch++) =  get_char();  *ch = '\0';
   script(font_gif, job_name ,page_n+1, str);
}


                 dvi_flag = TRUE;  dvi_page = TRUE;
                 
#line 5768 "./tex4ht-c.tex"

visible_cnt = FALSE;
bop_addr = advance_idv_page( bop_addr, cur_font );
stack_depth = 0;
set_loc( 
#line 17332 "./tex4ht-c.tex"

143 
, x_val );
set_loc( 
#line 17374 "./tex4ht-c.tex"

157 
, y_val );

  break; }
     case '@': { 
#line 5716 "./tex4ht-c.tex"

while( --i ) (void)  putc( get_char(), log_file );
(IGNORED) putc( '\n', log_file );

 break; }
      default: { while( --i ) (void)  get_char();  break; }
} }

 }
     else   while( i-- ) (void)  get_char();
  }else if( dvi_flag ){ 
#line 5679 "./tex4ht-c.tex"

visible_cnt = TRUE;   
#line 5684 "./tex4ht-c.tex"

{
      U_CHAR *ch;
      int j;
   ch = special_hd;
   (IGNORED) putc( (unsigned) 
#line 17447 "./tex4ht-c.tex"

242 
, idv_file );  file_n++;
   for(j=4; j--; ){  (IGNORED) putc( *ch, idv_file );  file_n++;  ch++; }
   while( *ch ){  (IGNORED) putc( *ch, idv_file );  file_n++;  ch++; }
   file_n += (int) i;
   while( i-- )  (IGNORED) putc( get_char(), idv_file );
}




  }else { 
#line 5664 "./tex4ht-c.tex"

if( dvi_page || !page_n ){ dvi_page = FALSE;  
#line 5768 "./tex4ht-c.tex"

visible_cnt = FALSE;
bop_addr = advance_idv_page( bop_addr, cur_font );
stack_depth = 0;
set_loc( 
#line 17332 "./tex4ht-c.tex"

143 
, x_val );
set_loc( 
#line 17374 "./tex4ht-c.tex"

157 
, y_val );

  }
dvi_flag = TRUE;

#line 5684 "./tex4ht-c.tex"

{
      U_CHAR *ch;
      int j;
   ch = special_hd;
   (IGNORED) putc( (unsigned) 
#line 17447 "./tex4ht-c.tex"

242 
, idv_file );  file_n++;
   for(j=4; j--; ){  (IGNORED) putc( *ch, idv_file );  file_n++;  ch++; }
   while( *ch ){  (IGNORED) putc( *ch, idv_file );  file_n++;  ch++; }
   file_n += (int) i;
   while( i-- )  (IGNORED) putc( get_char(), idv_file );
}


dvi_flag = FALSE;

 }
  break;
}


#line 5800 "./tex4ht-c.tex"

case 
#line 17326 "./tex4ht-c.tex"

141 
: {
   push_stack();
   stack_depth++;
   cond_idv_char( ch );
   break; }
case 
#line 17329 "./tex4ht-c.tex"

142 
: {      INTEGER cur_x, cur_y;
   stack_depth--;
   cur_x = (INTEGER) x_val;  cur_y = (INTEGER) y_val;  pop_stack();
   if( dvi_flag ){
      if( stack_depth<0 ){ warn_i_int( 24,  page_n );
                           
#line 5817 "./tex4ht-c.tex"

cond_idv_char( 
#line 17341 "./tex4ht-c.tex"

146 
 );
idv_int( x_val - cur_x - dx_1 - dx_2 );
cond_idv_char( 
#line 17356 "./tex4ht-c.tex"

151 
 );
idv_int( dx_1 );
cond_idv_char( 
#line 17371 "./tex4ht-c.tex"

156 
 );
idv_int( dx_2 );
cond_idv_char( 
#line 17383 "./tex4ht-c.tex"

160 
 );
idv_int( y_val - cur_y - dy_1 - dy_2 );
cond_idv_char( 
#line 17398 "./tex4ht-c.tex"

165 
 );
idv_int( dy_1 );
cond_idv_char( 
#line 17413 "./tex4ht-c.tex"

170 
 );
idv_int( dy_2 );
cond_idv_char( 
#line 17326 "./tex4ht-c.tex"

141 
 );  file_n += 24;

     }
      cond_idv_char( ch );
   }
   break; }


#line 5854 "./tex4ht-c.tex"

case 
#line 17460 "./tex4ht-c.tex"

246 
:
case 
#line 17457 "./tex4ht-c.tex"

245 
:
case 
#line 17454 "./tex4ht-c.tex"

244 
:
case 
#line 17451 "./tex4ht-c.tex"

243 
: {  idv_char( ch );             file_n++;
  for( i=14; i; i-- ){  ch = get_char(); idv_char( ch ); file_n++; }
  i = ch;  i += ch = get_char();  idv_char( ch );        file_n++;
  while( i-- ){ idv_copy(); }
  break;  }


#line 5873 "./tex4ht-c.tex"

case  
#line 17429 "./tex4ht-c.tex"

235  
:
case 
#line 17432 "./tex4ht-c.tex"

236  
:
case 
#line 17435 "./tex4ht-c.tex"

237  
:
case     
#line 17438 "./tex4ht-c.tex"

238  
: {    int i;
   idv_char( ch );  file_n++;
   cur_font[0] = ch - 
#line 17429 "./tex4ht-c.tex"

235  
 + 2;
   cur_font[1] = ch;
   for( i=2; i <= cur_font[0]; i++ ){
      ch = get_char();    idv_char( ch );
      cur_font[i] = ch;   file_n++;         }
   break;  }


#line 5887 "./tex4ht-c.tex"

default: {
  if( (ch < 
#line 17417 "./tex4ht-c.tex"

171  
) || (ch > 
#line 17423 "./tex4ht-c.tex"

234  
)   ){
     if( 
#line 8971 "./tex4ht-c.tex"

(version_id == 
#line 2292 "./tex4ht-c.tex"

10

)
&&
(
  (ch == 
#line 17484 "./tex4ht-c.tex"

251

)
  ||
  (ch == 
#line 17490 "./tex4ht-c.tex"

252

)
  ||
  (ch == 
#line 17496 "./tex4ht-c.tex"

253

)
  ||
  (ch == 
#line 17501 "./tex4ht-c.tex"

254

)
)

 ){
        
#line 8993 "./tex4ht-c.tex"

switch( ch ){
  case 
#line 17490 "./tex4ht-c.tex"

252

:
     
#line 9021 "./tex4ht-c.tex"

{      int i, flags;

(void) get_unt(4);  
(void) get_unt(4);  

    flags = (INTEGER) get_unt(2);



if ((flags & 
#line 8985 "./tex4ht-c.tex"

0x0002

) || (flags & 
#line 8989 "./tex4ht-c.tex"

0x0001

)) {

for (
i =
  (INTEGER) get_unt(1)   
+  (INTEGER) get_unt(1)     
 + (INTEGER) get_unt(1)   
;
i>0;
i--
){ (void) get_unt(1); }





        if( flags & 
#line 8955 "./tex4ht-c.tex"

0x0200

 ){
           (void) get_unt(4);
        }

if( flags & 
#line 8959 "./tex4ht-c.tex"

0x0800

 ){
   int n =  (INTEGER) get_unt(2);  
   for (i = 0; i < n; ++i) {       
         (void) get_unt(4);
   }
   for (i = 0; i < n; ++i) {       
      (void) get_int(4);
}  }

}

}


     break;
  case 
#line 17501 "./tex4ht-c.tex"

254

:
     
#line 9006 "./tex4ht-c.tex"

{
           int i, glyphCount;
   (void) get_unt(4);
   glyphCount = (INTEGER) get_unt(2);
   for( i = 0; i < glyphCount; ++i ){
     (void) get_int(4);
   }
   for (i = 0; i < glyphCount; ++i){
     (void) get_unt(2);
   }
}


     break;
  default:
    printf(" ===> ---------- xdv's idv ------------ <====  %d\n" , ch);
}


     } else { err_i(23); }
  }
  else {  idv_char( ch );  file_n++;
          cur_font[0] = 1;    cur_font[1] = ch;   }
  break;
}

 }

 }

#line 5420 "./tex4ht-c.tex"

if( errCode > 0 ){
   (IGNORED) fprintf(log_file, "tex4ht.c error: %d\n", errCode);
}



#line 5913 "./tex4ht-c.tex"

{                                               int   ch, i, mag;
                                                U_CHAR  str[256];
   (IGNORED) fprintf(log_file, "%s", begin_char_gif);
   dvi_flag = TRUE;
   for( cur_fnt = font_tbl_size; cur_fnt--; ){
      
#line 8815 "./tex4ht-c.tex"

(IGNORED) fprintf(log_file, lg_font_fmt,
  font_tbl[cur_fnt].family_name,
  font_tbl[cur_fnt].font_size,
  (int)( font_tbl[cur_fnt].design_sz * 100 / 655360 / 10),
  font_tbl[cur_fnt].mag);


      for( i = font_tbl[cur_fnt].char_l - font_tbl[cur_fnt].char_f + 1;
           i--; )
         if( get_bit( font_tbl[cur_fnt].gif_on, i) ){
            bop_addr = advance_idv_page( bop_addr, cur_font );
            set_loc( 
#line 17332 "./tex4ht-c.tex"

143 
, (long int) mid_page_x );
            set_loc( 
#line 17374 "./tex4ht-c.tex"

157 
, (long int) mid_page_y );
            
#line 5983 "./tex4ht-c.tex"

{           INTEGER num;
   num = font_tbl[cur_fnt].num;
   if( num <= 
#line 17426 "./tex4ht-c.tex"

63 
 )
                                cond_idv_char( (int) (num + 
#line 17417 "./tex4ht-c.tex"

171  
) );
   else if( dvi_flag ){
     if( (num < 0) || (num > 16777215L) ) idv_int(
#line 17438 "./tex4ht-c.tex"

238  
);
     else if( num < 256 ) { idv_char(
#line 17429 "./tex4ht-c.tex"

235  
); file_n++; }
     else if( num < 65536L ) int_to_dvi((long int) 
#line 17432 "./tex4ht-c.tex"

236  
,2);
     else                   int_to_dvi((long int) 
#line 17435 "./tex4ht-c.tex"

237  
,3);
     cond_idv_char( (int) num );
}  }


            
#line 5941 "./tex4ht-c.tex"

if( (ch = i + font_tbl[cur_fnt].char_f) > 127 )  {
  if( ch < 256 ) cond_idv_char(133);  else  warn_i(23);   }
cond_idv_char( ch );
mag = (int) ((double) font_tbl[cur_fnt].scale /
             font_tbl[cur_fnt].design_sz  * 10 );

#line 10901 "./tex4ht-c.tex"

if( !dos_file_names ){
   if( mag == 10 ) (IGNORED) sprintf(str, "%s-%x%s",
                        font_tbl[cur_fnt].name, ch, gif);
   else            (IGNORED) sprintf(str, "%s-%x-%x%s",
     font_tbl[cur_fnt].name, mag, ch, gif);
}


#line 10929 "./tex4ht-c.tex"


if( dos_file_names ){
   (IGNORED) strcpy((char *) str, (char *) font_tbl[cur_fnt].name);
   dos_gif_file(str, mag, ch);
   strct(str,gif);
}



script(font_gif, job_name ,page_n, str);

        }
   }
   (IGNORED) printf("Execute script `%slg'\n",
                    job_name);
   (IGNORED) fclose( log_file );
}



#line 5494 "./tex4ht-c.tex"

idv_char(
#line 17329 "./tex4ht-c.tex"

142 
);  file_n += 2;
idv_char( 
#line 17323 "./tex4ht-c.tex"

140 
 );
(IGNORED) fseek(dvi_file, begin_postamble, 
#line 2174 "./tex4ht-c.tex"

0
);
begin_postamble  = file_n;
idv_char( 
#line 17466 "./tex4ht-c.tex"

248 
 );   file_n += 5;
idv_int( bop_addr );  (IGNORED) fseek(dvi_file, 5L, 
#line 2177 "./tex4ht-c.tex"

1
);
for( i = 20; i;  i-- ) idv_copy();


#line 5504 "./tex4ht-c.tex"

i = (INTEGER) get_int(2) + 1;    idv_char( (int) i >> 8 );  
idv_char( (int) i & 0xFF );  file_n += 2;
if( !page_n ) page_n++;   idv_char( page_n >> 8 );    
idv_char( (int) page_n & 0xFF );  file_n += 2;
(IGNORED) fseek(dvi_file, 2L, 
#line 2177 "./tex4ht-c.tex"

1
);


#line 5512 "./tex4ht-c.tex"

eof_op_n -= 32;                                       
while( --eof_op_n ) idv_copy();
idv_int(begin_postamble);                   
(IGNORED) fseek(dvi_file, 4L, 
#line 2177 "./tex4ht-c.tex"

1
);  file_n += 4;

#line 5462 "./tex4ht-c.tex"

ch = get_char();
if( id_version != -1 ){ ch = id_version; }
idv_char( ch );


for( i = 8 - file_n % 4;  i;  i-- ) idv_char( 
#line 17420 "./tex4ht-c.tex"

223 
 );



 }


   return 0;
}


