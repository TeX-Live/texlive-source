@echo off
set objdir=%1
set srcdir=.
set win32seddir=%srcdir%\..\msvc
set perldir=%srcdir%\..\msvc
rem set PATH=%srcdir%\..\kpathsea\%objdir%;%srcdir%\..\..\gnu\bin;%PATH%
if NOT %2==bibtex goto mfmp
	cat %srcdir%\web2c\common.defines %2.p | %srcdir%\web2c\%objdir%\web2c -hcpascal.h -c%2 | sed -f %srcdir%/web2c/cvtbib.sed | %srcdir%\web2c\%objdir%\fixwrites %2 > %2.c
	cat %srcdir%\bibtex.h | sed -e "1,$s/(buftype)//g" | sed -e "1,$s/(pdstype)//g" > %srcdir%\bibtex.h.new
	copy %srcdir%\bibtex.h.new %srcdir%\bibtex.h && del /f %srcdir%\bibtex.h.new
	goto exit
:mfmp
if NOT %2==mf goto mp
	cat %srcdir%\web2c\common.defines %srcdir%\web2c\texmf.defines %srcdir%\web2c\mfmp.defines %2.p | sed -f %srcdir%/web2c/cvtmf1.sed | %srcdir%\web2c\%objdir%\web2c -htexmfmp.h -m -c%2coerce | sed -f %srcdir%/web2c/cvtmf2.sed | %srcdir%\web2c\%objdir%\fixwrites %2 | perl %perldir%\splitup.pl --name=%2 --dfile > %2.c
rem so timestamps are definitely later, to avoid make weirdness
	copy %srcdir%\%2coerce.h+%srcdir%\web2c\coerce.h %srcdir%\x%2coerce.h
	sed -f %win32seddir%\mfmp.sed %srcdir%\x%2coerce.h > %srcdir%\%2coerce.h
	del /f %srcdir%\x%2coerce.h
	goto exit
:mp
if NOT %2==mp goto omega
	cat %srcdir%\web2c\common.defines %srcdir%\web2c\texmf.defines %srcdir%\web2c\mfmp.defines %2.p | sed -f %srcdir%/web2c/cvtmf1.sed | %srcdir%\web2c\%objdir%\web2c -htexmfmp.h -m -c%2coerce | sed -f %srcdir%/web2c/cvtmf2.sed | %srcdir%\web2c\%objdir%\fixwrites %2 | perl %perldir%\splitup.pl --name=%2 --dfile > %2.c
	copy %srcdir%\%2coerce.h+%srcdir%\web2c\coerce.h %srcdir%\x%2coerce.h
	sed -f %win32seddir%\mfmp.sed %srcdir%\x%2coerce.h > %srcdir%\%2coerce.h
	del /f %srcdir%\x%2coerce.h
	goto exit
:omega
if NOT %2==omega goto eomega
goto omega_or_eomega_or_aleph
:eomega
if NOT %2==eomega goto aleph
goto omega_or_eomega_or_aleph
:aleph
if NOT %2==aleph goto pdf
:omega_or_eomega_or_aleph
	cat %srcdir%\web2c\common.defines %srcdir%\web2c\texmf.defines %srcdir%\%2dir\%2.defines %2.p | %srcdir%\web2c\%objdir%\web2c -htexmfmp.h -t -c%2coerce | %srcdir%\web2c\%objdir%\fixwrites -t %2 | perl %perldir%\splitup.pl --name=%2 --dfile > %2.c
	copy .\%2coerce.h+%srcdir%\web2c\coerce.h .\x%2coerce.h
	copy .\x%2coerce.h .\%2coerce.h
rem	sed -f %win32seddir%\tex-deopt.sed < %srcdir%\%2.c > %srcdir%\%2.c.opt
rem	copy %srcdir%\%2.c.opt %srcdir%\%2.c && del /f %srcdir%\%2.c.opt
	del /f .\x%2coerce.h
	goto exit
:pdf
if NOT %2==pdftex goto pdfetex
goto pdftex_or_pdfetex
:pdfetex
goto pdftex_or_pdfetex
:pdftex_or_pdfetex
	cat %srcdir%\web2c\common.defines %srcdir%\web2c\texmf.defines %srcdir%\%2dir\%2.defines %2.p | %srcdir%\web2c\%objdir%\web2c -htexmfmp.h -t -c%2coerce | %srcdir%\web2c\%objdir%\fixwrites -t %2 | perl %perldir%\splitup.pl --name=%2 --dfile > %2.c
	copy %srcdir%\%2coerce.h+%srcdir%\web2c\coerce.h %srcdir%\x%2coerce.h
	copy %srcdir%\x%2coerce.h %srcdir%\%2coerce.h
	del /f %srcdir%\x%2coerce.h
rem	sed -f %win32seddir%\tex-deopt.sed < %srcdir%\%2.c > %srcdir%\%2.c.opt
rem	copy %srcdir%\%2.c.opt %srcdir%\%2.c && del /f %srcdir%\%2.c.opt
	goto exit
:tex
if NOT %2==tex goto etex
goto tex_or_etex
:etex
if NOT %2==etex goto vftovp
:tex_or_etex
	cat %srcdir%\web2c\common.defines %srcdir%\web2c\texmf.defines %2.p | %srcdir%\web2c\%objdir%\web2c -htexmfmp.h -t -c%2coerce | %srcdir%\web2c\%objdir%\fixwrites -t %2 | perl %perldir%\splitup.pl --name=%2 --dfile > %2.c
	copy %srcdir%\%2coerce.h+%srcdir%\web2c\coerce.h %srcdir%\x%2coerce.h
	copy %srcdir%\x%2coerce.h %srcdir%\%2coerce.h
	del /f %srcdir%\x%2coerce.h
rem	sed -f %win32seddir%\tex-deopt.sed < %srcdir%\%2.c > %srcdir%\%2.c.opt
rem	copy %srcdir%\%2.c.opt %srcdir%\%2.c && del /f %srcdir%\%2.c.opt
	goto exit
:vftovp
REM if NOT %2==vftovp goto else
REM 	cat %srcdir%\web2c\common.defines %2.p | %srcdir%\web2c\%objdir%\web2c -hcpascal.h -c%2 | sed -f %srcdir%/web2c/cvtvf2vp.sed | %srcdir%\web2c\%objdir%\fixwrites %2 > %2.c
REM 	goto exit:
:else
	cat %srcdir%\web2c\common.defines %2.p | %srcdir%\web2c\%objdir%\web2c -hcpascal.h -c%2 | %srcdir%\web2c\%objdir%\fixwrites %2 > %2.c
:exit
	if EXIST %2d.h touch %2d.h
	echo 'Conversion done !'
