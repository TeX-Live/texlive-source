/* 
Copyright (c) 2008 jerome DOT laurens AT u-bourgogne DOT fr

This file is part of the SyncTeX package.

License:
--------
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE

Except as contained in this notice, the name of the copyright holder  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from the copyright holder.

Important notice:
-----------------
This file is named "synctex.c", it may or may not have a header counterpart
depending on its use.  It aims to provide basic components useful for the
input/output synchronization technology for TeX.
The purpose of the implementation is threefold
- firstly, it defines a new input/output synchronization technology named
  "synchronize texnology", "SyncTeX" or "synctex"
- secondly, it defines the naming convention and format of the auxiliary file
  used by this technology
- thirdly, it defines the API of a controller and a controller, used in
  particular by the pdfTeX and XeTeX programs to prepare synchronization.

All these are up to a great extent de facto definitions, which means that they
are partly defined by the implementation itself.

This technology was first designed for pdfTeX, an extension of TeX managing the
pdf output file format, but it can certainly be adapted to other programs built
from TeX as long as the extensions do not break too much the core design.
Moreover, the synchronize texnology only relies on code concept and not
implementation details, so it can be ported to other TeX systems.  In order to
support SyncTeX, one can start reading the dedicated section in synctex.ch,
sync-pdftex.ch and sync-xetex.ch.

Other existing public synchronization technologies are defined by srcltx.sty -
also used by source specials - and pdfsync.sty.  Like them, the synchronize
texnology is meant to be shared by various text editors, viewers and TeX
engines.  A centralized reference and source of information is available on
CTAN, in directory support/synctex.

Versioning:
-----------
As synctex is embedded into different TeX implementation, there is an independent
versionning system.
For TeX implementations, the actual version is: 1
For .synctex file format, the actual version is SYNCTEX_VERSION below

Please, do not remove these explanations.

*/

#  define SYNCTEX_DEBUG 0

extern char *gettexstring(int n);

#  define EXTERN extern

#  define SYNCTEX_VERSION 1

/* Some parts of the code may differ depending on the TeX engine.
 * Here are the possible switches:  */
#  define SYNCTEX_ENGINE_TEX 0
#  define SYNCTEX_ENGINE_PDFTEX 1
/* Default engine (suitable for xetex): */
#  define SYNCTEX_ENGINE SYNCTEX_ENGINE_TEX

/* Other parts of the code may differ depending on the ouput mode,
 * dvi or xdv vs pdf, in particular the management of magnification.
 * The default is not pdf mode.  */
#  define SYNCTEX_IS_PDF 0

#  if defined(XETEX_MAC) || defined(XETEX_OTHER)

#  include <stdio.h>

#  include "xetexd.h"
  /* this will define XeTeX, which we can use in later conditionals */

#  include <xetexdir/xetexextra.h>

#  define MAX(a, b) ((a) > (b) ? (a) : (b))
#  define xfree(x) do { if (x) free(x); } while (0)

#  else

#  ifdef pdfTeX
#    undef pdfTeX
#  endif

#  include "pdftexd.h"

#  undef  SYNCTEX_ENGINE
#  define SYNCTEX_ENGINE SYNCTEX_ENGINE_PDFTEX

/* We observe pdfoutputvalue in order to determine whether output mode is pdf.
 * We will assume that pdf_output_value equals pdf_output before entering
 * the synctex_sheet function below.  */
#  undef  SYNCTEX_IS_PDF
#  define SYNCTEX_IS_PDF (pdfoutputvalue>0)

#  endif /* defined XETEX_* */

#  if defined(pdfTeX) || defined(XeTeX) || defined(__SyncTeX__)

/*  Here are all the local variables gathered in one "synchronization context"  */
static struct {
    FILE *file;                 /*  the foo.synctex I/O identifier  */
    char *name;                 /*  the real "foo.synctex" name  */
    char *root_name;            /*  in general jobname.tex  */
    integer count;              /*  The number of records in "foo.synctex"  */
    /*  next concern the last sync record encountered  */
    halfword p;                 /*  the last synchronized node, must be set 
                                 *  before the recorder */
    void (*recorder) (halfword p);      /*  the recorder of the node above, the
                                         *  routine that knows how to record the 
                                         *  node to the .synctex file */
    integer h, v;               /*  the last sync record coordinates  */
    integer offset;             /*  the offset of the origin / the topleft of
                                 *  the page in both directions  */
    integer scale;              /*  Some magnification factor to define UNIT */
} synctex_ctxt = {
NULL, NULL, NULL, 0, 0, NULL, 0, 0, 0, 0};

/*  the macros defined below do the same job than their almost eponym
 *  counterparts of *tex.web, the memory access is sometimes more direct
 *  because *tex.web won't share its own constants the main purpose is to
 *  maintain very few hook points into *tex.web in order both to ensure
 *  portability and not modifying to much the original code.  see texmfmem.h
 *  and *tex.web for details, the synctex_ prefix prevents name conflicts, it
 *  is some kind of namespace
*/
#  warning These structures MUST be kept in synchronization with the main program
/*  synctexoption is a global integer variable defined in *tex.web
 *  it is set to 1 by texmfmp.c if the command line has the '-synchronize=1'
 *  option.  */
#  define SYNCTEX_OPTIONS synctexoption
#  define SYNCTEX_DISABLED_MASK 0x80000000
/*  if the SYNCTEX_DISABLED_MASK bit of SYNCTEX_OPTIONS is set, the
 *  synchronization is definitely disabled.  */
#  define SYNCTEX_IGNORE_CLI_MASK 0x40000000
/*  if the SYNCTEX_IGNORE_CLI_MASK bit of SYNCTEX_OPTIONS is set, the option
 *  given from the command line is ignored.  */

/*  glue code: really define the main memory,
 *  this is exactly the same "mem" as in *tex.web.  */
#  define mem zmem
/*  glue code: synctexoffset is a global integer variable defined in *tex.web
 *  it is set to the offset where the primitive \synctex reads and writes its
 *  value.  */
#  define SYNCTEX_IS_ENABLED zeqtb[synctexoffset].cint
/*  if there were a mean to share the value of synctex_code between pdftex.web
 *  and this file, it would be great.  */

/*  synctex_dot_open ensures that the foo.synctex file is open.
 *  In case of problem, it definitely disables synchronization.
 *  Now all the synchronization info is gathered in only one file.
 *  It is possible to split this info into as many different output files as sheets
 *  plus 1 for the control but the overall benefits are not so clear.
 *      For example foo-i.synctex would contain input synchronization
 *      information for page i alone.
*/
static FILE *synctex_dot_open(void)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nwarning: Synchronize DEBUG: synctex_dot_open\n");
    fprintf(stdout, "\nwarning: SYNCTEX_IS_ENABLED=%0X\n", SYNCTEX_IS_ENABLED);
    fprintf(stdout, "\nwarning: SYNCTEX_OPTIONS=%0X\n", SYNCTEX_OPTIONS);
#  endif
    if (0 != (SYNCTEX_OPTIONS & SYNCTEX_DISABLED_MASK)) {
        return 0;      /*  synchronization is definitely disabled: do nothing  */
    }
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nwarning: Synchronize DEBUG: synctex_dot_open 1\n");
#  endif
    if (NULL == synctex_ctxt.file) {
        /*  this is the first time we are asked to open the file
           this part of code is executed only once:
           either synctex_ctxt.file is nonnegative or synchronization is
           definitely disabled. */
        static char *suffix = ".synctex";
        /*  jobname was set by the \jobname command on the *TeX side  */
        char *tmp = gettexstring(jobname);
        /*  jobname was set by the \jobname command on the *TeX side  */
        char *the_syncname = xmalloc(strlen(tmp) + strlen(suffix) + 1);
        strcpy(the_syncname, tmp);
        xfree(tmp);
        strcat(the_syncname, suffix);
        synctex_ctxt.file = xfopen(the_syncname, FOPEN_W_MODE);
#  if SYNCTEX_DEBUG
        fprintf(stdout, "\nwarning: Synchronize DEBUG: synctex_dot_open 2\n");
#  endif
        if (NULL != synctex_ctxt.file) {
            /*  synctex_ctxt.name was NULL before, it now owns the_syncname */
            synctex_ctxt.name = the_syncname;
            /*  print the preamble, this is an UTF8 file  */
            fprintf(synctex_ctxt.file, "SyncTeX\nversion:%d\n",
                SYNCTEX_VERSION);
            if (NULL != synctex_ctxt.root_name) {
                fprintf(synctex_ctxt.file, "i:1:%s\n", synctex_ctxt.root_name);
                xfree(synctex_ctxt.root_name);
                synctex_ctxt.root_name = NULL;
            }
#  if SYNCTEX_DEBUG
            fprintf(stdout,
                    "\nwarning: Synchronize DEBUG: synctex_dot_open SYNCTEX AVAILABLE\n");
#  endif
        } else {
            /*  no .synctex file available, so disable synchronization  */
            SYNCTEX_OPTIONS = SYNCTEX_DISABLED_MASK;
            /* and free the_syncname */
            xfree(the_syncname);
            the_syncname = NULL;
#  if SYNCTEX_DEBUG
            fprintf(stdout,
                    "\nwarning: Synchronize DEBUG: synctex_dot_open SYNCTEX DISABLED\n");
#  endif
        }
    }
    return synctex_ctxt.file;
}

/*  Each time TeX opens a file, it sends a syncstartinput message and enters
 *  this function.  Here, a new synchronization tag is created and stored in
 *  the synctex_tag_field of the TeX current input context.  Each synchronized
 *  TeX node will record this tag instead of the file name.  syncstartinput
 *  writes the mapping synctag <-> file name to the .synctex file.  A client
 *  will read the .synctex file and retrieve this mapping, it will be able to
 *  open the correct file just knowing its tag.  If the same file is read
 *  multiple times, it might be associated to different tags.  Synchronization
 *  controller, either in viewers, editors or standalone should be prepared to
 *  handle this situation and take the appropriate action of they want to
 *  optimize memory.  No two different files will have the same positive tag.
 *  It is not advisable to definitely store the file names here.  If the file
 *  names ever have to be stored, it should definitely be done at the TeX level
 *  just like src-specials do such that other components of the program can use
 *  it.  This function does not make any difference between the files, it
 *  treats the same way .tex, .aux, .sty ... files, even if many of them do not
 *  contain any material meant to be typeset.
*/
void synctexstartinput(void)
{
    static unsigned int synctex_tag_counter = 0;

#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nwarning: Synchronize DEBUG: synctexstartinput %i\n",
            synctex_tag_counter);
    fprintf(stdout, "\nwarning: SYNCTEX_IS_ENABLED=%i\n", SYNCTEX_IS_ENABLED);
    fprintf(stdout, "\nwarning: SYNCTEX_OPTIONS=%0X\n", SYNCTEX_OPTIONS);
    fprintf(stdout, "\nwarning: SYNCTEX_DISABLED_MASK=%0X\n",
            SYNCTEX_DISABLED_MASK);
#  endif

    if (0 != (SYNCTEX_OPTIONS & SYNCTEX_DISABLED_MASK)) {
        /*  this is where we disable synchronization -synchronization=-1  */
        return;
    }
    /*  synctex_tag_counter is a counter uniquely identifying the file actually
     *  open each time tex opens a new file, syncstartinput will increment this
     *  counter  */
    if (~synctex_tag_counter > 0) {
        ++synctex_tag_counter;
    } else {
        /*  we have reached the limit, subsequent files will be softly ignored
         *  this makes a lot of files... even in 32 bits  */
        curinput.synctextagfield = 0;
        return;
    }

    if (0 == (SYNCTEX_OPTIONS & SYNCTEX_IGNORE_CLI_MASK)) {
        /*  the command line options are not ignored  */
        SYNCTEX_IS_ENABLED = MAX(SYNCTEX_OPTIONS, SYNCTEX_IS_ENABLED);
        SYNCTEX_OPTIONS |= SYNCTEX_IGNORE_CLI_MASK;
        /*  the command line options will be ignored from now on.  Every
         *  subsequent call of syncstartinput won't get there SYNCTEX_OPTIONS
         *  is now the list of option flags  */
    }

    curinput.synctextagfield = synctex_tag_counter;     /*  -> *TeX.web  */
    if (synctex_tag_counter == 1) {
        /*  this is the first file TeX ever opens, in general \jobname.tex we
         *  do not know yet if synchronization will ever be enabled so we have
         *  to store the file name, because we will need it later This is
         *  certainly not necessary due to \jobname  */
        synctex_ctxt.root_name = gettexstring(curinput.namefield);
        return;
    }
    if ((NULL != synctex_ctxt.file)
        || ((SYNCTEX_IS_ENABLED && synctex_dot_open()) != 0)) {
        char *tmp = gettexstring(curinput.namefield);
        fprintf(synctex_ctxt.file,"i:%u:%s\n",curinput.synctextagfield,tmp);
        xfree(tmp);
    }
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nwarning: Synchronize DEBUG: synctexstartinput END\n");
#  endif
    return;
}

/*  All the synctex... functions below have the smallest set of parameters.  It
 *  appears to be either the address of a node, or nothing at all.  Using zmem,
 *  which is the place where all the nodes are stored, one can retrieve every
 *  information about a node.  The other information is obtained through the
 *  global context variable.
*/

/*  Recording the "s:..." line.  In *tex.web, use synctex_sheet(pdf_output) at
 *  the very beginning of the ship_out procedure.
*/
void synctexsheet(integer mag)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctexsheet %i\n",mag);
#  endif
    if (0 != (SYNCTEX_OPTIONS & SYNCTEX_DISABLED_MASK)) {
        return;
    }
    if ((synctex_ctxt.file != NULL)
        || ((SYNCTEX_IS_ENABLED != 0) && (synctex_dot_open() != 0))) {
        /*  tries to open the .synctex, useful if synchronization was enabled
         *  from the source file and not from the CLI
         *  totalpages is defined in tex.web   */
        synctex_ctxt.count = 0;
        if (totalpages == 0) {
            if (SYNCTEX_IS_PDF != 0) {
                fprintf(synctex_ctxt.file, ">:pdf\n");
            }
            /*  Now it is time to properly set up the scale factor.
             *  Depending on the output mode
             *  dvi and pdf, don't start from the same origin.
             *  dvi starts at (1in,1in) from the top left corner
             *  pdf starts exactly from the top left corner.
             *  mag is a global TeX variable.  */
            if(mag>0) {
                synctex_ctxt.scale = 8192000/mag;
            }
            else {
                synctex_ctxt.scale = 8192;
             }
            fprintf(synctex_ctxt.file, "z:%ld\n", ((SYNCTEX_IS_PDF != 0) ? 0 : 578));
#  if SYNCTEX_DEBUG
            fprintf(stdout, "\nSynchronize DEBUG: synctex_ctxt.scale:%u\n",synctex_ctxt.scale);
#  endif
        }
        fprintf(synctex_ctxt.file, "s:%ld\n", (long int) totalpages + 1);
        ++synctex_ctxt.count;
    }
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctexsheet END\n");
#  endif
    return;
}

#  define UNIT / synctex_ctxt.scale
/*  UNIT is the scale. TeX coordinates are very accurate and client won't need
 *  that, at leat in a first step.  1.0 <-> 2^16 = 65536. 
 *  The TeX unit is sp (scaled point) or pt/65536 which means that the scale
 *  factor to retrieve a bp unit (a postscript) is 72/72.27/65536 =
 *  1/4096/16.06 = 1/8192/8.03
 *  Here we use 1/8192 as scale factor, then we can limit ourselves to
 *  integers. This default value assumes that TeX magnification factor is 1000.
 *  The real TeX magnification factor is used to fine tune the synctex context
 *  scale in the synctex_dot_open function.
 *  IMPORTANT: We can say that the natural unit of .synctex files is 8192 sp.
 *  To retrieve the proper bp unit, we'll have to divide by 8.03.  To reduce
 *  rounding errors, we'll certainly have to add 0.5 for non negative integers
 *  and ±0.5 for negative integers.  This trick is mainly to gain speed and
 *  size. A binary file would be more appropriate in that respect, but I guess
 *  that some clients like auctex would not like it very much.  we cannot use
 *  "<<13" instead of "/8192" because the integers are signed and we do not
 *  want the sign bit to be propagated.  The origin of the coordinates is at
 *  the top left corner of the page.  For pdf mode, it is straightforward, but
 *  for dvi mode, we'll have to record the 1in offset in both directions,
 *  eventually modified by the magnification.
*/

/*  WARNING:
        The 5 definitions below must be in sync with their eponym declarations in *tex.web
*/
#  ifdef XeTeX
#  define synchronization_field_size 1
#  else
#  define synchronization_field_size 2
#  endif

#  define box_node_size (7+synchronization_field_size)
/*  see: @d box_node_size=...  */
#  define width_offset 1
/*  see: @d width_offset=...  */
#  define depth_offset 2
/*  see: @d depth_offset=...  */
#  define height_offset 3
/*  see: @d height_offset=...  */

/*  Now define the local version of width(##), height(##) and depth(##) macros
        These only depend on the macros above.  */
#  define SYNCTEX_WIDTH(NODE) mem[NODE+width_offset].cint
#  define SYNCTEX_DEPTH(NODE) mem[NODE+depth_offset].cint
#  define SYNCTEX_HEIGHT(NODE) mem[NODE+height_offset].cint

#  ifdef XeTeX
/* in XeTeX, "halfword" fields are at least 32 bits, so we'll use those for
 * tag and line so that the sync field size is only one memory_word. */
#  define SYNCTEX_TAG(NODE) mem[NODE+box_node_size-1].hh.lhfield
#  define SYNCTEX_LINE(NODE) mem[NODE+box_node_size-1].hh.rh
#  else
/*  The tag and the line are just the two last words of the node.  This is a
 *  very handy design but this is not strictly required by the concept.  If
 *  really necessary, one can define other storage rules.  */
#  define SYNCTEX_TAG(NODE) mem[NODE+box_node_size-2].cint
#  define SYNCTEX_LINE(NODE) mem[NODE+box_node_size-1].cint
#endif

/*  When an hlist ships out, it can contain many different kern nodes with
 *  exactly the same sync tag and line.  To reduce the size of the .synctex
 *  file, we only display a kern node sync info when either the sync tag or the
 *  line changes.  Also, we try ro reduce the distance between the chosen nodes
 *  in order to improve accuracy.  It means that we display information for
 *  consecutive nodes, as far as possible.  This tricky part uses a "recorder",
 *  which is the address of the routine that knows how to write the
 *  synchronization info to the .synctex file.  It also uses criteria to detect
 *  a change in the context, this is the macro SYNCTEX_CONTEXT_DID_CHANGE The
 *  SYNCTEX_IGNORE macro is used to detect unproperly initialized nodes.  See
 *  details in the implementation of the functions below.  */
#  define SYNCTEX_IGNORE(NODE) (0 != (SYNCTEX_OPTIONS & SYNCTEX_DISABLED_MASK) ) \
                                || (SYNCTEX_IS_ENABLED == 0) \
                                || (synctex_ctxt.file == 0)

/*  Recording a "h:..." line  */
void synctex_hlist_recorder(halfword p)
{
    fprintf(synctex_ctxt.file, "h:%u:%u(%i,%i,%i,%i,%i)\n",
            SYNCTEX_TAG(p), SYNCTEX_LINE(p),
            curh UNIT,
            curv UNIT,
            SYNCTEX_WIDTH(p) UNIT,
            SYNCTEX_HEIGHT(p) UNIT,
            SYNCTEX_DEPTH(p) UNIT);
    ++synctex_ctxt.count;
}

/*  This message is sent when an hlist will be shipped out, more precisely at
 *  the beginning of the hlist_out procedure in *TeX.web.  It will be balanced
 *  by a synctex_tsilh, sent at the end of the hlist_out procedure.  p is the
 *  address of the hlist We assume that p is really an hlist node! */
void synctexhlist(halfword p)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctexhlist\n");
#  endif
    if (SYNCTEX_IGNORE(p)) {
        return;
    }
    synctex_ctxt.p = 0;             /*  reset  */
    synctex_ctxt.recorder = NULL;   /*  reset  */
    synctex_hlist_recorder(p);
}

/*  Recording a "e" line ending an hbox this message is sent whenever an hlist
 *  has been shipped out it is used to close the hlist nesting level. It is
 *  sent at the end of the hlist_out procedure in *TeX.web to balance a former
 *  synctex_hlist sent at the beginning of that procedure.    */
void synctextsilh(halfword p)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctextsilh\n");
#  endif
    if (SYNCTEX_IGNORE(p)) {
        return;
    }
    /*  is there a pending info to be recorded  */
    if (NULL != synctex_ctxt.recorder) {
        /*  synctex_ctxt node is set and must be recorded as last node  */
        (*synctex_ctxt.recorder) (synctex_ctxt.p);
        synctex_ctxt.p = 0;     /*  force next node to be recorded!  */
        synctex_ctxt.recorder = NULL;
    }
    fputs("e\n", synctex_ctxt.file);
}

#  undef SYNCTEX_IGNORE
#  define SYNCTEX_IGNORE(NODE) (0 != (SYNCTEX_OPTIONS & SYNCTEX_DISABLED_MASK) ) \
                                || (0 == SYNCTEX_IS_ENABLED) \
                                || (0 >= SYNCTEX_TAG(NODE)) \
                                || (0 >= SYNCTEX_LINE(NODE)) \
                                || (0 == synctex_ctxt.file)
#  undef SYNCTEX_TAG
#  undef SYNCTEX_LINE
/*  glue code: these only work with nodes of size medium_node_size  */
#  define small_node_size 2
/*  see: @d small_node_size=2 {number of words to allocate for most node types}  */
#  define medium_node_size (small_node_size+synchronization_field_size)
#  ifdef XeTeX
#  define SYNCTEX_TAG(NODE) mem[NODE+medium_node_size-1].hh.lhfield
#  define SYNCTEX_LINE(NODE) mem[NODE+medium_node_size-1].hh.rh
#  else
#  define SYNCTEX_TAG(NODE) mem[NODE+medium_node_size-2].cint
#  define SYNCTEX_LINE(NODE) mem[NODE+medium_node_size-1].cint
#  endif

/*  This macro will detect a change in the synchronization context.  As long as
 *  the synchronization context remains the same, there is no need to write
 *  synchronization info: it would not help more.  The synchronization context
 *  has changed when either the line number or the file tag has changed.  */
#  define SYNCTEX_CONTEXT_DID_CHANGE ((0 == synctex_ctxt.p)\
                                      || (SYNCTEX_TAG(p) != SYNCTEX_TAG(synctex_ctxt.p))\
                                      || (SYNCTEX_LINE(p) != SYNCTEX_LINE(synctex_ctxt.p)))

/*  Recording a "$:..." line  */
void synctex_math_recorder(halfword p)
{
    fprintf(synctex_ctxt.file, "$:%u:%u(%i,%i)\n",
            SYNCTEX_TAG(p), SYNCTEX_LINE(p),
            synctex_ctxt.h UNIT,
            synctex_ctxt.v UNIT);
    ++synctex_ctxt.count;
}

/*  glue code this message is sent whenever an inline math node will ship out
        See: @ @<Output the non-|char_node| |p| for...  */
void synctexmath(halfword p)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctexmath\n");
#  endif
    if (SYNCTEX_IGNORE(p)) {
        return;
    }
    if ((synctex_ctxt.recorder != NULL) && SYNCTEX_CONTEXT_DID_CHANGE) {
        /*  the sync context did change  */
        (*synctex_ctxt.recorder) (synctex_ctxt.p);
    }
    synctex_ctxt.h = curh;
    synctex_ctxt.v = curv;
    synctex_ctxt.p = p;
    synctex_ctxt.recorder = NULL;/*  no need to record once more  */
    synctex_math_recorder(p);/*  always record  */
}

/*  Recording a "g:..." line  */
void synctex_glue_recorder(halfword p)
{
    fprintf(synctex_ctxt.file, "g:%u:%u(%i,%i)\n",
            SYNCTEX_TAG(p), SYNCTEX_LINE(p),
            synctex_ctxt.h UNIT,
            synctex_ctxt.v UNIT);
    ++synctex_ctxt.count;
}

/*  this message is sent whenever a glue node ships out
        See: @ @<Output the non-|char_node| |p| for...    */
void synctexglue(halfword p)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctexglue\n");
#  endif
    if (SYNCTEX_IGNORE(p)) {
        return;
    }
    if (SYNCTEX_CONTEXT_DID_CHANGE) {
        /*  the sync context has changed  */
        if (synctex_ctxt.recorder != NULL) {
            /*  but was not yet recorded  */
            (*synctex_ctxt.recorder) (synctex_ctxt.p);
        }
        synctex_ctxt.h = curh;
        synctex_ctxt.v = curv;
        synctex_ctxt.p = p;
        synctex_ctxt.recorder = NULL;
        /*  always record when the context has just changed  */
        synctex_glue_recorder(p);
    } else {
        /*  just update the geometry and type (for future improvements)  */
        synctex_ctxt.h = curh;
        synctex_ctxt.v = curv;
        synctex_ctxt.p = p;
        synctex_ctxt.recorder = &synctex_glue_recorder;
    }
}

/*  Recording a "k:..." line  */
void synctex_kern_recorder(halfword p)
{
    fprintf(synctex_ctxt.file, "k:%u:%u(%i,%i,%i)\n",
            SYNCTEX_TAG(p), SYNCTEX_LINE(p),
            synctex_ctxt.h UNIT,
            synctex_ctxt.v UNIT,
            SYNCTEX_WIDTH(p) UNIT);
    ++synctex_ctxt.count;
}

/*  this message is sent whenever a kern node ships out
        See: @ @<Output the non-|char_node| |p| for...    */
void synctexkern(halfword p)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctexkern\n");
#  endif
    if (SYNCTEX_IGNORE(p)) {
        return;
    }
    if (SYNCTEX_CONTEXT_DID_CHANGE || (SYNCTEX_WIDTH(p)<0)) {
        /*  the sync context has changed  */
        if (synctex_ctxt.recorder != NULL) {
            /*  but was not yet recorded  */
            (*synctex_ctxt.recorder) (synctex_ctxt.p);
        }
        synctex_ctxt.h = curh;
        synctex_ctxt.v = curv;
        synctex_ctxt.p = p;
        synctex_ctxt.recorder = NULL;
        /*  always record when the context has just changed  */
        synctex_kern_recorder(p);
    } else {
        /*  just update the geometry and type (for future improvements)  */
        synctex_ctxt.h = curh;
        synctex_ctxt.v = curv;
        synctex_ctxt.p = p;
        synctex_ctxt.recorder = &synctex_kern_recorder;
    }
}

/*  Free all memory used and close the file, sent by utils.c  */
void synctex_terminate(void)
{
#  if SYNCTEX_DEBUG
    fprintf(stdout, "\nSynchronize DEBUG: synctex_terminate\n");
#  endif
    if (synctex_ctxt.file != NULL) {
        fprintf(synctex_ctxt.file, "#:%u\n",synctex_ctxt.count);
        xfclose(synctex_ctxt.file, synctex_ctxt.name);
        xfree(synctex_ctxt.name);
    }
    xfree(synctex_ctxt.root_name);
}

#endif
