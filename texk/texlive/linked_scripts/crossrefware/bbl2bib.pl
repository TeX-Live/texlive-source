#!/usr/bin/env perl

=pod

=head1 NAME

bbl2bib.pl - convert thebibliography environment to a bib file

=head1 SYNOPSIS

bbl2bib.pl [-d] [-u] [B<-o> I<output>] I<file>

=head1 OPTIONS

=over 4

=item [-d]

Send debugging output to stdout

=item B<-o> I<output>

Output file.  If this option is not used, the name for the 
output file is formed by changing the extension to C<.bib>


=item B<-u>

Do not clean URL fields.

Normally C<bbl2bib> recognizes URL fields of the kind
C<http://dx.doi.org> and their variants and converts them to DOI
fields (see also L<biburl2doi(1)> script).  The switch B<-u>
suppresses this cleanup.  

=back

=head1 DESCRIPTION

The script tries to reconstruct a C<bib> file from the corresponding
C<thebibliography> environment.  One can argue that this operation is
akin to reconstructing a cow from the steak.  The way the script does
it is searching for the entry in the MR database, and
creating the corresponding BibTeX fields.

The script reads a TeX or Bbl file and extracts from it the
C<thebibliography> environment.  For each bibitem it creates a plain
text bibliography entry, and then  tries to match it in
the database.  

=head1 INPUT FILE

We assume some structure of the input file:

=over 4

=item 1.

The bibliography is contained between the lines

   \begin{thebibliography}...

and

   \end{thebibliography}

=item 2.

Each bibliography item starts from the line 

   \bibitem[...]{....}

=back



=head1 EXAMPLES

   bbl2bib  -o - file.tex > result.bib
   bbl2bib  -o result.bib file.bbl
   bbl2bib  file.tex

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
use FileHandle;
use LaTeX::ToUnicode qw (convert);
use Getopt::Std;
use URI::Escape;
use LWP::Simple;
# Sometimes AMS forgets to update certificates
$ENV{PERL_LWP_SSL_VERIFY_HOSTNAME}=0;


my $USAGE="USAGE: $0 [-d] [-u] [-o output] file\n";
my $VERSION = <<END;
bbl2bib v2.3
This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html.  There is NO WARRANTY, to the
extent permitted by law.
$USAGE
END
our %opts;
getopts('do:huV',\%opts) or die $USAGE;

if ($opts{h} || $opts{V}){
    print $VERSION;
    exit 0;
}

################################################################
# Defaults and parameters
################################################################

my $inputfile = shift;

my $outputfile = $inputfile;

$outputfile =~ s/\.([^\.]*)$/.bib/;

if (exists $opts{o}) {
    $outputfile = $opts{o};
}

my $debug=0;
if ($opts{d}) {
    $debug=1;
}

my $cleanUrls = 1;
if ($opts{u}) {
    $cleanUrls = 0;
}

my $input= IO::File->new($inputfile) or 
    die "Cannot find Bbl or TeX file $inputfile\n$USAGE\n";
my $output = IO::File->new("> $outputfile") or 
    die "Cannot write to $outputfile\n$USAGE\n";

my $userAgent = LWP::UserAgent->new;


# Bibitem is a hash with the entries 'key', 'text', 'mr',
#  'zbl'
my $bibitem;

while (<$input>) {
    if (!(/\\begin\{thebibliography\}/../\\end\{thebibliography\}/) ||
	/\\begin\{thebibliography\}/ || /\\end\{thebibliography\}/) {
	next;
    }
    if (/\\bibitem\s*(\[[^\]]*\])?\{([^\}]*)\}/) {
	ProcessBibitem($bibitem);
	$bibitem = undef;
	$bibitem->{key}=$2;
	$bibitem->{text}="";
    }
    if (!/^\s*$/) {
	$bibitem -> {text} .= $_;
    }
}
ProcessBibitem($bibitem);


exit 0;

sub ProcessBibitem {
    my $bibitem = shift;
    my $key = $bibitem->{key};
    my $text=$bibitem->{text};

    if ($debug) {
	print STDOUT "DEBUG: Processing item $key\n";
    }
    
    if (!length($text) || $text =~ /^\s+$/s) {
	if ($debug) {
	    print STDOUT "DEBUG: No text found\n";
	}
	return;
    }

    my $printtext = $text;
    $printtext =~ s/^(.)/% $1/mg;
    print $output "$printtext";
    $text =~ s/\n/ /mg;
    $text =~ s/\\bibitem(\[[^\]]*\])?\{[^\}]*\}//;

    # Arxiv entry?
    if ($text =~ s/\\arxiv\{([^\}]+)\}\.?//) {
	if ($debug) {
	    print STDOUT "DEBUG: Found arXiv number $1\n";
	}
	$bibitem->{arxiv}=$1;
    }

    # Mr number exists?
    if ($text =~ s/\\mr\{([^\}]+)\}\.?//) {
	if ($debug) {
	    print STDOUT "DEBUG: Found mr number $1\n";
	}
	$bibitem->{mr}=$1;
    }

    # zbl  number exists?
    if ($text =~ s/\\zbl\{([^\}]+)\}\.?//) {
	if ($debug) {
	    print STDOUT "DEBUG: Found zbl number $1\n";
	}
	$bibitem->{zbl}=$1;
    }

    # doi  number exists?
    if ($text =~ s/\\doi\{([^\}]+)\}\.?//) {
	if ($debug) {
	    print STDOUT "DEBUG: Found doi $1\n";
	}
	$bibitem->{doi}=$1;
    }

    $bibitem->{bib} = SearchMref($bibitem);

    if ($cleanUrls) {
	$bibitem->{bib} = CleanUrl ($bibitem->{bib});
    }
    
    PrintBibitem($bibitem);
    return;
}


sub SearchMref {
    my $bibitem = shift;
    my $mirror = "http://www.ams.org/mathscinet-mref";
    my $string=uri_escape_utf8($bibitem->{text});
    if ($debug) {
	print STDOUT "Sending $mirror?ref=$string".'&'."dataType=bibtex\n"
    }
    my $response = $userAgent->get("$mirror?ref=$string&dataType=bibtex") ->
	decoded_content();
    if ($debug) {
	print STDOUT "DEBUG: Response $response\n";
    }
    if ($response =~ /<pre>(.*)<\/pre>/s) {
	my $bib= $1;
	# Looks like Mathscinet sometimes fails to unaccent text.  
	# For the time being we just delete the offending characters.
	# Should probably write LaTeX::FromUnicode instead
	$bib =~ s/[^\x00-\x7f]//g;
	if ($debug) {
	    print STDOUT "DEBUG: got $bib\n";
	}
	my $fh = new FileHandle;
	open $fh, "<", \$bib;
	my $parser = new BibTeX::Parser($fh);
	my $entry = $parser->next;
	if (ref($entry) && $entry->parse_ok()) {
	    $entry->key($bibitem->{key});
	    return ($entry);
	} else {
	    if ($debug) {
		if (!ref($entry)) {
		    print STDERR "DEBUG: not a reference\n";
		} else{
		    print STDERR "DEBUG: parsing $entry->parse_ok\n";
		}
	    }
	}
    } else {
	if ($debug) {
	    print STDOUT "DEBUG: did not get an entry\n";
	}
    }
}

sub CleanUrl {
    my $entry = shift;
    if (!ref($entry)) {
	return $entry;
    }

    if ($entry->has('doi')) {
	return $entry;
    }
    if (!$entry->has('url')) {
	return $entry;
    }
    if ($entry->field('url') =~ m|^http(?:s)?://(?:dx\.)?doi\.org/(.*)$|) {
	$entry->field('doi', $1);
	delete $entry->{'url'};
    }
    return $entry;
    
}


sub PrintBibitem {
    print $output "\n";
    my $bibitem = shift;
    if (!ref($bibitem->{bib})) {
	return;
    }
    my $entry=$bibitem->{bib};
    if ($bibitem->{mr} && ! $entry->field('mrnumber')) {
	$entry->field('mrnumber', $bibitem->{mr});
    }
    if ($entry->field('mrnumber')) {
	my $mr=$entry->field('mrnumber');
	while (length($mr)<7) {
	    $mr = "0$mr";
	}
	$mr=$entry->field('mrnumber', $mr);
    }
    if ($bibitem->{zbl} && ! $entry->field('zblnumber')) {
	$entry->field('zblnumber', $bibitem->{zbl});
    }
    if ($bibitem->{doi} && ! $entry->field('doi')) {
	$entry->field('doi', $bibitem->{doi});
    }
    if ($bibitem->{arxiv} && ! $entry->field('arxiv')) {
	$entry->field('arxiv', $bibitem->{arxiv});
    }
	
    print $output $entry->to_string(), "\n\n";
}


