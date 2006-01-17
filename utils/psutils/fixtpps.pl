@PERL@
# fixtpps: fix tpscript document to work with PSUtils
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$nesting = 0;
$header = 1;

while (<>) {
   if (/^%%Page:/ && $nesting == 0) {
      print $_;
      print "save home\n";
      $header = 0;
   } elsif (/^%%BeginDocument/ || /^%%BeginBinary/ || /^%%BeginFile/) {
      print $_;
      $nesting++;
   } elsif (/^%%EndDocument/ || /^%%EndBinary/ || /^%%EndFile/) {
      print $_;
      $nesting--;
   } elsif (/save home/) {
      s/save home//;
      print $_;
   } elsif (!$header || (! /^save$/ && ! /^home$/)) {
      print $_;
   }
}
@END@
