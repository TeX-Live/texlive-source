#!/usr/bin/perl
#
# Convert the refences to \fullref.  Usage:
# perl fullref.pl original.tex > converted.tex
#
#
#
# Author: Boris Veytsman
#
# Version: 2.0, 2026/10/15

=pod

=head1 NAME

aom-fullref.pl - confert references to aomart fullref commands

=head1 SYNOPSIS

aom-fullref.pl INPUT > OUTPUT


=head1 DESCRIPTION

Aomart class L<https://ctan.org/pkg/aomart> uses commands like
C<\fullref{Theorem}{ref}> to generate clickable reference to constructions
like ``Theorem 7'' or ``Equation 4''.  This script changes  C<\ref> and
C<\pageref> commands into C<\fullref> and C<\pfullref> commands suitable
for use with aomart.


=head1 AUTHOR

Boris Veytsman

=head1 COPYRIGHT AND LICENSE

This work is in public domain and licensed under CC0 1.0

=cut


use strict;

# List of patterns for 'equation' words
my @eqsynonyms = ('equa[a-zA-Z]*', 'relat[a-zA-Z]*',  'item[a-zA-Z]*',
		  'condition[a-zA-Z]*',
		  'propert[a-zA-Z]*');
my @thmsynonyms = ('theorem[a-zA-Z]*', 'proposition[a-zA-Z]*',
		   'lemma[a-zA-Z]*', 'corollar[a-zA-Z]*');
my @sectsynonyms = ('(sub)*+section', '(sub)*+sections');

# First we skip preamble
while(<>) {
    print;
    last if (/\\begin\{document\}/);
}

# This is a trick to catch \ref being the first word on the line
my $prevline = "";

# Processing document
while (<>) {
    if ((/^[\s~]*\(?\\ref/) || (/^[\s~]*\\eqref/)) {
	chomp $prevline;
	$_ = "$prevline $_";
    } else {
	print $prevline;
    }
    
    # Now the meat of the substitutions
    # We add initial space, so all words have space before them.
    $_ = " $_";
    foreach my $synonym (@eqsynonyms) {
	s/(\s)($synonym)[\s~]*\(\\ref\{([^\}]+)\}\)/$1\\pfullref{$2}{$3}/gi;
	s/(\s)($synonym)[\s~]*\\eqref\{([^\}]+)\}/$1\\eqfullref{$2}{$3}/gi;
	s/(\s)($synonym)[\s~]*\\pageref\{([^\}]+)\}/$1\\fullpageref{$2}{$3}/gi;
    }
    # We uppercase theorems
    foreach my $synonym (@thmsynonyms) {
    	s/(\s)($synonym)[\s~]*\(\\ref\{([^\}]+)\}\)/$1\\pfullref{\u$2}{$3}/gi;
    	s/(\s)($synonym)[\s~]*\[\\ref\{([^\}]+)\}\]/$1\\bfullref{\u$2}{$3}/gi;
    	s/(\s)($synonym)[\s~]*\\ref\{([^\}]+)\}/$1\\fullref{\u$2}{$3}/gi;
	s/(\s)($synonym)[\s~]*\\pageref\{([^\}]+)\}/$1\\fullpageref{$2}{$3}/gi;
    }
    foreach my $synonym (@sectsynonyms) {
    	s/(\s)($synonym)[\s~]*\(\\ref\{([^\}]+)\}\)/$1\\pfullref{\u$2}{$3}/gi;
    	s/(\s)($synonym)[\s~]*\[\\ref\{([^\}]+)\}\]/$1\\bfullref{\u$2}{$3}/gi;
    	s/(\s)($synonym)[\s~]*\\ref\{([^\}]+)\}/$1\\fullref{\u$2}{$3}/gi;
	s/(\s)($synonym)[\s~]*\\pageref\{([^\}]+)\}/$1\\fullpageref{\u$2}{$3}/gi;
    }
    # Now delete the extra space
    s/^ //;
    $prevline=$_;
    if (/\\end\{document\}/) {
	print;
	last;
    }
}

# And processing whatever is left
while (<>) {
    print;
}

exit 0;
