# Values from c-auto.h

$mk_NOTOOL='';
$mk_GREY='';
$mk_BUTTONS='';
$mk_PS_GS='';
$mk_GS_PATH='gswin32c';
$mk_PS_DPS='';
$mk_PS_NEWS='';
$mk_A4='';

# HyperTeX: There appears to be no (easy) way to disable this
$mk_HTEX=1;

# Kpathsea defaults, they are #if'ed in the source but there is no
# config option to disable them.
$mk_SELFILE=1;
$mk_KPATHSEA=1;

# Optional values from CFLAGS

$mk_MFMODE='unspecified';
$mk_BDPI=600;
$mk_DEFAULT_FONT_SIZES='m0:m0.5:m1:m2:m3:m4:m5';
$mk_SHRINK=8;

&main;

exit 0;

sub arg_cflags 
{
    while(<@ARGV>) {
	if ($_ =~ m/-D(.*)=(.*)/) {
	    eval ("\$mk_$1=$2\n");
	}
	elsif ($_ =~ m/-D(.*)/ ) {
	    eval ("\$mk_$1=1\n");
	}
    }
}

sub cauto_cflags
{
    open IN, "<c-auto.h";
    while (<IN>) {
	if ($_ =~ m/#define\s+(\S+)\s*=\s*(\S+)/ ) {
	    eval("\$mk_$1=$2");
	}
    }
    close IN;
}

sub main
{
    &arg_cflags();
    &cauto_cflags();

    if ($mk_NOTOOL ne '') {
	$mk_BUTTONS='';
    }
    if ($mk_GREY eq '') {
	print  "/^#ifgrey/,/^#/d\n";		
    }
    if ($mk_BUTTONS eq '') {
	print  "/^#ifbuttons/,/^#/d\n";	
    }
    if ($mk_PS_DPS eq '') {
	print  "/^#ifdps/,/^#/d\n";		
    }
    if ($mk_PS_NEWS eq '') {
	print  "/^#ifnews/,/^#/d\n";		
    }
    if ($mk_HTEX eq '') {
	print  "/^#ifhyper/,/^#/d\n";		
    }
    if ($mk_NOGRID ne '') {
	print "/^#ifgrid/,/^#/d\n";
    }

    if ($mk_KPATHSEA eq '') {
	print "/^#ifkpathsea/,/^#/d\n";
    }
    else {
	print "/^#ifnokpathsea/,/^#/d\n";
    }

# -DNOSELFILE is supposed to override everything
    if ($mk_NOSELFILE == 1) {
	print "/^#ifselfile/,/^#/d\n";
    }
    elsif ($$mk_SELFILE == 1) {
	print "/^#ifnoselfile/,/^#/d\n";
    }

# See HTEX comment above    
# if test -z "$mk_HAVE_LIBWWW" || test -z "$mk_HAVE_WWWLIB_H";
# 				then echo '/^#ifhyper/,/^#/d';		fi

# Process remaining PostScript emulation arguments

    if ($mk_PS_GS eq '') {
	print "/^#ifghost/,/^#/d\n";
    }
    else {
	print "s^\@GS_PATH\@^$mk_GS_PATH^\n";
    }

    if ($mk_PS_GS ne '' || $mk_PS_DPS ne '' || $mk_PS_NEWS ne '') {
	$mk_PS='y';
	print "/^#ifnops/,/^#/d\n";
    }
    else {
	$mk_PS='';
	print "/^#ifps/,/^#/d\n";
    }

# Miscellaneous args

    if ($mk_A4 ne '') {
	print "s/\@DEFAULT_PAGE_SIZE\@/21 x 29.7 cm (A4 size)/\n";
	print "s/\@DEFAULT_UNIT\@/cm \" (centimeters)\"/\n";
    }
    else {
	print "s/\@DEFAULT_PAGE_SIZE\@/8.5 x 11 inches/\n";
	print "s/\@DEFAULT_UNIT\@/in \" (inches)\"/\n";
    }

    print "s^\@MFMODE\@^$mk_MFMODE^\n";
    print "s/\@BDPI\@/$mk_BDPI/\n";
    print "s/\@SHRINK\@/$mk_SHRINK/\n";

    $tmp = "s/\@DEFAULT_FONT_SIZES\@/$mk_DEFAULT_FONT_SIZES/";
    $tmp =~ s/:/ :\\\\\\/g;
    print "$tmp\n";
# Remove all remaining '#' lines

    print	"/^#/d\n";

# Done

    exit 0;
}

