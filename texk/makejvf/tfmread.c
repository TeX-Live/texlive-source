#include <kpathsea/kpathsea.h>
#include "makejvf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nt,nl,unit,zh,zw,jfm_id,rightamount;
int *width,*height,*depth,*italic,*kern,*glue,*param;
unsigned char *header,*char_type,*char_info,*glue_kern;

int jfmread(int kcode)
{
	int i,ctype=0,w_ind,w,ll=0,rr=0,tag,gk_ind,gk2_ind;

	for (i = 0 ; i < nt ; i++) {
		if (upair(&char_type[i*4]) == kcode) {
			ctype = upair(&char_type[i*4+2]);
			break;
		}
	}

	/* get character width of <kcode> */
	w_ind = char_info[ctype*4];
	w = width[w_ind];

	rightamount = 0;
	if (w != zw && ctype > 0) {
		/* get natural length of JFM glue between <type0> and <type of kcode> */
		tag = char_info[ctype*4+2] % 4;
		if (tag == 1) {
			gk_ind = char_info[0*4+3]; /* remainder for <type0> */
			if (glue_kern[gk_ind*4] == 254) /* support for large gluekern table */
				gk_ind = upair(&glue_kern[gk_ind*4+2]);
			for (i = 0 ; i < nl-gk_ind ; i++) {
				if (glue_kern[(gk_ind+i)*4+1] == ctype) {
					if (glue_kern[(gk_ind+i)*4+2] >= 128) {
						gk2_ind = glue_kern[(gk_ind+i)*4+3];
						ll = kern[gk2_ind];
					}
					else {
						gk2_ind = glue_kern[(gk_ind+i)*4+3];
						ll = glue[3*gk2_ind];
					}
					break;
				}
				if (glue_kern[(gk_ind+i)*4] >= 128)
					break;
			}
		}
		/* get natural length of JFM glue between <type of kcode> and <type0> */
		tag = char_info[ctype*4+2] % 4;
		if (tag == 1) {
			gk_ind = char_info[ctype*4+3]; /* remainder for <type of kcode> */
			if (glue_kern[gk_ind*4] == 254) /* support for large gluekern table */
				gk_ind = upair(&glue_kern[gk_ind*4+2]);
			for (i = 0 ; i < nl-gk_ind ; i++) {
				if (glue_kern[(gk_ind+i)*4+1] == 0) {
					if (glue_kern[(gk_ind+i)*4+2] >= 128) {
						gk2_ind = glue_kern[(gk_ind+i)*4+3];
						rr = kern[gk2_ind];
					}
					else {
						gk2_ind = glue_kern[(gk_ind+i)*4+3];
						rr = glue[3*gk2_ind];
					}
					break;
				}
				if (glue_kern[(gk_ind+i)*4] >= 128)
					break;
			}
		}
		if (abs(zw - ll - w - rr) <= 1) /* allow round-off error */
			/* character width is truncated,
			   and metric glue/kern is inserted as a substitute to fill zenkaku */
			rightamount = ll;
		else
			/* character width is actually truncated */
			rightamount = (zw - w)/2;
	}

	return(w);
}

int tfmget(char *name)
{
	char nbuff[1024];
	FILE *fp;

	strcpy(nbuff,name);
	fp = fopen(nbuff,"rb");
	if (fp == NULL) {
		strcat(nbuff,".tfm");
		fp = fopen(nbuff,"rb");
		if (fp == NULL) {
			fprintf(stderr,"%s is not found.\n",name);
			exit(0);
		}
	}

	tfmidx(fp);

	fclose(fp);

	return 0;
}

int tfmidx(FILE *fp)
{
	int i;
	int lh,ec,nw,nh,nd,ni,nk,ng,np;

	jfm_id = fpair(fp);

	if ((jfm_id == 9) || (jfm_id == 11)) {
		nt = ufpair(fp);
		     fpair(fp);
		lh = ufpair(fp);
		     fpair(fp);
		ec = ufpair(fp);
		nw = ufpair(fp);
		nh = ufpair(fp);
		nd = ufpair(fp);
		ni = ufpair(fp);
		nl = ufpair(fp);
		nk = ufpair(fp);
		ng = ufpair(fp);
		np = ufpair(fp);

		header = xmalloc(lh*4);
		for (i = 0 ; i < lh*4 ; i++) {
			header[i] = fgetc(fp);
		}
		char_type = xmalloc(nt*4);
		for (i = 0 ; i < nt*4 ; i++) {
			char_type[i] = fgetc(fp);
		}
		char_info = xmalloc((ec+1)*4);
		for (i = 0 ; i < (ec+1)*4 ; i++) {
			char_info[i] = fgetc(fp);
		}
		width = xmalloc(nw*sizeof(int));
		for (i = 0 ; i < nw ; i++) {
			width[i] = fquad(fp);
		}
		height = xmalloc(nh*sizeof(int));
		for (i = 0 ; i < nh ; i++) {
			height[i] = fquad(fp);
		}
		depth = xmalloc(nd*sizeof(int));
		for (i = 0 ; i < nd ; i++) {
			depth[i] = fquad(fp);
		}
		italic = xmalloc(ni*sizeof(int));
		for (i = 0 ; i < ni ; i++) {
			italic[i] = fquad(fp);
		}
		glue_kern = xmalloc(nl*4);
		for (i = 0 ; i < nl*4 ; i++) {
			glue_kern[i] = fgetc(fp);
		}
		kern = xmalloc(nk*sizeof(int));
		for (i = 0 ; i < nk ; i++) {
			kern[i] = fquad(fp);
		}
		glue = xmalloc(ng*sizeof(int));
		for (i = 0 ; i < ng ; i++) {
			glue[i] = fquad(fp);
		}
		param = xmalloc(np*sizeof(int));
		for (i = 0 ; i < np ; i++) {
			param[i] = fquad(fp);
		}
		unit = mquad(&header[4]);
		zh = param[4];
		zw = param[5];

		if (baseshift)
			baseshift = (int)(zh*baseshift/1000.0+0.5);
	}
	else {
		fprintf(stderr,"This TFM is not for Japanese.\n");
		exit(100);
	}

	return 0;
}
