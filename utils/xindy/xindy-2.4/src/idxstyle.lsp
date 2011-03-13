;; line 289 "idxstyle.nw"
;; $Id: idxstyle.nw,v 1.19 1997/03/26 16:18:23 kehr Exp $
;;

(lisp:defpackage "IDXSTYLE")
(lisp:in-package "IDXSTYLE")
(lisp:provide "idxstyle")

#+CLISP (lisp:require "base")
#+CLISP (lisp:require "locref")
#+CLISP (lisp:require "ordrules")

(eval-when (compile load eval)
  (lisp:use-package "CLOS")
  (lisp:use-package "COMMON-LISP")
  #+CLISP (lisp:use-package "EXT")
  #+(and :XP CLISP) (lisp:use-package "XP")
  #-CLISP (lisp:require "base")
  #-CLISP (lisp:require "locref")
  (lisp:use-package "BASE")
  (lisp:use-package "LOCREF")

;; line 472 "intface.nw"
(eval-when (eval load compile)
  (when (find-package "ORDRULES")
    (pushnew :ORDRULES *features*)))
;; line 306 "idxstyle.nw"
                                            ;; see submodule intface
  )

;; line 78 "idxstyle.nw"
(defparameter *indexstyle* nil)
(defparameter *indexstyle-readtable* (copy-readtable))
(defparameter *included-files* '())
(defvar *load-paths* "")
(defparameter *default-load-paths* *load-paths*)

(defparameter *default-minimum-range-length* 2)
(defparameter *default-locref-hierdepth*     0)

;; line 1151 "intface.nw"
;; $Id: intface.nw,v 1.30 1997/10/20 11:25:09 kehr Exp $

;; line 1016 "intface.nw"
;;; $Id: intface.nw,v 1.30 1997/10/20 11:25:09 kehr Exp $
;;;----------------------------------------------------------------------

;;;
;;; util-lib                    LISP Utitlity Library
;;;
;;; (history at end)


;(provide 'util-lib)
;(in-package 'util-lib)

(defvar whole)
(eval-when (compile load eval)
  (export '(destructuring-switch-bind &switch whole)))

(defmacro destructuring-switch-bind (lambdalist expr . body)
  "Similar to 'destructuring-bind' but also allows switch options at the
end of LAMBDALIST with the following syntax:

  &switch { var | (var key [defval]) | (var ({ key }*) [defval]) }*

In all cases VAR will be accessible in BODY as a variable.

If only VAR is specified, it is bound to the keyword with the same
name, if that keyword occured in EXPR. Otherwise it is bound to NIL.

If both a VAR and KEY were specified, VAR will be bound to KEY, if KEY
occured in EXPR and to NIL otherwise.

If VAR and a list of KEYs were specified, VAR will be bound to the
first KEY, which occured in EXPR. If none of the KEYS occured within
EXPR, VAR will be bound to NIL.

If DEFVAL was specified, variable will be bound to DEFVAL instead of
NIL in cases where no switch keyword was found in EXPR.
"

  (let* ((switchlist (member '&switch lambdalist))

         ;; 'remove' switches from lambdalist
         (lambdalist (ldiff lambdalist switchlist))

         ;; drop &switch keyword
         (switches (when switchlist (cdr switchlist)))

         ;; construct bindings for each switch and collect valid
         ;; keywords in 'switchkeys'
         (switchkeys '())
         (bindings
          (mapcar
           #'(lambda (spec)
               (cond

                 ;; spec simply consists of VAR
                 ((symbolp spec)
                    (let ((key (intern (symbol-name spec) 'keyword)))
                      (push key switchkeys)
                      `(,spec (FIND ,key <DESTRUCTURING-SWITCH-FORM>))
                      ))

                 ;; spec is (VAR form)
                 ((and (listp spec) (>= (list-length spec) 2))
                    (let ((var (first spec))
                          (keyspec (second spec))
                          (defval (when (cddr spec) (third spec))))
                      (cond
                        ;; spec is (VAR (KEY1 KEY2 ...))
                        ((listp keyspec)
                           `(,var
                             (COND
                               ,@(mapcar
                                  #'(lambda (key)
                                      (push key switchkeys)
                                      `((FIND ,key <DESTRUCTURING-SWITCH-FORM>)
                                        ,key))
                                  keyspec)
                               (T ,defval))))

                        ;; spec is (VAR KEY)
                        (t
                           (push keyspec switchkeys)
                           ` (,var (OR
                                    (FIND ,keyspec <DESTRUCTURING-SWITCH-FORM>)
                                    ,defval)))
                        )))

                 ;; illegal spec
                 (t (error "~A is not a valid switch specifier." spec))
                 ))
           switches))
         )

    ;; construct the expansion form
    ` (LET ((<DESTRUCTURING-SWITCH-FORM> ,expr))
        (LET ,bindings
          (HANDLER-CASE
              (DESTRUCTURING-BIND ,lambdalist
                  (SET-DIFFERENCE-KEEP-ORIGINAL-ORDER
                   <DESTRUCTURING-SWITCH-FORM>
                   ',switchkeys)
                ,@body)
            (ERROR (COND)
              (DECLARE (IGNORE COND))
              (ERROR "~&Syntax Error in ~S." WHOLE)
              )
            )
          )
        )
    ))

(defun set-difference-keep-original-order (set1 set2)
  (mapcan #'(lambda (elt)
              (unless (member elt set2)
                (list elt)))
          set1))

#|
(destructuring-switch-bind (name &rest args &key foo bar
                                 &switch
                                 on                     ; simple form
                                 (started :start)
                                 (speed (:fast :slow))
                                 (off :turn-off :turn-on) ; with switch key
                                 (choose (:one :two :three) :three))
                           '(width :foo 5 :bar 7 :on :turn-off :one)
  (list name foo bar args on started speed off choose))
|#
;; line 111 "intface.nw"
(eval-when (compile load eval)
  (defun stringify (name)
    (declare (inline))
    (etypecase name
      (symbol (symbol-name name))
      (string name)
      (number (write-to-string name)))))

(defun list-of-strings-p (x)
  (cond ((not (listp x)) nil)
        ((every #'(lambda (elt)
                    (or (stringp elt) (symbolp elt) (numberp elt)))
                x))))
;; line 136 "intface.nw"
(defun make-well-formed-list (attr-list)
  (mapcar #'(lambda (x)
              (cond ((null x)
                       (error "empty lists are not allowed !"))
                    ((or (symbolp x) (stringp x) (numberp x))
                       (list (stringify x)))
                    ((listp x)
                       (map-to-strings x))))
          attr-list))

(defun map-to-strings (slist)
  (mapcar #'(lambda (x)
              (cond ((null x)
                       (error "empty lists are not allowed !"))
                    ((or (symbolp x) (stringp x) (numberp x))
                       (stringify x))
                    ((listp  x)
                       (error "nesting level too deep !"))
                    (t (error "~S is not a string !" x))))
          slist))
;; line 185 "intface.nw"
(defun c-string-reader (stream char)
  (declare (ignore char))
  (let ((ch nil)
        (str (make-array 32
                         :element-type
                         #+CLISP 'string-char
                         #-CLISP 'base-char
                         :adjustable t
                         :fill-pointer 0)))
    (loop
      (setq ch (read-char stream t #\Newline t))
      (case ch
        (#\" (let ((nextch (read-char-no-hang stream t #\Newline t)))
               (when nextch
                 (unread-char nextch stream)
                 (case nextch
                   (#\Newline)
                   (#\t)
                   (#\Space)
                   (#\))
                   (t (nraw "~&Possible read-error due to ill-formed string \"~A\" ~S"
                            str *))))
               (return str)))
        (#\~ (setq ch (read-char stream t #\Newline t))
             (case ch
               (#\n (vector-push-extend #\Newline str))
               (#\t (vector-push-extend #\Tab str))
               (#\b (vector-push-extend (character   1) str))
               (#\e (vector-push-extend (character 255) str))
               (t   (vector-push-extend ch str))))
        (t (vector-push-extend ch str))))))

(set-macro-character #\" #'c-string-reader nil *indexstyle-readtable*)

#|
(let ((x #\Null))
  (loop for i from 1 to 255
        do (progn (print (character i))
                  (when (char>= x (character i))
                    (print "Whee!"))
                  (setq x (character i)))))
|#
;; line 254 "intface.nw"
(defmacro define-alphabet (&whole whole &optional name string-list)
  (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 256 "intface.nw"
        ((not (listp string-list))
           (error "missing argument <string-list> !"))
        ((not (list-of-strings-p string-list))
           (error "~S is not a list of strings" string-list))
        (t `(LET ((NAME (STRINGIFY ',name)))
             (WHEN (LOOKUP-BASETYPE *indexstyle* NAME)
               (NRAW "redefining alphabet `~S'" NAME))
             (ADD *indexstyle* (MAKE-ALPHABET NAME ',string-list))))))

(defmacro define-alphabet* (&whole whole &optional name string-list)
  (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 267 "intface.nw"
        ((not (listp string-list))
           (error "missing argument <string-list> !"))
        (t (let ((scar (car string-list)))
             (if (and (symbolp scar) (fboundp scar))
                 `(LET ((NAME (STRINGIFY ',name))
                        (STRING-LIST ,string-list)) #| evaluates string-list |#
                   (UNLESS (LIST-OF-STRINGS-P STRING-LIST)
                     (ERROR "~S is not a list of strings" STRING-LIST))
                   (WHEN (LOOKUP-BASETYPE *indexstyle* NAME)
                     (NRAW "redefining alphabet `~S'" NAME))
                   (ADD *indexstyle* (MAKE-ALPHABET NAME STRING-LIST))))))))
;; line 284 "intface.nw"
(defmacro define-enumeration (&whole
                              whole
                              &optional
                              name prefix-match-func base-alphabet)
  (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 289 "intface.nw"
        ((null prefix-match-func)
           (error "missing argument <prefix-match-function> !"))
        (t (let* ((name   (stringify name))
                  (symbol (make-symbol
                           (concatenate 'string "ENUMERATION-" name))))
             `(LET () #| PROGN instead of LET () yields a runtime error !? |#
               (IF (LOOKUP-BASETYPE *indexstyle* ,name)
                   (NRAW "redefining alphabet `~S'" ,name)
                   (PROGN
                     (DEFCLASS ,symbol (ENUMERATION) ())
                     (ADD *indexstyle*
                          (MAKE-ENUMERATION ',symbol ,name ,base-alphabet))))
               (DEFMETHOD PREFIX-MATCH ((STR STRING) (ENUM ,symbol))
                 (FUNCALL ,prefix-match-func STR)))))))
;; line 312 "intface.nw"
(defmacro define-location-class (&whole whole &rest args)
  (destructuring-switch-bind (name
                              layer-list
                              &key
                              (min-range-length *default-minimum-range-length*)
                              (hierdepth 0) ;no hierarchies are to be formed
                              &switch
                              (var :var))
      args
    (cond
      ((null name)
         (error "missing argument <name> in~%~S" whole))
      ((null layer-list)
         (error "missing argument <layer-list> in~%~S" whole))
      ;;((not (list-of-strings-p layer-list))
      ;;  (error "~S is not a list of strings in~%~S" layer-list whole))
      ((not (numberp hierdepth))
         (error "~S is not a number! in~%~S" hierdepth whole))
      ((not (or (numberp   min-range-length)
                (eql 'none min-range-length)))
         (error "~S must be a number or the keyword `none' in~%~S"
                min-range-length whole))
      (t (when (eql 'none min-range-length)
           (setq min-range-length 0))
         `(LET ((NAME (STRINGIFY ',name))
                (LAYERS (BUILD-LOCCLASS-LAYERS ',layer-list ',whole)))
           ;;(GOL T "~&define-location-class: ~S" LAYERS)
           (COND ((LOOKUP-CROSSREF-CLASS *indexstyle* NAME)
                    (OOPS "replacing cross-reference-class `~S' is not allowed !"
                          NAME))
                 (t (WHEN (LOOKUP-LOCATION-CLASS *indexstyle* NAME)
                      (NRAW "redefining location-reference-class `~S' !" NAME))
                    (ADD *indexstyle*
                         ,(if var
                              `(CHECKED-MAKE-VAR-LOCATION-CLASS NAME LAYERS
                                ,hierdepth)
                              `(CHECKED-MAKE-STANDARD-LOCATION-CLASS NAME LAYERS
                                ,min-range-length ,hierdepth))))
                 ))))))
;; line 354 "intface.nw"
(defun build-locclass-layers (layers whole)
  (let ((res-list '()))
    (loop
      ;;(gol t "~&build-locclass-layers: ~S" layers)
      (when (endp layers) (return (nreverse res-list)))
      (cond ((eql :sep (car layers))
               (setq layers (cdr layers))
               (if (endp layers)
                   (error "Reached end of list in~%~S" whole)
                   (push (make-loccls-separator (stringify (car layers)))
                         res-list)))
            (t (let* ((layer (stringify (car layers)))
                      (basetype (lookup-basetype *indexstyle* layer)))
                 (if basetype
                     (push (make-loccls-layer basetype) res-list)
                     (error "Unknown basetype ~S in~%~S" layer whole)))))
      (setq layers (cdr layers)))))
;; line 377 "intface.nw"
(defmacro define-crossref-class (&whole whole &rest args)
  (destructuring-switch-bind (name
                              &switch (verified :unverified))
      args
    (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 382 "intface.nw"
      (t `(LET ((NAME (STRINGIFY ',name)))
           (cond ((LOOKUP-LOCATION-CLASS *indexstyle* NAME)
                    (OOPS "replacing location-reference-class `~S' is not allowed !"
                          NAME))
                 (t (WHEN (LOOKUP-CROSSREF-CLASS *indexstyle* NAME)
                      (NRAW "redefining crossref-location-class `~S'" NAME))
                    (ADD *indexstyle*
                         ,(if verified
                              `(MAKE-UNVERIFIED-CROSSREF-LOCATION-CLASS NAME)
                              `(MAKE-VERIFIED-CROSSREF-LOCATION-CLASS NAME))))))))))
;; line 398 "intface.nw"
(defmacro define-location-class-order (olist)
  (cond ((not (listp olist))
           (error "missing argument <list> !"))
        ((not (list-of-strings-p olist))
           (error "~S is not a list of strings" olist))
        (t `(LET ((CTR 1))
             (MAPC #'(LAMBDA (NAME)
                       (LET* ((NEW-NAME (STRINGIFY NAME))
                              (LOCCLS
                               (CDR (LOOKUP-LOCATION-CLASS *indexstyle*
                                                           NEW-NAME))))
                         (INCF CTR)
                         (COND (LOCCLS (SET-ORDNUM CTR LOCCLS))
                               (t (OOPS "unknown location-class `~S'"
                                        NEW-NAME)))))
              ',olist)))))
;; line 423 "intface.nw"
(defmacro define-attributes (&whole whole &optional attr-list)
  (cond ((null attr-list)
           (error "missing argument <attribute-list> in~&~S" whole))
        (t (let* ((wf-attr-list (make-well-formed-list attr-list))
                  (plain-list (apply #'concatenate 'list wf-attr-list))
                  )
             `(PROGN
               (MAPCAR
                #'(LAMBDA (CATATTR)
                    (IF (LOOKUP-CATATTR *indexstyle* CATATTR)
                        (NRAW "ignoring redefinition of attribute ~S in~&~S"
                              CATATTR ',whole)
                        (ADD *indexstyle*
                             (MAKE-CATEGORY-ATTRIBUTE CATATTR))))
                ',plain-list)
               (INITIALIZE-CATEGORY-ATTRIBUTES ',wf-attr-list *indexstyle*))
             ))))
;; line 488 "intface.nw"
#+:ORDRULES
(defmacro merge-rule (&whole whole &rest args)
  (destructuring-switch-bind (pattern
                              replacement
                              &switch
                              (again           :again)
                              (string          :string)
                              (extended-regexp :eregexp)
                              (basic-regexp    :bregexp))
      args
    (cond ((null pattern)
             (error "missing argument <pattern> !"))
          ((or (and string extended-regexp) (and string basic-regexp)
               (and extended-regexp basic-regexp))
             (error "~&Only one of (:string :bregexp :eregexp) allowed in:~&~S !"
                    whole))
          ((null replacement)
             (error "~&Missing argument <replacement> in:~&~S!" whole))
          (t `(PROGN
                (ORDRULES:ADD-KEYWORD-MERGE-RULE ,pattern ,replacement
						 ,(if again 1 0)
						 ,(cond (string 1)
							(basic-regexp 2)
							(extended-regexp 3)
							(t 0)))
                (FLUSH-ORDRULES-MESSAGE-BUFFER))))))

#-:ORDRULES
(defmacro merge-rule (&rest args)
  (declare (ignore args))
  (oops "merge-rule not supported! ignoring definition"))
;; line 525 "intface.nw"
#+:ORDRULES
(defparameter *sort-rule-orientations*
  '(forward forward forward forward
    forward forward forward forward))

#+:ORDRULES
(defmacro define-sort-rule-orientations (&whole whole &rest args)
  (destructuring-switch-bind (&optional
                              (orientations *sort-rule-orientations*))
      args
    (unless (every #'(lambda (run)
                       (member run '(forward backward)))
                   orientations)
      (error "~&Elements of ~S must be one of {forward,backward}!"
        orientations))
    `(PROGN
      (SETQ *SORT-RULE-ORIENTATIONS* ',orientations)
      (ORDRULES:INITIALIZE (LENGTH *SORT-RULE-ORIENTATIONS*))
      (FLUSH-ORDRULES-MESSAGE-BUFFER))))

#+:ORDRULES
(defmacro define-rule-set (&whole whole &rest args)
  (destructuring-bind (&optional
                       name
                       &key
                       inherit-from
                       rules)
      args
    (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 554 "intface.nw"
          ((not (listp rules))
             (error "missing argument <rules-list> !"))
          (t `(LET ((RS (MAKE-RULE-SET ',name ',inherit-from ',rules)))
               (MAPC #'(LAMBDA (INH-NAME)
                         (INHERIT-RULE-SET RS (LOOKUP-RULE-SET *INDEXSTYLE*
                                                               INH-NAME)))
                ',inherit-from)
               (ADD-RULE-SET *INDEXSTYLE* RS))))))

#-:ORDRULES
(defmacro define-rule-set (&rest args)
  (declare (ignore args))
  (oops "define-rule-sets not supported! ignoring definition"))

#+:ORDRULES
(defmacro use-rule-set (&whole whole &rest args)
  ;; FIXME: if we use destructuring-switch-bind-here an error occurs
  (destructuring-bind (&key
                       run
                       rule-set)
      args
    (cond ((not (integerp run))
             (error "incorrect argument <run> !"))
          ((not rule-set)
             (error "missing argument <rule-set> !"))
          ((not (or (listp rule-set)
                    (stringp rule-set)))
             (error "wrong argument to <rule-set> !"))
          (t `(INSTANTIATE-RULE-SET ,run ,rule-set)))))

#-:ORDRULES
(defmacro use-rule-set (&rest args)
  (declare (ignore args))
  (oops "use-rule-sets not supported! ignoring definition"))

#+:ORDRULES
(defmacro sort-rule (&whole whole &rest args)
  (destructuring-switch-bind (pattern
                              replacement
                              &key
                              (run 0)
                              &switch
                              (again           :again)
                              (string          :string)
                              (extended-regexp :eregexp)
                              (basic-regexp    :bregexp))
      args
    (cond ((null pattern)
             (error "missing argument <pattern> !"))
          ((or (< run 0) (>= run (length *sort-rule-orientations*)))
             (error "~&Argument of :run must be in the interval [0..~A]!"
                    (length *sort-rule-orientations*)))
          ((or (and string extended-regexp) (and string basic-regexp)
               (and extended-regexp basic-regexp))
             (error "~&Only one of (:string :bregexp :eregexp) allowed in:~&~S !"
                    whole))
          ((null replacement)
             (error "missing argument <replacement> !"))
          (t `(PROGN
	        (ORDRULES:ADD-KEYWORD-SORT-RULE ,run ,pattern ,replacement
		                                ,(if again 1 0)
		                                ,(cond (string 1)
                                                       (basic-regexp 2)
                                                       (extended-regexp 3)
                                                       (t 0)))
                (FLUSH-ORDRULES-MESSAGE-BUFFER))))))

#-:ORDRULES
(defmacro define-sort-rule-orientations (&rest args)
  (declare (ignore args))
  (oops "define-sort-rule-orientations not supported! ignoring definition"))

#-:ORDRULES
(defmacro sort-rule (&rest args)
  (declare (ignore args))
  (oops "sort-rule not supported! ignoring definition"))

(defun flush-ordrules-message-buffer ()
  #+:ORDRULES
  (when (and *mappings-trace* ordrules::*message-logging*)
    (gol t "~A" ordrules::*message-buffer*)
    (setq ordrules::*message-buffer-ptr* 0)))
;; line 696 "intface.nw"
(defmacro define-letter-group
    (&whole whole name &key prefixes before after)
  (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 699 "intface.nw"
        ((and before
              (not (lookup-letter-group-definition *indexstyle* before)))
           (error "~S does not name a letter group in~%~S" before whole))
        ((and after
              (not (lookup-letter-group-definition *indexstyle* after)))
           (error "~S does not name a letter group in~%~S" after whole))
        ((not (list-of-strings-p prefixes))
           (error "argument <prefixes> is not a list of strings in~%~S"
                  whole))
        (t `(LET ()
             (ADD-LETTER-GROUPS *INDEXSTYLE*
              ,name ',(or prefixes (list name))
              :BEFORE ,before :AFTER ,after)))))
;; line 718 "intface.nw"
(defmacro define-letter-groups (&whole whole &optional letter-groups)
  (cond ((not (list-of-strings-p letter-groups))
           (error "~S is not a list of strings in~%~S" letter-groups whole))
        (t `(LET ()
             (ADD-LETTER-GROUPS *INDEXSTYLE*
              ,(car letter-groups) ',(list (car letter-groups)))
             ,@(let ((last (car letter-groups)))
                    (mapcar #'(lambda (letter-group)
                                (prog1
                                    `(ADD-LETTER-GROUPS *INDEXSTYLE*
                                      ,letter-group ',(list letter-group)
                                      :AFTER ,last)
                                  (setq last letter-group)))
                            (cdr letter-groups)))))))
;; line 650 "intface.nw"
(defmacro merge-to (&whole whole &rest args)
  (destructuring-switch-bind (from to &switch drop)
      args
    (let ((from-attr (lookup-catattr *indexstyle* from))
          (to-attr   (lookup-catattr *indexstyle* to)))
      (cond ((not from-attr)
               (nraw "`~S' is not a known attribute in:~&~A" from whole))
            ((not to-attr)
               (nraw "`~S' is not a known attribute in:~&~A" to whole))
            (t (if drop
                   `(ADD-DROP-IF-MERGED-RULE *INDEXSTYLE*
                     ',from-attr ',to-attr)
                   `(ADD-MERGE-RULE *INDEXSTYLE*
                     ',from-attr ',to-attr)))))))
;; line 740 "intface.nw"
;;; we remove the inherited symbol and establish a new one
(eval-when (compile load eval)
  (shadow 'require))

(defmacro require (&rest args)
  (cond ((null args)
           (error "missing argument <filename> !"))
        ((not (stringp (car args)))
           (error "(require ~S): expected <filename> !" (car args)))
        (t `(LET ((FILENAME (STRINGIFY ',(car args))))
             (DO-REQUIRE FILENAME)))))

(defun do-require (filename)
  (let ((file (find-file filename)))
    (unless (member file *included-files* :test #'equalp)
      (push file *included-files*)
      (info "~&Loading module ~S..." filename)
      (watched-load file :echo t)
      (info "~&Finished loading module ~S.~%" filename))))

#|
(defun do-require (filename)
  (let ((file (find-file filename)))
    (unless (member file *included-files* :test #'equalp)
      (push file *included-files*)
      (info "~&Loading module ~S..." filename)
      (let ((*standard-output* (make-string-output-stream)))
        (handler-case
            (load file :verbose nil :echo t)
;         (stream-error (cond)
;                       (step
;                        (progn
;                          (oops*
;                            (simple-condition-format-string cond)
;                            (simple-condition-format-arguments cond))
;                          (exit))
;                        )
;                       )
          (error (condition)
                 (info "~&~%~A~%" ";;;;;;;;;;;;;;;;;;;;;;;;")
                 (info "~A~%"     ";; ERROR SECTION START")
                 (info "~%~A~%" (get-output-stream-string *standard-output*))
                 (info "~%~A~%"   ";; ERROR SECTION END")
                 (info "~A~%~%"   ";;;;;;;;;;;;;;;;;;;;;;;;")
                 (oops* (simple-condition-format-string    condition)
                        (simple-condition-format-arguments condition))
                 (error-exit))
          )
        )
      (info "~&Finished loading module ~S.~%" filename))))
|#
;; line 802 "intface.nw"
(defun find-file (filename)
  (let ((file (parse-namestring filename)))
    (when (equal (file-namestring file) "")
      (error "~S is not a valid filename !" filename))
    (let ((found (some #'(lambda (path)
                           (and (probe-file (append-pathnames path file))))
                       *load-paths*)))
      (or found (error "Could not find file ~S !" filename)))))
;; line 813 "intface.nw"
(defvar *wl-fmt* "~&~%Error in line ~S:~%~A~%")
(defun watched-load (filename
                     &key
                     (print *load-print*)
                     (if-does-not-exist t)
                     echo)
  (let ((stream (open (setq filename (pathname filename))
                      :direction
                      #+CLISP :input-immutable
                      #-CLISP :input
                      :element-type
                      #+CLISP 'string-char
                      #-CLISP 'base-char
                      :if-does-not-exist nil))
        (lineno 1))
    (if stream
        (let* ((load-stream (make-string-output-stream))
               (input-stream (if echo
                                 (make-echo-stream stream load-stream)
                                 stream
                                 ))
               (*load-pathname* (if (pathnamep filename) filename nil))
               (*load-truename* (if (pathnamep filename) (truename filename) nil))
               (*package* *package*)    ; *PACKAGE* binden
               (*readtable* *readtable*) ; *READTABLE* binden
               (end-of-file "EOF"))     ; einmaliges Objekt
          ;;(setq sys::*load-input-stream* input-stream)
          (block nil
            (unwind-protect
                 (tagbody weiter
                    (let* ((obj
                            (handler-case (read input-stream nil end-of-file)
                              (error (cond)
                                (when echo
                                  (let* ((str (get-output-stream-string load-stream))
                                         (cnt (count #\Newline str)))
                                    (info *wl-fmt* (+ cnt lineno) str)
                                    (oops* (simple-condition-format-string cond)
                                           (simple-condition-format-arguments cond))
                                    (error-exit))))))
                           (str (get-output-stream-string load-stream))
                           (cnt (count #\Newline str)))
                      (incf lineno cnt)
                      (when (eql obj end-of-file)
                        (return-from nil))
                      (setq obj
                            (multiple-value-list
                                (cond ((compiled-function-p obj) (funcall obj))
                                      ;;(compiling (funcall (compile-form obj nil nil nil nil nil)))
                                      (t (handler-case (eval obj)
                                           (error (cond)
                                             (info *wl-fmt* lineno str)
                                             (oops* (simple-condition-format-string cond)
                                                    (simple-condition-format-arguments cond))
                                             (error-exit)))
                                         (get-output-stream-string load-stream)
                                         ))))
                      (when print (when obj (print (first obj))))
                      )
                    (go weiter)
                    )
              (close stream) (close input-stream)
              ) )
          t)
        (if if-does-not-exist
            (error "A file with name ~A does not exist" filename)
            nil)
        ) ))
;; line 891 "intface.nw"
(defmacro searchpath (&rest args)
  (cond ((null args)
           (error "missing argument <pathlist> !"))
        ((stringp (car args))
           `(SET-SEARCHPATH-BY-STRING ,(car args)))
        ((listp args)
           `(SET-SEARCHPATH ',(car args)))))

(defun set-searchpath-by-string (searchpath-str)
  (set-searchpath (string-to-searchlist searchpath-str)))
;; line 913 "intface.nw"
(defun string-to-searchlist (str)
  (let ((paths '())
        pos)
    (nreverse
     (loop (when (equalp str "")
             (return (push :default paths)))
           (setq pos (position #+UNIX #\:
                               #+OS/2 #\;
                               #+WIN32 #\;
                               str))
           (if pos
               (progn
                 (push (subseq str 0 pos) paths)
                 (setq str (subseq str (1+ pos))))
               (return (push str paths)))))))

(defun set-searchpath (path-list)
  (setq *load-paths*
        (apply #'nconc
               (mapcar
                #'(lambda (elt)
                    (cond ((stringp elt)
                             (list (make-real-pathname elt)))
                          ((eq elt :default)
                             *default-load-paths*)
                          ((eq elt :last)
                             *load-paths*)
                          (t (oops "~&searchpath: invalid argument ~S in~%~S"
                                   elt path-list))))
                path-list))))
;; line 954 "intface.nw"
#+CLISP
(defun make-real-pathname (pathname-str)
  (let* ((pn     (pathname pathname-str))
         (fname  (file-namestring pn)))
    (if (string/= fname "")
        (make-pathname :device (pathname-device pn)
                       :directory (append (or (pathname-directory pn)
					      '(:relative))
                                          (list fname)))
        pn)))
;; line 966 "intface.nw"
#-CLISP
(defun make-real-pathname (pathname-str)
  (let ((pn (parse-namestring
             (if (eql (elt pathname-str (1- (length pathname-str))) #\/)
                 pathname-str
                 (concatenate 'string pathname-str "/")))))
    (if (pathname-directory pn)
        pn
        (make-pathname :directory (list :relative (pathname-name pn))))))
;; line 986 "intface.nw"
(defun append-pathnames (pn-dir pn-sub)
  (let ((dir-dir (pathname-directory pn-dir))
        (dir-sub (pathname-directory pn-sub)))
    (cond ((eq :absolute (car dir-sub)) pn-sub)
          (t (make-pathname :device (pathname-device pn-dir)
                            :directory (append dir-dir (cdr dir-sub))
                            :name (pathname-name pn-sub)
                            :type (pathname-type pn-sub))))))

;; line 62 "intface.nw"
(defparameter *idxstyle-user-interface-definitions*
  '(
    *default-minimum-range-length*
    *included-files*
    *indexstyle-readtable*
    define-alphabet
    define-alphabet*
    define-attributes
    define-crossref-class
    define-enumeration
    define-letter-group
    define-letter-groups
    define-location-class
    define-location-class-order
    define-rule-set
    define-sort-rule-orientations
    forward backward
    info
    merge-rule
    merge-to
    require
    searchpath
    sort-rule
    use-rule-set
    *indexstyle*
    ))

;; line 1168 "intface.nw"
(eval-when (compile load eval)

;; line 91 "intface.nw"
(export '(*indexstyle*
          *indexstyle-readtable*
          do-require
          set-searchpath-by-string
          *sort-rule-orientations*
          forward backward
          *idxstyle-user-interface-definitions*
          ))
;; line 159 "intface.nw"
(export '(stringify list-of-strings-p
          make-well-formed-list map-to-strings))
;; line 642 "intface.nw"
(export 'flush-ordrules-message-buffer)
;; line 1170 "intface.nw"
  )
;; line 557 "idxrule.nw"
;; $Id: idxrule.nw,v 1.10 1997/03/26 16:18:19 kehr Exp $

;; line 323 "idxrule.nw"
(defclass relation ()
    ((rel-sets :accessor get-rel-sets
               :initform '() )
     (rel-hash :accessor get-rel-hash
               :initform (make-hash-table :test #'equal))))

(defun make-relation ()
  (make-instance 'relation))
;; line 344 "idxrule.nw"
(defmethod rel-insert ((rel relation) x y)
  (let* ((rel-sets (get-rel-sets rel))
         (x-node (assoc x rel-sets))
         (y-node (assoc y rel-sets)))
    (cond ((null x-node) (setf (get-rel-sets rel) (cons (list x y) rel-sets)))
          ((member y (cdr x-node)))
          (t (rplacd x-node (cons y (cdr x-node)))))
    (when (and (null y-node) (not (eql x y)))
      (setf (get-rel-sets rel) (cons (list y) (get-rel-sets rel)))))
  rel)

(defmethod rel-simple-insert ((rel relation) x)
  (let* ((rel-sets (get-rel-sets rel))
         (x-node (assoc x rel-sets)))
    (unless x-node (setf (get-rel-sets rel) (cons (list x) rel-sets))))
  rel)
;; line 366 "idxrule.nw"
(defmethod rel-merge ((rel-1 relation) x)
  (declare (ignore x))
  rel-1)

(defmethod rel-merge ((rel-1 relation) (rel-2 relation))
  (mapc #'(lambda (r-2)
            (let ((r-1 (assoc (car r-2) (get-rel-sets rel-1))))
              (cond ((null r-1)
                       (setf (get-rel-sets rel-1)
                             (cons r-2 (get-rel-sets rel-1))))
                    (t (rplacd r-1 (union (cdr r-1) (cdr r-2)))))))
        (get-rel-sets rel-2))
  rel-1)
;; line 387 "idxrule.nw"
(defmacro relation-set (rel x)
  `(CDR (ASSOC ,x (SLOT-VALUE ,rel 'REL-SETS))))
;; line 408 "idxrule.nw"
(defmethod make-ready ((rel relation))
  (let ((ht (get-rel-hash rel)))
    (mapc #'(lambda (node)
              (mapc #'(lambda (succ-node)
                        (setf (gethash (cons (car node) succ-node) ht)
                              t))
                    (cdr node)))
          (get-rel-sets rel)))
  rel)
;; line 423 "idxrule.nw"
(defmacro relation-p (rel x y)
  `(GETHASH (CONS ,x ,y) (SLOT-VALUE ,rel 'REL-HASH)))
;; line 439 "idxrule.nw"
(defun print-relation (rel-sets stream printfunc
                                &key open close headsep listsep)
  (mapc #'(lambda (from)
            (when open (format stream open))
            (write (funcall printfunc (car from)) :stream stream)
            (when headsep (format stream headsep))
            (mapc #'(lambda (to)
                      (write (funcall printfunc to) :stream stream)
                      (when listsep (format stream listsep)))
                  (cdr from))
            (when close (format stream close)))
        rel-sets))
;; line 467 "idxrule.nw"
(defmethod topsort ((rel relation))
  (let ((rel-set (copy-tree (get-rel-sets rel)))
        (topsort-list '()))
    (loop
      (let ((zero-indeg (find-if #'(lambda (x) (null (cdr x))) rel-set)))
        (when (null zero-indeg) (return))
        (let ((zero-indeg-elt (car zero-indeg)))
          (setq topsort-list (cons zero-indeg-elt topsort-list))
          (setq rel-set (delete zero-indeg rel-set :test #'equal))
          (mapc #'(lambda (x)
                    (rplacd x (delete zero-indeg-elt (cdr x) :test #'equal)))
                rel-set)))
      )
    (if (null rel-set)
        (values topsort-list t)
        (values nil rel-set))))
;; line 494 "idxrule.nw"
(defmethod transform-relation ((rel relation) transform-func)
  (let ((new-rel (make-relation)))
    (mapc #'(lambda (x)
              (let ((transformed-x (funcall transform-func (car x))))
                (mapc #'(lambda (y)
                          (rel-insert new-rel
                                      transformed-x
                                      (funcall transform-func y)))
                      (cdr x))))
          (get-rel-sets rel))
    new-rel))
;; line 511 "idxrule.nw"
(defmethod remove-reflexivity ((rel relation))
  (mapc #'(lambda (x)
            (rplacd x (delete (car x) (cdr x) :test #'equal)))
        (get-rel-sets rel))
  rel)
;; line 529 "idxrule.nw"
(defmethod transitive-hull ((rel relation))
  (setf (get-rel-sets rel)
        (let ((rel-sets (get-rel-sets rel)))
          (mapc #'(lambda (j)
                    (mapc #'(lambda (i)
                              (when (member (car j) (cdr i))
                                (mapc #'(lambda (k)
                                          (when (and
                                                 (member (car k) (cdr j))
                                                 (not (member (car k)
                                                              (cdr i))))
                                            (rplacd (last i)
                                                    (list (car k)))))
                                      rel-sets)))
                          rel-sets))
                rel-sets)))
  rel)
;; line 61 "idxrule.nw"
(defclass index-rule ()
    ())

(defclass relation-rule (index-rule relation)
    ())
;; line 88 "idxrule.nw"
(defclass merge-rule (relation-rule)
    ())

(defun make-merge-rule ()
  (make-instance 'merge-rule))
;; line 117 "idxrule.nw"
(defclass drop-if-merged-rule (relation-rule)
    ())

(defun make-drop-if-merged-rule ()
  (make-instance 'drop-if-merged-rule))
;; line 278 "idxrule.nw"
(defclass substitution-rule (relation-rule)
    ())

(defun make-substitution-rule ()
  (make-instance 'substitution-rule))
;; line 164 "idxrule.nw"
(defclass catattr-groups (index-rule)
    ((attribute-list  :reader get-attribute-list
                      :writer set-attribute-list
                      :initform nil)
     (attribute-table :reader get-attribute-table
                      :initform (make-hash-table :test #'equal
                                                 :size 16))))

(defun make-catattr-groups ()
  (make-instance 'catattr-groups))

(defmethod add ((catattr-grp catattr-groups)
                (catattr     category-attribute))
  (setf (gethash (get-name catattr) (get-attribute-table catattr-grp))
        catattr))

(defmacro lookup-catattr-name (catattr-grp catattr-name)
  `(GETHASH ,catattr-name (SLOT-VALUE ,catattr-grp 'ATTRIBUTE-TABLE)))
;; line 217 "idxrule.nw"
(defun initialize-category-attributes (attribute-list idxcls)
  (let ((catattr-grps    (get-cat-attrs idxcls))
        (impl-subst-rule (get-implicit-subst-rules idxcls)))
    (set-attribute-list attribute-list catattr-grps)
    (let ((catattr-grp-counter 0)
          (sort-ordnum-counter 0))
      (mapc
       #'(lambda (sublist)
           (incf catattr-grp-counter)
           (let ((last-in-group (lookup-catattr-name catattr-grps
                                                     (car (last sublist))))
                 (prev-catattr nil))
             (mapc
              #'(lambda (catattr)
                  (cond ((stringp catattr)
                           (let ((ca (lookup-catattr-name catattr-grps
                                                          catattr)))
                             (incf sort-ordnum-counter)
                             (set-sort-ordnum        sort-ordnum-counter ca)
                             (set-catattr-grp-ordnum catattr-grp-counter ca)
                             (set-last-in-group      last-in-group       ca)
                             (when prev-catattr
                               (rel-insert impl-subst-rule
                                           (lookup-catattr-name catattr-grps
                                                                prev-catattr)
                                           (lookup-catattr-name catattr-grps
                                                                catattr)))
                             (setq prev-catattr catattr)))
                        (t (error "~A  is not a string" catattr))))
              sublist)))
       attribute-list))
    catattr-grps))

;; line 566 "idxrule.nw"
(eval-when (compile load eval)

;; line 69 "idxrule.nw"
(export '(index-rule relation-rule))
;; line 96 "idxrule.nw"
(export '(merge-rule make-merge-rule))
;; line 125 "idxrule.nw"
(export '(drop-if-merged-rule make-drop-if-merged-rule))
;; line 185 "idxrule.nw"
(export '(catattr-groups make-catattr-groups lookup-catattr-name))
;; line 252 "idxrule.nw"
(export '(initialize-category-attributes catattr-single catattr-mixed))
;; line 286 "idxrule.nw"
(export '(substitution-rule make-substitution-rule))
;; line 428 "idxrule.nw"
(export '(relation make-relation get-rel-sets
          rel-insert rel-merge make-ready
          relation-set relation-p))
;; line 549 "idxrule.nw"
(export '(topsort transform-relation remove-reflexivity transitive-hull))
;; line 568 "idxrule.nw"
  )
;; line 873 "idxclass.nw"
;; $Id: idxclass.nw,v 1.20 1997/03/26 16:18:16 kehr Exp $
;;

;; line 129 "idxclass.nw"
(defparameter *locref-cache-size*        1000)
(defparameter *locref-cache-rehash-size*    2)
(defparameter *locref-cache-hits*           0)
(defparameter *locref-cache-misses*         0)
;; line 80 "idxclass.nw"
(defclass indexclass ()
    ((name                 :initarg :name :type string)
     (basetypes            :accessor get-basetypes
                           :initform (make-hash-table :test #'equal :size 10))
     (cat-attrs            :accessor get-cat-attrs
                           :initform (make-catattr-groups))
     (locref-classes       :accessor get-locref-classes
                           :initform '() :type list)
     (xref-classes         :accessor get-xref-classes
                           :initform '() :type list)
     (letter-groups        :reader get-letter-groups
                           :initform (make-letter-group-manager))
     (partial-order-dict   :reader get-partial-order-dict
                           :initform (make-partial-order-dict))
     (letter-dict          :reader get-letter-dict
                           :initform (make-letter-dict))
     (rule-sets            :reader get-rule-sets
                           :initform (make-hash-table :test #'equal :size 20))

;; line 136 "idxclass.nw"
     (locref-cache :initform (initialize-locref-cache))
;; line 99 "idxclass.nw"
     ;;     (succ-table           :accessor get-succ-table
     ;;                           :initform (make-hash-table))

;; line 219 "idxclass.nw"
     (merge-rules          :reader get-merge-rules
                           :initform (make-merge-rule))
     (drop-if-merged-rules :reader get-drop-if-merged-rules
                           :initform (make-drop-if-merged-rule))
;;     (merge-rule-topsort   :reader get-merge-rule-topsort
;;                           :writer set-merge-rule-topsort
;;                           :initform '())


;; line 724 "idxclass.nw"
     (merge-drop-hook  :reader get-merge-drop-hook
                       :writer set-merge-drop-hook
                       :initform #'set-state-deleted)

;; line 229 "idxclass.nw"
     (implicit-subst-rules :reader get-implicit-subst-rules
                           :initform (make-substitution-rule))
     (explicit-subst-rules :reader get-explicit-subst-rules
                           :writer set-explicit-subst-rules
                           :initform (make-substitution-rule))

;; line 776 "idxclass.nw"
     (implicit-subst-hook  :reader get-implicit-subst-hook
                           :writer set-implicit-subst-hook
                           :initform #'transitive-hull)
     (explicit-subst-hook  :reader get-explicit-subst-hook
                           :writer set-explicit-subst-hook
                           :initform #'identity)
     (global-subst-hook    :reader get-global-subst-hook
                           :writer set-global-subst-hook
                           :initform #'identity)
     (substitution-subst-hook :reader get-substitution-subst-hook
                              :writer set-substitution-subst-hook
                              :initform #'null)
;; line 102 "idxclass.nw"
     ))

(defun make-indexclass (name)
  (make-instance 'indexclass :name name))
;; line 253 "idxclass.nw"
(defparameter *default-letter-group-definition* "default")

(defclass letter-group-definition ()
    ((name    :initarg :name    :type string)
     (ordnum  :initarg :ordnum  :type number)))

(defun make-letter-group-definition (name)
  (make-instance 'letter-group-definition :name name :ordnum 0))

(defclass letter-group-manager ()
    ((prefixes      :accessor get-prefixes
                    :initform (make-hash-table :test #'equal))
     (letter-groups :accessor get-letter-groups
                    :initform (make-hash-table :test #'equal))
     (sort-relation :accessor get-sort-relation
                    :initform (make-relation))
     (lookup-list   :accessor get-lookup-list  :initform '())))

(defun make-letter-group-manager ()
  (let* ((lgm (make-instance 'letter-group-manager))
         (letter-groups (get-letter-groups lgm))
         (default-group (make-letter-group-definition
                         *default-letter-group-definition*)))
    (setf (gethash *default-letter-group-definition* letter-groups)
          default-group)
    (rel-simple-insert (get-sort-relation lgm) default-group)
    lgm))
;; line 295 "idxclass.nw"
(defun make-lookup-list (lgm)
  (setf (get-lookup-list lgm)
        (let ((lookup-list '()))
          (maphash #'(lambda (key elt)
                       (push (cons key elt) lookup-list))
                   (get-prefixes lgm))
          (sort lookup-list #'prefix<))))

(defun prefix< (x y)
  (let ((cx (car x))
        (cy (car y)))
    (let ((same-prefix (string/= cx cy)))
      (cond ((null same-prefix))
            ((> same-prefix 0)
               (and (= same-prefix (length (car y)))))
            (t (string< cx cy))))))
;; line 326 "idxclass.nw"
(defun add-letter-group-to-manager (lgm name prefix &key before after)
  (let* ((prefixes      (get-prefixes      lgm))
         (letter-groups (get-letter-groups lgm))
         (letter-group  (gethash name letter-groups)))
    (unless letter-group;; this letter group is unknown -- create one!
      (setq letter-group (make-letter-group-definition name))
      (setf (gethash name letter-groups) letter-group)
      (rel-simple-insert (get-sort-relation lgm) letter-group))
    (let ((current-binding (gethash prefix prefixes)))
      (and current-binding
           (not (equal name (get-name current-binding)))
           (warn "define-letter-group: prefix ~S now maps to letter group ~S"
                 prefix name)))
    (setf (gethash prefix prefixes) letter-group)
    (when before
      (rel-insert (get-sort-relation lgm)
                  letter-group
                  (lookup-letter-group-definition *indexstyle* before)))
    (when after
      (rel-insert (get-sort-relation lgm)
                  (lookup-letter-group-definition *indexstyle* after)
                  letter-group)))
  lgm)

(defmethod lookup-letter-group-definition ((idxcls indexclass) (letter string))
  (gethash letter (get-letter-groups (get-letter-groups idxcls))))
;; line 360 "idxclass.nw"
(defmethod make-ready ((lgm letter-group-manager))
  (make-lookup-list lgm)
  (let ((sort-relation  (get-sort-relation lgm)))
    (multiple-value-bind (topsorted-list remaining-relation)
        (topsort sort-relation)
      (if topsorted-list
          (let ((ctr 0))
            (mapc #'(lambda (grp-def)
                      (setf (get-ordnum grp-def) (incf ctr)))
                  topsorted-list))
          (error
           (concatenate 'string
                        "The user-defined relations for the letter-groups contains cycles!~%The part of the relation containing cycles is"
                        (with-output-to-string (s)
                          (print-relation remaining-relation s
                                          #'(lambda (lgdef)
                                              (get-name lgdef))
                                          :open "~%" :headsep " before "
                                          :listsep " "))))))))
;; line 388 "idxclass.nw"
(defmethod match-letter-group ((idxcls indexclass) (letter string))
  (let* ((lgm         (get-letter-groups idxcls))
         (lookup-list (get-lookup-list lgm))
         (match (find-if #'(lambda (prefix-cons)
                             (let* ((prefix (car prefix-cons))
                                    (match-len (string/= prefix letter)))
                               (or (not match-len) #| full match |#
                                   (= match-len    #| full match with lg |#
                                      (length prefix)))))
                         lookup-list))
         (letter-group-definition
          (if match
              (cdr match);; the corresponding letter-grp-def
              (lookup-letter-group-definition
               *indexstyle* *default-letter-group-definition*))))
    (gol t "~&Letter-group: ~S -> ~S"
         letter (get-name letter-group-definition))
    letter-group-definition))
;; line 549 "idxclass.nw"
(defclass rule-set ()
    ((name        :initarg  :name)
     (rules-list  :accessor rules-list :initarg :rules-list)
     (rules-hash  :reader   rules-hash
                  :initform (make-hash-table :test #'equal :size 32))
     ))

(defun make-rule-set (name inherit rules)
  (unless (every #'(lambda (rule)
                     (and (stringp (car  rule));; pattern
                          (stringp (cadr rule)));; replacement
                     )
                 rules)
    (error "not a valid rule-set definition!"))
  (let ((rs (make-instance 'rule-set :name name :rules-list rules)))
    (let ((rh (rules-hash rs)))
      (mapc #'(lambda (rule)
                (setf (gethash (car rule) rh) rule))
            rules)
      (mapc #'(lambda (inh-name)
                (let ((inh-rs  (lookup-rule-set *indexstyle* inh-name)))
                  (when inh-rs
                    (inherit-rule-set rs inh-rs))))
            inherit)
      rs)))

#|
(add-rule-set *indexstyle*
              (make-rule-set "foo" '() '(("foo" "bar" :again)
                                         ("baz" "bam" :string)
                                         ("1"   "3"))))

(add-rule-set *indexstyle*
              (make-rule-set "bar" '() '(("1" "bar" :again)
                                         ("2" "bam" :string))))
|#

(defmacro instantiate-rule-set (run rule-set-names)
  (let ((rs (make-rule-set "temporary rule-set" rule-set-names '())))
    (when rs
      (cons 'progn
            (mapcar #'(lambda (rule)
                        `(SORT-RULE ,@rule :RUN ,RUN))
                    (rules-list rs))))))



(defun inherit-rule-set (rs parent)
  (let ((rs-hash (rules-hash rs)))
    (mapc #'(lambda (rule)
              (let ((pattern (car rule)))
                (unless (gethash pattern rs-hash)
                  (setf (gethash pattern rs-hash) rule)
                  (setf (rules-list rs)
                        (append (rules-list rs) (list rule))))))
          (rules-list parent))
    rs))
;; line 140 "idxclass.nw"
(defun initialize-locref-cache ()
  (setq *locref-cache-hits*   0)
  (setq *locref-cache-misses* 0)
  (make-hash-table :test #'equal
                   :size        *locref-cache-size*
                   :rehash-size *locref-cache-rehash-size*))

(defmethod clear-locref-cache ((idxclass indexclass))
  (setf (get-locref-cache idxclass)
        (initialize-locref-cache)))

(defun set-locref-cache-parameters (&key (size 1000) (rehash-size 3))
  (when (< size 100) (setq size 100))
  (when (< rehash-size 1.5) (setq rehash-size 1.5))
  (setq *locref-cache-size* size)
  (setq *locref-cache-rehash-size* rehash-size))
;; line 165 "idxclass.nw"
(defmacro lookup-locref-cache (idxcls locref-string)
  `(LET ((LOCREF-LOOKUP (GETHASH ,locref-string
                                 (SLOT-VALUE ,idxcls 'LOCREF-CACHE))))
    (IF LOCREF-LOOKUP
        (PROGN
          (INCF *LOCREF-CACHE-HITS*)
          (VALUES (CAR LOCREF-LOOKUP) (CDR LOCREF-LOOKUP)))
        (PROGN
          (INCF *LOCREF-CACHE-MISSES*)
          (VALUES NIL NIL)))))

(defmethod add-locref-cache ((idxcls indexclass)
                             (locref-string string)
                             (num    number)
                             ;; the next one is typeless, since we
                             ;; also want to store the value NIL into
                             ;; it, to denote a mismatch
                             locref)
  (setf (gethash locref-string (get-locref-cache idxcls))
        (cons num locref))
  (values num locref))

(defmethod locref-statistics ((idxcls indexclass))
  (let ((ht (get-locref-cache idxcls)))
    (with-output-to-string (s)
      (format
       s "Statistics for locref-cache of indexclass ~S:~%" (get-name idxcls))
      (format
       s
       "Hash-table: size=~A, count=~A, cache-hits=~A, cache-misses=~A, hits=~4,1,,'*F%~%"
       (hash-table-size ht) (hash-table-count ht)
       *locref-cache-hits* *locref-cache-misses*
       (if (> *locref-cache-misses* 0)
           (* 100 (/ *locref-cache-hits* (+ *locref-cache-hits*
                                            *locref-cache-misses*)))
           100.0 #| produces an invalid value |#)))))
;; line 430 "idxclass.nw"
(defmethod add ((idxcls indexclass) (basetype basetype))
  (setf (gethash (get-name basetype) (get-basetypes idxcls))
        basetype)
  idxcls)

(defmethod add-alias ((idxcls indexclass) basetype alias-name)
  (let ((new-basetype (make-copy (lookup-basetype idxcls basetype)
                                 alias-name)))
    (add idxcls new-basetype)))

(defmethod add ((idxcls indexclass) (catattr category-attribute))
  (add (get-cat-attrs idxcls) catattr)
  idxcls)
;; line 446 "idxclass.nw"
(defun add-letter-groups (idxcls name prefixes &key before after)
  (let ((lgm (get-letter-groups idxcls)))
    (mapc #'(lambda (prefix)
              (add-letter-group-to-manager lgm name prefix
                                           :before before
                                           :after  after))
          prefixes)))
;; line 462 "idxclass.nw"
(defmethod add ((idxcls indexclass) (loccls layered-location-class))
  (let* ((loccls-name (get-name loccls))
         (old-loccls (cdr (lookup-locref-class idxcls loccls-name))))
    (when old-loccls
      (remove-location-class idxcls old-loccls))
    (if (get-locref-classes idxcls) #| if it is empty, we can't rplacd |#
        (rplacd (last (get-locref-classes idxcls))
                (acons loccls-name loccls nil))
        (setf (get-locref-classes idxcls)
              (acons loccls-name loccls nil))))
  idxcls)

(defmethod remove-location-class ((idxcls indexclass)
                                  (loccls layered-location-class))
  (setf (get-locref-classes idxcls)
        (delete-if #'(lambda (elt)
                       (eq (cdr elt) loccls))
                   (get-locref-classes idxcls)))
  idxcls)
;; line 484 "idxclass.nw"
(defmethod add ((idxcls indexclass) (xrefcls crossref-location-class))
  (let ((xrefcls-name (get-name xrefcls)))
    (setf (get-xref-classes idxcls)
          (acons xrefcls-name xrefcls (get-xref-classes idxcls)))
    idxcls))
;; line 495 "idxclass.nw"
(defmethod add-merge-rule ((idxcls indexclass)
                           (catattr-1 category-attribute)
                           (catattr-2 category-attribute))
  (rel-insert (get-merge-rules idxcls) catattr-1 catattr-2)
  idxcls)

(defmethod add-drop-if-merged-rule ((idxcls indexclass)
                                    (catattr-1 category-attribute)
                                    (catattr-2 category-attribute))
  (rel-insert (get-merge-rules idxcls) catattr-1 catattr-2)
  (rel-insert (get-drop-if-merged-rules idxcls) catattr-1 catattr-2)
  idxcls)
;; line 512 "idxclass.nw"
(defmethod add-implicit-substitution-rule (
                                           (idxcls indexclass)
                                           (catattr-1 category-attribute)
                                           (catattr-2 category-attribute))
  (rel-insert (get-implicit-subst-rules idxcls) catattr-1 catattr-2)
  idxcls)


(defmethod add-explicit-substitution-rule (
                                           (idxcls indexclass)
                                           (catattr-1 category-attribute)
                                           (catattr-2 category-attribute))
  (rel-insert (get-explicit-subst-rules idxcls) catattr-1 catattr-2)
  idxcls)
;; line 609 "idxclass.nw"
(defmethod add-rule-set ((idxcls indexclass) (rs rule-set))
  (let ((rule-sets (get-rule-sets idxcls))
        (rs-name (get-name rs)))
    (setf (gethash rs-name rule-sets) rs)))
;; line 635 "idxclass.nw"
(defmethod lookup-basetype ((idxcls indexclass) (basetype-name string))
  (gethash basetype-name (get-basetypes idxcls)))

(defmethod lookup-catattr ((idxcls indexclass) (catattr-name string))
  (lookup-catattr-name (slot-value idxcls 'cat-attrs) catattr-name))

(defmethod lookup-location-class ((idxcls indexclass) (loccls-name string))
  (or (lookup-crossref-class idxcls loccls-name)
      (lookup-locref-class idxcls loccls-name)))

(defmethod lookup-crossref-class ((idxcls indexclass) (xref-name string))
  (assoc xref-name (slot-value idxcls 'xref-classes) :test #'equal))

(defmethod lookup-locref-class ((idxcls indexclass) (loccls-name string))
  (assoc loccls-name (slot-value idxcls 'locref-classes) :test #'equal))

(defmethod lookup-rule-set ((idxcls indexclass) (rule-set-name string))
  (gethash rule-set-name (get-rule-sets idxcls)))
;; line 685 "idxclass.nw"
(defmacro map-location-classes (idxcls apply-func &rest arguments)
  `(LET ()
    (APPLY #'NCONC
           (MAPCAR
            #'(LAMBDA (ELT)
                (LET ((RES
                       (FUNCALL ,apply-func
                                ,idxcls
                                (CDR ELT) #|ELT is ("name".<LOCATION-CLASS>))|#
                                ,@arguments)))
                  (WHEN RES (LIST RES))))
            (GET-LOCREF-CLASSES ,idxcls)))))

;; line 800 "idxclass.nw"
(defmethod apply-substitution-hooks ((idxcls indexclass))
  (let ((impl (get-implicit-subst-rules idxcls))
        (expl (get-explicit-subst-rules idxcls)))
    (funcall (get-implicit-subst-hook idxcls) impl)
    (funcall (get-explicit-subst-hook idxcls) expl)
    (rel-merge impl expl)
    (set-explicit-subst-rules nil idxcls)
    (funcall (get-global-subst-hook idxcls) impl)
    (make-ready impl)
    ))
;; line 838 "idxclass.nw"
(defmethod make-ready ((idxcls indexclass))
  (apply-substitution-hooks idxcls)
  (make-ready (get-merge-rules idxcls))
  (make-ready (get-drop-if-merged-rules idxcls))
  (make-ready (get-letter-groups idxcls))


;;  (set-merge-rule-topsort (topsort
;;                         (remove-reflexivity
;;                          (transform-relation
;;                           (get-drop-if-merged-rules idxcls)
;;                           #'(lambda (attr)
;;                               (get-ordnum attr)))))
;;                        *indexstyle*)

  ;;(pprint (get-drop-if-merged-rules idxcls))
  ;;(pprint (topsort (get-drop-if-merged-rules idxcls)))

  (let ((counter 0))
    (mapc #'(lambda (catattr)
              (set-processing-ordnum (incf counter) catattr))
          (reverse (topsort (get-drop-if-merged-rules idxcls)))))

  idxcls)

;; line 888 "idxclass.nw"
(eval-when (compile load eval)

;; line 109 "idxclass.nw"
(export '(indexclass              make-indexclass
          get-name                get-basetypes
          get-cat-attrs           get-xref-classes
          get-succ-table          get-locref-classes
          get-rule-sets

;; line 241 "idxclass.nw"
          get-merge-rules
          get-drop-if-merged-rules
;          get-merge-rule-topsort   set-merge-rule-topsort
          get-implicit-subst-rules set-implicit-subst-rules
          get-explicit-subst-rules set-explicit-subst-rules
;; line 115 "idxclass.nw"
          ))
;; line 204 "idxclass.nw"
(export '(locref-cache-size*      *locref-cache-rehash-size*
          initialize-locref-cache clear-locref-cache
          lookup-locref-cache     add-locref-cache
          locref-statistics))
;; line 409 "idxclass.nw"
(export '(letter-group-definition match-letter-group))
;; line 530 "idxclass.nw"
(export '(add set
          add-letter-groups
          add-merge-rule
          add-drop-if-merged-rule
          add-implict-substitution-rule
          add-explicit-substitution-rule))
;; line 616 "idxclass.nw"
(export '(rule-set
          make-rule-set
          add-rule-set
          instantiate-rule-set
          inherit-rule-set))
;; line 656 "idxclass.nw"
(export '(lookup-basetype
          lookup-catattr
          lookup-letter-group-definition
          lookup-location-class
          lookup-crossref-class
          lookup-locref-class
          lookup-rule-set))
;; line 700 "idxclass.nw"
(export 'map-location-classes)
;; line 730 "idxclass.nw"
(export '(get-merge-drop-hook set-merge-drop-hook))
;; line 791 "idxclass.nw"
(export '(get-implicit-subst-hook     set-implicit-subst-hook
          get-explicit-subst-hook     set-explicit-subst-hook
          get-global-subst-hook       set-global-subst-hook
          get-substitution-subst-hook set-substitution-subst-hook))
;; line 823 "idxclass.nw"
(export '(apply-substitution-hooks))
;; line 865 "idxclass.nw"
(export '(make-ready))
;; line 890 "idxclass.nw"
  )
;; line 54 "idxstyle.nw"
(defun create-indexclass (idxcls-name)
  (setq *indexstyle* (make-indexclass idxcls-name)))
;; line 259 "letters.nw"
;; $Id$

;; line 37 "letters.nw"
(defclass letter ()
    ((name  :initarg :name  :type string)
     (props :initarg :props :type list)))

(defun make-letter (name properties inherits-from)
  (make-instance 'letter
                 :name  name
                 :props (if inherits-from
                            (append properties (get-props inherits-from))
                            properties)))

(defun find-prop (letter prop)
  (assoc prop (get-props letter)))
;; line 53 "letters.nw"
(defclass letter-dictionary ()
    ((letters :initform (make-hash-table :test #'equal))))

(defun make-letter-dict ()
  (make-instance 'letter-dictionary))

(defun find-letter* (dict-ht letter)
  (gethash letter dict-ht))

(defun find-letter (dict letter)
  (gethash letter (get-letters dict)))

(defun add-letter-to-dict (letter-dict
                           po-dict
                           name props
                           &optional
                           inherits)
  (let ((dict-ht (get-letters letter-dict)))
    (when (gethash name dict-ht)
      (nraw "Letter `~S' redefined!" name))
    (setf (gethash name dict-ht)
          (make-letter name
                       (pre-calculate-props props po-dict)
                       (when inherits
                         (find-letter* dict-ht name))))))
;; line 85 "letters.nw"
(defun pre-calculate-props (props po-dict)
  (mapc #'(lambda (po-spec)
            (let ((po-name (car po-spec))
                  (po-val  (cadr po-spec)))
              (setf (cdr (last po-spec))
                    (list (find-prop-weight
                           (find-partial-order po-dict po-name)
                           po-val)))))
        props)
  props)
;; line 98 "letters.nw"
(defmacro define-letter (&whole whole &rest rest)
  (destructuring-switch-bind (name &rest proplist)
      rest
    (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 102 "letters.nw"
          ((not (listp proplist))
             (error "~S is not a property list in ~S." proplist whole))
          (t `(LET ((NAME (STRINGIFY ',name)))
               (ADD-LETTER-TO-DICT
                (GET-LETTER-DICT *INDEXSTYLE*)
                (GET-PARTIAL-ORDER-DICT *INDEXSTYLE*)
                ,name ',proplist))))))
;; line 119 "letters.nw"
(defclass partial-order ()
    ((name   :initarg :name   :type string)
     (order  :initform (make-hash-table :test #'equal))
     (direct :initarg :direct :type symbol)))

(defun make-partial-order (name order &optional (direct :from-start))
  (let* ((po (make-instance 'partial-order :name name :direct direct))
         (po-ht (get-order po))
         (ctr 0))
    (mapc #'(lambda (elt)
              (when (gethash elt po-ht)
                (error "Element ~S doubly defined in partial order ~S."
                       elt name))
              (setf (gethash elt po-ht) (incf ctr)))
          order)
    po))
;; line 140 "letters.nw"
(defclass partial-order-dictionary ()
    ((order :initform (make-hash-table :test #'equal))))

(defun make-partial-order-dict ()
  (make-instance 'partial-order-dictionary))

(defun find-partial-order (dict po-name)
  (gethash po-name (get-order dict)))

(defun find-prop-weight (po po-name)
  (gethash po-name (get-order po)))

(defun add-partial-order-to-dict (dict po-name po-order po-direct)
  (let ((dict-ht (get-order dict)))
    (when (gethash po-name dict-ht)
      (nraw "Partial order ~S redefined!" po-name))
    (setf (gethash po-name dict-ht)
          (make-partial-order po-name po-order po-direct))))
;; line 161 "letters.nw"
(defmacro define-partial-order (&whole whole &rest rest)
  (destructuring-switch-bind (name
                              ordlist
                              &switch
                              from-start
                              from-end)
      rest
    (cond
;; line 239 "intface.nw"
((null name)
 (error "missing argument <name> in `~S' !" whole))
((not (or (symbolp name) (stringp name)))
 (error "~S is not a symbol or a string in `~S' !" name whole))
;; line 169 "letters.nw"
          ((not (listp ordlist))
             (error "~S is not a partial order list in ~S." ordlist whole))
          ((and from-start from-end)
             (error "Only one of :from-start or :from-end is allowed in ~S."
                    whole))
          (t `(LET ((NAME (STRINGIFY ',name)))
               (ADD-PARTIAL-ORDER-TO-DICT
                (GET-PARTIAL-ORDER-DICT *INDEXSTYLE*)
                ,name ',ordlist
                ,(if (or from-start (not from-end))
                     :from-start :from-end)))))))
;; line 194 "letters.nw"
(defun word-cmp (word1 word2 po)
  (let ((po-name (get-name po))
        let1 let2 weight1 weight2)
    (loop
      (cond
        ((and (endp word1) (endp word2)) (return :equal))
        ((endp word1) (return :less))
        ((endp word2) (return :greater))
        (t (setq let1 (car word1))
           (setq word1 (cdr word1))
           (setq let2 (car word2))
           (setq word2 (cdr word2))
           (setq weight1 (lookup-letter-weight let1 po-name))
           (setq weight2 (lookup-letter-weight let2 po-name))
           (when (< weight1 weight2) (return :less))
           (when (> weight1 weight2) (return :greater)))))))

(defun calc-list-of-weights (word po)
  (let ((po-name (get-name po)))
    (mapcar #'(lambda (letter)
                (lookup-letter-weight letter po-name))
            word)))
;; line 227 "letters.nw"
(defun lookup-letter-weight (letter po-name)
  (let ((prop (find-prop letter po-name)))
    (if prop
        (car (last prop))
        (find-prop-weight
         (find-partial-order (get-partial-order-dict *indexstyle*) po-name)
         'others))))
;; line 237 "letters.nw"
(defun make-word (letter-list)
  (let ((dict (get-letter-dict *indexstyle*)))
    (mapcar #'(lambda (str)
                (find-letter dict str))
            letter-list)))

(defun sort-words (word-list po)
  (split-list #'(lambda (word)
                  (calc-list-of-weights word po))
              word-list))



;; line 99 "idxstyle.nw"
(create-indexclass "default")
;; line 107 "idxstyle.nw"
(defmacro string-expand (str)
  (let ((str-expansion
         (map 'list
              #'(lambda (c)
                  (make-string 1 :initial-element c))
              str)))
    `(QUOTE ,str-expansion)))

(define-alphabet* "ALPHA"  (string-expand "ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
(define-alphabet* "alpha"  (string-expand "abcdefghijklmnopqrstuvwxyz"))
(define-alphabet* "digits" (string-expand "0123456789"))
;; line 128 "idxstyle.nw"
(defun prefix-match-for-radix-numbers (str radix)
  (let* ((n nil)
         (strlen (length str))
         (len-match (do ((i 0 (1+ i)))
                        ((or (>= i strlen)
                             (not (digit-char-p (char str i) radix)))
                           i)
                      (setq n (+ (* (if n n 0) radix)
                                 (digit-char-p (char str i) radix))))))
    (values (subseq str 0 len-match)
            (subseq str len-match)
            n)))
;; line 155 "idxstyle.nw"
(define-enumeration "arabic-numbers"
                    #'(lambda (str)
                        (prefix-match-for-radix-numbers str 10))
                    "0123456789")
;; line 178 "idxstyle.nw"
(defparameter *lowercase-roman-numbers-lookup-table*
  '((#\i ("ix" . 9) ("iv" . 4) ("iiii" . 4) ("iii". 3) ("ii" . 2) ("i" . 1))
    (#\v ("viiii" . 9)  ("viii" . 8)  ("vii" . 7)  ("vi" . 6)  ("v" . 5))
    (#\x ("xc" . 90) ("xl" . 40) ("x" . 10))
    (#\m ("m" . 1000))
    (#\c ("cm" . 900) ("cd" . 400) ("c" . 100))
    (#\l ("lxxxx" . 90) ("lxxx" . 80) ("lxx" . 70) ("lx" . 60) ("l" . 50))
    (#\d ("dcccc" . 900) ("dccc" . 800) ("dcc" . 700) ("dc" . 600) ("d" . 500))
    ))

(defparameter *uppercase-roman-numbers-lookup-table*
  '((#\I ("IX" . 9) ("IV" . 4) ("IIII" . 4) ("III". 3) ("II" . 2) ("I" . 1))
    (#\V ("VIIII" . 9)  ("VIII" . 8)  ("VII" . 7)  ("VI" . 6)  ("V" . 5))
    (#\X ("XC" . 90) ("XL" . 40) ("X" . 10))
    (#\M ("M" . 1000))
    (#\C ("CM" . 900) ("CD" . 400) ("C" . 100))
    (#\L ("LXXXX" . 90) ("LXXX" . 80) ("LXX" . 70) ("LX" . 60) ("L" . 50))
    (#\D ("DCCCC" . 900) ("DCCC" . 800) ("DCC" . 700) ("DC" . 600) ("D" . 500))
    ))
;; line 206 "idxstyle.nw"
(defun roman-number-prefix-match (roman-number-string *table*)
  (let ((result 0)
        (str roman-number-string)
        (last-val 100000) #| hope that's enough |#
        (str-len (length roman-number-string))
        ch table match-cons curr-val
        )
    (loop (when (= 0 (length str))
            (return (values roman-number-string "" result)))
          (setq ch (aref str 0))
          (setq table (cdr (assoc ch *table*)))
          (setq match-cons
                (find-if #'(lambda (cons)
                             (let* ((prefix (car cons))
                                    (match-len (string/= prefix str)))
                               (or (not match-len)
                                   (= match-len (length prefix)))))
                         table))
          (unless match-cons
            (return (if (= 0 result)
                        (values nil nil nil)
                        (values (subseq roman-number-string
                                        0 (- str-len (length str)))
                                str result))))
          (setq curr-val (cdr match-cons))
          (when (> curr-val last-val)
            (return (values (subseq roman-number-string
                                    0 (- str-len (length str)))
                            str result)))
          (incf result curr-val)
          (setq last-val curr-val)
          (setq str (subseq str (length (car match-cons)))))))
;; line 241 "idxstyle.nw"
(define-enumeration "roman-numbers-uppercase"
                    #'(lambda (str)
                        (roman-number-prefix-match
                         str
                         *uppercase-roman-numbers-lookup-table*))
                    "IVXLCDM")

(add-alias *indexstyle* "roman-numbers-uppercase" "ROMAN")
(add-alias *indexstyle* "roman-numbers-uppercase" "roman-numerals-uppercase")

(define-enumeration "roman-numbers-lowercase"
                    #'(lambda (str)
                        (roman-number-prefix-match
                         str
                         *lowercase-roman-numbers-lookup-table*))
                    "ivxlcdm")

(add-alias *indexstyle* "roman-numbers-lowercase" "roman")
(add-alias *indexstyle* "roman-numbers-lowercase" "roman-numerals-lowercase")


;; line 902 "idxclass.nw"
#+:XP
(defmacro pprint-slot-block (slot-name body)
  `(progn
    (pprint-logical-block (s nil :suffix " }")
                          (pprint-indent :block 2 s)
                          (write-string ,slot-name s)
                          (pprint-newline :mandatory s)
                          ,body)
    (pprint-newline :mandatory s)))

#+:XP
(defmacro pprint-slot-block-line (slot-name body)
  `(progn
    (pprint-logical-block (s nil :suffix " }")
                          (pprint-indent :block 2 s)
                          (write-string ,slot-name s)
                          (pprint-newline :linear s)
                          ,body)
    (pprint-newline :mandatory s)))
;; line 924 "idxclass.nw"
#+:XP
(set-pprint-dispatch 'indexclass
                     #'(lambda (s idxcls)
                         (pprint-logical-block
                          (s nil :suffix "}")
                          (pprint-indent :block 2 s)
                          (write-string "indexclass " s)
                          (write (get-name idxcls) :stream s)
                          (write-string " {" s)
                          (pprint-newline :mandatory s)
                          (pprint-slot-block
                           "basetypes {"
                           (maphash #'(lambda (key bt)
                                        (declare (ignore key))
                                        (write (get-name bt) :stream s)
                                        (write-string " " s)
                                        (pprint-newline :linear s))
                                    (get-basetypes idxcls)))
                          (pprint-slot-block
                           "cat-attrs {"
                           (write (get-cat-attrs idxcls) :stream s))
                          (pprint-slot-block
                           "locclasses {"
                           (mapc #'(lambda (lc)
                                     (write (get-name (cdr lc)) :stream s)
                                     (write-string " [" s)
                                     (write (get-ordnum (cdr lc)) :stream s)
                                     (write-string "] " s)
                                     (pprint-newline :linear s))
                                 (get-locref-classes idxcls)))
                          (pprint-slot-block
                           "xrefclasses {"
                           (mapc #'(lambda (lc)
                                     (write (get-name (cdr lc)) :stream s)
                                     (write-string " [" s)
                                     (write (get-ordnum (cdr lc)) :stream s)
                                     (write-string "] " s)
                                     (pprint-newline :linear s))
                                 (get-xref-classes idxcls)))

                          (write (get-letter-dict idxcls) :stream s)
                          (pprint-newline :mandatory s)
                          (write (get-partial-order-dict idxcls) :stream s)
                          (pprint-newline :mandatory s)

                          (pprint-slot-block
                           "merge-rules {"
                           (write (get-merge-rules idxcls) :stream s))
                          (pprint-slot-block
                           "drop-if-merged-rules {"
                           (write (get-drop-if-merged-rules idxcls) :stream s))
                          ;; (pprint-slot-block-line
                          ;; "merge-rule-topsort { "
                          ;; (write (get-merge-rule-topsort idxcls) :stream s))
                          (pprint-slot-block
                           "implicit-subst-rules {"
                           (write (get-implicit-subst-rules idxcls) :stream s))
                          (pprint-slot-block
                           "explicit-subst-rules {"
                           (write (get-explicit-subst-rules idxcls) :stream s))

                          (pprint-slot-block-line
                           "implicit-subst-hook {"
                           (write (get-implicit-subst-hook idxcls) :stream s))
                          (pprint-slot-block-line
                           "explicit-subst-hook {"
                           (write (get-explicit-subst-hook idxcls) :stream s))
                          (pprint-slot-block-line
                           "global-subst-hook {"
                           (write (get-global-subst-hook idxcls) :stream s))
                          (pprint-slot-block-line
                           "substitution-subst-hook {"
                           (write (get-substitution-subst-hook idxcls) :stream s))
                          (pprint-slot-block-line
                           "merge-drop-hook {"
                           (write (get-merge-drop-hook idxcls) :stream s))

                                        ;                          (pprint-slot-block
                                        ;                           "keyword-markup {"
                                        ;                           (write (get-keyword-markup idxcls) :stream s))
                          )))
;; line 580 "idxrule.nw"
#+:XP
(defun pprint-relation (s rel)
  (pprint-logical-block
   (s nil :suffix "}")
   (pprint-indent :block 2 s)
   (write-string "Relation { " s)
   (pprint-logical-block (s nil)
                         (pprint-indent :block 2)
                         (mapc #'(lambda (x)
                                   (when x
                                     (write (car x) :stream s)
                                     (write-string " ->" s)
                                     (mapc #'(lambda (y)
                                               (write-string " " s)
                                               (write y :stream s))
                                           (cdr x))
                                     (pprint-newline :mandatory s)))
                               (get-rel-sets rel)))))

#+:XP
(set-pprint-dispatch 'relation-rule #'pprint-relation)
#+:XP
(set-pprint-dispatch 'relation      #'pprint-relation)
;; line 606 "idxrule.nw"
#+:XP
(set-pprint-dispatch
 'catattr-groups
 #'(lambda (s catgrp)
     (pprint-logical-block
      (s nil :suffix "}")
      (pprint-indent :block 2 s)
      (write-string "Category-attribute-group { " s)
      (pprint-newline :mandatory s)
      (write (get-attribute-list catgrp) :stream s)
      (pprint-newline :mandatory s)
      (write (get-attribute-table catgrp) :stream s))))
;; line 277 "letters.nw"
#+:XP
(set-pprint-dispatch 'letter
                     #'(lambda (s lt)
                         (pprint-logical-block
                          (s nil :suffix ">")
                          (pprint-indent :block 2 s)
                          (write-string "<" s)
                          (write (get-name lt) :stream s)
                          (write-string " " s)
                          (pprint-newline :linear s)
                          (write (get-props lt) :stream s))))

#+:XP
(set-pprint-dispatch 'letter-dictionary
                     #'(lambda (s dict)
                         (pprint-logical-block
                          (s nil :suffix "}")
                          (pprint-indent :block 2 s)
                          (write-string "letters {" s)
                          (maphash #'(lambda (key lt)
                                       (declare (ignore key))
                                       (write lt :stream s)
                                       (write-string " " s)
                                       (pprint-newline :linear s))
                                   (get-letters dict)))))

#+:XP
(set-pprint-dispatch 'partial-order
                     #'(lambda (s po)
                         (pprint-logical-block
                          (s nil :suffix ">")
                          (pprint-indent :block 2 s)
                          (write-string "<" s)
                          (write (get-name po) :stream s)
                          (pprint-newline :linear s)
                          (maphash #'(lambda (key po)
                                       (write-string " [" s)
                                       (write key :stream s)
                                       (write-string " " s)
                                       (write po :stream s)
                                       (write-string "]" s)
                                       (pprint-newline :linear s))
                                   (get-order po)))))


#+:XP
(set-pprint-dispatch 'partial-order-dictionary
                     #'(lambda (s dict)
                         (pprint-logical-block
                          (s nil :suffix "}")
                          (pprint-indent :block 2 s)
                          (write-string "partial-orders {" s)
                          (maphash #'(lambda (key po)
                                       (declare (ignore key))
                                       (write po :stream s)
                                       (write-string " " s)
                                       (pprint-newline :linear s))
                                   (get-order dict)))))

;; line 321 "idxstyle.nw"
(defvar *RCS-Identifier* '(
;; line 894 "idxclass.nw"
("idxclass" . "$Id: idxclass.nw,v 1.20 1997/03/26 16:18:16 kehr Exp $")
;; line 572 "idxrule.nw"
("idxrule" . "$Id: idxrule.nw,v 1.10 1997/03/26 16:18:19 kehr Exp $")
;; line 335 "idxstyle.nw"
("idxstyle" . "$Id: idxstyle.nw,v 1.19 1997/03/26 16:18:23 kehr Exp $")
;; line 1174 "intface.nw"
("intface" . "$Id: intface.nw,v 1.30 1997/10/20 11:25:09 kehr Exp $")
;; line 269 "letters.nw"
("letters" . "$Id$")
;; line 321 "idxstyle.nw"
                                               ))

(eval-when (compile load eval)

;; line 59 "idxstyle.nw"
(export 'create-indexclass)
;; line 89 "idxstyle.nw"
(export '*load-paths*)
;; line 325 "idxstyle.nw"
  )
