#!/usr/bin/env perl
# $Id$
# Public domain.  Originally written 2011, Karl Berry.
# Check that makeindex doesn't create spurious output from nested ranges.
# See nested-range.tex and -bb.tex.

# srcdir = makeindexk (in the source tree)
BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `cd \`dirname $0\`/.. && pwd`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main {
  my $ret = &test_run ("./makeindex", "$srcdir/tests/nested-range.idx",
                       "-o", "nested-range.ind",
                       "-t", "nested-range.ilg");

  # The test fails if the output contains \(.
  my $bad = 0;
  local *IND;
  $IND = "nested-range.ind";
  open (IND) || die "open($IND) failed: $!";
  while (<IND>) {
    $bad = 1 if /\\\(/;
  }
  close (IND) || die "close($IND) failed: $!";
  
  return $bad;
}
