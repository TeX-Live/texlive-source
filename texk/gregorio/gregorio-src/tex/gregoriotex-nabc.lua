--GregorioTeX Nabc Lua file.
--
--Copyright (C) 2014-2024 The Gregorio Project (see CONTRIBUTORS.md)
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

-- this file contains lua functions used by GregorioTeX St. Gall ancient
-- neume support when called with LuaTeX.

-- GREGORIO_VERSION 6.1.0

local catcode_at_letter = luatexbase.catcodetables['gre@atletter']

local gregallaliases = {
  ["ci~"] = "cl>",
  ["pe~"] = "ta>",
  ["vs~"] = "ta>",
  ["pf~"] = "po>",
  ["sc~"] = "pe>2",
  ["sa~"] = "pe>2",
  ["suu1"] = "ta-",
  ["ppu1"] = "ta-",
  ["pfG"] = "sfM",
  ["cl~"] = "vi>",
  ["pr~"] = "vs>",
  ["visu2"] = "ci",
  ["vi-su2"] = "ci-",
  ["visu3"] = "ci1",
  ["visu1sux1"] = "ci>",
  ["clSsut1"] = "ciG",
  ["clSsu2"] = "ciG1",
  ["visu1suw1"] = "ciM",
  ["vi-su1suw1"] = "ciM-",
  ["visu1sut1"] = "ciS",
  ["visut2"] = "ciS1",
  ["visu2lsc3"] = "cilsc3",
  ["vipp2"] = "sc",
  ["vi-ppt2"] = "sc-",
  ["vipp3"] = "sc1",
  ["vi>pp2"] = "sc>",
  ["peppt1"] = "scG",
  ["vippt2"] = "scS",
  ["ta>ppt1"] = "sc~",
  ["clpp2"] = "sf",
  ["cl-ppt2"] = "sf-",
  ["cl>pp2"] = "sf>",
  ["toppt1"] = "sfG",
  ["clMpp2"] = "sfM1",
  ["clppt2"] = "sfS",
  ["clpp2lsc2"] = "sflsc2",
  ["ciSppt1"] = "vippt1su1sut1",
  ["cippt1"] = "vippt1su2",
  ["cippt1lsc3"] = "vippt1su2lsc3",
  ["cippu1"] = "vippu1su2",
  ["cippu1lsc3"] = "vippu1su2lsc3",
  ["clppt1"] = "toG",
  ["pf!cl~"] = "cl!po>",
  ["pf!cl~ppt1"] = "cl!po>ppt1"
}

local function gregallreadfont(fontname, font_id)
  local tab = {}
  local metrics = {}
  local fontdata = font.getfont(font_id)
  -- The unicodes table may be lazy-loaded, so iterating it may not
  -- return everything.  Attempting to retrieve the code point of a
  -- glyph that has not already been loaded will trigger the __index
  -- method in the metatable (implemented by the fontloader) to load
  -- the table until the glyph is found.  When iterating the
  -- unicodes, we want the whole table to be filled, so we try to
  -- access a non-existing glyph in order to force load the entire
  -- table.
  local ignored = fontdata.resources.unicodes['_this_is_hopefully_a_nonexistent_glyph_']
  for key, value in pairs(fontdata.resources.unicodes) do
    local g = fontdata.characters[value]
    local name = key:gsub("B", ">"):gsub("N", "-"):gsub("E", "!"):gsub("T", "~")
    if g and (value >= 0xe400) and (value < 0xf000) then
      tab[name] = "\\char" .. value
      metrics[name] = { width = g.width, height = g.height, depth = (g.depth and g.depth or 0) }
    end
  end
  if fontname ~= "grelaon" then
    for key, value in pairs(gregallaliases) do
      if tab[value] and not tab[key] then
        tab[key] = tab[value]
        metrics[key] = metrics[value]
      end
    end
  end
  return tab, metrics
end

local gregalltab = {}
local gregallmetrics = {}

local gregallneumekinds = { vi = 1, pu = 1, ta = 1, gr = 1, cl = 1, un = 1, pv = 1, pe = 1, po = 1, to = 1, ci = 1, sc = 1, pf = 1, sf = 1, tr = 1,
  st = 1, ds = 1, ts = 1, tg = 1, bv = 1, tv = 1, pr = 1, pi = 1, vs = 1, ["or"] = 1, sa = 1, pq = 1, qi = 1, ql = 1, pt = 1,
  un = 1, oc = 1, ni = 1 }
local gregalllskinds = { c = 1, t = 1, s = 1, l = 1, x = 1, ["+"] = 1, a = 1, al = 1, am = 1, b = 1, cm = 1, co = 1, cw = 1, d = 1, e = 1, eq = 1,
  ew = 1, f = 1, fid = 1, fr = 1, g = 1, h = 1, hp = 1, hn = 1, i = 1, im = 1, iv = 1, k = 1, lb = 1, lc = 1, len = 1,
  lm = 1, lp = 1, lt = 1, m = 1, md = 1, moll = 1, n = 1, nl = 1, nt = 1, p = 1, par = 1, pfec = 1, pm = 1, q = 1,
  sb = 1, sc = 1, sc = 1, simil = 1, simul = 1, sj = 1, sjc = 1, sjcm = 1, sm = 1, st = 1, sta = 1, su = 1, tb = 1,
  th = 1, tm = 1, tw = 1, v = 1, ve = 1, vol = 1,
  ["eq-"] = 1, equ = 1, simp = 1, simpl = 1, sp = 1 }
local grelaonltkinds = { i = 1, ["do"] = 1, dr = 1, dx = 1, ps = 1, qm = 1, sb = 1, se = 1, sj = 1, sl = 1, sn = 1, sp = 1, sr = 1, st = 1, us = 1 }

-- Parse a single base neume
local gregallparse_base = function (str, idx, len)
  local ret = str:sub(idx, idx + 1)
  local alt = {}
  local height = 5
  local alts = "MSG-><~"
  if idx >= len or not gregallneumekinds[ret] then return 1 end
  idx = idx + 2
  -- The alternation modifiers can be written in arbitrary order,
  -- canonicalize it and remove duplicates.
  while idx <= len and alts:find(str:sub(idx, idx)) do
    alt[str:sub(idx, idx)] = 1
    idx = idx + 1
  end
  for i = 1, 7 do
    local c = alts:sub(i, i)
    if alt[c] then ret = ret .. c end
  end
  -- This is followed by a single optional variant digit.
  if idx <= len and string.find("123456789", str:sub(idx, idx)) then
    ret = ret .. str:sub(idx, idx)
    idx = idx + 1
  end
  -- Ambitus not handled yet, neither during parsing, nor when
  -- typesetting.
  -- Optional height, h[a-np].
  if idx < len and str:sub(idx, idx) == "h" then
    local p = string.find("abcdefghijklmnp", str:sub(idx + 1, idx + 1))
    if not p then return 1 end
    height = p - 1
    idx = idx + 2
  end
  return 0, idx, ret, height
end

-- Parse one neume, which is one base neume or several base neumes
-- separated with ! characters, and all this followed by arbitrary
-- ls, pp and su modifiers.
local gregallparse_neume = function (str, idx, len)
  local err
  local bases = {}
  local heights = {}
  local pp = ''
  local su = ''
  local ls = {}
  local lsidx = 0
  local i = 1
  err, idx, bases[0], heights[0] = gregallparse_base(str, idx, len)
  if err == 1 then return 1 end
  while idx <= len and str:sub(idx, idx) == "!" do
    err, idx, bases[i], heights[i] = gregallparse_base(str, idx + 1, len)
    if err == 1 then return 1 end
    i = i + 1
  end
  while idx < len do
    local v = str:sub(idx, idx + 1)
    if v == "ls" then
      local idx2 = idx + 2
      while idx2 <= len and not string.find("123456789", str:sub(idx2, idx2)) do
        idx2 = idx2 + 1
      end
      if idx2 > len or not gregalllskinds[str:sub(idx + 2, idx2 - 1)] then return 1 end
      ls[lsidx] = str:sub(idx, idx2)
      lsidx = lsidx + 1
      idx = idx2 + 1
    elseif v == "lt" then
      local idx2 = idx + 2
      while idx2 <= len and not string.find("123456789", str:sub(idx2, idx2)) do
        idx2 = idx2 + 1
      end
      if idx2 > len or not grelaonltkinds[str:sub(idx + 2, idx2 - 1)] then return 1 end
      ls[lsidx] = str:sub(idx, idx2)
      lsidx = lsidx + 1
      idx = idx2 + 1
    elseif v == "su" or v == "pp" then
      local mod = ''
      idx = idx + 2
      local c = str:sub(idx, idx)
      if idx <= len and string.find("tuvwxyqnz", c) then
        mod = mod .. c
        idx = idx + 1
        c = str:sub(idx, idx)
      end
      -- Pre/subpuncta with height not supported yet
      -- the heights would need to be adjusted relatively to heights[0]
      if idx > len or not string.find("123456789", c) then return 1 end
      mod = mod .. c
      if v == "su" then su = su .. "su" .. mod else pp = pp .. "pp" .. mod end
      idx = idx + 1
    else break end
  end
  return 0, idx, bases, heights, ls, pp, su
end

local add_ls = function(base, pre, post, ls, position, glyphbox, lsbox, baseraise, lwidths, curlwidths, scale)
  local raise = 0
  if position == 3 or position == 6 or position == 9 then
    if position == 3 then
      raise = glyphbox.height - (lsbox.height-lsbox.depth)/2 + baseraise
    elseif position == 6 then
      raise = (glyphbox.height-glyphbox.depth)/2 - (lsbox.height+lsbox.depth)/2 + baseraise
    else
      raise = -glyphbox.depth - (lsbox.height+lsbox.depth)/2 + baseraise
    end
    local kern1 = curlwidths[position] - lwidths[12]
    curlwidths[position] = curlwidths[position] + lsbox.width
    local kern2 = lwidths[12] - curlwidths[position]
    if curlwidths[12] == 0 then
      curlwidths[12] = 1
      kern1 = 0
    end
    kern1 = kern1 * scale
    kern2 = kern2 * scale
    raise = raise * scale
    return base, pre, post..'\\kern '..string.format("%.3f",kern1)..'sp\\raise '..string.format("%.3f",raise)..'sp\\hbox{'..ls..'}\\kern '..string.format("%.3f",kern2)..'sp'
  elseif position == 2 or position == 8 then
    if position == 2 then
      raise = glyphbox.height + lsbox.depth + baseraise
    else
      raise = -glyphbox.depth - lsbox.height + baseraise
    end
    local kern1 = -math.max(glyphbox.width, lwidths[11])/2 - lwidths[position]/2 + curlwidths[position]
    curlwidths[position] = curlwidths[position] + lsbox.width
    local kern2 = - kern1 - lsbox.width
    if curlwidths[11] == 0 and glyphbox.width < lwidths[11] then
      curlwidths[11] = 1
      base = '\\kern '..string.format("%.3f",(lwidths[11] - glyphbox.width)/2 * scale)..'sp'..base
      kern1 = kern1 + (lwidths[11] - glyphbox.width)/2
    end
    kern1 = kern1 * scale
    kern2 = kern2 * scale
    raise = raise * scale
    return base..'\\kern '..string.format("%.3f",kern1)..'sp\\raise '..string.format("%.3f",raise)..'sp\\hbox{'..ls..'}\\kern '..string.format("%.3f",kern2)..'sp', pre, post
  else
    if position == 1 then
      raise = glyphbox.height - (lsbox.height-lsbox.depth)/2 + baseraise
    elseif position == 4 then
      raise = (glyphbox.height-glyphbox.depth)/2 - (lsbox.height+lsbox.depth)/2 + baseraise
    else
      raise = -glyphbox.depth - (lsbox.height+lsbox.depth)/2 + baseraise
    end
    local kern1 = curlwidths[position] - lwidths[position]
    curlwidths[position] = curlwidths[position] + lsbox.width
    local kern2 = lwidths[position] - curlwidths[position]
    if curlwidths[10] == 0 then
      curlwidths[10] = 1
      kern1 = lwidths[10] - lwidths[position]
    end
    kern1 = kern1 * scale
    kern2 = kern2 * scale
    raise = raise * scale
    return base, pre..'\\kern '..string.format("%.3f",kern1)..'sp\\raise '..string.format("%.3f",raise)..'sp\\hbox{'..ls..'}\\kern '..string.format("%.3f",kern2)..'sp', post
  end
end

local add_spacing = function(str, len, idx, ret)
  while idx <= len and (str:sub(idx, idx) == "/" or str:sub(idx, idx) == "`") do
    if idx < len and str:sub(idx, idx + 1) == "//" then
      ret = ret .. "\\gre@hskip \\gre@space@skip@nabclargerspace"
      idx = idx + 2
    elseif idx < len and str:sub(idx, idx + 1) == "``" then
      ret = ret .. "\\gre@hskip -\\gre@space@skip@nabclargerspace"
      idx = idx + 2
    elseif str:sub(idx, idx) == "/" then
      ret = ret .. "\\gre@hskip \\gre@space@skip@nabcinterelementspace"
      idx = idx + 1
    else
      ret = ret .. "\\gre@hskip -\\gre@space@skip@nabcinterelementspace"
      idx = idx + 1
    end
  end
  return idx, ret
end

local gregallparse_neumes = function(str, kind, scale)
  local len = str:len()
  local idx = 1
  local ret = ''
  idx, ret = add_spacing(str, len, idx, ret)
  while idx <= len do
    local err, bases, heights, ls, pp, su, lscount
    err, idx, bases, heights, ls, pp, su = gregallparse_neume (str, idx, len)
    if err == 1 then return ret .. "ERR" end
    local base = bases[0]
    local i = 1
    while bases[i] do
      base = base .. "!" .. bases[i]
      local h = heights[i] - heights[0]
      if h ~= 0 then
        h = h + 5
        if h < 0 or h > 14 then
          base = "ERR"
          break
        end
        base = base .. string.sub("abcdefghijklmnp", h + 1, h + 1)
      end
      i = i + 1
    end
    local ls5 = ''
    lscount = 0
    while ls[lscount] do
      if not gregalltab[kind][ls[lscount]:sub(1, -2)] then base = "ERR" end
      if tonumber(ls[lscount]:sub(-1, -1)) == 5 then
        ls5 = ls5 .. ls[lscount]
        ls[lscount] = ''
      end
      lscount = lscount + 1
    end
    if base ~= "ERR" then
      local l = {}
      function l.try (kind, base, parts, pp, su, ls5, ls)
        if parts == 2 and pp ~= '' and su ~= '' and gregalltab[kind][base .. pp .. su .. ls5 .. ls] then return base .. pp .. su .. ls5 .. ls, '', '' end
        -- Prefer subpunctis over prepunctis.
        if parts == 1 and su ~= '' and gregalltab[kind][base .. su .. ls5 .. ls] then return base .. su .. ls5 .. ls, pp, '' end
        if parts == 1 and pp ~= '' and gregalltab[kind][base .. pp .. ls5 .. ls] then return base .. pp .. ls5 .. ls, '', su end
        -- Prefer subpunctis over significative letters.
        if parts == 0 and su ~= '' and ls ~= '' and gregalltab[kind][base .. su .. ls5] then return nil, pp, su end
        if parts == 0 and gregalltab[kind][base .. ls5 .. ls] then return base .. ls5 .. ls, pp, su end
        return nil, pp, su
      end
      local r = nil
      local ppsuparts = 0
      if pp ~= '' then ppsuparts = 1 end
      if su ~= '' then ppsuparts = ppsuparts + 1 end
      -- We assume here no character in the font has more than three
      -- significative letters.  Significative letters with position 5
      -- are always required to be in font and have preference over
      -- pre/subpunctis and other significative letters.
      local allparts = ppsuparts + lscount
      if lscount >= 3 then allparts = ppsuparts + 3 end
      -- Try to match as many parts (ls sequences, pp string, su string) as possible
      -- except that for ls accept any of ls sequences only if we have all of them.
      for parts = allparts, 0, -1 do
        if lscount == 3 and parts >= 3 and parts <= 3 + ppsuparts then
          r, pp, su = l.try(kind, base, parts - 3, pp, su, ls5, ls[0] .. ls[1] .. ls[2])
          if (not r) and ls[0] ~= '' and ls[1] ~= '' and ls[2] ~= '' then
            local p0 = tonumber(ls[0]:sub(-1, -1))
            local p1 = tonumber(ls[1]:sub(-1, -1))
            local p2 = tonumber(ls[2]:sub(-1, -1))
            if (p0 ~= p1) and (p0 ~= p2) and (p1 ~= p2) then
              r, pp, su = l.try(kind, base, parts - 3, pp, su, ls5, ls[1] .. ls[0] .. ls[2])
              if not r then r, pp, su = l.try(kind, base, parts - 3, pp, su, ls5, ls[0] .. ls[2] .. ls[1]) end
              if not r then r, pp, su = l.try(kind, base, parts - 3, pp, su, ls5, ls[1] .. ls[2] .. ls[0]) end
              if not r then r, pp, su = l.try(kind, base, parts - 3, pp, su, ls5, ls[2] .. ls[0] .. ls[1]) end
              if not r then r, pp, su = l.try(kind, base, parts - 3, pp, su, ls5, ls[2] .. ls[1] .. ls[0]) end
            end
          end
          if r then
            ls[0] = ''
            ls[1] = ''
            ls[2] = ''
            break
          end
          if r then break end
        end
        if lscount == 2 and parts >= 2 and parts <= 2 + ppsuparts then
          r, pp, su = l.try(kind, base, parts - 2, pp, su, ls5, ls[0] .. ls[1])
          if (not r) and ls[0] ~= '' and ls[1] ~= '' and (tonumber(ls[0]:sub(-1, -1)) ~= tonumber(ls[1]:sub(-1, -1))) then
            r, pp, su = l.try(kind, base, parts - 2, pp, su, ls5, ls[1] .. ls[0])
          end
          if r then
            ls[0] = ''
            ls[1] = ''
            break
          end
          if r then break end
        end
        if lscount == 1 and parts >= 1 and parts <= 1 + ppsuparts then
          r, pp, su = l.try(kind, base, parts - 1, pp, su, ls5, ls[0])
          if r then
            ls[0] = ''
            break
          end
        end
        r, pp, su = l.try(kind, base, parts, pp, su, ls5, '')
        if r then break end
      end
      if not r or (pp ~= '' and not gregalltab[kind][pp]) or (su ~= '' and not gregalltab[kind][su]) then
        base = "ERR"
      else
        base = gregalltab[kind][r]
        local above = ''
        local below = ''
        local rmetrics = { width = gregallmetrics[kind][r].width,
                           height = gregallmetrics[kind][r].height,
                           depth = gregallmetrics[kind][r].depth }
        -- Should the pre and subpuncta be somehow specially positioned
        -- against the base neume?
        if pp ~= '' then
          base = gregalltab[kind][pp] .. base
          rmetrics.width = rmetrics.width + gregallmetrics[kind][pp].width
          rmetrics.height = math.max (rmetrics.height, gregallmetrics[kind][pp].height)
          rmetrics.depth = math.max (rmetrics.height, gregallmetrics[kind][pp].depth)
        end
        if su ~= '' then
          base = base .. gregalltab[kind][su]
          rmetrics.width = rmetrics.width + gregallmetrics[kind][su].width
          rmetrics.height = math.max (rmetrics.height, gregallmetrics[kind][su].height)
          rmetrics.depth = math.max (rmetrics.height, gregallmetrics[kind][su].depth)
        end
        local baseraise = 0
        if heights[0] ~= 5 then
          baseraise = (heights[0] - 5) * gregallmetrics[kind].cl.height / 4
          base = '\\raise '..string.format("%.3f",baseraise * scale)..'sp\\hbox{'..base..'}'
        end
        local lwidths = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
        local curlwidths = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
        for i = 0, lscount - 1 do
          if ls[i] ~= '' then
            local p = tonumber(ls[i]:sub(-1, -1))
            local l = ls[i]:sub(1, -2)
            lwidths[p] = lwidths[p] + gregallmetrics[kind][l].width
          end
        end
        lwidths[10] = math.max (lwidths[1], lwidths[4], lwidths[7])
        lwidths[11] = math.max (lwidths[2], lwidths[8])
        lwidths[12] = math.max (lwidths[3], lwidths[6], lwidths[9])
        local pre = ''
        local post = ''
        for i = 0, lscount - 1 do
          if ls[i] ~= '' then
            local p = tonumber(ls[i]:sub(-1, -1))
            local l = ls[i]:sub(1, -2)
            local lstr = gregalltab[kind][l]
            base, pre, post = add_ls(base, pre, post, lstr, p, rmetrics, gregallmetrics[kind][l], baseraise, lwidths, curlwidths, scale)
          end
        end
        base = pre..base..post
      end
    end
    ret = ret .. base
    idx, ret = add_spacing(str, len, idx, ret)
  end
  return ret
end

local function init_font(fontname)
  if not gregalltab[fontname] then
    gregalltab[fontname], gregallmetrics[fontname] = gregallreadfont(fontname, font.current())
  end
end

local function print_nabc(nabc)
  tex.sprint(catcode_at_letter, nabc)
end

gregoriotex.parse_nabc = gregallparse_neumes
gregoriotex.print_nabc = print_nabc
gregoriotex.init_nabc_font = init_font
gregoriotex.nabc_font_tables = gregalltab
