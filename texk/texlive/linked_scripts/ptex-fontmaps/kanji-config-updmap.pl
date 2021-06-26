#!/usr/bin/env perl
# kanji-config-updmap: setup Japanese font embedding
# Version 20210625.0
#
# formerly known as updmap-setup-kanji
#
# Copyright 2004-2006 by KOBAYASHI R. Taizo for the shell version (updmap-otf)
# Copyright 2011-2021 by PREINING Norbert
# Copyright 2016-2021 by Japanese TeX Development Community
#
# This file is licensed under GPL version 3 or any later version.
# For copyright statements see end of file.
#
# For development see
#  https://github.com/texjporg/ptex-fontmaps
#
# For a changelog see the git log
#

$^W = 1;
use Getopt::Long qw(:config no_autoabbrev ignore_case_always);
use strict;

my $prg = "kanji-config-updmap";
my $version = '20210625.0';

my $updmap_real = "updmap";
my $updmap = $updmap_real;

my $dry_run = 0;
my $opt_help = 0;
my $opt_jis = 0;
my $opt_sys = 0;
my $opt_user = 0;
my $opt_old = 0;
my $opt_force = 0;
my @opt_mode_list;
my $opt_mode_one;
my $opt_mode_ja;
my $opt_mode_sc;
my $opt_mode_tc;
my $opt_mode_ko;

if (! GetOptions(
        "n|dry-run" => \$dry_run,
        "h|help" => \$opt_help,
        "jis2004" => \$opt_jis,
        "mode=s"   => \$opt_mode_one,
        "ja=s"     => \$opt_mode_ja,
        "sc=s"     => \$opt_mode_sc,
        "tc=s"     => \$opt_mode_tc,
        "ko=s"     => \$opt_mode_ko,
        "sys"      => \$opt_sys,
        "user"     => \$opt_user,
        "old"      => \$opt_old,
        "force"    => \$opt_force,
        "version"  => sub { print &version(); exit(0); }, ) ) {
  die "Try \"$0 --help\" for more information.\n";
}

if ($opt_mode_one) {
  if (defined($opt_mode_ja) || defined($opt_mode_sc) ||
      defined($opt_mode_tc) || defined($opt_mode_ko)) {
    die "Options --ja/--sc/--tc/--ko are invalid with --mode=NN!\n";
  }
  # define a corresponding option by empty string
  if ($opt_mode_one eq "ja") {
    $opt_mode_ja = '';
  } elsif ($opt_mode_one eq "sc") {
    $opt_mode_sc = '';
  } elsif ($opt_mode_one eq "tc") {
    $opt_mode_tc = '';
  } elsif ($opt_mode_one eq "ko") {
    $opt_mode_ko = '';
  } else {
    die "Unknown mode $opt_mode_one!";
  }
}
push @opt_mode_list, "ja" if (defined($opt_mode_ja));
push @opt_mode_list, "sc" if (defined($opt_mode_sc));
push @opt_mode_list, "tc" if (defined($opt_mode_tc));
push @opt_mode_list, "ko" if (defined($opt_mode_ko));
if (!@opt_mode_list) {
  # default mode needs to be set, define it by empty string
  $opt_mode_one = "ja";
  $opt_mode_ja = '';
  push @opt_mode_list, "ja";
}

sub win32 { return ($^O=~/^MSWin(32|64)$/i); }
my $nul = (win32() ? 'nul' : '/dev/null') ;

if (defined($ARGV[0]) && $ARGV[0] ne "status") {
  if (!($opt_user || $opt_sys)) {
    die "Either -user or -sys mode is required.";
  }
}

if ($opt_user && $opt_sys) {
  die "Only one of -user and -sys can be used!";
}

if ($opt_sys) {
  $updmap = "$updmap --sys" ;
  $updmap_real = "$updmap_real --sys" ;
} else {
  # TeX Live 2017 requires --user option
  # try to determine the version of updmap installed
  my $updver = `updmap --version 2>&1`;
  if ($updver =~ m/^updmap version r([0-9]*) /) {
    if ($1 >= 44080) {
      $updmap = "$updmap --user" ;
      $updmap_real = "$updmap_real --user" ;
    } # else nothing to do, already set up for old updmap
  } else {
    # not recognized updmap -> assume new updmap unless --old
    if (!$opt_old) {
      $updmap = "$updmap --user" ;
      $updmap_real = "$updmap_real --user" ;
    }
  }
}
if ($dry_run) {
  $updmap = "echo updmap";
}

if ($opt_help) {
  Usage();
  exit(0);
}

#
# representatives of support font families
#
my %representatives;
my %ai0flags;
my @databaselist = "ptex-fontmaps-data.dat";
push @databaselist, "ptex-fontmaps-macos-data.dat" if (macosx_new());


main(@ARGV);

sub macosx { return ($^O=~/^darwin$/i); }

sub macosx_new {
  if (macosx()) {
    my $macos_ver = `sw_vers -productVersion`;
    my $macos_ver_major = $macos_ver;
    $macos_ver_major =~ s/^(\d+)\.(\d+).*/$1/;
    my $macos_ver_minor = $macos_ver;
    $macos_ver_minor =~ s/^(\d+)\.(\d+).*/$2/;
    if ($macos_ver_major==10 && $macos_ver_minor>=11) {
      return 1; # macOS 10.11 (El Capitan) or later
    } elsif ($macos_ver_major>=11) {
      return 1; # macOS 11.0 (Big Sur) or later
    }
  }
  return 0;
}

sub version {
  my $ret = sprintf "%s version %s\n", $prg, $version;
  return $ret;
}

sub Usage {
  my $usage = <<"EOF";
  $prg $version
  Set up embedding of Japanese/Chinese/Korean fonts via updmap.cfg.

    This script searches for some of the most common fonts
    for embedding into pdfs by dvipdfmx.

    In addition it allows to set up arbitrary font families
    to be embedded into the generated pdf files, as long
    as at least the representative map file is present.
    Other map files will be used if available:

      For Japanese:
        ptex-<family>.map (representative map file)
        uptex-<family>.map
        otf-<family>.map
        otf-up-<family>.map

      For Simplified Chinese, Traditional Chinese and Korean:
        uptex-<NN>-<family>.map (representative map file)
        otf-<NN>-<family>.map
       (NN being: sc, tc, ko)

  Please see the documentation of updmap for details (updmap --help).

  Usage:  $prg [OPTION] {<fontname>|auto|nofont|status}

     <family>    Embed an arbitrary font family <family>, at least
                 the representative map file has to be available.
     auto:       If the current status is noEmbed or unknown, try to embed
                 one of the supported font families automatically.
                 If none of them is available, fall back to nofont
     nofont:     Embed no fonts (and rely on system fonts when displaying pdfs).
                 If your system does not have any of the supported font
                 families, this target is selected automatically.
     status:     Get information about current environment and usable font maps.

  Options:
    -n, --dry-run  Do not actually run updmap
    -h, --help     Show this message and exit
    --mode=NN      Setup for Japanese (NN=ja), Korean (NN=ko),
                   Simplified Chinese (NN=sc), Traditional Chinese (NN=tc)
    --NN           Shorthand for --mode=NN
    --jis2004      Use JIS2004 variants for default fonts of (u)pTeX
    --sys          Run in sys mode, i.e., call updmap -sys
    --user         Run in user mode, i.e., call updmap -user or updmap,
                   by checking the version of the updmap script.
                   If a non-parsable output of `updmap --version' is found,
                   a new updmap with --user option is assumed.
                   If this is not the case, explicitly use --old.
    --old          Makes $prg call `updmap' without --user argument in user mode.
    --force        Set up font embedding even if the font is not available.
    --version      Show version information and exit

EOF
;
  print $usage;
  exit(0);
}



###
### Collect Database Lines
###

sub InitDatabase {
  %representatives = ();
  %ai0flags = ();
}

sub ReadDatabase {
  my @curdbl;
  # open database
  for my $f (@databaselist) {
    my $foo = kpse_miscfont($f);
    if (!open(FDB, "<$foo")) {
      printf STDERR "Cannot find $f, skipping!\n";
      next;
    }
    @curdbl = <FDB>;
    close(FDB);
    # parse lines
    my $lineno = 0;
    chomp(@curdbl);
    push @curdbl, ""; # add a "final empty line" to easy parsing
    for my $l (@curdbl) {
      $lineno++;
      next if ($l =~ m/^\s*$/); # skip empty line
      next if ($l =~ m/^\s*#/); # skip comment line
      $l =~ s/\s*#.*$//; # skip comment after '#'
      if ($l =~ m/^JA\*\((\d+)\):\s*(.*):\s*(.*)$/) { # no -04 map
        $representatives{'ja'}{$2}{'priority'} = $1;
        $representatives{'ja'}{$2}{'file'} = $3;
        $representatives{'ja'}{$2}{'nojis04'} = 1;
        next;
      }
      if ($l =~ m/^JA\((\d+)\):\s*(.*):\s*(.*)$/) {
        $representatives{'ja'}{$2}{'priority'} = $1;
        $representatives{'ja'}{$2}{'file'} = $3;
        next;
      }
      if ($l =~ m/^SC\((\d+)\):\s*(.*):\s*(.*)$/) {
        $representatives{'sc'}{$2}{'priority'} = $1;
        $representatives{'sc'}{$2}{'file'} = $3;
        next;
      }
      if ($l =~ m/^TC\((\d+)\):\s*(.*):\s*(.*)$/) {
        $representatives{'tc'}{$2}{'priority'} = $1;
        $representatives{'tc'}{$2}{'file'} = $3;
        next;
      }
      if ($l =~ m/^KO\((\d+)\):\s*(.*):\s*(.*)$/) {
        $representatives{'ko'}{$2}{'priority'} = $1;
        $representatives{'ko'}{$2}{'file'} = $3;
        next;
      }
      if ($l =~ m/^JA-AI0\*:\s*(.*):\s*(.*)$/) { # no -04 map
        $representatives{'ja'}{$1}{'priority'} = 9999; # lowest
        $representatives{'ja'}{$1}{'file'} = $2;
        $representatives{'ja'}{$1}{'nojis04'} = 1;
        $ai0flags{'ja'}{$1} = 1;
        next;
      }
      if ($l =~ m/^JA-AI0:\s*(.*):\s*(.*)$/) {
        $representatives{'ja'}{$1}{'priority'} = 9999; # lowest
        $representatives{'ja'}{$1}{'file'} = $2;
        $ai0flags{'ja'}{$1} = 1;
        next;
      }
      if ($l =~ m/^SC-AI0:\s*(.*):\s*(.*)$/) {
        $representatives{'sc'}{$1}{'priority'} = 9999; # lowest
        $representatives{'sc'}{$1}{'file'} = $2;
        $ai0flags{'sc'}{$1} = 1;
        next;
      }
      if ($l =~ m/^TC-AI0:\s*(.*):\s*(.*)$/) {
        $representatives{'tc'}{$1}{'priority'} = 9999; # lowest
        $representatives{'tc'}{$1}{'file'} = $2;
        $ai0flags{'tc'}{$1} = 1;
        next;
      }
      if ($l =~ m/^KO-AI0:\s*(.*):\s*(.*)$/) {
        $representatives{'ko'}{$1}{'priority'} = 9999; # lowest
        $representatives{'ko'}{$1}{'file'} = $2;
        $ai0flags{'ko'}{$1} = 1;
        next;
      }
      # we are still here??
      die "Cannot parse \"$foo\" at line $lineno,
           exiting. Strange line: >>>$l<<<\n";
    }
  }
  if (!%representatives) {
    die "Candidate list is empty, cannot proceed!\n";
  }
}

sub kpse_miscfont {
  my ($file) = @_;
  my $foo = '';
  # first, prioritize GitHub repository diretory structure
  $foo = "database/$file" if (-f "database/$file");
  if ($foo eq "") {
    chomp($foo = `kpsewhich -format=miscfont $file`);
  }
  return $foo;
}

###
### Check Installed Font
###

sub CheckInstallFont {
  for my $opt_mode (@opt_mode_list) {
    for my $k (keys %{$representatives{$opt_mode}}) {
      my $f = `kpsewhich $representatives{$opt_mode}{$k}{'file'}`;
      if ($?) {
        $representatives{$opt_mode}{$k}{'available'} = "";
      } else {
        $representatives{$opt_mode}{$k}{'available'} = chomp($f);
      }
    }
  }
}

###
### GetStatus
###

sub check_mapfile {
  my $mapf = shift;
  my $f = `kpsewhich $mapf 2> $nul`;
  my $ret = $?;
  if (wantarray) {
    return (!$ret, $f);
  } else {
    return (!$ret);
  }
}

sub gen_mapfile {
  my $opt_mode = shift;
  my $map_base = shift;
  # returns a representative map file name
  # ptex-${map_base}.map also exists for Japanese AI0 fonts,
  # but it is a stub so we use uptex-${map_base}.map instead
  return ($opt_mode eq "ja" ?
            ($ai0flags{$opt_mode}{$map_base} ?
               "uptex-${map_base}.map" :
               "ptex-${map_base}.map") :
            "uptex-${opt_mode}-${map_base}.map");
}

sub GetStatus {
  my $opt_mode = shift;
  my $val;
  my $STATUS = "";
  my $VARIANT = "";

  # fetch jaEmbed/scEmbed/tcEmbed/koEmbed
  $val = `$updmap_real --quiet --showoption ${opt_mode}Embed`;
  if ($val =~ m/^${opt_mode}Embed=([^()\s]*)(\s+\()?/) {
    $STATUS = $1;
  } else {
    die "Cannot find status of current ${opt_mode}Embed setting via updmap --showoption!\n";
  }
  # fetch jaVariant
  if ($opt_mode eq "ja") {
    $val = `$updmap_real --quiet --showoption ${opt_mode}Variant`;
    if ($val =~ m/^${opt_mode}Variant=([^()\s]*)(\s+\()?/) {
      $VARIANT = $1; # should be '' or '-04'
    } else {
      die "Cannot find status of current ${opt_mode}Variant setting via updmap --showoption!\n";
    }
  }

  my $testmap = gen_mapfile($opt_mode, "$STATUS$VARIANT");
  $VARIANT = "<empty>" if ($VARIANT eq ""); # for printing
  if (check_mapfile($testmap)) {
    print "CURRENT family for $opt_mode: $STATUS";
    print " (variant: $VARIANT)" if ($opt_mode eq "ja");
    print " (AI0)" if ($ai0flags{$opt_mode}{$STATUS});
    print "\n";
  } else {
    print STDERR "WARNING: Currently selected map file for $opt_mode cannot be found: $testmap\n";
  }

  for my $k (sort keys %{$representatives{$opt_mode}}) {
    my $MAPFILE = gen_mapfile($opt_mode, $k);
    next if ($MAPFILE eq $testmap);
    if (check_mapfile($MAPFILE)) {
      if ($representatives{$opt_mode}{$k}{'available'}) {
        print "Standby family : $k";
        print " (AI0)" if ($ai0flags{$opt_mode}{$k});
        print "\n";
      }
    }
  }
  return $STATUS;
}

###
### Setup Map files
###

sub SetupMapFile {
  my $opt_mode = shift;
  my $rep = shift;
  my $MAPFILE = gen_mapfile($opt_mode, $rep);
  if (check_mapfile($MAPFILE)) {
    print "Setting up ... $rep";
    print " (AI0)" if ($ai0flags{$opt_mode}{$rep});
    print " for $opt_mode\n";
    system("$updmap --quiet --nomkmap --nohash --setoption ${opt_mode}Embed $rep");
    if ($opt_mode eq "ja") {
      if ($opt_jis && $representatives{'ja'}{$rep}{'nojis04'}) {
        print STDERR "WARNING: No -04 map available, option --jis2004 ignored!\n";
        $opt_jis = 0;
      }
      if ($opt_jis) {
        system("$updmap --quiet --nomkmap --nohash --setoption jaVariant -04");
      } else {
        system("$updmap --quiet --nomkmap --nohash --setoption jaVariant \"\"");
      }
    }
  } else {
    die "NOT EXIST $MAPFILE\n";
  }
}

sub SetupReplacement {
  my $opt_mode = shift;
  my $rep = shift;
  if (defined($representatives{$opt_mode}{$rep})) {
    if ($representatives{$opt_mode}{$rep}{'available'} || $opt_force) {
      return SetupMapFile($opt_mode, $rep);
    } else {
      printf STDERR "$rep not available, falling back to auto!\n";
      return SetupReplacement($opt_mode, "auto");
    }
  } else {
    if ($rep eq "nofont") {
      return SetupMapFile($opt_mode, "noEmbed");
    } elsif ($rep eq "auto") {
      my $STATUS = GetStatus($opt_mode);
      # first check if we have a status set and the font is installed
      # in this case don't change anything, just make sure
      if (defined($representatives{$opt_mode}{$STATUS}) &&
          $representatives{$opt_mode}{$STATUS}{'available'}) {
        return SetupMapFile($opt_mode, $STATUS);
      } else {
        if (!($STATUS eq "noEmbed" || $STATUS eq "")) {
          # some unknown setting is set up currently, overwrite, but warn
          print STDERR "Previous setting $STATUS for $opt_mode is unknown, replacing it!\n"
        }
        # if we are in the noEmbed or nothing set case,
        # and if one of the supported fonts are present, then use them
        for my $i (sort { $representatives{$opt_mode}{$a}{'priority'}
                          <=>
                          $representatives{$opt_mode}{$b}{'priority'} }
                        keys %{$representatives{$opt_mode}}) {
          if ($representatives{$opt_mode}{$i}{'available'}) {
            return SetupMapFile($opt_mode, $i);
          }
        }
        # still here, no map file found!
        return SetupMapFile($opt_mode, "noEmbed");
      }
    } else {
      # anything else is treated as a map file name
      return SetupMapFile($opt_mode, $rep);
    }
  }
}

###
### MAIN
###

sub main {
  # Number of arguments allowed:
  #  0: should be only --NN=<family> lists ('=' can be omitted)
  #  1: treated as [--mode=NN] <family> ('=' can be omitted)
  #  2 or more: I can't handle!
  my ($a, $b) = @_;
  if (defined($b)) {
    die "Number of the arguments should be at most one!\n";
  }
  if ($a) {
    die "Strange argument found! >>>$a<<<\n" if (!$opt_mode_one);
    # argument is passed to defined-but-empty language mode
    if (defined($opt_mode_ja) && !$opt_mode_ja) {
      $opt_mode_ja = $a;
    } elsif (defined($opt_mode_sc) && !$opt_mode_sc) {
      $opt_mode_sc = $a;
    } elsif (defined($opt_mode_tc) && !$opt_mode_tc) {
      $opt_mode_tc = $a;
    } elsif (defined($opt_mode_ko) && !$opt_mode_ko) {
      $opt_mode_ko = $a;
    }
  } else {
    die "No family or operation is specified for $opt_mode_one!\n",
        "Try \"$0 --help\" for more information.\n" if ($opt_mode_one);
  }

  InitDatabase();
  ReadDatabase();
  CheckInstallFont();

  # if one of <family> arguments is "status", then
  # all arguments are forced into "status"
  if (($opt_mode_ja && ($opt_mode_ja eq "status")) ||
      ($opt_mode_sc && ($opt_mode_sc eq "status")) ||
      ($opt_mode_tc && ($opt_mode_tc eq "status")) ||
      ($opt_mode_ko && ($opt_mode_ko eq "status"))) {
    GetStatus("ja") if ($opt_mode_ja);
    GetStatus("sc") if ($opt_mode_sc);
    GetStatus("tc") if ($opt_mode_tc);
    GetStatus("ko") if ($opt_mode_ko);
  } else {
    SetupReplacement("ja", $opt_mode_ja) if ($opt_mode_ja);
    SetupReplacement("sc", $opt_mode_sc) if ($opt_mode_sc);
    SetupReplacement("tc", $opt_mode_tc) if ($opt_mode_tc);
    SetupReplacement("ko", $opt_mode_ko) if ($opt_mode_ko);
    system("$updmap");
  }
}

#
#
# Copyright statements:
#
# KOBAYASHI Taizo
# email to preining@logic.at
# Message-Id: <20120130.162953.59640143170594580.tkoba@cc.kyushu-u.ac.jp>
# Message-Id: <20120201.105639.625859878546968959.tkoba@cc.kyushu-u.ac.jp>
# --------------------------------------------------------
# copyright statement は簡単に以下で結構です。
#
#        Copyright 2004-2006 by KOBAYASHI Taizo
#
# では
#        GPL version 3 or any later version
#
# --------------------------------------------------------
#
# PREINING Norbert
# as author and maintainer of the current file
# Licensed under GPL version 3 or any later version
#
### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim: set tabstop=2 expandtab autoindent:
