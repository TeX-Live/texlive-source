;;; chktex.el --- quick hack to add ChkTeX capability to AUC-TeX mode
;;
;;
;;  Keywords: LaTeX, AUC-TeX, ChkTeX
;;
;;  ChkTeX, quick hack to add ChkTeX in LaTeX menu.
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
;;  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

(eval-after-load 'tex
  '(progn
     (add-to-list 'TeX-command-list
                  '("ChkTeX" "chktex -v6 %s" TeX-run-compile nil t))))

(provide 'chktex)

;;; chktex.el ends here
