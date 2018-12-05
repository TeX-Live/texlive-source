;; macros to go with oemacs

; suntool-map is the map for "\C-x*", the oemacs prefix for function keys
; commented-out lines agree with the settings in $(EMACS)/lisp/term/sun.el

(setq suntool-map-hooks '(			; not your usual hook list
; (define-key suntool-map "al" 'keyboard-quit)		; Stop
; (define-key suntool-map "bl" 'redraw-display)		; Again	L2
; (define-key suntool-map "b\M-l" 'repeat-complex-command); M-Again = C-X ESC
; (define-key repeat-complex-command-map "\C-x*b\M-l"
;                   'previous-complex-command)		; M-Again M-Again...
  (define-key suntool-map "bL" 'rerun-prev-command)	; AGAIN (no confirm)
; (define-key suntool-map "b\M-L" 'rerun-prev-command)	; M-AGAIN
; (define-key suntool-map "cl" 'buffer-menu)		; Props
; (define-key suntool-map "dl" 'undo)			; Undo
; (define-key suntool-map "el" 'ignore-key)		; Front
; (define-key suntool-map "fl" 'sun-select-region)	; Copy
; (define-key suntool-map "f," 'copy-region-as-kill)	; C-Copy
; (define-key suntool-map "gl" 'ignore-key)		; Open
; (define-key suntool-map "hl" 'sun-yank-selection)	; Paste
; (define-key suntool-map "h," 'yank)			; C-Paste
; (define-key suntool-map "il" 'research-forward)	; Find (with default)
; (define-key suntool-map "i\M-l" 'research-backward)	; M-Find
; (define-key suntool-map "iL" 'isearch-forward-regexp)	; FIND (incremental)
; (define-key suntool-map "i\M-L" 'isearch-backward-regexp); M-FIND
; (define-key suntool-map "i," 're-search-forward)	; C-Find (ordinary)
; (define-key suntool-map "i\M-," 're-search-backward)	; C-M-Find
  (define-key suntool-map "jl" 'sun-cut-region)		; Cut
  (define-key suntool-map "j," 'kill-region-and-unmark) ; C-Cut
  (define-key suntool-map "jL" 'pop-the-mark)		; CUT
; (define-key suntool-map "j\M-l" 'exchange-point-and-mark); M-Cut

  (define-key suntool-map "at" 'bury-buffer)			; F1
; (define-key suntool-map "bt" 'toggle-selective-display) 	; F2
; (define-key suntool-map "ct" 'scroll-down-in-place)		; F3
; (define-key suntool-map "cT" '(lambda(n) (interactive "p") (scroll-down n)))
; (define-key suntool-map "dt" 'scroll-up-in-place)		; F4
; (define-key suntool-map "dT" '(lambda(n) (interactive "p") (scroll-up n)))
  (define-key suntool-map "et" 'jumpup)				; F5
; (define-key suntool-map "ft" 'shrink-window)			; F6
; (define-key suntool-map "fT" 'shrink-window-horizontally)	; Shift-F6
; (define-key suntool-map "gt" 'enlarge-window)			; F7
; (define-key suntool-map "gT" 'enlarge-window-horizontally)	; Shift-F7
  (define-key suntool-map "ht" 'tags-search)			; F8
  (define-key suntool-map "it" 'tags-query-replace)		; F9
  (define-key suntool-map "jt" 'narrow-to-region)		; F10
  (define-key suntool-map "j\M-t" 'narrow-to-page)		; M-F10
  (define-key suntool-map "kt" 'revert-buffer)  		; F11
  (define-key suntool-map "k\M-t" 'quick-revert-other-buffer)	; M-F11
  (define-key suntool-map "lt" 'goto-line)			; F12
  (define-key suntool-map "l\M-t" 'goto-line)			; M-F12

  (define-key suntool-map "ar" 'redraw-display)			; Pause, R1
  (define-key suntool-map "br" 'call-secondlast-kbd-macro)	; PrSc, R2
  (define-key suntool-map "cr" 'ignore-key)		; Scroll Lock Break, R3
  (define-key suntool-map "dr" 'unbound-key)			; KP=, R4
  (define-key suntool-map "d\M-r" 'bind-last-kbd-macro-to-KP=)	; M-KP=, M-R4
  (define-key suntool-map "er" 'unbound-key)			; KP/, R5
  (define-key suntool-map "e\M-r" 'bind-last-kbd-macro-to-KP/)	; M-KP/, M-R5
  (define-key suntool-map "fr" 'unbound-key)			; KP*, R6
  (define-key suntool-map "f\M-r" 'bind-last-kbd-macro-to-KP*)	; M-KP*, M-R6
; (define-key suntool-map "gr" 'beginning-of-buffer)		; Home, R7

; (define-key suntool-map "hr" 'previous-line)			; Up, R8
; (define-key suntool-map "ir" 'scroll-down)			; PgUp, R9
; (define-key suntool-map "iR" 'backward-page)			; Shift-PGUP
; (define-key suntool-map "jr" 'backward-char)			; Lft, R10
  (define-key suntool-map "kr" "\C-u\C-l")			; KP5, R11
; (define-key suntool-map "lr" 'forward-char)			; Rt, R12
; (define-key suntool-map "mr" 'end-of-buffer)			; End, R13
; (define-key suntool-map "nr" 'next-line)			; Dn, R14
; (define-key suntool-map "or" 'scroll-up)			; PgDn, R15
; (define-key suntool-map "oR" 'forward-page)			; Shift-PGUP

  (define-key suntool-map "ab" 'apropos) 		; Help
  (define-key suntool-map "a\M-b" 'mouse-help)	 	; M-Help
  (define-key suntool-map "aB" 'describe-bindings) 	; HELP
  (define-key suntool-map "a\M-B" 'describe-mouse-bindings); M-HELP
  (define-key suntool-map "a\"" 'command-apropos)	; C-Help
  (define-key suntool-map "a\C-B" 'apropos)		; C-HELP
  (define-key suntool-map "a\M-\C-B" 'help-for-dummies)	; M-C-HELP
  (define-key suntool-map "bb" 'ignore-key)		; Alt
  (define-key suntool-map "cb" 'ignore-key)		; AltGraph
  (define-key suntool-map "db" 'auto-fill-mode)		; Keypad Ins
  (define-key suntool-map "eb" 'overwrite-mode)		; Keypad Del
  (define-key suntool-map "fb" 'call-last-kbd-macro)	; Keypad Enter
  (define-key suntool-map "gb" 'forward-paragraph)	; Keypad +
  (define-key suntool-map "hb" 'backward-paragraph)	; Keypad -
  ))


; redefinition of emacs functions
(defun end-of-buffer ()
  "Move point to the end of the buffer; leave a mark at previous position.
Scroll so that point is at the bottom of the window, if possible."
  (interactive)
  (push-mark)
  (goto-char (point-max))
  (recenter -1))

; new functions
(defun jumpup ()
  "Scroll so that point is at the top of the window."
  (interactive)(recenter 0))

(defun quick-revert-buffer ()
  "Revert the buffer without asking for confirmation."
  (interactive)
  (revert-buffer t t))

(defun quick-revert-other-buffer (n)
  "Revert the Nth other buffer without asking for confirmation."
  (interactive "p")
  (other-window n)
  (revert-buffer t t)
  (other-window (- n)))

(defun bind-last-kbd-macro-to-KP= ()
  "Assign the last keyboard macro to the = key on the numeric keypad."
  (interactive)
  (define-key suntool-map "dr" last-kbd-macro))

(defun bind-last-kbd-macro-to-KP/ ()
  "Assign the last keyboard macro to the / key on the numeric keypad."
  (interactive)
  (define-key suntool-map "er" last-kbd-macro))

(defun bind-last-kbd-macro-to-KP* ()
  "Assign the last keyboard macro to the * key on the numeric keypad."
  (interactive)
  (define-key suntool-map "fr" last-kbd-macro))

(defvar secondlast-kbd-macro nil)
(defun pre-end-kbd-macro ()
  "Preserve last-kbd-macro as secondlast-kbd-macro, then end-kbd-macro."
  (interactive)
  (setq secondlast-kbd-macro last-kbd-macro)
  (end-kbd-macro))
(substitute-key-definition 'end-kbd-macro 'pre-end-kbd-macro global-map)
(substitute-key-definition 'end-kbd-macro 'pre-end-kbd-macro esc-map)
(substitute-key-definition 'end-kbd-macro 'pre-end-kbd-macro ctl-x-map)

(defun call-secondlast-kbd-macro (n)
  "Like call-last-kbd-macro, but uses the macro defined before that."
  (interactive "p")
  (let ((last-kbd-macro secondlast-kbd-macro))
    (call-last-kbd-macro n)))

; The turn-numlock-on/off commands are created automatically by oemacs
; when the NumLock light changes state.
(defun turn-numlock-on () ; numlock is always off when oemacs starts
  "Bind keys in keypad area to numeric interpretations."
  (interactive)
  (setq hb-binding (lookup-key suntool-map "hb"))
  (setq gb-binding (lookup-key suntool-map "gb"))
  (setq fb-binding (lookup-key suntool-map "fb"))
  (setq eb-binding (lookup-key suntool-map "eb"))
  (setq db-binding (lookup-key suntool-map "db"))
  (define-key suntool-map "hb" "-")
  (define-key suntool-map "gb" "+")
  (define-key suntool-map "fb" "\C-j")
  (define-key suntool-map "eb" ".")
  (define-key suntool-map "db" "0"))
(defun turn-numlock-off ()
  "Rebind keys in keypad area to function-key interpretations."
  (interactive)
  (define-key suntool-map "hb" hb-binding)
  (define-key suntool-map "gb" gb-binding)
  (define-key suntool-map "fb" fb-binding)
  (define-key suntool-map "eb" eb-binding)
  (define-key suntool-map "db" db-binding))
(defvar hb-binding nil)
(defvar gb-binding nil)
(defvar fb-binding nil)
(defvar eb-binding nil)
(defvar db-binding nil)
