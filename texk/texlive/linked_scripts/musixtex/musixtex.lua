#!/usr/bin/env texlua  

VERSION = "0.2"

--[[
     musixtex.lua: runs [pdf]etex -> musixflx -> [pdfe]tex [ -> dvips -> ps2pdf ]

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

     version 0.2  2011-04-21 RDT
       Allow basename.tex as filename.
       Add -p option for pdfetex processing.
       Add standard -v -h options.

--]]


tex = "etex"  
musixflx = "musixflx"
dvips = "dvips -q"
ps2pdf = "ps2pdf"

function usage()
  print("Usage:  [texlua] musixtex.lua [options] basename[.tex]")
end

function whoami ()
  print("This is musixtex.lua version ".. VERSION .. ".")
end

if #arg == 0 then
  usage()
  os.exit(1)
end

narg = 1

if arg[narg] == "-v" then
  whoami()
  os.exit(0)
elseif arg[narg] == "-h" then
  usage()
  os.exit(0)
end

while narg ~= #arg do
  if arg[narg] == "-p" then
    tex = "pdfetex"
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
if 
  (os.execute(tex .. " " .. basename) == 0) 
  and (os.execute(musixflx .. " " .. basename) == 0) 
  and (os.execute(tex .. " " .. basename) == 0) 
  and (tex == "pdfetex" or (os.execute(dvips .. " -o " .. basename ..".ps " .. basename) == 0) )
  and (tex == "pdfetex" or (os.execute(ps2pdf .. " " .. basename .. ".ps " .. basename .. ".pdf") == 0) )
then
  print(basename .. ".pdf generated.")
  exit_code = 0
else
  print("musixtex processing fails.")
  exit_code = 3
end
os.remove( basename .. ".mx1" )
os.remove( basename .. ".mx2" )
os.remove( basename .. ".dvi" )
os.remove( basename .. ".ps" )
os.exit( exit_code )
