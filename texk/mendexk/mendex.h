#ifdef WIN32
#define nkf_open fopen
#define nkf_close fclose
#endif

struct page {
	char *page;
	char *enc;
	char attr[3];
};

struct index {
	int num;
	char words;
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
int alphanumeric(char c);
int alphabet(char c);
int numeric(char c);
int japanese(char *buff);
int chkcontinue(struct page *p, int num);

/* styfile.c */
void styread(const char *filename);

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
