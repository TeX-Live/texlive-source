#!/usr/bin/env perl
# This is texloganalyser, for parsing of TeX logs
# Copyright (c) 2006-2020 Thomas van Oudenhove
# All rights reserved

# README - important notice:
# you should feed the first line with the path of your Perl executable
# (on *nix systems, the output of 'which perl')
#
# (this program works at least with perl 5.8.8)
# Getopt::Long MUST be installed on your system for this program to work

# License:
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

use warnings;
use strict;
use Getopt::Long;
Getopt::Long::Configure ("bundling");

my $version = "0.11";
my $copyright = "2006-2020";
my $opt = {
    help => '',
    version => '',
    last => '',
    a => '',
    h => '',
    o => '',
    u => '',
    v => '',
    c => '',
    p => '',
    f => '',
    w => '',
    e => '',
    i => '',
    s => '',
    r => '',
    t => '',
    n => '',
};

GetOptions(
    'help' => \$opt->{help},
    'version' => \$opt->{version},
    'last' => \$opt->{last},
    'a' => \$opt->{a},
    'c' => \$opt->{c},
    'h' => \$opt->{h},
    'o' => \$opt->{o},
    'u' => \$opt->{u},
    'v' => \$opt->{v},
    'p' => \$opt->{p},
    'f' => \$opt->{f},
    'w' => \$opt->{w},
    'e' => \$opt->{e},
    'i' => \$opt->{i},
    't' => \$opt->{t},
    's' => \$opt->{s},
    'r' => \$opt->{r},
    'n' => \$opt->{n},
);

my $abstract = {
    warnings => 0,
    badboxes => 0,
};

my $texlog = $ARGV[-1];

die "Usage: $0 ".options()." <TeX log>\n"
    if (not defined $texlog and
        (not $opt->{help} and not $opt->{version}));

if ($opt->{help}) {
    printhelp();
    exit();
}

if ($opt->{version}) {
    printversion();
    exit();
}

analyze();

sub analyze {
    my $line;
    my ($cbox, $chead);
    my $output;
    my $test = 0;
    my $png = 0;

    open my $source, "<", $texlog;
    while (defined ($line = <$source>)) {
        undef $output;
        # default: display summary
        summary($line);
        #          and page numbers ('n' option)
        my $pn = pagenumber($line);
        print '['.$pn.']' if ($opt->{n} and $pn);
        $png = $pn if $pn;

        if ($opt->{e}) {
            if ($line =~ m/Here is how much/) {
                $test = 1;
                $output = "\n";
            }
            $output .= $line if $test;
        }
        # look for boxes warnings
        if ($opt->{c} and $opt->{o} and $opt->{h}) {
            if ($line =~ m/\\hbox/ and $line =~ m/Overfull/) {
                $line =~ m/^Overfull \\hbox \((\d+\.\d+)pt too wide\) .*$/;
                $cbox->{$1} = 'Page '.$png.': '.$line;
                $chead = 'Overfull hboxes classified:';
            }
        } elsif ($opt->{c} and $opt->{u} and $opt->{h}) {
            if ($line =~ m/\\hbox/ and $line =~ m/Underfull/) {
                $line =~ m/^Underfull \\hbox \(badness (\d+)\) .*$/;
                $cbox->{$1} = 'Page '.$png.': '.$line;
                $chead = 'Underfull hboxes classified:';
            }
        } elsif ($opt->{o} and $opt->{h}) {
            $output = $line if ($line =~ m/\\hbox/ and
                $line =~ m/Overfull/);
        } elsif ($opt->{u} and $opt->{h}) {
            $output = $line if ($line =~ m/\\hbox/ and
                $line =~ m/Underfull/);
        } elsif ($opt->{o} and $opt->{v}) {
            $output = $line if ($line =~ m/\\vbox/ and
                $line =~ m/Overfull/);
        } elsif ($opt->{u} and $opt->{v}) {
            $output = $line if ($line =~ m/\\vbox/ and
                $line =~ m/Underfull/);
        } elsif ($opt->{h} and $opt->{v}) {
            $output = $line if ($line =~ m/\\vbox/ or
                $line =~ m/\\hbox/);
        } elsif ($opt->{o} and $opt->{u}) {
            $output = $line if ($line =~ m/Underfull/ or
                $line =~ m/Overfull/);
        } elsif ($opt->{h}) {
            $output = $line if ($line =~ m/\\hbox/);
        } elsif ($opt->{v}) {
            $output = $line if ($line =~ m/\\vbox/);
        } elsif ($opt->{o}) {
            $output = $line if ($line =~ m/Overfull/);
            $abstract->{badboxes} += 1;
        } elsif ($opt->{u}) {
            $output = $line if ($line =~ m/Underfull/);
            $abstract->{badboxes} += 1;
        }
        # look for LaTeX Font Info
        if ($opt->{f}) {
            $output = $line if ($line =~ m/Font Info/);
        }
        # packages infos
        if ($opt->{p}) {
            $output = $line if ($line =~ m/Package/);
        }
        # references warnings
        if ($opt->{r}) {
            $output = $line if ($line =~ m/Reference/ or
                $line =~ m/Label(.*)/ and $1 =~ m/multiply defined/);
        }
        # TeX files used
        if ($opt->{t}) {
            scannedtexfiles($line, "tex");
        }
        # sty and cls files used
        if ($opt->{s}) {
            scannedfiles($line, "sty");
            scannedfiles($line, "cls");
        }
        # images (pdf, ps, jpg, png)
        if ($opt->{i}) {
            foreach my $t ('pdf', 'eps', 'ps', 'png', 'jpg') {
                scannedfiles($line, $t);
            }
        }
        # display all warnings
        if ($opt->{w}) {
            $output = $line if ($line =~ m/Warning/);
        }
        # if 'last' option, let's flush data
        if ($opt->{last} and $line =~ m/^This is .*TeX, Version 3\.14159/) {
            # Flush.
            $output = '';
            $abstract->{warnings} = 0;
        }

        print "$output" if defined $output;
    }
    print "\n";
    if ($opt->{c} and $opt->{h} and ($opt->{u} or $opt->{o})) {
        print "$chead\n";
        foreach my $key (sort {$a <=> $b} keys %{$cbox}) {
            print $cbox->{$key};
        }
        print "\n";
    }
    print_abstract();
    close $source;
}

sub pagenumber {
    my $logline = shift;
    if ($logline =~ m/.*\[([0-9]+)(\]|$)/) {
        return $1;
    }
}

sub scannedfiles {
    my ($logline, $filetype) = @_;
    if ($logline =~ m/^\((.*\.$filetype)/i) {
        print "(".$1.")";
    }
}

sub scannedtexfiles {
    my ($logline, $filetype) = @_;
    if ($logline =~ m/\((.*\.$filetype)/i) {
        print "(".$1.")";
    }
}

sub summary {
    my $line = shift;
    # prints log summary ()
    $abstract->{warnings} += 1 if ($line =~ m/Warning/);
}

sub options {
    my $stropts = "[";
    my $multopt = '';
    foreach my $k (sort keys %{$opt}) {
        if ($k eq 'version') {
            $stropts .= "--version|";
        } elsif ($k eq 'help') {
            $stropts .= "--help|";
        } else {
            $multopt .= $k;
        }
    }
    $stropts .= '-'.$multopt.']';
    return $stropts;
}

sub print_abstract {
    print "The log contained ".$abstract->{warnings}." warnings";
    print " and $abstract->{badboxes} bad boxes" if $opt->{a};
    print ".\n";
}

sub printversion {
    print "This is texloganalyser (Perl script), version $version\n";
    print "\t Copyright $copyright Thomas van Oudenhove\n";
    print "\t License: BSD\n";
}

sub printhelp {
    printversion();
    print "\nUsage: texloganalyser ".options()." <TeX log>\n";
    print "\t Displays selective infos of LaTeX logs\n";
    print "\nThese flags may be used:\n";
    print "\t a: displays number of warnings about bad boxes\n";
    print "\t c: displays the boxes warning by size (at the end)\n";
    print "\t e: displays the end of the log\n";
    print "\t f: outputs the LaTeX Font Infos\n";
    print "\t h: outputs only warnings about horizontal boxes\n";
    print "\t i: displays 'images' (pdf, [e]ps, png, jpg) used\n";
    print "\t n: displays page numbers\n";
    print "\t o: outputs only warnings about overfull boxes\n";
    print "\t p: outputs the LaTeX Packages infos\n";
    print "\t r: displays warnings about references\n";
    print "\t s: displays .sty and .cls files used\n";
    print "\t t: displays .tex files used\n";
    print "\t u: outputs only warnings about underfull boxes\n";
    print "\t v: outputs only warnings about vertical boxes\n";
    print "\t w: displays all Warnings\n\n";
    print "More documentation is available with `perldoc texloganalyser`.\n";
}

__END__


=pod

=head1 texloganalyser Documentation

=head2 NAME

texloganalyser -- displays selective infos from TeX log files

=head2 SYNOPSIS

texloganalyser [-acefhinoprstuvw] tex_log_file

texloganalyser [--version]

texloganalyser [--help]

=head2 DESCRIPTION

Various options may be used to select which information from the TeX log you want to see/analyze (see next section). The default is to display only warnings number.

=head2 OPTIONS

=over 1

=item B<--version>: displays version information.

=item B<--help>: displays help as a summary of options.

=item B<--last>: the Log file may content several logs; in this case, consider only the last one.

=item B<-a>: counts number of badboxes.

=item B<-c>: classify boxes warnings by size (needs h&o,u or v&o,u) at the end.

=item B<-e>: displays the end of the log, about the TeX's memory.

=item B<-f>: outputs the LaTeX Font Infos warnings and calculations.

=item B<-h>: outputs only warnings about horizontal boxes.

=item B<-i>: displays 'images' (pdf, [e]ps, png, jpg) used.

=item B<-n>: displays page numbers.

=item B<-o>: outputs only warnings about overfull boxes.

=item B<-p>: outputs the LaTeX Packages infos and warnings.

=item B<-r>: displays warnings about references (missing or multiply-defined).

=item B<-s>: displays .sty and .cls files used.

=item B<-t>: displays .tex files used. This option is B<very> useful when you parse logs of master files, to know in which file to look to correct errors/warnings.

=item B<-u>: outputs only warnings about underfull boxes.

=item B<-v>: outputs only warnings about vertical boxes.

=item B<-w>: displays all TeX, LaTeX and font Warnings.

=back

=head2 BUGS

None yet identified. However, the display should be improved...

=head2 HISTORY

This program was first released in 2006. Its purpose was to display selective information of my PhD's TeX log.

Markus Hennig submitted some new options (-a and -n) at the end of 2009.

Akim Demaille submitted a patch for 'last' option, June 2012.

Paulo Ney de Souza had the idea of -c option to better handle hboxes, March 2020.

=head3 Changelog

=over

=item v0.11: bug fix for -c option (regexp was too specific)

=item v0.10: added option -c, to classify boxes warnings by size (Paulo Ney de Souza's idea).

=item v0.9: added option --last, if log file contains several compilations (Akim Demaille's patch).

=item v0.8: added options for page and bad boxes number (Markus Hennig's patch).

=item v0.7: bug patch for .tex files (-t option)

=item v0.6: default displays number of warnings (and page numbers).

  added option: references and label warnings

=item v0.5: use of GetOpt::Long and code cleaning

  added options: display of files used
  improved documentation

=item v0.4: added features and bugs corrections (options h, v, o and u)

=item v0.3: display of Warnings and last lines of the log

=item v0.2: display of "Font Info" and "Packages" added

=item v0.1: first version, displays only warnings about boxes

=back

=head2 AUTHORS

I<texloganalyser> is a program by Thomas van Oudenhove (L<thomasvo+tex_AT_thomasvo.net>). Feel free to contact him for features requests or bugs.

Markus Hennig submitted a patch, many thanks to him :).

June 2012, Akim Demaille submitted the 'last' option patch, many thanks to him :).

=cut

