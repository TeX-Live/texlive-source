#!/usr/bin/perl -w

######################################################################
# $Id: fragmaster.pl,v 1.3 2006/09/26 08:59:30 tvogel Exp $
#
# fragmaster.pl
# creates EPS and PDF graphics from source EPS and control files
# with \psfrag commands
#
# Copyright (C) 2004 Tilman Vogel <tilman vogel web de> (dot at dot)
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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# IMPORTANT: ALLOW DVIPS TO MAKE _PORTRAIT_ PS WITH WIDTH > HEIGHT
# BY ADDING
#
# @ custom 0pt 0pt
#
# TO YOUR /usr/share/texmf/dvips/config/config.pdf
# IF THIS ENTRY IS MISSING, DVIPS WILL GUESS ORIENTATION FROM
# WIDTH / HEIGHT RATIO. THIS STILL CAN HAPPEN IN CASE YOUR INPUT EPS
# MATCHES A STANDARD PAPER SIZE!
#
# Source files:
#    <graphics>_fm.eps
#       a source EPS file
#    <graphics>_fm
#       a control file containing \psfrag commands and optionally
#       special comments:
#         % fmclass: <class>
#           use <class> instead of "article"
#         % fmclassopt: <options>
#           use <options> as class options instead of "12pt"
#         % head:
#         % <texcode>
#         % end head
#           causes <texcode> to be put into the preamble
#         % fmopt: <option>
#           causes <option> to be given to \includegraphics as
#           optional parameter
#
# fragmaster.pl scans the current directory for files matching the
# pattern "*_fm" and "*_fm.eps" and converts them to the respective
# ".eps"- and ".pdf"-files if they are outdated.
#
# Credits:
#
# This script was inspired by a posting from
#   Karsten Roemke <k roemke gmx de> (dot at dot)
# with subject
#   "psfrag pdflatex, lange her"
# in de.comp.text.tex on 2003-11-11 05:25:44 PST.
#
# Karsten Roemke was inspired for his solution by postings from
# Thomas Wimmer.

chomp($cwd = `pwd`);

die "Current path contains whitespace. I am sorry, but LaTeX cannot handle this correctly, move somewhere else. Stopped"
    if $cwd =~ /\s/;

foreach $fm_file (<*_fm>) {
  ($base = $fm_file) =~ s/_fm$//;
  $source = "$fm_file.eps";

  if(! -f $source) {
    print "Cannot find EPS file '$source' for fragmaster file '$fm_file'! Skipped.\n";
    next;
  }

  
  $dest_eps = "$base.eps";
  $dest_pdf = "$base.pdf";
  
  $do_it = 0;

  $do_it = 1
    if ! -f $dest_eps;
  $do_it = 1
    if ! -f $dest_pdf;

  if(! $do_it) {
    $oldest_dest = -M $dest_eps;
    $oldest_dest = -M $dest_pdf
      if -M $dest_pdf > $oldest_dest;
  
    $youngest_source = -M $fm_file;
    $youngest_source = -M $source
      if -M $source < $youngest_source;
    $do_it = 1
      if $oldest_dest > $youngest_source;
  }
  
  if( $do_it ) {
    print "$fm_file, $source -> $dest_eps, $dest_pdf\n";

    open FMFILE, "<$fm_file"
      or die "Cannot read $fm_file!";

    $tempdir = `mktemp -d fm-$base.XXXXXX`
      or die "Cannot make temporary directory!";

    chomp($tempdir);

    print "using $tempdir/\n";

    open TEXFILE, ">$tempdir/fm.tex"
      or die "Cannot write LaTeX file!";

    $fmopt = "";
    @fmfile = ();
    @fmhead = ();
    $fmclass = "article";
    $fmclassopt = "12pt";
    while (<FMFILE>) {
      chomp;
      $fmopt = $1 if /fmopt:(.*)/;
      $fmclass = $1 if /fmclass:(.*)/;
      $fmclassopt = $1 if /fmclassopt:(.*)/;
      if (/head:/) {
	push @fmfile, "  $_%\n";
	while(<FMFILE>) {
	  chomp;
	  last if /end head/;
	  push @fmfile, "  $_%\n";
	  # Remove comment prefix
	  s/^[\s%]*//;
	  push @fmhead, "$_%\n";
	}
      }
            
      push @fmfile, "  $_%\n";
    }

    print TEXFILE <<"EOF";
\\documentclass[$fmclassopt]{$fmclass}
\\usepackage{graphicx,psfrag,color}
\\usepackage{german}
EOF
  print TEXFILE
    foreach(@fmhead);
  print TEXFILE <<'EOF';
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
EOF
  print TEXFILE "\\graphicspath{{../}}\n";
  print TEXFILE <<'EOF';
\begin{document}
  \begin{lrbox}{\pict}%
EOF

  print TEXFILE
    foreach (@fmfile);

  print TEXFILE "  \\includegraphics[$fmopt]{$source}%\n";
  print TEXFILE <<'EOF';
  \end{lrbox}
  \special{papersize=\the\wd\pict,\the\ht\pict}
  \usebox{\pict}
\end{document}
EOF
    
    close TEXFILE;

    chdir($tempdir)
      or die "Cannot chdir to $tempdir!";

    system("latex fm.tex") / 256 == 0
      or die "Cannot latex fm.tex!";

    # Using -E here, causes dvips to detect
    # the psfrag phantom stuff and to set the BoundingBox wrong
    system("dvips -E -P pdf fm.dvi -o fm.ps") / 256 == 0
      or die "Cannot dvips!";
    
    chdir("..")
      or die "Cannot chdir back up!";

    open PS, "<$tempdir/fm.ps"
      or die "Cannot read fm.ps!";

    open EPS, ">$dest_eps"
      or die "Cannot write $dest_eps!";

    # Correct the bounding box by setting the left margin to 0
    # top margin to top of letterpaper!
    # (I hope that is general enough...)
    $saw_bounding_box = 0;
    while(<PS>) {
      if(! $saw_bounding_box) {
	# if(s/^\%\%BoundingBox:\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/\%\%BoundingBox: 0 $2 $3 $4/) {
	if(s/^\%\%BoundingBox:\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/\%\%BoundingBox: 0 $2 $3 792/) {
	  $saw_bounding_box = 1;
	}
      }
      print EPS;
    }
    
    # Not using -E above causes
    # papersizes to be included into the PS
    # Strip off the specifications.
    # Otherwise gv doesn't show the BBox
    # and epstopdf won't detect the correct
    # PDF media size!
    
    #     while(<PS>) {
    #       s/^%!PS-Adobe.*/%!PS-Adobe-3.0 EPSF-3.0/;
    
    #       next if /^\%\%DocumentPaperSizes:/;
    #       if(/^\%\%BeginPaperSize:/) {
    # 	while(<PS>) {
    # 	  last if /^\%\%EndPaperSize/;
    # 	}
    # 	next;
    #       }
    #       s/statusdict \/setpage known/false/;
    #       s/statusdict \/setpageparams known/false/;
    #       print EPS;
    #     }

    close EPS;
    close PS;

    system("epstopdf $dest_eps --outfile=$dest_pdf") / 256 == 0
      or die "Cannot epstopdf!";

    system("rm -rf $tempdir") / 256 == 0
      or die "Cannot remove $tempdir!";

    close FMFILE;

    
    
  }
}
