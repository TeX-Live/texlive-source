#!/usr/bin/env perl
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# mk4ht                                 Version 1.1      %
# Copyright (C) 2003--2009               Eitan M. Gurari %
# Copyright 2009 TeX Users Group                         %
#                                                        %
# This work may be distributed and/or modified under the %
# conditions of the LaTeX Project Public License, either %
# version 1.3 of this license or (at your option) any    %
# later version. The latest version of this license is   %
# in                                                     %
#   http://www.latex-project.org/lppl.txt                %
# and version 1.3 or later is part of all distributions  %
# of LaTeX version 2003/12/01 or later.                  %
#                                                        %
# This work has the LPPL maintenance status "maintained".%
#                                                        %
# The Current Maintainer of this work                    %
# is the TeX4ht Project.                                 %
#                                                        %
#                                        tex4ht@tug.org  %
#                             http://www.tug.org/tex4ht  %
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

use strict;
$^W=1; # turn warning on
my $texFile = '';
if( @ARGV  ){
  my(@array) = split('\.',@ARGV[1]);
  $texFile = @array[0];
}
my @options = (
 "",     "ht",        "",         "", "", "-cvalidatehtml",
 "ht",   "htlatex",   "latex",    "", "", "-cvalidatehtml",
 "ht",   "httex",     "tex",      "", "", "-cvalidatehtml",
 "ht",   "httexi",    "texi",     "", "", "-cvalidatehtml",
 "ht",   "htcontext", "context",  "", "", "-cvalidatehtml",
 "ht",   "htxetex",   "xetex",    "", "", "-cvalidatehtml",
 "ht",   "htxelatex", "xelatex",  "", "", "-cvalidatehtml",
 "xh", "xhlatex",  "latex",   "xhtml", "", "-cvalidate",
 "xh", "xhtex",    "tex",     "xhtml", "", "-cvalidate",
 "xh", "xhtexi",   "texi",    "xhtml", "", "-cvalidate",
 "xh", "xhcontext","context", "xhtml", "", "-cvalidat",
 "xh", "xhxelatex", "xelatex", "xhtml", "", "-cvalidate",
 "xh", "xhxetex",  "xetex",   "xhtml", "", "-cvalidate",
 "uxh", "uxhlatex",  "latex",   "xhtml,uni-html4", " -cunihtf", "-cvalidate",
 "uxh", "uxhtex",    "tex",     "xhtml,uni-html4", " -cunihtf", "-cvalidate",
 "uxh", "uxhtexi",   "texi",    "xhtml,uni-html4", " -cunihtf", "-cvalidate",
 "uxh", "uxhcontext","context",  "xhtml,uni-html4", " -cunihtf", "-cvalidate",
 "uxh", "uxhxelatex",  "xelatex",   "xhtml,uni-html4", " -cunihtf", "-cvalidate",
 "uxh", "uxhxetex",    "xetex",     "xhtml,uni-html4", " -cunihtf", "-cvalidate",
 "xhm", "xhmlatex", "latex",  "xhtml,mathml", " -cunihtf", "-cvalidate",
 "xhm", "xhmtex",   "tex",    "xhtml,mathml", " -cunihtf", "-cvalidate",
 "xhm", "xhmtexi",  "texi",   "xhtml,mathml", " -cunihtf", "-cvalidate",
 "xhm", "xhmcontext","context", "xhtml,mathml", " -cunihtf", "-cvalidate",
 "xhm", "xhmxelatex", "xelatex",  "xhtml,mathml", " -cunihtf", "-cvalidate",
 "xhm", "xhmxetex",   "xetex",    "xhtml,mathml", " -cunihtf", "-cvalidate",
 "mz", "mzlatex",   "latex",   "xhtml,mozilla", " -cmozhtf",  "-cvalidate",
 "mz", "mztex",     "tex",     "xhtml,mozilla", " -cmozhtf",  "-cvalidate",
 "mz", "mztexi",    "texi",    "xhtml,mozilla", " -cmozhtf",  "-cvalidate",
 "mz", "mzcontext", "context", "xhtml,mozilla", " -cmozhtf",  "-cvalidate",
 "mz", "mzxelatex", "xelatex",   "xhtml,mozilla", " -cmozhtf",  "-cvalidate",
 "mz", "mzxetex",   "xetex",     "xhtml,mozilla", " -cmozhtf",  "-cvalidate",
 "oo", "oolatex",   "latex",   "xhtml,ooffice", "ooffice/\! -cmozhtf",  "-cooxtpipes -coo",
 "oo", "ootex",     "tex",     "xhtml,ooffice", "ooffice/\! -cmozhtf",  "-cooxtpipes -coo",
 "oo", "ootexi",    "texi",    "xhtml,ooffice", "ooffice/\! -cmozhtf",  "-cooxtpipes -coo",
 "oo", "oocontext", "context", "xhtml,ooffice", "ooffice/\! -cmozhtf",  "-cooxtpipes -coo",
 "oo", "ooxelatex",   "xelatex",   "xhtml,ooffice", "ooffice/\! -cmozhtf",  "-cooxtpipes -coo",
 "oo", "ooxetex",     "xetex",     "xhtml,ooffice", "ooffice/\! -cmozhtf",  "-cooxtpipes -coo",
 "es", "eslatex",   "latex",   "xhtml,emspk", " -cemspkhtf -s4es",  "-cemspk",
 "es", "estex",     "tex",     "xhtml,emspk", " -cemspkhtf -s4es",  "-cemspk",
 "es", "estexi",    "texi",    "xhtml,emspk", " -cemspkhtf -s4es",  "-cemspk",
 "es", "escontext", "context", "xhtml,emspk", " -cemspkhtf -s4es",  "-cemspk",
 "es", "esxelatex",   "xelatex",   "xhtml,emspk", " -cemspkhtf -s4es",  "-cemspk",
 "es", "esxetex",     "xetex",     "xhtml,emspk", " -cemspkhtf -s4es",  "-cemspk",
 "js", "jslatex",   "latex",   "xhtml,jsml", " -cjsmlhtf",  "-cjsml",
 "js", "jstex",     "tex",     "xhtml,jsml", " -cjsmlhtf",  "-cjsml",
 "js", "jstexi",    "texi",    "xhtml,jsml", " -cjsmlhtf",  "-cjsml",
 "js", "jscontext", "context", "xhtml,jsml", " -cjsmlhtf",  "-cjsml",
 "js", "jsxelatex",   "xelatex",   "xhtml,jsml", " -cjsmlhtf",  "-cjsml",
 "js", "jsxetex",     "xetex",     "xhtml,jsml", " -cjsmlhtf",  "-cjsml",
 "jm", "jmlatex",   "latex",   "xhtml,jsmath", " -cmozhtf", "",
 "jm", "jmtex",     "tex",     "xhtml,jsmath", " -cmozhtf", "",
 "jm", "jmtexi",    "texi",    "xhtml,jsmath", " -cmozhtf", "",
 "jm", "jmcontext", "context", "xhtml,jsmath", " -cmozhtf", "",
 "jm", "jmxelatex",   "xelatex",   "xhtml,jsmath", " -cmozhtf", "",
 "jm", "jmxetex",     "xetex",     "xhtml,jsmath", " -cmozhtf", "",
 "tei",  "teilatex",  "latex",   "xhtml,tei",    " -cunihtf",  "-cvalidate",
 "tei",  "teitex",    "tex",     "xhtml,tei",    " -cunihtf",  "-cvalidate",
 "tei",  "teitexi",   "texi",    "xhtml,tei",    " -cunihtf",  "-cvalidate",
 "tei",  "teicontext","context", "xhtml,tei",    " -cunihtf",  "-cvalidate",
 "teim", "teimlatex", "latex",   "xhtml,tei-mml"," -cunihtf",  "-cvalidate",
 "teim", "teimtex",   "tex",     "xhtml,tei-mml"," -cunihtf",  "-cvalidate",
 "teim", "teimtexi",  "texi",    "xhtml,tei-mml"," -cunihtf",  "-cvalidate",
 "teim", "teimcontext","context","xhtml,tei-mml"," -cunihtf",  "-cvalidate",
 "tei",  "teixelatex",  "xelatex",   "xhtml,tei",    " -cunihtf",  "-cvalidate",
 "tei",  "teixetex",    "xetex",     "xhtml,tei",    " -cunihtf",  "-cvalidate",
 "db",  "dblatex",   "latex",   "xhtml,docbook",     " -cunihtf",  "-cvalidate -cdocbk",
 "db",  "dbtex",     "tex",     "xhtml,docbook",     " -cunihtf",  "-cvalidate -cdocbk",
 "db",  "dbtexi",    "texi",    "xhtml,docbook",     " -cunihtf",  "-cvalidate -cdocbk",
 "db",  "dbcontext", "context", "xhtml,docbook",     " -cunihtf",  "-cvalidate -cdocbk",
 "dbm", "dbmlatex",  "latex",   "xhtml,docbook-mml", " -cunihtf",  "-cdocbk",
 "dbm", "dbmtex",    "tex",     "xhtml,docbook-mml", " -cunihtf",  "-cdocbk",
 "dbm", "dbmtexi",   "texi",    "xhtml,docbook-mml", " -cunihtf",  "-cdocbk",
 "dbm", "dbmcontext","context", "xhtml,docbook-mml", " -cunihtf",  "-cdocbk",
 "db",  "dbxelatex",   "xelatex",   "xhtml,docbook",     " -cunihtf",  "-cvalidate -cdocbk",
 "db",  "dbxetex",     "xetex",     "xhtml,docbook",     " -cunihtf",  "-cvalidate -cdocbk",
 "w", "wlatex",   "latex",   "xhtml,word", " -csymhtf", "",
 "w", "wtex",     "tex",     "xhtml,word", " -csymhtf", "",
 "w", "wtexi",    "texi",    "xhtml,word", " -csymhtf", "",
 "w", "wcontext", "context", "xhtml,word", " -csymhtf", "",
 "w", "wxelatex",   "xelatex",   "xhtml,word", " -csymhtf", "",
 "w", "wxetex",     "xetex",     "xhtml,word", " -csymhtf", "",
 "jh", "jhlatex",  "latex",  "html,javahelp,xml,3.2,unicode", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp -cvalidatehtml",
 "jh", "jhtex",    "tex",    "html,javahelp,xml,3.2,unicode", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp -cvalidatehtml",
 "jh", "jhtexi",   "texi",   "html,javahelp,xml,3.2,unicode", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp -cvalidatehtml",
 "jh", "jhcontext","context", "html,javahelp,xml,3.2,unicode", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp -cvalidatehtml",
 "jh", "jhxelatex",  "xelatex",  "html,javahelp,xml,3.2,unicode", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp -cvalidatehtml",
 "jh", "jhxetex",    "xetex",    "html,javahelp,xml,3.2,unicode", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp -cvalidatehtml",
 "jh1", "jh1latex",  "latex",  "html,javahelp,xml,3.2,unicode,jh1.0", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp",
 "jh1", "jh1tex",    "tex",    "html,javahelp,xml,3.2,unicode,jh1.0", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp",
 "jh1", "jh1texi",   "texi",   "html,javahelp,xml,3.2,unicode,jh1.0", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp",
 "jh1", "jh1context","context", "html,javahelp,xml,3.2,unicode,jh1.0", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp",
 "jh1", "jh1xelatex",  "xelatex",  "html,javahelp,xml,3.2,unicode,jh1.0", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp",
 "jh1", "jh1xetex",    "xetex",    "html,javahelp,xml,3.2,unicode,jh1.0", " -cmozhtf -u10", " -d$texFile-doc/ -cjavahelp",

);

sub showInstrucions(){
  print " option1:  mk4ht #1 \"#2\" \"#3\" \"#4\" \"#5\"\n";
  print " \n";
  print "    #1: htlatex, xhlatex, mzlatex, oolatex, dblatex, dbmlatex,\n";
  print "        jhlatex, eslatex, teilatex, teimlatex, uxhlatex,  \n";
  print "        wlatex, xhmlatex\n";
  print " \n";
  print "        also 'tex', 'texi', 'context', 'xetex', and 'xelatex'\n";
  print "        instead of 'latex'\n";
  print " \n";
  print "    #2: file name\n";
  print "    #3: optional arguments for latex/tex/... \n";
  print "    #4: optional arguments for tex4ht.c\n";
  print "    #5: optional arguments for t4ht.c\n";
  print " \n";
  print " option2:  mk4ht ht #2 #3 \"#4\" \"#5\"\n";
  print " \n";
  print "    #1: ht\n";
  print "    #2: latex, tex\n";
  print "    #3: file name\n";
  print "    #4: optional arguments for tex4ht.c\n";
  print "    #5: optional arguments for t4ht.c\n";
  print " \n";
  print " Within the program, in column three of the options\n";
  print " variable, the  requests for the commands \"latex\",\n";
  print " \"tex\", etc. can be replaced with other equivalent\n";
  print " commands (e.g., \"tex -fmt=latex\").\n";

  print "--------------------------------------------------------------------------\n";
print "           Private configuration file: mk4ht.cfg\n";
print "--------------------------------------------------------------------------\n";
print "\n";
print "A private configuration file mk4ht.cfg or .mk4ht may be placed at the\n";
print "work or home directory, to update existing commands and introduce new\n";
print "ones. The configuration file may contain records of the following\n";
print "kinds.\n";
print "\n";
print "   #  Comment\n";
print "    \n";
print "   name = type\n";
print "          Defines a ht*tex like command, and assocites to it the \n";
print "          TeX compiler of the specified type. Examples of TeX \n";
print "          types: latex, tex, texi, context, xetex, and xelatex.\n";
print "    \n";
print "   name.tex = options\n";
print "          Command line options for the compilation under\n";
print "          the (la)tex compiler\n";
print "      \n";
print "   name.tex4ht = options\n";
print "          Command line options for tex4ht.c \n";
print "    \n";
print "   name.t4ht = options\n";
print "          Command line options for t4ht.c\n";
print "    \n";
print "Each record should appear in a different line.  Variants\n";
print "`name.tex += options', `name.tex4ht += options',\n";
print "`name.t4ht += options' of the above records are also allowed.\n";
print "They append the listed options to the base values.\n";
print "\n";
print "Example:\n";
print "\n";
print "   foohlatex         = latex\n";
print "   foohlatex.tex     = xhtml,uni-html4\n";
print "   foohlatex.tex4ht += -cunihtf\n";
print "   foohlatex.t4ht    = -cvalidate\n";
print "   htlatex.t4ht     += -d./\n";


  print "--------------------------------------------------------------------------\n";
print "           Deleting files\n";
print "--------------------------------------------------------------------------\n";
print "\n";
print "The configuration file mk4ht.cfg may also contain requests for\n";
print "removing files created in the work directory during the compilation.\n";
print "The requests are to be made through records of the following forms.\n";
print "\n";
print "   clean ext1 ext2 ...\n";
print "     The extensions of the file name to be removed.\n";
print "   \n";
print "   clean.name ext1 ext2 ...\n";
print "     Conditional request. The `name' refers to the ht*tex \n";
print "     like command in use.\n";
print "\n";
print "Example:\n";
print "  clean             dvi idv\n";
print "  clean.foohlatex   lg \n";
print "  clean.htlatex     lg tmp\n";

}

print "mk4ht (Version 1.1)\n";
if(  !@ARGV  ){
  print "improper command\n";
  showInstrucions(); exit(1);
}
my @command=("","","","","");
my $i=0;
my $j=0;
my $param;
my $name;
my $compiler;
my $tex;
my $tex4ht;
my $t4ht;
my $texp;
my $tex4htp = "";
my $t4htp = "";
my @ext;

foreach $param (@ARGV) {
  if( $i == 0 ){
    my $inf;
open $inf, "<mk4ht.cfg"
or
(  open $inf, "<.mk4ht"
   or
  (
     open $inf, "<" . $ENV{HOME} . "/mk4ht.cfg"
     or
     (
        open $inf, "<" . $ENV{HOME} . "/.mk4ht"
        or $inf = ""
) )  )
;

if( $inf ){
   print "(mk4ht cfg)\n";
   while(<$inf>) {
     my($line) = $_;
     chomp($line);      # remove eoln char
     if ($line =~ m|\s*#.*|) {}
     elsif($line =~ m|^\s*(\S*)\.(\S*)\s*\+=\s*(.*\S)\s*$|) {
         if( ($param."tex4ht") eq ($1.$2) ){
              $tex4htp = $tex4htp . " " . $3;
         }
         elsif( ($param."t4ht") eq ($1.$2) ){
              $t4htp = $t4htp . " " . $3;
         }
         elsif( ($param."tex") eq ($1.$2) ){
              $texp = $texp . "," . $3;
         }
     }
     elsif($line =~ m|^\s*(\S*)\.(\S*)\s*=\s*(.*\S)\s*$|) {
         if( ($param."tex4ht") eq ($1.$2) ){
              $tex4ht = $3 . " ";
              $tex4htp = "";
         }
         elsif( ($param."t4ht") eq ($1.$2) ){
              $t4ht = $3 . " ";
              $t4htp = "";
         }
         elsif( ($param."tex") eq ($1.$2) ){
              $tex = $3 . ",";
              $texp = "";
         }
     }
     elsif($line =~ m|^\s*(\S*)\s*=\s*(.*\S)\s*$|) {
         if( $param eq $1 ){
              $name = $1;
              $compiler = $2;
         }
     }
     elsif($line =~ m|^\s*clean\s+(.+)|){
   my(@array) = split(' ',$1);
   push(@ext,@array);
} elsif($line =~ m|^\s*clean\.(\S+)\s+(.+)|){
   if( $1 eq @ARGV[0] ){
      my(@array) = split(' ',$2);
      push(@ext,@array);
}  }

     elsif ($line) { print "--- Error --- " . $line . "\n"; }
   }
   close $inf;
}

    for( $j=1; $j<$#options; $j+=6 ){
      if( $param eq $options[$j] ){
         if( $name ){
            $command[0] = "ht".$compiler;
if( $tex    ){ $command[2] = $tex;    } else { $command[2] = ""; }
if( $tex4ht ){ $command[3] = $tex4ht; } else { $command[3] = ""; }
if( $t4ht   ){ $command[4] = $t4ht;   } else { $command[4] = ""; }

         } else {
            if( $options[$j-1] eq "" ){
  $command[0] = $options[$j];
} else {
  $command[0] = "ht".$options[$j+1];
}
if( $tex    ){ $command[2] = $tex;    } else { $command[2] = $options[$j+2]; }
if( $tex4ht ){ $command[3] = $tex4ht; } else { $command[3] = $options[$j+3]; }
if( $t4ht   ){ $command[4] = $t4ht;   } else { $command[4] = $options[$j+4]; }

         }
         if( $texp   ){ $command[2] = $command[2] . "," . $texp; }
if( $tex4htp){ $command[3] = $command[3] . " " . $tex4htp; }
if( $t4htp  ){ $command[4] = $command[4] . " " . $t4htp; }

         last;
    } }
    if( $j>$#options ){
      if( $name ){ $command[0] = "ht".$compiler;
if( $tex    ){ $command[2] = $tex;    } else { $command[2] = ""; }
if( $tex4ht ){ $command[3] = $tex4ht; } else { $command[3] = ""; }
if( $t4ht   ){ $command[4] = $t4ht;   } else { $command[4] = ""; }
 }
      else {
          print "improper command: $param \n";
          showInstrucions(); exit(1);
    } }
  } elsif ( $i== 1 ) {
    $command[1] = $param;
  } elsif ( $i== 2 ) {
    if( $command[2] eq "" ){
      $command[2] = $param;
    } else {
      $command[2] = $param. "," . $command[2];
    }
  } elsif ( $i== 3 ) {
    $command[3] = $param . $command[3];
  } else {
    $command[4] = $param. " " .$command[4];
  }
  $i++;
}
my $cmd;
open (KPSEA, "kpsewhich " . $command[0] . " |");
if ($cmd = <KPSEA>){
  $cmd =~ s/\s+$//;
} else {
  $cmd = $command[0];
}
close KPSEA;

my $commando = $cmd . " "    . $command[1] . " \"".
            $command[2] . "\" \"". $command[3] . "\" \"" .
            $command[4] . "\"";
print "$commando\n";
my $rtrn;
if( $rtrn = system($commando) ){
   print "--- error --- failed to execute command\n";
} else {
   my $file;
my $ext;
opendir(DIR,".") ;
while ($file = readdir(DIR) ){
  if(index($file,$texFile) == 0 ){
    foreach $ext(@ext){
    if (index($file,$ext,length($file)-length($ext)) != -1){
      if( stat($file)){
         unlink($file);
         print  "Deleted: ". $file . "\n";
} } } } }
closedir(DIR);

}
exit( $rtrn );

