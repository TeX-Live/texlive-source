## Makefile.am for the TeX Live subdirectory texk/dvipng/help/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
DVIPNG_HELP = $(top_srcdir)/doc/dvipng.help

all-local: help-stamp $(DVIPNG_HELP)

# If dvipng.help is to be rebuilt, this would fail for a read-only source
# directory, but rebuilding dvipng.info would equally fail.
# Do not even try to rebuild dvipng.help if help-stamp is up to date.
$(DVIPNG_HELP): help-stamp
help-stamp: ../dvipng$(EXEEXT)
if !cross
	-(cd .. && ./dvipng$(EXEEXT)) | \
	  sed -e's/dvipng.exe/dvipng/' \
	      -e's/ (dvipng) / /' \
	      -e's/ (dvipng (TeX Live)) / (TeX Live) /' \
	      -e's,/lt-dvipng ,/dvipng ,' \
	      -e's,^This is .*/dvipng ,This is ./dvipng ,' \
	      -e's,^Usage: .*/dvipng ,Usage: ./dvipng ,' > dvipng.tmp
	( test -r $(DVIPNG_HELP) && diff dvipng.tmp $(DVIPNG_HELP) ) \
		|| cp dvipng.tmp $(DVIPNG_HELP)
	rm -f dvipng.tmp
endif !cross
	echo timestamp >$@

DISTCLEANFILES = help-stamp

