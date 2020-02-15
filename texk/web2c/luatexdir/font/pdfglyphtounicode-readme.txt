In pdftex there is are some more heuristics going on when determining the
tounicode mapping:

- more lookups using periods
- a prefix tfm:cmr10/foo -> bar mapping

Because in luatex one can have a callback that just loads the tfm and then
decorates it with tounicodes we don't do this in luatex.

HH
