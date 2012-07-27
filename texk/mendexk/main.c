#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kpathsea/config.h>
#include <kpathsea/tex-file.h>
#include <ptexenc/ptexenc.h>
#include "mendex.h"

#include "kana.h"
#include "var.h"

#ifdef KPATHSEA
#include "kp.h"
#endif

char *styfile,*idxfile[256],indfile[256],*dicfile,logfile[256];

#ifdef KPATHSEA
/* default paths */
#ifndef DEFAULT_INDEXSTYLES
#define DEFAULT_INDEXSTYLES "."
#endif
#ifndef DEFAULT_INDEXDICTS
#define DEFAULT_INDEXDICTS "."
#endif
KpathseaSupportInfo kp_ist,kp_dict;
#endif

#define VERSION "version 2.6f [14-Aug-2009]"

int main(int argc, char **argv)
{
	int i,j,cc=0,startpagenum=-1,ecount=0;
	const char *envbuff;

        set_enc_string(NULL, "EUC");
#ifdef WIN32
		_setmaxstdio(2048);
#endif
#ifdef KPATHSEA
	KP_init(argv[0]);
	kp_ist.var_name = "INDEXSTYLE";
	kp_ist.path = DEFAULT_INDEXSTYLES; /* default path. */
	kp_ist.suffix = "ist";
	KP_entry_filetype(&kp_ist);
	kp_dict.var_name = "INDEXDICTIONARY";
	kp_dict.path = DEFAULT_INDEXDICTS; /* default path */
	kp_dict.suffix = "dict";
	KP_entry_filetype(&kp_dict);
#endif

/*   check options   */

	for (i=1,j=0;i<argc;i++) {
		if ((argv[i][0]=='-')&&(strlen(argv[i])>=2)) {
			switch (argv[i][1]) {
			case 'c':
				bcomp=1;
				break;

			case 'd':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					i++;
					dicfile=malloc(strlen(argv[i])+1);
					strcpy(dicfile,argv[i]);
				}
				else {
					dicfile=malloc(strlen(&argv[i][2])+1);
					strcpy(dicfile,&argv[i][2]);
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
					strcpy(indfile,argv[++i]);
				}
				else {
					strcpy(indfile,&argv[i][2]);
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
					strcpy(logfile,argv[++i]);
				}
				else {
					strcpy(logfile,&argv[i][2]);
				}
				break;

			case 'r':
				prange=0;
				break;

			case 's':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					i++;
					styfile=malloc(strlen(argv[i])+1);
					strcpy(styfile,argv[i]);
				}
				else {
					styfile=malloc(strlen(&argv[i][2])+1);
					strcpy(styfile,&argv[i][2]);
				}
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

			default:
				fprintf(stderr,"mendex - Japanese index processor, %s (%s).\n",VERSION, get_enc_string());
				fprintf(stderr," Copyright 2009 ASCII MEDIA WORKS.(ptex-staff@ml.asciimw.jp)\n");
				fprintf(stderr,"usage:\n");
				fprintf(stderr,"%% mendex [-ilqrcg] [-s sty] [-d dic] [-o ind] [-t log] [-p no] [idx0 idx1 ...]\n");
				fprintf(stderr,"options:\n");
				fprintf(stderr,"-i      use stdin as the input file.\n");
				fprintf(stderr,"-l      use letter ordering.\n");
				fprintf(stderr,"-q      quiet mode.\n");
				fprintf(stderr,"-r      disable implicit page formation.\n");
				fprintf(stderr,"-c      compress blanks. (ignore leading and trailing blanks.)\n");
				fprintf(stderr,"-g      make Japanese index head <%s>.\n", AKASATANA);
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
				fprintf(stderr,"idx...  input files.\n");
				exit(0);
				break;
			}
		}
		else {
			cc=strlen(argv[i]);
			if (cc<4) cc+=4;
			else if (strcmp(&argv[i][cc-4],".idx")) cc+=4;
			idxfile[j]=malloc(cc+1);
			strcpy(idxfile[j++],argv[i]);
		}
	}
	idxcount=j+fsti;

/*   check option errors   */

	if (idxcount==0) idxcount=fsti=1;

	if (styfile==NULL) {
#ifdef KPATHSEA
		envbuff=KP_get_value("INDEXDEFAULTSTYLE",NULL);
#else
		envbuff=getenv("INDEXDEFAULTSTYLE");
#endif
		if (envbuff!=NULL) {
			styfile=malloc(strlen(envbuff)+1);
			strcpy(styfile,envbuff);
		}
	}

	if (styfile!=NULL) styread(styfile);

	if ((indfile[0]=='\0')&&(idxcount-fsti>0)) {
		for (i=strlen(idxfile[0]);i>=0;i--) {
			if (idxfile[0][i]=='.') {
				strncpy(indfile,idxfile[0],i);
				sprintf(&indfile[i],".ind");
				break;
			}
		}
		if (i==-1) sprintf(indfile,"%s.ind",idxfile[0]);
	}

	if ((logfile[0] == '\0') && (idxcount-fsti > 0)) {
		for (i=strlen(idxfile[0]);i>=0;i--) {
			if (idxfile[0][i]=='.') {
				strncpy(logfile,idxfile[0],i);
				sprintf(&logfile[i],".ilg");
				break;
			}
		}
		if (i==-1) sprintf(logfile,"%s.ilg",idxfile[0]);
		}
	if ((logfile[0] != '\0') && kpse_out_name_ok(logfile))
		efp=fopen(logfile,"wb");
	if(efp == NULL) {
		efp=stderr;
		strcpy(logfile,"stderr");
	}

	if (strcmp(argv[0],"makeindex")==0) {
		verb_printf(efp,"This is Not `MAKEINDEX\', But `MENDEX\' %s (%s).\n",
			    VERSION, get_enc_string());
	}
	else {
		verb_printf(efp,"This is mendex %s (%s).\n",
			    VERSION, get_enc_string());
	}

/*   init kanatable   */

	initkanatable();

/*   read dictionary   */

	ecount+=dicread(dicfile);

	switch (letter_head) {
	case 0:
	case 1:
		if (gflg==1) {
			strcpy(atama,akasatana);
		}
		else {
			strcpy(atama,aiueo);
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
	ind=malloc(sizeof(struct index));

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

	if (idxcount-fsti==0) strcpy(indfile,"stdout");

	verb_printf(efp,"%d warnings, written in %s.\n",warn,logfile);
	verb_printf(efp,"Output written in %s.\n",indfile);
	if (efp!=stderr) fclose(efp);

	return 0;
}
