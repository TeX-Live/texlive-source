@PERL@
# fixwfwps: fix Word for windows PostScript for printing.
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$nesting = 0;
while (<>) {
   tr/\000-\010\012-\014\016-\037//d;
   foreach (grep($_ ne "", split("\015"))) {
      s!/SVDoc\s+save\s+def!!g;
      s!SVDoc\s+restore!!g;
      if (/^(%!PS-Adobe-\d*\.\d*) EPSF-/ && !$nesting) {
	 print "$1\n";
	 $wfwepsf = 1;
      } elsif (/^SS\s*$/ && $wfwepsf) {
	 print "%%Page: $wfwepsf $wfwepsf\n";
	 $wfwepsf++;
	 print "$_\n";
      } elsif (/^%MSEPS Preamble/) {
	 print "%%BeginDocument: (Included EPSF)\n";
	 print "$_\n";
	 $nesting++;
      } elsif (/^%MSEPS Trailer/) {
	 $nesting--;
	 print "$_\n";
	 print "%%EndDocument\n";
      } elsif (! /^%%BoundingBox/) {
	 print "$_\n";
      }
   }
}
@END@
