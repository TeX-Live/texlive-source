#!/usr/bin/env texlua  

VERSION = "0.12"

--[[
     musixtex.lua: processes MusiXTeX files (and deletes intermediate files)

     (c) Copyright 2012-15 Bob Tennent rdt@cs.queensu.ca

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
     version 0.12 2015-11-28 RDT
      Process .ltx files with -l implied

     version 0.11 2015-07-16 RDT
      Automatic autosp preprocessing. 

     version 0.10 2015-04-23 RDT
      Add -a option to preprocess using autosp

     version 0.9 2015-02-13 RDT
      Add an additional latex pass to resolve cross-references.
      Add -e0 option to dvips as given in musixdoc.tex
      Add -x option to call makeindex

     version 0.8 2014-05-18 RDT
      Add -g option

     version 0.7  2013-12-11 RDT
      Add -F fmt option

     version 0.6  2012-09-14 RDT
      Add -1 (one-pass [pdf][la]tex processing) option.

     version 0.5  2011-11-28 RDT
      Add -i (retain intermediate files) option.

     version 0.4  2011-04-30 RDT
       Allow multiple filenames (and options).
       Add -f (default) and -l (latex) options.

     version 0.3  2011-04-25 RDT
       Add -d (dvipdfm)  and -s (stop at dvi) options.

     version 0.2  2011-04-21 RDT
       Allow basename.tex as filename.
       Add -p option for pdfetex processing.
       Add standard -v -h options.

--]]

function usage()
  print("Usage:  [texlua] musixtex.lua { option | basename[.tex] | basename[.aspc] basename[.ltx] } ... ")
  print("options: -v  version")
  print("         -h  help")
  print("         -l  latex (or pdflatex) (implied with .ltx extension)")
  print("         -p  pdfetex (or pdflatex)")
  print("         -d  dvipdfm")
  print("         -s  stop at dvi")
  print("         -g  stop at ps")
  print("         -i  retain intermediate files")
  print("         -1  one-pass [pdf][la]tex processing")
  print("         -F fmt  use fmt as the TeX processor")
  print("         -x  run makeindex")
  print("         -a  preprocess using autosp (impled with .aspc extension)")
  print("         -f  restore default processing")
end

function whoami ()
  print("This is musixtex.lua version ".. VERSION .. ".")
end

whoami()
if #arg == 0 then
  usage()
  os.exit(0)
end

-- defaults:
tex = "etex"  
musixflx = "musixflx"
dvips = "dvips -e0 "
dvi = dvips
ps2pdf = "ps2pdf"
intermediate = 1
passes = 2
index = 0
preprocess = 0

exit_code = 0
narg = 1
repeat
  this_arg = arg[narg]
  if this_arg == "-v" then
    os.exit(0)
  elseif this_arg == "-h" then
    usage()
    os.exit(0)
  elseif this_arg == "-l" then
    if tex == "pdfetex" then
      tex = "pdflatex"
    else
      tex = "latex"
    end
  elseif this_arg == "-p" then
    if tex == "latex" then
      tex = "pdflatex"
    else
      tex = "pdfetex"
    end
    dvi = ""; ps2pdf = ""
  elseif this_arg == "-d" then
    dvi = "dvipdfm"; ps2pdf = ""
  elseif this_arg == "-s" then
    dvi = ""; ps2pdf = ""
  elseif this_arg == "-i" then
    intermediate = 0
  elseif this_arg == "-1" then
    passes = 1
  elseif this_arg == "-f" then
    tex = "etex"; dvi = dvips; ps2pdf = "ps2pdf"; intermediate = 1; passes = 2; index = 0; preprocess = 0
  elseif this_arg == "-g" then
    dvi = dvips; ps2pdf = ""
  elseif this_arg == "-x" then
    index = 1
  elseif this_arg == "-a" then
    preprocess = 1
  elseif this_arg == "-F" then
    narg = narg+1
    tex = arg[narg]
  else
    repeat  -- pseudo loop to get effect of "continue" using "break"
      texfilename = this_arg 
      if texfilename ~= "" and string.sub(texfilename, -5, -1) == ".aspc" then
        if io.open(texfilename, "r") then
          print("Processing ".. texfilename )
        else
          print("No file: " .. texfilename )
          break -- out of pseudo loop
        end
        preprocess = 1
        filename = string.sub(texfilename, 1, -6)
        texfilename = filename .. ".tex"
      elseif texfilename ~= "" and string.sub(texfilename, -4, -1) == ".ltx" then
        if io.open(texfilename, "r") then
          print("Processing ".. texfilename )
        else
          print("No file: " .. texfilename )
          break -- out of pseudo loop
        end
        if tex == "pdfetex" then
          tex = "pdflatex"
        else
          tex = "latex"
        end
        filename = string.sub(texfilename, 1, -5) 
      elseif texfilename ~= "" and string.sub(texfilename, -4, -1) == ".tex" then
        if io.open(texfilename, "r") then
          print("Processing ".. texfilename )
        else
          print("No file: " .. texfilename )
          break -- out of pseudo loop
        end
        filename = string.sub(texfilename, 1, -5)
      elseif io.open(texfilename .. ".aspc", "r") then
        preprocess = 1
        print("Processing ".. texfilename .. ".aspc")
        filename = texfilename 
      elseif io.open(texfilename .. ".ltx", "r") then
        if tex == "pdfetex" then
          tex = "pdflatex"
        else
          tex = "latex"
        end
        ltx = 1
        filename = texfilename
        texfilename = filename .. ".ltx"
      elseif io.open(texfilename .. ".tex", "r") then
        print("Processing ".. texfilename .. ".tex")
        filename = texfilename
        texfilename = filename .. ".tex"
      else
        print("No file: " .. texfilename )
        break -- out of pseudo loop
      end
      if preprocess == 1 and os.execute("autosp " .. filename) ~= 0 then
        print ("Preprocessing fails.")
        break -- out of pseudo loop
      end
      os.remove( filename .. ".mx2" )
      if (passes == 1 or os.execute(tex .. " " .. texfilename) == 0) and
         (passes == 1 or os.execute(musixflx .. " " .. filename) == 0) and
         (os.execute(tex .. " " .. texfilename) == 0) and
         ((tex ~= "latex" and tex ~="pdflatex")
           or (index == 0)
           or (os.execute("makeindex -q " .. filename) == 0)) and
         ((tex ~= "latex" and tex ~= "pdflatex")
           or (os.execute(tex .. " " .. texfilename) == 0)) and
         ((tex ~= "latex" and tex ~= "pdflatex")
           or (os.execute(tex .. " " .. texfilename) == 0)) and
         ((tex ~= "latex" and tex ~= "pdflatex")
           or (os.execute(tex .. " " .. texfilename) == 0)) and
         (dvi == "" or  (os.execute(dvi .. " " .. filename) == 0)) and
         (ps2pdf == "" or (os.execute(ps2pdf .. " " .. filename .. ".ps") == 0) )
      then 
        if ps2pdf ~= "" then 
          print(filename .. ".pdf generated by " .. ps2pdf .. ".")
        end
        if intermediate == 1 then -- clean-up:
          os.remove( filename .. ".mx1" )
          os.remove( filename .. ".mx2" )
          if dvi ~= "" then
            os.remove( filename .. ".dvi" )
          end
          if ps2pdf ~= "" then 
            os.remove( filename .. ".ps" )
          end
        end
      else
        print("Musixtex processing of " .. texfilename " fails.\n")
        exit_code = 2
        --[[ uncomment for debugging
        print("tex = ", tex)
        print("dvi = ", dvi)
        print("ps2pdf = ", ps2pdf)
        --]]
      end
    until true  -- end of pseudo loop

  end --if this_arg == ...
  narg = narg+1
until narg > #arg 
os.exit( exit_code )
