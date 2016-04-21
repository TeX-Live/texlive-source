#!/usr/bin/env perl
# $Id$
# Public domain.  Originally written 2011, Karl Berry.
# Check that updmap can parse various cmdline invocations.

# srcdir = texlive (in the source tree)
BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `cd \`dirname $0\`/.. && pwd`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main {
  my $ret = &test_run ("$srcdir/linked_scripts/texlive/updmap.pl","--version");

  # erroneous invocations we should check for and/or handle better:
  #   updmap foo
  #   updmap --enable Map

  my $bad = $ret != 0;
  return $bad;
}
