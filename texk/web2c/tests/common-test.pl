# $Id$
# Copyright 2010 Karl Berry <tex-live@tug.org>.
# You may freely use, modify and/or distribute this file.
# Common definitions for Perl tests in TeX Live.

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
