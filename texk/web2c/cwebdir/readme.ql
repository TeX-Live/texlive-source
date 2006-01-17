This is the readme file for CWEB for QDOS/SMSQ systems

(These instructions are for systems with Toolkit II,
 please report any problems to me, the address is at the
 end of the file)

If you want to compile CWEB for your QL system, you must first
convert the ASCII code to the QL's character set.  Therefore you
can use the little BASIC program appended at the end of the file.

For a first version you have to alter the supplied ctangle_c file.
Set your DATA_USE to the directory you have the CWEB sources in, and
the PROG_USE to C68's directory.
compile it with:
ex cc;'common_c ctangle_c -bufp500K -octangle'

then ctangle the source files with:
ex datad$&'ctangle';'common_w comm-ql_ch'
ex datad$&'ctangle';'ctangle_w ctang-ql_ch'
ex datad$&'ctangle';'cweave_w cweav-ql_ch'

Compile the following way (C68v4.14 +):

ex cc;'-c common_c'
ex cc;'-c ctangle_c'
ex cc;'-c cweave_c'
ex ld;'-bufp500K common_o ctangle_o -octangle'
ex ld;'-bufp500K common_o cweave_o -ocweave'

After that copy the ctangle and cweave executables into your
programs directory (if you want them there).

The DATA default may be used as usual.  E.g. if you have
a file win2_data_test_w and the DATA default set to
win2_data_ you may call ctangle with
     ex ctangle;'test'
assuming that ctangle is in your PROG default.

Please report any problems with the QL version to:
Robert Klein
Bluecherstrasse 24
D-56349 Kaub
Germany


**************** ASCII Conversion program ***************
With this program you can convert DOS and Unix 7-Bit ASCII
files to the QL's character set.  You'll need Toolkit II.
The program will take all the files in the source directory
and put them converted into the destination&source directory,
e.g. you have as drive1$='win2_', drive2$='win3_',
path1$='cweb_' and path2$='ql_'.  Then the routine will take
all files in win2_cweb_ (wthout subdirectories!) convert them
and write them to 'win3_ql_cweb_'.  It's only a lousy hack,
but it'll do it's job.

1000 DEFine PROCedure dos2ql (drive1$,path1$,drive2$,path2$)
1002 LOCal zx$,zx%,ishl,ashl
1005 DELETE drive2$&path2$&'dirlist'
1010 OPEN_NEW#5,drive2$&path2$&'dirlist'
1020 DIR#5,drive1$&path1$
1030 CLOSE#5
1040 OPEN_IN#5,drive2$&path2$&'dirlist'
1050 INPUT#5,zx$:INPUT#5,zx$:REMark devicename and sectors
1060 REPeat ashl
1070   IF EOF(#5)THEN EXIT ashl
1080   INPUT#5,zx$
1085   IF '->' INSTR zx$ THEN NEXT ashl
1090   OPEN_IN#3,drive1$&zx$
1100   OPEN_NEW#4,drive2$&path2$&zx$
1110   REPeat ishl
1120     IF EOF(#3)THEN EXIT ishl
1130     BGET#3,zx%
1140     SELect ON zx%
1150       = 13:REMark do nothing (DOS LineFeed)
1155       = 26:REMark do nothing (DOS EOF sign)
1160       = 96: BPUT#4,159:REMark Unix/DOS beginning quote sign
1170       = REMAINDER : BPUT#4,zx%
1180     END SELect
1190   END REPeat ishl
1195   CLOSE#3:CLOSE#4
1200 END REPeat ashl
1205 CLOSE#5
1210 END DEFine dos2ql
