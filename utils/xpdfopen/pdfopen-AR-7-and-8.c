
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* returns 1 on failure */
extern int sendx_control_token(char *,char *);
extern int sendx_alt_token(char *,char *);

void usage (void) {
  puts ("pdfopen 0.61: you are mistaking me for an actual program.\n");
  puts("  pdfopen [--file filename.pdf]");
  puts ("\nusing no arguments tells the Reader to 'go back'.\n");
}

#define READERFIVE "Acrobat Reader"
#define READERNAME "Adobe Reader"
#define READERWINPREFIX  "Adobe Reader - "
#define READERWINPOSTFIX " - Adobe Reader"
#define SUCCESS(a) (!a)

int main (int argc, char **argv){
  char *newargv[3];
  char *winname, *win8name;
  char *filename;
  char *basefile;
  pid_t reader;
  if (argc == 5 && 
      (strcmp(argv[3],"--page") == 0 
       && strcmp(argv[1],"--file") == 0 )) {
    argc = 3;
  } 
  if (argc == 3 && strcmp(argv[1],"--file") == 0) {
    filename = malloc(strlen(argv[2])+1);
	basefile = malloc(strlen(argv[2])+1);
    if (filename == NULL || basefile == NULL) {
      puts ("out of memory\n");
      exit(EXIT_FAILURE);
    }
    strcpy(filename,argv[2]);
	strcpy(basefile,filename);
	if (strrchr(basefile,'/'))
	  basefile = strrchr(basefile,'/')+1;

    winname = malloc(strlen(basefile)+1+strlen(READERWINPREFIX));
    win8name = malloc(strlen(basefile)+1+strlen(READERWINPOSTFIX));
    if (winname == NULL || win8name == NULL) {
      puts ("out of memory\n");
      exit(EXIT_FAILURE);
    }
    strcpy (winname,READERWINPREFIX);
    strcpy ((winname+strlen(READERWINPREFIX)), basefile);
    strcpy (win8name,basefile);
    strcpy ((win8name+strlen(basefile)), READERWINPOSTFIX);
    if (SUCCESS(sendx_control_token("W",winname)) ||  /* that's seven */
		SUCCESS(sendx_control_token("W",win8name))) { /* that's eight */
      sendx_alt_token("Left",READERNAME);      /* seven or eight */
      sendx_control_token("Left",READERNAME);  /* that's seven, with artificial five keymaps */
    } else if (SUCCESS(sendx_control_token("W",filename))) {
      sendx_control_token("Left",READERFIVE); /* that's five */
    } else if (SUCCESS(sendx_control_token("W",basefile))) {
      sendx_control_token("Left",READERFIVE); /* that's five */
    } else {
      if((reader = fork()) >= 0) {
		if (reader) {
		  waitpid(reader,NULL,WNOHANG);
		} else {
		  newargv[0] = "acroread";
		  newargv[1] = filename;
		  newargv[2] = NULL;
		  if(execvp("acroread",newargv)) {
			puts ("acroread startup failed\n");
			exit(EXIT_FAILURE);
		  }			
		}
	  } else {
		puts ("fork failed\n");
		exit(EXIT_FAILURE);
	  }
    }
  } else if (argc != 1) {
    usage();
  } else {
    sendx_alt_token("Left",READERNAME);
    sendx_control_token("Left",READERNAME); /* that's seven */
    sendx_control_token("Left",READERFIVE);
  }
  exit(EXIT_SUCCESS);
}

