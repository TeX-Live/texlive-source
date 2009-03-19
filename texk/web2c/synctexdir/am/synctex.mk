
### SyncTeX support in xetex
# Actually, xetex cannot build without SyncTeX because the tex.web
# code does not provide proper entry points
# list of change files
xetex_ch_srcs-with_synctex-mem_only = \
	$(synctex_dir)/synctex-mem.ch0 \
	$(synctex_dir)/synctex-e-mem.ch0 \
	$(synctex_dir)/synctex-e-mem.ch1 \
	$(synctex_dir)/synctex-xe-mem.ch2
xetex_ch_srcs-with_synctex = \
	$(xetex_ch_srcs-with_synctex-mem_only) \
	$(synctex_dir)/synctex-rec.ch0 \
	$(synctex_dir)/synctex-e-rec.ch0 \
	$(synctex_dir)/synctex-xe-rec.ch2
xetex_ch_srcs-without_synctex =
xetex_post_ch_srcs-with_synctex = \
	$(synctex_dir)/synctex-xe-rec.ch3
xetex_post_ch_srcs-without_synctex =
# The C sources and headers
xetex_o-with_synctex = synctex-xe.o
xetex_o-without_synctex = 
synctex-xe.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-xetex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex-xe.c: xetexd.h $(synctex_dir)/synctex.c synctex-xetex.h
	sed s/TEX-OR-MF-OR-MP/xetex/ $(synctex_dir)/synctex.c >$@
xetexd.h-with_synctex = if test -z "`grep __SyncTeX__ xetexd.h`";\
	then\
		$(synctex_common_texd);\
		sed -f synctex_sed_command.txt xetexd.h > synctex_xetexd.h;\
		mv synctex_xetexd.h xetexd.h;\
		if test -z "`grep __SyncTeX__ xetexd.h`";\
		then\
			echo "warning: SyncTeX activation FAILED";\
			exit 1;\
		fi;\
		echo "warning: SyncTeX is enabled";\
	fi
xetexd.h-without_synctex = echo "warning: SyncTeX is NOT enabled"

