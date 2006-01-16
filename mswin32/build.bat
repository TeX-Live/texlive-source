taskkill /im:ispell.exe /f
rem p4 sync -f //depot/Master/texmf/...
rem rmdir /q/s "c:\program files\texlive\texmf"
rem mkdir "c:\program files\texlive\texmf"
rem xcopy c:\source\texlive\Master\texmf "c:\program files\texlive\texmf" /r/i/e/k
rem perl ./sync.pl c:/source/texlive/Master/texmf "c:/Program Files/texlive/texmf"
rem perl ./sync.pl c:/source/texlive/Master/texmf-dist "c:/Program Files/texlive/texmf-dist"
rem perl ./sync.pl c:/source/texlive/Master/texmf-doc "c:/Program Files/texlive/texmf-doc"
perl ./build.pl --distclean=tex --development
perl ./build.pl --distclean=xemtex --development
perl ./sync.pl --sources
perl ./sync.pl --restrict --texmf
rem pushd .. & ( \source\fptex\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude -x "*/texk.development/*" c:/source/TeXLive/Build/source/TeX ./ > diffs\tl-source-29.diff ) & popd
rem pushd .. & ( \source\fptex\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude c:/source/TeXLive/Build/source.development/TeX/texk ./texk.development > diffs\tl-dev-29.diff ) & popd
rem pushd .. & ( \source\fptex\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude ./texk ./texk.development > diffs\dev-29.diff ) & popd
perl ./build.pl --build=tex --log=build.log --development
perl ./build.pl --build=xemtex --log=xemtex.log --development
perl ./build.pl --install=tex --log=install.log --from_scratch --development
mktexlsr
updmap
fmtutil --all --dolinks --force
perl ./build.pl --install=tex --log=install.log --development
perl ./build.pl --install=xemtex --log=xemtex-install.log --development
perl ./build.pl --standalone --development --log=standalone.developement.log
perl ./sync.pl c:/progra~1/texlive/bin/win32 c:/source/texlive/Master/bin/win32
perl ./sync.pl c:/progra~1/texlive/bin/win32-static c:/source/texlive/Master/bin/win32-static
perl ./sync.pl --restrict c:/progra~1/texlive/xemtex c:/source/texlive/Master/xemtex
perl ./sync.pl --restrict c:/progra~1/texlive/texmf c:/source/texlive/Master/texmf
perl ./p4update.pl --update --subdir=Master/bin/win32
perl ./p4update.pl --update --subdir=Master/bin/win32-static
perl ./p4update.pl --update --subdir=Master/texmf
perl ./p4update.pl --update --subdir=Master/xemtex
pushd \source\texlive\Master\Tools
rem perl ./tpm-factory.pl --clean --patterns=from --arch=all --type=TLCore
rem perl ./tpm-factory.pl --clean --patterns=auto --arch=all --type=Package
rem perl ./tpm-factory.pl --clean --patterns=auto --arch=all --type=Documentation
rem perl ./tpm-factory.pl --check=dep --arch=all > dependencies.log
rem perl ./tpm-factory.pl --check=cov --arch=all --type=TLCore > tlcore-coverage.log
rem perl ./tpm-factory.pl --check=cov --arch=all --type=Package > package-coverage.log
rem perl ./tpm-factory.pl --check=cov --arch=all --type=Documentation > documentation-coverage.log
rem perl ./tpm-factory.pl --tpm2zip --arch=win32 --all
rem perl ./tpm-factory.pl --tpm2zip=full --arch=win32-static --standalone
popd

