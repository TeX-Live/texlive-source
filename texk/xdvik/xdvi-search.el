;;; xdvi-search.el --- (X)Emacs frontend for forward search with xdvi(k)
;;; Requires xdvi(k) >= 22.38.
;;;
;;; See http://xdvi.sourceforge.net/inverse-search.html and the section
;;; `FORWARD SEARCH' in the xdvi man page for more information on forward search.
;;;
;;; This file is available from: http://xdvi.sourceforge.net/xdvi-search.el
;;;
;;; Copyright (C) 2004 - 2006 Stefan Ulrich
;;; Copyright (C) 2006, Chris Stucchio (minor modifications)
;;; $Revision: 1.37 $, tested with Emacs 20.4 to 21.2 and Xemacs 21.1 to 21.5
;;;
;;; This program is free software; you can redistribute it and/or
;;; modify it under the terms of the GNU General Public License
;;; as published by the Free Software Foundation; either version 2
;;; of the License, or (at your option) any later version.
;;; 
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;; 
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;;;
;;;
;;; Commentary:
;; 
;; Usage:
;;
;; - Add this file to some place where Emacs can find it
;;   (e.g. put it into a directory ~/emacs/, which you add to your
;;   load path by putting the following line into your .emacs file:
;;   
;;      (add-to-list 'load-path (expand-file-name "~/emacs/"))
;;   
;;   Then, add the following line to your .emacs file:
;;   
;;      (require 'xdvi-search)
;;   
;;   After compiling your .tex file with source specials activated, you should
;;   be able to use
;;      M-x xdvi-jump-to-line
;;   to make xdvi jump to the current location of the cursor.
;;   
;;   You could also bind this command to a key, e.g. by adding
;;   a binding to tex-mode-hook:
;;   
;;      (add-hook 'tex-mode-hook (lambda ()
;;                                 (local-set-key "\C-c\C-j" 'xdvi-jump-to-line)))
;;
;;   
;;   (without AucTeX; use LaTeX-mode-hook instead of tex-mode-hook with AucTeX.
;;   Note that with AucTeX, C-c C-j is already bound to LaTeX-insert-item, so
;;   you might want to use a different key combination instead.)
;;
;; - Note that with xdvik < 22.74.2, you may need to use one of the
;;   functions xdvi-jump-to-line-fullpath() or xdvi-jump-to-line-relpath()
;;   instead of xdvi-jump-to-line(), depending on the type of paths contained
;;   in your \include{} commands.  See the docstring of xdvi-jump-to-line()
;;   for more information.
;;
;; Please send bug reports, improvements etc.  to
;; <stefanulrich@users.sourceforge.net>.
;;


;;; Code:

;;;
;;; Customizable stuff
;;;

(defgroup xdvi-search nil
  "Support for inverse search with (La)TeX and DVI previewers."
  :group 'languages)

(defcustom explicit-shell-file-name nil
  "*If non-nil, file name to use for explicitly requested inferior shell."
  :type '(choice (const :tag "nil" nil) file)
  :group 'xdvi-search)

(defcustom xdvi-shell-buffer-name nil
  "*If non-nil, file name to use for explicitly requested inferior shell."
  :type '(choice (const :tag "nil" nil) file)
  :group 'xdvi-search)


(defcustom xdvi-bin "xdvi"
  "*Name of the xdvi executable."
  :type '(choice (const "xdvi") file)
  :group 'xdvi-search)

(defcustom xdvi-logfile "~/.xdvi-log"
  "*Write xdvi output to this file, or throw away output if set to nil."
  :type '(choice (const :tag "nil" nil) file)
  :group 'xdvi-search)


;;;
;;; global variables/constants
;;;
(defvar xdvi-version-number 0
  "*Version of xdvi(k) you're using.
Only set this to override the built-in version recognition if that fails.
The version number is computed as: major * 1000000 + minor * 1000 + patchlevel,
where alphabetic characters are ignored; e.g. 22040000 equals version 22.40c;
 22077002 equals version 22.77.2.")

(defvar xdvi-is-xdvik nil
  "Whether we're really using xdvik instead of xdvi.")

;;;
;;; exported functions
;;;
;;;###autoload
(defun xdvi-jump-to-line-fullpath ()
  "See `xdvi-jump-to-line' for documentation."
  (interactive)
  (xdvi-jump-to-line 3))

;;;###autoload
(defun xdvi-jump-to-line-relpath ()
  "See `xdvi-jump-to-line' for documentation."
  (interactive)
  (xdvi-jump-to-line 2))

;;;###autoload
(defun xdvi-jump-to-line (flag)
  "Call xdvi to perform a `forward search' for current file and line number.
The optional argument FLAG controls the path searching behaviour; see also
'xdvi-jump-to-line-fullpath' and 'xdvi-jump-to-line-relpath'.

Xdvi needs three pieces of information for forward search:

- the current line number
- the name of the current input file
- the name of the master .tex file.

The `master' .tex file (following AucTeX's terminology) is the same
as `\jobname' and determines the name of the DVI file.  It is either
obtained by using AucTeX's function `TeX-master-file' or, in case
AucTeX isn't available, by calling `xdvi-master-file-name', a function
which tries to mimick AucTeX's behaviour to obtain this filename.

For the current input file name, we need to rely on the DVI viewer to
make a smart matching of path suffixes with the file names that
actually occur in the specials, since specials inserted by TeX's
`-src' option may contain absolute or relative paths, depending on
whether TeX is called with a relative or absolute filename.  (Similarly
for srcltx.sty, which can't expand paths.)

Note to users of older versions of xdvik (< 22.74.2), or non-k xdvi:

There are various ways of specifying an input file path for LaTeX's
\\input{} commands (which will end up in the source specials):

   (1) Filename only: \\input{chapter1}
   (2) Relative path name: \\input{chapters/chapter1}
   (3) Full path name: \\input{/home/user/tex/diss/chapters/chapter1}

Versions of xdvik < 22.74.2 and plain xdvi will not match pathname suffixes.
With these versions, `xdvi-jump-to-line' will only work with variant (1);
for (2) or (3) you will need to use one of `xdvi-jump-to-line-relpath' (for 2)
or `xdvi-jump-to-line-fullpath' (for 3) instead."
  (interactive "P")
  (save-excursion
    (save-restriction
      (widen)
      (beginning-of-line 1)
      (if (not flag)
	  (setq flag 1))
      (let* (;;; Current line in file.
	     ;;; `count-lines' yields different results at beginning and in the
             ;;; middle of a line, so we normalize by going to BOL first and
             ;;; then adding 1
	     (curr-line (+ 1 (count-lines (point-min) (point))))
	     ;;; name of master .tex file, to be used as DVI basename:
	     (abspath (file-name-directory buffer-file-name))
	     (master-file (if (fboundp 'TeX-master-file)
			      (TeX-master-file t)
			    (xdvi-get-masterfile (xdvi-master-file-name))))
	     (xdvi-version-info (xdvi-get-version-number xdvi-version-number))
	     ;;; DVI file name:
	     (dvi-file (concat (file-name-sans-extension master-file) ".dvi"))
	     ;;; Current source filename
	     (filename (get-source-filename (buffer-file-name) flag)))
	(if (not (xdvi-has-inverse-search (car xdvi-version-info)))
	    (error "Your xdvi version is too old; see http://xdvi.sourceforge.net/inverse-search.html for more information"))
	(if (not (file-exists-p dvi-file))
	    (message "File %s does not exist; maybe you need to run latex first?" dvi-file)
	  (save-window-excursion
	    (if (xdvi-has-nofork (car xdvi-version-info) (car (cdr xdvi-version-info)))
		(progn
		  (setq proc
			(start-process "xdvi" "*xdvi output*"
				       xdvi-bin
				       "-nofork"
				       "-sourceposition"
				       (concat (int-to-string curr-line) " " filename)
				       dvi-file))
		  (set-process-sentinel proc 'xdvi-process-sentinel))
	      ;;;
	      ;;; More complicated if we don't have nofork: start a
	      ;;; subshell, since otherwise there's no way of viewing
	      ;;; the stderr/stdout output of the xdvi child process
	      ;;; without having either xdvi freezing, Emacs freezing,
	      ;;; or Emacs killing the xdvi child process (which is
	      ;;; forked when running with -sourceposition).
	      ;;;
	      ;;; Obtain the type of I/O-redirection needed for current shell:
	      (let* ((default-shell
		       (or (and (boundp 'explicit-shell-file-name) explicit-shell-file-name)
			   (getenv "ESHELL")
			   (getenv "SHELL")
			   "/bin/sh"))
		     (shell-redirection
		      (cond ((string-match "/bin/t?csh" default-shell)
			     (list ">&" ""))
			    ;; bash/ksh/sh
			    ((list ">" "2>&1")))))
		(cond ((string-match "XEmacs" emacs-version)
		       (xdvi-shell))
		      ((< emacs-major-version 21)
		       (xdvi-shell))
		      ((shell (or xdvi-shell-buffer-name "*xdvi-shell*"))))
		(comint-send-input)
		(insert (concat "cd " abspath ";") )
		(insert xdvi-bin
			" -sourceposition '" (int-to-string curr-line) " " filename "' "
			dvi-file
			" "
		      ;;; we could probably do without the logfile, but it's an easy
		      ;;; way for toggling between /dev/null and logging.
			(if xdvi-logfile
			    (let ((xdvi-logfile-fullname (expand-file-name xdvi-logfile)))
			      ;;; to avoid problems with `noclobber'
			      (if (file-exists-p xdvi-logfile-fullname)
				  (delete-file xdvi-logfile-fullname))
			      (car shell-redirection)
			      " "
			      xdvi-logfile-fullname
			      " "
			      (car (cdr shell-redirection))
			      )
			  )
			" &")
		(if xdvi-logfile
		    (message "Writing xdvi output to \"%s\"." xdvi-logfile))
		(comint-send-input)))))))))

;;;
;;; internal functions
;;;
(defun shell-with-name (name)
  "This function opens a shell in a buffer with name given by the argument, and switches to that buffer. If the buffer already exists, we simply switch to it."
  (let ((buffer-of-name (get-buffer name)) 
	(tempname (generate-new-buffer-name "*tempshell*") ) )
    (cond ((bufferp buffer-of-name) ;If the buffer exists, switch to it (assume it is a shell)
	   (switch-to-buffer name))
	  ( (bufferp (get-buffer "*shell*"))
	  (progn
	    (shell)
	    (rename-buffer tempname)
	    (shell)
	    (rename-buffer name)
	    (switch-to-buffer tempname)
	    (rename-buffer "*shell*")
	    (switch-to-buffer name)))
	  ( t
	    (progn
	      (shell)
	      (rename-buffer name)
	      )
	    )
	  )
    )
  )

(defun xdvi-shell () 
  "Behaves just like function shell, but creates the shell in buffer named *xdvi-shell* (unless of course the user chooses to modify the xdvi-shell-buffer-name variable.)"
  (shell-with-name (or xdvi-shell-buffer-name "*xdvi-shell*") )
  )

(defun xdvi-has-inverse-search (version)
  "Return True if inverse search is available for this VERSION of xdvi(k)."
  (>= version 22038000))

(defun xdvi-has-nofork (version is-xdvik)
  "Return True if the '-nofork' option is available for this VERSION of xdvi(k).
This option was implemented in version 22.74.1 of xdvik, and 22.61 of plain xdvi.
Argument IS-XDVIK controls whether we're running xdvik or plain xdvi."
  (if is-xdvik
      (>= version 22074001)
    (>= version 22061000)))

(defun get-source-filename (fname flag)
  "Helper function to extract the file name from the file name FNAME.
FLAG controls whether to use the full path or a relative path."
  (if (> flag 1)
      (if (> flag 2)
	  ;;; full path name, for use with TeX patch and when using the
	  ;;; full path in \input or \include with srcltx.sty:
	  (expand-file-name fname)
	  ;;; relative path name: if current path and path of master file match partly,
	  ;;; use the rest of this path; else use buffer name. This can treat both the
	  ;;; cases when a relative path is used in \input{}, or when just the filename
	  ;;; is used and the file is located in the current directory (suggested by
	  ;;; frisk@isy.liu.se).
	(if (string-match (concat "^" (regexp-quote (file-name-directory master-file)))
			  fname)
	    (substring fname (match-end 0))
	  fname))
    ;;; buffer file name without path:
    (file-name-nondirectory fname)))

(defun xdvi-get-version-number (default)
  "Return a tuple of (version-number xdvi-is-xdvik), or (DEFAULT nil) if DEFAULT != 0."
  (if (not (= default 0))
      (list default nil)
    (save-window-excursion
      (switch-to-buffer "*xdvi-version*")
      (erase-buffer)
      (call-process xdvi-bin nil "*xdvi-version*" nil "-version")
      (goto-char (point-min))
      (if (not (re-search-forward "xdvi\\((?k)?\\)? version \\([0-9]+\\)\\.\\([0-9]+\\)\\(\\.\\([0-9]+\\)\\)?" nil t))
	  (error "Unable to get xdvi version number - please check value of `xdvi-bin', or set it manually via `xdvi-version-number'")
	(let* ((major (string-to-int (buffer-substring (match-beginning 2) (match-end 2))))
	       (minor (string-to-int (buffer-substring (match-beginning 3) (match-end 3))))
	       (patchlevel (if (match-beginning 5)
			       (string-to-int (buffer-substring (match-beginning 5) (match-end 5)))
			     0))
	       (xdvi-is-xdvik (if (match-beginning 1) t nil))
	       (version (+ (* 1000 minor) (* 1000000 major) patchlevel)))
	  (kill-buffer "*xdvi-version*")
;;;	  (message "Xdvi%s version: major %d, minor %d, version %d" (if xdvi-is-xdvik "k" "") major minor version)
	  (list version xdvi-is-xdvik)
	  )))))

(defun xdvi-process-sentinel (process signal)
  "Return a readable error message for subprocess PROCESS which has terminated with SIGNAL."
  (let ((retmsg (substring signal 0 -1)))
    (if (string-match "abnormally" retmsg)
	(progn
	  (message "XDvi: %s" retmsg)
	  (pop-to-buffer (process-buffer process)))
      (message "XDvi: %s" retmsg))))

(defun xdvi-get-masterfile (file)
  "Small helper function for AucTeX compatibility.
Converts the special value t that TeX-master might be set to (Argument FILE)
into a real file name."
  (if (eq file t)
      (buffer-file-name)
    file))


(defun xdvi-master-file-name ()
  "Emulate AucTeX's TeX-master-file function.
Partly copied from tex.el's TeX-master-file and TeX-add-local-master."
  (if (boundp 'TeX-master)
      TeX-master
    (let ((master-file (read-file-name "Master file (default this file): ")))
      (if (y-or-n-p "Save info as local variable? ")
	  (progn
	    (goto-char (point-max))
	    (if (re-search-backward "^\\([^\n]+\\)Local Variables:" nil t)
		(let* ((prefix (if (match-beginning 1)
				   (buffer-substring (match-beginning 1) (match-end 1))
				 ""))
		       (start (point)))
		  (re-search-forward (regexp-quote (concat prefix "End:")) nil t)
		  (if (re-search-backward (regexp-quote (concat prefix "TeX-master")) start t)
		      ;;; if TeX-master line exists already, replace it
		      (progn
			(beginning-of-line 1)
			(kill-line 1))
		    (beginning-of-line 1))
		  (insert prefix "TeX-master: " (prin1-to-string master-file) "\n"))
	      (insert "\n%%% Local Variables: "
;;; mode is of little use without AucTeX ...
;;;		      "\n%%% mode: " (substring (symbol-name major-mode) 0 -5)
		      "\n%%% TeX-master: " (prin1-to-string master-file)
		      "\n%%% End: \n"))
	    (save-buffer)
	    (message "(local variables written.)"))
	(message "(nothing written.)"))
      (set (make-local-variable 'TeX-master) master-file))))


(provide 'xdvi-search)

;;; page break to avoid "Local variables entry is missing the prefix" error for previous stuff


;;; xdvi-search.el ends here
