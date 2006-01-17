# Files used to create an Aleph without e-TeX extensions
noaleph_files = \
	om16bit.ch \
	omstr.ch \
	omfont.ch \
	omchar.ch \
	omfi.ch \
	ompar.ch \
	omocp.ch \
	omfilter.ch \
	omtrans.ch \
	omdir.ch \

# Files to add e-TeX extensions to Aleph
aleph_build=rc1

aleph_files = \
	eobase.ch \
	eofmt.ch \
	eomem-$(aleph_build).ch \
	eo16bit-$(aleph_build).ch \
	eoext.ch \
	eoeqtb.ch \
	eofix.ch \
	eocprt.ch \
	eonewdir-$(aleph_build).ch \
	eover-$(aleph_build).ch \
	eopage-$(aleph_build).ch \
	eochar-$(aleph_build).ch

new_tex = tex-new.web
old_tex = tex-old.web

localdir = c:/texmf/miktex/bin/
tie = $(localdir)tie.exe
tangle = $(localdir)tangle.exe
weave = $(localdir)weave.exe

sourcedir = c:/sorci/miktex/miktex/

rm = rm
cp = cp

all: aleph.p aleph.pdf

etex.web: $(old_tex) etex.ch
	$(tie) -m etex.web $(old_tex) etex.ch

noaleph.web: $(new_tex) $(noaleph_files)
	$(tie) -m noaleph.web $(new_tex) $(noaleph_files)

aleph.web: noaleph.web $(aleph_files)
	$(tie) -m aleph.web noaleph.web $(aleph_files)
	$(cp) aleph.web aleph-$(aleph_build).web

aleph.p aleph.pool: aleph.web
	$(tangle) aleph.web nul.ch aleph.p aleph.pool
	$(cp) aleph.pool aleph-$(aleph_build).pool

aleph.tex: aleph.web
	$(weave) aleph.web nul aleph.tex

aleph.pdf: aleph.tex
	pdftex aleph
	$(cp) aleph.pdf aleph-$(aleph_build).pdf

clean:
	$(rm) -f noaleph.web
	$(rm) -f aleph.web
	$(rm) -f aleph.tex
	$(rm) -f aleph.pdf
	$(rm) -f aleph.log
	$(rm) -f aleph.p
	$(rm) -f aleph.pool
