#include <stdio.h>
#include <stdlib.h>

#include <kpathsea/config.h>
#include "makejvf.h"

FILE *vfopen(char *name)
{
	FILE *fp;

	fp = fopen(name,"w");
	if (fp == NULL) {
		fprintf(stderr,"I cannot create VF file, %s.",name);
		exit(-1);
	}

	fputc(247,fp); /* PRE */
	fputc(202,fp); /* ID */
	fputc(0,fp); /* comment size */
	fputnum(0,4,fp); /* TFM check sum */
	fputnum(10*(1<<20),4,fp); /* TFM design size */

	fputc(243,fp); /* fnt_def1 */
	fputc(1,fp); /* Font ID */
	fputnum(0,4,fp); /* TFM check sum */
	if (chotai)
		fputnum(zh,4,fp); /* font design size (scaled) */
	else
		fputnum(zw,4,fp); /* font design size (scaled) */
	fputnum(10*(1<<20),4,fp); /* font design size */
	fputc(0,fp); /* directory length */
	fputc(strlen(vtfmname),fp); /* fontname length */
	fputstr(vtfmname,strlen(vtfmname),fp); /* directory + fontname */

	if (kanatfm) {
		fputc(243,fp); /* fnt_def1 */
		fputc(2,fp); /* Font ID */
		fputnum(0,4,fp); /* TFM check sum */
		if (chotai)
			fputnum(zh,4,fp); /* font design size (scaled) */
		else
			fputnum(zw,4,fp); /* font design size (scaled) */
		fputnum(10*(1<<20),4,fp); /* font design size */
		fputc(0,fp); /* directory length */
		fputc(strlen(kanatfm),fp); /* fontname length */
		fputstr(kanatfm,strlen(kanatfm),fp); /* directory + fontname */
	}

	return fp;
}

void writevf(int code, FILE *fp)
{
	int cc,cc2,cc3,cc4,w,skip=0,skip2=0,height;
	char buf[256],buf2[256];

	w = jfmread(code);

	fputc(242,fp); /* long_char */

	skip2=baseshift;
	switch (code) {
	case 0x2146: /* ¡Æ */
	case 0x2148: /* ¡È */
		if (jfm_id == 9 && minute) { /* ½Ä½ñ¤­»þ¤Ï¥ß¥Ë¥å¡¼¥È¤ØÊÑ´¹ */
			if (afp) {
				if (code == 0x2146)
					sprintf(buf2,"CH <216C>");
				else
					sprintf(buf2,"CH <216D>");
				rewind(afp);
				while (fgets(buf,255,afp)!=NULL) {
					if (jfm_id==9 && !strncmp(buf,"FontBBox ",9)) {
						sscanf(&buf[9],"%d %d %d %d",&cc,&cc2,&cc3,&cc4);
						height=cc4;
					}
					if (!strncmp(buf,buf2,strlen(buf2))) {
						sscanf(&buf[14],"%d %d %d %d",&cc,&cc2,&cc3,&cc4);
						skip=(int)(w+((double)(cc2-height)/1000.0-0.05)*zw);
						break;
					}
				}
			}
			else
				skip=(int)((0.1)*zw);
			if (code == 0x2146) {
				skip2+=-(int)((0.65)*zh);
			}
			else {
				skip2+=-(int)((0.6)*zh);
			}

			if (kanatfm)
				cc=4;
			else
				cc=3;
			if (skip)
				cc+=numcount(skip)+1;
			if (skip2)
				cc+=numcount(skip2)+1;
			fputnum(cc,4,fp);
			fputnum(code,4,fp); /* char code */
			fputnum(w,4,fp); /* char width */
			if (skip) {
				fputc(143+numcount(skip)-1,fp); /* RIGHT */
				fputnum2(skip,fp);
			}
			if (skip2) {
				fputc(157+numcount(skip2)-1,fp); /* DOWN */
				fputnum2(skip2,fp);
			}
			if (kanatfm) fputc(173,fp); /* FONT_NUM_2 */
			fputc(129,fp); /* SET2 */
			if (code == 0x2146)
				fputnum(0x216c,2,fp); /* char code */
			else
				fputnum(0x216d,2,fp); /* char code */
			return;
		}
	case 0x214a: /* ¡Ê */
	case 0x214c: /* ¡Ì */
	case 0x214e: /* ¡Î */
	case 0x2150: /* ¡Ð */
	case 0x2152: /* ¡Ò */
	case 0x2154: /* ¡Ô */
	case 0x2156: /* ¡Ö */
	case 0x2158: /* ¡Ø */
	case 0x215a: /* ¡Ú */
		skip = -(zw-w);
		if (kanatfm)
			cc=4;
		else
			cc=3;
		if (skip)
			cc+=numcount(skip)+1;
		if (skip2)
			cc+=numcount(skip2)+1;
		fputnum(cc,4,fp);
		break;
	case 0x2147: /* ¡Ç */
	case 0x2149: /* ¡É */
		if (jfm_id == 9 && minute) { /* ½Ä½ñ¤­»þ¤Ï¥ß¥Ë¥å¡¼¥È¤ØÊÑ´¹ */
			if (afp) {
				if (code == 0x2147)
					sprintf(buf2,"CH <216C>");
				else
					sprintf(buf2,"CH <216D>");
				rewind(afp);
				while (fgets(buf,255,afp)!=NULL) {
					if (jfm_id==9 && !strncmp(buf,"FontBBox ",9)) {
						sscanf(&buf[9],"%d %d %d %d",&cc,&cc2,&cc3,&cc4);
						height=cc4;
					}
					if (!strncmp(buf,buf2,strlen(buf2))) {
						sscanf(&buf[14],"%d %d %d %d",&cc,&cc2,&cc3,&cc4);
						skip=(int)(((double)(height-cc2)/1000.0+0.05)*zw);
						break;
					}
				}
			}
			else
				skip=(int)((0.4)*zw);
			if (code == 0x2147) {
				skip2+=(int)((0.65)*zh);
			}
			else {
				skip2+=(int)((0.6)*zh);
			}

			if (kanatfm)
				cc=4;
			else
				cc=3;
			if (skip)
				cc+=numcount(skip)+1;
			if (skip2)
				cc+=numcount(skip2)+1;
			fputnum(cc+2+88+2+32,4,fp);
			fputnum(code,4,fp); /* char code */
			fputnum(w,4,fp); /* char width */
			if (skip) {
				fputc(143+numcount(skip)-1,fp); /* RIGHT */
				fputnum2(skip,fp);
			}
			if (skip2) {
				fputc(157+numcount(skip2)-1,fp); /* DOWN */
				fputnum2(skip2,fp);
			}
			fputc(239,fp); /* XXX1 */
			fputc(88,fp);
			fputs("ps: gsave currentpoint currentpoint translate 180 neg rotate neg exch neg exch translate",fp);
			if (kanatfm) fputc(173,fp); /* FONT_NUM_2 */
			fputc(129,fp); /* SET2 */
			if (code == 0x2147)
				fputnum(0x216c,2,fp); /* char code */
			else
				fputnum(0x216d,2,fp); /* char code */
			fputc(239,fp); /* XXX1 */
			fputc(32,fp);
			fputs("ps: currentpoint grestore moveto",fp);
			return;
		}
	case 0x2121: /* spc */
	case 0x2122: /* ¡¢ */
	case 0x2123: /* ¡£ */
	case 0x2124: /* ¡¤ */
	case 0x2125: /* ¡¥ */
	case 0x212b: /* ¡« */
	case 0x212c: /* ¡¬ */
	case 0x214b: /* ¡Ë */
	case 0x214d: /* ¡Í */
	case 0x214f: /* ¡Ï */
	case 0x2151: /* ¡Ñ */
	case 0x2153: /* ¡Ó */
	case 0x2155: /* ¡Õ */
	case 0x2157: /* ¡× */
	case 0x2159: /* ¡Ù */
	case 0x215b: /* ¡Û */
	case 0x216b: /* ¡ë */
	case 0x216c: /* ¡ì */
	case 0x216d: /* ¡í */
		if (kanatfm)
			cc=4;
		else
			cc=3;
		if (skip2)
			cc+=numcount(skip2)+1;
		fputnum(cc,4,fp);
		break;
	default:
		if (w != zw) {
			if (((code >= 0x2421 && code <= 0x2576) || code == 0x213c ) && kanatume>=0) {
				sprintf(buf2,"CH <%X>",code);
				rewind(afp);
				while (fgets(buf,255,afp)!=NULL) {
					if (jfm_id==9 && !strncmp(buf,"FontBBox ",9)) {
						sscanf(&buf[9],"%d %d %d %d",&cc,&cc2,&cc3,&cc4);
						height=cc4;
					}
					if (!strncmp(buf,buf2,strlen(buf2))) {
						sscanf(&buf[14],"%d %d %d %d",&cc,&cc2,&cc3,&cc4);
						if (jfm_id==9) {
							switch (code) {
							case 0x2421:
							case 0x2423:
							case 0x2425:
							case 0x2427:
							case 0x2429:
							case 0x2443:
							case 0x2463:
							case 0x2465:
							case 0x2467:
							case 0x246e:
							case 0x2521:
							case 0x2523:
							case 0x2525:
							case 0x2527:
							case 0x2529:
							case 0x2543:
							case 0x2563:
							case 0x2565:
							case 0x2567:
							case 0x256e:
							case 0x2575:
							case 0x2576:
								skip=-(int)(((double)(1000-(cc4-cc2)-kanatume*2)/2/1000.0)*zw);
								break;
							case 0x213c:
								skip=-(int)((double)(cc-kanatume)/1000.0*zw);
								break;
							default:
								skip=-(int)(((double)(height-cc4-kanatume)/1000.0)*zw);
								break;
							}
						}
						else {
							skip=-(int)(((double)(cc-kanatume)/1000.0)*zw);
						}
						if (kanatfm)
							cc=4;
						else
							cc=3;
						if (skip)
							cc+=numcount(skip)+1;
						if (skip2)
							cc+=numcount(skip2)+1;
						fputnum(cc,4,fp);
						break;
					}
				}
			}
			else {
				skip = -(zw-w)/2;
				if (kanatfm)
					cc=4;
				else
					cc=3;
				if (skip)
					cc+=numcount(skip)+1;
				if (skip2)
					cc+=numcount(skip2)+1;
				fputnum(cc,4,fp);
			}
		}
		else {
			if (kanatfm)
				cc=4;
			else
				cc=3;
			if (skip)
				cc+=numcount(skip)+1;
			if (skip2)
				cc+=numcount(skip2)+1;
			fputnum(cc,4,fp);
		}
		break;
	}

	fputnum(code,4,fp); /* char code */
	fputnum(w,4,fp); /* char width */
	if (skip) {
		fputc(143+numcount(skip)-1,fp); /* RIGHT */
		fputnum2(skip,fp);
	}
	if (skip2) {
		fputc(157+numcount(skip2)-1,fp); /* DOWN */
		fputnum2(skip2,fp);
	}
	if (kanatfm) {
		if (code <= 0x2576)
			fputc(173,fp); /* FONT_NUM_2 */
		else
			fputc(172,fp); /* FONT_NUM_1 */
	}
	fputc(129,fp); /* SET2 */
	fputnum(code,2,fp); /* char code */
}

void vfclose(FILE *fp)
{
	int i,cc;

    cc = ftell(fp);
    for (i = 0 ; i < 4-(cc%4) ; i++) {
        fputc(248,fp); /* POST */
	}
	fclose(fp);
}

void maketfm(char *name)
{
	char nbuf[256];
	FILE *fp;

	strcpy(nbuf,name);
	strcat(nbuf,".tfm");
	fp = fopen(nbuf,"w");
	if (fp == NULL) {
		fprintf(stderr,"I cannot create TFM file, %s.",name);
		exit(-1);
	}

	fputnum(jfm_id,2,fp); /* JFM ID */
	fputnum(1,2,fp); /* number of char type */
	fputnum(27,2,fp); /* file words */
	fputnum(2,2,fp); /* header words */
	fputnum(0,2,fp); /* min of char type */
	fputnum(0,2,fp); /* max of char type */
	fputnum(2,2,fp); /* width words */
	fputnum(2,2,fp); /* height words */
	fputnum(2,2,fp); /* depth words */
	fputnum(1,2,fp); /* italic words */
	fputnum(0,2,fp); /* glue/kern words */
	fputnum(0,2,fp); /* kern words */
	fputnum(0,2,fp); /* glue words */
	fputnum(9,2,fp); /* param words */

	fputnum(0,4,fp); /* check sum */
	fputnum(10*(1<<20),4,fp); /* design size */

	fputnum(0,2,fp); /* char code */
	fputnum(0,2,fp); /* char type */

	fputnum((1<<24)+(1<<20)+(1<<16),4,fp); /* char info */
	fputnum(0,4,fp); /* width */
	fputnum(1<<20,4,fp); /* width */
	if (jfm_id == 11) {
		fputnum(0,4,fp); /* height */
		fputnum((int)((1<<20)*0.9),4,fp); /* height */
		fputnum(0,4,fp); /* depth */
		fputnum((1<<20)-(int)((1<<20)*0.9),4,fp); /* depth */
	}
	else {
		fputnum(0,4,fp); /* height */
		fputnum(1<<19,4,fp); /* height */
		fputnum(0,4,fp); /* depth */
		fputnum(1<<19,4,fp); /* depth */
	}
	fputnum(0,4,fp); /* italic */

	fputnum(0,4,fp); /* tan */
	fputnum(0,4,fp); /* kanjiskip */
	fputnum(0,4,fp); /* +kanjiskip */
	fputnum(0,4,fp); /* -kanjiskip */
	fputnum(1<<20,4,fp); /* zh */
	fputnum(1<<20,4,fp); /* zw */
	fputnum(0,4,fp); /* xkanjiskip */
	fputnum(0,4,fp); /* +xkanjiskip */
	fputnum(0,4,fp); /* -xkanjiskip */

	fclose(fp);
}
