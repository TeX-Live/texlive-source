#!/usr/bin/env perl
# updmap-setup-kanji: setup Japanese font embedding
#
# Copyright 2004-2006 by KOBAYASHI R. Taizo for the shell version (updmap-otf)
# Copyright 2011-2012 by PREINING Norbert
#
# This file is licensed under GPL version 3 or any later version.
# For copyright statements see end of file.
#
# 31 Jan 2012
#    rewrite in Perl, so that the script is usable under Windows
#    add command line options for help, dry-run, etc
#    support jis2004 via cmd line -jis2004
# 27 Jan 2012 by PREINING Norbert <preining@logic.at>  v0.9.2
#    support IPA and IPAex fonts
#    improve and extended documentation
# 11 Nov 2011 by PREINING Norbert <preining@logic.at>  v0.9.1
#    use kpsewhich for finding fonts
#    use updmap-sys --setoption kanjiEmbed to select the font family
#    use current names of map files
#    use different font name for Kozuka font, as used in the map file
#    get state from updmap.cfg, not from some state file
# 27 May 2006 by KOBAYASHI R. Taizo <tkoba965@mac.com> v0.9
#    use noEmbed.map instead of noEmbeddedFont.map
# 10 Jun 2005 by KOBAYASHI R. Taizo <tkoba965@mac.com> v0.8
#    modified to use updmap-sys in teTeX3
# 07 Nov 2004 by KOBAYASHI R. Taizo <tkoba965@mac.com> v0.7
#    do not echo back the message of updmap.
# 17 Oct 2004 by KOBAYASHI R. Taizo <tkoba965@mac.com> v0.6
#    set hiragino map file if nofont is installed and arg is auto.
# 04 Oct 2004 by KOBAYASHI R. Taizo <tkoba965@mac.com> v0.5
#    handl standby map files more strictly
# 20 Sep 2004 by KOBAYASHI R. Taizo <tkoba965@mac.com> v0.4
#    hand over current status to map file installer
# 19 Sep 2004 by KOBAYASHI R. Taizo <tkoba965@mac.com> v0.3
#    handl *-udvips.map in TEXMF/dvipdfm/config/otf/
# 02 Mar 2004 by KOBAYASHI R. Taizo <tkoba@ike-dyn.ritsumei.ac.jp> v0.2
#    added noFont-udvips.map
# 28 Feb 2004 by KOBAYASHI R. Taizo <tkoba@ike-dyn.ritsumei.ac.jp> v0.1

$^W = 1;
use Getopt::Long qw(:config no_autoabbrev ignore_case_always);
use strict;

my $prg = "updmap-setup-kanji";
my $version = '$Id$';

my $updmap_real = "updmap-sys";
my $updmap = $updmap_real;

my $dry_run = 0;
my $opt_help = 0;
my $opt_jis = 0;

if (! GetOptions(
        "n|dry-run" => \$dry_run,
        "h|help" => \$opt_help,
        "jis2004" => \$opt_jis,
        "version" => sub { print &version(); exit(0); }, ) ) {
  die "Try \"$0 --help\" for more information.\n";
}

if ($dry_run) {
  $updmap = "echo updmap-sys"; 
}

if ($opt_help) {
  Usage();
  exit 0;
}

#
# representatives of support font families
#
my %representatives = (
  hiragino => "HiraMinPro-W3.otf",
  morisawa => "A-OTF-RyuminPro-Light.otf",
  kozuka   => "KozMinPro-Regular.otf",
  ipa      => "ipam.ttf",
  ipaex    => "ipaexm.ttf",
);
my %available;


main(@ARGV);

sub version {
  my $ret = sprintf "%s (perl) version %s\n", $prg, $version;
  return $ret;
}

sub Usage {
  my $usage = <<"EOF";
  $prg  Set up embedding of Japanese fonts via updmap.cfg.

                 This script searches for some of the most common fonts
                 for embedding into pdfs by dvipdfmx.

  Usage:  $prg {<fontname>|auto|nofont|status}

     <fontname>  embed fonts as defined by the map file otf-<fontname>.map
                 if it exists.
     auto:       embed one of the following supported font families
                 automatically:
                   hiragino, morisawa, kozuka, ipaex, ipa
     nofont:     embed no fonts (and rely on system fonts when displaying pdfs)
                 If your system does not have any of the supported font 
                 families as specified above, this target is selected 
                 automatically.
     status:     get information about current environment and usable font map

EOF
;
  print $usage;
  exit 0;
}



###
### Check Installed Font
###

sub CheckInstallFont {
  for my $k (keys %representatives) {
    my $f = `kpsewhich $representatives{$k}`;
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
  my $f = `kpsewhich $mapf 2>/dev/null`;
  my $ret = $?;
  if (wantarray) {
    return (!$ret, $f);
  } else {
    return (!$ret);
  }
}

sub GetStatus {
  my $val = `$updmap_real --quiet --showoption kanjiEmbed`;
  my $STATUS;
  if ($val =~ m/^kanjiEmbed=(.*)( \()?/) {
    $STATUS = $1;
    $STATUS =~ s/\s*$//;
    $STATUS =~ s/^"(.*)"$/\1/;
  } else {
    printf STDERR "Cannot find status of current kanjiEmbed setting via updmap-sys --showoption!\n";
    exit 1;
  }

  if (check_mapfile("otf-$STATUS.map")) {
    print "CURRENT map file : otf-$STATUS.map\n";
  } else {
    print "WARNING: Currently selected map file cannot be found: otf-$STATUS.map\n";
  }

  for my $k (sort keys %representatives) {
    my $MAPFILE = "otf-$k.map";
    next if ($MAPFILE eq "otf-$STATUS.map");
    if (check_mapfile($MAPFILE)) {
      if ($available{$k}) {
        print "Standby map file : $MAPFILE\n";
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
  my $MAPFILE = "otf-$rep.map";
  if (check_mapfile($MAPFILE)) {
    print "Setting up ... $MAPFILE\n";
    system("$updmap -setoption kanjiEmbed $rep");
    if ($opt_jis) {
      system("$updmap -setoption kanjiVariant -04");
    } else {
      system("$updmap -setoption kanjiVariant \"\"");
    }
    system("$updmap");
  } else {
    print "NOT EXIST $MAPFILE\n";
    exit 1;
  }
}

sub SetupReplacement {
  my $rep = shift;
  if (defined($representatives{$rep})) {
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
      if (defined($representatives{$STATUS}) && $available{$STATUS}) {
        return SetupMapFile($STATUS);
      } else {
        if (!($STATUS eq "noEmbed" || $STATUS eq "")) {
          # some unknown setting is set up currently, overwrite, but warn
          print "Previous setting $STATUS is unknown, replacing it!\n"
        }
        # if we are in the noEmbed or nothing set case, but one
        # of the three fonts hiragino/morisawa/kozuka are present
        # then use them
        for my $i (qw/hiragino morisawa kozuka ipaex ipa/) {
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

  # mktexlsr 2> /dev/null

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
# --------------------------------------------------------
# copyright statement は簡単に以下で結構です。
#
#        Copyright 2004-2006 by KOBAYASHI Taizo
#
# license も特にこだわりはありませんので
#
#        GPL(v3)
#
# で結構です。
# --------------------------------------------------------
#
# PREINING Norbert
# as author and maintainer of the current file
#
#
### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim: set tabstop=2 expandtab:
