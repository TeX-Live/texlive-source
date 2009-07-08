#! /bin/sh
# this file is part of the synctex package
# it is a unit test to enlight any malfunction of the _synctex_get_name function
# This script can be sourced with "synctex" command available
# The test is a SUCCESS if no "FAILURE" appears in the output

mkdir -p synctex_tests/foo/bar
mkdir -p synctex_tests/bar
cp ../source/texk/web2c/synctexdir/unit\ tests/footest.synctex synctex_tests
cp ../source/texk/web2c/synctexdir/unit\ tests/bartest.synctex.gz synctex_tests
rm synctex_tests/bartest.synctex
cp ../source/texk/web2c/synctexdir/unit\ tests/story-zapfino.tex synctex_tests
cd synctex_tests
which synctex
synctex test file -o footest.pdf
#
echo "--------------------------------  A-1"
synctex view -i 1:0:test.tex -o footest
synctex edit -o 1:0:0:bartest.pdf
synctex update -o footest.pdf -m 2000 -x 212 -y 734
cat test.synctex
synctex update -o bartest.pdf -m 2000 -x 212 -y 734
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
