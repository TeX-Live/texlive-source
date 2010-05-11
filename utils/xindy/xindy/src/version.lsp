;; $Id$ -*- Lisp -*-
;; ------------------------------------------------------------
;; version identification of xindy kernel
;; src/version.lsp.

(lisp:provide "version")

(defpackage :xindy-version
    (:export *xindy-kernel-version*))

(in-package :xindy-version)

(defconstant *xindy-kernel-version* "3.0")
