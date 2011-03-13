;; $Id: dump-xindy.xdy,v 1.2 1996/08/02 17:41:07 kehr Exp $
;; ----------------------------------------------------------------------
;;
;; This file is necessary to dump xindy.
;;

#+CLISP (setq custom:*suppress-check-redefinition* t)

(load 'base)
(load 'locref)
(load 'ordrules)
(load 'idxstyle)
(load 'index)
(load 'version)
(load 'markup)

(load "defaults.xdy")

(setq *load-verbose* nil)

(shadow 'require)
(lisp:shadowing-import idxstyle:*idxstyle-user-interface-definitions*)
(lisp:import markup:*markup-user-interface-definitions*)
(lisp:import index:*raw-index-interface-definitions*)

;; Install the break-driver if its feature exists.
#+:BREAK-DRIVER
(setq *break-driver* #'xindy:*break-driver*)

(gc)

(saveinitmem "xindy.mem" :quiet t)
(exit)
