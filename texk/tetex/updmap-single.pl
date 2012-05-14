#! /usr/bin/env perl
# updmap: utility to maintain map files for outline fonts.
#
# Copyright 2002-2011 Thomas Esser (for the shell version)
# Copyright 2002-2011 Fabrice Popineau (for the first perl version)
# Copyright 2009-2011 Reinhard Kotucha
# Copyright 2011-2012 Norbert Preining
#
# Anyone may freely use, modify, and/or distribute this file, without
# limitation.

BEGIN {
  $^W=1;
  chomp($TEXMFROOT = `kpsewhich -var-value=TEXMFROOT`);
  unshift (@INC, "$TEXMFROOT/tlpkg");
}

my $version = '$Id$';

use strict;
use TeXLive::TLUtils qw(mkdirhier mktexupd win32);
use Getopt::Long;
$Getopt::Long::autoabbrev=0;
Getopt::Long::Configure (qw(ignore_case_always));

my $short_progname = "updmap";

my $cnfFile;
my $cnfFileShort;
my $outputdir;
my $dvipsoutputdir;
my $pdftexoutputdir;
my $dvipdfmoutputdir;
my $pxdvioutputdir;
my $quiet;
my $nohash;
my $nomkmap;
my (@psADOBE, @fileADOBE, @fileADOBEkb, @fileURW);

my $enableItem;
my @setoptions = ();
my @showoptions = ();
my @showoption = ();
my @disableItem = ();
my $listmaps;
my $listavailablemaps;
my $syncwithtrees;

my $opt_edit;
my $opt_force;
my $opt_help;
my $dry_run;
my $TEXMFMAIN;
my $TEXMFVAR;

my $mode;
my $dvipsPreferOutline;
my $dvipsDownloadBase35;
my $pdftexDownloadBase14;
my $pxdviUse;
my $kanjiEmbed;
my $kanjiVariant;

my $dvips35;
my $pdftex35;
my $ps2pk35;

my $newcnf;
my $copy;
my %link;
my %maps;
my @missing;
my $writelog = 0;
my $cache = 0; # don't change!
my $pdftexStripEnc = 0;

# initialize mktexupd
my $updLSR=&mktexupd();
$updLSR->{mustexist}(0);

my @cfg = ();

&main;
exit 0;


# return program name + version string.
sub version {
  my $ret = sprintf "%s (TeX Live) version %s\n", $short_progname, $version;
  return $ret;
}

###############################################################################
# help()
#   display help message and exit
#
sub help {
  my $usage= <<"EOF";
Usage: $short_progname     [OPTION] ... [COMMAND]
   or: $short_progname-sys [OPTION] ... [COMMAND]

Update the default font map files used by pdftex, dvips, and dvipdfm(x),
(and optionally pxdvi) as determined by the configuration file updmap.cfg 
(the one returned by running "kpsewhich updmap.cfg").

Among other things, these map files are used to determine which fonts
should be used as bitmaps and which as outlines, and to determine which
fonts are embedded in the output.

By default, the TeX filename database (ls-R) is also updated.

Options:
  --cnffile FILE            read FILE for the updmap configuration
  --dvipdfmoutputdir DIR    specify output directory (kanjix.map)
  --dvipsoutputdir DIR      specify output directory (dvips syntax)
  --pdftexoutputdir DIR     specify output directory (pdftex syntax)
  --pxdvioutputdir DIR      specify output directory (pxdvi syntax)
  --outputdir DIR           specify output directory (for all files)
  --copy                    cp generic files rather than using symlinks
  --force                   recreate files even if config hasn't changed
  --nomkmap                 do not recreate map files
  --nohash                  do not run texhash
  -n, --dry-run             only show the configuration, no output
  --quiet, --silent         reduce verbosity

Commands:
  --help                    show this message and exit
  --version                 show version information and exit
  --showoptions ITEM        show alternatives for options
  --showoption ITEM         show value of option
  --setoption OPTION VALUE  set option, where OPTION is one of:
                             LW35, dvipsPreferOutline, dvipsDownloadBase35,
                             pdftexDownloadBase14, pxdviUse, kanjiEmbed,
                             or kanjiVariant
  --setoption OPTION=VALUE  as above, just different syntax
  --enable MAPTYPE MAPFILE  add "MAPTYPE MAPFILE" to updmap.cfg,
                             where MAPTYPE is one of: Map, MixedMap, KanjiMap
  --enable Map=MAPFILE      add \"Map MAPFILE\" to updmap.cfg
  --enable MixedMap=MAPFILE add \"MixedMap MAPFILE\" to updmap.cfg
  --enable KanjiMap=MAPFILE add \"KanjiMap MAPFILE\" to updmap.cfg
  --disable MAPFILE         disable MAPFILE, of any type
  --listmaps                list all active and inactive maps
  --listavailablemaps       same as --listmaps, but without
                             unavailable map files
  --syncwithtrees           disable unavailable map files in updmap.cfg

Explanation of the map types: the (only) difference between Map and
MixedMap is that MixedMap entries are not added to psfonts_pk.map.
The purpose is to help users with printers that render Type 1 outline
fonts worse than mode-tuned Type 1 bitmap fonts.  So, MixedMap is used
for fonts that are available as both Type 1 and Metafont.

KanjiMap, on the other hand, specifies possibly several map files for
Kanji font support; see below.  KanjiMap entries are added to
psfonts_t1.map and kanjix.map.

Explanation of the --setoption possibilities:

  dvipsPreferOutline    true|false  (default true)
    Whether dvips uses bitmaps or outlines, when both are available.
  dvipsDownloadBase35   true|false  (default false)
    Whether dvips includes the standard 35 PostScript fonts in its output.
  pdftexDownloadBase14  true|false   (default true)
    Whether pdftex includes the standard 14 PDF fonts in its output.
  pxdviUse              true|false  (default false)
    Whether maps for pxdvi (Japanese-patched xdvi) is under control of updmap.
  kanjiEmbed            any string, default noEmbed
    specify embedding and choice of Kanji fonts, 
  kanjiVariant          any string, default empty
    used as suffix for some Kanji maps; set to -04 for JIS2004.
  LW35                  URWkb|URW|ADOBEkb|ADOBE  (default URWkb)
    Adapt the font and file names of the standard 35 PostScript fonts.
    URWkb    URW fonts with "berry" filenames    (e.g. uhvbo8ac.pfb)
    URW      URW fonts with "vendor" filenames   (e.g. n019064l.pfb)
    ADOBEkb  Adobe fonts with "berry" filenames  (e.g. phvbo8an.pfb)
    ADOBE    Adobe fonts with "vendor" filenames (e.g. hvnbo___.pfb)

  These options are only read and acted on by updmap; dvips, pdftex, etc.,
  do not know anything about them.  They work by changing the default map
  file which the programs read, so they can be overridden by specifying
  command-line options or configuration files to the programs, as
  explained at the beginning of updmap.cfg.

  The options kanjiEmbed and kanjiVariant determine special replacements
  in map lines: the string \@kanjiEmbed\@ is replaced by the value of
  the respective option; similarly for kanjiVariant.  These are useful
  on KanjiMap lines.  This allows Japanese TeX users to easily select
  different fonts to be included in the final PDF output from dvipdfmx.

Explanation of trees and files normally used:

  updmap both reads and writes TEXMFCONFIG/web2c/updmap.cfg, according
  to the actions specified.

  updmap writes the map files for dvips (psfonts.map) and pdftex
  (pdftex.map) to the TEXMFVAR/fonts/map/updmap/{dvips,pdftex}/
  directories.  The map file for Kanji fonts, read by dvipdfmx, is
  written to TEXMFVAR/fonts/map/updmap/dvipdfm/kanjix.map.

  The log file is written to TEXMFVAR/web2c/updmap.log.

  When updmap-sys is run, TEXMFSYSCONFIG and TEXMFSYSVAR are used
  instead.  This is the only difference between updmap-sys and updmap.

  Other locations can be used if overridden on the command line, or these
  trees don't exist, or you are not using the original TeX Live.

  To see the precise locations of the various files that
  will be read and written, give the -n option.

For step-by-step instructions on making new fonts known to TeX, read
http://tug.org/fonts/fontinstall.html.  For even more terse
instructions, read the beginning of updmap.cfg.

Report bugs to: tex-k\@tug.org
TeX Live home page: <http://tug.org/texlive/>
EOF
;
  print &version();
  print $usage;
  exit 0;
}



###############################################################################
# processOptions()
#   process cmd line options
#
sub processOptions {
  #
  # We parse the command line twice.  The first time is to handle
  # --setoption and --enable, which might take either one or two following 
  # values, the second to handle everything else.  The Getopt::Long feature to
  # handle this is only supported in 5.8.8 (released in 2006) or later,
  # and a few people run older perls.
  # 
  my $oldconfig = Getopt::Long::Configure(qw(pass_through));
  #  
  sub read_one_or_two {
    my ($opt, $val) = @_;
    # check if = occurs in $val, if not, get the next argument
    if ($val =~ m/=/) {
      if ($opt eq "setoption") {
        push @setoptions, $val;
      } else {
        $enableItem = $val;
      }
    } else {
      my $vv = shift @ARGV;
      die "Try \"$0 --help\" for more information.\n"
        if !defined($vv);
      if ($opt eq "setoption") {
        push @setoptions, "$val=$vv";
      } else {
        $enableItem = "$val=$vv";
      }
    }
  }
  GetOptions("setoption=s@" => \&read_one_or_two,
             "enable=s"    => \&read_one_or_two) ||
    die "$0: illegal arguments, try --help for more information.\n";

  # restore old getopt config and read everything else.
  Getopt::Long::Configure($oldconfig);
  unless (&GetOptions (
      "cnffile=s" => \$cnfFile,
      "copy" => \$copy,
      "disable=s" => \@disableItem,
      "dvipdfmoutputdir=s" => \$dvipdfmoutputdir,
      "dvipsoutputdir=s" => \$dvipsoutputdir,
      # "enable=s" => \$enableItem,
      "edit" => \$opt_edit,
      "force" => \$opt_force,
      "listavailablemaps" => \$listavailablemaps,
      "l|listmaps" => \$listmaps,
      "nohash" => \$nohash,
      "nomkmap" => \$nomkmap,
      "n|dry-run" => \$dry_run,
      "outputdir=s" => \$outputdir,
      "pdftexoutputdir=s" => \$pdftexoutputdir,
      "pxdvioutputdir=s" => \$pxdvioutputdir,
      "q|quiet|silent" => \$quiet,
      "setoption" =>
        sub {die "$0: --setoption needs an option and value; try --help.\n"},
      "showoptions=s" => \@showoptions,
      "showoption=s" => \@showoption,
      "syncwithtrees" => \$syncwithtrees,
      "version" => sub { print &version(); exit(0); },
      "h|help" => \$opt_help)) {
    die "Try \"$0 --help\" for more information.\n";
  }
  
  if (@ARGV) {
    warn "$0: Ignoring unexpected non-option argument(s): @ARGV.\n";
  }

  if ($outputdir) {
    $dvipsoutputdir = $outputdir if (! $dvipsoutputdir);
    $pdftexoutputdir = $outputdir if (! $pdftexoutputdir);
    $dvipdfmoutputdir = $outputdir if (! $dvipdfmoutputdir);
    $pxdvioutputdir = $outputdir if (! $pxdvioutputdir);
  }
  if ($cnfFile && ! -f $cnfFile) {
    die "$0: Config file \"$cnfFile\" not found.\n";
  }
  if ($dvipsoutputdir && ! $dry_run && ! -d $dvipsoutputdir) {
    &mkdirhier ($dvipsoutputdir);
  }
  if ($dvipdfmoutputdir && ! $dry_run && ! -d $dvipdfmoutputdir) {
    &mkdirhier ($dvipdfmoutputdir);
  }
  if ($pdftexoutputdir && ! $dry_run && ! -d $pdftexoutputdir) {
    &mkdirhier ($pdftexoutputdir);
  }
  if ($pxdvioutputdir && ! $dry_run && ! -d $pxdvioutputdir) {
    &mkdirhier ($pxdvioutputdir);
  }
}


###############################################################################
# equalize_file(filename[, comment_char])
#   read a file and return its processed content as a string.
#   look into the source code for more details.
#
sub equalize_file {
  my $file=shift;
  my $comment=shift;
  my @temp;

  open IN, "$file";
  my @lines = (<IN>);
  close IN;
  chomp(@lines);

  for (@lines) {
    s/\s*${comment}.*// if (defined $comment); # remove comments
    next if /^\s*$/;                           # remove empty lines
    s/\s+/ /g;     # replace multiple whitespace chars by a single one
    push @temp, $_;
  }
  return join('X', sort(@temp));
}

###############################################################################
# files_are_different(file_A, file_B[, comment_char])
#   compare two equalized files.
#
sub files_are_different {
  my $file_A=shift;
  my $file_B=shift;
  my $comment=shift;
  my $retval=0;

  my $A=equalize_file("$file_A", $comment);
  my $B=equalize_file("$file_B", $comment);
  $retval=1 unless ($A eq $B);
  return $retval;
}

###############################################################################
# files_are_equal(file_A, file_B[, comment_char])
#   compare two equalized files.  Same as files_are_different() with
#   return value inverted.
#
sub files_are_equal {
  return (&files_are_different (@_))? 0:1;
}

###############################################################################
# files_are_identical(file_A, file_B)
#   compare two files.  Same as cmp(1).
#
sub files_are_identical {
  my $file_A=shift;
  my $file_B=shift;
  my $retval=0;

  open IN, "$file_A";
  my $A=(<IN>);
  close IN;
  open IN, "$file_B";
  my $B=(<IN>);
  close IN;

  $retval=1 if ($A eq $B);
  return $retval;
}

###############################################################################
# getLines()
#
###############################################################################
sub getLines {
  my @lines = ();
  foreach my $fname (@_) {
    next if (! $fname);
    if (! exists $maps{"$fname"}) {
      open FILE, "<$fname" or die "$0: can't get lines from $fname: $!";
      my @file=<FILE>;
      close FILE;
      if ($writelog) {
        print LOG ("\n$fname:\n");
        foreach my $line (@file) {
          next if $line =~ /^\s*%/; # comment
          next if $line =~ /^\s*#/; # comment
          next if $line =~ /^\s*$/; # empty line
          my $tfm;
          ($tfm)=split ' ', $line;
          print LOG "$tfm\n";
        }
      }
      $maps{"$fname"} = [ @file ] if ($cache);
      push @lines, @file;
    } else {
      push @lines, @{$maps{"$fname"}};
    }
  }
  chomp @lines;
  return @lines;
}

###############################################################################
# writeLines()
#   write the lines in $filename
#
sub writeLines {
  my ($fname, @lines) = @_;
  map { ($_ !~ m/\n$/ ? s/$/\n/ : $_ ) } @lines;
  open FILE, ">$fname" or die "$0: can't write lines to $fname: $!";
  print FILE @lines;
  close FILE;
}

###############################################################################
# copyFile()
#   copy file $src to $dst, sets $dst creation and mod time
#
sub copyFile {
  my ($src, $dst) = @_;
  my $dir;
  ($dir=$dst)=~s/(.*)\/.*/$1/;
  mkdirhier $dir;

  $src eq $dst && return "can't copy $src to itself!\n";

  open IN, "<$src" or die "$0: can't open source file $src for copying: $!";
  open OUT, ">$dst";

  binmode(IN);
  binmode(OUT);
  print OUT <IN>;
  close(OUT);
  close(IN);
  my @t = stat($src);
  utime($t[8], $t[9], $dst);
}

###############################################################################
# SymlinkOrCopy(dir, src, dest)
#   create symlinks if possible, otherwise copy files 
#
sub SymlinkOrCopy {
  my ($dir, $src, $dest) = @_;
  if (&win32 || $copy) {  # always copy
    &copyFile("$dir/$src", "$dir/$dest");
  } else { # symlink if supported by fs, copy otherwise
    system("cd \"$dir\" && ln -s $src $dest 2>/dev/null || "
           . "cp -p \"$dir/$src\" \"$dir/$dest\"");
  }
  # remember for "Files generated" in &mkMaps.
  $link{"$dest"}="$src";
}

###############################################################################
# setupSymlinks()
#   set symlink for psfonts.map according to dvipsPreferOutline variable
#
sub setupSymlinks {
  my $src;

  if ($dvipsPreferOutline) {
    $src = "psfonts_t1.map";
  } else {
    $src = "psfonts_pk.map";
  }
  unlink "$dvipsoutputdir/psfonts.map";
  &SymlinkOrCopy("$dvipsoutputdir", "$src", "psfonts.map");

  if ($pdftexDownloadBase14) {
    $src = "pdftex_dl14.map";
  } else {
    $src = "pdftex_ndl14.map";
  }
  unlink "$pdftexoutputdir/pdftex.map";
  &SymlinkOrCopy("$pdftexoutputdir", "$src", "pdftex.map");
}


###############################################################################
# transLW35(args ...)
#   transform fontname and filenames according to transformation specified
#   by mode.  Possible values:
#      URW|URWkb|ADOBE|ADOBEkb
#
sub transLW35 {
  my ($name) = @_;
  my @lines = &getLines($name);

  if ($mode eq "" || $mode eq "URWkb") {
    # do nothing
  } elsif ($mode eq "URW") {
    for my $r (@fileURW) {
      map { eval($r); } @lines;
    }
  } elsif ($mode eq "ADOBE" || $mode eq "ADOBEkb") {
    for my $r (@psADOBE) {
      map { eval($r); } @lines;
    }
    my @filemode = eval ("\@file" . $mode);
    for my $r (@filemode) {
      map { eval($r); } @lines;
    }
  }
  return @lines;
}

###############################################################################
# locateWeb2c (file ...)
#   apply kpsewhich with format 'web2c files'
#
sub locateWeb2c {
  my @files = @_;
  return @files if ($#files < 0);

  @files = split (/\n/, `kpsewhich --format="web2c files" @files`);
  if (wantarray) {
    return @files;
  }
  else {
    return $files[0];
  }
}

###############################################################################
# locateMap (file ...)
#   apply kpsewhich with format 'map'
#
sub locateMap {
  my @maps = @_;
  return @maps if ($#maps < 0);

  my @files = `kpsewhich --format=map @maps`;
  chomp @files;

  foreach my $map (@maps) {
    push @missing, $map if (! grep /\/$map(\.map)?$/, @files);
  }

  if (wantarray) {
    return @files;
  } else {
    return $files[0];
  }
}


###############################################################################
# cfgval(variable)
#   read variable ($1) from config file
#
sub cfgval {
  my ($variable) = @_;
  my $value;

  if ($#cfg < 0) {
    open FILE, "<$cnfFile" or die "$0: can't open configuration file $cnfFile: $!";
    while (<FILE>) {
      s/\s*$//; # strip trailing spaces
      push @cfg, $_;
    }
    close FILE;
    chomp (@cfg);
  }
  for my $line (@cfg) {
    if ($line =~ m/^\s*${variable}[\s=]+(.*)\s*$/) {
      $value = $1;
      if ($value =~ m/^(true|yes|t|y|1)$/) {
        $value = 1;
      }
      elsif ($value =~ m/^(false|no|f|n|0)$/) {
        $value = 0;
      }
      last;
    }
  }
  return $value;
}

###############################################################################
# configReplace(file, pattern, line)
#   The first line in file that matches pattern gets replaced by line.
#   line will be added at the end of the file if pattern does not match.
#
sub configReplace {
  my ($file, $pat, $line) = @_;
  my @lines = &getLines($file);
  my $found = 0;
  map {
    if (/$pat/) {
      $found = 1; $_ = $line;
    }
  } @lines;
  if (! $found) {
    push @lines, $line;
  }
  &writeLines($file, @lines);
}

###############################################################################
# setOption (@options)
#   parse @options for "key=value" (one element of @options)
#   or "key", "value" (two elements of @options) pairs.
#   (These were the values provided to --setoption.)
#   
sub setOptions {
  my (@options) = @_;
  for (my $i = 0; $i < @options; $i++) {
    my $o = $options[$i];
    
    my ($key,$val);
    if ($o =~ /=/) {
      ($key,$val) = split (/=/, $o, 2);
    } else {
      $key = $o;
      die "$0: no value for --setoption $key, goodbye.\n"
        if $i + 1 >= @options;
      $val = $options[$i + 1];
      $i++;
    }
    
    die "$0: unexpected empty key or val for options (@options), goodbye.\n"
      if !$key || !defined($val);
    &setOption ($key, $val);
  }
}

###############################################################################
# setOption (option, value)
#   sets option to value in the config file (replacing the existing setting
#   or by adding a new line to the config file).
#
sub setOption {
  my ($opt, $val) = @_;

  if ($opt eq "LW35") {
    if ($val !~ m/^(URWkb|URW|ADOBE|ADOBEkb)$/) {
      die "$0: Invalid value $val for option $opt; try --help.\n";
    }
  } elsif ($opt =~ 
m/^(dvipsPreferOutline|dvipsDownloadBase35|(pdftex|dvipdfm)DownloadBase14|pxdviUse)$/) {
      if ($val !~ m/^(true|false)$/) {
        die "$0: Invalid value $val for option $opt; should be \"true\" or \"false\".\n";
      }
  } elsif ($opt eq "kanjiEmbed" || $opt eq "kanjiVariant"){
    # do nothing
  } else {
    die "$0: Unsupported option $opt (value given: $val).\n";
  }

  # silently accept this old option name, just in case.
  return if $opt eq "dvipdfmDownloadBase14";
  
  #print "Setting option $opt to $val...\n" if !$quiet;
  &configReplace("$cnfFile", "^" . "$opt" . "\\s", "$opt $val");
}

###############################################################################
# showOption(item item ...)
#   show current value for an item
#
sub showOption {
  sub doit {
    my ($a, $b) = @_;
    my $v = &cfgval($a);
    $v = $b unless (defined $v);
    print "$a=$v\n";
  }
  foreach my $item (@_) {
    doit("LW35", "URWkb") if ($item eq "LW35");
    doit("dvipsPreferOutline", 1) if ($item eq "dvipsPreferOutline");
    doit("dvipsDownloadBase35", 1) if ($item eq "dvipsDownloadBase35");
    doit("pdftexDownloadBase14", 1) if ($item eq "pdftexDownloadBase14");
    doit("kanjiEmbed", "noEmbed") if ($item eq "kanjiEmbed");
    doit("kanjiVariant", "") if ($item eq "kanjiVariant");
    doit("pxdviUse", 0) if ($item eq "pxdviUse");
  }
}

###############################################################################
# showOptions(item)
#   show Options for an item
#
sub showOptions {
  foreach my $item (@_) {
    if ($item eq "LW35") {
      print "URWkb URW ADOBE ADOBEkb\n";
    }
    elsif ($item =~ 
m/(dvipsPreferOutline|(dvipdfm|pdftex)DownloadBase14|dvipsDownloadBase35|pxdviUse)/) {
      print "true false\n";
    }
    elsif ($item eq "kanjiEmbed" || $item eq "kanjiVariant") {
      print "(any string)\n";
    }
    else {
      print "Unknown item \"$item\"; should be one of LW35, dvipsPreferOutline,\n" 
          . "  dvipsDownloadBase35, pdftexDownloadBase14, pxdviUse, kanjiEmbed, or kanjiVariant\n";
    }
  }
  exit 0
}


###############################################################################
# enableMap (type, map)
#   enables an entry in the config file for map with a given type.
#
sub enableMap {
  my ($type, $map) = @_;

  if ($type !~ m/^(Map|MixedMap|KanjiMap)$/) {
    die "$0: Invalid mapType $type\n";
  }
  # a map can only have one type, so we carefully disable everything
  # about map here:
  &disableMap("$map");

  # now enable with the right type:
  &configReplace("$cnfFile", "^#!\\s*" . "$type" . "\\s*$map", "$type $map");
}

###############################################################################
# disableMap (map)
#   disables map in config file (any type)
#
sub disableMap {
  my ($map) = @_;
  my %count = ();
  my $type;

  my @mapType = grep {
    my @fields = split;
    if ($fields[0] and $fields[0] =~ /^(MixedMap|Map|KanjiMap)$/
        and $fields[1] eq $map and ++$count{$fields[0]}) {
      $_ = $fields[0];
    }
    else {
      $_ = '';
    }
  } &getLines($cnfFile);

  foreach $type (@mapType) {
    &configReplace("$cnfFile", "^$type" . "\\s*$map", "#! $type $map");
  }
}

###############################################################################
# setupDestDir()
#   find an output directory if none specified on cmd line. First choice is
#   $TEXMFVAR/fonts/map/updmap (if TEXMFVAR is set), next is relative to
#   config file location. Fallback is $TEXMFMAIN/fonts/map/updmap.
#
sub setupOutputDir {
  my($od, $driver) = @_;

  if (!$od) {
    my $rel = "fonts/map/$driver/updmap";
    my $tf;
    # Try TEXMFVAR tree. Use it if variable is set and $rel can
    # be written.
    chomp($tf = `kpsewhich --var-value=TEXMFVAR`);
    if ($tf) {
      &mkdirhier("$tf/$rel");
      if (! -w "$tf/$rel") {
        die "$0: Directory \"$tf/$rel\" isn't writable.\n";
      }
    }
    $od = "$tf/$rel";
  }
  &mkdirhier($od);
  print "$driver output dir: \"$od\"\n" if !$quiet;
  return $od;
}

sub setupDestDir {
  $dvipsoutputdir = &setupOutputDir($dvipsoutputdir, "dvips");
  $pdftexoutputdir = &setupOutputDir($pdftexoutputdir, "pdftex");
  $dvipdfmoutputdir = &setupOutputDir($dvipdfmoutputdir, "dvipdfm");
  $pxdvioutputdir = &setupOutputDir($pxdvioutputdir, "pxdvi");
}

###############################################################################
# setupCfgFile()
#   find config file if none specified on cmd line.
#
sub setupCfgFile {
  if (! $cnfFile) {
    my $tf = `kpsewhich --var-value=TEXMFCONFIG`;
    chomp($tf);
    if ($tf && ! -f "$tf/web2c/$cnfFileShort") {
      &mkdirhier("$tf/web2c") if (! -d "$tf/web2c");
      if (-d "$tf/web2c" && -w "$tf/web2c") {
        unlink "$tf/web2c/$cnfFileShort";
        my $original_cfg=`kpsewhich updmap.cfg`;
        chomp($original_cfg);
        print("copy $original_cfg => $tf/web2c/$cnfFileShort\n") if !$quiet;
        $newcnf="$tf/web2c/$cnfFileShort";
        &copyFile("$original_cfg", "$tf/web2c/$cnfFileShort");
        $updLSR->{add}("$tf/web2c/$cnfFileShort");
      }
    }
    $cnfFile = "$tf/web2c/updmap.cfg";
    if (-s $cnfFile) {
      print "Config file: \"$cnfFile\"\n" if !$quiet;
    } else {
      die "$0: Config file updmap.cfg not found.\n";
    }
  }
}

###############################################################################
# catMaps(regex)
#   filter config file by regex for map lines and extract the map filenames.
#   These are then looked up (by kpsewhich in locateMap) and the content of
#   all map files is send to stdout.
#
sub catMaps {
  my ($map) = @_;
  my %count = ( );
  my @maps = grep { $_ =~ m/$map/ } @cfg;
  map{
    $_ =~ s/\#.*//;
    # $_ =~ s/\@kanjiEmbed@/$kanjiEmbed/;
    # $_ =~ s/\@kanjiVariant@/$kanjiVariant/;
    $_ =~ s/\s*([^\s]*)\s*([^\s]*)/$2/;
  } @maps;

  my @newmaps;
  for my $m (@maps) {
    if ($m =~ m/\@kanjiEmbed@/ || $m =~ m/\@kanjiVariant@/) {
      my $newm = $m;
      $newm =~ s/\@kanjiEmbed@/$kanjiEmbed/;
      $newm =~ s/\@kanjiVariant@/$kanjiVariant/;
      my $mf = `kpsewhich --format=map $newm`;
      chomp($mf);
      if (!$mf) {
        print LOG "\n$short_progname: generated map $newm\n\tfrom $m\n\tdoes not exists, not activating it!\n";
      } else {
        push @newmaps, $newm;
      }
    } else {
      push @newmaps, $m;
    }
  }
  @maps = sort(@newmaps);
  @maps = grep { ++$count{$_} < 2; } @maps;

  @maps = &locateMap(@maps);
  return @maps;
}

###############################################################################
# listMaps()
#   list all maps mentioned in the config file
#
sub listMaps {
  my $what=shift;
  my @mapfiles;
  my @paths;
  $kanjiEmbed = &cfgval("kanjiEmbed");
  $kanjiEmbed = "noEmbed" unless (defined $kanjiEmbed);
  $kanjiVariant = &cfgval("kanjiVariant");
  $kanjiVariant = "" unless (defined $kanjiVariant);

  my @lines = grep {
    if ($what eq 'sync') {
      $_ =~ m/^(Mixed|Kanji)?Map/
    } else {
      $_ =~ m/^(\#! *)?(Mixed|Kanji)?Map/
    }
  } &getLines($cnfFile);

  if ($what eq 'list') {
    # --listmaps
    map { print "$_\n"; } @lines;
  } else {
    map { $_ =~ s/\@kanjiEmbed@/$kanjiEmbed/ } @lines;
    map { $_ =~ s/\@kanjiVariant@/$kanjiVariant/ } @lines;
    @mapfiles=grep { $_ =~ s/^(\#! *)?(Mixed|Kanji)?Map\s+// } @lines;
    @paths=&locateMap(@mapfiles);

    if ($what eq 'avail') {
      # --listavailablemaps
      map {
        my $entry="$_"; 
        # that was disabled, but I don't understand why (NP)
        print "$entry\n" if (grep { $_ =~ m/\/$entry/ } @paths);
      } @lines;
    } elsif ($what eq 'sync') {
      # --syncwithtrees
      map { 
        my $entry="$_"; 
        unless (grep { $_ =~ m/\/$entry/ } @paths) {
          &disableMap($entry);
          print "  $entry disabled\n" if !$quiet; 
        }
      } @lines;
    }
  }
}

###############################################################################
# normalizeLines()
#   remove comments, whitespace is exactly one space, no empty lines,
#   no whitespace at end of line, one space before and after "
#
sub normalizeLines {
  my @lines = @_;
  my %count = ();

  @lines = grep { $_ !~ m/^[*#;%]/ } @lines;
  map {$_ =~ s/\s+/ /gx } @lines;
  @lines = grep { $_ !~ m/^\s*$/x } @lines;
  map { $_ =~ s/\s$//x ;
        $_ =~ s/\s*\"\s*/ \" /gx;
        $_ =~ s/\" ([^\"]*) \"/\"$1\"/gx;
      } @lines;

  @lines = grep {++$count{$_} < 2 } (sort @lines);

  return @lines;
}

###############################################################################
# to_pdftex()
#   if $pdftexStripEnc is set, strip "PS_Encoding_Name ReEncodeFont"
#   from map entries; they are ignored by pdftex.  But since the sh
#   incarnation of updmap included them, and we want to minimize
#   differences, this is not done by default.
#
sub to_pdftex {
  return @_ unless $pdftexStripEnc;
  my @in = @_;
  my @out;
  foreach my $line (@in) {
    if ($line =~ /^(.*\s+)(\S+\s+ReEncodeFont\s)(.*)/) {
            $line = "$1$3";
            $line =~ s/\s+\"\s+\"\s+/ /;
    }
    push @out, $line;
  }
  return @out;
}

###############################################################################
# cidx2dvips()
#   reads from stdin, writes to stdout. It transforms "cid-x"-like syntax into
#   "dvips"-like syntax.
###############################################################################
sub cidx2dvips {
    my ($s) = @_;
    my @d;
    foreach (@$s) {
      s/,BoldItalic/ -s .3/;
      s/,Bold//;
      s/,Italic/ -s .3/;
      s/\s\s*/ /g;
      if ($_ =~ /.*[@\:\/,]/) {next;}
      elsif ($_ =~ /^[^ ][^ ]* unicode /) {next;}
      s/^([^ ][^ ]* [^ ][^ ]* [^ ][^ ]*)\.[Oo][Tt][Ff]/$1/;
      s/^([^ ][^ ]* [^ ][^ ]* [^ ][^ ]*)\.[Tt][Tt][FfCc]/$1/; 
      s/$/ %/;
      s/^(([^ ]*).*)/$1$2/;
      s/^([^ ][^ ]* ([^ ][^ ]*) !*([^ ][^ ]*).*)/$1 $3-$2/;
      s/^(.* -e ([.0-9-][.0-9-]*).*)/$1 "$2 ExtendFont"/;
      s/^(.* -s ([.0-9-][.0-9-]*).*)/$1 "$2 SlantFont"/;
      s/.*%//;
      push(@d, $_);
    }
    return @d
}


###############################################################################
# mkMaps()
#   the main task of this script: create the output files
#
sub mkMaps {
  my @lines;
  my $logfile;

  $cache=1;

  if (! $dry_run) {
    my $TEXMFVAR = `kpsewhich --var-value=TEXMFVAR`;
    chomp($TEXMFVAR);
    $logfile = "$TEXMFVAR/web2c/updmap.log";
    mkdirhier "$TEXMFVAR/web2c";
    open LOG, ">$logfile" 
        or die "$0: Can't open log file \"$logfile\": $!";
    $writelog=1;
    print LOG &version();
    printf LOG "%s\n\n", scalar localtime();
    print LOG  "Using config file \"$cnfFile\".\n";
  }
  sub wlog () {
    my $str=shift;
    if ($dry_run) {
      print $str;
    } else {
      print $str if !$quiet;
      print LOG $str;
    }
  }
  sub newline() {
    if ($dry_run) {
      print "\n";
    } else {
      print LOG "\n";
    }
  }

  $mode = &cfgval("LW35");
  $mode = "URWkb" unless (defined $mode);

  $dvipsPreferOutline = &cfgval("dvipsPreferOutline");
  $dvipsPreferOutline = 1 unless (defined $dvipsPreferOutline);

  $dvipsDownloadBase35 = &cfgval("dvipsDownloadBase35");
  $dvipsDownloadBase35 = 1 unless (defined $dvipsDownloadBase35);

  $pdftexDownloadBase14 = &cfgval("pdftexDownloadBase14");
  $pdftexDownloadBase14 = 1 unless (defined $pdftexDownloadBase14);

  $kanjiEmbed = &cfgval("kanjiEmbed");
  $kanjiEmbed = "noEmbed" unless (defined $kanjiEmbed);

  $kanjiVariant = &cfgval("kanjiVariant");
  $kanjiVariant = "" unless (defined $kanjiVariant);

  $pxdviUse = &cfgval("pxdviUse");
  $pxdviUse = 0 unless (defined $pxdviUse);

  &wlog ("\n$0 "
         . ($dry_run ? "would create" : "is creating") . " new map files"
         . "\nusing the following configuration:"
         . "\n  LW35 font names                  : "
         .      $mode
         . "\n  prefer outlines                  : "
         .      ($dvipsPreferOutline ? "true" : "false")
         . "\n  texhash enabled                  : "
         .      ($nohash ? "false" : "true")
         . "\n  download standard fonts (dvips)  : "
         .      ($dvipsDownloadBase35 ? "true" : "false")
         . "\n  download standard fonts (pdftex) : "
         .      ($pdftexDownloadBase14 ? "true" : "false")
         . "\n  kanjiEmbed/variant replacement   : "
         .      "$kanjiEmbed/$kanjiVariant"
         . "\n  create a mapfile for pxdvi       : "
         .      ($pxdviUse ? "true" : "false")
         . "\n\n");

  &wlog ("Scanning for LW35 support files");
  $dvips35 = &locateMap("dvips35.map");
  $pdftex35 = &locateMap("pdftex35.map");
  $ps2pk35 = &locateMap("ps2pk35.map");
  my $LW35 = "\n$dvips35\n$pdftex35\n$ps2pk35\n\n";
  if ($dry_run) {
    print $LW35;
  } else {
    print LOG $LW35;
  }
  printf "  [%3d files]\n", 3 unless ($quiet || $dry_run);

  &wlog ("Scanning for MixedMap entries");
  &newline;
  my @tmp1 = &catMaps('^MixedMap');
  foreach my $line (@tmp1) {
    if ($dry_run) {
      print "$line\n";
    } else {
      print LOG "$line\n";
    }
  }
  &newline;
  printf "    [%3d files]\n", scalar @tmp1 
      unless ($quiet || $dry_run);

  &wlog ("Scanning for KanjiMap entries");
  &newline;
  my @tmpkanji0 = &catMaps('^KanjiMap');
  foreach my $line (@tmpkanji0) {
    if ($dry_run) {
      print "$line\n";
    } else {
      print LOG "$line\n";
    }
  }
  &newline;
  printf "    [%3d files]\n", scalar @tmpkanji0
      unless ($quiet || $dry_run);

  &wlog ("Scanning for Map entries");
  &newline();
  my @tmp2 = &catMaps('^Map');
  foreach my $line (@tmp2) {
    if ($dry_run) {
      print "$line\n";
    } else {
      print LOG "$line\n";
    }
  }
  &newline;
  printf "         [%3d files]\n\n", scalar @tmp2 
      unless ($quiet || $dry_run);

  if (@missing > 0) {
    print STDERR "\nERROR:  The following map file(s) couldn't be found:\n\t";
    print STDERR join(' ', @missing);
    print STDERR "\n\n\tDid you run mktexlsr?\n\n" .
        "\tYou can delete non-existent map entries using the option\n".
        "\t  --syncwithtrees.\n\n";
    exit (1);
  }
  exit(0) if $dry_run;

  # Create psfonts_t1.map, psfonts_pk.map, ps2pk.map and pdftex.map:
  my @managed_files =  ("$dvipsoutputdir/download35.map",
                "$dvipsoutputdir/builtin35.map",
                "$dvipsoutputdir/psfonts_t1.map",
                "$dvipsoutputdir/psfonts_pk.map",
                "$pdftexoutputdir/pdftex_dl14.map",
                "$pdftexoutputdir/pdftex_ndl14.map",
                "$dvipdfmoutputdir/kanjix.map",
                "$dvipsoutputdir/ps2pk.map");
  if ($pxdviUse) {
    push @managed_files, "$pxdvioutputdir/xdvi-ptex.map";
  }
  for my $file (@managed_files) {
    open FILE, ">$file";
    print FILE "% $file:\
%   maintained by updmap[-sys].\
%   Don't change this file directly. Use updmap[-sys] instead.\
%   See texmf/web2c/$cnfFileShort and the updmap documentation.\
% A log of the run that created this file is available here:\
%   $logfile\
";
    close FILE;
  }

  print "Generating output for dvipdfm...\n" if !$quiet;
  my @tmpkanji1;
  push @tmpkanji1, &getLines(@tmpkanji0);
  @tmpkanji1 = &normalizeLines(@tmpkanji1);
  &writeLines(">$dvipdfmoutputdir/kanjix.map", 
              @tmpkanji1);

  if ($pxdviUse) {
    # we use the very same data as for kanjix.map, but generate
    # a different file, so that in case a user wants to hand-craft it
    print "Generating output for pxdvi...\n" if !$quiet;
    &writeLines(">$pxdvioutputdir/xdvi-ptex.map", 
                @tmpkanji1);
  }

  print "Generating output for ps2pk...\n" if !$quiet;
  my @ps2pk_map = &transLW35($ps2pk35);
  push @ps2pk_map, &getLines(@tmp1);
  push @ps2pk_map, &getLines(@tmp2);
  &writeLines(">$dvipsoutputdir/ps2pk.map", 
              &normalizeLines(@ps2pk_map));

  print "Generating output for dvips...\n" if !$quiet;
  my @download35_map = &transLW35($ps2pk35);
  &writeLines(">$dvipsoutputdir/download35.map", 
              &normalizeLines(@download35_map));

  my @builtin35_map = &transLW35($dvips35);
  &writeLines(">$dvipsoutputdir/builtin35.map", 
              &normalizeLines(@builtin35_map));

  my $dftdvips = ($dvipsDownloadBase35 ? $ps2pk35 : $dvips35);

  my @psfonts_t1_map = &transLW35($dftdvips);
  my @tmpkanji2 = &cidx2dvips(\@tmpkanji1);
  push @psfonts_t1_map, &getLines(@tmp1);
  push @psfonts_t1_map, &getLines(@tmp2);
  push @psfonts_t1_map, @tmpkanji2;
  &writeLines(">$dvipsoutputdir/psfonts_t1.map", 
              &normalizeLines(@psfonts_t1_map));

  my @psfonts_pk_map = &transLW35($dftdvips);
  push @psfonts_pk_map, &getLines(@tmp2);
  push @psfonts_pk_map, @tmpkanji2;
  &writeLines(">$dvipsoutputdir/psfonts_pk.map", 
              &normalizeLines(@psfonts_pk_map));

  print "Generating output for pdftex...\n" if !$quiet;
  # remove PaintType due to Sebastian's request
  my @tmp3 = &transLW35($pdftex35);
  push @tmp3, &getLines(@tmp1);
  push @tmp3, &getLines(@tmp2);
  @tmp3 = grep { $_ !~ m/(^%|PaintType)/ } @tmp3;

  my @tmp7 = &transLW35($ps2pk35);
  push @tmp7, &getLines(@tmp1);
  push @tmp7, &getLines(@tmp2);
  @tmp7 = grep { $_ !~ m/(^%|PaintType)/ } @tmp7;

  my @pdftex_ndl14_map = @tmp3;
  @pdftex_ndl14_map = &normalizeLines(@pdftex_ndl14_map);
  @pdftex_ndl14_map = &to_pdftex(@pdftex_ndl14_map);
  &writeLines(">$pdftexoutputdir/pdftex_ndl14.map", @pdftex_ndl14_map);

  my @pdftex_dl14_map = @tmp7;
  @pdftex_dl14_map = &normalizeLines(@pdftex_dl14_map);
  @pdftex_dl14_map = &to_pdftex(@pdftex_dl14_map);
  &writeLines(">$pdftexoutputdir/pdftex_dl14.map", @pdftex_dl14_map);

  &setupSymlinks;

  &wlog ("\nFiles generated:\n");
  sub dir {
    my ($d, $f, $target)=@_;
    if (-e "$d/$f") {
      my @stat=lstat("$d/$f");
      my ($s,$m,$h,$D,$M,$Y)=localtime($stat[9]);
      my $timestamp=sprintf ("%04d-%02d-%02d %02d:%02d:%02d", 
                             $Y+1900, $M+1, $D, $h, $m, $s);
      my $date=sprintf "%12d %s %s", $stat[7], $timestamp, $f;
      &wlog ($date);
      
      if (-l "$d/$f") {
        my $lnk=sprintf " -> %s\n", readlink ("$d/$f");
        &wlog ($lnk);
      } elsif ($f eq $target) {
        if (&files_are_identical("$d/$f", "$d/$link{$target}")) {
          &wlog (" = $link{$target}\n");
        } else {
          &wlog (" = ?????\n"); # This shouldn't happen.
        }
      } else {
        &wlog ("\n");
      } 
    } else {
      print STDERR "Warning: File $d/$f doesn't exist.\n";
      print LOG    "Warning: File $d/$f doesn't exist.\n";
    }
  }
  my $d;
  $d="$dvipsoutputdir"; &wlog ("  $d:\n");
  foreach my $f ('builtin35.map', 'download35.map', 'psfonts_pk.map', 
                 'psfonts_t1.map', 'ps2pk.map', 'psfonts.map') {
    dir ($d, $f, 'psfonts.map');
    $updLSR->{add}("$d/$f");
  }
  $d="$pdftexoutputdir"; &wlog ("  $d:\n");
  foreach my $f ('pdftex_dl14.map', 'pdftex_ndl14.map', 'pdftex.map') {
    dir ($d, $f, 'pdftex.map');
    $updLSR->{add}("$d/$f");
  }
  $d="$dvipdfmoutputdir"; &wlog ("  $d:\n");
  foreach my $f ('kanjix.map') {
    dir ($d, $f, '');
    $updLSR->{add}("$d/$f");
  }
  if ($pxdviUse) {
    $d="$pxdvioutputdir"; &wlog ("  $d:\n");
    foreach my $f ('xdvi-ptex.map') {
      dir ($d, $f, '');
      $updLSR->{add}("$d/$f");
    }
  }
  close LOG;
  print "\nTranscript written on \"$logfile\".\n" if !$quiet;
}


###############################################################################
# initVars()
#   initialize global variables
#
sub initVars {
  $quiet = 0;
  $nohash = 0;
  $nomkmap = 0;
  $cnfFile = "";
  $cnfFileShort = "updmap.cfg";
  $outputdir = "";
  chomp($TEXMFMAIN =`kpsewhich --var-value=TEXMFMAIN`);

  @psADOBE = (
       's/ URWGothicL-Demi / AvantGarde-Demi /',
       's/ URWGothicL-DemiObli / AvantGarde-DemiOblique /',
       's/ URWGothicL-Book / AvantGarde-Book /',
       's/ URWGothicL-BookObli / AvantGarde-BookOblique /',
       's/ URWBookmanL-DemiBold / Bookman-Demi /',
       's/ URWBookmanL-DemiBoldItal / Bookman-DemiItalic /',
       's/ URWBookmanL-Ligh / Bookman-Light /',
       's/ URWBookmanL-LighItal / Bookman-LightItalic /',
       's/ NimbusMonL-Bold / Courier-Bold /',
       's/ NimbusMonL-BoldObli / Courier-BoldOblique /',
       's/ NimbusMonL-Regu / Courier /',
       's/ NimbusMonL-ReguObli / Courier-Oblique /',
       's/ NimbusSanL-Bold / Helvetica-Bold /',
       's/ NimbusSanL-BoldCond / Helvetica-Narrow-Bold /',
       's/ NimbusSanL-BoldItal / Helvetica-BoldOblique /',
       's/ NimbusSanL-BoldCondItal / Helvetica-Narrow-BoldOblique /',
       's/ NimbusSanL-Regu / Helvetica /',
       's/ NimbusSanL-ReguCond / Helvetica-Narrow /',
       's/ NimbusSanL-ReguItal / Helvetica-Oblique /',
       's/ NimbusSanL-ReguCondItal / Helvetica-Narrow-Oblique /',
       's/ CenturySchL-Bold / NewCenturySchlbk-Bold /',
       's/ CenturySchL-BoldItal / NewCenturySchlbk-BoldItalic /',
       's/ CenturySchL-Roma / NewCenturySchlbk-Roman /',
       's/ CenturySchL-Ital / NewCenturySchlbk-Italic /',
       's/ URWPalladioL-Bold / Palatino-Bold /',
       's/ URWPalladioL-BoldItal / Palatino-BoldItalic /',
       's/ URWPalladioL-Roma / Palatino-Roman /',
       's/ URWPalladioL-Ital / Palatino-Italic /',
       's/ StandardSymL / Symbol /',
       's/ NimbusRomNo9L-Medi / Times-Bold /',
       's/ NimbusRomNo9L-MediItal / Times-BoldItalic /',
       's/ NimbusRomNo9L-Regu / Times-Roman /',
       's/ NimbusRomNo9L-ReguItal / Times-Italic /',
       's/ URWChanceryL-MediItal / ZapfChancery-MediumItalic /',
       's/ Dingbats / ZapfDingbats /',
                );

  @fileADOBEkb = (
        's/\buagd8a.pfb\b/pagd8a.pfb/',
        's/\buagdo8a.pfb\b/pagdo8a.pfb/',
        's/\buagk8a.pfb\b/pagk8a.pfb/',
        's/\buagko8a.pfb\b/pagko8a.pfb/',
        's/\bubkd8a.pfb\b/pbkd8a.pfb/',
        's/\bubkdi8a.pfb\b/pbkdi8a.pfb/',
        's/\bubkl8a.pfb\b/pbkl8a.pfb/',
        's/\bubkli8a.pfb\b/pbkli8a.pfb/',
        's/\bucrb8a.pfb\b/pcrb8a.pfb/',
        's/\bucrbo8a.pfb\b/pcrbo8a.pfb/',
        's/\bucrr8a.pfb\b/pcrr8a.pfb/',
        's/\bucrro8a.pfb\b/pcrro8a.pfb/',
        's/\buhvb8a.pfb\b/phvb8a.pfb/',
        's/\buhvb8ac.pfb\b/phvb8an.pfb/',
        's/\buhvbo8a.pfb\b/phvbo8a.pfb/',
        's/\buhvbo8ac.pfb\b/phvbo8an.pfb/',
        's/\buhvr8a.pfb\b/phvr8a.pfb/',
        's/\buhvr8ac.pfb\b/phvr8an.pfb/',
        's/\buhvro8a.pfb\b/phvro8a.pfb/',
        's/\buhvro8ac.pfb\b/phvro8an.pfb/',
        's/\buncb8a.pfb\b/pncb8a.pfb/',
        's/\buncbi8a.pfb\b/pncbi8a.pfb/',
        's/\buncr8a.pfb\b/pncr8a.pfb/',
        's/\buncri8a.pfb\b/pncri8a.pfb/',
        's/\buplb8a.pfb\b/pplb8a.pfb/',
        's/\buplbi8a.pfb\b/pplbi8a.pfb/',
        's/\buplr8a.pfb\b/pplr8a.pfb/',
        's/\buplri8a.pfb\b/pplri8a.pfb/',
        's/\busyr.pfb\b/psyr.pfb/',
        's/\butmb8a.pfb\b/ptmb8a.pfb/',
        's/\butmbi8a.pfb\b/ptmbi8a.pfb/',
        's/\butmr8a.pfb\b/ptmr8a.pfb/',
        's/\butmri8a.pfb\b/ptmri8a.pfb/',
        's/\buzcmi8a.pfb\b/pzcmi8a.pfb/',
        's/\buzdr.pfb\b/pzdr.pfb/',
                  );

  @fileURW = (
        's/\buagd8a.pfb\b/a010015l.pfb/',
        's/\buagdo8a.pfb\b/a010035l.pfb/',
        's/\buagk8a.pfb\b/a010013l.pfb/',
        's/\buagko8a.pfb\b/a010033l.pfb/',
        's/\bubkd8a.pfb\b/b018015l.pfb/',
        's/\bubkdi8a.pfb\b/b018035l.pfb/',
        's/\bubkl8a.pfb\b/b018012l.pfb/',
        's/\bubkli8a.pfb\b/b018032l.pfb/',
        's/\bucrb8a.pfb\b/n022004l.pfb/',
        's/\bucrbo8a.pfb\b/n022024l.pfb/',
        's/\bucrr8a.pfb\b/n022003l.pfb/',
        's/\bucrro8a.pfb\b/n022023l.pfb/',
        's/\buhvb8a.pfb\b/n019004l.pfb/',
        's/\buhvb8ac.pfb\b/n019044l.pfb/',
        's/\buhvbo8a.pfb\b/n019024l.pfb/',
        's/\buhvbo8ac.pfb\b/n019064l.pfb/',
        's/\buhvr8a.pfb\b/n019003l.pfb/',
        's/\buhvr8ac.pfb\b/n019043l.pfb/',
        's/\buhvro8a.pfb\b/n019023l.pfb/',
        's/\buhvro8ac.pfb\b/n019063l.pfb/',
        's/\buncb8a.pfb\b/c059016l.pfb/',
        's/\buncbi8a.pfb\b/c059036l.pfb/',
        's/\buncr8a.pfb\b/c059013l.pfb/',
        's/\buncri8a.pfb\b/c059033l.pfb/',
        's/\buplb8a.pfb\b/p052004l.pfb/',
        's/\buplbi8a.pfb\b/p052024l.pfb/',
        's/\buplr8a.pfb\b/p052003l.pfb/',
        's/\buplri8a.pfb\b/p052023l.pfb/',
        's/\busyr.pfb\b/s050000l.pfb/',
        's/\butmb8a.pfb\b/n021004l.pfb/',
        's/\butmbi8a.pfb\b/n021024l.pfb/',
        's/\butmr8a.pfb\b/n021003l.pfb/',
        's/\butmri8a.pfb\b/n021023l.pfb/',
        's/\buzcmi8a.pfb\b/z003034l.pfb/',
        's/\buzdr.pfb\b/d050000l.pfb/',
                   );

  @fileADOBE = (
        's/\buagd8a.pfb\b/agd_____.pfb/',
        's/\buagdo8a.pfb\b/agdo____.pfb/',
        's/\buagk8a.pfb\b/agw_____.pfb/',
        's/\buagko8a.pfb\b/agwo____.pfb/',
        's/\bubkd8a.pfb\b/bkd_____.pfb/',
        's/\bubkdi8a.pfb\b/bkdi____.pfb/',
        's/\bubkl8a.pfb\b/bkl_____.pfb/',
        's/\bubkli8a.pfb\b/bkli____.pfb/',
        's/\bucrb8a.pfb\b/cob_____.pfb/',
        's/\bucrbo8a.pfb\b/cobo____.pfb/',
        's/\bucrr8a.pfb\b/com_____.pfb/',
        's/\bucrro8a.pfb\b/coo_____.pfb/',
        's/\buhvb8a.pfb\b/hvb_____.pfb/',
        's/\buhvb8ac.pfb\b/hvnb____.pfb/',
        's/\buhvbo8a.pfb\b/hvbo____.pfb/',
        's/\buhvbo8ac.pfb\b/hvnbo___.pfb/',
        's/\buhvr8a.pfb\b/hv______.pfb/',
        's/\buhvr8ac.pfb\b/hvn_____.pfb/',
        's/\buhvro8a.pfb\b/hvo_____.pfb/',
        's/\buhvro8ac.pfb\b/hvno____.pfb/',
        's/\buncb8a.pfb\b/ncb_____.pfb/',
        's/\buncbi8a.pfb\b/ncbi____.pfb/',
        's/\buncr8a.pfb\b/ncr_____.pfb/',
        's/\buncri8a.pfb\b/nci_____.pfb/',
        's/\buplb8a.pfb\b/pob_____.pfb/',
        's/\buplbi8a.pfb\b/pobi____.pfb/',
        's/\buplr8a.pfb\b/por_____.pfb/',
        's/\buplri8a.pfb\b/poi_____.pfb/',
        's/\busyr.pfb\b/sy______.pfb/',
        's/\butmb8a.pfb\b/tib_____.pfb/',
        's/\butmbi8a.pfb\b/tibi____.pfb/',
        's/\butmr8a.pfb\b/tir_____.pfb/',
        's/\butmri8a.pfb\b/tii_____.pfb/',
        's/\buzcmi8a.pfb\b/zcmi____.pfb/',
        's/\buzdr.pfb\b/zd______.pfb/',
                );
}


###############################################################################
# main()
#
sub main {
  &initVars;
  &processOptions;

  &help if ($opt_help);

  if (@showoptions) {
    &showOptions(@showoptions);
    exit 0;
  }

  &setupCfgFile;

  if (@showoption) {
    &showOption(@showoption);
    exit 0;
  }
  if ($listmaps) {
    &listMaps ('list');
    exit 0;
  }
  if ($listavailablemaps) {
    &listMaps ('avail');
    exit 0;
  }
  if ($syncwithtrees) {
    &listMaps ('sync');
    exit 0;
  }

  my $bakFile = $cnfFile;
  $bakFile =~ s/\.cfg$/.bak/;
  &copyFile($cnfFile, $bakFile);

  my $cmd = '';

  if ($opt_edit) {
    # it's not a good idea to edit updmap.cfg manually these days,
    # but for compatibility we'll silently keep the option.
    $cmd = 'edit';
    my $editor = $ENV{'VISUAL'} || $ENV{'EDITOR'};
    $editor ||= (&win32 ? "notepad" : "vi");
    system($editor, $cnfFile);

  } elsif (@setoptions) {
    $cmd = 'setOption';
    &setOptions (@setoptions);

  } elsif ($enableItem) {
    $cmd = 'enableMap';
    if ($enableItem =~ /=/) {
      &enableMap(split('=', $enableItem));
    } else {
      &enableMap($enableItem, shift @ARGV);
    }

  } elsif (@disableItem) {
    $cmd = 'disableMap';
    foreach my $m (@disableItem) {
      &disableMap($m);
    }
  }

  if ($cmd && !$opt_force && &files_are_equal($bakFile, $cnfFile)) {
    print "$cnfFile unchanged.  Map files not recreated.\n" if !$quiet;
  } else {
    if (! $nomkmap) {
      &setupDestDir;
      &mkMaps;
    }
    unlink ($bakFile);
  }

  unless ($nohash) {
    print "$0: Updating ls-R files.\n" if !$quiet;
    $updLSR->{exec}() 
  }
}
__END__

### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim:set tabstop=2 expandtab: #
