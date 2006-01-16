# DO NOT DELETE

$(objdir)/ttapi.obj: ttapi.c ttconfig.h ft_conf.h \
	freetype.h fterrid.h ftnameid.h ttengine.h tttypes.h ttmutex.h ttcalc.h \
	ttmemory.h \
	ttcache.h ttfile.h ttdebug.h ttobjs.h tttables.h ttcmap.h ttload.h \
	ttgload.h ttraster.h ttextend.h
$(objdir)/ttcache.obj: ttcache.c ttengine.h tttypes.h ttconfig.h ft_conf.h \
	freetype.h fterrid.h ftnameid.h ttmutex.h ttmemory.h \
	ttcache.h ttobjs.h tttables.h ttcmap.h ttdebug.h
$(objdir)/ttcalc.obj: ttcalc.c ttcalc.h ttconfig.h ft_conf.h \
	freetype.h fterrid.h ftnameid.h ttdebug.h tttypes.h tttables.h
$(objdir)/ttcmap.obj: ttcmap.c ttobjs.h ttconfig.h ft_conf.h \
	ttengine.h tttypes.h freetype.h fterrid.h ftnameid.h ttmutex.h ttcache.h \
	tttables.h ttcmap.h ttdebug.h ttfile.h ttmemory.h \
	ttload.h
$(objdir)/ttdebug.obj: ttdebug.c ttdebug.h ttconfig.h ft_conf.h \
	tttypes.h freetype.h fterrid.h ftnameid.h tttables.h ttobjs.h ttengine.h \
	ttmutex.h ttcache.h ttcmap.h
$(objdir)/ttextend.obj: ttextend.c ttextend.h ttconfig.h ft_conf.h \
	tttypes.h freetype.h fterrid.h ftnameid.h ttobjs.h ttengine.h ttmutex.h \
	ttcache.h tttables.h ttcmap.h ttmemory.h \
	ttconfig.h
$(objdir)/ttfile.obj: ttfile.c ft_conf.h \
	freetype.h fterrid.h ftnameid.h tttypes.h ttdebug.h ttengine.h ttmutex.h \
	ttmemory.h ttfile.h
$(objdir)/ttgload.obj: ttgload.c tttypes.h ttconfig.h ft_conf.h \
	freetype.h fterrid.h ftnameid.h ttdebug.h ttcalc.h ttfile.h ttengine.h \
	ttmutex.h tttables.h ttobjs.h ttcache.h ttcmap.h ttgload.h ttmemory.h \
	tttags.h ttload.h
$(objdir)/ttinterp.obj: ttinterp.c freetype.h fterrid.h ftnameid.h tttypes.h \
	ttconfig.h ft_conf.h \
	ttdebug.h ttcalc.h ttmemory.h \
	ttinterp.h ttobjs.h ttengine.h ttmutex.h ttcache.h tttables.h ttcmap.h \
	tttypes.h
$(objdir)/ttload.obj: ttload.c ttconfig.h ft_conf.h \
	freetype.h fterrid.h ftnameid.h ttdebug.h ttcalc.h ttfile.h ttengine.h \
	ttmutex.h tttables.h ttobjs.h ttcache.h ttcmap.h ttmemory.h \
	tttags.h ttload.h
$(objdir)/ttmemory.obj: ttmemory.c ttdebug.h ttconfig.h ft_conf.h \
	tttypes.h freetype.h fterrid.h ftnameid.h ttmemory.h \
	ttengine.h ttmutex.h
$(objdir)/ttmutex.obj: ttmutex.c ttmutex.h ttconfig.h ft_conf.h \
	ttobjs.h
$(objdir)/ttobjs.obj: ttobjs.c ttconfig.h ft_conf.h \
	ttengine.h tttypes.h freetype.h fterrid.h ftnameid.h ttmutex.h ttcache.h \
	tttables.h ttcmap.h ttfile.h ttdebug.h ttcalc.h ttmemory.h \
	ttload.h ttinterp.h ttextend.h
$(objdir)/ttraster.obj: ttraster.c ttraster.h ttconfig.h ft_conf.h \
	freetype.h fterrid.h ftnameid.h ttengine.h tttypes.h ttmutex.h ttdebug.h \
	ttcalc.h ttmemory.h \
	
