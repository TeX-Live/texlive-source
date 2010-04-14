/* ptexhelp.h: for the --help option"
*/
#ifndef PTEXHELP_H
#define PTEXHELP_H
/* block texk/web2c/help.h */
#define HELP_H

#ifdef PTFTOPL
const_string based_prog_name = "TFtoPL";
const_string PTEXTFTOPLHELP[] = {
"Usage: ptftopl [option] tfmfile [plfile]",
"",
"  -verbose               output progress reports.",
"  -version               print version information and exit.",
"  -help                  print this message and exit.",
"  -kanji={jis|euc|sjis|utf8}",
"                         plfile kanji code.",
NULL };
#endif /* PTFTOPL */

#ifdef PPLTOTF
const_string based_prog_name = "PLtoTF";
const_string PTEXPLTOTFHELP[] = {
"Usage: ppltotf [option] plfile [tfmfile]",
"",
"  -verbose               output progress reports.",
"  -version               print version information and exit.",
"  -help                  print this message and exit.",
"  -kanji={jis|euc|sjis|utf8}",
"                         plfile kanji code.",
NULL };
#endif /* PPLTOTF */

#ifdef PBIBTEX
const_string based_prog_name = "BibTeX";
const_string PBIBTEXHELP[] = {
"Usage: pbibtex [option] auxfile",
"",
"  -min-crossrefs=INTEGER     minimum number of cross-refs required",
"                             for automatic cite_list inclusion (default 2).",
"  -terse                     do silently.",
"  -version                   print version information and exit.",
"  -help                      print this message and exit.",
"  -kanji={jis|euc|sjis|utf8} kanji code for output-file.",
NULL };
#endif /* PBIBTEX */

#ifdef PDVITYPE
const_string based_prog_name = "DVItype";
const_string PDVITYPEHELP[] = {
"Usage: pdvitype [option] dvifile",
"",
"  -dpi=REAL                  assumed device resolution in pixels per inch",
"                             (default 300.0).",
"  -magnification=INTEGER     overrride the postamble's magnification.",
"  -max-pages=INTEGER         maximum number of pages (default 1000000).",
"  -output-level=1..4         verbosity level.",
"                                 1: terse, 2: medium-level, 3: verbose,",
"                                 4: plus check for postamble(default).",
"  -page-start=PAGE           type, e.g., `1.*.-5' to specify the first page",
"                             with \\count0=1, \\count2=4.",
"                             (see the TeX book chapter 15).",
"  -show-opcodes              show opcodes in dicimal.",
"  -kanji={jis|euc|sjis|utf8} kanji code for output-file.",
"  -version                   print version information and exit.",
"  -help                      print this message and exit.",
NULL };
#endif /* PDVITYPE */
#endif /* PTEXHELP_H */
