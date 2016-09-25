-- Copyright (C) 2006-2016 The Gregorio Project (see CONTRIBUTORS.md)
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
  nonmarkingreturn = true,
  AscendensOriscusLineBLTR = true,
  AscendensOriscusLineTR = true,
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

local GABC = {
  Accentus = [[\excluded{g}r1]],
  AccentusReversus = [[\excluded{g}r2]],
  Ancus = [[gec]],
  AncusLongqueue = [[hfd]],
  AscendensOriscusCavum = [[go1r]],
  AscendensOriscusCavumHole = [[\excluded{gor}]],
  AscendensOriscus = [[go1]],
  AscendensOriscusLineBL = [[\excluded{e}@go1]],
  AscendensOriscusLineTL = [[\excluded{i}@go1]],
  AscendensOriscusScapus = [[gO1]],
  AscendensOriscusScapusLongqueue = [[hO1]],
  AscendensOriscusScapusOpenqueue = [[aO1]],
  AscendensPunctumInclinatum = [[G1]],
  AuctumMora = [[\excluded{g}.]],
  BarBrace = [[\excluded{,}\_]],
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
  DescendensOriscusCavum = [[go0r]],
  DescendensOriscusCavumHole = [[\excluded{go0r}]],
  DescendensOriscus = [[go0]],
  DescendensOriscusLineBL = [[\excluded{e}@go0]],
  DescendensOriscusLineTL = [[\excluded{i}@go0]],
  DescendensOriscusScapus = [[gO0]],
  DescendensOriscusScapusLongqueue = [[hO0]],
  DescendensOriscusScapusOpenqueue = [[aO0]],
  DescendensPunctumInclinatum = [[G0]],
  DivisioDominican = [[,3]],
  DivisioDominicanAlt = [[,4]],
  DivisioMaior = [[:]],
  DivisioMinima = [[,]],
  DivisioMinor = [[;]],
  FClefChange = [[f3]],
  FClef = [[f3]],
  Flat = [[gx]],
  FlatHole = [[\excluded{gx}]],
  Flexus = [[ge]],
  FlexusLongqueue = [[hf]],
  FlexusNobar = [[@hf]],
  FlexusOriscus = [[goe]],
  FlexusOriscusInusitatus = [[go1e]],
  FlexusOriscusScapus = [[gOe]],
  FlexusOriscusScapusInusitatus = [[gO1e]],
  FlexusOriscusScapusInusitatusLongqueue = [[hO1f]],
  FlexusOriscusScapusLongqueue = [[hOf]],
  LeadingOriscus = [[go\excluded{igig}]],
  LeadingPunctum = [[g\excluded{igig}]],
  LeadingQuilisma = [[gw\excluded{igig}]],
  Linea = [[g=]],
  LineaPunctumCavum = [[gr0]],
  LineaPunctumCavumHole = [[\excluded{gr0}]],
  LineaPunctum = [[gR]],
  Natural = [[gy]],
  NaturalHole = [[\excluded{gy}]],
  OblatusAscendensOriscus = [[go1]],
  OblatusDescendensOriscus = [[go0]],
  OblatusFlexusOriscus = [[goe]],
  OblatusFlexusOriscusInusitatus = [[go1e]],
  OblatusPesQuassus = [[goi]],
  OblatusPesQuassusLongqueue = [[hoj]],
  OblatusPesQuassusInusitatus = [[go0i]],
  OblatusPesQuassusInusitatusLongqueue = [[ho0j]],
  Oriscus = [[go]], -- for Deminutus
  OriscusCavum = [[gor]], -- for Deminutus
  OriscusCavumDeminutusHole = [[\excluded{gor\~{}}]],
  Pes = [[gi]],
  PesQuadratum = [[gqi]],
  PesQuadratumLongqueue = [[hqj]],
  PesQuassus = [[goi]],
  PesQuassusInusitatus = [[go0i]],
  PesQuassusInusitatusLongqueue = [[ho0j]],
  PesQuassusLongqueue = [[hoj]],
  PesQuilisma = [[gwi]],
  PesQuilismaQuadratum = [[gWi]],
  PesQuilismaQuadratumLongqueue = [[hWj]],
  PorrectusFlexus = [[gege]],
  PorrectusFlexusNobar = [[\excluded{e}gege]],
  Porrectus = [[geg]],
  PorrectusLongqueue = [[hfh]],
  PorrectusNobar = [[@geg]],
  PunctumCavum = [[gr]],
  PunctumCavumHole = [[\excluded{gr}]],
  PunctumCavumInclinatumAuctus = [[Gr>]],
  PunctumCavumInclinatumAuctusHole = [[\excluded{Gr>}]],
  PunctumCavumInclinatum = [[Gr]],
  PunctumCavumInclinatumHole = [[\excluded{Gr}]],
  Punctum = [[g]],
  PunctumInclinatum = [[G]], -- for deminutus
  PunctumInclinatumAuctus = [[G>]],
  PunctumLineBL = [[\excluded{e}@g]],
  PunctumLineTL = [[\excluded{i}@g]],
  Quilisma = [[gw]],
  RoundBraceDown = '[ub:1;6mm]',
  RoundBrace = '[ob:1;6mm]',
  SalicusFlexus = [[giOki]],
  Salicus = [[giOk]],
  SalicusLongqueue = [[hjOl]],
  Scandicus = [[gik]],
  Semicirculus = [[\excluded{g}r4]],
  SemicirculusReversus = [[\excluded{g}r5]],
  Sharp = [[g\#{}]],
  SharpHole = [[\excluded{g\#{}}]],
  StrophaAucta = [[gs>]],
  StrophaAuctaLongtail = [[hs>]],
  Stropha = [[gs]],
  Torculus = [[gig]],
  TorculusLiquescens = [[gige]],
  TorculusLiquescensQuilisma = [[gwige]],
  TorculusQuilisma = [[gwig]],
  TorculusResupinus = [[gigi]],
  TorculusResupinusQuilisma = [[gwigi]],
  VEpisema = [[\excluded{g}^^^^0027]],
  Virga = [[gv]],
  VirgaLongqueue = [[hv]],
  VirgaOpenqueue = [[av]],
  VirgaReversa = [[gV]],
  VirgaReversaLongqueue = [[hV]],
  VirgaReversaOpenqueue = [[aV]],
  VirgaStrata = [[giO]],
  Virgula = [[^^^^0060]],
}

local GABC_AMBITUS_ONE = {
  PorrectusLongqueue = [[hgh]],
  PorrectusFlexusLongqueue = [[hghg]],
  FlexusOpenqueue = [[ba]],
  FlexusOriscusScapusOpenqueue = [[bOa]],
  PesQuadratumOpenqueue = [[aqb]],
  PesQuassusOpenqueue = [[aob]],
  PesQuilismaQuadratumOpenqueue = [[aWb]],
  OblatusPesQuassusInusitatusOpenqueue = [[ao0b]],
  OblatusPesQuassusOpenqueue = [[boc]],
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

function GregorioRef.emit_score_glyphs(cs_greciliae, cs_gregorio, cs_granapadano)
  local common_glyphs = {}
  local greciliae = {}
  local gregorio = {}
  local granapadano = {}

  local function index_font(csname, variants, common)
    local glyphs = font.fonts[font.id(csname)].resources.unicodes
    -- force-load the code points of the font --
    local ignored = glyphs['___magic___']
    local glyph, cp
    for glyph, cp in pairs(glyphs) do
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

  index_font(cs_greciliae, greciliae, common_glyphs)
  index_font(cs_gregorio, gregorio)
  index_font(cs_granapadano, granapadano)

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
    tex.sprint(string.format(
        [[{\scriptsize %s{\bfseries %s}{\itshape %s}%s%s}&{\ttfamily\small %s}&{\%s\char%d}&{\%s\char%d}&{\%s\char%d}&]],
        fusion, shape, ambitus, debilis, liquescence, gabc or '', cs_greciliae, char, cs_gregorio, char, cs_granapadano, char
    ))
    local emitted = false, i, variant
    for i, variant in ipairs(sort_unique_keys{greciliae[name], gregorio[name], granapadano[name]}) do
      if emitted then
        tex.sprint([[\nopagebreak&&&&&]])
      else
        emitted = true
      end
      tex.sprint(string.format([[{\scriptsize %s}]], variant))
      maybe_emit_glyph('greciliae', greciliae, name, variant)
      maybe_emit_glyph('gregorio', gregorio, name, variant)
      maybe_emit_glyph('granapadano', granapadano, name, variant)
      tex.print([[\\]])
    end
    if not emitted then
      tex.print([[&&&\\]])
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
