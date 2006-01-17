$! Build new version of DVIDVI.
$
$ on warning then exit
$ cc dvidvi
$ define/user lnk$library sys$library:vaxcrtl
$ link/notrace dvidvi

