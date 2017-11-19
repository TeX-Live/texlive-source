#!/usr/bin/env perl

=pod

=head1 NAME

bibmradd.pl - add MR numbers to papers in a given bib file

=head1 SYNOPSIS

bibmradd  [-d] [B<-f>] [B<-e> 1|0] [B<-o> I<output>] I<bib_file>

=head1 OPTIONS

=over 4

=item B<-d>

Debug mode

=item B<-e>

If 1 (default), add an empty mrnumber if a mr cannot be found.  This
prevents repeated searches for the same entries if you add new entries
to the file.  Calling C<-e 0> suppresses this behavior.


=item B<-f>

Force searching for MR numbers even if the entry already has one.

=item B<-o> I<output>

Output file.  If this option is not used, the name for the 
output file is formed by adding C<_mr> to the input file

=back

=head1 DESCRIPTION

The script reads a BibTeX file.  It checks whether the entries have
mrnumberss.  If not, tries to contact internet to get the numbers.  The
result is a BibTeX file with the fields 
C<mrnumber=...> added.  

The name of the output file is either set by the B<-o> option or 
is derived by adding the suffix C<_mr> to the output file.

=head1 AUTHOR

Boris Veytsman

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2014-2017  Boris Veytsman

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
use Getopt::Std;
use URI::Escape;
use LWP::UserAgent;
$ENV{PERL_LWP_SSL_VERIFY_HOSTNAME}=0;
# Sometimes AMS forgets to update certificates
$ENV{PERL_LWP_SSL_VERIFY_HOSTNAME}=0;

my $USAGE="USAGE: $0  [-d] [-e 1|0] [-f] [-o output] file\n";
my $VERSION = <<END;
bibmradd v2.2
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

$outputfile =~ s/\.([^\.]*)$/_mr.$1/;

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
    "http://www.ams.org/mathscinet-mref";
my $userAgent = LWP::UserAgent->new;

while (my $entry = $parser->next ) {
    if (!$entry->parse_ok()) {
	print STDERR "Cannot understand entry: ";
	$entry->print(*STDERR);
	print STDERR "Skipping this entry\n";
	next;
    }
    if ($entry->has('mrnumber') && !$forceSearch) {
	print $output $entry->raw_bibtex(), "\n\n";
	if ($debug) {
	    print STDERR "DEBUG:  entry ", $entry->key(), 
	    " has mrnumber ", $entry->field('mrnumber'), 
	    " and no forced search is requested\n";
	}
	next;
    }
    

     # Now we have an entry with no MR.  Let us get to work.
    if ($debug) {
	print STDERR "DEBUG:  Searching for mr number for entry ",
	$entry->key, "\n";
    }
     my $mr = GetMr($entry, $userAgent, $mirror);
     if (length($mr) || $forceEmpty) {
 	$entry->field('mrnumber', $mr);
     }
    print $output $entry->to_string(), "\n\n";

}

$input->close();
$output->close();
exit 0;

###############################################################
#  Getting one MR
###############################################################

sub GetMr {
    my $entry=shift;
    my $userAgent=shift;
    my $mirror=shift;
    
    my @query;

    my $string=uri_escape_utf8($entry->to_string());
    
    if ($debug) {
	print STDERR "DEBUG:  query: $mirror?ref=$string&dataType=bibtex\n" ;
    }


    my $response = $userAgent->get("$mirror?ref=$string&dataType=bibtex");
    if ($debug) {
	print STDERR "DEBUG:  response: ",
	$response->decoded_content, "\n";
    }
    
    if ($response->decoded_content =~ /MRNUMBER\s*=\s*{(.*)}/m) {
	my $mr=$1;
	# Somehow mref deletes leading zeros.  They are needed!
	while (length($mr)<7) {
	    $mr = "0$mr";
	}
	if ($debug) {
	    print STDERR "DEBUG:  got MR: $mr\n",
	}
	return $mr;
     } else {
	if ($debug) {
	    print STDERR "DEBUG: Did not get MR\n",
	}
 	return ("");
    }

}
	
