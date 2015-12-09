#!/usr/bin/env texlua
--[[
Gregorio/Scribus script.
Copyright (C) 2010 Elie Roux <elie.roux@telecom-bretagne.eu>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

You must run this program with texlua, if possible under TeXLive 2010 (or later).
This program allows gregorio to be integrated in Scribus.
--]]

require"lfs"

local gregoriobin = 'gregorio'
local lualatexbin = "lualatex"

local function basename(name)
  return name and string.match(name,"^.+[/\\](.-)$") or name
end

local f = arg[1]
local dir = arg[2]
local current = ""

-- first we need to rebuild the arguments that are lost in pieces
for i, a in ipairs(arg) do
  if a:sub(1,1) == '"' then
    if a:sub(-1,-1) == '"' then
      current = a:sub(2, -2)
      if not f then
	f = current
      else
	dir = current
      end
    else
      current = a:sub(2, -1)
    end
  elseif a:sub(-1,-1) == '"' then
    current = current..' '..a:sub(1, -2)
    if not f then
      f = current
    else
      dir = current
    end
    current = ""
  else
    current = current..' '..a
  end
end

if dir then
  lfs.chdir(dir)
end

local pathbase = basename(f)

local fd = io.open(f, "r")
local texfile = io.open(f..".tex", "w")
local gabcfile = io.open(f.."-score.gabc", "w")
for l in fd:lines() do
  if string.match(l, "^\\") then
    texfile:write(l..'\n')
  elseif l=="\n" then
  else
    gabcfile:write(l..'\n')
  end
end

local format = string.format

texfile:write(format("\n\\gregorioscore{%s-score.gtex}\n\\end{document}\n", pathbase))
texfile:close()
gabcfile:close()

print(format("calling 'gregorio %s-score.gabc'\n", f))
os.remove(format("%s-score.gtex", f))
os.spawn(format("%s %s-score.gabc", gregoriobin, pathbase))
if not lfs.isfile(format("%s-score.gtex", pathbase)) then
  print("error: gregorio did not work as expected\n")
  exit(1)
end

print(format("calling 'lualatex --interaction nonstopmode %s.tex'\n", pathbase))
os.spawn(format("%s --interaction=nonstopmode %s.tex", lualatexbin, pathbase))
