#!/usr/bin/env texlua  

VERSION = "0.3"

--[[
     musixtex.lua: processes a MusiXTeX file and then deletes intermediate files

     (c) Copyright 2011 Bob Tennent rdt@cs.queensu.ca

     This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

--]]

--[[

  ChangeLog:

     version 0.3  2011-04-25 RDT
       Add -d (dvipdfm)  and -s (stop at dvi) options.

     version 0.2  2011-04-21 RDT
       Allow basename.tex as filename.
       Add -p option for pdfetex processing.
       Add standard -v -h options.

--]]

function usage()
  print("Usage:  [texlua] musixtex.lua [option] [basename[.tex]]")
  print("options: -v  version")
  print("         -h  help")
  print("         -p  pdfetex")
  print("         -d  dvipdfm")
  print("         -s  stop at dvi")
end

function whoami ()
  print("This is musixtex.lua version ".. VERSION .. ".")
end

if #arg == 0 then
  whoami()
  usage()
  os.exit(0)
end

narg = 1

if arg[narg] == "-v" then
  whoami()
  os.exit(0)
elseif arg[narg] == "-h" then
  usage()
  os.exit(0)
end

-- defaults:
tex = "etex"  
musixflx = "musixflx"
dvi = "dvips -q"
ps2pdf = "ps2pdf"

if narg < #arg then
  if arg[narg] == "-p" then
    tex = "pdfetex"; dvi = ""; ps2pdf = ""
  elseif arg[narg] == "-d" then
    tex = "etex"; dvi = "dvipdfm"; ps2pdf = ""
  elseif arg[narg] == "-s" then
    tex = "etex"; dvi = ""; ps2pdf = ""
  else
    print("Argument \"".. arg[narg] .. "\" ignored.")
  end
  narg = narg+1
end 


filename = arg[narg] 
if not filename then
  usage()
  os.exit(1)
end

extension = string.sub(filename, -4, -1)
if extension == ".tex" then
  basename = string.sub(filename, 1, -5)
else
  basename = filename
end

if not io.open(basename .. ".tex", "r") then
  print("Non-existent file: ", basename .. ".tex")
  os.exit(2)
end

whoami()
print("Processing ".. basename .. ".tex")
os.remove( basename .. ".mx2" )
if (os.execute(tex .. " " .. basename) == 0) and
   (os.execute(musixflx .. " " .. basename) == 0) and
   (os.execute(tex .. " " .. basename) == 0) and
   (dvi == "" or  (os.execute(dvi .. " " .. basename) == 0)) and
   (ps2pdf == "" or (os.execute(ps2pdf .. " " .. basename .. ".ps") == 0) )
then
  if dvi ~= "" then
    print(basename .. ".pdf generated.")
  end
  exit_code = 0
else
  print("Musixtex processing fails.")
--[[ uncomment for debugging
  print("tex = ", tex)
  print("dvi = ", dvi)
  print("ps2pdf = ", ps2pdf)
--]]
  exit_code = 3
end

-- clean-up:
os.remove( basename .. ".mx1" )
os.remove( basename .. ".mx2" )
if dvi ~= "" then 
  os.remove( basename .. ".dvi" )
end
os.remove( basename .. ".ps" )

os.exit( exit_code )
