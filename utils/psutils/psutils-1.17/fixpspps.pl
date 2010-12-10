@PERL@
# mangle PostScript produced by PSPrint to make it almost conforming
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$header = 1; $ignore = 0;
$verbose = 0;
@body = ();
%fonts = (); $font = "";
$inchar = 0; @char = ();

while (<>) {
   if (/^\@end$/) {
      $ignore = 1;
   } elsif (/^[0-9]+ \@bop0$/) {
      $ignore = 0;
      $header = 1;
   } elsif ($header) {
      if (/^\/([a-z.0-9]+) \@newfont$/) {
	 if (! defined($fonts{$1})) {
	    $fonts{$1} = 1;
	    print;
	 } elsif ($verbose) {
	    print STDERR "$font already defined\n";
	 }
      } elsif (/^([a-z.0-9]+) sf$/) {
	 $font = $1;
	 print;
      } elsif (/^\[</) {
	 $inchar = 1;
	 push (@char, $_);
      } elsif ($inchar) {
	 push (@char, $_);
	 if (/.*\] ([0-9]+) dc$/) {
	    if (! defined($fonts{$font,$1})) {
	       $fonts{$font,$1} = 1;
	       print (@char);
	    } elsif ($verbose) {
	       print STDERR "$font character $1 already defined\n";
	    }
	    $inchar = 0;
	    @char = ();
	 }
      } elsif (/^([0-9]+) \@bop1$/) {
	 $header = 0;
	 push (@body, "%%Page: ? $1\n");
	 push (@body, $_);
      } else {
	 print;
      }
   } elsif (! $ignore) {
      push (@body, $_);
   }
}
print (@body);
print ("\@end\n");
@END@
