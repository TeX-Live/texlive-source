eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q' # -*-perl-*-
        if 0;
# The expression in the previous line replaces the unix specific line 
# {#!/usr/bin/perl}.   
# ps2eps - convert PostScript to EPS (Encapsulated PostScript) files
# -------------------------------------------------------------------
# $Id: ps2eps,v 1.61 2006/12/28 16:34:06 bless Exp $
# -------------------------------------------------------
# (C)opyright 1999-2006 Roland Bless
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
# Author: Roland Bless
# Send bug reports to roland <at> bless.de
# -------------------------------------------------------------------
# Additional filtering is performed when Windows generated PostScript files
# are processed. Some instructions will otherwise lead to bad output
# if EPS-file gets embedded into other PostScript files.
#
# Requirements:
# + perl
# + gs   (ghostscript supporting pbm output)
# + bbox (a little C program [ANSI-C - should work on every platform]
#         for calculation of the actual BoundingBox)

use POSIX;

#use warnings;

#use Getopt package
use Getopt::Long;
Getopt::Long::Configure("no_ignore_case");

$prgname= "ps2eps";

if (! -d "/usr/bin")
{ # we assume that we are running under native windows
  $ghostscriptname = "gswin32c";
  $NULLDEV = "nul";
} 
else 
{ # Unix or cygwin
  $ghostscriptname = "gs";
  $NULLDEV = "/dev/null 2>&1";
}

$bboxver=`bbox >$NULLDEV -V`;
$bboxname= ($?== -1) ? "" : "bbox";
$version= '$Id: ps2eps,v 1.61 2006/12/28 16:34:06 bless Exp $'; #'
$insertPScode= 1;     # Insert surrounding Postscript code
$infhandle = STDIN;   # Standard input is the default input file
$outfhandle = STDOUT; # Standard output is default output if STDIN is input
$infname= '-';
$outfname= '-';
$forceoverwrite=0;    # do not overwrite existing files
$ignoreBB= 0;         # ignore existing Bounding Box comment
$removeDSC= 1;        # remove Adobe document structure comments
$removeADO= 1;        # remove Adobe printer Driver console Output [Page: ...]
$ignoreEOFDSC= 0;     # ignore %%EOF DSC hint
$removePreview= 0;    # remove preview
$quiet= 0;            # write progress to stdout
$resolution= 144;     # resolution for bounding box calculation is 2x72 dpi
$trytofixps= 1;       # try to fix postscript code
$forcefixps= 0;       # fix postscript code unconditionally if eq 1
$filterorientation= 1;# filter Orientation line
$looseBB='';          # default: tight bounding box
$clip=0;              # do not clip
$warnings=0;          # do not print warnings concerning postscript sanity
$debuggs=0;           # no debugging of ghostscript call, turn this on if you want to see the gs call
$inch=2.54;           # one inch is 2.54cm
$fullcolors= 1;       # use ppm format (24-bit full color)
$trailerseen= 0;      # Trailer comment seen?
$PSversion="2.0";     # default Postscript Version
$PSDSCversion="2.0";  # default Postscript DSC Version
$translate_x= 0;      # translate by x postscript points
$translate_y= 0;      # translate by y postscript points

$defaultext = '(ps|prn)';  # default extension
$defaultoutext = '.eps';   # default output extension
$envname_size = 'PS2EPS_SIZE';
$envname_gsbbox = 'PS2EPS_GSBBOX';

$gpar="";
$known_papersizes="11x17|ledger|legal|letter(small)?|arch[A-E]|a([0-9]|10)|isob[0-6]|b[0-5]|c[0-6]|jisb[0-6]|fls(a|e)|halfletter";
$papersize_help="11x17,ledger,legal,letter,lettersmall,archA,archB,archC,archD,archE\
a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,isob0,isob1,isob2,isob3,isob4,isob5,isob6,\
b0,b1,b2,b3,b4,b5,c0,c1,c2,c3,c4,c5,c6,\
jisb0,jisb1,jisb2,jisb3,jisb4,jisb5,jisb6,flsa,flse,halfletter\n";
$trigger= 0;
$notsane= 0; 
$dummy="";

@ver= split(/ /,$version);

# filename for temporary files
if ($^O =~ /MSWin32/i or $^O =~ /cygwin/i) 
{
  # it is less trouble to use the current directory if working on
  # cygwin and nevertheless using gswin32c.
  $tmpfname= "$prgname.$$";
  $win=1;
}
elsif (defined($ENV{'TMP'}))
{ 
  $tmpdir= $ENV{'TMP'};
  $filesep= ($tmpdir =~ /^?\:\\/) ? '\\' : '/';
  if ($tmpdir =~ /$filesep$/)  
  { $tmpfname= $tmpdir . "$prgname.$$"; }
  else 
  { $tmpfname= $tmpdir . "$filesep$prgname.$$"; }
  $win=1;
}
else #assume we're on a UNIXBOX
{ 
  $tmpfname= "/tmp/" . "$prgname.$$";
  $win=0;
}


$licensetxt= "\
    This program is free software; you can redistribute it and/or modify\
    it under the terms of the GNU General Public License as published by\
    the Free Software Foundation; either version 2 of the License, or\
    (at your option) any later version.\
\
    This program is distributed in the hope that it will be useful,\
    but WITHOUT ANY WARRANTY; without even the implied warranty of\
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\
    GNU General Public License for more details.\
\
    You should have received a copy of the GNU General Public License\
    along with this program; if not, write to the Free Software\
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n";

@prgidtxt= ( "$prgname - convert PostScript to EPS (Encapsulated PostScript) files\n",
	     "(C)opyright 1998-2006 Roland Bless\n\n" );

@helptxt= ("Version: $ver[2]\n",
          "Operation:\n",
          " Without any argument, $prgname reads from standard input\n",
          " and writes to standard output.\n",
          " If filenames are given as arguments they are processed\n",
          " one by one and output files are written to filenames\n",
          " with extension '$defaultoutext'. If input filenames have the extension\n",
          " '.ps' or '.prn', this extension is replaced with '$defaultoutext'.\n",
          " In all other cases '$defaultoutext' is appended to the input filename.\n",
          " Please note that PostScript files for input should contain\n",
          " only one single page.\n\n",
          " If BoundingBox in output seems to be wrong, please try options --size or --ignoreBB.\n\n" );

@usagetxt= ("Syntax:\n",
            " $prgname [-f] [-q] [-N] [-O] [-n] [-P] [-c] [-C] [-m] [-B] [-E] [-s <pagedim>] [-R +|-|^] [-t <x,y>] [-l] [-g] [-d] [-H] [-h|--help] [-g] [-W] [-L] [-V|--version] [--] [psfile1] [psfile2] [...]\n",
            "Options:\n",
            " -f, --force                force overwriting existing files\n",
            " -q, --quiet                quiet operation (no output while processing files)\n",
            " -N, --noinsert             do not insert any postscript code\n",
            " -O, --preserveorientation  do not filter Orientation: header comment\n",
            " -n, --nofix                do not try to fix postscript code\n",
            " -P, --removepreview        remove preview image (smaller file, but no preview)\n",
            " -F, --fixps                fix postscript code unconditionally\n",
            " -c, --comments             preserve document structure comments\n",
	    " -C, --clip                 insert postscript code for clipping\n",
            " -m, --mono                 use black/white bitmap as base for calculation\n",
            " -s, --size=<pagedim>       page size (a0-a10,letter,...) or in format XxY[cm|in] (default:cm), where X and Y are numbers\n",
	    "                            use --size=list to list pre-defined page sizes\n",
	    " -R, --rotate=<direction>   rotate resulting EPS. <direction>: +=+90 (clockw.),-=-90 (counter-clockw.) ^=180 degrees\n",
	    " -t, --translate            specify x,y offset (may be negative) in postscript points (1/72 dpi)\n",
	    " -r, --resolution           specify dpi resolution to render with ghostscript (default 144)",
            " -l, --loose                expand the original bounding box by one point in each direction\n",
	    " -B, --ignoreBB             do not use existing bounding box as page size for rendering\n",
	    " -E, --ignoreEOF            do not use %%EOF as hint for end of file\n",
	    " -g, --gsbbox               use internal bbox device of ghostscript\n",
	    " -H, --no-hires             do not use a HiResBoundingBox\n",
            " -h, --help                 help information\n",
            " -L, --license              show licensing information\n",
            " -V, --version              show version information\n",
	    " -d, --debuggs              show ghostscript call\n",
	    " -W, --warnings             show warnings about sanity of generated eps file\n",  
            " --     all following arguments are treated as files\n",
            "        (allows filenames starting with -)\n",
            "\n",
            "Arguments:\n",
            " One or more names of PostScript files for input\n");

## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
## -- argument checking -- 
## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#environment variable for papersize
if (defined($ENV{"$envname_size"}))
{
    $opt_s= $ENV{"$envname_size"};
}
else
{
    $opt_s = '';          # for s-option
}

if (defined($ENV{"$envname_gsbbox"}))
{
    $bboxname="";
}

$opt_t = '';          # for t-option
$opt_R = '';          # for R-option
$opt_r = '';          # for r-option
$stopnow = 0;
die "Wrong option(s), please check usage with $prgname --help\n" unless 
GetOptions('f|force'	=> \$forceoverwrite,
	   'q|quiet'	=> \$quiet,
	   'm|mono'	=> sub { $fullcolors = 0 },
	   'n|nofix'	=> sub { $trytofixps = 0 },
	   'F|fixps'	=> \$forcefixps,
	   'N|noinsert'	=> sub { $insertPScode = 0 },
           'O|preserveorientation'  => sub { $filterorientation= 0 },
	   'P|removepreview'	    => \$removePreview,
	   'c|comments' => sub { $removeDSC = 0 },
	   'C|clip'	=> \$clip,
	   'l|loose'	=> sub { $looseBB = '-l' },
	   'B|ignoreBB'	=> \$ignoreBB,
	   'E|ignoreEOF'=> \$ignoreEOFDSC,
	   's|size=s'	=> \$opt_s,
	   't|translate=s'	=> \$opt_t,
	   'r|resolution=s' => \$opt_r,
	   'R|rotate=s'	=> \$opt_R,
	   'g|gsbbox'   => sub { $bboxname=""; },
	   'H|nohires'  => \$nohires,
	   'h|help'	=> sub { $stopnow = 1; print @prgidtxt,@helptxt,@usagetxt,"\nAuthor: Roland Bless (roland\@bless.de)\n\n"; },
	   'L|license'	=> sub { $stopnow = 1; print @prgidtxt,$licensetxt,"\nAuthor: Roland Bless (roland\@bless.de)\n\n"; },
	   'd|debuggs'  => \$debuggs,
	   'W|warnings' => \$warnings,
	   'V|version'	=> sub { $stopnow = 1; print @prgidtxt,"Version: $ver[2]\n"; });exit if ($stopnow);

## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
## -- wildcard processing --
## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
## internal wildcard processing for current directory, 
## only used for non UNIX-based OSs (which may lack shell wildcard expansion)
@filenames = ();
foreach $object (@ARGV) {
    if ($win && $object =~ m/\*/o) # asterisk is present in filename
    {
      $wildcard = $object;
      $wildcard =~ s/\./\\\./g; # replace . in pattern with \.
      $wildcard =~ s/\*/\(\.\*\)/g; # replace * in pattern with (.*)
	opendir(DIR,'.') || die 'Unable to open current directory, stopped'; # open current directory
      print STDERR $wildcard;
	@fid = grep { /$wildcard(\.$defaultext)?/i } readdir(DIR);
	foreach (@fid) { push @filenames, $_; }
	closedir DIR;
    }
    else { push @filenames, $object; }
} # end foreach
$filenames[0]= '-' if (scalar(@filenames) == 0); # no file arguments, use STDIN as input

print STDERR "Input files: @filenames\n" if (!$quiet);

if ($opt_r ne '') 
{
  $resolution=$opt_r;
}

# papersize stuff
if ($opt_s ne '') 
{
    # if explicit size is given, ignore existing BoundingBox always
    $ignoreBB = 1; 
    $pagedimension = $opt_s;
    if ($opt_s eq "list")
    {
	print STDERR "Available paper sizes: $papersize_help";
	exit 0;
    }

    #explicit format XxY[cm|in]
    if ($pagedimension =~ /(\d*\.?\d+)x(\d*\.?\d+)/)
    {
	($x_dim, $dummy, $y_dim, $unit)= split(/(x|cm|in)/,$pagedimension);
	if ( $x_dim !~ /^\d*\.?\d+$/ ) 
	{ die "$x_dim in $arg is not a valid number, stopped"; } 
	if ( $y_dim !~ /^\d*\.?\d+$/ ) 
	{ die "$y_dim in $arg is not a valid number, stopped"; } 
	
	#print STDERR "xdim: $x_dim ydim: $y_dim unit:$unit\n" ;
	if (!defined($unit) )
	{
	    $unit='cm';
	    $opt_s=$opt_s . $unit;
	}
	if ( $unit ne 'in' ) # assume centimeters
	{ # calculate dimension in pixels (remember: resolution is in dpi)
	    $xpixels= int(($x_dim * $resolution) / $inch)+1;
	    $ypixels= int(($y_dim * $resolution) / $inch)+1;
	    $gpar= "-g${xpixels}x${ypixels}";
	}
	else
	{
	    $xpixels= int($x_dim * $resolution)+1;
	    $ypixels= int($y_dim * $resolution)+1;
	    $gpar= "-g${xpixels}x${ypixels}";
	}
    } #endif XxY in opt_s
    else
    {
	if ($opt_s =~ /$known_papersizes/)
	{
	   $gpar="-sPAPERSIZE=$opt_s";
	}
	else
	{
	    print STDERR "Error: Unknown paper size: $opt_s\n Acceptable papersizes are:$papersize_help\n";
	    exit 1;
	}
    }
}

#translate option    
if ($opt_t ne '')
{
  ($translate_x,$translate_y)= split(/\,/,$opt_t);
}

#rotate
$rotright='-90 rotate';
$rotleft='90 rotate';
$rotupsidedown='180 rotate';
$rotate='';
if ($opt_R ne '')
{
    if ($opt_R eq '+') { $rotate=$rotright; }
    elsif ($opt_R eq '-') { $rotate=$rotleft; }
    elsif ($opt_R eq '^') { $rotate=$rotupsidedown; }
    else { die "Wrong parameter for option -R: Valid are only +,-,^\n"; };
}

$device= $fullcolors ? "ppmraw" : "pbmraw";

## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
## -- iterate over different input files --
## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
PROCESSFILE:
while ($infname= (shift @filenames))
{
  # reset filter definitions for each file
  undef $linefilter; 
  undef $rangefilter_begin;
  undef $rangefilter_end;
  $fixthisps= $trytofixps;
  $fixmsgprinted= 0;

  if (!$quiet) { print STDERR "Processing: $infname\n"; }
  unless (open($infhandle,"<$infname"))
  { # skip over this file 
    print STDERR "$prgname: Can't open $infname: $!\n";
    next PROCESSFILE;
  }
 
  # buffer input from stdin into temporary file, because it has to be read twice
  # one time for ghostscript processing, the second time for generating output
  if ($infname eq '-') # input is stdin
  {
    $tmpfhandle='';
    open($tmpfhandle,">$tmpfname") or 
       die "Cannot open temporary file $tmpfname for writing: $!\n";
  }
  else # input is not stdin
  {
    undef $tmpfhandle;
    #if filename ends with $defaultext usually .ps or .prn, replace the extension with $defaultoutext
    if ($infname =~ /\.$defaultext$/i) 
    { 
      $outfname= $infname; $outfname =~ s/\.$defaultext$/$defaultoutext/i; 
    }
    else # otherwise simply append the extension $defaultoutext
    {
        $outfname= $infname . "$defaultoutext";
    }
    if (!$forceoverwrite and -s "$outfname") 
      { 
        die "$prgname: Sorry, file named $outfname already exists,",
            " will not overwrite it.\n",
            " You will have to use the -f option, delete it or rename it",
            " before running $prgname again.\n";   
      }
    else
    {
      open($outfhandle,">$outfname") or die "Can't open file $outfname for writing: $!\n";
    }
  } #end else input is not stdin
  
  ## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  ## -- process input file --
  ## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  $linefilter= '^$'; #'# filter empty lines by default 
  while (<$infhandle>)
  {
    # get postscript and DSC version
    if (/%!PS-Adobe-(\S+).*EPSF-(\S+)/) 
    {
      $PSversion=$1;
      $PSDSCversion=$2;
      if (! ($PSversion =~ /\d+\.\d+/))
      {
        $PSDSCversion="2.0";
      }
      if (! ($PSDSCversion =~ /\d+\.\d+/))
      {
        $PSDSCversion="2.0";
      }
    }

    # check for existing BoundingBox parameters
    if ( /^%%\s*BoundingBox:\s*(.*)/ && !defined($eBBllx) )
    {
      $BBarg= $1;
      # accept even negative and fractional BBs
      if ( $BBarg =~ /(\-?\d+\.?\d*\s+){3,}\d+/ ) # ignore %% BoundingBox: (atend) comments
      {
	($eBBllx,$eBBlly,$eBBurx,$eBBury,$dummy)= split /\s/,$BBarg;
	#print STDERR "Existing BB: $eBBllx,$eBBlly,$eBBurx,$eBBury\n";
	if (int($eBBllx) < 0) { $translate_x= - int($eBBllx-0.5); }
	if (int($eBBlly) < 0) { $translate_y= - int($eBBlly-0.5); }

	$xpixels= int((($eBBurx-$eBBllx) * $resolution)/72)+1;
	$ypixels= int((($eBBury-$eBBlly) * $resolution)/72)+1;
	if (!$ignoreBB)
	{
	    $gpar= "-g${xpixels}x${ypixels}";
	    # check for meaningful values
	    if (($xpixels <= 1) || ($ypixels <= 1))
	    {
		$gpar=""; undef $eBBllx; undef $eBBlly;
	    }
	    else
	    {
		if (!$quiet) 
		{
		    print STDERR "Rendering with existing $_";
		    if (int($eBBllx) < 0 || int($eBBlly) < 0)
		    {
			print STDERR "WARNING: existing Bounding Box shows negative values - shifting\n";
		    }
		}
	    }
	} #endif !$ignoreBB
      } #endif $BBarg =~
  }


    if ($fixthisps) # try to fix bad postscript code
    {
      # check for Windows 3.x output
      if ( /^Win.*Dict/ )
      { 
        if (!$quiet && !$fixmsgprinted) 
           { print STDERR "Windows 3.5 generated Postscript file detected, fixing\n"; }
        $linefilter= '^(EJ|RS)';
        $rangefilter_begin= '^statusdict';
        $rangefilter_end= 'cvx\ settransfer$';  #'
        $fixmsgprinted= 1; # stop printing message
      }
      else
      {
        if ( /^%%Creator:\s*Wind.U\s*Xprinter/ )
        { 
	    if (!$quiet && !$fixmsgprinted) 
	    { print STDERR "Star/OpenOffice generated Postscript file detected, fixing\n"; }
	    $linefilter= '^rs';
	    $fixmsgprinted= 1; # stop printing message
	}
      else
      {
        if ( $forcefixps || 
             /^\/NTPS/ || 
             /Creator:\s*(AdobePS|Pscript|.*Windows)/i ) #check for NT generated output
        {
          if (!$quiet && !$fixmsgprinted) 
          { 
            if ($forcefixps)
	    {
		print STDERR "Postscript filtering requested, fixing\n";
	    }
	    else
	    {
		print STDERR "Windows generated Postscript file detected, fixing\n"; 
	    }
	  }
	  $rangefilter_begin= '^((\[\{)|(featurebegin\{))$'; #'
	  $rangefilter_end= '^(\} stopped cleartomark|\}featurecleanup)';
	  $exclude_rangefilter_begin= '^(?i)%%BeginNonPPDFeature'; #'
	  $exclude_rangefilter_end= '^(?i)%%EndNonPPDFeature';
	  #$triggered_rangefilter_begin= ''; #'
	  #$triggered_rangefilter_end= ''; #'
	  $fixsearchpat='(^|\s)(initmatrix|initclip|initgraphics)(\s|$)';
	  $fixreplacepat=' ';
	  $fixmsgprinted= 1; # stop printing message
        } # end if NTPS
      } #end else
    }
    } #end if trytofixps

    if (defined($tmpfhandle))
    { 
      print $tmpfhandle $_ or die "$prgname: Failure during writing to temporary file $tmpfname";
    }

    if (/^%%EOF\s*$/) 
    {
	$totalEOFDSC++
    }
  } #end while <$infhandle>
  
  if (defined($tmpfhandle)) 
  { 
    close($tmpfhandle); 
  }
  else
  { 
    $tmpfhandle= $infhandle;
    $tmpfname= $infname; 
  }
  
  ## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  ## -- calculate the bounding box --
  ## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if ($translate_x!=0 || $translate_y!=0)
  {
    $translation="$translate_x $translate_y translate";
    $translatecmd="-c $translation";
  }
  else
  {
    $translation="";
    $translatecmd="";
  }

  if (!$quiet) 
  { 
    print STDERR "Calculating Bounding Box...";
    if ($opt_s)
    {
	print STDERR "using page size $opt_s...";
    }
  }

  $rotatecmd='';
  if ($rotate ne '')
  {
     $rotatecmd="-c \"$rotate\""
  }
  if ($bboxname ne '')
  {
      $cmdline="$ghostscriptname -dSAFER -dNOPAUSE -q $gpar -r$resolution -sDEVICE=$device -sOutputFile=- $translatecmd -f $tmpfname -c showpage -c quit | $bboxname -r $resolution";
  }
  else
  {
      if (!$quiet) {
	  print STDERR "...using bbox device of $ghostscriptname...";
      }
      $cmdline = "$ghostscriptname -dSAFER -dBATCH -dNOPAUSE -q $gpar -r$resolution -sDEVICE=bbox -sOutputFile=- -c \"/setpagedevice {pop} def\" $translatecmd -f $tmpfname -c quit 2>&1";
  }

  if ($debuggs) { print STDERR "\nCalling: $cmdline\n"; }
  
  # actual ghostscript call
  $boundingbox=`$cmdline`;
  if ($debuggs) { print STDERR "Call result: $boundingbox"; }

  # check result of gs call
  if ($boundingbox !~ /^%%BoundingBox/m)
  {
    print STDERR "Error: Could not determine bounding box!\n",
    "I suppose $ghostscriptname had some trouble interpreting the postscript-file\n";
  }

  $boundingbox =~ /^%%HiResBoundingBox:\s*(.*)/m;
  if (defined($1)) # HiResBoundingBox given
  {
      ($hcBBllx,$hcBBlly,$hcBBurx,$hcBBury,$dummy)= split(/\s/,$1);
      $hiresboundingbox="%%HiResBoundingBox: $hcBBllx $hcBBlly $hcBBurx $hcBBury\n";
      $cBBllx = floor($hcBBllx);
      $cBBlly = floor($hcBBlly);
      $cBBurx = ceil($hcBBurx);
      $cBBury = ceil($hcBBury);
  }
  else
  { #use normal BoundingBox
      $boundingbox =~ /^%%.*BoundingBox:\s*(.*)/;
      ($cBBllx,$cBBlly,$cBBurx,$cBBury,$dummy)= split(/\s/,$1);
  }
  # if loose BB should be performed by ps2eps
  # apply changes to resulting bounding box if needed
  if ($looseBB ne '') 
  {
      if ($cBBllx > 0) { $cBBllx--; }
      if ($cBBlly > 0) { $cBBlly--; }
      $cBBurx++; $cBBury++;
  }
  # extend clipping box by 1 point
  if ($clip)
  {
      if ($cBBllx > 0) { $cBBllx--; }
      if ($cBBlly > 0) { $cBBlly--; }
      $cBBurx++; $cBBury++;
  }
  if ($cBBllx < 0 || $cBBlly < 0)
  {
    if (!$quiet) 
    {
      print STDERR "WARNING! Your drawing had a negative Bounding Box which is deprecated and may cause problems!. I'll shift it.\n";
    }
    $translate_x= -int($cBBllx);
    $translate_y= -int($cBBlly);
    $cBBllx=0;
    $cBBurx= $cBBurx + $translate_x;
    $cBBlly=0;
    $cBBury= $cBBury + $translate_y;

    $hcBBurx= $hcBBurx + $hcBBllx;
    $hcBBury= $hcBBury + $hcBBlly;
    $hcBBllx= 0;
    $hcBBlly= 0;

    $hiresboundingbox="%%HiResBoundingBox: $hcBBllx $hcBBlly $hcBBurx $hcBBury\n";

    $translation="$translate_x $translate_y translate";
    $translatecmd="-c \'$translation\'";
  }
  $boundingbox = "%%BoundingBox: $cBBllx $cBBlly $cBBurx $cBBury\n";

  if (!$quiet) { print STDERR "ready. $boundingbox" };
  
  $before_startps= 1;
  $inserted_prolog= 0;
  $prolog_passed= 0;

  ## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  ## -- Create output file --
  ## %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if (!$quiet) { print STDERR "Creating output file $outfname..."; }
  
  open($tmpfhandle,"<$tmpfname") or die "Cannot open file $tmpfname for reading";
  CREATEOUTPUT: 
  while (<$tmpfhandle>) 
  {
    # check whether we are in a binary section

    $binarysection=(/^(%%BeginBinary)|(beginimage)\r?\n?$/ ... /^(%%EndBinary)|^(endimage)/) || 
                   (/^(doNimage)|(doclutimage)\r?\n?$/ ... /(^|~> )Z\r?\n?$/) || # Pscript_Win_Dib_L2 5.0 0
                   (/^beginjpeg / ... /~> endjpeg\r?\n?$/) ||             # Pscript_Win_Dib_L2 5.0 0
		   (/^pdfIm/ ... /^%-EOD-/);

    if ( !$binarysection )
    {
      s/\r?\n?$//;  # remove CR and/or LF at end of line if not in binary section
    }

    # check where magic Postscript header starts - skip leading binary stuff, e.g., HP PCL/PJL code
    if ($before_startps)
    {
      if ( /%!/ ) # This is usually the smallest magic sequence
      {           # Note: Adobe Photoshop generated a binary heading, so ^ is not applicable
	if (! /%!PS-Adobe.*/i) # some strange programs use other magics
	{
	   print STDERR "** Warning **: Weird heading line -- ",$_," -- ";
	}
	$before_startps= 0; 
      }
      next CREATEOUTPUT;
    }
    else # we are hopefully in regular postscript code now
    {
      # count %%EOFs as we want to know when we got the last EOF
      if ( /^%%EOF\s*$/ ) 
      {
	  $seenEOF++;
      }
      
      # We should insert our own prologue including the newly calculated BoundingBox
      if (! $inserted_prolog)
      {
        print $outfhandle "%!PS-Adobe-$PSversion EPSF-$PSDSCversion\n";
	# check if we need to rotate
	$transrotcmd='';
	if ($rotatecmd)
	{
	  if ($rotate eq $rotright)
	  {
	      $transrotcmd="-$cBBlly $cBBurx translate";
	      $boundingbox='%%BoundingBox: 0 0 ' . ($cBBury-$cBBlly) . ' ' . ($cBBurx-$cBBllx) . "\n";
	      if ($hiresboundingbox ne "")
	      {
		  $hiresboundingbox='%%HiResBoundingBox: 0 0 '  . ($hcBBury-$hcBBlly) . ' ' . ($hcBBurx-$hcBBllx) . "\n";
	      }

	  }
	  elsif ($rotate eq $rotleft)
	  {
	      $transrotcmd="$cBBury -$cBBllx translate";
	      $boundingbox='%%BoundingBox: 0 0 ' . ($cBBury-$cBBlly) . ' ' . ($cBBurx-$cBBllx) . "\n";
	      if ($hiresboundingbox ne "")
	      {
		  $hiresboundingbox= '%%HiResBoundingBox: 0 0 ' . ($hcBBury-$hcBBlly) . ' ' . ($hcBBurx-$hcBBllx) . "\n";
	      }
	  }
	  elsif ($rotate eq $rotupsidedown)
	  {
	      $transrotcmd="$cBBurx $cBBury translate";
	      $boundingbox='%%BoundingBox: 0 0 ' . ($cBBurx-$cBBllx) . ' ' . ($cBBury-$cBBlly) . "\n";
	      if ($hiresboundingbox ne "")
	      {
		  $hiresboundingbox='%%HiResBoundingBox: 0 0 ' . ($hcBBurx-$hcBBllx) . ' ' . ($hcBBury-$hcBBlly) . "\n";
	      }
	  }
	}
        print $outfhandle $boundingbox;
	if (!defined($hiresboundingbox)) 
	{
	    $nohires=1;
	}
        if (defined($hiresboundingbox) && !defined($nohires)) { print $outfhandle $hiresboundingbox; }
        $inserted_prolog= 1;
        redo CREATEOUTPUT;
      }
      else # already inserted_prolog
      {
        if (! $prolog_passed)
        {   
          #ignore the following lines in the prologue
          if ( /^%%(HiRes)?BoundingBox/ ||
               /^%%Pages/       ||
               /^%%BeginProlog/ ||
               /^%%EndProlog/   ||
	       ($filterorientation && /^%%Orientation/) ||
               ($removeDSC && /^%%.*: \(atend\)/) ||
	       ($removePreview && (/^%%BeginPreview/ ... /^%%EndPreview/)) ) 
          { 
	      next CREATEOUTPUT; 
	  }
          else
          {
            if ( /^[^%].*/ ||
                 /^%%EndComments/ ) # line is not a comment
            { 
              #output postscript code for proper EPS file
              if ($insertPScode) 
              { 
		  print $outfhandle "%%EndComments\n", 
		                    "% EPSF created by ps2eps $ver[2]\n",
		                    "%%BeginProlog\n";
              }
	      # Insert own postscript code for clipping
              if ($clip)
              {
		 if (!defined($nohires))
		 {
		     printf $outfhandle "newpath %f %f moveto %f %f lineto %f %f lineto %f %f lineto closepath clip\n",$hcBBllx-0.1,$hcBBlly-0.1,$hcBBurx+0.1,$hcBBlly-0.1,$hcBBurx+0.1,$hcBBury+0.1,$hcBBllx-0.1,$hcBBury+0.1;
		 }
		 else
		 {
		     printf $outfhandle "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto closepath clip\n",$cBBllx,$cBBlly,$cBBurx,$cBBlly,$cBBurx,$cBBury,$cBBllx,$cBBury;
		 }
              } #endif clip

	      if ($rotate ne '')
	      {
		  print $outfhandle "$transrotcmd\n";
		  print $outfhandle "$rotate\n";
	      }
	      if ($translation ne '')
	      {
		  print $outfhandle "$translation\n";
	      }

	      #insert surrounding postscript code
              if ($insertPScode) 
              {              
                print $outfhandle  "save\n",
                                   "countdictstack\n",
                                   "mark\n",
                                   "newpath\n",
                                   "/showpage {} def\n",
		                   "/setpagedevice {pop} def\n",
                         	   "%%EndProlog\n",
                                   "%%Page 1 1\n";
              }
              $prolog_passed= 1;
              if (/^%%EndComments/) { next CREATEOUTPUT; }
            } #endif line is not a comment
          } #end else
        } #endif (we are in the prologue section)
        else #we are in the main part of postscript file
        {
          #end of postscript file reached?
	  #Usually the DSC %%EOF signifies the end
          if ( eof($tmpfhandle) || 
	       ($ignoreEOFDSC == 0 && /^%%EOF\s*$/ && $seenEOF == $totalEOFDSC)
               || ( $trailerseen && /^II\*\000.*/ )	       
             ) 
          { 
	    #do not forget to print last line if not terminated by LF
	    if ( eof($tmpfhandle) && !/^$/ && !/^%%EOF\s*$/ ) # do not insert %%EOF twice
	    {
		print $outfhandle $_,"\n";
	    }
            #add appropriate trailer
            if ($insertPScode) 
	    {              
		print $outfhandle "%%Trailer\n",
		                  "cleartomark\n", 
		                  "countdictstack\n", 
                                  "exch sub { end } repeat\n",
		                  "restore\n",
		                  "%%EOF\n"; 
	    }
            last CREATEOUTPUT; 
          } # stop output

	  # Trailer comment seen?
	  if ( /^%%Trailer\s*$/ )
	  {
	    $trailerseen=1;
	  }
	  else
	  {
	    if (!/^\s*$/) #non empty lines follow
	    {
		$trailerseen=0;
	    }
	  }

          # check for trigger
          if (defined($triggerstring) && /^$triggerstring$/)
          {
            $trigger= 1;
          };

          # remove complete lines if one of the expression matches
          if ( !$binarysection # only when not in binary section
	       &&
	       (
	       ($removePreview && (/^%%BeginPreview/ ... /^%%EndPreview/))
	       ||                        # no preview
               (defined($rangefilter_begin) && 
                 (/$rangefilter_begin/ ... /$rangefilter_end/) &&
		(!(/$exclude_rangefilter_begin/ ... /$exclude_rangefilter_end/))
               ) 
               ||
               (defined($triggered_rangefilter_begin) && defined($triggered_rangefilter_end) &&
                $trigger &&
                (/$triggered_rangefilter_begin/ ... /$triggered_rangefilter_end/)
               ) 
	       ||
               /$linefilter/             # lines by linefilter
	       ||
	       ($removeDSC && (/^%( |!)(\w )+/ || /^%%/)) # any type of structured comment
	       ||
	       ($removeADO && 
		(/^statusdict begin.*ProductName.*print product print.*flush end\r?\n?$/ ||
                 /^\(%%\[\s*(Page:.*|LastPage)\s*\]%%\)\s*=\s*\w*\s*\r?\n?/ ))
	       ||                    
	       /^$/                       # empty lines
               )
             )
          { 
	      next CREATEOUTPUT; 
	  }

	  # replacement
	  if ( defined($fixsearchpat) )
	  {
	      #if (/$fixsearchpat/) { print STDERR "**filter** before:",$_,"\n"; }
	      #if (s/$fixsearchpat/$fixreplacepat/) {	      print STDERR "**filter** after:",$_,"\n";}
              s/$fixsearchpat/$fixreplacepat/;
	  }
  
          # sanity check for potential dangerous commands
          if ( /(^|\s)(clear|erasepage|initmatrix|initclip|initgraphics|startjob|cleardictstack|setmatrix|setpagedevice|copypage|grestoreall|exitserver|quit)(\s|$)/ )
          { 
	      $notsane= 1;
	      #print STDERR "Warning: dangerous command in line: ",$_,"\n";
	  }
        } # end else (this is main part) 

        # Output the postscript line to result file
        print $outfhandle $_;

	if (!$binarysection)
	{
	    print $outfhandle "\n"; # terminate line with LF
	}
      } # end else prolog_passed
    } # end else inserted_prolog
  } # end while CREATEOUTPUT

  close($tmpfhandle);

  if ($tmpfname ne $infname) { unlink "$tmpfname"; } #remove temporary file

  close ($outfhandle);

  # print warning if magic sequence not found
  if ( $before_startps ) 
  {
    print STDERR "\n ** Error!! **: could not identify begin of postscript code in file $infname, please check header line!\n First line should start with %!. No output generated.\n";
  }

  if (!$quiet) { print STDERR "ready.\n"; }
  if ($warnings and $notsane and !$quiet)
  { 
    print STDERR "** Warning **: EPS-output for $infname is not sane, at least one\n",
                 "of the following commands was still present:\n",
                 "clear erasepage initmatrix initclip initgraphics startjob\n",
                 "cleardictstack setmatrix setpagedevice copypage grestoreall\n",
                 "exitserver quit\n";
  }
} #end while PROCESSFILE

# ---- end of perl-script -------
