#!/usr/bin/env perl

## getnonfreefonts
## Copyright 2006-2008 Reinhard Kotucha <reinhard.kotucha@web.de>
#
# This work may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either version 1.3
# of this license or (at your option) any later version.
# The latest version of this license is in
#   http://www.latex-project.org/lppl.txt
# 
# The current maintainer is Reinhard Kotucha.

my $TL_version='2008';  
my $revision='2009-01-29';

use Getopt::Long;
$Getopt::Long::autoabbrev=0;
Getopt::Long::Configure ("bundling");

$opt_lsfonts=0;
$opt_force=0;

sub usage {
    print <<'EOF';
Usage:
    getnonfreefonts[-sys] [-a|--all] [-d|--debug] [-f|--force]
        [-l|--lsfonts] [-v|--verbose] [--version] [-H|--http]
	[font1] [font2] ...

    getnonfreefonts installs fonts in $TEXMFHOME.
    getnonfreefonts-sys installs fonts in $TEXMFLOCAL.

    Options:
        -a|--all      Install all fonts.

        -d|--debug    Provide additional messages for debugging.

        -f|--force    Install fonts even if they are installed already.

        -h|--help     Print this message.

        -l|--lsfonts  List all fonts available.

        -v|--verbose  Be more verbose.

	-H|--http     Use http instead of ftp (see manual).

        --version     Print version number.

EOF
;
}

@ARGS=@ARGV;

GetOptions 
    "all|a",
    "debug|d",
    "force|f",
    "help|h",
    "http|H",
    "lsfonts|l",
    "verbose|v",
    "version",
    "sys";

$^W=1 if $opt_debug;

my $sys=($0=~/-sys$/)? 1:0;
$sys=1 if $opt_sys;
$sys=1 if (defined $ENV{'TEX_SYS_PROG'});

@allpackages=@ARGV;

print "$TL_version\n" and exit 0 if $opt_version; 

if ($opt_help or !@ARGS) {
    print "\nThis is getnonfreefonts";
    print '-sys' if ($sys);
    print ", version $TL_version.\n\n";
    usage; 
}


sub win32 {
    if ($^O=~/^MSWin(32|64)/i) {
	return 1;
    } else {
	return 0;
    }
}


sub message {
    my $message=shift;
    if ($message=~/^\[/) {
	print "$message\n";
    } else {
	printf "%-60s", $message;
    }
}


sub debug_msg {
    my $message=shift;
    if ($opt_debug) {
	print STDERR "DEBUG: $message\n";
    }
}

sub which {
    my $prog=shift;
    my @PATH;
    my $PATH=$ENV{'PATH'};
    if (&win32) {
	my @PATHEXT=split ';', $ENV{'PATHEXT'};
	push @PATHEXT, '';  # if argument contains an extension
	@PATH=split ';', $PATH;
	for my $dir (@PATH) {
	    $dir=~s/\\/\//g;
	    for my $ext (@PATHEXT) {
		if (-f "$dir/$prog$ext") {
		    return "$dir/$prog$ext";
		}
	    }
	}
    } else {
	@PATH=split ':', $PATH;
	for my $dir (@PATH) {
	    if (-x "$dir/$prog") {
		return "$dir/$prog";
	    }
	}
    }
    return 0;
}


sub expand_var {
    my $var=shift;

    if (win32) {
	open KPSEWHICH, 'kpsewhich --expand-var=$'  . "$var |";
    } else {
	open KPSEWHICH, 'kpsewhich --expand-var=\$' . "$var |";
    }
    while (<KPSEWHICH>) {
	chop;
	return "$_";
    }
    close KPSEWHICH;
}


sub expand_braces {
    my $var=shift;
    my $pathsep;
    my $retstring;
    my @retlist;
    if (win32) {
	open KPSEWHICH, 'kpsewhich --expand-braces=$'  . "$var |";
	$pathsep=';';
    } else {
	open KPSEWHICH, 'kpsewhich --expand-braces=\$' . "$var |";
	$pathsep=':';
    }
    $retstring=(<KPSEWHICH>);
    close KPSEWHICH;
    chop $retstring;
    @retlist=split $pathsep, $retstring;
    if ($opt_debug) {
	my $index=0;
	foreach my $entry (@retlist) {
	    debug_msg "$var\[$index\]: '$entry'.";
	    ++$index;
	}
	debug_msg "Extracting the first element of the list ($var\[0\]):";
	debug_msg "$var\[0\]='$retlist[0]'.";
    }
    return "$retlist[0]";
}

debug_msg "getnonfreefonts rev. $revision (TL $TL_version).";

# Determine the URL.

my $getfont_url;

if ($opt_http) {
    $getfont_url="http://tug.org/~kotucha/getnonfreefonts/getfont$TL_version";
} else {
    $getfont_url="ftp://tug.org/tex/getnonfreefonts/getfont$TL_version";
}
($opt_http)? 
    debug_msg 'Download method: HTTP.':debug_msg 'Download method: FTP.';

debug_msg "Note that not all fonts are accessible via HTTP." if $opt_http;
debug_msg "Try --html if your firewall blocks FTP." unless $opt_http;
debug_msg "Using script '$getfont_url'.";

if ($opt_debug) {
    for my $var qw(TMP TEMP TMPDIR) {
	if (defined $ENV{$var}) {
	    debug_msg "Environment variable $var='$ENV{$var}'.";
	} else {
	    debug_msg "Environment variable $var not set.";
	}
    }
}

# get TMP|TEMP|TMPDIR environment variable

if (defined $ENV{'TMP'}) {
    $SYSTMP="$ENV{'TMP'}";
} elsif (defined $ENV{'TEMP'}) {
    $SYSTMP="$ENV{'TEMP'}";
} elsif (defined $ENV{'TMPDIR'}) {
    $SYSTMP="$ENV{'TMPDIR'}";
} else {
    $SYSTMP="/tmp";
}

debug_msg "Internal variable SYSTMP set to '$SYSTMP'.";

# Windows usually uses backslashes though forward slashes are probably
# allowed.  Perl always needs forward slashes.

# We convert backslashes to forward slashes on Windows.

if (win32) {
    $SYSTMP=~s/\\/\//g;
    debug_msg "Internal variable SYSTMP converted to '$SYSTMP'.";
}


sub check_tmpdir{
die "! ERROR: The temporary directory '$SYSTMP' doesn't exist.\n"
    unless (-d "$SYSTMP");

die "! ERROR: The temporary directory '$SYSTMP' is not writable.\n"
    unless (-w "$SYSTMP");
}


if ($opt_debug) {
    debug_msg "Search for kpsewhich in PATH:";
    my $kpsewhich=which "kpsewhich";
    debug_msg "Found '$kpsewhich'.";
}

# Determine INSTALLROOT.

$INSTALLROOTNAME=($sys)? 'TEXMFLOCAL':'TEXMFHOME';

$INSTALLROOT=expand_braces "$INSTALLROOTNAME";

# Remove trailing exclamation marks and double slashes.

debug_msg "Removing leading \"!!\" and trailing \"//\"and set INSTALLROOT:";

$INSTALLROOT=~s/^!!//;
$INSTALLROOT=~s/\/\/$//;

debug_msg "INSTALLROOT='$INSTALLROOT'.";

($sys)? debug_msg "sys=true.":debug_msg "sys=false.";

$INSTALLROOT=~s/\\/\//g if (win32);

debug_msg "Internal variable INSTALLROOT converted to '$INSTALLROOT'.";

sub check_installroot {
    mkdir "$INSTALLROOT" unless (-d "$INSTALLROOT");

    die "! ERROR: The variable $INSTALLROOTNAME is not set.\n"
	unless length ("$INSTALLROOT") > 0;

    die "! ERROR: The install directory '$INSTALLROOT' doesn't " .
	"exist.\n" .
	"         If this is the correct path, please create " .
	"this directory manually.\n" 
	unless (-d "$INSTALLROOT");
    
    die "! ERROR: The install directory '$INSTALLROOT' is not writable.\n"
	unless (-w "$INSTALLROOT");
}

if ($opt_help or !@ARGS) {
    print <<"ENDUSAGE";
  Directories:
       temporary: '$SYSTMP/getfont-<PID>'
       install:   '$INSTALLROOT'

ENDUSAGE
check_tmpdir;
check_installroot;
exit 0;
}

check_tmpdir;
check_installroot;

my $tmpdir="$SYSTMP/getfont-$$";
debug_msg "Internal variable tmpdir set to '$tmpdir'.";

mkdir "$tmpdir" or die "! ERROR: Can't mkdir '$tmpdir'.";
chdir "$tmpdir" or die "! ERROR: Can't cd '$tmpdir'.";

sub remove_tmpdir {
    chdir "$SYSTMP" or die "! ERROR: Can't cd '$SYSTMP'.\n";
    opendir TMPDIR, "$tmpdir" 
	or print "! ERROR: Can't read directory '$tmpdir'.\n";

    @alltmps=readdir TMPDIR;
    closedir TMPDIR;

    foreach $file (@alltmps) {
	next if $file=~/^\./;
	unlink "$tmpdir/$file";
    }
    rmdir "$tmpdir" or die "! ERROR: Can't remove directory '$tmpdir'.\n";
}

# Signal handlers.

my @common_signals=qw(INT ILL FPE SEGV TERM ABRT);
my @signals_UNIX=qw(QUIT BUS PIPE);
my @signals_Win32=qw(BREAK);


if (win32) {
    @signals=(@common_signals, @signals_Win32);
} else {
    @signals=(@common_signals, @signals_UNIX);
}

debug_msg "Supported signals: @signals.";

foreach my $signal (@signals) {
    $SIG{"$signal"}=\&remove_tmpdir;
}


my $has_wget=0;

if (win32) {
    $has_wget=1; ## shipped with TL.
} elsif (which "wget") {
    $has_wget=1; ## wget is in PATH.
}


if ($opt_debug) {
    my @PATH;
    if (win32) {
	@PATH=split ';', $ENV{'PATH'};
    } else {
	@PATH=split ':', $ENV{'PATH'};
    }
    my $index=0;

    foreach my $dir (@PATH) {
	debug_msg "PATH\[$index\]: '$dir'.";
	++$index;
    }
}

debug_msg "No wget binary found on your system, trying curl." 
    unless ($has_wget);

if ($has_wget) {
    debug_msg "Running 'wget $getfont_url'.";
    system ('wget', "$getfont_url") == 0 
	or die "! Error: Can't execute wget.\n";
} else {
    debug_msg "Running 'curl -O $getfont_url'.";
    system ('curl', '-O', "$getfont_url") == 0 
	or die "! Error: Can't execute curl.\n";
}

# Download the fonts.

my @getfont=('perl', "./getfont$TL_version", '--getnonfreefonts');
push @getfont, "--installroot=$INSTALLROOT";
push @getfont, '--lsfonts' if $opt_lsfonts;
push @getfont, '--force' if $opt_force;
push @getfont, '--debug' if $opt_debug;
push @getfont, '--verbose' if $opt_verbose;
push @getfont, '--sys' if $sys;
push @getfont, '--all' if $opt_all;
push @getfont, '--http' if $opt_http;
push @getfont, '--use_curl' unless ($has_wget);
push @getfont, @allpackages;

debug_msg "Running '@getfont'.";

system @getfont;

# Evaluate the exit status.  It will be 2 if something had been
# installed and in this case we need to run texhash/updmap.

my $exit_code=$?;
my $exit_status=int($exit_code/256);

if ($sys) {		     
    debug_msg "Info: Execute updmap-sys if exit status is 2.";
} else {
    debug_msg "Info: Execute updmap if exit status is 2.";
}

debug_msg "Exit status of getfont$TL_version is $exit_status.";

if ($exit_status==2) {
    print "\n";
    system 'texhash', "$INSTALLROOT";
    $updmap_command=($sys)? 'updmap-sys':'updmap';
    @updmap=("$updmap_command");
    push @updmap, '--quiet' unless $opt_verbose;
    print "\n";
    message "Updating map files ($updmap_command)...";
    system @updmap;
    
    message $?? '[failed]':'[done]';
}

remove_tmpdir;

__END__
