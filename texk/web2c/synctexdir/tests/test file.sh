#! /bin/sh
# this file is part of the synctex package
# it is a unit test to enlight any malfunction of the _synctex_get_name function
# This script can be sourced with "synctex" command available
# The test is a SUCCESS if no "FAILURE" appears in the output

echo "This is test file.sh"
echo "You can source this file or execute it"
echo "[SYNCTEX_PATH=/the/path/to/synctex ][SYNCTEX_TEST_DIR=/the/path/to/the/test/folder ](source |./)\"test file.sh\""

if test -z "$SYNCTEX_PATH"
then
SYNCTEX_PATH="$(which synctex)"
elif ! test -f "$SYNCTEX_PATH" || ! test -x "$SYNCTEX_PATH"
then
    echo "No executable file at $SYNCTEX_PATH"
exit -1
fi
SYNCTEX_PATH="$(cd "$(dirname "$SYNCTEX_PATH")"; pwd)/$(basename "$SYNCTEX_PATH")"
echo "synctex command used: $SYNCTEX_PATH"

if ! test -x "$SYNCTEX_PATH"
then
    echo "No executable file at $SYNCTEX_PATH"
    exit -1
fi

mkdir -p synctex_tests/foo/bar
mkdir -p synctex_tests/bar

if test -z "$SYNCTEX_TEST_DIR"
then
SYNCTEX_TEST_DIR="../source/texk/web2c/synctexdir/unit\ tests"
fi
if ! test -d "$SYNCTEX_TEST_DIR"
then
    echo "No directory file at $SYNCTEX_TEST_DIR"
    exit -1
fi
echo "synctex test directory: $SYNCTEX_TEST_DIR"

cp "$SYNCTEX_TEST_DIR/footest.synctex" synctex_tests
cp "$SYNCTEX_TEST_DIR/bartest.synctex.gz" synctex_tests
rm synctex_tests/bartest.synctex
cp "$SYNCTEX_TEST_DIR/story-zapfino.tex" synctex_tests
cd synctex_tests

$SYNCTEX_PATH test file -o footest.pdf
#
echo "--------------------------------  A-1"
"$SYNCTEX_PATH" view -i 1:0:test.tex -o footest
"$SYNCTEX_PATH" edit -o 1:0:0:bartest.pdf
"$SYNCTEX_PATH" update -o footest.pdf -m 2000 -x 212 -y 734
cat footest.synctex
"$SYNCTEX_PATH" update -o bartest.pdf -m 2000 -x 212 -y 734
gunzip bartest.synctex.gz
echo "test diff"
diff footest.synctex bartest.synctex
echo "test done"
#pdflatex -synctex=1 story-zapfino.tex
exit 0
function my_touch {
	sleep 1
	touch $1
}
function my_test {
	# syntax test output build touched expected mode YES
	echo "
================================"
	echo "output:$1"
	echo "build:$2"
	if test -z "$2"
	then
		if test -z "$(synctex test file -o "$1"|grep "file name:$3\$")" || test -z "$(synctex test file -o "$1"|grep "mode:$4")"
		then
			if test "YES" = "$5"
			then
				echo "!!!!!!!!!!!!!!!"
				echo "!  FAILURE 1  !"
				echo "!!!!!!!!!!!!!!!"
				synctex test file -o "$1"
			else
				echo "SUCCESS"
				ls -R
			fi
		else
			if test "YES" = "$5"
			then
				echo "SUCCESS"
			else
				echo "!!!!!!!!!!!!!!!"
				echo "!  FAILURE 2  !"
				echo "!!!!!!!!!!!!!!!"
				synctex test file -o "$1"
			fi
		fi
	else
		if test -z "$(synctex test file -o "$1" -d "$2"|grep "file name:$3\$")" || test -z "$(synctex test file -o "$1" -d "$2"|grep "mode:$4")"
		then
			if test "YES" = "$5"
			then
				echo "!!!!!!!!!!!!!!!"
				echo "!  FAILURE 3  !"
				echo "!!!!!!!!!!!!!!!"
				synctex test file -o "$1" -d "$2"
			else
				echo "SUCCESS"
				ls -R
			fi
		else
			if test "YES" = "$5"
			then
				echo "SUCCESS"
			else
				echo "!!!!!!!!!!!!!!!"
				echo "!  FAILURE 4  !"
				echo "!!!!!!!!!!!!!!!"
				synctex test file -o "$1" -d "$2"
			fi
		fi
	fi
}
echo "--------------------------------  B-1"
my_touch foo.synctex.gz
my_touch foo.synctex
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test foo.pdf "" foo.synctex none YES
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test foo.pdf "" foo.synctex.gz gz YES
echo "--------------------------------  B-2"
my_touch bar/foo.synctex.gz
my_touch bar/foo.synctex
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test bar/foo.pdf "" bar/foo.synctex none YES
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test bar/foo.pdf "" bar/foo.synctex.gz gz YES
echo "--------------------------------  B-3"
my_touch foo/bar/foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch bar/foo.synctex.gz
my_touch bar/foo.synctex
my_test foo/bar/foo.pdf "" foo/bar/foo.synctex none YES
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch bar/foo.synctex.gz
my_touch bar/foo.synctex
my_test foo/bar/foo.pdf "" foo/bar/foo.synctex.gz gz YES
echo "--------------------------------  B-4"
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo.synctex.gz
my_touch foo.synctex
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test foo.pdf bar foo.synctex none YES
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test foo.pdf bar foo.synctex.gz gz YES
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch bar/foo.synctex.gz
my_touch bar/foo.synctex
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test foo.pdf bar bar/foo.synctex none YES
my_touch foo.synctex
my_touch foo.synctex.gz
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_test foo.pdf bar bar/foo.synctex.gz gz YES
echo "--------------------------------  B-5"
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_touch bar/foo.synctex.gz
my_touch bar/foo.synctex
my_touch foo.synctex.gz
my_touch foo.synctex
my_test bar/foo.pdf foo bar/foo.synctex none YES
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo.synctex.gz
my_touch foo.synctex
my_test bar/foo.pdf foo bar/foo.synctex.gz gz YES
my_touch bar/foo.synctex.gz
my_touch bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo.synctex.gz
my_touch foo.synctex
my_test bar/foo.pdf foo foo/bar/foo.synctex none YES
my_touch bar/foo.synctex
my_touch bar/foo.synctex.gz
my_touch foo/bar/foo.synctex
my_touch foo/bar/foo.synctex.gz
my_touch foo.synctex.gz
my_touch foo.synctex
my_test bar/foo.pdf foo foo/bar/foo.synctex.gz gz YES
