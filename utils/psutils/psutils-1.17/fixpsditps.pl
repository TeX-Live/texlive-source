@PERL@
# fixpsditps: fix psdit output for use in psutils
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$nesting = 0;

while (<>) {
   if (/^\/p{pop showpage pagesave restore \/pagesave save def}def$/) {
      print "/p{pop showpage pagesave restore}def\n";
   } elsif (/^%%BeginDocument/ || /^%%BeginBinary/ || /^%%BeginFile/ ) {
      print $_;
      $nesting++;
   } elsif (/^%%EndDocument/ || /^%%EndBinary/ || /^%%EndFile/) {
      print $_;
      $nesting--;
   } elsif (/^%%Page:/ && $nesting == 0) {
      print $_;
      print "xi\n";
   } elsif (! /^xi$/) {
      print $_;
   }
}
@END@
