#!/usr/bin/env perl
# kanji-config-updmap: setup Japanese font embedding
# Version 20170624.0
#
# formerly known as updmap-setup-kanji
#
# Copyright 2004-2006 by KOBAYASHI R. Taizo for the shell version (updmap-otf)
# Copyright 2011-2017 by PREINING Norbert
# Copyright 2016-2017 by Japanese TeX Development Community
#
# This file is licensed under GPL version 3 or any later version.
# For copyright statements see end of file.
#
# For development see
#  https://github.com/texjporg/jfontmaps
#
# For a changelog see the git log
# 

$^W = 1;
use Getopt::Long qw(:config no_autoabbrev ignore_case_always);
use strict;

my $prg = "kanji-config-updmap";
my $version = '20170624.0';

my $updmap_real = "updmap";
my $updmap = $updmap_real;

my $dry_run = 0;
my $opt_help = 0;
my $opt_jis = 0;
my $opt_sys = 0;
my $opt_user = 0;
my $opt_mode = "ja";
my $opt_old = 0;

if (! GetOptions(
        "n|dry-run" => \$dry_run,
        "h|help" => \$opt_help,
        "jis2004" => \$opt_jis,
        "mode=s"   => \$opt_mode,
        "ja"       => sub { $opt_mode = "ja"; },
        "sc"       => sub { $opt_mode = "sc"; },
        "tc"       => sub { $opt_mode = "tc"; },
        "ko"       => sub { $opt_mode = "ko"; },
        "sys"      => \$opt_sys,
        "user"     => \$opt_user,
        "old"      => \$opt_old,
        "version"  => sub { print &version(); exit(0); }, ) ) {
  die "Try \"$0 --help\" for more information.\n";
}


sub win32 { return ($^O=~/^MSWin(32|64)$/i); }

my $nul = (win32() ? 'nul' : '/dev/null') ;

if ($opt_user && $opt_sys) {
  die "Only one of -user and -sys can be used!";
}

if (defined($ARGV[0]) && $ARGV[0] ne "status") {
  if (!($opt_user || $opt_sys)) {
    die "Either -user or -sys mode is required.";
  }
}


if ($dry_run) {
  $updmap = "echo updmap"; 
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

if ($opt_help) {
  Usage();
  exit 0;
}

#
# representatives of support font families
#
my %representatives = (
  "ja" => {
    "hiragino"      => "HiraMinPro-W3.otf",
    "hiragino-pron" => "HiraMinProN-W3.otf",
    "hiragino-elcapitan" => "HiraginoSerif-W3.ttc",
    "hiragino-elcapitan-pron" => "HiraginoSerif-W3.ttc",
    "toppanbunkyu-sierra" => "ToppanBunkyuGothic.ttc",
    "morisawa"      => "A-OTF-RyuminPro-Light.otf",
    "morisawa-pr6n" => "A-OTF-RyuminPr6N-Light.otf",
    "kozuka"        => "KozMinPro-Regular.otf",
    "kozuka-pr6n"   => "KozMinPr6N-Regular.otf",
    "kozuka-pr6"    => "KozMinProVI-Regular.otf",
    "ipa"           => "ipam.ttf",
    "ipaex"         => "ipaexm.ttf",
    "moga-mobo"     => "mogam.ttc",
    "moga-mobo-ex"  => "mogam.ttc",
    "ume"           => "ume-tmo3.ttf",
    "ms"            => "msgothic.ttc",
    "ms-osx"        => "MS-Gothic.ttf",
    "yu-win"        => "yugothib.ttf",
    "yu-win10"      => "YuGothB.ttc",
    "yu-osx"        => "YuMin-Medium.otf",
    "canon"         => "FGCCHMW3.TTC",
  },
  "sc" => {
    "ms"            => "simsun.ttc",
    #"sinotype"      => "STSong.ttf", # removed
    "adobe"         => "AdobeSongStd-Light.otf",
    "fandol"        => "FandolSong-Regular.otf",
    "founder"       => "FZSSK.TTF",
    "arphic"        => "gbsn00lp.ttf",
    "cjkunifonts"   => "uming.ttc",
    "cjkunifonts-ttf" => "uming.ttf",
  },
  "tc" => {
    "ms"            => "msjh.ttf",
    "ms-win10"      => "msjh.ttc",
    "dynacomware"   => "LiSongPro.ttf",
    "adobe"         => "AdobeMingStd-Light.otf",
    "arphic"        => "bsmi00lp.ttf",
    "cjkunifonts"   => "uming.ttc",
    "cjkunifonts-ttf" => "uming.ttf",
  },
  "ko" => {
    "ms"            => "batang.ttc",
    "apple"         => "AppleMyungjo.ttf",
    "adobe"         => "AdobeMyungjoStd-Medium.otf",
    "baekmuk"       => "dotum.ttf", # slightly safer than batang.ttf on case-insentive systems
    "unfonts"       => "UnBatang.ttf",
    "solaris"       => "h2mjsm.ttf",
  }
);
my %available;


main(@ARGV);

sub version {
  my $ret = sprintf "%s version %s\n", 
    $prg, $version;
  return $ret;
}

sub Usage {
  my $usage = <<"EOF";
  $prg  Set up embedding of Japanese/Chinese/Korean fonts via updmap.cfg.

                 This script searches for some of the most common fonts
                 for embedding into pdfs by dvipdfmx.

                 In addition it allows to set up arbitrary font families
                 to be embedded into the generated pdf files, as long
                 as at least the map file otf-<family>.map is present.
                 Other map files that will be used if available are
                  
                 For Japanese:
                   ptex-<family>.map
                   uptex-<family>.map
                   otf-<family>.map
                   otf-up-<family>.map

                 For Korean, Traditional Chinese and Simplified Chinese:
                   uptex-<NN>-<family>.map
                   otf-<NN>-<family>.map
                 (NN being: ko, tc, sc)

  Please see the documentation of updmap for details (updmap --help).

  Usage:  $prg [OPTION] {<fontname>|auto|nofont|status}

     <family>    embed an arbitrary font family <family>, at least the
                 map file otf-<family>.map has to be available.
     auto:       embed one of the following supported font families
                 automatically:
                   hiragino, hiragino-pron, hiragino-elcapitan,
                   hiragino-elcapitan-pron, toppanbunkyu-sierra,
                   morisawa, morisawa-pr6n,
                   kozuka, kozuka-pr6n, kozuka-pr6,
                   ipa, ipaex, moga-mobo, moga-mobo-ex, ume,
                   ms, ms-osx,
                   yu-win, yu-win10, yu-osx,
                   canon
                 and fall back to not embedding any font if none of them
                 is available
     nofont:     embed no fonts (and rely on system fonts when displaying pdfs)
                 If your system does not have any of the supported font 
                 families as specified above, this target is selected 
                 automatically.
     status:     get information about current environment and usable font map

  Options:
    -n, --dry-run  do not actually run updmap
    -h, --help     show this message and exit
    --mode=NN      setup for Japanese (NN=ja), Korean (NN=ko),
                   Simplified Chinese (NN=sc), Traditional Chinese (NN=tc)
    --NN           short for --mode=NN
    --jis2004      use JIS2004 variants for default fonts of (u)pTeX
    --sys          run in sys mode, i.e., call updmap -sys
    --user         run in user mode, i.e., call updmap -user or updmap
                   by checking the version of the updmap script. If a
                   non-parsable output of `updmap --version' is found, a new
                   updmap with --user option is assumed. If this is not the
                   case, use --old.
    --old          Makes $prg call `updmap' without --user argument in user mode.
    --version      show version information and exit

EOF
;
  print $usage;
  exit 0;
}



###
### Check Installed Font
###

sub CheckInstallFont {
  for my $k (keys %{$representatives{$opt_mode}}) {
    my $f = `kpsewhich $representatives{$opt_mode}{$k}`;
    if (! $?) {
      $available{$k} = chomp($f);
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

sub GetStatus {
  my $val = `$updmap_real --quiet --showoption ${opt_mode}Embed`;
  my $STATUS;
  if ($val =~ m/^${opt_mode}Embed=([^()\s]*)(\s+\()?/) {
    $STATUS = $1;
  } else {
    printf STDERR "Cannot find status of current ${opt_mode}Embed setting via updmap --showoption!\n";
    exit 1;
  }

  my $testmap = ($opt_mode eq "ja" ? "ptex-$STATUS.map" : "uptex-${opt_mode}-$STATUS.map");
  if (check_mapfile($testmap)) {
    print "CURRENT family for $opt_mode: $STATUS\n";
  } else {
    print "WARNING: Currently selected map file for $opt_mode cannot be found: $testmap\n";
  }

  for my $k (sort keys %{$representatives{$opt_mode}}) {
    my $MAPFILE = ($opt_mode eq "ja" ? "ptex-$k.map" : "uptex-${opt_mode}-$k.map");
    next if ($MAPFILE eq $testmap);
    if (check_mapfile($MAPFILE)) {
      if ($available{$k}) {
        print "Standby family : $k\n";
      }
    }
  }
  return $STATUS;
}

###
### Setup Map files
###

sub SetupMapFile {
  my $rep = shift;
  my $MAPFILE = ($opt_mode eq "ja" ? "ptex-$rep.map" : "uptex-${opt_mode}-$rep.map");
  if (check_mapfile($MAPFILE)) {
    print "Setting up ... $MAPFILE\n";
    system("$updmap --quiet --nomkmap --nohash -setoption ${opt_mode}Embed $rep");
    if ($opt_jis) {
      system("$updmap --quiet --nomkmap --nohash -setoption jaVariant -04");
    } else {
      system("$updmap --quiet --nomkmap --nohash -setoption jaVariant \"\"");
    }
    system("$updmap");
  } else {
    print "NOT EXIST $MAPFILE\n";
    exit 1;
  }
}

sub SetupReplacement {
  my $rep = shift;
  if (defined($representatives{$opt_mode}{$rep})) {
    if ($available{$rep}) {
      return SetupMapFile($rep);
    } else {
      printf STDERR "$rep not available, falling back to auto!\n";
      return SetupReplacement("auto");
    }
  } else {
    if ($rep eq "nofont") {
      return SetupMapFile("noEmbed");
    } elsif ($rep eq "auto") {
      my $STATUS = GetStatus();
      # first check if we have a status set and the font is installed
      # in this case don't change anything, just make sure
      if (defined($representatives{$opt_mode}{$STATUS}) && $available{$STATUS}) {
        return SetupMapFile($STATUS);
      } else {
        if (!($STATUS eq "noEmbed" || $STATUS eq "")) {
          # some unknown setting is set up currently, overwrite, but warn
          print "Previous setting $STATUS for $opt_mode is unknown, replacing it!\n"
        }
        # if we are in the noEmbed or nothing set case, but one
        # of the supported fonts are present then use them
        # (originally it said "three fonts hiragino/morisawa/kozuka", but the code below
        #  was different from this statement; changed to "supported fonts" on 2016/12/08)
        my @testlist;
        if ($opt_mode eq 'ja') {
          @testlist = qw/
            morisawa-pr6n morisawa
            hiragino-pron hiragino hiragino-elcapitan-pron hiragino-elcapitan
            kozuka-pr6n kozuka-pr6 kozuka
            toppanbunkyu-sierra
            yu-osx yu-win10 yu-win10 canon
            ms ms-osx moga-mobo moga-mobo-ex ume
            ipa ipaex/;
        } elsif ($opt_mode eq 'tc') {
          @testlist = qw/dynacomware adobe ms-win10 ms arphic cjkunifonts cjkunifonts-ttf/;
        } elsif ($opt_mode eq 'sc') {
          @testlist = qw/fandol adobe ms arphic cjkunifonts cjkunifonts-ttf/;
        } elsif ($opt_mode eq 'ko') {
          @testlist = qw/apple adobe ms baekmuk unfonts solaris/;
        }
        # else cannot happen unless getopt is broken

        for my $i (@testlist) {
          if ($available{$i}) {
            return SetupMapFile($i);
          }
        }
        # still here, no map file found!
        return SetupMapFile("noEmbed");
      }
    } else {
      # anything else is treated as a map file name
      return SetupMapFile($rep);
    }
  }
}

###
### MAIN
###

sub main {
  my ($a, $b) = @_;

  CheckInstallFont();

  if (!defined($a) || defined($b)) {
    Usage();
    exit 1;
  }

  if ($a eq "status") {
    GetStatus();
    exit 0;
  }

  return SetupReplacement($a);
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
