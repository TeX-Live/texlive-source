#!/usr/bin/env perl
use v5.26;
use File::Basename;
use Getopt::Long qw(:config bundling_values require_order no_ignore_case);
use File::Temp qw(tempdir);
use File::Copy;
use Config;
use File::Spec::Functions qw(catfile devnull);
use File::Find;
use IO::Compress::Zip qw(:all);
use Archive::Tar;
use POSIX qw(strftime);
use Cwd;
use autodie;
#use Data::Dumper;

### Directory for work and temp files
my $tempDir = tempdir( CLEANUP => 1);
my $workdir = cwd;
my $null    = devnull();

### Program identification
my $program   = "LTXimg";
my $nv        = 'v1.7';
my $copyright = <<END_COPYRIGHT ;
[2019-08-24] (c) 2013-2019 by Pablo Gonzalez, pablgonz<at>yahoo.com
END_COPYRIGHT

### Default values
my $prefix   = 'fig';          # defaul prefix for extract files
my $skiptag  = 'noltximg';     # internal tag for regex
my $extrtag  = 'ltximg';       # internal tag for regex
my $imageDir = "images";       # dir for images
my $verbcmd  = "myverb";       # set \myverb command
my $margins  = "0";            # margins for pdfcrop
my $DPI      = "150";          # dpi for image formats
my $zip      = 0;              # zip images dir
my $tar      = 0;              # tar.gz images dir
my $arara    = 0;              # use arara to compiler
my $force    = 0;              # force capture \psset|\tikzset
my $latex    = 0;              # compiling all images using latex
my $dvips    = 0;              # compiling output using dvips>ps2pdf
my $dvipdf   = 0;              # compiling all images using dvipdfmx
my $xetex    = 0;              # compiling all images using xelatex
my $luatex   = 0;              # compiling all images using lualatex
my $noprew   = 0;              # don't use preview packpage
my $srcenv   = 0;              # create src code for environments
my $subenv   = 0;              # create sub document for environments
my @extr_env_tmp;              # save extract environments
my @skip_env_tmp;              # save skip some environments
my @verb_env_tmp;              # save verbatim environments
my @verw_env_tmp;              # save verbatim write environments
my @delt_env_tmp;              # save delete environments in output file
my @clean;                     # clean options
my $pdf      = 1;              # create a PDF image file
my $run      = 1;              # run mode compiler
my $crop     = 1;              # croped pdf image files
my $gray     = 0;              # create a gray scale images
my $output;                    # set output name for outfile
my $outfile  = 0;              # write output file
my $outsrc   = 0;              # enable write src env files
my $debug    = 0;              # debug
my $PSTexa   = 0;              # extract PSTexample environment
my $STDenv   = 0;              # extract standart environments
my $verbose  = 0;              # verbose

### Search Ghostscript
# The next code it's part of pdfcrop adapted from TexLive 2014
# Windows detection
my $Win = 0;
$Win = 1 if $^O =~ /mswin32/i;
$Win = 1 if $^O =~ /cygwin/i;

my $archname = $Config{'archname'};
$archname = 'unknown' unless defined $Config{'archname'};

# get Ghostscript command name
my $gscmd = '';
sub find_ghostscript () {
    return if $gscmd;
    if ($debug) {
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
    print "* OS name: $^O\n" if $debug;
    print "* Arch name: $archname\n" if $debug;
    print "* System: $system\n" if $debug;
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
                $gscmd = $candidate;
                $found = 1;
                print "* Found ($candidate): $file\n" if $debug;
                last;
            }
            print "* Not found ($candidate): $file\n" if $debug;
        }
        last if $found;
    }
    if (not $found and $Win) {
        $found = SearchRegistry();
    }
    if ($found) {
        print "* Autodetected ghostscript command: $gscmd\n" if $debug;
    }
    else {
        $gscmd = $$candidates_ref[0];
        print "* Default ghostscript command: $gscmd\n" if $debug;
    }
}

sub SearchRegistry () {
    my $found = 0;
    eval 'use Win32::TieRegistry qw|KEY_READ REG_SZ|;';
    if ($@) {
        if ($debug) {
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
                if $debug;
        return $found;
    }
    print "* Search registry at `$current_key'.\n" if $debug;
    my %list;
    foreach my $key_name_gs (grep /Ghostscript/i, $software->SubKeyNames()) {
        $current_key = "$key_name_software$key_name_gs/";
        print "* Registry entry found: $current_key\n" if $debug;
        my $key_gs = $software->Open($key_name_gs, $open_params);
        if (not $key_gs) {
            print "* Cannot open registry key `$current_key'!\n" if $debug;
            next;
        }
        foreach my $key_name_version ($key_gs->SubKeyNames()) {
            $current_key = "$key_name_software$key_name_gs/$key_name_version/";
            print "* Registry entry found: $current_key\n" if $debug;
            if (not $key_name_version =~ /^(\d+)\.(\d+)$/) {
                print "  The sub key is not a version number!\n" if $debug;
                next;
            }
            my $version_main = $1;
            my $version_sub = $2;
            $current_key = "$key_name_software$key_name_gs/$key_name_version/";
            my $key_version = $key_gs->Open($key_name_version, $open_params);
            if (not $key_version) {
                print "* Cannot open registry key `$current_key'!\n" if $debug;
                next;
            }
            $key_version->FixSzNulls(1);
            my ($value, $type) = $key_version->GetValue('GS_DLL');
            if ($value and $type == REG_SZ()) {
                print "  GS_DLL = $value\n" if $debug;
                $value =~ s|([\\/])([^\\/]+\.dll)$|$1gswin32c.exe|i;
                my $value64 = $value;
                $value64 =~ s/gswin32c\.exe$/gswin64c.exe/;
                if ($archname =~ /mswin32-x64/i and -f $value64) {
                    $value = $value64;
                }
                if (-f $value) {
                    print "EXE found: $value\n" if $debug;
                }
                else {
                    print "EXE not found!\n" if $debug;
                    next;
                }
                my $sortkey = sprintf '%02d.%03d %s',
                        $version_main, $version_sub, $key_name_gs;
                $list{$sortkey} = $value;
            }
            else {
                print "Missing key `GS_DLL' with type `REG_SZ'!\n" if $debug;
            }
        }
    }
    foreach my $entry (reverse sort keys %list) {
        $gscmd = $list{$entry};
        print "* Found (via registry): $gscmd\n" if $debug;
        $found = 1;
        last;
    }
    return $found;
} # end GS search

### If windows
if ($Win and $gscmd =~ /\s/) { $gscmd = "\"$gscmd\"";}

### Call GS
find_ghostscript();

### Program identification, options and help for command line

my $licensetxt = <<END_LICENSE ;
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
END_LICENSE

my $title = "$program $nv $copyright";
my $usage = <<"END_OF_USAGE";
${title}** Description **
   LTXimg is a "perl" script that automates the process of extracting and
   converting "environments" provided by tikz, pstricks and other packages
   from LaTeX file to image formats in "individual files" using ghostscript
   and poppler-utils. Generates a one file with only extracted environments
   and other with all extracted environments converted to \\includegraphics.

** Syntax **
 \$ ltximg [<compiler>] [<options>] [--] <filename>.<tex|ltx>

** Usage **
 \$ ltximg --latex [<options>] <file.tex>
 \$ ltximg --arara [<options>] <file.tex>
 \$ ltximg [<options>] <file.tex>
 \$ ltximg <file.tex>

   If used without [<compiler>] and [<options>] the extracted environments
   are converted to pdf image format and saved in the "/images" directory
   using "pdflatex" and "preview" package. Relative or absolute paths for
   directories and files is not supported. If the last option take a list
   separated by commas, you need -- at the end.

** Default environments extract **
        pspicture tikzpicture pgfpicture psgraph postscript PSTexample

** Options **
                                                                    [default]
-h, --help            Display command line help and exit            [off]
-l, --license         Display GPL license and exit                  [off]
-v, --version         Display current version ($nv) and exit       [off]
-t, --tif             Create .tif files using ghostscript           [$gscmd]
-b, --bmp             Create .bmp files using ghostscript           [$gscmd]
-j, --jpg             Create .jpg files using ghostscript           [$gscmd]
-p, --png             Create .png files using ghostscript           [$gscmd]
-e, --eps             Create .eps files using poppler-utils         [pdftops]
-s, --svg             Create .svg files using poppler-utils         [pdftocairo]
-P, --ppm             Create .ppm files using poppler-utils         [pdftoppm]
-g, --gray            Gray scale for images using ghostscript       [off]
-f, --force           Capture "\\psset" and "\\tikzset" to extract    [off]
-n, --noprew          Create images files whitout "preview" package [off]
-d <integer>, --dpi <integer>
                      Dots per inch resolution for images           [150]
-m <integer>, --margin <integer>
                      Set margins for pdfcrop                       [0]
--imgdir <dirname>    Set name of directory to save images          [images]
--zip                 Compress files generated in .zip format       [off]
--tar                 Compress files generated in .tar.gz format    [off]
-o <filename>, --output <filename>
                      Create output file                            [off]
--verbose             Verbose printing                              [off]
--srcenv              Create files whit only code environment       [off]
--subenv              Create files whit preamble and code           [off]
--latex               Using latex>dvips>ps2pdf for compiler input
                      and pdflatex for compiler output              [off]
--dvips               Using latex>dvips>ps2pdf for compiler input
                      and latex>dvips>ps2pdf for compiler output    [off]
--arara               Use arara for compiler input and output       [off]
--xetex               Using xelatex for compiler input and output   [off]
--dvipdf              Using dvipdfmx for compiler input and output  [off]
--luatex              Using lualatex for compiler input and output  [off]
--prefix <string>     Set prefix append to each image file          [off]
--norun               Run script, but no create images files        [off]
--nopdf               Don't create a ".pdf" image files             [off]
--nocrop              Don't run pdfcrop                             [off]
--verbcmd <cmdname>   Set "\\cmdname" verbatim command               [myverb]
--clean (doc|pst|tkz|all|off)
                      Removes specific text in output file          [doc]
--extrenv <env1,...>  Add new environments to extract               [empty]
--skipenv <env1,...>  Skip environments to extract                  [empty]
--verbenv <env1,...>  Add verbatim environments                     [empty]
--writenv <env1,...>  Add verbatim write environments               [empty]
--deltenv <env1,...>  Delete environments in output file            [empty]

** Example **
 \$ ltximg --latex -e -p --srcenv --imgdir=mypics -o test-out test-in.ltx
 \$ ltximg --latex -ep --srcenv --imgdir mypics -o test-out  test-in.ltx

   Create a "/mypics" directory whit all extracted environments converted to
   image formats (.pdf, .eps, .png), individual files whit source code (.tex)
   for all extracted environments, a file "test-out.ltx" whit all extracted
   environments converted to \\includegraphics and file "test-in-fig-all.tex"
   with only the extracted environments using latex>dvips>ps2pdf and preview
   package for <input file> and pdflatex for <output file>.

** Documentation **
   For extended documentation use:
 \$ texdoc ltximg
END_OF_USAGE

### Error in command line
sub errorUsage { die "@_ (try ltximg --help for more information)\n"; }

### Getopt::Long configuration for command line
my %opts_cmd;

my $result=GetOptions (
# short and long options
    'h|help'         => \$opts_cmd{help}, # help
    'v|version'      => \$opts_cmd{version}, # version
    'l|license'      => \$opts_cmd{license}, # license
    'd|dpi=i'        => \$DPI, # numeric
    'm|margin=i'     => \$margins, # numeric
    'b|bmp'          => \$opts_cmd{bmp}, # gs
    't|tif'          => \$opts_cmd{tif}, # gs
    'j|jpg'          => \$opts_cmd{jpg}, # gs
    'p|png'          => \$opts_cmd{png}, # gs
    's|svg'          => \$opts_cmd{svg}, # pdftocairo
    'e|eps'          => \$opts_cmd{eps}, # pdftops
    'P|ppm'          => \$opts_cmd{ppm}, # pdftoppm
    'g|gray'         => \$gray,   # gray (bolean)
    'f|force'        => \$force,  # force (bolean)
    'n|noprew'       => \$noprew, # no preview (bolean)
    'o|output=s{1}'  => \$output, # output file name (string)
# bolean options
    'subenv'         => \$subenv, # subfile environments (bolean)
    'srcenv'         => \$srcenv, # source files (bolean)
    'arara'          => \$arara,  # arara compiler
    'xetex'          => \$xetex,  # xelatex compiler
    'latex'          => \$latex,  # latex compiler
    'luatex'         => \$luatex, # lualatex compiler
    'dvips'          => \$dvips,  # dvips compiler
    'dvipdf'         => \$dvipdf, # dvipdfmx compiler
    'zip'            => \$zip,    # zip images dir
    'tar'            => \$tar,    # tar images dir
# string options from command line
    'extrenv=s{1,9}' => \@extr_env_tmp, # extract environments
    'skipenv=s{1,9}' => \@skip_env_tmp, # skip environment
    'verbenv=s{1,9}' => \@verb_env_tmp, # verbatim environment
    'writenv=s{1,9}' => \@verw_env_tmp, # verbatim write environment
    'deltenv=s{1,9}' => \@delt_env_tmp, # delete environment
# string options
    'imgdir=s{1}'    => \$imageDir, # images dir name
    'verbcmd=s{1}'   => \$verbcmd,  # \myverb inline (string)
    'prefix=s{1}'    => \$prefix,   # prefix
# negated options
    'crop!'          => \$crop,    # run pdfcrop
    'pdf!'           => \$pdf,     # pdf image format
    'clean=s{1}'     => \@clean,   # clean output file
    'run!'           => \$run,     # run compiler
    'debug!'         => \$debug,   # debug mode
    'verbose!'       => \$verbose, # debug mode,
    ) or die $usage;

### Remove white space and '=' in array captured from command line
s/^\s*(\=):?|\s*//mg foreach @extr_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @skip_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @verb_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @verw_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @delt_env_tmp;

### Split comma separte list options from command line
@extr_env_tmp = split(/,/,join('',@extr_env_tmp));
@skip_env_tmp = split(/,/,join('',@skip_env_tmp));
@verb_env_tmp = split(/,/,join('',@verb_env_tmp));
@verw_env_tmp = split(/,/,join('',@verw_env_tmp));
@delt_env_tmp = split(/,/,join('',@delt_env_tmp));

### Validate input string options
if (grep( /(^\-|^\.).*?/, @extr_env_tmp )) {
    die errorUsage "Invalid argument for --extrenv option";
}
if (grep( /(^\-|^\.).*?/, @skip_env_tmp )) {
    die errorUsage "Invalid argument for --skipenv option";
}
if (grep( /(^\-|^\.).*?/, @verb_env_tmp )) {
    die errorUsage "Invalid argument for --verbenv option";
}
if (grep( /(^\-|^\.).*?/, @verw_env_tmp )) {
    die errorUsage "Invalid argument for --writenv option";
}
if (grep( /(^\-|^\.).*?/, @delt_env_tmp )) {
    die errorUsage "Invalid argument for --deltenv option";
}

### Help
if (defined $opts_cmd{help}) {
    find_ghostscript();
    print $usage;
    exit(0);
}

### Version
if (defined $opts_cmd{version}) {
    print $title;
    exit(0);
}

### Licence
if (defined $opts_cmd{license}) {
    print $licensetxt;
    exit(0);
}

### Set tmp random name for name-fig-tmp (temp files)
my $tmp = "$$";

### Check --srcenv and --subenv option from command line
if ($srcenv && $subenv) {
    die errorUsage "--srcenv and --subenv options are mutually exclusive";
}

### Check the input file from command line
@ARGV > 0 or errorUsage "Input filename missing";
@ARGV < 2 or errorUsage "Unknown option or too many input files";

### Check input file extention
my @SuffixList = ('.tex', '', '.ltx'); # posibles
my ($name, $path, $ext) = fileparse($ARGV[0], @SuffixList);
$ext = '.tex' if not $ext;

### Read input file in memory (slurp), need :crlf for windows/linux
open my $INPUTfile, '<:crlf', "$name$ext";
my $ltxfile;
    {
        local $/;
        $ltxfile = <$INPUTfile>;
    }
close $INPUTfile;

### Funtion uniq
sub uniq {
    my %seen;
    grep !$seen{$_}++, @_;
}

### Funtion array_minus
sub array_minus(\@\@) {
    my %e = map{ $_ => undef } @{$_[1]};
    return grep( ! exists( $e{$_} ), @{$_[0]} );
}

### Funtion to create hash begin -> BEGIN, end -> END
sub crearhash {
    my %cambios;
    for my $aentra(@_){
        for my $initend (qw(begin end)) {
            $cambios{"\\$initend\{$aentra"} = "\\\U$initend\E\{$aentra";
            }
        }
    return %cambios;
}

### Default environment to extract
my @extr_tmp = qw (
   postscript tikzpicture pgfpicture pspicture psgraph
   );

push (@extr_env_tmp, @extr_tmp);

### Default verbatim environment
my @verb_tmp = qw (
   Example CenterExample SideBySideExample PCenterExample PSideBySideExample
   verbatim Verbatim BVerbatim LVerbatim SaveVerbatim PSTcode
   LTXexample tcblisting spverbatim minted listing lstlisting
   alltt comment chklisting verbatimtab listingcont boxedverbatim
   demo sourcecode xcomment pygmented pyglist program programl
   programL programs programf programsc programt
   );

push (@verb_env_tmp, @verb_tmp);

### Default verbatim write environment
my @verbw_tmp = qw (
   scontents filecontents tcboutputlisting tcbexternal tcbwritetmp extcolorbox extikzpicture
   VerbatimOut verbatimwrite PSTexample filecontentsdef filecontentshere filecontentsdefmacro
   );

push (@verw_env_tmp, @verbw_tmp);

### Rules to capture in regex
my $braces      = qr/ (?:\{)(.+?)(?:\}) /msx;
my $braquet     = qr/ (?:\[)(.+?)(?:\]) /msx;
my $no_corchete = qr/ (?:\[ .*? \])?    /msx;

### Capture new verbatim environments defined in input file
my @new_verb = qw (
   newtcblisting DeclareTCBListing ProvideTCBListing NewTCBListing
   lstnewenvironment NewListingEnvironment NewProgram specialcomment
   includecomment DefineVerbatimEnvironment newverbatim newtabverbatim
   );

### Regex to capture names for new verbatim environments from input file
my $newverbenv = join "|", map quotemeta, sort { length $a <=> length $b } @new_verb;
   $newverbenv = qr/\b(?:$newverbenv) $no_corchete $braces/msx;

### Capture new verbatim write environments defined in input file
my @new_verb_write = qw (
   renewtcbexternalizetcolorbox renewtcbexternalizeenvironment
   newtcbexternalizeenvironment newtcbexternalizetcolorbox
   );

### Regex to capture names for new verbatim write environments from input file
my $newverbwrt = join "|", map quotemeta, sort { length $a <=> length $b } @new_verb_write;
   $newverbwrt = qr/\b(?:$newverbwrt) $no_corchete $braces/msx;

### Regex to capture MINTED related environments
my $mintdenv  = qr/\\ newminted $braces (?:\{.+?\})      /x;
my $mintcenv  = qr/\\ newminted $braquet (?:\{.+?\})     /x;
my $mintdshrt = qr/\\ newmint $braces (?:\{.+?\})        /x;
my $mintcshrt = qr/\\ newmint $braquet (?:\{.+?\})       /x;
my $mintdline = qr/\\ newmintinline $braces (?:\{.+?\})  /x;
my $mintcline = qr/\\ newmintinline $braquet (?:\{.+?\}) /x;

### Pass input file to @array and remove % and comments
my @filecheck = $ltxfile;
s/%.*\n//mg foreach @filecheck; # del comments
s/^\s*|\s*//mg foreach @filecheck; # del white space
my $filecheck = join '', @filecheck;

### Capture \newverbatim write names in input file
my @newv_write = $filecheck =~ m/$newverbwrt/xg;

### Add @newv_write defined in input file to @verw_env_tmp
push (@verw_env_tmp, @newv_write);

### Capture \newminted{$mintdenv}{options} (for)
my @mint_denv = $filecheck =~ m/$mintdenv/xg;

### Capture \newminted[$mintcenv]{lang} (for)
my @mint_cenv = $filecheck =~ m/$mintcenv/xg;

### Capture \newmint{$mintdshrt}{options} (while)
my @mint_dshrt = $filecheck =~ m/$mintdshrt/xg;

### Capture \newmint[$mintcshrt]{lang}{options} (while)
my @mint_cshrt = $filecheck =~ m/$mintcshrt/xg;

### Capture \newmintinline{$mintdline}{options} (while)
my @mint_dline = $filecheck =~ m/$mintdline/xg;

### Capture \newmintinline[$mintcline]{lang}{options} (while)
my @mint_cline = $filecheck =~ m/$mintcline/xg;

### Capture \newverbatim environments in input file (for)
my @verb_input = $filecheck =~ m/$newverbenv/xg;

### Add new verbatim environment defined in input file to @vrbenv
push (@verb_env_tmp,@mint_denv,@mint_cenv,@verb_input);

### Append "code" (minted)
if (!@mint_denv == 0) {
    $mintdenv  = join "\n", map { qq/$_\Qcode\E/ } @mint_denv;
    @mint_denv = split /\n/, $mintdenv;
}

### Append "inline" (minted)
if (!@mint_dline == 0) {
    $mintdline  = join "\n", map { qq/$_\Qinline\E/ } @mint_dline;
    @mint_dline = split /\n/, $mintdline;
}

### Join all minted inline/short and lstinline in @array
my @mintline;
my @mint_tmp = qw ( mint  mintinline lstinline);
push (@mintline,@mint_dline,@mint_cline,@mint_dshrt,@mint_cshrt,@mint_tmp);
      @mintline = uniq(@mintline);

### Create a regex using @mintline
my $mintline = join "|", map quotemeta, sort { length $a <=> length $b } @mintline;
   $mintline = qr/\b(?:$mintline)/x;

### Options from input file
# % ltximg : extrenv : {extrenv1, extrenv2, ... , extrenvn}
# % ltximg : skipenv : {skipenv1, skipenv2, ... , skipenvn}
# % ltximg : verbenv : {verbwrt1, verbwrt2, ... , verbwrtn}
# % ltximg : writenv : {verbwrt1, verbwrt2, ... , verbwrtn}
# % ltximg : deltenv : {deltenv1, deltenv2, ... , deltenvn}
# % ltximg : options : {opt1=arg, opt2=arg, ... , bolean}

### Regex to capture before preamble $readoptfile, %opt_from_file;
my $readoptfile = qr/
    ^ %+ \s* ltximg (?&SEPARADOR) (?<clave>(?&CLAVE)) (?&SEPARADOR) \{ (?<argumentos>(?&ARGUMENTOS)) \}
    (?(DEFINE)
    (?<CLAVE>      \w+       )
    (?<ARGUMENTOS> .+?       )
    (?<SEPARADOR>  \s* : \s* )
    )
/mx;

### Split input file, $atbegindoc contain % ltximg : <argument>
my ($atbegindoc, $document) = $ltxfile =~ m/\A (\s* .*? \s*) (\\documentclass.*)\z/msx;

### Process options from input file
my %opts_file;

while ($atbegindoc =~ /$readoptfile/g) {
    my ($clave, $argumentos) = @+{qw(clave argumentos)};
    my  @argumentos = split /\s*,\s*?/, $argumentos;
    for (@argumentos) { s/^ \s* | \s* $//gx; }
        if  ($clave eq 'options') {
            for my $argumento (@argumentos) {
                if ($argumento =~ /(?<key>\S+) \s* = \s* (?<valor>\S+)/x) {
                    $opts_file{$clave}{$+{'key'}} = $+{'valor'};
                        } # close for
                   else {
                    $opts_file{$clave}{$argumento} = 1;
                        }
            } # close for
        } # close if
        else {
            push @{ $opts_file{ $clave } }, @argumentos;
        }
} # close while

### Validate clean
my %clean = map { $_ => 1 } @clean;

### By default clean = doc
$clean{doc} = 1 ;

### Set clean options from input file
$clean{doc} = 1 if ($opts_file{options}{clean} eq 'doc');
$clean{off} = 1 if ($opts_file{options}{clean} eq 'off');
$clean{pst} = 1 if ($opts_file{options}{clean} eq 'pst');
$clean{tkz} = 1 if ($opts_file{options}{clean} eq 'tkz');
$clean{all} = 1 if ($opts_file{options}{clean} eq 'all');

### Set clean options for script
if ($clean{pst} or $clean{tikz}) { $clean{doc} = 1; }
if ($clean{all}) { @clean{qw(pst doc tkz)} = (1) x 3; }
if ($clean{off}) { undef %clean; }

### Add extract options from input file
if (exists $opts_file{extract}) {
    push @extr_env_tmp, @{ $opts_file{extract} };
}

### Add skipenv options from input file
if (exists $opts_file{skipenv}) {
    push @skip_env_tmp, @{ $opts_file{skipenv} };
}

### Add verbenv options from input file
if (exists $opts_file{verbenv}) {
    push @verb_env_tmp, @{ $opts_file{verbenv} };
}

### Add writenv options from input file
if (exists $opts_file{writenv}) {
    push @verw_env_tmp, @{ $opts_file{writenv} };
}

### Add deltenv options from input file
if (exists $opts_file{deltenv}) {
    push @delt_env_tmp, @{ $opts_file{deltenv} };
}

### Set \myverb|<code>| options from input file
if (exists $opts_file{options}{verbcmd}) {
    $verbcmd = $opts_file{options}{verbcmd};
}

### Create  @env_all_tmp contain all environments
my @env_all_tmp;
push(@env_all_tmp,@extr_env_tmp,@skip_env_tmp,@verb_env_tmp,@verw_env_tmp,@delt_env_tmp);
@env_all_tmp = uniq(@env_all_tmp);

### Create @no_env_all_tmp contain all No extracted environments
my @no_env_all_tmp;
push(@no_env_all_tmp,@skip_env_tmp,@verb_env_tmp,@verw_env_tmp,@delt_env_tmp);
@no_env_all_tmp = uniq(@no_env_all_tmp);

### The operation return @extract environment
my @extract = array_minus(@env_all_tmp,@no_env_all_tmp);
   @extract = uniq(@extract);

### The operation return @no_extract
my @no_extract = array_minus(@env_all_tmp,@extract);
my @no_skip;
push(@no_skip,@verb_env_tmp,@verw_env_tmp,@delt_env_tmp);
my @skipped = array_minus(@no_extract,@no_skip);
   @skipped = uniq(@skipped);

### The operation return @delte_env environment
my @no_ext_skip = array_minus(@no_extract,@skipped);
my @no_del;
push(@no_del,@verb_env_tmp,@verw_env_tmp);
my @delete_env = array_minus(@no_ext_skip,@no_del);
   @delete_env = uniq(@delete_env);

### The operation return @verbatim environment
my @no_ext_skip_del = array_minus(@no_ext_skip,@delete_env);
my @verbatim = array_minus(@no_ext_skip_del,@verw_env_tmp);

### The operation return @verbatim write environment
my @verbatim_w = array_minus(@no_ext_skip_del,@verbatim);

### Create @env_all for hash and replace in while
my @no_verb_env;
push(@no_verb_env,@extract,@skipped,@delete_env,@verbatim_w);
my @no_verw_env;
push(@no_verw_env,@extract,@skipped,@delete_env,@verbatim);

### Reserved words in verbatim inline (while)
my %changes_in = (
# ltximg tags
    '%<*ltximg>'      =>  '%<*LTXIMG>',
    '%</ltximg>'      =>  '%</LTXIMG>',
    '%<*noltximg>'    =>  '%<*NOLTXIMG>',
    '%</noltximg>'    =>  '%</NOLTXIMG>',
    '%<*remove>'      =>  '%<*REMOVE>',
    '%</remove>'      =>  '%</REMOVE>',
    '%<*ltximgverw>'  =>  '%<*LTXIMGVERW>',
    '%</ltximgverw>'  =>  '%</LTXIMGVERW>',
# pst/tikz set
    '\psset'          =>  '\PSSET',
    '\tikzset'        =>  '\TIKZSET',
# pspicture
    '\pspicture'      =>  '\TRICKS',
    '\endpspicture'   =>  '\ENDTRICKS',
# pgfpicture
    '\pgfpicture'     =>  '\PGFTRICKS',
    '\endpgfpicture'  =>  '\ENDPGFTRICKS',
# tikzpicture
    '\tikzpicture'    =>  '\TKZTRICKS',
    '\endtikzpicture' =>  '\ENDTKZTRICKS',
# psgraph
    '\psgraph'        =>  '\PSGRAPHTRICKS',
    '\endpsgraph'     =>  '\ENDPSGRAPHTRICKS',
# some reserved
    '\usepackage'     =>  '\USEPACKAGE',
    '{graphicx}'      =>  '{GRAPHICX}',
    '\graphicspath{'  =>  '\GRAPHICSPATH{',
    );

### Changues \begin... \end in verbatim inline
my %init_end = (
# begin{ and end{
    '\begin{' => '\BEGIN{',
    '\end{'   => '\END{',
    );

### Changues for \begin{document} ... \end{document}
my %document = (
# begin/end document for split
    '\begin{document}' => '\BEGIN{document}',
    '\end{document}'   => '\END{document}',
    );

### Reverse for extract and output file
my %changes_out = (
# ltximg tags
    '\begin{nopreview}' =>  '%<*noltximg>',
    '\end{nopreview}'   =>  '%</noltximg>',
# pst/tikz set
    '\PSSET'            =>  '\psset',
    '\TIKZSET'          =>  '\tikzset',
# pspicture
    '\TRICKS'           =>  '\pspicture',
    '\ENDTRICKS'        =>  '\endpspicture',
# pgfpicture
    '\PGFTRICKS'        =>  '\pgfpicture',
    '\ENDPGFTRICKS'     =>  '\endpgfpicture',
# tikzpicture
    '\TKZTRICKS'        =>  '\tikzpicture',
    '\ENDTKZTRICKS'     =>  '\endtikzpicture',
# psgraph
    '\PSGRAPHTRICKS'    =>  '\psgraph',
    '\ENDPSGRAPHTRICKS' =>  '\endpsgraph',
# some reserved
    '\USEPACKAGE'       =>  '\usepackage',
    '{GRAPHICX}'        =>  '{graphicx}',
    '\GRAPHICSPATH{'    =>  '\graphicspath{',
# begin{ and end{
    '\BEGIN{'           =>  '\begin{',
    '\END{'             =>  '\end{',
    );

### Reverse tags, need back in all file to extract
my %reverse_tag = (
# ltximg tags
    '%<*LTXIMG>'     => '%<*ltximg>',
    '%</LTXIMG>'     => '%</ltximg>',
    '%<*NOLTXIMG>'   => '%<*noltximg>',
    '%</NOLTXIMG>'   => '%</noltximg>',
    '%<*REMOVE>'     => '%<*remove>',
    '%</REMOVE>'     => '%</remove>',
    '%<*LTXIMGVERW>' => '%<*ltximgverw>',
    '%</LTXIMGVERW>' => '%</ltximgverw>',
    );

### Creatate a hash for changues
my %extract_env      = crearhash(@extract);
my %skiped_env       = crearhash(@skipped);
my %verb_env         = crearhash(@verbatim);
my %verbw_env        = crearhash(@verbatim_w);
my %delete_env       = crearhash(@delete_env);
my %change_verbw_env = crearhash(@no_verw_env);
my %change_verb_env  = crearhash(@no_verb_env);

### Join changues in new hash
my %cambios = (%changes_in, %init_end);

### Variables y constantes
my $no_del = "\0";
my $del    = $no_del;

### Rules
my $llaves      = qr/\{ .+? \}                                                          /x;
my $no_llaves   = qr/(?: $llaves )?                                                     /x;
my $corchetes   = qr/\[ .+? \]                                                          /x;
my $nestedbr    = qr/(\{(?>[^\{\}\\]++|\\.|(?R))*+\})                                   /x;
my $delimitador = qr/\{ (?<del>.+?) \}                                                  /x;
my $scontents   = qr/Scontents [*]? $no_corchete                                        /ix;
my $verb        = qr/(?:((spv|(?:q|f)?v|V)erb|$verbcmd)[*]?)                            /ix;
my $lst         = qr/(?:(lst|pyg)inline)(?!\*) $no_corchete                             /ix;
my $mint        = qr/(?: $mintline |SaveVerb) (?!\*) $no_corchete $no_llaves $llaves    /ix;
my $no_mint     = qr/(?: $mintline) (?!\*) $no_corchete                                 /ix;
my $marca       = qr/\\ (?:$verb | $lst |$scontents | $mint |$no_mint) (?:\s*)? (\S) .+? \g{-1}     /sx;
my $comentario  = qr/^ \s* \%+ .+? $                                                    /mx;
my $definedel   = qr/\\ (?: DefineShortVerb | lstMakeShortInline| MakeSpecialShortVerb ) [*]? $no_corchete $delimitador /ix;
my $indefinedel = qr/\\ (?: (Undefine|Delete)ShortVerb | lstDeleteShortInline) $llaves  /ix;

### Changues in input file (in memory)
while ($document =~
        / $marca
        | $comentario
        | $definedel
        | $indefinedel
        | $del .+? $del
        /pgmx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my  $encontrado = ${^MATCH};
    if ($encontrado =~ /$definedel/) {
        $del = $+{del};
        $del = "\Q$+{del}" if substr($del,0,1) ne '\\';
    }
    elsif ($encontrado =~ /$indefinedel/) {
        $del = $no_del;
    }
    else {
        while (my($busco, $cambio) = each %cambios) {
         $encontrado =~ s/\Q$busco\E/$cambio/g;
            } # close while
        substr $document, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($document) = $pos_inicial + length $encontrado;
    }
}

### Regex for verbatim inline whit braces {...}
my $fvextra    = qr /\\ (?: (Save|Esc)Verb [*]?) $no_corchete                     /x;
my $mintedbr   = qr /\\ (?:$mintline|pygment) (?!\*) $no_corchete $no_llaves      /x;
my $tcbxverb   = qr /\\ (?: tcboxverb [*]?| Scontents [*]? |$verbcmd [*]?|lstinline) $no_corchete /x;
my $verb_brace = qr /   (?:$tcbxverb|$mintedbr|$fvextra) (?:\s*)? $nestedbr      /x;

### Changue \verb*{code} for verbatim inline
while ($document =~ /$verb_brace/pgmx) {
        my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
        my  $encontrado = ${^MATCH};
        while (my($busco, $cambio) = each %cambios) {
               $encontrado =~ s/\Q$busco\E/$cambio/g;
        } # close while
        substr $document, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($document) = $pos_inicial + length $encontrado;
} # close while

### Changue <*TAGS> to <*tags>
my $ltxtags  =  join "|", map {quotemeta} sort { length($a)<=>length($b) } keys %reverse_tag;
   $document =~ s/^($ltxtags)/$reverse_tag{$1}/gmsx;

### Define environments Verbatim standart
my $verbatim = join "|", map quotemeta, sort { length $a <=> length $b } @verbatim;
   $verbatim = qr/$verbatim/x;

### Define environments Verbatim write
my $verbatim_w = join "|", map quotemeta, sort { length $a <=> length $b } @verbatim_w;
   $verbatim_w = qr/$verbatim_w/x;

### Define environments to skip
my $skipenv = join "|", map quotemeta, sort { length $a <=> length $b } @skipped;
   $skipenv = qr/$skipenv/x;

### Define environments to extract
my $environ = join "|", map quotemeta, sort { length $a <=> length $b } @extract;
   $environ = qr/$environ/x;

### Define environments to delete
my $delenv = join "|", map quotemeta, sort { length $a <=> length $b } @delt_env_tmp;
   $delenv = qr/$delenv/x;

### Split file by lines
my @lineas = split /\n/, $document;

### Hash and Regex for changues
my %replace = (%change_verb_env,%changes_in,%document);
my $find    = join "|", map {quotemeta} sort { length($a)<=>length($b) } keys %replace;

### Change in $verbatim and $verbatim_w
my $DEL;
for (@lineas) {
    if (/\\begin\{($verbatim\*?)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
        s/($find)/$replace{$1}/g; }
    if (/\\begin\{($verbatim_w\*?)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
    my %replace = (%change_verbw_env,%changes_in,%document);
    my $find    = join "|", map {quotemeta} sort { length($a)<=>length($b) } keys %replace;
        s/($find)/$replace{$1}/g; }
} # close for

### Join lines in $document
$document = join("\n", @lineas);

### Split input file
my ($preamble,$bodydoc,$enddoc) = $document =~ m/\A (.+?) (\\begin\{document\} .+?)(\\end\{document\}.*)\z/msx;

### Regex for delete environment
my $delt_env = qr /
                (
                    (?:
                        \\begin\{$delenv\*?\}
                      (?:
                        (?>[^\\]+)|
                        \\
                        (?!begin\{$delenv\*?\})
                        (?!end\{$delenv\*?\})|
                        (?-1)
                      )*
                      \\end\{$delenv\*?\}
                    )
                )
                /x;

### Regex for verbatim write environment
my $verb_wrt = qr /
                (
                    (?:
                        \\begin\{$verbatim_w\*?\}
                      (?:
                        (?>[^\\]+)|
                        \\
                        (?!begin\{$verbatim_w\*?\})
                        (?!end\{$verbatim_w\*?\})|
                        (?-1)
                      )*
                      \\end\{$verbatim_w\*?\}
                    )
                )
                /x;

### Pass $verb_wrt to %<*ltximgverw> ... %</ltximgverw>
$bodydoc =~ s/($verb_wrt)/\%<\*ltximgverw>\n$1\n\%<\/ltximgverw>/gmsx;

### Regex for skip environment
my $skip_env = qr /
                (
                    (?:
                        \\begin\{$skipenv\*?\}
                      (?:
                        (?>[^\\]+)|
                        \\
                        (?!begin\{$skipenv\*?\})
                        (?!end\{$skipenv\*?\})|
                        (?-1)
                      )*
                      \\end\{$skipenv\*?\}
                    )
                )
                /x;

### Pass %<*noltximg> ... %</noltximg> to \begin{nopreview} ... \end{nopreview}
$bodydoc =~ s/^\%<\*$skiptag>(.+?)\%<\/$skiptag>/\\begin\{nopreview\}$1\\end\{nopreview\}/gmsx;

### Pass $skip_env to \begin{nopreview} .+? \end{nopreview}
$bodydoc =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
              \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
              ($skip_env)/\\begin\{nopreview\}\n$1\n\\end\{nopreview\}\n/gmsx;

### Regex to extract environments
my $extr_env = qr /
                (
                    (?:
                        \\begin\{$environ\*?\}
                      (?:
                        (?>[^\\]+)|
                        \\
                        (?!begin\{$environ\*?\})
                        (?!end\{$environ\*?\})|
                        (?-1)
                      )*
                      \\end\{$environ\*?\}
                    )
                )
                /x;

### Set bolean options from input file
$zip    = 1 if exists $opts_file{options}{zip};
$tar    = 1 if exists $opts_file{options}{tar};
$force  = 1 if exists $opts_file{options}{force};
$run    = 0 if exists $opts_file{options}{norun};
$pdf    = 0 if exists $opts_file{options}{nopdf};
$crop   = 0 if exists $opts_file{options}{nocrop};
$noprew = 1 if exists $opts_file{options}{noprew};
$force  = 1 if exists $opts_file{options}{force};
$arara  = 1 if exists $opts_file{options}{arara};
$xetex  = 1 if exists $opts_file{options}{xetex};
$latex  = 1 if exists $opts_file{options}{latex};
$dvips  = 1 if exists $opts_file{options}{dvips};
$dvipdf = 1 if exists $opts_file{options}{dvipdf};
$luatex = 1 if exists $opts_file{options}{luatex};
$srcenv = 1 if exists $opts_file{options}{srcenv};
$subenv = 1 if exists $opts_file{options}{subenv};

### Check plain TeX syntax
my %special = map { $_ => 1 } @extract; # anon hash

### Convert \pspicture to LaTeX syntax
if (exists($special{pspicture})) {
    $bodydoc =~ s/
    \\pspicture(\*)?(.+?)\\endpspicture/\\begin{pspicture$1}$2\\end{pspicture$1}/gmsx;
}

### Convert \psgraph to LaTeX syntax
if (exists($special{psgraph})) {
    $bodydoc =~ s/
    \\psgraph(\*)?(.+?)\\endpsgraph/\\begin{psgraph$1}$2\\end{psgraph$1}/gmsx;
}

### Convert \tikzpicture to LaTeX syntax
if (exists($special{tikzpicture})) {
    $bodydoc =~ s/
    \\tikzpicture(.+?)\\endtikzpicture/\\begin{tikzpicture}$1\\end{tikzpicture}/gmsx;
}

### Convert \pgfpicture to LaTeX syntax
if (exists($special{pgfpicture})) {
    $bodydoc =~ s/
    \\pgfpicture(.+?)\\endpgfpicture/\\begin{pgfpicture}$1\\end{pgfpicture}/gmsx;
}

### Pass %<*ltximg> (.+?) %</ltximg> to \begin{preview} (.+?) \end{preview}
$bodydoc =~ s/^\%<\*$extrtag>(.+?)\%<\/$extrtag>/\\begin\{preview\}$1\\end\{preview\}/gmsx;

### $force mode for pstricks/psgraph/tikzpiture
if ($force) {
if (exists($special{pspicture}) or exists($special{psgraph})) { # pspicture or psgraph found
$bodydoc =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
              \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
              \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
              \\begin\{postscript\}.+?\\end\{postscript\}(*SKIP)(*F)|
              (?<code>
              (?:\\psset\{(?:\{.*?\}|[^\{])*\}.+?)?  # if exist ...save
              \\begin\{(?<env> pspicture\*?| psgraph)\} .+? \\end\{\k<env>\}
              )
              /\\begin\{preview\}\n$+{code}\n\\end\{preview\}/gmsx;
    } # close pspicture
if (exists($special{tikzpicture})) {# tikzpicture found
$bodydoc =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
              \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
              \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
              \\begin\{postscript\}.+?\\end\{postscript\}(*SKIP)(*F)|
              (?<code>
              (?:\\tikzset\{(?:\{.*?\}|[^\{])*\}.+?)?  # if exist ...save
              \\begin\{(?<env> tikzpicture)\} .+? \\end\{\k<env>\}
              )
              /\\begin\{preview\}\n$+{code}\n\\end\{preview\}/gmsx;
    } # close tikzpicture
} # close $force mode

### Pass $extr_env to \begin{preview} .+? \end{preview}
$bodydoc =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
              \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
              \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
              ($extr_env)/\\begin\{preview\}\n$1\n\\end\{preview\}/gmsx;

### The extract environments need back words to original
%replace = (%changes_out,%reverse_tag);
   $find = join "|", map {quotemeta} sort { length($a)<=>length($b) } keys %replace;

### Split $preamble by lines
@lineas = split /\n/, $preamble;

### Changues in verbatim write
for (@lineas) {
    if (/\\begin\{($verbatim_w\*?)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
        s/($find)/$replace{$1}/g; }
} # close for

### Join lines in $preamble
$preamble = join("\n", @lineas);

### Change back betwen \begin{preview} ... \end{preview}
@lineas = split /\n/, $bodydoc;

for (@lineas) {
    if (/\\begin\{(preview)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) { # range operator
        s/($find)/$replace{$1}/g; }
    if (/\\begin\{($verbatim_w\*?)(?{ $DEL = "\Q$^N" })\}/ .. /\\end\{$DEL\}/) {
        s/($find)/$replace{$1}/g; }
} # close for

### Join lines in $bodydoc
$bodydoc = join("\n", @lineas);

### preview environment for extract
my $BP = '\\\\begin\{preview\}';
my $EP = '\\\\end\{preview\}';

my @env_extract = $bodydoc =~ m/(?<=$BP)(.+?)(?=$EP)/gms;
my $envNo = scalar @env_extract;

### PSTexample environment for extract
my $BE = '\\\\begin\{PSTexample\}';
my $EE = '\\\\end\{PSTexample\}';

my @exa_extract = $bodydoc =~ m/(?<=$BE)(.+?)(?=$EE)/gms;
my $exaNo = scalar @exa_extract;

### Check if PSTexample environment found, 1 = run script
if ($exaNo!=0) { $PSTexa = 1; }

### Check if standart environment found, , 1 = run script
if ($envNo!=0) { $STDenv=1; }

### PSTexample environment suport
if ($PSTexa) { # append [graphic={[...]...}]
$exaNo = 1;
while ($bodydoc =~ /\\begin\{PSTexample\}(\[.+?\])?/gsm) {
    my $swpl_grap = "graphic=\{\[scale=1\]$imageDir/$name-$prefix-exa";
    my $corchetes = $1;
    my ($pos_inicial, $pos_final) = ($-[1], $+[1]);
        if (not $corchetes) { $pos_inicial = $pos_final = $+[0]; }
        if (not $corchetes  or  $corchetes =~ /\[\s*\]/) {
            $corchetes = "[$swpl_grap-$exaNo}]";
        }
    else { $corchetes =~ s/\]/,$swpl_grap-$exaNo}]/; }
    substr($bodydoc, $pos_inicial, $pos_final - $pos_inicial) = $corchetes;
    pos($bodydoc) = $pos_inicial + length $corchetes;
} # close while
continue { $exaNo++; } # increment counter

### Pass PSTexample to nopreview environment
$bodydoc =~ s/\%<\*ltximgverw>\n
              (?<code>
              \\begin\{PSTexample\} .+? \\end\{PSTexample\}
              )
              \n\%<\/ltximgverw>
              /\\begin\{nopreview\}\n$+{code}\n\\end\{nopreview\}/gmsx;
} # close PSTexa

### Command line script identification
print "$program $nv $copyright" ;

### Check if enviroment found in input file
if ($envNo == 0 and $exaNo == 0) {
    die errorUsage "ltximg can not find any environment to extract in file $name$ext";
      }
elsif ($envNo!= 0 and $exaNo!= 0) {
    say "The file $name$ext contain $envNo environment to extract and $exaNo PSTexample environment to extract";
      }
elsif ($envNo == 0 and $exaNo!= 0) {
    say "The file $name$ext contain $exaNo PSTexample environment to extract";
      }
else  { say "The file $name$ext contain $envNo environment to extract"; }

### Set output file name from input file
if (exists $opts_file{options}{output}){
    $output = $opts_file{options}{output};
}

### Validate output file name
if (defined $output) {
# Not contain - at begin
if ($output =~ /(^\-|^\.).*?/) {
    die errorUsage "$output it is not a valid name for output file";
}
# The name of the output file must be different that $name
if ($output eq "$name") { $output = "$name-out$ext"; }

# The name of the output file must be different that $name.ext
if ($output eq "$name$ext") { $output = "$name-out$ext"; }

# Remove .ltx or .tex extension
if ($output =~ /.*?$ext/) { $output =~ s/(.+?)$ext/$1/gms;}
} # close  check

### If output name are ok, then $outfile = 1
if (defined($output)) { $outfile = 1; }

### If --srcenv or --subenv option are OK then execute script
if ($srcenv) { $outsrc = 1; $subenv = 0; }
if ($subenv) { $outsrc = 1; $srcenv = 0; }

### Set imgdir name from input file
if (exists $opts_file{options}{imgdir}) {
    $imageDir = $opts_file{options}{imgdir};
}

### Set prefix name from input file
if (exists $opts_file{options}{prefix}) {
    $prefix = $opts_file{options}{prefix};
}

### Set pdfcrop margins from input file
if (exists $opts_file{options}{margins}) {
    $margins = $opts_file{options}{margins};
}

### Set DPI resolution for images from input file
if (exists $opts_file{options}{dpi}) {
    $DPI = $opts_file{options}{dpi};
}

### Create /images dir to save image and source code
-e $imageDir or mkdir($imageDir,0744) or die "Can't create $imageDir: $!\n";

### Options for \pagestyle{empty} ($crop)
my $opt_page = $crop ? "\n\\pagestyle\{empty\}\n\\begin\{document\}"
             :         "\n\\begin\{document\}"
             ;

### Add options to preamble for subfiles
my $sub_prea = "$atbegindoc$preamble$opt_page";

### Delete <*remove> ... </remove> in $sub_prea
$sub_prea =~ s/^\%<\*remove>\s*(.+?)\s*\%<\/remove>(?:[\t ]*(?:\r?\n|\r))?+//gmsx;

### Options for preview packpage
my $opt_prew = $xetex ? 'xetex,'
             : $latex ? ''
             :          'pdftex,'
             ;

### Lines to add at begin input file
my $preview = <<"EXTRA";
\\AtBeginDocument\{%
\\RequirePackage\[${opt_prew}active,tightpage\]\{preview\}%
\\renewcommand\\PreviewBbAdjust\{-60pt -60pt 60pt 60pt\}\}%
EXTRA

### Extract source $outsrc
if ($outsrc) {
my $src_name = "$name-$prefix-";
my $srcNo    = 1;

### Source file whitout preamble for standart environment
if ($srcenv) {
if ($STDenv) { # extract standard environments in individual files
    say "Creating $envNo files with source code for all environments";
    while ($bodydoc =~ m/$BP\s*(?<env_src>.+?)\s*$EP/gms) {
        open my $OUTsrc, '>', "$imageDir/$src_name$srcNo$ext";
            print $OUTsrc $+{env_src};
        close $OUTsrc;
    } # close while
    continue { $srcNo++; } # increment counter
} # close STDenv
if ($PSTexa) { # extract PSTexample environments in individual files
    say "Creating $exaNo files with source code for all PSTexample environments";
    while ($bodydoc =~ m/$BE\[.+?(?<pst_exa_name>$imageDir\/.+?-\d+)\}\]\s*(?<exa_src>.+?)\s*$EE/gms) {
        open my $OUTexa, '>', "$+{'pst_exa_name'}$ext";
            print $OUTexa $+{'exa_src'};
        close $OUTexa;
        }
    } # close PSTexa
} # close srcenv

### Subfile whit preamble
if ($subenv) {
if ($STDenv) { # extract standard environments (with preamble) in individual files
say "Creating a $envNo files whit source code and preamble for all environments";
while ($bodydoc =~ m/(?<=$BP)(?<env_src>.+?)(?=$EP)/gms) { # search $bodydoc
open my $OUTsub, '>', "$imageDir/$src_name$srcNo$ext";
print $OUTsub <<"EOC";
$sub_prea$+{'env_src'}\\end\{document\}
EOC
close $OUTsub;
        } # close while
continue { $srcNo++; }
} # close STDenv
if ($PSTexa) { # extract PSTexample environments (with preamble) in individual files
say "Creating a $exaNo files whit source code and preamble for all PSTexample environments";
while ($bodydoc =~ m/$BE\[.+?(?<pst_exa_name>$imageDir\/.+?-\d+)\}\]\s*(?<exa_src>.+?)\s*$EE/gms) {
    open my $OUTsub, '>', "$+{'pst_exa_name'}$ext";
        print $OUTsub "$sub_prea\n$+{'exa_src'}\n\\end\{document\}";
    close $OUTsub;
                } # close while
            } # close $PSTexa
    } # close subenv
} # close $outsrc

### Create a one file with "all" PSTexample environments extracted
if ($PSTexa) {
say "Creating the temporary file $name-$prefix-exa-$tmp$ext whit $exaNo PSTexample environments extracted";
@exa_extract = undef;
    while ( $bodydoc =~ m/$BE\[.+? $imageDir\/.+?-\d+\}\](?<exa_src>.+?)$EE/gmsx ) { # search $bodydoc
        push @exa_extract, $+{exa_src}."\n\\newpage\n";
        open my $OUTfig, '>', "$name-$prefix-exa-$tmp$ext";
            print $OUTfig "$atbegindoc"."$preamble"."$opt_page"."@exa_extract\n"."\\end\{document\}";
        close $OUTfig;
           }# close while
if (!$run) { # Move and rename tmp-exa-rand file to /image dir
say "Moving the file $name-$prefix-exa-$tmp$ext to /$imageDir/$name-$prefix-exa-all$ext";
move("$workdir/$name-$prefix-exa-$tmp$ext", "$imageDir/$name-$prefix-exa-all$ext");
    }
}# close $PSTexa

### Create a one file whit "all" standard environments extracted
if ($STDenv) {
open my $OUTfig, '>', "$name-$prefix-$tmp$ext";
    if ($noprew) { # $noprew option
    say "Creating the temporary file $name-$prefix-$tmp$ext whit $envNo environments extracted";
        my @env_extract;
        while ( $bodydoc =~ m/(?<=$BP)(?<env_src>.+?)(?=$EP)/gms ) { # search $bodydoc
            push @env_extract, $+{env_src}."\n\\newpage\n";
        } # close while
    print $OUTfig "$atbegindoc"."$preamble"."$opt_page"."@env_extract\n"."\\end{document}";
    } # close $noprew
    else { # default
    say "Creating the temporary file $name-$prefix-$tmp$ext whit $envNo environment extracted using preview package";
    print $OUTfig $atbegindoc.$preview.$preamble."\n".$bodydoc."\n\\end{document}";
         }
close $OUTfig;

### Move tmp-rand file to /image dir
if (!$run) {
say "Moving the file $name-$prefix-$tmp$ext to /$imageDir/$name-$prefix-all$ext";
    move("$workdir/$name-$prefix-$tmp$ext", "$imageDir/$name-$prefix-all$ext");
    }
} # close $STDenv

### Define --shell-escape for TeXLive and MikTeX
my $write18 = '-shell-escape'; # TeXLive
   $write18 = '-enable-write18' if defined($ENV{"TEXSYSTEM"}) and
                          $ENV{"TEXSYSTEM"} =~ /miktex/i;

### Define --interaction=mode for compilers
my $opt_compiler = $verbose ? "$write18 -interaction=nonstopmode -recorder"
                 :            "$write18 -interaction=batchmode -recorder"
                 ;

### Define $silence
my $silence = $verbose ? ''
            :            ">$null"
            ;

### Append -q to gs and poppler for system command line
my $quiet = $verbose ? ''
          :            '-q'
          ;

### Compilers
my $compiler = $xetex  ? "xelatex $opt_compiler"
             : $luatex ? "lualatex $opt_compiler"
             : $latex  ? "latex $opt_compiler"
             : $dvips  ? "latex $opt_compiler"
             : $dvipdf ? "latex $opt_compiler"
             : $arara  ? 'arara'
             :           "pdflatex $opt_compiler"
             ;

### Message in command line for compilers
my $msg_compiler = $xetex  ? 'xelatex'
                 : $luatex ? 'lualatex'
                 : $latex  ? 'latex>dvips>ps2pdf'
                 : $dvips  ? 'latex>dvips>ps2pdf'
                 : $dvipdf ? 'latex>dvipdfmx'
                 : $arara  ? 'arara'
                 :           'pdflatex'
                 ;

### Options for ghostscript in command line
my %opt_gs_dev = (
   pdf  => "$gscmd $quiet -dNOSAFER -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress",
   gray => "$gscmd $quiet -dNOSAFER -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -sColorConversionStrategy=Gray -dProcessColorModel=/DeviceGray",
   png  => "$gscmd $quiet -dNOSAFER -sDEVICE=pngalpha -r$DPI",
   bmp  => "$gscmd $quiet -dNOSAFER -sDEVICE=bmp32b -r$DPI",
   jpg  => "$gscmd $quiet -dNOSAFER -sDEVICE=jpeg -r$DPI -dJPEGQ=100 -dGraphicsAlphaBits=4 -dTextAlphaBits=4",
   tif  => "$gscmd $quiet -dNOSAFER -sDEVICE=tiff32nc -r$DPI",
   );

### Options for poppler-utils in command line
my %opt_poppler = (
   eps => "pdftops $quiet -eps",
   ppm => "pdftoppm $quiet -r $DPI",
   svg => "pdftocairo $quiet -svg",
   );

### Option for pdfcrop in command line
my $opt_crop = $xetex  ? "--xetex  --margins $margins"
             : $luatex ? "--luatex --margins $margins"
             : $latex  ? "--margins $margins"
             :           "--pdftex --margins $margins"
             ;

### Compiler generate file whit all environment extracted
if ($run) {
opendir(my $DIR, $workdir);
while (readdir $DIR) {
### Compiler generate file
if (/(?<name>$name-$prefix(-exa)?)(?<type>-$tmp$ext)/) {
    system("$compiler $+{name}$+{type} $silence");
    say "Compiling the file $+{name}$+{type} using $msg_compiler";

### Compiling file using latex>dvips>ps2pdf
if ($dvips or $latex) {
    system("dvips -q -Ppdf -o $+{name}-$tmp.ps $+{name}-$tmp.dvi");
    system("ps2pdf  -dPDFSETTINGS=/prepress -dAutoRotatePages=/None $+{name}-$tmp.ps  $+{name}-$tmp.pdf");
} # close latex

### Compiling file using latex>dvipdfmx
if ($dvipdf) { system("dvipdfmx -q $+{name}-$tmp.dvi"); }

### If option gray
if ($gray) {
    say "Moving the file $+{name}-$tmp.pdf to $tempDir/$+{name}-all.pdf (gray scale)";
    system("$opt_gs_dev{gray} -o $tempDir/$+{name}-all.pdf $workdir/$+{name}-$tmp.pdf");
    move("$workdir/$+{name}-$tmp.pdf","$tempDir/$+{name}-$tmp.pdf");
     }
else {
    say "Moving the file $+{name}-$tmp.pdf to $tempDir/$+{name}-all.pdf";
    move("$workdir/$+{name}-$tmp.pdf", "$tempDir/$+{name}-all.pdf");
}

### Crop generate file
if ($crop) {
    say "The file $+{name}-all.pdf need a crop, using pdfcrop $opt_crop";
    system("pdfcrop $opt_crop $tempDir/$+{name}-all.pdf $tempDir/$+{name}-all.pdf $silence");
}

### Move tmp-rand.tex file whit all source code for environments to /images dir
move("$workdir/$+{name}$+{type}", "$imageDir/$+{name}-all$ext");
        } # close if m/.../
    } # close while
closedir $DIR;
} # close run

### Append image format options
$opts_cmd{pdf} = 'pdf' if $pdf;                        # ghostscript
$opts_cmd{eps} = 1 if exists $opts_file{options}{eps}; # pdftops
$opts_cmd{ppm} = 1 if exists $opts_file{options}{ppm}; # pdftoppm
$opts_cmd{svg} = 1 if exists $opts_file{options}{svg}; # pdftocairo
$opts_cmd{png} = 1 if exists $opts_file{options}{png}; # ghostscript
$opts_cmd{jpg} = 1 if exists $opts_file{options}{jpg}; # ghostscript
$opts_cmd{bmp} = 1 if exists $opts_file{options}{bmp}; # ghostscript
$opts_cmd{tif} = 1 if exists $opts_file{options}{tif}; # ghostscript

### Suported format
my %format = (%opts_cmd);
my $format = join " ",grep { defined $format{$_} } keys %format;

### Create image formats in separate files
if ($run) {
opendir(my $DIR, $tempDir);
while (readdir $DIR) {
### PDF/PNG/JPG/BMP/TIFF format suported by ghostscript
if (/(?<name>$name-$prefix(-exa)?)(?<type>-all\.pdf)/) {
for my $var (qw(pdf png jpg bmp tif)) {
    if (defined $opts_cmd{$var}) {
    my $ghostcmd = "$opt_gs_dev{$var} -o $workdir/$imageDir/$+{name}-%1d.$var $tempDir/$+{name}$+{type}";
    system("$ghostcmd");
    print "Create a $var image format: runing command $opt_gs_dev{$var} in $+{name}$+{type}\r\n";
        } # close defined for ghostscript
    }# close for
} # close if m/.../

### EPS/PPM/SVG format suported by poppler-utils
if (/(?<name>$name-$prefix)(?<type>-all\.pdf)/) {
for my $var (qw(eps ppm svg)) {
    if (defined $opts_cmd{$var}) {
    for (my $epsNo = 1; $epsNo <= $envNo; $epsNo++) {
my $poppler = "$opt_poppler{$var} -f $epsNo -l $epsNo $tempDir/$+{name}$+{type} $workdir/$imageDir/$+{name}-$epsNo.$var";
system("$poppler");
            } # close for C style
print "Create a $var image format: runing command $opt_poppler{$var} in $+{name}$+{type}\r\n";
        } #  close defined
    } # close for my $var
} # close if m/.../

### EPS/PPM/SVG for pst-exa package
if (/(?<name>$name-$prefix-exa)(?<type>-all\.pdf)/) {
for my $var (qw(eps ppm svg)) {
    if (defined $opts_cmd{$var}) {
    for (my $epsNo = 1; $epsNo <= $exaNo; $epsNo++) {
my $poppler = "$opt_poppler{$var} -f $epsNo -l $epsNo $tempDir/$+{name}$+{type} $workdir/$imageDir/$+{name}-$epsNo.$var";
system("$poppler");
                } # close for C style
print "Create a $var image format: runing command $opt_poppler{$var} in $+{name}$+{type}\r\n";
            } #  close defined
        } # close for my $var
    } # close if m/.../
} # close while
closedir $DIR; #close dir

### Renaming PPM image files
if (defined $opts_cmd{ppm}) {
    opendir(my $DIR, $imageDir);
        while (readdir $DIR) {
            if (/(?<name>$name-fig(-exa)?-\d+\.ppm)(?<sep>-\d+)(?<ppm>\.ppm)/) {
                move("$imageDir/$+{name}$+{sep}$+{ppm}", "$imageDir/$+{name}");
               } # close if m/.../
        } # close while
    closedir $DIR;
   } # close renaming PPM
} # close run

### Create a output file
if ($outfile) {
say "Creating the file $output$ext, convert extracted environments to \\includegraphics";

### Convert extracted environments to \includegraphics
my $grap  =  "\\includegraphics[scale=1]{$name-$prefix-";
my $close =  '}';
my $imgNo =  1;
$bodydoc  =~ s/$BP.+?$EP/$grap@{[$imgNo++]}$close/msg; # changes

### Constant
my $USEPACK     = quotemeta('\usepackage');
my $GRAPHICPATH = quotemeta('\graphicspath{');

### Precompiled regex
my $CORCHETES = qr/\[ [^]]*? \]/x;
my $PALABRAS  = qr/\b (?: graphicx )/x;
my $FAMILIA   = qr/\{ \s* $PALABRAS (?: \s* [,] \s* $PALABRAS )* \s* \}(\%*)?/x;

### Regex to capture \graphicspath
my $graphix = qr/(\\ usepackage \s*\[\s* .+? \s*\] \s*\{\s* graphicx \s*\} )/ix;

### Capture graphix package for future use
my (@graphix) = $preamble =~ m/$graphix/x;

### Remove graphix package
$preamble =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
               ^ $USEPACK (?: $CORCHETES )? $FAMILIA \s*//msxg;

### Comment \graphicspath for order and future use
$preamble =~s /\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
               ^ ($GRAPHICPATH) /%$1/msxg;

### Regex to capture [options] for pst-exa package
my $pstexa = qr/(?:\\ usepackage) \[\s*(.+?)\s*\] (?:\{\s*(pst-exa)\s*\} )   /x;

### Capture [option] for pst-exa package
my (@pst_exa) = $preamble =~ m/$pstexa/xg;

### Search [option] in pst-exa package
my %pst_exa = map { $_ => 1 } @pst_exa;

### Clean file (pst/tags)
if ($clean{pst}) {
$PALABRAS = qr/\b (?: pst-\w+ | pstricks (?: -add )? | psfrag |psgo |vaucanson-g| auto-pst-pdf )/x;
$FAMILIA  = qr/\{ \s* $PALABRAS (?: \s* [,] \s* $PALABRAS )* \s* \}(\%*)?/x;

### Remove pst packages lines
$preamble =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
               ^ $USEPACK (?: $CORCHETES )? $FAMILIA \s*//msxg;

### Delete package words
$preamble =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
(?: ^ $USEPACK \{ | \G) [^}]*? \K (,?) \s* $PALABRAS (\s*) (,?) /$1 and $3 ? ',' : $1 ? $2 : ''/gemsx;

### Delete \psset
$preamble =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
               \\psset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))+//gmsx;

### Delete \SpecialCoor
$preamble =~ s/\%<\*ltximgverw> .+?\%<\/ltximgverw>(*SKIP)(*F)|
               \\SpecialCoor(?:[\t ]*(?:\r?\n|\r))+//gmsx;
} # close clean{pst}

### Delete empty package line \usepackage{}
$preamble =~ s/^\\usepackage\{\}(?:[\t ]*(?:\r?\n|\r))+/\n/gmsx;

### Append graphicx to end of preamble
if (!@graphix == 0) {
$preamble .= <<"EXTRA";
@graphix
EXTRA
    } else {
$preamble .= <<"EXTRA";
\\usepackage{graphicx}
EXTRA
}

### Regex to capture \graphicspath
my $graphicspath= qr/\\ graphicspath \{ ((?: $llaves )+) \}/ix;

### If preamble contain \graphicspath
if ($preamble =~ m/($graphicspath)/m) {
    while ($preamble =~ /$graphicspath /pgmx) {
       my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
       my $encontrado = ${^MATCH};
            if ($encontrado =~ /$graphicspath/) {
            my  $argumento = $1;
            if ($argumento !~ /\{$imageDir\\\}/) {
                $argumento .= "\{$imageDir/\}";
            my  $cambio = "\\graphicspath{$argumento}";
            substr $preamble, $pos_inicial, $pos_final-$pos_inicial, $cambio;
            pos($preamble) = $pos_inicial + length $cambio;
             }
         }
    } #close while

### Regex to capture
my ($GraphicsPath) = $preamble =~ m/($graphicspath)/msx;

### Append \graphicspath to end of preamble
$preamble .= <<"EXTRA";
$GraphicsPath
\\usepackage{grfext}
\\PrependGraphicsExtensions*{.pdf}
EXTRA
     } # close if ($preamble)
else {
### If preamble not contain graphicspath, append to premble
my $GraphicsPath = "\\graphicspath\{\{$imageDir/\}\}";

### Append graphicspath to end of preamble
$preamble .= <<"EXTRA";
$GraphicsPath
\\usepackage{grfext}
\\PrependGraphicsExtensions*{.pdf}
EXTRA
} # close graphicspath

### Suport for \usepackage[swpl]{pst-exa}
if (exists($pst_exa{swpl})) {
$preamble .= <<'EXTRA';
\usepackage[swpl,pdf]{pst-exa}
EXTRA
}

### Suport for \usepackage[tcb]{pst-exa}
if (exists($pst_exa{tcb})) {
$preamble .= <<'EXTRA';
\usepackage[tcb,pdf]{pst-exa}
EXTRA
### Regex
$bodydoc =~ s/(graphic=\{)\[(scale=\d*)\]($imageDir\/$name-$prefix-exa-\d*)\}/$1$2\}\{$3\}/gsmx;
} # close if

### Options for out_file (add $end to outfile)
my $out_file = $clean{doc} ? "$atbegindoc$preamble$bodydoc\n\\end\{document\}"
             :               "$atbegindoc$preamble$bodydoc\n$enddoc"
             ;

### Clean \psset content in output file
if ($clean{pst}) {
$out_file =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \%<\*ltximgverw> .+? \%<\/ltximgverw>(*SKIP)(*F)|
               \\psset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
} # close clean

### Clean \tikzset content in output file
if ($clean{tkz}) {
$out_file =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \%<\*ltximgverw> .+? \%<\/ltximgverw>(*SKIP)(*F)|
               \\tikzset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
} # close clean

### Revert changues in all words in outfile
$out_file =~ s/\\begin\{nopreview\}\s*(.+?)\s*\\end\{nopreview\}/$1/gmsx;
$out_file =~ s/\%<\*ltximgverw>\s*(.+?)\s*\%<\/ltximgverw>/$1/gmsx;
$out_file =~ s/\%<\*noltximg>\n(.+?)\n\%<\/noltximg>/$1/gmsx;
$out_file =~ s/^\%<\*remove>\s*(.+?)\s*\%<\/remove>(?:[\t ]*(?:\r?\n|\r))+//gmsx;
$out_file =~ s/($delt_env)(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
$out_file =~ s/($find)/$replace{$1}/g;

### Write output file
open my $OUTfile, '>', "$output$ext";
    print $OUTfile "$out_file";
close $OUTfile;

### Process the output file
if ($run) {
### Set correct $compiler, if input use  latex then output use pdflatex
$compiler     = "pdflatex $opt_compiler" if $latex;
$msg_compiler = "pdflatex" if $latex;

### Compiling output file using pdflatex or arara
say "Compiling file $output$ext using $msg_compiler";
system("$compiler $output$ext $silence");

### Compiling output file using latex>dvips>ps2pdf
if ($dvips) {
    system("dvips -q -Ppdf $output.dvi");
    system("ps2pdf  -dPDFSETTINGS=/prepress -dAutoRotatePages=/None $output.ps  $output.pdf");
} # close dvips

### Compiling output file using latex>dvipdfmx
if ($dvipdf) { system("dvipdfmx -q $output.dvi"); }
      } # close run
} # close outfile file

### Remove temporary files
if ($run) {
say "Deleting (most) temporary files created during the process";
my @protected = qw();
push (@protected,"$output$ext","$output.pdf") if defined $output;

my $flsline = "OUTPUT";
my @flsfile;

if ($PSTexa) { push @flsfile,"$name-$prefix-exa-$tmp.fls"; }
if ($STDenv) { push @flsfile,"$name-$prefix-$tmp.fls"; }
push (@flsfile,"$output.fls") if defined $output;

my @tmpfiles;
for my $filename(@flsfile){
    open my $RECtmp, '<', "$filename";
        push @tmpfiles, grep /^$flsline/,<$RECtmp>;
    close $RECtmp;
}

foreach (@tmpfiles) { s/^$flsline\s+|\s+$//g; }

if ($latex or $dvips) { push @tmpfiles,"$name-$prefix-$tmp.ps"; }
if (-e "$name-$prefix-$tmp.ps") { push @tmpfiles,"$name-$prefix-$tmp.ps"; }
if ($PSTexa) { push @tmpfiles,"$name-$prefix-exa-$tmp.ps"; }
if (-e "$output.ps") { push @tmpfiles,"$output.ps"; }
push @tmpfiles,@flsfile,"$name-$prefix-$tmp$ext","$name-$prefix-$tmp.pdf";

my @delfiles = array_minus(@tmpfiles, @protected);
foreach my $tmpfile (@delfiles) { move("$tmpfile", "$tempDir"); }
} # close clean tmp files

### Compress images dir whit generated files
if ($zip or $tar) {
my $stamp = strftime "%F", localtime;
my $archivetar = "$imageDir-$stamp";

my @savetozt;
find(\&zip_tar, $imageDir);
sub zip_tar{
    my $filesto = $_;
    if (-f $filesto && $filesto =~ m/$name-$prefix-.+?$/) { # buscamos
        push @savetozt, $File::Find::name;
    }
}
# Write compressed zip file
if ($zip) {
    say "Creating a file $workdir/$archivetar.zip";
    zip \@savetozt => "$archivetar.zip";
    }

# Write compressed tar.gz file
if ($tar) {
    say "Creating a file $workdir/$archivetar.tar.gz";
    my $imgdirtar = Archive::Tar->new();
    $imgdirtar->add_files( @savetozt );
    $imgdirtar->write( "$archivetar.tar.gz" , 9 );
    }
} #close compress

### End of script process
if ($run) { say "Finish, image formats: $format are in $workdir/$imageDir/"; }
     else { say "Done"; }

__END__
