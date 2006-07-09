# sam2ptol.pm by pts@math.bme.hu at Sun Apr  7 14:48:27 CEST 2002
package sam2ptol;
BEGIN{$^W=1}
use integer;
use strict;

my %keys=qw{/InputFile 1 /OutputFile 1 /TmpRemove 1 /Profile 1
  /FileFormat 1 /SampleFormat 1 /WarningOK 1 /TransferEncoding 1 /Compression 1
  /Predictor 1 /Transparent 1 /Hints 1
  /Effort 1 /RecordSize 1 /K 1 /Quality 1 /ColorTransform 1 /TransferCPL 1
  /EncoderColumns 1 /EncoderRows 1 /EncoderColors 1
  /PredictorColumns 1 /PredictorBPC 1 /PredictorColors 1
  /Comment 1 /Title 1 /Subject 1 /Author 1 /Creator 1 /Producer 1
  /Created 1 /Produced 1 /LoadHints 1 /Templates 1};
# Dat: DCT is not key!
my %values=qw{
  /PSL1 1 /PSLC 1 /PSL2 1 /PSL3 1 /PDFB1.0 1 /PDFB1.2 1 /PDF1.0 1 /PDF1.2 1
  /GIF89a 1 /Empty 1 /Meta 1 /PNM 1 /PAM 1 /PIP 1 /Opaque 1 /Transparent 1
  /Transparent2 1 /Transparent4 1 /Transparent8 1 /Gray1 1 /Gray2 1 /Gray4 1
  /Gray8 1 /Indexed1 1 /Indexed2 1 /Indexed4 1 /Indexed8 1 /Mask 1 /Rgb1 1
  /Rgb2 1 /Rgb4 1 /Rgb8 1 /RGB1 1 /RGB2 1 /RGB4 1 /RGB8 1 /Asis 1 /Binary 1
  /ASCII 1 /Hex 1 /AHx 1 /ASCIIHex 1 /A85 1 /ASCII85 1 /None 1 /LZW 1 /ZIP 1
  /Flate 1 /Fl 1 /RLE 1 /RunLength 1 /RunLengthEncoded 1 /RL 1 /PackBits 1
  /Fax 1 /CCITTFax 1 /CCF 1 /DCT 1 /JPEG 1 /JPG 1 /JFIF 1 /IJG 1 /JAI 1
};

sub highlight {
  #** Example: perl -I. -Msam2ptol -e sam2ptol::highlight alma <examples/a_jpg.job
  die "$0: 1arg, tclPath expected\n" if @ARGV!=1;
  my $w=$ARGV[0]; # name of the widget
  $_=join'',<STDIN>;
  my($beg,$end,$c,$ta,$tb);
  for (qw{tError tInt tComment tString tSing tKey tNameval tBrac}) { print "$w tag remove tString 1.0 end\n" }
  while (m@(
    \((\\.|[^\\()]+)*\) # String. Imp: nesting??
   |/([^\000-\040/\%{}<>\[\]()\177-\377]*) # Sname
   |[_A-Za-z_]([^\000-\040/\%{}<>\[\]()\177-\377]*) # Ename
   |<<|>>|\[|\]
   |true|false|pop|null
   |%[^\r\n]*
   |-?\d+
  )@gsx) {
    my $e=$1;
    $beg=($end=pos())-length($e);
    $c=substr($e,0,1);
    $ta="$w tag add"; $tb=" {1.0 + $beg chars} {1.0 + $end chars}\n";
    if ($c eq'/') {
      if (exists $keys{$e}) {
        print "$ta tKey$tb";
        # print STDERR "($e)\n";
      } elsif (exists $values{$e}) {
        print "$ta tNameval$tb";
      } else {
        print "$ta tError$tb";
      }
    } elsif ($c eq'(') {
      print "$ta tString$tb";
      print "$w tag add tString {1.0 + $beg chars} {1.0 + $end chars}\n"
    } elsif ($e eq'true' or $e eq'false' or $e eq'pop' or $e eq'null') {
      print "$ta tSing$tb";
    } elsif ($e=~/\A[_A-Za-z]/) {
    # } elsif ($e=~/\A\w/) {
      # print STDERR "($e)\n";
      print "$ta tError$tb";
    } elsif ($c eq'%') {
      print "$ta tComment$tb";
    } elsif ($c eq'-' or ord($c)>=ord('0') and ord($c)<=ord('9')) {
      print "$ta tInt$tb";
    } else {
      print "$ta tBrac$tb";
    }
    print "$ta tAny$tb";
    # print "[".pos()."]\n";
  }
}

1;
