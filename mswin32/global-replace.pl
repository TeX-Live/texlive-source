: #-*- Perl -*-

### global-modify --- modify the contents of a file by a Perl expression

## Copyright (C) 1999 Martin Buchholz.
## Copyright (C) 2001 Ben Wing.

## Authors: Martin Buchholz <martin@xemacs.org>, Ben Wing <ben@xemacs.org>
## Maintainer: Ben Wing <ben@xemacs.org>
## Current Version: 1.0, May 5, 2001

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with XEmacs; see the file COPYING.  If not, write to the Free
# Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

eval 'exec perl -w -S $0 ${1+"$@"}'
    if 0;

use strict;
use FileHandle;
use Carp;
use Getopt::Long;
use File::Basename;

(my $myName = $0) =~ s@.*/@@; my $usage="
Usage: $myName [--help] [--backup-dir=DIR] [--line-mode] [--hunk-mode]
       PERLEXPR FILE ...

Globally modify a file, either line by line or in one big hunk.

Typical usage is like this:

[with GNU print, GNU xargs: guaranteed to handle spaces, quotes, etc.
 in file names]

find . -name '*.[ch]' -print0 | xargs -0 $0 's/\bCONST\b/const/g'\n

[with non-GNU print, xargs]

find . -name '*.[ch]' -print | xargs $0 's/\bCONST\b/const/g'\n


The file is read in, either line by line (with --line-mode specified)
or in one big hunk (with --hunk-mode specified; it's the default), and
the Perl expression is then evalled with \$_ set to the line or hunk of
text, including the terminating newline if there is one.  It should
destructively modify the value there, storing the changed result in \$_.

Files in which any modifications are made are backed up to the directory
specified using --backup-dir, or to `backup' by default.  To disable this,
use --backup-dir= with no argument.

Hunk mode is the default because it is MUCH MUCH faster than line-by-line.
Use line-by-line only when it matters, e.g. you want to do a replacement
only once per line (the default without the `g' argument).  Conversely,
when using hunk mode, *ALWAYS* use `g'; otherwise, you will only make one
replacement in the entire file!
";

my %options = ();
$Getopt::Long::ignorecase = 0;
&GetOptions (
	     \%options,
	     'help', 'backup-dir=s', 'line-mode', 'hunk-mode',
);

while (<STDIN>) {
  chomp;
  if (-f ) {
    push @ARGV, $_;
  }
}

die $usage if $options{"help"} or @ARGV <= 1;
my $code = shift;

die $usage if grep (-d || ! -w, @ARGV);

sub SafeOpen {
  open ((my $fh = new FileHandle), $_[0]);
  binmode($fh);
  confess "Can't open $_[0]: $!" if ! defined $fh;
  return $fh;
}

sub SafeClose {
  close $_[0] or confess "Can't close $_[0]: $!";
}

sub FileContents {
  my $fh = SafeOpen ("< $_[0]");
  my $olddollarslash = $/;
  local $/ = undef;
  my $contents = <$fh>;
  $/ = $olddollarslash;
  return $contents;
}

sub WriteStringToFile {
  my $fh = SafeOpen ("> $_[0]");
  binmode $fh;
  print $fh $_[1] or confess "$_[0]: $!\n";
  SafeClose $fh;
}

foreach my $file (@ARGV) {
  my $changed_p = 0;
  my $new_contents = "";
  if ($options{"line-mode"}) {
    my $fh = SafeOpen $file;
    while (<$fh>) {
      my $save_line = $_;
      eval $code;
      $changed_p = 1 if $save_line ne $_;
      $new_contents .= $_;
    }
  } else {
    my $orig_contents = $_ = FileContents $file;
    eval $code;
    if ($_ ne $orig_contents) {
      $changed_p = 1;
      $new_contents = $_;
    }
  }

  if ($changed_p) {
    my $backdir = $options{"backup-dir"};
    $backdir = "backup" if !defined ($backdir);
    if ($backdir) {
      my ($name, $path, $suffix) = fileparse ($file, "");
      my $backfulldir = $path . $backdir;
      my $backfile = "$backfulldir/$name";
      mkdir $backfulldir, 0755 unless -d $backfulldir;
      print "modifying $file (original saved in $backfile)\n";
      rename $file, $backfile;
    }
    WriteStringToFile ($file, $new_contents);
  }
}
