# Used to convert chktexrc.in (which see) into chktexrc and ChkTeXRC.tex

use strict;
use warnings;

my $chktexrc_output;
if ( $ARGV[0] eq '--chktexrc' ) {
    $chktexrc_output = 1;
} elsif ( $ARGV[0] eq '--latex' ) {
    $chktexrc_output = 0;
} else {
    exit 1;
}


# Filter the lines of the file
open(my $fh, '<:encoding(UTF-8)', $ARGV[1])
    or die "Could not open file '$ARGV[1]' $!";

while ( <$fh> ) {
    if (/^#### END$/) {
        exit 0;
    }

    if ($chktexrc_output) {
        # chktexrc output, so discard all lines over 2
        next if /^#{3,}/;

        s/\@verb\@/`/g;
        s/\@endverb\@/'/g;
        s/\@emph\@/*/g;
        s/\@endemph\@/*/g;
        s/\@ref\@/ /g;
        s/\@endref\@//g;

        s/\@TeX@/TeX/g;
        s/\@LaTeX@/LaTeX/g;
        s/\@ChkTeX@/ChkTeX/g;
        s/\@\\{2}\@//g;         # \\ in LaTeX
        s/\\\@ */ /g;           # \@ in LaTeX
        s/\@bf //g;
        s/\@\&\@//g;
        print;

    } else {

        # LaTeX output, so discard lines except with 1,3
        next if /^#{4,}/;
        next if /^#{2} /;
        # next unless /^#/;
        s/^#+[ ]*//;

        s/\@emph\@/\\emph{/g;
        s/\@endemph\@/}/g;
        s/\@ref\@(.*)\@endref\@/~\\hyperref[rc:$1]{$1}/g;


        s/\@TeX@/\\TeX{}/g;
        s/\@LaTeX@/\\LaTeX{}/g;
        s/\@ChkTeX@/\\chktex{}/g;
        s/\@verb\@/\\verb@/g;
        s/\@endverb\@/@/g;
        s/\@\\{2}\@/\\\\/g;
        s/\@bf/\\bf/g;
        s/\@\&\@/&/g;

        print;
    }
}
# TODO:
# \ChkTeX
