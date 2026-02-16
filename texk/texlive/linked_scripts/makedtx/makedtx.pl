#! /usr/bin/env perl

# File          : makedtx
# Author        : Nicola L. C. Talbot
# Date          : 29 Oct 2004
# Last Modified : $Date: 2026-02-15 01:18:48 +0100 (dim., 15 févr. 2026) $
# Version       : 1.2 $Rev: 106 $

# usage : makedtx [options] -src <expr>=><expr> -doc <filename> <basename>
#
# -h  : help message
# -src <expr>=><expr> : e.g. -src "(foo)src\.(bar)=>$1.$2" will add foosrc.bar to <basename>.dtx to be extracted to foo.bar
# -doc <filename> : file containing documentation.
# -prefinale <string> : text to add to dtx file just before \Finale (added to version 0.91b)
# <basename> : create <basename>.dtx and <basename>.ins

use strict;
use warnings;
use Getopt::Long;

my $version = "1.3";

my $srcdir          = ".";
my $patternop       = "=";
my $verbose         = 0;
my $ins             = 1;
my $testmode        = 0;
my $askforoverwrite = 0;
my $preamble        = "";
my $postamble       = "";
my $author;
my @comment;
my @source;
my @setambles;
my @macrocode;
my $docsrc;
my $drivendoc = 1;

if ($^O =~ m/MSWin/)
{
    $author          = $ENV{"USERNAME"};
    if ($author eq ""){ $author = 'Unknown';}
}
else
{
    $author          = (getpwuid($<))[6] || 'Unknown';
}
my $stopeventually  = "";
my $prefinale       = "";
my $codetitle       = "The Code";
my $section         = "section";
my $license         = "lppl";

my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);

$year = $year + 1900;
my $copyright_date = $year;
my $creation_date = "$year/" . ($mon+1) . "/$mday $hour:" . ($min<10?"0$min" : $min);

my @cmdline_args = capture_cmdline(@ARGV);

# process command line options
# v1.1 added section switch
&GetOptions("h|help" => \&help,
    "v" => \$verbose,
    "src=s@" => \@source,
    "doc=s" => \$docsrc,
    "dir=s" => \$srcdir,
    "op=s" => \$patternop,
    "askforoverwrite!" => \$askforoverwrite,
    "ins!" => \$ins,
    "preamble=s" => \$preamble,
    "postamble=s" => \$postamble,
    "setambles=s@" => \@setambles,
    "testmode" => \$testmode,
    "macrocode=s@" => \@macrocode,
    "author=s" => \$author,
    "date=s" => \$copyright_date,
    "stopeventually=s" => \$stopeventually,
    "prefinale=s" => \$prefinale,
    "codetitle=s" => \$codetitle,
    "comment=s@" => \@comment,
    "drivendoc!" => \$drivendoc,
    "version" => \&print_version,
    "license=s" => \$license,
    "section=s" => \$section
    ) or &syntaxerror();

if($testmode)
{
    $version = "x.y";
    $copyright_date = "YYYY";
    $creation_date = "YYYY/MM/DD hh:mm";
}

if ($#ARGV != 0)
{
   print "No basename specified\n";
   &syntaxerror();
}

my $basename = $ARGV[0];

if ($docsrc eq "")
{
   print "No document source specified (missing -doc)\n";
   &syntaxerror();
}

if ($#source == -1)
{
   print "No source code specified (missing -src)\n";
   &syntaxerror();
}

open DTX, ">:unix","$basename.dtx" or die "Can't open '$basename.dtx'\n";

if ($verbose)
{
   print "Documentation source : " . $docsrc . "\n";
}

# work out the derived files

my @srcdirfile = glob("$srcdir/*");

my @derivedfiles = ();

my @outputfiles = ();

my $numoutput = 0;

foreach my $source (@source)
{
   my ($infile, $outfile, $remainder) = split /=>/, $source;

   if ($outfile eq "")
   {
      print "-src $source argument invalid (no output file specified)\n";

      &syntaxerror();
   }

   if (defined $remainder)
   {
      print "-src $source argument invalid (too many => specified)\n";

      &syntaxerror();
   }

   foreach my $srcdirfile (@srcdirfile)
   {
      my $fileexp = $srcdir . "/" . $infile;

      $_ = $srcdirfile;

      my $expr = "s$patternop$fileexp$patternop$outfile$patternop";

      if (eval($expr))
      {
         my $thisoutfile = $_;

         my $thisinfile  = $srcdirfile;

         $derivedfiles[$numoutput]{'in'} = $thisinfile;
         $derivedfiles[$numoutput]{'out'} = $thisoutfile;
         $outputfiles[$numoutput] = $thisoutfile;

         $numoutput++;
      }
   }
}

if ($preamble eq "")
{
   if ($license eq "lppl")
   {
# v1.2 fixed typo in preamble
      $preamble = <<_END_LICENSE;

 $basename.dtx
 Copyright $copyright_date $author

 This work may be distributed and/or modified under the
 conditions of the LaTeX Project Public License, either version 1.3
 of this license or (at your option) any later version.
 The latest version of this license is in
   http://www.latex-project.org/lppl.txt
 and version 1.3 or later is part of all distributions of LaTeX
 version 2005/12/01 or later.

 This work has the LPPL maintenance status `maintained'.

 The Current Maintainer of this work is $author.

_END_LICENSE
   }
   elsif ($license eq 'bsd')
   {
      $preamble = <<_END_LICENSE;

 $basename.dtx
 Copyright (c) $copyright_date $author
 All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

_END_LICENSE
   }
   elsif ($license eq 'gpl')
   {
      $preamble = <<_END_LICENSE;

 $basename.dtx
 Copyright (c) $copyright_date $author

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

_END_LICENSE
   }
   else
   {
      die "Unknown license '$license'\n".
          "Known licenses: lppl, bsd, gpl";
   }

   $preamble .= " This work consists of the files "
             . "$basename.dtx and $basename.ins and the derived "
             . ($numoutput > 1 ? "files" : "file")
             . " " . join(', ', @outputfiles) . ".\n";
}

open DOC, '<:crlf',$docsrc or die "Can't open '$docsrc'\n";

print DTX "\%\\iffalse\n";
print DTX "\% $basename.dtx generated using makedtx version $version (c) Nicola Talbot\n";
print DTX "\% Command line args:\n";

if($testmode){
   @cmdline_args = sort @cmdline_args;
}
foreach my $arg (@cmdline_args){
   print DTX "\%   ", $arg , "\n";
}

print DTX <<__EOF_HEADER
\% Created on $creation_date
\%\\fi
\%\\iffalse
\%<*package>
\%\% \\CharacterTable
\%\%  {Upper-case    \\A\\B\\C\\D\\E\\F\\G\\H\\I\\J\\K\\L\\M\\N\\O\\P\\Q\\R\\S\\T\\U\\V\\W\\X\\Y\\Z
\%\%   Lower-case    \\a\\b\\c\\d\\e\\f\\g\\h\\i\\j\\k\\l\\m\\n\\o\\p\\q\\r\\s\\t\\u\\v\\w\\x\\y\\z
\%\%   Digits        \\0\\1\\2\\3\\4\\5\\6\\7\\8\\9
\%\%   Exclamation   \\!     Double quote  \\\"     Hash (number) \\#
\%\%   Dollar        \\\$     Percent       \\\%     Ampersand     \\&
\%\%   Acute accent  \\\'     Left paren    \\(     Right paren   \\)
\%\%   Asterisk      \\*     Plus          \\+     Comma         \\,
\%\%   Minus         \\-     Point         \\.     Solidus       \\/
\%\%   Colon         \\:     Semicolon     \\;     Less than     \\<
\%\%   Equals        \\=     Greater than  \\>     Question mark \\?
\%\%   Commercial at \\\@     Left bracket  \\[     Backslash     \\\\
\%\%   Right bracket \\]     Circumflex    \\^     Underscore    \\_
\%\%   Grave accent  \\\`     Left brace    \\{     Vertical bar  \\|
\%\%   Right brace   \\}     Tilde         \\~}
\%</package>
\%\\fi
\% \\iffalse
\% Doc-Source file to use with LaTeX2e
\% Copyright (C) $copyright_date $author, all rights reserved.
\% \\fi
\% \\iffalse
\%<*driver>
__EOF_HEADER
;

my $stopfound=0;
my $indoc=0;

while (<DOC>)
{
   s/\\usepackage\{creatdtx}//;

   my $restofline = $_;

   my $beginline = "";
   my $line = $restofline;
   my $group;

   while ($restofline =~ /(.*)\\ifmakedtx(.*)/)
   {
      $beginline = $1;

      my $done;

      ($group,$restofline,$done) = &getnextgroup($2);

      my $startline = $.;

      while (!$done)
      {
         if (my $nextline = <DOC>)
         {
            $line = $line . $nextline;

            $restofline = $restofline . $nextline;

            ($group,$restofline,$done) = &getnextgroup($restofline);
         }
         else
         {
            die "EOF found whilst scanning first argument to \\ifmakedtx on line $startline\n";
         }
      }

      # print first arg, ignore second

      $beginline = $beginline . $group;

      ($group,$restofline,$done) = &getnextgroup($restofline);

      while (!$done)
      {
         if (my $nextline = <DOC>)
         {
            $line = $line . $nextline;

            $restofline = $restofline . $nextline;

            ($group,$restofline,$done) = &getnextgroup($restofline);
         }
         else
         {
            die "EOF found whilst scanning second argument to \\ifmakedtx on line $startline\n";
         }
      }

      $line = $restofline;
   }

   $line = $beginline . $restofline;

   if (!$drivendoc && $indoc && $line=~/\\end *\{document}/) {
       $indoc = 0;
       last;
   }

   print DTX $line;

   if ($line=~/\\begin\{document}/)
   {
      $indoc = 1;

      if ($drivendoc) { last; }
   }
}

print DTX "\\DocInput{$basename.dtx}\n";
print DTX "\\end{document}\n";
print DTX "\%</driver>\n";
print DTX "\%\\fi\n";

my $inverb=0;

if ($drivendoc) {
    print DTX "\%";

    while (<DOC>)
    {
if (/\\begin\{verbatim}/)
{
    $inverb=1;
}

if (/\\end\{verbatim}/)
{
    $inverb=0;
}

if (/\\StopEventually/ && ($inverb==0))
{
    $stopfound=1;
}

my $restofline = $_;

my $beginline = "";
my $line = $restofline;
my $group;

while ($restofline =~ /(.*)\\ifmakedtx(.*)/)
{
    $beginline = $1;

    my $done;

    ($group,$restofline,$done) = &getnextgroup($2);

    my $startline = $.;

    while (!$done)
    {
if (my $nextline = <DOC>)
{
    $line = $line . $nextline;

    $restofline = $restofline . $nextline;

    ($group,$restofline,$done) = &getnextgroup($restofline);
}
else
{
    die "EOF found whilst scanning first argument to \\ifmakedtx on line $startline\n";
}
    }

    # print first arg, ignore second

    $beginline = $beginline . $group;

    ($group,$restofline,$done) = &getnextgroup($restofline);

    while (!$done)
    {
if (my $nextline = <DOC>)
{
    $line = $line . $nextline;

    $restofline = $restofline . $nextline;

    ($group,$restofline,$done) = &getnextgroup($restofline);
}
else
{
    die "EOF found whilst scanning second argument to \\ifmakedtx on line $startline\n";
}
    }

    $line = $restofline;
}

$line = $beginline . $restofline;

if (($line=~/\\end\{document}/) and not $inverb)
{
    $indoc=0;

    $line=~s/\\end\{document}//;
}

$line=~s/\n/\n\%/mg;

print DTX "$line";
    }
    print DTX "\n";
}
close DOC;

if ($stopfound==0)
{
   print DTX "\%\\StopEventually{$stopeventually}\n";
}

# v1.0 added check
if ($codetitle)
{
  # v1.1 added section
  print DTX "\%\\$section\{$codetitle\}\n";
}

for (my $idx = 0; $idx <= $#derivedfiles; $idx++)
{
   my $thisinfile = $derivedfiles[$idx]{'in'};
   my $thisoutfile = $derivedfiles[$idx]{'out'};

   if ($verbose)
   {
         print "$thisinfile -> $thisoutfile\n";
   }

   open SRC, "<:crlf", $thisinfile or die "Can't open $thisinfile\n";

   print DTX "\%\\iffalse\n";
   print DTX "\%    \\begin{macrocode}\n";
   print DTX "\%<*$thisoutfile>\n";
   print DTX "\%    \\end{macrocode}\n";
   print DTX "\%\\fi\n";

   my $macrocode = 0;
   my $comment   = 0;

   foreach my $expr (@comment)
   {
      if ($thisoutfile =~ m/$expr/)
      {
         print DTX "\%\\iffalse\n";

 $comment = 1;

 last;
      }
   }

   foreach my $expr (@macrocode)
   {
      if ($thisoutfile =~ m/$expr/)
      {
         print DTX "\%    \\begin{macrocode}\n";

         $macrocode = 1;

 last;
      }
   }

   while (<SRC>)
   {
      print DTX "$_";
   }

   if ($macrocode == 1)
   {
      print DTX "\%    \\end{macrocode}\n";
   }

   if ($comment == 1)
   {
      print DTX "\%\\fi\n";
   }

   print DTX "\%\\iffalse\n";
   print DTX "\%    \\begin{macrocode}\n";
   print DTX "\%</$thisoutfile>\n";
   print DTX "\%    \\end{macrocode}\n";
   print DTX "\%\\fi\n";

   close SRC;
}

print DTX "\%$prefinale\n" if ($prefinale);
print DTX "\%\\Finale\n";
print DTX "\\endinput\n";

close DTX;

if ($ins)
{
   open INS, ">:unix","$basename.ins" or die "Can't open '$basename.ins'\n";

   print INS "\% $basename.ins generated using makedtx version $version $creation_date\n";

   print INS "\\input docstrip\n\n";
   print INS "\\preamble\n";
   print INS "$preamble\n";
   print INS "\\endpreamble\n\n";

   if ($postamble ne "")
   {
      print INS "\\postamble\n";
      print INS "$postamble\n";
      print INS "\\endpostamble\n\n";
   }

   if ($askforoverwrite)
   {
      print INS "\\askforoverwritetrue\n\n";
   }
   else
   {
      print INS "\\askforoverwritefalse\n\n";
   }

   print INS "\\generate{";

   for (my $idx = 0; $idx <= $#derivedfiles; $idx++)
   {
      my $thisinfile = $derivedfiles[$idx]{'in'};
      my $thisoutfile = $derivedfiles[$idx]{'out'};

      print INS "\\file{$thisoutfile}{";

      my $ambleset = 0;
      my $noamble  = 0;

      foreach my $setamble (@setambles)
      {
         my ($fileexp, $amble, $remainder) = split /=>/, $setamble;

         if (defined $remainder)
         {
            die "-setambles $setamble argument invalid (too many => specified)\n";
         }

         if ($thisoutfile =~ m/$fileexp/)
         {
            if ($verbose)
            {
               print "$fileexp matches $thisoutfile -> setting \"$amble\"\n";
            }

            print INS $amble;

            $ambleset = 1;

            if ($amble =~ m/\\nopreamble/)
            {
               $noamble = 1;
            }
         }
      }

      if (!$ambleset)
      {
         print INS "\\usepreamble\\defaultpreamble\n\\usepostamble\\defaultpostamble";
      }

      print INS "\\from{$basename.dtx}{$thisoutfile";

      if ($noamble == 0)
      {
         # this will add the character table to all files except those that use \nopreamble

         print INS ",package";
      }

      print INS "}}\n";
   }

   print INS "}\n\n";

   print INS "\\endbatchfile\n";

   close INS;
}

sub syntaxerror
{
   die "Syntax : makedtx [options] <basename>\nUse -h for help\n";
}

sub help
{
    print <<__EOF_HELP
makedtx Help

Current Version : $version

usage : makedtx [options] -src "<expr>=><expr>" -doc <filename> <basename>

makedtx can be used to construct a LaTeX2e dtx and ins file from
the specified source code.  The final command line argument
<basename> should be used to specify the basename of the dtx
and ins files.

-src "<expr1>=><expr2>"
The command line switch -src identifies the original source code and the name
of the file to which it will utimately be extracted on latexing the ins file
<expr1> can be a Perl expression, such as (foo)src.(sty), and <expr2> can
a Perl substitution style expression, such as \$1.\$2
Note that double quotes must be used to prevent shell expansion
Multiple invocations of -src are permitted
See examples below.

-doc <filename>
The name of the documentation source code.  This should be a LaTeX2e document

Optional Arguments:

-dir <directory>   : search for source files in <directory>
-op <character>    : set the pattern matching operator (default '$patternop')
-askforoverwrite   : set askforoverwrite switch in INS file to true
-noaskforoverwrite : set askforoverwrite switch in INS file to false (default)
-preamble <text>   : set the preamble.  Standard one inserted if omitted
-postamble <text>  : set the postamble.
-setambles "<pattern>=><text>" : set pre- and postambles to <text> if file matches
                     pattern
-author <text>     : name of author (inserted into standard preamble. User name
                     inserted if omitted)
-date <text>       : copyright date
-ins               : create the ins file (default)
-noins             : don't create the ins file
-prefinale <text>  : add <text> immediately prior to \\Finale
-macrocode <expr>  : surround any file which matches <expr> in a macrocode
                     environment
-comment <expr>    : surround any file which matches <expr> with \\iffalse \\fi pair
-codetitle <text>  : The title for the documented code section (default: The Code)
-section <cs>      : The sectioning command to use for the documented code section
                     (default: 'section')
-license <license> : use the given license for the preamble.
                     Known licenses: lppl (default), bsd, gpl.
-h                 : help message
-nodrivendoc       : documentation body is placed in driver block as is rather than
                     being make docstrippable (default: not set).
-v                 : verbose

Examples:

Example 1:
Documenation is in foodoc.tex
Source code is in foosrc.sty.  The final extracted version should be
called foo.sty. The dtx file should be called foo.dtx and the ins file
 should be called foo.ins

makedtx -src "foosrc\\.sty=>foo.sty" -doc foodoc.tex foo

Example 2:
Documenation is in bardoc.tex
Source code is in barsrc.sty.  The final extracted version should be
called bar.sty.  Source code is also in barsrc.bst.  The final extracted
version should be called bar.bst.  The dtx file should be called bar.dtx and
the ins file should be called bar.ins

makedtx -src "barsrc\\.sty=>bar.sty" -src "barsrc\\.bst=>bar.bst" -doc bardoc.tex bar

Or

makedtx -src "barsrc\\.(bst|sty)=>bar.\$1" -doc bardoc.tex bar

__EOF_HELP
 ;
   die;
}

sub print_version
{
    print "makedtx version $version\n";
    exit 0;
}

sub eatinitialspaces
{
   my ($STR) = @_;

   while (substr($STR,0,1) =~ m/\s/)
   {
      $STR = substr($STR,1);
   }

   return $STR;
}

sub getnextgroup
{
   my($curline) = @_;

   $curline = &eatinitialspaces($curline);

   # check to see if current string is blank

   if ($curline!~/[^\s]+/m)
   {
      return ("","",0);
   }

   if ((my $group = substr($curline,0,1)) ne "{")
   {
       # next group hasn't been delimited with braces
       # return first non-whitespace character

       $curline = substr($curline,1);

       # unless it's a backslash, in which case get command name

       if ($group eq "\\")
       {
          if ($curline=~/([a-zA-Z]+)(^[a-zA-Z].*)/m)
          {
             $group = $1;

             $curline = $2;
          }
          else
          {
             # command is made up of backslash followed by symbol

             $curline=~/([\W_0-9\s\\])(.*)/m;

             $group = $1;

             $curline = $2;
          }
       }

       return ($group,$curline,1);
   }

   my $pos=index($curline, "{");
   my $startpos=$pos;
   my $posopen=0;
   my $posclose=0;

   my $bracelevel = 1;

   my $done=0;

   while (!$done)
   {
      $pos++;

      $posopen = index($curline, "{", $pos);

      # check to make sure it's not a \{

      while ((substr($curline, $posopen-1,1) eq "\\") and ($posopen > 0))
      {
         # count how many backlashes come before it.

         my $i = $posopen-1;

         my $numbs = 1;

         while ((substr($curline, $i-1,1) eq "\\") and ($i > 0))
         {
            $numbs++;
            $i--;
         }

         # is $numbs is odd, we have a \{, otherwise we have \\{

         if ($numbs%2 == 0)
         {
            last;
         }
         else
         {
            $posopen = index($curline, "{", $posopen+1);
         }
      }

      $posclose= index($curline, "}", $pos);

      # check to make sure it's not a \}

      while ((substr($curline, $posclose-1,1) eq "\\") and ($posclose > 0))
      {
         # count how many backlashes come before it.

         my $i = $posclose-1;

         my $numbs = 1;

         while ((substr($curline, $i-1,1) eq "\\") and ($i > 0))
         {
            $numbs++;
            $i--;
         }

         # is $numbs is odd, we have a \}, otherwise we have \\}

         if ($numbs%2 == 0)
         {
            last;
         }
         else
         {
            $posclose = index($curline, "}", $posclose+1);
         }
      }

      if (($posopen==-1) and ($posclose==-1))
      {
         $done=1;
      }
      elsif ($posopen==-1)
      {
         $pos=$posclose;

         $bracelevel--;

         if ($bracelevel==0)
         {
            my $group = substr($curline, $startpos+1, $pos-$startpos-1);

            $curline = substr($curline, $pos+1);

            return ($group,$curline,1);
         }
      }
      elsif ($posclose==-1)
      {
         $pos=$posopen;

         $bracelevel++;
      }
      elsif ($posopen<$posclose)
      {
         $pos=$posopen;

         $bracelevel++;
      }
      elsif ($posclose<$posopen)
      {
         $pos=$posclose;

         $bracelevel--;

         if ($bracelevel==0)
         {
            my $group = substr($curline, $startpos+1, $pos-$startpos-1);

            $curline = substr($curline, $pos+1);

            return ($group,$curline,1);
         }
      }
   }

   # closing brace must be on another line

   return ("", $curline, 0);
}

sub make_argset {
    my @args = @_;
    my @escaped_args = ();

    while ($#args >= 0) {
my $arg = shift @args;
# Quoting is needed when the string $arg contains a special character
if ($arg =~ m/[`"\\\$ '*]/) {
    # Precede characters ` " \ and $ by a backslash to quote them
    $arg =~ s/([`"\\\$])/\\$1/g;
    # backslash preceded characters w/o a special meaning (ie ` " \ and $) are left unmodified
    $arg =~ s/\\([^`"\\\$])/$1/g;
    $arg = "\"$arg\"";
}
push @escaped_args, $arg;
    }
    return join " ", @escaped_args;
}

sub capture_cmdline
{
    my @args = @_;
    my @cmdline_args = ();
    # number of arguments following option switch, 2 means 1 or more
    my %options = (
h => 0,
help => 0,
v => 0,
src => 2,
doc => 1,
dir => 1,
op => 1,
askforoverwrite => 0,
noaskforoverwrite => 0,
ins => 0,
noins => 0,
preamble => 1,
postamble => 1,
setambles => 2,
testmode => 0,
macrocode => 2,
author => 1,
date => 1,
stopeventually => 1,
prefinale => 1,
codetitle => 1,
comment => 2,
drivendoc => 0,
nodrivendoc => 0,
version => 0,
license => 1,
section => 1
);
    while ($#args >= 0) {
my $arg = shift @args;
my $switch = $arg =~ s/\A-{1,2}([a-z])/$1/r;
unless (exists $options{$switch}) {
    push @args, $arg;
    $arg = '--';
}
if ($arg eq '--') {
    while ($#args >= 0) {
my @argset = (shift @args);
push @cmdline_args, make_argset(@argset);
    }
    last;
}
my $arg_count = $options{$switch};
if ($arg_count == 0) {
    push @cmdline_args, $arg;
} elsif ($arg_count == 1) {
    $#args >= 0 or die "Missing argument after $arg";
    my @argset = ($arg, shift @args);
    push @cmdline_args, make_argset(@argset);
} elsif  ($arg_count == 2) {
    $#args >= 0 or die "Missing argument after $arg";
    my @argset = ($arg, shift @args);
    while ($#args >= 0 and do {
   $arg = $args[0];
   $switch = $arg =~ s/\A-{1,2}([a-z])/$1/r;
   !exists($options{$switch});
   }) {
push @argset, (shift @args);
    }
    push @cmdline_args, make_argset(@argset);
} else {
    die "INTERNAL";
}
    }
    return @cmdline_args;
}
