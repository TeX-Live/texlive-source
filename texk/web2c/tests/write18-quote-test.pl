#!/usr/bin/env perl
# $Id$
# Public domain.  Originally written 2010, Karl Berry.
# Check that restricted shell invocation doesn't misquote.

BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `dirname $0`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main
{  
  $badfile = "write18-quote-newfile.tex";
  unlink ($badfile);  # ensure no leftover from previous test
  
  my @args = (qw(-ini -shell-restricted), "$srcdir/tests/write18-quote.tex");
  my $ret = &test_run ("./tex", @args);

  my $bad = -f $badfile;  # file should not have been created
  return $bad;
}
