struct page {
	char *page;
	char *enc;
	char attr[3];
};

struct index {
	int num;
	char words;
	unsigned char *org[3];
	unsigned char *dic[3];
	unsigned char *idx[3];
	struct page *p;
	int lnum;
};

/* convert.c */
void initkanatable(void);
int convert(unsigned char *buff1, unsigned char *buff2);
int pnumconv(char *page, int attr);
int dicread(char *filename);

/* pageread.c */
int lastpage(char *filename);

/* sort.c */
void wsort(struct index *ind, int num);
void pagesort(struct index *ind, int num);
int alphanumeric(unsigned char c);
int alphabet(unsigned char c);
int numeric(unsigned char c);
int japanese(unsigned char *buff);
int chkcontinue(struct page *p, int num);

/* styfile.c */
void styread(char *filename);

/* fread.c */
char *mfgets(char *buf, int byte, FILE *fp);
int idxread(char *filename, int start);

/* fwrite.c */
int fprintf2   (FILE *fp, const char *format, ...);
int warn_printf(FILE *fp, const char *format, ...);
int verb_printf(FILE *fp, const char *format, ...);

struct index;
void indwrite(char *filename, struct index *ind, int pagenum);

#ifdef fprintf
#undef fprintf
#endif
#define fprintf fprintf2

#ifdef fputs
#undef fputs
#endif
#define fputs   fputs2
