#!/usr/bin/env perl -w

#########################################################################
# Written and (C) by Jérôme Lelong <jerome.lelong@gmail.com>            #
#                                                                       #
# This program is free software; you can redistribute it and/or modify  #
# it under the terms of the GNU General Public License as published by  #
# the Free Software Foundation; either version 3 of the License, or     #
# (at your option) any later version.                                   #
#                                                                       #
# This program is distributed in the hope that it will be useful,       #
# but WITHOUT ANY WARRANTY; without even the implied warranty of        #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
# GNU General Public License for more details.                          #
#                                                                       #
# You should have received a copy of the GNU General Public License     #
# along with this program.  If not, see <http://www.gnu.org/licenses/>. #
#########################################################################

use strict;
use warnings;
use File::Basename;
use Cwd;
use Getopt::Long;
# recognised math environments
my @have_star_modes = qw( equation eqnarray align multline table gather);
my $have_star_mode = join('|', @have_star_modes);
$have_star_mode = "($have_star_mode)";


my $TEX = 'pdflatex';
my $TEXOPTIONS = ' -interaction nonstopmode ';
my $USERTEXOPTIONS = '';
my $RUNQUIET = 0;
my $RUNDEBUG = 0;
my $PARSEONLY = 0;

# Create a hash with three keys "str", "line", "file" and returns a
# reference to it
#
# Input: 3 args
#   a string
#   a line number
#   a filename
sub new_ref {
    my ($str, $line, $file) = @_;
    my $ref = {};
    $ref->{str}  = $str;
    $ref->{line} = $line;
    $ref->{file} = $file;
    return $ref;
}

# Create a hash with six keys "str", "begin", "end", "star",
# "label_line" and "label" and returns a reference to it.
#
# Input : 7 args
#    a string (the env name)
#    line of beginning
#    line of end
#    has the env a star?
#    line on which the label appears
#    value of the label
#    file name
sub new_math_env {
    my ($str, $begin, $end, $star, $label_line, $label, $file) = @_;
    my $this = {};
    $this->{str}        = $str;
    $this->{begin}      = $begin;
    $this->{end}        = $end;
    $this->{star}       = $star;
    $this->{label_line} = $label_line;
    $this->{label}      = $label;
    $this->{file}       = $file;
    return $this;
}


# Parse .chk file and looks for the environments defined by
# @have_star_modes. The first and last line of the environment are stored
# along with the name of the environment. One last variable is
# used to remember if the star version was used.
#
# Input:
#   a chk file content
#   entries
#   refs
#   labels
# the last 3 args are passed as references and are modified
sub chk_parse {
    my ($txt, $entries, $refs, $labels, $citations, $bibcites) = @_;
    my ($str, $begin, $end, $star, $labelled_env, $label, $file, $entry, $l);

    while (defined($l = shift(@$txt))) {
        if ($l =~ m/^label (.+) line (\d+) file (.+)\n$/o) {
            push(@$labels, new_ref($1, $2, $3));
        }
        elsif ($l =~ m/^ref (.+) line (\d+) file (.+)$/o) {
            push(@$refs, new_ref($1, $2, $3));
        }
        elsif ($l =~ m/^citation (.+) line (\d+) file (.+)$/o) {
            foreach my $c (split(/\s*,\s*/, $1)) {
                ${$citations}{$c} = 1;
            }
        }
        elsif ($l =~ m/^bibcite (.+) line (\d+) file (.+)$/o) {
            push(@$bibcites, $1);
        }
        elsif ($l =~ m/^begin\{$have_star_mode(\**)\} line (\d+) file (.+)$/o) {
            $str          = $1;
            $star         = 1;
            $star         = 0 unless ($2);
            $begin        = $3;
            $file         = $4;
            $label        = "";
            $labelled_env = 0;
            while (defined($l = shift(@$txt))) {
                if ($l =~ m/^label (.+) line (\d+) file (.+)$/o) {
                    push(@$labels, new_ref($1, $2, $3));
                    $label       = $1;
                    $labelled_env = $2;
                }
                elsif ($l =~ m/^ref (.+) line (\d+) file (.+)$/o) {
                    push(@$refs, new_ref($1, $2, $3));
                }
                elsif ($l =~ m/^end\{$str\*{$star}\} line (\d+) file (.+)$/) {
                    $end = $1;
                    $entry =  new_math_env($str, $begin, $end, $star, $labelled_env,
                        $label, $file);
                    push(@$entries, $entry);
                    last;
                }
            }
        }
    }
    return;
}

# Find environments with both labels and stars
# Input : list of entries
sub star_label {
    my ($entries) = @_;
    print "************************************\n";
    print "** Labels in starred environments **\n";
    print "************************************\n";
    foreach my $e (@$entries) {
        printf("-- %-20s line %4d: remove label %s \n",
            $e->{file}, $e->{label_line}, $e->{label})
        if (($e->{star} == 1) && ($e->{label_line} > 0));

        printf("-- %-20s line %4d: consider using a STAR environment\n",
            $e->{file}, $e->{begin})
        if (($e->{star} == 0) && ($e->{label_line} == 0));
    }
    print "\n";
    return;
}

# Find labels without any corrsponding references.
# references must be sorted
sub unused_label {
    my ($labels, $refs) = @_;
    my $found;

    print "*******************\n";
    print "** Unused Labels **\n";
    print "*******************\n";
    foreach my $l (@$labels) {
        $found = 0;
        foreach my $r (@$refs) {
            if ($r->{str} eq $l->{str}) {
                $found = 1;
                last;
            }
            elsif ($r->{str} ge $l->{str}) {
                last;
            }
        }
        printf("-- %-20s line %4d: remove label %s\n",
            $l->{file}, $l->{line}, $l->{str})
        if (!$found);
    }
    print "\n";
    return;
}

# Find bibitems without any corresponding \cite
sub unused_cites {
    my ($bibcites, $citations) = @_;
    print "**********************************\n";
    print "** Uncited Bibliography entries **\n";
    print "**********************************\n";
    foreach my $c (@$bibcites) {
        print("remove bibitem : $c\n") unless exists ${$citations}{$c};
    }
    return;
}

# Find and remove duplicates in an
# array of { str, line } entries.
# Note that the array must be sorted according to str
# Input : ref or label list
# Return the corresponding list
sub rm_duplicate {
    my ($array) = @_;
    my $prev = '___________not_a_true_label______';
    my @uniq_array =
    grep { $_->{str} ne $prev && (($prev) = $_->{str}) } @$array;
    return \@uniq_array;
}

# Display all math envs with their characteristics
# Input args
#  line of beginning
#  line of end
#  starred or not
#  label
sub disp_msg {
    my ($entries) = @_;
    my @entries = @$entries;

    foreach my $e (@entries) {
        print("env $e->{str} \n");
        print("\tbeginning : $e->{begin}\n\tend : $e->{end}\n");
        print("\tstar environment\n") if ($e->{star} == 1);
        print("\tlabelled environement\n") if ($e->{label} > 0);
        print("\n");
    }
    return;
}

sub parse {
    my ($chkfile) = @_;
    my @entries = ();
    my @refs    = ();
    my @labels  = ();
    my %citations = ();
    my @bibcites  = ();
    my $FIC;

    # Read the whole file to an array because label and end commands
    # generally need to be swapped.
    open($FIC, '<', $chkfile) or die("open: $!");
    my @txt = <$FIC>;
    close($FIC);

    chk_parse(\@txt, \@entries, \@refs, \@labels, \%citations, \@bibcites);

    @labels = sort { $a->{line} cmp $b->{line} } @labels;
    @refs   = sort { $a->{str} cmp $b->{str} } @refs;
    my $uniq_refs = rm_duplicate(\@refs);
    star_label(\@entries);
    #disp_msg( \@entries );
    unused_label(\@labels, $uniq_refs);
    unused_cites(\@bibcites, \%citations);
}

sub usage {
    print << 'EOT';
Usage: chklref [options] texfile

Check unused labels and bibitems in a LaTeX file

Options:
  --tex <compiler>, -t     Specify the TeX compiler to be used. Default is `pdflatex`.
  --tex-options            List of options to pass to the TeX compiler. It should be a quoted
                           string of white space delimited options. Note that we always add
                           `-interaction nonstopmode` on top of these options.
  --debug, -d              Run in debug mode. Do not clean the generated `.chk` file.
  --quiet, -q              Run in quiet mode. Do not print the output of the TeX compiler.
  --parse-only             Do not run the LaTeX compiler but use the already existing `.chk` file.
                           When this option is passed, the following other options are meaningless:
                           `--tex`, `--tex-options`, `--quiet`, `--debug`.
  --version, -v            Print the version of this script.
  --help,h                 Print this help.


Report bugs to https://github.com/jlelong/chklref. `chklref` is known not to work with `cleveref`. If you would like to contribute to `chklref`, feel free to open a PR on https://github.com/jlelong/chklref.
EOT
    exit(0)
}

sub version {
    print << 'EOT';
chklref 3.1.2

Copyright 2005-2019 Jerome Lelong <jerome.lelong@gmail.com>.
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under GPLv3.

Written by Jerome Lelong <jerome.lelong@gmail.com>
EOT
    exit(0)
}

sub texcompile {
    my ($texfile, $basetexfile) = @_;
    print "Running $TEX $TEXOPTIONS $USERTEXOPTIONS \"$texfile\" to collect labels references and environment declarations\n\n";
    my $texoutput = `$TEX $TEXOPTIONS $USERTEXOPTIONS "$texfile" 2>&1`;
    print($texoutput) unless $RUNQUIET;
    unlink($basetexfile . '.chk');
    $texoutput = `$TEX $TEXOPTIONS $USERTEXOPTIONS --jobname "$basetexfile" '\\RequirePackage{chklref}\\input' "{$texfile}"  2>&1`;
    print($texoutput) unless $RUNQUIET;
}

Getopt::Long::Configure ("bundling");
GetOptions (
   'version|v!' => \&version,
   'quiet|q!' => \$RUNQUIET,
   'debug|d!' => \$RUNDEBUG,
   'tex|t=s' => \$TEX,
   'texoptions=s' => \$USERTEXOPTIONS,
   'parse-only' => \$PARSEONLY,
   'help|h' => \&usage
) || usage();

if (@ARGV != 1) {
    usage();
    exit 0;
}

my $texfile = $ARGV[0];
my $cwd = getcwd();
my $dirtexfile = dirname($texfile);
my $basetexfile = basename($texfile, '.tex');
my $chkfile = $basetexfile . '.chk';
chdir($dirtexfile);
if ($PARSEONLY) {
    parse($chkfile);
}
else {
    texcompile($texfile, $basetexfile);
    parse($chkfile);
    if (! $RUNDEBUG) {
        unlink($chkfile);
    }
}