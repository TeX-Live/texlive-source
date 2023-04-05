#!/usr/bin/env texlua

-- Description: Install TeX packages and their dependencies
-- Copyright: 2023 (c) Jianrui Lyu <tolvjr@163.com>
-- Repository: https://github.com/lvjr/texfindpkg
-- License: GNU General Public License v3.0

local tfpversion = "2023D"
local tfpdate = "2023-04-05"

------------------------------------------------------------
--> \section{Some variables and functions}
------------------------------------------------------------

local lfs = require("lfs")
local insert = table.insert
local match = string.match

local lookup = kpse.lookup
kpse.set_program_name("kpsewhich")

require(lookup("lualibs.lua"))
local json = utilities.json -- for json.tostring and json.tolua
local gzip = gzip           -- for gzip.compress and gzip.decompress

local function tfpPrint(msg)
  print("[tfp] " .. msg)
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
      tfpPrint("error in reading texlive package database!")
    end
  else
    -- no texlive.tlpdb.main file in a fresh TeX live
    tfpPrint("error in finding texlive package database!")
    tfpPrint("please run 'tlmgr update --self' first.")
  end
end

local tlpkgdata = {}

local function tlExtractFiles(name, desc)
  -- ignore binary packages
  -- also ignore latex-dev packages
  if name:find("%.") or name:find("^latex%-[%a]-%-dev") then
    --print(name)
    return
  end
  -- ignore package files in doc folder
  desc = desc:match("\nrunfiles .+") or ""
  for base, ext in desc:gmatch("/([%a%d%-]+)%.([%a%d]+)\n") do
    if ext == "sty" or ext == "cls" then
      dbgPrint(name, base .. "." .. ext)
      tlpkgdata[base .. "." .. ext] = name
    end
  end
end

local function tlParsePackageDB()
  tlpkgtext:gsub("name (.-)\n(.-)\n\n", tlExtractFiles)
end

------------------------------------------------------------
--> \section{Handle MiKTeX package database}
------------------------------------------------------------

local mtpkgtext

local function mtReadPackageDB()
  local mtvar = kpse.var_value("TEXMFVAR")
  if mtvar then
    mtpdb = mtvar .. "/miktex/cache/packages/miktex-zzdb3-2.9/package-manifests.ini"
  else
    tfpPrint("error in finding texmf root!")
  end
  mtpkgtext = fileRead(mtpdb)
  if not mtpkgtext then
    tfpPrint("error in reading miktex package database!")
  end
end

local mtpkgdata = {}

local function mtExtractFiles(name, desc)
  -- ignore package files in source or doc folders
  -- also ignore latex-dev packages
  if name:find("_") or name:find("^latex%-[%a]-%-dev") then
    --print(name)
    return
  end
  for base, ext in desc:gmatch("/([%a%d%-]+)%.([%a%d]+)\r?\n") do
    if ext == "sty" or ext == "cls" then
      dbgPrint(name, base .. "." .. ext)
      mtpkgdata[base .. "." .. ext] = name
    end
  end
end

local function mtParsePackageDB()
  -- package-manifests.ini might use different eol characters
  mtpkgtext:gsub("%[(.-)%]\r?\n(.-)\r?\n\r?\n", mtExtractFiles)
end

------------------------------------------------------------
--> \section{Compare TeX Live and MiKTeX packages}
------------------------------------------------------------

local tlpkgname = "download/texlive.tlpdb"
local mtpkgname = "download/package-manifests.ini"

local function compareDistributions()
  tlpkgtext = fileRead(tlpkgname)
  if tlpkgtext then
    tlParsePackageDB()
  else
    tfpPrint("error in reading texlive package database!")
  end
  mtpkgtext = fileRead(mtpkgname)
  if mtpkgtext then
    mtParsePackageDB()
  else
    tfpPrint("error in reading miktex package database!")
  end
  local tlmissing, mkmissing = {}, {}
  for k, vt in pairs(tlpkgdata) do
    local vm = mtpkgdata[k] -- or "[none]"
    if vm then
      mtpkgdata[k] = nil -- remove it
      if vm ~= vt then
        print("texlive->" .. vt, "miktex->" .. vm, k)
      end
    else
      insert(mkmissing, {vt, k})
    end
  end
  for k, v in pairs(mtpkgdata) do
    print("texlive doesn't include " .. v .. " -> " .. k)
  end
  for _, v in ipairs(mkmissing) do
    print("miktex doesn't include " .. v[1] .. " -> " .. v[2])
  end
end

------------------------------------------------------------
--> \section{Generate json file from cwl files}
------------------------------------------------------------

local function insertNewValue(tbl, val)
  if not valueExists(tbl, val) then
    insert(tbl, val)
  end
end

local function extractFileData(cwl)
  -- the cwl files have different eol characters
  local deps = {}
  for d in cwl:gmatch("\n#include:(.-)[\r\n]") do
    --dbgPrint(d)
    n = d:match("^class-(.+)$")
    if n then
      insertNewValue(deps, n .. ".cls")
    else
      insertNewValue(deps, d .. ".sty")
    end
  end
  local envs = {}
  for b, e in cwl:gmatch("\n\\begin{(.-)}.-\n\\end{(.-)}") do
    if b == e then
      --dbgPrint("{" .. e .. "}")
      insert(envs, e)
    end
  end
  local cmds = {}
  for c in cwl:gmatch("\n\\(%a+)") do
    if c ~= "begin" and c ~= "end" then
      --dbgPrint("\\" .. c)
      if not valueExists(cmds, c) then
        insert(cmds, c)
      end
    end
  end
  --return {deps, envs, cmds}
  return {deps = deps, envs = envs, cmds = cmds}
end

local function writeJson(cwldata)
  tfpPrint("writing json database to file...")
  local tbl1 = {}
  for k, v in pairs(cwldata) do
    table.insert(tbl1, {k, v})
  end
  table.sort(tbl1, function(a, b)
    if a[1] < b[1] then return true end
  end)
  local tbl2 = {}
  for _, v in ipairs(tbl1) do
    local item = '"' .. v[1] .. '":' .. json.tostring(v[2])
    table.insert(tbl2, item)
  end
  local text = "{\n" .. table.concat(tbl2, "\n,\n") .. "\n}"
  fileWrite(text, "texfindpkg.json")
  fileWrite(gzip.compress(text), "texfindpkg.json.gz")
end

local cwlpath = "completion"

local function generateJsonData()
  local list = getFiles(cwlpath, "%.cwl$")
  local fname
  local cwldata = {}
  for _, v in ipairs(list) do
    local a, b = match(v, "^([^%-]-)%-(.+)%.cwl")
    if a == "class" then
      fname = b .. ".cls"
    else
      b = match(v, "^(.+)%.cwl")
      fname = b .. ".sty"
    end
    --print(fname)
    local cwl = fileRead(cwlpath .. "/" .. v)
    --print(cwl)
    if cwl then
      local item = extractFileData(cwl)
      dbgPrint(item)
      cwldata[fname] = item
    else
      tfpPrint("error in reading " .. v)
    end
  end
  writeJson(cwldata)
end

------------------------------------------------------------
--> \section{Install packages in current TeX distribution}
------------------------------------------------------------

local dist -- name of current tex distribution

local function initPackageDB()
  dist = testDistribution()
  tfpPrint("you are using " .. dist)
  if dist == "texlive" then
    tlReadPackageDB()
    tlParsePackageDB()
  else
    mtReadPackageDB()
    mtParsePackageDB()
  end
end

local function findOnePackage(fname)
  if dist == "texlive" then
    return tlpkgdata[fname]
  else
    return mtpkgdata[fname]
  end
end

local function tfpExecute(c)
  if os.type == "windows" then
    os.execute(c)
  else
    os.execute('sudo env "PATH=$PATH" ' .. c)
  end
end

local function installSomePackages(list)
  if not list then return end
  if dist == "texlive" then
    local p = table.concat(list, " ")
    tfpPrint("installing package " .. p)
    tfpExecute("tlmgr install " .. p)
  else
    for _, p in ipairs(list) do
      tfpPrint("installing package " .. p)
      tfpExecute("miktex packages install " .. p)
    end
  end
end

local function listSomePackages(list)
  if not list then return end
  local p = table.concat(list, " ")
  tfpPrint("please install " .. dist .. " package " .. p)
end

------------------------------------------------------------
--> \section{Find dependencies of package files}
------------------------------------------------------------

local tfptext = ""  -- the json text
local tfpdata = {}  -- the lua object
local fnlist = {}  -- file name list

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

local function findDependencies(fname)
  --print(fname)
  if valueExists(fnlist, fname) then return end
  local item = tfpdata[fname]
  if not item then
    tfpPrint("no dependency info for " .. fname)
    return
  end
  tfpPrint("finding dependencies for " .. fname)
  table.insert(fnlist, fname)
  local deps = item.deps
  if deps then
    for _, dname in ipairs(deps) do
      findDependencies(dname)
    end
  end
end

local function queryByFileName(fname)
  fnlist = {} -- reset the list
  findDependencies(fname)
  if #fnlist == 0 then
    tfpPrint("could not find any package with file " .. fname)
    return
  end
  local pkglist = {}
  for _, fn in ipairs(fnlist) do
    local pkg = findOnePackage(fn)
    --print(fn, pkg)
    if pkg then
      table.insert(pkglist, pkg)
    end
  end
  if not pkglist then
    tfpPrint("error in finding package in " .. dist)
    return
  end
  return pkglist
end

local function getFileNameFromCmdEnvName(cmdenv, name)
  --print(name)
  for line in tfptext:gmatch("(.-)\n[,}]") do
    if line:find('"' .. name .. '"') then
      --print(line)
      local fname, fspec = line:match('"(.-)":(.+)')
      --print(fname, fspec)
      local item = json.tolua(fspec)
      if valueExists(item[cmdenv], name) then
        tfpPrint("found package file " .. fname)
        return fname
      end
    end
  end
end

local function queryByCommandName(cname)
  --print(cname)
  local fname = getFileNameFromCmdEnvName("cmds", cname)
  if fname then
    return queryByFileName(fname)
  else
    tfpPrint("could not find any package with command \\" .. cname)
  end
end

local function queryByEnvironmentName(ename)
  --print(ename)
  local fname = getFileNameFromCmdEnvName("envs", ename)
  if fname then
    return queryByFileName(fname)
  else
    tfpPrint("could not find any package with environment {" .. ename .. "}")
  end
end

local function query(name)
  local h = name:sub(1,1)
  if h == "\\" then
    local b = name:sub(2)
    return queryByCommandName(b)
  elseif h == "{" then
    if name:sub(-1) == "}" then
      local b = name:sub(2,-2)
      return queryByEnvironmentName(b)
    else
      tfpPrint("invalid input " .. name)
    end
  else
    return queryByFileName(name)
  end
end

local function install(name)
  local list = query(name)
  installSomePackages(list)
end

------------------------------------------------------------
--> \section{Respond to user input}
------------------------------------------------------------

local helptext = [[
usage: texfindpkg <action> [<options>] [<name>]

Valid actions are:
   install      Install some package and its dependencies
   query        Query dependencies for some package

Valid options are:
   --help       Print this message and exit
   --version    Print version information and exit
]]

local function main()
  if arg[1] == nil then
    print(helptext)
  elseif arg[1] == "--help" then
    print(helptext)
  elseif arg[1] == "--version" then
    print("TeXFindPkg Version " .. tfpversion .. " (" .. tfpdate .. ")\n")
  elseif arg[1] == "install" then
    if arg[2] then
      initPackageDB()
      initDependencyDB()
      install(arg[2])
    else
      tfpPrint("missing the name of file/cmd/env!")
    end
  elseif arg[1] == "query" then
    if arg[2] then
      initPackageDB()
      initDependencyDB()
      local list = query(arg[2])
      listSomePackages(list)
    else
      tfpPrint("missing the name of file/cmd/env!")
    end
  elseif arg[1] == "generate" then
    generateJsonData()
  elseif arg[1] == "compare" then
    compareDistributions()
  else
    tfpPrint("unknown option " .. arg[1])
  end
end

main()
