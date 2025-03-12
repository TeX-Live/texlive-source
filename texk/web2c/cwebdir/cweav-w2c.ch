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

@x [0.0] l.34
\def\title{CWEAVE (Version 4.12.1)}
@y
\def\title{CWEAVE (Version 4.12.1 [\TeX~Live])}
@z

@x [0.0] l.38
  \centerline{(Version 4.12.1)}
@y
  \centerline{(Version 4.12.1 [\TeX~Live])}
@z

@x [0.0] l.40
\def\botofcontents{\vfill
@y
\def\covernote{\vbox{%
@z

@x [0.0] l.53
}
@y
}}\datecontentspage
@z

@x [1.1] l.69
@d banner "This is CWEAVE (Version 4.12.1)"
@y
@d banner "This is CWEAVE, Version 4.12.1"
  /* will be extended by the \TeX~Live |versionstring| */
@z

@x [1.2] l.97
  if (show_banner) puts(banner); /* print a ``banner line'' */
@y
  if (show_banner) cb_show_banner(); /* print a ``banner line'' */
@z

@x [1.3] l.111
@i common.h
@y
@i comm-w2c.h
@z

@x [2.23] l.216
@ @d max_refs 30000 /* number of cross-references; must be less than 65536 */
@y
@ @d max_refs 65535 /* number of cross-references; must be less than 65536 */
@z

@x [2.25] l.245
@d append_xref(c) if (xref_ptr==xmem_end) overflow("cross-reference");
@y
@d append_xref(c) if (xref_ptr==xmem_end) overflow(_("cross-reference"));
@z

@x [2.30] l.336
@d max_toks 30000 /* number of symbols in \CEE/ texts being parsed;
  must be less than 65536 */
@d max_texts 8000 /* number of phrases in \CEE/ texts being parsed;
  must be less than 10240 */
@y
@d max_toks 65535 /* number of symbols in \CEE/ texts being parsed;
  must be less than 65536 */
@d max_texts 10239 /* number of phrases in \CEE/ texts being parsed;
  must be less than 10240 */
@z

@x [4.57] l.887
        err_print("! String didn't end"); loc=limit; break;
@y
        err_print(_("! String didn't end")); loc=limit; break;
@z

@x [4.57] l.891
        err_print("! Input ended in middle of string"); loc=buffer; break;
@y
        err_print(_("! Input ended in middle of string")); loc=buffer; break;
@z

@x [4.57] l.910
    printf("%s","\n! String too long: ");
@y
    printf("%s",_("\n! String too long: "));
@z

@x [4.59] l.929
  case translit_code: err_print("! Use @@l in limbo only"); continue;
@y
  case translit_code: err_print(_("! Use @@l in limbo only")); continue;
@z

@x [4.62] l.967
    err_print("! Input ended in section name");
@y
    err_print(_("! Input ended in section name"));
@z

@x [4.62] l.980
  printf("%s","\n! Section name too long: ");
@y
  printf("%s",_("\n! Section name too long: "));
@z

@x [4.63] l.994
    err_print("! Section name didn't end"); break;
@y
    err_print(_("! Section name didn't end")); break;
@z

@x [4.63] l.998
    err_print("! Control codes are forbidden in section name"); break;
@y
    err_print(_("! Control codes are forbidden in section name")); break;
@z

@x [4.64] l.1015
    err_print("! Control text didn't end"); loc=limit;
@y
    err_print(_("! Control text didn't end")); loc=limit;
@z

@x [4.64] l.1020
      err_print("! Control codes are forbidden in control text");
@y
      err_print(_("! Control codes are forbidden in control text"));
@z

@x [4.66] l.1035
if (loc>=limit) err_print("! Verbatim string didn't end");
@y
if (loc>=limit) err_print(_("! Verbatim string didn't end"));
@z

@x [5.70] l.1072
  if (++section_count==max_sections) overflow("section number");
@y
  if (++section_count==max_sections) overflow(_("section number"));
@z

@x [5.74] l.1161
    case translit_code: err_print("! Use @@l in limbo only"); continue;
@y
    case translit_code: err_print(_("! Use @@l in limbo only")); continue;
@z

@x [5.79] l.1248
  err_print("! Missing left identifier of @@s");
@y
  err_print(_("! Missing left identifier of @@s"));
@z

@x [5.79] l.1253
    err_print("! Missing right identifier of @@s");
@y
    err_print(_("! Missing right identifier of @@s"));
@z

@x [5.82] l.1301
      printf("%s","\n! Never defined: <");
      print_section_name(p); putchar('>'); mark_harmless();
@y
      printf("%s",_("\n! Never defined: <"));
      print_section_name(p); putchar('>'); mark_harmless();
@z

@x [5.82] l.1307
      printf("%s","\n! Never used: <");
      print_section_name(p); putchar('>'); mark_harmless();
@y
      printf("%s",_("\n! Never used: <"));
      print_section_name(p); putchar('>'); mark_harmless();
@z

@x [6.89] l.1396
@ In particular, the |finish_line| procedure is called near the very
beginning of phase two. We initialize the output variables in a slightly
tricky way so that the first line of the output file will be
`\.{\\input cwebmac}'.

@<Start \9{t}\TEX/...@>=
out_ptr=out_buf+1; out_line=1; active_file=tex_file;
tex_printf("\\input cwebma"); *out_ptr='c';
@y
@ In particular, the |finish_line| procedure is called near the very
beginning of phase two. We initialize the output variables in a slightly
tricky way so that the first line of the output file will be dependent of
the user language set by the `\.{+l}' option and its argument.  If you call
\.{CWEAVE} with `\.{+lX}' (or `\.{-lX}' as well), where `\.X' is the
(possibly empty) string of characters to the right of~`\.l', `\.X'~will be
prepended to `\.{cwebmac.tex}', e.g., if you call \.{CWEAVE} with
`\.{+ldeutsch}', you will receive the line `\.{\\input deutschcwebmac}'.
Without this option the first line of the output file will be
`\.{\\input cwebmac}'.

@<Start \9{t}\TEX/...@>=
out_ptr=out_buf+1; out_line=1; active_file=tex_file;
tex_puts("\\input ");
tex_printf(use_language);
tex_puts("cwebma");
*out_ptr='c';
@z

@x [6.94] l.1461
  printf("\n! Line had to be broken (output l. %d):\n",out_line);
@y
  printf(_("\n! Line had to be broken (output l. %d):\n"),out_line);
@z

@x [7.99] l.1542
        default: err_print("! Double @@ should be used in limbo");
@y
        default: err_print(_("! Double @@ should be used in limbo"));
@z

@x [7.101] l.1584
@d app_tok(c) {if (tok_ptr+2>tok_mem_end) overflow("token"); *(tok_ptr++)=c;}
@y
@d app_tok(c) {if (tok_ptr+2>tok_mem_end) overflow(_("token")); *(tok_ptr++)=c;}
@z

@x [7.101] l.1596
          err_print("! Input ended in mid-comment");
@y
          err_print(_("! Input ended in mid-comment"));
@z

@x [7.101] l.1601
        if (bal>1) err_print("! Missing } in comment");
@y
        if (bal>1) err_print(_("! Missing } in comment"));
@z

@x [7.101] l.1617
      else {err_print("! Extra } in comment");
@y
      else {err_print(_("! Extra } in comment"));
@z

@x [7.102] l.1629
  if (bal>1) err_print("! Missing } in comment");
@y
  if (bal>1) err_print(_("! Missing } in comment"));
@z

@x [7.103] l.1637
    err_print("! Illegal use of @@ in comment");
@y
    err_print(_("! Illegal use of @@ in comment"));
@z

@x [8.110] l.2128
@i prod.w
@y
@i prod-cweave.w
@z

@x [9.128] l.2650
@<Cases for |exp|@>=
if (cat1==lbrace || cat1==int_like || cat1==decl) {
  make_underlined(pp); big_app(dindent); big_app1(pp);
  reduce(pp,1,fn_decl,0,1);
}
@y
@<Cases for |exp|@>=
if(cat1==lbrace || cat1==int_like || cat1==decl) {
  make_underlined(pp); if (indent_param_decl) big_app(dindent); big_app1(pp);
  reduce(pp,1,fn_decl,0,1);
}
@z

@x [9.138] l.2754
@ @<Cases for |decl_head|@>=
if (cat1==comma) {
  big_app2(pp); app(opt); app('9'); reduce(pp,2,decl_head,-1,33);
}
else if (cat1==ubinop) {
  big_app1_insert(pp,'{'); big_app('}'); reduce(pp,2,decl_head,-1,34);
}
else if (cat1==exp && cat2!=lpar && cat2!=lbrack && cat2!=exp && cat2!=cast) {
  make_underlined(pp+1); squash(pp,2,decl_head,-1,35);
}
else if ((cat1==binop||cat1==colon) && cat2==exp && (cat3==comma ||
    cat3==semi || cat3==rpar))
  squash(pp,3,decl_head,-1,36);
else if (cat1==cast) squash(pp,2,decl_head,-1,37);
else if (cat1==int_like || cat1==lbrace || cat1==decl) {
  big_app(dindent); squash(pp,1,fn_decl,0,38);
}
else if (cat1==semi) squash(pp,2,decl,-1,39);
@y
@ @<Cases for |decl_head|@>=
if (cat1==comma) {
  big_app2(pp); app(opt); app('9'); reduce(pp,2,decl_head,-1,33);
}
else if (cat1==ubinop) {
  big_app1_insert(pp,'{'); big_app('}');
  reduce(pp,2,decl_head,-1,34);
}
else if (cat1==exp && cat2!=lpar && cat2!=lbrack && cat2!=exp && cat2!=cast) {
  make_underlined(pp+1); squash(pp,2,decl_head,-1,35);
}
else if ((cat1==binop||cat1==colon) && cat2==exp && (cat3==comma ||
    cat3==semi || cat3==rpar))
  squash(pp,3,decl_head,-1,36);
else if (cat1==cast) squash(pp,2,decl_head,-1,37);
else if (cat1==int_like || cat1==lbrace || cat1==decl) {
  if (indent_param_decl) big_app(dindent);
  squash(pp,1,fn_decl,0,38);
}
else if (cat1==semi) squash(pp,2,decl,-1,39);
@z

@x [9.139] l.2776
@ @<Cases for |decl|@>=
if (cat1==decl) {
  big_app1_insert(pp,force); reduce(pp,2,decl,-1,40);
}
else if (cat1==stmt || cat1==function) {
  big_app1_insert(pp,big_force); reduce(pp,2,cat1,-1,41);
}
@y
@ @<Cases for |decl|@>=
if (cat1==decl) {
  big_app1_insert(pp,force); reduce(pp,2,decl,-1,40);
}
else if (cat1==stmt || cat1==function) {
  big_app1_insert(pp,order_decl_stmt ? big_force : force);
  reduce(pp,2,cat1,-1,41);
}
@z

@x [9.143] l.2833
@ @<Cases for |fn_decl|@>=
if (cat1==decl) {
  big_app1_insert(pp,force); reduce(pp,2,fn_decl,0,51);
}
else if (cat1==stmt) {
  big_app1(pp); app(outdent); app(outdent); big_app(force);
  big_app1(pp+1); reduce(pp,2,function,-1,52);
}
@y
@ @<Cases for |fn_decl|@>=
if (cat1==decl) {
  big_app1_insert(pp,force); reduce(pp,2,fn_decl,0,51);
}
else if (cat1==stmt) {
  big_app1(pp);
  if (indent_param_decl) {
    app(outdent); app(outdent);
  }
  big_app(force); big_app1(pp+1); reduce(pp,2,function,-1,52);
}
@z

@x [9.153] l.2937
  big_app1_insert(pp,dindent); reduce(pp,2,fn_decl,0,73);
@y
  big_app1(pp); if (indent_param_decl) big_app(dindent);
  big_app1(pp+1); reduce(pp,2,fn_decl,0,73);
@z

@x [9.156] l.2963
  big_app1_insert(pp, (cat1==decl || cat1==function) ? big_force :
     force_lines ? force : break_space); reduce(pp,2,cat1,-1,76);
@y
  big_app1_insert(pp, (cat1==decl || cat1==function) ? @|
     ( order_decl_stmt ? big_force : force ) : @|
     ( force_lines ? force : break_space ) ); reduce(pp,2,cat1,-1,76);
@z

@x [9.186] l.3292
    overflow("token");
@y
    overflow(_("token"));
@z

@x [9.186] l.3296
    overflow("text");
@y
    overflow(_("text"));
@z

@x [9.190] l.3351
  if (tok_ptr+6>tok_mem_end) overflow("token");
@y
  if (tok_ptr+6>tok_mem_end) overflow(_("token"));
@z

@x [9.191] l.3357
  printf("\nIrreducible scrap sequence in section %d:",(int)section_count);
@y
  printf(_("\nIrreducible scrap sequence in section %d:"),(int)section_count);
@z

@x [9.192] l.3367
  printf("\nTracing after l. %d:\n",cur_line); mark_harmless();
@y
  printf(_("\nTracing after l. %d:\n"),cur_line); mark_harmless();
@z

@x [10.197] l.3499
  overflow("scrap/token/text");
@y
  overflow(_("scrap/token/text"));
@z

@x [10.199] l.3584
      else err_print("! Double @@ should be used in strings");
@y
      else err_print(_("! Double @@ should be used in strings"));
@z

@x [10.203] l.3667
  if (next_control!='|') err_print("! Missing '|' after C text");
@y
  if (next_control!='|') err_print(_("! Missing '|' after C text"));
@z

@x [11.211] l.3808
  if (stack_ptr==stack_end) overflow("stack");
@y
  if (stack_ptr==stack_end) overflow(_("stack"));
@z

@x [11.224] l.4131
  printf("%s","\n! Illegal control code in section name: <");
@y
  printf("%s",_("\n! Illegal control code in section name: <"));
@z

@x [11.225] l.4146
    printf("%s","\n! C text in section name didn't end: <");
@y
    printf("%s",_("\n! C text in section name didn't end: <"));
@z

@x [11.225] l.4158
      if (j>buffer+long_buf_size-3) overflow("buffer");
@y
      if (j>buffer+long_buf_size-3) overflow(_("buffer"));
@z

@x [11.226] l.4166
  if (j>buffer+long_buf_size-4) overflow("buffer");
@y
  if (j>buffer+long_buf_size-4) overflow(_("buffer"));
@z

@x [12.227] l.4181
if (show_progress) printf("%s","\nWriting the output file...");
@y
if (show_progress) printf("%s",_("\nWriting the output file..."));
@z

@x [12.232] l.4268
        err_print("! TeX string should be in C text only");
@y
        err_print(_("! TeX string should be in C text only"));
@z

@x [12.232] l.4271
        err_print("! Verbatim string should be in C text only"); break;
@y
        err_print(_("! Verbatim string should be in C text only")); break;
@z

@x [12.232] l.4277
        err_print("! You can't do that in TeX text"); break;
@y
        err_print(_("! You can't do that in TeX text")); break;
@z

@x [12.236] l.4349
    err_print("! Improper macro definition");
@y
    err_print(_("! Improper macro definition"));
@z

@x [12.236] l.4362
        } @=/* otherwise fall through */@>@;
      default: err_print("! Improper macro definition"); break;
@y
        } @=/* otherwise fall through */@>@;
      default: err_print(_("! Improper macro definition")); break;
@z

@x [12.237] l.4389
  if (scrap_ptr!=scrap_info+2) err_print("! Improper format definition");
@y
  if (scrap_ptr!=scrap_info+2) err_print(_("! Improper format definition"));
@z

@x [12.240] l.4424
  err_print("! You need an = sign after the section name");
@y
  err_print(_("! You need an = sign after the section name"));
@z

@x [12.241] l.4446
  err_print("! You can't do that in C text");
@y
  err_print(_("! You can't do that in C text"));
@z

@x [13.247] l.4537
  if (show_progress) printf("%s","\nWriting the index...");
@y
  if (show_progress) printf("%s",_("\nWriting the index..."));
@z

@x [13.247] l.4548
    fatal("! Cannot open index file ",idx_file_name);
@y
    fatal(_("! Cannot open index file "),idx_file_name);
@z

@x [13.247] l.4560
    fatal("! Cannot open section file ",scn_file_name);
@y
    fatal(_("! Cannot open section file "),scn_file_name);
@z

@x [13.247] l.4572
fclose(active_file);
@y
fclose(active_file); active_file=tex_file=NULL;
if (check_for_change) @<Update the result when it has changed@>@;
@z

@x [13.247] l.4575
  printf("%s","Done.");
@y
  printf("%s",_("Done."));
@z

@x [13.257] l.4727
    if (sort_ptr>=scrap_info_end) overflow("sorting");
@y
    if (sort_ptr>=scrap_info_end) overflow(_("sorting"));
@z

@x [13.269] l.4864
  puts("\nMemory usage statistics:");
@.Memory usage statistics:@>
  printf("%td names (out of %ld)\n",@^system dependencies@>
            (ptrdiff_t)(name_ptr-name_dir),(long)max_names);
  printf("%td cross-references (out of %ld)\n",
            (ptrdiff_t)(xref_ptr-xmem),(long)max_refs);
  printf("%td bytes (out of %ld)\n",
            (ptrdiff_t)(byte_ptr-byte_mem),(long)max_bytes);
  puts("Parsing:");
  printf("%td scraps (out of %ld)\n",
            (ptrdiff_t)(max_scr_ptr-scrap_info),(long)max_scraps);
  printf("%td texts (out of %ld)\n",
            (ptrdiff_t)(max_text_ptr-tok_start),(long)max_texts);
  printf("%td tokens (out of %ld)\n",
            (ptrdiff_t)(max_tok_ptr-tok_mem),(long)max_toks);
  printf("%td levels (out of %ld)\n",
            (ptrdiff_t)(max_stack_ptr-stack),(long)stack_size);
  puts("Sorting:");
  printf("%td levels (out of %ld)\n",
            (ptrdiff_t)(max_sort_ptr-scrap_info),(long)max_scraps);
@y
  puts(_("\nMemory usage statistics:"));
@.Memory usage statistics:@>
  printf(_("%td names (out of %ld)\n"),@^system dependencies@>
            (ptrdiff_t)(name_ptr-name_dir),(long)max_names);
  printf(_("%td cross-references (out of %ld)\n"),
            (ptrdiff_t)(xref_ptr-xmem),(long)max_refs);
  printf(_("%td bytes (out of %ld)\n"),
            (ptrdiff_t)(byte_ptr-byte_mem),(long)max_bytes);
  puts(_("Parsing:"));
  printf(_("%td scraps (out of %ld)\n"),
            (ptrdiff_t)(max_scr_ptr-scrap_info),(long)max_scraps);
  printf(_("%td texts (out of %ld)\n"),
            (ptrdiff_t)(max_text_ptr-tok_start),(long)max_texts);
  printf(_("%td tokens (out of %ld)\n"),
            (ptrdiff_t)(max_tok_ptr-tok_mem),(long)max_toks);
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_stack_ptr-stack),(long)stack_size);
  puts(_("Sorting:"));
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_sort_ptr-scrap_info),(long)max_scraps);
@z

@x [14.270] l.4886
@** Index.
@y
@** Extensions to {\tentex CWEB}.  The following sections introduce new or
improved features that have been created by numerous contributors over the
course of a quarter century.

Care has been taken to keep the original section numbering intact, so this new
material should nicely integrate with the original ``\&{270.~Index}.''

@* Formatting alternatives.
\.{CWEAVE} indents declarations after old-style function definitions and
long parameter lists of modern function definitions.
With the \.{-i} option they will come out flush left.

@d indent_param_decl flags['i'] /* should formal parameter declarations be indented? */

@<Set init...@>=
indent_param_decl=true;

@ The original manual described the \.{-o} option for \.{CWEAVE}, but this was
not yet present.  Here is a simple implementation.  The purpose is to suppress
the extra space between local variable declarations and the first statement in
a function block.

@d order_decl_stmt flags['o'] /* should declarations and statements be separated? */

@<Set init...@>=
order_decl_stmt=true;

@* Output file update. Most \CEE/ projects are controlled by a \.{Makefile}
that automatically takes care of the temporal dependencies between the different
source modules. It may be convenient that \.{CWEB} doesn't create new output
for all existing files, when there are only changes to some of them. Thus the
\.{make} process will only recompile those modules where necessary. You can
activate this feature with the `\.{+c}' command-line option. The idea and basic
implementation of this mechanism can be found in the program \.{NUWEB} by
Preston Briggs, to whom credit is due.

@<Update the result...@>= {
if((tex_file=fopen(tex_file_name,"r"))!=NULL) {
  boolean comparison=false;

  if((check_file=fopen(check_file_name,"r"))==NULL)
    fatal(_("! Cannot open output file "),check_file_name);
@.Cannot open output file@>

  @<Compare the temporary output...@>@;

  fclose(tex_file); tex_file=NULL;
  fclose(check_file); check_file=NULL;

  @<Take appropriate action depending on the comparison@>@;
} else
  rename(check_file_name,tex_file_name); /* This was the first run */

strcpy(check_file_name,""); /* We want to get rid of the temporary file */
}

@ We hope that this runs fast on most systems.

@<Compare the temporary output to the previous output@>=
do {
  char x[BUFSIZ],y[BUFSIZ];
  int x_size = fread(x,sizeof(char),BUFSIZ,tex_file);
  int y_size = fread(y,sizeof(char),BUFSIZ,check_file);
  comparison = (x_size == y_size) && !memcmp(x,y,x_size);
} while(comparison && !feof(tex_file) && !feof(check_file));

@ Note the superfluous call to |remove| before |rename|.  We're using it to
get around a bug in some implementations of |rename|.@^system dependencies@>

@<Take appropriate action...@>=
if(comparison)
  remove(check_file_name); /* The output remains untouched */
else {
  remove(tex_file_name);
  rename(check_file_name,tex_file_name);
}

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

