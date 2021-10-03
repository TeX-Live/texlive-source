#!/usr/bin/env perl

=pod

=head1 NAME

ltx2crossrefxml.pl - create XML files for submitting to crossref.org

=head1 SYNOPSIS

ltx2crossrefxml [B<-c> I<config_file>]  [B<-o> I<output_file>] [B<-input-is-xml>]
                I<latex_file1> I<latex_file2> ...

=head1 OPTIONS

=over 4

=item B<-c> I<config_file>

Configuration file.  If this file is absent, defaults are used.
See below for its format.

=item B<-o> I<output_file>

Output file.  If this option is not used, the XML is output to stdout.

=item B<-rpi-is-xml>

Do not transform author and title input strings, assume they are valid XML.

=back

The usual C<--help> and C<--version> options are also supported. Options
can begin with either C<-> or C<-->, and ordered arbitrarily.

=head1 DESCRIPTION

For each given I<latex_file>, this script reads C<.rpi> and (if they
exist) C<.bbl> files and outputs corresponding XML that can be uploaded
to Crossref (L<https://crossref.org>). Any extension of I<latex_file> is
ignored, and I<latex_file> itself is not read (and need not even exist).

Each C<.rpi> file specifies the metadata for a single article to be
uploaded to Crossref (a C<journal_article> element in their schema); an
example is below. These files are output by the C<resphilosophica>
package (L<https://ctan.org/pkg/resphilosophica>), but (as always) can
also be created by hand or by whatever other method you implement.

Any C<.bbl> files present are used for the citation information in the
output XML. See the L<CITATIONS> section below.

Unless C<--rpi-is-xml> is specified, for all text (authors, title,
citations), standard TeX control sequences are replaced with plain text
or UTF-8 or eliminated, as appropriate. The C<LaTeX::ToUnicode::convert>
routine is used for this (L<https://ctan.org/pkg/bibtexperllibs>).
Tricky TeX control sequences will almost surely not be handled
correctly. If C<--rpi-is-xml> is given, the author and title strings
from the rpi files are output as-is, assuming they are valid XML; no
checking is done. Citation text from C<.bbl> files is always converted
from LaTeX to plain text.

This script just writes an XML file. It's up to you to actually do the
uploading to Crossref; for example, you can use their Java tool 
C<crossref-upload-tool.jar>
(L<https://www.crossref.org/education/member-setup/direct-deposit-xml/https-post>).
For the definition of their schema, see
L<https://data.crossref.org/reports/help/schema_doc/4.4.2/index.html>
(this is the schema version currently followed by this script).

=head1 CONFIGURATION FILE FORMAT

The configuration file is read as Perl code. Thus, comment lines
starting with C<#> and blank lines are ignored. The other lines are
typically assignments in the form (spaces are optional):

    $variable = value ;

Usually the value is a C<"string"> enclosed in ASCII double-quote or
single-quote characters, per Perl syntax. The idea is to specify the
user-specific and journal-specific values needed for the Crossref
upload. The variables which are used are these:

    $depositorName = "Depositor Name";
    $depositorEmail = 'depositor@example.org';
    $registrant = 'Registrant';  # organization name
    $fullTitle = "FULL TITLE";   # journal name
    $issn = "1234-5678";         # required
    $abbrevTitle = "ABBR. TTL."; # optional
    $coden = "CODEN";            # optional


For a given run, all C<.rpi> data read is assumed to belong to the
journal that is specified in the configuration file. More precisely, the
configuration data is written as a C<journal_metadata> element, with
given C<full_title>, C<issn>, etc., and then each C<.rpi> is written as
C<journal_issue> plus C<journal_article> elements.

The configuration file can also define one Perl function:
C<LaTeX_ToUnicode_convert_hook>. If it is defined, it is called at the
beginning of the procedure that converts LaTeX text to Unicode, which is
done with the L<LaTeX::ToUnicode> module, from the C<bibtexperllibs>
package (L<https://ctan.org/pkg/bibtexperllibs>). The function must
accept one string (the LaTeX text), and return one string (presumably
the transformed string). The standard conversions are then applied to
the returned string, so the configured function need only handle special
cases, such as control sequences particular to the journal at hand.

=head1 RPI FILE FORMAT

Here's the (relevant part of the) C<.rpi> file corresponding to the
C<rpsample.tex> example in the C<resphilosophica> package
(L<https://ctan.org/pkg/resphilosophica>):

  %authors=Boris Veytsman\and A. U. Th{\o }r\and C. O. R\"espondent
  %title=A Sample Paper:\\ \emph  {A Template}
  %year=2012
  %volume=90
  %issue=1--2
  %startpage=1
  %endpage=1
  %doi=10.11612/resphil.A31245
  %paperUrl=http://borisv.lk.net/paper12
  %publicationType=full_text

Other lines, some not beginning with %, are ignored (and not shown).
For more details on processing, see the code.

The C<%paperUrl> value is what will be associated with the given C<%doi>
(output as the C<resource> element). Crossref strongly recommends that
the url be for a so-called landing page, and not directly for a pdf
(L<https://www.crossref.org/education/member-setup/creating-a-landing-page/>).
Special case: if the url is not specified, 
and the journal is I<S<Res Philosophica>>,
a special-purpose search url using L<pdcnet.org> is returned.
Any other journal must always specify this.

The C<%authors> field is split at C<\and> (ignoring whitespace before
and after), and output as the C<contributors> element, using
C<sequence="first"> for the first listed, C<sequence="additional"> for
the remainder.

If the C<%publicationType> is not specified, it defaults to
C<full_text>, since that has historically been the case; C<full_text>
can also be given explicitly. The other values allowed by the Crossref
schema are C<abstract_only> and C<bibliographic_record>. Finally, if the
value is C<omit>, the C<publication_type> attribute is omitted entirely
from the given C<journal_article> element.

Each C<.rpi> must contain information for only one article, but multiple
files can be read in a single run. It would not be difficult to support
multiple articles in a single C<.rpi> file, but it makes debugging and
error correction easier when each uploaded XML contains a single
article.

=head2 MORE ABOUT AUTHOR NAMES

The three formats for names recognized are (not coincidentally) the same
as BibTeX:

   First von Last
   von Last, First
   von Last, Jr., First
   
The forms can be freely intermixed within a single C<%authors> line,
separated with C<\and> (including the backslash). Commas as name
separators are not supported, unlike BibTeX.

In short, you may almost always use the first form; you shouldn't if
either there's a Jr part, or the Last part has multiple tokens but
there's no von part. See the C<btxdoc> (``BibTeXing'' by Oren Patashnik)
document for details.

In the C<%authors> line of a C<.rpi> file, some secondary directives are
recognized, indicated by C<|> characters. Easiest to explain with an
example:

  %authors=|organization|\LaTeX\ Project Team \and Alex Brown|orcid=123

Thus: 1) if C<|organization|> is specified, the author name will be output
as an C<organization> contributor, instead of the usual C<person_name>,
as the Crossref schema requires.

2) If C<|orcid=I<value>|> is specified, the I<value> is output as an
C<ORCID> element for that C<person_name>.

These two directives, C<|organization>| and C<|orcid|> are mutually
exclusive, because that's how the Crossref schema defines them. The C<=>
sign after C<orcid> is required, while all spaces after the C<orcid>
keyword are ignored. Other than that, the ORCID value is output
literally. (E.g., the ORCID value of C<123> above is clearly invalid,
but it would be output anyway, with no warning.)

Extra C<|> characters, at the beginning or end of the entire C<%authors>
string, or doubled in the middle, are accepted and ignored. Whitespace
is ignored around all C<|> characters.

=head1 CITATIONS

Each C<.bbl> file corresponding to an input C<.rpi> file is read and
used to output a C<citation_list> element for that C<journal_article> in
the output XML. If no C<.bbl> file exists for a given C<.rpi>,
no C<citation_list> is output for that article.

The C<.bbl> processing is rudimentary: only so-called
C<unstructured_citation> references are produced for Crossref, that is,
the contents of the citation (each paragraph in the C<.bbl>) is dumped
as a single flat string without markup.

Bibliography text is unconditionally converted from TeX to XML, via the
method described above. It is not unusual for the conversion to be
incomplete or incorrect.  It is up to you to check for this; e.g., if
any backslashes remain in the output, it is most likely an error.

Furthermore, it is assumed that the C<.bbl> file contains a sequence of
references, each starting with C<\bibitem{I<KEY>}> (which itself must be
at the beginning of a line, preceded only by whitespace), and the whole
bibliography ending with C<\end{thebibliography}> (similarly at the
beginning of a line). A bibliography not following this format will not
produce useful results. Bibliographies can be created by hand, or with
BibTeX, or any other method.

The C<key> attribute for the C<citation> element is taken as the I<KEY>
argument to the C<\bibitem> command. The sequential number of the
citation (1, 2, ...) is appended. The argument to C<\bibitem> can be
empty (C<\bibitem{}>, and the sequence number will be used on its own.
Although TeX will not handle empty C<\bibitem> keys, it can be
convenient when creating a C<.bbl> purely for Crossref.

The C<.rpi> file is also checked for the bibliography information, in
this same format.

Feature request: if anyone is interested in figuring out how to generate
structured citations
(L<https://data.crossref.org/reports/help/schema_doc/4.4.2/schema_4_4_2.html#citation>)
instead of these flat text dumps, that would be great.

=head1 EXAMPLES

  ltx2crossrefxml.pl ../paper1/paper1.tex ../paper2/paper2.tex \
                      -o result.xml

  ltx2crossrefxml.pl -c myconfig.cfg paper.tex -o paper.xml

=head1 AUTHOR

Boris Veytsman L<https://github.com/borisveytsman/crossrefware>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2012-2021  Boris Veytsman

This is free software.  You may redistribute copies of it under the
terms of the GNU General Public License
L<https://www.gnu.org/licenses/gpl.html>.  There is NO WARRANTY, to the
extent permitted by law.

=cut

 use strict;
 use warnings;

 use Cwd;
 use File::Basename;
 use File::Spec;

 BEGIN {
     # find files relative to our installed location within TeX Live
     chomp(my $TLMaster = `kpsewhich -var-value=TEXMFROOT`); # TL root
     if (length($TLMaster)) {
	 unshift @INC, "$TLMaster/texmf-dist/scripts/bibtexperllibs";
     }
     # find development bibtexperllibs in sibling checkout to this script,
     # even if $0 is a symlink. All irrelevant when using from an installation.
     my $real0 = Cwd::abs_path($0);
     my $scriptdir = File::Basename::dirname($real0);
     my $dev_btxperllibs = Cwd::abs_path("$scriptdir/../bibtexperllibs");
     # we need the lib/ subdirectories inside ...
     unshift (@INC, glob ("$dev_btxperllibs/*/lib")) if -d $dev_btxperllibs;
 }

 use POSIX qw(strftime);

 use BibTeX::Parser::Author;
 use LaTeX::ToUnicode;

 my $USAGE = <<END;
Usage: $0 [-c CONFIG] [-o OUTPUT] [--rpi-is-xml] LTXFILE...

Convert .rpi and (if any are present) .bbl files corresponding to each
LTXFILE to xml, for submitting to crossref.org. The LTXFILE is not read
(and need not even exist); any extension it has is replaced by .rpi and
.bbl.

The .rpi files are plain text, with values on lines beginning with %, as
output by (for example) the resphilosophica LaTeX package. The .bbl
files are as output by BibTeX. Both are also commonly created by hand.
The documentation for this script has examples.

The xml is written to standard output by default; the -o (--output)
option overrides this.

If the -c (--config) option is given, the given file is read before any
processing is done. This is used to define journal-specific defaults.

The usual --help and --version options are also supported.

For an example of using this script and associatd code, see the TUGboat
processing at
https://github.com/TeXUsersGroup/tugboat/tree/trunk/capsules/crossref.

Development sources, bug tracker: https://github.com/borisveytsman/crossrefware
Releases: https://ctan.org/pkg/crossrefware
END

 my $VERSION = <<END;
ltx2crossrefxml (crossrefware) 2.51
This is free software: you are free to change and redistribute it, under
the terms of the GNU General Public License
http://www.gnu.org/licenses/gpl.html (any version).
There is NO WARRANTY, to the extent permitted by law.

Written by Boris Veytsman.
END
 use Getopt::Long;
 my %opts;

 GetOptions(
   "config|c=s" => \($opts{c}),
   "output|o=s" => \($opts{o}),
   "rpi-is-xml!"=> \($opts{xi}),
   "version|V"  => \($opts{V}),
   "help|?"     => \($opts{h})) || pod2usage(1);

 if ($opts{h}) { print "$USAGE\n$VERSION"; exit 0; } 
 if ($opts{V}) { print $VERSION; exit 0; } 

 use utf8;
 binmode(STDOUT, ":utf8");

 ################################################################
 # Defaults and parameters
 ################################################################

 *OUT=*STDOUT;
 
 if (defined($opts{o})) {
     open (OUT, ">$opts{o}") or die "open($opts{o}) for writing failed: $!\n";
     binmode(OUT, ":utf8")
 }


 our $depositorName = 'DEPOSITOR_NAME';
 our $depositorEmail = 'DEPOSITOR_EMAIL';
 our $registrant = 'REGISTRANT';
 our $fullTitle = "FULL TITLE";
 our $abbrevTitle = "ABBR. TTL.";
 our $issn = "0000-0000";
 our $coden = "CODEN";
 our $timestamp = strftime("%Y%m%d%H%M%S", gmtime);
 # use timestamp in batchid, since the value is supposed to be unique
 # for every submission to crossref by a given publisher.
 # https://data.crossref.org/reports/help/schema_doc/4.4.2/schema_4_4_2.html#doi_batch_id
 our $batchId="ltx2crossref-$timestamp-$$";


 if ($opts{c}) {
     if (-r $opts{c}) {
         # if config arg is absolute, fine; if not, prepend "./" as slightly
         # less troublesome than putting "." in the @INC path.
         my $rel = (File::Spec->file_name_is_absolute($opts{c}) ? "" : "./");
	 require "$rel$opts{c}";
     } else {
	 die "Cannot read config file $opts{c}. Goodbye.";
     }
 }

 PrintHead();

 # 
 # The hash %papers.  Keys year->vol->issue->number
 #
 my %papers;

 foreach my $file (@ARGV) {
     AddPaper($file);
 }

 foreach my $year (keys %papers) {
     foreach my $volume (keys %{$papers{$year}}) {
	 foreach my $issue (keys %{$papers{$year}->{$volume}}) {
	     PrintIssueHead($year, $volume, $issue);
	     my $paperList = $papers{$year}->{$volume}->{$issue};
	     foreach my $paper (@{$paperList}) {
		 PrintPaper($paper);
	     }
	 }
     }
 }

 PrintTail();
 exit(0);


#####################################################
#  Printing the head and the tail
#####################################################
sub PrintHead {
    # do not output the <coden> or <abbrev_title> if the journal doesn't
    # have them.
    my $indent = "        ";
    my $coden_out = $coden ne "CODEN" ? "\n$indent<coden>$coden</coden>" : "";
    my $abbrev_title_out = $abbrevTitle ne "ABBR. TTL."
        ? "\n$indent<abbrev_title>$abbrevTitle</abbrev_title>"
        : "";

    # as of schema version 4.3.4, crossref renamed the <name> element
    # inside <depositor> to <depositor_name>. Sigh. Something to take
    # into account with older schemas.
    # https://www.crossref.org/education/content-registration/crossrefs-metadata-deposit-schema/schema-versions/
    print OUT <<END;
<doi_batch xmlns="http://www.crossref.org/schema/4.4.2" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="4.4.2" xsi:schemaLocation="http://www.crossref.org/schema/4.4.2 http://www.crossref.org/schema/deposit/crossref4.4.2.xsd">
  <head>
    <doi_batch_id>$batchId</doi_batch_id>
    <timestamp>$timestamp</timestamp>
    <depositor>
      <depositor_name>$depositorName</depositor_name>
      <email_address>$depositorEmail</email_address>
    </depositor>
    <registrant>$registrant</registrant>
  </head>
  <body><journal>
    <journal_metadata language="en">
      <full_title>$fullTitle</full_title>$abbrev_title_out
      <issn>$issn</issn>$coden_out	
    </journal_metadata>
END
}

sub PrintTail {
    print OUT <<END;
  </journal></body>
</doi_batch>
END

    return;
}


#######################################################
#  Adding one paper from $file.rpi and .bbl to global %papers.
#######################################################
sub AddPaper {
    my $file = shift;
    my ($name,$path,$suffix) = fileparse($file, '\.[^\.]*$');
    my $rpifile = File::Spec->catfile($path, "$name.rpi");
    open (RPI, $rpifile)
      or die "open($rpifile) failed: $! (did you process $file?)\n";
    my %data;
    while (<RPI>) {
	chomp;
        if (/^%([^=]*)\s*=\s*(.*)\s*$/) {
           if (exists $data{$1}) {
             warn "$rpifile:$.: already saw data{$1}=$data{$1};"
                  . " an .rpi file should have data for only one article,"
                  . " but overwriting with `$2' anyway.\n";
           }
           $data{$1} = $2;
        }
    }
    close RPI;
    
    # look for bibliographies in both the .rpi and any .bbl file.
    my @bibliography;
    foreach my $bibfile ($file, File::Spec->catfile($path, "$name.bbl")) {
         @bibliography = (@bibliography, AddBibliography($bibfile));
    }
    $data{'bibliography'} = \@bibliography;

    # Die if the fields we use unconditionally are empty. Not all of
    # them are required by the schema, but we can wait to generalize.
    foreach my $field (qw(title year volume issue startpage endpage doi)) {
        if (! $data{$field}) {
            die ("$0: field must not be empty: $field\n  "
                 . &debug_hash_as_string("whole hash", %data));
        }
    }

    push @{$papers{$data{year}}->{$data{volume}}->{$data{issue}}}, \%data;
}

############################################################## 
# Reading a list of papers from BIBFILE and adding it to the
# bibliography. Each item is assumed to start with
# \bibitem{KEY} and the whole bib to end with \end{thebibliography}.
# 
# We return a list of hashes, each hash with a single key, the citation
# key, and its value a flat string of the entry.
# 
# No conversion of the text is done here.
##############################################################
sub AddBibliography {
    my $bibfile = shift;
    open (BIB, $bibfile) or return;
    
    my $insidebibliography = 0;
    my $currpaper = ""; # that is, the current bib entry
    my $bibno = 0;
    my @result;
    my $key;
    while (<BIB>) {
	chomp;
	next if /^\s*%/; # TeX comment line
	s/[ \t]%.*//;    # remove TeX comment
	#
	# allow empty \bibitem key for the sake of handwritten bbls.
	# Similarly, might be more stuff on the line when handwritten.
	if (s/^\s*\\bibitem(?:\[.*?\])?+\s*\{(.*?)\}//) {
	    my $newkey = $1;
	    if ($insidebibliography) {
		if ($currpaper) {
                    # Append the current sequence number for this citation,
                    # since that's what Crossref recommends (sort of).
                    # For prettiness, if the key is otherwise empty,
                    # don't include a dash beforehand.
		    $bibno++;
                    $key .= ($key ? "-" : "") . $bibno;
                    #
                    my %paperhash;
		    $paperhash{$key} = $currpaper;
		    push @result, \%paperhash;
		}
	    }
	    # The citation key (required by schema) starts as the bibitem key.
	    $key = $newkey;
	    
	    $currpaper = $_;
	    $insidebibliography = 1;
	    next;
	}
	if (/^\s*\\end\{thebibliography\}/) {
	    if ($currpaper) {
	        $bibno++;
                $key .= ($key ? "-" : "") . $bibno;
                #
		my %paperhash;
		$paperhash{$key} = $currpaper;
		push @result, \%paperhash;
	    }
	    $currpaper = "";
	    $insidebibliography = 0;
	    next;
	}
	if ($insidebibliography) {
	    $currpaper .= " $_";
	}
    }
    close BIB;
    
    # We look in the .rpi files too, which will generally have none.
    if (@result == 0 && $bibfile =~ /\.bbl$/) {
        warn "$0: no \\bibitems found in: $bibfile\n";
    } elsif ($insidebibliography) {
        warn "$0: no \\end{thebibliography} found in: $bibfile\n";
        warn "$0:   so the last bib entry is missing.\n";
    }
    return @result;
}

#################################################################
#  Printing information about one issue
#################################################################
sub PrintIssueHead {
    my ($year, $volume, $issue) = @_;
    print OUT <<END;
    <journal_issue>
      <publication_date media_type="print">
        <year>$year</year>
      </publication_date>
      <journal_volume><volume>$volume</volume></journal_volume>
      <issue>$issue</issue>
    </journal_issue>
END
}

###############################################################
# Printing information about one paper
###############################################################
sub PrintPaper {
    my $paper = shift;
    my $title = SanitizeText($paper->{title});
    my $url = GetURL($paper);
    my $publication_type = GetPublicationType($paper->{publicationType});
    
    &TitleCheck($title);
    print OUT <<END;
    <journal_article$publication_type>
      <titles>
        <title>$title</title>
      </titles>
      <contributors>
END
    my @authors = split /\s*\\and\s*/, $paper->{authors};
    my $seq = 'first';
    foreach my $author (@authors) {
	PrintAuthor($author, $seq);
        $seq = 'additional';
    }

    print OUT <<END;
      </contributors>
      <publication_date media_type="print">
        <year>$paper->{year}</year>
      </publication_date>
      <pages>
        <first_page>$paper->{startpage}</first_page>
        <last_page>$paper->{endpage}</last_page>
      </pages>
      <doi_data>
        <doi>$paper->{doi}</doi>
        <timestamp>$timestamp</timestamp>
        <resource>$url</resource>
      </doi_data>
END

    if (scalar(@{$paper->{bibliography}})) {
    print OUT <<END;
      <citation_list>
END
    foreach my $citation (@{$paper->{bibliography}}) {
	PrintCitation($citation);
    }
    print OUT <<END;
      </citation_list>
END
    }

    print OUT <<END;
    </journal_article>
END
}


###############################################################
# Crossref <title> strings can contain a few so-called "face" HTML
# commands. Complain if they have anything anything else.
# schema doc: https://data.crossref.org/reports/help/schema_doc/4.4.2/schema_4_4_2.html#title
#   face doc: https://www.crossref.org/education/content-registration/crossrefs-metadata-deposit-schema/face-markup/
# mathml doc: https://www.crossref.org/education/content-registration/crossrefs-metadata-deposit-schema/including-mathml-in-deposits/
# 
# We don't technically validate the string, e.g., mismatched tags will
# go unnoticed here. The real validator at Crossref will catch whatever.
###############################################################
sub TitleCheck {
    my $title = shift;
    my $orig_title = $title;
    
    foreach my $tag (qw(b em i ovl scp strong sub sup tt u)) {
        $title =~ s,<\s*/?$tag\s*>,,g; # eradicate <tag> and </tag>
    }

    # <font> can (maybe?) take lots of extra attributes:
    $title =~ s,<\s*/?font.*?>,,g;

    # MathML is too complex; just wipe it all out. If there are
    # problems, the real validator at Crossref will complain.
    $title =~ s,<\s*mml:math.*/mml:math\s*>,,g;
    
    # No tags should remain.
    if ($title =~ /</) {
       die "$0: invalid tags remaining in: $title (original: $orig_title)\n";
    }
}

###############################################################
# Simplistic TeX-to-html
# (no-op for rpi text if --input-is-xml was given).
###############################################################
sub SanitizeText {
    my $string = shift;
    return $string if $opts{xi}; # do nothing if --rpi-is-xml
    return SanitizeTextAlways($string);
}

# Split into two functions so we can sanitize bbl but not rpi.
sub SanitizeTextAlways {
    my $string = shift;
   
    # pass user hook subroutine if defined.
    my @hook = (defined(&{"LaTeX_ToUnicode_convert_hook"}))
               ? ("hook" => \&LaTeX_ToUnicode_convert_hook)
               : ();

    # conversion of accented control sequences to characters, etc.
    # Let's use &#uuuu; entities instead of literal UTF-8; Crossref
    # recommends it, and it's easier for postprocessing.
    $string = LaTeX::ToUnicode::convert($string, entities => 1, @hook);
    
    return $string;
}

################################################################
# Printing one author in arg ORIG_AUTHOR, in sequence SEQ.
################################################################
sub PrintAuthor {
    my ($orig_author,$seq) = @_;

    # recognize extra directives, either |organization|
    # or |orcid=<value>|.
    my $organization = 0;
    my $orcid = 0;
    my $author = "";
    my @name_parts = split (/\|/, $orig_author);
    foreach my $np (@name_parts) {
        $np =~ s/^\s*(.*)\s*$/$1/s; # remove leading and trailing whitespace
        if ($np eq "organization") {
            $organization = 1;
        } elsif ($np =~ /^orcid/) {
            ($orcid = $np) =~ s/^orcid\s*=//;
            $orcid =~ s/\s//g; # remove all whitespace from value
            if (! $orcid) {
                warn "$0: ignoring empty orcid specified in: $orig_author\n";
            }
        } elsif (! $np) {
            # silently ignore empty part, as in ||
        } else {
            if ($author) {
                die ("$0: already saw author name `$author', should not"
                     . " have second: $np\n");
            }
            $author = $np;
        }
    }
    
    if ($organization && $orcid) {
        die ("$0: orcid and organization cannot both be present in:"
             . " $orig_author\n");
    }

    # for organizations, nothing to do but output it.
    if ($organization) {
        my $line = SanitizeText($author);
        print OUT <<END;
        <organization>$line</organization>
END
        return;
    }
    
    # what's left is the common case of a person, not an organization.
    print OUT <<END;
        <person_name sequence="$seq" contributor_role="author">
END


    my $person=new BibTeX::Parser::Author ($author);

    if ($person->first) {
        my $line = $person->first;
        $line = SanitizeText($line);
        print OUT <<END;
          <given_name>$line</given_name>
END
    }

    if ($person->last) {
        my $line = SanitizeText($person->last);
        if ($person->von) {
            $line = SanitizeText($person->von)." $line";
        }
        print OUT <<END;
          <surname>$line</surname>
END
    }

    if ($person->jr) {
        my $line = SanitizeText($person->jr);
        print OUT <<END;
          <suffix>$line</suffix>
END
    }

    if ($orcid) {
        print OUT <<END;
          <ORCID>https://orcid.org/$orcid</ORCID>
END
    }

    print OUT <<END;
        </person_name>
END
}

#############################################################
#  Printing citations
#############################################################
sub PrintCitation {
    my $paperhash=shift;

    foreach my $key (keys (%{$paperhash})) {
	my $citation = $paperhash->{$key};
	$citation = SanitizeTextAlways($citation);

	print OUT <<END;
        <citation key="$key"><unstructured_citation>
          $citation
        </unstructured_citation></citation>
END
    }
}

##############################################################
#  Return publication_type attribute for <journal_article>, given $PUBTYPE.
#  https://data.crossref.org/reports/help/schema_doc/4.4.2/schema_4_4_2.html#publication_type.atts
#  
#  If not specified in input, return " publication_type=full_text" since
#  it was hardwired that way before. If set to "omit", return empty
#  string. Else return " publication_type=$PUBTYPE>, if the value
#  is valid. If not, die. (Leading space is so result can be directly used.)
##############################################################
sub GetPublicationType {
    my $pubtype = shift;
    my $ret;

    if (! $pubtype) {
        $ret = "full_text"; 
    } elsif ($pubtype eq "omit") {
        $ret = "";
    } elsif ($pubtype =~ /^(abstract_only|full_text|bibliographic_record)$/) {
        $ret = $pubtype;
    } else {
        die "$0: invalid publication_type: $pubtype\n";
    }
    
    $ret = " publication_type=\"$ret\"" if $ret;
    return $ret;
}

##############################################################
#  Calculating URL. Res Philosophica gets special treatment.
##############################################################
sub GetURL {
    my $paper = shift;

    my $result;
    if ($paper->{paperUrl}) {
	$result = $paper->{paperUrl}

    } elsif ($paper->{doi} =~ m,^10\.11612/resphil,) {
	my $doi = $paper->{doi};
	$result = 'http://www.pdcnet.org/oom/service?url_ver=Z39.88-2004&rft_val_fmt=&rft.imuse_synonym=resphilosophica&rft.DOI='.$doi.'&svc_id=info:www.pdcnet.org/collection';

    } else {
        die ("$0: paperUrl field is required\n  "
             . &debug_hash_as_string("whole hash", $paper));
    }
    
    $result =~ s/&/&#x26;/g; # amp(ersand)
    return $result;
}


##############################################################
#  debug_hash_as_string($LABEL, HASH)
#
# Return LABEL followed by HASH elements, followed by a newline, as a
# single string. If HASH is a reference, it is followed (but no recursive
# derefencing).
###############################################################
sub debug_hash_as_string {
  my ($label) = shift;
  my (%hash) = (ref $_[0] && $_[0] =~ /.*HASH.*/) ? %{$_[0]} : @_;

  my $str = "$label: {";
  my @items = ();
  for my $key (sort keys %hash) {
    my $val = $hash{$key};
    $val = ".undef" if ! defined $val;
    $key =~ s/\n/\\n/g;
    $val =~ s/\n/\\n/g;
    push (@items, "$key:$val");
  }
  $str .= join (",", @items);
  $str .= "}";

  return "$str\n";
}
