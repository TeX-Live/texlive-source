#! /usr/bin/env texlua
--
-- global constants
--
VERSION = "0.9.3"
FILE_SUFFIX = "_chtr"
PMX_CMD = "pmxab"

--[[
   pmxchords.lua: transpose chords (\ch.C.\) and process pmxab

   (c) Copyright 2013 Ondrej Fafejta <fafejtao@gmail.com>
       https://github.com/fafejtao/pmxChords

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

--]]

--[[
  ChangeLog:
    version 0.9.3 2014-04-21 - fixed transposition problem:
                               If song began in E-minor and signature is changed in half of song to A-minor
                               transposition must be omitted.

    version 0.9.2 2013-12-13 - fixed: stop processing when pmxab fail. 
                             - When pmxab is called generated file pmxaerr.dat is not removed.

    version 0.9.1 2013-12-09 - added some options. Improved processing.

    version 0.9   2013-12-05 - rewritted code from perl to lua...
--]]

function usage()
    print("Usage:  [texlua] pmxchords.lua  basename[.pmx] ")
    print("     1. transpose chords into new file basename" .. FILE_SUFFIX .. ".pmx")
    print("     2. call pmxab on transposed file basename" .. FILE_SUFFIX .. ".pmx")
    print("     3. rename basename" .. FILE_SUFFIX .. ".tex to basename.tex")
    print("options: -v  version")
    print("         -h  help")
    print("         -s  stop after fist step. e.g. generate only transposed basename" .. FILE_SUFFIX .. ".pmx file")
end

function whoami()
    print("This is pmxchords.lua version " .. VERSION)
end

whoami()
if #arg == 0 then
    usage()
    os.exit(0)
end

kpse.set_program_name("luatex")
require"ChordsTr"

function parseInputSignature(line)
    -- parse input signature from pmx digits line. Only one line digits format is supported!
    -- e.g.
    --    1      1       2     4      2      4        0    -1
    -- signature is -1 : F major
    local res = string.match(line, "^%s*[0-9]+%s+[0-9]+%s+[0-9]+%s+[0-9]+%s+[0-9]+%s+[0-9]+%s+[0-9]+%s+([%+%-]?[0-9]+)%s*$")
    return tonumber(res)
end

function parseOutputSignature(line)
    -- parse output signature from pmx
    -- e.g.
    -- K-2+2
    -- output signature is +2 : D major
    local res = string.match(line, "^K[%+%-]?[0-9]+([%+%-]?[0-9]+)")
    return tonumber(res)
end

--
-- Strip comments from line
-- Comments begin with % character
--
function lineWithoutComment(line)
    local i = string.find(line, "%%")
    if (i == nil) then
        return line
    end
    return line:sub(0, i - 1)
end

--
-- Function checks only if the line contains \ch. string.
-- It does not check correctly if notes already started. It checks only if chords started..
-- Scenario bellow will fail!
--   d44 f a r / % first line does not contain chords symbol
--   % signature is changed
--   K+0+0
--   \ch.C.\ c44 e g r4 /
--
function checkChordsAlreadyStarted(line)
    local i = string.find(lineWithoutComment(line), ChordsTr.CHORDS_BEGIN)
    if (i ~= nil) then
        return true
    end
    return false;
end

function handleErr(msg)
    io.stderr:write(msg .. "\n")
    os.exit(2)
end

--
-- get file name without .pmx suffix
--
function getBaseFileName(fileName)
    if fileName ~= "" and string.sub(fileName, -4, -1) == ".pmx" then
        return string.sub(fileName, 1, -5)
    else
        return fileName
    end
end

--
-- Make chords transposition. Generates new pmx file with transposed chords.
--
-- param baseName input file name without .pmx suffix
-- return outputBaseName transposed pmx file name without .pmx suffix
-- 
function makeChordsTransposition(baseName)
    local inputFileName = baseName .. ".pmx"
    local inputFile = io.open(inputFileName, "r")
    if (inputFile == nil) then
        handleErr("File does not exist: " .. inputFileName)
    end

    local outputBaseName = baseName .. FILE_SUFFIX -- output base name without suffix. Used for renaming generated .tex file ...
    local outputFileName = outputBaseName .. ".pmx"
    local outputFile = io.open(outputFileName, "w")
    if (outputFile == nil) then
        handleErr("Can not create temporary file: " .. outputFileName)
    end

    --
    -- transpose chords from inputFile into outputFile
    --
    -- input and output signature will be parsed from input pmx file
    --
    local iSig = nil -- input signature
    local chTr = nil -- chords transposition class

    -- If song began in E-minor and signature is changed in half of song to A-minor
    --      transposition must be omitted.
    local chordsAlreadyStarted = false;

    for line in inputFile:lines() do
        if (chTr ~= nil) then
            outputFile:write(chTr:lineTranspose(line) .. "\n")
        else
            outputFile:write(line .. "\n")
        end
        if (iSig == nil) then
            iSig = parseInputSignature(line)
            if (iSig ~= nil) then
                io.stderr:write("Chords: input signature detected: " .. iSig .. "\n")
            end
        elseif (chTr == nil and not chordsAlreadyStarted) then
            local oSig = parseOutputSignature(line)
            if (oSig ~= nil) then
                io.stderr:write("Chords: output signature detected: " .. oSig .. "\n")
                chTr = ChordsTr(iSig, oSig)
            elseif (checkChordsAlreadyStarted(line)) then
                io.stderr:write("Chords: notes already started. Chords transposition is omitted.\n")
                chordsAlreadyStarted = true;
            end
        end
    end
    inputFile:close()
    outputFile:close()

    return outputBaseName
end

--
-- Make pmxab process.
-- param baseName input file name without .pmx suffix
-- param outputBaseName transposed pmx file name without .pmx suffix
--
function pmxabProcess(baseName, outputBaseName)
    --
    -- call pmxab to generate .tex file
    --
    os.execute(PMX_CMD .. " " .. outputBaseName)
    -- check error
    local pmxaerr = io.open("pmxaerr.dat", "r")
    if (not pmxaerr) then
        handleErr("No log file.")
    end
    local linebuf = pmxaerr:read()
    local err = tonumber(linebuf)
    pmxaerr:close()
    if (err ~= 0) then
        handleErr("PMX process fail! " .. outputBaseName .. ".pmx")
    end

    os.rename(outputBaseName .. ".tex", baseName .. ".tex")

    -- remove temporary files
    os.remove(outputBaseName .. ".pmx")
    os.remove(outputBaseName .. ".pml")
end

narg = 1
onlyTranspose = false

repeat
    local this_arg = arg[narg]
    if this_arg == "-v" then
        os.exit(0)
    elseif this_arg == "-h" then
        usage()
        os.exit(0)
    elseif this_arg == "-s" then
        onlyTranspose = true
    else
        local baseName = getBaseFileName(this_arg)
        local outputBaseName = makeChordsTransposition(baseName)

        if (onlyTranspose) then
            os.exit(0)
        end
        pmxabProcess(baseName, outputBaseName)
    end

    narg = narg + 1
until narg > #arg

