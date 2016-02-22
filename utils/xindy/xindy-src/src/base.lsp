;; line 30 "base.nw"
(lisp:defpackage "BASE")
(lisp:in-package "BASE")
(lisp:provide "base")

#+:CMU17 (defpackage "CLOS")

(eval-when (compile load eval)
  (lisp:use-package "CLOS")
  (lisp:use-package "COMMON-LISP")
  #+CLISP (lisp:use-package "EXT")
  )

(eval-when (eval load compile)
  #+CLISP (when (find-package "XP")
            (pushnew :XP *features*))
  #+(or ALLEGRO :CMU17) (pushnew :XP *features*)
  )

(setq *print-pretty* t)
(setq *print-escape* t)

(defparameter *mappings-trace* nil)
(defparameter *locref-trace*   nil)
(eval-when (compile load eval)
  (export '(*mappings-trace* *locref-trace*)))

;; line 104 "base.nw"
#-CLISP
(defmacro simple-condition-format-string (&rest args)
  `(simple-condition-format-control ,@args))
#-CLISP
(eval-when (compile load eval)
  (export '(simple-condition-format-string)))
;; line 133 "base.nw"
(defmacro info (&rest msg)
  `(APPLY #'FORMAT *STANDARD-OUTPUT* (LIST ,@msg)))
;; line 138 "base.nw"
(defparameter *nraw-error-message* "~&WARNING: ")

(defmacro nraw (&rest msg)
  `(PROGN
    (FORMAT *ERROR-OUTPUT* BASE::*NRAW-ERROR-MESSAGE*)
    (APPLY #'FORMAT *ERROR-OUTPUT* (LIST ,@msg))))
;; line 147 "base.nw"
(defparameter *oops-error-message* "~&ERROR: ")

(defmacro oops (&rest msg)
  `(LET ((MSG (LIST ,@msg)))
    (FORMAT *ERROR-OUTPUT* BASE::*OOPS-ERROR-MESSAGE*)
    (APPLY #'FORMAT *ERROR-OUTPUT* MSG)
    (WHEN *LOGGING-ON*
      (FORMAT *LOGGING-STREAM* BASE::*OOPS-ERROR-MESSAGE*)
      (APPLY #'FORMAT *LOGGING-STREAM* MSG))))
;; line 162 "base.nw"
(defmacro oops* (fmt args)
  `(LET ((FMT  ,fmt)
         (ARGS ,args))
    (FORMAT *ERROR-OUTPUT* BASE::*OOPS-ERROR-MESSAGE*)
    (APPLY #'FORMAT *ERROR-OUTPUT* FMT ARGS)
    (WHEN *LOGGING-ON*
      (FORMAT *LOGGING-STREAM* BASE::*OOPS-ERROR-MESSAGE*)
      (APPLY #'FORMAT *LOGGING-STREAM* FMT ARGS))))
;; line 183 "base.nw"
(defparameter *logging-stream* nil)
(defparameter *logging-on*     nil)

(defmacro gol (flag &rest msg)
  `(WHEN (AND ,flag *LOGGING-ON*)
    (APPLY #'FORMAT *LOGGING-STREAM* (list ,@msg))))
;; line 74 "base.nw"
(defmacro ifnot (expr then-body else-body)
  `(IF (NOT ,expr) ,then-body ,else-body))
;; line 82 "base.nw"
(defun error-exit ()
  #+CLISP (exit 1)
  #+ALLEGRO (excl:exit 1)
  #+CMU17 (unix:unix-exit 1)
  )

(defun exit-normally ()
  #+CLISP (exit 0)
  #+ALLEGRO (excl:exit 0)
  #+CMU17 (unix:unix-exit 0)
  )
;; line 201 "base.nw"
(defmacro assert! (cond &optional msg)
  `(UNLESS ,cond
    (FORMAT *ERROR-OUTPUT* "Internal error! Assertion ~A failed!" ',cond)
    ,(if msg
         `(ERROR "~S" ,msg)
         `(ERROR "Entering debugger!"))))

(pushnew :ASSERT! *features*)
;; line 218 "base.nw"
(defmacro define-slot-accessors (accessors)
  `(eval-when (compile load eval)
    ,@(apply #'nconc
             (mapcar
              #'(lambda (acc)
                  (let ((getnam (intern (concatenate 'string "GET-"
                                                     (symbol-name acc))))
                        (setnam (intern (concatenate 'string "SET-"
                                                     (symbol-name acc))))
                        )
                    `((defmacro ,getnam (obj)
                        (list 'slot-value obj '',acc))
                      (defmacro ,setnam (val obj)
                        (list 'setf (list 'slot-value obj '',acc) val))
                      (export '(,getnam ,setnam ,acc)))))
              accessors))))

(define-slot-accessors (
                        attribute
                        base-alphabet
                        basetype
                        catattr
                        catattr-grp-ordnum
                        entries
                        entry-list
                        entry-table
                        first
                        group-definition
                        hierdepth
                        idxclass
                        inner
                        join-length
                        last
                        last-in-group
                        layers
                        length
                        letters
                        locclass
                        locref-cache
                        locref-string
                        locrefs
                        members
                        merge-key
                        sort-key
                        print-key
                        main-key
                        name
                        order
                        ordnum
                        ordnums
                        origin
                        processing-ordnum
                        props
                        rangeattr
                        separator
                        sort-ordnum
                        state
                        subentries
                        subrefs
                        symbols
                        target
                        type
                        ))
;; line 297 "base.nw"
(defun split-list (split-function list &key sortfunc headfunc)
  #+:ASSERT! (assert (not (and sortfunc headfunc))
                     ()
                     "split-list")
  (let ((list-len (length list)))
    (if (= 1 list-len)
        (list list)
        (let ((split-table (make-hash-table :test #'equal
                                            :size (round (* 1.2 list-len))))
              (key-list '()))
          (mapc #'(lambda (elt)
                    (let ((key (funcall split-function elt)))
                      (setf (gethash key split-table)
                            (cons elt (gethash key split-table)))))
                list)
          (maphash #'(lambda (key element)
                       (declare (ignore element))
                       (push key key-list))
                   split-table)
          (maplist #'(lambda (rest-list)
                       (let ((entries (gethash (car rest-list) split-table)))
                         (if headfunc
                             (let ((head (find-if headfunc entries)))
                               (rplaca rest-list
                                       (if head
                                           (cons head (delete head entries))
                                           entries)))
                             (rplaca rest-list entries))))
                   (if sortfunc (sort key-list sortfunc) key-list))
          key-list))))

#|
This version seems to be a little bit slower than the above one,
despite the fact that we use a temporary array with a fill-pointer.

(defun split-list (split-function sort-function list)
  (let ((list-len (length list)))
    (if (= 1 list-len)
        (list list)
        (let ((split-table (make-hash-table :test #'equal
                                            :size (round (* 1.5 list-len)))))
          (mapc #'(lambda (elt)
                    (let ((key (funcall split-function elt)))
                      (setf (gethash key split-table)
                            (cons elt (gethash key split-table)))))
                list)
          (let ((sort-array (make-array (hash-table-count split-table)
                                        :fill-pointer 0)))
            (maphash #'(lambda (key element)
                         (declare (ignore element))
                         (vector-push key sort-array))
                     split-table)
            (map 'list
                 #'(lambda (key)
                     (gethash key split-table))
                 (sort sort-array sort-function)))))))
|#

;; line 60 "base.nw"
(defvar *RCS-Identifier* '(
;; line 68 "base.nw"
("base" . "$Id: base.nw,v 1.20 1997/03/26 16:17:58 kehr Exp $")
;; line 60 "base.nw"
                                               ))

(eval-when (compile load eval)

;; line 96 "base.nw"
(export '(ifnot error-exit exit-normally))
;; line 192 "base.nw"
(export '(oops oops* nraw info gol
          *logging-stream* *logging-on*))
;; line 212 "base.nw"
(export 'assert!)
;; line 357 "base.nw"
(export '(split-list))
;; line 64 "base.nw"
  )
