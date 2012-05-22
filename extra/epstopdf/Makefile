# $Id$
# This file is public domain.  (Originally written by Karl Berry, 2009.)

e2p = ./epstopdf.pl #--debug
re2p = ./repstopdf --restricted
re2p_script = ./repstopdf

default: check

# These are not comprehensive tests by any means.  Mostly we're just
# making sure we don't crash.
check: check-help check-version \
       check-help-r \
       check-nogs \
       check-restricted-gscmd \
       check-filter check-filter-outfile check-write-error check-gscmd \
       check-simple check-atend check-binary check-bin2 \
       check-binhdr-lf check-binhdr-cr check-binhdr-crlf \

# needs new kpsewhich: check-restricted-safe-in check-restricted-safe-out
check-help:
	$(e2p) --help

check-version:
	$(e2p) --version

check-help-r: $(re2p_script)
	$(re2p_script) --help | grep restricted  # should have default=true

check-nogs:
	$(e2p) --no-gs test-simple.eps >test-simple.out
# the two files should be different:
	cmp -s test-simple.eps test-simple.out || exit 0
	
$(re2p_script):
	ln -s epstopdf.pl $@

check-restricted-gscmd:
	! $(re2p) --gscmd=/bin/unsafe test-simple.eps
	! $(re2p) --gscmd=unsafe test-simple.eps

check-restricted-safe-in:
	! $(re2p) /unsafe/in.eps

check-restricted-safe-out:
	! $(re2p) test-simple.eps -o /unsafe/out.eps

check-filter:
	cat test-simple.eps | $(e2p) --filter >test-filter.pdf
	pdfinfo test-filter.pdf | grep "Page size:"

check-filter-outfile:
	rm -f test-filter.pdf
	cat test-simple.eps | $(e2p) --filter --outfile=test-filter.pdf
	pdfinfo test-filter.pdf | grep "Page size:"
#
	rm -f test-filter.pdf
	cat test-simple.eps | $(e2p) --outfile=test-filter.pdf --filter
	pdfinfo test-filter.pdf | grep "Page size:"

check-write-error:
	-$(e2p) test-simple.eps --outfile=/dev/full

check-gscmd:
	-$(e2p) --gscmd=true test-simple.eps

check-simple:
	$(MAKE) file=test-simple check1

check-atend:
	$(MAKE) file=test-atend check1

check-binary:
	$(MAKE) file=test-binary check1

check-bin2:
	$(MAKE) file=test-bin2 check1

check-binhdr-lf:
	$(MAKE) file=test-binhdr-lf check1

check-binhdr-cr:
	$(MAKE) file=test-binhdr-cr check1

check-binhdr-crlf:
	$(MAKE) file=test-binhdr-crlf check1

check1:
	$(e2p) $(file).eps 
	pdfinfo $(file).pdf | grep "Page size"

dist: epstopdf.tar.gz epstopdf.html
epstopdf.tar.gz: README epstopdf.pl epstopdf.1 repstopdf.1 epstopdf.man1.pdf
	tar czf epstopdf.tar.gz $^

groff = groff
groff_opts = -man -t
pdfopts = $(groff_opts)
htmlopts = $(groff_opts) -Thtml
pspdf = ps2pdf -sPAPERSIZE=a4
epstopdf.man1.pdf: epstopdf.1
	$(groff) $(pdfopts) $< | $(pspdf) - $@
epstopdf.html: epstopdf.1
	$(groff) $(htmlopts) $< >$@

# upload .tar.gz to ctan.org/upload, move .html to ~www/epstopdf.
