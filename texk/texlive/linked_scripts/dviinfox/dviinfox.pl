#!/usr/bin/env perl

# dviinfox
# ========
# 
# This script is a joint effort of
#   Dag Langmyhr, Department of Informatics, University of Oslo
#     (dag at ifi.uio.no)
# and
#   Hironobu Yamashita, Japanese TeX Development Community
#     (h.y.acetaminophen at gmail.com)
#
# A program to print information about a DVI file.
#
# Usage: dviinfox [options] file1 file2 ...
# where the flags indicate which information is desired.
#  -f  Give information about the fonts used.
#  -p  Give information about the number of pages.
#  -v  List the program version number.
#  -h  Show help message.
# No options will provide all information available.
#
# Example:
# % dviinfox alltt.dvi
# alltt.dvi: DVI format 2; 3 pages
#   Magnification: 1000/1000
#   Size unit: 1000x25400000/(1000x473628672)dum = 0.054dum = 1.000sp
#   Page size: 407ptx682pt = 14.340cmx23.970cm
#   Stack size: 8
#   Comment: " TeX output 1995.07.07:1513"
#   Font  27:      cmr9 at  9.000 (design size  9.000, checksum=1874103239)
#   Font  26:     cmsy6 at  6.000 (design size  6.000, checksum=1906386187)
#   Font  21:      cmr8 at  8.000 (design size  8.000, checksum=2088458503)
#   Font  20:    cmsy10 at 12.000 (design size 10.000, checksum=555887770)
#   Font  16:     cmr12 at 12.000 (design size 12.000, checksum=1487622411)
#   Font  15:    cmtt12 at 17.280 (design size 12.000, checksum=3750147412)
#   Font  14:     cmr17 at 17.280 (design size 17.280, checksum=1154739572)
#   Font  13:    cmsy10 at 10.000 (design size 10.000, checksum=555887770)
#   Font   7:     cmr10 at 10.000 (design size 10.000, checksum=1274110073)
#   Font   6:      cmr7 at  7.000 (design size  7.000, checksum=3650330706)

use strict;

my $Prog    = "dviinfox";
my $Version = "1.04";
my $Author = "Dag Langmyhr and Hironobu Yamashita";

my $True = 1;
my $False = 0;

my $List_all = $True;
my $List_fonts = $False;
my $List_pages = $False;

# DVI commands:
my $DVI_Filler    = "\337"; # 223 = 0xdf
my $DVI_Font      = "\363"; # 243 = 0xf3
my $DVI_Post      = "\370"; # 248 = 0xf8
my $DVI_Post_post = "\371"; # 249 = 0xf9
my $DVI_Pre       = "\367"; # 247 = 0xf7
my $XDV_Font      = "\374"; # 252 = 0xfc

my $Unit;

if (!@ARGV) {
    show_usage();
    print "Try \"$Prog -h\" for more information.\n";
}

Param:
foreach (@ARGV) {
    /^-f$/ && do {
	$List_fonts = $True;  $List_all = $False;  next Param; };
    /^-p$/ && do {
	$List_pages = $True;  $List_all = $False;  next Param; };
    /^-v$/ && do {
	show_version();  next Param; };
    /^-h$/ && do {
	show_help();  next Param; };
    /^-/ && do {
	print STDERR "$Prog: Unknown option '$_' ignored.\n";  next Param; };
    
    &Read_DVI_file($_);
}

exit 0;

# Only sub definitions from here on

sub show_version {
    print "This is $Prog $Version, by $Author\n";
}

sub show_usage {
    show_version();
    print "Usage: $Prog [-f][-p][-v] file...\n";
}

sub show_help {
    show_usage();
    print "A program to print information about a DVI file.\n";
    print "Options:\n";
    print "  -f  Give information about the fonts used.\n";
    print "  -p  Give information about the number of pages.\n";
    print "  -v  List the program version number.\n";
    print "  -h  Show help message.\n";
    exit 0;
}

sub Read_DVI_file {
    local($_) = @_;
    my ($c, $cn);
    my $IS_XDV = 0;

    print "$_: ";

    open(F, $_) || do { print STDERR "Could not open!\n\n"; exit 1; };

    # First, read info at start of DVI file:

    if (($c = getc(F)) ne $DVI_Pre) {
	printf STDERR ("Not a DVI file (first byte is 0x%02x, not 0x%02x)!\n\n", 
	       ord($c), ord($DVI_Pre));
	close F;  exit 1;
    };

    my $Format  = ord(getc(F));
    my $Numer   = &Read4;
    my $Denom   = &Read4;
    my $Magni   = &Read4;
    my $Comment = &Read_text;

    # Then, read information at the end of the DVI file:

    seek(F, -1, 2);
    while (($c = getc(F)) eq $DVI_Filler) { seek(F, -2, 1); };
    my $VersionID = ord($c);
    # Previously we required equality ($VersionID == $Format). However,
    # it seems ok even when format id (pre) and version id (post_post)
    # are different. TeX4ht allows $VersionID <= 10, so we follow it
    if (($VersionID != $Format) && ($VersionID > 10)) {
	print STDERR "DVI format error (format: $Format vs id: $VersionID)!\n\n";
	close F;  exit 1;
    };
    $IS_XDV = 1 if ($Format > 2);

    seek(F, -6, 1);
    if (($c = getc(F)) ne $DVI_Post_post) {
	$cn = ord($c);
	printf STDERR ("DVI error: Expected POST_POST command, not 0x%02x!\n\n", $cn);
	close F;  exit 1;
    };

    my $Last_post = &Read4;
    seek(F, $Last_post, 0) || do {
	print STDERR "Could not locate position $Last_post!\n\n";
	close F;  exit 1;
    };
    if (($c = getc(F)) ne $DVI_Post) {
	$cn = ord($c);
	printf STDERR ("DVI error: Expected POST command, not 0x%02x!\n\n", $cn);
	close F;  exit 1;
    };

    my $Final_page = &Read4;
    my $Numer2     = &Read4;
    my $Denom2     = &Read4;
    my $Magni2     = &Read4;
    my $Height     = &Read4;
    my $Width      = &Read4;
    my $Stack      = &Read2_u;
    my $Pages      = &Read2_u;

    if ($List_all) {
	print "DVI format $Format";
	print "; id $VersionID" if ($VersionID != $Format);
	print " (pTeX DVI)" if (($Format == 2) && ($VersionID == 3));
	print " (XeTeX XDV)" if ($Format > 2);
	print "; ";
    }
    if ($List_all || $List_pages) {
	print "$Pages page";
	print "s" if ($Pages > 1);
    };

    $Unit = $Magni*$Numer/(1000*$Denom);
    if ($List_all) {
	print "\n  Magnification: $Magni/1000";
	printf("\n  Size unit: %dx$Numer/(1000x$Denom)dum = %5.3fdum = %5.3fsp", 
	       $Magni, $Unit, &Scale_to_sp(1));
	printf("\n  Page size: %dptx%dpt = %5.3fcmx%5.3fcm", 
	       &Scale_to_pt($Width), &Scale_to_pt($Height),
	       &Scale_to_cm($Width), &Scale_to_cm($Height));
	print "\n  Stack size: $Stack";
	print "\n  Comment: \"$Comment\"";
    }
    print "\n";

    if ($List_all || $List_fonts) {
	my ($F_count, $F_check, $F_scale, $F_design, $F_name);
	my ($F_flag, $F_index, $F_colored, $F_extend, $F_slant, $F_embolden);
	my $F_tempswa;
	while (($c = getc(F)) eq $DVI_Font || $c eq $XDV_Font) {
	    # initialize
	    $F_count  = 0;
	    $F_check  = 0;
	    $F_scale  = 0;
	    $F_design = 0;
	    $F_name   = '';
	    $F_flag   = 0;
	    $F_index  = 0;
	    $F_colored = 0;
	    $F_extend = 0;
	    $F_slant = 0;
	    $F_embolden = 0;
	    if ($c eq $DVI_Font) {
		# standard DVI: TFM font definition command
		$F_count  = ord(getc(F));
		$F_check  = &Read4_u;
		$F_scale  = &Read4;
		$F_design = &Read4;
		$F_name   = &Read_text2;
		printf("  Font %3d: %9s at %6.3f", 
		       $F_count, $F_name, &Scale_to_pt($F_scale));
		printf(" (design size %6.3f, ", &Scale_to_pt($F_design));
		print "checksum=$F_check)\n";
	    } else { # $c eq $XDV_Font
		# extended XDV for XeTeX: Native font definition command
		if (!$IS_XDV) {
		    printf STDERR ("Erorr: Command %d used in non-XDV file!\n", ord($XDV_Font));
		    close F;  exit 1;
		}
		$F_count  = &Read4_u;
		$F_scale  = &Read4;
		$F_flag   = &Read2_u;
		$F_name   = &Read_text;
		$F_index  = &Read4_u;
		$F_colored = &Read4_u if ($F_flag & 512);
		$F_extend = &Read4_u if ($F_flag & 4096);
		$F_slant = &Read4_u if ($F_flag & 8192);
		$F_embolden = &Read4_u if ($F_flag & 16384);
		printf("  Native Font %3d: %s at %6.3f", 
		       $F_count, $F_name, &Scale_to_pt($F_scale));
		printf(" (flags 0x%04x, face index %d)\n", $F_flag, $F_index);
		if ($F_flag) {
		    print("        +features: ");
		    $F_tempswa = 0;
		    if ($F_colored) {
			printf("Colored=0x%x", $F_colored);
			$F_tempswa = 1;
		    }
		    if ($F_extend) {
			print(", ") if ($F_tempswa);
			printf("Extend=0x%x", $F_extend);
			$F_tempswa = 1;
		    }
		    if ($F_slant) {
			print(", ") if ($F_tempswa);
			printf("Slant=0x%x", $F_slant);
			$F_tempswa = 1;
		    }
		    if ($F_embolden) {
			print(", ") if ($F_tempswa);
			printf("Embolden=0x%x", $F_embolden);
			$F_tempswa = 1;
		    }
		    print("\n");
		}
	    }
	};

	if ($c ne $DVI_Post_post) {
	    $cn = ord($c);
	    printf STDERR ("DVI error: Expected POST_POST command, not 0x%02x!\n", $cn);
	    close F;  exit 1;
	};

	print "\n";
    };

    close F;
}


# Scale_to_pt (Size)
# -----
# Give the Size (which is in dum, the standard DVI size) in pt.
sub Scale_to_pt {
    return $Unit*$_[0]*72.27/254000;
}


# Scale_to_cm (Size)
# -----------
# Give the Size (which is in dum, the standard DVI size) in cm.
sub Scale_to_cm {
    return &Scale_to_pt($_[0])*2.54/72.27;
}


# Scale_to_sp (Size)
# -----------
# Give the Size (which is in dum, the standard DVI size) in sp.
sub Scale_to_sp {
    return &Scale_to_pt($_[0])*65536;
}


# Read2_u
# -------
# Read an unsigned two-byte value.
sub Read2_u {
    return ord(getc(F))*256 + ord(getc(F));
}


# Read4
# -----
# Read a four-byte value.
# (I assume the value is positive and less than 2^31, so the sign bit
# won't matter.)
sub Read4 {
    return ((ord(getc(F))*256+ord(getc(F)))*256+ord(getc(F)))*256+ord(getc(F));
}


# Read4_u
# -----
# Read an unsigned four-byte value.
# (I don't know why this works for values >=2^31, and Read4 does not,
# but as long as it works...)
sub Read4_u {
    my (@bytes, @sum);

    $bytes[0] = ord(getc(F));  $bytes[1] = ord(getc(F));
    $bytes[2] = ord(getc(F));  $bytes[3] = ord(getc(F));

    $sum[0] = $bytes[0]*256 + $bytes[1];
    $sum[1] = $bytes[2]*256 + $bytes[3];

    return $sum[0]*65536 + $sum[1];
}


# Read_text
# ---------
# Read a text (a one-byte length and the the text byte).
sub Read_text {
    local($_);
    my ($Leng, $Res);

    $Leng = ord(getc(F));  read(F, $Res, $Leng);
    return $Res;
}


# Read_text2
# ----------
# Like 'Read_text', but the length is the sum of two bytes.
sub Read_text2 {
    local($_);
    my ($Leng, $Res);

    $Leng = ord(getc(F)) + ord(getc(F));  read(F, $Res, $Leng);
    return $Res;
}
