% BibTeX test style file
% Copyright 2022 TANAKA Takuji <ttk@t-lab.opal.ne.jp>
% You may freely use, modify and/or distribute this file.

ENTRY {field}{}{}

FUNCTION {output_entry} {
  "\item "    field * write$
  "  i2str: " field chr.to.int$ int.to.str$ * write$
  "  i2chr: " field chr.to.int$ int.to.chr$ * write$
                 newline$
}

FUNCTION {type} {output_entry}

READ

FUNCTION {set_sort_key} {
  field 'sort.key$ :=
}

ITERATE {set_sort_key}

SORT

FUNCTION {output_bgn} {
  "\section*{Test result}" write$ newline$
  "\begin{itemize}" write$ newline$
}

FUNCTION {output_end} {
  "\end{itemize}" write$ newline$
  "\endinput" write$ newline$
}

EXECUTE {output_bgn}

ITERATE {call.type$}

EXECUTE {output_end}
