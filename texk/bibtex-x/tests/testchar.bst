ENTRY {field}{}{}

FUNCTION {output_entry} {
  "\item "    field * write$
  "  i2str: " field chr.to.int$ int.to.str$ * write$
  "  i2chr: " field chr.to.int$ int.to.chr$ * write$
                 newline$
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
