@q Changes for CWEB in TeX Live from numerous contributors.              @>
@q This file is in the Public Domain.                                    @>

@q Most of the original Kpathsea changes by Wlodek Bzyl and Olaf Weber   @>
@q were merged with the set of change files of the CWEBbin project;      @>
@q see https://github.com/ascherer/cwebbin for the original parts.       @>

@q This stripped change file {comm,ctang,cweav,ctwill,cwebman}-w2c.ch    @>
@q has been created from the elaborate set of change files               @>
@q {comm,ctang,cweav,cwebman}-patch.ch,                                  @>
@q {comm,cweav,cwebman}-extensions.ch, {comm,ctang,cweav}-output.ch,     @>
@q {comm,ctang,cweav}-i18n.ch, and cweav-twill.ch for CTWILL, and        @>
@q {comm,ctang,cweav,ctwill,cwebman}-texlive.ch with the 'tie' processor @>
@q and is used as a monolithic changefile for {common,ctangle,cweave}.w  @>
@q and cwebman.tex in TeX Live.                                          @>

@q Please send comments, suggestions, etc. to tex-k@@tug.org.            @>

@x [0.0] l.30
\def\title{CTANGLE (Version 4.12.2)}
@y
\def\title{CTANGLE (Version 4.12.2 [\TeX~Live])}
@z

@x [0.0] l.34
  \centerline{(Version 4.12.2)}
@y
  \centerline{(Version 4.12.2 [\TeX~Live])}
@z

@x [0.0] l.36
\def\botofcontents{\vfill
@y
\def\covernote{\vbox{%
@z

@x [0.0] l.49
}
@y
}}\datecontentspage
@z

@x [1.1] l.64
@d banner "This is CTANGLE (Version 4.12.2)"
@y
@d banner "This is CTANGLE, Version 4.12.2"
  /* will be extended by the \TeX~Live |versionstring| */
@z

@x [1.2] l.90
  if (show_banner) puts(banner); /* print a ``banner line'' */
@y
  if (show_banner) cb_show_banner(); /* print a ``banner line'' */
@z

@x [1.3] l.101
@i common.h
@y
@i comm-w2c.h
@z

@x [2.20] l.129
@ @d max_texts 4000 /* number of replacement texts, must be less than 10240 */
@d max_toks 270000 /* number of bytes in compressed \CEE/ code */
@y
@ @d max_texts 10239 /* number of replacement texts, must be less than 10240 */
@d max_toks 1000000 /* number of bytes in compressed \CEE/ code */
@z

@x [3.29] l.237
  if (tok_ptr+2>tok_mem_end) overflow("token");
@y
  if (tok_ptr+2>tok_mem_end) overflow(_("token"));
@z

@x [4.35] l.320
  if (stack_ptr==stack_end) overflow("stack");
@y
  if (stack_ptr==stack_end) overflow(_("stack"));
@z

@x [4.40] l.412
    printf("%s","\n! Not present: <");
@y
    printf("%s",_("\n! Not present: <"));
@z

@x [5.47] l.510
    else overflow("output files");
@y
    else overflow(_("output files"));
@z

@x [6.48] l.526
    printf("%s","\n! No program text was specified."); mark_harmless();
@y
    printf("%s",_("\n! No program text was specified.")); mark_harmless();
@z

@x [6.48] l.532
        "\nWriting the output file (%s):" : @|
        "\nWriting the output files: (%s)",C_file_name);
@y
       _("\nWriting the output file (%s):") : @|
       _("\nWriting the output files: (%s)"),C_file_name);
@z

@x [6.48] l.542
      printf("%s","Done.");
@y
      printf("%s",_("Done."));
@z

@x [6.50] l.556
@<Write all the named output files@>=
for (an_output_file=end_output_files; an_output_file>cur_out_file;) {
    an_output_file--;
    sprint_section_name(output_file_name,*an_output_file);
    fclose(C_file);
    if ((C_file=fopen(output_file_name,"wb"))==NULL)
      fatal("! Cannot open output file ",output_file_name);
@.Cannot open output file@>
    if (show_progress) { printf("\n(%s)",output_file_name); update_terminal(); }
    cur_line=1;
    @<Initialize the secondary output@>@;
    @<Output material...@>@;
}
@y
@<Write all the named output files@>=
if (check_for_change) {
  fclose(C_file); C_file=NULL;
  @<Update the primary result when it has changed@>@;
}
for (an_output_file=end_output_files; an_output_file>cur_out_file;) {
  an_output_file--;
  sprint_section_name(output_file_name,*an_output_file);
  if (check_for_change) @<Open the intermediate output file@>@;
  else {
    fclose(C_file);
    if ((C_file=fopen(output_file_name,"wb"))==NULL)
      fatal(_("! Cannot open output file "),output_file_name);
@.Cannot open output file@>
  }
  if (show_progress) { printf("\n(%s)",output_file_name); update_terminal(); }
  cur_line=1;
  @<Initialize the secondary output@>@;
  @<Output material...@>@;
  if (check_for_change) {
    fclose(C_file); C_file=NULL;
    @<Update the secondary results when they have changed@>@;
  }
}
if (check_for_change)
  strcpy(check_file_name,""); /* We want to get rid of the temporary file */
@z

@x [6.54] l.616
          else if (a<050000) confusion("macro defs have strange char");
@y
          else if (a<050000) confusion(_("macro defs have strange char"));
@z

@x [6.59] l.703
@ @<Case of an identifier@>=@t\1\quad@>
@y
@ Nowadays, most computer files are encoded in some form of ``Unicode''. A very
convenient special case is ``UTF-8'', a variable-length multi-byte encoding. In
order to avoid major surgery for the transliteration feature---as tempting as
the extended notation \.{@@l c3bc ue} might be---, \.{CTANGLE}
accepts the \.{+u} option to activate a ``poor man's UTF-8'' mechanism. The
first in a sequence of up to four high-bit bytes (amounting to more than
$2^{20}$~possible character representations) determines the number of bytes
used to represent the next character. Instead of extending the |translit| table
to this multi-byte scenario, we simply strip all but the last byte and use this
as the transliteration index.

% Exercise 11.6 from the TeXbook:
\def\frac#1/#2{\leavevmode\kern.1em\raise.5ex\hbox{\the\scriptfont0 #1}
  \kern-.1em/\kern-.15em\lower.25ex\hbox{\the\scriptfont0 #2}}

\&{Example:} While in ``classic ASCII'' the German word {\it gr\"un\/} could be
treated with transliteration \.{@@l fc ue} (from codepage ISO/IEC~8859-1) to
get \.{gruen} as suggested above, in UTF-8 you'd be advised to use \.{@@l bc
ue} instead, because character {\it \"u\/} (latin small letter u with
diaeresis) is encoded as the two-byte sequence \.{c3 bc}, indicated by the
initial three bits of byte \.{c3} (\.{1100 0011}). Note that this simple
approach leads to the collision with character $\frac1/4$ (vulgar fraction one
quarter) with its two-byte encoding \.{c2 bc}.

@d transliterate_utf_eight flags['u']

@<Case of an identifier@>=@t\1\quad@>
@z

@x [6.59] l.708
    if (ishigh(*j)) C_printf("%s",translit[(eight_bits)(*j)-0200]);
@^high-bit character handling@>
@y
    if (ishigh(*j)) {
@^high-bit character handling@>
      if (transliterate_utf_eight) {
        if ((eight_bits)(*j)>=0360) j+=3;
        else if ((eight_bits)(*j)>=0340) j+=2;
        else if ((eight_bits)(*j)>=0300) j+=1;
      }
      C_printf("%s",translit[(eight_bits)(*j)-0200]);
    }
@z

@x [7.67] l.840
          err_print("! Input ended in mid-comment");
@y
          err_print(_("! Input ended in mid-comment"));
@z

@x [7.67] l.853
        err_print("! Section name ended in mid-comment"); loc--;
@y
        err_print(_("! Section name ended in mid-comment")); loc--;
@z

@x [8.74] l.1004
        err_print("! String didn't end"); loc=limit; break;
@y
        err_print(_("! String didn't end")); loc=limit; break;
@z

@x [8.74] l.1008
        err_print("! Input ended in middle of string"); loc=buffer; break;
@y
        err_print(_("! Input ended in middle of string")); loc=buffer; break;
@z

@x [8.74] l.1026
    printf("%s","\n! String too long: ");
@y
    printf("%s",_("\n! String too long: "));
@z

@x [8.75] l.1041
  case translit_code: err_print("! Use @@l in limbo only"); continue;
@y
  case translit_code: err_print(_("! Use @@l in limbo only")); continue;
@z

@x [8.75] l.1046
      err_print("! Double @@ should be used in control text");
@y
      err_print(_("! Double @@ should be used in control text"));
@z

@x [8.76] l.1070
        err_print("! Double @@ should be used in ASCII constant");
@y
        err_print(_("! Double @@ should be used in ASCII constant"));
@z

@x [8.76] l.1076
        err_print("! String didn't end"); loc=limit-1; break;
@y
        err_print(_("! String didn't end")); loc=limit-1; break;
@z

@x [8.79] l.1108
    err_print("! Input ended in section name");
@y
    err_print(_("! Input ended in section name"));
@z

@x [8.79] l.1121
  printf("%s","\n! Section name too long: ");
@y
  printf("%s",_("\n! Section name too long: "));
@z

@x [8.80] l.1135
    err_print("! Section name didn't end"); break;
@y
    err_print(_("! Section name didn't end")); break;
@z

@x [8.80] l.1139
    err_print("! Nesting of section names not allowed"); break;
@y
    err_print(_("! Nesting of section names not allowed")); break;
@z

@x [8.81] l.1153
if (loc>=limit) err_print("! Verbatim string didn't end");
@y
if (loc>=limit) err_print(_("! Verbatim string didn't end"));
@z

@x [9.82] l.1177
@d app_repl(c) {
  if (tok_ptr==tok_mem_end) overflow("token");
  else *(tok_ptr++)=(eight_bits)c;
}
@y
@d app_repl(c) {
  if (tok_ptr==tok_mem_end) overflow(_("token"));
  else *(tok_ptr++)=(eight_bits)c;
}
@z

@x [9.83] l.1204
  if (text_ptr>text_info_end) overflow("text");
@y
  if (text_ptr>text_info_end) overflow(_("text"));
@z

@x [9.86] l.1250
case output_defs_code: if (t!=section_name) err_print("! Misplaced @@h");
@y
case output_defs_code: if (t!=section_name) err_print(_("! Misplaced @@h"));
@z

@x [9.86] l.1268
    err_print("! @@d, @@f and @@c are ignored in C text"); continue;
@y
    err_print(_("! @@d, @@f and @@c are ignored in C text")); continue;
@z

@x [9.87] l.1278
  if (*try_loc=='=') err_print ("! Missing `@@ ' before a named section");
@y
  if (*try_loc=='=') err_print (_("! Missing `@@ ' before a named section"));
@z

@x [9.88] l.1295
      else err_print("! Double @@ should be used in string");
@y
      else err_print(_("! Double @@ should be used in string"));
@z

@x [9.89] l.1344
    default: err_print("! Unrecognized escape sequence");
@y
    default: err_print(_("! Unrecognized escape sequence"));
@z

@x [10.93] l.1417
  err_print("! Definition flushed, must start with identifier");
@y
  err_print(_("! Definition flushed, must start with identifier"));
@z

@x [10.100] l.1498
            err_print("! Double @@ should be used in control text");
@y
            err_print(_("! Double @@ should be used in control text"));
@z

@x [10.100] l.1501
          } @=/* otherwise fall through */@>@;
        default: err_print("! Double @@ should be used in limbo");
@y
          } @=/* otherwise fall through */@>@;
        default: err_print(_("! Double @@ should be used in limbo"));
@z

@x [10.102] l.1516
    err_print("! Improper hex number following @@l");
@y
    err_print(_("! Improper hex number following @@l"));
@z

@x [10.102] l.1526
      err_print("! Replacement string in @@l too long");
@y
      err_print(_("! Replacement string in @@l too long"));
@z

@x [10.103] l.1540
  puts("\nMemory usage statistics:");
  printf("%td names (out of %ld)\n",@^system dependencies@>
          (ptrdiff_t)(name_ptr-name_dir),(long)max_names);
  printf("%td replacement texts (out of %ld)\n",
          (ptrdiff_t)(text_ptr-text_info),(long)max_texts);
  printf("%td bytes (out of %ld)\n",
          (ptrdiff_t)(byte_ptr-byte_mem),(long)max_bytes);
  printf("%td tokens (out of %ld)\n",
@y
  puts(_("\nMemory usage statistics:"));
  printf(_("%td names (out of %ld)\n"),@^system dependencies@>
          (ptrdiff_t)(name_ptr-name_dir),(long)max_names);
  printf(_("%td replacement texts (out of %ld)\n"),
          (ptrdiff_t)(text_ptr-text_info),(long)max_texts);
  printf(_("%td bytes (out of %ld)\n"),
          (ptrdiff_t)(byte_ptr-byte_mem),(long)max_bytes);
  printf(_("%td tokens (out of %ld)\n"),
@z

@x [11.104] l.1551
@** Index.
@y
@** Extensions to {\tentex CWEB}.  The following sections introduce new or
improved features that have been created by numerous contributors over the
course of a quarter century.

Care has been taken to keep the original section numbering intact, so this new
material should nicely integrate with the original ``\&{104.~Index}.''

@* Output file update. Most \CEE/ projects are controlled by a \.{Makefile}
that automatically takes care of the temporal dependencies between the different
source modules. It may be convenient that \.{CWEB} doesn't create new output
for all existing files, when there are only changes to some of them. Thus the
\.{make} process will only recompile those modules where necessary. You can
activate this feature with the `\.{+c}' command-line option. The idea and basic
implementation of this mechanism can be found in the program \.{NUWEB} by
Preston Briggs, to whom credit is due.

@<Open the intermediate output file@>= {
  if ((C_file=fopen(output_file_name,"a"))==NULL)
    fatal(_("! Cannot open output file "),output_file_name);
@.Cannot open output file@>
  else fclose(C_file); /* Test accessability */
  if((C_file=fopen(check_file_name,"wb"))==NULL)
    fatal(_("! Cannot open output file "),check_file_name);
}

@ @<Update the primary result...@>=
if((C_file=fopen(C_file_name,"r"))!=NULL) {
  @<Set up the comparison of temporary output@>@;
  @<Create the primary output depending on the comparison@>@;
} else
  rename(check_file_name,C_file_name); /* This was the first run */

@ @<Set up the comparison of temporary output@>=
  bool comparison=false;

  if((check_file=fopen(check_file_name,"r"))==NULL)
    fatal(_("! Cannot open output file "),check_file_name);
@.Cannot open output file@>

  @<Compare the temporary output...@>@;

  fclose(C_file); C_file=NULL;
  fclose(check_file); check_file=NULL;

@ We hope that this runs fast on most systems.

@<Compare the temporary output to the previous output@>=
do {
  char x[BUFSIZ],y[BUFSIZ];
  int x_size = fread(x,sizeof(char),BUFSIZ,C_file);
  int y_size = fread(y,sizeof(char),BUFSIZ,check_file);
  comparison = (x_size == y_size) && !memcmp(x,y,x_size);
} while(comparison && !feof(C_file) && !feof(check_file));

@ Note the superfluous call to |remove| before |rename|.  We're using it to
get around a bug in some implementations of |rename|.@^system dependencies@>

@<Create the primary output...@>=
if(comparison)
  remove(check_file_name); /* The output remains untouched */
else {
  remove(C_file_name);
  rename(check_file_name,C_file_name);
}

@ The author of a \.{CWEB} program may want to write the \\{secondary} output
instead of to a file (in \.{@@(...@@>}) to \.{/dev/null} or \.{/dev/stdout} or
\.{/dev/stderr}.  We must take care of the \\{temporary} output already written
to a file and finally get rid of that file.

@<Update the secondary results...@>=
if(0==strcmp("/dev/stdout",output_file_name))@/
  @<Redirect temporary output to \.{/dev/stdout}@>@;
else if(0==strcmp("/dev/stderr",output_file_name))@/
  @<Redirect temporary output to \.{/dev/stderr}@>@;
else if(0==strcmp("/dev/null",output_file_name))@/
  @<Redirect temporary output to \.{/dev/null}@>@;
else { /* Hopefully a regular output file */
  if((C_file=fopen(output_file_name,"r"))!=NULL) {
    @<Set up the comparison of temporary output@>@;
    @<Create the secondary output depending on the comparison@>@;
  } else
    rename(check_file_name,output_file_name); /* This was the first run */
}

@ Again, we use a call to |remove| before |rename|.

@<Create the secondary output...@>=
if(comparison)
  remove(check_file_name); /* The output remains untouched */
else {
  remove(output_file_name);
  rename(check_file_name,output_file_name);
}

@ Copy secondary output to |stdout|.

@<Redirect temporary output to \.{/dev/stdout}@>={
  @<Setup system redirection@>@;
  do {
    in_size = fread(in_buf,sizeof(char),BUFSIZ,check_file);
    in_buf[in_size]='\0';
    fprintf(stdout,"%s",in_buf);
  } while(!feof(check_file));@/
  fclose(check_file); check_file=NULL;
  @<Create the secondary output...@>@;
}

@ Copy secondary output to |stderr|.

@<Redirect temporary output to \.{/dev/stderr}@>={
  @<Setup system redirection@>@;
  do {
    in_size = fread(in_buf,sizeof(char),BUFSIZ,check_file);
    in_buf[in_size]='\0';
    fprintf(stderr,"%s",in_buf);
  } while(!feof(check_file));@/
  fclose(check_file); check_file=NULL;
  @<Create the secondary output...@>@;
}

@ No copying necessary, just remove the temporary output file.

@<Redirect temporary output to \.{/dev/null}@>={
  bool comparison=true;
  @<Create the secondary output...@>@;
}

@ @<Setup system redirection@>=
char in_buf[BUFSIZ+1];
int in_size;
bool comparison=true;
if((check_file=fopen(check_file_name,"r"))==NULL)
  fatal(_("! Cannot open output file "),check_file_name);
@.Cannot open output file@>

@* Print ``version'' information.
Don't do this at home, kids! Push our local macro to the variable in \.{COMMON}
for printing the |banner| and the |versionstring| from there.

@d max_banner 50

@<Common code...@>=
extern char cb_banner[];

@ @<Set init...@>=
  strncpy(cb_banner,banner,max_banner-1);

@** Index.
@z

