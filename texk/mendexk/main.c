#include "mendex.h"
#include "version.h"
#include <kpathsea/tex-file.h>
#include <kpathsea/variable.h>
#include <ptexenc/ptexenc.h>

#include "kana.h"
#include "var.h"

#include "kp.h"

char *styfile[64],*idxfile[256],*indfile,*dicfile,*logfile;

/* default paths */
#ifndef DEFAULT_INDEXSTYLES
#define DEFAULT_INDEXSTYLES "."
#endif
#ifndef DEFAULT_INDEXDICTS
#define DEFAULT_INDEXDICTS "."
#endif
KpathseaSupportInfo kp_ist,kp_dict;

int main(int argc, char **argv)
{
	int i,j,k,cc=0,startpagenum=-1,ecount=0,chkopt=1;
	const char *envbuff;
	char *p;

	enable_UPTEX (true); /* enable */

#ifdef WIN32
	_setmaxstdio(2048);
#endif
	set_enc_string(NULL, "uptex");
	kpse_set_program_name(argv[0], "mendex");

	p = getenv ("PTEX_KANJI_ENC");
	if (p) {
		if (!set_enc_string (p, NULL))
			fprintf (stderr, "Ignoring bad kanji encoding \"%s\".\n", p);
	}

/*   check options   */

	for (i=1,j=k=0;i<argc && j<256;i++) {
		if ((argv[i][0]=='-')&&(strlen(argv[i])>=2)&&chkopt) {
			switch (argv[i][1]) {
			case 'c':
				bcomp=1;
				break;

			case 'd':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					dicfile=xstrdup(argv[++i]);
				}
				else {
					dicfile=xstrdup(&argv[i][2]);
				}
				break;

			case 'f':
				force=1;
				break;

			case 'g':
				gflg=1;
				break;

			case 'i':
				fsti=1;
				break;

			case 'l':
				lorder=1;
				break;

			case 'o':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					indfile=xstrdup(argv[++i]);
				}
				else {
					indfile=xstrdup(&argv[i][2]);
				}
				break;

			case 'p':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					i++;
					if (strcmp(argv[i],"any")==0) fpage=2;
					else if (strcmp(argv[i],"odd")==0) fpage=3;
					else if (strcmp(argv[i],"even")==0) fpage=4;
					else {
						fpage=1;
						startpagenum=atoi(argv[i]);
					}
				}
				else {
					if (strcmp(&argv[i][2],"any")==0) fpage=2;
					else if (strcmp(&argv[i][2],"odd")==0) fpage=3;
					else if (strcmp(&argv[i][2],"even")==0) fpage=4;
					else {
						fpage=1;
						startpagenum=atoi(&argv[i][2]);
					}
				}
				break;

			case 'q':
				verb=0;
				break;

			case 't':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					logfile=xstrdup(argv[++i]);
				}
				else {
					logfile=xstrdup(&argv[i][2]);
				}
				break;

			case 'r':
				prange=0;
				break;

			case 's':
				if (k==64) {
					fprintf (stderr, "Too many style files.\n");
					exit(255);
				}
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					styfile[k]=xstrdup(argv[++i]);
				}
				else {
					styfile[k]=xstrdup(&argv[i][2]);
				}
				k++;
				break;

			case 'v':
				debug=1;
				break;

			case 'E':
				set_enc_string("EUC", NULL);
				break;

			case 'J':
				set_enc_string("JIS", NULL);
				break;

			case 'S':
				set_enc_string("SJIS", NULL);
				break;

			case 'U':
				set_enc_string("UTF8", NULL);
				break;


			case 'I':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					i++;
					if (strcmp(argv[i],"euc")==0) set_enc_string(NULL, "euc");
					else if (strcmp(argv[i],"utf8")==0) set_enc_string(NULL, "uptex");
				}
				else {
					if (strcmp(&argv[i][2],"euc")==0) set_enc_string(NULL, "euc");
					else if (strcmp(&argv[i][2],"utf8")==0) set_enc_string(NULL, "uptex");
				}
				break;

			case '-':
				if (strlen(argv[i])==2) chkopt=0;
				if (strcmp(argv[i],"--guess-input-enc"   )==0) infile_enc_auto=1;
				if (strcmp(argv[i],"--no-guess-input-enc")==0) infile_enc_auto=0;
				if (strcmp(argv[i],"--help")!=0) break;

			default:
				fprintf(stderr,"mendex - Japanese index processor, %s (%s) (%s).\n",VERSION, get_enc_string(), TL_VERSION);
				fprintf(stderr," Copyright 2009 ASCII MEDIA WORKS, 2017-2023 Japanese TeX Development Community\n");
				fprintf(stderr,"usage:\n");
				fprintf(stderr,"%% mendex [-ilqrcgfEJSU] [-s sty] [-d dic] [-o ind] [-t log] [-p no] [-I enc] [--[no-]guess-input-enc] [--] [idx0 idx1 ...]\n");
				fprintf(stderr,"options:\n");
				fprintf(stderr,"-i      use stdin as the input file.\n");
				fprintf(stderr,"-l      use letter ordering.\n");
				fprintf(stderr,"-q      quiet mode.\n");
				fprintf(stderr,"-r      disable implicit page formation.\n");
				fprintf(stderr,"-c      compress blanks. (ignore leading and trailing blanks.)\n");
				fprintf(stderr,"-g      make Japanese index head <%s>.\n",
							is_internalUPTEX() ? AKASATANAutf8 : AKASATANA);
				fprintf(stderr,"-f      force to output kanji.\n");
				fprintf(stderr,"-s sty  take sty as style file.\n");
				fprintf(stderr,"-d dic  take dic as dictionary file.\n");
				fprintf(stderr,"-o ind  take ind as the output index file.\n");
				fprintf(stderr,"-t log  take log as the error log file.\n");
				fprintf(stderr,"-p no   set the starting page number of index.\n");
				fprintf(stderr,"-E      EUC mode.\n");
				fprintf(stderr,"-J      JIS mode.\n");
				fprintf(stderr,"-S      ShiftJIS mode.\n");
				fprintf(stderr,"-U      UTF-8 mode.\n");
				fprintf(stderr,"-I enc  internal encoding for keywords (enc: euc or utf8).\n");
				fprintf(stderr,"--[no-]guess-input-enc  disable/enable to guess input file encoding.\n");
				fprintf(stderr,"idx...  input files.\n");
				fprintf(stderr,"\nEmail bug reports to %s.\n", BUG_ADDRESS);
				exit(0);
				break;
			}
		}
		else {
			cc=strlen(argv[i])+6;
			idxfile[j]=xmalloc(cc);
			strcpy(idxfile[j++],argv[i]);
		}
	}
	idxcount=j+fsti;

	kp_ist.var_name = "INDEXSTYLE";
	kp_ist.path = DEFAULT_INDEXSTYLES; /* default path. */
	kp_ist.suffix = "ist";
	KP_entry_filetype(&kp_ist);
	kp_dict.var_name = "INDEXDICTIONARY";
	kp_dict.path = DEFAULT_INDEXDICTS; /* default path */
	kp_dict.suffix = "dict";
	KP_entry_filetype(&kp_dict);

/*   check option errors   */

	if (idxcount==0) idxcount=fsti=1;

	if (styfile[0]==NULL) {
		envbuff=kpse_var_value("INDEXDEFAULTSTYLE");
		if (envbuff!=NULL) {
			styfile[0]=xstrdup(envbuff);
		}
	}

	if (!indfile &&(idxcount-fsti>0)) {
		indfile=xmalloc(strlen(idxfile[0])+6);
		for (i=strlen(idxfile[0]);i>=0;i--) {
			if (idxfile[0][i]=='.') {
				strncpy(indfile,idxfile[0],i);
				sprintf(&indfile[i],".ind");
				break;
			}
		}
		if (i==-1) sprintf(indfile,"%s.ind",idxfile[0]);
	}

	if (!logfile && (idxcount-fsti > 0)) {
		logfile=xmalloc(strlen(idxfile[0])+6);
		for (i=strlen(idxfile[0]);i>=0;i--) {
			if (idxfile[0][i]=='.') {
				strncpy(logfile,idxfile[0],i);
				sprintf(&logfile[i],".ilg");
				break;
			}
		}
		if (i==-1) sprintf(logfile,"%s.ilg",idxfile[0]);
		}
	if (logfile && kpse_out_name_ok(logfile))
		efp=fopen(logfile,"wb");
	if(efp == NULL) {
		efp=stderr;
		logfile=xstrdup("stderr");
	}

	if (strcmp(argv[0],"makeindex")==0) {
		verb_printf(efp,"This is Not `MAKEINDEX\', But `MENDEX\' %s (%s) (%s).\n",
			    VERSION, get_enc_string(), TL_VERSION);
	}
	else {
		verb_printf(efp,"This is mendex %s (%s) (%s).\n",
			    VERSION, get_enc_string(), TL_VERSION);
	}

/*   init kanatable   */

	initkanatable();

	for (k=0;styfile[k]!=NULL;k++) {
		styread(styfile[k]);
	}

/*   read dictionary   */

	ecount+=dicread(dicfile);

	switch (letter_head) {
	case 0:
	case 1:
		if (gflg==1) {
			strncpy(atama,akasatana,2048);
		}
		else {
			strncpy(atama,aiueo,2048);
		}
		break;

	case 2:
		if (gflg==1) {
			strcpy(atama,AKASATANA);
		}
		else {
			strcpy(atama,AIUEO);
		}
		break;

	default:
		break;
	}

/*   read idx file   */

	lines=0;
	ecount=0;
	ind=xmalloc(sizeof(struct index));

	for (i=0;i<idxcount-fsti;i++) {
		ecount+=idxread(idxfile[i],lines);
	}
	if (fsti==1) {
		ecount+=idxread(NULL,lines);
	}
	verb_printf(efp,"%d entries accepted, %d rejected.\n",acc,reject);

	if (ecount!=0) {
		verb_printf(efp,"%d errors, written in %s.\n",ecount,logfile);
		lines=0;
	}
	if (lines==0) {
		verb_printf(efp,"Nothing written in output file.\n");
		if (efp!=stderr) fclose(efp);
		exit(255);
	}

/*   sort index   */

	verb_printf(efp,"Sorting index.");

	scount=0;
	wsort(ind,lines);

	verb_printf(efp,"...done(%d comparisons).\n",scount);

/*   sort pages   */

	verb_printf(efp,"Sorting pages.");

	scount=0;
	pagesort(ind,lines);

	verb_printf(efp,"...done(%d comparisons).\n",scount);

/*   get last page   */

	if ((fpage>1)&&(idxcount-fsti>0)) cc=lastpage(idxfile[0]);

	switch (fpage) {
	case 2:
		startpagenum=cc+1;
		break;

	case 3:
		if ((cc+1)%2==0) startpagenum=cc+2;
		else startpagenum=cc+1;
		break;

	case 4:
		if ((cc+1)%2==1) startpagenum=cc+2;
		else startpagenum=cc+1;
		break;
		
	default:
		break;
	}

/*   write indfile   */

	verb_printf(efp,"Making index file.");

	indwrite(indfile,ind,startpagenum);

	verb_printf(efp,"...done.\n");

	if (idxcount-fsti==0) indfile=xstrdup("stdout");

	verb_printf(efp,"%d warnings, written in %s.\n",warn,logfile);
	verb_printf(efp,"Output written in %s.\n",indfile);
	if (efp!=stderr) fclose(efp);

	return 0;
}
