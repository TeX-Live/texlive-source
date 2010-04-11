#!/usr/bin/env perl
# $Id$
# Public domain.  Originally written 2010, Karl Berry.
# Check that afm2tfm functions at all.

BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `dirname $0`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main
{
  my $outfile = "afmtest.tfm";
  my @args = ("afmtest.afm", $outfile);  # should be found via kpse
  my $ret = &test_run ("./afm2tfm", @args);

  my $bad = ! -f $outfile;  # tfm should have been created
  return $bad;
}
