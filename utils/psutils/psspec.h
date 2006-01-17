/* psspec.h
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * routines for page rearrangement specs
 */

/* pagespec flags */
#define ADD_NEXT (0x01)
#define ROTATE   (0x02)
#define SCALE    (0x04)
#define OFFSET   (0x08)
#define GSAVE    (ROTATE|SCALE|OFFSET)

typedef struct pagespec {
   int reversed, pageno, flags, rotate;
   double xoff, yoff, scale;
   struct pagespec *next;
} PageSpec ;

extern double width, height;

extern PageSpec *newspec(void);
extern int parseint(char **sp, void (*errorfn)(void));
extern double parsedouble(char **sp, void (*errorfn)(void));
extern double parsedimen(char **sp, void (*errorfn)(void));
extern double singledimen(char *str, void (*errorfn)(void),
			  void (*usagefn)(void));
extern void pstops(int modulo, int pps, int nobind, PageSpec *specs,
		   double draw);
