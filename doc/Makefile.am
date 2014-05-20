## Makefile.am for the TeX Live subdirectory ./doc/
##
## Copyright 2013-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## The info files
info_TEXINFOS = tlbuild.texi
texlive_TEXINFOS = build-tools.txt

EXTRA_DIST = build-tools.txt splitinfo.gawk

# just for development of the doc.
readme-files: tlbuild.texi
	rm -f README.*
	makeinfo --no-split -o /tmp/i $< # for error checking
	makeinfo --plaintext -o - $< \
	| tee /tmp/a \
	| gawk -f $(srcdir)/splitinfo.gawk
	ls -l README.*
#	head -8 README.*
.PHONY: readme-files
