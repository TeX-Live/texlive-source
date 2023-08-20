#!/usr/bin/env perl -s
#
# Copyright 2022-2023 Japanese TeX Development Community <issue@texjp.org>
# You may freely use, modify and/or distribute this file.

use strict;
use warnings;
use 5.008;
use Encode;

my $st = 0;
our $windows; # option -windows

foreach $_ (<DATA>) {
    chomp;
    my ($encname, $fname0, $fname1) = split ' ', $_;

    $fname0 = "$ARGV[0]/$fname0" if @ARGV;
    my $src = &make_str($encname, $fname0, $fname1);

    open(my $ofh, '>', $fname0) or die "Cannot open $fname0:$!";
    print $ofh $src;
}

exit($st ? 239 : 0);


sub make_str ($$;$) {
    my ($encname, $fname0, $fname1) = @_;
    my ($src);

    my ($fnameT) = $fname0;
    my $cmnt = $windows ? '%' : ''; # comment out if option -windows
    $fnameT =~ s/\.tex$/-tmp.tex/;

$src = <<END;
\% $fname0
\% $encname encoding
\\catcode`\\{=1
\\catcode`\\}=2

\\immediate\\write16{JOB[\\jobname] :: We are in $fname0}

\\immediate\\openout0=\\jobname.txt
\\immediate\\write0{abc αβγ абв あア※￥ 天地人}
\\immediate\\closeout0

\\immediate\\openout1=$fnameT
\\immediate\\write1{\\relax}
\\immediate\\closeout1

% current directory
$cmnt\\input "|cat $fnameT"
\\input $fnameT
END

$src .= <<END if $fname1;

% vir kpathsearch
\\immediate\\write16{JOB[\\jobname] :: Search $fname1}
\\input $fname1
END

$src .= <<END;

\\relax\\end
END

    if ($encname !~ /UTF.*8/i) {
        my $ret = Encode::from_to($src, 'utf8', $encname);
        if (!$ret) {
            warn "fn-generate.perl: Encode::from_to() failed.\n";
            $st++;
        }
    }
    return ($src);

}

__DATA__
UTF-8        fn±×÷§¶-utf8.tex
UTF-8        fn-utf8.tex             fn±×÷§¶-utf8.tex
UTF-8        fn£¥µÆÇñß-utf8.tex      fn±×÷§¶-utf8.tex
UTF-8        fnさざ波-utf8.tex       fn±×÷§¶-utf8.tex
EUC-JP       fnさざ波-euc.tex        fn±×÷§¶-utf8.tex
Shift_JIS    fnさざ波-sjis.tex       fn±×÷§¶-utf8.tex
