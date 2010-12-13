@PERL@
# fixmacps: swap to sanitised appledict
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

use File::Basename;

$line = 0;			# keep line count
$predir = `kpsewhich -progname=dvips -format="other text files" md71_0.ps`;
$dir=`dirname $predir`;
$prefix = "md";
$default = "md71_0.ps";

while ($_ = shift(@ARGV)) {
   if (/^-d(ir)?$/)   { $dir = shift(@ARGV); }
   elsif (/^-n(ame)?$/)   { $prefix = shift(@ARGV); }
   else {
      unshift(@ARGV, $_);
      last;
   }
}

%fonts = ();
$nesting = 0;

while (<>) {
   if (/^%!/) {
      if (! $line) {
	 print;
      }
   } elsif (/^%%(Begin|Include)ProcSet: "?\(AppleDict md\)"? ([0-9]+) ([0-9]+)$/) {
      local($inc, $mdv, $mdr) = ($1, $2, $3);
      if (open(SANE, "<$dir/$prefix${mdv}_$mdr.ps") ||
	  open(SANE, "<$dir/$default")) {
	 $sane = <SANE>;
	 local($snv, $snr) =
	    $sane =~ /^%%BeginProcSet: \(AppleDict md\) ([0-9]+) ([0-9]+)$/;
	 if ($mdv == $snv && $mdr == $snr) {
	     if ( $inc eq "Include" ) {
		 print STDERR "Inserting ProcSet \"(AppleDict md)\" $snv $snr\n";
		 print $sane;
		 while(<SANE>) {
		     print;
		 }
		 close(SANE);
	     }
	     else {
		 print STDERR "Substituting ProcSet \"(AppleDict md)\" $snv $snr\n";
		 $ignore = 1;
	     }
	 } else {
	    print STDERR "Unrecognised AppleDict version $mdv $mdr\n";
	    print "%!\n" if !$line;
	    print;
	 }
      } else {
	 print STDERR "Can't find sanitised AppleDict\n";
	 print "%!\n" if !$line;
	 print;
      }
   } elsif (/^%%EndProcSet/) {
      if ($ignore) {
	 $ignore = 0;
	 print "%!\n" if !$line;
	 print $sane;
	 while(<SANE>) {
	    print;
	 }
	 close(SANE);
      } else {
	 print "%!\n" if !$line;
	 print;
      }
   } elsif (/^%%Page:/ && $nesting == 0) {
      print $_;
      print values(%fonts);
   } elsif (/^%%BeginDocument/ || /^%%BeginBinary/ || /^%%BeginFile/) {
      print $_;
      $nesting++;
   } elsif (/^%%EndDocument/ || /^%%EndBinary/ || /^%%EndFile/) {
      print $_;
      $nesting--;
   } else {
      if (! $ignore) {
	 if (/^\{\}mark .*rf$/) {
	    $fonts{$_} = $_;
	    print;
	 } else {
	    print "%!\n" if !$line;
	    print;
	 }
      }
   }
   $line++;
}
@END@
