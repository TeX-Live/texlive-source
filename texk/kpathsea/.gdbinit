define redo
file kpsewhich
end

# afoot.sty
set env TEXMFCNF  /home/texlive/karl/Master/texmf/web2c
set env TEXMFDBS  /home/texlive/karl/Master/texmf-dist
set env TEXINPUTS !!/home/texlive/karl/Master/texmf-dist/tex/latex//
set args --all --subdir=latex/arabtex --subdir=ledmac afoot.sty
#
# preload.cfg
#set env TEXMFDBS  /home/texlive/karl/Master/texmf
#set env TEXINPUTS !!/home/texlive/karl/Master/texmf/tex/generic//:!!/home/texlive/karl/Master/texmf/tex//
#  --debug=32
