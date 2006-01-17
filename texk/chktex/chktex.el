;;; chktex.el --- quick hack to add ChkTeX capability to AUC-TeX mode
;;
;;
;;  Keywords: LaTeX, AUC-TeX, ChkTeX
;;
;;  ChkTeX v1.5, quick hack to add ChkTeX in LaTeX menu.
;;  Copyright (C) 1995-96 Jens T. Berger Thielemann
;;
;;  This program is free software; you can redistribute it and/or modify
;;  it under the terms of the GNU General Public License as published by
;;  the Free Software Foundation; either version 2 of the License, or
;;  (at your option) any later version.
;;
;;  This program is distributed in the hope that it will be useful,
;;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;  GNU General Public License for more details.
;;
;;  You should have received a copy of the GNU General Public License
;;  along with this program; if not, write to the Free Software
;;  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;;
;;  Contact the author at:
;;              Jens Berger
;;              Spektrumvn. 4
;;              N-0666 Oslo
;;              Norway
;;              E-mail: <jensthi@ifi.uio.no>
;;
;;
;;; Commentary:
;;
;;  This tiny hack will add a "ChkTeX" item to the command menu of
;;  AUC-TeX, thus making the use of ChkTeX much more comfortable from
;;  within AUC-TeX (`LaTeX-mode').
;;
;;  INSTALLATION
;;
;;  1. Put this file somewhere in your loading path.
;;  2. Add the line
;;        (require 'chktex)
;;     to your .emacs file
;;
;;  It is doubtful whether byte-compiling this file gains anything.
;; 
;;; Code:
;; 



(require 'tex-site)
(require 'easymenu)

(setq TeX-command-list 
      (nconc TeX-command-list 
             (list (list "ChkTeX" "chktex -v3 %s" 'TeX-run-compile nil t))))

(add-hook 'LaTeX-mode-hook
	  (function (lambda ()
(easy-menu-define LaTeX-mode-menu
    LaTeX-mode-map
    "Menu used in LaTeX mode."
  (append '("Command")
          '(("Command on"
             [ "Master File" TeX-command-select-master
               :keys "C-c C-c" :style radio
               :selected (eq TeX-command-current 'TeX-command-master) ]
             [ "Buffer" TeX-command-select-buffer
               :keys "C-c C-b" :style radio
               :selected (eq TeX-command-current 'TeX-command-buffer) ]
             [ "Region" TeX-command-select-region
               :keys "C-c C-r" :style radio
               :selected (eq TeX-command-current 'TeX-command-region) ]))
          (let ((file 'TeX-command-on-current))
            (mapcar 'TeX-command-menu-entry TeX-command-list)))))))

(provide 'chktex)