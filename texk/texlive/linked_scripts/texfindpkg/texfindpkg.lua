#!/usr/bin/env texlua

-- Description: Install TeX packages and their dependencies
-- Copyright: 2023 (c) Jianrui Lyu <tolvjr@163.com>
-- Repository: https://github.com/lvjr/texfindpkg
-- License: GNU General Public License v3.0

local tfp = tfp or {}

tfp.version = "2023E"
tfp.date = "2023-05-05"

local building = tfp.building
local tfpresult = ""

------------------------------------------------------------
--> \section{Some variables and functions}
------------------------------------------------------------

local lfs = require("lfs")

local insert = table.insert
local remove = table.remove
local concat = table.concat
local gmatch = string.gmatch
local match  = string.match
local find   = string.find
local gsub   = string.gsub
local sub    = string.sub
local rep    = string.rep

local lookup = kpse.lookup
kpse.set_program_name("kpsewhich")

require(lookup("lualibs.lua"))
local json = utilities.json -- for json.tostring and json.tolua
local gzip = gzip           -- for gzip.compress and gzip.decompress

local function tfpPrint(msg)
  msg = "[tfp] " .. msg
  if building then
    tfpresult = tfpresult .. msg .. "\n"
  else
    print(msg)
  end
end

local function tfpRealPrint(msg)
  if not building then
    print("[tfp] " .. msg)
  end
end

local showdbg = false

local function dbgPrint(msg)
  if showdbg then print("[debug] " .. msg) end
end

local function valueExists(tab, val)
  for _, v in ipairs(tab) do
    if v == val then return true end
  end
  return false
end

local function getFiles(path, pattern)
  local files = { }
  for entry in lfs.dir(path) do
    if match(entry, pattern) then
     insert(files, entry)
    end
  end
  return files
end

local function fileRead(input)
  local f = io.open(input, "rb")
  local text
  if f then -- file exists and is readable
    text = f:read("*all")
    f:close()
    --print(#text)
    return text
  end
  -- return nil if file doesn't exists or isn't readable
end

local function fileWrite(text, output)
  -- using "wb" keeps unix eol characters
  f = io.open(output, "wb")
  f:write(text)
  f:close()
end

local function testDistribution()
  -- texlive returns "texmf-dist/web2c/updmap.cfg"
  -- miktex returns nil although there is "texmfs/install/miktex/config/updmap.cfg"
  local d = lookup("updmap.cfg")
  if d then
    return "texlive"
  else
    return "miktex"
  end
end

------------------------------------------------------------
--> \section{Handle TeX Live package database}
------------------------------------------------------------

local tlpkgtext
local tlinspkgtext

local function tlReadPackageDB()
  local tlroot = kpse.var_value("TEXMFROOT")
  if tlroot then
    tlroot = tlroot .. "/tlpkg"
  else
    tfpPrint("error in finding texmf root!")
  end
  local list = getFiles(tlroot, "^texlive%.tlpdb%.main")
  if #list > 0 then
    tlpkgtext = fileRead(tlroot .. "/" .. list[1])
    if not tlpkgtext then
      tfpPrint("error in reading texlive.tlpdb.main file!")
    end
  else
    -- no texlive.tlpdb.main file in a fresh TeX live
    tfpPrint("error in finding texlive package database!")
    tfpPrint("please run 'tlmgr update --self' first.")
  end
  tlinspkgtext = fileRead(tlroot .. "/texlive.tlpdb")
  if not tlinspkgtext then
    tfpPrint("error in reading texlive.tlpdb file!")
  end
end

local tlpkgdata = {}
local tlinspkgdata = {}

local function tlExtractFiles(name, desc)
  -- ignore binary packages
  -- also ignore latex-dev packages
  if find(name, "%.") or find(name, "^latex%-[%a]-%-dev") then
    --print(name)
    return
  end
  -- ignore package files in doc folder
  desc = match(desc, "\nrunfiles .+") or ""
  for base, ext in gmatch(desc, "/([%a%d%-%.]+)%.([%a%d]+)\n") do
    if ext == "sty" or ext == "cls" or ext == "tex" or ext == "ltx" then
      dbgPrint(name, base .. "." .. ext)
      tlpkgdata[base .. "." .. ext] = name
    end
  end
end

local function tlExtractPackages(name, desc)
  tlinspkgdata[name] = true
end

local function tlParsePackageDB(tlpkgtext)
  gsub(tlpkgtext, "name (.-)\n(.-)\n\n", tlExtractFiles)
  return tlpkgdata
end

local function tlParseTwoPackageDB()
  gsub(tlpkgtext, "name (.-)\n(.-)\n\n", tlExtractFiles)
  -- texlive.tlpdb might use different eol characters
  gsub(tlinspkgtext, "name (.-)\r?\n(.-)\r?\n\r?\n", tlExtractPackages)
end

------------------------------------------------------------
--> \section{Handle MiKTeX package database}
------------------------------------------------------------

local mtpkgtext
local mtinspkgtext

local function mtReadPackageDB()
  local mtvar = kpse.var_value("TEXMFDIST")
  if mtvar then
    mtpkgtext = fileRead(mtvar .. "/miktex/config/package-manifests.ini")
    if not mtpkgtext then
      tfpPrint("error in reading package-manifests.ini file!")
    end
    mtinspkgtext = fileRead(mtvar .. "/miktex/config/packages.ini")
    if not mtinspkgtext then
      tfpPrint("error in reading packages.ini file!")
    end
  else
    tfpPrint("error in finding texmf root!")
  end
end

local mtpkgdata = {}
local mtinspkgdata = {}

local function mtExtractFiles(name, desc)
  -- ignore package files in source or doc folders
  -- also ignore latex-dev packages
  if find(name, "_") or find(name, "^latex%-[%a]-%-dev") then
    --print(name)
    return
  end
  for base, ext in gmatch(desc, "/([%a%d%-%.]+)%.([%a%d]+)\r?\n") do
    if ext == "sty" or ext == "cls" or ext == "tex" or ext == "ltx" then
      dbgPrint(name, base .. "." .. ext)
      mtpkgdata[base .. "." .. ext] = name
    end
  end
end

local function mtExtractPackages(name, desc)
  mtinspkgdata[name] = true
end

local function mtParsePackageDB(mtpkgtext)
  -- package-manifests.ini might use different eol characters
  gsub(mtpkgtext, "%[(.-)%]\r?\n(.-)\r?\n\r?\n", mtExtractFiles)
  return mtpkgdata
end

local function mtParseTwoPackageDB()
  -- package-manifests.ini and packages.ini might use different eol characters
  gsub(mtpkgtext, "%[(.-)%]\r?\n(.-)\r?\n\r?\n", mtExtractFiles)
  gsub(mtinspkgtext, "%[(.-)%]\r?\n(.-)\r?\n\r?\n", mtExtractPackages)
end

------------------------------------------------------------
--> \section{Install packages in current TeX distribution}
------------------------------------------------------------

local dist               -- name of current tex distribution
local totalinslist = {}  -- list of all missing packages
local filecount = 0      -- total number of files found

local function initPackageDB()
  dist = testDistribution()
  tfpPrint("you are using " .. dist)
  if dist == "texlive" then
    tlReadPackageDB()
    tlParseTwoPackageDB()
  else
    mtReadPackageDB()
    mtParseTwoPackageDB()
  end
end

local function findOnePackage(fname)
  if dist == "texlive" then
    return tlpkgdata[fname]
  else
    return mtpkgdata[fname]
  end
end

local function checkInsPakage(pkg)
  if dist == "texlive" then
    return tlinspkgdata[pkg]
  else
    return mtinspkgdata[pkg]
  end
end

local function tfpExecute(c)
  if not building then
    if os.type == "windows" then
      os.execute(c)
    else
      os.execute('sudo env "PATH=$PATH" ' .. c)
    end
  end
end

local function installSomePackages(list)
  if not list then return end
  if dist == "texlive" then
    local pkgs = concat(list, " ")
    if #list > 1 then
      tfpRealPrint("installing texlive packages: " .. pkgs)
    else
      tfpRealPrint("installing texlive package: " .. pkgs)
    end
    tfpExecute("tlmgr install " .. pkgs)
  else
    for _, p in ipairs(list) do
      tfpRealPrint("installing miktex package: " .. p)
      tfpExecute("miktex packages install " .. p)
    end
  end
end

local function updateTotalInsList(inslist)
  if #totalinslist == 0 then
    totalinslist = inslist
  else
    for _, pkg in ipairs(inslist) do
      if not valueExists(totalinslist, pkg) then
        insert(totalinslist, pkg)
      end
    end
  end
end

local function listSomePackages(list)
  if not list then return {} end
  if #list > 0 then
    filecount = filecount + 1
  end
  local pkgs = concat(list, " ")
  if #list == 1 then
    tfpPrint(dist .. " package needed: " .. pkgs)
  else
    tfpPrint(dist .. " packages needed: " .. pkgs)
  end
  local inslist = {}
  for _, p in ipairs(list) do
    if not checkInsPakage(p) then
      insert(inslist, p)
    end
  end
  if #inslist == 0 then
    if #list == 1 then
      tfpRealPrint("this package is already installed")
    else
      tfpRealPrint("these packages are already installed")
    end
  else
    local pkgs = concat(inslist, " ")
    if #inslist == 1 then
      tfpRealPrint(dist .. " package not yet installed: " .. pkgs)
    else
      tfpRealPrint(dist .. " packages not yet installed: " .. pkgs)
    end
  end
  updateTotalInsList(inslist)
end

------------------------------------------------------------
--> \section{Find dependencies of package files}
------------------------------------------------------------

local tfptext = ""  -- the json text
local tfpdata = {}  -- the lua object
local fnlist  = {}  -- file name list
local pkglist = {}  -- package name list

local function initDependencyDB()
  local ziptext = fileRead(lookup("texfindpkg.json.gz"))
  tfptext = gzip.decompress(ziptext)
  if tfptext then
    --print(tfptext)
    tfpdata = json.tolua(tfptext)
  else
    tfpPrint("error in reading texfindpkg.json.gz!")
  end
end

local function printDependency(fname, level)
  local msg = fname
  local pkg = findOnePackage(fname)
  if pkg then
    msg = msg .. " (from " .. pkg .. ")"
    if not valueExists(pkglist, pkg) then
      insert(pkglist, pkg)
    end
  else
    msg = msg .. " (not found)"
  end
  if level == 0 then
    tfpPrint(msg)
  else
    tfpPrint(rep("   ", level - 1) .. "|- " .. msg)
  end
end

local function findDependencies(fname, level)
  --print(fname)
  if valueExists(fnlist, fname) then return end
  local item = tfpdata[fname]
  if not item then
    -- no dependency info for fname
    printDependency(fname, level)
    return
  end
  -- finding dependencies for fname
  printDependency(fname, level)
  insert(fnlist, fname)
  local deps = item.deps
  if deps then
    for _, dname in ipairs(deps) do
      findDependencies(dname, level + 1)
    end
  end
end

local function queryByFileName(fname)
  fnlist, pkglist = {}, {} -- reset the list
  tfpPrint("building dependency tree for " .. fname .. ":")
  tfpPrint(rep("-", 24))
  findDependencies(fname, 0)
  tfpPrint(rep("-", 24))
  if #fnlist == 0 then
    tfpPrint("could not find any package with file " .. fname)
    return
  end
  if #pkglist == 0 then
    tfpPrint("error in finding package in " .. dist)
    return
  end
  listSomePackages(pkglist)
end

local function getFileNameFromCmdEnvName(cmdenv, name)
  --print(name)
  local flist = {}
  for line in gmatch(tfptext, "(.-)\n[,}]") do
    if find(line, '"' .. name .. '"') then
      --print(line)
      local fname, fspec = match(line, '"(.-)":(.+)')
      --print(fname, fspec)
      local item = json.tolua(fspec)
      if item[cmdenv] and valueExists(item[cmdenv], name) then
        insert(flist, fname)
      end
    end
  end
  return flist
end

local function queryByCommandName(cname)
  --print(cname)
  local flist = getFileNameFromCmdEnvName("cmds", cname)
  if #flist > 0 then
    for _, fname in ipairs(flist) do
      tfpPrint(rep("=", 48))
      tfpPrint("found package file " .. fname .. " with command \\" .. cname)
      queryByFileName(fname)
    end
  else
    tfpPrint("could not find any package with command \\" .. cname)
  end
end

local function queryByEnvironmentName(ename)
  --print(ename)
  local flist = getFileNameFromCmdEnvName("envs", ename)
  if #flist > 0 then
    for _, fname in ipairs(flist) do
      tfpPrint(rep("=", 48))
      tfpPrint("found package file " .. fname .. " with environment {" .. ename .. "}")
      queryByFileName(fname)
    end
  else
    tfpPrint("could not find any package with environment {" .. ename .. "}")
  end
end

local function queryOne(t, name)
  if t == "cmd" then
    queryByCommandName(name)
  elseif t == "env" then
    queryByEnvironmentName(name)
  elseif t == "file" then
    tfpPrint(rep("=", 48))
    queryByFileName(name)
  else
    -- do something for t == "pkg"
  end
end

local function query(namelist)
  for _, v in ipairs(namelist) do
    queryOne(v[1], v[2])
  end
  if filecount > 1 then
    tfpRealPrint(rep("=", 48))
    local pkgs = concat(totalinslist, " ")
    if #totalinslist == 1 then
      tfpRealPrint(dist .. " package not yet installed in total: " .. pkgs)
    else
      tfpRealPrint(dist .. " packages not yet installed in total: " .. pkgs)
    end
  end
end

local function install(namelist)
  query(namelist)
  if #totalinslist > 0 then
    installSomePackages(totalinslist)
  end
end

------------------------------------------------------------
--> \section{Parse query or install arguments}
------------------------------------------------------------

local function parseName(name)
  local h = sub(name, 1, 1)
  if h == "\\" then
    local b = sub(name, 2)
    return({"cmd", b})
  elseif h == "{" then
    if sub(name, -1) == "}" then
      local b = sub(name, 2, -2)
      return({"env", b})
    else
      error("invalid name '" .. name .. "'")
    end
  elseif find(name, "%.") then
    return({"file", name})
  else
    return({"pkg", name})
  end
end

local function parseArgList(arglist)
  local namelist = {}
  local nametype = nil
  for _, v in ipairs(arglist) do
    if v == "-f" then
      nametype = "file"
    elseif v == "-c" then
      nametype = "cmd"
    elseif v == "-e" then
      nametype = "env"
    else
      if nametype then
        insert(namelist, {nametype, v})
      else
        insert(namelist, parseName(v))
      end
    end
  end
  if #namelist == 0 then
    error("missing the name of file/cmd/env!")
  else
    return namelist
  end
end

local function doQuery(arglist)
  local namelist = parseArgList(arglist)
  initPackageDB()
  initDependencyDB()
  query(namelist)
end

local function doInstall(arglist)
  local namelist = parseArgList(arglist)
  initPackageDB()
  initDependencyDB()
  install(namelist)
end

------------------------------------------------------------
--> \section{Print help or version text}
------------------------------------------------------------

local helptext = [[
usage: texfindpkg <action> [<options>] [<name>]

valid actions are:
   install      Install some package and its dependencies
   query        Query dependencies for some package
   help         Print this message and exit
   version      Print version information and exit

valid options are:
   -f           Query or install by file name
   -c           Query or install by command name
   -e           Query or install by environment name

please report bug at https://github.com/lvjr/texfindpkg
]]

local function help()
  print(helptext)
end

local function version()
  print("TeXFindPkg Version " .. tfp.version .. " (" .. tfp.date .. ")\n")
end

------------------------------------------------------------
--> \section{Respond to user input}
------------------------------------------------------------

local function tfpMain(tfparg)
  tfpresult = ""
  if tfparg[1] == nil then return help() end
  local action = remove(tfparg, 1)
  action = match(action, "^%-*(.*)$") -- remove leading dashes
  --print(action)
  if action == "query" then
    doQuery(tfparg)
  elseif action == "install" then
    doInstall(tfparg)
  elseif action == "help" then
    help()
  elseif action == "version" then
    version()
  else
    tfpPrint("unknown action '" .. action .. "'")
    help()
  end
  return tfpresult
end

local function main()
  tfpMain(arg)
end

if building then
  tfp.tfpMain          = tfpMain
  tfp.showdbg          = showdbg
  tfp.dbgPrint         = dbgPrint
  tfp.tfpPrint         = tfpPrint
  tfp.fileRead         = fileRead
  tfp.fileWrite        = fileWrite
  tfp.getFiles         = getFiles
  tfp.valueExists      = valueExists
  tfp.json             = json
  tfp.gzip             = gzip
  tfp.tlParsePackageDB = tlParsePackageDB
  tfp.mtParsePackageDB = mtParsePackageDB
  return tfp
else
  main()
end
