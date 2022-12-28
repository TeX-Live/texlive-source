#!/usr/bin/env perl
#
# Copyright 2022 Japanese TeX Development Community <issue@texjp.org>
# You may freely use, modify and/or distribute this file.

use strict;
use warnings;
use 5.010;
use Encode;

foreach $_ (<DATA>) {
    chomp;
    my ($encname, $fname0, $fname1) = split ' ', $_;

    my $src = &make_str($encname, $fname0, $fname1);

    open(my $ofh, '>', $fname0) or die "Cannot open $fname0:$!";
    print $ofh $src;
}


sub make_str ($$;$) {
    my ($encname, $fname0, $fname1) = @_;
    my ($src);

    my ($fnameT) = $fname0;
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
\\input "|cat $fnameT"
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

    Encode::from_to($src, 'utf8', $encname) if ($encname !~ /UTF.*8/i);
    return ($src);

}

__DATA__
UTF-8        fn±×÷§¶-utf8.tex
UTF-8        fn-utf8.tex             fn±×÷§¶-utf8.tex
UTF-8        fn£¥µÆÇñß-utf8.tex      fn±×÷§¶-utf8.tex
UTF-8        fnさざ波-utf8.tex       fn±×÷§¶-utf8.tex
EUC-JP       fnさざ波-euc.tex        fn±×÷§¶-utf8.tex
Shift_JIS    fnさざ波-sjis.tex       fn±×÷§¶-utf8.tex
