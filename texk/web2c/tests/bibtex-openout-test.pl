#!/usr/bin/env perl
# $Id$
# Copyright 2009, 2010 Karl Berry <tex-live@tug.org>.
# You may freely use, modify and/or distribute this file.
# Check that bibtex respects openout_any.

BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `pwd`); }
require "$srcdir/tests/common-test.pl";

exit (&main ());

sub main
{
  $ENV{"openout_any"} = "p";  # should already be in texmf.cnf, but ...
  
  # .blg open of abs path should fail
  my $ret = &test_run ("./bibtex", "$srcdir/exampl.aux");

  # The test fails if the program succeeded.  Ideally we'd grep the output.
  my $bad = ($ret == 0);
  return $bad;
}
