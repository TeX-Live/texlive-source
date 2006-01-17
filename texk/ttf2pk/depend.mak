$(objdir)/ttf2tfm.obj: \
	ttf2tfm.h \
	newobj.h \
	ttfenc.h \
	ligkern.h \
	texenc.h \
	ttfaux.h \
	tfmaux.h \
	vplaux.h \
	errormsg.h \
	filesrch.h \
	parse.h \
	subfont.h

$(objdir)/case.obj: \
	case.h

$(objdir)/subfont.obj: \
	filesrch.h \
	ttf2tfm.h \
	subfont.h \
	newobj.h \
	errormsg.h

$(objdir)/parse.obj: \
	ttf2tfm.h \
	errormsg.h \
	newobj.h \
	ligkern.h \
	texenc.h \
	parse.h \
	filesrch.h

$(objdir)/newobj.obj: \
	newobj.h \
	ttf2tfm.h \
	errormsg.h \
	texenc.h

$(objdir)/errormsg.obj: \
	errormsg.h

$(objdir)/emtexdir.obj: \
	emdir.h \
	emtexdir.h

$(objdir)/ttfenc.obj: \
	newobj.h \
	ttf2tfm.h \
	ttfenc.h \
	errormsg.h

$(objdir)/emdir.obj: \
	emdir.h

$(objdir)/ttfaux.obj: \
	$(ttfdir)/freetype.h \
	$(ttfdir)/fterrid.h \
	$(ttfdir)/ftnameid.h \
	$(ttfdir)/extend/ftxkern.h \
	$(ttfdir)/extend/ftxpost.h \
	$(ttfdir)/extend/ftxopen.h \
	$(ttfdir)/extend/ftxgdef.h \
	$(ttfdir)/extend/ftxgsub.h \
	$(ttfdir)/extend/ftxgpos.h \
	ttf2tfm.h \
	newobj.h \
	ligkern.h \
	ttfenc.h \
	tfmaux.h \
	errormsg.h \
	ttfaux.h \
	filesrch.h

$(objdir)/filesrch.obj: \
	filesrch.h \
	ttf2tfm.h \
	newobj.h

$(objdir)/tfmaux.obj: \
	ttf2tfm.h \
	newobj.h \
	tfmaux.h \
	errormsg.h

$(objdir)/pklib.obj: \
	newobj.h \
	ttf2tfm.h \
	pklib.h \
	errormsg.h \
	filesrch.h

$(objdir)/ttf2pk.obj: \
	ttf2tfm.h \
	newobj.h \
	pklib.h \
	ttfenc.h \
	ttflib.h \
	errormsg.h \
	filesrch.h \
	parse.h \
	subfont.h

$(objdir)/ligkern.obj: \
	ttf2tfm.h \
	ligkern.h \
	ttfenc.h \
	texenc.h \
	newobj.h \
	errormsg.h

$(objdir)/vplaux.obj: \
	ttf2tfm.h \
	newobj.h \
	ttfenc.h \
	texenc.h \
	tfmaux.h \
	vplaux.h \
	errormsg.h \
	case.h

$(objdir)/ttflib.obj: \
	pklib.h \
	$(ttfdir)/freetype.h \
	$(ttfdir)/fterrid.h \
	$(ttfdir)/ftnameid.h \
	ttfenc.h \
	ttf2tfm.h \
	ttflib.h \
	errormsg.h \
	newobj.h \
	$(ttfdir)/extend/ftxpost.h \
	$(ttfdir)/extend/ftxopen.h \
	$(ttfdir)/extend/ftxgdef.h \
	$(ttfdir)/extend/ftxgsub.h \
	$(ttfdir)/extend/ftxgpos.h

$(objdir)/texenc.obj: \
	ttf2tfm.h \
	texenc.h

