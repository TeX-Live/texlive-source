#! /usr/bin/perl

# Convert web .pool file into C source
# Result is an array of chars (bytes) containing the strings,
# each prefixed by a length byte.
# The function initpool() loads this data into TeX's pool.

# usage: perl pool2c.pl NAME.pool
# writes output to ${NAME}_pool.c if only one arg provided

die "expected pool name as argument" unless $#ARGV >= 0;
die "too many arguments" if $#ARGV > 1;

$prog = `basename $ARGV[0] .pool`;
chomp($prog);

$outfile = ($#ARGV == 1) ? $ARGV[1] : "${prog}_pool.c";

open IN, "< $ARGV[0]" or die "can't open input $ARGV[0]";
open OUT, "> $outfile" or die "can't open output $outfile";

print OUT <<__EOT__;
/* This file is auto-generated from $ARGV[0] by pool2c.pl */
#define EXTERN extern
#include "${prog}d.h"

#include <stdio.h>

static unsigned char POOL[] =
__EOT__
$pool_len = 0;
while (<IN>) {
	chomp;
	if (m/^([0-9]{2})(.*)$/) {
		($len, $str) = ($1, $2);
		if (length($str) != $len) {
			die "*** pool string length error:\n$_\n";
		}
		$str =~ s/(["\\])/\\$1/g;
		$str =~ s/\?\?\?/\\?\\?\\?/g;
		printf OUT "\t\"\\x%02X\" \"%s\"\n", $len, $str;
		$pool_len += $len + 1;
	}
	elsif (m/^\*[0-9]+$/) {
		last;
	}
	else {
		die "*** unexpected line in pool file:\n$_\n"; 
	}
}

close IN;

print OUT <<__EOT__;
	;

#define POOL_LEN $pool_len

int initpool(int limit) {
	int g = 0;
	unsigned char *s = POOL;
	while (s < POOL + POOL_LEN) {
		int len = *s++;
		if (poolptr + len >= limit)
			return 0;
		while (len-- > 0)
			strpool[poolptr++] = *s++;
		g = makestring();
	}
	return g;
}
__EOT__

close OUT;
