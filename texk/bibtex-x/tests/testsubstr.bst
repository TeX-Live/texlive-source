ENTRY {field}{}{}

FUNCTION {output_entry} {
  "\item[orig] " field * write$ newline$
  "\item[1 1] " field #1 #1 substring$ * write$ newline$
  "\item[1 2] " field #1 #2 substring$ * write$ newline$
  "\item[1 3] " field #1 #3 substring$ * write$ newline$
  "\item[1 4] " field #1 #4 substring$ * write$ newline$
  "\item[1 5] " field #1 #5 substring$ * write$ newline$
  "\item[1 6] " field #1 #6 substring$ * write$ newline$
  "\item[1 7] " field #1 #7 substring$ * write$ newline$
  "\item[1 1] " field #1 #1 substring$ * write$ newline$
  "\item[2 1] " field #2 #1 substring$ * write$ newline$
  "\item[3 1] " field #3 #1 substring$ * write$ newline$
  "\item[4 1] " field #4 #1 substring$ * write$ newline$
  "\item[5 1] " field #5 #1 substring$ * write$ newline$
  "\item[6 1] " field #6 #1 substring$ * write$ newline$
  "\item[7 1] " field #7 #1 substring$ * write$ newline$
  "\item[-1 1] " field #-1 #1 substring$ * write$ newline$
  "\item[-1 2] " field #-1 #2 substring$ * write$ newline$
  "\item[-1 3] " field #-1 #3 substring$ * write$ newline$
  "\item[-1 4] " field #-1 #4 substring$ * write$ newline$
  "\item[-1 5] " field #-1 #5 substring$ * write$ newline$
  "\item[-1 6] " field #-1 #6 substring$ * write$ newline$
  "\item[-1 7] " field #-1 #7 substring$ * write$ newline$
  "\item[-2 7] " field #-2 #7 substring$ * write$ newline$
  "\item[-3 7] " field #-3 #7 substring$ * write$ newline$
  "\item[-4 7] " field #-4 #7 substring$ * write$ newline$
  "\item[-5 7] " field #-5 #7 substring$ * write$ newline$
  "\item[-6 7] " field #-6 #7 substring$ * write$ newline$
  "\item[-7 7] " field #-7 #7 substring$ * write$ newline$
  "\item[-1 7] " field #-1 #7 substring$ * write$ newline$
  "\item[-2 6] " field #-2 #6 substring$ * write$ newline$
  "\item[-3 5] " field #-3 #5 substring$ * write$ newline$
  "\item[-4 4] " field #-4 #4 substring$ * write$ newline$
  "\item[-5 3] " field #-5 #3 substring$ * write$ newline$
  "\item[-6 2] " field #-6 #2 substring$ * write$ newline$
  "\item[-7 1] " field #-7 #1 substring$ * write$ newline$
  "\item[-1 8] " field #-1 #8 substring$ * write$ newline$
  "\item[-2 7] " field #-2 #7 substring$ * write$ newline$
  "\item[-3 6] " field #-3 #6 substring$ * write$ newline$
  "\item[-4 5] " field #-4 #5 substring$ * write$ newline$
  "\item[-5 4] " field #-5 #4 substring$ * write$ newline$
  "\item[-6 3] " field #-6 #3 substring$ * write$ newline$
  "\item[-7 2] " field #-7 #2 substring$ * write$ newline$
  "\item[-8 1] " field #-7 #1 substring$ * write$ newline$
}

FUNCTION {type} {output_entry}

READ

FUNCTION {set_sortkey} {
  field 'sort.key$ :=
}

ITERATE {set_sortkey}

SORT

FUNCTION {output_beg} {
  "\section*{Test result}" write$ newline$
  "\begin{itemize}" write$ newline$
}

FUNCTION {output_end} {
  "\end{itemize}" write$ newline$
  "\endinput" write$ newline$
}

EXECUTE {output_beg}

ITERATE {call.type$}

EXECUTE {output_end}
