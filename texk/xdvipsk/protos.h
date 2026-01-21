#ifndef _PROTOS_H_

#ifndef XDVIPSK
/* see dvips.h for copyright notice */
#endif /* XDVIPSK */

#define _PROTOS_H_

#ifdef XDVIPSK
#include "freetype/freetype.h"
#endif /* XDVIPSK */

/* some types need declaration before being used */
struct nlink;
struct hps_link;
struct Char;
struct String;
struct tcd;

/******* prototypes for functions *******/

/* prototypes for functions from bbox.c */
extern void findbb(int bop);

#ifdef XDVIPSK
/* prototypes for functions from charcode.c */
extern chardesctype *add_chardesc(fontdesctype *f, int charcode);
extern chardesctype *find_chardesc(fontdesctype *f, int charcode);
extern void delete_chardesc(fontdesctype *f, int charcode);
extern void delete_all_chardesc(fontdesctype *f);
extern void sort_by_charcode(fontdesctype *f);
#endif /* XDVIPSK */

/* prototypes for functions from bitmapenc.c */
extern void bmenc_startsection(void) ;
extern void bitmapencopt(int) ;
extern int downloadbmencoding(const char *name, double scale, fontdesctype *curfnt) ;
extern void finishbitmapencoding(const char *name, double scale) ;

/* prototypes for functions from color.c */
extern void initcolor(void);
extern void background(char *bkgrnd);
extern void resetcolorstack(char *p, int outtops);
extern void bopcolor(int outtops);
extern void pushcolor(char *p, Boolean outtops);
extern void popcolor(Boolean outtops);

/* prototypes for functions from dopage.c */
extern void dopage(void);
#ifdef XDVIPSK
extern integer dvipos;
#endif /* XDVIPSK */

/* prototypes for functions from dosection.c */
extern void dosection(sectiontype *s, int c);
extern int InPageList(int i);
extern int ParsePages(char *s);

/* prototypes for functions from dospecial.c */
extern void specerror(const char *s);
extern void outbangspecials(void);
#ifdef XDVIPSK
extern void predospecial(int numbytes, Boolean scanning, boolean parse_maps);
#else
extern void predospecial(int numbytes, Boolean scanning);
#endif /* XDVIPSK */
extern void dospecial(int numbytes);
extern float *bbdospecial(int nbytes);

/* prototypes for functions from download.c */
quarterword *unpack_bb(chardesctype *c, integer *cwidth, integer *cheight,
                                        integer *xoff, integer *yoff) ;
#ifndef XDVIPSK
extern void download(charusetype *p, int psfont);
#else
extern void download(charusetype *p);
#endif /* XDVIPSK */
extern void makepsname(char *s, int n);
extern void lfontout(int n);
extern void dopsfont(sectiontype *fs);

/* prototypes for functions from dpicheck.c */
extern unsigned short dpicheck(unsigned short dpi);

/* prototypes for functions from drawPS.c */
#ifdef TPIC
extern void setPenSize(char *cp);
extern void addPath(char *cp);
extern void arc(char *cp, int invis);
extern void flushPath(int invis);
extern void flushDashed(char *cp, int dotted);
extern void flushSpline(char *cp);
extern void SetShade(char *cp);
extern void shadeLast(char *cp);
extern void whitenLast(void);
extern void blackenLast(void);
#endif /* TPIC */

/* prototypes for functions from dviinput.c */
extern short dvibyte(void);
extern unsigned short twobytes(void);
extern int threebytes(void);
extern short signedbyte(void);
extern short signedpair(void);
extern int signedtrio(void);
extern int signedquad(void);
extern void skipover(int i);

/* prototypes for functions from dvips.c */
extern void help(int status);
extern void error_with_perror(const char *s, const char *fname);
extern void error(const char *s);
extern void check_checksum(unsigned int c1, unsigned int c2, const char *name);
extern char *mymalloc(int n);
extern void checkstrings(void);
extern char *newstring(const char *s);
extern void *revlist(void *p);
#ifdef XDVIPSK
extern void dvips_exit(int code);
extern void writelogrecord(const char *s);
extern void morestrings(void);
#ifndef WIN32
extern char *strlwr(char *str);
#endif
#endif /* XDVIPSK */

/* prototypes for functions from emspecial.c */
extern void emclear(void);
extern void emspecial(char *p);

/* prototypes for functions from finclude.c */
extern void scanfontcomments(const char *filename);
extern void fonttableout(void);

/* prototypes for functions from flib.c */
#ifdef FONTLIB
extern void fliload(void);
extern char *fliparse(char *path, char *name);
#endif

/* prototypes for functions from fontdef.c */
extern fontdesctype *newfontdesc(int cksum, int scsize, int dssize, char *name, char *area);
extern fontdesctype *matchfont(char *name, char *area, int scsize, char *scname);
extern void fontdef(int siz);
extern int skipnop(void);

/* prototypes for functions from header.c */
extern int add_name(const char *s, struct header_list **what );
extern void checkhmem(const char *s, char *p, char *q);
extern int add_header(const char *s);
extern int add_header_general(const char *s, char *pre, char* post);
extern char *get_name(struct header_list **what );
extern void send_headers(void);

/* prototypes for functions from hps.c */
#ifdef HPS
extern void do_html(char *s);
extern void finish_hps(void);
extern void set_bitfile(const char *s, int mode);
extern void vertical_in_hps(void);
extern void end_current_box(void);
extern void start_new_box(void);
#endif /* HPS */

/* prototypes for functions from loadfont.c */
extern void badpk(const char *s);
extern short pkbyte(void);
extern int pkquad(void);
extern int pktrio(void);
extern void loadfont(fontdesctype *curfnt);

#ifdef XDVIPSK
/* prototypes for functions from luamap.c */
extern void LuaMap_cache_init(void);
extern luamaptype *LuaMap_cache_get(int id);
extern int LuaMap_cache_find(const char *luamap_name);
extern void LuaMap_cache_close(void);
#endif /* XDVIPSK */

/* prototypes for functions from makefont.c */
extern void makefont(char *name, int dpi, int bdpi);

/* prototypes for functions from output.c */
extern void copyfile(const char *s);
extern void copyfile_general(const char *s, struct header_list *h);
extern void figcopyfile(char *s, int systemtype);
extern void specialout(char c);
extern void stringend(void);
#ifdef SHIFTLOWCHARS
extern int T1Char(int c);
#endif
extern void scout(unsigned char c);
extern void cmdout(const char *s);
extern void floatout(float n);
extern void doubleout(double n);
extern void numout(int n);
extern void mhexout(unsigned char *p, long len);
extern void hvpos(void);
extern void newline(void);
extern void open_output(void);
extern void nlcmdout(const char *s);
extern void initprinter(sectiontype *sect);
extern void setup(void);
extern void cleanprinter(void);
extern void psflush(void);
extern void pslineout(const char *s);
extern void psnameout(const char *s);
extern void pageinit(void);
extern void pageend(void);
extern void drawrule(int rw, int rh);
extern void cmddir(void);
extern void drawchar(chardesctype * c, int cc);

/* prototypes for functions from papersiz.c */
extern void handlepapersize(char *p, int *x, int *y);

/* prototypes for functions from pprescan.c */
extern void pprescanpages(void);

/* prototypes for functions from prescan.c */
extern void readpreamble(void);
#ifdef XDVIPSK
/* parse_maps == TRUE -- first transparent pass without any processing, exept of parsing of mapline and g2umap* specials */
extern void prescanpages(boolean parse_maps);
#else
extern void prescanpages(void);
#endif /* XDVIPSK */

/* prototypes for functions from repack.c */
extern long getlong(unsigned char *a);
extern void dochar(unsigned char *from, short width, short height);
extern void repack(struct tcd *cp);

/* prototypes for functions from resident.c */
#ifndef XDVIPSK
extern unsigned int hash(char *s);
#else  /* XDVIPSK */
extern unsigned int hash(const char *s);
#endif /* XDVIPSK */
extern void revpslists(void);
extern void cleanres(void);
#ifndef XDVIPSK
extern struct resfont *lookup(char *name);
extern struct resfont *findPSname(char *name);
#else  /* XDVIPSK */
extern boolean otftype_conforms(quarterword /* otftype_enum */ font_otftype, quarterword /* otftype_enum */ otftype);
extern struct resfont *lookup(char *name);            /* search by resfont::TeXname */
extern resfont_ref *lookup_v(const char *vect_name);  /* search by the font name resfont::Vectfile */
extern resfont_ref *lookup_ps(const char *ps_name, quarterword /* otftype_enum */ otftype);    /* search by resfont::PSname and resfont::otftype */
                                                      /* searches for the last occurence of the resfont record with given PSname and the encoding vector luamap_idx properly set */
                                                      /* findPSname() in contrary, searches for the first occurence of the resfont record without an encoding vector luamap_idx */
extern struct resfont *findPSname(char *ps_name, quarterword /* otftype_enum */ otftype);      /* slow search by resfont::PSname */
#endif /* XDVIPSK */
extern int residentfont(fontdesctype *curfnt);
extern Boolean getdefaults(const char *s);
extern void getpsinfo(const char *name);
extern void checkenv(int which);
#ifdef XDVIPSK
extern void getotfinfo(const char *dviname);
extern void add_entry_spec(char *TeXname, char *PSname, char *Fontfile,
					char *Vectfile, char *specinfo, char *downloadinfo, boolean partial, boolean isOTF, quarterword embolden, boolean replace);
extern void getpsinfo_spec(const char *name, boolean replace);
extern void parse_otf_map_line(const char *map_line);
#endif /* XDVIPSK */

/* prototypes for functions from scalewidth.c */
extern int scalewidth(int a, int b);

/* prototypes for functions from scanpage.c */
extern Boolean prescanchar(chardesctype *cd);
extern Boolean preselectfont(fontdesctype *f);
#ifdef XDVIPSK
extern short scanpage(boolean parse_maps);
#else
extern short scanpage(void);
#endif /* XDVIPSK */

/* prototypes for functions from search.c */
#ifdef KPATHSEA
extern FILE *search(kpse_file_format_type format, const char *file, const char *mode);
extern FILE *pksearch(const char *file, const char *mode, halfword dpi, const char **name_ret, int *dpi_ret);
#else /* !KPATSHEA */
extern FILE *search(char *path, const char *file, const char *mode);
extern FILE *pksearch(char *path, const char *file, const char *mode, char *n, halfword dpi, halfword vdpi);
#endif /* KPATHSEA */
#ifdef XDVIPSK
/* search() variant with preserving of the realnameoffile */
extern FILE *search_safe(kpse_file_format_type format, const char *file_name, const char *mode, boolean verbose);
extern void parse_g2u(const char *pfb_fname, boolean encoding_only);
#endif /* XDVIPSK */
extern FILE *my_real_fopen(const char *n, const char *t);
extern int close_file(FILE *f);

#ifdef XDVIPSK
/* prototypes for functions from sfntload.c */
#ifndef KPATHSEA
extern char *expandfilename(const char *src);
#endif /* KPATHSEA */
extern int sfntload(fontdesctype *curfnt);
#endif /* XDVIPSK */

/* prototypes for functions from skippage.c */
extern void skippage(void);

/* prototypes for functions from t1part.c */
extern void *getmem(unsigned int size);
extern struct Char *UnDefineChars(struct Char *TmpChar);
extern int FontPart(FILE *fout, unsigned char *fontfile, unsigned char *vectfile);

/* prototypes for functions from tfmload.c */
extern void badtfm(const char *s);
extern void tfmopen(fontdesctype *fd);
extern unsigned short tfm16(void);
extern int tfm32(void);
extern int tfmload(fontdesctype *curfnt);

/* prototypes for functions from unpack.c */
extern long unpack(unsigned char *pack, unsigned short *raster,
                    unsigned short cwidth, unsigned short cheight, unsigned short cmd);

/* prototypes for functions from virtualfont.c */
extern Boolean virtualfont(fontdesctype *curfnt);

#ifdef XDVIPSK
/* prototypes for functions from writecid.c */
extern int writecid(charusetype *p);
#endif /* XDVIPSK */

/* prototypes for functions from writet1.c */
extern char **load_enc_file(char *);
extern boolean t1_subset_2(char *, unsigned char *, char *);
#ifdef XDVIPSK
/* breaks line if in the middle of the latter */
extern void printf_pr(const char* fmt, ...);
extern const char *err_file;
extern int err_line;

#ifdef _MSC_VER
#define PRINTF_PR(fmt, ...) { err_file = __FILE__; err_line = __LINE__; printf_pr(fmt, __VA_ARGS__); }
#else
#define PRINTF_PR(fmt, ...) { err_file = __FILE__; err_line = __LINE__; printf_pr(fmt __VA_OPT__(,) __VA_ARGS__); }
#endif

extern void load_touni_tables(void);
/* verbose: 2 - all messages; 1 -- info messages, no file not found error; 0 -- critical messages only */
/* pfb_name -- pfb:.../ prefix to prepend to every glyph; could be NULL */
extern void load_touni_file(const char *map_fname, boolean overwrite, const char *pfb_name, int verbose);
/* returns an array of unicode values, corresponding to the glyph; NULL in case of unrecognized glyph name */
/* returned array, if not NULL, should be RELEASE'd after the call */
/* *p_ucnt: returned count of elements in the returned array */
/* prefix and font_name are used for searching of glyphs, specific for particular fonts only ("tfm:pzdr/a105", "pfb:msbm10/A", for example) */
/* prefix: "tfm:", "pfb:", "processed:" or "otf:" */
/* font_name: LuaMap::name (/FullName) value for .otf fonts (prefix "otf:"), font file name without extension for entries of the whole .pfb fonts (prefices "pfb:" and "processed:"), TeXname for partial encodings of a .pfb file (prefix "tfm:") */
/* prefix and font_name can be NULL in case of searching for common glyph names only */
/* exact: look for the glyph_name only, do not parse compounds, ligatures or "uni..." glyphnames */
/* *p_needs_g2u is returned with FALSE value, if the code was scanned from "uniXXXX" or "uXXXXX" like glyph name; the passed pointer can be NULL, if no need for checking */
extern unsigned int *get_glname_tounicode(const char *glyph_name, const char *font_name, const char *prefix, boolean exact, int *p_ucnt, boolean *p_needs_g2u);
/* converts UTF-16 set with possible high value surrogates to an array of long UTF-32 codes */
/* returned array, if not NULL, should be RELEASE'd after the call */
extern unsigned long *utf16_2_uni32(const unsigned int *tounicode, int ucnt, int *p_uni32_cnt);

/* various glyph name renaming functions */
/* returned value, if not NULL, should be free'd after use */
/* p_needs_g2u is returned with FALSE value, if the name returned was formatted using an "uniXXXX" or "uXXXXX" pattern; the passed pointer can be NULL */
extern char *glyph_name_cvt(const char *glyph_name, const char *font_name, boolean *p_needs_g2u);
extern char *get_glname_substitute(const char *glyph_name, const char *font_name, const char *prefix, boolean exact, boolean *p_needs_g2u);
extern char *make_uni_glyph_name(unsigned int *tounicode, int ucnt);
/* returns the file name part with extension removed; should be free'ed after use */
/* returns pointer to the file name part inside of the fpath (with the extension) to *fname_ptr_p if the latter is not NULL */
extern char *extract_fname(const char *fpath, const char **fname_ptr_p);

extern char **getEncoding(char *encoding, char **builtin);
extern boolean t1_write_full(char *, unsigned char *);
#endif /* XDVIPSK */

/*********** global variables ***********/

/* global variables from bitmapenc.c */
extern int encodetype3 ;

/* global variables from dopage.c */
extern integer dir;
#ifdef HPS
extern integer hhmem, vvmem;
extern integer pushcount;
extern Boolean PAGEUS_INTERUPPTUS;
extern Boolean NEED_NEW_BOX;
#endif

/* global variables from dosection.c */
#ifdef HPS
extern int pagecounter;
#endif

/* global variables from dvips.c */
extern int found_problems;
extern char *downloadedpsnames[];
extern int unused_top_of_psnames;
extern fontdesctype *fonthead;
extern fontdesctype *curfnt;
extern sectiontype *sections;
extern Boolean partialdownload;
#ifdef XDVIPSK
extern Boolean t1_partialdownload;
extern Boolean cid_partialdownload;
#endif /* XDVIPSK */
extern Boolean manualfeed;
extern Boolean landscaperotate;
extern Boolean compressed;
extern Boolean downloadpspk;
extern Boolean safetyenclose;
extern Boolean removecomments;
extern Boolean nosmallchars;
extern Boolean cropmarks;
extern Boolean abspage;
extern Boolean tryepsf;
extern int secure;
extern int secure_option;
extern int collatedcopies;
extern integer pagecopies;
extern shalfword linepos;
extern integer maxpages;
extern Boolean notfirst, notlast;
extern Boolean evenpages, oddpages, pagelist;
extern Boolean sendcontrolD;
extern Boolean shiftlowchars;
extern integer firstpage, lastpage;
extern integer firstseq, lastseq;
extern integer hpapersize, vpapersize;
extern integer hoff, voff;
extern integer maxsecsize;
extern integer firstboploc;
extern Boolean sepfiles;
extern int numcopies;
extern char *titlename;
extern const char *oname;
extern char *iname;
extern char *fulliname;
extern char *nextstring, *maxstring;
extern FILE *dvifile, *bitfile;
extern quarterword *curpos, *curlim;
extern fontmaptype *ffont;
extern real conv;
extern real vconv;
extern real alpha;
extern double mag;
extern integer num, den;
extern int overridemag;
extern int actualdpi, vactualdpi;
extern int maxdrift, vmaxdrift;
extern char *paperfmt;
extern int landscape;
extern integer fontmem;
extern integer pagecount;
extern integer pagenum;
extern long bytesleft;
extern quarterword *raster;
extern integer hh, vv;
extern Boolean noomega;
extern Boolean noptex;
extern Boolean lastpsizwins;
#ifdef XDVIPSK
extern Boolean noluatex;
extern Boolean noToUnicode;
extern Boolean VTEX_SPEC_MODE;
extern quarterword /* otftype_enum */ inotftype;
#endif /* XDVIPSK */
extern const char *infont;
#ifndef KPATHSEA
extern char *tfmpath;
extern char *pkpath;
extern char *vfpath;
extern char *figpath;
extern char *headerpath;
extern char *configpath;
extern char *pictpath;
#ifdef SEARCH_SUBDIRECTORIES
extern char *fontsubdirpath;
#endif
#endif /* ! KPATHSEA */
#ifdef FONTLIB
extern char *flipath;
extern char *fliname;
#endif
extern integer swmem;
extern int quiet;
extern int filter;
extern int dvips_debug_flag;
extern int prettycolumn;
extern int gargc;
extern char **gargv;
extern int totalpages;
extern Boolean reverse;
extern Boolean usesPSfonts;
#ifdef XDVIPSK
extern Boolean usesOTFfonts;
extern otfcmaptype *OTF_list;
extern FT_Library ft_lib;
#endif /* XDVIPSK */
extern Boolean usesspecial;
extern Boolean headers_off;
extern Boolean usescolor;
extern char *warningmsg;
extern Boolean multiplesects;
extern Boolean disablecomments;
extern char *printer;
extern char *mfmode;
extern char *mflandmode;
extern int mfmode_option;
extern int oname_option;
extern frametype frames[];
extern integer pagecost;
extern integer fsizetol;
extern Boolean includesfonts;
extern fontdesctype *fonthd[MAXFONTHD];
extern int nextfonthd;
extern char xdig[256];
#ifndef XDVIPSK
extern char banner[], banner2[];
#else
extern char banner[], banner2[], banner3[];
#endif /* XDVIPSK */
extern Boolean noenv;
extern Boolean dopprescan;
extern int dontmakefont;
extern struct papsiz *papsizes;
extern int headersready;
#if defined(MSDOS) || defined(OS2) || defined(ATARIST)
extern char *mfjobname;
extern FILE *mfjobfile;
#endif
#ifdef DEBUG
extern integer debug_flag;
#endif
#ifdef HPS
extern Boolean HPS_FLAG;
#endif

/* global variables from flib.c */
#ifdef FONTLIB
extern Boolean flib;
#endif

/* global variables from hps.c */
#ifdef HPS
extern Boolean inHTMLregion;
extern integer HREF_COUNT;
extern int current_pushcount;
extern Boolean noprocset;
#endif

/* global variables from loadfont.c */
extern char errbuf[1500];
extern int lastresortsizes[40];
extern FILE *pkfile;

/* global variables from output.c */
extern char preamblecomment[256];
extern integer rdir, fdir;

/* global variables from pprescan.c */
extern Boolean pprescan;

/* global variables from repack.c */
extern long mbytesleft;
extern quarterword *mraster;

/* global variables from resident.c */
extern struct header_list *ps_fonts_used;
extern const char *psmapfile;
#ifdef XDVIPSK
extern const char *luascript;
extern Boolean lua_prescan_specials;
extern Boolean lua_scan_specials;
#endif

/* global variables from search.c */
extern int to_close;
#ifdef KPATHSEA
extern char *realnameoffile;
#else
extern char realnameoffile[];
#endif

/* global variables from tfmload.c */
extern FILE *tfmfile;

/* global variables from loadfont.c */
extern int bitmapfontseen ;

#ifdef XDVIPSK
/* global variables and prototypes for Lua callbacks*/
#include "lua.h"
extern lua_State *L;
extern Boolean lua_prescan_specials;
extern Boolean lua_scan_specials;
extern Boolean lua_after_prescan;
extern Boolean lua_after_drawchar;
extern Boolean lua_after_drawrule;
extern Boolean lua_process_stack;
extern Boolean lua_dvips_exit;
extern Boolean lua_callback_defined(lua_State *L, const char* lua_callback);
extern void load_lua_scripts(const char* luascript);
extern int run_lua_specials(lua_State *L, const char* lua_func, char* p, Boolean lua_available);
extern void run_lua_after_prescan(lua_State *L);
extern void run_lua_after_drawchar(lua_State *L, chardesctype *c, int cc, int rhh, int rvv, integer dir, int lastfont, int tu_count, unsigned int *tounicode);
extern void run_lua_after_drawrule(lua_State *L, integer rw, integer rh);
extern void run_lua_process_stack(lua_State *L, const char *cmd);
extern void run_lua_dvips_exit(lua_State *L, int code, int log_record_count);
#endif /* XDVIPSK */

#endif
