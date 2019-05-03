#!/usr/bin/env perl

use strict;
use warnings;

my $isWindows = $^O =~ /^MSWin/i;

my $sep  = $isWindows ? '\\' : '/';
my $diff = $isWindows ? "fc" : "diff";

my $srcdir = $ENV{'SRCDIR'} || "";
my $bindir = $ENV{'BINDIR'} || "../bin";

if ($srcdir && substr($srcdir, -1) ne $sep) { $srcdir .= ${sep}; }
if ($bindir && substr($bindir, -1) ne $sep) { $bindir .= ${sep}; }

my $status = 0; # OK

sub dotest {
	my ($description, $command) = @_;
	print "$description...";
	my $errs = `$command 2>&1`;
	my $output = "ok\n";
	if ($? != 0) {
		$status = 1;
		$output = "failed: $errs";
	}
	print " $output";
}

sub compare {
	my ($file1, $file2) = @_;
	dotest("comparing", "$diff $file1 $file2");
}

dotest("compiling Greek mapping (uncompressed)",
	"${bindir}teckit_compile ${srcdir}SILGreek2004-04-27.map -z -o SILGreek.uncompressed.tec");

compare("${srcdir}SILGreek2004-04-27.uncompressed.tec.orig", "SILGreek.uncompressed.tec");

dotest("compiling Greek mapping (compressed)",
	"${bindir}teckit_compile ${srcdir}SILGreek2004-04-27.map -o SILGreek.tec");

# Don't bother to check the compressed file as changes to the compression library can
# slightly change the resulting image. The tests below which use the compressed file
# should be sufficient to verify that it can be decompressed/used properly

dotest("converting plain-text file to unicode",
	"${bindir}txtconv -t SILGreek.tec -i ${srcdir}mrk.txt -o mrk.utf8.txt -nfc");

dotest("converting back to legacy encoding",
	"${bindir}txtconv -t SILGreek.tec -r -i mrk.utf8.txt -o mrk.bytes.txt");

compare("${srcdir}mrk.txt", "mrk.bytes.txt");

dotest("converting unicode to utf16 and nfd",
	"${bindir}txtconv -i mrk.utf8.txt -o mrk.utf16be.txt -of utf16be -nfd");

dotest("converting back to utf8 and nfc",
	"${bindir}txtconv -i mrk.utf16be.txt -o mrk.utf8b.txt -of utf8 -nfc");

compare("mrk.utf8.txt", "mrk.utf8b.txt");

dotest("compiling ISO-8859-1 mapping for sfconv test",
	"${bindir}teckit_compile ${srcdir}ISO-8859-1.map -o ISO-8859-1.tec");

dotest("converting standard format file to unicode",
	"${bindir}sfconv -8u -c ${srcdir}GNT-map.xml -i ${srcdir}Mrk-GNT.sf -o mrk.sf.utf8.txt -utf8 -bom");

dotest("converting back to legacy encodings",
	"${bindir}sfconv -u8 -c ${srcdir}GNT-map.xml -i mrk.sf.utf8.txt -o mrk.sf.legacy.txt");

compare("${srcdir}mrk.sf.legacy.txt.orig", "mrk.sf.legacy.txt");


print "preparing normalization tests...\n";
my @col;
open(FH, "< ${srcdir}NormalizationTest.txt") or die "can't open NormalizationTest.txt";
while (<FH>) {
	s/\#.*//;
	my @cols = split(/;/);
	if (defined $cols[4]) {
		foreach (1..5) {
			$col[$_] .= pack('U*', map { hex "0x$_" } split(/ /,$cols[$_ - 1])) . "\n";
		}
	}
}
close(FH);
foreach (1..5) {
	open(FH, ">:utf8", "NormCol$_.txt") or die "can't write to NormCol$_.txt";
	print FH $col[$_];
	system("${bindir}txtconv -i NormCol$_.txt -o NormCol$_.NFC.txt -of utf8 -nfc -nobom");
	system("${bindir}txtconv -i NormCol$_.txt -o NormCol$_.NFD.txt -of utf8 -nfd -nobom");
	close FH;
}
foreach ("2,1.NFC", "2,2.NFC", "2,3.NFC", "4,4.NFC", "4,5.NFC",
               "3,1.NFD", "3,2.NFD", "3,3.NFD", "5,4.NFD", "5,5.NFD") {
	my @pair = split(/,/, $_);
	compare("NormCol$pair[0].txt", "NormCol$pair[1].txt");
}
print "done\n";

if (1) {
	print "removing working files...";
	unlink("SILGreek.uncompressed.tec");
	unlink("SILGreek.tec");
	unlink("ISO-8859-1.tec");
	unlink("mrk.utf8.txt");
	unlink("mrk.utf8b.txt");
	unlink("mrk.utf16be.txt");
	unlink("mrk.bytes.txt");
	unlink("mrk.sf.utf8.txt");
	unlink("mrk.sf.legacy.txt");
	while (<NormCol*.txt>) {
		unlink($_);
	}
	print " done\n";
}

exit $status;
