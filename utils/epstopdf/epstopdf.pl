eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;
use strict;

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
# History
#  1999/05/06 v2.5 (Heiko Oberdiek)
#    * New options: --hires, --exact, --filter, --help.
#    * Many cosmetics: title, usage, ...
#    * New code for debug, warning, error
#    * Detecting of cygwin perl
#    * Scanning for %%{Hires,Exact,}BoundingBox.
#    * Scanning only the header in order not to get a wrong
#      BoundingBox of an included file.
#    * (atend) supported.
#    * uses strict; (earlier error detecting).
#    * changed first comment from '%!PS' to '%!';
#    * corrected (atend) pattern: '\s*\(atend\)'
#    * using of $bbxpat in all BoundingBox cases,
#      correct the first white space to '...Box:\s*$bb...'
#    * corrected first line (one line instead of two before 'if 0;';
#  2000/11/05 v2.6 (Heiko Oberdiek)
#    * %%HiresBoundingBox corrected to %%HiResBoundingBox
#  2001/03/05 v2.7 (Heiko Oberdiek)
#    * Newline before grestore for the case that there is no
#      whitespace at the end of the eps file.
#  2002/02/18 v2.8draft (Gerben Wierda)
#    * Handle different eol styles transparantly
#    * Applied fix from Peder Axensten for Freehand bug
#  2002/02/21 v2.8draft (Gerben Wierda)
#    * Fixed bug where last line of buffer was not copied out (ugh!)
#  2003/04/22 v2.9draft (Gerben Wierda)
#    * Fixed bug where with cr-eol files everything up to the first %!
#    * in the first 2048 bytes was gobbled (double ugh!)
#  2004/03/17 v2.9.1draft (Gerben Wierda)
#    * No autorotate page
#  2005/09/29 v2.9.2draft (Gerben Wierda)
#    * Quote OutFilename
#  2005/10/01 v2.9.3draft (Gerben Wierda)
#    * Quote OutFilename
#

### program identification
my $program = "epstopdf";
my $filedate="2003/04/20";
my $fileversion="2.9.3draft";
my $copyright = "Copyright 1998-2002 by Sebastian Rahtz et al.";
my $title = "\U$program\E $fileversion, $filedate - $copyright\n";

### ghostscript command name
my $GS = "gs";
$GS = "gswin32c" if $^O eq 'MSWin32';
$GS = "gswin32c" if $^O =~ /cygwin/;

### options
$::opt_help=0;
$::opt_debug=0;
$::opt_compress=1;
$::opt_gs=1;
$::opt_hires=0;
$::opt_exact=0;
$::opt_filter=0;
$::opt_outfile="";

### usage
my @bool = ("false", "true");
my $usage = <<"END_OF_USAGE";
${title}Syntax:  $program [options] <eps file>
Options:
  --help:           print usage
  --outfile=<file>: write result to <file>
  --(no)filter:     read standard input   (default: $bool[$::opt_filter])
  --(no)gs:         run ghostscript       (default: $bool[$::opt_gs])
  --(no)compress:   use compression       (default: $bool[$::opt_compress])
  --(no)hires:      scan HiResBoundingBox (default: $bool[$::opt_hires])
  --(no)exact:      scan ExactBoundingBox (default: $bool[$::opt_exact])
  --(no)debug:      debug informations    (default: $bool[$::opt_debug])
Examples for producing 'test.pdf':
  * $program test.eps
  * produce postscript | $program --filter >test.pdf
  * produce postscript | $program -f -d -o=test.pdf
Example: look for HiResBoundingBox and produce corrected PostScript:
  * $program -d --nogs -hires test.ps>testcorr.ps
END_OF_USAGE

### process options
use Getopt::Long;
GetOptions (
  "help!",
  "debug!",
  "filter!",
  "compress!",
  "gs!",
  "hires!",
  "exact!",
  "outfile=s",
) or die $usage;

### help functions
sub debug {
  print STDERR "* @_\n" if $::opt_debug;
}
sub warning {
  print STDERR "==> Warning: @_!\n";
}
sub error {
  die "$title!!! Error: @_!\n";
}
sub errorUsage {
  die "$usage\n!!! Error: @_!\n";
}

### option help
die $usage if $::opt_help;

### get input filename
my $InputFilename = "";
if ($::opt_filter) {
  @ARGV == 0 or
    die errorUsage "Input file cannot be used with filter option";
  $InputFilename = "-";
  debug "Input file: standard input";
}
else {
  @ARGV > 0 or die errorUsage "Input filename missing";
  @ARGV < 2 or die errorUsage "Unknown option or too many input files";
  $InputFilename = $ARGV[0];
  -f $InputFilename or error "'$InputFilename' does not exist";
  debug "Input filename:", $InputFilename;
}

### option compress
my $GSOPTS = "";
$GSOPTS = "-dUseFlateCompression=false " unless $::opt_compress;

### option BoundingBox types
my $BBName = "%%BoundingBox:";
!($::opt_hires and $::opt_exact) or
  error "Options --hires and --exact cannot be used together";
$BBName = "%%HiResBoundingBox:" if $::opt_hires;
$BBName = "%%ExactBoundingBox:" if $::opt_exact;
debug "BoundingBox comment:", $BBName;

### option outfile
my $OutputFilename = $::opt_outfile;
if ($OutputFilename eq "") {
  if ($::opt_gs) {
    $OutputFilename = $InputFilename;
    if (!$::opt_filter) {
      $OutputFilename =~ s/\.[^\.]*$//;
      $OutputFilename .= ".pdf";
    }
  }
  else {
    $OutputFilename = "-"; # standard output
  }
}
if ($::opt_filter) {
  debug "Output file: standard output";
}
else {
  debug "Output filename:", $OutputFilename;
}

### option gs
if ($::opt_gs) {
  debug "Ghostscript command:", $GS;
  debug "Compression:", ($::opt_compress) ? "on" : "off";
}

### open input file
open(IN,"<$InputFilename") or error "Cannot open",
  ($::opt_filter) ? "standard input" : "'$InputFilename'";
binmode IN;

### open output file
if ($::opt_gs) {
  my $pipe = "$GS -q -sDEVICE=pdfwrite $GSOPTS -dAutoRotatePages=/None" .
          " -sOutputFile='$OutputFilename' - -c quit";
  debug "Ghostscript pipe:", $pipe;
  open(OUT,"|$pipe") or error "Cannot open Ghostscript for piped input";
}
else {
  open(OUT,">$OutputFilename") or error "Cannot write '$OutputFilename";
}

# reading a cr-eol file on a lf-eol system makes it impossible to parse
# the header and besides it will read the intire file into yor line by line
# scalar. this is also true the other way around.

### scan a block, try to determine eol style

my $buf;
my $buflen;
my @bufarray;
my @parsedbufarray; # for mytell/myseek
my $bufarraypos;

# We assume 2048 is big enough.
my $EOLSCANBUFSIZE = 2048;

$buflen = read( IN, $buf, $EOLSCANBUFSIZE);
if ($buflen > 0) {
  my $crlfpos;
  my $lfpos;
  my $crpos;

  # remove binary junk before header
  # if there is no header, we assume the file starts with ascii style and
  # we look for a eol style anyway, to prevent possible loading of the
  # entire file
  if ($buf =~ /%!/) {
    # throw away binary junk before %!
    $buf =~ s/(.*?)%!/%!/o;
  }
  $lfpos = index( $buf, "\n");
  $crpos = index( $buf, "\r");
  $crlfpos = index( $buf, "\r\n");

  if ($crpos > 0 and ($lfpos == -1 or $lfpos > $crpos+1)) {
    # The first eol was a cr and it was not immediately followed by a lf
    $/ = "\r";
    debug "The first eol character was a CR ($crpos) and not immediately followed by a LF ($lfpos)";
  }

  # Now we have set the correct eol-character. Get one more line and add
  # it to our buffer. This will make the buffer contain an entire line
  # at the end. Then split the buffer in an array. We will draw lines from
  # that array until it is empty, then move again back to <IN>
  $buf .= <IN> unless eof( IN);
  $buflen = length( $buf);

  # Some extra magic is needed here: if we set $/ to \r, Perl's re engine
  # still thinks eol is \n in regular expressions (not very nice) so we
  # cannot split on ^, but have to split on \r and reappend those.
  if ($/ eq "\r") {
    @bufarray = split( /\r/ms, $buf);
    grep( $_ .= "\r", @bufarray);
  }
  else {
    @bufarray = split( /^/ms, $buf);
  }
}

### getline
sub getline {
  if ($#bufarray >= 0) {
    $_ = shift( @bufarray);
    unshift( @parsedbufarray, $_); # for myseek and mytell
    $bufarraypos += length( $_);
  }
  else {
    $_ = <IN>;
  }
  return( defined( $_));
}

### mytell and myseek, work on <IN> only
sub mytell {
  if ($#bufarray) {
    return $bufarraypos;
  }
  else {
    return tell( IN);
  }
}

sub myseek {
  my $pos = shift;
  if ($pos < $buflen) {
    # We were still parsing the array, reset to the end of buf and
    # move to the right line in the array.
    # Now, move stuff from the @parsedbufarray until we are back at $pos
    my $tmpline;
    while ($pos > 0) {
      # we test on parsedbufarray to prevent an infinite loop on
      # a programming error (DEVELOP only)
      die "Programming error 1\n" unless $#parsedbufarray;
      $tmpline = pop( @parsedbufarray);
      $pos -= length( $tmpline);
      push( @bufarray, $tmpline);
    }
    return seek( IN, $buflen, 0);
  }
  else {
    return seek( IN, $pos, 0);
  }
}

### scan first line
my $header = 0;
getline();
if (/%!/) {
  # throw away binary junk before %!
  s/(.*)%!/%!/o;
}
$header = 1 if /^%/;
debug "Scanning header for BoundingBox";
print OUT;

### variables and pattern for BoundingBox search
my $bbxpatt = '[0-9eE\.\-]';
               # protect backslashes: "\\" gets '\'
my $BBValues = "\\s*($bbxpatt+)\\s+($bbxpatt+)\\s+($bbxpatt+)\\s+($bbxpatt+)";
my $BBCorrected = 0;

sub CorrectBoundingBox {
  my ($llx, $lly, $urx, $ury) = @_;
  debug "Old BoundingBox:", $llx, $lly, $urx, $ury;
  my ($width, $height) = ($urx - $llx, $ury - $lly);
  my ($xoffset, $yoffset) = (-$llx, -$lly);
  debug "New BoundingBox: 0 0", $width, $height;
  debug "Offset:", $xoffset, $yoffset;

  print OUT "%%BoundingBox: 0 0 $width $height\n";
  print OUT "<< /PageSize [$width $height] >> setpagedevice\n";
  print OUT "gsave $xoffset $yoffset translate\n";
}

### scan header
if ($header) {
  HEADER: while (getline()) {
    ### Fix for freehand bug ### by Peder Axensten
    next HEADER if(!/\S/);

    ### end of header
    if (!/^%/ or /^%%EndComments/) {
      print OUT;
      last;
    }

    ### BoundingBox with values
    if (/^$BBName$BBValues/) {
      CorrectBoundingBox $1, $2, $3, $4;
      $BBCorrected = 1;
      last;
    }

    ### BoundingBox with (atend)
    if (/^$BBName\s*\(atend\)/) {
      debug $BBName, "(atend)";
      if ($::opt_filter) {
        warning "Cannot look for BoundingBox in the trailer",
                "with option --filter";
        last;
      }
      my $pos = mytell();
      debug "Current file position:", $pos;

      # looking for %%BoundingBox
      while (getline()) {
        # skip over included documents
        if (/^%%BeginDocument/) {
          while (getline()) {
            last if /^%%EndDocument/;
          }
        }
        if (/^$BBName$BBValues/) {
          CorrectBoundingBox $1, $2, $3, $4;
          $BBCorrected = 1;
          last;
        }
      }

      # go back
      myseek( $pos) or error "Cannot go back to line '$BBName (atend)'";
      last;
    }

    # print header line
    print OUT;
  }
}

### print rest of file
while (getline()) {
  print OUT;
}

### close files
close(IN);
print OUT "\ngrestore\n" if $BBCorrected;
close(OUT);
warning "BoundingBox not found" unless $BBCorrected;
debug "Ready.";
;
