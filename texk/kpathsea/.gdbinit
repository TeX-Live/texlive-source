define redo
file kpsewhich
end

# afoot.sty
set env TEXMFCNF  /home/texlive/karl/Master/texmf/web2c
set env TEXMFDBS  /home/texlive/karl/Master/texmf-dist
set env TEXINPUTS !!/home/texlive/karl/Master/texmf-dist/tex/latex//
set args --all --subdir=latex/arabtex --subdir=ledmac afoot.sty

# preload.cfg
#set env TEXMFDBS  /home/texlive/karl/Master/texmf
#set env TEXINPUTS !!/home/texlive/karl/Master/texmf/tex/generic//:!!/home/texlive/karl/Master/texmf/tex//
#  --debug=32

# garbage in debug output, vvv 13 May 2008 02:25:06
# mkdir -p ~/test1/test2/test3/tex
set env TEXINPUTS ~/test1/test2/test3/tex
set args -debug=8 texmf.cnf
