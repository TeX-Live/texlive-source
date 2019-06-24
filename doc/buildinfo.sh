#!/bin/sh
# $Id$
# Public domain. Report basics of current system; run from top-level
# Makefile so any make overrides will be taken into account. (And from
# Build.) buildenv.log with full environment dump is also created.

do_config_status=true
if test "x$1" = x--no-config-status; then
  shift
  do_config_status=false
fi

printf 'UNAME\t"%s"\n'    "`uname -a`"

# /etc/issue often contains only placeholders, so don't bother with it.

# Return version identification for $1, by calling it with --version.
# gcc on Macs, when linked to cc, has a useless "Configured with:" as
# the first line. Likely we'll need to generalize for other compilers.
# We intentionally don't quote $1 in case CC was set to something like
# "cc --someopt".
compiler_version () {
  $1 --version 2>&1 | grep -v '^Configured' | sed 1q
}

printf 'MAKE\t"%s"\n'     "${MAKE-make}"
printf 'MAKE-v\t"%s"\n'   "`${MAKE-make} -v 2>&1 | sed 1q`"
# BSD make does not give version info with -v, but the
# first line of the usage message is sort of an identifier.

# our configure defaults to using gcc and g++, so we will too.
printf 'CC\t"%s"\n'       "${CC-gcc}"
printf 'CFLAGS\t"%s"\n'   "${CFLAGS}"
printf 'CC-v\t"%s"\n'     "`compiler_version ${CC-gcc}`"
# 
printf 'CXX\t"%s"\n'      "${CXX-g++}"
printf 'CXXFLAGS\t"%s"\n' "${CXXFLAGS}"
printf 'CXX-v\t"%s"\n'    "`compiler_version ${CXX-g++}`"
#
printf 'OBJCXX\t"%s"\n'      "${OBJCXX-cc}"
printf 'OBJCXXFLAGS\t"%s"\n' "${OBJCXXFLAGS}"
#
printf 'LDFLAGS\t"%s"\n'  "${LDFLAGS}"

# Some Linux-based systems provide this, but don't worry if not there.
# Let's hope that other systems are sufficiently identified by uname,
# don't feel like doing a big system-information hunt. But if we do:
#   http://www.datadisk.co.uk/html_docs/misc/unix_commands.htm
if test -n "`lsb_release -a 2>/dev/null`"; then
  printf '\nLSB_RELEASE\n'
  lsb_release -a 2>&1
fi

# the whole configure line and more, if requested.
# (We want this from make, but not from Build.)
$do_config_status \
&& test -s config.status \
&& (printf '\nCONFIG_STATUS\n'; ./config.status --version | sed -n '1,/^$/p')

exit 0
