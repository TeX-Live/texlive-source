#!/usr/bin/env texlua
-- -----------------------------------------------------------------
-- checkcites.lua
-- Copyright 2012, 2019, Enrico Gregorio, Paulo Roberto Massa Cereda
--
-- This work may be distributed and/or modified under the conditions
-- of the LaTeX  Project Public License, either version  1.3 of this
-- license or (at your option) any later version.
--
-- The latest version of this license is in
--
-- http://www.latex-project.org/lppl.txt
--
-- and version  1.3 or later is  part of all distributions  of LaTeX
-- version 2005/12/01 or later.
--
-- This  work  has the  LPPL  maintenance  status `maintained'.  the
-- current maintainers of  this work are the  original authors. This
-- work consists of the file checkcites.lua.
--
-- Project repository: http://github.com/cereda/checkcites
-- -----------------------------------------------------------------

-- Checks if the table contains the element.
-- @param a Table.
-- @param hit Element.
-- @return Boolean value if the table contains the element.
local function exists(a, hit)
  for _, v in ipairs(a) do
    if v == hit then
      return true
    end
  end
  return false
end

-- Parses the list of arguments based on a configuration map.
-- @param map Configuration map.
-- @param args List of command line arguments.
-- @return Table containing the valid keys and entries.
-- @return Table containing the invalid keys.
local function parse(map, args)
  local keys, key, unknown = {}, 'unpaired', {}
  local a, b
  for _, v in ipairs(args) do
    a, _, b = string.find(v, '^%-(%w)$')
    if a then
      for _, x in ipairs(map) do
        key = 'unpaired'
        if x['short'] == b then
          key = x['long']
          break
        end
      end
      if key == 'unpaired' then
        table.insert(unknown, '-' .. b)
      end
      if not keys[key] then
        keys[key] = {}
      end
    else
      a, _, b = string.find(v, '^%-%-([%w-]+)$')
      if a then
        for _, x in ipairs(map) do
          key = 'unpaired'
          if x['long'] == b then
            key = b
            break
          end
        end
        if key == 'unpaired' then
          if not exists(unknown, '--' .. b) then
            table.insert(unknown, '--' .. b)
          end
        end
        if not keys[key] then
          keys[key] = {}
        end
      else
        if not keys[key] then
          keys[key] = {}
        end
        if key ~= 'unpaired' then
          for _, x in ipairs(map) do
            if x['long'] == key then
              if not (x['argument'] and
                 #keys[key] == 0) then
                key = 'unpaired'
              end
              break
            end
          end
          if not keys[key] then
            keys[key] = {}
          end
          table.insert(keys[key], v)
        else
          if not keys[key] then
            keys[key] = {}
          end
          table.insert(keys[key], v)
        end
      end
    end
  end
  return keys, unknown
end

-- Calculates the difference between two tables.
-- @param a First table.
-- @param b Second table.
-- @return Table containing the difference between two tables.
local function difference(a, b)
  local result = {}
  for _, v in ipairs(a) do
    if not exists(b, v) then
      table.insert(result, v)
    end
  end
  return result
end

-- Splits the string based on a pattern.
-- @param str String.
-- @param pattern Pattern.
local function split(str, pattern)
  local result = {}
  string.gsub(str, pattern, function(a)
              table.insert(result, a) end)
  return result
end

-- Reads lines from a file.
-- @param file File.
-- @returns Table representing the lines.
local function read(file)
  local handler = io.open(file, 'r')
  local lines = {}
  if handler then
    for line in handler:lines() do
      table.insert(lines, line)
    end
    handler:close()
  end
  return lines
end

-- Gets a pluralized word based on a counter.
-- @param i Counter.
-- @param a Word in singular.
-- @param b Word in plural.
-- @return Either the first or second word based on the counter.
local function plural(i, a, b)
  if i == 1 then
    return a
  else
    return b
  end
end

-- Normalizes the string, removing leading and trailing spaces.
-- @param str String.
-- @return Normalized string without leading and trailing spaces.
local function normalize(str)
  local result, _ = string.gsub(str, '^%s', '')
  result, _ = string.gsub(result, '%s$', '')
  return result
end

-- Checks if the element is in a blacklist.
-- @param a Element.
-- @return Boolean value if the element is blacklisted.
local function blacklist(a)
  local list = {}
  for _, v in ipairs(list) do
    if v == a then
      return true
    end
  end
  return false
end

-- Checks if the key is allowed.
-- @param v The key itself.
-- @return Boolean value if the key is allowed.
local function allowed(key)
  local keys = { 'string', 'comment' }
  for _, v in ipairs(keys) do
    if string.lower(key) == v then
      return false
    end
  end
  return true
end

-- Extracts the biblographic key.
-- @param lines Lines of a file.
-- @return Table containing bibliographic keys.
local function extract(lines)
  local result = {}
  for _, line in ipairs(lines) do
    local key, hit = string.match(line,
                '^%s*%@(%w+%s*){%s*(.+),')
    if key and allowed(key) then
      if not exists(result, hit) then
        hit = normalize(hit)
        table.insert(result, hit)
      end
    end
  end
  return result
end

-- Extracts the cross-references found
-- in lines of the bibligraphy file.
-- @param lines Line of a file.
-- @return Table containing cross-references.
local function crossref(lines)
  local result, lookup, key, hit = {}, ''
  for _, line in ipairs(lines) do
     key, hit = string.match(line,
                '^%s*%@(%w+%s*){%s*(.+),')
    if key and allowed(key) then
      lookup = normalize(hit)
    else
      key, hit = string.match(line,
                 '^%s*(%w+)%s*=%s*(.+)$')
      if key then
        key = string.lower(key)
        if key == 'crossref' then
          if string.sub(hit, -1) == ',' then
            hit = string.sub(hit, 2, -3)
          else
            hit = string.sub(hit, 2, -2)
          end
          result[lookup] = hit
        end
      end
    end
  end
  return result
end

-- Adds the extension if the file does not have it.
-- @param file File.
-- @param extension Extension.
-- @return File with proper extension.
local function sanitize(file, extension)
  extension = '.' .. extension
  if string.sub(file, -#extension) ~= extension then
    file = file .. extension
  end
  return file
end

-- Checks if a file exists.
-- @param file File.
-- @return Boolean value indicating if the file exists.
local function valid(file)
  local handler = io.open(file, 'r')
  if handler then
    handler:close()
    return true
  else
    return false
  end
end

-- Wraps a string based on a line width.
-- @param str String.
-- @param size Line width.
-- @return Wrapped string.
local function wrap(str, size)
  local parts = split(str, '[^%s]+')
  local r, l = '', ''
  for _, v in ipairs(parts) do
    if (#l + #v) > size then
      r = r .. '\n' .. l
      l = v
    else
      l = normalize(l .. ' ' .. v)
    end
  end
  r = normalize(r .. '\n' .. l)
  return r
end

-- Backend namespace
local backends = {}

-- Gets data from auxiliary files (BibTeX).
-- @param lines Lines of a file.
-- @param rec Recursive switch.
-- @return Boolean indicating if an asterisk was found.
-- @return Table containing the citations.
-- @return Table containing the bibliography files.
backends.bibtex = function(lines, rec)
  local citations, bibliography, invalid = {}, {}, {}
  local asterisk, parts, hit = false
  for _, line in ipairs(lines) do
    hit = string.match(line, '^%s*\\citation{(.+)}$')
    if hit then
      if hit ~= '*' then
        parts = split(hit, '[^,%s]+')
        for _, v in ipairs(parts) do
          v = normalize(v)
          if not exists(citations, v) then
            table.insert(citations, v)
          end
        end
      else
        asterisk = true
      end
    else
      hit = string.match(line, '^%s*\\bibdata{(.+)}$')
      if hit then
        parts = split(hit, '[^,%s]+')
        for _, v in ipairs(parts) do
          v = normalize(v)
          if not exists(bibliography, v) and
             not blacklist(v) then
            table.insert(bibliography, v)
          end
        end
      else
        hit = string.match(line, '^%s*\\@input{(.+)}$')
        if rec and hit then
          hit = sanitize(hit, 'aux')
          if not valid(hit) then
            table.insert(invalid, hit)
          else
            local a, b, c = backends.bibtex(read(hit), false)
            asterisk = asterisk or a
            for _, v in ipairs(b) do
              if not exists(citations, v) then
                table.insert(citations, v)
              end
            end
            for _, v in ipairs(c) do
              if not exists(bibliography, v) then
                table.insert(bibliography, v)
              end
            end
          end
        end
      end
    end
  end
  if #invalid ~= 0 then
    print()
    print(wrap('Warning: there ' .. plural(#invalid,
               'is an invalid reference ', 'are ' ..
               'invalid references ') .. 'to the ' ..
               'following auxiliary ' .. plural(#invalid,
               'file ', 'files ') .. 'that could not ' ..
               'be resolved at runtime:', 74))
    for _, v in ipairs(invalid) do
      print('=> ' .. v)
    end
  end
  return asterisk, citations, bibliography
end

-- Gets data from auxiliary files (Biber).
-- @param lines Lines of a file.
-- @param _ To be discarded with biber.
-- @return Boolean indicating if an asterisk was found.
-- @return Table containing the citations.
-- @return Table containing the bibliography files.
backends.biber = function(lines, _)
  local citations, bibliography = {}, {}
  local asterisk, parts, hit = false
  for _, line in ipairs(lines) do
    hit = string.match(line, '^%s*<bcf:citekey order="%d+">' ..
          '(.+)</bcf:citekey>$')
    if hit then
      if hit ~= '*' then
        parts = split(hit, '[^,%s]+')
        for _, v in ipairs(parts) do
          v = normalize(v)
          if not exists(citations, v) then
            table.insert(citations, v)
          end
        end
      else
        asterisk = true
      end
    else
      hit = string.match(line, '^%s*<bcf:datasource type="file" ' ..
            'datatype="%w+">(.+)</bcf:datasource>$')
      if hit then
        parts = split(hit, '[^,%s]+')
        for _, v in ipairs(parts) do
          v = normalize(v)
          if not exists(bibliography, v) and
             not blacklist(v) then
            table.insert(bibliography, v)
          end
        end
      end
    end
  end
  return asterisk, citations, bibliography
end

-- Counts the number of elements of a nominal table.
-- @param t Table.
-- @return Table size.
local function count(t)
  local counter = 0
  for _, _ in pairs(t) do
    counter = counter + 1
  end
  return counter
end

-- Repeats the provided char a certain number of times.
-- @param c Char.
-- @param size Number of times.
-- @return String with a char repeated a certain number of times.
local function pad(c, size)
  local r = c
  while #r < size do
    r = r .. c
  end
  return r
end

-- Flattens a table of tables into only one table.
-- @param t Table.
-- @return Flattened table.
local function flatten(t)
  local result = {}
  for _, v in ipairs(t) do
    for _, k in ipairs(v) do
      if not exists(result, k) then
        table.insert(result, k)
      end
    end
  end
  return result
end

-- Organizes a key/value table of tables into only one table.
-- @param t Table.
-- @return Flattened key/value table.
local function organize(t)
  local result = {}
  for _, v in ipairs(t) do
    for j, k in pairs(v) do
      if not result[j] then
        result[j] = k
      end
    end
  end
  return result
end

-- Applies a function to elements of a table.
-- @param c Table.
-- @param f Function.
-- @return A new table.
local function apply(c, f)
  local result = {}
  for _, v in ipairs(c) do
    table.insert(result, f(v))
  end
  return result
end

-- Search the TeX tree for the file.
-- @param library The library reference.
-- @param file The filename.
-- @param extension The extension.
-- @return String pointing to the file location.
local function lookup(library, file, extension)
  return library.find_file(file, extension)
end

-- Prints the script header.
local function header()
print("     _           _       _ _")
print(" ___| |_ ___ ___| |_ ___|_| |_ ___ ___")
print("|  _|   | -_|  _| '_|  _| |  _| -_|_ -|")
print("|___|_|_|___|___|_,_|___|_|_| |___|___|")
print()
  print(wrap('checkcites.lua -- a reference ' ..
             'checker script (v2.4)', 74))
  print(wrap('Copyright (c) 2012, 2019, ' ..
             'Enrico Gregorio, Paulo ' ..
             'Roberto Massa Cereda', 74))
end

-- Operation namespace
local operations = {}

-- Reports the unused references.
-- @param citations Citations.
-- @param references References.
-- @return Integer representing the status.
operations.unused = function(citations, references, crossrefs)
  print()
  print(pad('-', 74))
  print(wrap('Report of unused references in your TeX ' ..
             'document (that is, references present in ' ..
             'bibliography files, but not cited in ' ..
             'the TeX source file)', 74))
  print(pad('-', 74))

  local z = {}
  for _, citation in ipairs(citations) do
    if crossrefs[citation] then
      table.insert(z, crossrefs[citation])
    end
  end

  for _, i in ipairs(z) do
    if not exists(i, citations) then
      table.insert(citations, i)
    end
  end

  local r = difference(references, citations)
  print()
  print(wrap('Unused references in your TeX document: ' ..
             tostring(#r), 74))
  if #r == 0 then
    return 0
  else
    for _, v in ipairs(r) do
      print('=> ' .. v)
    end
    return 1
  end
end

-- Reports the undefined references.
-- @param citations Citations.
-- @param references References.
-- @return Integer value indicating the status.
operations.undefined = function(citations, references, crossrefs)
  print()
  print(pad('-', 74))
  print(wrap('Report of undefined references in your TeX ' ..
             'document (that is, references cited in the ' ..
             'TeX source file, but not present in the ' ..
             'bibliography files)', 74))
  print(pad('-', 74))

  local z = {}
  for _, citation in ipairs(citations) do
    if crossrefs[citation] then
      table.insert(z, crossrefs[citation])
    end
  end

  for _, i in ipairs(z) do
    if not exists(i, citations) then
      table.insert(citations, i)
    end
  end

  local r = difference(citations, references)
  print()
  print(wrap('Undefined references in your TeX document: ' ..
        tostring(#r), 74))
  if #r == 0 then
    return 0
  else
    for _, v in ipairs(r) do
      print('=> ' .. v)
    end
    return 1
  end
end

-- Reports both unused and undefined references.
-- @param citations Citations.
-- @param references References.
-- @return Integer value indicating the status.
operations.all = function(citations, references, crossrefs)
  local x, y
  x = operations.unused(citations, references, crossrefs)
  y = operations.undefined(citations, references, crossrefs)
  if x + y > 0 then
    return 1
  else
    return 0
  end
end

-- Filters a table of files, keeping the inexistent ones.
-- @param files Table.
-- @param lib Search library.
-- @param enabled Boolean switch to enable lookup.
-- @param extension Extension for lookup.
-- @return Table of inexistent files.
-- @return Table of existent files.
local function validate(files, lib, enabled, extension)
  local bad, good = {}, {}
  for _, v in ipairs(files) do
    if not valid(v) then
      if enabled and lookup(lib, v, extension) then
        table.insert(good, lookup(lib, v, extension))
      else
        table.insert(bad, v)
      end
    else
      table.insert(good, v)
    end
  end
  return bad, good
end

-- Main function.
-- @param args Command line arguments.
-- @return Integer value indicating the status
local function checkcites(args)

  local kpse = require('kpse')
  kpse.set_program_name('texlua')

  header()

  local parameters = {
    { short = 'a', long = 'all', argument = false },
    { short = 'u', long = 'unused', argument = false },
    { short = 'U', long = 'undefined', argument = false },
    { short = 'v', long = 'version', argument = false },
    { short = 'h', long = 'help', argument = false },
    { short = 'c', long = 'crossrefs', argument = false },
    { short = 'b', long = 'backend', argument = true }
  }

  local keys, err = parse(parameters, args)
  local check, backend = 'all', 'bibtex'

  if #err ~= 0 then
    print()
    print(pad('-', 74))
    print(wrap('I am sorry, but I do not recognize ' ..
               'the following ' .. plural(#err, 'option',
               'options') .. ':', 74))
    for _, v in ipairs(err) do
      print('=> ' .. v)
    end

    print()
    print(wrap('Please make sure to use the correct ' ..
               'options when running this script. You ' ..
               'can also refer to the user documentation ' ..
               'for a list of valid options. The script ' ..
               'will end now.', 74))
    return 1
  end

  if count(keys) == 0 then
    print()
    print(pad('-', 74))
    print(wrap('I am sorry, but you have not provided ' ..
               'any command line argument, including ' ..
               'files to check and options. Make ' ..
               'sure to invoke the script with the actual ' ..
               'arguments. Refer to the user documentation ' ..
               'if you are unsure of how this tool ' ..
               'works. The script will end now.', 74))
    return 1
  end

  if keys['version'] or keys['help'] then
    if keys['version'] then
      print()
      print(wrap('checkcites.lua, version 2.4 (dated September ' ..
                 '3, 2019)', 74))

      print(pad('-', 74))
      print(wrap('You can find more details about this ' ..
                 'script, as well as the user documentation, ' ..
                 'in the official source code repository:', 74))

      print()
      print('https://github.com/cereda/checkcites')

      print()
      print(wrap('The checkcites.lua script is licensed ' ..
                 'under the LaTeX Project Public License, ' ..
                 'version 1.3. The current maintainers ' ..
                 'are the original authors.', 74))
    else
      print()
      print(wrap('Usage: ' .. args[0] .. ' [ [ --all | --unused | ' ..
                 '--undefined ] [ --backend <arg> ] <file> [ ' ..
                 '<file 2> ... <file n> ] | --help | --version ' ..
                 ']', 74))

      print()
      print('-a,--all           list all unused and undefined references')
      print('-u,--unused        list only unused references in your bibliography files')
      print('-U,--undefined     list only undefined references in your TeX source file')
      print('-c,--crossrefs     enable cross-reference checks (disabled by default)')
      print('-b,--backend <arg> set the backend-based file lookup policy')
      print('-h,--help          print the help message')
      print('-v,--version       print the script version')

      print()
      print(wrap('Unless specified, the script lists all unused and ' ..
                 'undefined references by default. Also, the default ' ..
                 'backend is set to "bibtex". Please refer to the user ' ..
                 'documentation for more details.', 74))
    end
    return 0
  end

  if not keys['unpaired'] then
    print()
    print(pad('-', 74))
    print(wrap('I am sorry, but you have not provided ' ..
               'files to process. The tool requires ' ..
               'least one file in order to properly ' ..
               'work. Make sure to invoke the script ' ..
               'with an actual file (or files). Refer ' ..
               'to the user documentation if you are ' ..
               'unsure of how this tool works. The ' ..
               'script will end now.', 74))
    return 1
  end

  if keys['backend'] then
    if not exists({ 'bibtex', 'biber' }, keys['backend'][1]) then
      print()
      print(pad('-', 74))
      print(wrap('I am sorry, but you provided an ' ..
                 'invalid backend. I know two: ' ..
                 '"bibtex" (which is the default ' ..
                 'one) and "biber". Please make ' ..
                 'sure to select one of the two. ' ..
                 'Also refer to the user documentation ' ..
                 'for more information on how these ' ..
                 'backends work. The script will end ' ..
                 'now.', 74))
      return 1
    else
      backend = keys['backend'][1]
    end
  end

  if not keys['all'] then
    if keys['unused'] and keys['undefined'] then
      check = 'all'
    elseif keys['unused'] or keys['undefined'] then
      check = (keys['unused'] and 'unused') or
              (keys['undefined'] and 'undefined')
    end
  end

  local auxiliary = apply(keys['unpaired'], function(a)
                    return sanitize(a, (backend == 'bibtex'
                    and 'aux') or 'bcf') end)

  local invalid, _ = validate(auxiliary, kpse, false, 'aux')
  if #invalid ~= 0 then
    print()
    print(pad('-', 74))
    print(wrap('I am sorry, but I was unable to ' ..
               'locate ' .. plural(#invalid, 'this file',
               'these files')  .. ' (the extension ' ..
               'is automatically set based on the ' ..
               '"' .. backend .. '" backend):', 74))
    for _, v in ipairs(invalid) do
      print('=> ' .. v)
    end

    print()
    print(wrap('Selected backend: ' .. backend, 74))
    print(wrap('File lookup policy: add ".' ..
               ((backend == 'bibtex' and 'aux') or 'bcf') ..
               '" to files if not provided.', 74))

    print()
    print(wrap('Please make sure the ' .. plural(#invalid,
               'path is', 'paths are') .. ' ' ..
               'correct and the ' .. plural(#invalid,
               'file exists', 'files exist') ..  '. ' ..
               'There is nothing I can do at the moment. ' ..
               'Refer to the user documentation for ' ..
               'details on the file lookup. If ' .. plural(#invalid,
               'this is not the file', 'these are not the ' ..
               'files') .. ' you were expecting, ' ..
               'double-check your source file or ' ..
               'change the backend option when running ' ..
               'this tool. The script will end now.', 74))
    return 1
  end

  local lines = flatten(apply(auxiliary, read))
  local asterisk, citations, bibliography = backends[backend](lines, true)

  print()
  print(wrap('Great, I found ' .. tostring(#citations) .. ' ' ..
             plural(#citations, 'citation', 'citations') .. ' in ' ..
             tostring(#auxiliary) .. ' ' .. plural(#auxiliary, 'file',
             'files') ..'. I also found ' .. tostring(#bibliography) ..
             ' ' .. 'bibliography ' .. plural(#bibliography, 'file',
             'files') .. '. Let me check ' .. plural(#bibliography,
             'this file', 'these files') .. ' and extract the ' ..
             'references. Please wait a moment.', 74))

  if asterisk then
    print()
    print(wrap('Also, it is worth noticing that I found a mention to ' ..
               'a special "*" when retrieving citations. That means ' ..
               'your TeX document contains "\\nocite{*}" somewhere in ' ..
               'the source code. I will continue with the check ' ..
               'nonetheless.', 74))
  end

  bibliography = apply(bibliography, function(a)
                 return sanitize(a, 'bib') end)

  invalid, bibliography = validate(bibliography, kpse, true, 'bib')
  if #invalid ~= 0 then
    print()
    print(pad('-', 74))
    print(wrap('I am sorry, but I was unable to locate ' ..
               plural(#invalid, 'this file', 'these files') .. ' ' ..
               '(the extension is automatically set to ' ..
               '".bib", if not provided):', 74))
    for _, v in ipairs(invalid) do
      print('=> ' .. v)
    end

    print()
    print(wrap('Please make sure the ' .. plural(#invalid,
               'path is', 'paths are') .. ' ' ..
               'correct and the ' .. plural(#invalid,
               'file exists', 'files exist') ..  '. ' ..
               'There is nothing I can do at the moment. ' ..
               'Refer to to the user documentation ' ..
               'for details on bibliography lookup. If ' ..
               plural(#invalid, 'this is not the file',
               'these are not the files') .. ' you were ' ..
               'expecting (wrong bibliography), double-check ' ..
               'your source file. The script will end ' ..
               'now.', 74))
    return 1
  end

  local references = flatten(apply(bibliography, function(a)
                     return extract(read(a)) end))

  local crossrefs = (keys['crossrefs'] and organize(apply(bibliography,
                    function(a) return crossref(read(a)) end))) or {}

  print()
  print(wrap('Fantastic, I found ' .. tostring(#references) ..
             ' ' .. plural(#references, 'reference',
             'references') .. ' in ' .. tostring(#bibliography) ..
             ' bibliography ' .. plural(#bibliography, 'file',
             'files') .. '. Please wait a moment while the ' ..
             plural(((check == 'all' and 2) or 1), 'report is',
             'reports are') .. ' generated.', 74))

  return operations[check](citations, references, crossrefs)
end

-- Call and exit
os.exit(checkcites(arg))

-- EOF
