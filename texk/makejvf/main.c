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
int kanatume=-1,chotai=0,baseshift=0,minute=0,useset3=0,hankana=0,fidzero=0,enhanced=0,omitzw=0;
int pstfm_nt;
long ucs=0;

int main(int argc, char ** argv)
{
	int i,j;
	int c;
	long ch,ch_max;
	const char *atfmname_base;

	kpse_set_program_name(argv[0], "makejvf");
	set_enc_string("sjis", "euc");

	while ((c = getopt (argc, argv, "k:K:Ca:b:mu:3J:U:Hiet:O")) != -1)
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
		case 'O':
			omitzw=1;
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
		exit(101);
	}

	if (argc - optind != 2) {
		usage();
		exit(0);
	}

	atfmname = xstrdup(argv[optind]);
	if (FILESTRCASEEQ(&atfmname[strlen(atfmname)-4], ".tfm")) {
		atfmname[strlen(atfmname)-4] = '\0';
	}

	atfmname_base = xbasename(atfmname);
	vfname = xmalloc(strlen(atfmname_base)+4);
	strcpy(vfname, atfmname_base);
	strcat(vfname,".vf");

	vtfmname = xstrdup(argv[optind+1]);
	if (FILESTRCASEEQ(&vtfmname[strlen(vtfmname)-4], ".tfm")) {
		vtfmname[strlen(vtfmname)-4] = '\0';
	}
	if (FILESTRCASEEQ(&vtfmname[0], &atfmname_base[0])) {
		fprintf(stderr,"Invalid usage: input TFM and output TFM must be different.\n");
		exit(102);
	}

	if (kanatfm) {
		if (FILESTRCASEEQ(&kanatfm[strlen(kanatfm)-4], ".tfm")) {
			kanatfm[strlen(kanatfm)-4] = '\0';
		}
		if (FILESTRCASEEQ(&kanatfm[0], &atfmname_base[0])) {
			fprintf(stderr,"Invalid usage: input TFM and output TFM must be different.\n");
			exit(102);
		}
	}

	if (!ucs) {
		if (jistfm) {
			fprintf(stderr,"[Warning] Option -J invalid in non-UCS mode, ignored.\n");
			jistfm = NULL;
		}
		if (ucsqtfm) {
			fprintf(stderr,"[Warning] Option -U invalid in non-UCS mode, ignored.\n");
			ucsqtfm = NULL;
		}
		if (useset3) {
			fprintf(stderr,"[Warning] Option -3 invalid in non-UCS mode, ignored.\n");
			useset3 = 0;
		}
		if (hankana) {
			fprintf(stderr,"[Warning] Option -H invalid in non-UCS mode, ignored.\n");
			hankana = 0;
		}
	}

	if (jistfm && ucsqtfm) {
		fprintf(stderr,"Options -J and -U at the same time? I'm confused.\n");
		exit(110);
	}

	if (jistfm) {
		if (FILESTRCASEEQ(&jistfm[strlen(jistfm)-4], ".tfm")) {
			jistfm[strlen(jistfm)-4] = '\0';
		}
		if (FILESTRCASEEQ(&jistfm[0], &atfmname_base[0])) {
			fprintf(stderr,"Invalid usage: input TFM and output TFM must be different.\n");
			exit(102);
		}
	}

	if (ucsqtfm) {
		if (FILESTRCASEEQ(&ucsqtfm[strlen(ucsqtfm)-4], ".tfm")) {
			ucsqtfm[strlen(ucsqtfm)-4] = '\0';
		}
		if (FILESTRCASEEQ(&ucsqtfm[0], &atfmname_base[0])) {
			fprintf(stderr,"Invalid usage: input TFM and output TFM must be different.\n");
			exit(102);
		}
	}

	if (omitzw && usertable) {
		fprintf(stderr,"Invalid usage: conflict options -O and -t.\n");
		exit(120);
	}
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
		exit(130);
	}

	tfmget(atfmname);

	vfp = vfopen(vfname);

	pstfm_nt=1; /* initialize */
	if (ucs) {
		if (ucs==ENTRY_CUSTOM) ch_max=usertable_charset[usertable_charset_max-1].max;
		else if (useset3) ch_max=0x3FFFF;
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
		maketfm(kanatfm);
		pstfm_nt=1; /* already done*/
	}

	maketfm(vtfmname);
	pstfm_nt=1; /* already done*/

	if (jistfm) maketfm(jistfm);

	if (ucsqtfm) maketfm(ucsqtfm);

	exit(0);
}

void usage(void)
{
	fprintf(stderr, "MAKEJVF version %s -- make Japanese VF from a JFM file.\n", VERSION);
	fputs2("Usage:\n", stderr);
	fputs2("%% makejvf [<options>] <TFMfile> <PSfontTFM>\n", stderr);
	fputs2("  <TFMfile>:   Name of input pTeX/upTeX JFM file.\n", stderr);
	fputs2("               The basename is inherited by the name of output VF file.\n", stderr);
	fputs2("  <PSfontTFM>: Name of output PSfont JFM file.\n", stderr);
	fputs2("Options:\n", stderr);
	fputs2("-C           長体モード\n", stderr);
	fputs2("-K <PS-TFM>  非漢字部用に作成するPSフォントTFM名\n", stderr);
	fputs2("-b <数値>    ベースライン補正\n", stderr);
	fputs2("             文字の高さを1000として整数で指定\n", stderr);
	fputs2("             プラスで文字が下がり、マイナスで文字が上がる\n", stderr);
	fputs2("-m           縦書き時にクオート(’”)の代わりにミニュート(′″)を使用\n", stderr);
	fputs2("-a <AFMfile> AFMファイル名（かな詰め時に使用）\n", stderr);
	fputs2("-k <数値>    かな詰めマージン指定\n", stderr);
	fputs2("             文字幅を1000として整数で指定。-aオプションと共に使用\n", stderr);
	fputs2("-i           Start mapped font ID from No. 0\n", stderr);
	fputs2("-e           Enhanced mode; the horizontal shift amount is determined\n", stderr);
	fputs2("             from the glue/kern table of <TFMfile> input\n", stderr);
	fputs2("-t <CNFfile> Use <CNFfile> as a configuration file\n", stderr);
	fputs2("-O           Omit entries in VF for characters with default metric\n", stderr);
	fputs2("-u <Charset> UCS mode\n", stderr);
	fputs2("             <Charset> gb : GB,  cns : CNS,  ks : KS\n", stderr);
	fputs2("                       jis : JIS,  jisq : JIS quote only\n", stderr);
	fputs2("                       custom : Use user-defined CHARSET from <CNFfile>\n", stderr);
	fputs2("Options below are effective only in UCS mode:\n", stderr);
	fputs2("-J <PS-TFM>  Map single/double quote to another JIS-encoded PSfont TFM\n", stderr);
	fputs2("-U <PS-TFM>  Map single/double quote to another UCS-encoded PSfont TFM\n", stderr);
	fputs2("-3           Use set3, that is, enable non-BMP characters support\n", stderr);
	fputs2("-H           Use half-width katakana\n", stderr);
	fprintf(stderr, "Email bug reports to %s.\n", BUG_ADDRESS);
}
