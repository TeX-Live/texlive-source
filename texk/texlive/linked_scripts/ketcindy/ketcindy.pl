#!/usr/bin/env perl
#
# KETCindy starter script
#
# (C) 2017-2018 Norbert Preining
# Licensed under the same license terms as ketpic itself, that is GPLv3+
#

use strict;
$^W = 1;

use Digest::MD5;
use File::Copy;

my $BinaryName = "Cinderella2";
my $TemplateFile = "template1basic.cdy";
my $devnull = "/dev/null";
my $prog = "ketcindy";
my $systype;
if (win32()) {
  $systype = "Windows";
  $devnull = "nul";
} else {
  $systype = `uname`;
  chomp($systype);
}
my $HOME = ($systype eq "Windows") ? $ENV{'USERPROFILE'} : $ENV{'HOME'};
my $workdir ="$HOME/ketcindy";

my $cinderella;
if ($#ARGV >= 0) {
  if ($ARGV[0] eq '-c') {
    $cinderella = ($ARGV[1] ? $ARGV[1] : "");
  }
} else {
  $cinderella = which($BinaryName);
}

if (! "$cinderella") {
  if ($systype eq 'Darwin') {
    if (-r '/Applications/Cinderella2.app/Contents/MacOS/Cinderella2') {
      $cinderella = '/Applications/Cinderella2.app/Contents/MacOS/Cinderella2';
    }
  } elsif ($systype eq 'Windows') {
    if (-f 'c:/Program Files (x86)/Cinderella/Cinderella2.exe') {
      $cinderella = 'c:/Program Files (x86)/Cinderella/Cinderella2.exe';
    }
  }
}

if (! "$cinderella") {
  die "$prog: Cannot find $BinaryName!";
}

if ( ! -x "$cinderella" ) {
  die "$prog: Program $cinderella is not executable!";
}

# find real path
my $realcind = win32() ? $cinderella : `realpath "$cinderella"`;
chomp($realcind);
my ($cinddir, $bn) = dirname_and_basename($realcind);

my $plugindir = ($systype eq 'Darwin') ? "$cinddir/../PlugIns" : "$cinddir/Plugins";

my $plugin = "$plugindir/KetCindyPlugin.jar";
my $dirheadplugin = "$plugindir/ketcindy.ini";

# find Jar
chomp(my $KetCdyJar = `kpsewhich -format=texmfscripts KetCindyPlugin.jar`);
# search for template.cdy
chomp(my $TempCdy = `kpsewhich -format=texmfscripts $TemplateFile`);
chomp(my $DirHead=`kpsewhich -format=texmfscripts ketcindy.ini`);

if (-z "$TempCdy" || -z "$KetCdyJar") {
  die "$prog: Cannot find $TemplateFile via kpsewhich, is ketpic installed?";
}


if ( ! -r "$plugin" || ! -r "$dirheadplugin" ) {
  print "$prog: Cinderella is *NOT* set up for KETCindy!\n";
  print "$prog: You need to copy\n";
  print "$prog:    $KetCdyJar\n";
  print "$prog:    $DirHead\n";
  print "$prog: into\n";
  print "$prog:    $plugindir\n";
  print "\n";
  exit(1);
}

my $myjarmd = md5digest($KetCdyJar);
my $sysjarmd = md5digest($plugin);

if ( $myjarmd ne $sysjarmd ) {
  print "$prog: The installed version of the plugin in\n";
  print "$prog:   $plugin\n";
  print "$prog: differs from the version shipped in\n";
  print "$prog:   $KetCdyJar\n";
  print "$prog: You might need to update the former one with the later one!\n";
}

# print "DEBUG workdir =$workdir=\n";
# print "DEBUG TemplateFile =$TemplateFile=\n";
mkdir($workdir);
copy($TempCdy, $workdir) or die "$prog: Copy failed: $!";

# print "Exec $cinderella $workdir/$TemplateFile\n";
if (win32()) {
  # no idea why a normal call with exec did not find the template file
  my $out = `"$cinderella" "$workdir/$TemplateFile"`;
} else {
  exec($cinderella, "$workdir/$TemplateFile");
}


sub md5digest {
  my $file = shift;
  open(FILE, $file) || die "$prog: open($file) failed: $!";
  binmode(FILE);
  my $out = Digest::MD5->new->addfile(*FILE)->hexdigest;
  close(FILE);
  return $out;
}

# taken from TeXLive::TLUtils.pm
sub win32 {
  if ($^O =~ /^MSWin/i) {
    return 1;
  } else {
    return 0;
  }
}
sub which {
  my ($prog) = @_;
  my @PATH;
  my $PATH = getenv('PATH');

  if (&win32) {
    my @PATHEXT = split (';', getenv('PATHEXT'));
    push (@PATHEXT, '');  # in case argument contains an extension
    @PATH = split (';', $PATH);
    for my $dir (@PATH) {
      for my $ext (@PATHEXT) {
        if (-f "$dir/$prog$ext") {
          return "$dir/$prog$ext";
        }
      }
    }

  } else { # not windows
    @PATH = split (':', $PATH);
    for my $dir (@PATH) {
      if (-x "$dir/$prog") {
        return "$dir/$prog";
      }
    }
  }
  return "";
}
sub dirname_and_basename {
  my $path=shift;
  my ($share, $base) = ("", "");
  if (win32()) {
    $path=~s!\\!/!g;
  }
  # do not try to make sense of paths ending with /..
  return (undef, undef) if $path =~ m!/\.\.$!;
  if ($path=~m!/!) {   # dirname("foo/bar/baz") -> "foo/bar"
    # eliminate `/.' path components
    while ($path =~ s!/\./!/!) {};
    # UNC path? => first split in $share = //xxx/yy and $path = /zzzz
    if (win32() and $path =~ m!^(//[^/]+/[^/]+)(.*)$!) {
      ($share, $path) = ($1, $2);
      if ($path =~ m!^/?$!) {
        $path = $share;
        $base = "";
      } elsif ($path =~ m!(/.*)/(.*)!) {
        $path = $share.$1;
        $base = $2;
      } else {
        $base = $path;
        $path = $share;
      }
      return ($path, $base);
    }
    # not a UNC path
    $path=~m!(.*)/(.*)!; # works because of greedy matching
    return ((($1 eq '') ? '/' : $1), $2);
  } else {             # dirname("ignore") -> "."
    return (".", $path);
  }
}
sub getenv {
  my $envvar=shift;
  my $var=$ENV{"$envvar"};
  return 0 unless (defined $var);
  if (&win32) {
    $var=~s!\\!/!g;  # change \ -> / (required by Perl)
  }
  return "$var";
}


