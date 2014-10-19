#!/usr/bin/env perl

=pod

=head1 NAME

bibzbladd.pl - add Zbl numbers to papers in a given bib file

=head1 SYNOPSIS

bibzbladd  [B<-o> I<output>] I<bib_file>

=head1 OPTIONS

=over 4


=item B<-o> I<output>

Output file.  If this option is not used, the name for the 
output file is formed by adding C<_zbl> to the input file

=back

=head1 DESCRIPTION

The script reads a BibTeX file.  It checks whether the entries have
Zbls.  If now, tries to contact internet to get the numbers.  The
result is a BibTeX file with the fields 
C<zblnumber=...> added.  

The name of the output file is either set by the B<-o> option or 
is derived by adding the suffix C<_zbl> to the output file.

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
use LWP::UserAgent;
use TeX::Encode;

my $USAGE="USAGE: $0  [-o output] file\n";
my $VERSION = <<END;
bibzbladd v1.0
This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html.  There is NO WARRANTY, to the
extent permitted by law.
$USAGE
END
my %opts;
getopts('o:hV',\%opts) or die $USAGE;

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

my $input = new Text::BibTeX::File "$inputfile" or 
    die "Cannot BibTeX file $inputfile\n";
my $output = new Text::BibTeX::File "> $outputfile" or 
    die "Cannot write to $outputfile\n";


# Creating the HTTP parameters
my $mirror =
    "http://www.zentralblatt-math.org/MIRROR/zmath/en/search/";
my $userAgent = LWP::UserAgent->new;

# Processing the input
while (my $entry = new Text::BibTeX::Entry $input) {
    if (!$entry->parse_ok()) {
	print STDERR "Cannot understand entry: ";
	$entry->print(*STDERR);
	print STDERR "Skipping this entry\n";
	next;
    }
    if (!(($entry->metatype() eq BTE_REGULAR))) {
	$entry->write($output);
	next;
    }
    if ($entry->exists('zblnumber')) {
	$entry->write($output);
	next;
    }
    

    # Now we have an entry with no Zbl.  Let us get to work.
    my $zbl = GetZbl($entry, $userAgent, $mirror);
    if (length($zbl)) {
	$entry->set('zblnumber',$zbl);
    }
    $entry->write($output);

}

$input->close();
$output->close();
exit 0;

###############################################################
#  Getting one Zbl
###############################################################

sub GetZbl {
    my $entry=shift;
    my $userAgent=shift;
    my $mirror=shift;
    
    my @query;

    my @names=$entry->names ('author');
    if (scalar(@names)) {
	foreach my $name (@names) {
	    my @lastnames = $name->part ('last');
	    if (scalar(@lastnames)) {
		foreach my $lastname (@lastnames) {
		    push @query, "au:$lastname";
		}
	    }
	}
    }
    if (my $title = $entry->get('title')) {
	push @query, "ti:$title";
    }
    
    if (my $year = $entry->get('year')) {
	push @query, "py:$year";
    }

    if (my $year = $entry->get('year')) {
	push @query, "py:$year";
    }

    my $type = $entry->type;
    if ($type eq 'article') {
	push @query, "dt:j";
    }
    if ($type eq 'book') {
	push @query, "dt:b";
    }
    if ($type eq 'inproceedings' || $type eq 'incollection') {
	push @query, "dt:a";
    }

    my $source = "";
    if ($type eq 'article') {
	if ($entry->get('journal')) {
	    $source .= $entry->get('journal');
	}
	if (my $vol=$entry->get('volume')) {
	    $source .= ",$vol";
	}
	if (my $pages=$entry->get('pages')) {
	    $source .= ",$pages";
	}
    } else {
	if (my $bt=$entry->get('booktitle')) {
	    $source .= "$bt";
	}
    }

    if ($source) {
	push @query, "so:$source";
    }


    my $qstring = join(" & ", @query);
#    print STDERR "$qstring\n";

    my $form;

    $form->{name}='form';
    $form->{q} =  $qstring;
    $form->{type} = "ascii";
    $form->{submit} = 'Search';

    my $response = $userAgent->post($mirror, $form);
    if ($response->decoded_content =~ /^an:\s*Zbl\s*(\S+)\s*$/m) {
	return $1;
    } else {
	return ("");
    }
}
	
