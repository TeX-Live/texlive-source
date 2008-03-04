# config.mk -- autoconf rules to remake the Makefile, c-auto.h, etc.
@MAINT@# BTW, xt.ac isn't really required for dvipsk or dviljk, but it doesn't
@MAINT@# seem worth the trouble.
@MAINT@# 
@MAINT@configure_in = $(top_srcdir)/configure.in $(kpathsea_srcdir)/common.ac \
@MAINT@  $(kpathsea_srcdir)/withenable.ac $(kpathsea_srcdir)/xt.ac \
@MAINT@  $(kpathsea_srcdir_parent)/libtool.m4 $(top_srcdir)/../m4/*.m4
@MAINT@
@MAINT@aclocal_m4 = $(top_srcdir)/stamp-aclocal $(top_srcdir)/aclocal.m4
@MAINT@
@MAINT@$(top_srcdir)/stamp-aclocal: $(configure_in)
@MAINT@	cd $(top_srcdir) && aclocal -I ../m4
@MAINT@	date >$(top_srcdir)/stamp-aclocal
@MAINT@# For the case that aclocal.m4 is missing
@MAINT@$(top_srcdir)/aclocal.m4:
@MAINT@	cd $(top_srcdir) && aclocal -I ../m4
@MAINT@	date >$(top_srcdir)/stamp-aclocal
@MAINT@
@MAINT@$(top_srcdir)/stamp-configure: $(configure_in) $(aclocal_m4)
@MAINT@	cd $(top_srcdir) && autoconf
@MAINT@	date >$(top_srcdir)/stamp-configure
@MAINT@# For the case that configure is missing
@MAINT@$(top_srcdir)/configure:
@MAINT@	cd $(top_srcdir) && autoconf
@MAINT@	date >$(top_srcdir)/stamp-configure
@MAINT@
@MAINT@$(top_srcdir)/c-auto.in: $(top_srcdir)/stamp-auto.in
@MAINT@$(top_srcdir)/stamp-auto.in: $(configure_in) $(aclocal_m4)
@MAINT@	cd $(top_srcdir) && autoheader
@MAINT@	date >$(top_srcdir)/stamp-auto.in

config.status: $(top_srcdir)/stamp-configure $(top_srcdir)/configure
	$(SHELL) $@ --recheck

# FIXME: Shouldn't this be $(top_builddir)/config.status
Makefile: config.status $(srcdir)/Makefile.in $(top_srcdir)/../make/*.mk
	$(SHELL) $<

# This rule isn't used for the top-level Makefile, but it doesn't hurt.
# We don't depend on config.status because configure always rewrites
# config.status, even when it doesn't change. Thus it might be newer
# than c-auto.h when we don't need to remake the latter.
c-auto.h: stamp-auto
stamp-auto: $(srcdir)/c-auto.in
	$(SHELL) config.status
	date >stamp-auto

# End of config.mk.
