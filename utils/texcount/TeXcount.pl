#! /local/bin/perl -w
use strict;
use Term::ANSIColor;
use POSIX qw(locale_h);
use locale;
setlocale(LC_CTYPE,"no_NO");

my $versionnumber="1.9";
my $versiondate="2008 Jan 10";


### How many tokens to exclude from count after macro
# Macro name and first N tokens (or {...}) are
# ignored. Options [...] right after macro name are
# also ignored, as are options [...] between and after
# the excluded tokens.
#
# For macros not specified here, the default value is 0: i.e.
# no tokens are excluded, but [...] options are. Some macros
# have been included here with =>0 for completeness and as a
# reminder.
my %TeXexcl=('\documentclass'=>1,'\documentstyle'=>1,'\usepackage'=>1, '\hyphenation'=>1,
     '\pagestyle'=>1,'\thispagestyle'=>1, '\pagenumbering'=>1,'\markboth'=>1, '\markright'=>1,
     '\newcommand'=>2,'\renewcommand'=>2,'\newenvironment'=>3, 'renewenvironment'=>3,
     '\newfont'=>2,'\newtheorem'=>2,'\bibliographystyle'=>1, '\bibliography'=>1,
     '\parbox'=>1, '\marginpar'=>0,'\makebox'=>0, '\raisebox'=>1, '\framebox'=>0,
     '\newsavebox'=>1, '\sbox'=>1, '\savebox'=>2, '\usebox'=>1,'\rule'=>2,
     '\footnote'=>1,'\label'=>1, '\ref'=>1, '\pageref'=>1, '\bibitem'=>1,
     '\cite'=>1, '\citep'=>1, '\citet'=>1, '\citeauthor'=>1, '\citealt'=>1, '\nocite'=>1,
     '\eqlabel'=>1, '\eqref'=>1,'\hspace'=>1, '\vspace'=>1, '\addvspace'=>1,
     '\input'=>1, '\include'=>1, '\includeonly'=>1,'\includegraphics'=>1,
     '\newlength'=>1, '\setlength'=>2, '\addtolength'=>2,'\settodepth'=>2,
     '\settoheight'=>2, '\settowidth'=>2,'\newcounter'=>1, '\setcounter'=>2,
     '\addtocounter'=>2,'\stepcounter'=>1, '\refstepcounter'=>1, '\usecounter'=>1,
     '\alph'=>1, '\arabic'=>1, '\fnsymbol'=>1, '\roman'=>1, '\value'=>1,
     '\cline'=>1, '\multicolumn'=>3,'\typeout'=>1, '\typein'=>1,
     'beginlist'=>2, 'beginminipage'=>1, 'begintabular'=>1,
     'beginthebibliography'=>1,'beginlrbox'=>1 );

### Macros that should be counted as one or more words
# Macros that represent text may be declared here. The value gives
# the number of words the macro represents.
my %TeXmacroword=('\LaTeX'=>1);

### Macros for headers
# Macros that identify headers: i.e. following token or
# {...} is counted as header. The =>2 indicates transition to
# state 2 which is used within headers.
my %TeXheader=('\title'=>2,'\part'=>2,'\chapter'=>2,
               '\section'=>2,'\subsection'=>2,'\subsubsection'=>2,
               '\paragraph'=>2,'\subparagraph'=>2);

### Macros that are counted within the preamble
# The preamble is the text between \documentclass and \begin{document}.
# Text and macros in the preamble is ignored unless specified here. The
# value is the status (1=text, 2=header, etc.) they should be interpreted as.
# Note that only the first unit (token or {...} block) is counted.
my %TeXpreamble=('\title'=>2);

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
# By default, groups not specified are parsed as floats (-1) and only
# caption text is counted, and counted as float caption rather than
# as words in text.
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
     'math'=>6,'displaymath'=>7,'equation'=>7,'eqnarray'=>7,
     'figure'=>-1,'float'=>-1,'picture'=>-1,'table'=>-1,
     'tabbing'=>0,'tabular'=>0,'thebibliography'=>0,'lrbox'=>0);

### In floats: include only specific macros
# Macros used to identify caption text within floats.
my %TeXfloatinc=('\caption'=>1);

### Count labels
# Labels used to describe the counts
my @countlabel=('Files','Words in text','Words in headers',
      'Words in float captions','Number of headers','Number of floats',
      'Number of math inlines','Number of math displayed');

# Styles
my @STYLES=();
my %STYLE;
$STYLES[0]={};
$STYLES[1]={'word1'=>'blue','word2'=>'bold blue','word3'=>'blue',
            'grouping'=>'red','mathgroup'=>'magenta','state'=>'cyan underline'};
$STYLES[2]={%{$STYLES[1]},
            'word0'=>'yellow','word-1'=>'yellow',
            'command'=>'green','exclgroup'=>'yellow','exclmath'=>'yellow',
            'ignore'=>'cyan'};
$STYLES[3]={%{$STYLES[2]},
            'comment'=>'yellow','option'=>'yellow'};
$STYLES[4]={%{$STYLES[3]}};

# Options and states
my $verbose=0;
my $showstates=0;
my $htmlstyle=0;
my $blankline=0;


## SHORT HELP AND INFO
if (!defined @ARGV) {
  print_version();
  print_syntax();
  print_reference();
  exit;
} elsif ($ARGV[0]=~/^(\-(h|\?|\-help)|\/\?)$/) {
  print_help();
  exit;
} elsif ($ARGV[0]=~/^\-\-version$/) {
  print_version();
  exit;
} elsif ($ARGV[0]=~/^\-\-licence$/) {
  print_licence();
  exit;
}

# PROCESS ARGUMENTS AND FILES
my $totalcount=new_count();
foreach my $file (<@ARGV>) {
  if ($file eq '-v') {$verbose=1; next;}
  if ($file eq '-vv') {$verbose=2; next;}
  if ($file eq '-vvv') {$verbose=3; next;}
  if ($file eq '-vvvv') {$verbose=3; $showstates=1; next;}
  if ($file =~ /^\-showstate/ ){$showstates=1; next;}
  if ($file =~ /^\-html/ ) {
    $ENV{'ANSI_COLORS_DISABLED'} = 1;
    $htmlstyle = $file eq '-htmlcore' ? 1 : 2;
    if ($htmlstyle>1) {html_head();}
    next;
  }
  if ($file=~/^\-(nocol|nc$)/) {
    $ENV{'ANSI_COLORS_DISABLED'} = 1;
    next;
  }
  %STYLE=%{$STYLES[$verbose]};
  my $tex=TeXfile($file);
  if (!defined $tex) {
    print "File not found or not readable: ".$file."\n";
    next;
  }
  parse($tex);
  print "\n";
  print_count($tex->{'count'},'FILE: '.$file);
  print "\n";
  add_count($totalcount,$tex->{'count'});
}
if (${$totalcount}[0]>1) {
  formatprint('Total','h1');
  print_count($totalcount,'SUM');
}
if ($htmlstyle>1) {
  html_tail();
}



#########
######### Subroutines
#########

sub TeXfile {
  my $filename=shift @_;
  my $file=read_file($filename) || return undef;
  return TeXcode($file);
}

sub TeXcode {
  my $texcode=shift @_;
  my %TeX=();
  $TeX{'line'}=$texcode;
  $TeX{'next'}=undef;
  $TeX{'type'}=undef;
  $TeX{'style'}=undef;
  $TeX{'printstate'}=undef;
  $TeX{'eof'}=0;
  $TeX{'count'}=new_count();
  $TeX{'count'}[0]++;
  return \%TeX;
}

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
  my $blank=shift @_;
  if ($blank>$blankline) {
    if ($htmlstyle) {print "<br>\n";} else {print "\n";}
    $blankline++;
  }
}

sub print_style {
  my ($text,$style,$state)=@_;
  (($verbose>0) && (defined $text) && (defined $style)) || return;
  my $colour;
  ($colour=$STYLE{$style}) || return;
  if (($colour) && !($colour eq '-')) {
    if ($htmlstyle) {
      print "<span class='".$style."'>".$text."</span>";
    } else {
      print Term::ANSIColor::colored($text,$colour);
      #print $text;
    }
    if ($state) {
      print_style($state,'state');
    }
    $blankline=-1;
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

sub read_file {
  my $filename=shift @_;
  open(FH,"<".$filename."") || return undef;
  if ($verbose) {
    #line_return(1);
    formatprint("File: ".$filename."\n",'h2');
    $blankline=0;
  }
  my @text=();
  while (my $line=<FH>) {
    push @text,$line;
  }
  close(FH);
  return join('',@text);
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
      if ($verbose) {line_return(1);}
    } else {
      return $next;
    }
  }
  return $next;
}

sub flush_next {
  my $tex=shift @_;
  if (defined $tex->{'next'}) {
    print_style($tex->{'next'}.' ',$tex->{'style'},$tex->{'printstate'});
  }
  $tex->{'printstate'}=undef;
  $tex->{'style'}='-';
}

sub get_next_token {
  my $tex=shift @_;
  my $next;
  (defined ($next=get_token($tex,'\%[^\n]*',0))) && return $next;
  (defined ($next=get_token($tex,'\n',9))) && return $next;
  (defined ($next=get_token($tex,'\\\\[\{\}]',1))) && return $next;
  (defined ($next=get_token($tex,'\w+(\.\w+)+\.',1))) && return $next;
  (defined ($next=get_token($tex,'\w+([\-\']\w+)*',1))) && return $next;
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
  if (!defined $regexp) {print "ERROR in get_token: undefined regex.";}
  if (!defined $tex->{'line'}) {print "ERROR in get_token: undefined tex-line. ".$tex->{'next'}."\n";}
  if ( $tex->{'line'} =~ s/^($regexp)[ \t\r\f]*// ) {
    $tex->{'next'}=$1;
    $tex->{'type'}=$type;
    return $1;
  }
  return undef;
}

sub new_count {
  my @count=(0,0,0,0,0,0,0,0);
  # files, text words, header words, float words,
  # headers, floats, math-inline, math-display;
  return \@count;
}

sub count_word {
  my ($count,$type,$word,$style,$verb)=@_;
  ($word) || ($word="");
  ($style) || ($style=0);
  ($verb) || ($verb=0);
  if ($type>0) {${$count}[$type]++;}
  if ($verb >= $verbose) {
  }
}

sub print_count {
  my ($count,$header)=@_;
  if ($htmlstyle) {print "<dl class='count'>\n";}
  if (defined $header) {
    formatprint($header."\n",'dt','header');
  }
  for (my $i=1;$i<8;$i++) {
    formatprint($countlabel[$i].': ','dt');
    formatprint(${$count}[$i]."\n",'dd');
  }
  if ($htmlstyle) {print "</dl>\n";}
}

sub add_count {
  my ($a,$b)=@_;
  for (my $i=0;$i<8;$i++) {
   ${$a}[$i]+=${$b}[$i];
  }
}

sub parse {
  my ($tex)=@_;
  if ($htmlstyle && $verbose) {print "<p class=parse>\n";}
  while (!($tex->{'eof'})) {
    parse_unit($tex,1);
  }
  if ($htmlstyle && $verbose) {print "</p>\n";}
}

sub parse_unit {
  # Status:
  #    0 = exclude
  #    1 = text
  #    2 = header text
  #    3 = float text
  #   -1 = float (exclude)
  #   -9 = preamble (between \documentclass and \begin{document})
  my ($tex,$status,$end)=@_;
  my $count=$tex->{'count'};
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
    if (($end) && ($end eq $next)) {
      # end of unit
      return;
    } elsif (!defined $next) {
      exit "ERROR: End of file while waiting for ".$end."\n";
    }
    if ($tex->{'type'}==1) {
      # word
      if ($status>0) {
        ${$count}[$status]++;
        set_style($tex,'word'.$status);
      }
    } elsif ($tex->{'type'}==9) {
      #print "\n";
    } elsif ($next eq '{') {
      # {...}
      parse_unit($tex,$status,'}');
    } elsif ($tex->{'type'}==3) {
      # macro call
      set_style($tex,'command');
      if ($next eq '\documentclass') {
      	# starts preamble
      	gobble_option($tex);
      	gobble_macro_parms($tex,1);
      	while (!($tex->{'eof'})) {
      	  parse_unit($tex,-9);
      	}
      } elsif ($next eq '\begin') {
        # \begin...\end
        if ($status==-9 && $tex->{'line'}=~/^\{document\}/) {
          $status=1;
        }
        my $localstyle=$status>0 ? 'grouping' : 'exclgroup';
        flush_style($tex,$localstyle);
        gobble_option($tex);
        if ($tex->{'line'} =~ s/^\{(\w+)\*?\}[ \t\r\f]*//) {
          # gobble group type
          print_style('{'.$1.'}',$localstyle);
          $next='begin'.$1;
          if (defined (my $i=$TeXexcl{$next})) {
          	gobble_macro_parms($tex,$i);
          }
        } else {
          print "\nWarning: BEGIN group without type.\n";
        }
        # find group status (or leave unchanged)
        defined ($substat=$TeXgroup{$1}) || ($substat=$status);
        if ($status<=0 && $status<$substat) {$substat=$status;}
        if (($status>0) && ($substat==-1)) {
          # Count float
          ${$count}[5]++;
	}
        if ($status>0 and $substat>3) {
          # count item, exclude contents
          ${$count}[$substat]++;
          $substat=0;
        }
        parse_unit($tex,$substat,'\end');
        if ($tex->{'line'} =~ s/^\{(\w+)\}[ \t\r\f]*//) {
          # gobble group type
          flush_style($tex,$localstyle);
          print_style('{'.$1.'}',$localstyle);
        } else {
          print "\nWarning: END group without type.\n";
        }
      } elsif ($next eq '\(') {
        # math inline
        my $localstyle=$status>0 ? 'mathgroup' : 'exclmath';
        if ($status>0) {${$count}[6]++;}
        set_style($tex,$localstyle);
        parse_unit($tex,0,'\)');
        set_style($tex,$localstyle);
      } elsif ($next eq '\[') {
        # math display
        my $localstyle=$status>0 ? 'mathgroup' : 'exclmath';
        if ($status>0) {${$count}[7]++;}
        set_style($tex,$localstyle);
        parse_unit($tex,0,'\]');
        set_style($tex,$localstyle);
      } elsif (defined (my $i=$TeXexcl{$next})) {
        # macro: exclude options
        gobble_macro_parms($tex,$i);
      } elsif (defined ($i=$TeXmacroword{$next})) {
      	# count macro as word (or a given number of words)
        if ($status>0) {
          ${$count}[$status]+=$i;
          set_style($tex,'word'.$status);
        }
      } elsif ($next eq '\def') {
        # ignore \def...
        $tex->{'line'} =~ s/^([^\{]*)\{/\{/;
        flush_next($tex);
        print_style($1.' ','ignore');
        parse_unit($tex,0);
      } elsif ($next =~ /^\\[^\w\_]/) {
      } else {
        gobble_option($tex);
      }
      if (($status>0 || ($status==-9 && defined $TeXpreamble{$next})) && ($substat=$TeXheader{$next})) {
        # headers
        ${$count}[4]++;
        gobble_macro_modifier($tex);
        gobble_option($tex);
        parse_unit($tex,$substat);
      } elsif ($status==-9 && defined ($substat=$TeXpreamble{$next})) {
        parse_unit($tex,$substat);
      }
      if (($status==-1) && ($substat=$TeXfloatinc{$next})) {
        # text included from float
        parse_unit($tex,3);
      }
    } elsif ($next eq '$') {
      # math inline
      my $localstyle=$status>0 ? 'mathgroup' : 'exclmath';
      if ($status>0) {${$count}[6]++;}
      set_style($tex,$localstyle);
      parse_unit($tex,0,$next);
      set_style($tex,$localstyle);
    } elsif ($next eq '$$') {
      # math display
      my $localstyle=$status>0 ? 'mathgroup' : 'exclmath';
      if ($status>0) {${$count}[7]++;}
      set_style($tex,$localstyle);
      parse_unit($tex,0,$next);
      set_style($tex,$localstyle);
    }
    if (!defined $end) {return;}
  }
}

sub gobble_option {
  my $tex=shift @_;
  flush_next($tex);
  if ($tex->{'line'} =~ s/^(\[(\w|[,\-\s\~\.\:\;\+\?\*\_\=])*\])//) {
    print_style($1,'option');
    return $1;
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
  my ($tex,$i)=@_;
  if ($i>0) {gobble_macro_modifier($tex);}
  gobble_options($tex);
  for (;$i-->0;) {
    parse_unit($tex,0);
    gobble_options($tex);
  }
}


### HTML context

sub html_head {
  print '
<html>
<head>
<style>
<!--
.parse {font-size: 80%; background: #eef;}
.word1,.word2,.word3 {color: #009;}
.word2 {font-weight: 700;}
.word3 {font-style: italic;}
.word-1 {color: #66c;}
.command {color: #c00;}
.option {color: #cc0;}
.grouping {color: #900; font-weight: 700;}
.mathgroup {color: #090;}
.exclmath {color: #6c6;}
.ignore {color: #999;}
.comment {color: #999; font-style: italic;}
.state {color: #990; font-size: 70%;}
dl.count {width: 100%; background: #cfc; color: 009;}
dl.count dt.header {font-weight: 700; font-style: italic; float: none;}
dl.count dt {clear: both; float: left;}
dl.count dd {font-weight: 700;}
.warning {color: #c00; font-weight: 700;}
.parse, .count {border: solid 1px #999; margin: 0pt; padding: 4pt;}
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


### HELP AND INFO

sub print_version {
  print "TeXcount version ".$versionnumber.", ".$versiondate.'.';
}

sub print_syntax {
  print '
Syntax: TeXcount.pl [options] files

Options:
  -v            Verbose: print parsed words, mark formulae
  -vv           More verbose: also print ignored text
  -vvv          Even more verbose: include comments and options
  -showstate    Show internal states (with verbose)
  -nc, -nocol   No colours (colours require ANSI)
  -html         Output in HTML format
  -htmlcore     Only HTML body contents
  -h, -?, --help, /?   Help
  --version     Print version number
  --licence     Licence information
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

The verbose options (-v, -vv, -vvv, showstate) produces output
indicating how the text has been interpreted. Check this to
ensure that words in the text has been interpreted as such,
whereas mathematical formulae and text/non-text in begin-end
groups have been correctly interpreted.

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
and published under the LaTeX Project Public Licence.

For more information about the script, e.g. news, updates, help,
usage tips, known issues and short-comings, go to
    http://folk.uio.no/einarro/Comp/texwordcount.html
or go to
    http://folk.uio.no/einarro/Services/texcount.html
to access the script as a web service. Feedback such as problems
or errors can be reported to einarro@ifi.uio.no.
';
}

sub print_licence {
  print 'TeXcount version '.$versionnumber.'
  
Copyright 2008 Einar Andreas Rødland

The TeXcount script is published under the LaTeX Project Public
Licence (LPPL)
    http://www.latex-project.org/lppl.txt
which grants you, the user, the right to use, modify and distribute
the script. However, if the script is modified, you must change its
name or use other technical means to avoid confusion.

The script has LPPL status "maintained" with Einar Andreas
Rødland being the current maintainer.
';
}
