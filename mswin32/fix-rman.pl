#
# Purpose: create cross references through pages translated
# 	   by Polyglottman.
# Author: F. Popineau
# Date  : 10/10/98
# Time-stamp: <03/05/16 00:28:07 popineau>
#
# Usage: 
#        rman -f HTML -n file.man file.man | perl fix-rman.pl > file.html
#

require "newgetopt.pl";

&main;

exit 0;

sub main {

  unless (&NGetOpt ("title=s")) {
    print STDERR "Usage: $0 --title=name\n";
    exit 1;
  }

  while (<>) {
    #	lines to match : <B><A HREF="rman.c.rman.c">mktexmf</B>(1)</A>
    $_ =~ s/<TITLE>..man.c\(..man.c\) manual page<\/TITLE>/<TITLE>$opt_title manual page<\/TITLE>/ ;
    $_ =~ s/(.*)<A HREF="rman.c.rman.c">([A-z_0-9-]+)<\/B>\(1\)<\/A>(.*)/$1<A HREF="$2.html">$2<\/B>\(1\)<\/A>$3/ ;
    print $_;
    #	$_ =~ s/(.*)<A HREF="rman.c.rman.c">([A-z_0-9-]+)<\/B>\(1\)<\/A>(.*)/ ;
  }
}
