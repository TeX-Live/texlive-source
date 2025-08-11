#include "mendex.h"

#include <kpathsea/tex-file.h>
#include <kpathsea/variable.h>
#include <ptexenc/ptexenc.h>
#include <ptexenc/unicode.h>
#include <ptexenc/unicode-jp.h>

#include "qsort.h"

#include "exkana.h"
#include "ktable.h"
#include "exvar.h"

#include "kp.h"

#define BUFFERLEN 4096

struct dictionary{
char* dic[2];
};

static struct dictionary *dictable,*envdic;
static int dlines=0,elines=0;

static int dicvalread(const char *filename, struct dictionary *dicval, int line);

#define is_katakana(a)  ((strncmp((a),KATATOP,2)>=0) && (strncmp((a),KATAEND,2)<=0))
#define is_hiragana(a)  ((strncmp((a),HIRATOP,2)>=0) && (strncmp((a),HIRAEND,2)<=0))
#define is_onbiki(a)    (strncmp((a),ONBIKI,2)==0)
#define is_alpha_numeric(a)  ((strncmp((a),SPACE,2)>=0)&&(strncmp((a),ALPHAEND,2)<=0))

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
	char buff[BUFFERLEN];
	FILE *fp;

	if (filename!=NULL) {
		filename = KP_find_dict_file(filename);
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
			if (mfgets(buff,BUFFERLEN-1,fp)==NULL) break;
			if ((buff[0]=='\r')||(buff[0]=='\n')||(buff[0]=='\0')) i--;
		}
		nkf_close(fp);

		dictable=xmalloc(sizeof(struct dictionary)*i);

		dlines=dicvalread(filename,dictable,i);

		verb_printf(efp,"...done.\n");
	}

ENV:
	envfile=kpse_var_value("INDEXDEFAULTDICTIONARY");
	if ((envfile!=NULL)&&(strlen(envfile)!=0)) {
		envfile = KP_find_dict_file(envfile);
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
			if ((buff[0]=='\r')||(buff[0]=='\n')||(buff[0]=='\0')) i--;
		}
		nkf_close(fp);

		envdic=xmalloc(sizeof(struct dictionary)*i);

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
	char buff[256],buff2[256];
	FILE *fp;

	if(kpse_in_name_ok(filename))
		fp=nkf_open(filename,"rb");
	else {
		fprintf(stderr, "mendex: %s is forbidden to open for reading.\n",filename);
		exit(255);
	}
	for (i=0;i<line;i++) {
		if (mfgets(buff,255,fp)==NULL) break;
		if ((buff[0]=='\r')||(buff[0]=='\n')||(buff[0]=='\0')) {
			i--;
			continue;
		}
		for (j=0;((buff[j]==' ')||(buff[j]=='\t'));j++);
		for (k=0;((buff[j]!='\r')&&(buff[j]!='\n')&&(buff[j]!=' ')&&(buff[j]!='\t'));j++,k++) {
			buff2[k]=buff[j];
		}
		buff2[k]='\0';
		if (strlen(buff2)==0) {
			i--;
			continue;
		}
		dicval[i].dic[0]=xstrdup(buff2);
		for (;((buff[j]==' ')||(buff[j]=='\t'));j++);
		for (k=0;((buff[j]!='\r')&&(buff[j]!='\n')&&(buff[j]!=' ')&&(buff[j]!='\t'));j++,k++) {
			buff2[k]=buff[j];
		}
		buff2[k]='\0';
		if (strlen(buff2)==0) {
			free(dicval[i].dic[0]);
			i--;
			continue;
		}
		dicval[i].dic[1]=xstrdup(buff2);

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

	for (i=0;i<256;i++) {
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
	char errbuff[BUFFERLEN];
	int chr,wclen;
	char buff3[3];

	while(1) {
		if (buff1[i]=='\0') {
			buff2[j]='\0';
			break;
		}
		else {
			if ((unsigned char)buff1[i]<0x80)
				wclen=1;
			else if (is_internalUPTEX()) {  /* convert a character from UTF8 to EUC */
				wclen=multibytelen((unsigned char)buff1[i]);
				if (wclen<0) {
					verb_printf(efp,"\nWarning: Illegal lead byte 0x%x in UTF-8.", (unsigned char)buff1[i]);
					i++;
					continue;
				}
				chr = UCS2toJIS(UTF8StoUCS((unsigned char*)&buff1[i]));
				if (chr==0) chr = 0xffff; /* conversion failed */
				chr |= 0x8080;
				buff3[0]=BYTE3(chr);
				buff3[1]=BYTE4(chr);
				buff3[2]='\0';
			} else {
				wclen=2;
				buff3[0]=buff1[i];
				buff3[1]=buff1[i+1];
				buff3[2]='\0';
			}

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

			else if (is_katakana(buff3)) {
/*   katakana   */
				for (k=0;k<strlen(katakana);k+=2) {
					if (strncmp(buff3,&katakana[k],2)==0) {
						strncpy(&buff2[j],&kanatable[k],2);
						goto MATCH1;
					}
				}
				sprintf(errbuff,"\nError: %s is bad katakana ",buff3);
				fputs(errbuff,efp);
				if (efp!=stderr) fputs(errbuff,stderr);
				return -1;
MATCH1:
				i+=wclen;
				j+=2;
			}

			else if (is_hiragana(buff3)) {
/*   hiragana   */
				for (k=0;k<strlen(hiragana);k+=2) {
					if (strncmp(buff3,&hiragana[k],2)==0) {
						strncpy(&buff2[j],&kanatable[k],2);
						goto MATCH2;
					}
				}
				sprintf(errbuff,"\nError: %s is bad hiragana ",buff3);
				fputs(errbuff,efp);
				if (efp!=stderr) fputs(errbuff,stderr);
				return -1;
MATCH2:
				i+=wclen;
				j+=2;
			}

			else if (is_onbiki(buff3)) {
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
				i+=wclen;
				j+=2;
			}

			else if ((unsigned char)buff1[i]>=0x80) {
				if (is_alpha_numeric(buff3)) {
/*   alpha-numeric,symbols   */
					for (k=0;k<strlen(symboltable);k+=2) {
						if (strncmp(buff3,&symboltable[k],2)==0) {
							buff2[j]=k/2+0x20;
							if ((buff2[j]>='a')&&(buff2[j]<='z')) buff2[j]-=32;
							i+=wclen;
							j++;
							break;
						}
					}
					if (k==strlen(symboltable)) {
						i+=wclen;
						buff2[j++]=buff3[0];
						buff2[j++]=buff3[1];
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
							i+=wclen;
							buff2[j++]=buff3[0];
							buff2[j++]=buff3[1];
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

	if (attr<0) return 0;  /* inappropriate page type */
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
