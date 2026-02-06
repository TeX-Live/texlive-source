# This Makefile is public domain. Originally written by Karl Berry, 2025.
# install luatex manual from Build to Master in TeX Live.

Master = ../../../../../../Master
dest = $(Master)/texmf-dist/doc/luatex/base/
lua_source_dir = ../../luatexdir

INSTALL_DATA = cp -p

install:
	@test -d $(dest) \
	|| { ls $(Master) echo "dest not directory: $(dest)" >&2; ls $(dest); exit 1; }
	$(INSTALL_DATA) *.tex $(dest)/
	$(INSTALL_DATA) *.pdf $(dest)/
	$(INSTALL_DATA) -r graphics/ $(dest)/
	$(INSTALL_DATA) $(lua_source_dir)/ChangeLog $(dest)/
	$(INSTALL_DATA) $(lua_source_dir)/NEWS $(dest)/
	cd $(dest) && svn status && pwd
