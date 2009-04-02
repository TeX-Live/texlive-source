#!/usr/bin/env perl

# Author: Jody Klymak <jklymak@apl.washington.edu>, publisted by a posting
#   to the pdftex mailinglist.

# recursively finds all your eps files.  Looks down \input{fname}.
# CAVEATS:
# 1) cannot handle \input{fname} split over more than one line.
#    1.5) cannot handle multiple \input{} or \includegraphics{} on one line.
# 2) Must be run from same directory as the Latex file.
# 3) Does not look down $TEXINPUTS or anything fancy like that...
# 4) Handling of \include is untested (though I guess its trivial)
# 5) Assumes *all* your graphics inclusions are [e]ps.  But don't
#    fret, because if they are not epstopdf dies anyhow....
# 6) Does not distinguish between percent (\%) and comment (%).

# Changelog:
# 20030103 -- Lachlan Andrew <lha@users.sourceforge.net>
#	* Only append '.tex' if $fname doesn't exist.
#	* Correctly handle lines with '}' after the \includegraphics{}
#	* Allow multiple extensions for graphics files.  .eps -> .ps -> none
#	  (Should try them in the same order as  \includegraphics  does
#	   -- given by \DeclareGraphicsExtensions{}?)
#	* Allow \include the same way as \input
#	* Allow \includegraphics{} to be split over multiple lines
#	* Check that commands begin with '\', and allow spaces before arguments

# EDIT these two lines for your system....

$Eps2PdfCom = "epstopdf";
$ThisFunCom = "e2pall";

$fname=$ARGV[0];

# check for a *.tex at the end...
if ((-f "$fname")=="" && $fname !~ /.tex$/){
    $fname = "$fname.tex";
}

open(TEXFILE,$fname) or die "Cannot open file $fname";
# print "Finding *.eps files in $fname\n";

$seekingArg = 0;
while($line=<TEXFILE>){
    # truncate $line after % sign....
    $line=~s/%.*//;
    # check for /input....
    if ($line=~/\\input *{([^}]*)}/){
        print `$ThisFunCom $1`;
    }
    # check for /include....
    if ($line=~/\\include *{([^}]*)}/){
        print `$ThisFunCom $1`;
    }

    $base = "";
    if ($line=~/\\includegraphics.*{([^}]*)}/){
        $base = $1;
    }
    elsif ($seekingArg==1 && ($line=~/{([^}]*)}/)){
        $base = $1;
    }
    elsif ($line=~/\\includegraphics/){
        $seekingArg = 1;
    }
    
    if ($base ne "") {
        $seekingArg = 0;
	if ((-f "$base.eps")!="") {
	    $srcfile = "$base.eps";
	}
	elsif ((-f "$base.ps")!=""){
	    $srcfile = "$base.ps";
	}
	else {
	    $srcfile = $base;
	}
        # check that the [e]ps version is newer than the pdf version....
        if ((-M "$base.pdf")=="" || (-M "$base.pdf") >= (-M "$srcfile")){
            print "Constructing \t $base.pdf from $srcfile\n";
            print `$Eps2PdfCom $srcfile`;
        }
        else{
            print "$base.pdf \t is up to date with $srcfile\n";
        }

    };
}
close(TEXFILE);
