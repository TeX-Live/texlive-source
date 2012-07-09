#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kpathsea/config.h>
#include "makejvf.h"

int nt,unit,zh,zw,jfm_id;
int *width,*height,*depth,*italic,*glue_kern,*kern,*glue,*param;
unsigned char *header,*char_type,*char_info;

int jfmread(int kcode)
{
	int i,ctype = 0,w_ind,w;

	for (i = 0 ; i < nt ; i++) {
		if (upair(&char_type[i*4]) == kcode) {
			ctype = upair(&char_type[i*4+2]);
			break;
		}
	}

	w_ind = char_info[ctype*4];

	w = width[w_ind];

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
	int i,cc;
	int lh,ec,nw,nh,nd,ni,nl,nk,ng,np;

	jfm_id = fpair(fp);

	if ((jfm_id == 9) || (jfm_id == 11)) {
		nt = ufpair(fp);
		cc = fpair(fp);
		lh = ufpair(fp);
		cc = fpair(fp);
		ec = ufpair(fp);
		nw = ufpair(fp);
		nh = ufpair(fp);
		nd = ufpair(fp);
		ni = ufpair(fp);
		nl = ufpair(fp);
		nk = ufpair(fp);
		ng = ufpair(fp);
		np = ufpair(fp);

		header = malloc(lh*4);
		for (i = 0 ; i < lh*4 ; i++) {
			header[i] = fgetc(fp);
		}
		char_type = malloc(nt*4);
		for (i = 0 ; i < nt*4 ; i++) {
			char_type[i] = fgetc(fp);
		}
		char_info = malloc((ec+1)*4);
		for (i = 0 ; i < (ec+1)*4 ; i++) {
			char_info[i] = fgetc(fp);
		}
		width = malloc(nw*sizeof(int));
		for (i = 0 ; i < nw ; i++) {
			width[i] = fquad(fp);
		}
		height = malloc(nh*sizeof(int));
		for (i = 0 ; i < nh ; i++) {
			height[i] = fquad(fp);
		}
		depth = malloc(nd*sizeof(int));
		for (i = 0 ; i < nd ; i++) {
			depth[i] = fquad(fp);
		}
		fseek(fp,(ni+nl+nk+ng)*4,SEEK_CUR);
		param = malloc(np*sizeof(int));
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
