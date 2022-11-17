#!/bin/sh

# This script outputs additional MPFR version information for a
# Git working tree (Git branch or "(none)", total commit count,
# commit id, and whether the current HEAD is modified). It is
# called in tests/Makefile.am for "make check", but may be used
# by other tools that need such information.
# Note that this does not replace version information found in
# the VERSION file, which may still need to be output in addition
# to the output of this script.

set -e

if [ "x`git rev-parse --is-inside-work-tree 2> /dev/null`" != xtrue ]; then
  echo "$0: This script should be executed from a Git working tree." >&2
  exit 1
fi

# Normally passed by tests/Makefile.am with "GREP=$(GREP) SED=$(SED)".
GREP=${GREP:-grep}
SED=${SED:-sed}

git tag --contains | $SED -n 's/-root$//p' > excluded-branches
gitb=`git branch --format='%(refname:short)' --contains | \
        $SED 's,(HEAD detached at origin/\(.*\)),\1,' | \
        $GREP -v '^(' | $GREP -v -F -f excluded-branches -x || true`
rm excluded-branches
gitc=`git rev-list --count HEAD`
gith=`git rev-parse --short HEAD`
gitm=`git update-index -q --refresh; git diff-index --name-only HEAD`
echo "${gitb:-(none)}-$gitc-$gith${gitm:+ (modified)}"

# References:
#   https://stackoverflow.com/q/3882838/3782797
#   https://stackoverflow.com/a/3899339/3782797
#     for the "git diff-index --name-only HEAD" solution, but this
#     is not sufficient, because autogen.sh modifies the "INSTALL"
#     and "doc/texinfo.tex" files (due to "autoreconf -f -i"), and
#     restores them. On needs:
#   https://stackoverflow.com/q/3882838/3782797#comment121636904_3899339
#     suggesting "git update-index -q --refresh" first.
