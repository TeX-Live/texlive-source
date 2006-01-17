BEGIN {   # get our other local perl modules.
  $mydir = "c:/Local/Perl/lib";  # fabrice
  unshift (@INC, $mydir) if -d $mydir;
  $mydir = "c:/Local/Perl/site/lib";  # fabrice
  unshift (@INC, $mydir) if -d $mydir;
}

require "newgetopt.pl";
# use lib "./perl";

$prologue = "#undef TRIP\n#undef TRAP\n#define STAT\n#define INI\n";

%engines = (
	    "tex" => "#define INITEX\n#define TeX\n",
	    "etex" => "#define INITEX\n#define TeX\n#define eTeX\n",
	    "pdftex"=> "#define INITEX\n#define TeX\n#define pdfTeX\n",
	    "pdfetex"=> "#define INITEX\n#define TeX\n#define pdfeTeX\n",
	    "pdfxtex"=> "#define INITEX\n#define TeX\n#define pdfxTeX\n",
	    "omega"=> "#define INITEX\n#define TeX\n#define Omega\n",
	    "eomega"=> "#define INITEX\n#define TeX\n#define eOmega\n",
	    "aleph"=> "#define INITEX\n#define TeX\n#define Aleph\n",
	    "mf"=> "#define INIMF\n#define MF\n",
	    "mp"=> "#define INIMP\n#define MP\n"
	    );

&main;

exit 0;

sub help {
  print STDERR "Usage: $0 --name=<engine>\n";
}

sub main {
  unless (&NGetOpt ("name=s", "dfile", "help")) {
    print STDERR "Try `$0 --help'";
    exit 1;
  }

  open DFILE,">${opt_name}d.h" if ($opt_dfile);
  print STDOUT $prologue;
  print DFILE $prologue if ($opt_dfile);
  print STDOUT $engines{$opt_name};
  print DFILE $engines{$opt_name} if ($opt_dfile);
  $coerceline = "^#include \"${opt_name}coerce\.h\"";
  while (<STDIN>) {
    print STDOUT $_;
    print DFILE ($_ =~ m@^(\#|\s|\n|}|\/|typedef)@x ? $_ : "EXTERN $_") if ($opt_dfile);
    #      print STDERR "coerceline = $coerceline\n";
    if (/${coerceline}/) {
      # Include $opt_nameextra.c
      open EXTRA, "<./lib/texmfmp.c";
      while (<EXTRA>) {
        s/TEX-OR-MF-OR-MP/${opt_name}/;
        s@^(#include \"${opt_name}d\.h\")@/* $1 */@;
        print STDOUT $_;
      }
      close EXTRA;
      close DFILE if ($opt_dfile);
    }
  }
}
