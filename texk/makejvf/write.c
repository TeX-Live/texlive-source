#include <kpathsea/config.h>
#include "makejvf.h"
#include "uniblock.h"
#include "usrtable.h"

#include <stdio.h>
#include <stdlib.h>

int pstfm_codes[256];

FILE *vfopen(char *name)
{
	FILE *fp;
	int fidshift=0;

	if (fidzero) fidshift=-1;
	fp = fopen(name,"wb");
	if (fp == NULL) {
		fprintf(stderr,"I cannot create VF file, %s.",name);
		exit(100);
	}

	fputc(247,fp); /* PRE */
	fputc(202,fp); /* ID */
	fputc(0,fp); /* comment size */
	fputnum(0,4,fp); /* TFM check sum */
	fputnum(unit,4,fp); /* TFM design size */

	fputc(243,fp); /* fnt_def1 */
	fputc(1+fidshift,fp); /* Font ID */
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
		fputc(2+fidshift,fp); /* Font ID */
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

	if (ucsqtfm) {
		fputc(243,fp); /* fnt_def1 */
		fputc(3+fidshift,fp); /* Font ID */
		fputnum(0,4,fp); /* TFM check sum */
		if (chotai)
			fputnum(zh,4,fp); /* font design size (scaled) */
		else
			fputnum(zw,4,fp); /* font design size (scaled) */
		fputnum(10*(1<<20),4,fp); /* font design size */
		fputc(0,fp); /* directory length */
		fputc(strlen(ucsqtfm),fp); /* fontname length */
		fputstr(ucsqtfm,strlen(ucsqtfm),fp); /* directory + fontname */
	}
	else if (jistfm) {
		fputc(243,fp); /* fnt_def1 */
		fputc(3+fidshift,fp); /* Font ID */
		fputnum(0,4,fp); /* TFM check sum */
		if (chotai)
			fputnum(zh,4,fp); /* font design size (scaled) */
		else
			fputnum(zw,4,fp); /* font design size (scaled) */
		fputnum(10*(1<<20),4,fp); /* font design size */
		fputc(0,fp); /* directory length */
		fputc(strlen(jistfm),fp); /* fontname length */
		fputstr(jistfm,strlen(jistfm),fp); /* directory + fontname */
	}

	return fp;
}

void writevf(int code, FILE *fp)
{
	int cc,cc2,cc3,cc4,w,skip=0,skip2=0,height=1000;
	char buf[256],buf2[256];
	int fidshift=0,l;
	int outcode=code;

	if (fidzero) fidshift=-1;

	w = jfmread(code); /* rightamount is also obtained */

	skip2=baseshift;
	switch (code) {
	case 0x2146: /* ‘ */
	case 0x2148: /* “ */
		if (jfm_id == 9 && minute) { /* 縦書き時はミニュートへ変換 */
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
				skip2+=-(int)(zh*3/5.0); /* skip2+=-(int)((0.6)*zh); */
			}

			fputc(242,fp); /* long_char */
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
			if (kanatfm) fputc(173+fidshift,fp); /* FONT_NUM_2 */
			fputc(129,fp); /* SET2 */
			if (code == 0x2146)
				fputnum(0x216c,2,fp); /* char code */
			else
				fputnum(0x216d,2,fp); /* char code */
			return;
		}
	case 0x214a: /* （ */
	case 0x214c: /* 〔 */
	case 0x214e: /* ［ */
	case 0x2150: /* ｛ */
	case 0x2152: /* 〈 */
	case 0x2154: /* 《 */
	case 0x2156: /* 「 */
	case 0x2158: /* 『 */
	case 0x215a: /* 【 */
		skip = -(zw-w);
		break;
	case 0x2147: /* ’ */
	case 0x2149: /* ” */
		if (jfm_id == 9 && minute) { /* 縦書き時はミニュートへ変換 */
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
				skip2+=(int)(zh*3/5.0); /* skip2+=(int)((0.6)*zh); */
			}

			fputc(242,fp); /* long_char */
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
			if (kanatfm) fputc(173+fidshift,fp); /* FONT_NUM_2 */
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
	case 0x2122: /* 、 */
	case 0x2123: /* 。 */
	case 0x2124: /* ， */
	case 0x2125: /* ． */
	case 0x212b: /* ゛ */
	case 0x212c: /* ゜ */
	case 0x214b: /* ） */
	case 0x214d: /* 〕 */
	case 0x214f: /* ］ */
	case 0x2151: /* ｝ */
	case 0x2153: /* 〉 */
	case 0x2155: /* 》 */
	case 0x2157: /* 」 */
	case 0x2159: /* 』 */
	case 0x215b: /* 】 */
	case 0x216b: /* ° */
	case 0x216c: /* ′ */
	case 0x216d: /* ″ */
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
						break;
					}
				}
			}
			else {
				skip = -(zw-w)/2;
			}
		}
		else {
			if (omitzw) { /* Omit entries in VF for characters with default ZW metric */
				if (!kanatfm || code > 0x2576)
					return;
			}
		}
		break;
	}

	fputc(242,fp); /* long_char */

	for (l = 0; l < usertable_replace_max; l++) {
		if (code == usertable_replace[l].codepoint) {
			outcode = usertable_replace[l].newcodepoint;
			break;
		}
	}
	for (l = 0; l < usertable_move_max; l++) {
		if (code == usertable_move[l].codepoint) {
			skip = usertable_move[l].moveright * zw;
			skip2 = usertable_move[l].movedown * zh;
			goto outputj;
		}
	}
	if (enhanced) {
#ifdef DEBUG
		if (skip != -rightamount) {
			fprintf(stderr,
				"[DEBUG] Conflicting MOVERIGHT value for code %x,\n"
				"[DEBUG]   makejvf default:    %08x\n"
				"[DEBUG]   suggested from JFM: %08x <= I'll use this ...\n",
				code, skip, -rightamount);
		}
#endif
		skip=-rightamount;
	}

outputj:
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
	if (kanatfm) {
		if (code <= 0x2576)
			fputc(173+fidshift,fp); /* FONT_NUM_2 */
		else
			fputc(172+fidshift,fp); /* FONT_NUM_1 */
	}
	fputc(129,fp); /* SET2 */
	fputnum(outcode,2,fp); /* char code */
}

void writevfu(int code, FILE *fp)
{
	int cc,cc2,cc3,cc4,w,skip=0,skip2=0,height=1000;
	char buf[256],buf2[256];
	int fidshift=0,l;
	int outcode=code;

	if (fidzero) fidshift=-1;

	w = jfmread(code); /* rightamount is also obtained */

	skip2=baseshift;
	switch (code) {
	case 0x2018: /* ‘ */
	case 0x201c: /* “ */
		if (jfm_id == 9 && minute) { /* 縦書き時はミニュートへ変換 */
			if (afp) {
				if (code == 0x2018)
					sprintf(buf2,"CH <2032>");
				else
					sprintf(buf2,"CH <2033>");
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
				skip = -(zw-w); /* changed */
			/* no correction needed for skip2 */

			fputc(242,fp); /* long_char */
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
			if (kanatfm) fputc(173+fidshift,fp); /* FONT_NUM_2 */
			fputc(129,fp); /* SET2 */
			if (code == 0x2018)
				fputnum(0x2032,2,fp); /* char code */
			else
				fputnum(0x2033,2,fp); /* char code */
			return;
		}
		else if (ucsqtfm) { /* UniJIS-UCS2-H系へ変換 */
			fputc(242,fp); /* long_char */
			cc=4;
			skip = -(zw-w);
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
			fputc(174+fidshift,fp); /* FONT_NUM_3 */
			fputc(129,fp); /* SET2 */
			fputnum(code,2,fp); /* char code */
			return;
		}
		else if (jfm_id == 11 && jistfm) { /* 横書き時はJIS系へ変換 */
		  /* UCS U+2018 → JIS 0x2146, UCS U+201C → JIS 0x2148 */
			fputc(242,fp); /* long_char */
			cc=4;
			skip = -(zw-w);
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
			fputc(174+fidshift,fp); /* FONT_NUM_3 */
			fputc(129,fp); /* SET2 */
			if (code == 0x2018)
				fputnum(0x2146,2,fp); /* char code */
			else
				fputnum(0x2148,2,fp); /* char code */
			return;
		}
	case 0xFF08: /* （ */
	case 0x3014: /* 〔 */
	case 0xFF3B: /* ［ */
	case 0xFF5B: /* ｛ */
	case 0x3008: /* 〈 */
	case 0x300A: /* 《 */
	case 0x300C: /* 「 */
	case 0x300E: /* 『 */
	case 0x3010: /* 【 */
	case 0xFF5F: /* JIS X 0213  1-02-54 始め二重パーレン */
	case 0x3018: /* JIS X 0213  1-02-56 始め二重亀甲括弧 */
	case 0x3016: /* JIS X 0213  1-02-58 始めすみ付き括弧(白) */
	case 0x00AB: /* JIS X 0213  1-09-08 始め二重山括弧引用記号/始めギュメ */
	case 0x301D: /* JIS X 0213  1-13-64 始めダブルミニュート */
	case 0x2329: /* LEFT-POINTING ANGLE BRACKET */
	case 0x301A: /* LEFT WHITE SQUARE BRACKET */
	case 0xFE59: /* SMALL LEFT PARENTHESIS */
	case 0xFE5B: /* SMALL LEFT CURLY BRACKET */
	case 0xFE5D: /* SMALL LEFT TORTOISE SHELL BRACKET */
		if (ucs != ENTRY_JQ)
			skip = -(zw-w);
		break;
	case 0x2019: /* ’ */
	case 0x201d: /* ” */
		if (jfm_id == 9 && minute) { /* 縦書き時はミニュートへ変換 */
			if (afp) {
				if (code == 0x2019)
					sprintf(buf2,"CH <2032>");
				else
					sprintf(buf2,"CH <2033>");
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
				skip = zw; /* changed */
			/* no correction needed for skip2 */

			fputc(242,fp); /* long_char */
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
			if (kanatfm) fputc(173+fidshift,fp); /* FONT_NUM_2 */
			fputc(129,fp); /* SET2 */
			if (code == 0x2019)
				fputnum(0x2032,2,fp); /* char code */
			else
				fputnum(0x2033,2,fp); /* char code */
			fputc(239,fp); /* XXX1 */
			fputc(32,fp);
			fputs("ps: currentpoint grestore moveto",fp);
			return;
		}
		else if (ucsqtfm) { /* UniJIS-UCS2-H系へ変換 */
			fputc(242,fp); /* long_char */
			cc=4;
			if (skip2)
				cc+=numcount(skip2)+1;
			fputnum(cc,4,fp);
			fputnum(code,4,fp); /* char code */
			fputnum(w,4,fp); /* char width */
			if (skip2) {
				fputc(157+numcount(skip2)-1,fp); /* DOWN */
				fputnum2(skip2,fp);
			}
			fputc(174+fidshift,fp); /* FONT_NUM_3 */
			fputc(129,fp); /* SET2 */
			fputnum(code,2,fp); /* char code */
			return;
		}
		else if (jfm_id == 11 && jistfm) { /* 横書き時はJIS系へ変換 */
		  /* UCS U+2019 → JIS 0x2147, UCS U+201D → JIS 0x2149 */
			fputc(242,fp); /* long_char */
			cc=4;
			if (skip2)
				cc+=numcount(skip2)+1;
			fputnum(cc,4,fp);
			fputnum(code,4,fp); /* char code */
			fputnum(w,4,fp); /* char width */
			if (skip2) {
				fputc(157+numcount(skip2)-1,fp); /* DOWN */
				fputnum2(skip2,fp);
			}
			fputc(174+fidshift,fp); /* FONT_NUM_3 */
			fputc(129,fp); /* SET2 */
			if (code == 0x2019)
				fputnum(0x2147,2,fp); /* char code */
			else
				fputnum(0x2149,2,fp); /* char code */
			return;
		}
	case 0x3000: /* spc */
	case 0x3001: /* 、 */
	case 0x3002: /* 。 */
	case 0xFF0C: /* ， */
	case 0xFF0E: /* ． */
	case 0x309B: /* ゛ */
	case 0x309C: /* ゜ */
	case 0xFF09: /* ） */
	case 0x3015: /* 〕 */
	case 0xFF3D: /* ］ */
	case 0xFF5D: /* ｝ */
	case 0x3009: /* 〉 */
	case 0x300B: /* 》 */
	case 0x300D: /* 」 */
	case 0x300F: /* 』 */
	case 0x3011: /* 】 */
	case 0xFF60: /* JIS X 0213  1-02-55 終わり二重パーレン */
	case 0x3019: /* JIS X 0213  1-02-57 終わり二重亀甲括弧 */
	case 0x3017: /* JIS X 0213  1-02-59 終わりすみ付き括弧(白) */
	case 0x00BB: /* JIS X 0213  1-09-18 終わり二重山括弧引用記号/終わりギュメ */
	case 0x301F: /* JIS X 0213  1-13-65 終わりダブルミニュート */
	case 0x301E: /* --- - ----  ------- 上付き終わりダブルミニュート */
	case 0x232A: /* RIGHT-POINTING ANGLE BRACKET */
	case 0x301B: /* RIGHT WHITE SQUARE BRACKET */
	case 0xFE5A: /* SMALL RIGHT PARENTHESIS */
	case 0xFE5C: /* SMALL RIGHT CURLY BRACKET */
	case 0xFE5E: /* SMALL RIGHT TORTOISE SHELL BRACKET */
	case 0x00B0: /* ° */
	case 0x2032: /* ′ */
	case 0x2033: /* ″ */
		break;
	default:
		if (w != zw) {
			if ((code >= 0xFF61 && code <= 0xFFDC) || (code >= 0xFFE8 && code <= 0xFFEE)) {
				if (jfm_id == 11 && hankana) { /* 半角片仮名など、横書き時 */
					pstfm_codes[pstfm_nt-1]=code;
					pstfm_nt+=1;
					rightamount=0; /* discard jfmread() result */
					break;
				}
			}
			if (!uniblock_iskanji && kanatume>=0) {
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
							case 0x3041:
							case 0x3043:
							case 0x3045:
							case 0x3047:
							case 0x3049:
							case 0x3063:
							case 0x3083:
							case 0x3085:
							case 0x3087:
							case 0x308E:
							case 0x30A1:
							case 0x30A3:
							case 0x30A5:
							case 0x30A7:
							case 0x30A9:
							case 0x30C3:
							case 0x30E3:
							case 0x30E5:
							case 0x30E7:
							case 0x30EE:
							case 0x30F5:
							case 0x30F6:
							case 0x3095: /* JIS X 0213  1-04-85 小書き平仮名か */
							case 0x3096: /* JIS X 0213  1-04-86 小書き平仮名け */
							case 0x31F0: /* JIS X 0213  1-06-78 小書き片仮名ク */
							case 0x31F1: /* JIS X 0213  1-06-79 小書き片仮名シ */
							case 0x31F2: /* JIS X 0213  1-06-80 小書き片仮名ス */
							case 0x31F3: /* JIS X 0213  1-06-81 小書き片仮名ト */
							case 0x31F4: /* JIS X 0213  1-06-82 小書き片仮名ヌ */
							case 0x31F5: /* JIS X 0213  1-06-83 小書き片仮名ハ */
							case 0x31F6: /* JIS X 0213  1-06-84 小書き片仮名ヒ */
							case 0x31F7: /* JIS X 0213  1-06-85 小書き片仮名フ */
							case 0x31F8: /* JIS X 0213  1-06-86 小書き片仮名ヘ */
							case 0x31F9: /* JIS X 0213  1-06-87 小書き片仮名ホ */
							case 0x31FA: /* JIS X 0213  1-06-89 小書き片仮名ム */
							case 0x31FB: /* JIS X 0213  1-06-90 小書き片仮名ラ */
							case 0x31FC: /* JIS X 0213  1-06-91 小書き片仮名リ */
							case 0x31FD: /* JIS X 0213  1-06-92 小書き片仮名ル */
							case 0x31FE: /* JIS X 0213  1-06-93 小書き片仮名レ */
							case 0x31FF: /* JIS X 0213  1-06-94 小書き片仮名ロ */
								skip=-(int)(((double)(1000-(cc4-cc2)-kanatume*2)/2/1000.0)*zw);
								break;
							case 0x30FC:
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
						break;
					}
				}
			}
			else {
				skip = -(zw-w)/2;
			}
		}
		else {
			if (omitzw) { /* Omit entries in VF for characters with default ZW metric */
				if (!kanatfm || uniblock_iskanji)
					return;
			}
		}
		break;
	}

	fputc(242,fp); /* long_char */

	for (l = 0; l < usertable_replace_max; l++) {
		if (code == usertable_replace[l].codepoint) {
			outcode = usertable_replace[l].newcodepoint;
			break;
		}
	}
	for (l = 0; l < usertable_move_max; l++) {
		if (code == usertable_move[l].codepoint) {
			skip = usertable_move[l].moveright * zw;
			skip2 = usertable_move[l].movedown * zh;
			goto outputu;
		}
	}
	if (enhanced) {
#ifdef DEBUG
		if (skip != -rightamount) {
			fprintf(stderr,
				"[DEBUG] Conflicting MOVERIGHT value for code %x,\n"
				"[DEBUG]   makejvf default:    %08x\n"
				"[DEBUG]   suggested from JFM: %08x <= I'll use this ...\n",
				code, skip, -rightamount);
		}
#endif
		skip=-rightamount;
	}

outputu:
	if (kanatfm)
		cc=4;
	else
		cc=3;
	if (outcode>=0x10000)
		cc+=1;
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
	if (kanatfm) {
		if (!uniblock_iskanji)
			fputc(173+fidshift,fp); /* FONT_NUM_2 */
		else
			fputc(172+fidshift,fp); /* FONT_NUM_1 */
	}
	if (outcode>=0x10000) {
		fputc(130,fp); /* SET3 */
		fputnum(outcode,3,fp); /* char code */
	} else {
		fputc(129,fp); /* SET2 */
		fputnum(outcode,2,fp); /* char code */
	}
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
	int i;

	if (strlen(name) >= 252) { /* <buffer size> - ".tfm" */
		fprintf(stderr,"Too long output file name.\n");
		exit(1);
	}
	strcpy(nbuf,name);
	strcat(nbuf,".tfm");
	fp = fopen(nbuf,"wb");
	if (fp == NULL) {
		fprintf(stderr,"I cannot create TFM file, %s.",name);
		exit(100);
	}

	fputnum(jfm_id,2,fp); /* JFM ID */
	fputnum(pstfm_nt,2,fp); /* number of char type */
	if (pstfm_nt>1)
		fputnum(27+pstfm_nt+1,2,fp); /* file words */
	else
		fputnum(27,2,fp); /* file words */
	fputnum(2,2,fp); /* header words */
	if (pstfm_nt>1) {
		fputnum(0,2,fp); /* min of char type */
		fputnum(1,2,fp); /* max of char type */
		fputnum(3,2,fp); /* width words */
	}
	else {
		fputnum(0,2,fp); /* min of char type */
		fputnum(0,2,fp); /* max of char type */
		fputnum(2,2,fp); /* width words */
	}
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
	for (i=0;i<pstfm_nt-1;i++) {
		fputnum(pstfm_codes[i],2,fp); /* char code */
		fputnum(1,2,fp); /* char type */
	}

	if (pstfm_nt>1)
		fputnum((2<<24)+(1<<20)+(1<<16),4,fp); /* char info */
	fputnum((1<<24)+(1<<20)+(1<<16),4,fp); /* char info */
	fputnum(0,4,fp); /* width */
	if (pstfm_nt>1)
		fputnum(1<<19,4,fp); /* width, hankaku-kana */
	fputnum(1<<20,4,fp); /* width, others */
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
