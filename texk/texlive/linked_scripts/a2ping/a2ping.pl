#! /bin/sh
eval '(exit $?0)' && eval 'PERL_BADLANG=x;export PERL_BADLANG;: \
;exec perl -x -- "$0" ${1+"$@"};#'if 0;
eval 'setenv PERL_BADLANG x;exec perl -x -- "$0" $argv:q;#'.q+
#!perl -w
package Htex::a2ping;  $0=~/(.*)/s;unshift@INC,'.';do($1);die$@if$@;__END__+if !1;
# Don't touch/remove any lines above; http://www.inf.bme.hu/~pts/justlib
#
# This program is free software, licensed under the GNU GPL, >=2.0.
# This software comes with absolutely NO WARRANTY. Use at your own risk!
#
# !! Ghostcript compute pipe too slow
#      $ a2ping.pl -v debrecen-hyph.ps debrecen-hyph.pdf
#      a2ping.pl ... -- Written by <pts@fazekas.hu> from April 2003.
#      This is free software, GNU GPL >=2.0. There is NO WARRANTY.
#      (epstopdf 2.7 Copyright 1998-2001 by Sebastian Rahtz et al.)
#      * Strongest BoundingBox comment: %%HiResBoundingBox:
#      * Doing --PaperSize unchanged
#      * Output filename: debrecen-hyph.pdf
#      * Output FileFormat: PDF
#      * Ghostscript ps2pdf command: gs -dSAFER
#      * Compression: zip
#      * Input filename: debrecen-hyph.ps
#      * Computing BBox info from non-EPS PS file
#      * Ghostscript compute pipe: gs -dSAFER -dWRITESYSTEMDICT -dNOPAUSE -sDEVICE=bbox -sFN=debrecen-hyph.ps /tmp/a2ping_pl-16977-298938572-c.tgs 2>&1
#      * Applying BoundingBox from Compute-GS T-: 71 81 539 769
#      * Applying HiResBoundingBox from Compute-GS T-H: 71.837998 81.971997 538.235984 768.113977
#      * Scanning header for BoundingBox
#      * Applying BoundingBox from ADSC T-: 0 0 596 842
#      * Final BoundingBox: 0 0 596 842
#      * Ghostscript ps2pdf pipe: gs -dSAFER -q -dBATCH -sDEVICE=pdfwrite  -sOutputFile=debrecen-hyph.pdf -
#      * Done OK, created PDF file debrecen-hyph.pdf (338451 bytes)
#
package just; BEGIN{$INC{'just.pm'}='just.pm'}
BEGIN{ $just::VERSION=2 }
sub end(){1}
sub main(){}

BEGIN{$ INC{'strict.pm'}='strict.pm'} {
package strict;
use just;
# by pts@fazekas.hu at Wed Jan 10 12:42:08 CET 2001
require 5.002;
sub bits {
  (grep{'refs'eq$_}@_ && 2)|
  (grep{'subs'eq$_}@_ && 0x200)|
  (grep{'vars'eq$_}@_ && 0x400)|
  ($@ || 0x602)
}
sub import { shift; $^H |= bits @_ }
sub unimport { shift; $^H &= ~ bits @_ }
just::end}

BEGIN{$ INC{'integer.pm'}='integer.pm'} {
package integer;
use just;
# by pts@fazekas.hu at Wed Jan 10 12:42:08 CET 2001
sub import   { $^H |= 1 }
sub unimport { $^H &= ~1 }
just::end}

BEGIN{$ INC{'Pts/string.pm'}='Pts/string.pm'} {
package Pts::string;
# by pts@fazekas.hu at Sat Dec 21 21:32:18 CET 2002
use just;
use integer;
use strict;

#** @param $_[0] a string
#** @param $_[1] index of first bit to return. Bit 128 of byte 0 is index 0.
#** @param $_[2] number of bits to return (<=32)
#** @return an integer (negative on overflow), bit at $_[1] is its MSB
sub get_bits_msb($$$) {
  # assume: use integer;
  my $loop=$_[1];
  my $count=$_[2];
  my $ret=0;
  ($ret+=$ret+(1&(vec($_[0],$loop>>3,8)>>(7-($loop&7)))), $loop++) while $count--!=0;
  $ret
}

#** @param $_[0] a string
#** @return value if $_[0] represents a floating point numeric constant
#**   in the C language (without the LU etc. modifiers) -- or undef. Returns
#**   undef for integer constants
sub c_floatval($) {
  my $S=$_[0];
  no integer; # very important; has local scope
  return 0.0+$S if $S=~/\A[+-]?(?:[0-9]*\.[0-9]+|[0-9]+\.])(?:[eE][+-]?[0-9]+)?\Z(?!\n)/;
  undef
}

#** @param $_[0] a string
#** @return value if $_[0] represents a floating point or integer numeric
#**   constant in the C language (without the LU etc. modifiers) -- or undef
sub c_numval($) {
  my $S=$_[0];
  no integer; # very important; has local scope
  return 0+$S if $S=~/\A[+-]?(?:[0-9]*\.[0-9]+(?:[eE][+-]?[0-9]+)?|[0-9]+\.?)\Z(?!\n)/;
  undef
}

#** @param $_[0] a string
#** @return the integer value of $_[0] in C -- or undef
sub c_intval($) {
  my $S=$_[0];
  my $neg=1;
  $neg=-1 if $S=~s@\A([+-])@@ and '-'eq$1;
  return $neg*hex $1 if $S=~/\A0[xX]([0-9a-fA-F]+)\Z(?!\n)/;
  return $neg*oct $1 if $S=~/\A0([0-7]+)\Z(?!\n)/;
  return $neg*$1     if $S=~/\A([0-9]+)\Z(?!\n)/;
  undef
}

sub import {
  no strict 'refs';
  my $package = (caller())[0];
  shift; # my package
  for my $p (@_ ? @_ : qw{get_bits_msb c_floatval c_numval c_intval}) { *{$package."::$p"}=\&{$p} }
}

just::end}

BEGIN{$ INC{'Htex/dimen.pm'}='Htex/dimen.pm'} {
package Htex::dimen;
# by pts@fazekas.hu at Sat Dec 21 21:26:15 CET 2002
use just;
use integer;
use strict;
use Pts::string qw(c_numval);

my %bp_mul;
{ no integer; %bp_mul=(
  'bp'=>1, # 1 bp = 1 bp (big point)
  'in'=>72, # 1 in = 72 bp (inch)
  'pt'=>72/72.27, # 1 pt = 72/72.27 bp (point)
  'pc'=>12*72/72.27, # 1 pc = 12*72/72.27 bp (pica)
  'dd'=>1238/1157*72/72.27, # 1 dd = 1238/1157*72/72.27 bp (didot point) [about 1.06601110141206 bp]
  'cc'=>12*1238/1157*72/72.27, # 1 cc = 12*1238/1157*72/72.27 bp (cicero)
  'sp'=>72/72.27/65536, # 1 sp = 72/72.27/65536 bp (scaled point)
  'cm'=>72/2.54, # 1 cm = 72/2.54 bp (centimeter)
  'mm'=>7.2/2.54, # 1 mm = 7.2/2.54 bp (millimeter)
) }

#** @param $_[0] a (real or integer) number, optionally postfixed by a
#**        TeX dimension specifier (default=bp)
#** @return the number in bp, or undef
sub dimen2bp($) {
  no integer;
  my $S=$_[0];
  my $mul;
  $mul=$bp_mul{$1} if $S=~s/\s*([a-z][a-z0-9]+)\Z(?!\n)// and exists $bp_mul{$1};
  my $val=c_numval($S);
  $val*=$mul if defined $val and defined $mul;
  $val
}

just::end}

BEGIN{$ INC{'Htex/papers.pm'}='Htex/papers.pm'} {
package Htex::papers;
# contains paper size information
# by pts@fazekas.hu at Sun Dec 22 00:30:58 CET 2002
use just;
use integer;
use strict;
use Htex::dimen;

my @papers=(
#
# paper.txt
# by pts@fazekas.hu at Tue Jan 16 18:21:59 CET 2001
# by pts@fazekas.hu at Tue Jan 16 19:13:16 CET 2001
#
# Examined: dvips, gs, libpaperg
#
# all units are measured in Big Points (bp)
# 72 bp == 1 in
# 2.54 cm == 1 in
#
# papername	width	height
qw{Comm10	297	684},
qw{Monarch	279	540},
qw{halfexecutive 378	522},

qw{Legal	612	1008},
qw{Statement	396	612},
qw{Tabloid	792	1224},
qw{Ledger	1224	792},
qw{Folio	612	936},
qw{Quarto	610	780},
qw{7x9		504	648},
qw{9x11		648	792},
qw{9x12		648	864},
qw{10x13	720	936},
qw{10x14	720	1008},
qw{Executive	540	720},

qw{ISOB0	2835	4008},
qw{ISOB1	2004	2835},
qw{ISOB2	1417	2004},
qw{ISOB3	1001	1417},
qw{ISOB4	 709	1001},
qw{ISOB5	 499	 709},
qw{ISOB6	 354	 499},
qw{ISOB7	 249	 354},
qw{ISOB8	 176	 249},
qw{ISOB9	 125	 176},
qw{ISOB10	 88	 125},
qw{jisb0	2916	4128},
qw{jisb1	2064	2916},
qw{jisb2	1458	2064},
qw{jisb3	1032	1458},
qw{jisb4	 729	1032},
qw{jisb5	 516	 729},
qw{jisb6	 363	 516},

qw{C7		230	323},
qw{DL		312	624},

qw{a3		842	1190},	# defined by Adobe
qw{a4		595	842},	# defined by Adobe; must precede a4small

# a4small should be a4 with an ImagingBBox of [25 25 570 817].},
qw{a4small	595	842},
qw{letter	612	792},	# must precede lettersmall
# lettersmall should be letter with an ImagingBBox of [25 25 587 767].
qw{lettersmall	612	792},
# note should be letter (or some other size) with the ImagingBBox
# shrunk by 25 units on all 4 sides.
qw{note		612	792},
qw{letterLand	792	612},
# End of Adobe-defined page sizes

qw{a0		2380	3368},
qw{a1		1684	2380},
qw{a2		1190	1684},
qw{a5		421	595},
qw{a6		297	421},
qw{a7		210	297},
qw{a8		148	210},
qw{a9		105	148},
qw{a10		74	105},
qw{b0		2836	4008},
qw{b1		2004	2836},
qw{b2		1418	2004},
qw{b3		1002	1418},
qw{b4		709	1002},
qw{b5		501	709}, # defined by Adobe

qw{a0Land	3368	2380},
qw{a1Land	2380	1684},
qw{a2Land	1684	1190},
qw{a3Land	1190	842},
qw{a4Land	842	595},
qw{a5Land	595	421},
qw{a6Land	421	297},
qw{a7Land	297	210},
qw{a8Land	210	148},
qw{a9Land	148	105},
qw{a10Land	105	74},
qw{b0Land	4008	2836},
qw{b1Land	2836	2004},
qw{b2Land	2004	1418},
qw{b3Land	1418	1002},
qw{b4Land	1002	709},
qw{b5Land	709	501},

qw{c0		2600	3677},
qw{c1		1837	2600},
qw{c2		1298	1837},
qw{c3		918	1298},
qw{c4		649	918},
qw{c5		459	649},
qw{c6		323	459},

# vvv U.S. CAD standard paper sizes
qw{archE	2592	3456},
qw{archD	1728	2592},
qw{archC	1296	1728},
qw{archB	864	1296},
qw{archA	648	864},

qw{flsa		612	936},	# U.S. foolscap
qw{flse		612	936},	# European foolscap
qw{halfletter	396	612},
qw{csheet	1224	1584},	# ANSI C 17x22
qw{dsheet	1584	2448},	# ANSI D 22x34
qw{esheet	2448	3168},	# ANSI E 34x44
qw{17x22	1224	1584},	# ANSI C 17x22
qw{22x34	1584	2448},	# ANSI D 22x34
qw{34x44	2448	3168},	# ANSI E 34x44
);

#** Converts a numeric paper size to a well-defined paper name. Tolerance is
#** 8.5bp
#** @param $_[0] width, in bp
#** @param $_[1] height, in bp
#** @return () or ("papername", ret.paper.width.bp, ret.paper.height.bp)
sub valid_bp($$;$$) {
  no integer;
  my ($W1,$H1)=(defined$_[2]?$_[2]:0,defined$_[3]?$_[3]:0);
  my ($WW,$HH)=(Htex::dimen::dimen2bp($_[0])-$W1, Htex::dimen::dimen2bp($_[1])-$H1);
  # Dat: 1mm == 720/254bp; 3mm =~ 8.5bp
  no integer;
  for (my $I=0; $I<@papers; $I+=3) {
    return @papers[$I,$I+1,$I+2] if abs($papers[$I+1]-$WW)<=8.5 and abs($papers[$I+2]-$HH)<=8.5;
  }
  ()
}

#** @param $_[0] (width width_unit "," height height_unit)
#** @return () or ("papername", width.bp, height.bp)
sub valid($) { # valid_papersize
  my $S=lc$_[0];
  $S=~/^\s*(\d+(\.\d+)?)\s*([a-z][a-z0-9]+)\s*,\s*(\d+(\.\d+)?)\s*([a-z][a-z0-9]+)\s*\Z(?!\n)/ ?
    valid_bp("$1$3","$4$6") : ();
}

#** @param $_[0] (width width_unit? ("," || "x") height height_unit?) || (papername)
#** @return () or ("papername"?, width.bp, height.bp)
sub any($) {
  my $S=lc$_[0];
  if ($S=~/\A[a-z]\w+\Z(?!\n)/) {
    for (my $I=0; $I<@papers; $I+=3) {
      return @papers[$I,$I+1,$I+2] if lc($papers[$I]) eq $S;
    }
  }
  return () if $S!~/^\s*(\d+(\.\d+)?)\s*((?:[a-z][a-z0-9]+)?)\s*[,xX]\s*(\d+(\.\d+)?)\s*((?:[a-z][a-z0-9]+)?)\s*\Z(?!\n)/;
  # ^^^ Dat: [xX] is xdvi-style, [,] is dvips-style spec
  my($w,$h)=($1.$3, $4.$6);
  my @L=valid_bp($w,$h);
  @L ? @L : (undef,Htex::dimen::dimen2bp($w),Htex::dimen::dimen2bp($h))
}

just::end}

BEGIN{$  INC{'Htex/a2ping.pm'}='Htex/a2ping.pm'}

package Htex::a2ping;
# a2ping.pl -- convert between PS, EPS and PDF and other page description formats
# by pts@fazekas.hu et al. at Wed Apr 16 14:54:13 CEST 2003
# a2ping.pa created at Sun Apr 20 22:25:47 2003
#
# This file contains perldoc(1) documentation. Search for `=head1'.
# See revision history at end of this file.
#

use just +1; # a JustLib application
use strict;
use integer;
use Htex::papers;
BEGIN { $Htex::a2ping::VERSION="2.84p" }

# Imp: option to ignore `%%Orientation: Portrait', which gs respects and rotates by 90 degrees if necessary
# Imp: gs(704?!) sometimes finds too small bbox, see Univers.eps
# Imp: respect bbox in METAPOST %! (not EPS), don't use Compute-GS T-
# Imp: -sPDFPassword=...
# Imp: `a2ping.pl -v jf.eps pdf1: t.pdf' PDF1: must be forced to have --below
# Imp: option to `clip' an EPS to the specified bbox -- does file size decrease?
# Imp: fix bug a2ping -v ~/a2ping_bug.ps a2ping_bug.pdf; running type1fix on
#      all fonts with dff.pl has fixed the problem
# Imp: post-process PNG etc. written by sam2p
# Imp: better help and docs
# Imp: respect full /MediaBox for a PDF -> EPS|PDF1 conversion
# Imp: --ll x,y command line option
# Imp: Htex/a2ping.pa -v ../image/tuzv.ps t.pdf  (1st and second page different)
# Imp: also save+restore /pdfmark ??
# Imp: fix /MediaBox an all PDF pages if !$ll_zero
# Imp: PDF -> PDF1 conversion with gs -sDEVICE=pdfwrite
# Imp: direct PDF to PCL5 conversion with gs
# Imp: remove %%BeginDefaults | %%PageMedia: plain | %%EndDefaults (pdftops(1))
# Imp: fix completely bogus margin and papersize handling:
#      ../justlib2/Htex/a2ping.pa --duplex=force-short -v -p:a3 -r force-unknown tuzv.ps t.pdf
# Imp: careful distinction between PDF and PDF1
# Imp: psnup support (-1 -2 -3 -4 ...)
# Imp: idempotent PS -> PS, add other header
# Imp: use convert(1) etc.
# Imp: possibly disable compute-pipe
# Imp: $header_remove_p ??
# Imp: --leftright option instead of --below
# Imp: pdfboxes.pl, get offset from gs
# Imp: detect error messages from GS, abort...
# Imp: use all pdftops + gs + acroread
# Imp: possibly accept /PageSize from %%DocumentMedia
# Imp: /DocumentMedia seems to screw up sub-pt placement in gv(1)
#
# Dat: example: a2ping.pl --extra=-c:ijg:50 -r86 nn1.eps nn1.jpg
# Dat: calling ``showpage'' is not required for -sDEVICE=pdfwrite with gs 6.50,
#      but -sDEVICE=pgmraw depends on it
# Dat: the functionality of pdfboxes.pl cannot be provided here with a shorter
#      implementation, because gs always outputs the content stream of the PDF
#      objects first
# Dat: pdftops -eps writes negative bbox correctly
# Dat: markedEPS: include pdfmarks
# Dat: gs 7.04 gdevdjet.[ch], gdevdljm.[ch]
# Dat: to be undeffed in setpagedevice: /.MarginsHWResolution /PageSize
#      /ImagingBBox /HWResolution /HWSize /.MediaSize (we undef all)

### program identification
my $program = "a2ping.pl";
my $filedate="2019-11-17";  # See also $Htex::a2ping::VERSION.
my $copyright = "Written by <pts\@fazekas.hu> from April 2003.
This is free software, GNU GPL >=2.0. There is NO WARRANTY.
(epstopdf 2.7 Copyright 1998-2001 by Sebastian Rahtz et al.)\n";
# "Contains modifications by pts\@fazekas.hu";
my $title = "$program $Htex::a2ping::VERSION, $filedate -- $copyright\n";

### ghostscript command name
my($quote,$GS)=("'","gs");
($quote,$GS) = ("\"","gswin32c") if $^O eq 'MSWin32' or $^O =~ /cygwin/i;

# --- help functions

sub fnq($) {
  my $fn=$_[0];
  return $fn if $fn!~y@-a-zA-Z0-9/.+_@@c;
  $fn=~s@'@\\'@g if $quote eq "'";
  $quote.$fn.$quote
}

sub debug {
  print STDERR "* @_\n" if $::opt_verbose;
}
sub warning {
  print STDERR "$0: warning: @_\n";
}
sub error {
  my $s=$title; $title="";
  die "$s$0: @_\n";
}

# unlink temporary files?
my $tmpunlink_p=1;
my $tmpsig=1;
my %tmpfiles;
my $tmpdir=exists $ENV{TMPDIR} ? $ENV{TMPDIR} : '/tmp';
$tmpdir="." if (!-d $tmpdir or !-w $tmpdir) and -w '.';
sub cleanup() {
  unlink keys %tmpfiles;
  exit 125;
}
END { unlink keys %tmpfiles; }
sub temp_unlink($) {
  if (defined $_[0] and exists $tmpfiles{$_[0]}) {
    unlink $_[0] if $tmpunlink_p;
    delete $tmpfiles{$_[0]};
  }
}
sub temp_prefix() {
  my $prog0=$program;
  $prog0=~y@a-zA-Z0-9@_@c;
  if ($tmpsig) {
    $tmpsig=0;
    $SIG{INT}=$SIG{TERM}=$SIG{HUP}=$SIG{QUIT}=\&cleanup;
  }
  return "$tmpdir/$prog0-$$-".int(rand(1<<30))."-"; # 30: nonnegative
}

#** @return arg rounded down to int
sub myfloor($) {
  # Dat: Perl int() rounds towards zero
  no integer;
  $_[0]==int($_[0]) ? $_[0] : $_[0] < 0 ? -int(1-$_[0]) : int($_[0])
}

#** @return arg rounded up to int
sub myceil($) {
  no integer; #### BUGFIX at Wed Nov 15 17:23:29 CET 2006
  $_[0]==int($_[0]) ? $_[0] : 1+ ($_[0] < 0 ? -int(-$_[0]) : int($_[0]));
}

just::main;

# ---

sub FL_PAGE1_STOP(){1} # is file format single-page?
sub FL_SET_PAGESIZE_OK(){2}
sub FL_PDFMARK(){4}
sub FL_NEED_SHOWPAGE(){8} # does gs -sDEVICE=... need showpage?
sub FL_SAMPLED(){16} # is it a sampled (raster, pixel-based)
sub FL_ANY_ORIGIN_OK(){32} # (llx,lly) may be anything, not just (0,0)
sub FL_HAS_ANTIALIAS(){64}
sub FL_VIA_SAM2P(){128} # sam2p(1) should convert PNM to such a format
sub FL_OK_SAM2P(){256} # sam2p(1) can convert PNM to such a format

my %fmts=( # Name=>[flags]
  'EPS'=>[FL_PAGE1_STOP],
  'markedEPS'=>[FL_PAGE1_STOP|FL_PDFMARK], # Imp: should we have FL_SET_PAGESIZE_OK?
  'PDF1'=>[FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_PDFMARK],
  'PDF'=>[FL_SET_PAGESIZE_OK|FL_PDFMARK|FL_ANY_ORIGIN_OK],
  'PS'=>[FL_SET_PAGESIZE_OK|FL_ANY_ORIGIN_OK],
  'markedPS'=>[FL_SET_PAGESIZE_OK|FL_PDFMARK|FL_ANY_ORIGIN_OK],
  'PCL5'=>[FL_SET_PAGESIZE_OK|FL_ANY_ORIGIN_OK],
  # ^^^ Dat: no FL_HAS_ANTIALIAS -- would need lj5gray, which is loonger
  # ^^^ Dat: no FL_SAMPLED, because cannot set resolution
  'PBM'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_OK_SAM2P], # Dat FL_HAS_ANTIALIAS produces obscure image
  'PGM'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_OK_SAM2P],
  'PPM'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_OK_SAM2P],
  # Now come the FileFormats supported via sam2p
  'PNG'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_VIA_SAM2P],
  'XWD'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_VIA_SAM2P],
  'BMP'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_VIA_SAM2P],
  'TIFF'=>[FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_VIA_SAM2P],
  'JPEG'=>[FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_VIA_SAM2P],
  'GIF'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_VIA_SAM2P], # Imp: disable antialias for few colors?
  'XPM'=> [FL_PAGE1_STOP|FL_SET_PAGESIZE_OK|FL_NEED_SHOWPAGE|FL_SAMPLED|FL_HAS_ANTIALIAS|FL_VIA_SAM2P], # Imp: disable antialias for few colors?
);
my %fmt_aliases=qw(MARKEDPS markedPS  MARKEDEPS markedEPS  PCL PCL5
  UNMARKEDPS PS  UNMARKEDEPS EPS  EPDF PDF1  MEPS markedEPS);
# Dat: .ps will be unmarked PS
# Imp: sometimes markedEPS for .eps?
my %fmt_exts=qw(eps EPS  epsi EPS  epsf EPS  eps2 EPS  ps PS  ps2  PS
  pcl PCL5  pcl5 PCL5  pbm PBM  pgm PGM  pnm PPM  ppm PPM  pdf PDF  png PNG
  xwd XWD  bmp  BMP  rle BMP  tif TIFF  tiff TIFF  jpg JPEG  jpe JPEG
  jpg JPEG  gif GIF  xpm XPM);

### usage
# vvv deprecated options:
#   --outfile=<file>: write result to <file>
#   --debug:          verbose debug informations       (default: $bool[$::opt_verbose])
#   --(no)filter:     d. read standard input           (default: false)
#   --(no)gs        d. run Ghostscript to create PDF
my $usage=
"${title}Usage:  $program [options] <inputfile> [[<outformat>:] <outputfile>]
Run with --doc to read documentation as a UNIX man(1) page.
Options: --help print this help message
--(no)compress  use compression                   (def: best)
--(no)hires     scan HiResBoundingBox             (def: yes)
--(no)exact     scan ExactBoundingBox             (def: no)
--(no)verbose   verbose debug informations        (def: no)
--(no)below     allow below+left_from baseline    (def: no)
--(no)tmpunlink unlink temporary files            (def: yes)
--(no)antialias render shades at outlines (def: scale3no) (=scale3yes =no =yes)
--(no)lossy     allow lossy image filters (EPS->PDF) (def: yes)
--(no)keepoldmediabox keep only old, [0 0]-based MediaBox in PDF (def: no)
--gs-cmd=       path to Ghostscript               (def: gs or gswin32c)
--gs-ccmd=      path to Ghostscript, 4 bbox calc  (def: gs or gswin32c)
--gsextra=      extra arg to gs
--extra=        extra arg to external prg (i.e pdftops)
--bboxfrom=     adsc|compute-gs|pagesize          (def: guess)
--papersize=    unchanged|force-unknown|600bpx5cm (def: default) (bp)
--threshold=    min color for 1 in 8->1 bit conv  (def: 128)
Possible input formats: PS EPS PDF JPEG GIF TIFF PNG PNM PCX BMP LBM XPM TGA
Possible output formats: @{[sort keys %fmts]}
Examples for producing 'test.pdf':
  * $program test.eps
  * produce postscript | $program -v - test.pdf
Example: look for HiResBoundingBox and produce corrected PostScript:
  * $program -d --nogs -hires test.ps>testcorr.ps
";
sub errorUsage {
  die "$usage\U!\E!\U!\E Error: @_\n";
}

# --- @ARGV parsing

### default option values
my @extra=();
my @gsextra=();
#** Output file format (string)
my $FileFormat=undef;
$::opt_help=0;
$::opt_verbose=0;
my %vals_compression=map{$_=>1} qw(best none flate zip);
$::opt_compression='best';
#** Prefer %%HiResBoundingBox over %%BoundingBox, but emit both
$::opt_hires=1;
$::opt_exact=0;
# $::opt_filter=0; # deprecated
# $::opt_outputfile=undef; # deprecated
$::opt_below=undef;
$::opt_keepoldmediabox=0;
$::opt_lossy=1;
$::opt_antialias=undef; # render shades at path outlines for better readability
$::opt_gs_cmd=undef;
$::opt_extra="";
$::opt_duplex="default";
$::opt_threshold=128;
my %vals_antialias=map{$_=>1} qw(no yes scale3yes scale3no);
my %vals_duplex=map{$_=>1} qw(force-unknown force-simplex force-long
  force-short unchanged default-simplex default-long default-short);
# ^^^ short: duplex printing, will bind short edge of paper (ideal for
#     duplexing psnup -2)
#** Dat: force-unknown is forced by /setpagedevice/load def
$::opt_resolution="default"; # unchanged force-unknown 600x600 (DPI)
$::opt_papersize="default"; # unchanged force-unknown 600bpx600cm (bp)
#** --bboxfrom=adsc sets %%BoundingBox from the 1st page if no ADSC comment in non-EPS ps
my %vals_bboxfrom=map{$_=>1} qw(adsc compute-gs guess pagesize);
$::opt_bboxfrom="guess";
my $InputFilename;
my $OutputFilename;

sub is_page1_stop() { 0!=($fmts{$FileFormat}[0]&FL_PAGE1_STOP) }
sub is_set_pagesize_ok() { 0!=($fmts{$FileFormat}[0]&FL_SET_PAGESIZE_OK) }
sub is_pdfmark() { 0!=($fmts{$FileFormat}[0]&FL_PDFMARK) }
sub is_need_showpage() { 0!=($fmts{$FileFormat}[0]&FL_NEED_SHOWPAGE) }
sub is_sampled() { 0!=($fmts{$FileFormat}[0]&FL_SAMPLED) }
sub is_any_origin_ok() { 0!=($fmts{$FileFormat}[0]&FL_ANY_ORIGIN_OK) }
sub is_has_antialias() { 0!=($fmts{$FileFormat}[0]&FL_HAS_ANTIALIAS) }
sub is_via_sam2p() { 0!=($fmts{$FileFormat}[0]&FL_VIA_SAM2P) }
sub is_ok_sam2p() { 0!=($fmts{$FileFormat}[0]&FL_OK_SAM2P) }

# ---

sub show_doc() {
  # run `pod2man __FILE__ | man -l -', same as `perldoc __FILE', but perldoc(1)
  # is missing from some Debian sites that have pod2man.
  my @path=split/:+/,$ENV{PATH};
  my $pod2man_='pod2man --center="a2ping: advanced PS, PDF, EPS converter" ';
  $pod2man_=q~perl -ne 'if($a>1){print}elsif($a&&/^=head1/){$a=2}else{$a=!/\S/}' ~
    if !grep { -x "$_/pod2man" } @path;
  my $pager='';
  for my $pageri ((defined $ENV{PAGER} ? $ENV{PAGER}: ''),'less','most','more','view -  ','vim -R -  ','vi -  ','joe -rdonly -asis -','pager') {
    next if $pageri!~/^(\S+)/;
    my $pagert="/$1";
    if (grep { -x $_.$pagert } @path) { $pager=$pageri; last }
  }
  $pager=q~perl -pe 's@\010_@@g;s@.\010@@gs' | ~.$pager
    if substr($pager,-2)eq'  ' or $pager=~/\A(?:view|vim?|joe|emacs|mcedit|nano|nano-tiny|ae)\b/;
    # ^^^ Dat: these cannot handle underline/bold backspace directly
  $ENV{PAGER}=$pager;
  my $man='';
  if (substr($pod2man_,0,5)ne 'perl ') {
    $man=' | man -l -'; # calls $PAGER
    if ((!grep { -x "$_/man" } @path) or qx(man -l 2>&1)=~/\binvalid option\b/) {
      $man=' | nroff -Tlatin1 -mandoc'; # Linux, no need for eqn(1), tbl(1) etc.
      if (!grep { -x "$_/nroff" } @path) { $man='' } # just write it
    }
  }
  my $cmd=$pod2man_.fnq(__FILE__).$man;
  if ($cmd!~/[|] man -l -\Z(?!\n)/) {
    if (!length $pager) {
      die unless open PIPE, "$cmd|";
      print while sysread PIPE, $_, 4096;
      die "$0: error showing doc\n" unless close PIPE;
      exit 0;
    }
    $cmd.=' | $PAGER';
  }
  ##die $cmd;
  $ENV{LESS}="" if !defined $ENV{LESS};
  $ENV{LESS}="$ENV{LESS}R"; # show ANSI escapes
  die "$0: exec ($cmd) failed: $!\n" if !exec $cmd;
}

die $usage if !@ARGV or (1==@ARGV and $ARGV[0] eq '-h' or $ARGV[0] eq '--help'
  or $ARGV[0] eq 'help');
show_doc() if 1==@ARGV and $ARGV[0] eq '--doc' or $ARGV[0] eq 'doc';

{ my($I,$optname,$optval);
  my %optmap=qw(o outputfile  outfile outputfile  r resolution  h help
    f filter  d verbose  v verbose  debug verbose  p papersize
    c compression  compress compression  h hires  b below  e exact  x extra);
  #** Options that have a mandatory argument
  my %argopt1=qw(outputfile 1 duplex 1 resolution 1 extra 1 compression 1 gs-cmd 1
    gs-ccmd 1
    papersize 1 paper 1 bboxfrom 1 antialias 1 gsextra 1 threshold 1); # 1 arg
  my %argnone=qw(help 1 verbose 1 noverbose 1 nocompress 1 noantialias 1); # 0 arg
  my %argmaybe=qw();  # 0 or 1 arg
  my %argbool=qw(hires 1 exact 1 below 1 gs 1 filter 1 tmpunlink 1
    approx 1 lossy 1 keepoldmediabox 1); # boolean arg
  # Dat: --noverbose --nocompress
  my $opts_ok=1;
  for ($I=0; $I<@ARGV; $I++) {
    if ($ARGV[$I]eq '--') {
      $OutputFilename=$InputFilename if defined $InputFilename and
        !defined $OutputFilename and $opts_ok;
      $opts_ok=0;
    } elsif ($opts_ok and $ARGV[$I]=~/\A--+(\w[\w-]*)(?:[:=](.*))?\Z(?!\n)/s) {
      $optname=lc$1; $optval=$2;
    } elsif ($opts_ok and $ARGV[$I]=~/\A-(\w)(.*)\Z(?!\n)/s) {
      $optname=lc$1;
      if (length($2)==0) { $optval=undef }
      elsif (index(":=",substr($2,0,1))>=0) { $optval=substr($2,1) }
      else { $optval=$2 }
    } elsif ($opts_ok and !defined $FileFormat and defined $InputFilename and $ARGV[$I]=~s@\A(\w+):@@) {
      my $fmtag=uc$1;
      # errorUsage "invalid FileFormat tag: $fmtag" if $fmtag!~s@:\Z(?!\n)@@;
      if (exists $fmts{$fmtag}) { $FileFormat=$fmtag }
      elsif (exists $fmt_aliases{$fmtag}) { $FileFormat=$fmt_aliases{$fmtag} }
      else { errorUsage "Unknown FileFormat tag: $fmtag" }
      if (0!=length($ARGV[$I])) {
        errorUsage "Multiple output filenames" if defined $OutputFilename;
        $OutputFilename=$ARGV[$I];
      }
      next
    } elsif (!defined $InputFilename) { $InputFilename=$ARGV[$I]; next }
    elsif (!defined $OutputFilename) { $OutputFilename=$ARGV[$I]; next }
    else { errorUsage "Too many arguments (multiple input/output files?)" }
    $optname=$optmap{$optname} if exists $optmap{$optname};
    if (exists $argopt1{$optname} and !defined $optval) {
      errorUsage "Argument expected for --$optname" if $I==@ARGV;
      $optval=$ARGV[++$I];
    }
    # Dat: $optname and $optval are now correct
    errorUsage "No argument expected for --$optname=$optval" if exists $argnone{$optname} and defined $optval;
    if (substr($optname,0,2)eq"no" and exists $argbool{substr($optname,2)}) {
      $optname=substr($optname,2);
      errorUsage "No argument expected for no --no$optname=$optval" if defined $optval;
      $optval="no";
    }
    if (exists $argbool{$optname}) {
      # same as sam2p GenBuffer::parseBool, understands:
      #   on  true  yes ja   igen oui enable  1 true  vrai? right sure allowed
      #   off false no  nein nem  non disable 0 false faux? wrong nope disallowed
      $optval=(!defined($optval) or 0==length($optval)
               or $optval=~/\Ao[nu]/i or $optval!~/\A[fndw0]/i) ? 1 : 0;
    } elsif (!exists $argopt1{$optname} and !exists $argnone{$optname} and !exists $argmaybe{$optname}) {
      errorUsage "Unknown option --$optname, see --help"
    }
    # vvv application-specific
       if ($optname eq "help") { die $usage }
    elsif ($optname eq "help") { show_doc() }
    elsif ($optname eq "noverbose") { $::opt_verbose=0 }
    elsif ($optname eq "nocompress") { $::opt_compression='none' }
    elsif ($optname eq "verbose") { $::opt_verbose++ }
    elsif ($optname eq "hires")  { $::opt_hires =$optval }
    elsif ($optname eq "exact")  { $::opt_exact =$optval }
    elsif ($optname eq "below")  { $::opt_below =$optval }
    elsif ($optname eq "keepoldmediabox")  { $::opt_keepoldmediabox=$optval }
    elsif ($optname eq "lossy")  { $::opt_lossy =$optval }
    elsif ($optname eq "approx") { $::opt_approx=$optval }
    elsif ($optname eq "threshold")  { $::opt_threshold=$optval+0 } # Imp: accept only int 0..256
    elsif ($optname eq "filter") {
      # errorUsage "Multiple input filenames" if defined $InputFilename;
      # $InputFilename='-';
      errorUsage "Multiple output filenames" if defined $OutputFilename;
      $OutputFilename='-';
    } elsif ($optname eq "tmpunlink") { $tmpunlink_p=$optval }
    elsif ($optname eq "gs")     { $FileFormat=$optval ? 'PDF1' : 'markedEPS' }
    elsif ($optname eq "compression") {
      errorUsage "--$optname expects one of: @{[keys%vals_compression]}" if !exists $vals_compression{$optval};
      $::opt_compression=$optval;
      $::opt_compression='zip' if $::opt_compression eq 'flate';
    } elsif ($optname eq "outputfile") {
      errorUsage "Multiple output filenames" if defined $OutputFilename;
      $OutputFilename=$optval;
    } elsif ($optname eq "gs-cmd") {
      errorUsage "Multiple --gs-cmd" if defined $::opt_gs_cmd;
      $::opt_gs_cmd=$optval;
    } elsif ($optname eq "gs-ccmd") {
      errorUsage "Multiple --gs-ccmd" if defined $::opt_gs_ccmd;
      $::opt_gs_ccmd=$optval;
    } elsif ($optname eq "extra") { push @extra, $optval }
    elsif ($optname eq "gsextra") { push @gsextra, $optval }
    elsif ($optname eq "duplex") {
      errorUsage "--$optname expects one of: @{[keys%vals_duplex]}" if !exists $vals_duplex{$optval};
      $::opt_duplex=$optval
    } elsif ($optname eq "bboxfrom") {
      errorUsage "--$optname expects one of: @{[keys%vals_bboxfrom]}" if !exists $vals_bboxfrom{$optval};
      $::opt_bboxfrom=$optval
    } elsif ($optname eq "noantialias") {
      $::opt_antialias='no'
    } elsif ($optname eq "antialias") {
      errorUsage "--$optname expects one of: @{[keys%vals_antialias]}" if !exists $vals_antialias{$optval};
      $::opt_antialias=$optval
    } elsif ($optname eq "resolution") {
      if ($optval eq "unchanged" or $optval eq "force-unknown") { }
      elsif ($optval=~/^(\d+(?:[.]\d+)?)\Z(?!\n)/) { $optval="$1x$1" }
      elsif ($optval=~/^(\d+(?:[.]\d+)?[x,]\d+(?:[.]\d+)?)\Z(?!\n)/) { }
      else { errorUsage "--Resultion expects unchanged | force-unknown | DPI | XDPIxYDPI" }
      $::opt_resolution=$optval
    } elsif ($optname eq "papersize" or $optname eq"paper") {
      if ($optval eq "unchanged" or $optval eq "force-unknown") { $::opt_papersize=$optval }
      else {
        my @L=Htex::papers::any($optval);
        errorUsage "invalid or unknown for --papersize" if !@L;
        $::opt_papersize="$L[1],$L[2]" # width, height
      }
    } else { die } # unreachable
  } # NEXT opt
  errorUsage "Too many arguments (multiple input/output files?)" if $I!=@ARGV;
  # splice @ARGV, 0, $I;
}

$GS=$::opt_gs_cmd if defined $::opt_gs_cmd;
my $CGS=$GS;
$CGS=$::opt_gs_ccmd if defined $::opt_gs_ccmd;
# vvv SUXX: (r) file doesn't work with gs 8.5x -DSAFER
# -dNOSAFER to override SAFER activated by default since gs 9.50
$GS.= " -dNOSAFER"; # -dWRITESYSTEMDICT
$CGS.=" -dNOSAFER"; # -dWRITESYSTEMDICT

### get input and output filename
if (!defined $InputFilename and defined $OutputFilename) { # --filter
  $InputFilename='-';
} elsif (!defined $InputFilename) {
  errorUsage "Input filename missing"
} elsif (!defined $OutputFilename) {
  $FileFormat='PDF1' if !defined $FileFormat;
  if ($FileFormat eq 'PDF1' or $FileFormat eq 'PDF') {
    if (($OutputFilename=$InputFilename) ne '-') {
      $OutputFilename =~ s/\.[^\.]*$//;
      $OutputFilename .= ".pdf";
    }
  } else {
    $OutputFilename = '-'; # standard output
  }
}
print STDERR $title if $::opt_verbose;
$title="";

# Dat: no more @ARGV
errorUsage "please specify <outformat>" if
  !defined $FileFormat and ($OutputFilename!~m@[.]([^/.]+)\Z(?!\n)@ or
  !defined($FileFormat=$fmt_exts{lc$1}));
$::opt_below=is_any_origin_ok() if !defined $::opt_below;
error "--below=1 invalid for FileFormat $FileFormat" if $::opt_below and
  !is_any_origin_ok() and $FileFormat ne 'PDF1' and $FileFormat ne 'EPS' and
  $FileFormat ne 'markedEPS';
error "--below=0 invalid for FileFormat $FileFormat" if !$::opt_below and
  is_any_origin_ok();
$::opt_antialias=is_has_antialias() ?
  (is_sampled() ? 'scale3no' : 'yes') : 'no' if
  !defined $::opt_antialias;

if ($FileFormat eq 'PBM' and ($::opt_antialias eq 'scale3yes' or
  $::opt_antialias eq 'scale3no')) {
} elsif ($::opt_antialias ne 'no' and !is_has_antialias()) {
  $::opt_antialias='no';
  warning "--AntiAlias ignored for FileFormat $FileFormat"
}
if ($::opt_antialias eq 'scale3no' or $::opt_antialias eq 'scale3yes') {
  $::opt_resolution="72,72" if $::opt_resolution eq 'unchanged' or $::opt_resolution eq 'force-unknown' or $::opt_resolution eq 'default';
  # ^^^ GS raster default
  my @L=split/[,x]/,$::opt_resolution;
  @L=(@L,@L); # Imp: ..
  $L[0]*=3; $L[1]*=3;
  $::opt_resolution="$L[0],$L[0]";
}

### option compress
my $GSOPTS=join("  ",map{fnq$_}@gsextra);
# $GSOPTS.=" -r72 -sPAPERSIZE=a4 "; # default -- will be overridden by `setpagedevice'
# ^^^ Dat: default does only harm; user should specify on command line

### option BoundingBox types
#**** pts ****
# scan all of them and find the best
{ my $BBprint = "%%BoundingBox:";
  $BBprint = "%%HiResBoundingBox:" if $::opt_hires;
  $BBprint = "%%ExactBoundingBox:" if $::opt_exact;
  debug "Strongest BoundingBox comment:", $BBprint;
}
my $BBregex='%%(Hi[Rr]es|Exact|)BoundingBox:';

if (!is_set_pagesize_ok()) {
  if ($::opt_papersize ne'default' and $::opt_papersize ne'force-unknown') {
    error "Cannot set --PaperSize for FileFormat $FileFormat"
  } else { $::opt_papersize='force-unknown' }
} elsif ($::opt_papersize eq'default') { $::opt_papersize='unchanged' }

if ($::opt_resolution eq'default') { $::opt_resolution='force-unknown' }
error "Cannot set --Resolution for FileFormat $FileFormat (must be markedPS or sampled)"
  if $FileFormat ne 'markedPS' and !is_sampled()
  and $::opt_resolution ne'force-unknown';
error "Bad --Resolution=$::opt_resolution" if $::opt_resolution ne 'unchanged'
  and $::opt_resolution ne 'force-unknown' and $::opt_resolution!~/\A(\d+)+[,x](\d+)\Z(?!\n)/;

if ($FileFormat ne 'markedPS' and $FileFormat ne 'PCL5') {
  if ($::opt_duplex ne'default' and $::opt_duplex ne'force-unknown') {
    error "Cannot set --Duplex for FileFormat $FileFormat (must be markedPS or PCL5)"
  } else { $::opt_duplex='force-unknown' }
} elsif ($::opt_duplex eq'default') { $::opt_duplex='force-unknown' }

debug "Doing --PaperSize $::opt_papersize" if $::opt_papersize ne 'force-unknown';
debug "Doing --Duplex $::opt_duplex" if $::opt_duplex ne 'force-unknown';
debug "Doing --Resolution $::opt_resolution" if $::opt_resolution ne 'force-unknown';
debug "Doing --AntiAlias=$::opt_antialias" if $::opt_antialias ne 'no';

### option outfile
if ($OutputFilename eq '-') {
  debug "Output file: standard output";
} else {
  debug "Output filename: $OutputFilename";
  #error "$OutputFilename: won't overwrite input file with itself"
  #  if $OutputFilename eq $InputFilename;
}

### option gs
debug "Output FileFormat: $FileFormat";
$::opt_compression='zip' if $::opt_compression ne 'none';
if ($FileFormat eq 'PDF' or $FileFormat eq 'PDF1') {
  debug "Ghostscript ps2pdf command: $GS  $GSOPTS";
  debug "Compression: $::opt_compression";
} elsif ($FileFormat eq 'PCL5') {
  debug "Ghostscript ps2ljet command: $GS  $GSOPTS";
} elsif (is_sampled()) {
  debug "Ghostscript ps2sampled command: $GS  $GSOPTS";
}

#**** pts ****
sub read_error() { error "read $InputFilename: $!" }
my $in_mac_p=0; # 0: "\n" or "\r\n" is line terminator; 1: "\r" is line terminator
my $bytes_left=-1; # -1==unlimited
my $already_read=0;
sub dem1($){defined$_[0]?$_[0]:-1}
#** @param $_[0] number of bytes to read, or undef to read a line
#** @return the string read
sub readIN(;$) {
  my $S;
  ## return "" if $bytes_left==0;
  ## print STDERR "READ($_[0])\n";
  if (defined $_[0]) { read_error if 0>dem1 read IN, $S, $_[0] }
  else {
    $!=0; # clean up error code
    if ($in_mac_p) {
      local $/="\r";
      $S=~s@\r\Z(?!\n)@\n@ if defined($S=<IN>);
    } else { $S=<IN> }
    read_error if !defined($S) and $!;
    $S="" if !defined $S; # EOF
  }
  if ($bytes_left<0) { # unlimited
  } elsif (length($S)>=$bytes_left) {
    $S=substr($S, 0, $bytes_left);
    $bytes_left=0;
  } else { $bytes_left-=length($S) }
  $already_read+=length($S);
  $S
}

sub open_OUT() {
  error "Cannot write outfile '$OutputFilename'" unless
    open(OUT, $OutputFilename eq '-' ? ">-" : "> $OutputFilename")
}

#** @param $_[0] temp file extension (e.g ".img")
#** @param $_[1] preprint
#** @param $_[2] bool: force pipe even if seekable?
sub fix_pipe_in($$$) {
  my $c="";
  if ($_[2] or (length($c=readIN(1))!=0 and !seek IN,-1,1)) { # we cannot seek back
    # Dat: ^^^ test seekability instead of $InputFilename eq '-'
    my($ext,$preprint)=@_;
    my $tifn;
    # $ext=$1 if $InputFilename=~/[.](\w+)\Z(?!\n)/; # never true
    $tifn=temp_prefix()."M$ext";
    error "Cannot open temp input: $tifn" unless open TI, "> $tifn";
    $tmpfiles{$tifn}=1;
    die unless print TI $preprint, $c;
    print TI or die while length($_=readIN 4096);
    die unless close TI;
    $InputFilename=$tifn;
    debug "Temp input file: $InputFilename";
    die unless open IN, "< $tifn";
    die unless seek IN, length($preprint), 0;
    $already_read=length($preprint);
    $bytes_left=-1; # unlimited, since readIN() has copied only part
    # $bytes_left++ if $bytes_left>=0; # ungetc($c)
    # temp_unlink $tifn; # do it later (at END{})
  } else {
    $already_read--; $bytes_left++ if $bytes_left>=0; # BUGFIX at Fri May 14 00:21:18 CEST 2004
  }
}

my $temp_out_fn;

#** Does overwrite $temp_out_fn. Fine.
sub fix_force_out($) {
  my($ext)=$_[0];
  # $ext=$1 if $InputFilename=~/[.](\w+)\Z(?!\n)/; # never true
  $temp_out_fn=temp_prefix()."O$ext";
  # error "Cannot save output: $!" unless open SAVEOUT, ">&OUT"; # always STDOUT; maybe not open yet
  error "Cannot open temp output: $temp_out_fn" unless open OUT, "> $temp_out_fn";
  $tmpfiles{$temp_out_fn}=1;
  # $OutputFilename=$temp_out_fn;
  debug "Temp output file: $temp_out_fn";
  # temp_unlink $temp_out_fn; # do it later (at END{})
  return $temp_out_fn;
}

#** @param $_[0] temp file extension (e.g ".img")
#** @return new output filename
sub fix_pipe_out($) {
  if (!defined $temp_out_fn) {
    return $OutputFilename if $OutputFilename ne '-';
    return fix_force_out($_[0]);
  }
  $temp_out_fn
}
sub fix_close_out() {
  # error "closing filter out: $? $!" unless close OUT;
  if (defined $temp_out_fn) {
    my $buf;
    die unless open FCO, "< $temp_out_fn";
    print STDOUT $buf while sysread FCO, $buf, 4096;
    die unless close FCO;
    temp_unlink $temp_out_fn;
    undef $temp_out_fn;
  }
}

sub do_system {
  my($progname)=splice@_,0,1;
  debug "Running: $progname  @extra @_";
  error "prog $progname failed: $? $!"
    if 0!=system $progname, @extra, @_; # Dat: non-zero exit() or not found
}

sub do_exec {
  my($progname)=splice@_,0,1;
  if (scalar keys %tmpfiles) {
    # Cannot exec() right now, because we have to unlink some temporary files
    # later.
    do_system $progname, @_;
  } else {
    debug "Execing: $progname  @extra @_";
    1 if exec $progname, @extra, @_;
    # ^^^ Dat: $OutputFilename eq '-' should be OK
    error "exec failed: $!";
  }
  exit(0);
}

#sub shq($) {
#  my $S=$_[0];
#  return $S if $S!~y@A-Za-z0-9_,:./-@@c and length($S)>0;
#  $S=~s@'@'\\''@g;
#  return "'$S'"
#}

### open input file
if ($InputFilename eq '-') {
  debug "Input file: standard input";
  open(IN, "<&STDIN") or error "cannot open standard input";
} else {
  # -f $InputFilename or error "input file missing: $InputFilename"; # Imp: named pipe?
  open(IN,"< $InputFilename") or error "cannot open input file: $InputFilename";
  debug "Input filename:", $InputFilename;
  if ($InputFilename eq $OutputFilename) {
    # error "same input and output file: $InputFilename";
    my $ext=$InputFilename=~m@([.][^./]+)\Z(?!\n)@ ? $1 : "";
    binmode IN; $bytes_left=-1;
    fix_pipe_in($ext, "", 1); # Dat: defined later
  }
}
binmode IN;

#** Dat: uses $FileFormat, $InputFileName, $OutputFileName
#** @param $S prepend to pipe
sub run_sam2p($$) {
  my($approx_p,$S)=@_;
  # Imp: why isn't sam2p(1) PNG -> PNG idempotent?
  my $tfmt=$FileFormat eq'markedEPS' || $FileFormat eq 'EPS' ? 'EPS'
         : $FileFormat eq'markedPS' || $FileFormat eq 'PS' ? 'PS' # Dat: emits no /PageSize
         : $FileFormat eq'PDF1' || $FileFormat eq 'PDF' ? 'PDF'
         : undef;
  if (defined $tfmt) {}
  elsif (is_via_sam2p() or is_ok_sam2p()) {$tfmt=$FileFormat; $::opt_approx=1}
  else { error "sam2p doesn't support our FileFormat $FileFormat" }
  fix_pipe_in ".img", $S, 0;
  if ($approx_p) {
    if ($tfmt eq 'GIF') {
      # Dat: reduce palette to 8-bit if necessary
      my @args=('sam2p',@extra,"$tfmt:",'--',$InputFilename,$OutputFilename);
      debug "Running: @args";
      my $cmd=join('  ',map{fnq$_}@args)." 2>&1";
      my $res=readpipe($cmd);
      if ($res=~/\binvalid combination, no applicable OutputRule\b/) {
        # Dat: reduce palette to 8-bit
        #die "NOR";
        my $have_convert_p;
        my $have_pnmquant_p=0;
        for my $dir (split/:/,$ENV{PATH}) {
          if ((-f"$dir/pnmquant")) { $have_pnmquant_p=1 }
        }
        if (!$have_pnmquant_p) {
          $have_convert_p=0;
          for my $dir (split/:/,$ENV{PATH}) {
            if ((-f"$dir/convert")) { $have_convert_p=1 }
          }
        }
        my $cmd;
        if ($have_pnmquant_p) {
          my @args1=('sam2p','PPM:','--',$InputFilename,'-');
          my @args2=('sam2p',@extra,"$tfmt:",'--','-',$OutputFilename);
          $cmd=join('  ',map{fnq$_}@args1)." | pnmquant 256 | ".
                  join('  ',map{fnq$_}@args2);
        } elsif ($have_convert_p) {
          my @args1=('sam2p','PPM:','--',$InputFilename,'-');
          my @args2=('sam2p',@extra,"$tfmt:",'--','-',$OutputFilename);
          # vvv Dat: `convert - GIF:-' does quantize (and emits GIF)
          $cmd=join('  ',map{fnq$_}@args1)." | convert - GIF:- | ".
                  join('  ',map{fnq$_}@args2);
        }
        debug "Running pipe: $cmd";
        exec($cmd);
      } elsif ($? !=0) { die $res }
      # die $cmd;
      #debug "Running: $progname  @extra @_";
      #error "prog $progname failed: $? $!"
      #if 0!=system $progname, @extra, @_; # Dat: non-zero exit() or not found
    }
    do_exec('sam2p', ("$tfmt:", '--', $InputFilename, $OutputFilename));
  } else {
    warning "post-processing of sam2p PDF output increases file size" if $tfmt eq 'PDF';
    $tfmt='EPS' if $tfmt eq 'PDF'; # Imp: PDF1<->PDF
    close IN;
    my $tpfn=temp_prefix()."Psimg";
    error "Cannot open temp pipe dest: $tpfn" unless open TP, "> $tpfn";
    $tmpfiles{$tpfn}=1;
    die unless close TP;
    do_system('sam2p', ("$tfmt:", '--', $InputFilename, $tpfn));
    error "Cannot open temp pipe src: $tpfn" unless open IN, "< $tpfn";
    $already_read=0; $bytes_left=-1;
    $InputFilename=$tpfn; # '-'
    goto SCAN_AGAIN
  }
}

#** Force this value for %%HiResBoundingBox if a %BoundingBox is read
my $force_hiresbbox_value;

### scan first line, check for DOS EPSF (and remove DOS headers)
my $header;
{ SCAN_AGAIN:
  my $S;
  $_=$header="";
  read_error if 0>read IN, $S, 4;
  error "$InputFilename: empty file" if 0==length($S);
  $already_read+=length($S);
  ##print tell(IN)." bar=$already_read\n";
  my $iff="?"; # Input File Format
  # vvv be permissive, since we have only 4 chars
  if ($S eq "\211PNG") { $iff="PNG" }
  elsif ($S=~/\A(\377+\330)\377/) { $iff="JPEG" }
  elsif ($S eq "MM\000\052" or $S eq "II\052\000") { $iff="TIFF" }
  elsif ($S=~m@\AP([1-6])[\s#]@) { $iff="PNM" }
  elsif ($S=~/\ABM/) { $iff="BMP" }
  elsif ($S eq "GIF8") { $iff="GIF" }
  elsif ($S eq "FORM") { $iff="LBM" }
  elsif ($S eq "/* X" or $S eq "/*XP") { $iff="XPM" }
  elsif ($S=~/\A\12[\0-\005]\001[\001-\10]/) { $iff="PCX" }
  elsif ($S=~/\A[\36-\77](?:\001[\001\11]|\0[\002\12\003\13])\0/) { $iff="TGA" }
  elsif ($S eq "\305\320\323\306") { $iff="DOS-EPSF" }
  elsif ($S eq "\033%-1") { $iff="UEL" }
  elsif (substr($S,0,1)eq'%') { $iff="P" } # PS, EPS or PDF

   # PNG JPEG TIFF PNM BMP GIF LBM XPM PCX TGA
  if ($iff eq "DOS-EPSF") { # DOS EPSF header
    read_error if 30-4>read IN, $S, 30-4, 4;
    my ($eheader,$ps_ofs,$ps_len,$wmf_ofs,$wmf_len,$tif_ofs,$tif_len,$checksum)=
      unpack"A4VVVVVVv", $S;
    $already_read+=30-4;
    error "$InputFilename: bad DOS EPS" if $eheader ne "\305\320\323\306" or $ps_ofs<30;
    my($ps_end, $wmf_end, $tif_end)=($ps_ofs+$ps_len, $wmf_ofs+$wmf_len, $tif_ofs+$tif_len);
    $ps_ofs-=30;
    if (!seek IN, $ps_ofs, 1) {
      while ($ps_ofs>4096) { $ps_ofs-=4096; readIN 4096 }
      read_in $ps_ofs if $ps_ofs>0;
    }
    $bytes_left=($ps_end>$wmf_end and $ps_end>$tif_end) ? -1 : $ps_len;
    $S=readIN(1);
  } elsif ($iff eq "UEL") { # HP PJL UEL, untested
    $S.=readIN;
    $S=substr($S,1);
    error "$InputFilename: bad HP PJL UEL header: ".(~chomp($S)&&$S)
      if $S!~/\A\\e?%-12345X\@PJL ENTER LANGUAGE\s*=\s*POSTSCRIPT\s*\r?$/i;
    1 while length($S=readIN())!=0 and substr($S,0,4)ne'%!PS';
    die "$InputFilename: premature HP PJL UEL header" if length($S)==0;
  } elsif ($iff eq "P") {
    # no-op yet, see later
  } elsif ($iff eq "?") {
    error "unknown input image format: $InputFilename";
  } else { # source file is in some raster graphics format
    run_sam2p($::opt_approx,$S);
    goto SCAN_AGAIN
  }

  # now deal with PS, EPS and PDF
  if (substr($S,0,1) eq '%') {
    { my $max=128;
      my $C;
      while (length($S)<$max and defined($C=readIN(1)) and
       $C ne "\n" and $C ne "\r") { $S.=$C }
      error "couldn't find end of PS/PDF header line in $max bytes\n" if
        length($S)>=$max or !defined($C);
      $C=($C eq "\r") ? readIN(1) : "NONE";
      if (!defined$C or ($C ne "\n" and $C ne "NONE")) {
        use IO::Handle; # Dat: needed for ungetc
	IN->ungetc(ord($C)) if defined $C;
        debug "MAC \\r detected";
        $in_mac_p=1;
      } elsif ($C eq "\n") { # Dat: \r\n, DOS CRLF
        $in_mac_p=0; $S.="\r";
      } else { $in_mac_p=0 }
      $S.="\n";
    } # $S.=readIN;
    if (substr($S,0,4)eq'%PDF') {
      # error "$InputFilename: won't read a PDF file";
      if ($FileFormat eq 'PDF') { # convert PDF to PDF
        # !! PDF->PS->PDF
        open_OUT();
        $_=$S;
        debug "Doing a bit-by-bit copy";
        do {
          error "input error: $!" unless print OUT;
        } while (length($_=readIN 4096));
        close OUT; close IN;
        exit 0;
      } elsif ($FileFormat eq 'PDF1') { # Dat: remove extra pages by running pdftops and gs -sDEVICE=pdfwrite
        # !! Imp: possibly Run MetaPost output through full dvips (texc.pro) when prologues:=0
        # !! Imp: add full dvips %* font comment when prologues:=1 (design sizes missing :-()
	# Dat: no way to use $::opt_approx, because it doesn't remove extra pages
        fix_pipe_in ".pdf", $S, 0; # in case of stdin
        # Imp: option to open pdftops pipe instead of temp file
        # Dat: we rather use a temp file here for safety and early error detection
       do_input_pdftops: # come from EPS: and markedEPS:
        close IN; # after fix_pipe_in()
	my $tpfn=temp_prefix()."Peps";
        error "Cannot open temp pipe dest: $tpfn" unless open TP, "> $tpfn";
        $tmpfiles{$tpfn}=1;
        die unless close TP;
        do_system qw(pdftops -f 1 -l 1 -eps -- ), $InputFilename, $tpfn;
        error "Cannot open temp pipe src: $tpfn" unless open IN, "< $tpfn";
	$already_read=0; $bytes_left=-1;
        $InputFilename=$tpfn; # '-'
        goto SCAN_AGAIN
      } elsif ($FileFormat eq 'EPS' or $FileFormat eq 'markedEPS') { # convert PDF to EPS
        # Dat: limitation: markedEPS and EPS are treated the same
        # vvv Dat: pdftops(1) is part of the xpdf package
	# vvv Dat: pdftops(1) can emit to stdout
        fix_pipe_in ".pdf", $S, 0;
	if ($::opt_approx) {
          do_exec qw(pdftops -f 1 -l 1 -eps --), $InputFilename, $OutputFilename;
	} else { goto do_input_pdftops }
      } elsif ($FileFormat eq 'PS' or $FileFormat eq 'markedPS') {
        # Dat: limitation: markedEPS and EPS are treated the same
        fix_pipe_in ".pdf", $S, 0;
	close IN;
        # vvv we must query the BoundingBox first
        my $cmd="pdftops -f 1 -l 1 -eps ".fnq($InputFilename)." -";
        debug "pdftops bbox pipe: $cmd";
        error "pipe: $!" unless open PIPE, "$cmd |";
        my $line;
        error "expected PS document" if !defined($line=<PIPE>) or $line!~/^%!PS-Adobe-\d.*EPSF-\d/;
        my @L; # $papersize_x, $papersize_y
        while (<PIPE>) {
          last if /^%%EndComments/ or !/^%/;
          @L=($1,$2) if /^%%(?:Hires|Exact)BoundingBox:\s*\S+\s*\S+\s*(\S+)\s*(\S+)\s*$/i;
          @L=($1,$2) if /^%%BoundingBox:\s*\S+\s*\S+\s*(\S+)\s*(\S+)\s*$/i and !@L;
          # ^^^ Dat: HiRes has priority
          # ^^^ Dat: ignore llx and lly coordinates
        }
        1 while read PIPE, $line, 4096;
        error "closing PIPE: $?" unless close PIPE;
        error "BoundingBox not found in pdftops output" if !@L;
        debug "Got PaperSize: @L";

	# vvv Dat: pdftops without -eps doesn't report HiResBoundingBox,
	#     so we force it here
	# at Wed Nov 15 17:19:23 CET 2006
	$::opt_bboxfrom='adsc' if $::opt_bboxfrom eq 'guess';
	$force_hiresbbox_value="0 0 @L";
	
        #die defined $L[1];
        $L[0]=myceil $L[0]; $L[1]=myceil $L[1]; # Dat: pdftops expects integer papersize :-( )
	if ($::opt_approx) {
	  # vvv Dat: even pdftops 3.01 accepts only integer for -paperw and paperh
          do_exec 'pdftops', '-paperw', myfloor($L[0]+0.5), '-paperh', myfloor($L[1]+0.5), $InputFilename, $OutputFilename;
	} else {
	  my $tpfn=temp_prefix()."Pps";
	  error "Cannot open temp pipe dest: $tpfn" unless open TP, "> $tpfn";
	  $tmpfiles{$tpfn}=1;
	  die unless close TP;
          do_system 'pdftops', '-paperw', myfloor($L[0]+0.5), '-paperh', myfloor($L[1]+0.5), $InputFilename, $tpfn;
	  error "Cannot open temp pipe src: $tpfn" unless open IN, "< $tpfn";
	  $already_read=0; $bytes_left=-1;
	  $InputFilename=$tpfn; # '-'
	  goto SCAN_AGAIN
	}
      } elsif (is_ok_sam2p() or is_via_sam2p()) {
        # Dat: PDF to GIF conversion
        run_sam2p(1,$S);
      }

      error "cannot create from PDF: FileFormat $FileFormat";
     OK:
    }
    error "$InputFilename: EPS DSC must be %!PS-Adobe" if substr($S,0,4)ne'%!PS';
    # ^^^ Dat: mpost outputs "%!PS\n"
  } else {
    warning "$InputFilename: no PS ADSC header, BoundingBox not found\n"
  }
  $header=$S;
}

# Dat: Now we are converting from PS|EPS to EPS|markedEPS|PDF|PCL5|PGM
#      So we're converting from PS|EPS with Ghostscript

### variables and pattern for BoundingBox search
my $bbxpatt = '[0-9eE\.\-]';
my $BBValues = "\\s*($bbxpatt+)\\s+($bbxpatt+)\\s+($bbxpatt+)\\s+($bbxpatt+)"; # protect backslashes: "\\" gets '\'

my $ll_zero=0; # ($llx,$lly)==(0,0) in the output file
my $need_grestore=0; # 0 v 1
#** Applies %%*BoundingBox, %%EndComments, special setpagedevice, gsave..translate
#** @param $_[0] llx, may be undef to signify that bbox is undetected
#** @param $_[1] lly
#** @param $_[2] urx
#** @param $_[3] ury
#** @param $_[4] after_correct PostScript code (resolution, page size)
#** @return PostScript code to be printed after the header
sub CorrectBoundingBox($$$$$$$) {
  no integer;
  my $bbx="";
  my $pagedev_mark="";
  my $translate="";
  my ($llx, $lly, $urx, $ury, $after_correct, $fontsdefs, $is_restored) = @_;
  if (defined $llx) {
    my ($xoffset, $yoffset) = (0, 0);
    my $old_bbox="$llx $lly $urx $ury"; # debug "Old BoundingBox: $old_bbox";
    # my ($width, $height) = ($urx - $llx, $ury - $lly);
    ($xoffset, $yoffset) = (-$llx, -$lly);
    # $::opt_below=0 if $lly>=0; # always move to (0,0)
    # my ($urxh,$uryh)=($urx,$ury);
    # my $no_translate=$::opt_below;
    #die $no_translate;
    ($llx,$lly,$urx,$ury)=(0,0,$urx-$llx,$ury-$lly) if !$::opt_below;
    $urx=1 if $urx<=0; # Dat: GS dislikes empty image; fix also negative image
    $ury=1 if $ury<=0;
    my($px,$py)=($urx,$ury);
    ($px,$py)=($1+0,$2+0) if $after_correct=~m@/PageSize\s*\[(\S+)\s+(\S+)+\]@;
    my @paper=Htex::papers::any("$px,$py");
    $paper[0]=defined $paper[0] ? "%%DocumentPaperSizes: $paper[0]\n" : "";
    $bbx.="%%BoundingBox: ".myfloor($llx)." ".myfloor($lly)." ".
                             myceil($urx)." ". myceil($ury)."\n";
    $bbx.="%%HiResBoundingBox: $llx $lly $urx $ury\n".
          "%%ExactBoundingBox: $llx $lly $urx $ury\n" if myfloor($llx)!=$llx
      or myfloor($lly)!=$lly or myceil($urx)!=$urx or myceil($ury)!=$ury;
    $bbx.="%%DocumentMedia: plain $px $py 0 () ()\n". # like pdftops(1)
          "$paper[0]";
    # ^^^ Imp: can DocumentMedia be non-integer? As of us, it can.
    # vvv we output a second /MediaBox here, and we'll remove the first one
    #     (written by GS) later
    # vvv Dat: old version of Ghostscript insisted on an integer /CropBox (??).
    #          we do not force it now
    $pagedev_mark.="mark /CropBox [$llx $lly $urx $ury] /PAGE pdfmark\n" if is_pdfmark();
    # die "$xoffset $yoffset $::opt_below";
    if ($xoffset==0 and $yoffset==0) { #**** pts ****
      $need_grestore=0;
      $ll_zero=1; # Dat: we do not insert extra /MediaBox here, gs -sDEVICE=pdfwrite will do
    } elsif ($::opt_below) {
      # Do not translate (set (0,0) to the origin) with --below or multi-page
      # file formats.
      $need_grestore=0;
      $ll_zero=0; # fix /MediaBox because it become non-(0,0)-based
      $pagedev_mark.="mark /MediaBox [$llx $lly $urx $ury] /PAGE pdfmark\n" if
        is_pdfmark(); # $FileFormat eq 'markedEPS' or $FileFormat eq 'markedPS';
      # Dat: markedPS and contains pdfmark!
    } else {
      # debug "Offset:", $xoffset, $yoffset; # no new information, see -$llx, -$lly
      $xoffset=0 if $xoffset==0; # get rid of `-0'
      $yoffset=0 if $yoffset==0; # get rid of `-0'
      if ($is_restored) { $translate="" } # save..restore does gsave..grestore
                   else { $translate="gsave "; $need_grestore=1 }
      $ll_zero=0;
      # vvv the /MediaBox is different from what gs dumps
      # $pagedev_mark=~s@/PageSize\s*\[[^\]]*]@/PageSize [$urx $ury]@; # BUGFIX at Tue Apr 22 10:08:17 CEST 2003
      $pagedev_mark.="mark /MediaBox [$llx $lly $urx $ury] /PAGE pdfmark\n" if is_pdfmark();
      $translate.="$xoffset $yoffset translate\n"
    }
    my $new_bbox="$llx $lly $urx $ury";
    if ($old_bbox eq $new_bbox) {
      debug "Final (HiRes)BoundingBox: $new_bbox";
    } else {
      debug "Old (HiRes)BoundingBox: $old_bbox";
      debug "Final corrected (HiRes)BoundingBox: $new_bbox";
    }
    $pagedev_mark="/pdfmark where{pop}{/pdfmark/cleartomark load def}ifelse\n$pagedev_mark"
      if length($pagedev_mark)!=0;
  }
  # vvv Imp: `<<' -> `dict'
  # Dat: it is inherently impossible to tell GS that it shouldn't
  #      recompress the images already compressed in the EPS file, but keep
  #      them in their original, compressed form. So we rather instruct GS to
  #      recompress
  # !! Dat: /CompatibilityLevel 1.3 %PDF-1.2 -- Dat: 1.2 won't embed Courier
  my $markpagedevices="";
  my $imagesopts=($::opt_lossy ? "
/AutoFilterMonoImages true
/AutoFilterGrayImages true
/AutoFilterColorImages true
/MonoImageFilter /CCITTFaxEncode
/GrayImageFilter /DCTEncode
/ColorImageFilter /DCTEncode
" : "
/AutoFilterMonoImages false
/AutoFilterGrayImages false
/AutoFilterColorImages false
/MonoImageFilter /LZWEncode
/GrayImageFilter /LZWEncode
/ColorImageFilter /LZWEncode
"); # Dat: assumes new, patent-free LZW
  if (is_pdfmark()) {
    # Dat: CompatibilityLevel 1.3 is required for font embedding & all /FlateDecode
    $markpagedevices="
/CompatibilityLevel 1.3 %PDF-1.3
/EmbedAllFonts true
/Optimize true % ignored by gs-6.70
/AutoRotatePages /None
/UseFlateCompression ".($::opt_compression ne 'none'?"true":"false")."
/AutoPositionEPSFiles false
/ConvertImagesToIndexed false
/DownsampleMonoImages false
/DownsampleGrayImages false
/DownsampleColorImages false
/EncodeMonoImages true
/EncodeGrayImages true
/EncodeColorImages true
/AntiAliasMonoImages false
/AntiAliasGrayImages false
/AntiAliasColorImages false\n$imagesopts";
    $markpagedevices=(length($markpagedevices)!=0 ? "<< $markpagedevices >> setpagedevice\n" : "");
    $markpagedevices.="1 dict dup /ImagingBBox null put setpagedevice\n";
    $markpagedevices.="1 dict dup /Policies 1 dict dup /PageSize 3 put put setpagedevice\n"; # ripped from pdftops(1)
  }
  my $setpagesize="";
  # die defined $urx;
  # die is_set_pagesize_ok();
  # die $after_correct;
  if (defined $urx and is_set_pagesize_ok()
  and $::opt_papersize ne'force-unknown'
  and $after_correct!~m@/PageSize\s*\[@) { # Imp: m@/Pagesize ugly
    # Dat: true for FileFormat PGM
    # Dat: emit /PageSize even for PDF1
    # Dat: Ghostscript 6.70 rounds /PageSize down, but we need up when creating /MediaBox for PDF
    $setpagesize="2 dict dup /PageSize [".myceil($urx)." ".myceil($ury)."] put setpagedevice\n";
    # ^^^ Dat: PLRM.pdf doesn't forbid a non-integer /PageSize
  }
  my $bsetup=is_page1_stop()?"":"%%BeginSetup\n%%EndSetup\n";
  # ^^^ Dat: CUPS inserts its setpagedevice calls for /Duplex and /PageSize
  #     etc. just after the %%BeginSetup line (or, if missing, puts it in
  #     front of the first %%Page). We'd like this execution order: CUPS,
  #     ours, PStoPS (or psnup). (When we come after CUPS, we'll have a
  #     chance to override its settings.) So we emit a fake
  #     %%BeginSetup..%%EndSetup pair just before our code doing
  #     `setpagedevice'.
  # !! ?? run pstops first, and then a2ping.pl
  # !! why does a PDF -> PS conversion need $is_restored?
  # vvv Dat: `mark' is necessary, because pstops 1.17 from xpdf(1) emits lines
  #     lines leaving `false' on the stack:
  #     %%BeginResource: font SKPOPP+LMRoman12-Regular
  #     %!PS-AdobeFont-1.0: LMRoman12-Regular 0.86
  #     %%CreationDate: 4th August (Monday) 2003
  #     % Generated by MetaType1 (a MetaPost-based engine)
  #     % CM sources: copyright (C) 1997 AMS, all rights reserved; METATYPE1/Type 1 ver
  #     % ADL: 694 194 112
  #     %%EndComments
  #     FontDirectory/LMRoman12-Regular known{/LMRoman12-Regular findfont dup/UniqueID
  #     /UniqueID get 0 eq exch/FontType get 1 eq and}{pop false}ifelse
  #     {save true}{false}ifelse}{false}ifelse
  my $save=$is_restored?"save mark\n":"";
  $bbx.$fontsdefs."%%EndComments\n".$bsetup.$setpagesize.$markpagedevices.$pagedev_mark.$after_correct.$save.$translate
}

### scan header
my $to_OUT="";
my $after_code="";
my $do_fix_tumble=0;
my $is_restored=0;
if (1<length($header)) {
  my($llx,$lly,$urx,$ury);
  my($bbtype)='-'; # None
  my $allow_adsc_bb=1;
  my $after_correct="";
  my $do_bb_line=sub { # sub do_bbline($$)
    no integer;
    # Decreasing precedence of various BoundingBoxes:
    # The last valid *bbox entry has effect.
    # Active policy:
    #   normal mode : BoundingBox ExactBoundingBox HiResBoundingBox
    #   --hires mode: HiResBoundingBox BoundingBox ExactBoundingBox
    #   --exact mode: ExactBoundingBox BoundingBox HiResBoundingBox
    #   -hi -ex mode: ExactBoundingBox HiResBoundingBox BoundingBox
    # Another possible policy:
    #   normal mode : BoundingBox HiResBoundingBox==ExactBoundingBox
    #   --hires mode: HiResBoundingBox ExactBoundingBox BoundingBox
    #   --exact mode: ExactBoundingBox HiResBoundingBox BoundingBox
    #   -hi -ex mode: ExactBoundingBox==HiResBoundingBox BoundingBox
    my($S,$from)=@_;
    return if $S!~/^(?:$BBregex|set)$BBValues/oi;
    # print STDERR "($S)\n";
    my $E1=defined$1 ? $1 : "+";
    my $T=!defined($1) ? 'S' : uc substr $1,0,1; # '' || 'H' || 'E'
    # debug "Trying BoundingBox T-$bbtype: $llx $lly $urx $ury";
    if ($T eq 'S'
     or !$::opt_hires and !$::opt_exact and ($T eq '' or ($bbtype ne '' and ($T eq 'E' or $bbtype ne 'E')))
     or  $::opt_hires and !$::opt_exact and ($T eq 'H' or ($bbtype ne 'H' and ($T eq '' or $bbtype ne '')))
     or  $::opt_exact and !$::opt_hires and ($T eq 'E' or ($bbtype ne 'E' and ($T eq '' or $bbtype ne '')))
     or  $::opt_exact and  $::opt_hires and ($T eq 'E' or ($bbtype ne 'E' and ($T eq 'H' or $bbtype ne 'H')))
       ) {
      # if ($allow_bb) {
        ($bbtype,$llx,$lly,$urx,$ury)=($T,$2+0,$3+0,$4+0,$5+0);
        debug "Applying ${E1}BoundingBox$from T-$bbtype: $llx $lly $urx $ury";
      # }
    } else {
      my @L=($2+0,$3+0,$4+0,$5+0); # convert 0.00 to 0
      debug "Ignoring ${E1}BoundingBox$from T-$bbtype: @L";
    }
    # Dat: don't do $do_bb_line=sub{};# same as $allow_bb=0;
  };
  $header=~s@\r\n?\Z(?!\n)@@;
  my $after_comments=""; # after %%EndComments
  # my $res;
  { my $headEPSF;
    my $headPS="PS-Adobe-3.0";
    # vvv Imp: run this correction even w/o input EPS header
    if ($header!~s/\s+(EPSF-[.\d]+)$// or $::opt_bboxfrom ne 'guess') { # a PS not an EPS already
      # This is the compute-pipe routine.
      # To convert an [E]PS to an EPS:
      # -- find the end of the 1st page in the code, and remove everything after it
      # -- `pop' off the execution stack after the 1st page
      # -- `end' the dictionary stack after the 1st page
      # -- change the ADSC magic `%!PS-Adobe-...' to `%!PS-Adobe-3.0 EPSF-3.0'
      # -- remove the `%%Pages', `%%DocumentPaperSizes', `%%PageOrder:'
      #    (Ascending) comment from the ADSC header
      # -- possibly remove the `%%Page' ADSC comment [no]
      # -- ignore calls to = setpage setpagemargin setpageparams .setpagesize
      #    setpagedevice setpagetype setprintername setresolution a4 letter ...
      # -- surround the code by save ... pop* end* restore (implies gsave ... grestore)
      # -- possibly ignore calls to showpage [showpage is forced]
      $headEPSF=" EPSF-3.0";
      debug "Computing BBox info from non-EPS PS file";
      fix_pipe_in 'i.ps', "%!$headPS\n", 0;
      my $tfn=temp_prefix()."c.tgs";
      error "temp open $tfn: $!" unless open F, "> $tfn";
      $tmpfiles{$tfn}=1;
      ##print tell(IN)." car=$already_read\n";
      die "$0: $!\n" unless print F "% this is temporary gs command file created by $program".'
/DOCUT true def
/MAINFILE FN (r) file def
/DICTCOUNT countdictstack def
count /OPCOUNT exch def
<</BeginPage { % <PageCount> BeginPage -
    dup 1 eq {
      count OPCOUNT sub 1 sub (pop-count==) ..print ===
      countdictstack DICTCOUNT sub (end-count==) ..print ===
      DOCUT { (cut-offset==) ..print MAINFILE fileposition === flush } if
      (bbox-success\n) ..print
      quit
    } if
  }
  % Run the previous contents of /BeginPage: {--.callbeginpage--}
  [exch aload pop currentpagedevice /BeginPage get aload pop] cvx
>> setpagedevice

% vvv do these after our call to /setpagedevice
currentglobal true setglobal
systemdict begin
/..paper.redef<< >>def
/..print/print load def
/setpageparams{pop pop pop pop (\nset-called-4==/setpageparams\n) ..print flush}def
/setpage{pop pop pop (\nset-called-3==/setpage\n) ..print flush}def
/setpagesize{pop pop (\nset-called-2==/setpagesize\n) ..print flush}def
/.setpagesize{pop pop (\nset-called-2==/.setpagesize\n) ..print flush}def
/setpagemargin{pop (\nset-called-1==/setpagemargin\n) ..print flush}def
{% anti-Windows-printer-driver `%%[ ProductName:` etc.
  dup type/stringtype eq{
    dup length 3 ge{
      dup 0 3 getinterval (%%[) eq{
        (\nset-called-1==/=\n) ..print flush
        (\nset-called-1==/print\n) ..print flush
  }if}if}if
  pop
}
dup/= exch def /print exch def
/setpagedevice{pop (\nset-called-1==/setpagedevice\n) ..print flush}def
/..sdict <<
  /PageSize { % [. .] PageSize -
    dup type /arraytype eq {
      dup length 2 ge {
        dup 0 get type dup /integertype eq exch /realtype eq or {
          dup 1 get type dup /integertype eq exch /realtype eq or {
            (\npapersize-x==) ..print dup 0 get ===
            (\npapersize-y==) ..print dup 1 get === (\n) ..print
          } if
        } if
      } if
    } if
    pop
  }
  /HWResolution { % [. .] PageSize -
    dup type /arraytype eq {
      dup length 2 ge {
        dup 0 get type dup /integertype eq exch /realtype eq or {
          dup 1 get type dup /integertype eq exch /realtype eq or {
            (\nresolution-x==) ..print dup 0 get ===
            (\nresolution-y==) ..print dup 1 get === (\n) ..print
          } if
        } if
      } if
    } if
    pop
  }
  /Duplex { % [. .] PageSize -
    dup type /booleantype eq {
      (\nsides-duplex==) ..print dup === (\n) ..print
    } if
    pop
  }
  /Tumble { % [. .] PageSize -
    dup type /booleantype eq {
      (\nsides-tumble==) ..print dup === (\n) ..print
    } if
    pop
  }
>> def
%/.setpagesize{pop pop (hehehe\n) print} def
% /a4{(hehehe\n) ..print} def % doesn"t work, has to be put into userdict
/setpagedevice{
  { % <key> <val>
    exch dup ..sdict exch known { % run all keys known in ..sdict
      ..sdict exch get exec
    } {pop pop} ifelse
  } forall
  (\nset-called-1==/setpagedevice\n) ..print flush
}def
/setpagetype{pop (\nset-called-1==/setpagetype\n) ..print flush}def
/setprintername{pop (\nset-called-1==/setprintername\n) ..print flush}def
/setresolution{pop (\nset-called-1==/setresolution\n) ..print flush}def
[ % Dat: fixed at Mon May 19 14:32:31 CEST 2003
  statusdict /.pagetypenames 2 copy known {get}{pop pop{}}ifelse
  % ^^^ Dat: may be {/a4 STRICT {(%END SIZES) .skipeof} if /a5}
  { /11x17/a3/a4/a4small/b5/ledger/legal/letter % GS 7.04
    /lettersmall/note/a0/a1/a2/a5/a6/a7/a8/a9/c0/c1/c2/c3/c4/c5/c6
    /a10/b0/b1/b2/b3/b4/b5/b6
    /isob0/isob1/isob2/isob3/isob4/isob5/isob6
    /jisb0/jisb1/jisb2/jisb3/jisb4/jisb5/jisb6
    /archE/archD/archC/archB/archA/flsa/flse/halfletter
    /tabloid/csheet/dsheet/esheet/executivepage/com10envelope
    /monarchenvelope/c5envelope/dlenvelope/folio/quarto
  }
]{{
  dup type /nametype eq { dup xcheck not { % Dat: fixed
    % dup ===
    dup ..paper.redef exch known {pop} {
      dup ..paper.redef exch null put
      dup userdict exch 2 copy known { 2 copy get
        1 index userdict exch undef
        % Stack: /a4 userdict /a4 {...}
        % 3 copy pop undef
        4 2 roll exch undef
      } { exch pop pop {} } ifelse
      % Stack: /a4 {595 842 //.setpagesize --exec--}
      % Stack: /a4 {595 842 {/statusdict --.systemvar-- --begin-- .setpagesize --end--} --exec--}
      { /get exec /pop (\nset-called-0==) ..print === flush } % dump
      dup length array copy cvx % make a copy for subsequent invocations
      2 copy exch 0 exch put exch pop % change /get to {...}
      2 copy exch 2 exch put % change /pop to /a4
      def % overwrite it in systemdict
    } ifelse
    true
  } if } if
  pop
}forall} forall
end % systemdict
setglobal
systemdict readonly pop

(bbox-begin\n) ..print
MAINFILE cvx exec
(add-showpage==1\n) ..print
/DOCUT false def
showpage quit
';
      die unless close F;
      # vvv Imp: make it work on Win32 (no >&1 redirection)
      my $gs3=$CGS. # "-dPAGE1QUIT=".($FileFormat eq 'EPS' or $FileFormat eq 'markedEPS' ? 'quit' : '{}').
        " -dWRITESYSTEMDICT -dNOPAUSE -sDEVICE=bbox -sFN=".fnq($InputFilename)." ".fnq($tfn)." 2>&1";
      debug "Ghostscript compute pipe: $gs3";
      my $res=`$gs3`;
      ## die $res;
      temp_unlink $tfn;
      ## print STDERR $res;
      error $?==11 ? "segmentation fault in $GS" : "not a GS output from $GS ($?)"
        if !defined $res # BUGFIX at Sun Mar  7 18:51:34 CET 2004
        or $res!~s/\A(?:\w+ Ghostscript \d|Copyright .* artofcode ).*\n// # AFPL Ghostscript 6.50 (2000-12-02)
        or $res!~s/.*?^bbox-begin\n//m;
      if ($res!~s/\nbbox-success\n\Z(?!\n)/\n/) {
        warning # not `error', mpost(1) `prologues:=0; ... btex fonts' output won't compile
          "BBox discovery was not successful";
        # !! continue only if MetaPost output?
        goto SKIP_BBOX_DISC;
      }
      #: Copyright (C) 2000 Aladdin Enterprises, Menlo Park, CA.  All rights reserved.
      #: This software comes with NO WARRANTY: see the file PUBLIC for details.
      #: set-called-0==/a4
      #: %%BoundingBox: 56 41 539 783
      #: %%HiResBoundingBox: 56.645998 41.849999 538.811984 782.351976
      #: pop-count==0
      #: end-count==1
      #: cut-offset==81898
      my $pop_count=0;
      my $end_count=0;
      my $cut_offset=-1;
      my $papersize_x=undef; # page_width
      my $papersize_y=undef; # page_height
      my $resolution_x=undef;
      my $resolution_y=undef;
      my $duplexi=0;
      my $tumblei=0;
      my %H;
      my $undefs="";
      my $bbc=0; # required
      for my $line (split/\n/, $res) {
        if ($line=~/^$BBregex$BBValues/oi) { $do_bb_line->($line," from Compute-GS"); $bbc++ }
        elsif ($line=~m@^set-called-(\d+)==/(\S+)$@) {
          if (not exists $H{$2}) {
            $H{$2}=1;
            $undefs.="/$2".(
              $1==0 ? "{}def\n" :
              $1==1 ? "/pop load def\n" :
                      "{".("pop "x$1)."}bind def\n"
            );
          }
          # Dat: Safe, restorable, EPS-wise: /setpagedevice {pop} def
          # Dat: Smart, documentwise /a4 dup where{dup wcheck{exch{}put}{pop{}def}ifelse}{pop}ifelse
        }
        elsif ($line=~/^pop-count==(\d+)$/) { $pop_count=$1+0 }
        elsif ($line=~/^end-count==(\d+)$/) { $end_count=$1+0 }
        elsif ($line=~/^cut-offset==(\d+)$/) { $cut_offset=$1+0 }
        elsif ($line=~/^papersize-x==([-+0-9eE.]+)$/) { no integer; $papersize_x=$1+0 }
        elsif ($line=~/^papersize-y==([-+0-9eE.]+)$/) { no integer; $papersize_y=$1+0 }
        elsif ($line=~/^resolution-x==([-+0-9eE.]+)$/) { no integer; $resolution_x=$1+0 }
        elsif ($line=~/^resolution-y==([-+0-9eE.]+)$/) { no integer; $resolution_y=$1+0 }
        elsif ($line=~/^sides-duplex==true$/) { $duplexi=1 }
        elsif ($line=~/^sides-dumplex==false$/) { $duplexi=2 }
        elsif ($line=~/^sides-tumble==true$/) { $tumblei=1 }
        elsif ($line=~/^sides-tumble==false$/) { $tumblei=2 }
        elsif ($line=~/^add-showpage==\d+$/) { } # !!
        elsif (length($line)==0 or $line=~/^(?:Copyright |This software )/) {}
        elsif ($line=~/^Loading (\S+) font from.*[.][.][.]/) { debug "GS builtin font used: $1" }
        else { debug "unknown line ($line)" }
      }
      undef $papersize_y if !defined $papersize_x;
      die unless $allow_adsc_bb==1;
      # Dat: This only applies when converting fron non-EPS PS:
      #      Setting $allow_adsc_bb=0|1 now would disallow/allow the %%BoundingBox
      #      etc. ADSC comment override the bbox computed by -sDEVICE=bbox.
      #      When converting PS -> EPS, the PS file usually contains
      #      `%%BoundingBox: 0 0 paperwidth paperheight', but the figure
      #      itself is smaller.
      $bbtype='-';
      ## die "$papersize_x;;"; # PDF -sPAPERSIZE=a4
      #if ($::opt_papersize ne 'force-unknown' and $::opt_papersize ne 'unchanged') {
      #  # override bbox
      #  ($llx,$lly)=(0,0);
      #  ($urx,$ury)=split/,/,$::opt_papersize;
      #} elsif (!is_page1_stop() and defined $papersize_x and defined $papersize_y) {
      #}
      debug "PaperSize wd=${papersize_x}bp ht=${papersize_y}bp" if defined $papersize_x;
      if ($::opt_papersize eq 'force-unknown' or ($::opt_papersize eq 'unchanged' and !defined $papersize_x)) {}
      elsif ($::opt_papersize ne 'unchanged') {
        die if is_page1_stop();
        ($papersize_x,$papersize_y)=split/,/,$::opt_papersize;
        goto do_force_papersize
      } else {
        # Dat: no $do_bb_line here, because we've done it with $bbc++, and we'll also do it later
       do_force_papersize:
        die if is_page1_stop();
        # vvv ($llx,$lly,$urx,$ury)=(0,0,$papersize_x,$papersize_y);
        # $do_bb_line->("set 0 0 $papersize_x $papersize_y"," from /PageSize");
        $after_correct.="1 dict dup /PageSize [".myceil($papersize_x)." ".myceil($papersize_y)."] put setpagedevice\n";
        # ^^^ Dat: both PS and markedPS would benefit from /PaperSize
        # ^^^ Dat: will be put after CorrectBoundingBox
        # Dat: unneeded: $allow_adsc_bb=0 if $FileFormat eq 'PDF'; # force this into /CropBox (otherwise only /MediaBox)
      }
      if (defined $papersize_x and ($::opt_bboxfrom eq 'papersize' or ($::opt_bboxfrom eq 'guess' and is_set_pagesize_ok()))) {
        $allow_adsc_bb=0;
        $do_bb_line->("set 0 0 $papersize_x $papersize_y"," from /PageSize"); # does ($llx,$lly,$urx,$ury)=(0,0,$papersize_x,$papersize_y);
      }
      $allow_adsc_bb=0 if ($::opt_bboxfrom eq 'compute-gs') ? ($bbc!=0)
                        : ($::opt_bboxfrom eq 'guess') ? ($bbc!=0 and is_page1_stop()) # Imp: is is_page1_stop() OK here?
                        : 0;
      if (!is_page1_stop()) {
        $pop_count=$end_count=0; # assume PS is correct
        $cut_offset=-1;
      }
      # if ($FileFormat ne 'EPS' and $FileFormat ne 'PS') { # device-specific (marked)
      # ^^^ Dat: $::opt_resolution and $::opt_duplex are already 'force-unknown' if $FileFormat is appropriate
      {
	$::opt_resolution=$resolution_x.','.$resolution_y if
	  $::opt_resolution eq 'unchanged' and defined $resolution_x and defined $resolution_y;

        # vvv Imp: move down like $::opt_resolution
	if ($::opt_duplex eq 'force-unknown' or ($::opt_duplex eq 'unchanged' and $duplexi==0)) { $do_fix_tumble=($duplexi==1 && $tumblei==1) }
        elsif ($::opt_duplex eq 'force-simplex') { do_simplex: $after_correct.="1 dict dup /Duplex false put setpagedevice\n" }
        elsif ($::opt_duplex eq 'force-long' ) { do_long: $after_correct.="2 dict dup /Duplex true put dup /Tumble false put setpagedevice\n" }
        elsif ($::opt_duplex eq 'force-short') { do_short: $do_fix_tumble=1; $after_correct.="2 dict dup /Duplex true put dup /Tumble true put setpagedevice\n" }
        else {
          $duplexi ||= $::opt_duplex eq 'default-simplex' ? 2 : 1;
          $tumblei ||= $::opt_duplex eq 'default-short'   ? 1 : 2;
          goto do_simplex if $duplexi!=1;
          goto do_long if $tumblei!=1;
          goto do_short;
        }
      }
      # vvv Dat: save...restore is _always_ necessary to undo the changes made
      #     by the file itself (??)
      # vvv BUGFIX (only EPS) at Tue Feb  8 21:40:11 CET 2005
      # vvv Dat: now with PS output it is possible that garbage is left on
      #     the stack (see the `LMRoman' example above)
      $is_restored=1 if $FileFormat eq'EPS' or $FileFormat eq'markedEPS';
      $after_comments.=$undefs; # after our precious setpagedevice calls
      # debug "pop_count=$pop_count;";
      # debug "end_count=$end_count;";
      # debug "cut_offset=$cut_offset;";
      $after_code.=("pop\n"x$pop_count).("end\n"x$end_count);
      if ($cut_offset>=0 and ($bytes_left==-1 or $cut_offset<$bytes_left)) {
        $bytes_left=$cut_offset-$already_read;
        ##print tell(IN)." ar=$already_read\n";
        debug "Cutting after showpage at $cut_offset -> $bytes_left";
        # ^^^ Dat: cutting after `showpage' makes PS -> EPS conversion easy
        # sleep 1000;
      }
      # Dat: don't unlink $tifn yet, we'll continue scanning it
      # Imp: verify EPS created
    } else { $headEPSF=" $1" }
   SKIP_BBOX_DISC:
    if ($::opt_resolution eq 'force-unknown' or $::opt_resolution eq 'unchanged') {}
    else { $after_correct.="1 dict dup /HWResolution [@{[split/[x,]/,$::opt_resolution]}] put setpagedevice\n" }
    # ^^^ Dat: syntax already ok for $::opt_resolution
    $after_correct.="2 dict dup /TextAlphaBits 4 put dup /GraphicsAlphaBits 4 put setpagedevice\n" if
      $::opt_antialias eq 'yes' or $::opt_antialias eq 'scale3yes';

    $headEPSF="" if $FileFormat ne 'EPS' and $FileFormat ne 'markedEPS';
    $headPS=$1 if $header=~s/(PS-Adobe-[.\d]+)$//;
    $to_OUT.="%!$headPS$headEPSF\n";
  }

  debug "Scanning header for BoundingBox";
  my $do_atend=0;
  my $doing_atend=0;
  my $saved_pos;
  my $saved_bytes_left;
  my $creator_metapost_p=0; # HiResBoundingBox: after EndProlog
  my $creator_adobeps_p=0;
  my $had_pages=is_page1_stop(); # Dat: don't put `Pages:' to target EPS
  my $fontsdefs="";
  my %fontsnames;
  my @creator;
  read_again: while (length($_=readIN)) {
    #print STDERR "(($_))\n";
    ### end of header
    next unless /\S/;
    y@\r@@d; chomp;
    if (!$doing_atend) {
      if (/^%%EndComments\b/i) {
        # Dat: EPSI created by ImageMagick has BeginDefaults+EndDefaults+BeginPreview
        # Dat: PS output by ADOBEPS4.DRV has BeginDefaults+PageBoundingBox+ViewingOrientation+PageFeatures+EndDefaults
        1 while length($_=readIN) and !/\S/;
        if (!/^%%BeginDefaults\b/i) { y@\r@@d; chomp; $after_comments.="$_\n"; last }
        1 while length($_=readIN) and !/\S/;
        if (!/^%%EndDefaults\b/i) { y@\r@@d; chomp; $after_comments.="%%BeginDefaults\n$_\n"; last }
        next
      } elsif (/^%%EndDefaults\b/i) { # EPSI created by ImageMagick
      } elsif (/^%%Creator:\s*ADOBEPS/i) { # ADOBEPS4.DRV
        # Emits ``%%BoundingBox 1 1 ...' instead of `0 0'
        $creator_adobeps_p=1;
      } elsif (/^%%Creator:\s*MetaPost\b/i) {
        $creator_metapost_p=1;
      } elsif ((substr($_,0,2)ne'%%' and substr($_,0,7)ne'%*Font:' and
       substr($_,0,5)ne'%ADO_' and !/^%AI\d_/ # Dat: %ADO_DSC_..., %AI7_Thumbnail
       and substr($_,0,5)ne'%EPS ') # epsincl.mp
       or !$creator_metapost_p and substr($_,0,5)eq'%%End'
       or /^%%Begin(?:Prolog|Setup)\b/i
         ) { $after_comments.="$_\n"; last }
    }
    if (/^%%BeginPreview\b/i) { # remove EPSI preview ballast ****pts****
      while (1) {
        error "Missing EPSI %%EndPreview" if !length($_=readIN);
        last if /^%%EndPreview\b/;
        y@\r@@d; chomp
      }
    } elsif (/^%%Creator:\s*(.*)/i) {
      push @creator, $1;
      $creator[-1]=~s@\s+\Z(?!\n)@@;
      $creator[-1]=~s@, a2ping .*@@; # remove old
    } elsif (/^%%(?:DocumentPaperSizes|PageOrder|DocumentMedia):/i) {
      # silently ignore these -- will be recalculated
    } elsif (/^%%Pages:\s+(\d+)\s*/i and !$had_pages) { # Not `%%Pages: (atend)'
      # Dat: don't `$do_atend=1' only for %%Pages
      $to_OUT.="$_\n"; $had_pages=1
    } elsif (/^%%Pages:/i) {
      # silently ignore these -- not significant for EPS
    } elsif (/^$BBregex$BBValues/oi) { ### BoundingBox with values
      s@($BBregex)\s*1\s+1\s+@$1 0 0 @ if $creator_adobeps_p;
      # vvv $bbtype may be possibly already set by compute-gs
      if ($allow_adsc_bb) {
        $do_bb_line->($_," from ADSC");
	if ($force_hiresbbox_value) {
	  $do_bb_line->("%%HiResBoundingBox: $force_hiresbbox_value"," from ADSC");
	}
      }
    } elsif (/^$BBregex\s*\(atend\)/oi) {
      ### BoundingBox with (atend)
      debug "At end $1BoundingBox";
      # warning "Cannot look for BoundingBox in the trailer with option --filter" if $::opt_filter;
      # ^^^ Dat: may be seekable anyway, omit warning
      $do_atend=1
    } elsif (/^%%Page:/i and !$creator_metapost_p) { # at Thu Sep 25 15:59:52 CEST 2003
      $after_comments.="$_\n"; last
    } elsif (/^%(?:ADO_DSC_|AI\d_)/) { # Dat: example: %ADO_DSC_Encoding: MacOS Roman
      $to_OUT.="%$_\n";
    } elsif (/^%\*Font:\s+(\S+)\s+/) { # mpost(1) output
      ## debug $_;
      $fontsdefs.="$_\n"; # put in front (before `gsave ... translate')
      $fontsnames{$1}=1;
    } elsif (substr($_,0,5) eq '%EPS ') { # epsincl.mp
      $after_correct.="$_\n" if !$doing_atend; # before `gsave'
    } elsif ($doing_atend or /^%%End/) {
      # we might be in mid-line
    } else {
      $to_OUT.="$_\n"
    }
  }
  if ($doing_atend) { # already read (atend); restore file position
    seek(IN, $saved_pos, 0) or error "Input unseekable, cannot go back to line `: (atend)'";
    $bytes_left=$saved_bytes_left;
  } elsif ($do_atend) { # seek to near EOF and try again
    # error "Cannot leave line `: (atend)'" if ($saved_pos=tell IN)<0 or !seek(IN,0,1);
    fix_pipe_in "j.ps", $to_OUT, 0;
    die if ($saved_pos=tell IN)<0;
    $saved_bytes_left=$bytes_left;
    # vvv get the very last *BoundingBox entry from the last 4096 bytes
    if ($bytes_left>4096) {
      die unless seek(IN, $bytes_left-4096, 1);
      $bytes_left=4096;
    } elsif ($bytes_left<0) {
      die unless seek(IN, -4096, 2) or seek(IN, 0,0); # Dat: 0,0 for short files
    }
    $doing_atend=1; goto read_again;
  }
  # if (!$had_pages) { debug "Lying %%Pages: 1"; $to_OUT.="%%Pages: 1\n" }
  # ^^^ Dat: rather not lie !! go to (atend) if command line
  # vvv Dat: $bbtype ne'-' would be a bad idea
  warning "BoundingBox not found, try --bboxfrom=compute-gs" if !defined $llx; # !! compute-gs
  push @creator, "$program $Htex::a2ping::VERSION"; # if "@creator"!~/\ba2ping\b/;
  $to_OUT.="%%Creator: ".join(", ",@creator)."\n"; # before CorrectBoundingBox to be before `gsave' etc.
  $to_OUT.=CorrectBoundingBox $llx, $lly, $urx, $ury, $after_correct, $fontsdefs, $is_restored;
  $to_OUT.=$after_comments;
  if (%fontsnames) { # !! save .. restore
    $to_OUT.="{@{[sort keys%fontsnames]}}{\ndup where{pop pop}{cvlit dup def}ifelse}forall\n";
    $to_OUT.="/fshow where{pop}{/fshow{exch findfont exch scalefont setfont show}bind def}ifelse\n";
  }
}

### open output file or pipe (do this as late as possible)
my $do_scale3_pnm=0;
my $scale3_pnm_fn;
my @pnm2sampled_cmd;
if ($FileFormat eq 'PDF' or $FileFormat eq 'PDF1') {
  my $ofn=$ll_zero ? $OutputFilename : fix_pipe_out('.pdf');
  my $pipe = "$GS -q -dBATCH -sDEVICE=pdfwrite $GSOPTS -sOutputFile=".fnq($ofn)." -";
  debug "Ghostscript ps2pdf pipe:", $pipe;
  open(OUT, "| $pipe") or error "Cannot open Ghostscript pipe";
} elsif ($FileFormat eq 'PCL5') {
  my $ofn=$do_fix_tumble ? fix_pipe_out('.pcl5') : $OutputFilename;
  # vvv ljet4 cannot do duplex, ljet4d can
  my $pipe = "$GS -q -dBATCH -sDEVICE=ljet4d $GSOPTS -sOutputFile=".fnq($ofn)." -";
  debug "Ghostscript ps2ljet pipe:", $pipe;
  open(OUT, "| $pipe") or error "Cannot open Ghostscript pipe";
} elsif ($FileFormat eq 'PBM' or $FileFormat eq 'PGM' or $FileFormat eq 'PPM'
  or is_via_sam2p()) {
  $do_scale3_pnm=($::opt_antialias eq'scale3no' or $::opt_antialias eq 'scale3yes');
  my $device=$FileFormat eq 'PPM' || is_via_sam2p() ? 'ppmraw' :
    $FileFormat eq 'PBM' && $::opt_antialias eq 'no' ? 'pbmraw' : 'pgmraw';
  # Dat: -sDEVICE=pgm is ASCII, pgmraw is binary
  my $pipe ="$GS -q -dBATCH -sDEVICE=$device ";
  # $pipe.="-dTextAlphaBits=4 -dGraphicsAlphaBits=4 " if $::opt_antialias; # Dat: already done
  my $ofn=$OutputFilename;
  if (is_via_sam2p()) {
    # Dat: fortunately the following file format names are commin in sam2p(1)
    #      and a2ping.pl: PNG XWD BMP TIFF JPEG GIF XPM
    @pnm2sampled_cmd=('sam2p',"$FileFormat:",'--'); # Imp: use convert(1) if no sam2p(1)
    $ofn=fix_force_out('SS.pnm');
    push @pnm2sampled_cmd, $ofn, $OutputFilename;
  }
  if ($do_scale3_pnm) {
    $scale3_pnm_fn=$ofn;
    $ofn=fix_force_out('S3.pnm') if $do_scale3_pnm;
    # ^^^ Dat: this must be the last call to fix_force_out()
  }
  $pipe.="$GSOPTS -sOutputFile=".fnq($ofn)." -";
  debug "Ghostscript ps2raster pipe:", $pipe;
  open(OUT, "| $pipe") or error "Cannot open Ghostscript pipe";
} else { open_OUT() }

my $unlink_OutputFilename;
END { unlink $unlink_OutputFilename if defined $unlink_OutputFilename }
$unlink_OutputFilename=$OutputFilename;

die unless binmode OUT;
#die $to_OUT;
# !! remove setpagedevice from EPS->EPS, but keep EPS->PGM
# vvv needed by EPS->PGM conversion !!
if (is_need_showpage() and is_page1_stop()) {
  $to_OUT.="/showpage{showpage quit}bind def\n"; # don't call showpage twice
  $after_code.="showpage\n";
}
error "write OUT" if !print OUT $to_OUT;
$to_OUT="";

### print rest of file
sub OVERLAP_LENGTH(){4096} # `%%TrailerLength: 1162' by ADOBEPS4.DRV
my $extra_trailer="%%Trailer\n";
##undef $unlink_OutputFilename; die;
{ my $overlap=""; # keeps OVERLAP_LENGTH chars
  my $S;
  # print OUT while length($_=readIN 4096);
  while (length($S=readIN 4096)) { # Dat: 4096>=OVERLAP_LENGTH
    #if (length($S)<OVERLAP_LENGTH) { $S="$overlap$S"; $overlap=""; }
    #die if length($S)<OVERLAP_LENGTH; # happens sometimes
    #print OUT $overlap, substr($S,0,length($S)-OVERLAP_LENGTH());
    #$overlap=substr($S,-OVERLAP_LENGTH(),OVERLAP_LENGTH);
    next if length($overlap.=$S)<OVERLAP_LENGTH;
    print OUT substr($overlap,0,length($overlap)-OVERLAP_LENGTH());
    $overlap=substr($overlap,-OVERLAP_LENGTH());
  }
  if ($overlap=~s@\r?\n%%Trailer\r?\n(.*?)\Z(?!\n)@\n%%Trailer\n@s) {
    my $S=$1;
    $S=~y@\r@@d;
    $S=~s@\n%%TrailerLength:.*$@@m; # ADOBEPS4.DRV
    $overlap.=$S;
    $extra_trailer="";
  }
  $overlap=~s@(?:[\n\r\0\f]+%%EOF)?[\n\r\0\f]*\Z(?!\n)@@;
  # vvv Dat: would move %%Trailer after dvips output `end userdict /end-hook known{end-hook}if'
  # $overlap=~s@(?:[\n\r\0\f]+%%Trailer)?(?:[\n\r\0\f]+%%EOF)?[\n\r\0\f]+\Z(?!\n)@@;
  print OUT $overlap;
}

### close files
error "closing IN: $?" unless close IN;
# ^^^ SUXX: gs always exit(0), if exists
# vvv Dat: $after_code is pop+end
print OUT "\n$extra_trailer$after_code",
  ("grestore\n"x$need_grestore),
  ("cleartomark restore\n"x$is_restored),
  "%%EOF\n";
error "closing gs filter: $? $!" unless close OUT;

# --- PNM scaling routines for --antialias=scale3*

sub pnm_gettok($) {
  my ($fh,$S,$C)=($_[0],"");
  while (1) {
    die "unexpected EOF" if !defined($C=getc($fh));
    if ($C eq'#') { <$fh> } # ignore rest of line
    elsif ($C=~y@ \n\r\t@@) { last if length($S)!=0 }
    else { $S.=$C }
  }
  $S
}

my @div9=(0,0,0,0,0,map { $_, $_, $_, $_, $_, $_, $_, $_, $_ } 1..255, 255);

#** @param $_[0] length always divisible by 3
sub p5_avg_lines($$$$) {
  use integer;
  my $len=length($_[0]);
  my $olen=$len/3;
  while ($len>0) {
    vec($_[3],--$olen,8)=$div9[
      vec($_[0],$len-1,8)+vec($_[0],$len-2,8)+vec($_[0],$len-3,8)+
      vec($_[1],$len-1,8)+vec($_[1],$len-2,8)+vec($_[1],$len-3,8)+
      vec($_[2],$len-1,8)+vec($_[2],$len-2,8)+vec($_[2],$len-3,8)];
    $len-=3;
  }
}

#** @param $_[0] length always divisible by 9
sub p6_avg_lines($$$$) {
  # Imp: why is it lighter than: convert -scale '33.3333%' a3.pbm a3r.pgm
  use integer;
  my $len=length($_[0]);
  my $olen=$len/3;
  while ($len>0) {
    vec($_[3],--$olen,8)=$div9[
      vec($_[0],$len-1,8)+vec($_[0],$len-4,8)+vec($_[0],$len-7,8)+
      vec($_[1],$len-1,8)+vec($_[1],$len-4,8)+vec($_[1],$len-7,8)+
      vec($_[2],$len-1,8)+vec($_[2],$len-4,8)+vec($_[2],$len-7,8)];
    $len-=6 if 0==--$len%3;
  }
}

# ---

if (!$ll_zero and ($FileFormat eq 'PDF' or $FileFormat eq 'PDF1')) { # correct /MediaBox if not (0,0)-based
  ### ****pts**** remove incorrect /MediaBox produced by gs
  my $tfn=temp_prefix()."p.tgs";
  error "temp open $tfn: $!" unless open F, "> $tfn";
  $tmpfiles{$tfn}=1;
  # vvv Dat: doesn't work with gs 8.53: Error: /undefined in readxrefentry
  die unless print F "% this is temporary gs command file created by $program".'
  GS_PDF_ProcSet begin
  pdfdict begin
  FN (r) file pdfopen begin
  % vvv keep file offsets, because `pdffindpageref` overrides it with contents
  /OFT Objects 0 get dup length array copy def
  % vvv Dat: the generation number is assumed to be 0
  % vvv Dat: modifies Objects[0]
  1 pdffindpageref 0 get
  Objects 0 OFT put
  %===
  %print_xref
  { readxrefentry } stopped { Objects exch lget } if
  ===
  currentdict pdfclose end end end
  ';
  die unless close F;

  my $gs2="$GS -dNODISPLAY -dBATCH -sFN=".fnq(fix_pipe_out(undef))." -q ".fnq($tfn);
  debug "Ghostscript dup pipe: $gs2";
  my $offset=`$gs2`;
  #die $offset;
  chomp $offset;
  temp_unlink $tfn;
  if ($offset=~/\A\d+\Z(?!\n)/) {
    # Dat: now $offsets is a file position containing our /Page object
    die unless open F, "+< ".fix_pipe_out(undef);
    die unless binmode F;
    die unless seek F, $offset+=0, 0;
    my $pageobj;
    die unless 32<read F, $pageobj, 4096;
    if ($::opt_keepoldmediabox) {
      if ($pageobj=~m@\A(.*?/Type\s*/Page\b.*?/MediaBox\s*\[0 0 [^\]]*\].*?)((?:/CropBox\s*\[[^\]]+\]\s*)?/MediaBox\s*\[[^\]]+\])@s) {
	substr($pageobj, length($1), length($2))=" "x length($2);
	# ^^^ overwrite first buggy /MediaBox definition with spaces
	die unless seek F, $offset, 0;
	die unless print F $pageobj;
	debug "new /MediaBox destroyed.";
      } else {
	debug "warning: double /MediaBox not found at $offset";
      }
    } else {
      if ($pageobj=~m@\A(.*?/Type\s*/Page\b.*?)(/MediaBox\s*\[0 0 [^\]]*\]).*?/MediaBox\b@s) {
	substr($pageobj, length($1), length($2))=" "x length($2);
	# ^^^ overwrite first buggy /MediaBox definition with spaces
	die unless seek F, $offset, 0;
	die unless print F $pageobj;
	debug "old /MediaBox destroyed.";
      } else {
	debug "warning: double /MediaBox not found at $offset";
      }
    }
    die unless close F;
  } else {
    debug "warning: gs failed to locate double /MediaBox";
  }
}
if ($FileFormat eq 'PCL5' and $do_fix_tumble) {
  # stupid Ghostscript ignores /Tumble true with -sDEVICE=ljet4
  #  2 dict dup /Duplex true  /Tumble false put setpagedevice % long     HP PCL5e "\033&l1S"
  #  2 dict dup /Duplex true  /Tumble true  put setpagedevice % short    HP PCL5e "\033&l2S"
  #  2 dict dup /Duplex false /Tumble false put setpagedevice % simplex  HP PCL5e "\033&l0S"
  # HP PCL5e gs header "\033E\033&l2A\033&l1S\033&l0o0l0E\033&l-180u36Z"
  die unless open F, "+< ".fix_pipe_out(undef);
  die unless binmode F;
  my $pageobj;
  die unless 32<read F, $pageobj, 4096;
  if ($pageobj=~s@\033&l1S.*@\033&l2S@s) {
    die unless seek F, 0, 0;
    die unless print F $pageobj;
    debug "fixed /Tumble true (short).";
  } elsif ($pageobj=~m@\033&l0S@) {
    debug "no need to fix to /Tumble.";
  } else {
    debug "warning: /Duplex /Tumble settings not found"
  }
  die unless close F;
}
if ($do_scale3_pnm) {
  # Imp: scale down the file in place, ovoid early overwrite
  # Imp: possibly call an external C program that is faster
  debug "Scaling down PNM by 3x3";
  die unless open F, "> $scale3_pnm_fn";
  die unless binmode F;
  die unless open FIN, "< ".fix_pipe_out(undef);
  my $hd;
  die "PNMraw expected\n" if read(FIN,$hd,2)!=2 or $hd!~/\AP[456]/;
  my $wd=pnm_gettok(*FIN); die "width expected\n"  if $wd!~/\A(\d+)\Z(?!\n)/;
  my $wd3=$hd eq 'P4' ? ($wd+7)>>3 : $hd eq 'P5' ? $wd : $wd*3; # bw/grayscale/RGB
  my $ht=pnm_gettok(*FIN); die "height expected\n" if $ht!~/\A(\d+)\Z(?!\n)/;
  if ($hd ne 'P4') {
    my $mx=pnm_gettok(*FIN); die "max==255 expected, got: $mx\n" if $mx ne '255';
  }
  $wd+=0; $ht+=0;
  { use integer;
    my $phd=($hd eq 'P5' and $FileFormat eq 'PBM') ? "P4 ".(($wd+2)/3)." ".(($ht+2)/3)."\n"
      : ($hd eq 'P6' ? 'P6' : 'P5')."\n# reduced-3x3\n".
      (($wd+2)/3)." ".(($ht+2)/3)." 255\n";
    die if !print F $phd;
  }

  my($l1,$l2,$l3);
  my $ret="";
  if ($hd eq 'P4') {
    while ($ht>0) {
      die "full row expected1\n" if $wd3!=read FIN, $l1, $wd3;
      if (--$ht==0) { $l2=$l1 }
      else {
	die "full row expected2\n" if $wd3!=read FIN, $l2, $wd3;
	if (--$ht==0) { $l3=$l2 } # Imp: adjust 2/3 weight
	else { $ht--;
	  die "full row expected3\n" if $wd3!=read FIN, $l3, $wd3;
	}
      }
      $l1=unpack("B$wd",$l1); $l1=~y@10@\000\377@;  $l1.=substr($l1,-3+length($l1)%3) if length($l1)%3!=0;
      $l2=unpack("B$wd",$l2); $l2=~y@10@\000\377@;  $l2.=substr($l2,-3+length($l2)%3) if length($l2)%3!=0;
      $l3=unpack("B$wd",$l3); $l3=~y@10@\000\377@;  $l3.=substr($l3,-3+length($l3)%3) if length($l3)%3!=0;
      p5_avg_lines($l1, $l2, $l3, $ret);
      die if !print F $ret;
    }
  } elsif ($hd eq 'P5') {
    while ($ht>0) {
      die "full row expected1\n" if $wd3!=read FIN, $l1, $wd3;
      $l1.=substr($l1,-3+length($l1)%3) if length($l1)%3!=0;
      if (--$ht==0) { $l2=$l1 }
      else {
	die "full row expected2\n" if $wd3!=read FIN, $l2, $wd3;
	$l2.=substr($l2,-3+length($l2)%3) if length($l2)%3!=0;
	if (--$ht==0) { $l3=$l2 } # Imp: adjust 2/3 weight
	else { $ht--;
	  die "full row expected3\n" if $wd3!=read FIN, $l3, $wd3;
	  $l3.=substr($l3,-3+length($l3)%3) if length($l3)%3!=0;
	}
      }
      p5_avg_lines($l1, $l2, $l3, $ret);
      if ($FileFormat eq 'PBM') {
        my $I=length($ret);
	while ($I--) { vec($ret,$I,8)=vec($ret,$I,8)<$::opt_threshold } # [\0\1]
	# ^^^ grayscale>=$::opt_threshold will be white
        $ret=pack"B".length($ret),$ret;
      }
      die if !print F $ret;
    }
  } elsif ($hd eq 'P6') {
    while ($ht>0) {
      die "full row expected1\n" if $wd3!=read FIN, $l1, $wd3;
      $l1.=substr($l1,-9+length($l1)%9) if length($l1)%9!=0;
      if (--$ht==0) { $l2=$l1 }
      else {
	die "full row expected2\n" if $wd3!=read FIN, $l2, $wd3;
	$l2.=substr($l2,-9+length($l2)%9) if length($l2)%9!=0;
	if (--$ht==0) { $l3=$l2 } # Imp: adjust 2/3 weight
	else { $ht--;
	  die "full row expected3\n" if $wd3!=read FIN, $l3, $wd3;
	  $l3.=substr($l3,-9+length($l3)%9) if length($l3)%9!=0;
	}
      }
      p6_avg_lines($l1, $l2, $l3, $ret);
      die if !print F $ret;
    }
  }
  die unless close F;
  temp_unlink $temp_out_fn;
  undef $temp_out_fn;
}
if (@pnm2sampled_cmd) { # $scale3_pnm_fn -> $OutputFilename
  do_system @pnm2sampled_cmd; # Dat: uses @extra -- really share that?
  temp_unlink $scale3_pnm_fn;
} else { # BUGFIX for `a2ping.pl -v --antialias=no negyzet.eps negyzet.png' at Wed Jul 20 21:34:29 CEST 2005
  fix_close_out();
}
undef $unlink_OutputFilename;
if ($OutputFilename eq '-') {
  debug "Done OK, stdout is $FileFormat"
} elsif (-f $OutputFilename) {
  debug "Done OK, created $FileFormat file $OutputFilename (".(-s _)." bytes)";
} else {
  error "missing $OutputFilename"
}
just::end __END__

Dat: `=item * foo' is wrong, puts big space between `*' and `foo'

=encoding latin1

=begin man

.ds pts-dev \*[.T]
.do if '\*[.T]'ascii'  .ds pts-dev tty
.do if '\*[.T]'ascii8' .ds pts-dev tty
.do if '\*[.T]'latin1' .ds pts-dev tty
.do if '\*[.T]'nippon' .ds pts-dev tty
.do if '\*[.T]'utf8'   .ds pts-dev tty
.do if '\*[.T]'cp1047' .ds pts-dev tty
.do if '\*[pts-dev]'tty' \{\
.ll 79
.pl 33333v
.nr IN 2n
.\}
.ad n

=end man

=head1 NAME

a2ping.pl -- convert between PS, EPS and PDF and other page description
formats

=head1 SYNOPSIS

Z<> B<a2ping.pl> [B<-->]B<help>
 B<a2ping.pl> [B<-->]B<doc>
 B<a2ping.pl> [I<options>] <I<inputfile>> [[I<outformat>:] I<outputfile>]

=head1 DESCRIPTION

B<a2ping> is a UNIX command line utility written in Perl that
converts many raster image and vector graphics formats to EPS or PDF and
other page description formats. Accepted input file formats are: PS
(PostScript), EPS, PDF, PNG, JPEG, TIFF, PNM, BMP, GIF, LBM, XPM, PCX,
TGA. Accepted output formats are: EPS, PCL5, PDF, PDF1, PBM, PGM, PPM,
PS, markedEPS, markedPS, PNG, XWD, BMP, TIFF, JPEG, GIF, XPM.
B<a2ping> delegates the low-level work to
Ghostscript (GS), B<pdftops> and B<sam2p>. B<a2ping> fixes many glitches
during the EPS to EPS conversion, so its output is often more compatible
and better embeddable than its input.

Without the C<--below> option, it is guarenteed to start at the 0,0
coordinate. C<--below>, C<--hires> and C<-v> are recommended options.

The page size is set exactly corresponding to the BoundingBox.
This means that when Ghostscript renders it, the result needs no
cropping, and the PDF MediaBox is correct.

If the bounding box is not right, of course, you have problems. If you
feed crap in, you get crap. But you can supply the
B<--bboxfrom=compute-gs> option to make GS recompute the bounding box.

The name of the input file doesn't matter -- B<a2ping> detects the file
format based on the first few bytes of the file. The name of the output
file matters if I<outformat> is missing from the command line: then the
extension of the output file determines the FileFormat (I<outformat>).

=head1 EXTERNAL PROGRAMS

The internal file format of B<a2ping.pl> is PS/EPS. Everything read is
first converted to PS or EPS, then processed by B<a2ping.pl>, then
converted to the output format.

To analyse the bounding box and other properties of non-EPS PS files
(and EPS files with option B<--bboxfrom> other than B<=guess>), GS is
used. Converting PS to EPS involves this analysis.

To write PDF files, GS is used.

To read PDF files, B<pdftops> from the B<xpdf> package is used.

Sampled input formats are PNG, JPEG, TIFF, PNM, BMP, GIF, LBM, XPM, PCX
and TGA. To read sampled input formats, B<sam2p> is used. B<sam2p> is
a raster image converter written in C++ by the author of B<a2ping.pl>.

Extra output formats are PNG, XWD, BMP, TIFF, JPEG, GIF and XPM. To
write extra output formats, B<sam2p> and GS are used.

PNM output formats are PGM, PGM and PPM. To write PNM output formats, GS
is used.

=head1 OPTIONS

=head2 General Options

=over 2

=item B<-h>, B<--help>

Show a summary of the usage

=item B<--doc>

Show the man page

=item B<-v>, B<--(no)verbose>

Show progress and debug messages (default: no)

=back

=head2 Options for the Bounding box

=over 2

=item B<--(no)hires>

Use HiResBoundingBox in the input file, if present (default: yes)

=item B<--(no)exact>

Use ExactBoundingBox in the input file, if present (default: no)

=item B<--(no)keepoldmediabox>

keep only old, [0 0]-based MediaBox in PDF (default: no)

=item B<--bboxfrom=>I<adsc|compute-gs|guess|pagesize>

Method for determining the BoundingBox  (default: guess)

=item B<--(no)below>

Allow page content below and left of the origin (default: no)

=back

=head2 Options for graphics and fonts

=over 2

=item B<--(no)compress>

use compression                   (default: best)

=item B<--(no)antialias>

render shades at outlines.  Possible values: (=I<scale3yes> =I<no> =I<yes>)  (default: scale3no)

=item B<--(no)lossy>

allow lossy image filters (EPS->PDF) (default: yes)

=item B<--papersize=>I<unchanged|force-unknown|600bpx5cm>

(default: default) (bp)

=item B<--threshold=>

min color for 1 in 8->1 bit conv  (default: 128)

=back

=head2 Options for debugging and changing internals

=over 2

=item B<--(no)tmpunlink>

Unlink temporary files (default: yes).  Use B<--notmpunklink> if you
want to inspect intermediate files.

=item B<--gs-cmd=>I<path>

path to Ghostscript program (default: gs or gswin32c)

=item B<--gs-ccmd=>I<path>

path to Ghostscript for BoundingBox calculation (default: gs or gswin32c)

=item B<--gsextra=>I<GS_ARGS>

Pass extra arguments to gs

=item B<--extra=>

Pass extra arguments to external program (i.e pdftops)

=back

=head1 TIPS AND TRICKS

=over 2

=item *

If your EPS contains a wrong bounding box, you can fix it by running
C<a2ping.pl -v --bboxfrom=compute-gs thefile.eps -->

=item *

You can specify B<-> as I<inputfile> to get stdin and as I<outputfile>
to get stdout. This works even for PDF files (which must be seekable),
because B<a2ping> copies them to a temporary file automatically.

=item *

If I<inputfile> and I<outputfile> are the same, B<a2ping> copies the
I<inputfile> to a temporary location first. However, this usage is
recommended only if there is a backup of the file to be restored in case
B<a2ping> doesn't produce the desired result.

=item *

If you specify B<--> as I<outputfile>, it will be the same as I<inputfile>.

=item *

B<a2ping> respects B<--Duplex> for FileFormat PCL5, even though GS doesn't.

=item *

If you have an incompatible PS that GS can read but your printer cannot print,
just run C<a2ping.pl foo.ps PDF: - | a2ping.pl - PS: foo.ps>

=item *

If you have a PS coming from Win32 (often with extension C<.prn>), run
it through B<a2ping>. It will remove the resolution changes and the
progress text printed to the terminal (which confuses gv(1) and makes
some filters in the print queue emit incorrect output).

=item *

B<a2ping> does antialiasing (B<--antialias=scale3no>) of glyphs and
curves when emitting a sampled image (FileFormats such as PGM and PPM).
This improves readability of the glyphs. B<=yes> instructs GS to do
internal antialiasing, but it usually doesn't improve much. B<=scale3no>
turns off GS internal antialiasing, but makes it render everything 3x3
as big, and then scales it back down. B<=scale3no> turns on both 3x3
scaling and GS internal antialiasing, which results in thicker lines and
worse quality in general.

=item *

When creating a PBM file, antialiasing usually doesn't improve the
quality, so it is switched off by default. But if you set
B<--antialias=scale3no> or B<--antialias=scale3yes>, GS will render a PGM file,
and the value of B<--threshold> determines the minimum intensity for white in
the final PBM.

=item *

If you need a bigger sampled output file, specify a larger
B<--Resolution>. The default is B<--Resolution=72>. If your sampled output file
is going to be really big, you should specify B<--AntiAlias=yes> instead of
the default B<--AntiAlias=scale3no> to speed up conversion.

=item *

To make sure fonts are included in a PDF file generated from eps, use
B<--gsextra='-dEmbedAllFonts=true -dPDFSETTINGS=/printer'>.

=back


=head1 MISC

=over 2

=item *

Doesn't depend on the filename or extension of the input file.

=item *

Conversion from EPS to PDF: fixes glitches etc., calls gs
-sDEVICE=pdfwrite

=item *

Conversion from EPS to EPS: fixes various glitches, moves (llx,lly) to
(0,0), removes binary junk from the beginning of the EPS etc.

=item *

Conversion from PDF to PDF: keeps the file intact

=item *

Conversion from PDF to EPS: calls pdftops -eps (of the xpdf package)

=item *

Conversion from PS to EPS: keeps 1st page only, removes setpagedevice etc.

=back

=head1 AUTHORS

The author of B<a2ping> is Péter Szabó <F<pts@fazekas.hu>>.

B<a2ping> is inspired by and historically based on the B<epstopdf> Perl
script modified by Thomas Esser, Sept. 1998, but his modifications have
been removed from B<a2ping>, and also B<a2ping> and B<epstopdf> do not
share common code anymore.  B<epstopdf> is written by Sebastian Rahtz,
for Elsevier Science. B<epstopdf> contained extra tricks from Hans Hagen's
texutil.

B<a2ping> contains contributions from several people, see the file
F<HISTORY.txt> for details. Thank you all for contributing!
