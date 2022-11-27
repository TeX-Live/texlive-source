#!/usr/bin/env perl
#
# Public domain.  Originally written 2010, Karl Berry.
# Check that upbibtex respects openout_any.

# srcdir = web2c (in the source tree)
BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `cd \`dirname $0\`/.. && pwd`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main
{
  $ENV{"openout_any"} = "p";  # should already be in texmf.cnf, but ...
  
  # .blg open of abs path should fail:
  my $ret = &test_run ("./upbibtex", "$srcdir/tests/exampl.aux");

  # The test fails if the program succeeded.  Could also check the output.
  my $bad = ($ret == 0);
  return $bad;
}
