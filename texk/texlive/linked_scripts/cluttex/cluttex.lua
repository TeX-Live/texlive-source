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
  if isdir(path) then
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
  texio_write_nl("log", string.format("CLUTTEX_EXEC %s", ...), "")
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
local function _Fail(message)
  return setmetatable({ tag = _Fail_tag, payload = message }, _exn_meta)
end
local _Error_tag = { "Error" }
local function _Error(x)
  return setmetatable({ tag = _Error_tag, payload = x }, _exn_meta)
end
local function _handle(f)
  local success, result = pcall(f)
  if not success and getmetatable(result) ~= _exn_meta then
    result = _Error(result)
  end
  return success, result
end
local function __exn_instanceof(e, tag)
  return e.tag == tag
end
local function _raise(x, location)
  local e
  if x.tag == _Error_tag then
    e = x.payload
  elseif location ~= nil then
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
local function _VectorOrArray_tabulate(t)
  local n, f = t[1], t[2]
  if n < 0 then -- or maxLen < n
    _raise(_Size, "(Vector|Array).tabulate")
  end
  local t = { n = n }
  for i = 1, n do
    t[i] = f(i - 1)
  end
  return t
end
local ref, _COLON_COLON, Chr, Domain, LESS, EQUAL, GREATER, _EXCLAM, _COLON_EQ, NONE, Option, getOpt, app, mapPartial, tmp, revAppend, app1, map, mapPartial1, find, foldl, exists, all, tabulate, sub, update, foldr, length, array, size, str, eq, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, TypeError_tag, DEC, quot, rem, compare, tmp24, tmp25, fromManExp, sub1, substring, extract, concat, implode, implodeRev, fields, isPrefix, isSuffix, isAscii, isDigit, isAlphaNum, isHexDigit, isPrint, isSpace, toUpper, toString, scanString
do
  ref = function(x)
    return {x}
  end
  _COLON_COLON = function(a)
    local x = a[1]
    return {x, a[2]}
  end
  Chr = {tag = {"Chr"}}
  Domain = {tag = {"Domain"}}
  LESS = "LESS"
  EQUAL = "EQUAL"
  GREATER = "GREATER"
  _EXCLAM = function(a)
    return a[1]
  end
  _COLON_EQ = function(a)
    local x = a[1]
    x[1] = a[2]
    return nil
  end
  NONE = {tag = "NONE"}
  Option = {tag = {"Option"}}
  getOpt = function(tmp26, default)
    if tmp26.tag == "NONE" then
      return default
    elseif tmp26.tag == "SOME" then
      return tmp26.payload
    else
      _raise(_Match, "option.sml:19:5")
    end
  end
  app = function(a)
    return function(a1)
      if a1.tag == "SOME" then
      elseif a1.tag == "NONE" then
        return nil
      else
        _raise(_Match, "option.sml:31:5")
      end
      do
        return a(a1.payload)
      end
    end
  end
  mapPartial = function(a)
    return function(a1)
      if a1.tag == "SOME" then
      elseif a1.tag == "NONE" then
        return NONE
      else
        _raise(_Match, "option.sml:35:5")
      end
      do
        return a(a1.payload)
      end
    end
  end
  tmp = function(eq1)
    return function(p)
      if p[1].tag == "NONE" and p[2].tag == "NONE" then
        return true
      else
        if p[1].tag == "SOME" and p[2].tag == "SOME" then
        else
          return false
        end
        do
          return eq1({p[1].payload, p[2].payload})
        end
      end
    end
  end
  revAppend = function(tmp26, ys)
    local tmp27, tmp28 = tmp26, ys
    ::cont::
    do
      local tmp29, ys1 = tmp27, tmp28
      if tmp29 == nil then
        return ys1
      elseif tmp29 ~= nil then
        local tmp30 = tmp29[1]
        local tmp31 = tmp29[2]
        tmp27 = tmp31
        tmp28 = {tmp30, ys1}
        goto cont
      else
        _raise(_Match, "list.sml:60:5")
      end
    end
  end
  app1 = function(a)
    return function(a1)
      if a1 == nil then
        return nil
      end
      if a1 ~= nil then
      else
        _raise(_Match, "list.sml:64:5")
      end
      do
        local tmp26 = a1[1]
        local tmp27 = a1[2]
        a(tmp26)
        local tmp28 = app1(a)
        return tmp28(tmp27)
      end
    end
  end
  map = function(a)
    return function(a1)
      if a1 == nil then
        return nil
      end
      if a1 ~= nil then
      else
        _raise(_Match, "list.sml:66:5")
      end
      do
        local tmp26 = a1[1]
        local tmp27 = a1[2]
        local tmp28 = a(tmp26)
        local tmp29 = map(a)
        local tmp30 = tmp29(tmp27)
        return {tmp28, tmp30}
      end
    end
  end
  mapPartial1 = function(a)
    return function(a1)
      if a1 == nil then
        return nil
      end
      if a1 ~= nil then
      else
        _raise(_Match, "list.sml:68:5")
      end
      do
        local tmp26, exp
        do
          local tmp27 = a1[1]
          tmp26 = a1[2]
          exp = a(tmp27)
          if exp.tag == "NONE" then
          else
            goto else1
          end
          do
            local tmp28 = mapPartial1(a)
            return tmp28(tmp26)
          end
        end
        ::else1::
        if exp.tag == "SOME" then
        else
          _raise(_Match, "list.sml:69:30")
        end
        do
          local y = exp.payload
          local tmp27 = mapPartial1(a)
          local tmp28 = tmp27(tmp26)
          return {y, tmp28}
        end
      end
    end
  end
  find = function(a)
    return function(a1)
      if a1 == nil then
        return NONE
      end
      if a1 ~= nil then
      else
        _raise(_Match, "list.sml:72:5")
      end
      do
        local tmp26 = a1[1]
        local tmp27 = a1[2]
        local tmp28 = a(tmp26)
        if tmp28 then
          return {tag = "SOME", payload = tmp26}
        end
        local tmp29 = find(a)
        return tmp29(tmp27)
      end
    end
  end
  foldl = function(a)
    return function(a1)
      return function(a2)
        if a2 == nil then
          return a1
        end
        if a2 ~= nil then
        else
          _raise(_Match, "list.sml:91:5")
        end
        do
          local tmp26 = a2[1]
          local tmp27 = a2[2]
          local tmp28 = foldl(a)
          local tmp29 = a({tmp26, a1})
          local tmp30 = tmp28(tmp29)
          return tmp30(tmp27)
        end
      end
    end
  end
  exists = function(a)
    return function(a1)
      if a1 == nil then
        return false
      end
      if a1 ~= nil then
      else
        _raise(_Match, "list.sml:96:5")
      end
      do
        local tmp26 = a1[1]
        local tmp27 = a1[2]
        local tmp28 = a(tmp26)
        if tmp28 then
          return true
        end
        local tmp29 = exists(a)
        return tmp29(tmp27)
      end
    end
  end
  all = function(a)
    return function(a1)
      if a1 == nil then
        return true
      end
      if a1 ~= nil then
      else
        _raise(_Match, "list.sml:98:5")
      end
      do
        local tmp26 = a1[1]
        local tmp27 = a1[2]
        local tmp28 = a(tmp26)
        if tmp28 then
        else
          return false
        end
        do
          local tmp29 = all(a)
          return tmp29(tmp27)
        end
      end
    end
  end
  tabulate = function(n, f)
    if n < 0 then
      _raise(_Size, "list.sml:101:27")
    end
    if n < 10 then
    else
      goto else1
    end
    do
      local function go(a)
        if a >= n then
          return nil
        end
        local tmp26 = f(a)
        local tmp27 = go(_Int_add(a, 1))
        return {tmp26, tmp27}
      end
      return go(0)
    end
    ::else1::
    local tmp26, tmp27 = 0, nil
    ::cont::
    do
      local i, acc = tmp26, tmp27
      if i >= n then
      else
        goto else2
      end
      do
        return revAppend(acc, nil)
      end
      ::else2::
      local tmp28 = _Int_add(i, 1)
      local tmp29 = f(i)
      tmp26 = tmp28
      tmp27 = {tmp29, acc}
      goto cont
    end
  end
  sub = function(a)
    local arr = a[1]
    return arr[a[2] + 1]
  end
  update = function(a)
    local arr = a[1]
    local i = a[2]
    arr[i + 1] = a[3]
    return nil
  end
  foldr = function(a, a1)
    return function(a2)
      local tmp26, tmp27, tmp28, tmp29 = a, a1, a2, _Int_sub(a2.n, 1)
      ::cont::
      do
        local f, acc, vec, i = tmp26, tmp27, tmp28, tmp29
        if i < 0 then
          return acc
        end
        local tmp30 = f({vec[i + 1], acc})
        tmp26 = f
        tmp27 = tmp30
        tmp28 = vec
        tmp29 = _Int_sub(i, 1)
        goto cont
      end
    end
  end
  length = function(a)
    return a.n
  end
  array = function(a)
    local n = a[1]
    return _Array_array(n, a[2])
  end
  size = function(a)
    return #a
  end
  str = function(a)
    return string_char(a)
  end
  eq = function(a)
    local x = a[1]
    return x == a[2]
  end
  local tmp26 = _ENV.io
  tmp1 = _ENV.os
  tmp2 = _ENV.package
  tmp3 = _ENV.require
  tmp4 = _ENV.tonumber
  tmp5 = _ENV.tostring
  tmp6 = _ENV.type
  tmp7 = tmp26.open
  tmp8 = tmp26.popen
  tmp9 = tmp26.stderr
  tmp10 = tmp26.stdout
  tmp11 = math.huge
  tmp12 = math.modf
  tmp13 = tmp1.difftime
  tmp14 = tmp1.execute
  tmp15 = tmp1.exit
  tmp16 = tmp1.getenv
  tmp17 = tmp1.setlocale
  tmp18 = tmp1.time
  tmp19 = string.byte
  tmp20 = string.find
  tmp21 = string.gsub
  tmp22 = string.match
  local tmp27 = string.sub
  do
    local tmp28, tmp29 = pcall(tmp3, "lfs")
    if tmp28 then
      tmp23 = {tag = "SOME", payload = tmp29}
    else
      tmp23 = NONE
    end
  end
  TypeError_tag = {"TypeError"}
  DEC = "DEC"
  quot = function(x, y)
    do
      local tmp28 = x >= 0 and y >= 0
      local tmp29
      tmp29 = tmp28 or x <= 0 and y <= 0
      if tmp29 then
        return _Int_div(x, y)
      end
      if x == math_mininteger then
      else
        goto else1
      end
      do
        if y == 1 then
          return x
        end
        local tmp30 = _Int_negate(y)
        return _Int_negate(_Int_div(x, tmp30))
      end
    end
    ::else1::
    local tmp28 = _Int_negate(x)
    return _Int_negate(_Int_div(tmp28, y))
  end
  rem = function(x, y)
    if y == -1 then
      return 0
    end
    local tmp28 = quot(x, y)
    return _Int_sub(x, _Int_mul(tmp28, y))
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
    local tmp28, tmp29 = math_maxinteger, 1
    ::cont5::
    do
      local x, n = tmp28, tmp29
      if x == 0 then
        tmp24 = n
      else
        local tmp30 = x >> 1
        tmp28 = tmp30
        tmp29 = _Int_add(n, 1)
        goto cont5
      end
    end
  end
  tmp25 = - tmp11
  fromManExp = function(exp, man)
    local tmp28, tmp29 = exp, man
    ::cont::
    do
      local exp1, man1 = tmp28, tmp29
      if -1022 <= exp1 then
        if exp1 < 1024 then
          return man1 * 2.0 ^ exp1
        end
        local exp_PRIME
        if exp1 > 2098 then
          exp_PRIME = 2098
        else
          exp_PRIME = exp1
        end
        local tmp30 = man1 * 0x1p1023
        tmp28 = _Int_sub(exp_PRIME, 1023)
        tmp29 = tmp30
        goto cont
      end
      local exp_PRIME
      if exp1 < -2099 then
        exp_PRIME = -2099
      else
        exp_PRIME = exp1
      end
      local tmp30 = exp_PRIME % -1022
      if tmp30 ~= 0 then
        local tmp31 = man1 * 2.0 ^ tmp30
        tmp28 = _Int_sub(exp_PRIME, tmp30)
        tmp29 = tmp31
        goto cont
      else
        local tmp31 = man1 * 0x1p-1022
        tmp28 = _Int_add(exp_PRIME, 1022)
        tmp29 = tmp31
        goto cont
      end
    end
  end
  sub1 = function(a)
    local s = a[1]
    local i = a[2]
    if i < 0 or #s <= i then
      _raise(_Subscript, "string-1.sml:33:44")
    else
      return tmp19(s, _Int_add(i, 1))
    end
  end
  substring = function(s, i, j)
    local tmp28
    if i < 0 then
      tmp28 = true
    elseif j < 0 then
      tmp28 = true
    else
      local tmp29 = #s
      tmp28 = tmp29 < _Int_add(i, j)
    end
    if tmp28 then
      _raise(_Subscript, "string-1.sml:40:59")
    else
      local tmp29 = _Int_add(i, 1)
      return tmp27(s, tmp29, _Int_add(i, j))
    end
  end
  extract = function(s, i, tmp28)
    if tmp28.tag == "NONE" then
      if i < 0 or #s < i then
        _raise(_Subscript, "string-1.sml:46:69")
      else
        return tmp27(s, _Int_add(i, 1))
      end
    else
      if tmp28.tag == "SOME" then
      else
        _raise(_Match, "string-1.sml:45:5")
      end
      do
        return substring(s, i, tmp28.payload)
      end
    end
  end
  concat = function(a)
    return table_concat(_VectorOrArray_fromList(a))
  end
  implode = function(a)
    local v = _VectorOrArray_fromList(a)
    local tmp28
    do
      local tmp29 = v.n
      tmp28 = _VectorOrArray_tabulate({tmp29, function(i)
        return string_char(v[i + 1])
      end})
    end
    return table_concat(tmp28)
  end
  implodeRev = function(a)
    local tmp28 = revAppend(a, nil)
    local v = _VectorOrArray_fromList(tmp28)
    local tmp29
    do
      local tmp30 = v.n
      tmp29 = _VectorOrArray_tabulate({tmp30, function(i)
        return string_char(v[i + 1])
      end})
    end
    return table_concat(tmp29)
  end
  fields = function(a)
    return function(a1)
      local tmp28, tmp29, tmp30
      do
        local tmp31
        do
          local tmp32 = foldr(_COLON_COLON, nil)
          local tmp33 = _VectorOrArray_tabulate({#a1, function(i)
            return sub1({a1, i})
          end})
          tmp31 = tmp32(tmp33)
        end
        tmp30, tmp29, tmp28 = nil, nil, tmp31
      end
      ::cont::
      do
        local revFields, acc, tmp31 = tmp30, tmp29, tmp28
        if tmp31 == nil then
        else
          goto else1
        end
        do
          local tmp32 = implodeRev(acc)
          return revAppend({tmp32, revFields}, nil)
        end
        ::else1::
        if tmp31 ~= nil then
        else
          _raise(_Match, "string-1.sml:80:26")
        end
        do
          local tmp32 = tmp31[1]
          local tmp33 = tmp31[2]
          local tmp34 = a(tmp32)
          if tmp34 then
          else
            tmp30 = revFields
            tmp29 = {tmp32, acc}
            tmp28 = tmp33
            goto cont
          end
          do
            local tmp35 = implodeRev(acc)
            tmp30 = {tmp35, revFields}
            tmp29 = nil
            tmp28 = tmp33
            goto cont
          end
        end
      end
    end
  end
  isPrefix = function(a)
    return function(a1)
      local tmp28 = #a
      if tmp28 > #a1 then
        return false
      end
      local tmp29 = substring(a1, 0, tmp28)
      return tmp29 == a
    end
  end
  isSuffix = function(a)
    return function(a1)
      local tmp28 = #a
      local tmp29 = #a1
      if tmp28 > tmp29 then
        return false
      end
      local tmp30 = substring(a1, _Int_sub(tmp29, tmp28), tmp28)
      return tmp30 == a
    end
  end
  isAscii = function(a)
    return a <= 127
  end
  isDigit = function(a)
    return 48 <= a and a <= 57
  end
  isAlphaNum = function(a)
    local tmp28
    if 65 <= a and a <= 90 then
      return true
    else
      tmp28 = 97 <= a and a <= 122
    end
    return tmp28 or 48 <= a and a <= 57
  end
  isHexDigit = function(a)
    return 48 <= a and a <= 57 or (97 <= a and a <= 102 or 65 <= a and a <= 70)
  end
  isPrint = function(a)
    return 33 <= a and a <= 126 or a == 32
  end
  isSpace = function(a)
    return 9 <= a and a <= 13 or a == 32
  end
  toUpper = function(a)
    if 97 <= a and a <= 122 then
      local tmp28 = _Int_sub(a, 32)
      if tmp28 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp28 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return tmp28
      end
    else
      return a
    end
  end
  toString = function(a)
    if a == 92 then
      return "\\\\"
    end
    if a == 34 then
      return "\\\""
    end
    local tmp28 = isPrint(a)
    if tmp28 then
      return string_char(a)
    end
    if a == 7 then
      return "\\a"
    end
    if a == 8 then
      return "\\b"
    end
    if a == 9 then
      return "\\t"
    end
    if a == 10 then
      return "\\n"
    end
    if a == 11 then
      return "\\v"
    end
    if a == 12 then
      return "\\f"
    end
    if a == 13 then
      return "\\r"
    end
    local tmp29 = a
    if tmp29 < 32 then
      local tmp30 = _Int_add(tmp29, 64)
      if tmp30 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp30 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return "\\^" .. string_char(tmp30)
      end
    elseif tmp29 < 100 then
      return "\\0" .. tmp21(tmp5(tmp29), "-", "~")
    else
      return "\\" .. tmp21(tmp5(tmp29), "-", "~")
    end
  end
  scanString = function(a)
    return function(a1)
      local tmp28 = a(function(a2)
        local s = a2[1]
        local i = a2[2]
        if i < #s then
        else
          return NONE
        end
        do
          local tmp29 = sub1({s, i})
          return {tag = "SOME", payload = {tmp29, {s, _Int_add(i, 1)}}}
        end
      end)
      local exp = tmp28({a1, 0})
      if exp.tag == "SOME" then
        return {tag = "SOME", payload = exp.payload[1]}
      elseif exp.tag == "NONE" then
        return NONE
      else
        _raise(_Match, "string-cvt-1.sml:33:25")
      end
    end
  end
  do
    if tmp24 ~= 64 then
    else
      goto cont
    end
    do
      local tmp28 = _Fail("Word64 is not available")
      _raise(tmp28, "word.sml:333:18")
    end
  end
end
::cont::
local MonoSequence, CharArray, extract1, full, string1, isEmpty, getc, concat1, splitl, splitr, tokens, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42
do
  MonoSequence = function(fromList, length1, maxLen, tmp43, create, tmp44, tmp45, tmp46, tmp47, fromList1, length2, maxLen1, tmp48, vector, tmp49, tmp50, tmp51)
    local tabulate1, sub2, update1, appi, app2, mapi, map1, foldli, foldri, foldl1, foldr1, findi, find1, exists1, all1, collate, toList, append, prepend, array1, tabulate2, sub3, update2, copy, copyVec, appi1, app3, modifyi, modify, foldli1, foldri1, foldl2, foldr2, findi1, find2, exists2, all2, collate1, toList1, vector1, fromVector, length3, sub4, update3, full1, slice, subslice, base, copy1, copyVec1, isEmpty1, getItem, appi2, app4, modifyi1, modify1, foldli2, foldri2, foldl3, foldr3, findi2, find3, exists3, all3, collate2, vector2, UnsafeMonoVector, UnsafeMonoArray, MonoVectorSlice
    do
      tabulate1 = function(a)
        local n = a[1]
        local tmp52 = tabulate(n, a[2])
        return tmp49({n, tmp52})
      end
      sub2 = function(a)
        local v = a[1]
        local i = a[2]
        local tmp52
        do
          if 0 <= i then
          else
            tmp52 = false
            goto cont
          end
          do
            local tmp53 = length2(v)
            tmp52 = i < tmp53
          end
        end
        ::cont::
        if tmp52 then
        else
          _raise(_Subscript, "mono-sequence.sml:221:22")
        end
        do
          return tmp51({v, i})
        end
      end
      update1 = function(a)
        local v = a[1]
        local i = a[2]
        local x = a[3]
        local tmp52 = tmp48({base = v, length = i, start = 0})
        local tmp53 = fromList1({x, nil})
        local tmp54 = _Int_add(i, 1)
        local tmp55 = length2(v)
        local tmp56 = tmp48({base = v, length = _Int_sub(_Int_sub(tmp55, i), 1), start = tmp54})
        return tmp47({tmp52, {tmp53, {tmp56, nil}}})
      end
      appi = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return nil
            end
            local tmp53 = tmp51({a1, a2})
            a({a2, tmp53})
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      app2 = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return nil
            end
            local tmp53 = tmp51({a1, a2})
            a(tmp53)
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      mapi = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = tabulate(n, function(i)
            local tmp53 = tmp51({a1, i})
            return a({i, tmp53})
          end)
          return tmp49({n, tmp52})
        end
      end
      map1 = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = tabulate(n, function(i)
            local tmp53 = tmp51({a1, i})
            return a(tmp53)
          end)
          return tmp49({n, tmp52})
        end
      end
      foldli = function(a)
        return function(a1)
          return function(a2)
            local n = length2(a2)
            local tmp52, tmp53 = 0, a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i >= n then
                return acc
              end
              local tmp54 = _Int_add(i, 1)
              local tmp55 = tmp51({a2, i})
              local tmp56 = a({i, tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      foldri = function(a)
        return function(a1)
          return function(a2)
            local tmp52, tmp53
            do
              local tmp54 = length2(a2)
              tmp53, tmp52 = _Int_sub(tmp54, 1), a1
            end
            ::cont::
            do
              local i, acc = tmp53, tmp52
              if i < 0 then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp51({a2, i})
              local tmp56 = a({i, tmp55, acc})
              tmp53 = tmp54
              tmp52 = tmp56
              goto cont
            end
          end
        end
      end
      foldl1 = function(a)
        return function(a1)
          return function(a2)
            local n = length2(a2)
            local tmp52, tmp53 = 0, a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i >= n then
                return acc
              end
              local tmp54 = _Int_add(i, 1)
              local tmp55 = tmp51({a2, i})
              local tmp56 = a({tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      foldr1 = function(a)
        return function(a1)
          return function(a2)
            local tmp52, tmp53
            do
              local tmp54 = length2(a2)
              tmp53, tmp52 = _Int_sub(tmp54, 1), a1
            end
            ::cont::
            do
              local i, acc = tmp53, tmp52
              if i < 0 then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp51({a2, i})
              local tmp56 = a({tmp55, acc})
              tmp53 = tmp54
              tmp52 = tmp56
              goto cont
            end
          end
        end
      end
      findi = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return NONE
            end
            local x = tmp51({a1, a2})
            local tmp53 = a({a2, x})
            if tmp53 then
              return {tag = "SOME", payload = {a2, x}}
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      find1 = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return NONE
            end
            local x = tmp51({a1, a2})
            local tmp53 = a(x)
            if tmp53 then
              return {tag = "SOME", payload = x}
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      exists1 = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return false
            end
            local tmp53 = tmp51({a1, a2})
            local tmp54 = a(tmp53)
            if tmp54 then
              return true
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      all1 = function(a)
        return function(a1)
          local n = length2(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return true
            end
            local tmp53 = tmp51({a1, a2})
            local tmp54 = a(tmp53)
            if tmp54 then
              tmp52 = _Int_add(a2, 1)
              goto cont
            else
              return false
            end
          end
        end
      end
      collate = function(a)
        return function(a1)
          local xs = a1[1]
          local ys = a1[2]
          local xl = length2(xs)
          local yl = length2(ys)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            local tmp53 = xl <= a2
            local tmp54 = yl <= a2
            local tmp55
            tmp55 = tmp53 and tmp54
            if tmp55 then
              return EQUAL
            end
            local tmp56
            tmp56 = tmp53 and not tmp54
            if tmp56 then
              return LESS
            end
            if not tmp53 and tmp54 then
              return GREATER
            end
            if not tmp53 and not tmp54 then
            else
              _raise(_Match, "mono-sequence.sml:310:49")
            end
            do
              local tmp57 = tmp51({xs, a2})
              local tmp58 = tmp51({ys, a2})
              local exp = a({tmp57, tmp58})
              if exp == "EQUAL" then
                tmp52 = _Int_add(a2, 1)
                goto cont
              else
                return exp
              end
            end
          end
        end
      end
      toList = function(a)
        local tmp52 = foldr1(_COLON_COLON)
        local tmp53 = tmp52(nil)
        return tmp53(a)
      end
      append = function(a)
        local v = a[1]
        local tmp52 = fromList1({a[2], nil})
        return tmp47({v, {tmp52, nil}})
      end
      prepend = function(a)
        local x = a[1]
        local v = a[2]
        local tmp52 = fromList1({x, nil})
        return tmp47({tmp52, {v, nil}})
      end
      local length4 = function(tmp52)
        return tmp52.length
      end
      local sub5 = function(a)
        local base1 = a[1].base
        local start = a[1].start
        local length5 = a[1].length
        local i = a[2]
        if 0 <= i and i < length5 then
        else
          _raise(_Subscript, "mono-sequence.sml:331:44")
        end
        do
          return tmp51({base1, _Int_add(start, i)})
        end
      end
      local full2 = function(a)
        local tmp52 = length2(a)
        return {base = a, length = tmp52, start = 0}
      end
      local slice1 = function(a)
        if a[3].tag == "NONE" then
        else
          goto else1
        end
        do
          local v = a[1]
          local i = a[2]
          local n = length2(v)
          if 0 <= i and i <= n then
            return {base = v, length = _Int_sub(n, i), start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:337:33")
          end
        end
        ::else1::
        if a[3].tag == "SOME" then
        else
          _raise(_Match, "mono-sequence.sml:333:5")
        end
        do
          local v = a[1]
          local i = a[2]
          local n = a[3].payload
          local tmp52
          do
            if 0 <= i then
            else
              tmp52 = false
              goto cont
            end
            do
              if 0 <= n then
              else
                tmp52 = false
                goto cont
              end
              do
                local tmp53 = _Int_add(i, n)
                local tmp54 = length2(v)
                tmp52 = tmp53 <= tmp54
              end
            end
          end
          ::cont::
          if tmp52 then
            return {base = v, length = n, start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:342:32")
          end
        end
      end
      local subslice1 = function(a)
        if a[3].tag == "NONE" then
          local base1 = a[1].base
          local start = a[1].start
          local length5 = a[1].length
          local i = a[2]
          if 0 <= i and i <= length5 then
            local tmp52 = _Int_add(start, i)
            return {base = base1, length = _Int_sub(length5, i), start = tmp52}
          else
            _raise(_Subscript, "mono-sequence.sml:346:55")
          end
        end
        if a[3].tag == "SOME" then
          local base1 = a[1].base
          local start = a[1].start
          local length5 = a[1].length
          local i = a[2]
          local n = a[3].payload
          if 0 <= i and (0 <= n and _Int_add(i, n) <= length5) then
            return {base = base1, length = n, start = _Int_add(start, i)}
          else
            _raise(_Subscript, "mono-sequence.sml:350:57")
          end
        else
          _raise(_Match, "mono-sequence.sml:343:5")
        end
      end
      local base1 = function(a)
        local b = a.base
        local start = a.start
        return {b, start, a.length}
      end
      local concat2 = function(a)
        local tmp52 = map(tmp48)
        local tmp53 = tmp52(a)
        return tmp47(tmp53)
      end
      local isEmpty2 = function(a)
        return a.length == 0
      end
      local getItem1 = function(a)
        local base2 = a.base
        local start = a.start
        local length5 = a.length
        if length5 > 0 then
        else
          return NONE
        end
        do
          local tmp52 = tmp51({base2, start})
          local tmp53 = _Int_add(start, 1)
          return {tag = "SOME", payload = {tmp52, {base = base2, length = _Int_sub(length5, 1), start = tmp53}}}
        end
      end
      local appi3 = function(a)
        return function(a1)
          local base2 = a1.base
          local start = a1.start
          local length5 = a1.length
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= length5 then
              return nil
            end
            local tmp53 = tmp51({base2, _Int_add(start, a2)})
            a({a2, tmp53})
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      local app5 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return nil
            end
            local tmp54 = tmp51({base2, a2})
            a(tmp54)
            tmp53 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      local mapi1 = function(a)
        return function(a1)
          local base2 = a1.base
          local start = a1.start
          local length5 = a1.length
          local tmp52, tmp53 = 0, nil
          ::cont::
          do
            local i, acc = tmp52, tmp53
            if i >= length5 then
            else
              goto else1
            end
            do
              return tmp50({length5, acc})
            end
            ::else1::
            local tmp54 = _Int_add(i, 1)
            local tmp55 = tmp51({base2, _Int_add(start, i)})
            local tmp56 = a({i, tmp55})
            tmp52 = tmp54
            tmp53 = {tmp56, acc}
            goto cont
          end
        end
      end
      local map2 = function(a)
        return function(a1)
          local base2, length5, tmp52, tmp53, tmp54
          do
            base2 = a1.base
            local start = a1.start
            length5 = a1.length
            tmp52 = _Int_add(start, length5)
            tmp54, tmp53 = start, nil
          end
          ::cont::
          do
            local i, acc = tmp54, tmp53
            if i >= tmp52 then
            else
              goto else1
            end
            do
              return tmp50({length5, acc})
            end
            ::else1::
            local tmp55 = _Int_add(i, 1)
            local tmp56 = tmp51({base2, i})
            local tmp57 = a(tmp56)
            tmp54 = tmp55
            tmp53 = {tmp57, acc}
            goto cont
          end
        end
      end
      local foldli3 = function(a)
        return function(a1)
          return function(a2)
            local base2 = a2.base
            local start = a2.start
            local length5 = a2.length
            local tmp52, tmp53 = 0, a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i >= length5 then
                return acc
              end
              local tmp54 = _Int_add(i, 1)
              local tmp55 = tmp51({base2, _Int_add(start, i)})
              local tmp56 = a({i, tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      local foldri3 = function(a)
        return function(a1)
          return function(a2)
            local base2 = a2.base
            local start = a2.start
            local tmp52, tmp53 = _Int_sub(a2.length, 1), a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i < 0 then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp51({base2, _Int_add(start, i)})
              local tmp56 = a({i, tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      local foldl4 = function(a)
        return function(a1)
          return function(a2)
            local base2, tmp52, tmp53, tmp54
            do
              base2 = a2.base
              local start = a2.start
              tmp52 = _Int_add(start, a2.length)
              tmp54, tmp53 = start, a1
            end
            ::cont::
            do
              local i, acc = tmp54, tmp53
              if i >= tmp52 then
                return acc
              end
              local tmp55 = _Int_add(i, 1)
              local tmp56 = tmp51({base2, i})
              local tmp57 = a({tmp56, acc})
              tmp54 = tmp55
              tmp53 = tmp57
              goto cont
            end
          end
        end
      end
      local foldr4 = function(a)
        return function(a1)
          return function(a2)
            local base2 = a2.base
            local start = a2.start
            local tmp52, tmp53 = _Int_sub(_Int_add(start, a2.length), 1), a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i < start then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp51({base2, i})
              local tmp56 = a({tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      local findi3 = function(a)
        return function(a1)
          local base2 = a1.base
          local start = a1.start
          local length5 = a1.length
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= length5 then
              return NONE
            end
            local x = tmp51({base2, _Int_add(start, a2)})
            local tmp53 = a({a2, x})
            if tmp53 then
              return {tag = "SOME", payload = {a2, x}}
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      local find4 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return NONE
            end
            local x = tmp51({base2, a2})
            local tmp54 = a(x)
            if tmp54 then
              return {tag = "SOME", payload = x}
            else
              tmp53 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      local exists4 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return false
            end
            local tmp54 = tmp51({base2, a2})
            local tmp55 = a(tmp54)
            if tmp55 then
              return true
            else
              tmp53 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      local all4 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return true
            end
            local tmp54 = tmp51({base2, a2})
            local tmp55 = a(tmp54)
            if tmp55 then
              tmp53 = _Int_add(a2, 1)
              goto cont
            else
              return false
            end
          end
        end
      end
      local collate3 = function(a)
        return function(a1)
          local base2, base_PRIME, tmp52, tmp53, tmp54, tmp55
          do
            base2 = a1[1].base
            local start = a1[1].start
            local length5 = a1[1].length
            base_PRIME = a1[2].base
            local start_PRIME = a1[2].start
            local length_PRIME = a1[2].length
            tmp52 = _Int_add(start, length5)
            tmp53 = _Int_add(start_PRIME, length_PRIME)
            tmp55, tmp54 = start, start_PRIME
          end
          ::cont::
          do
            local i, j = tmp55, tmp54
            local tmp56 = tmp52 <= i
            local tmp57 = tmp53 <= j
            local tmp58
            tmp58 = tmp56 and tmp57
            if tmp58 then
              return EQUAL
            end
            local tmp59
            tmp59 = tmp56 and not tmp57
            if tmp59 then
              return LESS
            end
            if not tmp56 and tmp57 then
              return GREATER
            end
            if not tmp56 and not tmp57 then
            else
              _raise(_Match, "mono-sequence.sml:454:29")
            end
            do
              local tmp60 = tmp51({base2, i})
              local tmp61 = tmp51({base_PRIME, j})
              local exp = a({tmp60, tmp61})
              if exp == "EQUAL" then
                tmp55 = _Int_add(i, 1)
                tmp54 = _Int_add(j, 1)
                goto cont
              else
                return exp
              end
            end
          end
        end
      end
      array1 = function(a)
        local n = a[1]
        local init = a[2]
        if n < 0 or maxLen < n then
          _raise(_Size, "mono-sequence.sml:470:27")
        else
          return tmp43({n, init})
        end
      end
      tabulate2 = function(a)
        local n = a[1]
        local f = a[2]
        if maxLen < n then
          _raise(_Size, "mono-sequence.sml:475:27")
        end
        local tmp52 = tabulate(n, f)
        return tmp44({n, tmp52})
      end
      sub3 = function(a)
        local a1 = a[1]
        local i = a[2]
        local tmp52
        do
          if 0 <= i then
          else
            tmp52 = false
            goto cont
          end
          do
            local tmp53 = length1(a1)
            tmp52 = i < tmp53
          end
        end
        ::cont::
        if tmp52 then
        else
          _raise(_Subscript, "mono-sequence.sml:482:22")
        end
        do
          return tmp45({a1, i})
        end
      end
      update2 = function(a)
        local a1 = a[1]
        local i = a[2]
        local x = a[3]
        local tmp52
        do
          if 0 <= i then
          else
            tmp52 = false
            goto cont
          end
          do
            local tmp53 = length1(a1)
            tmp52 = i < tmp53
          end
        end
        ::cont::
        if tmp52 then
        else
          _raise(_Subscript, "mono-sequence.sml:486:28")
        end
        do
          return tmp46({a1, i, x})
        end
      end
      copy = function(a)
        local src = a.src
        local dst = a.dst
        local di = a.di
        local srcLen = length1(src)
        local tmp52
        do
          if 0 <= di then
          else
            tmp52 = false
            goto cont
          end
          do
            local tmp53 = _Int_add(di, srcLen)
            local tmp54 = length1(dst)
            tmp52 = tmp53 <= tmp54
          end
        end
        ::cont::
        if tmp52 then
        else
          _raise(_Subscript, "mono-sequence.sml:499:36")
        end
        do
          local tmp53 = 0
          ::cont1::
          do
            local a1 = tmp53
            if a1 >= srcLen then
              return nil
            end
            local tmp54 = _Int_add(di, a1)
            local tmp55 = tmp45({src, a1})
            tmp46({dst, tmp54, tmp55})
            tmp53 = _Int_add(a1, 1)
            goto cont1
          end
        end
      end
      copyVec = function(a)
        local src = a.src
        local dst = a.dst
        local di = a.di
        local srcLen = length2(src)
        local tmp52
        do
          if 0 <= di then
          else
            tmp52 = false
            goto cont
          end
          do
            local tmp53 = _Int_add(di, srcLen)
            local tmp54 = length1(dst)
            tmp52 = tmp53 <= tmp54
          end
        end
        ::cont::
        if tmp52 then
        else
          _raise(_Subscript, "mono-sequence.sml:513:39")
        end
        do
          local tmp53 = 0
          ::cont1::
          do
            local a1 = tmp53
            if a1 >= srcLen then
              return nil
            end
            local tmp54 = _Int_add(di, a1)
            local tmp55 = tmp51({src, a1})
            tmp46({dst, tmp54, tmp55})
            tmp53 = _Int_add(a1, 1)
            goto cont1
          end
        end
      end
      appi1 = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return nil
            end
            local tmp53 = tmp45({a1, a2})
            a({a2, tmp53})
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      app3 = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return nil
            end
            local tmp53 = tmp45({a1, a2})
            a(tmp53)
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      modifyi = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return nil
            end
            local x = tmp45({a1, a2})
            local y = a({a2, x})
            tmp46({a1, a2, y})
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      modify = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return nil
            end
            local x = tmp45({a1, a2})
            local y = a(x)
            tmp46({a1, a2, y})
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      foldli1 = function(a)
        return function(a1)
          return function(a2)
            local n = length1(a2)
            local tmp52, tmp53 = 0, a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i >= n then
                return acc
              end
              local tmp54 = _Int_add(i, 1)
              local tmp55 = tmp45({a2, i})
              local tmp56 = a({i, tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      foldri1 = function(a)
        return function(a1)
          return function(a2)
            local tmp52, tmp53
            do
              local tmp54 = length1(a2)
              tmp53, tmp52 = _Int_sub(tmp54, 1), a1
            end
            ::cont::
            do
              local i, acc = tmp53, tmp52
              if i < 0 then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp45({a2, i})
              local tmp56 = a({i, tmp55, acc})
              tmp53 = tmp54
              tmp52 = tmp56
              goto cont
            end
          end
        end
      end
      foldl2 = function(a)
        return function(a1)
          return function(a2)
            local n = length1(a2)
            local tmp52, tmp53 = 0, a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i >= n then
                return acc
              end
              local tmp54 = _Int_add(i, 1)
              local tmp55 = tmp45({a2, i})
              local tmp56 = a({tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      foldr2 = function(a)
        return function(a1)
          return function(a2)
            local tmp52, tmp53
            do
              local tmp54 = length1(a2)
              tmp53, tmp52 = _Int_sub(tmp54, 1), a1
            end
            ::cont::
            do
              local i, acc = tmp53, tmp52
              if i < 0 then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp45({a2, i})
              local tmp56 = a({tmp55, acc})
              tmp53 = tmp54
              tmp52 = tmp56
              goto cont
            end
          end
        end
      end
      findi1 = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return NONE
            end
            local x = tmp45({a1, a2})
            local tmp53 = a({a2, x})
            if tmp53 then
              return {tag = "SOME", payload = {a2, x}}
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      find2 = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return NONE
            end
            local x = tmp45({a1, a2})
            local tmp53 = a(x)
            if tmp53 then
              return {tag = "SOME", payload = x}
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      exists2 = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return false
            end
            local tmp53 = tmp45({a1, a2})
            local tmp54 = a(tmp53)
            if tmp54 then
              return true
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      all2 = function(a)
        return function(a1)
          local n = length1(a1)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= n then
              return true
            end
            local tmp53 = tmp45({a1, a2})
            local tmp54 = a(tmp53)
            if tmp54 then
              tmp52 = _Int_add(a2, 1)
              goto cont
            else
              return false
            end
          end
        end
      end
      collate1 = function(a)
        return function(a1)
          local xs = a1[1]
          local ys = a1[2]
          local xl = length1(xs)
          local yl = length1(ys)
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            local tmp53 = xl <= a2
            local tmp54 = yl <= a2
            local tmp55
            tmp55 = tmp53 and tmp54
            if tmp55 then
              return EQUAL
            end
            local tmp56
            tmp56 = tmp53 and not tmp54
            if tmp56 then
              return LESS
            end
            if not tmp53 and tmp54 then
              return GREATER
            end
            if not tmp53 and not tmp54 then
            else
              _raise(_Match, "mono-sequence.sml:621:49")
            end
            do
              local tmp57 = tmp45({xs, a2})
              local tmp58 = tmp45({ys, a2})
              local exp = a({tmp57, tmp58})
              if exp == "EQUAL" then
                tmp52 = _Int_add(a2, 1)
                goto cont
              else
                return exp
              end
            end
          end
        end
      end
      toList1 = function(a)
        local tmp52 = foldr2(_COLON_COLON)
        local tmp53 = tmp52(nil)
        return tmp53(a)
      end
      vector1 = function(a)
        local tmp52 = length1(a)
        local tmp53
        do
          local tmp54 = foldr2(_COLON_COLON)
          local tmp55 = tmp54(nil)
          tmp53 = tmp55(a)
        end
        return tmp49({tmp52, tmp53})
      end
      fromVector = function(a)
        local tmp52 = length2(a)
        local tmp53
        do
          local tmp54 = foldr1(_COLON_COLON)
          local tmp55 = tmp54(nil)
          tmp53 = tmp55(a)
        end
        return tmp44({tmp52, tmp53})
      end
      length3 = function(tmp52)
        return tmp52.length
      end
      sub4 = function(a)
        local base2 = a[1].base
        local start = a[1].start
        local length5 = a[1].length
        local i = a[2]
        if 0 <= i and i < length5 then
        else
          _raise(_Subscript, "mono-sequence.sml:645:44")
        end
        do
          return tmp45({base2, _Int_add(start, i)})
        end
      end
      update3 = function(a)
        local base2 = a[1].base
        local start = a[1].start
        local length5 = a[1].length
        local i = a[2]
        local x = a[3]
        if 0 <= i and i < length5 then
        else
          _raise(_Subscript, "mono-sequence.sml:649:50")
        end
        do
          return tmp46({base2, _Int_add(start, i), x})
        end
      end
      full1 = function(a)
        local tmp52 = length1(a)
        return {base = a, length = tmp52, start = 0}
      end
      slice = function(a)
        if a[3].tag == "NONE" then
        else
          goto else1
        end
        do
          local v = a[1]
          local i = a[2]
          local n = length1(v)
          if 0 <= i and i <= n then
            return {base = v, length = _Int_sub(n, i), start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:655:33")
          end
        end
        ::else1::
        if a[3].tag == "SOME" then
        else
          _raise(_Match, "mono-sequence.sml:651:5")
        end
        do
          local v = a[1]
          local i = a[2]
          local n = a[3].payload
          local tmp52
          do
            if 0 <= i then
            else
              tmp52 = false
              goto cont
            end
            do
              if 0 <= n then
              else
                tmp52 = false
                goto cont
              end
              do
                local tmp53 = _Int_add(i, n)
                local tmp54 = length1(v)
                tmp52 = tmp53 <= tmp54
              end
            end
          end
          ::cont::
          if tmp52 then
            return {base = v, length = n, start = i}
          else
            _raise(_Subscript, "mono-sequence.sml:660:32")
          end
        end
      end
      subslice = function(a)
        if a[3].tag == "NONE" then
          local base2 = a[1].base
          local start = a[1].start
          local length5 = a[1].length
          local i = a[2]
          if 0 <= i and i <= length5 then
            local tmp52 = _Int_add(start, i)
            return {base = base2, length = _Int_sub(length5, i), start = tmp52}
          else
            _raise(_Subscript, "mono-sequence.sml:664:55")
          end
        end
        if a[3].tag == "SOME" then
          local base2 = a[1].base
          local start = a[1].start
          local length5 = a[1].length
          local i = a[2]
          local n = a[3].payload
          if 0 <= i and (0 <= n and _Int_add(i, n) <= length5) then
            return {base = base2, length = n, start = _Int_add(start, i)}
          else
            _raise(_Subscript, "mono-sequence.sml:668:57")
          end
        else
          _raise(_Match, "mono-sequence.sml:661:5")
        end
      end
      base = function(a)
        local b = a.base
        local start = a.start
        return {b, start, a.length}
      end
      copy1 = function(a)
        local base2 = a.src.base
        local start = a.src.start
        local length5 = a.src.length
        local dst = a.dst
        local di = a.di
        local tmp52
        do
          if di < 0 then
            tmp52 = true
            goto cont
          end
          local tmp53 = length1(dst)
          tmp52 = tmp53 < _Int_add(di, length5)
        end
        ::cont::
        if tmp52 then
          _raise(_Subscript, "mono-sequence.sml:684:14")
        end
        if start >= di then
        else
          goto else1
        end
        do
          local tmp53 = 0
          ::cont2::
          do
            local a1 = tmp53
            if a1 >= length5 then
              return nil
            end
            local tmp54 = _Int_add(di, a1)
            local tmp55 = tmp45({base2, _Int_add(start, a1)})
            tmp46({dst, tmp54, tmp55})
            tmp53 = _Int_add(a1, 1)
            goto cont2
          end
        end
        ::else1::
        local tmp53 = _Int_sub(length5, 1)
        ::cont1::
        do
          local a1 = tmp53
          if a1 < 0 then
            return nil
          end
          local tmp54 = _Int_add(di, a1)
          local tmp55 = tmp45({base2, _Int_add(start, a1)})
          tmp46({dst, tmp54, tmp55})
          tmp53 = _Int_sub(a1, 1)
          goto cont1
        end
      end
      copyVec1 = function(a)
        local base2, start, length5, dst, di, tmp52
        do
          base2 = a.src.base
          start = a.src.start
          length5 = a.src.length
          dst = a.dst
          di = a.di
          if di < 0 then
            _raise(_Subscript, "mono-sequence.sml:699:14")
          end
          local tmp53 = length1(dst)
          if tmp53 < _Int_add(di, length5) then
            _raise(_Subscript, "mono-sequence.sml:699:14")
          end
          tmp52 = 0
        end
        ::cont::
        do
          local a1 = tmp52
          if a1 >= length5 then
            return nil
          end
          local tmp53 = _Int_add(di, a1)
          local tmp54 = tmp51({base2, _Int_add(start, a1)})
          tmp46({dst, tmp53, tmp54})
          tmp52 = _Int_add(a1, 1)
          goto cont
        end
      end
      isEmpty1 = function(a)
        return a.length == 0
      end
      getItem = function(a)
        local base2 = a.base
        local start = a.start
        local length5 = a.length
        if length5 > 0 then
        else
          return NONE
        end
        do
          local tmp52 = tmp45({base2, start})
          local tmp53 = _Int_add(start, 1)
          return {tag = "SOME", payload = {tmp52, {base = base2, length = _Int_sub(length5, 1), start = tmp53}}}
        end
      end
      appi2 = function(a)
        return function(a1)
          local base2 = a1.base
          local start = a1.start
          local length5 = a1.length
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= length5 then
              return nil
            end
            local tmp53 = tmp45({base2, _Int_add(start, a2)})
            a({a2, tmp53})
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      app4 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return nil
            end
            local tmp54 = tmp45({base2, a2})
            a(tmp54)
            tmp53 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      modifyi1 = function(a)
        return function(a1)
          local base2 = a1.base
          local start = a1.start
          local length5 = a1.length
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= length5 then
              return nil
            end
            local tmp53 = _Int_add(start, a2)
            local x = tmp45({base2, tmp53})
            local y = a({a2, x})
            tmp46({base2, tmp53, y})
            tmp52 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      modify1 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return nil
            end
            local x = tmp45({base2, a2})
            local y = a(x)
            tmp46({base2, a2, y})
            tmp53 = _Int_add(a2, 1)
            goto cont
          end
        end
      end
      foldli2 = function(a)
        return function(a1)
          return function(a2)
            local base2 = a2.base
            local start = a2.start
            local length5 = a2.length
            local tmp52, tmp53 = 0, a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i >= length5 then
                return acc
              end
              local tmp54 = _Int_add(i, 1)
              local tmp55 = tmp45({base2, _Int_add(start, i)})
              local tmp56 = a({i, tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      foldri2 = function(a)
        return function(a1)
          return function(a2)
            local base2 = a2.base
            local start = a2.start
            local tmp52, tmp53 = _Int_sub(a2.length, 1), a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i < 0 then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp45({base2, _Int_add(start, i)})
              local tmp56 = a({i, tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      foldl3 = function(a)
        return function(a1)
          return function(a2)
            local base2, tmp52, tmp53, tmp54
            do
              base2 = a2.base
              local start = a2.start
              tmp52 = _Int_add(start, a2.length)
              tmp54, tmp53 = start, a1
            end
            ::cont::
            do
              local i, acc = tmp54, tmp53
              if i >= tmp52 then
                return acc
              end
              local tmp55 = _Int_add(i, 1)
              local tmp56 = tmp45({base2, i})
              local tmp57 = a({tmp56, acc})
              tmp54 = tmp55
              tmp53 = tmp57
              goto cont
            end
          end
        end
      end
      foldr3 = function(a)
        return function(a1)
          return function(a2)
            local base2 = a2.base
            local start = a2.start
            local tmp52, tmp53 = _Int_sub(_Int_add(start, a2.length), 1), a1
            ::cont::
            do
              local i, acc = tmp52, tmp53
              if i < start then
                return acc
              end
              local tmp54 = _Int_sub(i, 1)
              local tmp55 = tmp45({base2, i})
              local tmp56 = a({tmp55, acc})
              tmp52 = tmp54
              tmp53 = tmp56
              goto cont
            end
          end
        end
      end
      findi2 = function(a)
        return function(a1)
          local base2 = a1.base
          local start = a1.start
          local length5 = a1.length
          local tmp52 = 0
          ::cont::
          do
            local a2 = tmp52
            if a2 >= length5 then
              return NONE
            end
            local x = tmp45({base2, _Int_add(start, a2)})
            local tmp53 = a({a2, x})
            if tmp53 then
              return {tag = "SOME", payload = {a2, x}}
            else
              tmp52 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      find3 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return NONE
            end
            local x = tmp45({base2, a2})
            local tmp54 = a(x)
            if tmp54 then
              return {tag = "SOME", payload = x}
            else
              tmp53 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      exists3 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return false
            end
            local tmp54 = tmp45({base2, a2})
            local tmp55 = a(tmp54)
            if tmp55 then
              return true
            else
              tmp53 = _Int_add(a2, 1)
              goto cont
            end
          end
        end
      end
      all3 = function(a)
        return function(a1)
          local base2, tmp52, tmp53
          do
            base2 = a1.base
            local start = a1.start
            tmp52 = _Int_add(start, a1.length)
            tmp53 = start
          end
          ::cont::
          do
            local a2 = tmp53
            if a2 >= tmp52 then
              return true
            end
            local tmp54 = tmp45({base2, a2})
            local tmp55 = a(tmp54)
            if tmp55 then
              tmp53 = _Int_add(a2, 1)
              goto cont
            else
              return false
            end
          end
        end
      end
      collate2 = function(a)
        return function(a1)
          local base2, base_PRIME, tmp52, tmp53, tmp54, tmp55
          do
            base2 = a1[1].base
            local start = a1[1].start
            local length5 = a1[1].length
            base_PRIME = a1[2].base
            local start_PRIME = a1[2].start
            local length_PRIME = a1[2].length
            tmp52 = _Int_add(start, length5)
            tmp53 = _Int_add(start_PRIME, length_PRIME)
            tmp55, tmp54 = start, start_PRIME
          end
          ::cont::
          do
            local i, j = tmp55, tmp54
            local tmp56 = tmp52 <= i
            local tmp57 = tmp53 <= j
            local tmp58
            tmp58 = tmp56 and tmp57
            if tmp58 then
              return EQUAL
            end
            local tmp59
            tmp59 = tmp56 and not tmp57
            if tmp59 then
              return LESS
            end
            if not tmp56 and tmp57 then
              return GREATER
            end
            if not tmp56 and not tmp57 then
            else
              _raise(_Match, "mono-sequence.sml:812:29")
            end
            do
              local tmp60 = tmp45({base2, i})
              local tmp61 = tmp45({base_PRIME, j})
              local exp = a({tmp60, tmp61})
              if exp == "EQUAL" then
                tmp55 = _Int_add(i, 1)
                tmp54 = _Int_add(j, 1)
                goto cont
              else
                return exp
              end
            end
          end
        end
      end
      vector2 = function(a)
        local tmp52 = a.length
        local tmp53 = foldr3(_COLON_COLON)
        local tmp54 = tmp53(nil)
        local tmp55 = tmp54(a)
        return tmp49({tmp52, tmp55})
      end
      UnsafeMonoVector = {sub = tmp51}
      UnsafeMonoArray = {create = create, sub = tmp45, update = tmp46}
      MonoVectorSlice = {all = all4, app = app5, appi = appi3, base = base1, collate = collate3, concat = concat2, exists = exists4, find = find4, findi = findi3, foldl = foldl4, foldli = foldli3, foldr = foldr4, foldri = foldri3, full = full2, getItem = getItem1, isEmpty = isEmpty2, length = length4, map = map2, mapi = mapi1, slice = slice1, sub = sub5, subslice = subslice1, vector = vector}
    end
    local MonoVector = {all = all1, app = app2, append = append, appi = appi, collate = collate, concat = tmp47, exists = exists1, find = find1, findi = findi, foldl = foldl1, foldli = foldli, foldr = foldr1, foldri = foldri, fromList = fromList1, length = length2, map = map1, mapi = mapi, maxLen = maxLen1, prepend = prepend, sub = sub2, tabulate = tabulate1, toList = toList, update = update1}
    local MonoArraySlice = {all = all3, app = app4, appi = appi2, base = base, collate = collate2, copy = copy1, copyVec = copyVec1, exists = exists3, find = find3, findi = findi2, foldl = foldl3, foldli = foldli2, foldr = foldr3, foldri = foldri2, full = full1, getItem = getItem, isEmpty = isEmpty1, length = length3, modify = modify1, modifyi = modifyi1, slice = slice, sub = sub4, subslice = subslice, update = update3, vector = vector2}
    return {_MonoArray = {all = all2, app = app3, appi = appi1, array = array1, collate = collate1, copy = copy, copyVec = copyVec, exists = exists2, find = find2, findi = findi1, foldl = foldl2, foldli = foldli1, foldr = foldr2, foldri = foldri1, fromList = fromList, fromVector = fromVector, length = length1, maxLen = maxLen, modify = modify, modifyi = modifyi, sub = sub3, tabulate = tabulate2, toList = toList1, toVector = vector1, update = update2, vector = vector1}, _MonoArraySlice = MonoArraySlice, _MonoVector = MonoVector, _MonoVectorSlice = MonoVectorSlice, _UnsafeMonoArray = UnsafeMonoArray, _UnsafeMonoVector = UnsafeMonoVector}
  end
  local unsafeFromListN = function(a)
    return implode(a[2])
  end
  local unsafeFromListRevN = function(a)
    return implodeRev(a[2])
  end
  local sliceToVector = function(a)
    local base = a.base
    local start = a.start
    return substring(base, start, a.length)
  end
  local unsafeCreateWithZero = function(a)
    return _Array_array(a, 0)
  end
  local Base = MonoSequence(_VectorOrArray_fromList, length, math_maxinteger, array, unsafeCreateWithZero, function(a)
    return _VectorOrArray_fromList(a[2])
  end, sub, update, concat, implode, size, math_maxinteger, sliceToVector, sliceToVector, unsafeFromListN, unsafeFromListRevN, sub1)
  local CharVector = Base._MonoVector
  local CharVectorSlice = Base._MonoVectorSlice
  CharArray = Base._MonoArray
  local CharArraySlice = Base._MonoArraySlice
  local UnsafeCharVector = Base._UnsafeMonoVector
  local sub2 = CharVectorSlice.sub
  extract1 = CharVectorSlice.slice
  full = CharVectorSlice.full
  string1 = CharVectorSlice.vector
  isEmpty = CharVectorSlice.isEmpty
  getc = CharVectorSlice.getItem
  concat1 = CharVectorSlice.concat
  splitl = function(a)
    return function(a1)
      local base = a1.base
      local start = a1.start
      local length1 = a1.length
      local tmp43 = 0
      ::cont::
      do
        local a2 = tmp43
        if a2 >= length1 then
          return {a1, {base = base, length = 0, start = _Int_add(start, a2)}}
        end
        local tmp44 = sub2({a1, a2})
        local tmp45 = a(tmp44)
        if tmp45 then
          tmp43 = _Int_add(a2, 1)
          goto cont
        else
          local tmp46 = {base = base, length = a2, start = start}
          return {tmp46, {base = base, length = _Int_sub(length1, a2), start = _Int_add(start, a2)}}
        end
      end
    end
  end
  splitr = function(a, a1)
    local base = a1.base
    local start = a1.start
    local length1 = a1.length
    local tmp43 = _Int_sub(length1, 1)
    ::cont::
    do
      local a2 = tmp43
      if a2 < 0 then
        return {{base = base, length = 0, start = start}, a1}
      end
      local tmp44 = sub2({a1, a2})
      local tmp45 = a(tmp44)
      if tmp45 then
        tmp43 = _Int_sub(a2, 1)
        goto cont
      else
        local tmp46 = _Int_add(a2, 1)
        local tmp47 = {base = base, length = tmp46, start = start}
        local tmp48 = _Int_add(start, tmp46)
        return {tmp47, {base = base, length = _Int_sub(length1, tmp46), start = tmp48}}
      end
    end
  end
  tokens = function(a)
    return function(a1)
      local base, tmp43, tmp44, tmp45, tmp46
      do
        base = a1.base
        local start = a1.start
        tmp43 = _Int_add(start, a1.length)
        tmp46, tmp45, tmp44 = nil, start, start
      end
      ::cont::
      do
        local revTokens, s, i = tmp46, tmp45, tmp44
        if i >= tmp43 then
        else
          goto else1
        end
        do
          if s == i then
          else
            return revAppend({{base = base, length = _Int_sub(i, s), start = s}, revTokens}, nil)
          end
          do
            return revAppend(revTokens, nil)
          end
        end
        ::else1::
        local tmp47 = sub1({base, i})
        local tmp48 = a(tmp47)
        if tmp48 then
          local tmp49 = _Int_add(i, 1)
          if s == i then
            tmp46 = revTokens
            tmp45 = tmp49
            tmp44 = tmp49
            goto cont
          else
            tmp46 = {{base = base, length = _Int_sub(i, s), start = s}, revTokens}
            tmp45 = tmp49
            tmp44 = tmp49
            goto cont
          end
        else
          tmp46 = revTokens
          tmp45 = s
          tmp44 = _Int_add(i, 1)
          goto cont
        end
      end
    end
  end
  tmp26 = UnsafeCharVector.sub
  tmp27 = CharVectorSlice.concat
  tmp28 = CharVectorSlice.full
  tmp29 = CharVectorSlice.length
  tmp30 = CharVectorSlice.slice
  tmp31 = CharVectorSlice.vector
  tmp32 = CharVector.all
  tmp33 = CharVector.appi
  tmp34 = CharVector.exists
  tmp35 = CharVector.fromList
  tmp36 = CharVector.length
  tmp37 = CharArraySlice.base
  tmp38 = CharArraySlice.copyVec
  tmp39 = CharArraySlice.full
  tmp40 = CharArraySlice.length
  tmp41 = CharArraySlice.slice
  tmp42 = CharArraySlice.vector
end
local _L = {}
do
  _L[1] = CharArray.array
  _L[2] = CharArray.copyVec
  _L[3] = function(getc1, strm)
    local tmp43, tmp44 = getc1, strm
    ::cont::
    do
      local getc2, strm1 = tmp43, tmp44
      local exp = getc2(strm1)
      if exp.tag == "NONE" then
        return strm1
      end
      if exp.tag == "SOME" then
      else
        _raise(_Match, "scan-num-utils.sml:2:42")
      end
      do
        local c = exp.payload[1]
        local strm_PRIME = exp.payload[2]
        local tmp45 = isSpace(c)
        if tmp45 then
          tmp43 = getc2
          tmp44 = strm_PRIME
          goto cont
        else
          return strm1
        end
      end
    end
  end
  _L[4] = function(a)
    return a == 48 or a == 49
  end
  _L[5] = function(a)
    return 48 <= a and a <= 55
  end
  _L[6] = function(a)
    if 48 <= a and a <= 57 then
      return _Int_sub(a, 48)
    elseif 97 <= a and a <= 102 then
      return _Int_add(_Int_sub(a, 97), 10)
    else
      return _Int_add(_Int_sub(a, 65), 10)
    end
  end
  _L[7] = function(getc1, strm)
    local exp = getc1(strm)
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
  local scanSubstring = function(getc1, strm, s)
    local tmp43, tmp44, tmp45 = getc1, strm, s
    ::cont::
    do
      local getc2, strm1, s1 = tmp43, tmp44, tmp45
      local exp = getc(s1)
      if exp.tag == "NONE" then
        return {tag = "SOME", payload = strm1}
      end
      if exp.tag == "SOME" then
      else
        _raise(_Match, "scan-num.sml:341:37")
      end
      do
        local c = exp.payload[1]
        local s_PRIME = exp.payload[2]
        local exp1 = getc2(strm1)
        if exp1.tag == "SOME" then
        elseif exp1.tag == "NONE" then
          return NONE
        else
          _raise(_Match, "scan-num.sml:343:57")
        end
        do
          local strm_PRIME, tmp46
          do
            local c_PRIME = exp1.payload[1]
            strm_PRIME = exp1.payload[2]
            do
              if c == c_PRIME then
                tmp46 = true
                goto cont1
              end
              local tmp47
              tmp47 = 65 <= c and c <= 90 or 97 <= c and c <= 122
              if tmp47 then
              else
                tmp46 = false
                goto cont1
              end
              do
                local tmp48
                tmp48 = 65 <= c_PRIME and c_PRIME <= 90 or 97 <= c_PRIME and c_PRIME <= 122
                if tmp48 then
                else
                  tmp46 = false
                  goto cont1
                end
                do
                  local tmp49 = toUpper(c)
                  local tmp50 = toUpper(c_PRIME)
                  tmp46 = tmp49 == tmp50
                end
              end
            end
          end
          ::cont1::
          if tmp46 then
            tmp43 = getc2
            tmp44 = strm_PRIME
            tmp45 = s_PRIME
            goto cont
          else
            return NONE
          end
        end
      end
    end
  end
  local scanZeroOrMoreDigits = function(getc1, strm, revAcc)
    local tmp43, tmp44, tmp45 = getc1, strm, revAcc
    ::cont::
    do
      local getc2, strm1, revAcc1 = tmp43, tmp44, tmp45
      local exp = getc2(strm1)
      if exp.tag == "SOME" then
        local c = exp.payload[1]
        local strm_PRIME = exp.payload[2]
        if 48 <= c and c <= 57 then
          tmp43 = getc2
          tmp44 = strm_PRIME
          tmp45 = {c, revAcc1}
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
  end
  local scanOneOrMoreDigits = function(getc1, strm, revAcc)
    local exp = getc1(strm)
    if exp.tag == "SOME" then
    elseif exp.tag == "NONE" then
      return NONE
    else
      _raise(_Match, "scan-num.sml:355:48")
    end
    do
      local c = exp.payload[1]
      local strm_PRIME = exp.payload[2]
      if 48 <= c and c <= 57 then
      else
        return NONE
      end
      do
        local tmp43 = scanZeroOrMoreDigits(getc1, strm_PRIME, {c, revAcc})
        return {tag = "SOME", payload = tmp43}
      end
    end
  end
  local scanOptExpPart = function(getc1, strm, revAcc)
    local exp = getc1(strm)
    if exp.tag == "SOME" then
    elseif exp.tag == "NONE" then
      return {revAcc, strm}
    else
      _raise(_Match, "scan-num.sml:367:43")
    end
    do
      local c = exp.payload[1]
      local strm_PRIME = exp.payload[2]
      if c == 101 or c == 69 then
      else
        return {revAcc, strm}
      end
      do
        local exp1 = _L[7](getc1, strm_PRIME)
        local isNegative = exp1[1]
        local strm_PRIME_PRIME = exp1[2]
        local tmp43
        if isNegative then
          tmp43 = {45, {101, revAcc}}
        else
          tmp43 = {101, revAcc}
        end
        local exp2 = scanOneOrMoreDigits(getc1, strm_PRIME_PRIME, tmp43)
        if exp2.tag == "SOME" then
          local revAcc1 = exp2.payload[1]
          return {revAcc1, exp2.payload[2]}
        elseif exp2.tag == "NONE" then
          return {revAcc, strm}
        else
          _raise(_Match, "scan-num.sml:370:73")
        end
      end
    end
  end
  _L[8] = function(a)
    return function(a1)
      local isNegative, signPart, exp
      do
        local strm = _L[3](a, a1)
        local exp1 = _L[7](a, strm)
        isNegative = exp1[1]
        local strm1 = exp1[2]
        if isNegative then
          signPart = {45, nil}
        else
          signPart = nil
        end
        exp = a(strm1)
        if exp.tag == "SOME" and exp.payload[1] == 46 then
        else
          goto else1
        end
        do
          local exp2 = scanOneOrMoreDigits(a, exp.payload[2], {46, {48, signPart}})
          if exp2.tag == "SOME" then
          elseif exp2.tag == "NONE" then
            return NONE
          else
            _raise(_Match, "scan-num.sml:385:52")
          end
          do
            local exp3 = scanOptExpPart(a, exp2.payload[2], exp2.payload[1])
            local revAcc = exp3[1]
            local strm_PRIME_PRIME_PRIME = exp3[2]
            local tmp43 = implodeRev(revAcc)
            return {tag = "SOME", payload = {tmp4(tmp43) * 1.0, strm_PRIME_PRIME_PRIME}}
          end
        end
      end
      ::else1::
      if exp.tag == "SOME" then
      elseif exp.tag == "NONE" then
        return NONE
      else
        _raise(_Match, "scan-num.sml:384:25")
      end
      do
        local c = exp.payload[1]
        local strm_PRIME = exp.payload[2]
        if c == 105 or c == 73 then
        else
          goto else2
        end
        do
          local tmp43 = full("nf")
          local exp1 = scanSubstring(a, strm_PRIME, tmp43)
          if exp1.tag == "SOME" then
          elseif exp1.tag == "NONE" then
            return NONE
          else
            _raise(_Match, "scan-num.sml:393:52")
          end
          do
            local strm_PRIME_PRIME = exp1.payload
            local inf
            if isNegative then
              inf = tmp25
            else
              inf = tmp11
            end
            local tmp44 = full("inity")
            local exp2 = scanSubstring(a, strm_PRIME_PRIME, tmp44)
            if exp2.tag == "SOME" then
              return {tag = "SOME", payload = {inf, exp2.payload}}
            elseif exp2.tag == "NONE" then
              return {tag = "SOME", payload = {inf, strm_PRIME_PRIME}}
            else
              _raise(_Match, "scan-num.sml:395:74")
            end
          end
        end
        ::else2::
        if c == 110 or c == 78 then
        else
          goto else3
        end
        do
          local tmp43 = full("an")
          local exp1 = scanSubstring(a, strm_PRIME, tmp43)
          if exp1.tag == "SOME" then
            local strm_PRIME_PRIME = exp1.payload
            return {tag = "SOME", payload = {0.0 / 0.0, strm_PRIME_PRIME}}
          elseif exp1.tag == "NONE" then
            return NONE
          else
            _raise(_Match, "scan-num.sml:402:52")
          end
        end
        ::else3::
        if 48 <= c and c <= 57 then
        else
          return NONE
        end
        do
          local strm_PRIME_PRIME_PRIME, revAcc
          do
            local exp1 = scanZeroOrMoreDigits(a, strm_PRIME, {c, signPart})
            local revAcc1 = exp1[1]
            local strm_PRIME_PRIME = exp1[2]
            do
              local exp2 = a(strm_PRIME_PRIME)
              if exp2.tag == "SOME" and exp2.payload[1] == 46 then
              else
                strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME
                revAcc = revAcc1
                goto cont
              end
              do
                local exp3 = scanOneOrMoreDigits(a, exp2.payload[2], {46, revAcc1})
                if exp3.tag == "SOME" then
                  strm_PRIME_PRIME_PRIME = exp3.payload[2]
                  revAcc = exp3.payload[1]
                elseif exp3.tag == "NONE" then
                  strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME
                  revAcc = revAcc1
                else
                  _raise(_Match, "scan-num.sml:362:71")
                end
              end
            end
          end
          ::cont::
          local exp1 = scanOptExpPart(a, strm_PRIME_PRIME_PRIME, revAcc)
          local revAcc1 = exp1[1]
          local strm_PRIME_PRIME_PRIME_PRIME = exp1[2]
          local tmp43 = implodeRev(revAcc1)
          return {tag = "SOME", payload = {tmp4(tmp43) * 1.0, strm_PRIME_PRIME_PRIME_PRIME}}
        end
      end
    end
  end
  _L[9] = {"Io"}
  _L[10] = {tag = {"BlockingNotSupported"}}
  _L[11] = {tag = {"NonblockingNotSupported"}}
  _L[12] = {tag = {"RandomAccessNotSupported"}}
  _L[13] = {tag = {"ClosedStream"}}
  _L[14] = "NO_BUF"
  _L[15] = "LINE_BUF"
  _L[16] = "BLOCK_BUF"
  _L[17] = function(tmp43, tmp44)
    return tmp43 == "NO_BUF" and tmp44 == "NO_BUF" or (tmp43 == "LINE_BUF" and tmp44 == "LINE_BUF" or tmp43 == "BLOCK_BUF" and tmp44 == "BLOCK_BUF")
  end
  _L[18] = function(base, length1, start, i)
    if 0 <= i and i < length1 then
      return base[_Int_add(start, i) + 1]
    else
      _raise(_Subscript, "vector-slice.sml:39:44")
    end
  end
  _L[19] = function(a, i, tmp43)
    if tmp43.tag == "NONE" then
      if 0 <= i and i <= a.n then
        return {base = a, length = _Int_sub(a.n, i), start = i}
      else
        _raise(_Subscript, "vector-slice.sml:44:30")
      end
    end
    if tmp43.tag == "SOME" then
      local n = tmp43.payload
      local tmp44
      if 0 <= i then
        if 0 <= n then
          local tmp45 = _Int_add(i, n)
          tmp44 = tmp45 <= a.n
        else
          tmp44 = false
        end
      else
        tmp44 = false
      end
      if tmp44 then
        return {base = a, length = n, start = i}
      else
        _raise(_Subscript, "vector-slice.sml:48:32")
      end
    else
      _raise(_Match, "vector-slice.sml:41:5")
    end
  end
  _L[20] = function(a)
    return function(a1)
      local base = a1.base
      local start = a1.start
      local length1 = a1.length
      local tmp43 = 0
      ::cont::
      do
        local a2 = tmp43
        if a2 >= length1 then
          return false
        end
        local tmp44 = a(base[_Int_add(start, a2) + 1])
        if tmp44 then
          return true
        else
          tmp43 = _Int_add(a2, 1)
          goto cont
        end
      end
    end
  end
  _L[21] = {tag = "ZERO"}
  local tmp43 = 0x1 << tmp24 // 2
  local tmp44
  if tmp43 < 0x0 then
    _raise(_Overflow, "word-1.sml:52:39")
  else
    tmp44 = tmp43
  end
  _L[22] = function(a)
    if a == 0 then
      return _L[21]
    end
    if a > 0 then
    else
      goto else1
    end
    do
      local function go(a1)
        if a1 == 0 then
          return nil
        end
        local tmp45 = _Int_div(a1, tmp44)
        local tmp46 = _Int_mod(a1, tmp44)
        local tmp47 = _Int_div(tmp45, tmp44)
        local tmp48 = tmp46 + _Int_mod(tmp45, tmp44) * tmp43
        local tmp49 = go(tmp47)
        return {tmp48, tmp49}
      end
      local tmp45 = go(a)
      return {tag = "POSITIVE", payload = _VectorOrArray_fromList(tmp45)}
    end
    ::else1::
    local function go(a1)
      if a1 == 0 then
        return nil
      end
      local q1 = quot(a1, tmp44)
      local r1 = rem(a1, tmp44)
      local q2 = quot(q1, tmp44)
      local r2 = rem(q1, tmp44)
      local tmp45 = _Int_negate(r1)
      local tmp46 = _Int_negate(r2)
      local tmp47 = tmp45 + tmp46 * tmp43
      local tmp48 = go(q2)
      return {tmp47, tmp48}
    end
    local tmp45 = go(a)
    return {tag = "NEGATIVE", payload = _VectorOrArray_fromList(tmp45)}
  end
  local add2 = function(x, y)
    local tmp45 = x + y
    if math_ult(tmp45, x) then
      return {hi = 0x1, lo = tmp45}
    else
      return {hi = 0x0, lo = tmp45}
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
  local tmp45 = tmp24 >> 1
  local tmp46 = (0x1 << tmp45) - 0x1
  _L[23] = function(words, words_PRIME)
    local tmp47 = words.n
    local exp = compare({tmp47, words_PRIME.n})
    if exp == "EQUAL" then
      local tmp48 = _Int_sub(tmp47, 1)
      ::cont::
      do
        local a = tmp48
        if a < 0 then
          return EQUAL
        end
        local tmp49 = words[a + 1]
        local tmp50 = words_PRIME[a + 1]
        local exp1
        if tmp49 == tmp50 then
          exp1 = EQUAL
        elseif math_ult(tmp49, tmp50) then
          exp1 = LESS
        else
          exp1 = GREATER
        end
        if exp1 == "EQUAL" then
          tmp48 = _Int_sub(a, 1)
          goto cont
        else
          return exp1
        end
      end
    else
      return exp
    end
  end
  _L[24] = function(a)
    local length1
    do
      local tmp47 = a.n
      ::cont::
      do
        local a1 = tmp47
        local tmp48 = _Int_sub(a1, 1)
        if tmp48 < 0 then
          length1 = a1
        elseif a[tmp48 + 1] ~= 0x0 then
          length1 = a1
        else
          tmp47 = tmp48
          goto cont
        end
      end
    end
    if 0 <= length1 and length1 <= a.n then
    else
      _raise(_Subscript, "array-slice.sml:52:32")
    end
    do
      return _VectorOrArray_tabulate({length1, function(i)
        return a[i + 1]
      end})
    end
  end
  _L[25] = function(words, words_PRIME)
    local tmp47, tmp48, tmp49, tmp50, tmp51, tmp52
    do
      tmp47 = words.n
      tmp48 = words_PRIME.n
      local tmp53
      if tmp47 < tmp48 then
        tmp53 = tmp48
      else
        tmp53 = tmp47
      end
      tmp49 = _Int_add(tmp53, 1)
      tmp50 = _Array_array(tmp49, 0x0)
      tmp52, tmp51 = 0x0, 0
    end
    ::cont::
    do
      local carry, i = tmp52, tmp51
      if i == tmp49 then
      else
        goto else1
      end
      do
        return _L[24](tmp50)
      end
      ::else1::
      local w
      if i < tmp47 then
        w = words[i + 1]
      else
        w = 0x0
      end
      local w_PRIME
      if i < tmp48 then
        w_PRIME = words_PRIME[i + 1]
      else
        w_PRIME = 0x0
      end
      local exp = add3(w, w_PRIME, carry)
      local x = exp.lo
      local carry1 = exp.hi
      tmp50[i + 1] = x
      tmp52 = carry1
      tmp51 = _Int_add(i, 1)
      goto cont
    end
  end
  _L[26] = function(words, words_PRIME)
    local tmp47 = words.n
    local tmp48 = words_PRIME.n
    local tmp49 = _Array_array(tmp47, 0x0)
    local tmp50, tmp51 = 0x0, 0
    ::cont::
    do
      local carry, i = tmp50, tmp51
      if i == tmp47 then
      else
        goto else1
      end
      do
        if carry == 0x0 then
        else
          goto else2
        end
        do
          return _L[24](tmp49)
        end
        ::else2::
        local tmp52 = _Fail("subAbs: carry not zero")
        _raise(tmp52, "int-inf.sml:276:77")
      end
      ::else1::
      local tmp52 = words[i + 1]
      local w_PRIME
      if i < tmp48 then
        w_PRIME = words_PRIME[i + 1]
      else
        w_PRIME = 0x0
      end
      local carry1, lo
      if math_ult(tmp52, w_PRIME) then
        lo = tmp52 - w_PRIME - carry
        carry1 = 0x1
      elseif math_ult(w_PRIME, tmp52) then
        lo = tmp52 - w_PRIME - carry
        carry1 = 0x0
      elseif carry == 0x0 then
        lo = 0x0
        carry1 = 0x0
      else
        lo = - carry
        carry1 = 0x1
      end
      tmp49[i + 1] = lo
      tmp50 = carry1
      tmp51 = _Int_add(i, 1)
      goto cont
    end
  end
  _L[27] = function(words, words_PRIME)
    local tmp47 = words.n
    local tmp48 = words_PRIME.n
    local tmp49 = _Array_array(_Int_add(tmp47, tmp48), 0x0)
    local tmp50 = 0
    ::cont::
    do
      local a = tmp50
      if a >= tmp48 then
      else
        goto else1
      end
      do
        return _L[24](tmp49)
      end
      ::else1::
      local tmp51 = words_PRIME[a + 1]
      local tmp52, tmp53 = 0, 0x0
      ::cont1::
      do
        local k, i, lo, hi
        do
          i, k = tmp52, tmp53
          if i >= tmp47 then
            tmp49[_Int_add(i, a) + 1] = k
            tmp50 = _Int_add(a, 1)
            goto cont
          end
          local tmp54 = words[i + 1]
          do
            if tmp54 == 0x0 then
              lo = 0x0
              hi = 0x0
              goto cont2
            end
            if tmp51 == 0x0 then
              lo = 0x0
              hi = 0x0
              goto cont2
            end
            local tmp55 = tmp54 >> tmp45
            local tmp56 = tmp54 & tmp46
            local tmp57 = tmp51 >> tmp45
            local tmp58 = tmp51 & tmp46
            local tmp59 = tmp56 * tmp58
            local tmp60 = tmp55 * tmp58
            local tmp61 = tmp60 << tmp45
            local tmp62 = tmp60 >> tmp45
            local tmp63 = tmp56 * tmp57
            local tmp64 = tmp63 << tmp45
            local tmp65 = tmp63 >> tmp45
            local tmp66 = tmp55 * tmp57
            local exp = add3(tmp59, tmp61, tmp64)
            local lo1 = exp.lo
            local hi4 = exp.hi
            lo = lo1
            hi = tmp62 + tmp65 + tmp66 + hi4
          end
        end
        ::cont2::
        local exp = add3(lo, tmp49[_Int_add(i, a) + 1], k)
        local lo1 = exp.lo
        local hi_PRIME = exp.hi
        tmp49[_Int_add(i, a) + 1] = lo1
        local tmp54 = _Int_add(i, 1)
        tmp52 = tmp54
        tmp53 = hi + hi_PRIME
        goto cont1
      end
    end
  end
end
local add, mul, log2Word, LShiftAbs, scan, tmp43
do
  add = function(tmp44, y)
    if tmp44.tag == "ZERO" then
      return y
    end
    if y.tag == "ZERO" then
      return tmp44
    end
    if tmp44.tag == "POSITIVE" and y.tag == "POSITIVE" then
    else
      goto else1
    end
    do
      local words = tmp44.payload
      local tmp45 = _L[25](words, y.payload)
      return {tag = "POSITIVE", payload = tmp45}
    end
    ::else1::
    if tmp44.tag == "POSITIVE" and y.tag == "NEGATIVE" then
    else
      goto else2
    end
    do
      local words = tmp44.payload
      local words_PRIME = y.payload
      local exp = _L[23](words, words_PRIME)
      if exp == "LESS" then
      else
        goto else4
      end
      do
        local tmp45 = _L[26](words_PRIME, words)
        return {tag = "NEGATIVE", payload = tmp45}
      end
      ::else4::
      if exp == "GREATER" then
      elseif exp == "EQUAL" then
        return _L[21]
      else
        _raise(_Match, "int-inf.sml:347:46")
      end
      do
        local tmp45 = _L[26](words, words_PRIME)
        return {tag = "POSITIVE", payload = tmp45}
      end
    end
    ::else2::
    if tmp44.tag == "NEGATIVE" and y.tag == "POSITIVE" then
    else
      goto else3
    end
    do
      local words = tmp44.payload
      local words_PRIME = y.payload
      local exp = _L[23](words, words_PRIME)
      if exp == "LESS" then
      else
        goto else4
      end
      do
        local tmp45 = _L[26](words_PRIME, words)
        return {tag = "POSITIVE", payload = tmp45}
      end
      ::else4::
      if exp == "GREATER" then
      elseif exp == "EQUAL" then
        return _L[21]
      else
        _raise(_Match, "int-inf.sml:352:46")
      end
      do
        local tmp45 = _L[26](words, words_PRIME)
        return {tag = "NEGATIVE", payload = tmp45}
      end
    end
    ::else3::
    if tmp44.tag == "NEGATIVE" and y.tag == "NEGATIVE" then
    else
      _raise(_Match, "int-inf.sml:344:5")
    end
    do
      local words = tmp44.payload
      local tmp45 = _L[25](words, y.payload)
      return {tag = "NEGATIVE", payload = tmp45}
    end
  end
  mul = function(tmp44, tmp45)
    if tmp44.tag == "ZERO" then
      return tmp44
    end
    if tmp45.tag == "ZERO" then
      return tmp45
    end
    if tmp44.tag == "POSITIVE" and tmp45.tag == "POSITIVE" then
    else
      goto else1
    end
    do
      local words = tmp44.payload
      local tmp46 = _L[27](words, tmp45.payload)
      return {tag = "POSITIVE", payload = tmp46}
    end
    ::else1::
    if tmp44.tag == "POSITIVE" and tmp45.tag == "NEGATIVE" then
    else
      goto else2
    end
    do
      local words = tmp44.payload
      local tmp46 = _L[27](words, tmp45.payload)
      return {tag = "NEGATIVE", payload = tmp46}
    end
    ::else2::
    if tmp44.tag == "NEGATIVE" and tmp45.tag == "POSITIVE" then
    else
      goto else3
    end
    do
      local words = tmp44.payload
      local tmp46 = _L[27](words, tmp45.payload)
      return {tag = "NEGATIVE", payload = tmp46}
    end
    ::else3::
    if tmp44.tag == "NEGATIVE" and tmp45.tag == "NEGATIVE" then
    else
      _raise(_Match, "int-inf.sml:375:5")
    end
    do
      local words = tmp44.payload
      local tmp46 = _L[27](words, tmp45.payload)
      return {tag = "POSITIVE", payload = tmp46}
    end
  end
  log2Word = function(a)
    if a == 0x0 then
      _raise(Domain, "int-inf.sml:422:20")
    end
    if a == 0x1 then
      return 0
    end
    local tmp44 = log2Word(a >> 1)
    return _Int_add(1, tmp44)
  end
  LShiftAbs = function(words, amount)
    local tmp44, tmp45, tmp46, tmp47
    do
      tmp44 = _Word_div(amount, tmp24)
      tmp45 = _Word_mod(amount, tmp24)
      tmp46 = words.n
      local tmp48
      if tmp44 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp48 = tmp44
      end
      tmp47 = _Int_add(tmp46, tmp48)
      if tmp45 == 0x0 then
      else
        goto else1
      end
      do
        return _VectorOrArray_tabulate({tmp47, function(i)
          local tmp49
          if tmp44 < 0x0 then
            _raise(_Overflow, "word-1.sml:52:39")
          else
            tmp49 = tmp44
          end
          if i < tmp49 then
            return 0x0
          elseif tmp44 < 0x0 then
            _raise(_Overflow, "word-1.sml:52:39")
          else
            return words[_Int_sub(i, tmp44) + 1]
          end
        end})
      end
    end
    ::else1::
    local tmp48 = _Array_array(_Int_add(tmp47, 1), 0x0)
    local tmp49, tmp50 = 0x0, 0
    ::cont::
    do
      local lo, i = tmp49, tmp50
      if i == _Int_add(tmp46, 1) then
      else
        goto else2
      end
      do
        if lo == 0x0 then
        else
          goto else3
        end
        do
          return _L[24](tmp48)
        end
        ::else3::
        local tmp51 = _Fail("LShiftAbs: carry not zero")
        _raise(tmp51, "int-inf.sml:567:84")
      end
      ::else2::
      local w
      if i < tmp46 then
        w = words[i + 1]
      else
        w = 0x0
      end
      local tmp51 = w << tmp45 | lo
      local tmp52 = w >> tmp24 - tmp45
      if tmp44 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp48[_Int_add(i, tmp44) + 1] = tmp51
        tmp49 = tmp52
        tmp50 = _Int_add(i, 1)
        goto cont
      end
    end
  end
  local scanDigits = function(radix, isDigit1, getc1)
    return function(strm)
      local exp = getc1(strm)
      if exp.tag == "SOME" then
      elseif exp.tag == "NONE" then
        return NONE
      else
        _raise(_Match, "int-inf.sml:949:25")
      end
      do
        local c = exp.payload[1]
        local strm_PRIME = exp.payload[2]
        local tmp44 = isDigit1(c)
        if tmp44 then
        else
          return NONE
        end
        do
          local tmp45, tmp46
          do
            local tmp47 = _L[6](c)
            local tmp48 = _L[22](tmp47)
            tmp46, tmp45 = tmp48, strm_PRIME
          end
          ::cont::
          do
            local x, strm1 = tmp46, tmp45
            local exp1 = getc1(strm1)
            if exp1.tag == "SOME" then
            elseif exp1.tag == "NONE" then
              return {tag = "SOME", payload = {x, strm1}}
            else
              _raise(_Match, "int-inf.sml:943:35")
            end
            do
              local c1 = exp1.payload[1]
              local strm_PRIME1 = exp1.payload[2]
              local tmp47 = isDigit1(c1)
              if tmp47 then
              else
                return {tag = "SOME", payload = {x, strm1}}
              end
              do
                local tmp48 = mul(radix, x)
                local tmp49 = _L[6](c1)
                local tmp50 = _L[22](tmp49)
                local tmp51 = add(tmp48, tmp50)
                tmp46 = tmp51
                tmp45 = strm_PRIME1
                goto cont
              end
            end
          end
        end
      end
    end
  end
  local scanNegativeDigits = function(radix, isDigit1, getc1)
    return function(strm)
      local exp = getc1(strm)
      if exp.tag == "SOME" then
      elseif exp.tag == "NONE" then
        return NONE
      else
        _raise(_Match, "int-inf.sml:963:25")
      end
      do
        local c = exp.payload[1]
        local strm_PRIME = exp.payload[2]
        local tmp44 = isDigit1(c)
        if tmp44 then
        else
          return NONE
        end
        do
          local tmp45, tmp46
          do
            local tmp47 = _L[6](c)
            local tmp48 = _Int_negate(tmp47)
            local tmp49 = _L[22](tmp48)
            tmp46, tmp45 = tmp49, strm_PRIME
          end
          ::cont::
          do
            local x, strm1 = tmp46, tmp45
            local exp1 = getc1(strm1)
            if exp1.tag == "SOME" then
            elseif exp1.tag == "NONE" then
              return {tag = "SOME", payload = {x, strm1}}
            else
              _raise(_Match, "int-inf.sml:957:35")
            end
            do
              local c1 = exp1.payload[1]
              local strm_PRIME1 = exp1.payload[2]
              local tmp47 = isDigit1(c1)
              if tmp47 then
              else
                return {tag = "SOME", payload = {x, strm1}}
              end
              do
                local tmp48, tmp49
                do
                  tmp48 = mul(radix, x)
                  local tmp50 = _L[6](c1)
                  tmp49 = _L[22](tmp50)
                  if tmp49.tag == "ZERO" then
                    tmp46 = tmp48
                    tmp45 = strm_PRIME1
                    goto cont
                  end
                  if tmp48.tag == "ZERO" and tmp49.tag == "POSITIVE" then
                    tmp46 = {tag = "NEGATIVE", payload = tmp49.payload}
                    tmp45 = strm_PRIME1
                    goto cont
                  end
                  if tmp48.tag == "ZERO" and tmp49.tag == "NEGATIVE" then
                    tmp46 = {tag = "POSITIVE", payload = tmp49.payload}
                    tmp45 = strm_PRIME1
                    goto cont
                  end
                  if tmp48.tag == "POSITIVE" and tmp49.tag == "POSITIVE" then
                  else
                    goto else1
                  end
                  do
                    local words = tmp48.payload
                    local words_PRIME = tmp49.payload
                    local exp2 = _L[23](words, words_PRIME)
                    if exp2 == "LESS" then
                    else
                      goto else4
                    end
                    do
                      local tmp51 = _L[26](words_PRIME, words)
                      tmp46 = {tag = "NEGATIVE", payload = tmp51}
                      tmp45 = strm_PRIME1
                      goto cont
                    end
                    ::else4::
                    if exp2 == "GREATER" then
                    elseif exp2 == "EQUAL" then
                      tmp46 = _L[21]
                      tmp45 = strm_PRIME1
                      goto cont
                    else
                      _raise(_Match, "int-inf.sml:362:46")
                    end
                    do
                      local tmp51 = _L[26](words, words_PRIME)
                      tmp46 = {tag = "POSITIVE", payload = tmp51}
                      tmp45 = strm_PRIME1
                      goto cont
                    end
                  end
                end
                ::else1::
                if tmp48.tag == "POSITIVE" and tmp49.tag == "NEGATIVE" then
                else
                  goto else2
                end
                do
                  local tmp50 = _L[25](tmp48.payload, tmp49.payload)
                  tmp46 = {tag = "POSITIVE", payload = tmp50}
                  tmp45 = strm_PRIME1
                  goto cont
                end
                ::else2::
                if tmp48.tag == "NEGATIVE" and tmp49.tag == "POSITIVE" then
                else
                  goto else3
                end
                do
                  local tmp50 = _L[25](tmp48.payload, tmp49.payload)
                  tmp46 = {tag = "NEGATIVE", payload = tmp50}
                  tmp45 = strm_PRIME1
                  goto cont
                end
                ::else3::
                if tmp48.tag == "NEGATIVE" and tmp49.tag == "NEGATIVE" then
                else
                  _raise(_Match, "int-inf.sml:359:5")
                end
                do
                  local words = tmp48.payload
                  local words_PRIME = tmp49.payload
                  local exp2 = _L[23](words, words_PRIME)
                  if exp2 == "LESS" then
                  else
                    goto else4
                  end
                  do
                    local tmp50 = _L[26](words_PRIME, words)
                    tmp46 = {tag = "POSITIVE", payload = tmp50}
                    tmp45 = strm_PRIME1
                    goto cont
                  end
                  ::else4::
                  if exp2 == "GREATER" then
                  elseif exp2 == "EQUAL" then
                    tmp46 = _L[21]
                    tmp45 = strm_PRIME1
                    goto cont
                  else
                    _raise(_Match, "int-inf.sml:369:46")
                  end
                  do
                    local tmp50 = _L[26](words, words_PRIME)
                    tmp46 = {tag = "NEGATIVE", payload = tmp50}
                    tmp45 = strm_PRIME1
                    goto cont
                  end
                end
              end
            end
          end
        end
      end
    end
  end
  scan = function(a, a1)
    return function(a2)
      if a == "BIN" then
      else
        goto else1
      end
      do
        local strm
        do
          local strm1 = _L[3](a1, a2)
          local exp = _L[7](a1, strm1)
          local isNegative = exp[1]
          strm = exp[2]
          if isNegative then
          else
            goto else4
          end
          do
            local tmp44 = _L[22](2)
            local tmp45 = scanNegativeDigits(tmp44, _L[4], a1)
            return tmp45(strm)
          end
        end
        ::else4::
        local tmp44 = _L[22](2)
        local tmp45 = scanDigits(tmp44, _L[4], a1)
        return tmp45(strm)
      end
      ::else1::
      if a == "OCT" then
      else
        goto else2
      end
      do
        local strm
        do
          local strm1 = _L[3](a1, a2)
          local exp = _L[7](a1, strm1)
          local isNegative = exp[1]
          strm = exp[2]
          if isNegative then
          else
            goto else4
          end
          do
            local tmp44 = _L[22](8)
            local tmp45 = scanNegativeDigits(tmp44, _L[5], a1)
            return tmp45(strm)
          end
        end
        ::else4::
        local tmp44 = _L[22](8)
        local tmp45 = scanDigits(tmp44, _L[5], a1)
        return tmp45(strm)
      end
      ::else2::
      if a == "DEC" then
      else
        goto else3
      end
      do
        local strm
        do
          local strm1 = _L[3](a1, a2)
          local exp = _L[7](a1, strm1)
          local isNegative = exp[1]
          strm = exp[2]
          if isNegative then
          else
            goto else4
          end
          do
            local tmp44 = _L[22](10)
            local tmp45 = scanNegativeDigits(tmp44, isDigit, a1)
            return tmp45(strm)
          end
        end
        ::else4::
        local tmp44 = _L[22](10)
        local tmp45 = scanDigits(tmp44, isDigit, a1)
        return tmp45(strm)
      end
      ::else3::
      if a == "HEX" then
      else
        _raise(_Match, "int-inf.sml:971:5")
      end
      do
        local isNegative, strm
        do
          local strm1 = _L[3](a1, a2)
          local exp = _L[7](a1, strm1)
          isNegative = exp[1]
          local strm2 = exp[2]
          local exp1 = a1(strm2)
          local tmp44 = exp1.tag == "SOME" and exp1.payload[1] == 48
          do
            if tmp44 then
            else
              strm = strm2
              goto cont
            end
            do
              local exp2 = a1(exp1.payload[2])
              if exp2.tag == "SOME" then
              elseif exp2.tag == "NONE" then
                strm = strm2
                goto cont
              else
                _raise(_Match, "int-inf.sml:996:56")
              end
              do
                local c = exp2.payload[1]
                local strm_PRIME_PRIME = exp2.payload[2]
                if c == 120 or c == 88 then
                else
                  strm = strm2
                  goto cont
                end
                do
                  local exp3 = a1(strm_PRIME_PRIME)
                  if exp3.tag == "SOME" then
                  elseif exp3.tag == "NONE" then
                    strm = strm2
                    goto cont
                  else
                    _raise(_Match, "int-inf.sml:999:64")
                  end
                  do
                    local tmp45 = isHexDigit(exp3.payload[1])
                    if tmp45 then
                      strm = strm_PRIME_PRIME
                    else
                      strm = strm2
                    end
                  end
                end
              end
            end
          end
        end
        ::cont::
        if isNegative then
        else
          goto else4
        end
        do
          local tmp44 = _L[22](16)
          local tmp45 = scanNegativeDigits(tmp44, isHexDigit, a1)
          return tmp45(strm)
        end
        ::else4::
        local tmp44 = _L[22](16)
        local tmp45 = scanDigits(tmp44, isHexDigit, a1)
        return tmp45(strm)
      end
    end
  end
  do
    local tmp44, tmp45, tmp46
    do
      local tmp47 = 2.0
      tmp46, tmp45, tmp44 = tmp47, tmp24, 1.0
    end
    ::cont5::
    do
      local _L1 = {}
      local a, tmp47, acc = tmp46, tmp45, tmp44
      if tmp47 == 0 then
        tmp43 = acc
        goto cont1
      end
      if tmp47 == 1 then
        tmp43 = acc * a
        goto cont1
      end
      local tmp48 = tmp47 // 2
      if tmp47 % 2 == 0 then
        tmp46 = a * a
        tmp45 = tmp48
        tmp44 = acc
        goto cont5
      else
        local tmp49 = a * a
        tmp46 = tmp49
        tmp45 = tmp48
        tmp44 = acc * a
        goto cont5
      end
    end
  end
end
::cont1::
local toRealAbs, Word8Vector, Word8Array, Word8ArraySlice, UnsafeWord8Array, tmp44, tmp45, tmp46, tmp47, tmp48
do
  local function wordToReal(a)
    if a == 0x0 then
      return 0.0
    end
    local tmp49 = wordToReal(a >> 0x1F)
    local tmp50 = tmp49 * 0x1p31
    local tmp51 = a & 0x7FFFFFFF
    if tmp51 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      return tmp50 + tmp51 * 1.0
    end
  end
  local simpleNatToReal = function(a)
    local tmp49 = function(a1)
      local w = a1[1]
      local tmp50 = a1[2] * tmp43
      local tmp51 = wordToReal(w)
      return tmp50 + tmp51
    end
    local tmp50 = foldr(tmp49, 0.0)
    return tmp50(a)
  end
  toRealAbs = function(a)
    local k
    do
      local tmp49 = _Int_sub(a.n, 1)
      local tmp50 = _Int_mul(tmp24, tmp49)
      local tmp51 = log2Word(a[tmp49 + 1])
      k = _Int_add(tmp50, tmp51)
    end
    if k < 53 then
    else
      goto else1
    end
    do
      return simpleNatToReal(a)
    end
    ::else1::
    if k >= 1024 then
      return tmp11
    end
    local tmp49 = _Int_add(_Int_sub(k, 53), 1)
    local q
    do
      local tmp50, tmp51, tmp52
      do
        tmp50 = _Word_div(tmp49, tmp24)
        tmp51 = _Word_mod(tmp49, tmp24)
        local tmp53 = a.n
        local tmp54
        if tmp50 < 0x0 then
          _raise(_Overflow, "word-1.sml:52:39")
        else
          tmp54 = tmp50
        end
        tmp52 = _Int_sub(tmp53, tmp54)
        if tmp52 <= 0 then
          q = {n = 0}
          goto cont
        end
        if tmp51 == 0x0 then
        else
          goto else4
        end
        do
          local tmp55 = _VectorOrArray_tabulate({tmp52, function(i)
            if tmp50 < 0x0 then
              _raise(_Overflow, "word-1.sml:52:39")
            else
              return a[_Int_add(i, tmp50) + 1]
            end
          end})
          local tmp56 = _L[20](function(x)
            return x ~= 0x0
          end)
          if tmp50 < 0x0 then
            _raise(_Overflow, "word-1.sml:52:39")
          end
          local tmp57 = _L[19](a, 0, {tag = "SOME", payload = tmp50})
          tmp56(tmp57)
          q = tmp55
          goto cont
        end
      end
      ::else4::
      local tmp53 = _Array_array(tmp52, 0x0)
      local hasRemainder
      do
        local tmp54, tmp55 = 0x0, _Int_sub(tmp52, 1)
        ::cont2::
        do
          local hi, i = tmp54, tmp55
          if i < 0 then
            hasRemainder = hi ~= 0x0
            goto cont1
          end
          local tmp56
          if tmp50 < 0x0 then
            _raise(_Overflow, "word-1.sml:52:39")
          else
            tmp56 = tmp50
          end
          local tmp57 = a[_Int_add(i, tmp56) + 1]
          local tmp58 = tmp57 >> tmp51 | hi
          local tmp59 = tmp57 << tmp24 - tmp51
          tmp53[i + 1] = tmp58
          tmp54 = tmp59
          tmp55 = _Int_sub(i, 1)
          goto cont2
        end
      end
      ::cont1::
      do
        if hasRemainder then
        else
          goto else5
        end
        q = _L[24](tmp53)
        goto cont
        ::else5::
        local tmp54 = _L[20](function(x)
          return x ~= 0x0
        end)
        if tmp50 < 0x0 then
          _raise(_Overflow, "word-1.sml:52:39")
        end
        local tmp55 = _L[19](a, 0, {tag = "SOME", payload = tmp50})
        tmp54(tmp55)
      end
      q = _L[24](tmp53)
    end
    ::cont::
    local exp
    do
      local tmp50 = LShiftAbs({n = 1, 0x1}, tmp49)
      local tmp51 = _L[26](tmp50, {n = 1, 0x1})
      local r
      do
        local l, tmp52, tmp53
        do
          local tmp54 = a.n
          local tmp55 = tmp51.n
          if tmp54 < tmp55 then
            l = tmp54
          else
            l = tmp55
          end
          tmp52 = _Array_array(l, 0x0)
          tmp53 = 0
        end
        ::cont1::
        do
          local a1 = tmp53
          if a1 == l then
          else
            local tmp54 = a[a1 + 1]
            tmp52[a1 + 1] = tmp54 & tmp51[a1 + 1]
            tmp53 = _Int_add(a1, 1)
            goto cont1
          end
          r = _L[24](tmp52)
        end
      end
      local tmp52 = {n = 1, 0x1}
      local tmp53 = LShiftAbs(tmp52, _Int_sub(tmp49, 1))
      exp = _L[23](r, tmp53)
      if exp == "LESS" then
      else
        goto else2
      end
      do
        local tmp54 = simpleNatToReal(q)
        return fromManExp(tmp49, tmp54)
      end
    end
    ::else2::
    if exp == "EQUAL" then
    else
      goto else3
    end
    do
      if q[0 + 1] & 0x1 == 0x0 then
      else
        goto else4
      end
      do
        local tmp50 = simpleNatToReal(q)
        return fromManExp(tmp49, tmp50)
      end
      ::else4::
      local tmp50 = _L[25](q, {n = 1, 0x1})
      local tmp51 = simpleNatToReal(tmp50)
      return fromManExp(tmp49, tmp51)
    end
    ::else3::
    if exp == "GREATER" then
    else
      _raise(_Match, "int-inf.sml:1073:33")
    end
    do
      local tmp50 = _L[25](q, {n = 1, 0x1})
      local tmp51 = simpleNatToReal(tmp50)
      return fromManExp(tmp49, tmp51)
    end
  end
  local unsafeSub = function(a)
    local v = a[1]
    local tmp49 = sub1({v, a[2]})
    return tmp49 & 0xFF
  end
  local fromList = function(a)
    local tmp49 = map(function(a1)
      if a1 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      elseif a1 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif a1 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return a1
      end
    end)
    local tmp50 = tmp49(a)
    return implode(tmp50)
  end
  local unsafeFromListN = function(a)
    return fromList(a[2])
  end
  _L[28] = function(a)
    local xs = a[2]
    local tmp49 = map(function(a1)
      if a1 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      elseif a1 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif a1 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        return a1
      end
    end)
    local tmp50 = tmp49(xs)
    return implodeRev(tmp50)
  end
  _L[29] = function(a)
    local base = a.base
    local start = a.start
    return substring(base, start, a.length)
  end
  _L[30] = function(a)
    return _Array_array(a, 0x0)
  end
  _L[31] = MonoSequence(_VectorOrArray_fromList, length, math_maxinteger, array, _L[30], function(a)
    return _VectorOrArray_fromList(a[2])
  end, sub, update, concat, fromList, size, math_maxinteger, _L[29], _L[29], unsafeFromListN, _L[28], unsafeSub)
  Word8Vector = _L[31]._MonoVector
  _L[32] = _L[31]._MonoVectorSlice
  Word8Array = _L[31]._MonoArray
  Word8ArraySlice = _L[31]._MonoArraySlice
  UnsafeWord8Array = _L[31]._UnsafeMonoArray
  tmp44 = _L[32].concat
  tmp45 = _L[32].full
  tmp46 = _L[32].length
  tmp47 = _L[32].slice
  tmp48 = _L[32].vector
end
local tmp49 = Word8Vector.concat
local tmp50 = Word8Vector.fromList
local tmp51 = Word8Vector.length
local tmp52 = Word8Vector.sub
local tmp53 = Word8Vector.tabulate
_L[33] = Word8ArraySlice.base
_L[34] = Word8ArraySlice.copyVec
_L[35] = Word8ArraySlice.full
_L[36] = Word8ArraySlice.length
_L[37] = Word8ArraySlice.slice
_L[38] = Word8ArraySlice.vector
_L[39] = Word8Array.array
_L[40] = Word8Array.copyVec
_L[41] = Word8Array.length
_L[42] = Word8Array.maxLen
_L[43] = Word8Array.sub
_L[44] = Word8Array.tabulate
_L[45] = Word8Array.update
_L[46] = UnsafeWord8Array.sub
_L[47] = UnsafeWord8Array.update
_L[48] = function(a, i, bytesPerElem)
  if i < 0 then
    _raise(_Subscript, "pack-word.sml:29:48")
  end
  local tmp54 = _Int_mul(bytesPerElem, i)
  local tmp55 = _L[41](a)
  if tmp55 < _Int_add(tmp54, bytesPerElem) then
    _raise(_Subscript, "pack-word.sml:33:55")
  else
    return tmp54
  end
end
_L[49] = function(a)
  return function(a1)
    local tmp54 = #a
    local tmp55 = _Int_sub(#a1, tmp54)
    local tmp56 = 0
    ::cont::
    do
      local a2 = tmp56
      if a2 > tmp55 then
        return false
      end
      local tmp57
      do
        local tmp58 = 0
        ::cont2::
        do
          local a3 = tmp58
          if a3 >= tmp54 then
            tmp57 = true
            goto cont1
          end
          local tmp59 = tmp26({a, a3})
          local tmp60 = tmp26({a1, _Int_add(a2, a3)})
          if tmp59 == tmp60 then
            tmp58 = _Int_add(a3, 1)
            goto cont2
          else
            tmp57 = false
          end
        end
      end
      ::cont1::
      if tmp57 then
        return true
      else
        tmp56 = _Int_add(a2, 1)
        goto cont
      end
    end
  end
end
_L[50] = {}
_L[50].year = 2001
_L[50].month = 1
_L[50].day = 1
_L[50].hour = 0
_L[51] = tmp18(_L[50])
_L[52] = {tag = {"Time"}}
_L[53] = function(a)
  local tmp54 = tmp13(a, _L[51])
  local status, exn = _handle(function()
    local tmp55 = tmp54 * 1e6
    local tmp56 = tmp12(tmp55)
    if math_type(tmp56) == "integer" then
      return tmp56
    elseif tmp55 ~= tmp55 then
      _raise(Domain, "real-1.sml:301:26")
    else
      _raise(_Overflow, "real-1.sml:303:26")
    end
  end)
  if not status then
    _raise(_L[52], "time.sml:79:82")
  else
    return exn
  end
end
_L[54] = {}
_L[55] = {}
_L[55].__mode = "k"
setmetatable(_L[54], _L[55])
_L[56] = {}
_L[57] = {}
_L[57].__mode = "v"
setmetatable(_L[56], _L[57])
_L[58] = {{0, nil}}
_L[59] = function(a)
  local tmp54 = _L[54][a]
  if tmp54 == nil then
    local d
    do
      local x = _L[58][1]
      if x[2] == nil then
        local n = x[1]
        _L[58][1] = {_Int_add(n, 1), nil}
        d = n
      elseif x[2] ~= nil then
        local n = x[1]
        local tmp55 = x[2][1]
        _L[58][1] = {n, x[2][2]}
        d = tmp55
      else
        _raise(_Match, "iodesc.sml:29:24")
      end
    end
    _L[54][a] = d
    _L[56][d] = a
    return d
  else
    return tmp54
  end
end
_L[60] = function(a)
  local tmp54 = _L[56][a]
  if tmp54 ~= nil then
    local x = _L[58][1]
    local n = x[1]
    _L[58][1] = {n, {a, x[2]}}
    _L[56][a] = nil
    _L[54][tmp54] = nil
    return nil
  else
    return nil
  end
end
_L[61] = {"SysErr"}
_L[62] = function(field, f)
  if tmp23.tag == "SOME" then
  elseif tmp23.tag == "NONE" then
    return function(a)
      _raise({tag = _L[61], payload = {"LuaFileSystem not available", NONE}}, "os.sml:56:70")
    end
  else
    _raise(_Match, "os.sml:54:50")
  end
  do
    return f(tmp23.payload[field])
  end
end
_L[63] = _L[62]("chdir", function(lfs_chdir)
  return function(path)
    local tmp54, tmp55 = lfs_chdir(path)
    if not tmp54 then
      local tmp56 = tmp6(tmp55)
      if tmp56 == "string" then
        _raise({tag = _L[61], payload = {tmp55, {tag = "SOME", payload = tmp55}}}, "os.sml:61:98")
      else
        _raise({tag = TypeError_tag, payload = "expected a string, but got " .. tmp56}, "lua.sml:467:37")
      end
    else
      return nil
    end
  end
end)
_L[64] = _L[62]("currentdir", function(lfs_currentdir)
  return function(a)
    local tmp54, tmp55 = lfs_currentdir()
    if tmp6(tmp54) == "string" then
      return tmp54
    end
    local tmp56 = tmp6(tmp55)
    if tmp56 == "string" then
      _raise({tag = _L[61], payload = {tmp55, {tag = "SOME", payload = tmp55}}}, "os.sml:73:102")
    else
      _raise({tag = TypeError_tag, payload = "expected a string, but got " .. tmp56}, "lua.sml:467:37")
    end
  end
end)
_L[65] = _L[62]("mkdir", function(lfs_mkdir)
  return function(path)
    local tmp54, tmp55 = lfs_mkdir(path)
    if not tmp54 then
      local tmp56 = tmp6(tmp55)
      if tmp56 == "string" then
        _raise({tag = _L[61], payload = {tmp55, {tag = "SOME", payload = tmp55}}}, "os.sml:81:98")
      else
        _raise({tag = TypeError_tag, payload = "expected a string, but got " .. tmp56}, "lua.sml:467:37")
      end
    else
      return nil
    end
  end
end)
_L[66] = _L[62]("attributes", function(lfs_attributes)
  return function(path)
    local tmp54, tmp55 = lfs_attributes(path, "modification")
    if not tmp54 then
      local tmp56 = tmp6(tmp55)
      if tmp56 == "string" then
        _raise({tag = _L[61], payload = {tmp55, {tag = "SOME", payload = tmp55}}}, "os.sml:132:103")
      else
        _raise({tag = TypeError_tag, payload = "expected a string, but got " .. tmp56}, "lua.sml:467:37")
      end
    else
      return _L[53](tmp54)
    end
  end
end)
_L[67] = _L[62]("attributes", function(lfs_attributes)
  return function(path)
    local tmp54, tmp55 = lfs_attributes(path, "size")
    if not tmp54 then
      local tmp56 = tmp6(tmp55)
      if tmp56 == "string" then
        _raise({tag = _L[61], payload = {tmp55, {tag = "SOME", payload = tmp55}}}, "os.sml:142:114")
      else
        _raise({tag = TypeError_tag, payload = "expected a string, but got " .. tmp56}, "lua.sml:467:37")
      end
    else
      return tmp54
    end
  end
end)
_L[68] = {tag = {"Path"}}
_L[69] = {tag = {"InvalidArc"}}
_L[70] = function(a)
  local tmp54 = tmp32(function(c)
    return c ~= 47
  end)
  return tmp54(a)
end
_L[71] = function(a)
  local dir = a.dir
  local file = a.file
  local arcs, isAbs, vol
  do
    local tmp54 = fields(function(c)
      return c == 47
    end)
    local exp = tmp54(dir)
    if exp ~= nil and (exp[1] == "" and exp[2] == nil) then
      vol = ""
      isAbs = false
      arcs = nil
    elseif exp ~= nil and exp[1] == "" then
      vol = ""
      isAbs = true
      arcs = exp[2]
    else
      vol = ""
      isAbs = false
      arcs = exp
    end
  end
  local tmp54 = {file, nil}
  local tmp55
  do
    local tmp56 = revAppend(arcs, nil)
    tmp55 = revAppend(tmp56, tmp54)
  end
  if vol ~= "" then
    _raise(_L[68], "os-path-unix.sml:24:41")
  end
  if not isAbs and (tmp55 ~= nil and tmp55[1] == "") then
    _raise(_L[68], "os-path-unix.sml:27:65")
  end
  local tmp56 = all(_L[70])
  local tmp57 = tmp56(tmp55)
  if tmp57 then
    if isAbs then
      return "/" .. table_concat(_VectorOrArray_fromList(tmp55), "/")
    else
      return table_concat(_VectorOrArray_fromList(tmp55), "/")
    end
  else
    _raise(_L[69], "os-path-unix.sml:34:54")
  end
end
_L[72] = function(a)
  local tmp54
  do
    if #a >= 5 then
    else
      tmp54 = false
      goto cont
    end
    do
      local tmp55 = sub1({a, 0})
      if tmp55 == 47 or tmp55 == 92 then
      else
        tmp54 = false
        goto cont
      end
      do
        local tmp56 = sub1({a, 1})
        if tmp56 == 47 or tmp56 == 92 then
        else
          tmp54 = false
          goto cont
        end
        do
          local tmp57 = sub1({a, 2})
          tmp54 = tmp57 ~= 47 and tmp57 ~= 92
        end
      end
    end
  end
  ::cont::
  if tmp54 then
  else
    return NONE
  end
  do
    local tmp55 = 3
    ::cont1::
    do
      local a1 = tmp55
      if #a <= a1 then
        return NONE
      end
      local tmp56 = sub1({a, a1})
      if tmp56 == 47 or tmp56 == 92 then
      else
        tmp55 = _Int_add(a1, 1)
        goto cont1
      end
      do
        local tmp57 = _Int_add(a1, 1)
        local tmp58
        do
          if #a > tmp57 then
          else
            tmp58 = false
            goto cont2
          end
          do
            local tmp59 = sub1({a, tmp57})
            tmp58 = tmp59 ~= 47 and tmp59 ~= 92
          end
        end
        ::cont2::
        if tmp58 then
        else
          return NONE
        end
        do
          local tmp59 = _Int_add(tmp57, 1)
          ::cont3::
          do
            local a2 = tmp59
            if #a <= a2 then
              return {tag = "SOME", payload = a2}
            end
            local tmp60 = sub1({a, a2})
            if tmp60 == 47 or tmp60 == 92 then
              return {tag = "SOME", payload = a2}
            else
              tmp59 = _Int_add(a2, 1)
              goto cont3
            end
          end
        end
      end
    end
  end
end
_L[73] = {tag = "DOS_VOLUME"}
_L[74] = {tag = "NO_VOLUME"}
_L[75] = function(a)
  local tmp54 = tmp32(function(c)
    return c ~= 47 and c ~= 92
  end)
  return tmp54(a)
end
_L[76] = function(a)
  local vol, isAbs, tmp54, tmp55
  do
    local dir = a.dir
    local file = a.file
    local arcs
    do
      local exp
      do
        local tmp56
        do
          if #dir >= 2 then
          else
            tmp56 = false
            goto cont3
          end
          do
            local tmp57 = sub1({dir, 1})
            if tmp57 == 58 then
            else
              tmp56 = false
              goto cont3
            end
            do
              local tmp58 = sub1({dir, 0})
              tmp56 = 65 <= tmp58 and tmp58 <= 90 or 97 <= tmp58 and tmp58 <= 122
            end
          end
        end
        ::cont3::
        if tmp56 then
          exp = _L[73]
          goto cont1
        end
        local exp1 = _L[72](dir)
        if exp1.tag == "SOME" then
          exp = {tag = "UNC_PATH", payload = exp1.payload}
        elseif exp1.tag == "NONE" then
          exp = _L[74]
        else
          _raise(_Match, "os-path-windows.sml:44:27")
        end
      end
      ::cont1::
      local isUNC, vol1, rest
      do
        if exp.tag == "DOS_VOLUME" then
        else
          goto else1
        end
        do
          local tmp56 = substring(dir, 0, 2)
          local tmp57 = extract(dir, 2, NONE)
          rest = tmp57
          vol1 = tmp56
          isUNC = false
          goto cont2
        end
        ::else1::
        if exp.tag == "UNC_PATH" then
        elseif exp.tag == "NO_VOLUME" then
          rest = dir
          vol1 = ""
          isUNC = false
          goto cont2
        else
          _raise(_Match, "os-path-windows.sml:75:52")
        end
        do
          local i = exp.payload
          local tmp56 = substring(dir, 0, i)
          local tmp57 = extract(dir, i, NONE)
          rest = tmp57
          vol1 = tmp56
          isUNC = true
        end
      end
      ::cont2::
      local tmp56 = fields(function(c)
        return c == 47 or c == 92
      end)
      local exp1 = tmp56(rest)
      if exp1 ~= nil and (exp1[1] == "" and exp1[2] == nil) then
        vol = vol1
        isAbs = isUNC
        arcs = nil
      elseif exp1 ~= nil and exp1[1] == "" then
        vol = vol1
        isAbs = true
        arcs = exp1[2]
      else
        vol = vol1
        isAbs = isUNC
        arcs = exp1
      end
    end
    local tmp56 = {file, nil}
    do
      local tmp57 = revAppend(arcs, nil)
      tmp54 = revAppend(tmp57, tmp56)
    end
    do
      if not isAbs and vol == "" then
        tmp55 = true
        goto cont
      end
      local tmp57
      do
        if #vol == 2 then
        else
          tmp57 = false
          goto cont1
        end
        do
          local tmp58 = sub1({vol, 0})
          local tmp59
          tmp59 = 65 <= tmp58 and tmp58 <= 90 or 97 <= tmp58 and tmp58 <= 122
          if tmp59 then
          else
            tmp57 = false
            goto cont1
          end
          do
            local tmp60 = sub1({vol, 1})
            tmp57 = tmp60 == 58
          end
        end
      end
      ::cont1::
      if tmp57 then
        tmp55 = true
        goto cont
      end
      if isAbs then
      else
        tmp55 = false
        goto cont
      end
      do
        local exp = _L[72](vol)
        if exp.tag == "SOME" then
          local i = exp.payload
          tmp55 = i == #vol
        elseif exp.tag == "NONE" then
          tmp55 = false
        else
          _raise(_Match, "os-path-windows.sml:73:56")
        end
      end
    end
  end
  ::cont::
  if not tmp55 then
    _raise(_L[68], "os-path-windows.sml:87:41")
  end
  if not isAbs and #vol >= 3 then
    _raise(_L[68], "os-path-windows.sml:89:41")
  end
  if not isAbs and (tmp54 ~= nil and tmp54[1] == "") then
    _raise(_L[68], "os-path-windows.sml:92:65")
  end
  local tmp56 = all(_L[75])
  local tmp57 = tmp56(tmp54)
  if tmp57 then
    if isAbs then
      local tmp58 = vol .. "\\"
      return tmp58 .. table_concat(_VectorOrArray_fromList(tmp54), "\\")
    else
      return vol .. table_concat(_VectorOrArray_fromList(tmp54), "\\")
    end
  else
    _raise(_L[69], "os-path-windows.sml:99:54")
  end
end
do
  _L[78] = sub1({tmp2.config, 0})
  if _L[78] == 92 then
    _L[77] = _L[76]
  else
    _L[77] = _L[71]
  end
end
_L[79] = function(tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, compare1, tmp68)
  local RD = function(payload)
    return payload
  end
  local WR = function(payload)
    return payload
  end
  local openVector = function(a)
    local tmp69 = {0}
    local tmp70 = {false}
    local readVec = function(a1)
      if a1 < 0 then
        _raise(_Size, "prim-io.sml:112:31")
      end
      local x = tmp69[1]
      local total = tmp63(a)
      local tmp71 = _Int_add(x, a1)
      local newPos
      if tmp71 < total then
        newPos = tmp71
      else
        newPos = total
      end
      tmp69[1] = newPos
      local tmp72 = tmp66({a, x, {tag = "SOME", payload = _Int_sub(newPos, x)}})
      return tmp67(tmp72)
    end
    local readArr = function(a1)
      local x = tmp69[1]
      local total = tmp63(a)
      local tmp71 = tmp59(a1)
      local tmp72 = _Int_add(x, tmp71)
      local newPos
      if tmp72 < total then
        newPos = tmp72
      else
        newPos = total
      end
      local exp = tmp56(a1)
      local baseArr = exp[1]
      local start = exp[2]
      local tmp73 = _Int_sub(newPos, x)
      tmp69[1] = newPos
      local tmp74 = tmp66({a, x, {tag = "SOME", payload = tmp73}})
      tmp57({di = start, dst = baseArr, src = tmp74})
      return tmp73
    end
    local tmp71 = tmp63(a)
    local tmp72 = {tag = "SOME", payload = function(n)
      if tmp70[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readVec", name = "<openVector>"}}, "prim-io.sml:108:36")
      else
        return readVec(n)
      end
    end}
    local tmp73 = {tag = "SOME", payload = function(slice)
      if tmp70[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readArr", name = "<openVector>"}}, "prim-io.sml:108:36")
      else
        return readArr(slice)
      end
    end}
    local tmp74 = {tag = "SOME", payload = function(n)
      if tmp70[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readVecNB", name = "<openVector>"}}, "prim-io.sml:108:36")
      end
      local tmp75 = readVec(n)
      return {tag = "SOME", payload = tmp75}
    end}
    local tmp75 = {tag = "SOME", payload = function(slice)
      if tmp70[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readArrNB", name = "<openVector>"}}, "prim-io.sml:108:36")
      end
      local tmp76 = readArr(slice)
      return {tag = "SOME", payload = tmp76}
    end}
    local tmp76 = {tag = "SOME", payload = function(a1)
      if tmp70[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "block", name = "<openVector>"}}, "prim-io.sml:108:36")
      else
        return nil
      end
    end}
    local tmp77 = {tag = "SOME", payload = function(a1)
      if tmp70[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "canInput", name = "<openVector>"}}, "prim-io.sml:108:36")
      else
        return true
      end
    end}
    local tmp78 = function(a1)
      if tmp70[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "avail", name = "<openVector>"}}, "prim-io.sml:108:36")
      end
      local tmp79 = tmp63(a)
      return {tag = "SOME", payload = _Int_sub(tmp79, tmp69[1])}
    end
    return {avail = tmp78, block = tmp76, canInput = tmp77, chunkSize = tmp71, close = function(a1)
      tmp70[1] = true
      return nil
    end, endPos = NONE, getPos = NONE, ioDesc = NONE, name = "<openVector>", readArr = tmp73, readArrNB = tmp75, readVec = tmp72, readVecNB = tmp74, setPos = NONE, verifyPos = NONE}
  end
  local nullRd = function(a)
    local tmp69 = {false}
    local empty = tmp62(nil)
    local tmp70 = {tag = "SOME", payload = function(n)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readVec", name = "<nullRd>"}}, "prim-io.sml:149:46")
      elseif n < 0 then
        _raise(_Size, "prim-io.sml:155:81")
      else
        return empty
      end
    end}
    local tmp71 = {tag = "SOME", payload = function(slice)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readArr", name = "<nullRd>"}}, "prim-io.sml:149:46")
      else
        return 0
      end
    end}
    local tmp72 = {tag = "SOME", payload = function(n)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readVecNB", name = "<nullRd>"}}, "prim-io.sml:149:46")
      else
        return {tag = "SOME", payload = empty}
      end
    end}
    local tmp73 = {tag = "SOME", payload = function(slice)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "readArrNB", name = "<nullRd>"}}, "prim-io.sml:149:46")
      else
        return {tag = "SOME", payload = 0}
      end
    end}
    local tmp74 = {tag = "SOME", payload = function(a1)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "block", name = "<nullRd>"}}, "prim-io.sml:149:46")
      else
        return nil
      end
    end}
    local tmp75 = {tag = "SOME", payload = function(a1)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "canInput", name = "<nullRd>"}}, "prim-io.sml:149:46")
      else
        return true
      end
    end}
    local tmp76 = function(a1)
      return {tag = "SOME", payload = 0}
    end
    return {avail = tmp76, block = tmp74, canInput = tmp75, chunkSize = 1, close = function(a1)
      tmp69[1] = true
      return nil
    end, endPos = NONE, getPos = NONE, ioDesc = NONE, name = "<nullRd>", readArr = tmp71, readArrNB = tmp73, readVec = tmp70, readVecNB = tmp72, setPos = NONE, verifyPos = NONE}
  end
  local nullWr = function(a)
    local tmp69 = {false}
    local tmp70 = {tag = "SOME", payload = function(slice)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "writeVec", name = "<nullWr>"}}, "prim-io.sml:173:46")
      else
        return tmp65(slice)
      end
    end}
    local tmp71 = {tag = "SOME", payload = function(slice)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "writeArr", name = "<nullWr>"}}, "prim-io.sml:173:46")
      else
        return tmp59(slice)
      end
    end}
    local tmp72 = {tag = "SOME", payload = function(slice)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "writeVecNB", name = "<nullWr>"}}, "prim-io.sml:173:46")
      end
      local tmp73 = tmp65(slice)
      return {tag = "SOME", payload = tmp73}
    end}
    local tmp73 = {tag = "SOME", payload = function(slice)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "writeArrNB", name = "<nullWr>"}}, "prim-io.sml:173:46")
      end
      local tmp74 = tmp59(slice)
      return {tag = "SOME", payload = tmp74}
    end}
    local tmp74 = {tag = "SOME", payload = function(a1)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "block", name = "<nullWr>"}}, "prim-io.sml:173:46")
      else
        return nil
      end
    end}
    local tmp75 = {tag = "SOME", payload = function(a1)
      if tmp69[1] then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "canOutput", name = "<nullWr>"}}, "prim-io.sml:173:46")
      else
        return true
      end
    end}
    return {block = tmp74, canOutput = tmp75, chunkSize = 1, close = function(a1)
      tmp69[1] = true
      return nil
    end, endPos = NONE, getPos = NONE, ioDesc = NONE, name = "<nullWr>", setPos = NONE, verifyPos = NONE, writeArr = tmp71, writeArrNB = tmp73, writeVec = tmp70, writeVecNB = tmp72}
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
      local empty = tmp62(nil)
      if readVec.tag == "SOME" then
        readVec_PRIME = readVec
        goto cont
      end
      if readVec.tag == "NONE" then
        if readArr.tag == "SOME" then
          local ra = readArr.payload
          readVec_PRIME = {tag = "SOME", payload = function(n)
            local arr = tmp54({n, tmp68})
            local tmp69 = tmp58(arr)
            local actual = ra(tmp69)
            local tmp70 = tmp60({arr, 0, {tag = "SOME", payload = actual}})
            return tmp61(tmp70)
          end}
          goto cont
        end
        if readArr.tag == "NONE" then
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
              end
              if exp.tag == "NONE" then
              else
                _raise(_Match, "prim-io.sml:207:72")
              end
              do
                block_PRIME(nil)
                local exp1 = rvNB(n)
                if exp1.tag == "SOME" then
                  return exp1.payload
                elseif exp1.tag == "NONE" then
                  return empty
                else
                  _raise(_Match, "prim-io.sml:210:86")
                end
              end
            end}
          elseif block.tag == "SOME" and (readVecNB.tag == "NONE" and readArrNB.tag == "SOME") then
            local block_PRIME = block.payload
            local raNB = readArrNB.payload
            readVec_PRIME = {tag = "SOME", payload = function(n)
              if n < 0 then
                _raise(_Size, "prim-io.sml:217:72")
              end
              local arr = tmp54({n, tmp68})
              local aslice = tmp58(arr)
              local exp = raNB(aslice)
              if exp.tag == "SOME" then
              else
                goto else1
              end
              do
                local tmp69 = tmp60({arr, 0, {tag = "SOME", payload = exp.payload}})
                return tmp61(tmp69)
              end
              ::else1::
              if exp.tag == "NONE" then
              else
                _raise(_Match, "prim-io.sml:221:75")
              end
              do
                block_PRIME(nil)
                local exp1 = raNB(aslice)
                if exp1.tag == "SOME" then
                elseif exp1.tag == "NONE" then
                  return empty
                else
                  _raise(_Match, "prim-io.sml:224:89")
                end
                do
                  local tmp69 = tmp60({arr, 0, {tag = "SOME", payload = exp1.payload}})
                  return tmp61(tmp69)
                end
              end
            end}
          else
            readVec_PRIME = NONE
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
    end
    if readArr.tag == "NONE" then
      if readVec.tag == "SOME" then
        local rv = readVec.payload
        readArr_PRIME = {tag = "SOME", payload = function(slice)
          local tmp69 = tmp59(slice)
          local v = rv(tmp69)
          local exp = tmp56(slice)
          tmp55({di = exp[2], dst = exp[1], src = v})
          return tmp63(v)
        end}
        goto cont1
      end
      if readVec.tag == "NONE" then
        if block.tag == "SOME" and readArrNB.tag == "SOME" then
          local block_PRIME = block.payload
          local raNB = readArrNB.payload
          readArr_PRIME = {tag = "SOME", payload = function(slice)
            local exp = raNB(slice)
            if exp.tag == "SOME" then
              return exp.payload
            end
            if exp.tag == "NONE" then
            else
              _raise(_Match, "prim-io.sml:242:72")
            end
            do
              block_PRIME(nil)
              local exp1 = raNB(slice)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:245:86")
              end
            end
          end}
        elseif block.tag == "SOME" and (readVecNB.tag == "SOME" and readArrNB.tag == "NONE") then
          local block_PRIME = block.payload
          local rvNB = readVecNB.payload
          readArr_PRIME = {tag = "SOME", payload = function(slice)
            local n, base, start, exp
            do
              n = tmp59(slice)
              local exp1 = tmp56(slice)
              base = exp1[1]
              start = exp1[2]
              exp = rvNB(n)
              if exp.tag == "SOME" then
              else
                goto else1
              end
              do
                local v = exp.payload
                tmp55({di = start, dst = base, src = v})
                return tmp63(v)
              end
            end
            ::else1::
            if exp.tag == "NONE" then
            else
              _raise(_Match, "prim-io.sml:253:75")
            end
            do
              block_PRIME(nil)
              local exp1 = rvNB(n)
              if exp1.tag == "SOME" then
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:258:89")
              end
              do
                local v = exp1.payload
                tmp55({di = start, dst = base, src = v})
                return tmp63(v)
              end
            end
          end}
        else
          readArr_PRIME = NONE
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
    end
    if readVecNB.tag == "NONE" then
      if readArrNB.tag == "SOME" then
        local raNB = readArrNB.payload
        readVecNB_PRIME = {tag = "SOME", payload = function(n)
          if n < 0 then
            _raise(_Size, "prim-io.sml:271:75")
          end
          local arr = tmp54({n, tmp68})
          local tmp69 = tmp58(arr)
          local exp = raNB(tmp69)
          if exp.tag == "SOME" then
          elseif exp.tag == "NONE" then
            return NONE
          else
            _raise(_Match, "prim-io.sml:274:78")
          end
          do
            local tmp70 = tmp60({arr, 0, {tag = "SOME", payload = exp.payload}})
            local tmp71 = tmp61(tmp70)
            return {tag = "SOME", payload = tmp71}
          end
        end}
        goto cont2
      end
      if readArrNB.tag == "NONE" then
        if canInput.tag == "SOME" and readVec.tag == "SOME" then
          local canInput_PRIME = canInput.payload
          local rv = readVec.payload
          readVecNB_PRIME = {tag = "SOME", payload = function(n)
            local tmp69 = canInput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              local tmp70 = rv(n)
              return {tag = "SOME", payload = tmp70}
            end
          end}
        elseif canInput.tag == "SOME" and (readVec.tag == "NONE" and readArr.tag == "SOME") then
          local canInput_PRIME = canInput.payload
          local ra = readArr.payload
          readVecNB_PRIME = {tag = "SOME", payload = function(n)
            local tmp69 = canInput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              if n < 0 then
                _raise(_Size, "prim-io.sml:289:78")
              end
              local arr = tmp54({n, tmp68})
              local tmp70 = tmp58(arr)
              local actual = ra(tmp70)
              local tmp71 = tmp60({arr, 0, {tag = "SOME", payload = actual}})
              local tmp72 = tmp61(tmp71)
              return {tag = "SOME", payload = tmp72}
            end
          end}
        else
          readVecNB_PRIME = NONE
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
    end
    if readArrNB.tag == "NONE" then
      if readVecNB.tag == "SOME" then
        local rvNB = readVecNB.payload
        readArrNB_PRIME = {tag = "SOME", payload = function(slice)
          local tmp69 = tmp59(slice)
          local exp = rvNB(tmp69)
          if exp.tag == "SOME" then
          elseif exp.tag == "NONE" then
            return NONE
          else
            _raise(_Match, "prim-io.sml:303:62")
          end
          do
            local v = exp.payload
            local exp1 = tmp56(slice)
            tmp55({di = exp1[2], dst = exp1[1], src = v})
            local tmp70 = tmp63(v)
            return {tag = "SOME", payload = tmp70}
          end
        end}
        goto cont3
      end
      if readVecNB.tag == "NONE" then
        if canInput.tag == "SOME" and readArr.tag == "SOME" then
          local canInput_PRIME = canInput.payload
          local ra = readArr.payload
          readArrNB_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = canInput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              local tmp70 = ra(slice)
              return {tag = "SOME", payload = tmp70}
            end
          end}
        elseif canInput.tag == "SOME" and (readVec.tag == "SOME" and readArr.tag == "NONE") then
          local canInput_PRIME = canInput.payload
          local rv = readVec.payload
          readArrNB_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = canInput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              local tmp70 = tmp59(slice)
              local v = rv(tmp70)
              local exp = tmp56(slice)
              tmp55({di = exp[2], dst = exp[1], src = v})
              local tmp71 = tmp63(v)
              return {tag = "SOME", payload = tmp71}
            end
          end}
        else
          readArrNB_PRIME = NONE
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
    end
    if writeVec.tag == "NONE" then
      if writeArr.tag == "SOME" then
        local wa = writeArr.payload
        writeVec_PRIME = {tag = "SOME", payload = function(slice)
          local tmp69 = tmp65(slice)
          local arr = tmp54({tmp69, tmp68})
          tmp57({di = 0, dst = arr, src = slice})
          local tmp70 = tmp58(arr)
          return wa(tmp70)
        end}
        goto cont
      end
      if writeArr.tag == "NONE" then
        if block.tag == "SOME" and writeVecNB.tag == "SOME" then
          local block_PRIME = block.payload
          local wvNB = writeVecNB.payload
          writeVec_PRIME = {tag = "SOME", payload = function(slice)
            local exp = wvNB(slice)
            if exp.tag == "SOME" then
              return exp.payload
            end
            if exp.tag == "NONE" then
            else
              _raise(_Match, "prim-io.sml:359:64")
            end
            do
              block_PRIME(nil)
              local exp1 = wvNB(slice)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:362:78")
              end
            end
          end}
        elseif block.tag == "SOME" and (writeVecNB.tag == "NONE" and writeArrNB.tag == "SOME") then
          local block_PRIME = block.payload
          local waNB = writeArrNB.payload
          writeVec_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = tmp65(slice)
            local arr = tmp54({tmp69, tmp68})
            local aslice = tmp58(arr)
            tmp57({di = 0, dst = arr, src = slice})
            local exp = waNB(aslice)
            if exp.tag == "SOME" then
              return exp.payload
            end
            if exp.tag == "NONE" then
            else
              _raise(_Match, "prim-io.sml:372:67")
            end
            do
              block_PRIME(nil)
              local exp1 = waNB(aslice)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:375:81")
              end
            end
          end}
        else
          writeVec_PRIME = NONE
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
    end
    if writeArr.tag == "NONE" then
      if writeVec.tag == "SOME" then
        local wv = writeVec.payload
        writeArr_PRIME = {tag = "SOME", payload = function(slice)
          local v = tmp61(slice)
          local tmp69 = tmp64(v)
          return wv(tmp69)
        end}
        goto cont1
      end
      if writeVec.tag == "NONE" then
        if block.tag == "SOME" and writeArrNB.tag == "SOME" then
          local block_PRIME = block.payload
          local waNB = writeArrNB.payload
          writeArr_PRIME = {tag = "SOME", payload = function(slice)
            local exp = waNB(slice)
            if exp.tag == "SOME" then
              return exp.payload
            end
            if exp.tag == "NONE" then
            else
              _raise(_Match, "prim-io.sml:393:64")
            end
            do
              block_PRIME(nil)
              local exp1 = waNB(slice)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:396:78")
              end
            end
          end}
        elseif block.tag == "SOME" and (writeVecNB.tag == "SOME" and writeArrNB.tag == "NONE") then
          local block_PRIME = block.payload
          local wvNB = writeVecNB.payload
          writeArr_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = tmp61(slice)
            local vslice = tmp64(tmp69)
            local exp = wvNB(vslice)
            if exp.tag == "SOME" then
              return exp.payload
            end
            if exp.tag == "NONE" then
            else
              _raise(_Match, "prim-io.sml:404:67")
            end
            do
              block_PRIME(nil)
              local exp1 = wvNB(vslice)
              if exp1.tag == "SOME" then
                return exp1.payload
              elseif exp1.tag == "NONE" then
                return 0
              else
                _raise(_Match, "prim-io.sml:407:81")
              end
            end
          end}
        else
          writeArr_PRIME = NONE
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
    end
    if writeVecNB.tag == "NONE" then
      if writeArrNB.tag == "SOME" then
        local waNB = writeArrNB.payload
        writeVecNB_PRIME = {tag = "SOME", payload = function(slice)
          local tmp69 = tmp65(slice)
          local arr = tmp54({tmp69, tmp68})
          tmp57({di = 0, dst = arr, src = slice})
          local tmp70 = tmp58(arr)
          return waNB(tmp70)
        end}
        goto cont2
      end
      if writeArrNB.tag == "NONE" then
        if canOutput.tag == "SOME" and writeVec.tag == "SOME" then
          local canOutput_PRIME = canOutput.payload
          local wv = writeVec.payload
          writeVecNB_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = canOutput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              local tmp70 = wv(slice)
              return {tag = "SOME", payload = tmp70}
            end
          end}
        elseif canOutput.tag == "SOME" and (writeVec.tag == "NONE" and writeArr.tag == "SOME") then
          local canOutput_PRIME = canOutput.payload
          local wa = writeArr.payload
          writeVecNB_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = canOutput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              local tmp70 = tmp65(slice)
              local arr = tmp54({tmp70, tmp68})
              tmp57({di = 0, dst = arr, src = slice})
              local tmp71 = tmp58(arr)
              local tmp72 = wa(tmp71)
              return {tag = "SOME", payload = tmp72}
            end
          end}
        else
          writeVecNB_PRIME = NONE
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
    end
    if writeArrNB.tag == "NONE" then
      if writeVecNB.tag == "SOME" then
        local wvNB = writeVecNB.payload
        writeArrNB_PRIME = {tag = "SOME", payload = function(slice)
          local tmp69 = tmp61(slice)
          local vslice = tmp64(tmp69)
          return wvNB(vslice)
        end}
        goto cont3
      end
      if writeVecNB.tag == "NONE" then
        if canOutput.tag == "SOME" and writeArr.tag == "SOME" then
          local canOutput_PRIME = canOutput.payload
          local wa = writeArr.payload
          writeArrNB_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = canOutput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              local tmp70 = wa(slice)
              return {tag = "SOME", payload = tmp70}
            end
          end}
        elseif canOutput.tag == "SOME" and (writeVec.tag == "SOME" and writeArr.tag == "NONE") then
          local canOutput_PRIME = canOutput.payload
          local wv = writeVec.payload
          writeArrNB_PRIME = {tag = "SOME", payload = function(slice)
            local tmp69 = canOutput_PRIME(nil)
            if tmp69 then
            else
              return NONE
            end
            do
              local tmp70 = tmp61(slice)
              local vslice = tmp64(tmp70)
              local tmp71 = wv(vslice)
              return {tag = "SOME", payload = tmp71}
            end
          end}
        else
          writeArrNB_PRIME = NONE
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
_L[80] = _L[79](_L[39], _L[40], _L[33], _L[34], _L[35], _L[36], _L[37], _L[38], tmp50, tmp51, tmp45, tmp46, tmp47, tmp48, compare, 0x0)
_L[81] = _L[80].RD
_L[82] = _L[80].WR
_L[83] = _L[79](_L[1], _L[2], tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp35, tmp36, tmp28, tmp29, tmp30, tmp31, compare, 0)
_L[84] = _L[83].RD
_L[85] = _L[83].WR
_L[86] = function(tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74)
  local input = function(a)
    local exp = tmp65(a[1])
    local chunk = exp[1]
    a[1] = exp[2]
    return chunk
  end
  local input1 = function(a)
    local exp = tmp66(a[1])
    if exp.tag == "NONE" then
      return NONE
    elseif exp.tag == "SOME" then
      local e = exp.payload[1]
      a[1] = exp.payload[2]
      return {tag = "SOME", payload = e}
    else
      _raise(_Match, "imperative-io.sml:51:21")
    end
  end
  local inputN = function(a)
    local stream = a[1]
    local n = a[2]
    local exp = tmp68({stream[1], n})
    local chunk = exp[1]
    stream[1] = exp[2]
    return chunk
  end
  local inputAll = function(a)
    local exp = tmp67(a[1])
    local chunk = exp[1]
    a[1] = exp[2]
    return chunk
  end
  local canInput = function(a)
    local stream = a[1]
    local n = a[2]
    return tmp54({stream[1], n})
  end
  local lookahead = function(a)
    local exp = tmp66(a[1])
    if exp.tag == "NONE" then
      return NONE
    elseif exp.tag == "SOME" then
      return {tag = "SOME", payload = exp.payload[1]}
    else
      _raise(_Match, "imperative-io.sml:65:24")
    end
  end
  local closeIn = function(a)
    return tmp55(a[1])
  end
  local endOfStream = function(a)
    return tmp57(a[1])
  end
  local output = function(a)
    local stream = a[1]
    local chunk = a[2]
    return tmp71({stream[1], chunk})
  end
  local output1 = function(a)
    local stream = a[1]
    local elem = a[2]
    return tmp72({stream[1], elem})
  end
  local flushOut = function(a)
    return tmp60(a[1])
  end
  local closeOut = function(a)
    return tmp56(a[1])
  end
  local mkInstream = function(a)
    return {a}
  end
  local getInstream = function(a)
    return a[1]
  end
  local setInstream = function(a)
    local stream = a[1]
    stream[1] = a[2]
    return nil
  end
  local getPosOut = function(a)
    return tmp62(a[1])
  end
  local setPosOut = function(a)
    local stream = a[1]
    local tmp75 = tmp74(a[2])
    stream[1] = tmp75
    return nil
  end
  return {_StreamIO = {canInput = tmp54, closeIn = tmp55, closeOut = tmp56, endOfStream = tmp57, filePosIn = tmp58, filePosOut = tmp59, flushOut = tmp60, getBufferMode = tmp61, getPosOut = tmp62, getReader = tmp63, getWriter = tmp64, input = tmp65, input1 = tmp66, inputAll = tmp67, inputN = tmp68, mkInstream = tmp69, mkOutstream = tmp70, output = tmp71, output1 = tmp72, setBufferMode = tmp73, setPosOut = tmp74}, canInput = canInput, closeIn = closeIn, closeOut = closeOut, endOfStream = endOfStream, flushOut = flushOut, getInstream = getInstream, getOutstream = _EXCLAM, getPosOut = getPosOut, input = input, input1 = input1, inputAll = inputAll, inputN = inputN, lookahead = lookahead, mkInstream = mkInstream, mkOutstream = ref, output = output, output1 = output1, setInstream = setInstream, setOutstream = _COLON_EQ, setPosOut = setPosOut}
end
_L[87] = function(a)
  local rd = a[1]
  local content = a[2]
  local tip
  if rd.ioDesc.tag == "SOME" then
    local name = rd.name
    tip = {{tag = "READABLE", payload = {name = name, readable = _L[56][rd.ioDesc.payload]}}}
  else
    tip = {{tag = "PRIM_READER", payload = rd}}
  end
  local tmp54 = tmp36(content)
  if tmp54 > 0 then
    return {{tag = "BUFFERED", payload = {buffer = content, initialPosition = NONE, next = tip, position = 0}}}
  else
    return tip
  end
end
_L[88] = function(a)
  local tmp54, tmp55 = a, nil
  ::cont::
  do
    local ins, acc = tmp54, tmp55
    local x = ins[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local readable = x.payload.readable
      local name = x.payload.name
      ins[1] = {tag = "CLOSED", payload = name}
      local tmp56
      do
        local ioDesc = _L[59](readable)
        local tmp57 = {tag = "SOME", payload = function(a1)
          local exp
          do
            local tmp58 = readable:read(a1)
            if not tmp58 then
              exp = NONE
            else
              exp = {tag = "SOME", payload = tmp58}
            end
          end
          if exp.tag == "NONE" then
            return ""
          elseif exp.tag == "SOME" then
            return exp.payload
          else
            _raise(_Match, "text-io.sml:35:28")
          end
        end}
        local tmp58 = function(a1)
          return NONE
        end
        local tmp59 = function(a1)
          _L[60](ioDesc)
          readable:close()
          return nil
        end
        tmp56 = _L[84]({avail = tmp58, block = NONE, canInput = NONE, chunkSize = 1024, close = tmp59, endPos = NONE, getPos = NONE, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, readArr = NONE, readArrNB = NONE, readVec = tmp57, readVecNB = NONE, setPos = NONE, verifyPos = NONE})
      end
      local tmp57 = revAppend(acc, nil)
      local tmp58 = tmp27(tmp57)
      return {tmp56, tmp58}
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    do
      local rd = x.payload
      ins[1] = {tag = "CLOSED", payload = x.payload.name}
      local tmp56 = revAppend(acc, nil)
      local tmp57 = tmp27(tmp56)
      return {rd, tmp57}
    end
    ::else2::
    if x.tag == "BUFFERED" then
    elseif x.tag == "CLOSED" then
      _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "getReader", name = x.payload}}, "text-io.sml:113:57")
    else
      _raise(_Match, "text-io.sml:109:38")
    end
    do
      local buffer = x.payload.buffer
      local position = x.payload.position
      local next = x.payload.next
      local tmp56 = tmp30({buffer, position, NONE})
      tmp54 = next
      tmp55 = {tmp56, acc}
      goto cont
    end
  end
end
_L[89] = function(a)
  local x = a[1]
  if x.tag == "READABLE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = x.payload.name}}, "text-io.sml:118:55")
  end
  if x.tag == "PRIM_READER" and x.payload.getPos.tag == "NONE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = x.payload.name}}, "text-io.sml:119:89")
  end
  if x.tag == "PRIM_READER" and x.payload.getPos.tag == "SOME" then
  else
    goto else1
  end
  do
    return x.payload.getPos.payload(nil)
  end
  ::else1::
  if x.tag == "BUFFERED" and x.payload.initialPosition.tag == "NONE" then
    local tmp54
    do
      local tmp55 = x.payload.next
      ::cont::
      do
        local a1 = tmp55
        local x1 = a1[1]
        if x1.tag == "READABLE" then
          tmp54 = x1.payload.name
        elseif x1.tag == "PRIM_READER" then
          tmp54 = x1.payload.name
        elseif x1.tag == "BUFFERED" then
          tmp55 = x1.payload.next
          goto cont
        elseif x1.tag == "CLOSED" then
          tmp54 = x1.payload
        else
          _raise(_Match, "text-io.sml:93:23")
        end
      end
    end
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = tmp54}}, "text-io.sml:121:92")
  end
  if x.tag == "BUFFERED" and x.payload.initialPosition.tag == "SOME" then
  elseif x.tag == "CLOSED" then
    _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "filePosIn", name = x.payload}}, "text-io.sml:135:44")
  else
    _raise(_Match, "text-io.sml:117:25")
  end
  do
    local position, initialPosition, tmp54
    do
      position = x.payload.position
      local next = x.payload.next
      initialPosition = x.payload.initialPosition.payload
      tmp54 = next[1]
    end
    ::cont::
    do
      local a1 = tmp54
      if a1.tag == "PRIM_READER" and (a1.payload.getPos.tag == "SOME" and (a1.payload.setPos.tag == "SOME" and a1.payload.readVec.tag == "SOME")) then
      elseif a1.tag == "BUFFERED" then
        tmp54 = a1.payload.next[1]
        goto cont
      elseif a1.tag == "CLOSED" then
        _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "filePosIn", name = a1.payload}}, "text-io.sml:130:56")
      elseif a1.tag == "PRIM_READER" then
        _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = a1.payload.name}}, "text-io.sml:131:86")
      elseif a1.tag == "READABLE" then
        _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = a1.payload.name}}, "text-io.sml:132:67")
      else
        _raise(_Match, "text-io.sml:123:37")
      end
      do
        local getPos = a1.payload.getPos.payload
        local setPos = a1.payload.setPos.payload
        local readVec = a1.payload.readVec.payload
        local savedPosition = getPos(nil)
        setPos(initialPosition)
        readVec(position)
        local tmp55 = getPos(nil)
        setPos(savedPosition)
        return tmp55
      end
    end
  end
end
_L[90] = function(stream, f, n)
  local exp
  do
    local tmp54 = stream.readable:read(n)
    if not tmp54 then
      exp = NONE
    else
      exp = {tag = "SOME", payload = tmp54}
    end
  end
  if exp.tag == "SOME" then
    local chunk = exp.payload
    f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = NONE, next = {{tag = "READABLE", payload = stream}}, position = 0}}
    return true
  elseif exp.tag == "NONE" then
    return false
  else
    _raise(_Match, "text-io.sml:137:11")
  end
end
_L[91] = function(rd, f, n)
  local name = rd.name
  local readVec = rd.readVec
  local readArr = rd.readArr
  local readVecNB = rd.readVecNB
  local readArrNB = rd.readArrNB
  local block = rd.block
  local initialPosition
  do
    if rd.getPos.tag == "SOME" then
    else
      initialPosition = NONE
      goto cont
    end
    do
      local tmp54 = rd.getPos.payload(nil)
      initialPosition = {tag = "SOME", payload = tmp54}
    end
  end
  ::cont::
  local chunk
  do
    if readVec.tag == "SOME" then
    else
      goto else1
    end
    chunk = readVec.payload(n)
    goto cont1
    ::else1::
    if readVec.tag == "NONE" then
    else
      _raise(_Match, "text-io.sml:152:27")
    end
    do
      if readArr.tag == "SOME" then
      else
        goto else2
      end
      do
        local ra = readArr.payload
        local arr = _L[1]({n, 0})
        local tmp54 = tmp39(arr)
        local actual = ra(tmp54)
        local tmp55 = tmp41({arr, 0, {tag = "SOME", payload = actual}})
        chunk = tmp42(tmp55)
        goto cont1
      end
      ::else2::
      if readArr.tag == "NONE" then
      else
        _raise(_Match, "text-io.sml:154:39")
      end
      do
        if block.tag == "SOME" and readVecNB.tag == "SOME" then
        else
          goto else3
        end
        do
          local block_PRIME = block.payload
          local rvNB = readVecNB.payload
          local exp = rvNB(n)
          if exp.tag == "SOME" then
            chunk = exp.payload
            goto cont1
          end
          if exp.tag == "NONE" then
          else
            _raise(_Match, "text-io.sml:161:56")
          end
          do
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
          end
        end
        ::else3::
        if block.tag == "SOME" and (readVecNB.tag == "NONE" and readArrNB.tag == "SOME") then
        else
          _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "<unknown>", name = name}}, "text-io.sml:180:60")
        end
        do
          local block_PRIME = block.payload
          local raNB = readArrNB.payload
          local arr = _L[1]({n, 0})
          local aslice = tmp39(arr)
          local exp = raNB(aslice)
          if exp.tag == "SOME" then
          else
            goto else4
          end
          do
            local tmp54 = tmp41({arr, 0, {tag = "SOME", payload = exp.payload}})
            chunk = tmp42(tmp54)
            goto cont1
          end
          ::else4::
          if exp.tag == "NONE" then
          else
            _raise(_Match, "text-io.sml:172:58")
          end
          do
            block_PRIME(nil)
            local exp1 = raNB(aslice)
            if exp1.tag == "SOME" then
            elseif exp1.tag == "NONE" then
              chunk = ""
              goto cont1
            else
              _raise(_Match, "text-io.sml:175:72")
            end
            do
              local tmp54 = tmp41({arr, 0, {tag = "SOME", payload = exp1.payload}})
              chunk = tmp42(tmp54)
            end
          end
        end
      end
    end
  end
  ::cont1::
  f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = initialPosition, next = {{tag = "PRIM_READER", payload = rd}}, position = 0}}
  return nil
end
_L[92] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local tmp55 = _L[90](x.payload, a1, 1)
      if tmp55 then
        tmp54 = a1
        goto cont
      else
        return {"", a1}
      end
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    do
      local rd = x.payload
      _L[91](rd, a1, x.payload.chunkSize)
      tmp54 = a1
      goto cont
    end
    ::else2::
    if x.tag == "BUFFERED" then
    elseif x.tag == "CLOSED" then
      return {"", a1}
    else
      _raise(_Match, "text-io.sml:183:32")
    end
    do
      local buffer = x.payload.buffer
      local position = x.payload.position
      local next = x.payload.next
      local tmp55 = extract(buffer, position, NONE)
      return {tmp55, next}
    end
  end
end
_L[93] = function(buffer, position, next, initialPosition)
  if position >= #buffer then
    return next
  else
    return {{tag = "BUFFERED", payload = {buffer = buffer, initialPosition = initialPosition, next = next, position = position}}}
  end
end
_L[94] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local tmp55 = _L[90](x.payload, a1, 1)
      if tmp55 then
        tmp54 = a1
        goto cont
      else
        return NONE
      end
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    do
      local rd = x.payload
      _L[91](rd, a1, x.payload.chunkSize)
      tmp54 = a1
      goto cont
    end
    ::else2::
    if x.tag == "BUFFERED" then
    elseif x.tag == "CLOSED" then
      return NONE
    else
      _raise(_Match, "text-io.sml:196:33")
    end
    do
      local buffer = x.payload.buffer
      local position = x.payload.position
      local next = x.payload.next
      local initialPosition = x.payload.initialPosition
      local tmp55 = sub1({buffer, position})
      local tmp56 = _L[93](buffer, _Int_add(position, 1), next, initialPosition)
      return {tag = "SOME", payload = {tmp55, tmp56}}
    end
  end
end
_L[95] = function(a)
  local tmp54, tmp55
  do
    local f = a[1]
    tmp54 = f
    tmp55 = a[2]
  end
  ::cont::
  do
    local f, n = tmp54, tmp55
    local x = f[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local tmp56 = _L[90](x.payload, f, n)
      if tmp56 then
        tmp54 = f
        tmp55 = n
        goto cont
      else
        return {"", f}
      end
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    _L[91](x.payload, f, n)
    tmp54 = f
    tmp55 = n
    goto cont
    ::else2::
    if x.tag == "BUFFERED" then
    elseif x.tag == "CLOSED" then
      return {"", f}
    else
      _raise(_Match, "text-io.sml:204:36")
    end
    do
      local buffer, position, next
      do
        buffer = x.payload.buffer
        position = x.payload.position
        next = x.payload.next
        local initialPosition = x.payload.initialPosition
        local tmp56 = _Int_add(position, n)
        if tmp56 <= #buffer then
        else
          goto else3
        end
        do
          local tmp57 = substring(buffer, position, n)
          local tmp58 = _L[93](buffer, tmp56, next, initialPosition)
          return {tmp57, tmp58}
        end
      end
      ::else3::
      local buffer0 = extract(buffer, position, NONE)
      local exp = _L[95]({next, _Int_sub(n, #buffer0)})
      local buffer1 = exp[1]
      local next1 = exp[2]
      return {buffer0 .. buffer1, next1}
    end
  end
end
_L[96] = function(a)
  local tmp54 = a
  ::cont::
  do
    local x
    do
      local a1 = tmp54
      x = a1[1]
      if x.tag == "READABLE" then
        local s = x.payload
        local exp
        do
          local tmp55 = s.readable:read("L")
          if not tmp55 then
            exp = NONE
          else
            exp = {tag = "SOME", payload = tmp55}
          end
        end
        if exp.tag == "SOME" then
          local chunk = exp.payload
          a1[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = NONE, next = {{tag = "READABLE", payload = s}}, position = 0}}
          tmp54 = a1
          goto cont
        elseif exp.tag == "NONE" then
          return NONE
        else
          _raise(_Match, "text-io.sml:141:11")
        end
      end
      if x.tag == "PRIM_READER" then
      else
        goto else1
      end
      do
        local rd = x.payload
        _L[91](rd, a1, x.payload.chunkSize)
        tmp54 = a1
        goto cont
      end
    end
    ::else1::
    if x.tag == "BUFFERED" then
    elseif x.tag == "CLOSED" then
      return NONE
    else
      _raise(_Match, "text-io.sml:221:36")
    end
    do
      local buffer = x.payload.buffer
      local position = x.payload.position
      local next = x.payload.next
      local initialPosition = x.payload.initialPosition
      local tmp55 = position
      ::cont1::
      do
        local a1 = tmp55
        if a1 >= #buffer then
        else
          goto else2
        end
        do
          local exp = _L[96](next)
          if exp.tag == "NONE" then
          else
            goto else3
          end
          do
            local tmp56 = extract(buffer, position, NONE)
            return {tag = "SOME", payload = {tmp56 .. "\n", next}}
          end
          ::else3::
          if exp.tag == "SOME" then
          else
            _raise(_Match, "text-io.sml:229:68")
          end
          do
            local line = exp.payload[1]
            local next1 = exp.payload[2]
            local tmp56 = extract(buffer, position, NONE)
            return {tag = "SOME", payload = {tmp56 .. line, next1}}
          end
        end
        ::else2::
        local tmp56 = sub1({buffer, a1})
        if tmp56 == 10 then
        else
          tmp55 = _Int_add(a1, 1)
          goto cont1
        end
        do
          local tmp57 = _Int_add(a1, 1)
          local tmp58 = substring(buffer, position, _Int_sub(tmp57, position))
          local tmp59 = _L[93](buffer, tmp57, next, initialPosition)
          return {tag = "SOME", payload = {tmp58, tmp59}}
        end
      end
    end
  end
end
_L[97] = function(a)
  local tmp54, tmp55
  do
    local x = a[1]
    if x.tag == "READABLE" then
      local s = x.payload
      local tmp56 = s.readable:read("a")
      local tmp57 = {{tag = "READABLE", payload = s}}
      a[1] = {tag = "BUFFERED", payload = {buffer = tmp56, initialPosition = NONE, next = tmp57, position = 0}}
      return {tmp56, tmp57}
    end
    tmp55, tmp54 = nil, a
  end
  ::cont::
  do
    local contentsRev, f = tmp55, tmp54
    local exp = _L[92](f)
    if exp[1] == "" then
    else
      local content = exp[1]
      local f1 = exp[2]
      tmp55 = {content, contentsRev}
      tmp54 = f1
      goto cont
    end
    do
      local f1 = exp[2]
      local tmp56 = revAppend(contentsRev, nil)
      return {table_concat(_VectorOrArray_fromList(tmp56)), f1}
    end
  end
end
_L[98] = function(a)
  local n, x
  do
    local f = a[1]
    n = a[2]
    if n < 0 then
      _raise(_Size, "text-io.sml:251:42")
    end
    x = f[1]
    if x.tag == "READABLE" then
      _raise({tag = _L[9], payload = {cause = _L[11], ["function"] = "canInput", name = x.payload.name}}, "text-io.sml:254:77")
    end
    if x.tag == "PRIM_READER" and x.payload.canInput.tag == "SOME" then
    else
      goto else1
    end
    do
      local tmp54 = x.payload.canInput.payload(nil)
      if tmp54 then
        return {tag = "SOME", payload = 1}
      else
        return NONE
      end
    end
  end
  ::else1::
  if x.tag == "PRIM_READER" and x.payload.canInput.tag == "NONE" then
    _raise({tag = _L[9], payload = {cause = _L[11], ["function"] = "canInput", name = x.payload.name}}, "text-io.sml:259:108")
  end
  if x.tag == "BUFFERED" then
    local buffer = x.payload.buffer
    local position = x.payload.position
    local tmp54 = _Int_sub(#buffer, position)
    if n < tmp54 then
      return {tag = "SOME", payload = n}
    else
      return {tag = "SOME", payload = tmp54}
    end
  elseif x.tag == "CLOSED" then
    return {tag = "SOME", payload = 0}
  else
    _raise(_Match, "text-io.sml:253:42")
  end
end
_L[99] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
      local readable = x.payload.readable
      local name = x.payload.name
      readable:close()
      a1[1] = {tag = "CLOSED", payload = name}
      return nil
    end
    if x.tag == "PRIM_READER" then
    elseif x.tag == "BUFFERED" then
      tmp54 = x.payload.next
      goto cont
    elseif x.tag == "CLOSED" then
      return nil
    else
      _raise(_Match, "text-io.sml:262:34")
    end
    do
      local name = x.payload.name
      x.payload.close(nil)
      a1[1] = {tag = "CLOSED", payload = name}
      return nil
    end
  end
end
_L[100] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
      return not x.payload.readable:read(0)
    end
    if x.tag == "PRIM_READER" then
    elseif x.tag == "BUFFERED" then
      return false
    elseif x.tag == "CLOSED" then
      return true
    else
      _raise(_Match, "text-io.sml:269:38")
    end
    do
      local rd = x.payload
      _L[91](rd, a1, x.payload.chunkSize)
      tmp54 = a1
      goto cont
    end
  end
end
_L[101] = function(stream, name, s)
  local tmp54, tmp55 = stream:write(s)
  if not tmp54 then
  else
    return nil
  end
  do
    local tmp56 = _Fail(tmp55)
    _raise({tag = _L[9], payload = {cause = tmp56, ["function"] = "output", name = name}}, "text-io.sml:290:43")
  end
end
_L[102] = function(a)
  if a[1].tag == "LUA_WRITABLE" then
  else
    goto else1
  end
  do
    local writable = a[1].payload.writable
    local name = a[1].payload.name
    return _L[101](writable, name, a[2])
  end
  ::else1::
  if a[1].tag == "PRIM_WRITER" then
  else
    _raise(_Match, "text-io.sml:361:9")
  end
  do
    local name, chunkSize, writeVec, buffer, content, x
    do
      name = a[1].payload.writer.name
      chunkSize = a[1].payload.writer.chunkSize
      writeVec = a[1].payload.writer.writeVec
      local buffer_mode = a[1].payload.buffer_mode
      buffer = a[1].payload.buffer
      content = a[2]
      x = buffer_mode[1]
      if x == "NO_BUF" then
      else
        goto else2
      end
      do
        if writeVec.tag == "SOME" then
        elseif writeVec.tag == "NONE" then
          _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "output", name = name}}, "text-io.sml:367:41")
        else
          _raise(_Match, "text-io.sml:365:29")
        end
        do
          local writeVec1 = writeVec.payload
          local tmp54 = tmp28(content)
          writeVec1(tmp54)
          return nil
        end
      end
    end
    ::else2::
    if x == "LINE_BUF" then
    else
      goto else3
    end
    do
      local tmp54 = tmp34(function(c)
        return c == 10
      end)
      local tmp55 = tmp54(content)
      if tmp55 then
      else
        buffer[1] = {content, buffer[1]}
        return nil
      end
      do
        local tmp56 = revAppend({content, buffer[1]}, nil)
        local tmp57 = table_concat(_VectorOrArray_fromList(tmp56))
        if writeVec.tag == "SOME" then
        elseif writeVec.tag == "NONE" then
          _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "output", name = name}}, "text-io.sml:373:49")
        else
          _raise(_Match, "text-io.sml:371:37")
        end
        do
          local writeVec1 = writeVec.payload
          local tmp58 = tmp28(tmp57)
          writeVec1(tmp58)
          buffer[1] = nil
          return nil
        end
      end
    end
    ::else3::
    if x == "BLOCK_BUF" then
    else
      _raise(_Match, "text-io.sml:364:11")
    end
    do
      local x1 = buffer[1]
      local tmp54 = foldl(function(a1)
        local z = a1[1]
        return _Int_add(a1[2], #z)
      end)
      local tmp55 = tmp54(#content)
      local bufSize = tmp55(x1)
      if bufSize >= chunkSize then
      else
        buffer[1] = {content, buffer[1]}
        return nil
      end
      do
        local tmp56 = revAppend({content, buffer[1]}, nil)
        local tmp57 = table_concat(_VectorOrArray_fromList(tmp56))
        if writeVec.tag == "SOME" then
        elseif writeVec.tag == "NONE" then
          _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "output", name = name}}, "text-io.sml:384:53")
        else
          _raise(_Match, "text-io.sml:382:41")
        end
        do
          local writeVec1 = writeVec.payload
          local tmp58 = tmp28(tmp57)
          writeVec1(tmp58)
          buffer[1] = nil
          return nil
        end
      end
    end
  end
end
_L[103] = function(a)
  local stream = a[1]
  return _L[102]({stream, string_char(a[2])})
end
_L[104] = function(name, writeVec, buffer)
  local tmp54 = revAppend(buffer[1], nil)
  local tmp55 = table_concat(_VectorOrArray_fromList(tmp54))
  buffer[1] = nil
  if tmp55 ~= "" then
  else
    return nil
  end
  do
    if writeVec.tag == "SOME" then
    elseif writeVec.tag == "NONE" then
      _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "flushOut", name = name}}, "text-io.sml:406:30")
    else
      _raise(_Match, "text-io.sml:404:18")
    end
    do
      local writeVec1 = writeVec.payload
      local tmp56 = tmp28(tmp55)
      writeVec1(tmp56)
      return nil
    end
  end
end
_L[105] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:flush()
    return nil
  else
    if a.tag == "PRIM_WRITER" then
    else
      _raise(_Match, "text-io.sml:410:9")
    end
    do
      local name = a.payload.writer.name
      local writeVec = a.payload.writer.writeVec
      return _L[104](name, writeVec, a.payload.buffer)
    end
  end
end
_L[106] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:close()
    return nil
  end
  if a.tag == "PRIM_WRITER" then
  else
    _raise(_Match, "text-io.sml:413:9")
  end
  do
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local close = a.payload.writer.close
    _L[104](name, writeVec, a.payload.buffer)
    return close(nil)
  end
end
_L[107] = function(a)
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
  end
  if a[1].tag == "PRIM_WRITER" then
  else
    _raise(_Match, "text-io.sml:418:9")
  end
  do
    local name = a[1].payload.writer.name
    local writeVec = a[1].payload.writer.writeVec
    local buffer_mode = a[1].payload.buffer_mode
    local buffer = a[1].payload.buffer
    local mode = a[2]
    local tmp54 = _L[17](mode, _L[14])
    if tmp54 then
    else
      buffer_mode[1] = mode
      return nil
    end
    do
      _L[104](name, writeVec, buffer)
      buffer_mode[1] = mode
      return nil
    end
  end
end
_L[108] = function(a)
  if a.tag == "LUA_WRITABLE" then
    return a.payload.buffer_mode[1]
  elseif a.tag == "PRIM_WRITER" then
    return a.payload.buffer_mode[1]
  else
    _raise(_Match, "text-io.sml:429:9")
  end
end
_L[109] = function(a)
  if a[1].ioDesc.tag == "SOME" then
    local name = a[1].name
    local ioDesc = a[1].ioDesc.payload
    local mode = a[2]
    local tmp54 = _L[56][ioDesc]
    return {tag = "LUA_WRITABLE", payload = {buffer_mode = {mode}, name = name, writable = tmp54}}
  else
    local w = a[1]
    local tmp54 = {a[2]}
    return {tag = "PRIM_WRITER", payload = {buffer = {nil}, buffer_mode = tmp54, writer = w}}
  end
end
_L[110] = function(a)
  if a.tag == "LUA_WRITABLE" then
  else
    goto else1
  end
  do
    local writable = a.payload.writable
    local buffer_mode = a.payload.buffer_mode
    local name = a.payload.name
    writable:flush()
    local tmp54
    do
      local ioDesc = _L[59](writable)
      local tmp55 = {tag = "SOME", payload = function(slice)
        local tmp56 = tmp31(slice)
        _L[101](writable, name, tmp56)
        writable:flush()
        return tmp29(slice)
      end}
      local tmp56 = function(a1)
        _L[60](ioDesc)
        writable:close()
        return nil
      end
      tmp54 = _L[85]({block = NONE, canOutput = NONE, chunkSize = 1024, close = tmp56, endPos = NONE, getPos = NONE, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, setPos = NONE, verifyPos = NONE, writeArr = NONE, writeArrNB = NONE, writeVec = tmp55, writeVecNB = NONE})
    end
    return {tmp54, buffer_mode[1]}
  end
  ::else1::
  if a.tag == "PRIM_WRITER" then
  else
    _raise(_Match, "text-io.sml:436:9")
  end
  do
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local buffer_mode = a.payload.buffer_mode
    _L[104](name, writeVec, a.payload.buffer)
    return {writer, buffer_mode[1]}
  end
end
_L[111] = function(a)
  if a.tag == "LUA_WRITABLE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "getPosOut", name = a.payload.name}}, "text-io.sml:443:50")
  end
  if a.tag == "PRIM_WRITER" then
  else
    _raise(_Match, "text-io.sml:443:9")
  end
  do
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local getPos = a.payload.writer.getPos
    local buffer_mode = a.payload.buffer_mode
    local buffer = a.payload.buffer
    if getPos.tag == "SOME" then
    elseif getPos.tag == "NONE" then
      _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "getPosOut", name = name}}, "text-io.sml:449:23")
    else
      _raise(_Match, "text-io.sml:445:11")
    end
    do
      local getPos1 = getPos.payload
      _L[104](name, writeVec, buffer)
      local tmp54 = getPos1(nil)
      return {buffer = buffer, buffer_mode = buffer_mode, pos = tmp54, writer = writer}
    end
  end
end
_L[112] = function(a)
  local writer = a.writer
  local name = a.writer.name
  local setPos = a.writer.setPos
  local buffer_mode = a.buffer_mode
  local buffer = a.buffer
  local pos = a.pos
  if setPos.tag == "SOME" then
  elseif setPos.tag == "NONE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "setPosOut", name = name}}, "text-io.sml:455:23")
  else
    _raise(_Match, "text-io.sml:451:11")
  end
  do
    setPos.payload(pos)
    return {tag = "PRIM_WRITER", payload = {buffer = buffer, buffer_mode = buffer_mode, writer = writer}}
  end
end
_L[113] = _L[86](_L[98], _L[99], _L[106], _L[100], _L[89], function(a)
  return a.pos
end, _L[105], _L[108], _L[111], _L[88], _L[110], _L[92], _L[94], _L[97], _L[95], _L[87], _L[109], _L[102], _L[103], _L[107], _L[112])
_L[114] = function(a)
  local tmp54 = _L[113].getInstream(a)
  local exp = _L[96](tmp54)
  if exp.tag == "NONE" then
    return NONE
  end
  if exp.tag == "SOME" then
  else
    _raise(_Match, "text-io.sml:472:24")
  end
  do
    local line = exp.payload[1]
    local stream_PRIME = exp.payload[2]
    local tmp55 = _L[113].setInstream
    tmp55({a, stream_PRIME})
    return {tag = "SOME", payload = line}
  end
end
_L[115] = function(a)
  local tmp54, tmp55 = tmp7(a, "r")
  if tmp54 == nil then
  else
    local tmp56 = _L[113].mkInstream
    return tmp56({{tag = "READABLE", payload = {name = a, readable = tmp54}}})
  end
  do
    local tmp56 = _Fail(tmp55)
    _raise({tag = _L[9], payload = {cause = tmp56, ["function"] = "TextIO.openIn", name = a}}, "text-io.sml:479:26")
  end
end
do
  _L[117] = _L[113].mkOutstream
  _L[116] = _L[117]({tag = "LUA_WRITABLE", payload = {buffer_mode = {_L[15]}, name = "<stdout>", writable = tmp10}})
end
do
  _L[119] = _L[113].mkOutstream
  _L[118] = _L[119]({tag = "LUA_WRITABLE", payload = {buffer_mode = {_L[14]}, name = "<stderr>", writable = tmp9}})
end
_L[120] = _L[113].closeIn
_L[121] = _L[113].closeOut
_L[122] = _L[113].getInstream
_L[123] = _L[113].inputAll
_L[124] = _L[113].output
_L[125] = function(a)
  local rd = a[1]
  local content = a[2]
  local tip
  if rd.ioDesc.tag == "SOME" then
    local name = rd.name
    tip = {{tag = "READABLE", payload = {name = name, readable = _L[56][rd.ioDesc.payload]}}}
  else
    tip = {{tag = "PRIM_READER", payload = rd}}
  end
  local tmp54 = tmp51(content)
  if tmp54 > 0 then
    return {{tag = "BUFFERED", payload = {buffer = content, initialPosition = NONE, next = tip, position = 0}}}
  else
    return tip
  end
end
_L[126] = function(a)
  local tmp54, tmp55 = a, nil
  ::cont::
  do
    local ins, acc = tmp54, tmp55
    local x = ins[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local readable = x.payload.readable
      local name = x.payload.name
      ins[1] = {tag = "CLOSED", payload = name}
      local tmp56
      do
        local ioDesc = _L[59](readable)
        local tmp57 = {tag = "SOME", payload = function(a1)
          local exp
          do
            local tmp58 = readable:read(a1)
            if not tmp58 then
              exp = NONE
            else
              exp = {tag = "SOME", payload = tmp58}
            end
          end
          if exp.tag == "NONE" then
          elseif exp.tag == "SOME" then
            return exp.payload
          else
            _raise(_Match, "bin-io.sml:28:28")
          end
          do
            return tmp50(nil)
          end
        end}
        local tmp58 = function(a1)
          return NONE
        end
        local tmp59 = function(a1)
          _L[60](ioDesc)
          readable:close()
          return nil
        end
        tmp56 = _L[81]({avail = tmp58, block = NONE, canInput = NONE, chunkSize = 1024, close = tmp59, endPos = NONE, getPos = NONE, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, readArr = NONE, readArrNB = NONE, readVec = tmp57, readVecNB = NONE, setPos = NONE, verifyPos = NONE})
      end
      local tmp57 = revAppend(acc, nil)
      local tmp58 = tmp44(tmp57)
      return {tmp56, tmp58}
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    do
      local rd = x.payload
      ins[1] = {tag = "CLOSED", payload = x.payload.name}
      local tmp56 = revAppend(acc, nil)
      local tmp57 = tmp44(tmp56)
      return {rd, tmp57}
    end
    ::else2::
    if x.tag == "BUFFERED" then
    elseif x.tag == "CLOSED" then
      _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "getReader", name = x.payload}}, "bin-io.sml:103:57")
    else
      _raise(_Match, "bin-io.sml:99:38")
    end
    do
      local buffer = x.payload.buffer
      local position = x.payload.position
      local next = x.payload.next
      local tmp56 = tmp47({buffer, position, NONE})
      tmp54 = next
      tmp55 = {tmp56, acc}
      goto cont
    end
  end
end
_L[127] = function(a)
  local x = a[1]
  if x.tag == "READABLE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = x.payload.name}}, "bin-io.sml:108:55")
  end
  if x.tag == "PRIM_READER" and x.payload.getPos.tag == "NONE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = x.payload.name}}, "bin-io.sml:109:88")
  end
  if x.tag == "PRIM_READER" and x.payload.getPos.tag == "SOME" then
  else
    goto else1
  end
  do
    return x.payload.getPos.payload(nil)
  end
  ::else1::
  if x.tag == "BUFFERED" and x.payload.initialPosition.tag == "NONE" then
    local tmp54
    do
      local tmp55 = x.payload.next
      ::cont::
      do
        local a1 = tmp55
        local x1 = a1[1]
        if x1.tag == "READABLE" then
          tmp54 = x1.payload.name
        elseif x1.tag == "PRIM_READER" then
          tmp54 = x1.payload.name
        elseif x1.tag == "BUFFERED" then
          tmp55 = x1.payload.next
          goto cont
        elseif x1.tag == "CLOSED" then
          tmp54 = x1.payload
        else
          _raise(_Match, "bin-io.sml:84:23")
        end
      end
    end
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "filePosIn", name = tmp54}}, "bin-io.sml:111:92")
  elseif x.tag == "BUFFERED" and x.payload.initialPosition.tag == "SOME" then
    local position = x.payload.position
    return _Int_add(x.payload.initialPosition.payload, position)
  elseif x.tag == "CLOSED" then
    _raise({tag = _L[9], payload = {cause = _L[13], ["function"] = "filePosIn", name = x.payload}}, "bin-io.sml:113:44")
  else
    _raise(_Match, "bin-io.sml:107:25")
  end
end
_L[128] = function(stream, f, n)
  local exp
  do
    local tmp54 = stream.readable:read(n)
    if not tmp54 then
      exp = NONE
    else
      exp = {tag = "SOME", payload = tmp54}
    end
  end
  if exp.tag == "SOME" then
    local chunk = exp.payload
    f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = NONE, next = {{tag = "READABLE", payload = stream}}, position = 0}}
    return true
  elseif exp.tag == "NONE" then
    return false
  else
    _raise(_Match, "bin-io.sml:115:11")
  end
end
_L[129] = function(rd, f, n)
  local name = rd.name
  local readVec = rd.readVec
  local readArr = rd.readArr
  local readVecNB = rd.readVecNB
  local readArrNB = rd.readArrNB
  local block = rd.block
  local initialPosition
  do
    if rd.getPos.tag == "SOME" then
    else
      initialPosition = NONE
      goto cont
    end
    do
      local tmp54 = rd.getPos.payload(nil)
      initialPosition = {tag = "SOME", payload = tmp54}
    end
  end
  ::cont::
  local chunk
  do
    if readVec.tag == "SOME" then
    else
      goto else1
    end
    chunk = readVec.payload(n)
    goto cont1
    ::else1::
    if readVec.tag == "NONE" then
    else
      _raise(_Match, "bin-io.sml:126:27")
    end
    do
      if readArr.tag == "SOME" then
      else
        goto else2
      end
      do
        local ra = readArr.payload
        local arr = _L[39]({n, 0x0})
        local tmp54 = _L[35](arr)
        local actual = ra(tmp54)
        local tmp55 = _L[37]({arr, 0, {tag = "SOME", payload = actual}})
        chunk = _L[38](tmp55)
        goto cont1
      end
      ::else2::
      if readArr.tag == "NONE" then
      else
        _raise(_Match, "bin-io.sml:128:39")
      end
      do
        if block.tag == "SOME" and readVecNB.tag == "SOME" then
        else
          goto else3
        end
        do
          local block_PRIME = block.payload
          local rvNB = readVecNB.payload
          local exp = rvNB(n)
          if exp.tag == "SOME" then
            chunk = exp.payload
            goto cont1
          end
          if exp.tag == "NONE" then
          else
            _raise(_Match, "bin-io.sml:135:56")
          end
          do
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
              chunk = tmp50(nil)
              goto cont1
            end
          end
        end
        ::else3::
        if block.tag == "SOME" and (readVecNB.tag == "NONE" and readArrNB.tag == "SOME") then
        else
          _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "<unknown>", name = name}}, "bin-io.sml:154:60")
        end
        do
          local block_PRIME = block.payload
          local raNB = readArrNB.payload
          local arr = _L[39]({n, 0x0})
          local aslice = _L[35](arr)
          local exp = raNB(aslice)
          if exp.tag == "SOME" then
          else
            goto else4
          end
          do
            local tmp54 = _L[37]({arr, 0, {tag = "SOME", payload = exp.payload}})
            chunk = _L[38](tmp54)
            goto cont1
          end
          ::else4::
          if exp.tag == "NONE" then
          else
            _raise(_Match, "bin-io.sml:146:58")
          end
          do
            block_PRIME(nil)
            local exp1 = raNB(aslice)
            if exp1.tag == "SOME" then
            else
              if exp1.tag == "NONE" then
              else
                _raise(_Match, "bin-io.sml:149:72")
              end
              chunk = tmp50(nil)
              goto cont1
            end
            do
              local tmp54 = _L[37]({arr, 0, {tag = "SOME", payload = exp1.payload}})
              chunk = _L[38](tmp54)
            end
          end
        end
      end
    end
  end
  ::cont1::
  f[1] = {tag = "BUFFERED", payload = {buffer = chunk, initialPosition = initialPosition, next = {{tag = "PRIM_READER", payload = rd}}, position = 0}}
  return nil
end
_L[130] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local tmp55 = _L[128](x.payload, a1, 1)
      if tmp55 then
        tmp54 = a1
        goto cont
      end
      local tmp56 = tmp50(nil)
      return {tmp56, a1}
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    do
      local rd = x.payload
      _L[129](rd, a1, x.payload.chunkSize)
      tmp54 = a1
      goto cont
    end
    ::else2::
    if x.tag == "BUFFERED" then
    else
      goto else3
    end
    do
      local buffer = x.payload.buffer
      local position = x.payload.position
      local next = x.payload.next
      local tmp55 = tmp47({buffer, position, NONE})
      local tmp56 = tmp48(tmp55)
      return {tmp56, next}
    end
    ::else3::
    if x.tag == "CLOSED" then
    else
      _raise(_Match, "bin-io.sml:157:32")
    end
    do
      local tmp55 = tmp50(nil)
      return {tmp55, a1}
    end
  end
end
_L[131] = function(buffer, position, next, initialPosition)
  local tmp54 = tmp51(buffer)
  if position >= tmp54 then
    return next
  else
    return {{tag = "BUFFERED", payload = {buffer = buffer, initialPosition = initialPosition, next = next, position = position}}}
  end
end
_L[132] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local tmp55 = _L[128](x.payload, a1, 1)
      if tmp55 then
        tmp54 = a1
        goto cont
      else
        return NONE
      end
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    do
      local rd = x.payload
      _L[129](rd, a1, x.payload.chunkSize)
      tmp54 = a1
      goto cont
    end
    ::else2::
    if x.tag == "BUFFERED" then
    elseif x.tag == "CLOSED" then
      return NONE
    else
      _raise(_Match, "bin-io.sml:170:33")
    end
    do
      local buffer = x.payload.buffer
      local position = x.payload.position
      local next = x.payload.next
      local initialPosition = x.payload.initialPosition
      local tmp55 = tmp52({buffer, position})
      local tmp56 = _L[131](buffer, _Int_add(position, 1), next, initialPosition)
      return {tag = "SOME", payload = {tmp55, tmp56}}
    end
  end
end
_L[133] = function(a)
  local tmp54, tmp55
  do
    local f = a[1]
    tmp54 = f
    tmp55 = a[2]
  end
  ::cont::
  do
    local f, n = tmp54, tmp55
    local x = f[1]
    if x.tag == "READABLE" then
    else
      goto else1
    end
    do
      local tmp56 = _L[128](x.payload, f, n)
      if tmp56 then
        tmp54 = f
        tmp55 = n
        goto cont
      end
      local tmp57 = tmp50(nil)
      return {tmp57, f}
    end
    ::else1::
    if x.tag == "PRIM_READER" then
    else
      goto else2
    end
    _L[129](x.payload, f, n)
    tmp54 = f
    tmp55 = n
    goto cont
    ::else2::
    if x.tag == "BUFFERED" then
    else
      goto else3
    end
    do
      local buffer, position, next
      do
        buffer = x.payload.buffer
        position = x.payload.position
        next = x.payload.next
        local initialPosition = x.payload.initialPosition
        local tmp56 = _Int_add(position, n)
        local tmp57 = tmp51(buffer)
        if tmp56 <= tmp57 then
        else
          goto else4
        end
        do
          local tmp58 = tmp47({buffer, position, {tag = "SOME", payload = n}})
          local tmp59 = tmp48(tmp58)
          local tmp60 = _L[131](buffer, tmp56, next, initialPosition)
          return {tmp59, tmp60}
        end
      end
      ::else4::
      local buffer0 = tmp47({buffer, position, NONE})
      local tmp56 = tmp46(buffer0)
      local exp = _L[133]({next, _Int_sub(n, tmp56)})
      local buffer1 = exp[1]
      local next1 = exp[2]
      local tmp57 = tmp45(buffer1)
      local tmp58 = tmp44({buffer0, {tmp57, nil}})
      return {tmp58, next1}
    end
    ::else3::
    if x.tag == "CLOSED" then
    else
      _raise(_Match, "bin-io.sml:178:36")
    end
    do
      local tmp56 = tmp50(nil)
      return {tmp56, f}
    end
  end
end
_L[134] = function(a)
  local tmp54, tmp55
  do
    local x = a[1]
    if x.tag == "READABLE" then
      local s = x.payload
      local tmp56 = s.readable:read("a")
      local tmp57 = {{tag = "READABLE", payload = s}}
      a[1] = {tag = "BUFFERED", payload = {buffer = tmp56, initialPosition = NONE, next = tmp57, position = 0}}
      return {tmp56, tmp57}
    end
    tmp55, tmp54 = nil, a
  end
  ::cont::
  do
    local contentsRev, f = tmp55, tmp54
    local exp = _L[130](f)
    local content = exp[1]
    local f1 = exp[2]
    local tmp56 = tmp51(content)
    if tmp56 == 0 then
    else
      tmp55 = {content, contentsRev}
      tmp54 = f1
      goto cont
    end
    do
      local tmp57 = revAppend(contentsRev, nil)
      local tmp58 = tmp49(tmp57)
      return {tmp58, f1}
    end
  end
end
_L[135] = function(a)
  local n, x
  do
    local f = a[1]
    n = a[2]
    if n < 0 then
      _raise(_Size, "bin-io.sml:208:42")
    end
    x = f[1]
    if x.tag == "READABLE" then
      _raise({tag = _L[9], payload = {cause = _L[11], ["function"] = "canInput", name = x.payload.name}}, "bin-io.sml:211:77")
    end
    if x.tag == "PRIM_READER" and x.payload.canInput.tag == "SOME" then
    else
      goto else1
    end
    do
      local tmp54 = x.payload.canInput.payload(nil)
      if tmp54 then
        return {tag = "SOME", payload = 1}
      else
        return NONE
      end
    end
  end
  ::else1::
  if x.tag == "PRIM_READER" and x.payload.canInput.tag == "NONE" then
    _raise({tag = _L[9], payload = {cause = _L[11], ["function"] = "canInput", name = x.payload.name}}, "bin-io.sml:216:107")
  end
  if x.tag == "BUFFERED" then
  elseif x.tag == "CLOSED" then
    return {tag = "SOME", payload = 0}
  else
    _raise(_Match, "bin-io.sml:210:42")
  end
  do
    local buffer = x.payload.buffer
    local position = x.payload.position
    local tmp54 = tmp51(buffer)
    local tmp55 = _Int_sub(tmp54, position)
    if n < tmp55 then
      return {tag = "SOME", payload = n}
    else
      return {tag = "SOME", payload = tmp55}
    end
  end
end
_L[136] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
      local readable = x.payload.readable
      local name = x.payload.name
      readable:close()
      a1[1] = {tag = "CLOSED", payload = name}
      return nil
    end
    if x.tag == "PRIM_READER" then
    elseif x.tag == "BUFFERED" then
      tmp54 = x.payload.next
      goto cont
    elseif x.tag == "CLOSED" then
      return nil
    else
      _raise(_Match, "bin-io.sml:219:34")
    end
    do
      local name = x.payload.name
      x.payload.close(nil)
      a1[1] = {tag = "CLOSED", payload = name}
      return nil
    end
  end
end
_L[137] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    local x = a1[1]
    if x.tag == "READABLE" then
      return not x.payload.readable:read(0)
    end
    if x.tag == "PRIM_READER" then
    elseif x.tag == "BUFFERED" then
      return false
    elseif x.tag == "CLOSED" then
      return true
    else
      _raise(_Match, "bin-io.sml:226:38")
    end
    do
      local rd = x.payload
      _L[129](rd, a1, x.payload.chunkSize)
      tmp54 = a1
      goto cont
    end
  end
end
_L[138] = function(stream, name, s)
  local tmp54, tmp55 = stream:write(s)
  if not tmp54 then
  else
    return nil
  end
  do
    local tmp56 = _Fail(tmp55)
    _raise({tag = _L[9], payload = {cause = tmp56, ["function"] = "output", name = name}}, "bin-io.sml:248:18")
  end
end
_L[139] = function(a)
  if a[1].tag == "LUA_WRITABLE" then
  else
    goto else1
  end
  do
    local writable = a[1].payload.writable
    local name = a[1].payload.name
    return _L[138](writable, name, a[2])
  end
  ::else1::
  if a[1].tag == "PRIM_WRITER" then
  else
    _raise(_Match, "bin-io.sml:317:9")
  end
  do
    local name, chunkSize, writeVec, buffer, content
    do
      name = a[1].payload.writer.name
      chunkSize = a[1].payload.writer.chunkSize
      writeVec = a[1].payload.writer.writeVec
      local buffer_mode = a[1].payload.buffer_mode
      buffer = a[1].payload.buffer
      content = a[2]
      if buffer_mode[1] == "NO_BUF" then
      else
        goto else2
      end
      do
        if writeVec.tag == "SOME" then
        elseif writeVec.tag == "NONE" then
          _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "output", name = name}}, "bin-io.sml:323:41")
        else
          _raise(_Match, "bin-io.sml:321:29")
        end
        do
          local writeVec1 = writeVec.payload
          local tmp54 = tmp45(content)
          writeVec1(tmp54)
          return nil
        end
      end
    end
    ::else2::
    local x = buffer[1]
    local tmp54 = foldl(function(a1)
      local z = a1[1]
      local acc = a1[2]
      local tmp55 = tmp51(z)
      return _Int_add(acc, tmp55)
    end)
    local tmp55 = tmp51(content)
    local tmp56 = tmp54(tmp55)
    local bufSize = tmp56(x)
    if bufSize >= chunkSize then
    else
      buffer[1] = {content, buffer[1]}
      return nil
    end
    do
      local tmp57 = revAppend({content, buffer[1]}, nil)
      local content_PRIME = tmp49(tmp57)
      if writeVec.tag == "SOME" then
      elseif writeVec.tag == "NONE" then
        _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "output", name = name}}, "bin-io.sml:332:37")
      else
        _raise(_Match, "bin-io.sml:330:25")
      end
      do
        local writeVec1 = writeVec.payload
        local tmp58 = tmp45(content_PRIME)
        writeVec1(tmp58)
        buffer[1] = nil
        return nil
      end
    end
  end
end
_L[140] = function(a)
  local stream = a[1]
  local tmp54 = tmp50({a[2], nil})
  return _L[139]({stream, tmp54})
end
_L[141] = function(name, writeVec, buffer)
  local tmp54 = revAppend(buffer[1], nil)
  local content = tmp49(tmp54)
  buffer[1] = nil
  local tmp55 = tmp51(content)
  if tmp55 > 0 then
  else
    return nil
  end
  do
    if writeVec.tag == "SOME" then
    elseif writeVec.tag == "NONE" then
      _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "flushOut", name = name}}, "bin-io.sml:353:30")
    else
      _raise(_Match, "bin-io.sml:351:18")
    end
    do
      local writeVec1 = writeVec.payload
      local tmp56 = tmp45(content)
      writeVec1(tmp56)
      return nil
    end
  end
end
_L[142] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:flush()
    return nil
  else
    if a.tag == "PRIM_WRITER" then
    else
      _raise(_Match, "bin-io.sml:357:9")
    end
    do
      local name = a.payload.writer.name
      local writeVec = a.payload.writer.writeVec
      return _L[141](name, writeVec, a.payload.buffer)
    end
  end
end
_L[143] = function(a)
  if a.tag == "LUA_WRITABLE" then
    a.payload.writable:close()
    return nil
  end
  if a.tag == "PRIM_WRITER" then
  else
    _raise(_Match, "bin-io.sml:360:9")
  end
  do
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local close = a.payload.writer.close
    _L[141](name, writeVec, a.payload.buffer)
    return close(nil)
  end
end
_L[144] = function(a)
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
  end
  if a[1].tag == "PRIM_WRITER" then
  else
    _raise(_Match, "bin-io.sml:365:9")
  end
  do
    local name = a[1].payload.writer.name
    local writeVec = a[1].payload.writer.writeVec
    local buffer_mode = a[1].payload.buffer_mode
    local buffer = a[1].payload.buffer
    local mode = a[2]
    local tmp54 = _L[17](mode, _L[14])
    if tmp54 then
    else
      buffer_mode[1] = mode
      return nil
    end
    do
      _L[141](name, writeVec, buffer)
      buffer_mode[1] = mode
      return nil
    end
  end
end
_L[145] = function(a)
  if a.tag == "LUA_WRITABLE" then
    return a.payload.buffer_mode[1]
  elseif a.tag == "PRIM_WRITER" then
    return a.payload.buffer_mode[1]
  else
    _raise(_Match, "bin-io.sml:376:9")
  end
end
_L[146] = function(a)
  if a[1].ioDesc.tag == "SOME" then
    local name = a[1].name
    local ioDesc = a[1].ioDesc.payload
    local mode = a[2]
    local tmp54 = _L[56][ioDesc]
    return {tag = "LUA_WRITABLE", payload = {buffer_mode = {mode}, name = name, writable = tmp54}}
  else
    local w = a[1]
    local tmp54 = {a[2]}
    return {tag = "PRIM_WRITER", payload = {buffer = {nil}, buffer_mode = tmp54, writer = w}}
  end
end
_L[147] = function(a)
  if a.tag == "LUA_WRITABLE" then
  else
    goto else1
  end
  do
    local writable = a.payload.writable
    local buffer_mode = a.payload.buffer_mode
    local name = a.payload.name
    writable:flush()
    local tmp54
    do
      local ioDesc = _L[59](writable)
      local tmp55 = {tag = "SOME", payload = function(slice)
        local tmp56 = tmp48(slice)
        _L[138](writable, name, tmp56)
        writable:flush()
        return tmp46(slice)
      end}
      local tmp56 = function(a1)
        _L[60](ioDesc)
        writable:close()
        return nil
      end
      tmp54 = _L[82]({block = NONE, canOutput = NONE, chunkSize = 1024, close = tmp56, endPos = NONE, getPos = NONE, ioDesc = {tag = "SOME", payload = ioDesc}, name = name, setPos = NONE, verifyPos = NONE, writeArr = NONE, writeArrNB = NONE, writeVec = tmp55, writeVecNB = NONE})
    end
    return {tmp54, buffer_mode[1]}
  end
  ::else1::
  if a.tag == "PRIM_WRITER" then
  else
    _raise(_Match, "bin-io.sml:383:9")
  end
  do
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local buffer_mode = a.payload.buffer_mode
    _L[141](name, writeVec, a.payload.buffer)
    return {writer, buffer_mode[1]}
  end
end
_L[148] = function(a)
  if a.tag == "LUA_WRITABLE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "getPosOut", name = a.payload.name}}, "bin-io.sml:390:50")
  end
  if a.tag == "PRIM_WRITER" then
  else
    _raise(_Match, "bin-io.sml:390:9")
  end
  do
    local writer = a.payload.writer
    local name = a.payload.writer.name
    local writeVec = a.payload.writer.writeVec
    local getPos = a.payload.writer.getPos
    local buffer_mode = a.payload.buffer_mode
    local buffer = a.payload.buffer
    if getPos.tag == "SOME" then
    elseif getPos.tag == "NONE" then
      _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "getPosOut", name = name}}, "bin-io.sml:396:23")
    else
      _raise(_Match, "bin-io.sml:392:11")
    end
    do
      local getPos1 = getPos.payload
      _L[141](name, writeVec, buffer)
      local tmp54 = getPos1(nil)
      return {buffer = buffer, buffer_mode = buffer_mode, pos = tmp54, writer = writer}
    end
  end
end
_L[149] = function(a)
  local writer = a.writer
  local name = a.writer.name
  local setPos = a.writer.setPos
  local buffer_mode = a.buffer_mode
  local buffer = a.buffer
  local pos = a.pos
  if setPos.tag == "SOME" then
  elseif setPos.tag == "NONE" then
    _raise({tag = _L[9], payload = {cause = _L[12], ["function"] = "setPosOut", name = name}}, "bin-io.sml:402:23")
  else
    _raise(_Match, "bin-io.sml:398:11")
  end
  do
    setPos.payload(pos)
    return {tag = "PRIM_WRITER", payload = {buffer = buffer, buffer_mode = buffer_mode, writer = writer}}
  end
end
_L[150] = _L[86](_L[135], _L[136], _L[143], _L[137], _L[127], function(a)
  return a.pos
end, _L[142], _L[145], _L[148], _L[126], _L[147], _L[130], _L[132], _L[134], _L[133], _L[125], _L[146], _L[139], _L[140], _L[144], _L[149])
_L[151] = function(a)
  local tmp54, tmp55 = tmp7(a, "rb")
  if tmp54 == nil then
  else
    local tmp56 = _L[150].mkInstream
    return tmp56({{tag = "READABLE", payload = {name = a, readable = tmp54}}})
  end
  do
    local tmp56 = _Fail(tmp55)
    _raise({tag = _L[9], payload = {cause = tmp56, ["function"] = "BinIO.openIn", name = a}}, "bin-io.sml:421:26")
  end
end
_L[152] = _L[150].closeIn
_L[153] = _L[150].inputAll
_L[154] = _ENV.arg
_Array_array(64, nil)
_L[155] = function(a)
  local status, exn = _handle(function()
    local tmp54 = _Int_add(a, a)
    if tmp54 < math_maxinteger then
    else
      return a
    end
    do
      return _L[155](tmp54)
    end
  end)
  if not status then
    if __exn_instanceof(exn, _Overflow_tag) then
      return a
    else
      _raise(exn, nil)
    end
  else
    return exn
  end
end
_L[155](65536)
_L[156] = _L[39]({256, 0xFF})
_L[45]({_L[156], 9, 0x41})
_L[45]({_L[156], 10, 0x41})
_L[45]({_L[156], 13, 0x41})
_L[45]({_L[156], 32, 0x41})
_L[157] = tmp33(function(a)
  local i = a[1]
  local c = a[2]
  return _L[45]({_L[156], c, i & 0xFF})
end)
_L[157]("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")
tmp53({256, function(i)
  return _L[43]({_L[156], i})
end})
_L[158] = _L[39]({256, 0xFF})
_L[159] = tmp33(function(a)
  local i = a[1]
  local c = a[2]
  return _L[45]({_L[158], c, i & 0xFF})
end)
_L[159]("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")
tmp53({256, function(i)
  return _L[43]({_L[158], i})
end})
tmp50(_list({n = 9, 0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF}))
tmp50(_list({n = 9, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x0}))
do
  _L[160], _L[161] = _handle(function()
    _Int_mul(8, _L[42])
    return nil
  end)
  if not _L[160] then
    if __exn_instanceof(_L[161], _Overflow_tag) then
    else
      _raise(_L[161], nil)
    end
  end
end
do
  _L[162] = _L[22](2147483646)
  if _L[162].tag == "ZERO" then
  elseif _L[162].tag == "POSITIVE" then
  elseif _L[162].tag == "NEGATIVE" then
  else
    _raise(_Match, "int-inf.sml:130:5")
  end
end
_L[163] = _L[22](2147483647)
do
  if _L[163].tag == "ZERO" then
    goto cont2
  end
  if _L[163].tag == "POSITIVE" then
  else
    goto else1
  end
  toRealAbs(_L[163].payload)
  goto cont2
  ::else1::
  if _L[163].tag == "NEGATIVE" then
  else
    _raise(_Match, "int-inf.sml:1082:5")
  end
  do
    _L[164] = toRealAbs(_L[163].payload)
  end
end
::cont2::
_id(32768 * 1.0)
_id(32768 * 1.0)
_Int_add(3, 48)
do
  _L[165] = 123 & 0x7FFFFFFF
  if not math_ult(0x1, tmp24) then
    _L[166] = 0x0
  else
    _L[166] = _L[165] << 0x1
  end
  _L[167], _L[168], _L[169], _L[170] = 48, nil, _L[166] + 0x1, 73256
  ::cont5::
  do
    local _L1 = {}
    _L1[1], _L1[2], _L1[3], _L1[4] = _L[167], _L[168], _L[169], _L[170]
    if _L1[1] == 0 then
      goto cont3
    end
    _L1[5], _L1[6], _L1[7], _L1[8] = 31, 0x0, _L1[3], _L1[4]
    ::cont6::
    do
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
      do
        _L2[8] = 0xBC8F * _Word_mod(_L2[3], 0xADC8)
        _L2[9] = 0xD47 * ((_L2[3] >> 3) // 5561)
        if math_ult(_L2[9], _L2[8]) then
          _L2[7] = _L2[8] - _L2[9]
        else
          _L2[7] = 0x7FFFFFFF - _L2[9] + _L2[8]
        end
      end
      if not math_ult(0x12, tmp24) then
        _L2[10] = 0x0
      else
        _L2[10] = _L2[4] << 0x12
      end
      _L2[11] = _L2[4] ~ _L2[10]
      if not math_ult(0xD, tmp24) then
        _L2[12] = 0x0
      else
        _L2[12] = _L2[11] >> 0xD
      end
      _L2[13] = _L2[11] ~ _L2[12]
      if not math_ult(0x1, tmp24) then
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
  end
end
::cont3::
_L[171] = {"InvalidUtf8"}
_L[172] = {tag = _L[171]}
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
_L[185] = function(a)
  if a == "" then
    return "\"\""
  end
  local tmp54 = tmp32(function(c)
    local tmp55
    do
      local tmp56
      if 65 <= c and c <= 90 then
        tmp55 = true
        goto cont
      else
        tmp56 = 97 <= c and c <= 122
      end
      tmp55 = tmp56 or 48 <= c and c <= 57
    end
    ::cont::
    return tmp55 or (c == 45 or c == 95)
  end)
  local tmp55 = tmp54(a)
  if tmp55 then
    return a
  else
    return "\"" .. a .. "\""
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
  end
  if math_ult(i, 0x800) then
    local tmp54
    if not math_ult(0x6, tmp24) then
      tmp54 = 0x0
    else
      tmp54 = i >> 0x6
    end
    local tmp55 = 0xC0 | tmp54
    local tmp56 = 0x80 | i & 0x3F
    local tmp57
    if tmp56 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp57 = tmp56
    end
    local tmp58
    if tmp57 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp57 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      tmp58 = tmp57
    end
    if tmp55 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    elseif tmp55 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp55 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      return {tmp58, {tmp55, accum}}
    end
  end
  if math_ult(i, 0x10000) then
    if not math_ult(i, 0xD800) and math_ult(i, 0xE000) then
      _raise({tag = _L[186], payload = _L[182]}, "parse_toml_util.sml:33:9")
    end
    local tmp54
    if not math_ult(0xC, tmp24) then
      tmp54 = 0x0
    else
      tmp54 = i >> 0xC
    end
    local tmp55 = 0xE0 | tmp54
    local tmp56
    if not math_ult(0x6, tmp24) then
      tmp56 = 0x0
    else
      tmp56 = i >> 0x6
    end
    local tmp57 = 0x80 | tmp56 & 0x3F
    local tmp58 = 0x80 | i & 0x3F
    local tmp59
    if tmp58 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp59 = tmp58
    end
    local tmp60
    if tmp59 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp59 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      tmp60 = tmp59
    end
    local tmp61
    if tmp57 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp61 = tmp57
    end
    local tmp62
    if tmp61 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp61 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      tmp62 = tmp61
    end
    local tmp63
    if tmp55 < 0x0 then
      _raise(_Overflow, "word-1.sml:52:39")
    else
      tmp63 = tmp55
    end
    if tmp63 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp63 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      return {tmp60, {tmp62, {tmp63, accum}}}
    end
  end
  if math_ult(i, 0x110000) then
    local tmp54, tmp55, tmp56, tmp57
    do
      local tmp58
      if not math_ult(0x12, tmp24) then
        tmp58 = 0x0
      else
        tmp58 = i >> 0x12
      end
      local tmp59 = 0xF0 | tmp58
      local tmp60
      if not math_ult(0xC, tmp24) then
        tmp60 = 0x0
      else
        tmp60 = i >> 0xC
      end
      local tmp61 = 0x80 | tmp60 & 0x3F
      local tmp62
      if not math_ult(0x6, tmp24) then
        tmp62 = 0x0
      else
        tmp62 = i >> 0x6
      end
      local tmp63 = 0x80 | tmp62 & 0x3F
      local tmp64 = 0x80 | i & 0x3F
      local tmp65
      if tmp64 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp65 = tmp64
      end
      if tmp65 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp65 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        tmp54 = tmp65
      end
      local tmp66
      if tmp63 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp66 = tmp63
      end
      if tmp66 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp66 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        tmp55 = tmp66
      end
      local tmp67
      if tmp61 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp67 = tmp61
      end
      if tmp67 < 0 then
        _raise(Chr, "char-1.sml:47:37")
      elseif tmp67 > 255 then
        _raise(Chr, "char-1.sml:47:37")
      else
        tmp56 = tmp67
      end
      if tmp59 < 0x0 then
        _raise(_Overflow, "word-1.sml:52:39")
      else
        tmp57 = tmp59
      end
    end
    if tmp57 < 0 then
      _raise(Chr, "char-1.sml:47:37")
    elseif tmp57 > 255 then
      _raise(Chr, "char-1.sml:47:37")
    else
      return {tmp54, {tmp55, {tmp56, {tmp57, accum}}}}
    end
  else
    _raise({tag = _L[186], payload = _L[182]}, "parse_toml_util.sml:54:7")
  end
end
_L[188] = function(a)
  local tmp54
  do
    local tmp55 = scanString(_L[8])
    tmp54 = tmp55(a)
  end
  if tmp54.tag == "SOME" then
    return {tag = "FLOAT", payload = tmp54.payload}
  elseif tmp54.tag == "NONE" then
    _raise(Option, "option.sml:24:18")
  else
    _raise(_Match, "option.sml:23:5")
  end
end
_L[189] = function(x, y)
  local tmp54
  if not math_ult(y, tmp24) then
    tmp54 = 0x0
  else
    tmp54 = x << y & 0xFFFFFFFF
  end
  local tmp55 = 0x20 - y
  if not math_ult(tmp55, tmp24) then
    return tmp54
  else
    return tmp54 | x >> tmp55
  end
end
_L[190] = function(a)
  local tmp54
  if not math_ult(0x4, tmp24) then
    tmp54 = 0x0
  else
    tmp54 = a >> 0x4
  end
  local tmp55
  if tmp54 < 0x0 then
    _raise(_Overflow, "word-1.sml:52:39")
  else
    tmp55 = sub1({"0123456789abcdef", tmp54})
  end
  local tmp56 = a & 0xF
  local tmp57
  if tmp56 < 0x0 then
    _raise(_Overflow, "word-1.sml:52:39")
  else
    tmp57 = sub1({"0123456789abcdef", tmp56})
  end
  return implode({tmp55, {tmp57, nil}})
end
_L[191] = function(a)
  local content8
  do
    local origLen = tmp51(a)
    local tmp54 = tmp51(a)
    local tmp55 = tmp54 % 64
    local tmp56 = tmp50({0x80, nil})
    local tmp57
    if tmp55 < 56 then
      tmp57 = _Int_sub(63, tmp55)
    else
      tmp57 = _Int_sub(127, tmp55)
    end
    local tmp58 = tmp53({tmp57, function(a1)
      return 0x0
    end})
    local padded = tmp49({a, {tmp56, {tmp58, nil}}})
    local paddedLen = tmp51(padded)
    content8 = _L[44]({paddedLen, function(i)
      return tmp52({padded, i})
    end})
    local tmp59 = _Int_sub(paddedLen // 8, 1)
    local tmp60 = _Int_mul(8, origLen)
    do
      local ii
      do
        ii = _L[48](content8, tmp59, 8)
        _L[47]({content8, ii, tmp60 & 0xFF})
        local tmp61 = _Int_add(ii, 1)
        local tmp62
        if not math_ult(0x8, tmp24) then
          tmp62 = 0x0
        else
          tmp62 = tmp60 >> 0x8
        end
        _L[47]({content8, tmp61, tmp62 & 0xFF})
        local tmp63 = _Int_add(ii, 2)
        local tmp64
        if not math_ult(0x10, tmp24) then
          tmp64 = 0x0
        else
          tmp64 = tmp60 >> 0x10
        end
        _L[47]({content8, tmp63, tmp64 & 0xFF})
        local tmp65 = _Int_add(ii, 3)
        local tmp66
        if not math_ult(0x18, tmp24) then
          tmp66 = 0x0
        else
          tmp66 = tmp60 >> 0x18
        end
        _L[47]({content8, tmp65, tmp66 & 0xFF})
        local tmp67 = _Int_add(ii, 4)
        local tmp68
        if not math_ult(0x20, tmp24) then
          tmp68 = 0x0
        else
          tmp68 = tmp60 >> 0x20
        end
        _L[47]({content8, tmp67, tmp68 & 0xFF})
        local tmp69 = _Int_add(ii, 5)
        local tmp70
        if not math_ult(0x28, tmp24) then
          tmp70 = 0x0
        else
          tmp70 = tmp60 >> 0x28
        end
        _L[47]({content8, tmp69, tmp70 & 0xFF})
      end
      local tmp61 = _Int_add(ii, 6)
      local tmp62
      if not math_ult(0x30, tmp24) then
        tmp62 = 0x0
      else
        tmp62 = tmp60 >> 0x30
      end
      _L[47]({content8, tmp61, tmp62 & 0xFF})
      local tmp63 = _Int_add(ii, 7)
      local tmp64
      if not math_ult(0x38, tmp24) then
        tmp64 = 0x0
      else
        tmp64 = tmp60 >> 0x38
      end
      _L[47]({content8, tmp63, tmp64 & 0xFF})
    end
  end
  local content32, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59
  do
    local tmp60 = _L[41](content8)
    local tmp61 = tmp60 // 4
    content32 = _VectorOrArray_tabulate({tmp61, function(i)
      local ii = _L[48](content8, i, 4)
      local x0 = _L[46]({content8, ii})
      local x1 = _L[46]({content8, _Int_add(ii, 1)})
      local x2 = _L[46]({content8, _Int_add(ii, 2)})
      local x3 = _L[46]({content8, _Int_add(ii, 3)})
      local tmp62
      if not math_ult(0x18, tmp24) then
        tmp62 = 0x0
      else
        tmp62 = x3 << 0x18
      end
      local tmp63
      if not math_ult(0x10, tmp24) then
        tmp63 = 0x0
      else
        tmp63 = x2 << 0x10
      end
      local tmp64 = tmp62 | tmp63
      local tmp65
      if not math_ult(0x8, tmp24) then
        tmp65 = 0x0
      else
        tmp65 = x1 << 0x8
      end
      return (tmp64 | tmp65 | x0) & 0xFFFFFFFF
    end})
    tmp54 = content32.n // 16
    tmp59, tmp58, tmp57, tmp56, tmp55 = 0, 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476
  end
  ::cont::
  do
    local d, c, b, b_PRIME, c_PRIME, d_PRIME, tmp60, tmp61
    do
      local i, a1
      i, a1, b, c, d = tmp59, tmp58, tmp57, tmp56, tmp55
      if i >= tmp54 then
        return {a1, b, c, d}
      end
      local tmp62 = _Int_mul(i, 16)
      local X = _L[19](content32, tmp62, {tag = "SOME", payload = 16})
      local tmp63 = {a1, b, c, d}
      local tmp64
      do
        local ABCD, DABC, CDAB, BCDA, tmp65
        do
          ABCD = function(k, t)
            return function(a2)
              local a3 = a2[1]
              local b1 = a2[2]
              local c1 = a2[3]
              local d1 = a2[4]
              local tmp66 = a3 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
              local tmp67
              do
                local base = X.base
                tmp67 = _L[18](base, X.length, X.start, k)
              end
              local tmp68 = _L[189]((tmp66 + tmp67 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x7)
              return {b1 + tmp68 & 0xFFFFFFFF, b1, c1, d1}
            end
          end
          DABC = function(k, t)
            return function(a2)
              local b1 = a2[1]
              local c1 = a2[2]
              local d1 = a2[3]
              local a3 = a2[4]
              local tmp66 = a3 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
              local tmp67
              do
                local base = X.base
                tmp67 = _L[18](base, X.length, X.start, k)
              end
              local tmp68 = _L[189]((tmp66 + tmp67 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xC)
              return {b1, c1, d1, b1 + tmp68 & 0xFFFFFFFF}
            end
          end
          CDAB = function(k, t)
            return function(a2)
              local c1 = a2[1]
              local d1 = a2[2]
              local a3 = a2[3]
              local b1 = a2[4]
              local tmp66 = a3 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
              local tmp67
              do
                local base = X.base
                tmp67 = _L[18](base, X.length, X.start, k)
              end
              local tmp68 = _L[189]((tmp66 + tmp67 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x11)
              return {c1, d1, b1 + tmp68 & 0xFFFFFFFF, b1}
            end
          end
          BCDA = function(k, t)
            return function(a2)
              local d1 = a2[1]
              local a3 = a2[2]
              local b1 = a2[3]
              local c1 = a2[4]
              local tmp66 = a3 + (b1 & c1 | ~ b1 & 0xFFFFFFFF & d1) & 0xFFFFFFFF
              local tmp67
              do
                local base = X.base
                tmp67 = _L[18](base, X.length, X.start, k)
              end
              local tmp68 = _L[189]((tmp66 + tmp67 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x16)
              return {d1, b1 + tmp68 & 0xFFFFFFFF, b1, c1}
            end
          end
          local tmp66 = ABCD(0, 0xD76AA478)
          local tmp67 = tmp66(tmp63)
          local tmp68 = DABC(1, 0xE8C7B756)
          local tmp69 = tmp68(tmp67)
          local tmp70 = CDAB(2, 0x242070DB)
          local tmp71 = tmp70(tmp69)
          local tmp72 = BCDA(3, 0xC1BDCEEE)
          local tmp73 = tmp72(tmp71)
          local tmp74 = ABCD(4, 0xF57C0FAF)
          local tmp75 = tmp74(tmp73)
          local tmp76 = DABC(5, 0x4787C62A)
          tmp65 = tmp76(tmp75)
        end
        local tmp66
        do
          local tmp67 = CDAB(6, 0xA8304613)
          local tmp68 = tmp67(tmp65)
          local tmp69 = BCDA(7, 0xFD469501)
          local tmp70 = tmp69(tmp68)
          local tmp71 = ABCD(8, 0x698098D8)
          local tmp72 = tmp71(tmp70)
          local tmp73 = DABC(9, 0x8B44F7AF)
          local tmp74 = tmp73(tmp72)
          local tmp75 = CDAB(10, 0xFFFF5BB1)
          local tmp76 = tmp75(tmp74)
          local tmp77 = BCDA(11, 0x895CD7BE)
          tmp66 = tmp77(tmp76)
        end
        local tmp67 = ABCD(12, 0x6B901122)
        local tmp68 = tmp67(tmp66)
        local tmp69 = DABC(13, 0xFD987193)
        local tmp70 = tmp69(tmp68)
        local tmp71 = CDAB(14, 0xA679438E)
        local tmp72 = tmp71(tmp70)
        local tmp73 = BCDA(15, 0x49B40821)
        tmp64 = tmp73(tmp72)
      end
      local tmp65
      do
        local ABCD, DABC, CDAB, BCDA, tmp66
        do
          ABCD = function(k, t)
            return function(a2)
              local a3 = a2[1]
              local b1 = a2[2]
              local c1 = a2[3]
              local d1 = a2[4]
              local tmp67 = a3 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp68
              do
                local base = X.base
                tmp68 = _L[18](base, X.length, X.start, k)
              end
              local tmp69 = _L[189]((tmp67 + tmp68 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x5)
              return {b1 + tmp69 & 0xFFFFFFFF, b1, c1, d1}
            end
          end
          DABC = function(k, t)
            return function(a2)
              local b1 = a2[1]
              local c1 = a2[2]
              local d1 = a2[3]
              local a3 = a2[4]
              local tmp67 = a3 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp68
              do
                local base = X.base
                tmp68 = _L[18](base, X.length, X.start, k)
              end
              local tmp69 = _L[189]((tmp67 + tmp68 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x9)
              return {b1, c1, d1, b1 + tmp69 & 0xFFFFFFFF}
            end
          end
          CDAB = function(k, t)
            return function(a2)
              local c1 = a2[1]
              local d1 = a2[2]
              local a3 = a2[3]
              local b1 = a2[4]
              local tmp67 = a3 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp68
              do
                local base = X.base
                tmp68 = _L[18](base, X.length, X.start, k)
              end
              local tmp69 = _L[189]((tmp67 + tmp68 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xE)
              return {c1, d1, b1 + tmp69 & 0xFFFFFFFF, b1}
            end
          end
          BCDA = function(k, t)
            return function(a2)
              local d1 = a2[1]
              local a3 = a2[2]
              local b1 = a2[3]
              local c1 = a2[4]
              local tmp67 = a3 + (b1 & d1 | c1 & (~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp68
              do
                local base = X.base
                tmp68 = _L[18](base, X.length, X.start, k)
              end
              local tmp69 = _L[189]((tmp67 + tmp68 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x14)
              return {d1, b1 + tmp69 & 0xFFFFFFFF, b1, c1}
            end
          end
          local tmp67 = ABCD(1, 0xF61E2562)
          local tmp68 = tmp67(tmp64)
          local tmp69 = DABC(6, 0xC040B340)
          local tmp70 = tmp69(tmp68)
          local tmp71 = CDAB(11, 0x265E5A51)
          local tmp72 = tmp71(tmp70)
          local tmp73 = BCDA(0, 0xE9B6C7AA)
          local tmp74 = tmp73(tmp72)
          local tmp75 = ABCD(5, 0xD62F105D)
          local tmp76 = tmp75(tmp74)
          local tmp77 = DABC(10, 0x2441453)
          tmp66 = tmp77(tmp76)
        end
        local tmp67
        do
          local tmp68 = CDAB(15, 0xD8A1E681)
          local tmp69 = tmp68(tmp66)
          local tmp70 = BCDA(4, 0xE7D3FBC8)
          local tmp71 = tmp70(tmp69)
          local tmp72 = ABCD(9, 0x21E1CDE6)
          local tmp73 = tmp72(tmp71)
          local tmp74 = DABC(14, 0xC33707D6)
          local tmp75 = tmp74(tmp73)
          local tmp76 = CDAB(3, 0xF4D50D87)
          local tmp77 = tmp76(tmp75)
          local tmp78 = BCDA(8, 0x455A14ED)
          tmp67 = tmp78(tmp77)
        end
        local tmp68 = ABCD(13, 0xA9E3E905)
        local tmp69 = tmp68(tmp67)
        local tmp70 = DABC(2, 0xFCEFA3F8)
        local tmp71 = tmp70(tmp69)
        local tmp72 = CDAB(7, 0x676F02D9)
        local tmp73 = tmp72(tmp71)
        local tmp74 = BCDA(12, 0x8D2A4C8A)
        tmp65 = tmp74(tmp73)
      end
      local tmp66
      do
        local ABCD, DABC, CDAB, BCDA, tmp67
        do
          ABCD = function(k, t)
            return function(a2)
              local a3 = a2[1]
              local b1 = a2[2]
              local c1 = a2[3]
              local d1 = a2[4]
              local tmp68 = a3 + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x4)
              return {b1 + tmp70 & 0xFFFFFFFF, b1, c1, d1}
            end
          end
          DABC = function(k, t)
            return function(a2)
              local b1 = a2[1]
              local c1 = a2[2]
              local d1 = a2[3]
              local tmp68 = a2[4] + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xB)
              return {b1, c1, d1, b1 + tmp70 & 0xFFFFFFFF}
            end
          end
          CDAB = function(k, t)
            return function(a2)
              local c1 = a2[1]
              local d1 = a2[2]
              local a3 = a2[3]
              local b1 = a2[4]
              local tmp68 = a3 + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x10)
              return {c1, d1, b1 + tmp70 & 0xFFFFFFFF, b1}
            end
          end
          BCDA = function(k, t)
            return function(a2)
              local d1 = a2[1]
              local a3 = a2[2]
              local b1 = a2[3]
              local c1 = a2[4]
              local tmp68 = a3 + (b1 ~ (c1 ~ d1)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x17)
              return {d1, b1 + tmp70 & 0xFFFFFFFF, b1, c1}
            end
          end
          local tmp68 = ABCD(5, 0xFFFA3942)
          local tmp69 = tmp68(tmp65)
          local tmp70 = DABC(8, 0x8771F681)
          local tmp71 = tmp70(tmp69)
          local tmp72 = CDAB(11, 0x6D9D6122)
          local tmp73 = tmp72(tmp71)
          local tmp74 = BCDA(14, 0xFDE5380C)
          local tmp75 = tmp74(tmp73)
          local tmp76 = ABCD(1, 0xA4BEEA44)
          local tmp77 = tmp76(tmp75)
          local tmp78 = DABC(4, 0x4BDECFA9)
          tmp67 = tmp78(tmp77)
        end
        local tmp68
        do
          local tmp69 = CDAB(7, 0xF6BB4B60)
          local tmp70 = tmp69(tmp67)
          local tmp71 = BCDA(10, 0xBEBFBC70)
          local tmp72 = tmp71(tmp70)
          local tmp73 = ABCD(13, 0x289B7EC6)
          local tmp74 = tmp73(tmp72)
          local tmp75 = DABC(0, 0xEAA127FA)
          local tmp76 = tmp75(tmp74)
          local tmp77 = CDAB(3, 0xD4EF3085)
          local tmp78 = tmp77(tmp76)
          local tmp79 = BCDA(6, 0x4881D05)
          tmp68 = tmp79(tmp78)
        end
        local tmp69 = ABCD(9, 0xD9D4D039)
        local tmp70 = tmp69(tmp68)
        local tmp71 = DABC(12, 0xE6DB99E5)
        local tmp72 = tmp71(tmp70)
        local tmp73 = CDAB(15, 0x1FA27CF8)
        local tmp74 = tmp73(tmp72)
        local tmp75 = BCDA(2, 0xC4AC5665)
        tmp66 = tmp75(tmp74)
      end
      local exp
      do
        local ABCD, DABC, CDAB, BCDA, tmp67
        do
          ABCD = function(k, t)
            return function(a2)
              local a3 = a2[1]
              local b1 = a2[2]
              local c1 = a2[3]
              local d1 = a2[4]
              local tmp68 = a3 + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x6)
              return {b1 + tmp70 & 0xFFFFFFFF, b1, c1, d1}
            end
          end
          DABC = function(k, t)
            return function(a2)
              local b1 = a2[1]
              local c1 = a2[2]
              local d1 = a2[3]
              local tmp68 = a2[4] + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xA)
              return {b1, c1, d1, b1 + tmp70 & 0xFFFFFFFF}
            end
          end
          CDAB = function(k, t)
            return function(a2)
              local c1 = a2[1]
              local d1 = a2[2]
              local a3 = a2[3]
              local b1 = a2[4]
              local tmp68 = a3 + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0xF)
              return {c1, d1, b1 + tmp70 & 0xFFFFFFFF, b1}
            end
          end
          BCDA = function(k, t)
            return function(a2)
              local d1 = a2[1]
              local a3 = a2[2]
              local b1 = a2[3]
              local c1 = a2[4]
              local tmp68 = a3 + (c1 ~ (b1 | ~ d1 & 0xFFFFFFFF)) & 0xFFFFFFFF
              local tmp69
              do
                local base = X.base
                tmp69 = _L[18](base, X.length, X.start, k)
              end
              local tmp70 = _L[189]((tmp68 + tmp69 & 0xFFFFFFFF) + t & 0xFFFFFFFF, 0x15)
              return {d1, b1 + tmp70 & 0xFFFFFFFF, b1, c1}
            end
          end
          local tmp68 = ABCD(0, 0xF4292244)
          local tmp69 = tmp68(tmp66)
          local tmp70 = DABC(7, 0x432AFF97)
          local tmp71 = tmp70(tmp69)
          local tmp72 = CDAB(14, 0xAB9423A7)
          local tmp73 = tmp72(tmp71)
          local tmp74 = BCDA(5, 0xFC93A039)
          local tmp75 = tmp74(tmp73)
          local tmp76 = ABCD(12, 0x655B59C3)
          local tmp77 = tmp76(tmp75)
          local tmp78 = DABC(3, 0x8F0CCC92)
          tmp67 = tmp78(tmp77)
        end
        local tmp68
        do
          local tmp69 = CDAB(10, 0xFFEFF47D)
          local tmp70 = tmp69(tmp67)
          local tmp71 = BCDA(1, 0x85845DD1)
          local tmp72 = tmp71(tmp70)
          local tmp73 = ABCD(8, 0x6FA87E4F)
          local tmp74 = tmp73(tmp72)
          local tmp75 = DABC(15, 0xFE2CE6E0)
          local tmp76 = tmp75(tmp74)
          local tmp77 = CDAB(6, 0xA3014314)
          local tmp78 = tmp77(tmp76)
          local tmp79 = BCDA(13, 0x4E0811A1)
          tmp68 = tmp79(tmp78)
        end
        local tmp69 = ABCD(4, 0xF7537E82)
        local tmp70 = tmp69(tmp68)
        local tmp71 = DABC(11, 0xBD3AF235)
        local tmp72 = tmp71(tmp70)
        local tmp73 = CDAB(2, 0x2AD7D2BB)
        local tmp74 = tmp73(tmp72)
        local tmp75 = BCDA(9, 0xEB86D391)
        exp = tmp75(tmp74)
      end
      local a_PRIME = exp[1]
      b_PRIME = exp[2]
      c_PRIME = exp[3]
      d_PRIME = exp[4]
      tmp60 = _Int_add(i, 1)
      tmp61 = a1 + a_PRIME & 0xFFFFFFFF
    end
    local tmp62 = b + b_PRIME & 0xFFFFFFFF
    local tmp63 = c + c_PRIME & 0xFFFFFFFF
    tmp59 = tmp60
    tmp58 = tmp61
    tmp57 = tmp62
    tmp56 = tmp63
    tmp55 = d + d_PRIME & 0xFFFFFFFF
    goto cont
  end
end
_L[192] = function(a)
  local tmp54 = _L[190](a & 0xFF)
  local tmp55
  if not math_ult(0x8, tmp24) then
    tmp55 = 0x0
  else
    tmp55 = a >> 0x8
  end
  local tmp56 = _L[190](tmp55 & 0xFF)
  local tmp57
  if not math_ult(0x10, tmp24) then
    tmp57 = 0x0
  else
    tmp57 = a >> 0x10
  end
  local tmp58 = _L[190](tmp57 & 0xFF)
  local tmp59
  if not math_ult(0x18, tmp24) then
    tmp59 = 0x0
  else
    tmp59 = a >> 0x18
  end
  local tmp60 = _L[190](tmp59)
  return table_concat({n = 4, tmp54, tmp56, tmp58, tmp60})
end
_L[193] = function(a, b, c, d)
  local tmp54 = _L[192](a)
  local tmp55 = _L[192](b)
  local tmp56 = tmp54 .. tmp55
  local tmp57 = _L[192](c)
  local tmp58 = tmp56 .. tmp57
  local tmp59 = _L[192](d)
  return tmp58 .. tmp59
end
_L[194] = "R"
_L[195] = "B"
_L[196] = {tag = "E"}
_L[197] = {0, _L[196]}
_L[198] = function(tmp54, x)
  local nItems = tmp54[1]
  local m = tmp54[2]
  local tmp55 = {nItems}
  local function ins(a)
    if a.tag == "E" then
      tmp55[1] = _Int_add(nItems, 1)
      return {tag = "T", payload = {_L[194], _L[196], x, _L[196]}}
    end
    if a.tag == "T" then
    else
      _raise(_Match, "redblack-set-fn.sml:64:8")
    end
    do
      local color = a.payload[1]
      local a1 = a.payload[2]
      local y = a.payload[3]
      local b = a.payload[4]
      local exp
      if x == y then
        exp = EQUAL
      elseif x < y then
        exp = LESS
      else
        exp = GREATER
      end
      if exp == "LESS" then
      else
        goto else1
      end
      do
        if a1.tag == "T" and a1.payload[1] == "R" then
        else
          goto else2
        end
        do
          local c = a1.payload[2]
          local z = a1.payload[3]
          local d = a1.payload[4]
          local exp1
          if x == z then
            exp1 = EQUAL
          elseif x < z then
            exp1 = LESS
          else
            exp1 = GREATER
          end
          if exp1 == "LESS" then
          else
            goto else3
          end
          do
            local exp2 = ins(c)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local w = exp2.payload[3]
              local tmp56 = {tag = "T", payload = {_L[195], e, w, exp2.payload[4]}}
              return {tag = "T", payload = {_L[194], tmp56, z, {tag = "T", payload = {_L[195], d, y, b}}}}
            else
              return {tag = "T", payload = {_L[195], {tag = "T", payload = {_L[194], exp2, z, d}}, y, b}}
            end
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, {tag = "T", payload = {_L[194], c, x, d}}, y, b}}
          end
          if exp1 == "GREATER" then
          else
            _raise(_Match, "redblack-set-fn.sml:67:31")
          end
          do
            local exp2 = ins(d)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local w = exp2.payload[3]
              local f = exp2.payload[4]
              local tmp56 = {tag = "T", payload = {_L[195], c, z, e}}
              return {tag = "T", payload = {_L[194], tmp56, w, {tag = "T", payload = {_L[195], f, y, b}}}}
            else
              return {tag = "T", payload = {_L[195], {tag = "T", payload = {_L[194], c, z, exp2}}, y, b}}
            end
          end
        end
        ::else2::
        local tmp56 = ins(a1)
        return {tag = "T", payload = {_L[195], tmp56, y, b}}
      end
      ::else1::
      if exp == "EQUAL" then
        return {tag = "T", payload = {color, a1, x, b}}
      end
      if exp == "GREATER" then
      else
        _raise(_Match, "redblack-set-fn.sml:65:47")
      end
      do
        if b.tag == "T" and b.payload[1] == "R" then
        else
          goto else2
        end
        do
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
          else
            goto else3
          end
          do
            local exp2 = ins(c)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local w = exp2.payload[3]
              local f = exp2.payload[4]
              local tmp56 = {tag = "T", payload = {_L[195], a1, y, e}}
              return {tag = "T", payload = {_L[194], tmp56, w, {tag = "T", payload = {_L[195], f, z, d}}}}
            else
              return {tag = "T", payload = {_L[195], a1, y, {tag = "T", payload = {_L[194], exp2, z, d}}}}
            end
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, a1, y, {tag = "T", payload = {_L[194], c, x, d}}}}
          end
          if exp1 == "GREATER" then
          else
            _raise(_Match, "redblack-set-fn.sml:82:31")
          end
          do
            local exp2 = ins(d)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local w = exp2.payload[3]
              local f = exp2.payload[4]
              local tmp56 = {tag = "T", payload = {_L[195], a1, y, c}}
              return {tag = "T", payload = {_L[194], tmp56, z, {tag = "T", payload = {_L[195], e, w, f}}}}
            else
              return {tag = "T", payload = {_L[195], a1, y, {tag = "T", payload = {_L[194], c, z, exp2}}}}
            end
          end
        end
        ::else2::
        local tmp56 = ins(b)
        return {tag = "T", payload = {_L[195], a1, y, tmp56}}
      end
    end
  end
  local exp = ins(m)
  if exp.tag == "T" then
    local a = exp.payload[2]
    local y = exp.payload[3]
    local b = exp.payload[4]
    local x1 = tmp55[1]
    return {x1, {tag = "T", payload = {_L[195], a, y, b}}}
  else
    _raise(_Bind, "redblack-set-fn.sml:96:8")
  end
end
_L[199] = function(tmp54, k)
  local tmp55 = tmp54[2]
  ::cont::
  do
    local a = tmp55
    if a.tag == "E" then
      return false
    end
    if a.tag == "T" then
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
        tmp55 = a1
        goto cont
      elseif exp == "EQUAL" then
        return true
      elseif exp == "GREATER" then
        tmp55 = b
        goto cont
      else
        _raise(_Match, "redblack-set-fn.sml:226:33")
      end
    else
      _raise(_Match, "redblack-set-fn.sml:225:8")
    end
  end
end
_L[200] = "R"
_L[201] = "B"
_L[202] = {tag = "E"}
_L[203] = {0, _L[202]}
_L[204] = function(tmp54, xk, x)
  local nItems = tmp54[1]
  local m = tmp54[2]
  local tmp55 = {nItems}
  local function ins(a)
    if a.tag == "E" then
      tmp55[1] = _Int_add(nItems, 1)
      return {tag = "T", payload = {_L[200], _L[202], xk, x, _L[202]}}
    end
    if a.tag == "T" then
    else
      _raise(_Match, "redblack-map-fn.sml:46:8")
    end
    do
      local color = a.payload[1]
      local a1 = a.payload[2]
      local yk = a.payload[3]
      local y = a.payload[4]
      local b = a.payload[5]
      local exp
      if xk == yk then
        exp = EQUAL
      elseif xk < yk then
        exp = LESS
      else
        exp = GREATER
      end
      if exp == "LESS" then
      else
        goto else1
      end
      do
        if a1.tag == "T" and a1.payload[1] == "R" then
        else
          goto else2
        end
        do
          local c = a1.payload[2]
          local zk = a1.payload[3]
          local z = a1.payload[4]
          local d = a1.payload[5]
          local exp1
          if xk == zk then
            exp1 = EQUAL
          elseif xk < zk then
            exp1 = LESS
          else
            exp1 = GREATER
          end
          if exp1 == "LESS" then
          else
            goto else3
          end
          do
            local exp2 = ins(c)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local wk = exp2.payload[3]
              local w = exp2.payload[4]
              local tmp56 = {tag = "T", payload = {_L[201], e, wk, w, exp2.payload[5]}}
              return {tag = "T", payload = {_L[200], tmp56, zk, z, {tag = "T", payload = {_L[201], d, yk, y, b}}}}
            else
              return {tag = "T", payload = {_L[201], {tag = "T", payload = {_L[200], exp2, zk, z, d}}, yk, y, b}}
            end
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, {tag = "T", payload = {_L[200], c, xk, x, d}}, yk, y, b}}
          end
          if exp1 == "GREATER" then
          else
            _raise(_Match, "redblack-map-fn.sml:49:35")
          end
          do
            local exp2 = ins(d)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local wk = exp2.payload[3]
              local w = exp2.payload[4]
              local f = exp2.payload[5]
              local tmp56 = {tag = "T", payload = {_L[201], c, zk, z, e}}
              return {tag = "T", payload = {_L[200], tmp56, wk, w, {tag = "T", payload = {_L[201], f, yk, y, b}}}}
            else
              return {tag = "T", payload = {_L[201], {tag = "T", payload = {_L[200], c, zk, z, exp2}}, yk, y, b}}
            end
          end
        end
        ::else2::
        local tmp56 = ins(a1)
        return {tag = "T", payload = {_L[201], tmp56, yk, y, b}}
      end
      ::else1::
      if exp == "EQUAL" then
        return {tag = "T", payload = {color, a1, xk, x, b}}
      end
      if exp == "GREATER" then
      else
        _raise(_Match, "redblack-map-fn.sml:47:51")
      end
      do
        if b.tag == "T" and b.payload[1] == "R" then
        else
          goto else2
        end
        do
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
          else
            goto else3
          end
          do
            local exp2 = ins(c)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local wk = exp2.payload[3]
              local w = exp2.payload[4]
              local f = exp2.payload[5]
              local tmp56 = {tag = "T", payload = {_L[201], a1, yk, y, e}}
              return {tag = "T", payload = {_L[200], tmp56, wk, w, {tag = "T", payload = {_L[201], f, zk, z, d}}}}
            else
              return {tag = "T", payload = {_L[201], a1, yk, y, {tag = "T", payload = {_L[200], exp2, zk, z, d}}}}
            end
          end
          ::else3::
          if exp1 == "EQUAL" then
            return {tag = "T", payload = {color, a1, yk, y, {tag = "T", payload = {_L[200], c, xk, x, d}}}}
          end
          if exp1 == "GREATER" then
          else
            _raise(_Match, "redblack-map-fn.sml:66:35")
          end
          do
            local exp2 = ins(d)
            if exp2.tag == "T" and exp2.payload[1] == "R" then
              local e = exp2.payload[2]
              local wk = exp2.payload[3]
              local w = exp2.payload[4]
              local f = exp2.payload[5]
              local tmp56 = {tag = "T", payload = {_L[201], a1, yk, y, c}}
              return {tag = "T", payload = {_L[200], tmp56, zk, z, {tag = "T", payload = {_L[201], e, wk, w, f}}}}
            else
              return {tag = "T", payload = {_L[201], a1, yk, y, {tag = "T", payload = {_L[200], c, zk, z, exp2}}}}
            end
          end
        end
        ::else2::
        local tmp56 = ins(b)
        return {tag = "T", payload = {_L[201], a1, yk, y, tmp56}}
      end
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
  local x1 = tmp55[1]
  return {x1, {tag = "T", payload = {_L[201], a, yk, y, b}}}
end
_L[205] = function(tmp54, k)
  local tmp55 = tmp54[2]
  ::cont::
  do
    local a = tmp55
    if a.tag == "E" then
      return NONE
    end
    if a.tag == "T" then
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
        tmp55 = a1
        goto cont
      elseif exp == "EQUAL" then
        return {tag = "SOME", payload = y}
      elseif exp == "GREATER" then
        tmp55 = b
        goto cont
      else
        _raise(_Match, "redblack-map-fn.sml:155:37")
      end
    else
      _raise(_Match, "redblack-map-fn.sml:154:8")
    end
  end
end
_L[206] = function(a)
  local function mapf(a1)
    if a1.tag == "E" then
      return _L[202]
    end
    if a1.tag == "T" then
    else
      _raise(_Match, "redblack-map-fn.sml:551:8")
    end
    do
      local color = a1.payload[1]
      local a2 = a1.payload[2]
      local xk = a1.payload[3]
      local x = a1.payload[4]
      local b = a1.payload[5]
      local tmp54 = mapf(a2)
      local tmp55 = a(x)
      local tmp56 = mapf(b)
      return {tag = "T", payload = {color, tmp54, xk, tmp55, tmp56}}
    end
  end
  return function(a1)
    local n = a1[1]
    local tmp54 = mapf(a1[2])
    return {n, tmp54}
  end
end
_L[207] = tmp3("texrunner.shellutil")
_L[208] = _L[207].escape
_L[209] = _L[207].has_command
_L[210] = tmp3("texrunner.pathutil")
_L[211] = _L[210].basename
_L[212] = _L[210].dirname
_L[213] = _L[210].trimext
_L[214] = _L[210].ext
_L[215] = function(cwd, path)
  if cwd.tag == "SOME" then
    local cwd1 = cwd.payload
    return _L[210].abspath(path, cwd1)
  elseif cwd.tag == "NONE" then
    return _L[210].abspath(path)
  else
    _raise(_Match, "path-util.sml:22:7")
  end
end
_L[216] = tmp3("lfs")
_L[217] = tmp3("texrunner.fsutil")
_L[218] = _L[217].isfile
_L[219] = _L[217].isdir
_L[220] = tmp1.type == "windows"
_L[221] = tmp1.setenv
_L[222] = "BATCHMODE"
_L[223] = "NONSTOPMODE"
_L[224] = "SCROLLMODE"
_L[225] = "ERRORSTOPMODE"
_L[226] = "ALLOWED"
_L[227] = "RESTRICTED"
_L[228] = "FORBIDDEN"
_L[229] = function(p)
  return p[1] == "ALLOWED" and p[2] == "ALLOWED" or (p[1] == "RESTRICTED" and p[2] == "RESTRICTED" or p[1] == "FORBIDDEN" and p[2] == "FORBIDDEN")
end
_L[230] = "PDF"
_L[231] = "DVI"
_L[232] = function(tmp54, tmp55)
  return tmp54 == "PDF" and tmp55 == "PDF" or tmp54 == "DVI" and tmp55 == "DVI"
end
_L[233] = "PDFTEX"
_L[234] = "XETEX"
_L[235] = "LUATEX"
_L[236] = "OTHER"
_L[237] = function(tmp54, tmp55)
  return tmp54 == "PDFTEX" and tmp55 == "PDFTEX" or (tmp54 == "XETEX" and tmp55 == "XETEX" or (tmp54 == "LUATEX" and tmp55 == "LUATEX" or tmp54 == "OTHER" and tmp55 == "OTHER"))
end
_L[238] = {dvi_extension = "dvi", engine_type = _L[233], executable = "pdftex", name = "pdftex", supports_draftmode = true, supports_pdf_generation = true}
_L[239] = {dvi_extension = "dvi", engine_type = _L[233], executable = "pdflatex", name = "pdflatex", supports_draftmode = true, supports_pdf_generation = true}
_L[240] = {dvi_extension = "dvi", engine_type = _L[235], executable = "luatex", name = "luatex", supports_draftmode = true, supports_pdf_generation = true}
_L[241] = {dvi_extension = "dvi", engine_type = _L[235], executable = "lualatex", name = "lualatex", supports_draftmode = true, supports_pdf_generation = true}
_L[242] = {dvi_extension = "dvi", engine_type = _L[235], executable = "luajittex", name = "luajittex", supports_draftmode = true, supports_pdf_generation = true}
_L[243] = {dvi_extension = "xdv", engine_type = _L[234], executable = "xetex", name = "xetex", supports_draftmode = true, supports_pdf_generation = true}
_L[244] = {dvi_extension = "xdv", engine_type = _L[234], executable = "xelatex", name = "xelatex", supports_draftmode = true, supports_pdf_generation = true}
_L[245] = {dvi_extension = "dvi", engine_type = _L[236], executable = "tex", name = "tex", supports_draftmode = false, supports_pdf_generation = false}
_L[246] = {dvi_extension = "dvi", engine_type = _L[236], executable = "etex", name = "etex", supports_draftmode = false, supports_pdf_generation = false}
_L[247] = {dvi_extension = "dvi", engine_type = _L[236], executable = "latex", name = "latex", supports_draftmode = false, supports_pdf_generation = false}
_L[248] = {dvi_extension = "dvi", engine_type = _L[236], executable = "ptex", name = "ptex", supports_draftmode = false, supports_pdf_generation = false}
_L[249] = {dvi_extension = "dvi", engine_type = _L[236], executable = "eptex", name = "eptex", supports_draftmode = false, supports_pdf_generation = false}
_L[250] = {dvi_extension = "dvi", engine_type = _L[236], executable = "platex", name = "platex", supports_draftmode = false, supports_pdf_generation = false}
_L[251] = {dvi_extension = "dvi", engine_type = _L[236], executable = "uptex", name = "uptex", supports_draftmode = false, supports_pdf_generation = false}
_L[252] = {dvi_extension = "dvi", engine_type = _L[236], executable = "euptex", name = "euptex", supports_draftmode = false, supports_pdf_generation = false}
_L[253] = {dvi_extension = "dvi", engine_type = _L[236], executable = "uplatex", name = "uplatex", supports_draftmode = false, supports_pdf_generation = false}
_L[254] = function(a)
  if a == "pdftex" then
    return {tag = "SOME", payload = _L[238]}
  elseif a == "pdflatex" then
    return {tag = "SOME", payload = _L[239]}
  elseif a == "luatex" then
    return {tag = "SOME", payload = _L[240]}
  elseif a == "lualatex" then
    return {tag = "SOME", payload = _L[241]}
  elseif a == "luajittex" then
    return {tag = "SOME", payload = _L[242]}
  elseif a == "xetex" then
    return {tag = "SOME", payload = _L[243]}
  elseif a == "xelatex" then
    return {tag = "SOME", payload = _L[244]}
  elseif a == "tex" then
    return {tag = "SOME", payload = _L[245]}
  elseif a == "etex" then
    return {tag = "SOME", payload = _L[246]}
  elseif a == "latex" then
    return {tag = "SOME", payload = _L[247]}
  elseif a == "ptex" then
    return {tag = "SOME", payload = _L[248]}
  elseif a == "eptex" then
    return {tag = "SOME", payload = _L[249]}
  elseif a == "platex" then
    return {tag = "SOME", payload = _L[250]}
  elseif a == "uptex" then
    return {tag = "SOME", payload = _L[251]}
  elseif a == "euptex" then
    return {tag = "SOME", payload = _L[252]}
  elseif a == "uplatex" then
    return {tag = "SOME", payload = _L[253]}
  else
    return NONE
  end
end
_L[255] = "DEFAULT"
_L[256] = "BLACK"
_L[257] = "RED"
_L[258] = "GREEN"
_L[259] = "YELLOW"
_L[260] = "BLUE"
_L[261] = "MAGENTA"
_L[262] = "CYAN"
_L[263] = "WHITE"
_L[264] = "BRIGHT_BLACK"
_L[265] = "BRIGHT_RED"
_L[266] = "BRIGHT_GREEN"
_L[267] = "BRIGHT_YELLOW"
_L[268] = "BRIGHT_BLUE"
_L[269] = "BRIGHT_MAGENTA"
_L[270] = "BRIGHT_CYAN"
_L[271] = "BRIGHT_WHITE"
_L[272] = function(a)
  if a == "DEFAULT" then
    return NONE
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
_L[273] = function(a)
  if a == "DEFAULT" then
    return NONE
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
_L[274] = function(tmp54, xs)
  if tmp54.tag == "SOME" then
    return {tmp54.payload, xs}
  elseif tmp54.tag == "NONE" then
    return xs
  else
    _raise(_Match, "ansi-color.sml:137:7")
  end
end
_L[275] = function(background, blink, bold, dim, foreground, italic, reverse, strike, underline)
  local attrs, tmp54
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
    local tmp55 = mapPartial(_L[273])
    local tmp56 = tmp55(background)
    attrs = _L[274](tmp56, attrs7)
    local tmp57 = mapPartial(_L[272])
    tmp54 = tmp57(foreground)
  end
  local attrs1 = _L[274](tmp54, attrs)
  return "\x1B[" .. table_concat(_VectorOrArray_fromList(attrs1), ";") .. "m"
end
_ENV.CLUTTEX_VERBOSITY = 0
_L[276] = "ALWAYS"
_L[277] = "AUTO"
_L[278] = "NEVER"
_L[279] = {false}
_L[280] = function(a)
  if a == "ALWAYS" then
  else
    goto else1
  end
  do
    local tmp54 = tmp3("texrunner.isatty").enable_virtual_terminal
    local tmp55 = _ENV.io.stderr
    _L[279][1] = true
    if not not tmp54 then
    else
      return nil
    end
    do
      if not tmp54(tmp55) and _ENV.CLUTTEX_VERBOSITY >= 2 then
      else
        return nil
      end
      do
        return _L[124]({_L[118], "ClutTeX: Failed to enable virtual terminal\n"})
      end
    end
  end
  ::else1::
  if a == "AUTO" then
  elseif a == "NEVER" then
    _L[279][1] = false
    return nil
  else
    _raise(_Match, "message.sml:26:5")
  end
  do
    local tmp54 = tmp3("texrunner.isatty")
    local tmp55 = tmp54.enable_virtual_terminal
    local tmp56 = _ENV.io.stderr
    local tmp57 = not not tmp54.isatty(tmp56)
    _L[279][1] = tmp57
    local tmp58
    tmp58 = tmp57 and not not tmp55
    if tmp58 then
    else
      return nil
    end
    do
      local tmp59 = tmp55(tmp56)
      _L[279][1] = tmp59
      if not tmp59 and _ENV.CLUTTEX_VERBOSITY >= 2 then
      else
        return nil
      end
      do
        return _L[124]({_L[118], "ClutTeX: Failed to enable virtual terminal\n"})
      end
    end
  end
end
_L[281] = {tag = "SOME", payload = _L[271]}
_L[282] = _L[275]({tag = "SOME", payload = _L[257]}, false, false, false, _L[281], false, false, false, false)
_L[283] = {_L[282]}
_L[284] = _L[275](NONE, false, false, false, {tag = "SOME", payload = _L[262]}, false, false, false, false)
_L[285] = {_L[284]}
_L[286] = _L[275](NONE, false, false, false, {tag = "SOME", payload = _L[257]}, false, false, false, false)
_L[287] = {_L[286]}
_L[288] = _L[275](NONE, false, false, false, {tag = "SOME", payload = _L[260]}, false, false, false, false)
_L[289] = {_L[288]}
_L[290] = _L[275](NONE, false, false, false, {tag = "SOME", payload = _L[260]}, false, false, false, false)
_L[291] = {_L[290]}
_L[292] = _L[275](NONE, false, false, false, {tag = "SOME", payload = _L[261]}, false, false, false, false)
_L[293] = {_L[292]}
_L[294] = function(a)
  local tmp54
  do
    local background = a.background
    local blink = a.blink
    local bold = a.bold
    local dim = a.dim
    local foreground = a.foreground
    local italic = a.italic
    local reverse = a.reverse
    local strike = a.strike
    tmp54 = _L[275](background, blink, bold, dim, foreground, italic, reverse, strike, a.underline)
  end
  _L[283][1] = tmp54
  return nil
end
_L[295] = function(a)
  local tmp54
  do
    local background = a.background
    local blink = a.blink
    local bold = a.bold
    local dim = a.dim
    local foreground = a.foreground
    local italic = a.italic
    local reverse = a.reverse
    local strike = a.strike
    tmp54 = _L[275](background, blink, bold, dim, foreground, italic, reverse, strike, a.underline)
  end
  _L[285][1] = tmp54
  return nil
end
_L[296] = function(a)
  local tmp54
  do
    local background = a.background
    local blink = a.blink
    local bold = a.bold
    local dim = a.dim
    local foreground = a.foreground
    local italic = a.italic
    local reverse = a.reverse
    local strike = a.strike
    tmp54 = _L[275](background, blink, bold, dim, foreground, italic, reverse, strike, a.underline)
  end
  _L[287][1] = tmp54
  return nil
end
_L[297] = function(a)
  local tmp54
  do
    local background = a.background
    local blink = a.blink
    local bold = a.bold
    local dim = a.dim
    local foreground = a.foreground
    local italic = a.italic
    local reverse = a.reverse
    local strike = a.strike
    tmp54 = _L[275](background, blink, bold, dim, foreground, italic, reverse, strike, a.underline)
  end
  _L[289][1] = tmp54
  return nil
end
_L[298] = function(a)
  local tmp54
  do
    local background = a.background
    local blink = a.blink
    local bold = a.bold
    local dim = a.dim
    local foreground = a.foreground
    local italic = a.italic
    local reverse = a.reverse
    local strike = a.strike
    tmp54 = _L[275](background, blink, bold, dim, foreground, italic, reverse, strike, a.underline)
  end
  _L[291][1] = tmp54
  return nil
end
_L[299] = function(a)
  local tmp54
  do
    local background = a.background
    local blink = a.blink
    local bold = a.bold
    local dim = a.dim
    local foreground = a.foreground
    local italic = a.italic
    local reverse = a.reverse
    local strike = a.strike
    tmp54 = _L[275](background, blink, bold, dim, foreground, italic, reverse, strike, a.underline)
  end
  _L[293][1] = tmp54
  return nil
end
_L[300] = function(a)
  if _L[279][1] then
  else
    return _L[124]({_L[118], "[EXEC] " .. a .. "\n"})
  end
  do
    local tmp54 = _L[283][1] .. "[EXEC]" .. "\x1B[0m" .. " "
    return _L[124]({_L[118], tmp54 .. _L[285][1] .. a .. "\x1B[0m" .. "\n"})
  end
end
_L[301] = function(a)
  if _L[279][1] then
  else
    return _L[124]({_L[118], "[ERROR] " .. a .. "\n"})
  end
  do
    local tmp54 = _L[283][1] .. "[ERROR]" .. "\x1B[0m" .. " "
    return _L[124]({_L[118], tmp54 .. _L[287][1] .. a .. "\x1B[0m" .. "\n"})
  end
end
_L[302] = function(a)
  if _L[279][1] then
  else
    return _L[124]({_L[118], "[WARN] " .. a .. "\n"})
  end
  do
    local tmp54 = _L[283][1] .. "[WARN]" .. "\x1B[0m" .. " "
    return _L[124]({_L[118], tmp54 .. _L[289][1] .. a .. "\x1B[0m" .. "\n"})
  end
end
_L[303] = function(a)
  if _L[279][1] then
  else
    return _L[124]({_L[118], "[DIAG] " .. a .. "\n"})
  end
  do
    local tmp54 = _L[283][1] .. "[DIAG]" .. "\x1B[0m" .. " "
    return _L[124]({_L[118], tmp54 .. _L[291][1] .. a .. "\x1B[0m" .. "\n"})
  end
end
_L[304] = function(a)
  if _L[279][1] then
  else
    return _L[124]({_L[118], "[INFO] " .. a .. "\n"})
  end
  do
    local tmp54 = _L[283][1] .. "[INFO]" .. "\x1B[0m" .. " "
    return _L[124]({_L[118], tmp54 .. _L[293][1] .. a .. "\x1B[0m" .. "\n"})
  end
end
_L[305] = "DVIPDFMX"
_L[306] = "DVIPS"
_L[307] = "DVISVGM"
_L[308] = "PDFTEX"
_L[309] = "XETEX"
_L[310] = "LUATEX"
_L[311] = "DVIPDFMX"
_L[312] = "DVIPS"
_L[313] = "DVISVGM"
_L[314] = "PDFTEX"
_L[315] = "XETEX"
_L[316] = "LUATEX"
_L[317] = "UNKNOWN"
_L[318] = function(a)
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
_L[319] = "PDFMODE"
_L[320] = "DVISVGM"
_L[321] = "XDVIPDFMX"
_L[322] = "DVIPDFMX"
_L[323] = "DVIPS"
_L[324] = "PDFTEX"
_L[325] = "LUATEX"
_L[326] = "XETEX"
_L[327] = "UNKNOWN"
_L[328] = function(tmp54, tmp55)
  return tmp54 == "PDFMODE" and tmp55 == "PDFMODE" or (tmp54 == "DVISVGM" and tmp55 == "DVISVGM" or (tmp54 == "XDVIPDFMX" and tmp55 == "XDVIPDFMX" or (tmp54 == "DVIPDFMX" and tmp55 == "DVIPDFMX" or (tmp54 == "DVIPS" and tmp55 == "DVIPS" or (tmp54 == "PDFTEX" and tmp55 == "PDFTEX" or (tmp54 == "LUATEX" and tmp55 == "LUATEX" or (tmp54 == "XETEX" and tmp55 == "XETEX" or tmp54 == "UNKNOWN" and tmp55 == "UNKNOWN")))))))
end
_L[329] = function(a)
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
_L[330] = "DVIPDFMX"
_L[331] = "DVIPS"
_L[332] = "PDFTEX"
_L[333] = "LUATEX"
_L[334] = "XETEX"
_L[335] = "UNKNOWN"
_L[336] = function(a)
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
_L[337] = "PDF"
_L[338] = "DVIPS"
_L[339] = "UNKNOWN"
_L[340] = "DVIPDFMX"
_L[341] = "DVIPS"
_L[342] = "DVISVGM"
_L[343] = "FSWATCH"
_L[344] = "INOTIFYWAIT"
_L[345] = "AUTO"
_L[346] = function(tmp54, tmp55)
  return tmp54 == "FSWATCH" and tmp55 == "FSWATCH" or (tmp54 == "INOTIFYWAIT" and tmp55 == "INOTIFYWAIT" or tmp54 == "AUTO" and tmp55 == "AUTO")
end
_L[347] = {tag = "NOW"}
_L[348] = function(p)
  if p[1].tag == "NOW" and p[2].tag == "NOW" then
    return true
  elseif p[1].tag == "RAW" and p[2].tag == "RAW" then
    local a = p[1].payload
    return a == p[2].payload
  else
    return false
  end
end
_L[349] = {bibtex_or_biber = NONE, change_directory = NONE, check_driver = NONE, color = NONE, config_file = NONE, dvipdfmx_extraoptions = nil, engine = NONE, engine_executable = NONE, file_line_error = true, fmt = NONE, fresh = false, halt_on_error = true, includeonly = NONE, interaction = NONE, jobname = NONE, make_depends = NONE, makeglossaries = NONE, makeindex = NONE, max_iterations = NONE, output = NONE, output_directory = NONE, output_format = NONE, package_support = {epstopdf = false, minted = false, pdfx = false}, print_output_directory = false, shell_escape = NONE, source_date_epoch = NONE, start_with_draft = false, synctex = NONE, tex_extraoptions = nil, watch = NONE}
_L[350] = function(a)
  return function(a1)
    local tmp54 = isPrefix(a)
    local tmp55 = tmp54(a1)
    if tmp55 then
    else
      return NONE
    end
    do
      local tmp56 = extract1({a1, #a, NONE})
      return {tag = "SOME", payload = tmp56}
    end
  end
end
_L[351] = function(line, outdir)
  local tmp54 = _L[350]("\\@input{")
  local exp = tmp54(line)
  if exp.tag == "NONE" then
    return NONE
  end
  if exp.tag == "SOME" then
  else
    _raise(_Match, "auxfile.sml:15:5")
  end
  do
    local rest = exp.payload
    local tmp55 = splitl(function(c)
      return c ~= 125
    end)
    local tmp56 = tmp55(rest)
    local subauxfile = string1(tmp56[1])
    local subauxfile_abs = _L[215]({tag = "SOME", payload = outdir}, subauxfile)
    return {tag = "SOME", payload = {subauxfile = subauxfile, subauxfile_abs = subauxfile_abs}}
  end
end
_L[352] = function(auxfile, outdir, seen)
  local ins, tmp54, tmp55
  do
    ins = _L[115](auxfile)
    local seen1 = _L[198](seen, auxfile)
    tmp55, tmp54 = false, seen1
  end
  ::cont::
  do
    local did, seen1 = tmp55, tmp54
    local exp = _L[114](ins)
    if exp.tag == "NONE" then
    else
      goto else1
    end
    do
      _L[120](ins)
      return {did, seen1}
    end
    ::else1::
    if exp.tag == "SOME" then
    else
      _raise(_Match, "auxfile.sml:31:9")
    end
    do
      local exp1 = _L[351](exp.payload, outdir)
      if exp1.tag == "NONE" then
        tmp55 = did
        tmp54 = seen1
        goto cont
      end
      if exp1.tag == "SOME" then
      else
        _raise(_Match, "auxfile.sml:34:13")
      end
      do
        local subauxfile
        do
          subauxfile = exp1.payload.subauxfile
          local subauxfile_abs = exp1.payload.subauxfile_abs
          local tmp56 = _L[218](subauxfile_abs)
          if tmp56 then
          else
            goto else2
          end
          do
            local exp2 = _L[352](subauxfile_abs, outdir, seen1)
            local did_PRIME = exp2[1]
            local seen2 = exp2[2]
            if did then
              tmp55 = true
              tmp54 = seen2
              goto cont
            else
              tmp55 = did_PRIME
              tmp54 = seen2
              goto cont
            end
          end
        end
        ::else2::
        local tmp56 = _L[212](subauxfile)
        local tmp57 = _L[210].join(outdir, tmp56)
        local tmp58 = _L[219](tmp57)
        if tmp58 then
          tmp55 = did
          tmp54 = seen1
          goto cont
        end
        local tmp59, tmp60 = _L[217].mkdir_rec(tmp57)
        if not tmp59 then
        else
          tmp55 = true
          tmp54 = seen1
          goto cont
        end
        do
          local tmp61 = _Error(tmp60)
          _raise(tmp61, "fs-util.sml:16:28")
        end
      end
    end
  end
end
_L[353] = function(auxfile, outdir, revLines)
  local ins = _L[115](auxfile)
  local tmp54 = revLines
  ::cont::
  do
    local a = tmp54
    local exp = _L[114](ins)
    if exp.tag == "NONE" then
    else
      goto else1
    end
    do
      _L[120](ins)
      return a
    end
    ::else1::
    if exp.tag == "SOME" then
    else
      _raise(_Match, "auxfile.sml:62:9")
    end
    do
      local line = exp.payload
      local exp1 = _L[351](line, outdir)
      if exp1.tag == "SOME" then
      else
        goto else2
      end
      do
        local subauxfile_abs = exp1.payload.subauxfile_abs
        local tmp55 = _L[218](subauxfile_abs)
        if tmp55 then
        else
          tmp54 = a
          goto cont
        end
        tmp54 = _L[353](subauxfile_abs, outdir, a)
        goto cont
      end
      ::else2::
      if exp1.tag == "NONE" then
      else
        _raise(_Match, "auxfile.sml:65:13")
      end
      do
        local isBibTeXLine
        do
          local tmp55 = _L[350]("\\")
          local exp2 = tmp55(line)
          do
            if exp2.tag == "SOME" then
            elseif exp2.tag == "NONE" then
              isBibTeXLine = false
              goto cont1
            else
              _raise(_Match, "auxfile.sml:83:21")
            end
            do
              local s = exp2.payload
              local tmp56
              do
                local tmp57 = splitl(function(c)
                  local tmp58
                  tmp58 = 65 <= c and c <= 90 or 97 <= c and c <= 122
                  return tmp58 or c == 64
                end)
                local tmp58 = tmp57(s)
                tmp56 = tmp58[1]
              end
              local exp3 = string1(tmp56)
              isBibTeXLine = exp3 == "citation" or (exp3 == "bibdata" or exp3 == "bibstyle")
            end
          end
        end
        ::cont1::
        if isBibTeXLine then
        else
          tmp54 = a
          goto cont
        end
        do
          if _ENV.CLUTTEX_VERBOSITY >= 2 then
          else
            tmp54 = {line, a}
            goto cont
          end
          do
            local tmp55 = full(line)
            local tmp56 = splitr(isSpace, tmp55)
            local tmp57 = string1(tmp56[1])
            _L[304]("BibTeX line: " .. tmp57)
            tmp54 = {line, a}
            goto cont
          end
        end
      end
    end
  end
end
_L[354] = tmp3("texrunner.luatexinit")
_L[355] = "INPUT"
_L[356] = "OUTPUT"
_L[357] = "AUXILIARY"
_L[358] = function(tmp54, tmp55)
  return tmp54 == "INPUT" and tmp55 == "INPUT" or (tmp54 == "OUTPUT" and tmp55 == "OUTPUT" or tmp54 == "AUXILIARY" and tmp55 == "AUXILIARY")
end
_L[359] = function(fileInfo, fileMap)
  local tmp54 = foldl(function(a)
    local x = a[1]
    return {x[1], a[2]}
  end)
  local tmp55 = tmp54(nil)
  local tmp56 = tmp55(fileInfo)
  local tmp57 = _L[206](_EXCLAM)
  local tmp58 = tmp57(fileMap)
  return {tmp56, tmp58}
end
_L[360] = function(file, tmp54, fileList, fileMap)
  local ins = _L[115](file)
  local tmp55, tmp56 = fileList, fileMap
  ::cont::
  do
    local fileList1, fileMap1 = tmp55, tmp56
    local exp = _L[114](ins)
    if exp.tag == "NONE" then
    else
      goto else1
    end
    do
      _L[120](ins)
      return {fileList1, fileMap1}
    end
    ::else1::
    if exp.tag == "SOME" then
    else
      _raise(_Match, "reruncheck.sml:29:17")
    end
    do
      local path, exp1
      do
        local line = exp.payload
        local tmp57 = splitl(isAlphaNum)
        local tmp58 = full(line)
        local exp2 = tmp57(tmp58)
        local t = exp2[1]
        local tmp59
        do
          local tmp60 = splitr(isSpace, exp2[2])
          tmp59 = tmp60[1]
        end
        local tmp60
        do
          local tmp61 = splitl(isSpace)
          local tmp62 = tmp61(tmp59)
          tmp60 = tmp62[2]
        end
        path = string1(tmp60)
        exp1 = string1(t)
        if exp1 == "PWD" then
          tmp55 = fileList1
          tmp56 = fileMap1
          goto cont
        end
        if exp1 == "INPUT" then
        else
          goto else2
        end
        do
          local abspath = _L[215](NONE, path)
          local exp3 = _L[205](fileMap1, abspath)
          if exp3.tag == "SOME" then
          else
            goto else4
          end
          do
            local r = exp3.payload
            local path_PRIME = exp3.payload[1].path
            local abspath_PRIME = exp3.payload[1].abspath
            local kind = exp3.payload[1].kind
            local tmp61 = #path
            local tmp62
            if tmp61 < #path_PRIME then
              tmp62 = path
            else
              tmp62 = path_PRIME
            end
            local tmp63 = _L[358](kind, _L[356])
            local tmp64
            if tmp63 then
              tmp64 = _L[357]
            else
              tmp64 = kind
            end
            r[1] = {abspath = abspath_PRIME, kind = tmp64, path = tmp62}
            tmp55 = fileList1
            tmp56 = fileMap1
            goto cont
          end
          ::else4::
          if exp3.tag == "NONE" then
          else
            _raise(_Match, "reruncheck.sml:38:31")
          end
          do
            local tmp61 = _L[218](path)
            if tmp61 then
            else
              tmp55 = fileList1
              tmp56 = fileMap1
              goto cont
            end
            do
              local tmp62 = _L[214](path)
              local tmp63
              if tmp62 == "bbl" then
                tmp63 = _L[357]
              else
                tmp63 = _L[355]
              end
              local tmp64 = {{abspath = abspath, kind = tmp63, path = path}}
              local tmp65 = {tmp64, fileList1}
              local tmp66 = _L[204](fileMap1, abspath, tmp64)
              tmp55 = tmp65
              tmp56 = tmp66
              goto cont
            end
          end
        end
      end
      ::else2::
      if exp1 == "OUTPUT" then
      else
        goto else3
      end
      do
        local abspath = _L[215](NONE, path)
        local exp2 = _L[205](fileMap1, abspath)
        if exp2.tag == "SOME" then
        else
          goto else4
        end
        do
          local r = exp2.payload
          local path_PRIME = exp2.payload[1].path
          local abspath_PRIME = exp2.payload[1].abspath
          local kind = exp2.payload[1].kind
          local tmp57 = #path
          local tmp58
          if tmp57 < #path_PRIME then
            tmp58 = path
          else
            tmp58 = path_PRIME
          end
          local tmp59 = _L[358](kind, _L[355])
          local tmp60
          if tmp59 then
            tmp60 = _L[357]
          else
            tmp60 = kind
          end
          r[1] = {abspath = abspath_PRIME, kind = tmp60, path = tmp58}
          tmp55 = fileList1
          tmp56 = fileMap1
          goto cont
        end
        ::else4::
        if exp2.tag == "NONE" then
        else
          _raise(_Match, "reruncheck.sml:70:31")
        end
        do
          local tmp57
          do
            local ext = _L[214](path)
            do
              if ext == "out" then
                tmp57 = true
                goto cont1
              end
              local tmp58 = tmp(eq)
              local tmp59
              do
                local tmp60 = tmp58({tmp54, NONE})
                tmp59 = not tmp60
              end
              local tmp60
              tmp60 = tmp59 and ext == "idx"
              tmp57 = tmp60 or (ext == "bcf" or ext == "glo")
            end
          end
          ::cont1::
          local tmp58
          if tmp57 then
            tmp58 = _L[357]
          else
            tmp58 = _L[356]
          end
          local tmp59 = {{abspath = abspath, kind = tmp58, path = path}}
          local tmp60 = {tmp59, fileList1}
          local tmp61 = _L[204](fileMap1, abspath, tmp59)
          tmp55 = tmp60
          tmp56 = tmp61
          goto cont
        end
      end
      ::else3::
      _L[302]("Unrecognized line in recorder file '" .. file .. "': " .. exp1)
      tmp55 = fileList1
      tmp56 = fileMap1
      goto cont
    end
  end
end
_L[361] = function(fileList, auxstatus)
  local shouldRerun, auxstatus1
  do
    local tmp54, tmp55 = fileList, _L[203]
    ::cont1::
    do
      local tmp56, newauxstatus = tmp54, tmp55
      if tmp56 == nil then
        auxstatus1 = newauxstatus
        shouldRerun = false
        goto cont
      end
      if tmp56 ~= nil then
      else
        _raise(_Match, "reruncheck.sml:155:15")
      end
      do
        local shortPath = tmp56[1].path
        local abspath = tmp56[1].abspath
        local kind = tmp56[1].kind
        local tmp57 = tmp56[2]
        local tmp58 = _L[218](abspath)
        if tmp58 then
        else
          tmp54 = tmp57
          tmp55 = newauxstatus
          goto cont1
        end
        do
          local shouldRerun1, newauxstatus1
          do
            if kind == "INPUT" then
            else
              goto else1
            end
            do
              local mtime = _L[66](abspath)
              local exp = _L[205](auxstatus, abspath)
              if exp.tag == "SOME" and exp.payload.mtime.tag == "SOME" then
              else
                newauxstatus1 = newauxstatus
                shouldRerun1 = false
                goto cont2
              end
              do
                if exp.payload.mtime.payload < mtime then
                else
                  newauxstatus1 = newauxstatus
                  shouldRerun1 = false
                  goto cont2
                end
                do
                  _L[304]("Input file '" .. shortPath .. "' was modified (by user, or some external commands).")
                  local tmp59 = _L[204](newauxstatus, abspath, {{md5sum = NONE, mtime = {tag = "SOME", payload = mtime}, size = NONE}})
                  newauxstatus1 = tmp59
                  shouldRerun1 = true
                  goto cont2
                end
              end
            end
            ::else1::
            if kind == "AUXILIARY" then
            elseif kind == "OUTPUT" then
              newauxstatus1 = newauxstatus
              shouldRerun1 = false
              goto cont2
            else
              _raise(_Match, "reruncheck.sml:159:31")
            end
            do
              local exp = _L[205](auxstatus, abspath)
              if exp.tag == "SOME" then
              else
                goto else2
              end
              do
                local newauxstatus2, modifiedBecause
                do
                  local s = exp.payload
                  local size1 = _L[67](abspath)
                  local exp1 = s.size
                  local sizeIsDifferent
                  if exp1.tag == "SOME" then
                    sizeIsDifferent = exp1.payload ~= size1
                  elseif exp1.tag == "NONE" then
                    sizeIsDifferent = true
                  else
                    _raise(_Match, "reruncheck.sml:178:66")
                  end
                  do
                    if sizeIsDifferent then
                    else
                      goto else4
                    end
                    do
                      local exp2 = s.size
                      local previousSize
                      if exp2.tag == "SOME" then
                        previousSize = tmp21(tmp5(exp2.payload), "-", "~")
                      elseif exp2.tag == "NONE" then
                        previousSize = "(N/A)"
                      else
                        _raise(_Match, "reruncheck.sml:183:77")
                      end
                      local tmp59 = "size: " .. previousSize .. " -> "
                      local tmp60 = {tag = "SOME", payload = tmp59 .. tmp21(tmp5(size1), "-", "~")}
                      local tmp61 = _L[204](newauxstatus, abspath, {{md5sum = NONE, mtime = NONE, size = {tag = "SOME", payload = size1}}})
                      newauxstatus2 = tmp61
                      modifiedBecause = tmp60
                      goto cont3
                    end
                    ::else4::
                    local md5sum
                    do
                      local ins = _L[151](abspath)
                      local tmp59 = _L[153](ins)
                      _L[152](ins)
                      md5sum = _L[191](tmp59)
                    end
                    local exp2 = s.md5sum
                    local md5sumIsDifferent
                    if exp2.tag == "SOME" then
                      local h = exp2.payload
                      local tmp59 = h[1]
                      local tmp60 = h[2]
                      local tmp61 = h[3]
                      local tmp62 = h[4]
                      local tmp63 = md5sum[1]
                      local tmp64 = md5sum[2]
                      local tmp65 = md5sum[3]
                      md5sumIsDifferent = tmp62 ~= md5sum[4] or (tmp61 ~= tmp65 or (tmp60 ~= tmp64 or tmp59 ~= tmp63))
                    elseif exp2.tag == "NONE" then
                      md5sumIsDifferent = true
                    else
                      _raise(_Match, "reruncheck.sml:190:82")
                    end
                    if md5sumIsDifferent then
                    else
                      newauxstatus2 = newauxstatus
                      modifiedBecause = NONE
                      goto cont3
                    end
                    do
                      local previousMd5sum
                      do
                        local exp3 = s.md5sum
                        do
                          if exp3.tag == "SOME" then
                          elseif exp3.tag == "NONE" then
                            previousMd5sum = "(N/A)"
                            goto cont4
                          else
                            _raise(_Match, "reruncheck.sml:194:86")
                          end
                          do
                            local h = exp3.payload
                            local a = h[1]
                            local b = h[2]
                            local c = h[3]
                            previousMd5sum = _L[193](a, b, c, h[4])
                          end
                        end
                      end
                      ::cont4::
                      local tmp59 = "md5: " .. previousMd5sum .. " -> "
                      local tmp60
                      do
                        local a = md5sum[1]
                        local b = md5sum[2]
                        local c = md5sum[3]
                        tmp60 = _L[193](a, b, c, md5sum[4])
                      end
                      local tmp61 = {tag = "SOME", payload = tmp59 .. tmp60}
                      local tmp62 = {tag = "SOME", payload = size1}
                      local tmp63 = _L[204](newauxstatus, abspath, {{md5sum = {tag = "SOME", payload = md5sum}, mtime = NONE, size = tmp62}})
                      newauxstatus2 = tmp63
                      modifiedBecause = tmp61
                    end
                  end
                end
                ::cont3::
                if modifiedBecause.tag == "SOME" then
                else
                  goto else3
                end
                do
                  local reason = modifiedBecause.payload
                  _L[304]("File '" .. shortPath .. "' was modified (" .. reason .. ").")
                  newauxstatus1 = newauxstatus2
                  shouldRerun1 = true
                  goto cont2
                end
                ::else3::
                if modifiedBecause.tag == "NONE" then
                else
                  _raise(_Match, "reruncheck.sml:202:43")
                end
                do
                  if _ENV.CLUTTEX_VERBOSITY >= 1 then
                  else
                    newauxstatus1 = newauxstatus2
                    shouldRerun1 = false
                    goto cont2
                  end
                  _L[304]("File '" .. shortPath .. "' unmodified (size and md5sum).")
                  newauxstatus1 = newauxstatus2
                  shouldRerun1 = false
                  goto cont2
                end
              end
              ::else2::
              if exp.tag == "NONE" then
              else
                _raise(_Match, "reruncheck.sml:175:36")
              end
              do
                local newauxstatus2, shouldRerun2
                do
                  local tmp59 = isSuffix(".aux")
                  local tmp60 = tmp59(abspath)
                  do
                    if tmp60 then
                    else
                      newauxstatus2 = newauxstatus
                      shouldRerun2 = true
                      goto cont3
                    end
                    do
                      local size1 = _L[67](abspath)
                      if size1 == 8 then
                      else
                        goto else4
                      end
                      do
                        local ins = _L[151](abspath)
                        local tmp61 = _L[153](ins)
                        _L[152](ins)
                        local tmp62 = tmp61 == "\\relax \n"
                        local tmp63 = {tag = "SOME", payload = size1}
                        local tmp64 = _L[191](tmp61)
                        local newauxstatus3 = _L[204](newauxstatus, abspath, {{md5sum = {tag = "SOME", payload = tmp64}, mtime = NONE, size = tmp63}})
                        newauxstatus2 = newauxstatus3
                        shouldRerun2 = not tmp62
                        goto cont3
                      end
                      ::else4::
                      local newauxstatus3 = _L[204](newauxstatus, abspath, {{md5sum = NONE, mtime = NONE, size = {tag = "SOME", payload = size1}}})
                      newauxstatus2 = newauxstatus3
                      shouldRerun2 = true
                    end
                  end
                end
                ::cont3::
                if shouldRerun2 then
                else
                  goto else3
                end
                _L[304]("New auxiliary file '" .. shortPath .. "'.")
                newauxstatus1 = newauxstatus2
                shouldRerun1 = shouldRerun2
                goto cont2
                ::else3::
                if _ENV.CLUTTEX_VERBOSITY >= 1 then
                else
                  newauxstatus1 = newauxstatus2
                  shouldRerun1 = shouldRerun2
                  goto cont2
                end
                _L[304]("Ignoring almost-empty auxiliary file '" .. shortPath .. "'.")
                newauxstatus1 = newauxstatus2
                shouldRerun1 = shouldRerun2
              end
            end
          end
          ::cont2::
          if shouldRerun1 then
            auxstatus1 = newauxstatus1
            shouldRerun = true
          else
            tmp54 = tmp57
            tmp55 = newauxstatus1
            goto cont1
          end
        end
      end
    end
  end
  ::cont::
  local tmp54 = _L[206](_EXCLAM)
  local tmp55 = tmp54(auxstatus1)
  return {shouldRerun, tmp55}
end
_L[362] = function(auxstatus, dst, srcAbs)
  local tmp54 = _L[218](dst)
  if not tmp54 then
    return true
  end
  local exp = _L[205](auxstatus, srcAbs)
  if exp.tag == "SOME" and exp.payload.mtime.tag == "SOME" then
  else
    return false
  end
  do
    local mtime = exp.payload.mtime.payload
    local tmp55 = _L[66](dst)
    return mtime > tmp55
  end
end
_L[363] = {color = {diagnostic = NONE, error = NONE, execute = NONE, information = NONE, type_ = NONE, warning = NONE}, temporary_directory = NONE}
_L[364] = function(table, key)
  local tmp54 = find(function(a)
    return a[1] == key
  end)
  local exp = tmp54(table)
  if exp.tag == "NONE" then
    return NONE
  elseif exp.tag == "SOME" then
    return {tag = "SOME", payload = exp.payload[2]}
  else
    _raise(_Match, "config-file.sml:42:5")
  end
end
_L[365] = function(tmp54, key)
  if tmp54.tag == "SOME" then
  elseif tmp54.tag == "NONE" then
    return NONE
  else
    _raise(_Match, "config-file.sml:46:7")
  end
  do
    local table = tmp54.payload
    local tmp55 = find(function(a)
      return a[1] == key
    end)
    local exp = tmp55(table)
    if exp.tag == "NONE" then
      return NONE
    elseif exp.tag == "SOME" then
      return {tag = "SOME", payload = exp.payload[2]}
    else
      _raise(_Match, "config-file.sml:47:10")
    end
  end
end
_L[366] = function(tmp54, f)
  if tmp54.tag == "SOME" then
  elseif tmp54.tag == "NONE" then
    return NONE
  else
    _raise(_Match, "config-file.sml:54:7")
  end
  do
    return f(tmp54.payload)
  end
end
_L[367] = function(a)
  return function(a1)
    if a1.tag == "BOOL" then
      return {tag = "SOME", payload = a1.payload}
    end
    _L[302]("Config entry " .. a .. " should be a boolean.")
    return NONE
  end
end
_L[368] = function(a)
  return function(a1)
    if a1.tag == "STRING" then
    else
      goto else1
    end
    do
      local x = a1.payload
      local exp
      if x == "default" then
        exp = {tag = "SOME", payload = _L[255]}
      elseif x == "black" then
        exp = {tag = "SOME", payload = _L[256]}
      elseif x == "red" then
        exp = {tag = "SOME", payload = _L[257]}
      elseif x == "green" then
        exp = {tag = "SOME", payload = _L[258]}
      elseif x == "yellow" then
        exp = {tag = "SOME", payload = _L[259]}
      elseif x == "blue" then
        exp = {tag = "SOME", payload = _L[260]}
      elseif x == "magenta" then
        exp = {tag = "SOME", payload = _L[261]}
      elseif x == "cyan" then
        exp = {tag = "SOME", payload = _L[262]}
      elseif x == "white" then
        exp = {tag = "SOME", payload = _L[263]}
      elseif x == "brightblack" then
        exp = {tag = "SOME", payload = _L[264]}
      elseif x == "brightred" then
        exp = {tag = "SOME", payload = _L[265]}
      elseif x == "brightgreen" then
        exp = {tag = "SOME", payload = _L[266]}
      elseif x == "brightyellow" then
        exp = {tag = "SOME", payload = _L[267]}
      elseif x == "brightblue" then
        exp = {tag = "SOME", payload = _L[268]}
      elseif x == "brightmagenta" then
        exp = {tag = "SOME", payload = _L[269]}
      elseif x == "brightcyan" then
        exp = {tag = "SOME", payload = _L[270]}
      elseif x == "brightwhite" then
        exp = {tag = "SOME", payload = _L[271]}
      else
        exp = NONE
      end
      if exp.tag == "SOME" then
        return {tag = "SOME", payload = exp.payload}
      end
      if exp.tag == "NONE" then
      else
        _raise(_Match, "config-file.sml:76:10")
      end
      do
        _L[302]("Config entry " .. a .. " should be a valid color.")
        return NONE
      end
    end
    ::else1::
    _L[302]("Config entry " .. a .. " should be a string.")
    return NONE
  end
end
_L[369] = function(a)
  return function(a1)
    if a1.tag == "TABLE" then
    else
      goto else1
    end
    do
      local t, tmp54, tmp55, tmp56, tmp57
      do
        t = a1.payload
        local tmp58 = _L[364](t, "fore")
        local tmp59 = _L[368](a .. ".fore")
        tmp54 = _L[366](tmp58, tmp59)
        local tmp60 = _L[364](t, "back")
        local tmp61 = _L[368](a .. ".back")
        tmp55 = _L[366](tmp60, tmp61)
        local tmp62 = _L[364](t, "bold")
        local tmp63 = _L[367](a .. ".bold")
        local tmp64 = _L[366](tmp62, tmp63)
        tmp56 = getOpt(tmp64, false)
        local tmp65 = _L[364](t, "dim")
        local tmp66 = _L[367](a .. ".dim")
        local tmp67 = _L[366](tmp65, tmp66)
        tmp57 = getOpt(tmp67, false)
      end
      local tmp58, tmp59, tmp60, tmp61
      do
        local tmp62 = _L[364](t, "underline")
        local tmp63 = _L[367](a .. ".underline")
        local tmp64 = _L[366](tmp62, tmp63)
        tmp58 = getOpt(tmp64, false)
        local tmp65 = _L[364](t, "blink")
        local tmp66 = _L[367](a .. ".blink")
        local tmp67 = _L[366](tmp65, tmp66)
        tmp59 = getOpt(tmp67, false)
        local tmp68 = _L[364](t, "reverse")
        local tmp69 = _L[367](a .. ".reverse")
        local tmp70 = _L[366](tmp68, tmp69)
        tmp60 = getOpt(tmp70, false)
        local tmp71 = _L[364](t, "italic")
        local tmp72 = _L[367](a .. ".italic")
        tmp61 = _L[366](tmp71, tmp72)
      end
      local tmp62 = getOpt(tmp61, false)
      local tmp63 = _L[364](t, "strike")
      local tmp64 = _L[367](a .. ".strike")
      local tmp65 = _L[366](tmp63, tmp64)
      local tmp66 = getOpt(tmp65, false)
      return {tag = "SOME", payload = {background = tmp55, blink = tmp59, bold = tmp56, dim = tmp57, foreground = tmp54, italic = tmp62, reverse = tmp60, strike = tmp66, underline = tmp58}}
    end
    ::else1::
    _L[302]("Config entry " .. a .. " should be a table.")
    return NONE
  end
end
_L[370] = {"Abort"}
_L[371] = {tag = _L[370]}
tmp17("", "ctype")
_L[372] = function(a)
  local tmp54 = a
  ::cont::
  do
    local a1 = tmp54
    if a1 == nil then
      return NONE
    end
    if a1 ~= nil then
      local tmp55 = a1[1]
      local tmp56 = a1[2]
      local exp
      do
        local tmp57 = tmp16(tmp55)
        if tmp57 == nil then
          exp = NONE
        else
          exp = {tag = "SOME", payload = tmp57}
        end
      end
      if exp.tag == "SOME" then
        return {tag = "SOME", payload = exp.payload}
      elseif exp.tag == "NONE" then
        tmp54 = tmp56
        goto cont
      else
        _raise(_Match, "main.sml:27:32")
      end
    else
      _raise(_Match, "main.sml:26:5")
    end
  end
end
_L[373] = function()
  local progName
  do
    local tmp54 = _L[154][0]
    if tmp54 == nil then
    else
      progName = tmp54
      goto cont
    end
    do
      local tmp55 = _Fail("CommandLine.name: arg is not available")
      _raise(tmp55, "command-line.sml:10:45")
    end
  end
  ::cont::
  _L[124]({_L[118], "ClutTeX: Process TeX files without cluttering your working directory\n\nUsage:\n  " .. progName .. " [options] [--] FILE.tex\n\nOptions:\n  -e, --engine=ENGINE          Specify which TeX engine to use.\n                                 ENGINE is one of the following:\n                                     pdflatex, pdftex,\n                                     lualatex, luatex, luajittex,\n                                     xelatex, xetex, latex, etex, tex,\n                                     platex, eptex, ptex,\n                                     uplatex, euptex, uptex,\n      --engine-executable=COMMAND+OPTIONs\n                               The actual TeX command to use.\n                                 [default: ENGINE]\n  -o, --output=FILE            The name of output file.\n                                 [default: JOBNAME.pdf or JOBNAME.dvi]\n      --fresh                  Clean intermediate files before running TeX.\n                                 Cannot be used with --output-directory.\n      --max-iterations=N       Maximum number of running TeX to resolve\n                                 cross-references.  [default: 3]\n      --start-with-draft       Start with draft mode.\n      --[no-]change-directory  Change directory before running TeX.\n      --watch[=ENGINE]         Watch input files for change.  Requires fswatch\n                                 or inotifywait to be installed. ENGINE is one of\n                                 `fswatch', `inotifywait' or `auto' [default: `auto']\n      --tex-option=OPTION      Pass OPTION to TeX as a single option.\n      --tex-options=OPTIONs    Pass OPTIONs to TeX as multiple options.\n      --dvipdfmx-option[s]=OPTION[s]  Same for dvipdfmx.\n      --makeindex=COMMAND+OPTIONs  Command to generate index, such as\n                                     `makeindex' or `mendex'.\n      --bibtex=COMMAND+OPTIONs     Command for BibTeX, such as\n                                     `bibtex' or `pbibtex'.\n      --biber[=COMMAND+OPTIONs]    Command for Biber.\n      --makeglossaries[=COMMAND+OPTIONs]  Command for makeglossaries.\n  -h, --help                   Print this message and exit.\n  -v, --version                Print version information and exit.\n  -V, --verbose                Be more verbose.\n      --color[=WHEN]           Make ClutTeX's message colorful. WHEN is one of\n                                 `always', `auto', or `never'.\n                                 [default: `auto' if --color is omitted,\n                                           `always' if WHEN is omitted]\n      --includeonly=NAMEs      Insert '\\includeonly{NAMEs}'.\n      --make-depends=FILE      Write dependencies as a Makefile rule.\n      --print-output-directory  Print the output directory and exit.\n      --package-support=PKG1[,PKG2,...]\n                               Enable special support for some shell-escaping\n                                 packages.\n                               Currently supported: minted, epstopdf\n      --check-driver=DRIVER    Check that the correct driver file is loaded.\n                               DRIVER is one of `dvipdfmx', `dvips', `dvisvgm'.\n      --source-date-epoch=TIME\n                               Set SOURCE_DATE_EPOCH variable.\n                               TIME is `now' or an integer.\n\n      --[no-]shell-escape\n      --shell-restricted\n      --synctex=NUMBER\n      --fmt=FMTNAME\n      --[no-]file-line-error   [default: yes]\n      --[no-]halt-on-error     [default: yes]\n      --interaction=STRING     [default: nonstopmode]\n      --jobname=STRING\n      --output-directory=DIR   [default: somewhere in the temporary directory]\n      --output-format=FORMAT   FORMAT is `pdf' or `dvi'.  [default: pdf]\n\n" .. "Copyright (C) 2016-2024  ARATA Mizuki\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <http://www.gnu.org/licenses/>.\n"})
  tmp15(0, true)
end
_L[374] = function(payload)
  return {tag = "OPT_ENGINE", payload = payload}
end
_L[375] = function(payload)
  return {tag = "OPT_ENGINE_EXECUTABLE", payload = payload}
end
_L[376] = function(payload)
  return {tag = "OPT_OUTPUT", payload = payload}
end
_L[377] = {tag = "OPT_FRESH"}
_L[378] = function(payload)
  return {tag = "OPT_MAX_ITERATIONS", payload = payload}
end
_L[379] = {tag = "OPT_START_WITH_DRAFT"}
_L[380] = function(payload)
  return {tag = "OPT_WATCH", payload = payload}
end
_L[381] = {tag = "OPT_HELP"}
_L[382] = {tag = "OPT_VERSION"}
_L[383] = {tag = "OPT_VERBOSE"}
_L[384] = function(payload)
  return {tag = "OPT_COLOR", payload = payload}
end
_L[385] = function(payload)
  return {tag = "OPT_INCLUDEONLY", payload = payload}
end
_L[386] = function(payload)
  return {tag = "OPT_MAKE_DEPENDS", payload = payload}
end
_L[387] = {tag = "OPT_PRINT_OUTPUT_DIRECTORY"}
_L[388] = function(payload)
  return {tag = "OPT_PACKAGE_SUPPORT", payload = payload}
end
_L[389] = function(payload)
  return {tag = "OPT_CHECK_DRIVER", payload = payload}
end
_L[390] = function(payload)
  return {tag = "OPT_SOURCE_DATE_EPOCH", payload = payload}
end
_L[391] = function(payload)
  return {tag = "OPT_SYNCTEX", payload = payload}
end
_L[392] = function(payload)
  return {tag = "OPT_INTERACTION", payload = payload}
end
_L[393] = function(payload)
  return {tag = "OPT_JOBNAME", payload = payload}
end
_L[394] = function(payload)
  return {tag = "OPT_FMT", payload = payload}
end
_L[395] = function(payload)
  return {tag = "OPT_OUTPUT_DIRECTORY", payload = payload}
end
_L[396] = function(payload)
  return {tag = "OPT_OUTPUT_FORMAT", payload = payload}
end
_L[397] = function(payload)
  return {tag = "OPT_TEX_OPTION", payload = payload}
end
_L[398] = function(payload)
  return {tag = "OPT_TEX_OPTIONS", payload = payload}
end
_L[399] = function(payload)
  return {tag = "OPT_DVIPDFMX_OPTION", payload = payload}
end
_L[400] = function(payload)
  return {tag = "OPT_DVIPDFMX_OPTIONS", payload = payload}
end
_L[401] = function(payload)
  return {tag = "OPT_MAKEINDEX", payload = payload}
end
_L[402] = function(payload)
  return {tag = "OPT_BIBTEX", payload = payload}
end
_L[403] = function(payload)
  return {tag = "OPT_BIBER", payload = payload}
end
_L[404] = function(payload)
  return {tag = "OPT_MAKEGLOSSARIES", payload = payload}
end
_L[405] = function(payload)
  return {tag = "OPT_CONFIG_FILE", payload = payload}
end
_L[406] = {tag = "SHORT", payload = "-e"}
_L[407] = {_L[406], {tag = "WITH_ARG", payload = _L[374]}}
_L[408] = {tag = "LONG", payload = "--engine"}
_L[409] = {_L[408], {tag = "WITH_ARG", payload = _L[374]}}
_L[410] = {tag = "LONG", payload = "--engine-executable"}
_L[411] = {_L[410], {tag = "WITH_ARG", payload = _L[375]}}
_L[412] = {tag = "SHORT", payload = "-o"}
_L[413] = {_L[412], {tag = "WITH_ARG", payload = _L[376]}}
_L[414] = {tag = "LONG", payload = "--output"}
_L[415] = {_L[414], {tag = "WITH_ARG", payload = _L[376]}}
_L[416] = {tag = "LONG", payload = "--fresh"}
_L[417] = {_L[416], {tag = "SIMPLE", payload = _L[377]}}
_L[418] = {tag = "LONG", payload = "--max-iterations"}
_L[419] = {_L[418], {tag = "WITH_ARG", payload = _L[378]}}
_L[420] = {tag = "LONG", payload = "--start-with-draft"}
_L[421] = {_L[420], {tag = "SIMPLE", payload = _L[379]}}
_L[422] = {tag = "LONG", payload = "--change-directory"}
_L[423] = {_L[422], {tag = "SIMPLE", payload = {tag = "OPT_CHANGE_DIRECTORY", payload = true}}}
_L[424] = {tag = "LONG", payload = "--no-change-directory"}
_L[425] = {_L[424], {tag = "SIMPLE", payload = {tag = "OPT_CHANGE_DIRECTORY", payload = false}}}
_L[426] = {tag = "LONG", payload = "--watch"}
_L[427] = {_L[426], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[380], default = "auto"}}}
_L[428] = {tag = "SHORT", payload = "-h"}
_L[429] = {_L[428], {tag = "SIMPLE", payload = _L[381]}}
_L[430] = {tag = "LONG", payload = "-help"}
_L[431] = {_L[430], {tag = "SIMPLE", payload = _L[381]}}
_L[432] = {tag = "LONG", payload = "--help"}
_L[433] = {_L[432], {tag = "SIMPLE", payload = _L[381]}}
_L[434] = {tag = "SHORT", payload = "-v"}
_L[435] = {_L[434], {tag = "SIMPLE", payload = _L[382]}}
_L[436] = {tag = "LONG", payload = "--version"}
_L[437] = {_L[436], {tag = "SIMPLE", payload = _L[382]}}
_L[438] = {tag = "SHORT", payload = "-V"}
_L[439] = {_L[438], {tag = "SIMPLE", payload = _L[383]}}
_L[440] = {tag = "LONG", payload = "--verbose"}
_L[441] = {_L[440], {tag = "SIMPLE", payload = _L[383]}}
_L[442] = {tag = "LONG", payload = "--color"}
_L[443] = {_L[442], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[384], default = "always"}}}
_L[444] = {tag = "LONG", payload = "--includeonly"}
_L[445] = {_L[444], {tag = "WITH_ARG", payload = _L[385]}}
_L[446] = {tag = "LONG", payload = "--make-depends"}
_L[447] = {_L[446], {tag = "WITH_ARG", payload = _L[386]}}
_L[448] = {tag = "LONG", payload = "--print-output-directory"}
_L[449] = {_L[448], {tag = "SIMPLE", payload = _L[387]}}
_L[450] = {tag = "LONG", payload = "--package-support"}
_L[451] = {_L[450], {tag = "WITH_ARG", payload = _L[388]}}
_L[452] = {tag = "LONG", payload = "--check-driver"}
_L[453] = {_L[452], {tag = "WITH_ARG", payload = _L[389]}}
_L[454] = {tag = "LONG", payload = "--source-date-epoch"}
_L[455] = {_L[454], {tag = "WITH_ARG", payload = _L[390]}}
_L[456] = {tag = "LONG", payload = "-synctex"}
_L[457] = {_L[456], {tag = "WITH_ARG", payload = _L[391]}}
_L[458] = {tag = "LONG", payload = "--synctex"}
_L[459] = {_L[458], {tag = "WITH_ARG", payload = _L[391]}}
_L[460] = {tag = "LONG", payload = "-file-line-error"}
_L[461] = {_L[460], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = true}}}
_L[462] = {tag = "LONG", payload = "--file-line-error"}
_L[463] = {_L[462], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = true}}}
_L[464] = {tag = "LONG", payload = "-no-file-line-error"}
_L[465] = {_L[464], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = false}}}
_L[466] = {tag = "LONG", payload = "--no-file-line-error"}
_L[467] = {_L[466], {tag = "SIMPLE", payload = {tag = "OPT_FILE_LINE_ERROR", payload = false}}}
_L[468] = {tag = "LONG", payload = "-interaction"}
_L[469] = {_L[468], {tag = "WITH_ARG", payload = _L[392]}}
_L[470] = {tag = "LONG", payload = "--interaction"}
_L[471] = {_L[470], {tag = "WITH_ARG", payload = _L[392]}}
_L[472] = {tag = "LONG", payload = "-halt-on-error"}
_L[473] = {_L[472], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = true}}}
_L[474] = {tag = "LONG", payload = "--halt-on-error"}
_L[475] = {_L[474], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = true}}}
_L[476] = {tag = "LONG", payload = "-no-halt-on-error"}
_L[477] = {_L[476], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = false}}}
_L[478] = {tag = "LONG", payload = "--no-halt-on-error"}
_L[479] = {_L[478], {tag = "SIMPLE", payload = {tag = "OPT_HALT_ON_ERROR", payload = false}}}
_L[480] = {tag = "LONG", payload = "-shell-escape"}
_L[481] = {_L[480], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[226]}}}
_L[482] = {tag = "LONG", payload = "--shell-escape"}
_L[483] = {_L[482], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[226]}}}
_L[484] = {tag = "LONG", payload = "-no-shell-escape"}
_L[485] = {_L[484], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[228]}}}
_L[486] = {tag = "LONG", payload = "--no-shell-escape"}
_L[487] = {_L[486], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[228]}}}
_L[488] = {tag = "LONG", payload = "-shell-restricted"}
_L[489] = {_L[488], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[227]}}}
_L[490] = {tag = "LONG", payload = "--shell-restricted"}
_L[491] = {_L[490], {tag = "SIMPLE", payload = {tag = "OPT_SHELL_ESCAPE", payload = _L[227]}}}
_L[492] = {tag = "LONG", payload = "-jobname"}
_L[493] = {_L[492], {tag = "WITH_ARG", payload = _L[393]}}
_L[494] = {tag = "LONG", payload = "--jobname"}
_L[495] = {_L[494], {tag = "WITH_ARG", payload = _L[393]}}
_L[496] = {tag = "LONG", payload = "-fmt"}
_L[497] = {_L[496], {tag = "WITH_ARG", payload = _L[394]}}
_L[498] = {tag = "LONG", payload = "--fmt"}
_L[499] = {_L[498], {tag = "WITH_ARG", payload = _L[394]}}
_L[500] = {tag = "LONG", payload = "-output-directory"}
_L[501] = {_L[500], {tag = "WITH_ARG", payload = _L[395]}}
_L[502] = {tag = "LONG", payload = "--output-directory"}
_L[503] = {_L[502], {tag = "WITH_ARG", payload = _L[395]}}
_L[504] = {tag = "LONG", payload = "-output-format"}
_L[505] = {_L[504], {tag = "WITH_ARG", payload = _L[396]}}
_L[506] = {tag = "LONG", payload = "--output-format"}
_L[507] = {_L[506], {tag = "WITH_ARG", payload = _L[396]}}
_L[508] = {tag = "LONG", payload = "--tex-option"}
_L[509] = {_L[508], {tag = "WITH_ARG", payload = _L[397]}}
_L[510] = {tag = "LONG", payload = "--tex-options"}
_L[511] = {_L[510], {tag = "WITH_ARG", payload = _L[398]}}
_L[512] = {tag = "LONG", payload = "--dvipdfmx-option"}
_L[513] = {_L[512], {tag = "WITH_ARG", payload = _L[399]}}
_L[514] = {tag = "LONG", payload = "--dvipdfmx-options"}
_L[515] = {_L[514], {tag = "WITH_ARG", payload = _L[400]}}
_L[516] = {tag = "LONG", payload = "--makeindex"}
_L[517] = {_L[516], {tag = "WITH_ARG", payload = _L[401]}}
_L[518] = {tag = "LONG", payload = "--bibtex"}
_L[519] = {_L[518], {tag = "WITH_ARG", payload = _L[402]}}
_L[520] = {tag = "LONG", payload = "--biber"}
_L[521] = {_L[520], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[403], default = "biber"}}}
_L[522] = {tag = "LONG", payload = "--makeglossaries"}
_L[523] = {_L[522], {tag = "WITH_OPTIONAL_ARG", payload = {action = _L[404], default = "makeglossaries"}}}
_L[524] = {tag = "LONG", payload = "--config-file"}
_L[525] = _list({n = 60, _L[407], _L[409], _L[411], _L[413], _L[415], _L[417], _L[419], _L[421], _L[423], _L[425], _L[427], _L[429], _L[431], _L[433], _L[435], _L[437], _L[439], _L[441], _L[443], _L[445], _L[447], _L[449], _L[451], _L[453], _L[455], _L[457], _L[459], _L[461], _L[463], _L[465], _L[467], _L[469], _L[471], _L[473], _L[475], _L[477], _L[479], _L[481], _L[483], _L[485], _L[487], _L[489], _L[491], _L[493], _L[495], _L[497], _L[499], _L[501], _L[503], _L[505], _L[507], _L[509], _L[511], _L[513], _L[515], _L[517], _L[519], _L[521], _L[523], {_L[524], {tag = "WITH_ARG", payload = _L[405]}}})
_L[526] = function(command, recover)
  local status_or_signal, termination, success_or_recovered
  do
    _L[300](command)
    local tmp54, tmp55, tmp56 = tmp14(command)
    local success
    if tmp6(tmp54) == "number" then
      status_or_signal = tmp54
      termination = NONE
      success = tmp54 == 0
    else
      status_or_signal = tmp56
      termination = {tag = "SOME", payload = tmp55}
      success = tmp54
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
  end
  if termination.tag == "SOME" and termination.payload == "exit" then
  else
    goto else1
  end
  _L[301]("Command exited abnormally: exit status " .. tmp5(status_or_signal))
  _raise(_L[371], "main.sml:161:16")
  ::else1::
  if termination.tag == "SOME" and termination.payload == "signal" then
  else
    goto else2
  end
  _L[301]("Command exited abnormally: signal " .. tmp5(status_or_signal))
  _raise(_L[371], "main.sml:161:16")
  ::else2::
  _L[301]("Command exited abnormally: " .. tmp5(status_or_signal))
  _raise(_L[371], "main.sml:161:16")
end
_L[527] = {tag = "NO_NEED_TO_RERUN"}
_L[528] = {tag = "NO_PAGES_OF_OUTPUT"}
_L[529] = function(engine_type, tmp54, tmp55, tmp56, inputfile, exp, exp1, tmp57, exp2, tmp58, exp3, exp4, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, exp5, tmp68, tmp69, original_wd, output_extension, recorderfile, recorderfile2, source_date_epoch_info, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82)
  local tmp83
  do
    local tmp84, tmp85 = 0, _L[203]
    ::cont1::
    do
      local tmp86, exp6
      do
        local iteration, auxstatus = tmp84, tmp85
        tmp86 = _Int_add(iteration, 1)
        do
          local tmp87, pdfx, minted, filelist, epstopdf, bibtex_aux_hash, auxstatus1
          do
            local tmp88 = tmp58 .. "." .. "aux"
            tmp87 = _L[210].join(tmp62, tmp88)
            local tmp89 = _L[218](recorderfile)
            do
              if tmp89 then
              else
                goto else2
              end
              do
                local recorded, tmp90
                do
                  recorded = _L[360](recorderfile, tmp59, nil, _L[203])
                  local tmp91 = _L[237](engine_type, _L[235])
                  do
                    if tmp91 then
                    else
                      tmp90 = false
                      goto cont13
                    end
                    tmp90 = _L[218](recorderfile2)
                  end
                end
                ::cont13::
                local recorded1
                do
                  if tmp90 then
                  else
                    recorded1 = recorded
                    goto cont14
                  end
                  do
                    local fileList = recorded[1]
                    recorded1 = _L[360](recorderfile2, tmp59, fileList, recorded[2])
                  end
                end
                ::cont14::
                local filelist1, auxstatus2, minted1, epstopdf1, pdfx1, bibtex_aux_hash1
                do
                  local exp7
                  do
                    local fileInfo = recorded1[1]
                    exp7 = _L[359](fileInfo, recorded1[2])
                  end
                  filelist1 = exp7[1]
                  do
                    local go = function(a)
                      local abspath = a[1].abspath
                      local kind = a[1].kind
                      local auxstatus3 = a[2]
                      local tmp91 = _L[218](abspath)
                      if tmp91 then
                      else
                        return auxstatus3
                      end
                      do
                        local auxstatus4, status
                        do
                          local exp8 = _L[205](auxstatus3, abspath)
                          do
                            if exp8.tag == "NONE" then
                            elseif exp8.tag == "SOME" then
                              auxstatus4 = auxstatus3
                              status = exp8.payload
                              goto cont
                            else
                              _raise(_Match, "reruncheck.sml:124:51")
                            end
                            do
                              local tmp92 = {{md5sum = NONE, mtime = NONE, size = NONE}}
                              local tmp93 = _L[204](auxstatus3, abspath, tmp92)
                              auxstatus4 = tmp93
                              status = tmp92
                            end
                          end
                        end
                        ::cont::
                        if kind == "INPUT" then
                        else
                          goto else1
                        end
                        do
                          if status[1].mtime.tag == "NONE" then
                          else
                            return auxstatus4
                          end
                          do
                            local s = status[1]
                            local tmp92 = s.md5sum
                            local tmp93 = s.size
                            local tmp94 = _L[66](abspath)
                            status[1] = {md5sum = tmp92, mtime = {tag = "SOME", payload = tmp94}, size = tmp93}
                            return auxstatus4
                          end
                        end
                        ::else1::
                        if kind == "AUXILIARY" then
                        elseif kind == "OUTPUT" then
                          return auxstatus4
                        else
                          _raise(_Match, "reruncheck.sml:129:24")
                        end
                        do
                          local s
                          do
                            local x = status[1]
                            do
                              if x.mtime.tag == "NONE" then
                              else
                                s = x
                                goto cont1
                              end
                              do
                                local tmp92 = x.md5sum
                                local tmp93 = x.size
                                local tmp94 = _L[66](abspath)
                                s = {md5sum = tmp92, mtime = {tag = "SOME", payload = tmp94}, size = tmp93}
                              end
                            end
                          end
                          ::cont1::
                          local s1
                          do
                            if s.size.tag == "NONE" then
                            else
                              s1 = s
                              goto cont2
                            end
                            do
                              local tmp92 = s.md5sum
                              local tmp93 = s.mtime
                              local tmp94 = _L[67](abspath)
                              s1 = {md5sum = tmp92, mtime = tmp93, size = {tag = "SOME", payload = tmp94}}
                            end
                          end
                          ::cont2::
                          if s1.md5sum.tag == "NONE" then
                          else
                            status[1] = s1
                            return auxstatus4
                          end
                          do
                            local tmp92 = s1.mtime
                            local tmp93 = s1.size
                            local tmp94
                            do
                              local ins = _L[151](abspath)
                              local tmp95 = _L[153](ins)
                              _L[152](ins)
                              tmp94 = _L[191](tmp95)
                            end
                            status[1] = {md5sum = {tag = "SOME", payload = tmp94}, mtime = tmp92, size = tmp93}
                            return auxstatus4
                          end
                        end
                      end
                    end
                    local tmp91 = _L[206](_EXCLAM)
                    local tmp92 = foldl(go)
                    local tmp93 = _L[206](ref)
                    local tmp94 = tmp93(auxstatus)
                    local tmp95 = tmp92(tmp94)
                    local tmp96 = tmp95(filelist1)
                    auxstatus2 = tmp91(tmp96)
                  end
                  local tmp91 = foldl(function(a)
                    local path, epstopdf2, pdfx2, tmp92
                    do
                      path = a[1].path
                      local minted2 = a[2].minted
                      epstopdf2 = a[2].epstopdf
                      pdfx2 = a[2].pdfx
                      do
                        if minted2 then
                          tmp92 = true
                          goto cont
                        end
                        local tmp93 = isSuffix("minted/minted.sty")
                        tmp92 = tmp93(path)
                      end
                    end
                    ::cont::
                    local tmp93
                    do
                      if epstopdf2 then
                        tmp93 = true
                        goto cont1
                      end
                      local tmp94 = isSuffix("epstopdf.sty")
                      tmp93 = tmp94(path)
                    end
                    ::cont1::
                    local tmp94
                    do
                      if pdfx2 then
                        tmp94 = true
                        goto cont2
                      end
                      local tmp95 = isSuffix("pdfx.sty")
                      tmp94 = tmp95(path)
                    end
                    ::cont2::
                    return {epstopdf = tmp93, minted = tmp92, pdfx = tmp94}
                  end)
                  local tmp92 = tmp91({epstopdf = false, minted = false, pdfx = false})
                  local exp8 = tmp92(filelist1)
                  minted1 = exp8.minted
                  epstopdf1 = exp8.epstopdf
                  pdfx1 = exp8.pdfx
                  local tmp93 = exp.tag == "SOME" and exp.payload.tag == "BIBTEX"
                  do
                    if tmp93 then
                    else
                      bibtex_aux_hash1 = NONE
                      goto cont15
                    end
                    do
                      local biblines
                      do
                        local tmp94 = _L[353](tmp87, tmp62, nil)
                        biblines = revAppend(tmp94, nil)
                      end
                      local tmp94 = _L[191](table_concat(_VectorOrArray_fromList(biblines), "\n"))
                      bibtex_aux_hash1 = {tag = "SOME", payload = tmp94}
                    end
                  end
                end
                ::cont15::
                pdfx = pdfx1
                minted = minted1
                filelist = filelist1
                epstopdf = epstopdf1
                bibtex_aux_hash = bibtex_aux_hash1
                auxstatus1 = auxstatus2
                goto cont3
              end
              ::else2::
              if auxstatus[2].tag == "E" then
                pdfx = false
                minted = false
                filelist = nil
                epstopdf = false
                bibtex_aux_hash = NONE
                auxstatus1 = _L[203]
                goto cont3
              end
              _L[301]("Recorder file was not generated during the execution!")
              _raise(_L[371], "main.sml:218:27")
            end
          end
          ::cont3::
          do
            if source_date_epoch_info.tag == "NONE" then
              goto cont4
            end
            if source_date_epoch_info.tag == "SOME" then
            else
              _raise(_Match, "main.sml:229:20")
            end
            do
              local r = source_date_epoch_info.payload
              local tmp88 = exp5.tag == "SOME" and exp5.payload.tag == "NOW"
              local should_set_source_date_epoch
              if tmp88 then
                should_set_source_date_epoch = true
              elseif exp5.tag == "SOME" and exp5.payload.tag == "RAW" then
                should_set_source_date_epoch = false
              elseif exp5.tag == "NONE" then
                should_set_source_date_epoch = pdfx or tmp66
              else
                _raise(_Match, "main.sml:232:54")
              end
              if should_set_source_date_epoch then
              else
                goto cont4
              end
              do
                local tmp89
                do
                  local tmp90 = foldl(function(a)
                    if a[1].kind == "INPUT" then
                    else
                      return a[2]
                    end
                    do
                      local abspath = a[1].abspath
                      local acc = a[2]
                      local exp7 = _L[205](auxstatus1, abspath)
                      if exp7.tag == "SOME" then
                        local mtime = exp7.payload.mtime
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
                          _raise(_Match, "main.sml:240:68")
                        end
                      elseif exp7.tag == "NONE" then
                        return acc
                      else
                        _raise(_Match, "main.sml:238:59")
                      end
                    end
                  end)
                  local tmp91 = tmp90(NONE)
                  local input_time = tmp91(filelist)
                  do
                    if input_time.tag == "SOME" then
                    elseif input_time.tag == "NONE" then
                      tmp89 = r[1].time_since_epoch
                      goto cont13
                    else
                      _raise(_Match, "main.sml:253:37")
                    end
                    do
                      local input_time1 = input_time.payload
                      if r[1].time < input_time1 then
                      else
                        tmp89 = r[1].time_since_epoch
                        goto cont13
                      end
                      do
                        local tmp92 = tmp5(tmp18())
                        local new_info = {time = input_time1, time_since_epoch = tmp92}
                        if _ENV.CLUTTEX_VERBOSITY >= 1 then
                        else
                          r[1] = new_info
                          tmp89 = tmp92
                          goto cont13
                        end
                        _L[304]("Input file was modified; Updating SOURCE_DATE_EPOCH...")
                        r[1] = new_info
                        tmp89 = tmp92
                      end
                    end
                  end
                end
                ::cont13::
                if _ENV.CLUTTEX_VERBOSITY >= 1 then
                else
                  _L[221]("SOURCE_DATE_EPOCH", tmp89)
                  goto cont4
                end
                _L[304]("Setting SOURCE_DATE_EPOCH to " .. tmp89)
                _L[221]("SOURCE_DATE_EPOCH", tmp89)
              end
            end
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
              _raise(_Match, "main.sml:276:31")
            end
            local tmp88
            tmp88 = minted or tmp65
            do
              if tmp88 then
              else
                tex_injection = tex_injection1
                goto cont5
              end
              do
                do
                  if not tmp65 then
                  else
                    goto cont13
                  end
                  _L[303]("You may want to use --package-support=minted option.")
                end
                ::cont13::
                local outdir
                if _L[220] then
                  outdir = tmp21(tmp62, ".", function(a)
                    local tmp89 = tmp19(a)
                    if tmp89 == 92 then
                      return string_char(47)
                    else
                      return string_char(tmp89)
                    end
                  end)
                else
                  outdir = tmp62
                end
                tex_injection = tex_injection1 .. "\\PassOptionsToPackage{outputdir=" .. outdir .. "}{minted}"
              end
            end
          end
          ::cont5::
          local tex_injection1
          do
            local tmp88
            tmp88 = epstopdf or tmp64
            do
              if tmp88 then
              else
                tex_injection1 = tex_injection
                goto cont6
              end
              do
                do
                  if not tmp64 then
                  else
                    goto cont13
                  end
                  _L[303]("You may want to use --package-support=epstopdf option.")
                end
                ::cont13::
                local outdir
                if _L[220] then
                  outdir = tmp21(tmp62, ".", function(a)
                    local tmp89 = tmp19(a)
                    if tmp89 == 92 then
                      return string_char(47)
                    else
                      return string_char(tmp89)
                    end
                  end)
                else
                  outdir = tmp62
                end
                local tmp89 = isSuffix("/")
                local tmp90 = tmp89(outdir)
                local outdir1
                if tmp90 then
                  outdir1 = outdir
                else
                  outdir1 = outdir .. "/"
                end
                tex_injection1 = tex_injection .. "\\PassOptionsToPackage{outdir=" .. outdir1 .. "}{epstopdf}"
              end
            end
          end
          ::cont6::
          local lightweight_mode, command
          do
            local tmp88 = _L[237](engine_type, _L[233])
            local tmp89
            do
              local escaped
              do
                local tmp90, tmp91
                do
                  local tmp92 = full(tmp21(inputfile, ".", function(a)
                    local tmp93 = tmp19(a)
                    if tmp93 == 92 then
                      return "~\\\\"
                    elseif tmp93 == 37 then
                      return "~\\%"
                    elseif tmp93 == 94 then
                      return "~\\^"
                    elseif tmp93 == 123 then
                      return "~\\{"
                    elseif tmp93 == 125 then
                      return "~\\}"
                    elseif tmp93 == 126 then
                      return "~\\~"
                    elseif tmp93 == 35 then
                      return "~\\#"
                    else
                      return string_char(tmp93)
                    end
                  end))
                  tmp91, tmp90 = tmp92, nil
                end
                ::cont15::
                do
                  local acc, s
                  do
                    s, acc = tmp91, tmp90
                    local tmp92 = isEmpty(s)
                    if tmp92 then
                    else
                      goto else2
                    end
                    do
                      local tmp93 = revAppend(acc, nil)
                      escaped = concat1(tmp93)
                      goto cont13
                    end
                  end
                  ::else2::
                  local a, d, tmp92, tmp93
                  do
                    local tmp94 = splitl(function(c)
                      return c ~= 32
                    end)
                    local exp7 = tmp94(s)
                    a = exp7[1]
                    local b = exp7[2]
                    local tmp95 = splitl(function(c)
                      return c == 32
                    end)
                    local exp8 = tmp95(b)
                    local c = exp8[1]
                    d = exp8[2]
                    tmp92 = map(str)
                    do
                      local base, start, tmp96, tmp97
                      do
                        base = c.base
                        local length1 = c.length
                        start = c.start
                        tmp97, tmp96 = _Int_sub(_Int_add(start, length1), 1), nil
                      end
                      ::cont17::
                      do
                        local i, acc1 = tmp97, tmp96
                        if i < start then
                          tmp93 = acc1
                          goto cont16
                        end
                        local tmp98 = _Int_sub(i, 1)
                        local tmp99 = sub1({base, i})
                        tmp97 = tmp98
                        tmp96 = {tmp99, acc1}
                        goto cont17
                      end
                    end
                  end
                  ::cont16::
                  local tmp94 = tmp92(tmp93)
                  local c_PRIME = full(table_concat(_VectorOrArray_fromList(tmp94), "~"))
                  tmp91 = d
                  tmp90 = {c_PRIME, {a, acc}}
                  goto cont15
                end
              end
              ::cont13::
              local escaped1
              do
                if tmp88 then
                else
                  escaped1 = escaped
                  goto cont14
                end
                do
                  local tmp90, tmp91
                  do
                    local tmp92 = full(escaped)
                    tmp91, tmp90 = tmp92, nil
                  end
                  ::cont15::
                  do
                    local acc, s
                    do
                      s, acc = tmp91, tmp90
                      local tmp92 = isEmpty(s)
                      if tmp92 then
                      else
                        goto else2
                      end
                      do
                        local tmp93 = revAppend(acc, nil)
                        escaped1 = concat1(tmp93)
                        goto cont14
                      end
                    end
                    ::else2::
                    local a, d, c_PRIME
                    do
                      local tmp92 = splitl(isAscii)
                      local exp7 = tmp92(s)
                      a = exp7[1]
                      local b = exp7[2]
                      local tmp93 = splitl(function(a1)
                        return not (a1 <= 127)
                      end)
                      local exp8 = tmp93(b)
                      local c = exp8[1]
                      d = exp8[2]
                      local tmp94 = isEmpty(c)
                      do
                        if tmp94 then
                          c_PRIME = c
                          goto cont16
                        end
                        local tmp95 = string1(c)
                        c_PRIME = full("\\detokenize{" .. tmp95 .. "}")
                      end
                    end
                    ::cont16::
                    tmp91 = d
                    tmp90 = {c_PRIME, {a, acc}}
                    goto cont15
                  end
                end
              end
              ::cont14::
              if inputfile == escaped1 then
                tmp89 = "\\input\"" .. inputfile .. "\""
              else
                tmp89 = "\\begingroup\\escapechar-1\\let~\\string\\edef\\x{\"" .. escaped1 .. "\" }\\expandafter\\endgroup\\expandafter\\input\\x"
              end
            end
            local tmp90 = tex_injection1 .. tmp89
            local tmp91 = tmp86 == 1 and tmp68
            local tmp92, tmp93, tmp94, exp7, exp8, exp9, exp10, exp11, exp12, exp13, exp14, exp15, exp16
            if tmp91 then
              if tmp55 then
                lightweight_mode = true
                exp16 = tmp82
                exp15 = tmp81
                exp14 = tmp80
                exp13 = tmp79
                exp12 = tmp78
                exp11 = tmp77
                exp10 = {tag = "SOME", payload = _L[222]}
                exp9 = tmp75
                exp8 = tmp74
                exp7 = tmp73
                tmp94 = tmp72
                tmp93 = tmp71
                tmp92 = true
              else
                lightweight_mode = true
                exp16 = tmp82
                exp15 = tmp81
                exp14 = tmp80
                exp13 = tmp79
                exp12 = tmp78
                exp11 = tmp77
                exp10 = {tag = "SOME", payload = _L[222]}
                exp9 = tmp75
                exp8 = tmp74
                exp7 = tmp73
                tmp94 = tmp72
                tmp93 = tmp71
                tmp92 = tmp70
              end
            else
              lightweight_mode = false
              exp16 = tmp82
              exp15 = tmp81
              exp14 = tmp80
              exp13 = tmp79
              exp12 = tmp78
              exp11 = tmp77
              exp10 = tmp76
              exp9 = tmp75
              exp8 = tmp74
              exp7 = tmp73
              tmp94 = tmp72
              tmp93 = tmp71
              tmp92 = false
            end
            do
              local revCommand
              do
                local executable = getOpt(tmp93, tmp54)
                local revCommand1 = {"-recorder", {executable, nil}}
                local revCommand2
                if exp8.tag == "NONE" then
                  revCommand2 = revCommand1
                elseif exp8.tag == "SOME" then
                  revCommand2 = {"-fmt=" .. exp8.payload, revCommand1}
                else
                  _raise(_Match, "tex-engine.sml:58:28")
                end
                local revCommand3
                if not exp9 then
                  revCommand3 = revCommand2
                elseif exp9 then
                  revCommand3 = {"-halt-on-error", revCommand2}
                else
                  _raise(_Match, "tex-engine.sml:61:28")
                end
                if exp10.tag == "NONE" then
                  revCommand = revCommand3
                  goto cont13
                end
                if exp10.tag == "SOME" then
                  local mode = exp10.payload
                  local tmp95
                  if mode == "BATCHMODE" then
                    tmp95 = "batchmode"
                  elseif mode == "NONSTOPMODE" then
                    tmp95 = "nonstopmode"
                  elseif mode == "SCROLLMODE" then
                    tmp95 = "scrollmode"
                  elseif mode == "ERRORSTOPMODE" then
                    tmp95 = "errorstopmode"
                  else
                    _raise(_Match, "types.sml:14:7")
                  end
                  revCommand = {"-interaction=" .. tmp95, revCommand3}
                else
                  _raise(_Match, "tex-engine.sml:64:28")
                end
              end
              ::cont13::
              local revCommand1
              do
                local revCommand2
                if not exp7 then
                  revCommand2 = revCommand
                elseif exp7 then
                  revCommand2 = {"-file-line-error", revCommand}
                else
                  _raise(_Match, "tex-engine.sml:67:28")
                end
                do
                  if exp16.tag == "NONE" then
                    revCommand1 = revCommand2
                    goto cont14
                  end
                  if exp16.tag == "SOME" then
                  else
                    _raise(_Match, "tex-engine.sml:70:28")
                  end
                  do
                    local tmp95 = _L[208](exp16.payload)
                    revCommand1 = {"-synctex=" .. tmp95, revCommand2}
                  end
                end
              end
              ::cont14::
              local revCommand2
              do
                local revCommand3
                if exp15.tag == "NONE" then
                  revCommand3 = revCommand1
                elseif exp15.tag == "SOME" and exp15.payload == "FORBIDDEN" then
                  revCommand3 = {"-no-shell-escape", revCommand1}
                elseif exp15.tag == "SOME" and exp15.payload == "RESTRICTED" then
                  revCommand3 = {"-shell-restricted", revCommand1}
                elseif exp15.tag == "SOME" and exp15.payload == "ALLOWED" then
                  revCommand3 = {"-shell-escape", revCommand1}
                else
                  _raise(_Match, "tex-engine.sml:73:28")
                end
                do
                  if exp11.tag == "NONE" then
                    revCommand2 = revCommand3
                    goto cont15
                  end
                  if exp11.tag == "SOME" then
                  else
                    _raise(_Match, "tex-engine.sml:78:28")
                  end
                  do
                    local tmp95 = _L[208](exp11.payload)
                    revCommand2 = {"-jobname=" .. tmp95, revCommand3}
                  end
                end
              end
              ::cont15::
              local revCommand3
              do
                if exp13.tag == "NONE" then
                  revCommand3 = revCommand2
                  goto cont16
                end
                if exp13.tag == "SOME" then
                else
                  _raise(_Match, "tex-engine.sml:81:28")
                end
                do
                  local tmp95 = _L[208](exp13.payload)
                  revCommand3 = {"-output-directory=" .. tmp95, revCommand2}
                end
              end
              ::cont16::
              local revCommand4
              do
                if engine_type == "OTHER" then
                  revCommand4 = revCommand3
                  goto cont17
                end
                if engine_type == "PDFTEX" then
                  local revCommand5
                  if tmp92 then
                    revCommand5 = {"-draftmode", revCommand3}
                  else
                    revCommand5 = revCommand3
                  end
                  if exp14 == "DVI" then
                    revCommand4 = {"-output-format=dvi", revCommand5}
                    goto cont17
                  elseif exp14 == "PDF" then
                    revCommand4 = revCommand5
                    goto cont17
                  else
                    _raise(_Match, "tex-engine.sml:90:45")
                  end
                end
                if engine_type == "XETEX" then
                else
                  goto else2
                end
                do
                  local tmp95
                  if tmp92 then
                    tmp95 = true
                  else
                    tmp95 = _L[232](exp14, _L[231])
                  end
                  if tmp95 then
                    revCommand4 = {"-no-pdf", revCommand3}
                    goto cont17
                  else
                    revCommand4 = revCommand3
                    goto cont17
                  end
                end
                ::else2::
                if engine_type == "LUATEX" then
                else
                  _raise(_Match, "tex-engine.sml:84:28")
                end
                do
                  local revCommand5
                  do
                    if exp12.tag == "NONE" then
                      revCommand5 = revCommand3
                      goto cont18
                    end
                    if exp12.tag == "SOME" then
                    else
                      _raise(_Match, "tex-engine.sml:98:63")
                    end
                    do
                      local tmp95 = _L[208](exp12.payload)
                      revCommand5 = {"--lua=" .. tmp95, revCommand3}
                    end
                  end
                  ::cont18::
                  local revCommand6
                  if tmp92 then
                    revCommand6 = {"--draftmode", revCommand5}
                  else
                    revCommand6 = revCommand5
                  end
                  if exp14 == "DVI" then
                    revCommand4 = {"--output-format=dvi", revCommand6}
                  elseif exp14 == "PDF" then
                    revCommand4 = revCommand6
                  else
                    _raise(_Match, "tex-engine.sml:105:45")
                  end
                end
              end
              ::cont17::
              local revCommand5 = revAppend(tmp94, revCommand4)
              local tmp95 = _L[208](tmp90)
              local tmp96 = revAppend({tmp95, revCommand5}, nil)
              command = table_concat(_VectorOrArray_fromList(tmp96), " ")
            end
          end
          local getExecLog, recorded, tmp88
          do
            local tmp89 = {NONE}
            getExecLog = function()
              local x = tmp89[1]
              if x.tag == "NONE" then
              elseif x.tag == "SOME" then
                return x.payload
              else
                _raise(_Match, "main.sml:322:31")
              end
              do
                local tmp90 = tmp58 .. "." .. "log"
                local ins = _L[115](_L[210].join(tmp62, tmp90))
                local log = _L[123](ins)
                _L[120](ins)
                tmp89[1] = {tag = "SOME", payload = log}
                return log
              end
            end
            local tmp90 = {false}
            _L[526](command, {tag = "SOME", payload = function(a)
              local execlog = getExecLog()
              local tmp91 = _L[210].join(tmp62, tmp58 .. "." .. "aux")
              local r
              do
                local recovered
                do
                  local tmp92 = _L[49]("I can't write on file")
                  local tmp93 = tmp92(execlog)
                  if tmp93 then
                  else
                    recovered = false
                    goto cont
                  end
                  do
                    local exp7 = _L[352](tmp91, tmp62, _L[197])
                    local madeNewDirectory = exp7[1]
                    local tmp94
                    tmp94 = madeNewDirectory and _ENV.CLUTTEX_VERBOSITY >= 1
                    if tmp94 then
                    else
                      recovered = madeNewDirectory
                      goto cont
                    end
                    _L[304]("Created missing directories.")
                    recovered = madeNewDirectory
                  end
                end
                ::cont::
                local tmp92
                do
                  local tmp93 = tmp(_L[229])
                  local tmp94
                  do
                    local tmp95 = tmp93({tmp67, {tag = "SOME", payload = _L[228]}})
                    tmp94 = not tmp95
                  end
                  if tmp94 then
                  else
                    tmp92 = false
                    goto cont1
                  end
                  do
                    local tmp95 = tokens(function(c)
                      return c == 10
                    end)
                    local tmp96 = full(execlog)
                    local lines = tmp95(tmp96)
                    local tmp97 = foldl(function(a1)
                      local line = a1[1]
                      local run = a1[2]
                      local tmp98 = map(string1)
                      local tmp99 = tokens(isSpace)
                      local tmp100 = tmp99(line)
                      local exp7 = tmp98(tmp100)
                      if exp7 ~= nil and (exp7[1] == "(epstopdf)" and (exp7[2] ~= nil and (exp7[2][1] == "Command:" and (exp7[2][2] ~= nil and (exp7[2][2][2] ~= nil and (exp7[2][2][2][2] ~= nil and exp7[2][2][2][2][2] == nil)))))) then
                      else
                        return run
                      end
                      do
                        local tmp101, tmp102, tmp103
                        do
                          local tmp104 = exp7[2][2][1]
                          tmp101 = exp7[2][2][2][1]
                          tmp102 = exp7[2][2][2][2][1]
                          do
                            if tmp104 == "<epstopdf" then
                              tmp103 = true
                              goto cont
                            end
                            if tmp104 == "<repstopdf" then
                            else
                              tmp103 = false
                              goto cont
                            end
                            do
                              local tmp105 = isPrefix("--outfile=")
                              local tmp106 = tmp105(tmp101)
                              if tmp106 then
                              else
                                tmp103 = false
                                goto cont
                              end
                              do
                                local tmp107 = isSuffix(">")
                                tmp103 = tmp107(tmp102)
                              end
                            end
                          end
                        end
                        ::cont::
                        if tmp103 then
                        else
                          return run
                        end
                        do
                          local outfile = extract(tmp101, 10, NONE)
                          local infile = substring(tmp102, 0, _Int_sub(#tmp102, 1))
                          local infileAbs = _L[215]({tag = "SOME", payload = original_wd}, infile)
                          local tmp104 = _L[218](infileAbs)
                          if tmp104 then
                          else
                            return run
                          end
                          do
                            local outfileAbs = _L[215]({tag = "SOME", payload = tmp62}, outfile)
                            do
                              if _ENV.CLUTTEX_VERBOSITY >= 1 then
                              else
                                goto cont1
                              end
                              _L[304]("Running epstopdf on " .. infile .. ".")
                            end
                            ::cont1::
                            do
                              local outdir = _L[212](outfileAbs)
                              local tmp105 = _L[219](outdir)
                              do
                                if not tmp105 then
                                else
                                  goto cont2
                                end
                                do
                                  local tmp106, tmp107 = _L[217].mkdir_rec(outdir)
                                  if not tmp106 then
                                  else
                                    goto cont2
                                  end
                                  do
                                    local tmp108 = _Error(tmp107)
                                    _raise(tmp108, "fs-util.sml:16:28")
                                  end
                                end
                              end
                            end
                            ::cont2::
                            local tmp105 = _L[208](outfileAbs)
                            local tmp106 = "epstopdf --outfile=" .. tmp105 .. " "
                            local tmp107 = _L[208](infileAbs)
                            local tmp108 = tmp106 .. tmp107
                            _L[300](tmp108)
                            tmp14(tmp108)
                            return run
                          end
                        end
                      end
                    end)
                    local tmp98 = tmp97(false)
                    tmp92 = tmp98(lines)
                  end
                end
                ::cont1::
                local recovered1
                recovered1 = tmp92 or recovered
                if recovered1 then
                  tmp90[1] = true
                  return true
                end
                local tmp93 = _L[49]("Package minted Error: Missing Pygments output; \\inputminted was")
                r = tmp93(execlog)
              end
              tmp90[1] = true
              return r
            end})
            if tmp90[1] then
              exp6 = {tag = "SHOULD_RERUN", payload = _L[203]}
              goto cont2
            end
            recorded = _L[360](recorderfile, tmp59, nil, _L[203])
            local tmp91 = _L[237](engine_type, _L[235])
            do
              if tmp91 then
              else
                tmp88 = false
                goto cont7
              end
              tmp88 = _L[218](recorderfile2)
            end
          end
          ::cont7::
          local recorded1
          do
            if tmp88 then
            else
              recorded1 = recorded
              goto cont8
            end
            do
              local fileList = recorded[1]
              recorded1 = _L[360](recorderfile2, tmp59, fileList, recorded[2])
            end
          end
          ::cont8::
          local filelist1, execlog
          do
            local exp7
            do
              local fileInfo = recorded1[1]
              exp7 = _L[359](fileInfo, recorded1[2])
            end
            filelist1 = exp7[1]
            execlog = getExecLog()
            do
              if exp1.tag == "NONE" then
                goto cont9
              end
              if exp1.tag == "SOME" then
              else
                _raise(_Match, "main.sml:349:27")
              end
              do
                do
                  local driver = exp1.payload
                  local tmp89 = map(function(a)
                    local path = a.path
                    local kind = a.kind
                    if kind == "INPUT" then
                      return {kind = "input", path = path}
                    elseif kind == "OUTPUT" then
                      return {kind = "output", path = path}
                    elseif kind == "AUXILIARY" then
                      return {kind = "auxiliary", path = path}
                    else
                      _raise(_Match, "main.sml:351:141")
                    end
                  end)
                  local tmp90 = tmp89(filelist1)
                  do
                    do
                      if _ENV.CLUTTEX_VERBOSITY >= 1 then
                      else
                        goto cont14
                      end
                      do
                        if driver == "DVIPDFMX" then
                        else
                          goto else2
                        end
                        _L[304]("checkdriver: expects dvipdfmx")
                        goto cont14
                        ::else2::
                        if driver == "DVIPS" then
                        else
                          goto else3
                        end
                        _L[304]("checkdriver: expects dvips")
                        goto cont14
                        ::else3::
                        if driver == "DVISVGM" then
                        else
                          goto else4
                        end
                        _L[304]("checkdriver: expects dvisvgm")
                        goto cont14
                        ::else4::
                        if driver == "PDFTEX" then
                        else
                          goto else5
                        end
                        _L[304]("checkdriver: expects pdftex")
                        goto cont14
                        ::else5::
                        if driver == "XETEX" then
                        else
                          goto else6
                        end
                        _L[304]("checkdriver: expects xetex")
                        goto cont14
                        ::else6::
                        if driver == "LUATEX" then
                        else
                          _raise(_Match, "check-driver.sml:8:7")
                        end
                        _L[304]("checkdriver: expects luatex")
                      end
                    end
                    ::cont14::
                    local loadedSet, graphics_driver
                    do
                      local tmp91 = foldl(function(a)
                        local kind = a[1].kind
                        local path = a[1].path
                        local set = a[2]
                        if kind == "input" then
                        else
                          return set
                        end
                        do
                          local tmp92 = _L[211](path)
                          return _L[198](set, tmp92)
                        end
                      end)
                      local tmp92 = tmp91(_L[197])
                      loadedSet = tmp92(tmp90)
                      local tmp93 = _L[199](loadedSet, "graphics.sty")
                      local tmp94
                      if tmp93 then
                        tmp94 = true
                      else
                        tmp94 = _L[199](loadedSet, "color.sty")
                      end
                      do
                        if tmp94 then
                        else
                          graphics_driver = NONE
                          goto cont15
                        end
                        do
                          local tmp95 = _L[199](loadedSet, "dvipdfmx.def")
                          if tmp95 then
                            graphics_driver = {tag = "SOME", payload = _L[311]}
                            goto cont15
                          end
                          local tmp96 = _L[199](loadedSet, "dvips.def")
                          if tmp96 then
                            graphics_driver = {tag = "SOME", payload = _L[312]}
                            goto cont15
                          end
                          local tmp97 = _L[199](loadedSet, "dvisvgm.def")
                          if tmp97 then
                            graphics_driver = {tag = "SOME", payload = _L[313]}
                            goto cont15
                          end
                          local tmp98 = _L[199](loadedSet, "pdftex.def")
                          if tmp98 then
                            graphics_driver = {tag = "SOME", payload = _L[314]}
                            goto cont15
                          end
                          local tmp99 = _L[199](loadedSet, "luatex.def")
                          if tmp99 then
                            graphics_driver = {tag = "SOME", payload = _L[316]}
                            goto cont15
                          end
                          local tmp100 = _L[199](loadedSet, "xetex.def")
                          if tmp100 then
                            graphics_driver = {tag = "SOME", payload = _L[315]}
                          else
                            graphics_driver = {tag = "SOME", payload = _L[317]}
                          end
                        end
                      end
                    end
                    ::cont15::
                    local tmp91
                    do
                      local tmp92 = _L[199](loadedSet, "expl3-code.tex")
                      do
                        if tmp92 then
                          tmp91 = true
                          goto cont16
                        end
                        local tmp93 = _L[199](loadedSet, "expl3.sty")
                        if tmp93 then
                          tmp91 = true
                          goto cont16
                        end
                        local tmp94 = _L[199](loadedSet, "l3backend-dvips.def")
                        if tmp94 then
                          tmp91 = true
                          goto cont16
                        end
                        local tmp95 = _L[199](loadedSet, "l3backend-dvipdfmx.def")
                        if tmp95 then
                          tmp91 = true
                          goto cont16
                        end
                        local tmp96 = _L[199](loadedSet, "l3backend-xdvipdfmx.def")
                        if tmp96 then
                          tmp91 = true
                          goto cont16
                        end
                        local tmp97 = _L[199](loadedSet, "l3backend-pdfmode.def")
                        if tmp97 then
                          tmp91 = true
                          goto cont16
                        end
                        local tmp98 = _L[199](loadedSet, "l3backend-pdftex.def")
                        if tmp98 then
                          tmp91 = true
                          goto cont16
                        end
                        local tmp99 = _L[199](loadedSet, "l3backend-luatex.def")
                        if tmp99 then
                          tmp91 = true
                        else
                          tmp91 = _L[199](loadedSet, "l3backend-xetex.def")
                        end
                      end
                    end
                    ::cont16::
                    local expl3_driver
                    do
                      if tmp91 then
                      else
                        expl3_driver = NONE
                        goto cont17
                      end
                      do
                        local tmp92 = _L[199](loadedSet, "l3backend-pdfmode.def")
                        if tmp92 then
                          expl3_driver = {tag = "SOME", payload = _L[319]}
                          goto cont17
                        end
                        local tmp93 = _L[199](loadedSet, "l3backend-dvisvgm.def")
                        if tmp93 then
                          expl3_driver = {tag = "SOME", payload = _L[320]}
                          goto cont17
                        end
                        local tmp94 = _L[199](loadedSet, "l3backend-xdvipdfmx.def")
                        if tmp94 then
                          expl3_driver = {tag = "SOME", payload = _L[321]}
                          goto cont17
                        end
                        local tmp95 = _L[199](loadedSet, "l3backend-dvipdfmx.def")
                        if tmp95 then
                          expl3_driver = {tag = "SOME", payload = _L[322]}
                          goto cont17
                        end
                        local tmp96 = _L[199](loadedSet, "l3backend-dvips.def")
                        if tmp96 then
                          expl3_driver = {tag = "SOME", payload = _L[323]}
                          goto cont17
                        end
                        local tmp97 = _L[199](loadedSet, "l3backend-pdftex.def")
                        if tmp97 then
                          expl3_driver = {tag = "SOME", payload = _L[324]}
                          goto cont17
                        end
                        local tmp98 = _L[199](loadedSet, "l3backend-luatex.def")
                        if tmp98 then
                          expl3_driver = {tag = "SOME", payload = _L[325]}
                          goto cont17
                        end
                        local tmp99 = _L[199](loadedSet, "l3backend-xetex.def")
                        if tmp99 then
                          expl3_driver = {tag = "SOME", payload = _L[326]}
                        else
                          expl3_driver = {tag = "SOME", payload = _L[327]}
                        end
                      end
                    end
                    ::cont17::
                    local hyperref_driver
                    do
                      local tmp92 = _L[199](loadedSet, "hyperref.sty")
                      do
                        if tmp92 then
                        else
                          hyperref_driver = NONE
                          goto cont18
                        end
                        do
                          local tmp93 = _L[199](loadedSet, "hluatex.def")
                          if tmp93 then
                            hyperref_driver = {tag = "SOME", payload = _L[333]}
                            goto cont18
                          end
                          local tmp94 = _L[199](loadedSet, "hpdftex.def")
                          if tmp94 then
                            hyperref_driver = {tag = "SOME", payload = _L[332]}
                            goto cont18
                          end
                          local tmp95 = _L[199](loadedSet, "hxetex.def")
                          if tmp95 then
                            hyperref_driver = {tag = "SOME", payload = _L[334]}
                            goto cont18
                          end
                          local tmp96 = _L[199](loadedSet, "hdvipdfm.def")
                          if tmp96 then
                            hyperref_driver = {tag = "SOME", payload = _L[330]}
                            goto cont18
                          end
                          local tmp97 = _L[199](loadedSet, "hdvips.def")
                          if tmp97 then
                            hyperref_driver = {tag = "SOME", payload = _L[331]}
                          else
                            hyperref_driver = {tag = "SOME", payload = _L[335]}
                          end
                        end
                      end
                    end
                    ::cont18::
                    local xypic_driver
                    do
                      local tmp92 = _L[199](loadedSet, "xy.tex")
                      do
                        if tmp92 then
                        else
                          xypic_driver = NONE
                          goto cont19
                        end
                        do
                          local tmp93 = _L[199](loadedSet, "xypdf.tex")
                          if tmp93 then
                            xypic_driver = {tag = "SOME", payload = _L[337]}
                            goto cont19
                          end
                          local tmp94 = _L[199](loadedSet, "xydvips.tex")
                          if tmp94 then
                            xypic_driver = {tag = "SOME", payload = _L[338]}
                          else
                            xypic_driver = {tag = "SOME", payload = _L[339]}
                          end
                        end
                      end
                    end
                    ::cont19::
                    do
                      if _ENV.CLUTTEX_VERBOSITY >= 1 then
                      else
                        goto cont20
                      end
                      do
                        do
                          local tmp92
                          if graphics_driver.tag == "NONE" then
                            tmp92 = "not loaded"
                          else
                            if graphics_driver.tag == "SOME" then
                            else
                              _raise(_Match, "check-driver.sml:204:17")
                            end
                            tmp92 = _L[318](graphics_driver.payload)
                          end
                          _L[304]("checkdriver: graphics=" .. tmp92)
                          local tmp93
                          if expl3_driver.tag == "NONE" then
                            tmp93 = "not loaded"
                          else
                            if expl3_driver.tag == "SOME" then
                            else
                              _raise(_Match, "check-driver.sml:210:17")
                            end
                            tmp93 = _L[329](expl3_driver.payload)
                          end
                          _L[304]("checkdriver: expl3=" .. tmp93)
                          local tmp94
                          if hyperref_driver.tag == "NONE" then
                            tmp94 = "not loaded"
                          else
                            if hyperref_driver.tag == "SOME" then
                            else
                              _raise(_Match, "check-driver.sml:216:17")
                            end
                            tmp94 = _L[336](hyperref_driver.payload)
                          end
                          _L[304]("checkdriver: hyperref=" .. tmp94)
                          if xypic_driver.tag == "NONE" then
                          else
                            goto else2
                          end
                          _L[304]("checkdriver: xypic=not loaded")
                          goto cont20
                        end
                        ::else2::
                        if xypic_driver.tag == "SOME" then
                        else
                          _raise(_Match, "check-driver.sml:222:17")
                        end
                        do
                          local d = xypic_driver.payload
                          if d == "PDF" then
                          else
                            goto else3
                          end
                          _L[304]("checkdriver: xypic=pdf")
                          goto cont20
                          ::else3::
                          if d == "DVIPS" then
                          else
                            goto else4
                          end
                          _L[304]("checkdriver: xypic=dvips")
                          goto cont20
                          ::else4::
                          if d == "UNKNOWN" then
                          else
                            _raise(_Match, "check-driver.sml:67:9")
                          end
                          _L[304]("checkdriver: xypic=unknown")
                        end
                      end
                    end
                    ::cont20::
                    local expected_xypic, expected_hyperref, expected_expl3_old, expected_expl3_new
                    do
                      local expected_graphics
                      if driver == "DVIPS" then
                        local tmp92 = {tag = "SOME", payload = _L[331]}
                        expected_xypic = {tag = "SOME", payload = _L[338]}
                        expected_hyperref = tmp92
                        expected_graphics = _L[312]
                        expected_expl3_old = _L[323]
                        expected_expl3_new = _L[323]
                      elseif driver == "DVIPDFMX" then
                        local tmp92 = {tag = "SOME", payload = _L[330]}
                        expected_xypic = {tag = "SOME", payload = _L[337]}
                        expected_hyperref = tmp92
                        expected_graphics = _L[311]
                        expected_expl3_old = _L[322]
                        expected_expl3_new = _L[322]
                      elseif driver == "DVISVGM" then
                        expected_xypic = NONE
                        expected_hyperref = NONE
                        expected_graphics = _L[313]
                        expected_expl3_old = _L[320]
                        expected_expl3_new = _L[320]
                      elseif driver == "XETEX" then
                        local tmp92 = {tag = "SOME", payload = _L[334]}
                        expected_xypic = {tag = "SOME", payload = _L[337]}
                        expected_hyperref = tmp92
                        expected_graphics = _L[315]
                        expected_expl3_old = _L[321]
                        expected_expl3_new = _L[326]
                      elseif driver == "PDFTEX" then
                        local tmp92 = {tag = "SOME", payload = _L[332]}
                        expected_xypic = {tag = "SOME", payload = _L[337]}
                        expected_hyperref = tmp92
                        expected_graphics = _L[314]
                        expected_expl3_old = _L[319]
                        expected_expl3_new = _L[324]
                      elseif driver == "LUATEX" then
                        local tmp92 = {tag = "SOME", payload = _L[333]}
                        expected_xypic = {tag = "SOME", payload = _L[337]}
                        expected_hyperref = tmp92
                        expected_graphics = _L[316]
                        expected_expl3_old = _L[319]
                        expected_expl3_new = _L[325]
                      else
                        _raise(_Match, "check-driver.sml:72:7")
                      end
                      do
                        if graphics_driver.tag == "NONE" then
                          goto cont21
                        end
                        if graphics_driver.tag == "SOME" then
                        else
                          _raise(_Match, "check-driver.sml:236:7")
                        end
                        do
                          local d = graphics_driver.payload
                          local tmp92
                          tmp92 = not (d == "DVIPDFMX" and expected_graphics == "DVIPDFMX") and (not (d == "DVIPS" and expected_graphics == "DVIPS") and (not (d == "DVISVGM" and expected_graphics == "DVISVGM") and (not (d == "PDFTEX" and expected_graphics == "PDFTEX") and (not (d == "XETEX" and expected_graphics == "XETEX") and (not (d == "LUATEX" and expected_graphics == "LUATEX") and (d ~= "UNKNOWN" or expected_graphics ~= "UNKNOWN"))))))
                          if tmp92 then
                          else
                            goto cont21
                          end
                          do
                            _L[303]("The driver option for grahipcs(x)/color is missing or wrong.")
                            local tmp93 = _L[318](expected_graphics)
                            _L[303]("Consider setting '" .. tmp93 .. "' option.")
                          end
                        end
                      end
                    end
                    ::cont21::
                    do
                      if expl3_driver.tag == "NONE" then
                        goto cont22
                      end
                      if expl3_driver.tag == "SOME" then
                      else
                        _raise(_Match, "check-driver.sml:248:7")
                      end
                      do
                        local tmp92
                        do
                          local d = expl3_driver.payload
                          local tmp93
                          do
                            local tmp94 = _L[328](d, expected_expl3_old)
                            tmp93 = not tmp94
                          end
                          do
                            if tmp93 then
                            else
                              tmp92 = false
                              goto cont24
                            end
                            do
                              local tmp94 = _L[328](d, expected_expl3_new)
                              tmp92 = not tmp94
                            end
                          end
                        end
                        ::cont24::
                        if tmp92 then
                        else
                          goto cont22
                        end
                        do
                          _L[303]("The driver option for expl3 is missing or wrong.")
                          local tmp93 = _L[329](expected_expl3_new)
                          _L[303]("Consider setting 'driver=" .. tmp93 .. "' option when loading expl3.")
                          local tmp94 = _L[328](expected_expl3_old, expected_expl3_new)
                          if not tmp94 then
                          else
                            goto cont22
                          end
                          do
                            local tmp95 = _L[329](expected_expl3_old)
                            _L[303]("You might need to instead set 'driver=" .. tmp95 .. "' if you are using an older version of expl3.")
                          end
                        end
                      end
                    end
                    ::cont22::
                    do
                      local tmp92 = hyperref_driver.tag == "SOME" and expected_hyperref.tag == "SOME"
                      do
                        if tmp92 then
                        else
                          goto cont23
                        end
                        do
                          local actual = hyperref_driver.payload
                          local expected = expected_hyperref.payload
                          local tmp93
                          tmp93 = not (actual == "DVIPDFMX" and expected == "DVIPDFMX") and (not (actual == "DVIPS" and expected == "DVIPS") and (not (actual == "PDFTEX" and expected == "PDFTEX") and (not (actual == "LUATEX" and expected == "LUATEX") and (not (actual == "XETEX" and expected == "XETEX") and (actual ~= "UNKNOWN" or expected ~= "UNKNOWN")))))
                          if tmp93 then
                          else
                            goto cont23
                          end
                          do
                            _L[303]("The driver option for hyperref is missing or wrong.")
                            local tmp94 = _L[336](expected)
                            _L[303]("Consider setting '" .. tmp94 .. "' option.")
                          end
                        end
                      end
                    end
                    ::cont23::
                    if xypic_driver.tag == "SOME" and expected_xypic.tag == "SOME" then
                    else
                      goto cont9
                    end
                    do
                      local actual = xypic_driver.payload
                      local expected = expected_xypic.payload
                      local tmp92
                      tmp92 = not (actual == "PDF" and expected == "PDF") and (not (actual == "DVIPS" and expected == "DVIPS") and (actual ~= "UNKNOWN" or expected ~= "UNKNOWN"))
                      if tmp92 then
                      else
                        goto cont9
                      end
                      do
                        _L[303]("The driver option for Xy-pic is missing or wrong.")
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
                              _L[303]("Consider setting 'dvips' option.")
                              goto cont13
                            end
                            _L[303]("Consider setting 'pdf' package option or running \\xyoption{pdf}.")
                            goto cont13
                          end
                          _L[303]("Consider setting 'pdftex' option or running \\xyoption{pdf}.")
                          goto cont13
                        end
                        _L[303]("Consider setting 'dvipdfmx' option or running \\xyoption{pdf}.")
                      end
                    end
                  end
                end
                ::cont13::
              end
            end
          end
          ::cont9::
          local filelist2
          do
            if tmp59.tag == "NONE" then
            else
              goto else2
            end
            if not tmp20(execlog, "No file [^\n]+%.ind%.") then
              filelist2 = filelist1
              goto cont10
            end
            _L[303]("You may want to use --makeindex option.")
            filelist2 = filelist1
            goto cont10
            ::else2::
            if tmp59.tag == "SOME" then
            else
              _raise(_Match, "main.sml:354:33")
            end
            do
              local makeindex = tmp59.payload
              local tmp89 = foldl(function(a)
                local file = a[1]
                local filelist_acc = a[2]
                local tmp90 = _L[214](file.path)
                if tmp90 == "idx" then
                else
                  return filelist_acc
                end
                do
                  local tmp91
                  do
                    local idxfileinfo = {abspath = file.abspath, kind = _L[357], path = file.path}
                    tmp91 = _L[210].replaceext(file.abspath, "ind")
                    local tmp92 = _L[361]({idxfileinfo, nil}, auxstatus1)
                    local tmp93
                    if tmp92[1] then
                      tmp93 = true
                    else
                      tmp93 = _L[362](auxstatus1, tmp91, file.abspath)
                    end
                    if tmp93 then
                    else
                      goto else1
                    end
                    do
                      local idx_dir = _L[212](file.abspath)
                      local tmp94 = _L[208](idx_dir)
                      local tmp95 = _L[211](tmp91)
                      local tmp96 = _L[211](file.abspath)
                      _L[526](table_concat({n = 7, "cd", tmp94, "&&", makeindex, "-o", tmp95, tmp96}, " "), NONE)
                      return {{abspath = tmp91, kind = _L[357], path = tmp91}, filelist_acc}
                    end
                  end
                  ::else1::
                  do
                    local status, exn = _handle(function()
                      local tmp92, tmp93 = _L[216].touch(tmp91)
                      if not tmp92 then
                      else
                        return nil
                      end
                      do
                        local tmp94 = _Error(tmp93)
                        _raise(tmp94, "fs-util.sml:28:25")
                      end
                    end)
                    if not status then
                      if __exn_instanceof(exn, _Error_tag) then
                      else
                        _raise(exn, nil)
                      end
                      do
                        _L[302]("Failed to touch " .. tmp91 .. " (" .. exn.payload .. ")")
                        return filelist_acc
                      end
                    end
                  end
                  return filelist_acc
                end
              end)
              local tmp90 = tmp89(filelist1)
              filelist2 = tmp90(filelist1)
            end
          end
          ::cont10::
          local filelist3
          do
            if exp4.tag == "NONE" then
            else
              goto else2
            end
            if not tmp20(execlog, "No file [^\n]+%.gls%.") then
              filelist3 = filelist2
              goto cont11
            end
            _L[303]("You may want to use --makeglossaries option.")
            filelist3 = filelist2
            goto cont11
            ::else2::
            if exp4.tag == "SOME" then
            else
              _raise(_Match, "main.sml:390:33")
            end
            do
              local makeglossaries = exp4.payload
              local tmp89 = foldl(function(a)
                local file = a[1]
                local filelist_acc = a[2]
                local tmp90 = _L[214](file.path)
                if tmp90 == "glo" then
                else
                  return filelist_acc
                end
                do
                  local tmp91
                  do
                    local glofileinfo = {abspath = file.abspath, kind = _L[357], path = file.path}
                    tmp91 = _L[210].replaceext(file.abspath, "gls")
                    local tmp92 = _L[361]({glofileinfo, nil}, auxstatus1)
                    local tmp93
                    if tmp92[1] then
                      tmp93 = true
                    else
                      tmp93 = _L[362](auxstatus1, tmp91, file.abspath)
                    end
                    if tmp93 then
                    else
                      goto else1
                    end
                    do
                      local tmp94 = _L[208](tmp62)
                      local tmp95 = _L[211](file.path)
                      local tmp96 = _L[213](tmp95)
                      _L[526](table_concat({n = 4, makeglossaries, "-d", tmp94, tmp96}, " "), NONE)
                      return {{abspath = tmp91, kind = _L[357], path = tmp91}, filelist_acc}
                    end
                  end
                  ::else1::
                  do
                    local status, exn = _handle(function()
                      local tmp92, tmp93 = _L[216].touch(tmp91)
                      if not tmp92 then
                      else
                        return nil
                      end
                      do
                        local tmp94 = _Error(tmp93)
                        _raise(tmp94, "fs-util.sml:28:25")
                      end
                    end)
                    if not status then
                      if __exn_instanceof(exn, _Error_tag) then
                      else
                        _raise(exn, nil)
                      end
                      do
                        _L[302]("Failed to touch " .. tmp91 .. " (" .. exn.payload .. ")")
                        return filelist_acc
                      end
                    end
                  end
                  return filelist_acc
                end
              end)
              local tmp90 = tmp89(filelist2)
              filelist3 = tmp90(filelist2)
            end
          end
          ::cont11::
          local filelist4
          do
            if exp.tag == "NONE" then
            else
              goto else2
            end
            if not tmp20(execlog, "No file [^\n]+%.bbl%.") then
              filelist4 = filelist3
              goto cont12
            end
            _L[303]("You may want to use --bibtex or biber option.")
            filelist4 = filelist3
            goto cont12
            ::else2::
            if exp.tag == "SOME" and exp.payload.tag == "BIBTEX" then
            else
              goto else3
            end
            do
              local bibtex, bibtex_aux_hash2
              do
                bibtex = exp.payload.payload
                local biblines2
                do
                  local tmp89 = _L[353](tmp87, tmp62, nil)
                  biblines2 = revAppend(tmp89, nil)
                end
                local tmp89 = biblines2 == nil
                do
                  if tmp89 then
                    bibtex_aux_hash2 = NONE
                    goto cont13
                  end
                  local tmp90 = _L[191](table_concat(_VectorOrArray_fromList(biblines2), "\n"))
                  bibtex_aux_hash2 = {tag = "SOME", payload = tmp90}
                end
              end
              ::cont13::
              local tmp89, tmp90
              do
                local tmp91 = tmp58 .. "." .. "bbl"
                tmp89 = _L[210].join(tmp62, tmp91)
                local tmp92 = tmp(function(a)
                  local x = a[1]
                  local y = a[2]
                  return x[4] == y[4] and (x[3] == y[3] and (x[2] == y[2] and x[1] == y[1]))
                end)
                local tmp93
                do
                  local tmp94 = tmp92({bibtex_aux_hash, bibtex_aux_hash2})
                  tmp93 = not tmp94
                end
                do
                  if tmp93 then
                    tmp90 = true
                    goto cont14
                  end
                  local tmp94 = _L[215](NONE, tmp87)
                  tmp90 = _L[362](auxstatus1, tmp89, tmp94)
                end
              end
              ::cont14::
              if tmp90 then
              else
                goto else4
              end
              do
                local tmp91 = _L[208](tmp62)
                local tmp92 = _L[211](tmp87)
                _L[526](table_concat({n = 5, "cd", tmp91, "&&", bibtex, tmp92}, " "), NONE)
                filelist4 = filelist3
                goto cont12
              end
              ::else4::
              do
                if _ENV.CLUTTEX_VERBOSITY >= 1 then
                else
                  goto cont15
                end
                _L[304]("No need to run BibTeX.")
              end
              ::cont15::
              do
                local status, exn = _handle(function()
                  local tmp91, tmp92 = _L[216].touch(tmp89)
                  if not tmp91 then
                  else
                    return nil
                  end
                  do
                    local tmp93 = _Error(tmp92)
                    _raise(tmp93, "fs-util.sml:28:25")
                  end
                end)
                if not status then
                  if __exn_instanceof(exn, _Error_tag) then
                  else
                    _raise(exn, nil)
                  end
                  do
                    local err = exn.payload
                    _L[302]("Failed to touch " .. tmp89 .. " (" .. err .. ")")
                    filelist4 = filelist3
                    goto cont12
                  end
                end
              end
              filelist4 = filelist3
              goto cont12
            end
            ::else3::
            if exp.tag == "SOME" and exp.payload.tag == "BIBER" then
            else
              _raise(_Match, "main.sml:424:33")
            end
            do
              local biber = exp.payload.payload
              local tmp89 = foldl(function(a)
                local file = a[1]
                local filelist_acc = a[2]
                local tmp90 = _L[214](file.path)
                if tmp90 == "bcf" then
                else
                  return filelist_acc
                end
                do
                  local bcffileinfo = {abspath = file.abspath, kind = _L[357], path = file.path}
                  local tmp91 = _L[210].replaceext(file.abspath, "bbl")
                  local updated_dot_bib
                  do
                    local ins = _L[115](file.abspath)
                    local tmp92 = false
                    ::cont2::
                    do
                      local a1 = tmp92
                      local exp7 = _L[114](ins)
                      if exp7.tag == "NONE" then
                      else
                        goto else2
                      end
                      _L[120](ins)
                      updated_dot_bib = a1
                      goto cont
                      ::else2::
                      if exp7.tag == "SOME" then
                      else
                        _raise(_Match, "main.sml:470:65")
                      end
                      do
                        local tmp93
                        do
                          local tmp94 = tmp22(exp7.payload, "<bcf:datasource .*>(.*)</bcf:datasource>")
                          if not tmp94 then
                            tmp92 = a1
                            goto cont2
                          end
                          tmp93 = _L[210].join(original_wd, tmp94)
                          local tmp95 = _L[218](tmp93)
                          if tmp95 then
                          else
                            goto else3
                          end
                          do
                            local tmp96 = _L[215](NONE, tmp87)
                            local tmp97 = _L[362](auxstatus1, tmp93, tmp96)
                            local tmp98 = not tmp97
                            if tmp98 then
                            else
                              tmp92 = a1 or tmp98
                              goto cont2
                            end
                            _L[304](tmp93 .. " is newer than aux")
                            tmp92 = a1 or tmp98
                            goto cont2
                          end
                        end
                        ::else3::
                        _L[302](tmp93 .. " is not accessible")
                        tmp92 = a1
                        goto cont2
                      end
                    end
                  end
                  ::cont::
                  local tmp92
                  do
                    if updated_dot_bib then
                      tmp92 = true
                      goto cont1
                    end
                    local tmp93 = _L[361]({bcffileinfo, nil}, auxstatus1)
                    if tmp93[1] then
                      tmp92 = true
                    else
                      tmp92 = _L[362](auxstatus1, tmp91, file.abspath)
                    end
                  end
                  ::cont1::
                  if tmp92 then
                  else
                    goto else1
                  end
                  do
                    local tmp93 = _L[208](tmp62)
                    local tmp94 = _L[211](file.abspath)
                    _L[526](table_concat({n = 4, biber, "--output-directory", tmp93, tmp94}, " "), NONE)
                    return {{abspath = tmp91, kind = _L[357], path = tmp91}, filelist3}
                  end
                  ::else1::
                  do
                    local status, exn = _handle(function()
                      local tmp93, tmp94 = _L[216].touch(tmp91)
                      if not tmp93 then
                      else
                        return nil
                      end
                      do
                        local tmp95 = _Error(tmp94)
                        _raise(tmp95, "fs-util.sml:28:25")
                      end
                    end)
                    if not status then
                      if __exn_instanceof(exn, _Error_tag) then
                      else
                        _raise(exn, nil)
                      end
                      do
                        _L[302]("Failed to touch " .. tmp91 .. " (" .. exn.payload .. ")")
                        return filelist_acc
                      end
                    end
                  end
                  return filelist_acc
                end
              end)
              local tmp90 = tmp89(filelist3)
              filelist4 = tmp90(filelist3)
            end
          end
          ::cont12::
          local tmp89 = _L[49]("No pages of output.")
          local tmp90 = tmp89(execlog)
          if tmp90 then
            exp6 = _L[528]
            goto cont2
          end
          local exp7 = _L[361](filelist4, auxstatus1)
          local should_rerun = exp7[1]
          local auxstatus2 = exp7[2]
          local tmp91
          tmp91 = should_rerun or lightweight_mode
          if tmp91 then
            exp6 = {tag = "SHOULD_RERUN", payload = auxstatus2}
          else
            exp6 = _L[527]
          end
        end
      end
      ::cont2::
      if exp6.tag == "NO_PAGES_OF_OUTPUT" then
      else
        goto else1
      end
      _L[302]("No pages of output.")
      tmp83 = false
      goto cont
      ::else1::
      if exp6.tag == "NO_NEED_TO_RERUN" then
        tmp83 = true
        goto cont
      end
      if exp6.tag == "SHOULD_RERUN" then
      else
        _raise(_Match, "main.sml:533:20")
      end
      do
        local auxstatus = exp6.payload
        if tmp86 >= tmp60 then
        else
          tmp84 = tmp86
          tmp85 = auxstatus
          goto cont1
        end
        _L[302]("LaTeX should be run once more.")
        tmp83 = true
      end
    end
  end
  ::cont::
  if tmp83 then
  else
    return nil
  end
  do
    do
      local tmp84 = _L[232](tmp63, _L[231])
      local tmp85
      tmp85 = tmp84 or tmp56
      do
        if tmp85 then
        else
          goto else1
        end
        do
          local tmp86 = tmp58 .. "." .. output_extension
          local tmp87 = _L[210].join(tmp62, tmp86)
          local onCopyError
          if _L[220] then
            onCopyError = {tag = "SOME", payload = function(a)
              local output_format
              if tmp63 == "DVI" then
                output_format = "DVI"
              elseif tmp63 == "PDF" then
                output_format = "PDF"
              else
                _raise(_Match, "main.sml:551:85")
              end
              _L[301]("Failed to copy file.  Some applications may be locking the " .. output_format .. " file.")
              return false
            end}
          else
            onCopyError = NONE
          end
          _L[526](_L[217].copy_command(tmp87, tmp61), onCopyError)
          if tmp57 == nil then
            goto cont1
          end
          _L[302]("--dvipdfmx-option[s] are ignored.")
          goto cont1
        end
        ::else1::
        local tmp86 = tmp58 .. "." .. "dvi"
        local tmp87 = _L[210].join(tmp62, tmp86)
        local tmp88 = _L[208](tmp61)
        local tmp89 = _L[208](tmp87)
        local tmp90 = {tmp89, nil}
        local tmp91
        do
          local tmp92 = revAppend(tmp57, nil)
          tmp91 = revAppend(tmp92, tmp90)
        end
        _L[526](table_concat(_VectorOrArray_fromList({"dvipdfmx", {"-o", {tmp88, tmp91}}}), " "), NONE)
      end
    end
    ::cont1::
    do
      local tmp84 = _L[232](tmp63, _L[230])
      do
        if tmp84 then
        else
          goto cont2
        end
        do
          local tmp85 = getOpt(tmp69, "0")
          local tmp86 = tmp4(tmp85)
          local synctex_ext
          if tmp86 > 0 then
            synctex_ext = {tag = "SOME", payload = "synctex.gz"}
          elseif tmp86 < 0 then
            synctex_ext = {tag = "SOME", payload = "synctex"}
          else
            synctex_ext = NONE
          end
          if synctex_ext.tag == "SOME" then
          elseif synctex_ext.tag == "NONE" then
            goto cont2
          else
            _raise(_Match, "main.sml:583:23")
          end
          do
            local ext = synctex_ext.payload
            local tmp87 = tmp58 .. "." .. ext
            local tmp88 = _L[210].join(tmp62, tmp87)
            local tmp89 = _L[210].replaceext(tmp61, ext)
            _L[526](_L[217].copy_command(tmp88, tmp89), NONE)
          end
        end
      end
    end
    ::cont2::
    do
      if exp3.tag == "SOME" then
      elseif exp3.tag == "NONE" then
        goto cont3
      else
        _raise(_Match, "main.sml:590:16")
      end
      do
        local make_depends, recorded, tmp84
        do
          make_depends = exp3.payload
          recorded = _L[360](recorderfile, tmp59, nil, _L[203])
          local tmp85 = _L[237](engine_type, _L[235])
          do
            if tmp85 then
            else
              tmp84 = false
              goto cont4
            end
            tmp84 = _L[218](recorderfile2)
          end
        end
        ::cont4::
        local recorded1
        do
          if tmp84 then
          else
            recorded1 = recorded
            goto cont5
          end
          do
            local fileList = recorded[1]
            recorded1 = _L[360](recorderfile2, tmp59, fileList, recorded[2])
          end
        end
        ::cont5::
        local filelist, outs
        do
          local exp6
          do
            local fileInfo = recorded1[1]
            exp6 = _L[359](fileInfo, recorded1[2])
          end
          filelist = exp6[1]
          do
            local tmp85, tmp86 = tmp7(make_depends, "w")
            if tmp85 == nil then
            else
              local tmp87 = _L[113].mkOutstream
              outs = tmp87({tag = "LUA_WRITABLE", payload = {buffer_mode = {_L[16]}, name = make_depends, writable = tmp85}})
              goto cont6
            end
            do
              local tmp87 = _Fail(tmp86)
              _raise({tag = _L[9], payload = {cause = tmp87, ["function"] = "TextIO.openOut", name = make_depends}}, "text-io.sml:493:27")
            end
          end
        end
        ::cont6::
        _L[124]({outs, tmp61 .. ":"})
        local tmp85 = app1(function(a)
          if a.kind == "INPUT" then
          else
            return nil
          end
          do
            return _L[124]({outs, " " .. a.path})
          end
        end)
        tmp85(filelist)
        _L[124]({outs, "\n"})
        _L[121](outs)
      end
    end
    ::cont3::
    if _ENV.CLUTTEX_VERBOSITY >= 1 then
    else
      return nil
    end
    do
      return _L[304]("Command exited successfully")
    end
  end
end
_L[530] = function(a, a1)
  local tmp54 = assert(a.new())
  local tmp55 = app1(function(file)
    assert(tmp54:add(file))
    return nil
  end)
  tmp55(a1)
  local tmp56 = assert(tmp54:next())
  if _ENV.CLUTTEX_VERBOSITY >= 2 then
  else
    tmp54:close()
    return true
  end
  do
    local tmp57 = tmp56.action .. " "
    _L[304](tmp57 .. tmp56.path)
    tmp54:close()
    return true
  end
end
_L[531] = function(a)
  local tmp54
  do
    local tmp55 = map(_L[208])
    local tmp56 = tmp55(a)
    tmp54 = table_concat(_VectorOrArray_fromList({"fswatch", {"--one-event", {"--event=Updated", {"--", tmp56}}}}), " ")
    do
      if _ENV.CLUTTEX_VERBOSITY >= 1 then
      else
        goto cont
      end
      _L[300](tmp54)
    end
  end
  ::cont::
  local tmp55 = assert(tmp8(tmp54, "r"))
  local tmp56 = tmp55:lines()
  ::cont1::
  do
    local tmp57 = tmp56()
    if not tmp57 then
      tmp55:close()
      return false
    end
    local tmp58 = exists(function(path)
      return tmp57 == path
    end)
    local tmp59 = tmp58(a)
    if tmp59 then
      tmp55:close()
      return true
    else
      goto cont1
    end
  end
end
_L[532] = function(a)
  local tmp54
  do
    local tmp55 = map(_L[208])
    local tmp56 = tmp55(a)
    tmp54 = table_concat(_VectorOrArray_fromList({"inotifywait", {"--event=modify", {"--event=attrib", {"--format=%w", {"--quiet", tmp56}}}}}), " ")
    do
      if _ENV.CLUTTEX_VERBOSITY >= 1 then
      else
        goto cont
      end
      _L[300](tmp54)
    end
  end
  ::cont::
  local tmp55 = assert(tmp8(tmp54, "r"))
  local tmp56 = tmp55:lines()
  ::cont1::
  do
    local tmp57 = tmp56()
    if not tmp57 then
      tmp55:close()
      return false
    end
    local tmp58 = exists(function(path)
      return tmp57 == path
    end)
    local tmp59 = tmp58(a)
    if tmp59 then
      tmp55:close()
      return true
    else
      goto cont1
    end
  end
end
do
  do
    _L[534] = #_L[154]
    _L[533] = tabulate(_L[534], function(i)
      return _L[154][_Int_add(i, 1)]
    end)
  end
  do
    _L[536], _L[537] = _L[349], _L[533]
    ::cont18::
    do
      local _L1 = {}
      _L1[1], _L1[2] = _L[536], _L[537]
      do
        if _L1[2] == nil then
          _L1[3] = NONE
          goto cont19
        end
        _L1[4] = _L[525]
        ::cont20::
        do
          local _L2 = {}
          _L2[1] = _L1[4]
          if _L2[1] == nil then
            _L1[3] = NONE
            goto cont19
          end
          if _L2[1] ~= nil then
          else
            _raise(_Match, "handle-options.sml:63:41")
          end
          do
            _L2[2] = _L2[1][1]
            _L2[3] = _L2[1][2]
            do
              _L2[5] = _L2[2][1]
              _L2[6] = _L2[2][2]
              if _L1[2] == nil then
                _L2[4] = NONE
                goto cont21
              end
              _L2[7] = _L2[5].tag == "SHORT" and _L2[6].tag == "SIMPLE"
              _L2[8] = _L2[7] and _L1[2] ~= nil
              if _L2[8] then
                _L2[9] = _L2[5].payload
                _L2[10] = _L2[6].payload
                _L2[11] = _L1[2][1]
                _L2[12] = _L1[2][2]
                if _L2[11] == _L2[9] then
                  _L2[4] = {tag = "SOME", payload = {_L2[10], _L2[12]}}
                  goto cont21
                else
                  _L2[4] = NONE
                  goto cont21
                end
              end
              _L2[13] = _L2[5].tag == "SHORT" and _L2[6].tag == "WITH_ARG"
              _L2[14] = _L2[13] and _L1[2] ~= nil
              if _L2[14] then
              else
                goto else33
              end
              do
                _L2[15] = _L2[5].payload
                _L2[16] = _L2[6].payload
                _L2[17] = _L1[2][1]
                _L2[18] = _L1[2][2]
                if _L2[17] == _L2[15] then
                else
                  goto else36
                end
                do
                  if _L2[18] == nil then
                  else
                    goto else37
                  end
                  do
                    _L2[19] = _Fail("argument missing after " .. _L2[15])
                    _raise(_L2[19], "handle-options.sml:21:21")
                  end
                  ::else37::
                  if _L2[18] ~= nil then
                  else
                    _raise(_Match, "handle-options.sml:20:11")
                  end
                  do
                    _L2[20] = _L2[18][1]
                    _L2[21] = _L2[18][2]
                    _L2[22] = _L2[16](_L2[20])
                    _L2[4] = {tag = "SOME", payload = {_L2[22], _L2[21]}}
                    goto cont21
                  end
                end
                ::else36::
                _L2[23] = isPrefix(_L2[15])
                _L2[24] = _L2[23](_L2[17])
                if _L2[24] then
                else
                  _L2[4] = NONE
                  goto cont21
                end
                do
                  _L2[25] = extract(_L2[17], #_L2[15], NONE)
                  _L2[26] = _L2[16](_L2[25])
                  _L2[4] = {tag = "SOME", payload = {_L2[26], _L2[18]}}
                  goto cont21
                end
              end
              ::else33::
              _L2[27] = _L2[5].tag == "SHORT" and _L2[6].tag == "WITH_OPTIONAL_ARG"
              _L2[28] = _L2[27] and _L1[2] ~= nil
              if _L2[28] then
              else
                goto else34
              end
              do
                _L2[29] = _L2[5].payload
                _L2[30] = _L2[6].payload.default
                _L2[31] = _L2[6].payload.action
                _L2[32] = _L1[2][1]
                _L2[33] = _L1[2][2]
                if _L2[32] == _L2[29] then
                else
                  goto else36
                end
                do
                  _L2[34] = _L2[31](_L2[30])
                  _L2[4] = {tag = "SOME", payload = {_L2[34], _L2[33]}}
                  goto cont21
                end
                ::else36::
                _L2[35] = isPrefix(_L2[29])
                _L2[36] = _L2[35](_L2[32])
                if _L2[36] then
                else
                  _L2[4] = NONE
                  goto cont21
                end
                do
                  _L2[37] = extract(_L2[32], #_L2[29], NONE)
                  _L2[38] = _L2[31](_L2[37])
                  _L2[4] = {tag = "SOME", payload = {_L2[38], _L2[33]}}
                  goto cont21
                end
              end
              ::else34::
              _L2[39] = _L2[5].tag == "LONG" and _L2[6].tag == "SIMPLE"
              _L2[40] = _L2[39] and _L1[2] ~= nil
              if _L2[40] then
                _L2[41] = _L2[5].payload
                _L2[42] = _L2[6].payload
                _L2[43] = _L1[2][1]
                _L2[44] = _L1[2][2]
                if _L2[43] == _L2[41] then
                  _L2[4] = {tag = "SOME", payload = {_L2[42], _L2[44]}}
                  goto cont21
                else
                  _L2[4] = NONE
                  goto cont21
                end
              end
              _L2[45] = _L2[5].tag == "LONG" and _L2[6].tag == "WITH_ARG"
              _L2[46] = _L2[45] and _L1[2] ~= nil
              if _L2[46] then
              else
                goto else35
              end
              do
                _L2[47] = _L2[5].payload
                _L2[48] = _L2[6].payload
                _L2[49] = _L1[2][1]
                _L2[50] = _L1[2][2]
                if _L2[49] == _L2[47] then
                else
                  goto else36
                end
                do
                  if _L2[50] == nil then
                  else
                    goto else37
                  end
                  do
                    _L2[51] = _Fail("argument missing after " .. _L2[47])
                    _raise(_L2[51], "handle-options.sml:45:21")
                  end
                  ::else37::
                  if _L2[50] ~= nil then
                  else
                    _raise(_Match, "handle-options.sml:44:11")
                  end
                  do
                    _L2[52] = _L2[50][1]
                    _L2[53] = _L2[50][2]
                    _L2[54] = _L2[48](_L2[52])
                    _L2[4] = {tag = "SOME", payload = {_L2[54], _L2[53]}}
                    goto cont21
                  end
                end
                ::else36::
                _L2[55] = isPrefix(_L2[47] .. "=")
                _L2[56] = _L2[55](_L2[49])
                if _L2[56] then
                else
                  _L2[4] = NONE
                  goto cont21
                end
                do
                  _L2[57] = extract(_L2[49], _Int_add(#_L2[47], 1), NONE)
                  _L2[58] = _L2[48](_L2[57])
                  _L2[4] = {tag = "SOME", payload = {_L2[58], _L2[50]}}
                  goto cont21
                end
              end
              ::else35::
              _L2[59] = _L2[5].tag == "LONG" and _L2[6].tag == "WITH_OPTIONAL_ARG"
              _L2[60] = _L2[59] and _L1[2] ~= nil
              if _L2[60] then
              else
                _raise(_Match, "handle-options.sml:13:5")
              end
              do
                _L2[61] = _L2[5].payload
                _L2[62] = _L2[6].payload.default
                _L2[63] = _L2[6].payload.action
                _L2[64] = _L1[2][1]
                _L2[65] = _L1[2][2]
                if _L2[64] == _L2[61] then
                else
                  goto else36
                end
                do
                  _L2[66] = _L2[63](_L2[62])
                  _L2[4] = {tag = "SOME", payload = {_L2[66], _L2[65]}}
                  goto cont21
                end
                ::else36::
                _L2[67] = isPrefix(_L2[61] .. "=")
                _L2[68] = _L2[67](_L2[64])
                if _L2[68] then
                else
                  _L2[4] = NONE
                  goto cont21
                end
                do
                  _L2[69] = extract(_L2[64], _Int_add(#_L2[61], 1), NONE)
                  _L2[70] = _L2[63](_L2[69])
                  _L2[4] = {tag = "SOME", payload = {_L2[70], _L2[65]}}
                end
              end
            end
            ::cont21::
            if _L2[4].tag == "SOME" then
              _L1[3] = {tag = "SOME", payload = _L2[4].payload}
            elseif _L2[4].tag == "NONE" then
              _L1[4] = _L2[3]
              goto cont20
            else
              _raise(_Match, "handle-options.sml:64:62")
            end
          end
        end
      end
      ::cont19::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_ENGINE" then
      else
        goto else2
      end
      do
        _L1[5] = _L1[3].payload[1].payload
        _L1[6] = _L1[3].payload[2]
        _L1[7] = _L1[1].engine
        if _L1[7].tag == "NONE" then
          _L1[8] = _L1[1].bibtex_or_biber
          _L1[9] = _L1[1].change_directory
          _L1[10] = _L1[1].check_driver
          _L1[11] = _L1[1].color
          _L1[12] = _L1[1].config_file
          _L1[13] = _L1[1].dvipdfmx_extraoptions
          _L1[14] = _L1[1].engine_executable
          _L1[15] = _L1[1].file_line_error
          _L1[16] = _L1[1].fmt
          _L1[17] = _L1[1].fresh
          _L1[18] = _L1[1].halt_on_error
          _L1[19] = _L1[1].includeonly
          _L1[20] = _L1[1].interaction
          _L1[21] = _L1[1].jobname
          _L1[22] = _L1[1].make_depends
          _L1[23] = _L1[1].makeglossaries
          _L1[24] = _L1[1].makeindex
          _L1[25] = _L1[1].max_iterations
          _L1[26] = _L1[1].output
          _L1[27] = _L1[1].output_directory
          _L1[28] = _L1[1].output_format
          _L1[29] = _L1[1].package_support
          _L1[30] = _L1[1].print_output_directory
          _L1[31] = _L1[1].shell_escape
          _L1[32] = _L1[1].source_date_epoch
          _L1[33] = _L1[1].start_with_draft
          _L1[34] = _L1[1].synctex
          _L1[35] = _L1[1].tex_extraoptions
          _L1[36] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[8], change_directory = _L1[9], check_driver = _L1[10], color = _L1[11], config_file = _L1[12], dvipdfmx_extraoptions = _L1[13], engine = {tag = "SOME", payload = _L1[5]}, engine_executable = _L1[14], file_line_error = _L1[15], fmt = _L1[16], fresh = _L1[17], halt_on_error = _L1[18], includeonly = _L1[19], interaction = _L1[20], jobname = _L1[21], make_depends = _L1[22], makeglossaries = _L1[23], makeindex = _L1[24], max_iterations = _L1[25], output = _L1[26], output_directory = _L1[27], output_format = _L1[28], package_support = _L1[29], print_output_directory = _L1[30], shell_escape = _L1[31], source_date_epoch = _L1[32], start_with_draft = _L1[33], synctex = _L1[34], tex_extraoptions = _L1[35], watch = _L1[36]}
          _L[537] = _L1[6]
          goto cont18
        end
        if _L1[7].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:168:46")
        end
        _L[124]({_L[118], "multiple --engine options\n"})
        tmp15(1, true)
      end
      ::else2::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_ENGINE_EXECUTABLE" then
      else
        goto else3
      end
      do
        _L1[37] = _L1[3].payload[1].payload
        _L1[38] = _L1[3].payload[2]
        _L1[39] = _L1[1].engine_executable
        if _L1[39].tag == "NONE" then
          _L1[40] = _L1[1].bibtex_or_biber
          _L1[41] = _L1[1].change_directory
          _L1[42] = _L1[1].check_driver
          _L1[43] = _L1[1].color
          _L1[44] = _L1[1].config_file
          _L1[45] = _L1[1].dvipdfmx_extraoptions
          _L1[46] = _L1[1].engine
          _L1[47] = _L1[1].file_line_error
          _L1[48] = _L1[1].fmt
          _L1[49] = _L1[1].fresh
          _L1[50] = _L1[1].halt_on_error
          _L1[51] = _L1[1].includeonly
          _L1[52] = _L1[1].interaction
          _L1[53] = _L1[1].jobname
          _L1[54] = _L1[1].make_depends
          _L1[55] = _L1[1].makeglossaries
          _L1[56] = _L1[1].makeindex
          _L1[57] = _L1[1].max_iterations
          _L1[58] = _L1[1].output
          _L1[59] = _L1[1].output_directory
          _L1[60] = _L1[1].output_format
          _L1[61] = _L1[1].package_support
          _L1[62] = _L1[1].print_output_directory
          _L1[63] = _L1[1].shell_escape
          _L1[64] = _L1[1].source_date_epoch
          _L1[65] = _L1[1].start_with_draft
          _L1[66] = _L1[1].synctex
          _L1[67] = _L1[1].tex_extraoptions
          _L1[68] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[40], change_directory = _L1[41], check_driver = _L1[42], color = _L1[43], config_file = _L1[44], dvipdfmx_extraoptions = _L1[45], engine = _L1[46], engine_executable = {tag = "SOME", payload = _L1[37]}, file_line_error = _L1[47], fmt = _L1[48], fresh = _L1[49], halt_on_error = _L1[50], includeonly = _L1[51], interaction = _L1[52], jobname = _L1[53], make_depends = _L1[54], makeglossaries = _L1[55], makeindex = _L1[56], max_iterations = _L1[57], output = _L1[58], output_directory = _L1[59], output_format = _L1[60], package_support = _L1[61], print_output_directory = _L1[62], shell_escape = _L1[63], source_date_epoch = _L1[64], start_with_draft = _L1[65], synctex = _L1[66], tex_extraoptions = _L1[67], watch = _L1[68]}
          _L[537] = _L1[38]
          goto cont18
        end
        if _L1[39].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:172:61")
        end
        _L[124]({_L[118], "multiple --engine-executable options\n"})
        tmp15(1, true)
      end
      ::else3::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_OUTPUT" then
      else
        goto else4
      end
      do
        _L1[69] = _L1[3].payload[1].payload
        _L1[70] = _L1[3].payload[2]
        _L1[71] = _L1[1].output
        if _L1[71].tag == "NONE" then
          _L1[72] = _L1[1].bibtex_or_biber
          _L1[73] = _L1[1].change_directory
          _L1[74] = _L1[1].check_driver
          _L1[75] = _L1[1].color
          _L1[76] = _L1[1].config_file
          _L1[77] = _L1[1].dvipdfmx_extraoptions
          _L1[78] = _L1[1].engine
          _L1[79] = _L1[1].engine_executable
          _L1[80] = _L1[1].file_line_error
          _L1[81] = _L1[1].fmt
          _L1[82] = _L1[1].fresh
          _L1[83] = _L1[1].halt_on_error
          _L1[84] = _L1[1].includeonly
          _L1[85] = _L1[1].interaction
          _L1[86] = _L1[1].jobname
          _L1[87] = _L1[1].make_depends
          _L1[88] = _L1[1].makeglossaries
          _L1[89] = _L1[1].makeindex
          _L1[90] = _L1[1].max_iterations
          _L1[91] = _L1[1].output_directory
          _L1[92] = _L1[1].output_format
          _L1[93] = _L1[1].package_support
          _L1[94] = _L1[1].print_output_directory
          _L1[95] = _L1[1].shell_escape
          _L1[96] = _L1[1].source_date_epoch
          _L1[97] = _L1[1].start_with_draft
          _L1[98] = _L1[1].synctex
          _L1[99] = _L1[1].tex_extraoptions
          _L1[100] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[72], change_directory = _L1[73], check_driver = _L1[74], color = _L1[75], config_file = _L1[76], dvipdfmx_extraoptions = _L1[77], engine = _L1[78], engine_executable = _L1[79], file_line_error = _L1[80], fmt = _L1[81], fresh = _L1[82], halt_on_error = _L1[83], includeonly = _L1[84], interaction = _L1[85], jobname = _L1[86], make_depends = _L1[87], makeglossaries = _L1[88], makeindex = _L1[89], max_iterations = _L1[90], output = {tag = "SOME", payload = _L1[69]}, output_directory = _L1[91], output_format = _L1[92], package_support = _L1[93], print_output_directory = _L1[94], shell_escape = _L1[95], source_date_epoch = _L1[96], start_with_draft = _L1[97], synctex = _L1[98], tex_extraoptions = _L1[99], watch = _L1[100]}
          _L[537] = _L1[70]
          goto cont18
        end
        if _L1[71].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:176:46")
        end
        _L[124]({_L[118], "multiple --output options\n"})
        tmp15(1, true)
      end
      ::else4::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_FRESH" then
      else
        goto else5
      end
      do
        _L1[101] = _L1[3].payload[2]
        _L1[102] = _L1[1].fresh
        if not _L1[102] then
          _L1[103] = _L1[1].bibtex_or_biber
          _L1[104] = _L1[1].change_directory
          _L1[105] = _L1[1].check_driver
          _L1[106] = _L1[1].color
          _L1[107] = _L1[1].config_file
          _L1[108] = _L1[1].dvipdfmx_extraoptions
          _L1[109] = _L1[1].engine
          _L1[110] = _L1[1].engine_executable
          _L1[111] = _L1[1].file_line_error
          _L1[112] = _L1[1].fmt
          _L1[113] = _L1[1].halt_on_error
          _L1[114] = _L1[1].includeonly
          _L1[115] = _L1[1].interaction
          _L1[116] = _L1[1].jobname
          _L1[117] = _L1[1].make_depends
          _L1[118] = _L1[1].makeglossaries
          _L1[119] = _L1[1].makeindex
          _L1[120] = _L1[1].max_iterations
          _L1[121] = _L1[1].output
          _L1[122] = _L1[1].output_directory
          _L1[123] = _L1[1].output_format
          _L1[124] = _L1[1].package_support
          _L1[125] = _L1[1].print_output_directory
          _L1[126] = _L1[1].shell_escape
          _L1[127] = _L1[1].source_date_epoch
          _L1[128] = _L1[1].start_with_draft
          _L1[129] = _L1[1].synctex
          _L1[130] = _L1[1].tex_extraoptions
          _L[536] = {bibtex_or_biber = _L1[103], change_directory = _L1[104], check_driver = _L1[105], color = _L1[106], config_file = _L1[107], dvipdfmx_extraoptions = _L1[108], engine = _L1[109], engine_executable = _L1[110], file_line_error = _L1[111], fmt = _L1[112], fresh = true, halt_on_error = _L1[113], includeonly = _L1[114], interaction = _L1[115], jobname = _L1[116], make_depends = _L1[117], makeglossaries = _L1[118], makeindex = _L1[119], max_iterations = _L1[120], output = _L1[121], output_directory = _L1[122], output_format = _L1[123], package_support = _L1[124], print_output_directory = _L1[125], shell_escape = _L1[126], source_date_epoch = _L1[127], start_with_draft = _L1[128], synctex = _L1[129], tex_extraoptions = _L1[130], watch = _L1[1].watch}
          _L[537] = _L1[101]
          goto cont18
        end
        if _L1[102] then
        else
          _raise(_Match, "handle-options.sml:180:38")
        end
        _L[124]({_L[118], "multiple --fresh options\n"})
        tmp15(1, true)
      end
      ::else5::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_MAX_ITERATIONS" then
      else
        goto else6
      end
      do
        _L1[131] = _L1[3].payload[1].payload
        _L1[132] = _L1[3].payload[2]
        _L1[133] = _L1[1].max_iterations
        if _L1[133].tag == "NONE" then
        else
          goto else33
        end
        do
          do
            _L1[135] = scanString(function(a)
              return function(a1)
                local strm
                do
                  local strm1 = _L[3](a, a1)
                  local exp = _L[7](a, strm1)
                  local isNegative = exp[1]
                  strm = exp[2]
                  if isNegative then
                  else
                    goto else1
                  end
                  do
                    local exp1 = a(strm)
                    if exp1.tag == "SOME" then
                    elseif exp1.tag == "NONE" then
                      return NONE
                    else
                      _raise(_Match, "scan-num.sml:43:25")
                    end
                    do
                      local c = exp1.payload[1]
                      local strm_PRIME = exp1.payload[2]
                      if 48 <= c and c <= 57 then
                      else
                        return NONE
                      end
                      do
                        local tmp54, tmp55
                        do
                          local tmp56 = _L[6](c)
                          local tmp57 = _Int_negate(tmp56)
                          tmp55, tmp54 = tmp57, strm_PRIME
                        end
                        ::cont::
                        do
                          local x, strm2 = tmp55, tmp54
                          local exp2 = a(strm2)
                          if exp2.tag == "SOME" then
                          elseif exp2.tag == "NONE" then
                            return {tag = "SOME", payload = {x, strm2}}
                          else
                            _raise(_Match, "scan-num.sml:37:35")
                          end
                          do
                            local c1 = exp2.payload[1]
                            local strm_PRIME1 = exp2.payload[2]
                            if 48 <= c1 and c1 <= 57 then
                            else
                              return {tag = "SOME", payload = {x, strm2}}
                            end
                            do
                              local tmp56 = _Int_mul(10, x)
                              local tmp57 = _L[6](c1)
                              tmp55 = _Int_sub(tmp56, tmp57)
                              tmp54 = strm_PRIME1
                              goto cont
                            end
                          end
                        end
                      end
                    end
                  end
                end
                ::else1::
                local exp = a(strm)
                if exp.tag == "SOME" then
                elseif exp.tag == "NONE" then
                  return NONE
                else
                  _raise(_Match, "scan-num.sml:29:25")
                end
                do
                  local c = exp.payload[1]
                  local strm_PRIME = exp.payload[2]
                  if 48 <= c and c <= 57 then
                  else
                    return NONE
                  end
                  do
                    local tmp54, tmp55
                    do
                      local tmp56 = _L[6](c)
                      tmp55, tmp54 = tmp56, strm_PRIME
                    end
                    ::cont::
                    do
                      local x, strm1 = tmp55, tmp54
                      local exp1 = a(strm1)
                      if exp1.tag == "SOME" then
                      elseif exp1.tag == "NONE" then
                        return {tag = "SOME", payload = {x, strm1}}
                      else
                        _raise(_Match, "scan-num.sml:23:35")
                      end
                      do
                        local c1 = exp1.payload[1]
                        local strm_PRIME1 = exp1.payload[2]
                        if 48 <= c1 and c1 <= 57 then
                        else
                          return {tag = "SOME", payload = {x, strm1}}
                        end
                        do
                          local tmp56 = _Int_mul(10, x)
                          local tmp57 = _L[6](c1)
                          tmp55 = _Int_add(tmp56, tmp57)
                          tmp54 = strm_PRIME1
                          goto cont
                        end
                      end
                    end
                  end
                end
              end
            end)
            _L1[134] = _L1[135](_L1[131])
          end
          if _L1[134].tag == "SOME" then
            _L1[136] = _L1[134].payload
            _L1[137] = _L1[1].bibtex_or_biber
            _L1[138] = _L1[1].change_directory
            _L1[139] = _L1[1].check_driver
            _L1[140] = _L1[1].color
            _L1[141] = _L1[1].config_file
            _L1[142] = _L1[1].dvipdfmx_extraoptions
            _L1[143] = _L1[1].engine
            _L1[144] = _L1[1].engine_executable
            _L1[145] = _L1[1].file_line_error
            _L1[146] = _L1[1].fmt
            _L1[147] = _L1[1].fresh
            _L1[148] = _L1[1].halt_on_error
            _L1[149] = _L1[1].includeonly
            _L1[150] = _L1[1].interaction
            _L1[151] = _L1[1].jobname
            _L1[152] = _L1[1].make_depends
            _L1[153] = _L1[1].makeglossaries
            _L1[154] = _L1[1].makeindex
            _L1[155] = _L1[1].output
            _L1[156] = _L1[1].output_directory
            _L1[157] = _L1[1].output_format
            _L1[158] = _L1[1].package_support
            _L1[159] = _L1[1].print_output_directory
            _L1[160] = _L1[1].shell_escape
            _L1[161] = _L1[1].source_date_epoch
            _L1[162] = _L1[1].start_with_draft
            _L1[163] = _L1[1].synctex
            _L1[164] = _L1[1].tex_extraoptions
            _L1[165] = _L1[1].watch
            _L[536] = {bibtex_or_biber = _L1[137], change_directory = _L1[138], check_driver = _L1[139], color = _L1[140], config_file = _L1[141], dvipdfmx_extraoptions = _L1[142], engine = _L1[143], engine_executable = _L1[144], file_line_error = _L1[145], fmt = _L1[146], fresh = _L1[147], halt_on_error = _L1[148], includeonly = _L1[149], interaction = _L1[150], jobname = _L1[151], make_depends = _L1[152], makeglossaries = _L1[153], makeindex = _L1[154], max_iterations = {tag = "SOME", payload = _L1[136]}, output = _L1[155], output_directory = _L1[156], output_format = _L1[157], package_support = _L1[158], print_output_directory = _L1[159], shell_escape = _L1[160], source_date_epoch = _L1[161], start_with_draft = _L1[162], synctex = _L1[163], tex_extraoptions = _L1[164], watch = _L1[165]}
            _L[537] = _L1[132]
            goto cont18
          end
          if _L1[134].tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:185:62")
          end
          _L[124]({_L[118], "invalid value for --max-iterations option\n"})
          tmp15(1, true)
        end
        ::else33::
        if _L1[133].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:184:49")
        end
        _L[124]({_L[118], "multiple --max-iterations options\n"})
        tmp15(1, true)
      end
      ::else6::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_START_WITH_DRAFT" then
      else
        goto else7
      end
      do
        _L1[166] = _L1[3].payload[2]
        _L1[167] = _L1[1].start_with_draft
        if not _L1[167] then
          _L1[168] = _L1[1].bibtex_or_biber
          _L1[169] = _L1[1].change_directory
          _L1[170] = _L1[1].check_driver
          _L1[171] = _L1[1].color
          _L1[172] = _L1[1].config_file
          _L1[173] = _L1[1].dvipdfmx_extraoptions
          _L1[174] = _L1[1].engine
          _L1[175] = _L1[1].engine_executable
          _L1[176] = _L1[1].file_line_error
          _L1[177] = _L1[1].fmt
          _L1[178] = _L1[1].fresh
          _L1[179] = _L1[1].halt_on_error
          _L1[180] = _L1[1].includeonly
          _L1[181] = _L1[1].interaction
          _L1[182] = _L1[1].jobname
          _L1[183] = _L1[1].make_depends
          _L1[184] = _L1[1].makeglossaries
          _L1[185] = _L1[1].makeindex
          _L1[186] = _L1[1].max_iterations
          _L1[187] = _L1[1].output
          _L1[188] = _L1[1].output_directory
          _L1[189] = _L1[1].output_format
          _L1[190] = _L1[1].package_support
          _L1[191] = _L1[1].print_output_directory
          _L1[192] = _L1[1].shell_escape
          _L1[193] = _L1[1].source_date_epoch
          _L1[194] = _L1[1].synctex
          _L1[195] = _L1[1].tex_extraoptions
          _L[536] = {bibtex_or_biber = _L1[168], change_directory = _L1[169], check_driver = _L1[170], color = _L1[171], config_file = _L1[172], dvipdfmx_extraoptions = _L1[173], engine = _L1[174], engine_executable = _L1[175], file_line_error = _L1[176], fmt = _L1[177], fresh = _L1[178], halt_on_error = _L1[179], includeonly = _L1[180], interaction = _L1[181], jobname = _L1[182], make_depends = _L1[183], makeglossaries = _L1[184], makeindex = _L1[185], max_iterations = _L1[186], output = _L1[187], output_directory = _L1[188], output_format = _L1[189], package_support = _L1[190], print_output_directory = _L1[191], shell_escape = _L1[192], source_date_epoch = _L1[193], start_with_draft = true, synctex = _L1[194], tex_extraoptions = _L1[195], watch = _L1[1].watch}
          _L[537] = _L1[166]
          goto cont18
        end
        if _L1[167] then
        else
          _raise(_Match, "handle-options.sml:191:49")
        end
        _L[124]({_L[118], "multiple --start-with-draft options\n"})
        tmp15(1, true)
      end
      ::else7::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_WATCH" then
      else
        goto else8
      end
      do
        _L1[196] = _L1[3].payload[1].payload
        _L1[197] = _L1[3].payload[2]
        _L1[198] = _L1[1].watch
        if _L1[198].tag == "NONE" then
        else
          goto else33
        end
        do
          if _L1[196] == "fswatch" then
            _L1[199] = {tag = "SOME", payload = _L[343]}
          elseif _L1[196] == "inotifywait" then
            _L1[199] = {tag = "SOME", payload = _L[344]}
          elseif _L1[196] == "auto" then
            _L1[199] = {tag = "SOME", payload = _L[345]}
          else
            _L1[199] = NONE
          end
          if _L1[199].tag == "SOME" then
            _L1[200] = _L1[199].payload
            _L1[201] = _L1[1].bibtex_or_biber
            _L1[202] = _L1[1].change_directory
            _L1[203] = _L1[1].check_driver
            _L1[204] = _L1[1].color
            _L1[205] = _L1[1].config_file
            _L1[206] = _L1[1].dvipdfmx_extraoptions
            _L1[207] = _L1[1].engine
            _L1[208] = _L1[1].engine_executable
            _L1[209] = _L1[1].file_line_error
            _L1[210] = _L1[1].fmt
            _L1[211] = _L1[1].fresh
            _L1[212] = _L1[1].halt_on_error
            _L1[213] = _L1[1].includeonly
            _L1[214] = _L1[1].interaction
            _L1[215] = _L1[1].jobname
            _L1[216] = _L1[1].make_depends
            _L1[217] = _L1[1].makeglossaries
            _L1[218] = _L1[1].makeindex
            _L1[219] = _L1[1].max_iterations
            _L1[220] = _L1[1].output
            _L1[221] = _L1[1].output_directory
            _L1[222] = _L1[1].output_format
            _L1[223] = _L1[1].package_support
            _L1[224] = _L1[1].print_output_directory
            _L1[225] = _L1[1].shell_escape
            _L1[226] = _L1[1].source_date_epoch
            _L1[227] = _L1[1].start_with_draft
            _L1[228] = _L1[1].synctex
            _L1[229] = _L1[1].tex_extraoptions
            _L[536] = {bibtex_or_biber = _L1[201], change_directory = _L1[202], check_driver = _L1[203], color = _L1[204], config_file = _L1[205], dvipdfmx_extraoptions = _L1[206], engine = _L1[207], engine_executable = _L1[208], file_line_error = _L1[209], fmt = _L1[210], fresh = _L1[211], halt_on_error = _L1[212], includeonly = _L1[213], interaction = _L1[214], jobname = _L1[215], make_depends = _L1[216], makeglossaries = _L1[217], makeindex = _L1[218], max_iterations = _L1[219], output = _L1[220], output_directory = _L1[221], output_format = _L1[222], package_support = _L1[223], print_output_directory = _L1[224], shell_escape = _L1[225], source_date_epoch = _L1[226], start_with_draft = _L1[227], synctex = _L1[228], tex_extraoptions = _L1[229], watch = {tag = "SOME", payload = _L1[200]}}
            _L[537] = _L1[197]
            goto cont18
          end
          if _L1[199].tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:196:58")
          end
          _L[124]({_L[118], "invalid value for --watch option\n"})
          tmp15(1, true)
        end
        ::else33::
        if _L1[198].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:195:45")
        end
        _L[124]({_L[118], "multiple --watch options\n"})
        tmp15(1, true)
      end
      ::else8::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_HELP" then
      else
        goto else9
      end
      _L[535] = _L[373]()
      goto cont5
      ::else9::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_VERSION" then
      else
        goto else10
      end
      _L[124]({_L[118], "cluttex v0.7.0\n"})
      tmp15(0, true)
      ::else10::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_VERBOSE" then
        _L1[230] = _L1[3].payload[2]
        _ENV.CLUTTEX_VERBOSITY = _ENV.CLUTTEX_VERBOSITY + 1
        _L[536] = _L1[1]
        _L[537] = _L1[230]
        goto cont18
      end
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_COLOR" then
      else
        goto else11
      end
      do
        _L1[231] = _L1[3].payload[1].payload
        _L1[232] = _L1[3].payload[2]
        _L1[233] = _L1[1].color
        if _L1[233].tag == "NONE" then
        else
          goto else33
        end
        do
          if _L1[231] == "always" then
            _L1[234] = {tag = "SOME", payload = _L[276]}
          elseif _L1[231] == "auto" then
            _L1[234] = {tag = "SOME", payload = _L[277]}
          elseif _L1[231] == "never" then
            _L1[234] = {tag = "SOME", payload = _L[278]}
          else
            _L1[234] = NONE
          end
          if _L1[234].tag == "SOME" then
          else
            goto else34
          end
          do
            _L1[235] = _L1[234].payload
            _L[280](_L1[235])
            _L1[236] = _L1[1].bibtex_or_biber
            _L1[237] = _L1[1].change_directory
            _L1[238] = _L1[1].check_driver
            _L1[239] = _L1[1].config_file
            _L1[240] = _L1[1].dvipdfmx_extraoptions
            _L1[241] = _L1[1].engine
            _L1[242] = _L1[1].engine_executable
            _L1[243] = _L1[1].file_line_error
            _L1[244] = _L1[1].fmt
            _L1[245] = _L1[1].fresh
            _L1[246] = _L1[1].halt_on_error
            _L1[247] = _L1[1].includeonly
            _L1[248] = _L1[1].interaction
            _L1[249] = _L1[1].jobname
            _L1[250] = _L1[1].make_depends
            _L1[251] = _L1[1].makeglossaries
            _L1[252] = _L1[1].makeindex
            _L1[253] = _L1[1].max_iterations
            _L1[254] = _L1[1].output
            _L1[255] = _L1[1].output_directory
            _L1[256] = _L1[1].output_format
            _L1[257] = _L1[1].package_support
            _L1[258] = _L1[1].print_output_directory
            _L1[259] = _L1[1].shell_escape
            _L1[260] = _L1[1].source_date_epoch
            _L1[261] = _L1[1].start_with_draft
            _L1[262] = _L1[1].synctex
            _L1[263] = _L1[1].tex_extraoptions
            _L1[264] = _L1[1].watch
            _L[536] = {bibtex_or_biber = _L1[236], change_directory = _L1[237], check_driver = _L1[238], color = {tag = "SOME", payload = _L1[235]}, config_file = _L1[239], dvipdfmx_extraoptions = _L1[240], engine = _L1[241], engine_executable = _L1[242], file_line_error = _L1[243], fmt = _L1[244], fresh = _L1[245], halt_on_error = _L1[246], includeonly = _L1[247], interaction = _L1[248], jobname = _L1[249], make_depends = _L1[250], makeglossaries = _L1[251], makeindex = _L1[252], max_iterations = _L1[253], output = _L1[254], output_directory = _L1[255], output_format = _L1[256], package_support = _L1[257], print_output_directory = _L1[258], shell_escape = _L1[259], source_date_epoch = _L1[260], start_with_draft = _L1[261], synctex = _L1[262], tex_extraoptions = _L1[263], watch = _L1[264]}
            _L[537] = _L1[232]
            goto cont18
          end
          ::else34::
          if _L1[234].tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:208:56")
          end
          _L[124]({_L[118], "invalid value for --color option\n"})
          tmp15(1, true)
        end
        ::else33::
        if _L1[233].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:207:43")
        end
        _L[124]({_L[118], "multiple --color options\n"})
        tmp15(1, true)
      end
      ::else11::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_CHANGE_DIRECTORY" then
      else
        goto else12
      end
      do
        _L1[265] = _L1[3].payload[1].payload
        _L1[266] = _L1[3].payload[2]
        _L1[267] = _L1[1].change_directory
        if _L1[267].tag == "NONE" then
          _L1[268] = _L1[1].bibtex_or_biber
          _L1[269] = _L1[1].check_driver
          _L1[270] = _L1[1].color
          _L1[271] = _L1[1].config_file
          _L1[272] = _L1[1].dvipdfmx_extraoptions
          _L1[273] = _L1[1].engine
          _L1[274] = _L1[1].engine_executable
          _L1[275] = _L1[1].file_line_error
          _L1[276] = _L1[1].fmt
          _L1[277] = _L1[1].fresh
          _L1[278] = _L1[1].halt_on_error
          _L1[279] = _L1[1].includeonly
          _L1[280] = _L1[1].interaction
          _L1[281] = _L1[1].jobname
          _L1[282] = _L1[1].make_depends
          _L1[283] = _L1[1].makeglossaries
          _L1[284] = _L1[1].makeindex
          _L1[285] = _L1[1].max_iterations
          _L1[286] = _L1[1].output
          _L1[287] = _L1[1].output_directory
          _L1[288] = _L1[1].output_format
          _L1[289] = _L1[1].package_support
          _L1[290] = _L1[1].print_output_directory
          _L1[291] = _L1[1].shell_escape
          _L1[292] = _L1[1].source_date_epoch
          _L1[293] = _L1[1].start_with_draft
          _L1[294] = _L1[1].synctex
          _L1[295] = _L1[1].tex_extraoptions
          _L1[296] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[268], change_directory = {tag = "SOME", payload = _L1[265]}, check_driver = _L1[269], color = _L1[270], config_file = _L1[271], dvipdfmx_extraoptions = _L1[272], engine = _L1[273], engine_executable = _L1[274], file_line_error = _L1[275], fmt = _L1[276], fresh = _L1[277], halt_on_error = _L1[278], includeonly = _L1[279], interaction = _L1[280], jobname = _L1[281], make_depends = _L1[282], makeglossaries = _L1[283], makeindex = _L1[284], max_iterations = _L1[285], output = _L1[286], output_directory = _L1[287], output_format = _L1[288], package_support = _L1[289], print_output_directory = _L1[290], shell_escape = _L1[291], source_date_epoch = _L1[292], start_with_draft = _L1[293], synctex = _L1[294], tex_extraoptions = _L1[295], watch = _L1[296]}
          _L[537] = _L1[266]
          goto cont18
        end
        if _L1[267].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:216:51")
        end
        _L[124]({_L[118], "multiple --change-directory options\n"})
        tmp15(1, true)
      end
      ::else12::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_INCLUDEONLY" then
      else
        goto else13
      end
      do
        _L1[297] = _L1[3].payload[1].payload
        _L1[298] = _L1[3].payload[2]
        _L1[299] = _L1[1].includeonly
        if _L1[299].tag == "NONE" then
          _L1[300] = _L1[1].bibtex_or_biber
          _L1[301] = _L1[1].change_directory
          _L1[302] = _L1[1].check_driver
          _L1[303] = _L1[1].color
          _L1[304] = _L1[1].config_file
          _L1[305] = _L1[1].dvipdfmx_extraoptions
          _L1[306] = _L1[1].engine
          _L1[307] = _L1[1].engine_executable
          _L1[308] = _L1[1].file_line_error
          _L1[309] = _L1[1].fmt
          _L1[310] = _L1[1].fresh
          _L1[311] = _L1[1].halt_on_error
          _L1[312] = _L1[1].interaction
          _L1[313] = _L1[1].jobname
          _L1[314] = _L1[1].make_depends
          _L1[315] = _L1[1].makeglossaries
          _L1[316] = _L1[1].makeindex
          _L1[317] = _L1[1].max_iterations
          _L1[318] = _L1[1].output
          _L1[319] = _L1[1].output_directory
          _L1[320] = _L1[1].output_format
          _L1[321] = _L1[1].package_support
          _L1[322] = _L1[1].print_output_directory
          _L1[323] = _L1[1].shell_escape
          _L1[324] = _L1[1].source_date_epoch
          _L1[325] = _L1[1].start_with_draft
          _L1[326] = _L1[1].synctex
          _L1[327] = _L1[1].tex_extraoptions
          _L1[328] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[300], change_directory = _L1[301], check_driver = _L1[302], color = _L1[303], config_file = _L1[304], dvipdfmx_extraoptions = _L1[305], engine = _L1[306], engine_executable = _L1[307], file_line_error = _L1[308], fmt = _L1[309], fresh = _L1[310], halt_on_error = _L1[311], includeonly = {tag = "SOME", payload = _L1[297]}, interaction = _L1[312], jobname = _L1[313], make_depends = _L1[314], makeglossaries = _L1[315], makeindex = _L1[316], max_iterations = _L1[317], output = _L1[318], output_directory = _L1[319], output_format = _L1[320], package_support = _L1[321], print_output_directory = _L1[322], shell_escape = _L1[323], source_date_epoch = _L1[324], start_with_draft = _L1[325], synctex = _L1[326], tex_extraoptions = _L1[327], watch = _L1[328]}
          _L[537] = _L1[298]
          goto cont18
        end
        if _L1[299].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:220:46")
        end
        _L[124]({_L[118], "multiple --includeonly options\n"})
        tmp15(1, true)
      end
      ::else13::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_MAKE_DEPENDS" then
      else
        goto else14
      end
      do
        _L1[329] = _L1[3].payload[1].payload
        _L1[330] = _L1[3].payload[2]
        _L1[331] = _L1[1].make_depends
        if _L1[331].tag == "NONE" then
          _L1[332] = _L1[1].bibtex_or_biber
          _L1[333] = _L1[1].change_directory
          _L1[334] = _L1[1].check_driver
          _L1[335] = _L1[1].color
          _L1[336] = _L1[1].config_file
          _L1[337] = _L1[1].dvipdfmx_extraoptions
          _L1[338] = _L1[1].engine
          _L1[339] = _L1[1].engine_executable
          _L1[340] = _L1[1].file_line_error
          _L1[341] = _L1[1].fmt
          _L1[342] = _L1[1].fresh
          _L1[343] = _L1[1].halt_on_error
          _L1[344] = _L1[1].includeonly
          _L1[345] = _L1[1].interaction
          _L1[346] = _L1[1].jobname
          _L1[347] = _L1[1].makeglossaries
          _L1[348] = _L1[1].makeindex
          _L1[349] = _L1[1].max_iterations
          _L1[350] = _L1[1].output
          _L1[351] = _L1[1].output_directory
          _L1[352] = _L1[1].output_format
          _L1[353] = _L1[1].package_support
          _L1[354] = _L1[1].print_output_directory
          _L1[355] = _L1[1].shell_escape
          _L1[356] = _L1[1].source_date_epoch
          _L1[357] = _L1[1].start_with_draft
          _L1[358] = _L1[1].synctex
          _L1[359] = _L1[1].tex_extraoptions
          _L1[360] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[332], change_directory = _L1[333], check_driver = _L1[334], color = _L1[335], config_file = _L1[336], dvipdfmx_extraoptions = _L1[337], engine = _L1[338], engine_executable = _L1[339], file_line_error = _L1[340], fmt = _L1[341], fresh = _L1[342], halt_on_error = _L1[343], includeonly = _L1[344], interaction = _L1[345], jobname = _L1[346], make_depends = {tag = "SOME", payload = _L1[329]}, makeglossaries = _L1[347], makeindex = _L1[348], max_iterations = _L1[349], output = _L1[350], output_directory = _L1[351], output_format = _L1[352], package_support = _L1[353], print_output_directory = _L1[354], shell_escape = _L1[355], source_date_epoch = _L1[356], start_with_draft = _L1[357], synctex = _L1[358], tex_extraoptions = _L1[359], watch = _L1[360]}
          _L[537] = _L1[330]
          goto cont18
        end
        if _L1[331].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:224:47")
        end
        _L[124]({_L[118], "multiple --make-depends options\n"})
        tmp15(1, true)
      end
      ::else14::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_PRINT_OUTPUT_DIRECTORY" then
      else
        goto else15
      end
      do
        _L1[361] = _L1[3].payload[2]
        _L1[362] = _L1[1].print_output_directory
        if not _L1[362] then
          _L1[363] = _L1[1].bibtex_or_biber
          _L1[364] = _L1[1].change_directory
          _L1[365] = _L1[1].check_driver
          _L1[366] = _L1[1].color
          _L1[367] = _L1[1].config_file
          _L1[368] = _L1[1].dvipdfmx_extraoptions
          _L1[369] = _L1[1].engine
          _L1[370] = _L1[1].engine_executable
          _L1[371] = _L1[1].file_line_error
          _L1[372] = _L1[1].fmt
          _L1[373] = _L1[1].fresh
          _L1[374] = _L1[1].halt_on_error
          _L1[375] = _L1[1].includeonly
          _L1[376] = _L1[1].interaction
          _L1[377] = _L1[1].jobname
          _L1[378] = _L1[1].make_depends
          _L1[379] = _L1[1].makeglossaries
          _L1[380] = _L1[1].makeindex
          _L1[381] = _L1[1].max_iterations
          _L1[382] = _L1[1].output
          _L1[383] = _L1[1].output_directory
          _L1[384] = _L1[1].output_format
          _L1[385] = _L1[1].package_support
          _L1[386] = _L1[1].shell_escape
          _L1[387] = _L1[1].source_date_epoch
          _L1[388] = _L1[1].start_with_draft
          _L1[389] = _L1[1].synctex
          _L1[390] = _L1[1].tex_extraoptions
          _L[536] = {bibtex_or_biber = _L1[363], change_directory = _L1[364], check_driver = _L1[365], color = _L1[366], config_file = _L1[367], dvipdfmx_extraoptions = _L1[368], engine = _L1[369], engine_executable = _L1[370], file_line_error = _L1[371], fmt = _L1[372], fresh = _L1[373], halt_on_error = _L1[374], includeonly = _L1[375], interaction = _L1[376], jobname = _L1[377], make_depends = _L1[378], makeglossaries = _L1[379], makeindex = _L1[380], max_iterations = _L1[381], output = _L1[382], output_directory = _L1[383], output_format = _L1[384], package_support = _L1[385], print_output_directory = true, shell_escape = _L1[386], source_date_epoch = _L1[387], start_with_draft = _L1[388], synctex = _L1[389], tex_extraoptions = _L1[390], watch = _L1[1].watch}
          _L[537] = _L1[361]
          goto cont18
        end
        if _L1[362] then
        else
          _raise(_Match, "handle-options.sml:228:55")
        end
        _L[124]({_L[118], "multiple --print-output-directory options\n"})
        tmp15(1, true)
      end
      ::else15::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_PACKAGE_SUPPORT" then
      else
        goto else16
      end
      do
        _L1[391] = _L1[3].payload[1].payload
        _L1[392] = _L1[3].payload[2]
        do
          do
            _L1[395] = foldr(_COLON_COLON, nil)
            _L1[396] = #_L1[391]
            _L1[397] = _VectorOrArray_tabulate({_L1[396], function(i)
              return sub1({_L1[391], i})
            end})
            _L1[394] = _L1[395](_L1[397])
          end
          _L1[398], _L1[399], _L1[400] = nil, nil, _L1[394]
          ::cont21::
          do
            local _L2 = {}
            _L2[1], _L2[2], _L2[3] = _L1[398], _L1[399], _L1[400]
            if _L2[3] == nil then
            else
              goto else33
            end
            do
              if _L2[2] == nil then
              else
                goto else34
              end
              _L1[393] = revAppend(_L2[1], nil)
              goto cont20
              ::else34::
              _L2[4] = implodeRev(_L2[2])
              _L1[393] = revAppend({_L2[4], _L2[1]}, nil)
              goto cont20
            end
            ::else33::
            if _L2[3] ~= nil then
            else
              _raise(_Match, "string-1.sml:73:26")
            end
            do
              _L2[5] = _L2[3][1]
              _L2[6] = _L2[3][2]
              if _L2[5] == 44 then
                _L2[7] = true
              else
                _L2[7] = isSpace(_L2[5])
              end
              if _L2[7] then
              else
                _L1[398] = _L2[1]
                _L1[399] = {_L2[5], _L2[2]}
                _L1[400] = _L2[6]
                goto cont21
              end
              do
                if _L2[2] == nil then
                  _L1[398] = _L2[1]
                  _L1[399] = nil
                  _L1[400] = _L2[6]
                  goto cont21
                end
                _L2[8] = implodeRev(_L2[2])
                _L1[398] = {_L2[8], _L2[1]}
                _L1[399] = nil
                _L1[400] = _L2[6]
                goto cont21
              end
            end
          end
        end
        ::cont20::
        _L1[401] = foldl(function(a)
          if a[1] == "minted" then
            local ps = a[2]
            return {epstopdf = ps.epstopdf, minted = true, pdfx = ps.pdfx}
          end
          if a[1] == "epstopdf" then
            local ps = a[2]
            return {epstopdf = true, minted = ps.minted, pdfx = ps.pdfx}
          end
          if a[1] == "pdfx" then
            local ps = a[2]
            return {epstopdf = ps.epstopdf, minted = ps.minted, pdfx = true}
          end
          local pkg = a[1]
          local ps = a[2]
          if _ENV.CLUTTEX_VERBOSITY >= 1 then
          else
            return ps
          end
          do
            _L[302]("ClutTeX provides no special support for '" .. pkg .. "'.")
            return ps
          end
        end)
        _L1[402] = _L1[401](_L1[1].package_support)
        _L1[403] = _L1[402](_L1[393])
        _L1[404] = _L1[1].bibtex_or_biber
        _L1[405] = _L1[1].change_directory
        _L1[406] = _L1[1].check_driver
        _L1[407] = _L1[1].color
        _L1[408] = _L1[1].config_file
        _L1[409] = _L1[1].dvipdfmx_extraoptions
        _L1[410] = _L1[1].engine
        _L1[411] = _L1[1].engine_executable
        _L1[412] = _L1[1].file_line_error
        _L1[413] = _L1[1].fmt
        _L1[414] = _L1[1].fresh
        _L1[415] = _L1[1].halt_on_error
        _L1[416] = _L1[1].includeonly
        _L1[417] = _L1[1].interaction
        _L1[418] = _L1[1].jobname
        _L1[419] = _L1[1].make_depends
        _L1[420] = _L1[1].makeglossaries
        _L1[421] = _L1[1].makeindex
        _L1[422] = _L1[1].max_iterations
        _L1[423] = _L1[1].output
        _L1[424] = _L1[1].output_directory
        _L1[425] = _L1[1].output_format
        _L1[426] = _L1[1].print_output_directory
        _L1[427] = _L1[1].shell_escape
        _L1[428] = _L1[1].source_date_epoch
        _L1[429] = _L1[1].start_with_draft
        _L1[430] = _L1[1].synctex
        _L1[431] = _L1[1].tex_extraoptions
        _L[536] = {bibtex_or_biber = _L1[404], change_directory = _L1[405], check_driver = _L1[406], color = _L1[407], config_file = _L1[408], dvipdfmx_extraoptions = _L1[409], engine = _L1[410], engine_executable = _L1[411], file_line_error = _L1[412], fmt = _L1[413], fresh = _L1[414], halt_on_error = _L1[415], includeonly = _L1[416], interaction = _L1[417], jobname = _L1[418], make_depends = _L1[419], makeglossaries = _L1[420], makeindex = _L1[421], max_iterations = _L1[422], output = _L1[423], output_directory = _L1[424], output_format = _L1[425], package_support = _L1[403], print_output_directory = _L1[426], shell_escape = _L1[427], source_date_epoch = _L1[428], start_with_draft = _L1[429], synctex = _L1[430], tex_extraoptions = _L1[431], watch = _L1[1].watch}
        _L[537] = _L1[392]
        goto cont18
      end
      ::else16::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_CHECK_DRIVER" then
      else
        goto else17
      end
      do
        _L1[432] = _L1[3].payload[1].payload
        _L1[433] = _L1[3].payload[2]
        _L1[434] = _L1[1].check_driver
        if _L1[434].tag == "NONE" then
        else
          goto else33
        end
        do
          if _L1[432] == "dvipdfmx" then
            _L1[435] = {tag = "SOME", payload = _L[340]}
          elseif _L1[432] == "dvips" then
            _L1[435] = {tag = "SOME", payload = _L[341]}
          elseif _L1[432] == "dvisvgm" then
            _L1[435] = {tag = "SOME", payload = _L[342]}
          else
            _L1[435] = NONE
          end
          if _L1[435].tag == "SOME" then
            _L1[436] = _L1[435].payload
            _L1[437] = _L1[1].bibtex_or_biber
            _L1[438] = _L1[1].change_directory
            _L1[439] = _L1[1].color
            _L1[440] = _L1[1].config_file
            _L1[441] = _L1[1].dvipdfmx_extraoptions
            _L1[442] = _L1[1].engine
            _L1[443] = _L1[1].engine_executable
            _L1[444] = _L1[1].file_line_error
            _L1[445] = _L1[1].fmt
            _L1[446] = _L1[1].fresh
            _L1[447] = _L1[1].halt_on_error
            _L1[448] = _L1[1].includeonly
            _L1[449] = _L1[1].interaction
            _L1[450] = _L1[1].jobname
            _L1[451] = _L1[1].make_depends
            _L1[452] = _L1[1].makeglossaries
            _L1[453] = _L1[1].makeindex
            _L1[454] = _L1[1].max_iterations
            _L1[455] = _L1[1].output
            _L1[456] = _L1[1].output_directory
            _L1[457] = _L1[1].output_format
            _L1[458] = _L1[1].package_support
            _L1[459] = _L1[1].print_output_directory
            _L1[460] = _L1[1].shell_escape
            _L1[461] = _L1[1].source_date_epoch
            _L1[462] = _L1[1].start_with_draft
            _L1[463] = _L1[1].synctex
            _L1[464] = _L1[1].tex_extraoptions
            _L1[465] = _L1[1].watch
            _L[536] = {bibtex_or_biber = _L1[437], change_directory = _L1[438], check_driver = {tag = "SOME", payload = _L1[436]}, color = _L1[439], config_file = _L1[440], dvipdfmx_extraoptions = _L1[441], engine = _L1[442], engine_executable = _L1[443], file_line_error = _L1[444], fmt = _L1[445], fresh = _L1[446], halt_on_error = _L1[447], includeonly = _L1[448], interaction = _L1[449], jobname = _L1[450], make_depends = _L1[451], makeglossaries = _L1[452], makeindex = _L1[453], max_iterations = _L1[454], output = _L1[455], output_directory = _L1[456], output_format = _L1[457], package_support = _L1[458], print_output_directory = _L1[459], shell_escape = _L1[460], source_date_epoch = _L1[461], start_with_draft = _L1[462], synctex = _L1[463], tex_extraoptions = _L1[464], watch = _L1[465]}
            _L[537] = _L1[433]
            goto cont18
          end
          if _L1[435].tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:246:65")
          end
          _L[124]({_L[118], "invalid value for --check-driver option\n"})
          tmp15(1, true)
        end
        ::else33::
        if _L1[434].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:245:52")
        end
        _L[124]({_L[118], "multiple --check-driver options\n"})
        tmp15(1, true)
      end
      ::else17::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_SOURCE_DATE_EPOCH" then
      else
        goto else18
      end
      do
        _L1[466] = _L1[3].payload[1].payload
        _L1[467] = _L1[3].payload[2]
        _L1[468] = _L1[1].source_date_epoch
        if _L1[468].tag == "NONE" then
        else
          goto else33
        end
        do
          do
            if _L1[466] == "now" then
              _L1[469] = {tag = "SOME", payload = _L[347]}
              goto cont20
            end
            do
              if #_L1[466] > 0 then
              else
                _L1[470] = false
                goto cont21
              end
              do
                _L1[471] = tmp32(isDigit)
                _L1[470] = _L1[471](_L1[466])
              end
            end
            ::cont21::
            if _L1[470] then
              _L1[469] = {tag = "SOME", payload = {tag = "RAW", payload = _L1[466]}}
            else
              _L1[469] = NONE
            end
          end
          ::cont20::
          if _L1[469].tag == "SOME" then
            _L1[472] = _L1[469].payload
            _L1[473] = _L1[1].bibtex_or_biber
            _L1[474] = _L1[1].change_directory
            _L1[475] = _L1[1].check_driver
            _L1[476] = _L1[1].color
            _L1[477] = _L1[1].config_file
            _L1[478] = _L1[1].dvipdfmx_extraoptions
            _L1[479] = _L1[1].engine
            _L1[480] = _L1[1].engine_executable
            _L1[481] = _L1[1].file_line_error
            _L1[482] = _L1[1].fmt
            _L1[483] = _L1[1].fresh
            _L1[484] = _L1[1].halt_on_error
            _L1[485] = _L1[1].includeonly
            _L1[486] = _L1[1].interaction
            _L1[487] = _L1[1].jobname
            _L1[488] = _L1[1].make_depends
            _L1[489] = _L1[1].makeglossaries
            _L1[490] = _L1[1].makeindex
            _L1[491] = _L1[1].max_iterations
            _L1[492] = _L1[1].output
            _L1[493] = _L1[1].output_directory
            _L1[494] = _L1[1].output_format
            _L1[495] = _L1[1].package_support
            _L1[496] = _L1[1].print_output_directory
            _L1[497] = _L1[1].shell_escape
            _L1[498] = _L1[1].start_with_draft
            _L1[499] = _L1[1].synctex
            _L1[500] = _L1[1].tex_extraoptions
            _L1[501] = _L1[1].watch
            _L[536] = {bibtex_or_biber = _L1[473], change_directory = _L1[474], check_driver = _L1[475], color = _L1[476], config_file = _L1[477], dvipdfmx_extraoptions = _L1[478], engine = _L1[479], engine_executable = _L1[480], file_line_error = _L1[481], fmt = _L1[482], fresh = _L1[483], halt_on_error = _L1[484], includeonly = _L1[485], interaction = _L1[486], jobname = _L1[487], make_depends = _L1[488], makeglossaries = _L1[489], makeindex = _L1[490], max_iterations = _L1[491], output = _L1[492], output_directory = _L1[493], output_format = _L1[494], package_support = _L1[495], print_output_directory = _L1[496], shell_escape = _L1[497], source_date_epoch = {tag = "SOME", payload = _L1[472]}, start_with_draft = _L1[498], synctex = _L1[499], tex_extraoptions = _L1[500], watch = _L1[501]}
            _L[537] = _L1[467]
            goto cont18
          end
          if _L1[469].tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:253:68")
          end
          _L[124]({_L[118], "invalid value for --source-date-epoch option\n"})
          tmp15(1, true)
        end
        ::else33::
        if _L1[468].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:252:55")
        end
        _L[124]({_L[118], "multiple --source-date-epoch options\n"})
        tmp15(1, true)
      end
      ::else18::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_SYNCTEX" then
      else
        goto else19
      end
      do
        _L1[502] = _L1[3].payload[1].payload
        _L1[503] = _L1[3].payload[2]
        _L1[504] = _L1[1].synctex
        if _L1[504].tag == "NONE" then
          _L1[505] = _L1[1].bibtex_or_biber
          _L1[506] = _L1[1].change_directory
          _L1[507] = _L1[1].check_driver
          _L1[508] = _L1[1].color
          _L1[509] = _L1[1].config_file
          _L1[510] = _L1[1].dvipdfmx_extraoptions
          _L1[511] = _L1[1].engine
          _L1[512] = _L1[1].engine_executable
          _L1[513] = _L1[1].file_line_error
          _L1[514] = _L1[1].fmt
          _L1[515] = _L1[1].fresh
          _L1[516] = _L1[1].halt_on_error
          _L1[517] = _L1[1].includeonly
          _L1[518] = _L1[1].interaction
          _L1[519] = _L1[1].jobname
          _L1[520] = _L1[1].make_depends
          _L1[521] = _L1[1].makeglossaries
          _L1[522] = _L1[1].makeindex
          _L1[523] = _L1[1].max_iterations
          _L1[524] = _L1[1].output
          _L1[525] = _L1[1].output_directory
          _L1[526] = _L1[1].output_format
          _L1[527] = _L1[1].package_support
          _L1[528] = _L1[1].print_output_directory
          _L1[529] = _L1[1].shell_escape
          _L1[530] = _L1[1].source_date_epoch
          _L1[531] = _L1[1].start_with_draft
          _L1[532] = _L1[1].tex_extraoptions
          _L1[533] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[505], change_directory = _L1[506], check_driver = _L1[507], color = _L1[508], config_file = _L1[509], dvipdfmx_extraoptions = _L1[510], engine = _L1[511], engine_executable = _L1[512], file_line_error = _L1[513], fmt = _L1[514], fresh = _L1[515], halt_on_error = _L1[516], includeonly = _L1[517], interaction = _L1[518], jobname = _L1[519], make_depends = _L1[520], makeglossaries = _L1[521], makeindex = _L1[522], max_iterations = _L1[523], output = _L1[524], output_directory = _L1[525], output_format = _L1[526], package_support = _L1[527], print_output_directory = _L1[528], shell_escape = _L1[529], source_date_epoch = _L1[530], start_with_draft = _L1[531], synctex = {tag = "SOME", payload = _L1[502]}, tex_extraoptions = _L1[532], watch = _L1[533]}
          _L[537] = _L1[503]
          goto cont18
        end
        if _L1[504].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:259:42")
        end
        _L[124]({_L[118], "multiple --synctex options\n"})
        tmp15(1, true)
      end
      ::else19::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_FILE_LINE_ERROR" then
        _L1[534] = _L1[3].payload[1].payload
        _L1[535] = _L1[3].payload[2]
        _L1[536] = _L1[1].bibtex_or_biber
        _L1[537] = _L1[1].change_directory
        _L1[538] = _L1[1].check_driver
        _L1[539] = _L1[1].color
        _L1[540] = _L1[1].config_file
        _L1[541] = _L1[1].dvipdfmx_extraoptions
        _L1[542] = _L1[1].engine
        _L1[543] = _L1[1].engine_executable
        _L1[544] = _L1[1].fmt
        _L1[545] = _L1[1].fresh
        _L1[546] = _L1[1].halt_on_error
        _L1[547] = _L1[1].includeonly
        _L1[548] = _L1[1].interaction
        _L1[549] = _L1[1].jobname
        _L1[550] = _L1[1].make_depends
        _L1[551] = _L1[1].makeglossaries
        _L1[552] = _L1[1].makeindex
        _L1[553] = _L1[1].max_iterations
        _L1[554] = _L1[1].output
        _L1[555] = _L1[1].output_directory
        _L1[556] = _L1[1].output_format
        _L1[557] = _L1[1].package_support
        _L1[558] = _L1[1].print_output_directory
        _L1[559] = _L1[1].shell_escape
        _L1[560] = _L1[1].source_date_epoch
        _L1[561] = _L1[1].start_with_draft
        _L1[562] = _L1[1].synctex
        _L1[563] = _L1[1].tex_extraoptions
        _L[536] = {bibtex_or_biber = _L1[536], change_directory = _L1[537], check_driver = _L1[538], color = _L1[539], config_file = _L1[540], dvipdfmx_extraoptions = _L1[541], engine = _L1[542], engine_executable = _L1[543], file_line_error = _L1[534], fmt = _L1[544], fresh = _L1[545], halt_on_error = _L1[546], includeonly = _L1[547], interaction = _L1[548], jobname = _L1[549], make_depends = _L1[550], makeglossaries = _L1[551], makeindex = _L1[552], max_iterations = _L1[553], output = _L1[554], output_directory = _L1[555], output_format = _L1[556], package_support = _L1[557], print_output_directory = _L1[558], shell_escape = _L1[559], source_date_epoch = _L1[560], start_with_draft = _L1[561], synctex = _L1[562], tex_extraoptions = _L1[563], watch = _L1[1].watch}
        _L[537] = _L1[535]
        goto cont18
      end
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_INTERACTION" then
      else
        goto else20
      end
      do
        _L1[564] = _L1[3].payload[1].payload
        _L1[565] = _L1[3].payload[2]
        _L1[566] = _L1[1].interaction
        if _L1[566].tag == "NONE" then
        else
          goto else33
        end
        do
          if _L1[564] == "batchmode" then
            _L1[567] = {tag = "SOME", payload = _L[222]}
          elseif _L1[564] == "nonstopmode" then
            _L1[567] = {tag = "SOME", payload = _L[223]}
          elseif _L1[564] == "scrollmode" then
            _L1[567] = {tag = "SOME", payload = _L[224]}
          elseif _L1[564] == "errorstopmode" then
            _L1[567] = {tag = "SOME", payload = _L[225]}
          else
            _L1[567] = NONE
          end
          if _L1[567].tag == "SOME" then
            _L1[568] = _L1[567].payload
            _L1[569] = _L1[1].bibtex_or_biber
            _L1[570] = _L1[1].change_directory
            _L1[571] = _L1[1].check_driver
            _L1[572] = _L1[1].color
            _L1[573] = _L1[1].config_file
            _L1[574] = _L1[1].dvipdfmx_extraoptions
            _L1[575] = _L1[1].engine
            _L1[576] = _L1[1].engine_executable
            _L1[577] = _L1[1].file_line_error
            _L1[578] = _L1[1].fmt
            _L1[579] = _L1[1].fresh
            _L1[580] = _L1[1].halt_on_error
            _L1[581] = _L1[1].includeonly
            _L1[582] = _L1[1].jobname
            _L1[583] = _L1[1].make_depends
            _L1[584] = _L1[1].makeglossaries
            _L1[585] = _L1[1].makeindex
            _L1[586] = _L1[1].max_iterations
            _L1[587] = _L1[1].output
            _L1[588] = _L1[1].output_directory
            _L1[589] = _L1[1].output_format
            _L1[590] = _L1[1].package_support
            _L1[591] = _L1[1].print_output_directory
            _L1[592] = _L1[1].shell_escape
            _L1[593] = _L1[1].source_date_epoch
            _L1[594] = _L1[1].start_with_draft
            _L1[595] = _L1[1].synctex
            _L1[596] = _L1[1].tex_extraoptions
            _L1[597] = _L1[1].watch
            _L[536] = {bibtex_or_biber = _L1[569], change_directory = _L1[570], check_driver = _L1[571], color = _L1[572], config_file = _L1[573], dvipdfmx_extraoptions = _L1[574], engine = _L1[575], engine_executable = _L1[576], file_line_error = _L1[577], fmt = _L1[578], fresh = _L1[579], halt_on_error = _L1[580], includeonly = _L1[581], interaction = {tag = "SOME", payload = _L1[568]}, jobname = _L1[582], make_depends = _L1[583], makeglossaries = _L1[584], makeindex = _L1[585], max_iterations = _L1[586], output = _L1[587], output_directory = _L1[588], output_format = _L1[589], package_support = _L1[590], print_output_directory = _L1[591], shell_escape = _L1[592], source_date_epoch = _L1[593], start_with_draft = _L1[594], synctex = _L1[595], tex_extraoptions = _L1[596], watch = _L1[597]}
            _L[537] = _L1[565]
            goto cont18
          end
          if _L1[567].tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:265:59")
          end
          _L[124]({_L[118], "invalid argument for --interaction\n"})
          tmp15(1, true)
        end
        ::else33::
        if _L1[566].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:264:46")
        end
        _L[124]({_L[118], "multiple --interaction options\n"})
        tmp15(1, true)
      end
      ::else20::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_HALT_ON_ERROR" then
        _L1[598] = _L1[3].payload[1].payload
        _L1[599] = _L1[3].payload[2]
        _L1[600] = _L1[1].bibtex_or_biber
        _L1[601] = _L1[1].change_directory
        _L1[602] = _L1[1].check_driver
        _L1[603] = _L1[1].color
        _L1[604] = _L1[1].config_file
        _L1[605] = _L1[1].dvipdfmx_extraoptions
        _L1[606] = _L1[1].engine
        _L1[607] = _L1[1].engine_executable
        _L1[608] = _L1[1].file_line_error
        _L1[609] = _L1[1].fmt
        _L1[610] = _L1[1].fresh
        _L1[611] = _L1[1].includeonly
        _L1[612] = _L1[1].interaction
        _L1[613] = _L1[1].jobname
        _L1[614] = _L1[1].make_depends
        _L1[615] = _L1[1].makeglossaries
        _L1[616] = _L1[1].makeindex
        _L1[617] = _L1[1].max_iterations
        _L1[618] = _L1[1].output
        _L1[619] = _L1[1].output_directory
        _L1[620] = _L1[1].output_format
        _L1[621] = _L1[1].package_support
        _L1[622] = _L1[1].print_output_directory
        _L1[623] = _L1[1].shell_escape
        _L1[624] = _L1[1].source_date_epoch
        _L1[625] = _L1[1].start_with_draft
        _L1[626] = _L1[1].synctex
        _L1[627] = _L1[1].tex_extraoptions
        _L[536] = {bibtex_or_biber = _L1[600], change_directory = _L1[601], check_driver = _L1[602], color = _L1[603], config_file = _L1[604], dvipdfmx_extraoptions = _L1[605], engine = _L1[606], engine_executable = _L1[607], file_line_error = _L1[608], fmt = _L1[609], fresh = _L1[610], halt_on_error = _L1[598], includeonly = _L1[611], interaction = _L1[612], jobname = _L1[613], make_depends = _L1[614], makeglossaries = _L1[615], makeindex = _L1[616], max_iterations = _L1[617], output = _L1[618], output_directory = _L1[619], output_format = _L1[620], package_support = _L1[621], print_output_directory = _L1[622], shell_escape = _L1[623], source_date_epoch = _L1[624], start_with_draft = _L1[625], synctex = _L1[626], tex_extraoptions = _L1[627], watch = _L1[1].watch}
        _L[537] = _L1[599]
        goto cont18
      end
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_SHELL_ESCAPE" then
      else
        goto else21
      end
      do
        _L1[628] = _L1[3].payload[1].payload
        _L1[629] = _L1[3].payload[2]
        _L1[630] = _L1[1].shell_escape
        if _L1[630].tag == "NONE" then
          _L1[631] = _L1[1].bibtex_or_biber
          _L1[632] = _L1[1].change_directory
          _L1[633] = _L1[1].check_driver
          _L1[634] = _L1[1].color
          _L1[635] = _L1[1].config_file
          _L1[636] = _L1[1].dvipdfmx_extraoptions
          _L1[637] = _L1[1].engine
          _L1[638] = _L1[1].engine_executable
          _L1[639] = _L1[1].file_line_error
          _L1[640] = _L1[1].fmt
          _L1[641] = _L1[1].fresh
          _L1[642] = _L1[1].halt_on_error
          _L1[643] = _L1[1].includeonly
          _L1[644] = _L1[1].interaction
          _L1[645] = _L1[1].jobname
          _L1[646] = _L1[1].make_depends
          _L1[647] = _L1[1].makeglossaries
          _L1[648] = _L1[1].makeindex
          _L1[649] = _L1[1].max_iterations
          _L1[650] = _L1[1].output
          _L1[651] = _L1[1].output_directory
          _L1[652] = _L1[1].output_format
          _L1[653] = _L1[1].package_support
          _L1[654] = _L1[1].print_output_directory
          _L1[655] = _L1[1].source_date_epoch
          _L1[656] = _L1[1].start_with_draft
          _L1[657] = _L1[1].synctex
          _L1[658] = _L1[1].tex_extraoptions
          _L1[659] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[631], change_directory = _L1[632], check_driver = _L1[633], color = _L1[634], config_file = _L1[635], dvipdfmx_extraoptions = _L1[636], engine = _L1[637], engine_executable = _L1[638], file_line_error = _L1[639], fmt = _L1[640], fresh = _L1[641], halt_on_error = _L1[642], includeonly = _L1[643], interaction = _L1[644], jobname = _L1[645], make_depends = _L1[646], makeglossaries = _L1[647], makeindex = _L1[648], max_iterations = _L1[649], output = _L1[650], output_directory = _L1[651], output_format = _L1[652], package_support = _L1[653], print_output_directory = _L1[654], shell_escape = {tag = "SOME", payload = _L1[628]}, source_date_epoch = _L1[655], start_with_draft = _L1[656], synctex = _L1[657], tex_extraoptions = _L1[658], watch = _L1[659]}
          _L[537] = _L1[629]
          goto cont18
        end
        if _L1[630].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:272:48")
        end
        _L[124]({_L[118], "multiple --(no-)shell-escape / --shell-restricted options\n"})
        tmp15(1, true)
      end
      ::else21::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_JOBNAME" then
      else
        goto else22
      end
      do
        _L1[660] = _L1[3].payload[1].payload
        _L1[661] = _L1[3].payload[2]
        _L1[662] = _L1[1].jobname
        if _L1[662].tag == "NONE" then
          _L1[663] = _L1[1].bibtex_or_biber
          _L1[664] = _L1[1].change_directory
          _L1[665] = _L1[1].check_driver
          _L1[666] = _L1[1].color
          _L1[667] = _L1[1].config_file
          _L1[668] = _L1[1].dvipdfmx_extraoptions
          _L1[669] = _L1[1].engine
          _L1[670] = _L1[1].engine_executable
          _L1[671] = _L1[1].file_line_error
          _L1[672] = _L1[1].fmt
          _L1[673] = _L1[1].fresh
          _L1[674] = _L1[1].halt_on_error
          _L1[675] = _L1[1].includeonly
          _L1[676] = _L1[1].interaction
          _L1[677] = _L1[1].make_depends
          _L1[678] = _L1[1].makeglossaries
          _L1[679] = _L1[1].makeindex
          _L1[680] = _L1[1].max_iterations
          _L1[681] = _L1[1].output
          _L1[682] = _L1[1].output_directory
          _L1[683] = _L1[1].output_format
          _L1[684] = _L1[1].package_support
          _L1[685] = _L1[1].print_output_directory
          _L1[686] = _L1[1].shell_escape
          _L1[687] = _L1[1].source_date_epoch
          _L1[688] = _L1[1].start_with_draft
          _L1[689] = _L1[1].synctex
          _L1[690] = _L1[1].tex_extraoptions
          _L1[691] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[663], change_directory = _L1[664], check_driver = _L1[665], color = _L1[666], config_file = _L1[667], dvipdfmx_extraoptions = _L1[668], engine = _L1[669], engine_executable = _L1[670], file_line_error = _L1[671], fmt = _L1[672], fresh = _L1[673], halt_on_error = _L1[674], includeonly = _L1[675], interaction = _L1[676], jobname = {tag = "SOME", payload = _L1[660]}, make_depends = _L1[677], makeglossaries = _L1[678], makeindex = _L1[679], max_iterations = _L1[680], output = _L1[681], output_directory = _L1[682], output_format = _L1[683], package_support = _L1[684], print_output_directory = _L1[685], shell_escape = _L1[686], source_date_epoch = _L1[687], start_with_draft = _L1[688], synctex = _L1[689], tex_extraoptions = _L1[690], watch = _L1[691]}
          _L[537] = _L1[661]
          goto cont18
        end
        if _L1[662].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:276:42")
        end
        _L[124]({_L[118], "multiple --jobname options\n"})
        tmp15(1, true)
      end
      ::else22::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_FMT" then
      else
        goto else23
      end
      do
        _L1[692] = _L1[3].payload[1].payload
        _L1[693] = _L1[3].payload[2]
        _L1[694] = _L1[1].fmt
        if _L1[694].tag == "NONE" then
          _L1[695] = _L1[1].bibtex_or_biber
          _L1[696] = _L1[1].change_directory
          _L1[697] = _L1[1].check_driver
          _L1[698] = _L1[1].color
          _L1[699] = _L1[1].config_file
          _L1[700] = _L1[1].dvipdfmx_extraoptions
          _L1[701] = _L1[1].engine
          _L1[702] = _L1[1].engine_executable
          _L1[703] = _L1[1].file_line_error
          _L1[704] = _L1[1].fresh
          _L1[705] = _L1[1].halt_on_error
          _L1[706] = _L1[1].includeonly
          _L1[707] = _L1[1].interaction
          _L1[708] = _L1[1].jobname
          _L1[709] = _L1[1].make_depends
          _L1[710] = _L1[1].makeglossaries
          _L1[711] = _L1[1].makeindex
          _L1[712] = _L1[1].max_iterations
          _L1[713] = _L1[1].output
          _L1[714] = _L1[1].output_directory
          _L1[715] = _L1[1].output_format
          _L1[716] = _L1[1].package_support
          _L1[717] = _L1[1].print_output_directory
          _L1[718] = _L1[1].shell_escape
          _L1[719] = _L1[1].source_date_epoch
          _L1[720] = _L1[1].start_with_draft
          _L1[721] = _L1[1].synctex
          _L1[722] = _L1[1].tex_extraoptions
          _L1[723] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[695], change_directory = _L1[696], check_driver = _L1[697], color = _L1[698], config_file = _L1[699], dvipdfmx_extraoptions = _L1[700], engine = _L1[701], engine_executable = _L1[702], file_line_error = _L1[703], fmt = {tag = "SOME", payload = _L1[692]}, fresh = _L1[704], halt_on_error = _L1[705], includeonly = _L1[706], interaction = _L1[707], jobname = _L1[708], make_depends = _L1[709], makeglossaries = _L1[710], makeindex = _L1[711], max_iterations = _L1[712], output = _L1[713], output_directory = _L1[714], output_format = _L1[715], package_support = _L1[716], print_output_directory = _L1[717], shell_escape = _L1[718], source_date_epoch = _L1[719], start_with_draft = _L1[720], synctex = _L1[721], tex_extraoptions = _L1[722], watch = _L1[723]}
          _L[537] = _L1[693]
          goto cont18
        end
        if _L1[694].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:280:38")
        end
        _L[124]({_L[118], "multiple --fmt options\n"})
        tmp15(1, true)
      end
      ::else23::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_OUTPUT_DIRECTORY" then
      else
        goto else24
      end
      do
        _L1[724] = _L1[3].payload[1].payload
        _L1[725] = _L1[3].payload[2]
        _L1[726] = _L1[1].output_directory
        if _L1[726].tag == "NONE" then
          _L1[727] = _L1[1].bibtex_or_biber
          _L1[728] = _L1[1].change_directory
          _L1[729] = _L1[1].check_driver
          _L1[730] = _L1[1].color
          _L1[731] = _L1[1].config_file
          _L1[732] = _L1[1].dvipdfmx_extraoptions
          _L1[733] = _L1[1].engine
          _L1[734] = _L1[1].engine_executable
          _L1[735] = _L1[1].file_line_error
          _L1[736] = _L1[1].fmt
          _L1[737] = _L1[1].fresh
          _L1[738] = _L1[1].halt_on_error
          _L1[739] = _L1[1].includeonly
          _L1[740] = _L1[1].interaction
          _L1[741] = _L1[1].jobname
          _L1[742] = _L1[1].make_depends
          _L1[743] = _L1[1].makeglossaries
          _L1[744] = _L1[1].makeindex
          _L1[745] = _L1[1].max_iterations
          _L1[746] = _L1[1].output
          _L1[747] = _L1[1].output_format
          _L1[748] = _L1[1].package_support
          _L1[749] = _L1[1].print_output_directory
          _L1[750] = _L1[1].shell_escape
          _L1[751] = _L1[1].source_date_epoch
          _L1[752] = _L1[1].start_with_draft
          _L1[753] = _L1[1].synctex
          _L1[754] = _L1[1].tex_extraoptions
          _L1[755] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[727], change_directory = _L1[728], check_driver = _L1[729], color = _L1[730], config_file = _L1[731], dvipdfmx_extraoptions = _L1[732], engine = _L1[733], engine_executable = _L1[734], file_line_error = _L1[735], fmt = _L1[736], fresh = _L1[737], halt_on_error = _L1[738], includeonly = _L1[739], interaction = _L1[740], jobname = _L1[741], make_depends = _L1[742], makeglossaries = _L1[743], makeindex = _L1[744], max_iterations = _L1[745], output = _L1[746], output_directory = {tag = "SOME", payload = _L1[724]}, output_format = _L1[747], package_support = _L1[748], print_output_directory = _L1[749], shell_escape = _L1[750], source_date_epoch = _L1[751], start_with_draft = _L1[752], synctex = _L1[753], tex_extraoptions = _L1[754], watch = _L1[755]}
          _L[537] = _L1[725]
          goto cont18
        end
        if _L1[726].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:284:51")
        end
        _L[124]({_L[118], "multiple --output-directory options\n"})
        tmp15(1, true)
      end
      ::else24::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_OUTPUT_FORMAT" then
      else
        goto else25
      end
      do
        _L1[756] = _L1[3].payload[1].payload
        _L1[757] = _L1[3].payload[2]
        _L1[758] = _L1[1].output_format
        if _L1[758].tag == "NONE" then
        else
          goto else33
        end
        do
          if _L1[756] == "pdf" then
            _L1[759] = {tag = "SOME", payload = _L[230]}
          elseif _L1[756] == "dvi" then
            _L1[759] = {tag = "SOME", payload = _L[231]}
          else
            _L1[759] = NONE
          end
          if _L1[759].tag == "SOME" then
            _L1[760] = _L1[759].payload
            _L1[761] = _L1[1].bibtex_or_biber
            _L1[762] = _L1[1].change_directory
            _L1[763] = _L1[1].check_driver
            _L1[764] = _L1[1].color
            _L1[765] = _L1[1].config_file
            _L1[766] = _L1[1].dvipdfmx_extraoptions
            _L1[767] = _L1[1].engine
            _L1[768] = _L1[1].engine_executable
            _L1[769] = _L1[1].file_line_error
            _L1[770] = _L1[1].fmt
            _L1[771] = _L1[1].fresh
            _L1[772] = _L1[1].halt_on_error
            _L1[773] = _L1[1].includeonly
            _L1[774] = _L1[1].interaction
            _L1[775] = _L1[1].jobname
            _L1[776] = _L1[1].make_depends
            _L1[777] = _L1[1].makeglossaries
            _L1[778] = _L1[1].makeindex
            _L1[779] = _L1[1].max_iterations
            _L1[780] = _L1[1].output
            _L1[781] = _L1[1].output_directory
            _L1[782] = _L1[1].package_support
            _L1[783] = _L1[1].print_output_directory
            _L1[784] = _L1[1].shell_escape
            _L1[785] = _L1[1].source_date_epoch
            _L1[786] = _L1[1].start_with_draft
            _L1[787] = _L1[1].synctex
            _L1[788] = _L1[1].tex_extraoptions
            _L1[789] = _L1[1].watch
            _L[536] = {bibtex_or_biber = _L1[761], change_directory = _L1[762], check_driver = _L1[763], color = _L1[764], config_file = _L1[765], dvipdfmx_extraoptions = _L1[766], engine = _L1[767], engine_executable = _L1[768], file_line_error = _L1[769], fmt = _L1[770], fresh = _L1[771], halt_on_error = _L1[772], includeonly = _L1[773], interaction = _L1[774], jobname = _L1[775], make_depends = _L1[776], makeglossaries = _L1[777], makeindex = _L1[778], max_iterations = _L1[779], output = _L1[780], output_directory = _L1[781], output_format = {tag = "SOME", payload = _L1[760]}, package_support = _L1[782], print_output_directory = _L1[783], shell_escape = _L1[784], source_date_epoch = _L1[785], start_with_draft = _L1[786], synctex = _L1[787], tex_extraoptions = _L1[788], watch = _L1[789]}
            _L[537] = _L1[757]
            goto cont18
          end
          if _L1[759].tag == "NONE" then
          else
            _raise(_Match, "handle-options.sml:289:66")
          end
          _L[124]({_L[118], "invalid value for --output-format option\n"})
          tmp15(1, true)
        end
        ::else33::
        if _L1[758].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:288:53")
        end
        _L[124]({_L[118], "multiple --output-format options\n"})
        tmp15(1, true)
      end
      ::else25::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_TEX_OPTION" then
      else
        goto else26
      end
      do
        _L1[790] = _L1[3].payload[1].payload
        _L1[791] = _L1[3].payload[2]
        _L1[792] = _L[208](_L1[790])
        _L1[793] = _L1[1].bibtex_or_biber
        _L1[794] = _L1[1].change_directory
        _L1[795] = _L1[1].check_driver
        _L1[796] = _L1[1].color
        _L1[797] = _L1[1].config_file
        _L1[798] = _L1[1].dvipdfmx_extraoptions
        _L1[799] = _L1[1].engine
        _L1[800] = _L1[1].engine_executable
        _L1[801] = _L1[1].file_line_error
        _L1[802] = _L1[1].fmt
        _L1[803] = _L1[1].fresh
        _L1[804] = _L1[1].halt_on_error
        _L1[805] = _L1[1].includeonly
        _L1[806] = _L1[1].interaction
        _L1[807] = _L1[1].jobname
        _L1[808] = _L1[1].make_depends
        _L1[809] = _L1[1].makeglossaries
        _L1[810] = _L1[1].makeindex
        _L1[811] = _L1[1].max_iterations
        _L1[812] = _L1[1].output
        _L1[813] = _L1[1].output_directory
        _L1[814] = _L1[1].output_format
        _L1[815] = _L1[1].package_support
        _L1[816] = _L1[1].print_output_directory
        _L1[817] = _L1[1].shell_escape
        _L1[818] = _L1[1].source_date_epoch
        _L1[819] = _L1[1].start_with_draft
        _L1[820] = _L1[1].synctex
        _L1[821] = _L1[1].watch
        _L[536] = {bibtex_or_biber = _L1[793], change_directory = _L1[794], check_driver = _L1[795], color = _L1[796], config_file = _L1[797], dvipdfmx_extraoptions = _L1[798], engine = _L1[799], engine_executable = _L1[800], file_line_error = _L1[801], fmt = _L1[802], fresh = _L1[803], halt_on_error = _L1[804], includeonly = _L1[805], interaction = _L1[806], jobname = _L1[807], make_depends = _L1[808], makeglossaries = _L1[809], makeindex = _L1[810], max_iterations = _L1[811], output = _L1[812], output_directory = _L1[813], output_format = _L1[814], package_support = _L1[815], print_output_directory = _L1[816], shell_escape = _L1[817], source_date_epoch = _L1[818], start_with_draft = _L1[819], synctex = _L1[820], tex_extraoptions = {_L1[792], _L1[1].tex_extraoptions}, watch = _L1[821]}
        _L[537] = _L1[791]
        goto cont18
      end
      ::else26::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_TEX_OPTIONS" then
        _L1[822] = _L1[3].payload[1].payload
        _L1[823] = _L1[3].payload[2]
        _L1[824] = _L1[1].bibtex_or_biber
        _L1[825] = _L1[1].change_directory
        _L1[826] = _L1[1].check_driver
        _L1[827] = _L1[1].color
        _L1[828] = _L1[1].config_file
        _L1[829] = _L1[1].dvipdfmx_extraoptions
        _L1[830] = _L1[1].engine
        _L1[831] = _L1[1].engine_executable
        _L1[832] = _L1[1].file_line_error
        _L1[833] = _L1[1].fmt
        _L1[834] = _L1[1].fresh
        _L1[835] = _L1[1].halt_on_error
        _L1[836] = _L1[1].includeonly
        _L1[837] = _L1[1].interaction
        _L1[838] = _L1[1].jobname
        _L1[839] = _L1[1].make_depends
        _L1[840] = _L1[1].makeglossaries
        _L1[841] = _L1[1].makeindex
        _L1[842] = _L1[1].max_iterations
        _L1[843] = _L1[1].output
        _L1[844] = _L1[1].output_directory
        _L1[845] = _L1[1].output_format
        _L1[846] = _L1[1].package_support
        _L1[847] = _L1[1].print_output_directory
        _L1[848] = _L1[1].shell_escape
        _L1[849] = _L1[1].source_date_epoch
        _L1[850] = _L1[1].start_with_draft
        _L1[851] = _L1[1].synctex
        _L1[852] = _L1[1].watch
        _L[536] = {bibtex_or_biber = _L1[824], change_directory = _L1[825], check_driver = _L1[826], color = _L1[827], config_file = _L1[828], dvipdfmx_extraoptions = _L1[829], engine = _L1[830], engine_executable = _L1[831], file_line_error = _L1[832], fmt = _L1[833], fresh = _L1[834], halt_on_error = _L1[835], includeonly = _L1[836], interaction = _L1[837], jobname = _L1[838], make_depends = _L1[839], makeglossaries = _L1[840], makeindex = _L1[841], max_iterations = _L1[842], output = _L1[843], output_directory = _L1[844], output_format = _L1[845], package_support = _L1[846], print_output_directory = _L1[847], shell_escape = _L1[848], source_date_epoch = _L1[849], start_with_draft = _L1[850], synctex = _L1[851], tex_extraoptions = {_L1[822], _L1[1].tex_extraoptions}, watch = _L1[852]}
        _L[537] = _L1[823]
        goto cont18
      end
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_DVIPDFMX_OPTION" then
      else
        goto else27
      end
      do
        _L1[853] = _L1[3].payload[1].payload
        _L1[854] = _L1[3].payload[2]
        _L1[855] = _L[208](_L1[853])
        _L1[856] = _L1[1].bibtex_or_biber
        _L1[857] = _L1[1].change_directory
        _L1[858] = _L1[1].check_driver
        _L1[859] = _L1[1].color
        _L1[860] = _L1[1].config_file
        _L1[861] = _L1[1].engine
        _L1[862] = _L1[1].engine_executable
        _L1[863] = _L1[1].file_line_error
        _L1[864] = _L1[1].fmt
        _L1[865] = _L1[1].fresh
        _L1[866] = _L1[1].halt_on_error
        _L1[867] = _L1[1].includeonly
        _L1[868] = _L1[1].interaction
        _L1[869] = _L1[1].jobname
        _L1[870] = _L1[1].make_depends
        _L1[871] = _L1[1].makeglossaries
        _L1[872] = _L1[1].makeindex
        _L1[873] = _L1[1].max_iterations
        _L1[874] = _L1[1].output
        _L1[875] = _L1[1].output_directory
        _L1[876] = _L1[1].output_format
        _L1[877] = _L1[1].package_support
        _L1[878] = _L1[1].print_output_directory
        _L1[879] = _L1[1].shell_escape
        _L1[880] = _L1[1].source_date_epoch
        _L1[881] = _L1[1].start_with_draft
        _L1[882] = _L1[1].synctex
        _L1[883] = _L1[1].tex_extraoptions
        _L1[884] = _L1[1].watch
        _L[536] = {bibtex_or_biber = _L1[856], change_directory = _L1[857], check_driver = _L1[858], color = _L1[859], config_file = _L1[860], dvipdfmx_extraoptions = {_L1[855], _L1[1].dvipdfmx_extraoptions}, engine = _L1[861], engine_executable = _L1[862], file_line_error = _L1[863], fmt = _L1[864], fresh = _L1[865], halt_on_error = _L1[866], includeonly = _L1[867], interaction = _L1[868], jobname = _L1[869], make_depends = _L1[870], makeglossaries = _L1[871], makeindex = _L1[872], max_iterations = _L1[873], output = _L1[874], output_directory = _L1[875], output_format = _L1[876], package_support = _L1[877], print_output_directory = _L1[878], shell_escape = _L1[879], source_date_epoch = _L1[880], start_with_draft = _L1[881], synctex = _L1[882], tex_extraoptions = _L1[883], watch = _L1[884]}
        _L[537] = _L1[854]
        goto cont18
      end
      ::else27::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_DVIPDFMX_OPTIONS" then
        _L1[885] = _L1[3].payload[1].payload
        _L1[886] = _L1[3].payload[2]
        _L1[887] = _L1[1].bibtex_or_biber
        _L1[888] = _L1[1].change_directory
        _L1[889] = _L1[1].check_driver
        _L1[890] = _L1[1].color
        _L1[891] = _L1[1].config_file
        _L1[892] = _L1[1].dvipdfmx_extraoptions
        _L1[893] = _L1[1].engine
        _L1[894] = _L1[1].engine_executable
        _L1[895] = _L1[1].file_line_error
        _L1[896] = _L1[1].fmt
        _L1[897] = _L1[1].fresh
        _L1[898] = _L1[1].halt_on_error
        _L1[899] = _L1[1].includeonly
        _L1[900] = _L1[1].interaction
        _L1[901] = _L1[1].jobname
        _L1[902] = _L1[1].make_depends
        _L1[903] = _L1[1].makeglossaries
        _L1[904] = _L1[1].makeindex
        _L1[905] = _L1[1].max_iterations
        _L1[906] = _L1[1].output
        _L1[907] = _L1[1].output_directory
        _L1[908] = _L1[1].output_format
        _L1[909] = _L1[1].package_support
        _L1[910] = _L1[1].print_output_directory
        _L1[911] = _L1[1].shell_escape
        _L1[912] = _L1[1].source_date_epoch
        _L1[913] = _L1[1].start_with_draft
        _L1[914] = _L1[1].synctex
        _L1[915] = _L1[1].watch
        _L[536] = {bibtex_or_biber = _L1[887], change_directory = _L1[888], check_driver = _L1[889], color = _L1[890], config_file = _L1[891], dvipdfmx_extraoptions = _L1[892], engine = _L1[893], engine_executable = _L1[894], file_line_error = _L1[895], fmt = _L1[896], fresh = _L1[897], halt_on_error = _L1[898], includeonly = _L1[899], interaction = _L1[900], jobname = _L1[901], make_depends = _L1[902], makeglossaries = _L1[903], makeindex = _L1[904], max_iterations = _L1[905], output = _L1[906], output_directory = _L1[907], output_format = _L1[908], package_support = _L1[909], print_output_directory = _L1[910], shell_escape = _L1[911], source_date_epoch = _L1[912], start_with_draft = _L1[913], synctex = _L1[914], tex_extraoptions = {_L1[885], _L1[1].dvipdfmx_extraoptions}, watch = _L1[915]}
        _L[537] = _L1[886]
        goto cont18
      end
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_MAKEINDEX" then
      else
        goto else28
      end
      do
        _L1[916] = _L1[3].payload[1].payload
        _L1[917] = _L1[3].payload[2]
        _L1[918] = _L1[1].makeindex
        if _L1[918].tag == "NONE" then
          _L1[919] = _L1[1].bibtex_or_biber
          _L1[920] = _L1[1].change_directory
          _L1[921] = _L1[1].check_driver
          _L1[922] = _L1[1].color
          _L1[923] = _L1[1].config_file
          _L1[924] = _L1[1].dvipdfmx_extraoptions
          _L1[925] = _L1[1].engine
          _L1[926] = _L1[1].engine_executable
          _L1[927] = _L1[1].file_line_error
          _L1[928] = _L1[1].fmt
          _L1[929] = _L1[1].fresh
          _L1[930] = _L1[1].halt_on_error
          _L1[931] = _L1[1].includeonly
          _L1[932] = _L1[1].interaction
          _L1[933] = _L1[1].jobname
          _L1[934] = _L1[1].make_depends
          _L1[935] = _L1[1].makeglossaries
          _L1[936] = _L1[1].max_iterations
          _L1[937] = _L1[1].output
          _L1[938] = _L1[1].output_directory
          _L1[939] = _L1[1].output_format
          _L1[940] = _L1[1].package_support
          _L1[941] = _L1[1].print_output_directory
          _L1[942] = _L1[1].shell_escape
          _L1[943] = _L1[1].source_date_epoch
          _L1[944] = _L1[1].start_with_draft
          _L1[945] = _L1[1].synctex
          _L1[946] = _L1[1].tex_extraoptions
          _L1[947] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[919], change_directory = _L1[920], check_driver = _L1[921], color = _L1[922], config_file = _L1[923], dvipdfmx_extraoptions = _L1[924], engine = _L1[925], engine_executable = _L1[926], file_line_error = _L1[927], fmt = _L1[928], fresh = _L1[929], halt_on_error = _L1[930], includeonly = _L1[931], interaction = _L1[932], jobname = _L1[933], make_depends = _L1[934], makeglossaries = _L1[935], makeindex = {tag = "SOME", payload = _L1[916]}, max_iterations = _L1[936], output = _L1[937], output_directory = _L1[938], output_format = _L1[939], package_support = _L1[940], print_output_directory = _L1[941], shell_escape = _L1[942], source_date_epoch = _L1[943], start_with_draft = _L1[944], synctex = _L1[945], tex_extraoptions = _L1[946], watch = _L1[947]}
          _L[537] = _L1[917]
          goto cont18
        end
        if _L1[918].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:303:44")
        end
        _L[124]({_L[118], "multiple --makeindex options\n"})
        tmp15(1, true)
      end
      ::else28::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_BIBTEX" then
      else
        goto else29
      end
      do
        _L1[948] = _L1[3].payload[1].payload
        _L1[949] = _L1[3].payload[2]
        _L1[950] = _L1[1].bibtex_or_biber
        if _L1[950].tag == "NONE" then
          _L1[951] = _L1[1].change_directory
          _L1[952] = _L1[1].check_driver
          _L1[953] = _L1[1].color
          _L1[954] = _L1[1].config_file
          _L1[955] = _L1[1].dvipdfmx_extraoptions
          _L1[956] = _L1[1].engine
          _L1[957] = _L1[1].engine_executable
          _L1[958] = _L1[1].file_line_error
          _L1[959] = _L1[1].fmt
          _L1[960] = _L1[1].fresh
          _L1[961] = _L1[1].halt_on_error
          _L1[962] = _L1[1].includeonly
          _L1[963] = _L1[1].interaction
          _L1[964] = _L1[1].jobname
          _L1[965] = _L1[1].make_depends
          _L1[966] = _L1[1].makeglossaries
          _L1[967] = _L1[1].makeindex
          _L1[968] = _L1[1].max_iterations
          _L1[969] = _L1[1].output
          _L1[970] = _L1[1].output_directory
          _L1[971] = _L1[1].output_format
          _L1[972] = _L1[1].package_support
          _L1[973] = _L1[1].print_output_directory
          _L1[974] = _L1[1].shell_escape
          _L1[975] = _L1[1].source_date_epoch
          _L1[976] = _L1[1].start_with_draft
          _L1[977] = _L1[1].synctex
          _L1[978] = _L1[1].tex_extraoptions
          _L1[979] = _L1[1].watch
          _L[536] = {bibtex_or_biber = {tag = "SOME", payload = {tag = "BIBTEX", payload = _L1[948]}}, change_directory = _L1[951], check_driver = _L1[952], color = _L1[953], config_file = _L1[954], dvipdfmx_extraoptions = _L1[955], engine = _L1[956], engine_executable = _L1[957], file_line_error = _L1[958], fmt = _L1[959], fresh = _L1[960], halt_on_error = _L1[961], includeonly = _L1[962], interaction = _L1[963], jobname = _L1[964], make_depends = _L1[965], makeglossaries = _L1[966], makeindex = _L1[967], max_iterations = _L1[968], output = _L1[969], output_directory = _L1[970], output_format = _L1[971], package_support = _L1[972], print_output_directory = _L1[973], shell_escape = _L1[974], source_date_epoch = _L1[975], start_with_draft = _L1[976], synctex = _L1[977], tex_extraoptions = _L1[978], watch = _L1[979]}
          _L[537] = _L1[949]
          goto cont18
        end
        if _L1[950].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:307:41")
        end
        _L[124]({_L[118], "multiple --bibtex / --biber options\n"})
        tmp15(1, true)
      end
      ::else29::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_BIBER" then
      else
        goto else30
      end
      do
        _L1[980] = _L1[3].payload[1].payload
        _L1[981] = _L1[3].payload[2]
        _L1[982] = _L1[1].bibtex_or_biber
        if _L1[982].tag == "NONE" then
          _L1[983] = _L1[1].change_directory
          _L1[984] = _L1[1].check_driver
          _L1[985] = _L1[1].color
          _L1[986] = _L1[1].config_file
          _L1[987] = _L1[1].dvipdfmx_extraoptions
          _L1[988] = _L1[1].engine
          _L1[989] = _L1[1].engine_executable
          _L1[990] = _L1[1].file_line_error
          _L1[991] = _L1[1].fmt
          _L1[992] = _L1[1].fresh
          _L1[993] = _L1[1].halt_on_error
          _L1[994] = _L1[1].includeonly
          _L1[995] = _L1[1].interaction
          _L1[996] = _L1[1].jobname
          _L1[997] = _L1[1].make_depends
          _L1[998] = _L1[1].makeglossaries
          _L1[999] = _L1[1].makeindex
          _L1[1000] = _L1[1].max_iterations
          _L1[1001] = _L1[1].output
          _L1[1002] = _L1[1].output_directory
          _L1[1003] = _L1[1].output_format
          _L1[1004] = _L1[1].package_support
          _L1[1005] = _L1[1].print_output_directory
          _L1[1006] = _L1[1].shell_escape
          _L1[1007] = _L1[1].source_date_epoch
          _L1[1008] = _L1[1].start_with_draft
          _L1[1009] = _L1[1].synctex
          _L1[1010] = _L1[1].tex_extraoptions
          _L1[1011] = _L1[1].watch
          _L[536] = {bibtex_or_biber = {tag = "SOME", payload = {tag = "BIBER", payload = _L1[980]}}, change_directory = _L1[983], check_driver = _L1[984], color = _L1[985], config_file = _L1[986], dvipdfmx_extraoptions = _L1[987], engine = _L1[988], engine_executable = _L1[989], file_line_error = _L1[990], fmt = _L1[991], fresh = _L1[992], halt_on_error = _L1[993], includeonly = _L1[994], interaction = _L1[995], jobname = _L1[996], make_depends = _L1[997], makeglossaries = _L1[998], makeindex = _L1[999], max_iterations = _L1[1000], output = _L1[1001], output_directory = _L1[1002], output_format = _L1[1003], package_support = _L1[1004], print_output_directory = _L1[1005], shell_escape = _L1[1006], source_date_epoch = _L1[1007], start_with_draft = _L1[1008], synctex = _L1[1009], tex_extraoptions = _L1[1010], watch = _L1[1011]}
          _L[537] = _L1[981]
          goto cont18
        end
        if _L1[982].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:311:40")
        end
        _L[124]({_L[118], "multiple --bibtex / --biber options\n"})
        tmp15(1, true)
      end
      ::else30::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_MAKEGLOSSARIES" then
      else
        goto else31
      end
      do
        _L1[1012] = _L1[3].payload[1].payload
        _L1[1013] = _L1[3].payload[2]
        _L1[1014] = _L1[1].makeglossaries
        if _L1[1014].tag == "NONE" then
          _L1[1015] = _L1[1].bibtex_or_biber
          _L1[1016] = _L1[1].change_directory
          _L1[1017] = _L1[1].check_driver
          _L1[1018] = _L1[1].color
          _L1[1019] = _L1[1].config_file
          _L1[1020] = _L1[1].dvipdfmx_extraoptions
          _L1[1021] = _L1[1].engine
          _L1[1022] = _L1[1].engine_executable
          _L1[1023] = _L1[1].file_line_error
          _L1[1024] = _L1[1].fmt
          _L1[1025] = _L1[1].fresh
          _L1[1026] = _L1[1].halt_on_error
          _L1[1027] = _L1[1].includeonly
          _L1[1028] = _L1[1].interaction
          _L1[1029] = _L1[1].jobname
          _L1[1030] = _L1[1].make_depends
          _L1[1031] = _L1[1].makeindex
          _L1[1032] = _L1[1].max_iterations
          _L1[1033] = _L1[1].output
          _L1[1034] = _L1[1].output_directory
          _L1[1035] = _L1[1].output_format
          _L1[1036] = _L1[1].package_support
          _L1[1037] = _L1[1].print_output_directory
          _L1[1038] = _L1[1].shell_escape
          _L1[1039] = _L1[1].source_date_epoch
          _L1[1040] = _L1[1].start_with_draft
          _L1[1041] = _L1[1].synctex
          _L1[1042] = _L1[1].tex_extraoptions
          _L1[1043] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[1015], change_directory = _L1[1016], check_driver = _L1[1017], color = _L1[1018], config_file = _L1[1019], dvipdfmx_extraoptions = _L1[1020], engine = _L1[1021], engine_executable = _L1[1022], file_line_error = _L1[1023], fmt = _L1[1024], fresh = _L1[1025], halt_on_error = _L1[1026], includeonly = _L1[1027], interaction = _L1[1028], jobname = _L1[1029], make_depends = _L1[1030], makeglossaries = {tag = "SOME", payload = _L1[1012]}, makeindex = _L1[1031], max_iterations = _L1[1032], output = _L1[1033], output_directory = _L1[1034], output_format = _L1[1035], package_support = _L1[1036], print_output_directory = _L1[1037], shell_escape = _L1[1038], source_date_epoch = _L1[1039], start_with_draft = _L1[1040], synctex = _L1[1041], tex_extraoptions = _L1[1042], watch = _L1[1043]}
          _L[537] = _L1[1013]
          goto cont18
        end
        if _L1[1014].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:315:49")
        end
        _L[124]({_L[118], "multiple --makeglossaries options\n"})
        tmp15(1, true)
      end
      ::else31::
      if _L1[3].tag == "SOME" and _L1[3].payload[1].tag == "OPT_CONFIG_FILE" then
      else
        goto else32
      end
      do
        _L1[1044] = _L1[3].payload[1].payload
        _L1[1045] = _L1[3].payload[2]
        _L1[1046] = _L1[1].config_file
        if _L1[1046].tag == "NONE" then
          _L1[1047] = _L1[1].bibtex_or_biber
          _L1[1048] = _L1[1].change_directory
          _L1[1049] = _L1[1].check_driver
          _L1[1050] = _L1[1].color
          _L1[1051] = _L1[1].dvipdfmx_extraoptions
          _L1[1052] = _L1[1].engine
          _L1[1053] = _L1[1].engine_executable
          _L1[1054] = _L1[1].file_line_error
          _L1[1055] = _L1[1].fmt
          _L1[1056] = _L1[1].fresh
          _L1[1057] = _L1[1].halt_on_error
          _L1[1058] = _L1[1].includeonly
          _L1[1059] = _L1[1].interaction
          _L1[1060] = _L1[1].jobname
          _L1[1061] = _L1[1].make_depends
          _L1[1062] = _L1[1].makeglossaries
          _L1[1063] = _L1[1].makeindex
          _L1[1064] = _L1[1].max_iterations
          _L1[1065] = _L1[1].output
          _L1[1066] = _L1[1].output_directory
          _L1[1067] = _L1[1].output_format
          _L1[1068] = _L1[1].package_support
          _L1[1069] = _L1[1].print_output_directory
          _L1[1070] = _L1[1].shell_escape
          _L1[1071] = _L1[1].source_date_epoch
          _L1[1072] = _L1[1].start_with_draft
          _L1[1073] = _L1[1].synctex
          _L1[1074] = _L1[1].tex_extraoptions
          _L1[1075] = _L1[1].watch
          _L[536] = {bibtex_or_biber = _L1[1047], change_directory = _L1[1048], check_driver = _L1[1049], color = _L1[1050], config_file = {tag = "SOME", payload = _L1[1044]}, dvipdfmx_extraoptions = _L1[1051], engine = _L1[1052], engine_executable = _L1[1053], file_line_error = _L1[1054], fmt = _L1[1055], fresh = _L1[1056], halt_on_error = _L1[1057], includeonly = _L1[1058], interaction = _L1[1059], jobname = _L1[1060], make_depends = _L1[1061], makeglossaries = _L1[1062], makeindex = _L1[1063], max_iterations = _L1[1064], output = _L1[1065], output_directory = _L1[1066], output_format = _L1[1067], package_support = _L1[1068], print_output_directory = _L1[1069], shell_escape = _L1[1070], source_date_epoch = _L1[1071], start_with_draft = _L1[1072], synctex = _L1[1073], tex_extraoptions = _L1[1074], watch = _L1[1075]}
          _L[537] = _L1[1045]
          goto cont18
        end
        if _L1[1046].tag == "SOME" then
        else
          _raise(_Match, "handle-options.sml:319:46")
        end
        _L[124]({_L[118], "multiple --config-file options\n"})
        tmp15(1, true)
      end
      ::else32::
      if _L1[3].tag == "NONE" then
      else
        _raise(_Match, "handle-options.sml:167:7")
      end
      do
        if _L1[2] ~= nil and _L1[2][1] == "--" then
          _L[535] = {_L1[1], _L1[2][2]}
          goto cont5
        end
        if _L1[2] ~= nil then
        else
          if _L1[2] == nil then
          else
            _raise(_Match, "handle-options.sml:323:20")
          end
          _L[535] = _L[373]()
          goto cont5
        end
        do
          _L1[1076] = _L1[2][1]
          _L1[1077] = isPrefix("-")
          _L1[1078] = _L1[1077](_L1[1076])
          if _L1[1078] then
          else
            _L[535] = {_L1[1], _L1[2]}
            goto cont5
          end
          _L[124]({_L[118], "Unrecognized option: " .. _L1[1076] .. ".\n" .. "\n"})
          tmp15(1, true)
        end
      end
    end
  end
  ::cont5::
  _L[538] = _L[535][1]
  _L[539] = _L[535][2]
  _L[540] = _L[538].config_file
  do
    if _L[540].tag == "SOME" then
      _L[542] = {tag = "SOME", payload = _L[540].payload}
      goto cont18
    end
    if _L[540].tag == "NONE" then
      do
        _L[544] = tmp16("CLUTTEX_CONFIG_FILE")
        if _L[544] == nil then
          _L[543] = NONE
        else
          _L[543] = {tag = "SOME", payload = _L[544]}
        end
      end
      if _L[543].tag == "SOME" then
        _L[542] = {tag = "SOME", payload = _L[543].payload}
        goto cont18
      end
      if _L[543].tag == "NONE" then
        if _L[220] then
          do
            _L[546] = tmp16("APPDATA")
            if _L[546] == nil then
              _L[545] = NONE
            else
              _L[545] = {tag = "SOME", payload = _L[546]}
            end
          end
          if _L[545].tag == "SOME" then
            _L[542] = {tag = "SOME", payload = _L[545].payload .. "\\cluttex\\config.toml"}
            goto cont18
          elseif _L[545].tag == "NONE" then
            _L[542] = NONE
            goto cont18
          else
            _raise(_Match, "main.sml:754:45")
          end
        end
        do
          _L[548] = tmp16("XDG_CONFIG_HOME")
          if _L[548] == nil then
            _L[547] = NONE
          else
            _L[547] = {tag = "SOME", payload = _L[548]}
          end
        end
        if _L[547].tag == "SOME" then
          _L[542] = {tag = "SOME", payload = _L[547].payload .. "/cluttex/config.toml"}
          goto cont18
        end
        if _L[547].tag == "NONE" then
          do
            _L[550] = tmp16("HOME")
            if _L[550] == nil then
              _L[549] = NONE
            else
              _L[549] = {tag = "SOME", payload = _L[550]}
            end
          end
          if _L[549].tag == "SOME" then
            _L[542] = {tag = "SOME", payload = _L[549].payload .. "/.config/cluttex/config.toml"}
          elseif _L[549].tag == "NONE" then
            _L[542] = NONE
          else
            _raise(_Match, "main.sml:760:57")
          end
        else
          _raise(_Match, "main.sml:758:45")
        end
      else
        _raise(_Match, "main.sml:751:30")
      end
    else
      _raise(_Match, "main.sml:750:5")
    end
    ::cont18::
    if _L[542].tag == "NONE" then
      _L[541] = _L[363]
      goto cont6
    end
    if _L[542].tag == "SOME" then
    else
      _raise(_Match, "main.sml:764:32")
    end
    do
      _L[551] = _L[542].payload
      _L[552], _L[553] = _handle(function()
        local tmp54, tmp55, expect, skipWhiteSpace, skipWhiteSpaceAndGetc, skipUntilNewline, skipWhiteSpaceOrComment, skipWhiteSpaceOrCommentOrNewlineAndGetc, skipOptionalNewline, go, checkAllowedChar, go1, go2, readHexInt, readOctInt, readBinInt, readSigned, readUnsigned
        do
          local ins = _L[115](_L[551])
          tmp54 = _L[122](ins)
          tmp55 = function(strm, state)
            local tmp56 = _L[94](strm)
            if state == "START" and tmp56.tag == "NONE" then
              return NONE
            end
            if tmp56.tag == "NONE" then
              _raise(_L[172], "validate_utf8.sml:67:22")
            end
            if tmp56.tag == "SOME" then
              local c = tmp56.payload[1]
              local strm1 = tmp56.payload[2]
              local tmp57
              if state == "START" then
                if c < 128 then
                  tmp57 = _L[173]
                  goto cont
                end
                if c < 224 then
                  if 194 <= c then
                    tmp57 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:30:55")
                  end
                elseif c == 224 then
                  tmp57 = _L[174]
                  goto cont
                elseif c == 237 then
                  tmp57 = _L[175]
                  goto cont
                elseif c < 240 then
                  tmp57 = _L[179]
                  goto cont
                elseif c == 240 then
                  tmp57 = _L[176]
                  goto cont
                elseif c < 244 then
                  tmp57 = _L[180]
                  goto cont
                elseif c == 244 then
                  tmp57 = _L[177]
                  goto cont
                else
                  _raise(_L[172], "validate_utf8.sml:44:11")
                end
              end
              if state == "TAIL_1" then
                if 128 <= c then
                  if c < 192 then
                    tmp57 = _L[173]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:47:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:47:14")
                end
              end
              if state == "MID_1_OF_3_E0" then
                if 160 <= c then
                  if c < 192 then
                    tmp57 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:50:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:50:14")
                end
              end
              if state == "MID_1_OF_3_ED" then
                if 128 <= c then
                  if c < 160 then
                    tmp57 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:53:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:53:14")
                end
              end
              if state == "TAIL_2" then
                if 128 <= c then
                  if c < 192 then
                    tmp57 = _L[178]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:56:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:56:14")
                end
              end
              if state == "MID_1_OF_4_F0" then
                if 144 <= c then
                  if c < 192 then
                    tmp57 = _L[179]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:59:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:59:14")
                end
              end
              if state == "TAIL_3" then
                if 128 <= c then
                  if c < 192 then
                    tmp57 = _L[179]
                    goto cont
                  else
                    _raise(_L[172], "validate_utf8.sml:62:14")
                  end
                else
                  _raise(_L[172], "validate_utf8.sml:62:14")
                end
              end
              if state == "MID_1_OF_4_F4" then
                if 128 <= c then
                  if c < 144 then
                    tmp57 = _L[179]
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
              return {tag = "SOME", payload = {c, {strm1, tmp57}}}
            else
              _raise(_Match, "validate_utf8.sml:66:7")
            end
          end
          expect = function(c, strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
            else
              goto else1
            end
            do
              local tmp56 = toString(c)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = tmp56}}}, "parse_toml.sml:16:5")
            end
            ::else1::
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:28:9")
            end
            do
              local c_PRIME = exp.payload[1]
              local strm_PRIME = exp.payload[2]
              if c == c_PRIME then
                return strm_PRIME
              end
              local tmp56 = toString(c)
              local tmp57 = toString(c_PRIME)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp57, expected = tmp56}}}, "parse_toml.sml:19:5")
            end
          end
          skipWhiteSpace = function(a)
            local tmp56 = a
            ::cont::
            do
              local a1 = tmp56
              local exp
              do
                local strm = a1[1]
                exp = tmp55(strm, a1[2])
              end
              if exp.tag == "NONE" then
                return a1
              end
              if exp.tag == "SOME" then
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if c == 32 then
                  tmp56 = strm_PRIME
                  goto cont
                elseif c == 9 then
                  tmp56 = strm_PRIME
                  goto cont
                else
                  return a1
                end
              else
                _raise(_Match, "parse_toml.sml:34:9")
              end
            end
          end
          skipWhiteSpaceAndGetc = function(strm, state)
            local tmp56, tmp57 = strm, state
            ::cont::
            do
              local strm1, state1 = tmp56, tmp57
              local exp = tmp55(strm1, state1)
              if exp.tag == "SOME" and exp.payload[1] == 32 then
                local strm_PRIME = exp.payload[2]
                local strm2 = strm_PRIME[1]
                tmp56 = strm2
                tmp57 = strm_PRIME[2]
                goto cont
              elseif exp.tag == "SOME" and exp.payload[1] == 9 then
                local strm_PRIME = exp.payload[2]
                local strm2 = strm_PRIME[1]
                tmp56 = strm2
                tmp57 = strm_PRIME[2]
                goto cont
              else
                return exp
              end
            end
          end
          skipUntilNewline = function(a)
            local tmp56 = a
            ::cont::
            do
              local exp
              do
                local a1 = tmp56
                do
                  local strm = a1[1]
                  exp = tmp55(strm, a1[2])
                end
                if exp.tag == "NONE" then
                  return a1
                end
                if exp.tag == "SOME" and exp.payload[1] == 10 then
                  return exp.payload[2]
                end
                if exp.tag == "SOME" and exp.payload[1] == 13 then
                else
                  goto else1
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local strm = strm_PRIME[1]
                  return expect(10, strm, strm_PRIME[2])
                end
              end
              ::else1::
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:47:9")
              end
              do
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                local tmp57
                do
                  if c <= 127 then
                  else
                    tmp57 = false
                    goto cont1
                  end
                  do
                    local tmp58 = isPrint(c)
                    tmp57 = not tmp58
                  end
                end
                ::cont1::
                if tmp57 then
                else
                  tmp56 = strm_PRIME
                  goto cont
                end
                do
                  local tmp58 = toString(c)
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp58, expected = "printable character"}}}, "parse_toml.sml:19:5")
                end
              end
            end
          end
          skipWhiteSpaceOrComment = function(a)
            local tmp56 = a
            ::cont::
            do
              local exp
              do
                local a1 = tmp56
                do
                  local strm = a1[1]
                  exp = tmp55(strm, a1[2])
                end
                if exp.tag == "NONE" then
                  return a1
                end
                if exp.tag == "SOME" and exp.payload[1] == 35 then
                else
                  goto else1
                end
                do
                  return skipUntilNewline(exp.payload[2])
                end
              end
              ::else1::
              if exp.tag == "SOME" and exp.payload[1] == 32 then
                tmp56 = exp.payload[2]
                goto cont
              end
              if exp.tag == "SOME" and exp.payload[1] == 9 then
                tmp56 = exp.payload[2]
                goto cont
              end
              if exp.tag == "SOME" and exp.payload[1] == 13 then
              else
                goto else2
              end
              do
                local strm_PRIME = exp.payload[2]
                local strm = strm_PRIME[1]
                return expect(10, strm, strm_PRIME[2])
              end
              ::else2::
              if exp.tag == "SOME" and exp.payload[1] == 10 then
                return exp.payload[2]
              end
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:56:9")
              end
              do
                local tmp57 = toString(exp.payload[1])
                _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp57, expected = "whitespace or comment"}}}, "parse_toml.sml:19:5")
              end
            end
          end
          skipWhiteSpaceOrCommentOrNewlineAndGetc = function(a)
            local tmp56 = a
            ::cont::
            do
              local a1 = tmp56
              local exp
              do
                local strm = a1[1]
                exp = tmp55(strm, a1[2])
              end
              if exp.tag == "SOME" and exp.payload[1] == 35 then
              elseif exp.tag == "SOME" and exp.payload[1] == 32 then
                tmp56 = exp.payload[2]
                goto cont
              elseif exp.tag == "SOME" and exp.payload[1] == 9 then
                tmp56 = exp.payload[2]
                goto cont
              else
                if exp.tag == "SOME" and exp.payload[1] == 13 then
                elseif exp.tag == "SOME" and exp.payload[1] == 10 then
                  tmp56 = exp.payload[2]
                  goto cont
                else
                  return exp
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local strm = strm_PRIME[1]
                  tmp56 = expect(10, strm, strm_PRIME[2])
                  goto cont
                end
              end
              tmp56 = skipUntilNewline(exp.payload[2])
              goto cont
            end
          end
          skipOptionalNewline = function(a)
            local exp
            do
              local strm = a[1]
              exp = tmp55(strm, a[2])
            end
            if exp.tag == "SOME" and exp.payload[1] == 10 then
              return exp.payload[2]
            else
              if exp.tag == "SOME" and exp.payload[1] == 13 then
              else
                return a
              end
              do
                local strm_PRIME = exp.payload[2]
                local strm = strm_PRIME[1]
                return expect(10, strm, strm_PRIME[2])
              end
            end
          end
          local skipWhiteSpaceOrNewline = function(a)
            local tmp56 = a
            ::cont::
            do
              local a1 = tmp56
              local exp
              do
                local strm = a1[1]
                exp = tmp55(strm, a1[2])
              end
              if exp.tag == "NONE" then
                return a1
              end
              if exp.tag == "SOME" and exp.payload[1] == 13 then
              else
                goto else1
              end
              do
                local strm_PRIME = exp.payload[2]
                local strm = strm_PRIME[1]
                tmp56 = expect(10, strm, strm_PRIME[2])
                goto cont
              end
              ::else1::
              if exp.tag == "SOME" then
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if c == 32 then
                  tmp56 = strm_PRIME
                  goto cont
                elseif c == 9 then
                  tmp56 = strm_PRIME
                  goto cont
                elseif c == 10 then
                  tmp56 = strm_PRIME
                  goto cont
                else
                  return a1
                end
              else
                _raise(_Match, "parse_toml.sml:83:9")
              end
            end
          end
          local readHexDigit = function(strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "hexadecimal digit"}}}, "parse_toml.sml:16:5")
            end
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:93:9")
            end
            do
              local c = exp.payload[1]
              local strm_PRIME = exp.payload[2]
              if 48 <= c and c <= 57 then
                return {_Int_sub(c, 48), strm_PRIME}
              end
              if 65 <= c and c <= 70 then
                return {_Int_sub(c, 55), strm_PRIME}
              end
              if 97 <= c and c <= 102 then
                return {_Int_sub(c, 87), strm_PRIME}
              end
              local tmp56 = toString(c)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "hexadecimal digit"}}}, "parse_toml.sml:19:5")
            end
          end
          local readFourHexDigit = function(strm, state)
            local exp = readHexDigit(strm, state)
            local c3 = exp[1]
            local strm1 = exp[2]
            local exp1
            do
              local strm2 = strm1[1]
              exp1 = readHexDigit(strm2, strm1[2])
            end
            local c2 = exp1[1]
            local strm2 = exp1[2]
            local exp2
            do
              local strm3 = strm2[1]
              exp2 = readHexDigit(strm3, strm2[2])
            end
            local c1 = exp2[1]
            local strm3 = exp2[2]
            local exp3
            do
              local strm4 = strm3[1]
              exp3 = readHexDigit(strm4, strm3[2])
            end
            local c0 = exp3[1]
            local strm4 = exp3[2]
            return {_Int_add(_Int_mul(_Int_add(_Int_mul(_Int_add(_Int_mul(c3, 16), c2), 16), c1), 16), c0), strm4}
          end
          go = function(accum, strm, state)
            local tmp56, tmp57, tmp58 = accum, strm, state
            ::cont::
            do
              local accum1, exp
              do
                local strm1, state1
                accum1, strm1, state1 = tmp56, tmp57, tmp58
                exp = tmp55(strm1, state1)
                if exp.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, "parse_toml.sml:16:5")
                end
                if exp.tag == "SOME" and exp.payload[1] == 34 then
                else
                  goto else1
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp59 = implodeRev(accum1)
                  return {tmp59, strm_PRIME}
                end
              end
              ::else1::
              if exp.tag == "SOME" and exp.payload[1] == 92 then
              else
                goto else2
              end
              do
                local exp1
                do
                  local strm_PRIME = exp.payload[2]
                  do
                    local strm1 = strm_PRIME[1]
                    exp1 = tmp55(strm1, strm_PRIME[2])
                  end
                  if exp1.tag == "NONE" then
                    _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "escape sequence"}}}, "parse_toml.sml:16:5")
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 34 then
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local tmp59 = {34, accum1}
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME[2]
                    goto cont
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 92 then
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local tmp59 = {92, accum1}
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME[2]
                    goto cont
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 98 then
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local tmp59 = {8, accum1}
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME[2]
                    goto cont
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 102 then
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local tmp59 = {12, accum1}
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME[2]
                    goto cont
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 110 then
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local tmp59 = {10, accum1}
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME[2]
                    goto cont
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 114 then
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local tmp59 = {13, accum1}
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME[2]
                    goto cont
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 116 then
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local tmp59 = {9, accum1}
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME[2]
                    goto cont
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 117 then
                  else
                    goto else3
                  end
                  do
                    local strm_PRIME_PRIME = exp1.payload[2]
                    local exp2
                    do
                      local strm1 = strm_PRIME_PRIME[1]
                      exp2 = readFourHexDigit(strm1, strm_PRIME_PRIME[2])
                    end
                    local i = exp2[1]
                    local strm_PRIME_PRIME_PRIME = exp2[2]
                    local tmp59 = _L[187](i, accum1)
                    local strm1 = strm_PRIME_PRIME_PRIME[1]
                    tmp56 = tmp59
                    tmp57 = strm1
                    tmp58 = strm_PRIME_PRIME_PRIME[2]
                    goto cont
                  end
                end
                ::else3::
                if exp1.tag == "SOME" and exp1.payload[1] == 85 then
                else
                  goto else4
                end
                do
                  local strm_PRIME_PRIME = exp1.payload[2]
                  local exp2
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    exp2 = readFourHexDigit(strm1, strm_PRIME_PRIME[2])
                  end
                  local hi = exp2[1]
                  local strm_PRIME_PRIME_PRIME = exp2[2]
                  local exp3
                  do
                    local strm1 = strm_PRIME_PRIME_PRIME[1]
                    exp3 = readFourHexDigit(strm1, strm_PRIME_PRIME_PRIME[2])
                  end
                  local lo = exp3[1]
                  local strm_PRIME_PRIME_PRIME_PRIME = exp3[2]
                  if hi > 16 then
                    _raise({tag = _L[186], payload = _L[182]}, "parse_toml.sml:139:24")
                  end
                  local tmp59 = _L[187](_Int_add(_Int_mul(hi, 65536), lo), accum1)
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  tmp56 = tmp59
                  tmp57 = strm1
                  tmp58 = strm_PRIME_PRIME_PRIME_PRIME[2]
                  goto cont
                end
                ::else4::
                if exp1.tag == "SOME" then
                else
                  _raise(_Match, "parse_toml.sml:120:16")
                end
                do
                  local tmp59 = toString(exp1.payload[1])
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp59, expected = "escape sequence"}}}, "parse_toml.sml:19:5")
                end
              end
              ::else2::
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:116:11")
              end
              do
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                local tmp59 = c < 32 and c ~= 9
                local tmp60
                tmp60 = tmp59 or c == 127
                if tmp60 then
                else
                  local tmp61 = {c, accum1}
                  local strm1 = strm_PRIME[1]
                  tmp56 = tmp61
                  tmp57 = strm1
                  tmp58 = strm_PRIME[2]
                  goto cont
                end
                do
                  local tmp61 = toString(c)
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp61, expected = "printable character"}}}, "parse_toml.sml:19:5")
                end
              end
            end
          end
          checkAllowedChar = function(a)
            local tmp56 = a < 32 and (a ~= 9 and a ~= 10)
            local tmp57
            tmp57 = tmp56 or a == 127
            if tmp57 then
            else
              return a
            end
            do
              local tmp58 = toString(a)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp58, expected = "printable character"}}}, "parse_toml.sml:19:5")
            end
          end
          local function escape(accum, strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "escape sequence"}}}, "parse_toml.sml:16:5")
            end
            if exp.tag == "SOME" and exp.payload[1] == 34 then
            else
              goto else1
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = {34, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME[2])
            end
            ::else1::
            if exp.tag == "SOME" and exp.payload[1] == 92 then
            else
              goto else2
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = {92, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME[2])
            end
            ::else2::
            if exp.tag == "SOME" and exp.payload[1] == 98 then
            else
              goto else3
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = {8, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME[2])
            end
            ::else3::
            if exp.tag == "SOME" and exp.payload[1] == 102 then
            else
              goto else4
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = {12, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME[2])
            end
            ::else4::
            if exp.tag == "SOME" and exp.payload[1] == 110 then
            else
              goto else5
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = {10, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME[2])
            end
            ::else5::
            if exp.tag == "SOME" and exp.payload[1] == 114 then
            else
              goto else6
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = {13, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME[2])
            end
            ::else6::
            if exp.tag == "SOME" and exp.payload[1] == 116 then
            else
              goto else7
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = {9, accum}
              local strm1 = strm_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME[2])
            end
            ::else7::
            if exp.tag == "SOME" and exp.payload[1] == 117 then
            else
              goto else8
            end
            do
              local strm_PRIME = exp.payload[2]
              local exp1
              do
                local strm1 = strm_PRIME[1]
                exp1 = readFourHexDigit(strm1, strm_PRIME[2])
              end
              local i = exp1[1]
              local strm_PRIME_PRIME = exp1[2]
              local tmp56 = _L[187](i, accum)
              local strm1 = strm_PRIME_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME_PRIME[2])
            end
            ::else8::
            if exp.tag == "SOME" and exp.payload[1] == 85 then
            else
              goto else9
            end
            do
              local strm_PRIME = exp.payload[2]
              local exp1
              do
                local strm1 = strm_PRIME[1]
                exp1 = readFourHexDigit(strm1, strm_PRIME[2])
              end
              local hi = exp1[1]
              local strm_PRIME_PRIME = exp1[2]
              local exp2
              do
                local strm1 = strm_PRIME_PRIME[1]
                exp2 = readFourHexDigit(strm1, strm_PRIME_PRIME[2])
              end
              local lo = exp2[1]
              local strm_PRIME_PRIME_PRIME = exp2[2]
              local tmp56 = _L[187](_Int_add(_Int_mul(hi, 65536), lo), accum)
              local strm1 = strm_PRIME_PRIME_PRIME[1]
              return go1(tmp56, strm1, strm_PRIME_PRIME_PRIME[2])
            end
            ::else9::
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:204:11")
            end
            do
              local c = exp.payload[1]
              local strm_PRIME = exp.payload[2]
              if c == 32 or c == 9 then
              else
                goto else10
              end
              do
                local exp1
                do
                  local strm1 = strm_PRIME[1]
                  exp1 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
                end
                if exp1.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "newline"}}}, "parse_toml.sml:16:5")
                end
                if exp1.tag == "SOME" and exp1.payload[1] == 10 then
                else
                  goto else13
                end
                do
                  local tmp56 = skipWhiteSpaceOrNewline(exp1.payload[2])
                  local strm1 = tmp56[1]
                  return go1(accum, strm1, tmp56[2])
                end
                ::else13::
                if exp1.tag == "SOME" and exp1.payload[1] == 13 then
                else
                  goto else14
                end
                do
                  local strm_PRIME_PRIME = exp1.payload[2]
                  local tmp56
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp56 = expect(10, strm1, strm_PRIME_PRIME[2])
                  end
                  local tmp57 = skipWhiteSpaceOrNewline(tmp56)
                  local strm1 = tmp57[1]
                  return go1(accum, strm1, tmp57[2])
                end
                ::else14::
                if exp1.tag == "SOME" then
                else
                  _raise(_Match, "parse_toml.sml:229:17")
                end
                do
                  local tmp56 = toString(exp1.payload[1])
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "newline"}}}, "parse_toml.sml:19:5")
                end
              end
              ::else10::
              if c == 13 then
              else
                goto else11
              end
              do
                local tmp56
                do
                  local strm1 = strm_PRIME[1]
                  tmp56 = expect(10, strm1, strm_PRIME[2])
                end
                local tmp57 = skipWhiteSpaceOrNewline(tmp56)
                local strm1 = tmp57[1]
                return go1(accum, strm1, tmp57[2])
              end
              ::else11::
              if c == 10 then
              else
                goto else12
              end
              do
                local tmp56 = skipWhiteSpaceOrNewline(strm_PRIME)
                local strm1 = tmp56[1]
                return go1(accum, strm1, tmp56[2])
              end
              ::else12::
              local tmp56 = toString(c)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "escape sequence"}}}, "parse_toml.sml:19:5")
            end
          end
          go1 = function(accum, strm, state)
            local tmp56, tmp57, tmp58 = accum, strm, state
            ::cont::
            do
              local accum1, exp
              do
                local strm1, state1
                accum1, strm1, state1 = tmp56, tmp57, tmp58
                exp = tmp55(strm1, state1)
                if exp.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, "parse_toml.sml:16:5")
                end
                if exp.tag == "SOME" and exp.payload[1] == 92 then
                else
                  goto else1
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local strm2 = strm_PRIME[1]
                  return escape(accum1, strm2, strm_PRIME[2])
                end
              end
              ::else1::
              if exp.tag == "SOME" and exp.payload[1] == 34 then
              else
                goto else2
              end
              do
                local exp1
                do
                  local strm_PRIME = exp.payload[2]
                  do
                    local strm1 = strm_PRIME[1]
                    exp1 = tmp55(strm1, strm_PRIME[2])
                  end
                  if exp1.tag == "SOME" and exp1.payload[1] == 34 then
                  else
                    goto else4
                  end
                  do
                    local exp2
                    do
                      local strm_PRIME_PRIME = exp1.payload[2]
                      do
                        local strm1 = strm_PRIME_PRIME[1]
                        exp2 = tmp55(strm1, strm_PRIME_PRIME[2])
                      end
                      if exp2.tag == "SOME" and exp2.payload[1] == 34 then
                      else
                        goto else7
                      end
                      do
                        local strm_PRIME_PRIME_PRIME
                        do
                          strm_PRIME_PRIME_PRIME = exp2.payload[2]
                          local exp3
                          do
                            local strm1 = strm_PRIME_PRIME_PRIME[1]
                            exp3 = tmp55(strm1, strm_PRIME_PRIME_PRIME[2])
                          end
                          if exp3.tag == "SOME" and exp3.payload[1] == 34 then
                          else
                            goto else10
                          end
                          do
                            local strm_PRIME_PRIME_PRIME_PRIME
                            do
                              strm_PRIME_PRIME_PRIME_PRIME = exp3.payload[2]
                              local exp4
                              do
                                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                                exp4 = tmp55(strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                              end
                              if exp4.tag == "SOME" and exp4.payload[1] == 34 then
                              else
                                goto else11
                              end
                              do
                                local strm_PRIME_PRIME_PRIME_PRIME_PRIME = exp4.payload[2]
                                local tmp59 = implodeRev({34, {34, accum1}})
                                return {tmp59, strm_PRIME_PRIME_PRIME_PRIME_PRIME}
                              end
                            end
                            ::else11::
                            local tmp59 = implodeRev({34, accum1})
                            return {tmp59, strm_PRIME_PRIME_PRIME_PRIME}
                          end
                        end
                        ::else10::
                        local tmp59 = implodeRev(accum1)
                        return {tmp59, strm_PRIME_PRIME_PRIME}
                      end
                    end
                    ::else7::
                    if exp2.tag == "SOME" and exp2.payload[1] == 92 then
                    else
                      goto else8
                    end
                    do
                      local strm_PRIME_PRIME_PRIME = exp2.payload[2]
                      local tmp59 = {34, {34, accum1}}
                      local strm1 = strm_PRIME_PRIME_PRIME[1]
                      return escape(tmp59, strm1, strm_PRIME_PRIME_PRIME[2])
                    end
                    ::else8::
                    if exp2.tag == "SOME" and exp2.payload[1] == 13 then
                    else
                      goto else9
                    end
                    do
                      local strm_PRIME_PRIME_PRIME = exp2.payload[2]
                      local tmp59 = {10, {13, {34, {34, accum1}}}}
                      local tmp60
                      do
                        local strm1 = strm_PRIME_PRIME_PRIME[1]
                        tmp60 = expect(10, strm1, strm_PRIME_PRIME_PRIME[2])
                      end
                      local strm1 = tmp60[1]
                      tmp56 = tmp59
                      tmp57 = strm1
                      tmp58 = tmp60[2]
                      goto cont
                    end
                    ::else9::
                    if exp2.tag == "SOME" then
                    elseif exp2.tag == "NONE" then
                      _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, "parse_toml.sml:16:5")
                    else
                      _raise(_Match, "parse_toml.sml:170:21")
                    end
                    do
                      local c = exp2.payload[1]
                      local strm_PRIME_PRIME_PRIME = exp2.payload[2]
                      local tmp59 = checkAllowedChar(c)
                      local tmp60 = {tmp59, {34, {34, accum1}}}
                      local strm1 = strm_PRIME_PRIME_PRIME[1]
                      tmp56 = tmp60
                      tmp57 = strm1
                      tmp58 = strm_PRIME_PRIME_PRIME[2]
                      goto cont
                    end
                  end
                end
                ::else4::
                if exp1.tag == "SOME" and exp1.payload[1] == 92 then
                else
                  goto else5
                end
                do
                  local strm_PRIME_PRIME = exp1.payload[2]
                  local tmp59 = {34, accum1}
                  local strm1 = strm_PRIME_PRIME[1]
                  return escape(tmp59, strm1, strm_PRIME_PRIME[2])
                end
                ::else5::
                if exp1.tag == "SOME" and exp1.payload[1] == 13 then
                else
                  goto else6
                end
                do
                  local strm_PRIME_PRIME = exp1.payload[2]
                  local tmp59 = {10, {13, {34, accum1}}}
                  local tmp60
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    tmp60 = expect(10, strm1, strm_PRIME_PRIME[2])
                  end
                  local strm1 = tmp60[1]
                  tmp56 = tmp59
                  tmp57 = strm1
                  tmp58 = tmp60[2]
                  goto cont
                end
                ::else6::
                if exp1.tag == "SOME" then
                elseif exp1.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, "parse_toml.sml:16:5")
                else
                  _raise(_Match, "parse_toml.sml:168:16")
                end
                do
                  local c = exp1.payload[1]
                  local strm_PRIME_PRIME_PRIME = exp1.payload[2]
                  local tmp59 = checkAllowedChar(c)
                  local tmp60 = {tmp59, {34, accum1}}
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  tmp56 = tmp60
                  tmp57 = strm1
                  tmp58 = strm_PRIME_PRIME_PRIME[2]
                  goto cont
                end
              end
              ::else2::
              if exp.tag == "SOME" and exp.payload[1] == 13 then
              else
                goto else3
              end
              do
                local strm_PRIME = exp.payload[2]
                local tmp59 = {10, {13, accum1}}
                local tmp60
                do
                  local strm1 = strm_PRIME[1]
                  tmp60 = expect(10, strm1, strm_PRIME[2])
                end
                local strm1 = tmp60[1]
                tmp56 = tmp59
                tmp57 = strm1
                tmp58 = tmp60[2]
                goto cont
              end
              ::else3::
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:164:11")
              end
              do
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                local tmp59 = checkAllowedChar(c)
                local tmp60 = {tmp59, accum1}
                local strm1 = strm_PRIME[1]
                tmp56 = tmp60
                tmp57 = strm1
                tmp58 = strm_PRIME[2]
                goto cont
              end
            end
          end
          go2 = function(accum, strm, state)
            local tmp56, tmp57, tmp58 = accum, strm, state
            ::cont::
            do
              local accum1, exp
              do
                local strm1, state1
                accum1, strm1, state1 = tmp56, tmp57, tmp58
                exp = tmp55(strm1, state1)
                if exp.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, "parse_toml.sml:16:5")
                end
                if exp.tag == "SOME" and exp.payload[1] == 39 then
                else
                  goto else1
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp59 = implodeRev(accum1)
                  return {tmp59, strm_PRIME}
                end
              end
              ::else1::
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:248:11")
              end
              do
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                local tmp59 = c < 32 and c ~= 9
                local tmp60
                tmp60 = tmp59 or c == 127
                if tmp60 then
                else
                  local tmp61 = {c, accum1}
                  local strm1 = strm_PRIME[1]
                  tmp56 = tmp61
                  tmp57 = strm1
                  tmp58 = strm_PRIME[2]
                  goto cont
                end
                do
                  local tmp61 = toString(c)
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp61, expected = "printable character"}}}, "parse_toml.sml:19:5")
                end
              end
            end
          end
          local function readHexIntUnderscore(accum, strm)
            local tmp56, tmp57 = accum, strm
            ::cont::
            do
              local accum1, strm1 = tmp56, tmp57
              local exp
              do
                local strm2 = strm1[1]
                exp = tmp55(strm2, strm1[2])
              end
              if exp.tag == "NONE" then
                return {accum1, strm1}
              end
              if exp.tag == "SOME" and exp.payload[1] == 95 then
              else
                goto else1
              end
              do
                local strm_PRIME = exp.payload[2]
                local strm2 = strm_PRIME[1]
                return readHexInt(accum1, strm2, strm_PRIME[2])
              end
              ::else1::
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:324:9")
              end
              do
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if 48 <= c and c <= 57 then
                else
                  goto else2
                end
                do
                  local tmp58 = _L[22](16)
                  local tmp59 = mul(accum1, tmp58)
                  local tmp60 = _L[22](_Int_sub(c, 48))
                  local tmp61 = add(tmp59, tmp60)
                  tmp56 = tmp61
                  tmp57 = strm_PRIME
                  goto cont
                end
                ::else2::
                if 65 <= c and c <= 70 then
                else
                  goto else3
                end
                do
                  local tmp58 = _L[22](16)
                  local tmp59 = mul(accum1, tmp58)
                  local tmp60 = _L[22](_Int_sub(c, 55))
                  local tmp61 = add(tmp59, tmp60)
                  tmp56 = tmp61
                  tmp57 = strm_PRIME
                  goto cont
                end
                ::else3::
                if 97 <= c and c <= 102 then
                else
                  return {accum1, strm1}
                end
                do
                  local tmp58 = _L[22](16)
                  local tmp59 = mul(accum1, tmp58)
                  local tmp60 = _L[22](_Int_sub(c, 87))
                  local tmp61 = add(tmp59, tmp60)
                  tmp56 = tmp61
                  tmp57 = strm_PRIME
                  goto cont
                end
              end
            end
          end
          readHexInt = function(accum, strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "hexadecimal digit"}}}, "parse_toml.sml:16:5")
            end
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:292:9")
            end
            do
              local c = exp.payload[1]
              local strm_PRIME = exp.payload[2]
              if 48 <= c and c <= 57 then
              else
                goto else1
              end
              do
                local tmp56 = _L[22](16)
                local tmp57 = mul(accum, tmp56)
                local tmp58 = _L[22](_Int_sub(c, 48))
                local tmp59 = add(tmp57, tmp58)
                return readHexIntUnderscore(tmp59, strm_PRIME)
              end
              ::else1::
              if 65 <= c and c <= 70 then
              else
                goto else2
              end
              do
                local tmp56 = _L[22](16)
                local tmp57 = mul(accum, tmp56)
                local tmp58 = _L[22](_Int_sub(c, 55))
                local tmp59 = add(tmp57, tmp58)
                return readHexIntUnderscore(tmp59, strm_PRIME)
              end
              ::else2::
              if 97 <= c and c <= 102 then
              else
                goto else3
              end
              do
                local tmp56 = _L[22](16)
                local tmp57 = mul(accum, tmp56)
                local tmp58 = _L[22](_Int_sub(c, 87))
                local tmp59 = add(tmp57, tmp58)
                return readHexIntUnderscore(tmp59, strm_PRIME)
              end
              ::else3::
              local tmp56 = toString(c)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "hexadecimal digit"}}}, "parse_toml.sml:19:5")
            end
          end
          local function readOctIntUnderscore(accum, strm)
            local tmp56, tmp57 = accum, strm
            ::cont::
            do
              local accum1, strm1 = tmp56, tmp57
              local exp
              do
                local strm2 = strm1[1]
                exp = tmp55(strm2, strm1[2])
              end
              if exp.tag == "NONE" then
                return {accum1, strm1}
              end
              if exp.tag == "SOME" and exp.payload[1] == 95 then
              else
                goto else1
              end
              do
                local strm_PRIME = exp.payload[2]
                local strm2 = strm_PRIME[1]
                return readOctInt(accum1, strm2, strm_PRIME[2])
              end
              ::else1::
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:376:9")
              end
              do
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if 48 <= c and c <= 55 then
                else
                  return {accum1, strm1}
                end
                do
                  local tmp58 = _L[22](8)
                  local tmp59 = mul(accum1, tmp58)
                  local tmp60 = _L[22](_Int_sub(c, 48))
                  local tmp61 = add(tmp59, tmp60)
                  tmp56 = tmp61
                  tmp57 = strm_PRIME
                  goto cont
                end
              end
            end
          end
          readOctInt = function(accum, strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "octal digit"}}}, "parse_toml.sml:16:5")
            end
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:362:9")
            end
            do
              local c
              do
                c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if 48 <= c and c <= 55 then
                else
                  goto else1
                end
                do
                  local tmp56 = _L[22](8)
                  local tmp57 = mul(accum, tmp56)
                  local tmp58 = _L[22](_Int_sub(c, 48))
                  local tmp59 = add(tmp57, tmp58)
                  return readOctIntUnderscore(tmp59, strm_PRIME)
                end
              end
              ::else1::
              local tmp56 = toString(c)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "octal digit"}}}, "parse_toml.sml:19:5")
            end
          end
          local function readBinIntUnderscore(accum, strm)
            local tmp56, tmp57 = accum, strm
            ::cont::
            do
              local accum1, strm1 = tmp56, tmp57
              local exp
              do
                local strm2 = strm1[1]
                exp = tmp55(strm2, strm1[2])
              end
              if exp.tag == "NONE" then
                return {accum1, strm1}
              end
              if exp.tag == "SOME" and exp.payload[1] == 95 then
              else
                goto else1
              end
              do
                local strm_PRIME = exp.payload[2]
                local strm2 = strm_PRIME[1]
                return readBinInt(accum1, strm2, strm_PRIME[2])
              end
              ::else1::
              if exp.tag == "SOME" and exp.payload[1] == 48 then
              else
                goto else2
              end
              do
                local strm_PRIME = exp.payload[2]
                local tmp58 = _L[22](2)
                local tmp59 = mul(accum1, tmp58)
                tmp56 = tmp59
                tmp57 = strm_PRIME
                goto cont
              end
              ::else2::
              if exp.tag == "SOME" and exp.payload[1] == 49 then
              elseif exp.tag == "SOME" then
                return {accum1, strm1}
              else
                _raise(_Match, "parse_toml.sml:409:9")
              end
              do
                local strm_PRIME = exp.payload[2]
                local tmp58 = _L[22](2)
                local tmp59 = mul(accum1, tmp58)
                local tmp60 = _L[22](1)
                local tmp61 = add(tmp59, tmp60)
                tmp56 = tmp61
                tmp57 = strm_PRIME
                goto cont
              end
            end
          end
          readBinInt = function(accum, strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "binary digit"}}}, "parse_toml.sml:16:5")
            end
            if exp.tag == "SOME" and exp.payload[1] == 48 then
            else
              goto else1
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = _L[22](2)
              local tmp57 = mul(accum, tmp56)
              return readBinIntUnderscore(tmp57, strm_PRIME)
            end
            ::else1::
            if exp.tag == "SOME" and exp.payload[1] == 49 then
            else
              goto else2
            end
            do
              local strm_PRIME = exp.payload[2]
              local tmp56 = _L[22](2)
              local tmp57 = mul(accum, tmp56)
              local tmp58 = _L[22](1)
              local tmp59 = add(tmp57, tmp58)
              return readBinIntUnderscore(tmp59, strm_PRIME)
            end
            ::else2::
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:394:9")
            end
            do
              local tmp56 = toString(exp.payload[1])
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "binary digit"}}}, "parse_toml.sml:19:5")
            end
          end
          local readDecIntUnderscore, readDecInt
          readDecIntUnderscore = function(accum, hadUnderscore, strm)
            local tmp56, tmp57, tmp58 = accum, hadUnderscore, strm
            ::cont::
            do
              local accum1, hadUnderscore1, strm1 = tmp56, tmp57, tmp58
              local exp
              do
                local strm2 = strm1[1]
                exp = tmp55(strm2, strm1[2])
              end
              if exp.tag == "NONE" then
                return {accum1, hadUnderscore1, strm1}
              end
              if exp.tag == "SOME" and exp.payload[1] == 95 then
              else
                goto else1
              end
              do
                local strm_PRIME = exp.payload[2]
                local strm2 = strm_PRIME[1]
                return readDecInt(accum1, true, strm2, strm_PRIME[2])
              end
              ::else1::
              if exp.tag == "SOME" then
                local c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if 48 <= c and c <= 57 then
                  tmp56 = {c, accum1}
                  tmp57 = hadUnderscore1
                  tmp58 = strm_PRIME
                  goto cont
                else
                  return {accum1, hadUnderscore1, strm1}
                end
              else
                _raise(_Match, "parse_toml.sml:436:9")
              end
            end
          end
          readDecInt = function(accum, hadUnderscore, strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit"}}}, "parse_toml.sml:16:5")
            end
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:428:9")
            end
            do
              local c
              do
                c = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if 48 <= c and c <= 57 then
                else
                  goto else1
                end
                do
                  return readDecIntUnderscore({c, accum}, hadUnderscore, strm_PRIME)
                end
              end
              ::else1::
              local tmp56 = toString(c)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "digit"}}}, "parse_toml.sml:19:5")
            end
          end
          local readExpPart = function(accum_PRIME, e, strm, state)
            local exp
            do
              local exp1 = tmp55(strm, state)
              do
                if exp1.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "exponent part"}}}, "parse_toml.sml:16:5")
                end
                if exp1.tag == "SOME" and exp1.payload[1] == 43 then
                else
                  goto else1
                end
                do
                  local strm_PRIME_PRIME_PRIME = exp1.payload[2]
                  local tmp56 = {43, {e, accum_PRIME}}
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  exp = readDecInt(tmp56, false, strm1, strm_PRIME_PRIME_PRIME[2])
                  goto cont
                end
                ::else1::
                if exp1.tag == "SOME" and exp1.payload[1] == 45 then
                else
                  goto else2
                end
                do
                  local strm_PRIME_PRIME_PRIME = exp1.payload[2]
                  local tmp56 = {45, {e, accum_PRIME}}
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  exp = readDecInt(tmp56, false, strm1, strm_PRIME_PRIME_PRIME[2])
                  goto cont
                end
                ::else2::
                if exp1.tag == "SOME" then
                else
                  _raise(_Match, "parse_toml.sml:448:13")
                end
                do
                  local c
                  do
                    c = exp1.payload[1]
                    local strm_PRIME_PRIME_PRIME = exp1.payload[2]
                    if 48 <= c and c <= 57 then
                    else
                      goto else3
                    end
                    exp = readDecIntUnderscore({c, {e, accum_PRIME}}, false, strm_PRIME_PRIME_PRIME)
                    goto cont
                  end
                  ::else3::
                  local tmp56 = toString(c)
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "digit or sign (+/-)"}}}, "parse_toml.sml:19:5")
                end
              end
            end
            ::cont::
            local accum_PRIME_PRIME = exp[1]
            local strm_PRIME_PRIME_PRIME = exp[3]
            local tmp56 = implodeRev(accum_PRIME_PRIME)
            local tmp57 = _L[188](tmp56)
            return {tmp57, strm_PRIME_PRIME_PRIME}
          end
          readSigned = function(sign, d0, strm)
            local accum, strm_PRIME, checkPrefixZero, exp
            do
              local exp1 = readDecIntUnderscore({d0, {sign, nil}}, false, strm)
              accum = exp1[1]
              strm_PRIME = exp1[3]
              checkPrefixZero = function()
                if accum ~= nil and (accum[2] ~= nil and accum[2][2] == nil) then
                  return nil
                elseif d0 == 48 then
                  _raise({tag = _L[186], payload = _L[181]}, "parse_toml.sml:470:38")
                else
                  return nil
                end
              end
              do
                local strm1 = strm_PRIME[1]
                exp = tmp55(strm1, strm_PRIME[2])
              end
              if exp.tag == "SOME" and exp.payload[1] == 46 then
              else
                goto else1
              end
              do
                local accum_PRIME, strm_PRIME_PRIME_PRIME, exp2
                do
                  local strm_PRIME_PRIME = exp.payload[2]
                  checkPrefixZero()
                  local tmp56 = {46, accum}
                  local exp3
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    exp3 = readDecInt(tmp56, false, strm1, strm_PRIME_PRIME[2])
                  end
                  accum_PRIME = exp3[1]
                  strm_PRIME_PRIME_PRIME = exp3[3]
                  do
                    local strm1 = strm_PRIME_PRIME_PRIME[1]
                    exp2 = tmp55(strm1, strm_PRIME_PRIME_PRIME[2])
                  end
                  if exp2.tag == "SOME" and exp2.payload[1] == 101 then
                  else
                    goto else4
                  end
                  do
                    local strm_PRIME_PRIME_PRIME_PRIME = exp2.payload[2]
                    local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                    return readExpPart(accum_PRIME, 101, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                  end
                end
                ::else4::
                if exp2.tag == "SOME" and exp2.payload[1] == 69 then
                else
                  goto else5
                end
                do
                  local strm_PRIME_PRIME_PRIME_PRIME = exp2.payload[2]
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readExpPart(accum_PRIME, 69, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                end
                ::else5::
                local tmp56 = implodeRev(accum_PRIME)
                local tmp57 = _L[188](tmp56)
                return {tmp57, strm_PRIME_PRIME_PRIME}
              end
            end
            ::else1::
            if exp.tag == "SOME" and exp.payload[1] == 101 then
            else
              goto else2
            end
            do
              local strm_PRIME_PRIME = exp.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(accum, 101, strm1, strm_PRIME_PRIME[2])
            end
            ::else2::
            if exp.tag == "SOME" and exp.payload[1] == 69 then
            else
              goto else3
            end
            do
              local strm_PRIME_PRIME = exp.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(accum, 69, strm1, strm_PRIME_PRIME[2])
            end
            ::else3::
            checkPrefixZero()
            local tmp56 = implodeRev(accum)
            local tmp57
            do
              local tmp58 = scanString(function(a)
                return scan(DEC, a)
              end)
              tmp57 = tmp58(tmp56)
            end
            if tmp57.tag == "SOME" then
              return {{tag = "INTEGER", payload = tmp57.payload}, strm_PRIME}
            elseif tmp57.tag == "NONE" then
              _raise(Option, "option.sml:24:18")
            else
              _raise(_Match, "option.sml:23:5")
            end
          end
          local readTwoDigit = function(accum, strm, state)
            local exp = tmp55(strm, state)
            if exp.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit"}}}, "parse_toml.sml:16:5")
            end
            if exp.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:502:9")
            end
            do
              local c0
              do
                c0 = exp.payload[1]
                local strm_PRIME = exp.payload[2]
                if 48 <= c0 and c0 <= 57 then
                else
                  goto else1
                end
                do
                  local exp1
                  do
                    local strm1 = strm_PRIME[1]
                    exp1 = tmp55(strm1, strm_PRIME[2])
                  end
                  if exp1.tag == "NONE" then
                    _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit"}}}, "parse_toml.sml:16:5")
                  end
                  if exp1.tag == "SOME" then
                  else
                    _raise(_Match, "parse_toml.sml:506:15")
                  end
                  do
                    local c1 = exp1.payload[1]
                    local strm_PRIME_PRIME = exp1.payload[2]
                    if 48 <= c1 and c1 <= 57 then
                      local tmp56 = {c1, {c0, accum}}
                      local tmp57 = _Int_mul(_Int_sub(c0, 48), 10)
                      return {tmp56, _Int_add(tmp57, _Int_sub(c1, 48)), strm_PRIME_PRIME}
                    end
                    local tmp56 = toString(c1)
                    _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "digit"}}}, "parse_toml.sml:19:5")
                  end
                end
              end
              ::else1::
              local tmp56 = toString(c0)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "digit"}}}, "parse_toml.sml:19:5")
            end
          end
          local readMinSec = function(accum, strm, state)
            local exp = readTwoDigit(accum, strm, state)
            local accum_PRIME = exp[1]
            local min = exp[2]
            local strm_PRIME = exp[3]
            local strm_PRIME_PRIME
            do
              local strm1 = strm_PRIME[1]
              strm_PRIME_PRIME = expect(58, strm1, strm_PRIME[2])
            end
            local tmp56 = {58, accum_PRIME}
            local exp1
            do
              local strm1 = strm_PRIME_PRIME[1]
              exp1 = readTwoDigit(tmp56, strm1, strm_PRIME_PRIME[2])
            end
            local accum_PRIME_PRIME = exp1[1]
            local sec = exp1[2]
            local strm_PRIME_PRIME_PRIME = exp1[3]
            local tmp57 = min < 60 and sec <= 60
            if tmp57 then
            else
              _raise({tag = _L[186], payload = _L[184]}, "parse_toml.sml:535:18")
            end
            local exp2
            do
              local strm1 = strm_PRIME_PRIME_PRIME[1]
              exp2 = tmp55(strm1, strm_PRIME_PRIME_PRIME[2])
            end
            if exp2.tag == "SOME" and exp2.payload[1] == 46 then
            else
              return {accum_PRIME_PRIME, strm_PRIME_PRIME_PRIME}
            end
            do
              local tmp58, tmp59
              do
                local strm_PRIME_PRIME_PRIME_PRIME = exp2.payload[2]
                tmp59, tmp58 = {46, accum_PRIME_PRIME}, strm_PRIME_PRIME_PRIME_PRIME
              end
              ::cont::
              do
                local accum1, strm1 = tmp59, tmp58
                local exp3
                do
                  local strm2 = strm1[1]
                  exp3 = tmp55(strm2, strm1[2])
                end
                if exp3.tag == "NONE" then
                  return {accum1, strm1}
                end
                if exp3.tag == "SOME" then
                  local c = exp3.payload[1]
                  local strm_PRIME1 = exp3.payload[2]
                  if 48 <= c and c <= 57 then
                    tmp59 = {c, accum1}
                    tmp58 = strm_PRIME1
                    goto cont
                  else
                    return {accum1, strm1}
                  end
                else
                  _raise(_Match, "parse_toml.sml:520:11")
                end
              end
            end
          end
          local readTimePart = function(accum, strm, state)
            local accum_PRIME_PRIME, strm_PRIME_PRIME, exp
            do
              local exp1 = readTwoDigit(accum, strm, state)
              local accum_PRIME = exp1[1]
              local hour = exp1[2]
              local strm_PRIME = exp1[3]
              if hour < 24 then
              else
                _raise({tag = _L[186], payload = _L[184]}, "parse_toml.sml:545:46")
              end
              local tmp56 = {58, accum_PRIME}
              local tmp57
              do
                local strm1 = strm_PRIME[1]
                tmp57 = expect(58, strm1, strm_PRIME[2])
              end
              local exp2
              do
                local strm1 = tmp57[1]
                exp2 = readMinSec(tmp56, strm1, tmp57[2])
              end
              accum_PRIME_PRIME = exp2[1]
              strm_PRIME_PRIME = exp2[2]
              do
                local strm1 = strm_PRIME_PRIME[1]
                exp = tmp55(strm1, strm_PRIME_PRIME[2])
              end
              if exp.tag == "SOME" and exp.payload[1] == 90 then
              else
                goto else1
              end
              do
                local strm_PRIME_PRIME_PRIME = exp.payload[2]
                local tmp58 = implodeRev({90, accum_PRIME_PRIME})
                return {{tag = "DATETIME", payload = tmp58}, strm_PRIME_PRIME_PRIME}
              end
            end
            ::else1::
            if exp.tag == "SOME" and exp.payload[1] == 122 then
            else
              goto else2
            end
            do
              local strm_PRIME_PRIME_PRIME = exp.payload[2]
              local tmp56 = implodeRev({122, accum_PRIME_PRIME})
              return {{tag = "DATETIME", payload = tmp56}, strm_PRIME_PRIME_PRIME}
            end
            ::else2::
            if exp.tag == "SOME" and exp.payload[1] == 43 then
            else
              goto else3
            end
            do
              local strm_PRIME_PRIME_PRIME = exp.payload[2]
              local tmp56 = {43, accum_PRIME_PRIME}
              local exp1
              do
                local strm1 = strm_PRIME_PRIME_PRIME[1]
                exp1 = readTwoDigit(tmp56, strm1, strm_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME = exp1[1]
              local offsetHour = exp1[2]
              local strm_PRIME_PRIME_PRIME_PRIME = exp1[3]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                strm_PRIME_PRIME_PRIME_PRIME_PRIME = expect(58, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
              end
              local tmp57 = {58, accum_PRIME_PRIME_PRIME}
              local exp2
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME_PRIME[1]
                exp2 = readTwoDigit(tmp57, strm1, strm_PRIME_PRIME_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME_PRIME = exp2[1]
              local offsetMin = exp2[2]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME = exp2[3]
              if offsetHour < 24 and offsetMin < 60 then
              else
                _raise({tag = _L[186], payload = _L[184]}, "parse_toml.sml:567:24")
              end
              do
                local tmp58 = implodeRev(accum_PRIME_PRIME_PRIME_PRIME)
                return {{tag = "DATETIME", payload = tmp58}, strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME}
              end
            end
            ::else3::
            if exp.tag == "SOME" and exp.payload[1] == 45 then
            else
              goto else4
            end
            do
              local strm_PRIME_PRIME_PRIME = exp.payload[2]
              local tmp56 = {45, accum_PRIME_PRIME}
              local exp1
              do
                local strm1 = strm_PRIME_PRIME_PRIME[1]
                exp1 = readTwoDigit(tmp56, strm1, strm_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME = exp1[1]
              local offsetHour = exp1[2]
              local strm_PRIME_PRIME_PRIME_PRIME = exp1[3]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                strm_PRIME_PRIME_PRIME_PRIME_PRIME = expect(58, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
              end
              local tmp57 = {58, accum_PRIME_PRIME_PRIME}
              local exp2
              do
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME_PRIME[1]
                exp2 = readTwoDigit(tmp57, strm1, strm_PRIME_PRIME_PRIME_PRIME_PRIME[2])
              end
              local accum_PRIME_PRIME_PRIME_PRIME = exp2[1]
              local offsetMin = exp2[2]
              local strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME = exp2[3]
              if offsetHour < 24 and offsetMin < 60 then
              else
                _raise({tag = _L[186], payload = _L[184]}, "parse_toml.sml:580:24")
              end
              do
                local tmp58 = implodeRev(accum_PRIME_PRIME_PRIME_PRIME)
                return {{tag = "DATETIME", payload = tmp58}, strm_PRIME_PRIME_PRIME_PRIME_PRIME_PRIME}
              end
            end
            ::else4::
            local tmp56 = implodeRev(accum_PRIME_PRIME)
            return {{tag = "LOCAL_DATETIME", payload = tmp56}, strm_PRIME_PRIME}
          end
          readUnsigned = function(d0, strm)
            local revDigits, hadUnderscore, strm_PRIME, checkPrefixZero, tmp56
            do
              local exp = readDecIntUnderscore({d0, nil}, false, strm)
              revDigits = exp[1]
              hadUnderscore = exp[2]
              strm_PRIME = exp[3]
              checkPrefixZero = function()
                if revDigits ~= nil and revDigits[2] == nil then
                  return nil
                elseif d0 == 48 then
                  _raise({tag = _L[186], payload = _L[181]}, "parse_toml.sml:637:38")
                else
                  return nil
                end
              end
              do
                local strm1 = strm_PRIME[1]
                tmp56 = tmp55(strm1, strm_PRIME[2])
              end
              local tmp57 = revDigits ~= nil and (revDigits[2] ~= nil and (revDigits[2][2] ~= nil and (revDigits[2][2][2] ~= nil and revDigits[2][2][2][2] == nil)))
              local tmp58
              tmp58 = tmp57 and (not hadUnderscore and (tmp56.tag == "SOME" and tmp56.payload[1] == 45))
              if tmp58 then
              else
                goto else1
              end
              do
                local tmp59, accum_PRIME, month, strm_PRIME_PRIME
                do
                  local tmp60 = revDigits[1]
                  local tmp61 = revDigits[2][1]
                  local tmp62 = revDigits[2][2][1]
                  local strm_PRIME_PRIME1 = tmp56.payload[2]
                  local tmp63 = _Int_mul(_Int_sub(d0, 48), 10)
                  local tmp64 = _Int_mul(_Int_add(tmp63, _Int_sub(tmp62, 48)), 10)
                  local tmp65 = _Int_mul(_Int_add(tmp64, _Int_sub(tmp61, 48)), 10)
                  tmp59 = _Int_add(tmp65, _Int_sub(tmp60, 48))
                  local tmp66 = {45, revDigits}
                  local exp1
                  do
                    local strm1 = strm_PRIME_PRIME1[1]
                    exp1 = readTwoDigit(tmp66, strm1, strm_PRIME_PRIME1[2])
                  end
                  accum_PRIME = exp1[1]
                  month = exp1[2]
                  local strm_PRIME1 = exp1[3]
                  do
                    local strm1 = strm_PRIME1[1]
                    strm_PRIME_PRIME = expect(45, strm1, strm_PRIME1[2])
                  end
                end
                local accum_PRIME_PRIME, strm_PRIME_PRIME_PRIME, tmp60
                do
                  local exp1
                  do
                    local strm1 = strm_PRIME_PRIME[1]
                    exp1 = readTwoDigit(accum_PRIME, strm1, strm_PRIME_PRIME[2])
                  end
                  accum_PRIME_PRIME = exp1[1]
                  local mday = exp1[2]
                  strm_PRIME_PRIME_PRIME = exp1[3]
                  do
                    if month == 1 then
                      tmp60 = 1 <= mday and mday <= 31
                      goto cont
                    end
                    if month == 2 and mday == 29 then
                    elseif month == 2 then
                      tmp60 = 1 <= mday and mday <= 28
                      goto cont
                    elseif month == 3 then
                      tmp60 = 1 <= mday and mday <= 31
                      goto cont
                    elseif month == 4 then
                      tmp60 = 1 <= mday and mday <= 30
                      goto cont
                    elseif month == 5 then
                      tmp60 = 1 <= mday and mday <= 31
                      goto cont
                    elseif month == 6 then
                      tmp60 = 1 <= mday and mday <= 30
                      goto cont
                    elseif month == 7 then
                      tmp60 = 1 <= mday and mday <= 31
                      goto cont
                    elseif month == 8 then
                      tmp60 = 1 <= mday and mday <= 31
                      goto cont
                    elseif month == 9 then
                      tmp60 = 1 <= mday and mday <= 30
                      goto cont
                    elseif month == 10 then
                      tmp60 = 1 <= mday and mday <= 31
                      goto cont
                    elseif month == 11 then
                      tmp60 = 1 <= mday and mday <= 30
                      goto cont
                    else
                      tmp60 = month == 12 and (1 <= mday and mday <= 31)
                      goto cont
                    end
                    do
                      local tmp61 = rem(tmp59, 4)
                      if tmp61 == 0 then
                      else
                        tmp60 = false
                        goto cont
                      end
                      do
                        local tmp62 = rem(tmp59, 100)
                        if tmp62 ~= 0 then
                          tmp60 = true
                          goto cont
                        end
                        local tmp63 = rem(tmp59, 400)
                        tmp60 = tmp63 == 0
                      end
                    end
                  end
                end
                ::cont::
                if tmp60 then
                else
                  _raise({tag = _L[186], payload = _L[183]}, "parse_toml.sml:599:18")
                end
                local exp1
                do
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  exp1 = tmp55(strm1, strm_PRIME_PRIME_PRIME[2])
                end
                if exp1.tag == "SOME" and exp1.payload[1] == 84 then
                else
                  goto else6
                end
                do
                  local strm_PRIME_PRIME_PRIME_PRIME = exp1.payload[2]
                  local tmp61 = {84, accum_PRIME_PRIME}
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readTimePart(tmp61, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                end
                ::else6::
                if exp1.tag == "SOME" and exp1.payload[1] == 116 then
                else
                  goto else7
                end
                do
                  local strm_PRIME_PRIME_PRIME_PRIME = exp1.payload[2]
                  local tmp61 = {84, accum_PRIME_PRIME}
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readTimePart(tmp61, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                end
                ::else7::
                if exp1.tag == "SOME" and exp1.payload[1] == 32 then
                else
                  goto else8
                end
                do
                  do
                    local strm_PRIME_PRIME_PRIME_PRIME = exp1.payload[2]
                    local exp2
                    do
                      local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                      exp2 = tmp55(strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                    end
                    if exp2.tag == "SOME" then
                    else
                      goto else9
                    end
                    do
                      do
                        local c = exp2.payload[1]
                        if 48 <= c and c <= 57 then
                        else
                          goto else10
                        end
                        do
                          local tmp61 = {84, accum_PRIME_PRIME}
                          local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                          return readTimePart(tmp61, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                        end
                      end
                      ::else10::
                      local tmp61 = implodeRev(accum_PRIME_PRIME)
                      return {{tag = "DATE", payload = tmp61}, strm_PRIME_PRIME_PRIME}
                    end
                  end
                  ::else9::
                  local tmp61 = implodeRev(accum_PRIME_PRIME)
                  return {{tag = "DATE", payload = tmp61}, strm_PRIME_PRIME_PRIME}
                end
                ::else8::
                local tmp61 = implodeRev(accum_PRIME_PRIME)
                return {{tag = "DATE", payload = tmp61}, strm_PRIME_PRIME_PRIME}
              end
            end
            ::else1::
            do
              local tmp57 = revDigits ~= nil and (revDigits[2] ~= nil and revDigits[2][2] == nil)
              local tmp58
              tmp58 = tmp57 and (not hadUnderscore and (tmp56.tag == "SOME" and tmp56.payload[1] == 58))
              if tmp58 then
              else
                goto else2
              end
              do
                local tmp59 = revDigits[1]
                local tmp60 = revDigits[2][1]
                local strm_PRIME_PRIME = tmp56.payload[2]
                local tmp61 = _Int_mul(_Int_sub(tmp60, 48), 10)
                if _Int_add(tmp61, _Int_sub(tmp59, 48)) < 24 then
                else
                  _raise({tag = _L[186], payload = _L[184]}, "parse_toml.sml:652:45")
                end
                local tmp62 = {58, revDigits}
                local exp
                do
                  local strm1 = strm_PRIME_PRIME[1]
                  exp = readMinSec(tmp62, strm1, strm_PRIME_PRIME[2])
                end
                local accum = exp[1]
                local strm_PRIME_PRIME_PRIME = exp[2]
                local tmp63 = implodeRev(accum)
                return {{tag = "TIME", payload = tmp63}, strm_PRIME_PRIME_PRIME}
              end
            end
            ::else2::
            if tmp56.tag == "SOME" and tmp56.payload[1] == 46 then
            else
              goto else3
            end
            do
              local accum, strm_PRIME_PRIME_PRIME, exp
              do
                local strm_PRIME_PRIME = tmp56.payload[2]
                checkPrefixZero()
                local tmp57 = {46, revDigits}
                local exp1
                do
                  local strm1 = strm_PRIME_PRIME[1]
                  exp1 = readDecInt(tmp57, false, strm1, strm_PRIME_PRIME[2])
                end
                accum = exp1[1]
                strm_PRIME_PRIME_PRIME = exp1[3]
                do
                  local strm1 = strm_PRIME_PRIME_PRIME[1]
                  exp = tmp55(strm1, strm_PRIME_PRIME_PRIME[2])
                end
                if exp.tag == "SOME" and exp.payload[1] == 101 then
                else
                  goto else6
                end
                do
                  local strm_PRIME_PRIME_PRIME_PRIME = exp.payload[2]
                  local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                  return readExpPart(accum, 101, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
                end
              end
              ::else6::
              if exp.tag == "SOME" and exp.payload[1] == 69 then
              else
                goto else7
              end
              do
                local strm_PRIME_PRIME_PRIME_PRIME = exp.payload[2]
                local strm1 = strm_PRIME_PRIME_PRIME_PRIME[1]
                return readExpPart(accum, 69, strm1, strm_PRIME_PRIME_PRIME_PRIME[2])
              end
              ::else7::
              local tmp57 = implodeRev(accum)
              local tmp58 = _L[188](tmp57)
              return {tmp58, strm_PRIME_PRIME_PRIME}
            end
            ::else3::
            if tmp56.tag == "SOME" and tmp56.payload[1] == 101 then
            else
              goto else4
            end
            do
              local strm_PRIME_PRIME = tmp56.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(revDigits, 101, strm1, strm_PRIME_PRIME[2])
            end
            ::else4::
            if tmp56.tag == "SOME" and tmp56.payload[1] == 69 then
            else
              goto else5
            end
            do
              local strm_PRIME_PRIME = tmp56.payload[2]
              checkPrefixZero()
              local strm1 = strm_PRIME_PRIME[1]
              return readExpPart(revDigits, 69, strm1, strm_PRIME_PRIME[2])
            end
            ::else5::
            checkPrefixZero()
            local tmp57 = implodeRev(revDigits)
            local tmp58
            do
              local tmp59 = scanString(function(a)
                return scan(DEC, a)
              end)
              tmp58 = tmp59(tmp57)
            end
            if tmp58.tag == "SOME" then
              return {{tag = "INTEGER", payload = tmp58.payload}, strm_PRIME}
            elseif tmp58.tag == "NONE" then
              _raise(Option, "option.sml:24:18")
            else
              _raise(_Match, "option.sml:23:5")
            end
          end
        end
        local readKey, IMPLICIT_HEADER, finalizeTable, readKeyval, insertKeyval, insertTable
        do
          local isValidUnquotedKey = function(a)
            local tmp56
            do
              local tmp57
              if 65 <= a and a <= 90 then
                tmp56 = true
                goto cont
              else
                tmp57 = 97 <= a and a <= 122
              end
              tmp56 = tmp57 or 48 <= a and a <= 57
            end
            ::cont::
            return tmp56 or (a == 45 or a == 95)
          end
          local readSimpleKey = function(a)
            if a.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "key"}}}, "parse_toml.sml:16:5")
            end
            if a.tag == "SOME" and a.payload[1] == 34 then
            else
              goto else1
            end
            do
              local strm = a.payload[2]
              local strm1 = strm[1]
              return go(nil, strm1, strm[2])
            end
            ::else1::
            if a.tag == "SOME" and a.payload[1] == 39 then
            else
              goto else2
            end
            do
              local strm = a.payload[2]
              local strm1 = strm[1]
              return go2(nil, strm1, strm[2])
            end
            ::else2::
            if a.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:688:11")
            end
            do
              local c0
              do
                c0 = a.payload[1]
                local strm = a.payload[2]
                local tmp56 = isValidUnquotedKey(c0)
                if tmp56 then
                else
                  goto else3
                end
                do
                  local tmp57, tmp58 = {c0, nil}, strm
                  ::cont::
                  do
                    local accum, strm_PRIME = tmp57, tmp58
                    local exp
                    do
                      local strm1 = strm_PRIME[1]
                      exp = tmp55(strm1, strm_PRIME[2])
                    end
                    if exp.tag == "NONE" then
                    else
                      goto else4
                    end
                    do
                      local tmp59 = implodeRev(accum)
                      return {tmp59, strm_PRIME}
                    end
                    ::else4::
                    if exp.tag == "SOME" then
                    else
                      _raise(_Match, "parse_toml.sml:694:17")
                    end
                    do
                      local c = exp.payload[1]
                      local strm_PRIME_PRIME = exp.payload[2]
                      local tmp59 = isValidUnquotedKey(c)
                      if tmp59 then
                        tmp57 = {c, accum}
                        tmp58 = strm_PRIME_PRIME
                        goto cont
                      end
                      local tmp60 = implodeRev(accum)
                      return {tmp60, strm_PRIME}
                    end
                  end
                end
              end
              ::else3::
              local tmp56 = toString(c0)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp56, expected = "quote, alphanum, '-', or '_'"}}}, "parse_toml.sml:19:5")
            end
          end
          readKey = function(a)
            local tmp56, tmp57
            do
              local exp = readSimpleKey(a)
              local k0 = exp[1]
              local strm = exp[2]
              local tmp58 = {k0, nil}
              local tmp59 = skipWhiteSpace(strm)
              tmp57, tmp56 = tmp58, tmp59
            end
            ::cont::
            do
              local strm, accum
              do
                accum, strm = tmp57, tmp56
                local exp
                do
                  local strm1 = strm[1]
                  exp = tmp55(strm1, strm[2])
                end
                if exp.tag == "SOME" and exp.payload[1] == 46 then
                else
                  goto else1
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp58
                  do
                    local strm1 = strm_PRIME[1]
                    tmp58 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
                  end
                  local exp1 = readSimpleKey(tmp58)
                  local k = exp1[1]
                  local strm_PRIME_PRIME = exp1[2]
                  local tmp59 = {k, accum}
                  local tmp60 = skipWhiteSpace(strm_PRIME_PRIME)
                  tmp57 = tmp59
                  tmp56 = tmp60
                  goto cont
                end
              end
              ::else1::
              local tmp58 = revAppend(accum, nil)
              return {tmp58, strm}
            end
          end
          local EXACT_HEADER = "EXACT_HEADER"
          IMPLICIT_HEADER = "IMPLICIT_HEADER"
          local IMPLICIT_KEYVAL = "IMPLICIT_KEYVAL"
          local finalize, finalizeValue
          finalize = function(a)
            local tmp56 = finalizeTable(a)
            return {tag = "TABLE", payload = tmp56}
          end
          finalizeValue = function(a)
            if a.tag == "LEAF" then
              return a.payload
            end
            if a.tag == "PARTIAL_TABLE" then
            else
              goto else1
            end
            do
              return finalize(a.payload[2])
            end
            ::else1::
            if a.tag == "PARTIAL_ARRAY" then
            else
              _raise(_Match, "parse_toml.sml:733:11")
            end
            do
              local last = a.payload[1]
              local xs = a.payload[2]
              local tmp56 = map(finalize)
              local tmp57 = tmp56(xs)
              local tmp58 = finalize(last)
              local tmp59 = revAppend(tmp57, {tmp58, nil})
              return {tag = "ARRAY", payload = tmp59}
            end
          end
          finalizeTable = function(a)
            local tmp56 = map(function(a1)
              local key = a1[1]
              local tmp57 = finalizeValue(a1[2])
              return {key, tmp57}
            end)
            return tmp56(a)
          end
          local function insert(revPath, pt, tmp56, v)
            if tmp56 ~= nil and tmp56[2] == nil then
            else
              goto else1
            end
            do
              local tmp57
              do
                tmp57 = tmp56[1]
                local tmp58 = exists(function(a)
                  return tmp57 == a[1]
                end)
                local tmp59 = tmp58(pt)
                if tmp59 then
                else
                  goto else2
                end
                do
                  local tmp60 = revAppend(revPath, tmp56)
                  _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp60}}, "parse_toml.sml:22:5")
                end
              end
              ::else2::
              local tmp58 = {{tmp57, {tag = "LEAF", payload = v}}, nil}
              local tmp59 = revAppend(pt, nil)
              return revAppend(tmp59, tmp58)
            end
            ::else1::
            if tmp56 ~= nil then
            elseif tmp56 == nil then
              _raise(_Match, "parse_toml.sml:781:34")
            else
              _raise(_Match, "parse_toml.sml:741:11")
            end
            do
              local tmp57 = tmp56[1]
              local tmp58 = tmp56[2]
              local tmp59, tmp60 = nil, pt
              ::cont::
              do
                local accum, tmp61 = tmp59, tmp60
                if tmp61 == nil then
                else
                  goto else2
                end
                do
                  local tmp62 = insert({tmp57, revPath}, nil, tmp58, v)
                  return revAppend(accum, {{tmp57, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_KEYVAL, tmp62}}}, nil})
                end
                ::else2::
                if tmp61 ~= nil then
                else
                  _raise(_Match, "parse_toml.sml:748:19")
                end
                do
                  local tmp62 = tmp61[1]
                  local key_PRIME = tmp61[1][1]
                  local pv = tmp61[1][2]
                  local tmp63 = tmp61[2]
                  if tmp57 == key_PRIME then
                  else
                    tmp59 = {tmp62, accum}
                    tmp60 = tmp63
                    goto cont
                  end
                  do
                    if pv.tag == "LEAF" then
                    else
                      goto else3
                    end
                    do
                      local tmp64 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp64}}, "parse_toml.sml:22:5")
                    end
                    ::else3::
                    if pv.tag == "PARTIAL_TABLE" and pv.payload[1] == "EXACT_HEADER" then
                    else
                      goto else4
                    end
                    do
                      local tmp64 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp64}}, "parse_toml.sml:22:5")
                    end
                    ::else4::
                    if pv.tag == "PARTIAL_TABLE" then
                    else
                      goto else5
                    end
                    do
                      local definedBy = pv.payload[1]
                      local pt_PRIME = pv.payload[2]
                      local tmp64 = insert({tmp57, revPath}, pt_PRIME, tmp58, v)
                      return revAppend(accum, {{key_PRIME, {tag = "PARTIAL_TABLE", payload = {definedBy, tmp64}}}, tmp63})
                    end
                    ::else5::
                    if pv.tag == "PARTIAL_ARRAY" then
                    else
                      _raise(_Match, "parse_toml.sml:760:23")
                    end
                    do
                      local tmp64 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp64}}, "parse_toml.sml:22:5")
                    end
                  end
                end
              end
            end
          end
          local readInlineTable, readArray, readValue
          readKeyval = function(revPath, r)
            local exp = readKey(r)
            local key = exp[1]
            local strm_PRIME = exp[2]
            local tmp56 = revAppend(key, revPath)
            local tmp57
            do
              local strm = strm_PRIME[1]
              tmp57 = expect(61, strm, strm_PRIME[2])
            end
            local tmp58
            do
              local strm = tmp57[1]
              tmp58 = skipWhiteSpaceAndGetc(strm, tmp57[2])
            end
            local exp1 = readValue(tmp56, tmp58)
            local v = exp1[1]
            return {key, v, exp1[2]}
          end
          readInlineTable = function(revPath, accum, r)
            local tmp56, tmp57, tmp58 = revPath, accum, r
            ::cont::
            do
              local accum_PRIME, exp
              do
                local revPath1, accum1, r1 = tmp56, tmp57, tmp58
                local exp1 = readKeyval(revPath1, r1)
                local key = exp1[1]
                local v = exp1[2]
                local strm = exp1[3]
                accum_PRIME = insert(revPath1, accum1, key, v)
                do
                  local strm1 = strm[1]
                  exp = skipWhiteSpaceAndGetc(strm1, strm[2])
                end
                if exp.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "',' or '}'"}}}, "parse_toml.sml:16:5")
                end
                if exp.tag == "SOME" and exp.payload[1] == 44 then
                else
                  goto else1
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp59
                  do
                    local strm1 = strm_PRIME[1]
                    tmp59 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
                  end
                  tmp56 = revPath1
                  tmp57 = accum_PRIME
                  tmp58 = tmp59
                  goto cont
                end
              end
              ::else1::
              if exp.tag == "SOME" and exp.payload[1] == 125 then
              else
                goto else2
              end
              do
                local strm_PRIME = exp.payload[2]
                local tmp59 = finalize(accum_PRIME)
                return {tmp59, strm_PRIME}
              end
              ::else2::
              if exp.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:922:11")
              end
              do
                local tmp59 = toString(exp.payload[1])
                _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp59, expected = "',' or '}'"}}}, "parse_toml.sml:19:5")
              end
            end
          end
          readArray = function(revPath, accum, strm)
            local tmp56, tmp57, tmp58 = revPath, accum, strm
            ::cont::
            do
              local accum1, revPath1, exp
              do
                local strm1
                revPath1, accum1, strm1 = tmp56, tmp57, tmp58
                exp = skipWhiteSpaceOrCommentOrNewlineAndGetc(strm1)
                if exp.tag == "SOME" and exp.payload[1] == 93 then
                else
                  goto else1
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp59 = revAppend(accum1, nil)
                  return {{tag = "ARRAY", payload = tmp59}, strm_PRIME}
                end
              end
              ::else1::
              local exp1
              do
                local exp2 = readValue(revPath1, exp)
                local v = exp2[1]
                exp1 = skipWhiteSpaceOrCommentOrNewlineAndGetc(exp2[2])
                if exp1.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "',' or ']'"}}}, "parse_toml.sml:16:5")
                end
                if exp1.tag == "SOME" and exp1.payload[1] == 44 then
                  local strm_PRIME_PRIME = exp1.payload[2]
                  tmp56 = revPath1
                  tmp57 = {v, accum1}
                  tmp58 = strm_PRIME_PRIME
                  goto cont
                end
                if exp1.tag == "SOME" and exp1.payload[1] == 93 then
                else
                  goto else2
                end
                do
                  local strm_PRIME_PRIME = exp1.payload[2]
                  local tmp59 = revAppend({v, accum1}, nil)
                  return {{tag = "ARRAY", payload = tmp59}, strm_PRIME_PRIME}
                end
              end
              ::else2::
              if exp1.tag == "SOME" then
              else
                _raise(_Match, "parse_toml.sml:910:16")
              end
              do
                local tmp59 = toString(exp1.payload[1])
                _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp59, expected = "',' or ']'"}}}, "parse_toml.sml:19:5")
              end
            end
          end
          readValue = function(revPath, tmp56)
            if tmp56.tag == "NONE" then
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "value"}}}, "parse_toml.sml:16:5")
            end
            if tmp56.tag == "SOME" then
            else
              _raise(_Match, "parse_toml.sml:788:11")
            end
            do
              local c = tmp56.payload[1]
              local strm = tmp56.payload[2]
              if c == 34 then
              else
                goto else1
              end
              do
                do
                  local exp
                  do
                    local strm1 = strm[1]
                    exp = tmp55(strm1, strm[2])
                  end
                  if exp.tag == "NONE" then
                    _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, "parse_toml.sml:16:5")
                  end
                  if exp.tag == "SOME" and exp.payload[1] == 34 then
                  else
                    goto else13
                  end
                  do
                    local strm_PRIME = exp.payload[2]
                    local exp1
                    do
                      local strm1 = strm_PRIME[1]
                      exp1 = tmp55(strm1, strm_PRIME[2])
                    end
                    if exp1.tag == "SOME" and exp1.payload[1] == 34 then
                    else
                      return {{tag = "STRING", payload = ""}, strm_PRIME}
                    end
                    do
                      local tmp57 = skipOptionalNewline(exp1.payload[2])
                      local exp2
                      do
                        local strm1 = tmp57[1]
                        exp2 = go1(nil, strm1, tmp57[2])
                      end
                      local s = exp2[1]
                      local strm_PRIME_PRIME_PRIME = exp2[2]
                      return {{tag = "STRING", payload = s}, strm_PRIME_PRIME_PRIME}
                    end
                  end
                end
                ::else13::
                local exp
                do
                  local strm1 = strm[1]
                  exp = go(nil, strm1, strm[2])
                end
                local s = exp[1]
                local strm_PRIME = exp[2]
                return {{tag = "STRING", payload = s}, strm_PRIME}
              end
              ::else1::
              if c == 39 then
              else
                goto else2
              end
              do
                do
                  local exp
                  do
                    local strm1 = strm[1]
                    exp = tmp55(strm1, strm[2])
                  end
                  if exp.tag == "NONE" then
                    _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing quote"}}}, "parse_toml.sml:16:5")
                  end
                  if exp.tag == "SOME" and exp.payload[1] == 39 then
                  else
                    goto else13
                  end
                  do
                    local strm_PRIME = exp.payload[2]
                    local exp1
                    do
                      local strm1 = strm_PRIME[1]
                      exp1 = tmp55(strm1, strm_PRIME[2])
                    end
                    if exp1.tag == "SOME" and exp1.payload[1] == 39 then
                    else
                      return {{tag = "STRING", payload = ""}, strm_PRIME}
                    end
                    do
                      local strm_PRIME_PRIME_PRIME, s
                      do
                        local tmp57 = skipOptionalNewline(exp1.payload[2])
                        do
                          local tmp58, tmp59, tmp60
                          do
                            local strm1 = tmp57[1]
                            tmp58 = nil
                            tmp59 = strm1
                            tmp60 = tmp57[2]
                          end
                          ::cont1::
                          do
                            local accum, exp2
                            do
                              local strm1, state
                              accum, strm1, state = tmp58, tmp59, tmp60
                              exp2 = tmp55(strm1, state)
                              if exp2.tag == "NONE" then
                                _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, "parse_toml.sml:16:5")
                              end
                              if exp2.tag == "SOME" and exp2.payload[1] == 39 then
                              else
                                goto else14
                              end
                              do
                                local exp3
                                do
                                  local strm_PRIME1 = exp2.payload[2]
                                  do
                                    local strm2 = strm_PRIME1[1]
                                    exp3 = tmp55(strm2, strm_PRIME1[2])
                                  end
                                  if exp3.tag == "SOME" and exp3.payload[1] == 39 then
                                  else
                                    goto else15
                                  end
                                  do
                                    local exp4
                                    do
                                      local strm_PRIME_PRIME = exp3.payload[2]
                                      do
                                        local strm2 = strm_PRIME_PRIME[1]
                                        exp4 = tmp55(strm2, strm_PRIME_PRIME[2])
                                      end
                                      if exp4.tag == "SOME" and exp4.payload[1] == 39 then
                                      else
                                        goto else16
                                      end
                                      do
                                        local strm_PRIME_PRIME_PRIME1
                                        do
                                          strm_PRIME_PRIME_PRIME1 = exp4.payload[2]
                                          local exp5
                                          do
                                            local strm2 = strm_PRIME_PRIME_PRIME1[1]
                                            exp5 = tmp55(strm2, strm_PRIME_PRIME_PRIME1[2])
                                          end
                                          if exp5.tag == "SOME" and exp5.payload[1] == 39 then
                                          else
                                            goto else17
                                          end
                                          do
                                            local strm_PRIME_PRIME_PRIME_PRIME
                                            do
                                              strm_PRIME_PRIME_PRIME_PRIME = exp5.payload[2]
                                              local exp6
                                              do
                                                local strm2 = strm_PRIME_PRIME_PRIME_PRIME[1]
                                                exp6 = tmp55(strm2, strm_PRIME_PRIME_PRIME_PRIME[2])
                                              end
                                              if exp6.tag == "SOME" and exp6.payload[1] == 39 then
                                              else
                                                goto else18
                                              end
                                              do
                                                local strm_PRIME_PRIME_PRIME_PRIME_PRIME = exp6.payload[2]
                                                local tmp61 = implodeRev({39, {39, accum}})
                                                strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME_PRIME_PRIME_PRIME
                                                s = tmp61
                                                goto cont
                                              end
                                            end
                                            ::else18::
                                            local tmp61 = implodeRev({39, accum})
                                            strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME_PRIME_PRIME
                                            s = tmp61
                                            goto cont
                                          end
                                        end
                                        ::else17::
                                        local tmp61 = implodeRev(accum)
                                        strm_PRIME_PRIME_PRIME = strm_PRIME_PRIME_PRIME1
                                        s = tmp61
                                        goto cont
                                      end
                                    end
                                    ::else16::
                                    if exp4.tag == "SOME" then
                                    elseif exp4.tag == "NONE" then
                                      _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, "parse_toml.sml:16:5")
                                    else
                                      _raise(_Match, "parse_toml.sml:267:21")
                                    end
                                    do
                                      local c1 = exp4.payload[1]
                                      local strm_PRIME_PRIME_PRIME1 = exp4.payload[2]
                                      local tmp61 = checkAllowedChar(c1)
                                      local tmp62 = {tmp61, {39, {39, accum}}}
                                      local strm2 = strm_PRIME_PRIME_PRIME1[1]
                                      tmp58 = tmp62
                                      tmp59 = strm2
                                      tmp60 = strm_PRIME_PRIME_PRIME1[2]
                                      goto cont1
                                    end
                                  end
                                end
                                ::else15::
                                if exp3.tag == "SOME" then
                                elseif exp3.tag == "NONE" then
                                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "closing triple quote"}}}, "parse_toml.sml:16:5")
                                else
                                  _raise(_Match, "parse_toml.sml:265:16")
                                end
                                do
                                  local c1 = exp3.payload[1]
                                  local strm_PRIME_PRIME = exp3.payload[2]
                                  local tmp61 = checkAllowedChar(c1)
                                  local tmp62 = {tmp61, {39, accum}}
                                  local strm2 = strm_PRIME_PRIME[1]
                                  tmp58 = tmp62
                                  tmp59 = strm2
                                  tmp60 = strm_PRIME_PRIME[2]
                                  goto cont1
                                end
                              end
                            end
                            ::else14::
                            if exp2.tag == "SOME" then
                            else
                              _raise(_Match, "parse_toml.sml:262:11")
                            end
                            do
                              local c1 = exp2.payload[1]
                              local strm_PRIME1 = exp2.payload[2]
                              local tmp61 = checkAllowedChar(c1)
                              local tmp62 = {tmp61, accum}
                              local strm1 = strm_PRIME1[1]
                              tmp58 = tmp62
                              tmp59 = strm1
                              tmp60 = strm_PRIME1[2]
                              goto cont1
                            end
                          end
                        end
                      end
                      ::cont::
                      return {{tag = "STRING", payload = s}, strm_PRIME_PRIME_PRIME}
                    end
                  end
                end
                ::else13::
                local exp
                do
                  local strm1 = strm[1]
                  exp = go2(nil, strm1, strm[2])
                end
                local s = exp[1]
                local strm_PRIME = exp[2]
                return {{tag = "STRING", payload = s}, strm_PRIME}
              end
              ::else2::
              if c == 91 then
              else
                goto else3
              end
              do
                return readArray({"[]", revPath}, nil, strm)
              end
              ::else3::
              if c == 123 then
              else
                goto else4
              end
              do
                local exp
                do
                  local strm1 = strm[1]
                  exp = skipWhiteSpaceAndGetc(strm1, strm[2])
                end
                if exp.tag == "SOME" and exp.payload[1] == 125 then
                  local strm_PRIME = exp.payload[2]
                  return {{tag = "TABLE", payload = nil}, strm_PRIME}
                else
                  return readInlineTable(revPath, nil, exp)
                end
              end
              ::else4::
              if c == 116 then
              else
                goto else5
              end
              do
                local tmp57 = {tag = "BOOL", payload = true}
                local tmp58
                do
                  local strm1 = strm[1]
                  tmp58 = expect(114, strm1, strm[2])
                end
                local tmp59
                do
                  local strm1 = tmp58[1]
                  tmp59 = expect(117, strm1, tmp58[2])
                end
                local tmp60
                do
                  local strm1 = tmp59[1]
                  tmp60 = expect(101, strm1, tmp59[2])
                end
                return {tmp57, tmp60}
              end
              ::else5::
              if c == 102 then
              else
                goto else6
              end
              do
                local tmp57 = {tag = "BOOL", payload = false}
                local tmp58
                do
                  local strm1 = strm[1]
                  tmp58 = expect(97, strm1, strm[2])
                end
                local tmp59
                do
                  local strm1 = tmp58[1]
                  tmp59 = expect(108, strm1, tmp58[2])
                end
                local tmp60
                do
                  local strm1 = tmp59[1]
                  tmp60 = expect(115, strm1, tmp59[2])
                end
                local tmp61
                do
                  local strm1 = tmp60[1]
                  tmp61 = expect(101, strm1, tmp60[2])
                end
                return {tmp57, tmp61}
              end
              ::else6::
              if c == 105 then
              else
                goto else7
              end
              do
                local tmp57 = _L[188]("inf")
                local tmp58
                do
                  local strm1 = strm[1]
                  tmp58 = expect(110, strm1, strm[2])
                end
                local tmp59
                do
                  local strm1 = tmp58[1]
                  tmp59 = expect(102, strm1, tmp58[2])
                end
                return {tmp57, tmp59}
              end
              ::else7::
              if c == 110 then
              else
                goto else8
              end
              do
                local tmp57 = _L[188]("nan")
                local tmp58
                do
                  local strm1 = strm[1]
                  tmp58 = expect(97, strm1, strm[2])
                end
                local tmp59
                do
                  local strm1 = tmp58[1]
                  tmp59 = expect(110, strm1, tmp58[2])
                end
                return {tmp57, tmp59}
              end
              ::else8::
              if c == 43 then
              else
                goto else9
              end
              do
                local exp
                do
                  local strm1 = strm[1]
                  exp = tmp55(strm1, strm[2])
                end
                if exp.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit or 'inf' or 'nan'"}}}, "parse_toml.sml:16:5")
                end
                if exp.tag == "SOME" and exp.payload[1] == 105 then
                else
                  goto else13
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp57 = _L[188]("+inf")
                  local tmp58
                  do
                    local strm1 = strm_PRIME[1]
                    tmp58 = expect(110, strm1, strm_PRIME[2])
                  end
                  local tmp59
                  do
                    local strm1 = tmp58[1]
                    tmp59 = expect(102, strm1, tmp58[2])
                  end
                  return {tmp57, tmp59}
                end
                ::else13::
                if exp.tag == "SOME" and exp.payload[1] == 110 then
                else
                  goto else14
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp57 = _L[188]("+nan")
                  local tmp58
                  do
                    local strm1 = strm_PRIME[1]
                    tmp58 = expect(97, strm1, strm_PRIME[2])
                  end
                  local tmp59
                  do
                    local strm1 = tmp58[1]
                    tmp59 = expect(110, strm1, tmp58[2])
                  end
                  return {tmp57, tmp59}
                end
                ::else14::
                if exp.tag == "SOME" then
                else
                  _raise(_Match, "parse_toml.sml:845:18")
                end
                do
                  local c_PRIME
                  do
                    c_PRIME = exp.payload[1]
                    local strm_PRIME = exp.payload[2]
                    if 48 <= c_PRIME and c_PRIME <= 57 then
                    else
                      goto else15
                    end
                    do
                      return readSigned(43, c_PRIME, strm_PRIME)
                    end
                  end
                  ::else15::
                  local tmp57 = toString(c_PRIME)
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp57, expected = "digit or 'inf' or 'nan'"}}}, "parse_toml.sml:19:5")
                end
              end
              ::else9::
              if c == 45 then
              else
                goto else10
              end
              do
                local exp
                do
                  local strm1 = strm[1]
                  exp = tmp55(strm1, strm[2])
                end
                if exp.tag == "NONE" then
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = "end of input", expected = "digit or 'inf' or 'nan'"}}}, "parse_toml.sml:16:5")
                end
                if exp.tag == "SOME" and exp.payload[1] == 105 then
                else
                  goto else13
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp57 = _L[188]("-inf")
                  local tmp58
                  do
                    local strm1 = strm_PRIME[1]
                    tmp58 = expect(110, strm1, strm_PRIME[2])
                  end
                  local tmp59
                  do
                    local strm1 = tmp58[1]
                    tmp59 = expect(102, strm1, tmp58[2])
                  end
                  return {tmp57, tmp59}
                end
                ::else13::
                if exp.tag == "SOME" and exp.payload[1] == 110 then
                else
                  goto else14
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp57 = _L[188]("-nan")
                  local tmp58
                  do
                    local strm1 = strm_PRIME[1]
                    tmp58 = expect(97, strm1, strm_PRIME[2])
                  end
                  local tmp59
                  do
                    local strm1 = tmp58[1]
                    tmp59 = expect(110, strm1, tmp58[2])
                  end
                  return {tmp57, tmp59}
                end
                ::else14::
                if exp.tag == "SOME" then
                else
                  _raise(_Match, "parse_toml.sml:857:18")
                end
                do
                  local c_PRIME
                  do
                    c_PRIME = exp.payload[1]
                    local strm_PRIME = exp.payload[2]
                    if 48 <= c_PRIME and c_PRIME <= 57 then
                    else
                      goto else15
                    end
                    do
                      return readSigned(45, c_PRIME, strm_PRIME)
                    end
                  end
                  ::else15::
                  local tmp57 = toString(c_PRIME)
                  _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp57, expected = "digit or 'inf' or 'nan'"}}}, "parse_toml.sml:19:5")
                end
              end
              ::else10::
              if c == 48 then
              else
                goto else11
              end
              do
                local exp
                do
                  local strm1 = strm[1]
                  exp = tmp55(strm1, strm[2])
                end
                if exp.tag == "NONE" then
                else
                  goto else13
                end
                do
                  local tmp57 = _L[22](0)
                  return {{tag = "INTEGER", payload = tmp57}, strm}
                end
                ::else13::
                if exp.tag == "SOME" and exp.payload[1] == 120 then
                else
                  goto else14
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp57 = _L[22](0)
                  local exp1
                  do
                    local strm1 = strm_PRIME[1]
                    exp1 = readHexInt(tmp57, strm1, strm_PRIME[2])
                  end
                  local x = exp1[1]
                  local strm_PRIME_PRIME = exp1[2]
                  return {{tag = "INTEGER", payload = x}, strm_PRIME_PRIME}
                end
                ::else14::
                if exp.tag == "SOME" and exp.payload[1] == 111 then
                else
                  goto else15
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp57 = _L[22](0)
                  local exp1
                  do
                    local strm1 = strm_PRIME[1]
                    exp1 = readOctInt(tmp57, strm1, strm_PRIME[2])
                  end
                  local x = exp1[1]
                  local strm_PRIME_PRIME = exp1[2]
                  return {{tag = "INTEGER", payload = x}, strm_PRIME_PRIME}
                end
                ::else15::
                if exp.tag == "SOME" and exp.payload[1] == 98 then
                else
                  if exp.tag == "SOME" then
                  else
                    _raise(_Match, "parse_toml.sml:869:18")
                  end
                  do
                    return readUnsigned(48, strm)
                  end
                end
                do
                  local strm_PRIME = exp.payload[2]
                  local tmp57 = _L[22](0)
                  local exp1
                  do
                    local strm1 = strm_PRIME[1]
                    exp1 = readBinInt(tmp57, strm1, strm_PRIME[2])
                  end
                  local x = exp1[1]
                  local strm_PRIME_PRIME = exp1[2]
                  return {{tag = "INTEGER", payload = x}, strm_PRIME_PRIME}
                end
              end
              ::else11::
              if 48 <= c and c <= 57 then
              else
                goto else12
              end
              do
                return readUnsigned(c, strm)
              end
              ::else12::
              local tmp57 = toString(c)
              _raise({tag = _L[186], payload = {tag = "UNEXPECTED", payload = {encountered = tmp57, expected = "value"}}}, "parse_toml.sml:19:5")
            end
          end
          insertKeyval = function(revPath, pt, tmp56, keys, v)
            if tmp56 == nil then
            else
              goto else1
            end
            do
              return insert(revPath, pt, keys, v)
            end
            ::else1::
            if tmp56 ~= nil then
            else
              _raise(_Match, "parse_toml.sml:972:11")
            end
            do
              local tmp57 = tmp56[1]
              local tmp58 = tmp56[2]
              local tmp59, tmp60 = nil, pt
              ::cont::
              do
                local accum, tmp61 = tmp59, tmp60
                if tmp61 == nil then
                else
                  goto else2
                end
                do
                  local tmp62 = insertKeyval({tmp57, revPath}, nil, tmp58, keys, v)
                  return revAppend(accum, {{tmp57, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_HEADER, tmp62}}}, nil})
                end
                ::else2::
                if tmp61 ~= nil then
                else
                  _raise(_Match, "parse_toml.sml:976:19")
                end
                do
                  local tmp62 = tmp61[1]
                  local key = tmp61[1][1]
                  local pv = tmp61[1][2]
                  local tmp63 = tmp61[2]
                  if tmp57 == key then
                  else
                    tmp59 = {tmp62, accum}
                    tmp60 = tmp63
                    goto cont
                  end
                  do
                    local updated
                    do
                      if pv.tag == "LEAF" then
                      else
                        goto else3
                      end
                      do
                        local tmp64 = revAppend(revPath, {key, nil})
                        _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp64}}, "parse_toml.sml:22:5")
                      end
                      ::else3::
                      if pv.tag == "PARTIAL_TABLE" then
                      else
                        goto else4
                      end
                      do
                        local definedBy = pv.payload[1]
                        local pt_PRIME = pv.payload[2]
                        local tmp64 = insertKeyval({tmp57, revPath}, pt_PRIME, tmp58, keys, v)
                        updated = {tag = "PARTIAL_TABLE", payload = {definedBy, tmp64}}
                        goto cont1
                      end
                      ::else4::
                      if pv.tag == "PARTIAL_ARRAY" then
                      else
                        _raise(_Match, "parse_toml.sml:988:27")
                      end
                      do
                        local last = pv.payload[1]
                        local rest = pv.payload[2]
                        local tmp64 = insertKeyval({tmp57, revPath}, last, tmp58, keys, v)
                        updated = {tag = "PARTIAL_ARRAY", payload = {tmp64, rest}}
                      end
                    end
                    ::cont1::
                    return revAppend(accum, {{key, updated}, tmp63})
                  end
                end
              end
            end
          end
          insertTable = function(revPath, pt, tmp56)
            if tmp56 ~= nil and tmp56[2] == nil then
            else
              goto else1
            end
            do
              local tmp57 = tmp56[1]
              local tmp58, tmp59 = nil, pt
              ::cont::
              do
                local accum, tmp60 = tmp58, tmp59
                if tmp60 == nil then
                else
                  goto else2
                end
                do
                  return revAppend(accum, {{tmp57, {tag = "PARTIAL_TABLE", payload = {EXACT_HEADER, nil}}}, nil})
                end
                ::else2::
                if tmp60 ~= nil then
                else
                  _raise(_Match, "parse_toml.sml:1010:19")
                end
                do
                  local tmp61 = tmp60[1]
                  local key_PRIME = tmp60[1][1]
                  local pv = tmp60[1][2]
                  local tmp62 = tmp60[2]
                  if tmp57 == key_PRIME then
                  else
                    tmp58 = {tmp61, accum}
                    tmp59 = tmp62
                    goto cont
                  end
                  do
                    if pv.tag == "LEAF" then
                    else
                      goto else3
                    end
                    do
                      local tmp63 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp63}}, "parse_toml.sml:22:5")
                    end
                    ::else3::
                    if pv.tag == "PARTIAL_TABLE" and pv.payload[1] == "IMPLICIT_HEADER" then
                    else
                      goto else4
                    end
                    do
                      return revAppend(accum, {{key_PRIME, {tag = "PARTIAL_TABLE", payload = {EXACT_HEADER, pv.payload[2]}}}, tmp62})
                    end
                    ::else4::
                    if pv.tag == "PARTIAL_TABLE" then
                    else
                      goto else5
                    end
                    do
                      local tmp63 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp63}}, "parse_toml.sml:22:5")
                    end
                    ::else5::
                    if pv.tag == "PARTIAL_ARRAY" then
                    else
                      _raise(_Match, "parse_toml.sml:1017:23")
                    end
                    do
                      local tmp63 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp63}}, "parse_toml.sml:22:5")
                    end
                  end
                end
              end
            end
            ::else1::
            if tmp56 ~= nil then
            elseif tmp56 == nil then
              _raise(_Match, "parse_toml.sml:1069:36")
            else
              _raise(_Match, "parse_toml.sml:1008:11")
            end
            do
              local tmp57 = tmp56[1]
              local tmp58 = tmp56[2]
              local tmp59, tmp60 = nil, pt
              ::cont::
              do
                local accum, tmp61 = tmp59, tmp60
                if tmp61 == nil then
                else
                  goto else2
                end
                do
                  local tmp62 = insertTable({tmp57, revPath}, nil, tmp58)
                  return revAppend(accum, {{tmp57, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_HEADER, tmp62}}}, nil})
                end
                ::else2::
                if tmp61 ~= nil then
                else
                  _raise(_Match, "parse_toml.sml:1036:19")
                end
                do
                  local tmp62 = tmp61[1]
                  local key_PRIME = tmp61[1][1]
                  local pv = tmp61[1][2]
                  local tmp63 = tmp61[2]
                  if tmp57 == key_PRIME then
                  else
                    tmp59 = {tmp62, accum}
                    tmp60 = tmp63
                    goto cont
                  end
                  do
                    local updated
                    do
                      if pv.tag == "LEAF" then
                      else
                        goto else3
                      end
                      do
                        local tmp64 = revAppend(revPath, {tmp57, nil})
                        _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp64}}, "parse_toml.sml:22:5")
                      end
                      ::else3::
                      if pv.tag == "PARTIAL_TABLE" then
                      else
                        goto else4
                      end
                      do
                        local definedBy = pv.payload[1]
                        local pt_PRIME = pv.payload[2]
                        local tmp64 = insertTable({tmp57, revPath}, pt_PRIME, tmp58)
                        updated = {tag = "PARTIAL_TABLE", payload = {definedBy, tmp64}}
                        goto cont1
                      end
                      ::else4::
                      if pv.tag == "PARTIAL_ARRAY" then
                      else
                        _raise(_Match, "parse_toml.sml:1050:27")
                      end
                      do
                        local last = pv.payload[1]
                        local rest = pv.payload[2]
                        local tmp64 = insertTable({tmp57, revPath}, last, tmp58)
                        updated = {tag = "PARTIAL_ARRAY", payload = {tmp64, rest}}
                      end
                    end
                    ::cont1::
                    return revAppend(accum, {{key_PRIME, updated}, tmp63})
                  end
                end
              end
            end
          end
        end
        local table
        do
          local function insertArrayTable(revPath, pt, tmp56)
            if tmp56 ~= nil and tmp56[2] == nil then
            else
              goto else1
            end
            do
              local tmp57 = tmp56[1]
              local tmp58, tmp59 = nil, pt
              ::cont::
              do
                local accum, tmp60 = tmp58, tmp59
                if tmp60 == nil then
                else
                  goto else2
                end
                do
                  return revAppend(accum, {{tmp57, {tag = "PARTIAL_ARRAY", payload = {nil, nil}}}, nil})
                end
                ::else2::
                if tmp60 ~= nil then
                else
                  _raise(_Match, "parse_toml.sml:1072:19")
                end
                do
                  local tmp61 = tmp60[1]
                  local key_PRIME = tmp60[1][1]
                  local pv = tmp60[1][2]
                  local tmp62 = tmp60[2]
                  if tmp57 == key_PRIME then
                  else
                    tmp58 = {tmp61, accum}
                    tmp59 = tmp62
                    goto cont
                  end
                  do
                    if pv.tag == "LEAF" then
                    else
                      goto else3
                    end
                    do
                      local tmp63 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp63}}, "parse_toml.sml:22:5")
                    end
                    ::else3::
                    if pv.tag == "PARTIAL_TABLE" then
                    else
                      if pv.tag == "PARTIAL_ARRAY" then
                      else
                        _raise(_Match, "parse_toml.sml:1079:23")
                      end
                      do
                        local last = pv.payload[1]
                        return revAppend(accum, {{tmp57, {tag = "PARTIAL_ARRAY", payload = {nil, {last, pv.payload[2]}}}}, tmp62})
                      end
                    end
                    do
                      local tmp63 = revAppend(revPath, {tmp57, nil})
                      _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp63}}, "parse_toml.sml:22:5")
                    end
                  end
                end
              end
            end
            ::else1::
            if tmp56 ~= nil then
            elseif tmp56 == nil then
              _raise(_Match, "parse_toml.sml:1130:41")
            else
              _raise(_Match, "parse_toml.sml:1070:11")
            end
            do
              local tmp57 = tmp56[1]
              local tmp58 = tmp56[2]
              local tmp59, tmp60 = nil, pt
              ::cont::
              do
                local accum, tmp61 = tmp59, tmp60
                if tmp61 == nil then
                else
                  goto else2
                end
                do
                  local tmp62 = insertArrayTable({tmp57, revPath}, nil, tmp58)
                  return revAppend(accum, {{tmp57, {tag = "PARTIAL_TABLE", payload = {IMPLICIT_HEADER, tmp62}}}, nil})
                end
                ::else2::
                if tmp61 ~= nil then
                else
                  _raise(_Match, "parse_toml.sml:1095:19")
                end
                do
                  local tmp62 = tmp61[1]
                  local key_PRIME = tmp61[1][1]
                  local pv = tmp61[1][2]
                  local tmp63 = tmp61[2]
                  if tmp57 == key_PRIME then
                  else
                    tmp59 = {tmp62, accum}
                    tmp60 = tmp63
                    goto cont
                  end
                  do
                    local updated
                    do
                      if pv.tag == "LEAF" then
                      else
                        goto else3
                      end
                      do
                        local tmp64 = revAppend(revPath, {tmp57, nil})
                        _raise({tag = _L[186], payload = {tag = "DUPLICATE_KEY", payload = tmp64}}, "parse_toml.sml:22:5")
                      end
                      ::else3::
                      if pv.tag == "PARTIAL_TABLE" then
                      else
                        goto else4
                      end
                      do
                        local definedBy = pv.payload[1]
                        local pt_PRIME = pv.payload[2]
                        local tmp64 = insertArrayTable({tmp57, revPath}, pt_PRIME, tmp58)
                        updated = {tag = "PARTIAL_TABLE", payload = {definedBy, tmp64}}
                        goto cont1
                      end
                      ::else4::
                      if pv.tag == "PARTIAL_ARRAY" then
                      else
                        _raise(_Match, "parse_toml.sml:1109:27")
                      end
                      do
                        local last = pv.payload[1]
                        local rest = pv.payload[2]
                        local tmp64 = insertArrayTable({tmp57, revPath}, last, tmp58)
                        updated = {tag = "PARTIAL_ARRAY", payload = {tmp64, rest}}
                      end
                    end
                    ::cont1::
                    return revAppend(accum, {{tmp57, updated}, tmp63})
                  end
                end
              end
            end
          end
          do
            local tmp56, tmp57, tmp58, tmp59, tmp60 = nil, nil, nil, tmp54, _L[173]
            ::cont1::
            do
              local prefix, revPath, accum, exp
              do
                local strm, state
                accum, revPath, prefix, strm, state = tmp56, tmp57, tmp58, tmp59, tmp60
                do
                  local tmp61, tmp62, tmp63 = revPath, strm, state
                  ::cont3::
                  do
                    local revPath1, exp1
                    do
                      local strm1, state1
                      revPath1, strm1, state1 = tmp61, tmp62, tmp63
                      exp1 = skipWhiteSpaceAndGetc(strm1, state1)
                      if exp1.tag == "NONE" then
                        exp = NONE
                        goto cont2
                      end
                      if exp1.tag == "SOME" and exp1.payload[1] == 10 then
                        local strm_PRIME = exp1.payload[2]
                        local strm2 = strm_PRIME[1]
                        tmp61 = revPath1
                        tmp62 = strm2
                        tmp63 = strm_PRIME[2]
                        goto cont3
                      end
                      if exp1.tag == "SOME" and exp1.payload[1] == 13 then
                      else
                        goto else4
                      end
                      do
                        local strm_PRIME = exp1.payload[2]
                        local tmp64
                        do
                          local strm2 = strm_PRIME[1]
                          tmp64 = expect(10, strm2, strm_PRIME[2])
                        end
                        local strm2 = tmp64[1]
                        tmp61 = revPath1
                        tmp62 = strm2
                        tmp63 = tmp64[2]
                        goto cont3
                      end
                    end
                    ::else4::
                    if exp1.tag == "SOME" and exp1.payload[1] == 35 then
                    else
                      goto else5
                    end
                    do
                      local tmp64 = skipUntilNewline(exp1.payload[2])
                      local strm1 = tmp64[1]
                      tmp61 = revPath1
                      tmp62 = strm1
                      tmp63 = tmp64[2]
                      goto cont3
                    end
                    ::else5::
                    if exp1.tag == "SOME" and exp1.payload[1] == 91 then
                    else
                      goto else6
                    end
                    do
                      local strm_PRIME
                      do
                        strm_PRIME = exp1.payload[2]
                        local exp2
                        do
                          local strm1 = strm_PRIME[1]
                          exp2 = tmp55(strm1, strm_PRIME[2])
                        end
                        if exp2.tag == "SOME" and exp2.payload[1] == 91 then
                        else
                          goto else7
                        end
                        do
                          local strm_PRIME_PRIME = exp2.payload[2]
                          local tmp64
                          do
                            local strm1 = strm_PRIME_PRIME[1]
                            tmp64 = skipWhiteSpaceAndGetc(strm1, strm_PRIME_PRIME[2])
                          end
                          local exp3 = readKey(tmp64)
                          local key = exp3[1]
                          local strm_PRIME_PRIME_PRIME = exp3[2]
                          local tmp65
                          do
                            local strm1 = strm_PRIME_PRIME_PRIME[1]
                            tmp65 = expect(93, strm1, strm_PRIME_PRIME_PRIME[2])
                          end
                          local strm_PRIME_PRIME_PRIME_PRIME
                          do
                            local strm1 = tmp65[1]
                            strm_PRIME_PRIME_PRIME_PRIME = expect(93, strm1, tmp65[2])
                          end
                          local tmp66 = {tag = "ARRAY_TABLE", payload = key}
                          local tmp67 = skipWhiteSpaceOrComment(strm_PRIME_PRIME_PRIME_PRIME)
                          exp = {tag = "SOME", payload = {tmp66, tmp67}}
                          goto cont2
                        end
                      end
                      ::else7::
                      local tmp64
                      do
                        local strm1 = strm_PRIME[1]
                        tmp64 = skipWhiteSpaceAndGetc(strm1, strm_PRIME[2])
                      end
                      local exp2 = readKey(tmp64)
                      local key = exp2[1]
                      local strm_PRIME_PRIME = exp2[2]
                      local strm_PRIME_PRIME_PRIME
                      do
                        local strm1 = strm_PRIME_PRIME[1]
                        strm_PRIME_PRIME_PRIME = expect(93, strm1, strm_PRIME_PRIME[2])
                      end
                      local tmp65 = {tag = "STD_TABLE", payload = key}
                      local tmp66 = skipWhiteSpaceOrComment(strm_PRIME_PRIME_PRIME)
                      exp = {tag = "SOME", payload = {tmp65, tmp66}}
                      goto cont2
                    end
                    ::else6::
                    local exp2 = readKeyval(revPath1, exp1)
                    local key = exp2[1]
                    local v = exp2[2]
                    local strm_PRIME = exp2[3]
                    local tmp64 = {tag = "KEYVAL", payload = {key, v}}
                    local tmp65 = skipWhiteSpaceOrComment(strm_PRIME)
                    exp = {tag = "SOME", payload = {tmp64, tmp65}}
                  end
                end
              end
              ::cont2::
              if exp.tag == "NONE" then
              else
                goto else1
              end
              table = finalizeTable(accum)
              goto cont
              ::else1::
              if exp.tag == "SOME" and exp.payload[1].tag == "ARRAY_TABLE" then
              else
                goto else2
              end
              do
                local key = exp.payload[1].payload
                local strm_PRIME = exp.payload[2]
                local tmp61 = insertArrayTable(nil, accum, key)
                local tmp62 = revAppend(key, nil)
                local tmp63 = {"[]", tmp62}
                local strm = strm_PRIME[1]
                tmp56 = tmp61
                tmp57 = tmp63
                tmp58 = key
                tmp59 = strm
                tmp60 = strm_PRIME[2]
                goto cont1
              end
              ::else2::
              if exp.tag == "SOME" and exp.payload[1].tag == "STD_TABLE" then
              else
                goto else3
              end
              do
                local key = exp.payload[1].payload
                local strm_PRIME = exp.payload[2]
                local tmp61 = insertTable(nil, accum, key)
                local tmp62 = revAppend(key, nil)
                local strm = strm_PRIME[1]
                tmp56 = tmp61
                tmp57 = tmp62
                tmp58 = key
                tmp59 = strm
                tmp60 = strm_PRIME[2]
                goto cont1
              end
              ::else3::
              if exp.tag == "SOME" and exp.payload[1].tag == "KEYVAL" then
              else
                _raise(_Match, "parse_toml.sml:1132:9")
              end
              do
                local key = exp.payload[1].payload[1]
                local v = exp.payload[1].payload[2]
                local strm_PRIME = exp.payload[2]
                local tmp61 = insertKeyval(nil, accum, prefix, key, v)
                local strm = strm_PRIME[1]
                tmp56 = tmp61
                tmp57 = revPath
                tmp58 = prefix
                tmp59 = strm
                tmp60 = strm_PRIME[2]
                goto cont1
              end
            end
          end
        end
        ::cont::
        local tmp56, color, tmp57, tmp58, tmp59, tmp60
        do
          local tmp61 = _L[364](table, "temporary-directory")
          tmp56 = _L[366](tmp61, function(a)
            if a.tag == "STRING" then
              return {tag = "SOME", payload = a.payload}
            end
            _L[302]("Config entry temporary-directory should be a string.")
            return NONE
          end)
          local tmp62 = _L[364](table, "color")
          color = _L[366](tmp62, function(a)
            if a.tag == "TABLE" then
              return {tag = "SOME", payload = a.payload}
            end
            _L[302]("Config entry color should be a table.")
            return NONE
          end)
          local tmp63 = _L[365](color, "type")
          local tmp64 = _L[369]("color.type")
          tmp57 = _L[366](tmp63, tmp64)
          local tmp65 = _L[365](color, "execute")
          local tmp66 = _L[369]("color.execute")
          tmp58 = _L[366](tmp65, tmp66)
          local tmp67 = _L[365](color, "error")
          local tmp68 = _L[369]("color.error")
          tmp59 = _L[366](tmp67, tmp68)
          local tmp69 = _L[365](color, "warning")
          local tmp70 = _L[369]("color.warning")
          tmp60 = _L[366](tmp69, tmp70)
        end
        local tmp61 = _L[365](color, "diagnostic")
        local tmp62 = _L[369]("color.diagnostic")
        local tmp63 = _L[366](tmp61, tmp62)
        local tmp64 = _L[365](color, "information")
        local tmp65 = _L[369]("color.information")
        local tmp66 = _L[366](tmp64, tmp65)
        return {color = {diagnostic = tmp63, error = tmp59, execute = tmp58, information = tmp66, type_ = tmp57, warning = tmp60}, temporary_directory = tmp56}
      end)
      if not _L[552] then
        if __exn_instanceof(_L[553], _L[9]) then
          _L[541] = _L[363]
          goto cont6
        end
        if __exn_instanceof(_L[553], _L[171]) then
        else
          goto else2
        end
        _L[301]("Config file " .. _L[551] .. " is not UTF-8 encoded.")
        _L[541] = _L[363]
        goto cont6
        ::else2::
        if __exn_instanceof(_L[553], _L[186]) then
        else
          _raise(_L[553], nil)
        end
        do
          _L[554] = _L[553].payload
          _L[555] = "Config file " .. _L[551] .. " is not a valid TOML file: "
          do
            if _L[554].tag == "UNEXPECTED" then
              _L[557] = _L[554].payload.encountered
              _L[558] = _L[554].payload.expected
              _L[556] = "unexpected " .. _L[557] .. ", expected " .. _L[558]
              goto cont19
            end
            if _L[554].tag == "PREFIX_ZERO" then
              _L[556] = "prefix 0 is disallowed"
              goto cont19
            end
            if _L[554].tag == "INVALID_UNICODE_SCALAR" then
              _L[556] = "invalid Unicode scalar"
              goto cont19
            end
            if _L[554].tag == "INVALID_DATE" then
              _L[556] = "invalid date"
              goto cont19
            end
            if _L[554].tag == "INVALID_TIME" then
              _L[556] = "invalid time"
              goto cont19
            end
            if _L[554].tag == "DUPLICATE_KEY" then
            else
              _raise(_Match, "error.sml:24:9")
            end
            do
              _L[559] = _L[554].payload
              _L[560] = map(_L[185])
              _L[561] = _L[560](_L[559])
              _L[556] = "duplicate key at " .. table_concat(_VectorOrArray_fromList(_L[561]), ".")
            end
          end
          ::cont19::
          _L[301](_L[555] .. _L[556])
          _L[541] = _L[363]
        end
      else
        _L[541] = _L[553]
      end
    end
  end
  ::cont6::
  _L[562] = app(_L[294])
  _L[562](_L[541].color.type_)
  _L[563] = app(_L[295])
  _L[563](_L[541].color.execute)
  _L[564] = app(_L[296])
  _L[564](_L[541].color.error)
  _L[565] = app(_L[297])
  _L[565](_L[541].color.warning)
  _L[566] = app(_L[298])
  _L[566](_L[541].color.diagnostic)
  _L[567] = app(_L[299])
  _L[567](_L[541].color.information)
  _L[568] = _L[538].watch
  do
    if _L[538].color.tag == "NONE" then
    else
      goto cont7
    end
    _L[280](_L[277])
  end
  ::cont7::
  do
    if _L[539] == nil then
    else
      goto else2
    end
    _L[569] = _L[373]()
    goto cont8
    ::else2::
    if _L[539] ~= nil and _L[539][2] == nil then
      _L[569] = _L[539][1]
      goto cont8
    end
    _L[301]("Multiple input files are not supported.")
    tmp15(1, true)
  end
  ::cont8::
  _L[570] = _L[538].engine
  do
    if _L[570].tag == "SOME" then
    else
      goto else2
    end
    do
      _L[577] = _L[570].payload
      _L[578] = _L[254](_L[577])
      if _L[578].tag == "SOME" then
        _L[579] = _L[578].payload
        _L[580] = _L[579].dvi_extension
        _L[581] = _L[579].engine_type
        _L[582] = _L[579].executable
        _L[583] = _L[579].name
        _L[584] = _L[579].supports_draftmode
        _L[576] = _L[579].supports_pdf_generation
        _L[575] = _L[584]
        _L[574] = _L[583]
        _L[573] = _L[582]
        _L[572] = _L[581]
        _L[571] = _L[580]
        goto cont9
      end
      if _L[578].tag == "NONE" then
      else
        _raise(_Match, "main.sml:793:50")
      end
      _L[301]("Unknown engine name '" .. _L[577] .. "'.")
      tmp15(1, true)
    end
    ::else2::
    if _L[570].tag == "NONE" then
    else
      _raise(_Match, "main.sml:792:32")
    end
    do
      do
        _L[586] = _L[154][0]
        if _L[586] == nil then
        else
          _L[585] = _L[586]
          goto cont18
        end
        do
          _L[587] = _Fail("CommandLine.name: arg is not available")
          _raise(_L[587], "command-line.sml:10:45")
        end
      end
      ::cont18::
      _L[588] = _L[211](_L[585])
      _L[589] = _L[213](_L[588])
      _L[590] = isPrefix("cl")
      _L[591] = _L[590](_L[589])
      do
        if _L[591] then
        else
          _L[592] = false
          goto cont19
        end
        do
          _L[593] = tmp32(isAlphaNum)
          _L[592] = _L[593](_L[589])
        end
      end
      ::cont19::
      if _L[592] then
      else
        goto else3
      end
      do
        _L[594] = extract(_L[589], 2, NONE)
        _L[595] = _L[254](_L[594])
        if _L[595].tag == "NONE" then
        elseif _L[595].tag == "SOME" then
          _L[596] = _L[595].payload
          _L[597] = _L[596].dvi_extension
          _L[598] = _L[596].engine_type
          _L[599] = _L[596].executable
          _L[600] = _L[596].name
          _L[601] = _L[596].supports_draftmode
          _L[576] = _L[596].supports_pdf_generation
          _L[575] = _L[601]
          _L[574] = _L[600]
          _L[573] = _L[599]
          _L[572] = _L[598]
          _L[571] = _L[597]
          goto cont9
        else
          _raise(_Match, "main.sml:806:51")
        end
        _L[301]("Engine not specified.")
        tmp15(1, true)
      end
      ::else3::
      _L[301]("Engine not specified.")
      tmp15(1, true)
    end
  end
  ::cont9::
  _L[602] = getOpt(_L[538].output_format, _L[230])
  do
    if _L[602] == "PDF" then
    else
      goto else2
    end
    do
      _L[604] = _L[538].check_driver
      do
        if _L[604].tag == "NONE" then
          goto cont18
        end
        if _L[604].tag == "SOME" then
        else
          _raise(_Match, "main.sml:815:44")
        end
        _L[301]("--check-driver can only be used when the output format is DVI.")
        tmp15(1, true)
      end
      ::cont18::
      if _L[576] then
      else
        _L[603] = {tag = "SOME", payload = _L[305]}
        goto cont10
      end
      do
        _L[605] = _L[237](_L[572], _L[235])
        if _L[605] then
          _L[603] = {tag = "SOME", payload = _L[310]}
          goto cont10
        end
        _L[606] = _L[237](_L[572], _L[234])
        if _L[606] then
          _L[603] = {tag = "SOME", payload = _L[309]}
          goto cont10
        end
        _L[607] = _L[237](_L[572], _L[233])
        if _L[607] then
          _L[603] = {tag = "SOME", payload = _L[308]}
          goto cont10
        end
        _L[302]("Unknown engine: " .. _L[574])
        _L[302]("Driver check will not work.")
        _L[603] = NONE
        goto cont10
      end
    end
    ::else2::
    if _L[602] == "DVI" then
      _L[608] = _L[538].check_driver
      if _L[608].tag == "SOME" and _L[608].payload == "DVIPDFMX" then
        _L[603] = {tag = "SOME", payload = _L[305]}
      elseif _L[608].tag == "SOME" and _L[608].payload == "DVIPS" then
        _L[603] = {tag = "SOME", payload = _L[306]}
      elseif _L[608].tag == "SOME" and _L[608].payload == "DVISVGM" then
        _L[603] = {tag = "SOME", payload = _L[307]}
      elseif _L[608].tag == "NONE" then
        _L[603] = NONE
      else
        _raise(_Match, "main.sml:837:42")
      end
    else
      _raise(_Match, "main.sml:813:38")
    end
  end
  ::cont10::
  _L[609] = _L[538].jobname
  do
    if _L[609].tag == "SOME" then
      _L[612] = _L[609].payload
      _L[611] = _L[612]
      _L[610] = _L[612]
      goto cont11
    end
    if _L[609].tag == "NONE" then
    else
      _raise(_Match, "main.sml:842:55")
    end
    do
      _L[613] = _L[213](_L[569])
      _L[614] = _L[211](_L[613])
      _L[611] = _L[614]
      _L[610] = tmp21(_L[614], ".", function(a)
        local tmp54 = tmp19(a)
        if tmp54 == 32 then
          return "_"
        end
        local tmp55 = isSpace(tmp54)
        local tmp56
        tmp56 = tmp55 or (tmp54 == 34 or (tmp54 == 36 or (tmp54 == 37 or (tmp54 == 38 or (tmp54 == 39 or (tmp54 == 40 or (tmp54 == 41 or (tmp54 == 59 or (tmp54 == 60 or (tmp54 == 62 or (tmp54 == 92 or (tmp54 == 94 or (tmp54 == 96 or tmp54 == 124)))))))))))))
        if tmp56 then
          local tmp57 = tmp54
          local s
          if tmp57 >= 0 then
            s = string_format("%X", tmp57)
          else
            s = string_format("~%X", - tmp57)
          end
          if tmp57 <= 15 then
            return "_0" .. s
          else
            return "_" .. s
          end
        else
          return string_char(tmp54)
        end
      end)
    end
  end
  ::cont11::
  if _L[602] == "DVI" then
    _L[615] = _L[571]
  elseif _L[602] == "PDF" then
    _L[615] = "pdf"
  else
    _raise(_Match, "main.sml:847:42")
  end
  _L[616] = _L[538].output
  if _L[616].tag == "NONE" then
    _L[617] = _L[611] .. "." .. _L[615]
  elseif _L[616].tag == "SOME" then
    _L[617] = _L[616].payload
  else
    _raise(_Match, "main.sml:850:49")
  end
  _L[618] = _L[538].output_directory
  do
    if _L[618].tag == "SOME" then
    else
      goto else2
    end
    do
      _L[620] = _L[618].payload
      if _L[538].fresh then
      else
        _L[619] = _L[620]
        goto cont12
      end
      _L[301]("--fresh and --output-directory cannot be used together.")
      tmp15(1, true)
    end
    ::else2::
    if _L[618].tag == "NONE" then
    else
      _raise(_Match, "main.sml:854:25")
    end
    do
      _L[621] = _L[215](NONE, _L[569])
      _L[622] = _L[541].temporary_directory
      _L[623] = getOpt(_L[538].engine_executable, _L[573])
      do
        do
          _L[626] = _L[191](table_concat({n = 3, _L[621], _L[610], _L[623]}, "\x00"))
          _L[627] = _L[626][1]
          _L[628] = _L[626][2]
          _L[629] = _L[626][3]
          _L[625] = _L[193](_L[627], _L[628], _L[629], _L[626][4])
        end
        do
          if _L[622].tag == "SOME" then
            _L[630] = _L[622].payload
            goto cont18
          end
          if _L[622].tag == "NONE" then
          else
            _raise(_Match, "main.sml:34:24")
          end
          do
            _L[631] = _L[372]({"TMPDIR", {"TMP", {"TEMP", nil}}})
            if _L[631].tag == "SOME" then
              _L[630] = _L[631].payload
              goto cont18
            end
            if _L[631].tag == "NONE" then
            else
              _raise(_Match, "main.sml:36:36")
            end
            do
              _L[632] = _L[372]({"HOME", {"USERPROFILE", nil}})
              if _L[632].tag == "SOME" then
              else
                goto else4
              end
              _L[630] = _L[77]({dir = _L[632].payload, file = ".latex-build-temp"})
              goto cont18
              ::else4::
              if _L[632].tag == "NONE" then
              else
                _raise(_Match, "main.sml:38:48")
              end
              do
                _L[633] = _Fail("environment variable 'TMPDIR' not set!")
                _raise(_L[633], "main.sml:40:60")
              end
            end
          end
        end
        ::cont18::
        _L[624] = _L[77]({dir = _L[630], file = "cluttex-" .. _L[625]})
      end
      _L[634] = _L[219](_L[624])
      if not _L[634] then
      else
        goto else3
      end
      do
        _L[635], _L[636] = _L[217].mkdir_rec(_L[624])
        if not _L[635] then
        else
          _L[619] = _L[624]
          goto cont12
        end
        do
          _L[637] = _Error(_L[636])
          _raise(_L[637], "fs-util.sml:16:28")
        end
      end
      ::else3::
      if _L[538].fresh then
      else
        _L[619] = _L[624]
        goto cont12
      end
      do
        if _ENV.CLUTTEX_VERBOSITY >= 1 then
        else
          goto else4
        end
        do
          _L[304]("Cleaning '" .. _L[624] .. "'...")
          _L[638], _L[639] = _L[217].remove_rec(_L[624])
          if not _L[638] then
          else
            goto else6
          end
          do
            _L[640] = _Error(_L[639])
            _raise(_L[640], "fs-util.sml:22:29")
          end
          ::else6::
          _L[65](_L[624])
          _L[619] = _L[624]
          goto cont12
        end
        ::else4::
        _L[641], _L[642] = _L[217].remove_rec(_L[624])
        if not _L[641] then
        else
          goto else5
        end
        do
          _L[643] = _Error(_L[642])
          _raise(_L[643], "fs-util.sml:22:29")
        end
        ::else5::
        _L[65](_L[624])
        _L[619] = _L[624]
      end
    end
  end
  ::cont12::
  do
    if _L[538].print_output_directory then
    else
      goto cont13
    end
    do
      _L[644] = _L[619] .. "\n"
      _L[645] = _L[113].getOutstream(_L[116])
      if _L[645].tag == "LUA_WRITABLE" then
      else
        goto else2
      end
      do
        _L[646] = _L[645].payload.writable
        _L[101](_L[646], _L[645].payload.name, _L[644])
        _L[646]:flush()
        tmp15(0, true)
      end
      ::else2::
      if _L[645].tag == "PRIM_WRITER" then
      else
        _raise(_Match, "text-io.sml:392:9")
      end
      do
        _L[647] = _L[645].payload.writer.name
        _L[648] = _L[645].payload.writer.writeVec
        _L[649] = _L[645].payload.buffer
        _L[650] = revAppend({_L[644], _L[649][1]}, nil)
        _L[651] = table_concat(_VectorOrArray_fromList(_L[650]))
        if _L[648].tag == "SOME" then
        elseif _L[648].tag == "NONE" then
          _raise({tag = _L[9], payload = {cause = _L[10], ["function"] = "output", name = _L[647]}}, "text-io.sml:397:26")
        else
          _raise(_Match, "text-io.sml:395:14")
        end
        do
          _L[652] = _L[648].payload
          _L[653] = tmp28(_L[651])
          _L[652](_L[653])
          _L[649][1] = nil
          tmp15(0, true)
        end
      end
    end
  end
  ::cont13::
  if _L[220] then
    _L[654] = ";"
  else
    _L[654] = ":"
  end
  _L[655] = _L[64](nil)
  _L[656] = getOpt(_L[538].change_directory, false)
  do
    if _L[656] then
    else
      _L[659] = {tag = "SOME", payload = _L[619]}
      _L[658] = _L[619]
      _L[657] = _L[617]
      goto cont14
    end
    do
      do
        _L[661] = tmp16("TEXINPUTS")
        if _L[661] == nil then
          _L[660] = NONE
        else
          _L[660] = {tag = "SOME", payload = _L[661]}
        end
      end
      _L[662] = getOpt(_L[660], "")
      do
        _L[664] = tmp16("LUAINPUTS")
        if _L[664] == nil then
          _L[663] = NONE
        else
          _L[663] = {tag = "SOME", payload = _L[664]}
        end
      end
      _L[665] = getOpt(_L[663], "")
      _L[63](_L[619])
      _L[221]("TEXINPUTS", _L[655] .. _L[654] .. _L[662])
      _L[221]("LUAINPUTS", _L[655] .. _L[654] .. _L[665])
      _L[666] = _L[215]({tag = "SOME", payload = _L[655]}, _L[617])
      _L[659] = NONE
      _L[658] = "."
      _L[657] = _L[666]
    end
  end
  ::cont14::
  _L[667] = _L[538].bibtex_or_biber
  do
    if _L[667].tag == "SOME" then
    elseif _L[667].tag == "NONE" then
      _L[668] = _L[657]
      goto cont15
    else
      _raise(_Match, "main.sml:902:32")
    end
    do
      do
        _L[670] = tmp16("BIBINPUTS")
        if _L[670] == nil then
          _L[669] = NONE
        else
          _L[669] = {tag = "SOME", payload = _L[670]}
        end
      end
      _L[671] = getOpt(_L[669], "")
      _L[221]("BIBINPUTS", _L[655] .. _L[654] .. _L[671])
      _L[668] = _L[215]({tag = "SOME", payload = _L[655]}, _L[617])
    end
  end
  ::cont15::
  do
    _L[673] = tmp16("max_print_line")
    if _L[673] == nil then
      _L[672] = NONE
    else
      _L[672] = {tag = "SOME", payload = _L[673]}
    end
  end
  if _L[672].tag == "NONE" then
    _L[221]("max_print_line", "16384")
  elseif _L[672].tag == "SOME" then
  else
    _raise(_Match, "main.sml:919:28")
  end
  _L[674] = _L[610] .. "." .. "fls"
  _L[675] = _L[210].join(_L[658], _L[674])
  _L[676] = _L[610] .. "." .. "cluttex-fls"
  _L[677] = _L[210].join(_L[658], _L[676])
  if _L[602] == "DVI" then
    _L[678] = _L[231]
    goto cont16
  end
  if _L[602] == "PDF" then
    if _L[576] then
      _L[678] = _L[230]
    else
      _L[678] = _L[231]
    end
  else
    _raise(_Match, "main.sml:928:43")
  end
  ::cont16::
  _L[679] = _L[237](_L[572], _L[235])
  if _L[679] then
    _L[681] = _L[610] .. "." .. "cluttexinit.lua"
    _L[682] = _L[210].join(_L[658], _L[681])
    _L[683] = _L[538].file_line_error
    _L[684] = {file_line_error = _L[683], halt_on_error = _L[538].halt_on_error, jobname = _L[610], output_directory = _L[658]}
    _L[354].create_initialization_script(_L[682], _L[684])
    _L[680] = {tag = "SOME", payload = _L[682]}
  else
    _L[680] = NONE
  end
  _L[685] = _L[538].source_date_epoch
  _L[686] = _L[685].tag == "SOME" and _L[685].payload.tag == "RAW"
  do
    if _L[686] then
      _L[221]("SOURCE_DATE_EPOCH", _L[685].payload.payload)
      _L[687] = NONE
      goto cont17
    end
    _L[688] = tmp(_L[348])
    _L[689] = _L[538].source_date_epoch
    _L[690] = _L[688]({_L[689], {tag = "SOME", payload = _L[347]}})
    do
      if _L[690] then
        _L[691] = true
        goto cont18
      end
      _L[692] = tmp(eq)
      _L[693] = tmp16("SOURCE_DATE_EPOCH")
      if _L[693] == nil then
      else
        _L[691] = _L[692]({{tag = "SOME", payload = _L[693]}, NONE})
        goto cont18
      end
      _L[691] = _L[692]({NONE, NONE})
    end
    ::cont18::
    if _L[691] then
    else
      _L[687] = NONE
      goto cont17
    end
    do
      _L[694] = tmp5(tmp18())
      _L[695] = _L[53](tmp18())
      _L[687] = {tag = "SOME", payload = {{time = _L[695], time_since_epoch = _L[694]}}}
    end
  end
  ::cont17::
  _L[696] = _L[538].engine_executable
  _L[697] = getOpt(_L[538].interaction, _L[223])
  _L[698] = {tag = "SOME", payload = _L[697]}
  _L[699] = _L[538].file_line_error
  _L[700] = _L[538].halt_on_error
  _L[701] = _L[538].synctex
  _L[702] = _L[538].shell_escape
  _L[703] = {tag = "SOME", payload = _L[610]}
  _L[704] = _L[538].fmt
  _L[705] = _L[538].tex_extraoptions
  _L[706] = getOpt(_L[538].max_iterations, 4)
  _L[707] = _L[538].start_with_draft
  getOpt(_L[538].change_directory, false)
  _L[708] = _L[538].includeonly
  _L[709] = _L[538].make_depends
  _L[710] = _L[538].package_support
  _L[711] = _L[538].source_date_epoch
  _L[712] = _L[538].synctex
  getOpt(_L[538].interaction, _L[223])
  _L[713] = _L[538].shell_escape
  _L[714] = _L[538].dvipdfmx_extraoptions
  _L[715] = _L[538].makeindex
  _L[716] = _L[538].bibtex_or_biber
  _L[717] = _L[538].makeglossaries
  if _L[568].tag == "NONE" then
  else
    goto else1
  end
  do
    _L[718], _L[719] = _handle(function()
      local tmp54 = _L[710].epstopdf
      local tmp55 = _L[710].minted
      return _L[529](_L[572], _L[573], _L[575], _L[576], _L[569], _L[716], _L[603], _L[714], _L[708], _L[610], _L[709], _L[717], _L[715], _L[706], _L[668], _L[658], _L[602], tmp54, tmp55, _L[710].pdfx, _L[713], _L[711], _L[707], _L[712], _L[655], _L[615], _L[675], _L[677], _L[687], false, _L[696], _L[705], _L[699], _L[704], _L[700], _L[698], _L[703], _L[680], _L[659], _L[678], _L[702], _L[701])
    end)
    if not _L[718] then
      if __exn_instanceof(_L[719], _L[370]) then
        tmp15(1, true)
      else
        _raise(_L[719], nil)
      end
    else
      goto cont4
    end
  end
  ::else1::
  if _L[568].tag == "SOME" then
  else
    _raise(_Match, "main.sml:1009:18")
  end
  do
    _L[720] = _L[568].payload
    do
      if _L[220] then
      else
        _L[721] = NONE
        goto cont18
      end
      do
        _L[722], _L[723] = pcall(tmp3, "texrunner.fswatcher_windows")
        if not _L[722] then
        else
          _L[721] = {tag = "SOME", payload = _L[723]}
          goto cont18
        end
        do
          if _ENV.CLUTTEX_VERBOSITY >= 1 then
          else
            _L[721] = NONE
            goto cont18
          end
          _L[302]("Failed to load texrunner.fswatcher_windows: " .. _L[723])
          _L[721] = NONE
        end
      end
    end
    ::cont18::
    do
      if _L[721].tag == "SOME" then
      else
        goto else2
      end
      do
        _L[725] = _L[721].payload
        if _ENV.CLUTTEX_VERBOSITY >= 2 then
        else
          _L[724] = function(a)
            return _L[530](_L[725], a)
          end
          goto cont19
        end
        _L[304]("Using built-in filesystem watcher for Windows")
        _L[724] = function(a)
          return _L[530](_L[725], a)
        end
        goto cont19
      end
      ::else2::
      if _L[721].tag == "NONE" then
      else
        _raise(_Match, "main.sml:689:25")
      end
      do
        _L[726] = _L[209]("fswatch")
        do
          if _L[726] then
          else
            _L[727] = false
            goto cont23
          end
          do
            _L[728] = _L[346](_L[720], _L[345])
            if _L[728] then
              _L[727] = true
            else
              _L[727] = _L[346](_L[720], _L[345])
            end
          end
        end
        ::cont23::
        if _L[727] then
        else
          goto else3
        end
        do
          if _ENV.CLUTTEX_VERBOSITY >= 2 then
          else
            _L[724] = _L[531]
            goto cont19
          end
          _L[304]("Using `fswatch' command")
          _L[724] = _L[531]
          goto cont19
        end
        ::else3::
        _L[729] = _L[209]("inotifywait")
        do
          if _L[729] then
          else
            _L[730] = false
            goto cont24
          end
          do
            _L[731] = _L[346](_L[720], _L[345])
            if _L[731] then
              _L[730] = true
            else
              _L[730] = _L[346](_L[720], _L[344])
            end
          end
        end
        ::cont24::
        if _L[730] then
        else
          goto else4
        end
        do
          if _ENV.CLUTTEX_VERBOSITY >= 2 then
          else
            _L[724] = _L[532]
            goto cont19
          end
          _L[304]("Using `inotifywait' command")
          _L[724] = _L[532]
          goto cont19
        end
        ::else4::
        if _L[720] == "AUTO" then
        else
          goto else5
        end
        _L[301]("Could not watch files because neither `fswatch' nor `inotifywait' was installed.")
        _L[304]("See ClutTeX's manual for details.")
        tmp15(1, true)
        ::else5::
        if _L[720] == "FSWATCH" then
        else
          goto else6
        end
        _L[301]("Could not watch files because your selected engine `fswatch' was not installed.")
        _L[304]("See ClutTeX's manual for details.")
        tmp15(1, true)
        ::else6::
        if _L[720] == "INOTIFYWAIT" then
        else
          _raise(_Match, "main.sml:712:43")
        end
        _L[301]("Could not watch files because your selected engine `inotifywait' was not installed.")
        _L[304]("See ClutTeX's manual for details.")
        tmp15(1, true)
      end
    end
    ::cont19::
    do
      _L[732], _L[733] = _handle(function()
        do
          local tmp54 = _L[710].epstopdf
          local tmp55 = _L[710].minted
          _L[529](_L[572], _L[573], _L[575], _L[576], _L[569], _L[716], _L[603], _L[714], _L[708], _L[610], _L[709], _L[717], _L[715], _L[706], _L[668], _L[658], _L[602], tmp54, tmp55, _L[710].pdfx, _L[713], _L[711], _L[707], _L[712], _L[655], _L[615], _L[675], _L[677], _L[687], false, _L[696], _L[705], _L[699], _L[704], _L[700], _L[698], _L[703], _L[680], _L[659], _L[678], _L[702], _L[701])
        end
        return nil
      end)
      if not _L[732] then
        if __exn_instanceof(_L[733], _L[370]) then
        else
          _raise(_L[733], nil)
        end
      end
    end
    _L[734] = _L[360](_L[675], _L[715], nil, _L[203])
    _L[735] = _L[237](_L[572], _L[235])
    do
      if _L[735] then
      else
        _L[736] = false
        goto cont20
      end
      _L[736] = _L[218](_L[677])
    end
    ::cont20::
    do
      if _L[736] then
      else
        _L[737] = _L[734]
        goto cont21
      end
      do
        _L[738] = _L[734][1]
        _L[737] = _L[360](_L[677], _L[715], _L[738], _L[734][2])
      end
    end
    ::cont21::
    do
      _L[740] = _L[737][1]
      _L[739] = _L[359](_L[740], _L[737][2])
    end
    _L[741] = _L[739][1]
    _L[742] = mapPartial1(function(a)
      if a.kind == "INPUT" then
        return {tag = "SOME", payload = a.abspath}
      else
        return NONE
      end
    end)
    _L[743] = _L[742](_L[741])
    _L[744] = _L[743]
    ::cont22::
    do
      local _L1 = {}
      _L1[1] = _L[744]
      _L1[2] = _L[724](_L1[1])
      if _L1[2] then
      else
        return
      end
      do
        do
          _L1[4], _L1[5] = _handle(function()
            do
              local tmp54 = _L[710].epstopdf
              local tmp55 = _L[710].minted
              _L[529](_L[572], _L[573], _L[575], _L[576], _L[569], _L[716], _L[603], _L[714], _L[708], _L[610], _L[709], _L[717], _L[715], _L[706], _L[668], _L[658], _L[602], tmp54, tmp55, _L[710].pdfx, _L[713], _L[711], _L[707], _L[712], _L[655], _L[615], _L[675], _L[677], _L[687], false, _L[696], _L[705], _L[699], _L[704], _L[700], _L[698], _L[703], _L[680], _L[659], _L[678], _L[702], _L[701])
            end
            return true
          end)
          if not _L1[4] then
            if __exn_instanceof(_L1[5], _L[370]) then
              _L1[3] = false
            else
              _raise(_L1[5], nil)
            end
          else
            _L1[3] = _L1[5]
          end
        end
        if _L1[3] then
        else
          _L[744] = _L1[1]
          goto cont22
        end
        do
          _L1[6] = _L[360](_L[675], _L[715], nil, _L[203])
          _L1[7] = _L[237](_L[572], _L[235])
          do
            if _L1[7] then
            else
              _L1[8] = false
              goto cont23
            end
            _L1[8] = _L[218](_L[677])
          end
          ::cont23::
          do
            if _L1[8] then
            else
              _L1[9] = _L1[6]
              goto cont24
            end
            do
              _L1[10] = _L1[6][1]
              _L1[9] = _L[360](_L[677], _L[715], _L1[10], _L1[6][2])
            end
          end
          ::cont24::
          do
            _L1[12] = _L1[9][1]
            _L1[11] = _L[359](_L1[12], _L1[9][2])
          end
          _L1[13] = _L1[11][1]
          _L1[14] = mapPartial1(function(a)
            if a.kind == "INPUT" then
              return {tag = "SOME", payload = a.abspath}
            else
              return NONE
            end
          end)
          _L[744] = _L1[14](_L1[13])
          goto cont22
        end
      end
    end
  end
end
::cont4::
