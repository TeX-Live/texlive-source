#! /bin/sh
# creates a c header file with the current subversion version
# This script should be run within the source directory.

$DEBUG

FILE="texk/web2c/luatexdir/luatex_svnversion.h"

LANG=C
if [ ! -r $FILE ]
then
  echo '#define luatex_svn_revision -1' > $FILE
fi
if ( svn info . >/dev/null 2>&1 && svnversion > /dev/null )
then
  # svn up > /dev/null
  DEFREV=`cat $FILE`
  SVNREV=`svnversion -c . | sed -ne 's/^[0-9]*:*\([0-9]*\).*/#define luatex_svn_revision \1/p'`
  test "$DEFREV" != "$SVNREV" && echo "$SVNREV" > $FILE
elif ( [ -f ../.git/refs/remotes/git-svn ] || [ -d ../.git/svn ] && git svn --version > /dev/null )
then
  DEFREV=`cat $FILE`
  SVNREV=`git svn info | sed -ne 's/^Revision: \([0-9]*\).*$/#define luatex_svn_revision \1/p'`
  test "$DEFREV" != "$SVNREV" && echo "$SVNREV" > $FILE
fi
