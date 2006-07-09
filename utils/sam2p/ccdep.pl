#! /bin/sh --
eval '(exit $?0)' && eval 'PERL_BADLANG=x;export PERL_BADLANG;: \
;exec perl -T -S -- "$0" ${1+"$@"};#'if 0;
eval 'setenv PERL_BADLANG x;exec perl -x -S -- "$0" $argv:q;#'.q
#!perl -w
+($0=~/(.*)/s);do$1;die$@if$@;__END__+if 0;
# Don't touch/remove lines 1--7: http://www.inf.bme.hu/~pts/Magic.Perl.Header
#
# ccdep.pl v0.32 -- semiautomatic dependency discovery for C/C++ programs
# by pts@math.bme.hu at Fri May 31 13:36:29 CEST 2002
# 0.31 by pts@math.bme.hu at Sat Jun  1 15:19:55 CEST 2002
# 0.32 by pts@math.bme.hu at Tue Sep  3 19:12:20 CEST 2002
# 0.33 by pts@math.bme.hu at Thu Oct 31 09:47:25 CET 2002
#
# Dat: no -T (tainting checks) anymore, does bad to our readpipe()
# OK : emit `TARGETS_yes = ...'
# Imp: make #warning etc. in *.h files work as expected
# OK : generated.h
# Imp: add external libraries (-L...)
# Imp: abort if gcc command not found...
# Imp: abort if not all .o are mentioned in gcc output
# Imp: avoid $ etc. in Makefile
# OK : all 8 combinations of PROVIDES|REQUIRES|CONFLICTS
#
BEGIN { eval { require integer; import integer } }
BEGIN { eval { require strict ; import strict  } }

# Typical invocation: ccdep.pl --FAL=assert,no,yes,checker $(CXX)
my @FAL=();
if (@ARGV and $ARGV[0]=~/\A--FAL=(.*)/s) { @FAL=split/\W+/,$1; shift@ARGV }
my $GCCP; # 'g++' or 'gcc': C/C++ preproc with `-E -MG' switches
$GCCP="@ARGV";
$GCCP="gcc" if $GCCP!~y/ \t//c;

# ---

sub find_ds() {
  #** @return a list of .ds file in the current directory
  my @L;  my $E;
  die unless opendir DIR, '.';
  while (defined($E=readdir DIR)) {
    push @L, $E if $E=~/\.(c(|c|xx|pp|[+][+])|C)\Z(?!\n)/ and -f $E;
  }
  @L
}

sub expand_glob($$ $) {
  #** @param $_[0] string containing globs (? and *)
  #** @param $_[1] listref of names
  #** @param $_[2] dest listref
  my $S=quotemeta($_[0]);
  if (0==($S=~s@\\([?*])@.$1@g)) { push @{$_[2]}, $_[0]; return }
  my $RE=eval"+sub{\$_[0]=~/$S/}";
  die$@if$@;
  for my $E (@{$_[1]}) { push @{$_[2]}, $E if $RE->($E) }
}

sub mustbe_subset_of($$ $$) {
  #** A must be a subset (sublist) of B
  #** @param $_[0] caption A
  #** @param $_[1] ref(array(string)) A
  #** @param $_[2] caption B
  #** @param $_[3] ref(array(string)) B
  my @A=sort @{$_[1]};
  my @B=sort @{$_[3]};
  my($AV,$BV);
  while (defined($AV=pop@A)) {
    1 while defined($BV=pop@B) and $BV gt $AV;
    if (!defined($BV) or $BV ne $AV) {
      print STDERR "$0: $_[0] are: @{$_[1]}\n";
      print STDERR "$0: $_[2] are: @{$_[3]}\n";
      die "$0: $_[0] must be a subset of $_[2]\n";
    }
  }
}

# ---

print "$0: running.\n";

sub unix_shq($) {
  my $S=$_[0];
  $S=~s@'@'\\''@g;
  "'$S'"
}

sub shq($) {
  my $S=$_[0];
  return $S if $S!~/[^\w.-]/;
  if ($^O eq 'MSWin32') {
    # assume the called program is CygWin/Ming32; see later
    # Arguments are delimited by white space, which is either a space or a tab.
    # .       A string surrounded by double quotation marks is interpreted as a
    # single argument, regardless of white space contained within. A quoted
    # string can be embedded in an argument. Note that the caret (^) is not
    # recognized as an escape character or delimiter. 
    # .       A double quotation mark preceded by a backslash, \", is interpreted as
    # a literal double quotation mark (").
    # .       Backslashes are interpreted literally, unless they immediately precede
    # a double quotation mark.
    # .       If an even number of backslashes is followed by a double quotation
    # mark, then one backslash (\) is placed in the argv array for every pair
    # of backslashes (\\), and the double quotation mark (") is interpreted as
    # a string delimiter.
    # .       If an odd number of backslashes is followed by a double quotation
    # mark, then one backslash (\) is placed in the argv array for every pair
    # of backslashes (\\) and the double quotation mark is interpreted as an
    # escape sequence by the remaining backslash, causing a literal double
    # quotation mark (") to be placed in argv.
    $S=~s@"@\\"@g; return qq{"$S"}
  } else {
    $S=~s@'@'\\''@g; return qq{'$S'}
  }
}

sub backtick(@) {
  my $S=$_[0];
  if ($^O eq 'MSWin32') {
    # assume the called program is CygWin/Ming32; and we have proper /bin/sh
    $S="sh -c ".unix_shq($S); # /bin/sh can handle IO-redirections such as `2>&1' right
  } else {
    # assume UNIX
  }
  print "+ $S\n";
  #die unless $ENV{PATH}=~/(.*)/s; # Dat: untaint()
  #$ENV{PATH}=$1;
  #die "$ENV{PATH}";
  # Dat: if `.' is part of $ENV{PATH}, `Insecure directory...' is reported
  #die unless $S=~/(.*)/s; # Dat: untaint()
  readpipe $S # `` qx``
}

my @DS=find_ds();
my @DSQ=map{shq$_}@DS;
my $R="$GCCP -DOBJDEP -M -MG -E 2>&1 @DSQ";
$R=backtick($R);

if ($R!~/: warning: #warning\b/) {
  # config2.h:314:4: warning: #warning REQUIRES: c_lgcc3.o
  # Dat: g++-3.3 ignores #warning with -M -MG -E
  $R.="\n".backtick("$GCCP -DOBJDEP -E 2>&1 >/dev/null @DSQ");
}

## die $R;

#** $pro{"x.ds"} is the list of features provided by "x.ds"; multiplicity
my %pro;
#** $req{"x.ds"} is the list of features required by "x.ds"; multiplicity
my %req;
#** $con{"x.ds"} is the list of features conflicted by "x.ds"; multiplicity
my %con;
#** $repro{"feature"} is a hash=>1 of .ds that provide "feature"
my %repro;
#** $mapro{"executable"} eq "x.ds" if "x.ds" provides main() for "executable"
my %mapro;
#** $idep{"x"} contains the the dependency line for x.ds
my %idep;
#** hash=>1 of "feature"s of NULL-PROVIDES
my %nullpro;
my $included_from;

my $BS; # undef
while ($R=~/\G(.*)\n?/g) {
  my $S=$1;

  if ($S=~/\\\Z/) { if(defined$BS){$BS.="\n$S"}else{$BS=$S} next }
  if (defined $BS) { $S="$BS\n$S"; undef $BS }
  
  if (!length$S) {
  } elsif ($S=~/\AIn file included from ([^:]+)/) {
    $included_from=$1;
  } elsif ($S=~/\A\s{3,}from ([^:]+)/) {
    # ^^^ gcc-3.2
    $included_from=$1;
  } elsif ($S=~/\A([^:]+):\d+:(\d+:)? warning: #warning (NULL-PROVIDES|PROVIDES|CONFLICTS|REQUIRES):(.*)\Z/) {
    # ^^^ (\d+:)? added for gcc-3.1
    # print STDERR "[$S]\n";
    my($DS,$B)=($1,$3);
    if (defined $included_from) { $DS=$included_from; undef $included_from }
    # print STDERR "[$DS] [$B] [$4]\n";
    if ($B eq 'NULL-PROVIDES') {
      for my $FEAT (split' ',$4) { $nullpro{$FEAT}=1 }
    } elsif ($B eq 'PROVIDES') {
       my @L=split' ',$4;
       push @{$pro{$DS}}, @L;
       push @{$con{$DS}}, @L;
       for my $FEAT (@L) {
         $repro{$FEAT}{$DS}=1;
         if ($FEAT=~/\A(.*)_main\Z/) { $mapro{$1}=$DS }
       }
    } elsif ($B eq 'REQUIRES') {
      push @{$req{$DS}}, split' ',$4;
    } elsif ($B eq 'CONFLICTS') {
      push @{$con{$DS}}, split' ',$4;
    } else { die }
    # print "$1;$2;@L;\n";
    undef $included_from;
  } elsif ($S=~/\A([^: ]+)\.o: (([^: ]+?)\.([^ \n]+).*)\Z/s and $1 eq $3) {
    # print STDERR "($S)\n";
    my($A,$B,$C)=("$1.o",$2,"$3.$4");
    die "$0: multi gcc output: $A" if exists $idep{$B};
    # ^^^ Dat: maybe both t.c and t.cxx
    $B=~s@ /[^ ]+@@g; # remove absolute pathnames
    $B=~s@( \\\n)+@ \\\n@g;
    $B=~s@ \\\n\Z@@;
    $B=~s@^ +\\\n@@gsm; # what was left from absolute pathnames
    $B=~s@ *\\\n *\Z(?!\n)@@gsm; # what was left from absolute pathnames
    $idep{$C}=$B;
    # vvv automatic `t.ds: PROVIDES: t.o'
    # print "::: $C $A\n";
    push @{$pro{$C}}, $A;
    push @{$con{$C}}, $A;
    $repro{$A}{$C}=1;
    undef $included_from;
  } elsif ($S=~/\A([^:]+):\d+: .*\Z/) {
    print "# $S\n";
    undef $included_from;
  } elsif ($S=~/\A([^:]+):\d+:(\d+:)? warning: ".*?" redefined\Z/) {
    # ^^^ gcc-3.1
    undef $included_from;
  } elsif ($S=~/\A([^:]+):\d+:(\d+:)? warning: this is the location of /) {
    # ^^^ gcc-3.1
    undef $included_from;
  } elsif ($S=~/: No such file or directory$/) {
    # ^^^ gcc-3.3
    undef $included_from;
  } else {
    die "$0: invalid depret: [$S]\n";
  }
}

mustbe_subset_of "providers"=>[keys(%pro)], "dep_sources"=>[keys(%idep)];
mustbe_subset_of "dep_sources"=>[keys(%idep)], "sources"=>\@DS;

{ my @K=keys %repro;
  while (my($DS,$L)=each%con) {
    my @R=();
    for my $FEAT (@$L) { expand_glob $FEAT, \@K, \@R }
    # ^^^ multiplicity remains
    $con{$DS}=\@R;
  }
}

die unless open MD, "> Makedep";
die unless print MD '
ifndef CCALL
CCALL=$(CC) $(CFLAGS) $(CFLAGSB) $(CPPFLAGS) $(INCLUDES)
endif
ifndef CXXALL
CXXALL=$(CXX) $(CXXFLAGS) $(CXXFLAGSB) $(CPPFLAGS) $(INCLUDES)
endif
ifndef LDALL
LDALL=$(LDY) $(LDFLAGS) $(LIBS)
endif
ifndef CC
CC=gcc
endif
ifndef CXX
CXX=g++
endif
ifndef LD
LD=$(CC) -s
endif
ifndef LDXX
LDXX=$(CXX) -s
endif
ifndef LDY
LDY=$(LD)
endif
ifndef CFLAGS
CFLAGS=-O2 -W -Wall -fsigned-char
endif
ifndef CXXFLAGS
CXXFLAGS=-O2 -W -Wall -fsigned-char
endif
ifndef GLOBFILES
GLOBFILES=Makefile Makedep
endif
';

die unless print MD "ALL +=", join(' ',keys%mapro), "\n"; 
die unless print MD "TARGETS =", join(' ',keys%mapro), "\n"; 

# vvv Thu Oct 31 09:49:02 CET 2002
# (not required)
#my %targets_fal;
#for my $FA (@FAL) { $targets_fal{$FA}="TARGETS_$FA =" }

while (my($EXE,$MDS)=each%mapro) {
  print "exe $EXE\n";
  my @FEA2BA=("${EXE}_main");
  my @DSO=(); # list of .o files required
  my @DSL=(); # list of .ds files required
  my %CON=(); # hash=>1 of features already conflicted
  my %PRO=%nullpro; # hash=>1 of features already provided
  my $FEAT;
  
  while (defined($FEAT=pop@FEA2BA)) {
    next if exists $PRO{$FEAT};
    # print " feat $FEAT (@FEA2BA)\n"; ##
    # vvv Dat: r.b.e == required by executable
    die "$0: feature $FEAT r.b.e $EXE conflicts\n" if exists $CON{$FEAT};
    my @L=keys%{$repro{$FEAT}};
    die "$0: feature $FEAT r.b.e $EXE unprovided\n" if!@L;
    die "$0: feature $FEAT r.b.e $EXE overprovided: @L\n" if$#L>=1;
    # Now: $L[0] is a .ds providing the feature
    push @DSL, $L[0];
    my $O=$L[0]; $O=~s@\.[^.]+\Z@.o@;
    push @DSO, $O;

    $PRO{$FEAT}=1;
    for my $FEAT2 (@{$pro{$L[0]}}) {
      die "$0: extra feature $FEAT2 r.b.e $EXE conflicts\n" if exists $CON{$FEAT2} and not exists $PRO{$FEAT2};
      $PRO{$FEAT2}=1;
    }
    for my $FEAT2 (@{$req{$L[0]}}) { push @FEA2BA, $FEAT2 if!exists $PRO{$FEAT2} }
    for my $FEAT2 (@{$con{$L[0]}}) { $CON{$FEAT2}=1 }
    # die if! exists $PRO{$FEAT}; # assert
  }

  die unless print MD "${EXE}_DS=@DSL\n".
    "$EXE: \$(GLOBFILES) @DSO\n\t\$(LDALL) @DSO -o $EXE\n\t".
    q!@echo "Created executable file: !.$EXE.
    q! (size: `perl -e 'print -s "!.$EXE.q!"'`)."!. "\n";
  # vvv Sat Jun  1 15:40:19 CEST 2002
  for my $FA (@FAL) {
    die unless print MD "$EXE.$FA: \$(GLOBFILES) \$(${EXE}_DS)\n\t".
      "\$(CXD_$FA) \$(CXDFAL) \$(${EXE}_DS) -o $EXE.$FA\n";
    # $targets_fal{$FA}.=" $EXE.$FA";
  }
}
print MD "\n";

# vvv Thu Oct 31 09:49:02 CET 2002
# (not required)
# for my $FA (@FAL) { print MD "$targets_fal{$FA}\n" }
# print MD "\n";

while (my($K,$V)=each%idep) {
  my $O=$K; $O=~s@\.([^.]+)\Z@.o@;
  print MD "$O: \$(GLOBFILES) $V\n\t\$(", ($1 eq'c'?"CC":"CXX"), "ALL) -c $K\n"
}
print MD "\n";

die unless close MD;

print "$0: done.\n";

__END__
