/* mfdisplay.h: include file for Metafont display routines.

   This file is public domain.  */

#define MF_DISPLAY_PROTOS(x) \
extern int mf_ ## x ## _initscreen (void); \
extern void mf_ ## x ## _updatescreen (void); \
extern void mf_ ## x ## _blankrectangle (screencol, screencol, screenrow, screenrow); \
extern void mf_ ## x ## _paintrow (screenrow, pixelcolor, transspec, screencol);

MF_DISPLAY_PROTOS(amiga)
MF_DISPLAY_PROTOS(epsf)
MF_DISPLAY_PROTOS(hp2627)
MF_DISPLAY_PROTOS(mftalk)
MF_DISPLAY_PROTOS(next)
MF_DISPLAY_PROTOS(regis)
MF_DISPLAY_PROTOS(sun)
MF_DISPLAY_PROTOS(tektronix)
MF_DISPLAY_PROTOS(uniterm)
MF_DISPLAY_PROTOS(win32)
MF_DISPLAY_PROTOS(x11)
MF_DISPLAY_PROTOS(trap)

#undef MF_DISPLAY_PROTOS
