;; Alternative spelling enterface for Emacs, contributed by Don Knuth
;; (Uses the wordtest and extex programs, which are
;;  supplied as examples with the CWEB distribution,
;;  available by anonymous ftp from ftp.cs.stanford.edu)

;; Based in part on "spell.el" from GNU Emacs; redistributed under
;; the terms of the GNU General Public License; NO WARRANTY implied.

;; To install this, using the default directories defined below,
;; install wordtest and extex in /usr/local/bin, then say
;; "ln /usr/local/bin/extex /usr/local/bin/excweb", then install
;; a suitable dictionary in /usr/local/lib/dict/words; one such
;; dictionary can be found in ~ftp/pub/dict/words at ftp.cs.stanford.edu.
;; Also create an empty file called .words in your home directory.
;; Finally, add (load-library "kspell") to your .emacs file, or
;; include such a line in site-init.el if kspell is to be used by everybody.
;; If you get a message like "Checking spelling of buffer...not correct"
;; and nothing else, the probable cause is that the wordtest program
;; could not open /usr/local/lib/dict/words or ~/.words for reading.

;; Note (10 Feb 1998): The spell-check logic failed to catch "obso1ete"
;; as a scanner-typo for "obsolete". The reason is that, although "obso"
;; and "ete" are non-words, they couldn't be found in query-replace
;; for the regexps "\bobso\b" and "\bete\b".

(provide 'kspell)

(defvar wordtest-command "wordtest" ;; maybe "/usr/local/bin/wordtest" better?
  "*Command to run the wordtest program; can include command-line options.")

;; "wordtest [options] [dictionaries] <infile >outfile" outputs all
;; lines of infile that don't appear in the dictionaries. The options
;; can define arbitrary character code mappings of 8-bit characters.
;; The default mapping takes a-z into A-Z, otherwise is ASCII.

(defvar wordtest-filter "extex" ;; maybe "/usr/local/bin/extex" is better?
  "*Command to run the filter needed by wordtest.")
(make-variable-buffer-local 'wordtest-filter)

;; The extex filter extracts words from its input and outputs them on
;; separate lines as required by wordtest. It removes TeX control
;; sequences except those used to make accents and special characters.
;; There's a companion filter excweb that also removes C code from CWEBs.
(setq cweb-mode-hook '(lambda () (setq wordtest-filter "excweb")))

(defvar wordtest-system-dictionary "/usr/local/lib/dict/words"
  "*Sorted dictionary containing all \"correct\" words,
including all variant forms obtained by prefix and suffix transformations.")
;; The standard UNIX dictionary /usr/dict/words is NOT satisfactory.

(defvar wordtest-personal-dictionary "~/.words"
  "*Default dictionary to supplement the words in the system dictionary.
If nil, no supplementary dictionary will be used.
This dictionary must be in alphabetic order as defined by wordtest.
Inserting any word with the + option to kspell-region will sort the file.")
(make-variable-buffer-local 'wordtest-personal-dictionary)

(defun set-personal-dictionary (filename)
  "Defines the supplementary personal dictionary for kspell to use in the
current buffer, overriding the default value of wordtest-personal-dictionary."
  (interactive "FPersonal dictionary file name: ")
  (setq wordtest-personal-dictionary filename))

(defun unset-personal-dictionary ()
  "Tells kspell not to use personal spelling dictionary with current buffer."
  (interactive)
  (setq wordtest-personal-dictionary nil))

(defun insert-into-personal-dictionary (word)
  "Put WORD into user's dictionary and sort that dictionary."
  (interactive "sword: ")
  (let ((xword (concat word "\n")))
    (if (null wordtest-personal-dictionary)
        (setq wordtest-personal-dictionary
              (read-string "Personal dictionary file name: " "~/.words")))
    (set-buffer (find-file-noselect wordtest-personal-dictionary))
    (goto-char (point-min))
    (insert xword)
    (call-process-region (point-min) (point-max) shell-file-name
                         t t nil "-c" wordtest-command)
    (search-backward xword (point-min) 1) ;; in case the user is watching
    (while (not (bolp)) (search-backward xword (point-min) 1))
    (save-buffer)))
    
(defun kspell-buffer ()
  "Check spelling of every word in the buffer.
For each incorrect word, you are asked for the correct spelling
and then put into a query-replace to fix some or all occurrences.

If you do not want to change a word, just give the same word
as its \"incorrect\" spelling; then the query replace is skipped.
Words are given in lowercase, but they will be Capitalized when
replacing Capitalized words, ALL_CAPS when replacing ALL_CAPS words.
If you type ? after a replacement, your correction will first be
looked up in the dictionary, and the query-replace will occur
only if the replacement is found. If you type + after a replacement,
your replacement will be inserted into the current personal dictionary.

You can leave the minibuffer to do some other editing and then come
back again to the query-replace loop by typing \\[other-window]."
  (interactive)
  (save-excursion (kspell-region (point-min) (point-max) "buffer")))

(defun kspell-word ()
  "Check spelling of the word at or before point.
If it is not correct, ask user for the correct spelling and
query-replace the entire buffer to substitute it as with kspell-buffer."
  (interactive)
  (let (beg end wordtest-filter)
    (save-excursion
     (if (not (looking-at "\\<"))
         (forward-word -1))
     (setq beg (point))
     (forward-word 1)
     (setq end (point))
     (kspell-region beg end (buffer-substring beg end)))))

(defun kspell-region (start end &optional description)
  "Like kspell-buffer but checks only words in the current region.
Used in a program, applies from START to END.
DESCRIPTION is an optional string naming the unit being checked:
for example, \"buffer\"."
  (interactive "r")
  (let (correct
        (filter wordtest-filter)
        (buf (get-buffer-create " *kspell*")) ;; hidden by list-buffers
        (dicts wordtest-system-dictionary))
    (if wordtest-personal-dictionary
        (setq dicts (concat dicts " " wordtest-personal-dictionary)))        
    (save-excursion
      (save-excursion
        (set-buffer buf)
        (widen)
        (erase-buffer))
      (message "Checking spelling of %s..." (or description "region"))
      (if (and (null filter)
               (< end (point-max))
               (= ?\n (char-after end)))
          (call-process-region start (1+ end) shell-file-name
                               nil buf nil "-c"
                               (concat wordtest-command " " dicts))
        (let ((oldbuf (current-buffer)))
          (save-excursion
            (set-buffer buf)
            (insert-buffer-substring oldbuf start end)
            (or (bolp) (insert ?\n))
            (if filter
                (call-process-region (point-min) (point-max) shell-file-name
                                     t t nil "-c" filter))
            (call-process-region (point-min) (point-max) shell-file-name
                                 t t nil "-c"
                                 (concat wordtest-command " " dicts)))))
      (setq correct (save-excursion (set-buffer buf) (= (buffer-size) 0)))
      (message "Checking spelling of %s...%scorrect"
               (or description "region")
               (if correct "" "not "))
      (if correct t
        (let (word newword qtext lastchar
                   (case-fold-search t)
                   (case-replace t))
          (while (save-excursion
                   (set-buffer buf)
                   (> (buffer-size) 0))
            (save-excursion
              (set-buffer buf)
              (goto-char (point-min))
              (setq word (downcase
                          (buffer-substring (point)
                                            (progn (end-of-line) (point)))))
              (forward-char 1) ;; pass the newline
              (delete-region (point-min) (point))
              (setq qtext (concat "\\b" (regexp-quote word) "\\b")))
            (goto-char (point-min))
            (setq lastchar nil)
            (if (re-search-forward qtext nil t)
                (while (null lastchar)
                  (setq newword
                        (read-string
                         (concat "edit a replacement for `" word "': ")
                         word))
                  (if (null newword) (setq lastchar 0)
                    (setq lastchar (string-to-char (substring newword -1)))
                    (if (memq lastchar '(?? ?+))
                        (setq newword (substring newword 0 -1))))
                  (cond ((= lastchar ??)
                         (cond ((or (string= word newword) (string= "" newword))
                                (describe-function 'kspell-buffer)
                                (setq lastchar nil))
                               ((not (kspelt-right newword))
                                (setq lastchar nil))))
                        ((= lastchar ?+)
                         (save-excursion
                           (insert-into-personal-dictionary newword))))
                  (cond ((string= word newword))
                        ((null lastchar))
                        (t
                         (goto-char (point-min))
                         (if (or (equal word newword) (null lastchar)) t
                           (query-replace-regexp qtext newword))))))))))))

(defun kspelt-right (word)
  "T if WORD is in the system dictionary or user's personal dictionary."
  (let ((buf (get-buffer-create " *temp*"))
        (pdict wordtest-personal-dictionary))
    (message "Checking spelling of %s..." word)
    (save-excursion
     (set-buffer buf)
     (widen)
     (erase-buffer)
     (insert word "\n")
     (if pdict
         (call-process-region (point-min) (point-max) shell-file-name
                              t t nil "-c"
                              (concat wordtest-command " "
                                      wordtest-system-dictionary " "
                                      pdict))
       (call-process-region (point-min) (point-max) shell-file-name
                            t t nil "-c"
                            (concat wordtest-command " "
                                    wordtest-system-dictionary)))
     (= 0 (buffer-size)))))

(defun kspell-string (string)
  "Check spelling of string supplied as argument."
  (interactive "sSpell string: ")
  (message "%s is %scorrect" string
           (if (kspelt-right string) "" "in")))

