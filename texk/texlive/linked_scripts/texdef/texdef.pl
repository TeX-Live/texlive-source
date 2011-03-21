#!/usr/bin/env perl
################################################################################
#  texdef -- Show definitions of TeX commands
#  Copyright (c) 2011 Martin Scharrer <martin@scharrer-online.de>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
################################################################################
use strict;
use warnings;
use File::Temp qw/tempdir/; 
use File::Basename;
use Cwd;

my ($scriptname) = fileparse($0, qw(\.pl \.perl));

my $TEX = 'pdflatex';
if ($scriptname =~ /^(.*)def$/) {
    $TEX = $1;
}

my $CLASS      = "article";
my @PACKAGES   = ();
my @OTHERDEFS  = ();
my $INPREAMBLE = 0;
my $SHOWVALUE  = 0;
my $BEFORECLASS = 0;
my @ENVCODE = ();

use Getopt::Long;
my $data = "file.dat";
my $length = 24;
my $verbose;
  
my $ISLATEX = 0;
my $ISTEX   = 0;
my $ISCONTEXT = 0;

my $BEGINENVSTR = '%s';
my $ENDENVSTR   = '%s';

sub usage {
print << 'EOT';
texdef -- Show definitions of TeX commands
Version 1.1 -- 2011/03/16
Copyright (C) 2011  Martin Scharrer <martin@scharrer-online.de>
This program comes with ABSOLUTELY NO WARRANTY;
This is free software, and you are welcome to redistribute it under certain conditions;

Usage:
  texdef   [options] commandname [commandname ...]
  latexdef [options] commandname [commandname ...]

Other program names are possible. See the 'tex' option.  Command names do not need to start with `\`.

Options:
  --tex <flv>, -t <flv> : Use given flavour of TeX: 'tex', 'latex', 'context'.
                          Variations of 'tex' and 'latex', like 'luatex', 'lualatex', 'xetex', 'xelatex' are supported.
                          The default is given by the used program name: 'texdef' -> 'tex', 'latexdef' -> 'latex', etc.
  --value, -v           : Show value of command instead (i.e. \the\command).
  --preamble, -P        : Show definition of the command inside the preamble.
  --beforeclass, -B     : Show definition of the command before \documentclass.
  --package <pkg>, -p <pkg>     : (M) Load given tex-file, package or module depending on whether '*tex', '*latex'
                                  or 'context' is used. For LaTeX the <pkg> can start with `[<options>]` and end 
                                  with `<pkgname>` or `{<pkgname>}`.
  --class <class>, -c <class>   : (LaTeX only) Load given class instead of default ('article').
                                  The <class> can start with `[<classs options>]` and end 
                                  with `<classname>` or `{<classname>}`.
  --environment <env>, -p <env> : (M) Show definition inside the given environment <env>.
  --othercode <code>, -o <code> : (M) Add other code into the preamble before the definition is shown.
                                  This can be used to e.g. load PGF/TikZ libraries.
  --before <code>, -b <code>    : (M) Place <code> before definition is shown.
                                  The <code> can be arbitray TeX code and doesn't need be be balanced.
  --after  <code>, -a <code>    : (M) Place <code> after definition is shown.
                                  The <code> can be arbitray TeX code and doesn't need be be balanced.
  --help, -h                    : Print this help and quit.

 Long option can be shorten as long the are still unique.  Short options can be combined.
 If the option 'environment', 'before' and 'after' are used toegether the
 produced code will be inserted in the given order (reversed order for 'after').
 (M) = This option can be given multiple times.

Examples:
Show the definition of '\chapter' with different classes ('article' (default), 'book' and 'scrbook'):

    latexdef chapter
    latexdef -c book chapter
    latexdef -c scrbook chapter

Show value of `\textwidth` with different class options:

    latexdef -c [a4paper]{book} -v paperwidth
    latexdef -c [letter]{book}  -v paperwidth

Show definition of TikZ's '\draw' outside and inside a 'tikzpicture' environment:

    latexdef -p tikz draw
    latexdef -p tikz --env tikzpicture draw

Show definition of TikZ's '\draw' inside a node, inside a 'beamer' frame in 'handout' mode:

    latexdef -c [handout]beamer -p tikz --env frame --env tikzpicture -b '\node {' -a '};' draw

EOT
  exit (1);
}

sub envcode {
  my $opt = shift;
  my $arg = shift;
  push @ENVCODE, [ $opt, $arg ];
}


Getopt::Long::Configure ("bundling");
GetOptions (
   'value|v!' => \$SHOWVALUE,
   'preamble|P!' => \$INPREAMBLE,
   'beforeclass|B!' => \$BEFORECLASS,
   'class|c=s' => \$CLASS,
   'package|p=s' => \@PACKAGES,
   'otherdefs|o=s' => \@OTHERDEFS,
   'environment|e=s' => \&envcode,
   'before|b=s' => \&envcode,
   'after|a=s' => \&envcode,
   'tex|t=s' => \$TEX,
   'help|h' => \&usage,
) || usage();

if ($TEX =~ /latex$/) {
  $ISLATEX = 1;
  $BEGINENVSTR = '\begin{%s}' . "\n";
  $ENDENVSTR   = '\end{%s}'   . "\n";
}
elsif ($TEX =~ /tex$/) {
  $ISTEX   = 1;
  $BEGINENVSTR = '\%s' . "\n";
  $ENDENVSTR   = '\end%s' . "\n";
}
elsif ($TEX =~ /context$/) {
  $ISCONTEXT = 1;
  $BEGINENVSTR = '\start%s' . "\n";
  $ENDENVSTR   = '\stop%s'  . "\n";
}

$CLASS =~ /^(?:\[(.*)\])?{?(.*?)}?$/;
$CLASS = $2;
my $CLASSOPTIONS = $1 || '';


my $cwd = getcwd();
$ENV{TEXINPUTS} = $cwd . ':' . ($ENV{TEXINPUTS} || '');

my $TMPDIR  = tempdir( 'texdef_XXXXXX', CLEANUP => 1, TMPDIR => 1 );
chdir $TMPDIR or die;
my $TMPFILE = 'texdef.tex';

my @cmds = @ARGV;

sub testdef {
    my $cmd = shift;
    my $def = shift;
    if ($def eq 'macro:->\@latex@error {Can be used only in preamble}\@eha ' && $cmd ne '\@notprerr') {
        unshift @cmds, '^' . $cmd;
    }
    elsif ($def =~ /^(?:\\[a-z]+ )?macro:.*?>(.*)/) {
        my $macrodef = $1;
        if ($macrodef =~ /^\\protect (.*?) ?$/) {
            my $protectedmacro = $1;
            unshift @cmds, $protectedmacro;
        }
        elsif ($macrodef =~ /^\\x\@protect (.*?) ?\\protect (.*?) ?$/) {
            my $protectedmacro = $2;
            unshift @cmds, $protectedmacro;
        }
        elsif ($macrodef =~ /^\\\@protected\@testopt {?\\.*? }? *(\\\\.*?) /) {
            unshift @cmds, $1;
        }
        elsif ($macrodef =~ /^\\\@testopt {?(\\.*?) }?/) {
            unshift @cmds, $1;
        }
    }
    elsif ($def =~ /^\\(char|mathchar)|(dimen|skip|muskip|count)\d/) {
        unshift @cmds, '#' . $cmd;
    }
}

my $bschar = 0;
my $pcchar = 0;
my $lbchar = 0;
my $rbchar = 0;

sub special_chars {
    return if (!$bschar && !$pcchar && !$lbchar && !$rbchar);
    print '\begingroup'."\n";
    if ($bschar) {
        print '\lccode`.=92 \lowercase{\expandafter\gdef\csname @backslashchar\endcsname{.}}'."\n";
    }
    if ($pcchar) {
        print '\lccode`.=37 \lowercase{\expandafter\gdef\csname @percentchar\endcsname{.}}'."\n";
    }
    if ($lbchar) {
        print '\lccode`.=123 \lowercase{\expandafter\gdef\csname @charlb\endcsname{.}}'."\n";
    }
    if ($rbchar) {
        print '\lccode`.=125 \lowercase{\expandafter\gdef\csname @charrb\endcsname{.}}'."\n";
    }
    print '\endgroup'."\n";
}


while (my $cmd = shift @cmds) {

next if $cmd eq '';
my $origcmd = $cmd; 
my $showvalue;
my $inpreamble;
if (length ($cmd) > 1) {
    $cmd =~ s/^([#^])?\\?//;
    my $type = $1 || '';
    $showvalue  = $type eq '#';
    $inpreamble = $type eq '^';
}
$bschar = $cmd =~ s/\\/\\csname\0\@backslashchar\\endcsname\0/g;
$pcchar = $cmd =~ s/\%/\\csname\0\@percentchar\\endcsname\0/g;
$lbchar = $cmd =~ s/\{/\\csname\0\@charlb\\endcsname\0/g;
$rbchar = $cmd =~ s/\}/\\csname\0\@charrb\\endcsname\0/g;
$cmd =~ s/\s/\\space /g;
$cmd =~ s/\0/ /g;


open (my $tmpfile, '>', $TMPFILE);
select $tmpfile;

print "\\nonstopmode\n";

if ($ISLATEX) {
    print "\\nofiles\n";
    if (!$BEFORECLASS) {
        print "\\documentclass[$CLASSOPTIONS]{$CLASS}\n";

        foreach my $pkg (@PACKAGES) {
            $pkg =~ /^(?:\[(.*)\])?{?(.*?)}?$/;
            my ($pkgname,$pkgoptions) = ($2, $1 || '');
            print "\\usepackage[$pkgoptions]{$pkgname}\n";
        }
        {
            local $, = "\n";
            print @OTHERDEFS, '';
        }
        print "\\begin{document}\n" unless $inpreamble || $INPREAMBLE;
    }
}
elsif ($ISCONTEXT) {
    foreach my $pkgname (@PACKAGES) {
        print "\\usemodule[$pkgname]\n";
    }
    {
        local $, = "\n";
        print @OTHERDEFS, '';
    }
    &special_chars();
    print "\\starttext\n" unless $inpreamble || $INPREAMBLE;
}
elsif ($ISTEX) {
    foreach my $pkgname (@PACKAGES) {
        print "\\input $pkgname.tex \n";
    }
    {
        local $, = "\n";
        print @OTHERDEFS, '';
    }
    &special_chars();
}

foreach my $envc (@ENVCODE) {
    my ($envtype,$env) = @$envc;
    if ($envtype eq 'environment') {
        printf $BEGINENVSTR, $env;
    }
    elsif ($envtype eq 'before') {
        print "$env\n";
    }
}

print '\immediate\write0{==============================================================================}%'."\n";
if (length ($cmd) > 1) {
if ($showvalue || $SHOWVALUE) {
    print '\immediate\write0{\string\the\expandafter\string\csname ', $cmd, '\endcsname}%'."\n";
    print '\immediate\write0{------------------------------------------------------------------------------}%'."\n";
    print '\immediate\write0{\expandafter\the\csname ', $cmd, '\endcsname}%'."\n";
} else {
    print '\begingroup';
    print '\immediate\write0{\expandafter\string\csname ', $cmd, '\endcsname}%'."\n";
    print '\immediate\write0{------------------------------------------------------------------------------}%'."\n";
    print '\expandafter\endgroup\expandafter\immediate\expandafter\write\expandafter0\expandafter{\expandafter\meaning\csname ', $cmd, '\endcsname}%'."\n";
}
}
else {
if ($showvalue || $SHOWVALUE) {
    print '\immediate\write0{\string\the\string\\', $cmd, '}%'."\n";
    print '\immediate\write0{------------------------------------------------------------------------------}%'."\n";
    print '\immediate\write0{\the\\', $cmd, '}%'."\n";
} else {
    print '\immediate\write0{\string\\', $cmd, '}%'."\n";
    print '\immediate\write0{------------------------------------------------------------------------------}%'."\n";
    print '\immediate\write0{\meaning\\', $cmd, '}%'."\n";
}
}
print '\immediate\write0{==============================================================================}%'."\n";

foreach my $envc (reverse @ENVCODE) {
    my ($envtype,$env) = @$envc;
    if ($envtype eq 'environment') {
        printf $ENDENVSTR, $env;
    }
    elsif ($envtype eq 'after') {
        print "$env\n";
    }
}

if ($ISLATEX) {
    print "\\documentclass[$CLASSOPTIONS]{$CLASS}\n" if $BEFORECLASS;
    print "\\begin{document}\n" if $inpreamble || $INPREAMBLE || $BEFORECLASS;
    print "\\end{document}\n";
}
elsif ($ISCONTEXT) {
    print "\\starttext\n" if $inpreamble || $INPREAMBLE;
    print "\\stoptext\n";
}
elsif ($ISTEX) {
    print "\\bye\n";
}

close ($tmpfile);

select STDOUT;

open (my $texpipe, '-|', "$TEX '$TMPFILE' ");

my $name = '';
my $definition = '';
my $errormsg = '';

while (<$texpipe>) {
  last if /^=+$/;
  if ($_ =~ /^!\s*(.*)/ && !$errormsg) {
    chomp;
    my $line = $1;
    $errormsg = $line;
    while (length $line >= 79) {
        $line = <$texpipe>;
        chomp $line;
        $errormsg .= $line;
    }
  }
}
while (<$texpipe>) {
  last if /^-+$/;
  next if /^$/;
  chomp;
  $name .= $_;
}
while (<$texpipe>) {
  last if /^=+$/;
  next if /^$/;
  chomp;
  $definition .= $_;
  if ($_ =~ /^!\s*(.*)/ && !$errormsg) {
    $errormsg = $1;
  }
}
while (<$texpipe>) {
  if ($_ =~ /^!\s*(.*)/ && !$errormsg) {
    chomp;
    my $line = $1;
    $errormsg = $line;
    while (length $line >= 79) {
        $line = <$texpipe>;
        chomp $line;
        $errormsg .= $line;
    }
  }
}
close ($texpipe);

my $error = $? >> 8;

if ($error) {
  if ( ($SHOWVALUE || $showvalue) && ($errormsg =~ /^You can't use `.*' after \\the\./) ) {
    print STDERR "\n$name:\nError: Given command sequence does not contain a value.\n\n";
  }
  else {
    print STDERR "\n$name:\nCompile error: $errormsg\n\n";
  }
  next;
}

print "\n(in preamble)" if $inpreamble;
print "\n$name:\n$definition\n\n";


testdef($origcmd,$definition);

}

chdir $cwd;
__END__
