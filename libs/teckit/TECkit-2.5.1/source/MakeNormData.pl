#!perl

# generate data tables for normalization from UnicodeData.txt and CompositionExclusions.txt

open FH, "<CompositionExclusions.txt" or die;
while (<FH>) {
	chomp;
	s/\s*#.*//;
	next if $_ eq '';
	$exclusions{hex "0x$_"} = 1;
}
close FH;

open FH, "<UnicodeData.txt" or die;
while (<FH>) {
	chomp;
	@fields = split(/;/);
	$uc = hex "0x$fields[0]";
	$cc = $fields[3];
	$dc = $fields[5];
	
	$plane = $uc >> 16;
	$page = ($uc >> 8) & 0xff;
	$ch = $uc & 0xff;

	$cc{$plane}{$page}{$ch} = $cc unless $cc == 0;
	
	next if $dc eq '';
	next if $dc =~ /</;
	
	$dc{$plane}{$page}{$ch} = [ split(/ /, $dc) ];
}
close FH;

foreach $plane (keys %dc) {
	foreach $page (keys %{$dc{$plane}}) {
		foreach $ch (keys %{$dc{$plane}{$page}}) {
			next if scalar @{$dc{$plane}{$page}{$ch}} == 1;
			next if exists $exclusions{$plane * 0x10000 + $page * 0x100 + $ch};
			$d1 = $dc{$plane}{$page}{$ch}[0];
			next if $cc{$d1/0x10000}{($d1/0x100) & 0xff}{$d1 & 0xff} != 0;
			my ($l, $r) = map { hex "0x$_" } ($dc{$plane}{$page}{$ch}[0], $dc{$plane}{$page}{$ch}[1]);
			unless (exists $lIndex{$l}) {
				$lIndex{$l} = 1 + scalar keys %lIndex;
				$a = $l >> 16;
				$b = ($l >> 8) & 0xff;
				$c = $l & 0xff;
				$li{$a}{$b}{$c} = $lIndex{$l};
			}
			unless (exists $rIndex{$r}) {
				$rIndex{$r} = 1 + scalar keys %rIndex;
				$a = $r >> 16;
				$b = ($r >> 8) & 0xff;
				$c = $r & 0xff;
				$ri{$a}{$b}{$c} = $rIndex{$r};
			}
			$cmp{$lIndex{$l}}{$rIndex{$r}} = $plane * 0x10000 + $page * 0x100 + $ch;
		}
	}
}

$planeMapCC = "\x00" x 17;
$planeMapDecomp = "\x00" x 17;
$planeMapL = "\x00" x 17;
$planeMapR = "\x00" x 17;
foreach $plane (0 .. 16) {
	$pageMapCC = "\x00" x 256;
	$pageMapDecomp = "\x00" x 256;
	$pageMapL = "\x00" x 256;
	$pageMapR = "\x00" x 256;
	foreach $page (0 .. 255) {
		$charCC = "\x00" x 256;
		@decomp = (0) x 256;
		@left = (0) x 256;
		@right = (0) x 256;
		foreach $ch (0 .. 255) {
			substr($charCC, $ch, 1) = pack('C', $cc{$plane}{$page}{$ch});
			if (exists $dc{$plane}{$page}{$ch}) {
				push @decompositions, $dc{$plane}{$page}{$ch};
				$decomp[$ch] = scalar @decompositions;
			}
			if (exists $li{$plane}{$page}{$ch}) {
				$left[$ch] = $li{$plane}{$page}{$ch};
			}
			if (exists $ri{$plane}{$page}{$ch}) {
				$right[$ch] = $ri{$plane}{$page}{$ch};
			}
		}

		unless (exists $charCCindex{$charCC}) {
			$charCCindex{$charCC} = scalar keys %charCCindex;
			$charCC[$charCCindex{$charCC}] = $charCC;
		}
		substr($pageMapCC, $page, 1) = pack('C', $charCCindex{$charCC});

		$dci = pack('n*', @decomp);
		unless (exists $decompIndex{$dci}) {
			$decompIndex{$dci} = scalar keys %decompIndex;
			$decomps[$decompIndex{$dci}] = $dci;
		}
		substr($pageMapDecomp, $page, 1) = pack('C', $decompIndex{$dci});

		$left = pack('n*', @left);
		unless (exists $leftIndex{$left}) {
			$leftIndex{$left} = scalar keys %leftIndex;
			$lefts[$leftIndex{$left}] = $left;
		}
		substr($pageMapL, $page, 1) = pack('C', $leftIndex{$left});

		$right = pack('n*', @right);
		unless (exists $rightIndex{$right}) {
			$rightIndex{$right} = scalar keys %rightIndex;
			$rights[$rightIndex{$right}] = $right;
		}
		substr($pageMapR, $page, 1) = pack('C', $rightIndex{$right});
	}
	
	unless (exists $pageMapCCIndex{$pageMapCC}) {
		$pageMapCCIndex{$pageMapCC} = scalar keys %pageMapCCIndex;
		$pageMapCC[$pageMapCCIndex{$pageMapCC}] = $pageMapCC;
	}
	substr($planeMapCC, $plane, 1) = pack('C', $pageMapCCIndex{$pageMapCC});
	
	unless (exists $pageMapDecompIndex{$pageMapDecomp}) {
		$pageMapDecompIndex{$pageMapDecomp} = scalar keys %pageMapDecompIndex;
		$pageMapDecomp[$pageMapDecompIndex{$pageMapDecomp}] = $pageMapDecomp;
	}
	substr($planeMapDecomp, $plane, 1) = pack('C', $pageMapDecompIndex{$pageMapDecomp});

	unless (exists $pageMapLIndex{$pageMapL}) {
		$pageMapLIndex{$pageMapL} = scalar keys %pageMapLIndex;
		$pageMapL[$pageMapLIndex{$pageMapL}] = $pageMapL;
	}
	substr($planeMapL, $plane, 1) = pack('C', $pageMapLIndex{$pageMapL});

	unless (exists $pageMapRIndex{$pageMapR}) {
		$pageMapRIndex{$pageMapR} = scalar keys %pageMapRIndex;
		$pageMapR[$pageMapRIndex{$pageMapR}] = $pageMapR;
	}
	substr($planeMapR, $plane, 1) = pack('C', $pageMapRIndex{$pageMapR});
}

print "const UInt8 ccPlaneMap[] = {";
print join(',', map { sprintf("%d", $_) } unpack('C*', $planeMapCC));
print "};\n\n";

print "const UInt8 ccPageMaps[][256] = {\n";
foreach (@pageMapCC) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('C*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt8 ccCharClass[][256] = {\n";
foreach (@charCC) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('C*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt8 dcPlaneMap[] = {";
print join(',', map { sprintf("%d", $_) } unpack('C*', $planeMapDecomp));
print "};\n\n";

print "const UInt8 dcPageMaps[][256] = {\n";
foreach (@pageMapDecomp) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('C*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt16 dcCharIndex[][256] = {\n";
foreach (@decomps) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('n*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt32 dcDecomposition[][2] = {\n";
print "\t{0xFFFF,0xFFFF},\n";
foreach (@decompositions) {
	print "\t{";
	if (scalar @{$_} == 2) {
		print join(',', map { "0x$_" } @{$_});
	}
	else {
		print "0x$_->[0],0xFFFF";
	}
	print "},\n";
}
print "};\n\n";

print "const UInt8 cLPlaneMap[] = {";
print join(',', map { sprintf("%d", $_) } unpack ('C*', $planeMapL));
print "};\n\n";

print "const UInt8 cRPlaneMap[] = {";
print join(',', map { sprintf("%d", $_) } unpack ('C*', $planeMapL));
print "};\n\n";

print "const UInt8 cLPageMaps[][256] = {\n";
foreach (@pageMapL) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('C*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt8 cRPageMaps[][256] = {\n";
foreach (@pageMapR) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('C*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt16 cLCharIndex[][256] = {\n";
foreach (@lefts) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('n*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt8 cRCharIndex[][256] = {\n";
foreach (@rights) {
	print "\t{";
	print join(',', map { sprintf("%d", $_) } unpack('n*', $_));
	print "},\n";
}
print "};\n\n";

print "const UInt16 cComposites[" . (1 + scalar keys %lIndex) . "][" . (1 + scalar keys %rIndex) . "] = {\n";
for $l (0 .. scalar keys %lIndex) {
	print "\t{";
	print join(',', map { sprintf("0x%04X", $cmp{$l}{$_}) } (0 .. scalar keys %rIndex));
	print "},\n";
}
print "};\n\n";
