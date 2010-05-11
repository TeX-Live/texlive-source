;; $Id: ordrulei.lsp,v 1.5 2005/05/02 21:39:53 jschrod Exp $
;; ------------------------------------------------------------

;;;
;;; Interface definition of the ordrules module
;;;

(defpackage :ordrules
  (:use :cl :ffi)
  (:export :initialize
	   :add-keyword-sort-rule :add-keyword-merge-rule
	   :gen-keyword-sortkey :gen-keyword-mergekey))

(in-package :ordrules)

(c-lines "#include \"ordrules.h\"~%")

; Common OS definitions:
(def-c-type size_t uint)

(def-c-var *string-buffer*
    (:name "ordrules_string_buffer")
    (:type (c-array char 1024))
    (:alloc :NONE))

(def-c-var *string-buffer-used-bytes*
    (:name "ordrules_string_buffer_used_bytes")
    (:type int)
    (:alloc :NONE))

(def-c-var *string-buffer-used-bytes*
    (:name "ordrules_string_buffer_used_bytes")
    (:type int)
    (:alloc :NONE))

(def-c-var *message-logging*
    (:name "ordrules_msg_logging")
    (:type int)
    (:alloc :NONE))

(def-c-var *message-buffer*
    (:name "ordrules_msg_buffer")
    (:type c-string)
    (:alloc :NONE))

(def-c-var *message-buffer-ptr*
    (:name "ordrules_msg_buffer_ptr")
    (:type int)
    (:alloc :NONE))

(def-call-out initialize
    (:language :stdc)
    (:name "initialize")
    (:arguments (num-sort-tables int))
    (:return-type int))

(def-call-out add-keyword-sort-rule
    (:language :stdc)
    (:name "add_sort_rule")
    (:arguments (run int)
		(left c-string)
		(right c-string)
		(isreject int)
		(ruletype int))
    (:return-type int))

(def-call-out add-keyword-merge-rule
    (:language :stdc)
    (:name "add_merge_rule")
    (:arguments (left c-string)
		(right c-string)
		(isreject int)
		(ruletype int))
    (:return-type int))

(def-call-out gen-keyword-sortkey
    (:language :stdc)
    (:name "gen_sortkey")
    (:arguments (key c-string)
		(run int))
    (:return-type c-string :malloc-free))

(def-call-out gen-keyword-mergekey
    (:language :stdc)
    (:name "gen_mergekey")
    (:arguments (key c-string))
    (:return-type c-string :malloc-free))

#|

  $Log: ordrulei.lsp,v $
  Revision 1.5  2005/05/02 21:39:53  jschrod
      xindy run time engine 3.0; as used for CLISP 2.33.2.

  Revision 1.4  1997/10/20 11:23:12  kehr
  New version of sorting rules. Sorting of more complex indexes (i.e.
  French) is now possible.

  Revision 1.3  1997/01/17 16:43:38  kehr
  Several changes for new version 1.1.

  Revision 1.2  1996/04/30  15:56:38  kehr
  Renamed some of the functions to avoid conflicts with other functions
  in other packages (only for the sake of convenience).

  Revision 1.1  1996/03/27  20:29:07  kehr
  It works. Today I had my first success in getting the FFI running with
  the ordrules-library. The interface is defined in `ordrulei.lsp' and
  allows direct access to the functions in `ordrules.c'.

|#
