#! /usr/bin/env perl
use strict;
use warnings;
use Term::ANSIColor;
use Encode;
use POSIX qw(locale_h);
use locale;
setlocale(LC_CTYPE,"no_NO");

my $versionnumber="2.2";
my $versiondate="2009 Apr 30";

###### Set CMD specific settings and variables

# Options and states
my $verbose=0;
my $showcodes=1;
my $showstates=0;
my $showsubcounts=0;
my $htmlstyle=0;
my $includeTeX=0;
my $briefsum=0;
my @sumweights;
my $utf8flag=0;

# Global variables
my $blankline=0;
my $errorcount=0;

# CMD specific global variables
my $totalflag=0;
my @filelist;
my $workdir;
my $globalworkdir="";

###### Set global settings and variables

### Macros for headers
# Macros that identify headers: i.e. following token or
# {...} is counted as header. The =>[2] indicates transition to
# state 2 which is used within headers (although the value is
# actually never used). This is copied to %TeXmacro and the
# only role of defining it here is that the counter for the number
# of headers is incremented by one.
my %TeXheader=('\title'=>[2],'\part'=>[2],'\chapter'=>[2],
               '\section'=>[2],'\subsection'=>[2],'\subsubsection'=>[2],
               '\paragraph'=>[2],'\subparagraph'=>[2]);

### How many tokens to gobble after macro
# Each macro is assumed to gobble up a given number of
# tokens (or {...} groups), as well as options [...] before, within
# and after. The %TeXmacro hash gives a link from a macro
# (or beginNAME for begin-end groups without the backslash)
# to either an integer giving the number of tokens to ignore
# or to an array (specified as [num,num,...]) of length N where
# N is the number of tokens to be read with the macro and the
# array values tell how each is to be interpreted (see the status
# values: 0=ignore, 1=count, etc.). Thus specifying a number N is
# equivalent to specifying an array [0,...,0] of N zeros.
#
# For macros not specified here, the default value is 0: i.e.
# no tokens are excluded, but [...] options are. Header macros
# specified in %TeXheader are automatically included here.
my %TeXmacro=(%TeXheader,
     '\documentclass'=>1,'\documentstyle'=>1,'\usepackage'=>1, '\hyphenation'=>1,
     '\pagestyle'=>1,'\thispagestyle'=>1, '\pagenumbering'=>1,'\markboth'=>1, '\markright'=>1,
     '\newcommand'=>[-3,-3],'\renewcommand'=>[-3,-3],
     '\newenvironment'=>[-3,-3,-3], 'renewenvironment'=>[-3,-3,-3],
     '\newfont'=>2,'\newtheorem'=>2,'\bibliographystyle'=>1, '\bibliography'=>1,
     '\parbox'=>1, '\marginpar'=>[3],'\makebox'=>0, '\raisebox'=>1, '\framebox'=>0,
     '\newsavebox'=>1, '\sbox'=>1, '\savebox'=>2, '\usebox'=>1,'\rule'=>2,
     '\footnote'=>[3],'\label'=>1, '\ref'=>1, '\pageref'=>1, '\bibitem'=>1,
     '\cite'=>1, '\citep'=>1, '\citet'=>1, '\citeauthor'=>1, '\citealt'=>1, '\nocite'=>1,
     '\eqlabel'=>1, '\eqref'=>1,'\hspace'=>1, '\vspace'=>1, '\addvspace'=>1,
     '\input'=>1, '\include'=>1, '\includeonly'=>1,'\includegraphics'=>1,
     '\newlength'=>1, '\setlength'=>2, '\addtolength'=>2,'\settodepth'=>2,
     '\settoheight'=>2, '\settowidth'=>2,'\newcounter'=>1, '\setcounter'=>2,
     '\addtocounter'=>2,'\stepcounter'=>1, '\refstepcounter'=>1, '\usecounter'=>1,
     '\alph'=>1, '\arabic'=>1, '\fnsymbol'=>1, '\roman'=>1, '\value'=>1,
     '\cline'=>1, '\multicolumn'=>3,'\typeout'=>1, '\typein'=>1,
     'beginlist'=>2, 'beginminipage'=>1, 'begintabular'=>1,
     'beginthebibliography'=>1,'beginlrbox'=>1,
     '\begin'=>1,'\end'=>1,'\title'=>[2]);

### Macros that should be counted as one or more words
# Macros that represent text may be declared here. The value gives
# the number of words the macro represents.
my %TeXmacroword=('\LaTeX'=>1,'\TeX'=>1);

### Macros that are counted within the preamble
# The preamble is the text between \documentclass and \begin{document}.
# Text and macros in the preamble is ignored unless specified here. The
# value is the status (1=text, 2=header, etc.) they should be interpreted as.
# Note that only the first unit (token or {...} block) is counted.
my %TeXpreamble=('\title'=>[2],
     '\newcommand'=>[-3,-3],'\renewcommand'=>[-3,-3],
     '\newenvironment'=>[-3,-3,-3], 'renewenvironment'=>[-3,-3,-3],
     );

### Begin-End groups
# Identified as begin-end groups, and define =>state. The
# states used corresponds to the elements of the count array, and
# are:
#    0: Not included
#    1: Text, word included in text count
#    2: Header, words included in header count
#    3: Float caption, words included in float caption count
#    6: Inline mathematics, words not counted
#    7: Displayed mathematics, words not counted
#   -1: Float, not included, but looks for captions
#
#    4 and 5 are used to count number of headers and floats
#    and are not used as states.
#
# Groups that are not defined will be counted as the surrounding text.
#
# Note that some environments may only exist within math-mode, and
# therefore need not be defined here: in fact, they should not as it
# is not clear if they will be in inlined or displayed math.
#
my %TeXgroup=('document'=>1,'letter'=>1,'titlepage'=>0,
     'center'=>1,'flushleft'=>1,'flushright'=>1,
     'abstract'=>1,'quote'=>1,'quotation'=>1,'verse'=>1,'minipage'=>1,'verbatim'=>1,
     'description'=>1,'enumerate'=>1,'itemize'=>1,'list'=>1,
     'theorem'=>1,'lemma'=>1,'definition'=>1,'corollary'=>1,'example'=>1,
     'math'=>6,'displaymath'=>7,'equation'=>7,'eqnarray'=>7,'align'=>7,
     'figure'=>-1,'float'=>-1,'picture'=>-1,'table'=>-1,
     'tabbing'=>0,'tabular'=>0,'thebibliography'=>0,'lrbox'=>0);

### In floats: include only specific macros
# Macros used to identify caption text within floats.
my %TeXfloatinc=('\caption'=>[3]);

### Macros for including tex files
# Allows \macro{file} or \macro file. If the value is 0, the filename will
# be used as is; if it is 1, the filetype .tex will be added if the
# filename is without filetype; if it is 2, the filetype .tex will be added.
my %TeXfileinclude=('\input'=>1,'\include'=>2);

### Count labels
# Labels used to describe the counts
my @countlabel=('Files','Words in text','Words in headers',
      'Words in float captions','Number of headers','Number of floats',
      'Number of math inlines','Number of math displayed');

### Break points
# Definition of macros that define break points that start a new subcount.
# The values given are used as labels.
my %BreakPointsOptions;
$BreakPointsOptions{'none'}={};
$BreakPointsOptions{'part'}={%{$BreakPointsOptions{'none'}},'\part'=>'Part'};
$BreakPointsOptions{'chapter'}={%{$BreakPointsOptions{'part'}},'\chapter'=>'Chapter'};
$BreakPointsOptions{'section'}={%{$BreakPointsOptions{'chapter'}},'\section'=>'Section'};
$BreakPointsOptions{'subsection'}={%{$BreakPointsOptions{'section'}},'\subsection'=>'Subsection'};
$BreakPointsOptions{'default'}=$BreakPointsOptions{'subsection'};
my %BreakPoints=%{$BreakPointsOptions{'none'}};

### Print styles
# Definition of different print styles: maps of class labels
# to ANSI codes. Class labels are as used by HTML styles.
my @STYLES=();
my %STYLE;
$STYLES[0]={'error'=>'bold red'};
$STYLES[1]={%{$STYLES[0]},
            'word1'=>'blue','word2'=>'bold blue','word3'=>'blue',
            'grouping'=>'red','document'=>'red','mathgroup'=>'magenta',
            'state'=>'cyan underline','sumcount'=>'yellow'};
$STYLES[2]={%{$STYLES[1]},
            'command'=>'green','exclcommand'=>'yellow','exclgroup'=>'yellow','exclmath'=>'yellow',
            'ignore'=>'cyan'};
$STYLES[3]={%{$STYLES[2]},
            'tc'=>'bold yellow','comment'=>'yellow','option'=>'yellow',
            'fileinclude'=>'bold green'};
$STYLES[4]={%{$STYLES[3]}};

### Word regexp pattern list
# List of regexp patterns that should be analysed as words.
# Use @ to represent a letter, will be substituted with $LetterPattern.
my @WordPatterns=('(@+\.)+@+\.?','@+([\-\']@+)*');
my $specialchars='\\\\(ae|AE|o|O|aa|AA)';
my $modifiedchars='\\\\[\'\"\`\~\^\=](\w|\{\w\})';
my $LetterPattern='\w';
my $LetterPatternRelaxed='([\w\-\']|'.$modifiedchars.'|'.$specialchars.'(\{\})?|\{'.$specialchars.'\}|\{\w\})';
my %NamedWordPattern;
$NamedWordPattern{'chinese'}='\p{script=Han}';
$NamedWordPattern{'japanese'}='(\p{script=Han}|\p{script=Hiragana}|\p{script=Katakana})';

### Macro option regexp list
# List of regexp patterns to be gobbled as macro option in and after
# a macro.
my @MacroOptionPatterns=('\[(\w|[,\-\s\~\.\:\;\+\?\*\_\=])*\]');
my @MacroOptionPatternsRelaxed=('\[[^\[\]\n]*\]');

###### Main script


###################################################

MAIN(@ARGV);

###################################################


#########
######### Main routines
#########

# MAIN ROUTINE: Handle arguments, then parse files
sub MAIN {
  my @args=@_;
  my @toplevelfiles=Parse_Arguments(@args);
  Apply_Options();
  if (scalar(@toplevelfiles)==0) {
    conditional_print_help_style()
    || print_error("No files specified.","p","error");
  } else {
    conditional_print_help_style();
    my $totalcount=parse_file_list(@toplevelfiles);
    conditional_print_total($totalcount);
  }
  Report_ErrorCount();
  Close_Output();
}

# Checks arguments, exits on exit condition
sub Check_Arguments {
  my @args=@_;
  if (!@args) {
    print_version();
    print_syntax();
    print_reference();
    exit;
  } elsif ($args[0]=~/^(\-?\-(h|\?|help)|\/(\?|h))$/) {
    print_help();
    exit;
  } elsif ($args[0]=~/^\-?\-(ver|version)$/) {
    print_version();
    exit;
  } elsif ($args[0]=~/^\-?\-(lic|license)$/) {
    print_license();
    exit;
  }
  return 1;
}

# Parses arguments, sets options (global) and returns file list
sub Parse_Arguments {
  my @args=@_;
  Check_Arguments(@args);
  my @files;
  foreach my $arg (@ARGV) {
    if (Parse_Option($arg)) {next;}
    if ($arg=~/^\-/) {
      print 'Invalid opton '.$arg."\n";
      print_syntax();
      exit;
    }
    $arg=~s/\\/\//g;
    push @files,$arg;
  }
  return @files;
}

# Parse individual option parameters
sub Parse_Option {
  my $arg=shift @_;
  return parse_options_parsing($arg)
  || parse_options_sums($arg)
  || parse_options_output($arg)
  || parse_options_format($arg)
  ;
}

sub parse_options_parsing {
  my $arg=shift @_;
  if ($arg eq '-inc') {$includeTeX=1;}
  elsif ($arg eq '-noinc') {$includeTeX=0;}
  elsif ($arg eq '-dir') {$globalworkdir=undef;}
  elsif ($arg=~/^-dir=(.*)$/) {$globalworkdir=$1;}
  elsif ($arg=~/^-(utf8|unicode)$/) {$utf8flag=1;}
  elsif ($arg=~/^-(ch|chinese|zhongwen)$/) {
    $utf8flag=1;
    @WordPatterns=($NamedWordPattern{'chinese'},@WordPatterns);
  }
  elsif ($arg=~/^-(jp|japanese)$/) {
    $utf8flag=1;
    @WordPatterns=($NamedWordPattern{'japanese'},@WordPatterns);
  }
  elsif ($arg eq '-relaxed') {
    @MacroOptionPatterns=@MacroOptionPatternsRelaxed;
    $LetterPattern=$LetterPatternRelaxed;
  }
  else {return 0;}
  return 1;
}

sub parse_options_sums {
  my $arg=shift @_;
  if ($arg=~/^-sum(=(.+))?$/) {option_sum($2);}
  elsif ($arg=~/^-(sub|subcounts?)(=(.+))?$/) {option_subcount($3);}
  else {return 0;}
  return 1;
}

sub option_subcount {
  my $arg=shift @_;
  $showsubcounts=1;
  if (!defined $arg) {
    %BreakPoints=%{$BreakPointsOptions{'default'}};
  } elsif (my $option=$BreakPointsOptions{$arg}) {
    %BreakPoints=%{$option};
  } else {
    print STDERR "Warning: Option value ".$arg." not valid, using default instead.\n";
    %BreakPoints=%{$BreakPointsOptions{'default'}};
  }
}

sub option_sum {
  my $arg=shift @_;
  if (!defined $arg) {
    @sumweights=(1,1,1,0,0,1,1);
  } elsif ($arg=~/^(\d+(,\d+){0,6})$/) {
    @sumweights=split(',',$1);
  } else {
    print STDERR "Warning: Option value ".$arg." not valid, ignoring option.\n";
  }
}

sub parse_options_format {
  my $arg=shift @_;
  if ($arg eq '-brief') {$briefsum=1;}
  elsif ($arg eq '-total') {$totalflag=1;}
  elsif ($arg eq '-1') {$briefsum=1;$totalflag=1;$verbose=-1;}
  elsif ($arg eq "-html" ) {option_no_colours();$htmlstyle = 2;}
  elsif ($arg eq "-htmlcore" ) {option_no_colours();$htmlstyle = 1;}
  elsif ($arg=~/^\-(nocol|nc$)/) {option_no_colours();}
  elsif ($arg eq '-codes') {
    $showcodes=2;
    if ($verbose==0) {$verbose=3;}
  }
  elsif ($arg eq '-nocodes') {$showcodes=0;}
  else {return 0;}
  return 1;
}

sub parse_options_output {
  my $arg=shift @_;
  if ($arg eq "-v0") {$verbose=0;}
  elsif ($arg eq "-v1") {$verbose=1;}
  elsif ($arg eq '-vv' || $arg eq '-v2') {$verbose=2;}
  elsif ($arg eq '-vvv' || $arg eq '-v3' || $arg eq '-v') {$verbose=3;}
  elsif ($arg eq '-vvvv' || $arg eq '-v4') {$verbose=3; $showstates=1;}
  elsif ($arg =~ /^\-showstates?$/ ) {$showstates=1;}
  elsif ($arg =~ /^-(q|-?quiet)$/ ) {$verbose=-1;}
  else {return 0;}
  return 1;
}

# Parse file list and return total count
sub parse_file_list {
  my @filelist=@_;
  my $listtotalcount=new_count("TOTAL COUNT");
  for my $file (<@filelist>) {
    my $filetotalcount=parse_file($file);
    add_count($listtotalcount,$filetotalcount);
  }
  return $listtotalcount;
}

# Parse file and included files, and return total count
sub parse_file {
  my $file=shift @_;
  $workdir=$globalworkdir;
  if (!defined $workdir) {
    $workdir=$file;
    $workdir =~ s/^((.*[\\\/])?)[^\\\/]+$/$1/;
  }
  @filelist=($file);
  if ($htmlstyle) {print "\n<div class='filegroup'>\n";}
  my $filetotalcount=new_count("SUM COUNT FOR ".$file);
  foreach my $f (@filelist) {
    my $tex=TeXfile($f);
    my $fpath=$f;
    $fpath=~s/^((.*[\\\/])?)[^\\\/]+$/$1/;
    if (!defined $tex) {
      print STDERR "File not found or not readable: ".$f."\n";
      formatprint("File not found or not readable: ".$f."\n","p","error");
    } else {
      parse($tex);
      my $filecount=add_subcount($tex);
      if (!$totalflag) {
        print_count($filecount);
        print "\n";
      }
      add_count($filetotalcount,$filecount);
    }
  }
  if ($htmlstyle) {print "</div>\n\n";}
  return $filetotalcount;
}


######
###### Subroutines
######

###### CMD specific implementations


sub add_file_to_list {
  my $fname=shift @_;
  push @filelist,$workdir.$fname;
}

sub print_with_style {
  my ($text,$style,$colour)=@_;
  #if ($utf8flag || $htmlstyle) {utf8::encode($text);}
  if ($htmlstyle) {
    print "<span class='".$style."'>".$text."</span>";
  } else {
    print Term::ANSIColor::colored($text,$colour);
  }
}

sub option_no_colours {
  $ENV{'ANSI_COLORS_DISABLED'} = 1;
}

# Print count (total) if conditions are met
sub conditional_print_total {
  my $sumcount=shift @_;
  if ($totalflag || get_count($sumcount,0)>1) {
    if ($totalflag && $briefsum && @sumweights) {
      print total_count($sumcount);
    } else {
      if ($htmlstyle) {
        formatprint("Total word count",'h2');
      }
      print_count($sumcount);
    }
  }
}

###### Option handling


# Apply options to set values
sub Apply_Options {
  %STYLE=%{$STYLES[$verbose]};
  if ($utf8flag) {binmode STDOUT,':utf8';}
  if ($htmlstyle>1) {html_head();}
  foreach (@WordPatterns) {
    s/\@/$LetterPattern/g;
  }
}


###### TeX code handle


sub TeXfile {
  my $filename=shift @_;
  my $file=read_file($filename) || return undef;
  return TeXcode($file,$filename);
}

sub read_file {
  my $filename=shift @_;
  if ($utf8flag) {
    open(FH,"<:utf8",$filename) || return undef;
  } else {
    open(FH,"<".$filename) || return undef;
  }
  if ($verbose>0) {
    formatprint("File: ".$filename."\n",'h2');
    $blankline=0;
  }
  my @text=<FH>;
  close(FH);
  my $latexcode=join('',@text);
  if ($utf8flag) {
    $latexcode =~ s/^\x{feff}//;
  }
  return $latexcode;
}

###### Parsing routines


# Make TeXcode handle
sub TeXcode {
  my ($texcode,$filename,$title)=@_;
  my %TeX=();
  $TeX{'filename'}=$filename;
  if (!defined $filename) {
    $TeX{'filepath'}='';
  } elsif ($filename=~/^(.*[\\\/])[^\\\/]+$/) {
    $TeX{'filepath'}=$1;
  } else {
    $TeX{'filepath'}='';
  }
  if (defined $title) {}
  elsif (defined $filename) {$title="FILE: ".$filename;}
  else {$title="Word count";}
  $TeX{'line'}=$texcode;
  $TeX{'next'}=undef;
  $TeX{'type'}=undef;
  $TeX{'style'}=undef;
  $TeX{'printstate'}=undef;
  $TeX{'eof'}=0;
  my $countsum=new_count($title);
  $TeX{'countsum'}=$countsum;
  my $count=new_count("_top_");
  $TeX{'count'}=$count;
  inc_count(\%TeX,0);
  my @countlist=();
  $TeX{'countlist'}=\@countlist;
  $countsum->{'subcounts'}=\@countlist;
  return \%TeX;
}

# Parse LaTeX document
sub parse {
  my ($tex)=@_;
  if ($htmlstyle && $verbose) {print "<p class=parse>\n";}
  while (!($tex->{'eof'})) {
    parse_unit($tex,1);
  }
  if ($htmlstyle && $verbose) {print "</p>\n";}
}

# Parse one block or unit
sub parse_unit {
  # Status:
  #    0 = exclude from count
  #    1 = text
  #    2 = header text
  #    3 = float text
  #   -1 = float (exclude)
  #   -2 = strong exclude, ignore begin-end groups
  #   -3 = stronger exclude, do not parse macro parameters
  #   -9 = preamble (between \documentclass and \begin{document})
  my ($tex,$status,$end)=@_;
  if (!defined $status) {
    print_error("CRITICAL ERROR: Undefined parser status!");
    exit;
  } elsif (ref($status) eq 'ARRAY') {
    print_error("CRITICAL ERROR: Invalid parser status!");
    exit;
  }
  my $substat;
  if ($showstates) {
    if (defined $end) {
      $tex->{'printstate'}=':'.$status.':'.$end.':';
    } else {
      $tex->{'printstate'}=':'.$status.':';
    }
    flush_next($tex);
  }
  while (defined (my $next=next_token($tex))) {
    # parse next token; or tokens until match with $end
    set_style($tex,"ignore");
    if ((defined $end) && ($end eq $next)) {
      # end of unit
      return;
    } elsif (!defined $next) {
      print_error("ERROR: End of file while waiting for ".$end);
      return;
    }
    if ($status==-9 && $next eq '\begin' && $tex->{'line'}=~/^\{\s*document\s*\}/) {
      # \begin{document}
      $status=1;
    }
    if ($next eq '\documentclass') {
      # starts preamble
      set_style($tex,'document');
      gobble_option($tex);
      gobble_macro_parms($tex,1);
      while (!($tex->{'eof'})) {
       parse_unit($tex,-9);
      }
    } elsif ($tex->{'type'}==666) {
      # parse TC instructions
      parse_tc($tex);
    } elsif ($tex->{'type'}==1) {
      # word
      if ($status>0) {
        inc_count($tex,$status);
        set_style($tex,'word'.$status);
      }
    } elsif ($next eq '{') {
      # {...}
      parse_unit($tex,$status,'}');
    } elsif ($tex->{'type'}==3 && $status==-3) {
      set_style($tex,'ignore');
    } elsif ($tex->{'type'}==3) {
      # macro call
      parse_macro($tex,$next,$status,$substat);
    } elsif ($next eq '$') {
      # math inline
      parse_math($tex,$status,6,'$');
    } elsif ($next eq '$$') {
      # math display (unless already in inlined math)
      if (!(defined $end && $end eq '$')) {
        parse_math($tex,$status,7,'$$');
      }
    }
    if (!defined $end) {return;}
  }
}

sub parse_macro {
  my ($tex,$next,$status,$substat)=@_;
  if (my $label=$BreakPoints{$next}) {
    if ($tex->{'line'}=~ /^[*]?(\s*\[.*?\])*\s*\{(.+?)\}/ ) {
      $label=$label.': '.$2;
    }
    add_subcount($tex,$label);
  }
  set_style($tex,$status>0?'command':'exclcommand');
  if ($next eq '\begin' && $status!=-2) {
 	parse_begin_end($tex,$status);
  } elsif (($status==-1) && ($substat=$TeXfloatinc{$next})) {
    # text included from float
    set_style($tex,'command');
    gobble_macro_parms($tex,$substat);
  } elsif ($status==-9 && defined ($substat=$TeXpreamble{$next})) {
  	# parse preamble include macros
    set_style($tex,'command');
  	if (defined $TeXheader{$next}) {inc_count($tex,4);}
    gobble_macro_parms($tex,$substat,1);
  } elsif ($status<0) {
  	# ignore
    gobble_option($tex);
  } elsif ($next eq '\(') {
    # math inline
    parse_math($tex,$status,6,'\)');
  } elsif ($next eq '\[') {
    # math display
    parse_math($tex,$status,7,'\]');
  } elsif ($next eq '\def') {
    # ignore \def...
    $tex->{'line'} =~ s/^([^\{]*)\{/\{/;
    flush_next($tex);
    print_style($1.' ','ignore');
    parse_unit($tex,-2);
  } elsif (defined (my $addsuffix=$TeXfileinclude{$next})) {
  	# include file: queue up for parsing
  	parse_include_file($tex,$status,$addsuffix);
  } elsif (defined ($substat=$TeXmacro{$next})) {
    # macro: exclude options
  	if (defined $TeXheader{$next}) {inc_count($tex,4);}
    gobble_macro_parms($tex,$substat,$status);
  } elsif (defined ($substat=$TeXmacroword{$next})) {
  	# count macro as word (or a given number of words)
    inc_count($tex,$status,$substat);
    set_style($tex,'word'.$status);
  } elsif ($next =~ /^\\[^\w\_]/) {
  } else {
    gobble_option($tex);
  }
}

sub parse_tc {
  my ($tex)=@_;
  my $next=$tex->{'next'};
  set_style($tex,'tc');
  flush_next($tex);
  if (!($next=~s/^\%+TC:\s*(\w+)\s*// )) {
    print_error('Warning: TC command should have format %TC:instruction [macro] [parameters]');
    return;
  };
  my $instr=$1;
  if ($instr=~/^(break)$/) {
    if ($instr eq 'break') {add_subcount($tex,$next);}
  } elsif ($next=~/^([\\]*\w+)\s+([^\s\n]+)(\s+([0-9]+))?/) {
    # Format = TC:word macro
    my $macro=$1;
    my $param=$2;
    my $option=$4;
    if ($param=~/^\[([0-9,]+)\]$/) {$param=[split(',',$1)];}
    if (($instr eq 'macro') || ($instr eq 'exclude')) {$TeXmacro{$macro}=$param;}
    elsif ($instr eq 'header') {$TeXheader{$macro}=$param;$TeXmacro{$macro}=$param;}
    elsif ($instr eq 'macroword') {$TeXmacroword{$macro}=$param;}
    elsif ($instr eq 'preambleinclude') {$TeXpreamble{$macro}=$param;}
    elsif ($instr eq 'group') {
      $TeXmacro{'begin'.$macro}=$param;
      $TeXgroup{$macro}=$option;
    }
    elsif ($instr eq 'floatinclude') {$TeXfloatinc{$macro}=$param;}
    elsif ($instr eq 'fileinclude') {$TeXfileinclude{$macro}=$param;}
    elsif ($instr eq 'breakmacro') {$BreakPoints{$macro}=$param;}
    else {print_error("Warning: Unknown TC command: ".$instr);}
  } elsif ($instr eq 'ignore') {
    tc_ignore_input($tex);
  } else {
    print_error("Warning: Invalid TC command format: ".$instr);
  }
}

sub tc_ignore_input {
  my ($tex)=@_;
  set_style($tex,'ignore');
  parse_unit($tex,-3,"%TC:endignore");
  set_style($tex,'tc');
  flush_next($tex);
}

sub parse_math {
  my ($tex,$status,$substat,$end)=@_;
  my $localstyle=$status>0 ? 'mathgroup' : 'exclmath';
  if ($status>0) {inc_count($tex,$substat);}
  set_style($tex,$localstyle);
  parse_unit($tex,0,$end);
  set_style($tex,$localstyle);
}

sub parse_begin_end {
  my ($tex,$status)=@_;
  my $localstyle=$status>0 ? 'grouping' : 'exclgroup';
  flush_style($tex,$localstyle);
  gobble_option($tex);
  my $groupname;
  if ($tex->{'line'} =~ s/^\{\s*([^\{\}]+)\s*\*?\}[ \t\r\f]*//) {
    # gobble group type
    $groupname=$1;
    print_style('{'.$1.'}',$localstyle);
    my $next='begin'.$1;
    if (defined (my $substat=$TeXmacro{$next})) {
      gobble_macro_parms($tex,$substat);
    }
  } else {
    print_error("Warning: BEGIN group without type.");
  }
  # find group status (or leave unchanged)
  my $substat;
  defined ($substat=$TeXgroup{$1}) || ($substat=$status);
  if ($status<=0 && $status<$substat) {$substat=$status;}
  if (($status>0) && ($substat==-1)) {
    # Count float
    inc_count($tex,5);
  }
  if ($status>0 and $substat>3) {
    # count item, exclude contents
    inc_count($tex,$substat);
    $substat=0;
  }
  parse_unit($tex,$substat,'\end');
  if ($tex->{'line'} =~ s/^\{\s*([^\{\}]+)\s*\}[ \t\r\f]*//) {
    # gobble group type
    flush_style($tex,$localstyle);
    print_style('{'.$1.'}',$localstyle);
  } else {
    print_error("Warning: END group without type while waiting to end ".$groupname.".");
  }
}

sub parse_include_file {
  my ($tex,$status,$addsuffix)=@_;
  $tex->{'line'} =~ s/^\{([^\{\}\s]+)\}// ||
    $tex->{'line'} =~ s/^\s*([^\{\}\%\\\s]+)// ||
    return;
  flush_next($tex);
  if ($status>0) {
    print_style($&,'fileinclude');
    my $fname=$1;
    if ($addsuffix==2) {$fname.='.tex';}
    elsif ($addsuffix==1 && ($fname=~/^[^\.]+$/)) {$fname.='.tex';}
    if ($includeTeX) {add_file_to_list($fname);}
  } else {
    print_style($&,'ignored');
  }
}

sub gobble_option {
  my $tex=shift @_;
  flush_next($tex);
  foreach my $pattern (@MacroOptionPatterns) {
    if ($tex->{'line'}=~s/^($pattern)//) {
      print_style($1,'option');
      return $1;
    }
  }
  return undef;
}

sub gobble_options {
  while (gobble_option(@_)) {}
}

sub gobble_macro_modifier {
  my $tex=shift @_;
  flush_next($tex);
  if ($tex->{'line'} =~ s/^\*//) {
    print_style($1,'option');
    return $1;
  }
  return undef;
}

sub gobble_macro_parms {
  my ($tex,$parm,$oldstat)=@_;
  my $i;
  if (ref($parm) eq 'ARRAY') {
    $i=scalar @{$parm};
  } else {
    $i=$parm;
    $parm=[0,0,0,0,0,0,0,0,0];
  }
  if ($i>0) {gobble_macro_modifier($tex);}
  gobble_options($tex);
  for (my $j=0;$j<$i;$j++) {
    parse_unit($tex,new_status($parm->[$j],$oldstat));
    gobble_options($tex);
  }
}

sub new_status {
  my ($substat,$old)=@_;
  if (!defined $old) {return $substat;}
  if ($old==-3 || $substat==-3) {return -3;}
  if ($old==-2 || $substat==-2) {return -2;}
  if ($old==0 || $substat==0) {return 0;}
  if ($old==-9 || $substat==-9) {return -9;}
  if ($old>$substat) {return $old;}
  return $substat;
}

sub next_token {
  my $tex=shift @_;
  my ($next,$type);
  if (defined $tex->{'next'}) {print_style($tex->{'next'}.' ',$tex->{'style'});}
  $tex->{'style'}=undef;
  while (defined ($next=get_next_token($tex))) {
    $type=$tex->{'type'};
    if ($type==0) {
      print_style($next,'comment');
    } elsif ($type==9) {
      if ($verbose>0) {line_return(1,$tex);}
    } else {
      return $next;
    }
  }
  return $next;
}


sub get_next_token {
  # Token (or token group) category:
  #   0: comment
  #   1: word (or other forms of text or text components)
  #   2: symbol (not word, e.g. punctuation)
  #   3: macro
  #   4: curly braces {}
  #   5: brackets []
  #   6: maths
  #   9: line break in file
  #   999: end of line or blank line
  #   666: TeXcount instruction (%TC:instruction)
  my $tex=shift @_;
  my $next;
  (defined ($next=get_token($tex,'\%+TC:\s*endignore\b[^\r\n]*',666))) && return "%TC:endignore";
  (defined ($next=get_token($tex,'\%+TC:[^\r\n]*',666))) && return $next;
  (defined ($next=get_token($tex,'\%[^\r\n]*',0))) && return $next;
  (defined ($next=get_token($tex,'(\r|\n|\r\n)',9))) && return $next;
  (defined ($next=get_token($tex,'\\\\[\{\}]',2))) && return $next;
  foreach my $pattern (@WordPatterns) {
    (defined ($next=get_token($tex,$pattern,1))) && return $next;
  }
  (defined ($next=get_token($tex,'[\"\'\`:\.,\(\)\[\]!\+\-\*=/\^\_\@\<\>\~\#\&]',2))) && return $next;
  (defined ($next=get_token($tex,'\\\\([a-zA-Z_]+|[^a-zA-Z_])',3))) && return $next;
  (defined ($next=get_token($tex,'[\{\}]',4))) && return $next;
  (defined ($next=get_token($tex,'[\[\]]',5))) && return $next;
  (defined ($next=get_token($tex,'\$\$',6))) && return $next;
  (defined ($next=get_token($tex,'\$',6))) && return $next;
  (defined ($next=get_token($tex,'.',999))) && return $next;
  (defined ($next=get_token($tex,'[^\s]+',999))) && return $next;
  $tex->{'eof'}=1;
  return undef;
}

sub get_token {
  my ($tex,$regexp,$type)=@_;
  if (!defined $regexp) {print_error("ERROR in get_token: undefined regex.");}
  if (!defined $tex->{'line'}) {print_error("ERROR in get_token: undefined tex-line. ".$tex->{'next'});}
  if ( $tex->{'line'} =~ s/^($regexp)[ \t\r\f]*// ) {
    $tex->{'next'}=$1;
    $tex->{'type'}=$type;
    return $1;
  }
  return undef;
}

###### Count handling routines


sub new_count {
  my ($title)=@_;
  my @cnt=(0,0,0,0,0,0,0,0);
  my %count=('count'=>\@cnt,'title'=>$title);
  # files, text words, header words, float words,
  # headers, floats, math-inline, math-display;
  return \%count;
}

sub inc_count {
  my ($tex,$type,$value)=@_;
  my $count=$tex->{'count'};
  if (!defined $value) {$value=1;}
  ${$count->{'count'}}[$type]+=$value;
}

sub get_count {
  my ($count,$type)=@_;
  return ${$count->{'count'}}[$type];
}

sub total_count {
  my ($count)=@_;
  my $sum=0;
  for (my $i=scalar(@sumweights);$i-->0;) {
    $sum+=get_count($count,$i+1)*$sumweights[$i];
  }
  return $sum;
}

sub print_count {
  my ($count,$header)=@_;
  if ($briefsum && @sumweights) {
    print_count_total($count,$header);
  } elsif ($briefsum) {
    if ($htmlstyle) {print "<p class='briefcount'>";}
    print_count_brief($count,$header);
    if ($htmlstyle) {print "</p>\n";}
  } else {
    print_count_details($count,$header);
  }
}

sub print_count_with_header {
  my ($count,$header)=@_;
  if (!defined $header) {$header=$count->{'title'};}
  if (!defined $header) {$header="";}
  return $count,$header;
}  

sub print_count_total {
  my ($count,$header)=print_count_with_header(@_);
  if ($htmlstyle) {print "<p class='count'>".$header;}
  print total_count($count);
  if ($htmlstyle) {print "</p>\n";}
  else {print ": ".$header;}
}

sub print_count_brief {
  my ($count,$header)=print_count_with_header(@_);
  my $cnt=$count->{'count'};
  print ${$cnt}[1]."+".${$cnt}[2]."+".${$cnt}[3].
      " (".${$cnt}[4]."/".${$cnt}[5]."/".${$cnt}[6]."/".${$cnt}[7].") ".
      $header;
}

sub print_count_details {
  my ($count,$header)=print_count_with_header(@_);
  if ($htmlstyle) {print "<dl class='count'>\n";}
  if (defined $header) {
    formatprint($header."\n",'dt','header');
  }
  if (get_count($count,0)>1) {
    formatprint($countlabel[0].': ','dt');
    formatprint(get_count($count,0)."\n",'dd');
  }
  if (@sumweights) {
    formatprint('Sum count: ','dt');
    formatprint(total_count($count)."\n",'dd');
  }
  for (my $i=1;$i<8;$i++) {
    formatprint($countlabel[$i].': ','dt');
    formatprint(get_count($count,$i)."\n",'dd');
  }
  my $subcounts=$count->{'subcounts'};
  if ($showsubcounts && defined $subcounts && scalar(@{$subcounts})>1) {
    formatprint("Subcounts: text+headers+captions (#headers/#floats/#inlines/#displayed)\n",'dt');
    foreach my $subcount (@{$subcounts}) {
      if ($htmlstyle) {print "<dd class='briefcount'>";}
      print_count_brief($subcount);
      if ($htmlstyle) {print "</dd>";}
      print "\n";
    }
  }
  if ($htmlstyle) {print "</dl>\n";}
}

sub add_count {
  my ($a,$b)=@_;
  for (my $i=0;$i<8;$i++) {
   ${$a->{'count'}}[$i]+=${$b->{'count'}}[$i];
  }
}

sub add_subcount {
  my ($tex,$title)=@_;
  add_count($tex->{'countsum'},$tex->{'count'});
  push @{$tex->{'countlist'}},$tex->{'count'};
  $tex->{'count'}=new_count($title);
  return $tex->{'countsum'};
}

###### Printing routines


sub set_style {
  my ($tex,$style)=@_;
  if (!(($tex->{'style'}) && ($tex->{'style'} eq '-'))) {$tex->{'style'}=$style;}
}

sub flush_style {
  my ($tex,$style)=@_;
  set_style($tex,$style);
  flush_next($tex);
}

sub line_return {
  my ($blank,$tex)=@_;
  if ($blank>$blankline) {
    if ((defined $tex) && @sumweights) {
      my $num=total_count($tex->{'count'});
      print_style(" [".$num."]","sumcount");
    }
    linebreak();
    $blankline++;
  }
}

sub linebreak {
  if ($htmlstyle) {print "<br>\n";} else {print "\n";}
}

sub print_style {
  my ($text,$style,$state)=@_;
  (($verbose>=0) && (defined $text) && (defined $style)) || return 0;
  my $colour;
  ($colour=$STYLE{$style}) || return;
  if (($colour) && !($colour eq '-')) {
    print_with_style($text,$style,$colour);
    if ($state) {
      print_style($state,'state');
    }
    $blankline=-1;
    return 1;
  } else {
    return 0;
  }
}

sub print_error {
  my $text=shift @_;
  $errorcount++;
  if ($verbose>=0) {
    line_return(1);
    print_style("!!!  ".$text."  !!!",'error');
    line_return(1);
  }
}

sub formatprint {
  my ($text,$tag,$class)=@_;
  my $break=($text=~s/\n$//);
  if ($htmlstyle && defined $tag) {
    print '<'.$tag;
    if ($class) {print " class='".$class."'";}
    print '>'.$text.'</'.$tag.'>';
  } else {
    print $text;
  }
  if ($break) {print "\n";}
}

sub flush_next {
  my $tex=shift @_;
  if (defined $tex->{'next'}) {
    print_style($tex->{'next'}.' ',$tex->{'style'},$tex->{'printstate'});
  }
  $tex->{'printstate'}=undef;
  $tex->{'style'}='-';
}


# Close the output, e.g. adding HTML tail
sub Close_Output {
  if ($htmlstyle>1) {
    html_tail();
  }
}


# Report if there were any errors occurring during parsing
sub Report_ErrorCount {
  if ($errorcount==0) {return;}
  if ($briefsum && $totalflag) {print " ";}
  if ($htmlstyle) {
    print_error("Errors:".$errorcount,"p","error");
  } else {
    print "(errors:".$errorcount.")";
  }
}


sub print_help_style {
  if ($verbose<=0) {return;}
  formatprint("Format/colour codes of verbose output:","h2");
  print "\n\n";
  if ($htmlstyle) {print "<p class='stylehelp'>";}
  help_style_line('Text which is counted',"word1","counted as text words");
  help_style_line('Header and title text',"word2","counted as header words");
  help_style_line('Caption text and footnotes',"word3","counted as caption words");
  help_style_line("Ignored text or code","ignore","excluded or ignored");
  help_style_line('\documentclass',"document","document start, beginning of preamble");
  help_style_line('\macro',"command","macro not counted, but parameters may be");
  help_style_line('\macro',"exclcommand","macro in excluded region");
  help_style_line("[Macro options]","option","not counted");
  help_style_line('\begin{group}  \end{group}',"grouping","begin/end group");
  help_style_line('\begin{group}  \end{group}',"exclgroup","begin/end group in excluded region");
  help_style_line('$  $',"mathgroup","counted as one equation");
  help_style_line('$  $',"exclmath","equation in excluded region");
  help_style_line('% Comments',"comment","not counted");
  help_style_line('%TC:TeXcount instructions',"tc","not counted");
  help_style_line("File to include","fileinclude","not counted but file may be counted later");
  if ($showstates) {
    help_style_line('[state]',"state","internal TeXcount state");
  }
  if (@sumweights) {
    help_style_line('[sumcount]',"sumcount","cumulative sum count");
  }
  help_style_line("ERROR","error","TeXcount error message");
  if ($htmlstyle) {print "</p>";}
  print "\n\n";
}

sub help_style_line {
  my ($text,$style,$comment)=@_;
  if ($htmlstyle) {
    $comment="&nbsp;&nbsp;....&nbsp;&nbsp;".$comment;
  } else {
    $comment=" .... ".$comment;
  }
  if (print_style($text,$style)) {
    print $comment;
    linebreak();
  }
}

# Print output style codes if conditions are met
sub conditional_print_help_style {
  if ($showcodes) {print_help_style();}
  return $showcodes;
}

###### HTML routines



sub html_head {
  print "<html>\n<head>";
  if ($utf8flag) {
    print "\n<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">";
  }
  print '
<style>
<!--
body {width:auto;padding:5;margin:5;}
.error {font-weight:bold;color:#f00;font-style:italic;}
.word1,.word2,.word3 {color: #009;}
.word2 {font-weight: 700;}
.word3 {font-style: italic;}
.command {color: #c00;}
.exclcommand {color: #f99;}
.option {color: #cc0;}
.grouping, .document {color: #900; font-weight:bold;}
.mathgroup {color: #090;}
.exclmath {color: #6c6;}
.ignore {color: #999;}
.exclgroup {color:#c66;}
.tc {color: #999; font-weight:bold;}
.comment {color: #999; font-style: italic;}
.state {color: #990; font-size: 70%;}
.sumcount {color: #999; font-size: 80%;}
.fileinclude {color: #696; font-weight:bold;}
dl.count {background: #cfc; color: 009;}
dl.count dt.header {font-weight: bold; font-style: italic; float: none;}
dl.count dt {clear: both; float: left; margin-right: .5em;}
dl.count dd {font-weight: bold;}
dl.count dd.briefcount {font-weight: 700; clear: both; font-size:80%; font-weight:normal; margin-left:8pt;}
.warning {color: #c00; font-weight: 700;}
.parse, .count, .stylehelp, .filegroup {border: solid 1px #999; margin: 0pt; padding: 4pt;}
.parse {font-size: 80%; background: #eef;}
.parse {border-bottom:none;}
.stylehelp {font-size: 80%; background: #ffc; margin-bottom: 8pt;}
.filegroup {background: #efe; margin-bottom: 8pt;}
-->
</style>
</head>
<body>
<h1>LaTeX word count</h1>
';
}

sub html_tail {
  print '</body></html>';
}

###### Help routines



sub print_version {
  print "TeXcount version ".$versionnumber.", ".$versiondate.'.';
}

sub print_syntax {
  print '
Syntax: TeXcount.pl [options] files

Options:
  -relaxed      Uses relaxed rules for word and option handling:
                i.e. allows more general cases to be counted as
                either words or macros.
  -v            Verbose (same as -v3)
  -v0           Do not present parsing details
  -v1           Verbose: print parsed words, mark formulae
  -v2           More verbose: also print ignored text
  -v3           Even more verbose: include comments and options
  -v4           Same as -v3 -showstate
  -showstate    Show internal states (with verbose)
  -brief        Only prints a brief, one line summary of counts
  -q, -quiet    Quiet mode, no error messages (use is discouraged!)
  -sum, -sum=   Make sum of all word and equation counts. May also
                use -sum=#[,#] with up to 7 numbers to indicate how
                each of the counts (text words, header words, caption
                words, #headers, #floats, #inlined formulae,
                #displayed formulae) are summed. The default sum (if
                only -sum is used) is the same as -sum=1,1,1,0,0,1,1.
  -sub, -sub=   Generate subcounts. Option values are none, part,
                chapter, section or subsection. Default (-sub) is set
                to subsection, whereas unset is none. (Alternative
                option name is -subcount.)
  -nc, -nocol   No colours (colours require ANSI)
  -html         Output in HTML format
  -htmlcore     Only HTML body contents
  -inc          Include tex files included in the document
  -noinc        Do not include included tex files (default)
  -total        Do not give sums per file, only total sum.
  -1            Same as -brief and -total. Ensures there is only one
                line of output. If used in conjunction with -sum, the
                output will only be the total number. (NB: Character
                is the number one, not the letter L.)
  -dir, -dir=   Specify the working directory using -dir=path.
                Remember that the path must end with \ or /. If only
                -dir is used, the directory of the parent file is used.
  -utf8, -unicode  Turns on Unicode (UTF-8) for input and output. This
                is automatic with -chinese, and is required to handle
                e.g. Korean text. Note that the TeX file must be save
                in UTF-8 format (not e.g. GB2312 or Big5), or the
                result will be unpredictable.
  -ch, -chinese, -zhongwen  Turns on support for Chinese characters.
                TeXcount will then count each Chinese character as a
                word. Automatically turns on -utf8.
  -jp, -japanese Turns on support for Japanese characters. TeXcount
                will count each Japanese character (kanji, hiragana,
                and katakana) as one word, i.e. not do any form of
                word segmentation. Automatically turns on -utf8.
  -codes        Display output style code overview and explanation.
                This is on by default.
  -nocodes      Do not display output style code overview. 
  -h, -?, --help, /?   Help
  --version     Print version number
  --license     License information
';
}

sub print_help {
  print '
***************************************************************
*   TeXcount.pl '.$versionnumber.', '.$versiondate.'
*

Count words in TeX and LaTeX files, ignoring macros, tables,
formulae, etc.
';
  print_syntax();
  print '
The script counts words as either words in the text, words in
headers/titles or words in floats (figure/table captions).
Macro options (i.e. \marco[...]) are ignored; macro parameters
(i.e. \macro{...}) are counted or ignored depending on the
macro, but by default counted. Begin-end groups are by default
ignored and treated as \'floats\', though some (e.g. center) are
counted.

Unless -nocol (or -nc) has been specified, the output will be
colour coded. Counted text is coloured blue with headers are in
bold and in HTML output caption text is italicised.

Mathematical formulae are not counted as words, but are instead
counted separately with separate counts for inlined formulae
and displayed formulae. Similarly, the number of headers and
the number of \'floats\' are counted. Note that \'float\' is used
here to describe anything defined in a begin-end group unless
explicitly recognized as text or mathematics.

The verbose options (-v1, -v2, -v3, showstate) produces output
indicating how the text has been interpreted. Check this to
ensure that words in the text has been interpreted as such,
whereas mathematical formulae and text/non-text in begin-end
groups have been correctly interpreted.

Parsing instructions may be passed to TeXcount using comments
in the LaTeX files on the format
  %TC:instruction arguments
where valid instructions for setting parsing rules, typically
set at the start of the document (applies globally), are:
  %TC:macro [macro] [param.states]
        macro handling rule, no. of and rules for parameters
  %TC:macroword [macro] [number]
        macro counted as a given number of words
  %TC:header [macro] [param.states]
        header macro rule, as macro but counts as one header
  %TC:breakmacro [macro] [label]
        macro causing subcount break point
  %TC:group [name] [parsing-state]
        begin-end-group handling rule
  %TC:floatinclude [macro] [param.states]
        as macro, but also counted inside floats
  %TC:preambleinclude [macro] [param.states]
        as macro, but also counted inside the preamble
  %TC:fileinclue [macro] [rule]
        file include, add .tex if rule=2, not if rule=0
The [param.states] is used to indicate the number of parameters
used by the macro and the rules of handling each of these: format
is [#,#,...,#] with one number for each parameter, and main rules
are 0 to ignore and 1 to count as text. Parsing instructions
which may be used anywhere are:
  %TC:ignore           start block to ignore
  %TC:endignore        end block to ignore
  %TC:break [title]    add subcount break point here
See the documentation for more details.

Unix hint: Use \'less -r\' instead of just \'less\' to view output:
the \'-r\' option makes less treat text formating codes properly.

Windows hint: If your Windows interprets ANSI colour codes, lucky
you! Otherwise, use the -nocol (or -nc) option with the verbose
options or the output will be riddled with colour codes. Instead,
you can use -html to produce HTML code, write this to file and
view with your favourite browser.
';
  print_reference();
}

sub print_reference {
  print '
The TeXcount script is copyright of Einar Andreas Rødland (2008)
and published under the LaTeX Project Public License.

For more information about the script, e.g. news, updates, help,
usage tips, known issues and short-comings, go to
    http://folk.uio.no/einarro/Comp/texwordcount.html
or go to
    http://folk.uio.no/einarro/Services/texcount.html
to access the script as a web service. Feedback such as problems
or errors can be reported to einarro@ifi.uio.no.
';
}

sub print_license {
  print 'TeXcount version '.$versionnumber.'
  
Copyright 2008 Einar Andreas Rødland

The TeXcount script is published under the LaTeX Project Public
License (LPPL)
    http://www.latex-project.org/lppl.txt
which grants you, the user, the right to use, modify and distribute
the script. However, if the script is modified, you must change its
name or use other technical means to avoid confusion.

The script has LPPL status "maintained" with Einar Andreas
Rødland being the current maintainer.
';
}

