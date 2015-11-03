#!/usr/bin/perl

# File          : makedtx
# Author        : Nicola L. C. Talbot
# Date          : 29 Oct 2004
# Last Modified : 19 Aug 2007
# Version       : 0.94b

# usage : makedtx [options] -src <expr>=><expr> -doc <filename> <basename>
#
# -h  : help message
# -src <expr>=><expr> : e.g. -src "(foo)src\.(bar)=>$1.$2" will add foosrc.bar to <basename>.dtx to be extracted to foo.bar
# -doc <filename> : file containing documentation.
# -prefinale <string> : text to add to dtx file just before \Finale (added to version 0.91b)
# <basename> : create <basename>.dtx and <basename>.ins

use Getopt::Long;

$version = "0.94b";

# process command line options

 %optctl = ();

&GetOptions(\%optctl, "h", "help", "v", "src=s@", "doc=s",
"dir=s", "op=s", "askforoverwrite!", "ins!",
"preamble=s", "postamble=s", "setambles=s@", "macrocode=s@",
"author=s", "date=s", "stopeventually=s",
"prefinale=s", "codetitle=s", "comment=s@",
"version", "license=s") or &syntaxerror();

$srcdir          = ".";
$patternop       = "=";
$verbose         = 0;
$noins           = 0;
$askforoverwrite = 0;
$preamble        = "";
$postamble       = "";
$author          = (getpwuid($<))[6] || 'Unknown';
$stopeventually  = "";
$prefinale       = "";
$codetitle       = "The Code";
$license         = "lppl";

($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);

$year = $year + 1900;

foreach $setting (keys %optctl)
{
   if (($setting eq "h") || ($setting eq "help"))
   {
      &help();
   }
   elsif ($setting eq "version")
   {
      die "makedtx version $version\n";
   }
   elsif ($setting eq "doc")
   {
      $docsrc   = $optctl{$setting};
   }
   elsif ($setting eq "src")
   {
      @source = @{ $optctl{$setting} };
   }
   elsif ($setting eq "dir")
   {
      $srcdir = $optctl{$setting};
   }
   elsif ($setting eq "op")
   {
      $patternop = $optctl{$setting};
   }
   elsif ($setting eq "v")
   {
      $verbose = 1;
   }
   elsif ($setting eq "ins")
   {
      $noins = 1-$optctl{$setting};
   }
   elsif ($setting eq "askforoverwrite")
   {
      $askforoverwrite = $optctl{$setting};
   }
   elsif ($setting eq "preamble")
   {
      $preamble = $optctl{$setting};
   }
   elsif ($setting eq "postamble")
   {
      $postamble = $optctl{$setting};
   }
   elsif ($setting eq "setambles")
   {
      @setambles = @{ $optctl{$setting} };
   }
   elsif ($setting eq "macrocode")
   {
      @macrocode = @{ $optctl{$setting} };
   }
   elsif ($setting eq "author")
   {
      $author = $optctl{$setting};
   }
   elsif ($setting eq "date")
   {
      $year = $optctl{$setting};
   }
   elsif ($setting eq "stopeventually")
   {
      $stopeventually = $optctl{$setting};
   }
   elsif ($setting eq "prefinale")
   {
      $prefinale = $optctl{$setting};
   }
   elsif ($setting eq "codetitle")
   {
      $codetitle = $optctl{$setting};
   }
   elsif ($setting eq "comment")
   {
      @comment = @{ $optctl{$setting} };
   }
   elsif ($setting eq "license")
   {
      $license = $optctl{$setting};
   }
}

if ($#ARGV != 0)
{
   print "No basename specified\n";
   &syntaxerror();
}

$basename = $ARGV[0];

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

open DTX, ">$basename.dtx" or die "Can't open '$basename.dtx'\n";

if ($verbose)
{
   print "Documentation source : " . $docsrc . "\n";
}

# work out the derived files

 @srcdirfile = glob("$srcdir/*");

 @derivedfiles = ();

 @outputfiles = ();

$numoutput = 0;

foreach $source (@source)
{
   ($infile, $outfile, $remainder) = split /=>/, $source;

   if ($outfile eq "")
   {
      print "-src $source argument invalid (no output file specified)\n";

      &syntaxerror();
   }

   if (not ($remainder eq ""))
   {
      print "-src $source argument invalid (too many => specified)\n";

      &syntaxerror();
   }

   foreach $srcdirfile (@srcdirfile)
   {
      $fileexp = $srcdir . "/" . $infile;

      $_ = $srcdirfile;

      $expr = "s$patternop$fileexp$patternop$outfile$patternop";

      if (eval($expr))
      {
         $thisoutfile = $_;

         $thisinfile  = $srcdirfile;

         $file{$thisinfile} = $thisoutfile;
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
     $preamble = <<_END_LICENSE

 $basename.dtx
 Copyright $year $author

 This work may be distributed and/or modified under the
 conditions of the LaTeX Project Public License, either version 1.3
 of this license of (at your option) any later version.
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
     $preamble = <<_END_LICENSE

 $basename.dtx
 Copyright (c) $year $author
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
     $preamble = <<_END_LICENSE

 $basename.dtx
 Copyright (c) $year $author

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

open DOC, $docsrc or die "Can't open '$docsrc'\n";

print DTX "\%\\iffalse\n";
print DTX "\% $basename.dtx generated using makedtx version $version (c) Nicola Talbot\n";
print DTX "\% Command line args:\n";

foreach $setting (keys %optctl)
{
   if ($setting eq "src")
   {
      foreach $source (@source)
      {
         print DTX "\%   -src \"$source\"\n";
      }
   }
   elsif ($setting eq "setambles")
   {
      foreach $setamble (@setambles)
      {
         print DTX "\%   -setambles \"$setamble\"\n";
      }
   }
   elsif ($setting eq "macrocode")
   {
      foreach $macrocode (@macrocode)
      {
         print DTX "\%   -macrocode \"$macrocode\"\n";
      }
   }
   elsif ($setting eq "comment")
   {
      foreach $comment (@comment)
      {
         print DTX "\%   -comment \"$comment\"\n";
      }
   }
   else
   {
      $val = $optctl{$setting};
      $val=~s/\\/\\\\/g;
      print DTX "\%   -", $setting, " \"",  $val, "\"\n";
   }
}

print DTX "\%   $basename\n";

print DTX "\% Created on $year/", $mon+1, "/$mday $hour:", $min<10?"0$min" : $min,"\n";
print DTX "\%\\fi\n";
print DTX "\%\\iffalse\n";
print DTX "\%<*package>\n";
print DTX "\%\% \\CharacterTable\n";
print DTX "\%\%  {Upper-case    \\A\\B\\C\\D\\E\\F\\G\\H\\I\\J\\K\\L\\M\\N\\O\\P\\Q\\R\\S\\T\\U\\V\\W\\X\\Y\\Z\n";
print DTX "\%\%   Lower-case    \\a\\b\\c\\d\\e\\f\\g\\h\\i\\j\\k\\l\\m\\n\\o\\p\\q\\r\\s\\t\\u\\v\\w\\x\\y\\z\n";
print DTX "\%\%   Digits        \\0\\1\\2\\3\\4\\5\\6\\7\\8\\9\n";
print DTX "\%\%   Exclamation   \\!     Double quote  \\\"     Hash (number) \\#\n";
print DTX "\%\%   Dollar        \\\$     Percent       \\\%     Ampersand     \\&\n";
print DTX "\%\%   Acute accent  \\\'     Left paren    \\(     Right paren   \\)\n";
print DTX "\%\%   Asterisk      \\*     Plus          \\+     Comma         \\,\n";
print DTX "\%\%   Minus         \\-     Point         \\.     Solidus       \\/\n";
print DTX "\%\%   Colon         \\:     Semicolon     \\;     Less than     \\<\n";
print DTX "\%\%   Equals        \\=     Greater than  \\>     Question mark \\?\n";
print DTX "\%\%   Commercial at \\\@     Left bracket  \\[     Backslash     \\\\\n";
print DTX "\%\%   Right bracket \\]     Circumflex    \\^     Underscore    \\_\n";
print DTX "\%\%   Grave accent  \\\`     Left brace    \\{     Vertical bar  \\|\n";
print DTX "\%\%   Right brace   \\}     Tilde         \\~}\n";
print DTX "\%</package>\n";
print DTX "\%\\fi\n";

print DTX "\% \\iffalse\n";
print DTX "\% Doc-Source file to use with LaTeX2e\n";
print DTX "\% Copyright (C) $year $author, all rights reserved.\n";
print DTX "\% \\fi\n";

# driver

print DTX "\% \\iffalse\n";
print DTX "\%<*driver>\n";

$indoc=0;

while (<DOC>)
{
   s/\\usepackage{creatdtx}//;

   $restofline = $_;

   $beginline = "";
   $line = $restofline;

   while ($restofline =~ /(.*)\\ifmakedtx(.*)/)
   {
      $beginline = $1;

      ($group,$restofline,$done) = &getnextgroup($2);

      $startline = $.;

      while (!$done)
      {
         if ($nextline = <DOC>)
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
         if ($nextline = <DOC>)
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

   print DTX $line;

   if ($line=~/\\begin{document}/)
   {
      $indoc = 1;

      last;
   }
}

print DTX "\\DocInput{$basename.dtx}\n";
print DTX "\\end{document}\n";
print DTX "\%</driver>\n";
print DTX "\%\\fi\n";

$inverb=0;
$stopfound=0;

print DTX "\%";

while (<DOC>)
{
   if (/\\begin{verbatim}/)
   {
      $inverb=1;
   }

   if (/\\end{verbatim}/)
   {
      $inverb=0;
   }

   if (/\\StopEventually/ && ($inverb==0))
   {
      $stopfound=1;
   }

   $restofline = $_;

   $beginline = "";
   $line = $restofline;

   while ($restofline =~ /(.*)\\ifmakedtx(.*)/)
   {
      $beginline = $1;

      ($group,$restofline,$done) = &getnextgroup($2);

      $startline = $.;

      while (!$done)
      {
         if ($nextline = <DOC>)
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
         if ($nextline = <DOC>)
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

   if (($line=~/\\end{document}/) and not $inverb)
   {
      $indoc=0;

      $line=~s/\\end{document}//;
   }

   $line=~s/\n/\n\%/mg;

   print DTX "$line";
}

close DOC;

print DTX "\n";

if ($stopfound==0)
{
   print DTX "\%\\StopEventually{$stopeventually}\n";
}

print DTX "\%\\section{$codetitle}\n";

for (my $idx = 0; $idx <= $#derivedfiles; $idx++)
{
   $thisinfile = $derivedfiles[$idx]{'in'};
   $thisoutfile = $derivedfiles[$idx]{'out'};

   if ($verbose)
   {
         print "$srcdirfile -> $_ \n";
   }

   open SRC, $thisinfile or die "Can't open $thisinfile\n";

   print DTX "\%\\iffalse\n";
   print DTX "\%    \\begin{macrocode}\n";
   print DTX "\%<*$thisoutfile>\n";
   print DTX "\%    \\end{macrocode}\n";
   print DTX "\%\\fi\n";

   $macrocode = 0;
   $comment   = 0;

   foreach $expr (@comment)
   {
      if ($thisoutfile =~ m/$expr/)
      {
         print DTX "\%\\iffalse\n";

            $comment = 1;
      }
   }

   foreach $expr (@macrocode)
   {
      if ($thisoutfile =~ m/$expr/)
      {
         print DTX "\%    \\begin{macrocode}\n";

         $macrocode = 1;
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

if (!$noins)
{
   open INS, ">$basename.ins" or die "Can't open '$basename.ins'\n";

   print INS "\% $basename.ins generated using makedtx version $version $year/",$mon+1,"/$mday $hour:", $min<10?"0$min":$min,"\n";

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
      $file = $derivedfiles[$idx]{'in'};
      $outfile = $derivedfiles[$idx]{'out'};

      print INS "\\file{$outfile}{";

      $ambleset = 0;
      $noamble  = 0;

      foreach $setamble (@setambles)
      {
         ($fileexp, $amble, $remainder) = split /=>/, $setamble;

         if (not ($remainder eq ""))
         {
            die "-setambles $setamble argument invalid (too many => specified)\n";
         }

         if ($outfile =~ m/$fileexp/)
         {
            if ($verbose)
            {
               print "$fileexp matches $outfile -> setting \"$amble\"\n";
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

      print INS "\\from{$basename.dtx}{$outfile";

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
   print "makedtx Help\n\n";

   print "Current Version : $version\n\n";

   print "usage : makedtx [options] -src \"<expr>=><expr>\" -doc <filename> <basename>\n\n";

   print "makedtx can be used to construct a LaTeX2e dtx and ins file from\n";
   print "the specified source code.  The final command line argument\n";
   print "<basename> should be used to specify the basename of the dtx\n";
   print "and ins files.\n\n";

   print "-src \"<expr1>=><expr2>\"\n";
   print "The command line switch -src identifies the original source code and the name\n";
   print "of the file to which it will utimately be extracted on latexing the ins file\n";
   print "<expr1> can be a Perl expression, such as (foo)src.(sty), and <expr2> can\n";
   print "a Perl substitution style expression, such as $1.$2\n";
   print "Note that double quotes must be used to prevent shell expansion\n";
   print "Multiple invocations of -src are permitted\n";
   print "See examples below.\n\n";

   print "-doc <filename>\n";
   print "The name of the documentation source code.  This should be a LaTeX2e document\n\n";

   print "Optional Arguments:\n\n";

   print "-dir <directory>   : search for source files in <directory>\n";
   print "-op <character>    : set the pattern matching operator (default '$patternop')\n";
   print "-askforoverwrite   : set askforoverwrite switch in INS file to true\n";
   print "-noaskforoverwrite : set askforoverwrite switch in INS file to false (default)\n";
   print "-preamble <text>   : set the preamble.  Standard one inserted if omitted\n";
   print "-postamble <text>  : set the postamble.\n";
   print "-setambles \"<pattern>=><text>\" : set pre- and postambles to <text> if file matches pattern\n";
   print "-author <text>     : name of author (inserted into standard preamble. User name inserted if omitted)\n";
   print "-date <text>       : copyright date\n";
   print "-ins               : create the ins file (default)\n";
   print "-noins             : don't create the ins file\n";
   print "-prefinale <text>  : add <text> immediately prior to \\Finale\n";
   print "-macrocode <expr>  : surround any file which matches <expr> in a macrocode environment\n";
   print "-comment <expr>    : surround any file which matches <expr> with \\iffalse \\fi pair\n";
   print "-codetitle <text>  : The title for the documented code section (default: The Code)\n";
   print "-license <license> : use the given license for the preamble.\n";
   print "                     Known licenses: lppl (default), bsd, gpl.\n";
   print "-h                 : help message\n";
   print "-v                 : verbose\n\n";

   print "Examples:\n\n";

   print "Example 1:\n";
   print "Documenation is in foodoc.tex\n";
   print "Source code is in foosrc.sty.  The final extracted version should be \n";
   print "called foo.sty. The dtx file should be called foo.dtx and the ins file\n";
   print " should be called foo.ins\n\n";

   print "makedtx -src \"foosrc\\.sty=>foo.sty\" -doc foodoc.tex foo\n\n";

   print "Example 2:\n";
   print "Documenation is in bardoc.tex\n";
   print "Source code is in barsrc.sty.  The final extracted version should be\n";
   print "called bar.sty.  Source code is also in barsrc.bst.  The final extracted\n";
   print "version should be called bar.bst.  The dtx file should be called bar.dtx and\n";
   print "the ins file should be called bar.ins\n\n";

   print "makedtx -src \"barsrc\\.sty=>bar.sty\" -src \"barsrc\\.bst=>bar.bst\" -doc bardoc.tex bar\n\n";

   print "Or\n\n";

   print "makedtx -src \"barsrc\\.(bst|sty)=>bar.\$1\" -doc bardoc.tex bar\n\n";

   die;
}

sub eatinitialspaces
{
   my ($STR) = @_;

   while (substr($STR,0,1) eq "\s")
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

   if (($group = substr($curline,0,1)) ne "{")
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

         $i = $posopen-1;

         $numbs = 1;

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

         $i = $posclose-1;

         $numbs = 1;

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
            $group = substr($curline, $startpos+1, $pos-$startpos-1);

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
            $group = substr($curline, $startpos+1, $pos-$startpos-1);

            $curline = substr($curline, $pos+1);

            return ($group,$curline,1);
         }
      }
   }

   # closing brace must be on another line

   return ("", $curline, 0);
}
1;
