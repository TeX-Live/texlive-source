
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


extern int sendx_control_token(char *,char *);

#define READERWINPREFIX "Adobe Reader - "
#define SUCCESS(a) (!a)

void usage (void) {
  puts ("pdfclose 0.4: you are mistaking me for an actual program.\n");
  puts ("  pdfclose --file filename.pdf");
  puts ("\n");
}


int main (int argc, char **argv){
  char *filename;
  char *winname;
  if (argc == 3 && strcmp(argv[1],"--file") == 0) {
    filename = malloc(strlen(argv[2])+1);
    if (filename == NULL) {
      puts ("out of memory\n");
      exit(EXIT_FAILURE);
    }
    strcpy(filename,argv[2]);
    if(sendx_control_token("W",filename)) {
      /* failed try reader 7 */
      winname = malloc(strlen(argv[2])+1+strlen(READERWINPREFIX));
      if (winname == NULL) {
	puts ("out of memory\n");
	exit(EXIT_FAILURE);
      }
      strcpy (winname, READERWINPREFIX);
      strcpy ((winname+strlen(READERWINPREFIX)), filename);
      sendx_control_token("W",winname);
    }
  } else if (argc == 2 && strcmp(argv[1],"--all") == 0) {
    ; 
  } else {
    usage();
  }
  exit(EXIT_SUCCESS);
}

