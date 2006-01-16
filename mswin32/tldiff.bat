@echo off
cd \source\fptex
\source\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude -x "*/texk.development/*" . c:\source\TeXLive\Build\source\TeX > diffs\tl-source-%1.diff
move texk texk.stable
move texk.development texk
\source\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude -x "*/texk.stable/*" . c:\source\TeXLive\Build\source.development\TeX > diffs\tl-dev-%1.diff
move texk texk.development
move texk.stable texk