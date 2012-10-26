#! /usr/bin/perl

# script to create ShapingTypeData.cpp from UCD file DerivedJoiningType.txt
# written by JK, 2007-01-16
# This script may be distributed and used freely

%types = (
	'C' => 1,
	'D' => 2,
	'R' => 4,
	'T' => 5
);

@ranges = ();

sub addRange
{
	my ($s, $e, $t) = @_;
	
	if ((scalar @ranges > 0) 
		&& ($ranges[$#ranges]->[2] == $t)
		&& ($ranges[$#ranges]->[1] == $s - 1)) {
		$ranges[$#ranges]->[1] = $e;
		return;
	}
	
	push @ranges, [ $s, $e, $t ];
}

# read the DerivedJoiningType.txt file and remember all the ranges
while (<>) {
	if (m/^([0-9A-F]{4})\s+; ([CDRT]) \#/) {
		# single codepoint
		$start = hex $1;
		$end = $start;
		$type = $types{$2};
		addRange($start, $end, $type);
		next;
	}
	if (m/^([0-9A-F]{4})\.\.([0-9A-F]{4})\s+; ([CDRT]) \#/) {
		# range of codes
		$start = hex $1;
		$end = hex $2;
		$type = $types{$3};
		addRange($start, $end, $type);
		next;
	}
}

# write the ShapingTypeData.cpp file

$date = `date +"%F %T %Z"`;
print <<__EOT__;
/*
 *
 * (C) Copyright SIL International. 2007-2008.
 * (C) Copyright IBM Corp. 1998-2005.
 * Based on code distributed with ICU 3.6
 * and the Unicode Character Database, version 5.1.0d10
 *
 * WARNING: THIS FILE IS MACHINE GENERATED. DO NOT HAND EDIT IT UNLESS
 * YOU REALLY KNOW WHAT YOU'RE DOING.
 *
 * Generated on: $date
 */

#include "LETypes.h"
#include "ArabicShaping.h"

U_NAMESPACE_BEGIN

const le_uint8 ArabicShaping::shapingTypeTable[] = {
__EOT__

printf "    0x%02X, 0x%02X, /* classFormat */\n", 0, 2;
printf "    0x%02X, 0x%02X, /* classRangeCount */\n",
	(scalar @ranges) / 256, (scalar @ranges) % 256;
print join(",\n", map {
		sprintf "    0x%02X, 0x%02X,  0x%02X, 0x%02X,  0x%02X, 0x%02X",
			$_->[0] / 256, $_->[0] % 256,
			$_->[1] / 256, $_->[1] % 256,
			$_->[2] / 256, $_->[2] % 256, 
	} sort { $a->[0] <=> $b->[0] } @ranges);

print <<__EOT__;

};

U_NAMESPACE_END
__EOT__
