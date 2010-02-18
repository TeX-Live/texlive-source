# $Id: common-test.pl 16695 2010-01-13 01:18:02Z karl $
# Public domain.  Originally written 2010, Karl Berry.
# Common definitions for Perl tests in TeX Live.  We want to use Perl to
# have a chance of running the tests on Windows.

$ENV{"TEXMFCNF"} = "$srcdir/../kpathsea";
$ENV{"BIBINPUTS"} = "$srcdir/tests";
$ENV{"BSTINPUTS"} = "$srcdir/tests";
$ENV{"TEXINPUTS"} = "$srcdir/tests";

# Run PROG with ARGS.  Return the exit status.
# Die if PROG is not executable.
#
sub test_run
{
  my ($prog, @args) = @_;

  # Possibly we should check that $prog starts with ./, since we always
  # want to run out of the build dir.  I think.
  die "$0: no program $prog in " . `pwd` if ! -x $prog;
  
  my $ret = system ($prog, @args);  
  return $ret;
}
