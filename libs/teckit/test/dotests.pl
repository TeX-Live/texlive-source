#!/usr/bin/perl

print "compiling Greek mapping (uncompressed)...";
$errs = `../bin/teckit_compile SILGreek2004-04-27.map -z -o SILGreek.uncompressed.tec`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "comparing...";
$errs = `diff SILGreek.uncompressed.tec SILGreek2004-04-27.uncompressed.tec.orig`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "compiling Greek mapping (compressed)...";
$errs = `../bin/teckit_compile SILGreek2004-04-27.map -o SILGreek.tec`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "comparing...";
$errs = `diff SILGreek.tec SILGreek2004-04-27.tec.orig`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "converting plain-text file to unicode...";
$errs = `../bin/txtconv -t SILGreek.tec -i mrk.txt -o mrk.utf8.txt -nfc`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "converting back to legacy encoding...";
$errs = `../bin/txtconv -t SILGreek.tec -r -i mrk.utf8.txt -o mrk.bytes.txt`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "comparing...";
$errs = `diff mrk.txt mrk.bytes.txt`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "converting unicode to utf16 and nfd...";
$errs = `../bin/txtconv -i mrk.utf8.txt -o mrk.utf16be.txt -of utf16be -nfd`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "converting back to utf8 and nfc...";
$errs = `../bin/txtconv -i mrk.utf16be.txt -o mrk.utf8b.txt -of utf8 -nfc`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "comparing...";
$errs = `diff mrk.utf8.txt mrk.utf8b.txt`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "compiling ISO-8859-1 mapping for sfconv test...";
$errs = `../bin/teckit_compile ISO-8859-1.map`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "converting standard format file to unicode...";
$errs = `../bin/sfconv -8u -c GNT-map.xml -i Mrk-GNT.sf -o mrk.sf.utf8.txt -utf8 -bom`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "converting back to legacy encodings...";
$errs = `../bin/sfconv -u8 -c GNT-map.xml -i mrk.sf.utf8.txt -o mrk.sf.legacy.txt`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");

print "comparing...";
$errs = `diff mrk.sf.legacy.txt mrk.sf.legacy.txt.orig`;
print (($errs eq "" ? "ok" : "failed: $errs") . "\n");


print "preparing normalization tests...";
open(FH, "< NormalizationTest.txt") or die "can't open NormalizationTest.txt";
while (<FH>) {
	s/\#.*//;
	@cols = split(/;/);
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
	system("../bin/txtconv -i NormCol$_.txt -o NormCol$_.NFC.txt -of utf8 -nfc -nobom");
	system("../bin/txtconv -i NormCol$_.txt -o NormCol$_.NFD.txt -of utf8 -nfd -nobom");
	close FH;
}
foreach $diff ("2,1.NFC", "2,2.NFC", "2,3.NFC", "4,4.NFC", "4,5.NFC",
				"3,1.NFD", "3,2.NFD", "3,3.NFD", "5,4.NFD", "5,5.NFD") {
	split(/,/, $diff);
	$cmd = "diff NormCol$_[0].txt NormCol$_[1].txt";
	print "\n$cmd ...";
	$errs .= `$cmd`;
	if ($errs ne "") {
		print "\n", $errs;
	}
}
print "\ndone\n";

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
	system("rm NormCol*.txt");
	print "done\n";
}
