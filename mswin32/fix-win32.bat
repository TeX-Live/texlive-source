rem @echo off
rem dir /s/b *.tpm | perl \source\fptex\mswin32\global-replace.pl --line-mode --backup-dir=backup-win32 "{ $a=chr(10); $_=~s@^man/man(.)/([^\.]*)\.(.)@texmf/doc/man/man\1/\2.\3${a}texmf/doc/man/man\1/\2.html@; }"
dir /s/b *.tpm | perl \source\fptex\mswin32\global-replace.pl --line-mode --backup-dir=backup-win32 "{ $a=chr(10); $_=~s@^${a}@@; }"
rem popd
