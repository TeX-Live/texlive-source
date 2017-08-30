#ifndef T1ASMHELP_H
#define T1ASMHELP_H

static int lenIV = 4;

/* If the line contains an entry of the form `/lenIV <num>' then set the global
   lenIV to <num>.  This indicates the number of random bytes at the beginning
   of each charstring. */

#define memmem my_memmem

static void
set_lenIV(const char* line, size_t line_len)
{
  char* p = memmem(line, line_len, "/lenIV ", 7);

  /* Allow lenIV to be negative. Thanks to Tom Kacvinsky <tjk@ams.org> */
  if (p && p + 7 < line + line_len) {
    const char* x = p + 7 + (p[7] == '+' || p[7] == '-');
    if (x < line + line_len && isdigit((unsigned char) *x)) {
      lenIV = (unsigned char) *x - '0';
      for (++x; x < line + line_len && isdigit((unsigned char) *x); ++x)
        lenIV = 10 * lenIV + (unsigned char) *x - '0';
      if (p[7] == '-')
        lenIV = -lenIV;
    }
  }
}


static char cs_start_init[] = "";
static char *cs_start = cs_start_init;

static void
set_cs_start(const char* line, size_t line_len)
{
    static int cs_start_set = 0;
    char *p, *q, *r;

    if ((p = memmem(line, line_len, "string currentfile", 18))
        && memmem(line, line_len, "readstring", 10)) {
        /* locate the name of the charstring start command */
        for (q = p; q != line && *(q-1) != '/'; --q)
            /* nada */;
        if (q != line) {
            for (r = q; r != p && !isspace((unsigned char) *r) && *r != '{'; ++r)
                /* nada */;
            if (cs_start_set)
                free(cs_start);
            cs_start = p = malloc(r - q + 1);
            memcpy(p, q, r - q);
            p[r - q] = 0;
            cs_start_set = 1;
        }
    }
}

#endif
