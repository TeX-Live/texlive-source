#!/usr/bin/env texlua

if not modules then modules = { } end modules ['luatools'] = {
    version   = 1.001,
    comment   = "companion to context.tex",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local format = string.format

-- one can make a stub:
--
-- #!/bin/sh
-- env LUATEXDIR=/....../texmf/scripts/context/lua texlua luatools.lua "$@"

-- Although this script is part of the ConTeXt distribution it is
-- relatively indepent of ConTeXt.  The same is true for some of
-- the luat files. We may may make them even less dependent in
-- the future. As long as Luatex is under development the
-- interfaces and names of functions may change.

-- For the sake of independence we optionally can merge the library
-- code here. It's too much code, but that does not harm. Much of the
-- library code is used elsewhere. We don't want dependencies on
-- Lua library paths simply because these scripts are located in the
-- texmf tree and not in some Lua path. Normally this merge is not
-- needed when texmfstart is used, or when the proper stub is used or
-- when (windows) suffix binding is active.

texlua = true

-- begin library merge



do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-string'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local sub, gsub, find, match, gmatch, format, char, byte, rep, lower = string.sub, string.gsub, string.find, string.match, string.gmatch, string.format, string.char, string.byte, string.rep, string.lower
local lpegmatch = lpeg.match

-- some functions may disappear as they are not used anywhere

if not string.split then

    -- this will be overloaded by a faster lpeg variant

    function string:split(pattern)
        if #self > 0 then
            local t = { }
            for s in gmatch(self..pattern,"(.-)"..pattern) do
                t[#t+1] = s
            end
            return t
        else
            return { }
        end
    end

end

local chr_to_esc = {
    ["%"] = "%%",
    ["."] = "%.",
    ["+"] = "%+", ["-"] = "%-", ["*"] = "%*",
    ["^"] = "%^", ["$"] = "%$",
    ["["] = "%[", ["]"] = "%]",
    ["("] = "%(", [")"] = "%)",
    ["{"] = "%{", ["}"] = "%}"
}

string.chr_to_esc = chr_to_esc

function string:esc() -- variant 2
    return (gsub(self,"(.)",chr_to_esc))
end

function string:unquote()
    return (gsub(self,"^([\"\'])(.*)%1$","%2"))
end

--~ function string:unquote()
--~     if find(self,"^[\'\"]") then
--~         return sub(self,2,-2)
--~     else
--~         return self
--~     end
--~ end

function string:quote() -- we could use format("%q")
    return format("%q",self)
end

function string:count(pattern) -- variant 3
    local n = 0
    for _ in gmatch(self,pattern) do
        n = n + 1
    end
    return n
end

function string:limit(n,sentinel)
    if #self > n then
        sentinel = sentinel or " ..."
        return sub(self,1,(n-#sentinel)) .. sentinel
    else
        return self
    end
end

--~ function string:strip() -- the .- is quite efficient
--~  -- return match(self,"^%s*(.-)%s*$") or ""
--~  -- return match(self,'^%s*(.*%S)') or '' -- posted on lua list
--~     return find(s,'^%s*$') and '' or match(s,'^%s*(.*%S)')
--~ end

do -- roberto's variant:
    local space    = lpeg.S(" \t\v\n")
    local nospace  = 1 - space
    local stripper = space^0 * lpeg.C((space^0 * nospace^1)^0)
    function string.strip(str)
        return lpegmatch(stripper,str) or ""
    end
end

function string:is_empty()
    return not find(self,"%S")
end

function string:enhance(pattern,action)
    local ok, n = true, 0
    while ok do
        ok = false
        self = gsub(self,pattern, function(...)
            ok, n = true, n + 1
            return action(...)
        end)
    end
    return self, n
end

local chr_to_hex, hex_to_chr = { }, { }

for i=0,255 do
    local c, h = char(i), format("%02X",i)
    chr_to_hex[c], hex_to_chr[h] = h, c
end

function string:to_hex()
    return (gsub(self or "","(.)",chr_to_hex))
end

function string:from_hex()
    return (gsub(self or "","(..)",hex_to_chr))
end

if not string.characters then

    local function nextchar(str, index)
        index = index + 1
        return (index <= #str) and index or nil, sub(str,index,index)
    end
    function string:characters()
        return nextchar, self, 0
    end
    local function nextbyte(str, index)
        index = index + 1
        return (index <= #str) and index or nil, byte(sub(str,index,index))
    end
    function string:bytes()
        return nextbyte, self, 0
    end

end

-- we can use format for this (neg n)

function string:rpadd(n,chr)
    local m = n-#self
    if m > 0 then
        return self .. rep(chr or " ",m)
    else
        return self
    end
end

function string:lpadd(n,chr)
    local m = n-#self
    if m > 0 then
        return rep(chr or " ",m) .. self
    else
        return self
    end
end

string.padd = string.rpadd

function is_number(str) -- tonumber
    return find(str,"^[%-%+]?[%d]-%.?[%d+]$") == 1
end

--~ print(is_number("1"))
--~ print(is_number("1.1"))
--~ print(is_number(".1"))
--~ print(is_number("-0.1"))
--~ print(is_number("+0.1"))
--~ print(is_number("-.1"))
--~ print(is_number("+.1"))

function string:split_settings() -- no {} handling, see l-aux for lpeg variant
    if find(self,"=") then
        local t = { }
        for k,v in gmatch(self,"(%a+)=([^%,]*)") do
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
    return (gsub(self,".",patterns_escapes))
end

local simple_escapes = {
    ["-"] = "%-",
    ["."] = "%.",
    ["?"] = ".",
    ["*"] = ".*",
}

function string:simpleesc()
    return (gsub(self,".",simple_escapes))
end

function string:tohash()
    local t = { }
    for s in gmatch(self,"([^, ]+)") do -- lpeg
        t[s] = true
    end
    return t
end

local pattern = lpeg.Ct(lpeg.C(1)^0)

function string:totable()
    return lpegmatch(pattern,self)
end

--~ local t = {
--~     "1234567123456712345671234567",
--~     "a\tb\tc",
--~     "aa\tbb\tcc",
--~     "aaa\tbbb\tccc",
--~     "aaaa\tbbbb\tcccc",
--~     "aaaaa\tbbbbb\tccccc",
--~     "aaaaaa\tbbbbbb\tcccccc",
--~ }
--~ for k,v do
--~     print(string.tabtospace(t[k]))
--~ end

function string.tabtospace(str,tab)
    -- we don't handle embedded newlines
    while true do
        local s = find(str,"\t")
        if s then
            if not tab then tab = 7 end -- only when found
            local d = tab-(s-1) % tab
            if d > 0 then
                str = gsub(str,"\t",rep(" ",d),1)
            else
                str = gsub(str,"\t","",1)
            end
        else
            break
        end
    end
    return str
end

function string:compactlong() -- strips newlines and leading spaces
    self = gsub(self,"[\n\r]+ *","")
    self = gsub(self,"^ *","")
    return self
end

function string:striplong() -- strips newlines and leading spaces
    self = gsub(self,"^%s*","")
    self = gsub(self,"[\n\r]+ *","\n")
    return self
end

function string:topattern(lowercase,strict)
    if lowercase then
        self = lower(self)
    end
    self = gsub(self,".",simple_escapes)
    if self == "" then
        self = ".*"
    elseif strict then
        self = "^" .. self .. "$"
    end
    return self
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-lpeg'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local lpeg = require("lpeg")

lpeg.patterns  = lpeg.patterns or { } -- so that we can share
local patterns = lpeg.patterns

local P, R, S, Ct, C, Cs, Cc, V = lpeg.P, lpeg.R, lpeg.S, lpeg.Ct, lpeg.C, lpeg.Cs, lpeg.Cc, lpeg.V
local match = lpeg.match

local digit, sign      = R('09'), S('+-')
local cr, lf, crlf     = P("\r"), P("\n"), P("\r\n")
local utf8byte         = R("\128\191")

patterns.utf8byte      = utf8byte
patterns.utf8one       = R("\000\127")
patterns.utf8two       = R("\194\223") * utf8byte
patterns.utf8three     = R("\224\239") * utf8byte * utf8byte
patterns.utf8four      = R("\240\244") * utf8byte * utf8byte * utf8byte

patterns.digit         = digit
patterns.sign          = sign
patterns.cardinal      = sign^0 * digit^1
patterns.integer       = sign^0 * digit^1
patterns.float         = sign^0 * digit^0 * P('.') * digit^1
patterns.number        = patterns.float + patterns.integer
patterns.oct           = P("0") * R("07")^1
patterns.octal         = patterns.oct
patterns.HEX           = P("0x") * R("09","AF")^1
patterns.hex           = P("0x") * R("09","af")^1
patterns.hexadecimal   = P("0x") * R("09","AF","af")^1
patterns.lowercase     = R("az")
patterns.uppercase     = R("AZ")
patterns.letter        = patterns.lowercase + patterns.uppercase
patterns.space         = S(" ")
patterns.eol           = S("\n\r")
patterns.spacer        = S(" \t\f\v")  -- + string.char(0xc2, 0xa0) if we want utf (cf mail roberto)
patterns.newline       = crlf + cr + lf
patterns.nonspace      = 1 - patterns.space
patterns.nonspacer     = 1 - patterns.spacer
patterns.whitespace    = patterns.eol + patterns.spacer
patterns.nonwhitespace = 1 - patterns.whitespace
patterns.utf8          = patterns.utf8one + patterns.utf8two + patterns.utf8three + patterns.utf8four
patterns.utfbom        = P('\000\000\254\255') + P('\255\254\000\000') + P('\255\254') + P('\254\255') + P('\239\187\191')

function lpeg.anywhere(pattern) --slightly adapted from website
    return P { P(pattern) + 1 * V(1) } -- why so complex?
end

function lpeg.splitter(pattern, action)
    return (((1-P(pattern))^1)/action+1)^0
end

local spacing  = patterns.spacer^0 * patterns.newline -- sort of strip
local empty    = spacing * Cc("")
local nonempty = Cs((1-spacing)^1) * spacing^-1
local content  = (empty + nonempty)^1

local capture = Ct(content^0)

function string:splitlines()
    return match(capture,self)
end

patterns.textline = content

--~ local p = lpeg.splitat("->",false)  print(match(p,"oeps->what->more"))  -- oeps what more
--~ local p = lpeg.splitat("->",true)   print(match(p,"oeps->what->more"))  -- oeps what->more
--~ local p = lpeg.splitat("->",false)  print(match(p,"oeps"))              -- oeps
--~ local p = lpeg.splitat("->",true)   print(match(p,"oeps"))              -- oeps

local splitters_s, splitters_m = { }, { }

local function splitat(separator,single)
    local splitter = (single and splitters_s[separator]) or splitters_m[separator]
    if not splitter then
        separator = P(separator)
        if single then
            local other, any = C((1 - separator)^0), P(1)
            splitter = other * (separator * C(any^0) + "") -- ?
            splitters_s[separator] = splitter
        else
            local other = C((1 - separator)^0)
            splitter = other * (separator * other)^0
            splitters_m[separator] = splitter
        end
    end
    return splitter
end

lpeg.splitat = splitat

local cache = { }

function lpeg.split(separator,str)
    local c = cache[separator]
    if not c then
        c = Ct(splitat(separator))
        cache[separator] = c
    end
    return match(c,str)
end

function string:split(separator)
    local c = cache[separator]
    if not c then
        c = Ct(splitat(separator))
        cache[separator] = c
    end
    return match(c,self)
end

lpeg.splitters = cache

local cache = { }

function lpeg.checkedsplit(separator,str)
    local c = cache[separator]
    if not c then
        separator = P(separator)
        local other = C((1 - separator)^0)
        c = Ct(separator^0 * other * (separator^1 * other)^0)
        cache[separator] = c
    end
    return match(c,str)
end

function string:checkedsplit(separator)
    local c = cache[separator]
    if not c then
        separator = P(separator)
        local other = C((1 - separator)^0)
        c = Ct(separator^0 * other * (separator^1 * other)^0)
        cache[separator] = c
    end
    return match(c,self)
end

--~ function lpeg.append(list,pp)
--~     local p = pp
--~     for l=1,#list do
--~         if p then
--~             p = p + P(list[l])
--~         else
--~             p = P(list[l])
--~         end
--~     end
--~     return p
--~ end

--~ from roberto's site:

local f1 = string.byte

local function f2(s) local c1, c2         = f1(s,1,2) return   c1 * 64 + c2                       -    12416 end
local function f3(s) local c1, c2, c3     = f1(s,1,3) return  (c1 * 64 + c2) * 64 + c3            -   925824 end
local function f4(s) local c1, c2, c3, c4 = f1(s,1,4) return ((c1 * 64 + c2) * 64 + c3) * 64 + c4 - 63447168 end

patterns.utf8byte = patterns.utf8one/f1 + patterns.utf8two/f2 + patterns.utf8three/f3 + patterns.utf8four/f4


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-table'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

table.join = table.concat

local concat, sort, insert, remove = table.concat, table.sort, table.insert, table.remove
local format, find, gsub, lower, dump, match = string.format, string.find, string.gsub, string.lower, string.dump, string.match
local getmetatable, setmetatable = getmetatable, setmetatable
local type, next, tostring, tonumber, ipairs = type, next, tostring, tonumber, ipairs
local unpack = unpack or table.unpack

function table.strip(tab)
    local lst = { }
    for i=1,#tab do
        local s = gsub(tab[i],"^%s*(.-)%s*$","%1")
        if s == "" then
            -- skip this one
        else
            lst[#lst+1] = s
        end
    end
    return lst
end

function table.keys(t)
    local k = { }
    for key, _ in next, t do
        k[#k+1] = key
    end
    return k
end

local function compare(a,b)
    return (tostring(a) < tostring(b))
end

local function sortedkeys(tab)
    local srt, kind = { }, 0 -- 0=unknown 1=string, 2=number 3=mixed
    for key,_ in next, tab do
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
        sort(srt,compare)
    else
        sort(srt)
    end
    return srt
end

local function sortedhashkeys(tab) -- fast one
    local srt = { }
    for key,_ in next, tab do
        srt[#srt+1] = key
    end
    sort(srt)
    return srt
end

table.sortedkeys     = sortedkeys
table.sortedhashkeys = sortedhashkeys

function table.sortedhash(t)
    local s = sortedhashkeys(t) -- maybe just sortedkeys
    local n = 0
    local function kv(s)
        n = n + 1
        local k = s[n]
        return k, t[k]
    end
    return kv, s
end

table.sortedpairs = table.sortedhash

function table.append(t, list)
    for _,v in next, list do
        insert(t,v)
    end
end

function table.prepend(t, list)
    for k,v in next, list do
        insert(t,k,v)
    end
end

function table.merge(t, ...) -- first one is target
    t = t or {}
    local lst = {...}
    for i=1,#lst do
        for k, v in next, lst[i] do
            t[k] = v
        end
    end
    return t
end

function table.merged(...)
    local tmp, lst = { }, {...}
    for i=1,#lst do
        for k, v in next, lst[i] do
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

local function fastcopy(old) -- fast one
    if old then
        local new = { }
        for k,v in next, old do
            if type(v) == "table" then
                new[k] = fastcopy(v) -- was just table.copy
            else
                new[k] = v
            end
        end
        -- optional second arg
        local mt = getmetatable(old)
        if mt then
            setmetatable(new,mt)
        end
        return new
    else
        return { }
    end
end

local function copy(t, tables) -- taken from lua wiki, slightly adapted
    tables = tables or { }
    local tcopy = {}
    if not tables[t] then
        tables[t] = tcopy
    end
    for i,v in next, t do -- brrr, what happens with sparse indexed
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

table.fastcopy = fastcopy
table.copy     = copy

-- rougly: copy-loop : unpack : sub == 0.9 : 0.4 : 0.45 (so in critical apps, use unpack)

function table.sub(t,i,j)
    return { unpack(t,i,j) }
end

function table.replace(a,b)
    for k,v in next, b do
        a[k] = v
    end
end

-- slower than #t on indexed tables (#t only returns the size of the numerically indexed slice)

function table.is_empty(t) -- obolete, use inline code instead
    return not t or not next(t)
end

function table.one_entry(t) -- obolete, use inline code instead
    local n = next(t)
    return n and not next(t,n)
end

--~ function table.starts_at(t) -- obsolete, not nice
--~     return ipairs(t,1)(t,0)
--~ end

function table.tohash(t,value)
    local h = { }
    if t then
        if value == nil then value = true end
        for _, v in next, t do -- no ipairs here
            h[v] = value
        end
    end
    return h
end

function table.fromhash(t)
    local h = { }
    for k, v in next, t do -- no ipairs here
        if v then h[#h+1] = k end
    end
    return h
end

--~ print(table.serialize(t), "\n")
--~ print(table.serialize(t,"name"), "\n")
--~ print(table.serialize(t,false), "\n")
--~ print(table.serialize(t,true), "\n")
--~ print(table.serialize(t,"name",true), "\n")
--~ print(table.serialize(t,"name",true,true), "\n")

table.serialize_functions = true
table.serialize_compact   = true
table.serialize_inline    = true

local noquotes, hexify, handle, reduce, compact, inline, functions

local reserved = table.tohash { -- intercept a language flaw, no reserved words as key
    'and', 'break', 'do', 'else', 'elseif', 'end', 'false', 'for', 'function', 'if',
    'in', 'local', 'nil', 'not', 'or', 'repeat', 'return', 'then', 'true', 'until', 'while',
}

local function simple_table(t)
    if #t > 0 then
        local n = 0
        for _,v in next, t do
            n = n + 1
        end
        if n == #t then
            local tt = { }
            for i=1,#t do
                local v = t[i]
                local tv = type(v)
                if tv == "number" then
                    if hexify then
                        tt[#tt+1] = format("0x%04X",v)
                    else
                        tt[#tt+1] = tostring(v) -- tostring not needed
                    end
                elseif tv == "boolean" then
                    tt[#tt+1] = tostring(v)
                elseif tv == "string" then
                    tt[#tt+1] = format("%q",v)
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

-- Because this is a core function of mkiv I moved some function calls
-- inline.
--
-- twice as fast in a test:
--
-- local propername = lpeg.P(lpeg.R("AZ","az","__") * lpeg.R("09","AZ","az", "__")^0 * lpeg.P(-1) )

-- problem: there no good number_to_string converter with the best resolution

local function do_serialize(root,name,depth,level,indexed)
    if level > 0 then
        depth = depth .. " "
        if indexed then
            handle(format("%s{",depth))
        elseif name then
        --~ handle(format("%s%s={",depth,key(name)))
            if type(name) == "number" then -- or find(k,"^%d+$") then
                if hexify then
                    handle(format("%s[0x%04X]={",depth,name))
                else
                    handle(format("%s[%s]={",depth,name))
                end
            elseif noquotes and not reserved[name] and find(name,"^%a[%w%_]*$") then
                handle(format("%s%s={",depth,name))
            else
                handle(format("%s[%q]={",depth,name))
            end
        else
            handle(format("%s{",depth))
        end
    end
    -- we could check for k (index) being number (cardinal)
    if root and next(root) then
        local first, last = nil, 0 -- #root cannot be trusted here (will be ok in 5.2 when ipairs is gone)
        if compact then
            -- NOT: for k=1,#root do (we need to quit at nil)
            for k,v in ipairs(root) do -- can we use next?
                if not first then first = k end
                last = last + 1
            end
        end
        local sk = sortedkeys(root)
        for i=1,#sk do
            local k = sk[i]
            local v = root[k]
            --~ if v == root then
                -- circular
            --~ else
            local t = type(v)
            if compact and first and type(k) == "number" and k >= first and k <= last then
                if t == "number" then
                    if hexify then
                        handle(format("%s 0x%04X,",depth,v))
                    else
                        handle(format("%s %s,",depth,v)) -- %.99g
                    end
                elseif t == "string" then
                    if reduce and tonumber(v) then
                        handle(format("%s %s,",depth,v))
                    else
                        handle(format("%s %q,",depth,v))
                    end
                elseif t == "table" then
                    if not next(v) then
                        handle(format("%s {},",depth))
                    elseif inline then -- and #t > 0
                        local st = simple_table(v)
                        if st then
                            handle(format("%s { %s },",depth,concat(st,", ")))
                        else
                            do_serialize(v,k,depth,level+1,true)
                        end
                    else
                        do_serialize(v,k,depth,level+1,true)
                    end
                elseif t == "boolean" then
                    handle(format("%s %s,",depth,tostring(v)))
                elseif t == "function" then
                    if functions then
                        handle(format('%s loadstring(%q),',depth,dump(v)))
                    else
                        handle(format('%s "function",',depth))
                    end
                else
                    handle(format("%s %q,",depth,tostring(v)))
                end
            elseif k == "__p__" then -- parent
                if false then
                    handle(format("%s __p__=nil,",depth))
                end
            elseif t == "number" then
            --~ if hexify then
            --~     handle(format("%s %s=0x%04X,",depth,key(k),v))
            --~ else
            --~     handle(format("%s %s=%s,",depth,key(k),v)) -- %.99g
            --~ end
                if type(k) == "number" then -- or find(k,"^%d+$") then
                    if hexify then
                        handle(format("%s [0x%04X]=0x%04X,",depth,k,v))
                    else
                        handle(format("%s [%s]=%s,",depth,k,v)) -- %.99g
                    end
                elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                    if hexify then
                        handle(format("%s %s=0x%04X,",depth,k,v))
                    else
                        handle(format("%s %s=%s,",depth,k,v)) -- %.99g
                    end
                else
                    if hexify then
                        handle(format("%s [%q]=0x%04X,",depth,k,v))
                    else
                        handle(format("%s [%q]=%s,",depth,k,v)) -- %.99g
                    end
                end
            elseif t == "string" then
                if reduce and tonumber(v) then
                --~ handle(format("%s %s=%s,",depth,key(k),v))
                    if type(k) == "number" then -- or find(k,"^%d+$") then
                        if hexify then
                            handle(format("%s [0x%04X]=%s,",depth,k,v))
                        else
                            handle(format("%s [%s]=%s,",depth,k,v))
                        end
                    elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                        handle(format("%s %s=%s,",depth,k,v))
                    else
                        handle(format("%s [%q]=%s,",depth,k,v))
                    end
                else
                --~ handle(format("%s %s=%q,",depth,key(k),v))
                    if type(k) == "number" then -- or find(k,"^%d+$") then
                        if hexify then
                            handle(format("%s [0x%04X]=%q,",depth,k,v))
                        else
                            handle(format("%s [%s]=%q,",depth,k,v))
                        end
                    elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                        handle(format("%s %s=%q,",depth,k,v))
                    else
                        handle(format("%s [%q]=%q,",depth,k,v))
                    end
                end
            elseif t == "table" then
                if not next(v) then
                    --~ handle(format("%s %s={},",depth,key(k)))
                    if type(k) == "number" then -- or find(k,"^%d+$") then
                        if hexify then
                            handle(format("%s [0x%04X]={},",depth,k))
                        else
                            handle(format("%s [%s]={},",depth,k))
                        end
                    elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                        handle(format("%s %s={},",depth,k))
                    else
                        handle(format("%s [%q]={},",depth,k))
                    end
                elseif inline then
                    local st = simple_table(v)
                    if st then
                    --~ handle(format("%s %s={ %s },",depth,key(k),concat(st,", ")))
                        if type(k) == "number" then -- or find(k,"^%d+$") then
                            if hexify then
                                handle(format("%s [0x%04X]={ %s },",depth,k,concat(st,", ")))
                            else
                                handle(format("%s [%s]={ %s },",depth,k,concat(st,", ")))
                            end
                        elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                            handle(format("%s %s={ %s },",depth,k,concat(st,", ")))
                        else
                            handle(format("%s [%q]={ %s },",depth,k,concat(st,", ")))
                        end
                    else
                        do_serialize(v,k,depth,level+1)
                    end
                else
                    do_serialize(v,k,depth,level+1)
                end
            elseif t == "boolean" then
            --~ handle(format("%s %s=%s,",depth,key(k),tostring(v)))
                if type(k) == "number" then -- or find(k,"^%d+$") then
                    if hexify then
                        handle(format("%s [0x%04X]=%s,",depth,k,tostring(v)))
                    else
                        handle(format("%s [%s]=%s,",depth,k,tostring(v)))
                    end
                elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                    handle(format("%s %s=%s,",depth,k,tostring(v)))
                else
                    handle(format("%s [%q]=%s,",depth,k,tostring(v)))
                end
            elseif t == "function" then
                if functions then
                    --~ handle(format('%s %s=loadstring(%q),',depth,key(k),dump(v)))
                    if type(k) == "number" then -- or find(k,"^%d+$") then
                        if hexify then
                            handle(format("%s [0x%04X]=loadstring(%q),",depth,k,dump(v)))
                        else
                            handle(format("%s [%s]=loadstring(%q),",depth,k,dump(v)))
                        end
                    elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                        handle(format("%s %s=loadstring(%q),",depth,k,dump(v)))
                    else
                        handle(format("%s [%q]=loadstring(%q),",depth,k,dump(v)))
                    end
                end
            else
                --~ handle(format("%s %s=%q,",depth,key(k),tostring(v)))
                if type(k) == "number" then -- or find(k,"^%d+$") then
                    if hexify then
                        handle(format("%s [0x%04X]=%q,",depth,k,tostring(v)))
                    else
                        handle(format("%s [%s]=%q,",depth,k,tostring(v)))
                    end
                elseif noquotes and not reserved[k] and find(k,"^%a[%w%_]*$") then
                    handle(format("%s %s=%q,",depth,k,tostring(v)))
                else
                    handle(format("%s [%q]=%q,",depth,k,tostring(v)))
                end
            end
            --~ end
        end
    end
   if level > 0 then
        handle(format("%s},",depth))
    end
end

-- replacing handle by a direct t[#t+1] = ... (plus test) is not much
-- faster (0.03 on 1.00 for zapfino.tma)

local function serialize(root,name,_handle,_reduce,_noquotes,_hexify)
    noquotes = _noquotes
    hexify = _hexify
    handle = _handle or print
    reduce = _reduce or false
    compact = table.serialize_compact
    inline  = compact and table.serialize_inline
    functions = table.serialize_functions
    local tname = type(name)
    if tname == "string" then
        if name == "return" then
            handle("return {")
        else
            handle(name .. "={")
        end
    elseif tname == "number" then
        if hexify then
            handle(format("[0x%04X]={",name))
        else
            handle("[" .. name .. "]={")
        end
    elseif tname == "boolean" then
        if name then
            handle("return {")
        else
            handle("{")
        end
    else
        handle("t={")
    end
    if root and next(root) then
        do_serialize(root,name,"",0,indexed)
    end
    handle("}")
end

--~ name:
--~
--~ true     : return     { }
--~ false    :            { }
--~ nil      : t        = { }
--~ string   : string   = { }
--~ 'return' : return     { }
--~ number   : [number] = { }

function table.serialize(root,name,reduce,noquotes,hexify)
    local t = { }
    local function flush(s)
        t[#t+1] = s
    end
    serialize(root,name,flush,reduce,noquotes,hexify)
    return concat(t,"\n")
end

function table.tohandle(handle,root,name,reduce,noquotes,hexify)
    serialize(root,name,handle,reduce,noquotes,hexify)
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

function table.tofile(filename,root,name,reduce,noquotes,hexify)
    local f = io.open(filename,'w')
    if f then
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
            serialize(root,name,flush,reduce,noquotes,hexify)
            f:write(concat(t,"\n"),"\n")
        else
            local function flush(s)
                f:write(s,"\n")
            end
            serialize(root,name,flush,reduce,noquotes,hexify)
        end
        f:close()
    end
end

local function flatten(t,f,complete) -- is this used? meybe a variant with next, ...
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

-- a better one:

local function flattened(t,f)
    if not f then
        f = { }
    end
    for k, v in next, t do
        if type(v) == "table" then
            flattened(v,f)
        else
            f[k] = v
        end
    end
    return f
end

table.flattened = flattened

-- the next three may disappear

function table.remove_value(t,value) -- todo: n
    if value then
        for i=1,#t do
            if t[i] == value then
                remove(t,i)
                -- remove all, so no: return
            end
        end
    end
end

function table.insert_before_value(t,value,str)
    if str then
        if value then
            for i=1,#t do
                if t[i] == value then
                    insert(t,i,str)
                    return
                end
            end
        end
        insert(t,1,str)
    elseif value then
        insert(t,1,value)
    end
end

function table.insert_after_value(t,value,str)
    if str then
        if value then
            for i=1,#t do
                if t[i] == value then
                    insert(t,i+1,str)
                    return
                end
            end
        end
        t[#t+1] = str
    elseif value then
        t[#t+1] = value
    end
end

local function are_equal(a,b,n,m) -- indexed
    if a and b and #a == #b then
        n = n or 1
        m = m or #a
        for i=n,m do
            local ai, bi = a[i], b[i]
            if ai==bi then
                -- same
            elseif type(ai)=="table" and type(bi)=="table" then
                if not are_equal(ai,bi) then
                    return false
                end
            else
                return false
            end
        end
        return true
    else
        return false
    end
end

local function identical(a,b) -- assumes same structure
    for ka, va in next, a do
        local vb = b[k]
        if va == vb then
            -- same
        elseif type(va) == "table" and  type(vb) == "table" then
            if not identical(va,vb) then
                return false
            end
        else
            return false
        end
    end
    return true
end

table.are_equal = are_equal
table.identical = identical

-- maybe also make a combined one

function table.compact(t)
    if t then
        for k,v in next, t do
            if not next(v) then
                t[k] = nil
            end
        end
    end
end

function table.contains(t, v)
    if t then
        for i=1, #t do
            if t[i] == v then
                return i
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
    for k, v in next, t do
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
    setmetatable(t, { __index = function(_,key) return p[key] end }) -- why not __index = p ?
    return t
end

function table.hexed(t,seperator)
    local tt = { }
    for i=1,#t do tt[i] = format("0x%04X",t[i]) end
    return concat(tt,seperator or " ")
end

function table.reverse_hash(h)
    local r = { }
    for k,v in next, h do
        r[v] = lower(gsub(k," ",""))
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

function table.insert_before_value(t,value,extra)
    for i=1,#t do
        if t[i] == extra then
            remove(t,i)
        end
    end
    for i=1,#t do
        if t[i] == value then
            insert(t,i,extra)
            return
        end
    end
    insert(t,1,extra)
end

function table.insert_after_value(t,value,extra)
    for i=1,#t do
        if t[i] == extra then
            remove(t,i)
        end
    end
    for i=1,#t do
        if t[i] == value then
            insert(t,i+1,extra)
            return
        end
    end
    insert(t,#t+1,extra)
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-io'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local byte, find, gsub = string.byte, string.find, string.gsub

if string.find(os.getenv("PATH"),";") then
    io.fileseparator, io.pathseparator = "\\", ";"
else
    io.fileseparator, io.pathseparator = "/" , ":"
end

function io.loaddata(filename,textmode)
    local f = io.open(filename,(textmode and 'r') or 'rb')
    if f then
    --  collectgarbage("step") -- sometimes makes a big difference in mem consumption
        local data = f:read('*all')
    --  garbagecollector.check(data)
        f:close()
        return data
    else
        return nil
    end
end

function io.savedata(filename,data,joiner)
    local f = io.open(filename,"wb")
    if f then
        if type(data) == "table" then
            f:write(table.join(data,joiner or ""))
        elseif type(data) == "function" then
            data(f)
        else
            f:write(data or "")
        end
        f:close()
        return true
    else
        return false
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

local nextbyte = {
    [4] = function(f)
        local a, b, c, d = f:read(1,1,1,1)
        if d then
            return byte(a), byte(b), byte(c), byte(d)
        else
            return nil, nil, nil, nil
        end
    end,
    [2] = function(f)
        local a, b = f:read(1,1)
        if b then
            return byte(a), byte(b)
        else
            return nil, nil
        end
    end,
    [1] = function (f)
        local a = f:read(1)
        if a then
            return byte(a)
        else
            return nil
        end
    end,
    [-2] = function (f)
        local a, b = f:read(1,1)
        if b then
            return byte(b), byte(a)
        else
            return nil, nil
        end
    end,
    [-4] = function(f)
        local a, b, c, d = f:read(1,1,1,1)
        if d then
            return byte(d), byte(c), byte(b), byte(a)
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
        answer = gsub(answer,"^%s*(.*)%s*$","%1")
        if answer == "" and default then
            return default
        elseif not options then
            return answer
        else
            for k=1,#options do
                if options[k] == answer then
                    return answer
                end
            end
            local pattern = "^" .. answer
            for k=1,#options do
                local v = options[k]
                if find(v,pattern) then
                    return v
                end
            end
        end
    end
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-number'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local tostring = tostring
local format, floor, insert, match = string.format, math.floor, table.insert, string.match
local lpegmatch = lpeg.match

number = number or { }

-- a,b,c,d,e,f = number.toset(100101)

function number.toset(n)
    return match(tostring(n),"(.?)(.?)(.?)(.?)(.?)(.?)(.?)(.?)")
end

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

local one = lpeg.C(1-lpeg.S(''))^1

function number.toset(n)
    return lpegmatch(one,tostring(n))
end

function number.bits(n,zero)
    local t, i = { }, (zero and 0) or 1
    while n > 0 do
        local m = n % 2
        if m > 0 then
            insert(t,1,i)
        end
        n = floor(n/2)
        i = i + 1
    end
    return t
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-set'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

set = set or { }

local nums   = { }
local tabs   = { }
local concat = table.concat
local next, type = next, type

set.create = table.tohash

function set.tonumber(t)
    if next(t) then
        local s = ""
    --  we could save mem by sorting, but it slows down
        for k, v in next, t do
            if v then
            --  why bother about the leading space
                s = s .. " " .. k
            end
        end
        local n = nums[s]
        if not n then
            n = #tabs + 1
            tabs[n] = t
            nums[s] = n
        end
        return n
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

function set.tolist(n)
    if n == 0 or not tabs[n] then
        return ""
    else
        local t = { }
        for k, v in next, tabs[n] do
            if v then
                t[#t+1] = k
            end
        end
        return concat(t," ")
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



end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-os'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- maybe build io.flush in os.execute

local find, format, gsub = string.find, string.format, string.gsub
local random, ceil = math.random, math.ceil

local execute, spawn, exec, ioflush = os.execute, os.spawn or os.execute, os.exec or os.execute, io.flush

function os.execute(...) ioflush() return execute(...) end
function os.spawn  (...) ioflush() return spawn  (...) end
function os.exec   (...) ioflush() return exec   (...) end

function os.resultof(command)
    ioflush() -- else messed up logging
    local handle = io.popen(command,"r")
    if not handle then
    --  print("unknown command '".. command .. "' in os.resultof")
        return ""
    else
        return handle:read("*all") or ""
    end
end

--~ os.type     : windows | unix (new, we already guessed os.platform)
--~ os.name     : windows | msdos | linux | macosx | solaris | .. | generic (new)
--~ os.platform : extended os.name with architecture

if not io.fileseparator then
    if find(os.getenv("PATH"),";") then
        io.fileseparator, io.pathseparator, os.type = "\\", ";", os.type or "mswin"
    else
        io.fileseparator, io.pathseparator, os.type = "/" , ":", os.type or "unix"
    end
end

os.type = os.type or (io.pathseparator == ";"       and "windows") or "unix"
os.name = os.name or (os.type          == "windows" and "mswin"  ) or "linux"

if os.type == "windows" then
    os.libsuffix, os.binsuffix = 'dll', 'exe'
else
    os.libsuffix, os.binsuffix = 'so', ''
end

function os.launch(str)
    if os.type == "windows" then
        os.execute("start " .. str) -- os.spawn ?
    else
        os.execute(str .. " &")     -- os.spawn ?
    end
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

local startuptime = os.gettimeofday()

function os.runtime()
    return os.gettimeofday() - startuptime
end

--~ print(os.gettimeofday()-os.time())
--~ os.sleep(1.234)
--~ print (">>",os.runtime())
--~ print(os.date("%H:%M:%S",os.gettimeofday()))
--~ print(os.date("%H:%M:%S",os.time()))

-- no need for function anymore as we have more clever code and helpers now
-- this metatable trickery might as well disappear

os.resolvers = os.resolvers or { }

local resolvers = os.resolvers

local osmt = getmetatable(os) or { __index = function(t,k) t[k] = "unset" return "unset" end } -- maybe nil
local osix = osmt.__index

osmt.__index = function(t,k)
    return (resolvers[k] or osix)(t,k)
end

setmetatable(os,osmt)

if not os.setenv then

    -- we still store them but they won't be seen in
    -- child processes although we might pass them some day
    -- using command concatination

    local env, getenv = { }, os.getenv

    function os.setenv(k,v)
        env[k] = v
    end

    function os.getenv(k)
        return env[k] or getenv(k)
    end

end

-- we can use HOSTTYPE on some platforms

local name, platform = os.name or "linux", os.getenv("MTX_PLATFORM") or ""

local function guess()
    local architecture = os.resultof("uname -m") or ""
    if architecture ~= "" then
        return architecture
    end
    architecture = os.getenv("HOSTTYPE") or ""
    if architecture ~= "" then
        return architecture
    end
    return os.resultof("echo $HOSTTYPE") or ""
end

if platform ~= "" then

    os.platform = platform

elseif os.type == "windows" then

    -- we could set the variable directly, no function needed here

    function os.resolvers.platform(t,k)
        local platform, architecture = "", os.getenv("PROCESSOR_ARCHITECTURE") or ""
        if find(architecture,"AMD64") then
            platform = "mswin-64"
        else
            platform = "mswin"
        end
        os.setenv("MTX_PLATFORM",platform)
        os.platform = platform
        return platform
    end

elseif name == "linux" then

    function os.resolvers.platform(t,k)
        -- we sometims have HOSTTYPE set so let's check that first
        local platform, architecture = "", os.getenv("HOSTTYPE") or os.resultof("uname -m") or ""
        if find(architecture,"x86_64") then
            platform = "linux-64"
        elseif find(architecture,"ppc") then
            platform = "linux-ppc"
        else
            platform = "linux"
        end
        os.setenv("MTX_PLATFORM",platform)
        os.platform = platform
        return platform
    end

elseif name == "macosx" then

    --[[
        Identifying the architecture of OSX is quite a mess and this
        is the best we can come up with. For some reason $HOSTTYPE is
        a kind of pseudo environment variable, not known to the current
        environment. And yes, uname cannot be trusted either, so there
        is a change that you end up with a 32 bit run on a 64 bit system.
        Also, some proper 64 bit intel macs are too cheap (low-end) and
        therefore not permitted to run the 64 bit kernel.
      ]]--

    function os.resolvers.platform(t,k)
     -- local platform, architecture = "", os.getenv("HOSTTYPE") or ""
     -- if architecture == "" then
     --     architecture = os.resultof("echo $HOSTTYPE") or ""
     -- end
        local platform, architecture = "", os.resultof("echo $HOSTTYPE") or ""
        if architecture == "" then
         -- print("\nI have no clue what kind of OSX you're running so let's assume an 32 bit intel.\n")
            platform = "osx-intel"
        elseif find(architecture,"i386") then
            platform = "osx-intel"
        elseif find(architecture,"x86_64") then
            platform = "osx-64"
        else
            platform = "osx-ppc"
        end
        os.setenv("MTX_PLATFORM",platform)
        os.platform = platform
        return platform
    end

elseif name == "sunos" then

    function os.resolvers.platform(t,k)
        local platform, architecture = "", os.resultof("uname -m") or ""
        if find(architecture,"sparc") then
            platform = "solaris-sparc"
        else -- if architecture == 'i86pc'
            platform = "solaris-intel"
        end
        os.setenv("MTX_PLATFORM",platform)
        os.platform = platform
        return platform
    end

elseif name == "freebsd" then

    function os.resolvers.platform(t,k)
        local platform, architecture = "", os.resultof("uname -m") or ""
        if find(architecture,"amd64") then
            platform = "freebsd-amd64"
        else
            platform = "freebsd"
        end
        os.setenv("MTX_PLATFORM",platform)
        os.platform = platform
        return platform
    end

elseif name == "kfreebsd" then

    function os.resolvers.platform(t,k)
        -- we sometims have HOSTTYPE set so let's check that first
        local platform, architecture = "", os.getenv("HOSTTYPE") or os.resultof("uname -m") or ""
        if find(architecture,"x86_64") then
            platform = "kfreebsd-64"
        else
            platform = "kfreebsd-i386"
        end
        os.setenv("MTX_PLATFORM",platform)
        os.platform = platform
        return platform
    end

else

    -- platform = "linux"
    -- os.setenv("MTX_PLATFORM",platform)
    -- os.platform = platform

    function os.resolvers.platform(t,k)
        local platform = "linux"
        os.setenv("MTX_PLATFORM",platform)
        os.platform = platform
        return platform
    end

end

-- beware, we set the randomseed

-- from wikipedia: Version 4 UUIDs use a scheme relying only on random numbers. This algorithm sets the
-- version number as well as two reserved bits. All other bits are set using a random or pseudorandom
-- data source. Version 4 UUIDs have the form xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx with hexadecimal
-- digits x and hexadecimal digits 8, 9, A, or B for y. e.g. f47ac10b-58cc-4372-a567-0e02b2c3d479.
--
-- as we don't call this function too often there is not so much risk on repetition

local t = { 8, 9, "a", "b" }

function os.uuid()
    return format("%04x%04x-4%03x-%s%03x-%04x-%04x%04x%04x",
        random(0xFFFF),random(0xFFFF),
        random(0x0FFF),
        t[ceil(random(4))] or 8,random(0x0FFF),
        random(0xFFFF),
        random(0xFFFF),random(0xFFFF),random(0xFFFF)
    )
end

local d

function os.timezone(delta)
    d = d or tonumber(tonumber(os.date("%H")-os.date("!%H")))
    if delta then
        if d > 0 then
            return format("+%02i:00",d)
        else
            return format("-%02i:00",-d)
        end
    else
        return 1
    end
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-file'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- needs a cleanup

file = file or { }

local concat = table.concat
local find, gmatch, match, gsub, sub, char = string.find, string.gmatch, string.match, string.gsub, string.sub, string.char
local lpegmatch = lpeg.match

function file.removesuffix(filename)
    return (gsub(filename,"%.[%a%d]+$",""))
end

function file.addsuffix(filename, suffix)
    if not suffix or suffix == "" then
        return filename
    elseif not find(filename,"%.[%a%d]+$") then
        return filename .. "." .. suffix
    else
        return filename
    end
end

function file.replacesuffix(filename, suffix)
    return (gsub(filename,"%.[%a%d]+$","")) .. "." .. suffix
end

function file.dirname(name,default)
    return match(name,"^(.+)[/\\].-$") or (default or "")
end

function file.basename(name)
    return match(name,"^.+[/\\](.-)$") or name
end

function file.nameonly(name)
    return (gsub(match(name,"^.+[/\\](.-)$") or name,"%..*$",""))
end

function file.extname(name,default)
    return match(name,"^.+%.([^/\\]-)$") or default or ""
end

file.suffix = file.extname

--~ function file.join(...)
--~     local pth = concat({...},"/")
--~     pth = gsub(pth,"\\","/")
--~     local a, b = match(pth,"^(.*://)(.*)$")
--~     if a and b then
--~         return a .. gsub(b,"//+","/")
--~     end
--~     a, b = match(pth,"^(//)(.*)$")
--~     if a and b then
--~         return a .. gsub(b,"//+","/")
--~     end
--~     return (gsub(pth,"//+","/"))
--~ end

local trick_1 = char(1)
local trick_2 = "^" .. trick_1 .. "/+"

function file.join(...)
    local lst = { ... }
    local a, b = lst[1], lst[2]
    if a == "" then
        lst[1] = trick_1
    elseif b and find(a,"^/+$") and find(b,"^/") then
        lst[1] = ""
        lst[2] = gsub(b,"^/+","")
    end
    local pth = concat(lst,"/")
    pth = gsub(pth,"\\","/")
    local a, b = match(pth,"^(.*://)(.*)$")
    if a and b then
        return a .. gsub(b,"//+","/")
    end
    a, b = match(pth,"^(//)(.*)$")
    if a and b then
        return a .. gsub(b,"//+","/")
    end
    pth = gsub(pth,trick_2,"")
    return (gsub(pth,"//+","/"))
end

--~ print(file.join("//","/y"))
--~ print(file.join("/","/y"))
--~ print(file.join("","/y"))
--~ print(file.join("/x/","/y"))
--~ print(file.join("x/","/y"))
--~ print(file.join("http://","/y"))
--~ print(file.join("http://a","/y"))
--~ print(file.join("http:///a","/y"))
--~ print(file.join("//nas-1","/y"))

function file.iswritable(name)
    local a = lfs.attributes(name) or lfs.attributes(file.dirname(name,"."))
    return a and sub(a.permissions,2,2) == "w"
end

function file.isreadable(name)
    local a = lfs.attributes(name)
    return a and sub(a.permissions,1,1) == "r"
end

file.is_readable = file.isreadable
file.is_writable = file.iswritable

-- todo: lpeg

--~ function file.split_path(str)
--~     local t = { }
--~     str = gsub(str,"\\", "/")
--~     str = gsub(str,"(%a):([;/])", "%1\001%2")
--~     for name in gmatch(str,"([^;:]+)") do
--~         if name ~= "" then
--~             t[#t+1] = gsub(name,"\001",":")
--~         end
--~     end
--~     return t
--~ end

local checkedsplit = string.checkedsplit

function file.split_path(str,separator)
    str = gsub(str,"\\","/")
    return checkedsplit(str,separator or io.pathseparator)
end

function file.join_path(tab)
    return concat(tab,io.pathseparator) -- can have trailing //
end

-- we can hash them weakly

function file.collapse_path(str)
    str = gsub(str,"\\","/")
    if find(str,"/") then
        str = gsub(str,"^%./",(gsub(lfs.currentdir(),"\\","/")) .. "/") -- ./xx in qualified
        str = gsub(str,"/%./","/")
        local n, m = 1, 1
        while n > 0 or m > 0 do
            str, n = gsub(str,"[^/%.]+/%.%.$","")
            str, m = gsub(str,"[^/%.]+/%.%./","")
        end
        str = gsub(str,"([^/])/$","%1")
    --  str = gsub(str,"^%./","") -- ./xx in qualified
        str = gsub(str,"/%.$","")
    end
    if str == "" then str = "." end
    return str
end

--~ print(file.collapse_path("/a"))
--~ print(file.collapse_path("a/./b/.."))
--~ print(file.collapse_path("a/aa/../b/bb"))
--~ print(file.collapse_path("a/../.."))
--~ print(file.collapse_path("a/.././././b/.."))
--~ print(file.collapse_path("a/./././b/.."))
--~ print(file.collapse_path("a/b/c/../.."))

function file.robustname(str)
    return (gsub(str,"[^%a%d%/%-%.\\]+","-"))
end

file.readdata = io.loaddata
file.savedata = io.savedata

function file.copy(oldname,newname)
    file.savedata(newname,io.loaddata(oldname))
end

-- lpeg variants, slightly faster, not always

--~ local period    = lpeg.P(".")
--~ local slashes   = lpeg.S("\\/")
--~ local noperiod  = 1-period
--~ local noslashes = 1-slashes
--~ local name      = noperiod^1

--~ local pattern = (noslashes^0 * slashes)^0 * (noperiod^1 * period)^1 * lpeg.C(noperiod^1) * -1

--~ function file.extname(name)
--~     return lpegmatch(pattern,name) or ""
--~ end

--~ local pattern = lpeg.Cs(((period * noperiod^1 * -1)/"" + 1)^1)

--~ function file.removesuffix(name)
--~     return lpegmatch(pattern,name)
--~ end

--~ local pattern = (noslashes^0 * slashes)^1 * lpeg.C(noslashes^1) * -1

--~ function file.basename(name)
--~     return lpegmatch(pattern,name) or name
--~ end

--~ local pattern = (noslashes^0 * slashes)^1 * lpeg.Cp() * noslashes^1 * -1

--~ function file.dirname(name)
--~     local p = lpegmatch(pattern,name)
--~     if p then
--~         return sub(name,1,p-2)
--~     else
--~         return ""
--~     end
--~ end

--~ local pattern = (noslashes^0 * slashes)^0 * (noperiod^1 * period)^1 * lpeg.Cp() * noperiod^1 * -1

--~ function file.addsuffix(name, suffix)
--~     local p = lpegmatch(pattern,name)
--~     if p then
--~         return name
--~     else
--~         return name .. "." .. suffix
--~     end
--~ end

--~ local pattern = (noslashes^0 * slashes)^0 * (noperiod^1 * period)^1 * lpeg.Cp() * noperiod^1 * -1

--~ function file.replacesuffix(name,suffix)
--~     local p = lpegmatch(pattern,name)
--~     if p then
--~         return sub(name,1,p-2) .. "." .. suffix
--~     else
--~         return name .. "." .. suffix
--~     end
--~ end

--~ local pattern = (noslashes^0 * slashes)^0 * lpeg.Cp() * ((noperiod^1 * period)^1 * lpeg.Cp() + lpeg.P(true)) * noperiod^1 * -1

--~ function file.nameonly(name)
--~     local a, b = lpegmatch(pattern,name)
--~     if b then
--~         return sub(name,a,b-2)
--~     elseif a then
--~         return sub(name,a)
--~     else
--~         return name
--~     end
--~ end

--~ local test = file.extname
--~ local test = file.basename
--~ local test = file.dirname
--~ local test = file.addsuffix
--~ local test = file.replacesuffix
--~ local test = file.nameonly

--~ print(1,test("./a/b/c/abd.def.xxx","!!!"))
--~ print(2,test("./../b/c/abd.def.xxx","!!!"))
--~ print(3,test("a/b/c/abd.def.xxx","!!!"))
--~ print(4,test("a/b/c/def.xxx","!!!"))
--~ print(5,test("a/b/c/def","!!!"))
--~ print(6,test("def","!!!"))
--~ print(7,test("def.xxx","!!!"))

--~ local tim = os.clock() for i=1,250000 do local ext = test("abd.def.xxx","!!!") end print(os.clock()-tim)

-- also rewrite previous

local letter    = lpeg.R("az","AZ") + lpeg.S("_-+")
local separator = lpeg.P("://")

local qualified = lpeg.P(".")^0 * lpeg.P("/") + letter*lpeg.P(":") + letter^1*separator + letter^1 * lpeg.P("/")
local rootbased = lpeg.P("/") + letter*lpeg.P(":")

-- ./name ../name  /name c: :// name/name

function file.is_qualified_path(filename)
    return lpegmatch(qualified,filename) ~= nil
end

function file.is_rootbased_path(filename)
    return lpegmatch(rootbased,filename) ~= nil
end

local slash  = lpeg.S("\\/")
local period = lpeg.P(".")
local drive  = lpeg.C(lpeg.R("az","AZ")) * lpeg.P(":")
local path   = lpeg.C(((1-slash)^0 * slash)^0)
local suffix = period * lpeg.C(lpeg.P(1-period)^0 * lpeg.P(-1))
local base   = lpeg.C((1-suffix)^0)

local pattern = (drive + lpeg.Cc("")) * (path + lpeg.Cc("")) * (base + lpeg.Cc("")) * (suffix + lpeg.Cc(""))

function file.splitname(str) -- returns drive, path, base, suffix
    return lpegmatch(pattern,str)
end

-- function test(t) for k, v in next, t do print(v, "=>", file.splitname(v)) end end
--
-- test { "c:", "c:/aa", "c:/aa/bb", "c:/aa/bb/cc", "c:/aa/bb/cc.dd", "c:/aa/bb/cc.dd.ee" }
-- test { "c:", "c:aa", "c:aa/bb", "c:aa/bb/cc", "c:aa/bb/cc.dd", "c:aa/bb/cc.dd.ee" }
-- test { "/aa", "/aa/bb", "/aa/bb/cc", "/aa/bb/cc.dd", "/aa/bb/cc.dd.ee" }
-- test { "aa", "aa/bb", "aa/bb/cc", "aa/bb/cc.dd", "aa/bb/cc.dd.ee" }

--~ -- todo:
--~
--~ if os.type == "windows" then
--~     local currentdir = lfs.currentdir
--~     function lfs.currentdir()
--~         return (gsub(currentdir(),"\\","/"))
--~     end
--~ end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-md5'] = {
    version   = 1.001,
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- This also provides file checksums and checkers.

local gsub, format, byte = string.gsub, string.format, string.byte

local function convert(str,fmt)
    return (gsub(md5.sum(str),".",function(chr) return format(fmt,byte(chr)) end))
end

if not md5.HEX then function md5.HEX(str) return convert(str,"%02X") end end
if not md5.hex then function md5.hex(str) return convert(str,"%02x") end end
if not md5.dec then function md5.dec(str) return convert(str,"%03i") end end

--~ if not md5.HEX then
--~     local function remap(chr) return format("%02X",byte(chr)) end
--~     function md5.HEX(str) return (gsub(md5.sum(str),".",remap)) end
--~ end
--~ if not md5.hex then
--~     local function remap(chr) return format("%02x",byte(chr)) end
--~     function md5.hex(str) return (gsub(md5.sum(str),".",remap)) end
--~ end
--~ if not md5.dec then
--~     local function remap(chr) return format("%03i",byte(chr)) end
--~     function md5.dec(str) return (gsub(md5.sum(str),".",remap)) end
--~ end

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

function file.checksum(name)
    if md5 then
        local data = io.loaddata(name)
        if data then
            return md5.HEX(data)
        end
    end
    return nil
end

function file.loadchecksum(name)
    if md5 then
        local data = io.loaddata(name .. ".md5")
        return data and (gsub(data,"%s",""))
    end
    return nil
end

function file.savechecksum(name, checksum)
    if not checksum then checksum = file.checksum(name) end
    if checksum then
        io.savedata(name .. ".md5",checksum)
        return checksum
    end
    return nil
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-url'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local char, gmatch, gsub = string.char, string.gmatch, string.gsub
local tonumber, type = tonumber, type
local lpegmatch = lpeg.match

-- from the spec (on the web):
--
--     foo://example.com:8042/over/there?name=ferret#nose
--     \_/   \______________/\_________/ \_________/ \__/
--      |           |            |            |        |
--   scheme     authority       path        query   fragment
--      |   _____________________|__
--     / \ /                        \
--     urn:example:animal:ferret:nose

url = url or { }

local function tochar(s)
    return char(tonumber(s,16))
end

local colon, qmark, hash, slash, percent, endofstring = lpeg.P(":"), lpeg.P("?"), lpeg.P("#"), lpeg.P("/"), lpeg.P("%"), lpeg.P(-1)

local hexdigit  = lpeg.R("09","AF","af")
local plus      = lpeg.P("+")
local escaped   = (plus / " ") + (percent * lpeg.C(hexdigit * hexdigit) / tochar)

-- we assume schemes with more than 1 character (in order to avoid problems with windows disks)

local scheme    =                 lpeg.Cs((escaped+(1-colon-slash-qmark-hash))^2) * colon + lpeg.Cc("")
local authority = slash * slash * lpeg.Cs((escaped+(1-      slash-qmark-hash))^0)         + lpeg.Cc("")
local path      = slash *         lpeg.Cs((escaped+(1-            qmark-hash))^0)         + lpeg.Cc("")
local query     = qmark         * lpeg.Cs((escaped+(1-                  hash))^0)         + lpeg.Cc("")
local fragment  = hash          * lpeg.Cs((escaped+(1-           endofstring))^0)         + lpeg.Cc("")

local parser = lpeg.Ct(scheme * authority * path * query * fragment)

-- todo: reconsider Ct as we can as well have five return values (saves a table)
-- so we can have two parsers, one with and one without

function url.split(str)
    return (type(str) == "string" and lpegmatch(parser,str)) or str
end

-- todo: cache them

function url.hashed(str)
    local s = url.split(str)
    local somescheme = s[1] ~= ""
    return {
        scheme    = (somescheme and s[1]) or "file",
        authority = s[2],
        path      = s[3],
        query     = s[4],
        fragment  = s[5],
        original  = str,
        noscheme  = not somescheme,
    }
end

function url.hasscheme(str)
    return url.split(str)[1] ~= ""
end

function url.addscheme(str,scheme)
    return (url.hasscheme(str) and str) or ((scheme or "file:///") .. str)
end

function url.construct(hash)
    local fullurl = hash.sheme .. "://".. hash.authority .. hash.path
    if hash.query then
        fullurl = fullurl .. "?".. hash.query
    end
    if hash.fragment then
        fullurl = fullurl .. "?".. hash.fragment
    end
    return fullurl
end

function url.filename(filename)
    local t = url.hashed(filename)
    return (t.scheme == "file" and (gsub(t.path,"^/([a-zA-Z])([:|])/)","%1:"))) or filename
end

function url.query(str)
    if type(str) == "string" then
        local t = { }
        for k, v in gmatch(str,"([^&=]*)=([^&=]*)") do
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

--~ from the spec on the web (sort of):
--~
--~ function test(str)
--~     print(table.serialize(url.hashed(str)))
--~ end
--~
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


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-dir'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- dir.expand_name will be merged with cleanpath and collapsepath

local type = type
local find, gmatch, match, gsub = string.find, string.gmatch, string.match, string.gsub
local lpegmatch = lpeg.match

dir = dir or { }

-- handy

function dir.current()
    return (gsub(lfs.currentdir(),"\\","/"))
end

-- optimizing for no string.find (*) does not save time

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
        if not find(path,"/$") then path = path .. '/' end
        for name in scanner do
            local full = path .. name
            local mode = attributes(full,'mode')
            if mode == 'file' then
                if find(full,patt) then
                    action(full)
                end
            elseif recurse and (mode == "directory") and (name ~= '.') and (name ~= "..") then
                glob_pattern(full,patt,recurse,action)
            end
        end
    end
end

dir.glob_pattern = glob_pattern

local function collect_pattern(path,patt,recurse,result)
    local ok, scanner
    result = result or { }
    if path == "/" then
        ok, scanner = xpcall(function() return walkdir(path..".") end, function() end) -- kepler safe
    else
        ok, scanner = xpcall(function() return walkdir(path)      end, function() end) -- kepler safe
    end
    if ok and type(scanner) == "function" then
        if not find(path,"/$") then path = path .. '/' end
        for name in scanner do
            local full = path .. name
            local attr = attributes(full)
            local mode = attr.mode
            if mode == 'file' then
                if find(full,patt) then
                    result[name] = attr
                end
            elseif recurse and (mode == "directory") and (name ~= '.') and (name ~= "..") then
                attr.list = collect_pattern(full,patt,recurse)
                result[name] = attr
            end
        end
    end
    return result
end

dir.collect_pattern = collect_pattern

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
    if type(t) == "function" then
        if type(str) == "table" then
            for s=1,#str do
                glob(str[s],t)
            end
        elseif lfs.isfile(str) then
            t(str)
        else
            local split = lpegmatch(pattern,str)
            if split then
                local root, path, base = split[1], split[2], split[3]
                local recurse = find(base,"%*%*")
                local start = root .. path
                local result = lpegmatch(filter,start .. base)
                glob_pattern(start,result,recurse,t)
            end
        end
    else
        if type(str) == "table" then
            local t = t or { }
            for s=1,#str do
                glob(str[s],t)
            end
            return t
        elseif lfs.isfile(str) then
            local t = t or { }
            t[#t+1] = str
            return t
        else
            local split = lpegmatch(pattern,str)
            if split then
                local t = t or { }
                local action = action or function(name) t[#t+1] = name end
                local root, path, base = split[1], split[2], split[3]
                local recurse = find(base,"%*%*")
                local start = root .. path
                local result = lpegmatch(filter,start .. base)
                glob_pattern(start,result,recurse,action)
                return t
            else
                return { }
            end
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
        func = function(name) return find(name,s) end
    end
    files = files or { }
    for name in walkdir(path) do
        if find(name,"^%.") then
            --- skip
        else
            local mode = attributes(name,'mode')
            if mode == "directory" then
                if recurse then
                    globfiles(path .. "/" .. name,recurse,func,files)
                end
            elseif mode == "file" then
                if func then
                    if func(name) then
                        files[#files+1] = path .. "/" .. name
                    end
                else
                    files[#files+1] = path .. "/" .. name
                end
            end
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

if string.find(os.getenv("PATH"),";") then -- os.type == "windows"

    function dir.mkdirs(...)
        local str, pth, t = "", "", { ... }
        for i=1,#t do
            local s = t[i]
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
        first, middle, last = match(str,"^(//)(//*)(.*)$")
        if first then
            -- empty network path == local path
        else
            first, last = match(str,"^(//)/*(.-)$")
            if first then
                middle, last = match(str,"([^/]+)/+(.-)$")
                if middle then
                    pth = "//" .. middle
                else
                    pth = "//" .. last
                    last = ""
                end
            else
                first, middle, last = match(str,"^([a-zA-Z]:)(/*)(.-)$")
                if first then
                    pth, drive = first .. middle, true
                else
                    middle, last = match(str,"^(/*)(.-)$")
                    if not middle then
                        last = str
                    end
                end
            end
        end
        for s in gmatch(last,"[^/]+") do
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

    function dir.expand_name(str) -- will be merged with cleanpath and collapsepath
        local first, nothing, last = match(str,"^(//)(//*)(.*)$")
        if first then
            first = dir.current() .. "/"
        end
        if not first then
            first, last = match(str,"^(//)/*(.*)$")
        end
        if not first then
            first, last = match(str,"^([a-zA-Z]:)(.*)$")
            if first and not find(last,"^/") then
                local d = lfs.currentdir()
                if lfs.chdir(first) then
                    first = dir.current()
                end
                lfs.chdir(d)
            end
        end
        if not first then
            first, last = dir.current(), str
        end
        last = gsub(last,"//","/")
        last = gsub(last,"/%./","/")
        last = gsub(last,"^/*","")
        first = gsub(first,"/*$","")
        if last == "" then
            return first
        else
            return first .. "/" .. last
        end
    end

else

    function dir.mkdirs(...)
        local str, pth, t = "", "", { ... }
        for i=1,#t do
            local s = t[i]
            if s ~= "" then
                if str ~= "" then
                    str = str .. "/" .. s
                else
                    str = s
                end
            end
        end
        str = gsub(str,"/+","/")
        if find(str,"^/") then
            pth = "/"
            for s in gmatch(str,"[^/]+") do
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
            for s in gmatch(str,"[^/]+") do
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

    function dir.expand_name(str) -- will be merged with cleanpath and collapsepath
        if not find(str,"^/") then
            str = lfs.currentdir() .. "/" .. str
        end
        str = gsub(str,"//","/")
        str = gsub(str,"/%./","/")
        return str
    end

end

dir.makedirs = dir.mkdirs


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-boolean'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

boolean = boolean or { }

local type, tonumber = type, tonumber

function boolean.tonumber(b)
    if b then return 1 else return 0 end
end

function toboolean(str,tolerant)
    if tolerant then
        local tstr = type(str)
        if tstr == "string" then
            return str == "true" or str == "yes" or str == "on" or str == "1" or str == "t"
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
        if str == "true" or str == "yes" or str == "on" or str == "t" then
            return true
        elseif str == "false" or str == "no" or str == "off" or str == "f" then
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


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-unicode'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

if not unicode then

    unicode = { utf8 = { } }

    local floor, char = math.floor, string.char

    function unicode.utf8.utfchar(n)
        if n < 0x80 then
            return char(n)
        elseif n < 0x800 then
            return char(0xC0 + floor(n/0x40))  .. char(0x80 + (n % 0x40))
        elseif n < 0x10000 then
            return char(0xE0 + floor(n/0x1000)) .. char(0x80 + (floor(n/0x40) % 0x40)) .. char(0x80 + (n % 0x40))
        elseif n < 0x40000 then
            return char(0xF0 + floor(n/0x40000)) .. char(0x80 + floor(n/0x1000)) .. char(0x80 + (floor(n/0x40) % 0x40)) .. char(0x80 + (n % 0x40))
        else -- wrong:
          -- return char(0xF1 + floor(n/0x1000000)) .. char(0x80 + floor(n/0x40000)) .. char(0x80 + floor(n/0x1000)) .. char(0x80 + (floor(n/0x40) % 0x40)) .. char(0x80 + (n % 0x40))
            return "?"
        end
    end

end

utf = utf or unicode.utf8

local concat, utfchar, utfgsub = table.concat, utf.char, utf.gsub
local char, byte, find, bytepairs = string.char, string.byte, string.find, string.bytepairs

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

-- \000 fails in <= 5.0 but is valid in >=5.1 where %z is depricated

function unicode.utftype(f)
    local str = f:read(4)
    if not str then
        f:seek('set')
        return 0
 -- elseif find(str,"^%z%z\254\255") then            -- depricated
 -- elseif find(str,"^\000\000\254\255") then        -- not permitted and bugged
    elseif find(str,"\000\000\254\255",1,true) then  -- seems to work okay (TH)
        return 4
 -- elseif find(str,"^\255\254%z%z") then            -- depricated
 -- elseif find(str,"^\255\254\000\000") then        -- not permitted and bugged
    elseif find(str,"\255\254\000\000",1,true) then  -- seems to work okay (TH)
        return 3
    elseif find(str,"^\254\255") then
        f:seek('set',2)
        return 2
    elseif find(str,"^\255\254") then
        f:seek('set',2)
        return 1
    elseif find(str,"^\239\187\191") then
        f:seek('set',3)
        return 0
    else
        f:seek('set')
        return 0
    end
end

function unicode.utf16_to_utf8(str, endian) -- maybe a gsub is faster or an lpeg
    local result, tmp, n, m, p = { }, { }, 0, 0, 0
    -- lf | cr | crlf / (cr:13, lf:10)
    local function doit()
        if n == 10 then
            if p ~= 13 then
                result[#result+1] = concat(tmp)
                tmp = { }
                p = 0
            end
        elseif n == 13 then
            result[#result+1] = concat(tmp)
            tmp = { }
            p = n
        else
            tmp[#tmp+1] = utfchar(n)
            p = 0
        end
    end
    for l,r in bytepairs(str) do
        if r then
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
    end
    if #tmp > 0 then
        result[#result+1] = concat(tmp)
    end
    return result
end

function unicode.utf32_to_utf8(str, endian)
    local result = { }
    local tmp, n, m, p = { }, 0, -1, 0
    -- lf | cr | crlf / (cr:13, lf:10)
    local function doit()
        if n == 10 then
            if p ~= 13 then
                result[#result+1] = concat(tmp)
                tmp = { }
                p = 0
            end
        elseif n == 13 then
            result[#result+1] = concat(tmp)
            tmp = { }
            p = n
        else
            tmp[#tmp+1] = utfchar(n)
            p = 0
        end
    end
    for a,b in bytepairs(str) do
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
        result[#result+1] = concat(tmp)
    end
    return result
end

local function little(c)
    local b = byte(c) -- b = c:byte()
    if b < 0x10000 then
        return char(b%256,b/256)
    else
        b = b - 0x10000
        local b1, b2 = b/1024 + 0xD800, b%1024 + 0xDC00
        return char(b1%256,b1/256,b2%256,b2/256)
    end
end

local function big(c)
    local b = byte(c)
    if b < 0x10000 then
        return char(b/256,b%256)
    else
        b = b - 0x10000
        local b1, b2 = b/1024 + 0xD800, b%1024 + 0xDC00
        return char(b1/256,b1%256,b2/256,b2%256)
    end
end

function unicode.utf8_to_utf16(str,littleendian)
    if littleendian then
        return char(255,254) .. utfgsub(str,".",little)
    else
        return char(254,255) .. utfgsub(str,".",big)
    end
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-math'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local floor, sin, cos, tan = math.floor, math.sin, math.cos, math.tan

if not math.round then
    function math.round(x)
        return floor(x + 0.5)
    end
end

if not math.div then
    function math.div(n,m)
        return floor(n/m)
    end
end

if not math.mod then
    function math.mod(n,m)
        return n % m
    end
end

local pipi = 2*math.pi/360

function math.sind(d)
    return sin(d*pipi)
end

function math.cosd(d)
    return cos(d*pipi)
end

function math.tand(d)
    return tan(d*pipi)
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-utils'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- hm, quite unreadable

local gsub = string.gsub
local concat = table.concat
local type, next = type, next

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

utils.merger.strip_comment = true

function utils.merger._self_load_(name)
    local f, data = io.open(name), ""
    if f then
        utils.report("reading merge from %s",name)
        data = f:read("*all")
        f:close()
    else
        utils.report("unknown file to merge %s",name)
    end
    if data and utils.merger.strip_comment then
        -- saves some 20K
        data = gsub(data,"%-%-~[^\n\r]*[\r\n]", "")
    end
    return data or ""
end

function utils.merger._self_save_(name, data)
    if data ~= "" then
        local f = io.open(name,'w')
        if f then
            utils.report("saving merge from %s",name)
            f:write(data)
            f:close()
        end
    end
end

function utils.merger._self_swap_(data,code)
    if data ~= "" then
        return (gsub(data,utils.merger.pattern, function(s)
            return "\n\n" .. "-- "..utils.merger.m_begin .. "\n" .. code .. "\n" .. "-- "..utils.merger.m_end .. "\n\n"
        end, 1))
    else
        return ""
    end
end

--~ stripper:
--~
--~ data = gsub(data,"%-%-~[^\n]*\n","")
--~ data = gsub(data,"\n\n+","\n")

function utils.merger._self_libs_(libs,list)
    local result, f, frozen = { }, nil, false
    result[#result+1] = "\n"
    if type(libs) == 'string' then libs = { libs } end
    if type(list) == 'string' then list = { list } end
    local foundpath = nil
    for i=1,#libs do
        local lib = libs[i]
        for j=1,#list do
            local pth = gsub(list[j],"\\","/") -- file.clean_path
            utils.report("checking library path %s",pth)
            local name = pth .. "/" .. lib
            if lfs.isfile(name) then
                foundpath = pth
            end
        end
        if foundpath then break end
    end
    if foundpath then
        utils.report("using library path %s",foundpath)
        local right, wrong = { }, { }
        for i=1,#libs do
            local lib = libs[i]
            local fullname = foundpath .. "/" .. lib
            if lfs.isfile(fullname) then
            --  right[#right+1] = lib
                utils.report("merging library %s",fullname)
                result[#result+1] = "do -- create closure to overcome 200 locals limit"
                result[#result+1] = io.loaddata(fullname,true)
                result[#result+1] = "end -- of closure"
            else
            --  wrong[#wrong+1] = lib
                utils.report("no library %s",fullname)
            end
        end
        if #right > 0 then
            utils.report("merged libraries: %s",concat(right," "))
        end
        if #wrong > 0 then
            utils.report("skipped libraries: %s",concat(wrong," "))
        end
    else
        utils.report("no valid library path found")
    end
    return concat(result, "\n\n")
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

function utils.lua.compile(luafile, lucfile, cleanup, strip) -- defaults: cleanup=false strip=true
 -- utils.report("compiling",luafile,"into",lucfile)
    os.remove(lucfile)
    local command = "-o " .. string.quote(lucfile) .. " " .. string.quote(luafile)
    if strip ~= false then
        command = "-s " .. command
    end
    local done = (os.spawn("texluac " .. command) == 0) or (os.spawn("luac " .. command) == 0)
    if done and cleanup == true and lfs.isfile(lucfile) and lfs.isfile(luafile) then
     -- utils.report("removing",luafile)
        os.remove(luafile)
    end
    return done
end



end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['l-aux'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- for inline, no store split : for s in string.gmatch(str,",* *([^,]+)") do .. end

aux = aux or { }

local concat, format, gmatch = table.concat, string.format, string.gmatch
local tostring, type = tostring, type
local lpegmatch = lpeg.match

local P, R, V = lpeg.P, lpeg.R, lpeg.V

local escape, left, right = P("\\"), P('{'), P('}')

lpeg.patterns.balanced = P {
    [1] = ((escape * (left+right)) + (1 - (left+right)) + V(2))^0,
    [2] = left * V(1) * right
}

local space     = lpeg.P(' ')
local equal     = lpeg.P("=")
local comma     = lpeg.P(",")
local lbrace    = lpeg.P("{")
local rbrace    = lpeg.P("}")
local nobrace   = 1 - (lbrace+rbrace)
local nested    = lpeg.P { lbrace * (nobrace + lpeg.V(1))^0 * rbrace }
local spaces    = space^0

local value     = lpeg.P(lbrace * lpeg.C((nobrace + nested)^0) * rbrace) + lpeg.C((nested + (1-comma))^0)

local key       = lpeg.C((1-equal-comma)^1)
local pattern_a = (space+comma)^0 * (key * equal * value + key * lpeg.C(""))
local pattern_c = (space+comma)^0 * (key * equal * value)

local key       = lpeg.C((1-space-equal-comma)^1)
local pattern_b = spaces * comma^0 * spaces * (key * ((spaces * equal * spaces * value) + lpeg.C("")))

-- "a=1, b=2, c=3, d={a{b,c}d}, e=12345, f=xx{a{b,c}d}xx, g={}" : outer {} removes, leading spaces ignored

local hash = { }

local function set(key,value) -- using Carg is slower here
    hash[key] = value
end

local pattern_a_s = (pattern_a/set)^1
local pattern_b_s = (pattern_b/set)^1
local pattern_c_s = (pattern_c/set)^1

aux.settings_to_hash_pattern_a = pattern_a_s
aux.settings_to_hash_pattern_b = pattern_b_s
aux.settings_to_hash_pattern_c = pattern_c_s

function aux.make_settings_to_hash_pattern(set,how)
    if how == "strict" then
        return (pattern_c/set)^1
    elseif how == "tolerant" then
        return (pattern_b/set)^1
    else
        return (pattern_a/set)^1
    end
end

function aux.settings_to_hash(str,existing)
    if str and str ~= "" then
        hash = existing or { }
        if moretolerant then
            lpegmatch(pattern_b_s,str)
        else
            lpegmatch(pattern_a_s,str)
        end
        return hash
    else
        return { }
    end
end

function aux.settings_to_hash_tolerant(str,existing)
    if str and str ~= "" then
        hash = existing or { }
        lpegmatch(pattern_b_s,str)
        return hash
    else
        return { }
    end
end

function aux.settings_to_hash_strict(str,existing)
    if str and str ~= "" then
        hash = existing or { }
        lpegmatch(pattern_c_s,str)
        return next(hash) and hash
    else
        return nil
    end
end

local separator = comma * space^0
local value     = lpeg.P(lbrace * lpeg.C((nobrace + nested)^0) * rbrace) + lpeg.C((nested + (1-comma))^0)
local pattern   = lpeg.Ct(value*(separator*value)^0)

-- "aap, {noot}, mies" : outer {} removes, leading spaces ignored

aux.settings_to_array_pattern = pattern

-- we could use a weak table as cache

function aux.settings_to_array(str)
    if not str or str == "" then
        return { }
    else
        return lpegmatch(pattern,str)
    end
end

local function set(t,v)
    t[#t+1] = v
end

local value   = lpeg.P(lpeg.Carg(1)*value) / set
local pattern = value*(separator*value)^0 * lpeg.Carg(1)

function aux.add_settings_to_array(t,str)
    return lpegmatch(pattern,str,nil,t)
end

function aux.hash_to_string(h,separator,yes,no,strict,omit)
    if h then
        local t, s = { }, table.sortedkeys(h)
        omit = omit and table.tohash(omit)
        for i=1,#s do
            local key = s[i]
            if not omit or not omit[key] then
                local value = h[key]
                if type(value) == "boolean" then
                    if yes and no then
                        if value then
                            t[#t+1] = key .. '=' .. yes
                        elseif not strict then
                            t[#t+1] = key .. '=' .. no
                        end
                    elseif value or not strict then
                        t[#t+1] = key .. '=' .. tostring(value)
                    end
                else
                    t[#t+1] = key .. '=' .. value
                end
            end
        end
        return concat(t,separator or ",")
    else
        return ""
    end
end

function aux.array_to_string(a,separator)
    if a then
        return concat(a,separator or ",")
    else
        return ""
    end
end

function aux.settings_to_set(str,t)
    t = t or { }
    for s in gmatch(str,"%s*([^,]+)") do
        t[s] = true
    end
    return t
end

local value     = lbrace * lpeg.C((nobrace + nested)^0) * rbrace
local pattern   = lpeg.Ct((space + value)^0)

function aux.arguments_to_table(str)
    return lpegmatch(pattern,str)
end

-- temporary here

function aux.getparameters(self,class,parentclass,settings)
    local sc = self[class]
    if not sc then
        sc = table.clone(self[parent])
        self[class] = sc
    end
    aux.settings_to_hash(settings,sc)
end

-- temporary here

local digit         = lpeg.R("09")
local period        = lpeg.P(".")
local zero          = lpeg.P("0")
local trailingzeros = zero^0 * -digit -- suggested by Roberto R
local case_1        = period * trailingzeros / ""
local case_2        = period * (digit - trailingzeros)^1 * (trailingzeros / "")
local number        = digit^1 * (case_1 + case_2)
local stripper      = lpeg.Cs((number + 1)^0)

--~ local sample = "bla 11.00 bla 11 bla 0.1100 bla 1.00100 bla 0.00 bla 0.001 bla 1.1100 bla 0.100100100 bla 0.00100100100"
--~ collectgarbage("collect")
--~ str = string.rep(sample,10000)
--~ local ts = os.clock()
--~ lpegmatch(stripper,str)
--~ print(#str, os.clock()-ts, lpegmatch(stripper,sample))

lpeg.patterns.strip_zeros = stripper

function aux.strip_zeros(str)
    return lpegmatch(stripper,str)
end

function aux.definetable(target) -- defines undefined tables
    local composed, t = nil, { }
    for name in gmatch(target,"([^%.]+)") do
        if composed then
            composed = composed .. "." .. name
        else
            composed = name
        end
        t[#t+1] = format("%s = %s or { }",composed,composed)
    end
    return concat(t,"\n")
end

function aux.accesstable(target)
    local t = _G
    for name in gmatch(target,"([^%.]+)") do
        t = t[name]
    end
    return t
end

-- as we use this a lot ...

--~ function aux.cachefunction(action,weak)
--~     local cache = { }
--~     if weak then
--~         setmetatable(cache, { __mode = "kv" } )
--~     end
--~     local function reminder(str)
--~         local found = cache[str]
--~         if not found then
--~             found = action(str)
--~             cache[str] = found
--~         end
--~         return found
--~     end
--~     return reminder, cache
--~ end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['trac-tra'] = {
    version   = 1.001,
    comment   = "companion to trac-tra.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- the <anonymous> tag is kind of generic and used for functions that are not
-- bound to a variable, like node.new, node.copy etc (contrary to for instance
-- node.has_attribute which is bound to a has_attribute local variable in mkiv)

local debug = require "debug"

local getinfo = debug.getinfo
local type, next = type, next
local concat = table.concat
local format, find, lower, gmatch, gsub = string.format, string.find, string.lower, string.gmatch, string.gsub

debugger = debugger or { }

local counters = { }
local names = { }

-- one

local function hook()
    local f = getinfo(2,"f").func
    local n = getinfo(2,"Sn")
--  if n.what == "C" and n.name then print (n.namewhat .. ': ' .. n.name) end
    if f then
        local cf = counters[f]
        if cf == nil then
            counters[f] = 1
            names[f] = n
        else
            counters[f] = cf + 1
        end
    end
end
local function getname(func)
    local n = names[func]
    if n then
        if n.what == "C" then
            return n.name or '<anonymous>'
        else
            -- source short_src linedefined what name namewhat nups func
            local name = n.name or n.namewhat or n.what
            if not name or name == "" then name = "?" end
            return format("%s : %s : %s", n.short_src or "unknown source", n.linedefined or "--", name)
        end
    else
        return "unknown"
    end
end
function debugger.showstats(printer,threshold)
    printer   = printer or texio.write or print
    threshold = threshold or 0
    local total, grandtotal, functions = 0, 0, 0
    printer("\n") -- ugly but ok
 -- table.sort(counters)
    for func, count in next, counters do
        if count > threshold then
            local name = getname(func)
            if not find(name,"for generator") then
                printer(format("%8i  %s", count, name))
                total = total + count
            end
        end
        grandtotal = grandtotal + count
        functions = functions + 1
    end
    printer(format("functions: %s, total: %s, grand total: %s, threshold: %s\n", functions, total, grandtotal, threshold))
end

-- two

--~ local function hook()
--~     local n = getinfo(2)
--~     if n.what=="C" and not n.name then
--~         local f = tostring(debug.traceback())
--~         local cf = counters[f]
--~         if cf == nil then
--~             counters[f] = 1
--~             names[f] = n
--~         else
--~             counters[f] = cf + 1
--~         end
--~     end
--~ end
--~ function debugger.showstats(printer,threshold)
--~     printer   = printer or texio.write or print
--~     threshold = threshold or 0
--~     local total, grandtotal, functions = 0, 0, 0
--~     printer("\n") -- ugly but ok
--~  -- table.sort(counters)
--~     for func, count in next, counters do
--~         if count > threshold then
--~             printer(format("%8i  %s", count, func))
--~             total = total + count
--~         end
--~         grandtotal = grandtotal + count
--~         functions = functions + 1
--~     end
--~     printer(format("functions: %s, total: %s, grand total: %s, threshold: %s\n", functions, total, grandtotal, threshold))
--~ end

-- rest

function debugger.savestats(filename,threshold)
    local f = io.open(filename,'w')
    if f then
        debugger.showstats(function(str) f:write(str) end,threshold)
        f:close()
    end
end

function debugger.enable()
    debug.sethook(hook,"c")
end

function debugger.disable()
    debug.sethook()
--~ counters[debug.getinfo(2,"f").func] = nil
end

function debugger.tracing()
    local n = tonumber(os.env['MTX.TRACE.CALLS']) or tonumber(os.env['MTX_TRACE_CALLS']) or 0
    if n > 0 then
        function debugger.tracing() return true  end ; return true
    else
        function debugger.tracing() return false end ; return false
    end
end

--~ debugger.enable()

--~ print(math.sin(1*.5))
--~ print(math.sin(1*.5))
--~ print(math.sin(1*.5))
--~ print(math.sin(1*.5))
--~ print(math.sin(1*.5))

--~ debugger.disable()

--~ print("")
--~ debugger.showstats()
--~ print("")
--~ debugger.showstats(print,3)

setters      = setters      or { }
setters.data = setters.data or { }

--~ local function set(t,what,value)
--~     local data, done = t.data, t.done
--~     if type(what) == "string" then
--~         what = aux.settings_to_array(what) -- inefficient but ok
--~     end
--~     for i=1,#what do
--~         local w = what[i]
--~         for d, f in next, data do
--~             if done[d] then
--~                 -- prevent recursion due to wildcards
--~             elseif find(d,w) then
--~                 done[d] = true
--~                 for i=1,#f do
--~                     f[i](value)
--~                 end
--~             end
--~         end
--~     end
--~ end

local function set(t,what,value)
    local data, done = t.data, t.done
    if type(what) == "string" then
        what = aux.settings_to_hash(what) -- inefficient but ok
    end
    for w, v in next, what do
        if v == "" then
            v = value
        else
            v = toboolean(v)
        end
        for d, f in next, data do
            if done[d] then
                -- prevent recursion due to wildcards
            elseif find(d,w) then
                done[d] = true
                for i=1,#f do
                    f[i](v)
                end
            end
        end
    end
end

local function reset(t)
    for d, f in next, t.data do
        for i=1,#f do
            f[i](false)
        end
    end
end

local function enable(t,what)
    set(t,what,true)
end

local function disable(t,what)
    local data = t.data
    if not what or what == "" then
        t.done = { }
        reset(t)
    else
        set(t,what,false)
    end
end

function setters.register(t,what,...)
    local data = t.data
    what = lower(what)
    local w = data[what]
    if not w then
        w = { }
        data[what] = w
    end
    for _, fnc in next, { ... } do
        local typ = type(fnc)
        if typ == "function" then
            w[#w+1] = fnc
        elseif typ == "string" then
            w[#w+1] = function(value) set(t,fnc,value,nesting) end
        end
    end
end

function setters.enable(t,what)
    local e = t.enable
    t.enable, t.done = enable, { }
    enable(t,string.simpleesc(tostring(what)))
    t.enable, t.done = e, { }
end

function setters.disable(t,what)
    local e = t.disable
    t.disable, t.done = disable, { }
    disable(t,string.simpleesc(tostring(what)))
    t.disable, t.done = e, { }
end

function setters.reset(t)
    t.done = { }
    reset(t)
end

function setters.list(t) -- pattern
    local list = table.sortedkeys(t.data)
    local user, system = { }, { }
    for l=1,#list do
        local what = list[l]
        if find(what,"^%*") then
            system[#system+1] = what
        else
            user[#user+1] = what
        end
    end
    return user, system
end

function setters.show(t)
    commands.writestatus("","")
    local list = setters.list(t)
    for k=1,#list do
        commands.writestatus(t.name,list[k])
    end
    commands.writestatus("","")
end

-- we could have used a bit of oo and the trackers:enable syntax but
-- there is already a lot of code around using the singular tracker

-- we could make this into a module

function setters.new(name)
    local t
    t = {
        data     = { },
        name     = name,
        enable   = function(...) setters.enable  (t,...) end,
        disable  = function(...) setters.disable (t,...) end,
        register = function(...) setters.register(t,...) end,
        list     = function(...) setters.list    (t,...) end,
        show     = function(...) setters.show    (t,...) end,
    }
    setters.data[name] = t
    return t
end

trackers    = setters.new("trackers")
directives  = setters.new("directives")
experiments = setters.new("experiments")

-- nice trick: we overload two of the directives related functions with variants that
-- do tracing (itself using a tracker) .. proof of concept

local trace_directives  = false local trace_directives  = false  trackers.register("system.directives",  function(v) trace_directives  = v end)
local trace_experiments = false local trace_experiments = false  trackers.register("system.experiments", function(v) trace_experiments = v end)

local e = directives.enable
local d = directives.disable

function directives.enable(...)
    commands.writestatus("directives","enabling: %s",concat({...}," "))
    e(...)
end

function directives.disable(...)
    commands.writestatus("directives","disabling: %s",concat({...}," "))
    d(...)
end

local e = experiments.enable
local d = experiments.disable

function experiments.enable(...)
    commands.writestatus("experiments","enabling: %s",concat({...}," "))
    e(...)
end

function experiments.disable(...)
    commands.writestatus("experiments","disabling: %s",concat({...}," "))
    d(...)
end

-- a useful example

directives.register("system.nostatistics", function(v)
    statistics.enable = not v
end)



end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['luat-env'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- A former version provided functionality for non embeded core
-- scripts i.e. runtime library loading. Given the amount of
-- Lua code we use now, this no longer makes sense. Much of this
-- evolved before bytecode arrays were available and so a lot of
-- code has disappeared already.

local trace_locating = false  trackers.register("resolvers.locating", function(v) trace_locating = v end)

local format, sub, match, gsub, find = string.format, string.sub, string.match, string.gsub, string.find
local unquote, quote = string.unquote, string.quote

-- precautions

os.setlocale(nil,nil) -- useless feature and even dangerous in luatex

function os.setlocale()
    -- no way you can mess with it
end

-- dirty tricks

if arg and (arg[0] == 'luatex' or arg[0] == 'luatex.exe') and arg[1] == "--luaonly" then
    arg[-1]=arg[0] arg[0]=arg[2] for k=3,#arg do arg[k-2]=arg[k] end arg[#arg]=nil arg[#arg]=nil
end

if profiler and os.env["MTX_PROFILE_RUN"] == "YES" then
    profiler.start("luatex-profile.log")
end

-- environment

environment             = environment or { }
environment.arguments   = { }
environment.files       = { }
environment.sortedflags = nil

if not environment.jobname or environment.jobname == "" then if tex then environment.jobname = tex.jobname end end
if not environment.version or environment.version == "" then             environment.version = "unknown"   end
if not environment.jobname                              then             environment.jobname = "unknown"   end

function environment.initialize_arguments(arg)
    local arguments, files = { }, { }
    environment.arguments, environment.files, environment.sortedflags = arguments, files, nil
    for index=1,#arg do
        local argument = arg[index]
        if index > 0 then
            local flag, value = match(argument,"^%-+(.-)=(.-)$")
            if flag then
                arguments[flag] = unquote(value or "")
            else
                flag = match(argument,"^%-+(.+)")
                if flag then
                    arguments[flag] = true
                else
                    files[#files+1] = argument
                end
            end
        end
    end
    environment.ownname = environment.ownname or arg[0] or 'unknown.lua'
end

function environment.setargument(name,value)
    environment.arguments[name] = value
end

-- todo: defaults, better checks e.g on type (boolean versus string)
--
-- tricky: too many hits when we support partials unless we add
-- a registration of arguments so from now on we have 'partial'

function environment.argument(name,partial)
    local arguments, sortedflags = environment.arguments, environment.sortedflags
    if arguments[name] then
        return arguments[name]
    elseif partial then
        if not sortedflags then
            sortedflags = table.sortedkeys(arguments)
            for k=1,#sortedflags do
                sortedflags[k] = "^" .. sortedflags[k]
            end
            environment.sortedflags = sortedflags
        end
        -- example of potential clash: ^mode ^modefile
        for k=1,#sortedflags do
            local v = sortedflags[k]
            if find(name,v) then
                return arguments[sub(v,2,#v)]
            end
        end
    end
    return nil
end

environment.argument("x",true)

function environment.split_arguments(separator) -- rather special, cut-off before separator
    local done, before, after = false, { }, { }
    local original_arguments = environment.original_arguments
    for k=1,#original_arguments do
        local v = original_arguments[k]
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

function environment.reconstruct_commandline(arg,noquote)
    arg = arg or environment.original_arguments
    if noquote and #arg == 1 then
        local a = arg[1]
        a = resolvers.resolve(a)
        a = unquote(a)
        return a
    elseif #arg > 0 then
        local result = { }
        for i=1,#arg do
            local a = arg[i]
            a = resolvers.resolve(a)
            a = unquote(a)
            a = gsub(a,'"','\\"') -- tricky
            if find(a," ") then
                result[#result+1] = quote(a)
            else
                result[#result+1] = a
            end
        end
        return table.join(result," ")
    else
        return ""
    end
end

if arg then

    -- new, reconstruct quoted snippets (maybe better just remove the " then and add them later)
    local newarg, instring = { }, false

    for index=1,#arg do
        local argument = arg[index]
        if find(argument,"^\"") then
            newarg[#newarg+1] = gsub(argument,"^\"","")
            if not find(argument,"\"$") then
                instring = true
            end
        elseif find(argument,"\"$") then
            newarg[#newarg] = newarg[#newarg] .. " " .. gsub(argument,"\"$","")
            instring = false
        elseif instring then
            newarg[#newarg] = newarg[#newarg] .. " " .. argument
        else
            newarg[#newarg+1] = argument
        end
    end
    for i=1,-5,-1 do
        newarg[i] = arg[i]
    end

    environment.initialize_arguments(newarg)
    environment.original_arguments = newarg
    environment.raw_arguments = arg

    arg = { } -- prevent duplicate handling

end

-- weird place ... depends on a not yet loaded module

function environment.texfile(filename)
    return resolvers.find_file(filename,'tex')
end

function environment.luafile(filename)
    local resolved = resolvers.find_file(filename,'tex') or ""
    if resolved ~= "" then
        return resolved
    end
    resolved = resolvers.find_file(filename,'texmfscripts') or ""
    if resolved ~= "" then
        return resolved
    end
    return resolvers.find_file(filename,'luatexlibs') or ""
end

environment.loadedluacode = loadfile -- can be overloaded

--~ function environment.loadedluacode(name)
--~     if os.spawn("texluac -s -o texluac.luc " .. name) == 0 then
--~         local chunk = loadstring(io.loaddata("texluac.luc"))
--~         os.remove("texluac.luc")
--~         return chunk
--~     else
--~         environment.loadedluacode = loadfile -- can be overloaded
--~         return loadfile(name)
--~     end
--~ end

function environment.luafilechunk(filename) -- used for loading lua bytecode in the format
    filename = file.replacesuffix(filename, "lua")
    local fullname = environment.luafile(filename)
    if fullname and fullname ~= "" then
        if trace_locating then
            logs.report("fileio","loading file %s", fullname)
        end
        return environment.loadedluacode(fullname)
    else
        if trace_locating then
            logs.report("fileio","unknown file %s", filename)
        end
        return nil
    end
end

-- the next ones can use the previous ones / combine

function environment.loadluafile(filename, version)
    local lucname, luaname, chunk
    local basename = file.removesuffix(filename)
    if basename == filename then
        lucname, luaname = basename .. ".luc",  basename .. ".lua"
    else
        lucname, luaname = nil, basename -- forced suffix
    end
    -- when not overloaded by explicit suffix we look for a luc file first
    local fullname = (lucname and environment.luafile(lucname)) or ""
    if fullname ~= "" then
        if trace_locating then
            logs.report("fileio","loading %s", fullname)
        end
        chunk = loadfile(fullname) -- this way we don't need a file exists check
    end
    if chunk then
        assert(chunk)()
        if version then
            -- we check of the version number of this chunk matches
            local v = version -- can be nil
            if modules and modules[filename] then
                v = modules[filename].version -- new method
            elseif versions and versions[filename] then
                v = versions[filename]        -- old method
            end
            if v == version then
                return true
            else
                if trace_locating then
                    logs.report("fileio","version mismatch for %s: lua=%s, luc=%s", filename, v, version)
                end
                environment.loadluafile(filename)
            end
        else
            return true
        end
    end
    fullname = (luaname and environment.luafile(luaname)) or ""
    if fullname ~= "" then
        if trace_locating then
            logs.report("fileio","loading %s", fullname)
        end
        chunk = loadfile(fullname) -- this way we don't need a file exists check
        if not chunk then
            if trace_locating then
                logs.report("fileio","unknown file %s", filename)
            end
        else
            assert(chunk)()
            return true
        end
    end
    return false
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['trac-inf'] = {
    version   = 1.001,
    comment   = "companion to trac-inf.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local format = string.format

local statusinfo, n, registered = { }, 0, { }

statistics = statistics or { }

statistics.enable    = true
statistics.threshold = 0.05

-- timing functions

local clock = os.gettimeofday or os.clock

local notimer

function statistics.hastimer(instance)
    return instance and instance.starttime
end

function statistics.resettiming(instance)
    if not instance then
        notimer = { timing = 0, loadtime = 0 }
    else
        instance.timing, instance.loadtime = 0, 0
    end
end

function statistics.starttiming(instance)
    if not instance then
        notimer = { }
        instance = notimer
    end
    local it = instance.timing
    if not it then
        it = 0
    end
    if it == 0 then
        instance.starttime = clock()
        if not instance.loadtime then
            instance.loadtime = 0
        end
    else
--~         logs.report("system","nested timing (%s)",tostring(instance))
    end
    instance.timing = it + 1
end

function statistics.stoptiming(instance, report)
    if not instance then
        instance = notimer
    end
    if instance then
        local it = instance.timing
        if it > 1 then
            instance.timing = it - 1
        else
            local starttime = instance.starttime
            if starttime then
                local stoptime = clock()
                local loadtime = stoptime - starttime
                instance.stoptime = stoptime
                instance.loadtime = instance.loadtime + loadtime
                if report then
                    statistics.report("load time %0.3f",loadtime)
                end
                instance.timing = 0
                return loadtime
            end
        end
    end
    return 0
end

function statistics.elapsedtime(instance)
    if not instance then
        instance = notimer
    end
    return format("%0.3f",(instance and instance.loadtime) or 0)
end

function statistics.elapsedindeed(instance)
    if not instance then
        instance = notimer
    end
    local t = (instance and instance.loadtime) or 0
    return t > statistics.threshold
end

function statistics.elapsedseconds(instance,rest) -- returns nil if 0 seconds
    if statistics.elapsedindeed(instance) then
        return format("%s seconds %s", statistics.elapsedtime(instance),rest or "")
    end
end

-- general function

function statistics.register(tag,fnc)
    if statistics.enable and type(fnc) == "function" then
        local rt = registered[tag] or (#statusinfo + 1)
        statusinfo[rt] = { tag, fnc }
        registered[tag] = rt
        if #tag > n then n = #tag end
    end
end

function statistics.show(reporter)
    if statistics.enable then
        if not reporter then reporter = function(tag,data,n) texio.write_nl(tag .. " " .. data) end end
        -- this code will move
        local register = statistics.register
        register("luatex banner", function()
            return string.lower(status.banner)
        end)
        register("control sequences", function()
            return format("%s of %s", status.cs_count, status.hash_size+status.hash_extra)
        end)
        register("callbacks", function()
            local total, indirect = status.callbacks or 0, status.indirect_callbacks or 0
            return format("direct: %s, indirect: %s, total: %s", total-indirect, indirect, total)
        end)
        register("current memory usage", statistics.memused)
        register("runtime",statistics.runtime)
--         --
        for i=1,#statusinfo do
            local s = statusinfo[i]
            local r = s[2]()
            if r then
                reporter(s[1],r,n)
            end
        end
        texio.write_nl("") -- final newline
        statistics.enable = false
    end
end

function statistics.show_job_stat(tag,data,n)
    texio.write_nl(format("%-15s: %s - %s","mkiv lua stats",tag:rpadd(n," "),data))
end

function statistics.memused() -- no math.round yet -)
    local round = math.round or math.floor
    return format("%s MB (ctx: %s MB)",round(collectgarbage("count")/1000), round(status.luastate_bytes/1000000))
end

if statistics.runtime then
    -- already loaded and set
elseif luatex and luatex.starttime then
    statistics.starttime = luatex.starttime
    statistics.loadtime = 0
    statistics.timing = 0
else
    statistics.starttiming(statistics)
end

function statistics.runtime()
    statistics.stoptiming(statistics)
    return statistics.formatruntime(statistics.elapsedtime(statistics))
end

function statistics.formatruntime(runtime)
    return format("%s seconds", statistics.elapsedtime(statistics))
end

function statistics.timed(action,report)
    local timer = { }
    report = report or logs.simple
    statistics.starttiming(timer)
    action()
    statistics.stoptiming(timer)
    report("total runtime: %s",statistics.elapsedtime(timer))
end

-- where, not really the best spot for this:

commands = commands or { }

local timer

function commands.resettimer()
    statistics.resettiming(timer)
    statistics.starttiming(timer)
end

function commands.elapsedtime()
    statistics.stoptiming(timer)
    tex.sprint(statistics.elapsedtime(timer))
end

commands.resettimer()


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['trac-log'] = {
    version   = 1.001,
    comment   = "companion to trac-log.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- this is old code that needs an overhaul

--~ io.stdout:setvbuf("no")
--~ io.stderr:setvbuf("no")

local write_nl, write = texio.write_nl or print, texio.write or io.write
local format, gmatch = string.format, string.gmatch
local texcount = tex and tex.count

if texlua then
    write_nl = print
    write    = io.write
end

--[[ldx--
<p>This is a prelude to a more extensive logging module. For the sake
of parsing log files, in addition to the standard logging we will
provide an <l n='xml'/> structured file. Actually, any logging that
is hooked into callbacks will be \XML\ by default.</p>
--ldx]]--

logs     = logs     or { }
logs.xml = logs.xml or { }
logs.tex = logs.tex or { }

--[[ldx--
<p>This looks pretty ugly but we need to speed things up a bit.</p>
--ldx]]--

logs.moreinfo = [[
more information about ConTeXt and the tools that come with it can be found at:

maillist : ntg-context@ntg.nl / http://www.ntg.nl/mailman/listinfo/ntg-context
webpage  : http://www.pragma-ade.nl / http://tex.aanhet.net
wiki     : http://contextgarden.net
]]

logs.levels = {
    ['error']   = 1,
    ['warning'] = 2,
    ['info']    = 3,
    ['debug']   = 4,
}

logs.functions = {
    'report', 'start', 'stop', 'push', 'pop', 'line', 'direct',
    'start_run', 'stop_run',
    'start_page_number', 'stop_page_number',
    'report_output_pages', 'report_output_log',
    'report_tex_stat', 'report_job_stat',
    'show_open', 'show_close', 'show_load',
}

logs.tracers = {
}

logs.level = 0
logs.mode  = string.lower((os.getenv("MTX.LOG.MODE") or os.getenv("MTX_LOG_MODE") or "tex"))

function logs.set_level(level)
    logs.level = logs.levels[level] or level
end

function logs.set_method(method)
    for _, v in next, logs.functions do
        logs[v] = logs[method][v] or function() end
    end
end

-- tex logging

function logs.tex.report(category,fmt,...) -- new
    if fmt then
        write_nl(category .. " | " .. format(fmt,...))
    else
        write_nl(category .. " |")
    end
end

function logs.tex.line(fmt,...) -- new
    if fmt then
        write_nl(format(fmt,...))
    else
        write_nl("")
    end
end

--~ function logs.tex.start_page_number()
--~     local real, user, sub = texcount.realpageno, texcount.userpageno, texcount.subpageno
--~     if real > 0 then
--~         if user > 0 then
--~             if sub > 0 then
--~                 write(format("[%s.%s.%s",real,user,sub))
--~             else
--~                 write(format("[%s.%s",real,user))
--~             end
--~         else
--~             write(format("[%s",real))
--~         end
--~     else
--~         write("[-")
--~     end
--~ end

--~ function logs.tex.stop_page_number()
--~     write("]")
--~ end

local real, user, sub

function logs.tex.start_page_number()
    real, user, sub = texcount.realpageno, texcount.userpageno, texcount.subpageno
end

function logs.tex.stop_page_number()
    if real > 0 then
        if user > 0 then
            if sub > 0 then
                logs.report("pages", "flushing realpage %s, userpage %s, subpage %s",real,user,sub)
            else
                logs.report("pages", "flushing realpage %s, userpage %s",real,user)
            end
        else
            logs.report("pages", "flushing realpage %s",real)
        end
    else
        logs.report("pages", "flushing page")
    end
    io.flush()
end

logs.tex.report_job_stat = statistics.show_job_stat

-- xml logging

function logs.xml.report(category,fmt,...) -- new
    if fmt then
        write_nl(format("<r category='%s'>%s</r>",category,format(fmt,...)))
    else
        write_nl(format("<r category='%s'/>",category))
    end
end
function logs.xml.line(fmt,...) -- new
    if fmt then
        write_nl(format("<r>%s</r>",format(fmt,...)))
    else
        write_nl("<r/>")
    end
end

function logs.xml.start() if logs.level > 0 then tw("<%s>" ) end end
function logs.xml.stop () if logs.level > 0 then tw("</%s>") end end
function logs.xml.push () if logs.level > 0 then tw("<!-- ") end end
function logs.xml.pop  () if logs.level > 0 then tw(" -->" ) end end

function logs.xml.start_run()
    write_nl("<?xml version='1.0' standalone='yes'?>")
    write_nl("<job>") --  xmlns='www.pragma-ade.com/luatex/schemas/context-job.rng'
    write_nl("")
end

function logs.xml.stop_run()
    write_nl("</job>")
end

function logs.xml.start_page_number()
    write_nl(format("<p real='%s' page='%s' sub='%s'", texcount.realpageno, texcount.userpageno, texcount.subpageno))
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

function logs.xml.report_tex_stat(k,v)
    texiowrite_nl("log","<v k='"..k.."'>"..tostring(v).."</v>")
end

local level = 0

function logs.xml.show_open(name)
    level = level + 1
    texiowrite_nl(format("<f l='%s' n='%s'>",level,name))
end

function logs.xml.show_close(name)
    texiowrite("</f> ")
    level = level - 1
end

function logs.xml.show_load(name)
    texiowrite_nl(format("<f l='%s' n='%s'/>",level+1,name))
end

--

local name, banner = 'report', 'context'

local function report(category,fmt,...)
    if fmt then
        write_nl(format("%s | %s: %s",name,category,format(fmt,...)))
    elseif category then
        write_nl(format("%s | %s",name,category))
    else
        write_nl(format("%s |",name))
    end
end

local function simple(fmt,...)
    if fmt then
        write_nl(format("%s | %s",name,format(fmt,...)))
    else
        write_nl(format("%s |",name))
    end
end

function logs.setprogram(_name_,_banner_,_verbose_)
    name, banner = _name_, _banner_
    if _verbose_ then
        trackers.enable("resolvers.locating")
    end
    logs.set_method("tex")
    logs.report = report -- also used in libraries
    logs.simple = simple -- only used in scripts !
    if utils then
        utils.report = simple
    end
    logs.verbose = _verbose_
end

function logs.setverbose(what)
    if what then
        trackers.enable("resolvers.locating")
    else
        trackers.disable("resolvers.locating")
    end
    logs.verbose = what or false
end

function logs.extendbanner(_banner_,_verbose_)
    banner = banner .. " | ".. _banner_
    if _verbose_ ~= nil then
        logs.setverbose(what)
    end
end

logs.verbose = false
logs.report  = logs.tex.report
logs.simple  = logs.tex.report

function logs.reportlines(str) -- todo: <lines></lines>
    for line in gmatch(str,"(.-)[\n\r]") do
        logs.report(line)
    end
end

function logs.reportline() -- for scripts too
    logs.report()
end

logs.simpleline = logs.reportline

function logs.reportbanner() -- for scripts too
    logs.report(banner)
end

function logs.help(message,option)
    logs.reportbanner()
    logs.reportline()
    logs.reportlines(message)
    local moreinfo = logs.moreinfo or ""
    if moreinfo ~= "" and option ~= "nomoreinfo" then
        logs.reportline()
        logs.reportlines(moreinfo)
    end
end

logs.set_level('error')
logs.set_method('tex')

function logs.system(whereto,process,jobname,category,...)
    for i=1,10 do
        local f = io.open(whereto,"a")
        if f then
            f:write(format("%s %s => %s => %s => %s\r",os.date("%d/%m/%y %H:%m:%S"),process,jobname,category,format(...)))
            f:close()
            break
        else
            sleep(0.1)
        end
    end
end

--~ local syslogname = "oeps.xxx"
--~
--~ for i=1,10 do
--~     logs.system(syslogname,"context","test","fonts","font %s recached due to newer version (%s)","blabla","123")
--~ end

function logs.fatal(where,...)
    logs.report(where,"fatal error: %s, aborting now",format(...))
    os.exit()
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-inp'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files",
}

-- After a few years using the code the large luat-inp.lua file
-- has been split up a bit. In the process some functionality was
-- dropped:
--
-- * support for reading lsr files
-- * selective scanning (subtrees)
-- * some public auxiliary functions were made private
--
-- TODO: os.getenv -> os.env[]
-- TODO: instances.[hashes,cnffiles,configurations,522]
-- TODO: check escaping in find etc, too much, too slow

-- This lib is multi-purpose and can be loaded again later on so that
-- additional functionality becomes available. We will split thislogs.report("fileio",
-- module in components once we're done with prototyping. This is the
-- first code I wrote for LuaTeX, so it needs some cleanup. Before changing
-- something in this module one can best check with Taco or Hans first; there
-- is some nasty trickery going on that relates to traditional kpse support.

-- To be considered: hash key lowercase, first entry in table filename
-- (any case), rest paths (so no need for optimization). Or maybe a
-- separate table that matches lowercase names to mixed case when
-- present. In that case the lower() cases can go away. I will do that
-- only when we run into problems with names ... well ... Iwona-Regular.

-- Beware, loading and saving is overloaded in luat-tmp!

local format, gsub, find, lower, upper, match, gmatch = string.format, string.gsub, string.find, string.lower, string.upper, string.match, string.gmatch
local concat, insert, sortedkeys = table.concat, table.insert, table.sortedkeys
local next, type = next, type
local lpegmatch = lpeg.match

local trace_locating, trace_detail, trace_expansions = false, false, false

trackers.register("resolvers.locating",   function(v) trace_locating   = v end)
trackers.register("resolvers.details",    function(v) trace_detail     = v end)
trackers.register("resolvers.expansions", function(v) trace_expansions = v end) -- todo

if not resolvers then
    resolvers = {
        suffixes     = { },
        formats      = { },
        dangerous    = { },
        suffixmap    = { },
        alternatives = { },
        locators     = { },  -- locate databases
        hashers      = { },  -- load databases
        generators   = { },  -- generate databases
    }
end

local resolvers = resolvers

resolvers.locators  .notfound = { nil }
resolvers.hashers   .notfound = { nil }
resolvers.generators.notfound = { nil }

resolvers.cacheversion = '1.0.1'
resolvers.cnfname      = 'texmf.cnf'
resolvers.luaname      = 'texmfcnf.lua'
resolvers.homedir      = os.env[os.type == "windows" and 'USERPROFILE'] or os.env['HOME'] or '~'
resolvers.cnfdefault   = '{$SELFAUTODIR,$SELFAUTOPARENT}{,{/share,}/texmf{-local,.local,}/web2c}'

local dummy_path_expr = "^!*unset/*$"

local formats      = resolvers.formats
local suffixes     = resolvers.suffixes
local dangerous    = resolvers.dangerous
local suffixmap    = resolvers.suffixmap
local alternatives = resolvers.alternatives

formats['afm'] = 'AFMFONTS'       suffixes['afm'] = { 'afm' }
formats['enc'] = 'ENCFONTS'       suffixes['enc'] = { 'enc' }
formats['fmt'] = 'TEXFORMATS'     suffixes['fmt'] = { 'fmt' }
formats['map'] = 'TEXFONTMAPS'    suffixes['map'] = { 'map' }
formats['mp']  = 'MPINPUTS'       suffixes['mp']  = { 'mp' }
formats['ocp'] = 'OCPINPUTS'      suffixes['ocp'] = { 'ocp' }
formats['ofm'] = 'OFMFONTS'       suffixes['ofm'] = { 'ofm', 'tfm' }
formats['otf'] = 'OPENTYPEFONTS'  suffixes['otf'] = { 'otf' } -- 'ttf'
formats['opl'] = 'OPLFONTS'       suffixes['opl'] = { 'opl' }
formats['otp'] = 'OTPINPUTS'      suffixes['otp'] = { 'otp' }
formats['ovf'] = 'OVFFONTS'       suffixes['ovf'] = { 'ovf', 'vf' }
formats['ovp'] = 'OVPFONTS'       suffixes['ovp'] = { 'ovp' }
formats['tex'] = 'TEXINPUTS'      suffixes['tex'] = { 'tex' }
formats['tfm'] = 'TFMFONTS'       suffixes['tfm'] = { 'tfm' }
formats['ttf'] = 'TTFONTS'        suffixes['ttf'] = { 'ttf', 'ttc', 'dfont' }
formats['pfb'] = 'T1FONTS'        suffixes['pfb'] = { 'pfb', 'pfa' }
formats['vf']  = 'VFFONTS'        suffixes['vf']  = { 'vf' }

formats['fea'] = 'FONTFEATURES'   suffixes['fea'] = { 'fea' }
formats['cid'] = 'FONTCIDMAPS'    suffixes['cid'] = { 'cid', 'cidmap' }

formats ['texmfscripts'] = 'TEXMFSCRIPTS' -- new
suffixes['texmfscripts'] = { 'rb', 'pl', 'py' } -- 'lua'

formats ['lua'] = 'LUAINPUTS' -- new
suffixes['lua'] = { 'lua', 'luc', 'tma', 'tmc' }

-- backward compatible ones

alternatives['map files']            = 'map'
alternatives['enc files']            = 'enc'
alternatives['cid maps']             = 'cid' -- great, why no cid files
alternatives['font feature files']   = 'fea' -- and fea files here
alternatives['opentype fonts']       = 'otf'
alternatives['truetype fonts']       = 'ttf'
alternatives['truetype collections'] = 'ttc'
alternatives['truetype dictionary']  = 'dfont'
alternatives['type1 fonts']          = 'pfb'

-- obscure ones

formats ['misc fonts'] = ''
suffixes['misc fonts'] = { }

formats     ['sfd']                      = 'SFDFONTS'
suffixes    ['sfd']                      = { 'sfd' }
alternatives['subfont definition files'] = 'sfd'

-- lib paths

formats ['lib'] = 'CLUAINPUTS' -- new (needs checking)
suffixes['lib'] = (os.libsuffix and { os.libsuffix }) or { 'dll', 'so' }

-- In practice we will work within one tds tree, but i want to keep
-- the option open to build tools that look at multiple trees, which is
-- why we keep the tree specific data in a table. We used to pass the
-- instance but for practical pusposes we now avoid this and use a
-- instance variable.

-- here we catch a few new thingies (todo: add these paths to context.tmf)
--
-- FONTFEATURES  = .;$TEXMF/fonts/fea//
-- FONTCIDMAPS   = .;$TEXMF/fonts/cid//

-- we always have one instance active

resolvers.instance = resolvers.instance or nil -- the current one (slow access)
local instance = resolvers.instance or nil -- the current one (fast access)

function resolvers.newinstance()

    -- store once, freeze and faster (once reset we can best use
    -- instance.environment) maybe better have a register suffix
    -- function

    for k, v in next, suffixes do
        for i=1,#v do
            local vi = v[i]
            if vi then
                suffixmap[vi] = k
            end
        end
    end

    -- because vf searching is somewhat dangerous, we want to prevent
    -- too liberal searching esp because we do a lookup on the current
    -- path anyway; only tex (or any) is safe

    for k, v in next, formats do
        dangerous[k] = true
    end
    dangerous.tex = nil

    -- the instance

    local newinstance = {
        rootpath        = '',
        treepath        = '',
        progname        = 'context',
        engine          = 'luatex',
        format          = '',
        environment     = { },
        variables       = { },
        expansions      = { },
        files           = { },
        remap           = { },
        configuration   = { },
        setup           = { },
        order           = { },
        found           = { },
        foundintrees    = { },
        kpsevars        = { },
        hashes          = { },
        cnffiles        = { },
        luafiles        = { },
        lists           = { },
        remember        = true,
        diskcache       = true,
        renewcache      = false,
        scandisk        = true,
        cachepath       = nil,
        loaderror       = false,
        sortdata        = false,
        savelists       = true,
        cleanuppaths    = true,
        allresults      = false,
        pattern         = nil, -- lists
        data            = { }, -- only for loading
        force_suffixes  = true,
        fakepaths       = { },
    }

    local ne = newinstance.environment

    for k,v in next, os.env do
        ne[k] = resolvers.bare_variable(v)
    end

    return newinstance

end

function resolvers.setinstance(someinstance)
    instance = someinstance
    resolvers.instance = someinstance
    return someinstance
end

function resolvers.reset()
    return resolvers.setinstance(resolvers.newinstance())
end

local function reset_hashes()
    instance.lists = { }
    instance.found = { }
end

local function check_configuration() -- not yet ok, no time for debugging now
    local ie, iv = instance.environment, instance.variables
    local function fix(varname,default)
        local proname = varname .. "." .. instance.progname or "crap"
        local p, v = ie[proname], ie[varname] or iv[varname]
        if not ((p and p ~= "") or (v and v ~= "")) then
            iv[varname] = default -- or environment?
        end
    end
    local name = os.name
    if name == "windows" then
        fix("OSFONTDIR", "c:/windows/fonts//")
    elseif name == "macosx" then
        fix("OSFONTDIR", "$HOME/Library/Fonts//;/Library/Fonts//;/System/Library/Fonts//")
    else
        -- bad luck
    end
    fix("LUAINPUTS"   , ".;$TEXINPUTS;$TEXMFSCRIPTS") -- no progname, hm
    -- this will go away some day
    fix("FONTFEATURES", ".;$TEXMF/fonts/{data,fea}//;$OPENTYPEFONTS;$TTFONTS;$T1FONTS;$AFMFONTS")
    fix("FONTCIDMAPS" , ".;$TEXMF/fonts/{data,cid}//;$OPENTYPEFONTS;$TTFONTS;$T1FONTS;$AFMFONTS")
    --
    fix("LUATEXLIBS"  , ".;$TEXMF/luatex/lua//")
end

function resolvers.bare_variable(str) -- assumes str is a string
    return (gsub(str,"\s*([\"\']?)(.+)%1\s*", "%2"))
end

function resolvers.settrace(n) -- no longer number but: 'locating' or 'detail'
    if n then
        trackers.disable("resolvers.*")
        trackers.enable("resolvers."..n)
    end
end

resolvers.settrace(os.getenv("MTX_INPUT_TRACE"))

function resolvers.osenv(key)
    local ie = instance.environment
    local value = ie[key]
    if value == nil then
     -- local e = os.getenv(key)
        local e = os.env[key]
        if e == nil then
         -- value = "" -- false
        else
            value = resolvers.bare_variable(e)
        end
        ie[key] = value
    end
    return value or ""
end

function resolvers.env(key)
    return instance.environment[key] or resolvers.osenv(key)
end

--

local function expand_vars(lst) -- simple vars
    local variables, env = instance.variables, resolvers.env
    local function resolve(a)
        return variables[a] or env(a)
    end
    for k=1,#lst do
        lst[k] = gsub(lst[k],"%$([%a%d%_%-]+)",resolve)
    end
end

local function expanded_var(var) -- simple vars
    local function resolve(a)
        return instance.variables[a] or resolvers.env(a)
    end
    return (gsub(var,"%$([%a%d%_%-]+)",resolve))
end

local function entry(entries,name)
    if name and (name ~= "") then
        name = gsub(name,'%$','')
        local result = entries[name..'.'..instance.progname] or entries[name]
        if result then
            return result
        else
            result = resolvers.env(name)
            if result then
                instance.variables[name] = result
                resolvers.expand_variables()
                return instance.expansions[name] or ""
            end
        end
    end
    return ""
end

local function is_entry(entries,name)
    if name and name ~= "" then
        name = gsub(name,'%$','')
        return (entries[name..'.'..instance.progname] or entries[name]) ~= nil
    else
        return false
    end
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
-- {$SELFAUTODIR,$SELFAUTOPARENT}{,{/share,}/texmf{-local,.local,}/web2c}

-- this one is better and faster, but it took me a while to realize
-- that this kind of replacement is cleaner than messy parsing and
-- fuzzy concatenating we can probably gain a bit with selectively
-- applying lpeg, but experiments with lpeg parsing this proved not to
-- work that well; the parsing is ok, but dealing with the resulting
-- table is a pain because we need to work inside-out recursively

local function do_first(a,b)
    local t = { }
    for s in gmatch(b,"[^,]+") do t[#t+1] = a .. s end
    return "{" .. concat(t,",") .. "}"
end

local function do_second(a,b)
    local t = { }
    for s in gmatch(a,"[^,]+") do t[#t+1] = s .. b end
    return "{" .. concat(t,",") .. "}"
end

local function do_both(a,b)
    local t = { }
    for sa in gmatch(a,"[^,]+") do
        for sb in gmatch(b,"[^,]+") do
            t[#t+1] = sa .. sb
        end
    end
    return "{" .. concat(t,",") .. "}"
end

local function do_three(a,b,c)
    return a .. b.. c
end

local function splitpathexpr(str, t, validate)
    -- no need for further optimization as it is only called a
    -- few times, we can use lpeg for the sub
    if trace_expansions then
        logs.report("fileio","expanding variable '%s'",str)
    end
    t = t or { }
    str = gsub(str,",}",",@}")
    str = gsub(str,"{,","{@,")
 -- str = "@" .. str .. "@"
    local ok, done
    while true do
        done = false
        while true do
            str, ok = gsub(str,"([^{},]+){([^{}]+)}",do_first)
            if ok > 0 then done = true else break end
        end
        while true do
            str, ok = gsub(str,"{([^{}]+)}([^{},]+)",do_second)
            if ok > 0 then done = true else break end
        end
        while true do
            str, ok = gsub(str,"{([^{}]+)}{([^{}]+)}",do_both)
            if ok > 0 then done = true else break end
        end
        str, ok = gsub(str,"({[^{}]*){([^{}]+)}([^{}]*})",do_three)
        if ok > 0 then done = true end
        if not done then break end
    end
    str = gsub(str,"[{}]", "")
    str = gsub(str,"@","")
    if validate then
        for s in gmatch(str,"[^,]+") do
            s = validate(s)
            if s then t[#t+1] = s end
        end
    else
        for s in gmatch(str,"[^,]+") do
            t[#t+1] = s
        end
    end
    if trace_expansions then
        for k=1,#t do
            logs.report("fileio","% 4i: %s",k,t[k])
        end
    end
    return t
end

local function expanded_path_from_list(pathlist) -- maybe not a list, just a path
    -- a previous version fed back into pathlist
    local newlist, ok = { }, false
    for k=1,#pathlist do
        if find(pathlist[k],"[{}]") then
            ok = true
            break
        end
    end
    if ok then
        local function validate(s)
            s = file.collapse_path(s)
            return s ~= "" and not find(s,dummy_path_expr) and s
        end
        for k=1,#pathlist do
            splitpathexpr(pathlist[k],newlist,validate)
        end
    else
        for k=1,#pathlist do
            for p in gmatch(pathlist[k],"([^,]+)") do
                p = file.collapse_path(p)
                if p ~= "" then newlist[#newlist+1] = p end
            end
        end
    end
    return newlist
end

-- we follow a rather traditional approach:
--
-- (1) texmf.cnf given in TEXMFCNF
-- (2) texmf.cnf searched in default variable
--
-- also we now follow the stupid route: if not set then just assume *one*
-- cnf file under texmf (i.e. distribution)

local args = environment and environment.original_arguments or arg -- this needs a cleanup

resolvers.ownbin = resolvers.ownbin or args[-2] or arg[-2] or args[-1] or arg[-1] or arg[0] or "luatex"
resolvers.ownbin = gsub(resolvers.ownbin,"\\","/")

function resolvers.getownpath()
    local ownpath = resolvers.ownpath or os.selfdir
    if not ownpath or ownpath == "" or ownpath == "unset" then
        ownpath = args[-1] or arg[-1]
        ownpath = ownpath and file.dirname(gsub(ownpath,"\\","/"))
        if not ownpath or ownpath == "" then
            ownpath = args[-0] or arg[-0]
            ownpath = ownpath and file.dirname(gsub(ownpath,"\\","/"))
        end
        local binary = resolvers.ownbin
        if not ownpath or ownpath == "" then
            ownpath = ownpath and file.dirname(binary)
        end
        if not ownpath or ownpath == "" then
            if os.binsuffix ~= "" then
                binary = file.replacesuffix(binary,os.binsuffix)
            end
            for p in gmatch(os.getenv("PATH"),"[^"..io.pathseparator.."]+") do
                local b = file.join(p,binary)
                if lfs.isfile(b) then
                    -- we assume that after changing to the path the currentdir function
                    -- resolves to the real location and use this side effect here; this
                    -- trick is needed because on the mac installations use symlinks in the
                    -- path instead of real locations
                    local olddir = lfs.currentdir()
                    if lfs.chdir(p) then
                        local pp = lfs.currentdir()
                        if trace_locating and p ~= pp then
                            logs.report("fileio","following symlink '%s' to '%s'",p,pp)
                        end
                        ownpath = pp
                        lfs.chdir(olddir)
                    else
                        if trace_locating then
                            logs.report("fileio","unable to check path '%s'",p)
                        end
                        ownpath =  p
                    end
                    break
                end
            end
        end
        if not ownpath or ownpath == "" then
            ownpath = "."
            logs.report("fileio","forcing fallback ownpath .")
        elseif trace_locating then
            logs.report("fileio","using ownpath '%s'",ownpath)
        end
    end
    resolvers.ownpath = ownpath
    function resolvers.getownpath()
        return resolvers.ownpath
    end
    return ownpath
end

local own_places = { "SELFAUTOLOC", "SELFAUTODIR", "SELFAUTOPARENT", "TEXMFCNF" }

local function identify_own()
    local ownpath = resolvers.getownpath() or dir.current()
    local ie = instance.environment
    if ownpath then
        if resolvers.env('SELFAUTOLOC')    == "" then os.env['SELFAUTOLOC']    = file.collapse_path(ownpath) end
        if resolvers.env('SELFAUTODIR')    == "" then os.env['SELFAUTODIR']    = file.collapse_path(ownpath .. "/..") end
        if resolvers.env('SELFAUTOPARENT') == "" then os.env['SELFAUTOPARENT'] = file.collapse_path(ownpath .. "/../..") end
    else
        logs.report("fileio","error: unable to locate ownpath")
        os.exit()
    end
    if resolvers.env('TEXMFCNF') == "" then os.env['TEXMFCNF'] = resolvers.cnfdefault end
    if resolvers.env('TEXOS')    == "" then os.env['TEXOS']    = resolvers.env('SELFAUTODIR') end
    if resolvers.env('TEXROOT')  == "" then os.env['TEXROOT']  = resolvers.env('SELFAUTOPARENT') end
    if trace_locating then
        for i=1,#own_places do
            local v = own_places[i]
            logs.report("fileio","variable '%s' set to '%s'",v,resolvers.env(v) or "unknown")
        end
    end
    identify_own = function() end
end

function resolvers.identify_cnf()
    if #instance.cnffiles == 0 then
        -- fallback
        identify_own()
        -- the real search
        resolvers.expand_variables()
        local t = resolvers.split_path(resolvers.env('TEXMFCNF'))
        t = expanded_path_from_list(t)
        expand_vars(t) -- redundant
        local function locate(filename,list)
            for i=1,#t do
                local ti = t[i]
                local texmfcnf = file.collapse_path(file.join(ti,filename))
                if lfs.isfile(texmfcnf) then
                    list[#list+1] = texmfcnf
                end
            end
        end
        locate(resolvers.luaname,instance.luafiles)
        locate(resolvers.cnfname,instance.cnffiles)
    end
end

local function load_cnf_file(fname)
    fname = resolvers.clean_path(fname)
    local lname = file.replacesuffix(fname,'lua')
    if lfs.isfile(lname) then
        local dname = file.dirname(fname) -- fname ?
        if not instance.configuration[dname] then
            resolvers.load_data(dname,'configuration',lname and file.basename(lname))
            instance.order[#instance.order+1] = instance.configuration[dname]
        end
    else
        f = io.open(fname)
        if f then
            if trace_locating then
                logs.report("fileio","loading configuration file %s", fname)
            end
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
                        line, n = gsub(line,"\\%s*$", "")
                        if n > 0 then
                            line = line .. f:read()
                        else
                            break
                        end
                    end
                    if not find(line,"^[%%#]") then
                        local l = gsub(line,"%s*%%.*$","")
                        local k, v = match(l,"%s*(.-)%s*=%s*(.-)%s*$")
                        if k and v and not data[k] then
                            v = gsub(v,"[%%#].*",'')
                            data[k] = gsub(v,"~","$HOME")
                            instance.kpsevars[k] = true
                        end
                    end
                else
                    break
                end
            end
            f:close()
        elseif trace_locating then
            logs.report("fileio","skipping configuration file '%s'", fname)
        end
    end
end

local function collapse_cnf_data() -- potential optimization: pass start index (setup and configuration are shared)
    local order = instance.order
    for i=1,#order do
        local c = order[i]
        for k,v in next, c do
            if not instance.variables[k] then
                if instance.environment[k] then
                    instance.variables[k] = instance.environment[k]
                else
                    instance.kpsevars[k] = true
                    instance.variables[k] = resolvers.bare_variable(v)
                end
            end
        end
    end
end

function resolvers.load_cnf()
    local function loadoldconfigdata()
        local cnffiles = instance.cnffiles
        for i=1,#cnffiles do
            load_cnf_file(cnffiles[i])
        end
    end
    -- instance.cnffiles contain complete names now !
    -- we still use a funny mix of cnf and new but soon
    -- we will switch to lua exclusively as we only use
    -- the file to collect the tree roots
    if #instance.cnffiles == 0 then
        if trace_locating then
            logs.report("fileio","no cnf files found (TEXMFCNF may not be set/known)")
        end
    else
        local cnffiles = instance.cnffiles
        instance.rootpath = cnffiles[1]
        for k=1,#cnffiles do
            instance.cnffiles[k] = file.collapse_path(cnffiles[k])
        end
        for i=1,3 do
            instance.rootpath = file.dirname(instance.rootpath)
        end
        instance.rootpath = file.collapse_path(instance.rootpath)
        if instance.diskcache and not instance.renewcache then
            resolvers.loadoldconfig(instance.cnffiles)
            if instance.loaderror then
                loadoldconfigdata()
                resolvers.saveoldconfig()
            end
        else
            loadoldconfigdata()
            if instance.renewcache then
                resolvers.saveoldconfig()
            end
        end
        collapse_cnf_data()
    end
    check_configuration()
end

function resolvers.load_lua()
    if #instance.luafiles == 0 then
        -- yet harmless
    else
        instance.rootpath = instance.luafiles[1]
        local luafiles = instance.luafiles
        for k=1,#luafiles do
            instance.luafiles[k] = file.collapse_path(luafiles[k])
        end
        for i=1,3 do
            instance.rootpath = file.dirname(instance.rootpath)
        end
        instance.rootpath = file.collapse_path(instance.rootpath)
        resolvers.loadnewconfig()
        collapse_cnf_data()
    end
    check_configuration()
end

-- database loading

function resolvers.load_hash()
    resolvers.locatelists()
    if instance.diskcache and not instance.renewcache then
        resolvers.loadfiles()
        if instance.loaderror then
            resolvers.loadlists()
            resolvers.savefiles()
        end
    else
        resolvers.loadlists()
        if instance.renewcache then
            resolvers.savefiles()
        end
    end
end

function resolvers.append_hash(type,tag,name)
    if trace_locating then
        logs.report("fileio","hash '%s' appended",tag)
    end
    insert(instance.hashes, { ['type']=type, ['tag']=tag, ['name']=name } )
end

function resolvers.prepend_hash(type,tag,name)
    if trace_locating then
        logs.report("fileio","hash '%s' prepended",tag)
    end
    insert(instance.hashes, 1, { ['type']=type, ['tag']=tag, ['name']=name } )
end

function resolvers.extend_texmf_var(specification) -- crap, we could better prepend the hash
--  local t = resolvers.expanded_path_list('TEXMF') -- full expansion
    local t = resolvers.split_path(resolvers.env('TEXMF'))
    insert(t,1,specification)
    local newspec = concat(t,";")
    if instance.environment["TEXMF"] then
        instance.environment["TEXMF"] = newspec
    elseif instance.variables["TEXMF"] then
        instance.variables["TEXMF"] = newspec
    else
        -- weird
    end
    resolvers.expand_variables()
    reset_hashes()
end

-- locators

function resolvers.locatelists()
    local texmfpaths = resolvers.clean_path_list('TEXMF')
    for i=1,#texmfpaths do
        local path = texmfpaths[i]
        if trace_locating then
            logs.report("fileio","locating list of '%s'",path)
        end
        resolvers.locatedatabase(file.collapse_path(path))
    end
end

function resolvers.locatedatabase(specification)
    return resolvers.methodhandler('locators', specification)
end

function resolvers.locators.tex(specification)
    if specification and specification ~= '' and lfs.isdir(specification) then
        if trace_locating then
            logs.report("fileio","tex locator '%s' found",specification)
        end
        resolvers.append_hash('file',specification,filename)
    elseif trace_locating then
        logs.report("fileio","tex locator '%s' not found",specification)
    end
end

-- hashers

function resolvers.hashdatabase(tag,name)
    return resolvers.methodhandler('hashers',tag,name)
end

function resolvers.loadfiles()
    instance.loaderror = false
    instance.files = { }
    if not instance.renewcache then
        local hashes = instance.hashes
        for k=1,#hashes do
            local hash = hashes[k]
            resolvers.hashdatabase(hash.tag,hash.name)
            if instance.loaderror then break end
        end
    end
end

function resolvers.hashers.tex(tag,name)
    resolvers.load_data(tag,'files')
end

-- generators:

function resolvers.loadlists()
    local hashes = instance.hashes
    for i=1,#hashes do
        resolvers.generatedatabase(hashes[i].tag)
    end
end

function resolvers.generatedatabase(specification)
    return resolvers.methodhandler('generators', specification)
end

-- starting with . or .. etc or funny char

local weird = lpeg.P(".")^1 + lpeg.anywhere(lpeg.S("~`!#$%^&*()={}[]:;\"\'||<>,?\n\r\t"))

--~ local l_forbidden = lpeg.S("~`!#$%^&*()={}[]:;\"\'||\\/<>,?\n\r\t")
--~ local l_confusing = lpeg.P(" ")
--~ local l_character = lpeg.patterns.utf8
--~ local l_dangerous = lpeg.P(".")

--~ local l_normal = (l_character - l_forbidden - l_confusing - l_dangerous) * (l_character - l_forbidden - l_confusing^2)^0 * lpeg.P(-1)
--~ ----- l_normal = l_normal * lpeg.Cc(true) + lpeg.Cc(false)

--~ local function test(str)
--~     print(str,lpeg.match(l_normal,str))
--~ end
--~ test("ヒラギノ明朝 Pro W3")
--~ test("..ヒラギノ明朝 Pro W3")
--~ test(":ヒラギノ明朝 Pro W3;")
--~ test("ヒラギノ明朝 /Pro W3;")
--~ test("ヒラギノ明朝 Pro  W3")

function resolvers.generators.tex(specification)
    local tag = specification
    if trace_locating then
        logs.report("fileio","scanning path '%s'",specification)
    end
    instance.files[tag] = { }
    local files = instance.files[tag]
    local n, m, r = 0, 0, 0
    local spec = specification .. '/'
    local attributes = lfs.attributes
    local directory = lfs.dir
    local function action(path)
        local full
        if path then
            full = spec .. path .. '/'
        else
            full = spec
        end
        for name in directory(full) do
            if not lpegmatch(weird,name) then
         -- if lpegmatch(l_normal,name) then
                local mode = attributes(full..name,'mode')
                if mode == 'file' then
                    if path then
                        n = n + 1
                        local f = files[name]
                        if f then
                            if type(f) == 'string' then
                                files[name] = { f, path }
                            else
                                f[#f+1] = path
                            end
                        else -- probably unique anyway
                            files[name] = path
                            local lower = lower(name)
                            if name ~= lower then
                                files["remap:"..lower] = name
                                r = r + 1
                            end
                        end
                    end
                elseif mode == 'directory' then
                    m = m + 1
                    if path then
                        action(path..'/'..name)
                    else
                        action(name)
                    end
                end
            end
        end
    end
    action()
    if trace_locating then
        logs.report("fileio","%s files found on %s directories with %s uppercase remappings",n,m,r)
    end
end

-- savers, todo

function resolvers.savefiles()
    resolvers.save_data('files')
end

-- A config (optionally) has the paths split in tables. Internally
-- we join them and split them after the expansion has taken place. This
-- is more convenient.

--~ local checkedsplit = string.checkedsplit

local cache = { }

local splitter = lpeg.Ct(lpeg.splitat(lpeg.S(os.type == "windows" and ";" or ":;")))

local function split_kpse_path(str) -- beware, this can be either a path or a {specification}
    local found = cache[str]
    if not found then
        if str == "" then
            found = { }
        else
            str = gsub(str,"\\","/")
--~             local split = (find(str,";") and checkedsplit(str,";")) or checkedsplit(str,io.pathseparator)
local split = lpegmatch(splitter,str)
            found = { }
            for i=1,#split do
                local s = split[i]
                if not find(s,"^{*unset}*") then
                    found[#found+1] = s
                end
            end
            if trace_expansions then
                logs.report("fileio","splitting path specification '%s'",str)
                for k=1,#found do
                    logs.report("fileio","% 4i: %s",k,found[k])
                end
            end
            cache[str] = found
        end
    end
    return found
end

resolvers.split_kpse_path = split_kpse_path

function resolvers.splitconfig()
    for i=1,#instance do
        local c = instance[i]
        for k,v in next, c do
            if type(v) == 'string' then
                local t = split_kpse_path(v)
                if #t > 1 then
                    c[k] = t
                end
            end
        end
    end
end

function resolvers.joinconfig()
    local order = instance.order
    for i=1,#order do
        local c = order[i]
        for k,v in next, c do -- indexed?
            if type(v) == 'table' then
                c[k] = file.join_path(v)
            end
        end
    end
end

function resolvers.split_path(str)
    if type(str) == 'table' then
        return str
    else
        return split_kpse_path(str)
    end
end

function resolvers.join_path(str)
    if type(str) == 'table' then
        return file.join_path(str)
    else
        return str
    end
end

function resolvers.splitexpansions()
    local ie = instance.expansions
    for k,v in next, ie do
        local t, h, p = { }, { }, split_kpse_path(v)
        for kk=1,#p do
            local vv = p[kk]
            if vv ~= "" and not h[vv] then
                t[#t+1] = vv
                h[vv] = true
            end
        end
        if #t > 1 then
            ie[k] = t
        else
            ie[k] = t[1]
        end
    end
end

-- end of split/join code

function resolvers.saveoldconfig()
    resolvers.splitconfig()
    resolvers.save_data('configuration')
    resolvers.joinconfig()
end

resolvers.configbanner = [[
-- This is a Luatex configuration file created by 'luatools.lua' or
-- 'luatex.exe' directly. For comment, suggestions and questions you can
-- contact the ConTeXt Development Team. This configuration file is
-- not copyrighted. [HH & TH]
]]

function resolvers.serialize(files)
    -- This version is somewhat optimized for the kind of
    -- tables that we deal with, so it's much faster than
    -- the generic serializer. This makes sense because
    -- luatools and mtxtools are called frequently. Okay,
    -- we pay a small price for properly tabbed tables.
    local t = { }
    local function dump(k,v,m) -- could be moved inline
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
	local sortedfiles = sortedkeys(files)
	for i=1,#sortedfiles do
	    local k = sortedfiles[i]
            local fk  = files[k]
            if type(fk) == 'table' then
                t[#t+1] = "\t['" .. k .. "']={"
		local sortedfk = sortedkeys(fk)
        	for j=1,#sortedfk do
                    local kk = sortedfk[j]
                    t[#t+1] = dump(kk,fk[kk],"\t\t")
                end
                t[#t+1] = "\t},"
            else
                t[#t+1] = dump(k,fk,"\t")
            end
        end
    else
        for k, v in next, files do
            if type(v) == 'table' then
                t[#t+1] = "\t['" .. k .. "']={"
                for kk,vv in next, v do
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

local data_state = { }

function resolvers.data_state()
    return data_state or { }
end

function resolvers.save_data(dataname, makename) -- untested without cache overload
    for cachename, files in next, instance[dataname] do
        local name = (makename or file.join)(cachename,dataname)
        local luaname, lucname = name .. ".lua", name .. ".luc"
        if trace_locating then
            logs.report("fileio","preparing '%s' for '%s'",dataname,cachename)
        end
        for k, v in next, files do
            if type(v) == "table" and #v == 1 then
                files[k] = v[1]
            end
        end
        local data = {
            type    = dataname,
            root    = cachename,
            version = resolvers.cacheversion,
            date    = os.date("%Y-%m-%d"),
            time    = os.date("%H:%M:%S"),
            content = files,
            uuid    = os.uuid(),
        }
        local ok = io.savedata(luaname,resolvers.serialize(data))
        if ok then
            if trace_locating then
                logs.report("fileio","'%s' saved in '%s'",dataname,luaname)
            end
            if utils.lua.compile(luaname,lucname,false,true) then -- no cleanup but strip
                if trace_locating then
                    logs.report("fileio","'%s' compiled to '%s'",dataname,lucname)
                end
            else
                if trace_locating then
                    logs.report("fileio","compiling failed for '%s', deleting file '%s'",dataname,lucname)
                end
                os.remove(lucname)
            end
        elseif trace_locating then
            logs.report("fileio","unable to save '%s' in '%s' (access error)",dataname,luaname)
        end
    end
end

function resolvers.load_data(pathname,dataname,filename,makename) -- untested without cache overload
    filename = ((not filename or (filename == "")) and dataname) or filename
    filename = (makename and makename(dataname,filename)) or file.join(pathname,filename)
    local blob = loadfile(filename .. ".luc") or loadfile(filename .. ".lua")
    if blob then
        local data = blob()
        if data and data.content and data.type == dataname and data.version == resolvers.cacheversion then
            data_state[#data_state+1] = data.uuid
            if trace_locating then
                logs.report("fileio","loading '%s' for '%s' from '%s'",dataname,pathname,filename)
            end
            instance[dataname][pathname] = data.content
        else
            if trace_locating then
                logs.report("fileio","skipping '%s' for '%s' from '%s'",dataname,pathname,filename)
            end
            instance[dataname][pathname] = { }
            instance.loaderror = true
        end
    elseif trace_locating then
        logs.report("fileio","skipping '%s' for '%s' from '%s'",dataname,pathname,filename)
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

function resolvers.resetconfig()
    identify_own()
    instance.configuration, instance.setup, instance.order, instance.loaderror = { }, { }, { }, false
end

function resolvers.loadnewconfig()
    local luafiles = instance.luafiles
    for i=1,#luafiles do
        local cnf = luafiles[i]
        local pathname = file.dirname(cnf)
        local filename = file.join(pathname,resolvers.luaname)
        local blob = loadfile(filename)
        if blob then
            local data = blob()
            if data then
                if trace_locating then
                    logs.report("fileio","loading configuration file '%s'",filename)
                end
                if true then
                    -- flatten to variable.progname
                    local t = { }
                    for k, v in next, data do -- v = progname
                        if type(v) == "string" then
                            t[k] = v
                        else
                            for kk, vv in next, v do -- vv = variable
                                if type(vv) == "string" then
                                    t[vv.."."..v] = kk
                                end
                            end
                        end
                    end
                    instance['setup'][pathname] = t
                else
                    instance['setup'][pathname] = data
                end
            else
                if trace_locating then
                    logs.report("fileio","skipping configuration file '%s'",filename)
                end
                instance['setup'][pathname] = { }
                instance.loaderror = true
            end
        elseif trace_locating then
            logs.report("fileio","skipping configuration file '%s'",filename)
        end
        instance.order[#instance.order+1] = instance.setup[pathname]
        if instance.loaderror then break end
    end
end

function resolvers.loadoldconfig()
    if not instance.renewcache then
        local cnffiles = instance.cnffiles
        for i=1,#cnffiles do
            local cnf = cnffiles[i]
            local dname = file.dirname(cnf)
            resolvers.load_data(dname,'configuration')
            instance.order[#instance.order+1] = instance.configuration[dname]
            if instance.loaderror then break end
        end
    end
    resolvers.joinconfig()
end

function resolvers.expand_variables()
    local expansions, environment, variables = { }, instance.environment, instance.variables
    local env = resolvers.env
    instance.expansions = expansions
    if instance.engine   ~= "" then environment['engine']   = instance.engine   end
    if instance.progname ~= "" then environment['progname'] = instance.progname end
    for k,v in next, environment do
        local a, b = match(k,"^(%a+)%_(.*)%s*$")
        if a and b then
            expansions[a..'.'..b] = v
        else
            expansions[k] = v
        end
    end
    for k,v in next, environment do -- move environment to expansions
        if not expansions[k] then expansions[k] = v end
    end
    for k,v in next, variables do -- move variables to expansions
        if not expansions[k] then expansions[k] = v end
    end
    local busy = false
    local function resolve(a)
        busy = true
        return expansions[a] or env(a)
    end
    while true do
        busy = false
        for k,v in next, expansions do
            local s, n = gsub(v,"%$([%a%d%_%-]+)",resolve)
            local s, m = gsub(s,"%$%{([%a%d%_%-]+)%}",resolve)
            if n > 0 or m > 0 then
                expansions[k]= s
            end
        end
        if not busy then break end
    end
    for k,v in next, expansions do
        expansions[k] = gsub(v,"\\", '/')
    end
end

function resolvers.variable(name)
    return entry(instance.variables,name)
end

function resolvers.expansion(name)
    return entry(instance.expansions,name)
end

function resolvers.is_variable(name)
    return is_entry(instance.variables,name)
end

function resolvers.is_expansion(name)
    return is_entry(instance.expansions,name)
end

function resolvers.unexpanded_path_list(str)
    local pth = resolvers.variable(str)
    local lst = resolvers.split_path(pth)
    return expanded_path_from_list(lst)
end

function resolvers.unexpanded_path(str)
    return file.join_path(resolvers.unexpanded_path_list(str))
end

do -- no longer needed

    local done = { }

    function resolvers.reset_extra_path()
        local ep = instance.extra_paths
        if not ep then
            ep, done = { }, { }
            instance.extra_paths = ep
        elseif #ep > 0 then
            instance.lists, done = { }, { }
        end
    end

    function resolvers.register_extra_path(paths,subpaths)
        local ep = instance.extra_paths or { }
        local n = #ep
        if paths and paths ~= "" then
            if subpaths and subpaths ~= "" then
                for p in gmatch(paths,"[^,]+") do
                    -- we gmatch each step again, not that fast, but used seldom
                    for s in gmatch(subpaths,"[^,]+") do
                        local ps = p .. "/" .. s
                        if not done[ps] then
                            ep[#ep+1] = resolvers.clean_path(ps)
                            done[ps] = true
                        end
                    end
                end
            else
                for p in gmatch(paths,"[^,]+") do
                    if not done[p] then
                        ep[#ep+1] = resolvers.clean_path(p)
                        done[p] = true
                    end
                end
            end
        elseif subpaths and subpaths ~= "" then
            for i=1,n do
                -- we gmatch each step again, not that fast, but used seldom
                for s in gmatch(subpaths,"[^,]+") do
                    local ps = ep[i] .. "/" .. s
                    if not done[ps] then
                        ep[#ep+1] = resolvers.clean_path(ps)
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

local function made_list(instance,list)
    local ep = instance.extra_paths
    if not ep or #ep == 0 then
        return list
    else
        local done, new = { }, { }
        -- honour . .. ../.. but only when at the start
        for k=1,#list do
            local v = list[k]
            if not done[v] then
                if find(v,"^[%.%/]$") then
                    done[v] = true
                    new[#new+1] = v
                else
                    break
                end
            end
        end
        -- first the extra paths
        for k=1,#ep do
            local v = ep[k]
            if not done[v] then
                done[v] = true
                new[#new+1] = v
            end
        end
        -- next the formal paths
        for k=1,#list do
            local v = list[k]
            if not done[v] then
                done[v] = true
                new[#new+1] = v
            end
        end
        return new
    end
end

function resolvers.clean_path_list(str)
    local t = resolvers.expanded_path_list(str)
    if t then
        for i=1,#t do
            t[i] = file.collapse_path(resolvers.clean_path(t[i]))
        end
    end
    return t
end

function resolvers.expand_path(str)
    return file.join_path(resolvers.expanded_path_list(str))
end

function resolvers.expanded_path_list(str)
    if not str then
        return ep or { } -- ep ?
    elseif instance.savelists then
        -- engine+progname hash
        str = gsub(str,"%$","")
        if not instance.lists[str] then -- cached
            local lst = made_list(instance,resolvers.split_path(resolvers.expansion(str)))
            instance.lists[str] = expanded_path_from_list(lst)
        end
        return instance.lists[str]
    else
        local lst = resolvers.split_path(resolvers.expansion(str))
        return made_list(instance,expanded_path_from_list(lst))
    end
end

function resolvers.expanded_path_list_from_var(str) -- brrr
    local tmp = resolvers.var_of_format_or_suffix(gsub(str,"%$",""))
    if tmp ~= "" then
        return resolvers.expanded_path_list(tmp)
    else
        return resolvers.expanded_path_list(str)
    end
end

function resolvers.expand_path_from_var(str)
    return file.join_path(resolvers.expanded_path_list_from_var(str))
end

function resolvers.format_of_var(str)
    return formats[str] or formats[alternatives[str]] or ''
end
function resolvers.format_of_suffix(str)
    return suffixmap[file.extname(str)] or 'tex'
end

function resolvers.variable_of_format(str)
    return formats[str] or formats[alternatives[str]] or ''
end

function resolvers.var_of_format_or_suffix(str)
    local v = formats[str]
    if v then
        return v
    end
    v = formats[alternatives[str]]
    if v then
        return v
    end
    v = suffixmap[file.extname(str)]
    if v then
        return formats[isf]
    end
    return ''
end

function resolvers.expand_braces(str) -- output variable and brace expansion of STRING
    local ori = resolvers.variable(str)
    local pth = expanded_path_from_list(resolvers.split_path(ori))
    return file.join_path(pth)
end

resolvers.isreadable = { }

function resolvers.isreadable.file(name)
    local readable = lfs.isfile(name) -- brrr
    if trace_detail then
        if readable then
            logs.report("fileio","file '%s' is readable",name)
        else
            logs.report("fileio","file '%s' is not readable", name)
        end
    end
    return readable
end

resolvers.isreadable.tex = resolvers.isreadable.file

-- name
-- name/name

local function collect_files(names)
    local filelist = { }
    for k=1,#names do
        local fname = names[k]
        if trace_detail then
            logs.report("fileio","checking name '%s'",fname)
        end
        local bname = file.basename(fname)
        local dname = file.dirname(fname)
        if dname == "" or find(dname,"^%.") then
            dname = false
        else
            dname = "/" .. dname .. "$"
        end
        local hashes = instance.hashes
        for h=1,#hashes do
            local hash = hashes[h]
            local blobpath = hash.tag
            local files = blobpath and instance.files[blobpath]
            if files then
                if trace_detail then
                    logs.report("fileio","deep checking '%s' (%s)",blobpath,bname)
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
                        if not dname or find(blobfile,dname) then
                            filelist[#filelist+1] = {
                                hash.type,
                                file.join(blobpath,blobfile,bname), -- search
                                resolvers.concatinators[hash.type](blobpath,blobfile,bname) -- result
                            }
                        end
                    else
                        for kk=1,#blobfile do
                            local vv = blobfile[kk]
                            if not dname or find(vv,dname) then
                                filelist[#filelist+1] = {
                                    hash.type,
                                    file.join(blobpath,vv,bname), -- search
                                    resolvers.concatinators[hash.type](blobpath,vv,bname) -- result
                                }
                            end
                        end
                    end
                end
            elseif trace_locating then
                logs.report("fileio","no match in '%s' (%s)",blobpath,bname)
            end
        end
    end
    if #filelist > 0 then
        return filelist
    else
        return nil
    end
end

function resolvers.suffix_of_format(str)
    if suffixes[str] then
        return suffixes[str][1]
    else
        return ""
    end
end

function resolvers.suffixes_of_format(str)
    if suffixes[str] then
        return suffixes[str]
    else
        return {}
    end
end

function resolvers.register_in_trees(name)
    if not find(name,"^%.") then
        instance.foundintrees[name] = (instance.foundintrees[name] or 0) + 1 -- maybe only one
    end
end

-- split the next one up for readability (bu this module needs a cleanup anyway)

local function can_be_dir(name) -- can become local
    local fakepaths = instance.fakepaths
    if not fakepaths[name] then
        if lfs.isdir(name) then
            fakepaths[name] = 1 -- directory
        else
            fakepaths[name] = 2 -- no directory
        end
    end
    return (fakepaths[name] == 1)
end

local function collect_instance_files(filename,collected) -- todo : plugin (scanners, checkers etc)
    local result = collected or { }
    local stamp  = nil
    filename = file.collapse_path(filename)
    -- speed up / beware: format problem
    if instance.remember then
        stamp = filename .. "--" .. instance.engine .. "--" .. instance.progname .. "--" .. instance.format
        if instance.found[stamp] then
            if trace_locating then
                logs.report("fileio","remembering file '%s'",filename)
            end
            return instance.found[stamp]
        end
    end
    if not dangerous[instance.format or "?"] then
        if resolvers.isreadable.file(filename) then
            if trace_detail then
                logs.report("fileio","file '%s' found directly",filename)
            end
            instance.found[stamp] = { filename }
            return { filename }
        end
    end
    if find(filename,'%*') then
        if trace_locating then
            logs.report("fileio","checking wildcard '%s'", filename)
        end
        result = resolvers.find_wildcard_files(filename)
    elseif file.is_qualified_path(filename) then
        if resolvers.isreadable.file(filename) then
            if trace_locating then
                logs.report("fileio","qualified name '%s'", filename)
            end
            result = { filename }
        else
            local forcedname, ok, suffix = "", false, file.extname(filename)
            if suffix == "" then -- why
                if instance.format == "" then
                    forcedname = filename .. ".tex"
                    if resolvers.isreadable.file(forcedname) then
                        if trace_locating then
                            logs.report("fileio","no suffix, forcing standard filetype 'tex'")
                        end
                        result, ok = { forcedname }, true
                    end
                else
                    local suffixes = resolvers.suffixes_of_format(instance.format)
                    for _, s in next, suffixes do
                        forcedname = filename .. "." .. s
                        if resolvers.isreadable.file(forcedname) then
                            if trace_locating then
                                logs.report("fileio","no suffix, forcing format filetype '%s'", s)
                            end
                            result, ok = { forcedname }, true
                            break
                        end
                    end
                end
            end
            if not ok and suffix ~= "" then
                -- try to find in tree (no suffix manipulation), here we search for the
                -- matching last part of the name
                local basename = file.basename(filename)
                local pattern = gsub(filename .. "$","([%.%-])","%%%1")
                local savedformat = instance.format
                local format = savedformat or ""
                if format == "" then
                    instance.format = resolvers.format_of_suffix(suffix)
                end
                if not format then
                    instance.format = "othertextfiles" -- kind of everything, maybe texinput is better
                end
                --
                if basename ~= filename then
                    local resolved = collect_instance_files(basename)
                    if #result == 0 then
                        local lowered = lower(basename)
                        if filename ~= lowered then
                            resolved = collect_instance_files(lowered)
                        end
                    end
                    resolvers.format = savedformat
                    --
                    for r=1,#resolved do
                        local rr = resolved[r]
                        if find(rr,pattern) then
                            result[#result+1], ok = rr, true
                        end
                    end
                end
                -- a real wildcard:
                --
                -- if not ok then
                --     local filelist = collect_files({basename})
                --     for f=1,#filelist do
                --         local ff = filelist[f][3] or ""
                --         if find(ff,pattern) then
                --             result[#result+1], ok = ff, true
                --         end
                --     end
                -- end
            end
            if not ok and trace_locating then
                logs.report("fileio","qualified name '%s'", filename)
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
                filetype = resolvers.format_of_suffix(forcedname)
                if trace_locating then
                    logs.report("fileio","forcing filetype '%s'",filetype)
                end
            else
                filetype = resolvers.format_of_suffix(filename)
                if trace_locating then
                    logs.report("fileio","using suffix based filetype '%s'",filetype)
                end
            end
        else
            if ext == "" then
                local suffixes = resolvers.suffixes_of_format(instance.format)
                for _, s in next, suffixes do
                    wantedfiles[#wantedfiles+1] = filename .. "." .. s
                end
            end
            filetype = instance.format
            if trace_locating then
                logs.report("fileio","using given filetype '%s'",filetype)
            end
        end
        local typespec = resolvers.variable_of_format(filetype)
        local pathlist = resolvers.expanded_path_list(typespec)
        if not pathlist or #pathlist == 0 then
            -- no pathlist, access check only / todo == wildcard
            if trace_detail then
                logs.report("fileio","checking filename '%s', filetype '%s', wanted files '%s'",filename, filetype or '?',concat(wantedfiles," | "))
            end
            for k=1,#wantedfiles do
                local fname = wantedfiles[k]
                if fname and resolvers.isreadable.file(fname) then
                    filename, done = fname, true
                    result[#result+1] = file.join('.',fname)
                    break
                end
            end
            -- this is actually 'other text files' or 'any' or 'whatever'
            local filelist = collect_files(wantedfiles)
            local fl = filelist and filelist[1]
            if fl then
                filename = fl[3]
                result[#result+1] = filename
                done = true
            end
        else
            -- list search
            local filelist = collect_files(wantedfiles)
            local dirlist = { }
            if filelist then
                for i=1,#filelist do
                    dirlist[i] = file.dirname(filelist[i][2]) .. "/"
                end
            end
            if trace_detail then
                logs.report("fileio","checking filename '%s'",filename)
            end
            -- a bit messy ... esp the doscan setting here
            local doscan
            for k=1,#pathlist do
                local path = pathlist[k]
                if find(path,"^!!") then doscan  = false else doscan  = true  end
                local pathname = gsub(path,"^!+", '')
                done = false
                -- using file list
                if filelist then
                    local expression
                    -- compare list entries with permitted pattern -- /xx /xx//
                    if not find(pathname,"/$") then
                        expression = pathname .. "/"
                    else
                        expression = pathname
                    end
                    expression = gsub(expression,"([%-%.])","%%%1") -- this also influences
                    expression = gsub(expression,"//+$", '/.*')     -- later usage of pathname
                    expression = gsub(expression,"//", '/.-/')      -- not ok for /// but harmless
                    expression = "^" .. expression .. "$"
                    if trace_detail then
                        logs.report("fileio","using pattern '%s' for path '%s'",expression,pathname)
                    end
                    for k=1,#filelist do
                        local fl = filelist[k]
                        local f = fl[2]
                        local d = dirlist[k]
                        if find(d,expression) then
                            --- todo, test for readable
                            result[#result+1] = fl[3]
                            resolvers.register_in_trees(f) -- for tracing used files
                            done = true
                            if instance.allresults then
                                if trace_detail then
                                    logs.report("fileio","match in hash for file '%s' on path '%s', continue scanning",f,d)
                                end
                            else
                                if trace_detail then
                                    logs.report("fileio","match in hash for file '%s' on path '%s', quit scanning",f,d)
                                end
                                break
                            end
                        elseif trace_detail then
                            logs.report("fileio","no match in hash for file '%s' on path '%s'",f,d)
                        end
                    end
                end
                if not done and doscan then
                    -- check if on disk / unchecked / does not work at all / also zips
                    if resolvers.splitmethod(pathname).scheme == 'file' then -- ?
                        local pname = gsub(pathname,"%.%*$",'')
                        if not find(pname,"%*") then
                            local ppname = gsub(pname,"/+$","")
                            if can_be_dir(ppname) then
                                for k=1,#wantedfiles do
                                    local w = wantedfiles[k]
                                    local fname = file.join(ppname,w)
                                    if resolvers.isreadable.file(fname) then
                                        if trace_detail then
                                            logs.report("fileio","found '%s' by scanning",fname)
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
    for k=1,#result do
        result[k] = file.collapse_path(result[k])
    end
    if instance.remember then
        instance.found[stamp] = result
    end
    return result
end

if not resolvers.concatinators  then resolvers.concatinators = { } end

resolvers.concatinators.tex  = file.join
resolvers.concatinators.file = resolvers.concatinators.tex

function resolvers.find_files(filename,filetype,mustexist)
    if type(mustexist) == boolean then
        -- all set
    elseif type(filetype) == 'boolean' then
        filetype, mustexist = nil, false
    elseif type(filetype) ~= 'string' then
        filetype, mustexist = nil, false
    end
    instance.format = filetype or ''
    local result = collect_instance_files(filename)
    if #result == 0 then
        local lowered = lower(filename)
        if filename ~= lowered then
            return collect_instance_files(lowered)
        end
    end
    instance.format = ''
    return result
end

function resolvers.find_file(filename,filetype,mustexist)
    return (resolvers.find_files(filename,filetype,mustexist)[1] or "")
end

function resolvers.find_given_files(filename)
    local bname, result = file.basename(filename), { }
    local hashes = instance.hashes
    for k=1,#hashes do
        local hash = hashes[k]
        local files = instance.files[hash.tag] or { }
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
                result[#result+1] = resolvers.concatinators[hash.type](hash.tag,blist,bname) or ""
                if not instance.allresults then break end
            else
                for kk=1,#blist do
                    local vv = blist[kk]
                    result[#result+1] = resolvers.concatinators[hash.type](hash.tag,vv,bname) or ""
                    if not instance.allresults then break end
                end
            end
        end
    end
    return result
end

function resolvers.find_given_file(filename)
    return (resolvers.find_given_files(filename)[1] or "")
end

local function doit(path,blist,bname,tag,kind,result,allresults)
    local done = false
    if blist and kind then
        if type(blist) == 'string' then
            -- make function and share code
            if find(lower(blist),path) then
                result[#result+1] = resolvers.concatinators[kind](tag,blist,bname) or ""
                done = true
            end
        else
            for kk=1,#blist do
                local vv = blist[kk]
                if find(lower(vv),path) then
                    result[#result+1] = resolvers.concatinators[kind](tag,vv,bname) or ""
                    done = true
                    if not allresults then break end
                end
            end
        end
    end
    return done
end

function resolvers.find_wildcard_files(filename) -- todo: remap:
    local result = { }
    local bname, dname = file.basename(filename), file.dirname(filename)
    local path = gsub(dname,"^*/","")
    path = gsub(path,"*",".*")
    path = gsub(path,"-","%%-")
    if dname == "" then
        path = ".*"
    end
    local name = bname
    name = gsub(name,"*",".*")
    name = gsub(name,"-","%%-")
    path = lower(path)
    name = lower(name)
    local files, allresults, done = instance.files, instance.allresults, false
    if find(name,"%*") then
        local hashes = instance.hashes
        for k=1,#hashes do
            local hash = hashes[k]
            local tag, kind = hash.tag, hash.type
            for kk, hh in next, files[hash.tag] do
                if not find(kk,"^remap:") then
                    if find(lower(kk),name) then
                        if doit(path,hh,kk,tag,kind,result,allresults) then done = true end
                        if done and not allresults then break end
                    end
                end
            end
        end
    else
        local hashes = instance.hashes
        for k=1,#hashes do
            local hash = hashes[k]
            local tag, kind = hash.tag, hash.type
            if doit(path,files[tag][bname],bname,tag,kind,result,allresults) then done = true end
            if done and not allresults then break end
        end
    end
    -- we can consider also searching the paths not in the database, but then
    -- we end up with a messy search (all // in all path specs)
    return result
end

function resolvers.find_wildcard_file(filename)
    return (resolvers.find_wildcard_files(filename)[1] or "")
end

-- main user functions

function resolvers.automount()
    -- implemented later
end

function resolvers.load(option)
    statistics.starttiming(instance)
    resolvers.resetconfig()
    resolvers.identify_cnf()
    resolvers.load_lua() -- will become the new method
    resolvers.expand_variables()
    resolvers.load_cnf() -- will be skipped when we have a lua file
    resolvers.expand_variables()
    if option ~= "nofiles" then
        resolvers.load_hash()
        resolvers.automount()
    end
    statistics.stoptiming(instance)
end

function resolvers.for_files(command, files, filetype, mustexist)
    if files and #files > 0 then
        local function report(str)
            if trace_locating then
                logs.report("fileio",str) -- has already verbose
            else
                print(str)
            end
        end
        if trace_locating then
            report('') -- ?
        end
        for f=1,#files do
            local file = files[f]
            local result = command(file,filetype,mustexist)
            if type(result) == 'string' then
                report(result)
            else
                for i=1,#result do
                    report(result[i]) -- could be unpack
                end
            end
        end
    end
end

-- strtab

resolvers.var_value  = resolvers.variable   -- output the value of variable $STRING.
resolvers.expand_var = resolvers.expansion  -- output variable expansion of STRING.

function resolvers.show_path(str)     -- output search path for file type NAME
    return file.join_path(resolvers.expanded_path_list(resolvers.format_of_var(str)))
end

-- resolvers.find_file(filename)
-- resolvers.find_file(filename, filetype, mustexist)
-- resolvers.find_file(filename, mustexist)
-- resolvers.find_file(filename, filetype)

function resolvers.register_file(files, name, path)
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

function resolvers.splitmethod(filename)
    if not filename then
        return { } -- safeguard
    elseif type(filename) == "table" then
        return filename -- already split
    elseif not find(filename,"://") then
        return { scheme="file", path = filename, original=filename } -- quick hack
    else
        return url.hashed(filename)
    end
end

function table.sequenced(t,sep) -- temp here
    local s = { }
    for k, v in next, t do -- indexed?
        s[#s+1] = k .. "=" .. tostring(v)
    end
    return concat(s, sep or " | ")
end

function resolvers.methodhandler(what, filename, filetype) -- ...
    filename = file.collapse_path(filename)
    local specification = (type(filename) == "string" and resolvers.splitmethod(filename)) or filename -- no or { }, let it bomb
    local scheme = specification.scheme
    if resolvers[what][scheme] then
        if trace_locating then
            logs.report("fileio","handler '%s' -> '%s' -> '%s'",specification.original,what,table.sequenced(specification))
        end
        return resolvers[what][scheme](filename,filetype) -- todo: specification
    else
        return resolvers[what].tex(filename,filetype) -- todo: specification
    end
end

function resolvers.clean_path(str)
    if str then
        str = gsub(str,"\\","/")
        str = gsub(str,"^!+","")
        str = gsub(str,"^~",resolvers.homedir)
        return str
    else
        return nil
    end
end

function resolvers.do_with_path(name,func)
    local pathlist = resolvers.expanded_path_list(name)
    for i=1,#pathlist do
        func("^"..resolvers.clean_path(pathlist[i]))
    end
end

function resolvers.do_with_var(name,func)
    func(expanded_var(name))
end

function resolvers.with_files(pattern,handle)
    local hashes = instance.hashes
    for i=1,#hashes do
        local hash = hashes[i]
        local blobpath = hash.tag
        local blobtype = hash.type
        if blobpath then
            local files = instance.files[blobpath]
            if files then
                for k,v in next, files do
                    if find(k,"^remap:") then
                        k = files[k]
                        v = files[k] -- chained
                    end
                    if find(k,pattern) then
                        if type(v) == "string" then
                            handle(blobtype,blobpath,v,k)
                        else
                            for _,vv in next, v do -- indexed
                                handle(blobtype,blobpath,vv,k)
                            end
                        end
                    end
                end
            end
        end
    end
end

function resolvers.locate_format(name)
    local barename, fmtname = gsub(name,"%.%a+$",""), ""
    if resolvers.usecache then
        local path = file.join(caches.setpath("formats")) -- maybe platform
        fmtname = file.join(path,barename..".fmt") or ""
    end
    if fmtname == "" then
        fmtname = resolvers.find_files(barename..".fmt")[1] or ""
    end
    fmtname = resolvers.clean_path(fmtname)
    if fmtname ~= "" then
        local barename = file.removesuffix(fmtname)
        local luaname, lucname, luiname = barename .. ".lua", barename .. ".luc", barename .. ".lui"
        if lfs.isfile(luiname) then
            return barename, luiname
        elseif lfs.isfile(lucname) then
            return barename, lucname
        elseif lfs.isfile(luaname) then
            return barename, luaname
        end
    end
    return nil, nil
end

function resolvers.boolean_variable(str,default)
    local b = resolvers.expansion(str)
    if b == "" then
        return default
    else
        b = toboolean(b)
        return (b == nil and default) or b
    end
end

texconfig.kpse_init = false

kpse = { original = kpse } setmetatable(kpse, { __index = function(k,v) return resolvers[v] end } )

-- for a while

input = resolvers


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-tmp'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
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

local format, lower, gsub = string.format, string.lower, string.gsub

local trace_cache = false  trackers.register("resolvers.cache", function(v) trace_cache = v end) -- not used yet

caches = caches or { }

caches.path     = caches.path or nil
caches.base     = caches.base or "luatex-cache"
caches.more     = caches.more or "context"
caches.direct   = false -- true is faster but may need huge amounts of memory
caches.tree     = false
caches.paths    = caches.paths or nil
caches.force    = false
caches.defaults = { "TEXMFCACHE", "TMPDIR", "TEMPDIR", "TMP", "TEMP", "HOME", "HOMEPATH" }

function caches.temp()
    local cachepath = nil
    local function check(list,isenv)
        if not cachepath then
            for k=1,#list do
                local v = list[k]
                cachepath = (isenv and (os.env[v] or "")) or v or ""
                if cachepath == "" then
                    -- next
                else
                    cachepath = resolvers.clean_path(cachepath)
                    if lfs.isdir(cachepath) and file.iswritable(cachepath) then -- lfs.attributes(cachepath,"mode") == "directory"
                        break
                    elseif caches.force or io.ask(format("\nShould I create the cache path %s?",cachepath), "no", { "yes", "no" }) == "yes" then
                        dir.mkdirs(cachepath)
                        if lfs.isdir(cachepath) and file.iswritable(cachepath) then
                            break
                        end
                    end
                end
                cachepath = nil
            end
        end
    end
    check(resolvers.clean_path_list("TEXMFCACHE") or { })
    check(caches.defaults,true)
    if not cachepath then
        print("\nfatal error: there is no valid (writable) cache path defined\n")
        os.exit()
    elseif not lfs.isdir(cachepath) then -- lfs.attributes(cachepath,"mode") ~= "directory"
        print(format("\nfatal error: cache path %s is not a directory\n",cachepath))
        os.exit()
    end
    cachepath = file.collapse_path(cachepath)
    function caches.temp()
        return cachepath
    end
    return cachepath
end

function caches.configpath()
    return table.concat(resolvers.instance.cnffiles,";")
end

function caches.hashed(tree)
    return md5.hex(gsub(lower(tree),"[\\\/]+","/"))
end

function caches.treehash()
    local tree = caches.configpath()
    if not tree or tree == "" then
        return false
    else
        return caches.hashed(tree)
    end
end

function caches.setpath(...)
    if not caches.path then
        if not caches.path then
            caches.path = caches.temp()
        end
        caches.path = resolvers.clean_path(caches.path) -- to be sure
        caches.tree = caches.tree or caches.treehash()
        if caches.tree then
            caches.path = dir.mkdirs(caches.path,caches.base,caches.more,caches.tree)
        else
            caches.path = dir.mkdirs(caches.path,caches.base,caches.more)
        end
    end
    if not caches.path then
        caches.path = '.'
    end
    caches.path = resolvers.clean_path(caches.path)
    local dirs = { ... }
    if #dirs > 0 then
        local pth = dir.mkdirs(caches.path,...)
        return pth
    end
    caches.path = dir.expand_name(caches.path)
    return caches.path
end

function caches.definepath(category,subcategory)
    return function()
        return caches.setpath(category,subcategory)
    end
end

function caches.setluanames(path,name)
    return path .. "/" .. name .. ".tma", path .. "/" .. name .. ".tmc"
end

function caches.loaddata(path,name)
    local tmaname, tmcname = caches.setluanames(path,name)
    local loader = loadfile(tmcname) or loadfile(tmaname)
    if loader then
        loader = loader()
        collectgarbage("step")
        return loader
    else
        return false
    end
end

--~ function caches.loaddata(path,name)
--~     local tmaname, tmcname = caches.setluanames(path,name)
--~     return dofile(tmcname) or dofile(tmaname)
--~ end

function caches.iswritable(filepath,filename)
    local tmaname, tmcname = caches.setluanames(filepath,filename)
    return file.iswritable(tmaname)
end

function caches.savedata(filepath,filename,data,raw)
    local tmaname, tmcname = caches.setluanames(filepath,filename)
    local reduce, simplify = true, true
    if raw then
        reduce, simplify = false, false
    end
    data.cache_uuid = os.uuid()
    if caches.direct then
        file.savedata(tmaname, table.serialize(data,'return',false,true,false)) -- no hex
    else
        table.tofile(tmaname, data,'return',false,true,false) -- maybe not the last true
    end
    local cleanup = resolvers.boolean_variable("PURGECACHE", false)
    local strip = resolvers.boolean_variable("LUACSTRIP", true)
    utils.lua.compile(tmaname, tmcname, cleanup, strip)
end

-- here we use the cache for format loading (texconfig.[formatname|jobname])

--~ if tex and texconfig and texconfig.formatname and texconfig.formatname == "" then
if tex and texconfig and (not texconfig.formatname or texconfig.formatname == "") and input and resolvers.instance then
    if not texconfig.luaname then texconfig.luaname = "cont-en.lua" end -- or luc
    texconfig.formatname = caches.setpath("formats") .. "/" .. gsub(texconfig.luaname,"%.lu.$",".fmt")
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-inp'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

resolvers.finders = resolvers.finders or { }
resolvers.openers = resolvers.openers or { }
resolvers.loaders = resolvers.loaders or { }

resolvers.finders.notfound  = { nil }
resolvers.openers.notfound  = { nil }
resolvers.loaders.notfound  = { false, nil, 0 }


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-out'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

outputs = outputs or { }



end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-con'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local format, lower, gsub = string.format, string.lower, string.gsub

local trace_cache      = false  trackers.register("resolvers.cache",      function(v) trace_cache      = v end)
local trace_containers = false  trackers.register("resolvers.containers", function(v) trace_containers = v end)
local trace_storage    = false  trackers.register("resolvers.storage",    function(v) trace_storage    = v end)

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

containers = containers or { }

containers.usecache = true

local function report(container,tag,name)
    if trace_cache or trace_containers then
        logs.report(format("%s cache",container.subcategory),"%s: %s",tag,name or 'invalid')
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
                    path = caches and caches.setpath and caches.setpath(category,subcategory),
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
    return container.enabled and caches and caches.iswritable(container.path, name)
end

function containers.is_valid(container, name)
    if name and name ~= "" then
        local storage = container.storage[name]
        return storage and storage.cache_version == container.version
    else
        return false
    end
end

function containers.read(container,name)
    if container.enabled and caches and not container.storage[name] and containers.usecache then
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
        if container.enabled and caches then
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

function containers.cleanname(name)
    return (gsub(lower(name),"[^%w%d]+","-"))
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-use'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local format, lower, gsub, find = string.format, string.lower, string.gsub, string.find

local trace_locating = false  trackers.register("resolvers.locating", function(v) trace_locating = v end)

-- since we want to use the cache instead of the tree, we will now
-- reimplement the saver.

local save_data = resolvers.save_data
local load_data = resolvers.load_data

resolvers.cachepath = nil  -- public, for tracing
resolvers.usecache  = true -- public, for tracing

function resolvers.save_data(dataname)
    save_data(dataname, function(cachename,dataname)
        resolvers.usecache = not toboolean(resolvers.expansion("CACHEINTDS") or "false",true)
        if resolvers.usecache then
            resolvers.cachepath = resolvers.cachepath or caches.definepath("trees")
            return file.join(resolvers.cachepath(),caches.hashed(cachename))
        else
            return file.join(cachename,dataname)
        end
    end)
end

function resolvers.load_data(pathname,dataname,filename)
    load_data(pathname,dataname,filename,function(dataname,filename)
        resolvers.usecache = not toboolean(resolvers.expansion("CACHEINTDS") or "false",true)
        if resolvers.usecache then
            resolvers.cachepath = resolvers.cachepath or caches.definepath("trees")
            return file.join(resolvers.cachepath(),caches.hashed(pathname))
        else
            if not filename or (filename == "") then
                filename = dataname
            end
            return file.join(pathname,filename)
        end
    end)
end

-- we will make a better format, maybe something xml or just text or lua

resolvers.automounted = resolvers.automounted or { }

function resolvers.automount(usecache)
    local mountpaths = resolvers.clean_path_list(resolvers.expansion('TEXMFMOUNT'))
    if (not mountpaths or #mountpaths == 0) and usecache then
        mountpaths = { caches.setpath("mount") }
    end
    if mountpaths and #mountpaths > 0 then
        statistics.starttiming(resolvers.instance)
        for k=1,#mountpaths do
            local root = mountpaths[k]
            local f = io.open(root.."/url.tmi")
            if f then
                for line in f:lines() do
                    if line then
                        if find(line,"^[%%#%-]") then -- or %W
                            -- skip
                        elseif find(line,"^zip://") then
                            if trace_locating then
                                logs.report("fileio","mounting %s",line)
                            end
                            table.insert(resolvers.automounted,line)
                            resolvers.usezipfile(line)
                        end
                    end
                end
                f:close()
            end
        end
        statistics.stoptiming(resolvers.instance)
    end
end

-- status info

statistics.register("used config path", function() return caches.configpath()  end)
statistics.register("used cache path",  function() return caches.temp() or "?" end)

-- experiment (code will move)

function statistics.save_fmt_status(texname,formatbanner,sourcefile) -- texname == formatname
    local enginebanner = status.list().banner
    if formatbanner and enginebanner and sourcefile then
        local luvname = file.replacesuffix(texname,"luv")
        local luvdata = {
            enginebanner = enginebanner,
            formatbanner = formatbanner,
            sourcehash   = md5.hex(io.loaddata(resolvers.find_file(sourcefile)) or "unknown"),
            sourcefile   = sourcefile,
        }
        io.savedata(luvname,table.serialize(luvdata,true))
    end
end

function statistics.check_fmt_status(texname)
    local enginebanner = status.list().banner
    if enginebanner and texname then
        local luvname = file.replacesuffix(texname,"luv")
        if lfs.isfile(luvname) then
            local luv = dofile(luvname)
            if luv and luv.sourcefile then
                local sourcehash = md5.hex(io.loaddata(resolvers.find_file(luv.sourcefile)) or "unknown")
                local luvbanner = luv.enginebanner or "?"
                if luvbanner ~= enginebanner then
                    return string.format("engine mismatch (luv:%s <> bin:%s)",luvbanner,enginebanner)
                end
                local luvhash = luv.sourcehash or "?"
                if luvhash ~= sourcehash then
                    return string.format("source mismatch (luv:%s <> bin:%s)",luvhash,sourcehash)
                end
            else
                return "invalid status file"
            end
        else
            return "missing status file"
        end
    end
    return true
end


end -- of closure

do -- create closure to overcome 200 locals limit

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

<p>How about just forgetting about them?</p>
--ldx]]--

local suffixes = resolvers.suffixes
local formats  = resolvers.formats

suffixes['gf']                       = { '<resolution>gf' }
suffixes['pk']                       = { '<resolution>pk' }
suffixes['base']                     = { 'base' }
suffixes['bib']                      = { 'bib' }
suffixes['bst']                      = { 'bst' }
suffixes['cnf']                      = { 'cnf' }
suffixes['mem']                      = { 'mem' }
suffixes['mf']                       = { 'mf' }
suffixes['mfpool']                   = { 'pool' }
suffixes['mft']                      = { 'mft' }
suffixes['mppool']                   = { 'pool' }
suffixes['graphic/figure']           = { 'eps', 'epsi' }
suffixes['texpool']                  = { 'pool' }
suffixes['PostScript header']        = { 'pro' }
suffixes['ist']                      = { 'ist' }
suffixes['web']                      = { 'web', 'ch' }
suffixes['cweb']                     = { 'w', 'web', 'ch' }
suffixes['cmap files']               = { 'cmap' }
suffixes['lig files']                = { 'lig' }
suffixes['bitmap font']              = { }
suffixes['MetaPost support']         = { }
suffixes['TeX system documentation'] = { }
suffixes['TeX system sources']       = { }
suffixes['dvips config']             = { }
suffixes['type42 fonts']             = { }
suffixes['web2c files']              = { }
suffixes['other text files']         = { }
suffixes['other binary files']       = { }
suffixes['opentype fonts']           = { 'otf' }

suffixes['fmt']                      = { 'fmt' }
suffixes['texmfscripts']             = { 'rb','lua','py','pl' }

suffixes['pdftex config']            = { }
suffixes['Troff fonts']              = { }

suffixes['ls-R']                     = { }

--[[ldx--
<p>If you wondered abou tsome of the previous mappings, how about
the next bunch:</p>
--ldx]]--

formats['bib']                      = ''
formats['bst']                      = ''
formats['mft']                      = ''
formats['ist']                      = ''
formats['web']                      = ''
formats['cweb']                     = ''
formats['MetaPost support']         = ''
formats['TeX system documentation'] = ''
formats['TeX system sources']       = ''
formats['Troff fonts']              = ''
formats['dvips config']             = ''
formats['graphic/figure']           = ''
formats['ls-R']                     = ''
formats['other text files']         = ''
formats['other binary files']       = ''

formats['gf']                       = ''
formats['pk']                       = ''
formats['base']                     = 'MFBASES'
formats['cnf']                      = ''
formats['mem']                      = 'MPMEMS'
formats['mf']                       = 'MFINPUTS'
formats['mfpool']                   = 'MFPOOL'
formats['mppool']                   = 'MPPOOL'
formats['texpool']                  = 'TEXPOOL'
formats['PostScript header']        = 'TEXPSHEADERS'
formats['cmap files']               = 'CMAPFONTS'
formats['type42 fonts']             = 'T42FONTS'
formats['web2c files']              = 'WEB2C'
formats['pdftex config']            = 'PDFTEXCONFIG'
formats['texmfscripts']             = 'TEXMFSCRIPTS'
formats['bitmap font']              = ''
formats['lig files']                = 'LIGFONTS'


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-aux'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

local find = string.find

local trace_locating = false  trackers.register("resolvers.locating", function(v) trace_locating = v end)

function resolvers.update_script(oldname,newname) -- oldname -> own.name, not per se a suffix
    local scriptpath = "scripts/context/lua"
    newname = file.addsuffix(newname,"lua")
    local oldscript = resolvers.clean_path(oldname)
    if trace_locating then
        logs.report("fileio","to be replaced old script %s", oldscript)
    end
    local newscripts = resolvers.find_files(newname) or { }
    if #newscripts == 0 then
        if trace_locating then
            logs.report("fileio","unable to locate new script")
        end
    else
        for i=1,#newscripts do
            local newscript = resolvers.clean_path(newscripts[i])
            if trace_locating then
                logs.report("fileio","checking new script %s", newscript)
            end
            if oldscript == newscript then
                if trace_locating then
                    logs.report("fileio","old and new script are the same")
                end
            elseif not find(newscript,scriptpath) then
                if trace_locating then
                    logs.report("fileio","new script should come from %s",scriptpath)
                end
            elseif not (find(oldscript,file.removesuffix(newname).."$") or find(oldscript,newname.."$")) then
                if trace_locating then
                    logs.report("fileio","invalid new script name")
                end
            else
                local newdata = io.loaddata(newscript)
                if newdata then
                    if trace_locating then
                        logs.report("fileio","old script content replaced by new content")
                    end
                    io.savedata(oldscript,newdata)
                    break
                elseif trace_locating then
                    logs.report("fileio","unable to load new script")
                end
            end
        end
    end
end


end -- of closure

do -- create closure to overcome 200 locals limit

if not modules then modules = { } end modules ['data-lst'] = {
    version   = 1.001,
    comment   = "companion to luat-lib.mkiv",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- used in mtxrun

local find, concat, upper, format = string.find, table.concat, string.upper, string.format

resolvers.listers = resolvers.listers or { }

local function tabstr(str)
    if type(str) == 'table' then
        return concat(str," | ")
    else
        return str
    end
end

local function list(list,report)
    local instance = resolvers.instance
    local pat = upper(pattern or "","")
    local report = report or texio.write_nl
    local sorted = table.sortedkeys(list)
    for i=1,#sorted do
        local key = sorted[i]
        if instance.pattern == "" or find(upper(key),pat) then
            if instance.kpseonly then
                if instance.kpsevars[key] then
                    report(format("%s=%s",key,tabstr(list[key])))
                end
            else
                report(format('%s %s=%s',(instance.kpsevars[key] and 'K') or 'E',key,tabstr(list[key])))
            end
        end
    end
end

function resolvers.listers.variables () list(resolvers.instance.variables ) end
function resolvers.listers.expansions() list(resolvers.instance.expansions) end

function resolvers.listers.configurations(report)
    local report = report or texio.write_nl
    local instance = resolvers.instance
    local sorted = table.sortedkeys(instance.kpsevars)
    for i=1,#sorted do
        local key = sorted[i]
        if not instance.pattern or (instance.pattern=="") or find(key,instance.pattern) then
            report(format("%s\n",key))
            local order = instance.order
            for i=1,#order do
                local str = order[i][key]
                if str then
                    report(format("\t%s\t%s",i,str))
                end
            end
            report("")
        end
    end
end


end -- of closure
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
    'l-file.lua',
    'l-md5.lua',
    'l-url.lua',
    'l-dir.lua',
    'l-boolean.lua',
    'l-unicode.lua',
    'l-math.lua',
    'l-utils.lua',
    'l-aux.lua',
    'trac-tra.lua',
    'luat-env.lua',
    'trac-inf.lua',
    'trac-log.lua',
    'data-res.lua',
    'data-tmp.lua',
--  'data-pre.lua',
    'data-inp.lua',
    'data-out.lua',
    'data-con.lua',
    'data-use.lua',
--  'data-tex.lua',
--  'data-bin.lua',
--  'data-zip.lua',
--  'data-crl.lua',
--  'data-lua.lua',
    'data-kps.lua', -- so that we can replace kpsewhich
    'data-aux.lua', -- updater
    'data-lst.lua', -- lister
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

if not resolvers then
    locate_libs()
end

if not resolvers then
    print("")
    print("Luatools is unable to start up due to lack of libraries. You may")
    print("try to run 'lua luatools.lua --selfmerge' in the path where this")
    print("script is located (normally under ..../scripts/context/lua) which")
    print("will make luatools library independent.")
    os.exit()
end

logs.setprogram('LuaTools',"TDS Management Tool 1.32",environment.arguments["verbose"] or false)

local instance = resolvers.reset()

resolvers.defaultlibs = { -- not all are needed (this will become: context.lus (lua spec)
    'l-string.lua',
    'l-lpeg.lua',
    'l-table.lua',
    'l-boolean.lua',
    'l-number.lua',
    'l-unicode.lua',
    'l-os.lua',
    'l-io.lua',
    'l-file.lua',
    'l-md5.lua',
    'l-url.lua',
    'l-dir.lua',
    'l-utils.lua',
    'l-dimen.lua',
    'trac-inf.lua',
    'trac-tra.lua',
    'trac-log.lua',
    'luat-env.lua', -- here ?
    'data-res.lua',
    'data-inp.lua',
    'data-out.lua',
    'data-tmp.lua',
    'data-con.lua',
    'data-use.lua',
--  'data-pre.lua',
    'data-tex.lua',
    'data-bin.lua',
--  'data-zip.lua',
--  'data-clr.lua',
    'data-lua.lua',
    'data-ctx.lua',
    'luat-fio.lua',
    'luat-cnf.lua',
}

instance.engine     =     environment.arguments["engine"]   or 'luatex'
instance.progname   =     environment.arguments["progname"] or 'context'
instance.luaname    =     environment.arguments["luafile"]  or "" -- environment.ownname or ""
instance.lualibs    =     environment.arguments["lualibs"]  or table.concat(resolvers.defaultlibs,",")
instance.allresults =     environment.arguments["all"]      or false
instance.pattern    =     environment.arguments["pattern"]  or nil
instance.sortdata   =     environment.arguments["sort"]     or false
instance.kpseonly   = not environment.arguments["all"]      or false
instance.my_format  =     environment.arguments["format"]   or instance.format

if type(instance.pattern) == 'boolean' then
    logs.simple("invalid pattern specification")
    instance.pattern = nil
end

if environment.arguments["trace"] then resolvers.settrace(environment.arguments["trace"]) end

local trackspec = environment.argument("trackers") or environment.argument("track")

if trackspec then
    trackers.enable(trackspec)
end

runners  = runners  or { }
messages = messages or { }

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
--verbose         give a bit more info
--all             show all found files
--sort            sort cached data
--engine=str      target engine
--progname=str    format or backend
--pattern=str     filter variables
--trackers=list   enable given trackers
]]

function runners.make_format(texname)
    local instance = resolvers.instance
    if texname and texname ~= "" then
        if resolvers.usecache then
            local path = file.join(caches.setpath("formats")) -- maybe platform
            if path and lfs then
                lfs.chdir(path)
            end
        end
        local barename = texname:gsub("%.%a+$","")
        if barename == texname then
            texname = texname .. ".tex"
        end
        local fullname = resolvers.find_files(texname)[1] or ""
        if fullname == "" then
            logs.simple("no tex file with name: %s",texname)
        else
            local luaname, lucname, luapath, lualibs = "", "", "", { }
            -- the following is optional, since context.lua can also
            -- handle this collect and compile business
            if environment.arguments["compile"] then
                if luaname == "" then luaname = barename end
                logs.simple("creating initialization file: %s",luaname)
                luapath = file.dirname(luaname)
                if luapath == "" then
                    luapath = file.dirname(texname)
                end
                if luapath == "" then
                    luapath = file.dirname(resolvers.find_files(texname)[1] or "")
                end
                lualibs = string.split(instance.lualibs,",")
                luaname = file.basename(barename .. ".lua")
                lucname = file.basename(barename .. ".luc")
                -- todo: when this fails, we can just copy the merged libraries from
                -- luatools since they are normally the same, at least for context
                if lualibs[1] then
                    local firstlib = file.join(luapath,lualibs[1])
                    if not lfs.isfile(firstlib) then
                        local foundname = resolvers.find_files(lualibs[1])[1]
                        if foundname then
                            logs.simple("located library path: %s",luapath)
                            luapath = file.dirname(foundname)
                        end
                    end
                end
                logs.simple("using library path: %s",luapath)
                logs.simple("using lua libraries: %s",table.join(lualibs," "))
                utils.merger.selfcreate(lualibs,luapath,luaname)
                local strip = resolvers.boolean_variable("LUACSTRIP", true)
                if utils.lua.compile(luaname,lucname,false,strip) and io.exists(lucname) then
                    luaname = lucname
                    logs.simple("using compiled initialization file: %s",lucname)
                else
                    logs.simple("using uncompiled initialization file: %s",luaname)
                end
            else
                local what = { instance.luaname, instance.progname, barename }
                for k=1,#what do
                    local v = string.gsub(what[k]..".lua","%.lua%.lua$",".lua")
                    if v and (v ~= "") then
                        luaname = resolvers.find_files(v)[1] or ""
                        if luaname ~= "" then
                            break
                        end
                    end
                end
            end
            if environment.arguments["noluc"] then
                luaname = luaname:gsub("%.luc$",".lua") -- make this an option
            end
            if luaname == "" then
                if logs.verbose then
                    logs.simplelines(messages.no_ini_file)
                    logs.simple("texname : %s",texname)
                    logs.simple("luaname : %s",instance.luaname)
                    logs.simple("progname: %s",instance.progname)
                    logs.simple("barename: %s",barename)
                end
            else
                logs.simple("using lua initialization file: %s",luaname)
                local mp = dir.glob(file.removesuffix(file.basename(luaname)).."-*.mem")
                if mp and #mp > 0 then
                    for i=1,#mp do
                        local name = mp[i]
                        logs.simple("removing related mplib format %s", file.basename(name))
                        os.remove(name)
                    end
                end
                local flags = {
                    "--ini",
                    "--lua=" .. string.quote(luaname)
                }
                local bs = (os.platform == "unix" and "\\\\") or "\\" -- todo: make a function
                local command = "luatex ".. table.concat(flags," ")  .. " " .. string.quote(fullname) .. " " .. bs .. "dump"
                logs.simple("running command: %s\n",command)
                os.spawn(command)
                -- todo: do a dummy run that generates the related metafun and mfplain formats
            end
        end
    else
        logs.simple("no tex file given")
    end
end

function runners.run_format(name,data,more)
 -- hm, rather old code here; we can now use the file.whatever functions
    if name and (name ~= "") then
        local barename = name:gsub("%.%a+$","")
        local fmtname = ""
        if resolvers.usecache then
            local path = file.join(caches.setpath("formats")) -- maybe platform
            fmtname = file.join(path,barename..".fmt") or ""
        end
        if fmtname == "" then
            fmtname = resolvers.find_files(barename..".fmt")[1] or ""
        end
        fmtname = resolvers.clean_path(fmtname)
        barename = fmtname:gsub("%.%a+$","")
        if fmtname == "" then
            logs.simple("no format with name: %s",name)
        else
            local luaname = barename .. ".luc"
            local f = io.open(luaname)
            if not f then
                luaname = barename .. ".lua"
                f = io.open(luaname)
            end
            if f then
                f:close()
                local command = "luatex --fmt=" .. string.quote(barename) .. " --lua=" .. string.quote(luaname) .. " " .. string.quote(data) .. " " .. (more ~= "" and string.quote(more) or "")
                logs.simple("running command: %s",command)
                os.spawn(command)
            else
                logs.simple("using format name: %s",fmtname)
                logs.simple("no luc/lua with name: %s",barename)
            end
        end
    end
end

local ok = true

-- private option --noluc for testing errors in the stub

if environment.arguments["find-file"] then
    resolvers.load()
    instance.format  = environment.arguments["format"] or instance.format
    if instance.pattern then
        instance.allresults = true
        resolvers.for_files(resolvers.find_files, { instance.pattern }, instance.my_format)
    else
        resolvers.for_files(resolvers.find_files, environment.files, instance.my_format)
    end
elseif environment.arguments["find-path"] then
    resolvers.load()
    local path = resolvers.find_file(environment.files[1], instance.my_format)
    if logs.verbose then
        logs.simple(file.dirname(path))
    else
        print(file.dirname(path))
    end
elseif environment.arguments["run"] then
    resolvers.load("nofiles") -- ! no need for loading databases
    logs.setverbose(true)
    runners.run_format(environment.files[1] or "",environment.files[2] or "",environment.files[3] or "")
elseif environment.arguments["fmt"] then
    resolvers.load("nofiles") -- ! no need for loading databases
    logs.setverbose(true)
    runners.run_format(environment.arguments["fmt"], environment.files[1] or "",environment.files[2] or "")
elseif environment.arguments["expand-braces"] then
    resolvers.load("nofiles")
    resolvers.for_files(resolvers.expand_braces, environment.files)
elseif environment.arguments["expand-path"] then
    resolvers.load("nofiles")
    resolvers.for_files(resolvers.expand_path, environment.files)
elseif environment.arguments["expand-var"] or environment.arguments["expand-variable"] then
    resolvers.load("nofiles")
    resolvers.for_files(resolvers.expand_var, environment.files)
elseif environment.arguments["show-path"] or environment.arguments["path-value"] then
    resolvers.load("nofiles")
    resolvers.for_files(resolvers.show_path, environment.files)
elseif environment.arguments["var-value"] or environment.arguments["show-value"] then
    resolvers.load("nofiles")
    resolvers.for_files(resolvers.var_value, environment.files)
elseif environment.arguments["format-path"] then
    resolvers.load()
    logs.simple(caches.setpath("format"))
elseif instance.pattern then -- brrr
    resolvers.load()
    instance.format = environment.arguments["format"] or instance.format
    instance.allresults = true
    resolvers.for_files(resolvers.find_files, { instance.pattern }, instance.my_format)
elseif environment.arguments["generate"] then
    instance.renewcache = true
    logs.setverbose(true)
    resolvers.load()
elseif environment.arguments["make"] or environment.arguments["ini"] or environment.arguments["compile"] then
    resolvers.load()
    logs.setverbose(true)
    runners.make_format(environment.files[1] or "")
elseif environment.arguments["selfmerge"] then
    utils.merger.selfmerge(own.name,own.libs,own.list)
elseif environment.arguments["selfclean"] then
    utils.merger.selfclean(own.name)
elseif environment.arguments["selfupdate"] then
    resolvers.load()
    logs.setverbose(true)
    resolvers.update_script(own.name,"luatools")
elseif environment.arguments["variables"] or environment.arguments["show-variables"] then
    resolvers.load("nofiles")
    resolvers.listers.variables()
elseif environment.arguments["expansions"] or environment.arguments["show-expansions"] then
    resolvers.load("nofiles")
    resolvers.listers.expansions()
elseif environment.arguments["configurations"] or environment.arguments["show-configurations"] then
    resolvers.load("nofiles")
    resolvers.listers.configurations()
elseif environment.arguments["help"] or (environment.files[1]=='help') or (#environment.files==0) then
    logs.help(messages.help)
else
    resolvers.load()
    resolvers.for_files(resolvers.find_files, environment.files, instance.my_format)
end

if logs.verbose then
    logs.simpleline()
    logs.simple("runtime: %0.3f seconds",os.runtime())
end

if os.platform == "unix" then
    io.write("\n")
end
