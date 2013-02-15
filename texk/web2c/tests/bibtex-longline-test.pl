#!/usr/bin/env perl
# $Id: bibtex-openout-test.pl 17085 2010-02-18 18:03:18Z karl $
# Public domain.  Originally written 2010, Karl Berry.
# Check that bibtex does not break long strings (change in 2010).

# srcdir = web2c (in the source tree)
BEGIN { chomp ($srcdir = $ENV{"srcdir"} || `cd \`dirname $0\`/.. && pwd`); }
require "$srcdir/../tests/common-test.pl";

exit (&main ());

sub main
{
  # The blg and bbl file names are based on the aux name and cannot be
  # overridden.  We can't write to the aux (source) directory, though,
  # because that's an absolute path and openout_any=p.  Therefore, copy
  # the input aux file to our working directory.
  &test_file_copy ("$srcdir/tests/longline.aux", "./longline.aux");
  
  # Run BibTeX, quit if it fails.
  my $ret = &test_run ("./bibtex", "./longline.aux");
  return 1 if $ret != 0;

  # There should be lines longer than 80 chars in the output.
  # (In older versions of BibTeX, they are forcibly split, with a %.)
  local *IN;
  $IN = "longline.bbl";
  open (IN) || die "open($IN) failed: $!";
  while (<IN>) {
    last if length ($_) >= 80;
  }
  
  # We failed if all lines were < 80.
  my $bad = ! (length $_ >= 80);
  return $bad;
}
