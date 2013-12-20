SUBDIRS = src/songs src/songidx src/sbdchk sample src/vim

distclean-local:
	-test src/songs/songs.dtx -ef "${srcdir}/src/songs/songs.dtx" || rm -f src/songs/songs.dtx
	-test sample/chordbook.tex -ef "${srcdir}/sample/chordbook.tex" || rm -f sample/chordbook.tex
	-test sample/lyricbook.tex -ef "${srcdir}/sample/lyricbook.tex" || rm -f sample/lyricbook.tex
	-test sample/slidebook.tex -ef "${srcdir}/sample/slidebook.tex" || rm -f sample/slidebook.tex
	-test sample/transparencies.tex -ef "${srcdir}/sample/transparencies.tex" || rm -f sample/transparencies.tex
	-test sample/songs.sbd -ef "${srcdir}/sample/songs.sbd" || rm -f sample/songs.sbd

