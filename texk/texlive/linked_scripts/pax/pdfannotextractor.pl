#!/usr/bin/env perl
use strict;
$^W=1;

# Copyright (C) 2008, 2011, 2012 Heiko Oberdiek
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA
#
# This file is part of PDFAnnotExtractor. See README.

my $name        = 'PDFAnnotExtractor';
my $program     = "\L$name\E";
my $file        = "$program.pl";
my $version     = "0.1l";
my $date        = "2012/04/18";
my $author      = "Heiko Oberdiek";
my $copyright   = "Copyright (c) 2008, 2011, 2012 by $author.";

# History:
#  2008/10/01 v0.1i: First version of the wrapper script.
#  2012/04/18 v0.1l: Option --version added.

my $title = "$name $version, $date - $copyright\n";
my $usage = <<"END_OF_USAGE";
${title}Syntax:   $program [options] <PDF files[.pdf]>
Options:
  --help      print usage
  --version   print version number
  --debug     debug informations
END_OF_USAGE

my $help = 0;
my $debug = 0;
my $opt_version = 0;
use Getopt::Long;
GetOptions(
  'debug!' => \$debug,
  'help!' => \$help,
  'version!' => \$opt_version,
) or die $usage;
!$help or die $usage;
if ($opt_version) {
  print "$name $date v$version\n";
  exit(0);
}
(@ARGV >= 1 or die $usage);

print $title;

my $error = '!!! Error:';
my $pdfbox = 'PDFBox';
my $prg_kpsewhich = 'kpsewhich';
my $prg_java      = 'java';
my %prg;

my $jar_pax    = 'pax.jar';
my $main_class = 'pax.PDFAnnotExtractor';
my $jar_pdfbox = 'pdfbox.jar';
my @jar_pdfbox = qw[
    pdfbox.jar
    PDFBox.jar
    pdfbox-0.7.3.jar
    PDFBox-0.7.3.jar
    pdfbox-0.7.2.jar
    PDFBox-0.7.2.jar
];
my @dir_jar = qw[
    /usr/share/java
    /usr/local/share/java
];
my $path_jar_pax = '';
my $path_jar_pdfbox = '';
my $classpath = defined $ENV{'CLASSPATH'} ? $ENV{'CLASSPATH'} : '';
debug('CLASSPATH', $classpath);
my $pdfbox_in_classpath = $classpath =~ /PDFBox/ ? 1 : 0;

my $is_win = 0;
$is_win = 1 if $^O =~ /^MSWin(32|64)/i
            or $^O =~ /^dos/i
            or $^O =~ /^os2/i;
debug('is_win', $is_win);

use File::Which;

sub debug ($$) {
    my $key = shift;
    my $value = shift;
    print "* $key: [$value]\n" if $debug;
}

sub check_prg ($$) {
    my $prg = shift;
    my $die = shift;
    return 1 if $prg{$prg};
    my $path = which($prg);
    if ($path) {
        $prg{$prg} = $path;
        debug "Which $prg", $path;
        return 1;
    }
    debug "Which $prg", '<not found>';
    if ($die) {
        die "$error Cannot find program `$prg'!\n";
    }
    return 0;
}

sub find_jar ($) {
    my $jar_name = shift;

    check_prg $prg_kpsewhich, 1;
    my $cmd = "kpsewhich"
            . " --progname $program"
            . " --format texmfscripts"
            . " $jar_name";
    debug 'Backticks',  $cmd;
    my $path = `$cmd`;
    if ($? == 0) {
        chomp $path;
        debug 'Exit code', '0/success';
        debug $jar_name, $path;
        return $path;
    }
    if ($? == -1) {
        die "!!! Error: Cannot execute `$prg_kpsewhich' ($!)!\n";
    }
    if ($? & 127) {
        die "!!! Error: `$prg_kpsewhich' died with signal " . ($? & 127)
            . (($? & 128) ? ' with coredump' : '') . "!\n";
    }
    debug 'Exit code', ($? >> 8);
    return '';
}

sub find_jar_pax () {
    return if $path_jar_pax;
    foreach my $dir (@dir_jar) {
        my $path = "$dir/$jar_pax";
        if (-f $path) {
            $path_jar_pax = $path;
            debug $jar_pax, $path_jar_pax;
            return;
        }
    }
    $path_jar_pax = find_jar $jar_pax;
    if (!$path_jar_pax) {
        die "$error Cannot find `$jar_pax'!\n";
    }
}


sub launch_pax () {
    check_prg $prg_java, 1;
    my @cmd = ($prg_java);
    push @cmd, '-cp';
    push @cmd, $path_jar_pax;
    push @cmd, 'pax.PDFAnnotExtractor';
    push @cmd, @ARGV;
    debug 'System', "@cmd";
    system @cmd;
    if ($? == 0) {
        debug 'Result', 'ok';
        return 0;
    }
    if ($? == -1) {
        die "$error Cannot execute `$prg_java' ($!)!\n";
    }
    if ($? & 127) {
        die "$error `$prg_java' died with signal " . ($? & 127)
            . (($? & 128) ? ' with coredump' : '') . "!\n";
    }
    my $exit_code = $? >> 8;
    debug 'Exit code', $exit_code;
    return $exit_code;
}

# main program

my $ret = 0;
find_jar_pax;
exit launch_pax;

__END__
