#!/usr/bin/env perl
use v5.26;

############################# LICENCE ##################################
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

use Getopt::Long qw(:config bundling_values require_order no_ignore_case);
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

### Directory for work and temp files
my $tempDir = tempdir( CLEANUP => 1);
my $workdir = cwd;

### Script identification
my $scriptname = 'ltximg';
my $program    = 'LTXimg';
my $nv         = 'v1.9';
my $date       = '2020-08-22';
my $copyright  = <<"END_COPYRIGHT" ;
[$date] (c) 2013-2020 by Pablo Gonzalez, pablgonz<at>yahoo.com
END_COPYRIGHT

my $title = "$program $nv $copyright";

### Log vars
my $LogFile = "$scriptname.log";
my $LogWrite;
my $LogTime = strftime("%y/%m/%d %H:%M:%S", localtime);

### Default values
my $skiptag  = 'noltximg'; # internal tag for regex
my $extrtag  = 'ltximg';   # internal tag for regex
my @extr_env_tmp;          # save extract environments
my @skip_env_tmp;          # save skip environments
my @verb_env_tmp;          # save verbatim environments
my @verw_env_tmp;          # save verbatim write environments
my @delt_env_tmp;          # save delete environments in output file
my @clean;                 # clean document options
my $outfile  = 0;          # write output file
my $outsrc   = 0;          # write standalone files
my $PSTexa   = 0;          # run extract PSTexample environments
my $STDenv   = 0;          # run extract standart environments
my $verbose  = 0;          # verbose info
my $gscmd;                 # ghostscript executable name
my $write18;               # storing write18 for compiler in TeXLive and MikTeX
my $log      = 0;          # log file
my @currentopt;            # storing current options for log file

### Hash to store options for Getopt::Long and log file
my %opts_cmd;
$opts_cmd{string}{prefix} = 'fig';
$opts_cmd{string}{dpi}    = '150';
$opts_cmd{string}{runs}   = '1';
$opts_cmd{string}{margin} = '0';
$opts_cmd{string}{imgdir} = 'images';
$opts_cmd{string}{myverb} = 'myverb';
$opts_cmd{clean}          = 'doc';

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

### Help for command line
sub usage {
find_ghostscript();

my $usage = <<"END_OF_USAGE";
${title}Description
   LTXimg is a "perl" script that automates the process of extracting and
   converting "environments" provided by tikz, pstricks and other packages
   from LaTeX file to image formats and "standalone" files using ghostscript
   and poppler-utils. Generates a one file with only extracted environments
   and other with all extracted environments converted to \\includegraphics.

Syntax
\$ ltximg [<options>] [--] <filename>.<tex|ltx>

   If used without [<options>] the extracted environments are converted
   to pdf image format and saved in "./images" directory using pdflatex
   and preview package for process <filename>.

Default environments extract
   preview pspicture tikzpicture pgfpicture psgraph postscript PSTexample

Options
   Options that accept a value require either a blank space or = between
   the option and the value. Multiple short options can be bundling and
   if the last option takes a comma separated list you need -- at the end.
   Relative or absolute paths for directories and files is not supported.

                                                                    [default]
-h, --help            Display command line help and exit            [off]
-v, --version         Display current version ($nv) and exit       [off]
-V, --verbose         Verbose printing information                  [off]
-l, --log             Write .log file with debug information        [off]
-t, --tif             Create .tif files using ghostscript           [$gscmd]
-b, --bmp             Create .bmp files using ghostscript           [$gscmd]
-j, --jpg             Create .jpg files using ghostscript           [$gscmd]
-p, --png             Create .png files using ghostscript           [$gscmd]
-e, --eps             Create .eps files using poppler-utils         [pdftops]
-s, --svg             Create .svg files using poppler-utils         [pdftocairo]
-P, --ppm             Create .ppm files using poppler-utils         [pdftoppm]
-g, --gray            Gray scale for images using ghostscript       [off]
-f, --force           Capture "\\psset" and "\\tikzset" to extract    [off]
-n, --noprew          Create images files without "preview" package [off]
-r <integer>, --runs <integer>
                      Set the number of times the compiler will run
                      on the input file for environment extraction  [1]
-d <integer>, --dpi <integer>
                      Dots per inch resolution for images           [150]
-m <integer>, --margins <integer>
                      Set margins in bp for pdfcrop                 [0]
-o <filename>, --output <filename>
                      Create output file                            [off]
--imgdir <dirname>    Set name of directory to save images/files    [images]
--prefix <string>     Set prefix append to each generated files     [fig]
--myverb <macroname>  Add "\\macroname" to verbatim inline search    [myverb]
--clean (doc|pst|tkz|all|off)
                      Removes specific block text in output file    [doc]
--zip                 Compress files generated in .zip              [off]
--tar                 Compress files generated in .tar.gz           [off]
--srcenv              Create files with only code of environments   [off]
--subenv              Create standalone files for environments      [off]
--shell               Enable \\write18\{SHELL COMMAND\}              [off]
--latex               Using latex>dvips>ps2pdf for compiler input
                      and pdflatex for compiler output              [off]
--dvips               Using latex>dvips>ps2pdf for compiler input
                      and latex>dvips>ps2pdf for compiler output    [off]
--dvilua              Using dvilualatex>dvips>ps2pdf for compiler
                      input and lualatex for compiler output        [off]
--dvipdf              Using latex>dvipdfmx for compiler input and
                      latex>dvipdfmx for compiler output            [off]
--xetex               Using xelatex for compiler input and output   [off]
--luatex              Using lualatex for compiler input and output  [off]
--arara               Use arara for compiler input and output       [off]
--latexmk             Using latexmk for compiler output file        [off]
--norun               Run script, but no create images files        [off]
--nopdf               Don't create a ".pdf" image files             [off]
--nocrop              Don't run pdfcrop                             [off]
--extrenv <env1,...>  Add new environments to extract               [empty]
--skipenv <env1,...>  Skip some default environments to extract     [empty]
--verbenv <env1,...>  Add new verbatim environments                 [empty]
--writenv <env1,...>  Add new verbatim write environments           [empty]
--deltenv <env1,...>  Delete environments in output file            [empty]

Example
\$ ltximg --latex -e -p --subenv --imgdir mypics -o test-out test-in.ltx

   Create a "./mypics" directory (if it doesn't exist) with all extracted
   environments converted to images (.pdf, .eps, .png) and standalone files
   (.ltx), a file "test-in-fig-all.ltx" with all extracted environments and
   the file "test-out.ltx" with all environments converted to \\includegraphics
   using latex>dvips>ps2pdf and preview package for <input file> and pdflatex
   for <output file>.

Documentation
For full documentation use:
\$ texdoc ltximg

Issues and reports
Repository   : https://github.com/pablgonz/ltximg
Bug tracker  : https://github.com/pablgonz/ltximg/issues
END_OF_USAGE
print $usage;
exit 0;
}

### Getopt configuration
my $result=GetOptions (
# image options
    'b|bmp'          => \$opts_cmd{image}{bmp}, # gs
    't|tif'          => \$opts_cmd{image}{tif}, # gs
    'j|jpg'          => \$opts_cmd{image}{jpg}, # gs
    'p|png'          => \$opts_cmd{image}{png}, # gs
    's|svg'          => \$opts_cmd{image}{svg}, # pdftocairo
    'e|eps'          => \$opts_cmd{image}{eps}, # pdftops
    'P|ppm'          => \$opts_cmd{image}{ppm}, # pdftoppm
# compilers
    'arara'          => \$opts_cmd{compiler}{arara},   # arara compiler
    'xetex'          => \$opts_cmd{compiler}{xetex},   # xelatex compiler
    'latex'          => \$opts_cmd{compiler}{latex},   # latex compiler
    'latexmk'        => \$opts_cmd{compiler}{latexmk}, # latex compiler
    'dvips'          => \$opts_cmd{compiler}{dvips},   # dvips compiler
    'dvipdf'         => \$opts_cmd{compiler}{dvipdf},  # dvipdfmx compiler
    'dvilua'         => \$opts_cmd{compiler}{dvilua},  # dvilualatex compiler
    'luatex'         => \$opts_cmd{compiler}{luatex},  # lualatex compiler
# bolean
    'zip'            => \$opts_cmd{boolean}{zip},    # zip images dir
    'tar'            => \$opts_cmd{boolean}{tar},    # tar images dir
    'shell'          => \$opts_cmd{boolean}{shell},  # set write18 for compiler
    'nopdf'          => \$opts_cmd{boolean}{nopdf},  # no pdf image format
    'norun'          => \$opts_cmd{boolean}{norun},  # no run compiler
    'nocrop'         => \$opts_cmd{boolean}{nocrop}, # no run pdfcrop
    'subenv'         => \$opts_cmd{boolean}{subenv}, # subfile environments (bolean)
    'srcenv'         => \$opts_cmd{boolean}{srcenv}, # source files (bolean)
    'g|gray'         => \$opts_cmd{boolean}{gray},   # gray (boolean)
    'f|force'        => \$opts_cmd{boolean}{force},  # force (boolean)
    'n|noprew'       => \$opts_cmd{boolean}{noprew}, # no preview (boolean)
# string
    'd|dpi=i'        => \$opts_cmd{string}{dpi},     # positive integer < 2500
    'r|runs=i'       => \$opts_cmd{string}{runs},    # positive integer 1,2,3
    'm|margins=i'    => \$opts_cmd{string}{margins}, # integer
    'extrenv=s{1,9}' => \@extr_env_tmp, # extract environments
    'skipenv=s{1,9}' => \@skip_env_tmp, # skip environments
    'verbenv=s{1,9}' => \@verb_env_tmp, # verbatim environments
    'writenv=s{1,9}' => \@verw_env_tmp, # verbatim write environments
    'deltenv=s{1,9}' => \@delt_env_tmp, # delete environments
    'o|output=s{1}'  => \$opts_cmd{string}{output}, # output file name (string)
    'imgdir=s{1}'    => \$opts_cmd{string}{imgdir}, # images dir name
    'myverb=s{1}'    => \$opts_cmd{string}{myverb}, # \myverb inline (string)
    'prefix=s{1}'    => \$opts_cmd{string}{prefix}, # prefix
    'clean=s{1}'     => \$opts_cmd{clean},          # clean output file
# internal
    'h|help'         => \$opts_cmd{internal}{help},    # help
    'v|version'      => \$opts_cmd{internal}{version}, # version
    'l|log'          => \$log,     # write log file
    'V|verbose'      => \$verbose, # verbose mode
    ) or do { $log = 0 ; die usage(0); };

### Open log file
if ($log) {
    if (!defined $ARGV[0]) { errorUsage('Input filename missing'); }
    my $tempname = $ARGV[0];
    $tempname =~ s/\.(tex|ltx)$//;
    if ($LogFile eq "$tempname.log") { $LogFile = "$scriptname-log.log"; }
    $LogWrite  = FileHandle->new("> $LogFile");
}

### Init ltximg.log file
Log("The script $scriptname $nv was started in $workdir");
Log("Creating the temporary directory $tempDir");

### Make ENV safer (perldoc perlsec)
delete @ENV{qw(IFS CDPATH ENV BASH_ENV)};

### The next code it's part of pdfcrop (adapted from TexLive 2014)
# Windows detection
my $Win = 0;
if ($^O =~ /mswin32/i) { $Win = 1; }

my $archname = $Config{'archname'};
$archname = 'unknown' unless defined $Config{'archname'};

# Get ghostscript command name
sub find_ghostscript {
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
if (defined $opts_cmd{internal}{help}) {
    usage(1);
    exit 0;
}

### Version
if (defined $opts_cmd{internal}{version}) {
    print $title;
    exit 0;
}

### Check <input file> from command line
@ARGV > 0 or errorUsage('Input filename missing');
@ARGV < 2 or errorUsage('Unknown option or too many input files');

### Check <input file> extention
my @SuffixList = ('.tex', '.ltx');
my ($name, $path, $ext) = fileparse($ARGV[0], @SuffixList);
if ($ext eq '.tex' or $ext eq '.ltx') {
    $ext = $ext;
}
else {
    errorUsage('Invalid or empty extention for input file');
}

### Read <input file> in memory, need [:crlf] for old windows files
Log("Read input file $name$ext in memory");
open my $INPUTfile, '<:crlf', "$name$ext";
    my $ltxfile;
        {
            local $/;
            $ltxfile = <$INPUTfile>;
        }
close $INPUTfile;

### Set tmp random number for <name-fig-tmp> and others
my $tmp = int(rand(10000));

### Identification message in terminal
print $title;

### Remove white space and '=' in array captured from command line
s/^\s*(\=):?|\s*//mg foreach @extr_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @skip_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @verb_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @verw_env_tmp;
s/^\s*(\=):?|\s*//mg foreach @delt_env_tmp;

### Split comma separte list options from command line
@extr_env_tmp = split /,/,join q{},@extr_env_tmp;
@skip_env_tmp = split /,/,join q{},@skip_env_tmp;
@verb_env_tmp = split /,/,join q{},@verb_env_tmp;
@verw_env_tmp = split /,/,join q{},@verw_env_tmp;
@delt_env_tmp = split /,/,join q{},@delt_env_tmp;

### Validate environments options from comand line
if (grep /(^\-|^\.).*?/, @extr_env_tmp) {
    Log('Error!!: Invalid argument for --extrenv, some argument from list begin with -');
    errorUsage('Invalid argument for --extrenv option');
}
if (grep /(^\-|^\.).*?/, @skip_env_tmp) {
    Log('Error!!: Invalid argument for --skipenv, some argument from list begin with -');
    errorUsage('Invalid argument for --skipenv option');
}
if (grep /(^\-|^\.).*?/, @verb_env_tmp) {
    Log('Error!!: Invalid argument for --verbenv, some argument from list begin with -');
    errorUsage('Invalid argument for --verbenv option');
}
if (grep /(^\-|^\.).*?/, @verw_env_tmp) {
    Log('Error!!: Invalid argument for --writenv, some argument from list begin with -');
    errorUsage('Invalid argument for --writenv option');
}
if (grep /(^\-|^\.).*?/, @delt_env_tmp) {
    Log('Error!!: Invalid argument for --deltenv, some argument from list begin with -');
    errorUsage('Invalid argument for --deltenv option');
}

### Default environment to extract
my @extr_tmp = qw (
    preview postscript tikzpicture pgfpicture pspicture psgraph PSTexample
    );
push @extr_env_tmp, @extr_tmp;

### Default verbatim environment
my @verb_tmp = qw (
    Example CenterExample SideBySideExample PCenterExample PSideBySideExample
    verbatim Verbatim BVerbatim LVerbatim SaveVerbatim PSTcode
    LTXexample tcblisting spverbatim minted listing lstlisting
    alltt comment chklisting verbatimtab listingcont boxedverbatim
    demo sourcecode xcomment pygmented pyglist program programl
    programL programs programf programsc programt
    );
push @verb_env_tmp, @verb_tmp;

### Default verbatim write environment
my @verbw_tmp = qw (
    scontents filecontents tcboutputlisting tcbexternal tcbwritetmp extcolorbox extikzpicture
    VerbatimOut verbatimwrite filecontentsdef filecontentshere filecontentsdefmacro
    filecontentsdefstarred filecontentsgdef filecontentsdefmacro filecontentsgdefmacro
    );
push @verw_env_tmp, @verbw_tmp;

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

### Changes in <input file> for verbatim write and comment lines
while ($ltxfile =~ / $tmp_verbw | $tmpcomment /pgmx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my  $encontrado = ${^MATCH};
        while (my($busco, $cambio) = each %document) {
            $encontrado =~ s/\Q$busco\E/$cambio/g;
        }
        substr $ltxfile, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($ltxfile) = $pos_inicial + length $encontrado;
}

### Now, split input file in $atbegindoc and contain % ltximg : <argument>
my ($atbegindoc, $document) = $ltxfile =~ m/\A (\s* .*? \s*) (\\documentclass.*)\z/msx;

### Capture options in preamble of <input file>
# % ltximg : extrenv : {extrenv1, extrenv2, ... , extrenvn}
# % ltximg : skipenv : {skipenv1, skipenv2, ... , skipenvn}
# % ltximg : verbenv : {verbwrt1, verbwrt2, ... , verbwrtn}
# % ltximg : writenv : {verbwrt1, verbwrt2, ... , verbwrtn}
# % ltximg : deltenv : {deltenv1, deltenv2, ... , deltenvn}
# % ltximg : options : {opt1=arg, opt2=arg, ... , booleans}

my $readoptfile = qr/
    ^ %+ \s* ltximg (?&SEPARADOR) (?<clave>(?&CLAVE)) (?&SEPARADOR) \{ (?<argumentos>(?&ARGUMENTOS)) \}
    (?(DEFINE)
    (?<CLAVE>      \w+       )
    (?<ARGUMENTOS> .+?       )
    (?<SEPARADOR>  \s* : \s* )
    )
/mx;

### Search options in <input file> and store in %opts_file
my %opts_file;
while ($atbegindoc =~ /$readoptfile/g) {
    my ($clave, $argumentos) = @+{qw(clave argumentos)};
    my  @argumentos = split /\s*,\s*?/, $argumentos;
    for (@argumentos) { s/^ \s* | \s* $//gx; }
        if  ($clave eq 'options') {
            for my $argumento (@argumentos) {
                if ($argumento =~ /(?<key>\S+) \s* = \s* (?<valor>\S+)/x) {
                    $opts_file{$clave}{$+{'key'}} = $+{'valor'};
                }
                else {
                    $opts_file{$clave}{$argumento} = 1;
                }
            }
        }
        else {
            push @{ $opts_file{ $clave } }, @argumentos;
    }
}

### Process options from <input file>
if(%opts_file) {
    Infocolor('Warning', "Found options for script in $name$ext");
    # Search and extract options from <input file>
    Log("Searching options for script in $name$ext");
    if (exists $opts_file{extrenv}) {
        Log("Found \% ltximg\: extrenv\: \{...\} in $name$ext");
        if (grep /(^\-|^\.).*?/, @{$opts_file{extrenv}}) {
            Log('Error!!: Invalid argument for % ltximg: extrenv: {...}, some argument from list begin with -');
            errorUsage('Invalid argument in % ltximg: extrenv: {...}');
        }
        Logarray(\@{$opts_file{extrenv}});
        push @extr_env_tmp, @{$opts_file{extrenv}};
    }
    # Add skipenv options from <input file>
    if (exists $opts_file{skipenv}) {
        Log("Found \% ltximg\: skipenv\: \{...\} in $name$ext");
        if (grep /(^\-|^\.).*?/, @{$opts_file{skipenv}}) {
            Log('Error!!: Invalid argument for % ltximg: skipenv: {...}, some argument from list begin with -');
            errorUsage('Invalid argument in % ltximg: skipenv: {...}');
        }
        Logarray(\@{$opts_file{skipenv}});
        push @skip_env_tmp, @{$opts_file{skipenv}};
    }
    # Add verbenv options from <input file>
    if (exists $opts_file{verbenv}) {
        Log("Found \% ltximg\: verbenv\: \{...} in $name$ext");
        if (grep /(^\-|^\.).*?/, @{$opts_file{verbenv}}) {
            Log('Error!!: Invalid argument for % ltximg: verbenv: {...}, some argument from list begin with -');
            errorUsage('Invalid argument in % ltximg: verbenv: {...}');
        }
        Logarray(\@{ $opts_file{verbenv}});
        push @verb_env_tmp, @{$opts_file{verbenv}};
    }
    # Add writenv options from <input file>
    if (exists $opts_file{writenv}) {
        Log("Found \% ltximg\: writenv\: \{...\} in $name$ext");
        if (grep /(^\-|^\.).*?/, @{ $opts_file{writenv}}) {
            Log('Error!!: Invalid argument for % ltximg: writenv: {...}, some argument from list begin with -');
            errorUsage('Invalid argument in % ltximg: writenv: {...}');
        }
        Logarray(\@{ $opts_file{writenv}});
        push @verw_env_tmp, @{$opts_file{writenv}};
    }
    # Add deltenv options from <input file>
    if (exists $opts_file{deltenv}) {
        Log("Found \% ltximg\: deltenv\: \{...\} in $name$ext");
        if (grep /(^\-|^\.).*?/, @{$opts_file{deltenv}}) {
            Log('Error!!: Invalid argument for % ltximg: deltenv: {...}, some argument from list begin with -');
            errorUsage('Invalid argument in % ltximg: deltenv: {...}');
        }
        Logarray(\@{ $opts_file{deltenv}});
        push @delt_env_tmp, @{$opts_file{deltenv}};
    }
    # Add all other options from <input file>
    if (exists $opts_file{options}) {
        Log("Found \% ltximg\: options\: \{...\} in $name$ext");
        # Add compilers from <input file>
        for my $opt (qw(arara xetex luatex latex dvips dvipdf dvilua latexmk)) {
            if (exists $opts_file{options}{$opt}) {
                Log("Found [$opt] compiler option in $name$ext");
                $opts_cmd{compiler}{$opt} = 1;
            }
        }
        # Add image options
        for my $opt (qw(eps ppm svg png jpg bmp tif)) {
            if (exists $opts_file{options}{$opt}) {
                Log("Found [$opt] image option in $name$ext");
                $opts_cmd{image}{$opt} = 1;
            }
        }
        # Add boolean options
        for my $opt (qw(shell nopdf norun nocrop srcenv subenv zip tar gray force noprew)) {
            if (exists $opts_file{options}{$opt}) {
                Log("Found [$opt] option in $name$ext");
                $opts_cmd{boolean}{$opt} = 1;
            }
        }
        # Add string options
        for my $opt (qw(dpi myverb margins prefix imgdir output runs)) {
            if (exists $opts_file{options}{$opt}) {
                Log("Found [$opt = $opts_file{options}{$opt}] in $name$ext");
                $opts_cmd{string}{$opt} = $opts_file{options}{$opt};
            }
        }
        # Add clean option
        for my $opt (qw(doc off pst tkz all)) {
            if ($opts_file{options}{clean} eq "$opt" ) {
                Log("Found [clean = $opt] in $name$ext");
                $opts_cmd{clean} = $opt;
            }
        }
    }
}

### Validate --dpi
if( $opts_cmd{string}{dpi} <= 0 or $opts_cmd{string}{dpi} >= 2500) {
    Log('Error!!: Invalid argument for --dpi, argument out of range');
    errorUsage('Invalid argument for --dpi option');
}

### Validate --runs
if( $opts_cmd{string}{runs} <= 0 or $opts_cmd{string}{runs} >= 3) {
    Log('Error!!: Invalid argument for --runs, argument out of range');
    errorUsage('Invalid argument for --runs option');
}

### Check --arara and others compilers
if ($opts_cmd{compiler}{arara}) {
    # Search others compilers options
    for my $opt (qw(xetex luatex latex dvips dvipdf dvilua latexmk)) {
        if (defined $opts_cmd{compiler}{$opt}) {
            Log("Error!!: Options --arara and --$opt are mutually exclusive");
            errorUsage("Options --arara and --$opt are mutually exclusive");
        }
    }
}

### Check --srcenv and --subenv option
if ($opts_cmd{boolean}{srcenv} && $opts_cmd{boolean}{subenv}) {
    Log('Error!!: Options --srcenv and --subenv  are mutually exclusive');
    errorUsage('Options --srcenv and --subenv  are mutually exclusive');
}

### If --srcenv or --subenv option are OK activate write <sub files>
if ($opts_cmd{boolean}{srcenv}) {
    $outsrc = 1;
    $opts_cmd{boolean}{subenv} = undef;
}
if ($opts_cmd{boolean}{subenv}) {
    $outsrc = 1;
    $opts_cmd{boolean}{srcenv} = undef;
}

### Add pdf image format if --nopdf
if (!$opts_cmd{boolean}{nopdf}) {
    $opts_cmd{image}{pdf} = 1;
}

### Store defined image formats in $format
my $format = join q{, },grep { defined $opts_cmd{image}{$_} } keys %{$opts_cmd{image}};

### Write defined image format in log file
if (!$opts_cmd{boolean}{norun}) {
    Log("Defined image formats for creating: $format");
}

### Check --norun and no images type
if (!$opts_cmd{boolean}{norun} and $format eq q{}) {
    Log('Error!!: Option --nopdf need --norun or an image option');
    errorUsage('Option --nopdf need --norun or an image option');
}

### Check --dvips and no eps image type
if ($opts_cmd{compiler}{dvips} and !$opts_cmd{image}{eps} and !$opts_cmd{boolean}{norun}) {
    Log('Error!!: Option --dvips need --eps');
    errorUsage('Option --dvips need --eps');
}

### Validate myverb = macro option
if (defined $opts_cmd{string}{myverb}) {
    if ($opts_cmd{string}{myverb} =~ /^(?:\\|\-).+?/) {
        Log('Error!!: Invalid argument for --myverb option, argument begin with - or \ ');
        errorUsage('Invalid argument for --myverb');
    }
    else { Log("Set myverb = $opts_cmd{string}{myverb}"); }
}

### Validate imgdir = string option
if (defined $opts_cmd{string}{imgdir}) {
    if ($opts_cmd{string}{imgdir} =~ /^(?:\\|\-).+?/) {
        Log('Error!!: Invalid argument for --imgdir option, argument begin with -, \ or /');
        errorUsage('Invalid argument for --imgdir');
    }
    else { Log("Set imgdir = $opts_cmd{string}{imgdir}"); }
}

### Validate clean
my %clean = map { $_ => 1 } @clean;
$clean{doc} = 1; # by default clean = doc

### Pass $opts_cmd{clean} to $clean{$opt}
for my $opt (qw(doc off pst tkz all)) {
    if ($opts_cmd{clean} eq "$opt") {
        $clean{$opt} = 1;
        push @currentopt, "--clean=$opt";
    }
}

### Activate clean options for script
if ($clean{pst} or $clean{tkz}) { $clean{doc} = 1; }
if ($clean{all}) { @clean{qw(pst doc tkz)} = (1) x 3; }
if ($clean{off}) { undef %clean; }

### Validating the <output file> name and save extension
my $outext;
if (defined $opts_cmd{string}{output}) {
    Log('Validating name and extension for output file');
    # Capture and split
    my ($outname, $outpath, $tmpext) = fileparse($opts_cmd{string}{output}, @SuffixList);
    if ($outname =~ /(^\-|^\.).*?/) {
        Log('The name of output file begin with dash -');
        errorUsage("$opts_cmd{string}{output} it is not a valid name for output file");
    }
    if ($tmpext eq q{}) { # Check and set extension
        Log("Set extension for output file to $ext");
        $outext = $ext;
    }
    else {
        Log("Set extension for output file to $tmpext");
        $outext = $tmpext;
    }
    if ($outname eq $name) { # Check name
        Log("The name of the output file must be different that $name");
        Infoline("Changing the output file name to $name-out");
        $opts_cmd{string}{output} = "$name-out";
    }
    else {
        Log("Set name of the output file to $outname");
        $opts_cmd{string}{output} = $outname;
    }
    # If output name are ok, then $outfile = 1
    $outfile = 1;
}

### Storing the current options of script in array for ltximg.log file
my @allopt = qw (arara xetex luatex latex dvips dvipdf dvilua latexmk
    shell nopdf norun nocrop srcenv subenv zip tar gray force
    noprew eps ppm svg png jpg bmp tif dpi myverb margins
    prefix imgdir output runs
    );

for my $opt (@allopt) {
    if (defined $opts_cmd{boolean}{$opt}) {
        push @currentopt, "--$opt";
    }
    if (defined $opts_cmd{compiler}{$opt}) {
        push @currentopt, "--$opt";
    }
    if (defined $opts_cmd{image}{$opt}) {
        push @currentopt, "--$opt";
    }
    if (defined $opts_cmd{string}{$opt}) {
        push @currentopt, "--$opt=$opts_cmd{string}{$opt}"
    }
}

### Write all options in ltximg.log file
@currentopt = sort { length $a <=> length $b } @currentopt;
Log('The script will execute the following options:');
Logarray(\@currentopt);

### Rules to capture for regex
my $braces      = qr/ (?:\{)(.+?)(?:\}) /msx;
my $braquet     = qr/ (?:\[)(.+?)(?:\]) /msx;
my $no_corchete = qr/ (?:\[ .*? \])?    /msx;

### Array for capture new verbatim environments defined in input file
my @new_verb = qw (
    newtcblisting DeclareTCBListing ProvideTCBListing NewTCBListing
    lstnewenvironment NewListingEnvironment NewProgram specialcomment
    includecomment DefineVerbatimEnvironment newverbatim newtabverbatim
    );

### Regex to capture names for new verbatim environments from input file
my $newverbenv = join q{|}, map { quotemeta} sort { length $a <=> length $b } @new_verb;
$newverbenv = qr/\b(?:$newverbenv) $no_corchete $braces/msx;

### Array for capture new verbatim write environments defined in input file
my @new_verb_write = qw (
    renewtcbexternalizetcolorbox renewtcbexternalizeenvironment
    newtcbexternalizeenvironment newtcbexternalizetcolorbox newenvsc
    );

### Regex to capture names for new verbatim write environments from input file
my $newverbwrt = join q{|}, map { quotemeta} sort { length $a <=> length $b } @new_verb_write;
$newverbwrt = qr/\b(?:$newverbwrt) $no_corchete $braces/msx;

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
my @newv_write = $filecheck =~ m/$newverbwrt/xg;
if (@newv_write) {
    Log("Found new verbatim write environments in $name$ext");
    Logarray(\@newv_write);
    push @verw_env_tmp, @newv_write;
}

### Search new verbatim environments in <input file> (for)
my @verb_input = $filecheck =~ m/$newverbenv/xg;
if (@verb_input) {
    Log("Found new verbatim environments in $name$ext");
    Logarray(\@verb_input);
    push @verb_env_tmp, @verb_input;
}

### Search \newminted{$mintdenv}{options} in <input file>, need add "code" (for)
my @mint_denv = $filecheck =~ m/$mintdenv/xg;
if (@mint_denv) {
    Log("Found \\newminted\{envname\} in $name$ext");
    # Append "code"
    $mintdenv  = join "\n", map { qq/$_\Qcode\E/ } @mint_denv;
    @mint_denv = split /\n/, $mintdenv;
    Logarray(\@mint_denv);
    push @verb_env_tmp, @mint_denv;
}

### Search \newminted[$mintcenv]{lang} in <input file> (for)
my @mint_cenv = $filecheck =~ m/$mintcenv/xg;
if (@mint_cenv) {
    Log("Found \\newminted\[envname\] in $name$ext");
    Logarray(\@mint_cenv);
    push @verb_env_tmp, @mint_cenv;
}

### Remove repetead again :)
@verb_env_tmp = uniq(@verb_env_tmp);

### Capture verbatim inline macros in input file
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
    '%<*ltximg>'      => '%<*LTXIMG>',
    '%</ltximg>'      => '%</LTXIMG>',
    '%<*noltximg>'    => '%<*NOLTXIMG>',
    '%</noltximg>'    => '%</NOLTXIMG>',
    '%<*remove>'      => '%<*REMOVE>',
    '%</remove>'      => '%</REMOVE>',
    '\psset'          => '\PSSET',
    '\tikzset'        => '\TIKZSET',
    '\pspicture'      => '\TRICKS',
    '\endpspicture'   => '\ENDTRICKS',
    '\pgfpicture'     => '\PGFTRICKS',
    '\endpgfpicture'  => '\ENDPGFTRICKS',
    '\tikzpicture'    => '\TKZTRICKS',
    '\endtikzpicture' => '\ENDTKZTRICKS',
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
my $verb        = qr/(?:((spv|(?:q|f)?v|V)erb|$opts_cmd{string}{myverb})[*]?)           /ix;
my $lst         = qr/(?:(lst|pyg)inline)(?!\*) $no_corchete                             /ix;
my $mint        = qr/(?: $mintline |SaveVerb) (?!\*) $no_corchete $no_llaves $llaves    /ix;
my $no_mint     = qr/(?: $mintline) (?!\*) $no_corchete                                 /ix;
my $marca       = qr/\\ (?:$verb | $lst |$scontents | $mint |$no_mint) (?:\s*)? (\S) .+? \g{-1}     /sx;
my $comentario  = qr/^ \s* \%+ .+? $                                                    /mx;
my $definedel   = qr/\\ (?: DefineShortVerb | lstMakeShortInline| MakeSpecialShortVerb ) [*]? $no_corchete $delimitador /ix;
my $indefinedel = qr/\\ (?: (Undefine|Delete)ShortVerb | lstDeleteShortInline) $llaves  /ix;

Log('Making changes to inline/multiline verbatim before extraction');

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
my $tcbxverb   = qr /\\ (?: tcboxverb [*]?| Scontents [*]? |$opts_cmd{string}{myverb} [*]?|lstinline) $no_corchete /x;
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

### Reverse changes for extract and output file
my %changes_out = (
    '\PSSET'            => '\psset',
    '\TIKZSET'          => '\tikzset',
    '\TRICKS'           => '\pspicture',
    '\ENDTRICKS'        => '\endpspicture',
    '\PGFTRICKS'        => '\pgfpicture',
    '\ENDPGFTRICKS'     => '\endpgfpicture',
    '\TKZTRICKS'        => '\tikzpicture',
    '\ENDTKZTRICKS'     => '\endtikzpicture',
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
    );

### Reverse tags, need back in all file to extract
my %reverse_tag = (
    '%<*LTXIMG>'   => '%<*ltximg>',
    '%</LTXIMG>'   => '%</ltximg>',
    '%<*NOLTXIMG>' => '%<*noltximg>',
    '%</NOLTXIMG>' => '%</noltximg>',
    '%<*REMOVE>'   => '%<*remove>',
    '%</REMOVE>'   => '%</remove>',
    );

### First we do some security checks to ensure that they are verbatim and
### verbatim write environments are unique and disjointed
@verb_env_tmp = array_minus(@verb_env_tmp, @verw_env_tmp); #disjointed
my @verbatim = uniq(@verb_env_tmp);
my %verbatim = crearhash(@verbatim);

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
my %verbatim_w = crearhash(@verbatim_w);

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

### Check skip environments
my @skipped;
if (@skip_env_tmp) {
    my %envcheck = map { $_ => 1 } @skip_env_tmp;
    if (exists $envcheck{PSTexample}) {
        Log('The [PSTexample] environment is not allowed for skipping');
        @skip_env_tmp = grep !/PSTexample/, @skip_env_tmp;
    }
    if (exists $envcheck{preview}) {
        Log('The [preview] environment is not allowed for skipping');
        @skip_env_tmp = grep !/preview/, @skip_env_tmp;
    }
    # Valid environments for skip
    for my $skip (qw(pspicture tikzpicture pgfpicture psgraph postscript)) {
        if (exists $envcheck{$skip}) {
            push @skipped,$skip;
        }
    }
}

### List of environments to be skiped from extraction
@skipped = uniq(@skip_env_tmp);
Log('The environments that will be skiped for extraction:');
Logarray(\@skipped);

### Create a Regex for skip environment
my $skipenv = join q{|}, map { quotemeta } sort { length $a <=> length $b } @skipped;
$skipenv = qr/$skipenv/x;
my $skip_env = qr {
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
                  }x;

### Check reserved environments
my %envcheck = map { $_ => 1 } @extr_env_tmp;

if (exists $envcheck{document}) {
    Log('The [document] environment is allowed for extraction');
    @extr_env_tmp = grep !/document/, @extr_env_tmp;
}
if (exists $envcheck{nopreview}) {
    Log('The [nopreview] environment is allowed for extraction');
    @extr_env_tmp = grep !/nopreview/, @extr_env_tmp;
}

### An array with all environments to extract, including nopreview
my @extract_env = qw(nopreview);
push @extract_env,@extr_env_tmp;

### Some oprations for skip environments
@extract_env = array_minus(@extract_env, @skip_env_tmp);
@extract_env = uniq(@extract_env);

### Hash for regex
my %extract_env = crearhash(@extract_env);

Log('The environments that will be searched for extraction:');
my @real_extract_env = grep !/nopreview/, @extract_env;
Logarray(\@real_extract_env);

### Create a regex to extract environments
my $environ = join q{|}, map { quotemeta } sort { length $a <=> length $b } @extract_env;
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

### Check delete environments
if (@delt_env_tmp) {
    my %envcheck = map { $_ => 1 } @delt_env_tmp;
    if (exists $envcheck{document}) {
        Log('The [document] environment is not allowed for delete');
        @delt_env_tmp = grep !/document/, @delt_env_tmp;
    }
}

### An array of environments to be removed in <output file>
my @delete_env = uniq(@delt_env_tmp);
my %delete_env = crearhash(@delete_env);

Log('The environments that will be removed in output file:');
Logarray(\@delete_env);

### Create a Regex for delete environment in <output file>
my $delenv = join q{|}, map { quotemeta } sort { length $a <=> length $b } @delete_env;
$delenv = qr/$delenv/x;
my $delt_env = qr {
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
                  }x;


########################################################################
# In this first part the script only detects verbatim environments and #
# verbatim write don't distinguish between which ones are extracted,   #
# that's done in a second pass.                                        #
########################################################################

Log('Making changes to verbatim and verbatim write environments before extraction');

### First, revert %<*TAGS> to %<*tags> in all document
my $ltxtags = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %reverse_tag;
$document =~ s/^($ltxtags)/$reverse_tag{$1}/gmsx;

### Create an array with the temporary extraction list, no verbatim environments
my @extract_tmp = array_minus(@extract_env, @verb_env_tmp);
@extract_tmp = array_minus(@extract_tmp, @verw_env_tmp);
@extract_tmp = uniq(@extract_tmp);
my %extract_tmp = crearhash(@extract_tmp);

### Hash and Regex for changes, this "regex" is re-used in ALL script
my %replace = (%verbatim, %extract_tmp, %changes_in, %document); # revert tags again :)
my $find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;

### We go line by line and make the changes in verbatim's envoronments (/p for ${^MATCH})
while ($document =~ /$verb_wrt | $verb_std /pgmx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my $encontrado = ${^MATCH};
    if ($encontrado =~ /$verb_wrt/) {
        $encontrado =~ s/($find)/$replace{$1}/g;
        substr $document, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($document) = $pos_inicial + length $encontrado;
    }
    if ($encontrado =~ /$verb_std/) {
        %replace = (%verbatim_w, %extract_tmp, %changes_in, %document);
        $find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;
        $encontrado =~ s/($find)/$replace{$1}/g;
        substr $document, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($document) = $pos_inicial + length $encontrado;
    }
}

### Now split document
my ($preamble,$bodydoc,$enddoc) = $document =~ m/\A (.+?) (\\begin\{document\} .+?)(\\end\{document\}.*)\z/msx;

### Internal <dtxtag> for mark verbatim and verbatim write environments
my $dtxverb = "verbatim$tmp";

### Match <dtxtags>, if they're matched, we turn them :)
my @tag_extract   = $bodydoc =~ m/(?:^\%<\*ltximg>.+?\%<\/ltximg>)/gmsx;
my @tag_noextract = $bodydoc =~ m/(?:^\%<\*noltximg>.+?\%<\/noltximg>)/gmsx;

if (@tag_extract) {
    Log('Pass extract tags %<*ltximg> ... %</ltximg> to \begin{preview} ... \end{preview}');
    $bodydoc =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                  \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
    ^\%<\*$extrtag>(.+?)\%<\/$extrtag>/\\begin\{preview\}$1\\end\{preview\}/gmsx;
}
if (@tag_noextract) {
    Log('Pass no extract tags %<*noltximg> ... %</noltximg> to \begin{nopreview} ... \end{nopreview}');
    $bodydoc =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                  \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
    ^\%<\*$skiptag>(.+?)\%<\/$skiptag>/\\begin\{nopreview\}$1\\end\{nopreview\}/gmsx;
}

########################################################################
# We now make the real changes for environment extraction. Since we    #
# don't know what kind of environments are passed, need to redefine    #
# all regex to make the changes.                                       #
########################################################################

my @new_verb_tmp = array_minus(@verbatim, @extract_env);
$verbatim = join q{|}, map { quotemeta } sort { length $a <=> length $b } @new_verb_tmp;
$verbatim = qr/$verbatim/x;
$verb_std = qr {
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

my @new_verbw_tmp = array_minus(@verbatim_w, @extract_env);
$verbatim_w = join q{|}, map { quotemeta } sort { length $a <=> length $b } @new_verbw_tmp;
$verbatim_w = qr/$verbatim_w/x;
$verb_wrt = qr {
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

### Regex using hash
%replace = (%extract_env);
$find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;

### We go line by line and make the changes (/p for ${^MATCH})
while ($bodydoc =~ /$verb_wrt | $verb_std /pgmx) {
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my $encontrado = ${^MATCH};
    $encontrado =~ s/($find)/$replace{$1}/g;
    substr $bodydoc, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
    pos ($bodydoc) = $pos_inicial + length $encontrado;
}

### Now put a internal dtxmark in no extraction/replace environments
Log("Pass verbatim write environments to %<*$dtxverb> ... %</$dtxverb>");
$bodydoc  =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
               ($verb_wrt)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;
$preamble =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
               ($verb_wrt)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;
$atbegindoc =~ s/($verb_wrt)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;

Log("Pass verbatim environments to %<*$dtxverb> ... %</$dtxverb>");
$bodydoc  =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
               ($verb_std)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;
$preamble =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
               ($verb_std)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;
$atbegindoc =~ s/($verb_std)/\%<\*$dtxverb>$1\%<\/$dtxverb>/gmsx;

### Check plain TeX syntax
my %plainsyntax = map { $_ => 1 } @extract_env; # anon hash

if (exists $plainsyntax{pspicture}) {
    Log('Convert plain \pspicture to LaTeX syntax [skip in PSTexample]');
    $bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                  \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                  \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
                  \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
    \\pspicture(\*)?(.+?)\\endpspicture/\\begin\{pspicture$1\}$2\\end\{pspicture$1\}/gmsx;
}

if (exists $plainsyntax{psgraph}) {
    Log('Convert plain \psgraph to LaTeX syntax [skip in PSTexample]');
    $bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                  \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                  \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
                  \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
    \\psgraph(\*)?(.+?)\\endpsgraph/\\begin\{psgraph$1\}$2\\end\{psgraph$1\}/gmsx;
}

if (exists $plainsyntax{tikzpicture}) {
    Log('Convert plain \tikzpicture to LaTeX syntax');
    $bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                  \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                  \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
    \\tikzpicture(.+?)\\endtikzpicture/\\begin{tikzpicture}$1\\end{tikzpicture}/gmsx;
}

if (exists $plainsyntax{pgfpicture}) {
    Log('Convert plain \pgfpicture to LaTeX syntax');
    $bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                  \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                  \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
    \\pgfpicture(.+?)\\endpgfpicture/\\begin{pgfpicture}$1\\end{pgfpicture}/gmsx;
}

### Force mode for pstricks/psgraph/tikzpiture
if ($opts_cmd{boolean}{force}) {
    if (exists $plainsyntax{pspicture} or exists $plainsyntax{psgraph}) {
        Log('Try to capture \psset{...} for pstricks environments [force mode]');
        $bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                      \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                      \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
                      \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
                      \\begin\{postscript\}.+?\\end\{postscript\}(*SKIP)(*F)|
                         (?<code>
                         (?:\\psset\{(?:\{.*?\}|[^\{])*\}.+?)?  # if exist ...save
                         \\begin\{(?<env> pspicture\*?| psgraph)\} .+? \\end\{\k<env>\}
                         )
                     /\\begin\{preview\}$+{code}\\end\{preview\}/gmsx;
    }
    if (exists $plainsyntax{tikzpicture}) {
        Log('Try to capture \tikzset{...} for tikzpicture environments [force mode]');
        $bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                      \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                      \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
                      \\begin\{postscript\}.+?\\end\{postscript\}(*SKIP)(*F)|
                        (?<code>
                        (?:\\tikzset\{(?:\{.*?\}|[^\{])*\}.+?)?  # if exist ...save
                        \\begin\{(?<env> tikzpicture)\} .+? \\end\{\k<env>\}
                        )
                     /\\begin\{preview\}$+{code}\\end\{preview\}/gmsx;
    }
}

Log('Pass skip environments to \begin{nopreview} ... \end{nopreview}');
$bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
              \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
              \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
              \\begin\{PSTexample\}.+?\\end\{PSTexample\}(*SKIP)(*F)|
              ($skip_env)/\\begin\{nopreview\}$1\\end\{nopreview\}/gmsx;

### Pass all captured environments in body \begin{preview} ... \end{preview}
Log('Pass all captured environments to \begin{preview} ... \end{preview}');
$bodydoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
              \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
              \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
              ($extr_tmp)/\\begin\{preview\}$1\\end\{preview\}/gmsx;

########################################################################
#  All environments are now classified:                                #
#  Extraction       ->    \begin{preview} ... \end{preview}            #
#  No Extraction    ->    \begin{nopreview} ... \end{nopreview}        #
#  Verbatim's       ->    %<\*$dtxverb> ... <\/$dtxverb>               #
########################################################################

### The %<*remove> ... %</remove> tags need a special treatment :)
$bodydoc  =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
               ^(\%<(?:\*|\/))(remove)(\>)/$1$2$tmp$3/gmsx;
$preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
               \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
               \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
               ^(\%<(?:\*|\/))(remove)(\>)/$1$2$tmp$3/gmsx;
$atbegindoc =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                 \\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                 \\begin\{preview\}.+?\\end\{preview\}(*SKIP)(*F)|
                 ^(\%<(?:\*|\/))(remove)(\>)/$1$2$tmp$3/gmsx;

### Pass \begin{preview} ... \end{preview} to \START{preview} ... \STOP{preview}
### Pass \begin{nopreview} ... \end{nopreview} to \START{nopreview} ... \STOP{nopreview}
$bodydoc =~ s/\\begin\{((no)?preview)\}/\\START\{$1\}/gmsx;
$bodydoc =~ s/\\end\{((no)?preview)\}/\\STOP\{$1\}/gmsx;

### We restore the changes of all environments in body
my @lineas = split /\n/, $bodydoc;
my $NEWDEL;
for (@lineas) {
    %replace = (%changes_out);
    $find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;
    if (/\\START\{((no)?preview)(?{ $NEWDEL = "\Q$^N" })\}/ .. /\\STOP\{$NEWDEL\}/) {
        s/($find)/$replace{$1}/msgx;
    }
    if (/\%<\*($dtxverb)(?{ $NEWDEL = "\Q$^N" })>/ .. /\%<\/$NEWDEL>/) {
        s/($find)/$replace{$1}/msgx;
    }
}
$bodydoc = join "\n", @lineas;

### We restore the changes of all environments in preamble
while ($preamble =~ /\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/pgmsx) {
    %cambios = (%changes_out);
    my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
    my  $encontrado = ${^MATCH};
    while (my($busco, $cambio) = each %cambios) {
        $encontrado =~ s/\Q$busco\E/$cambio/msxg;
    }
    substr $preamble, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
    pos ($preamble) = $pos_inicial + length $encontrado;
}

### Set wraped environments for extraction
my $wrapping = "$scriptname$tmp";
Log("Set up the environment [$wrapping] to encapsulate the extraction");

### Set vars for match/regex
my $BP = "\\\\begin\{$wrapping\}";
my $EP = "\\\\end\{$wrapping\}";
my $BE = '\\\\begin\{PSTexample\}';
my $EE = '\\\\end\{PSTexample\}';

### Wrap environments for extraction
Log("Pass all captured environments to \\begin{$wrapping} ... \\end{$wrapping}");
$bodydoc =~ s/\\START\{preview\}
                (?<code>.+? )
              \\STOP\{preview\}
             /\\begin\{$wrapping\}$+{code}\\end\{$wrapping\}/gmsx;

$bodydoc =~ s/\\START\{nopreview\}
                (?<code>.+? )
              \\STOP\{nopreview\}
             /\\begin\{nopreview\}\%$tmp$+{code}\\end\{nopreview\}\%$tmp/gmsx;

### We put back ltximg tags :)
$bodydoc  =~ s/($ltxtags)/$reverse_tag{$1}/gmsx;
$preamble =~ s/($ltxtags)/$reverse_tag{$1}/gmsx;

### First search PSTexample environments for extract
my @exa_extract = $bodydoc =~ m/(?:\\begin\{$wrapping\})($BE.+?$EE)(?:\\end\{$wrapping\})/gmsx;
my $exaNo = scalar @exa_extract;

### Set vars for log and print in terminal
my $envEXA  = $exaNo > 1 ? 'PSTexample environments' : 'PSTexample environment';
my $fileEXA = $exaNo > 1 ? 'files' : 'file';

### Check if PSTexample environment found
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
        my $swpl_grap = "graphic=\{\[scale=1\]$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-exa";
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

### Second search standart environments for extract
my @env_extract = $bodydoc =~ m/(?<=$BP)(.+?)(?=$EP)/gms;
my $envNo = scalar @env_extract;

### Set vars for log and print in terminal
my $envSTD  = $envNo > 1 ? 'standard environments' : 'standard environment';
my $fileSTD = $envNo > 1 ? 'files' : 'file';

### If any standard environments found
if ($envNo!=0) {
    $STDenv = 1;
    Log("Found $envNo $envSTD in $name$ext");
    my $figNo = 1;
    for my $item (@env_extract) {
        Logline("%##### Standard environment captured number $figNo ######%");
        Logline($item);
        $figNo++;
    }
}

### Run script process only if any enviroment found in <input file>
if ($envNo == 0 and $exaNo == 0) {
    errorUsage("$scriptname can not find any environment to extract in $name$ext");
}

### Set directory to save generated files, need full path for goog log :)
my $imgdirpath = File::Spec->rel2abs($opts_cmd{string}{imgdir});

if (-e $opts_cmd{string}{imgdir}) {
    Infoline("The generated files will be saved in $imgdirpath");
}
else {
    Infoline("Creating the directory $imgdirpath to save the generated files");
    Infocolor('Running', "mkdir $imgdirpath");
    Logline("[perl] mkdir($opts_cmd{string}{imgdir},0744)");
    mkdir $opts_cmd{string}{imgdir},0744 or errorUsage("Can't create the directory $opts_cmd{string}{imgdir}");
}

### Set compiler
my $compiler = $opts_cmd{compiler}{xetex}  ? 'xelatex'
             : $opts_cmd{compiler}{luatex} ? 'lualatex'
             : $opts_cmd{compiler}{latex}  ? 'latex'
             : $opts_cmd{compiler}{dvips}  ? 'latex'
             : $opts_cmd{compiler}{dvilua} ? 'dvilualatex'
             : $opts_cmd{compiler}{dvipdf} ? 'latex'
             : $opts_cmd{compiler}{arara}  ? 'arara'
             :                               'pdflatex'
             ;

if (!$opts_cmd{boolean}{norun}) {
    if ($compiler eq 'arara') {
        Log("The file will be processed using $compiler, no ducks will be harmed in this process");
    }
    else { Log("The file will be processed using $compiler"); }
}

### Set option for pdfcrop (last version of pdfcrop https://github.com/ho-tex/pdfcrop)
my $opt_crop = $opts_cmd{compiler}{xetex}  ? "--xetex  --margins $opts_cmd{string}{margin}"
             : $opts_cmd{compiler}{luatex} ? "--luatex --margins $opts_cmd{string}{margin}"
             : $opts_cmd{compiler}{latex}  ? "--margins $opts_cmd{string}{margin}"
             :                               "--pdftex --margins $opts_cmd{string}{margin}"
             ;

### Set options for preview packpage
my $opt_prew = $opts_cmd{compiler}{xetex}  ? 'xetex,'
             : $opts_cmd{compiler}{latex}  ? q{}
             : $opts_cmd{compiler}{dvipdf} ? q{}
             : $opts_cmd{compiler}{dvips}  ? q{}
             : $opts_cmd{compiler}{dvilua} ? q{}
             :                               'pdftex,'
             ;

########################################################################
# One problem with using arara is that we don't know what the file is  #
# really compiled with, this affects [preview] and [pdfcrop].          #
########################################################################

### Try to capture arara:compiler in preamble of <input file>
my @arara_engines = qw (latex pdflatex lualatex xelatex luahbtex);
my $arara_engines = join q{|}, map { quotemeta} sort { length $a <=> length $b } @arara_engines;
$arara_engines = qr/\b(?:$arara_engines)/x;
my $arara_rule = qr /^(?:\%\s{1}arara[:]\s{1}) ($arara_engines) /msx;

### Set options for [compiler], [preview] and [pdfcrop]
if ($compiler eq 'arara') {
    Log('Trying to detect some suported [engine] in the rules of arara');
    my @engine = $atbegindoc =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|$arara_rule/msx;
    my %engine = map { $_ => 1 } @engine; # anon hash
    if (exists $engine{latex}) {
        Log('The [latex] engine was found in arara rule');
        # Set options for [preview] and [pdfcrop]
        $opt_crop = "--margins $opts_cmd{string}{margin}";
        $opt_prew = q{};
    }
    elsif (exists $engine{lualatex} or exists $engine{luahbtex}) {
        Log('The [lualatex] engine was found in arara rule');
        # Set options for [preview] and [pdfcrop]
        $opt_crop = "--luatex --margins $opts_cmd{string}{margin}";
        $opt_prew = 'pdftex,';
    }
    elsif (exists $engine{xelatex}) {
        Log('The [xelatex] engine was found in arara rule');
        # Set options for [preview] and [pdfcrop]
        $opt_crop = "--xetex --margins $opts_cmd{string}{margin}";
        $opt_prew = 'xetex,';
    }
    elsif (exists($engine{pdflatex})) {
        Log('The [pdflatex] engine was found in arara rule');
        # Set options for [preview] and [pdfcrop]
        $opt_crop = "--pdftex --margins $opts_cmd{string}{margin}";
        $opt_prew = 'pdftex,';
    }
    else {
        Log('No supported [engine] could be detected, default values will be used');
        # Set options for [preview] and [pdfcrop]
        $opt_crop = "--pdftex --margins $opts_cmd{string}{margin}";
        $opt_prew = 'pdftex,';
    }
}

### Message in command line for compiler
my $msg_compiler = $opts_cmd{compiler}{xetex}  ? 'xelatex'
                 : $opts_cmd{compiler}{luatex} ? 'lualatex'
                 : $opts_cmd{compiler}{latex}  ? 'latex>dvips>ps2pdf'
                 : $opts_cmd{compiler}{dvips}  ? 'latex>dvips>ps2pdf'
                 : $opts_cmd{compiler}{dvilua} ? 'dvilualatex>dvips>ps2pdf'
                 : $opts_cmd{compiler}{dvipdf} ? 'latex>dvipdfmx'
                 : $opts_cmd{compiler}{arara}  ? 'arara'
                 :                               'pdflatex'
                 ;

### Set write18 for compiler
if ($opts_cmd{boolean}{shell}) {
    $write18 = '-shell-escape';
    $write18 = '--enable-write18' if defined $ENV{'TEXSYSTEM'} and $ENV{'TEXSYSTEM'} =~ /miktex/i;
}
else {
    $write18 = '-no-shell-escape';
    $write18 = '--disable-write18' if defined $ENV{'TEXSYSTEM'} and $ENV{'TEXSYSTEM'} =~ /miktex/i;
}

### Set options for compiler
my $opt_compiler = $opts_cmd{compiler}{arara} ? '--log'
                 :                              "$write18 -interaction=nonstopmode -recorder"
                 ;

if (!$opts_cmd{boolean}{norun}) {
    Log("The options '$opt_compiler' will be passed to [$compiler]");
}

### Set -q for system command line (gs, poppler-utils, dvips, dvipdfmx)
my $quiet = $verbose ? q{}
          :            '-q'
          ;

### Set options for ghostscript in command line
my %opt_gs_dev = (
    pdf  => '-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=pdfwrite',
    gray => '-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -sColorConversionStrategy=Gray -sProcessColorModel=DeviceGray',
    png  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=pngalpha -r$opts_cmd{string}{dpi}",
    bmp  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=bmp32b -r$opts_cmd{string}{dpi}",
    jpg  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=jpeg -r$opts_cmd{string}{dpi} -dJPEGQ=100 -dGraphicsAlphaBits=4 -dTextAlphaBits=4",
    tif  => "-dNOSAFER -dBATCH -dNOPAUSE -sDEVICE=tiff32nc -r$opts_cmd{string}{dpi}",
    );

### Set executable from poppler-utils
my %cmd_poppler = (
    eps => "pdftops",
    ppm => "pdftoppm",
    svg => "pdftocairo",
    );

### Set options for poppler-utils
my %opt_poppler = (
    eps => "$quiet -eps",
    ppm => "$quiet -r $opts_cmd{string}{dpi}",
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

### Seting \pagestyle{empty} for subfiles and process
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

#### Remove wraped postscript environments provide by pst-pdf, auto-pst-pdf, auto-pst-pdf-lua pkgs
Log("Convert postscript environments to \\begin\{$wrapping\} ... \\end\{$wrapping\} for standalone files");
$tmpbodydoc =~ s/(?:$BP)(?:\\begin\{postscript\})(?:\s*\[ [^]]*? \])?
                 (?<code>.+?)
                 (?:\\end\{postscript\})
                 (?:$EP)
                /\\begin\{$wrapping\}$+{code}\\end\{$wrapping\}/gmsx;

### We created a preamble for standalone files
my $sub_prea = "$atbeginout$preamout".'\begin{document}';

### Revert changes
$sub_prea =~ s/\%<\*$dtxverb>\s*(.+?)\s*\%<\/$dtxverb>/$1/gmsx;
%replace = (%changes_out);
$find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;
$sub_prea =~ s/($find)/$replace{$1}/g;
$sub_prea =~ s/(remove$tmp)/remove/g;

### Write standalone files for environments
if ($outsrc) {
    my $src_name = "$name-$opts_cmd{string}{prefix}-";
    my $srcNo    = 1;
    if ($opts_cmd{boolean}{srcenv}) {
        Log('Extract source code of all captured environments without preamble');
        if ($STDenv) {
            if (-e "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-1$ext") {
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
            while ($tmpbodydoc =~ m/$BP\s*(?<env_src>.+?)\s*$EP/gms) {
                open my $outexasrc, '>', "$opts_cmd{string}{imgdir}/$src_name$srcNo$ext";
                    print {$outexasrc} $+{'env_src'};
                close $outexasrc;
            }
            continue { $srcNo++; }
        }
        if ($PSTexa) {
            if (-e "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-exa-1$ext") {
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
            while ($tmpbodydoc =~ m/$BE\[.+?(?<pst_exa_name>$opts_cmd{string}{imgdir}\/.+?-\d+)\}\]\s*(?<exa_src>.+?)\s*$EE/gms) {
                open my $outstdsrc, '>', "$+{'pst_exa_name'}$ext";
                    print {$outstdsrc} $+{'exa_src'};
                close $outstdsrc;
            }
        }
    }
    if ($opts_cmd{boolean}{subenv}) {
        Log('Extract source code of all captured environments with preamble');
        if ($STDenv) {
            if (-e "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-1$ext") {
                Log("Recreating a $envNo standalone $fileSTD [$ext] for $envSTD in $imgdirpath");
                print "Recreating a $envNo standalone $fileSTD ", color('magenta'), "[$ext]",
                color('reset'), " for $envSTD\r\n";
            }
            else {
                Log("Creating a $envNo standalone $fileSTD [$ext] for $envSTD in $imgdirpath");
                print "Creating a $envNo standalone $fileSTD ", color('magenta'), "[$ext]",
                color('reset'), " for $envSTD\r\n";
            }
            # Write files
            while ($tmpbodydoc =~ m/$BP(?:\s*)?(?<env_src>.+?)(?:\s*)?$EP/gms) {
                open my $outstdfile, '>', "$opts_cmd{string}{imgdir}/$src_name$srcNo$ext";
                    print {$outstdfile} "$sub_prea\n$+{'env_src'}\n\\end\{document\}";
                close $outstdfile;
            }
            continue { $srcNo++; }
        }
        if ($PSTexa) {
            if (-e "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-exa-1$ext") {
                Log("Recreating a $exaNo standalone $fileEXA [$ext] for $envEXA in $imgdirpath");
                print "Recreating a $exaNo standalone $fileEXA ", color('magenta'), "[$ext]",
                color('reset'), " for $envEXA\r\n";
            }
            else {
                Log("Creating a $exaNo standalone $fileEXA [$ext] for $envEXA in $imgdirpath");
                print "Creating a $exaNo standalone $fileEXA ", color('magenta'), "[$ext]",
                color('reset'), " for $envEXA\r\n";
            }
            # Write files
            while ($tmpbodydoc =~ m/$BE\[.+?(?<pst_exa_name>$opts_cmd{string}{imgdir}\/.+?-\d+)\}\]\s*(?<exa_src>.+?)\s*$EE/gms) {
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

Log('Remove preview package (if found) in preamble [memory]');
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
                /\\begin\{nopreview\}\n$+{code}\n\\end\{nopreview\}\n/gmsx;

### Adjust $wrapping environments (need \n after close for some environments, like a Verbatim)
$tmpbodydoc =~ s/\\begin\{$wrapping\}
                    (?<code>.+?)
                  \\end\{$wrapping\}
                /\\begin\{$wrapping\}\n$+{code}\n\\end\{$wrapping\}/gmsx;

### Reverse changes for temporary file with all env (no in -exa file)
$tmpbodydoc =~ s/($find)/$replace{$1}/g;
$tmpbodydoc =~ s/(\%$tmp)//g;
$tmpbodydoc =~ s/(remove$tmp)/remove/g;
$preamout   =~ s/($find)/$replace{$1}/g;
$atbeginout =~ s/(remove$tmp)/remove/g;
$atbeginout =~ s/($find)/$replace{$1}/g;

### We created a preamble for individual files with all environments
$sub_prea = $opts_cmd{boolean}{noprew} ? "$atbeginout$pstpdfpkg$preamout".'\begin{document}'
          :                              "$atbeginout$previewpkg$preamout"
          ;

### Create a one file with "all" PSTexample environments extracted
if ($PSTexa) {
    @exa_extract = undef;
    Log("Adding packages to $name-$opts_cmd{string}{prefix}-exa-$tmp$ext");
    Logline($pstpdfpkg);
    Log('Convert plain Tex syntax for pspicture and psgraph to LaTeX syntax in PSTexample environments');
    while ($tmpbodydoc =~ m/$BE\[.+? $opts_cmd{string}{imgdir}\/.+?-\d+\}\] .+?$EE/pgsmx ) { # search
        my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
        my $encontrado = ${^MATCH};
        $encontrado =~ s/\\pspicture(\*)?(.+?)\\endpspicture/\\begin\{pspicture$1\}$2\\end\{pspicture$1\}/gmsx;
        $encontrado =~ s/\\psgraph(\*)?(.+?)\\endpsgraph/\\begin\{psgraph$1\}$2\\end\{psgraph$1\}/gmsx;
        substr $tmpbodydoc, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($tmpbodydoc) = $pos_inicial + length $encontrado;
    }
    # Write files
    Infoline("Creating $name-$opts_cmd{string}{prefix}-exa-$tmp$ext with $exaNo $envEXA extracted");
    while ($tmpbodydoc =~ m/$BE\[.+? $opts_cmd{string}{imgdir}\/.+?-\d+\}\](?<exa_src>.+?)$EE/gmsx ) { # search
        push @exa_extract, $+{'exa_src'}."\\newpage\n";
        open my $allexaenv, '>', "$name-$opts_cmd{string}{prefix}-exa-$tmp$ext";
            print {$allexaenv} "$atbeginout$pstpdfpkg$preamout".'\begin{document}'."@exa_extract"."\\end\{document\}";
        close $allexaenv;
    }
    # Remove [graphic={...}] in PSTexample example environments
    $tmpbodydoc =~ s/($BE)(?:\[graphic=\{\[scale=1\]$opts_cmd{string}{imgdir}\/.+?-\d+\}\])/$1/gmsx;
    $tmpbodydoc =~ s/($BE\[.+?)(?:,graphic=\{\[scale=1\]$opts_cmd{string}{imgdir}\/.+?-\d+\})(\])/$1$2/gmsx;
    # Moving and renaming
    if ($opts_cmd{boolean}{norun}) {
        Infoline("Moving and renaming $name-$opts_cmd{string}{prefix}-exa-$tmp$ext to $name-$opts_cmd{string}{prefix}-exa-all$ext");
        if (-e "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-exa-all$ext") {
            Infocolor('Warning', "The file [$name-$opts_cmd{string}{prefix}-exa-all$ext] already exists and will be rewritten");
            Log("Rewriting the file $name-$opts_cmd{string}{prefix}-exa-all$ext in $imgdirpath");
        }
        else {
            Log("Writing the file $name-$opts_cmd{string}{prefix}-exa-all$ext in $imgdirpath");
        }
        if ($verbose) {
            Infocolor('Running', "mv $workdir/$name-$opts_cmd{string}{prefix}-exa-$tmp$ext $imgdirpath/$name-$opts_cmd{string}{prefix}-exa-all$ext");
        }
        else {
            Infocolor('Running', "mv $name-$opts_cmd{string}{prefix}-exa-$tmp$ext ./$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-exa-all$ext");
        }
        Logline("[perl] move($workdir/$name-$opts_cmd{string}{prefix}-exa-$tmp$ext, $imgdirpath/$name-$opts_cmd{string}{prefix}-exa-all$ext)");
        move("$workdir/$name-$opts_cmd{string}{prefix}-exa-$tmp$ext", "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-exa-all$ext")
        or die "* Error!!: Couldn't be renamed $name-$opts_cmd{string}{prefix}-exa-$tmp$ext to ./$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-exa-all$ext";
    }
}

### Create a one file with "all" standard environments extracted
if ($STDenv) {
    if ($opts_cmd{boolean}{noprew}) {
        Log("Creating $name-$opts_cmd{string}{prefix}-$tmp$ext with $envNo $envSTD extracted [no preview]");
        print "Creating $name-$opts_cmd{string}{prefix}-$tmp$ext with $envNo $envSTD extracted",
        color('magenta'), " [no preview]\r\n",color('reset');
    }
    else {
        Log("Creating $name-$opts_cmd{string}{prefix}-$tmp$ext with $envNo $envSTD extracted [preview]");
        print "Creating $name-$opts_cmd{string}{prefix}-$tmp$ext with $envNo $envSTD extracted",
        color('magenta'), " [preview]\r\n",color('reset');
    }
    open my $allstdenv, '>', "$name-$opts_cmd{string}{prefix}-$tmp$ext";
        if ($opts_cmd{boolean}{noprew}) {
            my @env_extract;
            while ($tmpbodydoc =~ m/(?:$BP)(?<env_src>.+?)(?:$EP)/gms) {
                push @env_extract, $+{'env_src'}."\\newpage\n";
            }
            Log("Adding packages to $name-$opts_cmd{string}{prefix}-$tmp$ext");
            Logline($pstpdfpkg);
            print {$allstdenv} $sub_prea."@env_extract"."\\end{document}";
        }
        else {
            Log("Adding packages to $name-$opts_cmd{string}{prefix}-$tmp$ext");
            Logline($previewpkg);
            Log("Convert $wrapping to preview environments in $name-$opts_cmd{string}{prefix}-$tmp$ext");
            # Convert $wrapping to preview environments
            $tmpbodydoc =~ s/\\begin\{$wrapping\}(?<code>.+?)\\end\{$wrapping\}
                            /\\begin\{preview\}$+{code}\\end\{preview\}\n/gmsx;
            print {$allstdenv} $sub_prea.$tmpbodydoc."\n\\end{document}";
        }
    close $allstdenv;
    if ($opts_cmd{boolean}{norun}) {
        # Moving and renaming
        Infoline("Moving and renaming $name-$opts_cmd{string}{prefix}-$tmp$ext to $name-$opts_cmd{string}{prefix}-all$ext");
        if (-e "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-all$ext") {
            Infocolor('Warning', "The file [$name-$opts_cmd{string}{prefix}-all$ext] already exists and will be rewritten");
            Log("Rewriting the file $name-$opts_cmd{string}{prefix}-all$ext in $imgdirpath");
        }
        else {
            Log("Writing the file $name-$opts_cmd{string}{prefix}-all$ext in $imgdirpath");
        }
        if ($verbose) {
            Infocolor('Running', "mv $workdir/$name-$opts_cmd{string}{prefix}-$tmp$ext $imgdirpath/$name-$opts_cmd{string}{prefix}-all$ext");
        }
        else {
            Infocolor('Running', "mv $name-$opts_cmd{string}{prefix}-$tmp$ext ./$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-all$ext");
        }
        Logline("[perl] move($workdir/$name-$opts_cmd{string}{prefix}-$tmp$ext, $imgdirpath/$name-$opts_cmd{string}{prefix}-all$ext)");
        move("$workdir/$name-$opts_cmd{string}{prefix}-$tmp$ext", "$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-all$ext")
        or die "* Error!!: Couldn't be renamed $name-$opts_cmd{string}{prefix}-$tmp$ext to ./$opts_cmd{string}{imgdir}/$name-$opts_cmd{string}{prefix}-all$ext";
    }
}

### Compiler and generate PDF files
if (!$opts_cmd{boolean}{norun}) {
Log('Generate a PDF file with all captured environments');
my @compiler = (1..$opts_cmd{string}{runs});
opendir (my $DIR, $workdir);
    while (readdir $DIR) {
        if (/(?<name>$name-$opts_cmd{string}{prefix}(-exa)?)(?<type>-$tmp$ext)/) {
            Log("Compiling the file $+{name}$+{type} using [$msg_compiler]");
            print "Compiling the file $+{name}$+{type} using ", color('magenta'), "[$msg_compiler]\r\n",color('reset');
            for (@compiler){
                RUNOSCMD("$compiler $opt_compiler","$+{name}$+{type}",'show');
            }
            # Compiling file using latex>dvips>ps2pdf
            if ($compiler eq 'dvips' or $compiler eq 'latex' or $compiler eq 'dvilualatex') {
                RUNOSCMD("dvips $quiet -Ppdf", "-o $+{name}-$tmp.ps $+{name}-$tmp.dvi",'show');
                RUNOSCMD("ps2pdf -sPDFSETTINGS=prepress -sAutoRotatePages=None", "$+{name}-$tmp.ps  $+{name}-$tmp.pdf",'show');
            }
            # Compiling file using latex>dvipdfmx
            if ($compiler eq 'dvipdf') {
                RUNOSCMD("dvipdfmx $quiet", "$+{name}-$tmp.dvi",'show');
            }
            # Moving and renaming temp files with source code
            Infoline("Moving and renaming $+{name}$+{type} to $+{name}-all$ext");
            if (-e "$opts_cmd{string}{imgdir}/$+{name}-all$ext") {
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
                Infocolor('Running', "mv $+{name}$+{type} ./$opts_cmd{string}{imgdir}/$+{name}-all$ext");
            }
            Logline("[perl] move($workdir/$+{name}$+{type}, $imgdirpath/$+{name}-all$ext)");
            move("$workdir/$+{name}$+{type}", "$opts_cmd{string}{imgdir}/$+{name}-all$ext")
            or die "* Error!!: Couldn't be renamed $+{name}$+{type} to $opts_cmd{string}{imgdir}/$+{name}-all$ext";
            # pdfcrop
            if (!$opts_cmd{boolean}{nocrop}) {
                Infoline("Cropping the file $+{name}-$tmp.pdf");
                RUNOSCMD("pdfcrop $opt_crop", "$+{name}-$tmp.pdf $+{name}-$tmp.pdf",'show');
            }
            # gray
            if ($opts_cmd{boolean}{gray}) {
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
if (!$opts_cmd{boolean}{norun}) {
    Log("Creating the image formats: $format, working on $tempDir");
    opendir(my $DIR, $tempDir);
        while (readdir $DIR) {
            # PDF/PNG/JPG/BMP/TIFF format suported by ghostscript
            if (/(?<name>$name-$opts_cmd{string}{prefix}(-exa)?)(?<type>-all\.pdf)/) {
                for my $var (qw(pdf png jpg bmp tif)) {
                    if (defined $opts_cmd{image}{$var}) {
                        Log("Generating format [$var] from file $+{name}$+{type} in $imgdirpath using $gscmd");
                        print 'Generating format', color('blue'), " [$var] ", color('reset'),"from file $+{name}$+{type}\r\n";
                        RUNOSCMD("$gscmd $quiet $opt_gs_dev{$var} ", "-o $workdir/$opts_cmd{string}{imgdir}/$+{name}-%1d.$var $tempDir/$+{name}$+{type}",'show');
                    }
                }
            }
            # EPS/PPM/SVG format suported by poppler-utils
            if (/(?<name>$name-$opts_cmd{string}{prefix}-exa)(?<type>-all\.pdf)/) { # pst-exa package
                for my $var (qw(eps ppm svg)) {
                    if (defined $opts_cmd{image}{$var}) {
                        Log("Generating format [$var] from file $+{name}$+{type} in $imgdirpath using $cmd_poppler{$var}");
                        print 'Generating format', color('blue'), " [$var] ", color('reset'),"from file $+{name}$+{type}\r\n";
                        if (!$verbose){
                            Infocolor('Running', "$cmd_poppler{$var} $opt_poppler{$var}");
                        }
                        for (my $epsNo = 1; $epsNo <= $exaNo; $epsNo++) {
                            RUNOSCMD("$cmd_poppler{$var} $opt_poppler{$var}", "-f $epsNo -l $epsNo $tempDir/$+{name}$+{type} $workdir/$opts_cmd{string}{imgdir}/$+{name}-$epsNo.$var",'only');
                        }
                    }
                }
            }
            if (/(?<name>$name-$opts_cmd{string}{prefix})(?<type>-all\.pdf)/) {
                for my $var (qw(eps ppm svg)) {
                    if (defined $opts_cmd{image}{$var}) {
                        Log("Generating format [$var] from file $+{name}$+{type} in $imgdirpath using $cmd_poppler{$var}");
                        print 'Generating format', color('blue'), " [$var] ", color('reset'),"from file $+{name}$+{type}\r\n";
                        if (!$verbose){
                            Infocolor('Running', "$cmd_poppler{$var} $opt_poppler{$var}");
                        }
                        for (my $epsNo = 1; $epsNo <= $envNo; $epsNo++) {
                            RUNOSCMD("$cmd_poppler{$var} $opt_poppler{$var}", "-f $epsNo -l $epsNo $tempDir/$+{name}$+{type} $workdir/$opts_cmd{string}{imgdir}/$+{name}-$epsNo.$var",'only');
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
        opendir(my $DIR, $opts_cmd{string}{imgdir});
            while (readdir $DIR) {
                if (/(?<name>$name-$opts_cmd{string}{prefix}(-exa)?-\d+\.ppm)(?<sep>-\d+)(?<ppm>\.ppm)/) {
                    if ($verbose){
                        Infocolor('Running', "mv $+{name}$+{sep}$+{ppm} $+{name}");
                    }
                    Logline("[perl] move($imgdirpath/$+{name}$+{sep}$+{ppm}, $imgdirpath/$+{name})");
                    move("$opts_cmd{string}{imgdir}/$+{name}$+{sep}$+{ppm}", "$opts_cmd{string}{imgdir}/$+{name}")
                    or die "* Error!!: Couldn't be renamed $+{name}$+{sep}$+{ppm} to $+{name}";
                }
            }
        closedir $DIR;
    }
} # close run

### Constant
my $findgraphicx = 'true';

### Add $atbegindoc to $preamble
$preamble = "$atbegindoc$preamble";

### Replacing the extracted environments with \\includegraphics
if ($outfile) {
    my @tag_remove_preamble = $preamble =~ m/(?:^\%<\*remove$tmp>.+?\%<\/remove$tmp>)/gmsx;
    if (@tag_remove_preamble) {
        Log("Removing the content between <*remove> ... </remove> tags in preamble for $opts_cmd{string}{output}$outext");
        $preamble =~ s/^\%<\*remove$tmp>\s*(.+?)\s*\%<\/remove$tmp>(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
    }
    Log("Convert standard extracted environments to \\includegraphics for $opts_cmd{string}{output}$outext");
    my $grap  =  "\\includegraphics[scale=1]{$name-$opts_cmd{string}{prefix}-";
    my $close =  '}';
    my $imgNo =  1;
    $bodydoc  =~ s/$BP.+?$EP/$grap@{[$imgNo++]}$close/msg;
}

### pst-exa package
my $pstexa = qr/(?:\\ usepackage) \[\s*(.+?)\s*\] (?:\{\s*(pst-exa)\s*\} ) /x;
my @pst_exa;
my %pst_exa;

### Search pst-exa package
@pst_exa  = $preamble =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|$pstexa/gmsx;
%pst_exa = map { $_ => 1 } @pst_exa;

### If found comment and adjust path for graphic files
if (@pst_exa and $outfile) {
    Log("Comment pst-exa package in preamble for $opts_cmd{string}{output}$outext");
    $findgraphicx = 'false';
    $preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   (\\usepackage\[)\s*(swpl|tcb)\s*(\]\{pst-exa\})/\%$1$2,pdf$3/msxg;
    if (exists $pst_exa{tcb}) {
        Log("Suport for \\usepackage[tcb,pdf]\{pst-exa\} for $opts_cmd{string}{output}$outext");
        $bodydoc =~ s/(graphic=\{)\[(scale=\d*)\]($opts_cmd{string}{imgdir}\/$name-$opts_cmd{string}{prefix}-exa-\d*)\}/$1$2\}\{$3\}/gsmx;
    }
}

### Regex for clean file (pst) in preamble
$PALABRAS = qr/\b (?: pst-\w+ | pstricks (?: -add | -pdf )? | psfrag |psgo |vaucanson-g| auto-pst-pdf(?: -lua )? )/x;
$FAMILIA  = qr/\{ \s* $PALABRAS (?: \s* [,] \s* $PALABRAS )* \s* \}(\%*)?/x;

### Clean PST content in preamble
if ($clean{pst} and $outfile) {
    Log("Remove pstricks packages in preamble for $opts_cmd{string}{output}$outext");
    $preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   ^ $USEPACK (?: $CORCHETES )? $FAMILIA \s*//msxg;
    $preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   (?: ^ $USEPACK \{ | \G) [^}]*? \K (,?) \s* $PALABRAS (\s*) (,?) /$1 and $3 ? ',' : $1 ? $2 : ''/gemsx;
    if (@pst_exa) {
        Log("Uncomment pst-exa package in preamble for $opts_cmd{string}{output}$outext");
        $preamble =~ s/(?:\%)(\\usepackage\[\s*)(swpl|tcb)(,pdf\s*\]\{pst-exa\})/$1$2$3/msxg;
    }
    Log("Remove \\psset\{...\} in preamble for $opts_cmd{string}{output}$outext");
    $preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   \\psset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))+//gmsx;
    Log("Remove \\SpecialCoor in preamble for $opts_cmd{string}{output}$outext");
    $preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   \\SpecialCoor(?:[\t ]*(?:\r?\n|\r))+//gmsx;
    Log("Remove empty lines in preamble for $opts_cmd{string}{output}$outext");
    $preamble =~ s/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|
                   ^\\usepackage\{\}(?:[\t ]*(?:\r?\n|\r))+/\n/gmsx;
}

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

if (@graphicspath and $outfile) {
    Log("Found \\graphicspath in preamble for $opts_cmd{string}{output}$outext");
    $findgraphicx = 'false';
    while ($preamble =~ /$graphicspath /pgmx) {
        my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
        my $encontrado = ${^MATCH};
        if ($encontrado =~ /$graphicspath/) {
            my  $argumento = $1;
            if ($argumento !~ /\{$opts_cmd{string}{imgdir}\/\}/) {
                $argumento .= "\{$opts_cmd{string}{imgdir}/\}";
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
    inline-images asymptote UNAMThesis authorarchive amscdx pst-pdf adjustbox
    trimclip fixmetodonotes incgraph scanpages pst-layout alertmessage
    svg quiz2socrative realhats autopdf egplot decorule figsize tikzexternal
    pgfcore frontespizio textglos graphicx tikz tcolorbox pst-exa
    );

my $pkgcandidates = join q{|}, map { quotemeta } sort { length $a <=> length $b } @pkgcandidates;
$pkgcandidates = qr/$pkgcandidates/x;
my @graphicxpkg;

### Now we're trying to capture graphicx package
@graphicxpkg = $preamble =~ m/\%<\*$dtxverb> .+?\%<\/$dtxverb>(*SKIP)(*F)|($pkgcandidates)/gmsx;
if (@graphicxpkg  and $outfile and $findgraphicx ne 'false') {
    Log("Found graphicx package in preamble for $opts_cmd{string}{output}$outext");
    $findgraphicx = 'false';
}

### Revert changes in preamble for temp <output file>
my %tmpoutreplace = (
    'TMPGRAPHICXTMP'     => 'graphicx',
    'TMPGRAPHICSPATHTMP' => 'graphicspath',
);

%replace  = (%changes_out,%tmpoutreplace);
$find     = join q{|}, map {quotemeta} sort { length $a <=> length $b } keys %replace;

### Capture graphicx.sty in .log of LaTeX file
if ($findgraphicx ne 'false' and $outfile) {
    Log("Couldn't capture the graphicx package for $opts_cmd{string}{output}$ext in preamble");
    my $ltxlog;
    my @graphicx;
    my $null = devnull();
    # Copy preamble and revert changes
    my $preambletmp = $preamble;
    $preambletmp =~ s/($find)/$replace{$1}/g;
    $preambletmp   =~ s/\%<\*$dtxverb>(.+?)\%<\/$dtxverb>/$1/gmsx;
    Log("Creating $name-$opts_cmd{string}{prefix}-$tmp$ext [only preamble]");
    if ($verbose) { say "Creating [$name-$opts_cmd{string}{prefix}-$tmp$ext] with only preamble"; }
    open my $OUTfile, '>', "$name-$opts_cmd{string}{prefix}-$tmp$ext";
        print {$OUTfile} "$preambletmp\n\\stop";
    close $OUTfile;
    # Set compiler
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
    if ($opts_cmd{compiler}{latex}) { $compiler = 'pdflatex'; }
    if ($opts_cmd{compiler}{dvilua}) { $compiler = 'lualatex'; }
    # Compiling file
    RUNOSCMD("$compiler $write18 -interaction=batchmode", "$name-$opts_cmd{string}{prefix}-$tmp$ext >$null", 'only');
    # Restore arara compiler
    if ($opts_cmd{compiler}{arara}) { $compiler = 'arara'; }
    Log("Search graphicx package in $name-$opts_cmd{string}{prefix}-$tmp.log");
    open my $LaTeXlog, '<', "$name-$opts_cmd{string}{prefix}-$tmp.log";
        {
            local $/;
            $ltxlog = <$LaTeXlog>;
        }
    close $LaTeXlog;
    # Try to capture graphicx
    @graphicx = $ltxlog =~ m/.+? (graphicx\.sty)/xg;
    if (@graphicx) {
        Log("Found graphicx package in $name-$opts_cmd{string}{prefix}-$tmp.log");
    }
    else {
        Log("Not found graphicx package in $name-$opts_cmd{string}{prefix}-$tmp.log");
        Log("Add \\usepackage\{graphicx\} to preamble of $opts_cmd{string}{output}$outext");
        $preamble= "$preamble\\usepackage\{graphicx\}\n";
    }
}

### Add last lines
if ($outfile) {
    if (!@graphicspath) {
        Log("Not found \\graphicspath in preamble for $opts_cmd{string}{output}$outext");
        Log("Add \\graphicspath\{\{$opts_cmd{string}{imgdir}/\}\} to preamble for $opts_cmd{string}{output}$ext");
        $preamble= "$preamble\\graphicspath\{\{$opts_cmd{string}{imgdir}/\}\}\n";
    }
    Log("Add \\usepackage\{grfext\} to preamble for $opts_cmd{string}{output}$ext");
    $preamble = "$preamble\\usepackage\{grfext\}\n";
    Log("Add \\PrependGraphicsExtensions\*\{\.pdf\} to preamble for $opts_cmd{string}{output}$ext");
    $preamble = "$preamble\\PrependGraphicsExtensions\*\{\.pdf\}";
    $preamble =~ s/\%<\*$dtxverb>\s*(.+?)\s*\%<\/$dtxverb>/$1/gmsx;
    $preamble =~ s/^\\usepackage\{\}(?:[\t ]*(?:\r?\n|\r))+/\n/gmsx;
    $preamble =~ s/^(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
}

### We remove environments from the <output file>
if (%delete_env and $outfile) {
    Log("Remove environments in body of $opts_cmd{string}{output}$ext");
    %replace = (%delete_env);
    $find    = join q{|}, map { quotemeta } sort { length $a <=> length $b } keys %replace;
    # We must prevent eliminating somewhere wrong
    # We re-create the regular expressions to make the changes
    my @new_verb_tmp = array_minus(@verbatim, @delete_env);
    $verbatim = join q{|}, map { quotemeta } sort { length $a <=> length $b } @new_verb_tmp;
    $verbatim = qr/$verbatim/x;
    $verb_std = qr {
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

    my @new_verbw_tmp = array_minus(@verbatim_w, @delete_env);
    $verbatim_w = join q{|}, map { quotemeta } sort { length $a <=> length $b } @new_verbw_tmp;
    $verbatim_w = qr/$verbatim_w/x;
    $verb_wrt = qr {
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
    while ($bodydoc =~ /$verb_wrt | $verb_std /pgmx) {
        my ($pos_inicial, $pos_final) = ($-[0], $+[0]);
        my $encontrado = ${^MATCH};
        $encontrado =~ s/($find)/$replace{$1}/g;
        substr $bodydoc, $pos_inicial, $pos_final-$pos_inicial, $encontrado;
        pos ($bodydoc) = $pos_inicial + length $encontrado;
    }
    # Now remove
    $bodydoc =~ s/($delt_env)(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
}

### Create a <output file>
if ($outfile) {
    # Options for out_file (add $end to outfile)
    my $out_file = $clean{doc} ? "$preamble\n$bodydoc\n\\end\{document\}"
                :                "$preamble\n$bodydoc\n$enddoc"
                ;
    # Clean \psset{...} content in <output file>
    if ($clean{pst}) {
        $out_file =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                       \%<\*$dtxverb> .+? \%<\/$dtxverb>(*SKIP)(*F)|
                       \\psset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
    }
    # Clean \tikzset{...} content in <output file>
    if ($clean{tkz}) {
        $out_file =~ s/\\begin\{nopreview\}.+?\\end\{nopreview\}(*SKIP)(*F)|
                       \%<\*$dtxverb> .+? \%<\/$dtxverb>(*SKIP)(*F)|
                       \\tikzset\{(?:\{.*?\}|[^\{])*\}(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
    }
    # Revert all changes in outfile
    $out_file =~ s/\\begin\{nopreview\}\%$tmp\s*(.+?)\s*\\end\{nopreview\}\%$tmp/$1/gmsx;
    my @tag_remove_outfile = $out_file =~ m/(?:^\%<\*remove$tmp>.+?\%<\/remove$tmp>)/gmsx;
    if (@tag_remove_outfile) {
        Log("Removing the content between <*remove> ... </remove> tags in all $opts_cmd{string}{output}$outext");
        $out_file =~ s/^\%<\*remove$tmp>\s*(.+?)\s*\%<\/remove$tmp>(?:[\t ]*(?:\r?\n|\r))?+//gmsx;
    }
    # Remove internal mark for verbatim and verbatim write environments
    $out_file =~ s/\%<\*$dtxverb>\s*(.+?)\s*\%<\/$dtxverb>/$1/gmsx;
    %replace = (%changes_out);
    $find    = join q{|}, map {quotemeta} sort { length $a <=> length $b } keys %replace;
    $out_file =~ s/($find)/$replace{$1}/g;
    if (-e "$opts_cmd{string}{output}$outext") {
        Log("Rewriting the file $opts_cmd{string}{output}$outext in $workdir");
        Infocolor('Warning', "The file [$opts_cmd{string}{output}$outext] already exists and will be rewritten");
    }
    else{
        Infoline("Creating the file $opts_cmd{string}{output}$outext");
        Log("Write the file $opts_cmd{string}{output}$outext in $workdir");
    }
    # Write <output file>
    open my $OUTfile, '>', "$opts_cmd{string}{output}$outext";
        print {$OUTfile} $out_file;
    close $OUTfile;
}

### Set compiler for process <output file>
$compiler = $opts_cmd{compiler}{xetex}   ? 'xelatex'
          : $opts_cmd{compiler}{luatex}  ? 'lualatex'
          : $opts_cmd{compiler}{dvips}   ? 'latex'
          : $opts_cmd{compiler}{dvilua}  ? 'lualatex'
          : $opts_cmd{compiler}{dvipdf}  ? 'latex'
          : $opts_cmd{compiler}{arara}   ? 'arara'
          : $opts_cmd{compiler}{latexmk} ? 'latexmk'
          :                                'pdflatex'
          ;

### Set options for latexmk
my $ltxmkopt = $opts_cmd{compiler}{xetex}  ? "-pdfxe -silent -xelatex=\"xelatex $write18 -recorder %O %S\""
             : $opts_cmd{compiler}{luatex} ? "-pdflua -silent -lualatex=\"lualatex $write18 -recorder %O %S\""
             : $opts_cmd{compiler}{dvilua} ? "-pdflua -silent -lualatex=\"lualatex $write18 -recorder %O %S\""
             : $opts_cmd{compiler}{dvips}  ? "-pdfps -silent -latex=\"latex $write18 -recorder %O %S\""
             : $opts_cmd{compiler}{dvipdf} ? "-pdfdvi -silent -latex=\"latex $write18 -recorder %O %S\""
             :                               "-pdf -silent -pdflatex=\"pdflatex $write18 -recorder %O %S\""
             ;

### Set options for compiler <output file>
$opt_compiler = $opts_cmd{compiler}{arara}   ? '--log'
              : $opts_cmd{compiler}{latexmk} ? "$ltxmkopt"
              :                                "$write18 -interaction=nonstopmode -recorder"
              ;

### Message in command line for compiler
$msg_compiler = $opts_cmd{compiler}{xetex}  ? 'xelatex'
              : $opts_cmd{compiler}{luatex} ? 'lualatex'
              : $opts_cmd{compiler}{latex}  ? 'pdflatex'
              : $opts_cmd{compiler}{dvips}  ? 'latex>dvips>ps2pdf'
              : $opts_cmd{compiler}{dvilua} ? 'lualatex'
              : $opts_cmd{compiler}{dvipdf} ? 'latex>dvipdfmx'
              : $opts_cmd{compiler}{arara}  ? 'arara'
              :                               'latexmk'
              ;

### Now set message for latexmk
if ($opts_cmd{compiler}{latexmk}) {
    $msg_compiler = 'latexmk';
}

### Compiling <output file>
if (!$opts_cmd{boolean}{norun} and $outfile) {
    Log("Compiling the file $opts_cmd{string}{output}$outext using [$msg_compiler]");
    print "Compiling the file $opts_cmd{string}{output}$outext using ", color('magenta'), "[$msg_compiler]\r\n",color('reset');
    RUNOSCMD("$compiler $opt_compiler", "$opts_cmd{string}{output}$outext",'show');
    # Compiling <output file> using dvips>ps2pdf
    if ($opts_cmd{compiler}{dvips}) {
        RUNOSCMD("dvips $quiet -Ppdf", "$opts_cmd{string}{output}.dvi",'show');
        RUNOSCMD("ps2pdf -sPDFSETTINGS=prepress -sAutoRotatePages=None", "$opts_cmd{string}{output}.ps $opts_cmd{string}{output}.pdf", 'show');
    }
    # Compiling <output file> using latex>dvipdfmx
    if ($opts_cmd{compiler}{dvipdf}) {
        RUNOSCMD("dvipdfmx $quiet", "$opts_cmd{string}{output}.dvi", 'show');
    }
}

### Remove temporary files
my @tmpfiles;
my @protected = qw();
my $flsline = 'OUTPUT';
my @flsfile;

### Protect generated files
if (defined $opts_cmd{string}{output}) {
    push @protected, "$opts_cmd{string}{output}$outext", "$opts_cmd{string}{output}.pdf";
}

### Find files
find(\&aux_files, $workdir);
sub aux_files{
    my $findtmpfiles = $_;
    if (-f $findtmpfiles && $findtmpfiles =~ m/$name-$opts_cmd{string}{prefix}(-exa)?-$tmp.+?$/) { # search
        push @tmpfiles, $_;
    }
    return;
}

### Add if exists
if (-e 'arara.log') {
    push @flsfile, 'arara.log';
}
if (-e "$name-$opts_cmd{string}{prefix}-$tmp.fls") {
    push @flsfile, "$name-$opts_cmd{string}{prefix}-$tmp.fls";
}
if (-e "$name-$opts_cmd{string}{prefix}-exa-$tmp.fls") {
    push @flsfile, "$name-$opts_cmd{string}{prefix}-exa-$tmp.fls";
}
if (-e "$opts_cmd{string}{output}.fls") {
    push @flsfile, "$opts_cmd{string}{output}.fls";
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
my $mintdir    = "\_minted\-$name-$opts_cmd{string}{prefix}-$tmp";
my $mintdirexa = "\_minted\-$name-$opts_cmd{string}{prefix}-exa-$tmp";
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

### Compress ./images with all generated files
my $archivetar;
if ($opts_cmd{boolean}{zip} or $opts_cmd{boolean}{tar}) {
    my $stamp = strftime("%Y-%m-%d", localtime);
    $archivetar = "$opts_cmd{string}{imgdir}-$stamp";

    my @savetozt;
    find(\&zip_tar, $opts_cmd{string}{imgdir});
    sub zip_tar{
        my $filesto = $_;
        if (-f $filesto && $filesto =~ m/$name-$opts_cmd{string}{prefix}-.+?$/) { # search
            push @savetozt, $File::Find::name;
        }
        return;
    }
    Log("The files are compress found in $imgdirpath are:");
    Logarray(\@savetozt);
    if ($opts_cmd{boolean}{zip}) {
        if (-e "$archivetar.zip") {
            Infocolor('Warning', "The file [$archivetar.zip] already exists and will be rewritten");
            Log("Rewriting the file $archivetar.zip in $workdir");
        }
        else{
            print "Creating the file ", color('magenta'), "[$archivetar.zip]",
            color('reset'), " with generate files in ./$opts_cmd{string}{imgdir}\r\n";
            Log("Writen the file $archivetar.tar.gz in $workdir");
        }
        zip \@savetozt => "$archivetar.zip";
    }
    if ($opts_cmd{boolean}{tar}) {
        if (-e "$archivetar.tar.gz") {
            Infocolor('Warning', "The file [$archivetar.tar.gz] already exists and will be rewritten");
            Log("Rewriting the file $archivetar.tar.gz in $workdir");
        }
        else{
            print "Creating the file ", color('magenta'), "[$archivetar.tar.gz]",
            color('reset'), " with generate files in ./$opts_cmd{string}{imgdir}\r\n";
            Log("Writen the file $archivetar.tar.gz in $workdir");
        }
        my $imgdirtar = Archive::Tar->new();
        $imgdirtar->add_files(@savetozt);
        $imgdirtar->write( "$archivetar.tar.gz" , 9 );
    }
}

### End script process
if (!$opts_cmd{boolean}{norun} and ($opts_cmd{boolean}{srcenv} or $opts_cmd{boolean}{subenv})) {
    Log("The image files: $format and generated files are in $imgdirpath");
}
if (!$opts_cmd{boolean}{norun} and (!$opts_cmd{boolean}{srcenv} and !$opts_cmd{boolean}{subenv})) {
    Log("The image files: $format are in $imgdirpath");
}
if ($opts_cmd{boolean}{norun} and ($opts_cmd{boolean}{srcenv} or $opts_cmd{boolean}{subenv})) {
    Log("The generated files are in $imgdirpath");
}
if ($outfile) { Log("The file $opts_cmd{string}{output}$ext are in $workdir"); }

Infocolor('Finish', "The execution of $scriptname has been successfully completed");

Log("The execution of $scriptname has been successfully completed");

__END__
