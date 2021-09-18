#include <c-auto.h>

#include <kpathsea/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#define nkf_open fopen
#define nkf_close(fp) {clear_infile_enc(fp); fclose(fp);}
#endif

#define PAGE_COMPOSIT_DEPTH 10

struct page {
	char *page;
	char *enc;
	int attr[PAGE_COMPOSIT_DEPTH];
};

struct index {
	int num;
	unsigned char words;
	char *org[3];
	char *dic[3];
	char *idx[3];
	struct page *p;
	int lnum;
};

/* convert.c */
void initkanatable(void);
int convert(char *buff1, char *buff2);
int pnumconv(char *page, int attr);
int dicread(const char *filename);

/* pageread.c */
int lastpage(const char *filename);

/* sort.c */
void wsort(struct index *ind, int num);
void pagesort(struct index *ind, int num);
int alphanumeric(char *c);
int alphabet(char *c);
int numeric(char *c);
int japanese(char *buff);
int chkcontinue(struct page *p, int num);

/* styfile.c */
void styread(const char *filename);

/* fread.c */
char *mfgets(char *buf, int size, FILE *fp);
int idxread(char *filename, int start);

/* fwrite.c */
int fprintf2   (FILE *fp, const char *format, ...);
void warn_printf(FILE *fp, const char *format, ...);
void verb_printf(FILE *fp, const char *format, ...);

struct index;
void indwrite(char *filename, struct index *ind, int pagenum);

#undef fprintf
#define fprintf fprintf2

#undef fputs
#define fputs   fputs2
