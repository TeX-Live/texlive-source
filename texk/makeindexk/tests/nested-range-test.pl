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
  my $IND = "nested-range.ind";

  my @test_args = ("./makeindex", "$srcdir/tests/nested-range.idx",
                       "-o", $IND,
                       "-t", "nested-range.ilg");
  my $ret = &test_run (@test_args);
  if ($ret != 0) {
    warn "test_run failed ($ret): $! (cmd=@test_args)\n";
    die `pwd; ls -lt`;
  }
  
  my $bad = 0;
  
  # Seems that unpredictably the output file doesn't exist yet?
  if (! -e $IND) {
    warn "test_run succeeded, but $IND doesn't exist? (cmd=@test_args)\n";
    warn `pwd; ls -lt; /l/bin/proc`;
  }
  
  # The test fails if the output contains \(.
  open (IND, $IND) || die "open($IND) failed: $!";
  while (<IND>) {
    $bad = 1 if /\\\(/;
  }
  close (IND) || die "close($IND) failed: $!";
  
  return $bad;
}
