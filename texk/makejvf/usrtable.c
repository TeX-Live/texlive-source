#ifndef DEBUG
#include <kpathsea/config.h>
#include "makejvf.h"
#endif

#include "usrtable.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
#include <string.h>
#endif

int usertable_replace_max=0,usertable_move_max=0,usertable_charset_max=0;
struct USERTABLE_REPLACE usertable_replace[MAX_TABLE];
struct USERTABLE_MOVE usertable_move[MAX_TABLE];
struct USERTABLE_CHARSET usertable_charset[MAX_CHAR_TABLE];

void get_usertable(char *name)
{
	FILE *fp;
	char *tok,*endptr,buf[BUF_SIZE],str0[8],str1[8];
	int charset_mode=0,l;
	long char_max=-2,ch0,ch1;

	fp = fopen(name,"r");
	if (fp == NULL) {
		fprintf(stderr,"Cannot find %s!\n",name);
		exit(1);
	}
	for (l = 0; fgets(buf, BUF_SIZE, fp) != NULL; l++) {
		if (strncmp(buf, "+", 1) && strncmp(buf, "%", 1)) charset_mode = 0;
		if ((endptr=strchr(buf, '%')) != NULL) strcpy(endptr,"\n");  /* ignore after '%'  */
		if (!strncmp(buf, "\n", 1)) continue;                        /* ignore empty line */
		tok = strtok(buf, "\t");
		if (!strcmp(tok, "REPLACE")) {
			if (usertable_replace_max >= MAX_TABLE) goto buferr;
			usertable_replace[usertable_replace_max].codepoint = strtol(strtok(NULL, "\t\n"), &endptr, 16);
			if (*endptr != '\0') goto taberr;
			usertable_replace[usertable_replace_max].newcodepoint = strtol(strtok(NULL, "\t\n"), &endptr, 16);
			if (*endptr != '\0') goto taberr;
			if (strtok(NULL, "\t\n") != NULL) goto taberr;
			usertable_replace_max++;
			continue;
		}
		if (!strcmp(tok, "MOVE")) {
			if (usertable_move_max >= MAX_TABLE) goto buferr;
			usertable_move[usertable_move_max].codepoint = strtol(strtok(NULL, "\t\n"), &endptr, 16);
			if (*endptr != '\0') goto taberr;
			usertable_move[usertable_move_max].moveright = strtod(strtok(NULL, "\t\n"), &endptr);
			if (*endptr != '\0') goto taberr;
			usertable_move[usertable_move_max].movedown = strtod(strtok(NULL, "\t\n"), &endptr);
			if (*endptr != '\0') goto taberr;
			if (strtok(NULL, "\t\n") != NULL) goto taberr;
			usertable_move_max++;
			continue;
		}
		if ((!strcmp(tok, "+") && charset_mode) || !strcmp(tok, "CHARSET")) {
			charset_mode = 1;
			while ((tok=strtok(NULL, ",\t\n")) != NULL) {
				if ((endptr=strstr(tok,"..")) != NULL) {
					*endptr = '\0';
					if (sscanf(tok,     "%7s",str0) != 1) goto taberr;
					if (sscanf(endptr+2,"%7s",str1) != 1) goto taberr;
					ch0 = strtol(str0, &endptr, 16);
					if (*endptr != '\0' || ch0<=char_max) goto codeerr;
					ch1 = strtol(str1, &endptr, 16);
					if (*endptr != '\0' || ch1<=ch0) goto codeerr;
				} else {
					if (sscanf(tok,"%7s",str0) != 1) goto taberr;
					ch0 = strtol(str0, &endptr, 16);
					if (*endptr != '\0' || ch0<=char_max) goto codeerr;
					ch1 = ch0;
				}
				if (char_max==ch0-1) {
					usertable_charset[usertable_charset_max-1].max = ch1;
				} else {
					if (usertable_charset_max >= MAX_CHAR_TABLE) goto buferr;
					usertable_charset[usertable_charset_max].min = ch0;
					usertable_charset[usertable_charset_max].max = ch1;
					usertable_charset_max++;
				}
				char_max = ch1;
			}
			continue;
		}
		fprintf(stderr, "Unknown setting %s found in %s (line %d)!\n", tok, name, l+1);
		exit(1);
	}
	fclose(fp);
	return;
taberr:
	fprintf(stderr, "Error in user-defined table file %s (line %d)!\n", name, l+1);
	exit(1);
buferr:
	fprintf(stderr, "User-defined table in %s is too large!\n", name);
	exit(1);
codeerr:
	fprintf(stderr, "Character codes must be given in ascending order (line %d)!\n", l+1);
	exit(1);
}


/* for unit test                                      */
/*   ex. $ gcc -g -o usrtable.test usrtable.c -DDEBUG */
#ifdef DEBUG
int main() {
  int i;
  char name[]="test_user_table";
  get_usertable(name);

  if (usertable_replace_max>0) {
    printf("REPLACE::\n");
    for(i=0;i<usertable_replace_max;i++)
      printf("%6d:  %06x -> %06x\n", i, usertable_replace[i].codepoint, usertable_replace[i].newcodepoint);
  }
  if (usertable_move_max>0) {
    printf("MOVE::\n");
    for(i=0;i<usertable_move_max;i++)
      printf("%6d:  %06x | %lf, %lf\n", i, usertable_move[i].codepoint, usertable_move[i].moveright, usertable_move[i].movedown);
  }
  if (usertable_charset_max>0) {
    printf("CHARSET::\n");
    for(i=0;i<usertable_charset_max;i++)
      printf("%6d:  %06lx .. %06lx\n", i, usertable_charset[i].min, usertable_charset[i].max);
  }
  return(0);
}
#endif
