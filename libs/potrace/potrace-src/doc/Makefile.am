man_MANS = potrace.1 mkbitmap.1

EPSFILES = potracelib-fig1a.eps potracelib-fig1b.eps			\
 potracelib-fig2.eps potracelib-fig3.eps potracelib-fig4a.eps		\
 potracelib-fig4b.eps potracelib-fig5.eps potracelib-fig6a.eps		\
 potracelib-fig6b.eps potracelib-fig7-00.eps potracelib-fig7-06.eps	\
 potracelib-fig7-10.eps potracelib-fig7-12.eps potracelib-fig7-13.eps	\
 potracelib-fig8.eps

EXTRA_DIST = potracelib.tex ${EPSFILES} dictionary.txt
dist_doc_DATA = placement.pdf

potracelib.dvi: potracelib.tex ${EPSFILES}
	latex $<
	latex $<

potracelib-1up.ps: potracelib.dvi ${EPSFILES}
	dvips $< -o $@

potracelib.ps: potracelib-1up.ps
	pstops '2:0@0.94L(9.16in,-1.12in)+1@0.94L(9.16in,4.07in)' $< $@

potracelib.pdf: potracelib.ps
	ps2pdf $<

spellcheck: potrace.1.in potracelib.tex
	ispell -d american -p ./dictionary.txt $^

MOSTLYCLEANFILES = potracelib.dvi potracelib-1up.ps potracelib.ps potracelib.pdf
