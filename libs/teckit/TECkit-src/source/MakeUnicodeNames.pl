#!perl

# make name list for teckit compiler from UnicodeData.txt

open FH, "<UnicodeData.txt" or die;
while (<FH>) {
	chomp;
	@fields = split(/;/);
	$uc = $fields[0];
	$un = $fields[1];

	next if $un =~ /</;
	
	push @names, [ $uc, $un ];
}
close FH;

print << '__END__';
#include "Compiler.h"

CharName	gUnicodeNames[] = {
__END__

# sort the names so that we can use binary search
foreach (sort { $a->[1] cmp $b->[1] } @names) {
	print "{0x$_->[0],\"$_->[1]\"},\n";
}

print << '__END__';
{0,0}
};
__END__
