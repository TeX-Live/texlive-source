#! /bin/bash --
#
# mkdist.sh by pts@fazekas.hu at Wed Mar  6 09:09:01 CET 2002
# added debian/changelog support at Fri Mar  5 19:37:45 CET 2004
# based on mkdist.sh of autotrace
#
#

if [ -f debian/changelog ]; then :; else
  echo "$0: missing: debian/changelog" >&2
  exit 2
fi
if [ -f files ]; then :; else
  echo "$0: missing: files" >&2
  exit 3
fi
PRO_VER="`<debian/changelog perl -ne 'print"$1-$2"if/^(\S+) +[(]([-.\w]+?)(?:-\d+)?[)] +\w+;/;last'`"
if [ "$PRO_VER" ]; then :; else
  echo "$0: couldn't determine version from debian/changelog" >&2
  exit 4
fi
if [ -e "$PRO_VER" ]; then
  echo "$0: $PRO_VER already exists, remove it first" >&2
  exit 5
fi

if [ $# -gt 0 ]; then
  TGZ_NAME="$1.tar.gz"; shift
else
  TGZ_NAME="$PRO_VER.tar.gz"
fi

set -e # exit on error
rm -f "../$TGZ_NAME"
mkdir "$PRO_VER"
echo "$PRO_VER"
(IFS='
'; exec tar -c -- `cat files` "$@") |
(cd "$PRO_VER" && exec tar -xv)
# ^^^ tar(1) magically calls mkdir(2) etc.

# vvv Dat: don't include sam2p-.../ in the filenames of the .tar.gz
#(IFS='
#'; cd "$PRO_VER" && exec tar -czf "../../$TGZ_NAME" -- `cat ../files` "$@")

# vvv Dat: do include sam2p-.../ in the filenames of the .tar.gz
(IFS='
'; export PRO_VER; exec tar -czf "../$TGZ_NAME" -- `perl -pe '$_="$ENV{PRO_VER}/$_"' files` "$@")

rm -rf "$PRO_VER"
set +e

if [ -s "../$TGZ_NAME" ]; then :; else
  echo "$0: failed to create dist: ../$TGZ_NAME" >&2
  exit 6
fi
(cd ..; echo "Created dist: `pwd`/$TGZ_NAME" >&2)

# __EOF__
