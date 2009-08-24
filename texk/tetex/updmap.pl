#!/usr/bin/env perl
###############################################################################
# updmap: utility to maintain map files for outline fonts.
#
# Thomas Esser, (C) 2002. Public domain.
# Fabrice Popineau, for the Perl version.
#
# Commands:
#  --edit                     edit config file
#  --help                     show help message
#
# Options:
#  --cnffile file             specify configuration file
#   --dvipsoutputdir directory   specify output directory (dvips syntax)
#   --pdftexoutputdir directory  specify output directory (pdftex syntax)
#   --dvipdfmoutputdir directory specify output directory (dvipdfm syntax)
#   --outputdir directory      specify output directory (for all files)
#  --quiet                    reduce verbosity
###############################################################################
#$^W=1;

require "newgetopt.pl";
use strict;

#  my $IsWin32 = ($^O =~ /MSWin32/i);

#  if ($IsWin32) {
#    use Win32::Registry;
#    use Win32::API;
#  }


my $progname;
my $cnfFile;
my $cnfFileShort;
my $outputdir;
my $dvipsoutputdir;
my $pdftexoutputdir;
my $dvipdfmoutputdir;
my $quiet;
my $texhashEnabled;
my $mkmapEnabled;

my %enableItem = ();
my %setOption = ();
my @showoptions = ();
my @disableItem = ();
my $listmaps;
my $needsCleanup;

my $verbose;
my $debug;
my $opt_edit;
my $opt_help;

my $TEXMFMAIN;
my $TEXMFVAR;

my $mode;
my $dvipsPreferOutline;
my $dvipsDownloadBase35;
my $pdftexDownloadBase14;
my $dvipdfmDownloadBase14;

my $dvips35;
my $pdftex35;
my $dvipdfm35;
my $ps2pk35;

my @cfg = ( );

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

#  if ($IsWin32) {

#    sub GetTempPath {
#      my $GetTempPath = new Win32::API('kernel32', 'GetTempPath', 'NP', 'N');
#      if(not defined $GetTempPath) {
#        die "Can't import API GetTempPath: $!\n";
#      }
#      my $lpBuffer = " " x 260;

#      $GetTempPath->Call(80, $lpBuffer);
#    }

#    sub ShellExecute {
#      my ($command, $file) = @_;
#      my $ShellExecute = new Win32::API('shell32', 'ShellExecute', 'NPPPPN', 'N');
#      if(not defined $ShellExecute) {
#        die "Can't import API ShellExecute: $!\n";
#      }
#      $ShellExecute->Call(0, $command, $file, '', '', 0);
#    }

#    sub GetHomeDir {
#      my $home = $ENV{'HOME'};
#      # Should check for CSIDL_...
#      return $home;
#    }
#  }


###############################################################################
# mkdirhier(directory)
#   create a directory and parent directories as needed
###############################################################################
sub mkdirhier {
  my $tree=shift;
  my $mode=shift;
  my $subdir;

  return if (-d $tree);
  my @dirs=split /\//, $tree;
  foreach my $dir (@dirs) {
    $subdir .= ("$dir" . "/");
    unless (-d $subdir)  {
      if (defined $mode) {
        mkdir ("$subdir", $mode) or die "Can't mkdir '$subdir': $!.\n";
      } else {
        mkdir "$subdir" or die "Can't mkdir '$subdir': $!.\n";
      }
    }
  }
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
# getLines()
#   return the lines in $filename
###############################################################################
sub getLines {
  my @lines = ();
  foreach my $fname (@_) {
    next if (! $fname);
    open FILE, "<$fname" or die "can't get lines from $fname: $!";
    push @lines, <FILE>;
    close FILE;
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
# start_redirection(), stop_redirection()
#   redirects stdout and stderr to log file (possibly "nul")
###############################################################################
sub start_redirection {
  my ($log) = @_;

  # start redirection if asked
  if ($log) {
    open(SO, ">&STDOUT");
    open(SE, ">&STDERR");

    close(STDOUT);
    close(STDERR);

    open(STDOUT, ">$log");
    open(STDERR,">&STDOUT");

    select(STDERR); $| = 1;
    select(STDOUT); $| = 1;
  }
}

sub stop_redirection {
  close(STDOUT);
  close(STDERR);
  open(STDOUT, ">&SO");
  open(STDERR, ">&SE");
}

###############################################################################
# help()
#   display help message and exit
###############################################################################
sub help {
  print "Usage: $0 [option] ... [command]\n";
  print "\n";
  print "Valid options:\n";
  print "  --cnffile file               specify configuration file\n";
  print "  --dvipsoutputdir directory   specify output directory (dvips syntax)\n";
  print "  --pdftexoutputdir directory  specify output directory (pdftex syntax)\n";
  print "  --dvipdfmoutputdir directory specify output directory (dvipdfm syntax)\n";
  print "  --outputdir directory        specify output directory (for all files)\n";
  print "  --nohash                     do not run texhash\n";
  print "  --nomkmap                    do not recreate map files\n";
  print "  --quiet                      reduce verbosity\n";
  print "\n";
  print "Valid commands:\n";
  print "  --edit                       edit updmap.cfg file\n";
  print "  --help                       show this message\n";
  print "  --showoptions item           show alternatives for options\n";
  print "  --setoption option=value     set option where option is one\n";
  print "                               of dvipsPreferOutline, LW35, dvipsDownloadBase35\n";
  print "                               or pdftexDownloadBase14\n";
  print "  --enable maptype=mapfile     add or enable a Map or MixedMap\n";
  print "  --disable mapfile            disable Map or MixedMap for mapfile\n";
  print "  --listmaps                   list all active and inactive maps\n";
  exit 0;
}

###############################################################################
# abort(errmsg)
#   print `errmsg' to stderr and exit with error code 1
###############################################################################
sub abort {
  my  ($msg) = @_;
  print STDERR "$progname: $msg\n";
  exit 1;
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
      print "$variable => " . $value . "\n";
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
# setupSymlinks()
#   set symlink for psfonts.map according to dvipsPreferOutline variable
###############################################################################
sub setupSymlinks {
  my $src;

  if ($dvipsPreferOutline ) {
    $src = "psfonts_t1.map";
  } else {
    $src = "psfonts_pk.map";
  }
  unlink "$dvipsoutputdir/psfonts.map";
  &copyFile("$dvipsoutputdir/$src", "$dvipsoutputdir/psfonts.map");

  if ($pdftexDownloadBase14) {
    $src = "pdftex_dl14.map";
  } else {
    $src = "pdftex_ndl14.map";
  }
  unlink "$pdftexoutputdir/pdftex.map";
  &copyFile("$pdftexoutputdir/$src", "$pdftexoutputdir/pdftex.map");

  if ($dvipdfmDownloadBase14) {
    $src = "dvipdfm_dl14.map";
  } else {
    $src = "dvipdfm_ndl14.map";
  }
  unlink "$dvipdfmoutputdir/dvipdfm.map";
  &copyFile("$dvipdfmoutputdir/$src", "$dvipdfmoutputdir/dvipdfm.map");
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
  my @files = @_;

  chomp @files;
  return @files if ($#files < 0);

  @files = `kpsewhich --format=map @files`;
  chomp @files;
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
    $_ =~ s/\s*([^\s]*)\s*([^\s]*)/\2/;
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
	printf STDERR "invalid value $val for option $opt\n";
	exit 1;
      }
  }
  elsif ($opt =~ m/^(dvipsPreferOutline|dvipsDownloadBase35|pdftexDownloadBase14|dvipdfmDownloadBase14)$/) {
    if ($val !~ m/^(true|false)$/) {
      printf STDERR "invalid value $val for option $opt\n";
      exit 1;
    }
  }
  else {
    printf STDERR "unsupported option $opt\n";
    exit 1;
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
    printf STDERR "invalid mapType $type\n";
    exit 1;
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
  my %count = ( );
  my $type;

  my @mapType = grep {
    my @fields = split;
    if ($fields[0] =~ /^(MixedMap|Map)$/
	and $fields[1] eq $map
	and ++$count{$fields[0]}) {
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
  $texhashEnabled = 1;
  $mkmapEnabled = 1;
  $verbose = 1;
  $needsCleanup = 0;

  $cnfFile = "";
  $cnfFileShort = "updmap.cfg";
  $outputdir = "";
  $TEXMFMAIN =`kpsewhich --expand-var="\$TEXMFMAIN"`;
  chomp($TEXMFMAIN);
  #    tmpdir=${TMP-/tmp}/$progname.$$
  #    tmp1=$tmpdir/a
  #    tmp2=$tmpdir/b
  #    tmp3=$tmpdir/c
}

###############################################################################
# setupTmpDir()
#   set up a temp directory and a trap to remove it
###############################################################################
sub setupTmpDir {

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
    elsif ($item =~ m/"(dvipsPreferOutline|pdftexDownloadBase14|dvipdfmDownloadBase14|dvipsDownloadBase35)"/) {
      print "true false\n";
    }
    else {
      print "Unknown item \"$item\". Choose one of LW35, dvipsPreferOutline, dvipsDownloadBase35, pdftexDownloadBase14 or dvipdfmDownloadBase14\n";
#      exit 1;
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
    # be written. Copy config file if it does not exist there.
    if ($^O=~/^MSWin(32|64)$/) {
	$tf = `kpsewhich --expand-var="\$TEXMFVAR"`;
    } else {
	$tf = `kpsewhich --expand-var="\\\$TEXMFVAR"`;
    }
    chomp($tf);
    if ($tf) {
      &mkdirhier("$tf/$rel");
      # system("$TEXMFMAIN/web2c/mktexdir \"$tf/$rel\"");
      if (! -d "$tf/$rel" || ! -w "$tf/$rel") {
        # forget about TEXMFVAR tree...
	$tf = "";
      }
    }

    # Try something relative to config file, fall back to $TEXMFMAIN.
    if (! $tf) {
      $tf = $cnfFile;
      if ($tf =~ m@/web2c/[^/]*$@) {
	$tf =~ s@/web2c/[^/]*$@@;
      }
      else {
	$tf = "";
      }
      $tf = $TEXMFMAIN if (! $tf);
    }

    $od = "$tf/$rel";
  }

  # No need to call mktexdir !
  # system("$TEXMFMAIN/bin/win32/mktexdir \"$od\"");
  &mkdirhier($od);
  &abort("output directory `$od' does not exist\n") if (! -d $od);
  &abort("output directory `$od' is not writable\n") if (! -w $od);
  print "using $driver output directory `$od'\n";

  return $od;
}

sub setupDestDir {

  $dvipsoutputdir = &setupOutputDir($dvipsoutputdir, "dvips");
  $pdftexoutputdir = &setupOutputDir($pdftexoutputdir, "pdftex");
  $dvipdfmoutputdir = &setupOutputDir($dvipdfmoutputdir, "dvipdfm");

}

###############################################################################
# setupCfgFile()
#   find config file if none specified on cmd line.
###############################################################################
sub setupCfgFile {

  if (! $cnfFile) {
    my $tf = `kpsewhich -expand-var="\$TEXMFVAR"`;
    chomp($tf);
    if ($tf && ! -f "$tf/web2c/$cnfFileShort") {
      &mkdirhier("$tf/web2c") if (! -d "$tf/web2c");
#	&start_redirection("nul");
#	system("mktexdir $tf/web2c");
#	&stop_redirection;
      if (-d "$tf/web2c" && -w "$tf/web2c") {
	unlink "$tf/web2c/$cnfFileShort";
	&copyFile("$TEXMFMAIN/web2c/$cnfFileShort", "$tf/web2c/$cnfFileShort");
	&start_redirection("nul");
	system("mktexupd $tf/web2c $cnfFileShort");
	&stop_redirection;
      }
    }

    $cnfFile = &locateWeb2c("updmap.cfg");
    if ($cnfFile) {
      print "using config file $cnfFile\n" if (! $quiet);
    }
    else {
      &abort("config file updmap.cfg not found");
    }
  }
}


###############################################################################
# processOptions()
#   process cmd line options
###############################################################################
sub processOptions {

  unless (&NGetOpt ("quiet" => \$quiet,
		    "cnffile=s" => \$cnfFile,
		    "outputdir=s" => \$outputdir,
		    "dvipsoutputdir=s" => \$dvipsoutputdir,
		    "pdftexoutputdir=s" => \$pdftexoutputdir,
		    "dvipdfmoutputdir=s" => \$dvipdfmoutputdir,
		    "setoption=s" => \%setOption,
		    "enable=s" => \%enableItem,
		    "disable=s" => \@disableItem,
		    "edit" => \$opt_edit,
		    "listmaps" => \$listmaps,
		    "showoptions=s" => \@showoptions,
		    "hash!" => \$texhashEnabled,
		    "mkmap!" => \$mkmapEnabled,
		    "verbose" => \$verbose,
		    "debug" => \$debug,
		    "help" => \$opt_help)) {
    print STDERR "Try `$0 --help'";
    exit 1;
  }

  if ($outputdir) {
    $dvipsoutputdir = $outputdir if (! $dvipsoutputdir);
    $pdftexoutputdir = $outputdir if (! $pdftexoutputdir);
    $dvipdfmoutputdir = $outputdir if (! $dvipdfmoutputdir);
  }
  if ($cnfFile && ! -f $cnfFile) {
    &abort("config file `$cnfFile' not found");
  }

  if ($dvipsoutputdir && ! -d $dvipsoutputdir) {
    &abort("dvips output directory `$dvipsoutputdir' not found");
  }
  if ($pdftexoutputdir && ! -d $pdftexoutputdir) {
    &abort("pdftex output directory `$pdftexoutputdir' not found");
  }
  if ($dvipdfmoutputdir && ! -d $dvipdfmoutputdir) {
    &abort("dvipdfm output directory `$dvipdfmoutputdir' not found");
  }

}

###############################################################################
# listMaps()
#   list all maps mentioned in the config file
###############################################################################
sub listMaps {
  my @lines = grep {
    $_ =~ m/^(\#! *)?(Mixed)?Map/
  } &getLines($cnfFile);
  map { print "$_\n"; } @lines;
#  &writeLines("con", @lines);
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
	$_ =~ s/\" ([^\"]*) \"/\"\1\"/gx;
      } @lines;

  @lines = grep {++$count{$_} < 2 } (sort @lines);

  return @lines;
}

###############################################################################
# dvips2dvipdfm()
#   reads from stdin, writes to stdout. It transforms "dvips"-like syntax into
#   "dvipdfm"-like syntax. It is a very ugly hack.
###############################################################################
sub dvips2dvipdfm {
  my @lines = @_;

  map {
    $_ =~ s@$@ %@;
    $_ =~ s@^(([^ ]*).*)@\1\2@;
    $_ =~ s@(.*<\[* *([^ ]*)\.enc(.*))@\1 \2@;
    $_ = (m/%[^ ]*$/x ? "$_ default" : $_) ;
    $_ =~ s@(.*<<* *([^ ]*)\.pf[ab].*)@\1 \2@;
    if (m/%[^ ]* [^ ]*$/x) { $_ =~ s@( ([^ ]*).*)$@\1 \2@; }
    $_ =~ s@(.* ([\.0-9-][\.0-9-]*) *ExtendFont.*)@\1 -e \2@;
    $_ =~ s@(.* ([\.0-9-][\.0-9-]*) *SlantFont.*)@\1 -s \2@;
    $_ =~ s@.*%@@ ;
  } @lines;

  map {
    my @elts = split (' ', $_);
    if ($elts[0] eq $elts[2]
	&& $elts[1] eq "default") {
      $elts[1] = '';
      $elts[2] = '';
    };
    $_ = join ' ',@elts;
  } @lines;

  map {
    if (/^(cm|eu|la|lc|line|msam|xy)/) {
      $_ .= " -r" ;
      $_ =~ s/(fmex[789]) -r/\1/;
    }
  } @lines;

  return @lines;
}

###############################################################################
# mkMaps()
#   the main task of this script: create the output files
###############################################################################
sub mkMaps {
  my @lines;

  $mode = &cfgval("LW35");
  $mode = "URWkb" unless (defined $mode);

  $dvipsPreferOutline = &cfgval("dvipsPreferOutline");
  $dvipsPreferOutline = 1 unless (defined $dvipsPreferOutline);

  $dvipsDownloadBase35 = &cfgval("dvipsDownloadBase35");
  $dvipsDownloadBase35 = 0 unless (defined $dvipsDownloadBase35);

  $pdftexDownloadBase14 = &cfgval("pdftexDownloadBase14");
  $pdftexDownloadBase14 = 0 unless (defined $pdftexDownloadBase14);

  $dvipdfmDownloadBase14 = &cfgval("dvipdfmDownloadBase14");
  $dvipdfmDownloadBase14 = 0 unless (defined $dvipdfmDownloadBase14);

  if (! $quiet) {
    print "\
updmap is creating new map files using the following configuration:\
\
  config file                      : " . ($cnfFile ? "true" : "false") ."\
  prefer outlines                  : " . ($dvipsPreferOutline ? "true" : "false") ."\
  texhash enabled                  : " . ($texhashEnabled ? "true" : "false") ."\
  download standard fonts (dvips)  : " . ($dvipsDownloadBase35 ? "true" : "false") ."\
  download standard fonts (pdftex) : " . ($pdftexDownloadBase14 ? "true" : "false") . "\
  download standard fonts (dvipdfm): " . ($dvipdfmDownloadBase14 ? "true" : "false") . "\
"
  };

  print "Scanning for LW35 support files\n" if (! $quiet);
  $dvips35 = &locateMap("dvips35.map");
  $pdftex35 = &locateMap("pdftex35.map");
  $dvipdfm35 = &locateMap("dvipdfm35.map");
  $ps2pk35 = &locateMap("ps2pk35.map");

  print "Scanning for MixedMap entries\n" if (! $quiet);
  my @tmp1 = &catMaps('^MixedMap');
  print "Scanning for Map entries\n" if (! $quiet);
  my @tmp2 = &catMaps('^Map');

  # Create psfonts_t1.map, psfonts_pk.map, ps2pk.map and pdftex.map:
  for my $file ("$dvipsoutputdir/download35.map",
		"$dvipsoutputdir/builtin35.map",
		"$dvipsoutputdir/psfonts_t1.map",
		"$dvipsoutputdir/psfonts_pk.map",
		"$pdftexoutputdir/pdftex_dl14.map",
		"$pdftexoutputdir/pdftex_ndl14.map",
		"$dvipdfmoutputdir/dvipdfm_dl14.map",
		"$dvipdfmoutputdir/dvipdfm_ndl14.map",
		"$dvipsoutputdir/ps2pk.map") {
    open FILE, ">$file";
    print FILE "% $file: maintained by the script updmap.\
%   Don't change this file directly. Edit texmf/web2c/$cnfFileShort\
%   and run updmap to recreate this file.\
";
    close FILE;
  }

#  print "$dvips35 $pdftex35 $dvipdfm35 $ps2pk35 @tmp1 @tmp2\n";

  my @ps2pk_map = &transLW35($ps2pk35);
  push @ps2pk_map, &getLines(@tmp1);
  push @ps2pk_map, &getLines(@tmp2);
  &writeLines(">$dvipsoutputdir/ps2pk.map", &normalizeLines(@ps2pk_map));

  my @download35_map = &transLW35($ps2pk35);
  &writeLines(">$dvipsoutputdir/download35.map", &normalizeLines(@download35_map));
  my @builtin35_map = &transLW35($dvips35);
  &writeLines(">$dvipsoutputdir/builtin35.map", &normalizeLines(@builtin35_map));

  my $dftdvips = ($dvipsDownloadBase35 ? $ps2pk35 : $dvips35);

  my @psfonts_t1_map = &transLW35($dftdvips);
  push @psfonts_t1_map, &getLines(@tmp1);
  push @psfonts_t1_map, &getLines(@tmp2);
  &writeLines(">$dvipsoutputdir/psfonts_t1.map", &normalizeLines(@psfonts_t1_map));

  my @psfonts_pk_map = &transLW35($dftdvips);
  push @psfonts_pk_map, &getLines(@tmp2);
  &writeLines(">$dvipsoutputdir/psfonts_pk.map", &normalizeLines(@psfonts_pk_map));

  # remove PaintType due to Sebastian's request
  my @tmp3 = &transLW35($pdftex35);
  push @tmp3, &getLines(@tmp1);
  push @tmp3, &getLines(@tmp2);
  @tmp3 = grep { $_ !~ m/(^%|PaintType)/ } @tmp3;

  my @tmp6 = &transLW35($dvipdfm35);
  push @tmp6, &getLines(@tmp1);
  push @tmp6, &getLines(@tmp2);
  @tmp6 = grep { $_ !~ m/(^%|PaintType)/ } @tmp6;

  my @tmp7 = &transLW35($ps2pk35);
  push @tmp7, &getLines(@tmp1);
  push @tmp7, &getLines(@tmp2);
  @tmp7 = grep { $_ !~ m/(^%|PaintType)/ } @tmp7;

  my @pdftex_ndl14_map = @tmp3;
  @pdftex_ndl14_map = &normalizeLines(@pdftex_ndl14_map);
  &writeLines(">$pdftexoutputdir/pdftex_ndl14.map", @pdftex_ndl14_map);

  my @pdftex_dl14_map = @tmp7;
  @pdftex_dl14_map = &normalizeLines(@pdftex_dl14_map);
  &writeLines(">$pdftexoutputdir/pdftex_dl14.map", @pdftex_dl14_map);

  my @dvipdfm_dl14_map = @tmp7;
  @dvipdfm_dl14_map = &normalizeLines(&dvips2dvipdfm(&normalizeLines(@dvipdfm_dl14_map)));
  &writeLines(">$dvipdfmoutputdir/dvipdfm_dl14.map", @dvipdfm_dl14_map);

  my @dvipdfm_ndl14_map = @tmp6;
  @dvipdfm_ndl14_map = &normalizeLines(&dvips2dvipdfm(&normalizeLines(@dvipdfm_ndl14_map)));
  &writeLines(">$dvipdfmoutputdir/dvipdfm_ndl14.map", @dvipdfm_ndl14_map);

  &setupSymlinks;

  if ($texhashEnabled) {
    my $cmd = "mktexlsr";
    &start_redirection("nul") if ($quiet);
    system($cmd);
    &stop_redirection if ($quiet);
  }

  if (! $quiet) {
    print STDOUT "Files generated:\n";
    for my $f ("$dvipsoutputdir/download35.map",
		  "$dvipsoutputdir/builtin35.map",
		  "$dvipsoutputdir/psfonts_t1.map",
		  "$dvipsoutputdir/psfonts_pk.map",
		  "$pdftexoutputdir/pdftex_dl14.map",
		  "$pdftexoutputdir/pdftex_ndl14.map",
		  "$dvipdfmoutputdir/dvipdfm_dl14.map",
		  "$dvipdfmoutputdir/dvipdfm_ndl14.map",
		  "$dvipsoutputdir/ps2pk.map") {
      if (-e $f) {
        my @stat=stat($f);
        printf "%7d %s %s\n", $stat[7], scalar (localtime $stat[9]), $f;
      } else {
        print STDERR "Warning: File $f doesn't exist.\n";
      }
    }
  }
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
    &listMaps;
    exit 0;
  }

  my $bakFile = $cnfFile;
  $bakFile =~ s/\.cfg$/.bak/;
  &copyFile($cnfFile, $bakFile);

  my $cmd = '';

  if ($opt_edit) {
    my $editor = `kpsewhich --expand-var \$TEXEDIT`;
    chomp($editor);
    $editor = "notepad" if ($editor eq "");
    $cmd = 'edit';
    system("$editor $cnfFile");
  }

  elsif (keys %setOption) {
    $cmd = 'setOption';
    foreach my $m (keys %setOption) {
      &setOption ($m, $setOption{$m});
    }
  }

  elsif (keys %enableItem) {
    $cmd = 'enableMap';
    foreach my $m (keys %enableItem) {
      &enableMap($m, $enableItem{$m});
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
    if ($mkmapEnabled) {
      &setupDestDir;
      &mkMaps;
    }
    # &cleanup;
    unlink ($bakFile);
  }

}
__END__

### Local Variables:
### perl-indent-level: 2
### tab-width: 2
### indent-tabs-mode: nil
### End:
# vim:set tabstop=2 expandtab: #
