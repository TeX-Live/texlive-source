#! /bin/sh

tex=$1

if test -z "`grep __SyncTeX__ ${tex}d.h`"; then
	echo "Enabling SyncTeX Support for $tex."
	sed -e 's|^#include "texmfmp.h"$|&\
/* Start of SyncTeX Section */\
#define __SyncTeX__\
#include "synctexdir/synctex-common.h"\
/* End of SyncTeX Section */|g' ${tex}d.h >synctex_${tex}d.h
	if test -z "`grep __SyncTeX__ synctex_${tex}d.h`"; then
		echo "warning: SyncTeX activation for $tex FAILED"
		exit 1
	fi
	mv synctex_${tex}d.h ${tex}d.h
	echo "warning: SyncTeX for $tex is enabled"
fi
