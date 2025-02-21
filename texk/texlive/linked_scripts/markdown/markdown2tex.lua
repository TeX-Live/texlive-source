#!/usr/bin/env texlua
--
-- Copyright (C) 2009-2016 John MacFarlane, Hans Hagen
--
-- Permission is hereby granted, free of charge, to any person obtaining
-- a copy of this software and associated documentation files (the
-- "Software"), to deal in the Software without restriction, including
-- without limitation the rights to use, copy, modify, merge, publish,
-- distribute, sublicense, and/or sell copies of the Software, and to
-- permit persons to whom the Software is furnished to do so, subject to
-- the following conditions:
--
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
-- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
-- MERCHANTABILITY, FITNESS FOR PARTICULAR PURPOSE AND NONINFRINGEMENT.
-- IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
-- CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
-- TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
-- SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--
-- Copyright (C) 2016-2025 Vít Starý Novotný, Andrej Genčur
--
-- This work may be distributed and/or modified under the
-- conditions of the LaTeX Project Public License, either version 1.3
-- of this license or (at your option) any later version.
-- The latest version of this license is in
--
--     http://www.latex-project.org/lppl.txt
--
-- and version 1.3 or later is part of all distributions of LaTeX
-- version 2005/12/01 or later.
--
-- This work has the LPPL maintenance status `maintained'.
-- The Current Maintainer of this work is Vít Starý Novotný.
--
-- Send bug reports, requests for additions and questions
-- either to the GitHub issue tracker at
--
--     https://github.com/witiko/markdown/issues
--
-- or to the e-mail address <witiko@mail.muni.cz>.
--
-- MODIFICATION ADVICE:
--
-- If you want to customize this file, it is best to make a copy of
-- the source file(s) from which it was produced. Use a different
-- name for your copy(ies) and modify the copy(ies); this will ensure
-- that your modifications do not get overwritten when you install a
-- new release of the standard system. You should also ensure that
-- your modified source file does not generate any modified file with
-- the same name as a standard file.
--
-- You will also need to produce your own, suitably named, .ins file to
-- control the generation of files from your source file; this file
-- should contain your own preambles for the files it generates, not
-- those in the standard .ins files.
--
local metadata = {
    version   = "3.11.0-0-ga9095584",
    comment   = "A module for the conversion from markdown "
             .. "to plain TeX",
    author    = "John MacFarlane, Hans Hagen, Vít Starý Novotný, "
             .. "Andrej Genčur",
    copyright = {"2009-2016 John MacFarlane, Hans Hagen",
                 "2016-2024 Vít Starý Novotný, Andrej Genčur"},
    license   = "LPPL 1.3c"
}

local defaultOptions = {}
defaultOptions.eagerCache = true
defaultOptions.experimental = false
defaultOptions.singletonCache = true
defaultOptions.unicodeNormalization = true
defaultOptions.unicodeNormalizationForm = "nfc"
defaultOptions.cacheDir = "."
defaultOptions.contentBlocksLanguageMap = "markdown-languages.json"
defaultOptions.debugExtensionsFileName = "debug-extensions.json"
defaultOptions.frozenCacheFileName = "frozenCache.tex"
defaultOptions.autoIdentifiers = false
defaultOptions.blankBeforeBlockquote = false
defaultOptions.blankBeforeCodeFence = false
defaultOptions.blankBeforeDivFence = false
defaultOptions.blankBeforeHeading = false
defaultOptions.blankBeforeList = false
defaultOptions.bracketedSpans = false
defaultOptions.breakableBlockquotes = true
defaultOptions.citationNbsps = true
defaultOptions.citations = false
defaultOptions.codeSpans = true
defaultOptions.contentBlocks = false
defaultOptions.contentLevel = "block"
defaultOptions.debugExtensions = false
defaultOptions.definitionLists = false
defaultOptions.ensureJekyllData = false
defaultOptions.expectJekyllData = false
defaultOptions.extensions = {}
defaultOptions.fancyLists = false
defaultOptions.fencedCode = true
defaultOptions.fencedCodeAttributes = false
defaultOptions.fencedDivs = false
defaultOptions.finalizeCache = false
defaultOptions.frozenCacheCounter = 0
defaultOptions.gfmAutoIdentifiers = false
defaultOptions.hashEnumerators = false
defaultOptions.headerAttributes = false
defaultOptions.html = true
defaultOptions.hybrid = false
defaultOptions.inlineCodeAttributes = false
defaultOptions.inlineNotes = false
defaultOptions.jekyllData = false
defaultOptions.linkAttributes = false
defaultOptions.lineBlocks = false
defaultOptions.mark = false
defaultOptions.notes = false
defaultOptions.pipeTables = false
defaultOptions.preserveTabs = true
defaultOptions.rawAttribute = false
defaultOptions.relativeReferences = false
defaultOptions.shiftHeadings = 0
defaultOptions.slice = "^ $"
defaultOptions.smartEllipses = false
defaultOptions.startNumber = true
defaultOptions.strikeThrough = false
defaultOptions.stripIndent = false
defaultOptions.subscripts = false
defaultOptions.superscripts = false
defaultOptions.tableAttributes = false
defaultOptions.tableCaptions = false
defaultOptions.taskLists = false
defaultOptions.texComments = false
defaultOptions.texMathDollars = false
defaultOptions.texMathDoubleBackslash = false
defaultOptions.texMathSingleBackslash = false
defaultOptions.tightLists = true
defaultOptions.underscores = true
local HELP_STRING = "Usage: " .. [[
markdown2tex [OPTIONS] -- [INPUT_FILE] [OUTPUT_FILE]

OPTIONS are documented in Section 2.2.1 of the Markdown Package User
Manual (https://ctan.org/pkg/markdown).

When OUTPUT_FILE is unspecified, the result of the conversion will be
written to the standard output. When INPUT_FILE is also unspecified, the
result of the conversion will be read from the standard input.

Report bugs to: witiko@mail.muni.cz
Markdown package home page: <https://github.com/witiko/markdown>]]

local VERSION_STRING = [[
markdown2tex (Markdown) ]] .. metadata.version .. [[

Copyright (C) ]] .. table.concat(metadata.copyright,
                                 "\nCopyright (C) ") .. [[

License: ]] .. metadata.license

local function warn(s)
  io.stderr:write("Warning: " .. s .. "\n")
end

local function error(s)
  io.stderr:write("Error: " .. s .. "\n")
  os.exit(1)
end
local function camel_case(option_name)
  local cased_option_name = option_name:gsub("_(%l)", function(match)
    return match:sub(2, 2):upper()
  end)
  return cased_option_name
end

local function snake_case(option_name)
  local cased_option_name = option_name:gsub("%l%u", function(match)
    return match:sub(1, 1) .. "_" .. match:sub(2, 2):lower()
  end)
  return cased_option_name
end

local cases = {camel_case, snake_case}
local various_case_options = {}
for option_name, _ in pairs(defaultOptions) do
  for _, case in ipairs(cases) do
    various_case_options[case(option_name)] = option_name
  end
end

local process_options = true
local options = {}
local input_filename
local output_filename
for i = 1, #arg do
  if process_options then
    if arg[i] == "--" then
      process_options = false
      goto continue
    elseif arg[i]:match("=") then
      local key, value = arg[i]:match("(.-)=(.*)")
      if defaultOptions[key] == nil and
         various_case_options[key] ~= nil then
        key = various_case_options[key]
      end
      local default_type = type(defaultOptions[key])
      if default_type == "boolean" then
        options[key] = (value == "true")
      elseif default_type == "number" then
        options[key] = tonumber(value)
      elseif default_type == "table" then
        options[key] = {}
        for item in value:gmatch("[^ ,]+") do
          table.insert(options[key], item)
        end
      else
        if default_type ~= "string" then
          if default_type == "nil" then
            warn('Option "' .. key .. '" not recognized.')
          else
            warn('Option "' .. key .. '" type not recognized, ' ..
                 'please file a report to the package maintainer.')
          end
          warn('Parsing the ' .. 'value "' .. value ..'" of option "' ..
               key .. '" as a string.')
        end
        options[key] = value
      end
      goto continue
    elseif arg[i] == "--help" or arg[i] == "-h" then
      print(HELP_STRING)
      os.exit()
    elseif arg[i] == "--version" or arg[i] == "-v" then
      print(VERSION_STRING)
      os.exit()
    end
  end
  if input_filename == nil then
    input_filename = arg[i]
  elseif output_filename == nil then
    output_filename = arg[i]
  else
    error('Unexpected argument: "' .. arg[i] .. '".')
  end
  ::continue::
end

local input
if input_filename then
  local input_file = assert(io.open(input_filename, "r"),
    [[Could not open file "]] .. input_filename .. [[" for reading]])
  input = assert(input_file:read("*a"))
  assert(input_file:close())
else
  input = assert(io.read("*a"))
end

local lfs = require("lfs")
if options.cacheDir and not lfs.isdir(options.cacheDir) then
  assert(lfs.mkdir(options["cacheDir"]))
end
local kpse
(function()
  local should_initialize = package.loaded.kpse == nil
                       or tex.initialize ~= nil
  kpse = require("kpse")
  if should_initialize then
    kpse.set_program_name("luatex")
  end
end)()
local md = require("markdown")
if metadata.version ~= md.metadata.version then
  warn("markdown-cli.lua " .. metadata.version .. " used with " ..
       "markdown.lua " .. md.metadata.version .. ".")
end

local convert = md.new(options)
local raw_output, flat_output = convert(input, true)
local output
if flat_output == nil then
  if options.eagerCache then
    warn("markdown.lua has not produced flat output, so I am using " ..
         "backwards-compatible raw output instead. This may cause " ..
         'the conversion result to be hidden behind "\\input".')
  end
  output = raw_output
else
  output = flat_output()
end

if output_filename then
  local output_file = assert(io.open(output_filename, "w"),
    [[Could not open file "]] .. output_filename .. [[" for writing]])
  assert(output_file:write(output))
  assert(output_file:close())
else
  assert(io.write(output))
end
if options.cacheDir then
  lfs.rmdir(options.cacheDir)
end
