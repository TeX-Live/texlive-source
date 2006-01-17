
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* returns 1 on failure */
extern int sendx_control_token(char *,char *);
extern int sendx_alt_token(char *,char *);

void usage (void) {
  puts ("pdfopen 0.4: you are mistaking me for an actual program.\n");
  puts("  pdfopen [--file filename.pdf]");
  puts ("\nusing no arguments tells the Reader to 'go back'.\n");
}

#define READERFIVE "Acrobat Reader"
#define READERNAME "Adobe Reader"
#define READERWINPREFIX "Adobe Reader - "
#define SUCCESS(a) (!a)

int main (int argc, char **argv){
  char *newargv[3];
  char *winname;
  char *filename;
  int test = 1;
  if (argc == 5 && 
      (strcmp(argv[3],"--page") == 0 
       && strcmp(argv[1],"--file") == 0 )) {
    argc = 3;
  } 
  if (argc == 3 && strcmp(argv[1],"--file") == 0) {
    filename = malloc(strlen(argv[2])+1);
    if (filename == NULL) {
      puts ("out of memory\n");
      exit(EXIT_FAILURE);
    }
    strcpy(filename,argv[2]);
    winname = malloc(strlen(argv[2])+1+strlen(READERWINPREFIX));
    if (winname == NULL) {
      puts ("out of memory\n");
      exit(EXIT_FAILURE);
    }
    strcpy (winname,READERWINPREFIX);
    strcpy ((winname+strlen(READERWINPREFIX)), filename);
    if (SUCCESS(sendx_control_token("W",winname))) {
      sendx_alt_token("Left",READERNAME); /* that's seven */
    } else if (SUCCESS(sendx_control_token("W",filename))) {
      sendx_control_token("Left",READERFIVE); /* that's five */
    } else {
      newargv[0] = "acroread";
      newargv[1] = filename;
      newargv[2] = NULL;
      if(!fork()) {
	execvp("acroread",newargv);
	/* this trick makes sure that there *is* a back option
	   in Acrobat Reader */
	while(test) {
	  test = sendx_control_token("+",filename);
	}
      }
    }
  } else if (argc != 1) {
    usage();
  } else {
    sendx_alt_token("Left",READERNAME);
    sendx_control_token("Left",READERFIVE);
  }
  exit(EXIT_SUCCESS);
}

