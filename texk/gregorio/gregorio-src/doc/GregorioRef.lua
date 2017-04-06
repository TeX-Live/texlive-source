-- Copyright (C) 2006-2017 The Gregorio Project (see CONTRIBUTORS.md)
--
-- This file is part of Gregorio.
--
-- Gregorio is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- Gregorio is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

local P = lpeg.P
local R = lpeg.R
local C = lpeg.C

local function sort_keys(table_to_sort, compare)
  local sorted = {}, key
  for key in pairs(table_to_sort) do
    table.insert(sorted, key)
  end
  table.sort(sorted, compare)
  return sorted
end

local function sort_unique_keys(tables, compare)
  local set = {}, ignored, table_to_scan, key
  for ignored, table_to_scan in pairs(tables) do
    if table_to_scan then
      for key in pairs(table_to_scan) do
        set[key] = true
      end
    end
  end
  return sort_keys(set)
end 

local EXCLUDE = {
  ['.notdef'] = true,
  ['.null'] = true,
  notdef = true,
  nonmarkingreturn = true,
  AscendensOriscusLineBLTR = true,
  AscendensOriscusLineTR = true,
  BracketLeftZero = true,
  BracketLeftSix = true,
  BracketLeftSeven = true,
  BracketLeftEight = true,
  BracketLeftNine = true,
  BracketLeftTen = true,
  BracketLeftEleven = true,
  BracketLeftTwelve = true,
  BracketLeftThirteen = true,
  BracketLeftFourteen = true,
  BracketLeftShortZero = true,
  BracketLeftShortSix = true,
  BracketLeftShortSeven = true,
  BracketLeftShortEight = true,
  BracketLeftShortNine = true,
  BracketLeftShortTen = true,
  BracketLeftShortEleven = true,
  BracketLeftShortTwelve = true,
  BracketLeftShortThirteen = true,
  BracketLeftShortFourteen = true,
  BracketLeftLongZero = true,
  BracketLeftLongSix = true,
  BracketLeftLongSeven = true,
  BracketLeftLongEight = true,
  BracketLeftLongNine = true,
  BracketLeftLongTen = true,
  BracketLeftLongEleven = true,
  BracketLeftLongTwelve = true,
  BracketLeftLongThirteen = true,
  BracketLeftLongFourteen = true,
  BracketRightZero = true,
  BracketRightSix = true,
  BracketRightSeven = true,
  BracketRightEight = true,
  BracketRightNine = true,
  BracketRightTen = true,
  BracketRightEleven = true,
  BracketRightTwelve = true,
  BracketRightThirteen = true,
  BracketRightFourteen = true,
  BracketRightShortZero = true,
  BracketRightShortSix = true,
  BracketRightShortSeven = true,
  BracketRightShortEight = true,
  BracketRightShortNine = true,
  BracketRightShortTen = true,
  BracketRightShortEleven = true,
  BracketRightShortTwelve = true,
  BracketRightShortThirteen = true,
  BracketRightShortFourteen = true,
  BracketRightLongZero = true,
  BracketRightLongSix = true,
  BracketRightLongSeven = true,
  BracketRightLongEight = true,
  BracketRightLongNine = true,
  BracketRightLongTen = true,
  BracketRightLongEleven = true,
  BracketRightLongTwelve = true,
  BracketRightLongThirteen = true,
  BracketRightLongFourteen = true,
  PunctumAuctusLineBL = true,
  PunctumLineBLBR = true,
  PunctumLineBR = true,
  PunctumLineTR = true,
  PunctumSmall = true,
  FlexusLineBL = true,
  FlexusAmOneLineBL = true,
  DescendensOriscusLineTR = true,
  DescendensOriscusLineBLTR = true,
  QuilismaLineTR = true,
  VirgaLineBR = true,
  SalicusOriscus = true,
  ['Virgula.2'] = true,
  ['Virgula.3'] = true,
  ['Virgula.5'] = true,
  ['DivisioMinima.2'] = true,
  ['DivisioMinima.3'] = true,
  ['DivisioMinima.5'] = true,
  ['DivisioMinor.2'] = true,
  ['DivisioMinor.3'] = true,
  ['DivisioMinor.5'] = true,
  ['DivisioMaior.2'] = true,
  ['DivisioMaior.3'] = true,
  ['DivisioMaior.5'] = true,
  VirgaBaseLineBL = true,
}

-- &&& in the following two tables is a placeholder for the cavum shape 'r'

local GABC = {
  Accentus = [[\excluded{g}r1]],
  AccentusReversus = [[\excluded{g}r2]],
  Ancus = [[g&&&ec]],
  AncusLongqueue = [[h&&&fd]],
  AscendensOriscus = [[g&&&o1]],
  AscendensOriscusLineBL = [[\excluded{e}@g&&&o1]],
  AscendensOriscusLineTL = [[\excluded{i}@g&&&o1]],
  AscendensOriscusScapus = [[g&&&O1]],
  AscendensOriscusScapusLongqueue = [[h&&&O1]],
  AscendensOriscusScapusOpenqueue = [[a&&&O1]],
  AscendensPunctumInclinatum = [[G&&&1]],
  AuctumMora = [[\excluded{g}.]],
  BarBrace = [[\excluded{,}\_]],
  BracketLeft = [=[[[\excluded{ce]]}]=],
  BracketLeftShort = [=[[[\excluded{fh]]}]=],
  BracketLeftLong = [=[[[\excluded{gi]]}]=],
  BracketRight = [=[\excluded{[[ce}]]]=],
  BracketRightShort = [=[\excluded{[[fh}]]]=],
  BracketRightLong = [=[\excluded{[[gi}]]]=],
  CClef = [[c3]],
  CClefChange = [[c3]],
  Circulus = [[\excluded{g}r3]],
  CurlyBrace = '[ocb:1;6mm]',
  CustosDownLong = [[j+]],
  CustosDownMedium = [[m+]],
  CustosDownShort = [[k+]],
  CustosUpLong = [[f+]],
  CustosUpMedium = [[a+]],
  CustosUpShort = [[g+]],
  DescendensOriscus = [[g&&&o0]],
  DescendensOriscusLineBL = [[\excluded{e}@g&&&o0]],
  DescendensOriscusLineTL = [[\excluded{i}@g&&&o0]],
  DescendensOriscusScapus = [[g&&&O0]],
  DescendensOriscusScapusLongqueue = [[h&&&O0]],
  DescendensOriscusScapusOpenqueue = [[a&&&O0]],
  DescendensPunctumInclinatum = [[G&&&0]],
  DivisioDominican = [[,3]],
  DivisioDominicanAlt = [[,4]],
  DivisioMaior = [[:]],
  DivisioMinima = [[,]],
  DivisioMinor = [[;]],
  FClefChange = [[f3]],
  FClef = [[f3]],
  Flat = [[gx]],
  FlatHole = [[\excluded{gx}]],
  Flexus = [[g&&&e]],
  FlexusLongqueue = [[h&&&f]],
  FlexusNobar = [[@h&&&f]],
  FlexusOriscus = [[g&&&oe]],
  FlexusOriscusInusitatus = [[g&&&o1e]],
  FlexusOriscusScapus = [[g&&&Oe]],
  FlexusOriscusScapusInusitatus = [[g&&&O1e]],
  FlexusOriscusScapusInusitatusLongqueue = [[h&&&O1f]],
  FlexusOriscusScapusLongqueue = [[h&&&Of]],
  LeadingOriscus = [[g&&&o\excluded{igig}]],
  LeadingPunctum = [[g&&&\excluded{igig}]],
  LeadingQuilisma = [[g&&&w\excluded{igig}]],
  Linea = [[g&&&=]],
  LineaPunctum = [[g&&&R]],
  Natural = [[gy]],
  NaturalHole = [[\excluded{gy}]],
  OblatusAscendensOriscus = [[g&&&o1]],
  OblatusDescendensOriscus = [[g&&&o0]],
  OblatusFlexusOriscus = [[g&&&oe]],
  OblatusFlexusOriscusInusitatus = [[g&&&o1e]],
  OblatusPesQuassus = [[g&&&oi]],
  OblatusPesQuassusLongqueue = [[h&&&oj]],
  OblatusPesQuassusInusitatus = [[g&&&o0i]],
  OblatusPesQuassusInusitatusLongqueue = [[h&&&o0j]],
  Oriscus = [[g&&&o]], -- for Deminutus
  Pes = [[g&&&i]],
  PesAscendensOriscus = [[g&&&iO\excluded{/j}]],
  PesDescendensOriscus = [[g&&&iO\excluded{/h}]],
  PesQuadratum = [[g&&&qi]],
  PesQuadratumLongqueue = [[h&&&qj]],
  PesQuassus = [[g&&&oi]],
  PesQuassusInusitatus = [[g&&&o0i]],
  PesQuassusInusitatusLongqueue = [[h&&&o0j]],
  PesQuassusLongqueue = [[h&&&oj]],
  PorrectusFlexus = [[g&&&ege]],
  PorrectusFlexusNobar = [[\excluded{e}g&&&ege]],
  Porrectus = [[g&&&eg]],
  PorrectusLongqueue = [[h&&&fh]],
  PorrectusNobar = [[@g&&&eg]],
  Punctum = [[g&&&]],
  PunctumInclinatum = [[G&&&]], -- for deminutus
  PunctumInclinatumAuctus = [[G&&&>]],
  PunctumLineBL = [[\excluded{e}@g&&&]],
  PunctumLineTL = [[\excluded{i}@g&&&]],
  Quilisma = [[g&&&w]],
  QuilismaPes = [[g&&&wi]],
  QuilismaPesQuadratum = [[g&&&Wi]],
  QuilismaPesQuadratumLongqueue = [[h&&&Wj]],
  RoundBraceDown = '[ub:1;6mm]',
  RoundBrace = '[ob:1;6mm]',
  SalicusFlexus = [[g&&&iOki]],
  Salicus = [[g&&&iOk]],
  SalicusLongqueue = [[h&&&jOl]],
  Scandicus = [[g&&&ik]],
  Semicirculus = [[\excluded{g}r4]],
  SemicirculusReversus = [[\excluded{g}r5]],
  Sharp = [[g\#{}]],
  SharpHole = [[\excluded{g\#{}}]],
  StrophaAucta = [[g&&&s>]],
  StrophaAuctaLongtail = [[h&&&s>]],
  Stropha = [[g&&&s]],
  Torculus = [[g&&&ig]],
  TorculusLiquescens = [[g&&&ige]],
  TorculusLiquescensQuilisma = [[g&&&wige]],
  TorculusQuilisma = [[g&&&wig]],
  TorculusResupinus = [[g&&&igi]],
  TorculusResupinusQuilisma = [[g&&&wigi]],
  VEpisema = [[\excluded{g}^^^^0027]],
  Virga = [[g&&&v]],
  VirgaLongqueue = [[h&&&v]],
  VirgaOpenqueue = [[a&&&v]],
  VirgaReversa = [[g&&&V]],
  VirgaReversaLongqueue = [[h&&&V]],
  VirgaReversaOpenqueue = [[a&&&V]],
  Virgula = [[^^^^0060]],
}

local GABC_AMBITUS_ONE = {
  PorrectusLongqueue = [[h&&&gh]],
  PorrectusFlexusLongqueue = [[h&&&ghg]],
  FlexusOpenqueue = [[b&&&a]],
  FlexusOriscusScapusOpenqueue = [[b&&&Oa]],
  PesQuadratumOpenqueue = [[a&&&qb]],
  PesQuassusOpenqueue = [[a&&&ob]],
  QuilismaPesQuadratumOpenqueue = [[a&&&Wb]],
  OblatusPesQuassusInusitatusOpenqueue = [[a&&&o0b]],
  OblatusPesQuassusOpenqueue = [[b&&&oc]],
}

-- if the item is a table, the values will replace fuse_head and gabc
local GABC_FUSE = {
  Upper = {
    Punctum = [[\excluded{e}@]],
    AscendensOriscus = [[\excluded{e}@]],
    DescendensOriscus = [[\excluded{e}@]],
    OblatusAscendensOriscus = [[\excluded{f}@]],
    OblatusFlexusOriscusInusitatus = [[\excluded{f}@]],
    OblatusPesQuassus = [[\excluded{f}@]],
    OblatusPesQuassusLongqueue = [[\excluded{g}@]],
    OblatusPesQuassusOpenqueue = [[\excluded{a}@]],
    Pes = [[\excluded{e}@]],
    PesQuadratum = [[\excluded{e}@]],
    PesQuadratumLongqueue = [[\excluded{f}@]],
    PesQuadratumOpenqueue = { [[\excluded{a}@]], [[bqc]] },
    PesQuassus = [[\excluded{e}@]],
    PesQuassusInusitatus = [[\excluded{e}@]],
    PesQuassusInusitatusLongqueue = [[\excluded{f}@]],
    PesQuassusLongqueue = [[\excluded{f}@]],
    PesQuassusOpenqueue = { [[\excluded{a}@]], [[cod]] },
    Flexus = [[\excluded{e}@]],
    FlexusOriscus = [[\excluded{e}@]],
    FlexusOriscusInusitatus = [[\excluded{e}@]],
  },
  Lower = {
    Punctum = [[\excluded{i}@]],
    AscendensOriscus = [[\excluded{i}@]],
    DescendensOriscus = [[\excluded{i}@]],
    OblatusDescendensOriscus = [[\excluded{h}@]],
    OblatusFlexusOriscus = [[\excluded{h}@]],
    OblatusPesQuassusInusitatus = [[\excluded{h}@]],
    OblatusPesQuassusInusitatusLongqueue = [[\excluded{i}@]],
    OblatusPesQuassusInusitatusOpenqueue = [[\excluded{b}@]],
    Pes = [[\excluded{i}@]],
    PesQuadratum = [[\excluded{i}@]],
    PesQuadratumLongqueue = [[\excluded{j}@]],
    PesQuadratumOpenqueue = [[\excluded{b}@]],
    PesQuassus = [[\excluded{i}@]],
    PesQuassusInusitatus = [[\excluded{i}@]],
    PesQuassusInusitatusLongqueue = [[\excluded{j}@]],
    PesQuassusLongqueue = [[\excluded{j}@]],
    PesQuassusOpenqueue = [[\excluded{b}@]],
    Flexus = [[\excluded{i}@]],
    FlexusOriscus = [[\excluded{i}@]],
    FlexusOriscusInusitatus = [[\excluded{i}@]],
  },
  Up = {
    Punctum = [[\excluded{@ij}]],
    AscendensOriscus = [[\excluded{@ij}]],
    AscendensOriscusScapus = [[\excluded{@ij}]],
    AscendensOriscusScapusLongqueue = [[\excluded{@jk}]],
    DescendensOriscus = [[\excluded{@ij}]],
    DescendensOriscusScapus = [[\excluded{@ij}]],
    DescendensOriscusScapusLongqueue = [[\excluded{@jk}]],
    OblatusAscendensOriscus = [[\excluded{@i}]],
    OblatusDescendensOriscus = [[\excluded{@i}]],
    Quilisma = [[\excluded{@ij}]],
    Flexus = [[\excluded{@gi}]],
    FlexusNobar = [[\excluded{@hj}]],
  },
  Down = {
    Punctum = [[\excluded{@eg}]],
    AscendensOriscus = [[\excluded{@eg}]],
    AscendensOriscusScapus = [[\excluded{@eg}]],
    AscendensOriscusScapusLongqueue = [[\excluded{@eg}]],
    DescendensOriscus = [[\excluded{@eg}]],
    DescendensOriscusScapus = [[\excluded{@eg}]],
    DescendensOriscusScapusLongqueue = [[\excluded{@eg}]],
    OblatusAscendensOriscus = [[\excluded{@e}]],
    OblatusDescendensOriscus = [[\excluded{@e}]],
    VirgaReversa = [[\excluded{@eg}]],
    VirgaReversaLongqueue = [[\excluded{@fg}]],
  },
}

local DEBILIS = {
  InitioDebilis = [[-]],
  [''] = [[]],
}

local LIQUESCENCE = {
  Ascendens = [[<]],
  Descendens = [[>]],
  Deminutus = [[\~{}]],
  Nothing = [[]],
  [''] = [[]],
}

GregorioRef = {}

function GregorioRef.emit_score_glyphs(cs_normal, cs_hollow)
  local common_glyphs = {}
  local normal_variants = {}
  local normal_names = {}
  local hollow_variants = {}
  local hollow_names = {}

  local function index_font(csname, variants, names, common)
    local glyphs = font.fonts[font.id(csname)].resources.unicodes
    -- force-load the code points of the font --
    local ignored = glyphs['___magic___']
    local glyph, cp
    for glyph, cp in pairs(glyphs) do
      names[glyph] = true
      if cp >= 0xe000 and not EXCLUDE[glyph] and not glyph:match('^HEpisema') then
        local name, variant = glyph:match('^([^.]*)(%.%a*)$')
        if name then
          local glyph_variants = variants[name]
          if glyph_variants == nil then
            glyph_variants = {}
            variants[name] = glyph_variants
          end
          glyph_variants[variant] = cp
        elseif common then
          common[glyph] = cp
        end
      end
    end
  end

  index_font(cs_normal, normal_variants, normal_names, common_glyphs)
  index_font(cs_hollow, hollow_variants, hollow_names, common_glyphs)

  local function maybe_emit_glyph(csname, variants, name, variant)
    local cp = variants[name]
    if cp then
      cp = cp[variant]
      if cp then
        tex.sprint(string.format([[&{\%s\char%d}]], csname, cp))
      end
    end
    if not cp then
      tex.sprint(string.format([[&{\tiny\itshape N/A}]], csname, cp))
    end
  end

  local function emit_score_glyph(fusion, shape, ambitus, debilis, liquescence)
    local name = fusion..shape..ambitus..debilis..liquescence
    local char = common_glyphs[name]
    local gabc = GABC[shape] or GABC_AMBITUS_ONE[shape]
    if gabc then
      local fuse_head = ''
      local fuse_tail = ''
      if fusion ~= '' then
        fuse_head = GABC_FUSE[fusion][shape]
        if fuse_head == nil then
          tex.error('No head fusion for '..name)
        end
        if type(fuse_head) == 'table' then
          fuse_head, gabc = fuse_head[1], fuse_head[2]
        end
      end
      local liq = liquescence
      if liq == 'Up' or liq == 'Down' then
        fuse_tail = GABC_FUSE[liq][shape]
        if fuse_tail == nil then
          tex.error('No tail fusion for '..name)
        end
        liq = ''
      end
      gabc = '('..fuse_head..DEBILIS[debilis]..gabc..LIQUESCENCE[liq]..fuse_tail..')'
    else
      texio.write_nl('GregorioRef Warning: missing GABC for '..name)
    end
    local sorted_normal = sort_unique_keys{normal_variants[name]}
    local sorted_hollow = sort_unique_keys{hollow_variants[name]}
    local n = math.max(1, #sorted_normal, #sorted_hollow)
    local emitted = false, i, variant
    for i = 1,n do
      if emitted then
        tex.sprint([[\nopagebreak&&&]])
      else
        tex.sprint(string.format(
            [[{\scriptsize %s{\bfseries %s}{\itshape %s}%s%s}&{\ttfamily\small %s}&{\%s\char%d}&]],
            fusion, shape, ambitus, debilis, liquescence, gabc and gabc:gsub('&&&', '') or '', cs_normal, char
        ))
      end
      variant = sorted_normal[i]
      if variant then
        tex.sprint(string.format([[{\scriptsize %s}]], variant))
        maybe_emit_glyph(cs_normal, normal_variants, name, variant)
      else
        tex.print([[&]])
      end
      if emitted or not hollow_names[name] then
        tex.sprint([[&&&]])
      else
        tex.sprint(string.format(
            [[&{\ttfamily\small %s}&{\%s\char%d}&]],
            gabc and gabc:gsub('&&&', 'r') or '', cs_hollow, char
        ))
      end
      variant = sorted_hollow[i]
      if variant then
        tex.sprint(string.format([[{\scriptsize %s}]], variant))
        maybe_emit_glyph(cs_hollow, hollow_variants, name, variant)
      else
        tex.print([[&]])
      end
      tex.print([[\\]])
      emitted = true
    end
  end

  local glyph_names = {}
  local ambitus = P'One' + P'Two' + P'Three' + P'Four' + P'Five'
  local majuscule = R'AZ'
  local minuscule = R'az'
  local fusion = P'Upper' + P'Lower'
  local debilis = P'InitioDebilis'
  local post_word_liquescentia = P'Nothing' + P'Deminutus' + P'Ascendens' +
      P'Descendens'
  local liquescentia = post_word_liquescentia + P'Up' + P'Down'
  local word = ((majuscule * minuscule^0) - fusion - ambitus - debilis -
      post_word_liquescentia) + ((P'Ascendens' + P'Descendens') * P'Oriscus')
  local liquescence = debilis^-1 * liquescentia^-1
  local pattern = C(fusion^-1) * C(word^1) * C(ambitus^0) * C(debilis^-1) *
      C(liquescentia^-1) * -1
  local only_twos = P'Two'^1 * -1
  local ambitus_one = P'One' * P'Two'^0 * -1
  for name in pairs(common_glyphs) do
    local a, b, c, d, e = pattern:match(name)
    if b then
      table.insert(glyph_names, { a, b, c, d, e })
    else
      -- if parse fails, just use the name
      table.insert(glyph_names, { '', name, '', '', '' })
    end
  end
  local function compare(x, y)
    local nx = x[1]..x[2]
    local ny = y[1]..y[2]
    if nx < ny then
      return true
    elseif nx == ny then
      if x[4] < y[4] then
        return true
      elseif x[4] == y[4] then
        if x[5] < y[5] then
          return true
        elseif x[5] == y[5] and x[3] < y[3] then
          return true
        end
      end
    end
    return false
  end
  table.sort(glyph_names, compare)
  local first = true
  local i, name
  for i, name in ipairs(glyph_names) do
    if not EXCLUDE[name[2]] then
      if (name[3] == '' and name[5] == '') or name[3] == '' or only_twos:match(name[3])
          or (GABC_AMBITUS_ONE[name[2]] and ambitus_one:match(name[3])) then
        if first then
          first = false
        else
          tex.print([[\hline]])
        end
        emit_score_glyph(name[1], name[2], name[3], name[4], name[5])
      end
    end
  end
end

function GregorioRef.emit_extra_glyphs(csname)
  local glyphs = font.fonts[font.id(csname)].resources.unicodes
  local first = true
  local odd = true
  for i, name in ipairs(sort_keys(glyphs)) do
    local cp = glyphs[name]
    if cp >= 0xe000 and not EXCLUDE[name] then
      if first then
        first = false
      elseif odd then
        tex.print([[\hline]])
      end
      tex.sprint(string.format([[{\scriptsize %s}&{\%s\char%d}]], name, csname, cp))
      if odd then
        tex.sprint([[&]])
      else
        tex.print([[\\]])
      end
      odd = not odd
    end
  end
  if not odd then
    tex.print([[&\\]])
  end
end

function GregorioRef.emit_dimension(value)
  value = string.gsub(value, '(-?%d+%.%d+)%s*(%a+)', [[\unit[%1]{%2}]])
  value = string.gsub(value, '(-?%d+%.)%s*(%a+)', [[\unit[%1]{%2}]])
  value = string.gsub(value, '(-?%.?%d+)%s*(%a+)', [[\unit[%1]{%2}]])
  tex.sprint(value)
end
