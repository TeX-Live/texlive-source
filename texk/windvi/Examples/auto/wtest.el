(TeX-add-style-hook "wtest"
 (function
  (lambda ()
    (LaTeX-add-labels
     "fig:f7")
    (TeX-add-symbols
     '("XX" 2)
     "WDVI"
     "HR")
    (TeX-run-style-hooks
     "xy"
     "graphicx"
     "pstcol"
     "pst-node"
     "pst-tree"
     "graphics"
     "pspicture"
     "epic"
     "eepic"
     "color"
     "latex2e"
     "art11"
     "article"
     "a4paper"
     "11pt"))))

