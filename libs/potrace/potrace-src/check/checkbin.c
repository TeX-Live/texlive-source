/* Copyright (C) 2001-2019 Peter Selinger.
   This file is part of Potrace. It is free software and it is covered
   by the GNU General Public License. See the file COPYING for details. */

/* Check whether a text file uses CR or CRLF style line endings, by
   checking the first line only. Return 1 if the first line ends in CR
   or CRLF, and 0 otherwise. Return 2 on error. */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../src/platform.h"

int checkbin(FILE *f) {
  int c;

  while (1) {
    c = fgetc(f);
    if (c == EOF) {
      return 0;
    } else if (c == '\n') {
      return 0;
    } else if (c == '\r') {
      return 1;
    }
  }
}

int main(int ac, char **av) {
  char *file;
  FILE *f;
  int r;

  platform_init();

  if (ac != 2) {
    fprintf(stderr, "checkbin: wrong number of arguments\n");
    fprintf(stderr, "Usage: checkbin file\n");
    return 2;
  }

  file = av[1];
  if (strcmp(file, "-") == 0) {
    f = stdin;
  } else {
    f = fopen(file, "rb");
    if (!f) {
      fprintf(stderr, "checkbin: %s: %s\n", file, strerror(errno));
      return 2;
    }
  }
  r = checkbin(f);
  if (strcmp(file, "-") == 0) {
    /* nothing */
  } else {
    fclose(f);
  }
  return r;
}
  
