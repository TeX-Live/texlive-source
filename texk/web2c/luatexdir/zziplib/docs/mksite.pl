#! /usr/bin/perl
# this is the perl variant of the mksite script. It based directly on a
# copy of mksite.sh which is derived from snippets that I was using to 
# finish doc pages for website publishing. Using only sh/sed along with
# files has a great disadvantage: it is very slow process atleast. The
# perl language in contrast has highly optimized string, replace, search
# functions as well as data structures to store intermediate values. As
# an advantage large parts of the syntax are similar to the sh/sed variant.
#
#                                               http://zziplib.sf.net/mksite/
#   THE MKSITE.SH (ZLIB/LIBPNG) LICENSE
#       Copyright (c) 2004 Guido Draheim <guidod@gmx.de>
#   This software is provided 'as-is', without any express or implied warranty
#       In no event will the authors be held liable for any damages arising
#       from the use of this software.
#   Permission is granted to anyone to use this software for any purpose, 
#       including commercial applications, and to alter it and redistribute it 
#       freely, subject to the following restrictions:
#    1. The origin of this software must not be misrepresented; you must not
#       claim that you wrote the original software. If you use this software 
#       in a product, an acknowledgment in the product documentation would be 
#       appreciated but is not required.
#    2. Altered source versions must be plainly marked as such, and must not
#       be misrepresented as being the original software.
#    3. This notice may not be removed or altered from any source distribution.
# $Id: mksite.pl,v 1.1 2005/12/13 00:27:06 guidod Exp $

use strict; use warnings; no warnings "uninitialized";
use File::Basename qw(basename);
use POSIX qw(strftime);

# initialize some defaults
my $SITEFILE="";
$SITEFILE="site.htm"  if not $SITEFILE and -f "site.htm";
$SITEFILE="site.html" if not $SITEFILE and -f "site.html";
$SITEFILE="site.htm"  if not $SITEFILE;
# my $MK="-mksite";     # note the "-" at the start
my $SED="sed";

my $INFO="~~";     # extension for meta data files
my $HEAD="~head~"; # extension for head sed script
my $BODY="~body~"; # extension for body sed script
my $FOOT="~foot~"; # append to body text (non sed)

my $SED_LONGSCRIPT="$SED -f";

my $az="a-z";                                     # for perl 
my $AZ="A-Z";                                     # we may assume there are
my $NN="0-9";                                     # char-ranges available
my $AA="_$NN$AZ$az";                              # that makes the resulting
my $AX="$AA.+-";                                  # script more readable

my $n = "\n";

# LANG="C" ; LANGUAGE="C" ; LC_COLLATE="C"     # these are needed for proper
# export LANG LANGUAGE LC_COLLATE              # lowercasing as some collate
                                               # treat A-Z to include a-z

# ==========================================================================
# reading options from the command line                            GETOPT
my %o = (); # to store option variables
$o{variables}="files";
$o{fileseparator}="?";
$o{files}="";
$o{main_file}="";
$o{formatter}="$0";
my $opt="";
for my $arg (@ARGV) {     # this variant should allow to embed spaces in $arg
    if ($opt) {
	$o{$opt}=$arg;
	$opt="";
    } else {
	$_=$arg;
	if (/^-.*=.*$/) {
	    $opt=$arg; $opt =~ s/-*([$AA][$AA-]*).*/$1/; $opt =~ y/-/_/;
	    if (not $opt) {
		print STDERR "ERROR: invalid option $arg$n";
	    } else {
		$arg =~ s/^[^=]*=//;
		$o{$opt} = $arg;
		$o{variables} .= " ".$opt;
	    }
	    $opt="";;
	} elsif (/^-.*-.*$/) {
	    $opt=$arg; $opt =~ s/-*([$AA][$AA-]*).*/$1/; $opt =~ y/-/_/;
	    if (not $opt) {
		print STDERR "ERROR: invalid option $arg$n";
		$opt="";
	    } else {
		    # keep the option for next round
	    } ;;
	} elsif (/^-.*/) {
	    $opt=$arg; $opt =~ s/^-*([$AA][$AA-]*).*/$1/; $opt =~ y/-/_/;
	    if (not $opt) {
		print STDERR "ERROR: invalid option $arg$n";
	    } else {
		$arg =~ s/^[^=]*=//;
		$o{$opt} = ' ';
	    }
	    $opt="" ;;
	} else {
	    if (not $o{main_file}) { $o{main_file} = $arg; } else {
	    $o{files} .= $o{fileseparator} if $o{files};
	    $o{files} .= $arg; };
	    $opt="" ;;
	};
    }
} ; if ($opt) {
	$o{$opt}=" ";
	$opt="";
    }

### env | grep ^opt

$SITEFILE=$o{main_file} if $o{main_file} and -f $o{main_file};
$SITEFILE=$o{site_file} if $o{site_file} and -f $o{site_file};

if ($o{help}) {
    $_=$SITEFILE;
    print "$0 [sitefile]$n";
    print "  default sitefile = $_$n";
    print "options:$n"
	. " --filelist : show list of target files as ectracted from $_$n"
	. " --src xx : if source files are not where mksite is executed$n";
    exit;
    print " internal:$n"
	."--fileseparator=x : for building the internal filelist (def. '?')$n"
	."--files xx : for list of additional files to be processed$n"
	."--main-file xx : for the main sitefile to take file list from$n";
}
	
if (not $SITEFILE) {
    print STDERR "error: no SITEFILE found (default would be 'site.htm')$n";
    exit 1;
} else {
    print STDERR "NOTE: sitefile: ",`ls -s $SITEFILE`,"$n";
}

# we use internal hashes to store mappings - kind of relational tables
my @MK_TAGS= (); # "./$MK.tags.tmp"
my @MK_VARS= (); # "./$MK.vars.tmp"
my @MK_META= (); # "./$MK.meta.tmp"
my @MK_METT= (); # "./$MK.mett.tmp"
my @MK_TEST= (); # "./$MK.test.tmp"
my @MK_FAST= (); # "./$MK.fast.tmp"
my @MK_GETS= (); # "./$MK.gets.tmp"
my @MK_PUTS= (); # "./$MK.puts.tmp"
my @MK_OLDS= (); # "./$MK.olds.tmp"
my @MK_SITE= (); # "./$MK.site.tmp"
my @MK_SECT1= (); # "./$MK.sect1.tmp"
my @MK_SECT2= (); # "./$MK.sect2.tmp"
my @MK_SECT3= (); # "./$MK.sect3.tmp"
my @MK_INFO= (); # "./$MK~~"
my %INFO= (); # used for $F.$PARTs
my %FAST= ();

# ========================================================================
# ========================================================================
# ========================================================================
#                                                             MAGIC VARS
#                                                            IN $SITEFILE
my $printerfriendly="";
my $sectionlayout="list";
my $sitemaplayout="list";
my $simplevars="warn";      # <!--varname-->default
my $attribvars=" ";         # <x ref="${varname:=default}">
my $updatevars=" ";         # <!--$varname:=-->default
my $expandvars=" ";         # <!--$varname-->
my $commentvars=" ";        # $updatevars && $expandsvars && $simplevars
my $sectiontab=" ";         # highlight ^<td class=...>...href="$section"
my $currenttab=" ";         # highlight ^<br>..<a href="$topic">
my $headsection="no";
my $tailsection="no";
my $sectioninfo="no";       # using <h2> title <h2> = info text
my $emailfooter="no";

for (source($SITEFILE)) {
    if (/<!--multi-->/) {
	warn("WARNING: do not use <!--multi-->,"
	     ." change to <!--mksite:multi-->  $SITEFILE"
	     ."warning: or"
	     ." <!--mksite:multisectionlayout-->"
	     ." <!--mksite:multisitemaplayout-->");
	$sectionlayout="multi";
	$sitemaplayout="multi";
    }
    if (/<!--mksite:multi-->/) {
	$sectionlayout="multi";
	$sitemaplayout="multi";
    }
    if (/<!--mksite:multilayout-->/) {
	$sectionlayout="multi";
	$sitemaplayout="multi";
    }
}

sub mksite_magic_option
{
    # $1 is word/option to check for
    my ($U,$INP,$Z) = @_;
    $INP=$SITEFILE if not $INP;
    for (source($INP)) {
	s/(<!--mksite:)($U)-->/$1$2: -->/g;
	s/(<!--mksite:)(\w\w*)($U)-->/$1$3:$2-->/g;
	/<!--mksite:$U:/ or next;
	s/.*<!--mksite:$U:([^<>]*)-->.*/$1/;
	s/.*<!--mksite:$U:([^-]*)-->.*/$1/;
	/<!--mksite:$U:/ and next;
	chomp;
	return $_;
    }
    return "";
}

{
    my $x;
    $x=mksite_magic_option("sectionlayout"); if 
	($x =~ /^(list|multi)$/) { $sectionlayout="$x" ; }
    $x=mksite_magic_option("sitemaplayout"); if
	($x =~ /^(list|multi)$/) { $sitemaplayout="$x" ; }
    $x=mksite_magic_option("simplevars"); if
	($x =~ /^( |no|warn)$/) { $simplevars="$x" ; }
    $x=mksite_magic_option("attribvars"); if 
	($x =~ /^( |no|warn)$/) { $attribvars="$x" ; }
    $x=mksite_magic_option("updatevars"); if
	($x =~ /^( |no|warn)$/) { $updatevars="$x" ; }
    $x=mksite_magic_option("expandvars"); if
	($x =~ /^( |no|warn)$/) { $expandvars="$x" ; }
    $x=mksite_magic_option("commentvars"); if
	($x =~ /^( |no|warn)$/) { $commentvars="$x" ; }
    $x=mksite_magic_option("printerfriendly"); if
	($x =~ /^( |[.].*|[-]-.*)$/) { $printerfriendly="$x" ; }
    $x=mksite_magic_option("sectiontab"); if
	($x =~ /^( |no|warn)$/) { $sectiontab="$x" ; }
    $x=mksite_magic_option("currenttab"); if
	($x =~ /^( |no|warn)$/) { $currenttab="$x" ; }
    $x=mksite_magic_option("sectioninfo"); if
	($x =~ /^( |no|[=:-])$/) { $sectioninfo="$x" ; }
    $x=mksite_magic_option("commentvars"); if
	($x =~ /^( |no|warn)$/) { $commentvars="$x" ; }
    $x=mksite_magic_option("emailfooter"); if
	($x) { $emailfooter="$x"; }
}

$printerfriendly=$o{print} if $o{print};
$updatevars="no" if $commentvars eq "no"; # duplicated into
$expandvars="no" if $commentvars eq "no"; # info2vars_sed
$simplevars="no" if $commentvars eq "no"; # function above

print "NOTE: '$sectionlayout\'sectionlayout '$sitemaplayout\'sitemaplayout$n"
    if -d "DEBUG";
print "NOTE: '$simplevars\'simplevars '$printerfriendly\'printerfriendly$n"
    if -d "DEBUG";
print "NOTE: '$attribvars\'attribvars '$updatevars\'updatevars$n"
    if -d "DEBUG";
print "NOTE: '$expandvars\'expandvars '$commentvars\'commentvars $n"
    if -d "DEBUG";
print "NOTE: '$currenttab\'currenttab '$sectiontab\'sectiontab$n"
    if -d "DEBUG";
print "NOTE: '$headsection\'headsection '$tailsection\'tailsection$n"
    if -d "DEBUG";

# ==========================================================================
# init a few global variables
#                                                                  0. INIT

# $MK.tags.tmp - originally, we would use a lambda execution on each 
# uppercased html tag to replace <P> with <p class="P">. Here we just
# walk over all the known html tags and make an sed script that does
# the very same conversion. There would be a chance to convert a single
# tag via "h;y;x" or something we do want to convert all the tags on
# a single line of course.
@MK_TAGS=();
for my $P (qw/P H1 H2 H3 H4 H5 H6 DL DD DT UL OL LI PRE CODE TABLE TR TD TH 
	   B U I S Q EM STRONG STRIKE CITE BIG SMALL SUP SUB TT THEAD TBODY 
	   CENTER HR BR NOBR WBR SPAN DIV IMG ADRESS BLOCKQUOTE/) {
    my $M=lc($P);
    push @MK_TAGS, "s|<$P>|<$M class=\\\"$P\\\">|g;";
    push @MK_TAGS, "s|<$P |<$M class=\\\"$P\\\" |g;";
    push @MK_TAGS, "s|</$P>|</$M>|g;";
}
push @MK_TAGS, "s|<>|\\&nbsp\\;|g;";
push @MK_TAGS, "s|<->|<WBR />\\;|g;";
# also make sure that some non-html entries are cleaned away that
# we are generally using to inject meta information. We want to see
# that meta ino in the *.htm browser view during editing but they
# shall not get present in the final html page for publishing.
my @DC_VARS = 
    ("contributor", "date", "source", "language", "coverage", "identifier",
     "rights", "relation", "creator", "subject", "description",
     "publisher", "DCMIType");
my @_EQUIVS =
    ("refresh", "expires", "content-type", "cache-control", 
     "redirect", "charset", # mapped to refresh / content-type
     "content-language", "content-script-type", "content-style-type");
for my $P (@DC_VARS) { # dublin core embedded
    push @MK_TAGS, "s|<$P>[^<>]*</$P>||g;";
}
for my $P (@_EQUIVS) {
    push @MK_TAGS, "s|<$P>[^<>]*</$P>||g;";
}
push @MK_TAGS, "s|<!--sect[$AZ$NN]-->||g;";
push @MK_TAGS, "s|<!--[$AX]*[?]-->||g;";
push @MK_TAGS, "s|<!--\\\$[$AX]*[?]:-->||g;";
push @MK_TAGS, "s|<!--\\\$[$AX]*:[?=]-->||g;";
push @MK_TAGS, "s|(<[^<>]*)\\\${[$AX]*:[?=]([^<{}>]*)}([^<>]*>)|\$1\$2\$3|g;";

my $TRIMM=" -e 's:^ *::' -e 's: *\$::'";  # trimm away leading/trailing spaces
sub trimm
{
    my ($T,$Z) = @_;
    $T =~ s:\A\s*::s; $T =~ s:\s*\Z::s;
    return $T;
}
sub trimmm
{
    my ($T,$Z) = @_;
    $T =~ s:\A\s*::s; $T =~ s:\s*\Z::s; $T =~ s:\s+: :g;
    return $T;
}
sub timezone
{
    # +%z is an extension while +%Z is supposed to be posix
    my $tz;
    eval { $tz = strftime("%z", localtime()) };
    return $tz  if $tz =~ /[+]/;
    return $tz  if $tz =~ /[-]/;
    return strftime("%Z", localtime());
}

sub timetoday
{
    return strftime("%Y-%m-%d", localtime());
}
sub timetodays
{
    return strftime("%Y-%m%d", localtime());
}

sub esc
{
    my ($TXT,$XXX) = @_;
    $TXT =~ s|&|\\\\&|g;
    return $TXT;
}

my %SOURCE;
sub source # $file : @lines
{
    my ($FILE,$Z) = @_;
    if (exists $SOURCE{$FILE}) { return @{$SOURCE{$FILE}}; }
    my @TEXT = ();
    open FILE, "<$FILE" or die "could not open $FILE: $!";
    for my $line (<FILE>) {
	push @TEXT, $line;
    } close FILE;
    @{$SOURCE{$FILE}} = @TEXT;
    return @{$SOURCE{$FILE}};
}
sub savesource # $file \@lines
{
    my ($FILE,$LINES,$Z) = @_;
    @{$SOURCE{$FILE}} = @{$LINES};
}

my $F; # current file during loop <<<<<<<<<
my $i = 100;
sub savelist {
    if (-d "DEBUG") {
	my $X = "$F._$i"; $i++; $X =~ s|/|:|g;
	open X, ">DEBUG/$X" or die "could not open $X: $!";
	print X "#! /usr/bin/perl -".$#_."$n";
	print X join("$n", @{$_[0]}),$n; close X;
    }
}

sub eval_MK_LIST # $str @list
{
    my $result = $_[0]; shift @_;
    my $extra = "";
    my $script = "\$_ = \$result; my \$Z;";
    $script .= join(";$n ", @_);
    $script .= "$n;\$result = \$_;$n";
    eval $script;
    return $result.$extra;
}

sub eval_MK_FILE  {
    my $FILENAME = $_[0]; shift @_;
    my $result = "";
    my $script = "my \$FILE; my \$extra = ''; my \$Z; $n";
    $script.= "for (source('$FILENAME')) { $n";
    $script.= join(";$n  ", @_);
    $script.= "$n; \$result .= \$_; ";
    $script.= "$n if(\$extra){\$result.=\$extra;\$extra='';\$result.=\"\\n\"}";
    $script.= "$n} if(\$extra){\$result.=\$extra;}$n";
    savelist([$script,""]);
    eval $script;
    return $result;
}
my $sed_add = "\$extra .= "; # "/r ";

sub foo { print "               '$F'$n"; }
sub ls_s {
    my $result = `ls -s @_`;
    chomp($result);
    return $result;
}

# ======================================================================
#                                                                FUNCS

my $SOURCEFILE; # current file <<<<<<<<
my @FILELIST; # <<<<<<<

sub sed_slash_key  # helper to escape chars special in /anchor/ regex
{                     # currently escaping "/" "[" "]" "."
    my $R = $_[0]; $R =~ s|[\"./[-]|\\$&|g; $R =~ s|\]|\\\\$&|g;
    return $R;
}
sub sed_piped_key  # helper to escape chars special in s|anchor|| regex
{                     # currently escaping "|" "[" "]" "."
    my $R = $_[0]; $R =~ s/[\".|[-]/\\$&/g; $R =~ s/\]/\\\\$&/g;
    return $R;
}

sub back_path      # helper to get the series of "../" for a given path
{
    my ($R,$Z) = @_; if ($R !~ /\//) { return ""; }
    $R =~ s|/[^/]*$|/|; $R =~ s|[^/]*/|../|g;
    return $R;
}

sub dir_name
{
    my $R = $_[0]; $R =~ s:/[^/][^/]*\$::;
    return $R;
}

sub info2test_sed # \@ \@ # cut out all old-style <!--vars--> usages
{
    my ($INP,$XXX) = @_;
    $INP = \@{$INFO{$F}} if not $INP;
    my @OUT = ();
    my $V8=" *([^ ][^ ]*) (.*)";
    my $V9=" *DC[.]([^ ][^ ]*) (.*)";
    my $q="\\\$";
    my ($_x_,$_y_,$_X_,$_Y_); my $W = "WARNING:";
    $_x_= sub {"$W: assumed simplevar <!--$1--> changed to <!--$q$1:=-->" };
    $_y_= sub {"$W: assumed simplevar <!--$1--> changed to <!--$q$1:?-->" };
    $_X_= sub {"$W: assumed tailvar <!--$q$1:--> changed to <!--$q$1:=-->" };
    $_Y_= sub {"$W: assumed tailvar <!--$q$1:--> changed to <!--$q$1:?-->" };
    push @OUT, "s/^/ /;";
    for (@$INP) {
    if (/^=....=formatter /) { next; };
    if (/=[Tt]ext=$V9%/){ push @OUT, esc("s|.*<!--($1)-->.*|".&$_x_."|;");}
    if (/=[Nn]ame=$V9%/){ push @OUT, esc("s|.*<!--($1)[?]-->.*|".&$_y_."|;");}
    if (/=[Tt]ext=$V8%/){ push @OUT, esc("s|.*<!--($1)-->.*|".&$_x_."|;");}
    if (/=[Nn]ame=$V8%/){ push @OUT, esc("s|.*<!--($1)[?]-->.*|".&$_y_."|;");}
    }
    for (@$INP) {
    if (/^=....=formatter /) { next; };
    if (/=[Tt]ext=$V9%/){ push @OUT, esc("s|.*<!--($1):-->.*|".&$_X_."|;");}
    if (/=[Nn]ame=$V9%/){ push @OUT, esc("s|.*<!--($1)[?]:-->.*|".&$_Y_."|;");}
    if (/=[Tt]ext=$V8%/){ push @OUT, esc("s|.*<!--($1):-->.*|".&$_X_."|;");}
    if (/=[Nn]ame=$V8%/){ push @OUT, esc("s|.*<!--($1)[?]:-->.*|".&$_Y_."|;");}
    }
    push @OUT, "/^WARNING:/ || next;";
    return @OUT;
}

sub info2vars_sed      # generate <!--$vars--> substition sed addon script
{
    my ($INP,$Z) = @_;
    $INP = \@{$INFO{$F}} if not $INP;
    my @OUT = ();
    my $V8=" *([^ ][^ ]*) +(.*)";
    my $V9=" *DC[.]([^ ][^ ]*) +(.*)";
    my $N8=" *([^ ][^ ]*) ([$NN].*)";
    my $N9=" *DC[.]([^ ][^ ]*) ([$NN].*)";
    my $V0="([<]*)\\\$";
    my $V1="([^<>]*)\\\$";
    my $V2="([^{<>}]*)";
    my $V3="([^<>]*)";
    my $SS="<"."<>".">"; # spacer so value="2004" dont make for s|\(...\)|\12004|
    $Z="\$Z=";
    $updatevars = "no" if $commentvars  eq "no";   # duplicated from
    $expandvars = "no" if $commentvars  eq "no";   # option handling
    $simplevars = "no" if $commentvars  eq "no";   # tests below
    my @_INP = (); for (@{$INP}) { my $x=$_; $x =~ s/'/\\'/; push @_INP, $x; }
    if ($expandvars ne "no") {
	for (@_INP) { 
    if    (/^=....=formatter /) { next; } 
    elsif (/^=name=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?-->|- \$Z|;"}
    elsif (/^=Name=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?-->|(\$Z)|;"}
    elsif (/^=name=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?-->|- \$Z|;"}
    elsif (/^=Name=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?-->|(\$Z)|;"}
        } 
    }
    if ($expandvars ne "no") {
	for (@_INP) {
    if    (/^=....=formatter /) { next; } 
    elsif (/^=text=$V9/){push @OUT, "\$Z='$2';s|<!--$V1$1-->|\$1$SS\$Z|;"}
    elsif (/^=Text=$V9/){push @OUT, "\$Z='$2';s|<!--$V1$1-->|\$1$SS\$Z|;"}
    elsif (/^=name=$V9/){push @OUT, "\$Z='$2';s|<!--$V1$1\\?-->|\$1$SS\$Z|;"}
    elsif (/^=Name=$V9/){push @OUT, "\$Z='$2';s|<!--$V1$1\\?-->|\$1$SS\$Z|;"}
    elsif (/^=text=$V8/){push @OUT, "\$Z='$2';s|<!--$V1$1-->|\$1$SS\$Z|;"}
    elsif (/^=Text=$V8/){push @OUT, "\$Z='$2';s|<!--$V1$1-->|\$1$SS\$Z|;"}
    elsif (/^=name=$V8/){push @OUT, "\$Z='$2';s|<!--$V1$1\\?-->|\$1$SS\$Z|;"}
    elsif (/^=Name=$V8/){push @OUT, "\$Z='$2';s|<!--$V1$1\\?-->|\$1$SS\$Z|;"}
	}
    }
    if ($simplevars ne "no" && $updatevars ne "no") {
	for (@_INP) { my $Q = "[$AX]*";
    if    (/^=....=formatter /) { next; } 
    elsif (/^=text=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1:-->$Q|\$Z|;"}
    elsif (/^=Text=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1:-->$Q|\$Z|;"}
    elsif (/^=name=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?:-->$Q|- \$Z|;"}
    elsif (/^=Name=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?:-->$Q|(\$Z)|;"}
    elsif (/^=text=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1:-->$Q|\$Z|;"}
    elsif (/^=Text=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1:-->$Q|\$Z|;"}
    elsif (/^=name=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?:-->$Q|- \$Z|;"}
    elsif (/^=Name=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1\\?:-->$Q|(\$Z)|;"}
	}
    }
    if ($updatevars ne "no") {
	for (@_INP) {  my $Q = "[^<>]*";
    if    (/^=....=formatter /) { next; }
    elsif (/^=name=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1:\\?-->$Q|- \$Z|;"}
    elsif (/^=Name=$V9/){push @OUT, "\$Z='$2';s|<!--$V0$1:\\?-->$Q|(\$Z)|;"}
    elsif (/^=name=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1:\\?-->$Q|- \$Z|;"}
    elsif (/^=Name=$V8/){push @OUT, "\$Z='$2';s|<!--$V0$1:\\?-->$Q|(\$Z)|;"}
	}
    }
    if ($updatevars ne "no") {
	for (@_INP) {  my $Q = "[^<>]*";
    if    (/^=....=formatter /) { next; }
    elsif (/^=text=$V9/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\=-->$Q|\$1$SS\$Z|;"}
    elsif (/^=Text=$V9/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\=-->$Q|\$1$SS\$Z|;"}
    elsif (/^=name=$V9/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\?-->$Q|\$1$SS\$Z|;"}
    elsif (/^=Name=$V9/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\?-->$Q|\$1$SS\$Z|;"}
    elsif (/^=text=$V8/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\=-->$Q|\$1$SS\$Z|;"}
    elsif (/^=Text=$V8/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\=-->$Q|\$1$SS\$Z|;"}
    elsif (/^=name=$V8/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\?-->$Q|\$1$SS\$Z|;"}
    elsif (/^=Name=$V8/){push @OUT,"\$Z='$2';s|<!--$V1$1:\\?-->$Q|\$1$SS\$Z|;"}
	}
    }
    if ($attribvars ne "no") {
	for (@_INP) {  my $Q = "[^<>]*";
    if    (/^=....=formatter /) { next; }
    elsif (/^=text=$V9/){push @OUT,"\$Z='$2';s|<$V1\{$1:[=]$V2}$V3>|<\$1$SS\$Z\$3>|;"}
    elsif (/^=Text=$V9/){push @OUT,"\$Z='$2';s|<$V1\{$1:[=]$V2}$V3>|<\$1$SS\$Z\$3>|;"}
    elsif (/^=name=$V9/){push @OUT,"\$Z='$2';s|<$V1\{$1:[?]$V2}$V3>|<\$1$SS\$Z\$3>|;"}
    elsif (/^=Name=$V9/){push @OUT,"\$Z='$2';s|<$V1\{$1:[?]$V2}$V3>|<\$1$SS\$Z\$3>|;"}
    elsif (/^=text=$V8/){push @OUT,"\$Z='$2';s|<$V1\{$1:[=]$V2}$V3>|<\$1$SS\$Z\$3>|;"}
    elsif (/^=Text=$V8/){push @OUT,"\$Z='$2';s|<$V1\{$1:[=]$V2}$V3>|<\$1$SS\$Z\$3>|;"}
    elsif (/^=name=$V8/){push @OUT,"\$Z='$2';s|<$V1\{$1:[?]$V2}$V3>|<\$1$SS\$Z\$3>|;"}
    elsif (/^=Name=$V8/){push @OUT,"\$Z='$2';s|<$V1\{$1:[?]$V2}$V3>|<\$1$SS\$Z\$3>|;"} 
	}
        for (split / /, $o{variables}) {
	    {push @OUT,"\$Z='$o{$_}';s|<$V1\{$_:[?]$V2}$V3>|<\$1$SS\$Z\$3>|;"} 
	}
    }
    if ($simplevars ne "no") {
	for (@_INP) {  my $Q = "[$AX]*";
    if    (/^=....=formatter /) { next; }
    elsif (/^=text=$V9/){push @OUT, "\$Z='$2';s|<!--$1-->$Q|\$Z|;"}
    elsif (/^=Text=$V9/){push @OUT, "\$Z='$2';s|<!--$1-->$Q|\$Z|;"}
    elsif (/^=name=$V9/){push @OUT, "\$Z='$2';s|<!--$1\\?-->$Q| - \$Z|;"}
    elsif (/^=Name=$V9/){push @OUT, "\$Z='$2';s|<!--$1\\?-->$Q| (\$Z)|;"}
    elsif (/^=text=$V8/){push @OUT, "\$Z='$2';s|<!--$1-->$Q|\$Z|;"}
    elsif (/^=Text=$V8/){push @OUT, "\$Z='$2';s|<!--$1-->$Q|\$Z|;"}
    elsif (/^=name=$V8/){push @OUT, "\$Z='$2';s|<!--$1\\?-->$Q| - \$Z|;"}
    elsif (/^=Name=$V8/){push @OUT, "\$Z='$2';s|<!--$1\\?-->$Q| (\$Z)|;"}
	}
    }
    # if value="2004" then generated sed might be "\\12004" which is bad
    # instead we generate an edited value of "\\1$SS$value" and cut out
    # the spacer now after expanding the variable values:
    push @OUT, "s|$SS||g;";
    return @OUT;
	
}

sub info2meta_sed     # generate <meta name..> text portion
{
    my ($INP,$XXX) = @_;
    $INP = \@{$INFO{$F}} if not $INP;
    my @OUT = ();
    # http://www.metatab.de/meta_tags/DC_type.htm
    my $V6=" *HTTP[.]([^ ]+) (.*)";
    my $V7=" *DC[.]([^ ]+) (.*)";
    my $V8=" *([^ ]+) (.*)" ;
    sub __TYPE_SCHEME { "name=\"DC.type\" content=\"$2\" scheme=\"$1\"" };
    sub __DCMI { "name=\"$1\" content=\"$2\" scheme=\"DCMIType\"" };
    sub __NAME { "name=\"$1\" content=\"$2\"" };
    sub __NAME_TZ { "name=\"$1\" content=\"$2 ".&timezone()."\"" };
    sub __HTTP { "http-equiv=\"$1\" content=\"$2\"" };
    for (@$INP) {
	if (/=....=today /) { next; }
	if (/=meta=HTTP[.]/ && /=meta=$V6/) {
	    push @OUT, " <meta ${\(__HTTP)} />" if $2; next; }
	if (/=meta=DC[.]DCMIType / && /=meta=$V7/) {
	    push @OUT, " <meta ${\(__TYPE_SCHEME)} />" if $2; next; }
	if (/=meta=DC[.]type Collection$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]type Dataset$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]type Event$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]type Image$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]type Service$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]type Software$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]type Sound$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]type Text$/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__DCMI)} />" if $2; next; }
	if (/=meta=DC[.]date[.].*[+]/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__NAME)} />" if $2; next; }
	if (/=meta=DC[.]date[.].*[:]/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__NAME_TZ)} />" if $2; next; }
	if (/=meta=/ && /=meta=$V8/) {
	    push @OUT, " <meta ${\(__NAME)} />" if $2; next; }
    }
    return @OUT;
}

sub info_get_entry # get the first <!--vars--> value known so far
{
    my ($TXT,$INP,$XXX) = @_;
    $TXT = "sect" if not $TXT;
    $INP = \@{$INFO{$F}} if not $INP;
    for (grep {/=text=$TXT /} @$INP) {
	my $info = $_;
	$info =~ s/=text=$TXT //; 
	chomp($info); chomp($info); return $info;
    }
}

sub info1grep # test for a <!--vars--> substition to be already present
{
    my ($TXT,$INP,$XXX) = @_;
    $TXT = "sect" if not $TXT;
    $INP = \@{$INFO{$F}} if not $INP;
    return scalar(grep {/^=text=$TXT /} @$INP); # returning the count
}

sub dx_init
{
    @{$INFO{$F}} = ();
    &dx_meta ("formatter", basename($o{formatter}));
    for (split / /, $o{variables}) {        # commandline --def=value
	if (/_/) { my $u=$_; $u =~ y/_/-/;  # makes for <!--$def--> override
                   &dx_meta ($u, $o{$_}); 
	} else {   &dx_text ($_, $o{$_}); }
    }
}

sub dx_line
{
    my ($U,$V,$W,$Z) = @_; chomp($U); chomp($V);
    push @{$INFO{$F}}, $U.$V." ".trimmm($W);
}

sub DX_line
{
    my ($U,$V,$W,$Z) = @_; $W =~ s/<[^<>]*>//g;
    &dx_line ($U,$V,$W);
}

sub dx_text
{
    my ($U,$V,$Z) = @_;
    &dx_line ("=text=",$U,$V);
}

sub DX_text   # add a <!--vars--> substition includings format variants
{
    my ($N, $T,$XXX) = @_;
    $N = trimm($N); $T = trimm($T); 
    if ($N) {
	if ($T) {
	    my $text=lc("$T"); $text =~ s/<[^<>]*>//g;
	    &dx_line ("=text=",$N,$T);
	    &dx_line ("=name=",$N,$text);
	    my $varname=$N; $varname =~ s/.*[.]//;  # cut out front part
	    if ($N ne $varname and $varname) {
		$text=lc("$varname $T"); $text =~ s/<[^<>]*>//g;
		&dx_line ("=Text=",$varname,$T);
		&dx_line ("=Name=",$varname,$text);
	    }
	}
    }
}

sub dx_meta
{
    my ($U,$V,$Z) = @_;
    &DX_line ("=meta=",$U,$V);
}

sub DX_meta  # add simple meta entry and its <!--vars--> subsitution
{
    my ($U,$V,$Z) = @_;
    &DX_line ("=meta=",$U,$V);
    &DX_text ("$U", $V);
}

sub DC_meta   # add new DC.meta entry plus two <!--vars--> substitutions
{
    my ($U,$V,$Z) = @_;
    &DX_line ("=meta=","DC.$U",$V);
    &DX_text ("DC.$U", $V);
    &DX_text ("$U", $V);
}

sub HTTP_meta   # add new HTTP.meta entry plus two <!--vars--> substitutions
{
    my ($U,$V,$Z) = @_;
    &DX_line ("=meta=","HTTP.$U",$V);
    &DX_text ("HTTP.$U", $V);
    &DX_text ("$U", $V);
}

sub DC_VARS_Of # check DC vars as listed in $DC_VARS global/generate DC_meta
{                 # the results will be added to .meta.tmp and .vars.tmp later
    my ($FILENAME,$Z)= @_; 
    $FILENAME=$SOURCEFILE if not $FILENAME;
    for my $M (@DC_VARS, "title") {
	# scan for a <markup> of this name                  FIXME
	my ($part,$text);
	for (source($FILENAME)) {
	    /<$M>/ or next; s|.*<$M>||; s|</$M>.*||;
	    $part = trimm($_); last;
	}
	$text=$part;  $text =~ s|^\w*:||; $text = trimm($text);
	next if not $text;
	# <mark:part> will be <meta name="mark.part"> 
	if ($text ne $part) {
	    my $N=$part; $N =~ s/:.*//;
	    &DC_meta ("$M.$N", $text);
	} elsif ($M eq "date") {
	    &DC_meta ("$M.issued", $text); # "<date>" -> "<date>issued:"
	} else {
	    &DC_meta ("$M", $text);
	}
    }
}

sub HTTP_VARS_Of  # check HTTP-EQUIVs as listed in $_EQUIV global then
{                 # generate meta tags that are http-equiv= instead of name=
    my ($FILENAME,$Z)= @_; 
    $FILENAME=$SOURCEFILE if not $FILENAME;
    for my $M (@_EQUIVS) {
	# scan for a <markup> of this name                  FIXME
	my ($part,$text);
	for (source($FILENAME)) {
	    /<$M>/ or next; s|.*<$M>||; s|</$M>.*||;
	    $part = trimm($_); last;
	}
	$text=$part;  $text =~ s|^\w*:||; $text = trimm($text);
	next if not $text;
	if ($M eq "redirect") {
	    &HTTP_meta ("refresh", "5; url=$text"); &DX_text ("$M", $text);
	} elsif ($M eq "charset") {
	    &HTTP_meta ("content-type", "text/html; charset=$text");
	} else {
	    &HTTP_meta ("$M", $text);
	}
    }
}

sub DC_isFormatOf   # make sure there is this DC.relation.isFormatOf tag
{                      # choose argument for a fallback (usually $SOURCEFILE)
    my ($NAME,$Z) = @_;
    $NAME=$SOURCEFILE if not $NAME;
    if (not &info1grep ("DC.relation.isFormatOf")) {
	&DC_meta ("relation.isFormatOf", "$NAME");
    }
}

sub DC_publisher    # make sure there is this DC.publisher meta tag
{                      # choose argument for a fallback (often $USER)
    my ($NAME,$Z) = @_;
    $NAME=$ENV{"USER"} if not $NAME;
    if (not &info1grep ("DC.publisher")) {
	&DC_meta ("publisher", "$NAME");
    }
}

sub DC_modified     # make sure there is a DC.date.modified meta tag
{                      # maybe choose from filesystem dates if possible
    my ($Q,$Z) = @_; # target file
    if (not &info1grep ("DC.date.modified")) {
	my @stats = stat($Q);
	my $text =  strftime("%Y-%m-%d", localtime($stats[9]));
	&DC_meta ("date.modified", $text);
    }
}

sub DC_date         # make sure there is this DC.date meta tag
{                      # choose from one of the available DC.date.* specials
    my ($Q,$Z) = @_; # source file
    if (&info1grep ("DC.date")) {
	&DX_text ("issue", "dated ".&info_get_entry("DC.date"));
        &DX_text ("updated", &info_get_entry("DC.date"));
    } else { 
	my $text=""; my $kind;
	for $kind (qw/available issued modified created/) {
	    $text=&info_get_entry("DC.date.$kind");
	    # test ".$text" != "." && echo "$kind = date = $text ($Q)"
	    last if $text;
	}
	if (not $text) {
	    my $part; my $M="date";
	    for (source($Q)) {
		/<$M>/ or next; s|.*<$M>||; s|</$M>.*||;
		$part=trimm($_); last;
	    }
	    $text=$part; $text =~ s|^[$AA]*:||;
	    $text = &trimm ($text);
	}
	if (not $text) {
	    my $part; my $M="!--date:*=*--"; # takeover updateable variable...
	    for (source($Q)) {
		/<$M>/ or next; s|.*<$M>||; s|</.*||;
		$part=trimm($_); last;
	    }
	    $text=$part; $text =~ s|^[$AA]*:||; $text =~ s|\&.*||;
	    $text = &trimm ($text);
	}
	$text =~ s/[$NN]*:.*//; # cut way seconds
	&DX_text ("updated", $text);
	my $text1=$text; $text1 =~ s|^.* *updated ||;
	if ($text ne $text1) {
	    $kind="modified" ; $text=$text1; $text =~ s|,.*||;
	}
	$text1=$text; $text1 =~ s|^.* *modified ||;
	if ($text ne $text1) {
	    $kind="modified" ; $text=$text1; $text =~ s|,.*||;
	}
	$text1=$text; $text1 =~ s|^.* *created ||;
	if ($text ne $text1) {
	    $kind="created" ; $text=$text1; $text =~ s|,.*||;
	}
	&DC_meta ("date", "$text");
	&DX_text ("issue", "$kind $text");
    }
}

sub DC_title
{
    # choose a title for the document, either an explicit title-tag
    # or one of the section headers in the document or fallback to filename
    my ($Q,$Z) = @_; # target file
    my ($term, $text);
    if (not &info1grep ("DC.title")) { 
	for my $M (qw/TITLE title H1 h1 H2 h2 H3 H3 H4 H4 H5 h5 H6 h6/) {
	    for (source($Q)) {
		/<$M>/ or next; s|.*<$M>||; s|</$M>.*||;
		$text = trimm($_); last;
	    }
	    last if $text;
	    for (source($Q)) {
		/<$M [^<>]*>/ or next; s|.*<$M [^<>]*>||; s|</$M>.*||;
		$text = trimm($_); last;
	    }
	    last if $text;
	}
	if (not $text) {
	    $text=basename($Q,".html"); 
	    $text=basename($text,".htm"); $text =~ y/_/ /; $text =~ s/$/ info/;
	    $text=~ s/\n/      /g;
	}
	$term=$text; $term =~ s/.*[\(]//; $term =~ s/[\)].*//;
	$text =~ s/[\(][^\(\)]*[\)]//;
	if (not $term or $term eq $text) {
	    &DC_meta ("title", "$text");
	} else {
	    &DC_meta ("title", "$term - $text");
	}
    }
}    

sub site_get_section # return parent section page of given page
{
    my $_F_ = &sed_slash_key(@_);
    for my $x (grep {/=sect=$_F_ /} @MK_INFO) {
	my $info = $x; $info =~ s/=sect=[^ ]* //; return $info;
    }
}

sub DC_section # not really a DC relation (shall we use isPartOf ?) 
{                 # each document should know its section father
    my $sectn = &site_get_section($F);
     if ($sectn) {
	&DC_meta ("relation.section", $sectn);
    }
}

sub info_get_entry_section
{
    return &info_get_entry("DC.relation.section");
}    

sub site_get_selected  # return section of given page
{
    my $_F_ = &sed_slash_key(@_);
    for my $x (grep {/=[u]se.=$_F_ /} @MK_INFO) {
	my $info = $x; $info =~ s/=[u]se.=[^ ]* //; return $info;
    }
}

sub DC_selected # not really a DC title (shall we use alternative ?)
{
    # each document might want to highlight the currently selected item
    my $short=&site_get_selected($F);
    if ($short) {
	&DC_meta ("title.selected", $short);
    }
}

sub info_get_entry_selected
{
    return &info_get_entry("DC.title.selected");
}

sub site_get_rootsections # return all sections from root of nav tree
{
    my @OUT;
    for (grep {/=[u]se1=/} @MK_INFO) { 
	my $x = $_;
	$x =~ s/=[u]se.=([^ ]*) .*/$1/; 
	push @OUT, $x;
    }
    return @OUT;
}

sub site_get_sectionpages # return all children pages in the given section
{
    my $_F_=&sed_slash_key(@_);
    my @OUT = ();
    for (grep {/^=sect=[^ ]* $_F_$/} @MK_INFO) {
	my $x = $_;
	$x =~ s/^=sect=//; $x =~ s/ .*//;
	push @OUT, $x;
    }
    return @OUT;
}

sub site_get_subpages # return all page children of given page
{
    my $_F_=&sed_slash_key(@_);
    my @OUT = ();
    for (grep {/^=node=[^ ]* $_F_$/} @MK_INFO) {
	my $x = $_;
	$x =~ s/^=node=//; $x =~ s/ .*//;
	push @OUT, $x;
    }
    return @OUT;
}

sub site_get_parentpage # ret parent page for given page (".." for sections)
{
    my $_F_=&sed_slash_key(@_);
    for (grep {/^=node=$_F_ /} @MK_INFO) {
	my $x = $_;
	$x =~ s/^=node=[^ ]* //;
	return $x;
    } 
}

sub DX_alternative    # detect wether page asks for alternative style
{                        # which is generally a shortpage variant 
    my ($U,$Z) = @_;
    my $x=&mksite_magic_option("alternative",$U);
    $x =~ s/^ *//; $x =~s/ .*//;
    if ($x) {
	&DX_text ("alternative", $x);
    }
}

sub info2head_sed  # append alternative handling script to $HEAD
{
    my @OUT = ();
    my $have=&info_get_entry("alternative");
    if ($have) {
	push @OUT, "/<!--mksite:alternative:$have .*-->/ && do {";
	push @OUT, "s/<!--mksite:alternative:$have( .*)-->/\$1/";
	push @OUT, "$sed_add \$_; last; };";
    }
    return @OUT;
}
sub info2body_sed  # append alternative handling script to $BODY
{
    my @OUT = ();
    my $have=&info_get_entry("alternative");
    if ($have) {
	push @OUT, "s/<!--mksite:alternative:$have( .*)-->/\$1/";
    }
    return @OUT;
}

sub bodymaker_for_sectioninfo
{
    if ($sectioninfo eq "no") { return ""; }
    my $_x_="<!--mksite:sectioninfo::-->";
    my $_q_="([^<>]*[$AX][^<>]*)";
    $_q_="[ ][ ]*$sectioninfo([ ])" if $sectioninfo ne " ";
    my @OUT = ();
    push @OUT, "s|(^<[hH][$NN][ >].*</[hH][$NN]>)$_q_|\$1$_x_\$2|";
    push @OUT, "/$_x_/ and s|^|<table width=\"100%\"><tr valign=\"bottom\"><td>|";
    push @OUT, "/$_x_/ and s|</[hH][$NN]>|&</td><td align=\"right\"><i>|";
    push @OUT, "/$_x_/ and s|\$|</i></td></tr></table>|";
    push @OUT, "s|$_x_||";
    return @OUT;
}

sub fast_href  # args "$FILETOREFERENCE" "$FROMCURRENTFILE:$F"
{   # prints path to $FILETOREFERENCE href-clickable in $FROMCURRENTFILE
    # if no subdirectoy then output is the same as input $FILETOREFERENCE
    my ($T,$R,$Z) = @_;
    my $S=&back_path ($R);
    if (not $S) {
	return $T;
    } else {
	my $t=$T;
	$t =~ s/^ *$//; $t =~ s/^\/.*//; 
	$t =~ s/^[.][.].*//; $t =~ s/^\w*:.*//;
	if (not $t) { # don't move any in the pattern above
	    return $T;
	} else {
	    return "$S$T";   # prefixed with backpath
	}
    }
}

sub make_fast # experimental - make a FAST file that can be applied
{             # to htm sourcefiles in a subdirectory of the sitefile.
#   R="$1" ; test ".$R" = "." && R="$F"
    my ($R,$Z) = @_;
    my $S=&back_path ($R);
    my @OUT = ();
    if (not $S) {
	# echo "backpath '$F' = none needed"
	return @OUT;
    } else {
	#  print "backpath '$F' -> '$S'$n";
	my @hrefs = ();
	for (source($SITEFILE)) {
	    /href=\"[^\"]*\"/ or next;
	    s/.*href=\"//; s/\".*//; chomp;
	    if (/^ *$/ || /^\// || /^[.][.]/ || /^[\w]*:/) { next; }
	    push @hrefs, $_;
	}
	for (source($SOURCEFILE)) {
	    /href=\"[^\"]*\"/ or next;
	    s/.*href=\"//; s/\".*//; chomp;
	    if (/^ *$/ || /^\// || /^[.][.]/ || /^[\w]*:/) { next; }
	    push @hrefs, $_;
	}
	my $ref = "";
	for (sort(@hrefs)) { 
	    next if /\$/; # some href="${...}" is problematic
	    next if $ref eq $_; $ref = $_; # uniq
	    push @OUT, "s|href=\\\"$ref\\\"|href=\\\"$S$ref\\\"|;";
	}
	return @OUT;
    }
}

# ============================================================== SITE MAP INFO
# each entry needs atleast a list-title, a long-title, and a list-date
# these are the basic information to be printed in the sitemap file
# where it is bound the hierarchy of sect/subsect of the entries.

sub site_map_list_title # $file $text
{
    my ($U,$V,$Z) = @_; chomp($U);
    push @MK_INFO, "=list=$U ".trimm($V);
}
sub info_map_list_title # $file $text
{
    my ($U,$V,$Z) = @_; chomp($U);
    push @{$INFO{$U}}, "=list=".trimm($V);
}
sub site_map_long_title # $file $text
{
    my ($U,$V,$Z) = @_; chomp($U);
    push @MK_INFO, "=long=$U ".trimm($V);
}
sub info_map_long_title # $file $text
{
    my ($U,$V,$Z) = @_; chomp($U);
    push @{$INFO{$U}}, "=long=".trimm($V);
}
sub site_map_list_date # $file $text
{
    my ($U,$V,$Z) = @_; chomp($U);
    push @MK_INFO, "=date=$U ".trimm($V);
}
sub info_map_list_date # $file $text
{
    my ($U,$V,$Z) = @_; chomp($U);
    push @{$INFO{$U}}, "=date=".trimm($V);
}

sub site_get_list_title
{
    my ($U,$V,$Z) = @_;
    for (@MK_INFO) { if (m|^=list=$U (.*)|) { return $1; } } return "";
}
sub site_get_long_title
{
    my ($U,$V,$Z) = @_;
    for (@MK_INFO) { if (m|^=long=$U (.*)|) { return $1; } } return "";
}
sub site_get_list_date
{
    my ($U,$V,$Z) = @_;
    for (@MK_INFO) { if (m|^=date=$U (.*)|) { return $1; } } return "";
}

sub siteinfo2sitemap# generate <name><page><date> addon sed scriptlet
{                      # the resulting script will act on each item/line
                       # containing <!--"filename"--> and expand any following
                       # reference of <!--name--> or <!--date--> or <!--long-->
    my ($INP,$Z) = @_ ; $INP= \@MK_INFO if not $INP;
    my @OUT = ();
    my $_list_=
	sub{"s|<!--\\\"$1\\\"-->.*<!--name-->|\$\&<name href=\\\"$1\\\">$2</name>|"};
    my $_date_=
	sub{"s|<!--\\\"$1\\\"-->.*<!--date-->|\$\&<date>$2</date>|"};
    my $_long_=
	sub{"s|<!--\\\"$1\\\"-->.*<!--long-->|\$\&<long>$2</long>|"};
    
    for (@$INP) {
	my $info = $_;
	$info =~ s:=list=([^ ]*) (.*):&$_list_:e;
	$info =~ s:=date=([^ ]*) (.*):&$_date_:e;
	$info =~ s:=long=([^ ]*) (.*):&$_long_:e;
	$info =~ /^s\|/ || next;
	push @OUT, $info;
    }
    return @OUT;
}

sub make_multisitemap
{  # each category gets its own column along with the usual entries
    my ($INPUTS,$Z)= @_ ; $INPUTS=\@MK_INFO if not $INPUTS;
    @MK_SITE = &siteinfo2sitemap(); # have <name><long><date> addon-sed
    my @OUT = (); 
    my $_form_= sub{"<!--\"$2\"--><!--use$1--><!--long--><!--end$1-->"
			."<br><!--name--><!--date-->" };
    my $_tiny_="small><small><small" ; my $_tinyX_="small></small></small ";
    my $_tabb_="<br><$_tiny_> </$_tinyX_>" ; my $_bigg_="<big> </big>";
    push @OUT, "<table width=\"100%\"><tr><td> ".$n;
    for (grep {/=[u]se.=/} @$INPUTS) {
	my $x = $_;
	$x =~ s|=[u]se(.)=([^ ]*) .*|&$_form_|e;
	$x = &eval_MK_LIST($x, @MK_SITE); $x =~ /<name/ or next;
	$x =~ s|<!--[u]se1-->|</td><td valign=\"top\"><b>|;
	$x =~ s|<!--[e]nd1-->|</b>|;
	$x =~ s|<!--[u]se2-->|<br>|;
	$x =~ s|<!--[u]se.-->|<br>|; $x =~ s/<!--[^<>]*-->/ /g;
	$x =~ s|<long>||; $x =~ s|</long>||;
	$x =~ s|<name |<$_tiny_><a |; $x =~ s|</name>||;
	$x =~ s|<date>| |; $x =~ s|</date>|</a><br></$_tinyX_>|;
	push @OUT, $x.$n;
    }
    push @OUT, "</td><tr></table>".$n;
    return @OUT;
}

sub make_listsitemap
{   # traditional - the body contains a list with date and title extras
    my ($INPUTS,$Z)= @_ ; $INPUTS=\@MK_INFO if not $INPUTS;
    @MK_SITE = &siteinfo2sitemap(); # have <name><long><date> addon-sed
    my @OUT = (); 
    my $_form_=sub{
	"<!--\"$2\"--><!--use$1--><!--name--><!--date--><!--long-->"};
    my $_tabb_="<td>\&nbsp\;</td>";
    push @OUT, "<table cellspacing=\"0\" cellpadding=\"0\">".$n;
    my $xx;
    for $xx (grep {/=[u]se.=/} @$INPUTS) {
	my $x = "".$xx;
	$x =~ s|=[u]se(.)=([^ ]*) .*|&$_form_|e;
	$x = &eval_MK_LIST($x, @MK_SITE); $x =~ /<name/ or next;
        $x =~ s|<!--[u]se(1)-->|<tr class=\"listsitemap$1\"><td>*</td>|;
        $x =~ s|<!--[u]se(2)-->|<tr class=\"listsitemap$1\"><td>-</td>|;
        $x =~ s|<!--[u]se(.)-->|<tr class=\"listsitemap$1\"><td> </td>|; 
        $x =~  /<tr.class=\"listsitemap3\">/ and $x =~ s|<name [^<>]*>|$&- |;
	$x =~ s|<!--[^<>]*-->| |g;
	$x =~ s|<name href=\"name:sitemap:|<name href=\"|;
        $x =~ s|<name |<td><a |;     $x =~ s|</name>|</a></td>$_tabb_|;
        $x =~ s|<date>|<td><small>|; $x =~ s|</date>|</small></td>$_tabb_|;
        $x =~ s|<long>|<td><em>|;    $x =~ s|</long>|</em></td></tr>|;
        push @OUT, $x; 
    }
    for $xx (grep {/=[u]se.=/} @$INPUTS) {
	my $x = $xx; $x =~ s/=[u]se.=name:sitemap://; $x =~ s:\s*::gs; 
	if (-f $x) { 
	    for (grep {/<tr.class=\"listsitemap\d\">/} source($x)) {
		push @OUT, $_;
	    }
	}
    }
    push @OUT, "</table>".$n;
    return @OUT;
}

sub print_extension
{
    my ($ARG,$Z)= @_ ; $ARG=$o{print} if not $ARG;
    if ($ARG =~ /^([.-])/) {
	return $ARG;
    } else {
	return ".print";
    }
}
    
sub html_sourcefile  # generally just cut away the trailing "l" (ell)
{                       # making "page.html" argument into "page.htm" return
    my ($U,$Z) = @_;
    my $_SRCFILE_=$U; $_SRCFILE_ =~ s/l$//;
    if (-f $_SRCFILE_) { 
	return $_SRCFILE_;
    } elsif (-f "$o{src_dir}/$_SRCFILE_") {
	return "$o{src_dir}/$_SRCFILE_";
    } else {
	return ".//$_SRCFILE_";
    }
}
sub html_printerfile_sourcefile 
{                   
    my ($U,$Z) = @_;
    if (not $printerfriendly) {
	$U =~ s/l\$//; return $U;
    } else {
	my $_ext_=&sed_slash_key(&print_extension($printerfriendly));
	$U =~ s/l\$//; $U =~ s/$_ext_([.][\w]*)$/$1/; return $U;
    }
}

sub fast_html_printerfile {
    my ($U,$V,$Z) = @_;
    my $x=&html_printerfile($U) ; return basename($x);
#   my $x=&html_printerfile($U) ; return &fast_href($x,$V);
}

sub html_printerfile # generate the printerfile for a given normal output
{
    my ($U,$Z) = @_;
    my $_ext_=&esc(&print_extension($printerfriendly));
    $U =~ s/([.][\w]*)$/$_ext_$1/; return $U; # index.html -> index.print.html
}

sub make_printerfile_fast # generate s/file.html/file.print.html/ for hrefs
{                        # we do that only for the $FILELIST
    my ($U,$Z) = @_;
    my $ALLPAGES=$U;
    my @OUT = ();
    for my $p (@$ALLPAGES) {
	my $a=&sed_slash_key($p);
	my $b=&html_printerfile($p);
	if ($b ne $p) {
	    $b =~ s:/:\\/:g;
	    push @OUT, 
	    "s/<a href=\\\"$a\\\"([^<>])*>/<a href=\\\"$b\\\"\$1>/;";
	}
    }
    return @OUT;
}

sub echo_printsitefile_style
{
    my $_bold_="text-decoration : none ; font-weight : bold ; ";
    return "   <style>"
	."$n     a:link    { $_bold_ color : #000060 ; }"
	."$n     a:visited { $_bold_ color : #000040 ; }"
	."$n     body      { background-color : white ; }"
	."$n   </style>"
	."$n";
}

sub make_printsitefile_head # $sitefile
{
    my $MK_STYLE = &echo_printsitefile_style();
    my @OUT = ();
    for (source($SITEFILE)) {
	if (/<head>/) {  push @OUT, $_; 
			 push @OUT, $MK_STYLE; next; }
	if (/<title>/) { push @OUT, $_; next; }
        if (/<\/head>/) { push @OUT, $_; next; }
	if (/<body>/) { push @OUT, $_; next; }
	if (/<link [^<>]*rel=\"shortcut icon\"[^<>]*>/) {
	    push @OUT, $_; next;
	}
    }
    return @OUT;
}

# ------------------------------------------------------------------------
# The printsitefile is a long text containing html href markups where
# each of the href lines in the file is being prefixed with the section
# relation. During a secondary call the printsitefile can grepp'ed for
# those lines that match a given output fast-file. The result is a
# navigation header with 1...3 lines matching the nesting level

# these alt-texts will be only visible in with a text-mode browser:
my $printsitefile_square="width=\"8\" height=\"8\" border=\"0\"";
my $printsitefile_img_1="<img alt=\"|go text:\" $printsitefile_square />";
my $printsitefile_img_2="<img alt=\"||topics:\" $printsitefile_square />";
my $printsitefile_img_3="<img alt=\"|||pages:\" $printsitefile_square />";
my $_SECT="mksite:sect:";

sub echo_current_line # $sect $extra
{
    # add the prefix which is used by select_in_printsitefile to cut out things
    my ($N,$M,$Z) = @_;
    return "<!--$_SECT\"$N\"-->$M";
}
sub make_current_entry # $sect $file      ## requires $MK_SITE
{
    my ($S,$R,$Z) = @_;
    my $RR=&sed_slash_key($R); 
    my $sep=" - " ; my $_left_=" [ " ; my $_right_=" ] ";
    my $name = site_get_list_title($R);
    $_ = &echo_current_line ("$S", "<a href=\"$R\">$name</a>$sep");
    if ($R eq $S) {
	s/<a href/$_left_$&/;
	s/<\/a>/$&$_right_/;
    }
    return $_;
}
sub echo_subpage_line # $sect $extra
{
    my ($N,$M,$Z) = @_;
    return "<!--$_SECT*:\"$N\"-->$M";
}

sub make_subpage_entry
{
    my ($S,$R,$Z) = @_;
    my $RR=&sed_slash_key($R);
    my $sep=" - " ;
    my $name = site_get_list_title($R);
    $_ = &echo_subpage_line ("$S", "<a href=\"$R\">$name</a>$sep");
    return $_;
}

sub make_printsitefile
{
   # building the printsitefile looks big but its really a loop over sects
    my ($INPUTS,$Z) = @_; $INPUTS=\@MK_INFO if not $INPUTS;
    @MK_SITE = &siteinfo2sitemap(); # have <name><long><date> addon-sed
    savelist(\@MK_SITE);
    my @OUT = &make_printsitefile_head ($SITEFILE);

    my $sep=" - " ;
    my $_sect1=
	"<a href=\"#.\" title=\"section\">$printsitefile_img_1</a> ||$sep";
    my $_sect2=
	"<a href=\"#.\" title=\"topics\">$printsitefile_img_2</a> ||$sep";
    my $_sect3=
	"<a href=\"#.\" title=\"pages\">$printsitefile_img_3</a> ||$sep";

    my $_SECT1="mksite:sect1";
    my $_SECT2="mksite:sect2";
    my $_SECT3="mksite:sect3";

    @MK_SECT1 = &site_get_rootsections();
    # round one - for each root section print a current menu
    for my $r (@MK_SECT1) {
	push @OUT, &echo_current_line ("$r", "<!--$_SECT1:A--><br>$_sect1");
	for my $s (@MK_SECT1) {
	    push @OUT, &make_current_entry ("$r", "$s");
	}
	push @OUT, &echo_current_line ("$r", "<!--$_SECT1:Z-->");
    }

    # round two - for each subsection print a current and subpage menu
    for my $r (@MK_SECT1) {
    @MK_SECT2 = &site_get_subpages ("$r");
    for my $s (@MK_SECT2) {
	push @OUT, &echo_current_line ("$s", "<!--$_SECT2:A--><br>$_sect2");
	for my $t (@MK_SECT2) {
	    push @OUT, &make_current_entry ("$s", "$t");
	} # "$t"
	push @OUT, &echo_current_line  ("$s", "<!--$_SECT2:Z-->");
    } # "$s"
	my $_have_children_="";
	for my $t (@MK_SECT2) {
	    if (not $_have_children_) {
	push @OUT, &echo_subpage_line ("$r", "<!--$_SECT2:A--><br>$_sect2"); }
	    $_have_children_ .= "1";
	    push @OUT, &make_subpage_entry ("$r", "$t");
	}
	    if ($_have_children_) {
	push @OUT, &echo_subpage_line ("$r", "<!--$_SECT2:Z-->"); }
    } # "$r"

    # round three - for each subsubsection print a current and subpage menu
    for my $r (@MK_SECT1) {
    @MK_SECT2 = &site_get_subpages ("$r");
    for my $s (@MK_SECT2) {
    @MK_SECT3 = &site_get_subpages ("$s");
    for my $t (@MK_SECT3) {
	push @OUT, &echo_current_line ("$t", "<!--$_SECT3:A--><br>$_sect3");
	for my $u (@MK_SECT3) {
	    push @OUT, &make_current_entry ("$t", "$u");
	} # "$t"
	push @OUT, &echo_current_line  ("$t", "<!--$_SECT3:Z-->");
    } # "$t"
	my $_have_children_="";
	for my $u (@MK_SECT3) {
	    if (not $_have_children_) {
	push @OUT, &echo_subpage_line ("$s", "<!--$_SECT3:A--><br>$_sect3"); }
	    $_have_children_ .= "1";
	    push @OUT, &make_subpage_entry ("$s", "$u");
	}
	    if ($_have_children_) {
	push @OUT, &echo_subpage_line ("$s", "<!--$_SECT3:Z-->"); }
    } # "$s"
    } # "$r"
    push @OUT, "<a name=\".\"></a>";
    push @OUT, "</body></html>";
    return @OUT;
}

# create a selector that can grep a printsitefile for the matching entries
sub select_in_printsitefile # arg = "page" : return to stdout >> $P.$HEAD
{
    my ($N,$Z) = @_;
    my $_selected_="$N" ; $_selected_="$F" if not $_selected_;
    my $_section_=&sed_slash_key($_selected_);
    my @OUT = ();
    push @OUT, "s/^<!--$_SECT\\\"$_section_\\\"-->//;";        # sect3
    push @OUT, "s/^<!--$_SECT\[*\]:\\\"$_section_\\\"-->//;";    # children
    $_selected_=&site_get_parentpage($_selected_);
    if ($F =~ /testscript/) { print "($F)parent=$_selected_$n"; }
    $_section_=&sed_slash_key($_selected_);
    push @OUT, "s/^<!--$_SECT\\\"$_section_\\\"-->//;";        # sect2
    $_selected_=&site_get_parentpage($_selected_);
    $_section_=&sed_slash_key($_selected_);
    push @OUT, "s/^<!--$_SECT\\\"$_section_\\\"-->//;";        # sect1
    push @OUT, "/^<!--$_SECT\\\"[^\\\"]*\\\"-->/ and next;";
    push @OUT, "/^<!--$_SECT\[*\]:\\\"[^\\\"]*\\\"-->/ and next;";
    push @OUT, "s/^<!--mksite:sect[$NN]:[$AZ]-->//;";
    return @OUT;
}

sub body_for_emailfooter
{
    return "" if $emailfooter eq "no";
    my $_email_=$emailfooter; $_email_ =~ s|[?].*||;
    my $_dated_=&info_get_entry("updated");
    return "<hr><table border=\"0\" width=\"100%\"><tr><td>"
	."$n"."<a href=\"mailto:$emailfooter\">$_email_</a>"
	."$n"."</td><td align=\"right\">"
	."$n"."$_dated_</td></tr></table>"
	."$n";
}

# ==========================================================================
#  
#  During processing we will create a series of intermediate files that
#  store relations. They all have the same format being
#   =relationtype=key value
#  where key is usually s filename or an anchor. For mere convenience
#  we assume that the source html text does not have lines that start
#  off with =xxxx= (btw, ye remember perl section notation...). Of course
#  any other format would be usuable as well.
#

# we scan the SITEFILE for href references to be converted
# - in the new variant we use a ".gets.tmp" sed script that            SECTS
# marks all interesting lines so they can be checked later
# with an sed anchor of <!--sect[$NN]--> (or <!--sect[$AZ]-->)
my $S="\\&nbsp\\;";
# S="[&]nbsp[;]"

# HR and EM style markups must exist in input - BR sometimes left out 
# these routines in(ter)ject hardspace before, between, after markups
# note that "<br>" is sometimes used with HR - it must exist in input
sub echo_HR_EM_PP
{
    my ($U,$V,$W,$X,$Z) = @_;
    my @list = (
		"/^$U$V$W*<a href=/   and s/^/$X/;",
		"/^<>$U$V$W*<a href=/ and s/^/$X/;",
		"/^$S$U$V$W*<a href=/ and s/^/$X/;",
		"/^$U<>$V$W*<a href=/ and s/^/$X/;",
		"/^$U$S$V$W*<a href=/ and s/^/$X/;",
		"/^$U$V<>$W*<a href=/ and s/^/$X/;",
		"/^$U$V$S$W*<a href=/ and s/^/$X/;" );
    return @list;
}

sub echo_br_EM_PP
{
    my ($U,$V,$W,$X,$Z) = @_;
    my @list = &echo_HR_EM_PP  ("$U", "$V", "$W", "$X");
    my @listt = (
		 "/^$V$W*<a href=/   and s/^/$X/;",
		 "/^<>$V$W*<a href=/ and s/^/$X/;",
		 "/^$S$V$W*<a href=/ and s/^/$X/;",
		 "/^$V<>$W*<a href=/ and s/^/$X/;",
		 "/^$V$S$W*<a href=/ and s/^/$X/;" );
    push @list, @listt;
    return @list;
}    

sub echo_HR_PP
{
    my ($U,$V,$W,$Z) = @_;
    my @list = (
		"/^$U$V*<a href=/   and s/^/$W/;",
		"/^<>$U$V*<a href=/ and s/^/$W/;",
		"/^$S$U$V*<a href=/ and s/^/$W/;",
		"/^$U<>$V*<a href=/ and s/^/$W/;",
		"/^$U$S$V*<a href=/ and s/^/$W/;" );
    return @list;
}
sub echo_br_PP
{
    my ($U,$V,$W,$Z) = @_;
    my @list = &echo_HR_PP ("$U", "$V", "$W");
    my @listt = (
		 "/^$V*<a href=/   and s/^/$W/;",
		 "/^<>$V*<a href=/ and s/^/$W/;",
		 "/^$S$V*<a href=/ and s/^/$W/;" );
    push @list, @listt;
    return @list;
}
sub echo_sp_PP
{
    my ($U,$V,$Z) = @_;
    my @list = (
		"/^<>$U*<a href=/ and s/^/$V/;",
		"/^$S$U*<a href=/ and s/^/$V/;",
		"/^<><>$U*<a href=/ and s/^/$V/;",
		"/^$S$S$U*<a href=/ and s/^/$V/;",
		"/^<>$U<>*<a href=/ and s/^/$V/;",
		"/^$S$U$S*<a href=/ and s/^/$V/;",
		"/^$U<><>*<a href=/ and s/^/$V/;",
		"/^$U$S$S*<a href=/ and s/^/$V/;",
		"/^$U<>*<a href=/ and s/^/$V/;",
		"/^$U$S*<a href=/ and s/^/$V/;" );
    return @list;
}
sub echo_sp_sp
{
    my ($U,$V,$Z) = @_;
    my @list = (
		"/^$U*<a name=/ and s/^/$V/;",
		"/^<>$U*<a name=/ and s/^/$V/;",
		"/^$S$U*<a name=/ and s/^/$V/;",
		"/^<><>$U*<a name=/ and s/^/$V/;",
		"/^$S$S$U*<a name=/ and s/^/$V/;",
		"/^<>$U<>*<a name=/ and s/^/$V/;",
		"/^$S$U$S*<a name=/ and s/^/$V/;",
		"/^$U<><>*<a name=/ and s/^/$V/;",
		"/^$U$S$S*<a name=/ and s/^/$V/;",
		"/^$U<>*<a name=/ and s/^/$V/;",
		"/^$U$S*<a name=/ and s/^/$V/;" );
    return @list;
}

sub make_sitemap_init
{
    # build a list of detectors that map site.htm entries to a section table
    # note that the resulting .gets.tmp / .puts.tmp are real sed-script
    my $h1="[-|[]";
    my $b1="[*=]";
    my $b2="[-|[]";
    my $b3="[\\/:]";
    my $q3="[\\/:,[]";
    @MK_GETS = ();
    push @MK_GETS, &echo_HR_PP   ("<hr>",            "$h1",    "<!--sect1-->");
    push @MK_GETS, &echo_HR_EM_PP("<hr>","<em>",     "$h1",    "<!--sect1-->");
    push @MK_GETS, &echo_HR_EM_PP("<hr>","<strong>", "$h1",    "<!--sect1-->");
    push @MK_GETS, &echo_HR_PP   ("<br>",          , "$b1$b1", "<!--sect1-->");
    push @MK_GETS, &echo_HR_PP   ("<br>",          , "$b2$b2", "<!--sect2-->");
    push @MK_GETS, &echo_HR_PP   ("<br>",          , "$b3$b3", "<!--sect3-->");
    push @MK_GETS, &echo_br_PP   ("<br>",          , "$b2$b2", "<!--sect2-->");
    push @MK_GETS, &echo_br_PP   ("<br>",          , "$b3$b3", "<!--sect3-->");
    push @MK_GETS, &echo_br_EM_PP("<br>","<small>" , "$q3"   , "<!--sect3-->");
    push @MK_GETS, &echo_br_EM_PP("<br>","<em>"    , "$q3"   , "<!--sect3-->");
    push @MK_GETS, &echo_br_EM_PP("<br>","<u>"     , "$q3"   , "<!--sect3-->");
    push @MK_GETS, &echo_HR_PP   ("<br>",          , "$q3"   , "<!--sect3-->");
    push @MK_GETS, &echo_sp_PP   (                   "$q3"   , "<!--sect3-->");
    push @MK_GETS, &echo_sp_sp   (                   "$q3"   , "<!--sect9-->");
    @MK_PUTS = map { my $x=$_; $x =~ s/(>)(\[)/$1 *$2/; $x } @MK_GETS;
    # the .puts.tmp variant is used to <b><a href=..></b> some hrefs which
    # shall not be used otherwise for being generated - this is nice for
    # some quicklinks somewhere. The difference: a whitspace "<hr> <a...>"
}

my $_uses_= sub{"=use$1=$2 $3" }; my $_name_= sub{"=use$1=name:$2 $3" }; 
my $_getW_="<!--sect([$NN])-->";
my $_getX_="<!--sect([$NN])--><[^<>]*>[^<>]*";
my $_getY_="<!--sect([$NN])--><[^<>]*>[^<>]*<[^<>]*>[^<>]*";

sub make_sitemap_list
{
    # scan sitefile for references pages - store as "=use+=href+ anchortext"
    for (source($SITEFILE)) {
#	print join("$n;",@MK_GETS),$n;
	$_ = &eval_MK_LIST($_, @MK_GETS);
	/^<!--sect[$NN]-->/ or next;
	s{^$_getX_<a href=\"([^\"]*)\"[^<>]*>(.*)</a>.*}{&$_uses_}e;
	s{^$_getY_<a href=\"([^\"]*)\"[^<>]*>(.*)</a>.*}{&$_uses_}e;
	s{^$_getW_<a name=\"([^\"]*)\"[^<>]*>(.*)</a>.*}{&$_name_}e;
	s{^$_getX_<a name=\"([^\"]*)\"[^<>]*>(.*)</a>.*}{&$_name_}e;
	s{^$_getY_<a name=\"([^\"]*)\"[^<>]*>(.*)</a>.*}{&$_name_}e;
	/^=....=/ or next;
	push @MK_INFO, $_;
    }
}

sub make_sitemap_sect
{
    # scan used pages and store prime section group relation =sect= and =node=
    # (A) each "use1" creates "=sect=href+ href1" for all following non-"use1"
    # (B) each "use1" creates "=node=href2 href1" for all following "use2"
    my $sect = "";
    for (grep {/=[u]se.=/} @MK_INFO) {
	if (/=[u]se1=([^ ]*) .*/) { $sect = $1; }
	my $x = $_; $x =~ s/=[u]se.=([^ ]*) .*/=sect=$1/; chomp $x;
	push @MK_INFO, "$x $sect";
    }
    for (grep {/=[u]se.=/} @MK_INFO) {
	if (/=[u]se1=([^ ]*) .*/) { $sect = $1; }
	/=[u]se[13456789]=/ and next;
	my $x = $_; $x =~ s/=[u]se.=([^ ]*) .*/=node=$1/; chomp $x;
	push @MK_INFO, "$x $sect";
    }
}

sub make_sitemap_page
{
    # scan used pages and store secondary group relation =page= and =node=
    # the parenting =node= for use3 is usually a use2 (or use1 if none there)
    my $sect = "";
    for (grep {/=[u]se.=/} @MK_INFO) {
	if (/=[u]se1=([^ ]*) .*/) { $sect = $1; }
	if (/=[u]se2=([^ ]*) .*/) { $sect = $1; }
	/=[u]se[1]=/ and next;
	my $x = $_; $x =~ s/=[u]se.=([^ ]*) .*/=page=$1/; chomp $x;
	push @MK_INFO, "$x $sect";
    }
    for (grep {/=[u]se.=/} @MK_INFO) {
	if (/=[u]se1=([^ ]*) .*/) { $sect = $1; }
	if (/=[u]se2=([^ ]*) .*/) { $sect = $1; }
	/=[u]se[12456789]=/ and next; 
	my $x = $_; $x =~ s/=[u]se.=([^ ]*) .*/=node=$1/; chomp $x;
	push @MK_INFO, "$x $sect"; print "(",$_,")","$x $sect", $n;
    }
    # and for the root sections we register ".." as the parenting group
    for (grep {/=[u]se1=/} @MK_INFO) {
	my $x = $_; $x =~ s/=[u]se.=([^ ]*) .*/=node=$1/; chomp $x;
	push @MK_INFO, trimm("$x ..");
    }
}
sub echo_site_filelist
{
    my @OUT = ();
    for (grep {/=[u]se.=/} @MK_INFO) {
	my $x = $_; $x =~ s/=[u]se.=//; $x =~ s/ .*[\n]*//; 
	push @OUT, $x;
    }
    return @OUT;
}

# ==========================================================================
# originally this was a one-pass compiler but the more information
# we were scanning out the more slower the system ran - since we
# were rescanning files for things like section information. Now
# we scan the files first for global information.
#                                                                    1.PASS

sub scan_sitefile # $F
{
    $SOURCEFILE=&html_sourcefile($F);
    if ($SOURCEFILE ne $F) {
	dx_init "$F";
	dx_text ("today", &timetoday());
	my $short=$F; 
	$short =~ s:.*/::; $short =~ s:[.].*::; # basename for all exts
	$short .=" ~";
	DC_meta ("title", "$short");
	DC_meta ("date.available", &timetoday());
	DC_meta ("subject", "sitemap");
	DC_meta ("DCMIType", "Collection");
	DC_VARS_Of ($SOURCEFILE) ; HTTP_VARS_Of ($SOURCEFILE) ;
	DC_modified ($SOURCEFILE) ; DC_date ($SOURCEFILE);
	DC_section ($F);
	DX_text ("date.formatted", &timetoday());
	if ($printerfriendly) {
	    DX_text ("printerfriendly", fast_html_printerfile($F)); }
	if ($ENV{USER}) { DC_publisher ($ENV{USER}); }
	print "'$SOURCEFILE': $short (sitemap)$n";
	site_map_list_title ($F, "$short");
	site_map_long_title ($F, "generated sitemap index");
	site_map_list_date  ($F, &timetoday());
    }
}

sub scan_htmlfile # "$F"
{
    my ($FF,$Z) = @_;
    $SOURCEFILE=&html_sourcefile($F);                                  # SCAN :
    if ($SOURCEFILE ne $F) {                                           # HTML :
    if ( -f $SOURCEFILE) {
	@{$FAST{$F}} = &make_fast ($F);
	dx_init "$F";
	dx_text ("today", &timetoday());
	dx_text ("todays", &timetodays());
	DC_VARS_Of ($SOURCEFILE); HTTP_VARS_Of ($SOURCEFILE);
	DC_title ($SOURCEFILE);
	DC_isFormatOf ($SOURCEFILE);
	DC_modified ($SOURCEFILE);
	DC_date ($SOURCEFILE); DC_date ($SITEFILE);
	DC_section ($F);  DC_selected ($F);  DX_alternative ($SOURCEFILE);
	if ($ENV{USER}) { DC_publisher ($ENV{USER}); }
	DX_text ("date.formatted", &timetoday());
	if ($printerfriendly) {
	    DX_text ("printerfriendly", fast_html_printerfile($F)); }
	my $sectn=&info_get_entry("DC.relation.section");
	my $short=&info_get_entry("DC.title.selected");
	&site_map_list_title ($F, "$short");
	&info_map_list_title ($F, "$short");
	my $title=&info_get_entry("DC.title");
	&site_map_long_title ($F, "$title");
	&info_map_long_title ($F, "$title");
	my $edate=&info_get_entry("DC.date");
	my $issue=&info_get_entry("issue");
	&site_map_list_date ($F, "$edate");
	&info_map_list_date ($F, "$edate");
	print "'$SOURCEFILE':  '$title' ('$short') @ '$issue' ('$sectn')$n";
    }else {
	print "'$SOURCEFILE': does not exist$n";
	site_map_list_title ($F, "$F");
	site_map_long_title ($F, "$F (no source)");
    } 
    } else {
	print "<$F> - skipped - ($SOURCEFILE)$n";
    }
}

sub scan_namespec 
{
    # nothing so far
    # my ($F,$ZZZ) = @_;
    if ($F =~ /^name:sitemap:/) {
	my $short=$F; 
	$short =~ s:.*/::; $short =~ s:[.].*::; # basename for all exts
	$short =~ s/name:sitemap://;
	$short .=" ~";
	site_map_list_title ($F, "$short");
	site_map_long_title ($F, "external sitemap index");
	site_map_list_date  ($F, &timetoday());
	print "'$F' external sitemap index$n";
    }
}
sub scan_httpspec
{
    # nothing so far
}

sub skip_namespec 
{
    # nothing so far
}
sub skip_httpspec
{
    # nothing so far
}

# ==========================================================================
# and now generate the output pages
#                                                                   2.PASS

sub head_sed_sitemap # $filename $section
{
    my ($U,$V,$Z) = @_;
    my $FF=&sed_slash_key($U);
    my $SECTION=&sed_slash_key($V);
    my $SECTS="<!--sect[$NN$AZ]-->" ; 
    my $SECTN="<!--sect[$NN]-->"; # lines with hrefs
    my @OUT = ();
    push @OUT, "/^$SECTS.*<a href=\\\"$FF\\\">/ and s|</a>|</a></b>|;";
    push @OUT, "/^$SECTS.*<a href=\\\"$FF\\\">/ and s|<a href=|<b><a href=|;";
    push @OUT, "/ href=\\\"$SECTION\\\"/ "
	."and s|^<td class=\\\"[^\\\"]*\\\"|<td |;" if $sectiontab ne "no";
    return @OUT;
}

sub head_sed_listsection # $filename $section
{
   # traditional.... the sitefile is the full navigation bar
    my ($U,$V,$Z) = @_;
    my $FF=&sed_slash_key($U);
    my $SECTION=&sed_slash_key($V);
    my $SECTS="<!--sect[$NN$AZ]-->" ; 
    my $SECTN="<!--sect[$NN]-->"; # lines with hrefs
    my @OUT = ();
    push @OUT, "/^$SECTS.*<a href=\\\"$FF\\\">/ and s|</a>|</a></b>|;";
    push @OUT, "/^$SECTS.*<a href=\\\"$FF\\\">/ and s|<a href=|<b><a href=|;";
    push @OUT, "/ href=\\\"$SECTION\\\"/ "
	."and s|^<td class=\\\"[^\\\"]*\\\"|<td |;" if $sectiontab ne "no";
    return @OUT;
}

sub head_sed_multisection # $filename $section
{
    # sitefile navigation bar is split into sections
    my ($U,$V,$Z) = @_;
    my $FF=&sed_slash_key($U);
    my $SECTION=&sed_slash_key($V);
    my $SECTS="<!--sect[$NN$AZ]-->" ; 
    my $SECTN="<!--sect[$NN]-->"; # lines with hrefs
    my @OUT = ();
    # grep all pages with a =sect= relation to current $SECTION and
    # build foreach an sed line "s|$SECTS\(<a href=$F>\)|<!--sectX-->\1|"
    # after that all the (still) numeric SECTNs are deactivated / killed.
    for my $section ($SECTION, $headsection, $tailsection) {
	next if $section eq "no";
	for (grep {/^=sect=[^ ]* $section/} @MK_INFO) {
	    my $x = $_;
	    $x =~ s, .*,\\\"\)|<!--sectX-->\$1|,;
	    $x =~ s,^=sect=,s|^$SECTS\(.*<a href=\\\",;
	    push @OUT, $x.";";
	}
    }
    push @OUT, "s|^$SECTN\[^ \]*(<a href=[^<>]*>).*|<!-- \$1 -->|;";
    push @OUT, "/^$SECTS.*<a href=\\\"$FF\\\">/ and s|</a>|</a></b>|;";
    push @OUT, "/^$SECTS.*<a href=\\\"$FF\\\">/ and s|<a href=|<b><a href=|;";
    push @OUT, "/ href=\\\"$SECTION\\\"/ "
	."and s|^<td class=\\\"[^\\\"]*\\\"|<td |;" if $sectiontab ne "no";
    return @OUT;
}

sub make_sitefile # "$F"
{
    $SOURCEFILE=&html_sourcefile($F); 
 if ($SOURCEFILE ne $F) { 
 if (-f $SOURCEFILE) {
   # remember that in this case "${SITEFILE}l" = "$F" = "${SOURCEFILE}l"
   @MK_VARS = &info2vars_sed();           # have <!--title--> vars substituted
   @MK_META = &info2meta_sed();           # add <meta name="DC.title"> values
   if ( $simplevars eq "warn") {
       @MK_TEST = &info2test_sed();       # check <!--title--> vars old-style
##       $SED_LONGSCRIPT ./$MK_TEST $SOURCEFILE | tee -a ./$MK_OLDS ; fi
   }
   my @F_HEAD = (); my @F_FOOT = ();
   push @F_HEAD, @MK_PUTS;
   push @F_HEAD, &head_sed_sitemap ($F, &info_get_entry_section());
   push @F_HEAD, "/<head>/ and $sed_add join(\"\\n\", \@MK_META);";
   push @F_HEAD, @MK_VARS; push @F_HEAD, @MK_TAGS; 
   push @F_HEAD, "/<\\/body>/ and next;";                #cut lastline
   if ( $sitemaplayout eq "multi") {
       push @F_FOOT,  &make_multisitemap();        # here we use ~foot~ to
   } else {
       push @F_FOOT,  &make_listsitemap();         # hold the main text
   }

   my $html = ""; # 
   $html .= &eval_MK_FILE($SITEFILE, @F_HEAD);
   $html .= join("", @F_FOOT);
   for (source($SITEFILE)) {
       /<\/body>/ or next;
       $html .= &eval_MK_LIST($_, @MK_VARS);
   }
  open F, ">$F"; print F $html; close F;
  print "'$SOURCEFILE': ",ls_s($SOURCEFILE)," >-> ",ls_s($F),"$n";
   savesource("$F.~head~", \@F_HEAD);
   savesource("$F.~foot~", \@F_FOOT);
} else {
    print "'$SOURCEFILE': does not exist$n";
} }
}

sub make_htmlfile # "$F"
{
    $SOURCEFILE=&html_sourcefile($F);                      #     2.PASS
 if ("$SOURCEFILE" ne "$F") {
 if (-f "$SOURCEFILE") {
    if (grep {/<meta name="formatter"/} source($SOURCEFILE)) {
      print "$SOURCEFILE: SKIP, this sourcefile looks like a formatted file$n";
      print "$SOURCEFILE:  (may be a sourcefile in place of a targetfile?)$n";
    return; }
    @MK_VARS = &info2vars_sed();           # have <!--title--> vars substituted
    @MK_META = &info2meta_sed();           # add <meta name="DC.title"> values
    if ( $simplevars eq "warn") {
        @MK_TEST = &info2test_sed();       # check <!--title--> vars old-style
##       $SED_LONGSCRIPT ./$MK_TEST $SOURCEFILE | tee -a ./$MK_OLDS ; fi
    }
    my @F_HEAD = (); my @F_BODY = (); my $F_FOOT = "";
    push @F_HEAD, @MK_PUTS;
    if ( $sectionlayout eq "multi") {
	push @F_HEAD, &head_sed_multisection ($F, &info_get_entry_section());
    } else {
	push @F_HEAD, &head_sed_listsection ($F, &info_get_entry_section());
    }
    push @F_HEAD, @MK_VARS; push @F_HEAD, @MK_TAGS;         #tag and vars
    push @F_HEAD, "/<\\/body>/ and next;";                #cut lastline
    push @F_HEAD, "/<head>/ and $sed_add join(\"\\n\",\@MK_META);"; #add metatags
    push @F_BODY, "/<title>/ and next;";                  #not that line
    push @F_BODY, @MK_VARS; push @F_BODY, @MK_TAGS;         #tag and vars
    push @F_BODY, &bodymaker_for_sectioninfo();             #if sectioninfo
    push @F_BODY, &info2body_sed();                         #cut early
    push @F_HEAD, &info2head_sed();
    push @F_HEAD, @{$FAST{$F}}; 
    if ($emailfooter ne "no") {
	$F_FOOT = &body_for_emailfooter();
    }
    my $html = "";
    $html .= eval_MK_FILE($SITEFILE, @F_HEAD);
    $html .= eval_MK_FILE($SOURCEFILE, @F_BODY);
    $html .= $F_FOOT;
    for (source($SITEFILE)) {
	/<\/body>/ or next;
	$_ = &eval_MK_LIST($_, @MK_VARS);
	$html .= $_;
    }
    savelist(\@{$INFO{$F}});
   open F, ">$F" or die "could not write $F: $!"; print F $html; close F;
   print "'$SOURCEFILE': ",&ls_s($SOURCEFILE)," -> ",&ls_s($F),"$n";
    savesource("$F.~head~", \@F_HEAD);
    savesource("$F.~body~", \@F_BODY);
 } else {
     print "'$SOURCEFILE': does not exist$n";
 }} else {
     print "<$F> - skipped$n";
 }
}

my $PRINTSITEFILE;
sub make_printerfriendly # "$F"
{                                                                 # PRINTER
    my $printsitefile="0";                                        # FRIENDLY
    my @F_FAST = (); my $BODY_TXT; my $BODY_SED;
    my $P=&html_printerfile ($F);
    my @P_HEAD = (); my @P_BODY = ();
    if ("$F" =~ /^(${SITEFILE}|${SITEFILE}l)$/) {
	@F_FAST = &make_fast ("$F");
	$printsitefile=">=>" ; $BODY_TXT="$F.~foot~" ; 
    } elsif ("$F" =~ /^(.*[.]html)$/) {
	$printsitefile="=>" ;  $BODY_TXT="$SOURCEFILE";
    }
    if (grep {/<meta name="formatter"/} source($BODY_TXT)) { return; }
    if ($printsitefile ne "0" and -f $SOURCEFILE) {
      @MK_FAST = &make_printerfile_fast (\@FILELIST);
      push @P_HEAD, @MK_VARS; push @P_HEAD, @MK_TAGS; push @P_HEAD, @MK_FAST;
      @MK_METT = map { my $x = $_; $x =~
      /DC.relation.isFormatOf/ and $x =~ s|content=\"[^\"]*\"|content=\"$F\"| ;
	  $x } @MK_META;
      push @P_HEAD, "/<head>/ and $sed_add join(\"\\n\", \@MK_METT);";
      push @P_HEAD, "/<\\/body>/ and next;";
      push @P_HEAD, &select_in_printsitefile ("$F");
      my $_ext_=&print_extension($printerfriendly);
      push @P_HEAD, map { my $x=$_; $x =~ s/[.]html\"|/$_ext_$&/g; $x} @F_FAST;
#     my $line_=&sed_slash_key($printsitefile_img_2);
      push @P_HEAD, "/\\|\\|topics:/"
	  ." and s| href=\\\"\\#\\.\\\"| href=\\\"$F\\\"|;";
      push @P_HEAD, "/\\|\\|\\|pages:/"
	  ." and s| href=\\\"\\#\\.\\\"| href=\\\"$F\\\"|;";
      push @P_HEAD, @F_FAST;
      push @P_BODY, @MK_VARS; push @P_BODY, @MK_TAGS; push @P_BODY, @MK_FAST;
      push @P_BODY, map { my $x=$_; $x =~ s/[.]html\"|/$_ext_$&/g; $x} @F_FAST;
      push @P_BODY, @F_FAST;
      my $html = "";
      $html .= eval_MK_FILE($PRINTSITEFILE, @P_HEAD);
      $html .= eval_MK_FILE($BODY_TXT, @P_BODY);
      for (source($PRINTSITEFILE)) {
	  /<\/body>/ or next;
	  $_ = &eval_MK_LIST($_, @MK_VARS);
	  $html .= $_;
      }
      open P, ">$P" or die "could not write $P: $!"; print P $html; close P;
      print "'$SOURCEFILE': ",ls_s($SOURCEFILE)," $printsitefile ",ls_s($P),"$n";
  }
}


# ========================================================================
# ========================================================================
# ========================================================================
# ========================================================================
#                                                          #### 0. INIT
$F=$SITEFILE;
&make_sitemap_init();
&make_sitemap_list();
&make_sitemap_sect();
&make_sitemap_page();
savelist(\@MK_INFO);

@FILELIST=&echo_site_filelist();
if ($o{filelist} or $o{list} eq "file" or $o{list} eq "files") {
    for (@FILELIST) { print $_,"$n";  } exit; # --filelist
}
if ($o{files}) { @FILELIST=split(/ /, $o{files}); } # --files
if ($#FILELIST < 0) { print STDERR "nothing to do$n"; }
if ($#FILELIST == 0 and 
    $FILELIST[0] eq $SITEFILE) { print STDERR "only '$SITEFILE'!?$n"; }

for (@FILELIST) {                                    #### 1. PASS
    $F = $_;
    if (/^(name:.*)$/) { 
	&scan_namespec ("$F"); 
    } elsif (/^(http:.*|.*:\/\/.*)$/) { 
	&scan_httpspec ("$F"); 
    } elsif (/^(${SITEFILE}|${SITEFILE}l)$/) {
	&scan_sitefile ("$F") ;;                      # ........... SCAN SITE
    } elsif (/^(\.\.\/.*)$/) { 
	print "!! -> '$F' (skipping topdir build)$n";
# */*.html) 
#    make_fast  # try for later subdir build
#    echo "!! -> '$F' (skipping subdir build)"
#    ;;
# */*/*/|*/*/|*/|*/index.htm|*/index.html) 
#    echo "!! -> '$F' (skipping subdir index.html)"
#    ;;
    } elsif (/^(.*\.html)$/) {
	&scan_htmlfile ("$F") ;;                      # ........... SCAN HTML
    } elsif (/^(.*\/)$/) {
	print "'$F' : directory - skipped$n";
	&site_map_list_title ("$F", &sed_slash_key($F));
	&site_map_long_title ("$F", "(directory)");
    } else {
	print "?? -> '$F'$n";
    }
}

if ($printerfriendly) {                            # .......... PRINT VERSION
    my $_ext_=esc(&print_extension($printerfriendly));
    $PRINTSITEFILE=$SITEFILE; $PRINTSITEFILE =~ s/(\.\w*)$/$_ext_$1/;
    $F=$PRINTSITEFILE;
    my @TEXT = &make_printsitefile();
    print "NOTE: going to create printer-friendly sitefile '$PRINTSITEFILE'"
	." $F._$i$n";
    savelist(\@TEXT);
    my @LINES = map { chomp; $_."$n" } @TEXT;
    savesource($PRINTSITEFILE, \@LINES);
    if (1) {
	if (open PRINTSITEFILE, ">$PRINTSITEFILE") {
	    print PRINTSITEFILE join("", @LINES); close PRINTSITEFILE;
	}
    }
}

if ($simplevars eq " ") {
    @MK_OLDS = ();
}

for (@FILELIST) {                                          #### 2. PASS
  $F = $_;
  if (/^(name:.*)$/) { 
      &skip_namespec ("$F") ;;
  } elsif (/^(http:.*|.*:\/\/.*)$/) { 
      &skip_httpspec ("$F") ;;
  } elsif (/^(${SITEFILE}|${SITEFILE}l)$/) {
      &make_sitefile ("$F") ;;                         # ........ SITE FILE
      &make_printerfriendly ("$F") if ($printerfriendly);
  } elsif (/^(\.\.\/.*)$/) {
      print "!! -> '$F' (skipping topdir build)$n";
# */*.html) 
#   echo "!! -> '$F' (skipping subdir build)"
#   ;;
# */*/*/|*/*/|*/|*/index.htm|*/index.html) 
#   echo "!! -> '$F' (skipping subdir index.html)"
#   ;;
  } elsif (/^(.*\.html)$/) {
      &make_htmlfile ("$F") ;;               # .................. HTML FILES
      &make_printerfriendly ("$F") if ($printerfriendly);
  } elsif (/^(.*\/)$/) {
      print "'$F' : directory - skipped$n";
  } else {
      print "?? -> '$F'$n";
  }
# .............. debug ....................
##   if test -d DEBUG && test -f "./$F" ; then
##      cp ./$F.$INFO DEBUG/$F.info.TMP
##      for P in tags vars meta page date list html sect info ; do
##      test -f ./$MK.$P.tmp && cp ./$MK.$P.tmp DEBUG/$F.$P.tmp
##      test -f ./$MK.$P.TMP && cp ./$MK.$P.TMP DEBUG/$F.$P.TMP
##      done
##   fi
} # done

if ( $simplevars eq "warn") {
    my $oldvars = $#MK_OLDS; $oldvars ++;
    if (not $oldvars) {
print "HINT: you have no simplevars in your htm sources, so you may want to$n";
print "hint: set the magic <!--mksite:nosimplevars--> in your $SITEFILE$n";
print "hint: which makes execution _faster_ actually in the 2. pass$n";
print "note: simplevars expansion was the oldstyle way of variable expansion$n";
} else {
print "HINT: there were $oldvars simplevars found in your htm sources.$n";
print "hint: This style of variable expansion will be disabled in the near$n";
print "hint: future. If you do not want change then add the $SITEFILE magic$n";
print "hint: <!--mksite:simplevars--> somewhere to suppress this warning$n";
print "note: simplevars expansion will be an explicit option in the future.$n";
print "note: errornous simplevar detection can be suppressed with a magic$n";
print "note: hint of <!--mksite:nosimplevars--> in the $SITEFILE for now.$n";
} }

## rm ./$MK.*.tmp
exit 0
