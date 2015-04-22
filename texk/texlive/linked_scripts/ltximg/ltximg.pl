eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q' if 0;
use v5.18;
use File::Path;               # creating/removing dirs
use File::Copy;               # copying files
use File::Basename;	      # scan argument
use IO::File;                 # simple IO operation
use Getopt::Long qw(:config bundling_override); # read parameter and activate "bundling"
use autodie;		      # more safe
use Config;
use File::Spec;
use File::Find;

#------------------------ Constantes ----------------------------------#
my $tempDir  = ".";		# temporary directory
my $other    = "other";		# other environment for search
my $imageDir = "images";        # Dir for images (images default)
my $ignore   = "ignore";      	# ignore verbatim environment
my $exacount = 1;		# Counter for source images
my $imgNo     = 1;		# Counter for PGF/TIKZ/PST environments
my $nopreview = 0;		# 1->create images in nopreview mode
my $noSource = 0;		# Delete TeX source for images
my $nopdf    = 0;		# No create a PDF image file
my $clear    = 0;               # 0 or 1, clears all temporary files
my $DPI      = "150";           # value for ppm, png, jpg 
my $margins  = "0";             # margins for pdf crop
my $latex    = 0;             	# 1->create all images using latex
my $xetex    = 0;              	# 1->create all images using xelatex
my $luatex   = 0;              	# 1->create all images using lualatex
my $png      = 0;              	# 1->create .png using Ghoscript
my $jpg      = 0;              	# 1->create .jpg using Ghoscript
my $eps      = 0;              	# 1->create .eps using pdftops
my $svg      = 0;		# 1->create .svg files
my $ppm      = 0;             	# 1->create .ppm using pdftoppm
my $all      = 0;	       	# 1->create all images and files for type
my $output   = 0;	       	# 1->create output file whitout PGF/PST
my $miktex   = 0;	       	# 1->enable write 18 for miktex system
my $Verbose  = 0;		# 0 or 1, logfile  

#------------------------------ CHANGES -------------------------------#
# v1.2 2015-04-21 - Change mogrify to gs for image formats
#		  - Create output file
#                 - Rewrite source code and fix regex
#                 - Add more image format 
#	 	  - Change date to iso format
# v1.0 2013-12-01 - First public release 
#----------------------------------------------------------------------#

#----------------------------- Search GS ------------------------------#
# The next code its part of pdfcrop from TexLive 2014
# Windows detection 
my $Win = 0;
$Win = 1 if $^O =~ /mswin32/i;
$Win = 1 if $^O =~ /cygwin/i;

my $archname = $Config{'archname'};
$archname = 'unknown' unless defined $Config{'archname'};

# get Ghostscript command name
$::opt_gscmd = '';
sub find_ghostscript () {
    return if $::opt_gscmd;
    if ($Verbose) {
        print "* Perl executable: $^X\n";
        if ($] < 5.006) {
            print "* Perl version: $]\n";
        }
        else {
            printf "* Perl version: v%vd\n", $^V;
        }
        if (defined &ActivePerl::BUILD) {
            printf "* Perl product: ActivePerl, build %s\n", ActivePerl::BUILD();
        }
        printf "* Pointer size: $Config{'ptrsize'}\n";
        printf "* Pipe support: %s\n",
                (defined($Config{'d_pipe'}) ? 'yes' : 'no');
        printf "* Fork support: %s\n",
                (defined($Config{'d_fork'}) ? 'yes' : 'no');
    }
    my $system = 'unix';
    $system = "dos" if $^O =~ /dos/i;
    $system = "os2" if $^O =~ /os2/i;
    $system = "win" if $^O =~ /mswin32/i;
    $system = "cygwin" if $^O =~ /cygwin/i;
    $system = "miktex" if defined($ENV{"TEXSYSTEM"}) and
                          $ENV{"TEXSYSTEM"} =~ /miktex/i;
    print "* OS name: $^O\n" if $Verbose;
    print "* Arch name: $archname\n" if $Verbose;
    print "* System: $system\n" if $Verbose;
    my %candidates = (
        'unix' => [qw|gs gsc|],
        'dos' => [qw|gs386 gs|],
        'os2' => [qw|gsos2 gs|],
        'win' => [qw|gswin32c gs|],
        'cygwin' => [qw|gs gswin32c|],
        'miktex' => [qw|mgs gswin32c gs|]
    );
    if ($system eq 'win' or $system eq 'cygwin' or $system eq 'miktex') {
        if ($archname =~ /mswin32-x64/i) {
            my @a = ();
            foreach my $name (@{$candidates{$system}}) {
                push @a, 'gswin64c' if $name eq 'gswin32c';
                push @a, $name;
            }
            $candidates{$system} = \@a;
        }
    }
    my %exe = (
        'unix' => '',
        'dos' => '.exe',
        'os2' => '.exe',
        'win' => '.exe',
        'cygwin' => '.exe',
        'miktex' => '.exe'
    );
    my $candidates_ref = $candidates{$system};
    my $exe = $Config{'_exe'};
    $exe = $exe{$system} unless defined $exe;
    my @path = File::Spec->path();
    my $found = 0;
    foreach my $candidate (@$candidates_ref) {
        foreach my $dir (@path) {
            my $file = File::Spec->catfile($dir, "$candidate$exe");
            if (-x $file) {
                $::opt_gscmd = $candidate;
                $found = 1;
                print "* Found ($candidate): $file\n" if $Verbose;
                last;
            }
            print "* Not found ($candidate): $file\n" if $Verbose;
        }
        last if $found;
    }
    if (not $found and $Win) {
        $found = SearchRegistry();
    }
    if ($found) {
        print "* Autodetected ghostscript command: $::opt_gscmd\n" if $Verbose;
    }
    else {
        $::opt_gscmd = $$candidates_ref[0];
        print "* Default ghostscript command: $::opt_gscmd\n" if $Verbose;
    }
}

sub SearchRegistry () {
    my $found = 0;
    eval 'use Win32::TieRegistry qw|KEY_READ REG_SZ|;';
    if ($@) {
        if ($Verbose) {
            print "* Registry lookup for Ghostscript failed:\n";
            my $msg = $@;
            $msg =~ s/\s+$//;
            foreach (split /\r?\n/, $msg) {
                print " $_\n";
            }
        }
        return $found;
    }
    my $open_params = {Access => KEY_READ(), Delimiter => '/'};
    my $key_name_software = 'HKEY_LOCAL_MACHINE/SOFTWARE/';
    my $current_key = $key_name_software;
    my $software = new Win32::TieRegistry $current_key, $open_params;
    if (not $software) {
        print "* Cannot find or access registry key `$current_key'!\n"
                if $::opt_verbose;
        return $found;
    }
    print "* Search registry at `$current_key'.\n" if $Verbose;
    my %list;
    foreach my $key_name_gs (grep /Ghostscript/i, $software->SubKeyNames()) {
        $current_key = "$key_name_software$key_name_gs/";
        print "* Registry entry found: $current_key\n" if $Verbose;
        my $key_gs = $software->Open($key_name_gs, $open_params);
        if (not $key_gs) {
            print "* Cannot open registry key `$current_key'!\n" if $Verbose;
            next;
        }
        foreach my $key_name_version ($key_gs->SubKeyNames()) {
            $current_key = "$key_name_software$key_name_gs/$key_name_version/";
            print "* Registry entry found: $current_key\n" if $Verbose;
            if (not $key_name_version =~ /^(\d+)\.(\d+)$/) {
                print "  The sub key is not a version number!\n" if $Verbose;
                next;
            }
            my $version_main = $1;
            my $version_sub = $2;
            $current_key = "$key_name_software$key_name_gs/$key_name_version/";
            my $key_version = $key_gs->Open($key_name_version, $open_params);
            if (not $key_version) {
                print "* Cannot open registry key `$current_key'!\n" if $Verbose;
                next;
            }
            $key_version->FixSzNulls(1);
            my ($value, $type) = $key_version->GetValue('GS_DLL');
            if ($value and $type == REG_SZ()) {
                print "  GS_DLL = $value\n" if $Verbose;
                $value =~ s|([\\/])([^\\/]+\.dll)$|$1gswin32c.exe|i;
                my $value64 = $value;
                $value64 =~ s/gswin32c\.exe$/gswin64c.exe/;
                if ($archname =~ /mswin32-x64/i and -f $value64) {
                    $value = $value64;
                }
                if (-f $value) {
                    print "EXE found: $value\n" if $Verbose;
                }
                else {
                    print "EXE not found!\n" if $Verbose;
                    next;
                }
                my $sortkey = sprintf '%02d.%03d %s',
                        $version_main, $version_sub, $key_name_gs;
                $list{$sortkey} = $value;
            }
            else {
                print "Missing key `GS_DLL' with type `REG_SZ'!\n" if $Verbose;
            }
        }
    }
    foreach my $entry (reverse sort keys %list) {
        $::opt_gscmd = $list{$entry};
        print "* Found (via registry): $::opt_gscmd\n" if $Verbose;
        $found = 1;
        last;
    }
    return $found;
}

find_ghostscript();

if ($Win and $::opt_gscmd =~ /\s/) {
    $::opt_gscmd = "\"$::opt_gscmd\"";
}
# end GS search 

#-----------------Program identification, options and help ------------#

my $program   = "LTXimg";
my $nv='1.1';
my $copyright = <<END_COPYRIGHT ;
2015-04-21 - Copyright (c) 2013-2015 by Pablo Gonzalez L.
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
my $title = "$program $nv, $copyright";
my $usage = <<"END_OF_USAGE";
${title}
Usage: ltximg file.tex [options]

LTXimg extract and convert all PGF|TiKZ|Pstricks environments from TeX 
  source into single images files (pdf/png/eps/jpg/svg) using Ghostscript. 
  By default search and extract environments using (pdf)LaTeX.

Environments suports by LTXimg:

    pspicture	tikzpicture	pgfpicture	forest	ganttchart
    tikzcd	circuitikz	dependency	other	postscript

Options:

  -h,--help          - display this help and exit
  -l,--license       - display license information and exit
  -v,--version 	     - display version (current $nv) and exit
  -d,--dpi=<int>     - the dots per inch for images (default $DPI)
  -j,--jpg           - create .jpg files (need Ghostscript)
  -p,--png           - create .png files (need Ghostscript)
  -e,--eps	     - create .eps files (need pdftops)
  -s,--svg	     - create .svg files (need pdf2svg)
  -P,--ppm	     - create .ppm files (need pdftoppm)
  -a,--all	     - create .(pdf,eps,jpg,png) images
  -c,--clear         - delete all temp and aux files
  -o,--output        - create a file-out.tex whitout PGF|TiKZ|PST code
  -m,--margins=<int> - margins in bp for pdfcrop (default 0)
  -np,--nopreview    - create images files whitout preview package
  -ns,--nosource     - delete all source for images files
  --miktex           - use --enable-write18 for MikTeX users
  --xetex            - using (Xe)LaTeX for create images
  --latex            - using LaTeX for create images
  --luatex           - using (Lua)LaTeX for create images
  --nopdf            - don't create a PDF image files (default off)
  --other=<string>   - search other environment (default other)
  --ignore=<string>  - skip verbatim environment (default ignore)
  --imgdir=<string>  - the folder for images (default images)
  
  
  
Example:
* ltximg test.tex -e -p -j -c -o --imgdir=pics
* produce test-out.tex whitout PGF|TiKZ|PST environments and create "pics"
* dir whit all images (pdf,eps,png,jpg) and source (.tex) for all related 
* parts using (pdf)LaTeX whit preview package and cleaning all tmp files. 
* Suport bundling for short options: ltximg test.tex -epjco --imgdir=pics
END_OF_USAGE

# Options
my $result=GetOptions (
	'h|help'     	=> \$::opt_help,
	'v|version'  	=> \$::opt_version,
	'l|license'  	=> \$::opt_license,
	'd|dpi=i'    	=> \$DPI,# numeric
	'm|margins=i'   => \$margins,# numeric
	'imgdir=s' 	=> \$imageDir, # string
	'ignore=s' 	=> \$ignore, # string
	'other=s' 	=> \$other, # string
	'c|clear'    	=> \$clear,    # flag
	'np|nopreview' 	=> \$nopreview, # flag
	'e|eps'      	=> \$eps, # flag
	'j|jpg'      	=> \$jpg, # flag
	'p|png'      	=> \$png, # flag
	'P|ppm'      	=> \$ppm, # flag
	's|svg'      	=> \$svg, # flag
	'a|all'      	=> \$all, # flag
	'miktex'     	=> \$miktex, # flag
	'nopdf'     	=> \$nopdf, # flag
	'o|output'     	=> \$output, # flag
	'xetex'    	=> \$xetex, # flag
	'latex'      	=> \$latex, # flag
	'luatex'     	=> \$luatex,# flag
	'ns|nosource'	=> \$noSource, # flag
	'Verbose'  	=> \$Verbose,
) or die $usage;

# help functions
sub errorUsage { die "@_ (try ltximg --help for more information)\n"; }
 
# options for command line
if ($::opt_help) {
    print $usage;
    exit(0);
}
if ($::opt_version) {
    print $title;
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

if ($nopdf) {
    $nopdf = 1;
}
if ($eps) {
    $eps = 1;
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
if ($svg) {
    $svg = 1;
}
if ($all){
    $eps = $png = $jpg  = 1;
}

if ($nopreview) {
  $nopreview= 1;
}

if ($output) {
  $output= 1;
}

if ($miktex) {
  $miktex= 1;
}
# open file

my $InputFilename = "";
@ARGV > 0 or errorUsage "Input filename missing";
@ARGV < 2 or errorUsage "Unknown option or too many input files";
$InputFilename = $ARGV[0];

#--------------------- Arreglo de la extensión ------------------------#
my @SuffixList = ('.tex', '', '.ltx'); # posible extensión
my ($name, $path, $ext) = fileparse($ARGV[0], @SuffixList);
$ext = '.tex' if not $ext; 

#---------------- Creamos el directorio para las imágenes -------------#
-e $imageDir or mkdir($imageDir,0744) or die "No puedo crear $imageDir: $!\n";

# Define in file
my $archivo_entrada = shift;

# Standart line ltximg run
print "$program $nv, $copyright";

##---------------------------- PARTE 1 -------------------------------##
#------------ Creamos un hash con los cambios para verbatim -----------#
my %cambios = (
# pst/tikz set    
    '\psset'                	=> '\PSSET',
    '\tikzset'		        => '\TIKZSET',
# pspicture    
    '\pspicture'                => '\TRICKS',
    '\endpspicture'             => '\ENDTRICKS',
# pspicture
    '\begin{pspicture'          => '\begin{TRICKS',
    '\end{pspicture'            => '\end{TRICKS',
# postscript
    '\begin{postscript}'        => '\begin{POSTRICKS}',
    '\end{postscript}'          => '\end{POSTRICKS}',
# $other    
    "\\begin\{$other"		=> '\begin{OTHER',
    "\\end\{$other"		=> '\end{OTHER',
# document
    '\begin{document}'          => '\begin{DOCTRICKS}',
    '\end{document}'            => '\end{DOCTRICKS}',
# tikzpicture
    '\begin{tikzpicture}'       => '\begin{TIKZPICTURE}',
    '\end{tikzpicture}'         => '\end{TIKZPICTURE}',
# pgfinterruptpicture
    '\begin{pgfinterruptpicture'=> '\begin{PGFINTERRUPTPICTURE',
    '\end{pgfinterruptpicture'  => '\end{PGFINTERRUPTPICTURE',
# pgfpicture
    '\begin{pgfpicture}'        => '\begin{PGFPICTURE}',
    '\end{pgfpicture}'          => '\end{PGFPICTURE}',
# ganttchart
    '\begin{ganttchart}'        => '\begin{GANTTCHART}',
    '\end{ganttchart}'          => '\end{GANTTCHART}',
# circuitikz
    '\begin{circuitikz}'        => '\begin{CIRCUITIKZ}',
    '\end{circuitikz}'          => '\end{CIRCUITIKZ}',
# forest     
    '\begin{forest}'       	=> '\begin{FOREST}',
    '\end{forest}'         	=> '\end{FOREST}',
# tikzcd 
    '\begin{tikzcd}'       	=> '\begin{TIKZCD}',
    '\end{tikzcd}'         	=> '\end{TIKZCD}',
# dependency
    '\begin{dependency}'       	=> '\begin{DEPENDENCY}',
    '\end{dependency}'         	=> '\end{DEPENDENCY}',
);

#------------------------ Coment inline Verbatim ----------------------#
open my $ENTRADA, '<', "$archivo_entrada";
my $archivo;
{
    local $/;
    $archivo = <$ENTRADA>;
}
close $ENTRADA;

# Variables y constantes
my $no_del = "\0";
my $del    = $no_del;

# Reglas
my $llaves      = qr/\{ .+? \}                                                                  /x;
my $no_corchete = qr/(?:\[ .+? \])?                                                             /x;
my $delimitador = qr/\{ (?<del>.+?) \}                                                          /x;
my $verb        = qr/(spv|v|V)erb [*]?                                                          /ix;
my $lst         = qr/lstinline (?!\*) $no_corchete                                              /ix;
my $mint        = qr/mint      (?!\*) $no_corchete $llaves                                      /ix;
my $marca       = qr/\\ (?:$verb | $lst | $mint ) (\S) .+? \g{-1}              			/x;
my $comentario  = qr/^ \s* \%+ .+? $                                                            /mx;
my $definedel   = qr/\\ (?:   DefineShortVerb | lstMakeShortInline  ) $no_corchete $delimitador /ix;
my $indefinedel = qr/\\ (?: UndefineShortVerb | lstDeleteShortInline) $llaves                   /ix;

while ($archivo =~
        /   $marca
        |   $comentario
        |   $definedel
        |   $indefinedel
        |   $del .+? $del                                                       # delimitado
        /pgmx) {
 
        my($pos_inicial, $pos_final) = ($-[0], $+[0]);                          # posiciones
        my $encontrado = ${^MATCH};                                             # lo encontrado
 
    if ($encontrado =~ /$definedel/){                                           # definimos delimitador
                        $del = $+{del};
                        $del = "\Q$+{del}" if substr($del,0,1) ne '\\';         # es necesario "escapar" el delimitador
                }
    elsif($encontrado =~ /$indefinedel/) {                                      # indefinimos delimitador
                 $del = $no_del;                                       
        }
    else {                                                                      # aquí se hacen los cambios
        while (my($busco, $cambio) = each %cambios) {
                       $encontrado =~ s/\Q$busco\E/$cambio/g;                   # es necesario escapar $busco
                        }
        substr $archivo, $pos_inicial, $pos_final-$pos_inicial, $encontrado;     # insertamos los nuevos cambios
 
        pos($archivo)= $pos_inicial + length $encontrado;                        # re posicionamos la siguiente búsqueda
        }
}

# Constantes
my $BP 	= '\\\\begin{postscript}';
my $EP 	= '\\\\end{postscript}';
my $BPL = '\begin{postscript}';
my $EPL = '\end{postscript}';
my $sipgf = 'pgfpicture';
my $nopgf = 'pgfinterruptpicture';
my $graphics = "graphic=\{\[scale=1\]$imageDir/$name-fig";

# directorio en el cual están las imágenes
my $dir = "$tempDir/$imageDir";   

#--------------------- Coment Verbatim environment --------------------#

my @lineas = split /\n/, $archivo;

# Verbatim environments 
my $ENTORNO  = qr/(?: (v|V)erbatim\*?| PSTexample | LTXexample| $ignore\*? | PSTcode | tcblisting\*? | spverbatim | minted | lstlisting | alltt | comment\*? | xcomment)/xi;

# postscript environment 
my $POSTSCRIPT = qr/(?: postscript)/xi;

# tikzpicture environment 
my $TIKZENV    = qr/(?: tikzpicture)/xi;
#    
my $DEL;

# tcbverb verbatim
my $tcbverb = qr/\\(?:tcboxverb|myverb)/;
my $arg_brac = qr/(?:\[.+?\])?/;
my $arg_curl = qr/\{(.+)\}/;          

# coment verbatim environment
for (@lineas) {
    if (/\\begin\{($ENTORNO)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
        while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    }
} 
# coment tcolorbox inline
for (@lineas) {
    if (m/$tcbverb$arg_brac$arg_curl/) {
        while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    } # close 
 }
# remove postscript from hash
delete @cambios{'\begin{postscript}','\end{postscript}'}; 
# coment in postscript environment
for (@lineas) {
    if (/\\begin\{($POSTSCRIPT)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
    	while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    } # close postcript environment
}
# remove tikzpicture from hash
delete @cambios{'\begin{tikzpicture}','\end{tikzpicture}'}; 
# coment in tikzpicture environment
for (@lineas) {
    if (/\\begin\{($TIKZENV)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
    	while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    } # close TIKZ environment
}

undef %cambios; # erase hash

#------------- Convert ALL into Postscript environments ---------------#

$archivo = join("\n", @lineas); 
## Partición del documento

my($cabeza,$cuerpo,$final) = $archivo =~ m/\A (.+?) (\\begin{document} .+?)(\\end{document}.*)\z/msx;

# \pspicture to \begin{pspicture}
$cuerpo =~ s/\\pspicture(\*)?(.+?)\\endpspicture/\\begin{pspicture$1}$2\\end{pspicture$1}/gmsx;

# pspicture to Postscript
$cuerpo =~ s/
    (
	(?:\\psset\{[^\}]+\}.*?)?
	(?:\\begin\{pspicture(\*)?\})
		.*?
	(?:\\end\{pspicture(\*)?\})
    )
    /$BPL\n$1\n$EPL/gmsx;

# pgfpicture to Postscript
$cuerpo =~ s/
    (
        \\begin{$sipgf}
            .*?
            (
                \\begin{$nopgf}
                .+?
                \\end{$nopgf}
                .*?
            )*?
        \\end{$sipgf}
    )
    /$BPL\n$1\n$EPL/gmsx;

# tikz to Postscript
$cuerpo =~ s/
     (
	(?:\\tikzset(\{(?:\{.*?\}|[^\{])*\}).*?)? # si está lo guardo
              (?:\\begin\{tikzpicture\})       # aquí comienza la búsqueda
                            .*?                # guardo el contenido en $1
                    (?:\\end\{tikzpicture\})   # termina la búsqueda
                ) # cierra $1
                /$BPL\n$1\n$EPL/gmsx;

# rest to PostScript
my $export  = qr/(forest|ganttchart|tikzcd|circuitikz|dependency|$other\*?)/x;

$cuerpo =~ s/(\\begin\{($export)\} (.*?)  \\end\{\g{-2}\})/$BPL\n$1\n$EPL/gmsx;

#-------------------------- Reverse changes ---------------------------#
$archivo = "$cabeza$cuerpo$final";
my %cambios = (
# pst/tikz set    
    '\PSSET'			=>	'\psset',
    '\TIKZSET' 			=> 	'\tikzset',		        
# pspicture    
    '\TRICKS'			=>	'\pspicture',
    '\ENDTRICKS'		=>	'\endpspicture',             
# pspicture
    '\begin{TRICKS'		=>	'\begin{pspicture',
    '\end{TRICKS'		=>	'\end{pspicture',
# $other    
    '\begin{OTHER'		=>	"\\begin\{$other",		
    '\end{OTHER'		=>	"\\end\{$other",
# document
    '\begin{DOCTRICKS}'		=>	'\begin{document}',
    '\end{DOCTRICKS}'		=>	'\end{document}',
# tikzpicture
    '\begin{TIKZPICTURE}'	=>	'\begin{tikzpicture}',
    '\end{TIKZPICTURE}'  	=>	'\end{tikzpicture}',
# pgfinterruptpicture
    '\begin{PGFINTERRUPTPICTURE'=>	'\begin{pgfinterruptpicture',
    '\end{PGFINTERRUPTPICTURE'  =>	'\end{pgfinterruptpicture',
# pgfpicture
    '\begin{PGFPICTURE}'	=>	'\begin{pgfpicture}',
    '\end{PGFPICTURE}'		=>	'\end{pgfpicture}',
# ganttchart
    '\begin{GANTTCHART}'	=>	'\begin{ganttchart}',
    '\end{GANTTCHART}'		=>	'\end{ganttchart}',
# circuitikz
    '\begin{CIRCUITIKZ}'	=>	'\begin{circuitikz}',
    '\end{CIRCUITIKZ}'		=>	'\end{circuitikz}',
# forest     
    '\begin{FOREST}'		=>	'\begin{forest}',
    '\end{FOREST}'		=>	'\end{forest}',
# tikzcd 
    '\begin{TIKZCD}'		=>	'\begin{tikzcd}',
    '\end{TIKZCD}'		=>	'\end{tikzcd}',
# dependency
    '\begin{DEPENDENCY}'	=>	'\begin{dependency}',
    '\end{DEPENDENCY}'		=>	'\end{dependency}',
);

#-------------------------- Back Postscript ---------------------------#

my @lineas = split /\n/, $archivo;
# reverse in postscript environment
for (@lineas) {
    if (/\\begin\{($POSTSCRIPT)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
    	while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    } # close postscript environment changes
}

# join changes 
$archivo = join("\n", @lineas);  

#--------------- Extract source code for PST/PGF/TIKZ -----------------#
# Dividir el archivo 
my($cabeza,$cuerpo,$final) = $archivo =~ m/\A (.+?) (\\begin{document} .+?)(\\end{document}.*)\z/msx;

# Poner el atributo añadido a PostScript
while ($cuerpo =~ /\\begin\{postscript\}/gsm) {
 
    my $corchetes = $1;
    my($pos_inicial, $pos_final) = ($-[1], $+[1]);      # posición donde están los corchetes
 
    if (not $corchetes) {
        $pos_inicial = $pos_final = $+[0];              # si no hay corchetes, nos ponemos al final de \begin
    }
    if (not $corchetes  or  $corchetes =~ /\[\s*\]/) {  # si no hay corchetes, o están vacíos,
        $corchetes = "[$graphics-$exacount}]";          # ponemos los nuestros
    }
    substr($cuerpo, $pos_inicial, $pos_final - $pos_inicial) = $corchetes;    
    pos($cuerpo) = $pos_inicial + length $corchetes;    # reposicionamos la búsqueda de la exp. reg.
}
continue {
    $exacount++;
}
# Delte source files 
if (-e "$imageDir/$name-fig-1$ext") {
unlink <"$imageDir/$name-fig-*$ext">;
}
#---------------------- Extract source code in images -----------------#
while ($cuerpo =~ /$BP\[.+?(?<img_src_name>$imageDir\/.+?-\d+)\}\](?<code>.+?)(?=$EP)/gsm) {
    open my $SALIDA, '>', "$+{'img_src_name'}$ext";
    print $SALIDA <<"EOC";
$cabeza\\pagestyle{empty}\n\\begin{document}$+{'code'}\\end{document}
EOC
close $SALIDA;
}

#--------------- Create a one file whit all figs file -----------------#
if ($nopreview) {
if (-e "$imageDir/$name-fig-1$ext") {
# 1- Leer los source files
my  @pstexafiles = glob("$imageDir/$name-fig-*$ext");

# 2- Ordenar según el índice y extención
@pstexafiles =
    map  { $_->[1]                       }
    sort { $a->[0] <=> $b->[0]           }
    map  { [ ($_ =~ /(\d+$ext)/), $_ ] }
    @pstexafiles;
 
# 3- Bucle para leer las secciones
my @almacen;
for my $exafile (@pstexafiles) {
 
    # 3.1- Leer el archivo
    open my $FH, '<', $exafile;                # 
    my $tex = join q{}, <$FH>;                 # 
    close   $FH;
 
    # 3.2- Extraer la parte que nos interesa
    my($pedazo) = $tex =~ m/\\begin\{document\}\s*(.+?)\s*\\end\{document\}/sm;
     
    # 3.3- Almacenamos, si hemos encontrado algo
    push @almacen, $pedazo if $pedazo;
}
# 4- Salida

open my $SALIDA, '>', "$tempDir/$name-fig$ext";
print $SALIDA "$cabeza\\pagestyle{empty}\n\\begin\{document\}\n";

my $fig = 1;
for my $item (@almacen) {
    print $SALIDA $item;
    print $SALIDA "\n%% fig $fig\n";
    print $SALIDA "\\newpage\n";
    $fig++;
}
 
print $SALIDA '\end{document}';
close $SALIDA;
    } # close join files
} # close $nopreview
else {
my $opcion = $xetex ? 'xetex,'
               : $latex ? ''
               :          'pdftex,'
               ;

my $preview = <<"EXTRA";
\\AtBeginDocument\{%
\\RequirePackage\[${opcion}active,tightpage\]\{preview\}%
\\renewcommand\\PreviewBbAdjust\{-60pt -60pt 60pt 60pt\}%
\\newenvironment\{postscript\}\{\}\{\}%
\\PreviewEnvironment\{postscript\}\}%
EXTRA
    # write
    open my $SALIDA, '>', "$tempDir/$name-fig$ext";
    print   $SALIDA $preview . $archivo;
    close   $SALIDA;
}

#---------------------- Create images files ---------------------------#
# Define compilers
my $compile = $xetex ? 'xelatex'
              : $luatex ? 'lualatex'
	      : $latex ?  'latex'
              :           'pdftex'
               ;
my $opt_compile = $miktex ? '--enable-write18 --interaction=batchmode'
              :             '--shell-escape --interaction=batchmode'
               ;

# Option for gs
my $opt_gspdf='-q -dSAFER -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress';
my $opt_gspng="-q -dSAFER -sDEVICE=pngalpha -r$DPI";
my $opt_gsjpg="-q -dSAFER -sDEVICE=jpeg -r$DPI -dJPEGQ=100 -dGraphicsAlphaBits=4 -dTextAlphaBits=4";

# Option for pdfcrop
my $opt_crop= $xetex ? "--xetex --margins $margins"
              : $luatex ? "--luatex --margins $margins"
	      : $latex ? "--margins $margins"
              :          "--pdftex --margins $margins"
               ;
# Fix pdftops error message in windows
if ($^O eq 'MSWin32' or $^O eq 'MSWin64'){
open my $ppmconf, '>', "$tempDir/xpd";
print $ppmconf <<'EOH';
errQuiet yes
EOH
close $ppmconf;
    }
#-------------------------- Compiling image file ----------------------#
if($nopreview){
    print "Create $name-fig.pdf in $imageDir dir whit all PGF/TIKZ/PST using $compile\n";
    }
else{
    print "Create $name-fig.pdf in $imageDir dir whit all PGF/TIKZ/PST using $compile and preview package\n";
    }
    
# Run TeX mode 
system("$compile $opt_compile -output-directory=$imageDir $tempDir/$name-fig$ext");
if($latex){
    system("dvips -q -Ppdf -o $imageDir/$name-fig.ps $imageDir/$name-fig.dvi");
    system("ps2pdf  -dPDFSETTINGS=/prepress $imageDir/$name-fig.ps  $imageDir/$name-fig.pdf");
    }
print "The file $imageDir/$name-fig.pdf need a cropping using pdfcrop whit options: $opt_crop\n";
system("pdfcrop $opt_crop $imageDir/$name-fig.pdf $imageDir/$name-fig.pdf");

##-------------------------- Create image formats ----------------------#

opendir(my $DIR, $imageDir);
my @figs = sort readdir $DIR;
closedir $DIR;
for my $fig (@figs) {
if ($fig =~ /(?<fig>$name-fig-)(?<num>\d+)$ext/) {
print "Create $imageDir/$name-fig-$+{num} from $name$ext\r";
# PDF format
if (!$nopdf) {
system("$::opt_gscmd $opt_gspdf -o $imageDir/$name-fig-%1d.pdf $imageDir/$name-fig.pdf");
}
# PNG format
if ($png) {
system("$::opt_gscmd $opt_gspng -o $imageDir/$name-fig-%1d.png $imageDir/$name-fig.pdf");
	}
# JPEG format
if ($jpg) {
system("$::opt_gscmd $opt_gsjpg -o $imageDir/$name-fig-%1d.jpg $imageDir/$name-fig.pdf");
	 }

# SVG format pdf2svg
if ($svg) {
    system("pdf2svg $imageDir/$name-fig.pdf $imageDir/$+{fig}%1d.svg all");
	 }
# EPS format
if ($eps) {
	if ($^O eq 'MSWin32' or $^O eq 'MSWin64'){
	system("pdftops -cfg $tempDir/xpd -q -eps -f $+{num} -l $+{num} $imageDir/$name-fig.pdf $imageDir/$+{fig}$+{num}.eps");
	}else{
	system("pdftops -q -eps -f $+{num} -l $+{num} $imageDir/$name-fig.pdf $imageDir/$+{fig}$+{num}.eps");
		}
	} # close EPS
# PPM format
if ($ppm) {
	if ($^O eq 'MSWin32' or $^O eq 'MSWin64'){
	system("pdftoppm  -cfg $tempDir/xpd  -q -r $DPI -f $+{num} -l $+{num} $imageDir/$name-fig.pdf $imageDir/$+{fig}$+{num}");
	}
	else{
	system("pdftoppm -q -r $DPI -f $+{num} -l $+{num} $imageDir/$name-fig.pdf $imageDir/$+{fig}$+{num}");
			}
		} # close PPM 
    } # close if ($fig =
} # close for 

## Renaming PPM
if ($ppm) {
if (opendir(DIR,$dir)) {                              # abro el directorio
    while (my $oldname = readdir DIR) {               # lo recorro
                                                      # el nuevo nombre es fruto de una sustitución
        my $newname = $oldname =~ s/^($name-(fig|exa)-\d+)(-\d+).ppm$/$1 . ".ppm"/re;
        
        if ($oldname ne $newname) {                   # comprobación
            rename("$dir/$oldname", "$dir/$newname"); # renombro
		}
	    }
    closedir DIR;
	} # close rename ppm
} # close ppm

#------- Creating output file whitout PGF/PST/TIKZ code ---------------#
if ($output) {
print "Create file $name-out$ext whitout PGF/PST/TIKZ code\n";

#----------------- Convert Postscript to includegraphics --------------#
my $grap="\\includegraphics[scale=1]{$name-fig-";
my $close = '}';
my $IMGno = 1;

$cuerpo =~ s/$BP.+?$EP/$grap@{[$IMGno++]}$close/msg;

#------------------------ Clean output file  --------------------------#
# append postscript to hash
$cambios{'\begin{POSTRICKS}'} = '\begin{postscript}';
$cambios{'\end{POSTRICKS}'} = '\end{postscript}';
 
# Constantes
my $BEGINDOC = quotemeta('\begin{document}');
my $USEPACK  = quotemeta('\usepackage');
my $REQPACK  = quotemeta('\usepackage');
my $GRAPHICX = quotemeta('{graphicx}');
 
# Exp. Reg.
my $CORCHETES = qr/\[ [^]]*? \]/x;
my $PALABRAS  = qr/\b (?: pst-\w+ | pstricks (?: -add )? | psfrag |psgo |vaucanson-g| auto-pst-pdf | graphicx )/x;
my $FAMILIA   = qr/\{ \s* $PALABRAS (?: \s* [,] \s* $PALABRAS )* \s* \}/x;
 
# comentar
$cabeza =~ s/ ^ ($USEPACK $CORCHETES $GRAPHICX) /%$1/msxg;
 
# eliminar líneas enteras
$cabeza =~ s/ ^ $USEPACK (?: $CORCHETES )? $FAMILIA \n//msxg;
 
# eliminar palabras sueltas
$cabeza =~ s/ (?: ^ $USEPACK \{ | \G) [^}]*? \K (,?) \s* $PALABRAS (\s*) (,?) /$1 and $3 ? ',' : $1 ? $2 : ''/gemsx;
     
# Añadir
$cabeza .= <<"EXTRA";
\\usepackage{graphicx}
\\graphicspath{{$imageDir/}}
\\usepackage{grfext}
\\PrependGraphicsExtensions*{.pdf}
EXTRA
 
# Clear PST content in preamble
$cabeza =~ s/\\usepackage\{\}/% delete/gmsx;
$cabeza =~ s/\\psset\{.+?\}/% \\psset delete/gmsx;
$cabeza =~ s/\\SpecialCoor/% \\SpecialCoor/gmsx;
 
# Recorremos el archivo y realizamos los cambios
while (my($busco, $cambio) = each %cambios) {
            $cabeza =~ s/\Q$busco\E/$cambio/g;
                        $cuerpo =~ s/\Q$busco\E/$cambio/g;
            }
 
# write
open my $SALIDA, '>', "$tempDir/$name-out$ext";
print   $SALIDA "$cabeza$cuerpo$final";
close   $SALIDA;

# compile output file

if ($xetex){
    system("xelatex $opt_compile $tempDir/$name-out$ext");
    } # xetex 
elsif($luatex){
    system("lualatex $opt_compile $tempDir/$name-out$ext");
    } # luatex 
else{
    system("pdflatex $opt_compile $tempDir/$name-out$ext");
    } # pdftex

}# close output file

#--------------------------------- Clean ------------------------------#
if ($clear) {
my @del_aux_tex;
find(\&del_aux_tex, $tempDir);
sub del_aux_tex{
my $auximgfile = $_;
# search .(aux|log) 
if(-f $auximgfile && $auximgfile =~ /$name-out\.(aux|log)$/){
push @del_aux_tex, $File::Find::name;
	}
if(-f $auximgfile && $auximgfile =~ /$name-fig$ext/){
push @del_aux_tex, $File::Find::name;
	}
}
unlink @del_aux_tex;
} # close clear in $tempDir

if ($clear) {
my @del_tmp;
find(\&del_tmp, $imageDir);
sub del_tmp{
my $auximgfile = $_;
# search .(aux|log) 
if(-f $auximgfile && $auximgfile =~ /$name-fig\.(aux|dvi|log|ps|pdf)/){
push @del_tmp, $File::Find::name;
	}
}

unlink @del_tmp;
} # close clear $name-fig.tex

# Clear source files
if ($noSource) {
unlink <"$imageDir/$name-fig-*$ext">;
}# end source clear

if ($output) {
print "Finish, LTXimg create $name-out$ext and put all figures in $imageDir dir\n";
}else{
print "Finish, LTXimg create all figures in $imageDir dir\n";
}

if ($^O eq 'MSWin32' or $^O eq 'MSWin64'){
unlink "$tempDir/xpd";
    }

__END__
