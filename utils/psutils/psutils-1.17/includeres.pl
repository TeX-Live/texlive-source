@PERL@
# includeres: include resources in PostScript file
#
# Copyright (C) Angus J. C. Duggan 1991-1995
# See file LICENSE for details.

$prog = ($0 =~ s=.*/==);

%extn = ("font", ".pfa", "file", ".ps", "procset", ".ps", # resource extns
	 "pattern", ".pat", "form", ".frm", "encoding", ".enc");
%type = ("%%BeginFile:", "file", "%%BeginProcSet:", "procset",
	 "%%BeginFont:", "font"); # resource types

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

while (<>) {
   if (/^%%IncludeResource:/ || /^%%IncludeFont:/ || /^%%IncludeProcSet:/) {
      local($comment, @res) = split(/\s+/);
      local($type) = defined($type{$comment}) ? $type{$comment} : shift(@res);
      local($name) = &filename(@res);
      local($inc) = ""; # system include directory
      if (open(RES, $name) || open(RES, "$name$extn{$type}") ||
	  open(RES, "$inc/$name") || open(RES, "$inc/$name$extn{$type}")) {
	 while (<RES>) {
	    print $_;
	 }
	 close(RES);
      } else {
	 print "%%IncludeResource: ", join(" ", $type, @res), "\n";
	 print STDERR "Resource $name not found\n";
      }
   } else {
      print $_;
   }
}
@END@
