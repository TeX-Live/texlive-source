#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kpathsea/config.h>
#include <ptexenc/ptexenc.h>
#include "makejvf.h"
#include "uniblock.h"

FILE *vfp,*afp=NULL;
char *atfmname,*vtfmname,*afmname,*vfname,*kanatfm,*jistfm,*ucsqtfm;
int kanatume=-1,chotai=0,baseshift=0,minute=0,useset3=0,hankana=0,fidzero=0;
long ucs=0;

int main(int argc, char ** argv)
{
	int i,j,ib;
	long ch;

	set_enc_string(NULL, "EUC");
	if (argc < 3) {
		usage();
		exit(0);
	}

	argv++;

	for (;**argv=='-';argv++) {
		switch ((*argv)[1]) {
		case 'k':
			if ((*argv)[2]!='\0') {
				kanatume = atoi(&(*argv)[2]);
			}
			else {
				kanatume = atoi(*(++argv));
			}
			break;
		case 'K':
			if ((*argv)[2]!='\0') {
				kanatfm = strdup(&(*argv)[2]);
			}
			else {
				kanatfm = strdup(*(++argv));
			}
			break;
		case 'C':
			chotai=1;
			break;
		case 'a':
			if ((*argv)[2]!='\0') {
				afmname = strdup(&(*argv)[2]);
			}
			else {
				afmname = strdup(*(++argv));
			}
			if ((afp = fopen(afmname,"r"))==NULL) {
				fprintf(stderr,"no AFM file, %s.\n",afmname);
				exit(100);
			}
			break;
		case 'b':
			if ((*argv)[2]!='\0') {
				baseshift = atoi(&(*argv)[2]);
			}
			else {
				baseshift = atoi(*(++argv));
			}
			break;
		case 'm':
			minute=1;
			break;
		case 'u':
			argv++;
			if (!strcmp(*argv,"gb"))
				ucs = ENTRY_G;
			else if (!strcmp(*argv,"cns"))
				ucs = ENTRY_C;
			else if (!strcmp(*argv,"jisq"))
				ucs = ENTRY_JQ;
			else if (!strcmp(*argv,"jis"))
				ucs = ENTRY_J;
			else if (!strcmp(*argv,"ks"))
				ucs = ENTRY_K;
			else {
				fprintf(stderr,"Charset is not set\n");
				ucs = ENTRY_NO;
			}
			break;
		case '3':
			useset3=1;
			break;
		case 'J':
			if ((*argv)[2]!='\0') {
				jistfm = strdup(&(*argv)[2]);
			}
			else {
				jistfm = strdup(*(++argv));
			}
			break;
		case 'U':
			if ((*argv)[2]!='\0') {
				ucsqtfm = strdup(&(*argv)[2]);
			}
			else {
				ucsqtfm = strdup(*(++argv));
			}
			break;
		case 'H':
			hankana=1;
			break;
		case 'i':
			fidzero=1;
			break;
		default:
			usage();
			exit(0);
		}
	}

	if (kanatume>=0 && !afp) {
		fprintf(stderr,"No AFM file for kanatume.\n");
		exit(100);
	}

	atfmname = malloc(strlen(*argv)+4);
	strcpy(atfmname,*argv);

	vfname = malloc(strlen(*argv)+4);
	strcpy(vfname,*argv);
	for (i = strlen(vfname)-1 ; i >= 0 ; i--) {
		if (vfname[i] == '/') {
			vfname = &vfname[i+1];
			break;
		}
	}
	if (!strcmp(&vfname[strlen(vfname)-4],".tfm")) {
		vfname[strlen(vfname)-4] = '\0';
	}
	strcat(vfname,".vf");

	argv++;

	vtfmname = strdup(*argv);
	if (!strcmp(&vtfmname[strlen(vtfmname)-4],".tfm")) {
		vtfmname[strlen(vtfmname)-4] = '\0';
	}

	tfmget(atfmname);

	maketfm(vtfmname);

	if (kanatfm) {
		if (!strcmp(&kanatfm[strlen(kanatfm)-4],".tfm")) {
			kanatfm[strlen(kanatfm)-4] = '\0';
		}
		maketfm(kanatfm);
	}

	if (jistfm) {
		if (!strcmp(&jistfm[strlen(jistfm)-4],".tfm")) {
			jistfm[strlen(jistfm)-4] = '\0';
		}
		maketfm(jistfm);
	}

	if (ucsqtfm) {
		if (!strcmp(&ucsqtfm[strlen(ucsqtfm)-4],".tfm")) {
			ucsqtfm[strlen(ucsqtfm)-4] = '\0';
		}
		maketfm(ucsqtfm);
	}

	vfp = vfopen(vfname);

	if (ucs) {
		ib=0;
		for (i=0;i<(useset3*2+1);i++)
			for (j=0;j<65536;j++) {
				ch=i*65536+j;
				if (search_cjk_entry(&ib,ch,ucs))
					writevfu(ch,vfp);
			}
	} else {
		for (i=0;i<94;i++)
			for (j=0;j<94;j++)
				writevf((0x21+i)*256+(0x21+j),vfp);
	}

	vfclose(vfp);

	exit(0);
}

void usage(void)
{
	fputs2("MAKEJVF ver.1.1a-u1.10 -- make Japanese VF file.\n", stderr);
	fputs2("%% makejvf [<options>] <TFMfile> <PSfontTFM>\n", stderr);
	fputs2("options:\n", stderr);
	fputs2("-C           長体モード\n", stderr);
	fputs2("-K <TFMfile> 非漢字部用に作成するPSフォントTFM名\n", stderr);
	fputs2("-b <数値>    ベースライン補正\n", stderr);
	fputs2("             文字の高さを1000として整数で指定\n", stderr);
	fputs2("             プラスで文字が下がり、マイナスで文字が上がる\n", stderr);
	fputs2("-m           縦書き時にクオート(’”)の代わりにミニュート(′″)を使用\n", stderr);
	fputs2("-a <AFMfile> AFMファイル名（かな詰め時に使用）\n", stderr);
	fputs2("-k <数値>    かな詰めマージン指定\n", stderr);
	fputs2("             文字幅を1000として整数で指定。-aオプションと共に使用\n", stderr);
	fputs2("-u <Charset> UCS mode\n", stderr);
	fputs2("             <Charset> gb : GB,  cns : CNS,  ks : KS\n", stderr);
	fputs2("                       jis : JIS,  jisq : JIS quote only\n", stderr);
	fputs2("-J <TFMfile> JIS encoded PS font TFM name for quote, double quote (with UCS mode)\n", stderr);
	fputs2("-U <TFMfile> UCS encoded PS font TFM name for quote, double quote (with UCS mode)\n", stderr);
	fputs2("-3           use set3 (with UCS mode)\n", stderr);
	fputs2("-H           use half-width katakana (with UCS mode)\n", stderr);
	fputs2("-i           font ID from No.0\n", stderr);
}
