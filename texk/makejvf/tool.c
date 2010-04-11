#include <stdio.h>
#include <strings.h>
#include <math.h>

#include <kpathsea/config.h>

int mquad(unsigned char *p)
{
	unsigned int cc;

	cc = (((unsigned int)p[0]*256
		+(unsigned int)p[1])*256
		+(unsigned int)p[2])*256
		+(unsigned int)p[3];

	return(cc);
}

unsigned int utri(unsigned char *p)
{
	unsigned int i,cc;
	unsigned char fchar[4];

	fchar[0] = 0;
	for (i = 1 ; i < 4 ; i++)
		fchar[i] = p[i-1];

	cc = mquad(fchar);

	return cc;
}

int tri(unsigned char *p)
{
	int i,cc;
	unsigned char fchar[4];

	fchar[0] = 0;
	for (i = 1 ; i < 4 ; i++)
		fchar[i] = p[i-1];

	cc = mquad(fchar);

	if (cc > 8388607) cc -= 16777216;
	return cc;
}

unsigned int upair(unsigned char *p)
{
	unsigned int i,cc;
	unsigned char fchar[4];

	fchar[0] = fchar[1] = 0;
	for (i = 2 ; i < 4 ; i++)
		fchar[i] = p[i-2];

	cc = mquad(fchar);

	return cc;
}

int pair(unsigned char *p)
{
	int i,cc;
	unsigned char fchar[4];

	fchar[0] = fchar[1] = 0;
	for (i = 2 ; i < 4 ; i++)
		fchar[i] = p[i-2];

	cc = mquad(fchar);

	if (cc > 32767) cc -= 65536;
	return cc;
}

int mget(char *p, int num)
{
	switch (num) {
	case 1:
		return *p;
		break;
	case 2:
		return pair(p);
		break;
	case 3:
		return tri(p);
		break;
	case 4:
		return mquad(p);
		break;
	}
}

int fquad(FILE *fp)
{
	int i,cc;
	unsigned char fchar[4];

	for (i = 0 ; i < 4 ; i++)
		fchar[i] = fgetc(fp);

	cc = mquad(fchar);
	return cc;
}

unsigned int uftri(FILE *fp)
{
	unsigned int i,cc;
	unsigned char fchar[4];

	fchar[0] = 0;
	for (i = 1 ; i < 4 ; i++)
		fchar[i] = fgetc(fp);

	cc = mquad(fchar);

	return cc;
}

int ftri(FILE *fp)
{
	int i,cc;
	unsigned char fchar[4];

	fchar[0]=0;
	for (i=1;i<4;i++)
		fchar[i]=fgetc(fp);

	cc = mquad(fchar);

	if (cc > 8388607) cc -= 16777216;

	return cc;
}

unsigned int ufpair(FILE *fp)
{
	unsigned int i,cc;
	unsigned char fchar[4];

	fchar[0] = fchar[1] = 0;
	for (i = 2 ; i < 4 ; i++)
		fchar[i] = fgetc(fp);

	cc = mquad(fchar);

	return cc;
}

int fpair(FILE *fp)
{
	int i,cc;
	unsigned char fchar[4];

	fchar[0] = fchar[1] = 0;
	for (i = 2 ; i < 4 ; i++)
		fchar[i] = fgetc(fp);

	cc = mquad(fchar);

	if (cc > 32767) cc -= 65536;

	return cc;
}

int fskip(FILE *fp, int num)
{
	fseek(fp,num,1);
}

int fputnum(int num, int byte, FILE *fp)
{
	int i,cc;
	unsigned char buf[16];

	if (num<0) {
		if (byte==3) {
			num=16777216+num;
		}
		else if (byte==2) {
			num=65536+num;
		}
		else if (byte==1) {
			num=256+num;
		}
	}

	for (i = byte-1 ; i >= 0 ; i--) {
		buf[i] = (num%256);
		num >>= 8;
	}

	for (i = 0 ; i < byte ; i++) {
		fputc(buf[i],fp);
	}

	return 0;
}

int numcount(int num)
{
	if (num >= 0) {
		if (num <= 127) {
			return 1;
		}
		else if (num <= 32767) {
			return 2;
		}
		else if (num <= 838607) {
			return 3;
		}
		else {
			return 4;
		}
	}
	else {
		if (num >= -128) {
			return 1;
		}
		else if (num >= -32768) {
			return 2;
		}
		else if (num >= -838608) {
			return 3;
		}
		else {
			return 4;
		}
	}
}

int fputnum2(int num, FILE *fp)
{
	fputnum(num,numcount(num),fp);
	return 0;
}

int fputstr(char *str, int byte, FILE *fp)
{
	int i;

	for (i = 0 ; i < byte ; i++) {
		fputc(str[i],fp);
	}

	return 0;
}
int cutspace(FILE *fp)
{
	int cc;

	cc = fgetc(fp);
	if (cc == ';') {
		ungetc(cc,fp);
		return 0;
	}
	if (cc != ' ' && cc != '\t' && cc != '\n' && cc != '\0') return -1;
	while (1) {
		cc = fgetc(fp);
		if (cc == EOF) return -1;
		if (cc != ' ' && cc != '\t' && cc != '\n' && cc != '\0') {
			ungetc(cc,fp);
			return 0;
		}
	}
}

int gethex(FILE *fp)
{
	int cc,i;
	char buf[256];

	if (fgetc(fp) != '<') {
		return -1;
	}

	for (i = 0 ; ; i++) {
		cc = fgetc(fp);
		if ((cc >= '0'  && cc <= '9') || (cc >= 'a' && cc <= 'f') || (cc >= 'A' && cc <= 'F')) {
			buf[i] = cc;
		}
		else if (cc == '>') {
			buf[i] = '\0';
			return strtol(buf,NULL,16);
		}
		else {
			return -1;
		}
	}
}

int getdec(FILE *fp)
{
	int cc,i;
	char buf[256];

	for (i = 0 ; ; i++) {
		cc = fgetc(fp);
		if ((cc >= '0'  && cc <= '9') || cc == '-') {
			buf[i] = cc;
		}
		else if (cc == ' ' || cc == '\t' || cc == '\n' || cc == '\0' || cc == EOF) {
			ungetc(cc,fp);
			buf[i] = '\0';
			return atoi(buf);
		}
		else return -1;
	}
}
