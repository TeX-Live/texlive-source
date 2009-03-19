##
noinst_DATA = ../subtexkdir-configure

DISTCLEANFILES = $(noinst_DATA)

all-local: $(noinst_DATA)

../subtexkdir-configure:
	echo "${srcdir}/configure" | sed 's,dummy/,SUBTEXKDIR/,' >$@

