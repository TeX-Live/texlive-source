;;; $Id$
;;;----------------------------------------------------------------------

;;;
;;; ordrules  ---  order rules for xindy
;;;
;;; (history at end)


(provide "ordrules")


;;;; ============================================================
;;;;
;;;; SPECIFICATION
;;;;

;;;
;;; package ordrules XXX
;;;

(defpackage "ORDRULES"
    (:documentation "Order rules for xindy.
Provides functionality for merge-rules and sort-rules, as ordrule collections.
An ordrule collection is an ordered set of order rules.
An order rule is an object that manages replacement of a string or
regular expression by a string.
Ordrule collections are applied to strings, the result is created by
successive application of matching order rules.")
  (:use common-lisp clos regexp)
  (:shadow match)
  (:export ;; object interface
	   "MAKE-ORDRULE-TABLE" "ADD-RULE" "APPLY-RULES"
	   ;; legacy (C plugin) interface
	   "INITIALIZE" "ADD-KEYWORD-MERGE-RULE" "ADD-KEYWORD-SORT-RULE"
	   "GEN-KEYWORD-MERGEKEY" "GEN-KEYWORD-SORTKEY"
	   "*MESSAGE-BUFFER-PTR*" "*MESSAGE-BUFFER*" "*MESSAGE-LOGGING*")
  )
(in-package "ORDRULES")



;;;; ============================================================
;;;;
;;;; ORDRULE OBJECTS
;;;;

;;; class ORDRULE
;;; Slots:
;;;  -- match: regexp, string, or char that might be matched
;;;  -- repl: replacement for the matched substring
;;;  -- again: boolean flag if repl should be eligible for further ordrules
;;;
;;; subclass ORDRULE-STRING -- no additional slots
;;; subclass ORDRULE-CHAR -- no additional slots
;;; subclass ORDRULE-REGEXP
;;; Additional Slots:
;;;  -- only-at-start: boolean flag if match should only be tested at start
;;;     of string

(defclass ordrule ()
    ((match :initarg :match :reader match)
     (repl :initarg :repl :reader repl)
     (again :initarg :again :reader again))
  (:documentation
   "An order rule that represents the replacement of a matching expression
with a replacement string."))

(defclass ordrule-string (ordrule)
    ()
  (:documentation
   "An ordrule where the matching expression is a string"))

(defclass ordrule-char (ordrule)
    ()
  (:documentation
   "An ordrule where the matching expression is a char"))

(defclass ordrule-regexp (ordrule)
    ((pattern :initarg :pattern :reader pattern)
     (only-at-start :initarg :only-at-start :reader only-at-start))
  (:documentation
   "An ordrule where the matching expression is a regexp"))

(defmethod print-object ((obj ordrule) stream)
  "Print an ordrule."
  (format stream "#<ordrule: '~A' => '~A' :again ~A>"
	  (match obj) (repl obj) (again obj)))

(defmethod print-object ((obj ordrule-regexp) stream)
  "Print an ordrule."
  (format stream "#<ordrule-regexp: '~A' => '~A' :again ~A :only-at-start ~A>"
	  (pattern obj) (repl obj) (again obj) (only-at-start obj)))

(defun make-ordrule-string (match repl &key again)
  "Create an ordrule-string object.
MATCH is a string to compare."
  (make-instance 'ordrule-string :match match :repl repl :again again))

(defun make-ordrule-char (match repl &key again)
  "Create an ordrule-string object.
MATCH is a character to compare."
  (make-instance 'ordrule-char :match match :repl (string repl) :again again))

(defun make-ordrule-regexp (match-pattern repl &key again (extended t))
  "Create an ordrule-regexp object.
MATCH-PATTERN is a POSIX regular expression.
The boolean keyword argument :extended tells if the regex is a basic
one or an extended one; default is to use extended ones."
  (let* ((only-at-start (char= (char match-pattern 0) #\^))
	 (match (if only-at-start (subseq match-pattern 1) match-pattern)))
    (make-instance 'ordrule-regexp
		   :match (regexp-compile (concatenate 'string "^(" match ")")
					  :extended extended)
		   :repl repl :again again
		   :pattern match-pattern :only-at-start only-at-start)))

;; A regexp-compile object does not survive dump and reload.
;; Therefore we have to initialize it at first usage.
(defvar *REGEXP-PATTERN*
  nil
  "If this pattern matches, the string is a regexp.")

(defun make-ordrule (match repl &key again)
  "Create an ordrule object.
MATCH may be a string, a char, or a POSIX regular expression.
If it's a regexp, it's an extended one."
  (or *REGEXP-PATTERN*
      (setq *REGEXP-PATTERN* (regexp-compile "[][|(){}.*+^$?]")))
  (cond ((characterp match)
	   (make-ordrule-char match repl :again again))
	((= (length match) 1)
	   (make-ordrule-char (char match 0) repl :again again))
	((regexp-exec *REGEXP-PATTERN* match)
	   (make-ordrule-regexp match repl :again again))
	(t (make-ordrule-string match repl :again again))))


;;;
;;; (try-rule RULE SOURCE) => FINAL ; SOURCE-REST  or  <no values>
;;;

(defgeneric try-rule (ordrule source &key at-start)
  (:documentation "Apply an ordrule to a source string.
If the rule does not match, return nothing. If it matches, return two
values: the 1st is the final prefix with the replacement
string and the 2nd is the rest of SOURCE that's left after the matched
text. If the rule's again flag is true, FINAL will be the empty string
and the replacement will be at the start of SOURCE-REST.
An ordrule is not allowed to turn a source completely into an empty string.
If that would happen, a warning is output and nothing is returned.
That situation would exist, if AT-START is true and both FINAL and
SOURCE-REST would be empty.
AT-START tells if we are at the start to process the source string."))

(defmethod try-rule ((rule ordrule-char) source &key at-start)
  (if (char= (char source 0) (match rule))
      (maybe-again-maybe-no rule source at-start
			    (repl rule) (subseq source 1))
    (values)))

(defun maybe-again-maybe-no (rule source at-start repl source-rest)
  (cond ((and at-start (string= repl "") (string= source-rest ""))
	   (warn "Would replace complete index key by empty string, ignoring ~S" rule)
	   (values source ""))
	((again rule)
	   (values "" (concatenate 'string repl source-rest)))
	(t (values repl source-rest))))

(defmethod try-rule ((rule ordrule-string) source &key at-start)
  (let* ((match-string (match rule))
	 (match-length (length match-string)))
    (cond ((> match-length (length source)) (values))
	  ((string= source match-string :end1 match-length)
	     (maybe-again-maybe-no rule source at-start
				   (repl rule) (subseq source match-length)))
	  (t (values)))))

(defmethod try-rule ((rule ordrule-regexp) source &key at-start)
  (if (and (only-at-start rule) (not at-start))
      nil
    (let ((match-regexp (multiple-value-list (regexp-exec (match rule) source))))
      (if match-regexp
	  (maybe-again-maybe-no rule source at-start
		       (maybe-repl-subexpr (repl rule) (rest match-regexp) source)
		       (subseq source (match-end (first match-regexp))))
	(values)))))

;; A regexp-compile object does not survive dump and reload.
;; Therefore we have to initialize it at first usage.
(defvar *REPL-PATTERN*
  nil
  "Compiled regexp that matches a backpattern in a regexp replacement text.")

;; We need to substitute all \x and & constructs in the replacement text by
;; the respective subexpression from source's matches. They are
;; located with a regexp in a loop and we collect in 'final-repl the
;; strings in front of each \x and the actual subexpression.
(defun maybe-repl-subexpr (repl source-matches source)
  (or *REPL-PATTERN*
      (setq *REPL-PATTERN* (regexp-compile "\\\\([1-9])|&" :extended t)))
  (do* ((start 0 (match-end (first subexpr-match)))
	(subexpr-match (multiple-value-list (regexp-exec *REPL-PATTERN* repl))
		       (multiple-value-list (regexp-exec *REPL-PATTERN* repl
							 :start start)))
       (final-repl ""))
      ((null subexpr-match)
	 ;; Don't forget to return the rest of the string, too. Its
	 ;; index is now in 'start.
	 (concatenate 'string final-repl (subseq repl start)))
    ;; OK, need a subexpr replacement. First, we determine which subexpression
    ;; this shall be: If it's "&", the second match subexpression will be nil,
    ;; as the parenthesis did not match anything. Otherwise the 2nd match
    ;; subexpr will be the digit. Then we get that subexpression from the
    ;; source string. Finally, we add the 'repl string part in front of \x and
    ;; the respective subexpression to 'final-repl and are ready to look for
    ;; our next \x construct.
    (let* ((subexpr-match-struct (second subexpr-match))
	   (subexpr (and subexpr-match-struct
			 (match-string repl subexpr-match-struct)))
	   (subexpr-repl (match-string source
				       (nth (if (null subexpr)
						0
					      (read-from-string subexpr))
					    source-matches))))
      (setq final-repl
	    (concatenate 'string final-repl
			 (subseq repl 0 (match-start (first subexpr-match)))
			 subexpr-repl)))))


;;;
;;; (rule-first-char RULE) => char or :regexp
;;;

(defgeneric rule-first-char (rule)
  (:documentation "Returns the first char of a rule match.
This returns either the first char of a rule match, or :regexp
if that first char is a regular expression. The is used to find the
slot for a rule in a rule table."))

(defmethod rule-first-char ((rule ordrule-char))
  (match rule))

(defmethod rule-first-char ((rule ordrule-string))
  (char (match rule) 0))

(defmethod rule-first-char ((rule ordrule-regexp))
  (let* ((pattern (pattern rule))
	 (start-index (if (only-at-start rule) 1 0))
	 (first-char (char pattern start-index)))
    (cond ((char= first-char #\\)
	     (char pattern (1+ start-index)))
	  ((find first-char "[]|(){}.*+?")
	     :regexp)
	  (t first-char))))



;;;; ============================================================
;;;;
;;;; ORDRULE TABLES
;;;;

;; An ordrule table is a hash table of ordrule lists, indexed by the first
;; character of the rule. The key :regexp is the entry for rules that start
;; with a regexp pattern.

(defclass ordrule-table ()
    ((table :initarg :table :reader table))
  (:documentation
   "A collection of ordrules."))

(defmethod print-object ((obj ordrule-table) stream)
  "Print an ordrule table."
  (format stream "#<ordrule table with ~A entries>" (hash-table-count (table obj))))

(defun make-ordrule-table ()
  "Create an ordrule-table object."
  (make-instance 'ordrule-table :table (make-hash-table :size 256)))

(defun add-rule (rule-table rule)
  "Add a rule to an ordrule table."
  (let* ((first-char (rule-first-char rule))
	 (first-char-rules (gethash first-char (table rule-table))))
    (if first-char-rules
	(nconc first-char-rules (list rule))
      (setf (gethash first-char (table rule-table)) (list rule)))))

;; XXX This is the place where we might want to sort rules according to their
;; length. But then we should also cache that sort result, because this
;; function is called for each character in the index!
;;
;; XXX Maybe we should try a cache anyhow, to save appends, too? Need to
;; measure that.
(defun get-rules (rule-table source-start)
  (append (gethash source-start (table rule-table))
	  (gethash :regexp (table rule-table))))

(defun apply-rules-once (rule-table source at-start)
  "Apply rules from rule-table once to source.
Returns two values: (1) prefix that has been processed, (2) suffix that has
still to be processed."
  (dolist (rule (get-rules rule-table (char source 0))
	   (values))
    (multiple-value-bind (prefix suffix)
	(try-rule rule source :at-start at-start)
      (if (stringp prefix)
	  (return (values prefix suffix))))))

(defun apply-rules (rule-table source &optional (at-start t))
  "Apply rules from rule-table to source."
  (multiple-value-bind (prefix suffix)
      (apply-rules-once rule-table source at-start)
    (log-rule (format nil "apply rules once: '~A' => '~A' '~A'~%"
		      source prefix suffix))
    (cond ((null prefix)
	     (if (= (length source) 1)
		 source
	       (concatenate 'string (subseq source 0 1)
			    (apply-rules rule-table (subseq source 1) nil))))
	  ((string= suffix "") prefix)
	  (t (concatenate 'string prefix
			  (apply-rules rule-table suffix nil))))))



;;;; ============================================================
;;;;
;;;; LEGACY (C PLUGIN) INTERFACE
;;;;

;; The following code realizes the interface of xindy 2.3, even though we
;; could now use real Lisp objects. This is a transient phase only.

(defvar *merge-rule-table*
  (make-ordrule-table))

;; By default, we have runs 0..7.
(defvar *sort-rule-tables*
  (let ((tables (make-array '(8) :element-type 'ordrule-table)))
    (dotimes (i 8)
      (setf (aref tables i) (make-ordrule-table)))
    tables))

(defun initialize (num)
  (setq *sort-rule-tables* (make-array (list num) :element-type 'ordrule-table))
  (dotimes (i num)
    (setf (aref *sort-rule-tables* i) (make-ordrule-table))))

(defun add-keyword-rule (rule-table rule repl again ruletype)
  (let* ((again (if (= again 1) t nil))
	 (rule-obj (ecase ruletype
		     (0 (make-ordrule rule repl :again again))
		     (1 (make-ordrule-string rule repl :again again))
		     (2 (make-ordrule-regexp rule repl :again again :extended nil))
		     (3 (make-ordrule-regexp rule repl :again again))
		     (4 (make-ordrule-char rule repl :again again)))))
    (add-rule rule-table rule-obj)))

(defun add-keyword-merge-rule (rule repl again ruletype)
  (let* ((rule-list (add-keyword-rule *merge-rule-table* rule
				     repl again ruletype))
	 (message (format nil "Add merge rule: ~S~%" (first (last rule-list)))))
    (log-rule message)))

(defun add-keyword-sort-rule (run rule repl again ruletype)
  (let* ((rule-list (add-keyword-rule (aref *sort-rule-tables* run)
				      rule repl again ruletype))
	 (message (format nil "Add sort rule to run ~A: ~S~%"
			  run (first (last rule-list)))))
    (log-rule message)))

(defun gen-keyword-mergekey (key)
  (let ((result (apply-rules *merge-rule-table* key)))
    (log-rule (format nil "Final merge-rule result: '~A' -> '~A'~%"
		      key result))
    result))

(defun gen-keyword-sortkey (key run)
  (let ((result (apply-rules (aref *sort-rule-tables* run) key)))
    (log-rule (format nil "Final sort-rule result, run ~A: '~A' -> '~A'~%"
		      run key result))
    result))


;;;
;;; Logging
;;;

(defvar *message-logging*
  nil
  "Boolean: Shall rule replacement be logged?")

(defvar *message-buffer*
  ""
  "String with the log message.
This is remaining cruft from the C plugin that could not use Lisp
output streams itself.")

(defvar *message-buffer-ptr*
  0
  "Current position in *message-buffer*.
This is remaining cruft from the C plugin that could not use Lisp
output streams itself.")

(defun log-rule (message)
  (and *message-logging*
       (if (= (incf *message-buffer-ptr*) 1)
	   (setq *message-buffer* message)
	 (setq *message-buffer* (concatenate 'string *message-buffer*
					     message)))
       ))



;;;======================================================================
;;
;; $Log$
