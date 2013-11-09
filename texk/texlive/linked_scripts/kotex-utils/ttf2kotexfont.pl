#!/usr/bin/env perl

# ttf2kotexfont.pl
#
# Copyright (c) 2003-2013 Dohyun Kim <nomos at ktug org>
#
# This work may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either version 1.3c
# of this license or (at your option) any later version.
# The latest version of this license is in
#  http://www.latex-project.org/lppl.txt
# and version 1.3c or later is part of all distributions of LaTeX
# version 2006/05/20 or later.

#use strict;
#use warnings;
use Getopt::Std;
use File::Copy;
use File::Path;

# 저작권 정보 표시
print "ttf2kotexfont v0.9 2007/07/03 \n",
"Copyright (c) 2003-2007 Dohyun Kim <nomos at ktug org>\n",
"Redistribution and use, with or without modification, are permitted\n",
"provided that this coryright notice is retained. If you redistribute\n",
"a modified version, please attach a note about what you have modified.\n\n";

# command line option을 읽어들임
my %args;
getopts("pc:i:",\%args);
unless($args{c}) {
    print <<"ENDOFHELP";
Usage:  $0 -c CONFIGFILE
        $0 -c CONFIGFILE -i TEXMFROOT

  -c CONFIGFILE
        obtain font-related configuration from CONFIGFILE and
        make TFMs and settings for ko.TeX. needs ttf2tfm 1.5 or higher
  -c CONFIGFILE -i TEXMFROOT
        install generated files to TEXMFROOT directory according to TDS

CONFIGFILE syntax example:

    FOUNDRY: xy
    FONTmj: f=bt m=BTmedium.ttf b=BTbold.ttf l=BTlight.ttf
    FONTgt: f=dt m=DTmedium.ttf b=DTbold.ttf
    FONTtz: f=tz m=TZmedium.ttf
    ...
ENDOFHELP
    exit 1;
}

# 사용자 설정파일을 읽어 %FONTS(hash of hash)에 저장
my($foundry,%FONTS);
open FILE,$args{c} or die "$args{c}: $!\n";
while(<FILE>) {
    s/#.*//;
    if(/FOUNDRY:\s*(\w+)/) {
        $foundry = $1;
    }
    elsif(/FONT(\w+?):\s*(.*)/) {
        foreach my $field ( split /\s+/,$2 ) {
            my($key,$value) = split /=/,$field;
            $FONTS{$1}{$key} = $value;
        }
    }
}
close FILE;

# 사용자 설정을 체크
error("FOUNDRY must have 1 or 2 byte length.")
    if(!$foundry or length($foundry) >2);

error("`m' field of FONTmj is not defined in `$args{c}'.")
    unless $FONTS{mj}{'m'}; # \mjdefault는 반드시 있어야 함

foreach my $hlatex (keys %FONTS) {
    # FONTxx의 xx는 영문 2글자만 허용
    error("`$hlatex' of `FONT$hlatex' must have 2 or 3 byte length.")
        unless(length($hlatex) == 2 or length($hlatex) == 3);

    # \textrm 같은 라텍 고유 명령을 재정의하는 것을 방지
    error("`FONT$hlatex' is not a valid name.") if $hlatex =~
        /^(rm|sf|tt|md|bf|up|it|sc|sl|mu|pm|bb|yen|bar|kra|mho|ohm|won|div)$/;

    # f 필드가 지정되지 않았다면 FONTxx의 xx를 f 필드값으로 사용
    $FONTS{$hlatex}{f} = $hlatex unless $FONTS{$hlatex}{f};
}


# define global variables
my $ttf2tfm = 'ttf2tfm';
#####   $ttf2tfm = 'echo'; 
my $styname = $foundry.'ttf.sty';
my $pkmap = $foundry.'ttf2pk.map';
my $pkcfg = 'ttf2pk.cfg';
my $cidmap = 'cid-'.$foundry.'ttf.map';
my $dvipdfmxcfg = 'dvipdfmx.cfg';
my $pdfmap = $foundry.'ttf-pdftex.map';
my $pdfcfg = 'pdftex.cfg';
my %SERIES = (
        'm'=>['m','c'],
        'b'=>['b','bc'],
        'l'=>['l'],
        );
my $slant = 'o';
my $cshape = '-e .92';
my $oshape = '-s .167';
my $bshape = '-b .15'; # for dvipdfmx fakebold feature
my @Wsh = qw/w s h/;
my $sansfonts = 'jgt|jsr|jnv|gt|tz|pg|gr|bm|yt|gl'; # for curemaphangulfamily()

my(@StyMapHangul,@StyOther,@CidMap,@PkMap,@PdfMap);


# variables for HLaTeX
my $prefix = '';
my $hlatexencoding = 'H';
my $cmap = 'UniKSCms-UCS2-H';
my $sfd = 'UKS-HLaTeX';
my $opt = '-q -L KS-HLaTeX -P 3 -E 1';
my $wancmap = 'KSCms-UHC-H';
my $wansfd = 'KS-HLaTeX';
my $wanopt = '-q -l -P 3 -E 5';
my $texteststr = '^^b0^^a1^^b2^^e5^^b5^^cb^^b8^^b1^^ba^^f5^^bd^^db'.
'^^c0^^c1^^c3^^a7^^c5^^eb^^c8^^d1 ^^a8^^b1^^a1^^db^^aa^^a2^^a5^^d2'.
' ^^ca^^a1^^cc^^e5^^cf^^cb^^d2^^b1^^d4^^f5^^d7^^db^^da^^c1^^dd^^a7'.
'^^df^^eb^^e2^^d1^^e5^^b7^^e7^^fb^^ea^^e1^^ed^^c7^^f0^^ad^^f2^^f1'.
'^^f5^^d7^^f8^^bd^^fb^^a3^^fd^^e7';
my $texstyname = 'hfont';
my $euctexname = 'testeuc.tex';
my $eucmjdefault = 'wmj';
my $eucgtdefault = 'wgt';
my $euctzdefault = 'wtt';
my $testtexname = $euctexname;
my $encoding = $hlatexencoding ;

domainprocess() unless $args{i};

$prefix = 'o';
my $ucsencoding = 'LUC';
$cmap = 'unicode'; # dvipdfmx 20040912 up is recommended
$sfd = 'UCS2';
$opt = '-q -P 3 -E 1';
$wancmap = 'KSCms-UHC-H';
$wansfd = "UKS-UCS2";
$wanopt = '-q -P 3 -E 5';
$texteststr = '\\unihangulchar{47924}\\unihangulchar{44417}'.
    '\\unihangulchar{54868} \\unihangulchar{44867}\\unihangulchar{51060} '.
    '\\unihangulchar{54588}\\unihangulchar{50632}\\unihangulchar{49845}'.
    '\\unihangulchar{45768}\\unihangulchar{45796}. \\unihangulchar{12298}'.
    '\\unihangulchar{12593}\\unihangulchar{12800}\\unihangulchar{12896}'.
    '\\unihangulchar{12299} \\unihangulchar{19968}\\unihangulchar{20108}'.
    '\\unihangulchar{19977}\\unihangulchar{22235}';
$texstyname = 'dhucs';
my $utftexname = 'testutf.tex';
my $utfmjdefault = 'unbt';
my $utfgtdefault = 'ungt';
my $utftzdefault = 'untz';
$testtexname = $utftexname ;
$encoding = $ucsencoding ;

domainprocess() unless $args{i};

doremainderprocess() unless $args{i};

exit 0 unless $args{i};

my $texmf = $args{i};
my $ttfd = "$texmf/fonts/truetype/kotex/${foundry}ttf";
my $latexd = "$texmf/tex/latex/kotex/${foundry}ttf/euc";
my $styd = "$texmf/tex/latex/kotex/${foundry}ttf";
my $dhucsd = "$texmf/tex/latex/kotex/${foundry}ttf/utf";
my $euctfmd = "$texmf/fonts/tfm/kotex/${foundry}ttf/euc";
my $utftfmd = "$texmf/fonts/tfm/kotex/${foundry}ttf/utf";
my $ttf2pkd = "$texmf/fonts/map/ttf2pk/kotex/${foundry}ttf";
my $dvipdfmd = "$texmf/fonts/map/dvipdfm/kotex/${foundry}ttf";
my $pdftexd = "$texmf/fonts/map/pdftex/kotex/${foundry}ttf";
my $eucpdftexencd = "$texmf/fonts/enc/kotex/${foundry}ttf/euc";
my $utfpdftexencd = "$texmf/fonts/enc/kotex/${foundry}ttf/utf";
my $pdfcfgd = "$texmf/pdftex/config";
my $ttf2pkcfgd = "$texmf/ttf2pk/base";
my $dvipdfmxcfgd = "$texmf/dvipdfmx";

installfiles();

sub installfiles {
    error("Cannot find $styname, $pkmap or $cidmap in current directory.\n".
        "Run $0 without -i option before installing.")
        unless(-e $styname and -e $pkmap and -e $cidmap);


    unless(mkpath($texmf)) {
        print
        "Some error occured while making $texmf.\n",
        "It seems that the directory already exists.\n",
        "Installing into an existing directory is very DANGEROUS!\n",
        "Any file of the same name will be overwritten without notice.\n",
        "Do you really want to install files into $texmf? [yes/no]: ";
        my $answer = <STDIN>;
        exit 1 unless $answer =~ /yes/;
    }

    mymove($styd,$styname);
    mymove($ttf2pkd,$pkmap);
    mymove($dvipdfmd,$cidmap);
    mymove($pdftexd,$pdfmap) ;

    foreach my $hlatex (keys %FONTS) {
        next unless $FONTS{$hlatex}{f};
        my $font = $foundry.$FONTS{$hlatex}{f};
        my $ttcount = 0;

        foreach my $mbl (keys %SERIES) {
            my $ttf = $FONTS{$hlatex}{$mbl};
            ($ttf) = parsettf($ttf);
            next unless $ttf;
            ++$ttcount;

            mymove("$ttfd",$ttf) if -e $ttf;

            if ($args{p}) {
                mymove("$eucpdftexencd/$font",
                    glob("$font$SERIES{$mbl}[0]*.enc"));
                mymove("$utfpdftexencd/$font",
                    glob("$prefix$font$SERIES{$mbl}[0]*.enc"));
            }

            foreach my $ser (@{$SERIES{$mbl}}) {
                mymove("$euctfmd/$font",glob("$font$ser*.tfm"));
                mymove("$utftfmd/$font",glob("$prefix$font$ser*.tfm"));
            }
        } # loop for ttfs ends here

        if($ttcount) {
            foreach my $wsh (@Wsh) {
                mymove($latexd,"$hlatexencoding$wsh$font.fd");
            }
            mymove($dhucsd,"$ucsencoding$font.fd")
                if -e "$ucsencoding$font.fd";
        }
    } # loop for families ends here

    addtocfg("f $cidmap",$dvipdfmxcfg,$dvipdfmxcfgd) or print
        "Failed!\n  Add `f $cidmap' at the end of $dvipdfmxcfg manually.\n";

    addtocfg("map +$pkmap",$pkcfg,$ttf2pkcfgd) or print
        "Failed!\n  Add `map +$pkmap' at th end of $pkcfg manually.\n";

    if($args{p}) {
        addtocfg("map +$pdfmap",$pdfcfg,$pdfcfgd) or print
            "Failed!\n  Add `map +$pdfmap' at the end of $pdfcfg manually.\n";
    }

    print
        "\nThe followings have to be done manually:\n",
        "1. register `$texmf' as the first TEXMF root directory\n",
        "   in texmf.cnf or in MikTeX Options.\n",
        "2. run `mktexlsr'.\n\n",
        "Happy TeX'ing!\a\n";
}

sub mymove {
    my($dest,@Files) = @_;

    mkpath $dest;
    foreach my $file (@Files) {
        print "$dest/$file\n";
        move($file,$dest) or die "$file: $!\n";
    }
}   


sub doremainderprocess  {
    if(@PdfMap) {
        open MAP,">$pdfmap" or die "$pdfmap: $!\n";
        print MAP @PdfMap;
        close MAP;
    }

    open MAP,">$cidmap" or die "$cidmap: $!\n";
    print MAP @CidMap;
    close MAP;

    open MAP,">$pkmap" or die "$pkmap: $!\n";
    print MAP @PkMap;
    close MAP;

    printstyfile(@StyMapHangul,@StyOther);

    unless(addtocfg("f $cidmap",$dvipdfmxcfg,'.')) {
        open CFG,">$dvipdfmxcfg" or die "$dvipdfmxcfg: $!\n";
        print CFG "f $cidmap\n";
        close CFG;
    }
    unless(addtocfg("map +$pkmap",$pkcfg,'.')) {
        open CFG,">$pkcfg" or die "$pkcfg: $!\n";
        print CFG "map +$pkmap\n";
        close CFG;
    }
    if(@PdfMap and $args{p} and !addtocfg("map +$pdfmap",$pdfcfg,'.')) {
        open CFG,">$pdfcfg" or die "$pdfcfg: $!\n";
        print CFG "output_format 1\n",
              "page_width 210 true mm\n",
              "page_height 297 true mm\n",
              "horigin 1 true in\n",
              "vorigin 1 true in\n",
              "map psfonts.map\n",
              "map +$pdfmap\n";
        close CFG;
    }

    ### cure maphangul
    @StyMapHangul = curemaphangulfamily(@StyMapHangul);
    print "\nTried to cure MapHangulFamily in $styname.\n",
          "Edit $styname to your taste :\n",
          @StyMapHangul;
    move($styname,"$styname.orig") or
        warn "Couldn't rename $styname to $styname.orig: $!\n";
    printstyfile(@StyMapHangul,@StyOther);

    $euctexname =~s/\.tex$//;
    $utftexname =~s/\.tex$//;
    print "\nAll done ...\n",
          "Try `latex $euctexname && dvipdfmx $euctexname' for testing.\n",
          "Try `latex $utftexname && dvipdfmx $utftexname' for testing.\a\n";

}


sub domainprocess {
    my @TestTeX;

    foreach my $hlatex (keys %FONTS) { # loop for each FONTxx families
        my $family = $FONTS{$hlatex}{f};
        my $font = $prefix.$foundry.$family;
        my(%TeXFd,@UCSFd,$nohangul,$nohanja);

        foreach my $mbl (keys %SERIES) { # loop for TTFs
            my($iswan,$isttc,$cidmapcnt,$ttf2pkcnt);
            my $ttf = $FONTS{$hlatex}{$mbl};
            next unless $ttf;
            ++$isttc if $ttf =~ /\.ttc\b/i;
            $args{p} = 0 if $isttc;

            ($iswan,$cidmapcnt,$ttf2pkcnt) =
                maketfmandmap($font,$ttf,@{$SERIES{$mbl}});

            $mbl eq 'b' and $ttf eq $FONTS{$hlatex}{m} and
                @{$cidmapcnt} = addfakeboldoption(@{$cidmapcnt});

            push @CidMap,@{$cidmapcnt};
            push @PkMap,@{$ttf2pkcnt};
            if($args{p}) {
                push @PdfMap,getpdfmapdata($font.$SERIES{$mbl}[0],$ttf)
                    unless $isttc;
            }
            else {
                push @PdfMap,$font,$SERIES{$mbl}[0],"\@",
                    $iswan ? $wansfd : $sfd,
                    "\@\t<$ttf\tPidEid=3,",
                    $iswan ? "5" : "1",
                    "\n" unless $isttc;
            }

            if($prefix) {
                -e "$font${mbl}ac.tfm" or ++$nohangul;
                -e "$font${mbl}4e.tfm" or ++$nohanja;
                push @UCSFd,getucsfd($font,@{$SERIES{$mbl}});
            }
            else {
                foreach my $wsh (@Wsh) { # w/s/h 각 fd 내용을 %TeXFd에 저장
                    push @{$TeXFd{$wsh}},gettexfd($font,$wsh,@{$SERIES{$mbl}});
                }
            }

            push @TestTeX,gettexfilecnt($font,$mbl,$ttf,$hlatex);
        } # loop for ttfs ends here

        my $fdname = $font; $fdname =~ s/^$prefix//;

        if($prefix and @UCSFd) {
            open FD,">$encoding$fdname.fd" or
                die "$encoding$fdname.fd: $!\n";
            print FD "\\DeclareFontFamily{$encoding}{$fdname}",
                  "{\\hyphenchar\\font\\m\@ne}\n",@UCSFd;
            close FD;
        }
        else {
            foreach my $wsh (keys %TeXFd) { # %TeXFd를 각 w/s/h .fd파일에 쓰기
                open FD,">$encoding$wsh$font.fd" or
                    die "$encoding$wsh$font.fd: $!\n";
                print FD "\\DeclareFontFamily{$encoding}{$wsh$font}",
                      "{\\hyphenchar\\font\\m\@ne}\n", @{$TeXFd{$wsh}};
                close FD;
            }
        }

        if(!$prefix and keys %TeXFd) { 
            push @StyMapHangul,
                 "\\MapHangulFamily{$family}{$font,$font,$font}\n";
            if($hlatex =~ /^(mj|gt|tz)$/) {
                push @StyOther,
                    "\\renewcommand\\${hlatex}default{$family}\n";
            }
            else {
                push @StyOther, "\\DeclareRobustCommand\\${hlatex}family",
                     "{\\hfontfamily{$family}}\n",
                     "\\DeclareTextFontCommand\\text$hlatex",
                     "{\\${hlatex}family}\n";
            }
        }

        if($prefix) {
            if($hlatex eq 'mj') {
                push @StyOther,
                    "\\def\\dhucs\@serifhangulfont\{",
                    $nohangul ? $utfmjdefault : $fdname, 
                    "\}\\def\\dhucs\@serifhanjafont\{",
                    $nohanja ? $utfmjdefault : $fdname,
                    "\}\n";
            }
            elsif($hlatex eq 'gt') {
                push @StyOther,
                    "\\def\\dhucs\@sanshangulfont\{",
                    $nohangul ? $utfgtdefault : $fdname, 
                    "\}\\def\\dhucs\@sanshanjafont\{",
                    $nohanja ? $utfgtdefault : $fdname,
                    "\}\n";
            }
            elsif($hlatex eq 'tz') {
                push @StyOther,
                    "\\def\\dhucs\@monohangulfont\{",
                    $nohangul ? $utftzdefault : $fdname, 
                    "\}\\def\\dhucs\@monohanjafont\{",
                    $nohanja ? $utftzdefault : $fdname,
                    "\}\n";
            }
        }
    } # loop for families ends here

    unless($prefix) {
        push @StyOther,
             "\\DeclareErrorHFont{$encoding}{\\mjdefault}{m}{n}{10}\n",
             "\\DeclareHFontSubstitution{$encoding}{\\mjdefault}{m}{n}\n",
             "\\else\n" ;
    }

    printtesttexfile(@TestTeX);

}

sub printtesttexfile {
    my @texcnt = @_;
    open TEX,">$testtexname" or die "$testtexname: $!\n";
    print TEX
        "\\documentclass[a4paper]{article}\n",
        "\\usepackage{",$texstyname,",${foundry}ttf}\n",
        "\\usepackage{ifpdf}\n",
        "\\ifpdf",
        "\\expandafter\\ifx\\csname pdfmapfile\\endcsname\\relax\\else\n",
        $prefix ? "\\pdfmapfile{=unttf-pdftex-dhucs.map}": "",
        "\\pdfmapfile{=$pdfmap}\n",
        "\\fi\\fi\n",
        "\\begin{document}\n",
        @texcnt,
        "\\end{document}\n";
    close TEX;
}


sub printstyfile {
    my(@stydata) = @_;
    
    open STY,">$styname" or die "$styname: $!\n";
    print STY "%%% $styname\n", 
          "%%%\n",
          "%%% Automatically created by $0\n",
          "%%% at ",scalar localtime,"\n",
          "%%%\n",
          "\\ProvidesPackage{${foundry}ttf}\n",
          "\\expandafter\\ifx\\csname unihangulchar\\endcsname\\relax\n",
          @stydata,
          "\\fi\n",
          "\\def\\hfontfamilynameprefix{$foundry}\n",
          "\\endinput\n";
    close STY;
}



sub maketfmandmap {
    my($font,$ttf,@Series) = @_;
    my($iswan,$findex,@cidmapdata,@ttf2pkdata);
    ($ttf,$findex) = parsettf($ttf);

    foreach my $ser (@Series) { # eg. for 'm' and 'c'
        foreach my $sla ('',$slant) { # for null and 'o'
            my $font = $font.$ser.$sla;
            my ($tfmcmd,$myopt,$mysfd);

            my $shape = '';
            $shape .= " $cshape" if $ser =~ /c/;
            $shape .= " $oshape" if $sla eq 'o';

            unless($iswan) {
                $myopt = "$opt$shape -f $findex";
                $mysfd = $sfd;
            }
            else {
                $myopt = "$wanopt$shape -f $findex";
                $mysfd = $wansfd;
            }

            $myopt .= ' -w' if($args{p} and $shape eq '');

            $tfmcmd = "$ttf2tfm $ttf $myopt $font\@$mysfd\@";
            print "$tfmcmd\n";

            my $ttf2tfmresult = `$tfmcmd`;
            if($? >> 8){ # ttf2tfm이 실패했다면 완성 인코딩 옵션으로 재시도
                $myopt = "$wanopt$shape -f $findex";
                $myopt .= ' -w' if($args{p} and $shape eq '');
                $tfmcmd = "$ttf2tfm $ttf $myopt $font\@$wansfd\@";
                print "... trying wansung ttf option ...\n$tfmcmd\n";
                $ttf2tfmresult = `$tfmcmd`;
                $? >> 8 and die; # 이번에도 실패하면 프로그램 종료
                ++$iswan; # 성공했다면 완성형 TTF
            }
            push @ttf2pkdata,$ttf2tfmresult;

            my @tmp = ( "$font\@",
                        $iswan ? $wansfd : $sfd,
                        "\@\t",
                        $iswan ? $wancmap : $cmap,
                        "\t",
                        $findex ? ":$findex:" : "",
                        "$ttf$shape\n");
            push @cidmapdata, join '',@tmp;
        }
    }

    return($iswan,\@cidmapdata,\@ttf2pkdata);
}


sub parsettf {
    my($ttf) = @_;
    my $findex = 0;

    if($ttf =~ /\((\d)\)/) {
        $findex = $1;
        $ttf =~ s/\($1\)//;
    }

    if($ttf =~ /\[([\w\-_\.]+?)\]/) { # old version compatible
        $ttf =~ s/\[$1\]//;
    }

    return($ttf,$findex);
}


sub addfakeboldoption {
    my @cidmaplines = @_;
    for(@cidmaplines) {
        s/$/ $bshape/;
    }
    return @cidmaplines;
}


sub getpdfmapdata {
    my($font,$ttf) = @_;
    my(@Data,@PLANES);
    ($ttf) = parsettf($ttf);

    if($prefix) {
        for my $i(0 .. 255) {
            $PLANES[$i] = sprintf "%02x",$i;
        }
    }
    else {
        @PLANES = ("04".."07",0 .. 29);
    }

    foreach my $plane (@PLANES) {
        push @Data,"$font$plane\t<$font$plane.enc\t<$ttf\n"
            if -e "$font$plane.enc";
    }

    return @Data;
}


sub gettexfd {
    my($font,$wsh,@Series) = @_;
    my @texfd;

    foreach my $ser (@Series) {
        push @texfd,"\n",
             "\\DeclareFontShape{$encoding}{$wsh$font}{$ser}{n}",
             "{<->$font$ser}{}\n",
             "\\DeclareFontShape{$encoding}{$wsh$font}{$ser}{sl}",
             "{<->$font$ser$slant}{}\n",
             "\\DeclareHFontShape{$encoding}{$wsh$font}{$ser}{it}",
             "{<->hssub*$wsh$font/$ser/sl}{}\n";
    }

    return @texfd;
}

sub getucsfd {
    my($ff,@Series) = @_;
    my $font = $ff;
    $font =~ s/^$prefix//;
    my @ucsfd;

    foreach my $ser (@Series) {
        push @ucsfd,"\n",
             "\\DeclareFontShape{$encoding}{$font}{$ser}{n}",
             "{<-> $encoding * $ff$ser}{}\n",
             "\\DeclareFontShape{$encoding}{$font}{$ser}{sl}",
             "{<-> $encoding * $ff$ser$slant}{}\n",
             "\\DeclareFontShape{$encoding}{$font}{$ser}{it}",
             "{<-> $encoding * $ff$ser$slant}{}\n";
        if($ser eq 'b') {
            my $bxser = 'bx';
            push @ucsfd,"\n",
                 "\\DeclareFontShape{$encoding}{$font}{$bxser}{n}",
                 "{<-> ${encoding}ssub * $font/$ser/n}{}\n",
                 "\\DeclareFontShape{$encoding}{$font}{$bxser}{sl}",
                 "{<-> ${encoding}ssub * $font/$ser/sl}{}\n",
                 "\\DeclareFontShape{$encoding}{$font}{$bxser}{it}",
                 "{<-> ${encoding}ssub * $font/$ser/it}{}\n";
        }
    }
    return @ucsfd;
}


sub gettexfilecnt {
    my($font,$mbl,$ttf,$hlatex) = @_;
    my(@texcnt,$findex);

    ($ttf,$findex) = parsettf($ttf);
    $ttf =~ s/_/\\_/g;
    $ttf .= "($findex)" if $findex;

    push @texcnt,"$font$SERIES{$mbl}[0]($ttf):";
    unless($prefix) {
        push @texcnt,"\\text$hlatex\{";
        push @texcnt,"\\bfseries" if $mbl eq 'b';
        push @texcnt,"\\hfontseries{l}" if $mbl eq 'l';
    }
    else {
        $font=~s/^$prefix//;
        if($hlatex eq 'mj') {
            push @texcnt,"\\textrm\{";
        }
        elsif($hlatex eq 'gt') {
            push @texcnt,"\\textsf\{";
        }
        elsif($hlatex eq 'tz') {
            push @texcnt,"\\texttt\{";
        }
        else {
            push @texcnt,"\{\\SetAdhocFonts{$font}{$font}";
        }
        push @texcnt,"\\bfseries" if $mbl eq 'b';
        push @texcnt,"\\fontseries{l}" if $mbl eq 'l';
    }
    push @texcnt,"\\\\$texteststr\}\n\n";

    return @texcnt;
}


sub addtocfg {
    my($line,$cfg,$destdir) = @_;
    print "`$line' -> $destdir/$cfg ... ";

    # 현재디렉토리의 $cfg는 kpsewhich의 작업에 방해가 됨
    if(-e $cfg) {
        move($cfg,"$cfg.exa") or return 0;
    }

    # kpsewhich가 성공하길 기원하자
    my $progname = $cfg;
    # $progname =~ s/dvipdfmx/dvipdfm/;
    $progname =~ s/\.cfg//;
    my $kpathcfg =
        `kpsewhich -format="other text files" -progname=$progname $cfg`;
    chomp $kpathcfg;
    return 0 unless $kpathcfg;

    # 발견된 $cfg를 읽어들임
    # 단, 추가하려는 줄과 같은 줄이 있으면 무시
    my @Lines = ();
    open(CFG,$kpathcfg) or return 0;
    while(<CFG>) {
        next if /^\Q$line/;
        next if /^% ${foundry}ttf -- added by/;
        push @Lines,$_;
    }
    close CFG;

    # .cfg를 $destdir에 쓰고 맨끝에 $line 추가
    mkpath $destdir;
    open(CFG,">$destdir/$cfg") or return 0;
    print CFG @Lines,"\n\% ${foundry}ttf -- added by $0\n$line\n";
    close CFG;

    # 여기까지 도달하면 성공
    print "Success.\n  But check out the file.\n";
    return 1;
}


sub curemaphangulfamily {
    my(@maphangul) = @_;
    my $mjfamily = $FONTS{mj}{f};
    my $gtfamily = $FONTS{gt}{f} || $mjfamily;
    my @serifcured = ($mjfamily,$mjfamily,$mjfamily);
    my @sanscured = ($gtfamily,$gtfamily,$gtfamily);
    my $texlog = $euctexname; $texlog =~ s/\.tex/\.log/;
    my(@ERRFNT,%SFONTS);

    # latex을 돌려 에러난 폰트를 @ERRFNT에 저장
    system("latex -interaction=batchmode -no-mktex=tfm $euctexname");
    open LOG,$texlog or return @maphangul;
    while(<LOG>) {
        if(/^! Font H\/(\w)(\w+?)\/.* \(TFM\) /) {
            push @{$ERRFNT[0]}, $2   if $1 eq 'w';
            push @{$ERRFNT[1]}, $2   if $1 eq 's';
            push @{$ERRFNT[2]}, $2   if $1 eq 'h';
        }
    }
    close LOG;

    # @ERRFNT에서 중복항목을 없앰
    foreach my $i (0..2) {
        my %seen = ();
        foreach (@{$ERRFNT[$i]}) {
            $seen{$_}++;
        }
        @{$ERRFNT[$i]} = keys %seen;
    }

    # @maphangul에서 %SFONTS 추출
    foreach (@maphangul) {
        if(/\\MapHangulFamily\{(\w+)\}\{(\w+),(\w+),(\w+)\}/) {
            push @{$SFONTS{$1}},$2,$3,$4;
        }
    }

    # FONTmj에 에러가 있으면 uhc 이용
    foreach my $i (0..2) {
        foreach my $errfnt (@{$ERRFNT[$i]}) {
            $serifcured[$i] = 'type1uhc' if $errfnt eq "$foundry$mjfamily";
        }
    }

    # FONTgt에 에러가 있으면 FONTmj 이용
    if($gtfamily ne $mjfamily) {
        foreach my $i (0..2) {
            foreach my $errfnt (@{$ERRFNT[$i]}) {
                $sanscured[$i] = 'type1uhc'
                    if $errfnt eq "$foundry$gtfamily";
            }
        }
    }

    # %SFONTS와 @ERRFNT를 비교하여
    # sans계열에 에러 있으면 FONTgt이용, serif계열이면 FONTmj이용
    foreach my $fam (keys %SFONTS) {
        foreach my $i (0..2) {
            foreach my $errfnt (@{$ERRFNT[$i]}) {
                if($SFONTS{$fam}[$i] eq $errfnt) {
                    if($fam =~ /^($sansfonts)$/) {
                        $SFONTS{$fam}[$i] = 
                            $sanscured[$i] eq 'type1uhc' ?
                            $eucgtdefault :
                            $SFONTS{$sanscured[$i]}[$i];
                    } else {
                        $SFONTS{$fam}[$i] = 
                            $serifcured[$i] eq 'type1uhc' ?
                            $eucmjdefault :
                            $SFONTS{$serifcured[$i]}[$i];
                    }
                }
            }
        }
    }

    # 새로운 @maphangul 리턴
    @maphangul = ();
    foreach my $fam (keys %SFONTS) {
        push @maphangul,"\\MapHangulFamily{$fam}{";
        foreach my $i (0..2) {
            push @maphangul,$SFONTS{$fam}[$i];
            push @maphangul,',' if $i < 2;
        }
        push @maphangul,"}\n";
    }
    return @maphangul;
}


sub error {
    my $message = shift;
    
    print "\n$message\nProgram Aborted!!!\n";
    exit 1;
}

#####
# 2008/09/23. touched by Karnes. change dvipdfmx.cfg location
