ENTRY {field}{}{}

FUNCTION {output_entry} {
  "\item[~~] " field *
  write$ newline$
  "\item[ap] " field add.period$ *
  write$ newline$
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
