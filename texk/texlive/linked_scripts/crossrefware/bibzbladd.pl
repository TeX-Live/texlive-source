#!/usr/bin/env perl -X

=pod

=head1 NAME

bibzbladd.pl - add Zbl numbers to papers in a given bib file

=head1 SYNOPSIS

bibzbladd  [-d] [B<-f>] [B<-e> 1|0] [B<-o> I<output>]  I<bib_file>

=head1 OPTIONS

=over 4

=item B<-d>

Debug mode

=item B<-e>

If 1 (default), add an empty zblnumber if a zbl cannot be found.  This
prevents repeated searches for the same entries if you add new entries
to the file.  Calling C<-e 0> suppresses this behavior.

=item B<-f>

Force searching for Zbl numbers even if the entry already has one.

=item B<-o> I<output>

Output file.  If this option is not used, the name for the 
output file is formed by adding C<_zbl> to the input file


=back

=head1 DESCRIPTION

The script reads a BibTeX file.  It checks whether the entries have
Zbls.  If not, it tries to find the numbers from Internet sites. The
result is a BibTeX file with C<zblnumber=...> fields added.

The name of the output file is either set by the B<-o> option or 
is derived by adding the suffix C<_zbl> to the output file.

See the C<bibdoiadd> script for more details on the processing.

=head1 AUTHOR

Boris Veytsman

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2014-2026 Boris Veytsman

This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
L<http://www.gnu.org/licenses/gpl.html>.  There is NO WARRANTY, to the
extent permitted by law.

=cut

use strict;
BEGIN {
    # find files relative to our installed location within TeX Live
    chomp(my $TLMaster = `kpsewhich -var-value=TEXMFROOT`); # TL root
    if (length($TLMaster)) {
	unshift @INC, "$TLMaster/texmf-dist/scripts/bibtexperllibs";
    }
}
use IO::File;
use BibTeX::Parser;
use Getopt::Std;
use URI::Escape;
use JSON;
use LWP::Simple;
$ENV{PERL_LWP_SSL_VERIFY_HOSTNAME}=0;

my $USAGE="USAGE: $0  [-d] [-e 1|0] [-f] [-o output] file\n";
my $VERSION = <<END;
bibzbladd v3.0
This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html.  There is NO WARRANTY, to the
extent permitted by law.
$USAGE
END
my %opts;
getopts('de:fo:hV',\%opts) or die $USAGE;

if ($opts{h} || $opts{V}){
    print $VERSION;
    exit 0;
}

################################################################
# Defaults and parameters
################################################################

my $inputfile = shift;

my $outputfile = $inputfile;

$outputfile =~ s/\.([^\.]*)$/_zbl.$1/;

if ($opts{o}) {
    $outputfile = $opts{o};
}

my $forceSearch=$opts{f};

my $forceEmpty = 1;
if (exists $opts{e}) {
    $forceEmpty = $opts{e};
}		

my $debug = $opts{d};




my $input= IO::File->new($inputfile) or 
    die "Cannot find BibTeX file $inputfile\n$USAGE\n";
my $output = IO::File->new("> $outputfile") or 
    die "Cannot write to $outputfile\n$USAGE\n";

my $parser=new BibTeX::Parser($input);



# Creating the HTTP parameters
my $mirror =
    "https://api.zbmath.org/v1/document/_search";

while (my $entry = $parser->next ) {
    if (!$entry->parse_ok()) {
	print STDERR "Cannot understand entry: ";
	$entry->print(*STDERR);
	print STDERR "Skipping this entry\n";
	next;
    }
    if ($entry->has('zblnumber') && !$forceSearch) {
	print $output $entry->raw_bibtex(), "\n\n";
	if ($debug) {
	    print STDERR "DEBUG:  entry ", $entry->key(), 
	    " has zblnumber ", $entry->field('zblnumber'), 
	    " and no forced search is requested\n";
	}
	next;
    }
    

     # Now we have an entry with no Zbl.  Let us get to work.
    if ($debug) {
	print STDERR "DEBUG:  Searching for zbl number for entry ",
	$entry->key, "\n";
    }
     my $zbl = GetZbl($entry, $mirror);
     if (length($zbl) || $forceEmpty) {
 	$entry->field('zblnumber',$zbl);
     }
    print $output $entry->to_string(), "\n\n";

}

$input->close();
$output->close();
exit 0;

###############################################################
#  Getting one Zbl
###############################################################

sub GetZbl {
    my $entry=shift;
    my $mirror=shift;
    
    my @data;

    if ($entry->has('arxiv')) {
	push @data, "arxiv:".$entry->cleaned_field('arxiv');
    }

    if ($entry->has('author')) {
	my @authors = $entry->cleaned_author();
	foreach my $author (@authors) {
	    push @data, "au:".$author->last();
	}
    }

    if ($entry->type() eq 'ARTICLE') {
	push @data, 'dt:j';	
    } elsif ($entry->type() eq 'INCOLLECTION' ||
	     $entry->type() eq 'INBOOK') {
	push @data, 'dt:a';
    } elsif ($entry->type() eq 'BOOK') {
	push @data, 'dt:a';
    } elsif ($entry->type() eq 'MISC') {
	push @data, 'dt:p';
    }

    if ($entry->has('doi')) {
	my $doi = $entry->field('doi');
	$doi =~ s/^http.*doi\.org\///;
	push @data, "doi:$doi";
    }

    if ($entry->has('editor')) {
	my @editors = $entry->cleaned_editor();
	foreach my $editor (@editors) {
	    push @data, "ed:".$editor->last();
	}
    }

    if ($entry->has('journal')) {
	my $journal = $entry->cleaned_field('journal');
	$journal =~ s/:/ /g;
	push @data, "so:".$journal;
    }
    if ($entry->has('title')) {
	my $title =$entry->cleaned_field('title');
	$title =~ s/:/ /g;
	push @data, "ti:".$title
    }
    if ($entry->has('year')) {
	push @data, "py:".$entry->cleaned_field('year');
    }

    
    my $search_string = join("&", @data);

    my $url = "$mirror?search_string=".uri_escape_utf8($search_string).
	"&page=0&results_per_page=1";
    
    if ($debug) {
	print STDERR "DEBUG:  query: $url\n" ;
    }

    my $response = get($url);
    if ($debug) {
	print STDERR "DEBUG:  response: $response \n";
    }
    if (length($response) == 0) {
	return("");
    }
    
    my $content = decode_json($response);
    if (!exists $content->{result}->[0]->{identifier}) {
	if ($debug) {
	    print STDERR "DEBUG: Did not find zbl\n";
	}
	return("");
    } else {
	my $zbl = $content->{result}->[0]->{identifier};
	if ($debug) {
	    print STDERR "DEBUG: Found zbl  $zbl\n";
	}
	return($zbl);
    }
}	    
