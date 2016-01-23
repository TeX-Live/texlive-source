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
  PunctumAuctusLineBL = true,
  PunctumLineBLBR = true,
  PunctumLineBR = true,
  PunctumLineTR = true,
  PunctumSmall = true,
  FlexusNobar = true,
  FlexusLineBL = true,
  FlexusAmOneLineBL = true,
  OriscusLineTR = true,
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
}

local GABC = {
  Accentus = [[\excluded{g}r1]],
  AccentusReversus = [[\excluded{g}r2]],
  Ancus = [[gec]],
  AncusLongqueue = [[hfd]],
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
  DivisioDominican = [[,3]],
  DivisioDominicanAlt = [[,4]],
  DivisioMaior = [[:]],
  DivisioMinima = [[,]],
  DivisioMinor = [[;]],
  FClef = [[f3]],
  FClefChange = [[f3]],
  Flat = [[gx]],
  FlatHole = [[\excluded{gx}]],
  Flexus = [[ge]],
  FlexusLongqueue = [[hf]],
  FlexusNobar = [[@hf]],
  FlexusOriscus = [[goe]],
  FlexusOriscusScapus = [[gOe]],
  FlexusOriscusScapusLongqueue = [[hOf]],
  LeadingOriscus = [[go\excluded{igig}]],
  LeadingPunctum = [[g\excluded{igig}]],
  LeadingQuilisma = [[gw\excluded{igig}]],
  Linea = [[g=]],
  LineaPunctum = [[gR]],
  LineaPunctumCavum = [[gr0]],
  LineaPunctumCavumHole = [[\excluded{gr0}]],
  Natural = [[gy]],
  NaturalHole = [[\excluded{gy}]],
  Oriscus = [[go]],
  OriscusCavum = [[gor]],
  OriscusCavumDeminutus = [[gor\~{}]],
  OriscusCavumDeminutusHole = [[\excluded{gor\~{}}]],
  OriscusCavumHole = [[\excluded{gor}]],
  OriscusCavumReversus = [[gor>]],
  OriscusCavumReversusHole = [[\excluded{gor>}]],
  OriscusLineBL = [[\excluded{e}@go]],
  OriscusReversus = [[go^^^^003c]],
  OriscusReversusLineTL = [[\excluded{i}@go]],
  OriscusScapus = [[gO]],
  OriscusScapusLongqueue = [[hO]],
  Pes = [[gi]],
  PesQuadratum = [[gqi]],
  PesQuadratumLongqueue = [[hqj]],
  PesQuassus = [[goi]],
  PesQuassusLongqueue = [[hoj]],
  PesQuilisma = [[gwi]],
  PesQuilismaQuadratum = [[gWi]],
  PesQuilismaQuadratumLongqueue = [[hWj]],
  Porrectus = [[geg]],
  PorrectusFlexus = [[gege]],
  PorrectusFlexusNobar = [[\excluded{e}gege]],
  PorrectusNobar = [[@geg]],
  Punctum = [[g]],
  PunctumCavum = [[gr]],
  PunctumCavumHole = [[\excluded{gr}]],
  PunctumCavumInclinatum = [[Gr]],
  PunctumCavumInclinatumAuctus = [[Gr>]],
  PunctumCavumInclinatumAuctusHole = [[\excluded{Gr>}]],
  PunctumCavumInclinatumHole = [[\excluded{Gr}]],
  PunctumInclinatum = [[G]],
  PunctumInclinatumAuctus = [[G>]],
  PunctumLineBL = [[\excluded{e}@g]],
  PunctumLineTL = [[\excluded{i}@g]],
  Quilisma = [[gw]],
  RoundBrace = '[ob:1;6mm]',
  RoundBraceDown = '[ub:1;6mm]',
  Salicus = [[giOk]],
  SalicusFlexus = [[giOki]],
  SalicusLongqueue = [[hjOl]],
  Scandicus = [[gik]],
  Semicirculus = [[\excluded{g}r4]],
  SemicirculusReversus = [[\excluded{g}r5]],
  Sharp = [[g\#{}]],
  SharpHole = [[\excluded{g\#{}}]],
  Stropha = [[gs]],
  StrophaAucta = [[gs>]],
  StrophaAuctaLongtail = [[hs>]],
  Torculus = [[gig]],
  TorculusLiquescens = [[gige]],
  TorculusLiquescensQuilisma = [[gwige]],
  TorculusQuilisma = [[gwig]],
  TorculusResupinus = [[gigi]],
  TorculusResupinusQuilisma = [[gwigi]],
  VEpisema = [[\excluded{g}^^^^0027]],
  Virga = [[gv]],
  VirgaLongqueue = [[hv]],
  VirgaReversa = [[gV]],
  VirgaReversaLongqueue = [[hV]],
  VirgaStrata = [[giO]],
  Virgula = [[^^^^0060]],
}

local GABC_FUSE = {
  Upper = {
    Punctum = [[\excluded{e}@]],
    Oriscus = [[\excluded{e}@]],
    Pes = [[\excluded{e}@]],
    PesQuadratum = [[\excluded{e}@]],
    PesQuadratumLongqueue = [[\excluded{f}@]],
    PesQuassus = [[\excluded{e}@]],
    PesQuassusLongqueue = [[\excluded{f}@]],
    Flexus = [[\excluded{e}@]],
  },
  Lower = {
    Punctum = [[\excluded{i}@]],
    Oriscus = [[\excluded{i}@]],
    Pes = [[\excluded{i}@]],
    PesQuadratum = [[\excluded{i}@]],
    PesQuadratumLongqueue = [[\excluded{j}@]],
    PesQuassus = [[\excluded{i}@]],
    Flexus = [[\excluded{i}@]],
    FlexusOriscus = [[\excluded{i}@]],
  },
  Up = {
    Punctum = [[\excluded{@ij}]],
    Oriscus = [[\excluded{@ij}]],
    OriscusScapus = [[\excluded{@ij}]],
    OriscusScapusLongqueue = [[\excluded{@jk}]],
    Quilisma = [[\excluded{@ij}]],
    Flexus = [[\excluded{@gi}]],
    FlexusNobar = [[\excluded{@hj}]],
  },
  Down = {
    Punctum = [[\excluded{@eg}]],
    Oriscus = [[\excluded{@eg}]],
    OriscusScapus = [[\excluded{@eg}]],
    OriscusScapusLongqueue = [[\excluded{@eg}]],
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

function GregorioRef.emit_score_glyphs(cs_greciliae, cs_gregorio, cs_parmesan)
  local common_glyphs = {}
  local greciliae = {}
  local gregorio = {}
  local parmesan = {}

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
  index_font(cs_parmesan, parmesan)

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
    local gabc = GABC[shape]
    if gabc then
      local fuse_head = ''
      local fuse_tail = ''
      if fusion ~= '' then
        fuse_head = GABC_FUSE[fusion][shape]
        if fuse_head == nil then
          tex.error('No head fusion for '..name)
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
    end
    tex.sprint(string.format(
        [[{\scriptsize %s{\bfseries %s}{\itshape %s}%s%s}&{\ttfamily\small %s}&{\%s\char%d}&{\%s\char%d}&{\%s\char%d}&]],
        fusion, shape, ambitus, debilis, liquescence, gabc or '', cs_greciliae, char, cs_gregorio, char, cs_parmesan, char
    ))
    local emitted = false, i, variant
    for i, variant in ipairs(sort_unique_keys{greciliae[name], gregorio[name], parmesan[name]}) do
      if emitted then
        tex.sprint([[\nopagebreak&&&&&]])
      else
        emitted = true
      end
      tex.sprint(string.format([[{\scriptsize %s}]], variant))
      maybe_emit_glyph('greciliae', greciliae, name, variant)
      maybe_emit_glyph('gregorio', gregorio, name, variant)
      maybe_emit_glyph('parmesan', parmesan, name, variant)
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
  local word = (majuscule * minuscule^0) - fusion - ambitus - debilis -
      post_word_liquescentia
  local liquescence = debilis^-1 * liquescentia^-1
  local pattern = C(fusion^-1) * C(word^1) * C(ambitus^0) * C(debilis^-1) *
      C(liquescentia^-1) * -1
  local only_twos = P'Two'^1 * -1
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
    if not EXCLUDE[name[1]] then
      if (name[3] == '' and name[5] == '') or name[3] == '' or only_twos:match(name[3]) then
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
