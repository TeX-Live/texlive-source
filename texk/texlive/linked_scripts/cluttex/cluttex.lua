#!/usr/bin/env texlua
local io, os, string, table, package, require, assert, error, ipairs, type, select, arg = io, os, string, table, package, require, assert, error, ipairs, type, select, arg
os.type = os.type or "unix"
if lfs and not package.loaded['lfs'] then package.loaded['lfs'] = lfs end
if os.type == "windows" then
package.preload["texrunner.pathutil"] = function(...)
--[[
  Copyright 2016 ARATA Mizuki

  This file is part of ClutTeX.

  ClutTeX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ClutTeX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ClutTeX.  If not, see <http://www.gnu.org/licenses/>.
]]

-- pathutil module

local assert = assert
local select = select
local string = string
local string_find = string.find
local string_sub = string.sub
local string_match = string.match
local string_gsub = string.gsub
local filesys = require "lfs"

local function basename(path)
  local i = 0
  while true do
    local j = string_find(path, "[\\/]", i + 1)
    if j == nil then
      return string_sub(path, i + 1)
    elseif j == #path then
      return string_sub(path, i + 1, -2)
    end
    i = j
  end
end


local function dirname(path)
  local i = 0
  while true do
    local j = string_find(path, "[\\/]", i + 1)
    if j == nil then
      if i == 0 then
        -- No directory portion
        return "."
      elseif i == 1 then
        -- Root
        return string_sub(path, 1, 1)
      else
        -- Directory portion without trailing slash
        return string_sub(path, 1, i - 1)
      end
    end
    i = j
  end
end


local function parentdir(path)
  local i = 0
  while true do
    local j = string_find(path, "[\\/]", i + 1)
    if j == nil then
      if i == 0 then
        -- No directory portion
        return "."
      elseif i == 1 then
        -- Root
        return string_sub(path, 1, 1)
      else
        -- Directory portion without trailing slash
        return string_sub(path, 1, i - 1)
      end
    elseif j == #path then
      -- Directory portion without trailing slash
      return string_sub(path, 1, i - 1)
    end
    i = j
  end
end


local function trimext(path)
  return (string_gsub(path, "%.[^\\/%.]*$", ""))
end


local function ext(path)
  return string_match(path, "%.([^\\/%.]*)$") or ""
end


local function replaceext(path, newext)
  local newpath, n = string_gsub(path, "%.([^\\/%.]*)$", function() return "." .. newext end)
  if n == 0 then
    return newpath .. "." .. newext
  else
    return newpath
  end
end


local function joinpath2(x, y)
  local xd = x
  local last = string_sub(x, -1)
  if last ~= "/" and last ~= "\\" then
    xd = x .. "\\"
  end
  if y == "." then
    return xd
  elseif y == ".." then
    return dirname(x)
  else
    if string_match(y, "^%.[\\/]") then
      return xd .. string_sub(y, 3)
    else
      return xd .. y
    end
  end
end

local function joinpath(...)
  local n = select("#", ...)
  if n == 2 then
    return joinpath2(...)
  elseif n == 0 then
    return "."
  elseif n == 1 then
    return ...
  else
    return joinpath(joinpath2(...), select(3, ...))
  end
end


-- https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx
local function isabspath(path)
  local init = string_sub(path, 1, 1)
  return init == "\\" or init == "/" or string_match(path, "^%a:[/\\]")
end

local function abspath(path, cwd)
  if isabspath(path) then
    -- absolute path
    return path
  else
    -- TODO: relative path with a drive letter is not supported
    cwd = cwd or filesys.currentdir()
    return joinpath2(cwd, path)
  end
end

return {
  basename = basename,
  dirname = dirname,
  parentdir = parentdir,
  trimext = trimext,
  ext = ext,
  replaceext = replaceext,
  join = joinpath,
  abspath = abspath,
}
end
else
package.preload["texrunner.pathutil"] = function(...)
--[[
  Copyright 2016 ARATA Mizuki

  This file is part of ClutTeX.

  ClutTeX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ClutTeX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ClutTeX.  If not, see <http://www.gnu.org/licenses/>.
]]

-- pathutil module for *nix

local assert = assert
local select = select
local string = string
local string_find = string.find
local string_sub = string.sub
local string_match = string.match
local string_gsub = string.gsub
local filesys = require "lfs"

local function basename(path)
  local i = 0
  while true do
    local j = string_find(path, "/", i + 1, true)
    if j == nil then
      return string_sub(path, i + 1)
    elseif j == #path then
      return string_sub(path, i + 1, -2)
    end
    i = j
  end
end


local function dirname(path)
  local i = 0
  while true do
    local j = string_find(path, "/", i + 1, true)
    if j == nil then
      if i == 0 then
        -- No directory portion
        return "."
      elseif i == 1 then
        -- Root
        return "/"
      else
        -- Directory portion without trailing slash
        return string_sub(path, 1, i - 1)
      end
    end
    i = j
  end
end


local function parentdir(path)
  local i = 0
  while true do
    local j = string_find(path, "/", i + 1, true)
    if j == nil then
      if i == 0 then
        -- No directory portion
        return "."
      elseif i == 1 then
        -- Root
        return "/"
      else
        -- Directory portion without trailing slash
        return string_sub(path, 1, i - 1)
      end
    elseif j == #path then
      -- Directory portion without trailing slash
      return string_sub(path, 1, i - 1)
    end
    i = j
  end
end


local function trimext(path)
  return (string_gsub(path, "%.[^/%.]*$", ""))
end


local function ext(path)
  return string_match(path, "%.([^/%.]*)$") or ""
end


local function replaceext(path, newext)
  local newpath, n = string_gsub(path, "%.([^/%.]*)$", function() return "." .. newext end)
  if n == 0 then
    return newpath .. "." .. newext
  else
    return newpath
  end
end


local function joinpath2(x, y)
  local xd = x
  if string_sub(x, -1) ~= "/" then
    xd = x .. "/"
  end
  if y == "." then
    return xd
  elseif y == ".." then
    return dirname(x)
  else
    if string_sub(y, 1, 2) == "./" then
      return xd .. string_sub(y, 3)
    else
      return xd .. y
    end
  end
end

local function joinpath(...)
  local n = select("#", ...)
  if n == 2 then
    return joinpath2(...)
  elseif n == 0 then
    return "."
  elseif n == 1 then
    return ...
  else
    return joinpath(joinpath2(...), select(3, ...))
  end
end


local function abspath(path, cwd)
  if string_sub(path, 1, 1) == "/" then
    -- absolute path
    return path
  else
    cwd = cwd or filesys.currentdir()
    return joinpath2(cwd, path)
  end
end


return {
  basename = basename,
  dirname = dirname,
  parentdir = parentdir,
  trimext = trimext,
  ext = ext,
  replaceext = replaceext,
  join = joinpath,
  abspath = abspath,
}
end
end
if os.type == "windows" then
package.preload["texrunner.shellutil"] = function(...)
--[[
  Copyright 2016,2019 ARATA Mizuki

  This file is part of ClutTeX.

  ClutTeX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ClutTeX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ClutTeX.  If not, see <http://www.gnu.org/licenses/>.
]]

local string_gsub = string.gsub
local os_execute = os.execute

-- s: string
local function escape(s)
  return '"' .. string_gsub(string_gsub(s, '(\\*)"', '%1%1\\"'), '(\\+)$', '%1%1') .. '"'
end


local function has_command(name)
  local result = os_execute("where " .. escape(name) .. " > NUL 2>&1")
  -- Note that os.execute returns a number on Lua 5.1 or LuaTeX
  return result == 0 or result == true
end

return {
  escape = escape,
  has_command = has_command,
  cd = "cd /d",
}
end
else
package.preload["texrunner.shellutil"] = function(...)
--[[
  Copyright 2016,2019 ARATA Mizuki

  This file is part of ClutTeX.

  ClutTeX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ClutTeX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ClutTeX.  If not, see <http://www.gnu.org/licenses/>.
]]

local assert = assert
local string_match = string.match
local table = table
local table_insert = table.insert
local table_concat = table.concat
local os_execute = os.execute

-- s: string
local function escape(s)
  if s == "" then
    return "''"
  end
  local len = #s
  local result = {}
  local t,i = string_match(s, "^([^']*)()")
  assert(t)
  if t ~= "" then
    table_insert(result, "'")
    table_insert(result, t)
    table_insert(result, "'")
  end
  while i < len do
    t,i = string_match(s, "^('+)()", i)
    assert(t)
    table_insert(result, '"')
    table_insert(result, t)
    table_insert(result, '"')
    t,i = string_match(s, "^([^']*)()", i)
    assert(t)
    if t ~= "" then
      table_insert(result, "'")
      table_insert(result, t)
      table_insert(result, "'")
    end
  end
  return table_concat(result, "")
end


local function has_command(name)
  local result = os_execute("which " .. escape(name) .. " > /dev/null")
  -- Note that os.execute returns a number on Lua 5.1 or LuaTeX
  return result == 0 or result == true
end

return {
  escape = escape,
  has_command = has_command,
  cd = "cd",
}
end
end
package.preload["texrunner.fsutil"] = function(...)
--[[
  Copyright 2016 ARATA Mizuki

  This file is part of ClutTeX.

  ClutTeX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ClutTeX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ClutTeX.  If not, see <http://www.gnu.org/licenses/>.
]]

local assert = assert
local os = os
local os_execute = os.execute
local os_remove = os.remove
local filesys = require "lfs"
local pathutil = require "texrunner.pathutil"
local shellutil = require "texrunner.shellutil"
local escape = shellutil.escape

local copy_command
if os.type == "windows" then
  function copy_command(from, to)
    -- TODO: What if `from` begins with a slash?
    return "copy " .. escape(from) .. " " .. escape(to) .. " > NUL"
  end
else
  function copy_command(from, to)
    -- TODO: What if `from` begins with a hypen?
    return "cp " .. escape(from) .. " " .. escape(to)
  end
end

local isfile = filesys.isfile or function(path)
  return filesys.attributes(path, "mode") == "file"
end

local isdir = filesys.isdir or function(path)
  return filesys.attributes(path, "mode") == "directory"
end

local function mkdir_rec(path)
  local succ, err = filesys.mkdir(path)
  if not succ then
    succ, err = mkdir_rec(pathutil.parentdir(path))
    if succ then
      return filesys.mkdir(path)
    end
  end
  return succ, err
end

local function remove_rec(path)
  if filesys.symlinkattributes(path, "mode") == "directory" then
    for file in filesys.dir(path) do
      if file ~= "." and file ~= ".." then
        local succ, err = remove_rec(pathutil.join(path, file))
        if not succ then
          return succ, err
        end
      end
    end
    return filesys.rmdir(path)
  else
    return os_remove(path)
  end
end

return {
  copy_command = copy_command,
  isfile = isfile,
  isdir = isdir,
  mkdir_rec = mkdir_rec,
  remove_rec = remove_rec,
}
end
package.preload["texrunner.luatexinit"] = function(...)
local function create_initialization_script(filename, options)
  local initscript = assert(io.open(filename,"w"))
  if type(options.file_line_error) == "boolean" then
    initscript:write(string.format("texconfig.file_line_error = %s\n", options.file_line_error))
  end
  if type(options.halt_on_error) == "boolean" then
    initscript:write(string.format("texconfig.halt_on_error = %s\n", options.halt_on_error))
  end
  initscript:write([==[
local print = print
local io_open = io.open
local io_write = io.write
local os_execute = os.execute
local texio_write = texio.write
local texio_write_nl = texio.write_nl
]==])

  -- Packages coded in Lua doesn't follow -output-directory option and doesn't write command to the log file
  initscript:write(string.format("local output_directory = %q\n", options.output_directory))
  -- tex.jobname may not be available when io.open is called for the first time
  initscript:write(string.format("local jobname = %q\n", options.jobname))
  initscript:write([==[
local luawritelog
local function openluawritelog()
  if not luawritelog then
    luawritelog = assert(io_open(output_directory .. "/" .. jobname .. ".cluttex-fls", "w"))
  end
  return luawritelog
end
io.open = function(fname, mode)
  -- luatexja-ruby
  if mode == "w" and fname == jobname .. ".ltjruby" then
    fname = output_directory .. "/" .. fname
  end
  if type(mode) == "string" and string.find(mode, "w") ~= nil then
    -- write mode
    openluawritelog():write("OUTPUT " .. fname .. "\n")
  end
  return io_open(fname, mode)
end
os.execute = function(...)
  if select("#", ...) > 0 then
    texio_write_nl("log", string.format("CLUTTEX_EXEC %s", ...), "")
  end
  return os_execute(...)
end
]==])

  -- Silence some of the TeX output to the terminal.
  initscript:write([==[
local function start_file_cb(category, filename)
  if category == 1 then -- a normal data file, like a TeX source
    texio_write_nl("log", "("..filename)
  elseif category == 2 then -- a font map coupling font names to resources
    texio_write("log", "{"..filename)
  elseif category == 3 then -- an image file (png, pdf, etc)
    texio_write("<"..filename)
  elseif category == 4 then -- an embedded font subset
    texio_write("<"..filename)
  elseif category == 5 then -- a fully embedded font
    texio_write("<<"..filename)
  else
    print("start_file: unknown category", category, filename)
  end
end
callback.register("start_file", start_file_cb)
local function stop_file_cb(category)
  if category == 1 then
    texio_write("log", ")")
  elseif category == 2 then
    texio_write("log", "}")
  elseif category == 3 then
    texio_write(">")
  elseif category == 4 then
    texio_write(">")
  elseif category == 5 then
    texio_write(">>")
  else
    print("stop_file: unknown category", category)
  end
end
callback.register("stop_file", stop_file_cb)
texio.write = function(...)
  if select("#",...) == 1 then
    -- Suppress luaotfload's message (See src/fontloader/runtime/fontload-reference.lua)
    local s = ...
    if string.match(s, "^%(using cache: ")
       or string.match(s, "^%(using write cache: ")
       or string.match(s, "^%(using read cache: ")
       or string.match(s, "^%(load luc: ")
       or string.match(s, "^%(load cache: ") then
      return texio_write("log", ...)
    end
  end
  return texio_write(...)
end
]==])

  -- Fix "arg" to make luamplib work
  initscript:write([==[
if string.match(arg[0], "^%-%-lua=") then
  local minindex = 0
  while arg[minindex - 1] ~= nil do
    minindex = minindex - 1
  end
  local arg2 = {}
  for i = 0, #arg - minindex do
    arg2[i] = arg[i + minindex]
  end
  arg = arg2
end
]==])
  initscript:close()
end

return {
  create_initialization_script = create_initialization_script
}
end
package.preload["texrunner.isatty"] = function(...)
--[[
  Copyright 2018 ARATA Mizuki

  This file is part of ClutTeX.

  ClutTeX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ClutTeX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ClutTeX.  If not, see <http://www.gnu.org/licenses/>.
]]

if os.type == "unix" then
  -- Try LuaJIT-like FFI
  local succ, M = pcall(function()
      local ffi = require "ffi"
      assert(ffi.os ~= "" and ffi.arch ~= "", "ffi library is stub")
      ffi.cdef[[
int isatty(int fd);
int fileno(void *stream);
]]
      local isatty = assert(ffi.C.isatty, "isatty not found")
      local fileno = assert(ffi.C.fileno, "fileno not found")
      return {
        isatty = function(file)
          -- LuaJIT converts Lua's file handles into FILE* (void*)
          return isatty(fileno(file)) ~= 0
        end
      }
  end)
  if succ then
    if CLUTTEX_VERBOSITY >= 3 then
      io.stderr:write("ClutTeX: isatty found via FFI (Unix)\n")
    end
    return M
  else
    if CLUTTEX_VERBOSITY >= 3 then
      io.stderr:write("ClutTeX: FFI (Unix) not found: ", M, "\n")
    end
  end

  -- Try luaposix
  local succ, M = pcall(function()
      local isatty = require "posix.unistd".isatty
      local fileno = require "posix.stdio".fileno
      return {
        isatty = function(file)
          return isatty(fileno(file)) == 1
        end,
      }
  end)
  if succ then
    if CLUTTEX_VERBOSITY >= 3 then
      io.stderr:write("ClutTeX: isatty found via luaposix\n")
    end
    return M
  else
    if CLUTTEX_VERBOSITY >= 3 then
      io.stderr:write("ClutTeX: luaposix not found: ", M, "\n")
    end
  end

  -- Fallback using system command
  return {
    isatty = function(file)
      local fd
      if file == io.stdin then
        fd = 0
      elseif file == io.stdout then
        fd = 1
      elseif file == io.stderr then
        fd = 2
      else
        return false
      end
      local result = os.execute(string.format("test -t %d", fd))
      return result == true or result == 0
    end,
  }

else
  -- Try LuaJIT
  local succ, M = pcall(function()
      local ffi = require "ffi"
      local bitlib = assert(bit32 or bit, "Neither bit32 (Lua 5.2) nor bit (LuaJIT) found") -- Lua 5.2 or LuaJIT
      ffi.cdef[[
int _isatty(int fd);
int _fileno(void *stream);
void *_get_osfhandle(int fd); // should return intptr_t
typedef int BOOL;
typedef uint32_t DWORD;
typedef int FILE_INFO_BY_HANDLE_CLASS; // ???
typedef struct _FILE_NAME_INFO {
DWORD FileNameLength;
uint16_t FileName[?];
} FILE_NAME_INFO;
DWORD GetFileType(void *hFile);
BOOL GetFileInformationByHandleEx(void *hFile, FILE_INFO_BY_HANDLE_CLASS fic, void *fileinfo, DWORD dwBufferSize);
BOOL GetConsoleMode(void *hConsoleHandle, DWORD* lpMode);
BOOL SetConsoleMode(void *hConsoleHandle, DWORD dwMode);
DWORD GetLastError();
]]
      local isatty = assert(ffi.C._isatty, "_isatty not found")
      local fileno = assert(ffi.C._fileno, "_fileno not found")
      local get_osfhandle = assert(ffi.C._get_osfhandle, "_get_osfhandle not found")
      local GetFileType = assert(ffi.C.GetFileType, "GetFileType not found")
      local GetFileInformationByHandleEx = assert(ffi.C.GetFileInformationByHandleEx, "GetFileInformationByHandleEx not found")
      local GetConsoleMode = assert(ffi.C.GetConsoleMode, "GetConsoleMode not found")
      local SetConsoleMode = assert(ffi.C.SetConsoleMode, "SetConsoleMode not found")
      local GetLastError = assert(ffi.C.GetLastError, "GetLastError not found")
      local function wide_to_narrow(array, length)
        local t = {}
        for i = 0, length - 1 do
          table.insert(t, string.char(math.min(array[i], 0xff)))
        end
        return table.concat(t, "")
      end
      local function is_mintty(fd)
        local handle = get_osfhandle(fd)
        local filetype = GetFileType(handle)
        if filetype ~= 0x0003 then -- not FILE_TYPE_PIPE (0x0003)
          -- mintty must be a pipe
          if CLUTTEX_VERBOSITY >= 4 then
            io.stderr:write("ClutTeX: is_mintty: not a pipe\n")
          end
          return false
        end
        local nameinfo = ffi.new("FILE_NAME_INFO", 32768)
        local FileNameInfo = 2 -- : FILE_INFO_BY_HANDLE_CLASS
        if GetFileInformationByHandleEx(handle, FileNameInfo, nameinfo, ffi.sizeof("FILE_NAME_INFO", 32768)) ~= 0 then
          local filename = wide_to_narrow(nameinfo.FileName, math.floor(nameinfo.FileNameLength / 2))
          -- \(cygwin|msys)-<hex digits>-pty<N>-(from|to)-master
          if CLUTTEX_VERBOSITY >= 4 then
            io.stderr:write("ClutTeX: is_mintty: GetFileInformationByHandleEx returned ", filename, "\n")
          end
          local a, b = string.match(filename, "^\\(%w+)%-%x+%-pty%d+%-(%w+)%-master$")
          return (a == "cygwin" or a == "msys") and (b == "from" or b == "to")
        else
          if CLUTTEX_VERBOSITY >= 4 then
            io.stderr:write("ClutTeX: is_mintty: GetFileInformationByHandleEx failed\n")
          end
          return false
        end
      end
      return {
        isatty = function(file)
          -- LuaJIT converts Lua's file handles into FILE* (void*)
          local fd = fileno(file)
          return isatty(fd) ~= 0 or is_mintty(fd)
        end,
        enable_virtual_terminal = function(file)
          local fd = fileno(file)
          if is_mintty(fd) then
            -- MinTTY
            if CLUTTEX_VERBOSITY >= 4 then
              io.stderr:write("ClutTeX: Detected MinTTY\n")
            end
            return true
          elseif isatty(fd) ~= 0 then
            -- Check for ConEmu or ansicon
            if os.getenv("ConEmuANSI") == "ON" or os.getenv("ANSICON") then
              if CLUTTEX_VERBOSITY >= 4 then
                io.stderr:write("ClutTeX: Detected ConEmu or ansicon\n")
              end
              return true
            else
              -- Try native VT support on recent Windows
              local handle = get_osfhandle(fd)
              local modePtr = ffi.new("DWORD[1]")
              local result = GetConsoleMode(handle, modePtr)
              if result == 0 then
                if CLUTTEX_VERBOSITY >= 3 then
                  local err = GetLastError()
                  io.stderr:write(string.format("ClutTeX: GetConsoleMode failed (0x%08X)\n", err))
                end
                return false
              end
              local ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004
              result = SetConsoleMode(handle, bitlib.bor(modePtr[0], ENABLE_VIRTUAL_TERMINAL_PROCESSING))
              if result == 0 then
                -- SetConsoleMode failed: Command Prompt on older Windows
                if CLUTTEX_VERBOSITY >= 3 then
                  local err = GetLastError()
                  -- Typical error code: ERROR_INVALID_PARAMETER (0x57)
                  io.stderr:write(string.format("ClutTeX: SetConsoleMode failed (0x%08X)\n", err))
                end
                return false
              end
              if CLUTTEX_VERBOSITY >= 4 then
                io.stderr:write("ClutTeX: Detected recent Command Prompt\n")
              end
              return true
            end
          else
            -- Not a TTY
            return false
          end
        end,
      }
  end)
  if succ then
    if CLUTTEX_VERBOSITY >= 3 then
      io.stderr:write("ClutTeX: isatty found via FFI (Windows)\n")
    end
    return M
  else
    if CLUTTEX_VERBOSITY >= 3 then
      io.stderr:write("ClutTeX: FFI (Windows) not found: ", M, "\n")
    end
  end
end

return {
  isatty = function(file)
    return false
  end,
}
end
package.preload["texrunner.fswatcher_windows"] = function(...)
--[[
  Copyright 2019 ARATA Mizuki

  This file is part of ClutTeX.

  ClutTeX is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ClutTeX is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ClutTeX.  If not, see <http://www.gnu.org/licenses/>.
]]

local ffi = require "ffi"
local bitlib = assert(bit32 or bit, "Neither bit32 (Lua 5.2) nor bit (LuaJIT) found") -- Lua 5.2 or LuaJIT

ffi.cdef[[
typedef int BOOL;
typedef unsigned int UINT;
typedef uint32_t DWORD;
typedef void *HANDLE;
typedef uintptr_t ULONG_PTR;
typedef uint16_t WCHAR;
typedef struct _OVERLAPPED {
  ULONG_PTR Internal;
  ULONG_PTR InternalHigh;
  union {
    struct {
      DWORD Offset;
      DWORD OffsetHigh;
    };
    void *Pointer;
  };
  HANDLE hEvent;
} OVERLAPPED;
typedef struct _FILE_NOTIFY_INFORMATION {
  DWORD NextEntryOffset;
  DWORD Action;
  DWORD FileNameLength;
  WCHAR FileName[?];
} FILE_NOTIFY_INFORMATION;
typedef void (__stdcall *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, OVERLAPPED *lpOverlapped);
DWORD GetLastError();
BOOL CloseHandle(HANDLE hObject);
HANDLE CreateFileA(const char *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void *lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HANDLE CreateIoCompletionPort(HANDLE fileHandle, HANDLE existingCompletionPort, ULONG_PTR completionKey, DWORD numberOfConcurrentThreads);
BOOL ReadDirectoryChangesW(HANDLE hDirectory, void *lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, DWORD *lpBytesReturned, OVERLAPPED *lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpOverlappedCompletionRoutine);
BOOL GetQueuedCompletionStatus(HANDLE CompletionPort, DWORD *lpNumberOfBytes, ULONG_PTR *lpCompletionKey, OVERLAPPED **lpOverlapped, DWORD dwMilliseconds);
int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, const char *lpMultiByteStr, int cbMultiByte, WCHAR *lpWideCharStr, int cchWideChar);
int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, const WCHAR *lpWideCharStr, int cchWideChar, char *lpMultiByteStr, int cbMultiByte, const char *lpDefaultChar, BOOL *lpUsedDefaultChar);
DWORD GetFullPathNameA(const char *lpFileName, DWORD nBufferLength, char *lpBuffer, char **lpFilePart);
uint64_t GetTickCount64();
]]

-- LuaTeX's FFI does not equate a null pointer with nil.
-- On LuaJIT, ffi.NULL is just nil.
local NULL = ffi.NULL

-- GetLastError
local ERROR_FILE_NOT_FOUND         = 0x0002
local ERROR_PATH_NOT_FOUND         = 0x0003
local ERROR_ACCESS_DENIED          = 0x0005
local ERROR_INVALID_PARAMETER      = 0x0057
local ERROR_INSUFFICIENT_BUFFER    = 0x007A
local WAIT_TIMEOUT                 = 0x0102
local ERROR_ABANDONED_WAIT_0       = 0x02DF
local ERROR_NOACCESS               = 0x03E6
local ERROR_INVALID_FLAGS          = 0x03EC
local ERROR_NOTIFY_ENUM_DIR        = 0x03FE
local ERROR_NO_UNICODE_TRANSLATION = 0x0459
local KnownErrors = {
  [ERROR_FILE_NOT_FOUND] = "ERROR_FILE_NOT_FOUND",
  [ERROR_PATH_NOT_FOUND] = "ERROR_PATH_NOT_FOUND",
  [ERROR_ACCESS_DENIED] = "ERROR_ACCESS_DENIED",
  [ERROR_INVALID_PARAMETER] = "ERROR_INVALID_PARAMETER",
  [ERROR_INSUFFICIENT_BUFFER] = "ERROR_INSUFFICIENT_BUFFER",
  [ERROR_ABANDONED_WAIT_0] = "ERROR_ABANDONED_WAIT_0",
  [ERROR_NOACCESS] = "ERROR_NOACCESS",
  [ERROR_INVALID_FLAGS] = "ERROR_INVALID_FLAGS",
  [ERROR_NOTIFY_ENUM_DIR] = "ERROR_NOTIFY_ENUM_DIR",
  [ERROR_NO_UNICODE_TRANSLATION] = "ERROR_NO_UNICODE_TRANSLATION",
}

-- CreateFile
local FILE_FLAG_BACKUP_SEMANTICS = 0x02000000
local FILE_FLAG_OVERLAPPED       = 0x40000000
local OPEN_EXISTING              = 3
local FILE_SHARE_READ            = 0x00000001
local FILE_SHARE_WRITE           = 0x00000002
local FILE_SHARE_DELETE          = 0x00000004
local FILE_LIST_DIRECTORY        = 0x1
local INVALID_HANDLE_VALUE       = ffi.cast("void *", -1)

-- ReadDirectoryChangesW / FILE_NOTIFY_INFORMATION
local FILE_NOTIFY_CHANGE_FILE_NAME   = 0x00000001
local FILE_NOTIFY_CHANGE_DIR_NAME    = 0x00000002
local FILE_NOTIFY_CHANGE_ATTRIBUTES  = 0x00000004
local FILE_NOTIFY_CHANGE_SIZE        = 0x00000008
local FILE_NOTIFY_CHANGE_LAST_WRITE  = 0x00000010
local FILE_NOTIFY_CHANGE_LAST_ACCESS = 0x00000020
local FILE_NOTIFY_CHANGE_CREATION    = 0x00000040
local FILE_NOTIFY_CHANGE_SECURITY    = 0x00000100
local FILE_ACTION_ADDED              = 0x00000001
local FILE_ACTION_REMOVED            = 0x00000002
local FILE_ACTION_MODIFIED           = 0x00000003
local FILE_ACTION_RENAMED_OLD_NAME   = 0x00000004
local FILE_ACTION_RENAMED_NEW_NAME   = 0x00000005

-- WideCharToMultiByte / MultiByteToWideChar
local CP_ACP  = 0
local CP_UTF8 = 65001

local C = ffi.C

local function format_error(name, lasterror, extra)
  local errorname = KnownErrors[lasterror] or string.format("error code %d", lasterror)
  if extra then
    return string.format("%s failed with %s (0x%04x) [%s]", name, errorname, lasterror, extra)
  else
    return string.format("%s failed with %s (0x%04x)", name, errorname, lasterror)
  end
end
local function wcs_to_mbs(wstr, wstrlen, codepage)
  -- wstr: FFI uint16_t[?]
  -- wstrlen: length of wstr, or -1 if NUL-terminated
  if wstrlen == 0 then
    return ""
  end
  codepage = codepage or CP_ACP
  local dwFlags = 0
  local result = C.WideCharToMultiByte(codepage, dwFlags, wstr, wstrlen, nil, 0, nil, nil)
  if result <= 0 then
    -- Failed
    local lasterror = C.GetLastError()
    -- Candidates: ERROR_INSUFFICIENT_BUFFER, ERROR_INVALID_FLAGS, ERROR_INVALID_PARAMETER, ERROR_NO_UNICODE_TRANSLATION
    return nil, format_error("WideCharToMultiByte", lasterror)
  end
  local mbsbuf = ffi.new("char[?]", result)
  result = C.WideCharToMultiByte(codepage, dwFlags, wstr, wstrlen, mbsbuf, result, nil, nil)
  if result <= 0 then
    -- Failed
    local lasterror = C.GetLastError()
    -- Candidates: ERROR_INSUFFICIENT_BUFFER, ERROR_INVALID_FLAGS, ERROR_INVALID_PARAMETER, ERROR_NO_UNICODE_TRANSLATION
    return nil, format_error("WideCharToMultiByte", lasterror)
  end
  return ffi.string(mbsbuf, result)
end
local function mbs_to_wcs(str, codepage)
  -- str: Lua string
  if str == "" then
    return ffi.new("WCHAR[0]")
  end
  codepage = codepage or CP_ACP
  local dwFlags = 0
  local result = C.MultiByteToWideChar(codepage, dwFlags, str, #str, nil, 0)
  if result <= 0 then
    local lasterror = C.GetLastError()
    -- ERROR_INSUFFICIENT_BUFFER, ERROR_INVALID_FLAGS, ERROR_INVALID_PARAMETER, ERROR_NO_UNICODE_TRANSLATION
    return nil, format_error("MultiByteToWideChar", lasterror)
  end
  local wcsbuf = ffi.new("WCHAR[?]", result)
  result = C.MultiByteToWideChar(codepage, dwFlags, str, #str, wcsbuf, result)
  if result <= 0 then
    local lasterror = C.GetLastError()
    return nil, format_error("MultiByteToWideChar", lasterror)
  end
  return wcsbuf, result
end


local function get_full_path_name(filename)
  local bufsize = 1024
  local buffer
  local filePartPtr = ffi.new("char*[1]")
  local result
  repeat
    buffer = ffi.new("char[?]", bufsize)
    result = C.GetFullPathNameA(filename, bufsize, buffer, filePartPtr)
    if result == 0 then
      local lasterror = C.GetLastError()
      return nil, format_error("GetFullPathNameA", lasterror, filename)
    elseif bufsize < result then
      -- result: buffer size required to hold the path + terminating NUL
      bufsize = result
    end
  until result < bufsize
  local fullpath = ffi.string(buffer, result)
  local filePart = ffi.string(filePartPtr[0])
  local dirPart = ffi.string(buffer, ffi.cast("intptr_t", filePartPtr[0]) - ffi.cast("intptr_t", buffer)) -- LuaTeX's FFI doesn't support pointer subtraction
  return fullpath, filePart, dirPart
end

--[[
  dirwatche.dirname : string
  dirwatcher._rawhandle : cdata HANDLE
  dirwatcher._overlapped : cdata OVERLAPPED
  dirwatcher._buffer : cdata char[?]
]]
local dirwatcher_meta = {}
dirwatcher_meta.__index = dirwatcher_meta
function dirwatcher_meta:close()
  if self._rawhandle ~= nil then
    C.CloseHandle(ffi.gc(self._rawhandle, nil))
    self._rawhandle = nil
  end
end
local function open_directory(dirname)
  local dwShareMode = bitlib.bor(FILE_SHARE_READ, FILE_SHARE_WRITE, FILE_SHARE_DELETE)
  local dwFlagsAndAttributes = bitlib.bor(FILE_FLAG_BACKUP_SEMANTICS, FILE_FLAG_OVERLAPPED)
  local handle = C.CreateFileA(dirname, FILE_LIST_DIRECTORY, dwShareMode, nil, OPEN_EXISTING, dwFlagsAndAttributes, nil)
  if handle == INVALID_HANDLE_VALUE then
    local lasterror = C.GetLastError()
    print("Failed to open "..dirname)
    return nil, format_error("CreateFileA", lasterror, dirname)
  end
  return setmetatable({
    dirname = dirname,
    _rawhandle = ffi.gc(handle, C.CloseHandle),
    _overlapped = ffi.new("OVERLAPPED"),
    _buffer = ffi.new("char[?]", 1024),
  }, dirwatcher_meta)
end
function dirwatcher_meta:start_watch(watchSubtree)
  local dwNotifyFilter = bitlib.bor(FILE_NOTIFY_CHANGE_FILE_NAME, FILE_NOTIFY_CHANGE_DIR_NAME, FILE_NOTIFY_CHANGE_ATTRIBUTES, FILE_NOTIFY_CHANGE_SIZE, FILE_NOTIFY_CHANGE_LAST_WRITE, FILE_NOTIFY_CHANGE_LAST_ACCESS, FILE_NOTIFY_CHANGE_CREATION, FILE_NOTIFY_CHANGE_SECURITY)
  local buffer = self._buffer
  local bufferSize = ffi.sizeof(buffer)
  local result = C.ReadDirectoryChangesW(self._rawhandle, buffer, bufferSize, watchSubtree, dwNotifyFilter, nil, self._overlapped, nil)
  if result == 0 then
    local lasterror = C.GetLastError()
    return nil, format_error("ReadDirectoryChangesW", lasterror, self.dirname)
  end
  return true
end
local ActionTable = {
  [FILE_ACTION_ADDED] = "added",
  [FILE_ACTION_REMOVED] = "removed",
  [FILE_ACTION_MODIFIED] = "modified",
  [FILE_ACTION_RENAMED_OLD_NAME] = "rename_from",
  [FILE_ACTION_RENAMED_NEW_NAME] = "rename_to",
}
function dirwatcher_meta:process(numberOfBytes)
  -- self._buffer received `numberOfBytes` bytes
  local buffer = self._buffer
  numberOfBytes = math.min(numberOfBytes, ffi.sizeof(buffer))
  local ptr = ffi.cast("char *", buffer)
  local structSize = ffi.sizeof("FILE_NOTIFY_INFORMATION", 1)
  local t = {}
  while numberOfBytes >= structSize do
    local notifyInfo = ffi.cast("FILE_NOTIFY_INFORMATION*", ptr)
    local nextEntryOffset = notifyInfo.NextEntryOffset
    local action = notifyInfo.Action
    local fileNameLength = notifyInfo.FileNameLength
    local fileName = notifyInfo.FileName
    local u = { action = ActionTable[action], filename = wcs_to_mbs(fileName, fileNameLength / 2) }
    table.insert(t, u)
    if nextEntryOffset == 0 or numberOfBytes <= nextEntryOffset then
      break
    end
    numberOfBytes = numberOfBytes - nextEntryOffset
    ptr = ptr + nextEntryOffset
  end
  return t
end

--[[
  watcher._rawport : cdata HANDLE
  watcher._pending : array of {
    action = ..., filename = ...
  }
  watcher._directories[dirname] = {
    dir = directory watcher,
    dirname = dirname,
    files = { [filename] = user-supplied path } -- files to watch
  }
  watcher[i] = i-th directory (_directories[dirname] for some dirname)
]]

local fswatcher_meta = {}
fswatcher_meta.__index = fswatcher_meta
local function new_watcher()
  local port = C.CreateIoCompletionPort(INVALID_HANDLE_VALUE, nil, 0, 0)
  if port == NULL then
    local lasterror = C.GetLastError()
    return nil, format_error("CreateIoCompletionPort", lasterror)
  end
  return setmetatable({
    _rawport = ffi.gc(port, C.CloseHandle), -- ?
    _pending = {},
    _directories = {},
  }, fswatcher_meta)
end
local function add_directory(self, dirname)
  local t = self._directories[dirname]
  if not t then
    local dirwatcher, err = open_directory(dirname)
    if not dirwatcher then
      return dirwatcher, err
    end
    t = { dirwatcher = dirwatcher, dirname = dirname, files = {} }
    table.insert(self, t)
    local i = #self
    local result = C.CreateIoCompletionPort(dirwatcher._rawhandle, self._rawport, i, 0)
    if result == NULL then
      local lasterror = C.GetLastError()
      return nil, format_error("CreateIoCompletionPort", lasterror, dirname)
    end
    self._directories[dirname] = t
    local result, err = dirwatcher:start_watch(false)
    if not result then
      return result, err
    end
  end
  return t
end
function fswatcher_meta:add_file(path, ...)
  local fullpath, filename, dirname = get_full_path_name(path)
  local t, err = add_directory(self, dirname)
  if not t then
    return t, err
  end
  t.files[filename] = path
  return true
end
local INFINITE = 0xFFFFFFFF
local function get_queued(self, timeout)
  local startTime = C.GetTickCount64()
  local timeout_ms
  if timeout == nil then
    timeout_ms = INFINITE
  else
    timeout_ms = timeout * 1000
  end
  local numberOfBytesPtr = ffi.new("DWORD[1]")
  local completionKeyPtr = ffi.new("ULONG_PTR[1]")
  local lpOverlapped = ffi.new("OVERLAPPED*[1]")
  repeat
    local result = C.GetQueuedCompletionStatus(self._rawport, numberOfBytesPtr, completionKeyPtr, lpOverlapped, timeout_ms)
    if result == 0 then
      local lasterror = C.GetLastError()
      if lasterror == WAIT_TIMEOUT then
        return nil, "timeout"
      else
        return nil, format_error("GetQueuedCompletionStatus", lasterror)
      end
    end
    local numberOfBytes = numberOfBytesPtr[0]
    local completionKey = tonumber(completionKeyPtr[0])
    local dir_t = assert(self[completionKey], "invalid completion key: " .. tostring(completionKey))
    local t = dir_t.dirwatcher:process(numberOfBytes)
    dir_t.dirwatcher:start_watch(false)
    local found = false
    for i,v in ipairs(t) do
      local path = dir_t.files[v.filename]
      if path then
        found = true
        table.insert(self._pending, {path = path, action = v.action})
      end
    end
    if found then
      return true
    end
    if timeout_ms ~= INFINITE then
      local tt = C.GetTickCount64()
      timeout_ms = timeout_ms - (tt - startTime)
      startTime = tt
    end
  until timeout_ms < 0
  return nil, "timeout"
end
function fswatcher_meta:next(timeout)
  if #self._pending > 0 then
    local result = table.remove(self._pending, 1)
    get_queued(self, 0) -- ignore error
    return result
  else
    local result, err = get_queued(self, timeout)
    if result == nil then
      return nil, err
    end
    return table.remove(self._pending, 1)
  end
end
function fswatcher_meta:close()
  if self._rawport ~= nil then
    for i,v in ipairs(self) do
      v.dirwatcher:close()
    end
    C.CloseHandle(ffi.gc(self._rawport, nil))
    self._rawport = nil
  end
end
--[[
local watcher = require("fswatcher_windows").new()
assert(watcher:add_file("rdc-sync.c"))
assert(watcher:add_file("sub2/hoge"))
for i = 1, 10 do
    local result, err = watcher:next(2)
    if err == "timeout" then
        print(os.date(), "timeout")
    else
        assert(result, err)
        print(os.date(), result.path, result.action)
    end
end
watcher:close()
]]
return {
  new = new_watcher,
}
end
local getmetatable = getmetatable
local pcall = pcall
local setmetatable = setmetatable
local math = math
local math_type = math.type
local math_maxinteger = math.maxinteger
local math_mininteger = math.mininteger
local math_ult = math.ult
local string_char = string.char
local string_format = string.format
local table_concat = table.concat
local function _id(x)
  return x
end
local _exn_meta = {}
function _exn_meta:__tostring()
  local traceback = self.traceback
  if traceback then
    traceback = "\n" .. traceback
  else
    traceback = ""
  end
  return string_format("%s: %s%s", self.location or "<no location info>", self.tag[1], traceback)
end
local _Match_tag = { "Match" }
local _Match = setmetatable({ tag = _Match_tag }, _exn_meta)
local _Bind_tag = { "Bind" }
local _Bind = setmetatable({ tag = _Bind_tag }, _exn_meta)
local _Overflow_tag = { "Overflow" }
local _Overflow = setmetatable({ tag = _Overflow_tag }, _exn_meta)
local _Div_tag = { "Div" }
local _Div = setmetatable({ tag = _Div_tag }, _exn_meta)
local _Size_tag = { "Size" }
local _Size = setmetatable({ tag = _Size_tag }, _exn_meta)
local _Subscript_tag = { "Subscript" }
local _Subscript = setmetatable({ tag = _Subscript_tag }, _exn_meta)
local _Fail_tag = { "Fail" }
local function _isError(e)
  return getmetatable(e) ~= _exn_meta
end
local function __exn_instanceof(e, tag)
  return getmetatable(e) == _exn_meta and e.tag == tag
end
local function _raise(x, location)
  local e
  if getmetatable(x) == _exn_meta and location ~= nil then
    local traceback = debug.traceback(nil, 2)
    e = setmetatable({ tag = x.tag, payload = x.payload, location = location, traceback = traceback }, _exn_meta)
  else
    e = x
  end
  error(e, 1)
end
local function _Int_add(x, y)
  -- assert(math_type(x) == "integer")
  -- assert(math_type(y) == "integer")
  local z = x + y
  if y > 0 and z < x then
    _raise(_Overflow, "Int.+")
  elseif y < 0 and z > x then
    _raise(_Overflow, "Int.+")
  else
    return z
  end
end
local function _Int_sub(x, y)
  -- assert(math_type(x) == "integer")
  -- assert(math_type(y) == "integer")
  local z = x - y
  if y < 0 and z < x then
    _raise(_Overflow, "Int.-")
  elseif y > 0 and x < z then
    _raise(_Overflow, "Int.-")
  else
    return z
  end
end
local function _Int_mul(x, y)
  -- assert(math_type(x) == "integer")
  -- assert(math_type(y) == "integer")
  local z = x * y
  if (x ~= 0 and z // x ~= y) or (y ~= 0 and z // y ~= x) then
    _raise(_Overflow, "Int.*")
  else
    return z
  end
end
local function _Int_div(x, y)
  -- assert(math_type(x) == "integer")
  -- assert(math_type(y) == "integer")
  if y == 0 then
    _raise(_Div, "Int.div")
  elseif x == math_mininteger and y == -1 then
    _raise(_Overflow, "Int.div")
  end
  return x // y
end
local function _Int_mod(x, y)
  -- assert(math_type(x) == "integer")
  -- assert(math_type(y) == "integer")
  if y == 0 then
    _raise(_Div, "Int.mod")
  end
  return x % y
end
local function _Int_negate(x)
  -- assert(math_type(x) == "integer")
  if x == math_mininteger then
    _raise(_Overflow, "Int.~")
  end
  return - x
end
local function _Word_div(x, y)
  -- assert(math_type(x) == "integer")
  -- assert(math_type(y) == "integer")
  if y == 0 then
    _raise(_Div, "Word.div")
  elseif y > 0 then
    if x >= 0 then
      return x // y
    else -- x < 0
      -- Algorithm from Programming in Lua, 4th ed.
      local q = ((x >> 1) // y) << 1
      local r = x - q * y
      if math_ult(r, y) then
        return q
      else
        return q + 1
      end
    end
  else -- y < 0
    if math_ult(x, y) then
      return 0
    else
      return 1
    end
  end
end
local function _Word_mod(x, y)
  -- assert(math_type(x) == "integer")
  -- assert(math_type(y) == "integer")
  if y == 0 then
    _raise(_Div, "Word.mod")
  elseif y > 0 then
    if x >= 0 then
      return x % y
    else -- x < 0
      local q = ((x >> 1) // y) << 1
      local r = x - q * y
      if math_ult(r, y) then
        return r
      else
        return r - y
      end
    end
  else -- y < 0
    if math_ult(x, y) then
      return x
    else
      return x - y
    end
  end
end
local function _list(t)
  local xs = nil
  for i = t.n, 1, -1 do
    xs = { t[i], xs }
  end
  return xs
end
local function _Array_array(n, init)
  if n < 0 then -- or maxLen < n
    _raise(_Size, "Array.array")
  end
  local t = { n = n }
  for i = 1, n do
    t[i] = init
  end
  return t
end
local function _VectorOrArray_fromList(xs)
  local t = {}
  local n = 0
  while xs ~= nil do
    n = n + 1
    t[n] = xs[1]
    xs = xs[2]
  end
  t.n = n
  return t
end
local function _VectorOrArray_tabulate(n, f)
  if n < 0 then -- or maxLen < n
    _raise(_Size, "(Vector|Array).tabulate")
  end
  local t = { n = n }
  for i = 1, n do
    t[i] = f(i - 1)
  end
  return t
end
local function _newTableWith(entries)
  local t = {}
  for _, p in ipairs(entries) do
    t[p[1]] = p[2]
  end
  return t
end
local Chr, Domain, LESS, EQUAL, GREATER, Option, getOpt, app, mapPartial, tmp, revAppend, app1, map, mapPartial1, find, foldl, exists, all, tabulate, foldr_PRIME, size, str, eq, os, package, require, tonumber, tostring, type_PRIME, open_PRIME, popen, stderr, stdout, huge, difftime, execute, exit, getenv, setlocale, time, find1, gsub, match, lfs, checkString, OCT, DEC, HEX, quot, rem, compare, wordSize, fmt, negInf, fromManExp, trunc, sub, substring, extract, concat, implodeRev, explode, map1, translate, fields, isPrefix, isSuffix, isAscii, isDigit, isAlphaNum, isPrint, isSpace, toUpper, toString, scanString, MonoSequence, Base
do
  Chr = setmetatable({tag = {"Chr"}}, _exn_meta)
  Domain = setmetatable({tag = {"Domain"}}, _exn_meta)
  LESS = "LESS"
  EQUAL = "EQUAL"
  GREATER = "GREATER"
  Option = setmetatable({tag = {"Option"}}, _exn_meta)
  getOpt = function(_1, default)
    if _1.tag == "NONE" then
      return default
    elseif _1.tag == "SOME" then
      return _1.payload
    else
      _raise(_Match, "option.sml:19:5")
    end
  end
  app = function(f)
    return function(a)
      if a.tag == "SOME" then
        return f(a.payload)
      elseif a.tag == "NONE" then
        return nil
      else
        _raise(_Match, "option.sml:31:5")
      end
    end
  end
  mapPartial = function(f)
    return function(a)
      if a.tag == "SOME" then
        return f(a.payload)
      elseif a.tag == "NONE" then
        return {tag = "NONE"}
      else
        _raise(_Match, "option.sml:35:5")
      end
    end
  end
  tmp = function(eq1)
    return function(a, b)
      if a.tag == "NONE" and b.tag == "NONE" then
        return true
      elseif a.tag == "SOME" and b.tag == "SOME" then
        return eq1(a.payload, b.payload)
      else
        return false
      end
    end
  end
  revAppend = function(_1, ys)
    local tmp1, tmp2 = _1, ys
    ::cont::
    local _1_1, ys1 = tmp1, tmp2
    if _1_1 == nil then
      return ys1
    elseif _1_1 ~= nil then
      local tmp3 = _1_1[1]
      local tmp4 = _1_1[2]
      tmp1 = tmp4
      tmp2 = {tmp3, ys1}
      goto cont
    else
      _raise(_Match, "list.sml:60:5")
    end
  end
  app1 = function(f)
    return function(a)
      if a == nil then
        return nil
      elseif a ~= nil then
        local tmp1 = a[1]
        local tmp2 = a[2]
        f(tmp1)
        local tmp3 = app1(f)
        return tmp3(tmp2)
      else
        _raise(_Match, "list.sml:64:5")
      end
    end
  end
  map = function(f)
    return function(a)
      if a == nil then
        return nil
      elseif a ~= nil then
        local tmp1 = a[1]
        local tmp2 = a[2]
        local tmp3 = f(tmp1)
        local tmp4 = map(f)
        local tmp5 = tmp4(tmp2)
        return {tmp3, tmp5}
      else
        _raise(_Match, "list.sml:66:5")
      end
    end
  end
  mapPartial1 = function(f)
    return function(a)
      if a == nil then
        return nil
      elseif a ~= nil then
        local tmp1, exp
        do
          local tmp2 = a[1]
          tmp1 = a[2]
          exp = f(tmp2)
          if exp.tag == "NONE" then
            local tmp3 = mapPartial1(f)
            return tmp3(tmp1)
          else
            goto else1
          end
        end
        ::else1::
        if exp.tag == "SOME" then
          local y = exp.payload
          local tmp2 = mapPartial1(f)
          local tmp3 = tmp2(tmp1)
          return {y, tmp3}
        else
          _raise(_Match, "list.sml:69:30")
        end
      else
        _raise(_Match, "list.sml:68:5")
      end
    end
  end
  find = function(f)
    return function(a)
      if a == nil then
        return {tag = "NONE"}
      elseif a ~= nil then
        local tmp1 = a[1]
        local tmp2 = a[2]
        local tmp3 = f(tmp1)
        if tmp3 then
          return {tag = "SOME", payload = tmp1}
        end
        local tmp4 = find(f)
        return tmp4(tmp2)
      else
        _raise(_Match, "list.sml:72:5")
      end
    end
  end
  foldl = function(f)
    return function(init)
      return function(a)
        if a == nil then
          return init
        elseif a ~= nil then
          local tmp1 = a[1]
          local tmp2 = a[2]
          local tmp3 = foldl(f)
          local tmp4 = f({tmp1, init})
          local tmp5 = tmp3(tmp4)
          return tmp5(tmp2)
        else
          _raise(_Match, "list.sml:91:5")
        end
      end
    end
  end
  exists = function(f)
    return function(a)
      if a == nil then
        return false
      elseif a ~= nil then
        local tmp1 = a[1]
        local tmp2 = a[2]
        local tmp3 = f(tmp1)
        if tmp3 then
          return true
        end
        local tmp4 = exists(f)
        return tmp4(tmp2)
      else
        _raise(_Match, "list.sml:96:5")
      end
    end
  end
  all = function(f)
    return function(a)
      if a == nil then
        return true
      elseif a ~= nil then
        local tmp1 = a[1]
        local tmp2 = a[2]
        local tmp3 = f(tmp1)
        if tmp3 then
          local tmp4 = all(f)
          return tmp4(tmp2)
        else
          return false
        end
      else
        _raise(_Match, "list.sml:98:5")
      end
    end
  end
  tabulate = function(n, f)
    if n < 0 then
      _raise(_Size, "list.sml:101:27")
    elseif n < 10 then
      local function go(i)
        if i >= n then
          return nil
        end
        local tmp1 = f(i)
        local tmp2 = go(_Int_add(i, 1))
        return {tmp1, tmp2}
      end
      return go(0)
    else
      goto else1
    end
    ::else1::
    local tmp1, tmp2 = 0, nil
    ::cont::
    local i, acc = tmp1, tmp2
    if i >= n then
      return revAppend(acc, nil)
    else
      goto else2
    end
    ::else2::
    local tmp3 = _Int_add(i, 1)
    local tmp4 = f(i)
    tmp1 = tmp3
    tmp2 = {tmp4, acc}
    goto cont
  end
  foldr_PRIME = function(f, acc, vec, i)
    local tmp1, tmp2, tmp3, tmp4 = f, acc, vec, i
    ::cont::
    local f1, acc1, vec1, i1 = tmp1, tmp2, tmp3, tmp4
    if i1 < 0 then
      return acc1
    end
    local tmp5 = f1({vec1[i1 + 1], acc1})
    tmp1 = f1
    tmp2 = tmp5
    tmp3 = vec1
    tmp4 = _Int_sub(i1, 1)
    goto cont
  end
  size = function(x)
    return #x
  end
  str = function(x)
    return string_char(x)
  end
  eq = function(a, b)
    return a == b
  end
  local io = _ENV.io
  os = _ENV.os
  package = _ENV.package
  require = _ENV.require
  tonumber = _ENV.tonumber
  tostring = _ENV.tostring
  type_PRIME = _ENV.type
  open_PRIME = io.open
  popen = io.popen
  stderr = io.stderr
  stdout = io.stdout
  huge = math.huge
  local modf = math.modf
  difftime = os.difftime
  execute = os.execute
  exit = os.exit
  getenv = os.getenv
  setlocale = os.setlocale
  time = os.time
  local byte = string.byte
  find1 = string.find
  gsub = string.gsub
  match = string.match
  local sub1 = string.sub
  do
    local tmp1, tmp2 = pcall(require, "lfs")
    if tmp1 then
      lfs = {tag = "SOME", payload = tmp2}
    else
      lfs = {tag = "NONE"}
    end
  end
  local TypeError_tag = {"TypeError"}
  checkString = function(x)
    local t = type_PRIME(x)
    if t == "string" then
      return x
    else
      _raise(setmetatable({tag = TypeError_tag, payload = "expected a string, but got " .. t}, _exn_meta), "lua.sml:551:37")
    end
  end
  OCT = "OCT"
  DEC = "DEC"
  HEX = "HEX"
  quot = function(x, y)
    do
      local tmp1 = x >= 0 and y >= 0
      local tmp2
      tmp2 = tmp1 or x <= 0 and y <= 0
      if tmp2 then
        return _Int_div(x, y)
      elseif x == math_mininteger then
        if y == 1 then
          return x
        end
        local tmp3 = _Int_negate(y)
        return _Int_negate(_Int_div(x, tmp3))
      else
        goto else1
      end
    end
    ::else1::
    local tmp1 = _Int_negate(x)
    return _Int_negate(_Int_div(tmp1, y))
  end
  rem = function(x, y)
    if y == -1 then
      return 0
    end
    local tmp1 = quot(x, y)
    return _Int_sub(x, _Int_mul(tmp1, y))
  end
  compare = function(a)
    local x = a[1]
    local y = a[2]
    if x == y then
      return EQUAL
    elseif x < y then
      return LESS
    else
      return GREATER
    end
  end
  do
    local tmp1, tmp2 = math_maxinteger, 1
    ::cont5::
    local x, n = tmp1, tmp2
    if x == 0 then
      wordSize = n
    else
      local tmp3 = x >> 1
      tmp1 = tmp3
      tmp2 = _Int_add(n, 1)
      goto cont5
    end
  end
  local fmtBIN = function(x)
    _raise(setmetatable({tag = _Fail_tag, payload = "StringCvt.BIN: not implemented yet"}, _exn_meta), "word-1.sml:90:34")
  end
  local fmtOCT = function(x)
    local result = string_format("%o", x)
    return result
  end
  local fmtDEC = function(x)
    local result = string_format("%u", x)
    return result
  end
  local toString1 = function(x)
    local result = string_format("%X", x)
    return result
  end
  fmt = function(a)
    if a == "BIN" then
      return fmtBIN
    elseif a == "OCT" then
      return fmtOCT
    elseif a == "DEC" then
      return fmtDEC
    elseif a == "HEX" then
      return toString1
    else
      _raise(_Match, "word-1.sml:103:5")
    end
  end
  negInf = - huge
  fromManExp = function(exp, man)
    local tmp1, tmp2 = exp, man
    ::cont::
    local exp1, man1 = tmp1, tmp2
    if -1022 <= exp1 then
      if exp1 < 1024 then
        local tmp3 = 2.0
        local tmp4 = tmp3 ^ exp1
        return man1 * tmp4
      end
      local exp_PRIME
      if exp1 > 2098 then
        exp_PRIME = 2098
      else
        exp_PRIME = exp1
      end
      local tmp3 = man1 * 0x1p1023
      tmp1 = _Int_sub(exp_PRIME, 1023)
      tmp2 = tmp3
      goto cont
    end
    local exp_PRIME
    if exp1 < -2099 then
      exp_PRIME = -2099
    else
      exp_PRIME = exp1
    end
    local j = exp_PRIME % -1022
    if j ~= 0 then
      local tmp3 = 2.0
      local tmp4 = tmp3 ^ j
      local tmp5 = man1 * tmp4
      tmp1 = _Int_sub(exp_PRIME, j)
      tmp2 = tmp5
      goto cont
    else
      local tmp3 = man1 * 0x1p-1022
      tmp1 = _Int_add(exp_PRIME, 1022)
      tmp2 = tmp3
      goto cont
    end
  end
  trunc = function(x)
    local result = modf(x)
    if math_type(result) == "integer" then
      return result
    elseif x ~= x then
      _raise(Domain, "real-1.sml:301:26")
    else
      _raise(_Overflow, "real-1.sml:303:26")
    end
  end
  sub = function(a)
    local s = a[1]
    local i = a[2]
    if i < 0 or #s <= i then
      _raise(_Subscript, "string-1.sml:33:44")
    else
      local i_PRIME = _Int_add(i, 1)
      local result = byte(s, i_PRIME)
      return result
    end
  end
  substring = function(s, i, j)
    local tmp1
    if i < 0 then
      tmp1 = true
    elseif j < 0 then
      tmp1 = true
    else
      local tmp2 = #s
      tmp1 = tmp2 < _Int_add(i, j)
    end
    if tmp1 then
      _raise(_Subscript, "string-1.sml:40:59")
    else
      local tmp2 = _Int_add(i, 1)
      local tmp3 = _Int_add(i, j)
      local result = sub1(s, tmp2, tmp3)
      return result
    end
  end
  extract = function(s, i, _3)
    if _3.tag == "NONE" then
      if i < 0 or #s < i then
        _raise(_Subscript, "string-1.sml:46:69")
      else
        local tmp1 = _Int_add(i, 1)
        local result = sub1(s, tmp1)
        return result
      end
    elseif _3.tag == "SOME" then
      return substring(s, i, _3.payload)
    else
      _raise(_Match, "string-1.sml:45:5")
    end
  end
  concat = function(l)
    local tmp1 = _VectorOrArray_fromList(l)
    local result = table_concat(tmp1)
    return result
  end
  local implode = function(l)
    local tmp1 = _VectorOrArray_fromList(l)
    local tmp2
    do
      local tmp3 = tmp1.n
      tmp2 = _VectorOrArray_tabulate(tmp3, function(i)
        return string_char(tmp1[i + 1])
      end)
    end
    local result = table_concat(tmp2)
    return result
  end
  implodeRev = function(l)
    local tmp1 = revAppend(l, nil)
    local tmp2 = _VectorOrArray_fromList(tmp1)
    local tmp3
    do
      local tmp4 = tmp2.n
      tmp3 = _VectorOrArray_tabulate(tmp4, function(i)
        return string_char(tmp2[i + 1])
      end)
    end
    local result = table_concat(tmp3)
    return result
  end
  explode = function(s)
    local tmp1 = function(a)
      local x = a[1]
      return {x, a[2]}
    end
    local tmp2 = #s
    local tmp3 = _VectorOrArray_tabulate(tmp2, function(i)
      return sub({s, i})
    end)
    return foldr_PRIME(tmp1, nil, tmp3, _Int_sub(tmp3.n, 1))
  end
  map1 = function(f)
    return function(s)
      local g = function(x)
        local tmp1 = byte(x)
        local tmp2 = f(tmp1)
        return string_char(tmp2)
      end
      local result = gsub(s, ".", g)
      return result
    end
  end
  translate = function(f)
    return function(s)
      local g = function(x)
        local tmp1 = byte(x)
        return f(tmp1)
      end
      local result = gsub(s, ".", g)
      return result
    end
  end
  fields = function(f)
    return function(s)
      local tmp1, tmp2, tmp3
      do
        local tmp4 = explode(s)
        tmp3, tmp2, tmp1 = nil, nil, tmp4
      end
      ::cont::
      local revFields, acc, _3 = tmp3, tmp2, tmp1
      if _3 == nil then
        local tmp4 = implodeRev(acc)
        return revAppend({tmp4, revFields}, nil)
      else
        goto else1
      end
      ::else1::
      if _3 ~= nil then
        local tmp4 = _3[1]
        local tmp5 = _3[2]
        local tmp6 = f(tmp4)
        if tmp6 then
          local tmp7 = implodeRev(acc)
          tmp3 = {tmp7, revFields}
          tmp2 = nil
          tmp1 = tmp5
          goto cont
        else
          tmp3 = revFields
          tmp2 = {tmp4, acc}
          tmp1 = tmp5
          goto cont
        end
      else
        _raise(_Match, "string-1.sml:80:26")
      end
    end
  end
  isPrefix = function(prefix)
    return function(s)
      local n = #prefix
      if n > #s then
        return false
      end
      local tmp1 = substring(s, 0, n)
      return tmp1 == prefix
    end
  end
  isSuffix = function(suffix)
    return function(s)
      local n = #suffix
      local m = #s
      if n > m then
        return false
      end
      local tmp1 = substring(s, _Int_sub(m, n), n)
      return tmp1 == suffix
    end
  end
  isAscii = function(c)
    return c <= 127
  end
  isDigit = function(c)
    return 48 <= c and c <= 57
  end
  isAlphaNum = function(c)
    local tmp1
    if 65 <= c and c <= 90 then
      return true
    else
      tmp1 = 97 <= c and c <= 122
    end
    return tmp1 or 48 <= c and c <= 57
  end
  isPrint = function(c)
    return 33 <= c and c <= 126 or c == 32
  end
  isSpace = function(c)
    return 9 <= c and c <= 13 or c == 32
  end
  toUpper = function(c)
    if 97 <= c and c <= 122 then
      local tmp1 = _Int_sub(c, 32)
      if tmp1 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp1 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return tmp1
      end
    else
      return c
    end
  end
  toString = function(c)
    if c == 92 then
      return "\\\\"
    elseif c == 34 then
      return "\\\""
    end
    local tmp1 = isPrint(c)
    if tmp1 then
      return string_char(c)
    elseif c == 7 then
      return "\\a"
    elseif c == 8 then
      return "\\b"
    elseif c == 9 then
      return "\\t"
    elseif c == 10 then
      return "\\n"
    elseif c == 11 then
      return "\\v"
    elseif c == 12 then
      return "\\f"
    elseif c == 13 then
      return "\\r"
    end
    local x = c
    if x < 32 then
      local tmp2 = _Int_add(x, 64)
      if tmp2 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp2 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return "\\^" .. string_char(tmp2)
      end
    elseif x < 100 then
      local tmp2 = gsub(tostring(x), "-", "~")
      return "\\0" .. tmp2
    else
      local tmp2 = gsub(tostring(x), "-", "~")
      return "\\" .. tmp2
    end
  end
  scanString = function(scan)
    return function(s)
      local tmp1 = scan(function(a)
        local s1 = a[1]
        local i = a[2]
        if i < #s1 then
          local tmp2 = sub({s1, i})
          return {tag = "SOME", payload = {tmp2, {s1, _Int_add(i, 1)}}}
        else
          return {tag = "NONE"}
        end
      end)
      local exp = tmp1({s, 0})
      if exp.tag == "SOME" then
        return {tag = "SOME", payload = exp.payload[1]}
      elseif exp.tag == "NONE" then
        return {tag = "NONE"}
      else
        _raise(_Match, "string-cvt-1.sml:33:25")
      end
    end
  end
  MonoSequence = function(fromList, length, maxLen, unsafeCreate, create, unsafeFromListN, sub2, update, concat1, fromList1, length1, maxLen1, shallowSliceToVector, vector, unsafeFromListN1, unsafeFromListRevN, sub3)
    local tabulate1, sub4, update1, appi, app2, mapi, map2, foldli, foldri, foldl1, foldr, findi, find2, exists1, all1, collate, toList, append, prepend, array, tabulate2, sub5, update2, copy, copyVec, modifyi, modify, foldli1, foldri1, foldl2, foldr1, findi1, find3, exists2, all2, collate1, toList1, vector1, fromVector, length2, sub6, update3, full, slice, copy1, copyVec1, getItem, appi1, app3, modifyi1, modify1, foldli2, foldri2, foldl3, foldr2, findi2, find4, exists3, all3, collate2, vector2, UnsafeMonoVector, UnsafeMonoArray, MonoVectorSlice
    do
      tabulate1 = function(a)
        local n = a[1]
        local tmp1 = tabulate(n, a[2])
        return unsafeFromListN1({n, tmp1})
      end
      sub4 = function(a)
        local v = a[1]
        local i = a[2]
        local tmp1
        if 0 <= i then
          local tmp2 = length1(v)
          tmp1 = i < tmp2
        else
          tmp1 = false
          goto cont
        end
        ::cont::
        if tmp1 then
          return sub3({v, i})
        else
          _raise(_Subscript, "mono-sequence.sml:221:22")
        end
      end
      update1 = function(a)
        local v = a[1]
        local i = a[2]
        local x = a[3]
        local tmp1 = shallowSliceToVector({base = v, length = i, start = 0})
        local tmp2 = fromList1({x, nil})
        local tmp3 = _Int_add(i, 1)
        local tmp4 = length1(v)
        local tmp5 = shallowSliceToVector({base = v, length = _Int_sub(_Int_sub(tmp4, i), 1), start = tmp3})
        return concat1({tmp1, {tmp2, {tmp5, nil}}})
      end
      appi = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return nil
          end
          local tmp2 = sub3({v, i})
          f({i, tmp2})
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      app2 = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return nil
          end
          local tmp2 = sub3({v, i})
          f(tmp2)
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      mapi = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = tabulate(n, function(i)
            local tmp2 = sub3({v, i})
            return f({i, tmp2})
          end)
          return unsafeFromListN1({n, tmp1})
        end
      end
      map2 = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = tabulate(n, function(i)
            local tmp2 = sub3({v, i})
            return f(tmp2)
          end)
          return unsafeFromListN1({n, tmp1})
        end
      end
      foldli = function(f)
        return function(init)
          return function(v)
            local n = length1(v)
            local tmp1, tmp2 = 0, init
            ::cont::
            local i, acc = tmp1, tmp2
            if i >= n then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub3({v, i})
            local tmp5 = f({i, tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      foldri = function(f)
        return function(init)
          return function(v)
            local tmp1, tmp2
            do
              local tmp3 = length1(v)
              tmp2, tmp1 = _Int_sub(tmp3, 1), init
            end
            ::cont::
            local i, acc = tmp2, tmp1
            if i < 0 then
              return acc
            end
            local tmp3 = _Int_sub(i, 1)
            local tmp4 = sub3({v, i})
            local tmp5 = f({i, tmp4, acc})
            tmp2 = tmp3
            tmp1 = tmp5
            goto cont
          end
        end
      end
      foldl1 = function(f)
        return function(init)
          return function(v)
            local n = length1(v)
            local tmp1, tmp2 = 0, init
            ::cont::
            local i, acc = tmp1, tmp2
            if i >= n then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub3({v, i})
            local tmp5 = f({tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      local foldr3 = function(f, init)
        return function(v)
          local tmp1, tmp2
          do
            local tmp3 = length1(v)
            tmp2, tmp1 = _Int_sub(tmp3, 1), init
          end
          ::cont::
          local i, acc = tmp2, tmp1
          if i < 0 then
            return acc
          end
          local tmp3 = _Int_sub(i, 1)
          local tmp4 = sub3({v, i})
          local tmp5 = f({tmp4, acc})
          tmp2 = tmp3
          tmp1 = tmp5
          goto cont
        end
      end
      foldr = function(f)
        return function(init)
          return foldr3(f, init)
        end
      end
      findi = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return {tag = "NONE"}
          end
          local x = sub3({v, i})
          local tmp2 = f({i, x})
          if tmp2 then
            return {tag = "SOME", payload = {i, x}}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      find2 = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return {tag = "NONE"}
          end
          local x = sub3({v, i})
          local tmp2 = f(x)
          if tmp2 then
            return {tag = "SOME", payload = x}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      exists1 = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return false
          end
          local tmp2 = sub3({v, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            return true
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      all1 = function(f)
        return function(v)
          local n = length1(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return true
          end
          local tmp2 = sub3({v, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            tmp1 = _Int_add(i, 1)
            goto cont
          else
            return false
          end
        end
      end
      collate = function(compare1)
        return function(a)
          local xs = a[1]
          local ys = a[2]
          local xl = length1(xs)
          local yl = length1(ys)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          local tmp2 = xl <= i
          local tmp3 = yl <= i
          local tmp4
          tmp4 = tmp2 and tmp3
          if tmp4 then
            return EQUAL
          end
          local tmp5
          tmp5 = tmp2 and not tmp3
          if tmp5 then
            return LESS
          elseif not tmp2 and tmp3 then
            return GREATER
          elseif not tmp2 and not tmp3 then
            local tmp6 = sub3({xs, i})
            local tmp7 = sub3({ys, i})
            local t = compare1({tmp6, tmp7})
            if t == "EQUAL" then
              tmp1 = _Int_add(i, 1)
              goto cont
            else
              return t
            end
          else
            _raise(_Match, "mono-sequence.sml:310:49")
          end
        end
      end
      toList = function(a)
        local tmp1 = foldr3(function(a1)
          return {a1[1], a1[2]}
        end, nil)
        return tmp1(a)
      end
      append = function(a)
        local v = a[1]
        local tmp1 = fromList1({a[2], nil})
        return concat1({v, {tmp1, nil}})
      end
      prepend = function(a)
        local x = a[1]
        local v = a[2]
        local tmp1 = fromList1({x, nil})
        return concat1({tmp1, {v, nil}})
      end
      local length3 = function(tmp1)
        return tmp1.length
      end
      local sub7 = function(a)
        local base = a[1].base
        local start = a[1].start
        local length4 = a[1].length
        local i = a[2]
        if 0 <= i and i < length4 then
          return sub3({base, _Int_add(start, i)})
        else
          _raise(_Subscript, "mono-sequence.sml:331:44")
        end
      end
      local full1 = function(v)
        local tmp1 = length1(v)
        return {base = v, length = tmp1, start = 0}
      end
      local slice1 = function(a)
        if a[3].tag == "NONE" then
          local v = a[1]
          local i = a[2]
          local n = length1(v)
          if 0 <= i and i <= n then
            return {base = v, length = _Int_sub(n, i), start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:337:33")
          end
        else
          goto else1
        end
        ::else1::
        if a[3].tag == "SOME" then
          local v = a[1]
          local i = a[2]
          local n = a[3].payload
          local tmp1
          if 0 <= i then
            if 0 <= n then
              local tmp2 = _Int_add(i, n)
              local tmp3 = length1(v)
              tmp1 = tmp2 <= tmp3
            else
              tmp1 = false
              goto cont
            end
          else
            tmp1 = false
            goto cont
          end
          ::cont::
          if tmp1 then
            return {base = v, length = n, start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:342:32")
          end
        else
          _raise(_Match, "mono-sequence.sml:333:5")
        end
      end
      local concat2 = function(slices)
        local tmp1 = map(shallowSliceToVector)
        local tmp2 = tmp1(slices)
        return concat1(tmp2)
      end
      local getItem1 = function(a)
        local base = a.base
        local start = a.start
        local length4 = a.length
        if length4 > 0 then
          local tmp1 = sub3({base, start})
          local tmp2 = _Int_add(start, 1)
          return {tag = "SOME", payload = {tmp1, {base = base, length = _Int_sub(length4, 1), start = tmp2}}}
        else
          return {tag = "NONE"}
        end
      end
      local appi2 = function(f)
        return function(a)
          local base = a.base
          local start = a.start
          local length4 = a.length
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= length4 then
            return nil
          end
          local tmp2 = sub3({base, _Int_add(start, i)})
          f({i, tmp2})
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      local app4 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return nil
          end
          local tmp2 = sub3({base, i})
          f(tmp2)
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      local mapi1 = function(f)
        return function(a)
          local base = a.base
          local start = a.start
          local length4 = a.length
          local tmp1, tmp2 = 0, nil
          ::cont::
          local i, acc = tmp1, tmp2
          if i >= length4 then
            return unsafeFromListRevN({length4, acc})
          else
            goto else1
          end
          ::else1::
          local tmp3 = _Int_add(i, 1)
          local tmp4 = sub3({base, _Int_add(start, i)})
          local tmp5 = f({i, tmp4})
          tmp1 = tmp3
          tmp2 = {tmp5, acc}
          goto cont
        end
      end
      local map3 = function(f)
        return function(a)
          local base, length4, n, tmp1, tmp2
          do
            base = a.base
            local start = a.start
            length4 = a.length
            n = _Int_add(start, length4)
            tmp2, tmp1 = start, nil
          end
          ::cont::
          local i, acc = tmp2, tmp1
          if i >= n then
            return unsafeFromListRevN({length4, acc})
          else
            goto else1
          end
          ::else1::
          local tmp3 = _Int_add(i, 1)
          local tmp4 = sub3({base, i})
          local tmp5 = f(tmp4)
          tmp2 = tmp3
          tmp1 = {tmp5, acc}
          goto cont
        end
      end
      local foldli3 = function(f)
        return function(init)
          return function(a)
            local base = a.base
            local start = a.start
            local length4 = a.length
            local tmp1, tmp2 = 0, init
            ::cont::
            local i, acc = tmp1, tmp2
            if i >= length4 then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub3({base, _Int_add(start, i)})
            local tmp5 = f({i, tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      local foldri3 = function(f)
        return function(init)
          return function(a)
            local base = a.base
            local start = a.start
            local tmp1, tmp2 = _Int_sub(a.length, 1), init
            ::cont::
            local i, acc = tmp1, tmp2
            if i < 0 then
              return acc
            end
            local tmp3 = _Int_sub(i, 1)
            local tmp4 = sub3({base, _Int_add(start, i)})
            local tmp5 = f({i, tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      local foldl4 = function(f)
        return function(init)
          return function(a)
            local base, n, tmp1, tmp2
            do
              base = a.base
              local start = a.start
              n = _Int_add(start, a.length)
              tmp2, tmp1 = start, init
            end
            ::cont::
            local i, acc = tmp2, tmp1
            if i >= n then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub3({base, i})
            local tmp5 = f({tmp4, acc})
            tmp2 = tmp3
            tmp1 = tmp5
            goto cont
          end
        end
      end
      local foldr4 = function(f)
        return function(init)
          return function(a)
            local base = a.base
            local start = a.start
            local tmp1, tmp2 = _Int_sub(_Int_add(start, a.length), 1), init
            ::cont::
            local i, acc = tmp1, tmp2
            if i < start then
              return acc
            end
            local tmp3 = _Int_sub(i, 1)
            local tmp4 = sub3({base, i})
            local tmp5 = f({tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      local findi3 = function(f)
        return function(a)
          local base = a.base
          local start = a.start
          local length4 = a.length
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= length4 then
            return {tag = "NONE"}
          end
          local x = sub3({base, _Int_add(start, i)})
          local tmp2 = f({i, x})
          if tmp2 then
            return {tag = "SOME", payload = {i, x}}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      local find5 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return {tag = "NONE"}
          end
          local x = sub3({base, i})
          local tmp2 = f(x)
          if tmp2 then
            return {tag = "SOME", payload = x}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      local exists4 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return false
          end
          local tmp2 = sub3({base, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            return true
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      local all4 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return true
          end
          local tmp2 = sub3({base, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            tmp1 = _Int_add(i, 1)
            goto cont
          else
            return false
          end
        end
      end
      local collate3 = function(compare1)
        return function(a)
          local base, base_PRIME, xn, yn, tmp1, tmp2
          do
            base = a[1].base
            local start = a[1].start
            local length4 = a[1].length
            base_PRIME = a[2].base
            local start_PRIME = a[2].start
            local length_PRIME = a[2].length
            xn = _Int_add(start, length4)
            yn = _Int_add(start_PRIME, length_PRIME)
            tmp2, tmp1 = start, start_PRIME
          end
          ::cont::
          local i, j = tmp2, tmp1
          local tmp3 = xn <= i
          local tmp4 = yn <= j
          local tmp5
          tmp5 = tmp3 and tmp4
          if tmp5 then
            return EQUAL
          end
          local tmp6
          tmp6 = tmp3 and not tmp4
          if tmp6 then
            return LESS
          elseif not tmp3 and tmp4 then
            return GREATER
          elseif not tmp3 and not tmp4 then
            local tmp7 = sub3({base, i})
            local tmp8 = sub3({base_PRIME, j})
            local t = compare1({tmp7, tmp8})
            if t == "EQUAL" then
              tmp2 = _Int_add(i, 1)
              tmp1 = _Int_add(j, 1)
              goto cont
            else
              return t
            end
          else
            _raise(_Match, "mono-sequence.sml:454:29")
          end
        end
      end
      array = function(a)
        local n = a[1]
        local init = a[2]
        if n < 0 or maxLen < n then
          _raise(_Size, "mono-sequence.sml:470:27")
        else
          return unsafeCreate({n, init})
        end
      end
      tabulate2 = function(a)
        local n = a[1]
        local f = a[2]
        if maxLen < n then
          _raise(_Size, "mono-sequence.sml:475:27")
        end
        local tmp1 = tabulate(n, f)
        return unsafeFromListN({n, tmp1})
      end
      sub5 = function(a)
        local a1 = a[1]
        local i = a[2]
        local tmp1
        if 0 <= i then
          local tmp2 = length(a1)
          tmp1 = i < tmp2
        else
          tmp1 = false
          goto cont
        end
        ::cont::
        if tmp1 then
          return sub2({a1, i})
        else
          _raise(_Subscript, "mono-sequence.sml:482:22")
        end
      end
      update2 = function(a)
        local a1 = a[1]
        local i = a[2]
        local x = a[3]
        local tmp1
        if 0 <= i then
          local tmp2 = length(a1)
          tmp1 = i < tmp2
        else
          tmp1 = false
          goto cont
        end
        ::cont::
        if tmp1 then
          return update({a1, i, x})
        else
          _raise(_Subscript, "mono-sequence.sml:486:28")
        end
      end
      copy = function(a)
        local src = a.src
        local dst = a.dst
        local di = a.di
        local srcLen = length(src)
        local tmp1
        if 0 <= di then
          local tmp2 = _Int_add(di, srcLen)
          local tmp3 = length(dst)
          tmp1 = tmp2 <= tmp3
        else
          tmp1 = false
          goto cont
        end
        ::cont::
        if tmp1 then
          local tmp2 = 0
          ::cont1::
          local i = tmp2
          if i >= srcLen then
            return nil
          end
          local tmp3 = _Int_add(di, i)
          local tmp4 = sub2({src, i})
          update({dst, tmp3, tmp4})
          tmp2 = _Int_add(i, 1)
          goto cont1
        else
          _raise(_Subscript, "mono-sequence.sml:499:36")
        end
      end
      copyVec = function(a)
        local src = a.src
        local dst = a.dst
        local di = a.di
        local srcLen = length1(src)
        local tmp1
        if 0 <= di then
          local tmp2 = _Int_add(di, srcLen)
          local tmp3 = length(dst)
          tmp1 = tmp2 <= tmp3
        else
          tmp1 = false
          goto cont
        end
        ::cont::
        if tmp1 then
          local tmp2 = 0
          ::cont1::
          local i = tmp2
          if i >= srcLen then
            return nil
          end
          local tmp3 = _Int_add(di, i)
          local tmp4 = sub3({src, i})
          update({dst, tmp3, tmp4})
          tmp2 = _Int_add(i, 1)
          goto cont1
        else
          _raise(_Subscript, "mono-sequence.sml:513:39")
        end
      end
      modifyi = function(f)
        return function(a)
          local n = length(a)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return nil
          end
          local x = sub2({a, i})
          local y = f({i, x})
          update({a, i, y})
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      modify = function(f)
        return function(a)
          local n = length(a)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return nil
          end
          local x = sub2({a, i})
          local y = f(x)
          update({a, i, y})
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      foldli1 = function(f)
        return function(init)
          return function(v)
            local n = length(v)
            local tmp1, tmp2 = 0, init
            ::cont::
            local i, acc = tmp1, tmp2
            if i >= n then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub2({v, i})
            local tmp5 = f({i, tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      foldri1 = function(f)
        return function(init)
          return function(v)
            local tmp1, tmp2
            do
              local tmp3 = length(v)
              tmp2, tmp1 = _Int_sub(tmp3, 1), init
            end
            ::cont::
            local i, acc = tmp2, tmp1
            if i < 0 then
              return acc
            end
            local tmp3 = _Int_sub(i, 1)
            local tmp4 = sub2({v, i})
            local tmp5 = f({i, tmp4, acc})
            tmp2 = tmp3
            tmp1 = tmp5
            goto cont
          end
        end
      end
      foldl2 = function(f)
        return function(init)
          return function(v)
            local n = length(v)
            local tmp1, tmp2 = 0, init
            ::cont::
            local i, acc = tmp1, tmp2
            if i >= n then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub2({v, i})
            local tmp5 = f({tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      local foldr5 = function(f, init)
        return function(v)
          local tmp1, tmp2
          do
            local tmp3 = length(v)
            tmp2, tmp1 = _Int_sub(tmp3, 1), init
          end
          ::cont::
          local i, acc = tmp2, tmp1
          if i < 0 then
            return acc
          end
          local tmp3 = _Int_sub(i, 1)
          local tmp4 = sub2({v, i})
          local tmp5 = f({tmp4, acc})
          tmp2 = tmp3
          tmp1 = tmp5
          goto cont
        end
      end
      foldr1 = function(f)
        return function(init)
          return foldr5(f, init)
        end
      end
      findi1 = function(f)
        return function(v)
          local n = length(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return {tag = "NONE"}
          end
          local x = sub2({v, i})
          local tmp2 = f({i, x})
          if tmp2 then
            return {tag = "SOME", payload = {i, x}}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      find3 = function(f)
        return function(v)
          local n = length(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return {tag = "NONE"}
          end
          local x = sub2({v, i})
          local tmp2 = f(x)
          if tmp2 then
            return {tag = "SOME", payload = x}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      exists2 = function(f)
        return function(v)
          local n = length(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return false
          end
          local tmp2 = sub2({v, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            return true
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      all2 = function(f)
        return function(v)
          local n = length(v)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= n then
            return true
          end
          local tmp2 = sub2({v, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            tmp1 = _Int_add(i, 1)
            goto cont
          else
            return false
          end
        end
      end
      collate1 = function(compare1)
        return function(a)
          local xs = a[1]
          local ys = a[2]
          local xl = length(xs)
          local yl = length(ys)
          local tmp1 = 0
          ::cont::
          local i = tmp1
          local tmp2 = xl <= i
          local tmp3 = yl <= i
          local tmp4
          tmp4 = tmp2 and tmp3
          if tmp4 then
            return EQUAL
          end
          local tmp5
          tmp5 = tmp2 and not tmp3
          if tmp5 then
            return LESS
          elseif not tmp2 and tmp3 then
            return GREATER
          elseif not tmp2 and not tmp3 then
            local tmp6 = sub2({xs, i})
            local tmp7 = sub2({ys, i})
            local t = compare1({tmp6, tmp7})
            if t == "EQUAL" then
              tmp1 = _Int_add(i, 1)
              goto cont
            else
              return t
            end
          else
            _raise(_Match, "mono-sequence.sml:621:49")
          end
        end
      end
      toList1 = function(a)
        local tmp1 = foldr5(function(a1)
          return {a1[1], a1[2]}
        end, nil)
        return tmp1(a)
      end
      vector1 = function(a)
        local tmp1 = length(a)
        local tmp2
        do
          local tmp3 = foldr5(function(a1)
            return {a1[1], a1[2]}
          end, nil)
          tmp2 = tmp3(a)
        end
        return unsafeFromListN1({tmp1, tmp2})
      end
      fromVector = function(v)
        local tmp1 = length1(v)
        local tmp2
        do
          local tmp3 = foldr3(function(a)
            return {a[1], a[2]}
          end, nil)
          tmp2 = tmp3(v)
        end
        return unsafeFromListN({tmp1, tmp2})
      end
      length2 = function(tmp1)
        return tmp1.length
      end
      sub6 = function(a)
        local base = a[1].base
        local start = a[1].start
        local length4 = a[1].length
        local i = a[2]
        if 0 <= i and i < length4 then
          return sub2({base, _Int_add(start, i)})
        else
          _raise(_Subscript, "mono-sequence.sml:645:44")
        end
      end
      update3 = function(a)
        local base = a[1].base
        local start = a[1].start
        local length4 = a[1].length
        local i = a[2]
        local x = a[3]
        if 0 <= i and i < length4 then
          return update({base, _Int_add(start, i), x})
        else
          _raise(_Subscript, "mono-sequence.sml:649:50")
        end
      end
      full = function(v)
        local tmp1 = length(v)
        return {base = v, length = tmp1, start = 0}
      end
      slice = function(a)
        if a[3].tag == "NONE" then
          local v = a[1]
          local i = a[2]
          local n = length(v)
          if 0 <= i and i <= n then
            return {base = v, length = _Int_sub(n, i), start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:655:33")
          end
        else
          goto else1
        end
        ::else1::
        if a[3].tag == "SOME" then
          local v = a[1]
          local i = a[2]
          local n = a[3].payload
          local tmp1
          if 0 <= i then
            if 0 <= n then
              local tmp2 = _Int_add(i, n)
              local tmp3 = length(v)
              tmp1 = tmp2 <= tmp3
            else
              tmp1 = false
              goto cont
            end
          else
            tmp1 = false
            goto cont
          end
          ::cont::
          if tmp1 then
            return {base = v, length = n, start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:660:32")
          end
        else
          _raise(_Match, "mono-sequence.sml:651:5")
        end
      end
      copy1 = function(a)
        local base = a.src.base
        local start = a.src.start
        local length4 = a.src.length
        local dst = a.dst
        local di = a.di
        local tmp1
        do
          if di < 0 then
            tmp1 = true
            goto cont
          end
          local tmp2 = length(dst)
          tmp1 = tmp2 < _Int_add(di, length4)
        end
        ::cont::
        if tmp1 then
          _raise(_Subscript, "mono-sequence.sml:684:14")
        elseif start >= di then
          local tmp2 = 0
          ::cont2::
          local i = tmp2
          if i >= length4 then
            return nil
          end
          local tmp3 = _Int_add(di, i)
          local tmp4 = sub2({base, _Int_add(start, i)})
          update({dst, tmp3, tmp4})
          tmp2 = _Int_add(i, 1)
          goto cont2
        else
          goto else1
        end
        ::else1::
        local tmp2 = _Int_sub(length4, 1)
        ::cont1::
        local i = tmp2
        if i < 0 then
          return nil
        end
        local tmp3 = _Int_add(di, i)
        local tmp4 = sub2({base, _Int_add(start, i)})
        update({dst, tmp3, tmp4})
        tmp2 = _Int_sub(i, 1)
        goto cont1
      end
      copyVec1 = function(a)
        local base, start, length4, dst, di, tmp1
        do
          base = a.src.base
          start = a.src.start
          length4 = a.src.length
          dst = a.dst
          di = a.di
          if di < 0 then
            _raise(_Subscript, "mono-sequence.sml:699:14")
          end
          local tmp2 = length(dst)
          if tmp2 < _Int_add(di, length4) then
            _raise(_Subscript, "mono-sequence.sml:699:14")
          end
          tmp1 = 0
        end
        ::cont::
        local i = tmp1
        if i >= length4 then
          return nil
        end
        local tmp2 = _Int_add(di, i)
        local tmp3 = sub3({base, _Int_add(start, i)})
        update({dst, tmp2, tmp3})
        tmp1 = _Int_add(i, 1)
        goto cont
      end
      getItem = function(a)
        local base = a.base
        local start = a.start
        local length4 = a.length
        if length4 > 0 then
          local tmp1 = sub2({base, start})
          local tmp2 = _Int_add(start, 1)
          return {tag = "SOME", payload = {tmp1, {base = base, length = _Int_sub(length4, 1), start = tmp2}}}
        else
          return {tag = "NONE"}
        end
      end
      appi1 = function(f)
        return function(a)
          local base = a.base
          local start = a.start
          local length4 = a.length
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= length4 then
            return nil
          end
          local tmp2 = sub2({base, _Int_add(start, i)})
          f({i, tmp2})
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      app3 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return nil
          end
          local tmp2 = sub2({base, i})
          f(tmp2)
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      modifyi1 = function(f)
        return function(a)
          local base = a.base
          local start = a.start
          local length4 = a.length
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= length4 then
            return nil
          end
          local j = _Int_add(start, i)
          local x = sub2({base, j})
          local y = f({i, x})
          update({base, j, y})
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      modify1 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return nil
          end
          local x = sub2({base, i})
          local y = f(x)
          update({base, i, y})
          tmp1 = _Int_add(i, 1)
          goto cont
        end
      end
      foldli2 = function(f)
        return function(init)
          return function(a)
            local base = a.base
            local start = a.start
            local length4 = a.length
            local tmp1, tmp2 = 0, init
            ::cont::
            local i, acc = tmp1, tmp2
            if i >= length4 then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub2({base, _Int_add(start, i)})
            local tmp5 = f({i, tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      foldri2 = function(f)
        return function(init)
          return function(a)
            local base = a.base
            local start = a.start
            local tmp1, tmp2 = _Int_sub(a.length, 1), init
            ::cont::
            local i, acc = tmp1, tmp2
            if i < 0 then
              return acc
            end
            local tmp3 = _Int_sub(i, 1)
            local tmp4 = sub2({base, _Int_add(start, i)})
            local tmp5 = f({i, tmp4, acc})
            tmp1 = tmp3
            tmp2 = tmp5
            goto cont
          end
        end
      end
      foldl3 = function(f)
        return function(init)
          return function(a)
            local base, n, tmp1, tmp2
            do
              base = a.base
              local start = a.start
              n = _Int_add(start, a.length)
              tmp2, tmp1 = start, init
            end
            ::cont::
            local i, acc = tmp2, tmp1
            if i >= n then
              return acc
            end
            local tmp3 = _Int_add(i, 1)
            local tmp4 = sub2({base, i})
            local tmp5 = f({tmp4, acc})
            tmp2 = tmp3
            tmp1 = tmp5
            goto cont
          end
        end
      end
      local foldr6 = function(f, init)
        return function(a)
          local base = a.base
          local start = a.start
          local tmp1, tmp2 = _Int_sub(_Int_add(start, a.length), 1), init
          ::cont::
          local i, acc = tmp1, tmp2
          if i < start then
            return acc
          end
          local tmp3 = _Int_sub(i, 1)
          local tmp4 = sub2({base, i})
          local tmp5 = f({tmp4, acc})
          tmp1 = tmp3
          tmp2 = tmp5
          goto cont
        end
      end
      foldr2 = function(f)
        return function(init)
          return foldr6(f, init)
        end
      end
      findi2 = function(f)
        return function(a)
          local base = a.base
          local start = a.start
          local length4 = a.length
          local tmp1 = 0
          ::cont::
          local i = tmp1
          if i >= length4 then
            return {tag = "NONE"}
          end
          local x = sub2({base, _Int_add(start, i)})
          local tmp2 = f({i, x})
          if tmp2 then
            return {tag = "SOME", payload = {i, x}}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      find4 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return {tag = "NONE"}
          end
          local x = sub2({base, i})
          local tmp2 = f(x)
          if tmp2 then
            return {tag = "SOME", payload = x}
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      exists3 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return false
          end
          local tmp2 = sub2({base, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            return true
          else
            tmp1 = _Int_add(i, 1)
            goto cont
          end
        end
      end
      all3 = function(f)
        return function(a)
          local base, n, tmp1
          do
            base = a.base
            local length4 = a.length
            local start = a.start
            n = _Int_add(start, length4)
            tmp1 = start
          end
          ::cont::
          local i = tmp1
          if i >= n then
            return true
          end
          local tmp2 = sub2({base, i})
          local tmp3 = f(tmp2)
          if tmp3 then
            tmp1 = _Int_add(i, 1)
            goto cont
          else
            return false
          end
        end
      end
      collate2 = function(compare1)
        return function(a)
          local base, base_PRIME, xn, yn, tmp1, tmp2
          do
            base = a[1].base
            local start = a[1].start
            local length4 = a[1].length
            base_PRIME = a[2].base
            local start_PRIME = a[2].start
            local length_PRIME = a[2].length
            xn = _Int_add(start, length4)
            yn = _Int_add(start_PRIME, length_PRIME)
            tmp2, tmp1 = start, start_PRIME
          end
          ::cont::
          local i, j = tmp2, tmp1
          local tmp3 = xn <= i
          local tmp4 = yn <= j
          local tmp5
          tmp5 = tmp3 and tmp4
          if tmp5 then
            return EQUAL
          end
          local tmp6
          tmp6 = tmp3 and not tmp4
          if tmp6 then
            return LESS
          elseif not tmp3 and tmp4 then
            return GREATER
          elseif not tmp3 and not tmp4 then
            local tmp7 = sub2({base, i})
            local tmp8 = sub2({base_PRIME, j})
            local t = compare1({tmp7, tmp8})
            if t == "EQUAL" then
              tmp2 = _Int_add(i, 1)
              tmp1 = _Int_add(j, 1)
              goto cont
            else
              return t
            end
          else
            _raise(_Match, "mono-sequence.sml:812:29")
          end
        end
      end
      vector2 = function(a)
        local tmp1 = a.length
        local tmp2 = foldr6(function(a1)
          return {a1[1], a1[2]}
        end, nil)
        local tmp3 = tmp2(a)
        return unsafeFromListN1({tmp1, tmp3})
      end
      UnsafeMonoVector = {sub = sub3}
      UnsafeMonoArray = {create = create, sub = sub2, update = update}
      local subslice = function(a)
        if a[3].tag == "NONE" then
          local base = a[1].base
          local start = a[1].start
          local length4 = a[1].length
          local i = a[2]
          if 0 <= i and i <= length4 then
            local tmp1 = _Int_add(start, i)
            return {base = base, length = _Int_sub(length4, i), start = tmp1}
          else
            _raise(_Subscript, "mono-sequence.sml:346:55")
          end
        elseif a[3].tag == "SOME" then
          local base = a[1].base
          local start = a[1].start
          local length4 = a[1].length
          local i = a[2]
          local n = a[3].payload
          if 0 <= i and (0 <= n and _Int_add(i, n) <= length4) then
            return {base = base, length = n, start = _Int_add(start, i)}
          else
            _raise(_Subscript, "mono-sequence.sml:350:57")
          end
        else
          _raise(_Match, "mono-sequence.sml:343:5")
        end
      end
      local isEmpty = function(a)
        return a.length == 0
      end
      MonoVectorSlice = {all = all4, app = app4, appi = appi2, base = function(a)
        local b = a.base
        return {b, a.start, a.length}
      end, collate = collate3, concat = concat2, exists = exists4, find = find5, findi = findi3, foldl = foldl4, foldli = foldli3, foldr = foldr4, foldri = foldri3, full = full1, getItem = getItem1, isEmpty = isEmpty, length = length3, map = map3, mapi = mapi1, slice = slice1, sub = sub7, subslice = subslice, vector = vector}
    end
    local MonoVector = {all = all1, app = app2, append = append, appi = appi, collate = collate, concat = concat1, exists = exists1, find = find2, findi = findi, foldl = foldl1, foldli = foldli, foldr = foldr, foldri = foldri, fromList = fromList1, length = length1, map = map2, mapi = mapi, maxLen = maxLen1, prepend = prepend, sub = sub4, tabulate = tabulate1, toList = toList, update = update1}
    local subslice = function(a)
      if a[3].tag == "NONE" then
        local base = a[1].base
        local start = a[1].start
        local length3 = a[1].length
        local i = a[2]
        if 0 <= i and i <= length3 then
          local tmp1 = _Int_add(start, i)
          return {base = base, length = _Int_sub(length3, i), start = tmp1}
        else
          _raise(_Subscript, "mono-sequence.sml:664:55")
        end
      elseif a[3].tag == "SOME" then
        local base = a[1].base
        local start = a[1].start
        local length3 = a[1].length
        local i = a[2]
        local n = a[3].payload
        if 0 <= i and (0 <= n and _Int_add(i, n) <= length3) then
          return {base = base, length = n, start = _Int_add(start, i)}
        else
          _raise(_Subscript, "mono-sequence.sml:668:57")
        end
      else
        _raise(_Match, "mono-sequence.sml:661:5")
      end
    end
    local isEmpty = function(a)
      return a.length == 0
    end
    local MonoArraySlice = {all = all3, app = app3, appi = appi1, base = function(a)
      local b = a.base
      return {b, a.start, a.length}
    end, collate = collate2, copy = copy1, copyVec = copyVec1, exists = exists3, find = find4, findi = findi2, foldl = foldl3, foldli = foldli2, foldr = foldr2, foldri = foldri2, full = full, getItem = getItem, isEmpty = isEmpty, length = length2, modify = modify1, modifyi = modifyi1, slice = slice, sub = sub6, subslice = subslice, update = update3, vector = vector2}
    local appi2 = function(f)
      return function(a)
        local n = length(a)
        local tmp1 = 0
        ::cont::
        local i = tmp1
        if i >= n then
          return nil
        end
        local tmp2 = sub2({a, i})
        f({i, tmp2})
        tmp1 = _Int_add(i, 1)
        goto cont
      end
    end
    return {_MonoArray = {all = all2, app = function(f)
      return function(a)
        local n = length(a)
        local tmp1 = 0
        ::cont::
        local i = tmp1
        if i >= n then
          return nil
        end
        local tmp2 = sub2({a, i})
        f(tmp2)
        tmp1 = _Int_add(i, 1)
        goto cont
      end
    end, appi = appi2, array = array, collate = collate1, copy = copy, copyVec = copyVec, exists = exists2, find = find3, findi = findi1, foldl = foldl2, foldli = foldli1, foldr = foldr1, foldri = foldri1, fromList = fromList, fromVector = fromVector, length = length, maxLen = maxLen, modify = modify, modifyi = modifyi, sub = sub5, tabulate = tabulate2, toList = toList1, toVector = vector1, update = update2, vector = vector1}, _MonoArraySlice = MonoArraySlice, _MonoVector = MonoVector, _MonoVectorSlice = MonoVectorSlice, _UnsafeMonoArray = UnsafeMonoArray, _UnsafeMonoVector = UnsafeMonoVector}
  end
  local sliceToVector = function(a)
    local base = a.base
    local start = a.start
    return substring(base, start, a.length)
  end
  local unsafeCreateWithZero = function(n)
    return _Array_array(n, 0)
  end
  local unsafeFromListRevN = function(a)
    return implodeRev(a[2])
  end
  local unsafeFromListN = function(a)
    local tmp1 = _VectorOrArray_fromList(a[2])
    local tmp2
    do
      local tmp3 = tmp1.n
      tmp2 = _VectorOrArray_tabulate(tmp3, function(i)
        return string_char(tmp1[i + 1])
      end)
    end
    local result = table_concat(tmp2)
    return result
  end
  local unsafeUpdate = function(a)
    local arr = a[1]
    local i = a[2]
    arr[i + 1] = a[3]
    return nil
  end
  local unsafeSub = function(a)
    local arr = a[1]
    return arr[a[2] + 1]
  end
  local unsafeFromListN1 = function(a)
    return _VectorOrArray_fromList(a[2])
  end
  local unsafeCreate = function(a)
    local n = a[1]
    return _Array_array(n, a[2])
  end
  local length = function(arr)
    return arr.n
  end
  Base = MonoSequence(function(xs)
    return _VectorOrArray_fromList(xs)
  end, length, math_maxinteger, unsafeCreate, unsafeCreateWithZero, unsafeFromListN1, unsafeSub, unsafeUpdate, concat, implode, size, math_maxinteger, sliceToVector, sliceToVector, unsafeFromListN, unsafeFromListRevN, sub)
end
local extract1, full, string1, isEmpty, concat1, splitl, splitr, tokens, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, skipInitialWhitespace, digitToInt, scanSign, scanSubstring, scanZeroOrMoreDigits, scanOneOrMoreDigits, scanOptExpPart, Io_tag, BlockingNotSupported, NonblockingNotSupported, RandomAccessNotSupported, ClosedStream, NO_BUF, LINE_BUF, BLOCK_BUF, EQUALbuffer_mode, sub1, slice, exists1, ZERO, fromInt
do
  local CharVector = Base._MonoVector
  local CharVectorSlice = Base._MonoVectorSlice
  local CharArray = Base._MonoArray
  local CharArraySlice = Base._MonoArraySlice
  local UnsafeCharVector = Base._UnsafeMonoVector
  local sub2 = CharVectorSlice.sub
  extract1 = CharVectorSlice.slice
  full = CharVectorSlice.full
  string1 = CharVectorSlice.vector
  isEmpty = CharVectorSlice.isEmpty
  local _L = {}
  _L[1] = CharVectorSlice.getItem
  concat1 = CharVectorSlice.concat
  splitl = function(f)
    return function(s)
      local base = s.base
      local start = s.start
      local length = s.length
      local tmp20 = 0
      ::cont::
      local j = tmp20
      if j >= length then
        return {s, {base = base, length = 0, start = _Int_add(start, j)}}
      end
      local tmp21 = sub2({s, j})
      local tmp22 = f(tmp21)
      if tmp22 then
        tmp20 = _Int_add(j, 1)
        goto cont
      else
        local tmp23 = {base = base, length = j, start = start}
        return {tmp23, {base = base, length = _Int_sub(length, j), start = _Int_add(start, j)}}
      end
    end
  end
  splitr = function(f, s)
    local base = s.base
    local start = s.start
    local length = s.length
    local tmp20 = _Int_sub(length, 1)
    ::cont::
    local j = tmp20
    if j < 0 then
      return {{base = base, length = 0, start = start}, s}
    end
    local tmp21 = sub2({s, j})
    local tmp22 = f(tmp21)
    if tmp22 then
      tmp20 = _Int_sub(j, 1)
      goto cont
    else
      local j_PRIME = _Int_add(j, 1)
      local tmp23 = {base = base, length = j_PRIME, start = start}
      local tmp24 = _Int_add(start, j_PRIME)
      return {tmp23, {base = base, length = _Int_sub(length, j_PRIME), start = tmp24}}
    end
  end
  tokens = function(f)
    return function(a)
      local base, n, tmp20, tmp21, tmp22
      do
        base = a.base
        local start = a.start
        n = _Int_add(start, a.length)
        tmp22, tmp21, tmp20 = nil, start, start
      end
      ::cont::
      local revTokens, s, i = tmp22, tmp21, tmp20
      if i >= n then
        if s == i then
          return revAppend(revTokens, nil)
        else
          return revAppend({{base = base, length = _Int_sub(i, s), start = s}, revTokens}, nil)
        end
      else
        goto else1
      end
      ::else1::
      local tmp23 = sub({base, i})
      local tmp24 = f(tmp23)
      if tmp24 then
        local ip1 = _Int_add(i, 1)
        if s == i then
          tmp22 = revTokens
          tmp21 = ip1
          tmp20 = ip1
          goto cont
        else
          tmp22 = {{base = base, length = _Int_sub(i, s), start = s}, revTokens}
          tmp21 = ip1
          tmp20 = ip1
          goto cont
        end
      else
        tmp22 = revTokens
        tmp21 = s
        tmp20 = _Int_add(i, 1)
        goto cont
      end
    end
  end
  tmp1 = UnsafeCharVector.sub
  tmp2 = CharVectorSlice.concat
  tmp3 = CharVectorSlice.full
  tmp4 = CharVectorSlice.length
  tmp5 = CharVectorSlice.slice
  tmp6 = CharVectorSlice.vector
  tmp7 = CharVector.all
  tmp8 = CharVector.appi
  tmp9 = CharVector.exists
  tmp10 = CharVector.fromList
  tmp11 = CharVector.length
  tmp12 = CharArraySlice.base
  tmp13 = CharArraySlice.copyVec
  tmp14 = CharArraySlice.full
  tmp15 = CharArraySlice.length
  tmp16 = CharArraySlice.slice
  tmp17 = CharArraySlice.vector
  tmp18 = CharArray.array
  tmp19 = CharArray.copyVec
  skipInitialWhitespace = function(getc, strm)
    local tmp20, tmp21 = getc, strm
    ::cont::
    local getc1, strm1 = tmp20, tmp21
    local exp = getc1(strm1)
    if exp.tag == "NONE" then
      return strm1
    elseif exp.tag == "SOME" then
      local c = exp.payload[1]
      local strm_PRIME = exp.payload[2]
      local tmp22 = isSpace(c)
      if tmp22 then
        tmp20 = getc1
        tmp21 = strm_PRIME
        goto cont
      else
        return strm1
      end
    else
      _raise(_Match, "scan-num-utils.sml:2:42")
    end
  end
  digitToInt = function(c)
    if 48 <= c and c <= 57 then
      return _Int_sub(c, 48)
    elseif 97 <= c and c <= 102 then
      return _Int_add(_Int_sub(c, 97), 10)
    else
      return _Int_add(_Int_sub(c, 65), 10)
    end
  end
  scanSign = function(getc, strm)
    local exp = getc(strm)
    if exp.tag == "SOME" and exp.payload[1] == 43 then
      return {false, exp.payload[2]}
    elseif exp.tag == "SOME" and exp.payload[1] == 126 then
      return {true, exp.payload[2]}
    elseif exp.tag == "SOME" and exp.payload[1] == 45 then
      return {true, exp.payload[2]}
    else
      return {false, strm}
    end
  end
  scanSubstring = function(getc, strm, s)
    local tmp20, tmp21, tmp22 = getc, strm, s
    ::cont::
    local getc1, strm1, s1 = tmp20, tmp21, tmp22
    local exp = _L[1](s1)
    if exp.tag == "NONE" then
      return {tag = "SOME", payload = strm1}
    elseif exp.tag == "SOME" then
      local c = exp.payload[1]
      local s_PRIME = exp.payload[2]
      local exp1 = getc1(strm1)
      if exp1.tag == "SOME" then
        local strm_PRIME, tmp23
        do
          local c_PRIME = exp1.payload[1]
          strm_PRIME = exp1.payload[2]
          if c == c_PRIME then
            tmp23 = true
            goto cont1
          end
          local tmp24
          tmp24 = 65 <= c and c <= 90 or 97 <= c and c <= 122
          if tmp24 then
            local tmp25
            tmp25 = 65 <= c_PRIME and c_PRIME <= 90 or 97 <= c_PRIME and c_PRIME <= 122
            if tmp25 then
              local tmp26 = toUpper(c)
              local tmp27 = toUpper(c_PRIME)
              tmp23 = tmp26 == tmp27
            else
              tmp23 = false
              goto cont1
            end
          else
            tmp23 = false
            goto cont1
          end
        end
        ::cont1::
        if tmp23 then
          tmp20 = getc1
          tmp21 = strm_PRIME
          tmp22 = s_PRIME
          goto cont
        else
          return {tag = "NONE"}
        end
      elseif exp1.tag == "NONE" then
        return {tag = "NONE"}
      else
        _raise(_Match, "scan-num.sml:343:57")
      end
    else
      _raise(_Match, "scan-num.sml:341:37")
    end
  end
  scanZeroOrMoreDigits = function(getc, strm, revAcc)
    local tmp20, tmp21, tmp22 = getc, strm, revAcc
    ::cont::
    local getc1, strm1, revAcc1 = tmp20, tmp21, tmp22
    local exp = getc1(strm1)
    if exp.tag == "SOME" then
      local c = exp.payload[1]
      local strm_PRIME = exp.payload[2]
      if 48 <= c and c <= 57 then
        tmp20 = getc1
        tmp21 = strm_PRIME
        tmp22 = {c, revAcc1}
        goto cont
      else
        return {revAcc1, strm1}
      end
    elseif exp.tag == "NONE" then
      return {revAcc1, strm1}
    else
      _raise(_Match, "scan-num.sml:349:49")
    end
  end
  scanOneOrMoreDigits = function(getc, strm, revAcc)
    local exp = getc(strm)
    if exp.tag == "SOME" then
      local c = exp.payload[1]
      local strm_PRIME = exp.payload[2]
      if 48 <= c and c <= 57 then
        local tmp20 = scanZeroOrMoreDigits(getc, strm_PRIME, {c, revAcc})
        return {tag = "SOME", payload = tmp20}
      else
        return {tag = "NONE"}
      end
    elseif exp.tag == "NONE" then
      return {tag = "NONE"}
    else
      _raise(_Match, "scan-num.sml:355:48")
    end
  end
  scanOptExpPart = function(getc, strm, revAcc)
    local exp = getc(strm)
    if exp.tag == "SOME" then
      local c = exp.payload[1]
      local strm_PRIME = exp.payload[2]
      if c == 101 or c == 69 then
        local exp1 = scanSign(getc, strm_PRIME)
        local isNegative = exp1[1]
        local strm_PRIME_PRIME = exp1[2]
        local tmp20
        if isNegative then
          tmp20 = {45, {101, revAcc}}
        else
          tmp20 = {101, revAcc}
        end
        local exp2 = scanOneOrMoreDigits(getc, strm_PRIME_PRIME, tmp20)
        if exp2.tag == "SOME" then
          local revAcc1 = exp2.payload[1]
          return {revAcc1, exp2.payload[2]}
        elseif exp2.tag == "NONE" then
          return {revAcc, strm}
        else
          _raise(_Match, "scan-num.sml:370:73")
        end
      else
        return {revAcc, strm}
      end
    elseif exp.tag == "NONE" then
      return {revAcc, strm}
    else
      _raise(_Match, "scan-num.sml:367:43")
    end
  end
  Io_tag = {"Io"}
  BlockingNotSupported = setmetatable({tag = {"BlockingNotSupported"}}, _exn_meta)
  NonblockingNotSupported = setmetatable({tag = {"NonblockingNotSupported"}}, _exn_meta)
  RandomAccessNotSupported = setmetatable({tag = {"RandomAccessNotSupported"}}, _exn_meta)
  ClosedStream = setmetatable({tag = {"ClosedStream"}}, _exn_meta)
  NO_BUF = "NO_BUF"
  LINE_BUF = "LINE_BUF"
  BLOCK_BUF = "BLOCK_BUF"
  EQUALbuffer_mode = function(a, b)
    return a == "NO_BUF" and b == "NO_BUF" or (a == "LINE_BUF" and b == "LINE_BUF" or a == "BLOCK_BUF" and b == "BLOCK_BUF")
  end
  sub1 = function(base, length, start, i)
    if 0 <= i and i < length then
      return base[_Int_add(start, i) + 1]
    else
      _raise(_Subscript, "vector-slice.sml:39:44")
    end
  end
  slice = function(a, i, _3)
    if _3.tag == "NONE" then
      if 0 <= i and i <= a.n then
        return {base = a, length = _Int_sub(a.n, i), start = i}
      else
        _raise(_Subscript, "vector-slice.sml:44:30")
      end
    elseif _3.tag == "SOME" then
      local n = _3.payload
      local tmp20
      if 0 <= i then
        if 0 <= n then
          local tmp21 = _Int_add(i, n)
          tmp20 = tmp21 <= a.n
        else
          tmp20 = false
        end
      else
        tmp20 = false
      end
      if tmp20 then
        return {base = a, length = n, start = i}
      else
        _raise(_Subscript, "vector-slice.sml:48:32")
      end
    else
      _raise(_Match, "vector-slice.sml:41:5")
    end
  end
  exists1 = function(f)
    return function(a)
      local base = a.base
      local start = a.start
      local length = a.length
      local tmp20 = 0
      ::cont::
      local i = tmp20
      if i >= length then
        return false
      end
      local tmp21 = f(base[_Int_add(start, i) + 1])
      if tmp21 then
        return true
      else
        tmp20 = _Int_add(i, 1)
        goto cont
      end
    end
  end
  ZERO = {tag = "ZERO"}
  _L[2] = wordSize // 2
  _L[3] = 0x1 << _L[2]
  if _L[3] < 0x0 then
    _raise(_Overflow, "word-1.sml:52:39")
  else
    _L[4] = _L[3]
  end
  fromInt = function(x)
    if x == 0 then
      return ZERO
    elseif x > 0 then
      local function go(x1)
        if x1 == 0 then
          return nil
        end
        local q1 = _Int_div(x1, _L[4])
        local r1 = _Int_mod(x1, _L[4])
        local q2 = _Int_div(q1, _L[4])
        local r2 = _Int_mod(q1, _L[4])
        local tmp20 = r1 + r2 * _L[3]
        local tmp21 = go(q2)
        return {tmp20, tmp21}
      end
      local tmp20 = go(x)
      return {tag = "POSITIVE", payload = _VectorOrArray_fromList(tmp20)}
    else
      goto else1
    end
    ::else1::
    local function go(x1)
      if x1 == 0 then
        return nil
      end
      local q1 = quot(x1, _L[4])
      local r1 = rem(x1, _L[4])
      local q2 = quot(q1, _L[4])
      local r2 = rem(q1, _L[4])
      local tmp20 = _Int_negate(r1)
      local tmp21 = _Int_negate(r2)
      local tmp22 = tmp20 + tmp21 * _L[3]
      local tmp23 = go(q2)
      return {tmp22, tmp23}
    end
    local tmp20 = go(x)
    return {tag = "NEGATIVE", payload = _VectorOrArray_fromList(tmp20)}
  end
end
local _L = {}
do
  local add2 = function(x, y)
    local z = x + y
    if math_ult(z, x) then
      return {hi = 0x1, lo = z}
    else
      return {hi = 0x0, lo = z}
    end
  end
  local add3 = function(x, y, z)
    local exp = add2(x, y)
    local lo1 = exp.lo
    local hi1 = exp.hi
    local exp1 = add2(lo1, z)
    local lo2 = exp1.lo
    return {hi = hi1 + exp1.hi, lo = lo2}
  end
  local tmp20 = wordSize >> 1
  local word_lo_mask = (0x1 << tmp20) - 0x1
  _L[1] = function(words, words_PRIME)
    local m = words.n
    local t = compare({m, words_PRIME.n})
    if t == "EQUAL" then
      local tmp21 = _Int_sub(m, 1)
      ::cont::
      local i = tmp21
      if i < 0 then
        return EQUAL
      end
      local w = words[i + 1]
      local w_PRIME = words_PRIME[i + 1]
      local t1
      if w == w_PRIME then
        t1 = EQUAL
      elseif math_ult(w, w_PRIME) then
        t1 = LESS
      else
        t1 = GREATER
      end
      if t1 == "EQUAL" then
        tmp21 = _Int_sub(i, 1)
        goto cont
      else
        return t1
      end
    else
      return t
    end
  end
  _L[2] = function(words)
    local length
    do
      local tmp21 = words.n
      ::cont::
      local i = tmp21
      local i_PRIME = _Int_sub(i, 1)
      if i_PRIME < 0 then
        length = i
      elseif words[i_PRIME + 1] ~= 0x0 then
        length = i
      else
        tmp21 = i_PRIME
        goto cont
      end
    end
    if 0 <= length and length <= words.n then
      return _VectorOrArray_tabulate(length, function(i)
        return words[_Int_add(0, i) + 1]
      end)
    else
      _raise(_Subscript, "array-slice.sml:52:32")
    end
  end
  _L[3] = function(words, words_PRIME)
    local m, n, l, arr, tmp21, tmp22
    do
      m = words.n
      n = words_PRIME.n
      local tmp23
      if m < n then
        tmp23 = n
      else
        tmp23 = m
      end
      l = _Int_add(tmp23, 1)
      arr = _Array_array(l, 0x0)
      tmp22, tmp21 = 0x0, 0
    end
    ::cont::
    local carry, i = tmp22, tmp21
    if i == l then
      return _L[2](arr)
    else
      goto else1
    end
    ::else1::
    local w
    if i < m then
      w = words[i + 1]
    else
      w = 0x0
    end
    local w_PRIME
    if i < n then
      w_PRIME = words_PRIME[i + 1]
    else
      w_PRIME = 0x0
    end
    local exp = add3(w, w_PRIME, carry)
    local x = exp.lo
    local carry1 = exp.hi
    arr[i + 1] = x
    tmp22 = carry1
    tmp21 = _Int_add(i, 1)
    goto cont
  end
  _L[4] = function(words, words_PRIME)
    local m = words.n
    local n = words_PRIME.n
    local arr = _Array_array(m, 0x0)
    local tmp21, tmp22 = 0x0, 0
    ::cont::
    local carry, i = tmp21, tmp22
    if i == m then
      if carry == 0x0 then
        return _L[2](arr)
      else
        _raise(setmetatable({tag = _Fail_tag, payload = "subAbs: carry not zero"}, _exn_meta), "int-inf.sml:276:77")
      end
    else
      goto else1
    end
    ::else1::
    local w = words[i + 1]
    local w_PRIME
    if i < n then
      w_PRIME = words_PRIME[i + 1]
    else
      w_PRIME = 0x0
    end
    local carry1, lo
    if math_ult(w, w_PRIME) then
      lo = w - w_PRIME - carry
      carry1 = 0x1
    elseif math_ult(w_PRIME, w) then
      lo = w - w_PRIME - carry
      carry1 = 0x0
    elseif carry == 0x0 then
      lo = 0x0
      carry1 = 0x0
    else
      lo = - carry
      carry1 = 0x1
    end
    arr[i + 1] = lo
    tmp21 = carry1
    tmp22 = _Int_add(i, 1)
    goto cont
  end
  local mulAbs = function(words, words_PRIME)
    local m = words.n
    local n = words_PRIME.n
    local arr = _Array_array(_Int_add(m, n), 0x0)
    local tmp21 = 0
    ::cont::
    local j = tmp21
    if j >= n then
      return _L[2](arr)
    else
      goto else1
    end
    ::else1::
    local v = words_PRIME[j + 1]
    local tmp22, tmp23 = 0, 0x0
    ::cont1::
    local k, i, lo, hi
    do
      i, k = tmp22, tmp23
      if i >= m then
        arr[_Int_add(i, j) + 1] = k
        tmp21 = _Int_add(j, 1)
        goto cont
      end
      local u = words[i + 1]
      if u == 0x0 then
        lo = 0x0
        hi = 0x0
        goto cont2
      elseif v == 0x0 then
        lo = 0x0
        hi = 0x0
        goto cont2
      end
      local x_hi = u >> tmp20
      local x_lo = u & word_lo_mask
      local y_hi = v >> tmp20
      local y_lo = v & word_lo_mask
      local lo1 = x_lo * y_lo
      local mid1 = x_hi * y_lo
      local lo2 = mid1 << tmp20
      local hi1 = mid1 >> tmp20
      local mid2 = x_lo * y_hi
      local lo3 = mid2 << tmp20
      local hi2 = mid2 >> tmp20
      local hi3 = x_hi * y_hi
      local exp = add3(lo1, lo2, lo3)
      local lo4 = exp.lo
      local hi4 = exp.hi
      lo = lo4
      hi = hi1 + hi2 + hi3 + hi4
    end
    ::cont2::
    local exp = add3(lo, arr[_Int_add(i, j) + 1], k)
    local lo1 = exp.lo
    local hi_PRIME = exp.hi
    arr[_Int_add(i, j) + 1] = lo1
    local tmp24 = _Int_add(i, 1)
    tmp22 = tmp24
    tmp23 = hi + hi_PRIME
    goto cont1
  end
  _L[5] = function(x, y)
    if x.tag == "ZERO" then
      return y
    elseif y.tag == "ZERO" then
      return x
    elseif x.tag == "POSITIVE" and y.tag == "POSITIVE" then
      local words = x.payload
      local tmp21 = _L[3](words, y.payload)
      return {tag = "POSITIVE", payload = tmp21}
    else
      goto else1
    end
    ::else1::
    if x.tag == "POSITIVE" and y.tag == "NEGATIVE" then
      local words = x.payload
      local words_PRIME = y.payload
      local exp = _L[1](words, words_PRIME)
      if exp == "LESS" then
        local tmp21 = _L[4](words_PRIME, words)
        return {tag = "NEGATIVE", payload = tmp21}
      else
        goto else4
      end
      ::else4::
      if exp == "GREATER" then
        local tmp21 = _L[4](words, words_PRIME)
        return {tag = "POSITIVE", payload = tmp21}
      elseif exp == "EQUAL" then
        return ZERO
      else
        _raise(_Match, "int-inf.sml:347:46")
      end
    else
      goto else2
    end
    ::else2::
    if x.tag == "NEGATIVE" and y.tag == "POSITIVE" then
      local words = x.payload
      local words_PRIME = y.payload
      local exp = _L[1](words, words_PRIME)
      if exp == "LESS" then
        local tmp21 = _L[4](words_PRIME, words)
        return {tag = "POSITIVE", payload = tmp21}
      else
        goto else4
      end
      ::else4::
      if exp == "GREATER" then
        local tmp21 = _L[4](words, words_PRIME)
        return {tag = "NEGATIVE", payload = tmp21}
      elseif exp == "EQUAL" then
        return ZERO
      else
        _raise(_Match, "int-inf.sml:352:46")
      end
    else
      goto else3
    end
    ::else3::
    if x.tag == "NEGATIVE" and y.tag == "NEGATIVE" then
      local words = x.payload
      local tmp21 = _L[3](words, y.payload)
      return {tag = "NEGATIVE", payload = tmp21}
    else
      _raise(_Match, "int-inf.sml:344:5")
    end
  end
  _L[6] = function(z, z1)
    if z.tag == "ZERO" then
      return z
    elseif z1.tag == "ZERO" then
      return z1
    elseif z.tag == "POSITIVE" and z1.tag == "POSITIVE" then
      local words = z.payload
      local tmp21 = mulAbs(words, z1.payload)
      return {tag = "POSITIVE", payload = tmp21}
    else
      goto else1
    end
    ::else1::
    if z.tag == "POSITIVE" and z1.tag == "NEGATIVE" then
      local words = z.payload
      local tmp21 = mulAbs(words, z1.payload)
      return {tag = "NEGATIVE", payload = tmp21}
    else
      goto else2
    end
    ::else2::
    if z.tag == "NEGATIVE" and z1.tag == "POSITIVE" then
      local words = z.payload
      local tmp21 = mulAbs(words, z1.payload)
      return {tag = "NEGATIVE", payload = tmp21}
    else
      goto else3
    end
    ::else3::
    if z.tag == "NEGATIVE" and z1.tag == "NEGATIVE" then
      local words = z.payload
      local tmp21 = mulAbs(words, z1.payload)
      return {tag = "POSITIVE", payload = tmp21}
    else
      _raise(_Match, "int-inf.sml:375:5")
    end
  end
  _L[7] = function(x)
    if x == 0x0 then
      _raise(Domain, "int-inf.sml:422:20")
    elseif x == 0x1 then
      return 0
    end
    local tmp21 = _L[7](x >> 1)
    return _Int_add(1, tmp21)
  end
  _L[8] = function(words, amount)
    local major, minor, nn, n
    do
      major = _Word_div(amount, wordSize)
      minor = _Word_mod(amount, wordSize)
      nn = words.n
      local tmp21
      if major < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp21 = major
      end
      n = _Int_add(nn, tmp21)
      if minor == 0x0 then
        return _VectorOrArray_tabulate(n, function(i)
          local tmp22
          if major < 0x0 then
            _raise(_Overflow, "word-1.sml:52:39")
          else
            tmp22 = major
          end
          if i < tmp22 then
            return 0x0
          elseif major < 0x0 then
            _raise(_Overflow, "word-1.sml:52:39")
          else
            return words[_Int_sub(i, major) + 1]
          end
        end)
      else
        goto else1
      end
    end
    ::else1::
    local arr = _Array_array(_Int_add(n, 1), 0x0)
    local tmp21, tmp22 = 0x0, 0
    ::cont::
    local lo, i = tmp21, tmp22
    if i == _Int_add(nn, 1) then
      if lo == 0x0 then
        return _L[2](arr)
      else
        _raise(setmetatable({tag = _Fail_tag, payload = "LShiftAbs: carry not zero"}, _exn_meta), "int-inf.sml:567:84")
      end
    else
      goto else2
    end
    ::else2::
    local w
    if i < nn then
      w = words[i + 1]
    else
      w = 0x0
    end
    local v = w << minor | lo
    local hi = w >> wordSize - minor
    if major < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      arr[_Int_add(i, major) + 1] = v
      tmp21 = hi
      tmp22 = _Int_add(i, 1)
      goto cont
    end
  end
  _L[9] = function(s)
    local tmp21 = scanString(function(getc)
      return function(strm)
        local strm1
        do
          local strm2 = skipInitialWhitespace(getc, strm)
          local exp = scanSign(getc, strm2)
          local isNegative = exp[1]
          strm1 = exp[2]
          if isNegative then
            local tmp22 = fromInt(10)
            local exp1 = getc(strm1)
            if exp1.tag == "SOME" then
              local c = exp1.payload[1]
              local strm_PRIME = exp1.payload[2]
              if 48 <= c and c <= 57 then
                local tmp23, tmp24
                do
                  local tmp25 = digitToInt(c)
                  local tmp26 = _Int_negate(tmp25)
                  local tmp27 = fromInt(tmp26)
                  tmp24, tmp23 = tmp27, strm_PRIME
                end
                ::cont::
                local x, strm3 = tmp24, tmp23
                local exp2 = getc(strm3)
                if exp2.tag == "SOME" then
                  local c1 = exp2.payload[1]
                  local strm_PRIME1 = exp2.payload[2]
                  if 48 <= c1 and c1 <= 57 then
                    local tmp25, tmp26
                    do
                      tmp25 = _L[6](tmp22, x)
                      local tmp27 = digitToInt(c1)
                      tmp26 = fromInt(tmp27)
                      if tmp26.tag == "ZERO" then
                        tmp24 = tmp25
                        tmp23 = strm_PRIME1
                        goto cont
                      elseif tmp25.tag == "ZERO" and tmp26.tag == "POSITIVE" then
                        tmp24 = {tag = "NEGATIVE", payload = tmp26.payload}
                        tmp23 = strm_PRIME1
                        goto cont
                      elseif tmp25.tag == "ZERO" and tmp26.tag == "NEGATIVE" then
                        tmp24 = {tag = "POSITIVE", payload = tmp26.payload}
                        tmp23 = strm_PRIME1
                        goto cont
                      elseif tmp25.tag == "POSITIVE" and tmp26.tag == "POSITIVE" then
                        local words = tmp25.payload
                        local words_PRIME = tmp26.payload
                        local exp3 = _L[1](words, words_PRIME)
                        if exp3 == "LESS" then
                          local tmp28 = _L[4](words_PRIME, words)
                          tmp24 = {tag = "NEGATIVE", payload = tmp28}
                          tmp23 = strm_PRIME1
                          goto cont
                        else
                          goto else5
                        end
                        ::else5::
                        if exp3 == "GREATER" then
                          local tmp28 = _L[4](words, words_PRIME)
                          tmp24 = {tag = "POSITIVE", payload = tmp28}
                          tmp23 = strm_PRIME1
                          goto cont
                        elseif exp3 == "EQUAL" then
                          tmp24 = ZERO
                          tmp23 = strm_PRIME1
                          goto cont
                        else
                          _raise(_Match, "int-inf.sml:362:46")
                        end
                      else
                        goto else2
                      end
                    end
                    ::else2::
                    if tmp25.tag == "POSITIVE" and tmp26.tag == "NEGATIVE" then
                      local tmp27 = _L[3](tmp25.payload, tmp26.payload)
                      tmp24 = {tag = "POSITIVE", payload = tmp27}
                      tmp23 = strm_PRIME1
                      goto cont
                    else
                      goto else3
                    end
                    ::else3::
                    if tmp25.tag == "NEGATIVE" and tmp26.tag == "POSITIVE" then
                      local tmp27 = _L[3](tmp25.payload, tmp26.payload)
                      tmp24 = {tag = "NEGATIVE", payload = tmp27}
                      tmp23 = strm_PRIME1
                      goto cont
                    else
                      goto else4
                    end
                    ::else4::
                    if tmp25.tag == "NEGATIVE" and tmp26.tag == "NEGATIVE" then
                      local words = tmp25.payload
                      local words_PRIME = tmp26.payload
                      local exp3 = _L[1](words, words_PRIME)
                      if exp3 == "LESS" then
                        local tmp27 = _L[4](words_PRIME, words)
                        tmp24 = {tag = "POSITIVE", payload = tmp27}
                        tmp23 = strm_PRIME1
                        goto cont
                      else
                        goto else5
                      end
                      ::else5::
                      if exp3 == "GREATER" then
                        local tmp27 = _L[4](words, words_PRIME)
                        tmp24 = {tag = "NEGATIVE", payload = tmp27}
                        tmp23 = strm_PRIME1
                        goto cont
                      elseif exp3 == "EQUAL" then
                        tmp24 = ZERO
                        tmp23 = strm_PRIME1
                        goto cont
                      else
                        _raise(_Match, "int-inf.sml:369:46")
                      end
                    else
                      _raise(_Match, "int-inf.sml:359:5")
                    end
                  else
                    return {tag = "SOME", payload = {x, strm3}}
                  end
                elseif exp2.tag == "NONE" then
                  return {tag = "SOME", payload = {x, strm3}}
                else
                  _raise(_Match, "int-inf.sml:957:35")
                end
              else
                return {tag = "NONE"}
              end
            elseif exp1.tag == "NONE" then
              return {tag = "NONE"}
            else
              _raise(_Match, "int-inf.sml:963:25")
            end
          else
            goto else1
          end
        end
        ::else1::
        local tmp22 = fromInt(10)
        local exp = getc(strm1)
        if exp.tag == "SOME" then
          local c = exp.payload[1]
          local strm_PRIME = exp.payload[2]
          if 48 <= c and c <= 57 then
            local tmp23, tmp24
            do
              local tmp25 = digitToInt(c)
              local tmp26 = fromInt(tmp25)
              tmp24, tmp23 = tmp26, strm_PRIME
            end
            ::cont::
            local x, strm2 = tmp24, tmp23
            local exp1 = getc(strm2)
            if exp1.tag == "SOME" then
              local c1 = exp1.payload[1]
              local strm_PRIME1 = exp1.payload[2]
              if 48 <= c1 and c1 <= 57 then
                local tmp25 = _L[6](tmp22, x)
                local tmp26 = digitToInt(c1)
                local tmp27 = fromInt(tmp26)
                local tmp28 = _L[5](tmp25, tmp27)
                tmp24 = tmp28
                tmp23 = strm_PRIME1
                goto cont
              else
                return {tag = "SOME", payload = {x, strm2}}
              end
            elseif exp1.tag == "NONE" then
              return {tag = "SOME", payload = {x, strm2}}
            else
              _raise(_Match, "int-inf.sml:943:35")
            end
          else
            return {tag = "NONE"}
          end
        elseif exp.tag == "NONE" then
          return {tag = "NONE"}
        else
          _raise(_Match, "int-inf.sml:949:25")
        end
      end
    end)
    return tmp21(s)
  end
  _L[14] = 2.0
  _L[13], _L[12], _L[11] = _L[14], wordSize, 1.0
  ::cont5::
  local _L1 = {}
  _L1[1], _L1[2], _L1[3] = _L[13], _L[12], _L[11]
  if _L1[2] == 0 then
    _L[10] = _L1[3]
    goto cont
  elseif _L1[2] == 1 then
    _L[10] = _L1[3] * _L1[1]
    goto cont
  end
  _L1[4] = _L1[2] // 2
  if _L1[2] % 2 == 0 then
    _L[13] = _L1[1] * _L1[1]
    _L[12] = _L1[4]
    _L[11] = _L1[3]
    goto cont5
  else
    _L1[5] = _L1[1] * _L1[1]
    _L[13] = _L1[5]
    _L[12] = _L1[4]
    _L[11] = _L1[3] * _L1[1]
    goto cont5
  end
end
::cont::
local toRealAbs, Base1
do
  local function wordToReal(w)
    if w == 0x0 then
      return 0.0
    end
    local tmp20 = wordToReal(w >> 0x1F)
    local tmp21 = tmp20 * 0x1p31
    local tmp22 = w & 0x7FFFFFFF
    if tmp22 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      local tmp23 = 1.0
      local tmp24 = tmp22 * tmp23
      return tmp21 + tmp24
    end
  end
  local simpleNatToReal = function(words)
    local tmp20 = function(a)
      local w = a[1]
      local tmp21 = a[2] * _L[10]
      local tmp22 = wordToReal(w)
      return tmp21 + tmp22
    end
    local tmp21 = 0.0
    return foldr_PRIME(tmp20, tmp21, words, _Int_sub(words.n, 1))
  end
  toRealAbs = function(words)
    local k
    do
      local n = _Int_sub(words.n, 1)
      local tmp20 = _Int_mul(wordSize, n)
      local tmp21 = _L[7](words[n + 1])
      k = _Int_add(tmp20, tmp21)
    end
    if k < 53 then
      return simpleNatToReal(words)
    else
      goto else1
    end
    ::else1::
    if k >= 1024 then
      return huge
    end
    local e = _Int_add(_Int_sub(k, 53), 1)
    local q
    do
      local major, minor, n
      do
        major = _Word_div(e, wordSize)
        minor = _Word_mod(e, wordSize)
        local tmp20 = words.n
        local tmp21
        if major < 0x0 then
          _raise(_Overflow, "word-1.sml:52:39")
        else
          tmp21 = major
        end
        n = _Int_sub(tmp20, tmp21)
        if n <= 0 then
          q = {n = 0}
          goto cont
        elseif minor == 0x0 then
          local tmp22 = _VectorOrArray_tabulate(n, function(i)
            if major < 0x0 then
              _raise(_Overflow, "word-1.sml:52:39")
            else
              return words[_Int_add(i, major) + 1]
            end
          end)
          local tmp23 = exists1(function(x)
            return x ~= 0x0
          end)
          if major < 0x0 then
            _raise(_Overflow, "word-1.sml:52:39")
          end
          local tmp24 = slice(words, 0, {tag = "SOME", payload = major})
          tmp23(tmp24)
          q = tmp22
          goto cont
        else
          goto else4
        end
      end
      ::else4::
      local arr = _Array_array(n, 0x0)
      local hasRemainder
      do
        local tmp20, tmp21 = 0x0, _Int_sub(n, 1)
        ::cont2::
        local hi, i = tmp20, tmp21
        if i < 0 then
          hasRemainder = hi ~= 0x0
          goto cont1
        end
        local tmp22
        if major < 0x0 then
          _raise(_Overflow, "word-1.sml:52:39")
        else
          tmp22 = major
        end
        local w = words[_Int_add(i, tmp22) + 1]
        local v = w >> minor | hi
        local lo = w << wordSize - minor
        arr[i + 1] = v
        tmp20 = lo
        tmp21 = _Int_sub(i, 1)
        goto cont2
      end
      ::cont1::
      do
        if hasRemainder then
        else
          goto else5
        end
        q = _L[2](arr)
        goto cont
        ::else5::
        local tmp20 = exists1(function(x)
          return x ~= 0x0
        end)
        if major < 0x0 then
          _raise(_Overflow, "word-1.sml:52:39")
        end
        local tmp21 = slice(words, 0, {tag = "SOME", payload = major})
        tmp20(tmp21)
      end
      q = _L[2](arr)
    end
    ::cont::
    local exp
    do
      local tmp20 = _L[8]({n = 1, 0x1}, e)
      local tmp21 = _L[4](tmp20, {n = 1, 0x1})
      local r
      do
        local l, arr, tmp22
        do
          local m = words.n
          local n = tmp21.n
          if m < n then
            l = m
          else
            l = n
          end
          arr = _Array_array(l, 0x0)
          tmp22 = 0
        end
        ::cont1::
        local i = tmp22
        if i == l then
        else
          local w = words[i + 1]
          arr[i + 1] = w & tmp21[i + 1]
          tmp22 = _Int_add(i, 1)
          goto cont1
        end
        r = _L[2](arr)
      end
      local tmp22 = {n = 1, 0x1}
      local tmp23 = _Int_sub(e, 1)
      local tmp24 = _L[8](tmp22, tmp23)
      exp = _L[1](r, tmp24)
      if exp == "LESS" then
        local tmp25 = simpleNatToReal(q)
        return fromManExp(e, tmp25)
      else
        goto else2
      end
    end
    ::else2::
    if exp == "EQUAL" then
      if q[0 + 1] & 0x1 == 0x0 then
        local tmp20 = simpleNatToReal(q)
        return fromManExp(e, tmp20)
      else
        goto else4
      end
      ::else4::
      local tmp20 = _L[3](q, {n = 1, 0x1})
      local tmp21 = simpleNatToReal(tmp20)
      return fromManExp(e, tmp21)
    else
      goto else3
    end
    ::else3::
    if exp == "GREATER" then
      local tmp20 = _L[3](q, {n = 1, 0x1})
      local tmp21 = simpleNatToReal(tmp20)
      return fromManExp(e, tmp21)
    else
      _raise(_Match, "int-inf.sml:1073:33")
    end
  end
  local unsafeSub = function(a)
    local v = a[1]
    local tmp20 = sub({v, a[2]})
    local tmp21 = tmp20
    return tmp21 & 0xFF
  end
  _L[15] = function(xs)
    local tmp20 = map(function(x)
      if x < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      elseif x < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif x > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return x
      end
    end)
    local tmp21 = tmp20(xs)
    local tmp22 = _VectorOrArray_fromList(tmp21)
    local tmp23
    do
      local tmp24 = tmp22.n
      tmp23 = _VectorOrArray_tabulate(tmp24, function(i)
        return string_char(tmp22[i + 1])
      end)
    end
    local result = table_concat(tmp23)
    return result
  end
  _L[16] = function(a)
    local base = a.base
    local start = a.start
    return substring(base, start, a.length)
  end
  _L[17] = function(n)
    return _Array_array(n, 0x0)
  end
  _L[18] = function(a)
    local xs = a[2]
    local tmp20 = map(function(x)
      if x < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      elseif x < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif x > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return x
      end
    end)
    local tmp21 = tmp20(xs)
    return implodeRev(tmp21)
  end
  _L[19] = function(a)
    local xs = a[2]
    local tmp20 = map(function(x)
      if x < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      elseif x < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif x > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return x
      end
    end)
    local tmp21 = tmp20(xs)
    local tmp22 = _VectorOrArray_fromList(tmp21)
    local tmp23
    do
      local tmp24 = tmp22.n
      tmp23 = _VectorOrArray_tabulate(tmp24, function(i)
        return string_char(tmp22[i + 1])
      end)
    end
    local result = table_concat(tmp23)
    return result
  end
  _L[20] = function(a)
    local arr = a[1]
    local i = a[2]
    arr[i + 1] = a[3]
    return nil
  end
  _L[21] = function(a)
    local arr = a[1]
    return arr[a[2] + 1]
  end
  _L[22] = function(a)
    return _VectorOrArray_fromList(a[2])
  end
  _L[23] = function(a)
    local n = a[1]
    return _Array_array(n, a[2])
  end
  _L[24] = function(arr)
    return arr.n
  end
  Base1 = MonoSequence(function(xs)
    return _VectorOrArray_fromList(xs)
  end, _L[24], math_maxinteger, _L[23], _L[17], _L[22], _L[21], _L[20], concat, _L[15], size, math_maxinteger, _L[16], _L[16], _L[19], _L[18], unsafeSub)
end
do
  local Word8Vector = Base1._MonoVector
  local Word8VectorSlice = Base1._MonoVectorSlice
  local Word8Array = Base1._MonoArray
  _L[55] = Base1._MonoArraySlice
  _L[56] = Base1._UnsafeMonoArray
  _L[25] = Word8VectorSlice.concat
  _L[26] = Word8VectorSlice.full
  _L[27] = Word8VectorSlice.length
  _L[28] = Word8VectorSlice.slice
  _L[29] = Word8VectorSlice.vector
  _L[30] = Word8Vector.concat
  _L[31] = Word8Vector.fromList
  _L[32] = Word8Vector.length
  _L[33] = Word8Vector.sub
  _L[34] = Word8Vector.tabulate
  _L[35] = _L[55].base
  _L[36] = _L[55].copyVec
  _L[37] = _L[55].full
  _L[38] = _L[55].length
  _L[39] = _L[55].slice
  _L[40] = _L[55].vector
  _L[41] = Word8Array.array
  _L[42] = Word8Array.copyVec
  _L[43] = Word8Array.length
  _L[44] = Word8Array.maxLen
  _L[45] = Word8Array.sub
  _L[46] = Word8Array.tabulate
  _L[47] = Word8Array.update
  _L[48] = _L[56].sub
  _L[49] = _L[56].update
  _L[50] = function(a, i, bytesPerElem)
    if i < 0 then
      _raise(_Subscript, "pack-word.sml:29:48")
    end
    local ii = _Int_mul(bytesPerElem, i)
    local tmp20 = _L[43](a)
    if tmp20 < _Int_add(ii, bytesPerElem) then
      _raise(_Subscript, "pack-word.sml:33:55")
    else
      return ii
    end
  end
  _L[51] = function(needle)
    return function(haystack)
      local m = #needle
      local t = _Int_sub(#haystack, m)
      local tmp20 = 0
      ::cont::
      local i = tmp20
      if i > t then
        return false
      end
      local tmp21
      do
        local tmp22 = 0
        ::cont2::
        local j = tmp22
        if j >= m then
          tmp21 = true
          goto cont1
        end
        local tmp23 = tmp1({needle, j})
        local tmp24 = tmp1({haystack, _Int_add(i, j)})
        if tmp23 == tmp24 then
          tmp22 = _Int_add(j, 1)
          goto cont2
        else
          tmp21 = false
        end
      end
      ::cont1::
      if tmp21 then
        return true
      else
        tmp20 = _Int_add(i, 1)
        goto cont
      end
    end
  end
  _L[57] = {"year", 2001}
  _L[58] = {"month", 1}
  _L[59] = {"day", 1}
  _L[60] = {"hour", 0}
  _L[52] = time(_newTableWith({n = 5, _L[57], _L[58], _L[59], _L[60], {"isdst", false}}))
  _L[53] = setmetatable({tag = {"Time"}}, _exn_meta)
  _L[61] = {__mode = "k"}
  _L[54] = setmetatable({}, _L[61])
end
do
  local meta = {__mode = "v"}
  _L[62] = setmetatable({}, meta)
  local freeList = {{0, nil}}
  _L[63] = function(obj)
    local v = _L[54][obj]
    if v == nil then
      local d
      do
        local exp = freeList[1]
        if exp[2] == nil then
          local n = exp[1]
          freeList[1] = {_Int_add(n, 1), nil}
          d = n
        elseif exp[2] ~= nil then
          local n = exp[1]
          local tmp20 = exp[2][1]
          freeList[1] = {n, exp[2][2]}
          d = tmp20
        else
          _raise(_Match, "iodesc.sml:19:24")
        end
      end
      _L[54][obj] = d
      _L[62][d] = obj
      return d
    else
      return v
    end
  end
  _L[64] = function(i)
    local obj = _L[62][i]
    if obj ~= nil then
      local exp = freeList[1]
      local n = exp[1]
      freeList[1] = {n, {i, exp[2]}}
      _L[62][i] = nil
      _L[54][obj] = nil
      return nil
    else
      return nil
    end
  end
  _L[65] = {"SysErr"}
  local use_lfs = function(field, f)
    if lfs.tag == "SOME" then
      return f(lfs.payload[field])
    elseif lfs.tag == "NONE" then
      return function(a)
        _raise(setmetatable({tag = _L[65], payload = {"LuaFileSystem not available", {tag = "NONE"}}}, _exn_meta), "os.sml:56:70")
      end
    else
      _raise(_Match, "os.sml:54:50")
    end
  end
  _L[66] = use_lfs("chdir", function(lfs_chdir)
    return function(path)
      local tmp20, tmp21 = lfs_chdir(path)
      if not tmp20 then
        local message = checkString(tmp21)
        _raise(setmetatable({tag = _L[65], payload = {message, {tag = "SOME", payload = message}}}, _exn_meta), "os.sml:61:98")
      else
        return nil
      end
    end
  end)
  _L[67] = use_lfs("currentdir", function(lfs_currentdir)
    return function(a)
      local tmp20, tmp21 = lfs_currentdir()
      local tmp22 = type_PRIME(tmp20)
      if tmp22 == "string" then
        return tmp20
      end
      local message = checkString(tmp21)
      _raise(setmetatable({tag = _L[65], payload = {message, {tag = "SOME", payload = message}}}, _exn_meta), "os.sml:73:102")
    end
  end)
  _L[68] = use_lfs("mkdir", function(lfs_mkdir)
    return function(path)
      local tmp20, tmp21 = lfs_mkdir(path)
      if not tmp20 then
        local message = checkString(tmp21)
        _raise(setmetatable({tag = _L[65], payload = {message, {tag = "SOME", payload = message}}}, _exn_meta), "os.sml:81:98")
      else
        return nil
      end
    end
  end)
  _L[69] = use_lfs("attributes", function(lfs_attributes)
    return function(path)
      local tmp20
      do
        local tmp21
        tmp20, tmp21 = lfs_attributes(path, "modification")
        if not tmp20 then
          local message = checkString(tmp21)
          _raise(setmetatable({tag = _L[65], payload = {message, {tag = "SOME", payload = message}}}, _exn_meta), "os.sml:132:103")
        else
          goto else1
        end
      end
      ::else1::
      local tmp21 = difftime(tmp20, _L[52])
      local status, exn = pcall(function()
        return trunc(tmp21 * 1e6)
      end)
      if not status then
        _raise(_L[53], "time.sml:80:82")
      else
        return exn
      end
    end
  end)
  _L[70] = use_lfs("attributes", function(lfs_attributes)
    return function(path)
      local tmp20, tmp21 = lfs_attributes(path, "size")
      if not tmp20 then
        local message = checkString(tmp21)
        _raise(setmetatable({tag = _L[65], payload = {message, {tag = "SOME", payload = message}}}, _exn_meta), "os.sml:142:114")
      else
        return tmp20
      end
    end
  end)
  _L[73] = setmetatable({tag = {"Path"}}, _exn_meta)
  _L[74] = setmetatable({tag = {"InvalidArc"}}, _exn_meta)
  _L[75] = function(arc)
    local tmp20 = tmp7(function(c)
      return c ~= 47
    end)
    return tmp20(arc)
  end
  _L[71] = function(a)
    local dir = a.dir
    local file = a.file
    local arcs, isAbs, vol
    do
      local tmp20 = fields(function(c)
        return c == 47
      end)
      local xs = tmp20(dir)
      if xs ~= nil and (xs[1] == "" and xs[2] == nil) then
        vol = ""
        isAbs = false
        arcs = nil
      elseif xs ~= nil and xs[1] == "" then
        vol = ""
        isAbs = true
        arcs = xs[2]
      else
        vol = ""
        isAbs = false
        arcs = xs
      end
    end
    local tmp20 = {file, nil}
    local tmp21
    do
      local tmp22 = revAppend(arcs, nil)
      tmp21 = revAppend(tmp22, tmp20)
    end
    if vol ~= "" then
      _raise(_L[73], "os-path-unix.sml:24:41")
    elseif not isAbs and (tmp21 ~= nil and tmp21[1] == "") then
      _raise(_L[73], "os-path-unix.sml:27:65")
    end
    local tmp22 = all(_L[75])
    local tmp23 = tmp22(tmp21)
    if tmp23 then
      if isAbs then
        local tmp24 = _VectorOrArray_fromList(tmp21)
        local tmp25 = table_concat(tmp24, "/")
        return "/" .. tmp25
      else
        local tmp24 = _VectorOrArray_fromList(tmp21)
        local result = table_concat(tmp24, "/")
        return result
      end
    else
      _raise(_L[74], "os-path-unix.sml:34:54")
    end
  end
  _L[76] = function(path)
    local tmp20
    if #path >= 5 then
      local tmp21 = sub({path, 0})
      if tmp21 == 47 or tmp21 == 92 then
        local tmp22 = sub({path, 1})
        if tmp22 == 47 or tmp22 == 92 then
          local tmp23 = sub({path, 2})
          tmp20 = tmp23 ~= 47 and tmp23 ~= 92
        else
          tmp20 = false
          goto cont
        end
      else
        tmp20 = false
        goto cont
      end
    else
      tmp20 = false
      goto cont
    end
    ::cont::
    if tmp20 then
      local tmp21 = 3
      ::cont1::
      local i = tmp21
      if #path <= i then
        return {tag = "NONE"}
      end
      local tmp22 = sub({path, i})
      if tmp22 == 47 or tmp22 == 92 then
        local ip1 = _Int_add(i, 1)
        local tmp23
        if #path > ip1 then
          local tmp24 = sub({path, ip1})
          tmp23 = tmp24 ~= 47 and tmp24 ~= 92
        else
          tmp23 = false
          goto cont2
        end
        ::cont2::
        if tmp23 then
          local tmp24 = _Int_add(ip1, 1)
          ::cont3::
          local i1 = tmp24
          if #path <= i1 then
            return {tag = "SOME", payload = i1}
          end
          local tmp25 = sub({path, i1})
          if tmp25 == 47 or tmp25 == 92 then
            return {tag = "SOME", payload = i1}
          else
            tmp24 = _Int_add(i1, 1)
            goto cont3
          end
        else
          return {tag = "NONE"}
        end
      else
        tmp21 = _Int_add(i, 1)
        goto cont1
      end
    else
      return {tag = "NONE"}
    end
  end
  _L[77] = {tag = "DOS_VOLUME"}
  _L[78] = {tag = "NO_VOLUME"}
  _L[79] = function(arc)
    local tmp20 = tmp7(function(c)
      return c ~= 47 and c ~= 92
    end)
    return tmp20(arc)
  end
  _L[72] = function(a)
    local vol, isAbs, tmp20, tmp21
    do
      local dir = a.dir
      local file = a.file
      local arcs
      do
        local exp
        do
          local tmp22
          if #dir >= 2 then
            local tmp23 = sub({dir, 1})
            if tmp23 == 58 then
              local tmp24 = sub({dir, 0})
              tmp22 = 65 <= tmp24 and tmp24 <= 90 or 97 <= tmp24 and tmp24 <= 122
            else
              tmp22 = false
              goto cont4
            end
          else
            tmp22 = false
            goto cont4
          end
          ::cont4::
          if tmp22 then
            exp = _L[77]
            goto cont2
          end
          local exp1 = _L[76](dir)
          if exp1.tag == "SOME" then
            exp = {tag = "UNC_PATH", payload = exp1.payload}
          elseif exp1.tag == "NONE" then
            exp = _L[78]
          else
            _raise(_Match, "os-path-windows.sml:44:27")
          end
        end
        ::cont2::
        local isUNC, vol1, rest
        if exp.tag == "DOS_VOLUME" then
          local tmp22 = substring(dir, 0, 2)
          local tmp23 = extract(dir, 2, {tag = "NONE"})
          rest = tmp23
          vol1 = tmp22
          isUNC = false
          goto cont3
        else
          goto else1
        end
        ::else1::
        if exp.tag == "UNC_PATH" then
          local i = exp.payload
          local tmp22 = substring(dir, 0, i)
          local tmp23 = extract(dir, i, {tag = "NONE"})
          rest = tmp23
          vol1 = tmp22
          isUNC = true
        elseif exp.tag == "NO_VOLUME" then
          rest = dir
          vol1 = ""
          isUNC = false
          goto cont3
        else
          _raise(_Match, "os-path-windows.sml:75:52")
        end
        ::cont3::
        local tmp22 = fields(function(c)
          return c == 47 or c == 92
        end)
        local xs = tmp22(rest)
        if xs ~= nil and (xs[1] == "" and xs[2] == nil) then
          vol = vol1
          isAbs = isUNC
          arcs = nil
        elseif xs ~= nil and xs[1] == "" then
          vol = vol1
          isAbs = true
          arcs = xs[2]
        else
          vol = vol1
          isAbs = isUNC
          arcs = xs
        end
      end
      local tmp22 = {file, nil}
      do
        local tmp23 = revAppend(arcs, nil)
        tmp20 = revAppend(tmp23, tmp22)
      end
      if not isAbs and vol == "" then
        tmp21 = true
        goto cont
      end
      local tmp23
      if #vol == 2 then
        local tmp24 = sub({vol, 0})
        local tmp25
        tmp25 = 65 <= tmp24 and tmp24 <= 90 or 97 <= tmp24 and tmp24 <= 122
        if tmp25 then
          local tmp26 = sub({vol, 1})
          tmp23 = tmp26 == 58
        else
          tmp23 = false
          goto cont1
        end
      else
        tmp23 = false
        goto cont1
      end
      ::cont1::
      if tmp23 then
        tmp21 = true
        goto cont
      elseif isAbs then
        local exp = _L[76](vol)
        if exp.tag == "SOME" then
          local i = exp.payload
          tmp21 = i == #vol
        elseif exp.tag == "NONE" then
          tmp21 = false
        else
          _raise(_Match, "os-path-windows.sml:73:56")
        end
      else
        tmp21 = false
        goto cont
      end
    end
    ::cont::
    if not tmp21 then
      _raise(_L[73], "os-path-windows.sml:87:41")
    elseif not isAbs and #vol >= 3 then
      _raise(_L[73], "os-path-windows.sml:89:41")
    elseif not isAbs and (tmp20 ~= nil and tmp20[1] == "") then
      _raise(_L[73], "os-path-windows.sml:92:65")
    end
    local tmp22 = all(_L[79])
    local tmp23 = tmp22(tmp20)
    if tmp23 then
      if isAbs then
        local tmp24 = vol .. "\\"
        local tmp25 = _VectorOrArray_fromList(tmp20)
        local tmp26 = table_concat(tmp25, "\\")
        return tmp24 .. tmp26
      else
        local tmp24 = _VectorOrArray_fromList(tmp20)
        local tmp25 = table_concat(tmp24, "\\")
        return vol .. tmp25
      end
    else
      _raise(_L[74], "os-path-windows.sml:99:54")
    end
  end
end
local joinDirFile
do
  local tmp20 = package.config
  local tmp21 = sub({tmp20, 0})
  if tmp21 == 92 then
    joinDirFile = _L[72]
  else
    joinDirFile = _L[71]
  end
end
local PrimIO = function(array, copyVec, base, copyVec1, full1, length, slice1, vector, fromList, length1, full2, length2, slice2, vector1, compare1, someElem)
  local RD = function(payload)
    return payload
  end
  local WR = function(payload)
    return payload
  end
  local openVector = function(content)
    local pos = {0}
    local closed = {false}
    local readVec = function(n)
      if n < 0 then
        _raise(_Size, "prim-io.sml:112:31")
      end
      local current = pos[1]
      local total = length1(content)
      local tmp20 = _Int_add(current, n)
      local newPos
      if tmp20 < total then
        newPos = tmp20
      else
        newPos = total
      end
      pos[1] = newPos
      local tmp21 = slice2({content, current, {tag = "SOME", payload = _Int_sub(newPos, current)}})
      return vector1(tmp21)
    end
    local readArr = function(slice3)
      local current = pos[1]
      local total = length1(content)
      local tmp20 = length(slice3)
      local tmp21 = _Int_add(current, tmp20)
      local newPos
      if tmp21 < total then
        newPos = tmp21
      else
        newPos = total
      end
      local exp = base(slice3)
      local baseArr = exp[1]
      local start = exp[2]
      local count = _Int_sub(newPos, current)
      pos[1] = newPos
      local tmp22 = slice2({content, current, {tag = "SOME", payload = count}})
      copyVec1({di = start, dst = baseArr, src = tmp22})
      return count
    end
    local tmp20 = length1(content)
    local tmp21 = {tag = "SOME", payload = function(n)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readVec", name = "<openVector>"}}, _exn_meta), "prim-io.sml:108:36")
      else
        return readVec(n)
      end
    end}
    local tmp22 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readArr", name = "<openVector>"}}, _exn_meta), "prim-io.sml:108:36")
      else
        return readArr(slice3)
      end
    end}
    local tmp23 = {tag = "SOME", payload = function(n)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readVecNB", name = "<openVector>"}}, _exn_meta), "prim-io.sml:108:36")
      end
      local tmp24 = readVec(n)
      return {tag = "SOME", payload = tmp24}
    end}
    local tmp24 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readArrNB", name = "<openVector>"}}, _exn_meta), "prim-io.sml:108:36")
      end
      local tmp25 = readArr(slice3)
      return {tag = "SOME", payload = tmp25}
    end}
    local tmp25 = {tag = "SOME", payload = function(a)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "block", name = "<openVector>"}}, _exn_meta), "prim-io.sml:108:36")
      else
        return nil
      end
    end}
    local tmp26 = {tag = "SOME", payload = function(a)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "canInput", name = "<openVector>"}}, _exn_meta), "prim-io.sml:108:36")
      else
        return true
      end
    end}
    local tmp27 = function(a)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "avail", name = "<openVector>"}}, _exn_meta), "prim-io.sml:108:36")
      end
      local tmp28 = length1(content)
      return {tag = "SOME", payload = _Int_sub(tmp28, pos[1])}
    end
    local tmp28 = {tag = "NONE"}
    local tmp29 = {tag = "NONE"}
    local tmp30 = {tag = "NONE"}
    local tmp31 = {tag = "NONE"}
    local tmp32 = function(a)
      closed[1] = true
      return nil
    end
    return {avail = tmp27, block = tmp25, canInput = tmp26, chunkSize = tmp20, close = tmp32, endPos = tmp30, getPos = tmp28, ioDesc = {tag = "NONE"}, name = "<openVector>", readArr = tmp22, readArrNB = tmp24, readVec = tmp21, readVecNB = tmp23, setPos = tmp29, verifyPos = tmp31}
  end
  local nullRd = function(a)
    local closed = {false}
    local empty = fromList(nil)
    local tmp20 = {tag = "SOME", payload = function(n)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readVec", name = "<nullRd>"}}, _exn_meta), "prim-io.sml:149:46")
      elseif n < 0 then
        _raise(_Size, "prim-io.sml:155:81")
      else
        return empty
      end
    end}
    local tmp21 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readArr", name = "<nullRd>"}}, _exn_meta), "prim-io.sml:149:46")
      else
        return 0
      end
    end}
    local tmp22 = {tag = "SOME", payload = function(n)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readVecNB", name = "<nullRd>"}}, _exn_meta), "prim-io.sml:149:46")
      else
        return {tag = "SOME", payload = empty}
      end
    end}
    local tmp23 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "readArrNB", name = "<nullRd>"}}, _exn_meta), "prim-io.sml:149:46")
      else
        return {tag = "SOME", payload = 0}
      end
    end}
    local tmp24 = {tag = "SOME", payload = function(a1)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "block", name = "<nullRd>"}}, _exn_meta), "prim-io.sml:149:46")
      else
        return nil
      end
    end}
    local tmp25 = {tag = "SOME", payload = function(a1)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "canInput", name = "<nullRd>"}}, _exn_meta), "prim-io.sml:149:46")
      else
        return true
      end
    end}
    local tmp26 = function(a1)
      return {tag = "SOME", payload = 0}
    end
    local tmp27 = {tag = "NONE"}
    local tmp28 = {tag = "NONE"}
    local tmp29 = {tag = "NONE"}
    local tmp30 = {tag = "NONE"}
    local tmp31 = function(a1)
      closed[1] = true
      return nil
    end
    return {avail = tmp26, block = tmp24, canInput = tmp25, chunkSize = 1, close = tmp31, endPos = tmp29, getPos = tmp27, ioDesc = {tag = "NONE"}, name = "<nullRd>", readArr = tmp21, readArrNB = tmp23, readVec = tmp20, readVecNB = tmp22, setPos = tmp28, verifyPos = tmp30}
  end
  local nullWr = function(a)
    local closed = {false}
    local tmp20 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "writeVec", name = "<nullWr>"}}, _exn_meta), "prim-io.sml:173:46")
      else
        return length2(slice3)
      end
    end}
    local tmp21 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "writeArr", name = "<nullWr>"}}, _exn_meta), "prim-io.sml:173:46")
      else
        return length(slice3)
      end
    end}
    local tmp22 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "writeVecNB", name = "<nullWr>"}}, _exn_meta), "prim-io.sml:173:46")
      end
      local tmp23 = length2(slice3)
      return {tag = "SOME", payload = tmp23}
    end}
    local tmp23 = {tag = "SOME", payload = function(slice3)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "writeArrNB", name = "<nullWr>"}}, _exn_meta), "prim-io.sml:173:46")
      end
      local tmp24 = length(slice3)
      return {tag = "SOME", payload = tmp24}
    end}
    local tmp24 = {tag = "SOME", payload = function(a1)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "block", name = "<nullWr>"}}, _exn_meta), "prim-io.sml:173:46")
      else
        return nil
      end
    end}
    local tmp25 = {tag = "SOME", payload = function(a1)
      if closed[1] then
        _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "canOutput", name = "<nullWr>"}}, _exn_meta), "prim-io.sml:173:46")
      else
        return true
      end
    end}
    local tmp26 = {tag = "NONE"}
    local tmp27 = {tag = "NONE"}
    local tmp28 = {tag = "NONE"}
    local tmp29 = {tag = "NONE"}
    local tmp30 = function(a1)
      closed[1] = true
      return nil
    end
    return {block = tmp24, canOutput = tmp25, chunkSize = 1, close = tmp30, endPos = tmp27, getPos = tmp26, ioDesc = {tag = "NONE"}, name = "<nullWr>", setPos = tmp28, verifyPos = tmp29, writeArr = tmp21, writeArrNB = tmp23, writeVec = tmp20, writeVecNB = tmp22}
  end
  local augmentReader = function(a)
    local name, chunkSize, readVec, readArr, readVecNB, readArrNB, block, canInput, avail, getPos, setPos, endPos, verifyPos, close, ioDesc, readVec_PRIME
    do
      name = a.name
      chunkSize = a.chunkSize
      readVec = a.readVec
      readArr = a.readArr
      readVecNB = a.readVecNB
      readArrNB = a.readArrNB
      block = a.block
      canInput = a.canInput
      avail = a.avail
      getPos = a.getPos
      setPos = a.setPos
      endPos = a.endPos
      verifyPos = a.verifyPos
      close = a.close
      ioDesc = a.ioDesc
      local empty = fromList(nil)
      if readVec.tag == "SOME" then
        readVec_PRIME = readVec
        goto cont
      elseif readVec.tag == "NONE" then
        if readArr.tag == "SOME" then
          local ra = readArr.payload
          readVec_PRIME = {tag = "SOME", payload = function(n)
            local arr = array({n, someElem})
            local tmp20 = full1(arr)
            local actual = ra(tmp20)
            local tmp21 = slice1({arr, 0, {tag = "SOME", payload = actual}})
            return vector(tmp21)
          end}
          goto cont
        elseif readArr.tag == "NONE" then
          if block.tag == "SOME" and readVecNB.tag == "SOME" then
            local block_PRIME = block.payload
            local rvNB = readVecNB.payload
            readVec_PRIME = {tag = "SOME", payload = function(n)
              if n < 0 then
                _raise(_Size, "prim-io.sml:205:72")
              end
              local exp = rvNB(n)
              if exp.tag == "SOME" then
                return exp.payload
              elseif exp.tag == "NONE" then
                block_PRIME(nil)
                local exp1 = rvNB(n)
                if exp1.tag == "SOME" then
                  return exp1.payload
                elseif exp1.tag == "NONE" then
                  return empty
                else
                  _raise(_Match, "prim-io.sml:210:86")
                end
              else
                _raise(_Match, "prim-io.sml:207:72")
              end
            end}
          elseif block.tag == "SOME" and (readVecNB.tag == "NONE" and readArrNB.tag == "SOME") then
            local block_PRIME = block.payload
            local raNB = readArrNB.payload
            readVec_PRIME = {tag = "SOME", payload = function(n)
              if n < 0 then
                _raise(_Size, "prim-io.sml:217:72")
              end
              local arr = array({n, someElem})
              local aslice = full1(arr)
              local exp = raNB(aslice)
              if exp.tag == "SOME" then
                local tmp20 = slice1({arr, 0, {tag = "SOME", payload = exp.payload}})
                return vector(tmp20)
              else
                goto else1
              end
              ::else1::
              if exp.tag == "NONE" then
                block_PRIME(nil)
                local exp1 = raNB(aslice)
                if exp1.tag == "SOME" then
                  local tmp20 = slice1({arr, 0, {tag = "SOME", payload = exp1.payload}})
                  return vector(tmp20)
                elseif exp1.tag == "NONE" then
                  return empty
                else
                  _raise(_Match, "prim-io.sml:224:89")
                end
              else
                _raise(_Match, "prim-io.sml:221:75")
              end
            end}
          else
            readVec_PRIME = {tag = "NONE"}
          end
        else
          _raise(_Match, "prim-io.sml:196:38")
        end
      else
        _raise(_Match, "prim-io.sml:194:26")
      end
    end
    ::cont::
    local readArr_PRIME
    if readArr.tag == "SOME" then
      readArr_PRIME = readArr
      goto cont1
    elseif readArr.tag == "NONE" then
      if readVec.tag == "SOME" then
        local rv = readVec.payload
        readArr_PRIME = {tag = "SOME", payload = function(slice3)
          local tmp20 = length(slice3)
          local v = rv(tmp20)
          local exp = base(slice3)
          copyVec({di = exp[2], dst = exp[1], src = v})
          return length1(v)
        end}
        goto cont1
      elseif readVec.tag == "NONE" then
        if block.tag == "SOME" and readArrNB.tag == "SOME" then
          local block_PRIME = block.payload
          local raNB = readArrNB.payload
          readArr_PRIME = {tag = "SOME", payload = function(slice3)
            local exp = raNB(slice3)
            if exp.tag == "SOME" then
              return exp.payload
            elseif exp.tag == "NONE" then
              block_PRIME(nil)
              local exp1 = raNB(slice3)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:245:86")
              end
            else
              _raise(_Match, "prim-io.sml:242:72")
            end
          end}
        elseif block.tag == "SOME" and (readVecNB.tag == "SOME" and readArrNB.tag == "NONE") then
          local block_PRIME = block.payload
          local rvNB = readVecNB.payload
          readArr_PRIME = {tag = "SOME", payload = function(slice3)
            local n, base1, start, exp
            do
              n = length(slice3)
              local exp1 = base(slice3)
              base1 = exp1[1]
              start = exp1[2]
              exp = rvNB(n)
              if exp.tag == "SOME" then
                local v = exp.payload
                copyVec({di = start, dst = base1, src = v})
                return length1(v)
              else
                goto else1
              end
            end
            ::else1::
            if exp.tag == "NONE" then
              block_PRIME(nil)
              local exp1 = rvNB(n)
              if exp1.tag == "SOME" then
                local v = exp1.payload
                copyVec({di = start, dst = base1, src = v})
                return length1(v)
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:258:89")
              end
            else
              _raise(_Match, "prim-io.sml:253:75")
            end
          end}
        else
          readArr_PRIME = {tag = "NONE"}
        end
      else
        _raise(_Match, "prim-io.sml:233:38")
      end
    else
      _raise(_Match, "prim-io.sml:231:26")
    end
    ::cont1::
    local readVecNB_PRIME
    if readVecNB.tag == "SOME" then
      readVecNB_PRIME = readVecNB
      goto cont2
    elseif readVecNB.tag == "NONE" then
      if readArrNB.tag == "SOME" then
        local raNB = readArrNB.payload
        readVecNB_PRIME = {tag = "SOME", payload = function(n)
          if n < 0 then
            _raise(_Size, "prim-io.sml:271:75")
          end
          local arr = array({n, someElem})
          local tmp20 = full1(arr)
          local exp = raNB(tmp20)
          if exp.tag == "SOME" then
            local tmp21 = slice1({arr, 0, {tag = "SOME", payload = exp.payload}})
            local tmp22 = vector(tmp21)
            return {tag = "SOME", payload = tmp22}
          elseif exp.tag == "NONE" then
            return {tag = "NONE"}
          else
            _raise(_Match, "prim-io.sml:274:78")
          end
        end}
        goto cont2
      elseif readArrNB.tag == "NONE" then
        if canInput.tag == "SOME" and readVec.tag == "SOME" then
          local canInput_PRIME = canInput.payload
          local rv = readVec.payload
          readVecNB_PRIME = {tag = "SOME", payload = function(n)
            local tmp20 = canInput_PRIME(nil)
            if tmp20 then
              local tmp21 = rv(n)
              return {tag = "SOME", payload = tmp21}
            else
              return {tag = "NONE"}
            end
          end}
        elseif canInput.tag == "SOME" and (readVec.tag == "NONE" and readArr.tag == "SOME") then
          local canInput_PRIME = canInput.payload
          local ra = readArr.payload
          readVecNB_PRIME = {tag = "SOME", payload = function(n)
            local tmp20 = canInput_PRIME(nil)
            if tmp20 then
              if n < 0 then
                _raise(_Size, "prim-io.sml:289:78")
              end
              local arr = array({n, someElem})
              local tmp21 = full1(arr)
              local actual = ra(tmp21)
              local tmp22 = slice1({arr, 0, {tag = "SOME", payload = actual}})
              local tmp23 = vector(tmp22)
              return {tag = "SOME", payload = tmp23}
            else
              return {tag = "NONE"}
            end
          end}
        else
          readVecNB_PRIME = {tag = "NONE"}
        end
      else
        _raise(_Match, "prim-io.sml:269:40")
      end
    else
      _raise(_Match, "prim-io.sml:267:28")
    end
    ::cont2::
    local readArrNB_PRIME
    if readArrNB.tag == "SOME" then
      readArrNB_PRIME = readArrNB
      goto cont3
    elseif readArrNB.tag == "NONE" then
      if readVecNB.tag == "SOME" then
        local rvNB = readVecNB.payload
        readArrNB_PRIME = {tag = "SOME", payload = function(slice3)
          local tmp20 = length(slice3)
          local exp = rvNB(tmp20)
          if exp.tag == "SOME" then
            local v = exp.payload
            local exp1 = base(slice3)
            copyVec({di = exp1[2], dst = exp1[1], src = v})
            local tmp21 = length1(v)
            return {tag = "SOME", payload = tmp21}
          elseif exp.tag == "NONE" then
            return {tag = "NONE"}
          else
            _raise(_Match, "prim-io.sml:303:62")
          end
        end}
        goto cont3
      elseif readVecNB.tag == "NONE" then
        if canInput.tag == "SOME" and readArr.tag == "SOME" then
          local canInput_PRIME = canInput.payload
          local ra = readArr.payload
          readArrNB_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = canInput_PRIME(nil)
            if tmp20 then
              local tmp21 = ra(slice3)
              return {tag = "SOME", payload = tmp21}
            else
              return {tag = "NONE"}
            end
          end}
        elseif canInput.tag == "SOME" and (readVec.tag == "SOME" and readArr.tag == "NONE") then
          local canInput_PRIME = canInput.payload
          local rv = readVec.payload
          readArrNB_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = canInput_PRIME(nil)
            if tmp20 then
              local tmp21 = length(slice3)
              local v = rv(tmp21)
              local exp = base(slice3)
              copyVec({di = exp[2], dst = exp[1], src = v})
              local tmp22 = length1(v)
              return {tag = "SOME", payload = tmp22}
            else
              return {tag = "NONE"}
            end
          end}
        else
          readArrNB_PRIME = {tag = "NONE"}
        end
      else
        _raise(_Match, "prim-io.sml:301:40")
      end
    else
      _raise(_Match, "prim-io.sml:299:28")
    end
    ::cont3::
    return {avail = avail, block = block, canInput = canInput, chunkSize = chunkSize, close = close, endPos = endPos, getPos = getPos, ioDesc = ioDesc, name = name, readArr = readArr_PRIME, readArrNB = readArrNB_PRIME, readVec = readVec_PRIME, readVecNB = readVecNB_PRIME, setPos = setPos, verifyPos = verifyPos}
  end
  return {RD = RD, WR = WR, augmentReader = augmentReader, augmentWriter = function(a)
    local name = a.name
    local chunkSize = a.chunkSize
    local writeVec = a.writeVec
    local writeArr = a.writeArr
    local writeVecNB = a.writeVecNB
    local writeArrNB = a.writeArrNB
    local block = a.block
    local canOutput = a.canOutput
    local getPos = a.getPos
    local setPos = a.setPos
    local endPos = a.endPos
    local verifyPos = a.verifyPos
    local close = a.close
    local ioDesc = a.ioDesc
    local writeVec_PRIME
    if writeVec.tag == "SOME" then
      writeVec_PRIME = writeVec
      goto cont
    elseif writeVec.tag == "NONE" then
      if writeArr.tag == "SOME" then
        local wa = writeArr.payload
        writeVec_PRIME = {tag = "SOME", payload = function(slice3)
          local tmp20 = length2(slice3)
          local arr = array({tmp20, someElem})
          copyVec1({di = 0, dst = arr, src = slice3})
          local tmp21 = full1(arr)
          return wa(tmp21)
        end}
        goto cont
      elseif writeArr.tag == "NONE" then
        if block.tag == "SOME" and writeVecNB.tag == "SOME" then
          local block_PRIME = block.payload
          local wvNB = writeVecNB.payload
          writeVec_PRIME = {tag = "SOME", payload = function(slice3)
            local exp = wvNB(slice3)
            if exp.tag == "SOME" then
              return exp.payload
            elseif exp.tag == "NONE" then
              block_PRIME(nil)
              local exp1 = wvNB(slice3)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:362:78")
              end
            else
              _raise(_Match, "prim-io.sml:359:64")
            end
          end}
        elseif block.tag == "SOME" and (writeVecNB.tag == "NONE" and writeArrNB.tag == "SOME") then
          local block_PRIME = block.payload
          local waNB = writeArrNB.payload
          writeVec_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = length2(slice3)
            local arr = array({tmp20, someElem})
            local aslice = full1(arr)
            copyVec1({di = 0, dst = arr, src = slice3})
            local exp = waNB(aslice)
            if exp.tag == "SOME" then
              return exp.payload
            elseif exp.tag == "NONE" then
              block_PRIME(nil)
              local exp1 = waNB(aslice)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:375:81")
              end
            else
              _raise(_Match, "prim-io.sml:372:67")
            end
          end}
        else
          writeVec_PRIME = {tag = "NONE"}
        end
      else
        _raise(_Match, "prim-io.sml:348:39")
      end
    else
      _raise(_Match, "prim-io.sml:346:27")
    end
    ::cont::
    local writeArr_PRIME
    if writeArr.tag == "SOME" then
      writeArr_PRIME = writeArr
      goto cont1
    elseif writeArr.tag == "NONE" then
      if writeVec.tag == "SOME" then
        local wv = writeVec.payload
        writeArr_PRIME = {tag = "SOME", payload = function(slice3)
          local v = vector(slice3)
          local tmp20 = full2(v)
          return wv(tmp20)
        end}
        goto cont1
      elseif writeVec.tag == "NONE" then
        if block.tag == "SOME" and writeArrNB.tag == "SOME" then
          local block_PRIME = block.payload
          local waNB = writeArrNB.payload
          writeArr_PRIME = {tag = "SOME", payload = function(slice3)
            local exp = waNB(slice3)
            if exp.tag == "SOME" then
              return exp.payload
            elseif exp.tag == "NONE" then
              block_PRIME(nil)
              local exp1 = waNB(slice3)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:396:78")
              end
            else
              _raise(_Match, "prim-io.sml:393:64")
            end
          end}
        elseif block.tag == "SOME" and (writeVecNB.tag == "SOME" and writeArrNB.tag == "NONE") then
          local block_PRIME = block.payload
          local wvNB = writeVecNB.payload
          writeArr_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = vector(slice3)
            local vslice = full2(tmp20)
            local exp = wvNB(vslice)
            if exp.tag == "SOME" then
              return exp.payload
            elseif exp.tag == "NONE" then
              block_PRIME(nil)
              local exp1 = wvNB(vslice)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:407:81")
              end
            else
              _raise(_Match, "prim-io.sml:404:67")
            end
          end}
        else
          writeArr_PRIME = {tag = "NONE"}
        end
      else
        _raise(_Match, "prim-io.sml:384:39")
      end
    else
      _raise(_Match, "prim-io.sml:382:27")
    end
    ::cont1::
    local writeVecNB_PRIME
    if writeVecNB.tag == "SOME" then
      writeVecNB_PRIME = writeVecNB
      goto cont2
    elseif writeVecNB.tag == "NONE" then
      if writeArrNB.tag == "SOME" then
        local waNB = writeArrNB.payload
        writeVecNB_PRIME = {tag = "SOME", payload = function(slice3)
          local tmp20 = length2(slice3)
          local arr = array({tmp20, someElem})
          copyVec1({di = 0, dst = arr, src = slice3})
          local tmp21 = full1(arr)
          return waNB(tmp21)
        end}
        goto cont2
      elseif writeArrNB.tag == "NONE" then
        if canOutput.tag == "SOME" and writeVec.tag == "SOME" then
          local canOutput_PRIME = canOutput.payload
          local wv = writeVec.payload
          writeVecNB_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = canOutput_PRIME(nil)
            if tmp20 then
              local tmp21 = wv(slice3)
              return {tag = "SOME", payload = tmp21}
            else
              return {tag = "NONE"}
            end
          end}
        elseif canOutput.tag == "SOME" and (writeVec.tag == "NONE" and writeArr.tag == "SOME") then
          local canOutput_PRIME = canOutput.payload
          local wa = writeArr.payload
          writeVecNB_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = canOutput_PRIME(nil)
            if tmp20 then
              local tmp21 = length2(slice3)
              local arr = array({tmp21, someElem})
              copyVec1({di = 0, dst = arr, src = slice3})
              local tmp22 = full1(arr)
              local tmp23 = wa(tmp22)
              return {tag = "SOME", payload = tmp23}
            else
              return {tag = "NONE"}
            end
          end}
        else
          writeVecNB_PRIME = {tag = "NONE"}
        end
      else
        _raise(_Match, "prim-io.sml:416:41")
      end
    else
      _raise(_Match, "prim-io.sml:414:29")
    end
    ::cont2::
    local writeArrNB_PRIME
    if writeArrNB.tag == "SOME" then
      writeArrNB_PRIME = writeArrNB
      goto cont3
    elseif writeArrNB.tag == "NONE" then
      if writeVecNB.tag == "SOME" then
        local wvNB = writeVecNB.payload
        writeArrNB_PRIME = {tag = "SOME", payload = function(slice3)
          local tmp20 = vector(slice3)
          local vslice = full2(tmp20)
          return wvNB(vslice)
        end}
        goto cont3
      elseif writeVecNB.tag == "NONE" then
        if canOutput.tag == "SOME" and writeArr.tag == "SOME" then
          local canOutput_PRIME = canOutput.payload
          local wa = writeArr.payload
          writeArrNB_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = canOutput_PRIME(nil)
            if tmp20 then
              local tmp21 = wa(slice3)
              return {tag = "SOME", payload = tmp21}
            else
              return {tag = "NONE"}
            end
          end}
        elseif canOutput.tag == "SOME" and (writeVec.tag == "SOME" and writeArr.tag == "NONE") then
          local canOutput_PRIME = canOutput.payload
          local wv = writeVec.payload
          writeArrNB_PRIME = {tag = "SOME", payload = function(slice3)
            local tmp20 = canOutput_PRIME(nil)
            if tmp20 then
              local tmp21 = vector(slice3)
              local vslice = full2(tmp21)
              local tmp22 = wv(vslice)
              return {tag = "SOME", payload = tmp22}
            else
              return {tag = "NONE"}
            end
          end}
        else
          writeArrNB_PRIME = {tag = "NONE"}
        end
      else
        _raise(_Match, "prim-io.sml:445:41")
      end
    else
      _raise(_Match, "prim-io.sml:443:29")
    end
    ::cont3::
    return {block = block, canOutput = canOutput, chunkSize = chunkSize, close = close, endPos = endPos, getPos = getPos, ioDesc = ioDesc, name = name, setPos = setPos, verifyPos = verifyPos, writeArr = writeArr_PRIME, writeArrNB = writeArrNB_PRIME, writeVec = writeVec_PRIME, writeVecNB = writeVecNB_PRIME}
  end, compare = compare1, nullRd = nullRd, nullWr = nullWr, openVector = openVector}
end
local tmp20 = PrimIO(_L[41], _L[42], _L[35], _L[36], _L[37], _L[38], _L[39], _L[40], _L[31], _L[32], _L[26], _L[27], _L[28], _L[29], compare, 0x0)
_L[80] = tmp20.RD
_L[81] = tmp20.WR
_L[82] = PrimIO(tmp18, tmp19, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp10, tmp11, tmp3, tmp4, tmp5, tmp6, compare, 0)
_L[83] = _L[82].RD
_L[84] = _L[82].WR
_L[85] = function(canInput, closeIn, closeOut, endOfStream, filePosIn, filePosOut, flushOut, getBufferMode, getPosOut, getReader, getWriter, input, input1, inputAll, inputN, mkInstream, mkOutstream, output, output1, setBufferMode, setPosOut)
  local input2 = function(stream)
    local exp = input(stream[1])
    local chunk = exp[1]
    stream[1] = exp[2]
    return chunk
  end
  local input1_1 = function(stream)
    local exp = input1(stream[1])
    if exp.tag == "NONE" then
      return {tag = "NONE"}
    elseif exp.tag == "SOME" then
      local e = exp.payload[1]
      stream[1] = exp.payload[2]
      return {tag = "SOME", payload = e}
    else
      _raise(_Match, "imperative-io.sml:51:21")
    end
  end
  local inputN1 = function(a)
    local stream = a[1]
    local n = a[2]
    local exp = inputN({stream[1], n})
    local chunk = exp[1]
    stream[1] = exp[2]
    return chunk
  end
  local inputAll1 = function(stream)
    local exp = inputAll(stream[1])
    local chunk = exp[1]
    stream[1] = exp[2]
    return chunk
  end
  local canInput1 = function(a)
    local stream = a[1]
    local n = a[2]
    return canInput({stream[1], n})
  end
  local lookahead = function(stream)
    local exp = input1(stream[1])
    if exp.tag == "NONE" then
      return {tag = "NONE"}
    elseif exp.tag == "SOME" then
      return {tag = "SOME", payload = exp.payload[1]}
    else
      _raise(_Match, "imperative-io.sml:65:24")
    end
  end
  local closeIn1 = function(stream)
    return closeIn(stream[1])
  end
  local endOfStream1 = function(stream)
    return endOfStream(stream[1])
  end
  local output2 = function(a)
    local stream = a[1]
    local chunk = a[2]
    return output({stream[1], chunk})
  end
  local output1_1 = function(a)
    local stream = a[1]
    local elem = a[2]
    return output1({stream[1], elem})
  end
  local flushOut1 = function(stream)
    return flushOut(stream[1])
  end
  local closeOut1 = function(stream)
    return closeOut(stream[1])
  end
  local mkOutstream1 = function(x)
    return {x}
  end
  local getOutstream = function(a)
    return a[1]
  end
  local setOutstream = function(a)
    local x = a[1]
    x[1] = a[2]
    return nil
  end
  local getPosOut1 = function(stream)
    return getPosOut(stream[1])
  end
  local setPosOut1 = function(a)
    local stream = a[1]
    local tmp21 = setPosOut(a[2])
    stream[1] = tmp21
    return nil
  end
  local setInstream = function(a)
    local stream = a[1]
    stream[1] = a[2]
    return nil
  end
  local mkInstream1 = function(stream)
    return {stream}
  end
  local getInstream = function(stream)
    return stream[1]
  end
  return {_StreamIO = {canInput = canInput, closeIn = closeIn, closeOut = closeOut, endOfStream = endOfStream, filePosIn = filePosIn, filePosOut = filePosOut, flushOut = flushOut, getBufferMode = getBufferMode, getPosOut = getPosOut, getReader = getReader, getWriter = getWriter, input = input, input1 = input1, inputAll = inputAll, inputN = inputN, mkInstream = mkInstream, mkOutstream = mkOutstream, output = output, output1 = output1, setBufferMode = setBufferMode, setPosOut = setPosOut}, canInput = canInput1, closeIn = closeIn1, closeOut = closeOut1, endOfStream = endOfStream1, flushOut = flushOut1, getInstream = getInstream, getOutstream = getOutstream, getPosOut = getPosOut1, input = input2, input1 = input1_1, inputAll = inputAll1, inputN = inputN1, lookahead = lookahead, mkInstream = mkInstream1, mkOutstream = mkOutstream1, output = output2, output1 = output1_1, setInstream = setInstream, setOutstream = setOutstream, setPosOut = setPosOut1}
end
_L[86] = function(a)
  local rd = a[1]
  local content = a[2]
  local tip
  if rd.ioDesc.tag == "SOME" then
    local name = rd.name
    local ioDesc = rd.ioDesc.payload
    tip = {{tag = "READABLE", payload = {name = name, readable = _L[62][ioDesc]}}}
  else
    tip = {{tag = "PRIM_READER", payload = rd}}
  end
  local tmp21 = tmp11(content)
  if tmp21 > 0 then
    return {{tag = "BUFFERED", payload = {buffer = content, initialPosition = {tag = "NONE"}, next = tip, position = 0}}}
  else
    return tip
  end
end
_L[87] = function(ins)
  local tmp21, tmp22 = ins, nil
  ::cont::
  local ins1, acc = tmp21, tmp22
  local exp = ins1[1]
  if exp.tag == "READABLE" then
    local readable = exp.payload.readable
    local name = exp.payload.name
    ins1[1] = {tag = "CLOSED", payload = name}
    local tmp23
    do
      local ioDesc = _L[63](readable)
      local tmp24 = {tag = "SOME", payload = function(n)
        local exp1
        do
          local result = readable:read(n)
          if not result then
            exp1 = {tag = "NONE"}
          else
            exp1 = {tag = "SOME", payload = result}
          end
        end
        if exp1.tag == "NONE" then
          return ""
        elseif exp1.tag == "SOME" then
          return exp1.payload
        else
          _raise(_Match, "text-io.sml:35:28")
        end
      end}
      local tmp25 = {tag = "NONE"}
      local tmp26 = {tag = "NONE"}
      local tmp27 = {tag = "NONE"}
      local tmp28 = {tag = "NONE"}
      local tmp29 = {tag = "NONE"}
      local tmp30 = function(a)
        return {tag = "NONE"}
      end
      local tmp31 = {tag = "NONE"}
      local tmp32 = {tag = "NONE"}
      local tmp33 = {tag = "NONE"}
      local tmp34 = {tag = "NONE"}
      local tmp35 = function(a)
        _L[64](ioDesc)
        readable:close()
        return nil
      end
      tmp23 = _L[83]({avail = tmp30, block = tmp28, canInput = tmp29, chunkSize = 1024, close = tmp35, endPos = tmp33, getPos = tmp31, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, readArr = tmp25, readArrNB = tmp27, readVec = tmp24, readVecNB = tmp26, setPos = tmp32, verifyPos = tmp34})
    end
    local tmp24 = revAppend(acc, nil)
    local tmp25 = tmp2(tmp24)
    return {tmp23, tmp25}
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
    local rd = exp.payload
    ins1[1] = {tag = "CLOSED", payload = exp.payload.name}
    local tmp23 = revAppend(acc, nil)
    local tmp24 = tmp2(tmp23)
    return {rd, tmp24}
  else
    goto else2
  end
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local next = exp.payload.next
    local tmp23 = tmp5({buffer, position, {tag = "NONE"}})
    tmp21 = next
    tmp22 = {tmp23, acc}
    goto cont
  elseif exp.tag == "CLOSED" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "getReader", name = exp.payload}}, _exn_meta), "text-io.sml:113:57")
  else
    _raise(_Match, "text-io.sml:109:38")
  end
end
_L[88] = function(ins)
  local exp = ins[1]
  if exp.tag == "READABLE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = exp.payload.name}}, _exn_meta), "text-io.sml:118:55")
  elseif exp.tag == "PRIM_READER" and exp.payload.getPos.tag == "NONE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = exp.payload.name}}, _exn_meta), "text-io.sml:119:89")
  elseif exp.tag == "PRIM_READER" and exp.payload.getPos.tag == "SOME" then
    return exp.payload.getPos.payload(nil)
  else
    goto else1
  end
  ::else1::
  if exp.tag == "BUFFERED" and exp.payload.initialPosition.tag == "NONE" then
    local tmp21
    do
      local tmp22 = exp.payload.next
      ::cont::
      local ins1 = tmp22
      local exp1 = ins1[1]
      if exp1.tag == "READABLE" then
        tmp21 = exp1.payload.name
      elseif exp1.tag == "PRIM_READER" then
        tmp21 = exp1.payload.name
      elseif exp1.tag == "BUFFERED" then
        tmp22 = exp1.payload.next
        goto cont
      elseif exp1.tag == "CLOSED" then
        tmp21 = exp1.payload
      else
        _raise(_Match, "text-io.sml:93:23")
      end
    end
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = tmp21}}, _exn_meta), "text-io.sml:121:92")
  elseif exp.tag == "BUFFERED" and exp.payload.initialPosition.tag == "SOME" then
    local position, initialPosition, tmp21
    do
      position = exp.payload.position
      local next = exp.payload.next
      initialPosition = exp.payload.initialPosition.payload
      tmp21 = next[1]
    end
    ::cont::
    local a = tmp21
    if a.tag == "PRIM_READER" and (a.payload.getPos.tag == "SOME" and (a.payload.setPos.tag == "SOME" and a.payload.readVec.tag == "SOME")) then
      local getPos = a.payload.getPos.payload
      local setPos = a.payload.setPos.payload
      local readVec = a.payload.readVec.payload
      local savedPosition = getPos(nil)
      setPos(initialPosition)
      readVec(position)
      local tmp22 = getPos(nil)
      setPos(savedPosition)
      return tmp22
    elseif a.tag == "BUFFERED" then
      tmp21 = a.payload.next[1]
      goto cont
    elseif a.tag == "CLOSED" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "filePosIn", name = a.payload}}, _exn_meta), "text-io.sml:130:56")
    elseif a.tag == "PRIM_READER" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = a.payload.name}}, _exn_meta), "text-io.sml:131:86")
    elseif a.tag == "READABLE" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = a.payload.name}}, _exn_meta), "text-io.sml:132:67")
    else
      _raise(_Match, "text-io.sml:123:37")
    end
  elseif exp.tag == "CLOSED" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "filePosIn", name = exp.payload}}, _exn_meta), "text-io.sml:135:44")
  else
    _raise(_Match, "text-io.sml:117:25")
  end
end
_L[89] = function(stream, f, n)
  local exp
  do
    local result = stream.readable:read(n)
    if not result then
      exp = {tag = "NONE"}
    else
      exp = {tag = "SOME", payload = result}
    end
  end
  if exp.tag == "SOME" then
    local chunk = exp.payload
    local tmp21 = {{tag = "READABLE", payload = stream}}
    f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = {tag = "NONE"}, next = tmp21, position = 0}}
    return true
  elseif exp.tag == "NONE" then
    return false
  else
    _raise(_Match, "text-io.sml:137:11")
  end
end
_L[90] = function(rd, f, n)
  local name = rd.name
  local readVec = rd.readVec
  local readArr = rd.readArr
  local readVecNB = rd.readVecNB
  local readArrNB = rd.readArrNB
  local block = rd.block
  local initialPosition
  if rd.getPos.tag == "SOME" then
    local tmp21 = rd.getPos.payload(nil)
    initialPosition = {tag = "SOME", payload = tmp21}
  else
    initialPosition = {tag = "NONE"}
    goto cont
  end
  ::cont::
  local chunk
  if readVec.tag == "SOME" then
  else
    goto else1
  end
  chunk = readVec.payload(n)
  goto cont1
  ::else1::
  if readVec.tag == "NONE" then
    if readArr.tag == "SOME" then
      local ra = readArr.payload
      local arr = tmp18({n, 0})
      local tmp21 = tmp14(arr)
      local actual = ra(tmp21)
      local tmp22 = tmp16({arr, 0, {tag = "SOME", payload = actual}})
      chunk = tmp17(tmp22)
      goto cont1
    else
      goto else2
    end
    ::else2::
    if readArr.tag == "NONE" then
      if block.tag == "SOME" and readVecNB.tag == "SOME" then
        local block_PRIME = block.payload
        local rvNB = readVecNB.payload
        local exp = rvNB(n)
        if exp.tag == "SOME" then
          chunk = exp.payload
          goto cont1
        elseif exp.tag == "NONE" then
          block_PRIME(nil)
          local exp1 = rvNB(n)
          if exp1.tag == "SOME" then
            chunk = exp1.payload
            goto cont1
          elseif exp1.tag == "NONE" then
            chunk = ""
            goto cont1
          else
            _raise(_Match, "text-io.sml:164:70")
          end
        else
          _raise(_Match, "text-io.sml:161:56")
        end
      else
        goto else3
      end
      ::else3::
      if block.tag == "SOME" and (readVecNB.tag == "NONE" and readArrNB.tag == "SOME") then
        local block_PRIME = block.payload
        local raNB = readArrNB.payload
        local arr = tmp18({n, 0})
        local aslice = tmp14(arr)
        local exp = raNB(aslice)
        if exp.tag == "SOME" then
          local tmp21 = tmp16({arr, 0, {tag = "SOME", payload = exp.payload}})
          chunk = tmp17(tmp21)
          goto cont1
        else
          goto else4
        end
        ::else4::
        if exp.tag == "NONE" then
          block_PRIME(nil)
          local exp1 = raNB(aslice)
          if exp1.tag == "SOME" then
            local tmp21 = tmp16({arr, 0, {tag = "SOME", payload = exp1.payload}})
            chunk = tmp17(tmp21)
          elseif exp1.tag == "NONE" then
            chunk = ""
            goto cont1
          else
            _raise(_Match, "text-io.sml:175:72")
          end
        else
          _raise(_Match, "text-io.sml:172:58")
        end
      else
        _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "<unknown>", name = name}}, _exn_meta), "text-io.sml:180:60")
      end
    else
      _raise(_Match, "text-io.sml:154:39")
    end
  else
    _raise(_Match, "text-io.sml:152:27")
  end
  ::cont1::
  f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = initialPosition, next = {{tag = "PRIM_READER", payload = rd}}, position = 0}}
  return nil
end
_L[91] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    local tmp22 = _L[89](exp.payload, f1, 1)
    if tmp22 then
      tmp21 = f1
      goto cont
    else
      return {"", f1}
    end
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
    local rd = exp.payload
    _L[90](rd, f1, exp.payload.chunkSize)
    tmp21 = f1
    goto cont
  else
    goto else2
  end
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local next = exp.payload.next
    local tmp22 = extract(buffer, position, {tag = "NONE"})
    return {tmp22, next}
  elseif exp.tag == "CLOSED" then
    return {"", f1}
  else
    _raise(_Match, "text-io.sml:183:32")
  end
end
_L[92] = function(buffer, position, next, initialPosition)
  if position >= #buffer then
    return next
  else
    return {{tag = "BUFFERED", payload = {buffer = buffer, initialPosition = initialPosition, next = next, position = position}}}
  end
end
_L[93] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    local tmp22 = _L[89](exp.payload, f1, 1)
    if tmp22 then
      tmp21 = f1
      goto cont
    else
      return {tag = "NONE"}
    end
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
    local rd = exp.payload
    _L[90](rd, f1, exp.payload.chunkSize)
    tmp21 = f1
    goto cont
  else
    goto else2
  end
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local next = exp.payload.next
    local initialPosition = exp.payload.initialPosition
    local tmp22 = sub({buffer, position})
    local tmp23 = _L[92](buffer, _Int_add(position, 1), next, initialPosition)
    return {tag = "SOME", payload = {tmp22, tmp23}}
  elseif exp.tag == "CLOSED" then
    return {tag = "NONE"}
  else
    _raise(_Match, "text-io.sml:196:33")
  end
end
_L[94] = function(a)
  local tmp21, tmp22
  do
    local f = a[1]
    tmp21 = f
    tmp22 = a[2]
  end
  ::cont::
  local f, n = tmp21, tmp22
  local exp = f[1]
  if exp.tag == "READABLE" then
    local tmp23 = _L[89](exp.payload, f, n)
    if tmp23 then
      tmp21 = f
      tmp22 = n
      goto cont
    else
      return {"", f}
    end
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
  else
    goto else2
  end
  _L[90](exp.payload, f, n)
  tmp21 = f
  tmp22 = n
  goto cont
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer, position, next
    do
      buffer = exp.payload.buffer
      position = exp.payload.position
      next = exp.payload.next
      local initialPosition = exp.payload.initialPosition
      local newPosition = _Int_add(position, n)
      if newPosition <= #buffer then
        local tmp23 = substring(buffer, position, n)
        local tmp24 = _L[92](buffer, newPosition, next, initialPosition)
        return {tmp23, tmp24}
      else
        goto else3
      end
    end
    ::else3::
    local buffer0 = extract(buffer, position, {tag = "NONE"})
    local exp1 = _L[94]({next, _Int_sub(n, #buffer0)})
    local buffer1 = exp1[1]
    local next1 = exp1[2]
    return {buffer0 .. buffer1, next1}
  elseif exp.tag == "CLOSED" then
    return {"", f}
  else
    _raise(_Match, "text-io.sml:204:36")
  end
end
_L[95] = function(f)
  local tmp21 = f
  ::cont::
  local exp
  do
    local f1 = tmp21
    exp = f1[1]
    if exp.tag == "READABLE" then
      local s = exp.payload
      local exp1
      do
        local result = s.readable:read("L")
        if not result then
          exp1 = {tag = "NONE"}
        else
          exp1 = {tag = "SOME", payload = result}
        end
      end
      if exp1.tag == "SOME" then
        local chunk = exp1.payload
        local tmp22 = {{tag = "READABLE", payload = s}}
        f1[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = {tag = "NONE"}, next = tmp22, position = 0}}
        tmp21 = f1
        goto cont
      elseif exp1.tag == "NONE" then
        return {tag = "NONE"}
      else
        _raise(_Match, "text-io.sml:141:11")
      end
    elseif exp.tag == "PRIM_READER" then
      local rd = exp.payload
      _L[90](rd, f1, exp.payload.chunkSize)
      tmp21 = f1
      goto cont
    else
      goto else1
    end
  end
  ::else1::
  if exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local next = exp.payload.next
    local initialPosition = exp.payload.initialPosition
    local tmp22 = position
    ::cont1::
    local i = tmp22
    if i >= #buffer then
      local exp1 = _L[95](next)
      if exp1.tag == "NONE" then
        local tmp23 = extract(buffer, position, {tag = "NONE"})
        return {tag = "SOME", payload = {tmp23 .. "\n", next}}
      else
        goto else3
      end
      ::else3::
      if exp1.tag == "SOME" then
        local line = exp1.payload[1]
        local next1 = exp1.payload[2]
        local tmp23 = extract(buffer, position, {tag = "NONE"})
        return {tag = "SOME", payload = {tmp23 .. line, next1}}
      else
        _raise(_Match, "text-io.sml:229:68")
      end
    else
      goto else2
    end
    ::else2::
    local tmp23 = sub({buffer, i})
    if tmp23 == 10 then
      local newPosition = _Int_add(i, 1)
      local tmp24 = substring(buffer, position, _Int_sub(newPosition, position))
      local tmp25 = _L[92](buffer, newPosition, next, initialPosition)
      return {tag = "SOME", payload = {tmp24, tmp25}}
    else
      tmp22 = _Int_add(i, 1)
      goto cont1
    end
  elseif exp.tag == "CLOSED" then
    return {tag = "NONE"}
  else
    _raise(_Match, "text-io.sml:221:36")
  end
end
_L[96] = function(f)
  local tmp21, tmp22
  do
    local exp = f[1]
    if exp.tag == "READABLE" then
      local s = exp.payload
      local result = s.readable:read("a")
      local next = {{tag = "READABLE", payload = s}}
      f[1] = {tag = "BUFFERED", payload = {buffer = result, initialPosition = {tag = "NONE"}, next = next, position = 0}}
      return {result, next}
    end
    tmp22, tmp21 = nil, f
  end
  ::cont::
  local contentsRev, f1 = tmp22, tmp21
  local exp = _L[91](f1)
  if exp[1] == "" then
    local f2 = exp[2]
    local tmp23 = revAppend(contentsRev, nil)
    local tmp24 = _VectorOrArray_fromList(tmp23)
    local tmp25 = table_concat(tmp24)
    return {tmp25, f2}
  else
    local content = exp[1]
    local f2 = exp[2]
    tmp22 = {content, contentsRev}
    tmp21 = f2
    goto cont
  end
end
_L[97] = function(a)
  local n, exp
  do
    local f = a[1]
    n = a[2]
    if n < 0 then
      _raise(_Size, "text-io.sml:251:42")
    end
    exp = f[1]
    if exp.tag == "READABLE" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = NonblockingNotSupported, ["function"] = "canInput", name = exp.payload.name}}, _exn_meta), "text-io.sml:254:77")
    elseif exp.tag == "PRIM_READER" and exp.payload.canInput.tag == "SOME" then
      local tmp21 = exp.payload.canInput.payload(nil)
      if tmp21 then
        return {tag = "SOME", payload = 1}
      else
        return {tag = "NONE"}
      end
    else
      goto else1
    end
  end
  ::else1::
  if exp.tag == "PRIM_READER" and exp.payload.canInput.tag == "NONE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = NonblockingNotSupported, ["function"] = "canInput", name = exp.payload.name}}, _exn_meta), "text-io.sml:259:108")
  elseif exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local tmp21 = _Int_sub(#buffer, position)
    if n < tmp21 then
      return {tag = "SOME", payload = n}
    else
      return {tag = "SOME", payload = tmp21}
    end
  elseif exp.tag == "CLOSED" then
    return {tag = "SOME", payload = 0}
  else
    _raise(_Match, "text-io.sml:253:42")
  end
end
_L[98] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    local readable = exp.payload.readable
    local name = exp.payload.name
    readable:close()
    f1[1] = {tag = "CLOSED", payload = name}
    return nil
  elseif exp.tag == "PRIM_READER" then
    local name = exp.payload.name
    exp.payload.close(nil)
    f1[1] = {tag = "CLOSED", payload = name}
    return nil
  elseif exp.tag == "BUFFERED" then
    tmp21 = exp.payload.next
    goto cont
  elseif exp.tag == "CLOSED" then
    return nil
  else
    _raise(_Match, "text-io.sml:262:34")
  end
end
_L[99] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    return not exp.payload.readable:read(0)
  elseif exp.tag == "PRIM_READER" then
    local rd = exp.payload
    _L[90](rd, f1, exp.payload.chunkSize)
    tmp21 = f1
    goto cont
  elseif exp.tag == "BUFFERED" then
    return false
  elseif exp.tag == "CLOSED" then
    return true
  else
    _raise(_Match, "text-io.sml:269:38")
  end
end
_L[100] = function(stream, name, s)
  local tmp21, tmp22 = stream:write(s)
  if not tmp21 then
    _raise(setmetatable({tag = Io_tag, payload = {cause = setmetatable({tag = _Fail_tag, payload = tmp22}, _exn_meta), ["function"] = "output", name = name}}, _exn_meta), "text-io.sml:290:43")
  else
    return nil
  end
end
_L[101] = function(a)
  if a[1].tag == "LUA_WRITABLE" then
    local writable = a[1].payload.writable
    local name = a[1].payload.name
    return _L[100](writable, name, a[2])
  else
    goto else1
  end
  ::else1::
  if a[1].tag == "PRIM_WRITER" then
    local name, chunkSize, writeVec, buffer, content, exp
    do
      name = a[1].payload.writer.name
      chunkSize = a[1].payload.writer.chunkSize
      writeVec = a[1].payload.writer.writeVec
      local buffer_mode = a[1].payload.buffer_mode
      buffer = a[1].payload.buffer
      content = a[2]
      exp = buffer_mode[1]
      if exp == "NO_BUF" then
        if writeVec.tag == "SOME" then
          local writeVec1 = writeVec.payload
          local tmp21 = tmp3(content)
          writeVec1(tmp21)
          return nil
        elseif writeVec.tag == "NONE" then
          _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "output", name = name}}, _exn_meta), "text-io.sml:367:41")
        else
          _raise(_Match, "text-io.sml:365:29")
        end
      else
        goto else2
      end
    end
    ::else2::
    if exp == "LINE_BUF" then
      local tmp21 = tmp9(function(c)
        return c == 10
      end)
      local tmp22 = tmp21(content)
      if tmp22 then
        local tmp23 = revAppend({content, buffer[1]}, nil)
        local tmp24 = _VectorOrArray_fromList(tmp23)
        local content_PRIME = table_concat(tmp24)
        if writeVec.tag == "SOME" then
          local writeVec1 = writeVec.payload
          local tmp25 = tmp3(content_PRIME)
          writeVec1(tmp25)
          buffer[1] = nil
          return nil
        elseif writeVec.tag == "NONE" then
          _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "output", name = name}}, _exn_meta), "text-io.sml:373:49")
        else
          _raise(_Match, "text-io.sml:371:37")
        end
      else
        buffer[1] = {content, buffer[1]}
        return nil
      end
    else
      goto else3
    end
    ::else3::
    if exp == "BLOCK_BUF" then
      local b = buffer[1]
      local tmp21 = foldl(function(a1)
        local z = a1[1]
        return _Int_add(a1[2], #z)
      end)
      local tmp22 = tmp21(#content)
      local bufSize = tmp22(b)
      if bufSize >= chunkSize then
        local tmp23 = revAppend({content, buffer[1]}, nil)
        local tmp24 = _VectorOrArray_fromList(tmp23)
        local content_PRIME = table_concat(tmp24)
        if writeVec.tag == "SOME" then
          local writeVec1 = writeVec.payload
          local tmp25 = tmp3(content_PRIME)
          writeVec1(tmp25)
          buffer[1] = nil
          return nil
        elseif writeVec.tag == "NONE" then
          _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "output", name = name}}, _exn_meta), "text-io.sml:384:53")
        else
          _raise(_Match, "text-io.sml:382:41")
        end
      else
        buffer[1] = {content, buffer[1]}
        return nil
      end
    else
      _raise(_Match, "text-io.sml:364:11")
    end
  else
    _raise(_Match, "text-io.sml:361:9")
  end
end
_L[102] = function(a)
  local stream = a[1]
  return _L[101]({stream, string_char(a[2])})
end
_L[103] = function(name, writeVec, buffer)
  local tmp21 = revAppend(buffer[1], nil)
  local tmp22 = _VectorOrArray_fromList(tmp21)
  local content = table_concat(tmp22)
  buffer[1] = nil
  if content ~= "" then
    if writeVec.tag == "SOME" then
      local writeVec1 = writeVec.payload
      local tmp23 = tmp3(content)
      writeVec1(tmp23)
      return nil
    elseif writeVec.tag == "NONE" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "flushOut", name = name}}, _exn_meta), "text-io.sml:406:30")
    else
      _raise(_Match, "text-io.sml:404:18")
    end
  else
    return nil
  end
end
_L[104] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:flush()
    return nil
  elseif a.tag == "PRIM_WRITER" then
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    return _L[103](name, writeVec, a.payload.buffer)
  else
    _raise(_Match, "text-io.sml:410:9")
  end
end
_L[105] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:close()
    return nil
  elseif a.tag == "PRIM_WRITER" then
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local close = a.payload.writer.close
    _L[103](name, writeVec, a.payload.buffer)
    return close(nil)
  else
    _raise(_Match, "text-io.sml:413:9")
  end
end
_L[106] = function(a)
  if a[1].tag == "LUA_WRITABLE" then
    local writable = a[1].payload.writable
    local buffer_mode = a[1].payload.buffer_mode
    local mode = a[2]
    local modeString
    if mode == "NO_BUF" then
      modeString = "no"
    elseif mode == "BLOCK_BUF" then
      modeString = "full"
    elseif mode == "LINE_BUF" then
      modeString = "line"
    else
      _raise(_Match, "text-io.sml:295:61")
    end
    writable:setvbuf(modeString)
    buffer_mode[1] = mode
    return nil
  elseif a[1].tag == "PRIM_WRITER" then
    local name = a[1].payload.writer.name
    local writeVec = a[1].payload.writer.writeVec
    local buffer_mode = a[1].payload.buffer_mode
    local buffer = a[1].payload.buffer
    local mode = a[2]
    local tmp21 = EQUALbuffer_mode(mode, NO_BUF)
    if tmp21 then
      _L[103](name, writeVec, buffer)
      buffer_mode[1] = mode
      return nil
    else
      buffer_mode[1] = mode
      return nil
    end
  else
    _raise(_Match, "text-io.sml:418:9")
  end
end
_L[107] = function(a)
  if a.tag == "LUA_WRITABLE" then
    return a.payload.buffer_mode[1]
  elseif a.tag == "PRIM_WRITER" then
    return a.payload.buffer_mode[1]
  else
    _raise(_Match, "text-io.sml:429:9")
  end
end
_L[108] = function(a)
  if a[1].ioDesc.tag == "SOME" then
    local name = a[1].name
    local ioDesc = a[1].ioDesc.payload
    local mode = a[2]
    local tmp21 = _L[62][ioDesc]
    return {tag = "LUA_WRITABLE", payload = {buffer_mode = {mode}, name = name, writable = tmp21}}
  else
    local w = a[1]
    local tmp21 = {a[2]}
    return {tag = "PRIM_WRITER", payload = {buffer = {nil}, buffer_mode = tmp21, writer = w}}
  end
end
_L[109] = function(a)
  if a.tag == "LUA_WRITABLE" then
    local writable = a.payload.writable
    local buffer_mode = a.payload.buffer_mode
    local name = a.payload.name
    writable:flush()
    local tmp21
    do
      local ioDesc = _L[63](writable)
      local tmp22 = {tag = "SOME", payload = function(slice1)
        local tmp23 = tmp6(slice1)
        _L[100](writable, name, tmp23)
        writable:flush()
        return tmp4(slice1)
      end}
      local tmp23 = {tag = "NONE"}
      local tmp24 = {tag = "NONE"}
      local tmp25 = {tag = "NONE"}
      local tmp26 = {tag = "NONE"}
      local tmp27 = {tag = "NONE"}
      local tmp28 = {tag = "NONE"}
      local tmp29 = {tag = "NONE"}
      local tmp30 = {tag = "NONE"}
      local tmp31 = {tag = "NONE"}
      local tmp32 = function(a1)
        _L[64](ioDesc)
        writable:close()
        return nil
      end
      tmp21 = _L[84]({block = tmp26, canOutput = tmp27, chunkSize = 1024, close = tmp32, endPos = tmp30, getPos = tmp28, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, setPos = tmp29, verifyPos = tmp31, writeArr = tmp23, writeArrNB = tmp25, writeVec = tmp22, writeVecNB = tmp24})
    end
    return {tmp21, buffer_mode[1]}
  else
    goto else1
  end
  ::else1::
  if a.tag == "PRIM_WRITER" then
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local buffer_mode = a.payload.buffer_mode
    _L[103](name, writeVec, a.payload.buffer)
    return {writer, buffer_mode[1]}
  else
    _raise(_Match, "text-io.sml:436:9")
  end
end
_L[110] = function(a)
  if a.tag == "LUA_WRITABLE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "getPosOut", name = a.payload.name}}, _exn_meta), "text-io.sml:443:50")
  elseif a.tag == "PRIM_WRITER" then
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local getPos = a.payload.writer.getPos
    local buffer_mode = a.payload.buffer_mode
    local buffer = a.payload.buffer
    if getPos.tag == "SOME" then
      local getPos1 = getPos.payload
      _L[103](name, writeVec, buffer)
      local tmp21 = getPos1(nil)
      return {buffer = buffer, buffer_mode = buffer_mode, pos = tmp21, writer = writer}
    elseif getPos.tag == "NONE" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "getPosOut", name = name}}, _exn_meta), "text-io.sml:449:23")
    else
      _raise(_Match, "text-io.sml:445:11")
    end
  else
    _raise(_Match, "text-io.sml:443:9")
  end
end
_L[111] = function(a)
  local writer = a.writer
  local name = a.writer.name
  local setPos = a.writer.setPos
  local buffer_mode = a.buffer_mode
  local buffer = a.buffer
  local pos = a.pos
  if setPos.tag == "SOME" then
    setPos.payload(pos)
    return {tag = "PRIM_WRITER", payload = {buffer = buffer, buffer_mode = buffer_mode, writer = writer}}
  elseif setPos.tag == "NONE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "setPosOut", name = name}}, _exn_meta), "text-io.sml:455:23")
  else
    _raise(_Match, "text-io.sml:451:11")
  end
end
_L[112] = _L[85](_L[97], _L[98], _L[105], _L[99], _L[88], function(a)
  return a.pos
end, _L[104], _L[107], _L[110], _L[87], _L[109], _L[91], _L[93], _L[96], _L[94], _L[86], _L[108], _L[101], _L[102], _L[106], _L[111])
_L[113] = function(stream)
  local tmp21 = _L[112].getInstream(stream)
  local exp = _L[95](tmp21)
  if exp.tag == "NONE" then
    return {tag = "NONE"}
  elseif exp.tag == "SOME" then
    local line = exp.payload[1]
    local stream_PRIME = exp.payload[2]
    local tmp22 = _L[112].setInstream
    tmp22({stream, stream_PRIME})
    return {tag = "SOME", payload = line}
  else
    _raise(_Match, "text-io.sml:472:24")
  end
end
_L[114] = function(path)
  local tmp21, tmp22 = open_PRIME(path, "r")
  if tmp21 == nil then
    _raise(setmetatable({tag = Io_tag, payload = {cause = setmetatable({tag = _Fail_tag, payload = tmp22}, _exn_meta), ["function"] = "TextIO.openIn", name = path}}, _exn_meta), "text-io.sml:479:26")
  else
    local tmp23 = _L[112].mkInstream
    return tmp23({{tag = "READABLE", payload = {name = path, readable = tmp21}}})
  end
end
_L[116] = {tag = "LUA_WRITABLE", payload = {buffer_mode = {LINE_BUF}, name = "<stdout>", writable = stdout}}
_L[115] = _L[112].mkOutstream(_L[116])
_L[118] = {tag = "LUA_WRITABLE", payload = {buffer_mode = {NO_BUF}, name = "<stderr>", writable = stderr}}
_L[117] = _L[112].mkOutstream(_L[118])
_L[119] = _L[112].closeIn
_L[120] = _L[112].closeOut
_L[121] = _L[112].getInstream
_L[122] = _L[112].inputAll
_L[123] = _L[112].output
_L[124] = function(a)
  local rd = a[1]
  local content = a[2]
  local tip
  if rd.ioDesc.tag == "SOME" then
    local name = rd.name
    local ioDesc = rd.ioDesc.payload
    tip = {{tag = "READABLE", payload = {name = name, readable = _L[62][ioDesc]}}}
  else
    tip = {{tag = "PRIM_READER", payload = rd}}
  end
  local tmp21 = _L[32](content)
  if tmp21 > 0 then
    return {{tag = "BUFFERED", payload = {buffer = content, initialPosition = {tag = "NONE"}, next = tip, position = 0}}}
  else
    return tip
  end
end
_L[125] = function(ins)
  local tmp21, tmp22 = ins, nil
  ::cont::
  local ins1, acc = tmp21, tmp22
  local exp = ins1[1]
  if exp.tag == "READABLE" then
    local readable = exp.payload.readable
    local name = exp.payload.name
    ins1[1] = {tag = "CLOSED", payload = name}
    local tmp23
    do
      local ioDesc = _L[63](readable)
      local tmp24 = {tag = "SOME", payload = function(n)
        local exp1
        do
          local result = readable:read(n)
          if not result then
            exp1 = {tag = "NONE"}
          else
            exp1 = {tag = "SOME", payload = result}
          end
        end
        if exp1.tag == "NONE" then
          return _L[31](nil)
        elseif exp1.tag == "SOME" then
          return exp1.payload
        else
          _raise(_Match, "bin-io.sml:28:28")
        end
      end}
      local tmp25 = {tag = "NONE"}
      local tmp26 = {tag = "NONE"}
      local tmp27 = {tag = "NONE"}
      local tmp28 = {tag = "NONE"}
      local tmp29 = {tag = "NONE"}
      local tmp30 = function(a)
        return {tag = "NONE"}
      end
      local tmp31 = {tag = "NONE"}
      local tmp32 = {tag = "NONE"}
      local tmp33 = {tag = "NONE"}
      local tmp34 = {tag = "NONE"}
      local tmp35 = function(a)
        _L[64](ioDesc)
        readable:close()
        return nil
      end
      tmp23 = _L[80]({avail = tmp30, block = tmp28, canInput = tmp29, chunkSize = 1024, close = tmp35, endPos = tmp33, getPos = tmp31, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, readArr = tmp25, readArrNB = tmp27, readVec = tmp24, readVecNB = tmp26, setPos = tmp32, verifyPos = tmp34})
    end
    local tmp24 = revAppend(acc, nil)
    local tmp25 = _L[25](tmp24)
    return {tmp23, tmp25}
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
    local rd = exp.payload
    ins1[1] = {tag = "CLOSED", payload = exp.payload.name}
    local tmp23 = revAppend(acc, nil)
    local tmp24 = _L[25](tmp23)
    return {rd, tmp24}
  else
    goto else2
  end
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local next = exp.payload.next
    local tmp23 = _L[28]({buffer, position, {tag = "NONE"}})
    tmp21 = next
    tmp22 = {tmp23, acc}
    goto cont
  elseif exp.tag == "CLOSED" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "getReader", name = exp.payload}}, _exn_meta), "bin-io.sml:103:57")
  else
    _raise(_Match, "bin-io.sml:99:38")
  end
end
_L[126] = function(ins)
  local exp = ins[1]
  if exp.tag == "READABLE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = exp.payload.name}}, _exn_meta), "bin-io.sml:108:55")
  elseif exp.tag == "PRIM_READER" and exp.payload.getPos.tag == "NONE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = exp.payload.name}}, _exn_meta), "bin-io.sml:109:88")
  elseif exp.tag == "PRIM_READER" and exp.payload.getPos.tag == "SOME" then
    return exp.payload.getPos.payload(nil)
  else
    goto else1
  end
  ::else1::
  if exp.tag == "BUFFERED" and exp.payload.initialPosition.tag == "NONE" then
    local tmp21
    do
      local tmp22 = exp.payload.next
      ::cont::
      local ins1 = tmp22
      local exp1 = ins1[1]
      if exp1.tag == "READABLE" then
        tmp21 = exp1.payload.name
      elseif exp1.tag == "PRIM_READER" then
        tmp21 = exp1.payload.name
      elseif exp1.tag == "BUFFERED" then
        tmp22 = exp1.payload.next
        goto cont
      elseif exp1.tag == "CLOSED" then
        tmp21 = exp1.payload
      else
        _raise(_Match, "bin-io.sml:84:23")
      end
    end
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "filePosIn", name = tmp21}}, _exn_meta), "bin-io.sml:111:92")
  elseif exp.tag == "BUFFERED" and exp.payload.initialPosition.tag == "SOME" then
    local position = exp.payload.position
    return _Int_add(exp.payload.initialPosition.payload, position)
  elseif exp.tag == "CLOSED" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = ClosedStream, ["function"] = "filePosIn", name = exp.payload}}, _exn_meta), "bin-io.sml:113:44")
  else
    _raise(_Match, "bin-io.sml:107:25")
  end
end
_L[127] = function(stream, f, n)
  local exp
  do
    local result = stream.readable:read(n)
    if not result then
      exp = {tag = "NONE"}
    else
      exp = {tag = "SOME", payload = result}
    end
  end
  if exp.tag == "SOME" then
    local chunk = exp.payload
    local tmp21 = {{tag = "READABLE", payload = stream}}
    f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = {tag = "NONE"}, next = tmp21, position = 0}}
    return true
  elseif exp.tag == "NONE" then
    return false
  else
    _raise(_Match, "bin-io.sml:115:11")
  end
end
_L[128] = function(rd, f, n)
  local name = rd.name
  local readVec = rd.readVec
  local readArr = rd.readArr
  local readVecNB = rd.readVecNB
  local readArrNB = rd.readArrNB
  local block = rd.block
  local initialPosition
  if rd.getPos.tag == "SOME" then
    local tmp21 = rd.getPos.payload(nil)
    initialPosition = {tag = "SOME", payload = tmp21}
  else
    initialPosition = {tag = "NONE"}
    goto cont
  end
  ::cont::
  local chunk
  if readVec.tag == "SOME" then
  else
    goto else1
  end
  chunk = readVec.payload(n)
  goto cont1
  ::else1::
  if readVec.tag == "NONE" then
    if readArr.tag == "SOME" then
      local ra = readArr.payload
      local arr = _L[41]({n, 0x0})
      local tmp21 = _L[37](arr)
      local actual = ra(tmp21)
      local tmp22 = _L[39]({arr, 0, {tag = "SOME", payload = actual}})
      chunk = _L[40](tmp22)
      goto cont1
    else
      goto else2
    end
    ::else2::
    if readArr.tag == "NONE" then
      if block.tag == "SOME" and readVecNB.tag == "SOME" then
        local block_PRIME = block.payload
        local rvNB = readVecNB.payload
        local exp = rvNB(n)
        if exp.tag == "SOME" then
          chunk = exp.payload
          goto cont1
        elseif exp.tag == "NONE" then
          block_PRIME(nil)
          local exp1 = rvNB(n)
          if exp1.tag == "SOME" then
            chunk = exp1.payload
            goto cont1
          else
            if exp1.tag == "NONE" then
            else
              _raise(_Match, "bin-io.sml:138:70")
            end
            chunk = _L[31](nil)
            goto cont1
          end
        else
          _raise(_Match, "bin-io.sml:135:56")
        end
      else
        goto else3
      end
      ::else3::
      if block.tag == "SOME" and (readVecNB.tag == "NONE" and readArrNB.tag == "SOME") then
        local block_PRIME = block.payload
        local raNB = readArrNB.payload
        local arr = _L[41]({n, 0x0})
        local aslice = _L[37](arr)
        local exp = raNB(aslice)
        if exp.tag == "SOME" then
          local tmp21 = _L[39]({arr, 0, {tag = "SOME", payload = exp.payload}})
          chunk = _L[40](tmp21)
          goto cont1
        else
          goto else4
        end
        ::else4::
        if exp.tag == "NONE" then
          block_PRIME(nil)
          local exp1 = raNB(aslice)
          if exp1.tag == "SOME" then
            local tmp21 = _L[39]({arr, 0, {tag = "SOME", payload = exp1.payload}})
            chunk = _L[40](tmp21)
          else
            if exp1.tag == "NONE" then
            else
              _raise(_Match, "bin-io.sml:149:72")
            end
            chunk = _L[31](nil)
            goto cont1
          end
        else
          _raise(_Match, "bin-io.sml:146:58")
        end
      else
        _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "<unknown>", name = name}}, _exn_meta), "bin-io.sml:154:60")
      end
    else
      _raise(_Match, "bin-io.sml:128:39")
    end
  else
    _raise(_Match, "bin-io.sml:126:27")
  end
  ::cont1::
  f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = initialPosition, next = {{tag = "PRIM_READER", payload = rd}}, position = 0}}
  return nil
end
_L[129] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    local tmp22 = _L[127](exp.payload, f1, 1)
    if tmp22 then
      tmp21 = f1
      goto cont
    end
    local tmp23 = _L[31](nil)
    return {tmp23, f1}
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
    local rd = exp.payload
    _L[128](rd, f1, exp.payload.chunkSize)
    tmp21 = f1
    goto cont
  else
    goto else2
  end
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local next = exp.payload.next
    local tmp22 = _L[28]({buffer, position, {tag = "NONE"}})
    local tmp23 = _L[29](tmp22)
    return {tmp23, next}
  else
    goto else3
  end
  ::else3::
  if exp.tag == "CLOSED" then
    local tmp22 = _L[31](nil)
    return {tmp22, f1}
  else
    _raise(_Match, "bin-io.sml:157:32")
  end
end
_L[130] = function(buffer, position, next, initialPosition)
  local tmp21 = _L[32](buffer)
  if position >= tmp21 then
    return next
  else
    return {{tag = "BUFFERED", payload = {buffer = buffer, initialPosition = initialPosition, next = next, position = position}}}
  end
end
_L[131] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    local tmp22 = _L[127](exp.payload, f1, 1)
    if tmp22 then
      tmp21 = f1
      goto cont
    else
      return {tag = "NONE"}
    end
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
    local rd = exp.payload
    _L[128](rd, f1, exp.payload.chunkSize)
    tmp21 = f1
    goto cont
  else
    goto else2
  end
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local next = exp.payload.next
    local initialPosition = exp.payload.initialPosition
    local tmp22 = _L[33]({buffer, position})
    local tmp23 = _L[130](buffer, _Int_add(position, 1), next, initialPosition)
    return {tag = "SOME", payload = {tmp22, tmp23}}
  elseif exp.tag == "CLOSED" then
    return {tag = "NONE"}
  else
    _raise(_Match, "bin-io.sml:170:33")
  end
end
_L[132] = function(a)
  local tmp21, tmp22
  do
    local f = a[1]
    tmp21 = f
    tmp22 = a[2]
  end
  ::cont::
  local f, n = tmp21, tmp22
  local exp = f[1]
  if exp.tag == "READABLE" then
    local tmp23 = _L[127](exp.payload, f, n)
    if tmp23 then
      tmp21 = f
      tmp22 = n
      goto cont
    end
    local tmp24 = _L[31](nil)
    return {tmp24, f}
  else
    goto else1
  end
  ::else1::
  if exp.tag == "PRIM_READER" then
  else
    goto else2
  end
  _L[128](exp.payload, f, n)
  tmp21 = f
  tmp22 = n
  goto cont
  ::else2::
  if exp.tag == "BUFFERED" then
    local buffer, position, next
    do
      buffer = exp.payload.buffer
      position = exp.payload.position
      next = exp.payload.next
      local initialPosition = exp.payload.initialPosition
      local newPosition = _Int_add(position, n)
      local tmp23 = _L[32](buffer)
      if newPosition <= tmp23 then
        local tmp24 = _L[28]({buffer, position, {tag = "SOME", payload = n}})
        local tmp25 = _L[29](tmp24)
        local tmp26 = _L[130](buffer, newPosition, next, initialPosition)
        return {tmp25, tmp26}
      else
        goto else4
      end
    end
    ::else4::
    local buffer0 = _L[28]({buffer, position, {tag = "NONE"}})
    local tmp23 = _L[27](buffer0)
    local exp1 = _L[132]({next, _Int_sub(n, tmp23)})
    local buffer1 = exp1[1]
    local next1 = exp1[2]
    local tmp24 = _L[26](buffer1)
    local tmp25 = _L[25]({buffer0, {tmp24, nil}})
    return {tmp25, next1}
  else
    goto else3
  end
  ::else3::
  if exp.tag == "CLOSED" then
    local tmp23 = _L[31](nil)
    return {tmp23, f}
  else
    _raise(_Match, "bin-io.sml:178:36")
  end
end
_L[133] = function(f)
  local tmp21, tmp22
  do
    local exp = f[1]
    if exp.tag == "READABLE" then
      local s = exp.payload
      local result = s.readable:read("a")
      local next = {{tag = "READABLE", payload = s}}
      f[1] = {tag = "BUFFERED", payload = {buffer = result, initialPosition = {tag = "NONE"}, next = next, position = 0}}
      return {result, next}
    end
    tmp22, tmp21 = nil, f
  end
  ::cont::
  local contentsRev, f1 = tmp22, tmp21
  local exp = _L[129](f1)
  local content = exp[1]
  local f2 = exp[2]
  local tmp23 = _L[32](content)
  if tmp23 == 0 then
    local tmp24 = revAppend(contentsRev, nil)
    local tmp25 = _L[30](tmp24)
    return {tmp25, f2}
  else
    tmp22 = {content, contentsRev}
    tmp21 = f2
    goto cont
  end
end
_L[134] = function(a)
  local n, exp
  do
    local f = a[1]
    n = a[2]
    if n < 0 then
      _raise(_Size, "bin-io.sml:208:42")
    end
    exp = f[1]
    if exp.tag == "READABLE" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = NonblockingNotSupported, ["function"] = "canInput", name = exp.payload.name}}, _exn_meta), "bin-io.sml:211:77")
    elseif exp.tag == "PRIM_READER" and exp.payload.canInput.tag == "SOME" then
      local tmp21 = exp.payload.canInput.payload(nil)
      if tmp21 then
        return {tag = "SOME", payload = 1}
      else
        return {tag = "NONE"}
      end
    else
      goto else1
    end
  end
  ::else1::
  if exp.tag == "PRIM_READER" and exp.payload.canInput.tag == "NONE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = NonblockingNotSupported, ["function"] = "canInput", name = exp.payload.name}}, _exn_meta), "bin-io.sml:216:107")
  elseif exp.tag == "BUFFERED" then
    local buffer = exp.payload.buffer
    local position = exp.payload.position
    local tmp21 = _L[32](buffer)
    local tmp22 = _Int_sub(tmp21, position)
    if n < tmp22 then
      return {tag = "SOME", payload = n}
    else
      return {tag = "SOME", payload = tmp22}
    end
  elseif exp.tag == "CLOSED" then
    return {tag = "SOME", payload = 0}
  else
    _raise(_Match, "bin-io.sml:210:42")
  end
end
_L[135] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    local readable = exp.payload.readable
    local name = exp.payload.name
    readable:close()
    f1[1] = {tag = "CLOSED", payload = name}
    return nil
  elseif exp.tag == "PRIM_READER" then
    local name = exp.payload.name
    exp.payload.close(nil)
    f1[1] = {tag = "CLOSED", payload = name}
    return nil
  elseif exp.tag == "BUFFERED" then
    tmp21 = exp.payload.next
    goto cont
  elseif exp.tag == "CLOSED" then
    return nil
  else
    _raise(_Match, "bin-io.sml:219:34")
  end
end
_L[136] = function(f)
  local tmp21 = f
  ::cont::
  local f1 = tmp21
  local exp = f1[1]
  if exp.tag == "READABLE" then
    return not exp.payload.readable:read(0)
  elseif exp.tag == "PRIM_READER" then
    local rd = exp.payload
    _L[128](rd, f1, exp.payload.chunkSize)
    tmp21 = f1
    goto cont
  elseif exp.tag == "BUFFERED" then
    return false
  elseif exp.tag == "CLOSED" then
    return true
  else
    _raise(_Match, "bin-io.sml:226:38")
  end
end
_L[137] = function(stream, name, s)
  local tmp21, tmp22 = stream:write(s)
  if not tmp21 then
    _raise(setmetatable({tag = Io_tag, payload = {cause = setmetatable({tag = _Fail_tag, payload = tmp22}, _exn_meta), ["function"] = "output", name = name}}, _exn_meta), "bin-io.sml:248:18")
  else
    return nil
  end
end
_L[138] = function(a)
  if a[1].tag == "LUA_WRITABLE" then
    local writable = a[1].payload.writable
    local name = a[1].payload.name
    return _L[137](writable, name, a[2])
  else
    goto else1
  end
  ::else1::
  if a[1].tag == "PRIM_WRITER" then
    local name, chunkSize, writeVec, buffer, content
    do
      name = a[1].payload.writer.name
      chunkSize = a[1].payload.writer.chunkSize
      writeVec = a[1].payload.writer.writeVec
      local buffer_mode = a[1].payload.buffer_mode
      buffer = a[1].payload.buffer
      content = a[2]
      if buffer_mode[1] == "NO_BUF" then
        if writeVec.tag == "SOME" then
          local writeVec1 = writeVec.payload
          local tmp21 = _L[26](content)
          writeVec1(tmp21)
          return nil
        elseif writeVec.tag == "NONE" then
          _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "output", name = name}}, _exn_meta), "bin-io.sml:323:41")
        else
          _raise(_Match, "bin-io.sml:321:29")
        end
      else
        goto else2
      end
    end
    ::else2::
    local b = buffer[1]
    local tmp21 = foldl(function(a1)
      local z = a1[1]
      local acc = a1[2]
      local tmp22 = _L[32](z)
      return _Int_add(acc, tmp22)
    end)
    local tmp22 = _L[32](content)
    local tmp23 = tmp21(tmp22)
    local bufSize = tmp23(b)
    if bufSize >= chunkSize then
      local tmp24 = revAppend({content, buffer[1]}, nil)
      local content_PRIME = _L[30](tmp24)
      if writeVec.tag == "SOME" then
        local writeVec1 = writeVec.payload
        local tmp25 = _L[26](content_PRIME)
        writeVec1(tmp25)
        buffer[1] = nil
        return nil
      elseif writeVec.tag == "NONE" then
        _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "output", name = name}}, _exn_meta), "bin-io.sml:332:37")
      else
        _raise(_Match, "bin-io.sml:330:25")
      end
    else
      buffer[1] = {content, buffer[1]}
      return nil
    end
  else
    _raise(_Match, "bin-io.sml:317:9")
  end
end
_L[139] = function(a)
  local stream = a[1]
  local tmp21 = _L[31]({a[2], nil})
  return _L[138]({stream, tmp21})
end
_L[140] = function(name, writeVec, buffer)
  local tmp21 = revAppend(buffer[1], nil)
  local content = _L[30](tmp21)
  buffer[1] = nil
  local tmp22 = _L[32](content)
  if tmp22 > 0 then
    if writeVec.tag == "SOME" then
      local writeVec1 = writeVec.payload
      local tmp23 = _L[26](content)
      writeVec1(tmp23)
      return nil
    elseif writeVec.tag == "NONE" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "flushOut", name = name}}, _exn_meta), "bin-io.sml:353:30")
    else
      _raise(_Match, "bin-io.sml:351:18")
    end
  else
    return nil
  end
end
_L[141] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:flush()
    return nil
  elseif a.tag == "PRIM_WRITER" then
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    return _L[140](name, writeVec, a.payload.buffer)
  else
    _raise(_Match, "bin-io.sml:357:9")
  end
end
_L[142] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:close()
    return nil
  elseif a.tag == "PRIM_WRITER" then
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local close = a.payload.writer.close
    _L[140](name, writeVec, a.payload.buffer)
    return close(nil)
  else
    _raise(_Match, "bin-io.sml:360:9")
  end
end
_L[143] = function(a)
  if a[1].tag == "LUA_WRITABLE" then
    local writable = a[1].payload.writable
    local buffer_mode = a[1].payload.buffer_mode
    local mode = a[2]
    local modeString
    if mode == "NO_BUF" then
      modeString = "no"
    else
      modeString = "full"
    end
    writable:setvbuf(modeString)
    buffer_mode[1] = mode
    return nil
  elseif a[1].tag == "PRIM_WRITER" then
    local name = a[1].payload.writer.name
    local writeVec = a[1].payload.writer.writeVec
    local buffer_mode = a[1].payload.buffer_mode
    local buffer = a[1].payload.buffer
    local mode = a[2]
    local tmp21 = EQUALbuffer_mode(mode, NO_BUF)
    if tmp21 then
      _L[140](name, writeVec, buffer)
      buffer_mode[1] = mode
      return nil
    else
      buffer_mode[1] = mode
      return nil
    end
  else
    _raise(_Match, "bin-io.sml:365:9")
  end
end
_L[144] = function(a)
  if a.tag == "LUA_WRITABLE" then
    return a.payload.buffer_mode[1]
  elseif a.tag == "PRIM_WRITER" then
    return a.payload.buffer_mode[1]
  else
    _raise(_Match, "bin-io.sml:376:9")
  end
end
_L[145] = function(a)
  if a[1].ioDesc.tag == "SOME" then
    local name = a[1].name
    local ioDesc = a[1].ioDesc.payload
    local mode = a[2]
    local tmp21 = _L[62][ioDesc]
    return {tag = "LUA_WRITABLE", payload = {buffer_mode = {mode}, name = name, writable = tmp21}}
  else
    local w = a[1]
    local tmp21 = {a[2]}
    return {tag = "PRIM_WRITER", payload = {buffer = {nil}, buffer_mode = tmp21, writer = w}}
  end
end
_L[146] = function(a)
  if a.tag == "LUA_WRITABLE" then
    local writable = a.payload.writable
    local buffer_mode = a.payload.buffer_mode
    local name = a.payload.name
    writable:flush()
    local tmp21
    do
      local ioDesc = _L[63](writable)
      local tmp22 = {tag = "SOME", payload = function(slice1)
        local tmp23 = _L[29](slice1)
        _L[137](writable, name, tmp23)
        writable:flush()
        return _L[27](slice1)
      end}
      local tmp23 = {tag = "NONE"}
      local tmp24 = {tag = "NONE"}
      local tmp25 = {tag = "NONE"}
      local tmp26 = {tag = "NONE"}
      local tmp27 = {tag = "NONE"}
      local tmp28 = {tag = "NONE"}
      local tmp29 = {tag = "NONE"}
      local tmp30 = {tag = "NONE"}
      local tmp31 = {tag = "NONE"}
      local tmp32 = function(a1)
        _L[64](ioDesc)
        writable:close()
        return nil
      end
      tmp21 = _L[81]({block = tmp26, canOutput = tmp27, chunkSize = 1024, close = tmp32, endPos = tmp30, getPos = tmp28, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, setPos = tmp29, verifyPos = tmp31, writeArr = tmp23, writeArrNB = tmp25, writeVec = tmp22, writeVecNB = tmp24})
    end
    return {tmp21, buffer_mode[1]}
  else
    goto else1
  end
  ::else1::
  if a.tag == "PRIM_WRITER" then
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local buffer_mode = a.payload.buffer_mode
    _L[140](name, writeVec, a.payload.buffer)
    return {writer, buffer_mode[1]}
  else
    _raise(_Match, "bin-io.sml:383:9")
  end
end
_L[147] = function(a)
  if a.tag == "LUA_WRITABLE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "getPosOut", name = a.payload.name}}, _exn_meta), "bin-io.sml:390:50")
  elseif a.tag == "PRIM_WRITER" then
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local getPos = a.payload.writer.getPos
    local buffer_mode = a.payload.buffer_mode
    local buffer = a.payload.buffer
    if getPos.tag == "SOME" then
      local getPos1 = getPos.payload
      _L[140](name, writeVec, buffer)
      local tmp21 = getPos1(nil)
      return {buffer = buffer, buffer_mode = buffer_mode, pos = tmp21, writer = writer}
    elseif getPos.tag == "NONE" then
      _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "getPosOut", name = name}}, _exn_meta), "bin-io.sml:396:23")
    else
      _raise(_Match, "bin-io.sml:392:11")
    end
  else
    _raise(_Match, "bin-io.sml:390:9")
  end
end
_L[148] = function(a)
  local writer = a.writer
  local name = a.writer.name
  local setPos = a.writer.setPos
  local buffer_mode = a.buffer_mode
  local buffer = a.buffer
  local pos = a.pos
  if setPos.tag == "SOME" then
    setPos.payload(pos)
    return {tag = "PRIM_WRITER", payload = {buffer = buffer, buffer_mode = buffer_mode, writer = writer}}
  elseif setPos.tag == "NONE" then
    _raise(setmetatable({tag = Io_tag, payload = {cause = RandomAccessNotSupported, ["function"] = "setPosOut", name = name}}, _exn_meta), "bin-io.sml:402:23")
  else
    _raise(_Match, "bin-io.sml:398:11")
  end
end
_L[149] = _L[85](_L[134], _L[135], _L[142], _L[136], _L[126], function(a)
  return a.pos
end, _L[141], _L[144], _L[147], _L[125], _L[146], _L[129], _L[131], _L[133], _L[132], _L[124], _L[145], _L[138], _L[139], _L[143], _L[148])
_L[150] = function(path)
  local tmp21, tmp22 = open_PRIME(path, "rb")
  if tmp21 == nil then
    _raise(setmetatable({tag = Io_tag, payload = {cause = setmetatable({tag = _Fail_tag, payload = tmp22}, _exn_meta), ["function"] = "BinIO.openIn", name = path}}, _exn_meta), "bin-io.sml:421:26")
  else
    local tmp23 = _L[149].mkInstream
    return tmp23({{tag = "READABLE", payload = {name = path, readable = tmp21}}})
  end
end
_L[151] = _L[149].closeIn
_L[152] = _L[149].inputAll
_L[153] = _ENV.arg
_L[154] = function(i)
  local status, exn = pcall(function()
    local i_PRIME = _Int_add(i, i)
    if i_PRIME < math_maxinteger then
      return _L[154](i_PRIME)
    else
      return i
    end
  end)
  if not status then
    if __exn_instanceof(exn, _Overflow_tag) then
      return i
    else
      _raise(exn, nil)
    end
  else
    return exn
  end
end
_L[154](65536)
_L[155] = _L[41]({256, 0xFF})
_L[47]({_L[155], 9, 0x41})
_L[47]({_L[155], 10, 0x41})
_L[47]({_L[155], 13, 0x41})
_L[47]({_L[155], 32, 0x41})
_L[156] = tmp8(function(a)
  local i = a[1]
  local c = a[2]
  return _L[47]({_L[155], c, i & 0xFF})
end)
_L[156]("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")
_L[34]({256, function(i)
  return _L[45]({_L[155], i})
end})
_L[157] = _L[41]({256, 0xFF})
_L[158] = tmp8(function(a)
  local i = a[1]
  local c = a[2]
  return _L[47]({_L[157], c, i & 0xFF})
end)
_L[158]("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")
_L[34]({256, function(i)
  return _L[45]({_L[157], i})
end})
_L[31](_list({n = 9, 0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF}))
_L[31](_list({n = 9, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x0}))
_L[159], _L[160] = pcall(function()
  return _Int_mul(8, _L[44])
end)
if not _L[159] then
  if __exn_instanceof(_L[160], _Overflow_tag) then
    goto cont1
  else
    _raise(_L[160], nil)
  end
end
::cont1::
fmt(OCT)
fmt(DEC)
fmt(HEX)
_L[161] = fromInt(2147483646)
if _L[161].tag == "ZERO" then
elseif _L[161].tag == "POSITIVE" then
elseif _L[161].tag == "NEGATIVE" then
else
  _raise(_Match, "int-inf.sml:130:5")
end
_L[162] = fromInt(2147483647)
if _L[162].tag == "ZERO" then
  goto cont3
elseif _L[162].tag == "POSITIVE" then
else
  goto else1
end
toRealAbs(_L[162].payload)
goto cont2
::else1::
if _L[162].tag == "NEGATIVE" then
else
  _raise(_Match, "int-inf.sml:1082:5")
end
toRealAbs(_L[162].payload)
goto cont3
::cont2::
::cont3::
_L[163] = 1.0
_id(32768 * _L[163])
_L[164] = 1.0
_id(32768 * _L[164])
do
  _L[165] = 123 & 0x7FFFFFFF
  if not math_ult(0x1, wordSize) then
    _L[166] = 0x0
  else
    _L[166] = _L[165] << 0x1
  end
  _L[167], _L[168], _L[169], _L[170] = 48, nil, _L[166] + 0x1, 73256
  ::cont5::
  local _L1 = {}
  _L1[1], _L1[2], _L1[3], _L1[4] = _L[167], _L[168], _L[169], _L[170]
  if _L1[1] == 0 then
    goto cont4
  end
  _L1[5], _L1[6], _L1[7], _L1[8] = 31, 0x0, _L1[3], _L1[4]
  ::cont6::
  local _L2 = {}
  _L2[1], _L2[2], _L2[3], _L2[4] = _L1[5], _L1[6], _L1[7], _L1[8]
  if _L2[1] == 0 then
    _L2[5] = _Int_sub(_L1[1], 1)
    _L[167] = _L2[5]
    _L[168] = {_L2[2], _L1[2]}
    _L[169] = _L2[3]
    _L[170] = _L2[4]
    goto cont5
  end
  _L2[6] = _Int_sub(_L2[1], 1)
  _L2[8] = 0xBC8F * _Word_mod(_L2[3], 0xADC8)
  _L2[9] = 0xD47 * ((_L2[3] >> 3) // 5561)
  if math_ult(_L2[9], _L2[8]) then
    _L2[7] = _L2[8] - _L2[9]
  else
    _L2[7] = 0x7FFFFFFF - _L2[9] + _L2[8]
  end
  if not math_ult(0x12, wordSize) then
    _L2[10] = 0x0
  else
    _L2[10] = _L2[4] << 0x12
  end
  _L2[11] = _L2[4] ~ _L2[10]
  if not math_ult(0xD, wordSize) then
    _L2[12] = 0x0
  else
    _L2[12] = _L2[11] >> 0xD
  end
  _L2[13] = _L2[11] ~ _L2[12]
  if not math_ult(0x1, wordSize) then
    _L2[14] = 0x0
  else
    _L2[14] = _L2[2] >> 0x1
  end
  _L2[15] = 0x3FFFFFFF & _L2[14]
  _L1[5] = _L2[6]
  _L1[6] = _L2[15] | 0x40000000 & (_L2[7] ~ _L2[13])
  _L1[7] = _L2[7]
  _L1[8] = _L2[13]
  goto cont6
end
::cont4::
_L[171] = {"InvalidUtf8"}
_L[172] = setmetatable({tag = _L[171]}, _exn_meta)
_L[173] = "START"
_L[174] = "MID_1_OF_3_E0"
_L[175] = "MID_1_OF_3_ED"
_L[176] = "MID_1_OF_4_F0"
_L[177] = "MID_1_OF_4_F4"
_L[178] = "TAIL_1"
_L[179] = "TAIL_2"
_L[180] = "TAIL_3"
_L[181] = {tag = "PREFIX_ZERO"}
_L[182] = {tag = "INVALID_UNICODE_SCALAR"}
_L[183] = {tag = "INVALID_DATE"}
_L[184] = {tag = "INVALID_TIME"}
_L[185] = function(s)
  if s == "" then
    return "\"\""
  end
  local tmp21 = tmp7(function(c)
    local tmp22
    do
      local tmp23
      if 65 <= c and c <= 90 then
        tmp22 = true
        goto cont
      else
        tmp23 = 97 <= c and c <= 122
      end
      tmp22 = tmp23 or 48 <= c and c <= 57
    end
    ::cont::
    return tmp22 or (c == 45 or c == 95)
  end)
  local tmp22 = tmp21(s)
  if tmp22 then
    return s
  else
    return "\"" .. s .. "\""
  end
end
_L[186] = {"ParseError"}
_L[187] = function(i, accum)
  if math_ult(i, 0x80) then
    if i < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    elseif i < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif i > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      return {i, accum}
    end
  elseif math_ult(i, 0x800) then
    local tmp21
    if not math_ult(0x6, wordSize) then
      tmp21 = 0x0
    else
      tmp21 = i >> 0x6
    end
    local u0 = 0xC0 | tmp21
    local u1 = 0x80 | i & 0x3F
    local tmp22
    if u1 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp22 = u1
    end
    local tmp23
    if tmp22 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp22 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      tmp23 = tmp22
    end
    local tmp24
    if u0 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp24 = u0
    end
    if tmp24 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp24 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      return {tmp23, {tmp24, accum}}
    end
  elseif math_ult(i, 0x10000) then
    if not math_ult(i, 0xD800) and math_ult(i, 0xE000) then
      _raise(setmetatable({tag = _L[186], payload = _L[182]}, _exn_meta), "parse_toml_util.sml:33:9")
    end
    local tmp21
    if not math_ult(0xC, wordSize) then
      tmp21 = 0x0
    else
      tmp21 = i >> 0xC
    end
    local u0 = 0xE0 | tmp21
    local tmp22
    if not math_ult(0x6, wordSize) then
      tmp22 = 0x0
    else
      tmp22 = i >> 0x6
    end
    local u1 = 0x80 | tmp22 & 0x3F
    local u2 = 0x80 | i & 0x3F
    local tmp23
    if u2 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp23 = u2
    end
    local tmp24
    if tmp23 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp23 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      tmp24 = tmp23
    end
    local tmp25
    if u1 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp25 = u1
    end
    local tmp26
    if tmp25 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp25 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      tmp26 = tmp25
    end
    local tmp27
    if u0 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp27 = u0
    end
    if tmp27 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp27 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      return {tmp24, {tmp26, {tmp27, accum}}}
    end
  elseif math_ult(i, 0x110000) then
    local tmp21, tmp22, tmp23, tmp24
    do
      local tmp25
      if not math_ult(0x12, wordSize) then
        tmp25 = 0x0
      else
        tmp25 = i >> 0x12
      end
      local u0 = 0xF0 | tmp25
      local tmp26
      if not math_ult(0xC, wordSize) then
        tmp26 = 0x0
      else
        tmp26 = i >> 0xC
      end
      local u1 = 0x80 | tmp26 & 0x3F
      local tmp27
      if not math_ult(0x6, wordSize) then
        tmp27 = 0x0
      else
        tmp27 = i >> 0x6
      end
      local u2 = 0x80 | tmp27 & 0x3F
      local u3 = 0x80 | i & 0x3F
      local tmp28
      if u3 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp28 = u3
      end
      if tmp28 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp28 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        tmp21 = tmp28
      end
      local tmp29
      if u2 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp29 = u2
      end
      if tmp29 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp29 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        tmp22 = tmp29
      end
      local tmp30
      if u1 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp30 = u1
      end
      if tmp30 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp30 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        tmp23 = tmp30
      end
      if u0 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp24 = u0
      end
    end
    if tmp24 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp24 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      return {tmp21, {tmp22, {tmp23, {tmp24, accum}}}}
    end
  else
    _raise(setmetatable({tag = _L[186], payload = _L[182]}, _exn_meta), "parse_toml_util.sml:54:7")
  end
end
_L[188] = function(s)
  local tmp21
  do
    local tmp22 = scanString(function(getc)
      return function(strm)
        local isNegative, signPart, exp
        do
          local strm1 = skipInitialWhitespace(getc, strm)
          local exp1 = scanSign(getc, strm1)
          isNegative = exp1[1]
          local strm2 = exp1[2]
          if isNegative then
            signPart = {45, nil}
          else
            signPart = nil
          end
          exp = getc(strm2)
          if exp.tag == "SOME" and exp.payload[1] == 46 then
            local exp2 = scanOneOrMoreDigits(getc, exp.payload[2], {46, {48, signPart}})
            if exp2.tag == "SOME" then
              local exp3 = scanOptExpPart(getc, exp2.payload[2], exp2.payload[1])
              local revAcc = exp3[1]
              local strm_PRIME_PRIME_PRIME = exp3[2]
              local tmp23 = implodeRev(revAcc)
              local result = tonumber(tmp23)
              local tmp24 = 1.0
              local tmp25 = result * tmp24
              return {tag = "SOME", payload = {tmp25, strm_PRIME_PRIME_PRIME}}
            elseif exp2.tag == "NONE" then
              return {tag = "NONE"}
            else
              _raise(_Match, "scan-num.sml:385:52")
            end
          else
            goto else1
          end
        end
        ::else1::
        if exp.tag == "SOME" then
          local c = exp.payload[1]
          local strm_PRIME = exp.payload[2]
          if c == 105 or c == 73 then
            local tmp23 = full("nf")
            local exp1 = scanSubstring(getc, strm_PRIME, tmp23)
            if exp1.tag == "SOME" then
              local strm_PRIME_PRIME = exp1.payload
              local inf
              if isNegative then
                inf = negInf
              else
                inf = huge
              end
              local tmp24 = full("inity")
              local exp2 = scanSubstring(getc, strm_PRIME_PRIME, tmp24)
              if exp2.tag == "SOME" then
                return {tag = "SOME", payload = {inf, exp2.payload}}
              elseif exp2.tag == "NONE" then
                return {tag = "SOME", payload = {inf, strm_PRIME_PRIME}}
              else
                _raise(_Match, "scan-num.sml:395:74")
              end
            elseif exp1.tag == "NONE" then
              return {tag = "NONE"}
            else
              _raise(_Match, "scan-num.sml:393:52")
            end
          else
            goto else2
          end
          ::else2::
          if c == 110 or c == 78 then
            local tmp23 = full("an")
            local exp1 = scanSubstring(getc, strm_PRIME, tmp23)
            if exp1.tag == "SOME" then
              return {tag = "SOME", payload = {0.0 / 0.0, exp1.payload}}
            elseif exp1.tag == "NONE" then
              return {tag = "NONE"}
            else
              _raise(_Match, "scan-num.sml:402:52")
            end
          else
            goto else3
          end
          ::else3::
          if 48 <= c and c <= 57 then
            local strm_PRIME_PRIME_PRIME, revAcc
            do
              local exp1 = scanZeroOrMoreDigits(getc, strm_PRIME, {c, signPart})
              local revAcc1 = exp1[1]
              local strm_PRIME_PRIME = exp1[2]
              local exp2 = getc(strm_PRIME_PRIME)
              if exp2.tag == "SOME" and exp2.payload[1] == 46 then
                local exp3 = scanOneOrMoreDigits(getc, exp2.payload[2], {46, revAcc1})
                if exp3.tag == "SOME" then
                  strm_PRIME_PRIME_PRIME = exp3.payload[2]
                  revAcc = exp3.payload[1]
                elseif exp3.tag == "NONE" then
                  strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME
                  revAcc = revAcc1
                else
                  _raise(_Match, "scan-num.sml:362:71")
                end
              else
                strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME
                revAcc = revAcc1
                goto cont
              end
            end
            ::cont::
            local exp1 = scanOptExpPart(getc, strm_PRIME_PRIME_PRIME, revAcc)
            local revAcc1 = exp1[1]
            local strm_PRIME_PRIME_PRIME_PRIME = exp1[2]
            local tmp23 = implodeRev(revAcc1)
            local result = tonumber(tmp23)
            local tmp24 = 1.0
            local tmp25 = result * tmp24
            return {tag = "SOME", payload = {tmp25, strm_PRIME_PRIME_PRIME_PRIME}}
          else
            return {tag = "NONE"}
          end
        elseif exp.tag == "NONE" then
          return {tag = "NONE"}
        else
          _raise(_Match, "scan-num.sml:384:25")
        end
      end
    end)
    tmp21 = tmp22(s)
  end
  if tmp21.tag == "SOME" then
    return {tag = "FLOAT", payload = tmp21.payload}
  elseif tmp21.tag == "NONE" then
    _raise(Option, "option.sml:24:18")
  else
    _raise(_Match, "option.sml:23:5")
  end
end
_L[189] = function(x, y)
  local tmp21
  if not math_ult(y, wordSize) then
    tmp21 = 0x0
  else
    tmp21 = x << y
  end
  local tmp22 = tmp21 & 0xFFFFFFFF
  local tmp23 = 0x20 - y
  if not math_ult(tmp23, wordSize) then
    return tmp22
  else
    return tmp22 | x >> tmp23
  end
end
_L[190] = function(x)
  local tmp21
  if not math_ult(0x4, wordSize) then
    tmp21 = 0x0
  else
    tmp21 = x >> 0x4
  end
  local tmp22
  if tmp21 < 0x0 then
    _raise(_Overflow, "word-1.sml:52:39")
  else
    tmp22 = sub({"0123456789abcdef", tmp21})
  end
  local tmp23 = x & 0xF
  local tmp24
  if tmp23 < 0x0 then
    _raise(_Overflow, "word-1.sml:52:39")
  else
    tmp24 = sub({"0123456789abcdef", tmp23})
  end
  local tmp25 = {n = 2, tmp22, tmp24}
  local tmp26
  do
    local tmp27 = tmp25.n
    tmp26 = _VectorOrArray_tabulate(tmp27, function(i)
      return string_char(tmp25[i + 1])
    end)
  end
  local result = table_concat(tmp26)
  return result
end
_L[191] = function(content)
  local content8
  do
    local origLen = _L[32](content)
    local tmp21 = _L[32](content)
    local r = tmp21 % 64
    local tmp22 = _L[31]({0x80, nil})
    local tmp23
    if r < 56 then
      tmp23 = _Int_sub(63, r)
    else
      tmp23 = _Int_sub(127, r)
    end
    local tmp24 = _L[34]({tmp23, function(a)
      return 0x0
    end})
    local padded = _L[30]({content, {tmp22, {tmp24, nil}}})
    local paddedLen = _L[32](padded)
    content8 = _L[46]({paddedLen, function(i)
      return _L[33]({padded, i})
    end})
    local tmp25 = _Int_sub(paddedLen // 8, 1)
    local tmp26 = _Int_mul(8, origLen)
    local ii
    do
      ii = _L[50](content8, tmp25, 8)
      _L[49]({content8, ii, tmp26 & 0xFF})
      local tmp27 = _Int_add(ii, 1)
      local tmp28
      if not math_ult(0x8, wordSize) then
        tmp28 = 0x0
      else
        tmp28 = tmp26 >> 0x8
      end
      _L[49]({content8, tmp27, tmp28 & 0xFF})
      local tmp29 = _Int_add(ii, 2)
      local tmp30
      if not math_ult(0x10, wordSize) then
        tmp30 = 0x0
      else
        tmp30 = tmp26 >> 0x10
      end
      _L[49]({content8, tmp29, tmp30 & 0xFF})
      local tmp31 = _Int_add(ii, 3)
      local tmp32
      if not math_ult(0x18, wordSize) then
        tmp32 = 0x0
      else
        tmp32 = tmp26 >> 0x18
      end
      _L[49]({content8, tmp31, tmp32 & 0xFF})
      local tmp33 = _Int_add(ii, 4)
      local tmp34
      if not math_ult(0x20, wordSize) then
        tmp34 = 0x0
      else
        tmp34 = tmp26 >> 0x20
      end
      _L[49]({content8, tmp33, tmp34 & 0xFF})
      local tmp35 = _Int_add(ii, 5)
      local tmp36
      if not math_ult(0x28, wordSize) then
        tmp36 = 0x0
      else
        tmp36 = tmp26 >> 0x28
      end
      _L[49]({content8, tmp35, tmp36 & 0xFF})
    end
    local tmp27 = _Int_add(ii, 6)
    local tmp28
    if not math_ult(0x30, wordSize) then
      tmp28 = 0x0
    else
      tmp28 = tmp26 >> 0x30
    end
    _L[49]({content8, tmp27, tmp28 & 0xFF})
    local tmp29 = _Int_add(ii, 7)
    local tmp30
    if not math_ult(0x38, wordSize) then
      tmp30 = 0x0
    else
      tmp30 = tmp26 >> 0x38
    end
    _L[49]({content8, tmp29, tmp30 & 0xFF})
  end
  local content32, max, tmp21, tmp22, tmp23, tmp24, tmp25
  do
    local tmp26 = _L[43](content8)
    local tmp27 = tmp26 // 4
    content32 = _VectorOrArray_tabulate(tmp27, function(i)
      local ii = _L[50](content8, i, 4)
      local x0 = _L[48]({content8, ii})
      local x1 = _L[48]({content8, _Int_add(ii, 1)})
      local x2 = _L[48]({content8, _Int_add(ii, 2)})
      local x3 = _L[48]({content8, _Int_add(ii, 3)})
      local tmp28
      if not math_ult(0x18, wordSize) then
        tmp28 = 0x0
      else
        tmp28 = x3 << 0x18
      end
      local tmp29
      if not math_ult(0x10, wordSize) then
        tmp29 = 0x0
      else
        tmp29 = x2 << 0x10
      end
      local tmp30 = tmp28 | tmp29
      local tmp31
      if not math_ult(0x8, wordSize) then
        tmp31 = 0x0
      else
        tmp31 = x1 << 0x8
      end
      return (tmp30 | tmp31 | x0) & 0xFFFFFFFF
    end)
    max = content32.n // 16
    tmp25, tmp24, tmp23, tmp22, tmp21 = 0, 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476
  end
  ::cont::
  local d, c, b, b_PRIME, c_PRIME, d_PRIME, tmp26, tmp27
  do
    local i, a
    i, a, b, c, d = tmp25, tmp24, tmp23, tmp22, tmp21
    if i >= max then
      return {a, b, c, d}
    end
    local tmp28 = _Int_mul(i, 16)
    local X = slice(content32, tmp28, {tag = "SOME", payload = 16})
    local tmp29 = {a, b, c, d}
    local tmp30
    do
      local ABCD, DABC, CDAB, BCDA, tmp31
      do
        ABCD = function(k, t)
          return function(a1)
            local a2 = a1[1]
            local b1 = a1[2]
            local c1 = a1[3]
            local d1 = a1[4]
            local tmp32 = a2 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
            local tmp33
            do
              local base = X.base
              tmp33 = sub1(base, X.length, X.start, k)
            end
            local tmp34 = _L[189]((tmp32 + tmp33 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x7)
            return {b1 + tmp34 & 0xFFFFFFFF, b1, c1, d1}
          end
        end
        DABC = function(k, t)
          return function(a1)
            local b1 = a1[1]
            local c1 = a1[2]
            local d1 = a1[3]
            local a2 = a1[4]
            local tmp32 = a2 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
            local tmp33
            do
              local base = X.base
              tmp33 = sub1(base, X.length, X.start, k)
            end
            local tmp34 = _L[189]((tmp32 + tmp33 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xC)
            return {b1, c1, d1, b1 + tmp34 & 0xFFFFFFFF}
          end
        end
        CDAB = function(k, t)
          return function(a1)
            local c1 = a1[1]
            local d1 = a1[2]
            local a2 = a1[3]
            local b1 = a1[4]
            local tmp32 = a2 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
            local tmp33
            do
              local base = X.base
              tmp33 = sub1(base, X.length, X.start, k)
            end
            local tmp34 = _L[189]((tmp32 + tmp33 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x11)
            return {c1, d1, b1 + tmp34 & 0xFFFFFFFF, b1}
          end
        end
        BCDA = function(k, t)
          return function(a1)
            local d1 = a1[1]
            local a2 = a1[2]
            local b1 = a1[3]
            local c1 = a1[4]
            local tmp32 = a2 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
            local tmp33
            do
              local base = X.base
              tmp33 = sub1(base, X.length, X.start, k)
            end
            local tmp34 = _L[189]((tmp32 + tmp33 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x16)
            return {d1, b1 + tmp34 & 0xFFFFFFFF, b1, c1}
          end
        end
        local tmp32 = ABCD(0, 0xD76AA478)
        local tmp33 = tmp32(tmp29)
        local tmp34 = DABC(1, 0xE8C7B756)
        local tmp35 = tmp34(tmp33)
        local tmp36 = CDAB(2, 0x242070DB)
        local tmp37 = tmp36(tmp35)
        local tmp38 = BCDA(3, 0xC1BDCEEE)
        local tmp39 = tmp38(tmp37)
        local tmp40 = ABCD(4, 0xF57C0FAF)
        local tmp41 = tmp40(tmp39)
        local tmp42 = DABC(5, 0x4787C62A)
        tmp31 = tmp42(tmp41)
      end
      local tmp32
      do
        local tmp33 = CDAB(6, 0xA8304613)
        local tmp34 = tmp33(tmp31)
        local tmp35 = BCDA(7, 0xFD469501)
        local tmp36 = tmp35(tmp34)
        local tmp37 = ABCD(8, 0x698098D8)
        local tmp38 = tmp37(tmp36)
        local tmp39 = DABC(9, 0x8B44F7AF)
        local tmp40 = tmp39(tmp38)
        local tmp41 = CDAB(10, 0xFFFF5BB1)
        local tmp42 = tmp41(tmp40)
        local tmp43 = BCDA(11, 0x895CD7BE)
        tmp32 = tmp43(tmp42)
      end
      local tmp33 = ABCD(12, 0x6B901122)
      local tmp34 = tmp33(tmp32)
      local tmp35 = DABC(13, 0xFD987193)
      local tmp36 = tmp35(tmp34)
      local tmp37 = CDAB(14, 0xA679438E)
      local tmp38 = tmp37(tmp36)
      local tmp39 = BCDA(15, 0x49B40821)
      tmp30 = tmp39(tmp38)
    end
    local tmp31
    do
      local ABCD, DABC, CDAB, BCDA, tmp32
      do
        ABCD = function(k, t)
          return function(a1)
            local a2 = a1[1]
            local b1 = a1[2]
            local c1 = a1[3]
            local d1 = a1[4]
            local tmp33 = a2 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp34
            do
              local base = X.base
              tmp34 = sub1(base, X.length, X.start, k)
            end
            local tmp35 = _L[189]((tmp33 + tmp34 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x5)
            return {b1 + tmp35 & 0xFFFFFFFF, b1, c1, d1}
          end
        end
        DABC = function(k, t)
          return function(a1)
            local b1 = a1[1]
            local c1 = a1[2]
            local d1 = a1[3]
            local a2 = a1[4]
            local tmp33 = a2 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp34
            do
              local base = X.base
              tmp34 = sub1(base, X.length, X.start, k)
            end
            local tmp35 = _L[189]((tmp33 + tmp34 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x9)
            return {b1, c1, d1, b1 + tmp35 & 0xFFFFFFFF}
          end
        end
        CDAB = function(k, t)
          return function(a1)
            local c1 = a1[1]
            local d1 = a1[2]
            local a2 = a1[3]
            local b1 = a1[4]
            local tmp33 = a2 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp34
            do
              local base = X.base
              tmp34 = sub1(base, X.length, X.start, k)
            end
            local tmp35 = _L[189]((tmp33 + tmp34 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xE)
            return {c1, d1, b1 + tmp35 & 0xFFFFFFFF, b1}
          end
        end
        BCDA = function(k, t)
          return function(a1)
            local d1 = a1[1]
            local a2 = a1[2]
            local b1 = a1[3]
            local c1 = a1[4]
            local tmp33 = a2 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp34
            do
              local base = X.base
              tmp34 = sub1(base, X.length, X.start, k)
            end
            local tmp35 = _L[189]((tmp33 + tmp34 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x14)
            return {d1, b1 + tmp35 & 0xFFFFFFFF, b1, c1}
          end
        end
        local tmp33 = ABCD(1, 0xF61E2562)
        local tmp34 = tmp33(tmp30)
        local tmp35 = DABC(6, 0xC040B340)
        local tmp36 = tmp35(tmp34)
        local tmp37 = CDAB(11, 0x265E5A51)
        local tmp38 = tmp37(tmp36)
        local tmp39 = BCDA(0, 0xE9B6C7AA)
        local tmp40 = tmp39(tmp38)
        local tmp41 = ABCD(5, 0xD62F105D)
        local tmp42 = tmp41(tmp40)
        local tmp43 = DABC(10, 0x2441453)
        tmp32 = tmp43(tmp42)
      end
      local tmp33
      do
        local tmp34 = CDAB(15, 0xD8A1E681)
        local tmp35 = tmp34(tmp32)
        local tmp36 = BCDA(4, 0xE7D3FBC8)
        local tmp37 = tmp36(tmp35)
        local tmp38 = ABCD(9, 0x21E1CDE6)
        local tmp39 = tmp38(tmp37)
        local tmp40 = DABC(14, 0xC33707D6)
        local tmp41 = tmp40(tmp39)
        local tmp42 = CDAB(3, 0xF4D50D87)
        local tmp43 = tmp42(tmp41)
        local tmp44 = BCDA(8, 0x455A14ED)
        tmp33 = tmp44(tmp43)
      end
      local tmp34 = ABCD(13, 0xA9E3E905)
      local tmp35 = tmp34(tmp33)
      local tmp36 = DABC(2, 0xFCEFA3F8)
      local tmp37 = tmp36(tmp35)
      local tmp38 = CDAB(7, 0x676F02D9)
      local tmp39 = tmp38(tmp37)
      local tmp40 = BCDA(12, 0x8D2A4C8A)
      tmp31 = tmp40(tmp39)
    end
    local tmp32
    do
      local ABCD, DABC, CDAB, BCDA, tmp33
      do
        ABCD = function(k, t)
          return function(a1)
            local a2 = a1[1]
            local b1 = a1[2]
            local c1 = a1[3]
            local d1 = a1[4]
            local tmp34 = a2 + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x4)
            return {b1 + tmp36 & 0xFFFFFFFF, b1, c1, d1}
          end
        end
        DABC = function(k, t)
          return function(a1)
            local b1 = a1[1]
            local c1 = a1[2]
            local d1 = a1[3]
            local tmp34 = a1[4] + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xB)
            return {b1, c1, d1, b1 + tmp36 & 0xFFFFFFFF}
          end
        end
        CDAB = function(k, t)
          return function(a1)
            local c1 = a1[1]
            local d1 = a1[2]
            local a2 = a1[3]
            local b1 = a1[4]
            local tmp34 = a2 + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x10)
            return {c1, d1, b1 + tmp36 & 0xFFFFFFFF, b1}
          end
        end
        BCDA = function(k, t)
          return function(a1)
            local d1 = a1[1]
            local a2 = a1[2]
            local b1 = a1[3]
            local c1 = a1[4]
            local tmp34 = a2 + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x17)
            return {d1, b1 + tmp36 & 0xFFFFFFFF, b1, c1}
          end
        end
        local tmp34 = ABCD(5, 0xFFFA3942)
        local tmp35 = tmp34(tmp31)
        local tmp36 = DABC(8, 0x8771F681)
        local tmp37 = tmp36(tmp35)
        local tmp38 = CDAB(11, 0x6D9D6122)
        local tmp39 = tmp38(tmp37)
        local tmp40 = BCDA(14, 0xFDE5380C)
        local tmp41 = tmp40(tmp39)
        local tmp42 = ABCD(1, 0xA4BEEA44)
        local tmp43 = tmp42(tmp41)
        local tmp44 = DABC(4, 0x4BDECFA9)
        tmp33 = tmp44(tmp43)
      end
      local tmp34
      do
        local tmp35 = CDAB(7, 0xF6BB4B60)
        local tmp36 = tmp35(tmp33)
        local tmp37 = BCDA(10, 0xBEBFBC70)
        local tmp38 = tmp37(tmp36)
        local tmp39 = ABCD(13, 0x289B7EC6)
        local tmp40 = tmp39(tmp38)
        local tmp41 = DABC(0, 0xEAA127FA)
        local tmp42 = tmp41(tmp40)
        local tmp43 = CDAB(3, 0xD4EF3085)
        local tmp44 = tmp43(tmp42)
        local tmp45 = BCDA(6, 0x4881D05)
        tmp34 = tmp45(tmp44)
      end
      local tmp35 = ABCD(9, 0xD9D4D039)
      local tmp36 = tmp35(tmp34)
      local tmp37 = DABC(12, 0xE6DB99E5)
      local tmp38 = tmp37(tmp36)
      local tmp39 = CDAB(15, 0x1FA27CF8)
      local tmp40 = tmp39(tmp38)
      local tmp41 = BCDA(2, 0xC4AC5665)
      tmp32 = tmp41(tmp40)
    end
    local exp
    do
      local ABCD, DABC, CDAB, BCDA, tmp33
      do
        ABCD = function(k, t)
          return function(a1)
            local a2 = a1[1]
            local b1 = a1[2]
            local c1 = a1[3]
            local d1 = a1[4]
            local tmp34 = a2 + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x6)
            return {b1 + tmp36 & 0xFFFFFFFF, b1, c1, d1}
          end
        end
        DABC = function(k, t)
          return function(a1)
            local b1 = a1[1]
            local c1 = a1[2]
            local d1 = a1[3]
            local tmp34 = a1[4] + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xA)
            return {b1, c1, d1, b1 + tmp36 & 0xFFFFFFFF}
          end
        end
        CDAB = function(k, t)
          return function(a1)
            local c1 = a1[1]
            local d1 = a1[2]
            local a2 = a1[3]
            local b1 = a1[4]
            local tmp34 = a2 + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xF)
            return {c1, d1, b1 + tmp36 & 0xFFFFFFFF, b1}
          end
        end
        BCDA = function(k, t)
          return function(a1)
            local d1 = a1[1]
            local a2 = a1[2]
            local b1 = a1[3]
            local c1 = a1[4]
            local tmp34 = a2 + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
            local tmp35
            do
              local base = X.base
              tmp35 = sub1(base, X.length, X.start, k)
            end
            local tmp36 = _L[189]((tmp34 + tmp35 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x15)
            return {d1, b1 + tmp36 & 0xFFFFFFFF, b1, c1}
          end
        end
        local tmp34 = ABCD(0, 0xF4292244)
        local tmp35 = tmp34(tmp32)
        local tmp36 = DABC(7, 0x432AFF97)
        local tmp37 = tmp36(tmp35)
        local tmp38 = CDAB(14, 0xAB9423A7)
        local tmp39 = tmp38(tmp37)
        local tmp40 = BCDA(5, 0xFC93A039)
        local tmp41 = tmp40(tmp39)
        local tmp42 = ABCD(12, 0x655B59C3)
        local tmp43 = tmp42(tmp41)
        local tmp44 = DABC(3, 0x8F0CCC92)
        tmp33 = tmp44(tmp43)
      end
      local tmp34
      do
        local tmp35 = CDAB(10, 0xFFEFF47D)
        local tmp36 = tmp35(tmp33)
        local tmp37 = BCDA(1, 0x85845DD1)
        local tmp38 = tmp37(tmp36)
        local tmp39 = ABCD(8, 0x6FA87E4F)
        local tmp40 = tmp39(tmp38)
        local tmp41 = DABC(15, 0xFE2CE6E0)
        local tmp42 = tmp41(tmp40)
        local tmp43 = CDAB(6, 0xA3014314)
        local tmp44 = tmp43(tmp42)
        local tmp45 = BCDA(13, 0x4E0811A1)
        tmp34 = tmp45(tmp44)
      end
      local tmp35 = ABCD(4, 0xF7537E82)
      local tmp36 = tmp35(tmp34)
      local tmp37 = DABC(11, 0xBD3AF235)
      local tmp38 = tmp37(tmp36)
      local tmp39 = CDAB(2, 0x2AD7D2BB)
      local tmp40 = tmp39(tmp38)
      local tmp41 = BCDA(9, 0xEB86D391)
      exp = tmp41(tmp40)
    end
    local a_PRIME = exp[1]
    b_PRIME = exp[2]
    c_PRIME = exp[3]
    d_PRIME = exp[4]
    tmp26 = _Int_add(i, 1)
    tmp27 = a + a_PRIME & 0xFFFFFFFF
  end
  local tmp28 = b + b_PRIME & 0xFFFFFFFF
  local tmp29 = c + c_PRIME & 0xFFFFFFFF
  tmp25 = tmp26
  tmp24 = tmp27
  tmp23 = tmp28
  tmp22 = tmp29
  tmp21 = d + d_PRIME & 0xFFFFFFFF
  goto cont
end
_L[192] = function(x)
  local tmp21 = _L[190](x & 0xFF)
  local tmp22
  if not math_ult(0x8, wordSize) then
    tmp22 = 0x0
  else
    tmp22 = x >> 0x8
  end
  local tmp23 = _L[190](tmp22 & 0xFF)
  local tmp24
  if not math_ult(0x10, wordSize) then
    tmp24 = 0x0
  else
    tmp24 = x >> 0x10
  end
  local tmp25 = _L[190](tmp24 & 0xFF)
  local tmp26
  if not math_ult(0x18, wordSize) then
    tmp26 = 0x0
  else
    tmp26 = x >> 0x18
  end
  local tmp27 = _L[190](tmp26)
  local tmp28 = {n = 4, tmp21, tmp23, tmp25, tmp27}
  local result = table_concat(tmp28)
  return result
end
_L[193] = function(a, b, c, d)
  local tmp21 = _L[192](a)
  local tmp22 = _L[192](b)
  local tmp23 = tmp21 .. tmp22
  local tmp24 = _L[192](c)
  local tmp25 = tmp23 .. tmp24
  local tmp26 = _L[192](d)
  return tmp25 .. tmp26
end
_L[194] = "R"
_L[195] = "B"
_L[196] = {tag = "E"}
_L[197] = {0, _L[196]}
_L[198] = function(_1, x)
  local nItems = _1[1]
  local m = _1[2]
  local nItems_PRIME = {nItems}
  local function ins(s)
    if s.tag == "E" then
      nItems_PRIME[1] = _Int_add(nItems, 1)
      return {tag = "T", payload = {_L[194], _L[196], x, _L[196]}}
    elseif s.tag == "T" then
      local color = s.payload[1]
      local a = s.payload[2]
      local y = s.payload[3]
      local b = s.payload[4]
      local exp
      if x == y then
        exp = EQUAL
      elseif x < y then
        exp = LESS
      else
        exp = GREATER
      end
      if exp == "LESS" then
        if a.tag == "T" and a.payload[1] == "R" then
          local c = a.payload[2]
          local z = a.payload[3]
          local d = a.payload[4]
          local exp1
          if x == z then
            exp1 = EQUAL
          elseif x < z then
            exp1 = LESS
          else
            exp1 = GREATER
          end
          if exp1 == "LESS" then
            local c1 = ins(c)
            if c1.tag == "T" and c1.payload[1] == "R" then
              local e = c1.payload[2]
              local w = c1.payload[3]
              local tmp21 = {tag = "T", payload = {_L[195], e, w, c1.payload[4]}}
              return {tag = "T", payload = {_L[194], tmp21, z, {tag = "T", payload = {_L[195], d, y, b}}}}
            else
              return {tag = "T", payload = {_L[195], {tag = "T", payload = {_L[194], c1, z, d}}, y, b}}
            end
          else
            goto else3
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, {tag = "T", payload = {_L[194], c, x, d}}, y, b}}
          elseif exp1 == "GREATER" then
            local d1 = ins(d)
            if d1.tag == "T" and d1.payload[1] == "R" then
              local e = d1.payload[2]
              local w = d1.payload[3]
              local f = d1.payload[4]
              local tmp21 = {tag = "T", payload = {_L[195], c, z, e}}
              return {tag = "T", payload = {_L[194], tmp21, w, {tag = "T", payload = {_L[195], f, y, b}}}}
            else
              return {tag = "T", payload = {_L[195], {tag = "T", payload = {_L[194], c, z, d1}}, y, b}}
            end
          else
            _raise(_Match, "redblack-set-fn.sml:67:31")
          end
        else
          goto else2
        end
        ::else2::
        local tmp21 = ins(a)
        return {tag = "T", payload = {_L[195], tmp21, y, b}}
      else
        goto else1
      end
      ::else1::
      if exp == "EQUAL" then
        return {tag = "T", payload = {color, a, x, b}}
      elseif exp == "GREATER" then
        if b.tag == "T" and b.payload[1] == "R" then
          local c = b.payload[2]
          local z = b.payload[3]
          local d = b.payload[4]
          local exp1
          if x == z then
            exp1 = EQUAL
          elseif x < z then
            exp1 = LESS
          else
            exp1 = GREATER
          end
          if exp1 == "LESS" then
            local c1 = ins(c)
            if c1.tag == "T" and c1.payload[1] == "R" then
              local e = c1.payload[2]
              local w = c1.payload[3]
              local f = c1.payload[4]
              local tmp21 = {tag = "T", payload = {_L[195], a, y, e}}
              return {tag = "T", payload = {_L[194], tmp21, w, {tag = "T", payload = {_L[195], f, z, d}}}}
            else
              return {tag = "T", payload = {_L[195], a, y, {tag = "T", payload = {_L[194], c1, z, d}}}}
            end
          else
            goto else3
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, a, y, {tag = "T", payload = {_L[194], c, x, d}}}}
          elseif exp1 == "GREATER" then
            local d1 = ins(d)
            if d1.tag == "T" and d1.payload[1] == "R" then
              local e = d1.payload[2]
              local w = d1.payload[3]
              local f = d1.payload[4]
              local tmp21 = {tag = "T", payload = {_L[195], a, y, c}}
              return {tag = "T", payload = {_L[194], tmp21, z, {tag = "T", payload = {_L[195], e, w, f}}}}
            else
              return {tag = "T", payload = {_L[195], a, y, {tag = "T", payload = {_L[194], c, z, d1}}}}
            end
          else
            _raise(_Match, "redblack-set-fn.sml:82:31")
          end
        else
          goto else2
        end
        ::else2::
        local tmp21 = ins(b)
        return {tag = "T", payload = {_L[195], a, y, tmp21}}
      else
        _raise(_Match, "redblack-set-fn.sml:65:47")
      end
    else
      _raise(_Match, "redblack-set-fn.sml:64:8")
    end
  end
  local exp = ins(m)
  if exp.tag == "T" then
    local a = exp.payload[2]
    local y = exp.payload[3]
    local b = exp.payload[4]
    local tmp21 = nItems_PRIME[1]
    return {tmp21, {tag = "T", payload = {_L[195], a, y, b}}}
  else
    _raise(_Bind, "redblack-set-fn.sml:96:8")
  end
end
_L[199] = function(_1, k)
  local tmp21 = _1[2]
  ::cont::
  local a = tmp21
  if a.tag == "E" then
    return false
  elseif a.tag == "T" then
    local a1 = a.payload[2]
    local y = a.payload[3]
    local b = a.payload[4]
    local exp
    if k == y then
      exp = EQUAL
    elseif k < y then
      exp = LESS
    else
      exp = GREATER
    end
    if exp == "LESS" then
      tmp21 = a1
      goto cont
    elseif exp == "EQUAL" then
      return true
    elseif exp == "GREATER" then
      tmp21 = b
      goto cont
    else
      _raise(_Match, "redblack-set-fn.sml:226:33")
    end
  else
    _raise(_Match, "redblack-set-fn.sml:225:8")
  end
end
_L[200] = "R"
_L[201] = "B"
_L[202] = {0, {tag = "E"}}
_L[203] = function(_1, xk, x)
  local nItems = _1[1]
  local m = _1[2]
  local nItems_PRIME = {nItems}
  local function ins(s)
    if s.tag == "E" then
      nItems_PRIME[1] = _Int_add(nItems, 1)
      local tmp21 = {tag = "E"}
      return {tag = "T", payload = {_L[200], tmp21, xk, x, {tag = "E"}}}
    elseif s.tag == "T" then
      local color = s.payload[1]
      local a = s.payload[2]
      local yk = s.payload[3]
      local y = s.payload[4]
      local b = s.payload[5]
      local exp
      if xk == yk then
        exp = EQUAL
      elseif xk < yk then
        exp = LESS
      else
        exp = GREATER
      end
      if exp == "LESS" then
        if a.tag == "T" and a.payload[1] == "R" then
          local c = a.payload[2]
          local zk = a.payload[3]
          local z = a.payload[4]
          local d = a.payload[5]
          local exp1
          if xk == zk then
            exp1 = EQUAL
          elseif xk < zk then
            exp1 = LESS
          else
            exp1 = GREATER
          end
          if exp1 == "LESS" then
            local c1 = ins(c)
            if c1.tag == "T" and c1.payload[1] == "R" then
              local e = c1.payload[2]
              local wk = c1.payload[3]
              local w = c1.payload[4]
              local tmp21 = {tag = "T", payload = {_L[201], e, wk, w, c1.payload[5]}}
              return {tag = "T", payload = {_L[200], tmp21, zk, z, {tag = "T", payload = {_L[201], d, yk, y, b}}}}
            else
              return {tag = "T", payload = {_L[201], {tag = "T", payload = {_L[200], c1, zk, z, d}}, yk, y, b}}
            end
          else
            goto else3
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, {tag = "T", payload = {_L[200], c, xk, x, d}}, yk, y, b}}
          elseif exp1 == "GREATER" then
            local d1 = ins(d)
            if d1.tag == "T" and d1.payload[1] == "R" then
              local e = d1.payload[2]
              local wk = d1.payload[3]
              local w = d1.payload[4]
              local f = d1.payload[5]
              local tmp21 = {tag = "T", payload = {_L[201], c, zk, z, e}}
              return {tag = "T", payload = {_L[200], tmp21, wk, w, {tag = "T", payload = {_L[201], f, yk, y, b}}}}
            else
              return {tag = "T", payload = {_L[201], {tag = "T", payload = {_L[200], c, zk, z, d1}}, yk, y, b}}
            end
          else
            _raise(_Match, "redblack-map-fn.sml:49:35")
          end
        else
          goto else2
        end
        ::else2::
        local tmp21 = ins(a)
        return {tag = "T", payload = {_L[201], tmp21, yk, y, b}}
      else
        goto else1
      end
      ::else1::
      if exp == "EQUAL" then
        return {tag = "T", payload = {color, a, xk, x, b}}
      elseif exp == "GREATER" then
        if b.tag == "T" and b.payload[1] == "R" then
          local c = b.payload[2]
          local zk = b.payload[3]
          local z = b.payload[4]
          local d = b.payload[5]
          local exp1
          if xk == zk then
            exp1 = EQUAL
          elseif xk < zk then
            exp1 = LESS
          else
            exp1 = GREATER
          end
          if exp1 == "LESS" then
            local c1 = ins(c)
            if c1.tag == "T" and c1.payload[1] == "R" then
              local e = c1.payload[2]
              local wk = c1.payload[3]
              local w = c1.payload[4]
              local f = c1.payload[5]
              local tmp21 = {tag = "T", payload = {_L[201], a, yk, y, e}}
              return {tag = "T", payload = {_L[200], tmp21, wk, w, {tag = "T", payload = {_L[201], f, zk, z, d}}}}
            else
              return {tag = "T", payload = {_L[201], a, yk, y, {tag = "T", payload = {_L[200], c1, zk, z, d}}}}
            end
          else
            goto else3
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, a, yk, y, {tag = "T", payload = {_L[200], c, xk, x, d}}}}
          elseif exp1 == "GREATER" then
            local d1 = ins(d)
            if d1.tag == "T" and d1.payload[1] == "R" then
              local e = d1.payload[2]
              local wk = d1.payload[3]
              local w = d1.payload[4]
              local f = d1.payload[5]
              local tmp21 = {tag = "T", payload = {_L[201], a, yk, y, c}}
              return {tag = "T", payload = {_L[200], tmp21, zk, z, {tag = "T", payload = {_L[201], e, wk, w, f}}}}
            else
              return {tag = "T", payload = {_L[201], a, yk, y, {tag = "T", payload = {_L[200], c, zk, z, d1}}}}
            end
          else
            _raise(_Match, "redblack-map-fn.sml:66:35")
          end
        else
          goto else2
        end
        ::else2::
        local tmp21 = ins(b)
        return {tag = "T", payload = {_L[201], a, yk, y, tmp21}}
      else
        _raise(_Match, "redblack-map-fn.sml:47:51")
      end
    else
      _raise(_Match, "redblack-map-fn.sml:46:8")
    end
  end
  local exp = ins(m)
  local a, b, y, yk
  if exp.tag == "T" then
    local a1 = exp.payload[2]
    local yk1 = exp.payload[3]
    local y1 = exp.payload[4]
    yk = yk1
    y = y1
    b = exp.payload[5]
    a = a1
  else
    _raise(_Bind, "redblack-map-fn.sml:82:8")
  end
  local tmp21 = nItems_PRIME[1]
  return {tmp21, {tag = "T", payload = {_L[201], a, yk, y, b}}}
end
_L[204] = function(_1, k)
  local tmp21 = _1[2]
  ::cont::
  local a = tmp21
  if a.tag == "E" then
    return {tag = "NONE"}
  elseif a.tag == "T" then
    local a1 = a.payload[2]
    local yk = a.payload[3]
    local y = a.payload[4]
    local b = a.payload[5]
    local exp
    if k == yk then
      exp = EQUAL
    elseif k < yk then
      exp = LESS
    else
      exp = GREATER
    end
    if exp == "LESS" then
      tmp21 = a1
      goto cont
    elseif exp == "EQUAL" then
      return {tag = "SOME", payload = y}
    elseif exp == "GREATER" then
      tmp21 = b
      goto cont
    else
      _raise(_Match, "redblack-map-fn.sml:155:37")
    end
  else
    _raise(_Match, "redblack-map-fn.sml:154:8")
  end
end
_L[205] = function(f)
  local function mapf(a)
    if a.tag == "E" then
      return {tag = "E"}
    elseif a.tag == "T" then
      local color = a.payload[1]
      local a1 = a.payload[2]
      local xk = a.payload[3]
      local x = a.payload[4]
      local b = a.payload[5]
      local tmp21 = mapf(a1)
      local tmp22 = f(x)
      local tmp23 = mapf(b)
      return {tag = "T", payload = {color, tmp21, xk, tmp22, tmp23}}
    else
      _raise(_Match, "redblack-map-fn.sml:551:8")
    end
  end
  return function(a)
    local n = a[1]
    local tmp21 = mapf(a[2])
    return {n, tmp21}
  end
end
_L[206] = require("texrunner.shellutil")
_L[207] = _L[206].escape
_L[208] = _L[206].has_command
_L[209] = _L[206].cd
_L[210] = require("texrunner.pathutil")
_L[211] = _L[210].basename
_L[212] = _L[210].dirname
_L[213] = _L[210].trimext
_L[214] = _L[210].ext
_L[215] = function(cwd, path)
  if cwd.tag == "SOME" then
    local cwd1 = cwd.payload
    local tmp21 = _L[210].abspath(path, cwd1)
    return tmp21
  elseif cwd.tag == "NONE" then
    local tmp21 = _L[210].abspath(path)
    return tmp21
  else
    _raise(_Match, "path-util.sml:22:7")
  end
end
_L[216] = require("lfs")
_L[217] = require("texrunner.fsutil")
_L[218] = _L[217].isfile
_L[219] = _L[217].isdir
_L[220] = os.type == "windows"
_L[221] = os.setenv
_L[222] = "BATCHMODE"
_L[223] = "NONSTOPMODE"
_L[224] = "SCROLLMODE"
_L[225] = "ERRORSTOPMODE"
_L[226] = "ALLOWED"
_L[227] = "RESTRICTED"
_L[228] = "FORBIDDEN"
_L[229] = function(a, b)
  return a == "ALLOWED" and b == "ALLOWED" or (a == "RESTRICTED" and b == "RESTRICTED" or a == "FORBIDDEN" and b == "FORBIDDEN")
end
_L[230] = "PDF"
_L[231] = "DVI"
_L[232] = function(a, b)
  return a == "PDF" and b == "PDF" or a == "DVI" and b == "DVI"
end
_L[233] = function(c)
  if c == 32 then
    return "_"
  end
  local tmp21 = isSpace(c)
  local tmp22
  tmp22 = tmp21 or (c == 34 or (c == 36 or (c == 37 or (c == 38 or (c == 39 or (c == 40 or (c == 41 or (c == 59 or (c == 60 or (c == 62 or (c == 92 or (c == 94 or (c == 96 or c == 124)))))))))))))
  if tmp22 then
    local x = c
    local s
    if x >= 0 then
      local result = string_format("%X", x)
      s = result
    else
      local result = string_format("~%X", - x)
      s = result
    end
    if x <= 15 then
      return "_0" .. s
    else
      return "_" .. s
    end
  else
    return string_char(c)
  end
end
_L[234] = function(c)
  if c == 92 then
    return "~\\\\"
  elseif c == 37 then
    return "~\\%"
  elseif c == 94 then
    return "~\\^"
  elseif c == 123 then
    return "~\\{"
  elseif c == 125 then
    return "~\\}"
  elseif c == 126 then
    return "~\\~"
  elseif c == 35 then
    return "~\\#"
  else
    return string_char(c)
  end
end
_L[235] = "PDFTEX"
_L[236] = "XETEX"
_L[237] = "LUATEX"
_L[238] = "OTHER"
_L[239] = function(a, b)
  return a == "PDFTEX" and b == "PDFTEX" or (a == "XETEX" and b == "XETEX" or (a == "LUATEX" and b == "LUATEX" or a == "OTHER" and b == "OTHER"))
end
_L[240] = {dvi_extension = "dvi", engine_type = _L[235], executable = "pdftex", name = "pdftex", supports_draftmode = true, supports_pdf_generation = true}
_L[241] = {dvi_extension = "dvi", engine_type = _L[235], executable = "pdflatex", name = "pdflatex", supports_draftmode = true, supports_pdf_generation = true}
_L[242] = {dvi_extension = "dvi", engine_type = _L[237], executable = "luatex", name = "luatex", supports_draftmode = true, supports_pdf_generation = true}
_L[243] = {dvi_extension = "dvi", engine_type = _L[237], executable = "lualatex", name = "lualatex", supports_draftmode = true, supports_pdf_generation = true}
_L[244] = {dvi_extension = "dvi", engine_type = _L[237], executable = "luajittex", name = "luajittex", supports_draftmode = true, supports_pdf_generation = true}
_L[245] = {dvi_extension = "xdv", engine_type = _L[236], executable = "xetex", name = "xetex", supports_draftmode = true, supports_pdf_generation = true}
_L[246] = {dvi_extension = "xdv", engine_type = _L[236], executable = "xelatex", name = "xelatex", supports_draftmode = true, supports_pdf_generation = true}
_L[247] = {dvi_extension = "dvi", engine_type = _L[238], executable = "tex", name = "tex", supports_draftmode = false, supports_pdf_generation = false}
_L[248] = {dvi_extension = "dvi", engine_type = _L[238], executable = "etex", name = "etex", supports_draftmode = false, supports_pdf_generation = false}
_L[249] = {dvi_extension = "dvi", engine_type = _L[238], executable = "latex", name = "latex", supports_draftmode = false, supports_pdf_generation = false}
_L[250] = {dvi_extension = "dvi", engine_type = _L[238], executable = "ptex", name = "ptex", supports_draftmode = false, supports_pdf_generation = false}
_L[251] = {dvi_extension = "dvi", engine_type = _L[238], executable = "eptex", name = "eptex", supports_draftmode = false, supports_pdf_generation = false}
_L[252] = {dvi_extension = "dvi", engine_type = _L[238], executable = "platex", name = "platex", supports_draftmode = false, supports_pdf_generation = false}
_L[253] = {dvi_extension = "dvi", engine_type = _L[238], executable = "uptex", name = "uptex", supports_draftmode = false, supports_pdf_generation = false}
_L[254] = {dvi_extension = "dvi", engine_type = _L[238], executable = "euptex", name = "euptex", supports_draftmode = false, supports_pdf_generation = false}
_L[255] = {dvi_extension = "dvi", engine_type = _L[238], executable = "uplatex", name = "uplatex", supports_draftmode = false, supports_pdf_generation = false}
_L[256] = function(a)
  if a == "pdftex" then
    return {tag = "SOME", payload = _L[240]}
  elseif a == "pdflatex" then
    return {tag = "SOME", payload = _L[241]}
  elseif a == "luatex" then
    return {tag = "SOME", payload = _L[242]}
  elseif a == "lualatex" then
    return {tag = "SOME", payload = _L[243]}
  elseif a == "luajittex" then
    return {tag = "SOME", payload = _L[244]}
  elseif a == "xetex" then
    return {tag = "SOME", payload = _L[245]}
  elseif a == "xelatex" then
    return {tag = "SOME", payload = _L[246]}
  elseif a == "tex" then
    return {tag = "SOME", payload = _L[247]}
  elseif a == "etex" then
    return {tag = "SOME", payload = _L[248]}
  elseif a == "latex" then
    return {tag = "SOME", payload = _L[249]}
  elseif a == "ptex" then
    return {tag = "SOME", payload = _L[250]}
  elseif a == "eptex" then
    return {tag = "SOME", payload = _L[251]}
  elseif a == "platex" then
    return {tag = "SOME", payload = _L[252]}
  elseif a == "uptex" then
    return {tag = "SOME", payload = _L[253]}
  elseif a == "euptex" then
    return {tag = "SOME", payload = _L[254]}
  elseif a == "uplatex" then
    return {tag = "SOME", payload = _L[255]}
  else
    return {tag = "NONE"}
  end
end
_L[257] = "DEFAULT"
_L[258] = "BLACK"
_L[259] = "RED"
_L[260] = "GREEN"
_L[261] = "YELLOW"
_L[262] = "BLUE"
_L[263] = "MAGENTA"
_L[264] = "CYAN"
_L[265] = "WHITE"
_L[266] = "BRIGHT_BLACK"
_L[267] = "BRIGHT_RED"
_L[268] = "BRIGHT_GREEN"
_L[269] = "BRIGHT_YELLOW"
_L[270] = "BRIGHT_BLUE"
_L[271] = "BRIGHT_MAGENTA"
_L[272] = "BRIGHT_CYAN"
_L[273] = "BRIGHT_WHITE"
_L[274] = function(a)
  if a == "DEFAULT" then
    return {tag = "NONE"}
  elseif a == "BLACK" then
    return {tag = "SOME", payload = "30"}
  elseif a == "RED" then
    return {tag = "SOME", payload = "31"}
  elseif a == "GREEN" then
    return {tag = "SOME", payload = "32"}
  elseif a == "YELLOW" then
    return {tag = "SOME", payload = "33"}
  elseif a == "BLUE" then
    return {tag = "SOME", payload = "34"}
  elseif a == "MAGENTA" then
    return {tag = "SOME", payload = "35"}
  elseif a == "CYAN" then
    return {tag = "SOME", payload = "36"}
  elseif a == "WHITE" then
    return {tag = "SOME", payload = "37"}
  elseif a == "BRIGHT_BLACK" then
    return {tag = "SOME", payload = "90"}
  elseif a == "BRIGHT_RED" then
    return {tag = "SOME", payload = "91"}
  elseif a == "BRIGHT_GREEN" then
    return {tag = "SOME", payload = "92"}
  elseif a == "BRIGHT_YELLOW" then
    return {tag = "SOME", payload = "93"}
  elseif a == "BRIGHT_BLUE" then
    return {tag = "SOME", payload = "94"}
  elseif a == "BRIGHT_MAGENTA" then
    return {tag = "SOME", payload = "95"}
  elseif a == "BRIGHT_CYAN" then
    return {tag = "SOME", payload = "96"}
  elseif a == "BRIGHT_WHITE" then
    return {tag = "SOME", payload = "97"}
  else
    _raise(_Match, "ansi-color.sml:62:7")
  end
end
_L[275] = function(a)
  if a == "DEFAULT" then
    return {tag = "NONE"}
  elseif a == "BLACK" then
    return {tag = "SOME", payload = "40"}
  elseif a == "RED" then
    return {tag = "SOME", payload = "41"}
  elseif a == "GREEN" then
    return {tag = "SOME", payload = "42"}
  elseif a == "YELLOW" then
    return {tag = "SOME", payload = "43"}
  elseif a == "BLUE" then
    return {tag = "SOME", payload = "44"}
  elseif a == "MAGENTA" then
    return {tag = "SOME", payload = "45"}
  elseif a == "CYAN" then
    return {tag = "SOME", payload = "46"}
  elseif a == "WHITE" then
    return {tag = "SOME", payload = "47"}
  elseif a == "BRIGHT_BLACK" then
    return {tag = "SOME", payload = "100"}
  elseif a == "BRIGHT_RED" then
    return {tag = "SOME", payload = "101"}
  elseif a == "BRIGHT_GREEN" then
    return {tag = "SOME", payload = "102"}
  elseif a == "BRIGHT_YELLOW" then
    return {tag = "SOME", payload = "103"}
  elseif a == "BRIGHT_BLUE" then
    return {tag = "SOME", payload = "104"}
  elseif a == "BRIGHT_MAGENTA" then
    return {tag = "SOME", payload = "105"}
  elseif a == "BRIGHT_CYAN" then
    return {tag = "SOME", payload = "106"}
  elseif a == "BRIGHT_WHITE" then
    return {tag = "SOME", payload = "107"}
  else
    _raise(_Match, "ansi-color.sml:79:7")
  end
end
_L[276] = function(_1, xs)
  if _1.tag == "SOME" then
    return {_1.payload, xs}
  elseif _1.tag == "NONE" then
    return xs
  else
    _raise(_Match, "ansi-color.sml:137:7")
  end
end
_L[277] = function(background, blink, bold, dim, foreground, italic, reverse, strike, underline)
  local attrs, tmp21
  do
    local attrs1
    if strike then
      attrs1 = {"9", nil}
    else
      attrs1 = nil
    end
    local attrs2
    if reverse then
      attrs2 = {"7", attrs1}
    else
      attrs2 = attrs1
    end
    local attrs3
    if blink then
      attrs3 = {"5", attrs2}
    else
      attrs3 = attrs2
    end
    local attrs4
    if underline then
      attrs4 = {"4", attrs3}
    else
      attrs4 = attrs3
    end
    local attrs5
    if italic then
      attrs5 = {"3", attrs4}
    else
      attrs5 = attrs4
    end
    local attrs6
    if dim then
      attrs6 = {"2", attrs5}
    else
      attrs6 = attrs5
    end
    local attrs7
    if bold then
      attrs7 = {"1", attrs6}
    else
      attrs7 = attrs6
    end
    local tmp22 = mapPartial(_L[275])
    local tmp23 = tmp22(background)
    attrs = _L[276](tmp23, attrs7)
    local tmp24 = mapPartial(_L[274])
    tmp21 = tmp24(foreground)
  end
  local attrs1 = _L[276](tmp21, attrs)
  local tmp22 = _VectorOrArray_fromList(attrs1)
  local tmp23 = table_concat(tmp22, ";")
  return "\x1B[" .. tmp23 .. "m"
end
_ENV.CLUTTEX_VERBOSITY = 0
_L[278] = "ALWAYS"
_L[279] = "AUTO"
_L[280] = "NEVER"
_L[281] = {false}
_L[282] = function(a)
  if a == "ALWAYS" then
    local enableVirtualTerminal = require("texrunner.isatty").enable_virtual_terminal
    local stderr1 = _ENV.io.stderr
    _L[281][1] = true
    if not not enableVirtualTerminal then
      local tmp21
      if not enableVirtualTerminal(stderr1) then
        local tmp22 = _ENV.CLUTTEX_VERBOSITY
        tmp21 = tmp22 >= 2
      else
        tmp21 = false
      end
      if tmp21 then
        return _L[123]({_L[117], "ClutTeX: Failed to enable virtual terminal\n"})
      else
        return nil
      end
    else
      return nil
    end
  else
    goto else1
  end
  ::else1::
  if a == "AUTO" then
    local isatty = require("texrunner.isatty")
    local enableVirtualTerminal = isatty.enable_virtual_terminal
    local stderr1 = _ENV.io.stderr
    local u = not not isatty.isatty(stderr1)
    _L[281][1] = u
    local tmp21
    tmp21 = u and not not enableVirtualTerminal
    if tmp21 then
      local tmp22 = enableVirtualTerminal(stderr1)
      _L[281][1] = tmp22
      local tmp23
      if not tmp22 then
        local tmp24 = _ENV.CLUTTEX_VERBOSITY
        tmp23 = tmp24 >= 2
      else
        tmp23 = false
      end
      if tmp23 then
        return _L[123]({_L[117], "ClutTeX: Failed to enable virtual terminal\n"})
      else
        return nil
      end
    else
      return nil
    end
  elseif a == "NEVER" then
    _L[281][1] = false
    return nil
  else
    _raise(_Match, "message.sml:26:5")
  end
end
_L[283] = {tag = "SOME", payload = _L[273]}
_L[284] = _L[277]({tag = "SOME", payload = _L[259]}, false, false, false, _L[283], false, false, false, false)
_L[285] = {_L[284]}
_L[286] = {tag = "SOME", payload = _L[264]}
_L[287] = _L[277]({tag = "NONE"}, false, false, false, _L[286], false, false, false, false)
_L[288] = {_L[287]}
_L[289] = {tag = "SOME", payload = _L[259]}
_L[290] = _L[277]({tag = "NONE"}, false, false, false, _L[289], false, false, false, false)
_L[291] = {_L[290]}
_L[292] = {tag = "SOME", payload = _L[262]}
_L[293] = _L[277]({tag = "NONE"}, false, false, false, _L[292], false, false, false, false)
_L[294] = {_L[293]}
_L[295] = {tag = "SOME", payload = _L[262]}
_L[296] = _L[277]({tag = "NONE"}, false, false, false, _L[295], false, false, false, false)
_L[297] = {_L[296]}
_L[298] = {tag = "SOME", payload = _L[263]}
_L[299] = _L[277]({tag = "NONE"}, false, false, false, _L[298], false, false, false, false)
_L[300] = {_L[299]}
_L[301] = function(style)
  local tmp21
  do
    local background = style.background
    local blink = style.blink
    local bold = style.bold
    local dim = style.dim
    local foreground = style.foreground
    local italic = style.italic
    local reverse = style.reverse
    local strike = style.strike
    tmp21 = _L[277](background, blink, bold, dim, foreground, italic, reverse, strike, style.underline)
  end
  _L[285][1] = tmp21
  return nil
end
_L[302] = function(style)
  local tmp21
  do
    local background = style.background
    local blink = style.blink
    local bold = style.bold
    local dim = style.dim
    local foreground = style.foreground
    local italic = style.italic
    local reverse = style.reverse
    local strike = style.strike
    tmp21 = _L[277](background, blink, bold, dim, foreground, italic, reverse, strike, style.underline)
  end
  _L[288][1] = tmp21
  return nil
end
_L[303] = function(style)
  local tmp21
  do
    local background = style.background
    local blink = style.blink
    local bold = style.bold
    local dim = style.dim
    local foreground = style.foreground
    local italic = style.italic
    local reverse = style.reverse
    local strike = style.strike
    tmp21 = _L[277](background, blink, bold, dim, foreground, italic, reverse, strike, style.underline)
  end
  _L[291][1] = tmp21
  return nil
end
_L[304] = function(style)
  local tmp21
  do
    local background = style.background
    local blink = style.blink
    local bold = style.bold
    local dim = style.dim
    local foreground = style.foreground
    local italic = style.italic
    local reverse = style.reverse
    local strike = style.strike
    tmp21 = _L[277](background, blink, bold, dim, foreground, italic, reverse, strike, style.underline)
  end
  _L[294][1] = tmp21
  return nil
end
_L[305] = function(style)
  local tmp21
  do
    local background = style.background
    local blink = style.blink
    local bold = style.bold
    local dim = style.dim
    local foreground = style.foreground
    local italic = style.italic
    local reverse = style.reverse
    local strike = style.strike
    tmp21 = _L[277](background, blink, bold, dim, foreground, italic, reverse, strike, style.underline)
  end
  _L[297][1] = tmp21
  return nil
end
_L[306] = function(style)
  local tmp21
  do
    local background = style.background
    local blink = style.blink
    local bold = style.bold
    local dim = style.dim
    local foreground = style.foreground
    local italic = style.italic
    local reverse = style.reverse
    local strike = style.strike
    tmp21 = _L[277](background, blink, bold, dim, foreground, italic, reverse, strike, style.underline)
  end
  _L[300][1] = tmp21
  return nil
end
_L[307] = function(commandline)
  if _L[281][1] then
    local tmp21 = _L[285][1] .. "[EXEC]\x1B[0m "
    return _L[123]({_L[117], tmp21 .. _L[288][1] .. commandline .. "\x1B[0m\n"})
  else
    return _L[123]({_L[117], "[EXEC] " .. commandline .. "\n"})
  end
end
_L[308] = function(message)
  if _L[281][1] then
    local tmp21 = _L[285][1] .. "[ERROR]\x1B[0m "
    return _L[123]({_L[117], tmp21 .. _L[291][1] .. message .. "\x1B[0m\n"})
  else
    return _L[123]({_L[117], "[ERROR] " .. message .. "\n"})
  end
end
_L[309] = function(message)
  if _L[281][1] then
    local tmp21 = _L[285][1] .. "[WARN]\x1B[0m "
    return _L[123]({_L[117], tmp21 .. _L[294][1] .. message .. "\x1B[0m\n"})
  else
    return _L[123]({_L[117], "[WARN] " .. message .. "\n"})
  end
end
_L[310] = function(message)
  if _L[281][1] then
    local tmp21 = _L[285][1] .. "[DIAG]\x1B[0m "
    return _L[123]({_L[117], tmp21 .. _L[297][1] .. message .. "\x1B[0m\n"})
  else
    return _L[123]({_L[117], "[DIAG] " .. message .. "\n"})
  end
end
_L[311] = function(message)
  if _L[281][1] then
    local tmp21 = _L[285][1] .. "[INFO]\x1B[0m "
    return _L[123]({_L[117], tmp21 .. _L[300][1] .. message .. "\x1B[0m\n"})
  else
    return _L[123]({_L[117], "[INFO] " .. message .. "\n"})
  end
end
_L[312] = "DVIPDFMX"
_L[313] = "DVIPS"
_L[314] = "DVISVGM"
_L[315] = "PDFTEX"
_L[316] = "XETEX"
_L[317] = "LUATEX"
_L[318] = "DVIPDFMX"
_L[319] = "DVIPS"
_L[320] = "DVISVGM"
_L[321] = "PDFTEX"
_L[322] = "XETEX"
_L[323] = "LUATEX"
_L[324] = "UNKNOWN"
_L[325] = function(a)
  if a == "DVIPDFMX" then
    return "dvipdfmx"
  elseif a == "DVIPS" then
    return "dvips"
  elseif a == "DVISVGM" then
    return "dvisvgm"
  elseif a == "PDFTEX" then
    return "pdftex"
  elseif a == "XETEX" then
    return "xetex"
  elseif a == "LUATEX" then
    return "luatex"
  elseif a == "UNKNOWN" then
    return "unknown"
  else
    _raise(_Match, "check-driver.sml:24:9")
  end
end
_L[326] = "PDFMODE"
_L[327] = "DVISVGM"
_L[328] = "XDVIPDFMX"
_L[329] = "DVIPDFMX"
_L[330] = "DVIPS"
_L[331] = "PDFTEX"
_L[332] = "LUATEX"
_L[333] = "XETEX"
_L[334] = "UNKNOWN"
_L[335] = function(a, b)
  return a == "PDFMODE" and b == "PDFMODE" or (a == "DVISVGM" and b == "DVISVGM" or (a == "XDVIPDFMX" and b == "XDVIPDFMX" or (a == "DVIPDFMX" and b == "DVIPDFMX" or (a == "DVIPS" and b == "DVIPS" or (a == "PDFTEX" and b == "PDFTEX" or (a == "LUATEX" and b == "LUATEX" or (a == "XETEX" and b == "XETEX" or a == "UNKNOWN" and b == "UNKNOWN")))))))
end
_L[336] = function(a)
  if a == "PDFMODE" then
    return "pdfmode"
  elseif a == "DVISVGM" then
    return "dvisvgm"
  elseif a == "XDVIPDFMX" then
    return "xdvipdfmx"
  elseif a == "DVIPDFMX" then
    return "dvipdfmx"
  elseif a == "DVIPS" then
    return "dvips"
  elseif a == "PDFTEX" then
    return "pdftex"
  elseif a == "LUATEX" then
    return "luatex"
  elseif a == "XETEX" then
    return "xetex"
  elseif a == "UNKNOWN" then
    return "unknown"
  else
    _raise(_Match, "check-driver.sml:44:9")
  end
end
_L[337] = "DVIPDFMX"
_L[338] = "DVIPS"
_L[339] = "PDFTEX"
_L[340] = "LUATEX"
_L[341] = "XETEX"
_L[342] = "UNKNOWN"
_L[343] = function(a)
  if a == "DVIPDFMX" then
    return "dvipdfmx"
  elseif a == "DVIPS" then
    return "dvips"
  elseif a == "PDFTEX" then
    return "pdftex"
  elseif a == "LUATEX" then
    return "luatex"
  elseif a == "XETEX" then
    return "xetex"
  elseif a == "UNKNOWN" then
    return "unknown"
  else
    _raise(_Match, "check-driver.sml:57:9")
  end
end
_L[344] = "PDF"
_L[345] = "DVIPS"
_L[346] = "UNKNOWN"
_L[347] = "DVIPDFMX"
_L[348] = "DVIPS"
_L[349] = "DVISVGM"
_L[350] = "FSWATCH"
_L[351] = "INOTIFYWAIT"
_L[352] = "AUTO"
_L[353] = function(a, b)
  return a == "FSWATCH" and b == "FSWATCH" or (a == "INOTIFYWAIT" and b == "INOTIFYWAIT" or a == "AUTO" and b == "AUTO")
end
_L[354] = {tag = "NOW"}
_L[355] = function(a, b)
  if a.tag == "NOW" and b.tag == "NOW" then
    return true
  elseif a.tag == "RAW" and b.tag == "RAW" then
    local a1 = a.payload
    return a1 == b.payload
  else
    return false
  end
end
_L[356] = {tag = "NONE"}
_L[357] = {tag = "NONE"}
_L[358] = {tag = "NONE"}
_L[359] = {tag = "NONE"}
_L[360] = {tag = "NONE"}
_L[361] = {tag = "NONE"}
_L[362] = {tag = "NONE"}
_L[363] = {tag = "NONE"}
_L[364] = {tag = "NONE"}
_L[365] = {epstopdf = false, minted = false, pdfx = false}
_L[366] = {tag = "NONE"}
_L[367] = {tag = "NONE"}
_L[368] = {tag = "NONE"}
_L[369] = {tag = "NONE"}
_L[370] = {tag = "NONE"}
_L[371] = {tag = "NONE"}
_L[372] = {tag = "NONE"}
_L[373] = {tag = "NONE"}
_L[374] = {tag = "NONE"}
_L[375] = {tag = "NONE"}
_L[376] = {tag = "NONE"}
_L[377] = {tag = "NONE"}
_L[378] = {bibtex_or_biber = _L[376], change_directory = _L[362], check_driver = _L[366], color = _L[361], config_file = {tag = "NONE"}, dvipdfmx_extraoptions_rev = nil, engine = _L[356], engine_executable = _L[357], file_line_error = true, fmt = _L[372], fresh = false, halt_on_error = true, includeonly = _L[363], interaction = _L[369], jobname = _L[371], make_depends = _L[364], makeglossaries = _L[377], makeindex = _L[375], max_iterations = _L[359], output = _L[358], output_directory = _L[373], output_format = _L[374], package_support = _L[365], print_output_directory = false, shell_escape = _L[370], source_date_epoch = _L[367], start_with_draft = false, synctex = _L[368], tex_extraoptions_rev = nil, watch = _L[360]}
_L[379] = function(prefix)
  return function(s)
    local tmp21 = isPrefix(prefix)
    local tmp22 = tmp21(s)
    if tmp22 then
      local tmp23 = extract1({s, #prefix, {tag = "NONE"}})
      return {tag = "SOME", payload = tmp23}
    else
      return {tag = "NONE"}
    end
  end
end
_L[380] = function(line, outdir)
  local tmp21 = _L[379]("\\@input{")
  local exp = tmp21(line)
  if exp.tag == "NONE" then
    return {tag = "NONE"}
  elseif exp.tag == "SOME" then
    local rest = exp.payload
    local tmp22 = splitl(function(c)
      return c ~= 125
    end)
    local tmp23 = tmp22(rest)
    local subauxfile = string1(tmp23[1])
    local subauxfile_abs = _L[215]({tag = "SOME", payload = outdir}, subauxfile)
    return {tag = "SOME", payload = {subauxfile = subauxfile, subauxfile_abs = subauxfile_abs}}
  else
    _raise(_Match, "auxfile.sml:15:5")
  end
end
_L[381] = function(auxfile, outdir, seen)
  local ins, tmp21, tmp22
  do
    ins = _L[114](auxfile)
    local seen1 = _L[198](seen, auxfile)
    tmp22, tmp21 = false, seen1
  end
  ::cont::
  local did, seen1 = tmp22, tmp21
  local exp = _L[113](ins)
  if exp.tag == "NONE" then
    _L[119](ins)
    return {did, seen1}
  else
    goto else1
  end
  ::else1::
  if exp.tag == "SOME" then
    local exp1 = _L[380](exp.payload, outdir)
    if exp1.tag == "NONE" then
      tmp22 = did
      tmp21 = seen1
      goto cont
    elseif exp1.tag == "SOME" then
      local subauxfile, subauxfile_abs, tmp23
      do
        subauxfile = exp1.payload.subauxfile
        subauxfile_abs = exp1.payload.subauxfile_abs
        local tmp24 = _L[199](seen1, subauxfile_abs)
        if not tmp24 then
        else
          tmp23 = false
          goto cont1
        end
        tmp23 = _L[218](subauxfile_abs)
      end
      ::cont1::
      if tmp23 then
        local exp2 = _L[381](subauxfile_abs, outdir, seen1)
        local did_PRIME = exp2[1]
        local seen2 = exp2[2]
        if did then
          tmp22 = true
          tmp21 = seen2
          goto cont
        else
          tmp22 = did_PRIME
          tmp21 = seen2
          goto cont
        end
      else
        goto else2
      end
      ::else2::
      local tmp24 = _L[212](subauxfile)
      local dir = _L[210].join(outdir, tmp24)
      local tmp25 = _L[219](dir)
      if tmp25 then
        tmp22 = did
        tmp21 = seen1
        goto cont
      end
      local tmp26, tmp27 = _L[217].mkdir_rec(dir)
      if not tmp26 then
        local tmp28 = _id(tmp27)
        _raise(tmp28, "fs-util.sml:16:28")
      else
        tmp22 = true
        tmp21 = seen1
        goto cont
      end
    else
      _raise(_Match, "auxfile.sml:34:13")
    end
  else
    _raise(_Match, "auxfile.sml:31:9")
  end
end
_L[382] = function(auxfile, outdir, revLines)
  local ins = _L[114](auxfile)
  local tmp21 = revLines
  ::cont::
  local revLines1 = tmp21
  local exp = _L[113](ins)
  if exp.tag == "NONE" then
    _L[119](ins)
    return revLines1
  else
    goto else1
  end
  ::else1::
  if exp.tag == "SOME" then
    local line = exp.payload
    local exp1 = _L[380](line, outdir)
    if exp1.tag == "SOME" then
      local subauxfile_abs = exp1.payload.subauxfile_abs
      local tmp22 = _L[218](subauxfile_abs)
      if tmp22 then
      else
        tmp21 = revLines1
        goto cont
      end
      tmp21 = _L[382](subauxfile_abs, outdir, revLines1)
      goto cont
    else
      goto else2
    end
    ::else2::
    if exp1.tag == "NONE" then
      local isBibTeXLine
      do
        local tmp22 = _L[379]("\\")
        local exp2 = tmp22(line)
        if exp2.tag == "SOME" then
          local s = exp2.payload
          local tmp23
          do
            local tmp24 = splitl(function(c)
              local tmp25
              tmp25 = 65 <= c and c <= 90 or 97 <= c and c <= 122
              return tmp25 or c == 64
            end)
            local tmp25 = tmp24(s)
            tmp23 = tmp25[1]
          end
          local exp3 = string1(tmp23)
          isBibTeXLine = exp3 == "citation" or (exp3 == "bibdata" or exp3 == "bibstyle")
        elseif exp2.tag == "NONE" then
          isBibTeXLine = false
          goto cont1
        else
          _raise(_Match, "auxfile.sml:86:21")
        end
      end
      ::cont1::
      if isBibTeXLine then
        local tmp22 = _ENV.CLUTTEX_VERBOSITY
        if tmp22 >= 2 then
          local tmp23 = full(line)
          local tmp24 = splitr(isSpace, tmp23)
          local tmp25 = string1(tmp24[1])
          _L[311]("BibTeX line: " .. tmp25)
          tmp21 = {line, revLines1}
          goto cont
        else
          tmp21 = {line, revLines1}
          goto cont
        end
      else
        tmp21 = revLines1
        goto cont
      end
    else
      _raise(_Match, "auxfile.sml:68:13")
    end
  else
    _raise(_Match, "auxfile.sml:65:9")
  end
end
_L[383] = require("texrunner.luatexinit")
_L[384] = "INPUT"
_L[385] = "OUTPUT"
_L[386] = "AUXILIARY"
_L[387] = function(a, b)
  return a == "INPUT" and b == "INPUT" or (a == "OUTPUT" and b == "OUTPUT" or a == "AUXILIARY" and b == "AUXILIARY")
end
_L[388] = function(fileInfo, fileMap)
  local tmp21 = foldl(function(a)
    local x = a[1]
    return {x[1], a[2]}
  end)
  local tmp22 = tmp21(nil)
  local tmp23 = tmp22(fileInfo)
  local tmp24 = _L[205](function(a)
    return a[1]
  end)
  local tmp25 = tmp24(fileMap)
  return {tmp23, tmp25}
end
_L[389] = function(file, makeindex, fileList, fileMap)
  local ins = _L[114](file)
  local tmp21, tmp22 = fileList, fileMap
  ::cont::
  local fileList1, fileMap1 = tmp21, tmp22
  local exp = _L[113](ins)
  if exp.tag == "NONE" then
    _L[119](ins)
    return {fileList1, fileMap1}
  else
    goto else1
  end
  ::else1::
  if exp.tag == "SOME" then
    local path, t
    do
      local line = exp.payload
      local tmp23 = splitl(isAlphaNum)
      local tmp24 = full(line)
      local exp1 = tmp23(tmp24)
      local t1 = exp1[1]
      local tmp25
      do
        local tmp26 = splitr(isSpace, exp1[2])
        tmp25 = tmp26[1]
      end
      local tmp26
      do
        local tmp27 = splitl(isSpace)
        local tmp28 = tmp27(tmp25)
        tmp26 = tmp28[2]
      end
      path = string1(tmp26)
      t = string1(t1)
      if t == "PWD" then
        tmp21 = fileList1
        tmp22 = fileMap1
        goto cont
      elseif t == "INPUT" then
        local abspath = _L[215]({tag = "NONE"}, path)
        local exp2 = _L[204](fileMap1, abspath)
        if exp2.tag == "SOME" then
          local r = exp2.payload
          local path_PRIME = exp2.payload[1].path
          local abspath_PRIME = exp2.payload[1].abspath
          local kind = exp2.payload[1].kind
          local tmp27 = #path
          local tmp28
          if tmp27 < #path_PRIME then
            tmp28 = path
          else
            tmp28 = path_PRIME
          end
          local tmp29 = _L[387](kind, _L[385])
          local tmp30
          if tmp29 then
            tmp30 = _L[386]
          else
            tmp30 = kind
          end
          r[1] = {abspath = abspath_PRIME, kind = tmp30, path = tmp28}
          tmp21 = fileList1
          tmp22 = fileMap1
          goto cont
        else
          goto else4
        end
        ::else4::
        if exp2.tag == "NONE" then
          local tmp27 = _L[218](path)
          if tmp27 then
            local tmp28 = _L[214](path)
            local tmp29
            if tmp28 == "bbl" then
              tmp29 = _L[386]
            else
              tmp29 = _L[384]
            end
            local r = {{abspath = abspath, kind = tmp29, path = path}}
            local tmp30 = {r, fileList1}
            local tmp31 = _L[203](fileMap1, abspath, r)
            tmp21 = tmp30
            tmp22 = tmp31
            goto cont
          else
            tmp21 = fileList1
            tmp22 = fileMap1
            goto cont
          end
        else
          _raise(_Match, "reruncheck.sml:38:31")
        end
      else
        goto else2
      end
    end
    ::else2::
    if t == "OUTPUT" then
      local abspath = _L[215]({tag = "NONE"}, path)
      local exp1 = _L[204](fileMap1, abspath)
      if exp1.tag == "SOME" then
        local r = exp1.payload
        local path_PRIME = exp1.payload[1].path
        local abspath_PRIME = exp1.payload[1].abspath
        local kind = exp1.payload[1].kind
        local tmp23 = #path
        local tmp24
        if tmp23 < #path_PRIME then
          tmp24 = path
        else
          tmp24 = path_PRIME
        end
        local tmp25 = _L[387](kind, _L[384])
        local tmp26
        if tmp25 then
          tmp26 = _L[386]
        else
          tmp26 = kind
        end
        r[1] = {abspath = abspath_PRIME, kind = tmp26, path = tmp24}
        tmp21 = fileList1
        tmp22 = fileMap1
        goto cont
      else
        goto else4
      end
      ::else4::
      if exp1.tag == "NONE" then
        local tmp23
        do
          local ext = _L[214](path)
          if ext == "out" then
            tmp23 = true
            goto cont1
          end
          local tmp24 = tmp(eq)
          local tmp25
          do
            local tmp26 = tmp24(makeindex, {tag = "NONE"})
            tmp25 = not tmp26
          end
          local tmp26
          tmp26 = tmp25 and ext == "idx"
          tmp23 = tmp26 or (ext == "bcf" or ext == "glo")
        end
        ::cont1::
        local tmp24
        if tmp23 then
          tmp24 = _L[386]
        else
          tmp24 = _L[385]
        end
        local r = {{abspath = abspath, kind = tmp24, path = path}}
        local tmp25 = {r, fileList1}
        local tmp26 = _L[203](fileMap1, abspath, r)
        tmp21 = tmp25
        tmp22 = tmp26
        goto cont
      else
        _raise(_Match, "reruncheck.sml:70:31")
      end
    else
      goto else3
    end
    ::else3::
    _L[309]("Unrecognized line in recorder file '" .. file .. "': " .. t)
    tmp21 = fileList1
    tmp22 = fileMap1
    goto cont
  else
    _raise(_Match, "reruncheck.sml:29:17")
  end
end
_L[390] = function(fileList, auxstatus)
  local shouldRerun, auxstatus1
  do
    local tmp21, tmp22 = fileList, _L[202]
    ::cont1::
    local _1, newauxstatus = tmp21, tmp22
    if _1 == nil then
      auxstatus1 = newauxstatus
      shouldRerun = false
      goto cont
    elseif _1 ~= nil then
      local shortPath = _1[1].path
      local abspath = _1[1].abspath
      local kind = _1[1].kind
      local tmp23 = _1[2]
      local tmp24 = _L[218](abspath)
      if tmp24 then
        local shouldRerun1, newauxstatus1
        if kind == "INPUT" then
          local mtime = _L[69](abspath)
          local exp = _L[204](auxstatus, abspath)
          if exp.tag == "SOME" and exp.payload.mtime.tag == "SOME" then
            if exp.payload.mtime.payload < mtime then
              _L[311]("Input file '" .. shortPath .. "' was modified (by user, or some external commands).")
              local tmp25 = {tag = "SOME", payload = mtime}
              local tmp26 = {tag = "NONE"}
              local tmp27 = _L[203](newauxstatus, abspath, {{md5sum = {tag = "NONE"}, mtime = tmp25, size = tmp26}})
              newauxstatus1 = tmp27
              shouldRerun1 = true
              goto cont2
            else
              newauxstatus1 = newauxstatus
              shouldRerun1 = false
              goto cont2
            end
          else
            newauxstatus1 = newauxstatus
            shouldRerun1 = false
            goto cont2
          end
        else
          goto else1
        end
        ::else1::
        if kind == "AUXILIARY" then
          local exp = _L[204](auxstatus, abspath)
          if exp.tag == "SOME" then
            local newauxstatus2, modifiedBecause
            do
              local s = exp.payload
              local size1 = _L[70](abspath)
              local exp1 = s.size
              local sizeIsDifferent
              if exp1.tag == "SOME" then
                sizeIsDifferent = exp1.payload ~= size1
              elseif exp1.tag == "NONE" then
                sizeIsDifferent = true
              else
                _raise(_Match, "reruncheck.sml:178:66")
              end
              if sizeIsDifferent then
                local exp2 = s.size
                local previousSize
                if exp2.tag == "SOME" then
                  local z = exp2.payload
                  local previousSize1 = gsub(tostring(z), "-", "~")
                  previousSize = previousSize1
                elseif exp2.tag == "NONE" then
                  previousSize = "(N/A)"
                else
                  _raise(_Match, "reruncheck.sml:183:77")
                end
                local tmp25 = "size: " .. previousSize .. " -> "
                local tmp26 = gsub(tostring(size1), "-", "~")
                local tmp27 = {tag = "SOME", payload = tmp25 .. tmp26}
                local tmp28 = {tag = "NONE"}
                local tmp29 = {tag = "SOME", payload = size1}
                local tmp30 = _L[203](newauxstatus, abspath, {{md5sum = {tag = "NONE"}, mtime = tmp28, size = tmp29}})
                newauxstatus2 = tmp30
                modifiedBecause = tmp27
                goto cont3
              else
                goto else4
              end
              ::else4::
              local md5sum
              do
                local ins = _L[150](abspath)
                local tmp25 = _L[152](ins)
                _L[151](ins)
                md5sum = _L[191](tmp25)
              end
              local exp2 = s.md5sum
              local md5sumIsDifferent
              if exp2.tag == "SOME" then
                local h = exp2.payload
                local _1_1 = h[1]
                local _2 = h[2]
                local _3 = h[3]
                local _4 = h[4]
                local _1_2 = md5sum[1]
                local _2_1 = md5sum[2]
                local _3_1 = md5sum[3]
                md5sumIsDifferent = _4 ~= md5sum[4] or (_3 ~= _3_1 or (_2 ~= _2_1 or _1_1 ~= _1_2))
              elseif exp2.tag == "NONE" then
                md5sumIsDifferent = true
              else
                _raise(_Match, "reruncheck.sml:190:82")
              end
              if md5sumIsDifferent then
                local previousMd5sum
                do
                  local exp3 = s.md5sum
                  if exp3.tag == "SOME" then
                    local h = exp3.payload
                    local a = h[1]
                    local b = h[2]
                    local c = h[3]
                    previousMd5sum = _L[193](a, b, c, h[4])
                  elseif exp3.tag == "NONE" then
                    previousMd5sum = "(N/A)"
                    goto cont4
                  else
                    _raise(_Match, "reruncheck.sml:194:86")
                  end
                end
                ::cont4::
                local tmp25 = "md5: " .. previousMd5sum .. " -> "
                local tmp26
                do
                  local a = md5sum[1]
                  local b = md5sum[2]
                  local c = md5sum[3]
                  tmp26 = _L[193](a, b, c, md5sum[4])
                end
                local tmp27 = {tag = "SOME", payload = tmp25 .. tmp26}
                local tmp28 = {tag = "NONE"}
                local tmp29 = {tag = "SOME", payload = size1}
                local tmp30 = _L[203](newauxstatus, abspath, {{md5sum = {tag = "SOME", payload = md5sum}, mtime = tmp28, size = tmp29}})
                newauxstatus2 = tmp30
                modifiedBecause = tmp27
              else
                newauxstatus2 = newauxstatus
                modifiedBecause = {tag = "NONE"}
                goto cont3
              end
            end
            ::cont3::
            if modifiedBecause.tag == "SOME" then
              local reason = modifiedBecause.payload
              _L[311]("File '" .. shortPath .. "' was modified (" .. reason .. ").")
              newauxstatus1 = newauxstatus2
              shouldRerun1 = true
              goto cont2
            else
              goto else3
            end
            ::else3::
            if modifiedBecause.tag == "NONE" then
              local tmp25 = _ENV.CLUTTEX_VERBOSITY
              if tmp25 >= 1 then
              else
                newauxstatus1 = newauxstatus2
                shouldRerun1 = false
                goto cont2
              end
              _L[311]("File '" .. shortPath .. "' unmodified (size and md5sum).")
              newauxstatus1 = newauxstatus2
              shouldRerun1 = false
              goto cont2
            else
              _raise(_Match, "reruncheck.sml:202:43")
            end
          else
            goto else2
          end
          ::else2::
          if exp.tag == "NONE" then
            local newauxstatus2, shouldRerun2
            do
              local tmp25 = isSuffix(".aux")
              local tmp26 = tmp25(abspath)
              if tmp26 then
                local size1 = _L[70](abspath)
                if size1 == 8 then
                  local ins = _L[150](abspath)
                  local tmp27 = _L[152](ins)
                  _L[151](ins)
                  local isTrivial = tmp27 == "\\relax \n"
                  local tmp28 = {tag = "NONE"}
                  local tmp29 = {tag = "SOME", payload = size1}
                  local tmp30 = _L[191](tmp27)
                  local newauxstatus3 = _L[203](newauxstatus, abspath, {{md5sum = {tag = "SOME", payload = tmp30}, mtime = tmp28, size = tmp29}})
                  newauxstatus2 = newauxstatus3
                  shouldRerun2 = not isTrivial
                  goto cont3
                else
                  goto else4
                end
                ::else4::
                local tmp27 = {tag = "NONE"}
                local tmp28 = {tag = "SOME", payload = size1}
                local newauxstatus3 = _L[203](newauxstatus, abspath, {{md5sum = {tag = "NONE"}, mtime = tmp27, size = tmp28}})
                newauxstatus2 = newauxstatus3
                shouldRerun2 = true
              else
                newauxstatus2 = newauxstatus
                shouldRerun2 = true
                goto cont3
              end
            end
            ::cont3::
            if shouldRerun2 then
            else
              goto else3
            end
            _L[311]("New auxiliary file '" .. shortPath .. "'.")
            newauxstatus1 = newauxstatus2
            shouldRerun1 = shouldRerun2
            goto cont2
            ::else3::
            local tmp25 = _ENV.CLUTTEX_VERBOSITY
            if tmp25 >= 1 then
            else
              newauxstatus1 = newauxstatus2
              shouldRerun1 = shouldRerun2
              goto cont2
            end
            _L[311]("Ignoring almost-empty auxiliary file '" .. shortPath .. "'.")
            newauxstatus1 = newauxstatus2
            shouldRerun1 = shouldRerun2
          else
            _raise(_Match, "reruncheck.sml:175:36")
          end
        elseif kind == "OUTPUT" then
          newauxstatus1 = newauxstatus
          shouldRerun1 = false
          goto cont2
        else
          _raise(_Match, "reruncheck.sml:159:31")
        end
        ::cont2::
        if shouldRerun1 then
          auxstatus1 = newauxstatus1
          shouldRerun = true
        else
          tmp21 = tmp23
          tmp22 = newauxstatus1
          goto cont1
        end
      else
        tmp21 = tmp23
        tmp22 = newauxstatus
        goto cont1
      end
    else
      _raise(_Match, "reruncheck.sml:155:15")
    end
  end
  ::cont::
  local tmp21 = _L[205](function(a)
    return a[1]
  end)
  local tmp22 = tmp21(auxstatus1)
  return {shouldRerun, tmp22}
end
_L[391] = function(auxstatus, dst, srcAbs)
  local tmp21 = _L[218](dst)
  if not tmp21 then
    return true
  end
  local exp = _L[204](auxstatus, srcAbs)
  if exp.tag == "SOME" and exp.payload.mtime.tag == "SOME" then
    local mtime = exp.payload.mtime.payload
    local tmp22 = _L[69](dst)
    return mtime > tmp22
  else
    return false
  end
end
_L[392] = {tag = "NONE"}
_L[393] = {tag = "NONE"}
_L[394] = {tag = "NONE"}
_L[395] = {tag = "NONE"}
_L[396] = {tag = "NONE"}
_L[397] = {tag = "NONE"}
_L[398] = {color = {diagnostic = _L[397], error = _L[395], execute = _L[394], information = {tag = "NONE"}, type_ = _L[393], warning = _L[396]}, temporary_directory = _L[392]}
_L[399] = function(table, key)
  local tmp21 = find(function(a)
    return a[1] == key
  end)
  local exp = tmp21(table)
  if exp.tag == "NONE" then
    return {tag = "NONE"}
  elseif exp.tag == "SOME" then
    return {tag = "SOME", payload = exp.payload[2]}
  else
    _raise(_Match, "config-file.sml:42:5")
  end
end
_L[400] = function(_1, key)
  if _1.tag == "SOME" then
    local table = _1.payload
    local tmp21 = find(function(a)
      return a[1] == key
    end)
    local exp = tmp21(table)
    if exp.tag == "NONE" then
      return {tag = "NONE"}
    elseif exp.tag == "SOME" then
      return {tag = "SOME", payload = exp.payload[2]}
    else
      _raise(_Match, "config-file.sml:47:10")
    end
  elseif _1.tag == "NONE" then
    return {tag = "NONE"}
  else
    _raise(_Match, "config-file.sml:46:7")
  end
end
_L[401] = function(_1, f)
  if _1.tag == "SOME" then
    return f(_1.payload)
  elseif _1.tag == "NONE" then
    return {tag = "NONE"}
  else
    _raise(_Match, "config-file.sml:54:7")
  end
end
_L[402] = function(path)
  return function(a)
    if a.tag == "BOOL" then
      return {tag = "SOME", payload = a.payload}
    end
    _L[309]("Config entry " .. path .. " should be a boolean.")
    return {tag = "NONE"}
  end
end
_L[403] = function(path)
  return function(a)
    if a.tag == "STRING" then
      local x = a.payload
      local exp
      if x == "default" then
        exp = {tag = "SOME", payload = _L[257]}
      elseif x == "black" then
        exp = {tag = "SOME", payload = _L[258]}
      elseif x == "red" then
        exp = {tag = "SOME", payload = _L[259]}
      elseif x == "green" then
        exp = {tag = "SOME", payload = _L[260]}
      elseif x == "yellow" then
        exp = {tag = "SOME", payload = _L[261]}
      elseif x == "blue" then
        exp = {tag = "SOME", payload = _L[262]}
      elseif x == "magenta" then
        exp = {tag = "SOME", payload = _L[263]}
      elseif x == "cyan" then
        exp = {tag = "SOME", payload = _L[264]}
      elseif x == "white" then
        exp = {tag = "SOME", payload = _L[265]}
      elseif x == "brightblack" then
        exp = {tag = "SOME", payload = _L[266]}
      elseif x == "brightred" then
        exp = {tag = "SOME", payload = _L[267]}
      elseif x == "brightgreen" then
        exp = {tag = "SOME", payload = _L[268]}
      elseif x == "brightyellow" then
        exp = {tag = "SOME", payload = _L[269]}
      elseif x == "brightblue" then
        exp = {tag = "SOME", payload = _L[270]}
      elseif x == "brightmagenta" then
        exp = {tag = "SOME", payload = _L[271]}
      elseif x == "brightcyan" then
        exp = {tag = "SOME", payload = _L[272]}
      elseif x == "brightwhite" then
        exp = {tag = "SOME", payload = _L[273]}
      else
        exp = {tag = "NONE"}
      end
      if exp.tag == "SOME" then
        return {tag = "SOME", payload = exp.payload}
      elseif exp.tag == "NONE" then
        _L[309]("Config entry " .. path .. " should be a valid color.")
        return {tag = "NONE"}
      else
        _raise(_Match, "config-file.sml:76:10")
      end
    else
      goto else1
    end
    ::else1::
    _L[309]("Config entry " .. path .. " should be a string.")
    return {tag = "NONE"}
  end
end
_L[404] = function(path)
  return function(a)
    if a.tag == "TABLE" then
      local t, tmp21, tmp22, tmp23, tmp24
      do
        t = a.payload
        local tmp25 = _L[399](t, "fore")
        local tmp26 = _L[403](path .. ".fore")
        tmp21 = _L[401](tmp25, tmp26)
        local tmp27 = _L[399](t, "back")
        local tmp28 = _L[403](path .. ".back")
        tmp22 = _L[401](tmp27, tmp28)
        local tmp29 = _L[399](t, "bold")
        local tmp30 = _L[402](path .. ".bold")
        local tmp31 = _L[401](tmp29, tmp30)
        tmp23 = getOpt(tmp31, false)
        local tmp32 = _L[399](t, "dim")
        local tmp33 = _L[402](path .. ".dim")
        local tmp34 = _L[401](tmp32, tmp33)
        tmp24 = getOpt(tmp34, false)
      end
      local tmp25, tmp26, tmp27, tmp28
      do
        local tmp29 = _L[399](t, "underline")
        local tmp30 = _L[402](path .. ".underline")
        local tmp31 = _L[401](tmp29, tmp30)
        tmp25 = getOpt(tmp31, false)
        local tmp32 = _L[399](t, "blink")
        local tmp33 = _L[402](path .. ".blink")
        local tmp34 = _L[401](tmp32, tmp33)
        tmp26 = getOpt(tmp34, false)
        local tmp35 = _L[399](t, "reverse")
        local tmp36 = _L[402](path .. ".reverse")
        local tmp37 = _L[401](tmp35, tmp36)
        tmp27 = getOpt(tmp37, false)
        local tmp38 = _L[399](t, "italic")
        local tmp39 = _L[402](path .. ".italic")
        tmp28 = _L[401](tmp38, tmp39)
      end
      local tmp29 = getOpt(tmp28, false)
      local tmp30 = _L[399](t, "strike")
      local tmp31 = _L[402](path .. ".strike")
      local tmp32 = _L[401](tmp30, tmp31)
      local tmp33 = getOpt(tmp32, false)
      return {tag = "SOME", payload = {background = tmp22, blink = tmp26, bold = tmp23, dim = tmp24, foreground = tmp21, italic = tmp29, reverse = tmp27, strike = tmp33, underline = tmp25}}
    else
      goto else1
    end
    ::else1::
    _L[309]("Config entry " .. path .. " should be a table.")
    return {tag = "NONE"}
  end
end
_L[405] = {"Abort"}
_L[406] = setmetatable({tag = _L[405]}, _exn_meta)
setlocale("", "ctype")
_L[407] = function(a)
  local tmp21 = a
  ::cont::
  local a1 = tmp21
  if a1 == nil then
    return {tag = "NONE"}
  elseif a1 ~= nil then
    local tmp22 = a1[1]
    local tmp23 = a1[2]
    local exp
    do
      local result = getenv(tmp22)
      if result == nil then
        exp = {tag = "NONE"}
      else
        exp = {tag = "SOME", payload = result}
      end
    end
    if exp.tag == "SOME" then
      return {tag = "SOME", payload = exp.payload}
    elseif exp.tag == "NONE" then
      tmp21 = tmp23
      goto cont
    else
      _raise(_Match, "main.sml:27:32")
    end
  else
    _raise(_Match, "main.sml:26:5")
  end
end
_L[408] = function(status)
  local progName
  do
    local progName1 = _L[153][0]
    if progName1 == nil then
      _raise(setmetatable({tag = _Fail_tag, payload = "CommandLine.name: arg is not available"}, _exn_meta), "command-line.sml:10:45")
    else
      progName = progName1
    end
  end
  _L[123]({_L[117], "ClutTeX: Process TeX files without cluttering your working directory\n\nUsage:\n  " .. progName .. " [options] [--] FILE.tex\n\nOptions:\n  -e, --engine=ENGINE          Specify which TeX engine to use.\n                                 ENGINE is one of the following:\n                                     pdflatex, pdftex,\n                                     lualatex, luatex, luajittex,\n                                     xelatex, xetex, latex, etex, tex,\n                                     platex, eptex, ptex,\n                                     uplatex, euptex, uptex,\n      --engine-executable=COMMAND+OPTIONs\n                               The actual TeX command to use.\n                                 [default: ENGINE]\n  -o, --output=FILE            The name of output file.\n                                 [default: JOBNAME.pdf or JOBNAME.dvi]\n      --fresh                  Clean intermediate files before running TeX.\n                                 Cannot be used with --output-directory.\n      --max-iterations=N       Maximum number of running TeX to resolve\n                                 cross-references.  [default: 4]\n      --start-with-draft       Start with draft mode.\n      --[no-]change-directory  Change directory before running TeX.\n      --watch[=ENGINE]         Watch input files for change.  Requires fswatch\n                                 or inotifywait to be installed. ENGINE is one of\n                                 `fswatch', `inotifywait' or `auto' [default: `auto']\n      --tex-option=OPTION      Pass OPTION to TeX as a single option.\n      --tex-options=OPTIONs    Pass OPTIONs to TeX as multiple options.\n      --dvipdfmx-option[s]=OPTION[s]  Same for dvipdfmx.\n      --makeindex=COMMAND+OPTIONs  Command to generate index, such as\n                                     `makeindex' or `mendex'.\n      --bibtex=COMMAND+OPTIONs     Command for BibTeX, such as\n                                     `bibtex' or `pbibtex'.\n      --biber[=COMMAND+OPTIONs]    Command for Biber.\n      --makeglossaries[=COMMAND+OPTIONs]  Command for makeglossaries.\n  -h, --help                   Print this message and exit.\n  -v, --version                Print version information and exit.\n  -V, --verbose                Be more verbose.\n      --color[=WHEN]           Make ClutTeX's message colorful. WHEN is one of\n                                 `always', `auto', or `never'.\n                                 [default: `auto' if --color is omitted,\n                                           `always' if WHEN is omitted]\n      --includeonly=NAMEs      Insert '\\includeonly{NAMEs}'.\n      --make-depends=FILE      Write dependencies as a Makefile rule.\n      --print-output-directory  Print the output directory and exit.\n      --package-support=PKG1[,PKG2,...]\n                               Enable special support for some shell-escaping\n                                 packages.\n                               Currently supported: minted, epstopdf, pdfx\n      --check-driver=DRIVER    Check that the correct driver file is loaded.\n                               DRIVER is one of `dvipdfmx', `dvips', `dvisvgm'.\n      --source-date-epoch=TIME\n                               Set SOURCE_DATE_EPOCH variable.\n                               TIME is `now' or an integer.\n\n      --[no-]shell-escape\n      --shell-restricted\n      --synctex=NUMBER\n      --fmt=FMTNAME\n      --[no-]file-line-error   [default: yes]\n      --[no-]halt-on-error     [default: yes]\n      --interaction=STRING     [default: nonstopmode]\n      --jobname=STRING\n      --output-directory=DIR   [default: somewhere in the temporary directory]\n      --output-format=FORMAT   FORMAT is `pdf' or `dvi'.  [default: pdf]\n\nCopyright (C) 2016-2026  ARATA Mizuki\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <http://www.gnu.org/licenses/>.\n"})
  exit(status, true)
end
_L[409] = function(payload)
  return {tag = "OPT_ENGINE", payload = payload}
end
_L[410] = function(payload)
  return {tag = "OPT_ENGINE_EXECUTABLE", payload = payload}
end
_L[411] = function(payload)
  return {tag = "OPT_OUTPUT", payload = payload}
end
_L[412] = {tag = "OPT_FRESH"}
_L[413] = function(payload)
  return {tag = "OPT_MAX_ITERATIONS", payload = payload}
end
_L[414] = {tag = "OPT_START_WITH_DRAFT"}
_L[415] = function(payload)
  return {tag = "OPT_WATCH", payload = payload}
end
_L[416] = {tag = "OPT_HELP"}
_L[417] = {tag = "OPT_VERSION"}
_L[418] = {tag = "OPT_VERBOSE"}
_L[419] = function(payload)
  return {tag = "OPT_COLOR", payload = payload}
end
_L[420] = function(payload)
  return {tag = "OPT_INCLUDEONLY", payload = payload}
end
_L[421] = function(payload)
  return {tag = "OPT_MAKE_DEPENDS", payload = payload}
end
_L[422] = {tag = "OPT_PRINT_OUTPUT_DIRECTORY"}
_L[423] = function(payload)
  return {tag = "OPT_PACKAGE_SUPPORT", payload = payload}
end
_L[424] = function(payload)
  return {tag = "OPT_CHECK_DRIVER", payload = payload}
end
_L[425] = function(payload)
  return {tag = "OPT_SOURCE_DATE_EPOCH", payload = payload}
end
_L[426] = function(payload)
  return {tag = "OPT_SYNCTEX", payload = payload}
end
_L[427] = function(payload)
  return {tag = "OPT_INTERACTION", payload = payload}
end
_L[428] = function(payload)
  return {tag = "OPT_JOBNAME", payload = payload}
end
_L[429] = function(payload)
  return {tag = "OPT_FMT", payload = payload}
end
_L[430] = function(payload)
  return {tag = "OPT_OUTPUT_DIRECTORY", payload = payload}
end
_L[431] = function(payload)
  return {tag = "OPT_OUTPUT_FORMAT", payload = payload}
end
_L[432] = function(payload)
  return {tag = "OPT_TEX_OPTION", payload = payload}
end
_L[433] = function(payload)
  return {tag = "OPT_TEX_OPTIONS", payload = payload}
end
_L[434] = function(payload)
  return {tag = "OPT_DVIPDFMX_OPTION", payload = payload}
end
_L[435] = function(payload)
  return {tag = "OPT_DVIPDFMX_OPTIONS", payload = payload}
end
_L[436] = function(payload)
  return {tag = "OPT_MAKEINDEX", payload = payload}
end
_L[437] = function(payload)
  return {tag = "OPT_BIBTEX", payload = payload}
end
_L[438] = function(payload)
  return {tag = "OPT_BIBER", payload = payload}
end
_L[439] = function(payload)
  return {tag = "OPT_MAKEGLOSSARIES", payload = payload}
end
_L[440] = function(payload)
  return {tag = "OPT_CONFIG_FILE", payload = payload}
end
_L[441] = {tag = "SHORT", payload = "-e"}
_L[442] = {_L[441], {tag = "WITH_ARG", payload = _L[409]}}
_L[443] = {tag = "LONG", payload = "--engine"}
_L[444] = {_L[443], {tag = "WITH_ARG", payload = _L[409]}}
_L[445] = {tag = "LONG", payload = "--engine-executable"}
_L[446] = {_L[445], {tag = "WITH_ARG", payload = _L[410]}}
_L[447] = {tag = "LONG", payload = "--output"}
_L[448] = {_L[447], {tag = "WITH_ARG", payload = _L[411]}}
_L[449] = {tag = "LONG", payload = "--fresh"}
_L[450] = {_L[449], {tag = "SIMPLE", payload = _L[412]}}
_L[451] = {tag = "LONG", payload = "--max-iterations"}
_L[452] = {_L[451], {tag = "WITH_ARG", payload = _L[413]}}
_L[453] = {tag = "LONG", payload = "--start-with-draft"}
_L[454] = {_L[453], {tag = "SIMPLE", payload = _L[414]}}
_L[455] = {tag = "LONG", payload = "--change-directory"}
_L[456] = {_L[455], {tag = "SIMPLE", payload = {tag = "OPT_CHANGE_DIRECTORY", payload = true}}}
_L[457] = {tag = "LONG", payload = "--no-change-directory"}
_L[458] = {_L[457], {tag = "SIMPLE", payload = {tag = "OPT_CHANGE_DIRECTORY", payload = false}}}
_L[459] = {tag = "LONG", payload = "--watch"}
_L[460] = {_L[459], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[415], default = "auto"}}}
_L[461] = {tag = "SHORT", payload = "-h"}
_L[462] = {_L[461], {tag = "SIMPLE", payload = _L[416]}}
_L[463] = {tag = "LONG", payload = "-help"}
_L[464] = {_L[463], {tag = "SIMPLE", payload = _L[416]}}
_L[465] = {tag = "LONG", payload = "--help"}
_L[466] = {_L[465], {tag = "SIMPLE", payload = _L[416]}}
_L[467] = {tag = "SHORT", payload = "-v"}
_L[468] = {_L[467], {tag = "SIMPLE", payload = _L[417]}}
_L[469] = {tag = "LONG", payload = "--version"}
_L[470] = {_L[469], {tag = "SIMPLE", payload = _L[417]}}
_L[471] = {tag = "SHORT", payload = "-V"}
_L[472] = {_L[471], {tag = "SIMPLE", payload = _L[418]}}
_L[473] = {tag = "LONG", payload = "--verbose"}
_L[474] = {_L[473], {tag = "SIMPLE", payload = _L[418]}}
_L[475] = {tag = "LONG", payload = "--color"}
_L[476] = {_L[475], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[419], default = "always"}}}
_L[477] = {tag = "LONG", payload = "--includeonly"}
_L[478] = {_L[477], {tag = "WITH_ARG", payload = _L[420]}}
_L[479] = {tag = "LONG", payload = "--make-depends"}
_L[480] = {_L[479], {tag = "WITH_ARG", payload = _L[421]}}
_L[481] = {tag = "LONG", payload = "--print-output-directory"}
_L[482] = {_L[481], {tag = "SIMPLE", payload = _L[422]}}
_L[483] = {tag = "LONG", payload = "--package-support"}
_L[484] = {_L[483], {tag = "WITH_ARG", payload = _L[423]}}
_L[485] = {tag = "LONG", payload = "--check-driver"}
_L[486] = {_L[485], {tag = "WITH_ARG", payload = _L[424]}}
_L[487] = {tag = "LONG", payload = "--source-date-epoch"}
_L[488] = {_L[487], {tag = "WITH_ARG", payload = _L[425]}}
_L[489] = {tag = "LONG", payload = "-synctex"}
_L[490] = {_L[489], {tag = "WITH_ARG", payload = _L[426]}}
_L[491] = {tag = "LONG", payload = "--synctex"}
_L[492] = {_L[491], {tag = "WITH_ARG", payload = _L[426]}}
_L[493] = {tag = "LONG", payload = "-file-line-error"}
_L[494] = {_L[493], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = true}}}
_L[495] = {tag = "LONG", payload = "--file-line-error"}
_L[496] = {_L[495], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = true}}}
_L[497] = {tag = "LONG", payload = "-no-file-line-error"}
_L[498] = {_L[497], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = false}}}
_L[499] = {tag = "LONG", payload = "--no-file-line-error"}
_L[500] = {_L[499], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = false}}}
_L[501] = {tag = "LONG", payload = "-interaction"}
_L[502] = {_L[501], {tag = "WITH_ARG", payload = _L[427]}}
_L[503] = {tag = "LONG", payload = "--interaction"}
_L[504] = {_L[503], {tag = "WITH_ARG", payload = _L[427]}}
_L[505] = {tag = "LONG", payload = "-halt-on-error"}
_L[506] = {_L[505], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = true}}}
_L[507] = {tag = "LONG", payload = "--halt-on-error"}
_L[508] = {_L[507], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = true}}}
_L[509] = {tag = "LONG", payload = "-no-halt-on-error"}
_L[510] = {_L[509], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = false}}}
_L[511] = {tag = "LONG", payload = "--no-halt-on-error"}
_L[512] = {_L[511], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = false}}}
_L[513] = {tag = "LONG", payload = "-shell-escape"}
_L[514] = {_L[513], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[226]}}}
_L[515] = {tag = "LONG", payload = "--shell-escape"}
_L[516] = {_L[515], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[226]}}}
_L[517] = {tag = "LONG", payload = "-no-shell-escape"}
_L[518] = {_L[517], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[228]}}}
_L[519] = {tag = "LONG", payload = "--no-shell-escape"}
_L[520] = {_L[519], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[228]}}}
_L[521] = {tag = "LONG", payload = "-shell-restricted"}
_L[522] = {_L[521], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[227]}}}
_L[523] = {tag = "LONG", payload = "--shell-restricted"}
_L[524] = {_L[523], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[227]}}}
_L[525] = {tag = "LONG", payload = "-jobname"}
_L[526] = {_L[525], {tag = "WITH_ARG", payload = _L[428]}}
_L[527] = {tag = "LONG", payload = "--jobname"}
_L[528] = {_L[527], {tag = "WITH_ARG", payload = _L[428]}}
_L[529] = {tag = "LONG", payload = "-fmt"}
_L[530] = {_L[529], {tag = "WITH_ARG", payload = _L[429]}}
_L[531] = {tag = "LONG", payload = "--fmt"}
_L[532] = {_L[531], {tag = "WITH_ARG", payload = _L[429]}}
_L[533] = {tag = "LONG", payload = "-output-directory"}
_L[534] = {_L[533], {tag = "WITH_ARG", payload = _L[430]}}
_L[535] = {tag = "LONG", payload = "--output-directory"}
_L[536] = {_L[535], {tag = "WITH_ARG", payload = _L[430]}}
_L[537] = {tag = "LONG", payload = "-output-format"}
_L[538] = {_L[537], {tag = "WITH_ARG", payload = _L[431]}}
_L[539] = {tag = "LONG", payload = "--output-format"}
_L[540] = {_L[539], {tag = "WITH_ARG", payload = _L[431]}}
_L[541] = {tag = "LONG", payload = "--tex-option"}
_L[542] = {_L[541], {tag = "WITH_ARG", payload = _L[432]}}
_L[543] = {tag = "LONG", payload = "--tex-options"}
_L[544] = {_L[543], {tag = "WITH_ARG", payload = _L[433]}}
_L[545] = {tag = "LONG", payload = "--dvipdfmx-option"}
_L[546] = {_L[545], {tag = "WITH_ARG", payload = _L[434]}}
_L[547] = {tag = "LONG", payload = "--dvipdfmx-options"}
_L[548] = {_L[547], {tag = "WITH_ARG", payload = _L[435]}}
_L[549] = {tag = "LONG", payload = "--makeindex"}
_L[550] = {_L[549], {tag = "WITH_ARG", payload = _L[436]}}
_L[551] = {tag = "LONG", payload = "--bibtex"}
_L[552] = {_L[551], {tag = "WITH_ARG", payload = _L[437]}}
_L[553] = {tag = "LONG", payload = "--biber"}
_L[554] = {_L[553], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[438], default = "biber"}}}
_L[555] = {tag = "LONG", payload = "--makeglossaries"}
_L[556] = {_L[555], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[439], default = "makeglossaries"}}}
_L[557] = {tag = "LONG", payload = "--config-file"}
_L[558] = {_L[557], {tag = "WITH_ARG", payload = _L[440]}}
_L[559] = {tag = "SHORT", payload = "-o"}
_L[560] = _list({n = 60, _L[442], _L[444], _L[446], _L[448], _L[450], _L[452], _L[454], _L[456], _L[458], _L[460], _L[462], _L[464], _L[466], _L[468], _L[470], _L[472], _L[474], _L[476], _L[478], _L[480], _L[482], _L[484], _L[486], _L[488], _L[490], _L[492], _L[494], _L[496], _L[498], _L[500], _L[502], _L[504], _L[506], _L[508], _L[510], _L[512], _L[514], _L[516], _L[518], _L[520], _L[522], _L[524], _L[526], _L[528], _L[530], _L[532], _L[534], _L[536], _L[538], _L[540], _L[542], _L[544], _L[546], _L[548], _L[550], _L[552], _L[554], _L[556], _L[558], {_L[559], {tag = "WITH_ARG", payload = _L[411]}}})
_L[561] = function(command, recover)
  local status_or_signal, termination, success_or_recovered
  do
    _L[307](command)
    local tmp21, tmp22, tmp23 = execute(command)
    local tmp24 = type_PRIME(tmp21)
    local success
    if tmp24 == "number" then
      local tmp25 = tmp21 == 0
      status_or_signal = tmp21
      termination = {tag = "NONE"}
      success = tmp25
    else
      status_or_signal = tmp23
      termination = {tag = "SOME", payload = tmp22}
      success = tmp21
    end
    if success then
      success_or_recovered = true
    else
      if recover.tag == "SOME" then
      elseif recover.tag == "NONE" then
        success_or_recovered = false
        goto cont
      else
        _raise(_Match, "main.sml:150:54")
      end
      success_or_recovered = recover.payload(nil)
    end
  end
  ::cont::
  if success_or_recovered then
    return nil
  elseif termination.tag == "SOME" and termination.payload == "exit" then
    local tmp21 = tostring(status_or_signal)
    _L[308]("Command exited abnormally: exit status " .. tmp21)
    _raise(_L[406], "main.sml:161:16")
  else
    goto else1
  end
  ::else1::
  if termination.tag == "SOME" and termination.payload == "signal" then
    local tmp21 = tostring(status_or_signal)
    _L[308]("Command exited abnormally: signal " .. tmp21)
    _raise(_L[406], "main.sml:161:16")
  else
    goto else2
  end
  ::else2::
  local tmp21 = tostring(status_or_signal)
  _L[308]("Command exited abnormally: " .. tmp21)
  _raise(_L[406], "main.sml:161:16")
end
_L[562] = {tag = "NO_NEED_TO_RERUN"}
_L[563] = {tag = "NO_PAGES_OF_OUTPUT"}
_L[564] = function(engine_type, executable, name, supports_draftmode, supports_pdf_generation, inputfile, exp, exp1, dvipdfmx_extraoptions, exp2, jobname, exp3, exp4, makeindex, max_iterations, output, output_directory, exp5, epstopdf, minted, pdfx, shell_escape, exp6, start_with_draft, synctex, original_wd, output_extension, recorderfile, recorderfile2, source_date_epoch_info, draftmode, engine_executable, extra_options, file_line_error, fmt1, halt_on_error, interaction, jobname1, lua_initialization_script, output_directory1, output_format, shell_escape1, synctex1)
  local tmp21
  do
    local tmp22, tmp23 = 0, _L[202]
    ::cont1::
    local iteration, exp7
    do
      local iteration1, auxstatus = tmp22, tmp23
      iteration = _Int_add(iteration1, 1)
      local mainauxfile, pdfx1, minted1, filelist, epstopdf1, bibtex_aux_hash, auxstatus1
      do
        local tmp24 = jobname .. ".aux"
        mainauxfile = _L[210].join(output_directory, tmp24)
        local tmp25 = _L[218](recorderfile)
        if tmp25 then
          local recorded, tmp26
          do
            recorded = _L[389](recorderfile, makeindex, nil, _L[202])
            local tmp27 = _L[239](engine_type, _L[237])
            if tmp27 then
            else
              tmp26 = false
              goto cont13
            end
            tmp26 = _L[218](recorderfile2)
          end
          ::cont13::
          local recorded1
          if tmp26 then
            local fileList = recorded[1]
            recorded1 = _L[389](recorderfile2, makeindex, fileList, recorded[2])
          else
            recorded1 = recorded
            goto cont14
          end
          ::cont14::
          local filelist1, auxstatus2, minted2, epstopdf2, pdfx2, bibtex_aux_hash1
          do
            local exp8
            do
              local fileInfo = recorded1[1]
              exp8 = _L[388](fileInfo, recorded1[2])
            end
            filelist1 = exp8[1]
            do
              local go = function(a)
                local abspath = a[1].abspath
                local kind = a[1].kind
                local auxstatus3 = a[2]
                local tmp27 = _L[218](abspath)
                if tmp27 then
                  local auxstatus4, status
                  do
                    local exp9 = _L[204](auxstatus3, abspath)
                    if exp9.tag == "NONE" then
                      local tmp28 = {tag = "NONE"}
                      local tmp29 = {tag = "NONE"}
                      local s = {{md5sum = {tag = "NONE"}, mtime = tmp28, size = tmp29}}
                      local tmp30 = _L[203](auxstatus3, abspath, s)
                      auxstatus4 = tmp30
                      status = s
                    elseif exp9.tag == "SOME" then
                      auxstatus4 = auxstatus3
                      status = exp9.payload
                      goto cont
                    else
                      _raise(_Match, "reruncheck.sml:124:51")
                    end
                  end
                  ::cont::
                  if kind == "INPUT" then
                    if status[1].mtime.tag == "NONE" then
                      local s = status[1]
                      local tmp28 = s.md5sum
                      local tmp29 = s.size
                      local tmp30 = _L[69](abspath)
                      status[1] = {md5sum = tmp28, mtime = {tag = "SOME", payload = tmp30}, size = tmp29}
                      return auxstatus4
                    else
                      return auxstatus4
                    end
                  else
                    goto else1
                  end
                  ::else1::
                  if kind == "AUXILIARY" then
                    local s
                    do
                      local s1 = status[1]
                      if s1.mtime.tag == "NONE" then
                        local tmp28 = s1.md5sum
                        local tmp29 = s1.size
                        local tmp30 = _L[69](abspath)
                        s = {md5sum = tmp28, mtime = {tag = "SOME", payload = tmp30}, size = tmp29}
                      else
                        s = s1
                        goto cont1
                      end
                    end
                    ::cont1::
                    local s1
                    if s.size.tag == "NONE" then
                      local tmp28 = s.md5sum
                      local tmp29 = s.mtime
                      local tmp30 = _L[70](abspath)
                      s1 = {md5sum = tmp28, mtime = tmp29, size = {tag = "SOME", payload = tmp30}}
                    else
                      s1 = s
                      goto cont2
                    end
                    ::cont2::
                    if s1.md5sum.tag == "NONE" then
                      local tmp28 = s1.mtime
                      local tmp29 = s1.size
                      local tmp30
                      do
                        local ins = _L[150](abspath)
                        local tmp31 = _L[152](ins)
                        _L[151](ins)
                        tmp30 = _L[191](tmp31)
                      end
                      status[1] = {md5sum = {tag = "SOME", payload = tmp30}, mtime = tmp28, size = tmp29}
                      return auxstatus4
                    else
                      status[1] = s1
                      return auxstatus4
                    end
                  elseif kind == "OUTPUT" then
                    return auxstatus4
                  else
                    _raise(_Match, "reruncheck.sml:129:24")
                  end
                else
                  return auxstatus3
                end
              end
              local tmp27 = _L[205](function(a)
                return a[1]
              end)
              local tmp28 = foldl(go)
              local tmp29 = _L[205](function(x)
                return {x}
              end)
              local tmp30 = tmp29(auxstatus)
              local tmp31 = tmp28(tmp30)
              local tmp32 = tmp31(filelist1)
              auxstatus2 = tmp27(tmp32)
            end
            local tmp27 = foldl(function(a)
              local path, epstopdf3, pdfx3, tmp28
              do
                path = a[1].path
                local minted3 = a[2].minted
                epstopdf3 = a[2].epstopdf
                pdfx3 = a[2].pdfx
                if minted3 then
                  tmp28 = true
                  goto cont
                end
                local tmp29 = isSuffix("minted/minted.sty")
                local tmp30 = tmp29(path)
                if tmp30 then
                  tmp28 = true
                  goto cont
                end
                local tmp31 = isSuffix("minted\\minted.sty")
                tmp28 = tmp31(path)
              end
              ::cont::
              local tmp29
              do
                if epstopdf3 then
                  tmp29 = true
                  goto cont1
                end
                local tmp30 = isSuffix("epstopdf.sty")
                tmp29 = tmp30(path)
              end
              ::cont1::
              local tmp30
              do
                if pdfx3 then
                  tmp30 = true
                  goto cont2
                end
                local tmp31 = isSuffix("pdfx.sty")
                tmp30 = tmp31(path)
              end
              ::cont2::
              return {epstopdf = tmp29, minted = tmp28, pdfx = tmp30}
            end)
            local tmp28 = tmp27({epstopdf = false, minted = false, pdfx = false})
            local exp9 = tmp28(filelist1)
            minted2 = exp9.minted
            epstopdf2 = exp9.epstopdf
            pdfx2 = exp9.pdfx
            local tmp29 = exp.tag == "SOME" and exp.payload.tag == "BIBTEX"
            if tmp29 then
              local biblines
              do
                local tmp30 = _L[382](mainauxfile, output_directory, nil)
                biblines = revAppend(tmp30, nil)
              end
              if biblines == nil then
                bibtex_aux_hash1 = {tag = "NONE"}
                goto cont15
              end
              local tmp30 = _VectorOrArray_fromList(biblines)
              local tmp31 = table_concat(tmp30, "\n")
              local tmp32 = _L[191](tmp31)
              bibtex_aux_hash1 = {tag = "SOME", payload = tmp32}
            else
              bibtex_aux_hash1 = {tag = "NONE"}
              goto cont15
            end
          end
          ::cont15::
          pdfx1 = pdfx2
          minted1 = minted2
          filelist = filelist1
          epstopdf1 = epstopdf2
          bibtex_aux_hash = bibtex_aux_hash1
          auxstatus1 = auxstatus2
          goto cont3
        else
          goto else6
        end
        ::else6::
        if auxstatus[2].tag == "E" then
          pdfx1 = false
          minted1 = false
          filelist = nil
          epstopdf1 = false
          bibtex_aux_hash = {tag = "NONE"}
          auxstatus1 = _L[202]
          goto cont3
        end
        _L[308]("Recorder file was not generated during the execution!")
        _raise(_L[406], "main.sml:221:27")
      end
      ::cont3::
      if source_date_epoch_info.tag == "NONE" then
        goto cont4
      elseif source_date_epoch_info.tag == "SOME" then
        local r = source_date_epoch_info.payload
        local tmp24 = exp6.tag == "SOME" and exp6.payload.tag == "NOW"
        local should_set_source_date_epoch
        if tmp24 then
          should_set_source_date_epoch = true
        elseif exp6.tag == "SOME" and exp6.payload.tag == "RAW" then
          should_set_source_date_epoch = false
        elseif exp6.tag == "NONE" then
          should_set_source_date_epoch = pdfx1 or pdfx
        else
          _raise(_Match, "main.sml:235:54")
        end
        if should_set_source_date_epoch then
          local time_since_epoch
          do
            local tmp25 = foldl(function(a)
              if a[1].kind == "INPUT" then
                local abspath = a[1].abspath
                local acc = a[2]
                local exp8 = _L[204](auxstatus1, abspath)
                if exp8.tag == "SOME" then
                  local mtime = exp8.payload.mtime
                  if mtime.tag == "SOME" and acc.tag == "SOME" then
                    if acc.payload < mtime.payload then
                      return mtime
                    else
                      return acc
                    end
                  elseif mtime.tag == "NONE" then
                    return acc
                  elseif acc.tag == "NONE" then
                    return mtime
                  else
                    _raise(_Match, "main.sml:243:68")
                  end
                elseif exp8.tag == "NONE" then
                  return acc
                else
                  _raise(_Match, "main.sml:241:59")
                end
              else
                return a[2]
              end
            end)
            local tmp26 = tmp25({tag = "NONE"})
            local input_time = tmp26(filelist)
            if input_time.tag == "SOME" then
              local input_time1 = input_time.payload
              if r[1].time < input_time1 then
                local tmp27 = tostring(time())
                local new_info = {time = input_time1, time_since_epoch = tmp27}
                local tmp28 = _ENV.CLUTTEX_VERBOSITY
                if tmp28 >= 1 then
                else
                  r[1] = new_info
                  time_since_epoch = tmp27
                  goto cont13
                end
                _L[311]("Input file was modified; Updating SOURCE_DATE_EPOCH...")
                r[1] = new_info
                time_since_epoch = tmp27
              else
                time_since_epoch = r[1].time_since_epoch
                goto cont13
              end
            elseif input_time.tag == "NONE" then
              time_since_epoch = r[1].time_since_epoch
              goto cont13
            else
              _raise(_Match, "main.sml:256:37")
            end
          end
          ::cont13::
          do
            local tmp25 = _ENV.CLUTTEX_VERBOSITY
            if tmp25 >= 1 then
            else
              goto cont14
            end
            _L[311]("Setting SOURCE_DATE_EPOCH to " .. time_since_epoch)
          end
          ::cont14::
          _L[221]("SOURCE_DATE_EPOCH", time_since_epoch)
        else
          goto cont4
        end
      else
        _raise(_Match, "main.sml:232:20")
      end
      ::cont4::
      local tex_injection
      do
        local tex_injection1
        if exp2.tag == "SOME" then
          tex_injection1 = "\\includeonly{" .. exp2.payload .. "}"
        elseif exp2.tag == "NONE" then
          tex_injection1 = ""
        else
          _raise(_Match, "main.sml:279:31")
        end
        local tmp24
        tmp24 = minted1 or minted
        local tmp25
        tmp25 = tmp24 and require("kpse").new(name):find_file("minted2.sty") == nil
        if tmp25 then
          if not minted then
          else
            goto cont13
          end
          _L[310]("You may want to use --package-support=minted option.")
          ::cont13::
          local outdir
          if _L[220] then
            local tmp26 = map1(function(c)
              if c == 92 then
                return 47
              else
                return c
              end
            end)
            outdir = tmp26(output_directory)
          else
            outdir = output_directory
            goto cont14
          end
          ::cont14::
          tex_injection = tex_injection1 .. "\\PassOptionsToPackage{outputdir=" .. outdir .. "}{minted}"
        else
          tex_injection = tex_injection1
          goto cont5
        end
      end
      ::cont5::
      local tex_injection1
      do
        local tmp24
        tmp24 = epstopdf1 or epstopdf
        if tmp24 then
          if not epstopdf then
          else
            goto cont13
          end
          _L[310]("You may want to use --package-support=epstopdf option.")
          ::cont13::
          local outdir
          if _L[220] then
            local tmp25 = map1(function(c)
              if c == 92 then
                return 47
              else
                return c
              end
            end)
            outdir = tmp25(output_directory)
          else
            outdir = output_directory
            goto cont14
          end
          ::cont14::
          local tmp25 = isSuffix("/")
          local tmp26 = tmp25(outdir)
          local outdir1
          if tmp26 then
            outdir1 = outdir
          else
            outdir1 = outdir .. "/"
          end
          tex_injection1 = tex_injection .. "\\PassOptionsToPackage{outdir=" .. outdir1 .. "}{epstopdf}"
        else
          tex_injection1 = tex_injection
          goto cont6
        end
      end
      ::cont6::
      local lightweight_mode, command
      do
        local tmp24 = _L[239](engine_type, _L[235])
        local tmp25
        do
          local escaped
          do
            local tmp26 = translate(_L[234])
            local tmp27 = tmp26(inputfile)
            local tmp28, tmp29
            do
              local tmp30 = full(tmp27)
              tmp29, tmp28 = tmp30, nil
            end
            ::cont21::
            local acc, s
            do
              s, acc = tmp29, tmp28
              local tmp30 = isEmpty(s)
              if tmp30 then
                local tmp31 = revAppend(acc, nil)
                escaped = concat1(tmp31)
                goto cont19
              else
                goto else7
              end
            end
            ::else7::
            local a, d, tmp30, tmp31
            do
              local tmp32 = splitl(function(c)
                return c ~= 32
              end)
              local exp8 = tmp32(s)
              a = exp8[1]
              local b = exp8[2]
              local tmp33 = splitl(function(c)
                return c == 32
              end)
              local exp9 = tmp33(b)
              local c = exp9[1]
              d = exp9[2]
              tmp30 = map(str)
              local base, start, tmp34, tmp35
              do
                base = c.base
                local length = c.length
                start = c.start
                tmp35, tmp34 = _Int_sub(_Int_add(start, length), 1), nil
              end
              ::cont23::
              local i, acc1 = tmp35, tmp34
              if i < start then
                tmp31 = acc1
                goto cont22
              end
              local tmp36 = _Int_sub(i, 1)
              local tmp37 = sub({base, i})
              tmp35 = tmp36
              tmp34 = {tmp37, acc1}
              goto cont23
            end
            ::cont22::
            local tmp32 = tmp30(tmp31)
            local tmp33 = _VectorOrArray_fromList(tmp32)
            local tmp34 = table_concat(tmp33, "~")
            local c_PRIME = full(tmp34)
            tmp29 = d
            tmp28 = {c_PRIME, {a, acc}}
            goto cont21
          end
          ::cont19::
          local escaped1
          if tmp24 then
            local tmp26, tmp27
            do
              local tmp28 = full(escaped)
              tmp27, tmp26 = tmp28, nil
            end
            ::cont21::
            local acc, s
            do
              s, acc = tmp27, tmp26
              local tmp28 = isEmpty(s)
              if tmp28 then
                local tmp29 = revAppend(acc, nil)
                escaped1 = concat1(tmp29)
                goto cont20
              else
                goto else7
              end
            end
            ::else7::
            local a, d, c_PRIME
            do
              local tmp28 = splitl(isAscii)
              local exp8 = tmp28(s)
              a = exp8[1]
              local b = exp8[2]
              local tmp29 = splitl(function(x)
                return not (x <= 127)
              end)
              local exp9 = tmp29(b)
              local c = exp9[1]
              d = exp9[2]
              local tmp30 = isEmpty(c)
              if tmp30 then
                c_PRIME = c
                goto cont22
              end
              local tmp31 = string1(c)
              c_PRIME = full("\\detokenize{" .. tmp31 .. "}")
            end
            ::cont22::
            tmp27 = d
            tmp26 = {c_PRIME, {a, acc}}
            goto cont21
          else
            escaped1 = escaped
            goto cont20
          end
          ::cont20::
          if inputfile == escaped1 then
            tmp25 = "\\input\"" .. inputfile .. "\""
          else
            tmp25 = "\\begingroup\\escapechar-1\\let~\\string\\edef\\x{\"" .. escaped1 .. "\" }\\expandafter\\endgroup\\expandafter\\input\\x"
          end
        end
        local inputline = tex_injection1 .. tmp25
        local tmp26 = iteration == 1 and start_with_draft
        local draftmode1, engine_executable1, extra_options1, exp8, exp9, exp10, exp11, exp12, exp13, exp14, exp15, exp16, exp17
        if tmp26 then
          if supports_draftmode then
            lightweight_mode = true
            exp17 = synctex1
            exp16 = shell_escape1
            exp15 = output_format
            exp14 = output_directory1
            exp13 = lua_initialization_script
            exp12 = jobname1
            exp11 = {tag = "SOME", payload = _L[222]}
            exp10 = halt_on_error
            exp9 = fmt1
            exp8 = file_line_error
            extra_options1 = extra_options
            engine_executable1 = engine_executable
            draftmode1 = true
          else
            lightweight_mode = true
            exp17 = synctex1
            exp16 = shell_escape1
            exp15 = output_format
            exp14 = output_directory1
            exp13 = lua_initialization_script
            exp12 = jobname1
            exp11 = {tag = "SOME", payload = _L[222]}
            exp10 = halt_on_error
            exp9 = fmt1
            exp8 = file_line_error
            extra_options1 = extra_options
            engine_executable1 = engine_executable
            draftmode1 = draftmode
          end
        else
          lightweight_mode = false
          exp17 = synctex1
          exp16 = shell_escape1
          exp15 = output_format
          exp14 = output_directory1
          exp13 = lua_initialization_script
          exp12 = jobname1
          exp11 = interaction
          exp10 = halt_on_error
          exp9 = fmt1
          exp8 = file_line_error
          extra_options1 = extra_options
          engine_executable1 = engine_executable
          draftmode1 = false
        end
        local revCommand
        do
          local executable1 = getOpt(engine_executable1, executable)
          local revCommand1 = {"-recorder", {executable1, nil}}
          if exp9.tag == "NONE" then
            revCommand = revCommand1
            goto cont13
          elseif exp9.tag == "SOME" then
            local tmp27 = _L[207](exp9.payload)
            revCommand = {"-fmt=" .. tmp27, revCommand1}
          else
            _raise(_Match, "tex-engine.sml:58:28")
          end
        end
        ::cont13::
        local revCommand1
        do
          local revCommand2
          if not exp10 then
            revCommand2 = revCommand
          elseif exp10 then
            revCommand2 = {"-halt-on-error", revCommand}
          else
            _raise(_Match, "tex-engine.sml:61:28")
          end
          if exp11.tag == "NONE" then
            revCommand1 = revCommand2
            goto cont14
          elseif exp11.tag == "SOME" then
            local mode = exp11.payload
            local tmp27
            if mode == "BATCHMODE" then
              tmp27 = "batchmode"
            elseif mode == "NONSTOPMODE" then
              tmp27 = "nonstopmode"
            elseif mode == "SCROLLMODE" then
              tmp27 = "scrollmode"
            elseif mode == "ERRORSTOPMODE" then
              tmp27 = "errorstopmode"
            else
              _raise(_Match, "types.sml:14:7")
            end
            revCommand1 = {"-interaction=" .. tmp27, revCommand2}
          else
            _raise(_Match, "tex-engine.sml:64:28")
          end
        end
        ::cont14::
        local revCommand2
        do
          local revCommand3
          if not exp8 then
            revCommand3 = revCommand1
          elseif exp8 then
            revCommand3 = {"-file-line-error", revCommand1}
          else
            _raise(_Match, "tex-engine.sml:67:28")
          end
          if exp17.tag == "NONE" then
            revCommand2 = revCommand3
            goto cont15
          elseif exp17.tag == "SOME" then
            local tmp27 = _L[207](exp17.payload)
            revCommand2 = {"-synctex=" .. tmp27, revCommand3}
          else
            _raise(_Match, "tex-engine.sml:70:28")
          end
        end
        ::cont15::
        local revCommand3
        do
          local revCommand4
          if exp16.tag == "NONE" then
            revCommand4 = revCommand2
          elseif exp16.tag == "SOME" and exp16.payload == "FORBIDDEN" then
            revCommand4 = {"-no-shell-escape", revCommand2}
          elseif exp16.tag == "SOME" and exp16.payload == "RESTRICTED" then
            revCommand4 = {"-shell-restricted", revCommand2}
          elseif exp16.tag == "SOME" and exp16.payload == "ALLOWED" then
            revCommand4 = {"-shell-escape", revCommand2}
          else
            _raise(_Match, "tex-engine.sml:73:28")
          end
          if exp12.tag == "NONE" then
            revCommand3 = revCommand4
            goto cont16
          elseif exp12.tag == "SOME" then
            local tmp27 = _L[207](exp12.payload)
            revCommand3 = {"-jobname=" .. tmp27, revCommand4}
          else
            _raise(_Match, "tex-engine.sml:78:28")
          end
        end
        ::cont16::
        local revCommand4
        if exp14.tag == "NONE" then
          revCommand4 = revCommand3
          goto cont17
        elseif exp14.tag == "SOME" then
          local tmp27 = _L[207](exp14.payload)
          revCommand4 = {"-output-directory=" .. tmp27, revCommand3}
        else
          _raise(_Match, "tex-engine.sml:81:28")
        end
        ::cont17::
        local revCommand5
        if engine_type == "OTHER" then
          revCommand5 = revCommand4
          goto cont18
        elseif engine_type == "PDFTEX" then
          local revCommand6
          if draftmode1 then
            revCommand6 = {"-draftmode", revCommand4}
          else
            revCommand6 = revCommand4
          end
          if exp15 == "DVI" then
            revCommand5 = {"-output-format=dvi", revCommand6}
            goto cont18
          elseif exp15 == "PDF" then
            revCommand5 = revCommand6
            goto cont18
          else
            _raise(_Match, "tex-engine.sml:90:45")
          end
        elseif engine_type == "XETEX" then
          local tmp27
          if draftmode1 then
            tmp27 = true
          else
            tmp27 = _L[232](exp15, _L[231])
          end
          if tmp27 then
            revCommand5 = {"-no-pdf", revCommand4}
            goto cont18
          else
            revCommand5 = revCommand4
            goto cont18
          end
        else
          goto else6
        end
        ::else6::
        if engine_type == "LUATEX" then
          local revCommand6
          if exp13.tag == "NONE" then
            revCommand6 = revCommand4
            goto cont19
          elseif exp13.tag == "SOME" then
            local tmp27 = _L[207](exp13.payload)
            revCommand6 = {"--lua=" .. tmp27, revCommand4}
          else
            _raise(_Match, "tex-engine.sml:98:63")
          end
          ::cont19::
          local revCommand7
          if draftmode1 then
            revCommand7 = {"--draftmode", revCommand6}
          else
            revCommand7 = revCommand6
          end
          if exp15 == "DVI" then
            revCommand5 = {"--output-format=dvi", revCommand7}
          elseif exp15 == "PDF" then
            revCommand5 = revCommand7
          else
            _raise(_Match, "tex-engine.sml:105:45")
          end
        else
          _raise(_Match, "tex-engine.sml:84:28")
        end
        ::cont18::
        local revCommand6 = revAppend(extra_options1, revCommand5)
        local tmp27 = _L[207](inputline)
        local tmp28 = revAppend({tmp27, revCommand6}, nil)
        local tmp29 = _VectorOrArray_fromList(tmp28)
        local result = table_concat(tmp29, " ")
        command = result
      end
      local getExecLog, recorded, tmp24
      do
        local execlogCache = {{tag = "NONE"}}
        getExecLog = function()
          local exp8 = execlogCache[1]
          if exp8.tag == "NONE" then
            local tmp25 = jobname .. ".log"
            local tmp26 = _L[210].join(output_directory, tmp25)
            local ins = _L[114](tmp26)
            local log = _L[122](ins)
            _L[119](ins)
            execlogCache[1] = {tag = "SOME", payload = log}
            return log
          elseif exp8.tag == "SOME" then
            return exp8.payload
          else
            _raise(_Match, "main.sml:330:31")
          end
        end
        local recovered = {false}
        _L[561](command, {tag = "SOME", payload = function(a)
          local execlog = getExecLog()
          local tmp25 = jobname .. ".aux"
          local tmp26 = _L[210].join(output_directory, tmp25)
          local r
          do
            local recovered1
            do
              local tmp27 = _L[51]("I can't write on file")
              local tmp28 = tmp27(execlog)
              if tmp28 then
                local exp8 = _L[381](tmp26, output_directory, _L[197])
                local madeNewDirectory = exp8[1]
                local tmp29
                if madeNewDirectory then
                  local tmp30 = _ENV.CLUTTEX_VERBOSITY
                  tmp29 = tmp30 >= 1
                else
                  tmp29 = false
                end
                if tmp29 then
                else
                  recovered1 = madeNewDirectory
                  goto cont
                end
                _L[311]("Created missing directories.")
                recovered1 = madeNewDirectory
              else
                recovered1 = false
                goto cont
              end
            end
            ::cont::
            local tmp27
            do
              local tmp28 = tmp(_L[229])
              local tmp29
              do
                local tmp30 = tmp28(shell_escape, {tag = "SOME", payload = _L[228]})
                tmp29 = not tmp30
              end
              if tmp29 then
                local tmp30 = tokens(function(c)
                  return c == 10
                end)
                local tmp31 = full(execlog)
                local lines = tmp30(tmp31)
                local tmp32 = foldl(function(a1)
                  local line = a1[1]
                  local run = a1[2]
                  local tmp33 = map(string1)
                  local tmp34 = tokens(isSpace)
                  local tmp35 = tmp34(line)
                  local exp8 = tmp33(tmp35)
                  if exp8 ~= nil and (exp8[1] == "(epstopdf)" and (exp8[2] ~= nil and (exp8[2][1] == "Command:" and (exp8[2][2] ~= nil and (exp8[2][2][2] ~= nil and (exp8[2][2][2][2] ~= nil and exp8[2][2][2][2][2] == nil)))))) then
                    local tmp36, tmp37, tmp38
                    do
                      local tmp39 = exp8[2][2][1]
                      tmp36 = exp8[2][2][2][1]
                      tmp37 = exp8[2][2][2][2][1]
                      local tmp40 = tmp39 == "<epstopdf" or tmp39 == "<repstopdf"
                      if tmp40 then
                        local tmp41 = isPrefix("--outfile=")
                        local tmp42 = tmp41(tmp36)
                        if tmp42 then
                          local tmp43 = isSuffix(">")
                          tmp38 = tmp43(tmp37)
                        else
                          tmp38 = false
                          goto cont
                        end
                      else
                        tmp38 = false
                        goto cont
                      end
                    end
                    ::cont::
                    if tmp38 then
                      local outfile = extract(tmp36, 10, {tag = "NONE"})
                      local infile = substring(tmp37, 0, _Int_sub(#tmp37, 1))
                      local infileAbs = _L[215]({tag = "SOME", payload = original_wd}, infile)
                      local tmp39 = _L[218](infileAbs)
                      if tmp39 then
                        local outfileAbs
                        do
                          outfileAbs = _L[215]({tag = "SOME", payload = output_directory}, outfile)
                          local tmp40 = _ENV.CLUTTEX_VERBOSITY
                          if tmp40 >= 1 then
                          else
                            goto cont1
                          end
                          _L[311]("Running epstopdf on " .. infile .. ".")
                        end
                        ::cont1::
                        do
                          local outdir = _L[212](outfileAbs)
                          local tmp40 = _L[219](outdir)
                          if not tmp40 then
                            local tmp41, tmp42 = _L[217].mkdir_rec(outdir)
                            if not tmp41 then
                              local tmp43 = _id(tmp42)
                              _raise(tmp43, "fs-util.sml:16:28")
                            else
                              goto cont2
                            end
                          else
                            goto cont2
                          end
                        end
                        ::cont2::
                        local tmp40 = _L[207](outfileAbs)
                        local tmp41 = "epstopdf --outfile=" .. tmp40 .. " "
                        local tmp42 = _L[207](infileAbs)
                        local command1 = tmp41 .. tmp42
                        _L[307](command1)
                        execute(command1)
                        return run
                      else
                        return run
                      end
                    else
                      return run
                    end
                  else
                    return run
                  end
                end)
                local tmp33 = tmp32(false)
                tmp27 = tmp33(lines)
              else
                tmp27 = false
                goto cont1
              end
            end
            ::cont1::
            local recovered2
            recovered2 = tmp27 or recovered1
            if recovered2 then
              recovered[1] = true
              return true
            end
            local tmp28 = _L[51]("Package minted Error: Missing Pygments output; \\inputminted was")
            r = tmp28(execlog)
          end
          recovered[1] = true
          return r
        end})
        if recovered[1] then
          exp7 = {tag = "SHOULD_RERUN", payload = _L[202]}
          goto cont2
        end
        recorded = _L[389](recorderfile, makeindex, nil, _L[202])
        local tmp25 = _L[239](engine_type, _L[237])
        if tmp25 then
        else
          tmp24 = false
          goto cont7
        end
        tmp24 = _L[218](recorderfile2)
      end
      ::cont7::
      local recorded1
      if tmp24 then
        local fileList = recorded[1]
        recorded1 = _L[389](recorderfile2, makeindex, fileList, recorded[2])
      else
        recorded1 = recorded
        goto cont8
      end
      ::cont8::
      local filelist1, execlog
      do
        local exp8
        do
          local fileInfo = recorded1[1]
          exp8 = _L[388](fileInfo, recorded1[2])
        end
        filelist1 = exp8[1]
        execlog = getExecLog()
        if exp1.tag == "NONE" then
          goto cont9
        elseif exp1.tag == "SOME" then
          do
            local driver = exp1.payload
            local tmp25 = map(function(a)
              local path = a.path
              local kind = a.kind
              if kind == "INPUT" then
                return {kind = "input", path = path}
              elseif kind == "OUTPUT" then
                return {kind = "output", path = path}
              elseif kind == "AUXILIARY" then
                return {kind = "auxiliary", path = path}
              else
                _raise(_Match, "main.sml:359:141")
              end
            end)
            local tmp26 = tmp25(filelist1)
            do
              local tmp27 = _ENV.CLUTTEX_VERBOSITY
              if tmp27 >= 1 then
                if driver == "DVIPDFMX" then
                else
                  goto else6
                end
                _L[311]("checkdriver: expects dvipdfmx")
                goto cont14
                ::else6::
                if driver == "DVIPS" then
                else
                  goto else7
                end
                _L[311]("checkdriver: expects dvips")
                goto cont14
                ::else7::
                if driver == "DVISVGM" then
                else
                  goto else8
                end
                _L[311]("checkdriver: expects dvisvgm")
                goto cont14
                ::else8::
                if driver == "PDFTEX" then
                else
                  goto else9
                end
                _L[311]("checkdriver: expects pdftex")
                goto cont14
                ::else9::
                if driver == "XETEX" then
                else
                  goto else10
                end
                _L[311]("checkdriver: expects xetex")
                goto cont14
                ::else10::
                if driver == "LUATEX" then
                else
                  _raise(_Match, "check-driver.sml:8:7")
                end
                _L[311]("checkdriver: expects luatex")
              else
                goto cont14
              end
            end
            ::cont14::
            local loadedSet, graphics_driver
            do
              local tmp27 = foldl(function(a)
                local kind = a[1].kind
                local path = a[1].path
                local set = a[2]
                if kind == "input" then
                  local tmp28 = _L[211](path)
                  return _L[198](set, tmp28)
                else
                  return set
                end
              end)
              local tmp28 = tmp27(_L[197])
              loadedSet = tmp28(tmp26)
              local tmp29 = _L[199](loadedSet, "graphics.sty")
              local tmp30
              if tmp29 then
                tmp30 = true
              else
                tmp30 = _L[199](loadedSet, "color.sty")
              end
              if tmp30 then
                local tmp31 = _L[199](loadedSet, "dvipdfmx.def")
                if tmp31 then
                  graphics_driver = {tag = "SOME", payload = _L[318]}
                  goto cont15
                end
                local tmp32 = _L[199](loadedSet, "dvips.def")
                if tmp32 then
                  graphics_driver = {tag = "SOME", payload = _L[319]}
                  goto cont15
                end
                local tmp33 = _L[199](loadedSet, "dvisvgm.def")
                if tmp33 then
                  graphics_driver = {tag = "SOME", payload = _L[320]}
                  goto cont15
                end
                local tmp34 = _L[199](loadedSet, "pdftex.def")
                if tmp34 then
                  graphics_driver = {tag = "SOME", payload = _L[321]}
                  goto cont15
                end
                local tmp35 = _L[199](loadedSet, "luatex.def")
                if tmp35 then
                  graphics_driver = {tag = "SOME", payload = _L[323]}
                  goto cont15
                end
                local tmp36 = _L[199](loadedSet, "xetex.def")
                if tmp36 then
                  graphics_driver = {tag = "SOME", payload = _L[322]}
                else
                  graphics_driver = {tag = "SOME", payload = _L[324]}
                end
              else
                graphics_driver = {tag = "NONE"}
                goto cont15
              end
            end
            ::cont15::
            local tmp27
            do
              local tmp28 = _L[199](loadedSet, "expl3-code.tex")
              if tmp28 then
                tmp27 = true
                goto cont16
              end
              local tmp29 = _L[199](loadedSet, "expl3.sty")
              if tmp29 then
                tmp27 = true
                goto cont16
              end
              local tmp30 = _L[199](loadedSet, "l3backend-dvips.def")
              if tmp30 then
                tmp27 = true
                goto cont16
              end
              local tmp31 = _L[199](loadedSet, "l3backend-dvipdfmx.def")
              if tmp31 then
                tmp27 = true
                goto cont16
              end
              local tmp32 = _L[199](loadedSet, "l3backend-xdvipdfmx.def")
              if tmp32 then
                tmp27 = true
                goto cont16
              end
              local tmp33 = _L[199](loadedSet, "l3backend-pdfmode.def")
              if tmp33 then
                tmp27 = true
                goto cont16
              end
              local tmp34 = _L[199](loadedSet, "l3backend-pdftex.def")
              if tmp34 then
                tmp27 = true
                goto cont16
              end
              local tmp35 = _L[199](loadedSet, "l3backend-luatex.def")
              if tmp35 then
                tmp27 = true
              else
                tmp27 = _L[199](loadedSet, "l3backend-xetex.def")
              end
            end
            ::cont16::
            local expl3_driver
            if tmp27 then
              local tmp28 = _L[199](loadedSet, "l3backend-pdfmode.def")
              if tmp28 then
                expl3_driver = {tag = "SOME", payload = _L[326]}
                goto cont17
              end
              local tmp29 = _L[199](loadedSet, "l3backend-dvisvgm.def")
              if tmp29 then
                expl3_driver = {tag = "SOME", payload = _L[327]}
                goto cont17
              end
              local tmp30 = _L[199](loadedSet, "l3backend-xdvipdfmx.def")
              if tmp30 then
                expl3_driver = {tag = "SOME", payload = _L[328]}
                goto cont17
              end
              local tmp31 = _L[199](loadedSet, "l3backend-dvipdfmx.def")
              if tmp31 then
                expl3_driver = {tag = "SOME", payload = _L[329]}
                goto cont17
              end
              local tmp32 = _L[199](loadedSet, "l3backend-dvips.def")
              if tmp32 then
                expl3_driver = {tag = "SOME", payload = _L[330]}
                goto cont17
              end
              local tmp33 = _L[199](loadedSet, "l3backend-pdftex.def")
              if tmp33 then
                expl3_driver = {tag = "SOME", payload = _L[331]}
                goto cont17
              end
              local tmp34 = _L[199](loadedSet, "l3backend-luatex.def")
              if tmp34 then
                expl3_driver = {tag = "SOME", payload = _L[332]}
                goto cont17
              end
              local tmp35 = _L[199](loadedSet, "l3backend-xetex.def")
              if tmp35 then
                expl3_driver = {tag = "SOME", payload = _L[333]}
              else
                expl3_driver = {tag = "SOME", payload = _L[334]}
              end
            else
              expl3_driver = {tag = "NONE"}
              goto cont17
            end
            ::cont17::
            local hyperref_driver
            do
              local tmp28 = _L[199](loadedSet, "hyperref.sty")
              if tmp28 then
                local tmp29 = _L[199](loadedSet, "hluatex.def")
                if tmp29 then
                  hyperref_driver = {tag = "SOME", payload = _L[340]}
                  goto cont18
                end
                local tmp30 = _L[199](loadedSet, "hpdftex.def")
                if tmp30 then
                  hyperref_driver = {tag = "SOME", payload = _L[339]}
                  goto cont18
                end
                local tmp31 = _L[199](loadedSet, "hxetex.def")
                if tmp31 then
                  hyperref_driver = {tag = "SOME", payload = _L[341]}
                  goto cont18
                end
                local tmp32 = _L[199](loadedSet, "hdvipdfm.def")
                if tmp32 then
                  hyperref_driver = {tag = "SOME", payload = _L[337]}
                  goto cont18
                end
                local tmp33 = _L[199](loadedSet, "hdvips.def")
                if tmp33 then
                  hyperref_driver = {tag = "SOME", payload = _L[338]}
                else
                  hyperref_driver = {tag = "SOME", payload = _L[342]}
                end
              else
                hyperref_driver = {tag = "NONE"}
                goto cont18
              end
            end
            ::cont18::
            local xypic_driver
            do
              local tmp28 = _L[199](loadedSet, "xy.tex")
              if tmp28 then
                local tmp29 = _L[199](loadedSet, "xypdf.tex")
                if tmp29 then
                  xypic_driver = {tag = "SOME", payload = _L[344]}
                  goto cont19
                end
                local tmp30 = _L[199](loadedSet, "xydvips.tex")
                if tmp30 then
                  xypic_driver = {tag = "SOME", payload = _L[345]}
                else
                  xypic_driver = {tag = "SOME", payload = _L[346]}
                end
              else
                xypic_driver = {tag = "NONE"}
                goto cont19
              end
            end
            ::cont19::
            do
              local tmp28 = _ENV.CLUTTEX_VERBOSITY
              if tmp28 >= 1 then
                do
                  local tmp29
                  if graphics_driver.tag == "NONE" then
                    tmp29 = "not loaded"
                  else
                    if graphics_driver.tag == "SOME" then
                    else
                      _raise(_Match, "check-driver.sml:204:17")
                    end
                    tmp29 = _L[325](graphics_driver.payload)
                  end
                  _L[311]("checkdriver: graphics=" .. tmp29)
                  local tmp30
                  if expl3_driver.tag == "NONE" then
                    tmp30 = "not loaded"
                  else
                    if expl3_driver.tag == "SOME" then
                    else
                      _raise(_Match, "check-driver.sml:210:17")
                    end
                    tmp30 = _L[336](expl3_driver.payload)
                  end
                  _L[311]("checkdriver: expl3=" .. tmp30)
                  local tmp31
                  if hyperref_driver.tag == "NONE" then
                    tmp31 = "not loaded"
                  else
                    if hyperref_driver.tag == "SOME" then
                    else
                      _raise(_Match, "check-driver.sml:216:17")
                    end
                    tmp31 = _L[343](hyperref_driver.payload)
                  end
                  _L[311]("checkdriver: hyperref=" .. tmp31)
                  if xypic_driver.tag == "NONE" then
                  else
                    goto else6
                  end
                  _L[311]("checkdriver: xypic=not loaded")
                  goto cont20
                end
                ::else6::
                if xypic_driver.tag == "SOME" then
                  local d = xypic_driver.payload
                  if d == "PDF" then
                  else
                    goto else7
                  end
                  _L[311]("checkdriver: xypic=pdf")
                  goto cont20
                  ::else7::
                  if d == "DVIPS" then
                  else
                    goto else8
                  end
                  _L[311]("checkdriver: xypic=dvips")
                  goto cont20
                  ::else8::
                  if d == "UNKNOWN" then
                  else
                    _raise(_Match, "check-driver.sml:67:9")
                  end
                  _L[311]("checkdriver: xypic=unknown")
                else
                  _raise(_Match, "check-driver.sml:222:17")
                end
              else
                goto cont20
              end
            end
            ::cont20::
            local expected_xypic, expected_hyperref, expected_expl3_old, expected_expl3_new
            do
              local expected_graphics
              if driver == "DVIPS" then
                local tmp28 = {tag = "SOME", payload = _L[338]}
                expected_xypic = {tag = "SOME", payload = _L[345]}
                expected_hyperref = tmp28
                expected_graphics = _L[319]
                expected_expl3_old = _L[330]
                expected_expl3_new = _L[330]
              elseif driver == "DVIPDFMX" then
                local tmp28 = {tag = "SOME", payload = _L[337]}
                expected_xypic = {tag = "SOME", payload = _L[344]}
                expected_hyperref = tmp28
                expected_graphics = _L[318]
                expected_expl3_old = _L[329]
                expected_expl3_new = _L[329]
              elseif driver == "DVISVGM" then
                local tmp28 = {tag = "NONE"}
                expected_xypic = {tag = "NONE"}
                expected_hyperref = tmp28
                expected_graphics = _L[320]
                expected_expl3_old = _L[327]
                expected_expl3_new = _L[327]
              elseif driver == "XETEX" then
                local tmp28 = {tag = "SOME", payload = _L[341]}
                expected_xypic = {tag = "SOME", payload = _L[344]}
                expected_hyperref = tmp28
                expected_graphics = _L[322]
                expected_expl3_old = _L[328]
                expected_expl3_new = _L[333]
              elseif driver == "PDFTEX" then
                local tmp28 = {tag = "SOME", payload = _L[339]}
                expected_xypic = {tag = "SOME", payload = _L[344]}
                expected_hyperref = tmp28
                expected_graphics = _L[321]
                expected_expl3_old = _L[326]
                expected_expl3_new = _L[331]
              elseif driver == "LUATEX" then
                local tmp28 = {tag = "SOME", payload = _L[340]}
                expected_xypic = {tag = "SOME", payload = _L[344]}
                expected_hyperref = tmp28
                expected_graphics = _L[323]
                expected_expl3_old = _L[326]
                expected_expl3_new = _L[332]
              else
                _raise(_Match, "check-driver.sml:72:7")
              end
              if graphics_driver.tag == "NONE" then
                goto cont21
              elseif graphics_driver.tag == "SOME" then
                local d = graphics_driver.payload
                local tmp28
                tmp28 = not (d == "DVIPDFMX" and expected_graphics == "DVIPDFMX") and (not (d == "DVIPS" and expected_graphics == "DVIPS") and (not (d == "DVISVGM" and expected_graphics == "DVISVGM") and (not (d == "PDFTEX" and expected_graphics == "PDFTEX") and (not (d == "XETEX" and expected_graphics == "XETEX") and (not (d == "LUATEX" and expected_graphics == "LUATEX") and (d ~= "UNKNOWN" or expected_graphics ~= "UNKNOWN"))))))
                if tmp28 then
                  _L[310]("The driver option for grahipcs(x)/color is missing or wrong.")
                  local tmp29 = _L[325](expected_graphics)
                  _L[310]("Consider setting '" .. tmp29 .. "' option.")
                else
                  goto cont21
                end
              else
                _raise(_Match, "check-driver.sml:236:7")
              end
            end
            ::cont21::
            if expl3_driver.tag == "NONE" then
              goto cont22
            elseif expl3_driver.tag == "SOME" then
              local tmp28
              do
                local d = expl3_driver.payload
                local tmp29
                do
                  local tmp30 = _L[335](d, expected_expl3_old)
                  tmp29 = not tmp30
                end
                if tmp29 then
                  local tmp30 = _L[335](d, expected_expl3_new)
                  tmp28 = not tmp30
                else
                  tmp28 = false
                  goto cont24
                end
              end
              ::cont24::
              if tmp28 then
                _L[310]("The driver option for expl3 is missing or wrong.")
                local tmp29 = _L[336](expected_expl3_new)
                _L[310]("Consider setting 'driver=" .. tmp29 .. "' option when loading expl3.")
                local tmp30 = _L[335](expected_expl3_old, expected_expl3_new)
                if not tmp30 then
                  local tmp31 = _L[336](expected_expl3_old)
                  _L[310]("You might need to instead set 'driver=" .. tmp31 .. "' if you are using an older version of expl3.")
                else
                  goto cont22
                end
              else
                goto cont22
              end
            else
              _raise(_Match, "check-driver.sml:248:7")
            end
            ::cont22::
            do
              local tmp28 = hyperref_driver.tag == "SOME" and expected_hyperref.tag == "SOME"
              if tmp28 then
                local actual = hyperref_driver.payload
                local expected = expected_hyperref.payload
                local tmp29
                tmp29 = not (actual == "DVIPDFMX" and expected == "DVIPDFMX") and (not (actual == "DVIPS" and expected == "DVIPS") and (not (actual == "PDFTEX" and expected == "PDFTEX") and (not (actual == "LUATEX" and expected == "LUATEX") and (not (actual == "XETEX" and expected == "XETEX") and (actual ~= "UNKNOWN" or expected ~= "UNKNOWN")))))
                if tmp29 then
                  _L[310]("The driver option for hyperref is missing or wrong.")
                  local tmp30 = _L[343](expected)
                  _L[310]("Consider setting '" .. tmp30 .. "' option.")
                else
                  goto cont23
                end
              else
                goto cont23
              end
            end
            ::cont23::
            if xypic_driver.tag == "SOME" and expected_xypic.tag == "SOME" then
              local actual = xypic_driver.payload
              local expected = expected_xypic.payload
              local tmp28
              tmp28 = not (actual == "PDF" and expected == "PDF") and (not (actual == "DVIPS" and expected == "DVIPS") and (actual ~= "UNKNOWN" or expected ~= "UNKNOWN"))
              if tmp28 then
                _L[310]("The driver option for Xy-pic is missing or wrong.")
                if driver == "DVIPDFMX" then
                else
                  if driver == "PDFTEX" then
                  else
                    if expected == "PDF" then
                    else
                      if expected == "DVIPS" then
                      elseif expected == "UNKNOWN" then
                        goto cont9
                      else
                        _raise(_Match, "check-driver.sml:280:15")
                      end
                      _L[310]("Consider setting 'dvips' option.")
                      goto cont13
                    end
                    _L[310]("Consider setting 'pdf' package option or running \\xyoption{pdf}.")
                    goto cont13
                  end
                  _L[310]("Consider setting 'pdftex' option or running \\xyoption{pdf}.")
                  goto cont13
                end
                _L[310]("Consider setting 'dvipdfmx' option or running \\xyoption{pdf}.")
              else
                goto cont9
              end
            else
              goto cont9
            end
          end
          ::cont13::
        else
          _raise(_Match, "main.sml:357:27")
        end
      end
      ::cont9::
      local filelist2
      if makeindex.tag == "NONE" then
      else
        goto else2
      end
      if not find1(execlog, "No file [^\n]+%.ind%.") then
        filelist2 = filelist1
        goto cont10
      end
      _L[310]("You may want to use --makeindex option.")
      filelist2 = filelist1
      goto cont10
      ::else2::
      if makeindex.tag == "SOME" then
        local makeindex1 = makeindex.payload
        local tmp25 = foldl(function(a)
          local file = a[1]
          local filelist_acc = a[2]
          local tmp26 = _L[214](file.path)
          if tmp26 == "idx" then
            local output_ind
            do
              local idxfileinfo = {abspath = file.abspath, kind = _L[386], path = file.path}
              local tmp27 = file.abspath
              output_ind = _L[210].replaceext(tmp27, "ind")
              local tmp28 = _L[390]({idxfileinfo, nil}, auxstatus1)
              local tmp29
              if tmp28[1] then
                tmp29 = true
              else
                tmp29 = _L[391](auxstatus1, output_ind, file.abspath)
              end
              if tmp29 then
                local idx_dir = _L[212](file.abspath)
                local tmp30 = _L[207](idx_dir)
                local tmp31 = _L[211](output_ind)
                local tmp32 = _L[211](file.abspath)
                local tmp33 = {n = 7, _L[209], tmp30, "&&", makeindex1, "-o", tmp31, tmp32}
                local tmp34 = table_concat(tmp33, " ")
                _L[561](tmp34, {tag = "NONE"})
                return {{abspath = output_ind, kind = _L[386], path = output_ind}, filelist_acc}
              else
                goto else1
              end
            end
            ::else1::
            do
              local status, exn = pcall(function()
                local tmp27, tmp28 = _L[216].touch(output_ind)
                if not tmp27 then
                  local tmp29 = _id(tmp28)
                  _raise(tmp29, "fs-util.sml:28:25")
                else
                  return nil
                end
              end)
              if not status then
                local tmp27 = _isError(exn)
                if tmp27 then
                  local err = _id(exn)
                  _L[309]("Failed to touch " .. output_ind .. " (" .. err .. ")")
                  return filelist_acc
                else
                  _raise(exn, nil)
                end
              end
            end
            return filelist_acc
          else
            return filelist_acc
          end
        end)
        local tmp26 = tmp25(filelist1)
        filelist2 = tmp26(filelist1)
      else
        _raise(_Match, "main.sml:362:33")
      end
      ::cont10::
      local filelist3
      if exp4.tag == "NONE" then
      else
        goto else3
      end
      if not find1(execlog, "No file [^\n]+%.gls%.") then
        filelist3 = filelist2
        goto cont11
      end
      _L[310]("You may want to use --makeglossaries option.")
      filelist3 = filelist2
      goto cont11
      ::else3::
      if exp4.tag == "SOME" then
        local makeglossaries = exp4.payload
        local tmp25 = foldl(function(a)
          local file = a[1]
          local filelist_acc = a[2]
          local tmp26 = _L[214](file.path)
          if tmp26 == "glo" then
            local output_gls
            do
              local glofileinfo = {abspath = file.abspath, kind = _L[386], path = file.path}
              local tmp27 = file.abspath
              output_gls = _L[210].replaceext(tmp27, "gls")
              local tmp28 = _L[390]({glofileinfo, nil}, auxstatus1)
              local tmp29
              if tmp28[1] then
                tmp29 = true
              else
                tmp29 = _L[391](auxstatus1, output_gls, file.abspath)
              end
              if tmp29 then
                local tmp30 = _L[207](output_directory)
                local tmp31 = _L[211](file.path)
                local tmp32 = _L[213](tmp31)
                local tmp33 = {n = 4, makeglossaries, "-d", tmp30, tmp32}
                local tmp34 = table_concat(tmp33, " ")
                _L[561](tmp34, {tag = "NONE"})
                return {{abspath = output_gls, kind = _L[386], path = output_gls}, filelist_acc}
              else
                goto else1
              end
            end
            ::else1::
            do
              local status, exn = pcall(function()
                local tmp27, tmp28 = _L[216].touch(output_gls)
                if not tmp27 then
                  local tmp29 = _id(tmp28)
                  _raise(tmp29, "fs-util.sml:28:25")
                else
                  return nil
                end
              end)
              if not status then
                local tmp27 = _isError(exn)
                if tmp27 then
                  local err = _id(exn)
                  _L[309]("Failed to touch " .. output_gls .. " (" .. err .. ")")
                  return filelist_acc
                else
                  _raise(exn, nil)
                end
              end
            end
            return filelist_acc
          else
            return filelist_acc
          end
        end)
        local tmp26 = tmp25(filelist2)
        filelist3 = tmp26(filelist2)
      else
        _raise(_Match, "main.sml:398:33")
      end
      ::cont11::
      local filelist4
      if exp.tag == "NONE" then
      else
        goto else4
      end
      if not find1(execlog, "No file [^\n]+%.bbl%.") then
        filelist4 = filelist3
        goto cont12
      end
      _L[310]("You may want to use --bibtex or biber option.")
      filelist4 = filelist3
      goto cont12
      ::else4::
      if exp.tag == "SOME" and exp.payload.tag == "BIBTEX" then
        local bibtex, bibtex_aux_hash2
        do
          bibtex = exp.payload.payload
          local biblines2
          do
            local tmp25 = _L[382](mainauxfile, output_directory, nil)
            biblines2 = revAppend(tmp25, nil)
          end
          local tmp25 = biblines2 == nil
          if tmp25 then
            bibtex_aux_hash2 = {tag = "NONE"}
            goto cont13
          end
          local tmp26 = _VectorOrArray_fromList(biblines2)
          local tmp27 = table_concat(tmp26, "\n")
          local tmp28 = _L[191](tmp27)
          bibtex_aux_hash2 = {tag = "SOME", payload = tmp28}
        end
        ::cont13::
        local output_bbl, tmp25
        do
          local tmp26 = jobname .. ".bbl"
          output_bbl = _L[210].join(output_directory, tmp26)
          local tmp27 = tmp(function(x, y)
            return x[4] == y[4] and (x[3] == y[3] and (x[2] == y[2] and x[1] == y[1]))
          end)
          local tmp28
          do
            local tmp29 = tmp27(bibtex_aux_hash, bibtex_aux_hash2)
            tmp28 = not tmp29
          end
          if tmp28 then
            tmp25 = true
            goto cont14
          end
          local tmp29 = _L[215]({tag = "NONE"}, mainauxfile)
          tmp25 = _L[391](auxstatus1, output_bbl, tmp29)
        end
        ::cont14::
        if tmp25 then
          local tmp26 = _L[207](output_directory)
          local tmp27 = _L[211](mainauxfile)
          local tmp28 = {n = 5, _L[209], tmp26, "&&", bibtex, tmp27}
          local tmp29 = table_concat(tmp28, " ")
          _L[561](tmp29, {tag = "NONE"})
          filelist4 = filelist3
          goto cont12
        else
          goto else6
        end
        ::else6::
        do
          local tmp26 = _ENV.CLUTTEX_VERBOSITY
          if tmp26 >= 1 then
          else
            goto cont15
          end
          _L[311]("No need to run BibTeX.")
        end
        ::cont15::
        do
          local status, exn = pcall(function()
            local tmp26, tmp27 = _L[216].touch(output_bbl)
            if not tmp26 then
              local tmp28 = _id(tmp27)
              _raise(tmp28, "fs-util.sml:28:25")
            else
              return nil
            end
          end)
          if not status then
            local tmp26 = _isError(exn)
            if tmp26 then
              local err = _id(exn)
              _L[309]("Failed to touch " .. output_bbl .. " (" .. err .. ")")
              filelist4 = filelist3
              goto cont12
            else
              _raise(exn, nil)
            end
          end
        end
        filelist4 = filelist3
        goto cont12
      else
        goto else5
      end
      ::else5::
      if exp.tag == "SOME" and exp.payload.tag == "BIBER" then
        local biber = exp.payload.payload
        local tmp25 = foldl(function(a)
          local file = a[1]
          local filelist_acc = a[2]
          local tmp26 = _L[214](file.path)
          if tmp26 == "bcf" then
            local bcffileinfo, output_bbl, updated_dot_bib
            do
              bcffileinfo = {abspath = file.abspath, kind = _L[386], path = file.path}
              local tmp27 = file.abspath
              output_bbl = _L[210].replaceext(tmp27, "bbl")
              local ins = _L[114](file.abspath)
              local tmp28 = false
              ::cont2::
              local updated_dot_bib1 = tmp28
              local exp8 = _L[113](ins)
              if exp8.tag == "NONE" then
              else
                goto else2
              end
              _L[119](ins)
              updated_dot_bib = updated_dot_bib1
              goto cont
              ::else2::
              if exp8.tag == "SOME" then
                local bibfile
                do
                  local l = exp8.payload
                  local bib = match(l, "<bcf:datasource .*>(.*)</bcf:datasource>")
                  if not bib then
                    tmp28 = updated_dot_bib1
                    goto cont2
                  end
                  bibfile = _L[210].join(original_wd, bib)
                  local tmp29 = _L[218](bibfile)
                  if tmp29 then
                    local tmp30 = _L[215]({tag = "NONE"}, mainauxfile)
                    local tmp31 = _L[391](auxstatus1, bibfile, tmp30)
                    local updated_dot_bib_tmp = not tmp31
                    if updated_dot_bib_tmp then
                    else
                      tmp28 = updated_dot_bib1 or updated_dot_bib_tmp
                      goto cont2
                    end
                    _L[311](bibfile .. " is newer than aux")
                    tmp28 = updated_dot_bib1 or updated_dot_bib_tmp
                    goto cont2
                  else
                    goto else3
                  end
                end
                ::else3::
                _L[309](bibfile .. " is not accessible")
                tmp28 = updated_dot_bib1
                goto cont2
              else
                _raise(_Match, "main.sml:478:65")
              end
            end
            ::cont::
            local tmp27
            do
              if updated_dot_bib then
                tmp27 = true
                goto cont1
              end
              local tmp28 = _L[390]({bcffileinfo, nil}, auxstatus1)
              if tmp28[1] then
                tmp27 = true
              else
                tmp27 = _L[391](auxstatus1, output_bbl, file.abspath)
              end
            end
            ::cont1::
            if tmp27 then
              local tmp28 = _L[207](output_directory)
              local tmp29 = _L[211](file.abspath)
              local tmp30 = {n = 4, biber, "--output-directory", tmp28, tmp29}
              local tmp31 = table_concat(tmp30, " ")
              _L[561](tmp31, {tag = "NONE"})
              return {{abspath = output_bbl, kind = _L[386], path = output_bbl}, filelist_acc}
            else
              goto else1
            end
            ::else1::
            do
              local status, exn = pcall(function()
                local tmp28, tmp29 = _L[216].touch(output_bbl)
                if not tmp28 then
                  local tmp30 = _id(tmp29)
                  _raise(tmp30, "fs-util.sml:28:25")
                else
                  return nil
                end
              end)
              if not status then
                local tmp28 = _isError(exn)
                if tmp28 then
                  local err = _id(exn)
                  _L[309]("Failed to touch " .. output_bbl .. " (" .. err .. ")")
                  return filelist_acc
                else
                  _raise(exn, nil)
                end
              end
            end
            return filelist_acc
          else
            return filelist_acc
          end
        end)
        local tmp26 = tmp25(filelist3)
        filelist4 = tmp26(filelist3)
      else
        _raise(_Match, "main.sml:432:33")
      end
      ::cont12::
      local tmp25 = _L[51]("No pages of output.")
      local tmp26 = tmp25(execlog)
      if tmp26 then
        exp7 = _L[563]
        goto cont2
      end
      local exp8 = _L[390](filelist4, auxstatus1)
      local should_rerun = exp8[1]
      local auxstatus2 = exp8[2]
      local tmp27
      tmp27 = should_rerun or lightweight_mode
      if tmp27 then
        exp7 = {tag = "SHOULD_RERUN", payload = auxstatus2}
      else
        exp7 = _L[562]
      end
    end
    ::cont2::
    if exp7.tag == "NO_PAGES_OF_OUTPUT" then
    else
      goto else1
    end
    _L[309]("No pages of output.")
    tmp21 = false
    goto cont
    ::else1::
    if exp7.tag == "NO_NEED_TO_RERUN" then
      tmp21 = true
      goto cont
    elseif exp7.tag == "SHOULD_RERUN" then
      local auxstatus = exp7.payload
      if iteration >= max_iterations then
      else
        tmp22 = iteration
        tmp23 = auxstatus
        goto cont1
      end
      _L[309]("LaTeX should be run once more.")
      tmp21 = true
    else
      _raise(_Match, "main.sml:541:20")
    end
  end
  ::cont::
  if tmp21 then
    do
      local tmp22 = _L[232](exp5, _L[231])
      local tmp23
      tmp23 = tmp22 or supports_pdf_generation
      if tmp23 then
        local tmp24 = jobname .. "." .. output_extension
        local outfile = _L[210].join(output_directory, tmp24)
        local onCopyError
        if _L[220] then
          onCopyError = {tag = "SOME", payload = function(a)
            local output_format1
            if exp5 == "DVI" then
              output_format1 = "DVI"
            elseif exp5 == "PDF" then
              output_format1 = "PDF"
            else
              _raise(_Match, "main.sml:559:85")
            end
            _L[308]("Failed to copy file.  Some applications may be locking the " .. output_format1 .. " file.")
            return false
          end}
        else
          onCopyError = {tag = "NONE"}
        end
        local tmp25 = _L[217].copy_command(outfile, output)
        _L[561](tmp25, onCopyError)
        if dvipdfmx_extraoptions == nil then
          goto cont1
        end
        _L[309]("--dvipdfmx-option[s] are ignored.")
        goto cont1
      else
        goto else1
      end
      ::else1::
      local tmp24 = jobname .. ".dvi"
      local dvifile = _L[210].join(output_directory, tmp24)
      local tmp25 = _L[207](output)
      local tmp26 = _L[207](dvifile)
      local tmp27 = {tmp26, nil}
      local tmp28
      do
        local tmp29 = revAppend(dvipdfmx_extraoptions, nil)
        tmp28 = revAppend(tmp29, tmp27)
      end
      local tmp29 = _VectorOrArray_fromList({"dvipdfmx", {"-o", {tmp25, tmp28}}})
      local tmp30 = table_concat(tmp29, " ")
      _L[561](tmp30, {tag = "NONE"})
    end
    ::cont1::
    do
      local tmp22 = _L[232](exp5, _L[230])
      if tmp22 then
        local tmp23 = getOpt(synctex, "0")
        local synctex_lua = tonumber(tmp23)
        if synctex_lua == nil then
          goto cont2
        end
        local synctex_ext
        if synctex_lua > 0 then
          synctex_ext = {tag = "SOME", payload = "synctex.gz"}
        elseif synctex_lua < 0 then
          synctex_ext = {tag = "SOME", payload = "synctex"}
        else
          synctex_ext = {tag = "NONE"}
        end
        if synctex_ext.tag == "SOME" then
          local ext = synctex_ext.payload
          local tmp24 = jobname .. "." .. ext
          local tmp25 = _L[210].join(output_directory, tmp24)
          local tmp26 = _L[210].replaceext(output, ext)
          local tmp27 = _L[217].copy_command(tmp25, tmp26)
          _L[561](tmp27, {tag = "NONE"})
        elseif synctex_ext.tag == "NONE" then
          goto cont2
        else
          _raise(_Match, "main.sml:596:27")
        end
      else
        goto cont2
      end
    end
    ::cont2::
    if exp3.tag == "SOME" then
      local make_depends, recorded, tmp22
      do
        make_depends = exp3.payload
        recorded = _L[389](recorderfile, makeindex, nil, _L[202])
        local tmp23 = _L[239](engine_type, _L[237])
        if tmp23 then
        else
          tmp22 = false
          goto cont4
        end
        tmp22 = _L[218](recorderfile2)
      end
      ::cont4::
      local recorded1
      if tmp22 then
        local fileList = recorded[1]
        recorded1 = _L[389](recorderfile2, makeindex, fileList, recorded[2])
      else
        recorded1 = recorded
        goto cont5
      end
      ::cont5::
      local exp7
      do
        local fileInfo = recorded1[1]
        exp7 = _L[388](fileInfo, recorded1[2])
      end
      local filelist = exp7[1]
      local outs
      do
        local tmp23, tmp24 = open_PRIME(make_depends, "w")
        if tmp23 == nil then
          _raise(setmetatable({tag = Io_tag, payload = {cause = setmetatable({tag = _Fail_tag, payload = tmp24}, _exn_meta), ["function"] = "TextIO.openOut", name = make_depends}}, _exn_meta), "text-io.sml:496:27")
        else
          local tmp25 = _L[112].mkOutstream
          outs = tmp25({tag = "LUA_WRITABLE", payload = {buffer_mode = {BLOCK_BUF}, name = make_depends, writable = tmp23}})
        end
      end
      _L[123]({outs, output .. ":"})
      local tmp23 = app1(function(a)
        if a.kind == "INPUT" then
          return _L[123]({outs, " " .. a.path})
        else
          return nil
        end
      end)
      tmp23(filelist)
      _L[123]({outs, "\n"})
      _L[120](outs)
    elseif exp3.tag == "NONE" then
      goto cont3
    else
      _raise(_Match, "main.sml:604:16")
    end
    ::cont3::
    local tmp22 = _ENV.CLUTTEX_VERBOSITY
    if tmp22 >= 1 then
      return _L[311]("Command exited successfully")
    else
      return nil
    end
  else
    return nil
  end
end
_L[565] = function(fswatcherlib, files)
  local watcher = assert(fswatcherlib.new())
  local tmp21 = app1(function(file)
    assert(watcher:add_file(file))
    return nil
  end)
  tmp21(files)
  local result = assert(watcher:next())
  local tmp22 = _ENV.CLUTTEX_VERBOSITY
  if tmp22 >= 2 then
    local tmp23 = result.action
    local tmp24 = tmp23 .. " "
    local tmp25 = result.path
    _L[311](tmp24 .. tmp25)
    watcher:close()
    return true
  else
    watcher:close()
    return true
  end
end
_L[566] = function(files)
  local fswatch_command_str
  do
    local tmp21 = map(_L[207])
    local tmp22 = tmp21(files)
    local tmp23 = _VectorOrArray_fromList({"fswatch", {"--one-event", {"--event=Updated", {"--", tmp22}}}})
    fswatch_command_str = table_concat(tmp23, " ")
    local tmp24 = _ENV.CLUTTEX_VERBOSITY
    if tmp24 >= 1 then
    else
      goto cont
    end
    _L[307](fswatch_command_str)
  end
  ::cont::
  local fswatch = assert(popen(fswatch_command_str, "r"))
  local readLine = fswatch:lines()
  ::cont1::
  local l = readLine()
  if not l then
    fswatch:close()
    return false
  end
  local tmp21 = exists(function(path)
    return l == path
  end)
  local tmp22 = tmp21(files)
  if tmp22 then
    fswatch:close()
    return true
  else
    goto cont1
  end
end
_L[567] = function(files)
  local inotifywait_command_str
  do
    local tmp21 = map(_L[207])
    local tmp22 = tmp21(files)
    local tmp23 = _VectorOrArray_fromList({"inotifywait", {"--event=modify", {"--event=attrib", {"--format=%w", {"--quiet", tmp22}}}}})
    inotifywait_command_str = table_concat(tmp23, " ")
    local tmp24 = _ENV.CLUTTEX_VERBOSITY
    if tmp24 >= 1 then
    else
      goto cont
    end
    _L[307](inotifywait_command_str)
  end
  ::cont::
  local inotifywait = assert(popen(inotifywait_command_str, "r"))
  local readLine = inotifywait:lines()
  ::cont1::
  local l = readLine()
  if not l then
    inotifywait:close()
    return false
  end
  local tmp21 = exists(function(path)
    return l == path
  end)
  local tmp22 = tmp21(files)
  if tmp22 then
    inotifywait:close()
    return true
  else
    goto cont1
  end
end
_L[568], _L[569] = pcall(function()
  local exp
  do
    local tmp21
    do
      local tmp22 = #_L[153]
      tmp21 = tabulate(tmp22, function(i)
        local tmp23 = _Int_add(i, 1)
        local tmp24 = _L[153][tmp23]
        return tmp24
      end)
    end
    local tmp22, tmp23 = _L[378], tmp21
    ::cont13::
    local opts, args = tmp22, tmp23
    local exp1
    do
      if args == nil then
        exp1 = {tag = "NONE"}
        goto cont14
      end
      local tmp24 = _L[560]
      ::cont15::
      local a = tmp24
      if a == nil then
        exp1 = {tag = "NONE"}
        goto cont14
      elseif a ~= nil then
        local tmp25, exp2
        do
          local tmp26 = a[1]
          tmp25 = a[2]
          local _1, _2
          do
            _1 = tmp26[1]
            _2 = tmp26[2]
            if args == nil then
              exp2 = {tag = "NONE"}
              goto cont16
            end
            local tmp27 = _1.tag == "SHORT" and _2.tag == "SIMPLE"
            local tmp28
            tmp28 = tmp27 and args ~= nil
            if tmp28 then
              local s = _1.payload
              local v = _2.payload
              local tmp29 = args[1]
              local tmp30 = args[2]
              if tmp29 == s then
                exp2 = {tag = "SOME", payload = {v, tmp30}}
                goto cont16
              else
                exp2 = {tag = "NONE"}
                goto cont16
              end
            end
            local tmp29 = _1.tag == "SHORT" and _2.tag == "WITH_ARG"
            local tmp30
            tmp30 = tmp29 and args ~= nil
            if tmp30 then
              local s = _1.payload
              local f = _2.payload
              local tmp31 = args[1]
              local tmp32 = args[2]
              if tmp31 == s then
                if tmp32 == nil then
                  _raise(setmetatable({tag = _Fail_tag, payload = "argument missing after " .. s}, _exn_meta), "handle-options.sml:21:21")
                elseif tmp32 ~= nil then
                  local tmp33 = tmp32[1]
                  local tmp34 = tmp32[2]
                  local tmp35 = f(tmp33)
                  exp2 = {tag = "SOME", payload = {tmp35, tmp34}}
                  goto cont16
                else
                  _raise(_Match, "handle-options.sml:20:11")
                end
              else
                goto else38
              end
              ::else38::
              local tmp33 = isPrefix(s)
              local tmp34 = tmp33(tmp31)
              if tmp34 then
                local tmp35 = #s
                local arg_PRIME = extract(tmp31, tmp35, {tag = "NONE"})
                local tmp36 = f(arg_PRIME)
                exp2 = {tag = "SOME", payload = {tmp36, tmp32}}
                goto cont16
              else
                exp2 = {tag = "NONE"}
                goto cont16
              end
            else
              goto else35
            end
          end
          ::else35::
          do
            local tmp27 = _1.tag == "SHORT" and _2.tag == "WITH_OPTIONAL_ARG"
            local tmp28
            tmp28 = tmp27 and args ~= nil
            if tmp28 then
              local s, action, tmp29, tmp30
              do
                s = _1.payload
                local default = _2.payload.default
                action = _2.payload.action
                tmp29 = args[1]
                tmp30 = args[2]
                if tmp29 == s then
                  local tmp31 = action(default)
                  exp2 = {tag = "SOME", payload = {tmp31, tmp30}}
                  goto cont16
                else
                  goto else38
                end
              end
              ::else38::
              local tmp31 = isPrefix(s)
              local tmp32 = tmp31(tmp29)
              if tmp32 then
                local tmp33 = #s
                local arg_PRIME = extract(tmp29, tmp33, {tag = "NONE"})
                local tmp34 = action(arg_PRIME)
                exp2 = {tag = "SOME", payload = {tmp34, tmp30}}
                goto cont16
              else
                exp2 = {tag = "NONE"}
                goto cont16
              end
            else
              goto else36
            end
          end
          ::else36::
          do
            local tmp27 = _1.tag == "LONG" and _2.tag == "SIMPLE"
            local tmp28
            tmp28 = tmp27 and args ~= nil
            if tmp28 then
              local s = _1.payload
              local v = _2.payload
              local tmp29 = args[1]
              local tmp30 = args[2]
              if tmp29 == s then
                exp2 = {tag = "SOME", payload = {v, tmp30}}
                goto cont16
              else
                exp2 = {tag = "NONE"}
                goto cont16
              end
            end
            local tmp29 = _1.tag == "LONG" and _2.tag == "WITH_ARG"
            local tmp30
            tmp30 = tmp29 and args ~= nil
            if tmp30 then
              local s = _1.payload
              local f = _2.payload
              local tmp31 = args[1]
              local tmp32 = args[2]
              if tmp31 == s then
                if tmp32 == nil then
                  _raise(setmetatable({tag = _Fail_tag, payload = "argument missing after " .. s}, _exn_meta), "handle-options.sml:45:21")
                elseif tmp32 ~= nil then
                  local tmp33 = tmp32[1]
                  local tmp34 = tmp32[2]
                  local tmp35 = f(tmp33)
                  exp2 = {tag = "SOME", payload = {tmp35, tmp34}}
                  goto cont16
                else
                  _raise(_Match, "handle-options.sml:44:11")
                end
              else
                goto else38
              end
              ::else38::
              local tmp33 = isPrefix(s .. "=")
              local tmp34 = tmp33(tmp31)
              if tmp34 then
                local tmp35 = _Int_add(#s, 1)
                local arg_PRIME = extract(tmp31, tmp35, {tag = "NONE"})
                local tmp36 = f(arg_PRIME)
                exp2 = {tag = "SOME", payload = {tmp36, tmp32}}
                goto cont16
              else
                exp2 = {tag = "NONE"}
                goto cont16
              end
            else
              goto else37
            end
          end
          ::else37::
          local tmp27 = _1.tag == "LONG" and _2.tag == "WITH_OPTIONAL_ARG"
          local tmp28
          tmp28 = tmp27 and args ~= nil
          if tmp28 then
            local s, action, tmp29, tmp30
            do
              s = _1.payload
              local default = _2.payload.default
              action = _2.payload.action
              tmp29 = args[1]
              tmp30 = args[2]
              if tmp29 == s then
                local tmp31 = action(default)
                exp2 = {tag = "SOME", payload = {tmp31, tmp30}}
                goto cont16
              else
                goto else38
              end
            end
            ::else38::
            local tmp31 = isPrefix(s .. "=")
            local tmp32 = tmp31(tmp29)
            if tmp32 then
              local tmp33 = _Int_add(#s, 1)
              local arg_PRIME = extract(tmp29, tmp33, {tag = "NONE"})
              local tmp34 = action(arg_PRIME)
              exp2 = {tag = "SOME", payload = {tmp34, tmp30}}
            else
              exp2 = {tag = "NONE"}
              goto cont16
            end
          else
            _raise(_Match, "handle-options.sml:13:5")
          end
        end
        ::cont16::
        if exp2.tag == "SOME" then
          exp1 = {tag = "SOME", payload = exp2.payload}
        elseif exp2.tag == "NONE" then
          tmp24 = tmp25
          goto cont15
        else
          _raise(_Match, "handle-options.sml:64:62")
        end
      else
        _raise(_Match, "handle-options.sml:63:41")
      end
    end
    ::cont14::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_ENGINE" then
      local engine = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.engine
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine_executable
        local tmp31 = opts.file_line_error
        local tmp32 = opts.fmt
        local tmp33 = opts.fresh
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = {tag = "SOME", payload = engine}, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:169:46")
      end
      _L[123]({_L[117], "multiple --engine options\n"})
      exit(1, true)
    else
      goto else4
    end
    ::else4::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_ENGINE_EXECUTABLE" then
      local executable = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.engine_executable
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.file_line_error
        local tmp32 = opts.fmt
        local tmp33 = opts.fresh
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = {tag = "SOME", payload = executable}, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:173:61")
      end
      _L[123]({_L[117], "multiple --engine-executable options\n"})
      exit(1, true)
    else
      goto else5
    end
    ::else5::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_OUTPUT" then
      local output = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.output
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeglossaries
        local tmp41 = opts.makeindex
        local tmp42 = opts.max_iterations
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = {tag = "SOME", payload = output}, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:177:46")
      end
      _L[123]({_L[117], "multiple --output options\n"})
      exit(1, true)
    else
      goto else6
    end
    ::else6::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_FRESH" then
      local args1 = exp1.payload[2]
      local exp2 = opts.fresh
      if not exp2 then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = true, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = opts.watch}
        tmp23 = args1
        goto cont13
      elseif exp2 then
      else
        _raise(_Match, "handle-options.sml:181:38")
      end
      _L[123]({_L[117], "multiple --fresh options\n"})
      exit(1, true)
    else
      goto else7
    end
    ::else7::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_MAX_ITERATIONS" then
      local exp2
      do
        local n = exp1.payload[1].payload
        local args1 = exp1.payload[2]
        exp2 = opts.max_iterations
        if exp2.tag == "NONE" then
          local exp3
          do
            local tmp24 = scanString(function(getc)
              return function(strm)
                local strm1
                do
                  local strm2 = skipInitialWhitespace(getc, strm)
                  local exp4 = scanSign(getc, strm2)
                  local isNegative = exp4[1]
                  strm1 = exp4[2]
                  if isNegative then
                    local exp5 = getc(strm1)
                    if exp5.tag == "SOME" then
                      local c = exp5.payload[1]
                      local strm_PRIME = exp5.payload[2]
                      if 48 <= c and c <= 57 then
                        local tmp25, tmp26
                        do
                          local tmp27 = digitToInt(c)
                          local tmp28 = _Int_negate(tmp27)
                          tmp26, tmp25 = tmp28, strm_PRIME
                        end
                        ::cont::
                        local x, strm3 = tmp26, tmp25
                        local exp6 = getc(strm3)
                        if exp6.tag == "SOME" then
                          local c1 = exp6.payload[1]
                          local strm_PRIME1 = exp6.payload[2]
                          if 48 <= c1 and c1 <= 57 then
                            local tmp27 = _Int_mul(10, x)
                            local tmp28 = digitToInt(c1)
                            tmp26 = _Int_sub(tmp27, tmp28)
                            tmp25 = strm_PRIME1
                            goto cont
                          else
                            return {tag = "SOME", payload = {x, strm3}}
                          end
                        elseif exp6.tag == "NONE" then
                          return {tag = "SOME", payload = {x, strm3}}
                        else
                          _raise(_Match, "scan-num.sml:37:35")
                        end
                      else
                        return {tag = "NONE"}
                      end
                    elseif exp5.tag == "NONE" then
                      return {tag = "NONE"}
                    else
                      _raise(_Match, "scan-num.sml:43:25")
                    end
                  else
                    goto else1
                  end
                end
                ::else1::
                local exp4 = getc(strm1)
                if exp4.tag == "SOME" then
                  local c = exp4.payload[1]
                  local strm_PRIME = exp4.payload[2]
                  if 48 <= c and c <= 57 then
                    local tmp25, tmp26
                    do
                      local tmp27 = digitToInt(c)
                      tmp26, tmp25 = tmp27, strm_PRIME
                    end
                    ::cont::
                    local x, strm2 = tmp26, tmp25
                    local exp5 = getc(strm2)
                    if exp5.tag == "SOME" then
                      local c1 = exp5.payload[1]
                      local strm_PRIME1 = exp5.payload[2]
                      if 48 <= c1 and c1 <= 57 then
                        local tmp27 = _Int_mul(10, x)
                        local tmp28 = digitToInt(c1)
                        tmp26 = _Int_add(tmp27, tmp28)
                        tmp25 = strm_PRIME1
                        goto cont
                      else
                        return {tag = "SOME", payload = {x, strm2}}
                      end
                    elseif exp5.tag == "NONE" then
                      return {tag = "SOME", payload = {x, strm2}}
                    else
                      _raise(_Match, "scan-num.sml:23:35")
                    end
                  else
                    return {tag = "NONE"}
                  end
                elseif exp4.tag == "NONE" then
                  return {tag = "NONE"}
                else
                  _raise(_Match, "scan-num.sml:29:25")
                end
              end
            end)
            exp3 = tmp24(n)
          end
          if exp3.tag == "SOME" then
            local n1 = exp3.payload
            local tmp24 = opts.bibtex_or_biber
            local tmp25 = opts.change_directory
            local tmp26 = opts.check_driver
            local tmp27 = opts.color
            local tmp28 = opts.config_file
            local tmp29 = opts.dvipdfmx_extraoptions_rev
            local tmp30 = opts.engine
            local tmp31 = opts.engine_executable
            local tmp32 = opts.file_line_error
            local tmp33 = opts.fmt
            local tmp34 = opts.fresh
            local tmp35 = opts.halt_on_error
            local tmp36 = opts.includeonly
            local tmp37 = opts.interaction
            local tmp38 = opts.jobname
            local tmp39 = opts.make_depends
            local tmp40 = opts.makeglossaries
            local tmp41 = opts.makeindex
            local tmp42 = opts.output
            local tmp43 = opts.output_directory
            local tmp44 = opts.output_format
            local tmp45 = opts.package_support
            local tmp46 = opts.print_output_directory
            local tmp47 = opts.shell_escape
            local tmp48 = opts.source_date_epoch
            local tmp49 = opts.start_with_draft
            local tmp50 = opts.synctex
            local tmp51 = opts.tex_extraoptions_rev
            local tmp52 = opts.watch
            tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = {tag = "SOME", payload = n1}, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
            tmp23 = args1
            goto cont13
          elseif exp3.tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:186:62")
          end
          _L[123]({_L[117], "invalid value for --max-iterations option\n"})
          exit(1, true)
        else
          goto else35
        end
      end
      ::else35::
      if exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:185:49")
      end
      _L[123]({_L[117], "multiple --max-iterations options\n"})
      exit(1, true)
    else
      goto else8
    end
    ::else8::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_START_WITH_DRAFT" then
      local args1 = exp1.payload[2]
      local exp2 = opts.start_with_draft
      if not exp2 then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeglossaries
        local tmp41 = opts.makeindex
        local tmp42 = opts.max_iterations
        local tmp43 = opts.output
        local tmp44 = opts.output_directory
        local tmp45 = opts.output_format
        local tmp46 = opts.package_support
        local tmp47 = opts.print_output_directory
        local tmp48 = opts.shell_escape
        local tmp49 = opts.source_date_epoch
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = tmp47, shell_escape = tmp48, source_date_epoch = tmp49, start_with_draft = true, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = opts.watch}
        tmp23 = args1
        goto cont13
      elseif exp2 then
      else
        _raise(_Match, "handle-options.sml:192:49")
      end
      _L[123]({_L[117], "multiple --start-with-draft options\n"})
      exit(1, true)
    else
      goto else9
    end
    ::else9::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_WATCH" then
      local exp2
      do
        local engine = exp1.payload[1].payload
        local args1 = exp1.payload[2]
        exp2 = opts.watch
        if exp2.tag == "NONE" then
          local exp3
          if engine == "fswatch" then
            exp3 = {tag = "SOME", payload = _L[350]}
          elseif engine == "inotifywait" then
            exp3 = {tag = "SOME", payload = _L[351]}
          elseif engine == "auto" then
            exp3 = {tag = "SOME", payload = _L[352]}
          else
            exp3 = {tag = "NONE"}
          end
          if exp3.tag == "SOME" then
            local engine1 = exp3.payload
            local tmp24 = opts.bibtex_or_biber
            local tmp25 = opts.change_directory
            local tmp26 = opts.check_driver
            local tmp27 = opts.color
            local tmp28 = opts.config_file
            local tmp29 = opts.dvipdfmx_extraoptions_rev
            local tmp30 = opts.engine
            local tmp31 = opts.engine_executable
            local tmp32 = opts.file_line_error
            local tmp33 = opts.fmt
            local tmp34 = opts.fresh
            local tmp35 = opts.halt_on_error
            local tmp36 = opts.includeonly
            local tmp37 = opts.interaction
            local tmp38 = opts.jobname
            local tmp39 = opts.make_depends
            local tmp40 = opts.makeglossaries
            local tmp41 = opts.makeindex
            local tmp42 = opts.max_iterations
            local tmp43 = opts.output
            local tmp44 = opts.output_directory
            local tmp45 = opts.output_format
            local tmp46 = opts.package_support
            local tmp47 = opts.print_output_directory
            local tmp48 = opts.shell_escape
            local tmp49 = opts.source_date_epoch
            local tmp50 = opts.start_with_draft
            local tmp51 = opts.synctex
            local tmp52 = opts.tex_extraoptions_rev
            tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = tmp47, shell_escape = tmp48, source_date_epoch = tmp49, start_with_draft = tmp50, synctex = tmp51, tex_extraoptions_rev = tmp52, watch = {tag = "SOME", payload = engine1}}
            tmp23 = args1
            goto cont13
          elseif exp3.tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:197:58")
          end
          _L[123]({_L[117], "invalid value for --watch option\n"})
          exit(1, true)
        else
          goto else35
        end
      end
      ::else35::
      if exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:196:45")
      end
      _L[123]({_L[117], "multiple --watch options\n"})
      exit(1, true)
    else
      goto else10
    end
    ::else10::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_HELP" then
    else
      goto else11
    end
    exp = _L[408](0)
    goto cont
    ::else11::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_VERSION" then
    else
      goto else12
    end
    _L[123]({_L[117], "cluttex v0.7.1\n"})
    exit(0, true)
    ::else12::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_VERBOSE" then
      local args1 = exp1.payload[2]
      _ENV.CLUTTEX_VERBOSITY = _ENV.CLUTTEX_VERBOSITY + 1
      tmp22 = opts
      tmp23 = args1
      goto cont13
    elseif exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_COLOR" then
      local exp2
      do
        local mode = exp1.payload[1].payload
        local args1 = exp1.payload[2]
        exp2 = opts.color
        if exp2.tag == "NONE" then
          local exp3
          if mode == "always" then
            exp3 = {tag = "SOME", payload = _L[278]}
          elseif mode == "auto" then
            exp3 = {tag = "SOME", payload = _L[279]}
          elseif mode == "never" then
            exp3 = {tag = "SOME", payload = _L[280]}
          else
            exp3 = {tag = "NONE"}
          end
          if exp3.tag == "SOME" then
            local mode1 = exp3.payload
            _L[282](mode1)
            local tmp24 = opts.bibtex_or_biber
            local tmp25 = opts.change_directory
            local tmp26 = opts.check_driver
            local tmp27 = opts.config_file
            local tmp28 = opts.dvipdfmx_extraoptions_rev
            local tmp29 = opts.engine
            local tmp30 = opts.engine_executable
            local tmp31 = opts.file_line_error
            local tmp32 = opts.fmt
            local tmp33 = opts.fresh
            local tmp34 = opts.halt_on_error
            local tmp35 = opts.includeonly
            local tmp36 = opts.interaction
            local tmp37 = opts.jobname
            local tmp38 = opts.make_depends
            local tmp39 = opts.makeglossaries
            local tmp40 = opts.makeindex
            local tmp41 = opts.max_iterations
            local tmp42 = opts.output
            local tmp43 = opts.output_directory
            local tmp44 = opts.output_format
            local tmp45 = opts.package_support
            local tmp46 = opts.print_output_directory
            local tmp47 = opts.shell_escape
            local tmp48 = opts.source_date_epoch
            local tmp49 = opts.start_with_draft
            local tmp50 = opts.synctex
            local tmp51 = opts.tex_extraoptions_rev
            local tmp52 = opts.watch
            tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = {tag = "SOME", payload = mode1}, config_file = tmp27, dvipdfmx_extraoptions_rev = tmp28, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
            tmp23 = args1
            goto cont13
          else
            goto else36
          end
          ::else36::
          if exp3.tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:209:56")
          end
          _L[123]({_L[117], "invalid value for --color option\n"})
          exit(1, true)
        else
          goto else35
        end
      end
      ::else35::
      if exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:208:43")
      end
      _L[123]({_L[117], "multiple --color options\n"})
      exit(1, true)
    else
      goto else13
    end
    ::else13::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_CHANGE_DIRECTORY" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.change_directory
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.check_driver
        local tmp26 = opts.color
        local tmp27 = opts.config_file
        local tmp28 = opts.dvipdfmx_extraoptions_rev
        local tmp29 = opts.engine
        local tmp30 = opts.engine_executable
        local tmp31 = opts.file_line_error
        local tmp32 = opts.fmt
        local tmp33 = opts.fresh
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = {tag = "SOME", payload = x}, check_driver = tmp25, color = tmp26, config_file = tmp27, dvipdfmx_extraoptions_rev = tmp28, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:217:51")
      end
      _L[123]({_L[117], "multiple --change-directory options\n"})
      exit(1, true)
    else
      goto else14
    end
    ::else14::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_INCLUDEONLY" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.includeonly
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = {tag = "SOME", payload = x}, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:221:46")
      end
      _L[123]({_L[117], "multiple --includeonly options\n"})
      exit(1, true)
    else
      goto else15
    end
    ::else15::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_MAKE_DEPENDS" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.make_depends
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = {tag = "SOME", payload = x}, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:225:47")
      end
      _L[123]({_L[117], "multiple --make-depends options\n"})
      exit(1, true)
    else
      goto else16
    end
    ::else16::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_PRINT_OUTPUT_DIRECTORY" then
      local args1 = exp1.payload[2]
      local exp2 = opts.print_output_directory
      if not exp2 then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeglossaries
        local tmp41 = opts.makeindex
        local tmp42 = opts.max_iterations
        local tmp43 = opts.output
        local tmp44 = opts.output_directory
        local tmp45 = opts.output_format
        local tmp46 = opts.package_support
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = true, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = opts.watch}
        tmp23 = args1
        goto cont13
      elseif exp2 then
      else
        _raise(_Match, "handle-options.sml:229:55")
      end
      _L[123]({_L[117], "multiple --print-output-directory options\n"})
      exit(1, true)
    else
      goto else17
    end
    ::else17::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_PACKAGE_SUPPORT" then
      local args1, packages
      do
        local s = exp1.payload[1].payload
        args1 = exp1.payload[2]
        local tmp24, tmp25, tmp26
        do
          local tmp27 = explode(s)
          tmp26, tmp25, tmp24 = nil, nil, tmp27
        end
        ::cont16::
        local revTokens, acc, _3 = tmp26, tmp25, tmp24
        if _3 == nil then
          if acc == nil then
          else
            goto else36
          end
          packages = revAppend(revTokens, nil)
          goto cont15
          ::else36::
          local tmp27 = implodeRev(acc)
          packages = revAppend({tmp27, revTokens}, nil)
          goto cont15
        else
          goto else35
        end
        ::else35::
        if _3 ~= nil then
          local tmp27 = _3[1]
          local tmp28 = _3[2]
          local tmp29
          if tmp27 == 44 then
            tmp29 = true
          else
            tmp29 = isSpace(tmp27)
          end
          if tmp29 then
            if acc == nil then
              tmp26 = revTokens
              tmp25 = nil
              tmp24 = tmp28
              goto cont16
            end
            local tmp30 = implodeRev(acc)
            tmp26 = {tmp30, revTokens}
            tmp25 = nil
            tmp24 = tmp28
            goto cont16
          else
            tmp26 = revTokens
            tmp25 = {tmp27, acc}
            tmp24 = tmp28
            goto cont16
          end
        else
          _raise(_Match, "string-1.sml:73:26")
        end
      end
      ::cont15::
      local tmp24 = foldl(function(a)
        if a[1] == "minted" then
          local ps = a[2]
          return {epstopdf = ps.epstopdf, minted = true, pdfx = ps.pdfx}
        elseif a[1] == "epstopdf" then
          local ps = a[2]
          return {epstopdf = true, minted = ps.minted, pdfx = ps.pdfx}
        elseif a[1] == "pdfx" then
          local ps = a[2]
          return {epstopdf = ps.epstopdf, minted = ps.minted, pdfx = true}
        end
        local pkg = a[1]
        local ps = a[2]
        local tmp25 = _ENV.CLUTTEX_VERBOSITY
        if tmp25 >= 1 then
          _L[309]("ClutTeX provides no special support for '" .. pkg .. "'.")
          return ps
        else
          return ps
        end
      end)
      local tmp25 = tmp24(opts.package_support)
      local ps = tmp25(packages)
      local tmp26 = opts.bibtex_or_biber
      local tmp27 = opts.change_directory
      local tmp28 = opts.check_driver
      local tmp29 = opts.color
      local tmp30 = opts.config_file
      local tmp31 = opts.dvipdfmx_extraoptions_rev
      local tmp32 = opts.engine
      local tmp33 = opts.engine_executable
      local tmp34 = opts.file_line_error
      local tmp35 = opts.fmt
      local tmp36 = opts.fresh
      local tmp37 = opts.halt_on_error
      local tmp38 = opts.includeonly
      local tmp39 = opts.interaction
      local tmp40 = opts.jobname
      local tmp41 = opts.make_depends
      local tmp42 = opts.makeglossaries
      local tmp43 = opts.makeindex
      local tmp44 = opts.max_iterations
      local tmp45 = opts.output
      local tmp46 = opts.output_directory
      local tmp47 = opts.output_format
      local tmp48 = opts.print_output_directory
      local tmp49 = opts.shell_escape
      local tmp50 = opts.source_date_epoch
      local tmp51 = opts.start_with_draft
      local tmp52 = opts.synctex
      local tmp53 = opts.tex_extraoptions_rev
      tmp22 = {bibtex_or_biber = tmp26, change_directory = tmp27, check_driver = tmp28, color = tmp29, config_file = tmp30, dvipdfmx_extraoptions_rev = tmp31, engine = tmp32, engine_executable = tmp33, file_line_error = tmp34, fmt = tmp35, fresh = tmp36, halt_on_error = tmp37, includeonly = tmp38, interaction = tmp39, jobname = tmp40, make_depends = tmp41, makeglossaries = tmp42, makeindex = tmp43, max_iterations = tmp44, output = tmp45, output_directory = tmp46, output_format = tmp47, package_support = ps, print_output_directory = tmp48, shell_escape = tmp49, source_date_epoch = tmp50, start_with_draft = tmp51, synctex = tmp52, tex_extraoptions_rev = tmp53, watch = opts.watch}
      tmp23 = args1
      goto cont13
    else
      goto else18
    end
    ::else18::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_CHECK_DRIVER" then
      local exp2
      do
        local driver = exp1.payload[1].payload
        local args1 = exp1.payload[2]
        exp2 = opts.check_driver
        if exp2.tag == "NONE" then
          local exp3
          if driver == "dvipdfmx" then
            exp3 = {tag = "SOME", payload = _L[347]}
          elseif driver == "dvips" then
            exp3 = {tag = "SOME", payload = _L[348]}
          elseif driver == "dvisvgm" then
            exp3 = {tag = "SOME", payload = _L[349]}
          else
            exp3 = {tag = "NONE"}
          end
          if exp3.tag == "SOME" then
            local driver1 = exp3.payload
            local tmp24 = opts.bibtex_or_biber
            local tmp25 = opts.change_directory
            local tmp26 = opts.color
            local tmp27 = opts.config_file
            local tmp28 = opts.dvipdfmx_extraoptions_rev
            local tmp29 = opts.engine
            local tmp30 = opts.engine_executable
            local tmp31 = opts.file_line_error
            local tmp32 = opts.fmt
            local tmp33 = opts.fresh
            local tmp34 = opts.halt_on_error
            local tmp35 = opts.includeonly
            local tmp36 = opts.interaction
            local tmp37 = opts.jobname
            local tmp38 = opts.make_depends
            local tmp39 = opts.makeglossaries
            local tmp40 = opts.makeindex
            local tmp41 = opts.max_iterations
            local tmp42 = opts.output
            local tmp43 = opts.output_directory
            local tmp44 = opts.output_format
            local tmp45 = opts.package_support
            local tmp46 = opts.print_output_directory
            local tmp47 = opts.shell_escape
            local tmp48 = opts.source_date_epoch
            local tmp49 = opts.start_with_draft
            local tmp50 = opts.synctex
            local tmp51 = opts.tex_extraoptions_rev
            local tmp52 = opts.watch
            tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = {tag = "SOME", payload = driver1}, color = tmp26, config_file = tmp27, dvipdfmx_extraoptions_rev = tmp28, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
            tmp23 = args1
            goto cont13
          elseif exp3.tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:247:65")
          end
          _L[123]({_L[117], "invalid value for --check-driver option\n"})
          exit(1, true)
        else
          goto else35
        end
      end
      ::else35::
      if exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:246:52")
      end
      _L[123]({_L[117], "multiple --check-driver options\n"})
      exit(1, true)
    else
      goto else19
    end
    ::else19::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_SOURCE_DATE_EPOCH" then
      local exp2
      do
        local time1 = exp1.payload[1].payload
        local args1 = exp1.payload[2]
        exp2 = opts.source_date_epoch
        if exp2.tag == "NONE" then
          local exp3
          do
            if time1 == "now" then
              exp3 = {tag = "SOME", payload = _L[354]}
              goto cont15
            end
            local tmp24
            if #time1 > 0 then
              local tmp25 = tmp7(isDigit)
              tmp24 = tmp25(time1)
            else
              tmp24 = false
              goto cont16
            end
            ::cont16::
            if tmp24 then
              exp3 = {tag = "SOME", payload = {tag = "RAW", payload = time1}}
            else
              exp3 = {tag = "NONE"}
            end
          end
          ::cont15::
          if exp3.tag == "SOME" then
            local time2 = exp3.payload
            local tmp24 = opts.bibtex_or_biber
            local tmp25 = opts.change_directory
            local tmp26 = opts.check_driver
            local tmp27 = opts.color
            local tmp28 = opts.config_file
            local tmp29 = opts.dvipdfmx_extraoptions_rev
            local tmp30 = opts.engine
            local tmp31 = opts.engine_executable
            local tmp32 = opts.file_line_error
            local tmp33 = opts.fmt
            local tmp34 = opts.fresh
            local tmp35 = opts.halt_on_error
            local tmp36 = opts.includeonly
            local tmp37 = opts.interaction
            local tmp38 = opts.jobname
            local tmp39 = opts.make_depends
            local tmp40 = opts.makeglossaries
            local tmp41 = opts.makeindex
            local tmp42 = opts.max_iterations
            local tmp43 = opts.output
            local tmp44 = opts.output_directory
            local tmp45 = opts.output_format
            local tmp46 = opts.package_support
            local tmp47 = opts.print_output_directory
            local tmp48 = opts.shell_escape
            local tmp49 = opts.start_with_draft
            local tmp50 = opts.synctex
            local tmp51 = opts.tex_extraoptions_rev
            local tmp52 = opts.watch
            tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = tmp47, shell_escape = tmp48, source_date_epoch = {tag = "SOME", payload = time2}, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
            tmp23 = args1
            goto cont13
          elseif exp3.tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:254:68")
          end
          _L[123]({_L[117], "invalid value for --source-date-epoch option\n"})
          exit(1, true)
        else
          goto else35
        end
      end
      ::else35::
      if exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:253:55")
      end
      _L[123]({_L[117], "multiple --source-date-epoch options\n"})
      exit(1, true)
    else
      goto else20
    end
    ::else20::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_SYNCTEX" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.synctex
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeglossaries
        local tmp41 = opts.makeindex
        local tmp42 = opts.max_iterations
        local tmp43 = opts.output
        local tmp44 = opts.output_directory
        local tmp45 = opts.output_format
        local tmp46 = opts.package_support
        local tmp47 = opts.print_output_directory
        local tmp48 = opts.shell_escape
        local tmp49 = opts.source_date_epoch
        local tmp50 = opts.start_with_draft
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = tmp47, shell_escape = tmp48, source_date_epoch = tmp49, start_with_draft = tmp50, synctex = {tag = "SOME", payload = x}, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:260:42")
      end
      _L[123]({_L[117], "multiple --synctex options\n"})
      exit(1, true)
    else
      goto else21
    end
    ::else21::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_FILE_LINE_ERROR" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local tmp24 = opts.bibtex_or_biber
      local tmp25 = opts.change_directory
      local tmp26 = opts.check_driver
      local tmp27 = opts.color
      local tmp28 = opts.config_file
      local tmp29 = opts.dvipdfmx_extraoptions_rev
      local tmp30 = opts.engine
      local tmp31 = opts.engine_executable
      local tmp32 = opts.fmt
      local tmp33 = opts.fresh
      local tmp34 = opts.halt_on_error
      local tmp35 = opts.includeonly
      local tmp36 = opts.interaction
      local tmp37 = opts.jobname
      local tmp38 = opts.make_depends
      local tmp39 = opts.makeglossaries
      local tmp40 = opts.makeindex
      local tmp41 = opts.max_iterations
      local tmp42 = opts.output
      local tmp43 = opts.output_directory
      local tmp44 = opts.output_format
      local tmp45 = opts.package_support
      local tmp46 = opts.print_output_directory
      local tmp47 = opts.shell_escape
      local tmp48 = opts.source_date_epoch
      local tmp49 = opts.start_with_draft
      local tmp50 = opts.synctex
      local tmp51 = opts.tex_extraoptions_rev
      tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = x, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = opts.watch}
      tmp23 = args1
      goto cont13
    elseif exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_INTERACTION" then
      local exp2
      do
        local x = exp1.payload[1].payload
        local args1 = exp1.payload[2]
        exp2 = opts.interaction
        if exp2.tag == "NONE" then
          local exp3
          if x == "batchmode" then
            exp3 = {tag = "SOME", payload = _L[222]}
          elseif x == "nonstopmode" then
            exp3 = {tag = "SOME", payload = _L[223]}
          elseif x == "scrollmode" then
            exp3 = {tag = "SOME", payload = _L[224]}
          elseif x == "errorstopmode" then
            exp3 = {tag = "SOME", payload = _L[225]}
          else
            exp3 = {tag = "NONE"}
          end
          if exp3.tag == "SOME" then
            local interaction = exp3.payload
            local tmp24 = opts.bibtex_or_biber
            local tmp25 = opts.change_directory
            local tmp26 = opts.check_driver
            local tmp27 = opts.color
            local tmp28 = opts.config_file
            local tmp29 = opts.dvipdfmx_extraoptions_rev
            local tmp30 = opts.engine
            local tmp31 = opts.engine_executable
            local tmp32 = opts.file_line_error
            local tmp33 = opts.fmt
            local tmp34 = opts.fresh
            local tmp35 = opts.halt_on_error
            local tmp36 = opts.includeonly
            local tmp37 = opts.jobname
            local tmp38 = opts.make_depends
            local tmp39 = opts.makeglossaries
            local tmp40 = opts.makeindex
            local tmp41 = opts.max_iterations
            local tmp42 = opts.output
            local tmp43 = opts.output_directory
            local tmp44 = opts.output_format
            local tmp45 = opts.package_support
            local tmp46 = opts.print_output_directory
            local tmp47 = opts.shell_escape
            local tmp48 = opts.source_date_epoch
            local tmp49 = opts.start_with_draft
            local tmp50 = opts.synctex
            local tmp51 = opts.tex_extraoptions_rev
            local tmp52 = opts.watch
            tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = {tag = "SOME", payload = interaction}, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
            tmp23 = args1
            goto cont13
          elseif exp3.tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:266:59")
          end
          _L[123]({_L[117], "invalid argument for --interaction\n"})
          exit(1, true)
        else
          goto else35
        end
      end
      ::else35::
      if exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:265:46")
      end
      _L[123]({_L[117], "multiple --interaction options\n"})
      exit(1, true)
    else
      goto else22
    end
    ::else22::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_HALT_ON_ERROR" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local tmp24 = opts.bibtex_or_biber
      local tmp25 = opts.change_directory
      local tmp26 = opts.check_driver
      local tmp27 = opts.color
      local tmp28 = opts.config_file
      local tmp29 = opts.dvipdfmx_extraoptions_rev
      local tmp30 = opts.engine
      local tmp31 = opts.engine_executable
      local tmp32 = opts.file_line_error
      local tmp33 = opts.fmt
      local tmp34 = opts.fresh
      local tmp35 = opts.includeonly
      local tmp36 = opts.interaction
      local tmp37 = opts.jobname
      local tmp38 = opts.make_depends
      local tmp39 = opts.makeglossaries
      local tmp40 = opts.makeindex
      local tmp41 = opts.max_iterations
      local tmp42 = opts.output
      local tmp43 = opts.output_directory
      local tmp44 = opts.output_format
      local tmp45 = opts.package_support
      local tmp46 = opts.print_output_directory
      local tmp47 = opts.shell_escape
      local tmp48 = opts.source_date_epoch
      local tmp49 = opts.start_with_draft
      local tmp50 = opts.synctex
      local tmp51 = opts.tex_extraoptions_rev
      tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = x, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = opts.watch}
      tmp23 = args1
      goto cont13
    elseif exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_SHELL_ESCAPE" then
      local se = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.shell_escape
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeglossaries
        local tmp41 = opts.makeindex
        local tmp42 = opts.max_iterations
        local tmp43 = opts.output
        local tmp44 = opts.output_directory
        local tmp45 = opts.output_format
        local tmp46 = opts.package_support
        local tmp47 = opts.print_output_directory
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = tmp47, shell_escape = {tag = "SOME", payload = se}, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:273:48")
      end
      _L[123]({_L[117], "multiple --(no-)shell-escape / --shell-restricted options\n"})
      exit(1, true)
    else
      goto else23
    end
    ::else23::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_JOBNAME" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.jobname
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = {tag = "SOME", payload = x}, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:277:42")
      end
      _L[123]({_L[117], "multiple --jobname options\n"})
      exit(1, true)
    else
      goto else24
    end
    ::else24::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_FMT" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.fmt
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fresh
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = {tag = "SOME", payload = x}, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:281:38")
      end
      _L[123]({_L[117], "multiple --fmt options\n"})
      exit(1, true)
    else
      goto else25
    end
    ::else25::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_OUTPUT_DIRECTORY" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.output_directory
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeglossaries
        local tmp41 = opts.makeindex
        local tmp42 = opts.max_iterations
        local tmp43 = opts.output
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = {tag = "SOME", payload = x}, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:285:51")
      end
      _L[123]({_L[117], "multiple --output-directory options\n"})
      exit(1, true)
    else
      goto else26
    end
    ::else26::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_OUTPUT_FORMAT" then
      local exp2
      do
        local format = exp1.payload[1].payload
        local args1 = exp1.payload[2]
        exp2 = opts.output_format
        if exp2.tag == "NONE" then
          local exp3
          if format == "pdf" then
            exp3 = {tag = "SOME", payload = _L[230]}
          elseif format == "dvi" then
            exp3 = {tag = "SOME", payload = _L[231]}
          else
            exp3 = {tag = "NONE"}
          end
          if exp3.tag == "SOME" then
            local format1 = exp3.payload
            local tmp24 = opts.bibtex_or_biber
            local tmp25 = opts.change_directory
            local tmp26 = opts.check_driver
            local tmp27 = opts.color
            local tmp28 = opts.config_file
            local tmp29 = opts.dvipdfmx_extraoptions_rev
            local tmp30 = opts.engine
            local tmp31 = opts.engine_executable
            local tmp32 = opts.file_line_error
            local tmp33 = opts.fmt
            local tmp34 = opts.fresh
            local tmp35 = opts.halt_on_error
            local tmp36 = opts.includeonly
            local tmp37 = opts.interaction
            local tmp38 = opts.jobname
            local tmp39 = opts.make_depends
            local tmp40 = opts.makeglossaries
            local tmp41 = opts.makeindex
            local tmp42 = opts.max_iterations
            local tmp43 = opts.output
            local tmp44 = opts.output_directory
            local tmp45 = opts.package_support
            local tmp46 = opts.print_output_directory
            local tmp47 = opts.shell_escape
            local tmp48 = opts.source_date_epoch
            local tmp49 = opts.start_with_draft
            local tmp50 = opts.synctex
            local tmp51 = opts.tex_extraoptions_rev
            local tmp52 = opts.watch
            tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = {tag = "SOME", payload = format1}, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
            tmp23 = args1
            goto cont13
          elseif exp3.tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:290:66")
          end
          _L[123]({_L[117], "invalid value for --output-format option\n"})
          exit(1, true)
        else
          goto else35
        end
      end
      ::else35::
      if exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:289:53")
      end
      _L[123]({_L[117], "multiple --output-format options\n"})
      exit(1, true)
    else
      goto else27
    end
    ::else27::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_TEX_OPTION" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local x1 = _L[207](x)
      local tmp24 = opts.bibtex_or_biber
      local tmp25 = opts.change_directory
      local tmp26 = opts.check_driver
      local tmp27 = opts.color
      local tmp28 = opts.config_file
      local tmp29 = opts.dvipdfmx_extraoptions_rev
      local tmp30 = opts.engine
      local tmp31 = opts.engine_executable
      local tmp32 = opts.file_line_error
      local tmp33 = opts.fmt
      local tmp34 = opts.fresh
      local tmp35 = opts.halt_on_error
      local tmp36 = opts.includeonly
      local tmp37 = opts.interaction
      local tmp38 = opts.jobname
      local tmp39 = opts.make_depends
      local tmp40 = opts.makeglossaries
      local tmp41 = opts.makeindex
      local tmp42 = opts.max_iterations
      local tmp43 = opts.output
      local tmp44 = opts.output_directory
      local tmp45 = opts.output_format
      local tmp46 = opts.package_support
      local tmp47 = opts.print_output_directory
      local tmp48 = opts.shell_escape
      local tmp49 = opts.source_date_epoch
      local tmp50 = opts.start_with_draft
      local tmp51 = opts.synctex
      local tmp52 = opts.watch
      tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = tmp47, shell_escape = tmp48, source_date_epoch = tmp49, start_with_draft = tmp50, synctex = tmp51, tex_extraoptions_rev = {x1, opts.tex_extraoptions_rev}, watch = tmp52}
      tmp23 = args1
      goto cont13
    else
      goto else28
    end
    ::else28::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_TEX_OPTIONS" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local tmp24 = opts.bibtex_or_biber
      local tmp25 = opts.change_directory
      local tmp26 = opts.check_driver
      local tmp27 = opts.color
      local tmp28 = opts.config_file
      local tmp29 = opts.dvipdfmx_extraoptions_rev
      local tmp30 = opts.engine
      local tmp31 = opts.engine_executable
      local tmp32 = opts.file_line_error
      local tmp33 = opts.fmt
      local tmp34 = opts.fresh
      local tmp35 = opts.halt_on_error
      local tmp36 = opts.includeonly
      local tmp37 = opts.interaction
      local tmp38 = opts.jobname
      local tmp39 = opts.make_depends
      local tmp40 = opts.makeglossaries
      local tmp41 = opts.makeindex
      local tmp42 = opts.max_iterations
      local tmp43 = opts.output
      local tmp44 = opts.output_directory
      local tmp45 = opts.output_format
      local tmp46 = opts.package_support
      local tmp47 = opts.print_output_directory
      local tmp48 = opts.shell_escape
      local tmp49 = opts.source_date_epoch
      local tmp50 = opts.start_with_draft
      local tmp51 = opts.synctex
      local tmp52 = opts.watch
      tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = tmp41, max_iterations = tmp42, output = tmp43, output_directory = tmp44, output_format = tmp45, package_support = tmp46, print_output_directory = tmp47, shell_escape = tmp48, source_date_epoch = tmp49, start_with_draft = tmp50, synctex = tmp51, tex_extraoptions_rev = {x, opts.tex_extraoptions_rev}, watch = tmp52}
      tmp23 = args1
      goto cont13
    elseif exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_DVIPDFMX_OPTION" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local x1 = _L[207](x)
      local tmp24 = opts.bibtex_or_biber
      local tmp25 = opts.change_directory
      local tmp26 = opts.check_driver
      local tmp27 = opts.color
      local tmp28 = opts.config_file
      local tmp29 = opts.engine
      local tmp30 = opts.engine_executable
      local tmp31 = opts.file_line_error
      local tmp32 = opts.fmt
      local tmp33 = opts.fresh
      local tmp34 = opts.halt_on_error
      local tmp35 = opts.includeonly
      local tmp36 = opts.interaction
      local tmp37 = opts.jobname
      local tmp38 = opts.make_depends
      local tmp39 = opts.makeglossaries
      local tmp40 = opts.makeindex
      local tmp41 = opts.max_iterations
      local tmp42 = opts.output
      local tmp43 = opts.output_directory
      local tmp44 = opts.output_format
      local tmp45 = opts.package_support
      local tmp46 = opts.print_output_directory
      local tmp47 = opts.shell_escape
      local tmp48 = opts.source_date_epoch
      local tmp49 = opts.start_with_draft
      local tmp50 = opts.synctex
      local tmp51 = opts.tex_extraoptions_rev
      local tmp52 = opts.watch
      tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = {x1, opts.dvipdfmx_extraoptions_rev}, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
      tmp23 = args1
      goto cont13
    else
      goto else29
    end
    ::else29::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_DVIPDFMX_OPTIONS" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local tmp24 = opts.bibtex_or_biber
      local tmp25 = opts.change_directory
      local tmp26 = opts.check_driver
      local tmp27 = opts.color
      local tmp28 = opts.config_file
      local tmp29 = opts.engine
      local tmp30 = opts.engine_executable
      local tmp31 = opts.file_line_error
      local tmp32 = opts.fmt
      local tmp33 = opts.fresh
      local tmp34 = opts.halt_on_error
      local tmp35 = opts.includeonly
      local tmp36 = opts.interaction
      local tmp37 = opts.jobname
      local tmp38 = opts.make_depends
      local tmp39 = opts.makeglossaries
      local tmp40 = opts.makeindex
      local tmp41 = opts.max_iterations
      local tmp42 = opts.output
      local tmp43 = opts.output_directory
      local tmp44 = opts.output_format
      local tmp45 = opts.package_support
      local tmp46 = opts.print_output_directory
      local tmp47 = opts.shell_escape
      local tmp48 = opts.source_date_epoch
      local tmp49 = opts.start_with_draft
      local tmp50 = opts.synctex
      local tmp51 = opts.tex_extraoptions_rev
      local tmp52 = opts.watch
      tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = {x, opts.dvipdfmx_extraoptions_rev}, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
      tmp23 = args1
      goto cont13
    elseif exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_MAKEINDEX" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.makeindex
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeglossaries
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = tmp40, makeindex = {tag = "SOME", payload = x}, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:304:44")
      end
      _L[123]({_L[117], "multiple --makeindex options\n"})
      exit(1, true)
    else
      goto else30
    end
    ::else30::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_BIBTEX" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.bibtex_or_biber
      if exp2.tag == "NONE" then
        local tmp24 = opts.change_directory
        local tmp25 = opts.check_driver
        local tmp26 = opts.color
        local tmp27 = opts.config_file
        local tmp28 = opts.dvipdfmx_extraoptions_rev
        local tmp29 = opts.engine
        local tmp30 = opts.engine_executable
        local tmp31 = opts.file_line_error
        local tmp32 = opts.fmt
        local tmp33 = opts.fresh
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = {tag = "SOME", payload = {tag = "BIBTEX", payload = x}}, change_directory = tmp24, check_driver = tmp25, color = tmp26, config_file = tmp27, dvipdfmx_extraoptions_rev = tmp28, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:308:41")
      end
      _L[123]({_L[117], "multiple --bibtex / --biber options\n"})
      exit(1, true)
    else
      goto else31
    end
    ::else31::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_BIBER" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.bibtex_or_biber
      if exp2.tag == "NONE" then
        local tmp24 = opts.change_directory
        local tmp25 = opts.check_driver
        local tmp26 = opts.color
        local tmp27 = opts.config_file
        local tmp28 = opts.dvipdfmx_extraoptions_rev
        local tmp29 = opts.engine
        local tmp30 = opts.engine_executable
        local tmp31 = opts.file_line_error
        local tmp32 = opts.fmt
        local tmp33 = opts.fresh
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = {tag = "SOME", payload = {tag = "BIBER", payload = x}}, change_directory = tmp24, check_driver = tmp25, color = tmp26, config_file = tmp27, dvipdfmx_extraoptions_rev = tmp28, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:312:40")
      end
      _L[123]({_L[117], "multiple --bibtex / --biber options\n"})
      exit(1, true)
    else
      goto else32
    end
    ::else32::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_MAKEGLOSSARIES" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.makeglossaries
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.config_file
        local tmp29 = opts.dvipdfmx_extraoptions_rev
        local tmp30 = opts.engine
        local tmp31 = opts.engine_executable
        local tmp32 = opts.file_line_error
        local tmp33 = opts.fmt
        local tmp34 = opts.fresh
        local tmp35 = opts.halt_on_error
        local tmp36 = opts.includeonly
        local tmp37 = opts.interaction
        local tmp38 = opts.jobname
        local tmp39 = opts.make_depends
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = tmp28, dvipdfmx_extraoptions_rev = tmp29, engine = tmp30, engine_executable = tmp31, file_line_error = tmp32, fmt = tmp33, fresh = tmp34, halt_on_error = tmp35, includeonly = tmp36, interaction = tmp37, jobname = tmp38, make_depends = tmp39, makeglossaries = {tag = "SOME", payload = x}, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:316:49")
      end
      _L[123]({_L[117], "multiple --makeglossaries options\n"})
      exit(1, true)
    else
      goto else33
    end
    ::else33::
    if exp1.tag == "SOME" and exp1.payload[1].tag == "OPT_CONFIG_FILE" then
      local x = exp1.payload[1].payload
      local args1 = exp1.payload[2]
      local exp2 = opts.config_file
      if exp2.tag == "NONE" then
        local tmp24 = opts.bibtex_or_biber
        local tmp25 = opts.change_directory
        local tmp26 = opts.check_driver
        local tmp27 = opts.color
        local tmp28 = opts.dvipdfmx_extraoptions_rev
        local tmp29 = opts.engine
        local tmp30 = opts.engine_executable
        local tmp31 = opts.file_line_error
        local tmp32 = opts.fmt
        local tmp33 = opts.fresh
        local tmp34 = opts.halt_on_error
        local tmp35 = opts.includeonly
        local tmp36 = opts.interaction
        local tmp37 = opts.jobname
        local tmp38 = opts.make_depends
        local tmp39 = opts.makeglossaries
        local tmp40 = opts.makeindex
        local tmp41 = opts.max_iterations
        local tmp42 = opts.output
        local tmp43 = opts.output_directory
        local tmp44 = opts.output_format
        local tmp45 = opts.package_support
        local tmp46 = opts.print_output_directory
        local tmp47 = opts.shell_escape
        local tmp48 = opts.source_date_epoch
        local tmp49 = opts.start_with_draft
        local tmp50 = opts.synctex
        local tmp51 = opts.tex_extraoptions_rev
        local tmp52 = opts.watch
        tmp22 = {bibtex_or_biber = tmp24, change_directory = tmp25, check_driver = tmp26, color = tmp27, config_file = {tag = "SOME", payload = x}, dvipdfmx_extraoptions_rev = tmp28, engine = tmp29, engine_executable = tmp30, file_line_error = tmp31, fmt = tmp32, fresh = tmp33, halt_on_error = tmp34, includeonly = tmp35, interaction = tmp36, jobname = tmp37, make_depends = tmp38, makeglossaries = tmp39, makeindex = tmp40, max_iterations = tmp41, output = tmp42, output_directory = tmp43, output_format = tmp44, package_support = tmp45, print_output_directory = tmp46, shell_escape = tmp47, source_date_epoch = tmp48, start_with_draft = tmp49, synctex = tmp50, tex_extraoptions_rev = tmp51, watch = tmp52}
        tmp23 = args1
        goto cont13
      elseif exp2.tag == "SOME" then
      else
        _raise(_Match, "handle-options.sml:320:46")
      end
      _L[123]({_L[117], "multiple --config-file options\n"})
      exit(1, true)
    else
      goto else34
    end
    ::else34::
    if exp1.tag == "NONE" then
      if args ~= nil and args[1] == "--" then
        exp = {opts, args[2]}
        goto cont
      elseif args ~= nil then
        local tmp24 = args[1]
        local tmp25 = isPrefix("-")
        local tmp26 = tmp25(tmp24)
        if tmp26 then
        else
          exp = {opts, args}
          goto cont
        end
        _L[123]({_L[117], "Unrecognized option: " .. tmp24 .. ".\n\n"})
        exit(1, true)
      elseif args == nil then
        exp = {opts, args}
        goto cont
      else
        _raise(_Match, "handle-options.sml:324:20")
      end
    else
      _raise(_Match, "handle-options.sml:168:7")
    end
  end
  ::cont::
  local options, rest, config
  do
    options = exp[1]
    rest = exp[2]
    local tmp21 = options.config_file
    local exp1
    if tmp21.tag == "SOME" then
      exp1 = {tag = "SOME", payload = tmp21.payload}
      goto cont13
    elseif tmp21.tag == "NONE" then
      local exp2
      do
        local result = getenv("CLUTTEX_CONFIG_FILE")
        if result == nil then
          exp2 = {tag = "NONE"}
        else
          exp2 = {tag = "SOME", payload = result}
        end
      end
      if exp2.tag == "SOME" then
        exp1 = {tag = "SOME", payload = exp2.payload}
        goto cont13
      elseif exp2.tag == "NONE" then
        if _L[220] then
          local exp3
          do
            local result = getenv("APPDATA")
            if result == nil then
              exp3 = {tag = "NONE"}
            else
              exp3 = {tag = "SOME", payload = result}
            end
          end
          if exp3.tag == "SOME" then
            exp1 = {tag = "SOME", payload = exp3.payload .. "\\cluttex\\config.toml"}
            goto cont13
          elseif exp3.tag == "NONE" then
            exp1 = {tag = "NONE"}
            goto cont13
          else
            _raise(_Match, "main.sml:768:45")
          end
        end
        local exp3
        do
          local result = getenv("XDG_CONFIG_HOME")
          if result == nil then
            exp3 = {tag = "NONE"}
          else
            exp3 = {tag = "SOME", payload = result}
          end
        end
        if exp3.tag == "SOME" then
          exp1 = {tag = "SOME", payload = exp3.payload .. "/cluttex/config.toml"}
          goto cont13
        elseif exp3.tag == "NONE" then
          local exp4
          do
            local result = getenv("HOME")
            if result == nil then
              exp4 = {tag = "NONE"}
            else
              exp4 = {tag = "SOME", payload = result}
            end
          end
          if exp4.tag == "SOME" then
            exp1 = {tag = "SOME", payload = exp4.payload .. "/.config/cluttex/config.toml"}
          elseif exp4.tag == "NONE" then
            exp1 = {tag = "NONE"}
          else
            _raise(_Match, "main.sml:774:57")
          end
        else
          _raise(_Match, "main.sml:772:45")
        end
      else
        _raise(_Match, "main.sml:765:30")
      end
    else
      _raise(_Match, "main.sml:764:5")
    end
    ::cont13::
    if exp1.tag == "NONE" then
      config = _L[398]
      goto cont1
    elseif exp1.tag == "SOME" then
      local path = exp1.payload
      local status, exn = pcall(function()
        local tmp22, tmp23, expect, skipWhiteSpace, skipWhiteSpaceAndGetc, skipUntilNewline, skipWhiteSpaceOrComment, skipWhiteSpaceOrCommentOrNewlineAndGetc, skipOptionalNewline, go, checkAllowedChar, go1, go2, readHexInt, readOctInt, readBinInt, readSigned, readUnsigned
        do
          local ins = _L[114](path)
          tmp22 = _L[121](ins)
          tmp23 = function(strm, state)
            local tmp24 = _L[93](strm)
            if state == "START" and tmp24.tag == "NONE" then
              return {tag = "NONE"}
            elseif tmp24.tag == "NONE" then
              _raise(_L[172], "validate_utf8.sml:67:22")
            elseif tmp24.tag == "SOME" then
              local c = tmp24.payload[1]
              local strm1 = tmp24.payload[2]
              local tmp25
              if state == "START" then
                if c < 128 then
                  tmp25 = _L[173]
                  goto cont
                elseif c < 224 then
                  if 194 <= c then
                    tmp25 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:30:55")
                  end
                elseif c == 224 then
                  tmp25 = _L[174]
                  goto cont
                elseif c == 237 then
                  tmp25 = _L[175]
                  goto cont
                elseif c < 240 then
                  tmp25 = _L[179]
                  goto cont
                elseif c == 240 then
                  tmp25 = _L[176]
                  goto cont
                elseif c < 244 then
                  tmp25 = _L[180]
                  goto cont
                elseif c == 244 then
                  tmp25 = _L[177]
                  goto cont
                else
                  _raise(_L[172], "validate_utf8.sml:44:11")
                end
              elseif state == "TAIL_1" then
                if 128 <= c then
                  if c < 192 then
                    tmp25 = _L[173]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:47:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:47:14")
                end
              elseif state == "MID_1_OF_3_E0" then
                if 160 <= c then
                  if c < 192 then
                    tmp25 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:50:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:50:14")
                end
              elseif state == "MID_1_OF_3_ED" then
                if 128 <= c then
                  if c < 160 then
                    tmp25 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:53:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:53:14")
                end
              elseif state == "TAIL_2" then
                if 128 <= c then
                  if c < 192 then
                    tmp25 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:56:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:56:14")
                end
              elseif state == "MID_1_OF_4_F0" then
                if 144 <= c then
                  if c < 192 then
                    tmp25 = _L[179]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:59:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:59:14")
                end
              elseif state == "TAIL_3" then
                if 128 <= c then
                  if c < 192 then
                    tmp25 = _L[179]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:62:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:62:14")
                end
              elseif state == "MID_1_OF_4_F4" then
                if 128 <= c then
                  if c < 144 then
                    tmp25 = _L[179]
                  else
                    _raise(_L[172], "validate_utf8.sml:65:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:65:14")
                end
              else
                _raise(_Match, "validate_utf8.sml:26:7")
              end
              ::cont::
              return {tag = "SOME", payload = {c, {strm1, tmp25}}}
            else
              _raise(_Match, "validate_utf8.sml:66:7")
            end
          end
          expect = function(c, strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              local tmp24 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = tmp24}}}, _exn_meta), "parse_toml.sml:16:5")
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" then
              local c_PRIME = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if c == c_PRIME then
                return strm_PRIME
              end
              local tmp24 = toString(c)
              local tmp25 = toString(c_PRIME)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp25, expected = tmp24}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:28:9")
            end
          end
          skipWhiteSpace = function(strm)
            local tmp24 = strm
            ::cont::
            local strm1 = tmp24
            local exp2
            do
              local strm2 = strm1[1]
              exp2 = tmp23(strm2, strm1[2])
            end
            if exp2.tag == "NONE" then
              return strm1
            elseif exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if c == 32 then
                tmp24 = strm_PRIME
                goto cont
              elseif c == 9 then
                tmp24 = strm_PRIME
                goto cont
              else
                return strm1
              end
            else
              _raise(_Match, "parse_toml.sml:34:9")
            end
          end
          skipWhiteSpaceAndGetc = function(strm, state)
            local tmp24, tmp25 = strm, state
            ::cont::
            local strm1, state1 = tmp24, tmp25
            local r = tmp23(strm1, state1)
            if r.tag == "SOME" and r.payload[1] == 32 then
              local strm_PRIME = r.payload[2]
              local strm2 = strm_PRIME[1]
              tmp24 = strm2
              tmp25 = strm_PRIME[2]
              goto cont
            elseif r.tag == "SOME" and r.payload[1] == 9 then
              local strm_PRIME = r.payload[2]
              local strm2 = strm_PRIME[1]
              tmp24 = strm2
              tmp25 = strm_PRIME[2]
              goto cont
            else
              return r
            end
          end
          skipUntilNewline = function(strm)
            local tmp24 = strm
            ::cont::
            local exp2
            do
              local strm1 = tmp24
              do
                local strm2 = strm1[1]
                exp2 = tmp23(strm2, strm1[2])
              end
              if exp2.tag == "NONE" then
                return strm1
              elseif exp2.tag == "SOME" and exp2.payload[1] == 10 then
                return exp2.payload[2]
              elseif exp2.tag == "SOME" and exp2.payload[1] == 13 then
                local strm_PRIME = exp2.payload[2]
                local strm2 = strm_PRIME[1]
                return expect(10, strm2, strm_PRIME[2])
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              local tmp25
              if c <= 127 then
                local tmp26 = isPrint(c)
                tmp25 = not tmp26
              else
                tmp25 = false
                goto cont1
              end
              ::cont1::
              if tmp25 then
                local tmp26 = toString(c)
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp26, expected = "printable character"}}}, _exn_meta), "parse_toml.sml:19:5")
              else
                tmp24 = strm_PRIME
                goto cont
              end
            else
              _raise(_Match, "parse_toml.sml:47:9")
            end
          end
          skipWhiteSpaceOrComment = function(strm)
            local tmp24 = strm
            ::cont::
            local exp2
            do
              local strm1 = tmp24
              do
                local strm2 = strm1[1]
                exp2 = tmp23(strm2, strm1[2])
              end
              if exp2.tag == "NONE" then
                return strm1
              elseif exp2.tag == "SOME" and exp2.payload[1] == 35 then
                return skipUntilNewline(exp2.payload[2])
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 32 then
              tmp24 = exp2.payload[2]
              goto cont
            elseif exp2.tag == "SOME" and exp2.payload[1] == 9 then
              tmp24 = exp2.payload[2]
              goto cont
            elseif exp2.tag == "SOME" and exp2.payload[1] == 13 then
              local strm_PRIME = exp2.payload[2]
              local strm1 = strm_PRIME[1]
              return expect(10, strm1, strm_PRIME[2])
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" and exp2.payload[1] == 10 then
              return exp2.payload[2]
            elseif exp2.tag == "SOME" then
              local tmp25 = toString(exp2.payload[1])
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp25, expected = "whitespace or comment"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:56:9")
            end
          end
          skipWhiteSpaceOrCommentOrNewlineAndGetc = function(strm)
            local tmp24 = strm
            ::cont::
            local strm1 = tmp24
            local r
            do
              local strm2 = strm1[1]
              r = tmp23(strm2, strm1[2])
            end
            if r.tag == "SOME" and r.payload[1] == 35 then
            elseif r.tag == "SOME" and r.payload[1] == 32 then
              tmp24 = r.payload[2]
              goto cont
            elseif r.tag == "SOME" and r.payload[1] == 9 then
              tmp24 = r.payload[2]
              goto cont
            elseif r.tag == "SOME" and r.payload[1] == 13 then
              local strm_PRIME = r.payload[2]
              local strm2 = strm_PRIME[1]
              tmp24 = expect(10, strm2, strm_PRIME[2])
              goto cont
            elseif r.tag == "SOME" and r.payload[1] == 10 then
              tmp24 = r.payload[2]
              goto cont
            else
              return r
            end
            tmp24 = skipUntilNewline(r.payload[2])
            goto cont
          end
          skipOptionalNewline = function(strm)
            local exp2
            do
              local strm1 = strm[1]
              exp2 = tmp23(strm1, strm[2])
            end
            if exp2.tag == "SOME" and exp2.payload[1] == 10 then
              return exp2.payload[2]
            elseif exp2.tag == "SOME" and exp2.payload[1] == 13 then
              local strm_PRIME = exp2.payload[2]
              local strm1 = strm_PRIME[1]
              return expect(10, strm1, strm_PRIME[2])
            else
              return strm
            end
          end
          local skipWhiteSpaceOrNewline = function(strm)
            local tmp24 = strm
            ::cont::
            local strm1 = tmp24
            local exp2
            do
              local strm2 = strm1[1]
              exp2 = tmp23(strm2, strm1[2])
            end
            if exp2.tag == "NONE" then
              return strm1
            elseif exp2.tag == "SOME" and exp2.payload[1] == 13 then
              local strm_PRIME = exp2.payload[2]
              local strm2 = strm_PRIME[1]
              tmp24 = expect(10, strm2, strm_PRIME[2])
              goto cont
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if c == 32 then
                tmp24 = strm_PRIME
                goto cont
              elseif c == 9 then
                tmp24 = strm_PRIME
                goto cont
              elseif c == 10 then
                tmp24 = strm_PRIME
                goto cont
              else
                return strm1
              end
            else
              _raise(_Match, "parse_toml.sml:83:9")
            end
          end
          local readHexDigit = function(strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "hexadecimal digit"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if 48 <= c and c <= 57 then
                return {_Int_sub(c, 48), strm_PRIME}
              elseif 65 <= c and c <= 70 then
                return {_Int_sub(c, 55), strm_PRIME}
              elseif 97 <= c and c <= 102 then
                return {_Int_sub(c, 87), strm_PRIME}
              end
              local tmp24 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "hexadecimal digit"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:93:9")
            end
          end
          local readFourHexDigit = function(strm, state)
            local exp2 = readHexDigit(strm, state)
            local c3 = exp2[1]
            local strm1 = exp2[2]
            local exp3
            do
              local strm2 = strm1[1]
              exp3 = readHexDigit(strm2, strm1[2])
            end
            local c2 = exp3[1]
            local strm2 = exp3[2]
            local exp4
            do
              local strm3 = strm2[1]
              exp4 = readHexDigit(strm3, strm2[2])
            end
            local c1 = exp4[1]
            local strm3 = exp4[2]
            local exp5
            do
              local strm4 = strm3[1]
              exp5 = readHexDigit(strm4, strm3[2])
            end
            local c0 = exp5[1]
            local strm4 = exp5[2]
            return {_Int_add(_Int_mul(_Int_add(_Int_mul(_Int_add(_Int_mul(c3, 16), c2), 16), c1), 16), c0), strm4}
          end
          go = function(accum, strm, state)
            local tmp24, tmp25, tmp26 = accum, strm, state
            ::cont::
            local accum1, exp2
            do
              local strm1, state1
              accum1, strm1, state1 = tmp24, tmp25, tmp26
              exp2 = tmp23(strm1, state1)
              if exp2.tag == "NONE" then
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, _exn_meta), "parse_toml.sml:16:5")
              elseif exp2.tag == "SOME" and exp2.payload[1] == 34 then
                local strm_PRIME = exp2.payload[2]
                local tmp27 = implodeRev(accum1)
                return {tmp27, strm_PRIME}
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 92 then
              local exp3
              do
                local strm_PRIME = exp2.payload[2]
                do
                  local strm1 = strm_PRIME[1]
                  exp3 = tmp23(strm1, strm_PRIME[2])
                end
                if exp3.tag == "NONE" then
                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "escape sequence"}}}, _exn_meta), "parse_toml.sml:16:5")
                elseif exp3.tag == "SOME" and exp3.payload[1] == 34 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp27 = {34, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME[2]
                  goto cont
                elseif exp3.tag == "SOME" and exp3.payload[1] == 92 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp27 = {92, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME[2]
                  goto cont
                elseif exp3.tag == "SOME" and exp3.payload[1] == 98 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp27 = {8, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME[2]
                  goto cont
                elseif exp3.tag == "SOME" and exp3.payload[1] == 102 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp27 = {12, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME[2]
                  goto cont
                elseif exp3.tag == "SOME" and exp3.payload[1] == 110 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp27 = {10, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME[2]
                  goto cont
                elseif exp3.tag == "SOME" and exp3.payload[1] == 114 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp27 = {13, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME[2]
                  goto cont
                elseif exp3.tag == "SOME" and exp3.payload[1] == 116 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp27 = {9, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME[2]
                  goto cont
                elseif exp3.tag == "SOME" and exp3.payload[1] == 117 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local exp4
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    exp4 = readFourHexDigit(strm1, strm_PRIME_PRIME[2])
                  end
                  local i = exp4[1]
                  local strm_PRIME_PRIME_PRIME = exp4[2]
                  local tmp27 = _L[187](i, accum1)
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  tmp24 = tmp27
                  tmp25 = strm1
                  tmp26 = strm_PRIME_PRIME_PRIME[2]
                  goto cont
                else
                  goto else3
                end
              end
              ::else3::
              if exp3.tag == "SOME" and exp3.payload[1] == 85 then
                local strm_PRIME_PRIME = exp3.payload[2]
                local exp4
                do
                  local strm1 = strm_PRIME_PRIME[1]
                  exp4 = readFourHexDigit(strm1, strm_PRIME_PRIME[2])
                end
                local hi = exp4[1]
                local strm_PRIME_PRIME_PRIME = exp4[2]
                local exp5
                do
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  exp5 = readFourHexDigit(strm1, strm_PRIME_PRIME_PRIME[2])
                end
                local lo = exp5[1]
                local strm_PRIME_PRIME_PRIME_PRIME = exp5[2]
                if hi > 16 then
                  _raise(setmetatable({tag = _L[186], payload = _L[182]}, _exn_meta), "parse_toml.sml:139:24")
                end
                local tmp27 = _Int_add(_Int_mul(hi, 65536), lo)
                local tmp28 = _L[187](tmp27, accum1)
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                tmp24 = tmp28
                tmp25 = strm1
                tmp26 = strm_PRIME_PRIME_PRIME_PRIME[2]
                goto cont
              else
                goto else4
              end
              ::else4::
              if exp3.tag == "SOME" then
                local tmp27 = toString(exp3.payload[1])
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp27, expected = "escape sequence"}}}, _exn_meta), "parse_toml.sml:19:5")
              else
                _raise(_Match, "parse_toml.sml:120:16")
              end
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              local tmp27 = c < 32 and c ~= 9
              local tmp28
              tmp28 = tmp27 or c == 127
              if tmp28 then
                local tmp29 = toString(c)
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp29, expected = "printable character"}}}, _exn_meta), "parse_toml.sml:19:5")
              else
                local tmp29 = {c, accum1}
                local strm1 = strm_PRIME[1]
                tmp24 = tmp29
                tmp25 = strm1
                tmp26 = strm_PRIME[2]
                goto cont
              end
            else
              _raise(_Match, "parse_toml.sml:116:11")
            end
          end
          checkAllowedChar = function(c)
            local tmp24 = c < 32 and (c ~= 9 and c ~= 10)
            local tmp25
            tmp25 = tmp24 or c == 127
            if tmp25 then
              local tmp26 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp26, expected = "printable character"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              return c
            end
          end
          local function escape(accum, strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "escape sequence"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif exp2.tag == "SOME" and exp2.payload[1] == 34 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = {34, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME[2])
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 92 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = {92, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME[2])
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" and exp2.payload[1] == 98 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = {8, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME[2])
            else
              goto else3
            end
            ::else3::
            if exp2.tag == "SOME" and exp2.payload[1] == 102 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = {12, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME[2])
            else
              goto else4
            end
            ::else4::
            if exp2.tag == "SOME" and exp2.payload[1] == 110 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = {10, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME[2])
            else
              goto else5
            end
            ::else5::
            if exp2.tag == "SOME" and exp2.payload[1] == 114 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = {13, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME[2])
            else
              goto else6
            end
            ::else6::
            if exp2.tag == "SOME" and exp2.payload[1] == 116 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = {9, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME[2])
            else
              goto else7
            end
            ::else7::
            if exp2.tag == "SOME" and exp2.payload[1] == 117 then
              local strm_PRIME = exp2.payload[2]
              local exp3
              do
                local strm1 = strm_PRIME[1]
                exp3 = readFourHexDigit(strm1, strm_PRIME[2])
              end
              local i = exp3[1]
              local strm_PRIME_PRIME = exp3[2]
              local tmp24 = _L[187](i, accum)
              local strm1 = strm_PRIME_PRIME[1]
              return go1(tmp24, strm1, strm_PRIME_PRIME[2])
            else
              goto else8
            end
            ::else8::
            if exp2.tag == "SOME" and exp2.payload[1] == 85 then
              local strm_PRIME = exp2.payload[2]
              local exp3
              do
                local strm1 = strm_PRIME[1]
                exp3 = readFourHexDigit(strm1, strm_PRIME[2])
              end
              local hi = exp3[1]
              local strm_PRIME_PRIME = exp3[2]
              local exp4
              do
                local strm1 = strm_PRIME_PRIME[1]
                exp4 = readFourHexDigit(strm1, strm_PRIME_PRIME[2])
              end
              local lo = exp4[1]
              local strm_PRIME_PRIME_PRIME = exp4[2]
              local tmp24 = _Int_add(_Int_mul(hi, 65536), lo)
              local tmp25 = _L[187](tmp24, accum)
              local strm1 = strm_PRIME_PRIME_PRIME[1]
              return go1(tmp25, strm1, strm_PRIME_PRIME_PRIME[2])
            else
              goto else9
            end
            ::else9::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if c == 32 or c == 9 then
                local exp3
                do
                  local strm1 = strm_PRIME[1]
                  exp3 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
                end
                if exp3.tag == "NONE" then
                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "newline"}}}, _exn_meta), "parse_toml.sml:16:5")
                elseif exp3.tag == "SOME" and exp3.payload[1] == 10 then
                  local tmp24 = skipWhiteSpaceOrNewline(exp3.payload[2])
                  local strm1 = tmp24[1]
                  return go1(accum, strm1, tmp24[2])
                else
                  goto else13
                end
                ::else13::
                if exp3.tag == "SOME" and exp3.payload[1] == 13 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp24
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp24 = expect(10, strm1, strm_PRIME_PRIME[2])
                  end
                  local tmp25 = skipWhiteSpaceOrNewline(tmp24)
                  local strm1 = tmp25[1]
                  return go1(accum, strm1, tmp25[2])
                else
                  goto else14
                end
                ::else14::
                if exp3.tag == "SOME" then
                  local tmp24 = toString(exp3.payload[1])
                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "newline"}}}, _exn_meta), "parse_toml.sml:19:5")
                else
                  _raise(_Match, "parse_toml.sml:229:17")
                end
              else
                goto else10
              end
              ::else10::
              if c == 13 then
                local tmp24
                do
                  local strm1 = strm_PRIME[1]
                  tmp24 = expect(10, strm1, strm_PRIME[2])
                end
                local tmp25 = skipWhiteSpaceOrNewline(tmp24)
                local strm1 = tmp25[1]
                return go1(accum, strm1, tmp25[2])
              else
                goto else11
              end
              ::else11::
              if c == 10 then
                local tmp24 = skipWhiteSpaceOrNewline(strm_PRIME)
                local strm1 = tmp24[1]
                return go1(accum, strm1, tmp24[2])
              else
                goto else12
              end
              ::else12::
              local tmp24 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "escape sequence"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:204:11")
            end
          end
          go1 = function(accum, strm, state)
            local tmp24, tmp25, tmp26 = accum, strm, state
            ::cont::
            local accum1, exp2
            do
              local strm1, state1
              accum1, strm1, state1 = tmp24, tmp25, tmp26
              exp2 = tmp23(strm1, state1)
              if exp2.tag == "NONE" then
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, _exn_meta), "parse_toml.sml:16:5")
              elseif exp2.tag == "SOME" and exp2.payload[1] == 92 then
                local strm_PRIME = exp2.payload[2]
                local strm2 = strm_PRIME[1]
                return escape(accum1, strm2, strm_PRIME[2])
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 34 then
              local exp3
              do
                local strm_PRIME = exp2.payload[2]
                do
                  local strm1 = strm_PRIME[1]
                  exp3 = tmp23(strm1, strm_PRIME[2])
                end
                if exp3.tag == "SOME" and exp3.payload[1] == 34 then
                  local exp4
                  do
                    local strm_PRIME_PRIME = exp3.payload[2]
                    do
                      local strm1 = strm_PRIME_PRIME[1]
                      exp4 = tmp23(strm1, strm_PRIME_PRIME[2])
                    end
                    if exp4.tag == "SOME" and exp4.payload[1] == 34 then
                      local strm_PRIME_PRIME_PRIME
                      do
                        strm_PRIME_PRIME_PRIME = exp4.payload[2]
                        local exp5
                        do
                          local strm1 = strm_PRIME_PRIME_PRIME[1]
                          exp5 = tmp23(strm1, strm_PRIME_PRIME_PRIME[2])
                        end
                        if exp5.tag == "SOME" and exp5.payload[1] == 34 then
                          local strm_PRIME_PRIME_PRIME_PRIME
                          do
                            strm_PRIME_PRIME_PRIME_PRIME = exp5.payload[2]
                            local exp6
                            do
                              local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                              exp6 = tmp23(strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                            end
                            if exp6.tag == "SOME" and exp6.payload[1] == 34 then
                              local strm_PRIME_PRIME_PRIME_PRIME_PRIME = exp6.payload[2]
                              local tmp27 = implodeRev({34, {34, accum1}})
                              return {tmp27, strm_PRIME_PRIME_PRIME_PRIME_PRIME}
                            else
                              goto else11
                            end
                          end
                          ::else11::
                          local tmp27 = implodeRev({34, accum1})
                          return {tmp27, strm_PRIME_PRIME_PRIME_PRIME}
                        else
                          goto else10
                        end
                      end
                      ::else10::
                      local tmp27 = implodeRev(accum1)
                      return {tmp27, strm_PRIME_PRIME_PRIME}
                    else
                      goto else7
                    end
                  end
                  ::else7::
                  if exp4.tag == "SOME" and exp4.payload[1] == 92 then
                    local strm_PRIME_PRIME_PRIME = exp4.payload[2]
                    local tmp27 = {34, {34, accum1}}
                    local strm1 = strm_PRIME_PRIME_PRIME[1]
                    return escape(tmp27, strm1, strm_PRIME_PRIME_PRIME[2])
                  else
                    goto else8
                  end
                  ::else8::
                  if exp4.tag == "SOME" and exp4.payload[1] == 13 then
                    local strm_PRIME_PRIME_PRIME = exp4.payload[2]
                    local tmp27 = {10, {13, {34, {34, accum1}}}}
                    local tmp28
                    do
                      local strm1 = strm_PRIME_PRIME_PRIME[1]
                      tmp28 = expect(10, strm1, strm_PRIME_PRIME_PRIME[2])
                    end
                    local strm1 = tmp28[1]
                    tmp24 = tmp27
                    tmp25 = strm1
                    tmp26 = tmp28[2]
                    goto cont
                  else
                    goto else9
                  end
                  ::else9::
                  if exp4.tag == "SOME" then
                    local c = exp4.payload[1]
                    local strm_PRIME_PRIME_PRIME = exp4.payload[2]
                    local tmp27 = checkAllowedChar(c)
                    local tmp28 = {tmp27, {34, {34, accum1}}}
                    local strm1 = strm_PRIME_PRIME_PRIME[1]
                    tmp24 = tmp28
                    tmp25 = strm1
                    tmp26 = strm_PRIME_PRIME_PRIME[2]
                    goto cont
                  elseif exp4.tag == "NONE" then
                    _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, _exn_meta), "parse_toml.sml:16:5")
                  else
                    _raise(_Match, "parse_toml.sml:170:21")
                  end
                else
                  goto else4
                end
              end
              ::else4::
              if exp3.tag == "SOME" and exp3.payload[1] == 92 then
                local strm_PRIME_PRIME = exp3.payload[2]
                local tmp27 = {34, accum1}
                local strm1 = strm_PRIME_PRIME[1]
                return escape(tmp27, strm1, strm_PRIME_PRIME[2])
              else
                goto else5
              end
              ::else5::
              if exp3.tag == "SOME" and exp3.payload[1] == 13 then
                local strm_PRIME_PRIME = exp3.payload[2]
                local tmp27 = {10, {13, {34, accum1}}}
                local tmp28
                do
                  local strm1 = strm_PRIME_PRIME[1]
                  tmp28 = expect(10, strm1, strm_PRIME_PRIME[2])
                end
                local strm1 = tmp28[1]
                tmp24 = tmp27
                tmp25 = strm1
                tmp26 = tmp28[2]
                goto cont
              else
                goto else6
              end
              ::else6::
              if exp3.tag == "SOME" then
                local c = exp3.payload[1]
                local strm_PRIME_PRIME_PRIME = exp3.payload[2]
                local tmp27 = checkAllowedChar(c)
                local tmp28 = {tmp27, {34, accum1}}
                local strm1 = strm_PRIME_PRIME_PRIME[1]
                tmp24 = tmp28
                tmp25 = strm1
                tmp26 = strm_PRIME_PRIME_PRIME[2]
                goto cont
              elseif exp3.tag == "NONE" then
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, _exn_meta), "parse_toml.sml:16:5")
              else
                _raise(_Match, "parse_toml.sml:168:16")
              end
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" and exp2.payload[1] == 13 then
              local strm_PRIME = exp2.payload[2]
              local tmp27 = {10, {13, accum1}}
              local tmp28
              do
                local strm1 = strm_PRIME[1]
                tmp28 = expect(10, strm1, strm_PRIME[2])
              end
              local strm1 = tmp28[1]
              tmp24 = tmp27
              tmp25 = strm1
              tmp26 = tmp28[2]
              goto cont
            else
              goto else3
            end
            ::else3::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              local tmp27 = checkAllowedChar(c)
              local tmp28 = {tmp27, accum1}
              local strm1 = strm_PRIME[1]
              tmp24 = tmp28
              tmp25 = strm1
              tmp26 = strm_PRIME[2]
              goto cont
            else
              _raise(_Match, "parse_toml.sml:164:11")
            end
          end
          go2 = function(accum, strm, state)
            local tmp24, tmp25, tmp26 = accum, strm, state
            ::cont::
            local accum1, exp2
            do
              local strm1, state1
              accum1, strm1, state1 = tmp24, tmp25, tmp26
              exp2 = tmp23(strm1, state1)
              if exp2.tag == "NONE" then
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, _exn_meta), "parse_toml.sml:16:5")
              elseif exp2.tag == "SOME" and exp2.payload[1] == 39 then
                local strm_PRIME = exp2.payload[2]
                local tmp27 = implodeRev(accum1)
                return {tmp27, strm_PRIME}
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              local tmp27 = c < 32 and c ~= 9
              local tmp28
              tmp28 = tmp27 or c == 127
              if tmp28 then
                local tmp29 = toString(c)
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp29, expected = "printable character"}}}, _exn_meta), "parse_toml.sml:19:5")
              else
                local tmp29 = {c, accum1}
                local strm1 = strm_PRIME[1]
                tmp24 = tmp29
                tmp25 = strm1
                tmp26 = strm_PRIME[2]
                goto cont
              end
            else
              _raise(_Match, "parse_toml.sml:248:11")
            end
          end
          local function readHexIntUnderscore(accum, strm)
            local tmp24, tmp25 = accum, strm
            ::cont::
            local accum1, strm1 = tmp24, tmp25
            local exp2
            do
              local strm2 = strm1[1]
              exp2 = tmp23(strm2, strm1[2])
            end
            if exp2.tag == "NONE" then
              return {accum1, strm1}
            elseif exp2.tag == "SOME" and exp2.payload[1] == 95 then
              local strm_PRIME = exp2.payload[2]
              local strm2 = strm_PRIME[1]
              return readHexInt(accum1, strm2, strm_PRIME[2])
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if 48 <= c and c <= 57 then
                local tmp26 = fromInt(16)
                local tmp27 = _L[6](accum1, tmp26)
                local tmp28 = fromInt(_Int_sub(c, 48))
                local tmp29 = _L[5](tmp27, tmp28)
                tmp24 = tmp29
                tmp25 = strm_PRIME
                goto cont
              else
                goto else2
              end
              ::else2::
              if 65 <= c and c <= 70 then
                local tmp26 = fromInt(16)
                local tmp27 = _L[6](accum1, tmp26)
                local tmp28 = fromInt(_Int_sub(c, 55))
                local tmp29 = _L[5](tmp27, tmp28)
                tmp24 = tmp29
                tmp25 = strm_PRIME
                goto cont
              else
                goto else3
              end
              ::else3::
              if 97 <= c and c <= 102 then
                local tmp26 = fromInt(16)
                local tmp27 = _L[6](accum1, tmp26)
                local tmp28 = fromInt(_Int_sub(c, 87))
                local tmp29 = _L[5](tmp27, tmp28)
                tmp24 = tmp29
                tmp25 = strm_PRIME
                goto cont
              else
                return {accum1, strm1}
              end
            else
              _raise(_Match, "parse_toml.sml:324:9")
            end
          end
          readHexInt = function(accum, strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "hexadecimal digit"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if 48 <= c and c <= 57 then
                local tmp24 = fromInt(16)
                local tmp25 = _L[6](accum, tmp24)
                local tmp26 = fromInt(_Int_sub(c, 48))
                local tmp27 = _L[5](tmp25, tmp26)
                return readHexIntUnderscore(tmp27, strm_PRIME)
              else
                goto else1
              end
              ::else1::
              if 65 <= c and c <= 70 then
                local tmp24 = fromInt(16)
                local tmp25 = _L[6](accum, tmp24)
                local tmp26 = fromInt(_Int_sub(c, 55))
                local tmp27 = _L[5](tmp25, tmp26)
                return readHexIntUnderscore(tmp27, strm_PRIME)
              else
                goto else2
              end
              ::else2::
              if 97 <= c and c <= 102 then
                local tmp24 = fromInt(16)
                local tmp25 = _L[6](accum, tmp24)
                local tmp26 = fromInt(_Int_sub(c, 87))
                local tmp27 = _L[5](tmp25, tmp26)
                return readHexIntUnderscore(tmp27, strm_PRIME)
              else
                goto else3
              end
              ::else3::
              local tmp24 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "hexadecimal digit"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:292:9")
            end
          end
          local function readOctIntUnderscore(accum, strm)
            local tmp24, tmp25 = accum, strm
            ::cont::
            local accum1, strm1 = tmp24, tmp25
            local exp2
            do
              local strm2 = strm1[1]
              exp2 = tmp23(strm2, strm1[2])
            end
            if exp2.tag == "NONE" then
              return {accum1, strm1}
            elseif exp2.tag == "SOME" and exp2.payload[1] == 95 then
              local strm_PRIME = exp2.payload[2]
              local strm2 = strm_PRIME[1]
              return readOctInt(accum1, strm2, strm_PRIME[2])
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if 48 <= c and c <= 55 then
                local tmp26 = fromInt(8)
                local tmp27 = _L[6](accum1, tmp26)
                local tmp28 = fromInt(_Int_sub(c, 48))
                local tmp29 = _L[5](tmp27, tmp28)
                tmp24 = tmp29
                tmp25 = strm_PRIME
                goto cont
              else
                return {accum1, strm1}
              end
            else
              _raise(_Match, "parse_toml.sml:376:9")
            end
          end
          readOctInt = function(accum, strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "octal digit"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif exp2.tag == "SOME" then
              local c
              do
                c = exp2.payload[1]
                local strm_PRIME = exp2.payload[2]
                if 48 <= c and c <= 55 then
                  local tmp24 = fromInt(8)
                  local tmp25 = _L[6](accum, tmp24)
                  local tmp26 = fromInt(_Int_sub(c, 48))
                  local tmp27 = _L[5](tmp25, tmp26)
                  return readOctIntUnderscore(tmp27, strm_PRIME)
                else
                  goto else1
                end
              end
              ::else1::
              local tmp24 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "octal digit"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:362:9")
            end
          end
          local function readBinIntUnderscore(accum, strm)
            local tmp24, tmp25 = accum, strm
            ::cont::
            local accum1, strm1 = tmp24, tmp25
            local exp2
            do
              local strm2 = strm1[1]
              exp2 = tmp23(strm2, strm1[2])
            end
            if exp2.tag == "NONE" then
              return {accum1, strm1}
            elseif exp2.tag == "SOME" and exp2.payload[1] == 95 then
              local strm_PRIME = exp2.payload[2]
              local strm2 = strm_PRIME[1]
              return readBinInt(accum1, strm2, strm_PRIME[2])
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 48 then
              local strm_PRIME = exp2.payload[2]
              local tmp26 = fromInt(2)
              local tmp27 = _L[6](accum1, tmp26)
              tmp24 = tmp27
              tmp25 = strm_PRIME
              goto cont
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" and exp2.payload[1] == 49 then
              local strm_PRIME = exp2.payload[2]
              local tmp26 = fromInt(2)
              local tmp27 = _L[6](accum1, tmp26)
              local tmp28 = fromInt(1)
              local tmp29 = _L[5](tmp27, tmp28)
              tmp24 = tmp29
              tmp25 = strm_PRIME
              goto cont
            elseif exp2.tag == "SOME" then
              return {accum1, strm1}
            else
              _raise(_Match, "parse_toml.sml:409:9")
            end
          end
          readBinInt = function(accum, strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "binary digit"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif exp2.tag == "SOME" and exp2.payload[1] == 48 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = fromInt(2)
              local tmp25 = _L[6](accum, tmp24)
              return readBinIntUnderscore(tmp25, strm_PRIME)
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 49 then
              local strm_PRIME = exp2.payload[2]
              local tmp24 = fromInt(2)
              local tmp25 = _L[6](accum, tmp24)
              local tmp26 = fromInt(1)
              local tmp27 = _L[5](tmp25, tmp26)
              return readBinIntUnderscore(tmp27, strm_PRIME)
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" then
              local tmp24 = toString(exp2.payload[1])
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "binary digit"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:394:9")
            end
          end
          local readDecIntUnderscore, readDecInt
          readDecIntUnderscore = function(accum, hadUnderscore, strm)
            local tmp24, tmp25, tmp26 = accum, hadUnderscore, strm
            ::cont::
            local accum1, hadUnderscore1, strm1 = tmp24, tmp25, tmp26
            local exp2
            do
              local strm2 = strm1[1]
              exp2 = tmp23(strm2, strm1[2])
            end
            if exp2.tag == "NONE" then
              return {accum1, hadUnderscore1, strm1}
            elseif exp2.tag == "SOME" and exp2.payload[1] == 95 then
              local strm_PRIME = exp2.payload[2]
              local strm2 = strm_PRIME[1]
              return readDecInt(accum1, true, strm2, strm_PRIME[2])
            else
              goto else1
            end
            ::else1::
            if exp2.tag == "SOME" then
              local c = exp2.payload[1]
              local strm_PRIME = exp2.payload[2]
              if 48 <= c and c <= 57 then
                tmp24 = {c, accum1}
                tmp25 = hadUnderscore1
                tmp26 = strm_PRIME
                goto cont
              else
                return {accum1, hadUnderscore1, strm1}
              end
            else
              _raise(_Match, "parse_toml.sml:436:9")
            end
          end
          readDecInt = function(accum, hadUnderscore, strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif exp2.tag == "SOME" then
              local c
              do
                c = exp2.payload[1]
                local strm_PRIME = exp2.payload[2]
                if 48 <= c and c <= 57 then
                  return readDecIntUnderscore({c, accum}, hadUnderscore, strm_PRIME)
                else
                  goto else1
                end
              end
              ::else1::
              local tmp24 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "digit"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:428:9")
            end
          end
          local readExpPart = function(accum_PRIME, e, strm, state)
            local exp2
            do
              local exp3 = tmp23(strm, state)
              if exp3.tag == "NONE" then
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "exponent part"}}}, _exn_meta), "parse_toml.sml:16:5")
              elseif exp3.tag == "SOME" and exp3.payload[1] == 43 then
                local strm_PRIME_PRIME_PRIME = exp3.payload[2]
                local tmp24 = {43, {e, accum_PRIME}}
                local strm1 = strm_PRIME_PRIME_PRIME[1]
                exp2 = readDecInt(tmp24, false, strm1, strm_PRIME_PRIME_PRIME[2])
                goto cont
              else
                goto else1
              end
              ::else1::
              if exp3.tag == "SOME" and exp3.payload[1] == 45 then
                local strm_PRIME_PRIME_PRIME = exp3.payload[2]
                local tmp24 = {45, {e, accum_PRIME}}
                local strm1 = strm_PRIME_PRIME_PRIME[1]
                exp2 = readDecInt(tmp24, false, strm1, strm_PRIME_PRIME_PRIME[2])
                goto cont
              else
                goto else2
              end
              ::else2::
              if exp3.tag == "SOME" then
                local c
                do
                  c = exp3.payload[1]
                  local strm_PRIME_PRIME_PRIME = exp3.payload[2]
                  if 48 <= c and c <= 57 then
                  else
                    goto else3
                  end
                  exp2 = readDecIntUnderscore({c, {e, accum_PRIME}}, false, strm_PRIME_PRIME_PRIME)
                  goto cont
                end
                ::else3::
                local tmp24 = toString(c)
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "digit or sign (+/-)"}}}, _exn_meta), "parse_toml.sml:19:5")
              else
                _raise(_Match, "parse_toml.sml:448:13")
              end
            end
            ::cont::
            local accum_PRIME_PRIME = exp2[1]
            local strm_PRIME_PRIME_PRIME = exp2[3]
            local tmp24 = implodeRev(accum_PRIME_PRIME)
            local tmp25 = _L[188](tmp24)
            return {tmp25, strm_PRIME_PRIME_PRIME}
          end
          readSigned = function(sign, d0, strm)
            local accum, strm_PRIME, checkPrefixZero, exp2
            do
              local exp3 = readDecIntUnderscore({d0, {sign, nil}}, false, strm)
              accum = exp3[1]
              strm_PRIME = exp3[3]
              checkPrefixZero = function()
                if accum ~= nil and (accum[2] ~= nil and accum[2][2] == nil) then
                  return nil
                elseif d0 == 48 then
                  _raise(setmetatable({tag = _L[186], payload = _L[181]}, _exn_meta), "parse_toml.sml:470:38")
                else
                  return nil
                end
              end
              do
                local strm1 = strm_PRIME[1]
                exp2 = tmp23(strm1, strm_PRIME[2])
              end
              if exp2.tag == "SOME" and exp2.payload[1] == 46 then
                local accum_PRIME, strm_PRIME_PRIME_PRIME, exp4
                do
                  local strm_PRIME_PRIME = exp2.payload[2]
                  checkPrefixZero()
                  local tmp24 = {46, accum}
                  local exp5
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    exp5 = readDecInt(tmp24, false, strm1, strm_PRIME_PRIME[2])
                  end
                  accum_PRIME = exp5[1]
                  strm_PRIME_PRIME_PRIME = exp5[3]
                  do
                    local strm1 = strm_PRIME_PRIME_PRIME[1]
                    exp4 = tmp23(strm1, strm_PRIME_PRIME_PRIME[2])
                  end
                  if exp4.tag == "SOME" and exp4.payload[1] == 101 then
                    local strm_PRIME_PRIME_PRIME_PRIME = exp4.payload[2]
                    local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                    return readExpPart(accum_PRIME, 101, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                  else
                    goto else4
                  end
                end
                ::else4::
                if exp4.tag == "SOME" and exp4.payload[1] == 69 then
                  local strm_PRIME_PRIME_PRIME_PRIME = exp4.payload[2]
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readExpPart(accum_PRIME, 69, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                else
                  goto else5
                end
                ::else5::
                local tmp24 = implodeRev(accum_PRIME)
                local tmp25 = _L[188](tmp24)
                return {tmp25, strm_PRIME_PRIME_PRIME}
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 101 then
              local strm_PRIME_PRIME = exp2.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(accum, 101, strm1, strm_PRIME_PRIME[2])
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" and exp2.payload[1] == 69 then
              local strm_PRIME_PRIME = exp2.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(accum, 69, strm1, strm_PRIME_PRIME[2])
            else
              goto else3
            end
            ::else3::
            checkPrefixZero()
            local tmp24 = implodeRev(accum)
            local tmp25 = _L[9](tmp24)
            if tmp25.tag == "SOME" then
              return {{tag = "INTEGER", payload = tmp25.payload}, strm_PRIME}
            elseif tmp25.tag == "NONE" then
              _raise(Option, "option.sml:24:18")
            else
              _raise(_Match, "option.sml:23:5")
            end
          end
          local readTwoDigit = function(accum, strm, state)
            local exp2 = tmp23(strm, state)
            if exp2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif exp2.tag == "SOME" then
              local c0
              do
                c0 = exp2.payload[1]
                local strm_PRIME = exp2.payload[2]
                if 48 <= c0 and c0 <= 57 then
                  local exp3
                  do
                    local strm1 = strm_PRIME[1]
                    exp3 = tmp23(strm1, strm_PRIME[2])
                  end
                  if exp3.tag == "NONE" then
                    _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit"}}}, _exn_meta), "parse_toml.sml:16:5")
                  elseif exp3.tag == "SOME" then
                    local c1 = exp3.payload[1]
                    local strm_PRIME_PRIME = exp3.payload[2]
                    if 48 <= c1 and c1 <= 57 then
                      local tmp24 = {c1, {c0, accum}}
                      local tmp25 = _Int_mul(_Int_sub(c0, 48), 10)
                      return {tmp24, _Int_add(tmp25, _Int_sub(c1, 48)), strm_PRIME_PRIME}
                    end
                    local tmp24 = toString(c1)
                    _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "digit"}}}, _exn_meta), "parse_toml.sml:19:5")
                  else
                    _raise(_Match, "parse_toml.sml:506:15")
                  end
                else
                  goto else1
                end
              end
              ::else1::
              local tmp24 = toString(c0)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "digit"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:502:9")
            end
          end
          local readMinSec = function(accum, strm, state)
            local exp2 = readTwoDigit(accum, strm, state)
            local accum_PRIME = exp2[1]
            local min = exp2[2]
            local strm_PRIME = exp2[3]
            local strm_PRIME_PRIME
            do
              local strm1 = strm_PRIME[1]
              strm_PRIME_PRIME = expect(58, strm1, strm_PRIME[2])
            end
            local tmp24 = {58, accum_PRIME}
            local exp3
            do
              local strm1 = strm_PRIME_PRIME[1]
              exp3 = readTwoDigit(tmp24, strm1, strm_PRIME_PRIME[2])
            end
            local accum_PRIME_PRIME = exp3[1]
            local sec = exp3[2]
            local strm_PRIME_PRIME_PRIME = exp3[3]
            local tmp25 = min < 60 and sec <= 60
            if tmp25 then
            else
              _raise(setmetatable({tag = _L[186], payload = _L[184]}, _exn_meta), "parse_toml.sml:535:18")
            end
            local exp4
            do
              local strm1 = strm_PRIME_PRIME_PRIME[1]
              exp4 = tmp23(strm1, strm_PRIME_PRIME_PRIME[2])
            end
            if exp4.tag == "SOME" and exp4.payload[1] == 46 then
              local tmp26, tmp27
              do
                local strm_PRIME_PRIME_PRIME_PRIME = exp4.payload[2]
                tmp27, tmp26 = {46, accum_PRIME_PRIME}, strm_PRIME_PRIME_PRIME_PRIME
              end
              ::cont::
              local accum1, strm1 = tmp27, tmp26
              local exp5
              do
                local strm2 = strm1[1]
                exp5 = tmp23(strm2, strm1[2])
              end
              if exp5.tag == "NONE" then
                return {accum1, strm1}
              elseif exp5.tag == "SOME" then
                local c = exp5.payload[1]
                local strm_PRIME1 = exp5.payload[2]
                if 48 <= c and c <= 57 then
                  tmp27 = {c, accum1}
                  tmp26 = strm_PRIME1
                  goto cont
                else
                  return {accum1, strm1}
                end
              else
                _raise(_Match, "parse_toml.sml:520:11")
              end
            else
              return {accum_PRIME_PRIME, strm_PRIME_PRIME_PRIME}
            end
          end
          local readTimePart = function(accum, strm, state)
            local accum_PRIME_PRIME, strm_PRIME_PRIME, exp2
            do
              local exp3 = readTwoDigit(accum, strm, state)
              local accum_PRIME = exp3[1]
              local hour = exp3[2]
              local strm_PRIME = exp3[3]
              if hour < 24 then
              else
                _raise(setmetatable({tag = _L[186], payload = _L[184]}, _exn_meta), "parse_toml.sml:545:46")
              end
              local tmp24 = {58, accum_PRIME}
              local tmp25
              do
                local strm1 = strm_PRIME[1]
                tmp25 = expect(58, strm1, strm_PRIME[2])
              end
              local exp4
              do
                local strm1 = tmp25[1]
                exp4 = readMinSec(tmp24, strm1, tmp25[2])
              end
              accum_PRIME_PRIME = exp4[1]
              strm_PRIME_PRIME = exp4[2]
              do
                local strm1 = strm_PRIME_PRIME[1]
                exp2 = tmp23(strm1, strm_PRIME_PRIME[2])
              end
              if exp2.tag == "SOME" and exp2.payload[1] == 90 then
                local strm_PRIME_PRIME_PRIME = exp2.payload[2]
                local tmp26 = implodeRev({90, accum_PRIME_PRIME})
                return {{tag = "DATETIME", payload = tmp26}, strm_PRIME_PRIME_PRIME}
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 122 then
              local strm_PRIME_PRIME_PRIME = exp2.payload[2]
              local tmp24 = implodeRev({122, accum_PRIME_PRIME})
              return {{tag = "DATETIME", payload = tmp24}, strm_PRIME_PRIME_PRIME}
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" and exp2.payload[1] == 43 then
              local strm_PRIME_PRIME_PRIME = exp2.payload[2]
              local tmp24 = {43, accum_PRIME_PRIME}
              local exp3
              do
                local strm1 = strm_PRIME_PRIME_PRIME[1]
                exp3 = readTwoDigit(tmp24, strm1, strm_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME = exp3[1]
              local offsetHour = exp3[2]
              local strm_PRIME_PRIME_PRIME_PRIME = exp3[3]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                strm_PRIME_PRIME_PRIME_PRIME_PRIME = expect(58, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
              end
              local tmp25 = {58, accum_PRIME_PRIME_PRIME}
              local exp4
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME_PRIME[1]
                exp4 = readTwoDigit(tmp25, strm1, strm_PRIME_PRIME_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME_PRIME = exp4[1]
              local offsetMin = exp4[2]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME = exp4[3]
              if offsetHour < 24 and offsetMin < 60 then
                local tmp26 = implodeRev(accum_PRIME_PRIME_PRIME_PRIME)
                return {{tag = "DATETIME", payload = tmp26}, strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME}
              else
                _raise(setmetatable({tag = _L[186], payload = _L[184]}, _exn_meta), "parse_toml.sml:567:24")
              end
            else
              goto else3
            end
            ::else3::
            if exp2.tag == "SOME" and exp2.payload[1] == 45 then
              local strm_PRIME_PRIME_PRIME = exp2.payload[2]
              local tmp24 = {45, accum_PRIME_PRIME}
              local exp3
              do
                local strm1 = strm_PRIME_PRIME_PRIME[1]
                exp3 = readTwoDigit(tmp24, strm1, strm_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME = exp3[1]
              local offsetHour = exp3[2]
              local strm_PRIME_PRIME_PRIME_PRIME = exp3[3]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                strm_PRIME_PRIME_PRIME_PRIME_PRIME = expect(58, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
              end
              local tmp25 = {58, accum_PRIME_PRIME_PRIME}
              local exp4
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME_PRIME[1]
                exp4 = readTwoDigit(tmp25, strm1, strm_PRIME_PRIME_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME_PRIME = exp4[1]
              local offsetMin = exp4[2]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME = exp4[3]
              if offsetHour < 24 and offsetMin < 60 then
                local tmp26 = implodeRev(accum_PRIME_PRIME_PRIME_PRIME)
                return {{tag = "DATETIME", payload = tmp26}, strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME}
              else
                _raise(setmetatable({tag = _L[186], payload = _L[184]}, _exn_meta), "parse_toml.sml:580:24")
              end
            else
              goto else4
            end
            ::else4::
            local tmp24 = implodeRev(accum_PRIME_PRIME)
            return {{tag = "LOCAL_DATETIME", payload = tmp24}, strm_PRIME_PRIME}
          end
          readUnsigned = function(d0, strm)
            local revDigits, hadUnderscore, strm_PRIME, checkPrefixZero, tmp24
            do
              local exp2 = readDecIntUnderscore({d0, nil}, false, strm)
              revDigits = exp2[1]
              hadUnderscore = exp2[2]
              strm_PRIME = exp2[3]
              checkPrefixZero = function()
                if revDigits ~= nil and revDigits[2] == nil then
                  return nil
                elseif d0 == 48 then
                  _raise(setmetatable({tag = _L[186], payload = _L[181]}, _exn_meta), "parse_toml.sml:637:38")
                else
                  return nil
                end
              end
              do
                local strm1 = strm_PRIME[1]
                tmp24 = tmp23(strm1, strm_PRIME[2])
              end
              local tmp25 = revDigits ~= nil and (revDigits[2] ~= nil and (revDigits[2][2] ~= nil and (revDigits[2][2][2] ~= nil and revDigits[2][2][2][2] == nil)))
              local tmp26
              tmp26 = tmp25 and (not hadUnderscore and (tmp24.tag == "SOME" and tmp24.payload[1] == 45))
              if tmp26 then
                local year, accum_PRIME, month, strm_PRIME_PRIME
                do
                  local tmp27 = revDigits[1]
                  local tmp28 = revDigits[2][1]
                  local tmp29 = revDigits[2][2][1]
                  local strm_PRIME_PRIME1 = tmp24.payload[2]
                  local tmp30 = _Int_mul(_Int_sub(d0, 48), 10)
                  local tmp31 = _Int_mul(_Int_add(tmp30, _Int_sub(tmp29, 48)), 10)
                  local tmp32 = _Int_mul(_Int_add(tmp31, _Int_sub(tmp28, 48)), 10)
                  year = _Int_add(tmp32, _Int_sub(tmp27, 48))
                  local tmp33 = {45, revDigits}
                  local exp3
                  do
                    local strm1 = strm_PRIME_PRIME1[1]
                    exp3 = readTwoDigit(tmp33, strm1, strm_PRIME_PRIME1[2])
                  end
                  accum_PRIME = exp3[1]
                  month = exp3[2]
                  local strm_PRIME1 = exp3[3]
                  local strm1 = strm_PRIME1[1]
                  strm_PRIME_PRIME = expect(45, strm1, strm_PRIME1[2])
                end
                local accum_PRIME_PRIME, strm_PRIME_PRIME_PRIME, tmp27
                do
                  local exp3
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    exp3 = readTwoDigit(accum_PRIME, strm1, strm_PRIME_PRIME[2])
                  end
                  accum_PRIME_PRIME = exp3[1]
                  local mday = exp3[2]
                  strm_PRIME_PRIME_PRIME = exp3[3]
                  if month == 1 then
                    tmp27 = 1 <= mday and mday <= 31
                    goto cont
                  elseif month == 2 and mday == 29 then
                    local tmp28 = rem(year, 4)
                    if tmp28 == 0 then
                      local tmp29 = rem(year, 100)
                      if tmp29 ~= 0 then
                        tmp27 = true
                        goto cont
                      end
                      local tmp30 = rem(year, 400)
                      tmp27 = tmp30 == 0
                    else
                      tmp27 = false
                      goto cont
                    end
                  elseif month == 2 then
                    tmp27 = 1 <= mday and mday <= 28
                    goto cont
                  elseif month == 3 then
                    tmp27 = 1 <= mday and mday <= 31
                    goto cont
                  elseif month == 4 then
                    tmp27 = 1 <= mday and mday <= 30
                    goto cont
                  elseif month == 5 then
                    tmp27 = 1 <= mday and mday <= 31
                    goto cont
                  elseif month == 6 then
                    tmp27 = 1 <= mday and mday <= 30
                    goto cont
                  elseif month == 7 then
                    tmp27 = 1 <= mday and mday <= 31
                    goto cont
                  elseif month == 8 then
                    tmp27 = 1 <= mday and mday <= 31
                    goto cont
                  elseif month == 9 then
                    tmp27 = 1 <= mday and mday <= 30
                    goto cont
                  elseif month == 10 then
                    tmp27 = 1 <= mday and mday <= 31
                    goto cont
                  elseif month == 11 then
                    tmp27 = 1 <= mday and mday <= 30
                    goto cont
                  else
                    tmp27 = month == 12 and (1 <= mday and mday <= 31)
                    goto cont
                  end
                end
                ::cont::
                if tmp27 then
                else
                  _raise(setmetatable({tag = _L[186], payload = _L[183]}, _exn_meta), "parse_toml.sml:599:18")
                end
                local exp3
                do
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  exp3 = tmp23(strm1, strm_PRIME_PRIME_PRIME[2])
                end
                if exp3.tag == "SOME" and exp3.payload[1] == 84 then
                  local strm_PRIME_PRIME_PRIME_PRIME = exp3.payload[2]
                  local tmp28 = {84, accum_PRIME_PRIME}
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readTimePart(tmp28, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                else
                  goto else6
                end
                ::else6::
                if exp3.tag == "SOME" and exp3.payload[1] == 116 then
                  local strm_PRIME_PRIME_PRIME_PRIME = exp3.payload[2]
                  local tmp28 = {84, accum_PRIME_PRIME}
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readTimePart(tmp28, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                else
                  goto else7
                end
                ::else7::
                if exp3.tag == "SOME" and exp3.payload[1] == 32 then
                  do
                    local strm_PRIME_PRIME_PRIME_PRIME = exp3.payload[2]
                    local exp4
                    do
                      local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                      exp4 = tmp23(strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                    end
                    if exp4.tag == "SOME" then
                      do
                        local c = exp4.payload[1]
                        if 48 <= c and c <= 57 then
                          local tmp28 = {84, accum_PRIME_PRIME}
                          local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                          return readTimePart(tmp28, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                        else
                          goto else10
                        end
                      end
                      ::else10::
                      local tmp28 = implodeRev(accum_PRIME_PRIME)
                      return {{tag = "DATE", payload = tmp28}, strm_PRIME_PRIME_PRIME}
                    else
                      goto else9
                    end
                  end
                  ::else9::
                  local tmp28 = implodeRev(accum_PRIME_PRIME)
                  return {{tag = "DATE", payload = tmp28}, strm_PRIME_PRIME_PRIME}
                else
                  goto else8
                end
                ::else8::
                local tmp28 = implodeRev(accum_PRIME_PRIME)
                return {{tag = "DATE", payload = tmp28}, strm_PRIME_PRIME_PRIME}
              else
                goto else1
              end
            end
            ::else1::
            do
              local tmp25 = revDigits ~= nil and (revDigits[2] ~= nil and revDigits[2][2] == nil)
              local tmp26
              tmp26 = tmp25 and (not hadUnderscore and (tmp24.tag == "SOME" and tmp24.payload[1] == 58))
              if tmp26 then
                local tmp27 = revDigits[1]
                local tmp28 = revDigits[2][1]
                local strm_PRIME_PRIME = tmp24.payload[2]
                local tmp29 = _Int_mul(_Int_sub(tmp28, 48), 10)
                if _Int_add(tmp29, _Int_sub(tmp27, 48)) < 24 then
                else
                  _raise(setmetatable({tag = _L[186], payload = _L[184]}, _exn_meta), "parse_toml.sml:652:45")
                end
                local tmp30 = {58, revDigits}
                local exp2
                do
                  local strm1 = strm_PRIME_PRIME[1]
                  exp2 = readMinSec(tmp30, strm1, strm_PRIME_PRIME[2])
                end
                local accum = exp2[1]
                local strm_PRIME_PRIME_PRIME = exp2[2]
                local tmp31 = implodeRev(accum)
                return {{tag = "TIME", payload = tmp31}, strm_PRIME_PRIME_PRIME}
              else
                goto else2
              end
            end
            ::else2::
            if tmp24.tag == "SOME" and tmp24.payload[1] == 46 then
              local accum, strm_PRIME_PRIME_PRIME, exp2
              do
                local strm_PRIME_PRIME = tmp24.payload[2]
                checkPrefixZero()
                local tmp25 = {46, revDigits}
                local exp3
                do
                  local strm1 = strm_PRIME_PRIME[1]
                  exp3 = readDecInt(tmp25, false, strm1, strm_PRIME_PRIME[2])
                end
                accum = exp3[1]
                strm_PRIME_PRIME_PRIME = exp3[3]
                do
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  exp2 = tmp23(strm1, strm_PRIME_PRIME_PRIME[2])
                end
                if exp2.tag == "SOME" and exp2.payload[1] == 101 then
                  local strm_PRIME_PRIME_PRIME_PRIME = exp2.payload[2]
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readExpPart(accum, 101, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                else
                  goto else6
                end
              end
              ::else6::
              if exp2.tag == "SOME" and exp2.payload[1] == 69 then
                local strm_PRIME_PRIME_PRIME_PRIME = exp2.payload[2]
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                return readExpPart(accum, 69, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
              else
                goto else7
              end
              ::else7::
              local tmp25 = implodeRev(accum)
              local tmp26 = _L[188](tmp25)
              return {tmp26, strm_PRIME_PRIME_PRIME}
            else
              goto else3
            end
            ::else3::
            if tmp24.tag == "SOME" and tmp24.payload[1] == 101 then
              local strm_PRIME_PRIME = tmp24.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(revDigits, 101, strm1, strm_PRIME_PRIME[2])
            else
              goto else4
            end
            ::else4::
            if tmp24.tag == "SOME" and tmp24.payload[1] == 69 then
              local strm_PRIME_PRIME = tmp24.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(revDigits, 69, strm1, strm_PRIME_PRIME[2])
            else
              goto else5
            end
            ::else5::
            checkPrefixZero()
            local tmp25 = implodeRev(revDigits)
            local tmp26 = _L[9](tmp25)
            if tmp26.tag == "SOME" then
              return {{tag = "INTEGER", payload = tmp26.payload}, strm_PRIME}
            elseif tmp26.tag == "NONE" then
              _raise(Option, "option.sml:24:18")
            else
              _raise(_Match, "option.sml:23:5")
            end
          end
        end
        local readKey, IMPLICIT_HEADER, finalizeTable, readKeyval, insertKeyval, insertTable
        do
          local isValidUnquotedKey = function(c)
            local tmp24
            do
              local tmp25
              if 65 <= c and c <= 90 then
                tmp24 = true
                goto cont
              else
                tmp25 = 97 <= c and c <= 122
              end
              tmp24 = tmp25 or 48 <= c and c <= 57
            end
            ::cont::
            return tmp24 or (c == 45 or c == 95)
          end
          local readSimpleKey = function(a)
            if a.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "key"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif a.tag == "SOME" and a.payload[1] == 34 then
              local strm = a.payload[2]
              local strm1 = strm[1]
              return go(nil, strm1, strm[2])
            else
              goto else1
            end
            ::else1::
            if a.tag == "SOME" and a.payload[1] == 39 then
              local strm = a.payload[2]
              local strm1 = strm[1]
              return go2(nil, strm1, strm[2])
            else
              goto else2
            end
            ::else2::
            if a.tag == "SOME" then
              local c0
              do
                c0 = a.payload[1]
                local strm = a.payload[2]
                local tmp24 = isValidUnquotedKey(c0)
                if tmp24 then
                  local tmp25, tmp26 = {c0, nil}, strm
                  ::cont::
                  local accum, strm_PRIME = tmp25, tmp26
                  local exp2
                  do
                    local strm1 = strm_PRIME[1]
                    exp2 = tmp23(strm1, strm_PRIME[2])
                  end
                  if exp2.tag == "NONE" then
                    local tmp27 = implodeRev(accum)
                    return {tmp27, strm_PRIME}
                  else
                    goto else4
                  end
                  ::else4::
                  if exp2.tag == "SOME" then
                    local c = exp2.payload[1]
                    local strm_PRIME_PRIME = exp2.payload[2]
                    local tmp27 = isValidUnquotedKey(c)
                    if tmp27 then
                      tmp25 = {c, accum}
                      tmp26 = strm_PRIME_PRIME
                      goto cont
                    end
                    local tmp28 = implodeRev(accum)
                    return {tmp28, strm_PRIME}
                  else
                    _raise(_Match, "parse_toml.sml:694:17")
                  end
                else
                  goto else3
                end
              end
              ::else3::
              local tmp24 = toString(c0)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "quote, alphanum, '-', or '_'"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:688:11")
            end
          end
          readKey = function(input)
            local tmp24, tmp25
            do
              local exp2 = readSimpleKey(input)
              local k0 = exp2[1]
              local strm = exp2[2]
              local tmp26 = {k0, nil}
              local tmp27 = skipWhiteSpace(strm)
              tmp25, tmp24 = tmp26, tmp27
            end
            ::cont::
            local strm, accum
            do
              accum, strm = tmp25, tmp24
              local exp2
              do
                local strm1 = strm[1]
                exp2 = tmp23(strm1, strm[2])
              end
              if exp2.tag == "SOME" and exp2.payload[1] == 46 then
                local strm_PRIME = exp2.payload[2]
                local tmp26
                do
                  local strm1 = strm_PRIME[1]
                  tmp26 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
                end
                local exp3 = readSimpleKey(tmp26)
                local k = exp3[1]
                local strm_PRIME_PRIME = exp3[2]
                local tmp27 = {k, accum}
                local tmp28 = skipWhiteSpace(strm_PRIME_PRIME)
                tmp25 = tmp27
                tmp24 = tmp28
                goto cont
              else
                goto else1
              end
            end
            ::else1::
            local tmp26 = revAppend(accum, nil)
            return {tmp26, strm}
          end
          local EXACT_HEADER = "EXACT_HEADER"
          IMPLICIT_HEADER = "IMPLICIT_HEADER"
          local IMPLICIT_KEYVAL = "IMPLICIT_KEYVAL"
          local finalize, finalizeValue
          finalize = function(xs)
            local tmp24 = finalizeTable(xs)
            return {tag = "TABLE", payload = tmp24}
          end
          finalizeValue = function(a)
            if a.tag == "LEAF" then
              return a.payload
            elseif a.tag == "PARTIAL_TABLE" then
              return finalize(a.payload[2])
            else
              goto else1
            end
            ::else1::
            if a.tag == "PARTIAL_ARRAY" then
              local last = a.payload[1]
              local xs = a.payload[2]
              local tmp24 = map(finalize)
              local tmp25 = tmp24(xs)
              local tmp26 = finalize(last)
              local tmp27 = revAppend(tmp25, {tmp26, nil})
              return {tag = "ARRAY", payload = tmp27}
            else
              _raise(_Match, "parse_toml.sml:733:11")
            end
          end
          finalizeTable = function(xs)
            local tmp24 = map(function(a)
              local key = a[1]
              local tmp25 = finalizeValue(a[2])
              return {key, tmp25}
            end)
            return tmp24(xs)
          end
          local function insert(revPath, pt, keys, v)
            if keys ~= nil and keys[2] == nil then
              local tmp24
              do
                tmp24 = keys[1]
                local tmp25 = exists(function(a)
                  return tmp24 == a[1]
                end)
                local tmp26 = tmp25(pt)
                if tmp26 then
                  local tmp27 = revAppend(revPath, keys)
                  _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp27}}, _exn_meta), "parse_toml.sml:22:5")
                else
                  goto else2
                end
              end
              ::else2::
              local tmp25 = {{tmp24, {tag = "LEAF", payload = v}}, nil}
              local tmp26 = revAppend(pt, nil)
              return revAppend(tmp26, tmp25)
            else
              goto else1
            end
            ::else1::
            if keys ~= nil then
              local tmp24 = keys[1]
              local tmp25 = keys[2]
              local tmp26, tmp27 = nil, pt
              ::cont::
              local accum, _2 = tmp26, tmp27
              if _2 == nil then
                local tmp28 = insert({tmp24, revPath}, nil, tmp25, v)
                return revAppend(accum, {{tmp24, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_KEYVAL, tmp28}}}, nil})
              else
                goto else2
              end
              ::else2::
              if _2 ~= nil then
                local tmp28 = _2[1]
                local key_PRIME = _2[1][1]
                local pv = _2[1][2]
                local tmp29 = _2[2]
                if tmp24 == key_PRIME then
                  if pv.tag == "LEAF" then
                    local tmp30 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp30}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else3
                  end
                  ::else3::
                  if pv.tag == "PARTIAL_TABLE" and pv.payload[1] == "EXACT_HEADER" then
                    local tmp30 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp30}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else4
                  end
                  ::else4::
                  if pv.tag == "PARTIAL_TABLE" then
                    local definedBy = pv.payload[1]
                    local pt_PRIME = pv.payload[2]
                    local tmp30 = insert({tmp24, revPath}, pt_PRIME, tmp25, v)
                    return revAppend(accum, {{key_PRIME, {tag = "PARTIAL_TABLE", payload = {definedBy, tmp30}}}, tmp29})
                  else
                    goto else5
                  end
                  ::else5::
                  if pv.tag == "PARTIAL_ARRAY" then
                    local tmp30 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp30}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    _raise(_Match, "parse_toml.sml:760:23")
                  end
                else
                  tmp26 = {tmp28, accum}
                  tmp27 = tmp29
                  goto cont
                end
              else
                _raise(_Match, "parse_toml.sml:748:19")
              end
            elseif keys == nil then
              _raise(_Match, "parse_toml.sml:781:34")
            else
              _raise(_Match, "parse_toml.sml:741:11")
            end
          end
          local readInlineTable, readArray, readValue
          readKeyval = function(revPath, r)
            local exp2 = readKey(r)
            local key = exp2[1]
            local strm_PRIME = exp2[2]
            local tmp24 = revAppend(key, revPath)
            local tmp25
            do
              local strm = strm_PRIME[1]
              tmp25 = expect(61, strm, strm_PRIME[2])
            end
            local tmp26
            do
              local strm = tmp25[1]
              tmp26 = skipWhiteSpaceAndGetc(strm, tmp25[2])
            end
            local exp3 = readValue(tmp24, tmp26)
            local v = exp3[1]
            return {key, v, exp3[2]}
          end
          readInlineTable = function(revPath, accum, r)
            local tmp24, tmp25, tmp26 = revPath, accum, r
            ::cont::
            local accum_PRIME, exp2
            do
              local revPath1, accum1, r1 = tmp24, tmp25, tmp26
              local exp3 = readKeyval(revPath1, r1)
              local key = exp3[1]
              local v = exp3[2]
              local strm = exp3[3]
              accum_PRIME = insert(revPath1, accum1, key, v)
              do
                local strm1 = strm[1]
                exp2 = skipWhiteSpaceAndGetc(strm1, strm[2])
              end
              if exp2.tag == "NONE" then
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "',' or '}'"}}}, _exn_meta), "parse_toml.sml:16:5")
              elseif exp2.tag == "SOME" and exp2.payload[1] == 44 then
                local strm_PRIME = exp2.payload[2]
                local tmp27
                do
                  local strm1 = strm_PRIME[1]
                  tmp27 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
                end
                tmp24 = revPath1
                tmp25 = accum_PRIME
                tmp26 = tmp27
                goto cont
              else
                goto else1
              end
            end
            ::else1::
            if exp2.tag == "SOME" and exp2.payload[1] == 125 then
              local strm_PRIME = exp2.payload[2]
              local tmp27 = finalize(accum_PRIME)
              return {tmp27, strm_PRIME}
            else
              goto else2
            end
            ::else2::
            if exp2.tag == "SOME" then
              local tmp27 = toString(exp2.payload[1])
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp27, expected = "',' or '}'"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:922:11")
            end
          end
          readArray = function(revPath, accum, strm)
            local tmp24, tmp25, tmp26 = revPath, accum, strm
            ::cont::
            local accum1, revPath1, r
            do
              local strm1
              revPath1, accum1, strm1 = tmp24, tmp25, tmp26
              r = skipWhiteSpaceOrCommentOrNewlineAndGetc(strm1)
              if r.tag == "SOME" and r.payload[1] == 93 then
                local strm_PRIME = r.payload[2]
                local tmp27 = revAppend(accum1, nil)
                return {{tag = "ARRAY", payload = tmp27}, strm_PRIME}
              else
                goto else1
              end
            end
            ::else1::
            local exp2
            do
              local exp3 = readValue(revPath1, r)
              local v = exp3[1]
              exp2 = skipWhiteSpaceOrCommentOrNewlineAndGetc(exp3[2])
              if exp2.tag == "NONE" then
                _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "',' or ']'"}}}, _exn_meta), "parse_toml.sml:16:5")
              elseif exp2.tag == "SOME" and exp2.payload[1] == 44 then
                local strm_PRIME_PRIME = exp2.payload[2]
                tmp24 = revPath1
                tmp25 = {v, accum1}
                tmp26 = strm_PRIME_PRIME
                goto cont
              elseif exp2.tag == "SOME" and exp2.payload[1] == 93 then
                local strm_PRIME_PRIME = exp2.payload[2]
                local tmp27 = revAppend({v, accum1}, nil)
                return {{tag = "ARRAY", payload = tmp27}, strm_PRIME_PRIME}
              else
                goto else2
              end
            end
            ::else2::
            if exp2.tag == "SOME" then
              local tmp27 = toString(exp2.payload[1])
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp27, expected = "',' or ']'"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:910:16")
            end
          end
          readValue = function(revPath, _2)
            if _2.tag == "NONE" then
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "value"}}}, _exn_meta), "parse_toml.sml:16:5")
            elseif _2.tag == "SOME" then
              local c = _2.payload[1]
              local strm = _2.payload[2]
              if c == 34 then
                do
                  local exp2
                  do
                    local strm1 = strm[1]
                    exp2 = tmp23(strm1, strm[2])
                  end
                  if exp2.tag == "NONE" then
                    _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, _exn_meta), "parse_toml.sml:16:5")
                  elseif exp2.tag == "SOME" and exp2.payload[1] == 34 then
                    local strm_PRIME = exp2.payload[2]
                    local exp3
                    do
                      local strm1 = strm_PRIME[1]
                      exp3 = tmp23(strm1, strm_PRIME[2])
                    end
                    if exp3.tag == "SOME" and exp3.payload[1] == 34 then
                      local tmp24 = skipOptionalNewline(exp3.payload[2])
                      local exp4
                      do
                        local strm1 = tmp24[1]
                        exp4 = go1(nil, strm1, tmp24[2])
                      end
                      local s = exp4[1]
                      local strm_PRIME_PRIME_PRIME = exp4[2]
                      return {{tag = "STRING", payload = s}, strm_PRIME_PRIME_PRIME}
                    else
                      return {{tag = "STRING", payload = ""}, strm_PRIME}
                    end
                  else
                    goto else13
                  end
                end
                ::else13::
                local exp2
                do
                  local strm1 = strm[1]
                  exp2 = go(nil, strm1, strm[2])
                end
                local s = exp2[1]
                local strm_PRIME = exp2[2]
                return {{tag = "STRING", payload = s}, strm_PRIME}
              else
                goto else1
              end
              ::else1::
              if c == 39 then
                do
                  local exp2
                  do
                    local strm1 = strm[1]
                    exp2 = tmp23(strm1, strm[2])
                  end
                  if exp2.tag == "NONE" then
                    _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, _exn_meta), "parse_toml.sml:16:5")
                  elseif exp2.tag == "SOME" and exp2.payload[1] == 39 then
                    local strm_PRIME = exp2.payload[2]
                    local exp3
                    do
                      local strm1 = strm_PRIME[1]
                      exp3 = tmp23(strm1, strm_PRIME[2])
                    end
                    if exp3.tag == "SOME" and exp3.payload[1] == 39 then
                      local strm_PRIME_PRIME_PRIME, s
                      do
                        local tmp24 = skipOptionalNewline(exp3.payload[2])
                        local tmp25, tmp26, tmp27
                        do
                          local strm1 = tmp24[1]
                          tmp25 = nil
                          tmp26 = strm1
                          tmp27 = tmp24[2]
                        end
                        ::cont1::
                        local accum, exp4
                        do
                          local strm1, state
                          accum, strm1, state = tmp25, tmp26, tmp27
                          exp4 = tmp23(strm1, state)
                          if exp4.tag == "NONE" then
                            _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, _exn_meta), "parse_toml.sml:16:5")
                          elseif exp4.tag == "SOME" and exp4.payload[1] == 39 then
                            local exp5
                            do
                              local strm_PRIME1 = exp4.payload[2]
                              do
                                local strm2 = strm_PRIME1[1]
                                exp5 = tmp23(strm2, strm_PRIME1[2])
                              end
                              if exp5.tag == "SOME" and exp5.payload[1] == 39 then
                                local exp6
                                do
                                  local strm_PRIME_PRIME = exp5.payload[2]
                                  do
                                    local strm2 = strm_PRIME_PRIME[1]
                                    exp6 = tmp23(strm2, strm_PRIME_PRIME[2])
                                  end
                                  if exp6.tag == "SOME" and exp6.payload[1] == 39 then
                                    local strm_PRIME_PRIME_PRIME1
                                    do
                                      strm_PRIME_PRIME_PRIME1 = exp6.payload[2]
                                      local exp7
                                      do
                                        local strm2 = strm_PRIME_PRIME_PRIME1[1]
                                        exp7 = tmp23(strm2, strm_PRIME_PRIME_PRIME1[2])
                                      end
                                      if exp7.tag == "SOME" and exp7.payload[1] == 39 then
                                        local strm_PRIME_PRIME_PRIME_PRIME
                                        do
                                          strm_PRIME_PRIME_PRIME_PRIME = exp7.payload[2]
                                          local exp8
                                          do
                                            local strm2 = strm_PRIME_PRIME_PRIME_PRIME[1]
                                            exp8 = tmp23(strm2, strm_PRIME_PRIME_PRIME_PRIME[2])
                                          end
                                          if exp8.tag == "SOME" and exp8.payload[1] == 39 then
                                            local strm_PRIME_PRIME_PRIME_PRIME_PRIME = exp8.payload[2]
                                            local tmp28 = implodeRev({39, {39, accum}})
                                            strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME_PRIME_PRIME_PRIME
                                            s = tmp28
                                            goto cont
                                          else
                                            goto else18
                                          end
                                        end
                                        ::else18::
                                        local tmp28 = implodeRev({39, accum})
                                        strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME_PRIME_PRIME
                                        s = tmp28
                                        goto cont
                                      else
                                        goto else17
                                      end
                                    end
                                    ::else17::
                                    local tmp28 = implodeRev(accum)
                                    strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME_PRIME1
                                    s = tmp28
                                    goto cont
                                  else
                                    goto else16
                                  end
                                end
                                ::else16::
                                if exp6.tag == "SOME" then
                                  local c1 = exp6.payload[1]
                                  local strm_PRIME_PRIME_PRIME1 = exp6.payload[2]
                                  local tmp28 = checkAllowedChar(c1)
                                  local tmp29 = {tmp28, {39, {39, accum}}}
                                  local strm2 = strm_PRIME_PRIME_PRIME1[1]
                                  tmp25 = tmp29
                                  tmp26 = strm2
                                  tmp27 = strm_PRIME_PRIME_PRIME1[2]
                                  goto cont1
                                elseif exp6.tag == "NONE" then
                                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, _exn_meta), "parse_toml.sml:16:5")
                                else
                                  _raise(_Match, "parse_toml.sml:267:21")
                                end
                              else
                                goto else15
                              end
                            end
                            ::else15::
                            if exp5.tag == "SOME" then
                              local c1 = exp5.payload[1]
                              local strm_PRIME_PRIME = exp5.payload[2]
                              local tmp28 = checkAllowedChar(c1)
                              local tmp29 = {tmp28, {39, accum}}
                              local strm2 = strm_PRIME_PRIME[1]
                              tmp25 = tmp29
                              tmp26 = strm2
                              tmp27 = strm_PRIME_PRIME[2]
                              goto cont1
                            elseif exp5.tag == "NONE" then
                              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, _exn_meta), "parse_toml.sml:16:5")
                            else
                              _raise(_Match, "parse_toml.sml:265:16")
                            end
                          else
                            goto else14
                          end
                        end
                        ::else14::
                        if exp4.tag == "SOME" then
                          local c1 = exp4.payload[1]
                          local strm_PRIME1 = exp4.payload[2]
                          local tmp28 = checkAllowedChar(c1)
                          local tmp29 = {tmp28, accum}
                          local strm1 = strm_PRIME1[1]
                          tmp25 = tmp29
                          tmp26 = strm1
                          tmp27 = strm_PRIME1[2]
                          goto cont1
                        else
                          _raise(_Match, "parse_toml.sml:262:11")
                        end
                      end
                      ::cont::
                      return {{tag = "STRING", payload = s}, strm_PRIME_PRIME_PRIME}
                    else
                      return {{tag = "STRING", payload = ""}, strm_PRIME}
                    end
                  else
                    goto else13
                  end
                end
                ::else13::
                local exp2
                do
                  local strm1 = strm[1]
                  exp2 = go2(nil, strm1, strm[2])
                end
                local s = exp2[1]
                local strm_PRIME = exp2[2]
                return {{tag = "STRING", payload = s}, strm_PRIME}
              else
                goto else2
              end
              ::else2::
              if c == 91 then
                return readArray({"[]", revPath}, nil, strm)
              else
                goto else3
              end
              ::else3::
              if c == 123 then
                local r
                do
                  local strm1 = strm[1]
                  r = skipWhiteSpaceAndGetc(strm1, strm[2])
                end
                if r.tag == "SOME" and r.payload[1] == 125 then
                  local strm_PRIME = r.payload[2]
                  return {{tag = "TABLE", payload = nil}, strm_PRIME}
                else
                  return readInlineTable(revPath, nil, r)
                end
              else
                goto else4
              end
              ::else4::
              if c == 116 then
                local tmp24 = {tag = "BOOL", payload = true}
                local tmp25
                do
                  local strm1 = strm[1]
                  tmp25 = expect(114, strm1, strm[2])
                end
                local tmp26
                do
                  local strm1 = tmp25[1]
                  tmp26 = expect(117, strm1, tmp25[2])
                end
                local tmp27
                do
                  local strm1 = tmp26[1]
                  tmp27 = expect(101, strm1, tmp26[2])
                end
                return {tmp24, tmp27}
              else
                goto else5
              end
              ::else5::
              if c == 102 then
                local tmp24 = {tag = "BOOL", payload = false}
                local tmp25
                do
                  local strm1 = strm[1]
                  tmp25 = expect(97, strm1, strm[2])
                end
                local tmp26
                do
                  local strm1 = tmp25[1]
                  tmp26 = expect(108, strm1, tmp25[2])
                end
                local tmp27
                do
                  local strm1 = tmp26[1]
                  tmp27 = expect(115, strm1, tmp26[2])
                end
                local tmp28
                do
                  local strm1 = tmp27[1]
                  tmp28 = expect(101, strm1, tmp27[2])
                end
                return {tmp24, tmp28}
              else
                goto else6
              end
              ::else6::
              if c == 105 then
                local tmp24 = _L[188]("inf")
                local tmp25
                do
                  local strm1 = strm[1]
                  tmp25 = expect(110, strm1, strm[2])
                end
                local tmp26
                do
                  local strm1 = tmp25[1]
                  tmp26 = expect(102, strm1, tmp25[2])
                end
                return {tmp24, tmp26}
              else
                goto else7
              end
              ::else7::
              if c == 110 then
                local tmp24 = _L[188]("nan")
                local tmp25
                do
                  local strm1 = strm[1]
                  tmp25 = expect(97, strm1, strm[2])
                end
                local tmp26
                do
                  local strm1 = tmp25[1]
                  tmp26 = expect(110, strm1, tmp25[2])
                end
                return {tmp24, tmp26}
              else
                goto else8
              end
              ::else8::
              if c == 43 then
                local exp2
                do
                  local strm1 = strm[1]
                  exp2 = tmp23(strm1, strm[2])
                end
                if exp2.tag == "NONE" then
                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit or 'inf' or 'nan'"}}}, _exn_meta), "parse_toml.sml:16:5")
                elseif exp2.tag == "SOME" and exp2.payload[1] == 105 then
                  local strm_PRIME = exp2.payload[2]
                  local tmp24 = _L[188]("+inf")
                  local tmp25
                  do
                    local strm1 = strm_PRIME[1]
                    tmp25 = expect(110, strm1, strm_PRIME[2])
                  end
                  local tmp26
                  do
                    local strm1 = tmp25[1]
                    tmp26 = expect(102, strm1, tmp25[2])
                  end
                  return {tmp24, tmp26}
                else
                  goto else13
                end
                ::else13::
                if exp2.tag == "SOME" and exp2.payload[1] == 110 then
                  local strm_PRIME = exp2.payload[2]
                  local tmp24 = _L[188]("+nan")
                  local tmp25
                  do
                    local strm1 = strm_PRIME[1]
                    tmp25 = expect(97, strm1, strm_PRIME[2])
                  end
                  local tmp26
                  do
                    local strm1 = tmp25[1]
                    tmp26 = expect(110, strm1, tmp25[2])
                  end
                  return {tmp24, tmp26}
                else
                  goto else14
                end
                ::else14::
                if exp2.tag == "SOME" then
                  local c_PRIME
                  do
                    c_PRIME = exp2.payload[1]
                    local strm_PRIME = exp2.payload[2]
                    if 48 <= c_PRIME and c_PRIME <= 57 then
                      return readSigned(43, c_PRIME, strm_PRIME)
                    else
                      goto else15
                    end
                  end
                  ::else15::
                  local tmp24 = toString(c_PRIME)
                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "digit or 'inf' or 'nan'"}}}, _exn_meta), "parse_toml.sml:19:5")
                else
                  _raise(_Match, "parse_toml.sml:845:18")
                end
              else
                goto else9
              end
              ::else9::
              if c == 45 then
                local exp2
                do
                  local strm1 = strm[1]
                  exp2 = tmp23(strm1, strm[2])
                end
                if exp2.tag == "NONE" then
                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit or 'inf' or 'nan'"}}}, _exn_meta), "parse_toml.sml:16:5")
                elseif exp2.tag == "SOME" and exp2.payload[1] == 105 then
                  local strm_PRIME = exp2.payload[2]
                  local tmp24 = _L[188]("-inf")
                  local tmp25
                  do
                    local strm1 = strm_PRIME[1]
                    tmp25 = expect(110, strm1, strm_PRIME[2])
                  end
                  local tmp26
                  do
                    local strm1 = tmp25[1]
                    tmp26 = expect(102, strm1, tmp25[2])
                  end
                  return {tmp24, tmp26}
                else
                  goto else13
                end
                ::else13::
                if exp2.tag == "SOME" and exp2.payload[1] == 110 then
                  local strm_PRIME = exp2.payload[2]
                  local tmp24 = _L[188]("-nan")
                  local tmp25
                  do
                    local strm1 = strm_PRIME[1]
                    tmp25 = expect(97, strm1, strm_PRIME[2])
                  end
                  local tmp26
                  do
                    local strm1 = tmp25[1]
                    tmp26 = expect(110, strm1, tmp25[2])
                  end
                  return {tmp24, tmp26}
                else
                  goto else14
                end
                ::else14::
                if exp2.tag == "SOME" then
                  local c_PRIME
                  do
                    c_PRIME = exp2.payload[1]
                    local strm_PRIME = exp2.payload[2]
                    if 48 <= c_PRIME and c_PRIME <= 57 then
                      return readSigned(45, c_PRIME, strm_PRIME)
                    else
                      goto else15
                    end
                  end
                  ::else15::
                  local tmp24 = toString(c_PRIME)
                  _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "digit or 'inf' or 'nan'"}}}, _exn_meta), "parse_toml.sml:19:5")
                else
                  _raise(_Match, "parse_toml.sml:857:18")
                end
              else
                goto else10
              end
              ::else10::
              if c == 48 then
                local exp2
                do
                  local strm1 = strm[1]
                  exp2 = tmp23(strm1, strm[2])
                end
                if exp2.tag == "NONE" then
                  local tmp24 = fromInt(0)
                  return {{tag = "INTEGER", payload = tmp24}, strm}
                else
                  goto else13
                end
                ::else13::
                if exp2.tag == "SOME" and exp2.payload[1] == 120 then
                  local strm_PRIME = exp2.payload[2]
                  local tmp24 = fromInt(0)
                  local exp3
                  do
                    local strm1 = strm_PRIME[1]
                    exp3 = readHexInt(tmp24, strm1, strm_PRIME[2])
                  end
                  local x = exp3[1]
                  local strm_PRIME_PRIME = exp3[2]
                  return {{tag = "INTEGER", payload = x}, strm_PRIME_PRIME}
                else
                  goto else14
                end
                ::else14::
                if exp2.tag == "SOME" and exp2.payload[1] == 111 then
                  local strm_PRIME = exp2.payload[2]
                  local tmp24 = fromInt(0)
                  local exp3
                  do
                    local strm1 = strm_PRIME[1]
                    exp3 = readOctInt(tmp24, strm1, strm_PRIME[2])
                  end
                  local x = exp3[1]
                  local strm_PRIME_PRIME = exp3[2]
                  return {{tag = "INTEGER", payload = x}, strm_PRIME_PRIME}
                else
                  goto else15
                end
                ::else15::
                if exp2.tag == "SOME" and exp2.payload[1] == 98 then
                  local strm_PRIME = exp2.payload[2]
                  local tmp24 = fromInt(0)
                  local exp3
                  do
                    local strm1 = strm_PRIME[1]
                    exp3 = readBinInt(tmp24, strm1, strm_PRIME[2])
                  end
                  local x = exp3[1]
                  local strm_PRIME_PRIME = exp3[2]
                  return {{tag = "INTEGER", payload = x}, strm_PRIME_PRIME}
                elseif exp2.tag == "SOME" then
                  return readUnsigned(48, strm)
                else
                  _raise(_Match, "parse_toml.sml:869:18")
                end
              else
                goto else11
              end
              ::else11::
              if 48 <= c and c <= 57 then
                return readUnsigned(c, strm)
              else
                goto else12
              end
              ::else12::
              local tmp24 = toString(c)
              _raise(setmetatable({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp24, expected = "value"}}}, _exn_meta), "parse_toml.sml:19:5")
            else
              _raise(_Match, "parse_toml.sml:788:11")
            end
          end
          insertKeyval = function(revPath, pt, _3, keys, v)
            if _3 == nil then
              return insert(revPath, pt, keys, v)
            else
              goto else1
            end
            ::else1::
            if _3 ~= nil then
              local tmp24 = _3[1]
              local tmp25 = _3[2]
              local tmp26, tmp27 = nil, pt
              ::cont::
              local accum, _2 = tmp26, tmp27
              if _2 == nil then
                local tmp28 = insertKeyval({tmp24, revPath}, nil, tmp25, keys, v)
                return revAppend(accum, {{tmp24, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_HEADER, tmp28}}}, nil})
              else
                goto else2
              end
              ::else2::
              if _2 ~= nil then
                local tmp28 = _2[1]
                local key = _2[1][1]
                local pv = _2[1][2]
                local tmp29 = _2[2]
                if tmp24 == key then
                  local updated
                  if pv.tag == "LEAF" then
                    local tmp30 = revAppend(revPath, {key, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp30}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else3
                  end
                  ::else3::
                  if pv.tag == "PARTIAL_TABLE" then
                    local definedBy = pv.payload[1]
                    local pt_PRIME = pv.payload[2]
                    local tmp30 = insertKeyval({tmp24, revPath}, pt_PRIME, tmp25, keys, v)
                    updated = {tag = "PARTIAL_TABLE", payload = {definedBy, tmp30}}
                    goto cont1
                  else
                    goto else4
                  end
                  ::else4::
                  if pv.tag == "PARTIAL_ARRAY" then
                    local last = pv.payload[1]
                    local rest1 = pv.payload[2]
                    local tmp30 = insertKeyval({tmp24, revPath}, last, tmp25, keys, v)
                    updated = {tag = "PARTIAL_ARRAY", payload = {tmp30, rest1}}
                  else
                    _raise(_Match, "parse_toml.sml:988:27")
                  end
                  ::cont1::
                  return revAppend(accum, {{key, updated}, tmp29})
                else
                  tmp26 = {tmp28, accum}
                  tmp27 = tmp29
                  goto cont
                end
              else
                _raise(_Match, "parse_toml.sml:976:19")
              end
            else
              _raise(_Match, "parse_toml.sml:972:11")
            end
          end
          insertTable = function(revPath, pt, _3)
            if _3 ~= nil and _3[2] == nil then
              local tmp24 = _3[1]
              local tmp25, tmp26 = nil, pt
              ::cont::
              local accum, _2 = tmp25, tmp26
              if _2 == nil then
                return revAppend(accum, {{tmp24, {tag = "PARTIAL_TABLE", payload = {EXACT_HEADER, nil}}}, nil})
              else
                goto else2
              end
              ::else2::
              if _2 ~= nil then
                local tmp27 = _2[1]
                local key_PRIME = _2[1][1]
                local pv = _2[1][2]
                local tmp28 = _2[2]
                if tmp24 == key_PRIME then
                  if pv.tag == "LEAF" then
                    local tmp29 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp29}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else3
                  end
                  ::else3::
                  if pv.tag == "PARTIAL_TABLE" and pv.payload[1] == "IMPLICIT_HEADER" then
                    return revAppend(accum, {{key_PRIME, {tag = "PARTIAL_TABLE", payload = {EXACT_HEADER, pv.payload[2]}}}, tmp28})
                  else
                    goto else4
                  end
                  ::else4::
                  if pv.tag == "PARTIAL_TABLE" then
                    local tmp29 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp29}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else5
                  end
                  ::else5::
                  if pv.tag == "PARTIAL_ARRAY" then
                    local tmp29 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp29}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    _raise(_Match, "parse_toml.sml:1017:23")
                  end
                else
                  tmp25 = {tmp27, accum}
                  tmp26 = tmp28
                  goto cont
                end
              else
                _raise(_Match, "parse_toml.sml:1010:19")
              end
            else
              goto else1
            end
            ::else1::
            if _3 ~= nil then
              local tmp24 = _3[1]
              local tmp25 = _3[2]
              local tmp26, tmp27 = nil, pt
              ::cont::
              local accum, _2 = tmp26, tmp27
              if _2 == nil then
                local tmp28 = insertTable({tmp24, revPath}, nil, tmp25)
                return revAppend(accum, {{tmp24, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_HEADER, tmp28}}}, nil})
              else
                goto else2
              end
              ::else2::
              if _2 ~= nil then
                local tmp28 = _2[1]
                local key_PRIME = _2[1][1]
                local pv = _2[1][2]
                local tmp29 = _2[2]
                if tmp24 == key_PRIME then
                  local updated
                  if pv.tag == "LEAF" then
                    local tmp30 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp30}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else3
                  end
                  ::else3::
                  if pv.tag == "PARTIAL_TABLE" then
                    local definedBy = pv.payload[1]
                    local pt_PRIME = pv.payload[2]
                    local tmp30 = insertTable({tmp24, revPath}, pt_PRIME, tmp25)
                    updated = {tag = "PARTIAL_TABLE", payload = {definedBy, tmp30}}
                    goto cont1
                  else
                    goto else4
                  end
                  ::else4::
                  if pv.tag == "PARTIAL_ARRAY" then
                    local last = pv.payload[1]
                    local rest1 = pv.payload[2]
                    local tmp30 = insertTable({tmp24, revPath}, last, tmp25)
                    updated = {tag = "PARTIAL_ARRAY", payload = {tmp30, rest1}}
                  else
                    _raise(_Match, "parse_toml.sml:1050:27")
                  end
                  ::cont1::
                  return revAppend(accum, {{key_PRIME, updated}, tmp29})
                else
                  tmp26 = {tmp28, accum}
                  tmp27 = tmp29
                  goto cont
                end
              else
                _raise(_Match, "parse_toml.sml:1036:19")
              end
            elseif _3 == nil then
              _raise(_Match, "parse_toml.sml:1069:36")
            else
              _raise(_Match, "parse_toml.sml:1008:11")
            end
          end
        end
        local table
        do
          local function insertArrayTable(revPath, pt, _3)
            if _3 ~= nil and _3[2] == nil then
              local tmp24 = _3[1]
              local tmp25, tmp26 = nil, pt
              ::cont::
              local accum, _2 = tmp25, tmp26
              if _2 == nil then
                return revAppend(accum, {{tmp24, {tag = "PARTIAL_ARRAY", payload = {nil, nil}}}, nil})
              else
                goto else2
              end
              ::else2::
              if _2 ~= nil then
                local tmp27 = _2[1]
                local key_PRIME = _2[1][1]
                local pv = _2[1][2]
                local tmp28 = _2[2]
                if tmp24 == key_PRIME then
                  if pv.tag == "LEAF" then
                    local tmp29 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp29}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else3
                  end
                  ::else3::
                  if pv.tag == "PARTIAL_TABLE" then
                    local tmp29 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp29}}, _exn_meta), "parse_toml.sml:22:5")
                  elseif pv.tag == "PARTIAL_ARRAY" then
                    local last = pv.payload[1]
                    return revAppend(accum, {{tmp24, {tag = "PARTIAL_ARRAY", payload = {nil, {last, pv.payload[2]}}}}, tmp28})
                  else
                    _raise(_Match, "parse_toml.sml:1079:23")
                  end
                else
                  tmp25 = {tmp27, accum}
                  tmp26 = tmp28
                  goto cont
                end
              else
                _raise(_Match, "parse_toml.sml:1072:19")
              end
            else
              goto else1
            end
            ::else1::
            if _3 ~= nil then
              local tmp24 = _3[1]
              local tmp25 = _3[2]
              local tmp26, tmp27 = nil, pt
              ::cont::
              local accum, _2 = tmp26, tmp27
              if _2 == nil then
                local tmp28 = insertArrayTable({tmp24, revPath}, nil, tmp25)
                return revAppend(accum, {{tmp24, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_HEADER, tmp28}}}, nil})
              else
                goto else2
              end
              ::else2::
              if _2 ~= nil then
                local tmp28 = _2[1]
                local key_PRIME = _2[1][1]
                local pv = _2[1][2]
                local tmp29 = _2[2]
                if tmp24 == key_PRIME then
                  local updated
                  if pv.tag == "LEAF" then
                    local tmp30 = revAppend(revPath, {tmp24, nil})
                    _raise(setmetatable({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp30}}, _exn_meta), "parse_toml.sml:22:5")
                  else
                    goto else3
                  end
                  ::else3::
                  if pv.tag == "PARTIAL_TABLE" then
                    local definedBy = pv.payload[1]
                    local pt_PRIME = pv.payload[2]
                    local tmp30 = insertArrayTable({tmp24, revPath}, pt_PRIME, tmp25)
                    updated = {tag = "PARTIAL_TABLE", payload = {definedBy, tmp30}}
                    goto cont1
                  else
                    goto else4
                  end
                  ::else4::
                  if pv.tag == "PARTIAL_ARRAY" then
                    local last = pv.payload[1]
                    local rest1 = pv.payload[2]
                    local tmp30 = insertArrayTable({tmp24, revPath}, last, tmp25)
                    updated = {tag = "PARTIAL_ARRAY", payload = {tmp30, rest1}}
                  else
                    _raise(_Match, "parse_toml.sml:1109:27")
                  end
                  ::cont1::
                  return revAppend(accum, {{tmp24, updated}, tmp29})
                else
                  tmp26 = {tmp28, accum}
                  tmp27 = tmp29
                  goto cont
                end
              else
                _raise(_Match, "parse_toml.sml:1095:19")
              end
            elseif _3 == nil then
              _raise(_Match, "parse_toml.sml:1130:41")
            else
              _raise(_Match, "parse_toml.sml:1070:11")
            end
          end
          local tmp24, tmp25, tmp26, tmp27, tmp28 = nil, nil, nil, tmp22, _L[173]
          ::cont1::
          local prefix, revPath, accum, exp2
          do
            local strm, state
            accum, revPath, prefix, strm, state = tmp24, tmp25, tmp26, tmp27, tmp28
            local tmp29, tmp30, tmp31 = revPath, strm, state
            ::cont3::
            local revPath1, r
            do
              local strm1, state1
              revPath1, strm1, state1 = tmp29, tmp30, tmp31
              r = skipWhiteSpaceAndGetc(strm1, state1)
              if r.tag == "NONE" then
                exp2 = {tag = "NONE"}
                goto cont2
              elseif r.tag == "SOME" and r.payload[1] == 10 then
                local strm_PRIME = r.payload[2]
                local strm2 = strm_PRIME[1]
                tmp29 = revPath1
                tmp30 = strm2
                tmp31 = strm_PRIME[2]
                goto cont3
              elseif r.tag == "SOME" and r.payload[1] == 13 then
                local strm_PRIME = r.payload[2]
                local tmp32
                do
                  local strm2 = strm_PRIME[1]
                  tmp32 = expect(10, strm2, strm_PRIME[2])
                end
                local strm2 = tmp32[1]
                tmp29 = revPath1
                tmp30 = strm2
                tmp31 = tmp32[2]
                goto cont3
              else
                goto else4
              end
            end
            ::else4::
            if r.tag == "SOME" and r.payload[1] == 35 then
              local tmp32 = skipUntilNewline(r.payload[2])
              local strm1 = tmp32[1]
              tmp29 = revPath1
              tmp30 = strm1
              tmp31 = tmp32[2]
              goto cont3
            else
              goto else5
            end
            ::else5::
            if r.tag == "SOME" and r.payload[1] == 91 then
              local strm_PRIME
              do
                strm_PRIME = r.payload[2]
                local exp3
                do
                  local strm1 = strm_PRIME[1]
                  exp3 = tmp23(strm1, strm_PRIME[2])
                end
                if exp3.tag == "SOME" and exp3.payload[1] == 91 then
                  local strm_PRIME_PRIME = exp3.payload[2]
                  local tmp32
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp32 = skipWhiteSpaceAndGetc(strm1, strm_PRIME_PRIME[2])
                  end
                  local exp4 = readKey(tmp32)
                  local key = exp4[1]
                  local strm_PRIME_PRIME_PRIME = exp4[2]
                  local tmp33
                  do
                    local strm1 = strm_PRIME_PRIME_PRIME[1]
                    tmp33 = expect(93, strm1, strm_PRIME_PRIME_PRIME[2])
                  end
                  local strm_PRIME_PRIME_PRIME_PRIME
                  do
                    local strm1 = tmp33[1]
                    strm_PRIME_PRIME_PRIME_PRIME = expect(93, strm1, tmp33[2])
                  end
                  local tmp34 = {tag = "ARRAY_TABLE", payload = key}
                  local tmp35 = skipWhiteSpaceOrComment(strm_PRIME_PRIME_PRIME_PRIME)
                  exp2 = {tag = "SOME", payload = {tmp34, tmp35}}
                  goto cont2
                else
                  goto else7
                end
              end
              ::else7::
              local tmp32
              do
                local strm1 = strm_PRIME[1]
                tmp32 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
              end
              local exp3 = readKey(tmp32)
              local key = exp3[1]
              local strm_PRIME_PRIME = exp3[2]
              local strm_PRIME_PRIME_PRIME
              do
                local strm1 = strm_PRIME_PRIME[1]
                strm_PRIME_PRIME_PRIME = expect(93, strm1, strm_PRIME_PRIME[2])
              end
              local tmp33 = {tag = "STD_TABLE", payload = key}
              local tmp34 = skipWhiteSpaceOrComment(strm_PRIME_PRIME_PRIME)
              exp2 = {tag = "SOME", payload = {tmp33, tmp34}}
              goto cont2
            else
              goto else6
            end
            ::else6::
            local exp3 = readKeyval(revPath1, r)
            local key = exp3[1]
            local v = exp3[2]
            local strm_PRIME = exp3[3]
            local tmp32 = {tag = "KEYVAL", payload = {key, v}}
            local tmp33 = skipWhiteSpaceOrComment(strm_PRIME)
            exp2 = {tag = "SOME", payload = {tmp32, tmp33}}
          end
          ::cont2::
          if exp2.tag == "NONE" then
          else
            goto else1
          end
          table = finalizeTable(accum)
          goto cont
          ::else1::
          if exp2.tag == "SOME" and exp2.payload[1].tag == "ARRAY_TABLE" then
            local key = exp2.payload[1].payload
            local strm_PRIME = exp2.payload[2]
            local tmp29 = insertArrayTable(nil, accum, key)
            local tmp30 = revAppend(key, nil)
            local tmp31 = {"[]", tmp30}
            local strm = strm_PRIME[1]
            tmp24 = tmp29
            tmp25 = tmp31
            tmp26 = key
            tmp27 = strm
            tmp28 = strm_PRIME[2]
            goto cont1
          else
            goto else2
          end
          ::else2::
          if exp2.tag == "SOME" and exp2.payload[1].tag == "STD_TABLE" then
            local key = exp2.payload[1].payload
            local strm_PRIME = exp2.payload[2]
            local tmp29 = insertTable(nil, accum, key)
            local tmp30 = revAppend(key, nil)
            local strm = strm_PRIME[1]
            tmp24 = tmp29
            tmp25 = tmp30
            tmp26 = key
            tmp27 = strm
            tmp28 = strm_PRIME[2]
            goto cont1
          else
            goto else3
          end
          ::else3::
          if exp2.tag == "SOME" and exp2.payload[1].tag == "KEYVAL" then
            local key = exp2.payload[1].payload[1]
            local v = exp2.payload[1].payload[2]
            local strm_PRIME = exp2.payload[2]
            local tmp29 = insertKeyval(nil, accum, prefix, key, v)
            local strm = strm_PRIME[1]
            tmp24 = tmp29
            tmp25 = revPath
            tmp26 = prefix
            tmp27 = strm
            tmp28 = strm_PRIME[2]
            goto cont1
          else
            _raise(_Match, "parse_toml.sml:1132:9")
          end
        end
        ::cont::
        local tmp24, color, tmp25, tmp26, tmp27, tmp28
        do
          local tmp29 = _L[399](table, "temporary-directory")
          tmp24 = _L[401](tmp29, function(a)
            if a.tag == "STRING" then
              return {tag = "SOME", payload = a.payload}
            end
            _L[309]("Config entry temporary-directory should be a string.")
            return {tag = "NONE"}
          end)
          local tmp30 = _L[399](table, "color")
          color = _L[401](tmp30, function(a)
            if a.tag == "TABLE" then
              return {tag = "SOME", payload = a.payload}
            end
            _L[309]("Config entry color should be a table.")
            return {tag = "NONE"}
          end)
          local tmp31 = _L[400](color, "type")
          local tmp32 = _L[404]("color.type")
          tmp25 = _L[401](tmp31, tmp32)
          local tmp33 = _L[400](color, "execute")
          local tmp34 = _L[404]("color.execute")
          tmp26 = _L[401](tmp33, tmp34)
          local tmp35 = _L[400](color, "error")
          local tmp36 = _L[404]("color.error")
          tmp27 = _L[401](tmp35, tmp36)
          local tmp37 = _L[400](color, "warning")
          local tmp38 = _L[404]("color.warning")
          tmp28 = _L[401](tmp37, tmp38)
        end
        local tmp29 = _L[400](color, "diagnostic")
        local tmp30 = _L[404]("color.diagnostic")
        local tmp31 = _L[401](tmp29, tmp30)
        local tmp32 = _L[400](color, "information")
        local tmp33 = _L[404]("color.information")
        local tmp34 = _L[401](tmp32, tmp33)
        return {color = {diagnostic = tmp31, error = tmp27, execute = tmp26, information = tmp34, type_ = tmp25, warning = tmp28}, temporary_directory = tmp24}
      end)
      if not status then
        if __exn_instanceof(exn, Io_tag) then
          config = _L[398]
          goto cont1
        elseif __exn_instanceof(exn, _L[171]) then
        else
          goto else4
        end
        _L[308]("Config file " .. path .. " is not UTF-8 encoded.")
        config = _L[398]
        goto cont1
        ::else4::
        if __exn_instanceof(exn, _L[186]) then
          local tmp22, tmp23
          do
            local e = exn.payload
            tmp22 = "Config file " .. path .. " is not a valid TOML file: "
            if e.tag == "UNEXPECTED" then
              local encountered = e.payload.encountered
              local expected = e.payload.expected
              tmp23 = "unexpected " .. encountered .. ", expected " .. expected
              goto cont14
            elseif e.tag == "PREFIX_ZERO" then
              tmp23 = "prefix 0 is disallowed"
              goto cont14
            elseif e.tag == "INVALID_UNICODE_SCALAR" then
              tmp23 = "invalid Unicode scalar"
              goto cont14
            elseif e.tag == "INVALID_DATE" then
              tmp23 = "invalid date"
              goto cont14
            elseif e.tag == "INVALID_TIME" then
              tmp23 = "invalid time"
              goto cont14
            elseif e.tag == "DUPLICATE_KEY" then
              local path1 = e.payload
              local tmp24 = map(_L[185])
              local tmp25 = tmp24(path1)
              local tmp26 = _VectorOrArray_fromList(tmp25)
              local tmp27 = table_concat(tmp26, ".")
              tmp23 = "duplicate key at " .. tmp27
            else
              _raise(_Match, "error.sml:24:9")
            end
          end
          ::cont14::
          _L[308](tmp22 .. tmp23)
          config = _L[398]
        else
          _raise(exn, nil)
        end
      else
        config = exn
      end
    else
      _raise(_Match, "main.sml:778:32")
    end
  end
  ::cont1::
  local watch
  do
    local tmp21 = app(_L[301])
    tmp21(config.color.type_)
    local tmp22 = app(_L[302])
    tmp22(config.color.execute)
    local tmp23 = app(_L[303])
    tmp23(config.color.error)
    local tmp24 = app(_L[304])
    tmp24(config.color.warning)
    local tmp25 = app(_L[305])
    tmp25(config.color.diagnostic)
    local tmp26 = app(_L[306])
    tmp26(config.color.information)
    watch = options.watch
    if options.color.tag == "NONE" then
    else
      goto cont2
    end
    _L[282](_L[279])
  end
  ::cont2::
  local inputfile
  if rest == nil then
  else
    goto else1
  end
  inputfile = _L[408](1)
  goto cont3
  ::else1::
  if rest ~= nil and rest[2] == nil then
    inputfile = rest[1]
    goto cont3
  end
  _L[308]("Multiple input files are not supported.")
  exit(1, true)
  ::cont3::
  local supports_pdf_generation, supports_draftmode, name, executable, engine_type, dvi_extension
  do
    local exp1 = options.engine
    if exp1.tag == "SOME" then
      local name1 = exp1.payload
      local exp2 = _L[256](name1)
      if exp2.tag == "SOME" then
        local engine = exp2.payload
        local dvi_extension1 = engine.dvi_extension
        local engine_type1 = engine.engine_type
        local executable1 = engine.executable
        local name2 = engine.name
        local supports_draftmode1 = engine.supports_draftmode
        supports_pdf_generation = engine.supports_pdf_generation
        supports_draftmode = supports_draftmode1
        name = name2
        executable = executable1
        engine_type = engine_type1
        dvi_extension = dvi_extension1
        goto cont4
      elseif exp2.tag == "NONE" then
      else
        _raise(_Match, "main.sml:807:50")
      end
      _L[308]("Unknown engine name '" .. name1 .. "'.")
      exit(1, true)
    else
      goto else4
    end
    ::else4::
    if exp1.tag == "NONE" then
      local basename, tmp21
      do
        local name1
        do
          local name2 = _L[153][0]
          if name2 == nil then
            _raise(setmetatable({tag = _Fail_tag, payload = "CommandLine.name: arg is not available"}, _exn_meta), "command-line.sml:10:45")
          else
            name1 = name2
          end
        end
        local tmp22 = _L[211](name1)
        basename = _L[213](tmp22)
        local tmp23 = isPrefix("cl")
        local tmp24 = tmp23(basename)
        if tmp24 then
          local tmp25 = tmp7(isAlphaNum)
          tmp21 = tmp25(basename)
        else
          tmp21 = false
          goto cont13
        end
      end
      ::cont13::
      if tmp21 then
        local tmp22 = extract(basename, 2, {tag = "NONE"})
        local exp2 = _L[256](tmp22)
        if exp2.tag == "NONE" then
        elseif exp2.tag == "SOME" then
          local engine = exp2.payload
          local dvi_extension1 = engine.dvi_extension
          local engine_type1 = engine.engine_type
          local executable1 = engine.executable
          local name1 = engine.name
          local supports_draftmode1 = engine.supports_draftmode
          supports_pdf_generation = engine.supports_pdf_generation
          supports_draftmode = supports_draftmode1
          name = name1
          executable = executable1
          engine_type = engine_type1
          dvi_extension = dvi_extension1
          goto cont4
        else
          _raise(_Match, "main.sml:820:51")
        end
        _L[308]("Engine not specified.")
        exit(1, true)
      else
        goto else5
      end
      ::else5::
      _L[308]("Engine not specified.")
      exit(1, true)
    else
      _raise(_Match, "main.sml:806:32")
    end
  end
  ::cont4::
  local output_format = getOpt(options.output_format, _L[230])
  local check_driver
  if output_format == "PDF" then
    do
      local exp1 = options.check_driver
      if exp1.tag == "NONE" then
        goto cont13
      elseif exp1.tag == "SOME" then
      else
        _raise(_Match, "main.sml:829:44")
      end
      _L[308]("--check-driver can only be used when the output format is DVI.")
      exit(1, true)
    end
    ::cont13::
    if supports_pdf_generation then
      local tmp21 = _L[239](engine_type, _L[237])
      if tmp21 then
        check_driver = {tag = "SOME", payload = _L[317]}
        goto cont5
      end
      local tmp22 = _L[239](engine_type, _L[236])
      if tmp22 then
        check_driver = {tag = "SOME", payload = _L[316]}
        goto cont5
      end
      local tmp23 = _L[239](engine_type, _L[235])
      if tmp23 then
        check_driver = {tag = "SOME", payload = _L[315]}
        goto cont5
      end
      _L[309]("Unknown engine: " .. name)
      _L[309]("Driver check will not work.")
      check_driver = {tag = "NONE"}
      goto cont5
    else
      check_driver = {tag = "SOME", payload = _L[312]}
      goto cont5
    end
  else
    goto else2
  end
  ::else2::
  if output_format == "DVI" then
    local exp1 = options.check_driver
    if exp1.tag == "SOME" and exp1.payload == "DVIPDFMX" then
      check_driver = {tag = "SOME", payload = _L[312]}
    elseif exp1.tag == "SOME" and exp1.payload == "DVIPS" then
      check_driver = {tag = "SOME", payload = _L[313]}
    elseif exp1.tag == "SOME" and exp1.payload == "DVISVGM" then
      check_driver = {tag = "SOME", payload = _L[314]}
    elseif exp1.tag == "NONE" then
      check_driver = {tag = "NONE"}
    else
      _raise(_Match, "main.sml:851:42")
    end
  else
    _raise(_Match, "main.sml:827:38")
  end
  ::cont5::
  local jobname_for_output, jobname
  do
    local exp1 = options.jobname
    if exp1.tag == "SOME" then
      local jobname1 = exp1.payload
      jobname_for_output = jobname1
      jobname = jobname1
      goto cont6
    elseif exp1.tag == "NONE" then
      local tmp21 = _L[213](inputfile)
      local basename = _L[211](tmp21)
      local tmp22
      do
        local tmp23 = translate(_L[233])
        tmp22 = tmp23(basename)
      end
      jobname_for_output = basename
      jobname = tmp22
    else
      _raise(_Match, "main.sml:856:55")
    end
  end
  ::cont6::
  local output_extension, output_from_original_wd, output_directory_from_original_wd
  do
    if output_format == "DVI" then
      output_extension = dvi_extension
    elseif output_format == "PDF" then
      output_extension = "pdf"
    else
      _raise(_Match, "main.sml:861:42")
    end
    local exp1 = options.output
    if exp1.tag == "NONE" then
      output_from_original_wd = jobname_for_output .. "." .. output_extension
    elseif exp1.tag == "SOME" then
      output_from_original_wd = exp1.payload
    else
      _raise(_Match, "main.sml:864:49")
    end
    local exp2 = options.output_directory
    if exp2.tag == "SOME" then
      local dir = exp2.payload
      if options.fresh then
      else
        output_directory_from_original_wd = dir
        goto cont7
      end
      _L[308]("--fresh and --output-directory cannot be used together.")
      exit(1, true)
    else
      goto else4
    end
    ::else4::
    if exp2.tag == "NONE" then
      local output_directory
      do
        local inputfile_abs = _L[215]({tag = "NONE"}, inputfile)
        local tmp21 = config.temporary_directory
        local tmp22 = getOpt(options.engine_executable, executable)
        do
          local hash, tmpdir
          do
            local tmp23 = {n = 3, inputfile_abs, jobname, tmp22}
            local message = table_concat(tmp23, "\x00")
            do
              local tmp24 = _L[191](message)
              local a = tmp24[1]
              local b = tmp24[2]
              local c = tmp24[3]
              hash = _L[193](a, b, c, tmp24[4])
            end
            if tmp21.tag == "SOME" then
              tmpdir = tmp21.payload
              goto cont13
            elseif tmp21.tag == "NONE" then
              local exp3 = _L[407]({"TMPDIR", {"TMP", {"TEMP", nil}}})
              if exp3.tag == "SOME" then
                tmpdir = exp3.payload
                goto cont13
              elseif exp3.tag == "NONE" then
                local exp4 = _L[407]({"HOME", {"USERPROFILE", nil}})
                if exp4.tag == "SOME" then
                elseif exp4.tag == "NONE" then
                  _raise(setmetatable({tag = _Fail_tag, payload = "environment variable 'TMPDIR' not set!"}, _exn_meta), "main.sml:40:60")
                else
                  _raise(_Match, "main.sml:38:48")
                end
                tmpdir = joinDirFile({dir = exp4.payload, file = ".latex-build-temp"})
              else
                _raise(_Match, "main.sml:36:36")
              end
            else
              _raise(_Match, "main.sml:34:24")
            end
          end
          ::cont13::
          output_directory = joinDirFile({dir = tmpdir, file = "cluttex-" .. hash})
        end
        local tmp23 = _L[219](output_directory)
        if not tmp23 then
          local tmp24, tmp25 = _L[217].mkdir_rec(output_directory)
          if not tmp24 then
            local tmp26 = _id(tmp25)
            _raise(tmp26, "fs-util.sml:16:28")
          else
            output_directory_from_original_wd = output_directory
            goto cont7
          end
        else
          goto else5
        end
      end
      ::else5::
      if options.fresh and not options.print_output_directory then
        do
          local tmp21 = _ENV.CLUTTEX_VERBOSITY
          if tmp21 >= 1 then
          else
            goto cont13
          end
          _L[311]("Cleaning '" .. output_directory .. "'...")
        end
        ::cont13::
        do
          local tmp21, tmp22 = _L[217].remove_rec(output_directory)
          if not tmp21 then
            local tmp23 = _id(tmp22)
            _raise(tmp23, "fs-util.sml:22:29")
          else
            goto else6
          end
        end
        ::else6::
        _L[68](output_directory)
        output_directory_from_original_wd = output_directory
      else
        output_directory_from_original_wd = output_directory
        goto cont7
      end
    else
      _raise(_Match, "main.sml:868:25")
    end
  end
  ::cont7::
  if options.print_output_directory then
    local tmp21 = output_directory_from_original_wd .. "\n"
    local tmp22 = _L[112].getOutstream(_L[115])
    if tmp22.tag == "LUA_WRITABLE" then
      local writable = tmp22.payload.writable
      _L[100](writable, tmp22.payload.name, tmp21)
      writable:flush()
      exit(0, true)
    else
      goto else4
    end
    ::else4::
    if tmp22.tag == "PRIM_WRITER" then
      local name1 = tmp22.payload.writer.name
      local writeVec = tmp22.payload.writer.writeVec
      local buffer = tmp22.payload.buffer
      local tmp23 = revAppend({tmp21, buffer[1]}, nil)
      local tmp24 = _VectorOrArray_fromList(tmp23)
      local content_PRIME = table_concat(tmp24)
      if writeVec.tag == "SOME" then
        local writeVec1 = writeVec.payload
        local tmp25 = tmp3(content_PRIME)
        writeVec1(tmp25)
        buffer[1] = nil
        exit(0, true)
      elseif writeVec.tag == "NONE" then
        _raise(setmetatable({tag = Io_tag, payload = {cause = BlockingNotSupported, ["function"] = "output", name = name1}}, _exn_meta), "text-io.sml:397:26")
      else
        _raise(_Match, "text-io.sml:395:14")
      end
    else
      _raise(_Match, "text-io.sml:392:9")
    end
  else
    goto cont8
  end
  ::cont8::
  local pathsep, original_wd, make_depends, tex_output_directory, output_directory, output
  do
    if _L[220] then
      pathsep = ";"
    else
      pathsep = ":"
    end
    original_wd = _L[67](nil)
    local tmp21 = getOpt(options.change_directory, false)
    if tmp21 then
      local make_depends1
      do
        local tmp22
        do
          local result = getenv("TEXINPUTS")
          if result == nil then
            tmp22 = {tag = "NONE"}
          else
            tmp22 = {tag = "SOME", payload = result}
          end
        end
        local TEXINPUTS = getOpt(tmp22, "")
        local tmp23
        do
          local result = getenv("LUAINPUTS")
          if result == nil then
            tmp23 = {tag = "NONE"}
          else
            tmp23 = {tag = "SOME", payload = result}
          end
        end
        local LUAINPUTS = getOpt(tmp23, "")
        _L[66](output_directory_from_original_wd)
        local tmp24 = original_wd .. pathsep .. TEXINPUTS
        _L[221]("TEXINPUTS", tmp24)
        local tmp25 = original_wd .. pathsep .. LUAINPUTS
        _L[221]("LUAINPUTS", tmp25)
        local tmp26 = options.make_depends
        if tmp26.tag == "SOME" then
          local x = tmp26.payload
          local tmp27 = _L[215]({tag = "SOME", payload = original_wd}, x)
          make_depends1 = {tag = "SOME", payload = tmp27}
        elseif tmp26.tag == "NONE" then
          make_depends1 = {tag = "NONE"}
          goto cont13
        else
          _raise(_Match, "option.sml:33:5")
        end
      end
      ::cont13::
      local tmp22 = _L[215]({tag = "SOME", payload = original_wd}, output_from_original_wd)
      make_depends = make_depends1
      tex_output_directory = {tag = "NONE"}
      output_directory = "."
      output = tmp22
    else
      local tmp22 = {tag = "SOME", payload = output_directory_from_original_wd}
      make_depends = options.make_depends
      tex_output_directory = tmp22
      output_directory = output_directory_from_original_wd
      output = output_from_original_wd
      goto cont9
    end
  end
  ::cont9::
  local output1
  do
    local exp1 = options.bibtex_or_biber
    if exp1.tag == "SOME" then
      local tmp21
      do
        local result = getenv("BIBINPUTS")
        if result == nil then
          tmp21 = {tag = "NONE"}
        else
          tmp21 = {tag = "SOME", payload = result}
        end
      end
      local BIBINPUTS = getOpt(tmp21, "")
      local tmp22 = original_wd .. pathsep .. BIBINPUTS
      _L[221]("BIBINPUTS", tmp22)
      output1 = _L[215]({tag = "SOME", payload = original_wd}, output_from_original_wd)
    elseif exp1.tag == "NONE" then
      output1 = output
      goto cont10
    else
      _raise(_Match, "main.sml:917:32")
    end
  end
  ::cont10::
  local recorderfile, recorderfile2, tex_output_format
  do
    local exp1
    do
      local result = getenv("max_print_line")
      if result == nil then
        exp1 = {tag = "NONE"}
      else
        exp1 = {tag = "SOME", payload = result}
      end
    end
    if exp1.tag == "NONE" then
      _L[221]("max_print_line", "16384")
    elseif exp1.tag == "SOME" then
    else
      _raise(_Match, "main.sml:934:28")
    end
    local tmp21 = jobname .. ".fls"
    recorderfile = _L[210].join(output_directory, tmp21)
    local tmp22 = jobname .. ".cluttex-fls"
    recorderfile2 = _L[210].join(output_directory, tmp22)
    if output_format == "DVI" then
      tex_output_format = _L[231]
      goto cont11
    elseif output_format == "PDF" then
      if supports_pdf_generation then
        tex_output_format = _L[230]
      else
        tex_output_format = _L[231]
      end
    else
      _raise(_Match, "main.sml:943:43")
    end
  end
  ::cont11::
  local lua_initialization_script, source_date_epoch_info
  do
    local tmp21 = _L[239](engine_type, _L[237])
    if tmp21 then
      local tmp22 = jobname .. ".cluttexinit.lua"
      local initscriptfile = _L[210].join(output_directory, tmp22)
      local tmp23 = options.file_line_error
      local tmp24 = {file_line_error = tmp23, halt_on_error = options.halt_on_error, jobname = jobname, output_directory = output_directory}
      _L[383].create_initialization_script(initscriptfile, tmp24)
      lua_initialization_script = {tag = "SOME", payload = initscriptfile}
    else
      lua_initialization_script = {tag = "NONE"}
    end
    local exp1 = options.source_date_epoch
    local tmp22 = exp1.tag == "SOME" and exp1.payload.tag == "RAW"
    local tmp23
    do
      if tmp22 then
        local raw = exp1.payload.payload
        _L[221]("SOURCE_DATE_EPOCH", raw)
        source_date_epoch_info = {tag = "NONE"}
        goto cont12
      end
      local tmp24 = tmp(_L[355])
      local tmp25 = options.source_date_epoch
      local tmp26 = tmp24(tmp25, {tag = "SOME", payload = _L[354]})
      if tmp26 then
        tmp23 = true
        goto cont13
      end
      local tmp27 = tmp(eq)
      local result = getenv("SOURCE_DATE_EPOCH")
      if result == nil then
        local tmp28 = {tag = "NONE"}
        tmp23 = tmp27(tmp28, {tag = "NONE"})
      else
        local tmp28 = {tag = "SOME", payload = result}
        tmp23 = tmp27(tmp28, {tag = "NONE"})
        goto cont13
      end
    end
    ::cont13::
    if tmp23 then
      local tmp24 = tostring(time())
      local tmp25
      do
        local tmp26 = difftime(time(), _L[52])
        local status, exn = pcall(function()
          return trunc(tmp26 * 1e6)
        end)
        if not status then
          _raise(_L[53], "time.sml:80:82")
        else
          tmp25 = exn
        end
      end
      source_date_epoch_info = {tag = "SOME", payload = {{time = tmp25, time_since_epoch = tmp24}}}
    else
      source_date_epoch_info = {tag = "NONE"}
      goto cont12
    end
  end
  ::cont12::
  local tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40
  do
    tmp21 = options.engine_executable
    local tmp41 = getOpt(options.interaction, _L[223])
    tmp22 = {tag = "SOME", payload = tmp41}
    tmp23 = options.file_line_error
    tmp24 = options.halt_on_error
    tmp25 = options.synctex
    tmp26 = options.shell_escape
    tmp27 = {tag = "SOME", payload = jobname}
    tmp28 = options.fmt
    tmp29 = revAppend(options.tex_extraoptions_rev, nil)
    tmp30 = getOpt(options.max_iterations, 4)
    tmp31 = options.start_with_draft
    getOpt(options.change_directory, false)
    tmp32 = options.includeonly
    tmp33 = options.package_support
    tmp34 = options.source_date_epoch
    tmp35 = options.synctex
    getOpt(options.interaction, _L[223])
    tmp36 = options.shell_escape
    revAppend(options.tex_extraoptions_rev, nil)
    tmp37 = revAppend(options.dvipdfmx_extraoptions_rev, nil)
    tmp38 = options.makeindex
    tmp39 = options.bibtex_or_biber
    tmp40 = options.makeglossaries
    if watch.tag == "NONE" then
      local status, exn = pcall(function()
        local epstopdf = tmp33.epstopdf
        local minted = tmp33.minted
        return _L[564](engine_type, executable, name, supports_draftmode, supports_pdf_generation, inputfile, tmp39, check_driver, tmp37, tmp32, jobname, make_depends, tmp40, tmp38, tmp30, output1, output_directory, output_format, epstopdf, minted, tmp33.pdfx, tmp36, tmp34, tmp31, tmp35, original_wd, output_extension, recorderfile, recorderfile2, source_date_epoch_info, false, tmp21, tmp29, tmp23, tmp28, tmp24, tmp22, tmp27, lua_initialization_script, tex_output_directory, tex_output_format, tmp26, tmp25)
      end)
      if not status then
        if __exn_instanceof(exn, _L[405]) then
          exit(1, true)
        else
          _raise(exn, nil)
        end
      else
        return exn
      end
    else
      goto else3
    end
  end
  ::else3::
  if watch.tag == "SOME" then
    local watch_engine = watch.payload
    local fswatcherlib
    if _L[220] then
      local tmp41, tmp42 = pcall(require, "texrunner.fswatcher_windows")
      if not tmp41 then
        local tmp43 = _ENV.CLUTTEX_VERBOSITY
        if tmp43 >= 1 then
        else
          fswatcherlib = {tag = "NONE"}
          goto cont13
        end
        _L[309]("Failed to load texrunner.fswatcher_windows: " .. tmp42)
        fswatcherlib = {tag = "NONE"}
      else
        fswatcherlib = {tag = "SOME", payload = tmp42}
        goto cont13
      end
    else
      fswatcherlib = {tag = "NONE"}
      goto cont13
    end
    ::cont13::
    local doWatch
    if fswatcherlib.tag == "SOME" then
      local fswatcherlib1 = fswatcherlib.payload
      local tmp41 = _ENV.CLUTTEX_VERBOSITY
      if tmp41 >= 2 then
      else
        doWatch = function(files)
          return _L[565](fswatcherlib1, files)
        end
        goto cont14
      end
      _L[311]("Using built-in filesystem watcher for Windows")
      doWatch = function(files)
        return _L[565](fswatcherlib1, files)
      end
      goto cont14
    else
      goto else4
    end
    ::else4::
    if fswatcherlib.tag == "NONE" then
      local tmp41
      do
        local tmp42 = _L[208]("fswatch")
        if tmp42 then
          local tmp43 = _L[353](watch_engine, _L[352])
          if tmp43 then
            tmp41 = true
          else
            tmp41 = _L[353](watch_engine, _L[350])
          end
        else
          tmp41 = false
          goto cont19
        end
      end
      ::cont19::
      if tmp41 then
        local tmp42 = _ENV.CLUTTEX_VERBOSITY
        if tmp42 >= 2 then
        else
          doWatch = _L[566]
          goto cont14
        end
        _L[311]("Using `fswatch' command")
        doWatch = _L[566]
        goto cont14
      else
        goto else5
      end
      ::else5::
      local tmp42
      do
        local tmp43 = _L[208]("inotifywait")
        if tmp43 then
          local tmp44 = _L[353](watch_engine, _L[352])
          if tmp44 then
            tmp42 = true
          else
            tmp42 = _L[353](watch_engine, _L[351])
          end
        else
          tmp42 = false
          goto cont20
        end
      end
      ::cont20::
      if tmp42 then
        local tmp43 = _ENV.CLUTTEX_VERBOSITY
        if tmp43 >= 2 then
        else
          doWatch = _L[567]
          goto cont14
        end
        _L[311]("Using `inotifywait' command")
        doWatch = _L[567]
        goto cont14
      else
        goto else6
      end
      ::else6::
      if watch_engine == "AUTO" then
      else
        goto else7
      end
      _L[308]("Could not watch files because neither `fswatch' nor `inotifywait' was installed.")
      _L[311]("See ClutTeX's manual for details.")
      exit(1, true)
      ::else7::
      if watch_engine == "FSWATCH" then
      else
        goto else8
      end
      _L[308]("Could not watch files because your selected engine `fswatch' was not installed.")
      _L[311]("See ClutTeX's manual for details.")
      exit(1, true)
      ::else8::
      if watch_engine == "INOTIFYWAIT" then
      else
        _raise(_Match, "main.sml:726:43")
      end
      _L[308]("Could not watch files because your selected engine `inotifywait' was not installed.")
      _L[311]("See ClutTeX's manual for details.")
      exit(1, true)
    else
      _raise(_Match, "main.sml:703:25")
    end
    ::cont14::
    do
      local status, exn = pcall(function()
        local epstopdf = tmp33.epstopdf
        local minted = tmp33.minted
        return _L[564](engine_type, executable, name, supports_draftmode, supports_pdf_generation, inputfile, tmp39, check_driver, tmp37, tmp32, jobname, make_depends, tmp40, tmp38, tmp30, output1, output_directory, output_format, epstopdf, minted, tmp33.pdfx, tmp36, tmp34, tmp31, tmp35, original_wd, output_extension, recorderfile, recorderfile2, source_date_epoch_info, false, tmp21, tmp29, tmp23, tmp28, tmp24, tmp22, tmp27, lua_initialization_script, tex_output_directory, tex_output_format, tmp26, tmp25)
      end)
      if not status then
        if __exn_instanceof(exn, _L[405]) then
          goto cont15
        else
          _raise(exn, nil)
        end
      end
    end
    ::cont15::
    local recorded, tmp41
    do
      recorded = _L[389](recorderfile, tmp38, nil, _L[202])
      local tmp42 = _L[239](engine_type, _L[237])
      if tmp42 then
      else
        tmp41 = false
        goto cont16
      end
      tmp41 = _L[218](recorderfile2)
    end
    ::cont16::
    local recorded1
    if tmp41 then
      local fileList = recorded[1]
      recorded1 = _L[389](recorderfile2, tmp38, fileList, recorded[2])
    else
      recorded1 = recorded
      goto cont17
    end
    ::cont17::
    local tmp42
    do
      local exp1
      do
        local fileInfo = recorded1[1]
        exp1 = _L[388](fileInfo, recorded1[2])
      end
      local filelist = exp1[1]
      local tmp43 = mapPartial1(function(a)
        if a.kind == "INPUT" then
          return {tag = "SOME", payload = a.abspath}
        else
          return {tag = "NONE"}
        end
      end)
      local inputFilesToWatch = tmp43(filelist)
      tmp42 = inputFilesToWatch
    end
    ::cont18::
    local inputFilesToWatch = tmp42
    local tmp43 = doWatch(inputFilesToWatch)
    if tmp43 then
      local success
      do
        local status, exn = pcall(function()
          do
            local epstopdf = tmp33.epstopdf
            local minted = tmp33.minted
            _L[564](engine_type, executable, name, supports_draftmode, supports_pdf_generation, inputfile, tmp39, check_driver, tmp37, tmp32, jobname, make_depends, tmp40, tmp38, tmp30, output1, output_directory, output_format, epstopdf, minted, tmp33.pdfx, tmp36, tmp34, tmp31, tmp35, original_wd, output_extension, recorderfile, recorderfile2, source_date_epoch_info, false, tmp21, tmp29, tmp23, tmp28, tmp24, tmp22, tmp27, lua_initialization_script, tex_output_directory, tex_output_format, tmp26, tmp25)
          end
          return true
        end)
        if not status then
          if __exn_instanceof(exn, _L[405]) then
            success = false
          else
            _raise(exn, nil)
          end
        else
          success = exn
        end
      end
      if success then
        local recorded2, tmp44
        do
          recorded2 = _L[389](recorderfile, tmp38, nil, _L[202])
          local tmp45 = _L[239](engine_type, _L[237])
          if tmp45 then
          else
            tmp44 = false
            goto cont19
          end
          tmp44 = _L[218](recorderfile2)
        end
        ::cont19::
        local recorded3
        if tmp44 then
          local fileList = recorded2[1]
          recorded3 = _L[389](recorderfile2, tmp38, fileList, recorded2[2])
        else
          recorded3 = recorded2
          goto cont20
        end
        ::cont20::
        local exp1
        do
          local fileInfo = recorded3[1]
          exp1 = _L[388](fileInfo, recorded3[2])
        end
        local filelist = exp1[1]
        local tmp45 = mapPartial1(function(a)
          if a.kind == "INPUT" then
            return {tag = "SOME", payload = a.abspath}
          else
            return {tag = "NONE"}
          end
        end)
        tmp42 = tmp45(filelist)
        goto cont18
      else
        tmp42 = inputFilesToWatch
        goto cont18
      end
    else
      return nil
    end
  else
    _raise(_Match, "main.sml:1024:18")
  end
end)
if not _L[568] then
  _L[570] = _isError(_L[569])
  if _L[570] then
    _L[571] = _id(_L[569])
    _L[572] = tostring(_L[571])
    _L[123]({_L[117], "unhandled Lua error: " .. _L[572] .. "\n"})
    exit(1, true)
  else
    goto else2
  end
  ::else2::
  if __exn_instanceof(_L[569], _Fail_tag) then
  else
    goto else3
  end
  _L[123]({_L[117], "unhandled error: " .. _L[569].payload .. "\n"})
  exit(1, true)
  ::else3::
  if __exn_instanceof(_L[569], _L[65]) then
  else
    goto else4
  end
  _L[123]({_L[117], "unhandled system error: " .. _L[569].payload[1] .. "\n"})
  exit(1, true)
  ::else4::
  if __exn_instanceof(_L[569], Io_tag) then
    _L[573] = _L[569].payload.name
    _L[123]({_L[117], "unhandled IO error: " .. _L[569].payload["function"] .. ", " .. _L[573] .. "\n"})
    exit(1, true)
  else
    _raise(_L[569], nil)
  end
end
