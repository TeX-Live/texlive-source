#! /bin/sh --
eval '(exit $?0)' && eval 'PERL_BADLANG=x;export PERL_BADLANG;: \
;exec perl -x -S -- "$0" ${1+"$@"};#'if 0;
eval 'setenv PERL_BADLANG x;exec perl -x -S -- "$0" $argv:q;#'.q
#!perl -w
+($0=~/(.*)/s);do$1;die$@if$@;__END__+if 0;
# Don't touch/remove lines 1--7: http://www.inf.bme.hu/~pts/Magic.Perl.Header

#
# hq.pl -- convert binary STDIN to C language "quoted\\-string"
# by pts@fazekas.hu at Tue Dec  3 17:46:52 CET 2002
#
# VC6.0 imposes the following restrictions:
# -- the number of (destination) characters inside a single "..." block may
#    not exceed 2048
# -- the number of "..." blocks may not exceed 999 if the /Zm999 compiler
#    option is specified; this number should be minimized
# -- "\12319" causes a stupid warning, should be emitted as: "\123\51\61"
# -- output lines should not be too long (I limit them to 79 chars)
#

my @numenc=('\60','\61','\62','\63','\64','\65','\66','\67','\70','\71');
my @enc;
for (my $I=0;$I<256;$I++) { $enc[$I]=sprintf"\\%03o",$I }

binmode STDIN if @ARGV;
binmode STDOUT; # UNIX NL in output
while (0<read STDIN, $_, 2040) {
  s/([^\w\.\/\- ])(\d*)/$enc[ord$1].join("",@numenc[split"",$2])/ge;
  s/(.{74}[^\\]{0,3})/$1\\\n/gs;
  # ^^^ Dat: [^\\]{0,3} is for ensuring not breaking a \\
  print "\"$_\"\n";
}
