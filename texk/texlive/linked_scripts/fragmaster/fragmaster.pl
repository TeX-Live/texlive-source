#!/usr/bin/perl -w

######################################################################
#
# fragmaster.pl
#
# creates EPS and PDF graphics from source EPS and control files
# with \psfrag commands
#
# Version: 1.6
#
# Copyright (C) 2004 Tilman Vogel <tilman vogel web de> (dot at dot)
# Copyright (C) 2011 Agustin Martin <agustin martin hispalinux es> (dot at dot)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# See pod section at the end of this script for more information
# ----------------------------------------------------------------------------

use strict;
use Cwd;
use File::Temp qw(tempdir);
use Pod::Usage;

my $force;
my $debug;
my $global_dirfm_mtime;
my $global_dirfm_file   = "fragmaster.dfm";
my $fragmaster_status   = "fragmaster.sts";
my $global_fmclass      = "article";
my $global_fmclassopt   = "12pt";
my $global_fmopt        = "";
my @global_fmhead       = ();
my @global_fmfile       = ();
my %dirdata_last_status = ();


foreach my $option ( @ARGV ){
  if ( $option eq "--force" ){
    $force++;
  } elsif ( $option eq "--debug" ){
    print STDERR "Debug enabled\n";
    $debug++;
  } elsif ( $option =~ s/^--dirfm(=|\#)// ) {
    if ( -e $option ){
      $global_dirfm_file = $option;
      print STDERR "fragmaster: using \"$global_dirfm_file\" directory control file.\n"
	if $debug;
    } else {
      die "fragmaster: Could not find given \"$option\" directory control file. Aborting ....\n";
    }
  } elsif ( $option eq "-h" || $option eq "--help" ) {
    pod2usage(1);
    exit 1;
  } elsif ( $option eq "-m" || $option eq "--man" ) {
    pod2usage(-exitstatus => 0, -verbose => 2);
    exit 1;
  } else {
    print STDERR "\nfragmaster: Unsupported option \"$option\"\n\n";
    pod2usage(1);
    exit 1;
  }
}

# Function definitions

# ----------------------------------------------------------------------------
sub win32_which {
# ----------------------------------------------------------------------------
# Look if given basename matches a win32 executable under $PATH
# ----------------------------------------------------------------------------
  my $basename   = shift;
  my @extensions = (".exe");

  foreach my $path ( '.', split(';',$ENV{'PATH'}) ){
    foreach my $ext ( @extensions ){
      my $exec_location = "$path\\$basename$ext";
      if ( -x "$exec_location" ){
	print STDERR "fragmaster: Found gs executable at \"$exec_location\"\n"
	  if $debug;
	return $basename;
      } else {
	print STDERR "fragmaster: No gs executable found at \"$exec_location\"\n"
	  if $debug;
      }
    }
  }
}

# ----------------------------------------------------------------------------
sub parse_fmfile {
# ----------------------------------------------------------------------------
  my $file   = shift;
  my %fmdata = ();
  my @fmfile = ();      # A copy of original file
  my @fmhead = ();      # Preamble to use
  my $fmopt;            # Options to \includegraphics
  my $fmclass;          # Class to use
  my $fmclassopt;       # Options for class invocation

  die "fragmaster::parse_fmfile: No file to parse passed as argument. Aborting ...\n"
    unless $file;

  open FMFILE, "<$file"
    or die "fragmaster: Cannot read \"$file\"!. Aborting ...\n";

  while (<FMFILE>) {
    chomp;
    push @fmfile, "  $_%";
    s/\s+$//;   # Strip trailing whitespace
    $fmopt      = $1 if /fmopt:\s*(.*)/;
    $fmclass    = $1 if /fmclass:\s*(.*)/;
    $fmclassopt = $1 if /fmclassopt:\s*(.*)/;
    if (/head:/) {
      while(<FMFILE>) {
	chomp;
	push @fmfile, "  $_%";
	last if /end head/;
	# Remove comment prefix
	s/^[\s%]*//;
	push @fmhead, "$_%";
      }
    }
  }
  close FMFILE;
  $fmdata{'fmfile'}     = \@fmfile    if scalar @fmfile;
  $fmdata{'fmpreamble'} = \@fmhead    if scalar @fmhead;
  $fmdata{'fmclass'}    = $fmclass    if $fmclass;
  $fmdata{'fmclassopt'} = $fmclassopt if $fmclassopt;
  $fmdata{'fmopt'}      = $fmopt      if $fmopt;
  print STDERR
    "fragmaster::parse_fmfile: Keys found in \"$file\": ",
    join(', ',sort keys %fmdata ), "\n"
    if $debug;
  return \%fmdata;
}

# ----------------------------------------------------------------------------

my $cwd = getcwd;

die "fragmaster: Current path contains whitespace. LaTeX cannot handle this correctly, move somewhere else. Aborting ... \n"
  if $cwd =~ /\s/;

# Get modification time for $global_dirfm_file if exists. Also used as existence flag.
$global_dirfm_mtime = -M $global_dirfm_file if ( -e $global_dirfm_file );

# Look for fragmaster _fm files.
my %all_basenames = ();
foreach (<*_fm>){
  s/_fm$//;
  $all_basenames{$_}++;
}
die "fragmaster: No fm_files found. Aborting ...\n"
  unless ( scalar %all_basenames );

# Check if there is a status file for last processing with dir control file
if ( -e $fragmaster_status ){
  open my $LAST, "< $fragmaster_status"
    or die "fragmaster: Could not open \"$fragmaster_status\" for read. Aborting ...\n";
  while ( <$LAST> ){
    next if m/^\s*\#/;
    next if m/^\s*~/;
    chomp;
    if ( m/^([^:]+):(.*)$/ ) {
      $dirdata_last_status{$1} = $2;
      print STDERR "fragmaster: Last dfm used for \"$1\": \"$dirdata_last_status{$1}\"\n"
	if $debug;
    } else {
      print STDERR "fragmaster: Wrongly formatted line in \"$fragmaster_status\": \"$_\". Ignoring ...\n";
    }
  }
  close $LAST;
}

# Remove no longer present files from status file
foreach my $base ( sort keys %dirdata_last_status ){
  delete $dirdata_last_status{$base} unless ( defined $all_basenames{$base} );
}

# For each _fm file found, check if we need to rebuild
my %doit_basenames = ();
foreach my $base ( sort keys %all_basenames ) {
  my $fm_file = "$base" . "_fm";
  my $source  = "$fm_file.eps";

  if ( -f $source ) {
    my $dest_eps = "$base.eps";
    my $dest_pdf = "$base.pdf";
    my $do_it    = $force;

    unless ( $do_it ) {
      # Rebuild if there is any mismatch betwen last and current per-dir file.
      if ( $global_dirfm_mtime ){
	if ( defined $dirdata_last_status{$base} ){
	  $do_it++ unless ( $global_dirfm_file eq $dirdata_last_status{$base} );
	} else {
	  $do_it++;
	}
      } elsif ( defined $dirdata_last_status{$base} ){
	$do_it++;
      }
    }

    unless ( $do_it ) {
      if ( -f $dest_eps && -f $dest_pdf ){
	# Sort after modification times. Redo if any source is newer than any dest.
	my @dest_stamps     = (-M $dest_eps, -M $dest_pdf);
	my @sources_stamps  = (-M $fm_file,  -M $source );
	push @sources_stamps, $global_dirfm_mtime if $global_dirfm_mtime;
	my $oldest_dest     = (sort { $b <=> $a } @dest_stamps)[0];
	my $youngest_source = (sort { $a <=> $b } @sources_stamps)[0];
	$do_it++ if $oldest_dest > $youngest_source;
      } else {
	$do_it++;
      }
    }

    $doit_basenames{$base}++ if $do_it;
  } else {
    print "fragmaster: \"$fm_file\" skipped. Cannot find associated EPS file \"$source\".\n";
  }
}

# Do nothing if targets are up to date and --force was not set
unless ( scalar %doit_basenames ){
  print STDERR "fragmaster: all targets are up-to-date.\n";
  exit;
}

# Create master tempdir for the temporary files
my $base_tempdir =
  tempdir( "fragmaster.XXXXXX",     # Use a fragmaster prefix for base tempdir
	   CLEANUP => ( ! $debug ), # Remove tempdir tree on exit unless $debug
	   DIR => $cwd )            # Create base tempdir under current dir
  or die "fragmaster: Cannot make temporary directory!";
print "fragmaster: Using $base_tempdir/\n";

# Check which name to use for ghostscript.
my $gs_prog = "gs";

if ( $^O =~ /^MSWin/ ){
  $gs_prog = win32_which("gswin32c") # Try first standard name
    || win32_which("mgs")            # Be kind with miktex
    || "gswin32c";                   # May be a wrapper with different extension. Try.
}

# If we have a per-directory control file with values set, use them to override defaults.
if ( -e $global_dirfm_file ){
  my $fmdata         = parse_fmfile($global_dirfm_file);
  $global_fmclass    = $fmdata->{'fmclass'}       if ( defined $fmdata->{'fmclass'} );
  $global_fmclassopt = $fmdata->{'fmclassopt'}    if ( defined $fmdata->{'fmclassopt'} );
  $global_fmopt      = $fmdata->{'fmopt'}         if ( defined $fmdata->{'fmopt'} );
  @global_fmhead     = @{$fmdata->{'fmpreamble'}} if ( defined $fmdata->{'fmpreamble'} );
}

# (Re-)build targets
foreach my $base ( sort keys %doit_basenames ) {
  my $fm_file  = "$base" . "_fm";
  my $source   = "$fm_file.eps";
  my $dest_eps = "$base.eps";
  my $dest_pdf = "$base.pdf";

  print "fragmaster: $fm_file, $source -> $dest_eps, $dest_pdf\n";

  # Parse per-graphic _fm control file
  my $fmdata     = parse_fmfile($fm_file);
  my $fmclass    = ( defined $fmdata->{'fmclass'} )    ? $fmdata->{'fmclass'}       : $global_fmclass;
  my $fmclassopt = ( defined $fmdata->{'fmclassopt'} ) ? $fmdata->{'fmclassopt'}    : $global_fmclassopt;
  my $fmopt      = ( defined $fmdata->{'fmopt'} )      ? $fmdata->{'fmopt'}         : $global_fmopt;
  my @fmhead     = ( defined $fmdata->{'fmpreamble'} ) ? @{$fmdata->{'fmpreamble'}} : @global_fmhead;
  my @fmfile     = ( defined $fmdata->{'fmfile'} )     ? @{$fmdata->{'fmfile'}}     : @global_fmfile;

  my $texstring = "\\documentclass[$fmclassopt]{$fmclass}
\\usepackage{graphicx,psfrag,color}
";
  # Enter heading info
  $texstring .= "$_\n" foreach (@fmhead);

  $texstring .= '
\setlength{\topmargin}{-1in}
\setlength{\headheight}{0pt}
\setlength{\headsep}{0pt}
\setlength{\topskip}{0pt}
\setlength{\textheight}{\paperheight}
\setlength{\oddsidemargin}{-1in}
\setlength{\evensidemargin}{-1in}
\setlength{\textwidth}{\paperwidth}
\setlength{\parindent}{0pt}
\special{! TeXDict begin /landplus90{true}store end }
%\special{! statusdict /setpage undef }
%\special{! statusdict /setpageparams undef }
\pagestyle{empty}
\newsavebox{\pict}
\graphicspath{{../../}}

\begin{document}
  \begin{lrbox}{\pict}%
';
  $texstring .= "$_\n" foreach (@fmfile);
  $texstring .= "  \\includegraphics[$fmopt]{$source}%\n";
  $texstring .= '  \end{lrbox}
  \special{papersize=\the\wd\pict,\the\ht\pict}
  \usebox{\pict}
\end{document}
';

  my $tempdir = "$base_tempdir/$base";
  mkdir $tempdir;

  # Writing TeX file
  open TEXFILE, ">$tempdir/fm.tex"
    or die "fragmaster: Cannot write LaTeX file!";
  print TEXFILE $texstring;
  close TEXFILE;

  chdir($tempdir)
    or die "fragmaster: Cannot chdir to \"$tempdir\"!. Aborting ...\n";

  system("latex fm.tex") == 0
    or die "fragmaster: Cannot latex \"fm.tex\"!. Aborting ...\n";

  # Using -E here, causes dvips to detect
  # the psfrag phantom stuff and to set the BoundingBox wrong
  system("dvips -E -P pdf fm.dvi -o fm.ps") == 0
    or die "fragmaster: Cannot dvips!. Aborting ...\n";

  chdir($cwd)
    or die "fragmaster: Cannot chdir back up!. Aborting ...\n";

  # Read original eps file as created by dvips -E.
  my $psfile;
  {
    local $/ = undef;
    open PS, "<$tempdir/fm.ps"
      or die "fragmaster: Cannot read \"fm.ps\"!. Aborting ...\n";
    $psfile = <PS>;
    close PS;
  }

  # Calculate real Bounding Box using ghostscript bbox device, also available
  # in strippped-down texlive gs. Currently, it writes result only to STDERR.
  my $bbox_line;
  my $bbox_cmd = "$gs_prog -q -sDEVICE#bbox -dNOPAUSE -dBATCH \"$tempdir/fm.ps\" 2> \"$tempdir/fm.bb\"";
  print STDERR "fragmaster: $bbox_cmd\n" if $debug;
  system("$bbox_cmd") == 0
    or die "fragmaster: Failed to obtain Bounding Box file with ghostscript bbox device.\n";
  open BB,"<$tempdir/fm.bb"
    or die "fragmaster: Could not read Bounding Box file \"$tempdir/fm.bb\". Aborting ...\n";
  while (<BB>) {
    if ( /^\%\%BoundingBox:/ ){
      chomp;
      $bbox_line = $_;
      last;
    }
  }
  close BB;
  die "fragmaster: No Bounding Box entry found in \"$tempdir/fm.bb\". Aborting ...\n"
    unless $bbox_line;
  print STDERR "fragmaster: $bbox_line\n"
    if $debug;

  # Fix Bounding Box and write corrected eps file.
  $psfile =~ s/^\%\%BoundingBox:.*/$bbox_line/m;
  open EPS, ">$dest_eps"
    or die "fragmaster: Cannot write \"$dest_eps\"!. Aborting ...\n";
  print EPS $psfile;
  close EPS;

  # Create pdf file after corrected ps file.
  system("epstopdf $dest_eps --outfile=$dest_pdf") == 0
    or die "fragmaster: Cannot epstopdf!. Aborting ...\n";

  # Update per-dir info in status file if everything was OK. We need to do this
  # after processing each entry. Otherwise an error may leave things inconsistent.
  if ( $global_dirfm_mtime ){
    $dirdata_last_status{$base} = $global_dirfm_file; # Mark per-dir file used
  } else {
    delete $dirdata_last_status{$base};                 # or leave empty otherwise
  }

  print STDERR
    "fragmaster: \"$base\" processed, current files with dfm: \n [" ,
    join(', ',sort keys %dirdata_last_status),
    "]\n"
    if $debug;
  open my $LAST, "> $fragmaster_status"
    or die "fragmaster: Could not open \"$fragmaster_status\" for write. Aborting ...\n";

  print $LAST "\# This file is automatically generated by fragmaster. Do not edit.\n";
  foreach ( sort keys %dirdata_last_status ){
    print $LAST "$_:$dirdata_last_status{$_}\n";
  }
  close $LAST;
}

__END__

=pod

=head1 NAME

fragmaster - Using psfrag constructs with pdflatex

=head1 SYNOPSIS

 fragmaster [options]

 Create EPS and PDF files with embedded psfrag substitutions.

 Options:
  -h,--help          Brief usage guide.
  -m,--man           Show full man page (needs perldoc)
  --debug            Show more info and leave temporary files behind.
  --force            Rebuild everything ignoring modification times.
  --dirfm=file       Use given file as optional per-directory
                     fragmaster fm control file instead of default
                     "fragmaster.dfm".

=head1 DESCRIPTION

B<fragmaster> is a perl script that helps using psfrag constructs with
B<pdflatex>.

B<psfrag> is a LaTeX package which allows to replace text elements in
included EPS graphics by arbitrary LaTeX output. Because B<psfrag>
uses Postscript for making the replacements, in principle you can't
use B<psfrag> with B<pdflatex> which doesn't have any interfaces to
postscript.

B<fragmaster> produces a new EPS from your original EPS which already
contains all those B<psfrag> replacements. This new EPS graphic
actually can be converted to PDF including all replacements. The
resulting "encapsulated" PDF can then be used with pdflatex.

B<fragmaster> will scan the current directory for files which end in
F<_fm> and have a F<_fm.eps> counterpart.
Looking at the modification dates, the script checks if the output
files have to be rebuilt and does so if necessary
(a little like "make" would do it).

In your LaTeX document you can include the produced graphics using

 \includegraphics{<graphics>}

conveniently omitting file extension.
B<latex> will choose the EPS, B<pdflatex> will choose the PDF.

=head2 B<fragmaster> control file and other related files.

To use the script you have to create two files per graphic:

    * <graphics>_fm.eps: the EPS file itself,
    * <graphics>_fm: a fragmaster control file.

From these files the psfragged graphics will be created:

    * <graphics>.eps,
    * <graphics>.pdf

The F<_fm> control file is basically a LaTeX file
(with optionally special comments) and can look like this:

 % Just an ordinary comment
 %
 % Some special comments:
 % fmclass: book
 % fmclassopt: 11pt
 % fmopt: width=6cm
 %
 % Another special comment:
 % head:
 % \usepackage{amsmath}
 % end head

 % psfrag commands:
 \psfrag{x}{$x$}
 \psfrag{y}{$y = x^2$}

Special comment C<fmclass:> will make the script use given class
instead of default C<article> class.

Special comment C<fmclassopt:> will make the script use given options
as class options instead of default C<12pt>.

The special comment C<fmopt:> will be evaluated such that the
following text will by passed as optional argument to
C<\includegraphics>.
This way you can e.g. adjust the relation between graphics size
and font size using something like C<fmopt: width=6cm>.
No global default for this.

The special comment construct C<head:/end head> causes the lines in
between to be included in the preamble of the LaTeX temporary document
after having the leading comment characters "%" stripped off.
This way, you can include LaTeX packages, as in C<\usepackage{amsmath}>.
No global default for this.

=head2 Per-directory B<fragmaster> dir control file.

You can set per-directory C<fmclass:>, C<fmclassopt:>, C<fmopt:> and
C<head:/end head> options by means of a per-directory fragmaster control
file F<fragmaster.dfm> with similar syntax as above.
You can use another file by means of the B<--dirfm> option.
Note that options set this way are mutually exclusive, any option set
in per-file F<_fm> file will completely override associated option in
per-directory file, and options set in per-directory file will
override initial defaults (C<\documentclass[12pt]{article}>).
Empty options are ignored.

This is work in progress and still needs extensive checking. Double-check
that modification date based rebuilds are working properly.

=head1 KNOWN PROBLEMS

In case the EPS will be produced as landscape graphics, i.e. B<gv> shows
I<Landscape> instead of I<Portrait> in the menu bar, and the graphic will
end up turned around 90 degrees in your document, then it is likely
that your original EPS is wider than it is tall.
In this case some (more recent) versions of B<dvips> make the "smart"
assumption that your graphic is landscape, even though the graphic's
proportions don't tell anything about the orientation of its contents...
This still can happen in case your input EPS matches a standard paper size.

Anyway, you can make B<dvips> behave nicer by specifying the following
line in F</usr/share/texmf/dvips/config/config.pdf>
(or a local equivalent inside F</usr/local/share/texmf>):

@ custom 0pt 0pt

In the likely case that you're wondering why, I'd recommend the
B<dvipsk> sources warmly to you...

=head1 AUTHOR

Tilman Vogel <tilman vogel web de> (dot at dot) and
Agustin Martin <agustin martin hispalinux es> (dot at dot)

=head1 HISTORY

This script was inspired by a posting from
Karsten Roemke <k roemke gmx de> (dot at dot) with subject
"psfrag pdflatex, lange her"
in de.comp.text.tex on 2003-11-11 05:25:44 PST.

Karsten Roemke was inspired for his solution by postings from
Thomas Wimmer.

=head1 COPYRIGHT

 Copyright (C) 2004 Tilman Vogel
 Copyright (C) 2011 Agustin Martin

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
