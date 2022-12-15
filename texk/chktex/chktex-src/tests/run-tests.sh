#!/bin/dash
# -- remember to change this to sh

set -e

usage() {
    echo "$0 BUILDDIR SRCDIR"
}

if [ "x$1" = x ] || [ "x$2" = x ]; then
    usage
    exit 1
fi

builddir="$(cd $1; pwd)"
srcdir="$(cd $2; pwd)"

testsdir="$srcdir/tests"
output="$testsdir/main.out"
expected="$testsdir/main.expected"

check_results() {
    sed -i -e 's~Message 22 in .*tests/~Message 22 in ~;' "$output"
	if cmp -s "$expected" "$output"; then
		echo ">>> OK!"
		rm -f "$output"
        return 0
	else
		echo "***WARNING***";
		echo "Unexpected test output";
		diff -u "$expected" "$output";
        return 1
	fi
}


# We don't check in the test files because CTAN can't handle files
# with the same names, and we need several that are named chktexrc.
# Also, they don't like invisible files (start with '.') so we
# generate those as well.
echo ">>> Generating test files..."
mkdir -p $testsdir/sub
cat > $testsdir/sub/chktexrc <<EOF
OutFormat
{
"loaded chktex/tests/sub %f!n"
}
EOF

mkdir -p $testsdir/sub1/.config
cat > $testsdir/sub1/.config/chktexrc <<EOF
OutFormat
{
"loaded chktex/tests/sub1/.config/chktexrc %f!n"
}
EOF

cat > $testsdir/sub2/.chktexrc <<EOF
OutFormat
{
"loaded chktex/tests/sub2/.chktexrc %f!n"
}
EOF

# Run the actual tests for inclusion
echo ">>> Testing that inclusion works correctly..."
# absolute path
rm -f "$output"
${builddir}/chktex -mall -r -g0 -l$srcdir/chktexrc -v5 -q \
           $testsdir/main.tex 2>/dev/null \
          1> $testsdir/main.out
check_results

# relative path
cd "$srcdir"
${builddir}/chktex -mall -r -g0 -l$srcdir/chktexrc -v5 -q \
           tests/main.tex 2>/dev/null \
	 	  1> tests/main.out
check_results

# file in the same directory
cd "$testsdir"
${builddir}/chktex -mall -r -g0 -l$srcdir/chktexrc -v5 -q \
           main.tex 2>/dev/null \
	 	  1> main.out
check_results

echo ">>> Testing that correct chktexrc files are loaded"
# XDG variables
unset HOME
echo '%' \
    | XDG_CONFIG_HOME=${testsdir}/sub \
        ${builddir}/chktex -mall -v0 -q \
    | grep 'loaded chktex/tests/sub stdin' \
    || (echo XDG_CONFIG_HOME/chktexrc inclusion failed; exit 1)
echo '%' \
    | HOME=${testsdir}/sub1  \
        ${builddir}/chktex -mall -v0 -q \
    | grep 'loaded chktex/tests/sub1/.config/chktexrc stdin' \
    || (echo HOME/.config/chktexrc inclusion failed; exit 1)

# HOME, LOGDIR
echo '%' \
    | HOME=${testsdir}/sub2  \
        ${builddir}/chktex -mall -v0 -q \
    | grep 'loaded chktex/tests/sub2/.chktexrc stdin' \
    || (echo HOME/.chktexrc inclusion failed; exit 1)
echo '%' \
    | LOGDIR=${testsdir}/sub2  \
        ${builddir}/chktex -mall -v0 -q \
    | grep 'loaded chktex/tests/sub2/.chktexrc stdin' \
    || (echo LOGDIR/.chktexrc inclusion failed; exit 1)

# CHKTEXRC
echo '%' \
    | CHKTEXRC=${testsdir}/sub2  \
        ${builddir}/chktex -mall -v0 -q \
    | grep 'loaded chktex/tests/sub2/.chktexrc stdin' \
    || (echo CHKTEXRC/.chktexrc inclusion failed; exit 1)

# CWD
echo '%' \
    | (cd ${testsdir}/sub2;  \
        ${builddir}/chktex -mall -v0 -q)  \
    | grep 'loaded chktex/tests/sub2/.chktexrc stdin' \
    || (echo PWD/.chktexrc inclusion failed; exit 1)

# Not sure how to test KPSE variables...
# TEXMFMAIN CHKTEX_CONFIG
echo ">>> OK!"

# Command line options
echo "Checking command line RC settings..."
(${builddir}/chktex -d 4 -STabSize=7 </dev/null 2>&1 \
     | grep -A1 TabSize | grep -E '\t7$' >/dev/null) \
    || (echo Setting TabSize from command line failed; exit 1)
echo ">>> OK!"
