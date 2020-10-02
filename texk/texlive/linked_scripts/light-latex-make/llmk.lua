#!/usr/bin/env texlua

--
-- This is file `llmk.lua'.
--
-- Copyright 2018-2020 Takuto ASAKURA (wtsnjp)
--   GitHub:   https://github.com/wtsnjp
--   Twitter:  @wtsnjp
--
-- This sofware is released under the MIT License.
--

local llmk = {} -- the module table

----------------------------------------

do -- The "core" submodule
local M = {}

-- option flags (default)
M.debug = {
  config = false,
  parser = false,
  run = false,
  fdb = false,
  programs = false,
}
M.verbosity_level = 1
M.silent = false
M.dry_run = false

llmk.core = M
end

----------------------------------------

do -- The "const" submodule
local M = {}

-- program information
M.prog_name = 'llmk'
M.version = '0.2.0'
M.copyright = 'Copyright 2018-2020'
M.author = 'Takuto ASAKURA (wtsnjp)'
M.llmk_toml = 'llmk.toml'

-- exit codes
M.exit_ok = 0
M.exit_error = 1
M.exit_failure = 2
M.exit_parser = 3
M.exit_type = 4

-- config item specification
M.top_level_spec = {
  -- <program> = {<type>, <default value>}
  bibtex = {'string', 'bibtex'},
  clean_files = {'[string]', {
    '%B.aux', '%B.bbl', '%B.bcf', '%B-blx.bib', '%B.blg', '%B.fls',
    '%B.idx', '%B.ilg', '%B.log', '%B.out', '%B.run.xml', '%B.toc'
  }},
  clobber_files = {'[string]', {'%B.dvi', '%B.pdf', '%B.ps', '%B.synctex.gz'}},
  dvipdf = {'string', 'dvipdfmx'},
  dvips = {'string', 'dvips'},
  latex = {'string', 'lualatex'},
  llmk_version = {'string', nil},
  makeindex = {'string', 'makeindex'},
  max_repeat = {'integer', 5},
  ps2pdf = {'string', 'ps2pdf'},
  sequence = {'[string]', {'latex', 'bibtex', 'makeindex', 'dvipdf'}},
  source = {'*[string]', nil},
}

M.program_spec = {
  -- <item> = {<type>, {<specifiers allowed>, <default value>}}
  args = {'*[string]', {true, {'%T'}}},
  aux_file = {'string', {true, nil}},
  aux_empty_size = {'integer', {false, nil}},
  command = {'string', {false, ''}}, -- '' default because it must be string
  generated_target = {'boolean', {false, false}},
  opts = {'*[string]', {true, nil}},
  postprocess = {'string', {false, nil}},
  target = {'string', {true, '%S'}},
}

M.default_programs = {
  bibtex = {
    target = '%B.bib',
    args = {'%B'}, -- "%B.bib" will result in an error
    postprocess = 'latex',
  },
  dvipdf = {
    target = '%B.dvi',
    generated_target = true,
  },
  dvips = {
    target = '%B.dvi',
    generated_target = true,
  },
  latex = {
    opts = {
      '-interaction=nonstopmode',
      '-file-line-error',
      '-synctex=1',
    },
    aux_file = '%B.aux',
    aux_empty_size = 9, -- "\\relax \n" is empty
  },
  makeindex = {
    target = '%B.idx',
    generated_target = true,
    postprocess = 'latex',
  },
  ps2pdf = {
    target = '%B.ps',
    generated_target = true,
  },
}

llmk.const = M
end

----------------------------------------

do -- The "util" submodule
local M = {}

local function log(label, msg, ...)
  local prefix = llmk.const.prog_name .. ' ' .. label .. ': '
  io.stderr:write(prefix .. msg:format(...) .. '\n')
end

function M.err_print(err_type, msg, ...)
  if err_type == 'error' then
    -- error must be reported
  elseif err_type == 'info' then
    if llmk.core.verbosity_level < 2 then return end
  elseif err_type == 'warning' then
    if llmk.core.verbosity_level < 1 then return end
  end
  log(err_type, msg, ...)
end

function M.dbg_print(dbg_type, msg, ...)
  if llmk.core.debug[dbg_type] then
    log('debug-' .. dbg_type, msg, ...)
  end
end

function M.dbg_print_table(dbg_type, table)
  if not llmk.core.debug[dbg_type] then return end

  local function helper(tab, ind)
    local function pp(msg, ...)
      M.dbg_print(dbg_type, string.rep(' ', ind) .. msg, ...)
    end
    for k, v in pairs(tab) do
      if type(v) == 'table' then
        pp(k .. ':')
        helper(v, ind + 2)
      elseif type(v) == 'string' then
        pp(k .. ': "%s"', v)
      else -- number,  boolean, etc.
        pp(k .. ': %s', tostring(v))
      end
    end
  end

  helper(table, 2)
end

-- return the filename if exits, even if the ".tex" extension is omitted
-- otherwise return nil
local lfs = require("lfs")

-- Replace config param to filename
function M.replace_specifiers(str, source, target)
  local tmp = '/' .. source
  local basename = tmp:match('^.*/(.*)%..*$')

  str = str:gsub('%%S', source)
  str = str:gsub('%%T', target)

  if basename then
    str = str:gsub('%%B', basename)
  else
    str = str:gsub('%%B', source)
  end

  return str
end

llmk.util = M
end

----------------------------------------

do -- The "checker" submodule
local M = {}

local function checked_value(k, v, expected)
  local function error_if_wrong_type(val, t)
    if type(val) ~= t then
      llmk.util.err_print('error',
        '[Type Error] Key "%s" must have value of type %s', k, expected)
      os.exit(llmk.const.exit_type)
    end
  end

  if expected == 'integer' then
    error_if_wrong_type(v, 'number')
  elseif expected == 'boolean' then
    error_if_wrong_type(v, 'boolean')
  elseif expected == 'string' then
    error_if_wrong_type(v, 'string')
  elseif expected == '[string]' then
    error_if_wrong_type(v, 'table')

    if v[1] ~= nil then -- it is not an empty array
      error_if_wrong_type(v[1], 'string')
    end
  elseif expected == '*[string]' then
    if type(v) == 'string' then
      v = {v}
    else
      error_if_wrong_type(v, 'table')

      if v[1] ~= nil then -- it is not an empty array
        error_if_wrong_type(v[1], 'string')
      end
    end
  end

  return v
end

local function type_check(tab)
  local new_top = {}

  for k, v in pairs(tab) do
    if k == 'programs' then
      if type(v) ~= 'table' then
        llmk.util.err_print('error', '[Type Error] Key "programs" must be a table')
        os.exit(llmk.const.exit_type)
      end

      local new_prog = {}
      for p_name, p_val in pairs(v) do
        if type(p_val) ~= 'table' then
          llmk.util.err_print('error',
            '[Type Error] Key "programs.%s" must be a table', p_name)
          os.exit(llmk.const.exit_type)
        else
          new_prog[p_name] = {}
          for ik, iv in pairs(p_val) do
            if not llmk.const.program_spec[ik] then
              llmk.util.err_print('warning',
                'Program key "%s" is unknown; ignoring it', ik)
            else
              expected = llmk.const.program_spec[ik][1]
              new_prog[p_name][ik] = checked_value(ik, iv, expected)
            end
          end
        end
      end
      new_top[k] = new_prog
    else
      if not llmk.const.top_level_spec[k] then
        llmk.util.err_print('warning',
          'Top-level key "%s" is unknown; ignoring it', k)
      else
        expected = llmk.const.top_level_spec[k][1]
        new_top[k] = checked_value(k, v, expected)
      end
    end
  end

  return new_top
end

local function version_check(given_version)
  if not given_version then -- nothing to do
    return
  end

  -- parse the given version to the llmk_version key
  local given_major, given_minor = given_version:match('^(%d+)%.(%d+)')
  if not given_major or not given_minor then
    llmk.util.err_print('warning', 'In valid llmk_version: ' .. given_version)
    return
  else
    given_major, given_minor = tonumber(given_major), tonumber(given_minor)
  end

  -- the version of this program
  local major, minor = llmk.const.version:match('^(%d+)%.(%d+)')
  major, minor = tonumber(major), tonumber(minor)

  -- warn if this program is older than the given version
  if major < given_major or (major == given_major and minor < given_minor) then
    llmk.util.err_print('warning',
      'This program (v%d.%d) is older than the specified llmk_version (v%d.%d)',
      major, minor, given_major, given_minor)
  end

  -- Note: no breaking change has been made (yet)
end

function M.check(tab)
  local new_tab = type_check(tab)
  version_check(new_tab.llmk_version)
  return new_tab
end

llmk.checker = M
end

----------------------------------------

do -- The "config" submodule
local M = {}

local function init_config()
  local config = {}

  for k, v in pairs(llmk.const.top_level_spec) do
    config[k] = v[2]
  end

  config.programs = llmk.const.default_programs
  return config
end

-- copy command name from top level
local function fetch_from_top_level(config, name)
  if config.programs[name] then
    if not config.programs[name].command and config[name] then
      config.programs[name].command = config[name]
    end
  end
  return config
end

local function update_config(config, tab)
  -- merge the table from TOML
  local function merge_table(tab1, tab2)
    for k, v in pairs(tab2) do
      if k == 'programs' then
        local programs1 = tab1[k]
        local programs2 = tab2[k]

        for i_k, i_v in pairs(programs2) do
          if type(programs1[i_k]) == 'table' then
            for ii_k, ii_v in pairs(programs2[i_k]) do
              programs1[i_k][ii_k] = ii_v
            end
          else
            programs1[i_k] = i_v
          end
        end
      else
        tab1[k] = v
      end
    end
    return tab1
  end
  local config = merge_table(config, tab)

  -- set essential program names from top-level
  local prg_names = {'latex', 'bibtex', 'makeindex', 'dvipdf', 'dvips', 'ps2pdf'}
  for _, name in pairs(prg_names) do
    config = fetch_from_top_level(config, name)
  end

  -- show config table (for debug)
  llmk.util.dbg_print('config', 'The final config table is as follows:')
  llmk.util.dbg_print_table('config', config)

  return config
end

function M.fetch_from_latex_source(fn)
  local tab
  local config = init_config()

  -- get TOML field and parse it
  local toml, line = llmk.parser.get_toml(fn)
  if toml == '' then
    llmk.util.err_print('warning',
      'Neither TOML field nor magic comment is found in "%s"; ' ..
      'using default config', fn)
  end
  tab = llmk.parser.parse_toml(toml, {fn, line})

  -- check input and merge it to the config
  tab = llmk.checker.check(tab)
  config = update_config(config, tab)

  return config
end

function M.fetch_from_llmk_toml()
  local tab
  local config = init_config()

  local f = io.open(llmk.const.llmk_toml)
  if f ~= nil then
    local toml = f:read('*all')
    tab = llmk.parser.parse_toml(toml, {llmk.const.llmk_toml, 1})
    f:close()
  else
    llmk.util.err_print('error', 'No target specified and no %s found',
      llmk.const.llmk_toml)
    os.exit(llmk.const.exit_error)
  end

  -- check input and merge it to the config
  tab = llmk.checker.check(tab)
  config = update_config(config, tab)

  return config
end

llmk.config = M
end

----------------------------------------

do -- The "parser" submodule
--[[
This TOML parser is modified version of toml.lua
- Copyright 2017 Jonathan Stoler
- Licensed under MIT
  https://github.com/jonstoler/lua-toml/blob/master/LICENSE
]]
local M = {}

function M.parse_toml(toml, file_info)
  -- basic local variables
  local ws = '[\009\032]'
  local nl = '[\10\13\10]'

  local buffer = ''
  local cursor = 1

  local line = 0

  local res = {}
  local obj = res

  -- basic local functions
  local function parser_err(msg)
    local function get_toml_str(nol)
      local pattern = string.format('([^%s]*)%s', nl:sub(2, -2), nl)
      local l = 0
      for cur_line in toml:gmatch(pattern) do
        if l == nol then
          return cur_line
        end
        l = l + 1
      end
    end

    llmk.util.err_print('error', '[Parse Error] %s', msg)
    llmk.util.err_print('error', '--> %s:%d: %s',
      file_info[1], file_info[2] + line, get_toml_str(line))
    os.exit(llmk.const.exit_parser)
  end

  local function char(n)
    n = n or 0
    return toml:sub(cursor + n, cursor + n)
  end

  local function step(n)
    n = n or 1
    for i = 0, n-1 do
      if char(i):match(nl) then
        line = line + 1
      end
    end
    cursor = cursor + n
  end

  local function skip_ws()
    while(char():match(ws)) do
      step()
    end
  end

  local function trim(str)
    return str:gsub('^%s*(.-)%s*$', '%1')
  end

  local function bounds()
    return cursor <= toml:len()
  end

  -- parse functions for each type
  local function parse_string()
    -- TODO: multiline
    local del = char() -- ' or "
    local str = ''
    -- all available escape characters
    local escape = {
      b = "\b",
      t = "\t",
      n = "\n",
      f = "\f",
      r = "\r",
      ['"'] = '"',
      ["\\"] = "\\",
    }
    -- utf function from http://stackoverflow.com/a/26071044
    -- converts \uXXX into actual unicode
    local function utf(char)
      local bytemarkers = {{0x7ff, 192}, {0xffff, 224}, {0x1fffff, 240}}
      if char < 128 then return string.char(char) end
      local charbytes = {}
      for bytes, vals in pairs(bytemarkers) do
        if char <= vals[1] then
          for b = bytes + 1, 2, -1 do
            local mod = char % 64
            char = (char - mod) / 64
            charbytes[b] = string.char(128 + mod)
          end
          charbytes[1] = string.char(vals[2] + char)
          break
        end
      end
      return table.concat(charbytes)
    end

    -- skip the quotes
    step()

    while(bounds()) do
      -- end of string
      if char() == del then
        step()
        break
      end

      if char():match(nl) then
        parser_err('Single-line string cannot contain line break')
      end

      if del == '"' and char() == '\\' then -- process escape characters
        if escape[char(1)] then
          -- normal escape
          str = str .. escape[char(1)]
          step(2) -- go past backslash and the character
        elseif char(1) == 'u' then
          -- utf-16
          step()
          local uni = char(1) .. char(2) .. char(3) .. char(4)
          step(5)
          uni = tonumber(uni, 16)
          if (uni >= 0 and uni <= 0xd7ff) and not (uni >= 0xe000 and uni <= 0x10ffff) then
            str = str .. utf(uni)
          else
            parser_err('Unicode escape is not a Unicode scalar')
          end
        elseif char(1) == 'U' then
          -- utf-32
          step()
          local uni = char(1) .. char(2) .. char(3) .. char(4) ..
                      char(5) .. char(6) .. char(7) .. char(8)
          step(9)
          uni = tonumber(uni, 16)
          if (uni >= 0 and uni <= 0xd7ff) and not (uni >= 0xe000 and uni <= 0x10ffff) then
            str = str .. utf(uni)
          else
            parser_err('Unicode escape is not a Unicode scalar')
          end
        else
          parser_err('Invalid escape')
        end
      else -- literal string; leave as it is
        str = str .. char()
        step()
      end
    end

    return str
  end

  local function parse_number()
    -- TODO: exp, date
    local num = ''

    while(bounds()) do
      if char():match('[%+%-%.eE_0-9]') then
        if char() ~= '_' then
          num = num .. char()
        end
      elseif char():match(nl) then
        break
      elseif char():match(ws) or char() == '#' then
        break
      else
        parser_err('Invalid number')
      end
      step()
    end

    return tonumber(num)
  end

  local get_value

  local function parse_array()
    step()
    skip_ws()

    local a_type
    local array = {}

    while(bounds()) do
      if char() == ']' then
        break
      elseif char():match(nl) then
        step()
        skip_ws()
      elseif char() == '#' then
        while(bounds() and not char():match(nl)) do
          step()
        end
      else
        local v = get_value()
        if not v then break end

        if a_type == nil then
          a_type = type(v)
        elseif a_type ~= type(v) then
          parser_err('Mixed types in array')
        end

        array = array or {}
        table.insert(array, v)

        if char() == ',' then
          step()
        end
        skip_ws()
      end
    end
    step()

    return array
  end

  local function parse_boolean()
    local bool

    if toml:sub(cursor, cursor + 3) == 'true' then
      step(4)
      bool = true
    elseif toml:sub(cursor, cursor + 4) == 'false' then
      step(5)
      bool = false
    else
      parser_err('Invalid primitive')
    end

    skip_ws()
    if char() == '#' then
      while(not char():match(nl)) do
        step()
      end
    end

    return bool
  end

  -- judge the type and get the value
  get_value = function()
    if (char() == '"' or char() == "'") then
      return parse_string()
    elseif char():match('[%+%-0-9]') then
      return parse_number()
    elseif char() == '[' then
      return parse_array()
    -- TODO: array of table, inline table
    else
      return parse_boolean()
    end
  end

  -- main loop of parser
  while(cursor <= toml:len()) do
    -- ignore comments and whitespace
    if char() == '#' then
      while(not char():match(nl)) do
        step()
      end
    end

    if char() == '=' then
      step()
      skip_ws()

      -- prepare the key
      local key = trim(buffer)
      buffer = ''

      if key == '' then
        parser_err('Empty key name')
      elseif obj[key] then
        -- duplicate keys are not allowed
        parser_err('Cannot redefine key "' .. key .. '"')
      end

      local value = get_value()
      if value ~= nil then
        obj[key] = value
        --dbg_print('parser', 'Entry "' .. key .. ' = ' .. value .. '"')
      end

      -- skip whitespace and comments
      skip_ws()
      if char() == '#' then
        while(bounds() and not char():match(nl)) do
          step()
        end
      end

      -- if garbage remains on this line, raise an error
      if not char():match(nl) and cursor < toml:len() then
        parser_err('Invalid primitive')
      end

    elseif char() == '[' then
      buffer = ''
      step()
      local table_array = false

      if char() == '[' then
        table_array = true
        step()
      end

      obj = res

      local function process_key(is_last)
        is_last = is_last or false
        buffer = trim(buffer)

        if buffer == '' then
          parser_err('Empty table name')
        end

        if is_last and obj[buffer] and not table_array and #obj[buffer] > 0 then
          parser_err('Cannot redefine tabel')
        end

        if table_array then
          if obj[buffer] then
            obj = obj[buffer]
            if is_last then
              table.insert(obj, {})
            end
            obj = obj[#obj]
          else
            obj[buffer] = {}
            obj = obj[buffer]
            if is_last then
              table.insert(obj, {})
              obj = obj[1]
            end
          end
        else
          obj[buffer] = obj[buffer] or {}
          obj = obj[buffer]
        end
      end

      while(bounds()) do
        if char() == ']' then
          if table_array then
            if char(1) ~= ']' then
              parser_err('Mismatching brackets')
            else
              step()
            end
          end
          step()

          process_key(true)
          buffer = ''
          break
        --elseif char() == '"' or char() == "'" then
          -- TODO: quoted keys
        elseif char() == '.' then
          step()
          process_key()
          buffer = ''
        else
          buffer = buffer .. char()
          step()
        end
      end

      buffer = ''
    --elseif (char() == '"' or char() == "'") then
      -- TODO: quoted keys
    end

    -- put the char to the buffer and proceed
    buffer = buffer .. (char():match(nl) and '' or char())
    step()
  end

  return res
end

function M.get_toml(fn)
  local toml = ''
  local toml_field = false
  local toml_source = fn

  local f = io.open(toml_source)

  llmk.util.dbg_print('config', 'Looking for config in the file "%s"', toml_source)

  local ts_tmp
  local ts_latex
  local ts_bibtex

  local first_line = true
  local shebang

  local line = 0
  local start_pos = -1

  for l in f:lines() do
    line = line + 1

    -- 1. llmk-style TOML field
    if string.match(l, '^%s*%%%s*%+%+%++%s*$') then
      -- NOTE: only topmost field is valid
      if not toml_field then
        toml_field = true
        start_pos = line + 1
      else
        llmk.util.dbg_print('config', 'TOML field found')
        break
      end
    else
      if toml_field then
        toml = toml .. string.match(l, '^%s*%%%s*(.-)%s*$') .. '\n'
      end
    end

    -- 2. TeXShop directives
    ts_tmp = string.match(l, '^%s*%%%s*!%s*TEX%s+program%s*=%s*(.-)%s*$') or
             string.match(l, '^%s*%%%s*!%s*TEX%s+TS%-program%s*=%s*(.-)%s*$')
    if ts_tmp then
      ts_latex = ts_latex or ts_tmp
    end

    ts_tmp = string.match(l, '^%s*%%%s*!%s*BIB%s+program%s*=%s*(.-)%s*$') or
             string.match(l, '^%s*%%%s*!%s*BIB%s+TS%-program%s*=%s*(.-)%s*$')
    if ts_tmp then
      ts_bibtex = ts_bibtex or ts_tmp
    end

    -- 3. shebang
    if first_line then
      first_line = false
      shebang = string.match(l, '^%s*%%#!%s*(.-)%s*$')
    end
  end

  f:close()

  -- convert magic or shebang to TOML
  if toml == '' and (ts_latex or ts_bibtex) then
    llmk.util.dbg_print('config', 'TeXShop directives found')
    if ts_latex then
      toml = toml .. 'latex = "' .. ts_latex .. '"\n'
    end
    if ts_bibtex then
      toml = toml .. 'bibtex = "' .. ts_bibtex .. '"\n'
    end
  elseif toml == '' and shebang then
    llmk.util.dbg_print('config', 'Shebang found')
    toml = 'latex = "' .. shebang .. '"\n'
  end

  return toml, start_pos
end

llmk.parser = M
end

----------------------------------------

do -- The "runner" submodule
local M = {}

-- dependencies
local lfs = require 'lfs'
local md5 = require 'md5'

-- module local variable
local start_time = os.time()

local function table_copy(org)
  local copy
  if type(org) == 'table' then
    copy = {}
    for org_key, org_value in next, org, nil do
      copy[table_copy(org_key)] = table_copy(org_value)
    end
    setmetatable(copy, table_copy(getmetatable(org)))
  else -- number, string, boolean, etc.
    copy = org
  end
  return copy
end

local function setup_programs(fn, config)
  --[[Setup the programs table for each sequence.

  Collecting tables of only related programs, which appears in the
  `config.sequence` or `prog.postprocess`, and replace all specifiers.

  Args:
    fn (str): the input FILE name

  Returns:
    table of program tables
  ]]
  local prognames = {}
  local new_programs = {}
  local programs = config.programs

  -- collect related programs
  local function add_progname(name)
    -- is the program known?
    if not programs[name] then
      llmk.util.err_print('error', 'Unknown program "%s" is in the sequence', name)
      os.exit(llmk.const.exit_error)
    end

    -- if not new, no addition
    for _, c in pairs(prognames) do
      if c == name then
        return
      end
    end

    -- if new, add it!
    prognames[#prognames + 1] = name
  end

  for _, name in pairs(config.sequence) do
    -- add the program name
    add_progname(name)

    -- add postprocess program if any
    local postprocess = programs[name].postprocess
    if postprocess then
      add_progname(postprocess)
    end
  end

  -- setup the programs
  for _, name in ipairs(prognames) do
    local prog = table_copy(programs[name])

    -- setup the `prog.target`
    local cur_target

    if prog.target == nil then
      -- the default value of `prog.target` is `fn`
      cur_target = fn
    else
      -- here, %T should be replaced by `fn`
      cur_target = llmk.util.replace_specifiers(prog.target, fn, fn)
    end

    prog.target = cur_target

    -- initialize other items
    for k, v in pairs(llmk.const.program_spec) do
      if k ~= 'target' then -- target is a special case: already treated
        if prog[k] == nil then
          if type(v[2][2]) == 'table' then
            prog[k] = table_copy(v[2][2])
          else
            prog[k] = v[2][2]
          end
        end

        if v[2][1] then -- need to replace specifiers
          if type(prog[k]) == 'table' then
            for ik, iv in ipairs(prog[k]) do
              if type(prog[k][ik]) == 'string' then
                prog[k][ik] = llmk.util.replace_specifiers(iv, fn, cur_target)
              end
            end
          elseif type(prog[k]) == 'string' then
            prog[k] = llmk.util.replace_specifiers(prog[k], fn, cur_target)
          end
        end
      end
    end

    -- register the program
    new_programs[name] = prog
  end

  return new_programs
end

local function file_mtime(path)
  return lfs.attributes(path, 'modification')
end

local function file_size(path)
  return lfs.attributes(path, 'size')
end

local function file_md5sum(path)
  local f = assert(io.open(path, 'rb'))
  local content = f:read('*a')
  f:close()
  return md5.sumhexa(content)
end

local function file_status(path)
  return {
    mtime = file_mtime(path),
    size = file_size(path),
    md5sum = file_md5sum(path),
  }
end

local function init_file_database(programs, fn, config)
  -- the template
  local fdb = {
    targets = {},
    aux_files = {},
  }

  -- investigate current status
  for _, v in ipairs(config.sequence) do
    -- names
    local cur_target = programs[v].target
    local cur_aux = programs[v].aux_file

    -- target
    if lfs.isfile(cur_target) and not fdb.targets[cur_target] then
      fdb.targets[cur_target] = file_status(cur_target)
    end

    -- aux_file
    if cur_aux then -- `prog.aux_file` is optional
      if lfs.isfile(cur_aux) and not fdb.aux_files[cur_aux] then
        fdb.aux_files[cur_aux] = file_status(cur_aux)
      end
    end
  end

  return fdb
end

local function construct_cmd(prog, fn, target)
  -- construct the option
  local cmd_opt = ''

  if prog.opts then
    -- construct each option
    for _, opt in ipairs(prog.opts) do
      if #opt > 0 then
        cmd_opt = cmd_opt .. ' ' .. opt
      end
    end
  end

  -- construct the argument
  local cmd_arg = ''

  -- construct each argument
  for _, arg in ipairs(prog.args) do
    cmd_arg = cmd_arg .. ' "' .. arg .. '"'
  end

  -- whole command
  return prog.command .. cmd_opt .. cmd_arg
end

local function check_rerun(prog, fdb)
  llmk.util.dbg_print('run', 'Checking the neccessity of rerun')

  local aux = prog.aux_file
  local old_aux_exist = false
  local old_status

  -- if aux file does not exist, no chance of rerun
  if not aux then
    llmk.util.dbg_print('run', 'No auxiliary file specified')
    return false, fdb
  end

  -- if aux file does not exist, no chance of rerun
  if not lfs.isfile(aux) then
    llmk.util.dbg_print('run', 'The auxiliary file "%s" does not exist', aux)
    return false, fdb
  end

  -- copy old information and update fdb
  if fdb.aux_files[aux] then
    old_aux_exist = true
    old_status = table_copy(fdb.aux_files[aux])
  end
  local aux_status = file_status(aux)
  fdb.aux_files[aux] = aux_status

  -- if aux file is not new, no rerun
  local new = aux_status.mtime >= start_time
  if not new and old_aux_exist then
    new = aux_status.mtime > old_status.mtime
  end

  if not new then
    llmk.util.dbg_print('run', 'No rerun because the aux file is not new')
    return false, fdb
  end

  -- if aux file is empty (or almost), no rerun
  if aux_status.size < prog.aux_empty_size then
    llmk.util.dbg_print('run', 'No rerun because the aux file is (almost) empty')
    return false, fdb
  end

  -- if new aux is not different from older one, no rerun
  if old_aux_exist then
    if aux_status.md5sum == old_status.md5sum then
      llmk.util.dbg_print('run', 'No rerun because the aux file has not been changed')
      return false, fdb
    end
  end

  -- ok, then try rerun
  llmk.util.dbg_print('run', 'Try to rerun!')
  return true, fdb
end

local function silencer(cmd)
  local redirect_code
  if os.type == 'windows' then
    redirect_code = ' >NUL 2>&1'
  else
    redirect_code = ' >/dev/null 2>&1'
  end
  silencer = function(cmd) return cmd .. redirect_code end
  return silencer(cmd)
end

local function run_program(name, prog, fn, fdb, postprocess)
  -- preparation for dry run
  local function concat_cond(tab)
    local res
    for i, v in ipairs(tab) do
      if i == 1 then
        res = v
      else
        res = res .. '; ' .. v
      end
    end
    return res
  end
  local cond = {}

  if postprocess then
    cond[#cond + 1] = 'as postprocess'
  end

  if prog.aux_file then
    cond[#cond + 1] = 'possibly with rerunning'
  end

  -- does command specified?
  if #prog.command < 1 then
    llmk.util.err_print('warning',
      'The "command" key is not set for program "%s"; skipping', name)
    return false
  end

  -- does target exist?
  if not llmk.core.dry_run and not lfs.isfile(prog.target) then
    llmk.util.dbg_print('run',
      'Skiping "%s" because target (%s) does not exist',
      prog.command, prog.target)
    return false
  end

  -- is the target modified?
  if prog.generated_target then
    if llmk.core.dry_run then
      cond[#cond + 1] = string.format('if the target file "%s" has been generated',
        prog.target)
    elseif file_mtime(prog.target) < start_time then
      llmk.util.dbg_print('run',
        'Skiping "%s" because target (%s) is not updated',
        prog.command, prog.target)
      return false
    end
  else
    if llmk.core.dry_run then
      cond[#cond + 1] = string.format('if the target file "%s" exists', prog.target)
    end
  end

  local cmd = construct_cmd(prog, fn, prog.target)
  if llmk.core.dry_run then
    print('Dry running: ' .. cmd)
    if #cond > 0 then
      llmk.util.err_print('info', '<-- ' .. concat_cond(cond))
    end
    return false
  else
    llmk.util.err_print('info', 'Running command: ' .. cmd)
  end

  -- redirect stdout and stderr to NULL in silent mode
  if llmk.core.silent then
    cmd = silencer(cmd)
  end

  -- call and check the status
  local status = os.execute(cmd)
  if status > 0 then
    llmk.util.err_print('error',
      'Fail running %s (exit code: %d)', cmd, status)
    os.exit(llmk.const.exit_failure)
  end

  return true
end

local function process_program(programs, name, fn, fdb, config, postprocess)
  local postprocess = postprocess or false
  local prog = programs[name]
  local should_rerun

  -- execute the command
  local run = false
  local exe_count = 0
  while true do
    exe_count = exe_count + 1
    run = run_program(name, prog, fn, fdb, postprocess)

    -- if the run is skipped, break immediately
    if not run then break end

    -- if not neccesarry to rerun or reached to max_repeat, break the loop
    should_rerun, fdb = check_rerun(prog, fdb)
    if not ((exe_count < config.max_repeat) and should_rerun) then
      break
    end
  end

  -- go to the postprocess process
  if prog.postprocess and (run or llmk.core.dry_run) then
    llmk.util.dbg_print('run', 'Going to postprocess "%s"', prog.postprocess)
    process_program(programs, prog.postprocess, fn, fdb, config, true)
  end
end

function M.run_sequence(fn, config)
  llmk.util.err_print('info', 'Beginning a sequence for "%s"', fn)

  -- setup the programs table
  local programs = setup_programs(fn, config)
  llmk.util.dbg_print('programs', 'Current programs table:')
  llmk.util.dbg_print_table('programs', programs)

  -- create a file database
  local fdb = init_file_database(programs, fn, config)
  llmk.util.dbg_print('fdb', 'The initial file database is as follows:')
  llmk.util.dbg_print_table('fdb', fdb)

  for _, name in ipairs(config.sequence) do
    llmk.util.dbg_print('run', 'Preparing for program "%s"', name)
    process_program(programs, name, fn, fdb, config)
  end
end

llmk.runner = M
end

do -- The "cleaner" submodule
local M = {}

-- dependencies
local lfs = require("lfs")

-- fn is filepath of target to remove.
local function remove(fn)
  if llmk.core.dry_run then
    print(string.format('Dry running: removing file "%s"', fn))
  else
    local ok = os.remove(fn)

    if ok ~= true then
      llmk.util.err_print('error', 'Failed to remove "%s"', fn)
    else
      llmk.util.err_print('info', 'Removed "%s"', fn)
    end
  end
end

local function replace_spec_and_remove_files(fns, source)
  for _, fn in ipairs(fns) do
    local replaced_fn = llmk.util.replace_specifiers(fn, source, source)
    if lfs.isfile(replaced_fn) then
      remove(replaced_fn)
    end
  end
end

-- the actual process for the --clean action
function M.clean(fn, config)
  llmk.util.err_print('info', 'Begining cleaning for "%s"', fn)
  replace_spec_and_remove_files(config.clean_files, fn)
end

-- the actual process for the --clobber action
function M.clobber(fn, config)
  llmk.util.err_print('info', 'Begining clobbering for "%s"', fn)
  replace_spec_and_remove_files(config.clean_files, fn)
  replace_spec_and_remove_files(config.clobber_files, fn)
end

llmk.cleaner = M
end

----------------------------------------

do -- The "cli" submodule
local M = {}
local C = llmk.const

local help_text = [[
Usage: llmk [OPTION]... [FILE]...

Options:
  -c, --clean           Remove the temporary files such as aux and log files.
  -C, --clobber         Remove all generated files including final PDFs.
  -d CAT, --debug=CAT   Activate debug output restricted to CAT.
  -D, --debug           Activate all debug output (equal to "--debug=all").
  -h, --help            Print this help message.
  -n, --dry-run         Show what would have been executed.
  -q, --quiet           Suppress most messages.
  -s, --silent          Silence messages from called programs.
  -v, --verbose         Print additional information.
  -V, --version         Print the version number.

Please report bugs to <https://github.com/wtsnjp/llmk/issues>.
]]

local version_text = [[
%s %s

%s %s.
License: The MIT License <https://opensource.org/licenses/mit-license>.
This is free software: you are free to change and redistribute it.
]]

-- execution functions
local function read_options()
  local curr_arg
  local action = false

  -- modified Alternative Get Opt
  -- cf. http://lua-users.org/wiki/AlternativeGetOpt
  local function getopt(arg, options)
    local tmp
    local tab = {}
    local saved_arg = {table.unpack(arg)}
    for k, v in ipairs(saved_arg) do
      if string.sub(v, 1, 2) == '--' then
        table.remove(arg, 1)
        local x = string.find(v, '=', 1, true)
          if x then
            table.insert(tab, {string.sub(v, 3, x-1), string.sub(v, x+1)})
          else
            table.insert(tab, {string.sub(v, 3), true})
          end
      elseif string.sub(v, 1, 1) == '-' then
        table.remove(arg, 1)
        local y = 2
        local l = string.len(v)
        local jopt
        while (y <= l) do
          jopt = string.sub(v, y, y)
          if string.find(options, jopt, 1, true) then
            if y < l then
              tmp = string.sub(v, y+1)
              y = l
            else
              table.remove(arg, 1)
              tmp = saved_arg[k + 1]
            end
            if string.match(tmp, '^%-') then
              table.insert(tab, {jopt, false})
            else
              table.insert(tab, {jopt, tmp})
            end
          else
            table.insert(tab, {jopt, true})
          end
          y = y + 1
        end
      end
    end
    return tab
  end

  local opts = getopt(arg, 'd')
  for _, tp in pairs(opts) do
    k, v = tp[1], tp[2]
    if #k == 1 then
      curr_arg = '-' .. k
    else
      curr_arg = '--' .. k
    end

    -- action
    if (curr_arg == '-h') or (curr_arg == '--help') then
      return 'help' -- immediately show help
    elseif (curr_arg == '-V') or (curr_arg == '--version') then
      return 'version' -- immediately show version
    elseif (curr_arg == '-c') or (curr_arg == '--clean') then
      action = 'clean'      
    elseif (curr_arg == '-C') or (curr_arg == '--clobber') then
      action = 'clobber'
    -- debug
    elseif (curr_arg == '-D') or
      (curr_arg == '--debug' and (v == 'all' or v == true)) then
      for c, _ in pairs(llmk.core.debug) do
        llmk.core.debug[c] = true
      end
    elseif (curr_arg == '-d') or (curr_arg == '--debug') then
      if llmk.core.debug[v] == nil then
        llmk.util.err_print('warning', 'unknown debug category: ' .. v)
      else
        llmk.core.debug[v] = true
      end
    -- verbosity
    elseif (curr_arg == '-q') or (curr_arg == '--quiet') then
      llmk.core.verbosity_level = 0
    elseif (curr_arg == '-v') or (curr_arg == '--verbose') then
      llmk.core.verbosity_level = 2
    elseif (curr_arg == '-s') or (curr_arg == '--silent') then
      llmk.core.silent = true
    -- dry run
    elseif (curr_arg == '-n') or (curr_arg == '--dry-run') then
      llmk.core.dry_run = true
    -- problem
    else
      llmk.util.err_print('error', 'unknown option: ' .. curr_arg)
      os.exit(C.exit_error)
    end
  end

  return action
end

local function check_filename(fn)
  if lfs.isfile(fn) then
    return fn -- ok
  end

  local ext = fn:match('%.(.-)$')
  if ext ~= nil then
    return nil
  end

  local new_fn = fn .. '.tex'
  if lfs.isfile(new_fn) then
    return new_fn
  else
    return nil
  end
end

local function make(fns, func)
  local config
  if #fns > 0 then
    for _, fn in ipairs(fns) do
      local checked_fn = check_filename(fn)
      if checked_fn then
        config = llmk.config.fetch_from_latex_source(checked_fn)
        func(checked_fn, config)
      else
        llmk.util.err_print('error', 'Source file "%s" does not exist', fn)
        os.exit(C.exit_error)
      end
    end
  else
    config = llmk.config.fetch_from_llmk_toml()

    local source = config.source
    if source ~= nil then
      for _, fn in ipairs(source) do
        local checked_fn = check_filename(fn)
        if checked_fn then
          func(checked_fn, config)
        else
          llmk.util.err_print('error', 'Source file "%s" does not exist', fn)
          os.exit(C.exit_error)
        end
      end
    else
      llmk.util.err_print('error', 'No source detected')
      os.exit(C.exit_error)
    end
  end
end

local function do_action(action)
  if action == 'help' then
    io.stdout:write(help_text)
  elseif action == 'version' then
    io.stdout:write(version_text:format(
      C.prog_name, C.version, C.copyright, C.author))
  elseif action == 'clean' then
    make(arg, llmk.cleaner.clean)
  elseif action == 'clobber' then
    make(arg, llmk.cleaner.clobber)
  end
end

function M.exec()
  local action = read_options()

  if action then
    do_action(action)
    os.exit(C.exit_ok)
  end

  make(arg, llmk.runner.run_sequence)
  os.exit(C.exit_ok)
end

llmk.cli = M
end

----------------------------------------

assert(llmk.cli, 'Internal error: llmk is not installed properly')
llmk.cli.exec()

-- EOF
