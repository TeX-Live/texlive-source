@PERL@
# psmerge: merge PostScript files produced by same application and setup
# usage: psmerge [-oout.ps] [-thorough] file1.ps file2.ps ...
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$0 =~ s=.*/==;
$prog = $0;

while ($ARGV[0] =~ /^-/) {
   $_ = shift;
   if (/^-o(.+)/) {
      if (!close(STDOUT) || !open(STDOUT, ">$1")) {
	 print STDERR "$prog: can't open $1 for output\n";
	 exit 1;
      }
   } elsif (/^-t(horough)?$/) {
      $thorough = 1;
   } else {
      print STDERR "Usage: $prog [-oout] [-thorough] file...\n";
      exit 1;
   }
}

$page = 0;
$first = 1;
$nesting = 0;

@header = ();
$header = 1;

@trailer = ();
$trailer = 0;

@pages = ();
@body = ();

@resources = ();
$inresource = 0;

while (<>) {
   if (/^%%BeginFont:/ || /^%%BeginResource:/ || /^%%BeginProcSet:/) {
      $inresource = 1;
      push(@resources, $_);
   } elsif ($inresource) {
      push(@resources, $_);
      $inresource = 0 if /^%%EndFont/ || /^%%EndResource/ || /^%%EndProcSet/;
   } elsif (/^%%Page:/ && $nesting == 0) {
      $header = $trailer = 0;
      push(@pages, join("", @body)) if @body;
      $page++;
      @body = ("%%Page: ($page) $page\n");
   } elsif (/^%%Trailer/ && $nesting == 0) {
      push(@trailer, $_);
      push(@pages, join("", @body)) if @body;
      @body = ();
      $trailer = 1;
      $header = 0;
   } elsif ($header) {
      push(@trailer, $_);
      push(@pages, join("", @body)) if @body;
      @body = ();
      $trailer = 1;
      $header = 0;
   } elsif ($trailer) {
      if (/^%!/ || /%%EOF/) {
	 $trailer = $first = 0;
      } elsif ($first) {
	 push(@trailer, $_);
      }
   } elsif (/^%%BeginDocument/ || /^%%BeginBinary/ || /^%%BeginFile/) {
      push(@body, $_);
      $nesting++;
   } elsif (/^%%EndDocument/ || /^%%EndBinary/ || /^%%EndFile/) {
      push(@body, $_);
      $nesting--;
   } else {
      print $_ if $print;
   }
}

print @trailer;

exit 0;
@END@
