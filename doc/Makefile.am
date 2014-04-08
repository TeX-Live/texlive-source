## Makefile.am for the TeX Live subdirectory ./doc/
##
## Copyright 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## The info files
info_TEXINFOS = texlive.texi
texlive_TEXINFOS = build.texi build-tools.txt

EXTRA_DIST = ${PDFS}

# just for development of the doc.
readme-files: texlive.texi
	rm -f README.*
	makeinfo --no-split -o /tmp/i $< # for error checking
	makeinfo --plaintext -o - $< \
	| tee /tmp/a \
	| gawk -f $(srcdir)/splitinfo.gawk
	ls -l README.*
.PHONY: readme-files
