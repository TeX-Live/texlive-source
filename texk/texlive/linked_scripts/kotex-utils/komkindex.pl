#!/usr/bin/env perl


# komkindex.pl
#
# Copyright (c) 2007-2015 Dohyun Kim <nomos at ktug org>
#
# This work may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either version 1.3c
# of this license or (at your option) any later version.
# The latest version of this license is in
#  http://www.latex-project.org/lppl.txt
# and version 1.3c or later is part of all distributions of LaTeX
# version 2006/05/20 or later.

###
### TODO: how to distinguish malformed utf8 string in IND file?
###

#use warnings;
#use strict;
use 5.8.0;
use Getopt::Std;

## -euc option
for (@ARGV) {
    if ( /^\-+euc/ ) {
        $_ = '';
        exec ("hmakeindex @ARGV") or die "couldn't exec hmakeindex: $!";
    }
}

binmode STDOUT,":utf8";

my $progname = $0;
$progname =~ s/^.*\///;

print "This is $progname, a makeindex wrapper for ko.TeX package.\n";

@ARGV == 0 and die "\n",
  "Usage: $progname [-euc] [makeindex options] idx_filename[s]\n",
  "       -euc: exec hmakeindex, passing other arguments as they are.\n\n",
  "       $progname [-k] [makeindex options] idx_filename[s]\n",
  "       -k: put hangul index entries before than others.\n\n";


###
### Many thanks to Jinsuk Kim, http://www.jinsuk.pe.kr
###
my @hanja_to_hangul = get_hanja_hangul_table("hanja_hangul.tab");
my @hanjacompat_to_hangul = get_hanja_hangul_table("hanjacom_hangul.tab");
my @hanjaextA_to_hangul = get_hanja_hangul_table("hanjaexa_hangul.tab");

my @cjamo_jamo =
  qw/
     1100 1101 11aa 1102 11ac 11ad 1103 1104
     1105 11b0 11b1 11b2 11b3 11b4 11b5 111a
     1106 1107 1108 1121 1109 110a 110b 110c
     110d 110e 110f 1110 1111 1112 1161 1162
     1163 1164 1165 1166 1167 1168 1169 116a
     116b 116c 116d 116e 116f 1170 1171 1172
     1173 1174 1175 1160 1114 1115 11c7 11c8
     11cc 11ce 11d3 11d7 11d9 111c 11dd 11df
     111d 111e 1120 1122 1123 1127 1129 112b
     112c 112d 112e 112f 1132 1136 1140 1147
     114c 11f1 11f2 1157 1158 1159 1184 1185
     1188 1191 1192 1194 119e 11a1
     /;

my @hangul_grouping =
  qw/
     F785 F788 F790 F798 F7A8 F7AE F7C2 F7D8
     F7D9 F7E7 F7E8 F7F0 F7F5 F7F6 F7F8 F7FC
     F7FF F807 F80C F811 F816 F81B F828 F831
     F83C F846 F84F F85E F86B
     /;
#     ㄱ   ㄴ   ㄷ   ㄹ   ㅁ   ㅂ   ㅅ   ㅿ
#     ㅇ   ㆁ   ㅈ   ㅊ   ㅋ   ㅌ   ㅍ   ㅎ
#     ㆆ   ㅏ   ㅑ   ㅓ   ㅕ   ㅗ   ㅛ   ㅜ
#     ㅠ   ㅡ   ㅣ   ㆍ   종성


# u+115F => U+F784 => U+F801 # 초성채움
my @jamo_puajamo_1100 = qw/
  F785 F786 F788 F790 F792 F798 F7A8 F7AE
  F7B2 F7C2 F7CA F7D9 F7E8 F7EA F7F0 F7F5
  F7F6 F7F8 F7FC F789 F78A F78B F78C F791
  F79B F79E F7A6 F7A7 F7AB F7AD F7AF F7B0
  F7B1 F7B3 F7B4 F7B5 F7B6 F7B7 F7B8 F7BA
  F7BB F7BD F7BE F7C0 F7C1 F7C3 F7C4 F7C5
  F7C6 F7C7 F7C8 F7C9 F7CC F7CD F7CE F7CF
  F7D0 F7D1 F7D2 F7D3 F7D4 F7D5 F7D6 F7D7
  F7D8 F7DA F7DB F7DD F7DE F7DF F7E0 F7E1
  F7E2 F7E3 F7E4 F7E5 F7E7 F7E9 F7EC F7ED
  F7EE F7EF F7F1 F7F2 F7F3 F7F4 F7F9 F7FB
  F7FE F7FF F787 F78D F78E F78F F793 F801
  F806 F807 F80B F80C F810 F811 F815 F816
  F81A F81B F81C F81D F827 F828 F831 F834
  F836 F83A F83C F846 F84D F84F F808 F809
  F80D F80E F812 F813 F814 F818 F819 F820
  F821 F823 F824 F826 F82B F82C F82E F82F
  F830 F832 F833 F835 F838 F839 F83D F83F
  F840 F841 F842 F844 F845 F84B F84C F84E
  F850 F851 F856 F859 F85B F85D F85E F860
  F862 F863 F864 F80A F80F F817 F81E F81F
  F86B F86C F870 F875 F87C F87F F880 F88B
  F88C F895 F899 F89F F8A4 F8A5 F8A6 F8AA
  F8B9 F8BF F8C6 F8CD F8D8 F8E6 F8EA F8EB
  F8EC F8ED F8F2 F86E F871 F876 F878 F87A
  F87B F87E F881 F884 F88E F890 F891 F892
  F893 F896 F897 F89B F89D F89E F8A0 F8A1
  F8A3 F8A7 F8AB F8AE F8B0 F8B2 F8B3 F8B4
  F8B6 F8B7 F8B8 F8BB F8C3 F8C4 F8C5 F8C7
  F8C8 F8C9 F8CB F8D5 F8E1 F8DA F8DD F8E4
  F8E0 F8E2 F8E3 F8EE F8F1 F8F3 F8F4 F8F5
  F8F6 F8F7 F86D F86F F872 F873 F874 F877
  /;

my @jamo_puajamo_A960 = qw/
  F794 F795 F796 F797 F799 F79A F79C F79D
  F79F F7A0 F7A1 F7A2 F7A3 F7A4 F7A5 F7A9
  F7AA F7AC F7B9 F7BC F7BF F7CB F7DC F7E6
  F7EB F7F7 F7FA F7FD F800
  /;

my @jamo_puajamo_D7B0 = qw/
  F822 F825 F829 F82A F82D F837 F83B F83E
  F843 F847 F848 F849 F84A F852 F853 F854
  F855 F857 F858 F85A F85C F85F F861
  /;

my @jamo_puajamo_D7CB = qw/
  F879 F87D F882 F883 F885 F886 F887 F888
  F889 F88A F88D F88F F894 F898 F89A F89C
  F8A2 F8A8 F8A9 F8AC F8AD F8AF F8B1 F8B5
  F8BA F8BC F8BD F8BE F8C0 F8C1 F8C2 F8CA
  F8CC F8CE F8CF F8D0 F8D1 F8D2 F8D3 F8D4
  F8D6 F8D7 F8DB F8E5 F8E7 F8E8 F8E9 F8EF
  F8F0
  /;

###
### parse options
###
my (
    %args,
    @IDXfiles,
    $istfile,
    $indfile,
    $logfile,
    $idx0
   );
getopts('kcgilo:p:qrs:t:LT',\%args);
@IDXfiles = @ARGV or die "No .idx file specified!\n";

if ($args{s}) {
  $istfile = $args{s};
  $istfile =~ s/(\.ist)?$/\.ist/;
}

$idx0 = $IDXfiles[0];
$idx0 =~ s/\.[A-Za-z]+$//;

$indfile = $args{o} ? $args{o} : $idx0 . ".ind";

$logfile = $args{t} ? $args{t} : $idx0 . ".ilg";

foreach my $file (@IDXfiles) {
  $file =~ /\.[A-Za-z]+$/ or $file .= ".idx";
}

###
### read .ist file
###
my $ist_heading_flag = 0;
my $ist_heading_prefix = '';
my $ist_heading_suffix = '';
my $ist_group_skip = '\indexspace';
my $ist_item_0 = '\item ';
my $ist_delim_0 = ', ';
my $ist_keyword = '\indexentry';
my $ist_actual = '@';
my $ist_encap = '|';
my $ist_level = '!';
my $ist_quote = '"';
my $ist_arg_open = '{';
my $ist_arg_close = '}';
my $ist_postamble = '\end{theindex}';

if ($istfile) {
  $istfile = `kpsewhich $istfile`;
  chomp $istfile;
  open IST,"<:utf8",$istfile or die "$args{s} : $!\n";
  while (<IST>) {
    s/\%.*//;
    s/\\n\b//g;
    s/\\\\/\\/g;
    if (/headings_flag\s+(.+)/) {
      $ist_heading_flag = $1;
    } elsif (/heading_prefix\s+"(.*)"/) {
      $ist_heading_prefix = $1;
    } elsif (/heading_suffix\s+"(.*)"/) {
      $ist_heading_suffix = $1;
    } elsif (/group_skip\s+"(.*)"/) {
      $ist_group_skip = $1;
    } elsif (/item_0\s+"(.*)"/) {
      $ist_item_0 = $1;
    } elsif (/delim_0\s+"(.*)"/) {
      $ist_delim_0 = $1;
    } elsif (/keyword\s+"(.*)"/) {
      $ist_keyword = $1;
    } elsif (/actual\s+'(.*)'/) {
      $ist_actual = $1;
    } elsif (/encap\s+'(.*)'/) {
      $ist_encap = $1;
    } elsif (/level\s+'(.*)'/) {
      $ist_level = $1;
    } elsif (/quote\s+'(.*)'/) {
      $ist_quote = $1;
    } elsif (/arg_open\s+'(.*)'/) {
      $ist_arg_open = $1;
    } elsif (/arg_close\s+'(.*)'/) {
      $ist_arg_close = $1;
    } elsif (/postamble\s+"(.*)"/) {
      $ist_postamble = $1;
    }
  }
  close IST;
}


# prepare to run makeindex
my $makeindexoption = "-i -o $indfile -t $logfile";
foreach ('c','l','q','r','L','g','T') {
  $makeindexoption .= " -$_" if $args{$_};
}
$makeindexoption .= " -s $istfile" if $args{s};
$makeindexoption .= " -p $args{p}" if $args{p};

###
### read .idx files
###
my ( @idxarr, %idxhash );
foreach my $file (@IDXfiles) {
  open IDX,"<:utf8",$file or die "$file : $!\n";
  while (<IDX>) {
    s/\^\^([0-9a-f]{2})/pack "U0C", hex $1/ge;
    s/\\unihangulchar\s*\{(\d+)\}/chr $1/ge;

    #    \indexentry{ ..... }{ .. }
    # -> $pre         $body $post
    if (/(\Q$ist_keyword\E\s*\Q$ist_arg_open\E)
         (.*)
         (\Q$ist_arg_close$ist_arg_open\E.+?\Q$ist_arg_close\E)
         $/x) {
      my($pre,$body,$post) = ($1,$2,$3);

      #    \indexentry{ ..... | .. }{ .. }
      # -> $pre         $body $post
      my @xbody = split /(?<!(?<!\\)[\\\Q$ist_quote\E])\Q$ist_encap/,$body;
      for ( my $i=$#xbody; $i>0; $i--) {
        $post = $ist_encap.$xbody[$i].$post;
      }
      $body = $xbody[0];

      # !을 경계로 가름.
      @xbody = split /(?<!(?<!\\)[\\\Q$ist_quote\E])\Q$ist_level/, $body;

      for (@xbody) {
        # @이 없으면... 넣어준다.
        unless (/(?<!(?<!\\)[\\\Q$ist_quote\E])\Q$ist_actual/) {
          $_ = $_.$ist_actual.$_;
        }

        # @을 경계로 가름.
        my @ybody = split /(?<!(?<!\\)[\\\Q$ist_quote\E])\Q$ist_actual\E/, $_;
        $_ = $ybody[0];

        s/(?<!\\)\\[A-Za-z]+\s*//g;
        s/(?<!\\)[{}]//g;

        &hanja_to_hangul;
        s/([\x{AC00}-\x{D7A3}])/syllable_to_jamo_chr($1)/ge;
        s/([\x{3131}-\x{318E}])/cjamo_to_jamo_chr($1)/ge;
        &insert_fillers;
        &compose_jamo;
        &jamo_to_puajamo;

        s/^([\x{80}-\x{F784}\x{F8F8}-\x{FFFF}])/symbolize_first($1)/e;

        $ybody[0] = $_;

        my $tmpkey = $ybody[1];
        $tmpkey =~ s/(?<!\\)\Q$ist_quote\E(.)/$1/g;
        my @tmp = split "",$ybody[0];
        $idxhash{ $tmpkey } = $tmp[0];	# for grouping routine
        if ( $tmp[0] =~ /^\x{F801}/ ) {   # 초성채움.
          $idxhash{ $tmpkey } = $tmp[1];
        }
        if ( $tmp[0] =~ /^\x{F806}/ ) {   # 중성채움.
          $idxhash{ $tmpkey } = $tmp[2];
        }

        $_ = join $ist_actual,@ybody;
      }
      $body = join $ist_level,@xbody;
      push @idxarr,"$pre$body$post\n";
    }
  }
  close IDX;
}

###
### run makeindex
###
open(MAKE,"| makeindex $makeindexoption") or die "Cannot fork: $!\n";
binmode MAKE,":utf8";
print MAKE @idxarr;
close MAKE;
$? >> 8 and die "\nmakeindex failed!\n";

###
### read .ind file and group hangul items
###
my @indarr;
my $indgroup = 0;
# (indgroup) 0 = header;
#            1 = latin;
#            2 = hangul;
#            3 = footer.

my $cho = -1;

open IND,"<:utf8",$indfile or die "$indfile : $!\n";
while (<IND>) {
  if (/\Q$ist_postamble\E/) {
    $indgroup = 3;
  } elsif ($ist_heading_flag and
           /\Q$ist_heading_prefix\E/) {
    if (/\Q$ist_heading_prefix\E.+?\Q$ist_heading_suffix\E/) {
      ### 깨진 글자가 index head로 하나 들어가는데 이를 제거하기 위해
      ### 복잡하게 이중으로 조건검사를 하였다. 깨진 글자는 두번째 조건을
      ### 통과하지 못한다.
      $indgroup++ if $indgroup == 0;
    } else {
      next;
    }
  } elsif (/\Q$ist_item_0\E(.+?)(\Q$ist_delim_0\E|$)/ ) {
    my $body = $1;
    if ( $idxhash{$body} =~ /[\x{F785}-\x{F8F7}]/ ) {
      $indgroup++ if $indgroup == 1;
      my $first = get_hindexhead_num($idxhash{$body});
      if ($first != $cho) {
        my $tmp = '';
        $tmp .= "\n$ist_group_skip\n" if $cho >= 0;
        $tmp .= "$ist_heading_prefix\\hindexhead{$first}$ist_heading_suffix\n"
          if ($ist_heading_flag);
        $_ = $tmp.$_;
        $cho = $first;
      }
    } else {
      $indgroup++ if $indgroup == 0;
    }
  }
  push @{$indarr[$indgroup]} , $_;
}
close IND;


open IND,">:utf8",$indfile or die "$indfile : $!\n";
if ($args{k}) {
  # latin의 마지막 indexspace를 hangul 마지막으로 이동. (-k 옵션에서)
  if ( @{ $indarr[1] } and @{ $indarr[2] } ) {
    my $tmp;
    $tmp = pop @{ $indarr[1] }
      until ( $tmp =~ /\Q$ist_group_skip\E/ );
    push @{ $indarr[2] },$tmp;
  }

  for my $i (0,2,1,3) {
    for my $j (0 .. $#{$indarr[$i]} ) {
      print IND $indarr[$i][$j];
    }
  }
} else {
  for my $i (0..3) {
    for my $j (0 .. $#{$indarr[$i]} ) {
      print IND $indarr[$i][$j];
    }
  }
}
close IND;


########## SUBROUTINES ##########

sub get_hindexhead_num {
  my $fn = ord shift;

  for ( my $i=0; $i<@hangul_grouping; $i++ ) {

    my $divval = $fn / hex($hangul_grouping[$i]);
    if ( $divval < 1 ) {
      return --$i;
    }
  }
}

sub symbolize_first {
  my $first = shift;
  my $symfirst = '';

  for (unpack("U0C*", $first)) {
    $symfirst .= sprintf "^^%02x",$_;
  }

  return $symfirst;
}

sub syllable_to_jamo_chr {
  my $syl  = ord shift;
  my $cho  = ($syl - 0xac00) / (21 * 28) + 0x1100;
  my $jung = ($syl - 0xac00) % (21 * 28) / 28 + 0x1161;
  my $jong = ($syl - 0xac00) % 28;
  if ($jong) {
    $jong += 0x11a7;
    return chr($cho).chr($jung).chr($jong);
  }
  return chr($cho).chr($jung);
}

sub cjamo_to_jamo_chr {
  my $cj = ord shift;
  $cj = hex $cjamo_jamo[$cj - 0x3131];
  if($cj <= 0x115f) { ### choseong: append U+F86A
    return chr($cj).chr(0x1160).chr(0xF86A);
  }
  elsif($cj >= 0x1160 and $cj <= 0x11a2) { ### jungseong: append U+F86A
    return chr(0x115f).chr($cj).chr(0xF86A);
  }
  else { ### jongseong
    return chr(0x115f).chr(0x1160).chr($cj);
  }
}

sub insert_fillers {
    my $cho  = "\x{1100}-\x{115F}\x{A960}-\x{A97C}";
    my $jung = "\x{1160}-\x{11A7}\x{D7B0}-\x{D7C6}";
    my $jong = "\x{11A8}-\x{11FF}\x{D7CB}-\x{D7FB}\x{F86A}"; # include Tf
    s/([$cho])([$jong])/$1\x{1160}\x{115F}\x{1160}$2/g;
    s/([$cho])(?![$jung])/$1\x{1160}/g;
    s/(?<![$cho])([$jung])/\x{115F}$1/g;
    s/(?<![$jung])([$jong])/\x{115F}\x{1160}$1/g;
    # \x{F86A} is jongsong filler
    s/([$jung])(?![$jong])/$1\x{F86A}/g;
}

sub hanja_to_hangul {
  s/([\x{3400}-\x{4DB5}])/chr $hanjaextA_to_hangul[ord($1)-0x3400]/ge;
  s/([\x{4E00}-\x{9FA5}])/chr $hanja_to_hangul[ord($1)-0x4E00]/ge;
  s/([\x{F900}-\x{FA2D}])/chr $hanjacompat_to_hangul[ord($1)-0xF900]/ge;
}

sub get_hanja_hangul_table {
  my $file = shift;
  my @HJHG;

  $file = `kpsewhich $file`;
  chomp $file;

  open TAB, $file or die "$file : $!\n";
  @HJHG = <TAB>;
  close TAB;

  chomp @HJHG;
  return @HJHG;
}

sub compose_jamo {
    s/\x{1107}\x{1107}\x{110B}/\x{112C}/g;
    s/\x{1107}\x{1109}\x{1100}/\x{1122}/g;
    s/\x{1107}\x{1109}\x{1103}/\x{1123}/g;
    s/\x{1107}\x{1109}\x{1107}/\x{1124}/g;
    s/\x{1107}\x{1109}\x{1109}/\x{1125}/g;
    s/\x{1107}\x{1109}\x{110C}/\x{1126}/g;
    s/\x{1109}\x{1107}\x{1100}/\x{1133}/g;
    s/\x{1109}\x{1109}\x{1109}/\x{1134}/g;
    s/\x{1105}\x{1100}\x{1100}/\x{A965}/g;
    s/\x{1105}\x{1103}\x{1103}/\x{A967}/g;
    s/\x{1105}\x{1107}\x{1107}/\x{A96A}/g;
    s/\x{1105}\x{1107}\x{110B}/\x{A96B}/g;
    s/\x{1107}\x{1109}\x{1110}/\x{A972}/g;
    s/\x{1109}\x{1109}\x{1107}/\x{A975}/g;
    s/\x{110C}\x{110C}\x{1112}/\x{A978}/g;
    s/\x{1100}\x{1100}/\x{1101}/g;
    s/\x{1102}\x{1100}/\x{1113}/g;
    s/\x{1102}\x{1102}/\x{1114}/g;
    s/\x{1102}\x{1103}/\x{1115}/g;
    s/\x{1102}\x{1107}/\x{1116}/g;
    s/\x{1103}\x{1100}/\x{1117}/g;
    s/\x{1103}\x{1103}/\x{1104}/g;
    s/\x{1105}\x{1102}/\x{1118}/g;
    s/\x{1105}\x{1105}/\x{1119}/g;
    s/\x{1105}\x{110B}/\x{111B}/g;
    s/\x{1105}\x{1112}/\x{111A}/g;
    s/\x{1106}\x{1107}/\x{111C}/g;
    s/\x{1106}\x{110B}/\x{111D}/g;
    s/\x{1107}\x{1100}/\x{111E}/g;
    s/\x{1107}\x{1102}/\x{111F}/g;
    s/\x{1107}\x{1103}/\x{1120}/g;
    s/\x{1107}\x{1107}/\x{1108}/g;
    s/\x{1107}\x{1109}/\x{1121}/g;
    s/\x{1107}\x{110A}/\x{1125}/g;
    s/\x{1107}\x{110B}/\x{112B}/g;
    s/\x{1107}\x{110C}/\x{1127}/g;
    s/\x{1107}\x{110E}/\x{1128}/g;
    s/\x{1107}\x{1110}/\x{1129}/g;
    s/\x{1107}\x{1111}/\x{112A}/g;
    s/\x{1107}\x{112B}/\x{112C}/g;
    s/\x{1107}\x{112D}/\x{1122}/g;
    s/\x{1107}\x{112F}/\x{1123}/g;
    s/\x{1107}\x{1132}/\x{1124}/g;
    s/\x{1107}\x{1136}/\x{1126}/g;
    s/\x{1108}\x{110B}/\x{112C}/g;
    s/\x{1109}\x{1100}/\x{112D}/g;
    s/\x{1109}\x{1102}/\x{112E}/g;
    s/\x{1109}\x{1103}/\x{112F}/g;
    s/\x{1109}\x{1105}/\x{1130}/g;
    s/\x{1109}\x{1106}/\x{1131}/g;
    s/\x{1109}\x{1107}/\x{1132}/g;
    s/\x{1109}\x{1109}/\x{110A}/g;
    s/\x{1109}\x{110A}/\x{1134}/g;
    s/\x{1109}\x{110B}/\x{1135}/g;
    s/\x{1109}\x{110C}/\x{1136}/g;
    s/\x{1109}\x{110E}/\x{1137}/g;
    s/\x{1109}\x{110F}/\x{1138}/g;
    s/\x{1109}\x{1110}/\x{1139}/g;
    s/\x{1109}\x{1111}/\x{113A}/g;
    s/\x{1109}\x{1112}/\x{113B}/g;
    s/\x{1109}\x{111E}/\x{1133}/g;
    s/\x{110A}\x{1109}/\x{1134}/g;
    s/\x{110B}\x{1100}/\x{1141}/g;
    s/\x{110B}\x{1103}/\x{1142}/g;
    s/\x{110B}\x{1106}/\x{1143}/g;
    s/\x{110B}\x{1107}/\x{1144}/g;
    s/\x{110B}\x{1109}/\x{1145}/g;
    s/\x{110B}\x{110B}/\x{1147}/g;
    s/\x{110B}\x{110C}/\x{1148}/g;
    s/\x{110B}\x{110E}/\x{1149}/g;
    s/\x{110B}\x{1110}/\x{114A}/g;
    s/\x{110B}\x{1111}/\x{114B}/g;
    s/\x{110B}\x{1140}/\x{1146}/g;
    s/\x{110C}\x{110B}/\x{114D}/g;
    s/\x{110C}\x{110C}/\x{110D}/g;
    s/\x{110E}\x{110F}/\x{1152}/g;
    s/\x{110E}\x{1112}/\x{1153}/g;
    s/\x{1111}\x{1107}/\x{1156}/g;
    s/\x{1111}\x{110B}/\x{1157}/g;
    s/\x{1112}\x{1112}/\x{1158}/g;
    s/\x{1121}\x{1100}/\x{1122}/g;
    s/\x{1121}\x{1103}/\x{1123}/g;
    s/\x{1121}\x{1107}/\x{1124}/g;
    s/\x{1121}\x{1109}/\x{1125}/g;
    s/\x{1121}\x{110C}/\x{1126}/g;
    s/\x{1132}\x{1100}/\x{1133}/g;
    s/\x{113C}\x{113C}/\x{113D}/g;
    s/\x{113E}\x{113E}/\x{113F}/g;
    s/\x{114E}\x{114E}/\x{114F}/g;
    s/\x{1150}\x{1150}/\x{1151}/g;
    s/\x{1100}\x{1103}/\x{115A}/g;
    s/\x{1102}\x{1109}/\x{115B}/g;
    s/\x{1102}\x{110C}/\x{115C}/g;
    s/\x{1102}\x{1112}/\x{115D}/g;
    s/\x{1103}\x{1105}/\x{115E}/g;
    s/\x{1103}\x{1106}/\x{A960}/g;
    s/\x{1103}\x{1107}/\x{A961}/g;
    s/\x{1103}\x{1109}/\x{A962}/g;
    s/\x{1103}\x{110C}/\x{A963}/g;
    s/\x{1105}\x{1100}/\x{A964}/g;
    s/\x{A964}\x{1100}/\x{A965}/g;
    s/\x{1105}\x{1101}/\x{A965}/g;
    s/\x{1105}\x{1103}/\x{A966}/g;
    s/\x{A966}\x{1103}/\x{A967}/g;
    s/\x{1105}\x{1104}/\x{A967}/g;
    s/\x{1105}\x{1106}/\x{A968}/g;
    s/\x{1105}\x{1107}/\x{A969}/g;
    s/\x{A969}\x{1107}/\x{A96A}/g;
    s/\x{1105}\x{1108}/\x{A96A}/g;
    s/\x{1105}\x{112B}/\x{A96B}/g;
    s/\x{1105}\x{1109}/\x{A96C}/g;
    s/\x{1105}\x{110C}/\x{A96D}/g;
    s/\x{1105}\x{110F}/\x{A96E}/g;
    s/\x{1106}\x{1100}/\x{A96F}/g;
    s/\x{1106}\x{1103}/\x{A970}/g;
    s/\x{1106}\x{1109}/\x{A971}/g;
    s/\x{1121}\x{1110}/\x{A972}/g;
    s/\x{1107}\x{1139}/\x{A972}/g;
    s/\x{1107}\x{110F}/\x{A973}/g;
    s/\x{1107}\x{1112}/\x{A974}/g;
    s/\x{110A}\x{1107}/\x{A975}/g;
    s/\x{1109}\x{1132}/\x{A975}/g;
    s/\x{110B}\x{1105}/\x{A976}/g;
    s/\x{110B}\x{1112}/\x{A977}/g;
    s/\x{110D}\x{1112}/\x{A978}/g;
    s/\x{1110}\x{1110}/\x{A979}/g;
    s/\x{1111}\x{1112}/\x{A97A}/g;
    s/\x{1112}\x{1109}/\x{A97B}/g;
    s/\x{1159}\x{1159}/\x{A97C}/g;
    s/\x{1169}\x{1161}\x{1175}/\x{116B}/g;
    s/\x{1169}\x{1165}\x{1175}/\x{1180}/g;
    s/\x{1169}\x{1167}\x{1175}/\x{1181}/g;
    s/\x{116D}\x{1163}\x{1175}/\x{1185}/g;
    s/\x{116E}\x{1161}\x{1175}/\x{118A}/g;
    s/\x{116E}\x{1165}\x{1173}/\x{118B}/g;
    s/\x{116E}\x{1165}\x{1175}/\x{1170}/g;
    s/\x{116E}\x{1167}\x{1175}/\x{118C}/g;
    s/\x{1172}\x{1165}\x{1175}/\x{1190}/g;
    s/\x{1172}\x{1167}\x{1175}/\x{1192}/g;
    s/\x{1173}\x{1175}\x{116E}/\x{1197}/g;
    s/\x{1169}\x{1163}\x{1175}/\x{11A7}/g;
    s/\x{1169}\x{1169}\x{1175}/\x{D7B1}/g;
    s/\x{116D}\x{1161}\x{1175}/\x{D7B3}/g;
    s/\x{116E}\x{1175}\x{1175}/\x{D7B6}/g;
    s/\x{1172}\x{1161}\x{1175}/\x{D7B7}/g;
    s/\x{1173}\x{1165}\x{1175}/\x{D7BB}/g;
    s/\x{1175}\x{1163}\x{1169}/\x{D7BD}/g;
    s/\x{1175}\x{1163}\x{1175}/\x{D7BE}/g;
    s/\x{1175}\x{1167}\x{1175}/\x{D7C0}/g;
    s/\x{1175}\x{1169}\x{1175}/\x{D7C1}/g;
    s/\x{119E}\x{1165}\x{1175}/\x{D7C6}/g;
    s/\x{1161}\x{1169}/\x{1176}/g;
    s/\x{1161}\x{116E}/\x{1177}/g;
    s/\x{1161}\x{1175}/\x{1162}/g;
    s/\x{1163}\x{1169}/\x{1178}/g;
    s/\x{1163}\x{116D}/\x{1179}/g;
    s/\x{1163}\x{1175}/\x{1164}/g;
    s/\x{1165}\x{1169}/\x{117A}/g;
    s/\x{1165}\x{116E}/\x{117B}/g;
    s/\x{1165}\x{1173}/\x{117C}/g;
    s/\x{1165}\x{1175}/\x{1166}/g;
    s/\x{1167}\x{1169}/\x{117D}/g;
    s/\x{1167}\x{116E}/\x{117E}/g;
    s/\x{1167}\x{1175}/\x{1168}/g;
    s/\x{1169}\x{1161}/\x{116A}/g;
    s/\x{1169}\x{1162}/\x{116B}/g;
    s/\x{1169}\x{1165}/\x{117F}/g;
    s/\x{1169}\x{1166}/\x{1180}/g;
    s/\x{1169}\x{1168}/\x{1181}/g;
    s/\x{1169}\x{1169}/\x{1182}/g;
    s/\x{1169}\x{116E}/\x{1183}/g;
    s/\x{1169}\x{1175}/\x{116C}/g;
    s/\x{116A}\x{1175}/\x{116B}/g;
    s/\x{116D}\x{1163}/\x{1184}/g;
    s/\x{116D}\x{1164}/\x{1185}/g;
    s/\x{116D}\x{1167}/\x{1186}/g;
    s/\x{116D}\x{1169}/\x{1187}/g;
    s/\x{116D}\x{1175}/\x{1188}/g;
    s/\x{116E}\x{1161}/\x{1189}/g;
    s/\x{116E}\x{1162}/\x{118A}/g;
    s/\x{116E}\x{1165}/\x{116F}/g;
    s/\x{116E}\x{1166}/\x{1170}/g;
    s/\x{116E}\x{1168}/\x{118C}/g;
    s/\x{116E}\x{116E}/\x{118D}/g;
    s/\x{116E}\x{1175}/\x{1171}/g;
    s/\x{116E}\x{117C}/\x{118B}/g;
    s/\x{116F}\x{1173}/\x{118B}/g;
    s/\x{116F}\x{1175}/\x{1170}/g;
    s/\x{1172}\x{1161}/\x{118E}/g;
    s/\x{1172}\x{1165}/\x{118F}/g;
    s/\x{1172}\x{1166}/\x{1190}/g;
    s/\x{1172}\x{1167}/\x{1191}/g;
    s/\x{1172}\x{1168}/\x{1192}/g;
    s/\x{1172}\x{116E}/\x{1193}/g;
    s/\x{1172}\x{1175}/\x{1194}/g;
    s/\x{1173}\x{116E}/\x{1195}/g;
    s/\x{1173}\x{1173}/\x{1196}/g;
    s/\x{1173}\x{1175}/\x{1174}/g;
    s/\x{1173}\x{119B}/\x{1197}/g;
    s/\x{1174}\x{116E}/\x{1197}/g;
    s/\x{1175}\x{1161}/\x{1198}/g;
    s/\x{1175}\x{1163}/\x{1199}/g;
    s/\x{1175}\x{1169}/\x{119A}/g;
    s/\x{1175}\x{116E}/\x{119B}/g;
    s/\x{1175}\x{1173}/\x{119C}/g;
    s/\x{1175}\x{119E}/\x{119D}/g;
    s/\x{117F}\x{1175}/\x{1180}/g;
    s/\x{1184}\x{1175}/\x{1185}/g;
    s/\x{1189}\x{1175}/\x{118A}/g;
    s/\x{118F}\x{1175}/\x{1190}/g;
    s/\x{1191}\x{1175}/\x{1192}/g;
    s/\x{119E}\x{1165}/\x{119F}/g;
    s/\x{119E}\x{116E}/\x{11A0}/g;
    s/\x{119E}\x{1175}/\x{11A1}/g;
    s/\x{119E}\x{119E}/\x{11A2}/g;
    s/\x{1161}\x{1173}/\x{11A3}/g;
    s/\x{1163}\x{116E}/\x{11A4}/g;
    s/\x{1167}\x{1163}/\x{11A5}/g;
    s/\x{1169}\x{1163}/\x{11A6}/g;
    s/\x{1169}\x{1164}/\x{11A7}/g;
    s/\x{1169}\x{1167}/\x{D7B0}/g;
    s/\x{1182}\x{1175}/\x{D7B1}/g;
    s/\x{1169}\x{116C}/\x{D7B1}/g;
    s/\x{116D}\x{1161}/\x{D7B2}/g;
    s/\x{116D}\x{1162}/\x{D7B3}/g;
    s/\x{116D}\x{1165}/\x{D7B4}/g;
    s/\x{116E}\x{1167}/\x{D7B5}/g;
    s/\x{1171}\x{1175}/\x{D7B6}/g;
    s/\x{116E}\x{D7C4}/\x{D7B6}/g;
    s/\x{1172}\x{1162}/\x{D7B7}/g;
    s/\x{1172}\x{1169}/\x{D7B8}/g;
    s/\x{1173}\x{1161}/\x{D7B9}/g;
    s/\x{1173}\x{1165}/\x{D7BA}/g;
    s/\x{1173}\x{1166}/\x{D7BB}/g;
    s/\x{1173}\x{1169}/\x{D7BC}/g;
    s/\x{1199}\x{1169}/\x{D7BD}/g;
    s/\x{1175}\x{1178}/\x{D7BD}/g;
    s/\x{1175}\x{1164}/\x{D7BE}/g;
    s/\x{1175}\x{1167}/\x{D7BF}/g;
    s/\x{1175}\x{1168}/\x{D7C0}/g;
    s/\x{119A}\x{1175}/\x{D7C1}/g;
    s/\x{1175}\x{116C}/\x{D7C1}/g;
    s/\x{1175}\x{116D}/\x{D7C2}/g;
    s/\x{1175}\x{1172}/\x{D7C3}/g;
    s/\x{1175}\x{1175}/\x{D7C4}/g;
    s/\x{119E}\x{1161}/\x{D7C5}/g;
    s/\x{119E}\x{1166}/\x{D7C6}/g;
    s/\x{11A8}\x{11BA}\x{11A8}/\x{11C4}/g;
    s/\x{11AF}\x{11A8}\x{11BA}/\x{11CC}/g;
    s/\x{11AF}\x{11AE}\x{11C2}/\x{11CF}/g;
    s/\x{11AF}\x{11B7}\x{11A8}/\x{11D1}/g;
    s/\x{11AF}\x{11B7}\x{11BA}/\x{11D2}/g;
    s/\x{11AF}\x{11B8}\x{11BA}/\x{11D3}/g;
    s/\x{11AF}\x{11B8}\x{11BC}/\x{11D5}/g;
    s/\x{11AF}\x{11B8}\x{11C2}/\x{11D4}/g;
    s/\x{11AF}\x{11BA}\x{11BA}/\x{11D6}/g;
    s/\x{11B7}\x{11BA}\x{11BA}/\x{11DE}/g;
    s/\x{11BC}\x{11A8}\x{11A8}/\x{11ED}/g; # legacy enc
    s/\x{11F0}\x{11A8}\x{11A8}/\x{11ED}/g;
    s/\x{11AE}\x{11AE}\x{11B8}/\x{D7CE}/g;
    s/\x{11AE}\x{11BA}\x{11A8}/\x{D7D1}/g;
    s/\x{11AF}\x{11A8}\x{11A8}/\x{D7D5}/g;
    s/\x{11AF}\x{11A8}\x{11C2}/\x{D7D6}/g;
    s/\x{11AF}\x{11AF}\x{11BF}/\x{D7D7}/g;
    s/\x{11AF}\x{11B7}\x{11C2}/\x{D7D8}/g;
    s/\x{11AF}\x{11B8}\x{11AE}/\x{D7D9}/g;
    s/\x{11AF}\x{11B8}\x{11C1}/\x{D7DA}/g;
    s/\x{11AF}\x{11F9}\x{11C2}/\x{D7DC}/g;
    s/\x{11B7}\x{11AB}\x{11AB}/\x{D7DF}/g;
    s/\x{11B7}\x{11B8}\x{11BA}/\x{D7E1}/g;
    s/\x{11B8}\x{11AF}\x{11C1}/\x{D7E4}/g;
    s/\x{11B8}\x{11BA}\x{11AE}/\x{D7E7}/g;
    s/\x{11BA}\x{11B8}\x{11BC}/\x{D7EB}/g;
    s/\x{11BA}\x{11BA}\x{11A8}/\x{D7EC}/g;
    s/\x{11BA}\x{11BA}\x{11AE}/\x{D7ED}/g;
    s/\x{11EB}\x{11B8}\x{11BC}/\x{D7F4}/g;
    s/\x{11BD}\x{11B8}\x{11B8}/\x{D7F8}/g;
    s/\x{11A8}\x{11A8}/\x{11A9}/g;
    s/\x{11A8}\x{11AF}/\x{11C3}/g;
    s/\x{11A8}\x{11BA}/\x{11AA}/g;
    s/\x{11A8}\x{11E7}/\x{11C4}/g;
    s/\x{11AA}\x{11A8}/\x{11C4}/g;
    s/\x{11AB}\x{11A8}/\x{11C5}/g;
    s/\x{11AB}\x{11AE}/\x{11C6}/g;
    s/\x{11AB}\x{11BA}/\x{11C7}/g;
    s/\x{11AB}\x{11BD}/\x{11AC}/g;
    s/\x{11AB}\x{11C0}/\x{11C9}/g;
    s/\x{11AB}\x{11C2}/\x{11AD}/g;
    s/\x{11AB}\x{11EB}/\x{11C8}/g;
    s/\x{11AE}\x{11A8}/\x{11CA}/g;
    s/\x{11AE}\x{11AF}/\x{11CB}/g;
    s/\x{11AF}\x{11A8}/\x{11B0}/g;
    s/\x{11AF}\x{11AA}/\x{11CC}/g;
    s/\x{11AF}\x{11AB}/\x{11CD}/g;
    s/\x{11AF}\x{11AE}/\x{11CE}/g;
    s/\x{11AF}\x{11AF}/\x{11D0}/g;
    s/\x{11AF}\x{11B7}/\x{11B1}/g;
    s/\x{11AF}\x{11B8}/\x{11B2}/g;
    s/\x{11AF}\x{11B9}/\x{11D3}/g;
    s/\x{11AF}\x{11BA}/\x{11B3}/g;
    s/\x{11AF}\x{11BB}/\x{11D6}/g;
    s/\x{11AF}\x{11BF}/\x{11D8}/g;
    s/\x{11AF}\x{11C0}/\x{11B4}/g;
    s/\x{11AF}\x{11C1}/\x{11B5}/g;
    s/\x{11AF}\x{11C2}/\x{11B6}/g;
    s/\x{11AF}\x{11DA}/\x{11D1}/g;
    s/\x{11AF}\x{11DD}/\x{11D2}/g;
    s/\x{11AF}\x{11E5}/\x{11D4}/g;
    s/\x{11AF}\x{11E6}/\x{11D5}/g;
    s/\x{11AF}\x{11EB}/\x{11D7}/g;
    s/\x{11AF}\x{11F9}/\x{11D9}/g;
    s/\x{11B0}\x{11BA}/\x{11CC}/g;
    s/\x{11B1}\x{11A8}/\x{11D1}/g;
    s/\x{11B1}\x{11BA}/\x{11D2}/g;
    s/\x{11B2}\x{11BA}/\x{11D3}/g;
    s/\x{11B2}\x{11BC}/\x{11D5}/g;
    s/\x{11B2}\x{11C2}/\x{11D4}/g;
    s/\x{11B3}\x{11BA}/\x{11D6}/g;
    s/\x{11B7}\x{11A8}/\x{11DA}/g;
    s/\x{11B7}\x{11AF}/\x{11DB}/g;
    s/\x{11B7}\x{11B8}/\x{11DC}/g;
    s/\x{11B7}\x{11BA}/\x{11DD}/g;
    s/\x{11B7}\x{11BB}/\x{11DE}/g;
    s/\x{11B7}\x{11BC}/\x{11E2}/g;
    s/\x{11B7}\x{11BE}/\x{11E0}/g;
    s/\x{11B7}\x{11C2}/\x{11E1}/g;
    s/\x{11B7}\x{11EB}/\x{11DF}/g;
    s/\x{11B8}\x{11AF}/\x{11E3}/g;
    s/\x{11B8}\x{11BA}/\x{11B9}/g;
    s/\x{11B8}\x{11BC}/\x{11E6}/g;
    s/\x{11B8}\x{11C1}/\x{11E4}/g;
    s/\x{11B8}\x{11C2}/\x{11E5}/g;
    s/\x{11BA}\x{11A8}/\x{11E7}/g;
    s/\x{11BA}\x{11AE}/\x{11E8}/g;
    s/\x{11BA}\x{11AF}/\x{11E9}/g;
    s/\x{11BA}\x{11B8}/\x{11EA}/g;
    s/\x{11BA}\x{11BA}/\x{11BB}/g;
    s/\x{11BC}\x{11A8}/\x{11EC}/g; # legacy enc
    s/\x{11F0}\x{11A8}/\x{11EC}/g;
    s/\x{11BC}\x{11A9}/\x{11ED}/g; # legacy enc
    s/\x{11F0}\x{11A9}/\x{11ED}/g;
    s/\x{11BC}\x{11BC}/\x{11EE}/g; # legacy enc
    s/\x{11F0}\x{11F0}/\x{11EE}/g;
    s/\x{11BC}\x{11BF}/\x{11EF}/g; # legacy enc
    s/\x{11F0}\x{11BF}/\x{11EF}/g;
    s/\x{11C1}\x{11B8}/\x{11F3}/g;
    s/\x{11C1}\x{11BC}/\x{11F4}/g;
    s/\x{11C2}\x{11AB}/\x{11F5}/g;
    s/\x{11C2}\x{11AF}/\x{11F6}/g;
    s/\x{11C2}\x{11B7}/\x{11F7}/g;
    s/\x{11C2}\x{11B8}/\x{11F8}/g;
    s/\x{11CE}\x{11C2}/\x{11CF}/g;
    s/\x{11DD}\x{11BA}/\x{11DE}/g;
    s/\x{11EC}\x{11A8}/\x{11ED}/g;
    s/\x{11F0}\x{11BA}/\x{11F1}/g;
    s/\x{11F0}\x{11EB}/\x{11F2}/g;
    s/\x{11A8}\x{11AB}/\x{11FA}/g;
    s/\x{11A8}\x{11B8}/\x{11FB}/g;
    s/\x{11A8}\x{11BE}/\x{11FC}/g;
    s/\x{11A8}\x{11BF}/\x{11FD}/g;
    s/\x{11A8}\x{11C2}/\x{11FE}/g;
    s/\x{11AB}\x{11AB}/\x{11FF}/g;
    s/\x{11AB}\x{11AF}/\x{D7CB}/g;
    s/\x{11AB}\x{11BE}/\x{D7CC}/g;
    s/\x{11AE}\x{11AE}/\x{D7CD}/g;
    s/\x{D7CD}\x{11B8}/\x{D7CE}/g;
    s/\x{11AE}\x{D7CF}/\x{D7CE}/g;
    s/\x{11AE}\x{11B8}/\x{D7CF}/g;
    s/\x{11AE}\x{11BA}/\x{D7D0}/g;
    s/\x{D7D0}\x{11A8}/\x{D7D1}/g;
    s/\x{11AE}\x{11E7}/\x{D7D1}/g;
    s/\x{11AE}\x{11BD}/\x{D7D2}/g;
    s/\x{11AE}\x{11BE}/\x{D7D3}/g;
    s/\x{11AE}\x{11C0}/\x{D7D4}/g;
    s/\x{11B0}\x{11A8}/\x{D7D5}/g;
    s/\x{11AF}\x{11A9}/\x{D7D5}/g;
    s/\x{11B0}\x{11C2}/\x{D7D6}/g;
    s/\x{11AF}\x{11FE}/\x{D7D6}/g;
    s/\x{11D0}\x{11BF}/\x{D7D7}/g;
    s/\x{11AF}\x{11D8}/\x{D7D7}/g;
    s/\x{11B1}\x{11C2}/\x{D7D8}/g;
    s/\x{11AF}\x{11E1}/\x{D7D8}/g;
    s/\x{11B2}\x{11AE}/\x{D7D9}/g;
    s/\x{11AF}\x{D7E3}/\x{D7D9}/g;
    s/\x{11B2}\x{11C1}/\x{D7DA}/g;
    s/\x{11AF}\x{11E4}/\x{D7DA}/g;
    s/\x{11AF}\x{11F0}/\x{D7DB}/g;
    s/\x{11D9}\x{11C2}/\x{D7DC}/g;
    s/\x{11AF}\x{11BC}/\x{D7DD}/g;
    s/\x{11B7}\x{11AB}/\x{D7DE}/g;
    s/\x{D7DE}\x{11AB}/\x{D7DF}/g;
    s/\x{11B7}\x{11FF}/\x{D7DF}/g;
    s/\x{11B7}\x{11B7}/\x{D7E0}/g;
    s/\x{11DC}\x{11BA}/\x{D7E1}/g;
    s/\x{11B7}\x{11B9}/\x{D7E1}/g;
    s/\x{11B7}\x{11BD}/\x{D7E2}/g;
    s/\x{11B8}\x{11AE}/\x{D7E3}/g;
    s/\x{11E3}\x{11C1}/\x{D7E4}/g;
    s/\x{11B8}\x{11B5}/\x{D7E4}/g;
    s/\x{11B8}\x{11B7}/\x{D7E5}/g;
    s/\x{11B8}\x{11B8}/\x{D7E6}/g;
    s/\x{11B9}\x{11AE}/\x{D7E7}/g;
    s/\x{11B8}\x{11E8}/\x{D7E7}/g;
    s/\x{11B8}\x{11BD}/\x{D7E8}/g;
    s/\x{11B8}\x{11BE}/\x{D7E9}/g;
    s/\x{11BA}\x{11B7}/\x{D7EA}/g;
    s/\x{11EA}\x{11BC}/\x{D7EB}/g;
    s/\x{11BA}\x{11E6}/\x{D7EB}/g;
    s/\x{11BB}\x{11A8}/\x{D7EC}/g;
    s/\x{11BA}\x{11E7}/\x{D7EC}/g;
    s/\x{11BB}\x{11AE}/\x{D7ED}/g;
    s/\x{11BA}\x{11E8}/\x{D7ED}/g;
    s/\x{11BA}\x{11EB}/\x{D7EE}/g;
    s/\x{11BA}\x{11BD}/\x{D7EF}/g;
    s/\x{11BA}\x{11BE}/\x{D7F0}/g;
    s/\x{11BA}\x{11C0}/\x{D7F1}/g;
    s/\x{11BA}\x{11C2}/\x{D7F2}/g;
    s/\x{11EB}\x{11B8}/\x{D7F3}/g;
    s/\x{D7F3}\x{11BC}/\x{D7F4}/g;
    s/\x{11EB}\x{11E6}/\x{D7F4}/g;
    s/\x{11F0}\x{11B7}/\x{D7F5}/g;
    s/\x{11F0}\x{11C2}/\x{D7F6}/g;
    s/\x{11BD}\x{11B8}/\x{D7F7}/g;
    s/\x{D7F7}\x{11B8}/\x{D7F8}/g;
    s/\x{11BD}\x{D7E6}/\x{D7F8}/g;
    s/\x{11BD}\x{11BD}/\x{D7F9}/g;
    s/\x{11C1}\x{11BA}/\x{D7FA}/g;
    s/\x{11C1}\x{11C0}/\x{D7FB}/g;
}

sub jamo_to_puajamo {
  s/([\x{1100}-\x{11FF}])/chr hex $jamo_puajamo_1100[ord($1)-0x1100]/ge;
  s/([\x{A960}-\x{A97C}])/chr hex $jamo_puajamo_A960[ord($1)-0xa960]/ge;
  s/([\x{D7B0}-\x{D7C6}])/chr hex $jamo_puajamo_D7B0[ord($1)-0xd7b0]/ge;
  s/([\x{D7CB}-\x{D7FB}])/chr hex $jamo_puajamo_D7CB[ord($1)-0xd7cb]/ge;
}

### EOF
