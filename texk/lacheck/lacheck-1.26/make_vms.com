$ if f$trnlnm("sys_arch").eqs."ALPHA"
$ then
$   CC :== CC/NoList/opt=level=4/standard=vaxc 
$ else
$   CC :==gcc/Nostandard/NoCase_Hack/NoList
$ endif
$ LINK :== Link/NoMap
$ CC  lacheck.c
$ if f$trnlnm("sys_arch").eqs."ALPHA"
$ then
$   LINK lacheck.obj
$ else
$   LINK lacheck.obj,gnu_cc:[000000]GCCLIB/LIB,	sys$share:vaxcrtl/opt
$ endif
$ Library/Help/Create=Blocks:10 lacheck.hlb lacheck.hlp
$ EXIT
