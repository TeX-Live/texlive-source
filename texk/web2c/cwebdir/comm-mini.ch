Limbo.

@x
\def\title{Common code for CTANGLE and CWEAVE (4.10 [\TeX~Live])}
@y
\def\title{COMMON (4.10 [\TeX~Live])}
\def\contentspagenumber{0}
@z

Section 4.

@x
internationalization.
@y
internationalization.
@-a@>
@-s@>
@-HAVE_GETTEXT@>
@z

Section 5.

@x
@ Code related to the character set:
@y
@ Code related to the character set:
@-c@>
@z

Section 6.

@x
@ Code related to input routines:
@y
@ Code related to input routines:
@-c@>
@z

Section 10.

@x
@ Code related to identifier and section name storage:
@y
@ Code related to identifier and section name storage:
@-c@>
@z

Section 15.

@x
@ Code related to output:
@y
@r @ Code related to output:
@-a@>
@-b@>
@z

Section 24.

@x
@ @<Predecl...@>=@+static boolean input_ln(FILE *);
@y
@ @<Predecl...@>=@+static boolean input_ln(FILE *);
@-input_ln@>
@$input_ln {COMMON}23 \&{static} \&{boolean} (\,)@>
@z

ẞection 28.

@x
@ @<Predecl...@>=@+static void prime_the_change_buffer(void);
@y
@ @<Predecl...@>=@+static void prime_the_change_buffer(void);
@-prime_the_change_buffer@>
@$prime_the_change_buffer {COMMON}27 \&{static} \&{void} (\,)@>
@z

Section 32.

@x
current line is nonempty.
@y
current line is nonempty.
@-b@>
@z

Section 33.

@x
@ @<Predecl...@>=@+static void check_change(void);
@y
@ @<Predecl...@>=@+static void check_change(void);
@-check_change@>
@$check_change {COMMON}32 \&{static} \&{void} (\,)@>
@z

Section 36.

@x
@ When an \.{@@i} line is found in the |cur_file|, we must temporarily
@y
@r @ When an \.{@@i} line is found in the |cur_file|, we must temporarily
@z

Section 48.

@x
id_lookup( /* looks up a string in the identifier table */
@y
id_lookup( /* looks up a string in the identifier table */
@-h@>
@-id_lookup@>
@$id_lookup {COMMON}48 \&{name\_pointer} (\,)@>
@z

Section 49.

@x
h=(int)((eight_bits)*i);
@y
h=(int)((eight_bits)*i);
@%
@$h {COMMON}48 \&{int}@>
@z

Section 52.

@x
@d first_chunk(p) ((p)->byte_start+2)
@y
@-m@>
@d first_chunk(p) ((p)->byte_start+2)
@z

Section 53.

@x
  *dest='\0';
}

@ @c
@y
  *dest='\0';
}

@r @ @c
@z

Section 56.

@x
static int web_strcmp( /* fuller comparison than |strcmp| */
@y
static int web_strcmp( /* fuller comparison than |strcmp| */
@-web_strcmp@>
@$web_strcmp {COMMON}56 \&{static} \&{int} ${}(\,){}$@>
@z

Section 57.

@x
add_section_name( /* install a new node in the tree */
@y
add_section_name( /* install a new node in the tree */
@-add_section_name@>
@$add_section_name {COMMON}57 \&{static} \&{name\_pointer} ${}(\,){}$@>
@z

Section 59.

@x
@ The |section_lookup| procedure is supposed to find a
@y
@r @ The |section_lookup| procedure is supposed to find a
@z

@x
section_lookup( /* find or install section name in tree */
@y
section_lookup( /* find or install section name in tree */
@-section_lookup@>
@$section_lookup {COMMON}59 \&{name\_pointer} (\,)@>
@z

Section 62.

@x
@ Although error messages are given in anomalous cases, we do return the
@y
@r @ Although error messages are given in anomalous cases, we do return the
@z

Section 64.

@x
@ @<Predec...@>=@+static int section_name_cmp(char **,size_t,name_pointer);
@y
@ @<Predec...@>=@+static int section_name_cmp(char **,size_t,name_pointer);
@-section_name_cmp@>
@$section_name_cmp {COMMON}63 \&{static} \&{int} (\,)@>
@z

Section 66.

@x
err_print( /* prints `\..' and location of error message */
@y
err_print( /* prints `\..' and location of error message */
@-err_print@>
@$err_print {COMMON}66 \&{void} (\,)@>
@z

Section 75.

@x
systems the contents of the compile-time variable |DEV_NULL| (\TeX~Live) or
@y
systems the contents of the compile-time variable |DEV_NULL| (\TeX~Live) or
@-DEV_NULL@>
@-_DEV_NULL@>
@z

Section 76.

@x
@ @<Pred...@>=@+static void scan_args(void);
@y
@ @<Pred...@>=@+static void scan_args(void);
@-scan_args@>
@$scan_args {COMMON}75 \&{static} \&{void} (\,)@>
@z

Section 77.

@x
@ We use all of |*argv| for the |web_file_name| if there is a |'.'| in it,
@y
@r @ We use all of |*argv| for the |web_file_name| if there is a |'.'| in it,
@z

Section 87.

@x
|bindtextdomain| argument string need a few extra variables.
@y
|bindtextdomain| argument string need a few extra variables.
@-SEPARATORS@>
@z

Section 91.

@x
@.cweb.mo@>
@y
@.cweb.mo@>
@-HAVE_GETTEXT@>
@-TEXMFLOCALEDIR@>
@-a@>
@-b@>
@z

Section 93.

@x
@d kpse_find_cweb(name) kpse_find_file(name,kpse_cweb_format,true)
@y
@d kpse_find_cweb(name) kpse_find_file(name,kpse_cweb_format,true)
@-name@>
@z

Section 96.

@x
#include "help.h" /* |@!CTANGLEHELP|, |@!CWEAVEHELP|, |@!CTWILLHELP| */
@y
#include "help.h" /* |@!CTANGLEHELP|, |@!CWEAVEHELP|, |@!CTWILLHELP| */
@$CTANGLEHELP "help.h" \zip@>
@$CTWILLHELP "help.h" \zip@>
@$CWEAVEHELP "help.h" \zip@>
@-CWEB@>
@z

Section 85/102.

@x
@** Index.
@y
@z
