This is the change file for CWEB's COMMON under QSOD/SMSQ
(Contributed by Robert H. Klein, September 1994)

This change file is intended for use with C68 v4.13 (or later).
compile with
ex <dev_>cc;'-v -h -c -=500000 -DCWEBINPUTS=flp2_ common_c'



@x
\def\v{\char'174} % vertical (|) in typewriter font

\def\title{Common code for CTANGLE and CWEAVE (Version 4.5)}
\def\topofcontents{\null\vfill
  \centerline{\titlefont Common code for {\ttitlefont CTANGLE} and
    {\ttitlefont CWEAVE}}
  \vskip 15pt
  \centerline{(Version 4.5)}
  \vfill}
\def\botofcontents{\vfill
\noindent
@y
\def\v{\char'174} % vertical (|) in typewriter font

\def\title{Common code for CTANGLE and CWEAVE (QL Version 4.5)}
\def\topofcontents{\null\vfill
  \centerline{\titlefont Common code for {\ttitlefont CTANGLE} and
    {\ttitlefont CWEAVE}}
  \vskip 15pt
  \centerline{(Version 4.5)}
  \vfill}
\def\botofcontents{\vfill
\noindent
@z


For use on QDOS/SMSQ systems the variable |max_file_name_length| is shortened
to 42 characters, i.e. 5 characters for the device name, 36 characters
for the file name plus one character as string terminator. (Note that
(current) QDOS/SMSQ file systems have a limitation of 36 characters as
maximum length for a file name.
@x
@d max_file_name_length 60
@y
@d max_file_name_length 42
@z


@x
    cur_file_name[l]='/'; /* \UNIX/ pathname separator */
@y
    cur_file_name[l]='_'; /* QDOS/SMSQ pathname separator */
@z


@x
@ We now must look at the command line arguments and set the file names
accordingly.  At least one file name must be present: the \.{CWEB}
file.  It may have an extension, or it may omit the extension to get |".w"| or
|".web"| added.  The \TEX/ output file name is formed by replacing the \.{CWEB}
file name extension by |".tex"|, and the \CEE/ file name by replacing
the extension by |".c"|, after removing the directory name (if any).

If there is a second file name present among the arguments, it is the
change file, again either with an extension or without one to get |".ch"|.
An omitted change file argument means that |"/dev/null"| should be used,
when no changes are desired.
@^system dependencies@>

If there's a third file name, it will be the output file.
@y
@ We now must look at the command line arguments and set the file names
accordingly.  At least one file name must be present: the \.{CWEB}
file.  It may have an extension, or it may omit the extension to get |"_w"| or
|"_web"| added.  The \TEX/ output file name is formed by replacing the \.{CWEB}
file name extension by |"_tex"|, and the \CEE/ file name by replacing
the extension by |"_c"|, after removing the directory name (if any).

If there is a second file name present among the arguments, it is the
change file, again either with an extension or without one to get |"_ch"|.
An omitted change file argument means that |"NUL"| should be used,
when no changes are desired.
@^system dependencies@>

If there's a third file name, it will be the output file.

Because |"_"| is a directory {\em and} extension separator, \.{CWEB} will
always use the {\em full} name (i.e. with full path).  Fortunately the
routine has been written to search for the last ``dot'', so the altered
version will search for the last |"_"| (including directory separators),
so we have what we want.
@z


@x
  char *dot_pos; /* position of |'.'| in the argument */
@y
  char *dot_pos; /* position of |'_'| in the argument */
@z


@x
  strcpy(change_file_name,"/dev/null");
@y
  strcpy(change_file_name,"NUL");
@z


@x
        if (*s=='.') dot_pos=s++;
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
        else s++;
@y
        if (*s=='_') dot_pos=s++;
        else if (*s=='_') dot_pos=NULL,name_pos=++s;
        else s++;
@z


@x
@ We use all of |*argv| for the |web_file_name| if there is a |'.'| in it,
otherwise we add |".w"|. If this file can't be opened, we prepare an
|alt_web_file_name| by adding |"web"| after the dot.
The other file names come from adding other things
after the dot.  We must check that there is enough room in
|web_file_name| and the other arrays for the argument.
@y
@ We use all of |*argv| for the |web_file_name| if there is a |'_'| in it,
otherwise we add |"_w"|. If this file can't be opened, we prepare an
|alt_web_file_name| by adding |"web"| after the dot.
The other file names come from adding other things
after the dot.  We must check that there is enough room in
|web_file_name| and the other arrays for the argument.

If you've read the section before carefully you'll have noticed, that
QDOS/SMSQ file names contain almost a |"_"|, so this routine will not work,
i.e. you'll {\em have} to add the |"_w"| always.  Nevertheless I adapted
the routine as if it would work.
@z


@x
@<Make |web_file_name|...@>=
{
  if (s-*argv > max_file_name_length-5)
    @<Complain about argument length@>@;
  if (dot_pos==NULL)
    sprintf(web_file_name,"%s.w",*argv);
  else {
    strcpy(web_file_name,*argv);
    *dot_pos='\0'; /* string now ends where the dot was */
  }
  sprintf(alt_web_file_name,"%s.web",*argv);
  sprintf(tex_file_name,"%s.tex",name_pos); /* strip off directory name */
  sprintf(idx_file_name,"%s.idx",name_pos);
  sprintf(scn_file_name,"%s.scn",name_pos);
  sprintf(C_file_name,"%s.c",name_pos);
  found_web=true;
}
@y
@<Make |web_file_name|...@>=
{
  if (s-*argv > max_file_name_length-5)
    @<Complain about argument length@>;
  if (dot_pos==NULL)
    sprintf(web_file_name,"%s_w",*argv);
  else {
    strcpy(web_file_name,*argv);
    *dot_pos='\0'; /* string now ends where the dot was */
  }
  sprintf(alt_web_file_name,"%s_web",*argv);
  sprintf(tex_file_name,"%s_tex",name_pos); /* strip off directory name */
  sprintf(idx_file_name,"%s_idx",name_pos);
  sprintf(scn_file_name,"%s_scn",name_pos);
  sprintf(C_file_name,"%s_c",name_pos);
  found_web=true;
}
@z


@x
@ @<Make |change_file_name|...@>=
{
  if (strcmp(*argv,"-")!=0) {
    if (s-*argv > max_file_name_length-4)
      @<Complain about argument length@>@;
    if (dot_pos==NULL)
      sprintf(change_file_name,"%s.ch",*argv);
    else strcpy(change_file_name,*argv);
  }
  found_change=true;
}
@y
@ @<Make |change_file_name|...@>=
{
  if (strcmp(*argv,"-")!=0) {
    if (s-*argv > max_file_name_length-4)
      @<Complain about argument length@>;
    if (dot_pos==NULL)
      sprintf(change_file_name,"%s_ch",*argv);
    else strcpy(change_file_name,*argv);
    found_change=true;
  }
}
@z


@x
@ @<Override...@>=
{
  if (s-*argv > max_file_name_length-5)
    @<Complain about argument length@>@;
  if (dot_pos==NULL) {
    sprintf(tex_file_name,"%s.tex",*argv);
    sprintf(idx_file_name,"%s.idx",*argv);
    sprintf(scn_file_name,"%s.scn",*argv);
    sprintf(C_file_name,"%s.c",*argv);
  } else {
    strcpy(tex_file_name,*argv);
    strcpy(C_file_name,*argv);
    if (make_xrefs) { /* indexes will be generated */
      *dot_pos='\0';
      sprintf(idx_file_name,"%s.idx",*argv);
      sprintf(scn_file_name,"%s.scn",*argv);
    }
  }
  found_out=true;
}
@y
@ @<Override...@>=
{
  if (s-*argv > max_file_name_length-5)
    @<Complain about argument length@>;
  if (dot_pos==NULL) {
    sprintf(tex_file_name,"%s_tex",*argv);
    sprintf(idx_file_name,"%s_idx",*argv);
    sprintf(scn_file_name,"%s_scn",*argv);
    sprintf(C_file_name,"%s_c",*argv);
  } else {
    strcpy(tex_file_name,*argv);
    strcpy(C_file_name,*argv);
    if (make_xrefs) { /* indexes will be generated */
      *dot_pos='\0';
      sprintf(idx_file_name,"%s_idx",*argv);
      sprintf(scn_file_name,"%s_scn",*argv);
    }
  }
  found_out=true;
}
@z


@x
@ @<Print usage error message and quit@>=
{
if (program==ctangle)
  fatal(
"! Usage: ctangle [options] webfile[.w] [{changefile[.ch]|-} [outfile[.c]]]\n"
   ,"");
@.Usage:@>
else fatal(
"! Usage: cweave [options] webfile[.w] [{changefile[.ch]|-} [outfile[.tex]]]\n"
   ,"");
}
@y
@ @<Print usage error message and quit@>=
{
if (program==ctangle)
  fatal(
"! Usage: ctangle [options] webfile[_w] [{changefile[_ch]|-} [outfile[_c]]]\n"
   ,"");
@.Usage:@>
else fatal(
"! Usage: cweave [options] webfile[_w] [{changefile[_ch]|-} [outfile[_tex]]]\n"
   ,"");
}
@z
