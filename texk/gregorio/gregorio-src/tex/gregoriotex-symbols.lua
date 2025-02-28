--GregorioTeX Symbols Lua support file.
--
--Copyright (C) 2016-2025 The Gregorio Project (see CONTRIBUTORS.md)
--
--This file is part of Gregorio.
--
--Gregorio is free software: you can redistribute it and/or modify
--it under the terms of the GNU General Public License as published by
--the Free Software Foundation, either version 3 of the License, or
--(at your option) any later version.
--
--Gregorio is distributed in the hope that it will be useful,
--but WITHOUT ANY WARRANTY; without even the implied warranty of
--MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--GNU General Public License for more details.
--
--You should have received a copy of the GNU General Public License
--along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

-- this file contains lua functions to support signs used by GregorioTeX.

-- GREGORIO_VERSION 6.1.0

local err = gregoriotex.module.err
local warn = gregoriotex.module.warn
local info = gregoriotex.module.info
local log = gregoriotex.module.log

local catcode_at_letter = luatexbase.catcodetables['gre@atletter']
local number_to_letter = gregoriotex.number_to_letter
local special_characters = {}
local next_special = 0

function gregoriotex.define_special_character(sequence)
  local substitution = special_characters[sequence]
  if substitution == nil then
    substitution = string.gsub(tostring(next_special), '[0-9]', number_to_letter)
    special_characters[sequence] = substitution
    next_special = next_special + 1
  end
  tex.print(catcode_at_letter,
      string.format([[\gdef\gre@special@%s]], substitution))
end

function gregoriotex.undefine_special_character(sequence)
  local substitution = special_characters[sequence]
  if substitution ~= nil then
    special_characters[sequence] = nil
    tex.print(catcode_at_letter,
        string.format([[\let\gre@special@%s\undefined\relax ]], substitution))
  end
end

function gregoriotex.special_character(sequence)
  local substitution = special_characters[sequence]
  if substitution ~= nil then
    tex.print(catcode_at_letter, string.format([[\gre@special@%s\relax ]], substitution))
  else
    tex.print(sequence)
  end
end
