#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kpathsea/kpathsea.h>
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
	int c;
	long ch;

	kpse_set_program_name(argv[0], "makejvf");
	set_enc_string("sjis", "euc");

	while ((c = getopt (argc, argv, "k:K:Ca:b:mu:3J:U:Hi")) != -1)
		switch (c) {


		case 'k':
			kanatume = atoi(optarg);
			break;
		case 'K':
			kanatfm = xstrdup(optarg);
			break;
		case 'C':
			chotai=1;
			break;
		case 'a':
			afmname = xstrdup(optarg);
			{
				char *fpath = kpse_find_file(afmname, kpse_afm_format, 0);
				if (fpath) {
					afp = fopen(fpath, "r");
					free(fpath);
					}
			}
			if (!afp) {
				fprintf(stderr,"no AFM file, %s.\n",afmname);
				exit(100);
			}
			break;
		case 'b':
			baseshift = atoi(optarg);
			break;
		case 'm':
			minute=1;
			break;
		case 'u':
			if (!strcmp(optarg, "gb"))
				ucs = ENTRY_G;
			else if (!strcmp(optarg, "cns"))
				ucs = ENTRY_C;
			else if (!strcmp(optarg, "jisq"))
				ucs = ENTRY_JQ;
			else if (!strcmp(optarg, "jis"))
				ucs = ENTRY_J;
			else if (!strcmp(optarg, "ks"))
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
			jistfm = xstrdup(optarg);
			break;
		case 'U':
			ucsqtfm = xstrdup(optarg);
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

	if (kanatume>=0 && !afp) {
		fprintf(stderr,"No AFM file for kanatume.\n");
		exit(100);
	}

	if (argc - optind != 2) {
		usage();
		exit(0);
	}

	atfmname = xmalloc(strlen(argv[optind])+4);
	strcpy(atfmname, argv[optind]);

	{
		const char *p = xbasename(argv[optind]);
		vfname = xmalloc(strlen(p)+4);
		strcpy(vfname, p);
	}
	if (FILESTRCASEEQ(&vfname[strlen(vfname)-4], ".tfm")) {
		vfname[strlen(vfname)-4] = '\0';
	}
	strcat(vfname,".vf");

	vtfmname = xstrdup(argv[optind+1]);
	if (FILESTRCASEEQ(&vtfmname[strlen(vtfmname)-4], ".tfm")) {
		vtfmname[strlen(vtfmname)-4] = '\0';
	}

	tfmget(atfmname);

	maketfm(vtfmname);

	if (kanatfm) {
		if (FILESTRCASEEQ(&kanatfm[strlen(kanatfm)-4], ".tfm")) {
			kanatfm[strlen(kanatfm)-4] = '\0';
		}
		maketfm(kanatfm);
	}

	if (jistfm) {
		if (FILESTRCASEEQ(&jistfm[strlen(jistfm)-4], ".tfm")) {
			jistfm[strlen(jistfm)-4] = '\0';
		}
		maketfm(jistfm);
	}

	if (ucsqtfm) {
		if (FILESTRCASEEQ(&ucsqtfm[strlen(ucsqtfm)-4], ".tfm")) {
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
