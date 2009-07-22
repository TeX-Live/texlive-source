eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;
use strict;
$^W=1; # turn warning on
#
# pdfcrop.pl
#
# Copyright (C) 2002, 2004, 2005, 2008, 2009 Heiko Oberdiek.
#
# This program may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either version 1.2
# of this license or (at your option) any later version.
# The latest version of this license is in
#   http://www.latex-project.org/lppl.txt
# and version 1.2 or later is part of all distributions of LaTeX
# version 1999/12/01 or later.
#
# See file "README" for a list of files that belong to this project.
#
# This file "pdfcrop.pl" may be renamed to "pdfcrop"
# for installation purposes.
#
my $file        = "pdfcrop.pl";
my $program     = uc($&) if $file =~ /^\w+/;
my $version     = "1.18";
my $date        = "2009/07/18";
my $author      = "Heiko Oberdiek";
my $copyright   = "Copyright (c) 2002-2009 by $author.";
#
# Reqirements: Perl5, Ghostscript
# History:
#   2002/10/30 v1.0:  First release.
#   2002/10/30 v1.1:  Option --hires added.
#   2002/11/04 v1.2:  "nul" instead of "/dev/null" for windows.
#   2002/11/23 v1.3:  Use of File::Spec module's "devnull" call.
#   2002/11/29 v1.4:  Option --papersize added.
#   2004/06/24 v1.5:  Clear map file entries so that pdfTeX
#                     does not touch the fonts.
#   2004/06/26 v1.6:  Use mgs.exe instead of gswin32c.exe for MIKTEX.
#   2005/03/11 v1.7:  Support of spaces in file names
#                     (open("-|") is used for ghostscript call).
#   2008/01/09 v1.8:  Fix for moving the temporary file to the output
#                     file across file system boundaries.
#   2008/04/05 v1.9:  Options --resolution and --bbox added.
#   2008/07/16 v1.10: Support for XeTeX added with new options
#                     --pdftex, --xetex, and --xetexcmd.
#   2008/07/22 v1.11: Workaround for open("-|").
#   2008/07/23 v1.12: Workarounds for the workaround (error detection, ...).
#   2008/07/24 v1.13: open("-|")/workaround removed.
#                     Input files with unsafe file names are linked/copied
#                     to temporary file with safe file name.
#   2008/09/12 v1.14: Error detection for invalid Bounding Boxes.
#   2009/07/14 v1.15: Fix for negative coordinates in Bounding Boxes
#                     (David Menestrina).
#   2009/07/16 v1.16: Security fixes:
#                     * -dSAFER added for Ghostscript,
#                     * -no-shell-escape added for pdfTeX/XeTeX.
#   2009/07/17 v1.17: Security fixes:
#                     * Backticks and whitespace are forbidden
#                       for options --(gs|pdftex|xetex)cmd.
#                     * Validation of options --papersize and --resolution.
#   2009/07/18 v1.18: * Restricted mode added.
#                     * Option --version added.

### program identification
my $title = "$program $version, $date - $copyright\n";

### error strings
my $Error = "!!! Error:"; # error prefix

### string constants for Ghostscript run
# get Ghostscript command name
my $GS = "gs";
$GS = "gs386"    if $^O =~ /dos/i;
$GS = "gsos2"    if $^O =~ /os2/i;
$GS = "gswin32c" if $^O =~ /mswin32/i;
$GS = "gswin32c" if $^O =~ /cygwin/i;
$GS = "mgs"      if defined($ENV{"TEXSYSTEM"}) and
                    $ENV{"TEXSYSTEM"} =~ /miktex/i;

# Windows detection (no SIGHUP)
my $Win = 0;
$Win = 1 if $^O =~ /mswin32/i;
$Win = 1 if $^O =~ /cygwin/i;

# restricted mode
my $restricted = 0;
if ($0 =~ /rpdfcrop/ or $0 =~ /restricted/) {
    $restricted = 1;
}

# "null" device
use File::Spec::Functions qw(devnull);
my $null = devnull();

### variables
my $inputfile   = "";
my $outputfile  = "";
my $tmp = "tmp-\L$program\E-$$";

### paper sizes

my @papersizes = qw[
  11x17 ledger legal letter lettersmall
  archE archD archC archB archA
  a0 a1 a2 a3 a4 a4small a5 a6 a7 a8 a9 a10
  isob0 isob1 isob2 isob3 isob4 isob5 isob6
  c0 c1 c2 c3 c4 c5 c6
  jisb0 jisb1 jisb2 jisb3 jisb4 jisb5 jisb6
  b0 b1 b2 b3 b4 b5
  flsa flse halfletter
];
my %papersizes;
foreach (@papersizes) {
    $papersizes{$_} = 1;
}

### option variables
my @bool = ("false", "true");
$::opt_version    = 0;
$::opt_help       = 0;
$::opt_debug      = 0;
$::opt_verbose    = 0;
$::opt_gscmd      = $GS;
$::opt_pdftexcmd  = "pdftex";
$::opt_xetexcmd   = "xetex";
$::opt_tex        = "pdftex";
$::opt_margins    = "0 0 0 0";
$::opt_clip       = 0;
$::opt_hires      = 0;
$::opt_papersize  = "";
$::opt_resolution = "";
$::opt_bbox       = "";

my $usage = <<"END_OF_USAGE";
${title}Syntax:   \L$program\E [options] <input[.pdf]> [output file]
Function: Margins are calculated and removed for each page in the file.
Options:                                                    (defaults:)
  --help              print usage
  --version           print version number
  --(no)verbose       verbose printing                      ($bool[$::opt_verbose])
  --(no)debug         debug informations                    ($bool[$::opt_debug])
  --gscmd <name>      call of ghostscript                   ($::opt_gscmd)
  --pdftex | --xetex  use pdfTeX | use XeTeX                ($::opt_tex)
  --pdftexcmd <name>  call of pdfTeX                        ($::opt_pdftexcmd)
  --xetexcmd <name>   call of XeTeX                         ($::opt_xetexcmd)
  --margins "<left> <top> <right> <bottom>"                 ($::opt_margins)
                      add extra margins, unit is bp. If only one number is
                      given, then it is used for all margins, in the case
                      of two numbers they are also used for right and bottom.
  --(no)clip          clipping support, if margins are set  ($bool[$::opt_clip])
                      (not available for --xetex)
  --(no)hires         using `%%HiResBoundingBox'            ($bool[$::opt_hires])
                      instead of `%%BoundingBox'
Expert options:
  --restricted        turn on restricted mode               ($bool[$restricted])
  --papersize <foo>   parameter for gs's -sPAPERSIZE=<foo>,
                      use only with older gs versions <7.32 ($::opt_papersize)
  --resolution <xres>x<yres>                                ()
  --resolution <res>  pass argument to ghostscript's option -r
                      Example: --resolution 72
  --bbox "<left> <top> <right> <bottom>"                    ()
                      override bounding box found by ghostscript
Examples:
  \L$program\E --margins 10 input.pdf output.pdf
  \L$program\E --margins '5 10 5 20' --clip input.pdf output.pdf
In case of errors:
  Try option --verbose first to get more information.
In case of bugs:
  Please, use option --debug for bug reports.
END_OF_USAGE

### process options
my @OrgArgv = @ARGV;
use Getopt::Long;
GetOptions(
  "help!",
  "version!",
  "debug!",
  "verbose!",
  "gscmd=s",
  "pdftexcmd=s",
  "xetexcmd=s",
  "pdftex" => sub { $::opt_tex = 'pdftex'; },
  "xetex"  => sub { $::opt_tex = 'xetex'; },
  "margins=s",
  "clip!",
  "hires!",
  "papersize=s",
  "resolution=s",
  "bbox=s",
  "restricted" => sub { $restricted = 1; },
) or die $usage;
!$::opt_help or die $usage;

if ($::opt_version) {
    print "$version\n";
    exit(0);
}

$::opt_verbose = 1 if $::opt_debug;

@ARGV >= 1 or die $usage;

print $title;

if ($::opt_bbox) {
    $::opt_bbox =~ s/^\s+//;
    $::opt_bbox =~ s/\s+$//;
    $::opt_bbox =~ s/\s+/ /;
    if ($::opt_bbox =~ /^-?\d*\.?\d+ -?\d*\.?\d+ -?\d*\.?\d+ -?\d*\.?\d+$/) {
        print "* Explicite Bounding Box: $::opt_bbox\n" if $::opt_debug;
    }
    else {
        die "$Error Parse error (option --bbox \"$::opt_bbox\")!\n";
    }
}

@ARGV <= 2 or die "$Error Too many files!\n";

### input file
$inputfile = shift @ARGV;

if (! -f $inputfile) {
    if (-f "$inputfile.pdf") {
        $inputfile .= ".pdf";
    }
    else {
        die "$Error Input file `$inputfile' not found!\n";
    }
}

print "* Input file: $inputfile\n" if $::opt_debug;

### output file
if (@ARGV) {
    $outputfile = shift @ARGV;
}
else {
    $outputfile = $inputfile;
    $outputfile =~ s/\.pdf$//i;
    $outputfile .= "-crop.pdf";
}

print "* Output file: $outputfile\n" if $::opt_debug;

if (($::opt_tex eq 'xetex') && $::opt_clip) {
    die "$Error No clipping support for XeTeX!\n";
}

### margins
my ($llx, $lly, $urx, $ury) = (0, 0, 0, 0);
if ($::opt_margins =~
        /^\s*([\-\.\d]+)\s+([\-\.\d]+)\s+([\-\.\d]+)\s+([\-\.\d]+)\s*$/) {
    ($llx, $lly, $urx, $ury) = ($1, $2, $3, $4);
}
else {
    if ($::opt_margins =~ /^\s*([\-\.\d]+)\s+([\-\.\d]+)\s*$/) {
        ($llx, $lly, $urx, $ury) = ($1, $2, $1, $2);
    }
    else {
        if ($::opt_margins =~ /^\s*([\-\.\d]+)\s*$/) {
            ($llx, $lly, $urx, $ury) = ($1, $1, $1, $1);
        }
        else {
            die "$Error Parse error (option --margins)!\n";
        }
    }
}
print "* Margins: $llx $lly $urx $ury\n" if $::opt_debug;

### papersize validation (security)
if ($::opt_papersize ne '') {
    $::opt_papersize =~ /^[0-9A-Za-z]+$/
            or die "$Error Invalid papersize ($::opt_papersize)!\n";
    $papersizes{$::opt_papersize}
            or die "$Error Unknown papersize ($::opt_papersize),"
                   . " see ghostscript's documentation for option `-r'!\n";
}

### resolution validation (security)
if ($::opt_resolution ne '') {
    $::opt_resolution =~ /^\d+(x\d+)?$/
            or die "$Error Invalid resolution ($::opt_resolution),"
                   . " see ghostscript's documentation!\n";
}

### command name validation (security)
my %cmd = (
    'gscmd' => \$::opt_gscmd,
    'pdftexcmd' => \$::opt_pdftexcmd,
    'xetexcmd' => \$::opt_xetexcmd
);
foreach my $cmd (keys %cmd) {
    my $val = ${$cmd{$cmd}};
    next unless $val;
    $val =~ s/^\s+//;
    $val =~ s/\s+$//;
    next unless $val;
    if ($val =~ /`/) {
        die "$Error Forbidden backtick for option `--$cmd' ($val)!\n";
    }
    if ($val =~ /\s/) {
        die "$Error Forbidden whitespace for option `--$cmd' ($val)!\n";
    }
}
if ($restricted) {
    if ($::opt_pdftexcmd and $::opt_pdftexcmd ne 'pdftex') {
        die "$Error pdfTeX program name must not be changed in restricted mode!\n";
    }
    if ($::opt_xetexcmd and $::opt_xetexcmd ne 'xetex') {
        die "$Error XeTeX program name must not be changed in restricted mode!\n";
    }
    if ($::opt_gscmd) {
        $::opt_gscmd =~ /^(gs|mgs|gswin32c|gs386|gsos2)$/
        or $::opt_gscmd =~ /^gs[\-_]?(\d|\d[\.-_]?\d\d)c?$/
        or die "$Error: Invalid Ghostscript program name in restricted mode!\n";
    }
}

### cleanup system
my @unlink_files = ();
my $exit_code = 1;
sub clean {
    print "* Cleanup\n" if $::opt_debug;
    if ($::opt_debug) {
        print "* Temporary files: @unlink_files\n";
    }
    else {
        for (; @unlink_files>0; ) {
            unlink shift @unlink_files;
        }
    }
}
sub cleanup {
    clean();
    exit($exit_code);
}
$SIG{'INT'} = \&cleanup;
$SIG{'__DIE__'} = \&clean;

### Calculation of BoundingBoxes

# use safe file name for use within cmd line of gs (unknown shell: space, ...)
# and pdfTeX (dollar, ...)
my $inputfilesafe = $inputfile;
if ($inputfile =~ /[\s\$~'"]/) {
    $inputfilesafe = "$tmp-img.pdf";
    push @unlink_files, $inputfilesafe;
    my $symlink_exists = eval { symlink("", ""); 1 };
    print "* Input file name `$inputfile' contains special characters.\n"
          . "* " . ($symlink_exists ? "Link" : "Copy")
          . " input file to temporary file `$inputfilesafe'.\n"
            if $::opt_verbose;
    if ($symlink_exists) {
        symlink($inputfile, $inputfilesafe)
            or die "$Error Link from `$inputfile' to"
                   . " `$inputfilesafe' failed: $!\n";
    }
    else {
        use File::Copy;
        copy($inputfile, $inputfilesafe)
                or die "$Error Copy from `$inputfile' to"
                       . " `$inputfilesafe' failed: $!\n";
    }
}

my @gsargs = (
    "-sDEVICE=bbox",
    "-dBATCH",
    "-dNOPAUSE"
);
push @gsargs, "-sPAPERSIZE=$::opt_papersize" if $::opt_papersize;
push @gsargs, "-r$::opt_resolution" if $::opt_resolution;
push @gsargs,
    "-c",
    "save",
    "pop",
    "-f",
    $inputfilesafe
;

my $tmpfile = "$tmp.tex";
push @unlink_files, $tmpfile;
open(TMP, ">$tmpfile") or
    die "$Error Cannot write tmp file `$tmpfile'!\n";
print TMP "\\def\\pdffile{$inputfilesafe}\n";
if ($::opt_tex eq 'pdftex') {
    print TMP <<'END_TMP_HEAD';
\csname pdfmapfile\endcsname{}
\def\page #1 [#2 #3 #4 #5]{%
  \count0=#1\relax
  \setbox0=\hbox{%
    \pdfximage page #1{\pdffile}%
    \pdfrefximage\pdflastximage
  }%
  \pdfhorigin=-#2bp\relax
  \pdfvorigin=#3bp\relax
  \pdfpagewidth=#4bp\relax
  \advance\pdfpagewidth by -#2bp\relax
  \pdfpageheight=#5bp\relax
  \advance\pdfpageheight by -#3bp\relax
  \ht0=\pdfpageheight
  \shipout\box0\relax
}
\def\pageclip #1 [#2 #3 #4 #5][#6 #7 #8 #9]{%
  \count0=#1\relax
  \dimen0=#4bp\relax \advance\dimen0 by -#2bp\relax
  \edef\imagewidth{\the\dimen0}%
  \dimen0=#5bp\relax \advance\dimen0 by -#3bp\relax
  \edef\imageheight{\the\dimen0}%
  \pdfximage page #1{\pdffile}%
  \setbox0=\hbox{%
    \kern -#2bp\relax
    \lower #3bp\hbox{\pdfrefximage\pdflastximage}%
  }%
  \wd0=\imagewidth\relax
  \ht0=\imageheight\relax
  \dp0=0pt\relax
  \pdfhorigin=#6pt\relax
  \pdfvorigin=#7bp\relax
  \pdfpagewidth=\imagewidth
  \advance\pdfpagewidth by #6bp\relax
  \advance\pdfpagewidth by #8bp\relax
  \pdfpageheight=\imageheight\relax
  \advance\pdfpageheight by #7bp\relax
  \advance\pdfpageheight by #9bp\relax
  \pdfxform0\relax
  \shipout\hbox{\pdfrefxform\pdflastxform}%
}%
\def\pageinclude#1{%
  \pdfhorigin=0pt\relax
  \pdfvorigin=0pt\relax
  \pdfximage page #1{\pdffile}%
  \setbox0=\hbox{\pdfrefximage\pdflastximage}%
  \pdfpagewidth=\wd0\relax
  \pdfpageheight=\ht0\relax
  \advance\pdfpageheight by \dp0\relax
  \shipout\hbox{%
    \raise\dp0\box0\relax
  }%
}
END_TMP_HEAD
}
else { # XeTeX
    print TMP <<'END_TMP_HEAD';
\expandafter\ifx\csname XeTeXpdffile\endcsname\relax
  \expandafter\ifx\csname pdffile\endcsname\relax
    \errmessage{XeTeX not found!}%
  \else
    \errmessage{XeTeX is too old!}%
  \fi
\fi
\def\page #1 [#2 #3 #4 #5]{%
  \count0=#1\relax
  \setbox0=\hbox{%
    \XeTeXpdffile "\pdffile" page #1%
  }%
  \pdfpagewidth=#4bp\relax
  \advance\pdfpagewidth by -#2bp\relax
  \pdfpageheight=#5bp\relax
  \advance\pdfpageheight by -#3bp\relax
  \shipout\hbox{%
    \kern-1in%
    \kern-#2bp%
    \vbox{%
      \kern-1in%
      \kern#3bp%
      \ht0=\pdfpageheight
      \box0 %
    }%
  }%
}
END_TMP_HEAD
}

print "* Running ghostscript for BoundingBox calculation ...\n"
    if $::opt_verbose;
print "* Ghostscript call: $::opt_gscmd @gsargs\n" if $::opt_debug;

my @bbox;
if ($::opt_bbox) {
     $::opt_bbox =~ /([-\d\.]+) ([-\d\.]+) ([-\d\.]+) ([-\d\.]+)/;
     @bbox = ($1, $2, $3, $4);
}
my $page = 0;
my $gs_pipe = "$::opt_gscmd -dSAFER @gsargs 2>&1";
$gs_pipe .= " 1>$null" unless $::opt_verbose;
$gs_pipe .= "|";

open(GS, $gs_pipe) or
        die "$Error Cannot call ghostscript ($::opt_gscmd)!\n";
my $bb = ($::opt_hires) ? "%%HiResBoundingBox" : "%%BoundingBox";
while (<GS>) {
    print $_ if $::opt_verbose;
    next unless
        /^$bb:\s*(-?[\.\d]+) (-?[\.\d]+) (-?[\.\d]+) (-?[\.\d]+)/o;
    @bbox = ($1, $2, $3, $4) unless $::opt_bbox;
    $page++;

    my $empty = 0;
    $empty = 1 if $bbox[0] >= $bbox[2];
    $empty = 1 if $bbox[1] >= $bbox[3];
    if ($empty) {
        print <<"END_WARNING";

!!! Warning: Empty Bounding Box is returned by Ghostscript!
!!!   Page $page: @bbox
!!! Either there is a problem with the page or with Ghostscript.
!!! Recovery is tried by embedding the page in its original size.

END_WARNING
        print TMP "\\pageinclude{$page}\n";
        next;
    }

    print "* Page $page: @bbox\n" if $::opt_verbose;

    my @bb = ($bbox[0] - $llx, $bbox[1] - $ury,
             $bbox[2] + $urx, $bbox[3] + $lly);

    $empty = 0;
    $empty = 1 if $bb[0] >= $bb[2];
    $empty = 1 if $bb[1] >= $bb[3];
    if ($empty) {
        print <<"END_WARNING";

!!! Warning: The final Bounding Box is empty!
!!!   Page: $page: @bb
!!! Probably caused by too large negative margin values.
!!! Recovery by ignoring margin values.

END_WARNING
        print TMP "\\page $page [@bbox]\n";
        # clipping shouldn't make a difference
        next;
    }
    if ($::opt_clip) {
        print TMP "\\pageclip $page [@bbox][$llx $lly $urx $ury]\n";
    }
    else {
        print TMP "\\page $page [@bb]\n";
    }
}
close(GS);

if ($? & 127) {
    die sprintf  "$Error Ghostscript died with signal %d!\n",
                 ($? & 127);
}
elsif ($? != 0) {
    die sprintf "$Error Ghostscript exited with error code %d!\n",
                $? >> 8;
}

print TMP "\\csname \@\@end\\endcsname\n\\end\n";
close(TMP);

if ($page == 0) {
    die "$Error Ghostscript does not report bounding boxes!\n";
}

### Run pdfTeX/XeTeX

push @unlink_files, "$tmp.log";
my $cmd;
my $texname;
if ($::opt_tex eq 'pdftex') {
    $cmd = $::opt_pdftexcmd;
    $texname = 'pdfTeX';
}
else {
    $cmd = $::opt_xetexcmd;
    $texname = 'XeTeX';
}
$cmd .= ' -no-shell-escape';
if ($::opt_verbose) {
    $cmd .= " -interaction=nonstopmode $tmp";
}
else {
    $cmd .= " -interaction=batchmode $tmp";
}
print "* Running $texname ...\n" if $::opt_verbose;
print "* $texname call: $cmd\n" if $::opt_debug;
if ($::opt_verbose) {
    system($cmd);
}
else {
    `$cmd`;
}
if ($?) {
    die "$Error $texname run failed!\n";
}

### Move temp file to output
if (!rename("$tmp.pdf", $outputfile)) {
    use File::Copy;
    move "$tmp.pdf", $outputfile or
            die "$Error Cannot move `$tmp.pdf' to `$outputfile'!\n";
}

print "==> $page page", (($page == 1) ? "" : "s"),
      " written on `$outputfile'.\n";

$exit_code = 0;
cleanup();

__END__
