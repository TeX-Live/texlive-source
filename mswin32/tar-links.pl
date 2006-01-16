#
# Tool to correct links in tar files.
# Author: F. Popineau
# Date  : 22/12/97
# Time-stamp: <02/12/23 00:33:46 popineau>
#
# Usage: 
#        gzip -dc <file>.tar.gz | tar tvf - | perl tar-links.pl
#
require "newgetopt.pl";
use Cwd;

&main;

exit 0;

sub canon_dir
{
    local ($p, $rep) = @_;
    if ($rep eq '' || $rep eq '\\') {
	$p =~ s@/@\\@g;
	$p =~ s@\\[\\]+@\\@g;
	$p =~ s@\\.\\@\\@g;
	$p =~ s@\\([^\\]+)\\..\\@\\@g;
    }
    elsif ($rep eq '/') {
	$p =~ s@\\@/@g;
	$p =~ s@/[/]+@/@g;
	$p =~ s@/./@/@g;
	$p =~ s@/([^/]+)/../@/@g;
    }
    else {
	die ("canon_dir($p) : invalid separator $rep.\n");
    }
    return $p;
}

# Merges all elements in the list into a single path, adding
# directory separators as needed.
sub newpath
{
    local($p, $s);
    # Concatenates the arguments, adding path separators as needed
    $p = @_[0];
    for ($i = 1; $i <= $#_; $i++) {
	$p = $p . "\\" . $_[$i];
    }
    return &canon_dir($p);
}

sub dirname {
    local ($f) = @_;
    $f =~ m@^(.*[/\\])[^/\\]*@;
    return $1;
}

# Copy source to dest
sub copy
{
    local ($src, $dest) = @_;

    print "Copying $src to $dest\n" if ($opt_verbose);

    die "Copy : source file $src does not exist.\n" 
	unless (-f $src);
#    die "Copy : destination file $dest does not exist.\n" 
#	unless (-f $dest || -d $dest);
    if (-d $dest) {
	$dest = &newpath($dest, &basename($src));
    }
    open IN, "<$src";
    open OUT, ">$dest";
    binmode(IN);
    binmode(OUT);
    print OUT <IN>;
    close(IN);
    close(OUT);
}

sub make_link {
    local ($to, $from) = @_;
    $to = canon_dir($to);
    $from = newpath(dirname($to), $from);
    print "linking $from -> $to ...";
    if (-e $to) {
	unlink($to);
    }
    if (-d $from) {
      system("xcopy $from $to /f/r/i/e/d/k");
    }
    else {
	copy($from, $to);
    }
    print " done\n";
}

sub main {
    while (<>) {
	if ($_ =~ m/([^\s]+)\s*->\s*([^\s]+)/) {
	    make_link($1, $2);
	}
    }
}
