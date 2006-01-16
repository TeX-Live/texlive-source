@echo off
pushd \source\texlive\Build\source.development\TeX\texk\web2c
rem dir /s/b *.c *.cc *.h *.ch *.ch0 *.ch1 *.ch2 *.mak *.in *.am *.defines | perl \source\fptex\mswin32\global-replace.pl --backup-dir=backup-eol "{ $a=chr(13); $_=~s/$a//g; }"
rem cd \source\texlive\Build\source.development\TeX
dir /s/b *.c *.cc *.h *.ch  *.ch0 *.ch1 *.ch2 *.mak *.in *.am *.defines | perl \source\fptex\mswin32\global-replace.pl --backup-dir=backup-eol "{ $a=chr(13); $_=~s/$a//g; }"
rem cd \source\texlive\Master\texmf\tpm
rem dir /s/b *.tpm | perl \source\fptex\mswin32\global-replace.pl --backup-dir=backup-eol "{ $a=chr(13); $_=~s/$a//g; }"
popd
