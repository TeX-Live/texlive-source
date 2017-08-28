#include <kpathsea/kpathsea.h>
#include <ptexenc/ptexenc.h>
#include "version.h"
#include "makejvf.h"
#include "uniblock.h"
#include "usrtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *vfp,*afp=NULL;
char *atfmname,*vtfmname,*afmname,*vfname,*kanatfm,*jistfm,*ucsqtfm,*usertable;
int kanatume=-1,chotai=0,baseshift=0,minute=0,useset3=0,hankana=0,fidzero=0,enhanced=0;
int pstfm_nt;
long ucs=0;

int main(int argc, char ** argv)
{
	int i,j;
	int c;
	long ch,ch_max;

	kpse_set_program_name(argv[0], "makejvf");
	set_enc_string("sjis", "euc");

	while ((c = getopt (argc, argv, "k:K:Ca:b:mu:3J:U:Hiet:")) != -1)
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
			else if (!strcmp(optarg, "custom"))
				ucs = ENTRY_CUSTOM;
			else {
				fprintf(stderr,"[Warning] Charset is not set.\n");
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
		case 'e':
			enhanced=1;
			break;
		case 't':
			usertable = xstrdup(optarg);
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

	if (usertable) {
		get_usertable(usertable);
	}
	if (ucs!=ENTRY_CUSTOM && usertable_charset_max>0) {
		fprintf(stderr,
			"[Warning] Custom charset is defined in usertable\n"
			"[Warning]   but it will be ignored.\n");
	}
	if (ucs==ENTRY_CUSTOM && usertable_charset_max<1) {
		fprintf(stderr,"No custom charset definition in usertable.\n");
		exit(101);
	}

	vfp = vfopen(vfname);

	pstfm_nt=1; /* initialize */
	if (ucs) {
		if (ucs==ENTRY_CUSTOM) ch_max=usertable_charset[usertable_charset_max-1].max;
		else if (useset3) ch_max=0x2FFFF;
		else ch_max=0xFFFF;
		for (ch=0;ch<=ch_max;ch++) {
			if (search_cjk_entry(ch,ucs))
				writevfu(ch,vfp);
		}
	} else {
		for (i=0;i<94;i++)
			for (j=0;j<94;j++)
				writevf((0x21+i)*256+(0x21+j),vfp);
	}

	vfclose(vfp);

	if (kanatfm) {
		if (FILESTRCASEEQ(&kanatfm[strlen(kanatfm)-4], ".tfm")) {
			kanatfm[strlen(kanatfm)-4] = '\0';
		}
		maketfm(kanatfm);
		pstfm_nt=1; /* already done*/
	}

	maketfm(vtfmname);
	pstfm_nt=1; /* already done*/

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

	exit(0);
}

void usage(void)
{
	fprintf(stderr, "MAKEJVF version %s -- make Japanese VF file.\n", VERSION);
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
	fputs2("-e           enhanced mode; the horizontal shift amount is determined\n", stderr);
	fputs2("             from the glue/kern table of <TFMfile> input\n", stderr);
	fputs2("-t <CNFfile> use <CNFfile> as a configuration file\n", stderr);
	fprintf(stderr, "Email bug reports to %s.\n", BUG_ADDRESS);
}
