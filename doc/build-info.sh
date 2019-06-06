#!/bin/sh
# $Id$
# Report build environment; run from top-level Makefile so any make
# overrides will be taken into account.

printf 'uname\t"%s"\n'    "`uname -a`"
test -s /etc/issue \
&& printf 'issue\t"%s"\n'    "`sed 1q /etc/issue`"
# bsd doesn't have /etc/issue, but seems sufficiently identified by uname.

printf 'MAKE-v\t"%s"\n'   "`${MAKE-make} -v 2>&1 | sed 1q`"
# BSD make does not give version info with -v, but the
# first line of the usage message is a sort of identifier.

# our configure defaults to using gcc and g++.
printf 'CC-v\t"%s"\n'     "`${CC-gcc} --version 2>&1 | sed 1q`"
printf 'CXX-v\t"%s"\n'    "`${CXX-g++} --version 2>&1 | sed 1q`"

# dump whole environment to be sure we get anything relevant,
# although it will surely include many irrelevancies.
env | sort -f

# the whole configure line and more.
test -s config.status \
&& (printf 'CONFIG_STATUS\n'; ./config.status --version)

exit 0
