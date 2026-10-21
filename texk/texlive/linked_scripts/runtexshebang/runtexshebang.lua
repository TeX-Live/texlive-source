#!/usr/bin/env texlua

-- Copyright (c) 2021-2026 Munehiro Yamamoto <munepixyz@gmail.com>
--
-- This work may be distributed and/or modified under the
-- conditions of the LaTeX Project Public License, either version 1.3c
-- of this license or any later version.
-- The latest version of this license is in
--   http://www.latex-project.org/lppl.txt
-- and version 1.3c or later is part of all distributions of LaTeX
-- version 2005/12/01 or later.
--
-- This work has the LPPL maintenance status `maintained'.
-- The Current Maintainer of this work is Munehiro Yamamoto.
--
-- This work consists of the files runtexshebang.lua, README.md,
-- LICENSE, Makefile, ctan.toml and test/run.lua.

-- Security notes
--
-- * This script executes the "%#!" line of the given document verbatim
--   through the system shell (/bin/sh on Unix, cmd.exe on Windows).
--   Running it on a document is equivalent to typing that line yourself:
--   only use it on documents you trust.  The command is printed on stderr
--   before it is executed; "--dry-run" prints it without executing it.
-- * A "%#!" line containing an ASCII control character other than TAB is
--   refused, with or without "--dry-run": a CR or an escape sequence could
--   make the printed command differ from the one the shell runs.  Bytes
--   >= 0x80 are not checked so that Shift_JIS and EUC-JP lines still work.
-- * It never calls require or dofile, reads no configuration file and
--   writes nothing.  Its behaviour depends only on its arguments, the
--   first MAXLINES lines of the input file and the environment
--   (PATH, COMSPEC).
-- * It must never be added to shell_escape_commands in texmf.cnf: that
--   would let any document run arbitrary commands under restricted
--   shell escape via \write18{runtexshebang evil.tex}.

local NAME = "runtexshebang"
local VERSION = "2026-10-21 v1.1"
local MAXLINES = 20
local USAGE = [[
Usage:	runtexshebang [options] [--] input.tex

Options:
	--dry-run	print the TeX-style shebang line of input.tex without running it
	-h, --help	print help
	-v, --version	print version

The TeX-style shebang line is the first line within the first 20 lines of
input.tex that starts with "%#!" in column 1.  The rest of that line is
executed through the system shell, so only use runtexshebang on documents
you trust.  A shebang line containing a control character other than TAB is
refused.  The exit status of that command is passed through; runtexshebang
itself exits 1 on usage or file errors and 2 when no usable shebang line is
found.

See also:
The command
	texdoc runtexshebang
should give you access to the README.
]]

local function whoami()
  print(NAME .. " " .. VERSION)
end

local function help()
  whoami()
  print()
  print(USAGE)
end

local function warn(msg)
  io.stderr:write(NAME .. ": " .. msg .. "\n")
end

local function die(code, msg)
  warn(msg)
  os.exit(code)
end

if #arg == 0 then
  help()
  os.exit(0)
end

-- Argument parsing.  Both "-opt" and "--opt" forms are accepted; "--" ends
-- option parsing.  Unknown options are an error rather than a file name.
local is_dryrun = false
local texfilename = nil
local only_files = false

local function set_filename(name)
  if texfilename then
    die(1, "only one input file is accepted (got '" .. texfilename .. "' and '" .. name .. "')")
  end
  texfilename = name
end

for i = 1, #arg do
  local a = arg[i]
  if only_files then
    set_filename(a)
  elseif a == "--" then
    only_files = true
  elseif a == "-h" or a == "-help" or a == "--help" then
    help()
    os.exit(0)
  elseif a == "-v" or a == "-version" or a == "--version" then
    whoami()
    os.exit(0)
  elseif a == "-dry-run" or a == "--dry-run" then
    is_dryrun = true
  elseif a:sub(1, 1) == "-" then
    die(1, "unknown option '" .. a .. "'\n" .. USAGE)
  else
    set_filename(a)
  end
end

whoami()

if not texfilename then
  die(1, "No filename argument given, exiting.")
end

-- Return the command of the TeX-style shebang line of |path|, or nil if
-- there is none within the first MAXLINES lines.  On a read error return
-- nil plus the error message.
local function find_shebang(path)
  local fh, open_err = io.open(path, "rb")
  if not fh then
    return nil, open_err
  end
  local cmd
  local ok, read_err = pcall(function()
    local n = 0
    for line in fh:lines() do
      n = n + 1
      if n == 1 then
        -- ignore a UTF-8 byte order mark
        line = line:gsub("^\239\187\191", "")
      end
      -- strip trailing whitespace, including the CR of CRLF line endings
      line = line:gsub("%s+$", "")
      local rest = line:match("^%%#!(.*)$")
      if rest then
        cmd = rest:gsub("^%s+", "")
        break
      end
      if n >= MAXLINES then
        break
      end
    end
  end)
  fh:close()
  if not ok then
    -- e.g. "Is a directory"; drop the "file:line: " prefix Lua adds
    return nil, (tostring(read_err):gsub("^.-:%d+: ", ""))
  end
  return cmd
end

-- Run |cmd| through the system shell and return its exit status.
-- texlua replaces os.execute with its own version that returns the raw
-- value of C system(): on Unix a wait status, on Windows the exit code of
-- cmd.exe, and -1 when the command interpreter could not be started.
-- Plain Lua 5.3/5.4 returns true|nil, "exit"|"signal", code instead.
local function run_shell(cmd)
  local r1, r2, r3 = os.execute(cmd)
  if type(r1) == "number" then
    if r1 == -1 then
      if os.type == "windows" then
        die(127, "cannot run the command interpreter (cmd.exe not found via COMSPEC/PATH, or the command line is too long)")
      else
        die(127, "cannot start /bin/sh")
      end
    end
    if os.type == "windows" then
      return r1
    end
    if r1 % 256 ~= 0 then
      return 128 + (r1 % 128)   -- killed by a signal
    end
    return r1 // 256            -- WEXITSTATUS
  elseif r1 == true then
    return 0
  elseif r2 == "signal" then
    return 128 + (r3 or 0)
  else
    return r3 or 1
  end
end

-- main process
local cmd, err = find_shebang(texfilename)
if err then
  die(1, "cannot read '" .. texfilename .. "': " .. err)
end
if cmd == nil then
  die(2, "TeX-style shebang not found.")
end
if cmd == "" then
  die(2, "TeX-style shebang line is empty.")
end
-- A CR, an escape sequence and the like could hide the real command from
-- the "running:" line and from --dry-run, so refuse them.
if cmd:find("[%z\1-\8\10-\31\127]") then
  die(2, "TeX-style shebang line contains a control character, refusing to run it: "
         .. string.format("%q", cmd))
end

if is_dryrun then
  print(cmd)
  os.exit(0)
end

warn("running: " .. cmd)
io.stdout:flush()
io.stderr:flush()

if os.type == "windows" and os.setenv then
  -- cmd.exe looks for programs in the current directory before PATH.
  -- Refuse that so that an executable placed next to the document cannot
  -- shadow a TeX Live program named on the shebang line.
  os.setenv("NoDefaultCurrentDirectoryInExePath", "1")
end

local code = run_shell(cmd)
if code ~= 0 then
  warn("command exited with status " .. code)
end
os.exit(code)
