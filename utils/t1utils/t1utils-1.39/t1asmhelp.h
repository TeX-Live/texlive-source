#ifndef T1ASMHELP_H
#define T1ASMHELP_H

static int lenIV = 4;

/* If the line contains an entry of the form `/lenIV <num>' then set the global
   lenIV to <num>.  This indicates the number of random bytes at the beginning
   of each charstring. */

static void
set_lenIV(const char* line)
{
  char *p = strstr(line, "/lenIV ");

  /* Allow lenIV to be negative. Thanks to Tom Kacvinsky <tjk@ams.org> */
  if (p && (isdigit((unsigned char) p[7]) || p[7] == '+' || p[7] == '-')) {
    lenIV = atoi(p + 7);
  }
}


static const char* cs_start = "";

static void
set_cs_start(const char* line)
{
    static int cs_start_set = 0;
    char *p, *q, *r;

    if ((p = strstr(line, "string currentfile"))
        && strstr(line, "readstring")) {
        /* locate the name of the charstring start command */
        for (q = p; q != line && q[-1] != '/'; --q)
            /* nada */;
        if (q != line) {
            for (r = q; r != p && !isspace((unsigned char) *r) && *r != '{'; ++r)
                /* nada */;
            if (cs_start_set)
                free((char*) cs_start);
            cs_start = p = malloc(r - q + 1);
            memcpy(p, q, r - q);
            p[r - q] = 0;
            cs_start_set = 1;
        }
    }
}

#endif
