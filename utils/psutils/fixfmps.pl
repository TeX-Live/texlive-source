@PERL@
# fixfmps: get conforming PostScript out of FrameMaker version 2 file
# move all FMDEFINEFONTs to start of pages
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

%fonts=();

while (<>) {
   if (/^([0-9]+) [0-9]+ .* FMDEFINEFONT$/) {
      $fonts{$1} = $_;
   } elsif (/^[0-9.]+ [0-9.]+ [0-9]+ FMBEGINPAGE$/) {
      print $_, join('',values(%fonts));
   } elsif (m%(.*/PageSize \[paperwidth paperheight\]put )setpagedevice(.*)%) {
      print "$1pop$2\n";
   } else {
      print $_;
   }
}
@END@
