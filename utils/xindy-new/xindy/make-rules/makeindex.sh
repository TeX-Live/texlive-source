#!/bin/sh
#
# This script is at the same time the documentation for the 
# localization architecture used in Xindy/make-rules.
# (c) 2001 Martin Vermeer
#
# Print out help text:
#
if [ "$1" = "" ] ; then
  echo ""
  echo "Syntax:" 
  echo "  ./makeindex.sh [[-v <var>] [-e <enc>] -m <lang>]* [-d <dp>] [-s <style>] <myindex>"
  echo ""
  echo "  where <myindex> is the index file name *without* extension."
  echo ""
  echo "Multiple -m options allowed; -m always after corresponding -v, -e."
  echo ""
  echo "Languages <lang>:"
  echo "  albanian"
  echo "  belarusian    finnish        swedish"
  echo "  bulgarian     french         hungarian"
  echo "  icelandic     norwegian      latin"
  echo "  croatian      general        polish"
  echo "  portuguese    romanian       slovenian"
  echo "  czech         german         russian"
  echo "  danish        greek          serbian"
  echo "  english       slovak         ukrainian"
  echo "  esperanto     macedonian     spanish"
  echo "  turkish       kurdish        georgian"
  echo "  estonian      latvian        lithuanian"
  echo "  gypsy         hausa          hebrew"
  echo "  gypsy         hausa          hebrew"
  echo "  upper-sorbian lower-sorbian  klingon"
  echo "  mongolian     vietnamese"
  echo ""
  echo "Variants <var>:"
  echo "  german:    duden, din5007"
  echo "  spanish:   traditional, modern"
  echo "  slovak:    large, small"
  echo "  gypsy:     northrussian"
  echo "  kurdish:   turkish-i, bedirxan"
  echo "  mongolian: cyrillic"
  echo ""
  echo "Docprocessors <dp>:"
  echo "  -t :    -d tex"
  echo "  -n :    -d nroff"
  echo ""
  echo "Styles <style>:"
  echo "  -s <style>"
  echo "  -s makeindex (default)"
  exit 0
fi

LOC=""
ENC=""
VAR=""

TEMP=`mktemp /tmp/xindy.XXXXXX` || exit 1

echo "" >$TEMP.xdy

# Default document processor and index style
#
DP="tex"
STYLE="makeindex"

while getopts "m:v:d:e:tnls:" OPT
do
  case "$OPT" in
    "m")
      LOC="$OPTARG"
      # Language two-letter codes following ISO-639-1
      case $LOC in
        "english")
        ENC_="ascii"
        PRE="en"
        ;;
        "finnish")
        ENC_="cp1252"
        PRE="fi"
        ;;
        "german")
        ENC_="latin1"
        PRE="de"
        VAR_="din5007-"
        ;;
        "belarusian")
        ENC_="iso88595"
        PRE="be"
        ;;
        "danish")
        ENC_="latin1"
        PRE="dk"
        ;;
        "french")
        ENC_="latin9"
        PRE="fr"
        ;;
        "bulgarian")
        ENC_="iso88595"
        PRE="bg"
        ;;
        "croatian")
        ENC_="latin2"
        PRE="hr"
        ;;
        "czech")
        ENC_="latin2"
        PRE="cs"
        ;;
        "esperanto")
        ENC_="latin3"
        PRE="eo"
        ;;
        "greek")
        ENC_="iso88597"
        PRE="el"
        ;;
        "macedonian")
        ENC_="latin2"
        PRE="mk"
        ;;
        "norwegian")
        ENC_="latin1"
        PRE="no"
        ;;
        "polish")
        ENC_="latin2"
        PRE="pl"
        ;;
        "russian")
        ENC_="koi8-r"
        PRE="ru"
        ;;
        "serbian")
        ENC_="iso88595"
        PRE="sr"
        ;;
        "slovak")
        ENC_="latin2"
        PRE="sk"
        VAR_="small-"
        ;;
        "spanish")
        ENC_="latin1"
        PRE="es"
        VAR_="traditional-"
        ;;
        "swedish")
        ENC_="latin1"
        PRE="se"
        ;;
        "ukrainian")
        ENC_="koi8-u"
        PRE="uk"
        ;;
        "albanian")
        ENC_="latin1"
        PRE="sq"
        ;;
        "hungarian")
        ENC_="latin2"
        PRE="hu"
        ;;
        "icelandic")
        ENC_="latin1"
        PRE="is"
        ;;
        "lower-sorbian")
        ENC_="latin2"
        PRE="s1"
        ;;
        "upper-sorbian")
        ENC_="latin2"
        PRE="s2"
        ;;
        "portuguese")
        ENC_="latin1"
        PRE="pt"
        ;;
        "romanian")
        ENC_="latin2"
        PRE="ro"
        ;;
        "slovenian")
        ENC_="latin2"
        PRE="sl"
        ;;
        "turkish")
        ENC_="latin5"
        PRE="tr"
        ;;
        "dutch")
        ENC_="latin1"
        PRE="nl"
        ;;
        "estonian")
        ENC_="latin1"
        PRE="et"
        ;;
        "georgian")
        ENC_="georgian"
        PRE="ka"
        ;;
        "gypsy")
        ENC_="utf8"
        PRE="g1"
		VAR="northrussian-"
        ;;
        "hausa")
        ENC_="utf8"
        PRE="ha"
		;;
        "hebrew")
        ENC_="iso88598"
        PRE="iw"
		;;
        "italian")
        ENC_="latin1"
        PRE="it"
		;;
        "klingon")
        ENC_="utf8"
        PRE="kl"
		;;
        "kurdish")
        ENC_="latin5"
        PRE="kl"
		VAR="turkish-i-"
		;;
        "latin")
        ENC_="utf8"
        PRE="la"
		;;
        "latvian")
        ENC_="latin4"
        PRE="lv"
		;;
        "lithuanian")
        ENC_="latin4"
        PRE="lt"
		;;
        "mongolian")
        ENC_="utf8"
        PRE="mn"
		VAR="cyrillic-"
		;;
        "vietnamese")
        ENC_="utf8"
        PRE="vi"
		;;
     esac
      IGNORESPECIAL="$IGNORESPECIAL \"$PRE-ignore-special\"" 
      ALPHABETIZE="$ALPHABETIZE \"$PRE-alphabetize\"" 
      RESOLVEDIACRITICS="$RESOLVEDIACRITICS \"$PRE-resolve-diacritics\"" 
      RESOLVECASE="$RESOLVECASE \"$PRE-resolve-case\"" 
      RESOLVESPECIAL="$RESOLVESPECIAL \"$PRE-resolve-special\"" 
      if [ "$ENC" = "" ] ; then
        ENC=$ENC_  
      fi
      if [ "$VAR" = "" ] ; then
        VAR=$VAR_  
      fi

      #
      # ** Create file(s) containing locale-related req's: **
      #
      # This file contains:
      # - the document processor specific stuff that was not 
      #   generated (correctly) automatically;
      # - a call to inputenc/<encoding>.xdy, which was generated
      #   automatically by make-enc-rules.pl.
      # This file is *created by hand for every language* 
      # (and doc processor, variant and encoding)
      #
      echo "(require \"lang/$LOC/$DP-$VAR$ENC.xdy\")" >>$TEMP.xdy
      #
      # This file will contain the language and encoding and variant 
      # specific stuff. Nothing document specific here. Generated
      # automatically by make-rules.pl.
      #
      echo "(require \"lang/$LOC/$VAR$ENC.xdy\")" >>$TEMP.xdy

      ENC=""
      VAR=""
      ;;
    "v")
      VAR="$OPTARG-"
      ;;
    "d")
      DP="$OPTARG"
      ;;
    "e")
      ENC="$OPTARG"
      ;;
    "t")
      DP="tex"
      ;;
    "n")
      DP="nroff"
      ;;
    "l")
      DP="lout"
      ;;
    "s")
      STYLE="$OPTARG"
      ;;
  esac
done

#
# Default language:
#
if [ "$LOC" = "" ] ; then
  LOC="english"
  ENC="ascii"
  PRE="en"
  IGNORESPECIAL="\"en-ignore-special\""
  ALPHABETIZE="\"en-alphabetize\""
  RESOLVEDIACRITICS="\"en-resolve-diacritics\"" 
  RESOLVECASE="\"en-resolve-case\"" 
  RESOLVESPECIAL="\"en-resolve-special\"" 
  # Do this in the absence of -m options:
  echo "(require \"lang/$LOC/$DP-$VAR$ENC.xdy\")" >>$TEMP.xdy
  echo "(require \"lang/$LOC/$VAR$ENC.xdy\")" >>$TEMP.xdy

fi

shift $[$#-1]
DATA=$1

tex2xindy <$DATA.idx >$DATA.raw

#
# Language/variant sorting rules, possibly multi:
#
echo "(define-sort-rule-orientations (forward backward forward forward))" \
	>>$TEMP.xdy
echo "(use-rule-set :run 0 :rule-set \
	($ALPHABETIZE $IGNORESPECIAL))" >>$TEMP.xdy
echo "(use-rule-set :run 1 :rule-set \
	($RESOLVEDIACRITICS $IGNORESPECIAL))" >>$TEMP.xdy
echo "(use-rule-set :run 2 :rule-set \
	($RESOLVECASE $IGNORESPECIAL))" >>$TEMP.xdy
echo "(use-rule-set :run 3 :rule-set \
	($RESOLVESPECIAL))" >>$TEMP.xdy

#
# The index style chosen:
#
echo "(require \"styles/$STYLE.xdy\")" >>$TEMP.xdy

xindy $TEMP.xdy $DATA.raw

