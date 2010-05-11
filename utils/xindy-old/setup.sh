#!/bin/sh

target=$PWD

cp $target/bin/xindy $target/bin/xindy.org

sed -e "s:\$lib_dir = \"\(.*\):\$lib_dir = \"$target\/lib/xindy\";:" \
$target/bin/xindy.org > $target/bin/xindy

# remove wrapper script
rm -f $target/setup.sh $target/bin/xindy.org



