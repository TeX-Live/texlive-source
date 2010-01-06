eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;

use strict;				# to be sure, that all is safe ... :-)

# v. 0.06				simplify the use of PSTricks with pdf
# 2007-08-28	(c) hvoss@tug.org
use File::Path;                         # creating/removing dirs
use File::Basename;			# scan argument
use IO::File;                           # simple IO operation
use Getopt::Long;			# read parameter

#----------------------- User part begin ------------------------
my $imageDir = "images";		# where to save the images
#my @imageType = ("eps","pdf","png");	# all image types (png only for Linux)
my @imageType = ("eps","pdf");		# all image types
my $Iext = ".png";			# leave empty, if not a special one
my $tempDir = ".";			# temporary directory
my $verbose = 1;			# 0 or 1, logfile  
my $clear = 0;				# 0 or 1, clears all temporary files
my $DPI = 75;				# very low value
my $Iscale = 1;				# for \includegraphics
my $noImages = 0;			# 1->create no images
#----------------------- User part end ---------------------------

@ARGV == 0 && die "file name expected!\n";

my @SuffixList = (".tex","",".ltx");	# possible extensions
my ($name,$path,$ext) = fileparse($ARGV[0],@SuffixList);
if ( $ext eq "" ) { $ext = ".tex"; }	# me need the extension as well
my $TeXfile = "$path$name$ext";
my $Logfile = "$tempDir/$name.plog";	# our own log file
open (LOGfile, ">$Logfile") or die "cannot open $Logfile!";

my $result = GetOptions ("DPI=i"      => \$DPI,      # numeric
                         "Iscale=f"   => \$Iscale,   # real
                         "imageDir=s" => \$imageDir, # string
                         "tempDir=s"  => \$tempDir,  # string
                         "Iext=s"     => \$Iext,     # string
                         "clear"      => \$clear,    # flag
                         "noImages"   => \$noImages, # flag
                         "verbose"    => \$verbose); # flag

LOG ("Parameters:"); 
LOG ("==> imageDir = $imageDir"); 
LOG ("==> Iext     = $Iext"); 
LOG ("==> DPI      = $DPI"); 
LOG ("==> Iscale   = $Iscale"); 
LOG ("==> tempDir  = $tempDir"); 
LOG ("==> verbose  = $verbose"); 
LOG ("==> clear    = $clear"); 
LOG ("==> noImages = $noImages"); 

my $imgNo = 0;				# internal image counter

LOG ("Running on [$path][$name][$ext]"); 
open (FILE, "<$TeXfile") or die "cannot open source file $TeXfile!";	# the source
if ( !$noImages ) {
  if (-d $imageDir) { LOG ("$imageDir exists") }
  else { mkdir("$imageDir", 0744) || die "cannot mkdir $imageDir: $!";
    LOG ("Imagedir created"); }

  LOG ("go to savePreamble ... "); 
  savePreamble($name);
  LOG ("done!\n go to runFile ..."); 
  runFile($name);
  LOG ("done!"); 
  close FILE;						# close source file
}# !noImages


LOG ("runpdfTeX ... "); 
runpdfTeX("$path$name",$name);
LOG ("all finished ... :-)"); 
close LOGfile;
if ( $clear ) {
  unlink "$path$name.aux";
  unlink "$path$name.log";
  unlink "$path$name.preamble";
  unlink "$path$name-tmp.aux";
  unlink "$path$name-tmp.dvi";
  unlink "$path$name-tmp.log";
  unlink "$path$name-tmp.pdf";
  unlink "$path$name-tmp.ps";
  unlink "$path$name-tmp.tex";
}

sub savePreamble {			# create a preamble file
# if we have a \input command inside the preamble, it doesn't hurt, we need
# it anyway for the postscript files and the pdf one.
  my $filename = pop;			# get the file name
  LOG ("----- Start Preamble -----"); 
  open (FILEp, ">$tempDir/$filename.preamble") 
    or die "cannot open preamble file $tempDir/$filename.preamble!";
  while (<FILE>) {			# read all until \begin{document}
    my $i = index($_,"begin{document}");
    if ($i > 0) { 
      if ($i > 1) { print FILEp substr($_,0,--$i); }	# write all until \begin{document}
      close(FILEp);  					# close preamble
      LOG ("----- Close Preamble ------"); 
      return; 
    } else { 
      print FILEp "$_"; 				# write into preamble
      LOG ("$_"); 
    }
  }
  close(FILEp);
  if ( $verbose ) { LOG("<-----Preamble<----"); }
  return;
}

sub searchPS {				# search the PostScript parts
  my @PS = ();				# single PS sequence
  my @PStotal = ();			# all PS sequences as list of arrays
  my $depth = -1;			# counts nested macros
  my $type = -1;			# -1-> none; 1->PST; 2->PS; 
  my $EndDocument = 0;			# ignore all after \end{document}
  my $iVerb = 0;			# test for verbatim or lstlisting environment, must be ignored
  while (<FILE>) {			# scan the input file
    if (!$EndDocument) {
    chomp;
    my $line = $_; 
    if ( !$iVerb ) { 
      $iVerb = ((index($line,"begin{verbatim}") > 0) or (index($line,"begin{lstlisting}") > 0)); 
    } # do nothing until \end{verbatim}
    if ( !$iVerb ) {
      my $iPS = index($line,"begin{postscript}");
      my $iPST = index($line,"begin{pspicture}");
      if ($iPST < 0) { $iPST = index($line,"pspicture"); }	# alternative \pspicture...
      if (($iPS > 0) && ( $type == 1 )){ print "postscript environment must be outer level!\n"; exit 1; }
      if ( $type < 0 ) {			# no active environment
        if ($iPS > 0) { 			# we have \begin{postscript}
          $type = 2; 			
          $line = substr($line,$iPS-1);	# add rest of the line
  	  LOG("PS-Zeile: $line");
        } 				
        elsif ( $iPST > 0 ) { 		# we have \begin{pspicture} or \pspicture
          $type = 1; 
	  $depth++;  
          $line = substr($line,$iPST-1);	# add all unitl pspicture
	  LOG("PST-Zeile: $line");
        }
      }
#	we have now \begin{pspicture} or \begin{postscript}
      if ($type > 0) {			# start Scan, we have an environment
        LOG ("searchPS: set \$type=$type"); 
        $iPST = index($line,"end{pspicture}");
        if ($iPST < 0) { $iPST = index($line,"endpspicture"); }	# alternative \endpspicture...
        $iPS = index($line,"end{postscript}");	
        if ($iPST > 0) {		# test, we can have postscript and pspicture in one line
          if ( $type < 2) {		# found end of pspicture environment 
            LOG ("searchPS: $line"); 
	    $depth--; 
	    if ($depth < 0) { 
	      $type = -1; 
	      if (index($line,"endpspicture") > 0) 		# add line, depends to type
	           { push @PS,substr($line,0,$iPST+12); }	# \endpspicture
	      else { push @PS,substr($line,0,$iPST+14); }	# \end{pspicture} 
              LOG ("searchPS: set \$type=$type"); 
              push @PStotal,[@PS];	# add PS sequence
              LOG ("---->PS---->\n@PS\n<----PS<----"); 
	      @PS = ();			# start new PS sequence
	    }				# no pspicture env left
	  } else { push @PS,$line; }	# pspicture inside postscript
        } elsif ($iPS > 0) { 		# must be type=1 -> stop Scan
          LOG ("searchPS: $line"); 
	  $type = -1;
    	  push @PS,substr($line,0,$iPS+15);# add line
          LOG ("searchPS: set \$type=$type"); 
          push @PStotal,[@PS];		# add PS sequence
          LOG ("---->PS---->\n@PS\n<----PS<----"); 
	  @PS = ();			# start new PS sequence
        } else { push @PS,$line; }		# add line
      }
      my $i = index($line,"end{document}");
      if ($i > 0) { $EndDocument++; LOG("EndDocument in searchPS"); }
    } # if ( $iVerb )
    if (( index($line,"end{verbatim}") > 0 ) or ( index($line,"end{lstlisting}") > 0 )) { $iVerb = 0; }
  }}
  if ( $verbose ) { 
    LOG("---->PStotal---->");
    for my $aref ( @PStotal ) { 
      my @a = @$aref;
      my $i = 0;
      foreach ( @a ) { LOG ($a[$i]); $i++; }
    }
    LOG ("<----PStotal<----"); 
  }
  close(FILE);
  return @PStotal;			# return all PS sequences
}

sub runTeX {
  my $filename = pop;
  system("latex $filename");	
  system("dvips $filename");
  system("ps2pdf $filename.ps");
  for my $Itype ( @imageType ) {
    if ($Itype eq "pdf") { system("pdfcrop $filename.pdf $imageDir/$filename-$imgNo.pdf");  }
    if ($Itype eq "eps") { system("pdftops -f 1 -l 1 -eps $imageDir/$filename-$imgNo.pdf $imageDir/$filename-$imgNo.eps"); }
    if ($Itype eq "png") { 
      system("pdftoppm -f 1 -l 1 -r $DPI $imageDir/$filename-$imgNo.pdf $imageDir/"); 
      system("convert $imageDir/-000001.ppm  $imageDir/$filename-$imgNo.png"); 
      system("rm $imageDir/-00001.ppm"); 
    }
  }
  $imgNo++;
}

sub runFile {
  my $filename = pop;
  my @PSarray = searchPS();
  if ( $verbose ) { 
    LOG("---->PSarray---->");
    for my $aref ( @PSarray ) { 
      my @a = @$aref;
      my $i = 0;
      foreach ( @a ) { print LOG $a[$i]."\n"; $i++; }
    }
    LOG("<----PSarray<----"); 
    my $no = @PSarray;
    LOG("PS: ".$no." PS sequence(s)"); 
  }
  for my $aref ( @PSarray ) {
    my @PS = @$aref;
    open (FILEp, "<$tempDir/$filename.preamble") or die "cannot open $tempDir/$filename.preamble!";
    open (FILEsub, ">$tempDir/$filename-tmp.tex") or die "cannot open $tempDir/$filename-tmp.tex!";
    while (<FILEp>) { print FILEsub $_; }
    print FILEsub "\\pagestyle{empty}\n";
    print FILEsub "\\newenvironment{postscript}{}{}\n";
    print FILEsub "\\begin{document}\n";
    if ( $verbose ) { LOG("\@PS: $_"); }
    foreach ( @PS ) { print FILEsub "$_\n"; }
    print FILEsub "\\end{document}\n";
    close (FILEsub);
    close (FILEp);
    runTeX("$tempDir/$filename-tmp");
  }
}

sub runpdfTeX() {
  my ($name,$pdfname) = @_;
  open (PDF, ">$tempDir/$pdfname-pdf.tex") or die "cannot open $tempDir/$pdfname-pdf.tex!";
  open (FILE, "<$name.tex") or die "cannot open $name!";
  print PDF "\\RequirePackage{graphicx}\n";
#  print PDF "\\setkeys{Gin}{scale=0.25}\n";
  print PDF "\\RequirePackage[pdftex]{xcolor}\n";
#  print PDF "\\graphicspath{{$imageDir/}}\n";
  my $ignore = 0;
  my $IMGno = 0;
  my $depth = -1;
  my $type = -1;
  my $EndDocument = 0;			# ignore all after \end{document}
  my $iVerb = 0;
  while (<FILE>) {			# scan the input file
    if ( !$iVerb ) { 
      $iVerb = ((index($_,"begin{verbatim}") > 0) or (index($_,"begin{lstlisting}") > 0)); 
    } # do nothing until \end{verbatim}|| \end{lstlisting}
    if ( !$iVerb ) {
      my $i = index($_,"end{document}");
      if ($i > 0) { print PDF $_; $EndDocument++; LOG("EndDocument in runpdfTeX"); }
      if ( !$EndDocument ) {
        my $iPS = index($_,"begin{postscript}");
        if ( $iPS > 0 ) { 
          $type = 2; 
          $ignore = 1; 
          if ($iPS > 1) { print PDF substr($_,0,--$iPS); }	# add preceeding text
          print PDF "\\includegraphics[scale=$Iscale]{$imageDir/$name-tmp-$IMGno$Iext}"; 
#          print PDF "\\includegraphics{$name-tmp-$IMGno$Iext}"; 	# use \graphicspath
          $IMGno++;
        }		# postscript env
        if ( $type < 2 ) {
          my $iPST = index($_,"begin{pspicture}");
          if ($iPST < 0) { $iPST = index($_,"\\pspicture"); }	# alternative \endpspicture...
          if ( $iPST >= 0 ) {	 			# start Scan
            $ignore = 1;
            $type = 1; 
            $depth++; 					# pspicture env
	    LOG("Increase depth: $depth");
	    if ( $depth == 0 ) {
              if ($iPST > 1) { print PDF substr($_,0,--$iPST); }# add preceeding text
              print PDF "\\includegraphics[scale=$Iscale]{$imageDir/$name-tmp-$IMGno$Iext}"; 
#              print PDF "\\includegraphics{$name-tmp-$IMGno$Iext}"; 	# use \graphicspath
              $IMGno++;
	      LOG("Increase Image counter: $IMGno");
            }
          }
        }
        if ( !$ignore ) { print PDF "$_"; }			# default line
        if ( $type == 2 ) {					# postscript env
          my $iPS = index($_,"end{postscript}");
          if ($iPS > 0) {
            print PDF substr($_,$iPS+15);			# rest of line
            $ignore = 0; 
          }							# end Scan
        } elsif ( $type == 1 ) {				# pspicture env
          my $iPST = index($_,"end{pspicture}");
          if ($iPST < 0) { $iPST = index($_,"endpspicture"); }	# alternative \endpspicture...
          if ($iPST > 0) {		 			# end Scan
	    if (index($_,"endpspicture") > 0) 		# add rest of line, depends to type
	       { print PDF substr($_,$iPST+12); }	# \endpspicture
	    else { print PDF substr($_,$iPST+14); }	# \end{pspicture} 
            $depth--; 
	    LOG("Decrease depth: $depth");
            if ($depth < 0) { $ignore = 0; }
          }
        }
      } # if ( !$EndDocument ) 
    } else { print PDF $_; } # if ( $iVerb )
    if (( index($_,"end{verbatim}") > 0 ) or ( index($_,"end{lstlisting}") > 0 )) { $iVerb = 0; }
  } # while (<FILE>)
  close (FILE);
  close (PDF);
  system("pdflatex $tempDir/$pdfname-pdf");	
  system("makeindex $tempDir/$pdfname-pdf.idx");	
  system("bibtex $tempDir/$pdfname-pdf");	
  system("pdflatex $tempDir/$pdfname-pdf");
  if ( $clear ) {
    unlink "$tempDir/$pdfname-pdf.log";
    unlink "$tempDir/$pdfname-pdf.aux";
  }
}

sub LOG() { 
  if ( $verbose ) { print LOGfile "@_\n"; } 
}

__END__

=head1 NAME

B<PST2pdf> - run a TeX source, and convert all PS-related part as single images
    (pdf and eps)

=head1 SYNOPSIS

  PSTpdf <texfile> 

TODO

=head1 DESCRIPTION

runs latex and pdflatex on the TeX 

=head1 OPTIONS

--imageDir  
--Iext=<.ext> 
--DPI=<int> 
--Iscale=<real> 
--tempDir=<dir> 
--verbose 
--clear 
--noImages 

=head1 AUTHORS

Herbert Voss <hvoss@tug.org>

=head1 COPYRIGHT

Copyright (c) 2007  Herbert Voss <hvoss@tug.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
1;6H