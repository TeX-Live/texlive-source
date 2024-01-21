#!/usr/bin/env perl

# This file is a part of Memoize, a TeX package for externalization of
# graphics and memoization of compilation results in general, available at
# https://ctan.org/pkg/memoize and https://github.com/sasozivanovic/memoize.
#
# Copyright (c) 2020- Saso Zivanovic <saso.zivanovic@guest.arnes.si>
#
# This work may be distributed and/or modified under the conditions of the
# LaTeX Project Public License, either version 1.3c of this license or (at
# your option) any later version.  The latest version of this license is in
# https://www.latex-project.org/lppl.txt and version 1.3c or later is part of
# all distributions of LaTeX version 2008 or later.
#
# This work has the LPPL maintenance status `maintained'.
# The Current Maintainer of this work is Saso Zivanovic.
# 
# The files belonging to this work and covered by LPPL are listed in
# <texmf>/doc/generic/memoize/FILES.

my $PROG = 'memoize-clean.pl';
my $VERSION = '2024/01/21 v1.1.2';

use strict;
use Getopt::Long;
use Cwd 'realpath';
use File::Spec;
use File::Basename;

my $usage = "usage: $PROG [-h] [--yes] [--all] [--quiet] [--prefix PREFIX] " .
            "[mmz ...]\n";
my $Help = <<END;
Remove (stale) memo and extern files produced by package Memoize.

positional arguments:
  mmz                   .mmz record files

options:
  -h, --help            show this help message and exit
  --version, -V         show version and exit
  --yes, -y             Do not ask for confirmation.
  --all, -a             Remove *all* memos and externs.
  --quiet, -q
  --prefix PREFIX, -p PREFIX
                        A path prefix to clean;
                        this option can be specified multiple times.

For details, see the man page or the Memoize documentation.
END

my ($yes, $all, @prefixes, $quiet, $help, $print_version);
GetOptions(
    "yes|y"   => \$yes,
    "all|a"   => \$all,
    "prefix|p=s" => \@prefixes,
    "quiet|q|?" => \$quiet,
    "help|h|?" => \$help,
    "version|V"  => \$print_version,
    ) or die $usage;
$help and die "$usage\n$Help";
if ($print_version) { print("memoize-clean.pl of Memoize $VERSION\n"); exit 0 }

my (%keep, %prefixes);

my $curdir = Cwd::getcwd();

for my $prefix (@prefixes) {
    $prefixes{Cwd::realpath(File::Spec->catfile(($curdir), $prefix))} = '';
}

my @mmzs = @ARGV;

for my $mmz (@mmzs) {
    my ($mmz_filename, $mmz_dir) = File::Basename::fileparse($mmz);
    @ARGV = ($mmz);
    my $endinput = 0;
    my $empty = -1;
    my $prefix = "";
    while (<>) {
	if (/^ *$/) {
	} elsif ($endinput) {
	    die "Bailing out, \\endinput is not the last line of file $mmz.\n";
	} elsif (/^ *\\mmzPrefix *{(.*?)}/) {
	    $prefix = $1;
	    $prefixes{Cwd::realpath(File::Spec->catfile(($curdir,$mmz_dir), $prefix))} = '';
	    $empty = 1 if $empty == -1;
	} elsif (/^%? *\\mmz(?:New|Used)(?:CC?Memo|Extern) *{(.*?)}/) {
	    my $fn = $1;
	    if ($prefix eq '') {
		die "Bailing out, no prefix announced before file $fn.\n";
	    }
	    $keep{Cwd::realpath(File::Spec->catfile(($mmz_dir), $fn))} = 1;
	    $empty = 0;
	    if (rindex($fn, $prefix, 0) != 0) {
		die "Bailing out, prefix of file $fn does not match " .
		    "the last announced prefix ($prefix).\n";
	    }
	} elsif (/^ *\\endinput *$/) {
	    $endinput = 1;
	} else {
	    die "Bailing out, file $mmz contains an unrecognized line: $_\n";
	}
    }
    die "Bailing out, file $mmz is empty.\n" if $empty && !$all;
    die "Bailing out, file $mmz does not end with \\endinput; this could mean that " .
	"the compilation did not finish properly. You can only clean with --all.\n"
	if $endinput == 0 && !$all;
}

my @tbdeleted;
sub populate_tbdeleted {
    my ($basename_prefix, $dir, $suffix_dummy) = @_;
    opendir(MD, $dir) or die "Cannot open directory '$dir'";
    while( (my $fn = readdir(MD)) ) {
	my $path = File::Spec->catfile(($dir),$fn);
	if ($fn =~
	    /\Q$basename_prefix\E[0-9A-F]{32}(?:-[0-9A-F]{32})?(?:-[0-9]+)?#
	     (\.memo|(?:-[0-9]+)?\.pdf|\.log)/x
	    and ($all || !exists($keep{$path}))) {
	      push @tbdeleted, $path;
	}
    }
    closedir(MD);
}
for my $prefix (keys %prefixes) {
    my ($basename_prefix, $dir, $suffix);
    if (-d $prefix) {
	populate_tbdeleted('', $prefix, '');
    }
    populate_tbdeleted(File::Basename::fileparse($prefix));
}
@tbdeleted = sort(@tbdeleted);

my @allowed_dirs = ($curdir);
my @deletion_not_allowed;
for my $f (@tbdeleted) {
    my $f_allowed = 0;
    for my $dir (@allowed_dirs) {
	if ($f =~ /^\Q$dir\E/) {
	    $f_allowed = 1;
	    last;
	}
    }
    push(@deletion_not_allowed, $f) if ! $f_allowed;
}
die "Bailing out, I was asked to delete these files outside the current directory:\n" .
    join("\n", @deletion_not_allowed) if (@deletion_not_allowed);

if (scalar(@tbdeleted) != 0) {
    my $a;
    unless ($yes) {
	print("I will delete the following files:\n" .
	      join("\n",@tbdeleted) . "\n" .
	      "Proceed (y/n)? ");
	$a = lc(<>);
	chomp $a;
    }
    if ($yes || $a eq 'y' || $a eq 'yes') {
	foreach my $fn (@tbdeleted) {
	    print "Deleting ", $fn, "\n" unless $quiet;
	    unlink $fn;
	}
    } else {
	die "Bailing out.\n";
    }
} elsif (!$quiet) {
    print "Nothing to do, the directory seems clean.\n";
}

# Local Variables:
# after-save-hook: pl2dtx
# End:
