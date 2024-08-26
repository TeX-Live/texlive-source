#!/usr/bin/env texlua
--
--  $Id: luafindfont.lua 961 2024-08-25 16:47:59Z herbert $
-----------------------------------------------------------------------
--         FILE:  luafindfont.lua
--  DESCRIPTION:  search for fonts in the database
-- REQUIREMENTS:  luatex v.0.80 or later; packages lualibs, xindex-lapp
--       AUTHOR:  Herbert Voß  (C) 2023-06-21
-----------------------------------------------------------------------
        luafindfont = luafindfont or { }
      local version = 0.14
luafindfont.version = version

--[[
Search the font database for fontnames. The database is used
by Lua(La)TeX and created by default with the first run of
Lua(La)TeX. If there is no such data file then "luafindfont" 
will execute the command to create it.

luafindfont(1)

%% This file may be distributed and/or modified under the
%% conditions of the LaTeX Project Public License, either version 1.3c
%% of this license or (at your option) any later version.
%% The latest version of this license is in
%%    http://www.latex-project.org/lppl.txt
%% and version 1.3c or later is part of all distributions of LaTeX
%% version 2005/12/01 or later.

Report bugs to  hvoss@tug.org

]]

-- equivilant with ConTeXt, e.g. times
-- mtxrun --script font --list --name --all --pattern=times

kpse.set_program_name("luatex")
local f = kpse.find_file("lualibs.lua")

require("lualibs")  -- all part of LuaTeX

if #arg == 0 then
  print("I need at least one argument or option! Will exit ...")
  os.exit()
end

local args_verbose = 0
local args_nosymbolicnames = false
local args_otfinfo = 0
local args_info = 0
local args_xetex = 0
local args_max_string = 90

local otfinfo_arg = ""
local mtxrun = 0
local fontNo = 0

local i = 1
while i <= #arg do
  if arg[i] == "-h" or arg[i] == "--help" then
    print("Version "..version..", Copyright 2021-23 by Herbert Voß") 
    print([[Syntax: luafindfont [options] <font> 
    By default the Lua program 'luafindfont' creates a list of the
    fonts which have in its names the given string.  

    parameter handling
    -h,--help
    -n,--nosymbolicnames
      ,--no-symbolic-names
    -o,--otfinfo (default 0)
    -i,--info (default 0)
    -I,--Info (default 0)
    -x, --xetex 
    -v, --verbose
    -V, --version
    -m,--max_string (default 90)
    <font> (string)  ]])
  elseif arg[i] == "-V" or arg[i] == "--version" then
    print("version "..version)
    os.exit()
  elseif arg[i] == "-v" or arg[i] == "--verbose" then
    args_verbose = 1
  elseif (arg[i] == "-n") or (arg[i] == "--nosymbolicnames") or (arg[i] == "--no-symbolic-names") then
    args_nosymbolicnames = true
  elseif arg[i] == "-x" or arg[i] == "--xetex" then
    args_xetex = 1
  elseif arg[i] == "-o" or arg[i] == "--otfinfo" then
    local o_arg = arg[i+1]
    otfinfo_arg = "i"
    fontNo = tonumber(o_arg)
    if not fontNo then                      -- combination: No and Arg
      fontNo = tonumber(string.match(o_arg,"%d+"))
      otfinfo_arg = string.match(o_arg,"%a+")
      if not fontNo then
        print("Option -o needs a following fontnumber!")
        fontNo = 0
      end
    end
    i = i + 1
  elseif arg[i] == "-i" or arg[i] == "--info" then
    local fontNr = tonumber(arg[i+1])
    if fontNr then
      args_info = fontNr
      i = i + 1
    else
      print("Option -i needs a following fontnumber!")
      args_info = 0
    end
  elseif arg[i] == "-I" or arg[i] == "--Info" then
    mtxrun = 1
    local I_arg = arg[i+1]
    fontNo = tonumber(I_arg)
    if not fontNo then
      print("Option -I needs a following fontnumber!")
      fontNo = 0
    end
    i = i + 1
  elseif arg[i] == "-m" or arg[i] == "--max_string" then
    local string_len = tonumber(arg[i+1])
    if string_len then
      args_max_string = string_len
      i = i + 1
    else
      print("Option -m needs a following fontnumber!")
      args_max_string = 90
    end
  else
    args_font = arg[i]
  end    
  i = i + 1
end

local vlevel = args_verbose

function logprint(str)
  if vlevel > 0 then print(str) end
end

if vlevel > 0 then
  print("Parameter:")
  print("  args_verbose = "..args_verbose)
  print("  args_nosymbolicnames = "..tostring(args_nosymbolicnames))
  print("  args_xetex = "..args_xetex)
  print("  otfinfo_arg = "..otfinfo_arg)
  print("  fontNo = "..fontNo)
  print("  args_max_string = "..args_max_string)
end
  
if not args_font then
  print("No fontname given, will close ...")
  os.exit()
end

--local otfinfo = args_otfinfo
local info = args_info
local Info = args_Info
local noSymbolicNames = args_nosymbolicnames
local maxStrLength = args_max_string
local font_str = args_font:lower():gsub("%s+", ""):split("&")
if #font_str == 1 then font_str[2] = "" end

local luaVersion = _VERSION
if vlevel > 0 then 
  print("We are using "..luaVersion)
  if font_str[2] ~= "" then
     print('Looking for font \"'..font_str[1]..' & '..font_str[2]..'\"')
  else 
     print('Looking for font \"'..font_str[1]..'\"')
  end
end

function getFileParts(fullpath,part)
  local path, file, ext = string.match(fullpath, "(.-)([^/]-([^%.]+))$")
  if part == "path" then return path 
  elseif part == "ext" then return ext
  else return file end
end

-- for fileparts see also file fontloader-l-file.lua in /luaotfload

function getFileLocation()
  local cachepaths = kpse.expand_var('$TEXMFCACHE') or ""
  if cachepaths == "" or cachepaths == "$TEXMFCACHE" then
    cachepaths = kpse.expand_var('$TEXMFVAR') or ""
  end
  logprint("cachepaths: "..cachepaths)
  if cachepaths == "" then
    print("umghhh ....")
    print("No cache path found ... ")
    return ""
  end  
  local windows = (os.type == "windows")
  if windows then logprint ("System: Windows")
             else logprint ("System: macOS or Linux")
  end
  if windows then
    paths = string.split(cachepaths,";")
  else
    paths = string.split(cachepaths,":")
  end
  logprint ("Paths: [1]"..paths[1])
  if #paths > 1 then
    logprint("       [2]"..paths[2])
  end
  local file = paths[1].."/luatex-cache/generic/names" 
  logprint("try path: "..file)
  local f,err = io.open (file.."/test.tmp", "w") 
  if not f and #paths > 1 then
    logprint("first path has no file, I'll try the second one, if exists ...")
    file = paths[2].."/luatex-cache/generic/names"
    logprint("try path: "..file)
    f,err = io.open (file.."/test.tmp", "w") 
    if not f then
      print("Error getting file location: \n",err)
      return ""
    else
      f:close()
    end
  else
    f:close()
  end
-- print("File: "..fontListFile)
  return file
end

function readBinaryOrZippedFile(file)
  logprint("Check for file "..file..".luc.gz")
  local f,err = io.open (file..".luc.gz", "rb") 
  if f then
    logprint("Found a zipped binary data file ... ") 
    local chunk = gzip.decompress(f:read"*all")
    f:close()
    local func = load (chunk, "b")
    str = func()
    return str
  end
  logprint("There is no zipped binary data file ... ") 
  logprint("Check for unzipped file "..file..".luc")
  local f,err = io.open (file..".luc", "rb") 
  if f then
    logprint("Found a binary data file ... ") 
    local chunk = f:read"*all"
    f:close()
    local func = load (chunk, "b")
    str = func()
    return str
  end
  logprint("There is no binary data file ... ") 
  logprint("Check for zipped file "..file..".lua.gz")
  f,err = io.open (file..".lua.gz", "rb") 
  if f then
    logprint("Found a gzipped data file ... ")
    local str = f:read("*all")
    local str2 = loadstring(gzip.decompress(str))
    str = str2()
    f:close()
    return str
  end
  logprint("There is no gzipped data file ... ") 
  logprint("Check for file "..file..".lua")
  f,err = io.open (file..".lua", "r") 
  if f then
    logprint("Found a normal data file ... ")
    local str = dofile(f)
    f:close()
    return str
  else
    logprint("There is no data file ... ")
    print("Error reading file: ",err)
    return nil
  end
end

function compareEntries(f1, f2)
  if (f1["basename"]   == f2["basename"])   and
     (f1["familyname"] == f2["familyname"]) and
     (f1["fullpath"]   == f2["fullpath"]) then
     return true 
  else 
    return false
  end
end

function centerText(text, width)
  if text == nil then return "" end
  local spaces = math.floor((width-string.len(text))/2)
  local len = string.len(text)
  return ((" "):rep(spaces)..text..(" "):rep(spaces))
end

local fontData = {}
local fontListFile = getFileLocation()
if fontListFile == "" then
    print("There is no cached font file list!")
    print('Will run at first "luaotfload-tool --update --force" ... Wait a minute, please ...')
    local exrun = io.popen('luaotfload-tool --update --force', 'r')
    local output = exrun:read('*all')
    print(output)
    exrun:close()
    fontListFile = getFileLocation()
end

fontListFile = fontListFile.."/luaotfload-names"
fontData = readBinaryOrZippedFile(fontListFile)

if not fontData then   
  print("umghhh ....")
  print("It does not work! I cannote find the base data file ... I'll give it up ... :-(")
  os.exit()
end

--print(require 'xindex-pretty'.dump(fontData)) --["families"]["system"]["otf"]))

fontDataMap = fontData["mappings"]
fontFilesTable = fontData["files"]["full"]

--print(require 'xindex-pretty'.dump(fontFilesTable)) --["families"]["system"]["otf"]))
--print(require 'xindex-pretty'.dump(fontFilesTable["bare"]["system"]["otf"])) --["families"]["system"]["otf"]))

--[[
print("Dateiliste für System->OTF")
for i, v in ipairs(fontFilesTable) do 
  print(i,v)
end
]]

table.sort(fontDataMap, 
    function(a,b) 
	if not a["basename"] or not b["basename"] then 
	    return false 
	else 
	    return string.lower(a["basename"]) < string.lower(b["basename"]) 
	end 
end)

-- strip duplicates
local newFontDataMap = {}
if #fontDataMap > 0 then
    newFontDataMap[1] = fontDataMap[1]
end
for i = 2,#fontDataMap do
    if not compareEntries(fontDataMap[i],newFontDataMap[#newFontDataMap]) then
	newFontDataMap[#newFontDataMap+1] = fontDataMap[i]
    end
end

fontDataMap = newFontDataMap
--for i, v in ipairs(fontDataMap) do 
--  print(i, v["basename"],v["familyname"], v["fullpath"]) 
-- end

local j = 1
local fontList = {}
-- now calculate the longest string for all colums
local l_max = {1, 1, 1}
for i, v in ipairs(fontDataMap) do 
  if v["familyname"] then
      if (string.find (v["familyname"]:lower(), font_str[1], 1, true)  and string.find (v["basename"]:lower(), font_str[2], 1, true) ) or (font_str[1] == "*") then
--	print(string.format("%2d. %30s %20s  %50s",j,v["basename"],v["familyname"],v["fullpath"])) 
        fontList[#fontList+1] = v
        local fullpath = getFileParts(v["fullpath"],"path")  -- strip file name
        local basename = v["basename"]
--      local basename = string.fromutf8(v["basename"])
        if string.len(basename) > l_max[1] then l_max[1] = string.len(basename) end
        if string.len(v["familyname"]) > l_max[2] then l_max[2] = string.len(v["familyname"]) end
        if string.len(fullpath) > l_max[3] then l_max[3] = string.len(fullpath) end
	j = j + 1
      end
  end
end

if #fontList == 0 then
  print("There are no fonts with the given search name!\n")
  os.exit()
end


-- print(l_max[1],l_max[2],l_max[3])
if l_max[3] > maxStrLength then l_max[3] = maxStrLength end

local minChars = 26
local Fontname = "Filename"
local Path = "Path"
local SymbolicName = "Symbolic"
local lfdNr = "No."

if (font_str ~= "*") and not noSymbolicNames then
  if args_xetex > 0 then
--    print(string.format("%5s %"..l_max[1].."s %"..l_max[2].."s  %"..l_max[3].."s".."%4s",lfdNr,Fontname,SymbolicName,Path,"X")) 
    io.write(string.format("%5s",lfdNr))
    io.write(centerText(Fontname,l_max[1]))
    io.write(centerText(SymbolicName,l_max[2]))
    io.write(centerText(Path,l_max[3]))
    print("X") 
  else      
--    print(string.format("%5s %"..l_max[1].."s %"..l_max[2].."s  %"..l_max[3].."s",lfdNr,Fontname,SymbolicName,Path)) 
    io.write(string.format("%5s",lfdNr))
    io.write(centerText(Fontname,l_max[1]))
    io.write(centerText(SymbolicName,l_max[2]))
    print(centerText(Path,l_max[3]))
  end
else
  if args_xetex > 0 then
--    print(string.format("%5s %"..l_max[1].."s  %"..l_max[3].."s".."%4s",lfdNr,Fontname,Path,"X")) 
    io.write(string.format("%5s",lfdNr))
    io.write(centerText(Fontname,l_max[1]))
    io.write(centerText(SymbolicName,l_max[2]))
    io.write(centerText(Path,l_max[3]))
    print("X")
  else
--    print(string.format("%5s %"..l_max[1].."s  %"..l_max[3].."s",lfdNr,Fontname,Path)) 
    io.write(string.format("%5s",lfdNr))
    io.write(centerText(Fontname,l_max[1]))
    io.write(centerText(SymbolicName,l_max[2]))
    print(centerText(Path,l_max[3]))
  end
end

local kpsewhich = "0" -- test if font is present for xetex
for i, v in ipairs(fontList) do
  local path = getFileParts(v["fullpath"],"path")
  if string.len(path) > l_max[3] then
    path = string.sub (path, 1, minChars).."..."..string.sub (path, string.len(path)-maxStrLength+minChars+4)    
  end
  if args_xetex > 0 then
    kpsewhich = "0"
    local exrun = io.popen("kpsewhich "..v["basename"],'r')
    if exrun then
      if string.len(exrun:read('*all')) > 0 then
        kpsewhich = "1"
      end
    else
      print("!!! There maybe a problem with font "..v["basename"].." kpsewhich doesn't work")
      print(tostring(exrun))
	  kpsewhich = "0"
	  os.exit()
    end
    exrun:close()
  end
  if (font_str ~= "*") and not noSymbolicNames then
    if args_xetex > 0 then
      print(string.format("%4d. %"..l_max[1].."s %"..l_max[2].."s  %"..l_max[3].."s".." %3s",i,v["basename"],v["familyname"],path,kpsewhich)) 
    else
      print(string.format("%4d. %"..l_max[1].."s %"..l_max[2].."s  %"..l_max[3].."s",i,v["basename"],v["familyname"],path)) 
    end
  else
    if args_xetex > 0 then
      print(string.format("%4d. %"..l_max[1].."s  %"..l_max[3].."s".." %3s",i,v["basename"],path,kpsewhich)) 
    else    
      print(string.format("%4d. %"..l_max[1].."s  %"..l_max[3].."s",i,v["basename"],path)) 
    end
  end
end

if fontNo > 0 and mtxrun == 0 then
  print()
  if fontNo > #fontList then
  	print("given font number is greater than the number of the fontlist!")
  else
    print("Running otfinfo -"..otfinfo_arg.." on font no."..fontNo)
    local font = fontList[fontNo]["fullpath"]
    print("otfinfo -"..otfinfo_arg.." \""..font.."\"")
    local exrun = io.popen("otfinfo -"..otfinfo_arg.." \""..font.."\"", 'r') -- ".." font may have spaces
    local output = exrun:read('*all')
    print(output)
    exrun:close()
  end
end

if info > 0 then
  if info > #fontList then
  	print("given font number is greater than the number of the fontlist!")
  else
    font = fontList[info]["familyname"]
    print("\nFont: "..font)
    local font_dir = {"local","system","texmf"}
    local font_ext = {"ttf","otf","ttc"}
    for j = 1,#font_dir do
      for i = 1,#font_ext do
        local fonttype = ""
        local ext = font_ext[i]
        local dir = font_dir[j]
        if fontData["families"][dir][ext]  then  -- font extension exists?
          local entry = fontData["families"][dir][ext][font]
          if entry then
            if entry["r"]  then fonttype = fonttype.."Regular "      end
            if entry["b"]  then fonttype = fonttype.."| Bold "       end
            if entry["i"]  then fonttype = fonttype.."| Italic "     end
            if entry["bi"] then fonttype = fonttype.."| BoldItalic"  end
            io.write("Fonttype "..ext.."("..dir..") --> ")
            if #fonttype > 0 then
              print("| "..fonttype.." |")
            else
              print(" undefined ") --- no regular definiert
            end
          else
            print()
          end
        end
      end
    end
  end
end

if mtxrun > 0 then
  print()
  print("Running mtxrun on font no."..fontNo)
--  local font = fontList[fontNo]["fullpath"]
  local font = fontList[fontNo]["basename"]
  print("mtxrun --script fonts --list --info --file \""..font.."\"")
  local exrun = io.popen("mtxrun --script fonts --list --info --file \""..font.."\"", 'r') -- ".." font may have spaces
  local output = exrun:read('*all')
  print(output)
  exrun:close()
end

--print(require 'xindex-pretty'.dump(fontData["families"]["system"]["otf"])) --["families"]["system"]["otf"]))


--[[

 ["families"]={
  ["local"]={},
  ["system"]={
   ["otf"]={
    ["adobecaslonpro"]={
     ["b"]={
      ["default"]=3143,
     },
     ["bi"]={
      ["default"]=3146,
     },
     ["i"]={
      ["default"]=3145,
     },
     ["r"]={
      ["default"]=3147,
     },
    },
   },
  },
 },

]]

--[[
This is a sketch of the luaotfload db:

    type dbobj = {
        families    : familytable;
        fontnames   : fontnametable;
        files       : filemap;
        status      : filestatus;
        mappings    : fontentry list;
        meta        : metadata;
    }
    and familytable = {
        local  : (format, familyentry) hash; // specified with include dir
        texmf  : (format, familyentry) hash;
        system : (format, familyentry) hash;
    }
    and familyentry = {
        r  : sizes; // regular
        i  : sizes; // italic
        b  : sizes; // bold
        bi : sizes; // bold italic
    }
    and sizes = {
        default : int;              // points into mappings or names
        optical : (int, int) list;  // design size -> index entry
    }
    and fontnametable = {
        local  : (format, index) hash;
        texmf  : (format, index) hash;
        system : (format, index) hash;
    }
    and metadata = {
        created     : string       // creation time
        formats     : string list; // { "otf", "ttf", "ttc" }
        local       : bool;        (* set if local fonts were added to the db *)
        modified    : string       // modification time
        statistics  : TODO;        // created when built with "--stats"
        version     : float;       // index version
    }
    and filemap = { // created by generate_filedata()
        base : {
            local  : (string, int) hash; // basename -> idx
            system : (string, int) hash;
            texmf  : (string, int) hash;
        };
        bare : {
            local  : (string, (string, int) hash) hash; // location -> (barename -> idx)
            system : (string, (string, int) hash) hash;
            texmf  : (string, (string, int) hash) hash;
        };
        full : (int, string) hash; // idx -> full path
    }
    and fontentry = { // finalized by collect_families()
        basename             : string;   // file name without path "foo.otf"
        conflicts            : { barename : int; basename : int }; // filename conflict with font at index; happens with subfonts
        familyname           : string;   // sanitized name of the font family the font belongs to, usually from the names table
        fontname             : string;   // sanitized name of the font
        format               : string;   // "otf" | "ttf" | "afm" (* | "pfb" *)
        fullname             : string;   // sanitized full name of the font including style modifiers
        fullpath             : string;   // path to font in filesystem
        index                : int;      // index in the mappings table
        italicangle          : float;    // italic angle; non-zero with oblique faces
        location             : string;   // "texmf" | "system" | "local"
        plainname            : string;   // unsanitized font name
        typographicsubfamily : string;   // sanitized preferred subfamily (names table 14)
        psname               : string;   // PostScript name
        size                 : (false | float * float * float);  // if available, size info from the size table converted from decipoints
        subfamily            : string;   // sanitized subfamily (names table 2)
        subfont              : (int | bool);     // integer if font is part of a TrueType collection ("ttc")
        version              : string;   // font version string
        weight               : int;      // usWeightClass
    }
    and filestatus = (string,       // fullname
                      { index       : int list; // pointer into mappings
                        timestamp   : int;      }) dict
]]
