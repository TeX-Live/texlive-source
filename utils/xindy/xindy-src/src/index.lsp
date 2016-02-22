;; line 835 "index.nw"
;; $Id: index.nw,v 1.31 1997/03/26 16:18:37 kehr Exp $

(lisp:defpackage "INDEX")
(lisp:in-package "INDEX")
(lisp:provide "index")

#+CLISP (lisp:require "base")
#+CLISP (lisp:require "locref")
#+CLISP (lisp:require "idxstyle")
#+CLISP (lisp:require "ordrules")

(eval-when (compile load eval)
  (lisp:use-package "CLOS")
  #+(and :XP CLISP) (lisp:use-package "XP")
  #-CLISP (lisp:require "base")
  #-CLISP (lisp:require "locref")
  #-CLISP (lisp:require "idxstyle")
  (lisp:use-package "BASE")
  (lisp:use-package "LOCREF")
  (lisp:use-package "IDXSTYLE"))

;; The following construct must be separated since the definition of
;; the *feature* :ORDRULES was added via loading the package
;; "idxstyle" but is only active *after* reading the whole SEXP.

(eval-when (compile load eval)
  #+:ORDRULES (use-package "ORDRULES"))

;; line 986 "idxentry.nw"
;; $Id: idxentry.nw,v 1.29 1997/03/27 17:17:25 kehr Exp $

;; line 1002 "idxentry.nw"
(defvar *number-of-indexentries*)
(defvar *current-number*)
;; line 1007 "idxentry.nw"
(defvar *percentage-list*)
(defvar *processing-percentage-list*)

(defun print-rest-of-percentages (plist)
  (dolist (percent plist)
    (when (<= (cdr percent) 100)
      (print-percent percent))))

(defun print-percent (percent-cons)
  (info " [~A%]" (cdr percent-cons)))
;; line 85 "idxentry.nw"
(defclass index-entry ()
    ((main-key   :initarg :main-key)
     (merge-key  :initarg :merge-key)
     (sort-key   :initarg :sort-key)
     (print-key  :initarg :print-key)
     (locrefs    :initarg :locrefs)
     (idxclass   :initarg :idxclass)
     (subentries :initform '())))

(defun make-index-entry (&key (main-key '()) (merge-key '())
                              (sort-key '()) (print-key '())
                              (locrefs '())  (idxclass nil)
                              &allow-other-keys)
  (make-instance 'index-entry
                 :main-key main-key :merge-key merge-key
                 :sort-key sort-key :print-key print-key
                 :locrefs  locrefs  :idxclass  idxclass))
;; line 108 "idxentry.nw"
(defvar *currently-processed-indexentry*)
;; line 116 "idxentry.nw"
(defmacro add-location-reference-to-indexentry (idxent locref)
  `(PROGN
    (UNLESS (SOME #'(LAMBDA (LREF)
                      (LOCREF= ,locref LREF))
                  (GET-LOCREFS ,idxent))
      (SET-LOCREFS (CONS ,locref (GET-LOCREFS ,idxent))
                   ,idxent))
    ,idxent))
;; line 249 "idxentry.nw"
(defun convert-sublists-to-group-objects (list-of-sublists
                                          object-type
                                          &key (initializer-func nil))
  (mapcar #'(lambda (sublist)
              (let ((locref-grp (make-locref-group object-type sublist)))
                (if initializer-func
                    (funcall initializer-func locref-grp)
                    locref-grp)))
          list-of-sublists))
;; line 267 "idxentry.nw"
(defun convert-sublists-to-specialized-groups (list-of-sublists
                                               convert-func
                                               &optional
                                               initializer-func)
  (mapcar #'(lambda (sublist)
              (let ((locref-grp
                     (make-locref-group (funcall convert-func sublist)
                                        sublist)))
                (if initializer-func
                    (funcall initializer-func locref-grp)
                    locref-grp)))
          list-of-sublists))
;; line 292 "idxentry.nw"
(defclass locref-group ()
    ((members :initarg :members)))

(defun make-locref-group (group-class members)
  (make-instance group-class :members members))

(defgeneric process-group (group-object indexclass))
;; line 322 "idxentry.nw"
(defclass locref-class-group (locref-group)
    ((locclass :initform 'nil)))
;; line 327 "idxentry.nw"
(defclass crossref-class-group (locref-group)
    ((locclass :initform 'nil)))
;; line 333 "idxentry.nw"
(defun process-indexentry (idxent)
  (setq *currently-processed-indexentry* idxent)
  (when (>= (incf *current-number*) (caar *processing-percentage-list*))
    (print-percent (pop *processing-percentage-list*)))
  (set-locrefs (convert-sublists-to-specialized-groups
                (split-list #'(lambda (locref)
                                (get-ordnum (get-locclass locref)))
                            (get-locrefs idxent)
                            :sortfunc #'<)
                #'(lambda (sublist)
                    (typecase (car sublist)
                      (layered-location-reference  'locref-class-group)
                      (crossref-location-reference 'crossref-class-group)
                      (t (error "internal error in `process-indexentry'"))))
                #'(lambda (locref-cls-grp)
                    (set-locclass (get-locclass
                                   (car (get-members locref-cls-grp)))
                                  locref-cls-grp)
                    locref-cls-grp))
               idxent)
  (mapc #'(lambda (locref-cls-grp)
            (process-group locref-cls-grp *indexstyle*))
        (get-locrefs idxent))
  idxent)
;; line 899 "idxentry.nw"
(defmacro take-first (list)
  `(AND ,list  (LIST (CAR ,list))))

(defun tree-location-references (locref-list
                                 max-depth
                                 &optional (curr-depth 1))
  (cond ((or (endp locref-list)         #| nothing more to do |#
             (> curr-depth max-depth))  #| we reached the end |#
           locref-list)
        (t (let ((locref-slist
                  (split-list #'(lambda (locref)
                                  (car (get-ordnums locref)))
                              locref-list
                              :sortfunc #'<)))
             (mapcar
              #'(lambda (sublist)
                  (let* ((locref  (car sublist))
                         (ordnums (get-ordnums locref)))
                    (if (= 1 (length ordnums))
                        (progn #| length = 1 |#
                          (set-subrefs
                           (sort-locrefs
                            (tree-location-references (remove-first-layers
                                                       (cdr sublist))
                                                      max-depth
                                                      (1+ curr-depth)))
                           locref)
                          locref)
                        (let #| length > 1 |#
                            ((new-locref
                              (make-layered-location-reference
                               :layers   (take-first (get-layers locref))
                               :ordnums  (take-first (get-ordnums locref))
                               :locclass (get-locclass locref)
                               :catattr  (get-last-in-group
                                          (get-catattr locref))
                               )))
                          (set-subrefs
                           (sort-locrefs
                            (tree-location-references (remove-first-layers sublist)
                                                      max-depth
                                                      (1+ curr-depth)))
                           new-locref)
                          new-locref))))
              locref-slist)))))
;; line 947 "idxentry.nw"
(defun remove-first-layers (locref-list)
  (declare (inline))
  (mapc #'(lambda (locref)
            (typecase locref
              (layered-location-reference
                 (set-layers  (cdr (get-layers  locref)) locref)
                 (set-ordnums (cdr (get-ordnums locref)) locref))
              (location-range
                 (let ((first (get-first locref))
                       (last (get-last locref)))
                   (set-layers  (cdr (get-layers  locref)) locref)
                   (set-ordnums (cdr (get-ordnums locref)) locref)
                   (set-layers  (cdr (get-layers  first)) first)
                   (set-ordnums (cdr (get-ordnums first)) first)
                   (set-layers  (cdr (get-layers  last))  last)
                   (set-ordnums (cdr (get-ordnums last))  last)
                   ))))
        locref-list)
  locref-list)
;; line 973 "idxentry.nw"
(defun sort-locrefs (locref-list)
  #+:ASSERT! (assert! (listp locref-list))
  (sort locref-list
        #'(lambda (locref-1 locref-2)
            (locref-ordnum< (get-ordnums locref-1)
                            (get-ordnums locref-2)))))
;; line 370 "idxentry.nw"
(defmethod process-group ((locref-cls-grp locref-class-group)
                          (idxcls indexclass))
  #+:XP
  (when *locref-trace* (pprint-locref-group *logging-stream* locref-cls-grp
                                            "Inital location-refrence group:"))
  (apply-substitution-and-merge-rules locref-cls-grp idxcls)
  #+:XP
  (when *locref-trace* (pprint-locref-group *logging-stream* locref-cls-grp
                                            "After subst- and merge-to-rules:"))
  (build-ranges-in-locref-class-group locref-cls-grp idxcls)
  #+:XP
  (when *locref-trace* (pprint-locref-group *logging-stream* locref-cls-grp
                                            "After build-ranges:"))
  (remove-virtual-locrefs locref-cls-grp)
  #+:XP
  (when *locref-trace* (pprint-locref-group *logging-stream* locref-cls-grp
                                            "After remove virtuals:"))
  (separate-into-catattr-groups locref-cls-grp)
  #+:XP
  (when *locref-trace* (pprint-locref-group *logging-stream* locref-cls-grp
                                            "After separate attr-groups:"))
  locref-cls-grp)

;; line 531 "idxentry.nw"
(defmethod apply-substitution-and-merge-rules ((locref-cls-grp locref-class-group)
                                               (idxcls         indexclass))
  (let ((locref-same-ordnum-grps
         (split-same-locrefs (get-members locref-cls-grp))))
    (set-members
     (mapcan #'(lambda (locref-same-ordnum-grp)
                 (apply-merge-rules
                  idxcls
                  (apply-substitution-rules idxcls
                                            locref-same-ordnum-grp)))
             locref-same-ordnum-grps)
     locref-cls-grp))
  locref-cls-grp)

;; line 553 "idxentry.nw"
(defun split-same-locrefs (locrefs)
  #+ASSERT! (assert! (listp locrefs))
  (split-list #'(lambda (locref)
                  (get-ordnums locref))
              locrefs
              :sortfunc #'(lambda (locref-ordnum-1 locref-ordnum-2)
                            (locref-ordnum< locref-ordnum-1 locref-ordnum-2))
              ))
;; line 569 "idxentry.nw"
(defmethod apply-substitution-rules ((idxcls indexclass)
                                     (locrefs list))
  (let ((substitute-locref (get-substitution-subst-hook idxcls)))
    (delete-if #'null
               (mapcar #'(lambda (test-locref)
                           (cond ((some #'(lambda (against-locref)
                                            (substitutable-p idxcls test-locref
                                                             against-locref))
                                        locrefs)
                                    (funcall substitute-locref test-locref))
                                 (t test-locref)))
                       locrefs))))
;; line 590 "idxentry.nw"
(defmethod substitutable-p ((idxcls         indexclass)
                            (test-locref    layered-location-reference)
                            (against-locref layered-location-reference))
  (cond ((eql test-locref against-locref)
           nil)
        ((and (state-normal-p against-locref)
              (relation-p (get-implicit-subst-rules idxcls)
                          (get-catattr against-locref)
                          (get-catattr test-locref))))
        (t nil)))
;; line 638 "idxentry.nw"
(defmethod apply-merge-rules ((idxcls indexclass) (locrefs list))
  (let ((merge-rules          (get-merge-rules idxcls))
        (drop-if-merged-rules (get-drop-if-merged-rules idxcls)))
    (nconc locrefs
           (mapcan #'(lambda (locref)
                       (mapcan #'(lambda (catattr)
                                   (let ((new-locref
                                          (make-layered-location-reference
                                           :virtual  t
                                           :layers   (get-layers locref)
                                           :ordnums  (get-ordnums locref)
                                           :catattr  catattr
                                           :locclass (get-locclass locref)
                                           :attribute (get-attribute locref)
                                           :string   (get-locref-string locref)
                                           )))
                                     (when (relation-p drop-if-merged-rules
                                                       (get-catattr locref)
                                                       catattr)
                                       (set-origin locref new-locref))
                                     (list new-locref)))
                               (relation-set merge-rules (get-catattr locref))))
                   locrefs))))
;; line 721 "idxentry.nw"
(defun build-ranges-in-locref-class-group (locref-cls-grp idxcls)
  (let ((locref-same-catattr-grps
         (split-list #'(lambda (locref) (get-catattr locref))
                     (get-members locref-cls-grp)
                     :sortfunc #'(lambda (x y)
                                   (< (get-processing-ordnum x)
                                      (get-processing-ordnum y))))))
    (set-members (mapcan #'(lambda (catattr-sublist)
                             (let ((processed-locrefs
                                    (build-ranges catattr-sublist
                                                  idxcls
                                                  (get-locclass
                                                   (car catattr-sublist)))))
                               (when *locref-trace*
                                 (let ((*print-pretty* t))
                                   (gol t "~&after build-ranges:")
                                   (pprint-newline :mandatory *logging-stream*)
                                   (pprint-newline :mandatory *logging-stream*)
                                   (write processed-locrefs
                                          :stream *logging-stream*)))
                               (let ((s2 (mapcar
                                          #'(lambda (x)
                                              (post-process
                                               x
                                               #'set-state-deleted))
                                          processed-locrefs)))
                                 (when *locref-trace*
                                   (let ((*print-pretty* t))
                                     (gol t "~&after set-state-deleted:")
                                     (pprint-newline :mandatory *logging-stream*)
                                     (pprint-newline :mandatory *logging-stream*)
                                     (write s2 :stream *logging-stream*)))
                                 s2)))
                         locref-same-catattr-grps)
                 locref-cls-grp)))
;; line 396 "idxentry.nw"
(defmethod process-group ((crossref-cls-grp crossref-class-group)
                          (idxcls indexclass))
  (let ((cross-references (get-members crossref-cls-grp)))
    (typecase (get-locclass (car cross-references))
      (verified-crossref-location-class
         (mapc #'(lambda (xref)
                   (let* ((target (get-target xref))
                          (merge-key-target
                           #+:ORDRULES (gen-keyword-mergekey-list target)
                           #-:ORDRULES target))
                     (unless (lookup-indexentry merge-key-target)
                       (oops "Cross-reference-target ~S does not exist!"
                             target))))
               cross-references))
      (unverified-crossref-location-class)
      (t (error "INTERNAL ERROR in process-group"))))
  crossref-cls-grp)
;; line 422 "idxentry.nw"
(defun remove-virtual-locrefs (locref-cls-grp)
  (set-members (delete-if #'(lambda (object)
                              (and (typep object 'location-reference)
                                   (state-virtual-p object)))
                          (get-members locref-cls-grp))
               locref-cls-grp))
;; line 794 "idxentry.nw"
(defclass category-attribute-group (locref-group)
    ((ordnum :initform 'nil)))

(defun separate-into-catattr-groups (locref-cls-grp)
  (set-members (convert-sublists-to-group-objects
                (split-list #'(lambda (object)
                                (get-catattr-grp-ordnum (get-catattr object)))
                            (get-members locref-cls-grp)
                            :sortfunc #'<)
                'category-attribute-group)
               locref-cls-grp)
  (mapc #'(lambda (catattr-grp)
            (set-ordnum (get-catattr-grp-ordnum
                         (get-catattr (car (get-members catattr-grp))))
                        catattr-grp)
            (set-members
             (sort (tree-location-references (get-members catattr-grp)
                                             (get-hierdepth
                                              (get-locclass locref-cls-grp)))
                   #'(lambda (object-1 object-2)
                       (let ((ordnum-1 (get-ordnums object-1))
                             (ordnum-2 (get-ordnums object-2)))
                         (or (locref-ordnum< ordnum-1 ordnum-2)
                             (and (locref-ordnum= ordnum-1 ordnum-2)
                                  (< (get-sort-ordnum (get-catattr object-1))
                                     (get-sort-ordnum (get-catattr object-2))))))))
             catattr-grp))
        (get-members locref-cls-grp))
  locref-cls-grp)

#|
(defun separate-into-catattr-groups-old-version (locref-cls-grp)
  (set-members (convert-sublists-to-group-objects
                (split-list #'(lambda (object)
                                (get-catattr-grp-ordnum (get-catattr object)))
                            #'<
                            (get-members locref-cls-grp))
                'category-attribute-group)
               locref-cls-grp)
  (mapc #'(lambda (catattr-grp)
            (set-ordnum (get-catattr-grp-ordnum
                         (get-catattr (car (get-members catattr-grp))))
                        catattr-grp)
            (set-members
             (sort (get-members catattr-grp)
                   #'(lambda (object-1 object-2)
                       (let ((ordnum-1 (get-ordnums object-1))
                             (ordnum-2 (get-ordnums object-2)))
                         (or (locref-ordnum< ordnum-1 ordnum-2)
                             (and (locref-ordnum= ordnum-1 ordnum-2)
                                  (< (get-sort-ordnum (get-catattr object-1))
                                     (get-sort-ordnum (get-catattr object-2))))))))
             catattr-grp))
        (get-members locref-cls-grp))
  locref-cls-grp)
|#

;; line 992 "idxentry.nw"
(eval-when (compile load eval)

;; line 127 "idxentry.nw"
(export '(index-entry make-index-entry
          add-location-reference-to-indexentry))
;; line 360 "idxentry.nw"
(export '(locref-group get-members
          locref-class-group crossref-class-group get-locclass
          make-locref-group process-group process-indexentry))
;; line 603 "idxentry.nw"
(export '(process-group split-same-locrefs
          apply-substitution-rules substitutable-p))
;; line 664 "idxentry.nw"
(export '(apply-merge-rules))
;; line 853 "idxentry.nw"
(export '(category-attribute-group))
;; line 1021 "idxentry.nw"
(export '(*percentage-list* print-rest-of-percentages print-percent))
;; line 994 "idxentry.nw"
  )
;; line 594 "ranges.nw"
;; $Id: ranges.nw,v 1.11 1997/03/26 16:18:41 kehr Exp $

;; line 68 "ranges.nw"
(defclass location-range ()
    ((length   :initarg :length)
     (first    :initarg :first)
     (last     :initarg :last)
     (layers   :initarg :layers)
     (ordnums  :initarg :ordnums)
     (inner    :initarg :inner)
     (locclass :initarg :locclass)
     (catattr  :initarg :catattr)
     (subrefs  :initform '())))

(defun make-location-range (&key length first last first-ordnum inner locclass)
  (make-instance 'location-range
                 :length   length
                 :first    first
                 :last     last
                 :inner    inner
                 :ordnums  first-ordnum
                 :layers   (get-layers first)
                 :locclass locclass
                 :catattr  (get-catattr first)))
;; line 581 "ranges.nw"
(defun ordnum-successor-p (ordnums-1 ordnums-2)
  (declare (inline))
  (and ordnums-1 #|is there really something in it, or we are about to start?|#
       (equal (butlast ordnums-1) (butlast ordnums-2))
       (equal (1+ (car (last ordnums-1)))
              (car (last ordnums-2)))))
;; line 143 "ranges.nw"
(defmethod build-ranges ((locref-list list)
                         (idxcls indexclass)
                         (loccls var-location-class))
  locref-list)
;; line 321 "ranges.nw"
(defmacro newstat (s)
  `(setq new-state ,s))

(defmacro pushl ()
  `(progn
    (setq curr-ordnum (get-ordnums locref))
    (unless locref-stack
      (setq first-ordnum curr-ordnum)
      (setq first-locref locref))
    (setq prev-ordnum curr-ordnum)
    (push locrefs locref-stack)))

(defmacro pushone ()
  `(progn
    (setq curr-ordnum (get-ordnums locref))
    (unless locref-stack
      (setq first-ordnum curr-ordnum)
      (setq first-locref locref))
    (setq prev-ordnum curr-ordnum)
    (push (list (car locrefs)) locref-stack)))

(defmacro set-open-range ()
  `(setq last-open-locref locref))

(defmacro keep ()
  `(setq back-flag t))

(defmacro make-range ()
  `(setq make-flag t))
;; line 197 "ranges.nw"
(defmethod build-ranges ((locref-list list)
                         (idxcls indexclass)
                         (loccls standard-location-class))
  (let ((valid-list (mapcan #'(lambda (locref)
                                (unless (state-deleted-p locref)
                                  (list locref)))
                            locref-list)))
    (ifnot valid-list
           '()
           (ifnot (joining-allowed-p loccls)
                  valid-list
                  (let ((same-ordnum-list (split-same-locrefs valid-list))

;; line 353 "ranges.nw"
(locref-stack '())
(result-list  '())
(state        :START)
(join-length  (get-join-length loccls))
new-state
open-p close-p open+close-p no-typ-p list-end
make-flag back-flag
locrefs locref last-open-locref first-locref
first-ordnum prev-ordnum curr-ordnum
;; line 209 "ranges.nw"
                                                   )
                    (loop

;; line 365 "ranges.nw"
(setq locrefs  (car same-ordnum-list))
(setq list-end (null locrefs))
(setq locref   (car locrefs))
(setq open-p   (find-if #'(lambda (locref)
                            (rangeattr-open-p (get-rangeattr locref)))
                        locrefs))
(setq close-p  (find-if #'(lambda (locref)
                            (rangeattr-close-p (get-rangeattr locref)))
                        locrefs))
(setq open+close-p  (and open-p close-p))
(setq no-typ-p (not (or  open-p close-p)))
;; line 212 "ranges.nw"
                      (ecase state
                        (:START
;; line 230 "ranges.nw"
(cond (list-end     (return (apply #'nconc  #| see note below for explanation |#
                                   (apply #'nconc result-list))))
      (no-typ-p     (newstat :READ)       (pushl))
      (open+close-p (newstat :READ)       (pushone))
      (open-p       (newstat :OPEN-RANGE) (pushl) (set-open-range))
      (close-p      (warn-invalid-close locref) (newstat :START))
      (t (error
          "INTERNAL-ERROR in build-ranges - :START")))
;; line 213 "ranges.nw"
                                                              )
                        (:READ
;; line 267 "ranges.nw"
(if list-end
    (progn (newstat :START) (make-range))
    (progn
      (setq curr-ordnum (get-ordnums locref))
      (let ((succ-p (ordnum-successor-p prev-ordnum curr-ordnum)))
        (cond ((and succ-p no-typ-p)     (pushl))
              ((and succ-p open+close-p) (pushone))
              ((or no-typ-p
                   (not (and succ-p
                             open-p))) (newstat :START) (keep) (make-range))
              (open-p                  (newstat :OPEN-RANGE)
                                       (pushl) (set-open-range))
              (close-p  (warn-invalid-close locref) (newstat :READ))
              (t (error "INTERNAL-ERROR in build-ranges - :READ"))))))
;; line 214 "ranges.nw"
                                                             )
                        (:OPEN-RANGE
;; line 288 "ranges.nw"
(cond (list-end
;; line 297 "ranges.nw"
(warn
   (with-output-to-string (s)
     (format s "Found no :close-range matching an already opened one!~%")
     (format s "Location-reference is ~A in keyword ~A.~%"
             (get-locref-string last-open-locref)
             (get-main-key *currently-processed-indexentry*))
     (format s "Maybe I lost some of the regular location-references.~%")))
;; line 288 "ranges.nw"
                                                                                             (newstat :START))
      ((or no-typ-p open+close-p) (pushl))
      (open-p
;; line 307 "ranges.nw"
(warn
 (with-output-to-string (s)
   (format s "Found :open-range that was already opened!~%")
   (format s "Location-references are ~A and ~A in keyword ~A.~%"
           (get-locref-string last-open-locref)
           (get-locref-string locref)
           (get-main-key *currently-processed-indexentry*))
   (format s "I'll continue and ignore this.~%")))
;; line 291 "ranges.nw"
                 (newstat :OPEN-RANGE))
      (close-p    (newstat :READ) (pushl))
      (t (error "INTERNAL-ERROR in build-ranges - :OPEN-RANGE")))
;; line 215 "ranges.nw"
                                                                   ))

;; line 402 "ranges.nw"
(when make-flag
  (let ((range-len (- (car (last prev-ordnum)) (car (last first-ordnum)))))
    (if (>= range-len join-length)
        (push (list (list
                     (make-location-range :first-ordnum first-ordnum
                                          :locclass loccls
                                          :inner  locref-stack
                                          :first  first-locref
                                          :last   (caar locref-stack)
                                          :length range-len)))
              result-list)
        (push locref-stack result-list))
    (setq locref-stack '())
    (setq make-flag nil)))
(if back-flag
    (setq back-flag nil)
    (setq same-ordnum-list (cdr same-ordnum-list)))
(setq state new-state)
;; line 216 "ranges.nw"
                                                ))))))
;; line 241 "ranges.nw"
(defun warn-invalid-close (locref)
  (warn
   (with-output-to-string (s)
     (format s "Found a :close-range in the index that wasn't opened before!~%")
     (format s "Location-reference is ~A in keyword ~A~%"
             (get-locref-string locref)
             (get-main-key *currently-processed-indexentry*))
     (format s "I'll continue and ignore this.~%"))))
;; line 509 "ranges.nw"
(defmethod post-process ((locrange location-range) (drop-function function))
  (let ((locref-list (apply #'nconc (get-inner locrange))))
    (cond ((>= (get-length locrange)
               (get-join-length (get-locclass locrange)))
             (mapc #'(lambda (locref)
                       (let ((origin (get-origin locref)))
                         (when origin
                           (funcall drop-function origin))))
                   locref-list)
             ;;(set-inner '() locrange)
             locrange)
          (t locref-list))))

;(defmethod post-process ((locref-list list) x)
;  (declare (ignore x))
;  locref-list)

(defmethod post-process ((locref location-reference) x)
  (declare (ignore x))
  locref)

;; line 600 "ranges.nw"
(eval-when (compile load eval)

;; line 92 "ranges.nw"
(export '(location-range get-first get-last get-length))
;; line 602 "ranges.nw"
  )
;; line 88 "index.nw"
(defparameter *index-entry-table*       nil) #| faster access to the entry-table |#
;; line 69 "index.nw"
(defconstant *all-hierarchy-layers* MOST-POSITIVE-FIXNUM)

(defclass base-index ()
    ((entry-table :initform (make-hash-table :test #'equal))
     (entry-list  :initform '())
     (entries     :initform '())
     (idxclass    :initarg  :idxclass)
     (hierdepth   :initform *all-hierarchy-layers*)))

(defun make-base-index (idxclass)
  (make-instance 'base-index :idxclass idxclass))
;; line 542 "index.nw"
(defclass letter-group ()
    ((members          :initarg :members :type list)
     (group-definition :initarg :group-definition)))
;; line 555 "index.nw"
(defun make-letter-group (members)
  (make-instance 'letter-group
                 :members members
                 :group-definition (match-letter-group
                                    *indexstyle*
                                    (car (get-sort-key (car members))))))

;; line 273 "index.nw"
(defun add-layered-locref-indexentry (&key
                                      (entry-table *index-entry-table*)
                                      (idxcls *indexstyle*)
                                      locref main-key
                                      (merge-key '())
                                      (print-key '())
                                      (open-range nil) (close-range nil)
                                      (catattr    nil) (attribute nil))
  #+:ASSERT! (assert! (and main-key locref))

  (if (and open-range close-range)

;; line 320 "index.nw"
(nraw "specification of both :open-range and :close-range is invalid! (ignored)")
;; line 285 "index.nw"
      (progn
        (when (or (not catattr)
                  (string= catattr ""))
          (setq catattr "default"))
        (let ((catattr-obj (lookup-catattr idxcls catattr)))
          (if (not catattr-obj)
              (nraw "unknown attribute `~A'! (ignored)" catattr)
              (multiple-value-bind (num location-reference)
                  (create-location-reference-from-string
                   locref catattr-obj attribute)
                (when (eql 1 num)
                  #|                    ;
                  Only one vaild location-reference! Use #'eql and
                  not #'= since we compare num against NIL, too.
                  |#

;; line 359 "index.nw"
#+:ORDRULES (unless merge-key
              (setq merge-key (gen-keyword-mergekey-list main-key)))
#-:ORDRULES (unless merge-key (setq merge-key main-key))
;; line 301 "index.nw"
                  (cond (open-range  (set-rangeattr-open location-reference))
                        (close-range (set-rangeattr-close location-reference)))
                  (let ((old-entry (gethash merge-key entry-table)))
                    (if old-entry
                        (setf (gethash merge-key entry-table)
                              (join-indexentries
                               old-entry;;(gethash merge-key entry-table)
                               main-key print-key
                               location-reference))
                        (setf (gethash merge-key entry-table)
                              (make-index-entry :main-key  main-key
                                                :merge-key merge-key
                                                :print-key print-key
                                                :locrefs   (list
                                                            location-reference)
                                                :indexclass idxcls)))))))))))
;; line 324 "index.nw"
(defun add-crossref-indexentry (&key
                                (entry-table *index-entry-table*)
                                (idxcls *indexstyle*)
                                (merge-key '())
                                (print-key '())
                                main-key
                                xref
                                xref-class-name
                                (attribute  nil))
  #+:ASSERT! (assert! (and main-key xref xref-class-name))

  (unless xref-class-name (setq xref-class-name "default"))
  (let ((xref-class (cdr (lookup-crossref-class idxcls xref-class-name))))
    (if (not xref-class)
        (nraw "unknown cross-reference-class `~A'! (ignored)~%"
              xref-class-name)
        (let ((cross-reference (create-cross-reference xref-class
                                                       xref attribute)))
          (when cross-reference

;; line 359 "index.nw"
#+:ORDRULES (unless merge-key
              (setq merge-key (gen-keyword-mergekey-list main-key)))
#-:ORDRULES (unless merge-key (setq merge-key main-key))
;; line 344 "index.nw"
            (let ((old-entry (gethash merge-key entry-table)))
              (if old-entry
                  (setf (gethash merge-key entry-table)
                        (join-indexentries old-entry ;;(gethash merge-key entry-table)
                                           main-key print-key
                                           cross-reference))
                  (setf (gethash merge-key entry-table)
                        (make-index-entry :main-key  main-key
                                          :merge-key merge-key
                                          :print-key print-key
                                          :locrefs   (list cross-reference)
                                          :indexclass idxcls)))))))))
;; line 367 "index.nw"
#+:ORDRULES
(defun gen-keyword-mergekey-list (list-of-strings)
  #+ASSERT! (assert! (and (not (null list-of-strings))
                          (listp list-of-strings)))
  (let ((res (mapcar #'(lambda (str)
                         (gen-keyword-mergekey str))
                     list-of-strings)))
    (flush-ordrules-message-buffer)
    res))

#+:ORDRULES
(defun gen-keyword-sortkey-list (list-of-strings run)
  #+ASSERT! (assert! (and (not (null list-of-strings))
                          (listp list-of-strings)
                          (<= 0 run) (run < 8)))
  (let ((res (mapcar #'(lambda (str)
                         (gen-keyword-sortkey str run))
                     list-of-strings)))
    (flush-ordrules-message-buffer)
    res))
;; line 398 "index.nw"
(defun join-indexentries (idxent main-key print-key locref)
  (let ((old-main-key  (get-main-key  idxent))
        (old-print-key (get-print-key idxent)))
    (unless old-main-key  (set-main-key  main-key  idxent))
    (set-print-key (mapcar #'(lambda (old curr)
                               (or old curr))
                           old-print-key print-key)
                   idxent)
    ;; (info "~&(join-indexentries :old-main-key ~S :old-print-key ~S :new-print-key ~S)"
    ;; old-main-key old-print-key (get-print-key idxent))
    (add-location-reference-to-indexentry idxent locref)))
;; line 438 "index.nw"
(defun create-location-reference-from-string (locref-str catattr attribute)
  (let* ((idxcls *indexstyle*))
    (multiple-value-bind (num locref)
        (lookup-locref-cache idxcls locref-str)
      (if num #| something was found! |#
          (case num
            (0
;; line 474 "index.nw"
(nraw "location-reference ~S did not match any location-class! (ignored)"
      locref-str)
;; line 445 "index.nw"
               (values nil nil))
            (1 (values 1 (copy-locref-from-template locref catattr attribute))
               #| return its only member |#)
            (t
;; line 469 "index.nw"
(nraw "location-reference ~S did match multiple location-classes! (taking first one)"
      locref-str)
;; line 449 "index.nw"
               (values num (copy-locref-from-template locref catattr attribute))
               #| return a copy of the stored template |#))
          (let ((locref-list
                 (map-location-classes
                  idxcls
                  #'(lambda (idxcls loccls)
                      (declare (ignore idxcls))
                      (build-location-reference loccls  locref-str
                                                catattr attribute)))))
            (case (length locref-list)
              (0
;; line 474 "index.nw"
(nraw "location-reference ~S did not match any location-class! (ignored)"
      locref-str)
;; line 460 "index.nw"
                 (add-locref-cache idxcls locref-str 0 nil))
              (1 (add-locref-cache idxcls locref-str 1 (car locref-list))
                 #| return its only member |#)
              (t
;; line 469 "index.nw"
(nraw "location-reference ~S did match multiple location-classes! (taking first one)"
      locref-str)
;; line 464 "index.nw"
                 (add-locref-cache idxcls locref-str 1 (car locref-list))
                 #| return its first member |#)))))))
;; line 482 "index.nw"
(defun copy-locref-from-template (locref catattr attribute)
  (declare (inline))
  (make-layered-location-reference :catattr   catattr
                                   :attribute attribute
                                   :string    (get-locref-string locref)
                                   :locclass  (get-locclass      locref)
                                   :layers    (copy-list (get-layers locref))
                                   :ordnums   (copy-list (get-ordnums locref))))
;; line 727 "index.nw"
(defun sort-indexentries (indexentries)
  #+:ASSERT! (assert! (listp indexentries))
  (gol t "~&sort-indexentries: ~S" *sort-rule-orientations*)
  (sort indexentries
        #'(lambda (idxent-1 idxent-2)
            (multiple-value-bind (less-equal? equal?)
                (strlist<= (get-sort-key idxent-1)
                           (get-sort-key idxent-2)
                           (first *sort-rule-orientations*))
              (if equal?
                  (sort-other-runs (get-merge-key idxent-1)
                                   (get-merge-key idxent-2)
                                   (rest *sort-rule-orientations*)
                                   1)
                  less-equal?)))))
;; line 751 "index.nw"
(defun sort-other-runs (merge-key-1 merge-key-2 orientations run)
  (gol t "~&sort-other-runs: ~S ~S ~S ~S."
       merge-key-1 merge-key-2 orientations run)
  (cond ((endp orientations)
           (nraw "Sort rules could not define exact order of key ~S."
                 merge-key-1)
           nil)
        (t (let ((key-1 (mapcar #'(lambda (key)
                                    (gen-keyword-sortkey key run))
                                merge-key-1))
                 (key-2 (mapcar #'(lambda (key)
                                    (gen-keyword-sortkey key run))
                                merge-key-2)))
             (flush-ordrules-message-buffer)
             (multiple-value-bind (less-equal? equal?)
                 (strlist<= key-1 key-2 (first orientations))
               (if equal?
                   (sort-other-runs merge-key-1 merge-key-2
                                    (rest orientations) (1+ run))
                   less-equal?))))))
;; line 782 "index.nw"
(defun oriented-string (string orientation)
  (declare (inline))
  (cond ((eql orientation 'forward)  string)
        ((eql orientation 'backward) (reverse string))
        (t (error "INDEX:ORIENTED-STRING wrong orientation!"))))
;; line 793 "index.nw"
(defun strlist<= (slist-1 slist-2 orientation)
  (declare (inline))
  (cond ((endp slist-1) (values t   (endp slist-2)))
        ((endp slist-2) (values nil nil))
        (t (let ((str-1 (oriented-string (first slist-1) orientation))
                 (str-2 (oriented-string (first slist-2) orientation)))
             (let ((pos (string/= str-1 str-2)))
               (if pos
                   (let ((lt (string< str-1 str-2)))
                     (if lt
                         (values t   nil)
                         (values nil (string= str-1 str-2))))
                   (strlist<= (cdr slist-1) (cdr slist-2) orientation)))))))
#|
(strlist<= '("foo") '("foo") 'forward)
(strlist<= '("fool") '("foo") 'forward)
(strlist<= '("foo") '("fool") 'forward)
(strlist<= '("foo") '("fool") 'backward)
(strlist<= '("foo") '("foo" "bar") 'forward)
(strlist<= '("foo" "bar") '("foo") 'forward)
(strlist<= '("foo" "bar") '("foo") 'backward)
|#


#+OLDVER
(defun strlist< (string-list-1 string-list-2)
  (declare (inline))
  (do ((rest-1 string-list-1 (cdr rest-1))
       (rest-2 string-list-2 (cdr rest-2)))
      ((or (endp rest-1) (endp rest-2)
           (string/= (first rest-1) (first rest-2)))
         (or (endp rest-1)
             (if (endp rest-2)
                 'nil
                 (string< (first rest-1) (first rest-2)))))))
;; line 504 "index.nw"
(defmethod process-index ((index base-index))
  (let ((hash-list '()))
    (maphash #'(lambda (key idxentry)
                 (declare (ignore key))
                 (setq hash-list (cons idxentry hash-list)))
             (get-entry-table index))
    (set-entry-list hash-list index)
    (build-indexentry-tree index)

    (setq *number-of-indexentries* (length (get-entry-list index)))
    (setq *current-number* 0)
    (setq *percentage-list*
          (mapcar #'(lambda (percent)
                      (cons (truncate (* (/ percent 100)
                                         *number-of-indexentries*))
                            percent))
                  ;; MOST-POSITIVE-FIXNUM acts as sentinel such that
                  ;; we never run out of elements
                  `(10 20 30 40 50 60 70 80 90 100 ,MOST-POSITIVE-FIXNUM)))
    (setq *processing-percentage-list* *percentage-list*)
    (mapc #'process-indexentry (get-entry-list index))
    (print-rest-of-percentages *processing-percentage-list*)
    index))
;; line 568 "index.nw"
(defmethod build-indexentry-tree ((index base-index))
  (let ((index-hierdepth (get-hierdepth index)))
    (gol t "~&~%Building indexentry-tree:~%")
    (gol t "Hierdepth of index is ~S~%~%"
         (if (= index-hierdepth most-positive-fixnum)
             :tree index-hierdepth))
    (set-entries (build-indexentry-tree-recursive (get-entry-list index)
                                                  index-hierdepth)
                 index)
    (gol t "~&~%Forming letter-groups:~%")
    (let ((letter-groups (split-list #'(lambda (idxent)
                                         (get-ordnum
                                          (match-letter-group
                                           *indexstyle*
                                           (car (get-sort-key idxent)))))
                                     (get-entries index)
                                     :sortfunc #'<)))
      (set-entries (mapcar #'(lambda (lg-indexentries)
                               (make-letter-group
                                (sort-indexentries lg-indexentries)))
                           letter-groups)
                   index))))
;; line 622 "index.nw"
(defun build-indexentry-tree-recursive (idxent-list
                                        max-depth
                                        &optional (curr-depth 1))
  (cond ((endp idxent-list) idxent-list)   #| nothing more to do |#
        ((> curr-depth max-depth)          #| we reached the end |#
           (gen-all-sort-keys idxent-list) #| this is the then case |#
           )
        (t (let ((idxent-slist
                  (split-list #'(lambda (idxent)
                                  (car (get-merge-key idxent)))
                              idxent-list
                              :headfunc
                              #'(lambda (idxent)
                                  (= 1 (length (get-merge-key idxent)))))))
             (mapcar
              #'(lambda (sublist)
                  (let* ((idxent (car sublist))
                         (merge-key (get-merge-key idxent)))
                    (if (= 1 (length merge-key))
                        (progn #| length = 1 |#
                          (set-sort-key #-:ORDRULES merge-key
                                        #+:ORDRULES (gen-keyword-sortkey-list
                                                     merge-key 0)
                                        idxent)
                          (set-subentries
                           (sort-indexentries
                            (build-indexentry-tree-recursive
                             (remove-first-keywords (cdr sublist))
                             max-depth
                             (1+ curr-depth)))
                           idxent)
                          idxent)
                        (let* #| length > 1 |#
                            ((main-key (get-main-key idxent))
                             (new-merge-key (take-first merge-key))
                             (some-print-key (find-print-key sublist))
                             (new-print-key (list (or some-print-key
                                                      (car main-key))))
                             (new-idxent
                              (make-index-entry
                               :main-key  (take-first main-key)
                               :merge-key new-merge-key
                               :print-key new-print-key
                               :sort-key
                               #-:ORDRULES new-merge-key
                               #+:ORDRULES (gen-keyword-sortkey-list
                                            new-merge-key 0)
                               :idxclass  (get-idxclass idxent))))
                          ;; (info "~&(treeing :main ~S :some-print ~S :new-print ~S)"
                          ;; main-key some-print-key new-print-key)
                          (set-subentries
                           (sort-indexentries
                            (build-indexentry-tree-recursive
                             (remove-first-keywords sublist)
                             max-depth
                             (1+ curr-depth)))
                           new-idxent)
                          new-idxent))))
              idxent-slist)))))

(defun find-print-key (idxent-list)
  (some #'(lambda (idxent)
            (car (get-print-key idxent)))
        idxent-list))
;; line 689 "index.nw"
(defun remove-first-keywords (idxent-list)
  (declare (inline))
  (mapc #'(lambda (idxent)
            (setf (slot-value idxent 'merge-key)
                  (cdr (slot-value idxent 'merge-key)))
            (setf (slot-value idxent 'main-key)
                  (cdr (slot-value idxent 'main-key)))
            (setf (slot-value idxent 'print-key)
                  (cdr (slot-value idxent 'print-key)))
            )
        idxent-list)
  idxent-list)
;; line 713 "index.nw"
(defun gen-all-sort-keys (idxent-list)
  (when idxent-list
    (mapc #'(lambda (idxent)
              (set-sort-key #-:ORDRULES (get-merge-key idxent)
                            #+:ORDRULES (gen-keyword-sortkey-list
                                         (get-merge-key idxent) 0)
                            idxent))
          idxent-list))
  idxent-list)
;; line 94 "index.nw"
(defparameter *index* nil)

(defun create-index ()
  (setq *index* (make-base-index *indexstyle*))
  (setq *index-entry-table* (get-entry-table *index*))
  *index*)

(create-index)
;; line 421 "index.nw"
(defun lookup-indexentry (key &optional (index-entry-table *index-entry-table*))
  (gethash key index-entry-table))

;; line 258 "index.nw"
(eval-when (compile load eval)
  (defparameter *raw-index-interface-definitions*
    '(indexentry)))
;; line 165 "index.nw"
(defmacro indexentry (&whole whole &rest args)
  (destructuring-switch-bind (&key
                              key print tkey merge attr locref xref
                              &switch
                              (open-range  :open-range)
                              (close-range :close-range))
      args
    (cond ((not (or key tkey))
             (error "missing argument :key or :tkey in~%~S."))
          ((and key tkey)
             (error "you can't specify :key and :tkey at the same time in~%~S."
                    whole))
          ((and locref xref)
;; line 213 "index.nw"
(error "you can't specify :locref and :xref at the same time in~%~S."
       whole)

;; line 177 "index.nw"
                                                           )
          (t (progn
               (if tkey
                   (multiple-value-setq (key print)
                     (tkey-to-normal-key tkey whole))
                   (when print;; check for correct key-length
                     (let ((keylen (length key)))
                       (unless (= keylen (length print))
                         (setq print;; take the first keylen elmts of print
                               (mapcar #'(lambda (x y) (declare (ignore y)) x)
                                       key
                                       (append print (make-list keylen))))))))
               (cond ((not (listp key))
                        (error ":key ~S is not a list in~%~S." key whole))
                     ((not (listp print))
                        (error ":print ~S is not a list in~%~S." print whole))
                     (locref `(ADD-LAYERED-LOCREF-INDEXENTRY
                               :MAIN-KEY    ',key
                               :PRINT-KEY   ',print
                               :MERGE-KEY   ',merge
                               :CATATTR     ',attr
                               :LOCREF      ',locref
                               :OPEN-RANGE  ',open-range
                               :CLOSE-RANGE ',close-range))
                     (xref (when (or open-range close-range)

;; line 221 "index.nw"
(error "you can't specify :open-range or :close-range with :xref at the same time in~%~S."
       whole)
;; line 202 "index.nw"
                                                  )
                           (unless attr
;; line 217 "index.nw"
(error "missing :attr in combination with :xref in~%~S." whole)
;; line 203 "index.nw"
                                                                   )
                           `(ADD-CROSSREF-INDEXENTRY
                             :MAIN-KEY  ',key
                             :PRINT-KEY ',print
                             :XREF      ',xref
                             :XREF-CLASS-NAME ',attr))
                     (t
;; line 226 "index.nw"
(error "you must at least specify :locref or :xref in~%~S." whole)
;; line 209 "index.nw"
                                               )))))))
;; line 238 "index.nw"
(defun tkey-to-normal-key (tkey whole)
  (when (not (listp tkey))
;; line 254 "index.nw"
(error "INDEXENTRY: invalid structure of :tkey in~%~S." tkey whole)
;; line 239 "index.nw"
                                              )
  (let ((main-key  '())
        (print-key '()))
    (mapc #'(lambda (key-elt)
              (cond ((listp key-elt)
                       (push (car key-elt) main-key)
                       (if (cdr key-elt)
                           (push (cadr key-elt) print-key)
                           (push nil print-key)))
                    (t
;; line 254 "index.nw"
(error "INDEXENTRY: invalid structure of :tkey in~%~S." tkey whole)
;; line 248 "index.nw"
                                           )))
          tkey)
    (values (nreverse main-key) (nreverse print-key))))

;; line 889 "index.nw"
(eval-when (compile load eval)

;; line 83 "index.nw"
(export '(base-index make-base-index get-entries))
;; line 105 "index.nw"
(export '(*index* create-index))
;; line 264 "index.nw"
(export '*raw-index-interface-definitions*)
(export *raw-index-interface-definitions*)
;; line 412 "index.nw"
(export '(add-locref-indexentry add-crossref-indexentry))
;; line 530 "index.nw"
(export '(process-index))
;; line 564 "index.nw"
(export '(letter-group get-members get-group-definition))
;; line 891 "index.nw"
  )

;; line 1029 "idxentry.nw"
#+:XP
(set-pprint-dispatch 'index-entry
                     #'(lambda (s idxent)
                         (pprint-logical-block
                          (s nil :suffix "}")
                          (pprint-indent :block 2 s)
                          (write-string "Indexentry {" s)
                          (pprint-newline :mandatory s)
                          (write-string "main-key: " s)
                          (write (get-main-key idxent) :stream s)
                          (pprint-newline :mandatory s)
                          (write-string "merge-key: " s)
                          (write (get-merge-key idxent) :stream s)
                          (pprint-newline :mandatory s)
                          (write-string "sort-key: " s)
                          (write (get-sort-key idxent) :stream s)
                          (pprint-newline :mandatory s)
                          (pprint-logical-block
                           (s nil :suffix "}")
                           (write-string "Locrefs {" s)
                           (pprint-indent :block 2)
                           (pprint-newline :mandatory s)
                           (pprint-linear s (get-locrefs idxent)))
                          (pprint-newline :mandatory s)
                          (pprint-logical-block
                           (s nil :suffix "}")
                           (write-string "Subentries {" s)
                           (pprint-indent :block 2)
                           (pprint-newline :mandatory s)
                           (pprint-linear s (get-subentries idxent)))
                          (pprint-newline :mandatory s))
                         ))
;; line 1071 "idxentry.nw"
#+:XP
(defun pprint-locref-group (s grp name)
  (format s "~%")
  (pprint-logical-block
   (s nil :prefix "(" :suffix ")")
   (write-string name s)
   (write-string ":" s)
   (when (typep grp 'category-attribute-group)
     (write (get-ordnum grp) :stream s))
   (pprint-indent :block 2 s)
   (pprint-newline :mandatory s)
   (pprint-fill s (get-members grp) nil)
   (pprint-indent :block -1 s)
   (pprint-newline :mandatory s)))

#+:XP
(defun set-pprint-locref-group-dispatch (class name &optional (rank 0))
  (set-pprint-dispatch class
                       #'(lambda (s grp)
                           (pprint-locref-group s grp name))
                       rank))
;; line 1107 "idxentry.nw"
#+:XP
(set-pprint-locref-group-dispatch 'locref-group
                                  "LOCREF-GROUP"               0)
#+:XP
(set-pprint-locref-group-dispatch 'locref-class-group
                                  "LOCREF-CLASS-GROUP"         1)
#+:XP
(set-pprint-locref-group-dispatch 'crossref-class-group
                                  "CROSSREF-CLASS-GROUP"       2)
#+:XP
(set-pprint-locref-group-dispatch 'category-attribute-group
                                  "CATEGORY-ATTRIBUTE-GROUP"   3)
;; line 614 "ranges.nw"
#+:XP
(defun pprint-location-range (s range)
  (pprint-logical-block
   (s nil :prefix "[" :suffix "]")
   (format s "range: ~D " (get-length range))
   (pprint-newline :fill s)
   (write-string " loccls: " s)
   (write (get-locclass range) :stream s)
   (pprint-newline :fill s)
   (write-string " catattr: " s)
   (write (get-catattr range) :stream s)
   (format s "/~D" (get-sort-ordnum (get-catattr range)))
   (pprint-newline :fill s)
   (format s " ordnums: ~A " (get-ordnums range))
   (pprint-newline :fill s)
   (pprint-logical-block
    (s nil :prefix "<" :suffix ">")
    (write-string "first " s)
    (pprint-newline :miser s)
    (write (get-first range) :stream s))
   (pprint-newline :mandatory s)
   (pprint-logical-block
    (s nil :prefix "<" :suffix ">")
    (write-string "inner " s)
    (pprint-newline :linear s)
    (write (get-inner range) :stream s))
   (pprint-newline :mandatory s)
   (pprint-logical-block
    (s nil :prefix "<" :suffix ">")
    (write-string "last " s)
    (pprint-newline :miser s)
    (write (get-last range) :stream s))
   (pprint-newline :mandatory s)))

#+:XP
(set-pprint-dispatch 'location-range #'pprint-location-range)

;; line 868 "index.nw"
(defvar *RCS-Identifier* '(
;; line 998 "idxentry.nw"
("idxentry" . "$Id: idxentry.nw,v 1.29 1997/03/27 17:17:25 kehr Exp $")
;; line 895 "index.nw"
("index" . "$Id: index.nw,v 1.31 1997/03/26 16:18:37 kehr Exp $")
;; line 606 "ranges.nw"
("ranges" . "$Id: ranges.nw,v 1.11 1997/03/26 16:18:41 kehr Exp $")
;; line 868 "index.nw"
                                               ))
