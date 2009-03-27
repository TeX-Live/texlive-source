
/* some dummy functions and variables so that a few ff source files can be ignored */

#include "uiinterface.h"
#include "fontforgevw.h"
#include "splinefont.h"
#include <stdarg.h>
#include <stdio.h>
#include <basics.h>
#include <ustring.h>


char **gww_errors = NULL;
int gww_error_count = 0;

void gwwv_errors_free (void) { 
  int i;
  if (gww_error_count>0) {
	for (i=0;i<gww_error_count;i++) {
	  free(gww_errors[i]);
	}
	free(gww_errors);
	gww_error_count = 0;
	gww_errors = NULL;
  }
}


static void LUAUI_IError(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    fprintf(stderr, "Internal Error: " );
    vfprintf(stderr,format,ap);
    va_end(ap);
}

static void LUAUI__LogError(const char *format,va_list ap) {
    char buffer[400], *str;
    vsnprintf(buffer,sizeof(buffer),format,ap);
    str = utf82def_copy(buffer);
    gww_errors = realloc(gww_errors, (gww_error_count+2)*sizeof(char *));
    if (gww_errors==NULL) {
	  perror("memory allocation failed");
      exit(EXIT_FAILURE);
    }
    gww_errors[gww_error_count ] = str ;
    gww_error_count ++;
    gww_errors[gww_error_count ] = NULL;
}

/* this is not static because it is used by gwwiconv.c */

void LUAUI_LogError(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    LUAUI__LogError(format,ap);
    va_end(ap);
}

static void LUAUI_post_notice(const char *title,const char *statement,...) {
    va_list ap;
    (void)title;
    va_start(ap,statement);
    LUAUI__LogError(statement,ap);
    va_end(ap);
}

static void LUAUI_post_error(const char *title,const char *statement,...) {
    va_list ap;
    (void)title;
    va_start(ap,statement);
    LUAUI__LogError(statement,ap);
    va_end(ap);
}

static int LUAUI_ask(const char *title, const char **answers,
	int def, int cancel,const char *question,...) {
  (void)title;
  (void)answers;
  (void)cancel;
  (void)question;
return( def );
}

static int LUAUI_choose(const char *title, const char **choices,int cnt, int def,
	const char *question,...) {
  (void)title;
  (void)choices;
  (void)cnt;
  (void)question;
return( def );
}

static int LUAUI_choose_multiple(char *title, const char **choices,char *sel,
	int cnt, char *buts[2], const char *question,...) {
  (void)title;
  (void)choices;
  (void)sel;
  (void)cnt;
  (void)buts;
  (void)question;
return( -1 );
}

static char *LUAUI_ask_string(const char *title, const char *def,
	const char *question,...) {
  (void)title;
  (void)def;
  (void)question;
return( (char *) def );
}

static char *LUAUI_open_file(const char *title, const char *defaultfile,
	const char *initial_filter) {
  (void)title;
  (void)initial_filter;
  (void)defaultfile;
return( NULL );
}

static char *LUAUI_saveas_file(const char *title, const char *defaultfile,
	const char *initial_filter) {
  (void)title;
  (void)initial_filter;
return( copy(defaultfile) );
}

static void LUAUI_progress_start(int delay, const char *title, const char *line1,
	const char *line2, int tot, int stages) {
  (void)delay;
  (void)title;
  (void)line1;
  (void)line2;
  (void)tot;
  (void)stages;
}

static void LUAUI_void_void_noop(void) {
}

static void LUAUI_void_int_noop(int useless) {
  (void)useless;
}

static int LUAUI_int_int_noop(int useless) {
  (void)useless;
return( true );
}

static void LUAUI_void_str_noop(const char * useless) {
  (void)useless;
}

static int LUAUI_alwaystrue(void) {
return( true );
}

static int LUAUI_DefaultStrokeFlags(void) {
return( sf_correctdir );
}

struct ui_interface luaui_interface = {
    LUAUI_IError,
    LUAUI_post_error,
    LUAUI_LogError,
    LUAUI_post_notice,
    LUAUI_ask,
    LUAUI_choose,
    LUAUI_choose_multiple,
    LUAUI_ask_string,
    LUAUI_ask_string,			/* password */
    LUAUI_open_file,
    LUAUI_saveas_file,

    LUAUI_progress_start,
    LUAUI_void_void_noop,
    LUAUI_void_void_noop,
    LUAUI_void_int_noop,
    LUAUI_alwaystrue,
    LUAUI_alwaystrue,
    LUAUI_int_int_noop,
    LUAUI_void_str_noop,
    LUAUI_void_str_noop,
    LUAUI_void_void_noop,
    LUAUI_void_void_noop,
    LUAUI_void_int_noop,
    LUAUI_void_int_noop,
    LUAUI_alwaystrue,

    LUAUI_void_void_noop,

    NOUI_TTFNameIds,
    NOUI_MSLangString,

    LUAUI_DefaultStrokeFlags
};

/* some bits and pieces */

int URLFromFile(char *url,FILE *from) { 
  (void)url;
  (void)from;
  return false; 
}

/* print.c */
int pagewidth = 0, pageheight=0;	/* In points */
char *printlazyprinter=NULL;
char *printcommand=NULL;
int printtype = 0;

void ScriptPrint(FontViewBase *fv,int type,int32 *pointsizes,char *samplefile,
	unichar_t *sample, char *outputfile) {
  (void)fv;
  (void)type;
  (void)pointsizes;
  (void)samplefile;
  (void)sample;
  (void)outputfile;
}

int PdfDumpGlyphResources(void *pi, SplineChar *sc) {
  (void)pi;
  (void)sc;
  return 0;
}

/* autotrace.c */
int autotrace_ask=0, mf_ask=0, mf_clearbackgrounds=0, mf_showerrors=0;
char *mf_args = NULL;
int preferpotrace=0;

void *GetAutoTraceArgs(void) {
return NULL;
}

void SetAutoTraceArgs(void *a) {
  (void)a;
}

void FVAutoTrace(FontViewBase *fv,int ask) {
  (void)fv; (void)ask;
}

SplineFont *SFFromMF(char *filename) {
  (void)filename;
return NULL;
}

/* http.c */

FILE *URLToTempFile(char *url,void *_lock) {
  (void)_lock; (void)url;
   ff_post_error(_("Could not parse URL"),_("FontForge only handles ftp and http URLs at the moment"));
return( NULL );
}
