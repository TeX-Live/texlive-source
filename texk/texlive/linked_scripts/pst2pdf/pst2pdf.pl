eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;

use strict;				# to be sure, that all is safe ... :-)

# $Id: pst2pdf.pl 611 2011-12-14 08:41:35Z herbert $
# v. 0.15	simplify the use of PSTricks with pdf
# 2011-12-12	(c) Herbert Voss <hvoss@tug.org> 
#                   Pablo González Luengo
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
use autodie;									# more safe
#----------------------- User part begin ------------------------
my $imageDir = "images";		# where to save the images
my $Iext = ".pdf";			# leave empty, if not a special one
my $tempDir = ".";			# temporary directory
my $verbose = 0;			# 0 or 1, logfile  
my $clear = 0;				# 0 or 1, clears all temporary files
my $DPI = 75;				# very low value for the png's
my $Iscale = 1;			 	# for \includegraphics
my $noImages = 0;			# 1->no create images
my $single = 0;				# 1->create images in single mode
my $runBibTeX = 0;			# 1->runs bibtex
my $runBiber = 0;			# 1->runs biber and sets $runBibTeX=0
my $jpg = 0;				# 1->create .jpg files
my $png = 0;				# 1->create .png files
my $ppm = 0;				# 1->create .ppm files
my $eps = 0;				# 1->create .eps files
my $files = 1;				# 1->create image files .tex 
my $all = 0;				# 1->create all images and files for type
my $xetex = 0;				# 1->Using (Xe)LaTeX for compilation.
my $PS2 = " ";          		# Options for ps2pdf
#----------------------- User part end ---------------------------
#-------------- program identification, options and help ---------
my $program = "pst2pdf";
my $nv="0.15";
my $ident = '$Id: pst2pdf.pl 611 2011-12-14 08:41:35Z herbert $';
my $copyright = <<END_COPYRIGHT ;
Copyright 2011-2013 (c) Herbert Voss <hvoss\@tug.org> and Pablo González.
END_COPYRIGHT
my $licensetxt= <<END_LICENSE ;
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
       
END_LICENSE
my $title = "$program $nv $ident\n";
my $usage = <<"END_OF_USAGE";
${title}Usage: $program <texfile.tex>  [Options]
pst2pdf run a TeX source, read all PS-related part and convert in 
	single images in pdf,eps,jpg or png format (default pdf) and 
	create new file whitout pst-enviroment and runs (pdf/Xe)latex. 
	See pst2pdf documentation for more info.
Options:
  -h,-help          - display this help and exit
  -l,-license       - display license information and exit
  -imageDir=        - the dir for the created images (default images) 
  -d,-DPI=<int>     - the dots per inch for a cretaed ppm files (default 75)
  -j,-jpg           - create .jpg files (need pdftoppm and ImageMagick)
  -p,-png           - create .png files (need pdftoppm and ImageMagick)
  -e,-eps	    - create .eps files	(need pdftops)
  -Iscale=<real>    - the value for [scale=] in \\includegraphics 
  -noImages         - generate file-pdf.tex, but not images
  -v,-verbose       - creates log file (.plog)
  -c,-clear         - delete all tmp files
  -runBibTeX        - run bibtex on the aux file, if exists
  -runBiber         - run biber on the bcf file, if exists
  -s,-single	    - create images whitout pdftk
  -a,-all	    - create all image type and images.tex	
  -x,-xetex         - using (Xe)LaTeX for compilation
  -PS2=<opt>        - pass options to ps2pdf (default empty)
Examples:
* $program test.tex -all
* produce test-pdf.tex whitout pst-enviroment and create image dir 
* whit all images (pdf,eps,jpg,png) and source(.tex) for all pst-enviroment
END_OF_USAGE
#
my $result=GetOptions (
	'h|help'     => \$::opt_help,
	'l|license'  => \$::opt_license,
	'd|DPI=i'    => \$DPI,		# numeric
	"Iscale=f"   => \$Iscale,   	# real
	"imageDir=s" => \$imageDir, 	# string
	"PS2=s"      => \$PS2, 	        # string
	"tempDir=s"  => \$tempDir,  	# string
	"Iext=s"     => \$Iext,     	# string
	'c|clear'    => \$clear,    	# flag
	"noImages"   => \$noImages, 	# flag
	's|single'   => \$single,	# flag
	"runBibTeX"  => \$runBibTeX,    # flag
	'e|eps'      => \$eps,		# flag
	'j|jpg'      => \$jpg,		# flag
	'p|png'      => \$png,		# flag
	'a|all'      => \$all,		# flag
	'x|xetex'    => \$xetex,	# flag
	"runBiber"   => \$runBiber, 	# flag
	'v|verbose'    => \$verbose,
) or die $usage;
#------------------------ Help functions --------------------------
sub errorUsage { die "Error: @_ (try --help for more information)\n"; }
# options for command line
if ($::opt_help) {
print $usage;
    exit (0);
}
if ($::opt_license) {
  print $title;
  print "$copyright\n";
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
	LOG ("==> PS2      = $PS2"); 
	LOG ("==> Iscale   = $Iscale"); 
	LOG ("==> tempDir  = $tempDir"); 
	LOG ("==> verbose  = $verbose"); 
	LOG ("==> clear    = $clear"); 
	LOG ("==> noImages = $noImages"); 
	LOG ("==> single   = $single"); 
	LOG ("==> runBibTeX= $runBibTeX"); 
	LOG ("==> runBiber = $runBiber"); 
	LOG ("==> ppm      = $ppm"); 
	LOG ("==> eps      = $eps");  
	LOG ("==> jpg      = $jpg"); 
	LOG ("==> png      = $png"); 
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
if ($eps) {
  LOG ("Generate .eps files ...");
  $eps = 1;
}
if ($jpg) {
  LOG ("Generate .jpg files ...");
	$ppm = 1;
  $jpg = 1;
}
if ($png) {
  LOG ("Generate .png files ...");
	$ppm = 1;
  $png = 1;
}
if ($files) {
  LOG ("Generate .tex images files ...");
  $files=1;
}
if ($all) {
  LOG ("Generate all images eps/pdf/files and clear...");
   $eps =$ppm=$jpg=$png=$clear = 1;
}
if ($single) {
  LOG ("single mode generate images files ...");
  $single= 1;
}
if ($xetex) {
  LOG ("Compiling using XeLaTeX ...");
  $xetex=1;
}
if ($noImages ) {
  LOG ("no create images");
 	$single= 0;
    }

my $imgNo = 1;				# internal image counter
#----------- single mode, no need pdftk  ----------------------
if ($single) {
LOG ("Running on [$path][$name][$ext]"); 
open my $FILE,'<', "$TeXfile" ;	# the source
LOG ("single mode generate images...");
if (-d $imageDir) { LOG ("$imageDir exists") }
else { mkdir("$imageDir", 0744) ;}
savePreamble($name);
runXPD($name);
runFile($name);
close $FILE;				# close source file
close $LOGfile;
}#---------------- Default way, using pdftk --------------
else{
LOG ("Running on [$path][$name][$ext]"); 
open my $FILE,'<', "$TeXfile" ;	# the source
if (!$noImages ) {
if (-d $imageDir) { LOG ("$imageDir exists") }
else { mkdir("$imageDir", 0744) ;
LOG ("Imagedir created"); }
LOG ("go to savePreamble ... "); 
savePreamble($name);
runXPD($name);
runFile($name);
LOG ("done!\n go to runFile ..."); 
LOG ("done!"); 
close $FILE;			# close source file
close $LOGfile;
}# !noImages
}
# Create xpdfrc conf for silent output pdftops/pdftoppm mesagge in windows

sub runXPD{
if ($^O eq 'MSWin32'){
open my $ppmconf, '>', "$tempDir/xpd";
print $ppmconf <<'EOH';
errQuiet yes
EOH
close $ppmconf;
	}
}
# end fix

# create a preamble file
# if we have a \input command inside the preamble, it doesn't hurt, we need
# it anyway for the postscript files and the pdf one.

# ----------------- Save Preamble ---------------------
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
# Creating file.tex and .pdf(eps,ppm) for images in single mode

sub runsingle{
	my $filename = pop;
	
if ($xetex){
			system("xelatex -interaction=batchmode $filename"); 
      system("pdfcrop -margins 1 -xetex $filename.pdf $imageDir/$filename-$imgNo.pdf");
			}
else{
			system("latex -interaction=batchmode $filename"); 
			system("dvips -q -Ppdf $filename"); 
			system("ps2pdf $PS2 $filename.ps");
			system("pdfcrop -margins 1 $filename.pdf $imageDir/$filename-$imgNo.pdf");
			}
if ($files){
	copy("$filename.tex", "$imageDir/$filename-$imgNo.tex");
	}
	
if ($eps) {
	if ($^O eq 'MSWin32'){
	system("pdftops -cfg $tempDir/xpd -q -level3 -eps $imageDir/$filename-$imgNo.pdf $imageDir/$filename-$imgNo.eps");
		    }
	else{
	system("pdftops -q -level3 -eps $imageDir/$filename-$imgNo.pdf $imageDir/$filename-$imgNo.eps");
	    }
	}
if ($ppm) {
	if ($^O eq 'MSWin32'){
	system("pdftoppm -cfg xpd -q -r $DPI $imageDir/$filename-$imgNo.pdf $imageDir/$filename-$imgNo");
	    }
else{
	system("pdftoppm -q -r $DPI $imageDir/$filename-$imgNo.pdf $imageDir/$filename-$imgNo");
	}
  }

	$imgNo=$imgNo+1;
}
#------------------- Copy files.tex for images in default mode ---------
sub runTeX{
	my $filename = pop;
		if ($files){
	copy("$filename.tex", "$imageDir/$filename-$imgNo.tex");
}
	$imgNo=$imgNo+1;
}

#------------------ Create files.tex for images  --------------
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
		runsingle("$tempDir/$filename-fig");
		}
		else{runTeX("$tempDir/$filename-fig");}
		}
}
LOG ("runpdfTeX ... "); 
runpdfTeX("$path$name",$name);
LOG ("all finished ... :-)"); 

# ----------------Renaming ppm need for correct name
if(!$noImages){
	my $dren = "$tempDir/$imageDir";
	my $fich = '';
	my $ppmren = '';
	my $PPMno = 1;
	my $renNo = 1;
	if(opendir(DIR,$dren)){
		foreach (readdir DIR){
			$fich = $_;
		if ( $fich =~ /($name-fig-)(\d+|\d+[-]\d+).ppm/) {
	my $renNo   = int($2);
	my $newname="$1$renNo.ppm";
		 $ppmren = rename("$dren/$fich","$dren/$newname");
	if($jpg){
		system("convert $imageDir/$name-fig-$renNo.ppm $imageDir/$name-fig-$renNo.jpg");
		}
	if($png){	
		system("convert $imageDir/$name-fig-$renNo.ppm $imageDir/$name-fig-$renNo.png");
		}
		}
		}
	}
closedir DIR;
	if($clear){
	unlink <$imageDir/*.ppm>;
	}
}# end renaming ppm

#------------------ Replace pst enviroment for images---------
sub runpdfTeX() {

  my ($name,$pdfname) = @_;
  open my $PDF,'>',"$tempDir/$pdfname-pdf.tex";
  open my $FILE,'<',"$name.tex";
  my $ignore = 0;
  my $IMGno = 1;
  my $depth = -1;
  my $type = -1;
  my $EndDocument = 0;				# ignore all after \end{document}
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
# ------------- Create one file contain all pst (default mode) ----------------
if (!$single==!$noImages){
my ($name,$pdfname) = @_;
my @PSTarray = searchPS();
my @pstfiles = glob("$imageDir/*.tex");
@pstfiles =
    map  { $_->[1]                       }
    sort { $a->[0] <=> $b->[0]           }
    map  { [ ($_ =~ /(\d+)\.tex$/), $_ ] }
    @pstfiles;
my @container;
for my $archivo (@pstfiles) {

    open my $FH, '<:crlf', $archivo;    # EOL
    my $tex = join q{}, <$FH>;           # 
    close   $FH;
    my($pstimg) = $tex =~ m/\\begin\{document\}\n(.+?)\n\\end\{document\}/sm;
    push @container, $pstimg if $pstimg;
}
open my $FILE,'<',"$tempDir/$name.preamble";
open my $OUTPST,'>',"$tempDir/$name-pst.tex";
    while (<$FILE>) {print $OUTPST $_; }
    print $OUTPST "\\newenvironment{postscript}{}{}\n";
	print $OUTPST "\\pagestyle{empty}\n";
	print $OUTPST "\\begin{document}\n";
my $fig = 1;
for my $item (@container) {
    print $OUTPST $item;
    print $OUTPST "\n%fig" . "$fig\n";
    print $OUTPST "\\newpage\n";
    $fig++;
}
	print $OUTPST "\\end{document}";
	close $OUTPST;
	close $FILE;
# close all
# -------------------- Compiling File using pdftk ----------------------
my ($name,$pdfname) = @_;
if ($xetex){
	system("xelatex -interaction=batchmode $tempDir/$pdfname-pst.tex");
  system("pdfcrop -xetex $tempDir/$name-pst.pdf $tempDir/$pdfname-pst.pdf");
	}
else{
	system("latex -interaction=batchmode  $tempDir/$pdfname-pst");
	system("dvips -q -Ppdf $tempDir/$pdfname-pst");
	system("ps2pdf $PS2 $tempDir/$pdfname-pst.ps $tempDir/$pdfname-pst.pdf");
	system("pdfcrop -margins 1 $tempDir/$pdfname-pst.pdf $tempDir/$pdfname-pst.pdf");
	}
	system("pdftk $tempDir/$pdfname-pst.pdf burst output $imageDir/$pdfname-fig-\%1d.pdf");
if ($eps){
for my $fichero_pdf (<$imageDir/*.pdf>) {
        (my $fichero_eps = $fichero_pdf) = s/ppm$/png/i;
    if ($^O eq 'MSWin32'){	
        system("pdftops -cfg xpd -q -level3 -eps $fichero_pdf $fichero_eps"); # conversión
				 }
    else{
	system("pdftops -q -level3 -eps $fichero_pdf $fichero_eps"); # conversión
	    }
  }
}
if ($ppm){
    if ($^O eq 'MSWin32'){
	system("pdftoppm -cfg xpd -q -r  $DPI $tempDir/$pdfname-pst.pdf $imageDir/$pdfname-fig");
	}
    else{
	system("pdftoppm -q -r  $DPI $tempDir/$pdfname-pst.pdf $imageDir/$pdfname-fig");
	}
    }
	}

#---------------- Coment pst-package in file-pdf.tex for (pdf/Xe)LaTeX ----------------
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

my $runAgain = 0;
		
		if($xetex){ # xelatex mode
		if ($noImages){print "The file $pdfname-pdf.tex are created (Xe)LaTeX\n";}
		else{	
		  system("xelatex -interaction=batchmode $tempDir/$pdfname-pdf"); 
			print "Done, compiled $pdfname-pdf.tex using (Xe)LaTeX\n";}}
		else{ #pdflatex mode
		if ($noImages){
		   print "The file $pdfname-pdf.tex are created (pdf)LaTeX\n";}
		else{			
			system("pdflatex -interaction=batchmode $tempDir/$pdfname-pdf.tex"); 
			print "Done, compiled $pdfname-pdf.tex using (pdf)LaTeX\n";}
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
		
		if ($clear) {
		unlink "$tempDir/$name.preamble";
		unlink "$tempDir/$name-pdf.aux";
		unlink "$tempDir/$name-pdf.log";
		unlink "$tempDir/$name-fig.tex";
		if ($eps or $ppm){
		if ($^O eq 'MSWin32'){unlink "$tempDir/xpd";}
		}
		if(!$verbose) {unlink "$tempDir/$name.plog";}
		if ($single) {#comon
		unlink "$tempDir/$name-fig.pdf";
		unlink "$tempDir/$name-fig.aux";
		unlink "$tempDir/$name-fig.log";
		if($xetex){	}
		else{
		unlink "$tempDir/$name-fig.dvi";
		unlink "$tempDir/$name-fig.ps";}
		}
		else{ #clean for pdftk mode
		unlink "$tempDir/$name-pst.log";
		unlink "$tempDir/$name-pst.pdf";
		unlink "$tempDir/$name-pst.aux";
		if($xetex){	}
		else{
		unlink "$tempDir/$name-pst.dvi";
		unlink "$tempDir/$name-pst.ps";
		}
	}
}
}
sub LOG() { 
	if ( $verbose ) { print $LOGfile "@_\n"; } 
	}
__END__
