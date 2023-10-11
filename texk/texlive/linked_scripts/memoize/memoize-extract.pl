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

use strict;
use Getopt::Long;
use Path::Class;
use File::Spec;
use File::Basename;
use PDF::API2;

my $VERSION = '2023/10/10 v1.0.0';

my $usage = "usage: memoize-extract.pl [-h] [--pdf PDF] [--prune] [--keep] [--force] [--log LOG] [--warning-template WARNING_TEMPLATE] [--quiet] [--mkdir] mmz\n";
my $Help = <<END;
Extract extern pages out of the document PDF.

positional arguments:
  mmz                   the record file produced by Memoize: doc.mmz when compiling doc.tex

options:
  --help, -h            show this help message and exit
  --version, -V         show version and exit
  --pdf PDF, -P PDF     extract from file PDF
  --prune, -p           remove the extern pages after extraction
  --keep, -k            do not mark externs as extracted
  --force, -f           extract even if the size-check fails
  --log LOG, -l LOG     the log file
  --warning-template WARNING_TEMPLATE, -w WARNING_TEMPLATE
                        \warningtext in the template will be replaced by the warning message
  --quiet, -q           don't describe what's happening
  --embedded, -e        prefix all messages to the standard output with the script name
  --mkdir, -m           create a directory (and exit)

For details, see the man page or the Memoize documentation.
END

my ($pdf_arg, $prune, $keep, $log, $quiet, $embedded, $force, $mkdir, $help, $print_version);
my $warning_template = '\warningtext';
Getopt::Long::Configure ("bundling");
GetOptions(
    "pdf|P=s"   => \$pdf_arg,
    "keep|k"    => \$keep,
    "prune|p"   => \$prune,
    "log|l=s"   => \$log,
    "quiet|q" => \$quiet,
    "embedded|e" => \$embedded,
    "force|f" => \$force,
    "warning-template|w=s" => \$warning_template,
    "mkdir|m"   => \$mkdir,
    "help|h|?"  => \$help,
    "version|V"  => \$print_version,
    ) or die $usage;
if ($help) {print("$usage\n$Help"); exit 0}
if ($print_version) { print("memoize-extract.pl of Memoize $VERSION\n"); exit 0 }
die $usage unless @ARGV == 1;

my $message_prefix = $embedded ? basename($0) . ': ' : '';
print("\n") if ($embedded);

my @output_paths = (dir()->absolute->resolve);
my $texmfoutput = `kpsewhich --var-value=TEXMFOUTPUT`;
$texmfoutput =~ s/^\s+|\s+$//g;
if ($texmfoutput) {
    my $texmfoutput_dir = dir($texmfoutput)->absolute->resolve;
    push(@output_paths, $texmfoutput_dir) unless $texmfoutput_dir->dir_list == 1 && ! $texmfoutput_dir->volume;
}

sub paranoia {
    my $file = $_[0];
    die "${message_prefix}Cannot create a hidden file or dir: $file"
	if $file->basename =~ /^\./;
    my $parent = $file->parent->absolute->resolve;
    die "${message_prefix}Cannot write outside the current working or output directory tree: $file"
	unless grep($_->contains($parent), @output_paths);
}

my $mmz_arg = $ARGV[0];
my $mmz_file = file($mmz_arg);
my $mmz_dir = $mmz_file->parent;

if ($mkdir) {
    my $dir = dir($mmz_arg)->cleanup;
    my $current = dir(File::Spec->catpath($dir->volume,
					  $dir->is_absolute ? File::Spec->rootdir : File::Spec->curdir,
					  ''))->absolute;
    for my $c ($dir->components) {
	$current = $current->subdir($c);
	if (-d $current) {
	    $current = $current->resolve;
	} else {
	    paranoia($current);
	    mkdir($current);
	    print("${message_prefix}Created directory $current\n") unless $quiet;
	}
    }
    exit;
} else {
    die "${message_prefix}The 'mmz' argument should be a file with suffix '.mmz', not '$mmz_file'\n" unless $mmz_file->basename =~ /\.mmz$/;
}

# Enable in-place editing (of the .mmz file).
paranoia($mmz_file) unless $keep;
$^I = "" unless $keep;

my $pdf_file = $pdf_arg ? file($pdf_arg) : $mmz_dir->file($mmz_file->basename =~ s/\.mmz$/\.pdf/r)->cleanup;
paranoia($pdf_file) if $prune;

if ($log) {
    paranoia(file($log));
    open LOG, ">$log";
} else {
    *LOG = *STDERR;
}

my ($pdf, %extern_pages);
print "${message_prefix}Extracting externs from $pdf_file:\n" unless $quiet;

my $tolerance = 0.01;
my $done_message = "${message_prefix}Done (there was nothing to extract).\n";
    
while (<>) {
    if (/^\\mmzNewExtern *{(?P<extern_path>(?P<prefix>.*?)(?P<code_md5sum>[0-9A-F]{32})-(?P<context_md5sum>[0-9A-F]{32})(?:-[0-9]+)?.pdf)}{(?P<page_n>[0-9]+)}{(?P<expected_width>[0-9.]*)pt}{(?P<expected_height>[0-9.]*)pt}/) {
	my $extern_file = file($+{extern_path});
	if (! $extern_file->is_absolute) {
	    $extern_file = $mmz_dir->file($+{extern_path});
	}
	paranoia($extern_file);
	my $page = $+{page_n};
	my $expected_width_pt = $+{expected_width};
	my $expected_height_pt = $+{expected_height};
	my $c_memo_file = $mmz_dir->file($+{prefix} . $+{code_md5sum} . '.memo');
	my $cc_memo_file = $mmz_dir->file($+{prefix} . $+{code_md5sum} . '-' . $+{context_md5sum} . '.memo');
	if (!(-e $c_memo_file && -e $cc_memo_file)) {
	    print LOG ($warning_template =~ s/\\warningtext/Not extracting page $page into extern $extern_file, because the associated (c)c-memo does not exist/gr), "\n\\endinput\n";
	    last;
	}
	eval { $pdf = PDF::API2->open($pdf_file->stringify) unless $pdf; };
	if ($@) {
	    print LOG ($warning_template =~ s/\\warningtext/Cannot read file "$pdf_file". Perhaps you have to load Memoize earlier in the preamble?/gr), "\n\\endinput\n";
	    close LOG;
	    die "${message_prefix}File '$pdf_file' cannot be read, bailing out.\n";
	}
	my $extern = PDF::API2->new();
	$extern->version($pdf->version);
	$extern->import_page($pdf, $page);
	my $extern_page = $extern->open_page(1);
	my ($x0, $y0, $x1, $y1) = $extern_page->get_mediabox();
	my $width_pt = ($x1 - $x0) / 72 * 72.27;
	my $height_pt = ($y1 - $y0) / 72 * 72.27;
	my $warning = '';
	if (abs($width_pt - $expected_width_pt) > $tolerance
	    || abs($height_pt - $expected_height_pt) > $tolerance)
	{
	    $warning = "I refuse to extract page $page from $pdf_file, because its size (${width_pt}pt x ${height_pt}pt) is not what I expected (${expected_width_pt}pt x ${expected_height_pt}pt)";
	    print LOG ($warning_template =~ s/\\warningtext/$warning/gr), "\n";
	}
	if ($warning && !$force) {
	    unlink $extern_file;
	} else {
	    $extern->saveas($extern_file->stringify);
	    $done_message = "${message_prefix}Done.\n";
	    print STDOUT "${message_prefix}  Page $page --> $extern_file\n" unless $quiet;
	    $extern_pages{$page} = 1 if $prune;
	    print("%") unless $keep;
	}
    }
    print unless $keep;
}

print $done_message unless $quiet;

if ($pdf and $prune) {
    paranoia($pdf_file);
    my $pruned_pdf = PDF::API2->new();
    for (my $n = 1; $n <= $pdf->page_count(); $n++) {
	if (! $extern_pages{$n}) {
	    $pruned_pdf->import_page($pdf, $n);
	}
    }
    $pruned_pdf->save($pdf_file->stringify);
    print("${message_prefix}The following extern pages were pruned out of the PDF: ",
	  join(",", sort(keys(%extern_pages))) . "\n") unless $quiet;
}

if ($log) {
    print LOG "\\endinput\n";
    close LOG;
}
