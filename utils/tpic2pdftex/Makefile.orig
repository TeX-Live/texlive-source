#$Id: Makefile,v 1.21 2007/01/14 13:35:17 hahe Exp hahe $

.PRECIOUS:	%.tpic %.tex

all:	example.pdf beamerexample.pdf

example.pdf:	example.tex
	pdftex $<

beamerexample.pdf:	beamerexample.tex
	pdflatex $<

%.tpic:	%.pic Makefile
	pic -z -t $< > $@

%.tex:	%.tpic
	tpic2pdftex $< > $@

clean:
	- rm *.tex
	- rm *.log
	- rm *.toc
	- rm *.out
	- rm *.snm
	- rm *.nav
	- rm *.aux
	- rm *.tpic

allclean:
	make clean
	- rm *.pdf
