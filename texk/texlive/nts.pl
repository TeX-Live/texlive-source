#!/usr/bin/env perl
# project : nts-zero / nts@dante.de
# date    : 12 September 2001
# author  : Hans Hagen / www.pragma-ade.com

# This is a wrapper for NTS, the reimplementation of TeX in 
# Java. Don't laugh too loud about such overkill in 
# launching a program. 
#
# We are a bit tolerant in user input and clean up incomplete 
# file, format and progname specifications. We do so because
# so many users today generate formats. 
#
# This script is not structured, is not organized in an 
# object oriented way, is as sequential as can be, but does 
# an acceptable job. A couple of more options may be added 
# later. 
#
# ConTeXt users can use TeXExec as with any TeX, which means
# that you can say: 
#
#   texexec --make --alone --tex=nts en 
#   texexec --tex=nts yourfile 
#
# or set up texexec.ini accordingly if you want to use NTS 
# as production system.
# 
# Be prepared for a slow run; compared to (pdf)(e)tex.
# Speaking of version 1.0-beta, a job will take to over 100
# times as much run time as normally. If you encounter
# incompatibilities or bugs, don't hesitate to report them to 
# 
#   nts@dante.de 
#
# Your message will be then forwarded to the right people. If 
# possible, provide a minimal test file. 

use strict ;
use Config ;
use Getopt::Long ;
use File::Copy ;

# We are tolerant with respect to options.

$Getopt::Long::passthrough = 1 ; # no error message
$Getopt::Long::autoabbrev  = 1 ; # partial switch accepted

# These are the switch related variables.

my $Initialize  = 0 ;
my $Interaction = "" ;
my $FormatName  = "plain" ;
my $ProgName    = "tex" ;
my $Version     = 0 ; 

# Some bonus switches.

my $Trace    = 0 ;
my $ShowHelp = 0 ;
my $Test     = 0 ;

# There are some NTS specific switches.

my $LogSuffix = "" ;
my $DviSuffix = "" ;

# And another bonus: 

my $JarFile = "nts.rt.jar" ;

# Only the relevant switches are implemented. Watch the
# alternative format switches.

&GetOptions
  ( "initialize"      => \$Initialize, # also catches --ini
    "interaction=s"   => \$Interaction,
    "format=s"        => \$FormatName,
    "fmt=s"           => \$FormatName,
    "nfmt=s"          => \$FormatName,
    "efmt=s"          => \$FormatName,
    "program=s"       => \$ProgName,
    "progname=s"      => \$ProgName,
    "log-extension=s" => \$LogSuffix,
    "logextension=s"  => \$LogSuffix,
    "dvi-extension=s" => \$DviSuffix,
    "dviextension=s"  => \$DviSuffix,
    "jar-file=s"      => \$JarFile, 
    "jarfile=s"       => \$JarFile, 
    "version"         => \$Version, 
    "test"            => \$Test,
    "trace"           => \$Trace,
    "help"            => \$ShowHelp) ;

# No explanation is needed here. We report the switches in a
# web2c conforming way but are more tolerant. 

if ($ShowHelp)
  { print
     "\n" .
     "usage : nts [switches] filename\n" .
     "\n" .
     "--ini           : set all variables to their initial value\n" .
     "--fmt           : use a previously generated format\n" .
     "--progname      : symbolic name that sets up kpsewhich file search\n" .
     "--interaction   : enable one of tex's interaction modes\n" .
     "--version       : report the current NTS version\n" . 
     "\n" .
     "--log-extension : suffix of logfile\n" .
     "--dvi-extension : suffix of dvifile\n" .
     "--jar-file      : alternative jar archive (default: $JarFile)\n" . 
     "\n" .
     "--trace         : show command and runtime\n" .
     "--test          : turn on tracing and use other suffixes\n" .
     "--help          : this message\n" .
     "\n" .
     "context : nts --ini cont-en\n" . 
     "          nts --fmt=cont-en yourfile\n" .
     "latex   : nts --ini latex.ltx\n" . 
     "          nts --fmt=latex yourfile\n" .
     "plain   : nts --ini plain \\dump\n" . 
     "          nts yourfile\n" ;
    exit }

# Testing. It may be a good idea to save the existing log
# and dvi file.

if ($Test)
  { $Trace = 1 ; $LogSuffix = "log.nts" ; $DviSuffix = "dvi.nts" }

# We do version testing in a virgin instance of NTS. 

$Initialize = 1 ; 

# For myself and those who are unaware of the face that the
# ConTeXt user interfaces have names different from the program
# name.

if (($FormatName =~ /^cont\-/o)||($ARGV[0] =~ /^cont\-/o))
  { $ProgName = "context" }

# We have to look for both the format and filename. We
# locate the jarfile by kpsewhich.

my $FileName = shift @ARGV ;
my $Command  = "java -jar" ;

# Compensate invalid user input.  

if ($JarFile eq "") { $JarFile = "nts.rt.jar" } 

# Locate (user supplied) jar file if not on local path. 

unless (-e $JarFile) 
  { $JarFile  = `kpsewhich -progname=nts -format="other binary files" $JarFile` ;
    chomp $JarFile ; $JarFile =~ s/\\/\//go }

# As last resort, take normal jar file if not yet found.

unless (-e $JarFile)
  { $JarFile = "nts.rt.jar" ;
    $JarFile = `kpsewhich -progname=nts -format="other binary files" $JarFile` ;
    chomp $JarFile ; $JarFile =~ s/\\/\//go }

# Abort if jar file is not found. 

unless (-e $JarFile)
  { print
      "\n" .
      "The file 'nts.rt.jar' cannot be found. You should copy the file\n" .
      "'nts-version.rt.jar' file to 'nts.rt.jar' and put it in one\n" .
      "of the texmf trees on your system. The 'jar' file should go\n" .
      "into the subtree 'texmf/nts/java' or 'texmf-local/nts/java'\n" .
      "or another similar path. Run 'mktexlsr' afterwards.\n" .
      "\n" ;
    exit }

# The progname directive enable efficient path searching.

if ($ProgName    ne "") { $Command .= " -Dnts.progname=$ProgName" }
if ($Interaction ne "") { $Command .= " -Dnts.interaction=$Interaction" }

# We need to collect all arguments.

if ($FileName =~ s/^\&//o)
  { $FormatName = $FileName ;
    $FileName = shift @ARGV }

my $FullName = "$FileName.nfmt" ;

# We normalize the format file suffix. For those who use TeX
# or eTeX and don't know that NTS has its own format suffix,
# we normalize their input.

$FormatName =~ s/\..*//io ; $FormatName .= ".nfmt" ;

# We need to pass on a couple NTS specific switches. Watch 
# the funny period. 

if ($LogSuffix ne "") { $Command .= " -Dnts.log.extension=$LogSuffix " }
if ($DviSuffix ne "") { $Command .= " -Dnts.dvi.extension=$DviSuffix " }

# Check for LaTeX suffix. 

if (($Initialize)&&($FileName =~ /latex/io)) 
  { $FileName =~ s/\..*//io ; $FileName .= ".ltx" }

# Append optional TeX input. 

$FileName .= " " . join (" ",@ARGV) ;

# The command line arguments differ for iniNTS and normal
# NTS. Watch how the Jarfile comes after the switches.

if ($Version) 
  { $Command .= " $JarFile \\end" }
elsif ($Initialize)
  { $Command .= " $JarFile $FileName" }
else
  { $Command .= " -Dnts.fmt=$FormatName $JarFile $FileName" }

# When turned on, tracing will result in some more
# information.

if ($Trace) { print "\n" . "command : $Command\n" . "\n" }

# We start timing.

my $StartTime = time ;

# Now we run nts!

my $Result = 0 ; 

if ($Version) 
  { $Result = `$Command` }  
else
  { $Result = system($Command) }

# If needed, we copy the formats to their natural location.

if (($Initialize)&&(-f $FullName))
  { my $FullPath = `kpsewhich -expand-var \$TEXFORMATS` ;
    chomp $FullPath ; $FullPath =~ s/\\/\//go ;
    if (-d $FullPath)
      { if ($Trace) { print "\naction : copying format\n" }
        unlink $FullName if copy ($FullName,"$FullPath/$FullName") ;
        if ($Trace) { print "action : generating database\n\n" }
        system ("mktexlsr") } }

# Finally we report the runtime, if asked for. Why? Because
# NTS is so slow that one will definitely want to go for a break
# when running a big file.

my $StopTime  = time - $StartTime ;

if ($Version) 
  { my @Version = split(/\n/,$Result) ;
    $Result = ($Version[0] =~ /version/io) ;  
    if ($Result)
      { print "$Version[0]" } }
elsif ($Trace)
  { print
      "\n" .
      "command : $Command\n" .
      "runtime : $StopTime seconds\n" .
      "result  : $Result\n" .
      "\n" }
exit $Result ; 
