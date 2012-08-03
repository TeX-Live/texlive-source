#!/usr/bin/env perl
# $Id$
# Public domain.  Perl version originally written by Karl Berry, 2012.
# Earlier shell version by Thomas Esser.
# 
# Generic -sys wrapper.  Symlink to this script as updmap-sys,
# fmtutil-sys, etc.

&main ();  # never returns.

sub main {
  chomp (my $v = `kpsewhich -var-value TEXMFSYSVAR`);
  die "$0: could not retrieve TEXMFSYSVAR value with kpsewhich: $!"
    if ! $v;
  $ENV{"TEXMFVAR"} = $v;
  
  chomp (my $c = `kpsewhich -var-value TEXMFSYSCONFIG`);
  die "$0: could not retrieve TEXMFSYSCONFIG value with kpsewhich: $!"
    if ! $c;
  $ENV{"TEXMFCONFIG"} = $c;
  
  # updmap-sys -> updmap, etc.  Case insensitive needed for Windows,
  # but doesn't seem bad on Unix either.
  (my $progname = $0) =~ s/-sys$//i;
  die "$0: program name does not end in -sys" if $progname eq $0;

  #warn "doing exec $0, $progname, @ARGV";
  
  # execute the program name we computed, with whatever arguments we have.
  exec ($progname, @ARGV);
  
  # should never get here.
  die "$0: exec($progname, @ARGV) failed: $!";
}
