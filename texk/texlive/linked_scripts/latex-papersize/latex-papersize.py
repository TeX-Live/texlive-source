#!/usr/bin/env python
r"""
Calculate LaTeX paper and margin settings for arbitrary magnification
(C) Silas S. Brown, 2005-2009, 2016, 2019.  Version 1.63.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

When producing enlarged material in LaTeX for people with
low vision, it's often not enough to simply add such
things as "\Large" because that doesn't enlarge
_everything_ and it can be difficult to achieve the exact
desired size, especially if unusual packages are being
used as well.

It's more effective to change the LaTeX paper size and
margin settings to simulate SMALL PAPER, and then magnify
the result up to the desired physical paper size.  This
magnifies everything, and also adds some clarity for
low-vision users because fonts like CMR have different
versions at different sizes and the small-sized versions
are often meant to be clearer.

This is a Python script to calculate the necessary
settings for arbitrary font and page sizes.
Works in both Python 2 and Python 3.

BASIC USAGE

In the following instructions, base-size is the point size
that the TeX file is based on (if the documentclass
specifies 12pt then \small=11 normal=12 \large=14
\Large=17 \LARGE=20 \huge=25), and desired-size is the
point size that you want to produce.

The command-line parameters depend on whether you are
using latex/dvips or pdflatex.

FOR USE WITH LATEX AND DVIPS:

To print the geometry settings:

python latex-papersize.py base-size desired-size tex

e.g.: python latex-papersize.py 12 26 tex

(If you install latex-papersize.py as /usr/local/bin/latex-papersize,
then you can say "latex-papersize" instead of "python latex-papersize.py"
throughout these examples.)

The output of this command should then be placed after
\documentclass in your .tex file.

Then run latex to make a DVI file, and then do:

python latex-papersize.py base-size desired-size dvi-file

e.g.: python latex-papersize.py 12 26 myfile.dvi

which will print the appropriate dvips command.

FOR USE WITH PDFLATEX:

With pdflatex you need only one command:

python latex-papersize.py base-size desired-size pdftex

the output of this command should go just before \begin{document}.
NB if using hyperref package, put hyperref AFTER these
settings (and if also using pinyin package then put pinyin
package after that again)

PAGE NUMBERS:

When invoking latex-papersize.py with the "tex" or "pdftex"
options, you can optionally add a fourth parameter to
specify how many points to leave for page numbers at the
bottom of each page.  Normally 15 is a good idea.  Leaving
this out causes no room to be left for page numbers and
\pagestyle{empty} to be added.  This does not affect the
dvips command.

PAPER SIZES AND MARGINS:

It is assumed that the final physical printout will be on
A4 portrait with 10mm margins.  You can override this by
setting the paper_width and paper_height environment
variables (in millimetres) before running, e.g.:
paper_width=297 paper_height=210 python latex-papersize.py ...
You can also set the environment variables margin_left and
margin_top (the right and bottom margins are assumed to be
mirrors of these).

If using with non-PDF latex, remember to set the
environment variables again when asking latex-papersize.py for
the dvips command.  Or "export" them so they remain set.

POSTER PRINTING:

If you want to print on a physical paper size that is
larger than your printer can handle, you can make up the
larger size by sticking together smaller pieces of paper.
Perhaps the best way to do this is to use a separate
utility that knows about printable areas, cut margins,
etc, such as Jos van Eijndhoven's "poster" utility (last
known URL: ftp://ftp.es.ele.tue.nl/pub/users/jos/poster/poster.tar.gz )

In this case you can give latex-papersize.py a margin_left and
margin_top of 0 (because "poster" will handle the
margins), set the paper size and desired point size
appropriate for the final poster, use latex/dvips, and run
the resulting .ps through "poster" with "-s 1".  This is
better than getting "poster" to scale, because if any of
your fonts are rendered into bitmaps by Metafont (as some
CJK fonts are) then you will likely get a better
resolution if the final size is given to latex-papersize.py
rather than to "poster".

If you're aiming for a specific number of sheets of paper,
don't pick a physical size that's an exact multiple of
your printer's paper size, because "poster" accounts for
unprintable areas and overlaps slightly.  Multiply the
printer paper's height and width by 0.88 (or 0.86 if you
want a visual margin added to the finished poster besides
the cutting margins) and try a multiple of that size.

EXAMPLE USAGE IN A SCRIPT:

To typeset a LaTeX file "file.tex" and magnify from
12-point to 26-point, type a line such as the following
(after adjusting the documentclass it specifies, and
removing or commenting out the documentclass in file.tex):

latex "\\documentclass[12pt]{article}$(python latex-papersize.py 12 26 tex)\\input{file.tex}" && mv article.dvi file.dvi

Or in pdflatex (slightly more complex because we need to
put the settings just before \begin{document}):
cat file.tex | awk -- "/^ *\\\\begin *\\{document\\}[^#-~]*\$/ { print \"$(python latex-papersize.py 12 26 pdftex | sed -e 's/\\/\\\\/g')\" } { print }" > /tmp/tmp.tex
pdflatex "\documentclass[12pt]{article}\\input{/tmp/tmp.tex}" && mv article.pdf file.pdf

To run dvips on the .dvi file (not needed for pdflatex):

$(python latex-papersize.py 12 26 file.dvi)
"""

import os, sys, math
try: from commands import getoutput # Python 2
except: from subprocess import getoutput # Python 3
def hasKey(a,b):
  try: return a.has_key(b) # old Python 2
  except: return b in a # newer Python 2 + Python 3
if len(sys.argv)==2 and sys.argv[1]=="--help":
  print(__doc__.strip()); raise SystemExit
if len(sys.argv)==2 and sys.argv[1]=="--version":
  print(__doc__[:__doc__.find("\n\n")].strip()); raise SystemExit

base_pointsize = float(sys.argv[1])
desired_pointsize = float(sys.argv[2])

if len(sys.argv)>4:
  extra_bottom_margin_mm = float(sys.argv[4])*25.4/72
  pageStyle = ""
else:
  extra_bottom_margin_mm = 0
  pageStyle = " \\pagestyle{empty}"

if hasKey(os.environ,"paper_width"): paper_width=float(os.environ["paper_width"])
else: paper_width=210
if hasKey(os.environ,"paper_height"): paper_height=float(os.environ["paper_height"])
else: paper_height=297
if hasKey(os.environ,"margin_left"): margin_left=float(os.environ["margin_left"])
else: margin_left=10
if hasKey(os.environ,"margin_top"): margin_top=float(os.environ["margin_top"])
else: margin_top=10

paper_magstep = 1.0*desired_pointsize/base_pointsize
paperwidth = paper_width/paper_magstep
textwidth = (paper_width-2*margin_left)/paper_magstep
paperheight = paper_height/paper_magstep
textheight = (paper_height-2*margin_top)/paper_magstep-extra_bottom_margin_mm # note extra_bottom_margin_mm is NOT divided by paper_magstep because it corresponds to the height of the textual page-number, which will be magnified
margin_left_setting = margin_left/paper_magstep
margin_top_setting = margin_top/paper_magstep

if sys.argv[3]=="tex" or sys.argv[3]=="pdftex":
  s="\\textwidth=%.1fmm \\textheight=%.1fmm \\topmargin=%.1fmm \\marginparwidth=0mm \\oddsidemargin=%.1fmm \\evensidemargin=%.1fmm \\columnsep=%.1fmm%s" % (textwidth,textheight,margin_top_setting,margin_left_setting,margin_left_setting,margin_left_setting,pageStyle)
  if sys.argv[3]=="pdftex":
    s += "\\mag=%d \\pdfpagewidth=%d true mm \\pdfpageheight=%d true mm \\pdfhorigin=0 mm \\pdfvorigin=-12.95 mm \\paperwidth=%d true mm \\paperheight=%d true mm" % (1000*paper_magstep,paper_width,paper_height,paper_width,paper_height) # the -12.95mm seems to be a constant regardless of magnification (previous version had -14 but it sems -12.95 is more accurate - at least 12.9 is too small and 13 is too big).  Need \paperwidth and \paperheight in there as well in case using hyperref.
  print(s)
else:
  r = os.system("dvips -T %dmm,%dmm -x %d %s -o bbox_test.ps" % (paper_width*10,paper_height*10,1000*paper_magstep+0.5,sys.argv[3]))
  assert not r, "dvips failed"
  # Now, that would have got the origin wrong.  I can't
  # figure out how dvips origin and magstep is supposed to
  # interoperate, so let's work it out on a case-by-case
  # basis from the bounding box.
  # (Note: multiplying paper_width and paper_height by 10 above, because if dealing with very small paper sizes then this may give a reading of 0 if the origin is off the page.  Increasing the paper size doesn't seem to affect the origin.)
  bbox=getoutput("echo|gs -sDEVICE=bbox bbox_test.ps 2>&1|grep BoundingBox")
  # (previous version used 'head -1' to take only the first page, but that can cause 'broken pipe' errors if the file contains too many pages, and will give an incorrect result if there is only one line per page and it is indented on the first page, so we'll look at ALL the pages and take the outermost bounds.  Will also look at high-resolution bounding boxes only, if available.)
  if "HiResBoundingBox" in bbox: bbox=filter(lambda x:"HiRes" in x,bbox.split("\n"))
  else: bbox=bbox.split("\n")
  bbox = map(lambda x:tuple(map(lambda y:float(y),x.split(" ")[1:])), bbox)
  bbox = filter(lambda x: not x==(0,0,0,0), bbox)
  assert bbox, "Could not get a sensible bounding box from bbox_test.ps.  (If you're on Ubuntu, beware of bug #160203 in Ubuntu's ghostscript package.)"
  os.unlink("bbox_test.ps")
  existing_left_margin_mm = min(map(lambda x:x[0],bbox))*25.4/72
  existing_top_margin_mm = paper_height*10-max(map(lambda x:x[3],bbox))*25.4/72
  print("dvips -T %dmm,%dmm -O %.1fmm,%.1fmm -x %d %s" % (paper_width,paper_height,margin_left - existing_left_margin_mm,margin_top - existing_top_margin_mm,1000*paper_magstep+0.5,sys.argv[3]))
