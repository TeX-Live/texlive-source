#
# Purpose: maintain the set of Xemtex files
# Author: F. Popineau
# See the xemtex-files.txt for more information.
#
require "newgetopt.pl";

use lib qw(c:/source/TeXLive/Master/Tools);
use FileUtils;

my $sourcedir;
my $destdir = "c:/Program Files/TeXLive";
my %files = ( );
my $root;

my $opt_verbose = 1;

#my $pattern = "c:/source/xemtex/Master/xemacs-packages/info/cc-mode.*";
#  if (-f $pattern) { print "OK\n"; };
&main;

1;

#
# FIXME: pb with ghostscript !!!
#

sub prune_files {
  my ($dir) = @_;
  my $cut = ($dir =~ m/^CVS$/ || $dir =~ m/[\\\/]CVS$/ ? 1 : 0);
  return $cut;
}

sub check_files {
  my ($dir, @files) = @_;

  $dir =~ s@\\@/@g;
  my $node = $root;
  map {
    $node = $node->{$_};
  } (split ("/", $dir));
  if ($node) {
    for my $f (@files) {
      if (! $node->{$f}) {
	my $name = "$dir/$f";
	$name =~ s@\\@/@g;
	next if ($name !~ m@/xemtex/@);
	#      unlink $name;
	if (-f $name) {
	  print "Unlinking $name\n";
	  unlink($name);
	} elsif (-d $name) {
	  print "Removing directory $name\n";
	  &FileUtils::rec_rmdir($name);
	}
      }
    }
  }
#  else {
#    print "Warning !!! You should remove directory $dir\n";
#  }
}

sub main {


  unless (&NGetOpt ("dry", "verbose")) {
    print STDERR "Try `$0 --help'";
    exit 1;
  };

  my $descfile = $ARGV[0];
  die "usage: $0 <xemtex-files.txt>\nThe list of files is required.\n" if (! $descfile);
  die "$0: $descfile is not a file!\n" if (! -f $descfile);

  open FILE, "<$descfile";
  while (<FILE>) {
    chomp;
    my $line = $_;
    $line =~ s@\\@/@g;
    next if ($line =~ m/^\#/);
    next if ($line =~ m/^\s*$/);
    if ($line =~ m/^\s*(\$[^\s]*\s*=\s*\"[^\"]*\")\s*$/ixo) {
      eval($1);
    } elsif ($line =~ m/^\[\s*source\s*=\s*(\"[^\"]*\")\s*\]/ixo) {
      $sourcedir = eval($1);
      $sourcedir =~ s@\\@/@g;
    }
    elsif ($line =~ m@^([^\s]*)\s*=>\s*([^\s]*)$@) {
     $files{"$sourcedir/$1"} = "$destdir/$2";
    }
    else {
      $files{"$sourcedir/$line"} = "$destdir/$line";
    }
  }
  close FILE;

  my @xemtexfiles = ();
  
  for my $src (keys %files) {
    if (-f $src) {
#      print "file src = $src\n";
      if (-d "$files{$src}") {
        $files{$src} .= "/" . &FileUtils::basename($src);
      }
      my $dst = &FileUtils::dirname($files{$src});
      if (! -d $dst) {
        print "Creating directory $dst\n" if ($opt_verbose || $opt_dry);
        &FileUtils::rec_mkdir($dst) unless($opt_dry);
      }
      if (! -f "$files{$src}") {
        print "Copying $src to $files{$src}\n" if ($opt_verbose || $opt_dry);
	&FileUtils::copy($src, $files{$src}) unless ($opt_dry);
	if ($files{$src} =~ m/\.gz$/) {
	  system ("gzip -qfd $files{$src}");
	  $files{$src} =~ s/\.gz$//;
	}
      } 
      elsif (
	     &FileUtils::newer($src, "$files{$src}") > 0
	    ) {
	&FileUtils::copy($src, $files{$src}) unless($opt_dry);
        print "Copying $src to $files{$src}\n" if ($opt_verbose || $opt_dry);
	if ($files{$src} =~ m/\.gz$/) {
	  system ("gzip -qfd $files{$src}");
	  $files{$src} =~ s/\.gz$//;
	}
      }
      push @xemtexfiles, $files{$src};
    }
    else {
      my $dst = $files{$src};
      if (! -d $files{$src}) {
        $dst = &FileUtils::dirname($files{$src});
      }
      if (! -d $dst) {
        print "Creating directory $dst\n" if ($opt_verbose || $opt_dry);
        &FileUtils::rec_mkdir($dst) unless($opt_dry);
      }
      my @srcfiles = &FileUtils::globexpand(1, $src);
      $src = &FileUtils::dirname($src);
#      print "multiple src = $src srcfiles = @srcfiles\n";
      map {
	$_ =~ m@$src/(.*)$@;
	my $dstfile = "$dst/$1";
	&FileUtils::rec_mkdir(&FileUtils::dirname("$dstfile")) if (! -d &FileUtils::dirname("$dstfile"));
	if (! -f "$dstfile") {
	  &FileUtils::copy($_, "$dstfile") unless($opt_dry);
          print "Copying $_ to $dstfile\n" if ($opt_verbose || $opt_dry);
	  if ($dstfile =~ m/\.gz$/) {
	    system ("gzip -qfd $dstfile");
	    $dstfile =~ s/\.gz$//;
	  }
	}
	elsif (
	       &FileUtils::newer($_, "$dstfile") > 0
	      ) {
	  &FileUtils::copy($_, "$dstfile") unless($opt_dry);
          print "Copying $_ to $dstfile\n" if ($opt_verbose || $opt_dry);
	  if ($dstfile =~ m/\.gz$/) {
	    system ("gzip -qfd $dstfile");
	    $dstfile =~ s/\.gz$//;
	  }
	}
	push @xemtexfiles, "$dstfile";
      } @srcfiles;
    }
  }
  $root = &FileUtils::build_tree(@xemtexfiles);
  &FileUtils::walk_dir($destdir, \&check_files);
}
