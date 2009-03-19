##
noinst_DATA = ../subutilsdir-configure

DISTCLEANFILES = $(noinst_DATA)

all-local: $(noinst_DATA)

../subutilsdir-configure:
	echo "${srcdir}/configure" | sed 's,dummy/,SUBUTILSDIR/,' >$@

