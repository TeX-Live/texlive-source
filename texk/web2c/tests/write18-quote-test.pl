#!/usr/bin/env perl
# $Id$
# Copyright 2010 Karl Berry <tex-live@tug.org>.
# You may freely use, modify and/or distribute this file.
# Check that restricted shell invocation doesn't misquote.

BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `pwd`); }
require "$srcdir/tests/common-test.pl";

exit (&main ());

sub main
{  
  $badfile = "write18-quote-newfile.tex";
  unlink ($badfile);  # ensure no leftover from previous test
  
  my @args = qw(-ini -shell-restricted $srcdir/tests/write18-quote.tex);
  my $ret = &test_run ("./tex", @args);

  my $bad = -f $badfile;  # file should not have been created
  return $bad;
}
