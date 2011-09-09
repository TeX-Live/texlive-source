#!/usr/bin/env perl
# $Id: bibtex-openout-test.pl 17384 2010-03-08 18:49:47Z karl $
# Public domain.  Originally written 2011, Karl Berry.
# Check that updmap can parse various cmdline invocations.

# srcdir = tetex (in the source tree)
BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `cd \`dirname $0\`/.. && pwd`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main {
  my $ret = &test_run ("$srcdir/updmap.pl", "-n", "--quiet",
                       #"--enable", "Map=foo"
                       );

  # The test fails if the program succeeded.  Could also check the output.
  my $bad = ($ret == 0);
  return $bad;
}
