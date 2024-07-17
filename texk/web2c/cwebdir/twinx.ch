Changes for the TWINX utility from the CTWILL tarball.

This minimal set of changes tries to satisfy the GCC compiler
and it fixes a few minor issues. See the comments after '@x'.

This file is not copyrighted and can be used freely.

Limbo.

@x l.1
\datethis
@y
\datethis
\let\maybe=\iffalse % print only sections that change
@z

Section 1.

@x l.10 Standard C interface.
#include <stdio.h>
@y
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
@z

@x l.15
main(argc,argv)
  int argc;
  char *argv[];
@y
int main(
  int argc,
  char *argv[])
@z

@x l.21
    f=fopen(*++argv,"r");
    if (!f)
@y
    if((f=fopen(*++argv,"r"))==NULL)
@z

@x l.27
      strncpy(*argv+strlen(*argv)-3,"idx",3);
      f=fopen(*argv,"r");
      if (!f)
@y
      memcpy(*argv+strlen(*argv)-3,"idx",3);
      if((f=fopen(*argv,"r"))==NULL)
@z

Section 3.

@x l.49 Document minor change in behavior.
@ @<Scan file |f|...@>=
@y
@ For your convenience, \.{TWINX} grabs the first ``word'' in \.{\\title} and
turns it into uppercase form.

@<Scan file |f|...@>=
@z

@x l.56 C++17 removed 'register' storage class.
  if (strncmp(buf,"\\def\\title\{",11)==0) {@+register char *p,*q;
@y
  if (strncmp(buf,"\\def\\title\{",11)==0) {@+char *p,*q;
@z

@x l.57 FIX: Fetch only the first word from the '\title'.
    for (p=buf+11,q=title;*p && *p!='}';p++) *q++=*p;
@y
    for (p=buf+11,q=title;*p&&*p!=' '&&*p!='}';p++) *q++=toupper(*p);
@z

Section 4.

@x l.96
@<Type...@>=
@y
@s node_struct int

@<Type...@>=
@z

@x l.102 Compiler warning.
  char *id;
@y
  const char *id;
@z

Section 5.

@x l.115
char *save_string(s)
  char *s;
@y
char *save_string(
  char *s)
@z

@x l.118 C++17 removed 'register' storage class.
  register char *p,*q; register int l;
@y
  char *p,*q; int l;
@z

Section 6.

@x l.145
node *new_node()
@y
node *new_node(void)
@z

@x l.181 C++17 removed 'register' storage class.
register node* main_node; /* current end of main list */
@y
node* main_node; /* current end of main list */
@z

@x l.195 C++17 removed 'register' storage class.
while (1) {@+register node *cur_node;
@y
while (1) {@+node *cur_node;
@z

@x l.213 C++17 removed 'register' storage class.
{@+register char *p,*q;@+register int bal=1;
@y
{@+char *p,*q;@+int bal=1;
@z

Section 11.

@x l.216 FIX: Don't count masked braces.
    if (*p=='{') bal++;
    else if (*p=='}') bal--;
@y
    switch (*p) {
    case '\\': *q++=*p++; break;
    case '{': bal++; break;
    case '}': bal--; break;
    }
@z

Section 12.

@x l.246
  do@+{
@y
  do {
@z

@x l.256
  }@+while(fgets(buf,buf_size,f));
@y
  } while(fgets(buf,buf_size,f));
@z

Section 14.

@x l.289 C++17 removed 'register' storage class.
while (1) {@+register node *p,*q,*r,*s,*t;
@y
while (1) {@+node *p,*q,*r,*s,*t;
@z

Section 15.

@x l.308 C++17 removed 'register' storage class.
do@+{@+register int d;
@y
do {@+int d;
@z

@x l.316
}@+while(1);
@y
} while(1);
@z

Section 16.

@x l.323 C++17 removed 'register' storage class.
do@+{@+register int d;
@y
do {@+int d;
@z

@x l.338
}@+while(1);
@y
} while(1);
@z

Section 17.

@x l.347
int compare(p,q)
  node *p,*q;
@y
int compare(
  node *p, node *q)
@z


@x l.349 C++17 removed 'register' storage class.
{@+register unsigned char *pp,*qq;
@y
{@+unsigned char *pp,*qq;
@z

Section 19.

@x l.378 C++17 removed 'register' storage class.
{@+register int j;
@y
{@+int j;
@z

@x l.379 Compiler warning.
  for (j=1;collate[j];j++) ord[collate[j]]=j;
@y
  for (j=1;collate[j];j++) ord[(int)collate[j]]=j;
@z

Section 20.

@x l.390
collapse(p,q)
  node *p,*q;
@y
void collapse(
  node *p, node *q)
@z

@x l.392 C++17 removed 'register' storage class.
{@+register node *x;
@y
{@+node *x;
@z

Section 21.

@x l.403 C++17 removed 'register' storage class.
{@+register node *x;
@y
{@+node *x;
@z

Section 22.

@x l.414 Compiler warning.
{@+register char *p=x->id;
@y
{@+const char *p=x->id;
@z

@x l.434 FIX: Don't mask already masked underscore.
    if (*p=='_') putchar('\\');
@y
    if (*p=='_'&&*(p-1)!='\\') putchar('\\');
@z

@x l.441 C++17 removed 'register' storage class.
{@+register node *y=x->data.n,*z=NULL;
  while (y) {@+register node *w;
@y
{@+node *y=x->data.n,*z=NULL;
  while (y) {@+node *w;
@z
