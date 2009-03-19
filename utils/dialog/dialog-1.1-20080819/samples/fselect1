#!/bin/sh
# $Id: fselect1,v 1.4 2003/10/09 22:45:06 tom Exp $
: ${DIALOG=dialog}

FILE=$HOME
for n in .cshrc .profile .bashrc
do
	if test -f $HOME/$n ; then
		FILE=$HOME/$n
		break
	fi
done

exec 3>&1
FILE=`$DIALOG --title "Please choose a file" --fselect $FILE 14 48 2>&1 1>&3`
code=$?
exec 3>&-

case $code in
	0)
		echo "\"$FILE\" chosen";;
	1)
		echo "Cancel pressed.";;
	255)
		echo "Box closed.";;
esac
