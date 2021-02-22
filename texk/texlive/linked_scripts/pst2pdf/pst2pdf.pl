#!/usr/bin/env perl
use v5.26;

############################# LICENCE ##################################
# v0.20  2020-08-24 simplify the use of PSTricks with pdf              #
# (c) Herbert Voss <hvoss@tug.org>                                     #
#     Pablo González L <pablgonz@yahoo.com>                            #
#                                                                      #
# This program is free software; you can redistribute it and/or modify #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation; either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# This program is distributed in the hope that it will be useful, but  #
# WITHOUT ANY WARRANTY; without even the implied warranty of           #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    #
# General Public License for more details.                             #
########################################################################

use Getopt::Long qw(:config bundling_override no_ignore_case); # require_order
use File::Spec::Functions qw(catfile devnull);
use File::Basename;
use Archive::Tar;
use Data::Dumper;
use FileHandle;
use IO::Compress::Zip qw(:all);
use File::Path qw(remove_tree);
use File::Temp qw(tempdir);
use POSIX qw(strftime);
use File::Copy;
use File::Find;
use Env qw(PATH);
use autodie;
use Config;
use Cwd;
use if $^O eq 'MSWin32', 'Win32';
use if $^O eq 'MSWin32', 'Win32::Console::ANSI'; # need for color :)
use Term::ANSIColor;

### Internal vars
my $tempDir = tempdir( CLEANUP => 1); # temporary directory
my $workdir = cwd;      # current working dir
my $imgdir  = 'images'; # where to save the images
my $margins = '1';      # margins in pdfcrop
my $clear   = 0;        # 0 or 1, clears all temporary files
my $dpi     = '300';    # very low value for the png's
my $runs    = '1';      # set runs for compliler
my $nocorp  = 0;        # 1->no crop pdf files created
my $norun   = 0;        # 1->no create images
my $noprew  = 0;        # 1->create images in nopreview mode
my $runbibtex = 0;      # 1->runs bibtex
my $runbiber  = 0;      # 1->runs biber and sets $runbibtex=0
my $xetex  = 0;         # 1->Using (Xe)LaTeX for compilation.
my $luatex = 0;         # 1->Using dvilualatex for compilation.
my $latexmk = 0;        # 1->Using latexmk for compiler output file.
my $arara;              # 1->Using arara for compiler output file.
my $nosource = 0;       # Delete TeX source for images
my $srcenv;             # write only source code of environments
my $nopdf;              # Don't create a pdf image files
my $force;              # try to capture \psset
my $nocrop;             # Don't crop image files
my $zip;                # compress files generated in .zip
my $tar;                # compress files generated in .tar.gz
my $help;               # help info
my $version;            # version info
my $shell;              # enable -shell-escape
my $verbose = 0;        # verbose info
my @verb_env_tmp;       # store verbatim environments
my $tmpverbenv;         # save verbatim environment
my $myverb = 'myverb' ; # internal \myverb macro
my $write18;            # storing write18 for compiler in TeXLive and MikTeX
my $gscmd;              # ghostscript executable name
my $gray;               # gray scale ghostscript
my $log      = 0;       # log file
my $PSTexa   = 0;       # run extract PSTexample environments
my $STDenv   = 0;       # run extract pspicture/psfrag environments
my @currentopt;         # storing current options for log file
my %opts_cmd;           # hash to store options for Getopt::Long  and log

### Script identification
my $scriptname = 'pst2pdf';
my $nv         = 'v0.20';
my $date       = '2020-08-22';
my $ident      = '$Id: pst2pdf.pl 119 2020-08-22 12:04:09Z herbert $';

### Log vars
my $LogFile = "$scriptname.log";
my $LogWrite;
my $LogTime = strftime("%y/%m/%d %H:%M:%S", localtime);

### Error in command line
sub errorUsage {
    my $msg = shift;
    die color('red').'* Error!!: '.color('reset').$msg.
    " (run $scriptname --help for more information)\n";
    return;
}

### Extended error messages
sub exterr {
    chomp(my $msg_errno = $!);
    chomp(my $msg_extended_os_error = $^E);
    if ($msg_errno eq $msg_extended_os_error) {
        $msg_errno;
    }
    else {
        "$msg_errno/$msg_extended_os_error";
    }
}

### Funtion uniq
sub uniq {
    my %seen;
    return grep !$seen{$_}++, @_;
}

### Funtion array_minus
sub array_minus(\@\@) {
    my %e = map{ $_ => undef } @{$_[1]};
    return grep !exists $e{$_}, @{$_[0]};
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

### Print colored info in screen
sub Infocolor {
    my $type = shift;
    my $info = shift;
    if ($type eq 'Running') {
        print color('cyan'), '* ', color('reset'), color('green'),
        "$type: ", color('reset'), color('cyan'), "$info\r\n", color('reset');
    }
    if ($type eq 'Warning') {
        print color('bold red'), "* $type: ", color('reset'),
        color('yellow'), "$info\r\n", color('reset');
    }
    if ($type eq 'Finish') {
        print color('cyan'), '* ', color('reset'), color('magenta'),
        "$type!: ", color('reset'),  color('green'), "$info\r\n",color('reset');
    }
    return;
}

### Write Log line and print msg (common)
sub Infoline {
    my $msg = shift;
    my $now = strftime("%y/%m/%d %H:%M:%S", localtime);
    if ($log) { $LogWrite->print(sprintf "[%s] * %s\n", $now, $msg); }
    say $msg;
    return;
}

### Write Log line (no print msg and time stamp)
sub Logline {
    my $msg = shift;
    if ($log) { $LogWrite->print("$msg\n"); }
    return;
}

### Write Log line (time stamp)
sub Log {
    my $msg = shift;
    my $now = strftime("%y/%m/%d %H:%M:%S", localtime);
    if ($log) { $LogWrite->print(sprintf "[%s] * %s\n", $now, $msg); }
    return;
}

### Write array env in Log
sub Logarray {
    my ($env_ref) = @_;
    my @env_tmp = @{ $env_ref }; # dereferencing and copying each array
    if ($log) {
        if (@env_tmp) {
            my $tmp  = join "\n", map { qq/* $_/ } @env_tmp;
            print {$LogWrite} "$tmp\n";
        }
        else {
            print {$LogWrite} "Not found\n";
        }
    }
    return;
}

### Extended print info for execute system commands using $ command
sub Logrun {
    my $msg = shift;
    my $now = strftime("%y/%m/%d %H:%M:%S", localtime);
    if ($log) { $LogWrite->print(sprintf "[%s] \$ %s\n", $now, $msg); }
    return;
}

### Capture and execute system commands
sub RUNOSCMD {
    my $cmdname  = shift;
    my $argcmd   = shift;
    my $showmsg  = shift;
    my $captured = "$cmdname $argcmd";
    Logrun($captured);
    if ($showmsg eq 'show') {
        if ($verbose) {
            Infocolor('Running', $captured);
        }
        else{ Infocolor('Running', $cmdname); }
    }
    if ($showmsg eq 'only' and $verbose) {
        Infocolor('Running', $captured);
    }
    # Run system system command
    $captured = qx{$captured};
    if ($log) { $LogWrite->print($captured); }
    if ($? == -1) {
        my $errorlog    = "* Error!!: ".$cmdname." failed to execute (%s)!\n";
        my $errorprint  = "* Error!!: ".color('reset').$cmdname." failed to execute (%s)!\n";
        if ($log) { $LogWrite->print(sprintf $errorlog, exterr); }
        print STDERR color('red');
        die sprintf $errorprint, exterr;
    } elsif ($? & 127) {
        my $errorlog   = "* Error!!: ".$cmdname." died with signal %d!\n";
        my $errorprint = "* Error!!: ".color('reset').$cmdname." died with signal %d!\n";
        if ($log) { $LogWrite->print(sprintf $errorlog, ($? & 127)); }
        print STDERR color('red');
        die sprintf $errorprint, ($? & 127);
    } elsif ($? != 0 ) {
        my $errorlog = "* Error!!: ".$cmdname." exited with error code %d!\n";
        my $errorprint  = "* Error!!: ".color('reset').$cmdname." exited with error code %d!\n";
        if ($log) { $LogWrite->print(sprintf $errorlog, $? >> 8); }
        print STDERR color('red');
        die sprintf $errorprint, $? >> 8;
    }
    if ($verbose) { print $captured; }
    return;
}

### General information
my $copyright = <<"END_COPYRIGHT" ;
[$date] (c) Herbert Voss <hvoss\@tug.org> and Pablo González L <pablgonz\@yahoo.com>.
END_COPYRIGHT

my $versiontxt= <<"END_VERSION" ;
${scriptname} ${nv} ${ident}
${copyright}
END_VERSION

### Standart info in terminal
my $title = "$scriptname $nv - Running a PSTricks document with (pdf/xe/lua)latex [HV \& PG]\n";

### Usage of script
sub usage {
find_ghostscript();

my $usage = <<"END_OF_USAGE";
${title}
   pst2pdf is a Perl script which isolates all PostScript or PSTricks related
   parts of the TeX document, read all postscript, pspicture, psgraph and
   PSTexample environments, extract source code in standalone files and
   converting them into image format. Create new file with all extracted
   environments converted to \\includegraphics and runs (pdf/xe/lua)latex.

Usage: $scriptname [<options>] <texfile>[.tex|.ltx]
       $scriptname <texfile>[.tex|.ltx] [options]

   If used without [<options>] the extracted environments are saved in
   standalone files and converted to pdf image format in the "./images"
   directory using "latex>dvips>ps2pdf" and "preview" package to process
   <texfile> and "pdflatex" for compiler <texfile-pdf>.

Options:
                                                                 [default]
  -l, --log          Write .log file with debug information      [off]
  -h, --help         Display command line help and exit          [off]
  -v, --version      Display current version ($nv) and exit    [off]
  -V, --verbose      Verbose printing information                [off]
  -t, --tif          Create .tif files using ghostscript         [$gscmd]
  -b, --bmp          Create .bmp files using ghostscript         [$gscmd]
  -j, --jpg          Create .jpg files using ghostscript         [$gscmd]
  -p, --png          Create .png files using ghostscript         [$gscmd]
  -e, --eps          Create .eps files using poppler-utils       [pdftops]
  -s, --svg          Create .svg files using poppler-utils       [pdftocairo]
  -P, --ppm          Create .ppm files using poppler-utils       [pdftoppm]
  -g, --gray         Gray scale for images using ghostscript     [off]
  -f, --force        Try to capture \\psset{...} to extract       [off]
  -x, --xetex        Using xelatex for compiler input and output [off]
  -ns,--nosource     Do not create standalone files              [off]
  -np, --noprew, --single
                     Create images files without preview package [off]
  -ni,--norun, --noimages
                     Generate file-pdf.tex, but not images       [off]
  -d <integer>, --dpi <integer>
                     Dots per inch resolution for images         [150]
  -r <integer>, --runs <integer>
                     Set the number of times the compiler will
                     run on the input file for extraction        [1]
  -m <integer>, --margins <integer>
                     Set margins in bp for pdfcrop               [0]
  --myverb <macro>   Add "\\macroname" to verbatim inline search  [myverb]
  --ignore <environment>
                     Add "environment" to verbatim environments  [empty]
  --srcenv           Create files with only code environment     [off]
  --shell            Enable \\write18\{SHELL COMMAND\}              [off]
  --nopdf            Do not create images in pdf format          [off]
  --nocrop           Does not run pdfcrop                        [off]
  --imgdir <dirname> Set name of directory to save images/files  [images]
  --luatex           Using dvilualatex>dvips>ps2pdf for compiler
                     input and lualatex for compiler output file [off]
  --arara            Use arara for compiler output file          [off]
  --latexmk          Using latexmk for compiler output file      [off]
  --zip              Compress generated files in .zip format     [off]
  --tar              Compress generated files in .tar.gz format  [off]
  --bibtex           Run bibtex on the .aux file (if exists)     [off]
  --biber            Run biber on the .bcf file (if exists)      [off]

Example:
\$ $scriptname -e -p --imgdir pics test.tex
* Create a ./pics directory (if it doesn't exist) with all extracted
* environments converted to individual files (.pdf, .eps, .png, .tex),
* a file test-fig-all.tex with all extracted environments and the file
* test-pdf.tex with all environments converted to \\includegraphics using
* latex>dvips>ps2pdf and preview package for <test.tex> and pdflatex
* for <test-pdf.tex>.

See texdoc pst2pdf for full documentation.
END_OF_USAGE
print $usage;
exit 0;
}

### Options in terminal
my $result=GetOptions (
    'h|help'             => \$help,     # flag
    'v|version'          => \$version,  # flag
    'l|log'              => \$log,      # flag
    'f|force'            => \$force,    # flag
    'd|dpi=i'            => \$dpi,      # numeric
    'runs=i'             => \$runs,     # numeric
    'm|margins=i'        => \$margins,  # numeric
    'imgdir=s'           => \$imgdir,   # string
    'myverb=s'           => \$myverb,   # string
    'ignore=s'           => \$tmpverbenv, # string
    'c|clear'            => \$clear,    # flag (unused)
    'ni|norun|noimages'  => \$norun,    # flag
    'np|noprew|single'   => \$noprew,   # flag
    'bibtex'             => \$runbibtex,# flag
    'biber'              => \$runbiber, # flag
    'arara'              => \$arara,    # flag
    'latexmk'            => \$latexmk,  # flag
    'srcenv'             => \$srcenv,   # flag
    'nopdf'              => \$nopdf,    # flag
    'shell'              => \$shell,    # flag
    'nocrop'             => \$nocrop,   # flag
    'zip'                => \$zip,      # flag
    'tar'                => \$tar,      # flag
    'g|gray'             => \$gray,     # flag
    'b|bmp'              => \$opts_cmd{image}{bmp}, # gs
    't|tif'              => \$opts_cmd{image}{tif}, # gs
    'j|jpg'              => \$opts_cmd{image}{jpg}, # gs
    'p|png'              => \$opts_cmd{image}{png}, # gs
    's|svg'              => \$opts_cmd{image}{svg}, # pdftocairo
    'e|eps'              => \$opts_cmd{image}{eps}, # pdftops
    'P|ppm'              => \$opts_cmd{image}{ppm}, # pdftoppm
    'x|xetex'            => \$xetex,    # flag
    'luatex'             => \$luatex,   # flag
    'ns|nosource'        => \$nosource, # flag
    'V|Verbose'          => \$verbose,  # flag
) or do { $log = 0 ; die usage(0); };

### Open pst2pdf.log file
if ($log) {
    if (!defined $ARGV[0]) { errorUsage('Input filename missing'); }
    my $tempname = $ARGV[0];
    $tempname =~ s/\.(tex|ltx)$//;
    if ($LogFile eq "$tempname.log") { $LogFile = "$scriptname-log.log"; }
    $LogWrite  = FileHandle->new("> $LogFile");
}

### Init pst2pdf.log file
Log("The script $scriptname $nv was started in $workdir");
Log("Creating the temporary directory $tempDir");

### Check --arara and --latexmk
if ($arara && $latexmk) {
    Log('Error!!: Options --arara and --latexmk  are mutually exclusive');
    errorUsage('Options --arara and --latexmk  are mutually exclusive');
}

### Check --biber and --bibtex
if ($runbiber && $runbibtex) {
    Log('Error!!: Options --biber and --bibtex  are mutually exclusive');
    errorUsage('Options --biber and --bibtex  are mutually exclusive');
}

### Check --runs
if( $runs <= 0 or $runs >= 3) {
    Log('Error!!: Invalid argument for --runs, argument out of range');
    errorUsage('Invalid argument for --runs option');
}

### Check --dpi
if( $dpi <= 0 or $dpi >= 2500) {
    Log('Error!!: Invalid argument for --dpi, argument out of range');
    errorUsage('Invalid argument for --dpi option');
}

### Validate --myverb
if ($myverb =~ /^(?:\\|\-).+?/) {
    Log('Error!!: Invalid argument for --myverb option, argument begin with \ or -');
    errorUsage('Invalid argument for --myverb');
}

### Validate --ignore
if ($tmpverbenv =~ /^(?:\\|\-).+?/) {
    Log('Error!!: Invalid argument for --ignore option, argument begin with \ or -');
    errorUsage('Invalid argument for --ignore');
}

### If $tmpverbenv is valid pass to @verb_env_tmp
if ($tmpverbenv) { @verb_env_tmp = $tmpverbenv; }

### Make ENV safer, see perldoc perlsec
delete @ENV{qw(IFS CDPATH ENV BASH_ENV)};

### The next code it's part of pdfcrop (adapted from TexLive 2014)
# Windows detection
my $Win = 0;
if ($^O =~ /mswin32/i) { $Win = 1; }

my $archname = $Config{'archname'};
$archname = 'unknown' unless defined $Config{'archname'};

# Get ghostscript command name
sub find_ghostscript () {
    if ($log) {
        Log('General information about the Perl instalation and operating system');
        print {$LogWrite} "* Perl executable: $^X\n";
        if ($] < 5.006) {
            print {$LogWrite} "* Perl version: $]\n";
        }
        else {
            printf {$LogWrite} "* Perl version: v%vd\n", $^V;
        }
        if (defined &ActivePerl::BUILD) {
            printf {$LogWrite} "* Perl product: ActivePerl, build %s\n", ActivePerl::BUILD();
        }
        printf {$LogWrite} "* Pointer size: $Config{'ptrsize'}\n";
        printf {$LogWrite} "* Pipe support: %s\n",
                (defined $Config{'d_pipe'} ? 'yes' : 'no');
        printf {$LogWrite} "* Fork support: %s\n",
                (defined $Config{'d_fork'} ? 'yes' : 'no');
    }
    my $system = 'unix';
    $system = 'win' if $^O =~ /mswin32/i;
    $system = 'msys' if $^O =~ /msys/i;
    $system = 'cygwin' if $^O =~ /cygwin/i;
    $system = 'miktex' if defined $ENV{'TEXSYSTEM'} and
                          $ENV{'TEXSYSTEM'} =~ /miktex/i;
    if ($log) {
        print {$LogWrite} "* OS name: $^O\n";
        print {$LogWrite} "* Arch name: $archname\n";
        if ($^O eq 'MSWin32') {
            my $tmp = Win32::GetOSName();
            print {$LogWrite} "* System: $tmp\n";
        }
        else { print {$LogWrite} "* System: $system\n"; }
    }
    Log('Trying to locate the executable for Ghostscript');
    my %candidates = (
        'unix'   => [qw|gs|],
        'win'    => [qw|gswin32c|],
        'msys'   => [qw|gswin64c gswin32c|],
        'cygwin' => [qw|gs|],
        'miktex' => [qw|mgs gswin32c|],
    );
    if ($system eq 'win' or $system eq 'miktex') {
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
        'unix'   => q{},
        'win'    => '.exe',
        'msys'   => '.exe',
        'cygwin' => '.exe',
        'miktex' => '.exe',
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
                if ($log) { print {$LogWrite} "* Found ($candidate): $file\n"; }
                last;
            }
            if ($log) { print {$LogWrite} "* Not found ($candidate): $file\n"; }
        }
        last if $found;
    }
    if (not $found and $Win and $system ne 'msys') {
        $found = SearchRegistry();
    }
    if (not $found and $system eq 'msys') {
        $found = Searchbyregquery();
    }
    if ($found) {
        if ($log) { print {$LogWrite} "* Autodetected ghostscript command: $gscmd\n"; }
    }
    else {
        $gscmd = $$candidates_ref[0];
        if ($log) { print {$LogWrite} "* Default ghostscript command: $gscmd\n"; }
    }
}

sub SearchRegistry {
    my $found = 0;
    # The module Win32::TieRegistry not aviable in cygwin/msys
    eval 'use Win32::TieRegistry qw|KEY_READ REG_SZ|';
    if ($@) {
        if ($log) {
            print {$LogWrite} "* Registry lookup for Ghostscript failed:\n";
            my $msg = $@;
            $msg =~ s/\s+$//;
            foreach (split /\r?\n/, $msg) {
                print {$LogWrite} " $_\n";
            }
        }
        return $found;
    }
    my $open_params = {Access => KEY_READ(), Delimiter => q{/}};
    my $key_name_software = 'HKEY_LOCAL_MACHINE/SOFTWARE/';
    my $current_key = $key_name_software;
    my $software = new Win32::TieRegistry $current_key, $open_params;
    if (not $software) {
        if ($log) {
            print {$LogWrite} "* Cannot find or access registry key `$current_key'!\n";
        }
        return $found;
    }
    if ($log) { print {$LogWrite} "* Search registry at `$current_key'.\n"; }
    my %list;
    foreach my $key_name_gs (grep /Ghostscript/i, $software->SubKeyNames()) {
        $current_key = "$key_name_software$key_name_gs/";
        if ($log) { print {$LogWrite} "* Registry entry found: $current_key\n"; }
        my $key_gs = $software->Open($key_name_gs, $open_params);
        if (not $key_gs) {
            if ($log) { print {$LogWrite} "* Cannot open registry key `$current_key'!\n"; }
            next;
        }
        foreach my $key_name_version ($key_gs->SubKeyNames()) {
            $current_key = "$key_name_software$key_name_gs/$key_name_version/";
            if ($log) { print {$LogWrite} "* Registry entry found: $current_key\n"; }
            if (not $key_name_version =~ /^(\d+)\.(\d+)$/) {
                if ($log) { print {$LogWrite} "  The sub key is not a version number!\n"; }
                next;
            }
            my $version_main = $1;
            my $version_sub = $2;
            $current_key = "$key_name_software$key_name_gs/$key_name_version/";
            my $key_version = $key_gs->Open($key_name_version, $open_params);
            if (not $key_version) {
                if ($log) { print {$LogWrite} "* Cannot open registry key `$current_key'!\n"; }
                next;
            }
            $key_version->FixSzNulls(1);
            my ($value, $type) = $key_version->GetValue('GS_DLL');
            if ($value and $type == REG_SZ()) {
                if ($log) { print {$LogWrite} "  GS_DLL = $value\n"; }
                $value =~ s|([\\/])([^\\/]+\.dll)$|$1gswin32c.exe|i;
                my $value64 = $value;
                $value64 =~ s/gswin32c\.exe$/gswin64c.exe/;
                if ($archname =~ /mswin32-x64/i and -f $value64) {
                    $value = $value64;
                }
                if (-f $value) {
                    if ($log) { print {$LogWrite} "EXE found: $value\n"; }
                }
                else {
                    if ($log) { print {$LogWrite} "EXE not found!\n"; }
                    next;
                }
                my $sortkey = sprintf '%02d.%03d %s',
                        $version_main, $version_sub, $key_name_gs;
                $list{$sortkey} = $value;
            }
            else {
                if ($log) { print {$LogWrite} "Missing key `GS_DLL' with type `REG_SZ'!\n"; }
            }
        }
    }
    foreach my $entry (reverse sort keys %list) {
        $gscmd = $list{$entry};
        if ($log) { print {$LogWrite} "* Found (via registry): $gscmd\n"; }
        $found = 1;
        last;
    }
    return $found;
} # end GS search registry

### This part is only necessary if you're using Git on windows and don't
### have gs configured in PATH. Git for windows don't have a Win32::TieRegistry
### module for perl (is not supported in the current versions of msys).
sub Searchbyregquery {
    my $found = 0;
    my $gs_regkey;
    my $opt_reg = '//s //v';
    if ($log) { print {$LogWrite} "* Search Ghostscript in Windows registry under mingw/msys:\n";}
    $gs_regkey = qx{reg query "HKLM\\Software\\GPL Ghostscript" $opt_reg GS_DLL};
    if ($? == 0) {
        if ($log) { print {$LogWrite} "* Registry entry found for GS_DLL (64 bits version)\n";}
    }
    else {
        $gs_regkey = qx{reg query "HKLM\\Software\\Wow6432Node\\GPL Ghostscript" $opt_reg GS_DLL};
        if ($? == 0) {
            if ($log) { print {$LogWrite} "* Registry entry found for GS_DLL (32 bits version)\n";}
        }
    }
    my ($gs_find) = $gs_regkey =~ m/(?:\s* GS_DLL \s* REG_SZ \s*) (.+?)(?:\.dll.+?\R)/s;
    if ($gs_find) {
        my ($gs_vol, $gs_path, $gs_ver) = $gs_find =~ m/
                                                        (\w{1})(?:\:)   # volumen
                                                        (.+?)           # path to executable
                                                        (?:\\gsdll)     # LIB
                                                        (\d{2})         # Version
                                                      /xs;
        # Adjust
        $gs_vol = lc($gs_vol);
        $gs_path = '/'.$gs_vol.$gs_path;
        $gs_path =~ s|\\|/|gmsxi;
        # Add to PATH
        if ($log) { print {$LogWrite} "* Add $gs_path to PATH for current session\n"; }
        $PATH .= ":$gs_path";
        # Set executable
        $gscmd = 'gswin'.$gs_ver.'c';
        if ($log) { print {$LogWrite} "* Found (via reg query): $gscmd\n"; }
        $found = 1;
    }
    if ($@) {
        if ($log) {
            print {$LogWrite} "* Registry lookup for Ghostscript by reg query failed:\n";
            my $msg = $@;
            $msg =~ s/\s+$//;
            foreach (split /\r?\n/, $msg) {
                print {$LogWrite} " $_\n";
            }
        }
        return $found;
    }
    return $found;
}

### Call GS
find_ghostscript();

### Windows need suport space in path
if ($Win and $gscmd =~ /\s/) { $gscmd = "\"$gscmd\"";}

### Help
if (defined $help) {
    usage(1);
    exit 0;
}

### Version
if (defined $version) {
    print $versiontxt;
    exit 0;
}

### Set temp internal vars for <name-fig-tmp> and extraction
my $tmp = int(rand(10000));
my $dtxverb = "verbatim$tmp";
my $wrapping = "$scriptname$tmp"; # wraped for environment extraction

Log("Set up the environment [$wrapping] to encapsulate the extraction");

### Set vars for match/regex
my $BP = "\\\\begin\{$wrapping\}";
my $EP = "\\\\end\{$wrapping\}";
my $BE = '\\\\begin\{PSTexample\}';
my $EE = '\\\\end\{PSTexample\}';

### Define key = pdf for image format
if (!$nopdf) {
    $opts_cmd{image}{pdf} = 1;
}

### Store image formats in hash
my $format = join q{, },grep { defined $opts_cmd{image}{$_} } keys %{$opts_cmd{image}};
if (!$norun) {
    Log("Defined image formats for creating: $format");
}

### Check <input file> from command line
@ARGV > 0 or errorUsage('Input filename missing');
@ARGV < 2 or errorUsage('Unknown option or too many input files');

### Check <input file> extention
my @SuffixList = ('.tex', '', '.ltx'); # posibles
my ($name, $path, $ext) = fileparse($ARGV[0], @SuffixList);
$ext = '.tex' if not $ext;

### Read <input file> in memory
Log("Read input file $name$ext in memory");
open my $inputfile, '<:crlf', "$name$ext";
    my $ltxfile;
        {
            local $/;
            $ltxfile = <$inputfile>;
        }
close $inputfile;

### Identification message in terminal
print $title;

### Default environment to extract
my @extractenv = qw (
    postscript pspicture psgraph PSTexample
    );

### Default verbatim environment
my @iverb_env = qw (
    Example CenterExample SideBySideExample PCenterExample PSideBySideExample
    verbatim Verbatim BVerbatim LVerbatim SaveVerbatim PSTcode
    LTXexample tcblisting spverbatim minted listing lstlisting
    alltt comment chklisting verbatimtab listingcont boxedverbatim
    demo sourcecode xcomment pygmented pyglist program programl
    programL programs programf programsc programt
    );
push @verb_env_tmp, @iverb_env;

### Default verbatim write environment
my @verw_env_tmp;
my @iverbw_env = qw (
    scontents filecontents tcboutputlisting tcbexternal tcbwritetmp extcolorbox extikzpicture
    VerbatimOut verbatimwrite filecontentsdef filecontentshere filecontentsdefmacro
    filecontentsdefstarred filecontentsgdef filecontentsdefmacro filecontentsgdefmacro
    );
push @verw_env_tmp, @iverbw_env;

########################################################################
# One problem that can arise is the filecontents environment, this can #
# contain a complete document and be anywhere, before dividing we will #
# make some replacements for this and comment lines                    #
########################################################################

### Create a Regex for verbatim write environment
@verw_env_tmp = uniq(@verw_env_tmp);
my $tmpverbw = join q{|}, map { quotemeta } sort { length $a <=> length $b } @verw_env_tmp;
$tmpverbw = qr/$tmpverbw/x;
my $tmp_verbw = qr {
                     (
                       (?:
                         \\begin\{$tmpverbw\*?\}
                           (?:
                             (?>[^\\]+)|
                             \\
                             (?!begin\{$tmpverbw\*?\})
                             (?!end\{$tmpverbw\*?\})|
                             (?-1)
                           )*
                         \\end\{$tmpverbw\*?\}
                       )
                     )
                   }x;

### A pre-regex for comment lines
my $tmpcomment = qr/^ \s* \%+ .+? $ /mx;

### Hash for replace in verbatim's and comment lines
my %document = (
    '\begin{document}' => '\BEGIN{document}',
    '\end{document}'   => '\END{document}',
    '\documentclass'   => '\DOCUMENTCLASS',
    '\pagestyle{'      => '\PAGESTYLE{',
    '\thispagestyle{'  => '\THISPAGESTYLE{',
    );

### Changes in input file for verbatim write and comment lines
while ($ltxfile =~ / $tmp_verbw | $tmpcomment /pgmx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my  $encontrado = ${^MATCH};
        while (my($busco, $cambio) = each %document) {
            $encontrado =~ s/\Q$busco\E/$cambio/g;
        }
        substr $ltxfile, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($ltxfile) = $pos_inicial + length $encontrado;
}

### Now, split <input file>
my ($atbegindoc, $document) = $ltxfile =~ m/\A (\s* .*? \s*) (\\documentclass.*)\z/msx;

### Rules to capture for regex my $CORCHETES = qr/\[ [^]]*? \]/x;
my $braces      = qr/ (?:\{)(.+?)(?:\}) /msx;
my $braquet     = qr/ (?:\[)(.+?)(?:\]) /msx;
my $no_corchete = qr/ (?:\[ .*? \])?    /msx;

### Array for capture new verbatim environments defined in input file
my @cverb_env = qw (
    newtcblisting DeclareTCBListing ProvideTCBListing NewTCBListing
    lstnewenvironment NewListingEnvironment NewProgram specialcomment
    includecomment DefineVerbatimEnvironment newverbatim newtabverbatim
    );

### Regex to capture names for new verbatim environments from input file
my $cverbenv = join q{|}, map { quotemeta} sort { length $a <=> length $b } @cverb_env;
$cverbenv = qr/\b(?:$cverbenv) $no_corchete $braces/msx;

### Array for capture new verbatim write environments defined in input file
my @cverb_env_w = qw (
    renewtcbexternalizetcolorbox renewtcbexternalizeenvironment
    newtcbexternalizeenvironment newtcbexternalizetcolorbox newenvsc
    );

### Regex to capture names for new verbatim write environments from input file
my $cverbenvw = join q{|}, map { quotemeta} sort { length $a <=> length $b } @cverb_env_w;
$cverbenvw = qr/\b(?:$cverbenvw) $no_corchete $braces/msx;

### Regex to capture MINTED related environments
my $mintdenv  = qr/\\ newminted $braces (?:\{.+?\})      /x;
my $mintcenv  = qr/\\ newminted $braquet (?:\{.+?\})     /x;
my $mintdshrt = qr/\\ newmint $braces (?:\{.+?\})        /x;
my $mintcshrt = qr/\\ newmint $braquet (?:\{.+?\})       /x;
my $mintdline = qr/\\ newmintinline $braces (?:\{.+?\})  /x;
my $mintcline = qr/\\ newmintinline $braquet (?:\{.+?\}) /x;

### Filter input file, now $ltxfile is pass to $filecheck
Log("Filter $name$ext \(remove % and comments\)");
my @filecheck = $ltxfile;
s/%.*\n//mg foreach @filecheck;    # del comments
s/^\s*|\s*//mg foreach @filecheck; # del white space
my $filecheck = join q{}, @filecheck;

### Search verbatim and verbatim write environments <input file>
Log("Search verbatim and verbatim write environments in $name$ext");

### Search new verbatim write names in <input file>
my @new_write_env = $filecheck =~ m/$cverbenvw/xg;
if (@new_write_env) {
    Log("Found new verbatim write environments in $name$ext");
    Logarray(\@new_write_env);
    push @verw_env_tmp, @new_write_env;
}

### Search new verbatim environments in <input file>
my @verb_input = $filecheck =~ m/$cverbenv/xg;
if (@verb_input) {
    Log("Found new verbatim environments in $name$ext");
    Logarray(\@verb_input);
    push @verb_env_tmp, @verb_input;
}

### Search \newminted{$mintdenv}{options} in <input file>, need add "code"
my @mint_denv = $filecheck =~ m/$mintdenv/xg;
if (@mint_denv) {
    Log("Found \\newminted\{envname\} in $name$ext");
    # Append "code"
    $mintdenv  = join "\n", map { qq/$_\Qcode\E/ } @mint_denv;
    @mint_denv = split /\n/, $mintdenv;
    Logarray(\@mint_denv);
    push @verb_env_tmp, @mint_denv;
}

### Search \newminted[$mintcenv]{lang} in <input file>
my @mint_cenv = $filecheck =~ m/$mintcenv/xg;
if (@mint_cenv) {
    Log("Found \\newminted\[envname\] in $name$ext");
    Logarray(\@mint_cenv);
    push @verb_env_tmp, @mint_cenv;
}

### Remove repetead again :)
@verb_env_tmp = uniq(@verb_env_tmp);

### Capture verbatim inline macros in <input file>
Log("Search verbatim macros in $name$ext");

### Store all minted inline/short in @mintline
my @mintline;

### Search \newmint{$mintdshrt}{options} in <input file> (while)
my @mint_dshrt = $filecheck =~ m/$mintdshrt/xg;
if (@mint_dshrt) {
    Log("Found \\newmint\{macroname\} (short) in $name$ext");
    Logarray(\@mint_dshrt);
    push @mintline, @mint_dshrt;
}

### Search \newmint[$mintcshrt]{lang}{options} in <input file> (while)
my @mint_cshrt = $filecheck =~ m/$mintcshrt/xg;
if (@mint_cshrt) {
    Log("Found \\newmint\[macroname\] (short) in $name$ext");
    Logarray(\@mint_cshrt);
    push @mintline, @mint_cshrt;
}

### Search \newmintinline{$mintdline}{options} in <input file> (while)
my @mint_dline = $filecheck =~ m/$mintdline/xg;
if (@mint_dline) {
    Log("Found \\newmintinline\{macroname\} in $name$ext");
    # Append "inline"
    $mintdline  = join "\n", map { qq/$_\Qinline\E/ } @mint_dline;
    @mint_dline = split /\n/, $mintdline;
    Logarray(\@mint_dline);
    push @mintline, @mint_dline;
}

### Search \newmintinline[$mintcline]{lang}{options} in <input file> (while)
my @mint_cline = $filecheck =~ m/$mintcline/xg;
if (@mint_cline) {
    Log("Found \\newmintinline\[macroname\] in $name$ext");
    Logarray(\@mint_cline);
    push @mintline, @mint_cline;
}

### Add standart mint, mintinline and lstinline
my @mint_tmp = qw(mint mintinline lstinline);

### Join all inline verbatim macros captured
push @mintline, @mint_tmp;
@mintline = uniq(@mintline);

### Create a regex using @mintline
my $mintline = join q{|}, map { quotemeta } sort { length $a <=> length $b } @mintline;
$mintline = qr/\b(?:$mintline)/x;

### Reserved words in verbatim inline (while)
my %changes_in = (
    '%CleanPST'       => '%PSTCLEAN',
    '\psset'          => '\PSSET',
    '\pspicture'      => '\TRICKS',
    '\endpspicture'   => '\ENDTRICKS',
    '\psgraph'        => '\PSGRAPHTRICKS',
    '\endpsgraph'     => '\ENDPSGRAPHTRICKS',
    '\usepackage'     => '\USEPACKAGE',
    '{graphicx}'      => '{GRAPHICX}',
    '\graphicspath{'  => '\GRAPHICSPATH{',
    );

### Hash to replace \begin and \end in verbatim inline
my %init_end = (
    '\begin{' => '\BEGIN{',
    '\end{'   => '\END{',
    );

### Join changes in new hash (while) for verbatim inline
my %cambios = (%changes_in,%init_end);

### Variables and constantes
my $no_del = "\0";
my $del    = $no_del;

### Rules
my $llaves      = qr/\{ .+? \}                                                          /x;
my $no_llaves   = qr/(?: $llaves )?                                                     /x;
my $corchetes   = qr/\[ .+? \]                                                          /x;
my $delimitador = qr/\{ (?<del>.+?) \}                                                  /x;
my $scontents   = qr/Scontents [*]? $no_corchete                                        /ix;
my $verb        = qr/(?:((spv|(?:q|f)?v|V)erb|$myverb)[*]?)           /ix;
my $lst         = qr/(?:(lst|pyg)inline)(?!\*) $no_corchete                             /ix;
my $mint        = qr/(?: $mintline |SaveVerb) (?!\*) $no_corchete $no_llaves $llaves    /ix;
my $no_mint     = qr/(?: $mintline) (?!\*) $no_corchete                                 /ix;
my $marca       = qr/\\ (?:$verb | $lst |$scontents | $mint |$no_mint) (?:\s*)? (\S) .+? \g{-1}     /sx;
my $comentario  = qr/^ \s* \%+ .+? $                                                    /mx;
my $definedel   = qr/\\ (?: DefineShortVerb | lstMakeShortInline| MakeSpecialShortVerb ) [*]? $no_corchete $delimitador /ix;
my $indefinedel = qr/\\ (?: (Undefine|Delete)ShortVerb | lstDeleteShortInline) $llaves  /ix;

Log('Making changes to inline/multiline verbatim macro before extraction');

### Changes in input file for verbatim inline/multiline
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
        }
        substr $document, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($document) = $pos_inicial + length $encontrado;
    }
}

### Change "escaped braces" to <LTXSB.> (this label is not the one in the document)
$document =~ s/\\[{]/<LTXSBO>/g;
$document =~ s/\\[}]/<LTXSBC>/g;

### Regex for verbatim inline/multiline with braces {...}
my $nestedbr   = qr /   ( [{] (?: [^{}]++ | (?-1) )*+ [}]  )                      /x;
my $fvextra    = qr /\\ (?: (Save|Esc)Verb [*]?) $no_corchete                     /x;
my $mintedbr   = qr /\\ (?:$mintline|pygment) (?!\*) $no_corchete $no_llaves      /x;
my $tcbxverb   = qr /\\ (?: tcboxverb [*]?| Scontents [*]? |$myverb [*]?|lstinline) $no_corchete /x;
my $verb_brace = qr /   (?:$tcbxverb|$mintedbr|$fvextra) (?:\s*)? $nestedbr       /x;

### Change \verb*{code} for verbatim inline/multiline
while ($document =~ /$verb_brace/pgmx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my  $encontrado = ${^MATCH};
    while (my($busco, $cambio) = each %cambios) {
        $encontrado =~ s/\Q$busco\E/$cambio/g;
    }
    substr $document, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
    pos ($document) = $pos_inicial + length $encontrado;
}

### We recovered the escaped braces
$document =~ s/<LTXSBO>/\\{/g;
$document =~ s/<LTXSBC>/\\}/g;

### We recovered CleanPST in all file, but only at begin of line
$document =~ s/^%PSTCLEAN/%CleanPST/gmsx;

### First we do some security checks to ensure that they are verbatim and
### verbatim write environments are unique and disjointed
@verb_env_tmp = array_minus(@verb_env_tmp, @verw_env_tmp); #disjointed
my @verbatim = uniq(@verb_env_tmp);

Log('The environments that are considered verbatim:');
Logarray(\@verbatim);

### Create a Regex for verbatim standart environment
my $verbatim = join q{|}, map { quotemeta } sort { length $a <=> length $b } @verbatim;
$verbatim = qr/$verbatim/x;
my $verb_std = qr {
                    (
                      (?:
                        \\begin\{$verbatim\*?\}
                          (?:
                            (?>[^\\]+)|
                            \\
                            (?!begin\{$verbatim\*?\})
                            (?!end\{$verbatim\*?\})|
                            (?-1)
                          )*
                        \\end\{$verbatim\*?\}
                      )
                    )
                  }x;

### Verbatim write
@verw_env_tmp = array_minus(@verw_env_tmp, @verb_env_tmp); #disjointed
my @verbatim_w = uniq(@verw_env_tmp);

Log('The environments that are considered verbatim write:');
Logarray(\@verbatim_w);

### Create a Regex for verbatim write environment
my $verbatim_w = join q{|}, map { quotemeta } sort { length $a <=> length $b } @verbatim_w;
$verbatim_w = qr/$verbatim_w/x;
my $verb_wrt = qr {
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
                  }x;

### The environments that will be searched for extraction
Log('The environments that will be searched for extraction:');
Logarray(\@extractenv);

### Create a regex to extract environments
my $environ = join q{|}, map { quotemeta } sort { length $a <=> length $b } @extractenv;
$environ = qr/$environ/x;
my $extr_tmp = qr {
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
                  }x;

### Hash for replace in verbatim begin -> Begin end -> END
my %extract_env = crearhash(@extractenv);

### The preview and nopreview environments are "special", need replace
### in verbatim's environments begin -> Begin end -> END
my @preview_env = qw(preview nopreview);
my %preview_env = crearhash(@preview_env);

Log('Making changes to verbatim/verbatim write environments before extraction');

### Hash and Regex for changes, this "regex" is re-used in ALL script
my %replace = (%extract_env, %preview_env, %changes_in, %document);
my $find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;

### We go line by line and make the changes [need /p for ${^MATCH}]
while ($document =~ /$verb_wrt | $verb_std /pgmx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my $encontrado = ${^MATCH};
    $encontrado =~ s/($find)/$replace{$1}/g;
    substr $document, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
    pos ($document) = $pos_inicial + length $encontrado;
}

### Now match preview environment in <input file>
my @env_preview = $document =~ m/(\\begin\{preview\}.+?\\end\{preview\})/gmsx;

### Convert preview to nopreview environment
if (@env_preview) {
    my $preNo = scalar @env_preview;
    Log("Found $preNo preview environments in $name$ext");
    Log("Pass all preview environments to \\begin{nopreview}\%TMP$tmp ... \\end{nopreview}\%TMP$tmp");
    $document =~ s/(?:(\\begin\{|\\end\{))(preview\})/$1no$2\%TMP$tmp/gmsx;
}

### Pass verbatim write environments to dtxtag
Log("Pass verbatim write environments to %<*$dtxverb> ... %</$dtxverb>");
$document   =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                 ($verb_wrt)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;
$atbegindoc =~ s/($verb_wrt)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;

### Pass verbatim environments to dtxtag
Log("Pass verbatim environments to %<*$dtxverb> ... %</$dtxverb>");
$document   =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                 \%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                 ($verb_std)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;

### Pass %CleanPST to dtxtag
Log("Pass %CleanPST ... %CleanPST to %<*remove$tmp> ... %</remove$tmp>");
$document =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
              ^(?:%CleanPST) (.+?) (?:%CleanPST)/\%<\*remove$tmp>$1\%<\/remove$tmp>/gmsx;

### Check plain TeX syntax for pspicture [skip PSTexample]
Log('Convert plain \pspicture to LaTeX syntax [skip in PSTexample]');
$document =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
               \\pspicture(\*)?(.+?)\\endpspicture/\\begin\{pspicture$1\}$2\\end\{pspicture$1\}/gmsx;

### Check plain TeX syntax for psgraph [skip PSTexample]
Log('Convert plain \psgraph to LaTeX syntax [skip in PSTexample]');
$document =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
               \\psgraph(\*)?(.+?)\\endpsgraph/\\begin\{psgraph$1\}$2\\end\{psgraph$1\}/gmsx;

### Pass all postscript environments [skip in PSTexample]
Log("Pass all postscript environments to \\begin{$wrapping} ... \\end{$wrapping}");
$document =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
               \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               (?:\\begin\{postscript\})(?:\s*\[ [^]]*? \])?
                   (?<code>.+?)
               (?:\\end\{postscript\})
              /\\begin\{$wrapping\}$+{code}\\end\{$wrapping\}/gmsx;

### Pass all pstricks environments to \\begin{$wrapping} ... \\end{$wrapping}");
if ($force) {
    # Try to capture \psset{...} for pstricks and psgraph [force]
    Log('Try to capture \psset{...} for pstricks environments [force mode]');
    Log("Pass all pstricks environments to \\begin{$wrapping} ... \\end{$wrapping} [force mode]");
    $document =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                   \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
                   \\begin\{$wrapping\}.+?\\end\{$wrapping\}(*SKIP)(*F)|
                    (?<code>
                     (?:\\psset\{(?:\{.*?\}|[^\{])*\}.+?)?  # if exist ...save
                     \\begin\{(?<env> pspicture\*?| psgraph)\} .+? \\end\{\k<env>\}
                    )
                  /\\begin\{$wrapping\}$+{code}\\end\{$wrapping\}/gmsx;
}
else {
    Log("Pass all pstricks environments to \\begin{$wrapping} ... \\end{$wrapping}");
    $document =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                   \\begin\{$wrapping\}.+?\\end\{$wrapping\}(*SKIP)(*F)|
                   ($extr_tmp)/\\begin\{$wrapping\}$1\\end\{$wrapping\}/gmsx;
}

### All environments are now classified:
### Extraction  ->  \begin{$wrapping} ... \end{$wrapping}
### Verbatim's  ->  %<\*$dtxverb> ... <\/$dtxverb>

### Now split document
my ($preamble,$bodydoc,$enddoc) = $document =~ m/\A (.+?) (\\begin\{document\} .+?)(\\end\{document\}.*)\z/msx;

### Hash for reverse changes for extract and <output file>
my %changes_out = (
    '\PSSET'            => '\psset',
    '\TIKZSET'          => '\tikzset',
    '\TRICKS'           => '\pspicture',
    '\ENDTRICKS'        => '\endpspicture',
    '\PSGRAPHTRICKS'    => '\psgraph',
    '\ENDPSGRAPHTRICKS' => '\endpsgraph',
    '\USEPACKAGE'       => '\usepackage',
    '{GRAPHICX}'        => '{graphicx}',
    '\GRAPHICSPATH{'    => '\graphicspath{',
    '\BEGIN{'           => '\begin{',
    '\END{'             => '\end{',
    '\DOCUMENTCLASS'    => '\documentclass',
    '\PAGESTYLE{'       => '\pagestyle{',
    '\THISPAGESTYLE{'   => '\thispagestyle{',
    '%PSTCLEAN'         => '%CleanPST',
    );

### We restore the changes in body of environments and dtxverb
%replace = (%changes_out);
$find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;
$bodydoc  =~ s/($find)/$replace{$1}/g;
$preamble =~ s/($find)/$replace{$1}/g;

### First search PSTexample environment for extract
my @exa_extract = $bodydoc =~ m/(?:\\begin\{$wrapping\})($BE.+?$EE)(?:\\end\{$wrapping\})/gmsx;
my $exaNo = scalar @exa_extract;

### Set vars for log and print in terminal
my $envEXA  = $exaNo > 1 ? 'PSTexample environments' : 'PSTexample environment';
my $fileEXA = $exaNo > 1 ? 'files' : 'file';

### If PSTexample environment found
if ($exaNo!=0) {
    $PSTexa = 1;
    Log("Found $exaNo $envEXA in $name$ext");
    my $figNo = 1;
    for my $item (@exa_extract) {
        Logline("%##### PSTexample environment captured number $figNo ######%");
        Logline($item);
        $figNo++;
    }
    # Add [graphic={[...]...}] to \begin{PSTexample}[...]
    Log('Append [graphic={[...]...}] to \begin{PSTexample}[...]');
    $figNo = 1;
    while ($bodydoc =~ /\\begin\{$wrapping\}(\s*)?\\begin\{PSTexample\}(\[.+?\])?/gsm) {
        my $swpl_grap = "graphic=\{\[scale=1\]$imgdir/$name-fig-exa";
        my $corchetes = $1;
        my ($pos_inicial, $pos_final) = ($-[1], $+[1]);
        if (not $corchetes) { $pos_inicial = $pos_final = $+[0]; }
        if (not $corchetes  or  $corchetes =~ /\[\s*\]/) {
            $corchetes = "[$swpl_grap-$figNo}]";
        }
        else { $corchetes =~ s/\]/,$swpl_grap-$figNo}]/; }
        substr($bodydoc, $pos_inicial, $pos_final - $pos_inicial) = $corchetes;
        pos($bodydoc) = $pos_inicial + length $corchetes;
    }
    continue { $figNo++; }
    Log('Pass PSTexample environments to \begin{nopreview} ... \end{nopreview}');
    $bodydoc =~ s/\\begin\{$wrapping\}
                    (?<code>\\begin\{PSTexample\} .+? \\end\{PSTexample\})
                  \\end\{$wrapping\}
                 /\\begin\{nopreview\}\%$tmp$+{code}\\end\{nopreview\}\%$tmp/gmsx;
}

### Second search any pstricks environment for extract
my @env_extract = $bodydoc =~ m/(?:$BP)(.+?)(?:$EP)/gmsx;
my $envNo = scalar @env_extract;

### Set vars for log and print in terminal
my $envSTD  = $envNo > 1 ? 'pstricks environments' : 'pstricks environment';
my $fileSTD = $envNo > 1 ? 'files' : 'file';

### If any pstricks environments found
if ($envNo!=0) {
    $STDenv = 1;
    Log("Found $envNo $envSTD in $name$ext");
    my $fig = 1;
    for my $item (@env_extract) {
        Logline("%##### Environment pstricks captured number $fig ######%");
        Logline($item);
        $fig++;
    }
}

### Run script process only if any enviroment(s) found in <input file>
if ($envNo == 0 and $exaNo == 0) {
    errorUsage("$scriptname can not find any environment to extract in $name$ext");
}

### Storing ALL current options of script process for .log file
if ($zip) { $opts_cmd{boolean}{zip} = 1; }
if ($tar) { $opts_cmd{boolean}{tar} = 1; }
if ($shell) { $opts_cmd{boolean}{shell} = 1; }
if ($nopdf) { $opts_cmd{boolean}{nopdf} = 1; }
if ($norun) { $opts_cmd{boolean}{norun} = 1; }
if ($nocrop) { $opts_cmd{boolean}{nocrop} = 1; }
if ($srcenv) { $opts_cmd{boolean}{srcenv} = 1; }
if ($gray) { $opts_cmd{boolean}{gray} = 1; }
if ($force) { $opts_cmd{boolean}{force} = 1; }
if ($noprew) { $opts_cmd{boolean}{noprew} = 1; }
if ($runbibtex) { $opts_cmd{boolean}{bibtex} = 1; }
if ($runbiber) { $opts_cmd{boolean}{biber} = 1; }
if ($clear) { $opts_cmd{boolean}{clear} = 1; }
if ($nosource) { $opts_cmd{boolean}{nosource} = 1; }
if ($arara) { $opts_cmd{compiler}{arara} = 1; }
if ($latexmk) { $opts_cmd{compiler}{latexmk} = 1; }
if ($luatex) { $opts_cmd{compiler}{luatex} = 1; }
if ($xetex) { $opts_cmd{compiler}{xetex} = 1; }
if ($tmpverbenv) { $opts_cmd{string}{ignore} = $tmpverbenv; }
$opts_cmd{string}{myverb} = $myverb;
$opts_cmd{string}{dpi} = $dpi;
$opts_cmd{string}{runs} = $runs;
$opts_cmd{string}{margins} = $margins;
$opts_cmd{string}{imgdir} = $imgdir;

foreach my $key (keys %{$opts_cmd{boolean}}) {
    if (defined $opts_cmd{boolean}{$key}) { push @currentopt, "--$key"; }
}
foreach my $key (keys %{$opts_cmd{compiler}}) {
    if (defined $opts_cmd{compiler}{$key}) { push @currentopt, "--$key"; }
}
foreach my $key (keys %{$opts_cmd{image}}) {
    if (defined $opts_cmd{image}{$key}) { push @currentopt, "--$key"; }
}
foreach my $key (keys %{$opts_cmd{string}}) {
    if (defined $opts_cmd{string}{$key}) { push @currentopt, "--$key $opts_cmd{string}{$key}"; }
}

@currentopt = grep !/--pdf/, @currentopt;
my @sorted_words = sort { length $a <=> length $b } @currentopt;

Log('The script will execute the following options:');
Logarray(\@sorted_words);

### Set directory to save generated files, need full path for goog log :)
my $imgdirpath = File::Spec->rel2abs($imgdir);

if (-e $imgdir) {
    Infoline("The generated files will be saved in $imgdirpath");
}
else {
    Infoline("Creating the directory $imgdirpath to save the generated files");
    Infocolor('Running', "mkdir $imgdirpath");
    Logline("[perl] mkdir($imgdir,0744)");
    mkdir $imgdir,0744 or errorUsage("Can't create the directory $imgdir");
}

### Set compiler for process <input file>
my $compiler = $xetex  ? 'xelatex'
             : $luatex ? 'dvilualatex'
             :           'latex'
             ;

### Set options for pdfcrop
my $opt_crop = $xetex ?  "--xetex  --margins $margins"
             : $luatex ? "--luatex --margins $margins"
             :           "--margins $margins"
             ;

### Set options for preview package
my $opt_prew = $xetex ? 'xetex,' : q{};

### Set message in terminal
my $msg_compiler = $xetex ?  'xelatex'
                 : $luatex ? 'dvilualatex>dvips>ps2pdf'
                 :           'latex>dvips>ps2pdf'
                 ;

### Set write18 for compiler in TeXLive and MikTeX
if ($shell) {
    $write18 = '-shell-escape';
    $write18 = '--enable-write18' if defined $ENV{'TEXSYSTEM'} and $ENV{'TEXSYSTEM'} =~ /miktex/i;
}
else {
    $write18 = '-no-shell-escape';
    $write18 = '--disable-write18' if defined $ENV{'TEXSYSTEM'} and $ENV{'TEXSYSTEM'} =~ /miktex/i;
}

### Set options for compiler
my $opt_compiler = "$write18 -interaction=nonstopmode -recorder";

if (!$norun) {
    Log("The options '$opt_compiler' will be passed to $compiler");
}

### Set -q for system command line (gs, poppler-utils, dvips)
my $quiet = $verbose ? q{} : '-q';

### Set options for ghostscript
my %opt_gs_dev = (
    pdf  => '-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=pdfwrite',
    gray => '-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -sColorConversionStrategy=Gray -sProcessColorModel=DeviceGray',
    png  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=pngalpha -r$dpi",
    bmp  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=bmp32b -r$dpi",
    jpg  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=jpeg -r$dpi -dJPEGQ=100 -dGraphicsAlphaBits=4 -dTextAlphaBits=4",
    tif  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=tiff32nc -r$dpi",
    );

### Set poppler-utils executables
my %cmd_poppler = (
    eps => "pdftops",
    ppm => "pdftoppm",
    svg => "pdftocairo",
    );

### Set options for poppler-utils
my %opt_poppler = (
    eps => "$quiet -eps",
    ppm => "$quiet -r $dpi",
    svg => "$quiet -svg",
    );

### Copy preamble and body for temp file with all environments
my $atbeginout = $atbegindoc;
my $preamout   = $preamble;
my $tmpbodydoc = $bodydoc;

### Match \pagestyle and \thispagestyle in preamble
my $style_page = qr /(?:\\)(?:this)?(?:pagestyle\{) (.+?) (?:\})/x;
my @style_page = $preamout =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)| $style_page/gmsx;
my %style_page = map { $_ => 1 } @style_page; # anon hash

### Set \pagestyle{empty} for standalone files and process
if (@style_page) {
    if (!exists $style_page{empty}) {
        Log("Replacing page style for generated files");
        $preamout =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                      (\\(this)?pagestyle)(?:\{.+?\})/$1\{empty\}/gmsx;
    }
}
else {
    Log('Add \pagestyle{empty} for generated files');
    $preamout = $preamout."\\pagestyle\{empty\}\n";
}

### We created a preamble for the individual files
my $sub_prea = "$atbeginout$preamout".'\begin{document}';

### Revert changes
$sub_prea =~ s/\%<\*$dtxverb>\s*(.+?)\s*\%<\/$dtxverb>/$1/gmsx;
%replace  = (%changes_out);
$find     = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;
$sub_prea =~ s/($find)/$replace{$1}/g;
$sub_prea =~ s/^(?:\%<\*remove$tmp>)(.+?)(?:\%<\/remove$tmp>)/%CleanPST$1%CleanPST/gmsx;

### Write standalone files for environments
if (!$nosource) {
    my $src_name = "$name-fig-";
    my $srcNo    = 1;
    if ($srcenv) {
        Log('Extract source code of all captured environments without preamble');
        if ($STDenv) {
            if (-e "$imgdir/$name-fig-1$ext") {
                Log("Recreating $envNo $fileSTD [$ext] with source code for $envSTD in $imgdirpath");
                print "Recreating $envNo $fileSTD ", color('magenta'), "[$ext]",
                color('reset'), " with source code for $envSTD\r\n";
            }
            else {
                Log("Creating $envNo $fileSTD [$ext] with source code for $envSTD in $imgdirpath");
                print "Creating $envNo $fileSTD ", color('magenta'), "[$ext]",
                color('reset'), " with source code for $envSTD\r\n";
            }
            # Write files
            while ($tmpbodydoc =~ m/$BP(?:\s*)?(?<env_src>.+?)(?:\s*)?$EP/gms) {
                open my $outexasrc, '>', "$imgdir/$src_name$srcNo$ext";
                    print {$outexasrc} $+{'env_src'};
                close $outexasrc;
            }
            continue { $srcNo++; }
        }
        if ($PSTexa) {
            if (-e "$imgdir/$name-fig-exa-1$ext") {
                Log("Recreating $exaNo $fileEXA [$ext] with source code for $envEXA in $imgdirpath");
                print "Recreating $exaNo $fileEXA ", color('magenta'), "[$ext]",
                color('reset'), " with source code for $envEXA\r\n";
            }
            else {
                Log("Creating $exaNo $fileEXA [$ext] with source code for $envEXA in $imgdirpath");
                print "Creating $exaNo $fileEXA ", color('magenta'), "[$ext]",
                color('reset'), " with source code for $envEXA\r\n";
            }
            # Write files
            while ($tmpbodydoc =~ m/$BE\[.+?(?<pst_exa_name>$imgdir\/.+?-\d+)\}\]\s*(?<exa_src>.+?)\s*$EE/gms) {
                open my $outstdsrc, '>', "$+{'pst_exa_name'}$ext";
                    print {$outstdsrc} $+{'exa_src'};
                close $outstdsrc;
            }
        }
    }
    else {
        Log('Extract source code of all captured environments with preamble');
        if ($STDenv) {
            if (-e "$imgdir/$name-fig-1$ext") {
                Log("Recreating $envNo standalone $fileSTD [$ext] for $envSTD in $imgdirpath");
                print "Recreating $envNo standalone $fileSTD ", color('magenta'), "[$ext]",
                color('reset'), " for $envSTD\r\n";
            }
            else {
                Log("Creating $envNo standalone $fileSTD [$ext] for $envSTD in $imgdirpath");
                print "Creating $envNo standalone $fileSTD ", color('magenta'), "[$ext]",
                color('reset'), " for $envSTD\r\n";
            }
            # Write files
            while ($tmpbodydoc =~ m/$BP(?:\s*)?(?<env_src>.+?)(?:\s*)?$EP/gms) {
                open my $outstdfile, '>', "$imgdir/$src_name$srcNo$ext";
                    print {$outstdfile} "$sub_prea\n$+{'env_src'}\n\\end\{document\}";
                close $outstdfile;
            }
            continue { $srcNo++; }
        }
        if ($PSTexa) {
            if (-e "$imgdir/$name-fig-exa-1$ext") {
                Log("Recreating $exaNo standalone $fileEXA [$ext] for $envEXA in $imgdirpath");
                print "Recreating $exaNo standalone $fileEXA ", color('magenta'), "[$ext]",
                color('reset'), " for $envEXA\r\n";
            }
            else {
                Log("Creating $exaNo standalone $fileEXA [$ext] for $envEXA in $imgdirpath");
                print "Creating $exaNo standalone $fileEXA ", color('magenta'), "[$ext]",
                color('reset'), " for $envEXA\r\n";
            }
            # Write files
            while ($tmpbodydoc =~ m/$BE\[.+?(?<pst_exa_name>$imgdir\/.+?-\d+)\}\]\s*(?<exa_src>.+?)\s*$EE/gms) {
                open my $outexafile, '>', "$+{'pst_exa_name'}$ext";
                    print {$outexafile} "$sub_prea\n$+{'exa_src'}\n\\end\{document\}";
                close $outexafile;
            }
        }
    }
}

### Store options for preview and pst-pdf (add at begin document)
my $previewpkg = <<"EXTRA";
\\PassOptionsToPackage\{inactive\}\{pst-pdf\}%
\\AtBeginDocument\{%
\\RequirePackage\[inactive\]\{pst-pdf\}%
\\RequirePackage\[${opt_prew}active,tightpage\]\{preview\}%
\\renewcommand\\PreviewBbAdjust\{-60pt -60pt 60pt 60pt\}\}%
EXTRA

### Store options for pst-pdf (add at begin document)
my $pstpdfpkg = <<'EXTRA';
\PassOptionsToPackage{inactive}{pst-pdf}
\AtBeginDocument{%
\RequirePackage[inactive]{pst-pdf}}%
EXTRA

### First match preview package in preamble (prevent option clash)
my $REQPACK   = quotemeta'\RequirePackage';
my $USEPACK   = quotemeta'\usepackage';
my $CORCHETES = qr/\[ [^]]*? \]/x;

my $PALABRAS = qr/\b (?: preview )/x;
my $FAMILIA  = qr/\{ \s* $PALABRAS (?: \s* [,] \s* $PALABRAS )* \s* \}(\%*)?/x;

Log('Remove preview package in preamble [memory]');
$preamout =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               ^ $USEPACK (?: $CORCHETES )? $FAMILIA \s*//msxg;
$preamout =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               (?: ^ $USEPACK \{ | \G) [^}]*? \K (,?) \s* $PALABRAS (\s*) (,?) /$1 and $3 ? ',' : $1 ? $2 : ''/gemsx;
$preamout =~ s/^\\usepackage\{\}(?:[\t ]*(?:\r?\n|\r))+/\n/gmsx;

### Remove %<*$dtxverb> ... %</$dtxverb> in tmpbodydoc and preamout
$tmpbodydoc =~ s/\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/$1/gmsx;
$preamout   =~ s/\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/$1/gmsx;
$atbeginout =~ s/\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/$1/gmsx;

### Adjust nopreview environments
$tmpbodydoc =~ s/\\begin\{nopreview\}\%$tmp
                    (?<code> .+?)
                  \\end\{nopreview\}\%$tmp
                /\\begin\{nopreview\}\n$+{code}\n\\end\{nopreview\}/gmsx;

### Adjust $wrapping environments (no need realy)
$tmpbodydoc =~ s/\\begin\{$wrapping\}
                    (?<code>.+?)
                  \\end\{$wrapping\}
                /\\begin\{$wrapping\}\n$+{code}\n\\end\{$wrapping\}/gmsx;

### Reverse changes for temporary file with all env (no in -exa file)
$tmpbodydoc =~ s/($find)/$replace{$1}/g;
$tmpbodydoc =~ s/(\%TMP$tmp)//g;
$preamout   =~ s/($find)/$replace{$1}/g;
$preamout   =~ s/^(?:\%<\*remove$tmp>)(.+?)(?:\%<\/remove$tmp>)/%CleanPST$1%CleanPST/gmsx;
$atbeginout =~ s/($find)/$replace{$1}/g;

### We created a preamble for individual files with all environments
$sub_prea = $noprew ? "$atbeginout$pstpdfpkg$preamout".'\begin{document}'
          :           "$atbeginout$previewpkg$preamout"
          ;

### Create a one file with "all" PSTexample environments extracted
if ($PSTexa) {
    @exa_extract = undef;
    Log("Adding packages to $name-fig-exa-$tmp$ext [memory]");
    Logline($pstpdfpkg);
    Log('Convert plain Tex syntax for pspicture and psgraph to LaTeX syntax in PSTexample environments');
    while ($tmpbodydoc =~ m/$BE\[.+? $imgdir\/.+?-\d+\}\] .+?$EE/pgsmx ) { # search
        my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
        my $encontrado = ${^MATCH};
        $encontrado =~ s/\\pspicture(\*)?(.+?)\\endpspicture/\\begin\{pspicture$1\}$2\\end\{pspicture$1\}/gmsx;
        $encontrado =~ s/\\psgraph(\*)?(.+?)\\endpsgraph/\\begin\{psgraph$1\}$2\\end\{psgraph$1\}/gmsx;
        substr $tmpbodydoc, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($tmpbodydoc) = $pos_inicial + length $encontrado;
    }
    # Write file
    Infoline("Creating $name-fig-exa-$tmp$ext with $exaNo $envEXA extracted");
    while ($tmpbodydoc =~ m/$BE\[.+? $imgdir\/.+?-\d+\}\](?<exa_src>.+?)$EE/gmsx ) { # search
        push @exa_extract, $+{'exa_src'}."\\newpage\n";
        open my $allexaenv, '>', "$name-fig-exa-$tmp$ext";
            print {$allexaenv} "$atbeginout$pstpdfpkg$preamout".'\begin{document}'."@exa_extract"."\\end\{document\}";
        close $allexaenv;
    }
    # Remove [graphic={...}] in PSTexample example environments
    $tmpbodydoc =~ s/($BE)(?:\[graphic=\{\[scale=1\]$imgdir\/.+?-\d+\}\])/$1/gmsx;
    $tmpbodydoc =~ s/($BE\[.+?)(?:,graphic=\{\[scale=1\]$imgdir\/.+?-\d+\})(\])/$1$2/gmsx;
    # Moving and renaming
    if ($norun) {
        Infoline("Moving and renaming $name-fig-exa-$tmp$ext to $name-fig-exa-all$ext");
        if (-e "$imgdir/$name-fig-exa-all$ext") {
            Infocolor('Warning', "The file [$name-fig-exa-all$ext] already exists and will be rewritten");
            Log("Rewriting the file $name-fig-exa-all$ext in $imgdirpath");
        }
        else {
            Log("Writing the file $name-fig-exa-all$ext in $imgdirpath");
        }
        if ($verbose) {
            Infocolor('Running', "mv $workdir/$name-fig-exa-$tmp$ext $imgdirpath/$name-fig-exa-all$ext");
        }
        else {
            Infocolor('Running', "mv $name-fig-exa-$tmp$ext ./$imgdir/$name-fig-exa-all$ext");
        }
        Logline("[perl] move($workdir/$name-fig-exa-$tmp$ext, $imgdirpath/$name-fig-exa-all$ext)");
        move("$workdir/$name-fig-exa-$tmp$ext", "$imgdir/$name-fig-exa-all$ext")
        or die "* Error!!: Couldn't be renamed $name-fig-exa-$tmp$ext to ./$imgdir/$name-fig-exa-all$ext";
    }
}

### Create a one file with "all" standard environments extracted
if ($STDenv) {
    if ($noprew) {
        Log("Creating $name-fig-$tmp$ext with $envNo $envSTD extracted [no preview]");
        print "Creating $name-fig-$tmp$ext with $envNo $envSTD extracted",
        color('magenta'), " [no preview]\r\n",color('reset');
    }
    else {
        Log("Creating $name-fig-$tmp$ext with $envNo $envSTD extracted [preview]");
        print "Creating $name-fig-$tmp$ext with $envNo $envSTD extracted",
        color('magenta'), " [preview]\r\n",color('reset');
    }
    open my $allstdenv, '>', "$name-fig-$tmp$ext";
        if ($noprew) {
            my @env_extract;
            while ($tmpbodydoc =~ m/(?:$BP)(?<env_src>.+?)(?:$EP)/gms) {
                push @env_extract,$+{'env_src'}."\\newpage\n";
            }
            Log("Adding packages to $name-fig-$tmp$ext");
            Logline($pstpdfpkg);
            print {$allstdenv} $sub_prea."@env_extract"."\\end{document}";
        }
        else {
            Log("Adding packages to $name-fig-$tmp$ext");
            Logline($previewpkg);
            Log("Convert $wrapping to preview environments in $name-fig-$tmp$ext");
            # Convert $wrapping to preview environments
            $tmpbodydoc =~ s/\\begin\{$wrapping\}(?<code>.+?)\\end\{$wrapping\}
                            /\\begin\{preview\}\n$+{code}\n\\end\{preview\}\n/gmsx;
            print {$allstdenv} $sub_prea.$tmpbodydoc."\n\\end{document}";
        }
    close $allstdenv;
    # Moving and renaming
    if ($norun) {
        Infoline("Moving and renaming $name-fig-$tmp$ext to $name-fig-all$ext");
        if (-e "$imgdir/$name-fig-all$ext") {
            Infocolor('Warning', "The file [$name-fig-all$ext] already exists and will be rewritten");
            Log("Rewriting the file $name-fig-all$ext in $imgdirpath");
        }
        else {
            Log("Writing the file $name-fig-all$ext in $imgdirpath");
        }
        if ($verbose) {
            Infocolor('Running', "mv $workdir/$name-fig-$tmp$ext $imgdirpath/$name-fig-all$ext");
        }
        else {
            Infocolor('Running', "mv $name-fig-$tmp$ext ./$imgdir/$name-fig-all$ext");
        }
        Logline("[perl] move($workdir/$name-fig-$tmp$ext, $imgdirpath/$name-fig-all$ext)");
        move("$workdir/$name-fig-$tmp$ext", "$imgdir/$name-fig-all$ext")
        or die "* Error!!: Couldn't be renamed $name-fig-$tmp$ext to ./$imgdir/$name-fig-all$ext";
    }
}

### Compiler and generate PDF files
if (!$norun) {
Log('Generate a PDF file with all captured environments');
my @compiler = (1..$runs);
opendir (my $DIR, $workdir);
    while (readdir $DIR) {
        if (/(?<name>$name-fig(-exa)?)(?<type>-$tmp$ext)/) {
            Log("Compiling the file $+{name}$+{type} using [$msg_compiler]");
            print "Compiling the file $+{name}$+{type} using ", color('magenta'), "[$msg_compiler]\r\n",color('reset');
            # Compiling file
            for (@compiler){
                RUNOSCMD("$compiler $opt_compiler","$+{name}$+{type}",'show');
            }
            # Using dvips>ps2pdf
            if ($compiler eq 'latex' or $compiler eq 'dvilualatex') {
                RUNOSCMD("dvips $quiet -Ppdf", "-o $+{name}-$tmp.ps $+{name}-$tmp.dvi",'show');
                RUNOSCMD("ps2pdf -sPDFSETTINGS=prepress -sAutoRotatePages=None", "$+{name}-$tmp.ps  $+{name}-$tmp.pdf",'show');
            }
            # Moving and renaming temp files with source code
            Infoline("Moving and renaming $+{name}$+{type} to $+{name}-all$ext");
            if (-e "$imgdir/$+{name}-all$ext") {
                Infocolor('Warning', "The file [$+{name}-all$ext] already exists and will be rewritten");
                Log("Rewriting the file $+{name}-all$ext with all source for environments in $imgdirpath");
            }
            else {
                Log("Writing the file $+{name}-all$ext with all source for environments in $imgdirpath");
            }
            if ($verbose){
                Infocolor('Running', "mv $workdir/$+{name}$+{type} $imgdirpath/$+{name}-all$ext");
            }
            else {
                Infocolor('Running', "mv $+{name}$+{type} ./$imgdir/$+{name}-all$ext");
            }
            Logline("[perl] move($workdir/$+{name}$+{type}, $imgdirpath/$+{name}-all$ext)");
            move("$workdir/$+{name}$+{type}", "$imgdir/$+{name}-all$ext")
            or die "* Error!!: Couldn't be renamed $+{name}$+{type} to $imgdir/$+{name}-all$ext";
            # pdfcrop
            if (!$nocrop) {
                Infoline("Cropping the file $+{name}-$tmp.pdf");
                RUNOSCMD("pdfcrop $opt_crop", "$+{name}-$tmp.pdf $+{name}-$tmp.pdf",'show');
            }
            # gray
            if ($gray) {
                Infoline("Creating the file $+{name}-all.pdf [gray] in $tempDir");
                RUNOSCMD("$gscmd $quiet $opt_gs_dev{gray} ","-o $tempDir/$+{name}-all.pdf $workdir/$+{name}-$tmp.pdf",'show');
            }
            else {
                Infoline("Creating the file $+{name}-all.pdf in $tempDir");
                if ($verbose){
                    Infocolor('Running', "mv $workdir/$+{name}-$tmp.pdf $tempDir/$+{name}-all.pdf");
                }
                else { Infocolor('Running', "mv $+{name}-$tmp.pdf $tempDir/$+{name}-all.pdf"); }
                # Renaming pdf file
                Logline("[perl] move($workdir/$+{name}-$tmp.pdf, $tempDir/$+{name}-all.pdf)");
                move("$workdir/$+{name}-$tmp.pdf", "$tempDir/$+{name}-all.pdf")
                or die "* Error!!: Couldn't be renamed $+{name}-$tmp.pdf to $tempDir/$+{name}-all.pdf";
            }
        }
    }
closedir $DIR;
}

### Create image formats in separate files
if (!$norun) {
    Log("Creating the image formats: $format, working on $tempDir");
    opendir(my $DIR, $tempDir);
        while (readdir $DIR) {
            # PDF/PNG/JPG/BMP/TIFF format suported by ghostscript
            if (/(?<name>$name-fig(-exa)?)(?<type>-all\.pdf)/) {
                for my $var (qw(pdf png jpg bmp tif)) {
                    if (defined $opts_cmd{image}{$var}) {
                        Log("Generating format [$var] from file $+{name}$+{type} in $imgdirpath using $gscmd");
                        print 'Generating format', color('blue'), " [$var] ", color('reset'),"from file $+{name}$+{type}\r\n";
                        RUNOSCMD("$gscmd $quiet $opt_gs_dev{$var} ", "-o $workdir/$imgdir/$+{name}-%1d.$var $tempDir/$+{name}$+{type}",'show');
                    }
                }
            }
            # EPS/PPM/SVG format suported by poppler-utils
            if (/(?<name>$name-fig-exa)(?<type>-all\.pdf)/) { # pst-exa package
                for my $var (qw(eps ppm svg)) {
                    if (defined $opts_cmd{image}{$var}) {
                        Log("Generating format [$var] from file $+{name}$+{type} in $imgdirpath using $cmd_poppler{$var}");
                        print 'Generating format', color('blue'), " [$var] ", color('reset'),"from file $+{name}$+{type}\r\n";
                        if (!$verbose){
                            Infocolor('Running', "$cmd_poppler{$var} $opt_poppler{$var}");
                        }
                        for (my $epsNo = 1; $epsNo <= $exaNo; $epsNo++) {
                            RUNOSCMD("$cmd_poppler{$var} $opt_poppler{$var}", "-f $epsNo -l $epsNo $tempDir/$+{name}$+{type} $workdir/$imgdir/$+{name}-$epsNo.$var",'only');
                        }
                    }
                }
            }
            if (/(?<name>$name-fig)(?<type>-all\.pdf)/) {
                for my $var (qw(eps ppm svg)) {
                    if (defined $opts_cmd{image}{$var}) {
                        Log("Generating format [$var] from file $+{name}$+{type} in $imgdirpath using $cmd_poppler{$var}");
                        print 'Generating format', color('blue'), " [$var] ", color('reset'),"from file $+{name}$+{type}\r\n";
                        if (!$verbose){
                            Infocolor('Running', "$cmd_poppler{$var} $opt_poppler{$var}");
                        }
                        for (my $epsNo = 1; $epsNo <= $envNo; $epsNo++) {
                            RUNOSCMD("$cmd_poppler{$var} $opt_poppler{$var}", "-f $epsNo -l $epsNo $tempDir/$+{name}$+{type} $workdir/$imgdir/$+{name}-$epsNo.$var",'only');
                        }
                    }
                }
            }
        } # close while
    closedir $DIR;
    # Renaming PPM image files
    if (defined $opts_cmd{image}{ppm}) {
        Log("Renaming [ppm] images in $imgdirpath");
        if ($verbose){
            print 'Renaming', color('blue'), " [ppm] ", color('reset'),"images in $imgdirpath\r\n";
        }
        opendir(my $DIR, $imgdir);
            while (readdir $DIR) {
                if (/(?<name>$name-fig(-exa)?-\d+\.ppm)(?<sep>-\d+)(?<ppm>\.ppm)/) {
                    if ($verbose){
                        Infocolor('Running', "mv $+{name}$+{sep}$+{ppm} $+{name}");
                    }
                    Logline("[perl] move($imgdirpath/$+{name}$+{sep}$+{ppm}, $imgdirpath/$+{name})");
                    move("$imgdir/$+{name}$+{sep}$+{ppm}", "$imgdir/$+{name}")
                    or die "* Error!!: Couldn't be renamed $+{name}$+{sep}$+{ppm} to $+{name}";
                }
            }
        closedir $DIR;
    }
} # close run

### Constant
my $findgraphicx = 'true';

### Replacing the extracted environments with \includegraphics
Log("Convert pstricks extracted environments to \\includegraphics for $name-pdf$ext");
my $grap  =  "\\includegraphics[scale=1]{$name-fig-";
my $close =  '}';
my $imgNo =  1;
$bodydoc  =~ s/$BP.+?$EP/$grap@{[$imgNo++]}$close/msg;

### Add $atbegindoc to $preamble
$preamble = "$atbegindoc$preamble";

### Remove content in preamble
my @tag_remove_preamble = $preamble =~ m/(?:^\%<\*remove$tmp>.+?\%<\/remove$tmp>)/gmsx;
if (@tag_remove_preamble) {
    Log("Removing the content between %CleanPST ... %CleanPST in preamble for $name-pdf$ext");
    $preamble =~ s/^\%<\*remove$tmp>\s*(.+?)\s*\%<\/remove$tmp>(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
}

### Suport for pst-exa package
my $pstexa = qr/(?:\\ usepackage) \[\s*(.+?)\s*\] (?:\{\s*(pst-exa)\s*\} ) /x;
my @pst_exa;
my %pst_exa;

### Search pst-exa package
@pst_exa  = $preamble =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|$pstexa/gmsx;
%pst_exa = map { $_ => 1 } @pst_exa;

### If found comment and adjust path for graphic files
if (@pst_exa) {
    Log("Comment pst-exa package in preamble for $name-pdf$ext");
    $findgraphicx = 'false';
    $preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                  (\\usepackage\[)\s*(swpl|tcb)\s*(\]\{pst-exa\})/\%$1$2,pdf$3/msxg;
    # Change for [tcb] option (path for graphic files)
    if (exists $pst_exa{tcb}) {
        Log("Suport for \\usepackage[tcb,pdf]\{pst-exa\} for $name-pdf$ext");
        $bodydoc =~ s/(graphic=\{)\[(scale=\d*)\]($imgdir\/$name-fig-exa-\d*)\}/$1$2\}\{$3\}/gsmx;
    }
}

### Regex for remove (pst-?) packages in preamble
$PALABRAS = qr/\b (?: pst-\w+ | pstricks (?: -add | -pdf )? | psfrag |psgo |vaucanson-g| auto-pst-pdf(?: -lua )? )/x;
$FAMILIA  = qr/\{ \s* $PALABRAS (?: \s* [,] \s* $PALABRAS )* \s* \}(\%*)?/x;

Log("Remove pstricks packages in preamble for $name-pdf$ext [no pst-exa]");
$preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               ^ $USEPACK (?: $CORCHETES )? $FAMILIA \s*//msxg;
$preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               (?: ^ $USEPACK \{ | \G) [^}]*? \K (,?) \s* $PALABRAS (\s*) (,?) /$1 and $3 ? ',' : $1 ? $2 : ''/gemsx;

### Uncomment pst-exa package
if (@pst_exa) {
    Log("Uncomment pst-exa package in preamble for $name-pdf$ext");
    $preamble =~ s/(?:\%)(\\usepackage\[\s*)(swpl|tcb)(,pdf\s*\]\{pst-exa\})/$1$2$3/msxg;
}

Log("Remove \\psset\{...\} in preamble for $name-pdf$ext");
$preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               \\psset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))+//gmsx;

Log("Remove \\SpecialCoor in preamble for $name-pdf$ext");
$preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               \\SpecialCoor(?:[\t ]*(?:\r?\n|\r))+//gmsx;

Log("Remove empty lines in preamble for $name-pdf$ext");
$preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               ^\\usepackage\{\}(?:[\t ]*(?:\r?\n|\r))+/\n/gmsx;
$preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               ^(?:[\t ]*(?:\r?\n|\r))?+//gmsx;

### To be sure that the package graphicx and \graphicspath is in the main
### document and not in a verbatim write environment we make the changes
my %tmpreplace = (
    'graphicx'     => 'TMPGRAPHICXTMP',
    'graphicspath' => 'TMPGRAPHICSPATHTMP',
);
my $findtmp     = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %tmpreplace;

### We go line by line and make the changes [need /p for ${^MATCH}]
while ($preamble =~ /\%<\*$dtxverb>(.+?)\%<\/$dtxverb> | $tmpcomment /pgmsx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my $encontrado = ${^MATCH};
    $encontrado =~ s/($findtmp)/$tmpreplace{$1}/g;
    substr $preamble, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
    pos ($preamble) = $pos_inicial + length $encontrado;
}

### Now we're trying to capture \graphicspath{...}
my $graphicspath= qr/\\ graphicspath \{ ((?: $llaves )+) \}/ix;
my @graphicspath;
@graphicspath = $preamble =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|($graphicspath)/gmsx;

if (@graphicspath) {
    Log("Found \\graphicspath in preamble for $name-pdf$ext");
    $findgraphicx = 'false';
    while ($preamble =~ /$graphicspath /pgmx) {
        my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
        my $encontrado = ${^MATCH};
        if ($encontrado =~ /$graphicspath/) {
            my  $argumento = $1;
            if ($argumento !~ /\{$imgdir\/\}/) {
                $argumento .= "\{$imgdir/\}";
                my  $cambio = "\\graphicspath{$argumento}";
                substr $preamble, $pos_inicial, $pos_final-$pos_inicial, $cambio;
                pos($preamble) = $pos_inicial + length $cambio;
            }
        }
    }
}

### Possible packages that load graphicx
my @pkgcandidates = qw (
    rotating epsfig lyluatex xunicode parsa xepersian-hm gregoriotex teixmlslides
    teixml fotex hvfloat pgfplots grfpaste gmbase hep-paper postage schulealt
    schule utfsym cachepic abc doclicense rotating epsfig semtrans mgltex
    graphviz revquantum mpostinl cmpj cmpj2 cmpj3 chemschemex register papercdcase
    flipbook wallpaper asyprocess draftwatermark rutitlepage dccpaper-base
    nbwp-manual mandi fmp toptesi isorot pinlabel cmll graphicx-psmin ptmxcomp
    countriesofeurope iodhbwm-templates fgruler combinedgraphics pax pdfpagediff
    psfragx epsdice perfectcut upmethodology-fmt ftc-notebook tabvar vtable
    teubner pas-cv gcard table-fct pdfpages keyfloat pdfscreen showexpl simplecd
    ifmslide grffile reflectgraphics markdown bclogo tikz-page pst-uml realboxes
    musikui csbulobalka lwarp mathtools sympytex mpgraphics miniplot.sty:77
    dottex pdftricks2 feupphdteses tex4ebook axodraw2 hagenberg-thesis dlfltxb
    hu-berlin-bundle draftfigure quicktype endofproofwd euflag othelloboard
    pdftricks unswcover breqn pdfswitch latex-make figlatex repltext etsvthor
    cyber xcookybooky xfrac mercatormap chs-physics-report tikzscale ditaa
    pst-poker gmp CJKvert asypictureb hletter tikz-network powerdot-fuberlin
    skeyval gnuplottex plantslabels fancytooltips ieeepes pst-vectorian
    phfnote overpic xtuformat stubs graphbox ucs pdfwin metalogo mwe
    inline-images asymptote UNAMThesis authorarchive amscdx adjustbox
    trimclip fixmetodonotes incgraph scanpages alertmessage
    svg quiz2socrative realhats autopdf egplot decorule figsize tikzexternal
    pgfcore frontespizio textglos graphicx tikz tcolorbox
    );

my $pkgcandidates = join q{|}, map { quotemeta } sort { length $a <=> length $b } @pkgcandidates;
$pkgcandidates = qr/$pkgcandidates/x;
my @graphicxpkg;

### Now we're trying to capture graphicx package
@graphicxpkg = $preamble =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|($pkgcandidates)/gmsx;

if (@graphicxpkg and $findgraphicx ne 'false') {
    Log("Found graphicx package in preamble for $name-pdf$ext");
    $findgraphicx = 'false';
}

### Revert changes in preamble for temp <output file>
my %tmpoutreplace = (
    'TMPGRAPHICXTMP'     => 'graphicx',
    'TMPGRAPHICSPATHTMP' => 'graphicspath',
);

%replace  = (%changes_out,%tmpoutreplace);
$find     = join q{|}, map {quotemeta} sort { length $a <=> length $b } keys %replace;

### Try to capture arara:compiler in preamble of <output file>
my @arara_engines = qw (latex pdflatex lualatex xelatex luahbtex);
my $arara_engines = join q{|}, map { quotemeta} sort { length $a <=> length $b } @arara_engines;
$arara_engines = qr/\b(?:$arara_engines)/x;
my $arara_rule = qr /^(?:\%\s{1}arara[:]\s{1}) ($arara_engines) /msx;

### Capture graphicx.sty in .log of LaTeX file
if ($findgraphicx ne 'false') {
    Log("Couldn't capture the graphicx package for $name-pdf$ext in preamble");
    my $ltxlog;
    my @graphicx;
    my $null = devnull();
    # Copy preamble and revert changes
    my $preambletmp = $preamble;
    $preambletmp =~ s/($find)/$replace{$1}/g;
    $preambletmp   =~ s/\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/$1/gmsx;
    Log("Creating $name-fig-$tmp$ext [only preamble]");
    if ($verbose) { say "Creating [$name-fig-$tmp$ext] with only preamble"; }
    open my $OUTfile, '>', "$name-fig-$tmp$ext";
        print {$OUTfile} "$preambletmp\n\\stop";
    close $OUTfile;
    # Set compiler for arara
    if ($opts_cmd{compiler}{arara}) {
        my @engine = $atbegindoc =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|$arara_rule/msx;
        my %engine = map { $_ => 1 } @engine;
        if (%engine) {
            for my $var (@arara_engines) {
                if (defined $engine{$var}) {
                    Log("Found engine [$var] for arara");
                    $compiler = $var;
                }
            }
        }
        else {
            Log("Not detected engine for arara, use default [pdflatex]");
            $compiler = 'pdflatex';
        }
    }
    if ($compiler eq 'latex') { $compiler = 'pdflatex'; }
    if ($compiler eq 'dvilualatex') { $compiler = 'lualatex'; }
    # Compiling file
    RUNOSCMD("$compiler $write18 -interaction=batchmode", "$name-fig-$tmp$ext >$null", 'only');
    # Restore arara compiler
    if ($arara) { $compiler = 'arara'; }
    Log("Search graphicx package in $name-fig-$tmp.log");
    open my $LaTeXlog, '<', "$name-fig-$tmp.log";
        {
            local $/;
            $ltxlog = <$LaTeXlog>;
        }
    close $LaTeXlog;
    # Try to capture graphicx
    @graphicx = $ltxlog =~ m/.+? (graphicx\.sty)/xg;
    if (@graphicx) {
        Log("Found graphicx package in $name-fig-$tmp.log");
    }
    else {
        Log("Not found graphicx package in $name-fig-$tmp.log");
        Log("Add \\usepackage\{graphicx\} to preamble of $name-pdf$ext");
        $preamble= "$preamble\\usepackage\{graphicx\}\n";
    }
}

### Add last lines
if (!@graphicspath) {
    Log("Not found \\graphicspath in preamble for $name-pdf$ext");
    Log("Add \\graphicspath\{\{$imgdir/\}\} to preamble for $name-pdf$ext");
    $preamble= "$preamble\\graphicspath\{\{$imgdir/\}\}\n";
}

Log("Add \\usepackage\{grfext\} to preamble for $name-pdf$ext");
$preamble = "$preamble\\usepackage\{grfext\}\n";
Log("Add \\PrependGraphicsExtensions\*\{\.pdf\} to preamble for $name-pdf$ext");
$preamble = "$preamble\\PrependGraphicsExtensions\*\{\.pdf\}";
$preamble =~ s/\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/$1/gmsx;

### Create a <output file>
my $out_file = "$preamble\n$bodydoc\n$enddoc";

### Clean \psset{...} content in <output file>
Log("Remove \\psset\{...\} in body for $name-pdf$ext");
$out_file =~ s/\\begin\{nopreview\}\%$tmp.+?\\end\{nopreview\}\%$tmp(*SKIP)(*F)|
               \%<\*$dtxverb> .+? \%<\/$dtxverb>(*SKIP)(*F)|
               \\psset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))?+//gmsx;

### Revert preview environments
if (@env_preview) {
    Log("Revert \\begin{nopreview}\%TMP$tmp ... \\end{nopreview}\%TMP$tmp");
    $out_file =~ s/\\begin\{nopreview\}\%TMP$tmp
                      (?<code> .+?)
                     \\end\{nopreview\}\%TMP$tmp
                  /\\begin\{preview\}$+{code}\\end\{preview\}/gmsx;
}

$out_file =~ s/\\begin\{nopreview\}%$tmp
               (?<code>\\begin\{PSTexample\} .+? \\end\{PSTexample\})
               \\end\{nopreview\}%$tmp
              /$+{code}/gmsx;

### Remove internal mark for verbatim and verbatim write environments
$out_file =~ s/\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/$1/gmsx;
%replace  = (%changes_out,%tmpoutreplace);
$find     = join q{|}, map {quotemeta} sort { length $a <=> length $b } keys %replace;
$out_file =~ s/($find)/$replace{$1}/g;

### Write <output file>
if (-e "$name-pdf$ext") {
    Log("Rewriting the file $name-pdf$ext in $workdir");
    Infocolor('Warning', "The file [$name-pdf$ext] already exists and will be rewritten");
}
else{
    Infoline("Creating the file $name-pdf$ext");
    Log("Write the file $name-pdf$ext in $workdir");
}
open my $OUTfile, '>', "$name-pdf$ext";
    print {$OUTfile} $out_file;
close $OUTfile;

### Set compiler for process <output file>
$compiler = $xetex  ? 'xelatex'
          : $luatex ? 'lualatex'
          :           'pdflatex'
          ;

### Set options for latexmk
my $ltxmkopt = $xetex  ? "-pdfxe -silent -xelatex=\"xelatex $write18 -recorder %O %S\""
             : $luatex ? "-pdflua -silent -lualatex=\"lualatex $write18 -recorder %O %S\""
             :           "-pdf -silent -pdflatex=\"pdflatex $write18 -recorder %O %S\""
             ;

### Set options for compiler
$opt_compiler = $arara   ? '--log'
              : $latexmk ? "$ltxmkopt"
              :            "$write18 -interaction=nonstopmode -recorder"
              ;

### Set message in terminal
$msg_compiler = $xetex  ? 'xelatex'
              : $luatex ? 'lualatex'
              :           'pdflatex'
              ;

### Set compiler message and compiler for arara and latexmk
if ($arara) { $compiler = $msg_compiler = 'arara'; }
if ($latexmk) { $compiler = $msg_compiler = 'latexmk'; }

### Process <output file>
if (!$norun) {
    Log("Compiling the file $name-pdf$ext using [$msg_compiler]");
    print "Compiling the file $name-pdf$ext using ", color('magenta'), "[$msg_compiler]\r\n",color('reset');
    RUNOSCMD("$compiler $opt_compiler", "$name-pdf$ext",'show');
    # biber
    if ($runbiber && -e "$name-pdf.bcf" && !$arara && !$latexmk) {
        RUNOSCMD("biber", "$name-pdf",'show');
        RUNOSCMD("$compiler $opt_compiler", "$name-pdf$ext",'show');
    }
    # bibtex
    if ($runbibtex && -e "$name-pdf.aux" && !$arara && !$latexmk) {
        RUNOSCMD("bibtex", "$name-pdf",'show');
        RUNOSCMD("$compiler $opt_compiler", "$name-pdf$ext",'show');
    }
}

### Remove temporary files
my @tmpfiles;
my @protected = qw();
my $flsline = 'OUTPUT';
my @flsfile;

### Protect generated files
push @protected, "$name-pdf$ext", "$name-pdf.pdf";

### Find files
find(\&aux_files, $workdir);
sub aux_files{
    my $findtmpfiles = $_;
    if (-f $findtmpfiles && $findtmpfiles =~ m/$name-fig(-exa)?-$tmp.+?$/) { # search
        push @tmpfiles, $_;
    }
    return;
}

### Add if exists
if (-e 'arara.log') {
    push @flsfile, 'arara.log';
}
if (-e "$name-fig-$tmp.fls") {
    push @flsfile, "$name-fig-$tmp.fls";
}
if (-e "$name-fig-exa-$tmp.fls") {
    push @flsfile, "$name-fig-exa-$tmp.fls";
}
if (-e "$name-pdf.fls") {
    push @flsfile, "$name-pdf.fls";
}

### Read .fls file
for my $filename(@flsfile){
    open my $RECtmp, '<', $filename;
        push @tmpfiles, grep /^$flsline/,<$RECtmp>;
    close $RECtmp;
}

foreach (@tmpfiles) { s/^$flsline\s+|\s+$//g; }
push @tmpfiles, @flsfile;

@tmpfiles = uniq(@tmpfiles);
@tmpfiles = array_minus(@tmpfiles, @protected);

Log('The files that will be deleted are:');
Logarray(\@tmpfiles);

### Remove only if exist
if (@tmpfiles) {
    Infoline("Remove temporary files created in $workdir");
    foreach my $tmpfiles (@tmpfiles) {
        move($tmpfiles, $tempDir);
    }
}

### Find dirs created by minted
my @deldirs;
my $mintdir    = "\_minted\-$name-fig-$tmp";
my $mintdirexa = "\_minted\-$name-fig-exa-$tmp";
if (-e $mintdir) { push @deldirs, $mintdir; }
if (-e $mintdirexa) { push @deldirs, $mintdirexa; }

Log('The directory that will be deleted are:');
Logarray(\@deldirs);

### Remove only if exist
if (@deldirs) {
    Infoline("Remove temporary directories created by minted in $workdir");
    foreach my $deldirs (@deldirs) {
        remove_tree($deldirs);
    }
}

### Compress "./images" with generated files
my $archivetar;
if ($zip or $tar) {
    my $stamp = strftime("%Y-%m-%d", localtime);
    $archivetar = "$imgdir-$stamp";

    my @savetozt;
    find(\&zip_tar, $imgdir);
    sub zip_tar{
        my $filesto = $_;
        if (-f $filesto && $filesto =~ m/$name-fig-.+?$/) { # search
            push @savetozt, $File::Find::name;
        }
        return;
    }
    Log("The files are compress found in $imgdirpath are:");
    Logarray(\@savetozt);
    if ($zip) {
        if (-e "$archivetar.zip") {
            Infocolor('Warning', "The file [$archivetar.zip] already exists and will be rewritten");
            Log("Rewriting the file $archivetar.zip in $workdir");
        }
        else{
            print "Creating the file ", color('magenta'), "[$archivetar.zip]",
            color('reset'), " with generate files in ./$imgdir\r\n";
            Log("Writen the file $archivetar.tar.gz in $workdir");
        }
        zip \@savetozt => "$archivetar.zip";
    }
    if ($tar) {
        if (-e "$archivetar.tar.gz") {
            Infocolor('Warning', "The file [$archivetar.tar.gz] already exists and will be rewritten");
            Log("Rewriting the file $archivetar.tar.gz in $workdir");
        }
        else{
            print "Creating the file ", color('magenta'), "[$archivetar.tar.gz]",
            color('reset'), " with generate files in ./$imgdir\r\n";
            Log("Writen the file $archivetar.tar.gz in $workdir");
        }
        my $imgdirtar = Archive::Tar->new();
        $imgdirtar->add_files(@savetozt);
        $imgdirtar->write( "$archivetar.tar.gz" , 9 );
    }
}

### End of script process
if (!$norun && !$nosource) {
    Log("The image files: $format and generated files are in $imgdirpath");
}
if (!$norun && $nosource) {
    Log("The image files: $format are in $imgdirpath");
}
if ($norun && !$nosource) {
    Log("The generated files are in $imgdirpath");
}
Log("The output file $name-pdf$ext are in $workdir");

Infocolor('Finish', "The execution of $scriptname has been successfully completed");

Log("The execution of $scriptname has been successfully completed");

__END__
