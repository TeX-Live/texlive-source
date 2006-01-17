(TeX-add-style-hook "rotsample"
 (function
  (lambda ()
    (TeX-add-symbols
     "foo")
    (TeX-run-style-hooks
     "rotating"
     "graphicx"
     "latex2e"
     "art10"
     "article"))))

