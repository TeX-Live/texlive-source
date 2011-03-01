eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;
use strict;
$^W=1; # turn warning on
#
# pdfatfi.pl
#
# Copyright (C) 2005, 2006 Heiko Oberdiek.
#
# This work may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either version 1.3
# of this license or (at your option) any later version.
# The latest version of this license is in
#   http://www.latex-project.org/lppl.txt
# and version 1.3 or later is part of all distributions of LaTeX
# version 2005/12/01 or later.
#
# This work has the LPPL maintenance status "maintained".
#
# This Current Maintainer of this work is Heiko Oberdiek.
#
# See file "attachfile2.pdf" for a list of files that belong to this project.
#
# This file "pdfatfi.pl" may be renamed to "pdfatfi"
# for installation purposes.
#
my $file        = "pdfatfi.pl";
my $program     = uc($&) if $file =~ /^\w+/;
my $version     = "2.5";
my $date        = "2009/09/25";
my $author      = "Heiko Oberdiek";
my $copyright   = "Copyright (c) 2005, 2006 by $author.";
#
# History:
#   2005/05/21 v1.0: First release.
#   2006/08/16 v2.2: Included in DTX file of attachfile2.dtx.
#

use POSIX qw(strftime); # %z is used (GNU)
use Digest::MD5;

### program identification
my $title = "$program $version, $date - $copyright\n";

### error strings
my $Error = "!!! Error:"; # error prefix

### variables
my $atfifile;

### option variables
my @bool = ("false", "true");
$::opt_help       = 0;
$::opt_quiet      = 0;
$::opt_debug      = 0;
$::opt_verbose    = 0;

my $usage = <<"END_OF_USAGE";
${title}Syntax:   \L$program\E [options] <file[.atfi]>
Function: Help program for LaTeX package "attachfile2".
Options:                                          (defaults:)
  --help          print usage
  --(no)quiet     suppress messages               ($bool[$::opt_quiet])
  --(no)verbose   verbose printing                ($bool[$::opt_verbose])
  --(no)debug     debug informations              ($bool[$::opt_debug])
END_OF_USAGE

### process options
my @OrgArgv = @ARGV;
use Getopt::Long;
GetOptions(
  "help!",
  "quiet!",
  "debug!",
  "verbose!",
) or die $usage;
!$::opt_help or die $usage;
@ARGV == 1 or die "$usage$Error Missing jobname!\n";

$::opt_quiet = 0 if $::opt_verbose;

print $title unless $::opt_quiet;

### get jobname
$atfifile = $ARGV[0];
if (!-f $atfifile && -f "$atfifile.atfi") {
    $atfifile .= ".atfi";
}
-f $atfifile or die "$Error File `$atfifile' not found!\n";

print "* job file     = $atfifile\n" if $::opt_verbose;

if ($::opt_debug) {
  print <<"END_DEB";
* OSNAME: $^O
* PERL_VERSION: $]
* ARGV: @OrgArgv
END_DEB
}

my $tmpfile = $atfifile . ".tmp";

my $timezone = strftime "%z", localtime;
$timezone =~ s/^([+\-]\d\d)(\d\d)$/$1'$2'/;

open(IN, $atfifile) or die "$Error Cannot open `$atfifile'!\n";
open(OUT, ">$tmpfile") or die "$Error Cannot write `$tmpfile'!\n";

while(<IN>) {
    # timezone
    if (s/^(\\attachfile\@timezone\{).*(\})$/$1$timezone$2/) {
        print "* timezone     = $timezone\n" if $::opt_verbose;
    }

    # file entry
    if (/^\\attachfile\@file\[[^\]]*\]\{(.*)\}$/) {
        my $hexfile = $1;
        my $file = pack('H*', $hexfile);
        my @s = stat($file);
        if (@s == 0) {
            print "!!! Warning: File `$file' not found!\n";
        }
        else {
            my $size = @s[7];
            my $mtime = @s[9];
            my $ctime = @s[10]; # inode change time

            my ($sec, $min, $hour, $mday, $mon, $year) = localtime($mtime);
            my $moddate = sprintf("%04d%02d%02d%02d%02d%02d",
                                  $year + 1900, $mon + 1, $mday,
                                  $hour, $min, $sec);

            # Manual page "perlport" says that "ctime" is creation
            # time instead of inode change time for "Win32" and
            # "Mac OS", but it is unsupported for "Mac OS X".
            my $creationdate = "";
            if ($^O eq 'MSWin32') { # cygwin?
                ($sec, $min, $hour, $mday, $mon, $year) = localtime($ctime);
                $creationdate = sprintf("%04d%02d%02d%02d%02d%02d",
                                        $year + 1900, $mon + 1, $mday,
                                        $hour, $min, $sec);
            }

            # md5 checksum
            my $checksum = "";
            my $ctx = Digest::MD5->new;
            if (open(FILE, $file)) {
                $ctx->addfile(*FILE);
                $checksum = $ctx->hexdigest;
                close(FILE);
            }
            else {
                print "!!! Warning: File `$file' cannot be read,"
                      . " dropping checksum!\n";
            }

            $_ = "\\attachfile\@file["
                 . "ModDate=$moddate,Size=$size"
                 . (($checksum) ? ",CheckSum=$checksum" : "")
                 . (($creationdate) ? ",CreationDate=$creationdate" : "")
                 . "]{$hexfile}\n";
            if ($::opt_verbose) {
                print "* file entry   = $file\n";
                print "  size         = $size\n";
                print "  moddate      = $moddate\n";
                print "  creationdate = $creationdate\n" if $creationdate;
                print "  checksum     = $checksum\n" if $checksum;
            }
        }
    }

    print OUT $_;
}

close(IN);
close(OUT);

unlink($atfifile) or die "$Error Cannot delete old `$atfifile'!\n";
rename $tmpfile, $atfifile
        or die "$Error Cannot move `$tmpfile' to `$atfifile'!\n";

print "*** ready. ***\n" unless $::opt_quiet;

__END__

