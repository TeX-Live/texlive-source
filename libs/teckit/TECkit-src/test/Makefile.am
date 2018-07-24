check_SCRIPTS = dotests.pl

check_DATA  = GNT-map.xml
check_DATA += ISO-8859-1.map
check_DATA += Mrk-GNT.sf
check_DATA += mrk.sf.legacy.txt.orig
check_DATA += mrk.txt
check_DATA += NormalizationTest.txt
check_DATA += SILGreek2004-04-27.map
check_DATA += SILGreek2004-04-27.uncompressed.tec.orig
check_DATA += academy.tec
check_DATA += silipa93.tec

EXTRA_DIST = $(check_SCRIPTS) $(check_DATA)

TESTS = $(check_SCRIPTS)
TESTS_ENVIRONMENT = BINDIR=$(top_builddir)/bin SRCDIR=$(srcdir)
