##
noinst_DATA = ../sublibsdir-configure

DISTCLEANFILES = $(noinst_DATA)

all-local: $(noinst_DATA)

../sublibsdir-configure:
	echo "${srcdir}/configure" | sed 's,dummy/,SUBLIBSDIR/,' >$@

