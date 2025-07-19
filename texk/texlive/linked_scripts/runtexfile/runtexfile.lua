#!/usr/bin/env texlua

-----------------------------------------------------------------------
--         FILE:  runtexfile.lua
--  DESCRIPTION:  run a latex document with special steps
-- REQUIREMENTS:  texlua
--       AUTHOR:  Herbert Voß
--      LICENSE:  LPPL 1.3
--
-- %% $Id: runtexfile.lua 1146 2025-07-19 07:38:43Z herbert $
-----------------------------------------------------------------------
        runtexfile = runtexfile or { }
 local version = 0.03
runtexfile.version = version

--[[doc--

runtexfile(1)

This file is provided under the terms of the LPPL v1.3 or
later as printed in full text in the manual (runtexfile.pdf).

\url{https://ctan.org/license/lppl1.3}.

Report bugs to

    \url{https://gitlab.com/hvoss49/runtexfile}.

--doc]]--

kpse.set_program_name("texlua")

local f = kpse.find_file("lualibs.lua")
--print ("filename "..f)

require("lualibs")  -- all part of LuaTeX

local function flog(s)
  if verbose then tmpfile:write(s.."\n") end
end

flog("Read parameter ... ")

local args = require ('xindex-lapp') [[
  parameter handling
    -h,--help
    -v,--verbose
    -V,--version
    <file> (string)  .tex file
]]

--for i=1, #arg do
--   command[#command+1] = arg[i]
--end

if args.version then
  print("runtexfile version "..runtexfile.version)
  os.exit()
end

verbose = args.verbose
-- not_quiet = not args["quiet"]
if verbose then
  tmpfile = io.open("runtexfile.log","w")
end

local luaVersion = _VERSION
flog("Check Lua version: "..luaVersion)
if (luaVersion < "Lua 5.3") then
  print("=========================================")
  print("Sorry. but we need at least LuaTeX 1.09")
  print("Leaving program runtexfile")
  print("=========================================")
  os.exit()
end

print("runtexfile version "..runtexfile.version)
local current_dir = "."

--[[
current_dir = os.getenv("pwd") or io.popen("cd"):read()
print("Run wrapperscript in directory "..current_dir)
flog("Run wrapperscript in directory "..current_dir)
]]

local LTXfile = args.file
print("Main file: "..LTXfile)
flog("Main file: "..LTXfile)

local commands = {"lualatex", "luatex", "luahbtex", "latex", "pdflatex", "xelatex", "xetex" }

local function isInArray(value,array)
  for _,v in pairs(array) do
    if v == value then
      return true
    end
  end
  return false
end

local function getFileName(s)
--   get different file name: runtexfile ... <filename>
    local r = s:match "(%b<>)$"
    return r and r:sub(2,-2)
--    return string.match(s, "<([^<>]-)>%s*$")
end

local specialFileName

file = io.open(LTXfile,"r")
if not file then
  file = io.open(LTXfile..".tex","r")
  if not file then
    print("Fatal error: no file "..LTXfile.." or "..LTXfile..".tex")
    print("I will exit ...")
    flog("Fatal error: no file "..LTXfile.." or "..LTXfile..".tex")
    if verbose then tmpfile:close() end
    os.exit()
  end
end

local step = 1
local saveLTXfile = LTXfile
local commandLineFound = false

for line in file:lines() do
  if line ~= "" then
    words = {}
    flog(step..": "..line)
    for word in line:gmatch("%S+") do table.insert(words, word) end
    if words[1] == "%!" and words[2] == "HV" then
      commandLineFound = true
      specialFileName = getFileName(line)
      if specialFileName then
        print("Special filename = ".. specialFileName)
      else
        print("Filename = "..LTXfile)
      end
      command = words[3]
      if isInArray(command,commands) then 
        para = "--interaction=nonstopmode"   -- kind of TeX run
      else
        para = ""                            -- something else
      end
      flog("Initial parameter: "..para)
      if #words > 3 then
        if specialFileName then
          noPara = #words - 1
        else 
          noPara = #words
        end
        for i = 4,noPara do para = para .. " " .. words[i] end
      end
      flog("Scanned parameter: "..para)
      if specialFileName then LTXfile = specialFileName end
      flog("Real filename: "..LTXfile)
      flog("Will run: ".. command.." "..para.." "..LTXfile)
      flog("and log into file "..current_dir.."/runtexfile-"..step..".log")
      print("running: " .. command.." "..para.." "..LTXfile)
      os.execute(command.." "..para.." "..LTXfile.." > "..current_dir.."/runtexfile-"..step..".log") 
    else
      break
    end
    LTXfile = saveLTXfile
    step = step + 1
  end
end
    
if commandLineFound == false then
  print("No command line with %! HV <command> defined! running: " .. commands[1])
  os.execute(commands[1].." "..para.." "..LTXfile.." > "..current_dir.."/runtexfile-"..step..".log") 
end
file:close()
if verbose then tmpfile:close() end
-- os.exit()
