#!/usr/bin/env perl

=pod

=head1 NAME

bibdoiadd.pl - add DOI numbers to papers in a given bib file

=head1 SYNOPSIS

bibdoiadd [B<-c> I<config_file>] [B<-C> 1|0] [B<-e> 1|0] [B<-f>] [B<-o> I<output>] I<bib_file>

=head1 OPTIONS

=over 4

=item B<-c> I<config_file>

Configuration file.  If this file is absent, some defaults are used.
See below for its format.

=item B<-C> 1|0

Whether to canonicalize names in the output (1) or not (0).  By default, 1.

=item B<-e>

If 1 (default), add empty doi if a doi cannot be found.  This prevents
repeated searches for the same entries if you add new entries to the
file.  Calling C<-e 0> suppresses this behavior.

=item B<-f>

Force checking doi number even if one is present

=item B<-o> I<output>

Output file.  If this option is not used, the name for the 
output file is formed by adding C<_doi> to the input file

=back

=head1 DESCRIPTION

The script reads a BibTeX file.  It checks whether the entries have
DOIs.  If not, it tries to contact http://www.crossref.org to get the
corresponding DOI.  The result is a BibTeX file with the fields
C<doi=...> added.

The name of the output file is either set by the B<-o> option or 
is derived by adding the suffix C<_doi> to the output file.

Every BibTeX record in the input is parsed, using BibTeX::Parser, but
only the ones that do not have the C<doi> field (or C<mrnumber> or
C<zblnumber> for the sibling scripts) are processed. These entries
without the requested field are written back, as described in
BibTeX::Parser::Entry.

The bib records that are not processed (because they already have the
requested field) are written back as-is, without any reformatting.

There are (were?) two options for making queries with Crossref: free
account and paid membership. In the first case you still must register
with Crossref and are limited to a small number of queries, see the
agreement at
C<http://www.crossref.org/01company/free_services_agreement.html>. In
the second case you have a username and password, and can use them for
automatic queries. I am not sure whether the use of this script is
allowed for the free account holders. At any rate, if you want to add
DOIs to a large number of entries, you should register as a paid member.


=head1 CONFIGURATION FILE 

The configuration file relates to the Crossref queries, and is mostly
self-explanatory: it has comments (starting with C<#>) and assginments
in the form

   $field = value ;

The important parameters are C<$mode> (C<'free'> or C<'paid'>),
C<$email> (for free users) and C<$username> & C<$password> for paid
members.


=head1 EXAMPLES

   bibdoiadd -c bibdoiadd.cfg -o - citations.bib > result.bib
   bibdoiadd -c bibdoiadd.cfg -o result.bib citations.bib 

=head1 AUTHOR

Boris Veytsman

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2014-2024 Boris Veytsman

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
use LaTeX::ToUnicode qw (convert);
use Getopt::Std;
use URI::Escape;
use LWP::Simple;
# Sometimes AMS forgets to update certificates
$ENV{PERL_LWP_SSL_VERIFY_HOSTNAME}=0;

my $USAGE="USAGE: $0 [-c config] [-C 1|0] [-e 1|0] [-f] [-o output] file\n";
my $VERSION = <<END;
bibdoiadd v2.3
This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html.  There is NO WARRANTY, to the
extent permitted by law.
$USAGE
END
our %opts;
getopts('fe:c:C:o:hV',\%opts) or die $USAGE;

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

if (exists $opts{o}) {
    $outputfile = $opts{o};
}

my $forceSearch=$opts{f};
my $forceEmpty = 1;
if (exists $opts{e}) {
    $forceEmpty = $opts{e};
}		

my $canonizeNames = 1;
if (exists $opts{C}) {
    $canonizeNames = $opts{C};
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

my $input= IO::File->new($inputfile) or 
    die "Cannot find BibTeX file $inputfile\n$USAGE\n";
my $output = IO::File->new("> $outputfile") or 
    die "Cannot write to $outputfile\n$USAGE\n";

my $parser=new BibTeX::Parser($input);

my $prefix = 
    "http://www.crossref.org/openurl?redirect=false";
if ($mode eq 'free') {
    $prefix .= '&pid='.uri_escape($email);
} else {
    $prefix .= '&pid='.uri_escape($username).":".
	uri_escape($password);
}

# Processing the input
while (my $entry = $parser->next) {
    if (!$entry->parse_ok()) {
	print STDERR "Cannot understand entry: ";
	$entry->print(*STDERR);
	print STDERR "Skipping this entry\n";
	next;
    }

    if (!($entry->type() eq 'ARTICLE') && !($entry->type() eq 'BOOK')
	&& !($entry->type() eq 'INCOLLECTION')) {
	print $output $entry->raw_bibtex(), "\n\n";
	next;
    }
    if ($entry->has('doi') && !$forceSearch) {
	print $output $entry->raw_bibtex(), "\n\n";
	next;
    }
    
    

     my $doi = GetDoi($prefix, $entry);
     if (length($doi) || $forceEmpty) {
 	$entry->field('doi',$doi);
     }

    print $output 
	 $entry->to_string(canonize_names=>$canonizeNames), 
	 "\n\n";


}

$input->close();
$output->close();
exit 0;

###############################################################
#  Getting one doi
###############################################################

sub GetDoi {
    my ($url,$entry) = @_;
    if ($entry->has('issn')) {
	$url .= "&issn=".uri_escape_utf8(SanitizeText($entry->field('issn')));
    }
    if ($entry->has('journal')) {
	$url .= "&title=".uri_escape_utf8(SanitizeText($entry->field('journal')));
    }
    my @names=$entry->author();
    if (scalar(@names)) {
	my $lastname = SanitizeText($names[0]->last());
	$url .= "&aulast=".uri_escape_utf8($lastname);
    }
    if ($entry->has('volume')) {
	$url .= "&volume=".uri_escape_utf8($entry->field('volume'));
    }    
    if ($entry->has('number')) {
	$url .= "&issue=".uri_escape_utf8($entry->field('number'));
    }    
    if ($entry->has('pages')) {
	my $pages=$entry->field('pages');
	$pages =~ s/-.*$//;
       $url .= "&spage=".uri_escape_utf8($pages);
    }    
    if ($entry->has('year')) {
	$url .= "&date=".uri_escape_utf8($entry->field('year'));
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
    $string = convert($string);
    $string =~ s/\\newblock//g;
    $string =~ s/\\bgroup//g;
    $string =~ s/\\egroup//g;
    $string =~ s/\\scshape//g;
    $string =~ s/\\urlprefix//g;
    $string =~ s/\\emph//g;
    $string =~ s/\\textbf//g;
    $string =~ s/\\enquote//g;
    $string =~ s/\\url/URL: /g;
    $string =~ s/\\doi/DOI: /g;
    $string =~ s/\\\\/ /g;
    $string =~ s/\$//g;
    $string =~ s/\\checkcomma/,/g;
    $string =~ s/~/ /g;
    $string =~ s/[\{\}]//g;
    return $string;
}
