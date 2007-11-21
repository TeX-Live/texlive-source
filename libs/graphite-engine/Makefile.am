AUTOMAKE_OPTIONS = 1.6

SUBDIRS = src test

EXTRA_DIST = doc license \
             build.bat build_d.bat makefile.vc \
             README

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = silgraphite.pc

$(pkgconfig_DATA): config.status

dist-hook:
	find $(distdir) -name .svn | xargs rm -rf

VC_VERSION = 2.1
D_VERSION = `svnversion ${top_srcdir}|sed -e "s/[0-9]*\://g"`
DEB_VERSION = $(VC_VERSION)+0$(D_VERSION)svn
DEBPACKAGE = silgraphite2.0-$(DEB_VERSION)
ORIGTAR = silgraphite2.0_$(DEB_VERSION).orig.tar.gz
WITH_EPOCH = 1:$(DEB_VERSION)-1

DISTRIB_CODENAME ?= ${REL_CODENAME}

showcodename:
	echo "Codename: ${DISTRIB_CODENAME}"

deb-base: dist
	rm -rf silgraphite2.0*
	mkdir -p ${DEBPACKAGE}
	cp ${PACKAGE}-${VERSION}.tar.gz $(DEBPACKAGE)
	tar cfz $(ORIGTAR) $(DEBPACKAGE)
	cp -a ${top_srcdir}/debian $(DEBPACKAGE)
	cd ${DEBPACKAGE} && find . -name .svn | xargs rm -rf

deb: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${WITH_EPOCH} -m -c${DEBPACKAGE}/debian/changelog "This is build direct from svn"
	cd ${DEBPACKAGE} && pdebuild

deb-binary-only: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${WITH_EPOCH} -m -c${DEBPACKAGE}/debian/changelog "This is build direct from svn"
	cd ${DEBPACKAGE} && pdebuild --debbuildopts -i --debbuildopts -b

deb-release: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${WITH_EPOCH} -m -c${DEBPACKAGE}/debian/changelog "This is built direct from svn"
	cd ${DEBPACKAGE} && pdebuild

deb-local: deb-base
	dch -D ${DISTRIB_CODENAME} -v ${WITH_EPOCH} -m -c${DEBPACKAGE}/debian/changelog "This is a debuild direct from svn"
	cd ${DEBPACKAGE} && debuild -us -uc
