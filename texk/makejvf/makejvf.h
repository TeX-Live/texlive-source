extern char *vtfmname,*kanatfm,*jistfm,*ucsqtfm,*usertable;
extern int unit,zh,zw,jfm_id,rightamount;
extern int kanatume,chotai,baseshift,minute,hankana,fidzero,enhanced,omitzw;
extern int pstfm_nt;
extern FILE *afp;
extern long ucs;

/* main.c */
void usage(void);

/* tfmread.c */
int jfmread(int kcode);
int tfmget(char *name);
int tfmidx(FILE *fp);

/* tool.c */
int mquad(unsigned char *p);
unsigned int upair(unsigned char *p);
int fquad(FILE *fp);
unsigned int ufpair(FILE *fp);
int fpair(FILE *fp);
int fputnum(int num, int byte, FILE *fp);
int numcount(int num);
int fputnum2(int num, FILE *fp);
int fputstr(char *str, int byte, FILE *fp);

/* write.c */
FILE *vfopen(char *name);
void writevf(int code, FILE *fp);
void writevfu(int code, FILE *fp);
void vfclose(FILE *fp);
void maketfm(char *name);
