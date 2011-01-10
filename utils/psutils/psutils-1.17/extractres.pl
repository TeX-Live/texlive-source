@PERL@
# extractres: extract resources from PostScript file
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$0 =~ s=.*/==;
$prog = $0;

%resources = ();		# list of resources included
%merge = ();			# list of resources extracted this time
%extn = ("font", ".pfa", "file", ".ps", "procset", ".ps", # resource extns
	 "pattern", ".pat", "form", ".frm", "encoding", ".enc");
%type = ("%%BeginFile:", "file", "%%BeginProcSet:", "procset",
	 "%%BeginFont:", "font"); # resource types

while (@ARGV) {
   $_ = shift;
   if (/^-m(erge)?$/) { $merge = 1; }
   elsif (/^-/) {
      print STDERR "Usage: $prog [-merge] [file]\n";
      exit 1;
   } else {
      unshift(@ARGV, $_);
      last;
   }
}

if (defined($ENV{TMPDIR})) {	# set body file name
   $body = "$ENV{TMPDIR}/body$$.ps";
} elsif (defined($ENV{TEMP})) {
   $body = "$ENV{TEMP}/body$$.ps";
} elsif (defined($ENV{TMP})) {
   $body = "$ENV{TMP}/body$$.ps";
} else {
   $body = "body$$.ps";
}

open(BODY, $body) && die "Temporary file $body already exists";
open(BODY, ">$body") || die "Can't write file $body";

sub filename {			# make filename for resource in @_
   local($name);
   foreach (@_) {		# sanitise name
      s/[!()\$\#*&\\\|\`\'\"\~\{\}\[\]\<\>\?]//g;
      $name .= $_;
   }
   $name =~ s@.*/@@;		# drop directories
   die "Filename not found for resource ", join(" ", @_), "\n"
      if $name =~ /^$/;
   $name;
}

$output = STDOUT;		# start writing header out
while (<>) {
   if (/^%%BeginResource:/ || /^%%BeginFont:/ || /^%%BeginProcSet:/) {
      local($comment, @res) = split(/\s+/); # look at resource type
      local($type) = defined($type{$comment}) ? $type{$comment} : shift(@res);
      local($name) = &filename(@res, $extn{$type}); # make file name
      $saveout = $output;
      if (!$resources{$name}) {
	 print "%%IncludeResource: $type ", join(" ", @res), "\n";
	 if (!open(RES, $name)) {
	    open(RES, ">$name") || die "Can't write file $name";
	    $resources{$name} = $name;
	    $merge{$name} = $merge;
	    $output = RES;
	 } else {		# resource already exists
	    close(RES);
	    undef $output;
	 }
      } elsif ($merge{$name}) {
	 open(RES, ">>$name") || die "Can't append to file $name";
	 $output = RES;
      } else {			# resource already included
	 undef $output;
      }
   } elsif (/^%%EndResource/ || /^%%EndFont/ || /^%%EndProcSet/) {
      if (defined $output) {
	 print $output $_;
	 close($output);
      }
      $output = $saveout;
      next;
   } elsif ((/^%%EndProlog/ || /^%%BeginSetup/ || /^%%Page:/)) {
      $output = BODY;
   }
   print $output $_
      if defined $output;
}

close(BODY);			# close body output file

open(BODY, $body);		# reopen body for input
while (<BODY>) {		# print it all
   print $_;
}
close(BODY);

unlink($body);			# dispose of body file
@END@
