% BibTeX test style file
% Copyright 2022 TANAKA Takuji <ttk@t-lab.opal.ne.jp>
% You may freely use, modify and/or distribute this file.

ENTRY {field}{}{}

FUNCTION {output_entry} {
  "\item[orig] " field * write$ newline$
  "\item[1 5] " field #1 #5 substring$ * write$ newline$
  "\item[1 4] " field #1 #4 substring$ * write$ newline$
  "\item[1 3] " field #1 #3 substring$ * write$ newline$
  "\item[1 2] " field #1 #2 substring$ * write$ newline$
  "\item[1 1] " field #1 #1 substring$ * write$ newline$
  "\item[2 1] " field #2 #1 substring$ * write$ newline$
  "\item[3 1] " field #3 #1 substring$ * write$ newline$
  "\item[4 1] " field #4 #1 substring$ * write$ newline$
  "\item[5 1] " field #5 #1 substring$ * write$ newline$
  "\item[1 5] " field #1 #5 substring$ * write$ newline$
  "\item[2 5] " field #2 #5 substring$ * write$ newline$
  "\item[3 5] " field #3 #5 substring$ * write$ newline$
  "\item[4 5] " field #4 #5 substring$ * write$ newline$
  "\item[5 5] " field #5 #5 substring$ * write$ newline$
  "\item[1 5] " field #1 #5 substring$ * write$ newline$
  "\item[2 4] " field #2 #4 substring$ * write$ newline$
  "\item[3 3] " field #3 #3 substring$ * write$ newline$
  "\item[4 2] " field #4 #2 substring$ * write$ newline$
  "\item[5 1] " field #5 #1 substring$ * write$ newline$
  "\item[1 8] " field #1 #8 substring$ * write$ newline$
  "\item[2 7] " field #2 #7 substring$ * write$ newline$
  "\item[3 6] " field #3 #6 substring$ * write$ newline$
  "\item[4 5] " field #4 #5 substring$ * write$ newline$
  "\item[5 4] " field #5 #4 substring$ * write$ newline$
  "\item[6 3] " field #6 #3 substring$ * write$ newline$
  "\item[7 2] " field #7 #2 substring$ * write$ newline$
  "\item[8 1] " field #8 #1 substring$ * write$ newline$
  "\item[-1 5] " field #-1 #5 substring$ * write$ newline$
  "\item[-1 4] " field #-1 #4 substring$ * write$ newline$
  "\item[-1 3] " field #-1 #3 substring$ * write$ newline$
  "\item[-1 2] " field #-1 #2 substring$ * write$ newline$
  "\item[-1 1] " field #-1 #1 substring$ * write$ newline$
  "\item[-2 1] " field #-2 #1 substring$ * write$ newline$
  "\item[-3 1] " field #-3 #1 substring$ * write$ newline$
  "\item[-4 1] " field #-4 #1 substring$ * write$ newline$
  "\item[-5 1] " field #-5 #1 substring$ * write$ newline$
  "\item[-1 5] " field #-1 #5 substring$ * write$ newline$
  "\item[-2 5] " field #-2 #5 substring$ * write$ newline$
  "\item[-3 5] " field #-3 #5 substring$ * write$ newline$
  "\item[-4 5] " field #-4 #5 substring$ * write$ newline$
  "\item[-5 5] " field #-5 #5 substring$ * write$ newline$
  "\item[-1 5] " field #-1 #5 substring$ * write$ newline$
  "\item[-2 4] " field #-2 #4 substring$ * write$ newline$
  "\item[-3 3] " field #-3 #3 substring$ * write$ newline$
  "\item[-4 2] " field #-4 #2 substring$ * write$ newline$
  "\item[-5 1] " field #-5 #1 substring$ * write$ newline$
  "\item[-1 8] " field #-1 #8 substring$ * write$ newline$
  "\item[-2 7] " field #-2 #7 substring$ * write$ newline$
  "\item[-3 6] " field #-3 #6 substring$ * write$ newline$
  "\item[-4 5] " field #-4 #5 substring$ * write$ newline$
  "\item[-5 4] " field #-5 #4 substring$ * write$ newline$
  "\item[-6 3] " field #-6 #3 substring$ * write$ newline$
  "\item[-7 2] " field #-7 #2 substring$ * write$ newline$
  "\item[-8 1] " field #-8 #1 substring$ * write$ newline$
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
