#!/usr/bin/perl
use v5.14;
use strict;                            # to be sure, that all is safe ... :-)
use File::Path;
use File::Copy;
use File::Basename;
use IO::File;
use Getopt::Long;
use autodie;

my $tempDir  = ".";                    # temporary directory
my $clear    = 0;                      # 0 or 1, clears all temporary files
my $DPI      = "300";                  # value for ppm 
my $margins  = "0";                    # margins for pdf crop
my $imageDir = "images";               # directorio de imágenes (images por defecto)
my $skip     = "skip";                 # skip environment
my $latex    = 0;                      # 1->create all images using latex
my $xetex    = 0;                      # 1->create all images using xelatex
my $luatex   = 0;                      # 1->create all images using lualatex
my $IMO      = "";                     # 1->Options for ImageMagick
my $GSCMD    = "gs";                   # Ghostscript name
my $other    = "other";                # search other environment for export
my $pdf      = 0;                      # 1->create .pdf using ghostscript
my $png      = 0;                      # 1->create .png 
my $jpg      = 0;                      # 1->create .jpg 
my $eps      = 0;                      # 1->create .eps using pdftops
my $ppm      = 0;                      # 1->create .ppm using pdftoppm
my $useppm   = 0;                      # 1->create jpg/png using mogrify and ppm
my $usemog   = 0;                      # 1->create jpg/png using mogrify
my $all      = 0;		       # 1->create all images and files for type

#----------------------- User part end ---------------------------
#-----------------program identification, options and help--------
my $program   = "ltximg";
my $ident     = '$Id: ltximg v1.0, 2013-03-12 pablo $';
my $copyright = <<END_COPYRIGHT ;
Copyright 2013-01-12 (c) Pablo Gonzalez L <pablgonz\@yahoo.com>
END_COPYRIGHT
my $licensetxt = <<END_LICENSE ;
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
${title}Usage: $program file.tex  [options]
ltximg create a copy for TeX source whit all TiKZ|Pstricks environments
       and convert all environments into single images (pdf/png/eps/jpg). 
       By default search and extract TiKZ environments using (pdf)LaTeX 
       and ghostscript (gs).       

Options:

  --h|help          - display this help and exit
  --v|version       - display version information and exit
  --li|license      - display license information and exit
  --imageDir=       - the dir for the created images (default images)
  --d|DPI=          - dots per inch for gs, pdftoppm and mogrify (default 300)
  --IMO="..."       - aditional options for mogrify (need double quotes)
  --c|clear         - delete all temp files
  --xe|xetex        - create all image using xelatex (tikz and pstricks)
  --lu|luatex       - create all image using lualatex (tikz)
  --la|latex        - create all image using latex (pstricks)
  --up|useppm       - create jpg and png using mogrify and ppm
  --um|usemog       - create jpg and png (transparent) using mogrify and pdf
  --m|margins=      - margins for pdfcrop (default 0)
  --pdf             - create .pdf files using gs
  --ppm             - create .ppm files (need pdftoppm)
  --eps             - create .eps files (need pdftops)
  --jpg             - create .jpg files (deafult use gs)
  --png             - create .png files (deafult use gs)
  --s|skip=         - name for skip environmet in input file (default skip)
  --o|other=        - name for other export environmet (default other)
  --a|all	    - create pdf/jpg/png/eps image type

Example:
* $program test.tex --xe -eps -jpg -c 
* produce test-fig-1.pdf, test-fig-2.pdf, test-fig-3.pdf,...
* and test-fig-1.eps, test-fig-2.eps, test-fig-3.eps,... and 
* test-fig-1.jpg, test-fig-2.jpg, test-fig-3.jpg,... for all 
* TiKZ|Pst environments in image dir using gs and (Xe)latex.
END_OF_USAGE

# Options
my $result = GetOptions ("imageDir=s"   => \$imageDir,
			"s|skip=s"      => \$skip,
			'h|help'        => \$::opt_help,
			'v|version' 	=> \$::opt_version,
			'li|license' 	=> \$::opt_license,
			'd|DPI=i'    	=> \$DPI,
			"tempDir=s"     => \$tempDir,
			'c|clear'       => \$clear,
			"la|latex"      => \$latex,
			"xe|xetex"      => \$xetex,
			"lu|luatex"     => \$luatex,
			"o|other=s"     => \$other,
			"m|margins=s"   => \$margins,
			"GSCMD=s"       => \$GSCMD,
			"IMO=s"         => \$IMO,
			"png"           => \$png,
			"jpg"           => \$jpg,
			"eps"           => \$eps,
			"ppm"           => \$ppm,
			"up|useppm"     => \$useppm,
			"um|usemog"     => \$usemog,
			"pdf"           => \$pdf,
			"a|all"         => \$all
			) or die $usage;

# help functions
sub errorUsage { die "Error: @_ (try --help for more information)\n"; }
 
# options for command line
if ($::opt_help) {
    print $usage;
    exit(0);
}
if ($::opt_version) {
    print $title;
    print $copyright;
    exit(0);
}
if ($::opt_license) {
    print $licensetxt;
    exit(0);
} 
# General options
if ($latex) {
    $latex = 1;
}
if ($xetex) {
    $xetex = 1;
}
if ($luatex) {
    $luatex = 1;
}
if ($pdf) {
    $pdf = 1;
}
if ($eps) {
    $pdf =$eps = 1;
}
if ($jpg) {
    $jpg = 1;
}
if ($png) {
    $png = 1;
}
if ($ppm) {
    $ppm = 1;
}
if ($^O eq 'MSWin32'){
    $GSCMD    = "gswin32c";
}
if ($^O eq 'MSWin64'){
    $GSCMD    = "gswin64c";
}
if ($useppm){
    $ppm = $useppm   = 1;
}
if ($usemog){
    $pdf = $usemog   = 1;
}
if ($all){
    $pdf =$eps = $png = $jpg  = 1;
}
## Create image dir
if (! -e $imageDir) {
 mkdir($imageDir,0744) or die "Can't create $imageDir";
}

# open file
my $InputFilename = "";
@ARGV > 0 or errorUsage "Input filename missing";
@ARGV < 2 or errorUsage "Unknown option or too many input files";
$InputFilename = $ARGV[0];
 
# end open file
my @SuffixList = ( ".tex", "", ".ltx" );    # possible extensions
my ( $name, $path, $ext ) = fileparse( $ARGV[0], @SuffixList );
if ( $ext eq "" ) { $ext = ".tex"; }   # me need the extension as well
my $TeXfile = "$path$name$ext";

# Define in file
my $archivo_entrada = shift;

# Read in file 
open my $ENTRADA, '<', $archivo_entrada;
my $archivo;
{
    local $/;
    $archivo = <$ENTRADA>;
}
close   $ENTRADA;
## cambiar
$archivo =~
s/(?<inicio>\\begin\{)(?<verb>verbatim|verbatim\*|lstlisting|LTXexample|Verbatim|comment|alltt|minted|tcblisting|xcomment|$skip) (?:\}) 
/\\begin\{nopreview}
$+{inicio}$+{verb}\}/gmxs;

$archivo =~
s/(?<fin>\\end\{)(?<verb>verbatim|verbatim\*|lstlisting|LTXexample|Verbatim|comment|alltt|minted|tcblisting|xcomment|$skip) (?:\}) 
/$+{fin}$+{verb}\}
\\end\{nopreview}/gmxs;
# Save
open my $SALIDA, '>', "$tempDir/$name-tmp.tex";
print $SALIDA $archivo;
close $SALIDA;

# Add preview in preamble

open my $INFILE,'<', "$tempDir/$name-tmp.tex";
open my $OUTFIlE,'>',"$tempDir/$name-fig.tex";
print $OUTFIlE "\\AtBeginDocument\{\n";
	if($xetex){
        print $OUTFIlE "\\RequirePackage\[xetex,active,tightpage\]\{preview\}\n";
        print $OUTFIlE "\\PreviewEnvironment\{tikzpicture\}\n";
	print $OUTFIlE "\\PreviewEnvironment\{pspicture\}\n";
        print $OUTFIlE "\\PreviewEnvironment\{$other\}\}\n";
        }
    elsif($latex){
        print $OUTFIlE "\\RequirePackage\[active,tightpage\]\{preview\}\n";
        print $OUTFIlE "\\PreviewEnvironment\{pspicture\}\n";
        print $OUTFIlE "\\PreviewEnvironment\{$other\}\}\n";        
        }
	else {
		print $OUTFIlE "\\RequirePackage\[pdftex,active,tightpage\]\{preview\}\n";
		print $OUTFIlE "\\PreviewEnvironment\{tikzpicture\}\n";
        print $OUTFIlE "\\PreviewEnvironment\{$other\}\}\n";		
		}
        
	while ( my $line = <$INFILE> ) {
            print $OUTFIlE $line;
        }
close $INFILE;
close $OUTFIlE;

if ($clear) {unlink "$tempDir/$name-tmp.tex";}

# Parser inline verbatim 

## Lectura del archivo
open my $PARSER, '<', "$tempDir/$name-fig.tex";
undef $/;       # read al file 
my $file_PARSER = <$PARSER>;
close   $PARSER;

## Partición del documento
my($cabeza,$cuerpo,$final) = $file_PARSER =~ m/\A (.+? ^\\begin{document}) \s* (.+?) \s* (^ \\end{document}) \s* \z/msx;
 
## Cambios a realizar
my %cambios = (
    '\pspicture'                => '\TRICKS',
    '\endpspicture'             => '\ENDTRICKS',
 
    '\begin{pspicture'          => '\begin{TRICKS',
    '\end{pspicture'            => '\end{TRICKS',

    '\begin{postscript}'        => '\begin{POSTRICKS}',
    '\end{postscript}'          => '\end{POSTRICKS}',
    
    '\begin{tikzpicture'        => '\begin{TIKZPICTURE',
    '\end{tikzpicture'          => '\end{TIKZPICTURE',
    
    "\\begin\{$other"           => '\begin{OTHER',
    "\\end\{$other"             => '\end{OTHER',
);
 
 
## Variables y constantes
my $no_del = "\0";
my $del    = $no_del;
 
## Reglas
my $llaves      = qr/\{ .+? \}                                                                  /x;
my $no_corchete = qr/(?:\[ .+? \])?                                                             /x;
my $delimitador = qr/\{ (?<del>.+?) \}                                                          /x;
my $verb        = qr/verb [*]?                                                                  /ix;
my $lst         = qr/lstinline (?!\*) $no_corchete                                              /ix;
my $mint        = qr/mint      (?!\*) $no_corchete $llaves                                      /ix;
my $marca       = qr/\\ (?:$verb | $lst | $mint) (\S) .+? \g{-1}                                /x;
my $comentario  = qr/^ \s* \%+ .+? $                                                            /mx;
my $definedel   = qr/\\ (?:   DefineShortVerb | lstMakeShortInline  ) $no_corchete $delimitador /ix;
my $indefinedel = qr/\\ (?: UndefineShortVerb | lstDeleteShortInline) $llaves                   /ix;
 
 
# Changes

while ($cuerpo =~
    /   $marca
    |   $comentario
    |   $definedel
    |   $indefinedel
    |   $del .+? $del                                          # delimitado
    /gimx) {
 
    my($pos_inicial, $pos_final) = ($-[0], $+[0]);              # posiciones
    my $encontrado = ${^MATCH};                                 # lo encontrado
 
    given ($encontrado) {
        when (/$definedel/) {                                   # definimos delimitador
            $del = $+{del};
            $del = "\Q$+{del}" if substr($del,0,1) ne '\\';     # es necesario "escapar" el delimitador
        }
        when (/$indefinedel/) {                                 # indefinimos delimitador
            $del = $no_del;
        }
        default {                                               # Aquí se hacen los cambios
            while (my($busco, $cambio) = each %cambios) {
 
                $encontrado =~ s/\Q$busco\E/$cambio/g;          # es necesario escapar $busco, ya que contiene caracteres extraños
            }
 
            substr $cuerpo, $pos_inicial, $pos_final-$pos_inicial, $encontrado; # insertamos los nuevos cambios
 
            pos($cuerpo)= $pos_inicial + length $encontrado;                    # reposicionamos la siguiente búsqueda
        }
    }
}
 
# Write

open my $OUTPARSER, '>', "$tempDir/$name-fig.tex";
print   $OUTPARSER "$cabeza\n$cuerpo\n$final\n";
close   $OUTPARSER;

# Compiling files...

# Using LaTeX

if($latex){
	print "Compiled using LaTeX\n";
        system("latex -interaction=batchmode $tempDir/$name-fig.tex");
        system("dvips -q -Ppdf $tempDir/$name-fig.dvi");
        system("ps2pdf -dPDFSETTINGS=\/prepress $tempDir/$name-fig.ps $tempDir/$name-fig.pdf");
if ($clear) {
		unlink "$tempDir/$name-fig.dvi";
		unlink "$tempDir/$name-fig.ps";
		unlink "$tempDir/$name-fig.log";
		unlink "$tempDir/$name-fig.aux";
		unlink "$tempDir/$name-fig.tex";
		}
	}
	
# Using XeLaTeX

elsif($xetex){
	print "Compiled using (Xe)LaTeX\n";
        system("xelatex -interaction=batchmode $tempDir/$name-fig.tex");
if ($clear) {
		unlink "$tempDir/$name-fig.log";
		unlink "$tempDir/$name-fig.aux";
		unlink "$tempDir/$name-fig.tex";
		}        
	}
	
# Using LuaLaTeX

elsif($luatex){
	print "Compiled using (lua)LaTeX\n";
        system("lualatex -interaction=batchmode $tempDir/$name-fig.tex");
if ($clear) {
		unlink "$tempDir/$name-fig.log";
		unlink "$tempDir/$name-fig.aux";
		unlink "$tempDir/$name-fig.tex";
		}        
	}

# Using pdfLaTeX (default)	
	
else{
	print "Compiled using (pdf)LaTeX\n";
        system("pdflatex -interaction=batchmode $tempDir/$name-fig.tex");
if ($clear) {
		unlink "$tempDir/$name-fig.log";
		unlink "$tempDir/$name-fig.aux";
		unlink "$tempDir/$name-fig.tex";
		}        
	}
	
# Croping

print "Done, $name-fig.pdf need croping, using pdfcrop whit margin $margins\n";
system("pdfcrop --margins $margins $tempDir/$name-fig.pdf $tempDir/$name-fig.pdf");

# Gs for split file

if ($pdf) {
print "Create pdf figs in $imageDir dir using Ghostscript \($GSCMD\)\n";
system("$GSCMD -q -sDEVICE=pdfwrite -dPDFSETTINGS=\/prepress -dNOPAUSE -dBATCH -sOutputFile=$imageDir/$name-fig-%d.pdf $tempDir/$name-fig.pdf");

# Delete last  pdf (bug from gs) 

my $i = 1;
while (-e "$imageDir/$name-fig-${i}.pdf") {     # if exisit search
    $i++;                       # next 
}                               # search end
 
$i--;                           # back the last
 
unlink "$imageDir/$name-fig-${i}.pdf"; 

}

# Fix pdftops error message in windows

if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){
open my $ppmconf, '>', "$tempDir/xpd";
print $ppmconf <<'EOH';
errQuiet yes
EOH
close $ppmconf;
    }

# Create eps files using pdftops
  
if ($eps) {
print "Create eps figs in $imageDir dir using pdftops\n";
# Search pdf in $imageDir
	for my $image_pdf (<$imageDir/*.pdf>) {
	    
if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){
	system("pdftops -cfg $tempDir/xpd -q -level3 -eps $image_pdf");
										}
else{
	system("pdftops -level3 -eps $image_pdf"); 
										}
					}
	}
	
# Create ppm files and renamig files 
	
if($ppm){
print "Create ppm figs in $imageDir dir using pdftoppm\n";
if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){
	system("pdftoppm -cfg $tempDir/xpd -q -r $DPI $tempDir/$name-fig.pdf $imageDir/$name-fig");
	}
else{
	system("pdftoppm -r  $DPI $tempDir/$name-fig.pdf $imageDir/$name-fig");
	}
	
# Renaming .ppm (only need in windows)

if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){
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
			}
    }

# Change dir and create png/jpg formats using mogrify and ppm

if($usemog){
chdir ($imageDir) or die "$imageDir not exist\n";
if ($png) {
print "Create png transparent in $imageDir dir using mogrify and pdf\n";	
system("mogrify $IMO -define png:format=png32 -define png:compression-filter=4 -quality 100 -transparent white -density $DPI -format png \*\.pdf");    
	}	
if ($jpg) {
print "Create jpg figs in $imageDir dir using mogrify and pdf\n";	
system("mogrify $IMO  -quality 100 -density $DPI -format jpg \*\.pdf");       
	}
}
elsif($useppm){
chdir ($imageDir) or die "$imageDir not exist\n";
if ($png) {
print "Create png figs in $imageDir dir using mogrify and ppm\n";	
system("mogrify $IMO -quality 100 -define png:format=png32 -define png:compression-filter=4 -density $DPI -format png \*\.ppm");       
	}	
if ($jpg) {
print "Create jpg figs in $imageDir dir using mogrify and ppm\n";	
system("mogrify $IMO -quality 100 -density $DPI -format jpg \*\.ppm");       
	}
}
else{

# Create png using gs (default)

if ($png) {
print "Create png figs in $imageDir dir using Ghostscript \($GSCMD\)\n";
system("$GSCMD -q -sDEVICE=png16m -r$DPI -dGraphicsAlphaBits=4 -dTextAlphaBits=4 -dNOPAUSE -dBATCH -sOutputFile=$imageDir/$name-fig-%d.png $tempDir/$name-fig.pdf");
	}

if ($jpg) {
print "Create jpg figs in $imageDir dir using Ghostscript \($GSCMD\)\n";
system("$GSCMD -q -sDEVICE=jpeg -r$DPI -dJPEGQ=100 -dGraphicsAlphaBits=4 -dTextAlphaBits=4 -dNOPAUSE -dBATCH  -sOutputFile=$imageDir/$name-fig-%d.jpg $tempDir/$name-fig.pdf");
	 }
 }


print "Finish, all figures are in $imageDir dir\n";
 
__END__
