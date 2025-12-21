# $Id: common-test.pl 16695 2010-01-13 01:18:02Z karl $
# Public domain.  Originally written 2010, Karl Berry.
# Common definitions for Perl tests in TeX Live.  We want to use Perl to
# have a chance of running the tests on Windows.

# Try to avoid output files (nested-range.ind) not being created due to
# caching, although apparently this is not likely to help. When the
# subprogram exits, that should ensure all caches are updated..
$| = 1;

# Find absolute path.
chomp (my $TL_TESTS_DIR = `cd "$srcdir/../tests" && pwd`);

# srcdir must be a sibling dir to kpathsea, e.g., web2c.
$ENV{"TEXMFCNF"} = "$srcdir/../kpathsea";
$ENV{"AFMFONTS"}
  = $ENV{"BIBINPUTS"}
  = $ENV{"BSTINPUTS"}
  = $ENV{"TEXINPUTS"}
  = ".:$srcdir/tests:$srcdir/../tests/texmf//";


# Run PROG with ARGS.  Return the exit status.
# Die if PROG is not executable.
#
sub test_run {
  my ($prog, @args) = @_;

  # Possibly we should check that $prog starts with ./, since we always
  # want to run out of the build dir.  I think.
  die "$0: no program $prog in " . `pwd` if ! -x $prog;
  
  # use local pm files and kpsewhich.
  $ENV{"PERL5LIB"} = $TL_TESTS_DIR;
  $ENV{"PATH"} = "../kpathsea:$ENV{PATH}";

  # Won't be copyable with weird names, but should get the info across.
  print "$0: running ", $prog, " ", join (" ", @args), "\n";
  
  # Run it.
  my $ret = system ($prog, @args);  
  return $ret;
}

sub test_file_copy {
  my ($srcfile,$dstfile) = @_;
  
  # don't copy onto itself.
  chomp (my $srcdir = `dirname $srcfile`);
  chomp ($srcdir = `cd $srcdir && pwd`);
  #
  chomp (my $dstdir = `dirname $dstfile`);
  chomp ($dstdir = `cd $dstdir && pwd`);
  return if $srcdir eq $dstdir;
  
  local *IN;
  $IN = "<$srcfile";
  open (IN) || die "open($srcfile) failed: $!";
  my @in = <IN>;
  close (IN) || warn "close($srcfile) failed: $!";
  
  local *OUT;
  $OUT = ">$dstfile";
  open (OUT) || die "open($dstfile) failed: $!";
  print (OUT @in) || die "print($dstfile) failed: $!";
  close (OUT) || warn "close($dstfile) failed: $!";
  
  return 0;
}
