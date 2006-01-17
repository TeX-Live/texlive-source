@PERL@
# fixwwps: get semi-conforming PostScript out of Windows Write file
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$page = 1;

while (<>) {
   if (/^(%!.*) EPSF-\d.\d/) {
      print $1, "\n";
   } elsif (/^SS/) {
      print "%%Page: $page $page\n";
      print $_;
      $page++;
   } else {
      print $_;
   }
}
@END@
