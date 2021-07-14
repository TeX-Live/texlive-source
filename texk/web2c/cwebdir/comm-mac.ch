This is the change file to CWEB's COMMON for porting to Macintoshes.
No changes to CTANGLE or CWEAVE are needed.

(Contributed 13 Oct 2000 by AndPio@aol.com; slightly edited by Don Knuth)

@x in limbo, change the title page document to specify Mac version
  \centerline{(Version 4.5)}
@y
  \centerline{(Version 4.5 for MacOS)}
@z

@x section 23: Make input_ln accept \n, \r, \n\r, or \r\n as line endings
@ In the unlikely event that your standard I/O library does not
support |feof|, |getc|, and |ungetc| you may have to change things here.
@^system dependencies@>

@c
static boolean input_ln( /* copies a line into |buffer| or returns |false| */
FILE *fp) /* what file to read from */
{
  register int c=EOF; /* character read; initialized so some compilers won't complain */
  register char *k; /* where next character goes */
  if (feof(fp)) return false; /* we have hit end-of-file */
  limit = k = buffer; /* beginning of buffer */
  while (k<=buffer_end && (c=getc(fp)) != EOF && c!='\n')
    if ((*(k++) = c) != ' ') limit = k;
  if (k>buffer_end)
    if ((c=getc(fp))!=EOF && c!='\n') {
      ungetc(c,fp); loc=buffer; err_print("! Input line too long");
@.Input line too long@>
    }
  if (c==EOF && limit==buffer) return false; /* there was nothing after
    the last newline */
  return true;
}
@y
@ In the unlikely event that your standard I/O library does not
support |feof|, |getc|, and |ungetc| you may have to change things here.

This |input_ln| function accepts |"\n"|, |"\r"|, |"\n\r"| and |"\r\n"| as
line endings, so that \.{CWEB} will works with ASCII files stored in
\UNIX/, {\mc DOS} or {\mc MAC} format.
@^system dependencies@>

@c
static boolean input_ln( /* copies a line into |buffer| or returns |false| */
FILE *fp) /* what file to read from */
{
  register int  c=EOF; /* character read; initialized so some compilers won't complain */
  register char *k; /* where next character goes */
  if (feof(fp)) return false; /* we have hit end-of-file */
  limit = k = buffer; /* beginning of buffer */
  while (true) {
    c = getc(fp);
    if (c==EOF)  return limit!=buffer; /* |false|, if there was nothing after
      the last newline */
    else if (c=='\n' || c=='\r') { /* we have hit end-of-line */
      int d = getc(fp);
      if (c+d!='\n'+'\r') /* no combination |"\n\r"| or |"\r\n"| */
        ungetc(d,fp);
      return true;
    }
    else if (k>buffer_end) {
      ungetc(c,fp); loc=buffer; err_print("! Input line too long");
      return true;
@.Input line too long@>
    }
    else
      if ((*(k++)=c) != ' ')  limit = k;
  }
}
@z

@x section 27, simply return if no change file was specified
  change_limit=change_buffer; /* this value is used if the change file ends */
  @<Skip over comment lines in the change file; |return| if end of file@>@;
@y
  change_limit=change_buffer; /* this value is used if the change file ends */
  if (change_file_name[0] == '\0') /* no change file specified */
    return; /* so we have reached the end of that file */
  @<Skip over comment lines in the change file; |return| if end of file@>@;
@z

@x section 36, don't try to open a change file if none was specified
if ((change_file=fopen(change_file_name,"r"))==NULL)
       fatal("! Cannot open change file ", change_file_name);
@y
if (change_file_name[0] == '\0')  /* no change file specified */
        change_file = NULL; /* reset at least the |change_file| */
else if ((change_file=fopen(change_file_name,"r"))==NULL)
       fatal("! Cannot open change file ", change_file_name);
@z

@x section 39, declare colon as Mac's path separator
(Colon-separated paths are not supported.)
The remainder of the \.{@@i} line after the file name is ignored.

@y
(Colon-separated path alternatives in the style of \UNIX/ or Kpathsea
are not supported. On a Macintosh, colons are used to separate the names on
different levels of a path.)
The remainder of the \.{@@i} line after the file name is ignored.

@d PATH_SEP ':'   /* MacOS pathname separator */
@^system dependencies@>
@z
@x section 39, use the path separator constant
    cur_file_name[l]='/'; /* \UNIX/ pathname separator */
@y
    cur_file_name[l]=PATH_SEP; /* pathname separator */
@z

@x section 75, explain the convention for omitted change files
An omitted change file argument means that |"/dev/null"| should be used,
@y
An omitted change file argument means that no change file should be used,
@z
@x section 75, make change file name empty when it is unspecified
  strcpy(change_file_name,"/dev/null");
@y
  change_file_name[0]='\0';   /* empty string */
@z
@x section 75, use the Metrowerks |ccommand| to access command lines
  while (--argc > 0) {
@y
  argc = ccommand (&argv); /* use Mac interface to command line */
@^system dependencies@>
  while (--argc > 0) {
@z
@x section 75, use the path separator constant
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
@y
        else if (*s==PATH_SEP) dot_pos=NULL,name_pos=++s;
@z

@x section 85, insert an extra module before the index
@** Index.
@y by putting the new module here, we preserve all the previous section numbers
@ We assume an interface to \CEE/ command-line emulation as supplied by
the |ccommand| function of Metrowerks CodeWarrior, as defined in
the header file \.{console.h}.

@<Include files@>=
#include <console.h>
@^system dependencies@>

@** Index.
@z
