Formatting changes for CTWILL by Andreas Scherer
This file is in the Public Domain.

This extensive set of changes is my first attempt to format CTWILL with
itself in HINT format, i.e., with the 'pdfctproofmac.tex' macros. This
produces output with 'mini-indexes' for every section.

Apply these additional changes in the following two-step procedure:
First create 'ctwill-w2c.ch' that mogrifies CWEAVE into CTWILL:
$ tie -c ctwill-w2c.ch \
> cweave.w cweav-{patch,extensions,output,i18n,twill}.ch \
> cwtw-texlive.ch ctwill-texlive.ch
Then create 'ctwill.w' that gets processed with HiTeX:
$ ctie -m ctwill.w \
> cweave.w ctwill-w2c.ch ctwill-hint.ch

Section 1.

@x
The ``banner line'' defined here should be changed whenever \.{CTWILL} is
@y
The ``banner line'' defined here should be changed whenever \.{CTWILL} is
@-banner@>
@$banner {CTWILL}1 =\.{"This\ is\ CTWILL"}@>

@$ctangle {CTWILL}3 \&{enum} \&{cweb}@>
@$ctwill {CTWILL}3 \&{enum} \&{cweb}@>
@$cweave {CTWILL}3 \&{enum} \&{cweb}@>

@$inner {CTWILL}207 \&{enum} \&{mode}@>
@$outer {CTWILL}207 \&{enum} \&{mode}@>
@z

Section 2.

@x
@ \.{CWEAVE} has a fairly straightforward outline.  It operates in
@y
@ \.{CWEAVE} has a fairly straightforward outline.  It operates in
@%
@$show_banner {CTWILL}15 =\\{flags}[\.{'b'}]@>
@$show_progress {CTWILL}15 =\\{flags}[\.{'p'}]@>
@z

Section 5.

@x
@d _(s) gettext(s)
@y
@d _(s) gettext(s)
@-s@>
@-a@>
@-HAVE_GETTEXT@>
@z

Section 6.

@x
@d compress(c) if (loc++<=limit) return c
@y
@d compress(c) if (loc++<=limit) return c
@-c@>
@z

Section 7.

@x
@ Code related to input routines:
@y
@ Code related to input routines:
@-c@>
@z

Section 8.

@x
@d cur_line line[include_depth] /* number of current line in current file */
@y
@d cur_line line[include_depth] /* number of current line in current file */
@-cur_file@>
@-cur_file_name@>
@-cur_line@>
@$cur_file {CTWILL}8 =\\{file}[\\{include\_depth}]@>
@$cur_file_name {CTWILL}8 =\hfil\break\\{file\_name}[\\{include\_depth}]@>
@$cur_line {CTWILL}8 =\\{line}[\\{include\_depth}]@>
@z

Section 11.

@x
@ Code related to identifier and section name storage:
@y
@ Code related to identifier and section name storage:
@-c@>
@-llink@>
@-rlink@>
@-root@>
@-ilk@>
@$llink {CTWILL}11 =\\{link}@>
@$rlink {CTWILL}11 =\\{dummy}.\\{Rlink}@>
@$root {CTWILL}11 =\\{name\_dir}$\MG$\\{rlink}@>
@$ilk {CTWILL}11 =\\{dummy}.\\{Ilk}@>
@z

Section 13.

@x
@ Code related to error handling:
@y
@ Code related to error handling:
@-s@>
@z

Section 15.

@x
extern const char *use_language; /* prefix to \.{cwebmac.tex} in \TEX/ output */
@y
extern const char *use_language; /* prefix to \.{ctwimac.tex} in \TEX/ output */
@-show_banner@>
@-show_progress@>
@-show_happiness@>
@-show_stats@>
@-make_xrefs@>
@-check_for_change@>
@$show_banner {CTWILL}15 =\\{flags}[\.{'b'}]@>
@$show_progress {CTWILL}15 =\\{flags}[\.{'p'}]@>
@$show_happiness {CTWILL}15 =\\{flags}[\.{'h'}]@>
@$show_stats {CTWILL}15 =\\{flags}[\.{'s'}]@>
@$make_xrefs {CTWILL}15 =\\{flags}[\.{'x'}]@>
@$check_for_change {CTWILL}15 =\\{flags}[\.{'c'}]@>
@z

Section 16.

@x
@ Code related to output:
@y
@ Code related to output:
@-a@>
@-b@>
@-c@>
@-update_terminal@>
@-new_line@>
@$update_terminal {CTWILL}16 =\\{fflush}(\\{stdout})@>
@$new_line {CTWILL}16 =\\{putchar}(\.{'\\n'})@>
@z

Section 17.

@x
@d long_buf_size (buf_size+longest_name) /* for \.{CWEAVE} */
@y
@d long_buf_size (buf_size+longest_name) /* for \.{CWEAVE} */
@-long_buf_size@>
@$long_buf_size {CTWILL}17 =$\\{buf\_size}+\\{longest\_name}$@>
@z

Section 20.

@x
@* Data structures exclusive to {\tt CWEAVE}.
@y
@* Data structures exclusive to {\tt CWEAVE}.
@-a@>
@z

Section 22.

@x
@ The other large memory area in \.{CWEAVE} keeps the cross-reference data.
@y
@ The other large memory area in \.{CWEAVE} keeps the cross-reference data.
@-p@>
@-x@>
@z

Section 24.

@x
@d file_flag (3*cite_flag)
@y
@-file_flag@>
@-def_flag@>
@-xref@>
@$file_flag {CTWILL}24 =$\T{3}*\\{cite\_flag}{}$@>
@$def_flag {CTWILL}24 =$\T{2}*\\{cite\_flag}{}$@>
@$xref {CTWILL}24 =\\{equiv\_or\_xref}@>
@d file_flag (3*cite_flag)
@z

Section 25.

@x
@ A new cross-reference for an identifier is formed by calling |new_xref|,
@y
@ A new cross-reference for an identifier is formed by calling |new_xref|,
@-a@>
@-c@>
@-p@>
@-no_xref@>
@$no_xref {CTWILL}25 =$\R\\{make\_xrefs}$@>
@z

Section 36.

@x
@d underline '\n' /* this code will be intercepted without confusion */
@y
@d underline '\n' /* this code will be intercepted without confusion */
@-begin_comment@>
@-underline@>
@$begin_comment {CTWILL}36 =\.{'\\t'}@>
@$underline {CTWILL}36 =\.{'\\n'}@>
@z

Section 42.

CTWILL hickups on comment and produces unmatched '$' in mini-index.

@x l.646
skip_TeX(void) /* skip past pure \TEX/ code */
@y
skip_TeX(void)
@z

Section 44.

@x
@ As one might expect, |get_next| consists mostly of a big switch
@y
@ As one might expect, |get_next| consists mostly of a big switch
@-c@>
@$c {CTWILL}44 \&{eight\_bits}@>
@z

Section 45.

@x
@ @<Predecl...@>=@+static eight_bits get_next(void);
@y
@ @<Predecl...@>=@+static eight_bits get_next(void);
@-get_next@>
@z

Section 46.

@x
@d left_preproc ord /* begins a preprocessor command */
@y
@-left_preproc@>
@$left_preproc {CTWILL}46 =\\{ord}@>
@d left_preproc ord /* begins a preprocessor command */
@z

Section 52.

@x
  id_first=--loc;
  do
    ++loc;
  while (isalpha((int)*loc) || isdigit((int)*loc) @|
      || isxalpha(*loc) || ishigh(*loc));
  id_loc=loc; return identifier;
@y
  id_first=--loc;@/
  do
    ++loc;
  while (isalpha((int)*loc) || isdigit((int)*loc) @|
      || isxalpha(*loc) || ishigh(*loc));@/
  id_loc=loc;@/
  return identifier;
@z

Section 53.

@x
@d gather_digits_while(t) while ((t) || *loc=='\'')
@y
@d gather_digits_while(t) while ((t) || *loc=='\'')
@-t@>
@z

Section 65.

@x
@ @<Predecl...@>=@+static void skip_restricted(void);
@y
@ @<Predecl...@>=@+static void skip_restricted(void);
@-skip_restricted@>
@z

Section 69.

@x
@ @<Predecl...@>=@+static void phase_one(void);
@y
@ @<Predecl...@>=@+static void phase_one(void);
@-phase_one@>
@z

Section 72.

@x
C_xref( /* makes cross-references for \CEE/ identifiers */
  eight_bits spec_ctrl)
@y
C_xref( /* makes cross-references for \CEE/ identifiers */
  eight_bits spec_ctrl)
@-C_xref@>
@$C_xref {CTWILL}72 \&{static} \&{void} (\,)@>
@z

Section 86.

@x
@ The |flush_buffer| routine empties the buffer up to a given breakpoint,
@y
@ The |flush_buffer| routine empties the buffer up to a given breakpoint,
@-b@>
@-c@>
@-tex_new_line@>
@$tex_new_line {CTWILL}86 =$\\{putc}(\.{'\\n'},\39\\{active\_file})$@>
@z

Section 89.

@x
@d proofing flags['P']
@y
@d proofing flags['P']
@-proofing@>
@$proofing {CTWILL}89 =\\{flags}[\.{'P'}]@>
@z

Section 90.

@x
@ When we wish to append one character |c| to the output buffer, we write
@y
@ When we wish to append one character |c| to the output buffer, we write
@-c@>
@-s@>
@z

Section 91.

@x
out_str( /* output characters from |s| to end of string */
const char*s)
@y
out_str( /* output characters from |s| to end of string */
const char*s)
@-out_str@>
@$out_str {CTWILL}91 \&{static} \&{void} (\,)@>
@z

Section 101.

@x
@ The |copy_comment| function issues a warning if more braces are opened than
@y
@ The |copy_comment| function issues a warning if more braces are opened than
@-c@>
@-t@>
@-copy_comment@>
@$copy_comment {CTWILL}101 \&{static} \&{int} (\,)@>
@z

Section 108.

@x
@d print_cat(c) fputs(cat_name[c],stdout) /* symbolic printout of a category */
@y
@d print_cat(c) fputs(cat_name[c],stdout) /* symbolic printout of a category */
@-c@>
@z

Section 109.

@x
@ The token lists for translated \TEX/ output contain some special control
@y
@ The token lists for translated \TEX/ output contain some special control
@-n@>
@z

Section 110.

@x
@ The raw input is converted into scraps according to the following table,
@y
@* From raw input to scraps.
@-c@>
The raw input is converted into scraps according to the following table,
@z

Sections 111.

@x l.7 line numbers refer to 'prod.w'
@ Here is a table of all the productions.  Each production that
@y
@* Table of all productions.  Each production that
@-time@>
@-in@>@-f@>@-x@>@-y@>
@-any@>@-z@>@-g@>@-a@>
@-any_other@>@-p@>
@-deprecated@>
@-fallthrough@>
@-likely@>
@-nodiscard@>
@-unlikely@>
@-s@>
@z

Section 112.

@x
@d trans trans_plus.Trans /* translation texts of scraps */
@y
@d trans trans_plus.Trans /* translation texts of scraps */
@-trans@>
@$trans {CTWILL}112 =\\{trans\_plus}.\\{Trans}@>
@z

Section 115.

@x
@ Token lists in |@!tok_mem| are composed of the following kinds of
@y
@ Token lists in |@!tok_mem| are composed of the following kinds of
@-DEBUG@>
@-p@>
@-res_flag@>
@-section_flag@>
@-tok_flag@>
@-inner_tok_flag@>
@$res_flag {CTWILL}115 =$\T{2}*\\{id\_flag}$@>
@$section_flag {CTWILL}115 =$\T{3}*\\{id\_flag}$@>
@$tok_flag {CTWILL}115 =$\T{3}*\\{id\_flag}$@>
@$inner_tok_flag {CTWILL}115 =$\T{4}*\\{id\_flag}$@>
@z

Section 116.

@x l.2221
@c
@y
@c
@-DEBUG@>
@z

Section 118.

@x
@ The production rules listed above are embedded directly into \.{CWEAVE},
@y
@ The production rules listed above are embedded directly into \.{CWEAVE},
@-a@>
@-c@>
@-d@>
@-j@>
@-k@>
@-n@>
@-pp@>
@z

Section 121.

@x
@ Let us consider the big switch for productions now, before looking
@y
@ Let us consider the big switch for productions now, before looking
@-cat1@>
@-cat2@>
@-cat3@>
@$cat1 {CTWILL}121 =$(\\{pp}+\T{1})\MG\\{cat}$@>
@$cat2 {CTWILL}121 =$(\\{pp}+\T{2})\MG\\{cat}$@>
@$cat3 {CTWILL}121 =$(\\{pp}+\T{3})\MG\\{cat}$@>
@z

Section 122.

@x
`\.{@@!@@:operator+=\}\{\\\&\{operator\} \$+\{=\}\$@@>}').
@y
`\.{@@!@@:operator+=\}\{\\\&\{operator\} \$+\{=\}\$@@>}').
@-find_first_ident@>
@-make_reserved@>
@-make_underlined@>
@-underline_xref@>
@-no_ident_found@>
@-case_found@>
@-operator_found@>
@$no_ident_found {CTWILL}122 =\hfil\break(\&{token\_pointer}) 0@>
@$case_found {CTWILL}122 =\hfil\break(\&{token\_pointer}) 1@>
@$operator_found {CTWILL}122 =\hfil\break(\&{token\_pointer}) 2@>
@z

Section 124.

@x
make_reserved( /* make the first identifier in |p->trans| like |int| */
scrap_pointer p)
@y
make_reserved( /* make the first identifier in |p->trans| like |int| */
scrap_pointer p)
@-make_reserved@>
@$make_reserved {CTWILL}124 \&{static} \&{void} (\,)@>
@z

Section 125.

@x
make_underlined(
/* underline the entry for the first identifier in |p->trans| */
scrap_pointer p)
@y
make_underlined(
/* underline the entry for the first identifier in |p->trans| */
scrap_pointer p)
@-make_underlined@>
@$make_underlined {CTWILL}125 \&{static} \&{void} (\,)@>
@z

@x
    return; /* this happens, for example, in |case found:| */
@y
    return; /* this happens, for example, in \&{case} \\{found}: */
@z

Section 155.

@x
@d force_lines flags['f'] /* should each statement be on its own line? */
@y
@d force_lines flags['f'] /* should each statement be on its own line? */
@-force_lines@>
@$force_lines {CTWILL}155 =\\{flags}[\.{'f'}]@>
@z

Section 187.

@x
@ @<Print a snapsh...@>=
@y
@ @<Print a snapsh...@>=
@-n@>
@%
@$n {CTWILL}182 \&{short}@>
@$n {CTWILL}183 \&{short}@>
@z

Section 189.

@x
@ @<Predecl...@>=@+static text_pointer translate(void);
@y
@ @<Predecl...@>=@+static text_pointer translate(void);
@-translate@>
@z

Section 193.

@x
    if (next_control=='|' || next_control==begin_comment ||
        next_control==begin_short_comment) return;
@y
@-C_parse@>
@$C_parse {CTWILL}193 \&{static} \&{void} (\,)@>
    if (next_control=='|' || next_control==begin_comment @| ||
        next_control==begin_short_comment) return;
@z

Section 194.

@x
@ @<Predecl...@>=@+static void C_parse(eight_bits);
@y
@ @<Predecl...@>=@+static void C_parse(eight_bits);
@-C_parse@>
@z

Section 195.

@x
@ The following macro is used to append a scrap whose tokens have just
@y
@ The following macro is used to append a scrap whose tokens have just
@-b@>
@-c@>
@z

Section 204.

@x
@d make_pb flags['e']
@y
@d make_pb flags['e']
@-make_pb@>
@$make_pb {CTWILL}204 =\\{flags}[\.{'e'}]@>
@z

Section 208.

@x
@d cur_mode cur_state.mode_field /* current mode of interpretation */
@y
@d cur_mode cur_state.mode_field /* current mode of interpretation */
@-cur_end@>
@-cur_tok@>
@-cur_mode@>
@$cur_end {CTWILL}208 =\\{cur\_state}.\\{end\_field}@>
@$cur_tok {CTWILL}208 =\\{cur\_state}.\\{tok\_field}@>
@$cur_mode {CTWILL}208 =\\{cur\_state}.\\{mode\_field}@>
@z

Section 211.

@x
push_level( /* suspends the current level */
text_pointer p)
@y
push_level( /* suspends the current level */
text_pointer p)
@-push_level@>
@$push_level {CTWILL}211 \&{static} \&{void} (\,)@>
@z

Section 228.

@x
@ @<Predecl...@>=@+static void phase_two(void);
@y
@ @<Predecl...@>=@+static void phase_two(void);
@-phase_two@>
@z

Section 230.

@x
@ @d usage_sentinel (struct perm_meaning *)1
@y
@ @d usage_sentinel (struct perm_meaning *)1
@-usage_sentinel@>
@$usage_sentinel {CTWILL}230 =(\&{struct} \&{perm\_meaning} ${}{*}{}$) \T{1}@>
@z

Section 234.

@x
finish_C( /* finishes a definition or a \CEE/ part */
  boolean visible) /* |true| if we should produce \TeX\ output */
@y
finish_C( /* finishes a definition or a \CEE/ part */
  boolean visible) /* |true| if we should produce \TeX\ output */
@-finish_C@>
@$finish_C {CTWILL}234 \&{static} \&{void} (\,)@>
@z

Section 235.

@x
@ @<Predecl...@>=@+static void finish_C(boolean);
@y
@ @<Predecl...@>=@+static void finish_C(boolean);
@-finish_C@>
@z

Section 243.

@x
footnote( /* outputs section cross-references */
sixteen_bits flag)
@y
footnote( /* outputs section cross-references */
sixteen_bits flag)
@-footnote@>
@$footnote {CTWILL}243 \&{static} \&{void} (\,)@>
@z

Section 244.

@x
@ @<Predecl...@>=@+static void footnote(sixteen_bits);
@y
@ @<Predecl...@>=@+static void footnote(sixteen_bits);
@-footnote@>
@z

Section 248.

@x
@ @<Predecl...@>=@+static void phase_three(void);
@y
@ @<Predecl...@>=@+static void phase_three(void);
@-phase_three@>
@z

Section 251.

@x
@ A left-to-right radix sorting method is used, since this makes it easy to
@y
@ A left-to-right radix sorting method is used, since this makes it easy to
@-c@>
@z

Section 253.

@x
@<Rest of |trans_plus| union@>=
@y
@<Rest of |trans_plus| union@>=
@-j@>
@-k@>
@z

Section 254.

@x
@ @d depth cat /* reclaims memory that is no longer needed for parsing */
@y
@ @d depth cat /* reclaims memory that is no longer needed for parsing */
@-depth@>
@-head@>
@-sort_ptr@>
@-max_sorts@>
@$depth {CTWILL}254 =\\{cat}@>
@$head {CTWILL}254 =\\{trans\_plus}.\\{Head}@>
@$sort_ptr {CTWILL}254 =\\{scrap\_ptr}@>
@z

Section 258.

@x
@ Procedure |unbucket| goes through the buckets and adds nonempty lists
@y
@ Procedure |unbucket| goes through the buckets and adds nonempty lists
@-unbucket@>
@$unbucket {CTWILL}258 \&{static} \&{void} (\,)@>
@z

Section 259.

@x
@ @<Predecl...@>=@+static void unbucket(eight_bits);
@y
@ @<Predecl...@>=@+static void unbucket(eight_bits);
@-unbucket@>
@z

Section 261.

@x
  next_name=sort_ptr->head;
@y
  next_name=sort_ptr->head;@/
@z

Section 262.

@x
  cur_name=sort_ptr->head;
@y
  cur_name=sort_ptr->head;@/
@z

Section 268.

@x
@ @<Predecl...@>=@+static void section_print(name_pointer);
@y
@ @<Predecl...@>=@+static void section_print(name_pointer);
@-section_print@>
@z

Section 270.

@x
  puts(_("\nMemory usage statistics:"));
@.Memory usage statistics:@>
  printf(_("%td names (out of %ld)\n"),@^system dependencies@>
            (ptrdiff_t)(name_ptr-name_dir),(long)max_names);
  printf(_("%td cross-references (out of %ld)\n"),
            (ptrdiff_t)(xref_ptr-xmem),(long)max_refs);
  printf(_("%td bytes (out of %ld)\n"),
            (ptrdiff_t)(byte_ptr-byte_mem),(long)max_bytes);
  printf(_("%td temp meanings (out of %ld)\n"),
            (ptrdiff_t)(max_temp_meaning_ptr-temp_meaning_stack),
            (long)max_meanings);
  printf(_("%td titles (out of %ld)\n"),
            (ptrdiff_t)(title_code_ptr-title_code),(long)max_titles);
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
@y
  puts(_("\nMemory usage statistics:"));@/
@.Memory usage statistics:@>
  printf(_("%td names (out of %ld)\n"),@^system dependencies@>
            (ptrdiff_t)(name_ptr-name_dir),@/
            @t\5\5\5\5@>(long)max_names);@/
  printf(_("%td cross-references (out of %ld)\n"),
            (ptrdiff_t)(xref_ptr-xmem),(long)max_refs);@/
  printf(_("%td bytes (out of %ld)\n"),
            (ptrdiff_t)(byte_ptr-byte_mem),@/
            @t\5\5\5\5@>(long)max_bytes);@/
  printf(_("%td temp meanings (out of %ld)\n"),
            (ptrdiff_t)(max_temp_meaning_ptr-temp_meaning_stack),@/
            @t\5\5\5\5@>(long)max_meanings);@/
  printf(_("%td titles (out of %ld)\n"),
            (ptrdiff_t)(title_code_ptr-title_code),@/
            @t\5\5\5\5@>(long)max_titles);@/
  puts(_("Parsing:"));@/
  printf(_("%td scraps (out of %ld)\n"),
            (ptrdiff_t)(max_scr_ptr-scrap_info),@/
            @t\5\5\5\5@>(long)max_scraps);@/
  printf(_("%td texts (out of %ld)\n"),
            (ptrdiff_t)(max_text_ptr-tok_start),@/
            @t\5\5\5\5@>(long)max_texts);@/
  printf(_("%td tokens (out of %ld)\n"),
            (ptrdiff_t)(max_tok_ptr-tok_mem),@/
            @t\5\5\5\5@>(long)max_toks);@/
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_stack_ptr-stack),@/
            @t\5\5\5\5@>(long)stack_size);@/
  puts(_("Sorting:"));@/
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_sort_ptr-scrap_info),@/
            @t\5\5\5\5@>(long)max_scraps);
@z

Section 272.

@x
@q Section 2->272. @>
@* {\tentex CTWILL} user manual.
@y
@* {\tentex CTWILL} user manual.
@-s@>
@-f@>
@-x@>
@-printf@>
@z

Section 274.

@x
static struct perm_meaning {
@y
static struct perm_meaning {
@-perm_meaning@>
@$perm_meaning {CTWILL}274 \&{static} \&{struct}@>
@z

Section 283.

@x
@ @<Predec...@>=@+static boolean app_supp(text_pointer);
@y
@ @<Predec...@>=@+static boolean app_supp(text_pointer);
@-app_supp@>
@z

Section 284.

@x
@ The trickiest part of \.{CTWILL} is the procedure |make_ministring(pp+l)|,
@y
@ The trickiest part of \.{CTWILL} is the procedure |make_ministring(pp+l)|,
@-a@>
@-b@>
@-c@>
@-l@>
@-make_ministring@>
@$make_ministring {CTWILL}284 \&{static} \&{void} (\,)@>
@z

Section 285.

@x
@ @<Predec...@>=@+static void make_ministring(scrap_pointer);
@y
@ @<Predec...@>=@+static void make_ministring(scrap_pointer);
@-make_ministring@>
@z

Section 293.

@x
@ @<Write the new meaning to the \.{.aux} file@>=
@y
@ @<Write the new meaning to the \.{.aux} file@>=
@%
@$p {CTWILL}277 \&{name\_pointer}@>
@$q {CTWILL}277 \&{struct perm\_meaning} ${}{*}{}$@>
@z

Section 294.

@x
@<Flag the usage of this identifier, for the mini-index@>=
@y
@<Flag the usage of this identifier, for the mini-index@>=
@%
@$p {CTWILL}202 \&{name\_pointer}@>
@z

Section 297.

@x
@ @<Predec...@>=@+static void out_mini(meaning_struct *);
@y
@ @<Predec...@>=@+static void out_mini(meaning_struct *);
@-out_mini@>
@z

Section 300.

@x
@ @<Predec...@>=@+static sixteen_bits title_lookup(void);
@y
@ @<Predec...@>=@+static sixteen_bits title_lookup(void);
@-title_lookup@>
@z

Section 303.

@x
@d indent_param_decl flags['i'] /* should formal parameter declarations be indented? */
@y
@d indent_param_decl flags['i'] /* should formal parameter declarations be indented? */
@-indent_param_decl@>
@$indent_param_decl {CTWILL}303 =\\{flags}[\.{'i'}]@>
@z

Section 304.

@x
@d order_decl_stmt flags['o'] /* should declarations and statements be separated? */
@y
@d order_decl_stmt flags['o'] /* should declarations and statements be separated? */
@-order_decl_stmt@>
@$order_decl_stmt {CTWILL}304 =\\{flags}[\.{'o'}]@>
@z

Section 310.

@x l.102 of CTWILL-TEXLIVE.CH
@d kpse_find_cweb(name) kpse_find_file(name,kpse_cweb_format,true)
@y
@d kpse_find_cweb(name) kpse_find_file(name,kpse_cweb_format,true)
@-name@>
@z
