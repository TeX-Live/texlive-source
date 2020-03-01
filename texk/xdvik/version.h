/* Version info that is displayed at -h, -v and in GUI elements */
#ifndef VERSION_H_
#define VERSION_H_

#define XDVI_VERSION "22.87.04"

#ifdef MOTIF
#define XDVI_GUI "(Motif toolkit)"
#else
#define XDVI_GUI "(Xaw toolkit)"
#endif

#define	XDVIK_PROGNAME "xdvik"
#define	XDVI_PROGNAME "xdvi"

#define XDVI_VERSION_INFO XDVI_VERSION" "XDVI_GUI
#define XDVI_TERSE_VERSION_INFO XDVI_VERSION

#endif /* VERSION_H_ */
