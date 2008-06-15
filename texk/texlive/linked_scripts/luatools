#!/usr/bin/env texlua

-- one can make a stub:
--
-- #!/bin/sh
-- env LUATEXDIR=/....../texmf/scripts/context/lua luatex --luaonly=luatools.lua "$@"
-- filename : luatools.lua
-- comment  : companion to context.tex
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files
-- Although this script is part of the ConTeXt distribution it is
-- relatively indepent of ConTeXt.  The same is true for some of
-- the luat files. We may may make them even less dependent in
-- the future. As long as Luatex is under development the
-- interfaces and names of functions may change.

banner = "version 1.2.0 - 2006+ - PRAGMA ADE / CONTEXT"
texlua = true

-- For the sake of independence we optionally can merge the library
-- code here. It's too much code, but that does not harm. Much of the
-- library code is used elsewhere. We don't want dependencies on
-- Lua library paths simply because these scripts are located in the
-- texmf tree and not in some Lua path. Normally this merge is not
-- needed when texmfstart is used, or when the proper stub is used or
-- when (windows) suffix binding is active.

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


-- filename : l-url.lua
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-url'] = 1.001
if not url      then url      = { } end

-- from the spec (on the web):
--
--     foo://example.com:8042/over/there?name=ferret#nose
--     \_/   \______________/\_________/ \_________/ \__/
--      |           |            |            |        |
--   scheme     authority       path        query   fragment
--      |   _____________________|__
--     / \ /                        \
--     urn:example:animal:ferret:nose

do

    local function tochar(s)
        return string.char(tonumber(s,16))
    end

    local colon, qmark, hash, slash, percent, endofstring = lpeg.P(":"), lpeg.P("?"), lpeg.P("#"), lpeg.P("/"), lpeg.P("%"), lpeg.P(-1)

    local hexdigit  = lpeg.R("09","AF","af")
    local escaped   = percent * lpeg.C(hexdigit * hexdigit) / tochar

    local scheme    =                 lpeg.Cs((escaped+(1-colon-slash-qmark-hash))^0) * colon + lpeg.Cc("")
    local authority = slash * slash * lpeg.Cs((escaped+(1-      slash-qmark-hash))^0)         + lpeg.Cc("")
    local path      = slash *         lpeg.Cs((escaped+(1-            qmark-hash))^0)         + lpeg.Cc("")
    local query     = qmark         * lpeg.Cs((escaped+(1-                  hash))^0)         + lpeg.Cc("")
    local fragment  = hash          * lpeg.Cs((escaped+(1-           endofstring))^0)         + lpeg.Cc("")

    local parser = lpeg.Ct(scheme * authority * path * query * fragment)

    function url.split(str)
        return (type(str) == "string" and parser:match(str)) or str
    end

end

function url.hashed(str)
    local s = url.split(str)
    return {
        scheme = (s[1] ~= "" and s[1]) or "file",
        authority = s[2],
        path = s[3],
        query = s[4],
        fragment = s[5],
        original = str
    }
end

function url.filename(filename)
    local t = url.hashed(filename)
    return (t.scheme == "file" and t.path:gsub("^/([a-zA-Z])([:|])/)","%1:")) or filename
end

function url.query(str)
    if type(str) == "string" then
        local t = { }
        for k, v in str:gmatch("([^&=]*)=([^&=]*)") do
            t[k] = v
        end
        return t
    else
        return str
    end
end

--~ print(url.filename("file:///c:/oeps.txt"))
--~ print(url.filename("c:/oeps.txt"))
--~ print(url.filename("file:///oeps.txt"))
--~ print(url.filename("file:///etc/test.txt"))
--~ print(url.filename("/oeps.txt"))

--  from the spec on the web (sort of):
--~
--~ function test(str)
--~     print(table.serialize(url.hashed(str)))
--~ end
---~
--~ test("%56pass%20words")
--~ test("file:///c:/oeps.txt")
--~ test("file:///c|/oeps.txt")
--~ test("file:///etc/oeps.txt")
--~ test("file://./etc/oeps.txt")
--~ test("file:////etc/oeps.txt")
--~ test("ftp://ftp.is.co.za/rfc/rfc1808.txt")
--~ test("http://www.ietf.org/rfc/rfc2396.txt")
--~ test("ldap://[2001:db8::7]/c=GB?objectClass?one#what")
--~ test("mailto:John.Doe@example.com")
--~ test("news:comp.infosystems.www.servers.unix")
--~ test("tel:+1-816-555-1212")
--~ test("telnet://192.0.2.16:80/")
--~ test("urn:oasis:names:specification:docbook:dtd:xml:4.1.2")
--~ test("/etc/passwords")
--~ test("http://www.pragma-ade.com/spaced%20name")

--~ test("zip:///oeps/oeps.zip#bla/bla.tex")
--~ test("zip:///oeps/oeps.zip?bla/bla.tex")


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


-- filename : l-unicode.lua
-- comment  : split off from luat-inp
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['l-unicode'] = 1.001
if not unicode  then unicode  = { } end

if not garbagecollector then
    garbagecollector = {
        push = function() collectgarbage("stop")    end,
        pop  = function() collectgarbage("restart") end,
    }
end

-- 0  EF BB BF      UTF-8
-- 1  FF FE         UTF-16-little-endian
-- 2  FE FF         UTF-16-big-endian
-- 3  FF FE 00 00   UTF-32-little-endian
-- 4  00 00 FE FF   UTF-32-big-endian

unicode.utfname = {
    [0] = 'utf-8',
    [1] = 'utf-16-le',
    [2] = 'utf-16-be',
    [3] = 'utf-32-le',
    [4] = 'utf-32-be'
}

function unicode.utftype(f) -- \000 fails !
    local str = f:read(4)
    if not str then
        f:seek('set')
        return 0
    elseif str:find("^%z%z\254\255") then
        return 4
    elseif str:find("^\255\254%z%z") then
        return 3
    elseif str:find("^\254\255") then
        f:seek('set',2)
        return 2
    elseif str:find("^\255\254") then
        f:seek('set',2)
        return 1
    elseif str:find("^\239\187\191") then
        f:seek('set',3)
        return 0
    else
        f:seek('set')
        return 0
    end
end

function unicode.utf16_to_utf8(str, endian)
    garbagecollector.push()
    local result = { }
    local tc, uc = table.concat, unicode.utf8.char
    local tmp, n, m, p = { }, 0, 0, 0
    -- lf | cr | crlf / (cr:13, lf:10)
    local function doit()
        if n == 10 then
            if p ~= 13 then
                result[#result+1] = tc(tmp,"")
                tmp = { }
                p = 0
            end
        elseif n == 13 then
            result[#result+1] = tc(tmp,"")
            tmp = { }
            p = n
        else
            tmp[#tmp+1] = uc(n)
            p = 0
        end
    end
    for l,r in str:bytepairs() do
        if endian then
            n = l*256 + r
        else
            n = r*256 + l
        end
        if m > 0 then
            n = (m-0xD800)*0x400 + (n-0xDC00) + 0x10000
            m = 0
            doit()
        elseif n >= 0xD800 and n <= 0xDBFF then
            m = n
        else
            doit()
        end
    end
    if #tmp > 0 then
        result[#result+1] = tc(tmp,"")
    end
    garbagecollector.pop()
    return result
end

function unicode.utf32_to_utf8(str, endian)
    garbagecollector.push()
    local result = { }
    local tc, uc = table.concat, unicode.utf8.char
    local tmp, n, m, p = { }, 0, -1, 0
    -- lf | cr | crlf / (cr:13, lf:10)
    local function doit()
        if n == 10 then
            if p ~= 13 then
                result[#result+1] = tc(tmp,"")
                tmp = { }
                p = 0
            end
        elseif n == 13 then
            result[#result+1] = tc(tmp,"")
            tmp = { }
            p = n
        else
            tmp[#tmp+1] = uc(n)
            p = 0
        end
    end
    for a,b in str:bytepairs() do
        if a and b then
            if m < 0 then
                if endian then
                    m = a*256*256*256 + b*256*256
                else
                    m = b*256 + a
                end
            else
                if endian then
                    n = m + a*256 + b
                else
                    n = m + b*256*256*256 + a*256*256
                end
                m = -1
                doit()
            end
        else
            break
        end
    end
    if #tmp > 0 then
        result[#result+1] = tc(tmp,"")
    end
    garbagecollector.pop()
    return result
end


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


-- filename : luat-zip.lua
-- comment  : companion to luat-lib.tex
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['luat-zip'] = 1.001

if zip and input then
    zip.supported = true
else
    zip           = { }
    zip.supported = false
end

if not zip.supported then

    if not input then input = { } end -- will go away

    function zip.openarchive        (...) return nil end -- needed ?
    function zip.closenarchive      (...)            end -- needed ?
    function input.usezipfile       (...)            end -- needed ?

else

    -- zip:///oeps.zip?name=bla/bla.tex
    -- zip:///oeps.zip?tree=tex/texmf-local

    local function validzip(str)
        if not str:find("^zip://") then
            return "zip:///" .. str
        else
            return str
        end
    end

    zip.archives        = { }
    zip.registeredfiles = { }

    function zip.openarchive(instance,name)
        if not name or name == "" then
            return nil
        else
            local arch = zip.archives[name]
            if arch then
                return arch
            else
               local full = input.find_file(instance,name) or ""
               local arch = (full ~= "" and zip.open(full)) or false
               zip.archives[name] = arch
               return arch
            end
        end
    end

    function zip.closearchive(instance,name)
        if not name or name == "" and zip.archives[name] then
            zip.close(zip.archives[name])
            zip.archives[name] = nil
        end
    end

    -- zip:///texmf.zip?tree=/tex/texmf
    -- zip:///texmf.zip?tree=/tex/texmf-local
    -- zip:///texmf-mine.zip?tree=/tex/texmf-projects

    function input.locators.zip(instance,specification) -- where is this used? startup zips (untested)
        specification = input.splitmethod(specification)
        local zipfile = specification.path
        local zfile = zip.openarchive(instance,name) -- tricky, could be in to be initialized tree
        if zfile then
            input.logger('! zip locator', specification.original ..' found')
        else
            input.logger('? zip locator', specification.original ..' not found')
        end
    end

    function input.hashers.zip(instance,tag,name)
        input.report("loading zip file",name,"as",tag)
        input.usezipfile(instance,tag .."?tree=" .. name)
    end

    function input.concatinators.zip(tag,path,name)
        if not path or path == "" then
            return tag .. '?name=' .. name
        else
            return tag .. '?name=' .. path .. "/" .. name
        end
    end

    function input.is_readable.zip(name)
        return true
    end

    function input.finders.zip(instance,specification,filetype)
        specification = input.splitmethod(specification)
        if specification.path then
            local q = url.query(specification.query)
            if q.name then
                local zfile = zip.openarchive(instance,specification.path)
                if zfile then
                    input.logger('! zip finder',specification.path)
                    local dfile = zfile:open(q.name)
                    if dfile then
                        dfile = zfile:close()
                        input.logger('+ zip finder',q.name)
                        return specification.original
                    end
                else
                    input.logger('? zip finder',specification.path)
                end
            end
        end
        input.logger('- zip finder',filename)
        return unpack(input.finders.notfound)
    end

    function input.openers.zip(instance,specification)
        local zipspecification = input.splitmethod(specification)
        if zipspecification.path then
            local q = url.query(zipspecification.query)
            if q.name then
                local zfile = zip.openarchive(instance,zipspecification.path)
                if zfile then
                    input.logger('+ zip starter',zipspecification.path)
                    local dfile = zfile:open(q.name)
                    if dfile then
                        input.show_open(specification)
                        return input.openers.text_opener(specification,dfile,'zip')
                    end
                else
                    input.logger('- zip starter',zipspecification.path)
                end
            end
        end
        input.logger('- zip opener',filename)
        return unpack(input.openers.notfound)
    end

    function input.loaders.zip(instance,specification)
        specification = input.splitmethod(specification)
        if specification.path then
            local q = url.query(specification.query)
            if q.name then
                local zfile = zip.openarchive(instance,specification.path)
                if zfile then
                    input.logger('+ zip starter',specification.path)
                    local dfile = zfile:open(q.name)
                    if dfile then
                        input.show_load(filename)
                        input.logger('+ zip loader',filename)
                        local s = dfile:read("*all")
                        dfile:close()
                        return true, s, #s
                    end
                else
                    input.logger('- zip starter',specification.path)
                end
            end
        end
        input.logger('- zip loader',filename)
        return unpack(input.openers.notfound)
    end

    -- zip:///somefile.zip
    -- zip:///somefile.zip?tree=texmf-local -> mount

    function input.usezipfile(instance,zipname)
        zipname = validzip(zipname)
        input.logger('! zip use','file '..zipname)
        local specification = input.splitmethod(zipname)
        local zipfile = specification.path
        if zipfile and not zip.registeredfiles[zipname] then
            local tree = url.query(specification.query).tree or ""
            input.logger('! zip register','file '..zipname)
            local z = zip.openarchive(instance,zipfile)
            if z then
                input.logger("= zipfile","registering "..zipname)
                input.starttiming(instance)
                input.aux.prepend_hash(instance,'zip',zipname,zipfile)
                input.aux.extend_texmf_var(instance,zipname) -- resets hashes too
                zip.registeredfiles[zipname] = z
                instance.files[zipname] = input.aux.register_zip_file(z,tree or "")
                input.stoptiming(instance)
            else
                input.logger("? zipfile","unknown "..zipname)
            end
        else
            input.logger('! zip register','no file '..zipname)
        end
    end

    function input.aux.register_zip_file(z,tree)
        local files, filter = { }, ""
        if tree == "" then
            filter = "^(.+)/(.-)$"
        else
            filter = "^"..tree.."/(.+)/(.-)$"
        end
        input.logger('= zip filter',filter)
        local register, n = input.aux.register_file, 0
        for i in z:files() do
            local path, name = i.filename:match(filter)
            if path then
                if name and name ~= '' then
                    register(files, name, path)
                    n = n + 1
                else
                    -- directory
                end
            else
                register(files, i.filename, '')
                n = n + 1
            end
        end
        input.report('= zip entries',n)
        return files
    end

end


-- filename : luat-zip.lua
-- comment  : companion to luat-lib.tex
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

if not versions then versions = { } end versions['luat-tex'] = 1.001

-- special functions that deal with io

if texconfig and not texlua then

    input.level = input.level or 0

    if input.logmode() == 'xml' then
        function input.show_open(name)
            input.level = input.level + 1
            texio.write_nl("<f l='"..input.level.."' n='"..name.."'>")
        end
        function input.show_close(name)
            texio.write("</f> ")
            input.level = input.level - 1
        end
        function input.show_load(name)
            texio.write_nl("<f l='"..(input.level+1).."' n='"..name.."'/>") -- level?
        end
    else
        function input.show_open () end
        function input.show_close() end
        function input.show_load () end
    end

    function input.finders.generic(instance,tag,filename,filetype)
        local foundname = input.find_file(instance,filename,filetype)
        if foundname and foundname ~= "" then
            input.logger('+ ' .. tag .. ' finder',filename,'filetype')
            return foundname
        else
            input.logger('- ' .. tag .. ' finder',filename,'filetype')
            return unpack(input.finders.notfound)
        end
    end

    input.filters.dynamic_translator = nil
    input.filters.frozen_translator  = nil
    input.filters.utf_translator     = nil

    function input.openers.text_opener(filename,file_handle,tag)
        local u = unicode.utftype(file_handle)
        local t = { }
        if u > 0  then
            input.logger('+ ' .. tag .. ' opener (' .. unicode.utfname[u] .. ')',filename)
            local l
            if u > 2 then
                l = unicode.utf32_to_utf8(file_handle:read("*a"),u==4)
            else
                l = unicode.utf16_to_utf8(file_handle:read("*a"),u==2)
            end
            file_handle:close()
            t = {
                utftype = u, -- may go away
                lines = l,
                current = 0, -- line number, not really needed
                handle = nil,
                noflines = #l,
                close = function()
                    input.logger('= ' .. tag .. ' closer (' .. unicode.utfname[u] .. ')',filename)
                    input.show_close(filename)
                end,
--~                 getline = function(n)
--~                     local line = t.lines[n]
--~                     if not line or line == "" then
--~                         return ""
--~                     else
--~                         local translator = input.filters.utf_translator
--~                         return (translator and translator(line)) or line
--~                     end
--~                 end,
                reader = function(self)
                    self = self or t
                    local current, lines = self.current, self.lines
                    if current >= #lines then
                        return nil
                    else
                        current = current + 1
                        self.current = current
                        local line = lines[current]
                        if line == "" then
                            return ""
                        else
                            local translator = input.filters.utf_translator
                        --  return (translator and translator(line)) or line
                            if translator then
                                return translator(line)
                            else
                                return line
                            end
                        end
                    end
                end
            }
        else
            input.logger('+ ' .. tag .. ' opener',filename)
            -- todo: file;name -> freeze / eerste regel scannen -> freeze
            local filters = input.filters
            t = {
                reader = function(self)
                    local line = file_handle:read()
                    if line == "" then
                        return ""
                    end
                    local translator = filters.utf_translator
                    if translator then
                        return translator(line)
                    end
                    translator = filters.dynamic_translator
                    if translator then
                        return translator(line)
                    end
                    return line
                end,
                close = function()
                    input.logger('= ' .. tag .. ' closer',filename)
                    input.show_close(filename)
                    file_handle:close()
                end,
                handle = function()
                    return file_handle
                end,
                noflines = function()
                    t.noflines = io.noflines(file_handle)
                    return t.noflines
                end
            }
        end
        return t
    end

    function input.openers.generic(instance,tag,filename)
        if filename and filename ~= "" then
            local f = io.open(filename,"r")
            if f then
                input.show_open(filename)
                return input.openers.text_opener(filename,f,tag)
            end
        end
        input.logger('- ' .. tag .. ' opener',filename)
        return unpack(input.openers.notfound)
    end

    function input.loaders.generic(instance,tag,filename)
        if filename and filename ~= "" then
            local f = io.open(filename,"rb")
            if f then
                input.show_load(filename)
                input.logger('+ ' .. tag .. ' loader',filename)
                local s = f:read("*a")
                f:close()
                if s then
                    return true, s, #s
                end
            end
        end
        input.logger('- ' .. tag .. ' loader',filename)
        return unpack(input.loaders.notfound)
    end

    function input.finders.tex(instance,filename,filetype)
        return input.finders.generic(instance,'tex',filename,filetype)
    end
    function input.openers.tex(instance,filename)
        return input.openers.generic(instance,'tex',filename)
    end
    function input.loaders.tex(instance,filename)
        return input.loaders.generic(instance,'tex',filename)
    end

end

-- callback into the file io and related things; disabling kpse


if texconfig and not texlua then do

    -- this is not the right place, because we refer to quite some not yet defined tables, but who cares ...

    ctx = ctx or { }

    local ss = { }

    function ctx.writestatus(a,b)
        local s = ss[a]
        if not ss[a] then
            s = a:rpadd(15) .. ": "
            ss[a] = s
        end
        texio.write_nl(s .. b .. "\n")
    end

    -- this will become: ctx.install_statistics(fnc() return ..,.. end) etc

    local statusinfo, n = { }, 0

    function ctx.register_statistics(tag,pattern,fnc)
        statusinfo[#statusinfo+1] = { tag, pattern, fnc }
        if #tag > n then n = #tag end
    end

    function ctx.show_statistics() -- todo: move calls
        if caches then
            ctx.register_statistics("used config path", "%s", function() return caches.configpath(texmf.instance) end)
            ctx.register_statistics("used cache path", "%s", function() return caches.path end)
        end
        if status.luabytecodes > 0 and input.storage and input.storage.done then
            ctx.register_statistics("modules/dumps/instances", "%s/%s/%s", function() return status.luabytecodes-500, input.storage.done, status.luastates end)
        end
        if texmf.instance then
            ctx.register_statistics("input load time", "%s seconds", function() return input.loadtime(texmf.instance) end)
        end
        if fonts then
            ctx.register_statistics("fonts load time","%s seconds", function() return input.loadtime(fonts) end)
        end
        if xml then
            ctx.register_statistics("xml load time", "%s seconds, backreferences: %i, outer filtering time: %s", function() return input.loadtime(xml), #lxml.self, input.loadtime(lxml) end)
        end
        if mptopdf then
            ctx.register_statistics("mps conversion time", "%s seconds", function() return input.loadtime(mptopdf) end)
        end
        if nodes then
            ctx.register_statistics("node processing time", "%s seconds (including kernel)", function() return input.loadtime(nodes) end)
        end
        if kernel then
            ctx.register_statistics("kernel processing time", "%s seconds", function() return input.loadtime(kernel) end)
        end
        if attributes then
            ctx.register_statistics("attribute processing time", "%s seconds", function() return input.loadtime(attributes) end)
        end
        if languages then
            ctx.register_statistics("language load time", "%s seconds, n=%s", function() return input.loadtime(languages), languages.hyphenation.n() end)
        end
        if figures then
            ctx.register_statistics("graphics processing time", "%s seconds, n=%s (including tex)", function() return input.loadtime(figures), figures.n or "?" end)
        end
        if metapost then
            ctx.register_statistics("metapost processing time", "%s seconds, loading: %s seconds, execution: %s seconds, n: %s", function() return input.loadtime(metapost), input.loadtime(mplib), input.loadtime(metapost.exectime), metapost.n end)
        end
        if status.luastate_bytes then
            ctx.register_statistics("current memory usage", "%s bytes", function() return status.luastate_bytes end)
        end
        if nodes then
            ctx.register_statistics("cleaned up reserved nodes", "%s nodes, %s lists of %s", function() return nodes.cleanup_reserved(tex.count[24]) end) -- \topofboxstack
        end
        if status.node_mem_usage then
            ctx.register_statistics("node memory usage", "%s", function() return status.node_mem_usage end)
        end
        if languages then
            ctx.register_statistics("loaded patterns", "%s", function() return languages.logger.report() end)
        end
        if fonts then
            ctx.register_statistics("loaded fonts", "%s", function() return fonts.logger.report() end)
        end
        if xml then -- so we are in mkiv, we need a different check
            ctx.register_statistics("runtime", "%s seconds, %i processed pages, %i shipped pages, %.3f pages/second", function()
                input.stoptiming(texmf)
                local runtime = input.loadtime(texmf)
                local shipped = tex.count['nofshipouts']
                local pages = tex.count['realpageno'] - 1
                local persecond = shipped / runtime
                return runtime, pages, shipped, persecond
            end)
        end
        for _, t in ipairs(statusinfo) do
            local tag, pattern, fnc = t[1], t[2], t[3]
            ctx.writestatus("mkiv lua stats", string.format("%s - %s", tag:rpadd(n," "), pattern:format(fnc())))
        end
    end

end end

if texconfig and not texlua then

    texconfig.kpse_init        = false
    texconfig.trace_file_names = input.logmode() == 'tex'
    texconfig.max_print_line   = 100000

    -- if still present, we overload kpse (put it off-line so to say)

    if not texmf then texmf = { } end

    input.starttiming(texmf)

    if not texmf.instance then

        if not texmf.instance then -- prevent a second loading

            texmf.instance            = input.reset()
            texmf.instance.progname   = environment.progname or 'context'
            texmf.instance.engine     = environment.engine   or 'luatex'
            texmf.instance.validfile  = input.validctxfile

            input.load(texmf.instance)

        end

        if callback then
            callback.register('find_read_file'      , function(id,name) return input.findtexfile(texmf.instance,name) end)
            callback.register('open_read_file'      , function(   name) return input.opentexfile(texmf.instance,name) end)
        end

        if callback then
            callback.register('find_data_file'      , function(name) return input.findbinfile(texmf.instance,name,"tex") end)
            callback.register('find_enc_file'       , function(name) return input.findbinfile(texmf.instance,name,"enc") end)
            callback.register('find_font_file'      , function(name) return input.findbinfile(texmf.instance,name,"tfm") end)
            callback.register('find_format_file'    , function(name) return input.findbinfile(texmf.instance,name,"fmt") end)
            callback.register('find_image_file'     , function(name) return input.findbinfile(texmf.instance,name,"tex") end)
            callback.register('find_map_file'       , function(name) return input.findbinfile(texmf.instance,name,"map") end)
            callback.register('find_ocp_file'       , function(name) return input.findbinfile(texmf.instance,name,"ocp") end)
            callback.register('find_opentype_file'  , function(name) return input.findbinfile(texmf.instance,name,"otf") end)
            callback.register('find_output_file'    , function(name) return name                                         end)
            callback.register('find_pk_file'        , function(name) return input.findbinfile(texmf.instance,name,"pk")  end)
            callback.register('find_sfd_file'       , function(name) return input.findbinfile(texmf.instance,name,"sfd") end)
            callback.register('find_truetype_file'  , function(name) return input.findbinfile(texmf.instance,name,"ttf") end)
            callback.register('find_type1_file'     , function(name) return input.findbinfile(texmf.instance,name,"pfb") end)
            callback.register('find_vf_file'        , function(name) return input.findbinfile(texmf.instance,name,"vf")  end)

            callback.register('read_data_file'      , function(file) return input.loadbinfile(texmf.instance,file,"tex") end)
            callback.register('read_enc_file'       , function(file) return input.loadbinfile(texmf.instance,file,"enc") end)
            callback.register('read_font_file'      , function(file) return input.loadbinfile(texmf.instance,file,"tfm") end)
         -- format
         -- image
            callback.register('read_map_file'       , function(file) return input.loadbinfile(texmf.instance,file,"map") end)
            callback.register('read_ocp_file'       , function(file) return input.loadbinfile(texmf.instance,file,"ocp") end)
            callback.register('read_opentype_file'  , function(file) return input.loadbinfile(texmf.instance,file,"otf") end)
         -- output
            callback.register('read_pk_file'        , function(file) return input.loadbinfile(texmf.instance,file,"pk")  end)
            callback.register('read_sfd_file'       , function(file) return input.loadbinfile(texmf.instance,file,"sfd") end)
            callback.register('read_truetype_file'  , function(file) return input.loadbinfile(texmf.instance,file,"ttf") end)
            callback.register('read_type1_file'     , function(file) return input.loadbinfile(texmf.instance,file,"pfb") end)
            callback.register('read_vf_file'        , function(file) return input.loadbinfile(texmf.instance,file,"vf" ) end)
        end

        if callback and environment.aleph_mode then
            callback.register('find_font_file'      , function(name) return input.findbinfile(texmf.instance,name,"ofm") end)
            callback.register('read_font_file'      , function(file) return input.loadbinfile(texmf.instance,file,"ofm") end)
            callback.register('find_vf_file'        , function(name) return input.findbinfile(texmf.instance,name,"ovf") end)
            callback.register('read_vf_file'        , function(file) return input.loadbinfile(texmf.instance,file,"ovf") end)
        end

        if callback then
            callback.register('find_write_file'   , function(id,name) return name end)
        end

        if callback and (not config or (#config == 0)) then
            callback.register('find_format_file'  , function(name) return name end)
        end

        if callback and false then
            for k, v in pairs(callback.list()) do
                if not v then texio.write_nl("<w>callback "..k.." is not set</w>") end
            end
        end

        if callback then

            input.start_actions = { }
            input.stop_actions  = { }

            function input.register_start_actions(f) table.insert(input.start_actions, f) end
            function input.register_stop_actions (f) table.insert(input.stop_actions,  f) end

        --~ callback.register('start_run', function() for _, a in pairs(input.start_actions) do a() end end)
        --~ callback.register('stop_run' , function() for _, a in pairs(input.stop_actions ) do a() end end)

        end

        if callback then

            if input.logmode() == 'xml' then

                function input.start_page_number()
                    texio.write_nl("<p real='" .. tex.count[0] .. "' page='"..tex.count[1].."' sub='"..tex.count[2].."'")
                end
                function input.stop_page_number()
                    texio.write("/>")
                    texio.write_nl("")
                end

                callback.register('start_page_number'  , input.start_page_number)
                callback.register('stop_page_number'   , input.stop_page_number )

                function input.report_output_pages(p,b)
                    texio.write_nl("<v k='pages'>"..p.."</v>")
                    texio.write_nl("<v k='bytes'>"..b.."</v>")
                    texio.write_nl("")
                end
                function input.report_output_log()
                end

                callback.register('report_output_pages', input.report_output_pages)
                callback.register('report_output_log'  , input.report_output_log  )

                function input.start_run()
                    texio.write_nl("<?xml version='1.0' standalone='yes'?>")
                    texio.write_nl("<job xmlns='www.tug.org/luatex/schemas/context-job.rng'>")
                    texio.write_nl("")
                end
                function input.stop_run()
                    texio.write_nl("</job>")
                end
                function input.show_statistics()
                    for k,v in pairs(status.list()) do
                        texio.write_nl("log","<v k='"..k.."'>"..tostring(v).."</v>")
                    end
                end

                table.insert(input.start_actions, input.start_run)
                table.insert(input.stop_actions , input.show_statistics)
                table.insert(input.stop_actions , input.stop_run)

            else
                table.insert(input.stop_actions , input.show_statistics)
            end

            callback.register('start_run', function() for _, a in pairs(input.start_actions) do a() end end)
            callback.register('stop_run' , function() for _, a in pairs(input.stop_actions ) do a() end ctx.show_statistics() end)

        end

    end

    if kpse then

        function kpse.find_file(filename,filetype,mustexist)
            return input.find_file(texmf.instance,filename,filetype,mustexist)
        end
        function kpse.expand_path(variable)
            return input.expand_path(texmf.instance,variable)
        end
        function kpse.expand_var(variable)
            return input.expand_var(texmf.instance,variable)
        end
        function kpse.expand_braces(variable)
            return input.expand_braces(texmf.instance,variable)
        end

    end

end

-- program specific configuration (memory settings and alike)

if texconfig and not texlua then

    luatex = luatex or { }

    luatex.variablenames = {
        'main_memory', 'extra_mem_bot', 'extra_mem_top',
        'buf_size','expand_depth',
        'font_max', 'font_mem_size',
        'hash_extra', 'max_strings', 'pool_free', 'pool_size', 'string_vacancies',
        'obj_tab_size', 'pdf_mem_size', 'dest_names_size',
        'nest_size', 'param_size', 'save_size', 'stack_size',
        'trie_size', 'hyph_size', 'max_in_open',
        'ocp_stack_size', 'ocp_list_size', 'ocp_buf_size'
    }

    function luatex.variables()
        local t, x = { }, nil
        for _,v in pairs(luatex.variablenames) do
            x = input.var_value(texmf.instance,v)
            if x and x:find("^%d+$") then
                t[v] = tonumber(x)
            end
        end
        return t
    end

    function luatex.setvariables(tab)
        for k,v in pairs(luatex.variables()) do
            tab[k] = v
        end
    end

    if not luatex.variables_set then
        luatex.setvariables(texconfig)
        luatex.variables_set = true
    end

    texconfig.max_print_line = 100000
    texconfig.max_in_open    = 127

end

-- some tex basics

if not cs then cs = { } end

function cs.def(k,v)
    tex.sprint(tex.texcatcodes, "\\def\\" .. k .. "{" .. v .. "}")
end

function cs.chardef(k,v)
    tex.sprint(tex.texcatcodes, "\\chardef\\" .. k .. "=" .. v .. "\\relax")
end

function cs.boolcase(b)
    if b then tex.write(1) else tex.write(0) end
end

function cs.testcase(b)
    if b then
        tex.sprint(tex.texcatcodes, "\\firstoftwoarguments")
    else
        tex.sprint(tex.texcatcodes, "\\secondoftwoarguments")
    end
end


if not modules then modules = { } end modules ['luat-kps'] = {
    version   = 1.001,
    comment   = "companion to luatools.lua",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

--[[ldx--
<p>This file is used when we want the input handlers to behave like
<type>kpsewhich</type>. What to do with the following:</p>

<typing>
{$SELFAUTOLOC,$SELFAUTODIR,$SELFAUTOPARENT}{,{/share,}/texmf{-local,}/web2c}
$SELFAUTOLOC    : /usr/tex/bin/platform
$SELFAUTODIR    : /usr/tex/bin
$SELFAUTOPARENT : /usr/tex
</typing>

<p>How about just forgetting abou them?</p>
--ldx]]--

input          = input          or { }
input.suffixes = input.suffixes or { }
input.formats  = input.formats  or { }

input.suffixes['gf']                       = { '<resolution>gf' }
input.suffixes['pk']                       = { '<resolution>pk' }
input.suffixes['base']                     = { 'base' }
input.suffixes['bib']                      = { 'bib' }
input.suffixes['bst']                      = { 'bst' }
input.suffixes['cnf']                      = { 'cnf' }
input.suffixes['mem']                      = { 'mem' }
input.suffixes['mf']                       = { 'mf' }
input.suffixes['mfpool']                   = { 'pool' }
input.suffixes['mft']                      = { 'mft' }
input.suffixes['mppool']                   = { 'pool' }
input.suffixes['graphic/figure']           = { 'eps', 'epsi' }
input.suffixes['texpool']                  = { 'pool' }
input.suffixes['PostScript header']        = { 'pro' }
input.suffixes['ist']                      = { 'ist' }
input.suffixes['web']                      = { 'web', 'ch' }
input.suffixes['cweb']                     = { 'w', 'web', 'ch' }
input.suffixes['cmap files']               = { 'cmap' }
input.suffixes['lig files']                = { 'lig' }
input.suffixes['bitmap font']              = { }
input.suffixes['MetaPost support']         = { }
input.suffixes['TeX system documentation'] = { }
input.suffixes['TeX system sources']       = { }
input.suffixes['dvips config']             = { }
input.suffixes['type42 fonts']             = { }
input.suffixes['web2c files']              = { }
input.suffixes['other text files']         = { }
input.suffixes['other binary files']       = { }
input.suffixes['opentype fonts']           = { 'otf' }

input.suffixes['fmt']                      = { 'fmt' }
input.suffixes['texmfscripts']             = { 'rb','lua','py','pl' }

input.suffixes['pdftex config']            = { }
input.suffixes['Troff fonts']              = { }

input.suffixes['ls-R']                     = { }

--[[ldx--
<p>If you wondered abou tsome of the previous mappings, how about
the next bunch:</p>
--ldx]]--

input.formats['bib']                      = ''
input.formats['bst']                      = ''
input.formats['mft']                      = ''
input.formats['ist']                      = ''
input.formats['web']                      = ''
input.formats['cweb']                     = ''
input.formats['MetaPost support']         = ''
input.formats['TeX system documentation'] = ''
input.formats['TeX system sources']       = ''
input.formats['Troff fonts']              = ''
input.formats['dvips config']             = ''
input.formats['graphic/figure']           = ''
input.formats['ls-R']                     = ''
input.formats['other text files']         = ''
input.formats['other binary files']       = ''

input.formats['gf']                       = ''
input.formats['pk']                       = ''
input.formats['base']                     = 'MFBASES'
input.formats['cnf']                      = ''
input.formats['mem']                      = 'MPMEMS'
input.formats['mf']                       = 'MFINPUTS'
input.formats['mfpool']                   = 'MFPOOL'
input.formats['mppool']                   = 'MPPOOL'
input.formats['texpool']                  = 'TEXPOOL'
input.formats['PostScript header']        = 'TEXPSHEADERS'
input.formats['cmap files']               = 'CMAPFONTS'
input.formats['type42 fonts']             = 'T42FONTS'
input.formats['web2c files']              = 'WEB2C'
input.formats['pdftex config']            = 'PDFTEXCONFIG'
input.formats['texmfscripts']             = 'TEXMFSCRIPTS'
input.formats['bitmap font']              = ''
input.formats['lig files']                = 'LIGFONTS'

-- end library merge

-- We initialize some characteristics of this program. We need to
-- do this before we load the libraries, else own.name will not be
-- properly set (handy for selfcleaning the file). It's an ugly
-- looking piece of code.

own = { }

own.libs = { -- todo: check which ones are really needed
    'l-string.lua',
    'l-lpeg.lua',
    'l-table.lua',
    'l-io.lua',
    'l-number.lua',
    'l-set.lua',
    'l-os.lua',
    'l-md5.lua',
    'l-file.lua',
    'l-url.lua',
    'l-dir.lua',
    'l-boolean.lua',
    'l-unicode.lua',
    'l-utils.lua',
    'luat-lib.lua',
    'luat-inp.lua',
    'luat-tmp.lua',
    'luat-zip.lua',
    'luat-tex.lua',
    'luat-kps.lua',
}

-- We need this hack till luatex is fixed.

if arg and arg[0] == 'luatex' and arg[1] == "--luaonly" then
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
    print("Luatools is unable to start up due to lack of libraries. You may")
    print("try to run 'lua luatools.lua --selfmerge' in the path where this")
    print("script is located (normally under ..../scripts/context/lua) which")
    print("will make luatools library independent.")
    os.exit()
end

instance            = input.reset()
input.verbose       = environment.arguments["verbose"]  or false
input.banner        = 'LuaTools | '
utils.report        = input.report

input.defaultlibs   = { -- not all are needed
    'l-string.lua', 'l-lpeg.lua', 'l-table.lua', 'l-boolean.lua', 'l-number.lua', 'l-set.lua', 'l-unicode.lua',
    'l-md5.lua', 'l-os.lua', 'l-io.lua', 'l-file.lua', 'l-url.lua', 'l-dir.lua', 'l-utils.lua', 'l-tex.lua',
    'luat-env.lua', 'luat-lib.lua', 'luat-inp.lua', 'luat-tmp.lua', 'luat-zip.lua', 'luat-tex.lua'
}

-- todo: use environment.argument() instead of environment.arguments[]

instance.engine     =     environment.arguments["engine"]   or 'luatex'
instance.progname   =     environment.arguments["progname"] or 'context'
instance.luaname    =     environment.arguments["luafile"]  or "" -- environment.ownname or ""
instance.lualibs    =     environment.arguments["lualibs"]  or table.concat(input.defaultlibs,",")
instance.allresults =     environment.arguments["all"]      or false
instance.pattern    =     environment.arguments["pattern"]  or nil
instance.sortdata   =     environment.arguments["sort"]     or false
instance.kpseonly   = not environment.arguments["all"]      or false
instance.my_format  =     environment.arguments["format"]   or instance.format
instance.lsrmode    =     environment.arguments["lsr"]      or false

if type(instance.pattern) == 'boolean' then
    input.report("invalid pattern specification") -- toto, force verbose for one message
    instance.pattern = nil
end

if environment.arguments["trace"] then input.settrace(environment.arguments["trace"]) end

if environment.arguments["minimize"] then
    if input.validators.visibility[instance.progname] then
        instance.validfile = input.validators.visibility[instance.progname]
    end
end

function input.my_prepare_a(instance)
    input.resetconfig(instance)
    input.identify_cnf(instance)
    input.load_lua(instance)
    input.expand_variables(instance)
    input.load_cnf(instance)
    input.expand_variables(instance)
end

function input.my_prepare_b(instance)
    input.my_prepare_a(instance)
    input.load_hash(instance)
    input.automount(instance)
end

-- barename

if not messages then messages = { } end

messages.no_ini_file = [[
There is no lua initialization file found. This file can be forced by the
"--progname" directive, or specified with "--luaname", or it is derived
automatically from the formatname (aka jobname). It may be that you have
to regenerate the file database using "luatools --generate".
]]

messages.help = [[
--generate        generate file database
--variables       show configuration variables
--expansions      show expanded variables
--configurations  show configuration order
--expand-braces   expand complex variable
--expand-path     expand variable (resolve paths)
--expand-var      expand variable (resolve references)
--show-path       show path expansion of ...
--var-value       report value of variable
--find-file       report file location
--find-path       report path of file
--make or --ini   make luatex format
--run or --fmt=   run luatex format
--luafile=str     lua inifile (default is <progname>.lua)
--lualibs=list    libraries to assemble (optional when --compile)
--compile         assemble and compile lua inifile
--mkii            force context mkii mode (only for testing, not usable!)
--verbose         give a bit more info
--minimize        optimize lists for format
--all             show all found files
--sort            sort cached data
--engine=str      target engine
--progname=str    format or backend
--pattern=str     filter variables
--lsr             use lsr and cnf directly
]]

function input.my_make_format(instance,texname)
    if texname and texname ~= "" then
        if input.usecache then
            local path = file.join(caches.setpath(instance,"formats")) -- maybe platform
            if path and lfs then
                lfs.chdir(path)
            end
        end
        local barename = texname:gsub("%.%a+$","")
        if barename == texname then
            texname = texname .. ".tex"
        end
        local fullname = input.find_files(instance,texname)[1] or ""
        if fullname == "" then
            input.report("no tex file with name",texname)
        else
            local luaname, lucname, luapath, lualibs = "", "", "", { }
            -- the following is optional, since context.lua can also
            -- handle this collect and compile business
            if environment.arguments["compile"] then
                if luaname == "" then luaname = barename end
                input.report("creating initialization file " .. luaname)
                luapath = file.dirname(luaname)
                if luapath == "" then
                    luapath = file.dirname(texname)
                end
                if luapath == "" then
                    luapath = file.dirname(input.find_files(instance,texname)[1] or "")
                end
                lualibs = string.split(instance.lualibs,",")
                luaname = file.basename(barename .. ".lua")
                lucname = file.basename(barename .. ".luc")
                -- todo: when this fails, we can just copy the merged libraries from
                -- luatools since they are normally the same, at least for context
                if lualibs[1] then
                    local firstlib = file.join(luapath,lualibs[1])
                    if not lfs.isfile(firstlib) then
                        local foundname = input.find_files(instance,lualibs[1])[1]
                        if foundname then
                            input.report("located library path : " .. luapath)
                            luapath = file.dirname(foundname)
                        end
                    end
                end
                input.report("using library path : " .. luapath)
                input.report("using lua libraries: " .. table.join(lualibs," "))
                utils.merger.selfcreate(lualibs,luapath,luaname)
                if utils.lua.compile(luaname, lucname) and io.exists(lucname) then
                    luaname = lucname
                    input.report("using compiled initialization file " .. lucname)
                else
                    input.report("using uncompiled initialization file " .. luaname)
                end
            else
                for _, v in pairs({instance.luaname, instance.progname, barename}) do
                    v = string.gsub(v..".lua","%.lua%.lua$",".lua")
                    if v and (v ~= "") then
                        luaname = input.find_files(instance,v)[1] or ""
                        if luaname ~= "" then
                            break
                        end
                    end
                end
            end
            if luaname == "" then
                input.reportlines(messages.no_ini_file)
                input.report("texname  : " .. texname)
                input.report("luaname  : " .. instance.luaname)
                input.report("progname : " .. instance.progname)
                input.report("barename : " .. barename)
            else
                input.report("using lua initialization file " .. luaname)
                local flags = { "--ini" }
                if environment.arguments["mkii"] then
                    flags[#flags+1] = "--progname=" .. instance.progname
                else
                    flags[#flags+1] = "--lua=" .. string.quote(luaname)
                end
                local bs = (environment.platform == "unix" and "\\\\") or "\\" -- todo: make a function
                local command = "luatex ".. table.concat(flags," ")  .. " " .. string.quote(fullname) .. " " .. bs .. "dump"
                input.report("running command: " .. command .. "\n")
                os.spawn(command)
            end
        end
    else
        input.report("no tex file given")
    end
end

function input.my_run_format(instance,name,data,more)
 -- hm, rather old code here; we can now use the file.whatever functions
    if name and (name ~= "") then
        local barename = name:gsub("%.%a+$","")
        local fmtname = ""
        if input.usecache then
            local path = file.join(caches.setpath(instance,"formats")) -- maybe platform
            fmtname = file.join(path,barename..".fmt") or ""
        end
        if fmtname == "" then
            fmtname = input.find_files(instance,barename..".fmt")[1] or ""
        end
        fmtname = input.clean_path(fmtname)
        barename = fmtname:gsub("%.%a+$","")
        if fmtname == "" then
            input.report("no format with name",name)
        else
            local luaname = barename .. ".luc"
            local f = io.open(luaname)
            if not f then
                luaname = barename .. ".lua"
                f = io.open(luaname)
            end
            if f then
                f:close()
                local command = "luatex --fmt=" .. string.quote(barename) .. " --lua=" .. string.quote(luaname) .. " " .. string.quote(data) .. " " .. string.quote(more)
                input.report("running command: " .. command)
                os.spawn(command)
            else
                input.report("using format name",fmtname)
                input.report("no luc/lua with name",barename)
            end
        end
    end
end

-- helpers for verbose lists

input.listers = input.listers or { }

local function tabstr(str)
    if type(str) == 'table' then
        return table.concat(str," | ")
    else
        return str
    end
end

local function list(instance,list)
    local pat = string.upper(instance.pattern or "","")
    for _,key in pairs(table.sortedkeys(list)) do
        if instance.pattern == "" or string.find(key:upper(),pat) then
            if instance.kpseonly then
                if instance.kpsevars[key] then
                    print(format("%s=%s",key,tabstr(list[key])))
                end
            else
                print(format('%s %s=%s',(instance.kpsevars[key] and 'K') or 'E',key,tabstr(list[key])))
            end
        end
    end
end

function input.listers.variables (instance) list(instance,instance.variables ) end
function input.listers.expansions(instance) list(instance,instance.expansions) end

function input.listers.configurations(instance)
    for _,key in pairs(table.sortedkeys(instance.kpsevars)) do
        if not instance.pattern or (instance.pattern=="") or key:find(instance.pattern) then
            print(key.."\n")
            for i,c in ipairs(instance.order) do
                local str = c[key]
                if str then
                    print(format("\t%s\t\t%s",i,input.aux.tabstr(str)))
                end
            end
            print()
        end
    end
end

input.report(banner,"\n")

local ok = true

if environment.arguments["find-file"] then
    input.my_prepare_b(instance)
    instance.format  = environment.arguments["format"] or instance.format
    if instance.pattern then
        instance.allresults = true
        input.for_files(instance, input.find_files, { instance.pattern }, instance.my_format)
    else
        input.for_files(instance, input.find_files, environment.files, instance.my_format)
    end
elseif environment.arguments["find-path"] then
    input.my_prepare_b(instance)
    local path = input.find_file(instance, environment.files[1], instance.my_format)
    if input.verbose then
        input.report(file.dirname(path))
    else
        print(file.dirname(path))
    end
--~ elseif environment.arguments["first-writable-path"] then
--~     input.my_prepare_b(instance)
--~     input.report(input.first_writable_path(instance,environment.files[1] or "."))
elseif environment.arguments["run"] then
    input.my_prepare_a(instance) -- ! no need for loading databases
    input.verbose = true
    input.my_run_format(instance,environment.files[1] or "",environment.files[2] or "",environment.files[3] or "")
elseif environment.arguments["fmt"] then
    input.my_prepare_a(instance) -- ! no need for loading databases
    input.verbose = true
    input.my_run_format(instance,environment.arguments["fmt"], environment.files[1] or "",environment.files[2] or "")
elseif environment.arguments["expand-braces"] then
    input.my_prepare_a(instance)
    input.for_files(instance, input.expand_braces, environment.files)
elseif environment.arguments["expand-path"] then
    input.my_prepare_a(instance)
    input.for_files(instance, input.expand_path, environment.files)
elseif environment.arguments["expand-var"] or environment.arguments["expand-variable"] then
    input.my_prepare_a(instance)
    input.for_files(instance, input.expand_var, environment.files)
elseif environment.arguments["show-path"] or environment.arguments["path-value"] then
    input.my_prepare_a(instance)
    input.for_files(instance, input.show_path, environment.files)
elseif environment.arguments["var-value"] or environment.arguments["show-value"] then
    input.my_prepare_a(instance)
    input.for_files(instance, input.var_value, environment.files)
elseif environment.arguments["format-path"] then
    input.my_prepare_b(instance)
    input.report(caches.setpath(instance,"format"))
elseif instance.pattern then -- brrr
    input.my_prepare_b(instance)
    instance.format = environment.arguments["format"] or instance.format
    instance.allresults = true
    input.for_files(instance, input.find_files, { instance.pattern }, instance.my_format)
elseif environment.arguments["generate"] then
    instance.renewcache = true
    input.verbose = true
    input.my_prepare_b(instance)
elseif environment.arguments["make"] or environment.arguments["ini"] or environment.arguments["compile"] then
    input.my_prepare_b(instance)
    input.verbose = true
    input.my_make_format(instance,environment.files[1] or "")
elseif environment.arguments["selfmerge"] then
    utils.merger.selfmerge(own.name,own.libs,own.list)
elseif environment.arguments["selfclean"] then
    utils.merger.selfclean(own.name)
elseif environment.arguments["selfupdate"] then
    input.my_prepare_b(instance)
    input.verbose = true
    input.update_script(instance,own.name,"luatools")
elseif environment.arguments["variables"] or environment.arguments["show-variables"] then
    input.my_prepare_a(instance)
    input.listers.variables(instance)
elseif environment.arguments["expansions"] or environment.arguments["show-expansions"] then
    input.my_prepare_a(instance)
    input.listers.expansions(instance)
elseif environment.arguments["configurations"] or environment.arguments["show-configurations"] then
    input.my_prepare_a(instance)
    input.listers.configurations(instance)
elseif environment.arguments["help"] or (environment.files[1]=='help') or (#environment.files==0) then
    if not input.verbose then
        input.verbose = true
        input.report(banner,"\n")
    end
    input.reportlines(messages.help)
else
    input.my_prepare_b(instance)
    input.for_files(instance, input.find_files, environment.files, instance.my_format)
end

if input.verbose then
    input.report("")
    input.report(string.format("runtime: %0.3f seconds",os.runtime()))
end

--~ if ok then
--~     input.report("exit code: 0") os.exit(0)
--~ else
--~     input.report("exit code: 1") os.exit(1)
--~ end

if environment.platform == "unix" then
    io.write("\n")
end
