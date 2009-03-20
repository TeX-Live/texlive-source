#! /bin/sh
# this is the sh/sed variant of the mksite script. It is largely
# derived from snippets that I was using to finish doc pages for 
# website publishing. For the mksite project the functionaliy has
# been expanded of course. Still this one does only use simple unix
# commands like sed, date, and test. And it still works. :-)=)
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
# $Id: mksite.sh,v 1.4 2005/05/14 15:08:23 guidod Exp $

# Zsh is not Bourne compatible without the following: (seen in autobook)
if test -n "$ZSH_VERSION"; then
  emulate sh
  NULLCMD=:
fi

# initialize some defaults
test ".$SITEFILE" = "." && test -f "site.htm"  && SITEFILE="site.htm"
test ".$SITEFILE" = "." && test -f "site.html" && SITEFILE="site.html"
test ".$SITEFILE" = "." && SITEFILE="site.htm"
MK="-mksite"     # note the "-" at the start
SED="sed"
CAT="cat"        # "sed -e n" would be okay too
GREP="grep"
DATE_NOW="date"  # should be available on all posix systems
DATE_R="date -r" # gnu date has it / solaris date not
STAT_R="stat"    # gnu linux
LS_L="ls -l"     # linux uses one less char than solaris

INFO="~~"     # extension for meta data files
HEAD="~head~" # extension for head sed script
BODY="~body~" # extension for body sed script
FOOT="~foot~" # append to body text (non sed)
FAST="~move~" # extension for printer friendly sed

NULL="/dev/null"                             # to divert stdout/stderr
CATNULL="$CAT $NULL"                         # to create 0-byte files
SED_LONGSCRIPT="$SED -f"

LOWER="abcdefghijklmnopqrstuvwxyz"
UPPER="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
az="$LOWER"                                  # some old sed tools can not
AZ="$UPPER"                                  # use char-ranges in the 
NN="0123456789"                              # match expressions so that
AA="_$NN$AZ$az"                              # we use their unrolled
AX="$AA.+-"                                  # definition here
if $SED -V 2>$NULL | $GREP "GNU sed" >$NULL ; then
az="a-z"                                     # but if we have GNU sed
AZ="A-Z"                                     # then we assume there are
NN="0-9"                                     # char-ranges available
AA="_$NN$AZ$az"                              # that makes the resulting
AX="$AA.+-"                                  # script more readable
elif uname -s | $GREP HP-UX >$NULL ; then
SED_LONGSCRIPT="sed_longscript"              # due to 100 sed lines limit
fi

LANG="C" ; LANGUAGE="C" ; LC_COLLATE="C"     # these are needed for proper
export LANG LANGUAGE LC_COLLATE              # lowercasing as some collate
                                             # treat A-Z to include a-z

# ==========================================================================
# reading options from the command line                            GETOPT
opt_variables="files"
opt_fileseparator="?"
opt_files=""
opt_main_file=""
opt_formatter="$0"
opt=""
for arg in "$@"        # this variant should allow to embed spaces in $arg
do if test ".$opt" != "." ; then
      eval "export opt_$opt='$arg'"
      opt=""
   else
      case "$arg" in
      -*=*) 
         opt=`echo "$arg" | $SED -e "s/-*\\([$AA][$AA-]*\\).*/\\1/" -e y/-/_/`
         if test ".$opt" = "." ; then
            echo "ERROR: invalid option $arg" >&2
         else
            arg=`echo "$arg" | $SED -e "s/^[^=]*=//"`
            eval "export opt_$opt='$arg'"
	    opt_variables="$opt_variables $opt"
         fi
         opt="" ;;
      -*-*)   
         opt=`echo "$arg" | $SED -e "s/-*\\([$AA][$AA-]*\\).*/\\1/" -e y/-/_/`
         if test ".$opt" = "." ; then
            echo "ERROR: invalid option $arg" >&2
            opt=""
         else :
            # keep the option for next round
         fi ;;
      -*)  
         opt=`echo "$arg" | $SED -e "s/^-*\\([$AA][$AA-]*\\).*/\\1/" -e y/-/_/`
         if test ".$opt" = "." ; then
            echo "ERROR: invalid option $arg" >&2
         else
            arg=`echo "$arg" | $SED -e "s/^[^=]*=//"`
            eval "export opt_$opt=' '"
         fi
         opt="" ;;
      *) if test ".$opt_main_file" = "." ; then opt_main_file="$arg" ; else
         test ".$opt_files" != "." && opt_files="$opt_files$opt_fileseparator"
         opt_files="$opt_files$arg" ; fi
         opt="" ;;
      esac
   fi
done ; if test ".$opt" != "." ; then
      eval "export opt_$opt='$arg'"
      opt=""
fi
### env | grep ^opt

test ".$opt_main_file" != "." && test -f "$opt_main_file" && \
SITEFILE="$opt_main_file"
test ".$opt_site_file" != "." && test -f "$opt_site_file" && \
SITEFILE="$opt_site_file"

if test ".$opt_help" != "." ; then
    F="$SITEFILE"
    echo "$0 [sitefile]";
    echo "  default sitefile = $F";
    echo "options:";
    echo " --filelist : show list of target files as ectracted from $F"
    echo " --src-dir xx : if source files are not where mksite is executed"
    echo " --tmp-dir xx : use temp instead of local directory"
    echo " --tmp : use automatic temp directory in ${TEMP-/tmp}/mksite.*"
    exit;
    echo " internal:"
    echo "--fileseparator=x : for building the internal filelist (default '?')"
    echo "--files xx : for list of additional files to be processed"
    echo "--main-file xx : for the main sitefile to take file list from"
fi

if test ".$SITEFILE" = "." ; then
   echo "error: no SITEFILE found (default would be 'site.htm')"
   exit 1
else
   echo "NOTE: sitefile: `ls -s $SITEFILE`"
fi

if "${SHELL-/bin/sh}" -c 'foo () { exit 0; }; foo' 2>$NULL ; then : ; else
echo "!! sorry, this shell '$SHELL' does not support shell functions"; exit 1
fi

tmp="." ; if test ".$opt_tmp_dir" != "." ; then tmp="$opt_tmp_dir" ; fi
if test ".$opt_tmp_dir" = "." && test ".$opt_tmp" != "." ; then
tmp="${TEMP-/tmp}/mksite.$$" ; fi

# we use external files to store mappings - kind of relational tables
MK_TAGS="$tmp/$MK.tags.tmp"
MK_VARS="$tmp/$MK.vars.tmp"
MK_META="$tmp/$MK.meta.tmp"
MK_METT="$tmp/$MK.mett.tmp"
MK_TEST="$tmp/$MK.test.tmp"
MK_FAST="$tmp/$MK.fast.tmp"
MK_GETS="$tmp/$MK.gets.tmp"
MK_PUTS="$tmp/$MK.puts.tmp"
MK_OLDS="$tmp/$MK.olds.tmp"
MK_SITE="$tmp/$MK.site.tmp"
MK_SECT1="$tmp/$MK.sect1.tmp"
MK_SECT2="$tmp/$MK.sect2.tmp"
MK_SECT3="$tmp/$MK.sect3.tmp"
MK_STYLE="$tmp/$MK.style.tmp"
MK_INFO="$tmp/$MK.$INFO"

# ========================================================================
# ========================================================================
# ========================================================================
#                                                             MAGIC VARS
#                                                            IN $SITEFILE
printerfriendly=""
sectionlayout="list"
sitemaplayout="list"
simplevars="warn"      # <!--varname-->default
attribvars=" "         # <x ref="${varname:=default}">
updatevars=" "         # <!--$varname:=-->default
expandvars=" "         # <!--$varname-->
commentvars=" "        # $updatevars && $expandsvars && $simplevars
sectiontab=" "         # highlight ^<td class=...>...href="$section"
currenttab=" "         # highlight ^<br>..<a href="$topic">
headsection="no"
tailsection="no"
sectioninfo="no"       # using <h2> title <h2> = info text
emailfooter="no"

if $GREP "<!--multi-->"               $SITEFILE >$NULL ; then
echo \
"WARNING: do not use <!--multi-->, change to <!--mksite:multi--> " "$SITEFILE"
echo \
"warning: or <!--mksite:multisectionlayout--> <!--mksite:multisitemaplayout-->"
sectionlayout="multi"
sitemaplayout="multi"
fi
if $GREP "<!--mksite:multi-->"               $SITEFILE >$NULL ; then
sectionlayout="multi"
sitemaplayout="multi"
fi
if $GREP "<!--mksite:multilayout-->"         $SITEFILE >$NULL ; then
sectionlayout="multi"
sitemaplayout="multi"
fi

mksite_magic_option ()
{
    # $1 is word/option to check for
    INP="$2" ; test ".$INP" = "." && INP="$SITEFILE"
    $SED \
      -e "s/\\(<!--mksite:\\)\\($1\\)-->/\\1\\2: -->/g" \
      -e "s/\\(<!--mksite:\\)\\([$AA][$AA]*\\)\\($1\\)-->/\\1\\3:\\2-->/g" \
      -e "/<!--mksite:$1:/!d" \
      -e "s/.*<!--mksite:$1:\\([^<>]*\\)-->.*/\\1/" \
      -e "s/.*<!--mksite:$1:\\([^-]*\\)-->.*/\\1/" \
      -e "/<!--mksite:$1:/d" -e q $INP # $++
}

x=`mksite_magic_option sectionlayout` ; case "$x" in
       "list"|"multi") sectionlayout="$x" ;; esac
x=`mksite_magic_option sitemaplayout` ; case "$x" in
       "list"|"multi") sitemaplayout="$x" ;; esac
x=`mksite_magic_option simplevars` ; case "$x" in
      " "|"no"|"warn") simplevars="$x" ;; esac
x=`mksite_magic_option attribvars` ; case "$x" in
      " "|"no"|"warn") attribvars="$x" ;; esac
x=`mksite_magic_option updatevars` ; case "$x" in
      " "|"no"|"warn") updatevars="$x" ;; esac
x=`mksite_magic_option expandvars` ; case "$x" in
      " "|"no"|"warn") expandvars="$x" ;; esac
x=`mksite_magic_option commentvars` ; case "$x" in
      " "|"no"|"warn") commentvars="$x" ;; esac
x=`mksite_magic_option printerfriendly` ; case "$x" in
        " "|".*"|"-*") printerfriendly="$x" ;; esac
x=`mksite_magic_option sectiontab` ; case "$x" in
      " "|"no"|"warn") sectiontab="$x" ;; esac
x=`mksite_magic_option currenttab` ; case "$x" in
      " "|"no"|"warn") currenttab="$x" ;; esac
x=`mksite_magic_option sectioninfo` ; case "$x" in
      " "|"no"|"[=:-]") sectioninfo="$x" ;; esac
x=`mksite_magic_option emailfooter` 
   test ".$x" != "." && emailfooter="$x"

test ".$opt_print" != "." && printerfriendly="$opt_print"
test ".$commentvars"  = ".no" && updatevars="no"   # duplicated into
test ".$commentvars"  = ".no" && expandvars="no"   # info2vars_sed ()
test ".$commentvars"  = ".no" && simplevars="no"   # function above

test -d DEBUG && \
echo "NOTE: '$sectionlayout'sectionlayout '$sitemaplayout'sitemaplayout"
test -d DEBUG && \
echo "NOTE: '$simplevars'simplevars '$printerfriendly'printerfriendly"
test -d DEBUG && \
echo "NOTE: '$attribvars'attribvars '$updatevars'updatevars"
test -d DEBUG && \
echo "NOTE: '$expandvars'expandvars '$commentvars'commentvars "
test -d DEBUG && \
echo "NOTE: '$currenttab'currenttab '$sectiontab'sectiontab"
test -d DEBUG && \
echo "NOTE: '$headsection'headsection '$tailsection'tailsection"

if ($STAT_R "$SITEFILE" >$NULL) 2>$NULL ; then : ; else STAT_R=":" ; fi
# ==========================================================================
# init a few global variables
#                                                                  0. INIT

mkpathdir () {
    if test -n "$1"  && test ! -d "$1" ; then
       echo "!! mkdir '$1'" ; mkdir "$1"
       test ! -d "$1" || mkdir -p "$1"
    fi
}

mkpathfile () {
    dirname=`echo "$1" | $SED -e "s:/[^/][^/]*\$::"`
    if test ".$1" != ".$dirname" && test ".$dirname" != "." ;
    then mkpathdir "$dirname"; fi
}

mknewfile () {
    mkpathfile "$1"
    $CATNULL > "$1" 
}

tmp_dir_was_created="no"
if test ! -d "$tmp" ; then mkpathdir "$tmp" ; tmp_dir_was_created="yes" ; fi

# $MK_TAGS - originally, we would use a lambda execution on each 
# uppercased html tag to replace <P> with <p class="P">. Here we just
# walk over all the known html tags and make an sed script that does
# the very same conversion. There would be a chance to convert a single
# tag via "h;y;x" or something we do want to convert all the tags on
# a single line of course.
mknewfile "$MK_TAGS"
for P in P H1 H2 H3 H4 H5 H6 DL DD DT UL OL LI PRE CODE TABLE TR TD TH \
         B U I S Q EM STRONG STRIKE CITE BIG SMALL SUP SUB TT THEAD TBODY \
         CENTER HR BR NOBR WBR SPAN DIV IMG ADRESS BLOCKQUOTE
do M=`echo "$P" | $SED -e "y/$UPPER/$LOWER/"`
  echo "s|<$P>|<$M class=\"$P\">|g"         >> "$MK_TAGS"
  echo "s|<$P |<$M class=\"$P\" |g"         >> "$MK_TAGS"
  echo "s|</$P>|</$M>|g"                    >> "$MK_TAGS"
done
  echo "s|<>|\\&nbsp\\;|g"                  >> "$MK_TAGS"
  echo "s|<->|<WBR />\\;|g"                 >> "$MK_TAGS"
# also make sure that some non-html entries are cleaned away that
# we are generally using to inject meta information. We want to see
# that meta ino in the *.htm browser view during editing but they
# shall not get present in the final html page for publishing.
DC_VARS="contributor date source language coverage identifier"
DC_VARS="$DC_VARS rights relation creator subject description"
DC_VARS="$DC_VARS publisher DCMIType"
_EQUIVS="refresh expires content-type cache-control"
_EQUIVS="$_EQUIVS redirect charset" # mapped to refresh / content-type
_EQUIVS="$_EQUIVS content-language content-script-type content-style-type"
for P in $DC_VARS $_EQUIVS ; do # dublin core embedded
   echo "s|<$P>[^<>]*</$P>||g"              >> "$MK_TAGS"
done
   echo "s|<!--sect[$AZ$NN]-->||g"          >> "$MK_TAGS"
   echo "s|<!--[$AX]*[?]-->||g"             >> "$MK_TAGS"
   echo "s|<!--\\\$[$AX]*[?]:-->||g"        >> "$MK_TAGS"
   echo "s|<!--\\\$[$AX]*:[?=]-->||g"       >> "$MK_TAGS"
   echo "s|\\(<[^<>]*\\)\\\${[$AX]*:[?=]\\([^<{}>]*\\)}\\([^<>]*>\\)|\\1\\2\\3|g"        >>$MK_TAGS

# see overview at www.metatab.de - http-equivs are
# <refresh>5; url=target</reresh>   or <redirect>target</redirect>
# <content-type>text/html; charset=koi8-r</content-type> iso-8859-1/UTF-8
# <content-language>de</content-language>             <charset>UTF-8</charset>
# <content-script-type>text/javascript</content-script-type> /jscript/vbscript
# <content-style-type>text/css</content-style-type>
# <cache-control>no-cache</cache-control>

trimm ()
{
    echo "$1" | $SED -e "s:^ *::" -e "s: *\$::";
}
trimmm ()
{
    echo "$1" | $SED -e "s:^ *::" -e "s: *\$::" -e "s:[	 ][	 ]*: :g";
}

timezone ()
{
    # +%z is an extension while +%Z is supposed to be posix
    _timezone=`$DATE_NOW +%z`
    case "$_timezone" in
	*+*) echo "$_timezone" ;;
	*-*) echo "$_timezone" ;;
	*) $DATE_NOW +%Z
    esac
}
timetoday () 
{
    $DATE_NOW +%Y-%m-%d
}
timetodays ()
{
    $DATE_NOW +%Y-%m%d
}
    
# ======================================================================
#                                                                FUNCS

sed_longscript ()
{
    # hpux sed has a limit of 100 entries per sed script !
    $SED             -e "100q" "$1" > "$1~1~"
    $SED -e "1,100d" -e "200q" "$1" > "$1~2~"
    $SED -e "1,200d" -e "300q" "$1" > "$1~3~"
    $SED -e "1,300d" -e "400q" "$1" > "$1~4~"
    $SED -e "1,400d" -e "500q" "$1" > "$1~5~"
    $SED -e "1,500d" -e "600q" "$1" > "$1~6~"
    $SED -e "1,600d" -e "700q" "$1" > "$1~7~"
    $SED -e "1,700d" -e "800q" "$1" > "$1~8~"
    $SED -e "1,800d" -e "900q" "$1" > "$1~9~"
    $SED -f "$1~1~"  -f "$1~2~" -f "$1~3~" -f "$1~4~" -f "$1~5~" \
         -f "$1~6~"  -f "$1~7~" -f "$1~8~" -f "$1~9~" "$2"
}

sed_slash_key ()      # helper to escape chars special in /anchor/ regex
{                     # currently escaping "/" "[" "]" "."
    echo "$1" | $SED -e "s|[./[-]|\\\\&|g" -e "s|\\]|\\\\&|g"
}
sed_piped_key ()      # helper to escape chars special in s|anchor|| regex
{                     # currently escaping "|" "[" "]" "."
    echo "$1" | $SED -e "s/[.|[-]/\\\\&/g" -e "s/\\]/\\\\&/g"
}

back_path ()          # helper to get the series of "../" for a given path
{
    echo "$1" | $SED -e "/\\//!d" -e "s|/[^/]*\$|/|" -e "s|[^/]*/|../|g"
}

dir_name ()
{
    echo "$1" | $SED -e "s:/[^/][^/]*\$::"
}

info2test_sed ()          # cut out all old-style <!--vars--> usages
{
  INP="$1" ; test ".$INP" = "." && INP="$tmp/$F.$INFO"
  V8=" *\\([^ ][^ ]*\\) \\(.*\\)"
  V9=" *DC[.]\\([^ ][^ ]*\\) \\(.*\\)"
   q="\\\$"
   _x_="WARNING: assumed simplevar <!--\\\\1--> changed to <!--$q\\\\1:=-->"
   _y_="WARNING: assumed simplevar <!--\\\\1--> changed to <!--$q\\\\1:?-->"
   _X_="WARNING: assumed tailvar <!--$q\\\\1:--> changed to <!--$q\\\\1:=-->"
   _Y_="WARNING: assumed tailvar <!--$q\\\\1:--> changed to <!--$q\\\\1:?-->"
   echo "s/^/ /" # $++
  $SED -e "/^=....=formatter /d" \
  -e "/=text=/s%=text=$V9%s|.*<!--\\\\(\\1\\\\)-->.*|$_x_|%" \
  -e "/=Text=/s%=Text=$V9%s|.*<!--\\\\(\\1\\\\)-->.*|$_x_|%" \
  -e "/=name=/s%=name=$V9%s|.*<!--\\\\(\\1\\\\)[?]-->.*|$_y_|%" \
  -e "/=Name=/s%=Name=$V9%s|.*<!--\\\\(\\1\\\\)[?]-->.*|$_y_|%" \
  -e "/=text=/s%=text=$V8%s|.*<!--\\\\(\\1\\\\)-->.*|$_x_|%" \
  -e "/=Text=/s%=Text=$V8%s|.*<!--\\\\(\\1\\\\)-->.*|$_x_|%" \
  -e "/=name=/s%=name=$V8%s|.*<!--\\\\(\\1\\\\)[?]-->.*|$_y_|%" \
  -e "/=Name=/s%=Name=$V8%s|.*<!--\\\\(\\1\\\\)[?]-->.*|$_y_|%" \
  -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  $SED -e "/^=....=formatter /d" \
  -e "/=text=/s%=text=$V9%s|.*<!--$q\\\\(\\1\\\\):-->.*|$_X_|%" \
  -e "/=Text=/s%=Text=$V9%s|.*<!--$q\\\\(\\1\\\\):-->.*|$_X_|%" \
  -e "/=name=/s%=name=$V9%s|.*<!--$q\\\\(\\1\\\\)[?]:-->.*|$_Y_|%" \
  -e "/=Name=/s%=Name=$V9%s|.*<!--$q\\\\(\\1\\\\)[?]:-->.*|$_Y_|%" \
  -e "/=text=/s%=text=$V8%s|.*<!--$q\\\\(\\1\\\\):-->.*|$_X_|%" \
  -e "/=Text=/s%=Text=$V8%s|.*<!--$q\\\\(\\1\\\\):-->.*|$_X_|%" \
  -e "/=name=/s%=name=$V8%s|.*<!--$q\\\\(\\1\\\\)[?]:-->.*|$_Y_|%" \
  -e "/=Name=/s%=Name=$V8%s|.*<!--$q\\\\(\\1\\\\)[?]:-->.*|$_Y_|%" \
  -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  echo "/^WARNING:/!d" # $++
}

info2vars_sed ()          # generate <!--$vars--> substition sed addon script
{
  INP="$1" ; test ".$INP" = "." && INP="$tmp/$F.$INFO"
  V8=" *\\([^ ][^ ]*\\) \\(.*\\)"
  V9=" *DC[.]\\([^ ][^ ]*\\) \\(.*\\)"
  N8=" *\\([^ ][^ ]*\\) \\([$NN].*\\)"
  N9=" *DC[.]\\([^ ][^ ]*\\) \\([$NN].*\\)"
  V0="\\\\([<]*\\\\)\\\\\\\$"
  V1="\\\\([^<>]*\\\\)\\\\\\\$"
  V2="\\\\([^{<>}]*\\\\)"
  V3="\\\\([^<>]*\\\\)"
  SS="<""<>"">" # spacer so value="2004" does not make for s|\(...\)|\12004|
  test ".$commentvars"  = ".no" && updatevars="no"   # duplicated from
  test ".$commentvars"  = ".no" && expandvars="no"   # option handling
  test ".$commentvars"  = ".no" && simplevars="no"   # tests below
  test ".$expandvars" != ".no" && \
  $SED -e "/^=....=formatter /d" \
      -e "/^=name=/s,=name=$V9,s|<!--$V0\\1[?]-->|- \\2|," \
      -e "/^=Name=/s,=Name=$V9,s|<!--$V0\\1[?]-->|(\\2)|," \
      -e "/^=name=/s,=name=$V8,s|<!--$V0\\1[?]-->|- \\2|," \
      -e "/^=Name=/s,=Name=$V8,s|<!--$V0\\1[?]-->|(\\2)|," \
      -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  test ".$expandvars" != ".no" && \
  $SED -e "/^=....=formatter /d" \
      -e "/^=text=/s,=text=$V9,s|<!--$V1\\1-->|\\\\1$SS\\2|," \
      -e "/^=Text=/s,=Text=$V9,s|<!--$V1\\1-->|\\\\1$SS\\2|," \
      -e "/^=name=/s,=name=$V9,s|<!--$V1\\1[?]-->|\\\\1$SS\\2|," \
      -e "/^=Name=/s,=Name=$V9,s|<!--$V1\\1[?]-->|\\\\1$SS\\2|," \
      -e "/^=text=/s,=text=$V8,s|<!--$V1\\1-->|\\\\1$SS\\2|," \
      -e "/^=Text=/s,=Text=$V8,s|<!--$V1\\1-->|\\\\1$SS\\2|," \
      -e "/^=name=/s,=name=$V8,s|<!--$V1\\1[?]-->|\\\\1$SS\\2|," \
      -e "/^=Name=/s,=Name=$V8,s|<!--$V1\\1[?]-->|\\\\1$SS\\2|," \
      -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  test ".$simplevars" != ".no" && test ".$updatevars" != ".no" && \
  $SED -e "/^=....=formatter /d" \
      -e "/^=text=/s,=text=$V9,s|<!--$V0\\1:-->[$AX]*|\\2|," \
      -e "/^=Text=/s,=Text=$V9,s|<!--$V0\\1:-->[$AX]*|\\2|," \
      -e "/^=name=/s,=name=$V9,s|<!--$V0\\1[?]:-->[$AX]*|- \\2|," \
      -e "/^=Name=/s,=Name=$V9,s|<!--$V0\\1[?]:-->[$AX]*| (\\2) |," \
      -e "/^=text=/s,=text=$V8,s|<!--$V0\\1:-->[$AX]*|\\2|," \
      -e "/^=Text=/s,=Text=$V8,s|<!--$V0\\1:-->[$AX]*|\\2|," \
      -e "/^=name=/s,=name=$V8,s|<!--$V0\\1[?]:-->[$AX]*|- \\2|," \
      -e "/^=Name=/s,=Name=$V8,s|<!--$V0\\1[?]:-->[$AX]*| (\\2) |," \
      -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  test ".$updatevars" != ".no" && \
  $SED -e "/^=....=formatter /d" \
      -e "/^=name=/s,=name=$V9,s|<!--$V0\\1:[?]-->[^<>]*|- \\2|," \
      -e "/^=Name=/s,=Name=$V9,s|<!--$V0\\1:[?]-->[^<>]*| (\\2) |," \
      -e "/^=name=/s,=name=$V8,s|<!--$V0\\1:[?]-->[^<>]*|- \\2|," \
      -e "/^=Name=/s,=Name=$V8,s|<!--$V0\\1:[?]-->[^<>]*| (\\2) |," \
  -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  test ".$updatevars" != ".no" && \
  $SED -e "/^=....=formatter /d" \
      -e "/^=text=/s,=text=$V9,s|<!--$V1\\1:[=]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=Text=/s,=Text=$V9,s|<!--$V1\\1:[=]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=name=/s,=name=$V9,s|<!--$V1\\1:[?]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=Name=/s,=Name=$V9,s|<!--$V1\\1:[?]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=text=/s,=text=$V8,s|<!--$V1\\1:[=]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=Text=/s,=Text=$V8,s|<!--$V1\\1:[=]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=name=/s,=name=$V8,s|<!--$V1\\1:[?]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=Name=/s,=Name=$V8,s|<!--$V1\\1:[?]-->[^<>]*|\\\\1$SS\\2|," \
      -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  test ".$attribvars" != ".no" && \
  $SED -e "/^=....=formatter /d" \
      -e "/^=text=/s,=text=$V9,s|<$V1{\\1:[=]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=Text=/s,=Text=$V9,s|<$V1{\\1:[=]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=name=/s,=name=$V9,s|<$V1{\\1:[?]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=Name=/s,=Name=$V9,s|<$V1{\\1:[?]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=text=/s,=text=$V8,s|<$V1{\\1:[=]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=Text=/s,=Text=$V8,s|<$V1{\\1:[=]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=name=/s,=name=$V8,s|<$V1{\\1:[?]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=Name=/s,=Name=$V8,s|<$V1{\\1:[?]$V2}$V3>|<\\\\1$SS\\2\\\\3>|," \
      -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  test ".$simplevars" != ".no" && \
  $SED -e "/^=....=formatter /d" \
      -e "/^=text=/s,=text=$V9,s|<!--\\1-->[$AX]*|\\2|," \
      -e "/^=Text=/s,=Text=$V9,s|<!--\\1-->[$AX]*|\\2|," \
      -e "/^=name=/s,=name=$V9,s|<!--\\1[?]-->[$AX]*| - \\2|," \
      -e "/^=Name=/s,=Name=$V9,s|<!--\\1[?]-->[$AX]*| (\\2) |," \
      -e "/^=text=/s,=text=$V8,s|<!--\\1-->[$AX]*|\\2|," \
      -e "/^=Text=/s,=Text=$V8,s|<!--\\1-->[$AX]*|\\2|," \
      -e "/^=name=/s,=name=$V8,s|<!--\\1[?]-->[$AX]*| - \\2|," \
      -e "/^=Name=/s,=Name=$V8,s|<!--\\1[?]-->[$AX]*| (\\2) |," \
      -e "/^=/d" -e "s|&|\\\\&|g"  $INP # $++
  # if value="2004" then generated sed might be "\\12004" which is bad
  # instead we generate an edited value of "\\1$SS$value" and cut out
  # the spacer now after expanding the variable values:
  echo "s|$SS||g" # $++
}

info2meta_sed ()         # generate <meta name..> text portion
{
  # http://www.metatab.de/meta_tags/DC_type.htm
  INP="$1" ; test ".$INP" = "." && INP="$tmp/$F.$INFO"
  V6=" *HTTP[.]\\([^ ][^ ]*\\) \\(.*\\)"
  V7=" *DC[.]\\([^ ][^ ]*\\) \\(.*\\)"
  V8=" *\\([^ ][^ ]*\\) \\(.*\\)"
  INFO_META_TYPE_SCHEME="name=\"DC.type\" content=\"\\2\" scheme=\"\\1\""
  INFO_META_DCMI="name=\"\\1\" content=\"\\2\" scheme=\"DCMIType\""
  INFO_META_NAME_TZ="name=\"\\1\" content=\"\\2 `timezone`\"" 
  INFO_META_NAME="name=\"\\1\" content=\"\\2\""
  INFO_META_HTTP="http-equiv=\"\\1\" content=\"\\2\""
  $SED -e "/=....=today /d" \
  -e "/=meta=HTTP[.]/s,=meta=$V6, <meta $INFO_META_HTTP />," \
  -e "/=meta=DC[.]DCMIType /s,=meta=$V7, <meta $INFO_META_TYPE_SCHEME />," \
  -e "/=meta=DC[.]type Collection$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]type Dataset$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]type Event$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]type Image$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]type Service$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]type Software$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]type Sound$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]type Text$/s,=meta=$V8, <meta $INFO_META_DCMI />," \
  -e "/=meta=DC[.]date[.].*[+]/s,=meta=$V8, <meta $INFO_META_NAME />," \
  -e "/=meta=DC[.]date[.].*[:]/s,=meta=$V8, <meta $INFO_META_NAME_TZ />," \
  -e "/=meta=/s,=meta=$V8, <meta $INFO_META_NAME />," \
  -e "/<meta name=\"[^\"]*\" content=\"\" /d" \
  -e "/<meta http-equiv=\"[^\"]*\" content=\"\" /d" \
  -e "/^=/d" $INP # $++
}

info_get_entry () # get the first <!--vars--> value known so far
{
  TXT="$1" ; test ".$TXT" = "." && TXT="sect"
  INP="$2" ; test ".$INP" = "." && INP="$tmp/$F.$INFO"
  $SED -e "/=text=$TXT /!d" -e "s/=text=$TXT //" -e "q" $INP # $++
}

info1grep () # test for a <!--vars--> substition to be already present
{
  TXT="$1" ; test ".$TXT" = "." && TXT="sect"
  INP="$2" ; test ".$INP" = "." && INP="$tmp/$F.$INFO"
  $GREP "^=text=$TXT " $INP >$NULL
  return $?
}

dx_init()
{
    mkpathdir "$tmp"
    dx_meta formatter `basename $opt_formatter` > "$tmp/$F.$INFO"
    for opt in $opt_variables ; do case "$opt" in # commandline --def=value
      *_*) op_=`echo "$opt" | sed -e "y/_/-/"`    # makes for <!--$def-->
           dx_meta "$op_" `eval echo "\\\$opt_$opt"` ;; 
      *)   dx_text "$opt" `eval echo "\\\$opt_$opt"` ;;
    esac ; done
}

dx_line ()
{
    echo "$1$2 "`trimmm "$3"` >> "$tmp/$F.$INFO"
}

DX_line ()
{
    dx_val_=`echo "$3" | sed -e "s/<[^<>]*>//g"`
    dx_line "$1" "$2" "$dx_val_"
}

dx_text ()
{
    dx_line "=text=" "$1" "$2"
}

DX_text ()   # add a <!--vars--> substition includings format variants
{
  N=`trimm "$1"` ; T=`trimm "$2"`
  if test ".$N" != "." ; then
    if test ".$T" != "." ; then
      text=`echo "$T" | $SED -e "y/$UPPER/$LOWER/" -e "s/<[^<>]*>//g"`
      dx_line "=text=" "$N" "$T"
      dx_line "=name=" "$N" "$text"
      varname=`echo "$N" | $SED -e 's/.*[.]//'`    # cut out front part
      if test ".$N" != ".$varname" ; then 
      text=`echo "$varname $T" | $SED -e "y/$UPPER/$LOWER/" -e "s/<[^<>]*>//g"`
      dx_line "=Text=" "$varname" "$T"
      dx_line "=Name=" "$varname" "$text"
      fi
    fi
  fi
}

dx_meta ()
{
    DX_line "=meta=" "$1" "$2"
}

DX_meta ()  # add simple meta entry and its <!--vars--> subsitution
{
    DX_line "=meta=" "$1" "$2"
    DX_text "$1" "$2"
}

DC_meta ()   # add new DC.meta entry plus two <!--vars--> substitutions
{
    DX_line "=meta=" "DC.$1" "$2"
    DX_text "DC.$1" "$2"
    DX_text "$1" "$2"
}

HTTP_meta ()   # add new HTTP.meta entry plus two <!--vars--> substitutions
{
    DX_line "=meta=" "HTTP.$1" "$2"
    DX_text "HTTP.$1" "$2"
    DX_text "$1" "$2"
}

DC_VARS_Of () # check DC vars as listed in $DC_VARS global and generate DC_meta
{             # the results will be added to .meta.tmp and .vars.tmp later
   FILENAME="$1" ; test ".$FILENAME" = "." && FILENAME="$SOURCEFILE"   
   for M in $DC_VARS title ; do
      # scan for a <markup> of this name
      part=`$SED -e "/<$M>/!d" -e "s|.*<$M>||" -e "s|</$M>.*||" -e q $FILENAME`
      part=`trimm "$part"`
      text=`echo  "$part" | $SED -e "s|^[$AA]*:||"`
      text=`trimm "$text"`
      test ".$text" = "." && continue
      # <mark:part> will be <meta name="mark.part">
      if test ".$text" != ".$part" ; then
         N=`echo "$part" | $SED -e "s/:.*//"`
         DC_meta "$M.$N" "$text"
      elif test ".$M" = ".date" ; then
         DC_meta "$M.issued" "$text" # "<date>" -> "<date>issued:"
      else
         DC_meta "$M" "$text"
      fi
   done
}

HTTP_VARS_Of () # check HTTP-EQUIVs as listed in $_EQUIV global then
{               # generate meta tags that are http-equiv= instead of name=
   FILENAME="$1" ; test ".$FILENAME" = "." && FILENAME="$SOURCEFILE"   
   for M in $_EQUIVS ; do
      # scan for a <markup> of this name
      part=`$SED -e "/<$M>/!d" -e "s|.*<$M>||" -e "s|</$M>.*||" -e q $FILENAME`
      part=`trimm "$part"`
      text=`echo  "$part" | $SED -e "s|^[$AA]*:||"`
      text=`trimm "$text"`
      test ".$text" = "." && continue
      if test ".$M" = ".redirect" ; then
         HTTP_meta "refresh" "5; url=$text" ; DX_text "$M" "$text"
      elif test ".$M" = ".charset" ; then
         HTTP_meta "content-type" "text/html; charset=$text" 
      else
         HTTP_meta "$M" "$text"
      fi
   done
}

DC_isFormatOf ()       # make sure there is this DC.relation.isFormatOf tag
{                      # choose argument for a fallback (usually $SOURCEFILE)
   NAME="$1" ; test ".$NAME" = "." && NAME="$SOURCEFILE"   
   info1grep DC.relation.isFormatOf || DC_meta relation.isFormatOf "$NAME"
}

DC_publisher ()        # make sure there is this DC.publisher meta tag
{                      # choose argument for a fallback (often $USER)
   NAME="$1" ; test ".$NAME" = "." && NAME="$USER"
   info1grep DC.publisher || DC_meta publisher "$NAME"
}

DC_modified ()         # make sure there is a DC.date.modified meta tag
{                      # maybe choose from filesystem dates if possible
   Q="$1" # target file
   if info1grep DC.date.modified ; then :
   else
      _42_chars="........................................."
      cut_42_55="s/^$_42_chars\\(.............\\).*/\\1/" # i.e.`cut -b 42-55`
      text=`$STAT_R $Q 2>$NULL | $SED -e '/odify:/!d' -e 's|.*fy:||' -e q`
      text=`echo "$text" | $SED -e "s/:..[.][$NN]*//"`
      text=`trimm "$text"`
      test ".$text" = "." && \
      text=`$DATE_R "$Q" +%Y-%m-%d 2>$NULL`   # GNU sed
      test ".$text" = "." && 
      text=`$LS_L "$Q" | $SED -e "$cut_42_55" -e "s/^ *//g" -e "q"`
      text=`echo "$text" | $SED -e "s/[$NN]*:.*//"` # cut way seconds
      DC_meta date.modified `trimm "$text"`
   fi
}

DC_date ()             # make sure there is this DC.date meta tag
{                      # choose from one of the available DC.date.* specials
   Q="$1" # source file
   if info1grep DC.date 
   then DX_text issue "dated `info_get_entry DC.date`"
        DX_text updated     "`info_get_entry DC.date`"
   else text=""
      for kind in available issued modified created ; do
        text=`info_get_entry DC.date.$kind` 
      # test ".$text" != "." && echo "$kind = date = $text ($Q)"
        test ".$text" != "." && break
      done
      if test ".$text" = "." ; then
        M="date"
        part=`$SED -e "/<$M>/!d" -e "s|.*<$M>||" -e "s|</$M>.*||" -e q $Q`
	part=`trimm "$part"`
        text=`echo "$part" | $SED -e "s|^[$AA]*:||"`
	text=`trimm "$text"`
      fi
      if test ".$text" = "." ; then 
        M="!--date:*=*--" # takeover updateable variable...
        part=`$SED -e "/<$M>/!d" -e "s|.*<$M>||" -e "s|</.*||" -e q $Q`
	part=`trimm "$part"`
        text=`echo "$part" | $SED -e "s|^[$AA]*:||" -e "s|\\&.*||"`
	text=`trimm "$text"`
      fi
      text=`echo "$text" | $SED -e "s/[$NN]*:.*//"` # cut way seconds
      DX_text updated "$text"
      text1=`echo "$text" | $SED -e "s|^.* *updated ||"`
      if test ".$text" != ".$text1" ; then
        kind="modified" ; text=`echo "$text1" | $SED -e "s|,.*||"`
      fi
      text1=`echo "$text" | $SED -e "s|^.* *modified ||"`
      if test ".$text" != ".$text1" ; then
        kind="modified" ; text=`echo "$text1" | $SED -e "s|,.*||"`
      fi
      text1=`echo "$text" | $SED -e "s|^.* *created ||"`
      if test ".$text" != ".$text1" ; then
        kind="created" ; text=`echo "$text1" | $SED -e "s|,.*||"`
      fi
      text=`echo "$text" | $SED -e "s/[$NN]*:.*//"` # cut way seconds
      DC_meta date `trimm "$text"`
      DX_text issue `trimm "$kind $text"`
   fi
}

DC_title ()
{
   # choose a title for the document, either an explicit title-tag
   # or one of the section headers in the document or fallback to filename
   Q="$1" # target file
   if info1grep DC.title ; then :
   else
      for M in TITLE title H1 h1 H2 h2 H3 H3 H4 H4 H5 h5 H6 h6 ; do
        text=`$SED -e "/<$M>/!d" -e "s|.*<$M>||" -e "s|</$M>.*||" -e q $Q`
	text=`trimm "$text"` ; test ".$text" != "." && break
        MM="$M [^<>]*"
        text=`$SED -e "/<$MM>/!d" -e "s|.*<$MM>||" -e "s|</$M>.*||" -e q $Q`
	text=`trimm "$text"` ; test ".$text" != "." && break
      done
      if test ".text" = "." ; then
	text=`basename $Q .html`
        text=`basename $text .htm | $SED -e 'y/_/ /' -e "s/\\$/ info/"`
      fi
      term=`echo "$text" | $SED -e 's/.*[(]//' -e 's/[)].*//'`
      text=`echo "$text" | $SED -e 's/[(][^()]*[)]//'`
      if test ".$term" = "." || test ".$term" = ".$text" ; then
         DC_meta title "$text"
      else
         DC_meta title "$term - $text"
      fi
   fi
}    

site_get_section () # return parent section page of given page
{
   _F_=`sed_slash_key "$1"`
   $SED -e "/^=sect=$_F_ /!d" -e "s/^=sect=$_F_ //" -e q "$MK_INFO" # $++
}

DC_section () # not really a DC relation (shall we use isPartOf ?) 
{             # each document should know its section father
   sectn=`site_get_section "$F"`
   if test ".$sectn" != "." ; then
      DC_meta relation.section "$sectn"
   fi
}

info_get_entry_section()
{
    info_get_entry DC.relation.section # $++
}    

site_get_selected ()  # return section of given page
{
   _F_=`sed_slash_key "$1"`
   $SED -e "/=use.=$_F_ /!d" -e "s/=use.=[^ ]* //" -e q "$MK_INFO" # $++
}

DC_selected () # not really a DC title (shall we use alternative ?)
{
   # each document might want to highlight the currently selected item
   short=`site_get_selected "$F"`
   if test ".$short" != "." ; then
      DC_meta title.selected "$short"
   fi
}

info_get_entry_selected ()
{
    info_get_entry DC.title.selected # $++
}

site_get_rootsections () # return all sections from root of nav tree
{
   $SED -e "/=use1=/!d" -e "s/=use.=\\([^ ]*\\) .*/\\1/" "$MK_INFO" # $++
}

site_get_sectionpages () # return all children pages in the given section
{
   _F_=`sed_slash_key "$1"`
   $SED -e "/^=sect=[^ ]* $_F_\$/!d" -e "s/^=sect=//" \
        -e "s/ .*//" "$MK_INFO" # $++
}

site_get_subpages () # return all page children of given page
{
   _F_=`sed_slash_key "$1"`
   $SED -e "/^=node=[^ ]* $_F_\$/!d" -e "s/^=node=//" -e "s/ .*//" "$MK_INFO"
   # $++
}

site_get_parentpage () # return parent page for given page (".." for sections)
{
   _F_=`sed_slash_key "$1"`
   $SED -e "/^=node=$_F_ /!d" -e "s/^=node=[^ ]* //" -e "q" "$MK_INFO"  # $++
}

DX_alternative ()        # detect wether page asks for alternative style
{                        # which is generally a shortpage variant
    x=`mksite_magic_option alternative $1 | sed -e "s/^ *//" -e "s/ .*//"`
    if test ".$x" != "." ; then
      DX_text alternative "$x"
    fi
}

info2head_sed ()      # append alternative handling script to $HEAD
{
    have=`info_get_entry alternative`
    if test ".$have" != "." ; then
       echo "/<!--mksite:alternative:$have .*-->/{" # $++
       echo "s/<!--mksite:alternative:$have\\( .*\\)-->/\\1/" # $++
       echo "q" # $++ 
       echo "}" # $++
    fi
}
info2body_sed ()      # append alternative handling script to $BODY
{
    have=`info_get_entry alternative`
    if test ".$have" != "." ; then
       echo "s/<!--mksite:alternative:$have\\( .*\\)-->/\\1/" # $++
    fi
}

bodymaker_for_sectioninfo ()
{
    test ".$sectioninfo" = ".no" && return
    _x_="<!--mksite:sectioninfo::-->"
    _q_="\\([^<>]*[$AX][^<>]*\\)"
    test ".$sectioninfo" != ". " && _q_="[ ][ ]*$sectioninfo\\([ ]\\)" 
    echo "s|\\(^<[hH][$NN][ >].*</[hH][$NN]>\\)$_q_|\\1$_x_\\2|"       # $++
    echo "/$_x_/s|^|<table width=\"100%\"><tr valign=\"bottom\"><td>|" # $++
    echo "/$_x_/s|</[hH][$NN]>|&</td><td align=\"right\"><i>|"         # $++
    echo "/$_x_/s|\$|</i></td></tr></table>|"                          # $++
    echo "s|$_x_||"                                                    # $++
}

fast_href ()  # args "$FILETOREFERENCE" "$FROMCURRENTFILE:$F"
{   # prints path to $FILETOREFERENCE href-clickable in $FROMCURRENTFILE
    # if no subdirectoy then output is the same as input $FILETOREFERENCE
    R="$2" ; test ".$R" = "." && R="$F"
    S=`back_path "$R"`
    if test ".$S" = "." 
    then echo "$1" # $++
    else _1_=`echo "$1" | \
         $SED -e "/^ *\$/d" -e "/^\\//d" -e "/^[.][.]/d" -e "/^[$AA]*:/d" `
         if test ".$_1_" = "." # don't move any in the pattern above
         then echo "$1"   # $++
         else echo "$S$1" # $++  prefixed with backpath
    fi fi
}

make_fast () # experimental - make a FAST file that can be applied
{            # to htm sourcefiles in a subdirectory of the sitefile.
#   R="$1" ; test ".$R" = "." && R="$F"
    S=`back_path "$F"` 
    if test ".$S" = "" ; then
       # echo "backpath '$F' = none needed"
       $CATNULL # $++
    else
       # echo "backpath '$F' -> '$S'"
       $SED -e "/href=\"[^\"]*\"/!d" -e "s/.*href=\"//" -e "s/\".*//" \
            -e "/^ *\$/d" -e "/^\\//d" -e "/^[.][.]/d" -e "/^[$AA]*:/d" \
	   $SITEFILE $SOURCEFILE | sort | uniq \
       | $SED -e "s,.*,s|href=\"&\"|href=\"$S&\"|," # $++
    fi
}

# ============================================================== SITE MAP INFO
# each entry needs atleast a list-title, a long-title, and a list-date
# these are the basic information to be printed in the sitemap file
# where it is bound the hierarchy of sect/subsect of the entries.

site_map_list_title() # $file $text
{
    echo "=list=$1 $2" >> "$MK_INFO"
}
info_map_list_title() # $file $text
{
    echo "=list=$2" >> "$tmp/$1.$INFO"
}
site_map_long_title() # $file $text
{
    echo "=long=$1 $2" >> "$MK_INFO"
}
info_map_long_title() # $file $text
{
    echo "=long=$2" >> "$tmp/$1.$INFO"
}
site_map_list_date() # $file $text
{
    echo "=date=$1 $2" >> "$MK_INFO"
}
info_map_list_date() # $file $text
{
    echo "=date=$2" >> "$tmp/$1.$INFO"
}

siteinfo2sitemap ()  # generate <name><page><date> addon sed scriptlet
{                    # the resulting script will act on each item/line
                     # containing <!--"filename"--> and expand any following
                     # reference of <!--name--> or <!--date--> or <!--long-->
  INP="$1" ; test ".$INP" = "." && INP="$MK_INFO"
  _list_="s|<!--\"\\1\"-->.*<!--name-->|\\&<name href=\"\\1\">\\2</name>|"
  _date_="s|<!--\"\\1\"-->.*<!--date-->|\\&<date>\\2</date>|"
  _long_="s|<!--\"\\1\"-->.*<!--long-->|\\&<long>\\2</long>|"
  $SED -e "s:&:\\\\&:g" \
       -e "s:=list=\\([^ ]*\\) \\(.*\\):$_list_:" \
       -e "s:=date=\\([^ ]*\\) \\(.*\\):$_date_:" \
       -e "s:=long=\\([^ ]*\\) \\(.*\\):$_long_:" \
       -e "/^s|/!d" $INP # $++
}

make_multisitemap ()
{  # each category gets its own column along with the usual entries
   INPUTS="$1" ; test ".$INPUTS" = "." && INPUTS="$MK_INFO"
   siteinfo2sitemap > "$MK_SITE" # have <name><long><date> addon-sed
  _form_="<!--\"\\2\"--><!--use\\1--><!--long--><!--end\\1-->"
  _form_="$_form_<br><!--name--><!--date-->"
  _tiny_="small><small><small" ; _tinyX_="small></small></small "
  _tabb_="<br><$_tiny_> </$_tinyX_>" ; _bigg_="<big> </big>"
  echo "<table width=\"100%\"><tr><td> " # $++
  $SED -e "/=use.=/!d" -e "s|=use\\(.\\)=\\([^ ]*\\) .*|$_form_|" \
       -f "$MK_SITE" -e "/<name/!d" \
       -e "s|<!--use1-->|</td><td valign=\"top\"><b>|" \
       -e "s|<!--end1-->|</b>|"  \
       -e "s|<!--use2-->|<br>|"  \
       -e "s|<!--use.-->|<br>|" -e "s/<!--[^<>]*-->/ /g" \
       -e "s|<long>||" -e "s|</long>||" \
       -e "s|<name |<$_tiny_><a |" -e "s|</name>||" \
       -e "s|<date>| |" -e "s|</date>|</a><br></$_tinyX_>|" \
       $INPUTS              # $++
   echo "</td><tr></table>" # $++
}

make_listsitemap ()
{   # traditional - the body contains a list with date and title extras
   INPUTS="$1" ; test ".$INPUTS" = "." && INPUTS="$MK_INFO"
   siteinfo2sitemap > "$MK_SITE" # have <name><long><date> addon-sed
   _form_="<!--\"\\2\"--><!--use\\1--><!--name--><!--date--><!--long-->"
   _tabb_="<td>\\&nbsp\\;</td>" 
   echo "<table cellspacing=\"0\" cellpadding=\"0\">" # $++
   $SED -e "/=use.=/!d" -e "s|=use\\(.\\)=\\([^ ]*\\) .*|$_form_|" \
        -f "$MK_SITE" -e "/<name/!d" \
        -e "s|<!--use\\(1\\)-->|<tr class=\"listsitemap\\1\"><td>*</td>|" \
        -e "s|<!--use\\(2\\)-->|<tr class=\"listsitemap\\1\"><td>-</td>|" \
        -e "s|<!--use\\(.\\)-->|<tr class=\"listsitemap\\1\"><td> </td>|" \
        -e  "/<tr.class=\"listsitemap3\">/s|<name [^<>]*>|&- |" \
        -e "s|<!--[^<>]*-->| |g" \
	-e "s|<name href=\"name:sitemap:|<name href=\"|" \
        -e "s|<name |<td><a |" -e "s|</name>|</a></td>$_tabb_|" \
        -e "s|<date>|<td><small>|" -e "s|</date>|</small></td>$_tabb_|" \
        -e "s|<long>|<td><em>|" -e "s|</long>|</em></td></tr>|" \
        $INPUTS             # $++
   for xx in `grep "=use.=name:sitemap:" $INPUTS` ; do
       xx=`echo $xx | sed -e "s/=use.=name:sitemap://"`
       if test -f "$xx" ; then
	   grep "<tr.class=\"listsitemap[$NN]\">" $xx # $++
       fi
   done
   echo "</table>"          # $++
}

print_extension ()
{
    ARG="$1" ; test ".$ARG" = "." && ARG="$opt_print"
    case "$ARG" in
      -*|.*) echo "$ARG" ;;   # $++
      *)     echo ".print" ;; # $++
    esac
}
    
html_sourcefile ()  # generally just cut away the trailing "l" (ell)
{                   # making "page.html" argument into "page.htm" return
    _SRCFILE_=`echo "$1" | $SED -e "s/l\\$//"`
    if test -f "$_SRCFILE_" ; then echo "$_SRCFILE_" # $++
    elif test -f "$opt_src_dir/$_SRCFILE_" 
    then echo "$opt_src_dir/$_SRCFILE_" # $++
    else echo ".//$_SRCFILE_" # $++
    fi
}
html_printerfile_sourcefile () 
{                   
    if test ".$printerfriendly" = "."
    then 
    echo "$1" | sed -e "s/l\$//" # $++
    else 
    _ext_=`print_extension "$printerfriendly"`
    _ext_=`sed_slash_key "$_ext_"`
    echo "$1" | sed -e "s/l\$//" -e "s/$_ext_\\([.][$AA]*\\)\$/\\1/" # $++
    fi
}

fast_html_printerfile () {
    x=`html_printerfile "$1"` ; basename "$x" # $++
#   x=`html_printerfile "$1"` ; fast_href "$x" $2 # $++
}

html_printerfile () # generate the printerfile for a given normal output
{
    _ext_=`print_extension "$printerfriendly" | sed -e "s/&/\\\\&/"`
    echo "$1" | sed -e "s/\\([.][$AA]*\\)\$/$_ext_\\1/" # $++
}

make_printerfile_fast () # generate s/file.html/file.print.html/ for hrefs
{                        # we do that only for the $FILELIST
   ALLPAGES="$1" ; # ="$FILELIST"
   for p in $ALLPAGES ; do
       a=`sed_slash_key "$p"`
       b=`html_printerfile "$p"`
       if test "$b" != "$p" ; then
         b=`html_printerfile "$p" | sed -e "s:&:\\\\&:g" -e "s:/:\\\\\\/:g"`
         echo "s/<a href=\"$a\">/<a href=\"$b\">/" # $++
         echo "s/<a href=\"$a\" /<a href=\"$b\" /" # $++
       fi
   done
}

echo_printsitefile_style ()
{
   _bold_="text-decoration : none ; font-weight : bold ; "
   echo "   <style>"                                          # $+++
   echo "     a:link    { $_bold_ color : #000060 ; }"        # $+++
   echo "     a:visited { $_bold_ color : #000040 ; }"        # $+++
   echo "     body      { background-color : white ; }"       # $+++
   echo "   </style>"                                         # $+++
}

make_printsitefile_head() # $sitefile
{
   echo_printsitefile_style > "$MK_STYLE"
   $SED -e "/<title>/p" -e "/<title>/d" \
        -e "/<head>/p"   -e "/<head>/d" \
        -e "/<\/head>/p"  -e "/<\/head>/d" \
        -e "/<body>/p"   -e "/<body>/d" \
        -e "/^.*<link [^<>]*rel=\"shortcut icon\"[^<>]*>.*\$/p" \
        -e "d" $SITEFILE | $SED -e "/<head>/r $MK_STYLE" # $+++
}


# ------------------------------------------------------------------------
# The printsitefile is a long text containing html href markups where
# each of the href lines in the file is being prefixed with the section
# relation. During a secondary call the printsitefile can grepp'ed for
# those lines that match a given output fast-file. The result is a
# navigation header with 1...3 lines matching the nesting level

# these alt-texts will be only visible in with a text-mode browser:
printsitefile_square="width=\"8\" height=\"8\" border=\"0\""
printsitefile_img_1="<img alt=\"|go text:\" $printsitefile_square />"
printsitefile_img_2="<img alt=\"||topics:\" $printsitefile_square />"
printsitefile_img_3="<img alt=\"|||pages:\" $printsitefile_square />"
_SECT="mksite:sect:"

echo_current_line () # $sect $extra
{
    echo "<!--$_SECT\"$1\"-->$2" # $++
}
make_current_entry () # $sect $file      ## requires $MK_SITE
{
  S="$1" ; R="$2"
  SSS=`sed_slash_key "$S"`  
  sep=" - " ; _left_=" [ " ; _right_=" ] "
  echo_current_line "$S" "<a href=\"$R\"><!--\"$R\"--><!--name--></a>$sep" \
       | $SED -f "$MK_SITE" -e "s/<name[^<>]*>//" -e "s/<\\/name>//" \
        -e "/<a href=\"$SSS\"/s/<a href/$_left_&/" \
        -e "/<a href=\"$SSS\"/s/<\\/a>/&$_right_/" \
        -e "s/<!--\"[^\"]*\"--><!--name-->//" # $+++
}
echo_subpage_line () # $sect $extra
{
    echo "<!--$_SECT*:\"$1\"-->$2" # $++
}

make_subpage_entry ()
{
  S="$1" ; R="$2"
  RR=`sed_slash_key "$R"`  
  sep=" - " ;
  echo_subpage_line "$S" "<a href=\"$R\"><!--\"$R\"--><!--name--></a>$sep" \
       | $SED -f "$MK_SITE" -e "s/<name[^<>]*>//" -e "s/<\\/name>//" \
        -e "s/<!--\"[^\"]*\"--><!--name-->//" # $+++
}

make_printsitefile ()
{
   # building the printsitefile looks big but its really a loop over sects
   INPUTS="$1" ; test ".$INPUTS" = "." && INPUTS="$MK_INFO"
   siteinfo2sitemap > "$MK_SITE" # have <name><long><date> addon-sed
   make_printsitefile_head $SITEFILE # $++

   sep=" - "
   _sect1="<a href=\"#.\" title=\"section\">$printsitefile_img_1</a> ||$sep"
   _sect2="<a href=\"#.\" title=\"topics\">$printsitefile_img_2</a> ||$sep"
   _sect3="<a href=\"#.\" title=\"pages\">$printsitefile_img_3</a> ||$sep"
   site_get_rootsections > "$MK_SECT1"
   # round one - for each root section print a current menu
   for r in `cat "$MK_SECT1"` ; do
       echo_current_line "$r" "<!--mksite:sect1:A--><br>$_sect1" # $++
       for s in `cat "$MK_SECT1"` ; do 
	   make_current_entry "$r" "$s" # $++
       done
       echo_current_line "$r" "<!--mksite:sect1:Z-->" # $++
   done # "$r"

   # round two - for each subsection print a current and subpage menu
   for r in `cat "$MK_SECT1"` ; do
   site_get_subpages "$r"     > "$MK_SECT2"
   for s in `cat "$MK_SECT2"` ; do test "$r" = "$s" && continue
       echo_current_line  "$s" "<!--mksite:sect2:A--><br>$_sect2" # $++
       for t in `cat "$MK_SECT2"` ; do test "$r" = "$t" && continue
	   make_current_entry "$s" "$t" # $++
       done # "$t"
       echo_current_line  "$s" "<!--mksite:sect2:Z-->" # $++
   done # "$s"
       _have_children_="0"
       for t in `cat "$MK_SECT2"` ; do test "$r" = "$t" && continue
	   test "$_have_children_" = "0" && _have_children_="1" && \
       echo_subpage_line  "$r" "<!--mksite:sect2:A--><br>$_sect2" # $++
	   make_subpage_entry "$r" "$t" # $++
       done # "$t"
           test "$_have_children_" = "1" && \
       echo_subpage_line  "$r" "<!--mksite:sect2:Z-->" # $++
   done # "$r"

   # round three - for each subsubsection print a current and subpage menu
   for r in `cat "$MK_SECT1"` ; do
   site_get_subpages "$r"     > "$MK_SECT2"
   for s in `cat "$MK_SECT2"` ; do test "$r" = "$s" && continue
   site_get_subpages "$s"     > "$MK_SECT3"
   for t in `cat "$MK_SECT3"` ; do test "$s" = "$t" && continue
       echo_current_line  "$t" "<!--mksite:sect3:A--><br>$_sect3" # $++
       for u in `cat "$MK_SECT3"` ; do test "$s" = "$u" && continue
	   make_current_entry "$t" "$u" # $++
       done # "$u"
       echo_current_line  "$t" "<!--mksite:sect3:Z-->" # $++
   done # "$t"
       _have_children_="0"
       for u in `cat "$MK_SECT3"` ; do test "$u" = "$s" && continue
	   test "$_have_children_" = "0" && _have_children_="1" && \
       echo_subpage_line  "$s" "<!--mksite:sect3:A--><br>$_sect3" # $++
	   make_subpage_entry "$s" "$u" # $++
       done # "$u"
           test "$_have_children_" = "1" && \
       echo_subpage_line  "$s" "<!--mksite:sect3:Z-->" # $++
   done # "$s"
   done # "$r"
   echo "<a name=\".\"></a>" # $++
   echo "</body></html>"    # $++
}

# create a selector that can grep a printsitefile for the matching entries
select_in_printsitefile () # arg = "page" : return to stdout >> $P.$HEAD
{
   _selected_="$1" ; test ".$_selected_" = "." && _selected_="$F"
   _section_=`sed_slash_key "$_selected_"`
   echo "s/^<!--$_SECT\"$_section_\"-->//"        # sect3
   echo "s/^<!--$_SECT[*]:\"$_section_\"-->//"    # children
   _selected_=`site_get_parentpage "$_selected_"` 
   _section_=`sed_slash_key "$_selected_"`
   echo "s/^<!--$_SECT\"$_section_\"-->//"        # sect2
   _selected_=`site_get_parentpage "$_selected_"` 
   _section_=`sed_slash_key "$_selected_"`
   echo "s/^<!--$_SECT\"$_section_\"-->//"        # sect1
   echo "/^<!--$_SECT\"[^\"]*\"-->/d"     
   echo "/^<!--$_SECT[*]:\"[^\"]*\"-->/d" 
   echo "s/^<!--mksite:sect[$NN]:[$AZ]-->//"
}

body_for_emailfooter ()
{
    test ".$emailfooter" = ".no" && return
    _email_=`echo "$emailfooter" | sed -e "s|[?].*||"`
    _dated_=`info_get_entry updated`
    echo "<hr><table border=\"0\" width=\"100%\"><tr><td>"
    echo "<a href=\"mailto:$emailfooter\">$_email_</a>"
    echo "</td><td align=\"right\">"
    echo "$_dated_</td></tr></table>"
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
S="\\&nbsp\\;"
# S="[&]nbsp[;]"

# HR and EM style markups must exist in input - BR sometimes left out 
# these routines in(ter)ject hardspace before, between, after markups
# note that "<br>" is sometimes used with HR - it must exist in input
echo_HR_EM_PP ()
{
    echo "/^$1$2$3*<a href=/s/^/$4/"
    echo "/^<>$1$2$3*<a href=/s/^/$4/"
    echo "/^$S$1$2$3*<a href=/s/^/$4/"
    echo "/^$1<>$2$3*<a href=/s/^/$4/"
    echo "/^$1$S$2$3*<a href=/s/^/$4/"
    echo "/^$1$2<>$3*<a href=/s/^/$4/"
    echo "/^$1$2$S$3*<a href=/s/^/$4/"
}

echo_br_EM_PP ()
{
    echo_HR_EM_PP  "$1" "$2" "$3" "$4"
    echo "/^$2$3*<a href=/s/^/$4/"
    echo "/^<>$2$3*<a href=/s/^/$4/"
    echo "/^$S$2$3*<a href=/s/^/$4/"
    echo "/^$2<>$3*<a href=/s/^/$4/"
    echo "/^$2$S$3*<a href=/s/^/$4/"
}    

echo_HR_PP ()
{
    echo "/^$1$2*<a href=/s/^/$3/"
    echo "/^<>$1$2*<a href=/s/^/$3/"
    echo "/^$S$1$2*<a href=/s/^/$3/"
    echo "/^$1<>$2*<a href=/s/^/$3/"
    echo "/^$1$S$2*<a href=/s/^/$3/"
}
echo_br_PP ()
{
    echo_HR_PP "$1" "$2" "$3"
    echo "/^$2*<a href=/s/^/$3/"
    echo "/^<>$2*<a href=/s/^/$3/"
    echo "/^$S$2*<a href=/s/^/$3/"
}
echo_sp_PP ()
{
    echo "/^<>$1*<a href=/s/^/$2/"
    echo "/^$S$1*<a href=/s/^/$2/"
    echo "/^<><>$1*<a href=/s/^/$2/"
    echo "/^$S$S$1*<a href=/s/^/$2/"
    echo "/^<>$1<>*<a href=/s/^/$2/"
    echo "/^$S$1$S*<a href=/s/^/$2/"
    echo "/^$1<><>*<a href=/s/^/$2/"
    echo "/^$1$S$S*<a href=/s/^/$2/"
    echo "/^$1<>*<a href=/s/^/$2/"
    echo "/^$1$S*<a href=/s/^/$2/"
}

echo_sp_sp ()
{
    echo "/^$1*<a name=/s/^/$2/"
    echo "/^<>$1*<a name=/s/^/$2/"
    echo "/^$S$1*<a name=/s/^/$2/"
    echo "/^<><>$1*<a name=/s/^/$2/"
    echo "/^$S$S$1*<a name=/s/^/$2/"
    echo "/^<>$1<>*<a name=/s/^/$2/"
    echo "/^$S$1$S*<a name=/s/^/$2/"
    echo "/^$1<><>*<a name=/s/^/$2/"
    echo "/^$1$S$S*<a name=/s/^/$2/"
    echo "/^$1<>*<a name=/s/^/$2/"
    echo "/^$1$S*<a name=/s/^/$2/"
}

make_sitemap_init()
{
    # build a list of detectors that map site.htm entries to a section table
    # note that the resulting .gets.tmp / .puts.tmp are real sed-script
    h1="[-|[]"
    b1="[*=]"
    b2="[-|[]"
    b3="[\\/:]"
    q3="[\\/:,[]"
    echo_HR_PP    "<hr>"            "$h1"    "<!--sect1-->"      > "$MK_GETS"
    echo_HR_EM_PP "<hr>" "<em>"     "$h1"    "<!--sect1-->"     >> "$MK_GETS"
    echo_HR_EM_PP "<hr>" "<strong>" "$h1"    "<!--sect1-->"     >> "$MK_GETS"
    echo_HR_PP    "<br>"            "$b1$b1" "<!--sect1-->"     >> "$MK_GETS"
    echo_HR_PP    "<br>"            "$b2$b2" "<!--sect2-->"     >> "$MK_GETS"
    echo_HR_PP    "<br>"            "$b3$b3" "<!--sect3-->"     >> "$MK_GETS"
    echo_br_PP    "<br>"            "$b2$b2" "<!--sect2-->"     >> "$MK_GETS"
    echo_br_PP    "<br>"            "$b3$b3" "<!--sect3-->"     >> "$MK_GETS"
    echo_br_EM_PP "<br>" "<small>"  "$q3"    "<!--sect3-->"     >> "$MK_GETS"
    echo_br_EM_PP "<br>" "<em>"     "$q3"    "<!--sect3-->"     >> "$MK_GETS"
    echo_br_EM_PP "<br>" "<u>"      "$q3"    "<!--sect3-->"     >> "$MK_GETS"
    echo_HR_PP    "<br>"            "$q3"    "<!--sect3-->"     >> "$MK_GETS"
    echo_sp_PP                      "$q3"    "<!--sect3-->"     >> "$MK_GETS"
    echo_sp_sp                      "$q3"    "<!--sect9-->"     >> "$MK_GETS"
    $SED -e "s/\\(>\\)\\(\\[\\)/\\1 *\\2/" "$MK_GETS" > "$MK_PUTS"
    # the .puts.tmp variant is used to <b><a href=..></b> some hrefs which
    # shall not be used otherwise for being generated - this is nice for
    # some quicklinks somewhere. The difference: a whitspace "<hr> <a...>"
}

_uses_="=use\\1=\\2 \\3" ; _name_="=use\\1=name:\\2 \\3" ; 
_getW_="<!--sect\\([$NN]\\)-->"
_getX_="<!--sect\\([$NN]\\)--><[^<>]*>[^<>]*"
_getY_="<!--sect\\([$NN]\\)--><[^<>]*>[^<>]*<[^<>]*>[^<>]*"

make_sitemap_list()
{
    # scan sitefile for references pages - store as "=use+=href+ anchortext"
    $SED -f $MK_GETS           -e "/^<!--sect[$NN]-->/!d" \
	-e "s|^$_getX_<a href=\"\\([^\"]*\\)\"[^<>]*>\\(.*\\)</a>.*|$_uses_|" \
	-e "s|^$_getY_<a href=\"\\([^\"]*\\)\"[^<>]*>\\(.*\\)</a>.*|$_uses_|" \
	-e "s|^$_getW_<a name=\"\\([^\"]*\\)\"[^<>]*>\\(.*\\)</a>.*|$_name_|" \
	-e "s|^$_getX_<a name=\"\\([^\"]*\\)\"[^<>]*>\\(.*\\)</a>.*|$_name_|" \
	-e "s|^$_getY_<a name=\"\\([^\"]*\\)\"[^<>]*>\\(.*\\)</a>.*|$_name_|" \
	-e "/^=....=/!d"    "$SITEFILE" > "$MK_INFO"
}

make_sitemap_sect() 
{
    # scan used pages and store prime section group relation =sect= and =node=
    # (A) each "use1" creates "=sect=href+ href1" for all following non-"use1"
    # (B) each "use1" creates "=node=href2 href1" for all following "use2"
    $SED -e "/=use.=/!d" \
	-e "/=use1=/{" -e "h" -e "s:=use1=\\([^ ]*\\) .*:\\1:" -e "x" -e "}" \
	-e "s/=use.=\\([^ ]*\\) .*/=sect=\\1/" \
	-e G -e "s:\\n: :" "$MK_INFO" >> "$MK_INFO"
    $SED -e "/=use.=/!d" \
	-e "/=use1=/{" -e "h" -e "s:=use1=\\([^ ]*\\) .*:\\1:" -e "x" -e "}" \
	-e "/=use[13456789]=/d" \
	-e "s/=use.=\\([^ ]*\\) .*/=node=\\1/" \
	-e G -e "s:\\n: :" "$MK_INFO" >> "$MK_INFO"
}

make_sitemap_page()
{
    # scan used pages and store secondary group relation =page= and =node=
    # the parenting =node= for use3 is usually a use2 (or use1 if none there)
    $SED -e "/=use.=/!d" \
	-e "/=use1=/{" -e "h" -e "s:=use1=\\([^ ]*\\) .*:\\1:" -e "x" -e "}" \
	-e "/=use2=/{" -e "h" -e "s:=use2=\\([^ ]*\\) .*:\\1:" -e "x" -e "}" \
	-e "/=use[1]=/d" \
	-e "s/=use.=\\([^ ]*\\) .*/=page=\\1/" \
	-e G -e "s:\\n: :" "$MK_INFO" >> "$MK_INFO"
    $SED -e "/=use.=/!d" \
	-e "/=use1=/{" -e "h" -e "s:=use1=\\([^ ]*\\) .*:\\1:" -e "x" -e "}" \
	-e "/=use2=/{" -e "h" -e "s:=use2=\\([^ ]*\\) .*:\\1:" -e "x" -e "}" \
	-e "/=use[12456789]=/d" \
	-e "s/=use.=\\([^ ]*\\) .*/=node=\\1/" \
	-e G -e "s:\\n: :" "$MK_INFO" >> "$MK_INFO"
    # and for the root sections we register ".." as the parenting group
    $SED -e "/=use1=/!d" \
	-e "s/=use.=\\([^ ]*\\) .*/=node=\\1 ../"  "$MK_INFO" >> "$MK_INFO"
}
echo_site_filelist()
{
    $SED -e "/=use.=/!d" -e "s/=use.=//" -e "s/ .*//" "$MK_INFO"
}

# ==========================================================================
# originally this was a one-pass compiler but the more information
# we were scanning out the more slower the system ran - since we
# were rescanning files for things like section information. Now
# we scan the files first for global information.
#                                                                    1.PASS

scan_sitefile () # $F
{
 SOURCEFILE=`html_sourcefile "$F"`
 if test "$SOURCEFILE" != "$F" ; then
   dx_init "$F"
   dx_text today "`timetoday`"
   short=`echo "$F" | $SED -e "s:.*/::" -e "s:[.].*::"` # basename for all exts
   short="$short ~"
   DC_meta title "$short"
   DC_meta date.available "`timetoday`"
   DC_meta subject sitemap
   DC_meta DCMIType Collection
   DC_VARS_Of "$SOURCEFILE"  ; HTTP_VARS_Of "$SOURCEFILE"
   DC_modified "$SOURCEFILE" ; DC_date "$SOURCEFILE"
   DC_section "$F"
   DX_text date.formatted `timetoday`
   test ".$printerfriendly" != "." && \
   DX_text "printerfriendly" `fast_html_printerfile "$F"`
   test ".$USER" != "." && DC_publisher "$USER"
   echo "'$SOURCEFILE': $short (sitemap)"
   site_map_list_title "$F" "$short"
   site_map_long_title "$F" "generated sitemap index"
   site_map_list_date  "$F" "`timetoday`"
 fi
}

scan_htmlfile() # "$F"
{
 SOURCEFILE=`html_sourcefile "$F"`                                    # SCAN :
 if test "$SOURCEFILE" != "$F" ; then :                               # HTML :
 if test -f "$SOURCEFILE" ; then make_fast "$F" > "$tmp/$F.$FAST"
   dx_init "$F"
   dx_text today "`timetoday`"
   dx_text todays "`timetodays`"
   DC_VARS_Of "$SOURCEFILE" ; HTTP_VARS_Of "$SOURCEFILE"
   DC_title "$SOURCEFILE"
   DC_isFormatOf "$SOURCEFILE" 
   DC_modified "$SOURCEFILE" ; DC_date "$SOURCEFILE" ; DC_date "$SITEFILE"
   DC_section "$F" ;  DC_selected "$F" ;  DX_alternative "$SOURCEFILE"
   test ".$USER" != "." && DC_publisher "$USER"
   DX_text date.formatted "`timetoday`"
   test ".$printerfriendly" != "." && \
   DX_text "printerfriendly" `fast_html_printerfile "$F"`
   sectn=`info_get_entry DC.relation.section`
   short=`info_get_entry DC.title.selected`
   site_map_list_title "$F" "$short"
   info_map_list_title "$F" "$short"
   title=`info_get_entry DC.title`
   site_map_long_title "$F" "$title"
   info_map_long_title "$F" "$title"
   edate=`info_get_entry DC.date`
   issue=`info_get_entry issue`
   site_map_list_date "$F" "$edate"
   info_map_list_date "$F" "$edate"
   echo "'$SOURCEFILE':  '$title' ('$short') @ '$issue' ('$sectn')"
 else
   echo "'$SOURCEFILE': does not exist"
   site_map_list_title "$F" "$F"
   site_map_long_title "$F" "$F (no source)"
 fi ; else
   echo "<$F> - skipped"
 fi
}

scan_namespec ()
{
    # nothing so far
    case "$1" in
	name:sitemap:*)
	    short=`echo "$F" | $SED -e "s:.*/::" -e "s:[.].*::"` 
	    short=`echo "$short ~" | $SED -e "s/name:sitemap://"` 
	    site_map_list_title "$F" "$short"
	    site_map_long_title "$F" "external sitemap index"
	    site_map_list_date  "$F" "`timetoday`"
	    echo "'$F' external sitemap index$n" 
	    ;;
    esac
}
scan_httpspec ()
{
    # nothing so far
    return;
}

skip_namespec ()
{
    # nothing so far
    return;
}
skip_httpspec ()
{
    # nothing so far
    return;
}

# ==========================================================================
# and now generate the output pages
#                                                                   2.PASS

head_sed_sitemap() # $filename $section
{
   FF="$1"
   SECTION=`sed_slash_key "$2"`
   SECTS="<!--sect[$NN$AZ]-->" ; SECTN="<!--sect[$NN]-->" # lines with hrefs
   echo "/^$SECTS.*<a href=\"$FF\">/s|</a>|</a></b>|"          # $++
   echo "/^$SECTS.*<a href=\"$FF\">/s|<a href=|<b><a href=|"   # $++
   test ".$sectiontab" != ".no" && \
   echo "/ href=\"$SECTION\"/s|^<td class=\"[^\"]*\"|<td |"    # $++
}

head_sed_listsection() # $filename $section
{
   # traditional.... the sitefile is the full navigation bar
   FF=`sed_slash_key "$1"`
   SECTION=`sed_slash_key "$2"`
   SECTS="<!--sect[$NN$AZ]-->" ; SECTN="<!--sect[$NN]-->" # lines with hrefs
   echo "/^$SECTS.*<a href=\"$FF\">/s|</a>|</a></b>|"          # $++
   echo "/^$SECTS.*<a href=\"$FF\">/s|<a href=|<b><a href=|"   # $++
   test ".$sectiontab" != ".no" && \
   echo "/ href=\"$SECTION\"/s|^<td class=\"[^\"]*\"|<td |"    # $++
}

head_sed_multisection() # $filename $section
{
   # sitefile navigation bar is split into sections
   FF=`sed_slash_key "$1"`
   SECTION=`sed_slash_key "$2"`
   SECTS="<!--sect[$NN$AZ]-->" ; SECTN="<!--sect[$NN]-->" # lines with hrefs
   # grep all pages with a =sect= relation to current $SECTION and
   # build foreach an sed line "s|$SECTS\(<a href=$F>\)|<!--sectX-->\1|"
   # after that all the (still) numeric SECTNs are deactivated / killed.
   for section in $SECTION $headsection $tailsection ; do
       test ".$section" = ".no" && continue
   $SED -e "/^=sect=[^ ]* $section/!d" \
        -e "s, .*,\"\\\\)|<!--sectX-->\\\\1|,"  \
        -e "s,^=sect=,s|^$SECTS\\\\(.*<a href=\"," "$MK_INFO"  # $++
   done
   echo "s|^$SECTN[^ ]*\\(<a href=[^<>]*>\\).*|<!-- \\1 -->|"  # $++
   echo "/^$SECTS.*<a href=\"$FF\">/s|</a>|</a></b>|"          # $++
   echo "/^$SECTS.*<a href=\"$FF\">/s|<a href=|<b><a href=|"   # $++
   test ".$sectiontab" != ".no" && \
   echo "/ href=\"$SECTION\"/s|^<td class=\"[^\"]*\"|<td |"    # $++
}

make_sitefile () # "$F"
{
 SOURCEFILE=`html_sourcefile "$F"`
 if test "$SOURCEFILE" != "$F" ; then
 if test -f "$SOURCEFILE" ; then 
   # remember that in this case "${SITEFILE}l" = "$F" = "${SOURCEFILE}l"
   info2vars_sed > $MK_VARS           # have <!--title--> vars substituted
   info2meta_sed > $MK_META           # add <meta name="DC.title"> values
   if test ".$simplevars" = ".warn" ; then
   info2test_sed > $MK_TEST           # check <!--title--> vars old-style
   $SED_LONGSCRIPT "$MK_TEST" "$SOURCEFILE" | tee -a "$MK_OLDS" ; fi
   F_HEAD="$tmp/$F.$HEAD" ; F_FOOT="$tmp/$F.$FOOT"
   $CAT "$MK_PUTS"                                    > "$F_HEAD"
   head_sed_sitemap "$F" "`info_get_entry_section`"  >> "$F_HEAD"
   echo "/<head>/r $MK_META"                         >> "$F_HEAD"
   $CAT "$MK_VARS" "$MK_TAGS"                        >> "$F_HEAD"
   echo "/<\\/body>/d"                               >> "$F_HEAD"
   case "$sitemaplayout" in
   multi) make_multisitemap > "$F_FOOT" ;;       # here we use ~foot~ to
   *)     make_listsitemap  > "$F_FOOT" ;;       # hold the main text
   esac

   mkpathfile "$F"
   $SED_LONGSCRIPT "$F_HEAD"               "$SITEFILE"  > $F   # ~head~
   $CAT            "$F_FOOT"                           >> $F   # ~body~
   $SED -e "/<\\/body>/!d" -f "$MK_VARS"   "$SITEFILE" >> $F   #</body>
   echo "'$SOURCEFILE': " `ls -s $SOURCEFILE` ">->" `ls -s $F` "(sitemap)"
 else
   echo "'$SOURCEFILE': does not exist"
 fi fi
}

make_htmlfile() # "$F"
{
 SOURCEFILE=`html_sourcefile "$F"`                      #     2.PASS
 if test "$SOURCEFILE" != "$F" ; then
 if test -f "$SOURCEFILE" ; then
   if grep '<meta name="formatter"' "$SOURCEFILE" >/dev/null ; then
     echo "$SOURCEFILE: SKIP, this sourcefile looks like a formatted file"
     echo "$SOURCEFILE:  (may be a sourcefile in place of a targetfile?)"
     return
   fi
   info2vars_sed > $MK_VARS           # have <!--$title--> vars substituted
   info2meta_sed > $MK_META           # add <meta name="DC.title"> values
   if test ".$simplevars" = ".warn" ; then
   info2test_sed > $MK_TEST           # check <!--title--> vars old-style
   $SED_LONGSCRIPT "$MK_TEST" "$SOURCEFILE" | tee -a "$MK_OLDS" ; fi
   F_HEAD="$tmp/$F.$HEAD" ; F_BODY="$tmp/$F.$BODY" ; F_FOOT="$tmp/$F.$FOOT"
   $CAT "$MK_PUTS"                        > "$F_HEAD"
   case "$sectionlayout" in
   multi) head_sed_multisection "$F" "`info_get_entry_section`" >> "$F_HEAD" ;;
       *) head_sed_listsection  "$F" "`info_get_entry_section`" >> "$F_HEAD" ;;
   esac
      $CAT "$MK_VARS" "$MK_TAGS"            >> "$F_HEAD" #tag and vars
      echo "/<\\/body>/d"                   >> "$F_HEAD" #cut lastline
      echo "/<head>/r $MK_META"             >> "$F_HEAD" #add metatags
      echo "/<title>/d"                      > "$F_BODY" #not that line
      $CAT "$MK_VARS" "$MK_TAGS"            >> "$F_BODY" #tag and vars
      bodymaker_for_sectioninfo             >> "$F_BODY" #if sectioninfo
      info2body_sed                         >> "$F_BODY" #cut early
      info2head_sed                         >> "$F_HEAD"
      $CAT "$tmp/$F.$FAST"                  >> "$F_HEAD"
      test ".$emailfooter" != ".no" && \
      body_for_emailfooter                   > "$F_FOOT"

      mkpathfile "$F"
      $SED_LONGSCRIPT "$F_HEAD" $SITEFILE                > $F # ~head~
      $SED_LONGSCRIPT "$F_BODY" $SOURCEFILE             >> $F # ~body~
      test -f "$F_FOOT" && $CAT "$F_FOOT"               >> $F # ~foot~
      $SED -e "/<\\/body>/!d" -f "$MK_VARS" "$SITEFILE" >> $F #</body>
   echo "'$SOURCEFILE': " `ls -s $SOURCEFILE` "->" `ls -s $F`
 else # test -f $SOURDEFILE
   echo "'$SOURCEFILE': does not exist"
 fi ; else
   echo "<$F> - skipped"
 fi
}

make_printerfriendly () # "$F"
{                                                                 # PRINTER
  printsitefile="0"                                               # FRIENDLY
  P=`html_printerfile "$F"`
  F_FAST="$tmp/$F.$FAST"
  P_HEAD="$tmp/$P.$HEAD"
  P_BODY="$tmp/$P.$BODY"
  case "$F" in
  ${SITEFILE}|${SITEFILE}l) make_fast "$F" > "$F_FAST"
          printsitefile=">=>" ; BODY_TXT="$tmp/$F.$FOOT"  ;;
  *.html) printsitefile="=>" ;  BODY_TXT="$SOURCEFILE" ;;
  esac
  if grep '<meta name="formatter"' "$BODY_TXT" >/dev/null ; then return; fi
  if test ".$printsitefile" != ".0" && test -f "$SOURCEFILE" ; then
      make_printerfile_fast "$FILELIST" > ./$MK_FAST
      $CAT "$MK_VARS" "$MK_TAGS" "$MK_FAST" > "$P_HEAD"
      $SED -e "/DC.relation.isFormatOf/s|content=\"[^\"]*\"|content=\"$F\"|" \
           "$MK_META" > "$MK_METT"
      echo "/<head>/r $MK_METT"                       >> "$P_HEAD" # meta
      echo "/<\\/body>/d"                             >> "$P_HEAD"
      select_in_printsitefile "$F"                    >> "$P_HEAD"
      _ext_=`print_extension "$printerfriendly"`                     # head-
      $SED -e "s/[.]html\"|/$_ext_&/g" "$F_FAST"      >> "$P_HEAD" # hrefs
      # line_=`sed_slash_key "$printsitefile_img_2"`                   # back-
      echo "/||topics:/s| href=\"[#][.]\"| href=\"$F\"|" >> "$P_HEAD"
      echo "/|||pages:/s| href=\"[#][.]\"| href=\"$F\"|" >> "$P_HEAD"
      $CAT                             "$F_FAST"      >> "$P_HEAD" # subdir
      $CAT "$MK_VARS" "$MK_TAGS" "$MK_FAST"            > "$P_BODY"
      $SED -e "s/[.]html\"|/$_ext_&/g" "$F_FAST"      >> "$P_BODY" # body-
      $CAT                             "$F_FAST"      >> "$P_BODY" # hrefs

      mkpathfile "$P"
      $SED_LONGSCRIPT "$P_HEAD"              $PRINTSITEFILE  > $P # ~head~
      $SED_LONGSCRIPT "$P_BODY"                   $BODY_TXT >> $P # ~body~
      $SED -e "/<\\/body>/!d" -f $MK_VARS $PRINTSITEFILE >> $P #</body>
   echo "'$SOURCEFILE': " `ls -s $SOURCEFILE` "$printsitefile" `ls -s $P`
   fi 
}


# ========================================================================
# ========================================================================
# ========================================================================

# ========================================================================
#                                                          #### 0. INIT
make_sitemap_init
make_sitemap_list
make_sitemap_sect
make_sitemap_page

FILELIST=`echo_site_filelist`
if test ".$opt_filelist" != "." || test ".$opt_list" = ".file"; then
   for F in $FILELIST; do echo "$F" ; done ; exit # --filelist
fi
if test ".$opt_files" != "." ; then FILELIST="$opt_files" ; fi # --files
if test ".$FILELIST" = "."; then echo "nothing to do" >&2 ; fi
if test ".$FILELIST" = ".SITEFILE" ; then echo "only '$SITEFILE'?!" >&2 ; fi

for F in $FILELIST ; do case "$F" in                       #### 1. PASS
name:*)                   scan_namespec "$F" ;;
http:*|*://*)             scan_httpspec "$F" ;;
${SITEFILE}|${SITEFILE}l) scan_sitefile "$F" ;;   # ........... SCAN SITE
../*) 
   echo "!! -> '$F' (skipping topdir build)"
   ;;
# */*.html) 
#    make_fast  > $F.$FAST # try for later subdir build
#    echo "!! -> '$F' (skipping subdir build)"
#    ;;
# */*/*/|*/*/|*/|*/index.htm|*/index.html) 
#    echo "!! -> '$F' (skipping subdir index.html)"
#    ;;
*.html) scan_htmlfile "$F" ;;                      # ........... SCAN HTML
*/) echo "'$F' : directory - skipped"
   site_map_list_title "$F" "`sed_slash_key $F`"
   site_map_long_title "$F" "(directory)"
   ;;
*) echo "?? -> '$F'"
   ;;
esac done

if test ".$printerfriendly" != "." ; then           # .......... PRINT VERSION
  _ext_=`print_extension "$printerfriendly" | sed -e "s/&/\\\\&/"`
  PRINTSITEFILE=`echo "$SITEFILE" | sed -e "s/\\.[$AA]*\$/$_ext_&/"`
  echo "NOTE: going to create printer-friendly sitefile $PRINTSITEFILE"
  make_printsitefile > "$PRINTSITEFILE"
fi

if test ".$simplevars" = ". " ; then
mknewfile $MK_OLDS 
fi

for F in $FILELIST ; do case "$F" in                        #### 2. PASS
name:*)                    skip_namespec "$F" ;; 
http:*|*://*)              skip_httpspec "$F" ;;
${SITEFILE}|${SITEFILE}l)  make_sitefile "$F"           # ........ SITE FILE
    if test ".$printerfriendly" != "." ; then make_printerfriendly "$F" ; fi ;;
../*) 
    echo "!! -> '$F' (skipping topdir build)"
    ;;
# */*.html) 
#   echo "!! -> '$F' (skipping subdir build)"
#   ;;
# */*/*/|*/*/|*/|*/index.htm|*/index.html) 
#   echo "!! -> '$F' (skipping subdir index.html)"
#   ;;
*.html)  make_htmlfile "$F"                  # .................. HTML FILES
    if test ".$printerfriendly" != "." ; then make_printerfriendly "$F" ; fi ;;
*/) echo "'$F' : directory - skipped"
    ;;
*)  echo "?? -> '$F'"
    ;;
esac
# .............. debug ....................
   if test -d DEBUG && test -f "./$F" ; then
      FFFF=`echo "$F" | sed -e s,/,:,g`
      cp "$tmp/$F.$INFO" DEBUG/$FFFF.info.TMP
      for P in tags vars meta page date list html sect info ; do
      test -f $tmp/$MK.$P.tmp && cp $tmp/$MK.$P.tmp DEBUG/$FFFF.$P.tmp
      test -f $tmp/$MK.$P.TMP && cp $tmp/$MK.$P.TMP DEBUG/$FFFF.$P.TMP
      done
   fi
done

if test ".$simplevars" = ".warn" ; then if test -f "$MK_OLDS" ; then
oldvars=`cat "$MK_OLDS" | wc -l | $SED -e "s/ *//g"`
if test "$oldvars" = "0" ; then
echo "HINT: you have no simplevars in your htm sources, so you may want to"
echo "hint: set the magic <!--mksite:nosimplevars--> in your $SITEFILE"
echo "hint: which makes execution _faster_ actually in the 2. pass"
echo "note: simplevars expansion was the oldstyle way of variable expansion"
else
echo "HINT: there were $oldvars simplevars found in your htm sources."
echo "hint: This style of variable expansion will be disabled in the near"
echo "hint: future. If you do not want change then add the $SITEFILE magic"
echo "hint: <!--mksite:simplevars--> somewhere to suppress this warning"
echo "note: simplevars expansion will be an explicit option in the future."
echo "note: errornous simplevar detection can be suppressed with a magic"
echo "note: hint of <!--mksite:nosimplevars--> in the $SITEFILE for now."
fi fi fi

rm $tmp/$MK.*.tmp
if test ".$tmp_dir_was_created" != ".no" ; then rm $tmp/* ; rmdir $tmp ; fi
exit 0
