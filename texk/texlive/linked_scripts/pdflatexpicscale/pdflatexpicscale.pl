#!/usr/bin/perl -w
# Scale pictures to be included into a LaTeX generated PDF file.
# written by peter willadt 
# contact: willadt at t-online.de
# this software is subject to the LaTeX project public license.
# changes:
# 2016-07-27 first public release
# 2016-08-02 changed regex to prevent problem with long filenames
# 2016-08-02 changed > to gt (shame on me)
# 2016-10-20 corrected behaviour when program gets called without arguments
#            (shame on me, again)
# 2016-10-20 added undocumented switch --help to show usage.
# 2017-01-23 Updated documentation (see pdflatexpicscale.tex), 
#            and corrected version number 
#            Please note: version numbers will be updated to keep sync
#            with the documentation, even when the perl code does not change.
# 2017-02-01 pdflatexpicscale did not work correct when log lines were 
#            wrapped twice. This is fixed.
#
use strict;
use File::Basename;
use File::Spec;
use File::Copy;
use Getopt::Long;

my $version = '0.32';
my $versiondate = '2018-02-12';           #version %version
my $showversion;

my $verbose;
my $help;
my $TeXproject;

# folders for scaled graphics, relative names
my $printfolderprefix   = 'printimg';
my $previewfolderprefix = 'previewimg';
my $srcfolder           = '';

# final resolution. special b/w handling not implemented yet
my $printdpi   = 300;
my $printbwdpi = 4 * $printdpi;
my $previewdpi = 72;
my $tolerance  = 20;

# file formats
my %canhandle =
( JPG => '1',
  PNG => '1',
);

# bookkeeping
my %processedimages;
my $copied  = 0;
my $scaled  = 0;
my $skipped = 0;

sub printInplaceError
{
    my $filename = shift;
    print "I will not do inplace modification of graphics:\n" .
	"($filename) will not be processed. (Missing path?)\n";
}

sub printMissingError
{
    my $filename = shift;
    print "I can not find ($filename). (What have you done?)\n";
}

sub handleImage
{
    my $filename  = shift;
    my $dstwidth  = shift;
    my $dstheight = shift;
    my ($srcwidth, $srcheight, $srcdepth);
    my $dstfilename;
    my $xfilename;
    my @xsrcdirs;
    my @dstdirs;
    my @convertargs;
    my $idstring;
    print "$filename\n";
    $dstdirs[0] = '.';
    $dstdirs[1] = $printfolderprefix;
    $dstfilename = File::Spec->catfile(@dstdirs, basename($filename));
    if($filename eq $dstfilename){
	unless ($srcfolder gt ' ') {
	    printInplaceError($filename);
	    $skipped++;
	    return;
	}
	$xsrcdirs[0] = $srcfolder;
	$xfilename = File::Spec->catfile(@xsrcdirs, basename($filename));
	unless (-e $xfilename){
	    printMissingError($xfilename);
	    $skipped++;
	    return;
	} 
	$filename = $xfilename;
    }
    # check for image width and height
    $idstring = `identify -format "%w x %h %[depth] " $filename`;
    return unless $idstring =~ /(\d+) x (\d+) (\d+)/;
    $srcwidth = $1; $srcheight = $2; $srcdepth = $3;
    if($srcdepth == 1) { # hires for bw pix
	$dstwidth  = int($dstwidth  * $printbwdpi / 72.27 + .5);
	$dstheight = int($dstheight * $printbwdpi / 72.27 + .5);
    }else {              # moderate res for any other
	$dstwidth  = int($dstwidth  * $printdpi / 72.27 + .5);
	$dstheight = int($dstheight * $printdpi / 72.27 + .5);
    }
    if(-e $dstfilename){
	# if younger and requested size: skip
	my ($mtime1, $mtime2, $size1, $size2, $idstring2, $existwidth);
	my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size,
	    $atime, $ctime, $blksize, $blocks);
	($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size,
	    $atime, $mtime1, $ctime, $blksize, $blocks)
           = stat($dstfilename);
	($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size,
        $atime, $mtime2, $ctime, $blksize, $blocks)
           = stat($filename);
	if($mtime1 >= $mtime2){
	    $idstring2 = `identify -format "%w x %h %[depth] " $dstfilename`;
	    if($idstring2 =~ /(\d+) x (\d+) (\d+)/){
		$existwidth = $1; 
		if($existwidth == $dstwidth){
		    $processedimages{$filename} = $existwidth;
		    $skipped++;
		    return;
		}
	    }
	}
    }
    if(($dstwidth >= $srcwidth) ||
       ($dstwidth *(100 + $tolerance)/100 >= $srcwidth)) {
	# we do no upscaling, but will probably copy the file.
	if (defined $processedimages{$filename}){
	    $skipped++;
	}else {
	    $processedimages{$filename} = $srcwidth;
	    print "will copy $filename to $dstfilename\n" if $verbose;
	    copy($filename, $dstfilename);
	    $copied++;
	    return;
	}	    
    }else {
	if((defined $processedimages{$filename}) && 
	   ($processedimages{$filename} >= $dstwidth)) {
	    $skipped++;
	    return;
	}
	# we do some downscaling
	$processedimages{$filename} = $dstwidth;
	$convertargs[0] = 'convert';
	$convertargs[1] = ${filename};
	$convertargs[2] = '-resize';
#	$convertargs[3] = "${dstwidth}x${dstheight}!";
	$convertargs[3] = "${dstwidth}x${dstheight}";
	$convertargs[4] = "-density";
	$convertargs[5] = $printdpi;
	$convertargs[6] = ${dstfilename};
	print "will @convertargs\n"  if $verbose;
	system(@convertargs);
	$scaled++;
    }
}

sub readlog
{
    my $logfilename = shift;
    $logfilename .= '.log';
    my ($picname, $picext, $picwidth, $picheight);
    my ($picextu, $state, $buffer);
    open LOGFILE, "<$logfilename" or die "Cannot read $logfilename";
    $state = 0;
    while (<LOGFILE>){
	if (/^Package pdftex\.def\sInfo\:\s/){
	    $buffer = $_;
	    unless ($buffer =~ /\sused/){
		chomp $buffer;
		$buffer .= <LOGFILE>;
	    } # twice ought to be enough
	    unless ($buffer =~ /\sused/){
		chomp $buffer;
		$buffer .= <LOGFILE>;
	    }
	    if($buffer =~ /Info:\s(\S*)\.(\w+)\sused/){
		$picname = $1;
		$picext = $2;
		($picextu = $picext) =~ tr/a-z/A-Z/;
		if(defined $canhandle{$picextu}){
		    $state = 1;
		    next;
		}
	    }
	}
	next unless ($state == 1);
	next unless /Requested size: (\d+\.\d+)pt x (\d+\.\d+)pt/;
	$picwidth  = $1;
	$picheight = $2;
	handleImage("$picname.$picext", $picwidth, $picheight);
	$state = 0;
    }
}

sub usage
{
    print <<'_endusage';
pdflatexpicscale will downscale large raster images for your TeX project
to reasonable size. 
Basic usage:
   pdflatexpicscale myarticle
where your main LaTeX file is called 'myarticle.tex' and a log file exists.
Fine tuning:
--destdir=folder (relative path, must exist, 
                  defaults to 'printimg' subfolder of current folder)
--srcdir=folder  (folder to find original images, if this software 
                  can't figure it out on it's own, because you already
                  used the target directory.)
--printdpi=XXX   (default 300)
--tolerance=XX   (default 20 Percents over printdpi to ignore scaling)
--verbose        (if you want to know what the script attempts to do)
--version        (show software version)
_endusage
}

GetOptions('verbose'       => \$verbose,
	   'printdpi=i'    => \$printdpi,
	   'destdir=s'     => \$printfolderprefix,
	   'tolerance=i'   => \$tolerance,
           'srcdir=s'      => \$srcfolder,
           'version'       => \$showversion,
	   'help'          => \$help,
	   'usage'         => \$help,
    );


if($showversion){
    print "pdflatexpicscale Version $version $versiondate\n";
}

if($help){
    usage();
}

$TeXproject = shift;

if((defined $TeXproject) && ($TeXproject gt ' ')){
    readlog($TeXproject);
    if($verbose) {
	print "pdflatexpicscale Version $version:\n"
	    . "$copied file(s) copied, $scaled file(s) converted " .
	    "and $skipped occurence(s) skipped for:\n${TeXproject}.log.\n";
    }
}else {    
    usage();
}
