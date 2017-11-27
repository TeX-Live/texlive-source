#!/usr/bin/env perl

=pod

=head1 NAME

biburl2doi.pl - convert URLs pointing to doi.org to DOIs

=head1 SYNOPSIS

biburl2doi  [B<-D>] [B<-o> I<output>] I<bib_file>

=head1 OPTIONS

=over 4

=item B<-D> 

Do not delete URLs converted to DOIs


=item B<-o> I<output>

Output file.  If this option is not used, the name for the 
output file is formed by adding C<_cleaned> to the input file

=back

=head1 DESCRIPTION

The script recognizes URL fields of the kind
C<http://dx.doi.org> and their variants and converts them to DOI
fields.  


=head1 AUTHOR

Boris Veytsman

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2017  Boris Veytsman

This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
L<http://www.gnu.org/licenses/gpl.html>.  There is NO WARRANTY, to the
extent permitted by law.

=cut

use strict;
BEGIN {
    # find files relative to our installed location within TeX Live
    chomp(my $TLMaster = `kpsewhich -var-value=SELFAUTOPARENT`); # TL root
    if (length($TLMaster)) {
	unshift @INC, "$TLMaster/texmf-dist/scripts/bibtexperllibs";
    }
}
use IO::File;
use BibTeX::Parser;
use LaTeX::ToUnicode qw (convert);
use Getopt::Std;

my $USAGE="USAGE: $0 [-D] [-o output] file\n";
my $VERSION = <<END;
biburl2doi 1.0
This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html.  There is NO WARRANTY, to the
extent permitted by law.
$USAGE
END
our %opts;
getopts('Do:hV',\%opts) or die $USAGE;

if ($opts{h} || $opts{V}){
    print $VERSION;
    exit 0;
}

################################################################
# Defaults and parameters
################################################################

my $inputfile = shift;

my $outputfile = $inputfile;

$outputfile =~ s/\.([^\.]*)$/_cleaned.$1/;

if (exists $opts{o}) {
    $outputfile = $opts{o};
}

my $deleteUrl = 1;
if (exists $opts{D}) {
    $deleteUrl = 0;
}		


my $input= IO::File->new($inputfile) or 
    die "Cannot find BibTeX file $inputfile\n$USAGE\n";
my $output = IO::File->new("> $outputfile") or 
    die "Cannot write to $outputfile\n$USAGE\n";

my $parser=new BibTeX::Parser($input);



# Processing the input
while (my $entry = $parser->next) {
    if (!$entry->parse_ok()) {
	print STDERR "Cannot understand entry: ";
	$entry->print(*STDERR);
	print STDERR "Skipping this entry\n";
	next;
    }

    if ($entry->has('doi') || !$entry->has('url')) {
	print $output $entry->raw_bibtex(), "\n\n";
	next;
    }

    if ($entry->field('url') =~ m|^http(?:s)?://(?:dx\.)?doi\.org/(.*)$|) {
	$entry->field('doi', $1);
	if ($deleteUrl) {
	    delete $entry->{'url'};
	}
	print $output $entry->to_string(), "\n\n";
    } else {
	print $output $entry->raw_bibtex(), "\n\n";
    }
}
	
    
					  
