% BibTeX test style file
% Copyright 2022 TANAKA Takuji <ttk@t-lab.opal.ne.jp>
% You may freely use, modify and/or distribute this file.

ENTRY {author}{}{}

STRINGS { s }

INTEGERS { nameptr namesleft numnames }

FUNCTION {format.names}
{ 's :=
  #1 'nameptr :=
  s num.names$ 'numnames :=
  numnames 'namesleft :=
    { namesleft #0 > }
    { " [" nameptr int.to.str$ * "]" * write$
      " {ff} " s nameptr "{ff}" format.name$ * write$
      " {ll} " s nameptr "{ll}" format.name$ * write$
      " {vv} " s nameptr "{vv}" format.name$ * write$
      " {jj} " s nameptr "{jj}" format.name$ * write$ newline$
      " | " s nameptr "{ff}{vv}{ll}{jj}" format.name$ * write$
      " | " s nameptr "{ff }{vv }{ll}{ jj}" format.name$ * write$
      " | " s nameptr "{vv~}{ll}{, jj}{, f.}" format.name$ * write$
      " | " s nameptr "{f.~}{v.~}{ll}{, jj}" format.name$ * write$
           newline$
      nameptr #1 + 'nameptr :=
      namesleft #1 - 'namesleft :=
    }
  while$
}

FUNCTION {output_entry} {
  "\item "    author * write$ newline$
    author format.names
}

FUNCTION {book} {output_entry}

READ

FUNCTION {set_sort_key} {
  author 'sort.key$ :=
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
