@PERL@
# fixscribeps: get conforming PostScript out of Scribe
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$inepsf = 0;
$epsfn = 0;
while (<>) {
   if (/^([0-9]+ [0-9]+ [0-9]+ PB) (%!.*)/) {
      print "$1\n%%BeginDocument: Scribe-EPSF $epsfn 0\n$2\n";
      $inepsf++;
   } elsif (/^ PE/ && $inepsf) {
      print "%%EndDocument\n", $_;
   } else {
      print $_;
   }
}
@END@
