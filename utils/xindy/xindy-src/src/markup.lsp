;; line 1154 "markup.nw"
;; $Id: markup.nw,v 1.24 1997/03/26 16:18:58 kehr Exp $

(lisp:defpackage "MARKUP")
(lisp:in-package "MARKUP")
(lisp:provide "markup")
#+CLISP (lisp:require "base")
#+CLISP (lisp:require "locref")
#+CLISP (lisp:require "idxstyle")
#+CLISP (lisp:require "index")
#+CLISP (lisp:require "ordrules")
#+CLISP (lisp:require "version")

(eval-when (compile load eval)
  (lisp:use-package "CLOS")
  #+(and :XP CLISP) (lisp:use-package "XP")
  #+CLISP (setq custom:*suppress-check-redefinition* t)
  #-CLISP (lisp:require "base")
  #-CLISP (lisp:require "locref")
  #-CLISP (lisp:require "idxstyle")
  #-CLISP (lisp:require "index")
  (lisp:use-package "BASE")
  (lisp:use-package "LOCREF")
  (lisp:use-package "IDXSTYLE")
  (lisp:use-package "INDEX"))

;; line 1222 "markup.nw"
;; $Id: markup.nw,v 1.24 1997/03/26 16:18:58 kehr Exp $

;; line 71 "markup.nw"
(defparameter *markup-output-stream* *standard-output*)
(defparameter *markup-verbose-mode*  nil)
(defparameter *markup-verbose-open*  "<")
(defparameter *markup-verbose-close* ">")
(defparameter *markup-indentation*   2)
(defparameter *markup-indent-level*  0)
(defparameter *empty-markup* "")

(defvar *markup-percentage-list*)
(defvar *current-number*)

(defun do-markup-indent ()
  (incf *markup-indent-level* *markup-indentation*))
(defun do-markup-outdent ()
  (decf *markup-indent-level* *markup-indentation*))
;; line 102 "markup.nw"
(defun do-markup-string (str)
  (declare (inline))
  (write-string str *markup-output-stream*))
;; line 111 "markup.nw"
(defun do-markup-default (str &optional arg1 arg2 arg3)
  (when *markup-verbose-mode*
    (loop for x from 1 to *markup-indent-level*
          do (write-string " " *markup-output-stream*))
    (do-markup-string *markup-verbose-open*)
    (do-markup-string str)
    (when arg1
      (format *markup-output-stream* " [~S]" arg1)
      (when arg2
        (format *markup-output-stream* " [~S]" arg2)
        (when arg3
          (format *markup-output-stream* " [~S]" arg3)
          )))
    (do-markup-string *markup-verbose-close*)
    (terpri *markup-output-stream*)
    ))
;; line 138 "markup.nw"
(defmacro do-markup-list (some-list
                          &key
                          identifier counter
                          elt-body   sep-body
                          open-body  close-body)
  `(PROGN
    ,(when open-body `,open-body)
    (LET ,(if counter
              `((LIST-END (CAR (LAST ,some-list)))
                (COUNTER ,counter))
              `((LIST-END (CAR (LAST ,some-list)))))
      (DO ((SLIST ,some-list (CDR SLIST)))
          ((ENDP SLIST))
        (LET ((,identifier (CAR SLIST)))
          ,elt-body
          ,(if sep-body
               `(UNLESS (EQL ,identifier LIST-END)
                 ,sep-body)))))
    ,(when close-body close-body)))
;; line 162 "markup.nw"
#|
(macroexpand '(markup-list '(1 2 3) :identifier FOO
               :open-body  (print "open")
               :close-body (print "close")
               :elt-body   (print FOO)
               :sep-body   (print ",")))

expands to

(PROGN (PRINT "open")
  (LET ((LIST-END (CAR (LAST '(1 2 3)))))
    (DO ((SLIST '(1 2 3) (CDR SLIST))) ((ENDP SLIST))
      (LET ((FOO (CAR SLIST))) (PRINT FOO)
        (UNLESS (EQL FOO LIST-END) (PRINT ","))
  ) ) )
  (PRINT "close")
) ;
T
|#
;; line 193 "markup.nw"
(defmacro define-list-environment-methods (name
                                           signature
                                           &key open close sep declare body)
  (let ((name (stringify name)))
    `(EVAL-WHEN (COMPILE LOAD EVAL)
      (HANDLER-BIND ((WARNING #'MUFFLE-WARNING))
       (CL:DEFMETHOD
           ,(intern (string-upcase (concatenate 'string name "-open"))
                    'markup)
           ,signature ,@declare ,@open ,@body)
       (CL:DEFMETHOD
           ,(intern (string-upcase (concatenate 'string name "-close"))
                    'markup)
           ,signature ,@declare ,@close ,@body)
       (CL:DEFMETHOD
           ,(intern (string-upcase (concatenate 'string name "-sep"))
                    'markup)
           ,signature ,@declare ,@sep ,@body)))))
;; line 214 "markup.nw"
(defmacro define-environment-methods (name
                                      signature
                                      &key open close declare body)
  (let ((name (stringify name)))
    `(EVAL-WHEN (COMPILE LOAD EVAL)
      (HANDLER-BIND ((WARNING #'MUFFLE-WARNING))
       (CL:DEFMETHOD
           ,(intern (string-upcase (concatenate 'string name "-open"))
                    'markup)
           ,signature ,@declare ,@open ,@body)
       (CL:DEFMETHOD
           ,(intern (string-upcase (concatenate 'string name "-close"))
                    'markup)
           ,signature ,@declare ,@close ,@body)))))

(defmacro define-method (name
                         signature
                         &key declare body)
  (let ((name (stringify name)))
    `(EVAL-WHEN (COMPILE LOAD EVAL)
      (HANDLER-BIND ((WARNING #'MUFFLE-WARNING))
       (CL:DEFMETHOD
           ,(intern (string-upcase name) 'markup)
           ,signature ,@declare ,@body)))))
;; line 326 "markup.nw"
(defmethod do-markup-index ((idx base-index))
  (setq *current-number* 0)
  (setq *markup-percentage-list* index:*percentage-list*)
  (do-markup-index-open idx)
  (do-markup-list (get-entries idx)
    :identifier LETTER-GRP
    :open-body  (do-markup-letter-group-list-open)
    :elt-body   (do-markup-letter-group LETTER-GRP)
    :sep-body   (do-markup-letter-group-list-sep)
    :close-body (do-markup-letter-group-list-close))
  (index:print-rest-of-percentages *markup-percentage-list*)
  (do-markup-index-close idx))

(define-environment-methods do-markup-index ((idx base-index))
  :open  ((do-markup-default "INDEX:OPEN")
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "INDEX:CLOSE")))
;; line 351 "markup.nw"
(defmacro markup-index (&whole whole &rest args)
  (destructuring-switch-bind (&key
                              open close hierdepth
                              &switch
                              tree flat)
      args
    (let (hierdepth-cmd)
      (when (or hierdepth tree flat)
        (cond
          ((and tree flat)
             (error "you can't specify :tree and :flat simultaneously in ~%~S~%"
                     whole))
          ((and hierdepth (or tree flat))
             (error "you can't specify :hierdepth with :tree or :flat simultaneously in ~%~S~%"
                     whole))
          (flat (setq hierdepth-cmd
                      `(SET-HIERDEPTH 0 *INDEX*)));; no tree-structure
          ;; MOST-POSITIVE-FIXNUM means make all trees
          (tree (setq hierdepth-cmd
                      `(SET-HIERDEPTH MOST-POSITIVE-FIXNUM *INDEX*)))
          (hierdepth
             (when (not (numberp hierdepth))
               (error "~S is not a number in ~S~%" whole))
             (setq hierdepth-cmd
                   `(SET-HIERDEPTH ,hierdepth *INDEX*))))
        `(LET ()
          (markup::define-environment-methods
              DO-MARKUP-INDEX ((idx index:base-index))
            :declare ((declare (ignore idx)))
            :open  ,(when open  `((do-markup-string ,open)))
            :close ,(when close `((do-markup-string ,close))))
          ,hierdepth-cmd)))))
;; line 390 "markup.nw"
(define-list-environment-methods do-markup-letter-group-list ()
  :open  ((do-markup-default "LETTER-GROUP-LIST:OPEN")
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LETTER-GROUP-LIST:CLOSE"))
  :sep   ((do-markup-default "LETTER-GROUP-LIST:SEP")))
;; line 409 "markup.nw"
(defmethod do-markup-letter-group ((letter-grp letter-group))
  (let ((group-definition (get-group-definition letter-grp)))
    (do-markup-letter-group-open       group-definition)
    (do-markup-letter-group-head-open  group-definition)
    (do-markup-letter-group-head       group-definition)
    (do-markup-letter-group-head-close group-definition)
    (do-markup-list (get-members letter-grp)
      :identifier IDXENT #| the identifier to use in the expansion |#
      :open-body  (do-markup-indexentry-list-open  0
                    #|initial depth:=0|#)
      :elt-body   (do-markup-indexentry IDXENT     0)
      :sep-body   (do-markup-indexentry-list-sep   0)
      :close-body (do-markup-indexentry-list-close 0))
    (do-markup-letter-group-close group-definition)))

(define-environment-methods do-markup-letter-group
    ((group letter-group-definition))
  :open  ((do-markup-default "LETTER-GROUP:OPEN"  (get-name group))
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LETTER-GROUP:CLOSE" (get-name group))))

(define-environment-methods do-markup-letter-group-head
    ((group letter-group-definition))
  :open  ((do-markup-default "LETTER-GROUP-HEAD:OPEN"  (get-name group))
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LETTER-GROUP-HEAD:CLOSE" (get-name group))))

(define-method do-markup-letter-group-head ((group letter-group-definition))
  :body ((do-markup-default "LETTER-GROUP-HEAD" (get-name group))))
;; line 503 "markup.nw"
(define-list-environment-methods do-markup-indexentry-list ((depth number))
  :open  ((do-markup-default "INDEXENTRY-LIST:OPEN"  depth)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "INDEXENTRY-LIST:CLOSE" depth))
  :sep   ((do-markup-default "INDEXENTRY-LIST:SEP"   depth)))
;; line 525 "markup.nw"
(defmethod do-markup-indexentry ((idxent index-entry)
                                 (depth number))
  (when (>= (incf *current-number*) (caar *markup-percentage-list*))
    (index:print-percent (pop *markup-percentage-list*)))

  (do-markup-indexentry-open depth)

  (let ((print-key (get-print-key idxent)))
    (do-markup-list (merge-print-and-main-key print-key (get-main-key idxent))
      :identifier KEYWORD
      :open-body  (do-markup-keyword-list-open  depth)
      :elt-body   (do-markup-keyword   KEYWORD  depth)
      :sep-body   (do-markup-keyword-list-sep   depth)
      :close-body (do-markup-keyword-list-close depth)))

  (let ((locrefs (get-locrefs idxent)))
    (unless (endp locrefs)
      (do-markup-list locrefs
        :identifier LOCCLS-GRP
        :open-body  (do-markup-locclass-list-open)
        :elt-body   (do-markup-locclass LOCCLS-GRP)
        :sep-body   (do-markup-locclass-list-sep)
        :close-body (do-markup-locclass-list-close))))

  (let ((subentries (get-subentries idxent)))
    (unless (endp subentries)
      (let ((new-depth (1+ depth)))
        (do-markup-list subentries
          :identifier IDXENT
          :open-body  (do-markup-indexentry-list-open  new-depth)
          :elt-body   (do-markup-indexentry IDXENT     new-depth)
          :sep-body   (do-markup-indexentry-list-sep   new-depth)
          :close-body (do-markup-indexentry-list-close new-depth)))))

  (do-markup-indexentry-close depth))

(defun merge-print-and-main-key (print-key main-key)
  ;;(info "~&(merge-print-and-main-key ~S ~S)" print-key main-key)
  (if print-key
      (mapcar #'(lambda (print main)
                  (or print main))
              print-key main-key)
      main-key))
;; line 571 "markup.nw"
(define-environment-methods do-markup-indexentry ((depth number))
  :open  ((do-markup-default "INDEXENTRY:OPEN"  depth)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "INDEXENTRY:CLOSE" depth)))
;; line 595 "markup.nw"
(define-list-environment-methods do-markup-keyword-list ((depth number))
  :open  ((do-markup-default "KEYWORD-LIST:OPEN"  depth)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "KEYWORD-LIST:CLOSE" depth))
  :sep   ((do-markup-default "KEYWORD-LIST:SEP"   depth)))
;; line 617 "markup.nw"
(defmethod do-markup-keyword (keyword (depth number))
  (do-markup-keyword-open depth)
  (do-markup-string keyword)
  (do-markup-keyword-close depth))
;; line 624 "markup.nw"
(define-environment-methods do-markup-keyword ((depth number))
  :open  ((do-markup-default "KEYWORD:OPEN"  depth)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "KEYWORD:CLOSE" depth)))
;; line 654 "markup.nw"
(define-list-environment-methods do-markup-locclass-list ()
  :open  ((do-markup-default "LOCCLASS-LIST:OPEN")
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LOCCLASS-LIST:CLOSE"))
  :sep   ((do-markup-default "LOCCLASS-LIST:SEP")))
;; line 677 "markup.nw"
(defmethod do-markup-locclass ((locref-cls-grp locref-class-group))
  (let ((locclass (get-locclass locref-cls-grp)))
    (do-markup-locref-class-open locclass)

    (do-markup-list (get-members locref-cls-grp)
      :identifier ATTRIBUTE-GRP
      :open-body  (do-markup-attribute-group-list-open)
      :elt-body   (do-markup-attribute-group ATTRIBUTE-GRP locclass)
      :sep-body   (do-markup-attribute-group-list-sep)
      :close-body (do-markup-attribute-group-list-close))

    (do-markup-locref-class-close locclass)))

(define-environment-methods do-markup-locref-class
    ((locrefcls layered-location-class))
  :open  ((do-markup-default "LOCREF-CLASS:OPEN"  (get-name locrefcls))
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LOCREF-CLASS:CLOSE" (get-name locrefcls))))
;; line 1044 "markup.nw"
(defmethod do-markup-locclass ((xref-cls-grp crossref-class-group))
  (let ((xrefclass (get-locclass xref-cls-grp)))

    (do-markup-list (get-members xref-cls-grp)
      :identifier XREF
      :open-body  (do-markup-crossref-list-open  xrefclass)
      :elt-body   (do-markup-crossref            XREF)
      :sep-body   (do-markup-crossref-list-sep   xrefclass)
      :close-body (do-markup-crossref-list-close xrefclass))))

(define-list-environment-methods do-markup-crossref-list
    ((xrefclass crossref-location-class))
  :open  ((do-markup-default "CROSSREF-LIST:OPEN"  (get-name xrefclass))
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "CROSSREF-LIST:CLOSE" (get-name xrefclass)))
  :sep   ((do-markup-default "CROSSREF-LIST:SEP"   (get-name xrefclass))))
;; line 1080 "markup.nw"
(defmethod do-markup-crossref ((xref crossref-location-reference))
  (let ((xrefclass (get-locclass xref)))

    (do-markup-list (get-target xref)
      :identifier XREF-LAYER
      :open-body  (do-markup-crossref-layer-list-open  xrefclass)
      :elt-body   (do-markup-crossref-layer XREF-LAYER xrefclass)
      :sep-body   (do-markup-crossref-layer-list-sep   xrefclass)
      :close-body (do-markup-crossref-layer-list-close xrefclass))))

(define-list-environment-methods do-markup-crossref-layer-list
    ((xref-class crossref-location-class))
  :open  ((do-markup-default "CROSSREF-LAYER-LIST:OPEN"
            (get-name xref-class))
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "CROSSREF-LAYER-LIST:CLOSE"
            (get-name xref-class)))
  :sep   ((do-markup-default "CROSSREF-LAYER-LIST:SEP"
            (get-name xref-class))))
;; line 1119 "markup.nw"
(defmethod do-markup-crossref-layer (xref-layer
                                     (xref-class crossref-location-class))
  (do-markup-crossref-layer-open  xref-class)
  (do-markup-string               xref-layer)
  (do-markup-crossref-layer-close xref-class))

(define-environment-methods do-markup-crossref-layer
    ((xref-class crossref-location-class))
  :open  ((do-markup-default "CROSSREF-LAYER:OPEN"  (get-name xref-class))
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "CROSSREF-LAYER:CLOSE" (get-name xref-class))))
;; line 718 "markup.nw"
(define-list-environment-methods do-markup-attribute-group-list ()
  :open  ((do-markup-default "ATTRIBUTE-GROUP-LIST:OPEN")
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "ATTRIBUTE-GROUP-LIST:CLOSE"))
  :sep   ((do-markup-default "ATTRIBUTE-GROUP-LIST:SEP")))
;; line 737 "markup.nw"
(defmethod do-markup-attribute-group ((attribute-group category-attribute-group)
                                      (loccls layered-location-class))
  (let ((ordnum (get-ordnum attribute-group)))
    (do-markup-attribute-group-open ordnum)

    (do-markup-list (get-members attribute-group)
      :identifier LOCREF
      :open-body  (do-markup-locref-list-open  loccls 0)
      :elt-body   (do-markup-locref (get-catattr LOCREF) LOCREF loccls 0)
      :sep-body   (do-markup-locref-list-sep   loccls 0)
      :close-body (do-markup-locref-list-close loccls 0))

    (do-markup-attribute-group-close ordnum)))

(define-environment-methods do-markup-attribute-group ((ordnum number))
  :open  ((do-markup-default "ATTRIBUTE-GROUP:OPEN"  ordnum)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "ATTRIBUTE-GROUP:CLOSE" ordnum)))
;; line 776 "markup.nw"
(define-list-environment-methods do-markup-locref-list
    ((loccls layered-location-class) (depth number))
  :open  ((do-markup-default "LOCREF-LIST:OPEN"  (get-name loccls) depth)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LOCREF-LIST:CLOSE" (get-name loccls) depth))
  :sep   ((do-markup-default "LOCREF-LIST:SEP"   (get-name loccls) depth)))
;; line 808 "markup.nw"
(defmethod do-markup-locref ((attr   category-attribute)
                             (locref location-reference)
                             (loccls layered-location-class)
                             (depth  number))
  #+ASSERT! (assert! (and (typep locref 'layered-location-reference)
                          (typep depth 'number)))
  (let ((attr    (get-catattr  locref))
        (subrefs (get-subrefs  locref))
        (new-depth (1+ depth)))

    (do-markup-locref-open attr loccls depth)

    (cond
      (subrefs
         (let ((layer 0))
           (do-markup-list (get-layers locref)
             :identifier LOCREF-LAYER
             :open-body  (do-markup-locref-layer-list-open  loccls depth)
             :elt-body   (do-markup-locref-layer LOCREF-LAYER loccls depth
                                                 (prog1 layer
                                                   (incf layer)))
             :sep-body   (do-markup-locref-layer-list-sep   loccls depth)
             :close-body (do-markup-locref-layer-list-close loccls depth)))

         (do-markup-list subrefs
           :identifier LOCREF
           :open-body  (do-markup-locref-list-open  loccls new-depth)
           :elt-body   (do-markup-locref attr LOCREF loccls new-depth)
           :sep-body   (do-markup-locref-list-sep   loccls new-depth)
           :close-body (do-markup-locref-list-close loccls new-depth)))

      ((= 0 depth)
         (do-markup-string (get-locref-string locref)))

      (t (let ((layer 0))
           (do-markup-list (get-layers locref)
             :identifier LOCREF-LAYER
             :open-body  (do-markup-locref-layer-list-open  loccls depth)
             :elt-body   (do-markup-locref-layer LOCREF-LAYER loccls depth
                                                 (prog1 layer
                                                   (incf layer)))
             :sep-body   (do-markup-locref-layer-list-sep   loccls depth)
             :close-body (do-markup-locref-layer-list-close loccls depth)))))

    (do-markup-locref-close attr loccls depth)))
;; line 855 "markup.nw"
(define-list-environment-methods do-markup-locref-layer-list
    ((loccls layered-location-class) (depth  number))
  :open  ((do-markup-default "LOCREF-LAYER-LIST:OPEN"  (get-name loccls) depth)
          (do-markup-indent))
  :sep   ((do-markup-default "LOCREF-LAYER-LIST:SEP"   (get-name loccls) depth))
  :close ((do-markup-outdent)
          (do-markup-default "LOCREF-LAYER-LIST:CLOSE" (get-name loccls) depth)))
;; line 883 "markup.nw"
(defun do-markup-locref-layer (locref-layer loccls depth layer)
  (do-markup-locref-layer-open loccls depth layer)
  (do-markup-string locref-layer)
  (do-markup-locref-layer-close loccls depth layer))

(define-environment-methods do-markup-locref-layer
    ((locref-class layered-location-class) (depth number) (layer number))
  :open  ((do-markup-default "LOCREF-LAYER:OPEN"
            (get-name locref-class) depth layer)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LOCREF-LAYER:CLOSE"
            (get-name locref-class) depth layer)))
;; line 918 "markup.nw"
(define-environment-methods do-markup-locref ((attr   category-attribute)
                                              (loccls layered-location-class)
                                              (depth  number))
  :open  ((do-markup-default "LOCREF:OPEN"
            (get-name attr) (get-name loccls) depth)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "LOCREF:CLOSE"
            (get-name attr) (get-name loccls) depth)))
;; line 955 "markup.nw"
(defmethod do-markup-locref ((attr  category-attribute)
                             (range location-range)
                             (loccls layered-location-class)
                             (depth number))
  (let ((length (get-length range)))
    (do-markup-range-open attr loccls length)
    (do-markup-locref attr (get-first range) loccls depth)
    (do-markup-range-sep attr loccls length)
    (when (markup-range-print-end-p loccls length)
      (do-markup-locref attr (get-last range)  loccls depth))
    (do-markup-range-close attr loccls length)))

(define-list-environment-methods do-markup-range
    ((attr category-attribute) (loccls layered-location-class) (length number))
  :open  ((do-markup-default "RANGE:OPEN" (get-name attr) (get-name loccls) length)
          (do-markup-indent))
  :close ((do-markup-outdent)
          (do-markup-default "RANGE:CLOSE" (get-name attr) (get-name loccls) length))
  :sep   ((do-markup-default "RANGE:SEP" (get-name loccls) length)))

(defmethod markup-range-print-end-p ((loccls layered-location-class)
                                     (length number))
  t)
;; line 304 "markup.nw"
(defmacro markup-trace (&rest args)
  (destructuring-switch-bind (&key
                              (open  *markup-verbose-open*)
                              (close *markup-verbose-close*)
                              &switch
                              on)
      args
    (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 312 "markup.nw"
          (t `(LET ()
               (SETQ *markup-verbose-open*  ,open)
               (SETQ *markup-verbose-close* ,close)
               ,(when on `(SETQ *markup-verbose-mode* t)))))))
;; line 399 "markup.nw"
(defmacro markup-letter-group-list (&key open close sep)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 401 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-LETTER-GROUP-LIST ()
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 443 "markup.nw"
(defmacro markup-letter-group (&whole whole &rest args)
  (destructuring-switch-bind (&key
                              open close group
                              open-head close-head
                              &switch
                              upcase downcase capitalize)
      args
    (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 451 "markup.nw"

;; line 252 "markup.nw"
((and open-head (not (stringp open-head)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open-head))
((and close-head (not (stringp close-head)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close-head))
;; line 452 "markup.nw"

;; line 275 "markup.nw"
((and group (progn
              (setq group (stringify group))
              (not (lookup-letter-group-definition *indexstyle* group))))
 (nraw "parameter `~S' is not a valid letter-group! (ignored)~%" group))
;; line 453 "markup.nw"
          ((or (and upcase downcase)
               (and upcase capitalize)
               (and downcase capitalize))
             (error "more than one modifier in~%~S" whole))
          (t `(LET ()
               (markup::define-environment-methods
                   DO-MARKUP-LETTER-GROUP
                   (,(if group
                         `(lg-def (EQL ',(lookup-letter-group-definition
                                          *indexstyle* group)))
                         '(lg-def letter-group-definition)))
                 :declare ((declare (ignore lg-def)))
                 :open  ,(when open  `((do-markup-string ,open)))
                 :close ,(when close `((do-markup-string ,close))))
               (markup::define-environment-methods
                   DO-MARKUP-LETTER-GROUP-HEAD
                   (,(if group
                         `(lg-def (EQL ',(lookup-letter-group-definition
                                          *indexstyle* group)))
                         '(lg-def letter-group-definition)))
                 :declare ((declare (ignore lg-def)))
                 :open  ,(when open-head  `((do-markup-string ,open-head)))
                 :close ,(when close-head `((do-markup-string ,close-head))))
               ,(when (or open-head close-head)
                      `(markup::define-method
                        DO-MARKUP-LETTER-GROUP-HEAD
                        (,(if group
                              `(lg-def (EQL ',(lookup-letter-group-definition
                                               *indexstyle* group)))
                              '(lg-def letter-group-definition)))
                        :body ((do-markup-string
                                   ,(cond (upcase     `(string-upcase
                                                        (get-name lg-def)))
                                          (downcase   `(string-downcase
                                                        (get-name lg-def)))
                                          (capitalize `(string-capitalize
                                                        (get-name lg-def)))
                                          (t `(get-name lg-def))))))))))))
;; line 512 "markup.nw"
(defmacro markup-indexentry-list (&key open close sep depth)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 514 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 515 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-INDEXENTRY-LIST
             (,(if depth `(depth (EQL ,depth)) '(depth number)))
             :declare ((declare (ignore depth)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 579 "markup.nw"
(defmacro markup-indexentry (&key open close depth)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 581 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 582 "markup.nw"
        (t `(markup::define-environment-methods
             DO-MARKUP-INDEXENTRY
             (,(if depth `(depth (EQL ,depth)) '(depth number)))
             :declare ((declare (ignore depth)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))))))
;; line 604 "markup.nw"
(defmacro markup-keyword-list (&key open close sep depth)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 606 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 607 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-KEYWORD-LIST
             (,(if depth `(depth (EQL ,depth)) '(depth number)))
             :declare ((declare (ignore depth)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 632 "markup.nw"
(defmacro markup-keyword (&key open close depth)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 634 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 635 "markup.nw"
        (t `(markup::define-environment-methods
             DO-MARKUP-KEYWORD
             (,(if depth `(depth (EQL ,depth)) '(depth number)))
             :declare ((declare (ignore depth)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))))))
;; line 663 "markup.nw"
(defmacro markup-locclass-list (&key open close sep)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 665 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-LOCCLASS-LIST ()
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 699 "markup.nw"
(defmacro markup-locref-class (&key open close class)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 701 "markup.nw"

;; line 282 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-locref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a location-reference class! (ignored)~%" class))
;; line 702 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-LOCREF-CLASS
             (,(if class
                   `(locrefcls (EQL ',(cdr (lookup-locref-class
                                            *indexstyle* class))))
                   '(locrefcls layered-location-class)))
             :declare ((declare (ignore locrefcls)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))))))
;; line 727 "markup.nw"
(defmacro markup-attribute-group-list (&key open close sep)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 729 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-ATTRIBUTE-GROUP-LIST ()
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 759 "markup.nw"
(defmacro markup-attribute-group (&key open close group)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 761 "markup.nw"

;; line 275 "markup.nw"
((and group (progn
              (setq group (stringify group))
              (not (lookup-letter-group-definition *indexstyle* group))))
 (nraw "parameter `~S' is not a valid letter-group! (ignored)~%" group))
;; line 762 "markup.nw"
        (t `(markup::define-environment-methods
             DO-MARKUP-ATTRIBUTE-GROUP
             (,(if group
                   `(ordnum (EQL ,group))
                   '(ordnum number)))
             :declare ((declare (ignore ordnum)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))))))
;; line 786 "markup.nw"
(defmacro markup-locref-list (&key open close sep class depth)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 788 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 789 "markup.nw"

;; line 282 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-locref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a location-reference class! (ignored)~%" class))
;; line 790 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-LOCREF-LIST
             (,(if class
                   `(locrefcls (EQL ',(cdr (lookup-locref-class
                                            *indexstyle* class))))
                   '(locrefcls layered-location-class))
              ,(if depth `(depth (EQL ,depth)) '(depth number)))
             :declare ((declare (ignore locrefcls depth)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 865 "markup.nw"
(defmacro markup-locref-layer-list (&key open close sep class depth)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 867 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 868 "markup.nw"

;; line 282 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-locref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a location-reference class! (ignored)~%" class))
;; line 869 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-LOCREF-LAYER-LIST
             (,(if class
                   `(locrefcls (EQL ',(cdr (lookup-locref-class
                                            *indexstyle* class))))
                   '(locrefcls layered-location-class))
              ,(if depth `(depth (EQL ,depth)) '(depth number)))
             :declare ((declare (ignore locrefcls depth)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 899 "markup.nw"
(defmacro markup-locref-layer (&key class open close depth layer)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 901 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 902 "markup.nw"

;; line 270 "markup.nw"
((and layer (not (integerp layer)))
 (nraw "parameter `~S' is not a number! (ignored)~%" layer))
;; line 903 "markup.nw"

;; line 282 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-locref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a location-reference class! (ignored)~%" class))
;; line 904 "markup.nw"
        (t `(markup::define-environment-methods
             DO-MARKUP-LOCREF-LAYER
             (,(if class
                   `(locrefcls (EQL ',(cdr (lookup-locref-class
                                            *indexstyle* class))))
                   '(locrefcls layered-location-class))
              ,(if depth `(depth (EQL ,depth)) '(depth number))
              ,(if layer `(layer (EQL ,layer)) '(layer number)))
             :declare ((declare (ignore depth layer)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))))))
;; line 930 "markup.nw"
(defmacro markup-locref (&key open close class attr depth)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 932 "markup.nw"

;; line 294 "markup.nw"
((and attr (progn (setq attr (stringify attr))
                   (not (lookup-catattr *indexstyle* attr))))
 (nraw "parameter `~S' is not an attribute! (ignored)~%" attr))
;; line 933 "markup.nw"

;; line 265 "markup.nw"
((and depth (not (integerp depth)))
 (nraw "parameter `~S' is not a number! (ignored)~%" depth))
;; line 934 "markup.nw"

;; line 282 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-locref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a location-reference class! (ignored)~%" class))
;; line 935 "markup.nw"
        (t `(markup::define-environment-methods
             DO-MARKUP-LOCREF
             (,(if attr
                   `(attr (EQL ',(lookup-catattr *indexstyle* attr)))
                   '(attr category-attribute))
              ,(if class
                   `(locrefcls (EQL ',(cdr (lookup-locref-class
                                            *indexstyle* class))))
                   '(locrefcls layered-location-class))
              ,(if depth `(depth (EQL ,depth)) '(depth number)))
             :declare ((declare (ignore attr locrefcls depth)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))))))
;; line 1002 "markup.nw"
(defmacro markup-range (&whole whole &rest args)
  (destructuring-switch-bind (&key
                              open close sep class attr length
                              &switch ignore-end)
      args
    (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 1008 "markup.nw"

;; line 282 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-locref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a location-reference class! (ignored)~%" class))
;; line 294 "markup.nw"
((and attr (progn (setq attr (stringify attr))
                   (not (lookup-catattr *indexstyle* attr))))
 (nraw "parameter `~S' is not an attribute! (ignored)~%" attr))
;; line 1009 "markup.nw"
          ((and length (not (numberp length)))
             (nraw "parameter `~S' is not a number! (ignored)~%" length))
          (t `(let ()
               (markup::define-list-environment-methods
                   DO-MARKUP-RANGE
                   (,(if attr
                         `(attr (EQL ',(lookup-catattr *indexstyle* attr)))
                         '(attr category-attribute))
                   ,(if class
                         `(locrefcls (EQL ',(cdr (lookup-locref-class
                                                  *indexstyle* class))))
                         '(locrefcls layered-location-class))
                    ,(if length
                         `(length (EQL ,length))
                         '(length number)))
                 :open  ,(when open  `((do-markup-string ,open)))
                 :close ,(when close `((do-markup-string ,close)))
                 :sep   ,(when sep   `((do-markup-string ,sep))))
               (define-method markup::MARKUP-RANGE-PRINT-END-P
                   (,(if class
                         `(locrefcls (EQL ',(cdr (lookup-locref-class
                                                  *indexstyle* class))))
                         '(locrefcls layered-location-class))
                    ,(if length
                         `(length (EQL ,length))
                         '(length number)))
                 :declare ((declare (ignore attr locrefcls length)))
                 :body    (,(not ignore-end))))))))
;; line 1064 "markup.nw"
(defmacro markup-crossref-list (&key open sep close class)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 1066 "markup.nw"

;; line 288 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-crossref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a cross-reference class! (ignored)~%" class))
;; line 1067 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-CROSSREF-LIST
             (,(if class
                   `(xrefcls (EQL ',(cdr (lookup-crossref-class *indexstyle*
                                                                class))))
                   '(xrefcls crossref-location-class)))
             :declare ((declare (ignore xrefcls)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 1103 "markup.nw"
(defmacro markup-crossref-layer-list (&key open sep close class)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 260 "markup.nw"
((and sep  (not (stringp sep)))
 (nraw "parameter `~S' is not a string! (ignored)~%" sep))
;; line 1105 "markup.nw"

;; line 288 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-crossref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a cross-reference class! (ignored)~%" class))
;; line 1106 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-CROSSREF-LAYER-LIST
             (,(if class
                   `(xrefcls (EQL ',(cdr (lookup-crossref-class *indexstyle*
                                                                class))))
                   '(xrefcls crossref-location-class)))
             :declare ((declare (ignore xrefcls)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))
             :sep   ,(when sep   `((do-markup-string ,sep)))))))
;; line 1134 "markup.nw"
(defmacro markup-crossref-layer (&key open close class)
  (cond
;; line 245 "markup.nw"
((and open (not (stringp open)))
 (nraw "parameter `~S' is not a string! (ignored)~%" open))
((and close (not (stringp close)))
 (nraw "parameter `~S' is not a string! (ignored)~%" close))
;; line 1136 "markup.nw"

;; line 288 "markup.nw"
((and class (progn (setq class (stringify class))
                   (not (lookup-crossref-class *indexstyle* class))))
 (nraw "parameter `~S' is not a cross-reference class! (ignored)~%" class))
;; line 1137 "markup.nw"
        (t `(markup::define-list-environment-methods
             DO-MARKUP-CROSSREF-LAYER
             (,(if class
                   `(xrefcls (EQL ',(cdr (lookup-crossref-class *indexstyle*
                                                                class))))
                   '(xrefcls crossref-location-class)))
             :declare ((declare (ignore xrefcls)))
             :open  ,(when open  `((do-markup-string ,open)))
             :close ,(when close `((do-markup-string ,close)))))))
;; line 1188 "markup.nw"
(eval-when (compile load eval)
  (defparameter *markup-user-interface-definitions*
    '(*markup-verbose-mode*
      *markup-verbose-open*
      *markup-verbose-close*
      markup-crossref-layer
      markup-crossref-layer-list
      markup-crossref-list
      markup-index
      markup-letter-group
      markup-letter-group-list
      markup-indexentry
      markup-indexentry-list
      markup-keyword
      markup-keyword-list
      markup-locclass-list
      markup-locref-class
      markup-attribute-group-list
      markup-attribute-group
      markup-locref-list
      markup-locref
      markup-locref-layer-list
      markup-locref-layer
      markup-range
      markup-trace
      )))

;; line 1238 "markup.nw"
(eval-when (compile load eval)

;; line 89 "markup.nw"
(export '(*markup-output-stream*
          *markup-verbose-mode*
          *markup-verbose-open*
          *markup-verbose-close*
          *indexstyle-readtable*))
;; line 347 "markup.nw"
(export '(do-markup-index))
;; line 1217 "markup.nw"
(export '*markup-user-interface-definitions*)
(export *markup-user-interface-definitions*)
;; line 1240 "markup.nw"
  )

;; line 1178 "markup.nw"
(defvar *RCS-Identifier* '(
;; line 1244 "markup.nw"
("markup" . "$Id: markup.nw,v 1.24 1997/03/26 16:18:58 kehr Exp $")
;; line 236 "startup.nw"
("startup" . "$Id: startup.nw,v 1.17 1997/03/26 16:19:03 kehr Exp $")
;; line 1178 "markup.nw"
                                               ))

;; this should be the last of the module since it defines the
;; additional package `xindy'.
;; line 210 "startup.nw"
;; $Id: startup.nw,v 1.17 1997/03/26 16:19:03 kehr Exp $

(lisp:defpackage  "XINDY")
(lisp:in-package  "XINDY")

(eval-when (compile load eval)
  (lisp:use-package "BASE")
  (lisp:use-package :xindy-version)
  (lisp:use-package "MARKUP")
  (lisp:use-package "CLOS")
  (lisp:use-package "COMMON-LISP")
  #+CLISP (lisp:use-package "EXT")
  (lisp:import markup:*markup-user-interface-definitions*))

(eval-when (compile load eval)
  (pushnew :HANDLER      *features*))
;; FIXME: error messages about package locks
;(eval-when (compile load eval)
;  (pushnew :BREAK-DRIVER *features*))

;; line 44 "startup.nw"
(defun issue-startup-message ()
  (info "xindy kernel version: ~A~%" *xindy-kernel-version*)
  (info "~A version ~A~%    architecture: ~A~%"
	(lisp-implementation-type) (lisp-implementation-version)
	(machine-version))
  )

(defun startup (&key idxstyle rawindex output logfile
                     show-version markup-trace (trace-level 0))
  (when show-version
    (issue-startup-message)
    (exit-normally))
  (when markup-trace (setq *markup-verbose-mode* t))
  #+:HANDLER
  (handler-case
      (do-startup idxstyle rawindex output logfile trace-level)
    (error
        (condition)
      (oops* (simple-condition-format-string    condition)
             (simple-condition-format-arguments condition))
      (error-exit)))
  #-:HANDLER
  (do-startup idxstyle rawindex output logfile trace-level))
;; line 70 "startup.nw"
(defun do-startup (idxstyle raw-index output logfile trace-level)
  (set-searchpath-by-environment)
  (setq custom:*default-file-encoding* charset:iso-8859-1)
  (when logfile
    (info "~&Opening logfile ~S " logfile)
    (handler-case
        (setq *logging-stream* (open logfile
                                     :direction :output
                                     :if-does-not-exist :create
                                     :if-exists :supersede))
      (error ()
             (oops "Opening logfile ~S failed!" logfile)
             (error-exit)))
    (info "(done)~%")
    ;; Set necessary flags...
    (setq *logging-on* t)
    (case trace-level
      (0)
      (1 (setq *mappings-trace* t))
      (2 (setq *mappings-trace* t) (setq *locref-trace* t))
      (3 (setq *mappings-trace* t) (setq *locref-trace* t))
      (t (error "Invalid :trace-level ~S !" trace-level)))
    #+:ORDRULES (when *mappings-trace*
                  (setq ordrules::*message-logging* 1))

    (multiple-value-bind (sec min hour day mon year)
        (get-decoded-time)
      (gol t ";; This logfile was generated automatically by `xindy'~%")
      (gol t ";; at ~2,'0D.~2,'0D.~4,'0D  ~2,'0D:~2,'0D:~2,'0D~%"
           day mon year hour min sec))
    (gol t ";; Indexstyle: ~S, Rawindex: ~S, Output: ~S~%~%"
         idxstyle raw-index output)
    )

  (info "~&Reading indexstyle...~%")
  (let ((*readtable* idxstyle:*indexstyle-readtable*))
    (idxstyle:do-require idxstyle))
  (info "~&Finished reading indexstyle.")
  (info "~&Finalizing indexstyle... ")
  (idxstyle:make-ready idxstyle:*indexstyle*)
  (info "(done)~%~%")

  (info "~&Reading raw-index ~S..." raw-index)
  (load raw-index :verbose nil)
  (info "~&Finished reading raw-index.~%~%")

  (handler-case
      (setq *markup-output-stream*
            (open output
                  :direction :output
                  :if-does-not-exist :create
                  :if-exists :supersede))
    (error ()
           (oops "Opening file ~S failed!" output)
           (error-exit)))

  (info "~&Processing index...")
  (index:process-index index:*index*)
  (info "~&Finished processing index.~%~%")

  (info "~&Writing markup...")
  (markup:do-markup-index index:*index*)
  (info "~%Markup written into file ~S.~%" output))
;; line 139 "startup.nw"
(defun set-searchpath-by-environment ()
  (let ((sp (#+CLISP
             system::getenv
             #+ALLEGRO
             sys:getenv
             "XINDY_SEARCHPATH")))
    (when sp (idxstyle:set-searchpath-by-string sp))))
;; line 170 "startup.nw"
#+:BREAK-DRIVER
(fmakunbound '*break-driver*)

#+:BREAK-DRIVER
(defun *break-driver* (continuable
                       &optional (condition nil) (print-it nil)
                       &aux (may-continue
                             (or continuable
                                 (and condition
                                      (find-restart 'continue condition))
                                 ) )
                       (interactive-p (interactive-stream-p *debug-io*))
                       (commandsr '())
                       )
  (declare (ignore may-continue interactive-p commandsr))
  ;; This when-clause is from Bruno Haible.
  (when (and condition print-it)
    (terpri *error-output*)
    (write-string "*** - " *error-output*)
    #+CLISP (system::print-condition condition *error-output*)
    #-CLISP (print condition *error-output*)
    )
  (format *ERROR-OUTPUT* "~&Bye.")
  (error-exit))

#+:BREAK-DRIVER-OLD
(defun *break-driver* (continuable &rest rest)
  (declare (ignore continuable rest))
  (format *ERROR-OUTPUT* "~&Bye.")
  (error-exit))

#+:BREAK-DRIVER
(eval-when (compile load eval)
  (export '*break-driver*))

;; line 230 "startup.nw"
(eval-when (compile load eval)

;; line 135 "startup.nw"
(export '(startup *xindy-kernel-version*))
;; line 232 "startup.nw"
  )
