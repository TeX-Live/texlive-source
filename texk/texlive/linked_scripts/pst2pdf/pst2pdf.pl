eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;

use strict;			# to be sure, that all is safe ... :-)
use v5.18;
# $Id: pst2pdf.pl 119 2014-09-24 12:04:09Z herbert $
# v. 0.18	 2017-10-04 simplify the use of PSTricks with pdf
# (c) Herbert Voss <hvoss@tug.org> 
#     Pablo González Luengo <pablgonz@yahoo.com>
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
use re 'eval';
use File::Path;               # creating/removing dirs
use File::Copy;               # copying files
use File::Basename;	      # scan argument
use IO::File;                 # simple IO operation
use Getopt::Long qw(:config bundling_override); # read parameter and activate "bundling"
use autodie;		      # more safe
use Config;
use File::Spec;
use File::Find;

#----------------------- User part begin ------------------------
my $tempDir = ".";			# temporary directory
my $imageDir = "images";		# where to save the images
my $margins = "1";			# margins in pdfcrop
my $Verbose = 0;			# 0 or 1, logfile  
my $clear = 0;				# 0 or 1, clears all temporary files
my $DPI = 300;				# very low value for the png's
my $noImages = 0;			# 1->no create images
my $nopreview = 0;			# 1->create images in nopreview mode
my $runBibTeX = 0;			# 1->runs bibtex
my $runBiber = 0;			# 1->runs biber and sets $runBibTeX=0
my $jpg = 0;				# 1->create .jpg files
my $png = 0;				# 1->create .png files
my $ppm = 0;				# 1->create .ppm files
my $eps = 0;				# 1->create .eps files
my $svg = 0;			        # 1->create .svg files
my $all = 0;				# 1->create all images and files for type
my $xetex = 0;				# 1->Using (Xe)LaTeX for compilation.
my $exacount = 1;			# Counter for PSTexample
my $noSource = 0;			# Delete TeX source for images
my $other    = "other";     		# search other verbatim environment
#------------------------- User part end -------------------------------

#---------------- Program identification, options and help -------------
my $program = 'pst2pdf';
my $nv='v0.18';
my $ident = '$Id: pst2pdf.pl 119 2017-09-11 12:04:09Z herbert $';
my $copyright = <<END_COPYRIGHT ;
Copyright 2011-2017 (c) Herbert Voss <hvoss\@tug.org> and Pablo González.
END_COPYRIGHT
my $licensetxt= <<END_LICENSE ;
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
       
END_LICENSE
my $version= <<END_VERSION ;
${program} ${nv} ${ident}
${copyright}
END_VERSION
my $title = "$program $nv $ident\n";
my $usage = <<"END_OF_USAGE";
${title}Usage: $program <texfile.tex>  [Options]
pst2pdf run a TeX source, read all PS-related part and convert in images
	in pdf,eps,jpg or png format (default pdf) and create new file 
	whitout pst-environment and runs (pdf/Xe)latex. 
	See pst2pdf documentation for more info.
Options:
  -h,--help          - display this help and exit
  -l,--license       - display license information and exit
  -v,--version 	     - display version (current $nv) and exit
  -d,--dpi=<int>     - the dots per inch for images (default 300)
  -j,--jpg           - create .jpg files (need Ghostscript)
  -p,--png           - create .png files (need Ghostscript)
  -e,--eps	     - create .eps files (need pdftops)
  -s,--svg	     - create .svg files (need pdf2svg)
  -P,--ppm	     - create .ppm files (need pdftoppm)
  -a,--all	     - create .(pdf,eps,jpg,png,ppm,svg) images
  -c,--clear         - delete all temp and aux files
  -x,--xetex         - using (Xe)LaTeX for create images
  -m,--margins=<int> - margins for pdfcrop (in bp) (default 1)
  -ni,--noimages     - generate file-pdf.tex, but not images
  -np,--single       - create images files whitout preview and Ghostscript
  -ns,--nosource     - delete all source(.tex) for images files
  --imgdir=<string>  - the folder for the created images (default images)
  --ignore=<string>  - skip other verbatim environment (default other)
  --bibtex           - run bibtex on the aux file, if exists
  --biber            - run biber on the bcf file, if exists
  --Verbose          - creates a long log file (.plog)
  
Examples:
* $program test.tex -e -p -j -c --imgdir=pics
* produce test-pdf.tex whitout pstriks related parts and create image
* dir whit all images (pdf,eps,png,jpg) and source (.tex) for all pst 
* environment in "pics" dir using Ghostscript and cleaning all tmp files. 
* suport bundling for short options $program test.tex -epjc --imgdir=pics
END_OF_USAGE
#
my $result=GetOptions (
	'h|help'     	=> \$::opt_help,
	'v|version'  	=> \$::opt_version,
	'l|license'  	=> \$::opt_license,
	'd|dpi=i'    	=> \$DPI,# numeric
	'm|margins=i'   => \$margins,# numeric
	'imgdir=s' 	=> \$imageDir, # string
	'ignore=s' 	=> \$other, # string
	'c|clear'    	=> \$clear,    # flag
	'ni|noimages'   => \$noImages, # flag
	'np|single'  	=> \$nopreview, # flag
	'bibtex'  	=> \$runBibTeX,# flag
	'e|eps'      	=> \$eps, # flag
	'j|jpg'      	=> \$jpg, # flag
	'p|png'      	=> \$png, # flag
	'P|ppm'      	=> \$ppm, # flag
	's|svg'      	=> \$svg, # flag
	'a|all'      	=> \$all, # flag
	'x|xetex'    	=> \$xetex, # flag
	'biber'   	=> \$runBiber, # flag
	'ns|nosource'	=> \$noSource, # flag
	'Verbose'  	=> \$Verbose,
) or die $usage;
#------------------------ Help functions --------------------------
sub errorUsage {
    die "Error: @_ (try --help for more information)\n"; 
    }

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
if ($::opt_version) {
print $version;
  exit (0);
}

# open file 
my $InputFilename = "";
  @ARGV > 0 or errorUsage "Input filename missing";
  @ARGV < 2 or errorUsage "Unknown option or too many input files";
  $InputFilename = $ARGV[0];

#------------------------ Arreglo de la extensión ----------------------
my @SuffixList = ('.tex', '', '.ltx');                  # posible extensión
my ($name, $path, $ext) = fileparse($ARGV[0], @SuffixList);
$ext = '.tex' if not $ext; 

#--------------------- Directorio destino de imágenes ------------------
-e $imageDir or mkdir($imageDir,0744) or die "No puedo crear $imageDir: $!\n";

# expresion regular para swpl
my $graphics = "graphic=\{\[scale=1\]$imageDir/$name-exa";

# directorio en el cual están las imágenes
my $dir = "$tempDir/$imageDir";             

# archivo de entrada para el resto del script
my $TeXfile = "$tempDir/$name-tmp$ext";


#----------------------------- Search GS ------------------------------
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
                    print "  EXE found: $value\n" if $Verbose;
                }
                else {
                    print "  EXE not found!\n" if $Verbose;
                    next;
                }
                my $sortkey = sprintf '%02d.%03d %s',
                        $version_main, $version_sub, $key_name_gs;
                $list{$sortkey} = $value;
            }
            else {
                print "  Missing key `GS_DLL' with type `REG_SZ'!\n" if $Verbose;
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

open  my $LOGfile,'>', "$tempDir/$name.plog"; # our own log file
	LOG ("Parameters:");
	LOG ("==> imageDir  = $imageDir"); 
	LOG ("==> dpi       = $DPI"); 
	LOG ("==> tempDir   = $tempDir"); 
	LOG ("==> Verbose   = $Verbose"); 
	LOG ("==> clear     = $clear"); 
	LOG ("==> noImages  = $noImages");
	LOG ("==> noSource  = $noSource");  
	LOG ("==> nopreview = $nopreview"); 
	LOG ("==> runBibTeX = $runBibTeX"); 
	LOG ("==> runBiber  = $runBiber"); 
	LOG ("==> ppm       = $ppm"); 
	LOG ("==> eps       = $eps");  
	LOG ("==> jpg       = $jpg"); 
	LOG ("==> png       = $png");
	LOG ("==> svg       = $svg");  
	LOG ("==> xetex     = $xetex");

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
	$jpg = 1;
}
if ($png) {
  LOG ("Generate .png files ...");
	$png = 1;
}
if ($svg) {
  LOG ("Generate .svg files ...");
	$svg = 1;
}
if ($all) {
  LOG ("Generate images eps/pdf/files and clear...");
   $eps =$ppm=$jpg=$png=$svg=$clear = 1;
}
if ($nopreview) {
  LOG ("nopreview mode generate images files ...");
  $nopreview= 1;
}
if ($xetex) {
  LOG ("Compiling using XeLaTeX ...");
  $xetex=1;
}
if ($noImages ) {
  LOG ("no create images");
 	$nopreview= 0;
 }
if ($noSource) {
  LOG ("Delete all source files");
 	$noSource= 1;
 }

#----------------------------- Parte 1 --------------------------------- 
# Comentamos los bloques PST dentro de verbatim de todo el archivo 
# Abrimos el archivo 
my $InputFilename = shift;
my @lineas;
{
   open my $FILE,'<:crlf',"$InputFilename";
   @lineas = <$FILE>;
   close $FILE;
}

# Creamos un hash con los cambios
my %cambios = (
    '\pspicture'                => '\TRICKS',
    '\endpspicture'             => '\ENDTRICKS',

    '\begin{PSTexample'          => '\begin{PSTEXAMPLE',
    '\end{PSTexample'            => '\end{PSTEXAMPLE',

    '\begin{pspicture'          => '\begin{TRICKS',
    '\end{pspicture'            => '\end{TRICKS',

    '\begin{postscript}'        => '\begin{POSTRICKS}',
    '\end{postscript}'          => '\end{POSTRICKS}',
    
    '\begin{document}'          => '\begin{DOCTRICKS}',
    '\end{document}'            => '\end{DOCTRICKS}'
);

# Definimos los entonos verbatim donde comentaremos
my $ENTORNO  = qr/(?: (v|V)erbatim\*?| LTXexample | PSTcode |$other\*? | tcblisting\*? | spverbatim | minted | lstlisting | alltt | comment\*? | xcomment)/xi;
#    
my $DEL;

my $tcbverb = qr/\\(?:tcboxverb|myverb)/;
 
my $arg_brac = qr/(?:\[.+?\])?/;
 
my $arg_curl = qr/\{(.+)\}/;          # ATENCIÓN

for (@lineas) {
    if (/^\\begin\{($ENTORNO)(?{ $DEL = "\Q$^N" })\}/ .. /^\\end\{$DEL\}/) {
        while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    }
    elsif (m/$tcbverb$arg_brac$arg_curl/) {
        while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    } #close elsif
 }

# Escritura del resultado
open my $SALIDA, '>', "$tempDir/$name-tmp$ext";
print   $SALIDA @lineas;
close   $SALIDA;

#--------------------------------Parte 2--------------------------------
# Comentamos el texto en verbatim a lo largo del cuerpo 
open my $ENTRADA, '<', "$tempDir/$name-tmp$ext";
my $archivo;
{
    local $/;
    $archivo = <$ENTRADA>;
}
close $ENTRADA;
 
## Partición del documento
my($cabeza,$cuerpo,$final) = $archivo =~ m/\A (.+? ^\\begin\{document\}) (.+) (^ \\end\{document\} .*) \z/msx;

## Variables y constantes
my $no_del = "\0";
my $del    = $no_del;

## Reglas
my $llaves      = qr/\{ .+? \}                                                                  /x;
my $no_corchete = qr/(?:\[ .+? \])?                                                             /x;
my $delimitador = qr/\{ (?<del>.+?) \}                                                          /x;
my $spverb      = qr/spverb [*]?                                                                /ix;
my $verb        = qr/(v|V)erb [*]?                                                              /ix;
my $lst         = qr/lstinline (?!\*) $no_corchete                                              /ix;
my $mint        = qr/mint      (?!\*) $no_corchete $llaves                                      /ix;
my $marca       = qr/\\ (?:$verb | $lst | $mint ) (\S) .+? \g{-1}              /x;
my $comentario  = qr/^ \s* \%+ .+? $                                                            /mx;
my $definedel   = qr/\\ (?:   DefineShortVerb | lstMakeShortInline  ) $no_corchete $delimitador /ix;
my $indefinedel = qr/\\ (?: UndefineShortVerb | lstDeleteShortInline) $llaves                   /ix;
 
## Cambiar
while ($cuerpo =~
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
        substr $cuerpo, $pos_inicial, $pos_final-$pos_inicial, $encontrado;     # insertamos los nuevos cambios
 
        pos($cuerpo)= $pos_inicial + length $encontrado;                        # re posicionamos la siguiente búsqueda
        }
}


# Poner el atributo añadido a \begin{PSTexample}
while ($cuerpo =~ /^\\begin\{PSTexample\}(\[.+?\])?/gsm) {
 
    my $corchetes = $1;
    my($pos_inicial, $pos_final) = ($-[1], $+[1]);      # posición donde están los corchetes
 
    if (not $corchetes) {
        $pos_inicial = $pos_final = $+[0];              # si no hay corchetes, nos ponemos al final de \begin
    }
 
    if (not $corchetes  or  $corchetes =~ /\[\s*\]/) {  # si no hay corchetes, o están vacíos,
        $corchetes = "[$graphics-$exacount}]";          # ponemos los nuestros
    }
    else {                                              # si sí hay corchetes,
        $corchetes =~ s/\]/,$graphics-$exacount}]/;     # lo agregamos al final, dentro de los corchetes
    }
 
    substr($cuerpo, $pos_inicial, $pos_final - $pos_inicial) = $corchetes;    
    pos($cuerpo) = $pos_inicial + length $corchetes;    # reposicionamos la búsqueda de la exp. reg.
}
continue {
    $exacount++;
}

#----------------------- Extract PSTexample files ----------------------

while ($cuerpo =~ /^\\begin\{PSTexample\}\[.+?(?<nombre_archivo_secundario>$imageDir\/.+?-\d+)\}\](?<contenido>.+?)(?=^\\end\{PSTexample})/gsm) {
    open my $SALIDA, '>', "$+{'nombre_archivo_secundario'}$ext";
    print $SALIDA <<"EOC";
$cabeza\n\\thispagestyle{empty}$+{'contenido'}\\end{document}
EOC
    close $SALIDA;
}
# Now, deault way join all PSTexample files 
if(!$nopreview){
if (-e "$imageDir/$name-exa-1$ext") {
#    print "PSTexample environment found\n";
# 1- Leer los PSTexa files
my  @pstexafiles = glob("$imageDir/$name-exa-*$ext");
 
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
    my($pedazo) = $tex =~ m/\\thispagestyle\{empty\}\s*(.+?)\s*\\end\{document\}/sm;
     
    # 3.3- Almacenamos, si hemos encontrado algo
    push @almacen, $pedazo if $pedazo;
}
 
# 4- Salida
open my $SALIDA, '>', "$tempDir/$name-exa$ext";

print $SALIDA "$cabeza\n";
 
my $fig = 1;
for my $item (@almacen) {
    print $SALIDA "\\thispagestyle\{empty\}\n";
    print $SALIDA $item;
    print $SALIDA "%fig . $fig\n";
    print $SALIDA "\\newpage\n";
    $fig++;
}
 
print $SALIDA '\end{document}';
close $SALIDA;
    } # close join exa files (if exist)
} # close nopreview

### Escribimos en el mismo archivo con los cambios en verbatim
open my $SALIDA, '>', "$tempDir/$name-tmp$ext";
print   $SALIDA "$cabeza$cuerpo$final";
close   $SALIDA;

## Comentamos pst-exa en el preambulo, para no entorpecer a preview
$cabeza =~
s/(?<exapack>\\usepackage\[(swpl|tcb)\]\{pst-exa\})/%$+{exapack}/gsmx;
## Eliminamos PSTexample y PSTcode para no entorpecer a preview
$cuerpo =~
s/(?<initexa>\\begin\{PSTexample\})(?<code>.+?)(?<endexa>\\end\{PSTexample\})//gsmx;
$cuerpo =~
s/(?<initexa>\\begin\{PSTcode\})(?<code>.+?)(?<endexa>\\end\{PSTcode\})//gsmx;

## Escribimos el archivo completo para las figuras
if(!$nopreview){
open my $OUTFILE, '>', "$tempDir/$name-pst$ext";
print $OUTFILE "\\AtBeginDocument\{\n";
if($xetex){
print $OUTFILE "\\RequirePackage\[xetex,active,tightpage\]\{preview\}\n";
}
else{
print $OUTFILE "\\RequirePackage\[active,tightpage\]\{preview\}\n";
}
print $OUTFILE "\\renewcommand\\PreviewBbAdjust\{-60pt -60pt 60pt 60pt\}%\n";
print $OUTFILE "\\PreviewEnvironment\{pspicture\}\n";
print $OUTFILE "\\PreviewEnvironment\{postscript\}\}\n";        
print $OUTFILE "$cabeza$cuerpo\\end\{document\}";	
close $OUTFILE;
}
## Escribimos en el mismo archivo, convirtiendo PSTexample 
## esta sera el archivo de entrada en el resto del script

my @lineas;
{
    open my $FILE, "$tempDir/$name-tmp$ext";
    @lineas = <$FILE>;
    close $FILE;
}

my $PSTexample  = qr/(?: PSTexample | PSTcode )/xi;
#    
my $DEL;

for (@lineas) {
    if (/^\\begin\{($PSTexample)(?{ $DEL = "\Q$^N" })\}/ .. /^\\end\{$DEL\}/) {
        while (my($busco, $cambio) = each %cambios) {
            s/\Q$busco\E/$cambio/g;
        }
    }
 }

# Escritura del resultado
open my $SALIDA, '>', "$tempDir/$name-tmp$ext";
print   $SALIDA @lineas;
close   $SALIDA;
# end code for pstexample

my $imgNo = 1;				# internal image counter
#----------------- nopreview mode, dont'n need gs ----------------------
if ($nopreview) {
LOG ("Running on [$path][$name][$ext]"); 
open my $FILE,'<', "$TeXfile" ;	# the source
if (!$noImages ) {
LOG ("nopreview mode generate images...");
savePreamble($name);
runFile($name);
runPSTimg($name);
close $FILE;				# close source file
close $LOGfile;
	}
}
#----------------------- Default way, using gs -------------------------
else{
LOG ("Running on [$path][$name][$ext]"); 
open my $FILE,'<', "$TeXfile" ;	# the source
if (!$noImages ) {
LOG ("Using gs for split images ... "); 
savePreamble($name);
runFile($name);
runPSTimg($name);
LOG ("done!\n go to runFile ..."); 
LOG ("done!"); 
close $FILE;			# close source file
close $LOGfile;
	}# !noImages
}# close



#-------------------------- Save Preamble ------------------------------
# Now create a preamble file if we have a \input command inside the 
# preamble, it doesn't hurt, we need it anyway for the postscript 
# files and the pdf one.
sub savePreamble {				
my $filename = pop;					# get the file name
	LOG ("----- Start Preamble -----"); 
open my $FILEp,'>',"$tempDir/$filename.preamble";
open my $FILE,  '<', "$tempDir/$name-tmp$ext";
	while (<$FILE>) {					# read all until \begin{document}
    	my $i = index($_,"begin{document}");
	if ($i > 0) { 
      	if ($i > 1) { print $FILEp substr($_,0,--$i); }	# write all until \begin{document}
	if ($nopreview) {
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
}
# Search in source-parser file
sub searchPS {				# search the PostScript parts
  my @PS = ();				# nopreview PS sequence
  my @PStotal = ();		        # all PS sequences as list of arrays
  my $depth = -1;			# counts nested macros
  my $type = -1;			# -1-> none; 1->PST; 2->PS; 
  my $EndDocument = 0;                  # ignore all after \end{document}
  my $iVerb = 0;			# test for verbatim or lstlisting environment, must be ignored
  open my $FILE, '<', "$tempDir/$name-tmp$ext";
  while (<$FILE>) {		        # scan the input file
		if (!$EndDocument) {
    chomp;				# delete EOL character
	my $line = $_; 			# save line
    if ( !$iVerb ) { 
      $iVerb = ((index($line,"begin{verbatim}") > 0) or ( index($line,"begin{lstlisting}") > 0)); 
    }     # do nothing until \end{verbatim}
    if ( !$iVerb ) {
	my $iPS  = index($line,"begin{postscript}");
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
	      if (index($line,"endpspicture") > 0) 	   # add line, depends to type
	           { push @PS,substr($line,0,$iPST+12); }  # \endpspicture
	      elsif (index($line,"pspicture*") > 0)
 	              { push @PS,substr($line,0,$iPST+15); }# \end{pspicture} 
	      else { push @PS,substr($line,0,$iPST+14); }   # \end{pspicture} 
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
    } # if ( $iVerb )
    if (( index($line,"end{verbatim}") > 0 ) or ( index($line,"end{lstlisting}") > 0 )) { $iVerb = 0; }
  }}
  if ( $Verbose ) { 
    LOG("---->PStotal---->");
    for my $aref ( @PStotal ) { 
      my @a = @$aref;
      my $i = 1;
			foreach ( @a ) { LOG ($a[$i]); $i=$i+1; }
    }
    LOG ("<----PStotal<----"); 
  }
  close $FILE;
 return @PStotal; # return all PS sequences
}

#---------------------- Create files.tex for images --------------------
sub runFile {

  my $filename = pop;
  my @PSarray = searchPS();

  if ( $Verbose ) { 
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
	open my $FILEsub,'>',"$tempDir/$filename-fig$ext";
    while (<$FILEp>) {print $FILEsub $_; }
	print $FILEsub "\\begin{document}\n";
		if ( $Verbose ) { LOG("\@PS: $_"); }
    foreach ( @PS ) { print $FILEsub "$_\n"; }
	print $FILEsub "\\end{document}";
	close $FILEsub;
	close $FILEp;
	runTeX("$tempDir/$filename-fig");
		}
}
LOG ("runpdfTeX ... "); 
runpdfTeX("$path$name",$name);
LOG ("all finished ... :-)"); 

#------------------- Copy files.tex for images in default mode ---------
sub runTeX{
	my $filename = pop;
	copy("$filename$ext", "$imageDir/$filename-$imgNo$ext");
	$imgNo=$imgNo+1;
}
#------------------- Create images in PDF fromat ------------------------
sub runPSTimg() {
# Option for gs
my $opt_gs_split='-q -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -dNOPAUSE -dBATCH ';

# Option for pdfcrop
my $opt_pdfcrop="-margins $margins";

# Abrimos el directorio y creamos las imagenes en formato PDF
if ($nopreview) {
opendir(my $DIR, $imageDir);
 while (readdir $DIR) {
if (/(?<nombre>$name-(fig|exa)-\d+)(?<extension>$ext)/) {
if ($xetex){
    system("xelatex -interaction=batchmode -output-directory=$imageDir $imageDir/$+{nombre}$+{extension}");
    system("pdfcrop $opt_pdfcrop -xetex $imageDir/$+{nombre}.pdf $imageDir/$+{nombre}.pdf");
	} # close xelatex 
else{
    system("latex -interaction=batchmode -output-directory=$imageDir $imageDir/$+{nombre}$+{extension}");
    system("dvips -q -P pdf -o $imageDir/$+{nombre}.ps 	 $imageDir/$+{nombre}.dvi");
    system("ps2pdf   $imageDir/$+{nombre}.ps    $imageDir/$+{nombre}.pdf");
    system("pdfcrop $opt_pdfcrop $imageDir/$+{nombre}.pdf $imageDir/$+{nombre}.pdf");
	    } # close latex>dvips>ps2pdf
        } # close find regex in nopreview
} # close while
closedir $DIR;
}  # close $nopreview
# Cerramos y abrimos con nopreview (gs por defecto)
else{
# Compilamos PSTexa usando gs
if (-e "$tempDir/$name-exa$ext") {
if ($xetex){
    system("xelatex -interaction=batchmode -output-directory=$imageDir $tempDir/$name-exa$ext");
    system("pdfcrop $opt_pdfcrop -xetex $imageDir/$name-exa.pdf $imageDir/$name-exa.pdf");
	} # xelatex 
else{
    system("latex -interaction=batchmode -output-directory=$imageDir $tempDir/$name-exa$ext");
    system("dvips -q -Ppdf -o $imageDir/$name-exa.ps $imageDir/$name-exa.dvi");
    system("ps2pdf  -dPDFSETTINGS=/prepress $imageDir/$name-exa.ps    $imageDir/$name-exa.pdf");
    system("pdfcrop $opt_pdfcrop $imageDir/$name-exa.pdf $imageDir/$name-exa.pdf");
    } # close latex>dvips>ps2pdf	
system("$::opt_gscmd $opt_gs_split -sOutputFile=$imageDir/$name-exa-%1d.pdf $imageDir/$name-exa.pdf");

# Eliminamos los fuentes
if ($clear) {
    unlink "$imageDir/$name-exa.pdf";
    unlink "$tempDir/$name-exa$ext";
    }
} # close check file exist

# Compilamos las imagenes y las separamos usando gs
if (-e "$imageDir/$name-fig-1$ext") {
if ($xetex){
    system("xelatex -interaction=batchmode -output-directory=$imageDir $tempDir/$name-pst$ext");
    system("pdfcrop $opt_pdfcrop -xetex $imageDir/$name-pst.pdf $imageDir/$name-pst.pdf");
	} # xelatex 
else{
    system("latex -interaction=batchmode -output-directory=$imageDir $tempDir/$name-pst$ext");
    system("dvips -q -Ppdf -o $imageDir/$name-pst.ps $imageDir/$name-pst.dvi");
    system("ps2pdf  -dPDFSETTINGS=/prepress $imageDir/$name-pst.ps    $imageDir/$name-pst.pdf");
    system("pdfcrop $opt_pdfcrop $imageDir/$name-pst.pdf $imageDir/$name-pst.pdf");
    } # close latex>dvips>ps2pdf
system("$::opt_gscmd  $opt_gs_split -sOutputFile=$imageDir/$name-fig-%1d.pdf $imageDir/$name-pst.pdf");	
if ($clear) {
unlink "$imageDir/$name-pst.pdf";
unlink "$tempDir/$name-pst$ext";
unlink "$tempDir/$name-fig$ext";
	} # end clear
    } # close check file exist
} # close !nopreview (ghostscript)
#--------------------------- Clear aux files ---------------------------
if ($clear) {	
my @del_tmp;
find(\&del_aux_tex, $imageDir);
sub del_aux_tex{
my $auximgfile = $_;

# search .(aux|dvi|log|ps) 
if(-f $auximgfile && $auximgfile =~ /\.(aux|dvi|log|ps)$/){
push @del_tmp, $File::Find::name;
	}
}
unlink @del_tmp;
}# end clear aux

# Create xpdfrc conf for silent output pdftops/pdftoppm mesagge 	
if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){

if ($eps or $ppm) {
open my $ppmconf, '>', "$tempDir/xpd";
print $ppmconf <<'EOH';
errQuiet yes
EOH
close $ppmconf;
	}
}# end fix  for windows

# options for other images type using ghostscript
my $opt_gs_raw='-dGraphicsAlphaBits=4 -dTextAlphaBits=4 -dNOPAUSE -dBATCH ';

# Creamos las imagenes en todos los formatos distintos a PDF
opendir(my $DIR, $imageDir);
while (readdir $DIR) {
if (/(?<nombre>$name-(fig|exa)-\d+)(?<extension>\.pdf)/) {

# PNG format
if ($png) {
system("$::opt_gscmd -q -sDEVICE=pngalpha -r$DPI $opt_gs_raw -sOutputFile=$imageDir/$+{nombre}.png $imageDir/$+{nombre}.pdf");
	}

# JPEG format
if ($jpg) {
system("$::opt_gscmd -q -sDEVICE=jpeg -r$DPI -dJPEGQ=100 $opt_gs_raw -sOutputFile=$imageDir/$+{nombre}.jpg $imageDir/$+{nombre}.pdf");
	 }

# SVG format
if ($svg) {
    system("pdf2svg $imageDir/$+{nombre}.pdf $imageDir/$+{nombre}.svg");
	 }

# EPS format
if ($eps) {
	if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){
	system("pdftops -cfg $tempDir/xpd -q -level3 -eps $imageDir/$+{nombre}.pdf $imageDir/$+{nombre}.eps");
	}else{
	system("pdftops -q -level3 -eps $imageDir/$+{nombre}.pdf $imageDir/$+{nombre}.eps");
		}
	} # close EPS

# PPM format
if ($ppm) {
	if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){
	system("pdftoppm  -cfg $tempDir/xpd  -q -r $DPI $imageDir/$+{nombre}.pdf $imageDir/$+{nombre}");
	}else{
	system("pdftoppm -q  -r $DPI  $imageDir/$+{nombre}.pdf $imageDir/$+{nombre}");
		}
	} # close PPM  
	} # close finde regex
} # close while
closedir $DIR; #close dir

## Renombramos los ppm
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
} # cerramos runPSTimg

#--------------------- Replace PST environment for images----------------
sub runpdfTeX() {

  my ($name,$pdfname) = @_;
  open my $PDF,'>',"$tempDir/$pdfname-pdf$ext";
  open my $FILE,'<',"$tempDir/$name-tmp$ext";
  my $ignore = 0;
  my $IMGno = 1;
  my $depth = -1;
  my $type = -1;
  my $EndDocument = 0;				# ignore all after \end{document}
  my $iVerb = 0;
  	while (<$FILE>) {			# scan the input file
    	if ( !$iVerb ) { 
      $iVerb = ((index($_,"begin{verbatim}") > 0) or ( index($_,"begin{lstlisting}") > 0)); 
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
			print $PDF "\\includegraphics[scale=1]{$pdfname-fig-$IMGno}"; # use pdfname
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
			print $PDF "\\includegraphics[scale=1]{$pdfname-fig-$IMGno}"; 	# use \graphicspath
			$IMGno=$IMGno+1;
			LOG("Increase Image counter: $IMGno");
            }
          }
        }
			if ( !$ignore ) { print $PDF "$_"; }		# default line
			if ( $type == 2 ) {				# postscript env
	my $iPS = index($_,"end{postscript}");
			if ($iPS > 0) {
					print $PDF substr($_,$iPS+15);	# rest of line
			$ignore = 0; 
			$type=-1;
          }						# end Scan
        } 
			elsif ( $type == 1 ) {		# pspicture env
	my $iPST = index($_,"end{pspicture*}");
			if ($iPST < 0) { $iPST = index($_,"end{pspicture}"); }	# alternative ...
			if ($iPST < 0) { $iPST = index($_,"endpspicture"); }	# alternative \endpspicture...
			if ($iPST > 0) {		 			# end Scan
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
    } else { print $PDF $_; } # if ( $iVerb )
	if (( index($_,"end{verbatim}") > 0 ) or (  index($_,"end{lstlisting}") > 0 )) { $iVerb = 0; }
  } # while (<$FILE>)
  close $FILE;
  close $PDF;
## Close generate file
if ($clear) {
unlink "$tempDir/$pdfname-tmp$ext";
#unlink "$tempDir/$pdfname-fig$ext";
}
#------------------- Clean pstpack, reverse changes --------------------
## Leer archivo 
open my $INFILE, '<', "$tempDir/$pdfname-pdf$ext";
my $IPDF;
{
    local $/;
    $IPDF = <$INFILE>;
}
close  $INFILE;

## Constantes 
my $BEGINDOC = quotemeta('\begin{document}');
my $USEPACK  = quotemeta('\usepackage');
my $GRAPHICX = quotemeta('{graphicx}');

## Exp. Reg.

my $CORCHETES = qr/\[ [^]]*? \]/x;
my $PALABRAS  = qr/\b (?: pst-\w+ | pstricks (?: -add )? | psfrag |psgo |vaucanson-g| auto-pst-pdf | graphicx )/x;
my $FAMILIA   = qr/\{ \s* $PALABRAS (?: \s* [,] \s* $PALABRAS )* \s* \}/x;

## Dividir el archivo 
my($cabeza, $cuerpo) = $IPDF =~ m/\A (.+?) ($BEGINDOC .+?) \z/msx;

## Revisamos si esta cargado pst-exa
my $pstTcbload = 0;		# Buscamos si esta cargado \usepackage[...]{pst-exa}
$pstTcbload = index($cabeza,"usepackage[tcb]{pst-exa}"); 

## Filtrado 
# comentar
$cabeza =~ s/ ^ ($USEPACK $CORCHETES $GRAPHICX) /%$1/msxg;
 
# eliminar líneas enteras
$cabeza =~ s/ ^ $USEPACK (?: $CORCHETES )? $FAMILIA \n//msxg;
 
# eliminar palabras sueltas
$cabeza =~ s/ (?: ^ $USEPACK \{ | \G) [^}]*? \K (,?) \s* $PALABRAS (\s*) (,?) /$1 and $3 ? ',' : $1 ? $2 : ''/gemsx;
     
## Añadir 

$cabeza .= <<"EXTRA";
\\usepackage{graphicx}
\\graphicspath{{$imageDir/}}
\\usepackage{grfext}
\\PrependGraphicsExtensions*{.pdf}
EXTRA

## Verificamos que exista el archivo
if (-e "$imageDir/$name-exa-1$ext") {
if ($pstTcbload > 1) {
# Cambiamos de swpl a tcb con una expresion regular
$cuerpo =~ s/(graphic=\{)\[(scale=\d*)\]($imageDir\/$pdfname-exa-\d*)\}/$1$2\}\{$3\}/gsmx;

# Se añade la opción [tcb] al paquete pst-exa
$cabeza .= <<"EXTRA";
\\usepackage[pdf,tcb]{pst-exa}
EXTRA
} # close if 
else{
# print "Se añade la opción swpl al paquete pst-exa\n";
$cabeza .= <<"EXTRA";
\\usepackage[swpl]{pst-exa}
EXTRA
    }
} # close regex for pst-exa pack

### Clear PST content in preamble
$cabeza =~ s/\\usepackage\{\}/%delete by pst2pdf/gmsx;
$cabeza =~ s/^\\psset\{.+?\}/%\\psset delete by pst2pdf/gmsx;
$cabeza =~ s/\\SpecialCoor/%\\SpecialCoor delete by pst2pdf/gmsx;
$cabeza =~ s/^%CleanPST .+? %CleanPST/% Clean PST by pst2pdf/gmsx;

### Delete extra psset{..} in body 
$cuerpo =~ s/\\psset\{[^\}]+\}(\s*\\includegraphics\[scale=1\]\{$pdfname-fig-\d*\})/$1/gms; 

### Back 
my %cambios = (
    '\TRICKS'		=>  '\pspicture',
    '\ENDTRICKS'	=>  '\endpspicture',
    '\begin{PSTEXAMPLE'	=>  '\begin{PSTexample',
    '\end{PSTEXAMPLE'  	=>  '\end{PSTexample',
    '\begin{TRICKS'	=>  '\begin{pspicture',
    '\end{TRICKS'  	=>  '\end{pspicture',
    '\begin{POSTRICKS}'	=>  '\begin{postscript}' ,
    '\end{POSTRICKS}'	=>  '\end{postscript}'   ,
    '\begin{DOCTRICKS}'	=>  '\begin{document}'   ,
    '\end{DOCTRICKS}'	=>  '\end{document}'     
);

## Recorremos el archivo y realizamos los cambios
while (my($busco, $cambio) = each %cambios) {
            $cabeza =~ s/\Q$busco\E/$cambio/g;
            $cuerpo =~ s/\Q$busco\E/$cambio/g;
        }

## Escribimos en el mismo archivo
open my $SALIDA, '>', "$tempDir/$pdfname-pdf$ext";
print   $SALIDA "$cabeza$cuerpo";
close   $SALIDA;
#

my $runAgain = 0;
		
if($xetex){ # xelatex mode
if ($noImages){
	print "The file $pdfname-pdf$ext are created (Xe)LaTeX\n";
	}
else{	
    system("xelatex -interaction=batchmode $tempDir/$pdfname-pdf"); 
    print "Done, compiled $pdfname-pdf$ext using (Xe)LaTeX\n";
    }
    }
else{ #pdflatex mode
if ($noImages){
    print "The file $pdfname-pdf$ext are created (pdf)LaTeX\n";
    }
else{			
    system("pdflatex -interaction=batchmode $tempDir/$pdfname-pdf$ext"); 
    print "Done, compiled $pdfname-pdf$ext using (pdf)LaTeX\n";
    }
}

if (-e "$tempDir/$pdfname-pdf.idx") {
	system("makeindex $tempDir/$pdfname-pdf.idx"); 
	$runAgain++;
	}
if ($runBibTeX && -e "$tempDir/$pdfname-pdf.aux") { 
	system("bibtex $tempDir/$pdfname-pdf");  
	$runAgain++;
	}
if ($runBiber && -e "$tempDir/$pdfname-pdf.bcf") {
	system("biber $tempDir/$pdfname-pdf");  
	$runAgain++; 
	}
if ($runAgain){
	if($xetex){
	system("xelatex -interaction=batchmode $tempDir/$pdfname-pdf");
	}
	else{
	system("pdflatex -interaction=batchmode $tempDir/$pdfname-pdf");
	}
    }
} #close sub run pdfTEX

#----------------------------- Write LOG file --------------------------
sub LOG() { 
	if ( $Verbose ) { print $LOGfile "@_\n"; } 
	}
#-------------------------- Clear all tmp files ------------------------	
if ($clear) {

if (-e "$tempDir/$name-pst$ext") {
unlink "$tempDir/$name-pst$ext";
}
    my @del_pdf_tmp;
    find(\&del_pdf_aux, $tempDir);
    sub del_pdf_aux{
    my $auximgfile = $_;
	if(-f $auximgfile && $auximgfile =~ /\.(aux|dvi|log|ps|idx|bfc|bib|preamble)$/){
    push @del_pdf_tmp, $File::Find::name;
	    }
    } # close sub
unlink @del_pdf_tmp; # delte files

if ($^O eq 'MSWin32' || $^O eq 'MSWin64'){
if ($eps or $ppm){
	unlink "$tempDir/xpd";
	}
}
if(!$Verbose) {
	unlink "$tempDir/$name.plog";
	}		
} # close clear 

# Clear source files
if ($noSource) {
my @del_src;
find(\&del_src_tex, $imageDir);
sub del_src_tex{
my $srcimg = $_;
# search image file source 
if(-f $srcimg && $srcimg =~ /($name-(fig|exa)-\d+)$ext/){
push @del_src, $File::Find::name;
	}
}
unlink @del_src;
}# end source clear

__END__
