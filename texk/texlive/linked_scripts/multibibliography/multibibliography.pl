#!/usr/bin/perl
# Usage: perl multibibliography.pl jobname
# Version: 2011-04-11T10:18Z
# By Yannis Haralambous and Michael Cohen

# Changes by Boris Veytsman, $Revision: 1.4 $, $Date: 2013-03-26 20:56:02 $

# This perl script does the bibtexing part, generating 3 separate .bbl
# files with descriptive names derived from source file name, so no
# need to separately invoke.

use strict;

sub cleanup {
    local $/=undef;
    my $file = shift;
    open IN, $file;
    my $BIG=<IN>;
    close IN;
    $BIG=~s/(\r\n|\r|\n)/ /g;
    $BIG=~s/\\bibitem/\n\n\\bibitem/g;
    $BIG=~s/\\newblock/\n\\newblock/g;
    $BIG=~s/\\end\{thebibliography\}/\n\n\\end{thebibliography}/g;
    $BIG=~s/[ ]+/ /g;
    open OUT, ">$file";
    print OUT $BIG;
    close OUT;
}

# First, change "apalike" in the aux file to "unsrt" and generate bbl
# file, renamed to "fn-sequence.bbl".
my $filename=$ARGV[0];
$filename=~s/\.(tex|bib|bbl|aux|log)$//;
if (-e $filename.".aux") {
    open IN, $filename.".aux";
    my $BIG="";
    while (<IN>) {
	if (m/\\bibstyle\{apalike\}/) {
	    $BIG .= "\\bibstyle{unsrt}\n";
	} else {
	    $BIG .= $_;
	}
    }
    close IN;
    open OUT, ">".$filename.".aux";
    print OUT $BIG;
    close OUT;
    system("bibtex $filename");
    system("mv $filename".".bbl $filename"."-sequence.bbl");
   
    # Now, change "unsrt" in the aux file to "chronological" 
    # and generate bbl file, renamed to "fn-timeline.bbl".

    open IN, $filename.".aux";
    $BIG="";
    while (<IN>) {
	if (m/\\bibstyle\{unsrt\}/) {
	    $BIG .= "\\bibstyle{chronological}\n";
	} else {
	    $BIG .= $_;
	}
    }
    close IN;
    open OUT, ">".$filename.".aux";
    print OUT $BIG;
    close OUT;
    system("bibtex $filename");
    system("mv $filename".".bbl $filename"."-timeline.bbl");
    
    # Lastly, change "chronological" in the aux file to "apalike" 
    # and generate bbl file, retaining name "fn.bbl".

    open IN, $filename.".aux";
    $BIG="";
    while (<IN>) {
	if (m/\\bibstyle\{chronological\}/) {
	    $BIG .= "\\bibstyle{apalike}\n";
	} else {
	    $BIG .= $_;
	}
    }
    close IN;
    open OUT, ">".$filename.".aux";
    print OUT $BIG;
    close OUT;
    system("bibtex $filename");
    
    &cleanup($filename.".bbl");
    &cleanup($filename."-sequence.bbl");
    &cleanup($filename."-timeline.bbl");
    
    open IN, $filename."-sequence.bbl";
    my $counter=1;
    my %UNSRT;
    my %DATE;
    my %NAME;
    while (<IN>) {
	if (m/\\bibitem{([^\}]+?)}/) {
	    $UNSRT{$1}=$counter;
	    $counter++;
	}
    }
    close IN;
    
    open IN, $filename.".bbl";
    $counter=1;
    while (<IN>) {
	if (m/\\bibitem\[([^\]]+)\]{([^}]+?)}/) {
	    my $longdesc=$1;
	    my $label=$2;
	    if ($longdesc =~ /^(.*),\s+(\S+)$/) {
		$NAME{$label}=$1;
	        $DATE{$label}=$2;
	    }
        }
    }
    close IN;
    
    open IN, $filename."-sequence.bbl";
    $counter=1;
    open OUT, ">tmp123456";
    while (<IN>) {
	if (s/\\bibitem{(.+?)}/\\bibitem[\\MBlabel{$counter}{$NAME{$1}}{$DATE{$1}}]{$1}\n/) {
	    $counter++;
	}
	print OUT;
    }
    close IN;
    close OUT;
    system("mv tmp123456 $filename"."-sequence.bbl");
    
    open IN, $filename.".bbl";
    open OUT, ">tmp123456";
    while (<IN>) {
	s/\\bibitem\[[^\]]+\]{(.+?)}/\\bibitem[\\MBlabel{$UNSRT{$1}}{$NAME{$1}}{$DATE{$1}}]{$1}/;
	print OUT $_;
    }
    close IN;
    close OUT;
    system("mv tmp123456 $filename".".bbl");
    
    open IN, $filename."-timeline.bbl";
    open OUT, ">tmp123456";
    while (<IN>) {
		s/\\bibitem\[[^\]]+\]{(.+?)}/\\bibitem[\\MBlabel{$UNSRT{$1}}{$NAME{$1}}{$DATE{$1}}]{$1}/;
	print OUT $_;
    }
    close IN;
    close OUT;
    system("mv tmp123456 $filename"."-timeline.bbl");
}



