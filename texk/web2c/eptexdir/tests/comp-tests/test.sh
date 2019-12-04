#!/bin/bash

LOGFILE=`pwd`/${USER}_`hostname`.log
echo '#' `LC_ALL=C date` > $LOGFILE
echo '#' >> $LOGFILE

if type "screenfetch" > /dev/null 2>&1; then
  screenfetch -n -N -d '-gtk;-pkgs;-wmtheme;-disk;-uptime;-wm;-res' | sed 's/^/#/' >> $LOGFILE
fi
echo '#' >> $LOGFILE
echo '# test source: ' >>$LOGFILE
echo '# \documentclass{minimal}\begin{document}\end{document}' >> $LOGFILE
echo '#' >> $LOGFILE
echo '# comp_level  binary_size  fmt_size  fmt_time[s]  typeset_time[ms]' >> $LOGFILE

function test0(){
ENGINE=$1
start_time=$(date +"%s.%3N")
for i in `seq 1 10`; do
  $ENGINE -progname=platex-dev -ini -etex -jobname=$ENGINE-x -comp-level=$2 platex.ini &>/dev/null || exit 1
  echo -n "*"
done
end_time=$(date +"%s.%3N")
elapsed=`echo "scale=3; $end_time - $start_time" | bc`
#
fmt_elapsed=`echo "scale=3; $elapsed / 10 " | bc`
BINARY=`which $ENGINE`
#
elapsed="0"
for i in `seq 1 20`; do
  start_time=$(date +"%s.%3N")
  for x in `seq 1 $3`; do $ENGINE -fmt=$ENGINE-x test.tex&>/dev/null ; done
  end_time=$(date +"%s.%3N")
  elapsed=`echo "scale=2; $elapsed + $end_time - $start_time" | bc`
  echo -n "."
done
echo
elapsed=`echo "scale=2; $elapsed * 50 / $3 " | bc`
echo $2 `wc -c < $BINARY` `wc -c < $ENGINE-x.fmt` $fmt_elapsed $elapsed | tee -a $LOGFILE
}

pushd /tmp
echo
echo '\documentclass{minimal}\begin{document}\end{document}' > test.tex

test0 eptex      0 10
test0 eptex-beta 1 10
test0 eptex-beta 3 10
test0 eptex-beta 4 10
test0 eptex-beta 5 10
test0 eptex-beta 6 10
test0 eptex-beta 7 10
test0 eptex-beta 8 10
test0 eptex-beta 9 10
test0 eptex-beta 10 10
test0 eptex-beta 11 10
test0 eptex-beta 12 10
