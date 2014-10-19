#!/usr/bin/env perl

=pod

=head1 NAME

bibdoiadd.pl - add DOI numbers to papers in a given bib file

=head1 SYNOPSIS

bibdoiadd [B<-c> I<config_file>]  [B<-o> I<output>] I<bib_file>

=head1 OPTIONS

=over 4

=item B<-c> I<config_file>

Configuration file.  If this file is absent, some defaults are used.
See below for its format.


=item B<-o> I<output>

Output file.  If this option is not used, the name for the 
output file is formed by adding C<_doi> to the input file

=back

=head1 DESCRIPTION

The script reads a BibTeX file.  It checks whether the entries have
DOIs.  If now, tries to contact http://www.crossref.org to get the
corresponding DOI.  The result is a BibTeX file with the fields
C<doi=...> added.  

The name of the output file is either set by the B<-o> option or 
is derived by adding the suffix C<_doi> to the output file.

There are two options for making queries with Crossref: free account
and paid membership.  In the first case you still must register with
Crossref and are limited to a small number of queries, see the
agreement at
C<http://www.crossref.org/01company/free_services_agreement.html>.  In
the second case you have a username and password, and can use them for
automatic queries.  I am not sure whether the use of this script is
allowed for the free account holders.  Anyway if you try to add DOI
to a large number of entries, you should register as a paid member.



=head1 CONFIGURATION FILE 

The configuration file is mostly self-explanatory: it has comments
(starting with C<#>) and assginments in the form

   $field = value ;

The important parameters are C<$mode> (C<'free'> or C<'paid'>,
C<$email> (for free users) and C<$username> & C<$password> for paid
members.


=head1 EXAMPLES

   bibdoiadd -c bibdoiadd.cfg citations.bib > result.bib
   bibdoiadd -c bibdoiadd.cfg citations.bib -o result.bib

=head1 AUTHOR

Boris Veytsman

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2014  Boris Veytsman

This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
L<http://www.gnu.org/licenses/gpl.html>.  There is NO WARRANTY, to the
extent permitted by law.

=cut

use strict;
use Text::BibTeX;
use Text::BibTeX::Name;
use Getopt::Std;
use URI::Escape;
use LWP::Simple;
use TeX::Encode;
use Encode;
use HTML::Entities;
use XML::Entities;

my $USAGE="USAGE: $0 [-c config] [-o output] file\n";
my $VERSION = <<END;
bibdoiadd v1.0
This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html.  There is NO WARRANTY, to the
extent permitted by law.
$USAGE
END
my %opts;
getopts('c:o:hV',\%opts) or die $USAGE;

if ($opts{h} || $opts{V}){
    print $VERSION;
    exit 0;
}

################################################################
# Defaults and parameters
################################################################

my $inputfile = shift;

my $outputfile = $inputfile;

$outputfile =~ s/\.([^\.]*)$/_doi.$1/;

if ($opts{o}) {
    $outputfile = $opts{o};
}

our $mode='free';
our $email;
our $username;
our $password;

if ($opts{c}) {
    if (-r $opts{c}) {
	push @INC, ".";
	require $opts{c};
    } else {
	die "Cannot read options $opts{c}.  $USAGE";
    }
}


# Check the consistency

if ($mode eq 'free' && !length($email)) {
    die "Crossref requires a registered e-mail for the free mode queries\n";
}

if ($mode eq 'paid' && (!length($username) || !length($password))) {
    die 
	"Crossref requires a username and password for the paid mode queries\n";
}

my $input = new Text::BibTeX::File "$inputfile" or 
    die "Cannot BibTeX file $inputfile\n";
my $output = new Text::BibTeX::File "> $outputfile" or 
    die "Cannot write to $outputfile\n";

my $prefix = 
    "http://www.crossref.org/openurl?redirect=false";
if ($mode eq 'free') {
    $prefix .= '&pid='.uri_escape($email);
} else {
    $prefix .= '&pid='.uri_escape($username).":".
	uri_escape($password);
}

# Processing the input
while (my $entry = new Text::BibTeX::Entry $input) {
    if (!$entry->parse_ok()) {
	print STDERR "Cannot understand entry: ";
	$entry->print(*STDERR);
	print STDERR "Skipping this entry\n";
	next;
    }
    if (!(($entry->metatype() eq BTE_REGULAR) &&
	  ($entry->type() eq 'article'))) {
	$entry->write($output);
	next;
    }
    if ($entry->exists('doi')) {
	$entry->write($output);
	next;
    }
    

    # Now we have an entry with no doi.  Let us get to work.
    my $doi = GetDoi($prefix,$entry);
    if (length($doi)) {
	$entry->set('doi',$doi);
    }
    $entry->write($output);

}

$input->close();
$output->close();
exit 0;

###############################################################
#  Getting one doi
###############################################################

sub GetDoi {
    my ($url,$entry) = @_;
    if ($entry->exists('issn')) {
	$url .= "&issn=".uri_escape(SanitizeText($entry->get('issn')));
    }
    if ($entry->exists('journal')) {
	$url .= "&title=".uri_escape(SanitizeText($entry->get('journal')));
    }
    my @names=$entry->names ('author');
    if (scalar(@names)) {
	my @lastnames = $names[0]->part ('last');
	if (scalar(@lastnames)) {
	    my $lastname = SanitizeText(join(' ',@lastnames));
	    $url .= "&aulast=".uri_escape($lastname);
	}
    }
    if ($entry->exists('volume')) {
	$url .= "&volume=".uri_escape($entry->get('volume'));
    }    
    if ($entry->exists('number')) {
	$url .= "&issue=".uri_escape($entry->get('number'));
    }    
    if ($entry->exists('pages')) {
	my $pages=$entry->get('pages');
	$pages =~ s/-.*$//;
	$url .= "&spage=".uri_escape($pages);
    }    
    if ($entry->exists('year')) {
	$url .= "&date=".uri_escape($entry->get('year'));
    }    

    my $result=get($url);

    if ($result =~ m/<doi [^>]*>(.*)<\/doi>/) {
	return $1;
    } else {
	return "";
    }
}
	
###############################################################
#  Sanitization of a text string
###############################################################
sub SanitizeText {
    my $string = shift;
    # There is a bug in the decode function, which we need to work 
    # around:  it adds space to constructions like \o x
    $string =~ s/(\\[a-zA-Z])\s+/$1/g;
    $string =~ s/\\newblock//g;
    $string =~ s/\\urlprefix//g;
    $string =~ s/\\emph//g;
    $string =~ s/\\enquote//g;
    $string =~ s/\\url/URL: /g;
    $string =~ s/\\doi/DOI: /g;
    $string =~ s/\\\\/ /g;
    $string = decode('latex', $string);
    $string =~ s/\\[a-zA-Z]+/ /g;
    $string =~ s/\\\\/ /g;
    $string =~ s/[\[\{\}\]]/ /g;
    $string = encode_entities($string);
    $string = XML::Entities::numify('all', $string);
    $string =~ s/amp;//g;
    $string =~ s/~/ /g;
    $string =~ s/\s*([\.;,])/$1/g;
    return $string;
}
