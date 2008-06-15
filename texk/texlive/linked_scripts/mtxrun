#!/usr/bin/env texlua

if not modules then modules = { } end modules ['mtxrun'] = {
    version   = 1.001,
    comment   = "runner, lua replacement for texmfstart.rb",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- one can make a stub:
--
-- #!/bin/sh
-- env LUATEXDIR=/....../texmf/scripts/context/lua luatex --luaonly mtxrun.lua "$@"

-- filename : mtxrun.lua
-- comment  : companion to context.tex
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

-- This script is based on texmfstart.rb but does not use kpsewhich to
-- locate files. Although kpse is a library it never came to opening up
-- its interface to other programs (esp scripting languages) and so we
-- do it ourselves. The lua variant evolved out of an experimental ruby
-- one. Interesting is that using a scripting language instead of c does
-- not have a speed penalty. Actually the lua variant is more efficient,
-- especially when multiple calls to kpsewhich are involved. The lua
-- library also gives way more ocntrol.

-- to be done / considered
--
-- support for --exec or make it default
-- support for jar files (or maybe not, never used, too messy)
-- support for $RUBYINPUTS cum suis (if still needed)
-- remember for subruns: _CTX_K_V_#{original}_
-- remember for subruns: _CTX_K_S_#{original}_
-- remember for subruns: TEXMFSTART.#{original} [tex.rb texmfstart.rb]

banner = "version 1.0.2 - 2007+ - PRAGMA ADE / CONTEXT"
texlua = true

-- begin library merge
-- filename : l-string.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-string'] = 1.001

--~ function string.split(str, pat) -- taken from the lua wiki
--~     local t = {n = 0} -- so this table has a length field, traverse with ipairs then!
--~     local fpat = "(.-)"..pat
--~     local last_end = 1
--~     local s, e, cap = string.find(str, fpat, 1)
--~     while s ~= nil do
--~         if s~=1 or cap~="" then
--~             table.insert(t,cap)
--~         end
--~         last_end = e+1
--~         s, e, cap = string.find(str, fpat, last_end)
--~     end
--~     if last_end<=string.len(str) then
--~         table.insert(t,(string.sub(str,last_end)))
--~     end
--~     return t
--~ end

--~ function string:split(pat) -- taken from the lua wiki but adapted
--~     local t = { }          -- self and colon usage (faster)
--~     local fpat = "(.-)"..pat
--~     local last_end = 1
--~     local s, e, cap = self:find(fpat, 1)
--~     while s ~= nil do
--~         if s~=1 or cap~="" then
--~             t[#t+1] = cap
--~         end
--~         last_end = e+1
--~         s, e, cap = self:find(fpat, last_end)
--~     end
--~     if last_end <= #self then
--~         t[#t+1] = self:sub(last_end)
--~     end
--~     return t
--~ end

--~ a piece of brilliant code by Rici Lake (posted on lua list) -- only names changed
--~
--~ function string:splitter(pat)
--~    local st, g = 1, self:gmatch("()"..pat.."()")
--~    local function splitter(self)
--~      if st then
--~        local s, f = g()
--~        local rv = self:sub(st, (s or 0)-1)
--~        st = f
--~        return rv
--~      end
--~    end
--~    return splitter, self
--~ end

function string:splitter(pat)
    -- by Rici Lake (posted on lua list) -- only names changed
    -- p 79 ref man: () returns position of match
    local st, g = 1, self:gmatch("()("..pat..")")
    local function strgetter(self, segs, seps, sep, cap1, ...)
        st = sep and seps + #sep
        return self:sub(segs, (seps or 0) - 1), cap1 or sep, ...
    end
    local function strsplitter(self)
        if st then return strgetter(self, st, g()) end
    end
    return strsplitter, self
end

function string:split(separator)
    local t = {}
    for k in self:splitter(separator) do t[#t+1] = k end
    return t
end

-- faster than a string:split:

function string:splitchr(chr)
    if #self > 0 then
        local t = { }
        for s in string.gmatch(self..chr,"(.-)"..chr) do
            t[#t+1] = s
        end
        return t
    else
        return { }
    end
end

--~ function string.piecewise(str, pat, fnc) -- variant of split
--~     local fpat = "(.-)"..pat
--~     local last_end = 1
--~     local s, e, cap = string.find(str, fpat, 1)
--~     while s ~= nil do
--~         if s~=1 or cap~="" then
--~             fnc(cap)
--~         end
--~         last_end = e+1
--~         s, e, cap = string.find(str, fpat, last_end)
--~     end
--~     if last_end <= #str then
--~         fnc((string.sub(str,last_end)))
--~     end
--~ end

function string.piecewise(str, pat, fnc) -- variant of split
    for k in string.splitter(str,pat) do fnc(k) end
end

--~ function string.piecewise(str, pat, fnc) -- variant of split
--~     for k in str:splitter(pat) do fnc(k) end
--~ end

--~ do if lpeg then

--~     -- this alternative is 30% faster esp when we cache them
--~     -- problem: no expressions

--~     splitters = { }

--~     function string:split(separator)
--~         if #self > 0 then
--~             local split = splitters[separator]
--~             if not split then
--~                 -- based on code by Roberto
--~                 local p = lpeg.P(separator)
--~                 local c = lpeg.C((1-p)^0)
--~                 split = lpeg.Ct(c*(p*c)^0)
--~                 splitters[separator] = split
--~             end
--~             return split:match(self)
--~         else
--~             return { }
--~         end
--~     end

--~     string.splitchr = string.split

--~     function string:piecewise(separator,fnc)
--~         for _,v in pairs(self:split(separator)) do
--~             fnc(v)
--~         end
--~     end

--~ end end

string.chr_to_esc = {
    ["%"] = "%%",
    ["."] = "%.",
    ["+"] = "%+", ["-"] = "%-", ["*"] = "%*",
    ["^"] = "%^", ["$"] = "%$",
    ["["] = "%[", ["]"] = "%]",
    ["("] = "%(", [")"] = "%)",
    ["{"] = "%{", ["}"] = "%}"
}

function string:esc() -- variant 2
    return (self:gsub("(.)",string.chr_to_esc))
end

function string.unquote(str)
    return (str:gsub("^([\"\'])(.*)%1$","%2"))
end

function string.quote(str)
    return '"' .. str:unquote() .. '"'
end

function string:count(pattern) -- variant 3
    local n = 0
    for _ in self:gmatch(pattern) do
        n = n + 1
    end
    return n
end

function string:limit(n,sentinel)
    if #self > n then
        sentinel = sentinel or " ..."
        return self:sub(1,(n-#sentinel)) .. sentinel
    else
        return self
    end
end

function string:strip()
    return (self:gsub("^%s*(.-)%s*$", "%1"))
end

--~ function string.strip(str) -- slightly different
--~     return (string.gsub(string.gsub(str,"^%s*(.-)%s*$","%1"),"%s+"," "))
--~ end

function string:is_empty()
    return not self:find("%S")
end

function string:enhance(pattern,action)
    local ok, n = true, 0
    while ok do
        ok = false
        self = self:gsub(pattern, function(...)
            ok, n = true, n + 1
            return action(...)
        end)
    end
    return self, n
end

--~ function string:enhance(pattern,action)
--~     local ok, n = 0, 0
--~     repeat
--~         self, ok = self:gsub(pattern, function(...)
--~             n = n + 1
--~             return action(...)
--~         end)
--~     until ok == 0
--~     return self, n
--~ end

--~     function string:to_hex()
--~         if self then
--~             return (self:gsub("(.)",function(c)
--~                 return string.format("%02X",c:byte())
--~             end))
--~         else
--~             return ""
--~         end
--~     end

--~     function string:from_hex()
--~         if self then
--~             return (self:gsub("(..)",function(c)
--~                 return string.char(tonumber(c,16))
--~             end))
--~         else
--~             return ""
--~         end
--~     end

string.chr_to_hex = { }
string.hex_to_chr = { }

for i=0,255 do
    local c, h = string.char(i), string.format("%02X",i)
    string.chr_to_hex[c], string.hex_to_chr[h] = h, c
end

--~     function string:to_hex()
--~         if self then return (self:gsub("(.)",string.chr_to_hex)) else return "" end
--~     end

--~     function string:from_hex()
--~         if self then return (self:gsub("(..)",string.hex_to_chr)) else return "" end
--~     end

function string:to_hex()
    return ((self or ""):gsub("(.)",string.chr_to_hex))
end

function string:from_hex()
    return ((self or ""):gsub("(..)",string.hex_to_chr))
end

if not string.characters then

    local function nextchar(str, index)
        index = index + 1
        return (index <= #str) and index or nil, str:sub(index,index)
    end
    function string:characters()
        return nextchar, self, 0
    end
    local function nextbyte(str, index)
        index = index + 1
        return (index <= #str) and index or nil, string.byte(str:sub(index,index))
    end
    function string:bytes()
        return nextbyte, self, 0
    end

end

--~ function string:padd(n,chr)
--~     return self .. self.rep(chr or " ",n-#self)
--~ end

function string:rpadd(n,chr)
    local m = n-#self
    if m > 0 then
        return self .. self.rep(chr or " ",m)
    else
        return self
    end
end

function string:lpadd(n,chr)
    local m = n-#self
    if m > 0 then
        return self.rep(chr or " ",m) .. self
    else
        return self
    end
end

string.padd = string.rpadd

function is_number(str)
    return str:find("^[%-%+]?[%d]-%.?[%d+]$") == 1
end

--~ print(is_number("1"))
--~ print(is_number("1.1"))
--~ print(is_number(".1"))
--~ print(is_number("-0.1"))
--~ print(is_number("+0.1"))
--~ print(is_number("-.1"))
--~ print(is_number("+.1"))

function string:split_settings() -- no {} handling, see l-aux for lpeg variant
    if self:find("=") then
        local t = { }
        for k,v in self:gmatch("(%a+)=([^%,]*)") do
            t[k] = v
        end
        return t
    else
        return nil
    end
end

local patterns_escapes = {
    ["-"] = "%-",
    ["."] = "%.",
    ["+"] = "%+",
    ["*"] = "%*",
    ["%"] = "%%",
    ["("] = "%)",
    [")"] = "%)",
    ["["] = "%[",
    ["]"] = "%]",
}

function string:pattesc()
    return (self:gsub(".",patterns_escapes))
end

function string:tohash()
    local t = { }
    for s in self:gmatch("([^, ]+)") do -- lpeg
        t[s] = true
    end
    return t
end


-- filename : l-lpeg.lua
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-lpeg'] = 1.001

--~ l-lpeg.lua :

--~ lpeg.digit         = lpeg.R('09')^1
--~ lpeg.sign          = lpeg.S('+-')^1
--~ lpeg.cardinal      = lpeg.P(lpeg.sign^0 * lpeg.digit^1)
--~ lpeg.integer       = lpeg.P(lpeg.sign^0 * lpeg.digit^1)
--~ lpeg.float         = lpeg.P(lpeg.sign^0 * lpeg.digit^0 * lpeg.P('.') * lpeg.digit^1)
--~ lpeg.number        = lpeg.float + lpeg.integer
--~ lpeg.oct           = lpeg.P("0") * lpeg.R('07')^1
--~ lpeg.hex           = lpeg.P("0x") * (lpeg.R('09') + lpeg.R('AF'))^1
--~ lpeg.uppercase     = lpeg.P("AZ")
--~ lpeg.lowercase     = lpeg.P("az")

--~ lpeg.eol           = lpeg.S('\r\n\f')^1 -- includes formfeed
--~ lpeg.space         = lpeg.S(' ')^1
--~ lpeg.nonspace      = lpeg.P(1-lpeg.space)^1
--~ lpeg.whitespace    = lpeg.S(' \r\n\f\t')^1
--~ lpeg.nonwhitespace = lpeg.P(1-lpeg.whitespace)^1

local hash = { }

function lpeg.anywhere(pattern) --slightly adapted from website
    return lpeg.P { lpeg.P(pattern) + 1 * lpeg.V(1) }
end

function lpeg.startswith(pattern) --slightly adapted
    return lpeg.P(pattern)
end

--~ g = lpeg.splitter(" ",function(s) ... end) -- gmatch:lpeg = 3:2

function lpeg.splitter(pattern, action)
    return (((1-lpeg.P(pattern))^1)/action+1)^0
end

local crlf     = lpeg.P("\r\n")
local cr       = lpeg.P("\r")
local lf       = lpeg.P("\n")
local space    = lpeg.S(" \t\f\v")
local newline  = crlf + cr + lf
local spacing  = space^0 * newline

local empty    = spacing * lpeg.Cc("")
local nonempty = lpeg.Cs((1-spacing)^1) * spacing^-1
local content  = (empty + nonempty)^1

local capture = lpeg.Ct(content^0)

function string:splitlines()
    return capture:match(self)
end


-- filename : l-table.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-table'] = 1.001

table.join = table.concat

function table.strip(tab)
    local lst = { }
    for k, v in ipairs(tab) do
     -- s = string.gsub(v, "^%s*(.-)%s*$", "%1")
        s = v:gsub("^%s*(.-)%s*$", "%1")
        if s == "" then
            -- skip this one
        else
            lst[#lst+1] = s
        end
    end
    return lst
end

--~ function table.sortedkeys(tab)
--~     local srt = { }
--~     for key,_ in pairs(tab) do
--~         srt[#srt+1] = key
--~     end
--~     table.sort(srt)
--~     return srt
--~ end

function table.sortedkeys(tab)
    local srt, kind = { }, 0 -- 0=unknown 1=string, 2=number 3=mixed
    for key,_ in pairs(tab) do
        srt[#srt+1] = key
        if kind == 3 then
            -- no further check
        else
            local tkey = type(key)
            if tkey == "string" then
            --  if kind == 2 then kind = 3 else kind = 1 end
                kind = (kind == 2 and 3) or 1
            elseif tkey == "number" then
            --  if kind == 1 then kind = 3 else kind = 2 end
                kind = (kind == 1 and 3) or 2
            else
                kind = 3
            end
        end
    end
    if kind == 0 or kind == 3 then
        table.sort(srt,function(a,b) return (tostring(a) < tostring(b)) end)
    else
        table.sort(srt)
    end
    return srt
end

function table.append(t, list)
    for _,v in pairs(list) do
        table.insert(t,v)
    end
end

function table.prepend(t, list)
    for k,v in pairs(list) do
        table.insert(t,k,v)
    end
end

function table.merge(t, ...) -- first one is target
    t = t or {}
    local lst = {...}
    for i=1,#lst do
        for k, v in pairs(lst[i]) do
            t[k] = v
        end
    end
    return t
end

function table.merged(...)
    local tmp, lst = { }, {...}
    for i=1,#lst do
        for k, v in pairs(lst[i]) do
            tmp[k] = v
        end
    end
    return tmp
end

function table.imerge(t, ...)
    local lst = {...}
    for i=1,#lst do
        local nst = lst[i]
        for j=1,#nst do
            t[#t+1] = nst[j]
        end
    end
    return t
end

function table.imerged(...)
    local tmp, lst = { }, {...}
    for i=1,#lst do
        local nst = lst[i]
        for j=1,#nst do
            tmp[#tmp+1] = nst[j]
        end
    end
    return tmp
end

if not table.fastcopy then do

    local type, pairs, getmetatable, setmetatable = type, pairs, getmetatable, setmetatable

    local function fastcopy(old) -- fast one
        if old then
            local new = { }
            for k,v in pairs(old) do
                if type(v) == "table" then
                    new[k] = fastcopy(v) -- was just table.copy
                else
                    new[k] = v
                end
            end
            local mt = getmetatable(old)
            if mt then
                setmetatable(new,mt)
            end
            return new
        else
            return { }
        end
    end

    table.fastcopy = fastcopy

end end

if not table.copy then do

    local type, pairs, getmetatable, setmetatable = type, pairs, getmetatable, setmetatable

    local function copy(t, tables) -- taken from lua wiki, slightly adapted
        tables = tables or { }
        local tcopy = {}
        if not tables[t] then
            tables[t] = tcopy
        end
        for i,v in pairs(t) do -- brrr, what happens with sparse indexed
            if type(i) == "table" then
                if tables[i] then
                    i = tables[i]
                else
                    i = copy(i, tables)
                end
            end
            if type(v) ~= "table" then
                tcopy[i] = v
            elseif tables[v] then
                tcopy[i] = tables[v]
            else
                tcopy[i] = copy(v, tables)
            end
        end
        local mt = getmetatable(t)
        if mt then
            setmetatable(tcopy,mt)
        end
        return tcopy
    end

    table.copy = copy

end end

-- rougly: copy-loop : unpack : sub == 0.9 : 0.4 : 0.45 (so in critical apps, use unpack)

function table.sub(t,i,j)
    return { unpack(t,i,j) }
end

function table.replace(a,b)
    for k,v in pairs(b) do
        a[k] = v
    end
end

-- slower than #t on indexed tables (#t only returns the size of the numerically indexed slice)

function table.is_empty(t)
    return not t or not next(t)
end

function table.one_entry(t)
    local n = next(t)
    return n and not next(t,n)
end

function table.starts_at(t)
    return ipairs(t,1)(t,0)
end

do

    -- one of my first exercises in lua ...

    -- 34.055.092 32.403.326 arabtype.tma
    --  1.620.614  1.513.863 lmroman10-italic.tma
    --  1.325.585  1.233.044 lmroman10-regular.tma
    --  1.248.157  1.158.903 lmsans10-regular.tma
    --    194.646    153.120 lmtypewriter10-regular.tma
    --  1.771.678  1.658.461 palatinosanscom-bold.tma
    --  1.695.251  1.584.491 palatinosanscom-regular.tma
    -- 13.736.534 13.409.446 zapfinoextraltpro.tma

    -- 13.679.038 11.774.106 arabtype.tmc
    --    886.248    754.944 lmroman10-italic.tmc
    --    729.828    466.864 lmroman10-regular.tmc
    --    688.482    441.962 lmsans10-regular.tmc
    --    128.685     95.853 lmtypewriter10-regular.tmc
    --    715.929    582.985 palatinosanscom-bold.tmc
    --    669.942    540.126 palatinosanscom-regular.tmc
    --  1.560.588  1.317.000 zapfinoextraltpro.tmc

    table.serialize_functions = true
    table.serialize_compact   = true
    table.serialize_inline    = true

    local function key(k)
        if type(k) == "number" then -- or k:find("^%d+$") then
            return "["..k.."]"
        elseif noquotes and k:find("^%a[%a%d%_]*$") then
            return k
        else
            return '["'..k..'"]'
        end
    end

    local function simple_table(t)
        if #t > 0 then
            local n = 0
            for _,v in pairs(t) do
                n = n + 1
            end
            if n == #t then
                local tt = { }
                for i=1,#t do
                    local v = t[i]
                    local tv = type(v)
                    if tv == "number" or tv == "boolean" then
                        tt[#tt+1] = tostring(v)
                    elseif tv == "string" then
                        tt[#tt+1] = ("%q"):format(v)
                    else
                        tt = nil
                        break
                    end
                end
                return tt
            end
        end
        return nil
    end

    local function serialize(root,name,handle,depth,level,reduce,noquotes,indexed)
        handle = handle or print
        reduce = reduce or false
        if depth then
            depth = depth .. " "
            if indexed then
                handle(("%s{"):format(depth))
            else
                handle(("%s%s={"):format(depth,key(name)))
            end
        else
            depth = ""
            local tname = type(name)
            if tname == "string" then
                if name == "return" then
                    handle("return {")
                else
                    handle(name .. "={")
                end
            elseif tname == "number" then
                handle("[" .. name .. "]={")
            elseif tname == "boolean" then
                if name then
                    handle("return {")
                else
                    handle("{")
                end
            else
                handle("t={")
            end
        end
        if root and next(root) then
            local compact = table.serialize_compact
            local inline  = compact and table.serialize_inline
            local first, last = nil, 0 -- #root cannot be trusted here
            if compact then
              for k,v in ipairs(root) do -- NOT: for k=1,#root do (why)
                    if not first then first = k end
                    last = last + 1
                end
            end
            for _,k in pairs(table.sortedkeys(root)) do
                local v = root[k]
                local t = type(v)
                if compact and first and type(k) == "number" and k >= first and k <= last then
                    if t == "number" then
                        handle(("%s %s,"):format(depth,v))
                    elseif t == "string" then
                        if reduce and (v:find("^[%-%+]?[%d]-%.?[%d+]$") == 1) then
                            handle(("%s %s,"):format(depth,v))
                        else
                            handle(("%s %q,"):format(depth,v))
                        end
                    elseif t == "table" then
                        if not next(v) then
                            handle(("%s {},"):format(depth))
                        elseif inline then
                            local st = simple_table(v)
                            if st then
                                handle(("%s { %s },"):format(depth,table.concat(st,", ")))
                            else
                                serialize(v,k,handle,depth,level+1,reduce,noquotes,true)
                            end
                        else
                            serialize(v,k,handle,depth,level+1,reduce,noquotes,true)
                        end
                    elseif t == "boolean" then
                        handle(("%s %s,"):format(depth,tostring(v)))
                    elseif t == "function" then
                        if table.serialize_functions then
                            handle(('%s loadstring(%q),'):format(depth,string.dump(v)))
                        else
                            handle(('%s "function",'):format(depth))
                        end
                    else
                        handle(("%s %q,"):format(depth,tostring(v)))
                    end
                elseif k == "__p__" then -- parent
                    if false then
                        handle(("%s __p__=nil,"):format(depth))
                    end
                elseif t == "number" then
                    handle(("%s %s=%s,"):format(depth,key(k),v))
                elseif t == "string" then
                    if reduce and (v:find("^[%-%+]?[%d]-%.?[%d+]$") == 1) then
                        handle(("%s %s=%s,"):format(depth,key(k),v))
                    else
                        handle(("%s %s=%q,"):format(depth,key(k),v))
                    end
                elseif t == "table" then
                    if not next(v) then
                        handle(("%s %s={},"):format(depth,key(k)))
                    elseif inline then
                        local st = simple_table(v)
                        if st then
                            handle(("%s %s={ %s },"):format(depth,key(k),table.concat(st,", ")))
                        else
                            serialize(v,k,handle,depth,level+1,reduce,noquotes)
                        end
                    else
                        serialize(v,k,handle,depth,level+1,reduce,noquotes)
                    end
                elseif t == "boolean" then
                    handle(("%s %s=%s,"):format(depth,key(k),tostring(v)))
                elseif t == "function" then
                    if table.serialize_functions then
                        handle(('%s %s=loadstring(%q),'):format(depth,key(k),string.dump(v)))
                    else
                        handle(('%s %s="function",'):format(depth,key(k)))
                    end
                else
                    handle(("%s %s=%q,"):format(depth,key(k),tostring(v)))
                --  handle(('%s %s=loadstring(%q),'):format(depth,key(k),string.dump(function() return v end)))
                end
            end
            if level > 0 then
                handle(("%s},"):format(depth))
            else
                handle(("%s}"):format(depth))
            end
        else
            handle(("%s}"):format(depth))
        end
    end

    --~ name:
    --~
    --~ true     : return     { }
    --~ false    :            { }
    --~ nil      : t        = { }
    --~ string   : string   = { }
    --~ 'return' : return     { }
    --~ number   : [number] = { }

    function table.serialize(root,name,reduce,noquotes)
        local t = { }
        local function flush(s)
            t[#t+1] = s
        end
        serialize(root, name, flush, nil, 0, reduce, noquotes)
        return table.concat(t,"\n")
    end

    function table.tohandle(handle,root,name,reduce,noquotes)
        serialize(root, name, handle, nil, 0, reduce, noquotes)
    end

    -- sometimes tables are real use (zapfino extra pro is some 85M) in which
    -- case a stepwise serialization is nice; actually, we could consider:
    --
    -- for line in table.serializer(root,name,reduce,noquotes) do
    --    ...(line)
    -- end
    --
    -- so this is on the todo list

    table.tofile_maxtab = 2*1024

    function table.tofile(filename,root,name,reduce,noquotes)
        local f = io.open(filename,'w')
        if f then
            local concat = table.concat
            local maxtab = table.tofile_maxtab
            if maxtab > 1 then
                local t = { }
                local function flush(s)
                    t[#t+1] = s
                    if #t > maxtab then
                        f:write(concat(t,"\n"),"\n") -- hm, write(sometable) should be nice
                        t = { }
                    end
                end
                serialize(root, name, flush, nil, 0, reduce, noquotes)
                f:write(concat(t,"\n"),"\n")
            else
                local function flush(s)
                    f:write(s,"\n")
                end
                serialize(root, name, flush, nil, 0, reduce, noquotes)
            end
            f:close()
        end
    end

end

--~ t = {
--~     b = "123",
--~     a = "x",
--~     c = 1.23,
--~     d = "1.23",
--~     e = true,
--~     f = {
--~         d = "1.23",
--~         a = "x",
--~         b = "123",
--~         c = 1.23,
--~         e = true,
--~         f = {
--~             e = true,
--~             f = {
--~                 e = true
--~             },
--~         },
--~     },
--~     g = function() end
--~ }

--~ print(table.serialize(t), "\n")
--~ print(table.serialize(t,"name"), "\n")
--~ print(table.serialize(t,false), "\n")
--~ print(table.serialize(t,true), "\n")
--~ print(table.serialize(t,"name",true), "\n")
--~ print(table.serialize(t,"name",true,true), "\n")

do

    local function flatten(t,f,complete)
        for i=1,#t do
            local v = t[i]
            if type(v) == "table" then
                if complete or type(v[1]) == "table" then
                    flatten(v,f,complete)
                else
                    f[#f+1] = v
                end
            else
                f[#f+1] = v
            end
        end
    end

    function table.flatten(t)
        local f = { }
        flatten(t,f,true)
        return f
    end

    function table.unnest(t) -- bad name
        local f = { }
        flatten(t,f,false)
        return f
    end

    table.flatten_one_level = table.unnest

end

function table.insert_before_value(t,value,str)
    for i=1,#t do
        if t[i] == value then
            table.insert(t,i,str)
            return
        end
    end
    table.insert(t,1,str)
end

function table.insert_after_value(t,value,str)
    for i=1,#t do
        if t[i] == value then
            table.insert(t,i+1,str)
            return
        end
    end
    t[#t+1] = str
end

function table.are_equal(a,b,n,m)
    if #a == #b then
        n = n or 1
        m = m or #a
        for i=n,m do
            local ai, bi = a[i], b[i]
            if (ai==bi) or (type(ai)=="table" and type(bi)=="table" and table.are_equal(ai,bi)) then
                -- continue
            else
                return false
            end
        end
        return true
    else
        return false
    end
end

function table.compact(t)
    if t then
        for k,v in pairs(t) do
            if not next(v) then
                t[k] = nil
            end
        end
    end
end

function table.tohash(t)
    local h = { }
    for _, v in pairs(t) do -- no ipairs here
        h[v] = true
    end
    return h
end

function table.fromhash(t)
    local h = { }
    for k, v in pairs(t) do -- no ipairs here
        if v then h[#h+1] = k end
    end
    return h
end

function table.contains(t, v)
    if t then
        for i=1, #t do
            if t[i] == v then
                return true
            end
        end
    end
    return false
end

function table.count(t)
    local n, e = 0, next(t)
    while e do
        n, e = n + 1, next(t,e)
    end
    return n
end

function table.swapped(t)
    local s = { }
    for k, v in pairs(t) do
        s[v] = k
    end
    return s
end

--~ function table.are_equal(a,b)
--~     return table.serialize(a) == table.serialize(b)
--~ end

function table.clone(t,p) -- t is optional or nil or table
    if not p then
        t, p = { }, t or { }
    elseif not t then
        t = { }
    end
    setmetatable(t, { __index = function(_,key) return p[key] end })
    return t
end


function table.hexed(t,seperator)
    local tt = { }
    for i=1,#t do tt[i] = string.format("0x%04X",t[i]) end
    return table.concat(tt,seperator or " ")
end

function table.reverse_hash(h)
    local r = { }
    for k,v in pairs(h) do
        r[v] = (k:gsub(" ","")):lower()
    end
    return r
end

function table.reverse(t)
    local tt = { }
    if #t > 0 then
        for i=#t,1,-1 do
            tt[#tt+1] = t[i]
        end
    end
    return tt
end


-- filename : l-io.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-io'] = 1.001

if string.find(os.getenv("PATH"),";") then
    io.fileseparator, io.pathseparator = "\\", ";"
else
    io.fileseparator, io.pathseparator = "/" , ":"
end

function io.loaddata(filename)
    local f = io.open(filename,'rb')
    if f then
        local data = f:read('*all')
        f:close()
        return data
    else
        return nil
    end
end

function io.savedata(filename,data,joiner)
    local f = io.open(filename, "wb")
    if f then
        if type(data) == "table" then
            f:write(table.join(data,joiner or ""))
        elseif type(data) == "function" then
            data(f)
        else
            f:write(data)
        end
        f:close()
    end
end

function io.exists(filename)
    local f = io.open(filename)
    if f == nil then
        return false
    else
        assert(f:close())
        return true
    end
end

function io.size(filename)
    local f = io.open(filename)
    if f == nil then
        return 0
    else
        local s = f:seek("end")
        assert(f:close())
        return s
    end
end

function io.noflines(f)
    local n = 0
    for _ in f:lines() do
        n = n + 1
    end
    f:seek('set',0)
    return n
end

do

    local sb = string.byte

    local nextchar = {
        [ 4] = function(f)
            return f:read(1,1,1,1)
        end,
        [ 2] = function(f)
            return f:read(1,1)
        end,
        [ 1] = function(f)
            return f:read(1)
        end,
        [-2] = function(f)
            local a, b = f:read(1,1)
            return b, a
        end,
        [-4] = function(f)
            local a, b, c, d = f:read(1,1,1,1)
            return d, c, b, a
        end
    }

    function io.characters(f,n)
        if f then
            return nextchar[n or 1], f
        else
            return nil, nil
        end
    end

end

do

    local sb = string.byte

--~     local nextbyte = {
--~         [4] = function(f)
--~             local a = f:read(1)
--~             local b = f:read(1)
--~             local c = f:read(1)
--~             local d = f:read(1)
--~             if d then
--~                 return sb(a), sb(b), sb(c), sb(d)
--~             else
--~                 return nil, nil, nil, nil
--~             end
--~         end,
--~         [2] = function(f)
--~             local a = f:read(1)
--~             local b = f:read(1)
--~             if b then
--~                 return sb(a), sb(b)
--~             else
--~                 return nil, nil
--~             end
--~         end,
--~         [1] = function (f)
--~             local a = f:read(1)
--~             if a then
--~                 return sb(a)
--~             else
--~                 return nil
--~             end
--~         end,
--~         [-2] = function (f)
--~             local a = f:read(1)
--~             local b = f:read(1)
--~             if b then
--~                 return sb(b), sb(a)
--~             else
--~                 return nil, nil
--~             end
--~         end,
--~         [-4] = function(f)
--~             local a = f:read(1)
--~             local b = f:read(1)
--~             local c = f:read(1)
--~             local d = f:read(1)
--~             if d then
--~                 return sb(d), sb(c), sb(b), sb(a)
--~             else
--~                 return nil, nil, nil, nil
--~             end
--~         end
--~     }

    local nextbyte = {
        [4] = function(f)
            local a, b, c, d = f:read(1,1,1,1)
            if d then
                return sb(a), sb(b), sb(c), sb(d)
            else
                return nil, nil, nil, nil
            end
        end,
        [2] = function(f)
            local a, b = f:read(1,1)
            if b then
                return sb(a), sb(b)
            else
                return nil, nil
            end
        end,
        [1] = function (f)
            local a = f:read(1)
            if a then
                return sb(a)
            else
                return nil
            end
        end,
        [-2] = function (f)
            local a, b = f:read(1,1)
            if b then
                return sb(b), sb(a)
            else
                return nil, nil
            end
        end,
        [-4] = function(f)
            local a, b, c, d = f:read(1,1,1,1)
            if d then
                return sb(d), sb(c), sb(b), sb(a)
            else
                return nil, nil, nil, nil
            end
        end
    }

    function io.bytes(f,n)
        if f then
            return nextbyte[n or 1], f
        else
            return nil, nil
        end
    end

end

function io.ask(question,default,options)
    while true do
        io.write(question)
        if options then
            io.write(string.format(" [%s]",table.concat(options,"|")))
        end
        if default then
            io.write(string.format(" [%s]",default))
        end
        io.write(string.format(" "))
        local answer = io.read()
        answer = answer:gsub("^%s*(.*)%s*$","%1")
        if answer == "" and default then
            return default
        elseif not options then
            return answer
        else
            for _,v in pairs(options) do
                if v == answer then
                    return answer
                end
            end
            local pattern = "^" .. answer
            for _,v in pairs(options) do
                if v:find(pattern) then
                    return v
                end
            end
        end
    end
end


-- filename : l-md5.lua
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-md5'] = 1.001

if md5 then do

    local function convert(str,fmt)
        return (string.gsub(md5.sum(str),".",function(chr) return string.format(fmt,string.byte(chr)) end))
    end

    if not md5.HEX then function md5.HEX(str) return convert(str,"%02X") end end
    if not md5.hex then function md5.hex(str) return convert(str,"%02x") end end
    if not md5.dec then function md5.dec(str) return convert(str,"%03i") end end

end end


-- filename : l-number.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-number'] = 1.001

if not number then number = { } end

-- a,b,c,d,e,f = number.toset(100101)

function number.toset(n)
    return (tostring(n)):match("(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?)")
end

local format = string.format

function number.toevenhex(n)
    local s = format("%X",n)
    if #s % 2 == 0 then
        return s
    else
        return "0" .. s
    end
end

-- the lpeg way is slower on 8 digits, but faster on 4 digits, some 7.5%
-- on
--
-- for i=1,1000000 do
--     local a,b,c,d,e,f,g,h = number.toset(12345678)
--     local a,b,c,d         = number.toset(1234)
--     local a,b,c           = number.toset(123)
-- end
--
-- of course dedicated "(.)(.)(.)(.)" matches are even faster

do
    local one = lpeg.C(1-lpeg.S(''))^1

    function number.toset(n)
        return one:match(tostring(n))
    end
end



-- filename : l-set.lua
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-set'] = 1.001

if not set then set = { } end

do

    local nums   = { }
    local tabs   = { }
    local concat = table.concat

    set.create = table.tohash

    function set.tonumber(t)
        if next(t) then
            local s = ""
        --  we could save mem by sorting, but it slows down
            for k, v in pairs(t) do
                if v then
                --  why bother about the leading space
                    s = s .. " " .. k
                end
            end
            if not nums[s] then
                tabs[#tabs+1] = t
                nums[s] = #tabs
            end
            return nums[s]
        else
            return 0
        end
    end

    function set.totable(n)
        if n == 0 then
            return { }
        else
            return tabs[n] or { }
        end
    end

    function set.contains(n,s)
        if type(n) == "table" then
            return n[s]
        elseif n == 0 then
            return false
        else
            local t = tabs[n]
            return t and t[s]
        end
    end

end

--~ local c = set.create{'aap','noot','mies'}
--~ local s = set.tonumber(c)
--~ local t = set.totable(s)
--~ print(t['aap'])
--~ local c = set.create{'zus','wim','jet'}
--~ local s = set.tonumber(c)
--~ local t = set.totable(s)
--~ print(t['aap'])
--~ print(t['jet'])
--~ print(set.contains(t,'jet'))
--~ print(set.contains(t,'aap'))



-- filename : l-os.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-os'] = 1.001

function os.resultof(command)
    return io.popen(command,"r"):read("*all")
end

if not os.exec  then os.exec  = os.execute end
if not os.spawn then os.spawn = os.execute end

--~ os.type : windows | unix (new, we already guessed os.platform)
--~ os.name : windows | msdos | linux | macosx | solaris | .. | generic (new)

if not io.fileseparator then
    if string.find(os.getenv("PATH"),";") then
        io.fileseparator, io.pathseparator, os.platform = "\\", ";", os.type or "windows"
    else
        io.fileseparator, io.pathseparator, os.platform = "/" , ":", os.type or "unix"
    end
end

os.platform = os.platform or os.type or (io.pathseparator == ";" and "windows") or "unix"

function os.launch(str)
    if os.platform == "windows" then
        os.execute("start " .. str) -- os.spawn ?
    else
        os.execute(str .. " &")     -- os.spawn ?
    end
end

if not os.setenv then
    function os.setenv() return false end
end

if not os.times then
    -- utime  = user time
    -- stime  = system time
    -- cutime = children user time
    -- cstime = children system time
    function os.times()
        return {
            utime  = os.gettimeofday(), -- user
            stime  = 0,                 -- system
            cutime = 0,                 -- children user
            cstime = 0,                 -- children system
        }
    end
end

os.gettimeofday = os.gettimeofday or os.clock

do
    local startuptime = os.gettimeofday()
    function os.runtime()
        return os.gettimeofday() - startuptime
    end
end

--~ print(os.gettimeofday()-os.time())
--~ os.sleep(1.234)
--~ print (">>",os.runtime())
--~ print(os.date("%H:%M:%S",os.gettimeofday()))
--~ print(os.date("%H:%M:%S",os.time()))


-- filename : l-file.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-file'] = 1.001

if not file then file = { } end

function file.removesuffix(filename)
    return filename:gsub("%.[%a%d]+$", "")
end

function file.addsuffix(filename, suffix)
    if not filename:find("%.[%a%d]+$") then
        return filename .. "." .. suffix
    else
        return filename
    end
end

function file.replacesuffix(filename, suffix)
    if not filename:find("%.[%a%d]+$") then
        return filename .. "." .. suffix
    else
        return (filename:gsub("%.[%a%d]+$","."..suffix))
    end
end

function file.dirname(name)
    return name:match("^(.+)[/\\].-$") or ""
end

function file.basename(name)
    return name:match("^.+[/\\](.-)$") or name
end

function file.nameonly(name)
    return ((name:match("^.+[/\\](.-)$") or name):gsub("%..*$",""))
end

function file.extname(name)
    return name:match("^.+%.([^/\\]-)$") or  ""
end

file.suffix = file.extname

function file.stripsuffix(name)
    return (name:gsub("%.[%a%d]+$",""))
end

--~ function file.join(...)
--~     local t = { ... }
--~     for i=1,#t do
--~         t[i] = (t[i]:gsub("\\","/")):gsub("/+$","")
--~     end
--~     return table.concat(t,"/")
--~ end

--~ print(file.join("x/","/y"))
--~ print(file.join("http://","/y"))
--~ print(file.join("http://a","/y"))
--~ print(file.join("http:///a","/y"))
--~ print(file.join("//nas-1","/y"))

function file.join(...)
    local pth = table.concat({...},"/")
    pth = pth:gsub("\\","/")
    local a, b = pth:match("^(.*://)(.*)$")
    if a and b then
        return a .. b:gsub("//+","/")
    end
    a, b = pth:match("^(//)(.*)$")
    if a and b then
        return a .. b:gsub("//+","/")
    end
    return (pth:gsub("//+","/"))
end

function file.is_writable(name)
    local f = io.open(name, 'w')
    if f then
        f:close()
        return true
    else
        return false
    end
end

function file.is_readable(name)
    local f = io.open(name,'r')
    if f then
        f:close()
        return true
    else
        return false
    end
end

--~ function file.split_path(str)
--~     if str:find(';') then
--~         return str:splitchr(";")
--~     else
--~         return str:splitchr(io.pathseparator)
--~     end
--~ end

-- todo: lpeg

function file.split_path(str)
    local t = { }
    str = str:gsub("\\", "/")
    str = str:gsub("(%a):([;/])", "%1\001%2")
    for name in str:gmatch("([^;:]+)") do
        if name ~= "" then
            name = name:gsub("\001",":")
            t[#t+1] = name
        end
    end
    return t
end

function file.join_path(tab)
    return table.concat(tab,io.pathseparator) -- can have trailing //
end

--~ print('test'           .. " == " .. file.collapse_path("test"))
--~ print("test/test"      .. " == " .. file.collapse_path("test/test"))
--~ print("test/test/test" .. " == " .. file.collapse_path("test/test/test"))
--~ print("test/test"      .. " == " .. file.collapse_path("test/../test/test"))
--~ print("test"           .. " == " .. file.collapse_path("test/../test"))
--~ print("../test"        .. " == " .. file.collapse_path("../test"))
--~ print("../test/"       .. " == " .. file.collapse_path("../test/"))
--~ print("a/a"            .. " == " .. file.collapse_path("a/b/c/../../a"))

--~ function file.collapse_path(str)
--~     local ok, n = false, 0
--~     while not ok do
--~         ok = true
--~         str, n = str:gsub("[^%./]+/%.%./", function(s)
--~             ok = false
--~             return ""
--~         end)
--~     end
--~     return (str:gsub("/%./","/"))
--~ end

function file.collapse_path(str)
    local n = 1
    while n > 0 do
        str, n = str:gsub("([^/%.]+/%.%./)","")
    end
    return (str:gsub("/%./","/"))
end

function file.robustname(str)
    return (str:gsub("[^%a%d%/%-%.\\]+","-"))
end

file.readdata = io.loaddata
file.savedata = io.savedata

function file.copy(oldname,newname)
    file.savedata(newname,io.loaddata(oldname))
end


-- filename : l-dir.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-dir'] = 1.001

dir = { }

-- optimizing for no string.find (*) does not save time

if lfs then do

--~     local attributes = lfs.attributes
--~     local walkdir    = lfs.dir
--~
--~     local function glob_pattern(path,patt,recurse,action)
--~         local ok, scanner = xpcall(function() return walkdir(path) end, function() end) -- kepler safe
--~         if ok and type(scanner) == "function" then
--~             if not path:find("/$") then path = path .. '/' end
--~             for name in scanner do
--~                 local full = path .. name
--~                 local mode = attributes(full,'mode')
--~                 if mode == 'file' then
--~                     if name:find(patt) then
--~                         action(full)
--~                     end
--~                 elseif recurse and (mode == "directory") and (name ~= '.') and (name ~= "..") then
--~                     glob_pattern(full,patt,recurse,action)
--~                 end
--~             end
--~         end
--~     end
--~
--~     dir.glob_pattern = glob_pattern
--~
--~     local function glob(pattern, action)
--~         local t = { }
--~         local action = action or function(name) t[#t+1] = name end
--~         local path, patt = pattern:match("^(.*)/*%*%*/*(.-)$")
--~         local recurse = path and patt
--~         if not recurse then
--~             path, patt = pattern:match("^(.*)/(.-)$")
--~             if not (path and patt) then
--~                 path, patt = '.', pattern
--~             end
--~         end
--~         patt = patt:gsub("([%.%-%+])", "%%%1")
--~         patt = patt:gsub("%*", ".*")
--~         patt = patt:gsub("%?", ".")
--~         patt = "^" .. patt .. "$"
--~      -- print('path: ' .. path .. ' | pattern: ' .. patt .. ' | recurse: ' .. tostring(recurse))
--~         glob_pattern(path,patt,recurse,action)
--~         return t
--~     end
--~
--~     dir.glob = glob

    local attributes = lfs.attributes
    local walkdir    = lfs.dir

    local function glob_pattern(path,patt,recurse,action)
        local ok, scanner
        if path == "/" then
            ok, scanner = xpcall(function() return walkdir(path..".") end, function() end) -- kepler safe
        else
            ok, scanner = xpcall(function() return walkdir(path)      end, function() end) -- kepler safe
        end
        if ok and type(scanner) == "function" then
            if not path:find("/$") then path = path .. '/' end
            for name in scanner do
                local full = path .. name
                local mode = attributes(full,'mode')
                if mode == 'file' then
                    if full:find(patt) then
                        action(full)
                    end
                elseif recurse and (mode == "directory") and (name ~= '.') and (name ~= "..") then
                    glob_pattern(full,patt,recurse,action)
                end
            end
        end
    end

    dir.glob_pattern = glob_pattern

    --~ local function glob(pattern, action)
    --~     local t = { }
    --~     local path, rest, patt, recurse
    --~     local action = action or function(name) t[#t+1] = name end
    --~     local pattern = pattern:gsub("^%*%*","./**")
    --~     local pattern = pattern:gsub("/%*/","/**/")
    --~     path, rest = pattern:match("^(/)(.-)$")
    --~     if path then
    --~         path = path
    --~     else
    --~         path, rest = pattern:match("^([^/]*)/(.-)$")
    --~     end
    --~     if rest then
    --~         patt = rest:gsub("([%.%-%+])", "%%%1")
    --~     end
    --~     patt = patt:gsub("%*", "[^/]*")
    --~     patt = patt:gsub("%?", "[^/]")
    --~     patt = patt:gsub("%[%^/%]%*%[%^/%]%*", ".*")
    --~     if path == "" then path = "." end
    --~     recurse = patt:find("%.%*/") ~= nil
    --~     glob_pattern(path,patt,recurse,action)
    --~     return t
    --~ end

    local P, S, R, C, Cc, Cs, Ct, Cv, V = lpeg.P, lpeg.S, lpeg.R, lpeg.C, lpeg.Cc, lpeg.Cs, lpeg.Ct, lpeg.Cv, lpeg.V

    local pattern = Ct {
        [1] = (C(P(".") + P("/")^1) + C(R("az","AZ") * P(":") * P("/")^0) + Cc("./")) * V(2) * V(3),
        [2] = C(((1-S("*?/"))^0 * P("/"))^0),
        [3] = C(P(1)^0)
    }

    local filter = Cs ( (
        P("**") / ".*" +
        P("*")  / "[^/]*" +
        P("?")  / "[^/]" +
        P(".")  / "%%." +
        P("+")  / "%%+" +
        P("-")  / "%%-" +
        P(1)
    )^0 )

    local function glob(str,t)
        if type(str) == "table" then
            local t = t or { }
            for _, s in ipairs(str) do
                glob(s,t)
            end
            return t
        else
            local split = pattern:match(str)
            if split then
                local t = t or { }
                local action = action or function(name) t[#t+1] = name end
                local root, path, base = split[1], split[2], split[3]
                local recurse = base:find("**")
                local start = root .. path
                local result = filter:match(start .. base)
                glob_pattern(start,result,recurse,action)
                return t
            else
                return { }
            end
        end
    end

    dir.glob = glob

    --~ list = dir.glob("**/*.tif")
    --~ list = dir.glob("/**/*.tif")
    --~ list = dir.glob("./**/*.tif")
    --~ list = dir.glob("oeps/**/*.tif")
    --~ list = dir.glob("/oeps/**/*.tif")

    local function globfiles(path,recurse,func,files) -- func == pattern or function
        if type(func) == "string" then
            local s = func -- alas, we need this indirect way
            func = function(name) return name:find(s) end
        end
        files = files or { }
        for name in walkdir(path) do
            if name:find("^%.") then
                --- skip
            elseif attributes(name,'mode') == "directory" then
                if recurse then
                    globfiles(path .. "/" .. name,recurse,func,files)
                end
            elseif func then
                if func(name) then
                    files[#files+1] = path .. "/" .. name
                end
            else
                files[#files+1] = path .. "/" .. name
            end
        end
        return files
    end

    dir.globfiles = globfiles

    -- t = dir.glob("c:/data/develop/context/sources/**/????-*.tex")
    -- t = dir.glob("c:/data/develop/tex/texmf/**/*.tex")
    -- t = dir.glob("c:/data/develop/context/texmf/**/*.tex")
    -- t = dir.glob("f:/minimal/tex/**/*")
    -- print(dir.ls("f:/minimal/tex/**/*"))
    -- print(dir.ls("*.tex"))

    function dir.ls(pattern)
        return table.concat(glob(pattern),"\n")
    end

    --~ mkdirs("temp")
    --~ mkdirs("a/b/c")
    --~ mkdirs(".","/a/b/c")
    --~ mkdirs("a","b","c")

    local make_indeed = true -- false

    if string.find(os.getenv("PATH"),";") then

        function dir.mkdirs(...)
            local str, pth = "", ""
            for _, s in ipairs({...}) do
                if s ~= "" then
                    if str ~= "" then
                        str = str .. "/" .. s
                    else
                        str = s
                    end
                end
            end
            local first, middle, last
            local drive = false
            first, middle, last = str:match("^(//)(//*)(.*)$")
            if first then
                -- empty network path == local path
            else
                first, last = str:match("^(//)/*(.-)$")
                if first then
                    middle, last = str:match("([^/]+)/+(.-)$")
                    if middle then
                        pth = "//" .. middle
                    else
                        pth = "//" .. last
                        last = ""
                    end
                else
                    first, middle, last = str:match("^([a-zA-Z]:)(/*)(.-)$")
                    if first then
                        pth, drive = first .. middle, true
                    else
                        middle, last = str:match("^(/*)(.-)$")
                        if not middle then
                            last = str
                        end
                    end
                end
            end
            for s in last:gmatch("[^/]+") do
                if pth == "" then
                    pth = s
                elseif drive then
                    pth, drive = pth .. s, false
                else
                    pth = pth .. "/" .. s
                end
                if make_indeed and not lfs.isdir(pth) then
                    lfs.mkdir(pth)
                end
            end
            return pth, (lfs.isdir(pth) == true)
        end

--~         print(dir.mkdirs("","","a","c"))
--~         print(dir.mkdirs("a"))
--~         print(dir.mkdirs("a:"))
--~         print(dir.mkdirs("a:/b/c"))
--~         print(dir.mkdirs("a:b/c"))
--~         print(dir.mkdirs("a:/bbb/c"))
--~         print(dir.mkdirs("/a/b/c"))
--~         print(dir.mkdirs("/aaa/b/c"))
--~         print(dir.mkdirs("//a/b/c"))
--~         print(dir.mkdirs("///a/b/c"))
--~         print(dir.mkdirs("a/bbb//ccc/"))

        function dir.expand_name(str)
            local first, nothing, last = str:match("^(//)(//*)(.*)$")
            if first then
                first = lfs.currentdir() .. "/"
                first = first:gsub("\\","/")
            end
            if not first then
                first, last = str:match("^(//)/*(.*)$")
            end
            if not first then
                first, last = str:match("^([a-zA-Z]:)(.*)$")
                if first and not last:find("^/") then
                    local d = lfs.currentdir()
                    if lfs.chdir(first) then
                        first = lfs.currentdir()
                        first = first:gsub("\\","/")
                    end
                    lfs.chdir(d)
                end
            end
            if not first then
                first, last = lfs.currentdir(), str
                first = first:gsub("\\","/")
            end
            last = last:gsub("//","/")
            last = last:gsub("/%./","/")
            last = last:gsub("^/*","")
            first = first:gsub("/*$","")
            if last == "" then
                return first
            else
                return first .. "/" .. last
            end
        end

    else

        function dir.mkdirs(...)
            local str, pth = "", ""
            for _, s in ipairs({...}) do
                if s ~= "" then
                    if str ~= "" then
                        str = str .. "/" .. s
                    else
                        str = s
                    end
                end
            end
            str = str:gsub("/+","/")
            if str:find("^/") then
                pth = "/"
                for s in str:gmatch("[^/]+") do
                    local first = (pth == "/")
                    if first then
                        pth = pth .. s
                    else
                        pth = pth .. "/" .. s
                    end
                    if make_indeed and not first and not lfs.isdir(pth) then
                        lfs.mkdir(pth)
                    end
                end
            else
                pth = "."
                for s in str:gmatch("[^/]+") do
                    pth = pth .. "/" .. s
                    if make_indeed and not lfs.isdir(pth) then
                        lfs.mkdir(pth)
                    end
                end
            end
            return pth, (lfs.isdir(pth) == true)
        end

--~         print(dir.mkdirs("","","a","c"))
--~         print(dir.mkdirs("a"))
--~         print(dir.mkdirs("/a/b/c"))
--~         print(dir.mkdirs("/aaa/b/c"))
--~         print(dir.mkdirs("//a/b/c"))
--~         print(dir.mkdirs("///a/b/c"))
--~         print(dir.mkdirs("a/bbb//ccc/"))

        function dir.expand_name(str)
            if not str:find("^/") then
                str = lfs.currentdir() .. "/" .. str
            end
            str = str:gsub("//","/")
            str = str:gsub("/%./","/")
            return str
        end

    end

    dir.makedirs = dir.mkdirs

end end


-- filename : l-boolean.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-boolean'] = 1.001
if not boolean  then boolean  = { } end

function boolean.tonumber(b)
    if b then return 1 else return 0 end
end

function toboolean(str,tolerant)
    if tolerant then
        local tstr = type(str)
        if tstr == "string" then
            return str == "true" or str == "yes" or str == "on" or str == "1"
        elseif tstr == "number" then
            return tonumber(str) ~= 0
        elseif tstr == "nil" then
            return false
        else
            return str
        end
    elseif str == "true" then
        return true
    elseif str == "false" then
        return false
    else
        return str
    end
end

function string.is_boolean(str)
    if type(str) == "string" then
        if str == "true" or str == "yes" or str == "on" then
            return true
        elseif str == "false" or str == "no" or str == "off" then
            return false
        end
    end
    return nil
end

function boolean.alwaystrue()
    return true
end

function boolean.falsetrue()
    return false
end


if not modules then modules = { } end modules ['l-xml'] = {
    version   = 1.001,
    comment   = "this module is the basis for the lxml-* ones",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- RJ: key=value ... lpeg.Ca(lpeg.Cc({}) * (pattern-producing-key-and-value / rawset)^0)

-- some code may move to l-xmlext

--[[ldx--
<p>The parser used here is inspired by the variant discussed in the lua book, but
handles comment and processing instructions, has a different structure, provides
parent access; a first version used different tricky but was less optimized to we
went this route. First we had a find based parser, now we have an <l n='lpeg'/> based one.
The find based parser can be found in l-xml-edu.lua along with other older code.</p>

<p>Expecially the lpath code is experimental, we will support some of xpath, but
only things that make sense for us; as compensation it is possible to hook in your
own functions. Apart from preprocessing content for <l n='context'/> we also need
this module for process management, like handling <l n='ctx'/> and <l n='rlx'/>
files.</p>

<typing>
a/b/c /*/c
a/b/c/first() a/b/c/last() a/b/c/index(n) a/b/c/index(-n)
a/b/c/text() a/b/c/text(1) a/b/c/text(-1) a/b/c/text(n)
</typing>

<p>Beware, the interface may change. For instance at, ns, tg, dt may get more
verbose names. Once the code is stable we will also remove some tracing and
optimize the code.</p>
--ldx]]--

xml = xml or { }
tex = tex or { }

xml.trace_lpath = false
xml.trace_print = false
xml.trace_remap = false

local format, concat = string.format, table.concat

--~ local pairs, next, type = pairs, next, type

-- todo: some things per xml file, liek namespace remapping

--[[ldx--
<p>First a hack to enable namespace resolving. A namespace is characterized by
a <l n='url'/>. The following function associates a namespace prefix with a
pattern. We use <l n='lpeg'/>, which in this case is more than twice as fast as a
find based solution where we loop over an array of patterns. Less code and
much cleaner.</p>
--ldx]]--

xml.xmlns = xml.xmlns or { }

do

    local check = lpeg.P(false)
    local parse = check

    --[[ldx--
    <p>The next function associates a namespace prefix with an <l n='url'/>. This
    normally happens independent of parsing.</p>

    <typing>
    xml.registerns("mml","mathml")
    </typing>
    --ldx]]--

    function xml.registerns(namespace, pattern) -- pattern can be an lpeg
        check = check + lpeg.C(lpeg.P(pattern:lower())) / namespace
        parse = lpeg.P { lpeg.P(check) + 1 * lpeg.V(1) }
    end

    --[[ldx--
    <p>The next function also registers a namespace, but this time we map a
    given namespace prefix onto a registered one, using the given
    <l n='url'/>. This used for attributes like <t>xmlns:m</t>.</p>

    <typing>
    xml.checkns("m","http://www.w3.org/mathml")
    </typing>
    --ldx]]--

    function xml.checkns(namespace,url)
        local ns = parse:match(url:lower())
        if ns and namespace ~= ns then
            xml.xmlns[namespace] = ns
        end
    end

    --[[ldx--
    <p>Next we provide a way to turn an <l n='url'/> into a registered
    namespace. This used for the <t>xmlns</t> attribute.</p>

    <typing>
    resolvedns = xml.resolvens("http://www.w3.org/mathml")
    </typing>

    This returns <t>mml</t>.
    --ldx]]--

    function xml.resolvens(url)
         return parse:match(url:lower()) or ""
    end

    --[[ldx--
    <p>A namespace in an element can be remapped onto the registered
    one efficiently by using the <t>xml.xmlns</t> table.</p>
    --ldx]]--

end

--[[ldx--
<p>This version uses <l n='lpeg'/>. We follow the same approach as before, stack and top and
such. This version is about twice as fast which is mostly due to the fact that
we don't have to prepare the stream for cdata, doctype etc etc. This variant is
is dedicated to Luigi Scarso, who challenged me with 40 megabyte <l n='xml'/> files that
took 12.5 seconds to load (1.5 for file io and the rest for tree building). With
the <l n='lpeg'/> implementation we got that down to less 7.3 seconds. Loading the 14
<l n='context'/> interface definition files (2.6 meg) went down from 1.05 seconds to 0.55.</p>

<p>Next comes the parser. The rather messy doctype definition comes in many
disguises so it is no surprice that later on have to dedicate quite some
<l n='lpeg'/> code to it.</p>

<typing>
<!DOCTYPE Something PUBLIC "... ..." "..." [ ... ] >
<!DOCTYPE Something PUBLIC "... ..." "..." >
<!DOCTYPE Something SYSTEM "... ..." [ ... ] >
<!DOCTYPE Something SYSTEM "... ..." >
<!DOCTYPE Something [ ... ] >
<!DOCTYPE Something >
</typing>

<p>The code may look a bit complex but this is mostly due to the fact that we
resolve namespaces and attach metatables. There is only one public function:</p>

<typing>
local x = xml.convert(somestring)
</typing>

<p>An optional second boolean argument tells this function not to create a root
element.</p>
--ldx]]--

xml.strip_cm_and_dt = false -- an extra global flag, in case we have many includes

do

    -- not just one big nested table capture (lpeg overflow)

    local remove, nsremap, resolvens = table.remove, xml.xmlns, xml.resolvens

    local stack, top, dt, at, xmlns, errorstr, entities = {}, {}, {}, {}, {}, nil, {}

    local mt = { __tostring = xml.text }

    function xml.check_error(top,toclose)
        return ""
    end

    local strip   = false
    local cleanup = false

    function xml.set_text_cleanup(fnc)
        cleanup = fnc
    end

    local function add_attribute(namespace,tag,value)
        if tag == "xmlns" then
            xmlns[#xmlns+1] = resolvens(value)
            at[tag] = value
        elseif namespace == "xmlns" then
            xml.checkns(tag,value)
            at["xmlns:" .. tag] = value
        else
            at[tag] = value
        end
    end
    local function add_begin(spacing, namespace, tag)
        if #spacing > 0 then
            dt[#dt+1] = spacing
        end
        local resolved = (namespace == "" and xmlns[#xmlns]) or nsremap[namespace] or namespace
        top = { ns=namespace or "", rn=resolved, tg=tag, at=at, dt={}, __p__ = stack[#stack] }
        setmetatable(top, mt)
        dt = top.dt
        stack[#stack+1] = top
        at = { }
    end
    local function add_end(spacing, namespace, tag)
        if #spacing > 0 then
            dt[#dt+1] = spacing
        end
        local toclose = remove(stack)
        top = stack[#stack]
        if #stack < 1 then
            errorstr = format("nothing to close with %s %s", tag, xml.check_error(top,toclose) or "")
        elseif toclose.tg ~= tag then -- no namespace check
            errorstr = format("unable to close %s with %s %s", toclose.tg, tag, xml.check_error(top,toclose) or "")
        end
        dt = top.dt
        dt[#dt+1] = toclose
        if at.xmlns then
            remove(xmlns)
        end
    end
    local function add_empty(spacing, namespace, tag)
        if #spacing > 0 then
            dt[#dt+1] = spacing
        end
        local resolved = (namespace == "" and xmlns[#xmlns]) or nsremap[namespace] or namespace
        top = stack[#stack]
        dt = top.dt
        local t = { ns=namespace or "", rn=resolved, tg=tag, at=at, dt={}, __p__ = top }
        dt[#dt+1] = t
        setmetatable(t, mt)
        at = { }
        if at.xmlns then
            remove(xmlns)
        end
    end
    local function add_text(text)
        if cleanup and #text > 0 then
            dt[#dt+1] = cleanup(text)
        else
            dt[#dt+1] = text
        end
    end
    local function add_special(what, spacing, text)
        if #spacing > 0 then
            dt[#dt+1] = spacing
        end
        if strip and (what == "@cm@" or what == "@dt@") then
            -- forget it
        else
            dt[#dt+1] = { special=true, ns="", tg=what, dt={text} }
        end
    end
    local function set_message(txt)
        errorstr = "garbage at the end of the file: " .. txt:gsub("([ \n\r\t]*)","")
    end

    local P, S, R, C, V = lpeg.P, lpeg.S, lpeg.R, lpeg.C, lpeg.V

    local space            = S(' \r\n\t')
    local open             = P('<')
    local close            = P('>')
    local squote           = S("'")
    local dquote           = S('"')
    local equal            = P('=')
    local slash            = P('/')
    local colon            = P(':')
    local valid            = R('az', 'AZ', '09') + S('_-.')
    local name_yes         = C(valid^1) * colon * C(valid^1)
    local name_nop         = C(P(true)) * C(valid^1)
    local name             = name_yes + name_nop

    local utfbom           = P('\000\000\254\255') + P('\255\254\000\000') +
                             P('\255\254') + P('\254\255') + P('\239\187\191') -- no capture

    local spacing          = C(space^0)
    local justtext         = C((1-open)^1)
    local somespace        = space^1
    local optionalspace    = space^0

    local value            = (squote * C((1 - squote)^0) * squote) + (dquote * C((1 - dquote)^0) * dquote)
    local attribute        = (somespace * name * optionalspace * equal * optionalspace * value) / add_attribute
    local attributes       = attribute^0

    local text             = justtext / add_text
    local balanced         = P { "[" * ((1 - S"[]") + V(1))^0 * "]" } -- taken from lpeg manual, () example

    local emptyelement     = (spacing * open         * name * attributes * optionalspace * slash * close) / add_empty
    local beginelement     = (spacing * open         * name * attributes * optionalspace         * close) / add_begin
    local endelement       = (spacing * open * slash * name              * optionalspace         * close) / add_end

    local begincomment     = open * P("!--")
    local endcomment       = P("--") * close
    local begininstruction = open * P("?")
    local endinstruction   = P("?") * close
    local begincdata       = open * P("![CDATA[")
    local endcdata         = P("]]") * close

    local someinstruction  = C((1 - endinstruction)^0)
    local somecomment      = C((1 - endcomment    )^0)
    local somecdata        = C((1 - endcdata      )^0)

    function entity(k,v) entities[k] = v end

    local begindoctype     = open * P("!DOCTYPE")
    local enddoctype       = close
    local beginset         = P("[")
    local endset           = P("]")
    local doctypename      = C((1-somespace)^0)
    local elementdoctype   = optionalspace * P("<!ELEMENT") * (1-close)^0 * close
    local entitydoctype    = optionalspace * P("<!ENTITY") * somespace * (doctypename * somespace * value)/entity * optionalspace * close
    local publicdoctype    = doctypename * somespace * P("PUBLIC") * somespace * value * somespace * value * somespace
    local systemdoctype    = doctypename * somespace * P("SYSTEM") * somespace * value * somespace
    local definitiondoctype= doctypename * somespace * beginset * P(elementdoctype + entitydoctype)^0 * optionalspace * endset
    local simpledoctype    = (1-close)^1                     -- * balanced^0
    local somedoctype      = C((somespace * (publicdoctype + systemdoctype + definitiondoctype + simpledoctype) * optionalspace)^0)

    local instruction      = (spacing * begininstruction * someinstruction * endinstruction) / function(...) add_special("@pi@",...) end
    local comment          = (spacing * begincomment     * somecomment     * endcomment    ) / function(...) add_special("@cm@",...) end
    local cdata            = (spacing * begincdata       * somecdata       * endcdata      ) / function(...) add_special("@cd@",...) end
    local doctype          = (spacing * begindoctype     * somedoctype     * enddoctype    ) / function(...) add_special("@dt@",...) end

    --  nicer but slower:
    --
    --  local instruction = (lpeg.Cc("@pi@") * spacing * begininstruction * someinstruction * endinstruction) / add_special
    --  local comment     = (lpeg.Cc("@cm@") * spacing * begincomment     * somecomment     * endcomment    ) / add_special
    --  local cdata       = (lpeg.Cc("@cd@") * spacing * begincdata       * somecdata       * endcdata      ) / add_special
    --  local doctype     = (lpeg.Cc("@dt@") * spacing * begindoctype     * somedoctype     * enddoctype    ) / add_special

    local trailer = space^0 * (justtext/set_message)^0

    --  comment + emptyelement + text + cdata + instruction + V("parent"), -- 6.5 seconds on 40 MB database file
    --  text + comment + emptyelement + cdata + instruction + V("parent"), -- 5.8
    --  text + V("parent") + emptyelement + comment + cdata + instruction, -- 5.5

    local grammar = P { "preamble",
        preamble = utfbom^0 * instruction^0 * (doctype + comment + instruction)^0 * V("parent") * trailer,
        parent   = beginelement * V("children")^0 * endelement,
        children = text + V("parent") + emptyelement + comment + cdata + instruction,
    }

    -- todo: xml.new + properties like entities and strip and such (store in root)

    function xml.convert(data, no_root, strip_cm_and_dt, given_entities) -- maybe use table met k/v (given_entities may disapear)
        strip = strip_cm_and_dt or xml.strip_cm_and_dt
        stack, top, at, xmlns, errorstr, result, entities = {}, {}, {}, {}, nil, nil, given_entities or {}
        stack[#stack+1] = top
        top.dt = { }
        dt = top.dt
        if not data or data == "" then
            errorstr = "empty xml file"
        elseif not grammar:match(data) then
            errorstr = "invalid xml file"
        else
            errorstr = ""
        end
        if errorstr and errorstr ~= "" then
            result = { dt = { { ns = "", tg = "error", dt = { errorstr }, at={}, er = true } }, error = true }
            setmetatable(stack, mt)
            if xml.error_handler then xml.error_handler("load",errorstr) end
        else
            result = stack[1]
        end
        if not no_root then
            result = { special = true, ns = "", tg = '@rt@', dt = result.dt, at={}, entities = entities }
            setmetatable(result, mt)
            local rdt = result.dt
            for k=1,#rdt do
                local v = rdt[k]
                if type(v) == "table" and not v.special then -- always table -)
                    result.ri = k -- rootindex
                    break
                end
            end
        end
        return result
    end

    --[[ldx--
    <p>Packaging data in an xml like table is done with the following
    function. Maybe it will go away (when not used).</p>
    --ldx]]--

    function xml.is_valid(root)
        return root and root.dt and root.dt[1] and type(root.dt[1]) == "table" and not root.dt[1].er
    end

    function xml.package(tag,attributes,data)
        local ns, tg = tag:match("^(.-):?([^:]+)$")
        local t = { ns = ns, tg = tg, dt = data or "", at = attributes or {} }
        setmetatable(t, mt)
        return t
    end

    function xml.is_valid(root)
        return root and not root.error
    end

    xml.error_handler = (logs and logs.report) or (input and input.report) or print

end

--[[ldx--
<p>We cannot load an <l n='lpeg'/> from a filehandle so we need to load
the whole file first. The function accepts a string representing
a filename or a file handle.</p>
--ldx]]--

function xml.load(filename)
    if type(filename) == "string" then
        local f = io.open(filename,'r')
        if f then
            local root = xml.convert(f:read("*all"))
            f:close()
            return root
        else
            return xml.convert("")
        end
    elseif filename then -- filehandle
        return xml.convert(filename:read("*all"))
    else
        return xml.convert("")
    end
end

--[[ldx--
<p>When we inject new elements, we need to convert strings to
valid trees, which is what the next function does.</p>
--ldx]]--

function xml.toxml(data)
    if type(data) == "string" then
        local root = { xml.convert(data,true) }
        return (#root > 1 and root) or root[1]
    else
        return data
    end
end

--[[ldx--
<p>For copying a tree we use a dedicated function instead of the
generic table copier. Since we know what we're dealing with we
can speed up things a bit. The second argument is not to be used!</p>
--ldx]]--

do

    function copy(old,tables)
        if old then
            tables = tables or { }
            local new = { }
            if not tables[old] then
                tables[old] = new
            end
            for k,v in pairs(old) do
                new[k] = (type(v) == "table" and (tables[v] or copy(v, tables))) or v
            end
            local mt = getmetatable(old)
            if mt then
                setmetatable(new,mt)
            end
            return new
        else
            return { }
        end
    end

    xml.copy = copy

end

--[[ldx--
<p>In <l n='context'/> serializing the tree or parts of the tree is a major
actitivity which is why the following function is pretty optimized resulting
in a few more lines of code than needed. The variant that uses the formatting
function for all components is about 15% slower than the concatinating
alternative.</p>
--ldx]]--

do

    -- todo: add <?xml version='1.0' standalone='yes'?> when not present

    local fallbackhandle = (tex and tex.sprint) or io.write

    local function serialize(e, handle, textconverter, attributeconverter, specialconverter, nocommands)
        if not e then
            return
        elseif not nocommands then
            local ec = e.command
            if ec ~= nil then -- we can have all kind of types

if e.special then -- todo test for true/false
    local etg, edt = e.tg, e.dt
    local spc = specialconverter and specialconverter[etg]
    if spc then
--~ print("SPECIAL",etg,table.serialize(specialconverter), spc)
        local result = spc(edt[1])
        if result then
            handle(result)
            return
        else
            -- no need to handle any further
        end
    end
end

                local xc = xml.command
                if xc then
                    xc(e,ec)
                    return
                end
            end
        end
        handle = handle or fallbackhandle
        local etg = e.tg
        if etg then
            if e.special then
                local edt = e.dt
                local spc = specialconverter and specialconverter[etg]
                if spc then
                    local result = spc(edt[1])
                    if result then
                        handle(result)
                    else
                        -- no need to handle any further
                    end
                elseif etg == "@pi@" then
                --  handle(format("<?%s?>",edt[1]))
                    handle("<?" .. edt[1] .. "?>")
                elseif etg == "@cm@" then
                --  handle(format("<!--%s-->",edt[1]))
                    handle("<!--" .. edt[1] .. "-->")
                elseif etg == "@cd@" then
                --  handle(format("<![CDATA[%s]]>",edt[1]))
                    handle("<![CDATA[" .. edt[1] .. "]]>")
                elseif etg == "@dt@" then
                --  handle(format("<!DOCTYPE %s>",edt[1]))
                    handle("<!DOCTYPE " .. edt[1] .. ">")
                elseif etg == "@rt@" then
                    serialize(edt,handle,textconverter,attributeconverter,specialconverter,nocommands)
                end
            else
                local ens, eat, edt, ern = e.ns, e.at, e.dt, e.rn
                local ats = eat and next(eat) and { } -- type test maybe faster
                if ats then
                    if attributeconverter then
                        for k,v in pairs(eat) do
                            ats[#ats+1] = format('%s=%q',k,attributeconverter(v))
                        end
                    else
                        for k,v in pairs(eat) do
                            ats[#ats+1] = format('%s=%q',k,v)
                        end
                    end
                end
                if ern and xml.trace_remap and ern ~= ens then
--~                     if ats then
--~                         ats[#ats+1] = format("xmlns:remapped='%s'",ern)
--~                     else
--~                         ats = { format("xmlns:remapped='%s'",ern) }
--~                     end
--~ if ats then
--~     ats[#ats+1] = format("remappedns='%s'",ens or '-')
--~ else
--~     ats = { format("remappedns='%s'",ens or '-') }
--~ end
ens = ern
                end
                if ens ~= "" then
                    if edt and #edt > 0 then
                        if ats then
                        --  handle(format("<%s:%s %s>",ens,etg,concat(ats," ")))
                            handle("<" .. ens .. ":" .. etg .. " " .. concat(ats," ") .. ">")
                        else
                        --  handle(format("<%s:%s>",ens,etg))
                            handle("<" .. ens .. ":" .. etg .. ">")
                        end
                        for i=1,#edt do
                            local e = edt[i]
                            if type(e) == "string" then
                                if textconverter then
                                    handle(textconverter(e))
                                else
                                    handle(e)
                                end
                            else
                                serialize(e,handle,textconverter,attributeconverter,specialconverter,nocommands)
                            end
                        end
                    --  handle(format("</%s:%s>",ens,etg))
                        handle("</" .. ens .. ":" .. etg .. ">")
                    else
                        if ats then
                        --  handle(format("<%s:%s %s/>",ens,etg,concat(ats," ")))
                            handle("<" .. ens .. ":" .. etg .. " " .. concat(ats," ") .. "/>")
                        else
                        --  handle(format("<%s:%s/>",ens,etg))
                            handle("<" .. ens .. ":" .. etg .. "/>")
                        end
                    end
                else
                    if edt and #edt > 0 then
                        if ats then
                        --  handle(format("<%s %s>",etg,concat(ats," ")))
                            handle("<" .. etg .. " " .. concat(ats," ") .. ">")
                        else
                        --  handle(format("<%s>",etg))
                            handle("<" .. etg .. ">")
                        end
                        for i=1,#edt do
                            serialize(edt[i],handle,textconverter,attributeconverter,specialconverter,nocommands)
                        end
                    --  handle(format("</%s>",etg))
                        handle("</" .. etg .. ">")
                    else
                        if ats then
                        --  handle(format("<%s %s/>",etg,concat(ats," ")))
                            handle("<" .. etg .. " " .. concat(ats," ") .. "/>")
                        else
                        --  handle(format("<%s/>",etg))
                            handle("<" .. etg .. "/>")
                        end
                    end
                end
            end
        elseif type(e) == "string" then
            if textconverter then
                handle(textconverter(e))
            else
                handle(e)
            end
        else
            for i=1,#e do
                serialize(e[i],handle,textconverter,attributeconverter,specialconverter,nocommands)
            end
        end
    end

    xml.serialize = serialize

    function xml.checkbom(root) -- can be made faster
        if root.ri then
            local dt, found = root.dt, false
            for k,v in ipairs(dt) do
                if type(v) == "table" and v.special and v.tg == "@pi" and v.dt:find("xml.*version=") then
                    found = true
                    break
                end
            end
            if not found then
                table.insert(dt, 1, { special=true, ns="", tg="@pi@", dt = { "xml version='1.0' standalone='yes'"} } )
                table.insert(dt, 2, "\n" )
            end
        end
    end

    --[[ldx--
    <p>At the cost of some 25% runtime overhead you can first convert the tree to a string
    and then handle the lot.</p>
    --ldx]]--

    function xml.tostring(root) -- 25% overhead due to collecting
        if root then
        if type(root) == 'string' then
            return root
        elseif next(root) then -- next is faster than type (and >0 test)
            local result = { }
            serialize(root,function(s) result[#result+1] = s end)
            return concat(result,"")
        end
    end
        return ""
    end

end

--[[ldx--
<p>The next function operated on the content only and needs a handle function
that accepts a string.</p>
--ldx]]--

function xml.string(e,handle)
    if not handle or (e.special and e.tg ~= "@rt@") then
        -- nothing
    elseif e.tg then
        local edt = e.dt
        if edt then
            for i=1,#edt do
                xml.string(edt[i],handle)
            end
        end
    else
        handle(e)
    end
end

--[[ldx--
<p>How you deal with saving data depends on your preferences. For a 40 MB database
file the timing on a 2.3 Core Duo are as follows (time in seconds):</p>

<lines>
1.3 : load data from file to string
6.1 : convert string into tree
5.3 : saving in file using xmlsave
6.8 : converting to string using xml.tostring
3.6 : saving converted string in file
</lines>

<p>The save function is given below.</p>
--ldx]]--

function xml.save(root,name)
    local f = io.open(name,"w")
    if f then
        xml.serialize(root,function(s) f:write(s) end)
        f:close()
    end
end

--[[ldx--
<p>A few helpers:</p>
--ldx]]--

function xml.body(root)
    return (root.ri and root.dt[root.ri]) or root
end

function xml.text(root)
    return (root and xml.tostring(root)) or ""
end

function xml.content(root) -- bugged
    return (root and root.dt and xml.tostring(root.dt)) or ""
end

--[[ldx--
<p>The next helper erases an element but keeps the table as it is,
and since empty strings are not serialized (effectively) it does
not harm. Copying the table would take more time. Usage:</p>

<typing>
dt[k] = xml.empty() or xml.empty(dt,k)
</typing>
--ldx]]--

function xml.empty(dt,k)
    if dt and k then
        dt[k] = ""
        return dt[k]
    else
        return ""
    end
end

--[[ldx--
<p>The next helper assigns a tree (or string). Usage:</p>

<typing>
dt[k] = xml.assign(root) or xml.assign(dt,k,root)
</typing>
--ldx]]--

function xml.assign(dt,k,root)
    if dt and k then
        dt[k] = (type(root) == "table" and xml.body(root)) or root
        return dt[k]
    else
        return xml.body(root)
    end
end

--[[ldx--
<p>We've now arrived at an intersting part: accessing the tree using a subset
of <l n='xpath'/> and since we're not compatible we call it <l n='lpath'/>. We
will explain more about its usage in other documents.</p>
--ldx]]--

do

    xml.functions = xml.functions or { }

    local functions = xml.functions

    local actions = {
        [10] = "stay",
        [11] = "parent",
        [12] = "subtree root",
        [13] = "document root",
        [14] = "any",
        [15] = "many",
        [16] = "initial",
        [20] = "match",
        [21] = "match one of",
        [22] = "match and attribute eq",
        [23] = "match and attribute ne",
        [24] = "match one of and attribute eq",
        [25] = "match one of and attribute ne",
        [27] = "has attribute",
        [28] = "has value",
        [29] = "fast match",
        [30] = "select",
        [31] = "expression",
        [40] = "processing instruction",
    }

    --~     local function make_expression(str) --could also be an lpeg
    --~         str = str:gsub("@([a-zA-Z%-_]+)", "(a['%1'] or '')")
    --~         str = str:gsub("position%(%)", "i")
    --~         str = str:gsub("text%(%)", "t")
    --~         str = str:gsub("!=", "~=")
    --~         str = str:gsub("([^=!~<>])=([^=!~<>])", "%1==%2")
    --~         str = str:gsub("([a-zA-Z%-_]+)%(", "functions.%1(")
    --~         return str, loadstring(format("return function(functions,i,a,t) return %s end", str))()
    --~     end

    -- a rather dumb lpeg

    local P, S, R, C, V, Cc = lpeg.P, lpeg.S, lpeg.R, lpeg.C, lpeg.V, lpeg.Cc

    local lp_position  = P("position()") / "id"
    local lp_text      = P("text()")     / "tx"
    local lp_name      = P("name()")     / "((rt.ns~='' and rt.ns..':'..rt.tg) or '')"
    local lp_tag       = P("tag()")      / "(rt.tg or '')"
    local lp_ns        = P("ns()")       / "(rt.ns or '')"
    local lp_noequal   = P("!=")         / "~=" + P("<=") + P(">=") + P("==")
    local lp_doequal   = P("=")          / "=="
    local lp_attribute = P("@")          / "" * Cc("(at['") * R("az","AZ","--","__")^1 * Cc("'] or '')")

    local lp_function  = C(R("az","AZ","--","__")^1) * P("(") / function(t)
        if functions[t] then
            return "functions." .. t .. "("
        else
            return "functions.error("
        end
    end

    local lparent  = lpeg.P("(")
    local rparent  = lpeg.P(")")
    local noparent = 1 - (lparent+rparent)
    local nested   = lpeg.P{lparent * (noparent + lpeg.V(1))^0 * rparent}
    local value    = lpeg.P(lparent * lpeg.C((noparent + nested)^0) * rparent)

--~ local value = P { "(" * C(((1 - S("()")) + V(1))^0) * ")" }

    local lp_special = (C(P("name")+P("text")+P("tag"))) * value / function(t,s)
        if functions[t] then
            if s then
                return "functions." .. t .. "(rt,k," .. s ..")"
            else
                return "functions." .. t .. "(rt,k)"
            end
        else
            return "functions.error(" .. t .. ")"
        end
    end

    local converter = lpeg.Cs ( (
        lp_position +
        lp_text + lp_name + -- fast one
        lp_special +
        lp_noequal + lp_doequal +
        lp_attribute +
        lp_function +
    1 )^1 )

    local function make_expression(str)
        str = converter:match(str)
        return str, loadstring(format("return function(functions,id,at,tx,rt,k) return %s end", str))()
    end

    local map = { }

    local space             = S(' \r\n\t')
    local squote            = S("'")
    local dquote            = S('"')
    local lparent           = P('(')
    local rparent           = P(')')
    local atsign            = P('@')
    local lbracket          = P('[')
    local rbracket          = P(']')
    local exclam            = P('!')
    local period            = P('.')
    local eq                = P('==') + P('=')
    local ne                = P('<>') + P('!=')
    local star              = P('*')
    local slash             = P('/')
    local colon             = P(':')
    local bar               = P('|')
    local hat               = P('^')
    local valid             = R('az', 'AZ', '09') + S('_-')
--~     local name_yes          = C(valid^1 + star) * colon * C(valid^1 + star) -- permits ns:* *:tg *:*
--~     local name_nop          = C(P(true)) * C(valid^1)
    local name_yes          = C(valid^1 + star) * colon * C(valid^1 + star) -- permits ns:* *:tg *:*
    local name_nop          = Cc("*") * C(valid^1)
    local name              = name_yes + name_nop
    local number            = C((S('+-')^0 * R('09')^1)) / tonumber
    local names             = (bar^0 * name)^1
    local morenames         = name * (bar^0 * name)^1
    local instructiontag    = P('pi::')
    local spacing           = C(space^0)
    local somespace         = space^1
    local optionalspace     = space^0
    local text              = C(valid^0)
    local value             = (squote * C((1 - squote)^0) * squote) + (dquote * C((1 - dquote)^0) * dquote)
    local empty             = 1-slash

    local is_eq             = lbracket * atsign * name * eq * value * rbracket
    local is_ne             = lbracket * atsign * name * ne * value * rbracket
    local is_attribute      = lbracket * atsign * name              * rbracket
    local is_value          = lbracket *          value             * rbracket
    local is_number         = lbracket *          number            * rbracket

    local nobracket         = 1-(lbracket+rbracket)  -- must be improved
    local is_expression     = lbracket * C(((C(nobracket^1))/make_expression)) * rbracket

    local is_expression     = lbracket * (C(nobracket^1))/make_expression * rbracket

    local is_one            =          name
    local is_none           = exclam * name
    local is_one_of         =          ((lparent * names * rparent) + morenames)
    local is_none_of        = exclam * ((lparent * names * rparent) + morenames)

    local stay                     = (period                )
    local parent                   = (period * period       ) / function(   ) map[#map+1] = { 11             } end
    local subtreeroot              = (slash + hat           ) / function(   ) map[#map+1] = { 12             } end
    local documentroot             = (hat * hat             ) / function(   ) map[#map+1] = { 13             } end
    local any                      = (star                  ) / function(   ) map[#map+1] = { 14             } end
    local many                     = (star * star           ) / function(   ) map[#map+1] = { 15             } end
    local initial                  = (hat * hat * hat       ) / function(   ) map[#map+1] = { 16             } end

    local match                    = (is_one                ) / function(...) map[#map+1] = { 20, true , ... } end
    local match_one_of             = (is_one_of             ) / function(...) map[#map+1] = { 21, true , ... } end
    local dont_match               = (is_none               ) / function(...) map[#map+1] = { 20, false, ... } end
    local dont_match_one_of        = (is_none_of            ) / function(...) map[#map+1] = { 21, false, ... } end

    local match_and_eq             = (is_one     * is_eq    ) / function(...) map[#map+1] = { 22, true , ... } end
    local match_and_ne             = (is_one     * is_ne    ) / function(...) map[#map+1] = { 23, true , ... } end
    local dont_match_and_eq        = (is_none    * is_eq    ) / function(...) map[#map+1] = { 22, false, ... } end
    local dont_match_and_ne        = (is_none    * is_ne    ) / function(...) map[#map+1] = { 23, false, ... } end

    local match_one_of_and_eq      = (is_one_of  * is_eq    ) / function(...) map[#map+1] = { 24, true , ... } end
    local match_one_of_and_ne      = (is_one_of  * is_ne    ) / function(...) map[#map+1] = { 25, true , ... } end
    local dont_match_one_of_and_eq = (is_none_of * is_eq    ) / function(...) map[#map+1] = { 24, false, ... } end
    local dont_match_one_of_and_ne = (is_none_of * is_ne    ) / function(...) map[#map+1] = { 25, false, ... } end

    local has_attribute            = (is_one  * is_attribute) / function(...) map[#map+1] = { 27, true , ... } end
    local has_value                = (is_one  * is_value    ) / function(...) map[#map+1] = { 28, true , ... } end
    local dont_has_attribute       = (is_none * is_attribute) / function(...) map[#map+1] = { 27, false, ... } end
    local dont_has_value           = (is_none * is_value    ) / function(...) map[#map+1] = { 28, false, ... } end
    local position                 = (is_one  * is_number   ) / function(...) map[#map+1] = { 30, true,  ... } end
    local dont_position            = (is_none * is_number   ) / function(...) map[#map+1] = { 30, false, ... } end

    local expression               = (is_one  * is_expression)/ function(...) map[#map+1] = { 31, true,  ... } end
    local dont_expression          = (is_none * is_expression)/ function(...) map[#map+1] = { 31, false, ... } end

    local self_expression          = (         is_expression) / function(...) if #map == 0 then map[#map+1] = { 11 } end
                                                                              map[#map+1] = { 31, true,  "*", "*", ... } end
    local dont_self_expression     = (exclam * is_expression) / function(...) if #map == 0 then map[#map+1] = { 11 } end
                                                                              map[#map+1] = { 31, false, "*", "*", ... } end

    local instruction              = (instructiontag * text ) / function(...) map[#map+1] = { 40,        ... } end
    local nothing                  = (empty                 ) / function(   ) map[#map+1] = { 15             } end -- 15 ?
    local crap                     = (1-slash)^1

    -- a few ugly goodies:

    local docroottag               = P('^^')             / function(   ) map[#map+1] = { 12             } end
    local subroottag               = P('^')              / function(   ) map[#map+1] = { 13             } end
    local roottag                  = P('root::')         / function(   ) map[#map+1] = { 12             } end
    local parenttag                = P('parent::')       / function(   ) map[#map+1] = { 11             } end
    local childtag                 = P('child::')
    local selftag                  = P('self::')

    -- there will be more and order will be optimized

    local selector = (
        instruction +
        many + any +
        parent + stay +
        dont_position + position +
        dont_match_one_of_and_eq + dont_match_one_of_and_ne +
        match_one_of_and_eq + match_one_of_and_ne +
        dont_match_and_eq + dont_match_and_ne +
        match_and_eq + match_and_ne +
        dont_expression + expression +
        dont_self_expression + self_expression +
        has_attribute + has_value +
        dont_match_one_of + match_one_of +
        dont_match + match +
        crap + empty
    )

    local grammar = P { "startup",
        startup  = (initial + documentroot + subtreeroot + roottag + docroottag + subroottag)^0 * V("followup"),
        followup = ((slash + parenttag + childtag + selftag)^0 * selector)^1,
    }

    local function compose(str)
        if not str or str == "" then
            -- wildcard
            return true
        elseif str == '/' then
            -- root
            return false
        else
            map = { }
            grammar:match(str)
            if #map == 0 then
                return true
            else
                local m = map[1][1]
                if #map == 1 then
                    if m == 14 or m == 15 then
                        -- wildcard
                        return true
                    elseif m == 12 then
                        -- root
                        return false
                    end
                elseif #map == 2 and m == 12 and map[2][1] == 20 then
                --  return { { 29, map[2][2], map[2][3], map[2][4], map[2][5] } }
                    map[2][1] = 29
                    return { map[2] }
                end
                if m ~= 11 and m ~= 12 and m ~= 13 and m ~= 14 and m ~= 15 and m ~= 16 then
                    table.insert(map, 1, { 16 })
                end
            --  print((table.serialize(map)):gsub("[ \n]+"," "))
                return map
            end
        end
    end

    local cache = { }

    function xml.lpath(pattern,trace)
        if type(pattern) == "string" then
            local result = cache[pattern]
            if not result then
                result = compose(pattern)
                cache[pattern] = result
            end
            if trace or xml.trace_lpath then
                xml.lshow(result)
            end
            return result
        else
            return pattern
        end
    end

    local fallbackreport = (texio and texio.write) or io.write

    function xml.lshow(pattern,report)
        report = report or fallbackreport
        local lp = xml.lpath(pattern)
        if lp == false then
            report(" -: root\n")
        elseif lp == true then
            report(" -: wildcard\n")
        else
            if type(pattern) == "string" then
                report(format("pattern: %s\n",pattern))
            end
            for k,v in ipairs(lp) do
                if #v > 1 then
                    local t = { }
                    for i=2,#v do
                        local vv = v[i]
                        if type(vv) == "string" then
                            t[#t+1] = (vv ~= "" and vv) or "#"
                        elseif type(vv) == "boolean" then
                            t[#t+1] = (vv and "==") or "<>"
                        end
                    end
                    report(format("%2i: %s %s -> %s\n", k,v[1],actions[v[1]],concat(t," ")))
                else
                    report(format("%2i: %s %s\n", k,v[1],actions[v[1]]))
                end
            end
        end
    end

    function xml.xshow(e,...) -- also handy when report is given, use () to isolate first e
        local t = { ... }
        local report = (type(t[#t]) == "function" and t[#t]) or fallbackreport
        if e == nil then
            report("<!-- no element -->\n")
        elseif type(e) ~= "table" then
            report(tostring(e))
        elseif e.tg then
            report(tostring(e) .. "\n")
        else
            for i=1,#e do
                report(tostring(e[i]) .. "\n")
            end
        end
    end

end

--[[ldx--
<p>An <l n='lpath'/> is converted to a table with instructions for traversing the
tree. Hoever, simple cases are signaled by booleans. Because we don't know in
advance what we want to do with the found element the handle gets three arguments:</p>

<lines>
<t>r</t> : the root element of the data table
<t>d</t> : the data table of the result
<t>t</t> : the index in the data table of the result
</lines>

<p> Access to the root and data table makes it possible to construct insert and delete
functions.</p>
--ldx]]--

do

    local functions = xml.functions

    functions.contains = string.find
    functions.find     = string.find
    functions.upper    = string.upper
    functions.lower    = string.lower
    functions.number   = tonumber
    functions.boolean  = toboolean

    functions.oneof = function(s,...) -- slow
        local t = {...} for i=1,#t do if s == t[i] then return true end end return false
    end
    functions.error = function(str)
        xml.error_handler("unknown function in lpath expression",str)
        return false
    end
    functions.text = function(root,k,n) -- unchecked, maybe one deeper
        local t = type(t)
        if t == "string" then
            return t
        else -- todo n
            local rdt = root.dt
            return (rdt and rdt[k]) or root[k] or ""
        end
    end
    functions.name = function(root,k,n)
        -- way too fuzzy
        local found
        if not k or not n then
            local ns, tg = root.rn or root.ns or "", root.tg
            if not tg then
                for i=1,#root do
                    local e = root[i]
                    if type(e) == "table" then
                        found = e
                        break
                    end
                end
            elseif ns ~= "" then
                return ns .. ":" .. tg
            else
                return tg
            end
        elseif n == 0 then
            local e = root[k]
            if type(e) ~= "table" then
                found = e
            end
        elseif n < 0 then
            for i=k-1,1,-1 do
                local e = root[i]
                if type(e) == "table" then
                    if n == -1 then
                        found = e
                        break
                    else
                        n = n + 1
                    end
                end
            end
        else
--~ print(k,n,#root)
            for i=k+1,#root,1 do
                local e = root[i]
                if type(e) == "table" then
                    if n == 1 then
                        found = e
                        break
                    else
                        n = n - 1
                    end
                end
            end
        end
        if found then
            local ns, tg = found.rn or found.ns or "", found.tg
            if ns ~= "" then
                return ns .. ":" .. tg
            else
                return tg
            end
        else
            return ""
        end
    end

    local function traverse(root,pattern,handle,reverse,index,parent,wildcard) -- multiple only for tags, not for namespaces
        if not root then -- error
            return false
        elseif pattern == false then -- root
            handle(root,root.dt,root.ri)
            return false
        elseif pattern == true then -- wildcard
            local rootdt = root.dt
            if rootdt then
                local start, stop, step = 1, #rootdt, 1
                if reverse then
                    start, stop, step = stop, start, -1
                end
                for k=start,stop,step do
                    if handle(root,rootdt,root.ri or k)            then return false end
                    if not traverse(rootdt[k],true,handle,reverse) then return false end
                end
            end
            return false
        elseif root.dt then
            index = index or 1
            local action = pattern[index]
            local command = action[1]
            if command == 29 then -- fast case /oeps
                local rootdt = root.dt
                for k=1,#rootdt do
                    local e = rootdt[k]
                    local tg = e.tg
                    if e.tg then
                        local ns = e.rn or e.ns
                        local ns_a, tg_a = action[3], action[4]
                        local matched = (ns_a == "*" or ns == ns_a) and (tg_a == "*" or tg == tg_a)
                        if not action[2] then matched = not matched end
                        if matched then
                            if handle(root,rootdt,k) then return false end
                        end
                    end
                end
            elseif command == 11 then -- parent
                local ep = root.__p__ or parent
                if index < #pattern then
                    if not traverse(ep,pattern,handle,reverse,index+1,root) then return false end
                elseif handle(root,rootdt,k) then
                    return false
                end
            else
                if (command == 16 or command == 12) and index == 1 then -- initial
                --  wildcard = true
                    wildcard = command == 16 -- ok?
                    index = index + 1
                    action = pattern[index]
                    command = action and action[1] or 0 -- something is wrong
                end
                if command == 11 then -- parent
                    local ep = root.__p__ or parent
                    if index < #pattern then
                        if not traverse(ep,pattern,handle,reverse,index+1,root) then return false end
                    elseif handle(root,rootdt,k) then
                        return false
                    end
                else
                    local rootdt = root.dt
                    local start, stop, step, n, dn = 1, #rootdt, 1, 0, 1
                    if command == 30 then
                        if action[5] < 0 then
                            start, stop, step = stop, start, -1
                            dn = -1
                        end
                    elseif reverse and index == #pattern then
                        start, stop, step = stop, start, -1
                    end
                    local idx = 0
                    for k=start,stop,step do -- we used to have functions for all but a case is faster
                        local e = rootdt[k]
                        local ns, tg = e.rn or e.ns, e.tg
                        if tg then
                            idx = idx + 1
                            if command == 30 then
                                local ns_a, tg_a = action[3], action[4]
                                if tg == tg_a then
                                    matched = ns_a == "*" or ns == ns_a
                                elseif tg_a == '*' then
                                    matched, multiple = ns_a == "*" or ns == ns_a, true
                                else
                                    matched = false
                                end
                                if not action[2] then matched = not matched end
                                if matched then
                                    n = n + dn
                                    if n == action[5] then
                                        if index == #pattern then
                                            if handle(root,rootdt,root.ri or k) then return false end
                                        else
                                            if not traverse(e,pattern,handle,reverse,index+1,root) then return false end
                                        end
                                        break
                                    end
                                elseif wildcard then
                                    if not traverse(e,pattern,handle,reverse,index,root,true) then return false end
                                end
                            else
                                local matched, multiple = false, false
                                if command == 20 then -- match
                                    local ns_a, tg_a = action[3], action[4]
                                    if tg == tg_a then
                                        matched = ns_a == "*" or ns == ns_a
                                    elseif tg_a == '*' then
                                        matched, multiple = ns_a == "*" or ns == ns_a, true
                                    else
                                        matched = false
                                    end
                                    if not action[2] then matched = not matched end
                                elseif command == 21 then -- match one of
                                    multiple = true
                                    for i=3,#action,2 do
                                        local ns_a, tg_a = action[i], action[i+1]
                                        if (ns_a == "*" or ns == ns_a) and (tg == "*" or tg == tg_a) then
                                            matched = true
                                            break
                                        end
                                    end
                                    if not action[2] then matched = not matched end
                                elseif command == 22 then -- eq
                                    local ns_a, tg_a = action[3], action[4]
                                    if tg == tg_a then
                                        matched = ns_a == "*" or ns == ns_a
                                    elseif tg_a == '*' then
                                        matched, multiple = ns_a == "*" or ns == ns_a, true
                                    else
                                        matched = false
                                    end
                                    matched = matched and e.at[action[6]] == action[7]
                                elseif command == 23 then -- ne
                                    local ns_a, tg_a = action[3], action[4]
                                    if tg == tg_a then
                                        matched = ns_a == "*" or ns == ns_a
                                    elseif tg_a == '*' then
                                        matched, multiple = ns_a == "*" or ns == ns_a, true
                                    else
                                        matched = false
                                    end
                                    if not action[2] then matched = not matched end
                                    matched = mached and e.at[action[6]] ~= action[7]
                                elseif command == 24 then -- one of eq
                                    multiple = true
                                    for i=3,#action-2,2 do
                                        local ns_a, tg_a = action[i], action[i+1]
                                        if (ns_a == "*" or ns == ns_a) and (tg == "*" or tg == tg_a) then
                                            matched = true
                                            break
                                        end
                                    end
                                    if not action[2] then matched = not matched end
                                    matched = matched and e.at[action[#action-1]] == action[#action]
                                elseif command == 25 then -- one of ne
                                    multiple = true
                                    for i=3,#action-2,2 do
                                        local ns_a, tg_a = action[i], action[i+1]
                                        if (ns_a == "*" or ns == ns_a) and (tg == "*" or tg == tg_a) then
                                            matched = true
                                            break
                                        end
                                    end
                                    if not action[2] then matched = not matched end
                                    matched = matched and e.at[action[#action-1]] ~= action[#action]
                                elseif command == 27 then -- has attribute
                                    local ns_a, tg_a = action[3], action[4]
                                    if tg == tg_a then
                                        matched = ns_a == "*" or ns == ns_a
                                    elseif tg_a == '*' then
                                        matched, multiple = ns_a == "*" or ns == ns_a, true
                                    else
                                        matched = false
                                    end
                                    if not action[2] then matched = not matched end
                                    matched = matched and e.at[action[5]]
                                elseif command == 28 then -- has value
                                    local edt, ns_a, tg_a = e.dt, action[3], action[4]
                                    if tg == tg_a then
                                        matched = ns_a == "*" or ns == ns_a
                                    elseif tg_a == '*' then
                                        matched, multiple = ns_a == "*" or ns == ns_a, true
                                    else
                                        matched = false
                                    end
                                    if not action[2] then matched = not matched end
                                    matched = matched and edt and edt[1] == action[5]
                                elseif command == 31 then
                                    local edt, ns_a, tg_a = e.dt, action[3], action[4]
                                    if tg == tg_a then
                                        matched = ns_a == "*" or ns == ns_a
                                    elseif tg_a == '*' then
                                        matched, multiple = ns_a == "*" or ns == ns_a, true
                                    else
                                        matched = false
                                    end
                                    if not action[2] then matched = not matched end
                                    if matched then
                                        matched = action[6](functions,idx,e.at or { },edt[1],rootdt,k)
                                    end
                                end
                                if matched then -- combine tg test and at test
                                    if index == #pattern then
                                        if handle(root,rootdt,root.ri or k) then return false end
                                        if wildcard then
                                            if multiple then
                                                if not traverse(e,pattern,handle,reverse,index,root,true) then return false end
                                            else
                                             -- maybe or multiple; anyhow, check on (section|title) vs just section and title in example in lxml
                                                if not traverse(e,pattern,handle,reverse,index,root) then return false end
                                            end
                                        end
                                    else
                                        if not traverse(e,pattern,handle,reverse,index+1,root) then return false end
                                    end
                                elseif command == 14 then -- any
                                    if index == #pattern then
                                        if handle(root,rootdt,root.ri or k) then return false end
                                    else
                                        if not traverse(e,pattern,handle,reverse,index+1,root) then return false end
                                    end
                                elseif command == 15 then -- many
                                    if index == #pattern then
                                        if handle(root,rootdt,root.ri or k) then return false end
                                    else
                                        if not traverse(e,pattern,handle,reverse,index+1,root,true) then return false end
                                    end
                                -- not here : 11
                                elseif command == 11 then -- parent
                                    local ep = e.__p__ or parent
                                    if index < #pattern then
                                        if not traverse(ep,pattern,handle,reverse,root,index+1) then return false end
                                    elseif handle(root,rootdt,k) then
                                        return false
                                    end
                                elseif command == 40 and e.special and tg == "@pi@" then -- pi
                                    local pi = action[2]
                                    if pi ~= "" then
                                        local pt = e.dt[1]
                                        if pt and pt:find(pi) then
                                            if handle(root,rootdt,k) then
                                                return false
                                            end
                                        end
                                    elseif handle(root,rootdt,k) then
                                        return false
                                    end
                                elseif wildcard then
                                    if not traverse(e,pattern,handle,reverse,index,root,true) then return false end
                                end
                            end
                        else
                            -- not here : 11
                            if command == 11 then -- parent
                                local ep = e.__p__ or parent
                                if index < #pattern then
                                    if not traverse(ep,pattern,handle,reverse,index+1,root) then return false end
                                elseif handle(root,rootdt,k) then
                                    return false
                                end
                                break -- else loop
                            end
                        end
                    end
                end
            end
        end
        return true
    end

    xml.traverse = traverse

end

--[[ldx--
<p>Next come all kind of locators and manipulators. The most generic function here
is <t>xml.filter(root,pattern)</t>. All registers functions in the filters namespace
can be path of a search path, as in:</p>

<typing>
local r, d, k = xml.filter(root,"/a/b/c/position(4)"
</typing>
--ldx]]--

do

    local traverse, lpath, convert = xml.traverse, xml.lpath, xml.convert

    xml.filters = { }

    function xml.filters.default(root,pattern)
        local rt, dt, dk
        traverse(root, lpath(pattern), function(r,d,k) rt,dt,dk = r,d,k return true end)
        return dt and dt[dk], rt, dt, dk
    end
    function xml.filters.attributes(root,pattern,arguments)
        local rt, dt, dk
        traverse(root, lpath(pattern), function(r,d,k) rt, dt, dk = r, d, k return true end)
        local ekat = (dt and dt[dk] and dt[dk].at) or (rt and rt.at)
        if ekat then
            if arguments then
                return ekat[arguments] or "", rt, dt, dk
            else
                return ekat, rt, dt, dk
            end
        else
            return { }, rt, dt, dk
        end
    end
    function xml.filters.reverse(root,pattern)
        local rt, dt, dk
        traverse(root, lpath(pattern), function(r,d,k) rt,dt,dk = r,d,k return true end, 'reverse')
        return dt and dt[dk], rt, dt, dk
    end
    function xml.filters.count(root,pattern,everything)
        local n = 0
        traverse(root, lpath(pattern), function(r,d,t)
            if everything or type(d[t]) == "table" then
                n = n + 1
            end
        end)
        return n
    end
    function xml.filters.elements(root, pattern) -- == all
        local t = { }
        traverse(root, lpath(pattern), function(r,d,k)
            local e = d[k]
            if e then
                t[#t+1] = e
            end
        end)
        return t
    end
    function xml.filters.texts(root, pattern)
        local t = { }
        traverse(root, lpath(pattern), function(r,d,k)
            local e = d[k]
            if e and e.dt then
                t[#t+1] = e.dt
            end
        end)
        return t
    end
    function xml.filters.first(root,pattern)
        local rt, dt, dk
        traverse(root, lpath(pattern), function(r,d,k) rt,dt,dk = r,d,k return true end)
        return dt and dt[dk], rt, dt, dk
    end
    function xml.filters.last(root,pattern)
        local rt, dt, dk
        traverse(root, lpath(pattern), function(r,d,k) rt,dt,dk = r,d,k return true end, 'reverse')
        return dt and dt[dk], rt, dt, dk
    end
    function xml.filters.index(root,pattern,arguments)
        local rt, dt, dk, reverse, i = nil, nil, nil, false, tonumber(arguments or '1') or 1
        if i and i ~= 0 then
            if i < 0 then
                reverse, i = true, -i
            end
            traverse(root, lpath(pattern), function(r,d,k) rt, dt, dk, i = r, d, k, i-1 return i == 0 end, reverse)
            if i == 0 then
                return dt and dt[dk], rt, dt, dk
            end
        end
        return nil, nil, nil, nil
    end
    function xml.filters.attribute(root,pattern,arguments)
        local rt, dt, dk
        traverse(root, lpath(pattern), function(r,d,k) rt, dt, dk = r, d, k return true end)
        local ekat = (dt and dt[dk] and dt[dk].at) or (rt and rt.at)
        return (ekat and (ekat[arguments] or ekat[arguments:gsub("^([\"\'])(.*)%1$","%2")])) or ""
    end
    function xml.filters.text(root,pattern,arguments) -- ?? why index, tostring slow
        local dtk, rt, dt, dk = xml.filters.index(root,pattern,arguments)
        if dtk then -- n
            local dtkdt = dtk.dt
            if not dtkdt then
                return "", rt, dt, dk
            elseif #dtkdt == 1 and type(dtkdt[1]) == "string" then
                return dtkdt[1], rt, dt, dk
            else
                return xml.tostring(dtkdt), rt, dt, dk
            end
        else
            return "", rt, dt, dk
        end
    end
    function xml.filters.tag(root,pattern,n)
        local tag = ""
        traverse(root, lpath(pattern), function(r,d,k)
            tag = xml.functions.tag(d,k,n and tonumber(n))
            return true
        end)
        return tag
    end
    function xml.filters.name(root,pattern,n)
        local tag = ""
        traverse(root, lpath(pattern), function(r,d,k)
            tag = xml.functions.name(d,k,n and tonumber(n))
            return true
        end)
        return tag
    end

    --[[ldx--
    <p>For splitting the filter function from the path specification, we can
    use string matching or lpeg matching. Here the difference in speed is
    neglectable but the lpeg variant is more robust.</p>
    --ldx]]--

    --  not faster but hipper ... although ... i can't get rid of the trailing / in the path

    local P, S, R, C, V, Cc = lpeg.P, lpeg.S, lpeg.R, lpeg.C, lpeg.V, lpeg.Cc

    local slash     = P('/')
    local name      = (R("az","AZ","--","__"))^1
    local path      = C(((1-slash)^0 * slash)^1)
    local argument  = P { "(" * C(((1 - S("()")) + V(1))^0) * ")" }
    local action    = Cc(1) * path * C(name) * argument
    local attribute = Cc(2) * path * P('@') * C(name)
    local direct    = Cc(3) * Cc("../*") * slash^0 * C(name) * argument

    local parser    = direct + action + attribute

    local filters          = xml.filters
    local attribute_filter = xml.filters.attributes
    local default_filter   = xml.filters.default

    -- todo: also hash, could be gc'd

    function xml.filter(root,pattern)
        local kind, a, b, c = parser:match(pattern)
--~ if xml.trace_lpath then
--~     print(pattern,kind,a,b,c)
--~ end
        if kind == 1 or kind == 3 then
            return (filters[b] or default_filter)(root,a,c)
        elseif kind == 2 then
            return attribute_filter(root,a,b)
        else
            return default_filter(root,pattern)
        end
    end

    --~     slightly faster, but first we need a proper test file
    --~
    --~     local hash = { }
    --~
    --~     function xml.filter(root,pattern)
    --~         local h = hash[pattern]
    --~         if not h then
    --~             local kind, a, b, c = parser:match(pattern)
    --~             if kind == 1 then
    --~                 h = { kind, filters[b] or default_filter, a, b, c }
    --~             elseif kind == 2 then
    --~                 h = { kind, attribute_filter, a, b, c }
    --~             else
    --~                 h = { kind, default_filter, a, b, c }
    --~             end
    --~             hash[pattern] = h
    --~         end
    --~         local kind = h[1]
    --~         if kind == 1 then
    --~             return h[2](root,h[2],h[4])
    --~         elseif kind == 2 then
    --~             return h[2](root,h[2],h[3])
    --~         else
    --~             return h[2](root,pattern)
    --~         end
    --~     end

    --[[ldx--
    <p>The following functions collect elements and texts.</p>
    --ldx]]--

    -- still somewhat bugged

    function xml.collect_elements(root, pattern, ignorespaces)
        local rr, dd = { }, { }
        traverse(root, lpath(pattern), function(r,d,k)
            local dk = d and d[k]
            if dk then
                if ignorespaces and type(dk) == "string" and dk:find("[^%S]") then
                    -- ignore
                else
                    local n = #rr+1
                    rr[n], dd[n] = r, dk
                end
            end
        end)
        return dd, rr
    end

    function xml.collect_texts(root, pattern, flatten)
        local t = { } -- no r collector
        traverse(root, lpath(pattern), function(r,d,k)
            if d then
                local ek = d[k]
                local tx = ek and ek.dt
                if flatten then
                    if tx then
                        t[#t+1] = xml.tostring(tx) or ""
                    else
                        t[#t+1] = ""
                    end
                else
                    t[#t+1] = tx or ""
                end
            else
                t[#t+1] = ""
            end
        end)
        return t
    end

    function xml.collect_tags(root, pattern, nonamespace)
        local t = { }
        xml.traverse(root, xml.lpath(pattern), function(r,d,k)
            local dk = d and d[k]
            if dk and type(dk) == "table" then
                local ns, tg = e.ns, e.tg
                if nonamespace then
                    t[#t+1] = tg -- if needed we can return an extra table
                elseif ns == "" then
                    t[#t+1] = tg
                else
                    t[#t+1] = ns .. ":" .. tg
                end
            end
        end)
        return #t > 0 and {}
    end

    --[[ldx--
    <p>Often using an iterators looks nicer in the code than passing handler
    functions. The <l n='lua'/> book describes how to use coroutines for that
    purpose (<url href='http://www.lua.org/pil/9.3.html'/>). This permits
    code like:</p>

    <typing>
    for r, d, k in xml.elements(xml.load('text.xml'),"title") do
        print(d[k])
    end
    </typing>

    <p>Which will print all the titles in the document. The iterator variant takes
    1.5 times the runtime of the function variant which is due to the overhead in
    creating the wrapper. So, instead of:</p>

    <typing>
    function xml.filters.first(root,pattern)
        for rt,dt,dk in xml.elements(root,pattern)
            return dt and dt[dk], rt, dt, dk
        end
        return nil, nil, nil, nil
    end
    </typing>

    <p>We use the function variants in the filters.</p>
    --ldx]]--

    local wrap, yield = coroutine.wrap, coroutine.yield

    function xml.elements(root,pattern,reverse)
        return wrap(function() traverse(root, lpath(pattern), yield, reverse) end)
    end

    function xml.elements_only(root,pattern,reverse)
        return wrap(function() traverse(root, lpath(pattern), function(r,d,k) yield(d[k]) end, reverse) end)
    end

    function xml.each_element(root, pattern, handle, reverse)
        local ok
        traverse(root, lpath(pattern), function(r,d,k) ok = true handle(r,d,k) end, reverse)
        return ok
    end

    function xml.process_elements(root, pattern, handle)
        traverse(root, lpath(pattern), function(r,d,k)
            local dkdt = d[k].dt
            if dkdt then
                for i=1,#dkdt do
                    local v = dkdt[i]
                    if v.tg then handle(v) end
                end
            end
        end)
    end

    function xml.process_attributes(root, pattern, handle)
        traverse(root, lpath(pattern), function(r,d,k)
            local ek = d[k]
            local a = ek.at or { }
            handle(a)
            if next(a) then -- next is faster than type (and >0 test)
                ek.at = a
            else
                ek.at = nil
            end
        end)
    end

    --[[ldx--
    <p>We've now arrives at the functions that manipulate the tree.</p>
    --ldx]]--

    function xml.inject_element(root, pattern, element, prepend)
        if root and element then
            local matches, collect = { }, nil
            if type(element) == "string" then
                element = convert(element,true)
            end
            if element then
                collect = function(r,d,k) matches[#matches+1] = { r, d, k, element } end
                traverse(root, lpath(pattern), collect)
                for i=1,#matches do
                    local m = matches[i]
                    local r, d, k, element, edt = m[1], m[2], m[3], m[4], nil
                    if element.ri then
                        element = element.dt[element.ri].dt
                    else
                        element = element.dt
                    end
                    if r.ri then
                        edt = r.dt[r.ri].dt
                    else
                        edt = d and d[k] and d[k].dt
                    end
                    if edt then
                        local be, af
                        if prepend then
                            be, af = xml.copy(element), edt
                        else
                            be, af = edt, xml.copy(element)
                        end
                        for i=1,#af do
                            be[#be+1] = af[i]
                        end
                        if r.ri then
                            r.dt[r.ri].dt = be
                        else
                            d[k].dt = be
                        end
                    else
                     -- r.dt = element.dt -- todo
                    end
                end
            end
        end
    end

    -- todo: copy !

    function xml.insert_element(root, pattern, element, before) -- todo: element als functie
        if root and element then
            if pattern == "/" then
                xml.inject_element(root, pattern, element, before)
            else
                local matches, collect = { }, nil
                if type(element) == "string" then
                    element = convert(element,true)
                end
                if element and element.ri then
                    element = element.dt[element.ri]
                end
                if element then
                    collect = function(r,d,k) matches[#matches+1] = { r, d, k, element } end
                    traverse(root, lpath(pattern), collect)
                    for i=#matches,1,-1 do
                        local m = matches[i]
                        local r, d, k, element = m[1], m[2], m[3], m[4]
                        if not before then k = k + 1 end
                        if element.tg then
                            table.insert(d,k,element) -- untested
                        elseif element.dt then
                            for _,v in ipairs(element.dt) do -- i added
                                table.insert(d,k,v)
                                k = k + 1
                            end
                        end
                    end
                end
            end
        end
    end

    xml.insert_element_after  =                 xml.insert_element
    xml.insert_element_before = function(r,p,e) xml.insert_element(r,p,e,true) end
    xml.inject_element_after  =                 xml.inject_element
    xml.inject_element_before = function(r,p,e) xml.inject_element(r,p,e,true) end

    function xml.delete_element(root, pattern)
        local matches, deleted = { }, { }
        local collect = function(r,d,k) matches[#matches+1] = { r, d, k } end
        traverse(root, lpath(pattern), collect)
        for i=#matches,1,-1 do
            local m = matches[i]
            deleted[#deleted+1] = table.remove(m[2],m[3])
        end
        return deleted
    end

    function xml.replace_element(root, pattern, element)
        if type(element) == "string" then
            element = convert(element,true)
        end
        if element and element.ri then
            element = element.dt[element.ri]
        end
        if element then
            traverse(root, lpath(pattern), function(rm, d, k)
                d[k] = element.dt -- maybe not clever enough
            end)
        end
    end

    local function load_data(name) -- == io.loaddata
        local f, data = io.open(name), ""
        if f then
            data = f:read("*all",'b') -- 'b' ?
            f:close()
        end
        return data
    end

    function xml.include(xmldata,pattern,attribute,recursive,loaddata)
        -- parse="text" (default: xml), encoding="" (todo)
        -- attribute = attribute or 'href'
        pattern = pattern or 'include'
        loaddata = loaddata or load_data
        local function include(r,d,k)
            local ek, name = d[k], nil
            if not attribute or attribute == "" then
                local ekdt = ek.dt
                name = (type(ekdt) == "table" and ekdt[1]) or ekdt
            end
            if not name then
                if ek.at then
                    for a in (attribute or "href"):gmatch("([^|]+)") do
                        name = ek.at[a]
                        if name then break end
                    end
                end
            end
            local data = (name and name ~= "" and loaddata(name)) or ""
            if data == "" then
                xml.empty(d,k)
            elseif ek.at["parse"] == "text" then -- for the moment hard coded
                d[k] = xml.escaped(data)
            else
                local xi = xml.convert(data)
                if not xi then
                    xml.empty(d,k)
                else
                    if recursive then
                        xml.include(xi,pattern,attribute,recursive,loaddata)
                    end
                    xml.assign(d,k,xi)
                end
            end
        end
        xml.each_element(xmldata, pattern, include)
    end

    function xml.strip_whitespace(root, pattern)
        traverse(root, lpath(pattern), function(r,d,k)
            local dkdt = d[k].dt
            if dkdt then -- can be optimized
                local t = { }
                for i=1,#dkdt do
                    local str = dkdt[i]
                    if type(str) == "string" and str:find("^[ \n\r\t]*$") then
                        -- stripped
                    else
                        t[#t+1] = str
                    end
                end
                d[k].dt = t
            end
        end)
    end

    function xml.rename_space(root, oldspace, newspace) -- fast variant
        local ndt = #root.dt
        local rename = xml.rename_space
        for i=1,ndt or 0 do
            local e = root[i]
            if type(e) == "table" then
                if e.ns == oldspace then
                    e.ns = newspace
                    if e.rn then
                        e.rn = newspace
                    end
                end
                local edt = e.dt
                if edt then
                    rename(edt, oldspace, newspace)
                end
            end
        end
    end

    function xml.remap_tag(root, pattern, newtg)
        traverse(root, lpath(pattern), function(r,d,k)
            d[k].tg = newtg
        end)
    end
    function xml.remap_namespace(root, pattern, newns)
        traverse(root, lpath(pattern), function(r,d,k)
            d[k].ns = newns
        end)
    end
    function xml.check_namespace(root, pattern, newns)
        traverse(root, lpath(pattern), function(r,d,k)
            local dk = d[k]
            if (not dk.rn or dk.rn == "") and dk.ns == "" then
                dk.rn = newns
            end
        end)
    end
    function xml.remap_name(root, pattern, newtg, newns, newrn)
        traverse(root, lpath(pattern), function(r,d,k)
            local dk = d[k]
            dk.tg = newtg
            dk.ns = newns
            dk.rn = newrn
        end)
    end

    function xml.filters.found(root,pattern,check_content)
        local found = false
        traverse(root, lpath(pattern), function(r,d,k)
            if check_content then
                local dk = d and d[k]
                found = dk and dk.dt and next(dk.dt) and true
            else
                found = true
            end
            return true
        end)
        return found
    end

end

--[[ldx--
<p>Here are a few synonyms.</p>
--ldx]]--

xml.filters.position = xml.filters.index

xml.count    = xml.filters.count
xml.index    = xml.filters.index
xml.position = xml.filters.index
xml.first    = xml.filters.first
xml.last     = xml.filters.last
xml.found    = xml.filters.found

xml.each     = xml.each_element
xml.process  = xml.process_element
xml.strip    = xml.strip_whitespace
xml.collect  = xml.collect_elements
xml.all      = xml.collect_elements

xml.insert   = xml.insert_element_after
xml.inject   = xml.inject_element_after
xml.after    = xml.insert_element_after
xml.before   = xml.insert_element_before
xml.delete   = xml.delete_element
xml.replace  = xml.replace_element

--[[ldx--
<p>The following helper functions best belong to the <t>lmxl-ini</t>
module. Some are here because we need then in the <t>mk</t>
document and other manuals, others came up when playing with
this module. Since this module is also used in <l n='mtxrun'/> we've
put them here instead of loading mode modules there then needed.</p>
--ldx]]--

function xml.gsub(t,old,new)
    if t.dt then
        for k,v in ipairs(t.dt) do
            if type(v) == "string" then
                t.dt[k] = v:gsub(old,new)
            else
                xml.gsub(v,old,new)
            end
        end
    end
end

function xml.strip_leading_spaces(dk,d,k) -- cosmetic, for manual
    if d and k and d[k-1] and type(d[k-1]) == "string" then
        local s = d[k-1]:match("\n(%s+)")
        xml.gsub(dk,"\n"..string.rep(" ",#s),"\n")
    end
end

function xml.serialize_path(root,lpath,handle)
    local dk, r, d, k = xml.first(root,lpath)
    dk = xml.copy(dk)
    xml.strip_leading_spaces(dk,d,k)
    xml.serialize(dk,handle)
end

--~ xml.escapes   = { ['&'] = '&amp;', ['<'] = '&lt;', ['>'] = '&gt;', ['"'] = '&quot;' }
--~ xml.unescapes = { } for k,v in pairs(xml.escapes) do xml.unescapes[v] = k end

--~ function xml.escaped  (str) return str:gsub("(.)"   , xml.escapes  ) end
--~ function xml.unescaped(str) return str:gsub("(&.-;)", xml.unescapes) end
--~ function xml.cleansed (str) return str:gsub("<.->"  , ''           ) end -- "%b<>"

do

    local P, S, R, C, V, Cc, Cs = lpeg.P, lpeg.S, lpeg.R, lpeg.C, lpeg.V, lpeg.Cc, lpeg.Cs

    -- 100 * 2500 * "oeps< oeps> oeps&" : gsub:lpeg|lpeg|lpeg
    --
    -- 1021:0335:0287:0247

    -- 10 * 1000 * "oeps< oeps> oeps& asfjhalskfjh alskfjh alskfjh alskfjh ;al J;LSFDJ"
    --
    -- 1559:0257:0288:0190 (last one suggested by roberto)

    --    escaped = Cs((S("<&>") / xml.escapes + 1)^0)
    --    escaped = Cs((S("<")/"&lt;" + S(">")/"&gt;" + S("&")/"&amp;" + 1)^0)
    local normal  = (1 - S("<&>"))^0
    local special = P("<")/"&lt;" + P(">")/"&gt;" + P("&")/"&amp;"
    local escaped = Cs(normal * (special * normal)^0)

    -- 100 * 1000 * "oeps&lt; oeps&gt; oeps&amp;" : gsub:lpeg == 0153:0280:0151:0080 (last one by roberto)

    --    unescaped = Cs((S("&lt;")/"<" + S("&gt;")/">" + S("&amp;")/"&" + 1)^0)
    --    unescaped = Cs((((P("&")/"") * (P("lt")/"<" + P("gt")/">" + P("amp")/"&") * (P(";")/"")) + 1)^0)
    local normal    = (1 - S"&")^0
    local special   = P("&lt;")/"<" + P("&gt;")/">" + P("&amp;")/"&"
    local unescaped = Cs(normal * (special * normal)^0)

    -- 100 * 5000 * "oeps <oeps bla='oeps' foo='bar'> oeps </oeps> oeps " : gsub:lpeg == 623:501 msec (short tags, less difference)

    local cleansed = Cs(((P("<") * (1-P(">"))^0 * P(">"))/"" + 1)^0)

    function xml.escaped  (str) return escaped  :match(str) end
    function xml.unescaped(str) return unescaped:match(str) end
    function xml.cleansed (str) return cleansed :match(str) end

end

function xml.join(t,separator,lastseparator)
    if #t > 0 then
        local result = { }
        for k,v in pairs(t) do
            result[k] = xml.tostring(v)
        end
        if lastseparator then
            return concat(result,separator or "",1,#result-1) .. (lastseparator or "") .. result[#result]
        else
            return concat(result,separator)
        end
    else
        return ""
    end
end


--[[ldx--
<p>We provide (at least here) two entity handlers. The more extensive
resolver consults a hash first, tries to convert to <l n='utf'/> next,
and finaly calls a handler when defines. When this all fails, the
original entity is returned.</p>
--ldx]]--

do if unicode and unicode.utf8 then

    xml.entities = xml.entities or { } -- xml.entities.handler == function

    function xml.entities.handler(e)
        return format("[%s]",e)
    end

    local char = unicode.utf8.char

    local function toutf(s)
        return char(tonumber(s,16))
    end

    local entities = xml.entities -- global entities

    function utfize(root)
        local d = root.dt
        for k=1,#d do
            local dk = d[k]
            if type(dk) == "string" then
            --  test prevents copying if no match
                if dk:find("&#x.-;") then
                    d[k] = dk:gsub("&#x(.-);",toutf)
                end
            else
                utfize(dk)
            end
        end
    end

    xml.utfize = utfize

    local function resolve(e) -- hex encoded always first, just to avoid mkii fallbacks
        if e:find("#x") then
            return char(tonumber(e:sub(3),16))
        else
            local ee = entities[e]
            if ee then
                return ee
            else
                local h = xml.entities.handler
                return (h and h(e)) or "&" .. e .. ";"
            end
        end
    end

    local function resolve_entities(root)
        if not root.special or root.tg == "@rt@" then
            local d = root.dt
            for k=1,#d do
                local dk = d[k]
                if type(dk) == "string" then
                    if dk:find("&.-;") then
                        d[k] = dk:gsub("&(.-);",resolve)
                    end
                else
                    resolve_entities(dk)
                end
            end
        end
    end

    xml.resolve_entities = resolve_entities

    function xml.utfize_text(str)
        if str:find("&#") then
            return (str:gsub("&#x(.-);",toutf))
        else
            return str
        end
    end

    function xml.resolve_text_entities(str) -- maybe an lpeg. maybe resolve inline
        if str:find("&") then
            return (str:gsub("&(.-);",resolve))
        else
            return str
        end
    end

    function xml.show_text_entities(str)
        if str:find("&") then
            return (str:gsub("&(.-);","[%1]"))
        else
            return str
        end
    end

    -- experimental, this will be done differently

    function xml.merge_entities(root)
        local documententities = root.entities
        local allentities = xml.entities
        if documententities then
            for k, v in pairs(documententities) do
                allentities[k] = v
            end
        end
    end

end end

--  xml.set_text_cleanup(xml.show_text_entities)
--  xml.set_text_cleanup(xml.resolve_text_entities)

--~ xml.lshow("/../../../a/(b|c)[@d='e']/f")
--~ xml.lshow("/../../../a/!(b|c)[@d='e']/f")
--~ xml.lshow("/../../../a/!b[@d!='e']/f")

--~ x = xml.convert([[
--~     <a>
--~         <b n='01'>01</b>
--~         <b n='02'>02</b>
--~         <b n='03'>03</b>
--~         <b n='04'>OK</b>
--~         <b n='05'>05</b>
--~         <b n='06'>06</b>
--~         <b n='07'>ALSO OK</b>
--~     </a>
--~ ]])

--~ xml.trace_lpath = true

--~ xml.xshow(xml.first(x,"b[position() > 2 and position() < 5 and text() == 'ok']"))
--~ xml.xshow(xml.first(x,"b[position() > 2 and position() < 5 and text() == upper('ok')]"))
--~ xml.xshow(xml.first(x,"b[@n=='03' or @n=='08']"))
--~ xml.xshow(xml.all  (x,"b[number(@n)>2 and number(@n)<6]"))
--~ xml.xshow(xml.first(x,"b[find(text(),'ALSO')]"))

--~ str = [[
--~ <?xml version="1.0" encoding="utf-8"?>
--~ <story line='mojca'>
--~     <windows>my secret</mouse>
--~ </story>
--~ ]]

--~ x = xml.convert([[
--~     <a><b n='01'>01</b><b n='02'>02</b><x>xx</x><b n='03'>03</b><b n='04'>OK</b></a>
--~ ]])
--~ xml.xshow(xml.first(x,"b[tag(2) == 'x']"))
--~ xml.xshow(xml.first(x,"b[tag(1) == 'x']"))
--~ xml.xshow(xml.first(x,"b[tag(-1) == 'x']"))
--~ xml.xshow(xml.first(x,"b[tag(-2) == 'x']"))

--~ print(xml.filter(x,"b/tag(2)"))
--~ print(xml.filter(x,"b/tag(1)"))


-- filename : l-utils.lua
-- comment  : split off from luat-lib
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-utils'] = 1.001

if not utils        then utils        = { } end
if not utils.merger then utils.merger = { } end
if not utils.lua    then utils.lua    = { } end

utils.merger.m_begin = "begin library merge"
utils.merger.m_end   = "end library merge"
utils.merger.pattern =
    "%c+" ..
    "%-%-%s+" .. utils.merger.m_begin ..
    "%c+(.-)%c+" ..
    "%-%-%s+" .. utils.merger.m_end ..
    "%c+"

function utils.merger._self_fake_()
    return
        "-- " .. "created merged file" .. "\n\n" ..
        "-- " .. utils.merger.m_begin  .. "\n\n" ..
        "-- " .. utils.merger.m_end    .. "\n\n"
end

function utils.report(...)
    print(...)
end

function utils.merger._self_load_(name)
    local f, data = io.open(name), ""
    if f then
        data = f:read("*all")
        f:close()
    end
    return data or ""
end

function utils.merger._self_save_(name, data)
    if data ~= "" then
        local f = io.open(name,'w')
        if f then
            f:write(data)
            f:close()
        end
    end
end

function utils.merger._self_swap_(data,code)
    if data ~= "" then
        return (data:gsub(utils.merger.pattern, function(s)
            return "\n\n" .. "-- "..utils.merger.m_begin .. "\n" .. code .. "\n" .. "-- "..utils.merger.m_end .. "\n\n"
        end, 1))
    else
        return ""
    end
end

function utils.merger._self_libs_(libs,list)
    local result, f = { }, nil
    if type(libs) == 'string' then libs = { libs } end
    if type(list) == 'string' then list = { list } end
    for _, lib in ipairs(libs) do
        for _, pth in ipairs(list) do
            local name = string.gsub(pth .. "/" .. lib,"\\","/")
            f = io.open(name)
            if f then
            --  utils.report("merging library",name)
                result[#result+1] = f:read("*all")
                f:close()
                list = { pth } -- speed up the search
                break
            else
            --  utils.report("no library",name)
            end
        end
    end
    return table.concat(result, "\n\n")
end

function utils.merger.selfcreate(libs,list,target)
    if target then
        utils.merger._self_save_(
            target,
            utils.merger._self_swap_(
                utils.merger._self_fake_(),
                utils.merger._self_libs_(libs,list)
            )
        )
    end
end

function utils.merger.selfmerge(name,libs,list,target)
    utils.merger._self_save_(
        target or name,
        utils.merger._self_swap_(
            utils.merger._self_load_(name),
            utils.merger._self_libs_(libs,list)
        )
    )
end

function utils.merger.selfclean(name)
    utils.merger._self_save_(
        name,
        utils.merger._self_swap_(
            utils.merger._self_load_(name),
            ""
        )
    )
end

utils.lua.compile_strip = true

function utils.lua.compile(luafile, lucfile)
 -- utils.report("compiling",luafile,"into",lucfile)
    os.remove(lucfile)
    local command = "-o " .. string.quote(lucfile) .. " " .. string.quote(luafile)
    if utils.lua.compile_strip then
        command = "-s " .. command
    end
    if os.spawn("texluac " .. command) == 0 then
        return true
    elseif os.spawn("luac " .. command) == 0 then
        return true
    else
        return false
    end
end



-- filename : luat-lib.lua
-- comment  : companion to luat-lib.tex
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['luat-lib'] = 1.001

-- mostcode moved to the l-*.lua and other luat-*.lua files

-- os / io

os.setlocale(nil,nil) -- useless feature and even dangerous in luatex

-- os.platform

-- mswin|bccwin|mingw|cygwin  windows
-- darwin|rhapsody|nextstep   macosx
-- netbsd|unix                unix
-- linux                      linux

if not io.fileseparator then
    if string.find(os.getenv("PATH"),";") then
        io.fileseparator, io.pathseparator, os.platform = "\\", ";", os.type or "windows"
    else
        io.fileseparator, io.pathseparator, os.platform = "/" , ":", os.type or "unix"
    end
end

os.platform = os.platform or os.type or (io.pathseparator == ";" and "windows") or "unix"

-- arg normalization
--
-- for k,v in pairs(arg) do print(k,v) end

-- environment

if not environment then environment = { } end

environment.ownbin = environment.ownbin or arg[-2] or arg[-1] or arg[0] or "luatex"

local ownpath = nil -- we could use a metatable here

function environment.ownpath()
    if not ownpath then
        for p in string.gmatch(os.getenv("PATH"),"[^"..io.pathseparator.."]+") do
            local b = file.join(p,environment.ownbin)
            if lfs.isfile(b..".exe") or lfs.isfile(b) then
                ownpath = p
                break
            end
        end
        if not ownpath then ownpath = '.' end
    end
    return ownpath
end

if arg and (arg[0] == 'luatex' or arg[0] == 'luatex.exe') and arg[1] == "--luaonly" then
    arg[-1]=arg[0] arg[0]=arg[2] for k=3,#arg do arg[k-2]=arg[k] end arg[#arg]=nil arg[#arg]=nil
end

environment.arguments            = { }
environment.files                = { }
environment.sorted_argument_keys = nil

environment.platform = os.platform

function environment.initialize_arguments(arg)
    environment.arguments = { }
    environment.files     = { }
    environment.sorted_argument_keys = nil
    for index, argument in pairs(arg) do
        if index > 0 then
            local flag, value = argument:match("^%-+(.+)=(.-)$")
            if flag then
                environment.arguments[flag] = string.unquote(value or "")
            else
                flag = argument:match("^%-+(.+)")
                if flag then
                    environment.arguments[flag] = true
                else
                    environment.files[#environment.files+1] = argument
                end
            end
        end
    end
    environment.ownname = environment.ownname or arg[0] or 'unknown.lua'
end

function environment.showarguments()
    for k,v in pairs(environment.arguments) do
        print(k .. " : " .. tostring(v))
    end
    if #environment.files > 0 then
        print("files : " .. table.concat(environment.files, " "))
    end
end

function environment.setargument(name,value)
    environment.arguments[name] = value
end

function environment.argument(name)
    if environment.arguments[name] then
        return environment.arguments[name]
    else
        if not environment.sorted_argument_keys then
            environment.sorted_argument_keys = { }
            for _,v in pairs(table.sortedkeys(environment.arguments)) do
                table.insert(environment.sorted_argument_keys, "^" .. v)
            end
        end
        for _,v in pairs(environment.sorted_argument_keys) do
            if name:find(v) then
                return environment.arguments[v:sub(2,#v)]
            end
        end
    end
    return nil
end

function environment.split_arguments(separator) -- rather special, cut-off before separator
    local done, before, after = false, { }, { }
    for _,v in ipairs(environment.original_arguments) do
        if not done and v == separator then
            done = true
        elseif done then
            after[#after+1] = v
        else
            before[#before+1] = v
        end
    end
    return before, after
end

function environment.reconstruct_commandline(arg)
    if not arg then arg = environment.original_arguments end
    local result = { }
    for _,a in ipairs(arg) do -- ipairs 1 .. #n
        local kk, vv = a:match("^(%-+.-)=(.+)$")
        if kk and vv then
            if vv:find(" ") then
                result[#result+1] = kk .. "=" .. string.quote(vv)
            else
                result[#result+1] = a
            end
        elseif a:find(" ") then
            result[#result+1] = string.quote(a)
        else
            result[#result+1] = a
        end
    end
    return table.join(result," ")
end

if arg then
    environment.initialize_arguments(arg)
    environment.original_arguments = arg
    arg = { } -- prevent duplicate handling
end


-- filename : luat-inp.lua
-- comment  : companion to luat-lib.tex
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

-- This lib is multi-purpose and can be loaded again later on so that
-- additional functionality becomes available. We will split this
-- module in components when we're done with prototyping.

-- TODO: os.getenv -> os.env[]
-- TODO: instances.[hashes,cnffiles,configurations,522] -> ipairs (alles check, sneller)
-- TODO: check escaping in find etc, too much, too slow

-- This is the first code I wrote for LuaTeX, so it needs some cleanup.

-- To be considered: hash key lowercase, first entry in table filename
-- (any case), rest paths (so no need for optimization). Or maybe a
-- separate table that matches lowercase names to mixed case when
-- present. In that case the lower() cases can go away. I will do that
-- only when we run into problems with names ... well ... Iwona-Regular.

-- Beware, loading and saving is overloaded in luat-tmp!

if not versions    then versions    = { } end versions['luat-inp'] = 1.001
if not environment then environment = { } end
if not file        then file        = { } end

if environment.aleph_mode == nil then environment.aleph_mode = true end -- temp hack

if not input            then input            = { } end
if not input.suffixes   then input.suffixes   = { } end
if not input.formats    then input.formats    = { } end
if not input.aux        then input.aux        = { } end

if not input.suffixmap  then input.suffixmap  = { } end

if not input.locators   then input.locators   = { } end  -- locate databases
if not input.hashers    then input.hashers    = { } end  -- load databases
if not input.generators then input.generators = { } end  -- generate databases
if not input.filters    then input.filters    = { } end  -- conversion filters

local format = string.format

input.locators.notfound   = { nil }
input.hashers.notfound    = { nil }
input.generators.notfound = { nil }

input.cacheversion = '1.0.1'
input.banner       = nil
input.verbose      = false
input.debug        = false
input.cnfname      = 'texmf.cnf'
input.luaname      = 'texmfcnf.lua'
input.lsrname      = 'ls-R'
input.luasuffix    = '.tma'
input.lucsuffix    = '.tmc'

-- we use a cleaned up list / format=any is a wildcard, as is *name

input.formats['afm'] = 'AFMFONTS'       input.suffixes['afm'] = { 'afm' }
input.formats['enc'] = 'ENCFONTS'       input.suffixes['enc'] = { 'enc' }
input.formats['fmt'] = 'TEXFORMATS'     input.suffixes['fmt'] = { 'fmt' }
input.formats['map'] = 'TEXFONTMAPS'    input.suffixes['map'] = { 'map' }
input.formats['mp']  = 'MPINPUTS'       input.suffixes['mp']  = { 'mp' }
input.formats['ocp'] = 'OCPINPUTS'      input.suffixes['ocp'] = { 'ocp' }
input.formats['ofm'] = 'OFMFONTS'       input.suffixes['ofm'] = { 'ofm', 'tfm' }
input.formats['otf'] = 'OPENTYPEFONTS'  input.suffixes['otf'] = { 'otf' } -- 'ttf'
input.formats['opl'] = 'OPLFONTS'       input.suffixes['opl'] = { 'opl' }
input.formats['otp'] = 'OTPINPUTS'      input.suffixes['otp'] = { 'otp' }
input.formats['ovf'] = 'OVFFONTS'       input.suffixes['ovf'] = { 'ovf', 'vf' }
input.formats['ovp'] = 'OVPFONTS'       input.suffixes['ovp'] = { 'ovp' }
input.formats['tex'] = 'TEXINPUTS'      input.suffixes['tex'] = { 'tex' }
input.formats['tfm'] = 'TFMFONTS'       input.suffixes['tfm'] = { 'tfm' }
input.formats['ttf'] = 'TTFONTS'        input.suffixes['ttf'] = { 'ttf', 'ttc' }
input.formats['pfb'] = 'T1FONTS'        input.suffixes['pfb'] = { 'pfb', 'pfa' }
input.formats['vf']  = 'VFFONTS'        input.suffixes['vf']  = { 'vf' }

input.formats['fea'] = 'FONTFEATURES'   input.suffixes['fea'] = { 'fea' }
input.formats['cid'] = 'FONTCIDMAPS'    input.suffixes['cid'] = { 'cid', 'cidmap' }

input.formats ['texmfscripts'] = 'TEXMFSCRIPTS' -- new
input.suffixes['texmfscripts'] = { 'rb', 'pl', 'py' } -- 'lua'

input.formats ['lua'] = 'LUAINPUTS' -- new
input.suffixes['lua'] = { 'lua', 'luc', 'tma', 'tmc' }

-- here we catch a few new thingies (todo: add these paths to context.tmf)
--
-- FONTFEATURES  = .;$TEXMF/fonts/fea//
-- FONTCIDMAPS   = .;$TEXMF/fonts/cid//

function input.checkconfigdata(instance) -- not yet ok, no time for debugging now
    local function fix(varname,default)
        local proname = varname .. "." .. instance.progname or "crap"
        local p = instance.environment[proname]
        local v = instance.environment[varname]
        if not ((p and p ~= "") or (v and v ~= "")) then
            instance.variables[varname] = default -- or environment?
        end
    end
    fix("LUAINPUTS"   , ".;$TEXINPUTS;$TEXMFSCRIPTS")
    fix("FONTFEATURES", ".;$TEXMF/fonts/fea//;$OPENTYPEFONTS;$TTFONTS;$T1FONTS;$AFMFONTS")
    fix("FONTCIDMAPS" , ".;$TEXMF/fonts/cid//;$OPENTYPEFONTS;$TTFONTS;$T1FONTS;$AFMFONTS")
end

-- backward compatible ones

input.alternatives = { }

input.alternatives['map files']            = 'map'
input.alternatives['enc files']            = 'enc'
input.alternatives['cid files']            = 'cid'
input.alternatives['fea files']            = 'fea'
input.alternatives['opentype fonts']       = 'otf'
input.alternatives['truetype fonts']       = 'ttf'
input.alternatives['truetype collections'] = 'ttc'
input.alternatives['type1 fonts']          = 'pfb'

-- obscure ones

input.formats ['misc fonts'] = ''
input.suffixes['misc fonts'] = { }

input.formats     ['sfd']                      = 'SFDFONTS'
input.suffixes    ['sfd']                      = { 'sfd' }
input.alternatives['subfont definition files'] = 'sfd'

function input.reset()

    local instance = { }

    instance.rootpath        = ''
    instance.treepath        = ''
    instance.progname        = environment.progname or 'context'
    instance.engine          = environment.engine   or 'luatex'
    instance.format          = ''
    instance.environment     = { }
    instance.variables       = { }
    instance.expansions      = { }
    instance.files           = { }
    instance.remap           = { }
    instance.configuration   = { }
    instance.setup           = { }
    instance.order           = { }
    instance.found           = { }
    instance.foundintrees    = { }
    instance.kpsevars        = { }
    instance.hashes          = { }
    instance.cnffiles        = { }
    instance.luafiles        = { }
    instance.lists           = { }
    instance.remember        = true
    instance.diskcache       = true
    instance.renewcache      = false
    instance.scandisk        = true
    instance.cachepath       = nil
    instance.loaderror       = false
    instance.smallcache      = false
    instance.savelists       = true
    instance.cleanuppaths    = true
    instance.allresults      = false
    instance.pattern         = nil    -- lists
    instance.kpseonly        = false  -- lists
    instance.cachefile       = 'tmftools'
    instance.loadtime        = 0
    instance.starttime       = 0
    instance.stoptime        = 0
    instance.validfile       = function(path,name) return true end
    instance.data            = { } -- only for loading
    instance.force_suffixes  = true
    instance.dummy_path_expr = "^!*unset/*$"
    instance.fakepaths       = { }
    instance.lsrmode         = false

    if os.env then
        -- store once, freeze and faster
        for k,v in pairs(os.env) do
            instance.environment[k] = input.bare_variable(v)
        end
    else
        -- we will access os.env frequently
        for k,v in pairs({'HOME','TEXMF','TEXMFCNF'}) do
            local e = os.getenv(v)
            if e then
            --  input.report("setting",v,"to",input.bare_variable(e))
                instance.environment[v] = input.bare_variable(e)
            end
        end
    end

    -- cross referencing

    for k, v in pairs(input.suffixes) do
        for _, vv in pairs(v) do
            if vv then
                input.suffixmap[vv] = k
            end
        end
    end

    return instance

end

function input.reset_hashes(instance)
    instance.lists = { }
    instance.found = { }
end

function input.bare_variable(str) -- assumes str is a string
 -- return string.gsub(string.gsub(string.gsub(str,"%s+$",""),'^"(.+)"$',"%1"),"^'(.+)'$","%1")
    return (str:gsub("\s*([\"\']?)(.+)%1\s*", "%2"))
end

if texio then
    input.log = texio.write_nl
else
    input.log = print
end

function input.simple_logger(kind, name)
    if name and name ~= "" then
        if input.banner then
            input.log(input.banner..kind..": "..name)
        else
            input.log("<<"..kind..": "..name..">>")
        end
    else
        if input.banner then
            input.log(input.banner..kind..": no name")
        else
            input.log("<<"..kind..": no name>>")
        end
    end
end

function input.dummy_logger()
end

function input.settrace(n)
    input.trace = tonumber(n or 0)
    if input.trace > 0 then
        input.logger = input.simple_logger
        input.verbose = true
    else
        input.logger = function() end
    end
end

function input.report(...) -- inefficient
    if input.verbose then
        if input.banner then
            input.log(input.banner .. table.concat({...},' '))
        elseif input.logmode() == 'xml' then
            input.log("<t>"..table.concat({...},' ').."</t>")
        else
            input.log("<<"..table.concat({...},' ')..">>")
        end
    end
end

function input.reportlines(str)
    if type(str) == "string" then
        str = str:split("\n")
    end
    for _,v in pairs(str) do input.report(v) end
end

input.settrace(tonumber(os.getenv("MTX.INPUT.TRACE") or os.getenv("MTX_INPUT_TRACE") or input.trace or 0))

-- These functions can be used to test the performance, especially
-- loading the database files.

do
    local clock = os.gettimeofday or os.clock

    function input.starttiming(instance)
        if instance then
            instance.starttime = clock()
            if not instance.loadtime then
                instance.loadtime = 0
            end
        end
    end

    function input.stoptiming(instance, report)
        if instance then
            local starttime = instance.starttime
            if starttime then
                local stoptime = clock()
                local loadtime = stoptime - starttime
                instance.stoptime = stoptime
                instance.loadtime = instance.loadtime + loadtime
                if report then
                    input.report('load time', format("%0.3f",loadtime))
                end
                return loadtime
            end
        end
        return 0
    end

end

function input.elapsedtime(instance)
    return format("%0.3f",(instance and instance.loadtime) or 0)
end

function input.report_loadtime(instance)
    if instance then
        input.report('total load time', input.elapsedtime(instance))
    end
end

input.loadtime = input.elapsedtime

function input.env(instance,key)
    return instance.environment[key] or input.osenv(instance,key)
end

function input.osenv(instance,key)
    local ie = instance.environment
    local value = ie[key]
    if value == nil then
     -- local e = os.getenv(key)
        local e = os.env[key]
        if e == nil then
         -- value = "" -- false
        else
            value = input.bare_variable(e)
        end
        ie[key] = value
    end
    return value or ""
end

-- we follow a rather traditional approach:
--
-- (1) texmf.cnf given in TEXMFCNF
-- (2) texmf.cnf searched in TEXMF/web2c
--
-- for the moment we don't expect a configuration file in a zip

function input.identify_cnf(instance)
    -- we no longer support treepath and rootpath (was handy for testing);
    -- also we now follow the stupid route: if not set then just assume *one*
    -- cnf file under texmf (i.e. distribution)
    if #instance.cnffiles == 0 then
        if input.env(instance,'TEXMFCNF') == "" then
            local ownpath = environment.ownpath() or "."
            if ownpath then
                -- beware, this is tricky on my own system because at that location I do have
                -- the raw tree that ends up in the zip; i.e. I cannot test this kind of mess
                local function locate(filename,list)
                    local ownroot = input.normalize_name(file.join(ownpath,"../.."))
                    if not lfs.isdir(file.join(ownroot,"texmf")) then
                        ownroot = input.normalize_name(file.join(ownpath,".."))
                        if not lfs.isdir(file.join(ownroot,"texmf")) then
                            input.verbose = true
                            input.report("error", "unable to identify cnf file")
                            return
                        end
                    end
                    local texmfcnf = file.join(ownroot,"texmf-local/web2c",filename) -- for minimals and myself
                    if not lfs.isfile(texmfcnf) then
                        texmfcnf = file.join(ownroot,"texmf/web2c",filename)
                        if not lfs.isfile(texmfcnf) then
                            input.verbose = true
                            input.report("error", "unable to locate",filename)
                            return
                        end
                    end
                    table.insert(list,texmfcnf)
                    local ie = instance.environment
                    if not ie['SELFAUTOPARENT'] then ie['SELFAUTOPARENT'] = ownroot end
                    if not ie['TEXMFCNF']       then ie['TEXMFCNF']       = file.dirname(texmfcnf) end
                end
                locate(input.luaname,instance.luafiles)
                locate(input.cnfname,instance.cnffiles)
                if #instance.luafiles == 0 and instance.cnffiles == 0 then
                    input.verbose = true
                    input.report("error", "unable to locate",filename)
                    os.exit()
                end
                -- here we also assume then TEXMF is set in the distribution, if this trickery is
                -- used in the minimals, then users who don't use setuptex are on their own with
                -- regards to extra trees
            else
                input.verbose = true
                input.report("error", "unable to identify own path")
                os.exit()
            end
        else
            local t = input.split_path(input.env(instance,'TEXMFCNF'))
            t = input.aux.expanded_path(instance,t)
            input.aux.expand_vars(instance,t)
            local function locate(filename,list)
                for _,v in ipairs(t) do
                    local texmfcnf = input.normalize_name(file.join(v,filename))
                    if lfs.isfile(texmfcnf) then
                        table.insert(list,texmfcnf)
                    end
                end
            end
            locate(input.luaname,instance.luafiles)
            locate(input.cnfname,instance.cnffiles)
        end
    end
end

function input.load_cnf(instance)
    local function loadoldconfigdata()
        for _, fname in ipairs(instance.cnffiles) do
            input.aux.load_cnf(instance,fname)
        end
    end
    -- instance.cnffiles contain complete names now !
    if #instance.cnffiles == 0 then
        input.report("no cnf files found (TEXMFCNF may not be set/known)")
    else
        instance.rootpath = instance.cnffiles[1]
        for k,fname in ipairs(instance.cnffiles) do
            instance.cnffiles[k] = input.normalize_name(fname:gsub("\\",'/'))
        end
        for i=1,3 do
            instance.rootpath = file.dirname(instance.rootpath)
        end
        instance.rootpath = input.normalize_name(instance.rootpath)
        instance.environment['SELFAUTOPARENT'] = instance.rootpath -- just to be sure
        if instance.lsrmode then
            loadoldconfigdata()
        elseif instance.diskcache and not instance.renewcache then
            input.loadoldconfig(instance,instance.cnffiles)
            if instance.loaderror then
                loadoldconfigdata()
                input.saveoldconfig(instance)
            end
        else
            loadoldconfigdata()
            if instance.renewcache then
                input.saveoldconfig(instance)
            end
        end
        input.aux.collapse_cnf_data(instance)
    end
    input.checkconfigdata(instance)
end

function input.load_lua(instance)
    if #instance.luafiles == 0 then
        -- yet harmless
    else
        instance.rootpath = instance.luafiles[1]
        for k,fname in ipairs(instance.luafiles) do
            instance.luafiles[k] = input.normalize_name(fname:gsub("\\",'/'))
        end
        for i=1,3 do
            instance.rootpath = file.dirname(instance.rootpath)
        end
        instance.rootpath = input.normalize_name(instance.rootpath)
        instance.environment['SELFAUTOPARENT'] = instance.rootpath -- just to be sure
        input.loadnewconfig(instance)
        input.aux.collapse_cnf_data(instance)
    end
    input.checkconfigdata(instance)
end

function input.aux.collapse_cnf_data(instance) -- potential optmization: pass start index (setup and configuration are shared)
    for _,c in ipairs(instance.order) do
        for k,v in pairs(c) do
            if not instance.variables[k] then
                if instance.environment[k] then
                    instance.variables[k] = instance.environment[k]
                else
                    instance.kpsevars[k] = true
                    instance.variables[k] = input.bare_variable(v)
                end
            end
        end
    end
end

function input.aux.load_cnf(instance,fname)
    fname = input.clean_path(fname)
    local lname = fname:gsub("%.%a+$",input.luasuffix)
    local f = io.open(lname)
    if f then -- this will go
        f:close()
        local dname = file.dirname(fname)
        if not instance.configuration[dname] then
            input.aux.load_configuration(instance,dname,lname)
            instance.order[#instance.order+1] = instance.configuration[dname]
        end
    else
        f = io.open(fname)
        if f then
            input.report("loading", fname)
            local line, data, n, k, v
            local dname = file.dirname(fname)
            if not instance.configuration[dname] then
                instance.configuration[dname] = { }
                instance.order[#instance.order+1] = instance.configuration[dname]
            end
            local data = instance.configuration[dname]
            while true do
                local line, n = f:read(), 0
                if line then
                    while true do -- join lines
                        line, n = line:gsub("\\%s*$", "")
                        if n > 0 then
                            line = line .. f:read()
                        else
                            break
                        end
                    end
                    if not line:find("^[%%#]") then
                        local k, v = (line:gsub("%s*%%.*$","")):match("%s*(.-)%s*=%s*(.-)%s*$")
                        if k and v and not data[k] then
                            data[k] = (v:gsub("[%%#].*",'')):gsub("~", "$HOME")
                            instance.kpsevars[k] = true
                        end
                    end
                else
                    break
                end
            end
            f:close()
        else
            input.report("skipping", fname)
        end
    end
end

-- database loading

function input.load_hash(instance)
    input.locatelists(instance)
    if instance.lsrmode then
        input.loadlists(instance)
    elseif instance.diskcache and not instance.renewcache then
        input.loadfiles(instance)
        if instance.loaderror then
            input.loadlists(instance)
            input.savefiles(instance)
        end
    else
        input.loadlists(instance)
        if instance.renewcache then
            input.savefiles(instance)
        end
    end
end

function input.aux.append_hash(instance,type,tag,name)
    input.logger("= hash append",tag)
    table.insert(instance.hashes, { ['type']=type, ['tag']=tag, ['name']=name } )
end

function input.aux.prepend_hash(instance,type,tag,name)
    input.logger("= hash prepend",tag)
    table.insert(instance.hashes, 1, { ['type']=type, ['tag']=tag, ['name']=name } )
end

function input.aux.extend_texmf_var(instance,specification) -- crap
    if instance.environment['TEXMF'] then
        input.report("extending environment variable TEXMF with", specification)
        instance.environment['TEXMF'] = instance.environment['TEXMF']:gsub("^%{", function()
            return "{" .. specification .. ","
        end)
    elseif instance.variables['TEXMF'] then
        input.report("extending configuration variable TEXMF with", specification)
        instance.variables['TEXMF'] = instance.variables['TEXMF']:gsub("^%{", function()
            return "{" .. specification .. ","
        end)
    else
        input.report("setting configuration variable TEXMF to", specification)
        instance.variables['TEXMF'] = "{" .. specification .. "}"
    end
    if instance.variables['TEXMF']:find("%,") and not instance.variables['TEXMF']:find("^%{") then
        input.report("adding {} to complex TEXMF variable, best do that yourself")
        instance.variables['TEXMF'] = "{" .. instance.variables['TEXMF'] .. "}"
    end
    input.expand_variables(instance)
    input.reset_hashes(instance)
end

-- locators

function input.locatelists(instance)
    for _, path in pairs(input.simplified_list(input.expansion(instance,'TEXMF'))) do
        path = file.collapse_path(path)
        input.report("locating list of",path)
        input.locatedatabase(instance,input.normalize_name(path))
    end
end

function input.locatedatabase(instance,specification)
    return input.methodhandler('locators', instance, specification)
end

function input.locators.tex(instance,specification)
    if specification and specification ~= '' and lfs.isdir(specification) then
        input.logger('! tex locator', specification..' found')
        input.aux.append_hash(instance,'file',specification,filename)
    else
        input.logger('? tex locator', specification..' not found')
    end
end

-- hashers

function input.hashdatabase(instance,tag,name)
    return input.methodhandler('hashers',instance,tag,name)
end

function input.loadfiles(instance)
    instance.loaderror = false
    instance.files = { }
    if not instance.renewcache then
        for _, hash in ipairs(instance.hashes) do
            input.hashdatabase(instance,hash.tag,hash.name)
            if instance.loaderror then break end
        end
    end
end

function input.hashers.tex(instance,tag,name)
    input.aux.load_files(instance,tag)
end

-- generators:

function input.loadlists(instance)
    for _, hash in ipairs(instance.hashes) do
        input.generatedatabase(instance,hash.tag)
    end
end

function input.generatedatabase(instance,specification)
    return input.methodhandler('generators', instance, specification)
end

do

    local weird = lpeg.anywhere(lpeg.S("~`!#$%^&*()={}[]:;\"\'||<>,?\n\r\t"))

    function input.generators.tex(instance,specification)
        local tag = specification
        if not instance.lsrmode and lfs and lfs.dir then
            input.report("scanning path",specification)
            instance.files[tag] = { }
            local files = instance.files[tag]
            local n, m, r = 0, 0, 0
            local spec = specification .. '/'
            local attributes = lfs.attributes
            local directory = lfs.dir
            local small = instance.smallcache
            local function action(path)
                local mode, full
                if path then
                    full = spec .. path .. '/'
                else
                    full = spec
                end
                for name in directory(full) do
                    if name:find("^%.") then
                      -- skip
                --  elseif name:find("[%~%`%!%#%$%%%^%&%*%(%)%=%{%}%[%]%:%;\"\'%|%<%>%,%?\n\r\t]") then -- too much escaped
                    elseif weird:match(name) then
                      -- texio.write_nl("skipping " .. name)
                      -- skip
                    else
                        mode = attributes(full..name,'mode')
                        if mode == "directory" then
                            m = m + 1
                            if path then
                                action(path..'/'..name)
                            else
                                action(name)
                            end
                        elseif path and mode == 'file' then
                            n = n + 1
                            local f = files[name]
                            if f then
                                if not small then
                                    if type(f) == 'string' then
                                        files[name] = { f, path }
                                    else
                                      f[#f+1] = path
                                    end
                                end
                            else
                                files[name] = path
                                local lower = name:lower()
                                if name ~= lower then
                                    files["remap:"..lower] = name
                                    r = r + 1
                                end
                            end
                        end
                    end
                end
            end
            action()
            input.report(format("%s files found on %s directories with %s uppercase remappings",n,m,r))
        else
            local fullname = file.join(specification,input.lsrname)
            local path     = '.'
            local f        = io.open(fullname)
            if f then
                instance.files[tag] = { }
                local files = instance.files[tag]
                local small = instance.smallcache
                input.report("loading lsr file",fullname)
            --  for line in f:lines() do -- much slower then the next one
                for line in (f:read("*a")):gmatch("(.-)\n") do
                    if line:find("^[%a%d]") then
                        local fl = files[line]
                        if fl then
                            if not small then
                                if type(fl) == 'string' then
                                    files[line] = { fl, path } -- table
                                else
                                    fl[#fl+1] = path
                                end
                            end
                        else
                            files[line] = path -- string
                            local lower = line:lower()
                            if line ~= lower then
                                files["remap:"..lower] = line
                            end
                        end
                    else
                        path = line:match("%.%/(.-)%:$") or path -- match could be nil due to empty line
                    end
                end
                f:close()
            end
        end
    end

end

-- savers, todo

function input.savefiles(instance)
    input.aux.save_data(instance, 'files', function(k,v)
        return instance.validfile(k,v) -- path, name
    end)
end

-- A config (optionally) has the paths split in tables. Internally
-- we join them and split them after the expansion has taken place. This
-- is more convenient.

function input.splitconfig(instance)
    for i,c in ipairs(instance) do
        for k,v in pairs(c) do
            if type(v) == 'string' then
                local t = file.split_path(v)
                if #t > 1 then
                    c[k] = t
                end
            end
        end
    end
end
function input.joinconfig(instance)
    for i,c in ipairs(instance.order) do
        for k,v in pairs(c) do
            if type(v) == 'table' then
                c[k] = file.join_path(v)
            end
        end
    end
end
function input.split_path(str)
    if type(str) == 'table' then
        return str
    else
        return file.split_path(str)
    end
end
function input.join_path(str)
    if type(str) == 'table' then
        return file.join_path(str)
    else
        return str
    end
end

function input.splitexpansions(instance)
    for k,v in pairs(instance.expansions) do
        local t, h = { }, { }
        for _,vv in pairs(file.split_path(v)) do
            if vv ~= "" and not h[vv] then
                t[#t+1] = vv
                h[vv] = true
            end
        end
        if #t > 1 then
            instance.expansions[k] = t
        else
            instance.expansions[k] = t[1]
        end
    end
end

-- end of split/join code

function input.saveoldconfig(instance)
    input.splitconfig(instance)
    input.aux.save_data(instance, 'configuration', nil)
    input.joinconfig(instance)
end

input.configbanner = [[
-- This is a Luatex configuration file created by 'luatools.lua' or
-- 'luatex.exe' directly. For comment, suggestions and questions you can
-- contact the ConTeXt Development Team. This configuration file is
-- not copyrighted. [HH & TH]
]]

function input.serialize(files)
    -- This version is somewhat optimized for the kind of
    -- tables that we deal with, so it's much faster than
    -- the generic serializer. This makes sense because
    -- luatools and mtxtools are called frequently. Okay,
    -- we pay a small price for properly tabbed tables.
    local t = { }
    local concat = table.concat
    local sorted = table.sortedkeys
    local function dump(k,v,m)
        if type(v) == 'string' then
            return m .. "['" .. k .. "']='" .. v .. "',"
        elseif #v == 1 then
            return m .. "['" .. k .. "']='" .. v[1] .. "',"
        else
            return m .. "['" .. k .. "']={'" .. concat(v,"','").. "'},"
        end
    end
    t[#t+1] = "return {"
    if instance.sortdata then
        for _, k in pairs(sorted(files)) do
            local fk  = files[k]
            if type(fk) == 'table' then
                t[#t+1] = "\t['" .. k .. "']={"
                for _, kk in pairs(sorted(fk)) do
                    t[#t+1] = dump(kk,fk[kk],"\t\t")
                end
                t[#t+1] = "\t},"
            else
                t[#t+1] = dump(k,fk,"\t")
            end
        end
    else
        for k, v in pairs(files) do
            if type(v) == 'table' then
                t[#t+1] = "\t['" .. k .. "']={"
                for kk,vv in pairs(v) do
                    t[#t+1] = dump(kk,vv,"\t\t")
                end
                t[#t+1] = "\t},"
            else
                t[#t+1] = dump(k,v,"\t")
            end
        end
    end
    t[#t+1] = "}"
    return concat(t,"\n")
end

if not texmf then texmf = {} end -- no longer needed, at least not here

function input.aux.save_data(instance, dataname, check, makename) -- untested without cache overload
    for cachename, files in pairs(instance[dataname]) do
        local name = (makename or file.join)(cachename,dataname)
        local luaname, lucname = name .. input.luasuffix, name .. input.lucsuffix
        input.report("preparing " .. dataname .. " for", luaname)
        for k, v in pairs(files) do
            if not check or check(v,k) then -- path, name
                if type(v) == "table" and #v == 1 then
                    files[k] = v[1]
                end
            else
                files[k] = nil -- false
            end
        end
        local data = {
            type    = dataname,
            root    = cachename,
            version = input.cacheversion,
            date    = os.date("%Y-%m-%d"),
            time    = os.date("%H:%M:%S"),
            content = files,
        }
        local f = io.open(luaname,'w')
        if f then
            input.report("saving " .. dataname .. " in", luaname)
            f:write(input.serialize(data))
            f:close()
            input.report("compiling " .. dataname .. " to", lucname)
            if not utils.lua.compile(luaname,lucname) then
                input.report("compiling failed for " .. dataname .. ", deleting file " .. lucname)
                os.remove(lucname)
            end
        else
            input.report("unable to save " .. dataname .. " in " .. name..input.luasuffix)
        end
    end
end

function input.aux.load_data(instance,pathname,dataname,filename,makename) -- untested without cache overload
    filename = ((not filename or (filename == "")) and dataname) or filename
    filename = (makename and makename(dataname,filename)) or file.join(pathname,filename)
    local blob = loadfile(filename .. input.lucsuffix) or loadfile(filename .. input.luasuffix)
    if blob then
        local data = blob()
        if data and data.content and data.type == dataname and data.version == input.cacheversion then
            input.report("loading",dataname,"for",pathname,"from",filename)
            instance[dataname][pathname] = data.content
        else
            input.report("skipping",dataname,"for",pathname,"from",filename)
            instance[dataname][pathname] = { }
            instance.loaderror = true
        end
    else
        input.report("skipping",dataname,"for",pathname,"from",filename)
    end
end

-- some day i'll use the nested approach, but not yet (actually we even drop
-- engine/progname support since we have only luatex now)
--
-- first texmfcnf.lua files are located, next the cached texmf.cnf files
--
-- return {
--     TEXMFBOGUS = 'effe checken of dit werkt',
-- }

function input.aux.load_texmfcnf(instance,dataname,pathname)
    local filename = file.join(pathname,input.luaname)
    local blob = loadfile(filename)
    if blob then
        local data = blob()
        if data then
            input.report("loading","configuration file",filename)
            if true then
                -- flatten to variable.progname
                local t = { }
                for k, v in pairs(data) do -- v = progname
                    if type(v) == "string" then
                        t[k] = v
                    else
                        for kk, vv in pairs(v) do -- vv = variable
                            if type(vv) == "string" then
                                t[vv.."."..v] = kk
                            end
                        end
                    end
                end
                instance[dataname][pathname] = t
            else
                instance[dataname][pathname] = data
            end
        else
            input.report("skipping","configuration file",filename)
            instance[dataname][pathname] = { }
            instance.loaderror = true
        end
    else
        input.report("skipping","configuration file",filename)
    end
end

function input.aux.load_configuration(instance,dname,lname)
    input.aux.load_data(instance,dname,'configuration',lname and file.basename(lname))
end
function input.aux.load_files(instance,tag)
    input.aux.load_data(instance,tag,'files')
end

function input.resetconfig(instance)
    instance.configuration, instance.setup, instance.order, instance.loaderror = { }, { }, { }, false
end

function input.loadnewconfig(instance)
    for _, cnf in ipairs(instance.luafiles) do
        local dname = file.dirname(cnf)
        input.aux.load_texmfcnf(instance,'setup',dname)
        instance.order[#instance.order+1] = instance.setup[dname]
        if instance.loaderror then break end
    end
end

function input.loadoldconfig(instance)
    if not instance.renewcache then
        for _, cnf in ipairs(instance.cnffiles) do
            local dname = file.dirname(cnf)
            input.aux.load_configuration(instance,dname)
            instance.order[#instance.order+1] = instance.configuration[dname]
            if instance.loaderror then break end
        end
    end
    input.joinconfig(instance)
end

function input.expand_variables(instance)
    instance.expansions = { }
--~ instance.environment['SELFAUTOPARENT'] = instance.environment['SELFAUTOPARENT'] or instance.rootpath
    if instance.engine   ~= "" then instance.environment['engine']   = instance.engine   end
    if instance.progname ~= "" then instance.environment['progname'] = instance.progname end
    for k,v in pairs(instance.environment) do
        local a, b = k:match("^(%a+)%_(.*)%s*$")
        if a and b then
            instance.expansions[a..'.'..b] = v
        else
            instance.expansions[k] = v
        end
    end
    for k,v in pairs(instance.environment) do -- move environment to expansions
        if not instance.expansions[k] then instance.expansions[k] = v end
    end
    for k,v in pairs(instance.variables) do -- move variables to expansions
        if not instance.expansions[k] then instance.expansions[k] = v end
    end
    while true do
        local busy = false
        for k,v in pairs(instance.expansions) do
            local s, n = v:gsub("%$([%a%d%_%-]+)", function(a)
                busy = true
                return instance.expansions[a] or input.env(instance,a)
            end)
            local s, m = s:gsub("%$%{([%a%d%_%-]+)%}", function(a)
                busy = true
                return instance.expansions[a] or input.env(instance,a)
            end)
            if n > 0 or m > 0 then
                instance.expansions[k]= s
            end
        end
        if not busy then break end
    end
    for k,v in pairs(instance.expansions) do
        instance.expansions[k] = v:gsub("\\", '/')
    end
end

function input.aux.expand_vars(instance,lst) -- simple vars
    for k,v in pairs(lst) do
        lst[k] = v:gsub("%$([%a%d%_%-]+)", function(a)
            return instance.variables[a] or input.env(instance,a)
        end)
    end
end

function input.aux.expanded_var(instance,var) -- simple vars
    return var:gsub("%$([%a%d%_%-]+)", function(a)
        return instance.variables[a] or input.env(instance,a)
    end)
end

function input.aux.entry(instance,entries,name)
    if name and (name ~= "") then
        name = name:gsub('%$','')
        local result = entries[name..'.'..instance.progname] or entries[name]
        if result then
            return result
        else
            result = input.env(instance,name)
            if result then
                instance.variables[name] = result
                input.expand_variables(instance)
                return instance.expansions[name] or ""
            end
        end
    end
    return ""
end
function input.variable(instance,name)
    return input.aux.entry(instance,instance.variables,name)
end
function input.expansion(instance,name)
    return input.aux.entry(instance,instance.expansions,name)
end

function input.aux.is_entry(instance,entries,name)
    if name and name ~= "" then
        name = name:gsub('%$','')
        return (entries[name..'.'..instance.progname] or entries[name]) ~= nil
    else
        return false
    end
end

function input.is_variable(instance,name)
    return input.aux.is_entry(instance,instance.variables,name)
end
function input.is_expansion(instance,name)
    return input.aux.is_entry(instance,instance.expansions,name)
end

function input.simplified_list(str)
    if type(str) == 'table' then
        return str -- troubles ; ipv , in texmf
    elseif str == '' then
        return { }
    else
        local t = { }
        for _,v in ipairs(string.splitchr(str:gsub("^\{(.+)\}$","%1"),",")) do
            t[#t+1] = (v:gsub("^[%!]*(.+)[%/\\]*$","%1"))
        end
        return t
    end
end

function input.unexpanded_path_list(instance,str)
    local pth = input.variable(instance,str)
    local lst = input.split_path(pth)
    return input.aux.expanded_path(instance,lst)
end
function input.unexpanded_path(instance,str)
    return file.join_path(input.unexpanded_path_list(instance,str))
end

do
    local done = { }

    function input.reset_extra_path(instance)
        local ep = instance.extra_paths
        if not ep then
            ep, done = { }, { }
            instance.extra_paths = ep
        elseif #ep > 0 then
            instance.lists, done = { }, { }
        end
    end

    function input.register_extra_path(instance,paths,subpaths)
        local ep = instance.extra_paths or { }
        local n = #ep
        if paths and paths ~= "" then
            if subpaths and subpaths ~= "" then
                for p in paths:gmatch("[^,]+") do
                    -- we gmatch each step again, not that fast, but used seldom
                    for s in subpaths:gmatch("[^,]+") do
                        local ps = p .. "/" .. s
                        if not done[ps] then
                            ep[#ep+1] = input.clean_path(ps)
                            done[ps] = true
                        end
                    end
                end
            else
                for p in paths:gmatch("[^,]+") do
                    if not done[p] then
                        ep[#ep+1] = input.clean_path(p)
                        done[p] = true
                    end
                end
            end
        elseif subpaths and subpaths ~= "" then
            for i=1,n do
                -- we gmatch each step again, not that fast, but used seldom
                for s in subpaths:gmatch("[^,]+") do
                    local ps = ep[i] .. "/" .. s
                    if not done[ps] then
                        ep[#ep+1] = input.clean_path(ps)
                        done[ps] = true
                    end
                end
            end
        end
        if #ep > 0 then
            instance.extra_paths = ep -- register paths
        end
        if #ep > n then
            instance.lists = { } -- erase the cache
        end
    end

end

function input.expanded_path_list(instance,str)
    local function made_list(list)
        local ep = instance.extra_paths
        if not ep or #ep == 0 then
            return list
        else
            local done, new = { }, { }
            -- honour . .. ../.. but only when at the start
            for k, v in ipairs(list) do
                if not done[v] then
                    if v:find("^[%.%/]$") then
                        done[v] = true
                        new[#new+1] = v
                    else
                        break
                    end
                end
            end
            -- first the extra paths
            for k, v in ipairs(ep) do
                if not done[v] then
                    done[v] = true
                    new[#new+1] = v
                end
            end
            -- next the formal paths
            for k, v in ipairs(list) do
                if not done[v] then
                    done[v] = true
                    new[#new+1] = v
                end
            end
            return new
        end
    end
    if not str then
        return ep or { }
    elseif instance.savelists then
        -- engine+progname hash
        str = str:gsub("%$","")
        if not instance.lists[str] then -- cached
            local lst = made_list(input.split_path(input.expansion(instance,str)))
            instance.lists[str] = input.aux.expanded_path(instance,lst)
        end
        return instance.lists[str]
    else
        local lst = input.split_path(input.expansion(instance,str))
        return made_list(input.aux.expanded_path(instance,lst))
    end
end

function input.expand_path(instance,str)
    return file.join_path(input.expanded_path_list(instance,str))
end

--~ function input.first_writable_path(instance,name)
--~     for _,v in pairs(input.expanded_path_list(instance,name)) do
--~         if file.is_writable(file.join(v,'luatex-cache.tmp')) then
--~             return v
--~         end
--~     end
--~     return "."
--~ end

function input.expanded_path_list_from_var(instance,str) -- brrr
    local tmp = input.var_of_format_or_suffix(str:gsub("%$",""))
    if tmp ~= "" then
        return input.expanded_path_list(instance,str)
    else
        return input.expanded_path_list(instance,tmp)
    end
end
function input.expand_path_from_var(instance,str)
    return file.join_path(input.expanded_path_list_from_var(instance,str))
end

function input.format_of_var(str)
    return input.formats[str] or input.formats[input.alternatives[str]] or ''
end
function input.format_of_suffix(str)
    return input.suffixmap[file.extname(str)] or 'tex'
end

function input.variable_of_format(str)
    return input.formats[str] or input.formats[input.alternatives[str]] or ''
end

function input.var_of_format_or_suffix(str)
    local v = input.formats[str]
    if v then
        return v
    end
    v = input.formats[input.alternatives[str]]
    if v then
        return v
    end
    v = input.suffixmap[file.extname(str)]
    if v then
        return input.formats[isf]
    end
    return ''
end

function input.expand_braces(instance,str) -- output variable and brace expansion of STRING
    local ori = input.variable(instance,str)
    local pth = input.aux.expanded_path(instance,input.split_path(ori))
    return file.join_path(pth)
end

-- {a,b,c,d}
-- a,b,c/{p,q,r},d
-- a,b,c/{p,q,r}/d/{x,y,z}//
-- a,b,c/{p,q/{x,y,z},r},d/{p,q,r}
-- a,b,c/{p,q/{x,y,z},r},d/{p,q,r}
-- a{b,c}{d,e}f
-- {a,b,c,d}
-- {a,b,c/{p,q,r},d}
-- {a,b,c/{p,q,r}/d/{x,y,z}//}
-- {a,b,c/{p,q/{x,y,z}},d/{p,q,r}}
-- {a,b,c/{p,q/{x,y,z},w}v,d/{p,q,r}}

-- this one is better and faster, but it took me a while to realize
-- that this kind of replacement is cleaner than messy parsing and
-- fuzzy concatenating we can probably gain a bit with selectively
-- applying lpeg, but experiments with lpeg parsing this proved not to
-- work that well; the parsing is ok, but dealing with the resulting
-- table is a pain because we need to work inside-out recursively

-- get rid of piecewise here, just a gmatch is ok

function input.aux.splitpathexpr(str, t, validate)
    -- no need for optimization, only called a few times, we can use lpeg for the sub
    t = t or { }
    local concat = table.concat
    while true do
        local done = false
        while true do
            local ok = false
            str = str:gsub("([^{},]+){([^{}]-)}", function(a,b)
                local t = { }
                b:piecewise(",", function(s) t[#t+1] = a .. s end)
                ok, done = true, true
                return "{" .. concat(t,",") .. "}"
            end)
            if not ok then break end
        end
        while true do
            local ok = false
            str = str:gsub("{([^{}]-)}([^{},]+)", function(a,b)
                local t = { }
                a:piecewise(",", function(s) t[#t+1] = s .. b end)
                ok, done = true, true
                return "{" .. concat(t,",") .. "}"
            end)
            if not ok then break end
        end
        while true do
            local ok = false
            str = str:gsub("([,{]){([^{}]+)}([,}])", function(a,b,c)
                ok, done = true, true
                return a .. b .. c
            end)
            if not ok then break end
        end
        if not done then break end
    end
    while true do
        local ok = false
        str = str:gsub("{([^{}]-)}{([^{}]-)}", function(a,b)
            local t = { }
            a:piecewise(",", function(sa)
                b:piecewise(",", function(sb)
                    t[#t+1] = sa .. sb
                end)
            end)
            ok = true
            return "{" .. concat(t,",") .. "}"
        end)
        if not ok then break end
    end
    while true do
        local ok = false
        str = str:gsub("{([^{}]-)}", function(a)
            ok = true
            return a
        end)
        if not ok then break end
    end
    if validate then
        str:piecewise(",", function(s)
            s = validate(s)
            if s then t[#t+1] = s end
        end)
    else
        str:piecewise(",", function(s)
            t[#t+1] = s
        end)
    end
    return t
end

function input.aux.expanded_path(instance,pathlist) -- maybe not a list, just a path
    -- a previous version fed back into pathlist
    local newlist, ok = { }, false
    for _,v in ipairs(pathlist) do
        if v:find("[{}]") then
            ok = true
            break
        end
    end
    if ok then
        for _, v in ipairs(pathlist) do
            input.aux.splitpathexpr(v, newlist, function(s)
                s = file.collapse_path(s)
                return s ~= "" and not s:find(instance.dummy_path_expr) and s
            end)
        end
    else
        for _,v in ipairs(pathlist) do
            for vv in string.gmatch(v..',',"(.-),") do
                vv = file.collapse_path(v)
                if vv ~= "" then newlist[#newlist+1] = vv end
            end
        end
    end
    return newlist
end

input.is_readable = { }

function input.aux.is_readable(readable, name)
    if input.trace > 2 then
        if readable then
            input.logger("+ readable", name)
        else
            input.logger("- readable", name)
        end
    end
    return readable
end

function input.is_readable.file(name)
 -- return input.aux.is_readable(file.is_readable(name), name)
    return input.aux.is_readable(input.aux.is_file(name), name)
end

input.is_readable.tex = input.is_readable.file

-- name
-- name/name

function input.aux.collect_files(instance,names)
    local filelist = { }
    for _, fname in pairs(names) do
        if fname then
            if input.trace > 2 then
                input.logger("? blobpath asked",fname)
            end
            local bname = file.basename(fname)
            local dname = file.dirname(fname)
            if dname == "" or dname:find("^%.") then
                dname = false
            else
                dname = "/" .. dname .. "$"
            end
            for _, hash in ipairs(instance.hashes) do
                local blobpath = hash.tag
                local files = blobpath and instance.files[blobpath]
                if files then
                    if input.trace > 2 then
                        input.logger('? blobpath do',blobpath .. " (" .. bname ..")")
                    end
                    local blobfile = files[bname]
                    if not blobfile then
                        local rname = "remap:"..bname
                        blobfile = files[rname]
                        if blobfile then
                            bname = files[rname]
                            blobfile = files[bname]
                        end
                    end
                    if blobfile then
                        if type(blobfile) == 'string' then
                            if not dname or blobfile:find(dname) then
                                filelist[#filelist+1] = {
                                    hash.type,
                                    file.join(blobpath,blobfile,bname), -- search
                                    input.concatinators[hash.type](blobpath,blobfile,bname) -- result
                                }
                            end
                        else
                            for _, vv in pairs(blobfile) do
                                if not dname or vv:find(dname) then
                                    filelist[#filelist+1] = {
                                        hash.type,
                                        file.join(blobpath,vv,bname), -- search
                                        input.concatinators[hash.type](blobpath,vv,bname) -- result
                                    }
                                end
                            end
                        end
                    end
                elseif input.trace > 1 then
                    input.logger('! blobpath no',blobpath .. " (" .. bname ..")" )
                end
            end
        end
    end
    if #filelist > 0 then
        return filelist
    else
        return nil
    end
end

function input.suffix_of_format(str)
    if input.suffixes[str] then
        return input.suffixes[str][1]
    else
        return ""
    end
end

function input.suffixes_of_format(str)
    if input.suffixes[str] then
        return input.suffixes[str]
    else
        return {}
    end
end

do

    -- called about 700 times for an empty doc (font initializations etc)
    -- i need to weed the font files for redundant calls

    local letter     = lpeg.R("az","AZ")
    local separator  = lpeg.P("://")

    local qualified = lpeg.P(".")^0 * lpeg.P("/") + letter*lpeg.P(":") + letter^1*separator
    local rootbased = lpeg.P("/") + letter*lpeg.P(":")

    -- ./name ../name  /name c: ://
    function input.aux.qualified_path(filename)
        return qualified:match(filename)
    end
    function input.aux.rootbased_path(filename)
        return rootbased:match(filename)
    end

    function input.normalize_name(original)
        return original
    end

    input.normalize_name = file.collapse_path

end

function input.aux.register_in_trees(instance,name)
    if not name:find("^%.") then
        instance.foundintrees[name] = (instance.foundintrees[name] or 0) + 1 -- maybe only one
    end
end

-- split the next one up, better for jit

function input.aux.find_file(instance,filename) -- todo : plugin (scanners, checkers etc)
    local result = { }
    local stamp  = nil
    filename = input.normalize_name(filename)  -- elsewhere
    filename = file.collapse_path(filename:gsub("\\","/")) -- elsewhere
    -- speed up / beware: format problem
    if instance.remember then
        stamp = filename .. "--" .. instance.engine .. "--" .. instance.progname .. "--" .. instance.format
        if instance.found[stamp] then
            input.logger('! remembered', filename)
            return instance.found[stamp]
        end
    end
    if filename:find('%*') then
        input.logger('! wildcard', filename)
        result = input.find_wildcard_files(instance,filename)
    elseif input.aux.qualified_path(filename) then
        if input.is_readable.file(filename) then
            input.logger('! qualified', filename)
            result = { filename }
        else
            local forcedname, ok = "", false
            if file.extname(filename) == "" then
                if instance.format == "" then
                    forcedname = filename .. ".tex"
                    if input.is_readable.file(forcedname) then
                        input.logger('! no suffix, forcing standard filetype tex')
                        result, ok = { forcedname }, true
                    end
                else
                    for _, s in pairs(input.suffixes_of_format(instance.format)) do
                        forcedname = filename .. "." .. s
                        if input.is_readable.file(forcedname) then
                            input.logger('! no suffix, forcing format filetype', s)
                            result, ok = { forcedname }, true
                            break
                        end
                    end
                end
            end
            if not ok then
                input.logger('? qualified', filename)
            end
        end
    else
        -- search spec
        local filetype, extra, done, wantedfiles, ext = '', nil, false, { }, file.extname(filename)
        if ext == "" then
            if not instance.force_suffixes then
                wantedfiles[#wantedfiles+1] = filename
            end
        else
            wantedfiles[#wantedfiles+1] = filename
        end
        if instance.format == "" then
            if ext == "" then
                local forcedname = filename .. '.tex'
                wantedfiles[#wantedfiles+1] = forcedname
                filetype = input.format_of_suffix(forcedname)
                input.logger('! forcing filetype',filetype)
            else
                filetype = input.format_of_suffix(filename)
                input.logger('! using suffix based filetype',filetype)
            end
        else
            if ext == "" then
                for _, s in pairs(input.suffixes_of_format(instance.format)) do
                    wantedfiles[#wantedfiles+1] = filename .. "." .. s
                end
            end
            filetype = instance.format
            input.logger('! using given filetype',filetype)
        end
        local typespec = input.variable_of_format(filetype)
        local pathlist = input.expanded_path_list(instance,typespec)
        if not pathlist or #pathlist == 0 then
            -- no pathlist, access check only / todo == wildcard
            if input.trace > 2 then
                input.logger('? filename',filename)
                input.logger('? filetype',filetype or '?')
                input.logger('? wanted files',table.concat(wantedfiles," | "))
            end
            for _, fname in pairs(wantedfiles) do
                if fname and input.is_readable.file(fname) then
                    filename, done = fname, true
                    result[#result+1] = file.join('.',fname)
                    break
                end
            end
            -- this is actually 'other text files' or 'any' or 'whatever'
            local filelist = input.aux.collect_files(instance,wantedfiles)
            local fl = filelist and filelist[1]
            if fl then
                filename = fl[3]
                result[#result+1] = filename
                done = true
            end
        else
            -- list search
            local filelist = input.aux.collect_files(instance,wantedfiles)
            local doscan, recurse
            if input.trace > 2 then
                input.logger('? filename',filename)
            --                if pathlist then input.logger('? path list',table.concat(pathlist," | ")) end
            --                if filelist then input.logger('? file list',table.concat(filelist," | ")) end
            end
            -- a bit messy ... esp the doscan setting here
            for _, path in pairs(pathlist) do
                if path:find("^!!") then doscan  = false else doscan  = true  end
                if path:find("//$") then recurse = true  else recurse = false end
                local pathname = path:gsub("^!+", '')
                done = false
                -- using file list
                if filelist and not (done and not instance.allresults) and recurse then
                    -- compare list entries with permitted pattern
                    pathname = pathname:gsub("([%-%.])","%%%1") -- this also influences
                    pathname = pathname:gsub("/+$", '/.*')      -- later usage of pathname
                    pathname = pathname:gsub("//", '/.-/')      -- not ok for /// but harmless
                    local expr = "^" .. pathname
                    -- input.debug('?',expr)
                    for _, fl in ipairs(filelist) do
                        local f = fl[2]
                        if f:find(expr) then
                            -- input.debug('T',' '..f)
                            if input.trace > 2 then
                                input.logger('= found in hash',f)
                            end
                            --- todo, test for readable
                            result[#result+1] = fl[3]
                            input.aux.register_in_trees(instance,f) -- for tracing used files
                            done = true
                            if not instance.allresults then break end
                        else
                            -- input.debug('F',' '..f)
                        end
                    end
                end
                if not done and doscan then
                    -- check if on disk / unchecked / does not work at all / also zips
                    if input.method_is_file(pathname) then -- ?
                        local pname = pathname:gsub("%.%*$",'')
                        if not pname:find("%*") then
                            local ppname = pname:gsub("/+$","")
                            if input.aux.can_be_dir(instance,ppname) then
                                for _, w in pairs(wantedfiles) do
                                    local fname = file.join(ppname,w)
                                    if input.is_readable.file(fname) then
                                        if input.trace > 2 then
                                            input.logger('= found by scanning',fname)
                                        end
                                        result[#result+1] = fname
                                        done = true
                                        if not instance.allresults then break end
                                    end
                                end
                            else
                                -- no access needed for non existing path, speedup (esp in large tree with lots of fake)
                            end
                        end
                    end
                end
                if not done and doscan then
                    -- todo: slow path scanning
                end
                if done and not instance.allresults then break end
            end
        end
    end
    for k,v in pairs(result) do
        result[k] = file.collapse_path(v)
    end
    if instance.remember then
        instance.found[stamp] = result
    end
    return result
end

input.aux._find_file_ = input.aux.find_file

function input.aux.find_file(instance,filename) -- maybe make a lowres cache too
    local result = input.aux._find_file_(instance,filename)
    if #result == 0 then
        local lowered = filename:lower()
        if filename ~= lowered then
            return input.aux._find_file_(instance,lowered)
        end
    end
    return result
end

if lfs and lfs.isfile then
    input.aux.is_file = lfs.isfile      -- to be done: use this
else
    input.aux.is_file = file.is_readable
end

if lfs and lfs.isdir then
    function input.aux.can_be_dir(instance,name)
        if not instance.fakepaths[name] then
            if lfs.isdir(name) then
                instance.fakepaths[name] = 1 -- directory
            else
                instance.fakepaths[name] = 2 -- no directory
            end
        end
        return (instance.fakepaths[name] == 1)
    end
else
    function input.aux.can_be_dir()
        return true
    end
end

if not input.concatinators  then input.concatinators = { } end

input.concatinators.tex  = file.join
input.concatinators.file = input.concatinators.tex

function input.find_files(instance,filename,filetype,mustexist)
    if type(mustexist) == boolean then
        -- all set
    elseif type(filetype) == 'boolean' then
        filetype, mustexist = nil, false
    elseif type(filetype) ~= 'string' then
        filetype, mustexist = nil, false
    end
    instance.format = filetype or ''
    local t = input.aux.find_file(instance,filename,true)
    instance.format = ''
    return t
end

function input.find_file(instance,filename,filetype,mustexist)
    return (input.find_files(instance,filename,filetype,mustexist)[1] or "")
end

function input.find_given_files(instance,filename)
    local bname, result = file.basename(filename), { }
    for k, hash in ipairs(instance.hashes) do
        local files = instance.files[hash.tag]
        local blist = files[bname]
        if not blist then
            local rname = "remap:"..bname
            blist = files[rname]
            if blist then
                bname = files[rname]
                blist = files[bname]
            end
        end
        if blist then
            if type(blist) == 'string' then
                result[#result+1] = input.concatinators[hash.type](hash.tag,blist,bname) or ""
                if not instance.allresults then break end
            else
                for kk,vv in pairs(blist) do
                    result[#result+1] = input.concatinators[hash.type](hash.tag,vv,bname) or ""
                    if not instance.allresults then break end
                end
            end
        end
    end
    return result
end

function input.find_given_file(instance,filename)
    return (input.find_given_files(instance,filename)[1] or "")
end

function input.find_wildcard_files(instance,filename) -- todo: remap:
    local result = { }
    local bname, dname = file.basename(filename), file.dirname(filename)
    local path = dname:gsub("^*/","")
    path = path:gsub("*",".*")
    path = path:gsub("-","%%-")
    if dname == "" then
        path = ".*"
    end
    local name = bname
    name = name:gsub("*",".*")
    name = name:gsub("-","%%-")
    path = path:lower()
    name = name:lower()
    local function doit(blist,bname,hash,allresults)
        local done = false
        if blist then
            if type(blist) == 'string' then
                -- make function and share code
                if (blist:lower()):find(path) then
                    result[#result+1] = input.concatinators[hash.type](hash.tag,blist,bname) or ""
                    done = true
                end
            else
                for kk,vv in pairs(blist) do
                    if (vv:lower()):find(path) then
                        result[#result+1] = input.concatinators[hash.type](hash.tag,vv,bname) or ""
                        done = true
                        if not allresults then break end
                    end
                end
            end
        end
        return done
    end
    local files, allresults, done = instance.files, instance.allresults, false
    if name:find("%*") then
        for k, hash in ipairs(instance.hashes) do
            for kk, hh in pairs(files[hash.tag]) do
                if not kk:find("^remap:") then
                    if (kk:lower()):find(name) then
                        if doit(hh,kk,hash,allresults) then done = true end
                        if done and not allresults then break end
                    end
                end
            end
        end
    else
        for k, hash in ipairs(instance.hashes) do
            if doit(files[hash.tag][bname],bname,hash,allresults) then done = true end
            if done and not allresults then break end
        end
    end
    return result
end

function input.find_wildcard_file(instance,filename)
    return (input.find_wildcard_files(instance,filename)[1] or "")
end

-- main user functions

function input.save_used_files_in_trees(instance, filename,jobname)
    if not filename then filename = 'luatex.jlg' end
    local f = io.open(filename,'w')
    if f then
        f:write("<?xml version='1.0' standalone='yes'?>\n")
        f:write("<rl:job>\n")
        if jobname then
            f:write("\t<rl:name>" .. jobname .. "</rl:name>\n")
        end
        f:write("\t<rl:files>\n")
        for _,v in pairs(table.sortedkeys(instance.foundintrees)) do
            f:write("\t\t<rl:file n='" .. instance.foundintrees[v] .. "'>" .. v .. "</rl:file>\n")
        end
        f:write("\t</rl:files>\n")
        f:write("</rl:usedfiles>\n")
        f:close()
    end
end

function input.automount(instance)
    -- implemented later
end

function input.load(instance)
    input.starttiming(instance)
    input.resetconfig(instance)
    input.identify_cnf(instance)
    input.load_lua(instance)
    input.expand_variables(instance)
    input.load_cnf(instance)
    input.expand_variables(instance)
    input.load_hash(instance)
    input.automount(instance)
    input.stoptiming(instance)
end

function input.for_files(instance, command, files, filetype, mustexist)
    if files and #files > 0 then
        local function report(str)
            if input.verbose then
                input.report(str) -- has already verbose
            else
                print(str)
            end
        end
        if input.verbose then
            report('')
        end
        for _, file in pairs(files) do
            local result = command(instance,file,filetype,mustexist)
            if type(result) == 'string' then
                report(result)
            else
                for _,v in pairs(result) do
                    report(v)
                end
            end
        end
    end
end

-- strtab

function input.var_value(instance,str)     -- output the value of variable $STRING.
    return input.variable(instance,str)
end
function input.expand_var(instance,str)    -- output variable expansion of STRING.
    return input.expansion(instance,str)
end
function input.show_path(instance,str)     -- output search path for file type NAME
    return file.join_path(input.expanded_path_list(instance,input.format_of_var(str)))
end

-- input.find_file(filename)
-- input.find_file(filename, filetype, mustexist)
-- input.find_file(filename, mustexist)
-- input.find_file(filename, filetype)

function input.aux.register_file(files, name, path)
    if files[name] then
        if type(files[name]) == 'string' then
            files[name] = { files[name], path }
        else
            files[name] = path
        end
    else
        files[name] = path
    end
end

if not input.finders  then input.finders  = { } end
if not input.openers  then input.openers  = { } end
if not input.loaders  then input.loaders  = { } end

input.finders.notfound  = { nil }
input.openers.notfound  = { nil }
input.loaders.notfound  = { false, nil, 0 }

function input.splitmethod(filename)
    if not filename then
        return { } -- safeguard
    elseif type(filename) == "table" then
        return filename -- already split
    elseif not filename:find("://") then
        return { scheme="file", path = filename, original=filename } -- quick hack
    else
        return url.hashed(filename)
    end
end

function input.method_is_file(filename)
    return input.splitmethod(filename).scheme == 'file'
end

function table.sequenced(t,sep) -- temp here
    local s = { }
    for k, v in pairs(t) do
        s[#s+1] = k .. "=" .. v
    end
    return table.concat(s, sep or " | ")
end

function input.methodhandler(what, instance, filename, filetype) -- ...
    local specification = (type(filename) == "string" and input.splitmethod(filename)) or filename -- no or { }, let it bomb
    local scheme = specification.scheme
    if input[what][scheme] then
        input.logger('= handler',specification.original .." -> " .. what .. " -> " .. table.sequenced(specification))
        return input[what][scheme](instance,filename,filetype) -- todo: specification
    else
        return input[what].tex(instance,filename,filetype) -- todo: specification
    end
end

-- also inside next test?

function input.findtexfile(instance, filename, filetype)
    return input.methodhandler('finders',instance, input.normalize_name(filename), filetype)
end
function input.opentexfile(instance,filename)
    return input.methodhandler('openers',instance, input.normalize_name(filename))
end

function input.findbinfile(instance, filename, filetype)
    return input.methodhandler('finders',instance, input.normalize_name(filename), filetype)
end
function input.openbinfile(instance,filename)
    return input.methodhandler('loaders',instance, input.normalize_name(filename))
end

function input.loadbinfile(instance, filename, filetype)
    local fname = input.findbinfile(instance, input.normalize_name(filename), filetype)
    if fname and fname ~= "" then
        return input.openbinfile(instance,fname)
    else
        return unpack(input.loaders.notfound)
    end
end

function input.texdatablob(instance, filename, filetype)
    local ok, data, size = input.loadbinfile(instance, filename, filetype)
    return data or ""
end

input.loadtexfile = input.texdatablob

function input.openfile(filename) -- brrr texmf.instance here  / todo ! ! ! ! !
    local fullname = input.findtexfile(texmf.instance, filename)
    if fullname and (fullname ~= "") then
        return input.opentexfile(texmf.instance, fullname)
    else
        return nil
    end
end

function input.logmode()
    return (os.getenv("MTX.LOG.MODE") or os.getenv("MTX_LOG_MODE") or "tex"):lower()
end

-- this is a prelude to engine/progname specific configuration files
-- in which case we can omit files meant for other programs and
-- packages

--- ctx

-- maybe texinputs + font paths
-- maybe positive selection tex/context fonts/tfm|afm|vf|opentype|type1|map|enc

input.validators            = { }
input.validators.visibility = { }

function input.validators.visibility.default(path, name)
    return true
end

function input.validators.visibility.context(path, name)
    path = path[1] or path -- some day a loop
    return not (
        path:find("latex")    or
--      path:find("doc")      or
        path:find("tex4ht")   or
        path:find("source")   or
--      path:find("config")   or
--      path:find("metafont") or
        path:find("lists$")   or
        name:find("%.tpm$")   or
        name:find("%.bak$")
    )
end

-- todo: describe which functions are public (maybe input.private. ... )

-- beware: i need to check where we still need a / on windows:

function input.clean_path(str)
--~     return (((str:gsub("\\","/")):gsub("^!+","")):gsub("//+","//"))
    if str then
        return ((str:gsub("\\","/")):gsub("^!+",""))
    else
        return nil
    end
end

function input.do_with_path(name,func)
    for _, v in pairs(input.expanded_path_list(instance,name)) do
        func("^"..input.clean_path(v))
    end
end

function input.do_with_var(name,func)
    func(input.aux.expanded_var(name))
end

function input.with_files(instance,pattern,handle)
    for _, hash in ipairs(instance.hashes) do
        local blobpath = hash.tag
        local blobtype = hash.type
        if blobpath then
            local files = instance.files[blobpath]
            if files then
                for k,v in pairs(files) do
                    if k:find("^remap:") then
                        k = files[k]
                        v = files[k] -- chained
                    end
                    if k:find(pattern) then
                        if type(v) == "string" then
                            handle(blobtype,blobpath,v,k)
                        else
                            for _,vv in pairs(v) do
                                handle(blobtype,blobpath,vv,k)
                            end
                        end
                    end
                end
            end
        end
    end
end

--~ function input.update_script(oldname,newname) -- oldname -> own.name, not per se a suffix
--~     newname = file.addsuffix(newname,"lua")
--~     local newscript = input.clean_path(input.find_file(instance, newname))
--~     local oldscript = input.clean_path(oldname)
--~     input.report("old script", oldscript)
--~     input.report("new script", newscript)
--~     if oldscript ~= newscript and (oldscript:find(file.removesuffix(newname).."$") or oldscript:find(newname.."$")) then
--~         local newdata = io.loaddata(newscript)
--~         if newdata then
--~             input.report("old script content replaced by new content")
--~             io.savedata(oldscript,newdata)
--~         end
--~     end
--~ end

function input.update_script(instance,oldname,newname) -- oldname -> own.name, not per se a suffix
    local scriptpath = "scripts/context/lua"
    newname = file.addsuffix(newname,"lua")
    local oldscript = input.clean_path(oldname)
    input.report("to be replaced old script", oldscript)
    local newscripts = input.find_files(instance, newname) or { }
    if #newscripts == 0 then
        input.report("unable to locate new script")
    else
        for _, newscript in ipairs(newscripts) do
            newscript = input.clean_path(newscript)
            input.report("checking new script", newscript)
            if oldscript == newscript then
                input.report("old and new script are the same")
            elseif not newscript:find(scriptpath) then
                input.report("new script should come from",scriptpath)
            elseif not (oldscript:find(file.removesuffix(newname).."$") or oldscript:find(newname.."$")) then
                input.report("invalid new script name")
            else
                local newdata = io.loaddata(newscript)
                if newdata then
                    input.report("old script content replaced by new content")
                    io.savedata(oldscript,newdata)
                    break
                else
                    input.report("unable to load new script")
                end
            end
        end
    end
end


--~ print(table.serialize(input.aux.splitpathexpr("/usr/share/texmf-{texlive,tetex}", {})))

-- command line resolver:

--~ print(input.resolve("abc env:tmp file:cont-en.tex path:cont-en.tex full:cont-en.tex rel:zapf/one/p-chars.tex"))

do

    local resolvers = { }

    resolvers.environment = function(instance,str)
        return input.clean_path(os.getenv(str) or os.getenv(str:upper()) or os.getenv(str:lower()) or "")
    end
    resolvers.relative = function(instance,str,n)
        if io.exists(str) then
            -- nothing
        elseif io.exists("./" .. str) then
            str = "./" .. str
        else
            local p = "../"
            for i=1,n or 2 do
                if io.exists(p .. str) then
                    str = p .. str
                    break
                else
                    p = p .. "../"
                end
            end
        end
        return input.clean_path(str)
    end
    resolvers.locate = function(instance,str)
        local fullname = input.find_given_file(instance,str) or ""
        return input.clean_path((fullname ~= "" and fullname) or str)
    end
    resolvers.filename = function(instance,str)
        local fullname = input.find_given_file(instance,str) or ""
        return input.clean_path(file.basename((fullname ~= "" and fullname) or str))
    end
    resolvers.pathname = function(instance,str)
        local fullname = input.find_given_file(instance,str) or ""
        return input.clean_path(file.dirname((fullname ~= "" and fullname) or str))
    end

    resolvers.env  = resolvers.environment
    resolvers.rel  = resolvers.relative
    resolvers.loc  = resolvers.locate
    resolvers.kpse = resolvers.locate
    resolvers.full = resolvers.locate
    resolvers.file = resolvers.filename
    resolvers.path = resolvers.pathname

    local function resolve(instance,str)
        if type(str) == "table" then
            for k, v in pairs(str) do
                str[k] = resolve(instance,v) or v
            end
        elseif str and str ~= "" then
            str = str:gsub("([a-z]+):([^ ]+)", function(method,target)
                if resolvers[method] then
                    return resolvers[method](instance,target)
                else
                    return method .. ":" .. target
                end
            end)
        end
        return str
    end

    input.resolve = resolve

end


if not modules then modules = { } end modules ['luat-tmp'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.tex",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

--[[ldx--
<p>This module deals with caching data. It sets up the paths and
implements loaders and savers for tables. Best is to set the
following variable. When not set, the usual paths will be
checked. Personally I prefer the (users) temporary path.</p>

</code>
TEXMFCACHE=$TMP;$TEMP;$TMPDIR;$TEMPDIR;$HOME;$TEXMFVAR;$VARTEXMF;.
</code>

<p>Currently we do no locking when we write files. This is no real
problem because most caching involves fonts and the chance of them
being written at the same time is small. We also need to extend
luatools with a recache feature.</p>
--ldx]]--

caches = caches or { }
dir    = dir    or { }
texmf  = texmf  or { }

caches.path   = caches.path or nil
caches.base   = caches.base or "luatex-cache"
caches.more   = caches.more or "context"
caches.direct = false -- true is faster but may need huge amounts of memory
caches.trace  = false
caches.tree   = false
caches.paths  = caches.paths or nil
caches.force  = false

input.usecache = not toboolean(os.getenv("TEXMFSHARECACHE") or "false",true) -- true

function caches.temp(instance)
    local function checkpath(cachepath)
        if not cachepath or cachepath == "" then
            return nil
        elseif lfs.attributes(cachepath,"mode") == "directory" then -- lfs.isdir(cachepath) then
            return cachepath
        elseif caches.force or io.ask(string.format("Should I create the cache path %s?",cachepath), "no", { "yes", "no" }) == "yes" then
            dir.mkdirs(cachepath)
            return (lfs.attributes(cachepath,"mode") == "directory") and cachepath
        else
            return nil
        end
    end
    local cachepath = input.expanded_path_list(instance,"TEXMFCACHE")
    cachepath = cachepath and #cachepath > 0 and checkpath(cachepath[1])
    if not cachepath then
        cachepath = os.getenv("TEXMFCACHE") or os.getenv("HOME") or os.getenv("HOMEPATH") or os.getenv("TMP") or os.getenv("TEMP") or os.getenv("TMPDIR") or nil
        cachepath = checkpath(cachepath)
    end
    if not cachepath then
        print("\nfatal error: there is no valid cache path defined\n")
        os.exit()
    elseif lfs.attributes(cachepath,"mode") ~= "directory" then
        print(string.format("\nfatal error: cache path %s is not a directory\n",cachepath))
        os.exit()
    end
    function caches.temp(instance)
        return cachepath
    end
    return cachepath
end

function caches.configpath(instance)
    return table.concat(instance.cnffiles,";")
end

function caches.hashed(tree)
    return md5.hex((tree:lower()):gsub("[\\\/]+","/"))
end

function caches.treehash(instance)
    local tree = caches.configpath(instance)
    if not tree or tree == "" then
        return false
    else
        return caches.hashed(tree)
    end
end

function caches.setpath(instance,...)
    if not caches.path then
        if not caches.path then
            caches.path = caches.temp(instance)
        end
        caches.path = input.clean_path(caches.path) -- to be sure
        if lfs then
            caches.tree = caches.tree or caches.treehash(instance)
            if caches.tree then
                caches.path = dir.mkdirs(caches.path,caches.base,caches.more,caches.tree)
            else
                caches.path = dir.mkdirs(caches.path,caches.base,caches.more)
            end
        end
    end
    if not caches.path then
        caches.path = '.'
    end
    caches.path = input.clean_path(caches.path)
    if lfs and not table.is_empty({...}) then
        local pth = dir.mkdirs(caches.path,...)
        return pth
    end
    caches.path = dir.expand_name(caches.path)
    return caches.path
end

function caches.definepath(instance,category,subcategory)
    return function()
        return caches.setpath(instance,category,subcategory)
    end
end

function caches.setluanames(path,name)
    return path .. "/" .. name .. ".tma", path .. "/" .. name .. ".tmc"
end

function caches.loaddata(path,name)
    local tmaname, tmcname = caches.setluanames(path,name)
    local loader = loadfile(tmcname) or loadfile(tmaname)
    if loader then
        return loader()
    else
        return false
    end
end

function caches.is_writable(filepath,filename)
    local tmaname, tmcname = caches.setluanames(filepath,filename)
    return file.is_writable(tmaname)
end

function caches.savedata(filepath,filename,data,raw) -- raw needed for file cache
    local tmaname, tmcname = caches.setluanames(filepath,filename)
    local reduce, simplify = true, true
    if raw then
        reduce, simplify = false, false
    end
    if caches.direct then
        file.savedata(tmaname, table.serialize(data,'return',true,true))
    else
        table.tofile (tmaname,                 data,'return',true,true) -- maybe not the last true
    end
    utils.lua.compile(tmaname, tmcname)
end

-- here we use the cache for format loading (texconfig.[formatname|jobname])

--~ if tex and texconfig and texconfig.formatname and texconfig.formatname == "" then
if tex and texconfig and (not texconfig.formatname or texconfig.formatname == "") and texmf.instance then
    if not texconfig.luaname then texconfig.luaname = "cont-en.lua" end -- or luc
    texconfig.formatname = caches.setpath(texmf.instance,"formats") .. "/" .. texconfig.luaname:gsub("%.lu.$",".fmt")
end

--[[ldx--
<p>Once we found ourselves defining similar cache constructs
several times, containers were introduced. Containers are used
to collect tables in memory and reuse them when possible based
on (unique) hashes (to be provided by the calling function).</p>

<p>Caching to disk is disabled by default. Version numbers are
stored in the saved table which makes it possible to change the
table structures without bothering about the disk cache.</p>

<p>Examples of usage can be found in the font related code.</p>
--ldx]]--

containers       = { }
containers.trace = false

do -- local report

    local function report(container,tag,name)
        if caches.trace or containers.trace or container.trace then
            logs.report(string.format("%s cache",container.subcategory),string.format("%s: %s",tag,name or 'invalid'))
        end
    end

    local allocated = { }

    -- tracing

    function containers.define(category, subcategory, version, enabled)
        return function()
            if category and subcategory then
                local c = allocated[category]
                if not c then
                    c  = { }
                    allocated[category] = c
                end
                local s = c[subcategory]
                if not s then
                    s = {
                        category = category,
                        subcategory = subcategory,
                        storage = { },
                        enabled = enabled,
                        version = version or 1.000,
                        trace = false,
                        path = caches.setpath(texmf.instance,category,subcategory),
                    }
                    c[subcategory] = s
                end
                return s
            else
                return nil
            end
        end
    end

    function containers.is_usable(container, name)
        return container.enabled and caches.is_writable(container.path, name)
    end

    function containers.is_valid(container, name)
        if name and name ~= "" then
            local storage = container.storage[name]
            return storage and not table.is_empty(storage) and storage.cache_version == container.version
        else
            return false
        end
    end

    function containers.read(container,name)
        if container.enabled and not container.storage[name] then
            container.storage[name] = caches.loaddata(container.path,name)
            if containers.is_valid(container,name) then
                report(container,"loaded",name)
            else
                container.storage[name] = nil
            end
        end
        if container.storage[name] then
            report(container,"reusing",name)
        end
        return container.storage[name]
    end

    function containers.write(container, name, data)
        if data then
            data.cache_version = container.version
            if container.enabled then
                local unique, shared = data.unique, data.shared
                data.unique, data.shared = nil, nil
                caches.savedata(container.path, name, data)
                report(container,"saved",name)
                data.unique, data.shared = unique, shared
            end
            report(container,"stored",name)
            container.storage[name] = data
        end
        return data
    end

    function containers.content(container,name)
        return container.storage[name]
    end

end

-- since we want to use the cache instead of the tree, we will now
-- reimplement the saver.

local save_data = input.aux.save_data

input.cachepath = nil

function input.aux.save_data(instance, dataname, check)
    input.cachepath = input.cachepath or caches.definepath(instance,"trees")
    save_data(instance, dataname, check, function(cachename,dataname)
        if input.usecache then
            return file.join(input.cachepath(),caches.hashed(cachename))
        else
            return file.join(cachename,dataname)
        end
    end)
end

local load_data = input.aux.load_data

function input.aux.load_data(instance,pathname,dataname,filename)
    input.cachepath = input.cachepath or caches.definepath(instance,"trees")
    load_data(instance,pathname,dataname,filename,function(dataname,filename)
        if input.usecache then
            return file.join(input.cachepath(),caches.hashed(pathname))
        else
            if not filename or (filename == "") then
                filename = dataname
            end
            return file.join(pathname,filename)
        end
    end)
end

-- we will make a better format, maybe something xml or just text or lua

input.automounted = input.automounted or { }

function input.automount(instance,usecache)
    local mountpaths = input.simplified_list(input.expansion(instance,'TEXMFMOUNT'))
    if table.is_empty(mountpaths) and usecache then
        mountpaths = { caches.setpath(instance,"mount") }
    end
    if not table.is_empty(mountpaths) then
        input.starttiming(instance)
        for k, root in pairs(mountpaths) do
            local f = io.open(root.."/url.tmi")
            if f then
                for line in f:lines() do
                    if line then
                        if line:find("^[%%#%-]") then -- or %W
                            -- skip
                        elseif line:find("^zip://") then
                            input.report("mounting",line)
                            table.insert(input.automounted,line)
                            input.usezipfile(instance,line)
                        end
                    end
                end
                f:close()
            end
        end
        input.stoptiming(instance)
    end
end

-- store info in format

input.storage            = { }
input.storage.data       = { }
input.storage.min        = 0 -- 500
input.storage.max        = input.storage.min - 1
input.storage.trace      = false -- true
input.storage.done       = 0
input.storage.evaluators = { }
-- (evaluate,message,names)

function input.storage.register(...)
    input.storage.data[#input.storage.data+1] = { ... }
end

function input.storage.evaluate(name)
    input.storage.evaluators[#input.storage.evaluators+1] = name
end

function input.storage.finalize() -- we can prepend the string with "evaluate:"
    for _, t in ipairs(input.storage.evaluators) do
        for i, v in pairs(t) do
            if type(v) == "string" then
                t[i] = loadstring(v)()
            elseif type(v) == "table" then
                for _, vv in pairs(v) do
                    if type(vv) == "string" then
                        t[i] = loadstring(vv)()
                    end
                end
            end
        end
    end
end

function input.storage.dump()
    for name, data in ipairs(input.storage.data) do
        local evaluate, message, original, target = data[1], data[2], data[3] ,data[4]
        local name, initialize, finalize, code = nil, "", "", ""
        for str in target:gmatch("([^%.]+)") do
            if name then
                name = name .. "." .. str
            else
                name = str
            end
            initialize = string.format("%s %s = %s or {} ", initialize, name, name)
        end
        if evaluate then
            finalize = "input.storage.evaluate(" .. name .. ")"
        end
        input.storage.max = input.storage.max + 1
        if input.storage.trace then
            logs.report('storage',string.format('saving %s in slot %s',message,input.storage.max))
            code =
                initialize ..
                string.format("logs.report('storage','restoring %s from slot %s') ",message,input.storage.max) ..
                table.serialize(original,name) ..
                finalize
        else
            code = initialize .. table.serialize(original,name) .. finalize
        end
        lua.bytecode[input.storage.max] = loadstring(code)
    end
end

if lua.bytecode then -- from 0 upwards
    local i = input.storage.min
    while lua.bytecode[i] do
        lua.bytecode[i]()
        lua.bytecode[i] = nil
        i = i + 1
    end
    input.storage.done = i
end


if not modules then modules = { } end modules ['luat-log'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.tex",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

--[[ldx--
<p>This is a prelude to a more extensive logging module. For the sake
of parsing log files, in addition to the standard logging we will
provide an <l n='xml'/> structured file. Actually, any logging that
is hooked into callbacks will be \XML\ by default.</p>
--ldx]]--

input = input or { }
logs  = logs  or { }

--[[ldx--
<p>This looks pretty ugly but we need to speed things up a bit.</p>
--ldx]]--

logs.levels = {
    ['error']   = 1,
    ['warning'] = 2,
    ['info']    = 3,
    ['debug']   = 4
}

logs.functions = {
    'error', 'warning', 'info', 'debug', 'report',
    'start', 'stop', 'push', 'pop'
}

logs.callbacks  = {
    'start_page_number',
    'stop_page_number',
    'report_output_pages',
    'report_output_log'
}

logs.xml = logs.xml or { }
logs.tex = logs.tex or { }

logs.level = 0

do
    local write_nl, write, format = texio.write_nl or print, texio.write or io.write, string.format

    if texlua then
        write_nl = print
        write    = io.write
    end

    function logs.xml.debug(category,str)
        if logs.level > 3 then write_nl(format("<d category='%s'>%s</d>",category,str)) end
    end
    function logs.xml.info(category,str)
        if logs.level > 2 then write_nl(format("<i category='%s'>%s</i>",category,str)) end
    end
    function logs.xml.warning(category,str)
        if logs.level > 1 then write_nl(format("<w category='%s'>%s</w>",category,str)) end
    end
    function logs.xml.error(category,str)
        if logs.level > 0 then write_nl(format("<e category='%s'>%s</e>",category,str)) end
    end
    function logs.xml.report(category,str)
        write_nl(format("<r category='%s'>%s</r>",category,str))
    end

    function logs.xml.start() if logs.level > 0 then tw("<%s>" ) end end
    function logs.xml.stop () if logs.level > 0 then tw("</%s>") end end
    function logs.xml.push () if logs.level > 0 then tw("<!-- ") end end
    function logs.xml.pop  () if logs.level > 0 then tw(" -->" ) end end

    function logs.tex.debug(category,str)
        if logs.level > 3 then write_nl(format("debug >> %s: %s"  ,category,str)) end
    end
    function logs.tex.info(category,str)
        if logs.level > 2 then write_nl(format("info >> %s: %s"   ,category,str)) end
    end
    function logs.tex.warning(category,str)
        if logs.level > 1 then write_nl(format("warning >> %s: %s",category,str)) end
    end
    function logs.tex.error(category,str)
        if logs.level > 0 then write_nl(format("error >> %s: %s"  ,category,str)) end
    end
    function logs.tex.report(category,str)
        write_nl(format("report >> %s: %s"  ,category,str))
    end

    function logs.set_level(level)
        logs.level = logs.levels[level] or level
    end

    function logs.set_method(method)
        for _, v in pairs(logs.functions) do
            logs[v] = logs[method][v] or function() end
        end
        if callback and input[method] then
            for _, cb in pairs(logs.callbacks) do
                callback.register(cb, input[method][cb])
            end
        end
    end

    function logs.xml.start_page_number()
        write_nl(format("<p real='%s' page='%s' sub='%s'", tex.count[0], tex.count[1], tex.count[2]))
    end

    function logs.xml.stop_page_number()
        write("/>")
        write_nl("")
    end

    function logs.xml.report_output_pages(p,b)
        write_nl(format("<v k='pages' v='%s'/>", p))
        write_nl(format("<v k='bytes' v='%s'/>", b))
        write_nl("")
    end

    function logs.xml.report_output_log()
    end

end

logs.set_level('error')
logs.set_method('tex')


if not modules then modules = { } end modules ['luat-sta'] = {
    version   = 1.001,
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

states          = states          or { }
states.data     = states.data     or { }
states.hash     = states.hash     or { }
states.tag      = states.tag      or ""
states.filename = states.filename or ""

function states.save(filename,tag)
    tag = tag or states.tag
    filename = file.addsuffix(filename or states.filename,'lus')
    io.savedata(filename,
        "-- generator : luat-sta.lua\n" ..
        "-- state tag : " .. tag .. "\n\n" ..
        table.serialize(states.data[tag or states.tag] or {},true)
    )
end

function states.load(filename,tag)
    states.filename = filename
    states.tag = tag or "whatever"
    states.filename = file.addsuffix(states.filename,'lus')
    states.data[states.tag], states.hash[states.tag] = (io.exists(filename) and dofile(filename)) or { }, { }
end

function states.set_by_tag(tag,key,value,default,persistent)
    local d, h = states.data[tag], states.hash[tag]
    if d then
        local dkey, hkey = key, key
        local pre, post = key:match("(.+)%.([^%.]+)$")
        if pre and post then
            for k in pre:gmatch("[^%.]+") do
                local dk = d[k]
                if not dk then
                    dk = { }
                    d[k] = dk
                end
                d = dk
            end
            dkey, hkey = post, key
        end
        if type(value) == nil then
            value = value or default
        elseif persistent then
            value = value or d[dkey] or default
        else
            value = value or default
        end
        d[dkey], h[hkey] = value, value
    end
end

function states.get_by_tag(tag,key,default)
    local h = states.hash[tag]
    if h and h[key] then
        return h[key]
    else
        local d = states.data[tag]
        if d then
            for k in key:gmatch("[^%.]+") do
                local dk = d[k]
                if dk then
                    d = dk
                else
                    return default
                end
            end
            return d or default
        end
    end
end

function states.set(key,value,default,persistent)
    states.set_by_tag(states.tag,key,value,default,persistent)
end

function states.get(key,default)
    return states.get_by_tag(states.tag,key,default)
end

--~ states.data.update = {
--~ 	["version"] = {
--~ 		["major"] = 0,
--~ 		["minor"] = 1,
--~ 	},
--~ 	["rsync"] = {
--~ 		["server"]     = "contextgarden.net",
--~ 		["module"]     = "minimals",
--~ 		["repository"] = "current",
--~ 		["flags"]      = "-rpztlv --stats",
--~ 	},
--~ 	["tasks"] = {
--~ 		["update"] = true,
--~ 		["make"]   = true,
--~         ["delete"] = false,
--~ 	},
--~ 	["platform"] = {
--~ 		["host"]  = true,
--~ 		["other"] = {
--~ 			["mswin"]     = false,
--~ 			["linux"]     = false,
--~ 			["linux-64"]  = false,
--~ 			["osx-intel"] = false,
--~ 			["osx-ppc"]   = false,
--~ 			["sun"]       = false,
--~ 		},
--~ 	},
--~ 	["context"] = {
--~ 		["available"] = {"current", "beta", "alpha", "experimental"},
--~ 		["selected"]  = "current",
--~ 	},
--~ 	["formats"] = {
--~ 		["cont-en"] = true,
--~ 		["cont-nl"] = true,
--~ 		["cont-de"] = false,
--~ 		["cont-cz"] = false,
--~ 		["cont-fr"] = false,
--~ 		["cont-ro"] = false,
--~ 	},
--~ 	["engine"] = {
--~ 		["pdftex"] = {
--~ 			["install"] = true,
--~ 			["formats"] = {
--~ 				["pdftex"] = true,
--~ 			},
--~ 		},
--~ 		["luatex"] = {
--~ 			["install"] = true,
--~ 			["formats"] = {
--~ 			},
--~ 		},
--~ 		["xetex"] = {
--~ 			["install"] = true,
--~ 			["formats"] = {
--~ 				["xetex"] = false,
--~ 			},
--~ 		},
--~ 		["metapost"] = {
--~ 			["install"] = true,
--~ 			["formats"] = {
--~ 				["mpost"] = true,
--~ 				["metafun"] = true,
--~ 			},
--~ 		},
--~ 	},
--~ 	["fonts"] = {
--~ 	},
--~ 	["doc"] = {
--~ 	},
--~ 	["modules"] = {
--~ 		["f-urwgaramond"] = false,
--~ 		["f-urwgothic"] = false,
--~ 		["t-bnf"] = false,
--~ 		["t-chromato"] = false,
--~ 		["t-cmscbf"] = false,
--~ 		["t-cmttbf"] = false,
--~ 		["t-construction-plan"] = false,
--~ 		["t-degrade"] = false,
--~ 		["t-french"] = false,
--~ 		["t-lettrine"] = false,
--~ 		["t-lilypond"] = false,
--~ 		["t-mathsets"] = false,
--~ 		["t-tikz"] = false,
--~ 		["t-typearea"] = false,
--~ 		["t-vim"] = false,
--~ 	},
--~ }


--~ states.save("teststate", "update")
--~ states.load("teststate", "update")

--~ print(states.get_by_tag("update","rsync.server","unknown"))
--~ states.set_by_tag("update","rsync.server","oeps")
--~ print(states.get_by_tag("update","rsync.server","unknown"))
--~ states.save("teststate", "update")
--~ states.load("teststate", "update")
--~ print(states.get_by_tag("update","rsync.server","unknown"))

-- end library merge

own = { }

own.libs = { -- todo: check which ones are really needed
    'l-string.lua',
    'l-lpeg.lua',
    'l-table.lua',
    'l-io.lua',
    'l-md5.lua',
    'l-number.lua',
    'l-set.lua',
    'l-os.lua',
    'l-file.lua',
    'l-dir.lua',
    'l-boolean.lua',
    'l-xml.lua',
--  'l-unicode.lua',
    'l-utils.lua',
--  'l-tex.lua',
    'luat-lib.lua',
    'luat-inp.lua',
--  'luat-zip.lua',
--  'luat-tex.lua',
--  'luat-kps.lua',
    'luat-tmp.lua',
    'luat-log.lua',
    'luat-sta.lua',
}

-- We need this hack till luatex is fixed.
--
-- for k,v in pairs(arg) do print(k,v) end

if arg and (arg[0] == 'luatex' or arg[0] == 'luatex.exe') and arg[1] == "--luaonly" then
    arg[-1]=arg[0] arg[0]=arg[2] for k=3,#arg do arg[k-2]=arg[k] end arg[#arg]=nil arg[#arg]=nil
end

-- End of hack.

own.name = (environment and environment.ownname) or arg[0] or 'luatools.lua'

own.path = string.match(own.name,"^(.+)[\\/].-$") or "."
own.list = { '.' }
if own.path ~= '.' then
    table.insert(own.list,own.path)
end
table.insert(own.list,own.path.."/../../../tex/context/base")
table.insert(own.list,own.path.."/mtx")
table.insert(own.list,own.path.."/../sources")

function locate_libs()
    for _, lib in pairs(own.libs) do
        for _, pth in pairs(own.list) do
            local filename = string.gsub(pth .. "/" .. lib,"\\","/")
            local codeblob = loadfile(filename)
            if codeblob then
                codeblob()
                own.list = { pth } -- speed up te search
                break
            end
        end
    end
end

if not input then
    locate_libs()
end

if not input then
    print("")
    print("Mtxrun is unable to start up due to lack of libraries. You may")
    print("try to run 'lua mtxrun.lua --selfmerge' in the path where this")
    print("script is located (normally under ..../scripts/context/lua) which")
    print("will make this script library independent.")
    os.exit()
end

instance            = input.reset()
input.verbose       = environment.argument("verbose") or false
input.banner        = 'MtxRun | '
utils.report        = input.report

instance.engine   = environment.argument("engine")   or 'luatex'
instance.progname = environment.argument("progname") or 'context'
instance.lsrmode  = environment.argument("lsr")      or false

-- use os.env or environment when available

--~ function input.check_environment(tree)
--~     input.report('')
--~     os.setenv('TMP', os.getenv('TMP') or os.getenv('TEMP') or os.getenv('TMPDIR') or os.getenv('HOME'))
--~     if os.platform == 'linux' then
--~         os.setenv('TEXOS', os.getenv('TEXOS') or 'texmf-linux')
--~     elseif os.platform == 'windows' then
--~         os.setenv('TEXOS', os.getenv('TEXOS') or 'texmf-windows')
--~     elseif os.platform == 'macosx'  then
--~         os.setenv('TEXOS', os.getenv('TEXOS') or 'texmf-macosx')
--~     end
--~     os.setenv('TEXOS',   string.gsub(string.gsub(os.getenv('TEXOS'),"^[\\\/]*", ''),"[\\\/]*$", ''))
--~     os.setenv('TEXPATH', string.gsub(tree,"\/+$",''))
--~     os.setenv('TEXMFOS', os.getenv('TEXPATH') .. "/" .. os.getenv('TEXOS'))
--~     input.report('')
--~     input.report("preset : TEXPATH => " .. os.getenv('TEXPATH'))
--~     input.report("preset : TEXOS   => " .. os.getenv('TEXOS'))
--~     input.report("preset : TEXMFOS => " .. os.getenv('TEXMFOS'))
--~     input.report("preset : TMP     => " .. os.getenv('TMP'))
--~     input.report('')
--~ end

function input.check_environment(tree)
    input.report('')
    os.setenv('TMP', os.getenv('TMP') or os.getenv('TEMP') or os.getenv('TMPDIR') or os.getenv('HOME'))
    os.setenv('TEXOS', os.getenv('TEXOS') or ("texmf-" .. os.currentplatform()))
    os.setenv('TEXPATH', (tree or "tex"):gsub("\/+$",''))
    os.setenv('TEXMFOS', os.getenv('TEXPATH') .. "/" .. os.getenv('TEXOS'))
    input.report('')
    input.report("preset : TEXPATH => " .. os.getenv('TEXPATH'))
    input.report("preset : TEXOS   => " .. os.getenv('TEXOS'))
    input.report("preset : TEXMFOS => " .. os.getenv('TEXMFOS'))
    input.report("preset : TMP     => " .. os.getenv('TMP'))
    input.report('')
end

function input.load_environment(name) -- todo: key=value as well as lua
    local f = io.open(name)
    if f then
        for line in f:lines() do
            if line:find("^[%%%#]") then
                -- skip comment
            else
                local key, how, value = line:match("^(.-)%s*([<=>%?]+)%s*(.*)%s*$")
                if how then
                    value = value:gsub("%%(.-)%%", function(v) return os.getenv(v) or "" end)
                        if how == "=" or how == "<<" then
                            os.setenv(key,value)
                    elseif how == "?" or how == "??" then
                            os.setenv(key,os.getenv(key) or value)
                    elseif how == "<" or how == "+=" then
                        if os.getenv(key) then
                            os.setenv(key,os.getenv(key) .. io.fileseparator .. value)
                        else
                            os.setenv(key,value)
                        end
                    elseif how == ">" or how == "=+" then
                        if os.getenv(key) then
                            os.setenv(key,value .. io.pathseparator .. os.getenv(key))
                        else
                            os.setenv(key,value)
                        end
                    end
                end
            end
        end
        f:close()
    end
end

function input.load_tree(tree)
    if tree and tree ~= "" then
        local setuptex = 'setuptex.tmf'
        if lfs.attributes(tree, "mode") == "directory" then -- check if not nil
            setuptex = tree .. "/" .. setuptex
        else
            setuptex = tree
        end
        if io.exists(setuptex) then
            input.check_environment(tree)
            input.load_environment(setuptex)
        end
    end
end

-- md5 extensions

-- maybe md.md5 md.md5hex md.md5HEX

if not md5 then md5 = { } end

if not md5.sum then
    function md5.sum(k)
        return string.rep("x",16)
    end
end

function md5.hexsum(k)
    return (string.gsub(md5.sum(k), ".", function(c) return string.format("%02x", string.byte(c)) end))
end

function md5.HEXsum(k)
    return (string.gsub(md5.sum(k), ".", function(c) return string.format("%02X", string.byte(c)) end))
end

-- file extensions

file.needs_updating_threshold = 1

function file.needs_updating(oldname,newname) -- size modification access change
    local oldtime = lfs.attributes(oldname, modification)
    local newtime = lfs.attributes(newname, modification)
    if newtime >= oldtime then
        return false
    elseif oldtime - newtime < file.needs_updating_threshold then
        return false
    else
        return true
    end
end

function file.mdchecksum(name)
    if md5 then
        local data = io.loadall(name)
        if data then
            return md5.HEXsum(data)
        end
    end
    return nil
end

function file.loadchecksum(name)
    if md then
        local data = io.loadall(name .. ".md5")
        if data then
            return string.gsub(md5.HEXsum(data),"%s$","")
        end
    end
    return nil
end

function file.savechecksum(name, checksum)
    if not checksum then checksum = file.mdchecksum(name) end
    if checksum then
        local f = io.open(name .. ".md5","w")
        if f then
            f:write(checksum)
            f:close()
            return checksum
        end
    end
    return nil
end

os.arch = os.arch or function()
    return os.resultof("uname -m") or "linux"
end

function os.currentplatform(name, default)
    local name = os.name or os.platform or name -- os.name is built in, os.platform is mine
    if name then
        if name == "windows" or name == "mswin" or name == "win32" or name == "msdos" then
            return "mswin"
        elseif name == "linux" then
            local architecture = os.arch()
            if architecture:find("x86_64") then
                return "linux-64"
            else
                return "linux"
            end
        elseif name == "macosx" then
            local architecture = os.arch()
            if architecture:find("i386") then
                return "osx-intel"
            else
                return "osx-ppc"
            end
        elseif name == "freebsd" then
            return "freebsd"
        end
    end
    return default or name
end

-- it starts here

input.runners              = { }
input.runners.applications = { }

input.runners.applications.lua = "luatex --luaonly"
input.runners.applications.pl  = "perl"
input.runners.applications.py  = "python"
input.runners.applications.rb  = "ruby"

input.runners.suffixes = {
    'rb', 'lua', 'py', 'pl'
}

input.runners.registered = {
    texexec      = { 'texexec.rb',      true  },
    texutil      = { 'texutil.rb',      true  },
    texfont      = { 'texfont.pl',      true  },
    texshow      = { 'texshow.pl',      false },

    makempy      = { 'makempy.pl',      true  },
    mptopdf      = { 'mptopdf.pl',      true  },
    pstopdf      = { 'pstopdf.rb',      true  },

    examplex     = { 'examplex.rb',     false },
    concheck     = { 'concheck.rb',     false },

    runtools     = { 'runtools.rb',     true  },
    textools     = { 'textools.rb',     true  },
    tmftools     = { 'tmftools.rb',     true  },
    ctxtools     = { 'ctxtools.rb',     true  },
    rlxtools     = { 'rlxtools.rb',     true  },
    pdftools     = { 'pdftools.rb',     true  },
    mpstools     = { 'mpstools.rb',     true  },
    exatools     = { 'exatools.rb',     true  },
    xmltools     = { 'xmltools.rb',     true  },
    luatools     = { 'luatools.lua',    true  },
    mtxtools     = { 'mtxtools.rb',     true  },

    pdftrimwhite = { 'pdftrimwhite.pl', false }
}

if not messages then messages = { } end

messages.help = [[
--script              run an mtx script
--execute             run a script or program
--resolve             resolve prefixed arguments
--ctxlua              run internally (using preloaded libs)
--locate              locate given filename

--autotree            use texmf tree cf. env 'texmfstart_tree' or 'texmfstarttree'
--tree=pathtotree     use given texmf tree (default file: 'setuptex.tmf')
--environment=name    use given (tmf) environment file
--path=runpath        go to given path before execution
--ifchanged=filename  only execute when given file has changed (md checksum)
--iftouched=old,new   only execute when given file has changed (time stamp)

--make                create stubs for (context related) scripts
--remove              remove stubs (context related) scripts
--stubpath=binpath    paths where stubs wil be written
--windows             create windows (mswin) stubs
--unix                create unix (linux) stubs

--verbose             give a bit more info
--engine=str          target engine
--progname=str        format or backend

--edit                launch editor with found file
--launch (--all)      launch files (assume os support)

--intern              run script using built in libraries
]]

function input.runners.my_prepare_a(instance)
    input.resetconfig(instance)
    input.identify_cnf(instance)
    input.load_lua(instance)
    input.expand_variables(instance)
    input.load_cnf(instance)
    input.expand_variables(instance)
end

function input.runners.my_prepare_b(instance)
    input.runners.my_prepare_a(instance)
    input.load_hash(instance)
    input.automount(instance)
end

function input.runners.prepare(instance)
    local checkname = environment.argument("ifchanged")
    if checkname and checkname ~= "" then
        local oldchecksum = file.loadchecksum(checkname)
        local newchecksum = file.checksum(checkname)
        if oldchecksum == newchecksum then
            report("file '" .. checkname .. "' is unchanged")
            return "skip"
        else
            report("file '" .. checkname .. "' is changed, processing started")
        end
        file.savechecksum(checkname)
    end
    local oldname, newname = string.split(environment.argument("iftouched") or "", ",")
    if oldname and newname and oldname ~= "" and newname ~= "" then
        if not file.needs_updating(oldname,newname) then
            report("file '" .. oldname .. "' and '" .. newname .. "'have same age")
            return "skip"
        else
            report("file '" .. newname .. "' is older than '" .. oldname .. "'")
        end
    end
    local tree = environment.argument('tree') or ""
    if environment.argument('autotree') then
        tree = os.getenv('TEXMFSTART_TREE') or os.getenv('TEXMFSTARTTREE') or tree
    end
    if tree and tree ~= "" then
        input.load_tree(tree)
    end
    local env = environment.argument('environment') or ""
    if env and env ~= "" then
        for _,e in pairs(string.split(env)) do
            -- maybe force suffix when not given
            input.load_tree(e)
        end
    end
    local runpath = environment.argument("path")
    if runpath and not dir.chdir(runpath) then
        input.report("unable to change to path '" .. runpath .. "'")
        return "error"
    end
    return "run"
end

function input.runners.execute_script(instance,fullname,internal)
    if fullname and fullname ~= "" then
        local state = input.runners.prepare(instance)
        if state == 'error' then
            return false
        elseif state == 'skip' then
            return true
        elseif state == "run" then
            instance.progname = environment.argument("progname") or instance.progname
            instance.format   = environment.argument("format")   or instance.format
            local path, name, suffix, result = file.dirname(fullname), file.basename(fullname), file.extname(fullname), ""
            if path ~= "" then
                result = fullname
            elseif name then
                name = name:gsub("^int[%a]*:",function()
                    internal = true
                    return ""
                end )
                name = name:gsub("^script:","")
                if suffix == "" and input.runners.registered[name] and input.runners.registered[name][1] then
                    name = input.runners.registered[name][1]
                    suffix = file.extname(name)
                end
                if suffix == "" then
                    -- loop over known suffixes
                    for _,s in pairs(input.runners.suffixes) do
                        result = input.find_file(instance, name .. "." .. s, 'texmfscripts')
                        if result ~= "" then
                            break
                        end
                    end
                elseif input.runners.applications[suffix] then
                    result = input.find_file(instance, name, 'texmfscripts')
                else
                    -- maybe look on path
                    result = input.find_file(instance, name, 'other text files')
                end
            end
            if result and result ~= "" then
                if internal then
                    local before, after = environment.split_arguments(fullname)
                    arg = { } for _,v in pairs(after) do arg[#arg+1] = v end
                    dofile(result)
                else
                    local binary = input.runners.applications[file.extname(result)]
                    if binary and binary ~= "" then
                        result = binary .. " " .. result
                    end
                    local before, after = environment.split_arguments(fullname)
                    local command = result .. " " .. environment.reconstruct_commandline(after)
                    input.report("")
                    input.report("executing: " .. command)
                    input.report("\n \n")
                    io.flush()
                    local code = os.exec(command) -- maybe spawn
                    return code == 0
                end
            end
        end
    end
    return false
end

function input.runners.execute_program(instance,fullname)
    if fullname and fullname ~= "" then
        local state = input.runners.prepare(instance)
        if state == 'error' then
            return false
        elseif state == 'skip' then
            return true
        elseif state == "run" then
            local before, after = environment.split_arguments(fullname)
            environment.initialize_arguments(after)
            fullname = fullname:gsub("^bin:","")
            local command = fullname .. " " .. environment.reconstruct_commandline(after)
            input.report("")
            input.report("executing: " .. command)
            input.report("\n \n")
            io.flush()
            local code = os.exec(command) -- (fullname,unpack(after)) does not work / maybe spawn
            return code == 0
        end
    end
    return false
end

function input.runners.handle_stubs(instance,create)
    local stubpath = environment.argument('stubpath') or '.' -- 'auto' no longer supported
    local windows  = environment.argument('windows') or environment.argument('mswin') or false
    local unix     = environment.argument('unix') or environment.argument('linux') or false
    if not windows and not unix then
        if environment.platform == "unix" then
            unix = true
        else
            windows = true
        end
    end
    for _,v in pairs(input.runners.registered) do
        local name, doit = v[1], v[2]
        if doit then
            local base = string.gsub(file.basename(name), "%.(.-)$", "")
            if create then
                -- direct local command = input.runners.applications[file.extname(name)] .. " " .. name
                local command = "luatex --luaonly mtxrun.lua " .. name
                if windows then
                    io.savedata(base..".bat", {"@echo off", command.." %*"}, "\013\010")
                        input.report("windows stub for '" .. base .. "' created")
                end
                if unix then
                    io.savedata(base, {"#!/bin/sh", command..' "$@"'}, "\010")
                    input.report("unix stub for '" .. base .. "' created")
                end
            else
                if windows and (os.remove(base..'.bat') or os.remove(base..'.cmd')) then
                    input.report("windows stub for '" .. base .. "' removed")
                end
                if unix and (os.remove(base) or os.remove(base..'.sh')) then
                    input.report("unix stub for '" .. base .. "' removed")
                end
            end
        end
    end
end

function input.runners.resolve_string(instance,filename)
    if filename and filename ~= "" then
        input.runners.report_location(instance,input.resolve(instance,filename))
    end
end

function input.runners.locate_file(instance,filename)
    if filename and filename ~= "" then
        input.runners.report_location(instance,input.find_given_file(instance,filename))
    end
end

function input.runners.locate_platform(instance)
    input.runners.report_location(instance,os.currentplatform())
end

function input.runners.report_location(instance,result)
    if input.verbose then
        input.report("")
        if result and result ~= "" then
            input.report(result)
        else
            input.report("not found")
        end
    else
        io.write(result)
    end
end

function input.runners.edit_script(instance,filename)
    local editor = os.getenv("MTXRUN_EDITOR") or os.getenv("TEXMFSTART_EDITOR") or os.getenv("EDITOR") or 'scite'
    local rest = input.resolve(instance,filename)
    if rest ~= "" then
        os.launch(editor .. " " .. rest)
    end
end

function input.runners.save_script_session(filename, list)
    local t = { }
    for _, key in ipairs(list) do
        t[key] = environment.arguments[key]
    end
    io.savedata(filename,table.serialize(t,true))
end

function input.runners.load_script_session(filename)
    if lfs.isfile(filename) then
        local t = io.loaddata(filename)
        if t then
            t = loadstring(t)
            if t then t = t() end
            for key, value in pairs(t) do
                environment.arguments[key] = value
            end
        end
    end
end

input.runners.launchers = {
    windows = { },
    unix = { }
}

function input.launch(str)
    -- maybe we also need to test on mtxrun.launcher.suffix environment
    -- variable or on windows consult the assoc and ftype vars and such
    local launchers = input.runners.launchers[os.platform] if launchers then
        local suffix = file.extname(str) if suffix then
            local runner = launchers[suffix] if runner then
                str = runner .. " " .. str
            end
        end
    end
    os.launch(str)
end

function input.runners.launch_file(instance,filename)
    instance.allresults = true
    input.verbose = true
    local pattern = environment.arguments["pattern"]
    if not pattern or pattern == "" then
        pattern = filename
    end
    if not pattern or pattern == "" then
        input.report("provide name or --pattern=")
    else
        local t = input.find_files(instance,pattern)
    --  local t = input.aux.find_file(instance,"*/" .. pattern,true)
        if t and #t > 0 then
            if environment.arguments["all"] then
                for _, v in pairs(t) do
                    input.report("launching", v)
                    input.launch(v)
                end
            else
                input.report("launching", t[1])
                input.launch(t[1])
            end
        else
            input.report("no match for", pattern)
        end
    end
end

function input.runners.execute_ctx_script(instance,filename,arguments)
    local function found(name)
        local path = file.dirname(name)
        if path and path ~= "" then
            return false
        else
            local fullname = own and own.path and file.join(own.path,name)
            return io.exists(fullname) and fullname
        end
    end
    local suffix = ""
    if not filename:find("%.lua$") then suffix = ".lua" end
    local fullname = filename
    -- just <filename>
    fullname = filename .. suffix
    fullname = input.find_file(instance,fullname)
    -- mtx-<filename>
    if not fullname or fullname == "" then
        fullname = "mtx-" .. filename .. suffix
        fullname = found(fullname) or input.find_file(instance,fullname)
    end
    -- mtx-<filename>s
    if not fullname or fullname == "" then
        fullname = "mtx-" .. filename .. "s" .. suffix
        fullname = found(fullname) or input.find_file(instance,fullname)
    end
    -- mtx-<filename minus trailing s>
    if not fullname or fullname == "" then
        fullname = "mtx-" .. filename:gsub("s$","") .. suffix
        fullname = found(fullname) or input.find_file(instance,fullname)
    end
    -- that should do it
    if fullname and fullname ~= "" then
        local state = input.runners.prepare(instance)
        if state == 'error' then
            return false
        elseif state == 'skip' then
            return true
        elseif state == "run" then
            -- load and save ... kind of undocumented
            arg = { } for _,v in pairs(arguments) do arg[#arg+1] = v end
            environment.initialize_arguments(arg)
            local loadname = environment.arguments['load']
            if loadname then
                if type(loadname) ~= "string" then loadname = file.basename(fullname) end
                loadname = file.replacesuffix(loadname,"cfg")
                input.runners.load_script_session(loadname)
            end
            filename = environment.files[1]
            if input.verbose then
                input.report("using script: " .. fullname)
            end
            dofile(fullname)
            local savename = environment.arguments['save']
            if savename and input.runners.save_list and not table.is_empty(input.runners.save_list or { }) then
                if type(savename) ~= "string" then savename = file.basename(fullname) end
                savename = file.replacesuffix(savename,"cfg")
                input.runners.save_script_session(savename, input.runners.save_list)
            end
            return true
        end
    else
        input.verbose = true
        input.report("unknown script: " .. filename)
        return false
    end
end

input.report(banner,"\n")

function input.help(banner,message)
    if not input.verbose then
        input.verbose = true
        input.report(banner,"\n")
    end
    input.reportlines(message)
end

-- this is a bit dirty ... first we store the first filename and next we
-- split the arguments so that we only see the ones meant for this script
-- ... later we will use the second half

local filename = environment.files[1] or ""
local ok      = true

local before, after = environment.split_arguments(filename)

input.runners.my_prepare_b(instance)
before = input.resolve(instance,before) -- experimental here
after = input.resolve(instance,after) -- experimental here

environment.initialize_arguments(before)

if environment.argument("selfmerge") then
    -- embed used libraries
    utils.merger.selfmerge(own.name,own.libs,own.list)
elseif environment.argument("selfclean") then
    -- remove embedded libraries
    utils.merger.selfclean(own.name)
elseif environment.argument("selfupdate") then
    input.verbose = true
    input.update_script(instance,own.name,"mtxrun")
elseif environment.argument("ctxlua") or environment.argument("internal") then
    -- run a script by loading it (using libs)
    ok = input.runners.execute_script(instance,filename,true)
elseif environment.argument("script") then
    -- run a script by loading it (using libs), pass args
    ok = input.runners.execute_ctx_script(instance,filename,after)
elseif environment.argument("execute") then
    -- execute script
    ok = input.runners.execute_script(instance,filename)
elseif environment.argument("direct") then
    -- equals bin:
    ok = input.runners.execute_program(instance,filename)
elseif environment.argument("edit") then
    -- edit file
    input.runners.edit_script(instance,filename)
elseif environment.argument("launch") then
    input.runners.launch_file(instance,filename)
elseif environment.argument("make") then
    -- make stubs
    input.runners.handle_stubs(instance,true)
elseif environment.argument("remove") then
    -- remove stub
    input.runners.handle_stubs(instance,false)
elseif environment.argument("resolve") then
    -- resolve string
    input.runners.resolve_string(instance,filename)
elseif environment.argument("locate") then
    -- locate file
    input.runners.locate_file(instance,filename)
elseif environment.argument("platform")then
    -- locate platform
    input.runners.locate_platform(instance)
elseif environment.argument("help") or filename=='help' or filename == "" then
    input.help(banner,messages.help)
    -- execute script
    if filename:find("^bin:") then
        ok = input.runners.execute_program(instance,filename)
    else
        ok = input.runners.execute_script(instance,filename)
    end
end

--~ if input.verbose then
--~     input.report("")
--~     input.report(string.format("runtime: %0.3f seconds",os.runtime()))
--~ end

--~ if ok then
--~     input.report("exit code: 0") os.exit(0)
--~ else
--~     input.report("exit code: 1") os.exit(1)
--~ end

if environment.platform == "unix" then
    io.write("\n")
end
