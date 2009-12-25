#!/usr/bin/env perl
# $Id$
# Copyright 2009 Karl Berry <tex-live@tug.org>.
# You may freely use, modify and/or distribute this file.
# Check that bibtex respects openout_any.

BEGIN {
  chomp ($mydir = `dirname $0`);
  unshift (@INC, "$mydir");
}

exit (&main ());

sub main
{
  # attempt at making it possible to run this when srcdir=builddir.
  $ENV{"srcdir"} = $mydir if ! exists $ENV{"srcdir"};
  my $srcdir = $ENV{"srcdir"};

  $ENV{"TEXMFCNF"} = "$srcdir/../kpathsea";
  $ENV{"BIBINPUTS"} = "$srcdir/tests";
  $ENV{"BSTINPUTS"} = "$srcdir/tests";
  
  $ENV{"openout_any"} = "p";  # should already be in texmf.cnf, but ...
  
  my $prog = "./bibtex";
  die "$0: no program $prog in " . `pwd` if ! -x $prog;

  my @args = ("$srcdir/exampl.aux");  # blg open of abs path should fail

  $ret = system ($prog, @args);
  #warn ("ret=$ret in " . `pwd`);

  # The test fails if the program succeeded.  Ideally we'd grep the output.
  my $bad = ($ret == 0);
  return $bad;
}
