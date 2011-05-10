#! /usr/bin/env perl
###############################################################################
# updmap: utility to maintain map files for outline fonts.
#
# Thomas Esser, (C) 2002.
# Fabrice Popineau, for the Perl version.
# Anyone may freely use, modify, and/or distribute this file, without
# limitation.
###############################################################################
# $Id: updmap.pl 21946 2011-04-04 00:16:58Z karl $
BEGIN {
  $^W=1;
  chomp($TEXMFROOT = `kpsewhich -var-value=TEXMFROOT`);
  unshift (@INC, "$TEXMFROOT/tlpkg");
}

my $version = '$Id: updmap.pl 21946 2011-04-04 00:16:58Z karl $';

use strict;
use TeXLive::TLUtils qw(mkdirhier mktexupd win32);
use Getopt::Long;
$Getopt::Long::autoabbrev=0;
Getopt::Long::Configure ("bundling");

my $progname='updmap';
my $cnfFile;
my $cnfFileShort;
my $outputdir;
my $dvipsoutputdir;
my $pdftexoutputdir;
my $quiet;
my $nohash;
my $nomkmap;

my $enableItem;
my %setOption = ();
my @showoptions = ();
my @disableItem = ();
my $listmaps;
my $listavailablemaps;
my $syncwithtrees;

my $verbose;
my $opt_edit;
my $opt_help;
my $dry_run;
my $TEXMFMAIN;
my $TEXMFVAR;

my $mode;
my $dvipsPreferOutline;
my $dvipsDownloadBase35;
my $pdftexDownloadBase14;

my $dvips35;
my $pdftex35;
my $ps2pk35;

my $newcnf;
my %link;
my %maps;
my @missing;
my $writelog = 0;
my $cache = 0; # don't change!
my $copy;
my $pdftexNoEnc=0;

# initialize mktexupd
my $updLSR=&mktexupd();
$updLSR->{mustexist}(0);

my @cfg = ();

my @psADOBE = (
	       's/ URWGothicL-Demi / AvantGarde-Demi /x',
	       's/ URWGothicL-DemiObli / AvantGarde-DemiOblique /x',
	       's/ URWGothicL-Book / AvantGarde-Book /x',
	       's/ URWGothicL-BookObli / AvantGarde-BookOblique /x',
	       's/ URWBookmanL-DemiBold / Bookman-Demi /x',
	       's/ URWBookmanL-DemiBoldItal / Bookman-DemiItalic /x',
	       's/ URWBookmanL-Ligh / Bookman-Light /x',
	       's/ URWBookmanL-LighItal / Bookman-LightItalic /x',
	       's/ NimbusMonL-Bold / Courier-Bold /x',
	       's/ NimbusMonL-BoldObli / Courier-BoldOblique /x',
	       's/ NimbusMonL-Regu / Courier /x',
	       's/ NimbusMonL-ReguObli / Courier-Oblique /x',
	       's/ NimbusSanL-Bold / Helvetica-Bold /x',
	       's/ NimbusSanL-BoldCond / Helvetica-Narrow-Bold /x',
	       's/ NimbusSanL-BoldItal / Helvetica-BoldOblique /x',
	       's/ NimbusSanL-BoldCondItal / Helvetica-Narrow-BoldOblique /x',
	       's/ NimbusSanL-Regu / Helvetica /x',
	       's/ NimbusSanL-ReguCond / Helvetica-Narrow /x',
	       's/ NimbusSanL-ReguItal / Helvetica-Oblique /x',
	       's/ NimbusSanL-ReguCondItal / Helvetica-Narrow-Oblique /x',
	       's/ CenturySchL-Bold / NewCenturySchlbk-Bold /x',
	       's/ CenturySchL-BoldItal / NewCenturySchlbk-BoldItalic /x',
	       's/ CenturySchL-Roma / NewCenturySchlbk-Roman /x',
	       's/ CenturySchL-Ital / NewCenturySchlbk-Italic /x',
	       's/ URWPalladioL-Bold / Palatino-Bold /x',
	       's/ URWPalladioL-BoldItal / Palatino-BoldItalic /x',
	       's/ URWPalladioL-Roma / Palatino-Roman /x',
	       's/ URWPalladioL-Ital / Palatino-Italic /x',
	       's/ StandardSymL / Symbol /x',
	       's/ NimbusRomNo9L-Medi / Times-Bold /x',
	       's/ NimbusRomNo9L-MediItal / Times-BoldItalic /x',
	       's/ NimbusRomNo9L-Regu / Times-Roman /x',
	       's/ NimbusRomNo9L-ReguItal / Times-Italic /x',
	       's/ URWChanceryL-MediItal / ZapfChancery-MediumItalic /x',
	       's/ Dingbats / ZapfDingbats /x'
	      );

my @fileADOBEkb = (
		   's/\buagd8a.pfb\b/pagd8a.pfb/x',
		   's/\buagdo8a.pfb\b/pagdo8a.pfb/x',
		   's/\buagk8a.pfb\b/pagk8a.pfb/x',
		   's/\buagko8a.pfb\b/pagko8a.pfb/x',
		   's/\bubkd8a.pfb\b/pbkd8a.pfb/x',
		   's/\bubkdi8a.pfb\b/pbkdi8a.pfb/x',
		   's/\bubkl8a.pfb\b/pbkl8a.pfb/x',
		   's/\bubkli8a.pfb\b/pbkli8a.pfb/x',
		   's/\bucrb8a.pfb\b/pcrb8a.pfb/x',
		   's/\bucrbo8a.pfb\b/pcrbo8a.pfb/x',
		   's/\bucrr8a.pfb\b/pcrr8a.pfb/x',
		   's/\bucrro8a.pfb\b/pcrro8a.pfb/x',
		   's/\buhvb8a.pfb\b/phvb8a.pfb/x',
		   's/\buhvb8ac.pfb\b/phvb8an.pfb/x',
		   's/\buhvbo8a.pfb\b/phvbo8a.pfb/x',
		   's/\buhvbo8ac.pfb\b/phvbo8an.pfb/x',
		   's/\buhvr8a.pfb\b/phvr8a.pfb/x',
		   's/\buhvr8ac.pfb\b/phvr8an.pfb/x',
		   's/\buhvro8a.pfb\b/phvro8a.pfb/x',
		   's/\buhvro8ac.pfb\b/phvro8an.pfb/x',
		   's/\buncb8a.pfb\b/pncb8a.pfb/x',
		   's/\buncbi8a.pfb\b/pncbi8a.pfb/x',
		   's/\buncr8a.pfb\b/pncr8a.pfb/x',
		   's/\buncri8a.pfb\b/pncri8a.pfb/x',
		   's/\buplb8a.pfb\b/pplb8a.pfb/x',
		   's/\buplbi8a.pfb\b/pplbi8a.pfb/x',
		   's/\buplr8a.pfb\b/pplr8a.pfb/x',
		   's/\buplri8a.pfb\b/pplri8a.pfb/x',
		   's/\busyr.pfb\b/psyr.pfb/x',
		   's/\butmb8a.pfb\b/ptmb8a.pfb/x',
		   's/\butmbi8a.pfb\b/ptmbi8a.pfb/x',
		   's/\butmr8a.pfb\b/ptmr8a.pfb/x',
		   's/\butmri8a.pfb\b/ptmri8a.pfb/x',
		   's/\buzcmi8a.pfb\b/pzcmi8a.pfb/x',
		   's/\buzdr.pfb\b/pzdr.pfb/x'
		  );

my @fileURW = (
	's/\buagd8a.pfb\b/a010015l.pfb/x',
	's/\buagdo8a.pfb\b/a010035l.pfb/x',
	's/\buagk8a.pfb\b/a010013l.pfb/x',
	's/\buagko8a.pfb\b/a010033l.pfb/x',
	's/\bubkd8a.pfb\b/b018015l.pfb/x',
	's/\bubkdi8a.pfb\b/b018035l.pfb/x',
	's/\bubkl8a.pfb\b/b018012l.pfb/x',
	's/\bubkli8a.pfb\b/b018032l.pfb/x',
	's/\bucrb8a.pfb\b/n022004l.pfb/x',
	's/\bucrbo8a.pfb\b/n022024l.pfb/x',
	's/\bucrr8a.pfb\b/n022003l.pfb/x',
	's/\bucrro8a.pfb\b/n022023l.pfb/x',
	's/\buhvb8a.pfb\b/n019004l.pfb/x',
	's/\buhvb8ac.pfb\b/n019044l.pfb/x',
	's/\buhvbo8a.pfb\b/n019024l.pfb/x',
	's/\buhvbo8ac.pfb\b/n019064l.pfb/x',
	's/\buhvr8a.pfb\b/n019003l.pfb/x',
	's/\buhvr8ac.pfb\b/n019043l.pfb/x',
	's/\buhvro8a.pfb\b/n019023l.pfb/x',
	's/\buhvro8ac.pfb\b/n019063l.pfb/x',
	's/\buncb8a.pfb\b/c059016l.pfb/x',
	's/\buncbi8a.pfb\b/c059036l.pfb/x',
	's/\buncr8a.pfb\b/c059013l.pfb/x',
	's/\buncri8a.pfb\b/c059033l.pfb/x',
	's/\buplb8a.pfb\b/p052004l.pfb/x',
	's/\buplbi8a.pfb\b/p052024l.pfb/x',
	's/\buplr8a.pfb\b/p052003l.pfb/x',
	's/\buplri8a.pfb\b/p052023l.pfb/x',
	's/\busyr.pfb\b/s050000l.pfb/x',
	's/\butmb8a.pfb\b/n021004l.pfb/x',
	's/\butmbi8a.pfb\b/n021024l.pfb/x',
	's/\butmr8a.pfb\b/n021003l.pfb/x',
	's/\butmri8a.pfb\b/n021023l.pfb/x',
	's/\buzcmi8a.pfb\b/z003034l.pfb/x',
	's/\buzdr.pfb\b/d050000l.pfb/x'
		   );

my @fileADOBE = (
	's/\buagd8a.pfb\b/agd_____.pfb/x',
	's/\buagdo8a.pfb\b/agdo____.pfb/x',
	's/\buagk8a.pfb\b/agw_____.pfb/x',
	's/\buagko8a.pfb\b/agwo____.pfb/x',
	's/\bubkd8a.pfb\b/bkd_____.pfb/x',
	's/\bubkdi8a.pfb\b/bkdi____.pfb/x',
  's/\bubkl8a.pfb\b/bkl_____.pfb/x',
	's/\bubkli8a.pfb\b/bkli____.pfb/x',
	's/\bucrb8a.pfb\b/cob_____.pfb/x',
	's/\bucrbo8a.pfb\b/cobo____.pfb/x',
	's/\bucrr8a.pfb\b/com_____.pfb/x',
	's/\bucrro8a.pfb\b/coo_____.pfb/x',
	's/\buhvb8a.pfb\b/hvb_____.pfb/x',
	's/\buhvb8ac.pfb\b/hvnb____.pfb/x',
	's/\buhvbo8a.pfb\b/hvbo____.pfb/x',
	's/\buhvbo8ac.pfb\b/hvnbo___.pfb/x',
	's/\buhvr8a.pfb\b/hv______.pfb/x',
	's/\buhvr8ac.pfb\b/hvn_____.pfb/x',
	's/\buhvro8a.pfb\b/hvo_____.pfb/x',
	's/\buhvro8ac.pfb\b/hvno____.pfb/x',
	's/\buncb8a.pfb\b/ncb_____.pfb/x',
	's/\buncbi8a.pfb\b/ncbi____.pfb/x',
	's/\buncr8a.pfb\b/ncr_____.pfb/x',
	's/\buncri8a.pfb\b/nci_____.pfb/x',
	's/\buplb8a.pfb\b/pob_____.pfb/x',
	's/\buplbi8a.pfb\b/pobi____.pfb/x',
	's/\buplr8a.pfb\b/por_____.pfb/x',
	's/\buplri8a.pfb\b/poi_____.pfb/x',
	's/\busyr.pfb\b/sy______.pfb/x',
	's/\butmb8a.pfb\b/tib_____.pfb/x',
	's/\butmbi8a.pfb\b/tibi____.pfb/x',
	's/\butmr8a.pfb\b/tir_____.pfb/x',
	's/\butmri8a.pfb\b/tii_____.pfb/x',
	's/\buzcmi8a.pfb\b/zcmi____.pfb/x',
	's/\buzdr.pfb\b/zd______.pfb/x'
		);

&main;
exit 0;

###############################################################################
# progname()
#   return the name of the program.  Needed if invoked by a wrapper. 
###############################################################################
sub progname {
  if (`kpsewhich --var-value=TEXMFVAR` eq `kpsewhich --var-value=TEXMFSYSVAR`) {
    return 'updmap-sys';
  } else {
    return 'updmap';
  }
}

# return program name + version string.
sub version {
  my $ret = sprintf "%s version %s", &progname(), $version;
  return $ret;
}

###############################################################################
# equalize_file(filename[, comment_char])
#   read a file and return its processed content as a string.
#   look into the source code for more details.
###############################################################################
sub equalize_file {
  my $file=shift;
  my $comment=shift;
  my @temp;

  open IN, "$file";
  my @lines=(<IN>);
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
###############################################################################
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
###############################################################################
sub files_are_equal {
  return (&files_are_different (@_))? 0:1;
}

###############################################################################
# files_are_identical(file_A, file_B)
#   compare two files.  Same as cmp(1).
###############################################################################
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
#   return the lines in $filename
###############################################################################
sub getLines {
  my @lines = ();
  foreach my $fname (@_) {
    next if (! $fname);
    if (! exists $maps{"$fname"}) {
      open FILE, "<$fname" or die "can't get lines from $fname: $!";
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
###############################################################################
sub writeLines {
  my ($fname, @lines) = @_;
  map { ($_ !~ m/\n$/ ? s/$/\n/ : $_ ) } @lines;
  open FILE, ">$fname" or die "can't write lines to $fname: $!";
  print FILE @lines;
  close FILE;
}

###############################################################################
# copyFile()
#   copy file $src to $dst, sets $dst creation and mod time
###############################################################################
sub copyFile {
  my ($src, $dst) = @_;
  my $dir;
  ($dir=$dst)=~s/(.*)\/.*/$1/;
  mkdirhier $dir;

  $src eq $dst && return "can't copy $src to itself!\n";

  open IN, "<$src" or die "can't open source file $src for copying: $!";
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
# help()
#   display help message and exit
###############################################################################
sub help {
  my $progname=&progname();
  my $usage= <<"EOF";
Usage: $progname [option] ... [command]

Update the default font map files used by pdftex, dvips, and dvipdfm, as
determined by updmap.cfg (the one returned by running "kpsewhich
updmap.cfg").

Among other things, these font map files are used to determine which
fonts should be used as bitmaps and which as outlines, and to determine
which fonts are included in the output.

By default, the TeX filename database is also rebuilt (with mktexlsr).

Valid options:
  --cnffile FILE            read FILE for the updmap configuration
  --dvipsoutputdir DIR      specify output directory (dvips syntax)
  --pdftexoutputdir DIR     specify output directory (pdftex syntax)
  --outputdir DIR           specify output directory (for all files)
  --copy                    cp generic files rather than using symlinks
  --nomkmap                 do not recreate map files
  --nohash                  do not run texhash
  -n, --dry-run             only show the configuration, no output
  --quiet                   reduce verbosity

Valid commands:
  --help                    show this message
  --edit                    edit updmap.cfg file
  --showoptions ITEM        show alternatives for options
  --setoption OPTION VALUE  set option, where OPTION is one of:
                             dvipsPreferOutline, LW35, dvipsDownloadBase35,
                             or pdftexDownloadBase14
  --setoption OPTION=VALUE  see above, just different syntax
  --enable MAPTYPE MAPFILE  add "MAPTYPE MAPFILE" to updmap.cfg,
                              where MAPTYPE is either Map or MixedMap
  --enable Map=MAPFILE      add \"Map MAPFILE\" to updmap.cfg
  --enable MixedMap=MAPFILE add \"MixedMap MAPFILE\" to updmap.cfg
  --disable MAPFILE         disable MAPFILE, whether Map or MixedMap
  --syncwithtrees           entries with unavailable map files will be
                             disabled in the config file
  --listmaps                list all active and inactive maps
  --listavailablemaps       same as --listmaps, but without
                             unavailable map files

Explanation of the map types: the (only) difference between Map and
MixedMap is that MixedMap entries are not added to psfonts_pk.map.  The
purpose is to help users with printers that render Type 1 outline fonts
worse than mode-tuned Type 1 bitmap fonts.  So MixedMap is used for
fonts that are available as both Type 1 and Metafont.

To see the precise locations of the various files that will be read and
written, run updmap -n.

For step-by-step instructions on making new fonts known to TeX, see
http://tug.org/fonts/fontinstall.html.

Report bugs to: tex-k\@tug.org
TeX Live home page: <http://tug.org/texlive/>
EOF
;
  print $usage;
  exit 0;
}


###############################################################################
# cfgval(variable)
#   read variable ($1) from config file
###############################################################################
sub cfgval {
  my ($variable) = @_;
  my $value;

  if ($#cfg < 0) {
    open FILE, "<$cnfFile" or die "can't open configuration file $cnfFile: $!";
    @cfg = <FILE>;
    close FILE;
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
# SymlinkOrCopy(dir, src, dest)
#   create symlinks if possible, otherwise copy files 
###############################################################################
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
###############################################################################
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
###############################################################################
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
    my @filemode = eval ("\@file" .$mode);
    for my $r (@filemode) {
      map { eval($r); } @lines;
    }
  }
  return @lines;
}

###############################################################################
# locateWeb2c (file ...)
#   apply kpsewhich with format 'web2c files'
###############################################################################
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
#   apply kpsewhich with format 'dvips config'
###############################################################################
sub locateMap {
  my @maps = @_;
  my @files;

  chomp @maps;
  return @maps if ($#maps < 0);

  @files = `kpsewhich --format=map @maps`;
  chomp @files;

  foreach my $map (@maps) {
    push @missing, $map if (! grep /\/$map$/, @files);
 }
  if (wantarray) {
    return @files;
  }
  else {
    return $files[0];
  }
}

###############################################################################
# catMaps(regex)
#   filter config file by regex for map lines and extract the map filenames.
#   These are then looked up (by kpsewhich in locateMap) and the content of
#   all map files is send to stdout.
###############################################################################
sub catMaps {
  my ($map) = @_;
  my %count = ( );
  my @maps = grep { $_ =~ m/$map/ } @cfg;
  map{
    $_ =~ s/\#.*//;
    $_ =~ s/\s*([^\s]*)\s*([^\s]*)/$2/;
  } @maps;
  @maps = sort(@maps);
  @maps = grep { ++$count{$_} < 2; } @maps;

  @maps = &locateMap(@maps);
  return @maps;
}

###############################################################################
# configReplace(file, pattern, line)
#   The first line in file that matches pattern gets replaced by line.
#   line will be added at the end of the file if pattern does not match.
###############################################################################
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
# setOption (option, value)
#   sets option to value in the config file (replacing the existing setting
#   or by adding a new line to the config file).
###############################################################################
sub setOption {
  my ($opt, $val) = @_;

  if ($opt eq "LW35") {
    if ($val !~ m/^(URWkb|URW|ADOBE|ADOBEkb)$/) {
      die "Invalid value $val for option $opt.\n";
    }
  }
  elsif ($opt =~ 
         m/^(dvipsPreferOutline|dvipsDownloadBase35|pdftexDownloadBase14)$/) {
    if ($val !~ m/^(true|false)$/) {
      die "invalid value $val for option $opt.\n";
    }
  }
  else {
    die "Unsupported option $opt.\n";
  }
  &configReplace("$cnfFile", "^" . "$opt" . "\\s", "$opt $val");
}

###############################################################################
# enableMap (type, map)
#   enables an entry in the config file for map with a given type.
###############################################################################
sub enableMap {
  my ($type, $map) = @_;

  if ($type !~ m/^(Map|MixedMap)$/) {
    die "updmap: Invalid mapType $type\n";
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
###############################################################################
sub disableMap {
  my ($map) = @_;
  my %count = ();
  my $type;

  my @mapType = grep {
    my @fields = split;
    if ($fields[0] and $fields[0] =~ /^(MixedMap|Map)$/
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
# initVars()
#   initialize global variables
###############################################################################
sub initVars {
  $progname="updmap";
  $quiet = 0;
  $nohash = 0;
  $nomkmap = 0;
  $verbose = 1;
  $cnfFile = "";
  $cnfFileShort = "updmap.cfg";
  $outputdir = "";
  chomp($TEXMFMAIN =`kpsewhich --var-value=TEXMFMAIN`);
}

###############################################################################
# showOptions(item)
#   show Options for an item
###############################################################################
sub showOptions {
  foreach my $item (@_) {
    if ($item eq "LW35") {
      print "URWkb URW ADOBE ADOBEkb\n";
    }
    elsif ($item =~ 
           m/(dvipsPreferOutline|pdftexDownloadBase14|dvipsDownloadBase35)/) {
      print "true false\n";
    }
    else {
      print "Unknown item \"$item\". Choose one of LW35, dvipsPreferOutline,\n" 
          . "  dvipsDownloadBase35, or pdftexDownloadBase14\n";
    }
  }
  exit 0
}

###############################################################################
# setupDestDir()
#   find an output directory if none specified on cmd line. First choice is
#   $TEXMFVAR/fonts/map/updmap (if TEXMFVAR is set), next is relative to
#   config file location. Fallback is $TEXMFMAIN/fonts/map/updmap.
###############################################################################
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
        die "Directory \"$tf/$rel\" isn't writable.\n";
      }
    }
    $od = "$tf/$rel";
  }
  &mkdirhier($od);
  print "$driver output dir: \"$od\"\n" if (! $quiet);
  return $od;
}

sub setupDestDir {
  $dvipsoutputdir = &setupOutputDir($dvipsoutputdir, "dvips");
  $pdftexoutputdir = &setupOutputDir($pdftexoutputdir, "pdftex");
}

###############################################################################
# setupCfgFile()
#   find config file if none specified on cmd line.
###############################################################################
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
        print("copy $original_cfg => $tf/web2c/$cnfFileShort\n") if (! $quiet);
        $newcnf="$tf/web2c/$cnfFileShort";
        &copyFile("$original_cfg", "$tf/web2c/$cnfFileShort");
        $updLSR->{add}("$tf/web2c/$cnfFileShort");
      }
    }
    $cnfFile = "$tf/web2c/updmap.cfg";
    if ($cnfFile) {
      print "Config file: \"$cnfFile\"\n" if (! $quiet);
    }
    else {
      die "Config file updmap.cfg not found.\n";
    }
  }
}

###############################################################################
# processOptions()
#   process cmd line options
###############################################################################
sub processOptions {
  unless (&GetOptions (
      "q|quiet" => \$quiet,
		  "cnffile=s" => \$cnfFile,
		  "outputdir=s" => \$outputdir,
		  "dvipsoutputdir=s" => \$dvipsoutputdir,
		  "pdftexoutputdir=s" => \$pdftexoutputdir,
		  "setoption=s" => \%setOption,
		  "enable=s" => \$enableItem,
		  "disable=s" => \@disableItem,
		  "e|edit" => \$opt_edit,
		  "l|listmaps" => \$listmaps,
		  "listavailablemaps" => \$listavailablemaps,
		  "syncwithtrees" => \$syncwithtrees,
		  "showoptions=s" => \@showoptions,
		  "nohash" => \$nohash,
		  "nomkmap" => \$nomkmap,
		  "version" => sub { print &version() . "\n"; exit(0) },

      "n|dry-run" => \$dry_run,
      "copy" => \$copy,
		  "h|help" => \$opt_help)) {
    my $progname=&progname();
    die "Try \"$progname --help\".\n";
  }

  if ($outputdir) {
    $dvipsoutputdir = $outputdir if (! $dvipsoutputdir);
    $pdftexoutputdir = $outputdir if (! $pdftexoutputdir);
  }
  if ($cnfFile && ! -f $cnfFile) {
    die "Config file \"$cnfFile\" not found.\n";
  }
  if ($dvipsoutputdir && ! -d $dvipsoutputdir) {
    &mkdirhier ($dvipsoutputdir);
  }
  if ($pdftexoutputdir && ! -d $pdftexoutputdir) {
    &mkdirhier ($pdftexoutputdir);
  }
}

###############################################################################
# listMaps()
#   list all maps mentioned in the config file
###############################################################################
sub listMaps {
  my $what=shift;
  my @mapfiles;
  my @paths;

  my @lines = grep {
    if ($what eq 'sync') {
      $_ =~ m/^(Mixed)?Map/
    } else {
      $_ =~ m/^(\#! *)?(Mixed)?Map/
    }
  } &getLines($cnfFile);

  if ($what eq 'list') {
    # --listmaps
    map { print "$_\n"; } @lines;
  } else {
    @mapfiles=grep { $_ =~ s/^(\#! *)?(Mixed)?Map\s+// } @lines;
    @paths=&locateMap(@mapfiles);

    if ($what eq 'avail') {
      # --listavailablemaps
      map {
        my $entry="$_"; 
      #  print "$entry\n" if (grep { $_ =~ m/\/$entry/ } @paths);
      } @lines;
    } elsif ($what eq 'sync') {
      # --syncwithtrees
      map { 
        my $entry="$_"; 
        unless (grep { $_ =~ m/$entry/ } @paths) {
          &disableMap($entry);
          print "  $entry disabled\n" if (! $quiet); 
        }
      } @lines;
    }
  }
}

###############################################################################
# normalizeLines()
#   remove comments, whitespace is exactly one space, no empty lines,
#   no whitespace at end of line, one space before and after "
###############################################################################
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
#   strip "PS_Encoding_Name ReEncodeFont" from map entries because
#   they are ignored by pdftex anyway.
###############################################################################
sub to_pdftex {
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
# mkMaps()
#   the main task of this script: create the output files
###############################################################################
sub mkMaps {
  my @lines;
  my $logfile;

  $cache=1;

  if (! $dry_run) {
    my $TEXMFVAR=`kpsewhich --var-value=TEXMFVAR`;
    chomp($TEXMFVAR);
    $logfile="$TEXMFVAR/web2c/updmap.log";
    mkdirhier "$TEXMFVAR/web2c";
    open LOG, ">$logfile" 
        or die "Can't open \"$logfile\"";
    $writelog=1;
    print LOG &version() . "\n";
    printf LOG "%s\n\n", scalar localtime();
    print LOG  "Using config file \"$cnfFile\".\n";
  }
  sub wlog () {
    my $str=shift;
    if ($dry_run) {
      print $str;
    } else {
      print $str if (! $quiet);
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

  &wlog ("\nupdmap is creating new map files " . 
         "using the following configuration:" .
         "\n  LW35 font names                  : " . 
         $mode .
         "\n  prefer outlines                  : " . 
         ($dvipsPreferOutline ? "true" : "false") .
         "\n  texhash enabled                  : " .
         ($nohash ? "false" : "true") .
         "\n  download standard fonts (dvips)  : " .
         ($dvipsDownloadBase35 ? "true" : "false") .
         "\n  download standard fonts (pdftex) : " .
         ($pdftexDownloadBase14 ? "true" : "false") . 
         "\n\n");

  &wlog ("Scanning for LW35 support files");
  $dvips35 = &locateMap("dvips35.map");
  $pdftex35 = &locateMap("pdftex35.map");
  $ps2pk35 = &locateMap("ps2pk35.map");
  my $LW35="\n$dvips35\n$pdftex35\n$ps2pk35\n\n";
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
    my $progname=&progname();
    print STDERR "\n\n\tDid you run mktexlsr?\n\n" .
        "\tYou can delete non-existent map entries using the command\n".
        "\n\t  $progname --syncwithtrees\n\n";
    exit (1);
  }
  exit(0) if $dry_run;

  # Create psfonts_t1.map, psfonts_pk.map, ps2pk.map and pdftex.map:
  for my $file ("$dvipsoutputdir/download35.map",
		"$dvipsoutputdir/builtin35.map",
		"$dvipsoutputdir/psfonts_t1.map",
		"$dvipsoutputdir/psfonts_pk.map",
		"$pdftexoutputdir/pdftex_dl14.map",
		"$pdftexoutputdir/pdftex_ndl14.map",
		"$dvipsoutputdir/ps2pk.map") {
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

  print "Generating output for ps2pk...\n" if (! $quiet);
  my @ps2pk_map = &transLW35($ps2pk35);
  push @ps2pk_map, &getLines(@tmp1);
  push @ps2pk_map, &getLines(@tmp2);
  &writeLines(">$dvipsoutputdir/ps2pk.map", 
              &normalizeLines(@ps2pk_map));

  print "Generating output for dvips...\n" if (! $quiet);
  my @download35_map = &transLW35($ps2pk35);
  &writeLines(">$dvipsoutputdir/download35.map", 
              &normalizeLines(@download35_map));

  my @builtin35_map = &transLW35($dvips35);
  &writeLines(">$dvipsoutputdir/builtin35.map", 
              &normalizeLines(@builtin35_map));

  my $dftdvips = ($dvipsDownloadBase35 ? $ps2pk35 : $dvips35);

  my @psfonts_t1_map = &transLW35($dftdvips);
  push @psfonts_t1_map, &getLines(@tmp1);
  push @psfonts_t1_map, &getLines(@tmp2);
  &writeLines(">$dvipsoutputdir/psfonts_t1.map", 
              &normalizeLines(@psfonts_t1_map));

  my @psfonts_pk_map = &transLW35($dftdvips);
  push @psfonts_pk_map, &getLines(@tmp2);
  &writeLines(">$dvipsoutputdir/psfonts_pk.map", 
              &normalizeLines(@psfonts_pk_map));

  print "Generating output for pdftex...\n" if (! $quiet);
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
  @pdftex_ndl14_map = &to_pdftex(@pdftex_ndl14_map) if ($pdftexNoEnc);
  &writeLines(">$pdftexoutputdir/pdftex_ndl14.map", @pdftex_ndl14_map);

  my @pdftex_dl14_map = @tmp7;
  @pdftex_dl14_map = &normalizeLines(@pdftex_dl14_map);
  @pdftex_dl14_map = &to_pdftex(@pdftex_dl14_map) if ($pdftexNoEnc);
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
  close LOG;
  print "\nTranscript written on \"$logfile\".\n" if (! $quiet);
}

###############################################################################
# main()
#   execution starts here
###############################################################################
sub main {
  &initVars;
  &processOptions;

  if (@showoptions) {
    &showOptions(@showoptions);
    exit 0;
  }

  &help if ($opt_help);

  &setupCfgFile;

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
    my $editor = undef;
    $editor ||= $ENV{'EDITOR'};
    $editor ||= $ENV{'VISUAL'};
    if (&win32) {
      $editor ||= "notepad";
    } else {
      $editor ||= "vi";
    }
    $cmd = 'edit';
    system("$editor $cnfFile");
  }
  elsif (keys %setOption) {
    $cmd = 'setOption';
    foreach my $m (keys %setOption) {
      &setOption ($m, $setOption{$m});
    }
  }
  elsif ($enableItem) {
    $cmd = 'enableMap';
    if ($enableItem=~/=/) {
      &enableMap(split('=', $enableItem));
    } else {
      &enableMap($enableItem, shift @ARGV);
    }
  }
  elsif (@disableItem) {
    $cmd = 'disableMap';
    foreach my $m (@disableItem) {
      &disableMap($m);
    }
  }
  if ($cmd and &files_are_equal($bakFile, $cnfFile)) {
    print "$cnfFile unchanged. Map files not recreated.\n" unless ($quiet);
  }
  else {
    if (! $nomkmap) {
      &setupDestDir;
      &mkMaps;
    }
    unlink ($bakFile);
  }
  $updLSR->{exec}() unless $nohash;
}
__END__

### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim:set tabstop=2 expandtab: #
