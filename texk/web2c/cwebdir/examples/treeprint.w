\def\covernote{Copyright 1987 Norman Ramsey -- Princeton University}

\def\vbar{\.{|}}
@*Directory Trees.
Our object is to print out a directory hierarchy in some pleasant way.
The program takes output from {\tt find * -type d -print \vbar\ sort}
@^system dependencies@>
and produces a nicer-looking listing.
More precisely, our input, which is the output of {\tt find} followed
by {\tt sort}, is a list of fully qualified directory names (parent
and child separated by slashes |'/'|); everything has already been
sorted nicely into lexicographic order.

The {\tt treeprint} routine takes one option, |"-p"|, which tells it
to use the printer's line-drawing set, rather than the terminal's.

@c
@<Global definitions@>@;
@<Global include files@>@;
@<Global declarations@>@;

@#
main(argc, argv)
     int argc;
     char **argv;
{
@<|main| variable declarations@>;
@<Search for options and set special characters on |"-p"|@>;
@<Read output from find and enter into tree@>;
@<Write tree on standard output@>@;
exit(0);
}

@
We make all the siblings of a directory a linked list off of its left child, 
and the offspring a linked list off the right side.
Data are just directory names.
@d sibling left
@d child right

@<Global decl...@>=
typedef struct tnode {
  struct tnode *left, *right;
  char *data;
} TNODE;
@ @<|main| variable...@>=
struct tnode *root=NULL;



@*Input.
Reading the tree is simple---we read one line at a time, and call on the 
recursive |add_tree| procedure.

@c
read_tree (fp, rootptr)
   FILE *fp;
   struct tnode **rootptr;
{
   char buf[255], *p;

   while ((fgets(buf, 255, fp))!=NULL) {
     @<If |buf| contains a newline, make it end there@>;
     add_tree(rootptr, buf);
   }
 }
@ @<Global include...@>=
#include <stdio.h>

@ Depending what system you're on, you may or may not get a newline in |buf|.
@<If |buf| contains a newline...@>=
     p=buf; while (*p!='\0'&&*p!='\n') p++;
@^system dependencies@>
     *p='\0';

@ 
To add a string, we split off the first part of the name and insert it into 
the sibling list. We then do the rest of the string as a child of the new node.

@c
add_tree(rootptr, p)
     struct tnode **rootptr;
     char *p;
{
     char *s;
     int slashed;

     if (*p=='\0') return;

@<Break up the string so |p| is the first word,
     |s| points at null-begun remainder,
    and |slashed| tells whether |*s=='/'| on entry@>;

     if (*rootptr==NULL) {
@<Allocate new node to hold string of size |strlen(p)|@>;
       strcpy((*rootptr)->data,p);
     } 
     if (strcmp((*rootptr)->data,p)==0) {
           if (slashed) ++s;
           add_tree(&((*rootptr)->child),s);
	 }
       else {
           if (slashed) *s='/';
           add_tree(&((*rootptr)->sibling),p);
	 }
   }

@ We perform some nonsense to cut off the string |p| so that |p| just
holds the first word of a multiword name. Variable |s| points at what
was either the end of |p| or a slash delimiting names. In either case
|*s| is made |'\0'|.  Later, depending on whether we want to pass the
whole string or the last piece, we will restore the slash or advance
|s| one character to the right.

@<Break up...@>=
     for (s=p;*s!='\0'&&*s!='/';) s++;
     if (*s=='/') {
       slashed=1;
       *s='\0';
     } else slashed=0;

@ Node allocation is perfectly standard \dots
@<Allocate new node...@>=
       *rootptr=(struct tnode *) malloc (sizeof(struct tnode));
       (*rootptr)->left = (*rootptr)->right = NULL;
       (*rootptr)->data = malloc (strlen(p)+1);

@
@<Global decl...@>= char *malloc();

@ In this simple implementation, we just read from standard input.
@<Read...@>= read_tree(stdin,&root);

@*Output.
We begin by defining some lines, tees, and corners.
The |s| stands for screen and the |p| for printer.
You will have to change this for your line-drawing set.
@^system dependencies@>

@<Global definitions@>=
#define svert '|'
#define shoriz '-'
#define scross '+'
#define scorner '\\' /* lower left corner */

#define pvert '|'
#define phoriz '-'
#define pcross '+'
#define pcorner '\\' /* lower left corner */

@ The default is to use the terminal's line drawing set.
@<Global declarations@>=
char vert=svert;
char horiz=shoriz;
char cross=scross;
char corner=scorner;

@ With option |"-p"| use the printer character set.
@<Search for options...@>=
while (--argc>0) {
  if (**++argv=='-') {
    switch (*++(*argv)) {
      case 'p': 
        vert=pvert;
        horiz=phoriz;
        cross=pcross;
        corner=pcorner;
	break;
      default:
        fprintf(stderr,"treeprint: bad option -%c\n",**argv);
	break;
      }
  }
}

@ We play games with a character stack to figure out when to put in vertical
bars.
A vertical bar connects every sibling with its successor, but the last sibling
in a list is followed by blanks, not by vertical bars. The state of 
bar-ness or space-ness for each preceding sibling is recorded in the 
|indent_string| variable, one character (bar or blank) per sibling.

@<Global decl...@>=
char indent_string[100]="";

@  Children get printed 
before siblings.
We don't bother trying to bring children up to the same line as their parents,
because the \UNIX/ filenames are so long.

We define a predicate telling us when a sibling is the last in a series.
@d is_last(S) (S->sibling==NULL)

@c
print_node(fp, indent_string, node)
     FILE *fp;
     char *indent_string;
     struct tnode *node;
{
  char string[255];
     int i;
     char *p, *is;

  if (node==NULL) {
  }
  else {
    *string='\0';
    for (i=strlen(indent_string); i>0; i--) 
      strcat(string,@,      " |  ");
    strcat(string,@t\ \ @>  " +--");
@<Replace chars in |string| with chars from 
         line-drawing set and from |indent_string|@>;
    fprintf(fp,"%s%s\n",string,node->data);

@#
    /* Add vertical bar or space for this sibling (claim |*is=='\0'|) */
    *is++ = (is_last(node) ? ' ' : vert);
    *is='\0';
   
    print_node(fp, indent_string, node->child); /* extended |indent_string| */
    *--is='\0';
    print_node(fp, indent_string, node->sibling); /* original |indent_string| */
  }

}
@ For simplicity, we originally wrote connecting lines with |'|'|, |'+'|, and
|'-'|.
Now we replace those characters with appropriate characters from the 
line-drawing set. 
We take the early vertical bars and replace them with characters from
|indent_string|, and we replace the other characters appropriately.
We are sure to put a |corner|, not a |cross|, on the last sibling in 
a group.
@<Replace chars...@>=
    is=indent_string;
    for (p=string; *p!='\0'; p++) switch(*p) {
       case '|': *p=*is++; break;
       case '+': *p=(is_last(node) ? corner : cross); break;
       case '-': *p=horiz; break;
       default: break;
	       }


@ For this simple implementation, we just write on standard output.

@<Write...@>= print_node(stdout, indent_string, root);

@*Index.
