/*----------------------------------------------------------------------*\
    cef5ltx.cmd

    This file is part of the CJK macro package for LaTeX2e ver. 4.8.4

    This file is part of the CJK macro package for LaTeX2e ver. 4.8.4

    Copyright (C) 1994-2015  Wonkoo Kim <wkim+@pitt.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program in doc/COPYING; if not, write to the Free
    Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
    MA 02110-1301 USA

    Wonkoo Kim (wkim+@pitt.edu), September 2, 1996
\*----------------------------------------------------------------------*/

Call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

parse arg file

drive = filespec('drive', file)
path  = filespec('path', file)
fname = filespec('name', file)
m = lastpos('.', fname)
if (m = 0) then m = length(fname) + 1
cjkfile = drive||path||delstr(fname, m)||'.cjk'

'cef5conv <' file '>' cjkfile

ucjkfile = translate(cjkfile,'/','\')
if stream(cjkfile, 'C', 'QUERY EXISTS') \= '' then
    'call latex.cmd' ucjkfile

/* End of cef5ltx.cmd */
