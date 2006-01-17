$! Given a DVI file, this command file carries out some of the steps
$! needed to print '.dvi' files double sided.  The fancy pagination 
$! is done by Tom Rokicki's DVIDVI program.
$!
$! We assume that the following symbols have been defined:
$! doubside :== @disk$utils:[utilities.tex.dvidvi]doubside
$! dvidvi    :== $disk$utils:[utilities.tex.dvidvi]dvidvi
$!
$ if (p1 .nes. "") then goto file_given
$ type sys$input

Usage:  doubside file[.dvi]

$ exit
$
$ file_given:
$ on warning then exit
$ dvidvi -m 2:-1 'p1' part1               ! create part1.dvi
$ dvidvi -m 2:0  'p1' part2               ! create part2.dvi
$!
$! Now explain how to preview or PSPRINT the new DVI files.
$!
$ type sys$input

PART1.DVI and PART2.DVI have been created; each part contains the pages that
must be printed on the same side of the paper.  

To print the document double sided, carry out the following steps (assuming 
you are using a LaserWriter):

   1. $ psprint /nobanner /notify part1

   2. When this job has finished, take the output and put it back into
      the input cassette without changing the orientation in any way.

   3. $ psprint part2

   4. Depending on the number of pages, the final output may not be collated.
      You may need to change the placement of the bottom sheet(s).

$ exit

