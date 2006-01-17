;; This file contains extensions to GNU-Emacs, to wit:
; (1) some WEB-oriented functions that are also of general use
; (2) changes to the GNU-distributed TeX mode
; (3) definitions of simple WEB and CWEB modes

; To use: Put this in your EMACS-lisp library and say (load-library "cweb")
; in your .emacs init file.

; Contributed by Don Knuth, July 1990

;; OK, here's part (1): some WEB-oriented functions whose main purpose is
; to maintain a stack of module names that are "pending" as you are writing
; a program. When you first think of a module that needs to be written later,
; put it into the pending list (by typing CTL-Z instead of @> after the
; name). Later you can say CTL-\ to retrieve a pending name (and if
; you want to cycle through the pending names, ESC-y after CTL-\ will
; do it, just as ESC-y works after a yank).
; After you've said CTL-\, the current region is the name just removed from
; the pending list. If you change your mind, you can put it back again by
; saying ESC-\. If you had put it into the pending list by mistake, you
; can get rid of it by using the normal CTL-W operation (kill-region).
; The following code binds the new commands to CTL-Z, CTL-\, and ESC-\
; in all modes. You may prefer other bindings, of course.
; CTL-Z is normally "suspend emacs", but it is best not used when emacs
; has its own window as it usually does nowadays; if you need the
; old CTL-Z, you might rather bind it to CTL-X CTL-Z.
; CTL-\ is normally undefined.
; ESC-\ is normally "delete space", but ESC-space DEL does that easily too.

(defvar pending-list nil
 "List of strings (usually WEB module names) still pending.")

(defun into-pending-list (beg end)
 "Copy region into pending-list."
 (interactive "r")
 (indicate-region)
 (setq pending-list (cons (buffer-substring beg end) pending-list)))

(defun new-module-name-pending ()
 "Insert @> to complete a module name, then put it into pending-list."
 (interactive)
 (insert "@>")
 (push-mark)
 (if (search-backward "@<" nil t)
     (progn
       (exchange-point-and-mark)
       (into-pending-list (point) (mark))
       )
   (message "There's no @< to begin the module name!")))
(global-set-key "\C-z" 'new-module-name-pending)

(defun pop-pending-list (arg)
 "Remove first element of pending-list and insert it as current region.
With argument, put point at left; otherwise point will follow the insertion.
Say \\[new-yank-pop] to replace this by another element of the list.
Say \\[into-pending-list] to put it back in the list."
 (interactive "*P")
 (if (consp pending-list)
     (progn
       (push-mark (point))
       (insert (car pending-list))
       (setq pending-list (cdr pending-list))
       (if arg
           (exchange-point-and-mark)))
   (message "Nothing is pending.")
   (setq this-command nil)))
(global-set-key "\C-\\" 'pop-pending-list)
(global-set-key "\M-\\" 'into-pending-list)

(defun new-yank-pop (arg)
 "If previous command was \\[pop-pending-list], pop a different string;
otherwise do an ordinary Meta-y."
 (interactive "*p")
 (if (eq last-command 'pop-pending-list)
     (let (xch)
       (setq xch (< (point) (mark)))
       (setq pending-list (append pending-list
                                 (list (buffer-substring (point) (mark)))))
       (delete-region (point) (mark))
       (setq this-command 'pop-pending-list)
       (pop-pending-list xch))
   (yank-pop arg)))
(global-set-key "\M-y" 'new-yank-pop)

(defun indicate-region ()
  "Bounce cursor to mark and back again"
  (let ((point-save (point)))
    (unwind-protect
        (progn (goto-char (mark))
               (sit-for 0 300)) ;; wait 300 milliseconds
      (goto-char point-save))))

; I prefer to change the standard copy-region command to the following,
; which gives me visual feedback about what I've copied to the kill ring:
(defun indicate-and-copy-region (beg end)
  "Indicate current region, then copy it to the kill ring."
  (interactive "r")(indicate-region)(copy-region-as-kill beg end))
(global-set-key "\M-w" 'indicate-and-copy-region)

; Here's another convenient command, bound to the usually unused ESC-".
(defun ditto (arg)
  "Copy ARG characters from the line above."
  (interactive "*p")
  (let (ch)
    (while (> arg 0)
      (setq temporary-goal-column (current-column))
      (save-excursion
        (previous-line 1)
        (setq ch (following-char)))
      (insert ch)
      (setq arg (1- arg)))))
(global-set-key "\M-\"" 'ditto)

;; OK, here's part (2): Changes to TeX mode.
; The WEB modes below are very much like TeX mode, but some improvements were
; desirable in TeX mode:
; I made newline act as it does in indented-text mode, since this
; works nicely for both TeX and WEB (Pascal or C code).
; I made RET check for unmatched delimiters if it ends a paragraph.
; Otherwise TeX mode remains as it was before.

(setq TeX-mode-map (make-sparse-keymap))
(define-key TeX-mode-map "\C-c\C-k" 'TeX-kill-job)
(define-key TeX-mode-map "\C-c\C-l" 'TeX-recenter-output-buffer)
(define-key TeX-mode-map "\C-c\C-q" 'TeX-show-print-queue)
(define-key TeX-mode-map "\C-c\C-p" 'TeX-print)
(define-key TeX-mode-map "\"" 'TeX-insert-quote)
(define-key TeX-mode-map "\e}" 'up-list)
(define-key TeX-mode-map "\e{" 'TeX-insert-braces)
(define-key TeX-mode-map "\C-c\C-r" 'TeX-region)
(define-key TeX-mode-map "\C-c\C-b" 'TeX-buffer)
(define-key TeX-mode-map "\C-c\C-f" 'TeX-close-LaTeX-block)
(define-key TeX-mode-map "\r" 'TeX-newline)
(define-key TeX-mode-map "\t" 'indent-relative)
(setq TeX-mode-hook '(lambda ()
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'indent-relative-maybe)))

(defun TeX-newline (arg)
"If previous character is newline and no ARG, check for unbalanced braces
and/or dollar signs in previous paragraph. If ARG is \\[universal-argument],
do a single newline; otherwise do ordinary newline."
 (interactive "*P")
 (if (and (eq (preceding-char) ?\n) (not arg))
     (TeX-check-paragraph)
   (if (listp arg)
       (newline)
     (newline arg))))

(defun TeX-check-paragraph ()
"Insert a newline following a newline, breaking a paragraph for TeX.
Check for mismatched delimiters in paragraph being terminated."
  (interactive)
  (if (TeX-validate-paragraph
           (save-excursion
             (search-backward "\n\n" nil 'move)
             (point))
           (point))
      (insert ?\n)
    (insert ?\n)
    (error "Mismatched delimiters in that paragraph?")))

;; and now, part (3): WEB and CWEB modes.
; These are like plain TeX mode except that the automatic conversion of
; " to `` or '' is disabled. (Personally I never liked that feature anyway,
; since it's easy to get used to typing `` and ''. In WEB modes, the
; feature soon becomes intolerable, unless you never use string constants!)
; Another thing distinguishing WEB mode from TeX is ESC-p and ESC-n, to
; move to previous or next module. These keys are usually unbound, except
; when processing email.

(defun forward-module (arg)
"Advance past next WEB module beginning; with ARG, repeat ARG times."
 (interactive "p")
 (move-to-module arg))
(defun backward-module (arg)
"Advance to previous WEB module beginning; with ARG, repeat ARG times."
 (interactive "p")
 (move-to-module (- arg)))
(defun move-to-module (arg)
 (while (> arg 0)
   (re-search-forward "@ \\|@\\*\\|@\n")
   (setq arg (1- arg)))
 (while (< arg 0)
   (re-search-backward "@ \\|@\\*\\|@\n")
   (setq arg (1+ arg))))

(defun web-mode ()
  "Major mode like TeX mode plus \\[forward-module] and \\[backward-module]
for relative module movement. The automatic \" feature is disabled."
  (interactive)
  (plain-tex-mode)
  (local-set-key "\M-n" 'forward-module)
  (local-set-key "\M-p" 'backward-module)
  (local-set-key "\"" 'self-insert-command)
  (setq mode-name "WEB")
  (setq major-mode 'web-mode)
  (run-hooks 'web-mode-hook))
(setq auto-mode-alist (cons '("\\.web$" . web-mode) auto-mode-alist))

(defun cweb-mode ()
  "Major mode like TeX mode plus \\[forward-module] and \\[backward-module]
for relative module movement. The automatic \" feature is disabled."
  (interactive)
  (plain-tex-mode)
  (local-set-key "\M-n" 'forward-module)
  (local-set-key "\M-p" 'backward-module)
  (local-set-key "\"" 'self-insert-command)
  (setq comment-start nil)
  (modify-syntax-entry ?% "@")
  (setq mode-name "CWEB")
  (setq major-mode 'cweb-mode)
  (run-hooks 'cweb-mode-hook))
(setq auto-mode-alist (cons '("\\.w$" . cweb-mode) auto-mode-alist))
(setq auto-mode-alist (cons '("\\.ch$" . cweb-mode) auto-mode-alist))
