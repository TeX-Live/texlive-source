#!/usr/bin/env texlua

-- This program is licensed under the terms of the MIT License.
--
-- Copyright (c) 2021-2023 Munehiro Yamamoto <munepixyz@gmail.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

NAME = "runtexshebang"
VERSION = "20231117 v0.5"
USAGE = [[
Usage:	runtexshebang [input.tex]

Options:
	--dry-run	print the TeX-style shebang line of [input.tex]
	-h, --help	print help
	-v, --version	print version

See also:
The command
	texdoc runtexshebang
should give you access to the README.
]]

function whoami()
  print(NAME .. " " .. VERSION)
end

function help()
   whoami()
   print()
   print(USAGE)
end

if #arg == 0 then
  help()
  os.exit(0)
end

--
is_dryrun = false
texfilename = ""
narg = 1
repeat
  this_arg = arg[narg]
  -- replace double dash by single dash at the beginning
  this_arg = string.gsub(this_arg, "^%-%-", "-")

  if this_arg == "-h" or this_arg == "-help" then
     help()
     os.exit(0)
  elseif this_arg == "-v" or this_arg == "-version" then
     whoami()
     os.exit(0)
  elseif this_arg == "-dry-run" then
     is_dryrun = true
  else
     texfilename = this_arg
  end --if this_arg == ...
  narg = narg+1
until narg > #arg

-- main process
whoami()

if ( texfilename == "" ) then
   print("No filename argument given, exiting.\n")
   os.exit(1)
end

line_ctr = 0
for line in io.lines(texfilename) do
   line_ctr = line_ctr + 1
   if line_ctr > 20 then break end

   if string.match(line, "^%%#!") then
      tex_cmd, err=string.gsub(line, "%%#!", "")

      if is_dryrun then
         print(tex_cmd .. "\n")
         os.exit(0)
      end

      tex_return = os.execute(tex_cmd)

      -- if os.execute(texcmd) returns -1 on Windows, then
      -- cmd.exe is not included in PATH, or some invalid string found before cmd.exe
      if os.type == 'windows' and tex_return == -1 then
         print("Invalid PATH setting found. Please ensure that cmd.exe can be found.\n")
         os.exit(1)
      end

      -- if not tex_return == 0 then
      --    print("TeX-style shebang processing of the below failed.\n" .. tex_cmd .. "\n")
      --    os.exit(1)
      -- end

      os.exit(0)

   -- else
   --    print("TeX-style shebang not matched.\n")
   end
end

print("TeX-style shebang not found.\n")
os.exit(2)
