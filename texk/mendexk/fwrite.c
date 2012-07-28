#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <kpathsea/config.h>
#include <kpathsea/tex-file.h>
#include <ptexenc/ptexenc.h>
#include "mendex.h"

#include "exkana.h"
#include "exvar.h"

int line_length=0;

static void printpage(struct index *ind, FILE *fp, int num, char *lbuff);
static int range_check(struct index ind, int count, char *lbuff);
static void linecheck(char *lbuff, char *tmpbuff);
static void crcheck(char *lbuff, FILE *fp);

#define TAIL(x) (x+strlen(x))

/* if we don't have vsnprintf() */
/* #define vsnprintf(buff,len,format,argptr) vsprintf(buff,format,argptr) */

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


/*   write ind file   */
void indwrite(char *filename, struct index *ind, int pagenum)
{
	int i,j,hpoint=0;
	char datama[256],lbuff[4096];
	FILE *fp;

	if (filename[0]!='\0' && kpse_out_name_ok(filename)) fp=fopen(filename,"wb");
	else {
		fp=stdout;
#ifdef WIN32
		setmode(fileno(fp), _O_BINARY);
#endif
	}

	convert(atama,datama);
	fputs(preamble,fp);

	if (fpage>0) {
		fprintf(fp,"%s%d%s",setpage_prefix,pagenum,setpage_suffix);
	}

	for (i=line_length=0;i<lines;i++) {
		if (i==0) {
			if (!((alphabet(ind[i].dic[0][0]))||(japanese(ind[i].dic[0])))) {
				if (lethead_flag) {
					if (symbol_flag && strlen(symbol)) {
						fprintf(fp,"%s%s%s",lethead_prefix,symbol,lethead_suffix);
					}
					else if (lethead_flag>0) {
						fprintf(fp,"%s%s%s",lethead_prefix,symhead_positive,lethead_suffix);
					}
					else if (lethead_flag<0) {
						fprintf(fp,"%s%s%s",lethead_prefix,symhead_negative,lethead_suffix);
					}
				}
				sprintf(lbuff,"%s%s",item_0,ind[i].idx[0]);
			}
			else if (alphabet(ind[i].dic[0][0])) {
				if (lethead_flag>0) {
					fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0],lethead_suffix);
				}
				else if (lethead_flag<0) {
					fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0]+32,lethead_suffix);
				}
				sprintf(lbuff,"%s%s",item_0,ind[i].idx[0]);
			}
			else if (japanese(ind[i].dic[0])) {
				if (lethead_flag) {
					fputs(lethead_prefix,fp);
					for (j=hpoint;j<(strlen(datama)/2);j++) {
						if ((unsigned char)ind[i].dic[0][1]<(unsigned char)datama[j*2+1]) {
							fprintf(fp,"%c%c",atama[(j-1)*2],atama[(j-1)*2+1]);
							hpoint=j;
							break;
						}
					}
					if (j==(strlen(datama)/2)) {
						fprintf(fp,"%c%c",atama[(j-1)*2],atama[(j-1)*2+1]);
					}
					fputs(lethead_suffix,fp);
				}
				sprintf(lbuff,"%s%s",item_0,ind[i].idx[0]);
				for (hpoint=0;hpoint<(strlen(datama)/2);hpoint++) {
					if ((unsigned char)ind[i].dic[0][1]<(unsigned char)datama[hpoint*2+1]) {
						break;
					}
				}
			}
			switch (ind[i].words) {
			case 1:
				sprintf(TAIL(lbuff),"%s",delim_0);
				break;

			case 2:
				sprintf(TAIL(lbuff),"%s%s",item_x1,ind[i].idx[1]);
				sprintf(TAIL(lbuff),"%s",delim_1);
				break;

			case 3:
				sprintf(TAIL(lbuff),"%s%s",item_x1,ind[i].idx[1]);
				sprintf(TAIL(lbuff),"%s%s",item_x2,ind[i].idx[2]);
				sprintf(TAIL(lbuff),"%s",delim_2);
				break;

			default:
				break;
			}
			printpage(ind,fp,i,lbuff);
		}
		else {
			if (!((alphabet(ind[i].dic[0][0]))||(japanese(ind[i].dic[0])))) {
				if ((alphabet(ind[i-1].dic[0][0]))||(japanese(ind[i-1].dic[0]))){
					fputs(group_skip,fp);
					if (lethead_flag && symbol_flag) {
						fprintf(fp,"%s%s%s",lethead_prefix,symbol,lethead_suffix);
					}
				}
			}
			else if (alphabet(ind[i].dic[0][0])) {
				if (ind[i].dic[0][0]!=ind[i-1].dic[0][0]) {
					fputs(group_skip,fp);
					if (lethead_flag>0) {
						fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0],lethead_suffix);
					}
					else if (lethead_flag<0) {
						fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0]+32,lethead_suffix);
					}
				}
			}
			else if (japanese(ind[i].dic[0])) {
				for (j=hpoint;j<(strlen(datama)/2);j++) {
					if ((unsigned char)(ind[i].dic[0][0]<=(unsigned char)datama[j*2])&&((unsigned char)ind[i].dic[0][1]<(unsigned char)datama[j*2+1])) {
						break;
					}
				}
				if ((j!=hpoint)||(j==0)) {
					hpoint=j;
					fputs(group_skip,fp);
					if (lethead_flag!=0) {
						fputs(lethead_prefix,fp);
						fprintf(fp,"%c%c",atama[(j-1)*2],atama[(j-1)*2+1]);
						fputs(lethead_suffix,fp);
					}
				}

				else if ((strncmp(ind[i].dic[0],HIRAEND,2)>=0)&&(strncmp(ind[i-1].dic[0],ind[i].dic[0],2)!=0)) {
					fputs(group_skip,fp);
					if (lethead_flag!=0) {
						fputs(lethead_prefix,fp);
						fprintf(fp,"%c%c",ind[i].dic[0][0],ind[i].dic[0][1]);
						fputs(lethead_suffix,fp);
					}
				}
			}

			switch (ind[i].words) {
			case 1:
				sprintf(TAIL(lbuff),"%s%s%s",item_0,ind[i].idx[0],delim_0);
				break;

			case 2:
				if (strcmp(ind[i-1].idx[0],ind[i].idx[0])!=0 || strcmp(ind[i-1].dic[0],ind[i].dic[0])!=0) {
					sprintf(TAIL(lbuff),"%s%s%s",item_0,ind[i].idx[0],item_x1);
				}
				else {
					if (ind[i-1].words==1) {
						sprintf(TAIL(lbuff),"%s",item_01);
					}
					else {
						sprintf(TAIL(lbuff),"%s",item_1);
					}
				}
				sprintf(TAIL(lbuff),"%s",ind[i].idx[1]);
				sprintf(TAIL(lbuff),"%s",delim_1);
				break;

			case 3:
				if (strcmp(ind[i-1].idx[0],ind[i].idx[0])!=0 || strcmp(ind[i-1].dic[0],ind[i].dic[0])!=0) {
					sprintf(TAIL(lbuff),"%s%s",item_0,ind[i].idx[0]);
					sprintf(TAIL(lbuff),"%s%s%s",item_x1,ind[i].idx[1],item_x2);
				}
				else if (ind[i-1].words==1) {
					sprintf(TAIL(lbuff),"%s%s%s",item_01,ind[i].idx[1],item_x2);
				}
				else if (strcmp(ind[i-1].idx[1],ind[i].idx[1])!=0 || strcmp(ind[i-1].dic[1],ind[i].dic[1])!=0) {
					if (ind[i-1].words==2) sprintf(TAIL(lbuff),"%s%s%s",item_1,ind[i].idx[1],item_12);
					else sprintf(TAIL(lbuff),"%s%s%s",item_1,ind[i].idx[1],item_x2);
				}
				else {
					sprintf(TAIL(lbuff),"%s",item_2);
				}
				sprintf(TAIL(lbuff),"%s%s",ind[i].idx[2],delim_2);
				break;

			default:
				break;
			}
			printpage(ind,fp,i,lbuff);
		}
	}
	fputs(postamble,fp);

	if (filename[0]!='\0') fclose(fp);
}

/*   write page block   */
static void printpage(struct index *ind, FILE *fp, int num, char *lbuff)
{
	int i,j,k,cc;
	char buff[4096],tmpbuff[4096],errbuff[4096];

	buff[0]=tmpbuff[0]='\0';

	crcheck(lbuff,fp);
	line_length=strlen(lbuff);

	for(j=0;j<ind[num].num;j++) {
		cc=range_check(ind[num],j,lbuff);
		if (cc>j) {
			if (pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0])==pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])) {
				j=cc-1;
				continue;
			}
/* range process */
			if (ind[num].p[j].enc[0]==range_open
				|| ind[num].p[j].enc[0]==range_close)
				ind[num].p[j].enc++;
			if (strlen(ind[num].p[j].enc)>0) {
				sprintf(buff,"%s%s%s",encap_prefix,ind[num].p[j].enc,encap_infix);
			}
			if (strlen(suffix_3p)>0 && (pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])-pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0]))==2) {
				sprintf(TAIL(buff),"%s%s",ind[num].p[j].page,suffix_3p);
			}
			else if (strlen(suffix_mp)>0 && (pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])-pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0]))>=2) {
				sprintf(TAIL(buff),"%s%s",ind[num].p[j].page,suffix_mp);
			}
			else if (strlen(suffix_2p)>0 && (pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])-pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0]))==1) {
				sprintf(TAIL(buff),"%s%s",ind[num].p[j].page,suffix_2p);
			}
			else {
				sprintf(TAIL(buff),"%s%s",ind[num].p[j].page,delim_r);
				sprintf(TAIL(buff),"%s",ind[num].p[cc].page);
			}
			sprintf(TAIL(tmpbuff),"%s",buff);
			buff[0]='\0';
			if (strlen(ind[num].p[j].enc)>0) {
				sprintf(TAIL(tmpbuff),"%s",encap_suffix);
			}
			linecheck(lbuff,tmpbuff);
			j=cc;
			if (j==ind[num].num) {
				goto PRINT;
			}
			else {
				sprintf(TAIL(tmpbuff),"%s",delim_n);
				linecheck(lbuff,tmpbuff);
			}
		}
		else if (strlen(ind[num].p[j].enc)>0) {
/* normal encap */
			if (ind[num].p[j].enc[0]==range_close) {
				sprintf(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
				for (i=0;i<ind[num].words;i++) sprintf(TAIL(errbuff),"%s.",ind[num].idx[i]);
				warn_printf(efp, "%s\n", errbuff);
				ind[num].p[j].enc++;
			}
			if (strlen(ind[num].p[j].enc)>0) {
				sprintf(TAIL(tmpbuff),"%s%s%s",encap_prefix,ind[num].p[j].enc,encap_infix);
				sprintf(TAIL(tmpbuff),"%s%s%s",ind[num].p[j].page,encap_suffix,delim_n);
				linecheck(lbuff,tmpbuff);
			}
			else {
				sprintf(TAIL(tmpbuff),"%s%s",ind[num].p[j].page,delim_n);
				linecheck(lbuff,tmpbuff);
			}
		}
		else {
/* no encap */
			sprintf(TAIL(tmpbuff),"%s%s",ind[num].p[j].page,delim_n);
			linecheck(lbuff,tmpbuff);
		}
	}

	if (ind[num].p[j].enc[0]==range_open) {
		sprintf(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
		for (k=0;k<ind[num].words;k++) sprintf(TAIL(errbuff),"%s.",ind[num].idx[k]);
		warn_printf(efp, "%s\n", errbuff);
		ind[num].p[j].enc++;
	}
	else if (ind[num].p[j].enc[0]==range_close) {
		sprintf(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
		for (k=0;k<ind[num].words;k++) sprintf(TAIL(errbuff),"%s.",ind[num].idx[k]);
		warn_printf(efp, "%s\n", errbuff);
		ind[num].p[j].enc++;
	}
	if (strlen(ind[num].p[j].enc)>0) {
		sprintf(TAIL(tmpbuff),"%s%s%s",encap_prefix,ind[num].p[j].enc,encap_infix);
		sprintf(TAIL(tmpbuff),"%s%s",ind[num].p[j].page,encap_suffix);
	}
	else {
		sprintf(TAIL(tmpbuff),"%s",ind[num].p[j].page);
	}
	linecheck(lbuff,tmpbuff);

PRINT:
	fputs(lbuff,fp);
	fputs(delim_t,fp);
	lbuff[0]='\0';
}

static int range_check(struct index ind, int count, char *lbuff)
{
	int i,j,k,cc1,cc2,start,force=0;
	char tmpbuff[4096],errbuff[4096];

	for (i=count;i<ind.num+1;i++) {
		if (ind.p[i].enc[0]==range_close) {
			sprintf(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
			sprintf(TAIL(errbuff),"%s.",ind.idx[0]);
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
						sprintf(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
						for (k=0;k<ind.words;k++) sprintf(TAIL(errbuff),"%s.",ind.idx[k]);
						warn_printf(efp, "%s\n", errbuff);
						ind.p[j].enc++;
					}
					if (strlen(ind.p[j].enc)>0) {
						sprintf(tmpbuff,"%s%s%s",encap_prefix,ind.p[j].enc,encap_infix);
						sprintf(tmpbuff,"%s%s%s",ind.p[j].page,encap_suffix,delim_n);
						linecheck(lbuff,tmpbuff);
					}
				}
			}
			if (j==ind.num+1) {
					sprintf(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
					for (k=0;k<ind.words;k++) sprintf(TAIL(errbuff),"%s.",ind.idx[k]);
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
	cc1=pnumconv(ind.p[i-1].page,ind.p[i-1].attr[0]);
	cc2=pnumconv(ind.p[count].page,ind.p[count].attr[0]);
	if (cc1>=cc2+2 || (cc1>=cc2+1 && strlen(suffix_2p)) || force) {
		return i-1;
	}
	else return count;
}

/*   check line length   */
static void linecheck(char *lbuff, char *tmpbuff)
{
	if (line_length+strlen(tmpbuff)>line_max) {
		sprintf(TAIL(lbuff),"\n%s%s",indent_space,tmpbuff);
		line_length=indent_length+strlen(tmpbuff);
		tmpbuff[0]='\0';
	}
	else {
		sprintf(TAIL(lbuff),"%s",tmpbuff);
		line_length+=strlen(tmpbuff);
		tmpbuff[0]='\0';
	}
}

static void crcheck(char *lbuff, FILE *fp)
{
	int i;
	char buff[4096];

	for (i=strlen(lbuff);i>=0;i--) {
		if (lbuff[i]=='\n') {
			strncpy(buff,lbuff,i+1);
			buff[i+1]='\0';
			fputs(buff,fp);
			strcpy(buff,&lbuff[i+1]);
			strcpy(lbuff,buff);
			break;
		}
	}
}
