#!/usr/bin/env texlua
--[[
Gregorio Windows automatic installation script.
Copyright (C) 2010-2015 Gregorio Project authors

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

This texlua script is called in Windows automatic installer (see gregorio.iss),
it installs GregorioTeX under Windows.
--]]

require("lfs")

kpse.set_program_name("luatex")

local pathsep = '/'

local function fixpath(path)
  return path
end

local function remove_trailing_slash(path)
  return path:gsub("/$" ,"")
end

local function basename(path)
  return path:gsub("^[^/]+/" ,"")
end

if os.type == "windows" or os.type == "msdos" then
  pathsep = '\\'
  fixpath = function(path)
    return path:gsub("/", "\\")
  end
  remove_trailing_slash = function(path)
    return path:gsub("\\$" ,"")
  end
  basename = function(path)
    return path:gsub("^[^\\]+\\" ,"")
  end
end

local texmflocal = fixpath(kpse.expand_var("$TEXMFLOCAL"))..pathsep
local suffix = "gregoriotex"..pathsep

function io.loaddata(filename,textmode)
  local f = io.open(filename,(textmode and 'r') or 'rb')
  if f then
    local data = f:read('*all')
    f:close()
    return data
  else
    return nil
  end
end

function io.savedata(filename,data,joiner)
  local f = io.open(filename,"wb")
  if f then
    f:write(data or "")
    f:close()
    return true
  else
    return false
  end
end

function copy_one_file(src, dest)
  local destfile = dest..basename(src)
  io.savedata(destfile, io.loaddata(src))
end

function copy_files()
  if not lfs.isdir(texmflocal) then
    lfs.mkdir(texmflocal)
  end
  print("Copying files...\n")
  local texmfdist = kpse.expand_var("$TEXMFDIST")
  --[[ MiKTeX uses slightly different paths for the location of it's bin
  directory for 32 and 64 bit versions.  Since the copy command will fail
  silently if the destination directory doesn't exist, the simplest way to deal
  with this is to simply try copying to both locations.
  --]]
  texmfbin_32 = fixpath(texmfdist.."/miktex/bin/")
  texmfbin_64 = fixpath(texmfdist.."/miktex/bin/x64/")
  print("gregorio.exe...")
  copy_one_file("gregorio.exe", texmfbin_32)
  copy_one_file("gregorio.exe", texmfbin_64)
  print("GregorioTeX files...")
  os.spawn("xcopy texmf "..texmflocal.." /e /f /y")
end

function run_texcommands()
  print("Running initexmf\n")
  local p = os.spawn("initexmf -u --admin")
end

function main_install()
  copy_files()
  run_texcommands()
  print("Post-install script complete.")
  print("Press return to continue...")
  answer=io.read()
end

function scribus_config()
  local f = io.open('contrib'..pathsep..'900_gregorio.xml', 'r')
  local data = ""
  for l in f:lines() do
    if l:match("executable command") then
      data = data..string.format("	<executable command='texlua \"%s\" \"%%file\" \"%%dir\"'/>\n", lfs.currentdir()..pathsep.."contrib"..pathsep.."gregorio-scribus.lua")
    else
      data = data..l.."\n"
    end
  end
  io.savedata('contrib'..pathsep..'900_gregorio.xml', data)
end

main_install()
scribus_config()

