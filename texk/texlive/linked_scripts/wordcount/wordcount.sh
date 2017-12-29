#!/bin/sh
# wordcount.sh 1.7 Copyright 2016/06/03 Michael John Downes

# Following Michael's death, his family have requested that his TeX
# related files are made publicly available under the LPPL license.
#
# This file is available under the LaTeX Project Public License version 1
# or (at your option) any later version.
# https://latex-project.org/lppl/
# (LPPL maintenance status: unmaintained)
#
# This is a Unix Bourne shell script that counts the characters and
# spaces in a LaTeX document by running it through LaTeX using
# wordcount.tex, and then counting the lines in the resulting
# wordcount.log file that match a special pattern. It assumes that you
# have a program "grep" with a -c option meaning "Report the number of
# matching lines". To use perl or some other tool instead of grep,
# redefine $COUNT in some suitable way, below.
#
# The characters that are counted with this method are actual font
# characters in the page body (excluding running heads): in math, each
# subscript and superscript character is counted separately, even when
# they appear one over the other; and so are the separate combining
# pieces used to make large delimiters. Interword spaces are counted as
# separate characters, and so are the spaces around mathrel symbols in
# math formulas (equal signs and the like)---but not the spaces around
# mathbin symbols (plus, minus, and the like).
# 
# You can count words by dividing the character count by 5 (or
# whatever), or by counting the occurrences of interword spaces. The
# latter method (illustrated here) is highly accurate at counting the
# actual number of words---until tables or math formulas enter the
# picture. In the case of tables, the number of words will tend to be
# overestimated because tabskip glue, counted as an interword space,
# appears on both sides of each table cell. (Not counting the tabskip
# glue at all would result in an underestimate, and the underestimate
# would tend to be slightly less accurate than the overestimate that we
# have chosen to do.)
#
# And of course, in the case of math formulas, how do you define what is
# a "word"?

SCRIPT=`basename $0`
LOGFILE=wordcount.log
COUNT="grep -c"
LATEX=latex

die () {
  echo "$@ was apparently unsuccessful."
  exit 1
}

no_files_msg () {
  echo "$SCRIPT: At least one file name must be supplied" \
       "on the command line."
  exit 1
}

case X$1 in
  X) no_files_msg ;;
  *) : ;;
esac

for f in "$@"
do
  # Since \? is not defined by LaTeX itself, this use via \csname causes
  # it to be defined as \relax, and it then serves as a filename
  # terminator. And within wordcount.tex if \? is found to be no longer
  # undefined, certain messages are suppressed.
  echo "$f" | $LATEX 'wordcount.tex\csname ?\endcsname' || die "LaTeX run"
  # Count interword spaces and end-of-paragraph spaces
  words=`$COUNT '3[.]0863[35]' $LOGFILE` || die "Log search"
  # Count nonspace characters and interword spaces
  chars=`$COUNT '3[.]0863[23]' $LOGFILE` || die "Log search"
  # If you want to see what the log file looks like, comment out the
  # following line:
  rm $LOGFILE
  echo "$f contains $chars characters and $words words."
done
