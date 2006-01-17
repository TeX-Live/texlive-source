$! Given a DVI file, this command file carries out some of the steps
$! needed to produce an A5 booklet suitable for folding and stapling.
$! It is assumed the user has set up a page format suitable for A5 paper.
$! The fancy pagination is done by Tom Rokicki's DVIDVI program.
$! Note that 148mm = A5 width = half A4 height.
$!
$! We assume that the following symbols have been defined:
$! a5booklet :== @disk$utils:[utilities.tex.dvidvi]a5booklet
$! dvidvi    :== $disk$utils:[utilities.tex.dvidvi]dvidvi
$!
$ if (p1 .nes. "") then goto file_given
$ type sys$input

Usage:  a5booklet file[.dvi]

$ exit
$
$ file_given:
$ on warning then exit
$ dvidvi -m 4:-1,2(148mm,0mm) 'p1' part1               ! create part1.dvi
$ dvidvi -m 4:-3,0(148mm,0mm) 'p1' part2               ! create part2.dvi
$!
$! Now explain how to preview or PSPRINT the new DVI files.
$!
$ type sys$input

PART1.DVI and PART2.DVI have been created; each part contains the pages that
must be printed on the same side of the paper.  In both parts, each A4 page
contains two side-by-side A5 pages from your original document.

To preview these DVI files you need to tell DVItoVDU to display each page
in a landscaped orientation; for example:

      $ dvitovdu /xsize=11.7in /ysize=8.3in part1

To create an A5 booklet suitable for folding and stapling, carry out the
following steps (assuming you are using a LaserWriter):

   1. $ psprint /land /nobanner /notify part1

   2. When this job has finished, take the output and put it back into
      the input cassette without changing the orientation in any way.

   3. $ psprint /land part2

   4. Depending on the number of pages, the final output may not be collated.
      You may need to change the placement of the bottom sheet(s).

$ exit

