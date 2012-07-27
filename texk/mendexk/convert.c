#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kpathsea/config.h>
#include <kpathsea/lib.h>
#include <kpathsea/tex-file.h>
#include <ptexenc/ptexenc.h>
#include "mendex.h"

#include "exkana.h"
#include "ktable.h"
#include "exvar.h"

#ifdef KPATHSEA
#include "kp.h"
#endif

struct dictionary{
char dic[2][50];
};

static struct dictionary *dictable,*envdic;
static int dlines=0,elines=0;

static int dicvalread(const char *filename, struct dictionary *dicval, int line);

/*   initialize kana table   */
void initkanatable(void)
{
	int i,cc;

	for (i=0xa4a1;i<=0xa4f3;i++) {
		cc=i-0xa4a1;
		hiragana[cc*2]=((i>>8)&0xff);
		hiragana[cc*2+1]=(i&0xff);
	}
	hiragana[(i-0xa4a1)*2]=0;

	for (i=0xa5a1;i<=0xa5f6;i++) {
		cc=i-0xa5a1;
		katakana[cc*2]=((i>>8)&0xff);
		katakana[cc*2+1]=(i&0xff);
	}
	katakana[(i-0xa5a1)*2]=0;

	akasatana=xstrdup(AKASATANA);
	for (i=0;;i++) {
		if (akasatana[i*2]==0) break;
		akasatana[i*2]++;
	}

	aiueo=xstrdup(AIUEO);
	for (i=0;;i++) {
		if (aiueo[i*2]==0) break;
		aiueo[i*2]++;
	}

	strcpy(atama,akasatana);
}

/*   get dictionary   */
int dicread(const char *filename)
{
	int i,ecount=0;
	const char *envfile;
	char buff[4096];
	FILE *fp;

	if (filename!=NULL) {
#ifdef KPATHSEA
		filename = KP_find_file(&kp_dict,filename);
#endif
		if(kpse_in_name_ok(filename))
			fp=nkf_open(filename,"rb");
		else
			fp = NULL;
		if (fp==NULL) {
			warn_printf(efp,"Warning: Couldn't find dictionary file %s.\n",filename);
			goto ENV;
		}
		verb_printf(efp,"Scanning dictionary file %s.",filename);

		for (i=0;;i++) {
			if (mfgets(buff,4095,fp)==NULL) break;
			if ((buff[0]=='\n')||(buff[0]=='\0')) i--;
		}
		nkf_close(fp);

		dictable=malloc(sizeof(struct dictionary)*i);

		dlines=dicvalread(filename,dictable,i);

		verb_printf(efp,"...done.\n");
	}

ENV:
#ifdef KPATHSEA
	envfile=KP_get_value("INDEXDEFAULTDICTIONARY",NULL);
#else
	envfile=getenv("INDEXDEFAULTDICTIONARY");
#endif
	if ((envfile!=NULL)&&(strlen(envfile)!=0)) {
#ifdef KPATHSEA
		envfile = KP_find_file(&kp_dict,envfile);
#endif
		if(kpse_in_name_ok(envfile))
			fp=nkf_open(envfile,"rb");
		else
			fp = NULL;
		if (fp==NULL) {
			warn_printf(efp,"Warning: Couldn't find environment dictionary file %s.\n",envfile);
			return ecount;
		}
		verb_printf(efp,"Scanning environment dictionary file %s.",envfile);

		for (i=0;;i++) {
			if (mfgets(buff,255,fp)==NULL) break;
			if ((buff[0]=='\n')||(buff[0]=='\0')) i--;
		}
		nkf_close(fp);

		envdic=malloc(sizeof(struct dictionary)*i);

		elines=dicvalread(envfile,envdic,i);

		verb_printf(efp,"...done.\n");
	}

	return 0; /* FIXME: is this right? */	
}

static int dcomp(const void *bf1, const void *bf2);

/*   read dictionary file   */
static int dicvalread(const char *filename, struct dictionary *dicval, int line)
{
	int i,j,k;
	char buff[256];
	FILE *fp;

	if(kpse_in_name_ok(filename))
		fp=nkf_open(filename,"rb");
	else {
		fprintf(stderr, "mendex: %s is forbidden to open for reading.\n",filename);
		exit(255);
	}
	for (i=0;i<line;i++) {
		if (mfgets(buff,255,fp)==NULL) break;
		if ((buff[0]=='\n')||(buff[0]=='\0')) {
			i--;
			continue;
		}
		for (j=0;((buff[j]==' ')||(buff[j]=='\t'));j++);
		for (k=0;((buff[j]!='\n')&&(buff[j]!=' ')&&(buff[j]!='\t'));j++,k++) {
			dicval[i].dic[0][k]=buff[j];
		}
		dicval[i].dic[0][k]='\0';
		if (strlen(dicval[i].dic[0])==0) {
			i--;
			continue;
		}
		for (;((buff[j]==' ')||(buff[j]=='\t'));j++);
		for (k=0;((buff[j]!='\n')&&(buff[j]!=' ')&&(buff[j]!='\t'));j++,k++) {
			dicval[i].dic[1][k]=buff[j];
		}
		dicval[i].dic[1][k]='\0';
		if (strlen(dicval[i].dic[1])==0) {
			i--;
			continue;
		}

		convert(dicval[i].dic[1],buff);
		strcpy(dicval[i].dic[1],buff);
	}

	nkf_close(fp);

	qsort(dicval,i,sizeof(struct dictionary),dcomp);
	return(i);
}

/*   comp-function of dictionary sorting   */
static int dcomp(const void *bf1, const void *bf2)
{
	const struct dictionary *buff1 = (const struct dictionary *) bf1;
	const struct dictionary *buff2 = (const struct dictionary *) bf2;
	int i;

	for (i=0;i<50;i++) {
		if (((*buff1).dic[0][i]=='\0')&&((*buff2).dic[0][i]=='\0')) return 0;
		else if (((*buff1).dic[0][i]=='\0')&&((*buff2).dic[0][i]!='\0')) return 1;
		else if (((*buff1).dic[0][i]!='\0')&&((*buff2).dic[0][i]=='\0')) return -1;
		else if ((unsigned char)(*buff1).dic[0][i]<(unsigned char)(*buff2).dic[0][i]) return 1;
		else if ((unsigned char)(*buff1).dic[0][i]>(unsigned char)(*buff2).dic[0][i]) return -1;
	}
	return 0;
}

/*   convert to capital-hiragana character   */
int convert(char *buff1, char *buff2)
{
	int i=0,j=0,k,l;
	char errbuff[4096];

	while(1) {
		if (buff1[i]=='\0') {
			buff2[j]='\0';
			break;
		}
		else {
			if ((buff1[i]>='a')&&(buff1[i]<='z')) {
				buff2[j]=buff1[i]-32;
				i++;
				j++;
			}

			else if ((lorder==1)&&((buff1[i]==' ')||(buff1[i]=='\t'))) {
				i++;
			}

			else if ((unsigned char)buff1[i]<0x80) {
				buff2[j]=buff1[i];
				i++;
				j++;
			}

			else if ((strncmp(&buff1[i],KATATOP,2)>=0)&&(strncmp(&buff1[i],KATAEND,2)<=0)) {
/*   katakana   */
				for (k=0;k<strlen(katakana);k+=2) {
					if (strncmp(&buff1[i],&katakana[k],2)==0) {
						strncpy(&buff2[j],&kanatable[k],2);
						goto MATCH1;
					}
				}
				sprintf(errbuff,"\nError: %s is bad katakana ",&buff1[i]);
				fputs(errbuff,efp);
				if (efp!=stderr) fputs(errbuff,stderr);
				return -1;
MATCH1:
				i+=2;
				j+=2;
			}

			else if ((strncmp(&buff1[i],HIRATOP,2)>=0)&&(strncmp(&buff1[i],HIRAEND,2)<=0)) {
/*   hiragana   */
				for (k=0;k<strlen(hiragana);k+=2) {
					if (strncmp(&buff1[i],&hiragana[k],2)==0) {
						strncpy(&buff2[j],&kanatable[k],2);
						goto MATCH2;
					}
				}
				sprintf(errbuff,"\nError: %s is bad hiragana ",&buff1[i]);
				fputs(errbuff,efp);
				if (efp!=stderr) fputs(errbuff,stderr);
				return -1;
MATCH2:
				i+=2;
				j+=2;
			}

			else if (strncmp(&buff1[i],ONBIKI,2)==0) {
/*   onbiki   */
				if (j>=2) {
					for (k=0;k<20;k+=2) {
						for (l=0;l<6;l++) {
							if (strncmp(&buff2[j-2],&btable[l][k],2)==0) {
								strncpy(&buff2[j],&btable[l][0],2);
								goto MATCH3;
							}
						}
					}
				}
				sprintf(errbuff,"\nError: %s is Illegal line ",buff1);
				fputs(errbuff,efp);
				if (efp!=stderr) fputs(errbuff,stderr);
				return -1;
MATCH3:
				i+=2;
				j+=2;
			}

			else if ((unsigned char)buff1[i]>=0x80) {
				if ((strncmp(&buff1[i],SPACE,2)>=0)&&(strncmp(&buff1[i],ALPHAEND,2)<=0)) {
/*   alpha-numeric,symbols   */
					for (k=0;k<strlen(symboltable);k+=2) {
						if (strncmp(&buff1[i],&symboltable[k],2)==0) {
							buff2[j]=k/2+0x20;
							if ((buff2[j]>='a')&&(buff2[j]<='z')) buff2[j]-=32;
							j++;
							i+=2;
							break;
						}
					}
					if (k==strlen(symboltable)) {
						buff2[j++]=buff1[i++];
						buff2[j++]=buff1[i++];
					}
				}

				else {
					for (k=0;k<dlines;k++) {
/*   dictionary table   */
						if (strncmp(dictable[k].dic[0],&buff1[i],strlen(dictable[k].dic[0]))==0) {
							strncpy(&buff2[j],dictable[k].dic[1],strlen(dictable[k].dic[1])); 
							i+=strlen(dictable[k].dic[0]);
							j+=strlen(dictable[k].dic[1]);
							break;
						}
					}
					if ((k==dlines)&&(elines!=0)) {
/*   environment dictionary table   */
						for (k=0;k<elines;k++) {
							if (strncmp(envdic[k].dic[0],&buff1[i],strlen(envdic[k].dic[0]))==0) {
								strncpy(&buff2[j],envdic[k].dic[1],strlen(envdic[k].dic[1])); 
								i+=strlen(envdic[k].dic[0]);
								j+=strlen(envdic[k].dic[1]);
								break;
							}
						}
					}
					if (((k==dlines)&&(elines==0))||((k==elines)&&(elines!=0))) {
						if (force==1) {
/*   forced convert   */
							buff2[j++]=buff1[i++];
							buff2[j++]=buff1[i++];
						}
						else {
							sprintf(errbuff,"\nError: %s is no entry in dictionary file ",&buff1[i]);
							fputs(errbuff,efp);
							if (efp!=stderr) fputs(errbuff,stderr);
							return -1;
						}
					}
				}
			}
		}
	}
	return 0;
}

int pnumconv(char *page, int attr)
{
	int i,cc=0;

	switch (page_precedence[attr]) {
	case 'a':
		cc=page[0]-'a'+1;
		break;

	case 'A':
		cc=page[0]-'A'+1;
		break;

	case 'n':
		cc=atoi(page);
		break;

	case 'r':
	case 'R':
		for (i=0;i<strlen(page);i++) {
			switch (page[i]) {
			case 'i':
			case 'I':
				if (i==0) cc=1;
				else cc++;
				break;

			case 'v':
			case 'V':
				if (i==0) cc=5;
				else {
					switch (page[i-1]) {
					case 'i':
					case 'I':
						cc+=3;
						break;

					case 'x':
					case 'X':
					case 'l':
					case 'L':
					case 'c':
					case 'C':
					case 'd':
					case 'D':
					case 'm':
					case 'M':
						cc+=5;
						break;

					default:
						break;
					}
				}
				break;

			case 'x':
			case 'X':
				if (i==0) cc=10;
				else {
					switch (page[i-1]) {
					case 'i':
					case 'I':
						cc+=8;
						break;

					case 'x':
					case 'X':
					case 'l':
					case 'L':
					case 'c':
					case 'C':
					case 'd':
					case 'D':
					case 'm':
					case 'M':
						cc+=10;
						break;

					default:
						break;
					}
				}
				break;

			case 'l':
			case 'L':
				if (i==0) cc=50;
				else {
					switch (page[i-1]) {
					case 'x':
					case 'X':
						cc+=30;
						break;

					case 'c':
					case 'C':
					case 'd':
					case 'D':
					case 'm':
					case 'M':
						cc+=50;
						break;

					default:
						break;
					}
				}
				break;

			case 'c':
			case 'C':
				if (i==0) cc=100;
					switch (page[i-1]) {
					case 'x':
					case 'X':
						cc+=80;
						break;

					case 'c':
					case 'C':
					case 'd':
					case 'D':
					case 'm':
					case 'M':
						cc+=100;
						break;

					default:
						break;
					}
				break;

			case 'd':
			case 'D':
				if (i==0) cc=500;
				else {
					switch (page[i-1]) {
					case 'c':
					case 'C':
						cc+=300;
						break;

					case 'm':
					case 'M':
						cc+=500;
						break;

					default:
						break;
					}
				}
				break;

			case 'm':
			case 'M':
				if (i==0) cc=1000;
					switch (page[i-1]) {
					case 'c':
					case 'C':
						cc+=800;
						break;

					case 'm':
					case 'M':
						cc+=1000;
						break;

					default:
						break;
					}
				break;

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return cc;
}
