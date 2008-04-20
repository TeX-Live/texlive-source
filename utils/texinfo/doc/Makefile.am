# $Id: Makefile.am,v 1.37 2008/03/25 10:51:19 akim Exp $
# Makefile.am for texinfo/doc.
# Run automake in .. to produce Makefile.in from this.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Put texinfo.txi first because that's the most important.
info_TEXINFOS = texinfo.txi info-stnd.texi info.texi
DISTCLEANFILES = texinfo texinfo-* info*.info*

man_MANS = info.1 infokey.1 install-info.1 makeinfo.1 texindex.1  \
           info.5 texinfo.5

# Use the programs built in our distribution, taking account of possible
# cross-compiling.
MAKEINFO = $(top_builddir)/$(native_tools)/makeinfo/makeinfo

# We'd also like to use something like this, but Automake calls
# "install-info" directly.
# INSTALL_INFO = $(top_builddir)/$(native_tools)/util/ginstall-info

TXI_XLATE = txi-cs.tex txi-de.tex txi-en.tex txi-es.tex txi-fr.tex \
	    txi-it.tex txi-nl.tex txi-nb.tex txi-pl.tex txi-pt.tex \
	    txi-ru.tex txi-sr.tex txi-tr.tex txi-uk.tex

# Include our texinfo.tex, not Automake's.
EXTRA_DIST = epsf.tex texinfo.tex \
             fdl.texi \
	     $(man_MANS) $(TXI_XLATE)

if INSTALL_WARNINGS
install-data-local:
	@echo
	@echo "WARNING: If your texmf tree does not already contain"
	@echo "         up-to-date versions, you must install"
	@echo "             texinfo.tex and txi-??.tex manually,"
	@echo "         perhaps in TEXMF/tex/texinfo/,"
	@echo "         where TEXMF is a root of your TeX tree."
	@echo "         See doc/README for some considerations."
	@echo "         You can run make TEXMF=/your/texmf install-tex to do this."
	@echo
	@echo "         You may also need to install epsf.tex in"
	@echo "         TEXMF/tex/generic/dvips, if your TeX"
	@echo "         installation did not include it."
endif

texmf_texinfo = $(TEXMF)/tex/texinfo
texmf_dvips = $(TEXMF)/tex/generic/epsf
install-tex:
	test -n "$(TEXMF)" || (echo "TEXMF must be set." >&2; exit 1)
	$(mkinstalldirs) $(DESTDIR)$(texmf_texinfo) $(DESTDIR)$(texmf_dvips)
	$(INSTALL_DATA) $(srcdir)/texinfo.tex $(DESTDIR)$(texmf_texinfo)/texinfo.tex
	$(INSTALL_DATA) $(srcdir)/epsf.tex $(DESTDIR)$(texmf_dvips)/epsf.tex
	for f in $(TXI_XLATE); do \
	  $(INSTALL_DATA) $(srcdir)/$$f $(DESTDIR)$(texmf_texinfo)/$$f; done

# The man pages depend on the --help strings and the version number.
common_mandeps = $(top_srcdir)/configure.ac

# Set up the variables:
man_rule_0 = \
	program=`expr '/$@' : '.*/\(.*\)\.1'` && \
	case $$program in makeinfo) dir=makeinfo;; info*) dir=info;; \
	        *install-info) dir=install-info;; *) dir=util;; esac && \
	dir="$(top_builddir)/$(native_tools)/$$dir" && \
	case $$program in info|install-info) program=g$$program;; \
		esac

# Update the manpage for a binary program; set up the variables,
# make sure the binary is up-to-date and then generate the man page.
man_rule_bin = echo "Updating man page $@" && \
	$(man_rule_0) && \
	echo cd "$$dir" '&&' $(MAKE) $(AM_MAKEFLAGS) $$program$(EXEEXT) && \
	    (cd "$$dir"  &&  $(MAKE) $(AM_MAKEFLAGS) $$program$(EXEEXT)) && \
	echo $(HELP2MAN) --name=\""$$name"\" "$$dir/$$program" -o '$@' && \
	     $(HELP2MAN) --name="$$name"     "$$dir/$$program" -o '$@'

$(srcdir)/info.1: $(top_srcdir)/info/info.c $(common_mandeps)
	@name="read Info documents" && $(man_rule_bin)
$(srcdir)/infokey.1: $(top_srcdir)/info/infokey.c $(common_mandeps)
	@name="compile customizations for Info" && $(man_rule_bin)
$(srcdir)/install-info.1: $(top_srcdir)/install-info/install-info.c $(common_mandeps)
	@name="update info/dir entries" && $(man_rule_bin)

# Here we need some postprocessing:
$(srcdir)/makeinfo.1: $(top_srcdir)/makeinfo/makeinfo.c $(common_mandeps)
	@name="translate Texinfo documents" && $(man_rule_bin)
	mv $@ $@.tmp
	sed '/^\.IP$$/N;/\nAlso/s/I//;/\nThe/s/I//' $@.tmp >$@
	rm $@.tmp

$(srcdir)/texindex.1: $(util_srcdir)/texindex.c $(common_mandeps)
	@name="sort Texinfo index files" && $(man_rule_bin)


## ---------- ##
## texi2dvi.  ##
## ---------- ##

man_MANS += texi2dvi.1 texi2pdf.1 pdftexi2dvi.1
util_srcdir = $(top_srcdir)/util
# Do not depend on the current user's settings.
unset_envvars = unset TEXI2DVI_BUILD_MODE; unset TEXI2DVI_BUILD_DIRECTORY

common_texi2dvi_deps = $(common_mandeps) \
                       $(util_srcdir)/texi2dvi $(util_srcdir)/texi2pdf

$(srcdir)/texi2dvi.1: $(common_texi2dvi_deps)
	$(unset_envvars); $(HELP2MAN) --name="convert Texinfo documents to DVI" $(util_srcdir)/texi2dvi >$@

$(srcdir)/texi2pdf.1: $(common_texi2dvi_deps)
	$(unset_envvars); $(HELP2MAN) --name="convert Texinfo documents to PDF" $(util_srcdir)/texi2pdf >$@

$(srcdir)/pdftexi2dvi.1:  $(common_texi2dvi_deps)
	$(unset_envvars); $(HELP2MAN) --name="convert Texinfo documents to PDF" $(util_srcdir)/pdftexi2dvi >$@

# Do not create info files for distribution.
dist-info:
