#!/usr/bin/env texlua

-- Description: Image-based regression testing for LaTeX packages
-- Copyright: 2024-2025 (c)  Jianrui Lyu <tolvjr@163.com>
-- Repository: https://github.com/lvjr/ppmcheckpdf
-- License: The LaTeX Project Public License 1.3c

local pcp = pcp or {}

pcp.version = "2025A"
pcp.date = "2025-02-20"

--------------------------------------------
--> \section{Some code from l3build.lua}
--------------------------------------------

local lfs = require("lfs")

kpse.set_program_name("kpsewhich")
build_kpse_path = string.match(kpse.lookup("l3build.lua"),"(.*[/])")
local function build_require(s)
  require(kpse.lookup("l3build-"..s..".lua", { path = build_kpse_path } ) )
end

-----------------------------------------

build_require("file-functions")

release_date = "2021-04-26" -- for old build.lua file
dofile("build.lua")

build_require("variables")

------------------------------------------------------------
--> \section{Some variables and functions}
------------------------------------------------------------

local assert           = assert
local ipairs           = ipairs
local insert           = table.insert
local remove           = table.remove
local match            = string.match
local gsub             = string.gsub

local md5 = require("md5")

local function md5sum(str)
  if str then return md5.sumhexa(str) end
end

local function filesum(name)
  local f = assert(io.open(name, "rb"))
  local s = f:read("*all")
  f:close()
  return md5sum(s)
end

local function readfile(name)
  local f = assert(io.open(name, "rb"))
  local s = f:read("*all")
  f:close()
  return s
end

local function writefile(name, sum)
  local f = assert(io.open(name, "w"))
  f:write(sum)
  f:close()
end

local function getfiles(path, pattern)
  local files = { }
  for entry in lfs.dir(path) do
    if match(entry, pattern) then
     insert(files, entry)
    end
  end
  return files
end

------------------------------------------------------------
--> \section{Run check or save actions}
------------------------------------------------------------

imgext = imgext or ".png"

local function getimgopt(imgext)
  local imgopt = ""
  if imgext == ".png" then
    imgopt = " -png "
  elseif imgext == ".ppm" then
    imgopt = " "
  elseif imgext == ".pgm" then
    imgopt = " -gray "
  elseif imgext == ".pbm" then
    imgopt = " -mono "
  else
    error("unsupported image extension" .. imgext)
  end
  return imgopt
end

local function pdftoimg(path, pdf)
  cmd = "pdftoppm " .. getimgopt(imgext) .. pdf .. " " .. jobname(pdf)
  run(path, cmd)
end

local function saveImgMd5(dir, imgname, md5file, newmd5)
  print("save md5 and image files for " .. imgname)
  cp(imgname, dir, testfiledir)
  writefile(md5file, newmd5)
end

local issave = false

local function checkOnePdf(dir, job)
  local errorlevel
  local imgname = job .. imgext
  local md5file = testfiledir .. "/" .. job .. ".md5"
  local newmd5 = filesum(dir .. "/" .. imgname)
  if fileexists(md5file) then
    local oldmd5 = readfile(md5file)
    if newmd5 == oldmd5 then
      errorlevel = 0
      print("md5 check passed for " .. imgname)
    else
      errorlevel = 1
      print("md5 check failed for " .. imgname)
      local imgdiffexe = os.getenv("imgdiffexe")
      if imgdiffexe then
        local oldimg = abspath(testfiledir) .. "/" .. imgname
        local newimg = abspath(dir) .. "/" .. imgname
        local diffname = job .. ".diff.png"
        local cmd = imgdiffexe .. " " .. oldimg .. " " .. newimg
                    .. " -compose src " .. diffname
        print("creating image diff file " .. diffname)
        run(dir, cmd)
      elseif issave == true then
        saveImgMd5(dir, imgname, md5file, newmd5)
      end
    end
  else
    errorlevel = 0
    saveImgMd5(dir, imgname, md5file, newmd5)
  end
  return errorlevel
end

local function checkOneFolder(dir)
  print("checking folder " .. dir)
  local errorlevel = 0
  local pattern = "%" .. pdfext .. "$"
  local files = getfiles(dir, pattern)
  for _, v in ipairs(files) do
    pdftoimg(dir, v)
    pattern = "^" .. jobname(v):gsub("%-", "%%-") .. "%-%d+%" .. imgext .. "$"
    local imgfiles = getfiles(dir, pattern)
    if #imgfiles == 1 then
      local imgname = jobname(v) .. imgext
      if fileexists(dir .. "/" .. imgname) then
        rm(dir, imgname)
      end
      ren(dir, imgfiles[1], imgname)
      local e = checkOnePdf(dir, jobname(v)) or 0
      errorlevel = errorlevel + e
    else
      for _, i in ipairs(imgfiles) do
        local e = checkOnePdf(dir, jobname(i)) or 0
        errorlevel = errorlevel + e
      end
    end
  end
  return errorlevel
end

local function cfgToDir(cfg)
  if cfg == "build" then
    return testdir
  else
    -- testfiledir may has been changed
    dofile(cfg .. ".lua")
    return testdir .. "-" .. cfg
  end
end

local function checkAllFolders(arglist)
  if arglist[1] == "-c" then
    if arglist[2] then
      return checkOneFolder(cfgToDir(arglist[2]))
    else
      print("missing config name for -c option")
      return 0
    end
  else
    if #checkconfigs == 0 then
      return checkOneFolder(testdir)
    else
      local errorlevel = 0
      for _, v in ipairs(checkconfigs) do
        local dir = cfgToDir(v)
        local e = checkOneFolder(dir) or 0
        errorlevel = errorlevel + e
      end
      return errorlevel
    end
  end
end

------------------------------------------------------------
--> \section{Print help or version text}
------------------------------------------------------------

local helptext = [[
usage: ppmcheckpdf <action> [<options>]

valid actions are:
   check        Run tests without saving outputs of failed tests
   save         Run tests and save outputs of failed tests
   help         Print this message and exit
   version      Print version information and exit

valid options are:
   -c           Set the config used for check or save action

please report bug at https://github.com/lvjr/ppmcheckpdf
]]

local function help()
  print(helptext)
  return 0
end

local function version()
  print("Ppmcheckpdf Version " .. pcp.version .. " (" .. pcp.date .. ")\n")
  return 0
end

------------------------------------------------------------
--> \section{Respond to user input}
------------------------------------------------------------

local function pcpMain(pcparg)
  if pcparg[1] == nil then return help() end
  local action = remove(pcparg, 1)
  -- remove leading dashes
  action = match(action, "^%-*(.*)$")
  if action == "check" then
    return checkAllFolders(pcparg)
  elseif action == "save" then
    issave = true
    return checkAllFolders(pcparg)
  elseif action == "help" then
    return help()
  elseif action == "version" then
    return version()
  else
    print("unknown action '" .. action .. "'\n")
    return help()
  end
end

local function main()
  return pcpMain(arg)
end

-- it equals to total number of failed tests
local errorlevel = main()

--print(errorlevel)

if os.type == "windows" then os.exit(errorlevel) end
