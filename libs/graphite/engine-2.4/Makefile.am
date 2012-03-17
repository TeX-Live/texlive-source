AUTOMAKE_OPTIONS = 1.6

SUBDIRS = src test

EXTRA_DIST = doc license \
             build.bat build_d.bat makefile.vc7 makefile.vc8 \
             README

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = silgraphite.pc

$(pkgconfig_DATA): config.status

dist-hook:
	find $(distdir) -name .svn | xargs rm -rf

# VC_VERSION is the last released version of the software
VC_VERSION = 2.4
# D_VERSION gets the number of the current svn revision
D_VERSION = `svnversion ${top_srcdir}|sed -e "s/[0-9]*\://g"`
SVNDEB_VERSION = $(VC_VERSION)+0$(D_VERSION)svn
SVNDEBPACKAGE = silgraphite2.0-$(SVNDEB_VERSION)
SVNORIGTAR = silgraphite2.0_$(SVNDEB_VERSION).orig.tar.gz
SVNWITH_EPOCH = 1:$(SVNDEB_VERSION)-1

DEB_VERSION = $(VC_VERSION)
DEBPACKAGE = silgraphite2.0-$(DEB_VERSION)
ORIGTAR = silgraphite2.0_$(DEB_VERSION).orig.tar.gz
WITH_EPOCH = 1:$(DEB_VERSION)-1

DISTRIB_CODENAME ?= ${REL_CODENAME}

showcodename:
	echo "Codename: ${DISTRIB_CODENAME}"

deb-base: dist
	rm -rf silgraphite2.0*
	mkdir -p ${SVNDEBPACKAGE}
	cp ${PACKAGE}-${VERSION}.tar.gz $(SVNDEBPACKAGE)
	tar cfz $(SVNORIGTAR) $(SVNDEBPACKAGE)
	cp -a ${top_srcdir}/debian $(SVNDEBPACKAGE)
	cd ${SVNDEBPACKAGE} && find . -name .svn | xargs rm -rf

# make deb builds an interim deb from svn source for release
deb: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${SVNWITH_EPOCH} -m -c${SVNDEBPACKAGE}/debian/changelog
	cd ${SVNDEBPACKAGE} && pdebuild

# make deb-binary-only builds a binary only deb from svn source 
deb-binary-only: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${SVNWITH_EPOCH} -m -c${SVNDEBPACKAGE}/debian/changelog "This is build direct from svn"
	cd ${SVNDEBPACKAGE} && pdebuild --debbuildopts -i --debbuildopts -b

# make deb-release builds a deb for a released version number
deb-release: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${WITH_EPOCH} -c${DEBPACKAGE}/debian/changelog
	cd ${DEBPACKAGE} && pdebuild

# make deb-local uses debuild to build a deb for local use only
deb-local: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${SVNWITH_EPOCH} -m -c${SVNDEBPACKAGE}/debian/changelog "This is a debuild direct from svn"
	cd ${SVNDEBPACKAGE} && debuild -us -uc
