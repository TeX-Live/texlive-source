% This file is part of CWEB.
% This program by Silvio Levy and Donald E. Knuth
% is based on a program by Knuth.
% It is distributed WITHOUT ANY WARRANTY, express or implied.
% Version 3.64 --- February 2017 (works also with later versions)

% Copyright (C) 1987,1990,1993 Silvio Levy and Donald E. Knuth

% Permission is granted to make and distribute verbatim copies of this
% document provided that the copyright notice and this permission notice
% are preserved on all copies.

% Permission is granted to copy and distribute modified versions of this
% document under the conditions for verbatim copying, provided that the
% entire resulting derived work is distributed under the terms of a
% permission notice identical to this one.

% Amendments to 'common.h' resulting in this extended 'comm-foo.h' were
% created by numerous collaborators over the course of many years.

% Please send comments, suggestions, etc. to tex-k@@tug.org.

% The next few sections contain stuff from the file |"common.w"| that has
% to be included in both |"ctangle.w"| and |"cweave.w"|. It appears in this
% file |"common.h"|, which needs to be updated when |"common.w"| changes.

First comes general stuff:

@d ctangle 0
@d cweave 1
@d ctwill 2

@s boolean int
@s uint8_t int
@s uint16_t int

@<Common code for \.{CWEAVE} and \.{CTANGLE}@>=
typedef bool boolean;
typedef uint8_t eight_bits;
typedef uint16_t sixteen_bits;
extern int program; /* \.{CWEAVE} or \.{CTANGLE} or \.{CTWILL}? */
extern int phase; /* which phase are we in? */

@ @<Include files@>=
#ifndef HAVE_GETTEXT
#define HAVE_GETTEXT 0
#endif
@#
#if HAVE_GETTEXT
#include <libintl.h>
#else
#define gettext(A) A
#endif
@#
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

@ Code related to the character set:
@^ASCII code dependencies@>

@d and_and 04 /* `\.{\&\&}'\,; corresponds to MIT's {\tentex\char'4} */
@d lt_lt 020 /* `\.{<<}'\,;  corresponds to MIT's {\tentex\char'20} */
@d gt_gt 021 /* `\.{>>}'\,;  corresponds to MIT's {\tentex\char'21} */
@d plus_plus 013 /* `\.{++}'\,;  corresponds to MIT's {\tentex\char'13} */
@d minus_minus 01 /* `\.{--}'\,;  corresponds to MIT's {\tentex\char'1} */
@d minus_gt 031 /* `\.{->}'\,;  corresponds to MIT's {\tentex\char'31} */
@d non_eq 032 /* `\.{!=}'\,;  corresponds to MIT's {\tentex\char'32} */
@d lt_eq 034 /* `\.{<=}'\,;  corresponds to MIT's {\tentex\char'34} */
@d gt_eq 035 /* `\.{>=}'\,;  corresponds to MIT's {\tentex\char'35} */
@d eq_eq 036 /* `\.{==}'\,;  corresponds to MIT's {\tentex\char'36} */
@d or_or 037 /* `\.{\v\v}'\,;  corresponds to MIT's {\tentex\char'37} */
@d dot_dot_dot 016 /* `\.{...}'\,;  corresponds to MIT's {\tentex\char'16} */
@d colon_colon 06 /* `\.{::}'\,;  corresponds to MIT's {\tentex\char'6} */
@d period_ast 026 /* `\.{.*}'\,;  corresponds to MIT's {\tentex\char'26} */
@d minus_gt_ast 027 /* `\.{->*}'\,;  corresponds to MIT's {\tentex\char'27} */

@<Common code...@>=
char section_text[longest_name+1]; /* name being sought for */
char *section_text_end = section_text+longest_name; /* end of |section_text| */
char *id_first; /* where the current identifier begins in the buffer */
char *id_loc; /* just after the current identifier in the buffer */

@ Code related to input routines:

@d xisalpha(c) (isalpha((eight_bits)c)&&((eight_bits)c<0200))
@d xisdigit(c) (isdigit((eight_bits)c)&&((eight_bits)c<0200))
@d xisspace(c) (isspace((eight_bits)c)&&((eight_bits)c<0200))
@d xislower(c) (islower((eight_bits)c)&&((eight_bits)c<0200))
@d xisupper(c) (isupper((eight_bits)c)&&((eight_bits)c<0200))
@d xisxdigit(c) (isxdigit((eight_bits)c)&&((eight_bits)c<0200))

@<Common code...@>=
extern char buffer[]; /* where each line of input goes */
extern char *buffer_end; /* end of |buffer| */
extern char *loc; /* points to the next character to be read from the buffer*/
extern char *limit; /* points to the last character in the buffer */

@ Code related to identifier and section name storage:
@d length(c) (size_t)((c+1)->byte_start-(c)->byte_start) /* the length of a name */
@d print_id(c) term_write((c)->byte_start,length((c))) /* print identifier */
@d llink link /* left link in binary search tree for section names */
@d rlink dummy.Rlink /* right link in binary search tree for section names */
@d root name_dir->rlink /* the root of the binary search tree
  for section names */
@d chunk_marker 0

@<Common code...@>=
typedef struct name_info {
  char *byte_start; /* beginning of the name in |byte_mem| */
  struct name_info *link;
  union {
    struct name_info *Rlink; /* right link in binary search tree for section
      names */
    char Ilk; /* used by identifiers in \.{CWEAVE} only */
  } dummy;
  void *equiv_or_xref; /* info corresponding to names */
} name_info; /* contains information about an identifier or section name */
typedef name_info *name_pointer; /* pointer into array of \&{name\_info}s */
typedef name_pointer *hash_pointer;
extern char byte_mem[]; /* characters of names */
extern char *byte_mem_end; /* end of |byte_mem| */
extern name_info name_dir[]; /* information about names */
extern name_pointer name_dir_end; /* end of |name_dir| */
extern name_pointer name_ptr; /* first unused position in |name_dir| */
extern char *byte_ptr; /* first unused position in |byte_mem| */
extern name_pointer hash[]; /* heads of hash lists */
extern hash_pointer hash_end; /* end of |hash| */
extern hash_pointer h; /* index into hash-head array */
extern boolean names_match(name_pointer,const char *,size_t,eight_bits);@/
extern name_pointer id_lookup(const char *,const char *,char);
   /* looks up a string in the identifier table */
extern name_pointer section_lookup(char *,char *,int); /* finds section name */
extern void init_node(name_pointer);@/
extern void init_p(name_pointer,eight_bits);@/
extern void print_prefix_name(name_pointer);@/
extern void print_section_name(name_pointer);@/
extern void sprint_section_name(char *,name_pointer);@/

@ Code related to error handling:
@d spotless 0 /* |history| value for normal jobs */
@d harmless_message 1 /* |history| value when non-serious info was printed */
@d error_message 2 /* |history| value when an error was noted */
@d fatal_message 3 /* |history| value when we had to stop prematurely */
@d mark_harmless {if (history==spotless) history=harmless_message;}
@d mark_error history=error_message
@d _(STRING) gettext(STRING)
@d confusion(s) fatal(_("! This can't happen: "),s)

@<Common...@>=
extern int history; /* indicates how bad this run was */
extern int wrap_up(void); /* indicate |history| and exit */
extern void err_print(const char *); /* print error message and context */
extern void fatal(const char *,const char *); /* issue error message and die */
extern void overflow(const char *); /* succumb because a table has overflowed */

@ Code related to file handling:
@f line x /* make |line| an unreserved word */
@d max_file_name_length 1024
@d cur_file file[include_depth] /* current file */
@d cur_file_name file_name[include_depth] /* current file name */
@d web_file_name file_name[0] /* main source file name */
@d cur_line line[include_depth] /* number of current line in current file */

@<Common code...@>=
extern int include_depth; /* current level of nesting */
extern FILE *file[]; /* stack of non-change files */
extern FILE *change_file; /* change file */
extern char C_file_name[]; /* name of |C_file| */
extern char tex_file_name[]; /* name of |tex_file| */
extern char idx_file_name[]; /* name of |idx_file| */
extern char scn_file_name[]; /* name of |scn_file| */
extern char check_file_name[]; /* name of |check_file| */
extern char file_name[][max_file_name_length];
  /* stack of non-change file names */
extern char change_file_name[]; /* name of change file */
extern int line[]; /* number of current line in the stacked files */
extern int change_line; /* number of current line in change file */
extern int change_depth; /* where \.{@@y} originated during a change */
extern boolean input_has_ended; /* if there is no more input */
extern boolean changing; /* if the current line is from |change_file| */
extern boolean web_file_open; /* if the web file is being read */
extern boolean get_line(void); /* inputs the next line */
extern void check_complete(void); /* checks that all changes were picked up */
extern void reset_input(void); /* initialize to read the web file and change file */

@ Code related to section numbers:
@<Common code...@>=
extern sixteen_bits section_count; /* the current section number */
extern boolean changed_section[]; /* is the section changed? */
extern boolean change_pending; /* is a decision about change still unclear? */
extern boolean print_where; /* tells \.{CTANGLE} to print line and file info */

@ Code related to command line arguments:
@d show_banner flags['b'] /* should the banner line be printed? */
@d show_progress flags['p'] /* should progress reports be printed? */
@d show_happiness flags['h'] /* should lack of errors be announced? */
@d make_xrefs flags['x'] /* should cross references be output? */

@<Common code...@>=
extern int argc; /* copy of |ac| parameter to |main| */
extern char **argv; /* copy of |av| parameter to |main| */
extern boolean flags[]; /* an option for each 7-bit code */

@ Code relating to output:
@d update_terminal fflush(stdout) /* empty the terminal output buffer */
@d new_line putchar('\n') @d putxchar putchar
@d term_write(a,b) fflush(stdout),fwrite(a,sizeof(char),b,stdout)
@d C_printf(c,a) fprintf(C_file,c,a)
@d C_putc(c) putc(c,C_file)

@<Common code...@>=
extern FILE *C_file; /* where output of \.{CTANGLE} goes */
extern FILE *tex_file; /* where output of \.{CWEAVE} goes */
extern FILE *idx_file; /* where index from \.{CWEAVE} goes */
extern FILE *scn_file; /* where list of sections from \.{CWEAVE} goes */
extern FILE *check_file; /* temporary output file */
extern FILE *active_file; /* currently active file for \.{CWEAVE} output */

@ The procedure that gets everything rolling:

@<Common code...@>=
extern void common_init(void);@/
extern void print_stats(void);@/
extern void cb_show_banner(void);@/
