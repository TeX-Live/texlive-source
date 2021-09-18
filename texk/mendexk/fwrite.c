#include "mendex.h"

#include <stdarg.h>

#include <kpathsea/tex-file.h>
#include <ptexenc/ptexenc.h>
#include <ptexenc/unicode.h>
#include <ptexenc/unicode-jp.h>

#include "exkana.h"
#include "exvar.h"

int line_length=0;

static void printpage(struct index *ind, FILE *fp, int num, char *lbuff);
static int range_check(struct index ind, int count, char *lbuff);
static void linecheck(char *lbuff, char *tmpbuff, int force);
static void crcheck(char *lbuff, FILE *fp);

/* All buffers have size BUFFERLEN.  */
#define BUFFERLEN 4096

#ifdef HAVE___VA_ARGS__
/* Use C99 variadic macros if they are supported.  */
/* We would like to use sizeof(buf) instead of BUFFERLEN but that fails
   for, e.g., gcc-4.8.3 on Cygwin and gcc-4.5.3 on NetBSD.  */
#define SPRINTF(buf, ...) \
    snprintf(buf, BUFFERLEN, __VA_ARGS__)
#define SAPPENDF(buf, ...) \
    snprintf(buf + strlen(buf), BUFFERLEN - strlen(buf), __VA_ARGS__)
#else
/* Alternatively use static inline functions.  */
static inline int SPRINTF(char *buf, const char *format, ...)
{
    va_list argptr;
    int n;

    va_start(argptr, format);
    n = vsnprintf(buf, BUFFERLEN, format, argptr);
    va_end(argptr);

    return n;
}
static inline int SAPPENDF(char *buf, const char *format, ...)
{
    va_list argptr;
    int n;

    va_start(argptr, format);
    n = vsnprintf(buf + strlen(buf), BUFFERLEN - strlen(buf), format, argptr);
    va_end(argptr);

    return n;
}
#endif

static void fprint_euc_char(FILE *fp, const char a, const char b)
{
	if (is_internalUPTEX()) {  /* convert a character from EUC to UTF8 */
		int k = 0;
		unsigned char str[5];
		int chr = (unsigned char)a<<8 | (unsigned char)b;
		chr = (chr==0xffff) ? U_REPLACEMENT_CHARACTER : JIStoUCS2(chr & 0x7f7f);
		chr = UCStoUTF8(chr);
		/* if (BYTE1(chr) != 0){ str[k++] = BYTE1(chr); } */  /* do not happen */
		if (BYTE2(chr) != 0){ str[k++] = BYTE2(chr); }
		if (BYTE3(chr) != 0){ str[k++] = BYTE3(chr); }
		                      str[k++] = BYTE4(chr);
		                      str[k++] = '\0';
		fprintf(fp,"%s",str);
	}
	else
		fprintf(fp,"%c%c",a,b);
}

/*   fprintf with convert kanji code   */
int fprintf2(FILE *fp, const char *format, ...)
{
    char print_buff[8000];
    va_list argptr;
    int n;

    va_start(argptr, format);
    n = vsnprintf(print_buff, sizeof print_buff, format, argptr);
    va_end(argptr);

    fputs(print_buff, fp);
    return n;
}

void warn_printf(FILE *fp, const char *format, ...)
{
    char print_buff[8000];
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(print_buff, sizeof print_buff, format, argptr);
    va_end(argptr);

    warn++;
    fputs(print_buff, stderr);
    if (fp!=stderr) fputs(print_buff, fp);
}

void verb_printf(FILE *fp, const char *format, ...)
{
    char print_buff[8000];
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(print_buff, sizeof print_buff, format, argptr);
    va_end(argptr);

    if (verb!=0)    fputs(print_buff, stderr);
    if (fp!=stderr) fputs(print_buff, fp);
}

static int pnumconv2(struct page *p)
{
	int j,k,cc,pclen;

	pclen=strlen(page_compositor);
	for (j=k=cc=0;j<strlen(p->page);j++) {
		if (strncmp(p->page+j,page_compositor,pclen)==0) {
			j+=pclen;
			k=j;
			cc++;
			continue;
		}
	}
	return pnumconv(p->page+k,p->attr[cc]);
}


/*   write ind file   */
void indwrite(char *filename, struct index *ind, int pagenum)
{
	int i,j,hpoint=0;
	char datama[2048],lbuff[BUFFERLEN];
	FILE *fp=NULL;
	int conv_euc_to_euc;
	char *init, *init_prev;

	if (filename && kpse_out_name_ok(filename))
		fp=fopen(filename,"wb");
	if (fp == NULL) {
		fp=stdout;
#ifdef WIN32
		setmode(fileno(fp), _O_BINARY);
#endif
	}

	conv_euc_to_euc = is_internalUPTEX() ? 1 : 0;
	if (conv_euc_to_euc) set_enc_string(NULL, "euc");
	convert(atama,datama);
	if (conv_euc_to_euc) set_enc_string(NULL, "uptex");
	fputs(preamble,fp);

	if (fpage>0) {
		fprintf(fp,"%s%d%s",setpage_prefix,pagenum,setpage_suffix);
	}

	if (strlen(symhead)==0) {
		if (lethead_flag>0) {
			strcpy(symhead, symhead_positive);
		}
		else if (lethead_flag<0) {
			strcpy(symhead, symhead_negative);
		}
	}
	{
		if (lethead_flag>0) {
			strcpy(numhead, numhead_positive);
		}
		else if (lethead_flag<0) {
			strcpy(numhead, numhead_negative);
		}
	}

	for (i=line_length=0;i<lines;i++) {
		init = ind[i].dic[0];
		if (i==0) {
			if (alphabet(init)) {
				if (lethead_flag>0) {
					fprintf(fp,"%s%c%s",lethead_prefix,init[0],lethead_suffix);
				}
				else if (lethead_flag<0) {
					fprintf(fp,"%s%c%s",lethead_prefix,init[0]+32,lethead_suffix);
				}
				SPRINTF(lbuff,"%s%s",item_0,ind[i].idx[0]);
			}
			else if (japanese(init)) {
				if (lethead_flag) {
					fputs(lethead_prefix,fp);
					for (j=hpoint;j<(strlen(datama)/2);j++) {
						if ((unsigned char)init[1]<(unsigned char)datama[j*2+1]) {
							fprint_euc_char(fp,atama[(j-1)*2],atama[(j-1)*2+1]);
							hpoint=j;
							break;
						}
					}
					if (j==(strlen(datama)/2)) {
						fprint_euc_char(fp,atama[(j-1)*2],atama[(j-1)*2+1]);
					}
					fputs(lethead_suffix,fp);
				}
				SPRINTF(lbuff,"%s%s",item_0,ind[i].idx[0]);
				for (hpoint=0;hpoint<(strlen(datama)/2);hpoint++) {
					if ((unsigned char)init[1]<(unsigned char)datama[hpoint*2+1]) {
						break;
					}
				}
			}
			else {
				if (lethead_flag!=0 && symbol_flag==2 && numeric(init)) {
					fprintf(fp,"%s%s%s",lethead_prefix,numhead,lethead_suffix);
				}
				if (lethead_flag!=0 && (symbol_flag==1 || (symbol_flag==2 && !numeric(init)))) {
					fprintf(fp,"%s%s%s",lethead_prefix,symhead,lethead_suffix);
				}
				SPRINTF(lbuff,"%s%s",item_0,ind[i].idx[0]);
			}
			switch (ind[i].words) {
			case 1:
				SAPPENDF(lbuff,"%s",delim_0);
				break;

			case 2:
				SAPPENDF(lbuff,"%s",item_x1);
				SAPPENDF(lbuff,"%s",ind[i].idx[1]);
				SAPPENDF(lbuff,"%s",delim_1);
				break;

			case 3:
				SAPPENDF(lbuff,"%s",item_x1);
				SAPPENDF(lbuff,"%s",ind[i].idx[1]);
				SAPPENDF(lbuff,"%s",item_x2);
				SAPPENDF(lbuff,"%s",ind[i].idx[2]);
				SAPPENDF(lbuff,"%s",delim_2);
				break;

			default:
				break;
			}
			printpage(ind,fp,i,lbuff);
		}
		else {
			init_prev = ind[i-1].dic[0];
			if (alphabet(init)) {
				if (init[0]!=init_prev[0]) {
					fputs(group_skip,fp);
					if (lethead_flag>0) {
						fprintf(fp,"%s%c%s",lethead_prefix,init[0],lethead_suffix);
					}
					else if (lethead_flag<0) {
						fprintf(fp,"%s%c%s",lethead_prefix,init[0]+32,lethead_suffix);
					}
				}
			}
			else if (japanese(init)) {
				for (j=hpoint;j<(strlen(datama)/2);j++) {
					if (((unsigned char)init[0]<=(unsigned char)datama[j*2])&&((unsigned char)init[1]<(unsigned char)datama[j*2+1])) {
						break;
					}
				}
				if ((j!=hpoint)||(j==0)) {
					hpoint=j;
					fputs(group_skip,fp);
					if (lethead_flag!=0) {
						fputs(lethead_prefix,fp);
						fprint_euc_char(fp,atama[(j-1)*2],atama[(j-1)*2+1]);
						fputs(lethead_suffix,fp);
					}
				}
			}
			else {
				if ( alphabet(init_prev) || japanese(init_prev) ||
				    (!numeric(init_prev)&&numeric(init)) || (numeric(init_prev)&&!numeric(init)) ) {
					if (alphabet(init_prev) || japanese(init_prev) || symbol_flag==2)
						fputs(group_skip,fp);
					if (lethead_flag!=0 && symbol_flag==2 && numeric(init)) {
						fprintf(fp,"%s%s%s",lethead_prefix,numhead,lethead_suffix);
					}
					if (lethead_flag!=0 && (symbol_flag==1 && (alphabet(init_prev)||japanese(init_prev)) ||
								symbol_flag==2 && !numeric(init)) ) {
						fprintf(fp,"%s%s%s",lethead_prefix,symhead,lethead_suffix);
					}
				}
			}

			switch (ind[i].words) {
			case 1:
				SAPPENDF(lbuff,"%s",item_0);
				SAPPENDF(lbuff,"%s",ind[i].idx[0]);
				SAPPENDF(lbuff,"%s",delim_0);
				break;

			case 2:
				if (strcmp(ind[i-1].idx[0],ind[i].idx[0])!=0 || strcmp(ind[i-1].dic[0],ind[i].dic[0])!=0) {
					SAPPENDF(lbuff,"%s",item_0);
					SAPPENDF(lbuff,"%s",ind[i].idx[0]);
					SAPPENDF(lbuff,"%s",item_x1);
				}
				else {
					if (ind[i-1].words==1) {
						SAPPENDF(lbuff,"%s",item_01);
					}
					else {
						SAPPENDF(lbuff,"%s",item_1);
					}
				}
				SAPPENDF(lbuff,"%s",ind[i].idx[1]);
				SAPPENDF(lbuff,"%s",delim_1);
				break;

			case 3:
				if (strcmp(ind[i-1].idx[0],ind[i].idx[0])!=0 || strcmp(ind[i-1].dic[0],ind[i].dic[0])!=0) {
					SAPPENDF(lbuff,"%s",item_0);
					SAPPENDF(lbuff,"%s",ind[i].idx[0]);
					SAPPENDF(lbuff,"%s",item_x1);
					SAPPENDF(lbuff,"%s",ind[i].idx[1]);
					SAPPENDF(lbuff,"%s",item_x2);
				}
				else if (ind[i-1].words==1) {
					SAPPENDF(lbuff,"%s",item_01);
					SAPPENDF(lbuff,"%s",ind[i].idx[1]);
					SAPPENDF(lbuff,"%s",item_x2);
				}
				else if (strcmp(ind[i-1].idx[1],ind[i].idx[1])!=0 || strcmp(ind[i-1].dic[1],ind[i].dic[1])!=0) {
					SAPPENDF(lbuff,"%s",item_1);
					SAPPENDF(lbuff,"%s",ind[i].idx[1]);
					if (ind[i-1].words==2) SAPPENDF(lbuff,"%s",item_12);
					else                   SAPPENDF(lbuff,"%s",item_x2);
				}
				else {
					SAPPENDF(lbuff,"%s",item_2);
				}
				SAPPENDF(lbuff,"%s",ind[i].idx[2]);
				SAPPENDF(lbuff,"%s",delim_2);
				break;

			default:
				break;
			}
			printpage(ind,fp,i,lbuff);
		}
	}
	fputs(postamble,fp);

	if (fp!=stdout) fclose(fp);
}

/*   write page block   */
static void printpage(struct index *ind, FILE *fp, int num, char *lbuff)
{
	int i,j,k,cc;
	char buff[BUFFERLEN],tmpbuff[BUFFERLEN],errbuff[BUFFERLEN];

	buff[0]=tmpbuff[0]='\0';

	crcheck(lbuff,fp);
	line_length=strlen(lbuff);

	for(j=0;j<ind[num].num;j++) {
		cc=range_check(ind[num],j,lbuff);
		if (cc>j) {
			int epage = pnumconv2(&ind[num].p[cc]);
			int bpage = pnumconv2(&ind[num].p[j]);
			if (epage==bpage) {
				j=cc-1;
				continue;
			}
/* range process */
			if (ind[num].p[j].enc[0]==range_open
				|| ind[num].p[j].enc[0]==range_close)
				ind[num].p[j].enc++;
			if (strlen(ind[num].p[j].enc)>0) {
				SPRINTF(buff,"%s%s%s",encap_prefix,ind[num].p[j].enc,encap_infix);
			}
			/* print beginning of range */
			SAPPENDF(buff,"%s",ind[num].p[j].page);
			if (strlen(suffix_3p)>0 && epage-bpage==2) {
				SAPPENDF(buff,"%s",suffix_3p);
			}
			else if (strlen(suffix_mp)>0 && epage-bpage>=2) {
				SAPPENDF(buff,"%s",suffix_mp);
			}
			else if (strlen(suffix_2p)>0 && epage-bpage==1) {
				SAPPENDF(buff,"%s",suffix_2p);
			}
			else {
				SAPPENDF(buff,"%s",delim_r);
				SAPPENDF(buff,"%s",ind[num].p[cc].page);
			}
			SAPPENDF(tmpbuff,"%s",buff);
			buff[0]='\0';
			if (strlen(ind[num].p[j].enc)>0) {
				SAPPENDF(tmpbuff,"%s",encap_suffix);
			}
			linecheck(lbuff,tmpbuff, FALSE);
			j=cc;
			if (j==ind[num].num) {
				goto PRINT;
			}
			else {
				SAPPENDF(tmpbuff,"%s",delim_n);
				linecheck(lbuff,tmpbuff, TRUE);
			}
		}
		else if (strlen(ind[num].p[j].enc)>0) {
/* normal encap */
			if (ind[num].p[j].enc[0]==range_close) {
				SPRINTF(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
				for (i=0;i<ind[num].words;i++) {
					SAPPENDF(errbuff,"%s",ind[num].idx[i]);
					SAPPENDF(errbuff,".");
				}
				warn_printf(efp, "%s\n", errbuff);
				ind[num].p[j].enc++;
			}
			if (strlen(ind[num].p[j].enc)>0) {
				SAPPENDF(tmpbuff,"%s",encap_prefix);
				SAPPENDF(tmpbuff,"%s",ind[num].p[j].enc);
				SAPPENDF(tmpbuff,"%s",encap_infix);
				SAPPENDF(tmpbuff,"%s",ind[num].p[j].page);
				SAPPENDF(tmpbuff,"%s",encap_suffix);
				SAPPENDF(tmpbuff,"%s",delim_n);
				linecheck(lbuff,tmpbuff, FALSE);
			}
			else {
				SAPPENDF(tmpbuff,"%s",ind[num].p[j].page);
				SAPPENDF(tmpbuff,"%s",delim_n);
				linecheck(lbuff,tmpbuff, FALSE);
			}
		}
		else {
/* no encap */
			SAPPENDF(tmpbuff,"%s",ind[num].p[j].page);
			SAPPENDF(tmpbuff,"%s",delim_n);
			linecheck(lbuff,tmpbuff, FALSE);
		}
	}

	if (ind[num].p[j].enc[0]==range_open) {
		SPRINTF(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
		for (k=0;k<ind[num].words;k++) {
			SAPPENDF(errbuff,"%s",ind[num].idx[k]);
			SAPPENDF(errbuff,".");
		}
		warn_printf(efp, "%s\n", errbuff);
		ind[num].p[j].enc++;
	}
	else if (ind[num].p[j].enc[0]==range_close) {
		SPRINTF(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
		for (k=0;k<ind[num].words;k++) {
			SAPPENDF(errbuff,"%s",ind[num].idx[k]);
			SAPPENDF(errbuff,".");
		}
		warn_printf(efp, "%s\n", errbuff);
		ind[num].p[j].enc++;
	}
	if (strlen(ind[num].p[j].enc)>0) {
		SAPPENDF(tmpbuff,"%s",encap_prefix);
		SAPPENDF(tmpbuff,"%s",ind[num].p[j].enc);
		SAPPENDF(tmpbuff,"%s",encap_infix);
		SAPPENDF(tmpbuff,"%s",ind[num].p[j].page);
		SAPPENDF(tmpbuff,"%s",encap_suffix);
	}
	else {
		SAPPENDF(tmpbuff,"%s",ind[num].p[j].page);
	}
	linecheck(lbuff,tmpbuff, FALSE);

PRINT:
	fputs(lbuff,fp);
	fputs(delim_t,fp);
	lbuff[0]='\0';
}

static int range_check(struct index ind, int count, char *lbuff)
{
	int i,j,k,cc1,cc2,start,force=0;
	char tmpbuff[BUFFERLEN],errbuff[BUFFERLEN];

	for (i=count;i<ind.num+1;i++) {
		if (ind.p[i].enc[0]==range_close) {
			SPRINTF(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
			SAPPENDF(errbuff,"%s",ind.idx[0]);
			SAPPENDF(errbuff,".");
			warn_printf(efp, "%s\n", errbuff);
			ind.p[i].enc++;
		}
		if (ind.p[i].enc[0]==range_open) {
			start=i;
			ind.p[i].enc++;
			for (j=i;j<ind.num+1;j++) {
				if (strcmp(ind.p[start].enc,ind.p[j].enc)) {
					if (ind.p[j].enc[0]==range_close) {
						ind.p[j].enc++;
						ind.p[j].enc[0]='\0';
						force=1;
						break;
					}
					else if (j!=i && ind.p[j].enc[0]==range_open) {
						SPRINTF(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
						for (k=0;k<ind.words;k++) {
							SAPPENDF(errbuff,"%s",ind.idx[k]);
							SAPPENDF(errbuff,".");
						}
						warn_printf(efp, "%s\n", errbuff);
						ind.p[j].enc++;
					}
					if (strlen(ind.p[j].enc)>0) {
						SPRINTF(tmpbuff,"%s%s%s%s%s%s",encap_prefix,ind.p[j].enc,encap_infix
						                              ,ind.p[j].page,encap_suffix,delim_n);
						linecheck(lbuff,tmpbuff, FALSE);
					}
				}
			}
			if (j==ind.num+1) {
					SPRINTF(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
					for (k=0;k<ind.words;k++) {
						SAPPENDF(errbuff,"%s",ind.idx[k]);
						SAPPENDF(errbuff,".");
					}
					warn_printf(efp, "%s\n", errbuff);
			}
			i=j-1;
		}
		else if (prange && i<ind.num) {
			if (chkcontinue(ind.p,i)
				&& (!strcmp(ind.p[i].enc,ind.p[i+1].enc)
				|| ind.p[i+1].enc[0]==range_open))
				continue;
			else {
				i++;
				break;
			}
		}
		else {
			i++;
			break;
		}
	}
	cc1=pnumconv2(&ind.p[i-1]);
	cc2=pnumconv2(&ind.p[count]);
	if (cc1>=cc2+2 || (cc1>=cc2+1 && strlen(suffix_2p)) || force) {
		return i-1;
	}
	else return count;
}

/*   check line length   */
static void linecheck(char *lbuff, char *tmpbuff, int force)
{
	if (line_length+strlen(tmpbuff)>line_max && !force) {
		SAPPENDF(lbuff,"\n");
		SAPPENDF(lbuff,"%s",indent_space);
		SAPPENDF(lbuff,"%s",tmpbuff);
		line_length=indent_length+strlen(tmpbuff);
		tmpbuff[0]='\0';
	}
	else {
		SAPPENDF(lbuff,"%s",tmpbuff);
		line_length+=strlen(tmpbuff);
		tmpbuff[0]='\0';
	}
}

static void crcheck(char *lbuff, FILE *fp)
{
	int i;
	char buff[BUFFERLEN];

	for (i=strlen(lbuff);i>=0;i--) {
		if (lbuff[i]=='\n') {
			strncpy(buff,lbuff,i+1);
			buff[i+1]='\0';
			fputs(buff,fp);
			strncpy(buff,&lbuff[i+1],BUFFERLEN-1);
			strcpy(lbuff,buff);
			break;
		}
	}
}
