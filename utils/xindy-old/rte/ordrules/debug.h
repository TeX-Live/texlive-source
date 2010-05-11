/* $Id: debug.h,v 1.3 2005/05/02 21:39:53 jschrod Exp $
 *------------------------------------------------------------
 *
 *  AU: Makros zur Unterstuetzung von Debug-Ausgaben
 *      with friendly permission by U. Graef
 *
 *  ST: Um die Macros zu definieren:
 *          #define debug
 *          #include "debug.h"
 *      Im Hauptprogramm bzw. Tester mu"s dann noch debug.o dazugebunden
 *      werden, um die externe Variable Debug_Output_Indent zur Verf"ugung
 *      zu stellen.
 *      Um im Modul die Macros zu deaktivieren:
 *          #undef debug
 *          #include "debug.h"
 *
 *  PO: HPUX, Linux, AIX
 */


#ifndef _debug_h
#define _debug_h

/*
 *  Beim ersten include werden folgende Deklarationen defininert:
 *
 *  Die globale Variable f"ur die Einr"uckung wird definiert.
 */
extern
int     Debug_Output_Indent;

#ifdef debug_special
/*
 *  Die Funktion f"ur den Memory-Print wird definiert
 */
extern
void    debug_printmem_asc( char* desc, char* str, char* lenname, int len );
#endif /* debug_special */

#define	Debug_Output_Indent_Step	4

#endif  /* _debug_h */


/*
 *  Falls die Macros bereits definiert waren, dann werden sie jetzt
 *  zuerst wegdefiniert. Damit kann man "debug.h" benutzen um die Macros
 *  zu (de-)aktivieren.
 */

#ifdef dispstart

#undef dispstart
#undef dispend
#undef displong
#undef dispint
#undef dispuint
#undef dispdouble
#undef dispchar
#undef dispstr
#undef dispmem
#undef dispmsg

#endif

/*
 *  Hier werden die Macros wieder definert bzw. erstmals definiert
 */
#ifdef debug

/* Start und Ende von Prozeduren	*/

#define dispstart(p)							\
	{								\
	fprintf( stderr, "\n%*s%s: Start -----\n",                      \
                         Debug_Output_Indent, "", #p );	                \
	Debug_Output_Indent += Debug_Output_Indent_Step;		\
	}

#define dispend(p)							\
	{								\
	Debug_Output_Indent -= Debug_Output_Indent_Step;		\
	fprintf( stderr, "%*s%s: End   -----\n\n",                      \
                          Debug_Output_Indent, "", #p );	        \
	}


/* Ausgabe von Werten verschiedenen Typs		*/
/*	(mit Ausgabe der Variablen bzw. des Ausdrucks)	*/


#define displong(l)	\
	fprintf( stderr, "%*s%s = %ld = 0x%lX \n",\
                          Debug_Output_Indent, "", #l, (long) l, (long) l );


#define dispint(i)	\
	fprintf( stderr, "%*s%s = %d = 0x%X \n", \
                          Debug_Output_Indent, "", #i, (int) i, (int) i );

#define dispuint(i)	\
	fprintf( stderr, "%*s%s = %d = 0x%X \n", \
                          Debug_Output_Indent, "", #i, (unsigned int) i, (unsigned int) i );

#define dispdouble(d)	\
	fprintf( stderr, "%*s%s = %20.10g \n",\
                           Debug_Output_Indent, "", #d, (double) d );

#define dispchar(c)	\
	fprintf( stderr, "%*s%s = %d = 0x%X = '%c' \n",                 \
                          Debug_Output_Indent, "", #c, (char) c,        \
                          (char) c, (char) c );

#define dispstr(s)	\
	fprintf( stderr, "%*s%s = \"%s\"\n",                           \
                          Debug_Output_Indent, "", #s, (char*) s );

#ifdef debug_special
#define dispmem(str,len)                                                \
        debug_printmem_asc( (char*)#str, (char*)str, (char*)#len, (int) len );
#endif /* debug_special */

#define dispmsg(p)							\
	{								\
	fprintf( stderr, "%*s%s:\n", Debug_Output_Indent, "", #p );	\
	}

#else  /* debug */

/*
 *  Falls kein weiteres Debugging erw"unscht, dann definiere die
 *  Macros auf leer.
 */
#define dispstart(p)
#define dispend(p)
#define displong(l)
#define dispint(i)
#define dispuint(i)
#define dispdouble(d)
#define dispchar(c)
#define dispstr(s)
#define dispmem(str,len)
#define dispmsg(p)

#endif  /* debug */


/*======================================================================
 *
 * $Log: debug.h,v $
 * Revision 1.3  2005/05/02 21:39:53  jschrod
 *     xindy run time engine 3.0; as used for CLISP 2.33.2.
 *
 * Revision 1.2  1996/07/18 15:56:38  kehr
 * Checkin after all changes that resulted from the define-letter-group
 * modification were finished. Additionally I found an ugly bug in the
 * ordrules.c file that was discovered when running the system under
 * Solaris (which seems to have signed chars..Whee!). This is fixed now
 * and the Imakefiles and that stuff was improved, too.
 *
 * Revision 1.1  1996/03/27  20:29:03  kehr
 * It works. Today I had my first success in getting the FFI running with
 * the ordrules-library. The interface is defined in `ordrulei.lsp' and
 * allows direct access to the functions in `ordrules.c'.
 *
 */
