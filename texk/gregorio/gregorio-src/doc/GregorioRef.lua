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
  PunctumLineBL = true,
  PunctumLineBLBR = true,
  PunctumLineBR = true,
  PunctumLineTL = true,
  PunctumLineTR = true,
  PunctumSmall = true,
  FlexusNobar = true,
  FlexusLineBL = true,
  FlexusAmOneLineBL = true,
  OriscusLineTR = true,
  QuilismaLineTR = true,
  VirgaLineBR = true,
  SalicusOriscus = true,
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
  OriscusReversus = [[go^^^^003c]],
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
  PorrectusNobar = [[\excluded{eo}geg]],
  Punctum = [[g]],
  PunctumCavum = [[gr]],
  PunctumCavumHole = [[\excluded{gr}]],
  PunctumInclinatum = [[G]],
  PunctumInclinatumAuctus = [[G>]],
  Quilisma = [[gw]],
  RoundBrace = '[ob:1;6mm]',
  RoundBraceDown = '[ub:1;6mm]',
  Salicus = [[giOk]],
  SalicusLongqueue = [[hjOl]],
  Scandicus = [[gik]],
  Semicirculus = [[\excluded{g}r4]],
  SemicirculusReversus = [[\excluded{g}r5]],
  Sharp = [[g\#{}]],
  SharpHole = [[\excluded{g\#{}}]],
  Stropha = [[gs]],
  StrophaAucta = [[gs>]],
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

  local function emit_score_glyph(shape, ambitus, debilis, liquescence)
    local name = shape..ambitus..debilis..liquescence
    local char = common_glyphs[name]
    local gabc = GABC[shape]
    if gabc then
      gabc = '('..DEBILIS[debilis]..gabc..LIQUESCENCE[liquescence]..')'
    end
    tex.sprint(string.format(
        [[{\scriptsize {\bfseries %s}{\itshape %s}%s%s}&{\ttfamily\small %s}&{\%s\char%d}&{\%s\char%d}&{\%s\char%d}&]],
        shape, ambitus, debilis, liquescence, gabc or '', cs_greciliae, char, cs_gregorio, char, cs_parmesan, char
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
  local debilis = P'InitioDebilis'
  local liquescentia = P'Nothing' + P'Deminutus' + P'Ascendens' + P'Descendens'
  local word = (majuscule * minuscule^0) - ambitus - debilis - liquescentia
  local liquescence = debilis^-1 * liquescentia^-1
  local pattern = C(word^1) * C(ambitus^0) * C(debilis^-1) * C(liquescentia^-1) * -1
  local only_twos = P'Two'^1 * -1
  for name in pairs(common_glyphs) do
    local a, b, c, d = pattern:match(name)
    if a then
      table.insert(glyph_names, { a, b, c, d })
    else
      tex.error('Unable to parse '..name)
    end
  end
  local function compare(x, y)
    if x[1] < y[1] then
      return true
    elseif x[1] == y[1] then
      if x[3] < y[3] then
        return true
      elseif x[3] == y[3] then
        if x[4] < y[4] then
          return true
        elseif x[4] == y[4] and x[2] < y[2] then
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
      if (name[2] == '' and name[4] == '') or name[2] == '' or only_twos:match(name[2]) then
        if first then
          first = false
        else
          tex.print([[\hline]])
        end
        emit_score_glyph(name[1], name[2], name[3], name[4])
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
