eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
        if 0;

# Change by Thomas Esser, Sept. 1998: The above lines allows us to find
# perl along $PATH rather than guessing a fixed location. The above
# construction should work with most shells.

# A script to transform an EPS file so that:
#   a) it is guarenteed to start at the 0,0 coordinate
#   b) it sets a page size exactly corresponding to the BoundingBox
# This means that when Ghostscript renders it, the result needs no
# cropping, and the PDF MediaBox is correct.
#   c) the result is piped to Ghostscript and a PDF version written
#
# It needs a Level 2 PS interpreter.
# If the bounding box is not right, of course, you have problems...
#
# The only thing I have not allowed for is the case of
# "%%BoundingBox: (atend)", which is more complicated.
#
# Sebastian Rahtz, for Elsevier Science
#
# now with extra tricks from Hans Hagen's texutil.
#
use Getopt::Long ;
$filedate="1998/10/22";
$fileversion="2.4";
if ($^O eq 'MSWin32')  { $GS="gswin32c";}
else                   { $GS="gs"; }
   
$opt_debug=0;
$opt_compress=1;
$opt_gs=1;
$opt_outfile="";
$result = GetOptions (
 "debug!",
 "compress!",
 "gs!",
 "outfile=s",
);
$Filename=$ARGV[0];
if (!$opt_compress)
  { $GSOPTS="-dUseFlateCompression=false " ;}
else 
  { $GSOPTS=""; }

open(TMP,"<$Filename") or  die "Usage: epstopdf <filename>";
binmode TMP;
$_=$Filename;
s/\.[A-Za-z]*$//;
if ($opt_gs) {
   open(OUT,"| $GS -q -sDEVICE=pdfwrite $GSOPTS -sOutputFile=$_.pdf - -c quit") 
   or  die "cannot open Ghostscript for piped input";
   if ($opt_debug) { print STDERR "read $Filename, write $_.pdf\n"; }
}
else
 { 
   if ($opt_outfile ne "")
     { open(OUT,">$opt_outfile") or die "cannot write $opt_outfile" ; 
      if ($opt_debug) { print STDERR "read $Filename, write $opt_outfile\n"; }
     }
   else { open(OUT,">&STDOUT"); }
}

$bbneeded=1;
local $bbpatt="[0-9\.\-]";
local $bbxpatt = "[0-9eE\.\-]";
while (<TMP>) { 
#
# throw away binary junk before %!PS
#
 if (/%!PS/)
          { s/(.*)%!PS/%!PS/o ;
            print OUT; }
#
# locate BoundingBox
#
 elsif ( /%%BoundingBox: (atend)/) {
	$bbneeded = 0;
        print OUT;
	}
 elsif (/%%BoundingBox:\s*($bbpatt+)\s+($bbpatt+)\s+($bbpatt+)\s+($bbpatt+)/ )
 { 
# only read the *first* bounding box
     if ($bbneeded) {
	 if ($opt_debug) { print STDERR "old BB is $1 $2 $3 $4\n";}
         
	 $width = $3 - $1;
	 $height = $4 - $2;
	 $xoffset = 0 - $1;
	 $yoffset = 0 - $2;
	 if ($opt_debug) {
	     print STDERR "new BB is 0 0 $width $height, offset $xoffset $yoffset\n";
	 }
	 print OUT "%%BoundingBox: 0 0 $width $height\n";
	 print OUT "<< /PageSize [$width $height] >> setpagedevice\n";
	 print OUT "gsave $xoffset $yoffset translate\n";
	 $bbneeded=0;
     }

# else ignore that embedded BoundingBox anyway
 }
  elsif (/%%(HiResB|ExactB)oundingBox:\s*($bbxpatt+)\s+($bbxpatt+)\s+($bbxpatt+)\s+($bbxpatt+)/ ) { 
     print STDERR "Unused hires/exact bounding box $_\n";
 }
else
 {    print OUT;  }
}
close(TMP);
print OUT "grestore\n";
close(OUT);
;
