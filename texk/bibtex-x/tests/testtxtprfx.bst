ENTRY {field}{}{}

FUNCTION {output_entry} {
  "\item[orig] " field * write$ newline$
  "\item[len] " field text.length$ int.to.str$ * write$ newline$
  "\item[1] " field #1 text.prefix$ * write$ newline$
  "\item[2] " field #2 text.prefix$ * write$ newline$
  "\item[3] " field #3 text.prefix$ * write$ newline$
  "\item[4] " field #4 text.prefix$ * write$ newline$
  "\item[5] " field #5 text.prefix$ * write$ newline$
  "\item[6] " field #6 text.prefix$ * write$ newline$
  "\item[7] " field #7 text.prefix$ * write$ newline$
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
