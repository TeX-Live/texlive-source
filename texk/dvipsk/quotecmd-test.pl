#!/usr/bin/env perl
# $Id$
# Public domain.  Originally written 2010, Karl Berry.
# Check that dvips quotes external command arguments.

BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `dirname $0`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main
{
  # create the weirdly-named file which dvips executes with popen.
  # quotecmd.tex also creates it, but we don't want to run TeX in
  # this test, nor do we want such a weirdly-named file in our
  # repository, so create it here.  The names here and in quotecmd.tex
  # must match exactly or the test won't be testing what it should.
  # 
  # Leave the file in place, so we can conveniently run dvips under the
  # debugger if we need to.
  my $weirdf = ' 2>&1 | echo badnews >pwned.txt #.gz';
  unlink ($weirdf);  # ensure no leftover
  open (WEIRDF, ">", $weirdf);
  close (WEIRDF) || die "open(weird file name) failed: $!";
  
  $badfile = "pwned.txt";  # will be created if program is broken
  unlink ($badfile);       # ensure no leftover from previous test
  
  my @args = ("$srcdir/testdata/quotecmd.dvi", qw(-o /dev/null));
  my $ret = &test_run ("./dvips", @args);

  my $bad = -f $badfile;  # file should not have been created
  return $bad;
}
