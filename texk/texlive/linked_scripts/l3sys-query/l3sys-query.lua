#!/usr/bin/env texlua

--[[

File l3sys-query.lua Copyright (C) 2024 The LaTeX Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-----------------------------------------------------------------------

The development version of the bundle can be found at

   https://github.com/latex3/l3sys-query

for those people who are interested.

--]]

--
-- Details of the script itself, etc.
--
local copyright = "Copyright (C) 2024 The LaTeX Project"
local release_date = "2024-04-08"
local script_desc = "System queries for LaTeX using Lua\n"
local script_name = "l3sys-query"

--
-- Setup for the CLI: commands and options
--
local cmd_impl = {}
local cmd_desc = {}

local option_list =
  {
    exclude =
      {
        cmds = {"ls"},
        desc = "[<xarg>] Exclude entries from directory listing",
        type = "string"
      },
    ["ignore-case"] =
      {
        cmds = {"ls"},
        desc = "Ignore case when sorting directory listing",
        type = "boolean"
      },
    help =
      {
        desc  = "Prints this message and exits",
        short = "h",
        type  = "boolean"
      },
    pattern =
      {
        cmds  = {"ls"},
        desc  = "Treat (x)args as a Lua pattern",
        short = "p",
        type  = "boolean"
      },
    recursive =
      {
        cmds  = {"ls"},
        desc  = "Activates recursive directory listing",
        short = "r",
        type  = "boolean"
      },
    reverse =
      {
        cmds = {"ls"},
        desc = "Reversing sorting order",
        type = "boolean"
      },
    sort =
      {
        cmds = {"ls"},
        desc = "[name|date] Method used to sort directory listing",
        type = "string"
      },
    type = 
      {
        cmds = {"ls"},
        desc  = "[d|f] Select just the directories or files in a listing",
        type  = "string"
      },
    version =
      {
        desc = "Prints version information and exits",
        short = "v",
        type  = "boolean"
      }
  }

--
-- Local copies of globals used here
--
local io     = io
local stderr = io.stderr

local lfs        = lfs
local attributes = lfs.attributes
local currentdir = lfs.currentdir
local dir        = lfs.dir

local os   = os
local exit = os.exit

local string = string
local find   = string.find
local gmatch = string.gmatch
local lower  = string.lower
local match  = string.match
local rep    = string.rep
local sub    = string.sub

local table  = table
local concat = table.concat
local insert = table.insert
local sort   = table.sort

--
-- Support functions and data
--

-- Remove '...' around an entire text:
-- we need this to support restricted shell escape on Windows
local function dequote(text)
  if (match(text,"^'") and match(text,"'$")) then
    return sub(text,2,-2)
  end
  return text
end


-- A short auxiliary used whever the script bails out
local function info_and_quit(s)
  stderr:write("\n" .. s .. "\nTry '" .. script_name .. " --help' for more information.\n")
  exit(1)
end

-- Convert a file glob into a pattern for use by e.g. string.gub
-- Based on https://github.com/davidm/lua-glob-pattern
-- Simplified substantially: "[...]" syntax not supported as is not
-- required by the file patterns used by the team. Also note style
-- changes to match coding approach in rest of this file.
--
-- License for original globtopattern
--[[

   (c) 2008-2011 David Manura.  Licensed under the same terms as Lua (MIT).

--]]
local function glob_to_pattern(glob,skip_convert)
  if skip_convert then
    return glob
  end

  local pattern = "^" -- pattern being built
  local i = 0 -- index in glob
  local char -- char at index i in glob
  
  -- escape pattern char
  local function escape(char)
    return match(char,"^%w$") and char or "%" .. char
  end
  
  -- Convert tokens.
  while true do
    i = i + 1
    char = sub(glob,i,i)
    if char == "" then
      pattern = pattern .. "$"
      break
    elseif char == "?" then
      pattern = pattern .. "."
    elseif char == "*" then
      pattern = pattern .. ".*"
    elseif char == "[" then -- ]
      -- Ignored
      info_and_quit("[...] syntax not supported in globs!")
    elseif char == "\\" then
      i = i + 1
      char = sub(glob,i,i)
      if char == "" then
        pattern = pattern .. "\\$"
        break
      end
      pattern = pattern .. escape(char)
    else
      pattern = pattern .. escape(char)
    end
  end
  return pattern
end


-- Initial data for the command line parser
local cmd = ""
local options = {}
local arg_list = ""

local function parse_args()
  -- Turn long/short options into two lookup tables
  local long_options = {}
  local short_options = {}
  for k,v in pairs(option_list) do
    if v.short then
      short_options[v.short] = k
    end
    long_options[k] = k
  end

  -- Minor speed-up
  local arg = arg

  -- arg[1] is a special case: must be a command or a very limited
  -- subset (--help|-h or --version|-v)
  local a = arg[1]
  if a then
    -- No options are allowed in position 1, so filter those out
    if a == "--version" or a == "-v" then
      cmd = "version"
    elseif a == "--help" or a == "-h" then
      cmd = "help"
    elseif not match(a,"^%-") then
      cmd = a
    end
  end

  -- Stop here if help or version is required
  if cmd == "help" or cmd == "version" then
    return
  end

  -- An auxiliary to grab all file names into a string:
  -- this reflects the fact that the use case for l3sys-query doesn't
  -- allow quoting spaces
  local function tidy(num)
    local t = {}
    for i = num,#arg do
      insert(t,dequote(arg[i]))
    end
    return concat(t," ")
  end

  -- Examine all other arguments
  -- Use a while loop rather than for as this makes it easier
  -- to grab arg for optionals where appropriate
  local i = 2
  while i <= #arg do
    local a = arg[i]
    -- Terminate search for options
    if a == "--" then
      arg_list = tidy(i + 1)
      return
    end

    -- Look for optionals
    local opt
    local optarg
    local opts
    -- Look for and option and get it into a variable
    if match(a,"^%-") then
      if match(a,"^%-%-") then
        opts = long_options
        local pos = find(a,"=")
        if pos then
          opt = sub(a,3,pos - 1)
          optarg = sub(a,pos + 1)
        else
          opt = sub(a,3)
        end
      else
        opts = short_options
        opt = sub(a,2,2)
        -- Only set optarg if it is there
        if #a > 2 then
          optarg = sub(a,3)
        end
      end

      -- Now check that the option is valid and sort out the argument
      -- if required
      local optname = opts[opt]
      if optname then
        -- Tidy up arguments
        if option_list[optname].type == "boolean" then
          if optarg then
            local opt = "-" .. (match(a,"^%-%-") and "-" or "") .. opt
            info_and_quit("Value not allowed for option " .. opt)
          end
        else
          if not optarg then
            optarg = arg[i + 1]
            if not optarg then
              info_and_quit("Missing value for option " .. a)
            end
            i = i + 1
          end
        end
      else
        info_and_quit("Unknown option " .. a)
      end

      -- Store the result
      if optarg then
        if option_list[optname].type == "string" then
          options[optname] = optarg
        else
          local opts = options[optname] or {}
          for hit in gmatch(optarg,"([^,%s]+)") do
            insert(opts,hit)
          end
          options[optname] = opts
        end
      else
        options[optname] = true
      end
      i = i + 1
    end

    -- Collect up the remaining arguments
    if not opt then
      arg_list = tidy(i)
      break
    end
  end
end

parse_args()

--
-- The help functions: local only and hard-coded
--

local function help()
  -- Find the longest entry to pad
  local function format_list(list)
    local longest = 0
    for k,_ in pairs(list) do
      if k:len() > longest then
        longest = k:len()
      end
    end
    -- Sort the list
    local t = {}
    for k,_ in pairs(list) do
      insert(t,k)
    end
    sort(t)
    return t,longest
  end

  -- 'Header' of fixed info
  print("Usage: " .. script_name .. " <cmd> [<options>] [<args>]\n")
  print("Valid <cmd>s are:")

  -- Sort the commands, pad the descriptions, print
  local t,longest = format_list(cmd_desc)
  for _,k in ipairs(t) do
    local cmd = cmd_desc[k]
    local filler = rep(" ",longest - k:len() + 1)
    print("   " .. k .. filler .. cmd)
  end

  -- Same for the options
  print("\nValid <options> are:")
  t,longest = format_list(option_list)
  for _,name in ipairs(t) do
    local opt = option_list[name]
    local filler = rep(" ",longest - name:len() + 1)
    if opt.desc then
      if opt.short then
        print("   --" .. name .. "|-" .. opt.short .. filler .. opt.desc)
      else
        print("   --" .. name .. "   " .. filler .. opt.desc)
      end
    end
  end
  
  -- Postamble
  print("\nFull manual available via 'texdoc " .. script_name .. "'.\n")
  print("Repository : https://github.com/latex3/" .. script_name)
  print("Bug tracker: https://github.com/latex3/" .. script_name .. "/issues")
  print("\n" .. copyright)
end

local function version()
  print(script_name .. " " .. release_date)
end

--
-- The functions for commands: all held in the table cmd_impl
-- with docstrings in the table cmd_desc.
--

-- The aim here is to convert a user file specification (if given) into a 
-- Lua pattern, and then to do a listing.
cmd_desc.ls = "Prints a listing based on the <args> and <options>"
function cmd_impl.ls(arg_list)
  if not arg_list or arg_list == "" or arg_list == "." then
    arg_list = "*"
  end
  -- Look for absolute paths or any trying to leave the confines of the current
  -- directory: this is not supported.
  if match(arg_list,"%.%.") or 
     match(arg_list,"^/") or 
     match(arg_list,"^\\") or 
     match(arg_list,"[a-zA-Z]:") then
    return
  end
  -- Tidy up and convert to a pattern.
  path,glob = match(arg_list,"^(.*)/([^/]*)$")
  if not path then
    path = "."
    glob = arg_list
  end
  if path == "" then
    path = "."
  end
  if not match(path,"^%.") then
    path = "./" .. path
  end
  local conv_pattern = options.pattern
  local pattern = glob_to_pattern(glob,conv_pattern)
  local exclude_pattern
  if options.exclude then
    local exclude = dequote(options.exclude)
    exclude_pattern = glob_to_pattern(exclude,conv_pattern)
  end
  -- A lookup table for attributes: map between lfs- and Unix-type naming
  local attrib_map = {d = "directory", f = "file"}

  -- A bit of setup to store the sorting data as well as the entry itself
  local i = 0 -- If no sorting active, just track the order from lfs
  local entries = {}
  local sort_mode = options.sort or "none"
  local function store(entry,full_entry)
    if not match(entry,pattern)
      or (exclude_pattern and match(entry,exclude_pattern)) then
      return
    end
    i = i + 1
    local key = i
    if sort_mode == "date" then
      key = attributes(full_entry,"modification")
    elseif sort_mode == "name" then
      key = full_entry
    end
    entries[key] = full_entry
  end

  -- Build a table of entries, excluding "." and "..", and return as a string
  -- with one entry per line.
  local opt = options.type
  local is_rec = options.recursive
  local function browse(path)
    for entry in dir(path) do
      if entry ~= "." and entry ~= ".." and not match(entry,"^%.") then
        local full_entry = path .. "/" .. entry
        local ft = attributes(full_entry,"mode")
        if not opt or ft == attrib_map[opt] then
          store(entry,full_entry)
        end
        if is_rec and ft == "directory" then
          browse(full_entry)
        end
      end
    end
  end
  -- Start a search at the top level
  browse(path)

  -- Extract keys and sort
  local s = {}
  for k,_ in pairs(entries) do
    insert(s,k)
  end

  -- Setup for case-insensitve sorting
  local function case(s)
      return s
  end
  if options["ignore-case"] then
    function case(s)
      return lower(s)
    end
  end

  if options.reverse then
    sort(s,function(a,b) return case(a) > case(b) end)
  else
    sort(s,function(a,b) return case(a) < case(b) end)
  end

  local result = {}
  for _,v in ipairs(s) do
      insert(result,entries[v])
  end

  return concat(result,"\n")
end

-- A simple rename
cmd_desc.pwd = "Prints the present working directory"
function cmd_impl.pwd()
  return currentdir()
end

--
-- Execute the given command
--
if cmd == "help" then
  help()
  exit(0)
elseif cmd == "version" then
  version()
  exit(0)
-- Only 'known' commands do anything at all
elseif not cmd_impl[cmd] then
  if cmd == "" then
    info_and_quit(script_name .. ": No " .. script_name .. " command specified.")
  else
    info_and_quit(script_name .. ": '" .. cmd .. "' is not a " .. script_name ..
      " command.")
  end
  exit(1)
end

-- Check options are valid for cmd
for k,_ in pairs(options) do
  local cmds = option_list[k].cmds
  if k == "help" then
    help()
    exit(0)
  elseif k == "version" then
    version()
    exit(0)
  elseif not cmds then
    -- Should not be possible:
    -- everything except --help and --version should have an entry
    print("Internal error")
    exit(1)
  else
    -- Make a lookup table
    local t = {}
    for _,v in pairs(cmds) do
      t[v] = true
    end
    if not t[cmd] then
      info_and_quit(script_name .. ": Option '" .. k .. 
        "' does not apply to '"  .. cmd .. "'")
    end
  end
end

local result = cmd_impl[cmd](arg_list)

if result then
  print(result)
  exit(0)
else
  exit(1)
end
