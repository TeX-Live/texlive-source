Limbo.

@x l.1
\datethis
@s try x
@y
\datethis
\def\title{HAM}
@z

Section 1.

@x l.15
@d deg u.I
@y
@d deg u.I
@-deg@>
@$deg {HAM}1 =\|u.\|I@>
@%
@$u {GB\_\,GRAPH}9 \&{util}@>
@z

@x l.21
main(int argc,char *argv[])
@y
@#@;@q Fix a bug in CWEB/CTWILL 4.3 @>
int main(int argc,char *argv[])
@z

@x l.33
  for (v=g->vertices;v<g->vertices+g->n;v++) printf(" %d",v->deg);
@y
  for (v=g->vertices;v<g->vertices+g->n;v++) printf(" %ld",v->deg);
@z

@x l.36
    printf("The minimum degree is %d (vertex %s)!\n",x->deg,x->name);
@y
    printf("The minimum degree is %ld (vertex %s)!\n",x->deg,x->name);
@z

@x l.46
  for (v=g->vertices;v<g->vertices+g->n;v++) printf(" %d",v->deg);
@y
  for (v=g->vertices;v<g->vertices+g->n;v++) printf(" %ld",v->deg);
@z

Section 2.

@x l.50
@ @<Process the command line, inputting the graph@>=
@y
@r @ @<Process the command line, inputting the graph@>=
@z

Section 3.

@x
@ Vertices that have already appeared in the path are ``taken,'' and
@y
@ Vertices that have already appeared in the path are ``taken,'' and
@-taken@>
@$taken {HAM}3 =\|v.\|I@>
@%
@$v {HAM}1 \&{register} \&{Vertex} ${}{*}{}$@>
@$v {GB\_\,GRAPH}9 \&{util}@>
@$I {GB\_\,GRAPH}8 \&{long}@>
@z

Section 4.

@x l.85
I may learn my lesson.
@y
I may learn my lesson.
@-k@>
@-vert@>
@-ark@>
@$vert {HAM}4 =\|w.\|V@>
@$ark {HAM}4 =\|x.\|A@>
@%
@$w {GB\_\,GRAPH}9 \&{util}@>
@$x {GB\_\,GRAPH}9 \&{util}@>
@$x {HAM}1 \&{register} \&{Vertex} ${}{*}{}$@>
@z

@x l.92
x->taken=1;@+t->vert=x;
@y
x->taken=1;@+t->vert=x;@+
@z

@x l.97
try: @<Look at edge |a| and its successors, advancing if it is a valid move@>;
@y
try_next: @<Look at edge |a| and its successors, advancing if it is a valid move@>;
@z

Section 8.

@x l.148
  goto try;
@y
  goto try_next;
@z

Section 10.

@x l.163
@*Index.
@y
@z
