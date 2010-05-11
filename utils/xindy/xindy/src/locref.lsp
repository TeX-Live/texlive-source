;; line 360 "locref.nw"
;; $Id: locref.nw,v 1.29 1997/03/26 16:18:53 kehr Exp $

(lisp:defpackage "LOCREF")
(lisp:in-package "LOCREF")
(lisp:provide "locref")

#+CLISP (lisp:require "base")

(eval-when (compile load eval)
  #-CLISP (lisp:require "base")
  (lisp:use-package "BASE")
  #+(and :XP CLISP) (lisp:use-package "XP")
  (lisp:use-package "CLOS"))

;; line 251 "basetype.nw"
;; $Id: basetype.nw,v 1.17 1997/03/26 16:18:47 kehr Exp $

;; line 110 "basetype.nw"
(defgeneric prefix-match (str object))
;; line 114 "basetype.nw"
(defclass basetype ()
    ((name          :initarg :name)
     (base-alphabet :initarg :base-alphabet)))
;; line 142 "basetype.nw"
(defclass alphabet (basetype)
    ((symbols :initarg :symbols)))

(defun make-alphabet (name symbols)
  (make-instance 'alphabet :name name :symbols symbols
                 :base-alphabet (calculate-base-alphabet symbols)))

(defmethod make-copy ((alph alphabet) new-name)
  (make-instance 'alphabet
                 :name          new-name
                 :symbols       (get-symbols alph)
                 :base-alphabet (get-base-alphabet alph)))
;; line 163 "basetype.nw"
(defun calculate-base-alphabet (list-of-strings)
  (sort (remove-duplicates
         (apply #'nconc
                (mapcar #'(lambda (str)
                            (coerce str 'list))
                        list-of-strings)))
        #'char<))
;; line 183 "basetype.nw"
(defmethod prefix-match ((str string) (alph alphabet))
  (multiple-value-bind (matched-string rest-string order-num)
      (prefix-match-stringlist str (get-symbols alph))
    (values matched-string rest-string order-num)))

(defun prefix-match-stringlist (pattern list-of-str)
  (let* ((matched-string "")
         (rest-string pattern)
         (ordnum-of-max-match nil)
         (len-max-match 0)
         (strlen (length pattern)))
    (do ((ordnum 0 (1+ ordnum))
         (string-list list-of-str (cdr string-list)))
        ((endp string-list))
      (let ((len-curr-match (string/= pattern (car string-list))))
        (when (null len-curr-match) (setq len-curr-match strlen))
        (when (> len-curr-match len-max-match)
          (setq ordnum-of-max-match ordnum)
          (setq len-max-match len-curr-match)
          (setq rest-string (subseq pattern len-curr-match))
          (setq matched-string
                (subseq (car string-list) 0 len-curr-match)))))
    (values matched-string rest-string ordnum-of-max-match)))
;; line 227 "basetype.nw"
(defclass enumeration (basetype)
    ())

(defun make-enumeration (enum-class name base-alphabet)
  (make-instance enum-class
                 :name          name
                 :base-alphabet base-alphabet))

(defmethod make-copy ((enum enumeration) new-name)
  (make-instance (class-name (class-of enum))
                 :name          new-name
                 :base-alphabet (get-base-alphabet enum)))

;; line 257 "basetype.nw"
(eval-when (compile load eval)
  
;; line 120 "basetype.nw"
(export '(basetype prefix-match get-name get-base-alphabet))
;; line 157 "basetype.nw"
(export '(alphabet make-alphabet get-symbols make-copy))
;; line 242 "basetype.nw"
(export '(enumeration make-enumeration))
;; line 259 "basetype.nw"
  )
;; line 493 "locclass.nw"
;; $Id: locclass.nw,v 1.21 1997/03/26 16:18:49 kehr Exp $

;; line 67 "locclass.nw"
(defclass location-class ()
    ((name   :type string :initarg :name)
     (ordnum :type number :initform (gen-next-loccls-ordnum))))
;; line 87 "locclass.nw"
(defparameter *curr-loccls-ordnum* 0)

(defun gen-next-loccls-ordnum ()
  (incf *curr-loccls-ordnum*))
;; line 106 "locclass.nw"
(defclass layered-location-class (location-class)
    ((layers    :initarg  :layers)
     (hierdepth :initarg  :hierdepth)))
;; line 125 "locclass.nw"
(defclass standard-location-class (layered-location-class)
    ((join-length :initarg :join-length)))

(defun make-standard-location-class (name layers join-length hierdepth)
  (make-instance 'standard-location-class
                 :name name :layers layers
                 :join-length join-length :hierdepth hierdepth))
;; line 135 "locclass.nw"
(defmacro joining-allowed-p (loccls)
  `(SLOT-VALUE ,loccls 'JOIN-LENGTH))
;; line 144 "locclass.nw"
(defun checked-make-standard-location-class (name layers join-layers
                                                  &optional (hierdepth 0))
  (make-standard-location-class name layers join-layers hierdepth))
;; line 163 "locclass.nw"
(defclass var-location-class (layered-location-class)
    ())

(defun make-var-location-class (name layers hierdepth)
  (make-instance 'var-location-class
                 :name name :layers layers :hierdepth hierdepth))
;; line 172 "locclass.nw"
(defun checked-make-var-location-class (name layers &optional (hierdepth 0))
  (make-var-location-class name layers hierdepth))
;; line 189 "locclass.nw"
(defclass loccls-layer ()
    ((basetype :initarg :basetype)))

(defun make-loccls-layer (basetype)
  (make-instance 'loccls-layer :basetype basetype))
;; line 206 "locclass.nw"
(defmethod prefix-match ((locstring string)
                         (layer     loccls-layer))
  (prefix-match locstring (get-basetype layer)))
;; line 220 "locclass.nw"
(defclass loccls-separator ()
    ((separator :initarg :separator)))

(defun make-loccls-separator (separator)
  (make-instance 'loccls-separator :separator separator))
;; line 246 "locclass.nw"
(defmethod prefix-match ((locstring string)
                         (separator loccls-separator))
  (let* ((sepstr (get-separator separator))
         (seplen (length sepstr))
         (match-len (string/= sepstr locstring)))
    (if (or (null match-len) (eql seplen match-len))
        (values locstring (subseq locstring seplen) t)
        (values nil nil nil))))
;; line 273 "locclass.nw"
(defclass crossref-location-class (location-class)
    ((target :initarg :target)))
;; line 299 "locclass.nw"
(defclass verified-crossref-location-class (crossref-location-class)
    ())

(defun make-verified-crossref-location-class (name)
  (make-instance 'verified-crossref-location-class :name name))
;; line 319 "locclass.nw"
(defclass unverified-crossref-location-class (crossref-location-class)
    ())

(defun make-unverified-crossref-location-class (name)
  (make-instance 'unverified-crossref-location-class :name name))

;; line 374 "locclass.nw"
(defgeneric perform-match (locstring locclass))
;; line 402 "locclass.nw"
(defmethod perform-match ((locstring string)
                          (loccls    standard-location-class))
  (let ((layer-list  '())
        (ordnum-list '())
        (give-up        nil)
        (matched-string nil)
        (rest-string    nil)
        (ordnum         nil))
    (do ((layers (get-layers loccls) (cdr layers)))
        ((or give-up
             (let ((empty-string (= 0 (length locstring)))
                   (empty-list   (endp layers)))
               (cond ((and empty-string empty-list) t)
                     ((or  empty-string empty-list) (setq give-up t))
                     (t nil)))))
      (let ((curr-layer (car layers)))
        (multiple-value-setq (matched-string rest-string ordnum)
          (prefix-match locstring curr-layer))
        (cond ((eq ordnum nil) (setq give-up t))
              ((numberp ordnum)
                 (setq locstring rest-string)
                 (setq layer-list
                       (cons matched-string layer-list))
                 (setq ordnum-list (cons ordnum ordnum-list)))
              (t (setq locstring rest-string)))
        ))
    (if (not give-up)
        (values (nreverse layer-list) (nreverse ordnum-list))
        (values nil nil))))
;; line 446 "locclass.nw"
(defmethod perform-match ((locstring string)
                          (loccls    var-location-class))
  (let ((layer-list  '())
        (ordnum-list '())
        (give-up        nil)
        (matched-string nil)
        (rest-string    nil)
        (ordnum         nil))
    (do ((layers (get-layers loccls) (cdr layers)))
        ((or give-up
             (let ((empty-string (= 0 (length locstring)))
                   (empty-list   (endp layers)))
               (cond (empty-string t)
                     (empty-list (setq give-up t))
                     (t nil)))))
      (let ((curr-layer (car layers)))
        (multiple-value-setq (matched-string rest-string ordnum)
          (prefix-match locstring curr-layer))
        (cond ((eq ordnum nil) (setq give-up t))
              ((numberp ordnum)
                 (setq locstring rest-string)
                 (setq layer-list
                       (cons matched-string layer-list))
                 (setq ordnum-list (cons ordnum ordnum-list)))
              (t (setq locstring rest-string)))
        ))
    (if (not give-up)
        (values (nreverse layer-list) (nreverse ordnum-list))
        (values nil nil))))
;; line 482 "locclass.nw"
;; FIXME (do we need this)
;;(defmethod perform-match ((locstring string)
;;                          (loccls    crossref-location-class))
;;  t)

;; line 507 "locclass.nw"
(eval-when (compile load eval)
  
;; line 94 "locclass.nw"
(export '(location-class perform-match
          get-name get-ordnum set-ordnum ordnum))
;; line 112 "locclass.nw"
(export '(layered-location-class))
;; line 150 "locclass.nw"
(export '(standard-location-class  checked-make-standard-location-class
          get-join-length set-join-length
          joining-allowed-p join-length))
;; line 177 "locclass.nw"
(export '(var-location-class checked-make-var-location-class))
;; line 197 "locclass.nw"
(export '(loccls-layer make-loccls-layer
          get-basetype get-markup))
;; line 228 "locclass.nw"
(export '(loccls-separator make-loccls-separator
          get-separator get-markup))
;; line 278 "locclass.nw"
(export '(crossref-location-class get-target))
;; line 307 "locclass.nw"
(export '(verified-crossref-location-class
          make-verified-crossref-location-class))
;; line 327 "locclass.nw"
(export '(unverified-crossref-location-class
          make-unverified-crossref-location-class))
;; line 183 "locref.nw"
(export '(crossref-location-reference create-cross-reference))
;; line 215 "locref.nw"
(export 'build-location-reference)
;; line 509 "locclass.nw"
  )
;; line 56 "locref.nw"
(defclass location-reference ()
    ((locclass  :initarg :locclass)
     (attribute :initarg :attribute)))
;; line 78 "locref.nw"
(defclass layered-location-reference (location-reference)
    ((layers        :initarg :layers)
     (locref-string :initarg :string   :type string)
     (ordnums       :initarg :ordnums)
     (catattr       :initarg :catattr)
     (state         :initarg :state)
     (rangeattr     :initform 'nil)
     (origin        :initform 'nil)
     (subrefs       :initform '() :type list)))

;; line 95 "locref.nw"
(defun make-layered-location-reference (&key (string "")
                                             (layers '())    (ordnums '())
                                             (catattr nil)   (locclass nil)
                                             (attribute nil) (virtual nil))
  (make-instance 'layered-location-reference
                 :string    string
                 :layers    layers
                 :ordnums   ordnums
                 :catattr   catattr
                 :locclass  locclass
                 :attribute attribute
                 :state (if virtual 'locref-state-virtual
                                    'locref-state-normal)))
;; line 116 "locref.nw"
(defmacro state-normal-p (locref)
  `(EQL 'LOCREF-STATE-NORMAL  (SLOT-VALUE ,locref 'STATE)))

(defmacro state-virtual-p (locref)
  `(EQL 'LOCREF-STATE-VIRTUAL (SLOT-VALUE ,locref 'STATE)))

(defmacro state-deleted-p (locref)
  `(EQL 'LOCREF-STATE-DELETED (SLOT-VALUE ,locref 'STATE)))

(defun set-state-normal (locref)
  (set-state 'locref-state-normal locref)
  locref)

(defun set-state-virtual (locref)
  (set-state 'locref-state-virtual locref)
  locref)

(defun set-state-deleted (locref)
  (set-state 'locref-state-deleted locref)
  locref)
;; line 139 "locref.nw"
(defmacro rangeattr-open-p (attr)
  `(EQL ,attr :RANGEATTR-OPEN))

(defmacro rangeattr-close-p (attr)
  `(EQL ,attr :RANGEATTR-CLOSE))

(defmacro set-rangeattr-open (locref)
  `(SETF (SLOT-VALUE ,locref 'RANGEATTR) :RANGEATTR-OPEN))

(defmacro set-rangeattr-close (locref)
  `(SETF (SLOT-VALUE ,locref 'RANGEATTR) :RANGEATTR-CLOSE))
;; line 241 "locref.nw"
(defclass category-attribute ()
    ((name               :initarg :name)
     (catattr-grp-ordnum :initarg :ordnum)
     (sort-ordnum        :initform 0)
     (processing-ordnum  :initform 0)
     (last-in-group      :initarg :type)))

(defun make-category-attribute (attr-name)
  (make-instance 'category-attribute :name attr-name))
;; line 174 "locref.nw"
(defclass crossref-location-reference (location-reference)
    ((target :initarg :target)))

(defun create-cross-reference (loccls target attribute)
  (make-instance 'crossref-location-reference
                 :locclass loccls :target target :attribute attribute))
;; line 201 "locref.nw"
(defmethod build-location-reference ((locclass   layered-location-class)
                                     (locref-str string)
                                     (catattr    category-attribute)
                                     attribute)
  (multiple-value-bind (layers ordnums)
      (perform-match locref-str locclass)
    (and layers ordnums
         (make-layered-location-reference
          :locclass  locclass  :layers  layers
          :ordnums   ordnums   :catattr catattr
          :attribute attribute :string locref-str))))
;; line 288 "locref.nw"
(defun locref-class< (locref-1 locref-2)
  (declare (inline))
  (< (get-ordnum (get-locclass locref-1))
     (get-ordnum (get-locclass locref-2))))

(defun locref-class= (locref-1 locref-2)
  (declare (inline))
  (eql (get-locclass locref-1) (get-locclass locref-2)))
;; line 301 "locref.nw"
(defmacro locref-ordnum< (ordnum-list-1 ordnum-list-2)
  `(COND
    ((EQUAL ,ordnum-list-1 ,ordnum-list-2) NIL)
    (T (DO ((REST-1 ,ordnum-list-1 (CDR REST-1))
            (REST-2 ,ordnum-list-2 (CDR REST-2)))
           ((OR (ENDP REST-1) (ENDP REST-2)
                (/= (FIRST REST-1) (FIRST REST-2)))
              (COND ((ENDP REST-1) T)
                    ((ENDP REST-2) NIL)
                    (T (< (FIRST REST-1) (FIRST REST-2)))))))))

(defmacro locref-ordnum= (ordnum-list-1 ordnum-list-2)
  `(EQUAL ,ordnum-list-1 ,ordnum-list-2))
;; line 320 "locref.nw"
#|
FIXME
(defun locref< (locref-1 locref-2)
  (cond ((locref-class= locref-1 locref-2)
           (locref-ordnum< locref-1 locref-2))
        (t (locref-class< locref-1 locref-2))))
|#

(defmethod locref= ((locref-1 crossref-location-reference)
                    (locref-2 layered-location-reference))
  nil)

(defmethod locref= ((locref-1 layered-location-reference)
                    (locref-2 crossref-location-reference))
  nil)

(defmethod locref= ((locref-1 crossref-location-reference)
                    (locref-2 crossref-location-reference))
  (and (locref-class= locref-1 locref-2)
       (equal (get-target locref-1) (get-target locref-2))))

(defmethod locref= ((locref-1 layered-location-reference)
                    (locref-2 layered-location-reference))
  (and (locref-class= locref-1 locref-2)
       (eql (get-catattr locref-1) (get-catattr locref-2))
       (locref-ordnum= (get-ordnums locref-1) (get-ordnums locref-2))
       (eql (get-rangeattr locref-1) (get-rangeattr locref-2))))

;; line 391 "locref.nw"
(eval-when (compile load eval)
  
;; line 62 "locref.nw"
(export 'location-reference)
;; line 153 "locref.nw"
(export '(layered-location-reference
          make-layered-location-reference
          get-layers     set-layers    layers
          get-ordnums    set-ordnums   ordnums
          get-subrefs    set-subrefs   subrefs
          get-catattr    catattr
          get-rangeattr    rangeattr
          get-locref-string  string
          get-origin       set-origin
          state
          state-normal-p   state-virtual-p   state-deleted-p
          set-state-normal set-state-virtual set-state-deleted
          set-rangeattr-open set-rangeattr-close
          rangeattr-open-p rangeattr-close-p))
;; line 257 "locref.nw"
(export '(category-attribute make-category-attribute
          get-name               catattr-grp-ordnum
          get-catattr-grp-ordnum set-catattr-grp-ordnum
          get-sort-ordnum        set-sort-ordnum
          get-processing-ordnum  set-processing-ordnum
          get-last-in-group      set-last-in-group
          get-type   set-type
          get-markup set-markup))
;; line 350 "locref.nw"
(export '(locref-class<  locref-class=
          locref-ordnum< locref-ordnum=
          locref=))
;; line 393 "locref.nw"
  )

;; line 521 "locclass.nw"
#+:XP
(set-pprint-dispatch 'location-class
                     #'(lambda (s loccls)
                         (pprint-logical-block
                          (s nil :prefix "{" :suffix "}")
                          (write (get-name loccls) :stream s)
                          (write-string ":" s)
                          (write (get-ordnum loccls) :stream s))))

#+:XP
(set-pprint-dispatch 'layered-location-class
                     #'(lambda (s loccls)
                         (pprint-logical-block
                          (s nil :prefix "{" :suffix "}")
                          (write (get-name loccls) :stream s)
                          (write-string ":" s)
                          (write (get-ordnum loccls) :stream s)
                          (mapc #'(lambda (x)
                                    (write-string " " s)
                                    (pprint x s))
                                (get-layers loccls)))))

#+:XP
(set-pprint-dispatch 'loccls-layer
                     #'(lambda (s layer)
                         (pprint-logical-block
                          (s nil :prefix "<" :suffix ">")
                          (pprint (get-basetype layer) s))))

#+:XP
(set-pprint-dispatch 'loccls-separator
                     #'(lambda (s sep)
                         (pprint-logical-block
                          (s nil :prefix "<" :suffix ">")
                          (write (get-separator sep) :stream s))))
;; line 406 "locref.nw"
#+:XP
(defun pprint-layered-location-reference (s locref)
  (pprint-logical-block
   (s nil :prefix "[" :suffix "]")
   (cond ((state-normal-p  locref) (write-string "Nor:" s))
         ((state-virtual-p locref) (write-string "Vir:" s))
         (t (write-string "Del:" s)))
   (cond ((rangeattr-open-p  (get-rangeattr locref)) (write-string "OPEN:" s))
         ((rangeattr-close-p (get-rangeattr locref)) (write-string "CLOSE:" s)))
   (write-string "\"" s)
   (write-string (get-locref-string locref) s)
   (write-string "\"=" s)
   (write (get-layers locref) :stream s)
   (write-string "=" s)
   (write (get-ordnums locref) :stream s)
   (write-string ";" s)
   (pprint-newline :fill s)
   (write (get-locclass locref) :stream s)
   (write-string ";" s)
   (pprint-newline :fill s)
   (write (get-catattr locref) :stream s)
   (when (get-origin locref)
     (write-string "<-" s)
     (write (get-catattr (get-origin locref)) :stream s))
   (pprint-newline :fill s)
   (write (get-subrefs locref) :stream s)
   ))

#+:XP
(set-pprint-dispatch 'layered-location-reference
                     #'pprint-layered-location-reference)
;; line 440 "locref.nw"
#+:XP
(defun pprint-crossref-location-reference (s xref)
  (pprint-logical-block
   (s nil :prefix "[" :suffix "]")
   (write (get-locclass xref) :stream s)
   (write-string "->" s)
   (write (get-target xref) :stream s)
   ))

#+:XP
(set-pprint-dispatch 'crossref-location-reference
                     #'pprint-crossref-location-reference)
;; line 455 "locref.nw"
#+:XP
(set-pprint-dispatch 'category-attribute
                     #'(lambda (s catattr)
                         (write-string "`" s)
                         (write-string (get-name catattr) s)
                         (write-string "'" s)))

;; line 380 "locref.nw"
(defvar *RCS-Identifier* '( 
;; line 263 "basetype.nw"
("basetype" . "$Id: basetype.nw,v 1.17 1997/03/26 16:18:47 kehr Exp $")
;; line 513 "locclass.nw"
("locclass" . "$Id: locclass.nw,v 1.21 1997/03/26 16:18:49 kehr Exp $")
;; line 397 "locref.nw"
("locref" . "$Id: locref.nw,v 1.29 1997/03/26 16:18:53 kehr Exp $")
;; line 380 "locref.nw"
                                               ))
