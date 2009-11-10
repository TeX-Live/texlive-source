#!/usr/bin/perl

# mk_agl_standard_nam_c.pl -- convert STANDARD.NAM and glyphlist.txt
# to C source for xdvipdfmx;
# this allows us to have the standard list compiled into the binary,
# and so the user doesn't have to install a glyphlist file separately.

# usage:
#
#   mk_agl_standard_nam_c.pl STANDARD.NAM glyphlist.txt > agl_standard.c
#
# where STANDARD.NAM is the glyph name list in FontLab's format,
# and glyphlist.txt is the Adobe glyph name list as found at
# http://partners.adobe.com/public/developer/en/opentype/glyphlist.txt
#
# xdvipdfmx includes data derived from STANDARD.NAM
# by kind permission of Adam Twardoch.

print << "__EOT__";
#include "agl.h"

const char* agl_standard_names[] = {
__EOT__

my %knownNames;
while (<ARGV>) {
	s/ *[\r\n]+$//;
	s/[\#%].*//;
	next if $_ eq '';
	if (m/^0x[0-9A-Fa-f]{4}/) {
		my ($unicode, $name) = split(/ +/);
		next unless defined $name and defined $unicode;
		$name =~ s/^!//;
		next if exists $knownNames{$name};
		$knownNames{$name} = 1;
		$unicode =~ s/^0x//;
		print "  \"$name;$unicode\",\n";
		next;
	}
	if (m/^.+;[0-9A-Fa-f]{4}/) {
		my ($name, $unicodes) = split(/;/);
		next unless defined $name and defined $unicodes;
		next if exists $knownNames{$name};
		print "  \"$name;$unicodes\",\n";
	}
}

print << "__EOT__";
  "" /* terminator */
};
__EOT__
