eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;

use strict;				# to be sure, that all is safe ... :-)

# $Id: pst2pdf.pl 611 2011-12-14 08:41:35Z herbert $
# v. 0.14				simplify the use of PSTricks with pdf
# 2011-12-12	(c) Herbert Voss <hvoss@tug.org> 
#                 Pablo González Luengo
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA  02111-1307  USA
#
use File::Path;               # creating/removing dirs
use File::Copy;               # copying files
use File::Basename;						# scan argument
use IO::File;                 # simple IO operation
use Getopt::Long;							# read parameter
use autodie;
#----------------------- User part begin ------------------------
my $imageDir = "images";# where to save the images
my $Iext = ".pdf";			# leave empty, if not a special one
my $tempDir = ".";			# temporary directory
my $verbose = 0;			  # 0 or 1, logfile  
my $clear = 0;				  # 0 or 1, clears all temporary files
my $DPI = 75;				    # very low value for the png's
my $Iscale = 1;			 	  # for \includegraphics
my $noImages = 0;			  # 1->create no images
my $single = 0;				  # 1->single create images
my $runBibTeX = 0;			# 1->runs bibtex
my $runBiber = 0;			  # 1->runs biber and sets $runBibTeX=0
my $ppm = 0;				    # 1->create .ppm files
my $norun = 0;				  # 1->runs pdflatex
my $eps = 0;				    # 1->create .eps files
my $files = 0;				  # 1->create image files .tex 
my $all = 0;				    # 1->create all images and files for type
my $nopdftk = 0;			  # 1->create all images and files for type in single mode
my $xetex = 0;				  # 1->Using XeLaTeX for compilation.
#----------------------- User part end ---------------------------
#-------------- program identification, options and help ---------
my $program = "pst2pdf";
my $ident = '$Id: pst2pdf.pl 611 2011-12-14 08:41:35Z herbert $';
my $copyright = <<END_COPYRIGHT ;
Copyright 2011-01-18 (c) Herbert Voss <hvoss\@tug.org> 
END_COPYRIGHT
my $licensetxt= <<END_LICENSE ;
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful, but 
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
    MA  02111-1307  USA
END_LICENSE
my $title = "$program $ident\n";
my $usage = <<"END_OF_USAGE";
${title}Usage: $program <texfile.tex>  [Options]
pst2pdf run a TeX source, and convert all PS-related part as 
	single images  (pdf or eps or ppm, default pdf) 
	and then runs pdflatex. See pst2pdf documentation for more info
Options:
  --help          - display this help and exit
  --version       - display version information and exit
  --license       - display license information and exit
  --imageDir      - the dir for the created images (default images) 
  --DPI=<int>     - the dots per inch for a cretaed ppm files (default 75)
  --ppm           - create .ppm files
  --eps	  	      - create .eps files	
  --Iscale=<real> - the value for [scale=] in \\includegraphics 
  --noImages      - generate files without compile (need -norun)
  --verbose       - creates long log
  --clear         - delete all temp files
  --norun         - create file-pdf.tex, but, no run pdflatex
  --runBibTeX     - run bibtex on the aux file, if exists
  --runBiber      - run biber on the bcf file, if exists
  --files         - create images files (.tex) for all pst enviroment	
  --single	  - create images whitout pdftk and auto-pst-pdf
  --all	      	  - create all image type and images.tex	
  --nopdftk       - create all image type and images.tex in single mode
  --xetex         - using XeLaTeX for compilation.
Examples:
* $program test.tex --all
* produce test-pdf.tex and ppm,eps,tex and pdf for pst-enviroment in image dir
END_OF_USAGE
#
my $result=GetOptions (
  	"help",
  	"version",
  	"license",
		"DPI=i"      => \$DPI,      	# numeric
		"Iscale=f"   => \$Iscale,   	# real
		"imageDir=s" => \$imageDir, 	# string
		"tempDir=s"  => \$tempDir,  	# string
		"Iext=s"     => \$Iext,     	# string
		"clear"      => \$clear,    	# flag
		"noImages"   => \$noImages, 	# flag
		"single"     => \$single, 	# flag
		"runBibTeX"  => \$runBibTeX,    # flag
		"ppm"        => \$ppm,          # flag
		"norun"      => \$norun,        # flag
		"eps"        => \$eps,		# flag
		"files"      => \$files,	# flag
		"all"        => \$all,		# flag
		"nopdftk"    => \$nopdftk,	# flag
		"xetex"      => \$xetex,	# flag
		"runBiber"   => \$runBiber, 	# flag
		"verbose"    => \$verbose,
) or die $usage;
#---------------- help functions --------------------------------
sub errorUsage { die "Error: @_ (try --help for more information)\n"; }
# options for command line
if ($::opt_help) {
print $usage;
		exit (0);
}
if ($::opt_version) {
  print $title;
  print $copyright;
  exit (0);
}
if ($::opt_license) {
  print $licensetxt;
  exit (0);
}
# open file 
my $InputFilename = "";
  @ARGV > 0 or errorUsage "Input filename missing";
  @ARGV < 2 or errorUsage "Unknown option or too many input files";
  $InputFilename = $ARGV[0];

my @SuffixList = (".tex","",".ltx");		# possible extensions
my ($name,$path,$ext) = fileparse($ARGV[0],@SuffixList);
if ($ext eq "") { $ext = ".tex"; }			# me need the extension as well
my $TeXfile = "$path$name$ext";
open  my $LOGfile,'>', "$tempDir/$name.plog"; # our own log file
	LOG ("Parameters:");
	LOG ("==> imageDir = $imageDir"); 
	LOG ("==> Iext     = $Iext"); 
	LOG ("==> DPI      = $DPI"); 
	LOG ("==> Iscale   = $Iscale"); 
	LOG ("==> tempDir  = $tempDir"); 
	LOG ("==> verbose  = $verbose"); 
	LOG ("==> clear    = $clear"); 
	LOG ("==> noImages = $noImages"); 
	LOG ("==> single   = $single"); 
	LOG ("==> runBibTeX= $runBibTeX"); 
	LOG ("==> runBiber = $runBiber"); 
	LOG ("==> ppm      = $ppm"); 
	LOG ("==> norun    = $norun"); 
	LOG ("==> eps      = $eps");  
	LOG ("==> files    = $files"); 
	LOG ("==> xetex    = $xetex");

if ($runBibTeX && $runBiber) {
	LOG ("!!! you cannot run BibTeX and Biber at the same document ...");
	LOG ("!!! Assuming to run Biber");
  $runBibTeX = 0;
}
if ($ppm) {
  LOG ("Generate .ppm files ...");
  $ppm = 1;
}
if ($norun) {
  LOG ("no compile $name-pdf.tex");
  $norun = 1;
}
if ($eps) {
  LOG ("Generate .eps files ...");
  $eps = 1;
}
if ($files) {
  LOG ("Generate .tex images files ...");
  $files=1;
}
if ($all) {
  LOG ("Generate all images files ...");
   $files=$eps = $ppm = $clear = 1;
}
if ($nopdftk) {
  LOG ("single mode generate all images files ...");
  $single=$files=$eps = $ppm = $clear = 1;
}
if ($xetex) {
  LOG ("Compiling using XeLaTeX ...");
  $xetex=1;
}

my $imgNo = 1;				# internal image counter
#----------- single mode no need pdftk and auto-pst-pdf ----------------------
if ($single) {
LOG ("Running on [$path][$name][$ext]"); 
open my $FILE,'<', "$TeXfile" ;	# the source
LOG ("single mode generate images...");
if (-d $imageDir) { LOG ("$imageDir exists") }
else { mkdir("$imageDir", 0744) ;}
savePreamble($name);
runFile($name);
close $FILE;				# close source file
close $LOGfile;
}
#---------------- end single --------------
else{
LOG ("Running on [$path][$name][$ext]"); 
open my $FILE,'<', "$TeXfile" ;	# the source
if (!$noImages ) {
if (-d $imageDir) { LOG ("$imageDir exists") }
else { mkdir("$imageDir", 0744) ;
LOG ("Imagedir created"); }
LOG ("go to savePreamble ... "); 
runBurst($tempDir);
savePreamble($name);
runFile($name);
LOG ("done!\n go to runFile ..."); 
LOG ("done!"); 
close $FILE;			# close source file
close $LOGfile;
}# !noImages
}
#------------ Create filename-pics.pdf, split and generate .ppm
sub runBurst{
if ($single){ print "single mode";}
else{
my $entrada = "$TeXfile";
my $salida  = "$name-pics.tex";
		open my $ENTRADA,'<', "$entrada";
		open my $SALIDA,'>',"$salida";
		print $SALIDA "\\AtBeginDocument\{\n";
if ($xetex){
	  print $SALIDA "\\RequirePackage\[xetex,active,tightpage\]\{preview\}\n";}
else{
	  print $SALIDA "\\RequirePackage\[active,dvips,tightpage\]\{preview\}\n";}
	  print $SALIDA "\\renewcommand\\PreviewBbAdjust\{-600pt -600pt 600pt 600pt\}\n";
	  print $SALIDA "\\newenvironment{postscript}{}{}\n";
	  print $SALIDA "\\PreviewEnvironment\{postscript\}\n";
	  print $SALIDA "\\PreviewEnvironment\{pspicture\}\}\n";
    while ( my $linea = <$ENTRADA> ) {
	  print $SALIDA $linea;}
    close $ENTRADA;
    close $SALIDA;
# --------------------Compiling File --------------------------
if ($xetex){
	system("xelatex -interaction=batchmode $tempDir/$name-pics.tex");
  system("pdfcrop -xetex $tempDir/$name-pics.pdf $tempDir/$name-pics.pdf");}
else{
	system("latex -interaction=batchmode $tempDir/$name-pics.tex");
	system("dvips $tempDir/$name-pics.dvi");
	system("ps2pdf -dProcessColorModel=/DeviceCMYK -dPDFSETTINGS=/prepress $tempDir/$name-pics.ps");
  system("pdfcrop $tempDir/$name-pics.pdf $tempDir/$name-pics.pdf");}
	system("pdftk $tempDir/$name-pics.pdf burst output $imageDir/$name-fig-\%1d.pdf");
if ($ppm){
	system("pdftoppm -r  $DPI $tempDir/$name-pics.pdf $imageDir/$name-fig");}
		}
}
#------------ end pdftk burst
LOG ("runpdfTeX ... "); 
runpdfTeX("$path$name",$name);
LOG ("all finished ... :-)"); 
# create a preamble file
# if we have a \input command inside the preamble, it doesn't hurt, we need
# it anyway for the postscript files and the pdf one.
# ----------------- Save Preamble ----------------------------------------
sub savePreamble {				
my $filename = pop;					# get the file name
	LOG ("----- Start Preamble -----"); 
open my $FILEp,'>',"$tempDir/$filename.preamble";
open my $FILE,  '<', "$name.tex";
	while (<$FILE>) {					# read all until \begin{document}
    	my $i = index($_,"begin{document}");
	if ($i > 0) { 
      	if ($i > 1) { print $FILEp substr($_,0,--$i); }	# write all until \begin{document}
	if ($single) {
		print $FILEp "\\newenvironment{postscript}{}{}\n";
		print $FILEp "\\pagestyle{empty}\n";
		    }
close $FILEp;						# close preamble
	LOG ("----- Close Preamble ------"); 
      return; 
    } 
else { 
		print $FILEp "$_";	# write into preamble
	LOG ("$_"); 
    }
}
close $FILE;
close $FILEp;
  if ( $verbose ) { LOG("<-----Preamble<----"); }
  return;
}

sub searchPS {				# search the PostScript parts
  my @PS = ();				# single PS sequence
  my @PStotal = ();		# all PS sequences as list of arrays
  my $depth = -1;			# counts nested macros
  my $type = -1;			# -1-> none; 1->PST; 2->PS; 
  my $EndDocument = 0;# ignore all after \end{document}
  my $iVerb = 0;			# test for verbatim or lstlisting environment, must be ignored
	open my $FILE, '<', "$name.tex";
  while (<$FILE>) {		# scan the input file
		if (!$EndDocument) {
    chomp;					  # delete EOL character
	my $line = $_; 			# save line
    if ( !$iVerb ) { 
      $iVerb = ((index($line,"begin{verbatim}") > 0) 
			or (index($line,"begin{lstlisting}") > 0) 
			or (index($line,"begin{Verbatim}") > 0) 
			or (index($line,"begin{verbatim*}") > 0)); 
    } 					      # do nothing until \end{verbatim}
    if ( !$iVerb ) {
	my $iPS = index($line,"begin{postscript}");
	my $iPST = index($line,"begin{pspicture*}");
		if ($iPST < 0) { $iPST = index($line,"begin{pspicture}"); }	# alternative 
		if ($iPST < 0) { $iPST = index($line,"pspicture"); }		# alternative \pspicture...
		if (($iPS > 0) && ( $type == 1 )){ print "postscript environment must be of outer level!\n"; exit 1; }
		if ( $type < 0 ) {	# no active environment
		if ($iPS > 0) { 		# we have \begin{postscript}
			$type = 2; 			
			$line = substr($line,$iPS-1);	# add rest of the line
  	  LOG("PS-Zeile: $line");
        } 				
		elsif ( $iPST > 0 ) { 		      # we have \begin{pspicture} or \pspicture
			$type = 1; 
			$depth++;  
			$line = substr($line,$iPST-1);# add all unitl pspicture
			LOG("PST-Zeile: $line");
        }
      }
# we have now \begin{pspicture} or \begin{postscript}
      if ($type > 0) {					# start Scan, we have an environment
        LOG ("searchPS: set \$type=$type"); 
        $iPST = index($line,"end{pspicture*}");
        if ($iPST < 0) { $iPST = index($line,"end{pspicture}"); }	# alternative
        if ($iPST < 0) { $iPST = index($line,"endpspicture"); }		# alternative \endpspicture...
        $iPS = index($line,"end{postscript}");	
        if ($iPST > 0) {				# test, we can have postscript and pspicture in one line
          if ( $type < 2) {				# found end of pspicture environment 
            LOG ("searchPS: $line"); 
	    $depth--; 
	    if ($depth < 0) { 
	      $type = -1; 
	      if (index($line,"endpspicture") > 0) 	# add line, depends to type
	           { push @PS,substr($line,0,$iPST+12); }	# \endpspicture
	      elsif (index($line,"pspicture*") > 0)
 	              { push @PS,substr($line,0,$iPST+15); }# \end{pspicture} 
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
    	  push @PS,substr($line,0,$iPS+15);	# add line
          LOG ("searchPS: set \$type=$type"); 
          push @PStotal,[@PS];			# add PS sequence
          LOG ("---->PS---->\n@PS\n<----PS<----"); 
	  @PS =();			# start new PS sequence
        } else { push @PS,$line; }	# add line
      }
      my $i = index($line,"end{document}");
      if ($i > 0) { $EndDocument++; LOG("EndDocument in searchPS"); }
    } 				# if ( $iVerb )
    if (( index($line,"end{verbatim}") > 0 ) 
		or ( index($line,"end{lstlisting}") > 0 ) 
		or ( index($line,"end{Verbatim}") > 0 ) 
		or ( index($line,"end{verbatim*}") > 0 )) { $iVerb = 0; }
  }}
  if ( $verbose ) { 
    LOG("---->PStotal---->");
    for my $aref ( @PStotal ) { 
      my @a = @$aref;
      my $i = 1;
			foreach ( @a ) { LOG ($a[$i]); $i=$i+1; }
    }
    LOG ("<----PStotal<----"); 
  }
  close $FILE;
  return @PStotal;		# return all PS sequences
}
# Creating ifile.tex and eps, pdf and ppm for images
if ($single){
sub runsingle{
	my $filename = pop;
		if ($xetex){
			system("xelatex -interaction=batchmode $tempDir/$filename-fig"); 
      system("pdfcrop -xetex $tempDir/$filename-fig.pdf $imageDir/$filename-fig-$imgNo.pdf");}
		else{
			system("latex -interaction=batchmode $tempDir/$filename-fig"); 
			system("dvips $tempDir/$filename-fig"); 
			system("ps2pdf -dProcessColorModel=/DeviceCMYK -dPDFSETTINGS=/prepress $tempDir/$filename-fig.ps");
      system("pdfcrop $tempDir/$filename-fig.pdf $imageDir/$filename-fig-$imgNo.pdf");}
		if ($files){
			copy("$filename-fig.tex", "$imageDir/$filename-fig-$imgNo.tex");}
    system("pdfcrop $tempDir/$filename-fig.pdf $imageDir/$filename-fig-$imgNo.pdf");  
		if ($eps) {
		system("pdftops -level3 -eps $imageDir/$filename-fig-$imgNo.pdf $imageDir/$filename-fig-$imgNo.eps");}
		if ($ppm) {
		system("pdftoppm -r $DPI $imageDir/$filename-fig-$imgNo.pdf $imageDir/$filename-fig-$imgNo");}
    $imgNo=$imgNo+1;
	}
}
else{
#----------------- Creating files.tex and .eps for images --------------
sub runTeX{
	my $filename = pop;
		if ($eps){
		system("pdftops -level3 -eps $imageDir/$filename-$imgNo.pdf $imageDir/$filename-$imgNo.eps");
		   }
		if ($files){
			copy("$filename.tex", "$imageDir/$filename-$imgNo.tex");
}
		$imgNo=$imgNo+1;
}
}
sub runFile {
  my $filename = pop;
  my @PSarray = searchPS();
  if ( $verbose ) { 
    LOG("---->PSarray---->");
    for my $aref ( @PSarray ) { 
	my @a = @$aref;
	my $i = 1;
		foreach ( @a ) { print LOG $a[$i]."\n"; $i=$i+1; }
    }
    LOG("<----PSarray<----"); 
	my $no = @PSarray;
		LOG("PS: ".$no." PS sequence(s)"); 
  }
		for my $aref ( @PSarray ) {
	my @PS = @$aref;
	open my $FILEp,'<',"$tempDir/$filename.preamble";
	open my $FILEsub,'>',"$tempDir/$filename-fig.tex";
    while (<$FILEp>) {print $FILEsub $_; }
	print $FILEsub "\\begin{document}\n";
		if ( $verbose ) { LOG("\@PS: $_"); }
    foreach ( @PS ) { print $FILEsub "$_\n"; }
	print $FILEsub "\\end{document}";
	close $FILEsub;
	close $FILEp;
		if ($single) {
	runsingle("$name");
	}
		else{
		runTeX("$tempDir/$name-fig");
  }
}
}
# ----------------Renaming ppm need for correct name
if(!$noImages){
	my $dren = "$tempDir/$imageDir";
	my $fichero = '';
	my $ppmren = '';
	my $renNo = 1;
		if(opendir(DIR,$dren)){
		foreach (readdir DIR){
			$fichero = $_;
		if ( $fichero =~ /($name-fig-)(\d+|\d+[-]\d+).ppm/) {
	my $renNo   = int($2);
	my $newname="$1$renNo.ppm";
		 $ppmren = rename("$dren/$fichero","$dren/$newname");
	}
	}
}
closedir DIR;
}# end renaming
#--------------------------- Replace files----------------------
sub runpdfTeX() {
  my ($name,$pdfname) = @_;
  open my $PDF,'>',"$tempDir/$pdfname-pdf.tex";
  open my $FILE,'<',"$name.tex";
  my $ignore = 0;
  my $IMGno = 1;
  my $depth = -1;
  my $type = -1;
  my $EndDocument = 0;					# ignore all after \end{document}
  my $iVerb = 0;
  	while (<$FILE>) {					# scan the input file
    	if ( !$iVerb ) { 
      $iVerb = ((index($_,"begin{verbatim}") > 0) 
			or (index($_,"begin{lstlisting}") > 0) 
			or (index($_,"begin{Verbatim}") > 0) 
			or (index($_,"begin{verbatim*}") > 0)); 
    } # do nothing until \end{verbatim}|| \end{lstlisting}
    	if ( !$iVerb ) {
	my $i = index($_,"end{document}");
      if ($i > 0) { print $PDF $_; $EndDocument++; LOG("EndDocument in runpdfTeX"); }
      if ( !$EndDocument ) {
	my $iPS = index($_,"begin{postscript}");
			if ( $iPS > 0 ) { 
			$type = 2; 
			$ignore = 1; 
			if ($iPS > 1) { print $PDF substr($_,0,--$iPS); }	# add preceeding text
			print $PDF "\\includegraphics[scale=$Iscale]{$pdfname-fig-$IMGno}"; # use pdfname
          $IMGno=$IMGno+1;
        }		# postscript env
      if ( $type < 2 ) {
	my $iPST = index($_,"begin{pspicture*}");
			if ($iPST < 0) { $iPST = index($_,"begin{pspicture}"); }	# alternative ...
			if ($iPST < 0) { $iPST = index($_,"\\pspicture"); }		# alternative \\pspicture...
			if ( $iPST >= 0 ) {	 					# start Scan
			$ignore = 1;
			$type = 1; 
			$depth++; 							# pspicture env
	    LOG("Increase depth: $depth");
	    if ( $depth == 0 ) {
			if ($iPST > 1) { print $PDF substr($_,0,--$iPST); }	# add preceeding text
					print $PDF "\\includegraphics[scale=$Iscale]{$pdfname-fig-$IMGno}"; 	# use \graphicspath
			$IMGno=$IMGno+1;
			LOG("Increase Image counter: $IMGno");
            }
          }
        }
			if ( !$ignore ) { print $PDF "$_"; }		# default line
			if ( $type == 2 ) {				# postscript env
	my $iPS = index($_,"end{postscript}");
			if ($iPS > 0) {
					print $PDF substr($_,$iPS+15);		# rest of line
			$ignore = 0; 
			$type=-1;
          }						# end Scan
        } 
			elsif ( $type == 1 ) {				# pspicture env
	my $iPST = index($_,"end{pspicture*}");
			if ($iPST < 0) { $iPST = index($_,"end{pspicture}"); }	# alternative ...
			if ($iPST < 0) { $iPST = index($_,"endpspicture"); }		# alternative \endpspicture...
			if ($iPST > 0) {		 				# end Scan
	    if (index($_,"endpspicture") > 0) 		# add rest of line, depends to type
	       { print $PDF substr($_,$iPST+12); }	# \endpspicture
	    elsif (index($_,"pspicture*") > 0)
					{ print $PDF substr($_,$iPST+15); }	# \end{pspicture*} 
	    else{ print $PDF substr($_,$iPST+14); }	# \end{pspicture}
			$depth--; 
	    LOG("Decrease depth: $depth");
      if ($depth < 0) { $ignore = 0; }
          }
        }
      } # if ( !$EndDocument ) 
    } 
			else { print $PDF $_; } # if ( $iVerb )
			if (( index($_,"end{verbatim}") > 0 ) 
			or ( index($_,"end{lstlisting}") > 0 ) 
			or ( index($_,"end{Verbatim}") > 0 ) 
			or ( index($_,"end{verbatim*}") > 0 )) { $iVerb = 0; }
  } # while (<$FILE>)
  close $FILE;
  close $PDF;
#------------------- Coment pst-package for pdfLaTeX --------------------
	open my $IPDF,'<', "$tempDir/$pdfname-pdf.tex"; 
	undef $/;   # read all file
	my ($uno,$dos) = split(/\\begin\{document\}/,<$IPDF>,2);
	close $IPDF;
	
	my @coment = split /\n/, $uno; # split in two
	my @preamb;

	foreach my $line (@coment) { 
	chomp($line); 
		if(!$xetex){
			$line =~ s/\\usepackage(?:\[.+?\])?\{pst.+?}//g; 		# delete
			$line =~ s/\\usepackage(?:\[.+?\])?\{psfr.+?}//g; 		# delete
			$line =~ s/(\\usepackage(?:\[.+?\])?)\{auto-pst.+?}//g; 	# delete
			$line =~ s/(\\usepackage(?:\[.+?\])?)\{vaucanson-.+?}//g; 	# delete
			$line =~ s/(\\usepackage(?:\[.+?\])?)\{psgo}//g; 	# delete
			}
			$line =~ s/(\\usepackage(?:\[.+?\])?)\{graph/\%$1\{graph/g; # comment
			next if $line =~ m/^\s*$/;
	push(@preamb,$line);}   # adding changes

my $clean = join("\n", @preamb, "\\usepackage{grfext}\n\\\PrependGraphicsExtensions*{$Iext}\n\\usepackage{graphicx}\n\\graphicspath{{$imageDir/}}\n\\begin{document}". $dos);

	open my $OPDF,'>',"$tempDir/$pdfname-pdf.tex"; # write changes
	print $OPDF $clean; 
	close $OPDF; 
# ------------------------- close coment pst-package ------------------------------
my $runAgain = 0;
		if ($norun){print "Done\n";}
		else {
		if($xetex){
			system("xelatex -interaction=batchmode $tempDir/$pdfname-pdf"); print "Done\n";}
		else{
			system("pdflatex -interaction=batchmode $tempDir/$pdfname-pdf"); print "Done\n";}
		}
		if (-e "$tempDir/$pdfname-pdf.idx") {
			system("makeindex $tempDir/$pdfname-pdf.idx"); $runAgain++;}
		if ($runBibTeX && -e "$tempDir/$pdfname-pdf.aux") { 
			system("bibtex $tempDir/$pdfname-pdf");  $runAgain++; }
		if ($runBiber && -e "$tempDir/$pdfname-pdf.bcf") {
		system("biber $tempDir/$pdfname-pdf");  $runAgain++; }
		if ($runAgain){
		if($xetex){
			system("xelatex -interaction=batchmode $tempDir/$pdfname-pdf");}
		else{
			system("pdflatex -interaction=batchmode $tempDir/$pdfname-pdf");}
		}
		if ($ppm){
		print "If you need to create jpg/png/svg type cd $imageDir and run\n";
		print "mogrify -format [ext] *.ppm\n";
        }
		if ($clear) {
		if ($norun){}
    if ($single) {
		unlink "$tempDir/$name-fig.pdf";
		if(!$xetex){
		unlink "$tempDir/$name-fig.dvi";
		unlink "$tempDir/$name-fig.ps";
			}
		unlink "$tempDir/$name-fig.aux";
		unlink "$tempDir/$name-fig.log";
		unlink "$tempDir/$name-fig.tex";
		if(!$verbose) { 
		unlink "$tempDir/$name.plog";
			}
		unlink "$tempDir/$name.preamble";
		unlink "$tempDir/$name-pdf.aux";
		unlink "$tempDir/$name-pdf.log";
			}
		else{
		if(!$verbose) { 
		unlink "$tempDir/$name.plog";
			}
		unlink "$tempDir/$name.preamble";
		unlink "$tempDir/$name-pdf.aux";
		unlink "$tempDir/$name-pdf.log";
		unlink "$tempDir/$name-pics.pdf";
		unlink "$tempDir/$name-pics.tex";
		unlink "$tempDir/$name-pics.aux";
		if(!$xetex){
		unlink "$tempDir/$name-pics.dvi";
		unlink "$tempDir/$name-pics.ps";}
		unlink "$tempDir/$name-pics.log";
		unlink "$tempDir/$name-fig.tex";
			}
		}
	}

sub LOG() { 
	if ( $verbose ) { print $LOGfile "@_\n"; } 
	}
__END__
