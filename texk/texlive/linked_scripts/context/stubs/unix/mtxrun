#!/usr/bin/env texlua

-- for k, v in next, _G.string do
--     local tv = type(v)
--     if tv == "table" then
--         for kk, vv in next, v do
--             print(k,kk,vv)
--         end
--     else
--         print(tv,k,v)
--     end
-- end

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
-- library also gives way more control.

-- to be done / considered
--
-- support for --exec or make it default
-- support for jar files (or maybe not, never used, too messy)
-- support for $RUBYINPUTS cum suis (if still needed)
-- remember for subruns: _CTX_K_V_#{original}_
-- remember for subruns: _CTX_K_S_#{original}_
-- remember for subruns: TEXMFSTART.#{original} [tex.rb texmfstart.rb]

-- begin library merge



do -- create closure to overcome 200 locals limit

package.loaded["l-lua"] = package.loaded["l-lua"] or true

-- original size: 10352, stripped down to: 5955

if not modules then modules={} end modules ['l-lua']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local major,minor=string.match(_VERSION,"^[^%d]+(%d+)%.(%d+).*$")
_MAJORVERSION=tonumber(major) or 5
_MINORVERSION=tonumber(minor) or 1
_LUAVERSION=_MAJORVERSION+_MINORVERSION/10
if not lpeg then
  lpeg=require("lpeg")
end
if loadstring then
  local loadnormal=load
  function load(first,...)
    if type(first)=="string" then
      return loadstring(first,...)
    else
      return loadnormal(first,...)
    end
  end
else
  loadstring=load
end
if not ipairs then
  local function iterate(a,i)
    i=i+1
    local v=a[i]
    if v~=nil then
      return i,v 
    end
  end
  function ipairs(a)
    return iterate,a,0
  end
end
if not pairs then
  function pairs(t)
    return next,t 
  end
end
if not table.unpack then
  table.unpack=_G.unpack
elseif not unpack then
  _G.unpack=table.unpack
end
if not package.loaders then 
  package.loaders=package.searchers
end
local print,select,tostring=print,select,tostring
local inspectors={}
function setinspector(inspector) 
  inspectors[#inspectors+1]=inspector
end
function inspect(...) 
  for s=1,select("#",...) do
    local value=select(s,...)
    local done=false
    for i=1,#inspectors do
      done=inspectors[i](value)
      if done then
        break
      end
    end
    if not done then
      print(tostring(value))
    end
  end
end
local dummy=function() end
function optionalrequire(...)
  local ok,result=xpcall(require,dummy,...)
  if ok then
    return result
  end
end
local type=type
local gsub,format=string.gsub,string.format
local package=package
local searchers=package.searchers or package.loaders
local libpaths=nil
local clibpaths=nil
local libhash={}
local clibhash={}
local libextras={}
local clibextras={}
local filejoin=file and file.join    or function(path,name)  return path.."/"..name end
local isreadable=file and file.is_readable or function(name)    local f=io.open(name) if f then f:close() return true end end
local addsuffix=file and file.addsuffix  or function(name,suffix) return name.."."..suffix end
local function cleanpath(path) 
  return path
end
local helpers=package.helpers or {
  libpaths=function() return {} end,
  clibpaths=function() return {} end,
  cleanpath=cleanpath,
  trace=false,
  report=function(...) print(format(...)) end,
}
package.helpers=helpers
local function getlibpaths()
  return libpaths or helpers.libpaths(libhash)
end
local function getclibpaths()
  return clibpaths or helpers.clibpaths(clibhash)
end
package.libpaths=getlibpaths
package.clibpaths=getclibpaths
local function addpath(what,paths,extras,hash,...)
  local pathlist={... }
  local cleanpath=helpers.cleanpath
  local trace=helpers.trace
  local report=helpers.report
  local function add(path)
    local path=cleanpath(path)
    if not hash[path] then
      if trace then
        report("extra %s path: %s",what,path)
      end
      paths [#paths+1]=path
      extras[#extras+1]=path
    end
  end
  for p=1,#pathlist do
    local path=pathlist[p]
    if type(path)=="table" then
      for i=1,#path do
        add(path[i])
      end
    else
      add(path)
    end
  end
  return paths,extras
end
function package.extralibpath(...)
   libpaths,libextras=addpath("lua",getlibpaths(),libextras,libhash,...)
end
function package.extraclibpath(...)
  clibpaths,clibextras=addpath("lib",getclibpaths(),clibextras,clibhash,...)
end
if not searchers[-2] then
  searchers[-2]=searchers[2]
end
searchers[2]=function(name)
  return helpers.loaded(name)
end
searchers[3]=nil 
local function loadedaslib(resolved,rawname)
  local init="luaopen_"..gsub(rawname,"%.","_")
  if helpers.trace then
    helpers.report("calling loadlib with '%s' with init '%s'",resolved,init)
  end
  return package.loadlib(resolved,init)
end
local function loadedbylua(name)
  if helpers.trace then
    helpers.report("locating '%s' using normal loader",name)
  end
  return true,searchers[-2](name) 
end
local function loadedbypath(name,rawname,paths,islib,what)
  local trace=helpers.trace
  local report=helpers.report
  if trace then
    report("locating '%s' as '%s' on '%s' paths",rawname,name,what)
  end
  for p=1,#paths do
    local path=paths[p]
    local resolved=filejoin(path,name)
    if trace then 
      report("checking for '%s' using '%s' path '%s'",name,what,path)
    end
    if isreadable(resolved) then
      if trace then
        report("lib '%s' located on '%s'",name,resolved)
      end
      if islib then
        return true,loadedaslib(resolved,rawname)
      else
        return true,loadfile(resolved)
      end
    end
  end
end
local function notloaded(name)
  if helpers.trace then
    helpers.report("unable to locate library '%s'",name)
  end
end
helpers.loadedaslib=loadedaslib
helpers.loadedbylua=loadedbylua
helpers.loadedbypath=loadedbypath
helpers.notloaded=notloaded
function helpers.loaded(name)
  local thename=gsub(name,"%.","/")
  local luaname=addsuffix(thename,"lua")
  local libname=addsuffix(thename,os.libsuffix or "so") 
  local libpaths=getlibpaths()
  local clibpaths=getclibpaths()
  local done,result=loadedbypath(luaname,name,libpaths,false,"lua")
  if done then
    return result
  end
  local done,result=loadedbypath(luaname,name,clibpaths,false,"lua")
  if done then
    return result
  end
  local done,result=loadedbypath(libname,name,clibpaths,true,"lib")
  if done then
    return result
  end
  local done,result=loadedbylua(name)
  if done then
    return result
  end
  return notloaded(name)
end
function helpers.unload(name)
  if helpers.trace then
    if package.loaded[name] then
      helpers.report("unloading library '%s', %s",name,"done")
    else
      helpers.report("unloading library '%s', %s",name,"not loaded")
    end
  end
  package.loaded[name]=nil
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-lpeg"] = package.loaded["l-lpeg"] or true

-- original size: 26252, stripped down to: 14371

if not modules then modules={} end modules ['l-lpeg']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
lpeg=require("lpeg")
local type,next,tostring=type,next,tostring
local byte,char,gmatch,format=string.byte,string.char,string.gmatch,string.format
local floor=math.floor
local P,R,S,V,Ct,C,Cs,Cc,Cp,Cmt=lpeg.P,lpeg.R,lpeg.S,lpeg.V,lpeg.Ct,lpeg.C,lpeg.Cs,lpeg.Cc,lpeg.Cp,lpeg.Cmt
local lpegtype,lpegmatch,lpegprint=lpeg.type,lpeg.match,lpeg.print
setinspector(function(v) if lpegtype(v) then lpegprint(v) return true end end)
lpeg.patterns=lpeg.patterns or {} 
local patterns=lpeg.patterns
local anything=P(1)
local endofstring=P(-1)
local alwaysmatched=P(true)
patterns.anything=anything
patterns.endofstring=endofstring
patterns.beginofstring=alwaysmatched
patterns.alwaysmatched=alwaysmatched
local digit,sign=R('09'),S('+-')
local cr,lf,crlf=P("\r"),P("\n"),P("\r\n")
local newline=crlf+S("\r\n") 
local escaped=P("\\")*anything
local squote=P("'")
local dquote=P('"')
local space=P(" ")
local utfbom_32_be=P('\000\000\254\255')
local utfbom_32_le=P('\255\254\000\000')
local utfbom_16_be=P('\255\254')
local utfbom_16_le=P('\254\255')
local utfbom_8=P('\239\187\191')
local utfbom=utfbom_32_be+utfbom_32_le+utfbom_16_be+utfbom_16_le+utfbom_8
local utftype=utfbom_32_be*Cc("utf-32-be")+utfbom_32_le*Cc("utf-32-le")+utfbom_16_be*Cc("utf-16-be")+utfbom_16_le*Cc("utf-16-le")+utfbom_8*Cc("utf-8")+alwaysmatched*Cc("utf-8") 
local utfoffset=utfbom_32_be*Cc(4)+utfbom_32_le*Cc(4)+utfbom_16_be*Cc(2)+utfbom_16_le*Cc(2)+utfbom_8*Cc(3)+Cc(0)
local utf8next=R("\128\191")
patterns.utf8one=R("\000\127")
patterns.utf8two=R("\194\223")*utf8next
patterns.utf8three=R("\224\239")*utf8next*utf8next
patterns.utf8four=R("\240\244")*utf8next*utf8next*utf8next
patterns.utfbom=utfbom
patterns.utftype=utftype
patterns.utfoffset=utfoffset
local utf8char=patterns.utf8one+patterns.utf8two+patterns.utf8three+patterns.utf8four
local validutf8char=utf8char^0*endofstring*Cc(true)+Cc(false)
local utf8character=P(1)*R("\128\191")^0 
patterns.utf8=utf8char
patterns.utf8char=utf8char
patterns.utf8character=utf8character 
patterns.validutf8=validutf8char
patterns.validutf8char=validutf8char
local eol=S("\n\r")
local spacer=S(" \t\f\v") 
local whitespace=eol+spacer
local nonspacer=1-spacer
local nonwhitespace=1-whitespace
patterns.eol=eol
patterns.spacer=spacer
patterns.whitespace=whitespace
patterns.nonspacer=nonspacer
patterns.nonwhitespace=nonwhitespace
local stripper=spacer^0*C((spacer^0*nonspacer^1)^0)
local collapser=Cs(spacer^0/""*nonspacer^0*((spacer^0/" "*nonspacer^1)^0))
patterns.stripper=stripper
patterns.collapser=collapser
patterns.digit=digit
patterns.sign=sign
patterns.cardinal=sign^0*digit^1
patterns.integer=sign^0*digit^1
patterns.unsigned=digit^0*P('.')*digit^1
patterns.float=sign^0*patterns.unsigned
patterns.cunsigned=digit^0*P(',')*digit^1
patterns.cfloat=sign^0*patterns.cunsigned
patterns.number=patterns.float+patterns.integer
patterns.cnumber=patterns.cfloat+patterns.integer
patterns.oct=P("0")*R("07")^1
patterns.octal=patterns.oct
patterns.HEX=P("0x")*R("09","AF")^1
patterns.hex=P("0x")*R("09","af")^1
patterns.hexadecimal=P("0x")*R("09","AF","af")^1
patterns.lowercase=R("az")
patterns.uppercase=R("AZ")
patterns.letter=patterns.lowercase+patterns.uppercase
patterns.space=space
patterns.tab=P("\t")
patterns.spaceortab=patterns.space+patterns.tab
patterns.newline=newline
patterns.emptyline=newline^1
patterns.equal=P("=")
patterns.comma=P(",")
patterns.commaspacer=P(",")*spacer^0
patterns.period=P(".")
patterns.colon=P(":")
patterns.semicolon=P(";")
patterns.underscore=P("_")
patterns.escaped=escaped
patterns.squote=squote
patterns.dquote=dquote
patterns.nosquote=(escaped+(1-squote))^0
patterns.nodquote=(escaped+(1-dquote))^0
patterns.unsingle=(squote/"")*patterns.nosquote*(squote/"") 
patterns.undouble=(dquote/"")*patterns.nodquote*(dquote/"") 
patterns.unquoted=patterns.undouble+patterns.unsingle 
patterns.unspacer=((patterns.spacer^1)/"")^0
patterns.singlequoted=squote*patterns.nosquote*squote
patterns.doublequoted=dquote*patterns.nodquote*dquote
patterns.quoted=patterns.doublequoted+patterns.singlequoted
patterns.propername=R("AZ","az","__")*R("09","AZ","az","__")^0*P(-1)
patterns.somecontent=(anything-newline-space)^1 
patterns.beginline=#(1-newline)
patterns.longtostring=Cs(whitespace^0/""*nonwhitespace^0*((whitespace^0/" "*(patterns.quoted+nonwhitespace)^1)^0))
local function anywhere(pattern) 
  return P { P(pattern)+1*V(1) }
end
lpeg.anywhere=anywhere
function lpeg.instringchecker(p)
  p=anywhere(p)
  return function(str)
    return lpegmatch(p,str) and true or false
  end
end
function lpeg.splitter(pattern,action)
  return (((1-P(pattern))^1)/action+1)^0
end
function lpeg.tsplitter(pattern,action)
  return Ct((((1-P(pattern))^1)/action+1)^0)
end
local splitters_s,splitters_m,splitters_t={},{},{}
local function splitat(separator,single)
  local splitter=(single and splitters_s[separator]) or splitters_m[separator]
  if not splitter then
    separator=P(separator)
    local other=C((1-separator)^0)
    if single then
      local any=anything
      splitter=other*(separator*C(any^0)+"") 
      splitters_s[separator]=splitter
    else
      splitter=other*(separator*other)^0
      splitters_m[separator]=splitter
    end
  end
  return splitter
end
local function tsplitat(separator)
  local splitter=splitters_t[separator]
  if not splitter then
    splitter=Ct(splitat(separator))
    splitters_t[separator]=splitter
  end
  return splitter
end
lpeg.splitat=splitat
lpeg.tsplitat=tsplitat
function string.splitup(str,separator)
  if not separator then
    separator=","
  end
  return lpegmatch(splitters_m[separator] or splitat(separator),str)
end
local cache={}
function lpeg.split(separator,str)
  local c=cache[separator]
  if not c then
    c=tsplitat(separator)
    cache[separator]=c
  end
  return lpegmatch(c,str)
end
function string.split(str,separator)
  if separator then
    local c=cache[separator]
    if not c then
      c=tsplitat(separator)
      cache[separator]=c
    end
    return lpegmatch(c,str)
  else
    return { str }
  end
end
local spacing=patterns.spacer^0*newline 
local empty=spacing*Cc("")
local nonempty=Cs((1-spacing)^1)*spacing^-1
local content=(empty+nonempty)^1
patterns.textline=content
local linesplitter=tsplitat(newline)
patterns.linesplitter=linesplitter
function string.splitlines(str)
  return lpegmatch(linesplitter,str)
end
local cache={}
function lpeg.checkedsplit(separator,str)
  local c=cache[separator]
  if not c then
    separator=P(separator)
    local other=C((1-separator)^1)
    c=Ct(separator^0*other*(separator^1*other)^0)
    cache[separator]=c
  end
  return lpegmatch(c,str)
end
function string.checkedsplit(str,separator)
  local c=cache[separator]
  if not c then
    separator=P(separator)
    local other=C((1-separator)^1)
    c=Ct(separator^0*other*(separator^1*other)^0)
    cache[separator]=c
  end
  return lpegmatch(c,str)
end
local function f2(s) local c1,c2=byte(s,1,2) return  c1*64+c2-12416 end
local function f3(s) local c1,c2,c3=byte(s,1,3) return (c1*64+c2)*64+c3-925824 end
local function f4(s) local c1,c2,c3,c4=byte(s,1,4) return ((c1*64+c2)*64+c3)*64+c4-63447168 end
local utf8byte=patterns.utf8one/byte+patterns.utf8two/f2+patterns.utf8three/f3+patterns.utf8four/f4
patterns.utf8byte=utf8byte
local cache={}
function lpeg.stripper(str)
  if type(str)=="string" then
    local s=cache[str]
    if not s then
      s=Cs(((S(str)^1)/""+1)^0)
      cache[str]=s
    end
    return s
  else
    return Cs(((str^1)/""+1)^0)
  end
end
local cache={}
function lpeg.keeper(str)
  if type(str)=="string" then
    local s=cache[str]
    if not s then
      s=Cs((((1-S(str))^1)/""+1)^0)
      cache[str]=s
    end
    return s
  else
    return Cs((((1-str)^1)/""+1)^0)
  end
end
function lpeg.frontstripper(str) 
  return (P(str)+P(true))*Cs(anything^0)
end
function lpeg.endstripper(str) 
  return Cs((1-P(str)*endofstring)^0)
end
function lpeg.replacer(one,two,makefunction,isutf) 
  local pattern
  local u=isutf and utf8char or 1
  if type(one)=="table" then
    local no=#one
    local p=P(false)
    if no==0 then
      for k,v in next,one do
        p=p+P(k)/v
      end
      pattern=Cs((p+u)^0)
    elseif no==1 then
      local o=one[1]
      one,two=P(o[1]),o[2]
      pattern=Cs((one/two+u)^0)
    else
      for i=1,no do
        local o=one[i]
        p=p+P(o[1])/o[2]
      end
      pattern=Cs((p+u)^0)
    end
  else
    pattern=Cs((P(one)/(two or "")+u)^0)
  end
  if makefunction then
    return function(str)
      return lpegmatch(pattern,str)
    end
  else
    return pattern
  end
end
function lpeg.finder(lst,makefunction)
  local pattern
  if type(lst)=="table" then
    pattern=P(false)
    if #lst==0 then
      for k,v in next,lst do
        pattern=pattern+P(k) 
      end
    else
      for i=1,#lst do
        pattern=pattern+P(lst[i])
      end
    end
  else
    pattern=P(lst)
  end
  pattern=(1-pattern)^0*pattern
  if makefunction then
    return function(str)
      return lpegmatch(pattern,str)
    end
  else
    return pattern
  end
end
local splitters_f,splitters_s={},{}
function lpeg.firstofsplit(separator) 
  local splitter=splitters_f[separator]
  if not splitter then
    separator=P(separator)
    splitter=C((1-separator)^0)
    splitters_f[separator]=splitter
  end
  return splitter
end
function lpeg.secondofsplit(separator) 
  local splitter=splitters_s[separator]
  if not splitter then
    separator=P(separator)
    splitter=(1-separator)^0*separator*C(anything^0)
    splitters_s[separator]=splitter
  end
  return splitter
end
function lpeg.balancer(left,right)
  left,right=P(left),P(right)
  return P { left*((1-left-right)+V(1))^0*right }
end
local nany=utf8char/""
function lpeg.counter(pattern)
  pattern=Cs((P(pattern)/" "+nany)^0)
  return function(str)
    return #lpegmatch(pattern,str)
  end
end
utf=utf or (unicode and unicode.utf8) or {}
local utfcharacters=utf and utf.characters or string.utfcharacters
local utfgmatch=utf and utf.gmatch
local utfchar=utf and utf.char
lpeg.UP=lpeg.P
if utfcharacters then
  function lpeg.US(str)
    local p=P(false)
    for uc in utfcharacters(str) do
      p=p+P(uc)
    end
    return p
  end
elseif utfgmatch then
  function lpeg.US(str)
    local p=P(false)
    for uc in utfgmatch(str,".") do
      p=p+P(uc)
    end
    return p
  end
else
  function lpeg.US(str)
    local p=P(false)
    local f=function(uc)
      p=p+P(uc)
    end
    lpegmatch((utf8char/f)^0,str)
    return p
  end
end
local range=utf8byte*utf8byte+Cc(false) 
function lpeg.UR(str,more)
  local first,last
  if type(str)=="number" then
    first=str
    last=more or first
  else
    first,last=lpegmatch(range,str)
    if not last then
      return P(str)
    end
  end
  if first==last then
    return P(str)
  elseif utfchar and (last-first<8) then 
    local p=P(false)
    for i=first,last do
      p=p+P(utfchar(i))
    end
    return p 
  else
    local f=function(b)
      return b>=first and b<=last
    end
    return utf8byte/f 
  end
end
function lpeg.is_lpeg(p)
  return p and lpegtype(p)=="pattern"
end
function lpeg.oneof(list,...) 
  if type(list)~="table" then
    list={ list,... }
  end
  local p=P(list[1])
  for l=2,#list do
    p=p+P(list[l])
  end
  return p
end
local sort=table.sort
local function copyindexed(old)
  local new={}
  for i=1,#old do
    new[i]=old
  end
  return new
end
local function sortedkeys(tab)
  local keys,s={},0
  for key,_ in next,tab do
    s=s+1
    keys[s]=key
  end
  sort(keys)
  return keys
end
function lpeg.append(list,pp,delayed,checked)
  local p=pp
  if #list>0 then
    local keys=copyindexed(list)
    sort(keys)
    for i=#keys,1,-1 do
      local k=keys[i]
      if p then
        p=P(k)+p
      else
        p=P(k)
      end
    end
  elseif delayed then 
    local keys=sortedkeys(list)
    if p then
      for i=1,#keys,1 do
        local k=keys[i]
        local v=list[k]
        p=P(k)/list+p
      end
    else
      for i=1,#keys do
        local k=keys[i]
        local v=list[k]
        if p then
          p=P(k)+p
        else
          p=P(k)
        end
      end
      if p then
        p=p/list
      end
    end
  elseif checked then
    local keys=sortedkeys(list)
    for i=1,#keys do
      local k=keys[i]
      local v=list[k]
      if p then
        if k==v then
          p=P(k)+p
        else
          p=P(k)/v+p
        end
      else
        if k==v then
          p=P(k)
        else
          p=P(k)/v
        end
      end
    end
  else
    local keys=sortedkeys(list)
    for i=1,#keys do
      local k=keys[i]
      local v=list[k]
      if p then
        p=P(k)/v+p
      else
        p=P(k)/v
      end
    end
  end
  return p
end
local function make(t)
  local p
  local keys=sortedkeys(t)
  for i=1,#keys do
    local k=keys[i]
    local v=t[k]
    if not p then
      if next(v) then
        p=P(k)*make(v)
      else
        p=P(k)
      end
    else
      if next(v) then
        p=p+P(k)*make(v)
      else
        p=p+P(k)
      end
    end
  end
  return p
end
function lpeg.utfchartabletopattern(list) 
  local tree={}
  for i=1,#list do
    local t=tree
    for c in gmatch(list[i],".") do
      if not t[c] then
        t[c]={}
      end
      t=t[c]
    end
  end
  return make(tree)
end
patterns.containseol=lpeg.finder(eol)
local function nextstep(n,step,result)
  local m=n%step   
  local d=floor(n/step) 
  if d>0 then
    local v=V(tostring(step))
    local s=result.start
    for i=1,d do
      if s then
        s=v*s
      else
        s=v
      end
    end
    result.start=s
  end
  if step>1 and result.start then
    local v=V(tostring(step/2))
    result[tostring(step)]=v*v
  end
  if step>0 then
    return nextstep(m,step/2,result)
  else
    return result
  end
end
function lpeg.times(pattern,n)
  return P(nextstep(n,2^16,{ "start",["1"]=pattern }))
end
local digit=R("09")
local period=P(".")
local zero=P("0")
local trailingzeros=zero^0*-digit 
local case_1=period*trailingzeros/""
local case_2=period*(digit-trailingzeros)^1*(trailingzeros/"")
local number=digit^1*(case_1+case_2)
local stripper=Cs((number+1)^0)
lpeg.patterns.stripzeros=stripper


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-function"] = package.loaded["l-function"] or true

-- original size: 361, stripped down to: 322

if not modules then modules={} end modules ['l-functions']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
functions=functions or {}
function functions.dummy() end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-string"] = package.loaded["l-string"] or true

-- original size: 5513, stripped down to: 2708

if not modules then modules={} end modules ['l-string']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local string=string
local sub,gmatch,format,char,byte,rep,lower=string.sub,string.gmatch,string.format,string.char,string.byte,string.rep,string.lower
local lpegmatch,patterns=lpeg.match,lpeg.patterns
local P,S,C,Ct,Cc,Cs=lpeg.P,lpeg.S,lpeg.C,lpeg.Ct,lpeg.Cc,lpeg.Cs
local unquoted=patterns.squote*C(patterns.nosquote)*patterns.squote+patterns.dquote*C(patterns.nodquote)*patterns.dquote
function string.unquoted(str)
  return lpegmatch(unquoted,str) or str
end
function string.quoted(str)
  return format("%q",str) 
end
function string.count(str,pattern) 
  local n=0
  for _ in gmatch(str,pattern) do 
    n=n+1
  end
  return n
end
function string.limit(str,n,sentinel) 
  if #str>n then
    sentinel=sentinel or "..."
    return sub(str,1,(n-#sentinel))..sentinel
  else
    return str
  end
end
local stripper=patterns.stripper
local collapser=patterns.collapser
local longtostring=patterns.longtostring
function string.strip(str)
  return lpegmatch(stripper,str) or ""
end
function string.collapsespaces(str)
  return lpegmatch(collapser,str) or ""
end
function string.longtostring(str)
  return lpegmatch(longtostring,str) or ""
end
local pattern=P(" ")^0*P(-1)
function string.is_empty(str)
  if str=="" then
    return true
  else
    return lpegmatch(pattern,str) and true or false
  end
end
local anything=patterns.anything
local allescapes=Cc("%")*S(".-+%?()[]*") 
local someescapes=Cc("%")*S(".-+%()[]")  
local matchescapes=Cc(".")*S("*?")     
local pattern_a=Cs ((allescapes+anything )^0 )
local pattern_b=Cs ((someescapes+matchescapes+anything )^0 )
local pattern_c=Cs (Cc("^")*(someescapes+matchescapes+anything )^0*Cc("$") )
function string.escapedpattern(str,simple)
  return lpegmatch(simple and pattern_b or pattern_a,str)
end
function string.topattern(str,lowercase,strict)
  if str=="" or type(str)~="string" then
    return ".*"
  elseif strict then
    str=lpegmatch(pattern_c,str)
  else
    str=lpegmatch(pattern_b,str)
  end
  if lowercase then
    return lower(str)
  else
    return str
  end
end
function string.valid(str,default)
  return (type(str)=="string" and str~="" and str) or default or nil
end
string.itself=function(s) return s end
local pattern=Ct(C(1)^0) 
function string.totable(str)
  return lpegmatch(pattern,str)
end
local replacer=lpeg.replacer("@","%%") 
function string.tformat(fmt,...)
  return format(lpegmatch(replacer,fmt),...)
end
string.quote=string.quoted
string.unquote=string.unquoted


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-table"] = package.loaded["l-table"] or true

-- original size: 44643, stripped down to: 19717

if not modules then modules={} end modules ['l-table']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local type,next,tostring,tonumber,ipairs,select=type,next,tostring,tonumber,ipairs,select
local table,string=table,string
local concat,sort,insert,remove=table.concat,table.sort,table.insert,table.remove
local format,lower,dump=string.format,string.lower,string.dump
local getmetatable,setmetatable=getmetatable,setmetatable
local getinfo=debug.getinfo
local lpegmatch,patterns=lpeg.match,lpeg.patterns
local floor=math.floor
local stripper=patterns.stripper
function table.strip(tab)
  local lst,l={},0
  for i=1,#tab do
    local s=lpegmatch(stripper,tab[i]) or ""
    if s=="" then
    else
      l=l+1
      lst[l]=s
    end
  end
  return lst
end
function table.keys(t)
  if t then
    local keys,k={},0
    for key,_ in next,t do
      k=k+1
      keys[k]=key
    end
    return keys
  else
    return {}
  end
end
local function compare(a,b)
  local ta,tb=type(a),type(b) 
  if ta==tb then
    return a<b
  else
    return tostring(a)<tostring(b)
  end
end
local function sortedkeys(tab)
  if tab then
    local srt,category,s={},0,0 
    for key,_ in next,tab do
      s=s+1
      srt[s]=key
      if category==3 then
      else
        local tkey=type(key)
        if tkey=="string" then
          category=(category==2 and 3) or 1
        elseif tkey=="number" then
          category=(category==1 and 3) or 2
        else
          category=3
        end
      end
    end
    if category==0 or category==3 then
      sort(srt,compare)
    else
      sort(srt)
    end
    return srt
  else
    return {}
  end
end
local function sortedhashkeys(tab,cmp) 
  if tab then
    local srt,s={},0
    for key,_ in next,tab do
      if key then
        s=s+1
        srt[s]=key
      end
    end
    sort(srt,cmp)
    return srt
  else
    return {}
  end
end
function table.allkeys(t)
  local keys={}
  for k,v in next,t do
    for k,v in next,v do
      keys[k]=true
    end
  end
  return sortedkeys(keys)
end
table.sortedkeys=sortedkeys
table.sortedhashkeys=sortedhashkeys
local function nothing() end
local function sortedhash(t,cmp)
  if t then
    local s
    if cmp then
      s=sortedhashkeys(t,function(a,b) return cmp(t,a,b) end)
    else
      s=sortedkeys(t) 
    end
    local n=0
    local function kv(s)
      n=n+1
      local k=s[n]
      return k,t[k]
    end
    return kv,s
  else
    return nothing
  end
end
table.sortedhash=sortedhash
table.sortedpairs=sortedhash 
function table.append(t,list)
  local n=#t
  for i=1,#list do
    n=n+1
    t[n]=list[i]
  end
  return t
end
function table.prepend(t,list)
  local nl=#list
  local nt=nl+#t
  for i=#t,1,-1 do
    t[nt]=t[i]
    nt=nt-1
  end
  for i=1,#list do
    t[i]=list[i]
  end
  return t
end
function table.merge(t,...) 
  t=t or {}
  for i=1,select("#",...) do
    for k,v in next,(select(i,...)) do
      t[k]=v
    end
  end
  return t
end
function table.merged(...)
  local t={}
  for i=1,select("#",...) do
    for k,v in next,(select(i,...)) do
      t[k]=v
    end
  end
  return t
end
function table.imerge(t,...)
  local nt=#t
  for i=1,select("#",...) do
    local nst=select(i,...)
    for j=1,#nst do
      nt=nt+1
      t[nt]=nst[j]
    end
  end
  return t
end
function table.imerged(...)
  local tmp,ntmp={},0
  for i=1,select("#",...) do
    local nst=select(i,...)
    for j=1,#nst do
      ntmp=ntmp+1
      tmp[ntmp]=nst[j]
    end
  end
  return tmp
end
local function fastcopy(old,metatabletoo) 
  if old then
    local new={}
    for k,v in next,old do
      if type(v)=="table" then
        new[k]=fastcopy(v,metatabletoo) 
      else
        new[k]=v
      end
    end
    if metatabletoo then
      local mt=getmetatable(old)
      if mt then
        setmetatable(new,mt)
      end
    end
    return new
  else
    return {}
  end
end
local function copy(t,tables) 
  tables=tables or {}
  local tcopy={}
  if not tables[t] then
    tables[t]=tcopy
  end
  for i,v in next,t do 
    if type(i)=="table" then
      if tables[i] then
        i=tables[i]
      else
        i=copy(i,tables)
      end
    end
    if type(v)~="table" then
      tcopy[i]=v
    elseif tables[v] then
      tcopy[i]=tables[v]
    else
      tcopy[i]=copy(v,tables)
    end
  end
  local mt=getmetatable(t)
  if mt then
    setmetatable(tcopy,mt)
  end
  return tcopy
end
table.fastcopy=fastcopy
table.copy=copy
function table.derive(parent) 
  local child={}
  if parent then
    setmetatable(child,{ __index=parent })
  end
  return child
end
function table.tohash(t,value)
  local h={}
  if t then
    if value==nil then value=true end
    for _,v in next,t do 
      h[v]=value
    end
  end
  return h
end
function table.fromhash(t)
  local hsh,h={},0
  for k,v in next,t do 
    if v then
      h=h+1
      hsh[h]=k
    end
  end
  return hsh
end
local noquotes,hexify,handle,reduce,compact,inline,functions
local reserved=table.tohash { 
  'and','break','do','else','elseif','end','false','for','function','if',
  'in','local','nil','not','or','repeat','return','then','true','until','while',
}
local function simple_table(t)
  if #t>0 then
    local n=0
    for _,v in next,t do
      n=n+1
    end
    if n==#t then
      local tt,nt={},0
      for i=1,#t do
        local v=t[i]
        local tv=type(v)
        if tv=="number" then
          nt=nt+1
          if hexify then
            tt[nt]=format("0x%04X",v)
          else
            tt[nt]=tostring(v) 
          end
        elseif tv=="boolean" then
          nt=nt+1
          tt[nt]=tostring(v)
        elseif tv=="string" then
          nt=nt+1
          tt[nt]=format("%q",v)
        else
          tt=nil
          break
        end
      end
      return tt
    end
  end
  return nil
end
local propername=patterns.propername 
local function dummy() end
local function do_serialize(root,name,depth,level,indexed)
  if level>0 then
    depth=depth.." "
    if indexed then
      handle(format("%s{",depth))
    else
      local tn=type(name)
      if tn=="number" then
        if hexify then
          handle(format("%s[0x%04X]={",depth,name))
        else
          handle(format("%s[%s]={",depth,name))
        end
      elseif tn=="string" then
        if noquotes and not reserved[name] and lpegmatch(propername,name) then
          handle(format("%s%s={",depth,name))
        else
          handle(format("%s[%q]={",depth,name))
        end
      elseif tn=="boolean" then
        handle(format("%s[%s]={",depth,tostring(name)))
      else
        handle(format("%s{",depth))
      end
    end
  end
  if root and next(root) then
    local first,last=nil,0
    if compact then
      last=#root
      for k=1,last do
        if root[k]==nil then
          last=k-1
          break
        end
      end
      if last>0 then
        first=1
      end
    end
    local sk=sortedkeys(root)
    for i=1,#sk do
      local k=sk[i]
      local v=root[k]
      local t,tk=type(v),type(k)
      if compact and first and tk=="number" and k>=first and k<=last then
        if t=="number" then
          if hexify then
            handle(format("%s 0x%04X,",depth,v))
          else
            handle(format("%s %s,",depth,v)) 
          end
        elseif t=="string" then
          if reduce and tonumber(v) then
            handle(format("%s %s,",depth,v))
          else
            handle(format("%s %q,",depth,v))
          end
        elseif t=="table" then
          if not next(v) then
            handle(format("%s {},",depth))
          elseif inline then 
            local st=simple_table(v)
            if st then
              handle(format("%s { %s },",depth,concat(st,", ")))
            else
              do_serialize(v,k,depth,level+1,true)
            end
          else
            do_serialize(v,k,depth,level+1,true)
          end
        elseif t=="boolean" then
          handle(format("%s %s,",depth,tostring(v)))
        elseif t=="function" then
          if functions then
            handle(format('%s load(%q),',depth,dump(v)))
          else
            handle(format('%s "function",',depth))
          end
        else
          handle(format("%s %q,",depth,tostring(v)))
        end
      elseif k=="__p__" then 
        if false then
          handle(format("%s __p__=nil,",depth))
        end
      elseif t=="number" then
        if tk=="number" then
          if hexify then
            handle(format("%s [0x%04X]=0x%04X,",depth,k,v))
          else
            handle(format("%s [%s]=%s,",depth,k,v)) 
          end
        elseif tk=="boolean" then
          if hexify then
            handle(format("%s [%s]=0x%04X,",depth,tostring(k),v))
          else
            handle(format("%s [%s]=%s,",depth,tostring(k),v)) 
          end
        elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
          if hexify then
            handle(format("%s %s=0x%04X,",depth,k,v))
          else
            handle(format("%s %s=%s,",depth,k,v)) 
          end
        else
          if hexify then
            handle(format("%s [%q]=0x%04X,",depth,k,v))
          else
            handle(format("%s [%q]=%s,",depth,k,v)) 
          end
        end
      elseif t=="string" then
        if reduce and tonumber(v) then
          if tk=="number" then
            if hexify then
              handle(format("%s [0x%04X]=%s,",depth,k,v))
            else
              handle(format("%s [%s]=%s,",depth,k,v))
            end
          elseif tk=="boolean" then
            handle(format("%s [%s]=%s,",depth,tostring(k),v))
          elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
            handle(format("%s %s=%s,",depth,k,v))
          else
            handle(format("%s [%q]=%s,",depth,k,v))
          end
        else
          if tk=="number" then
            if hexify then
              handle(format("%s [0x%04X]=%q,",depth,k,v))
            else
              handle(format("%s [%s]=%q,",depth,k,v))
            end
          elseif tk=="boolean" then
            handle(format("%s [%s]=%q,",depth,tostring(k),v))
          elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
            handle(format("%s %s=%q,",depth,k,v))
          else
            handle(format("%s [%q]=%q,",depth,k,v))
          end
        end
      elseif t=="table" then
        if not next(v) then
          if tk=="number" then
            if hexify then
              handle(format("%s [0x%04X]={},",depth,k))
            else
              handle(format("%s [%s]={},",depth,k))
            end
          elseif tk=="boolean" then
            handle(format("%s [%s]={},",depth,tostring(k)))
          elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
            handle(format("%s %s={},",depth,k))
          else
            handle(format("%s [%q]={},",depth,k))
          end
        elseif inline then
          local st=simple_table(v)
          if st then
            if tk=="number" then
              if hexify then
                handle(format("%s [0x%04X]={ %s },",depth,k,concat(st,", ")))
              else
                handle(format("%s [%s]={ %s },",depth,k,concat(st,", ")))
              end
            elseif tk=="boolean" then
              handle(format("%s [%s]={ %s },",depth,tostring(k),concat(st,", ")))
            elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
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
      elseif t=="boolean" then
        if tk=="number" then
          if hexify then
            handle(format("%s [0x%04X]=%s,",depth,k,tostring(v)))
          else
            handle(format("%s [%s]=%s,",depth,k,tostring(v)))
          end
        elseif tk=="boolean" then
          handle(format("%s [%s]=%s,",depth,tostring(k),tostring(v)))
        elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
          handle(format("%s %s=%s,",depth,k,tostring(v)))
        else
          handle(format("%s [%q]=%s,",depth,k,tostring(v)))
        end
      elseif t=="function" then
        if functions then
          local f=getinfo(v).what=="C" and dump(dummy) or dump(v)
          if tk=="number" then
            if hexify then
              handle(format("%s [0x%04X]=load(%q),",depth,k,f))
            else
              handle(format("%s [%s]=load(%q),",depth,k,f))
            end
          elseif tk=="boolean" then
            handle(format("%s [%s]=load(%q),",depth,tostring(k),f))
          elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
            handle(format("%s %s=load(%q),",depth,k,f))
          else
            handle(format("%s [%q]=load(%q),",depth,k,f))
          end
        end
      else
        if tk=="number" then
          if hexify then
            handle(format("%s [0x%04X]=%q,",depth,k,tostring(v)))
          else
            handle(format("%s [%s]=%q,",depth,k,tostring(v)))
          end
        elseif tk=="boolean" then
          handle(format("%s [%s]=%q,",depth,tostring(k),tostring(v)))
        elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
          handle(format("%s %s=%q,",depth,k,tostring(v)))
        else
          handle(format("%s [%q]=%q,",depth,k,tostring(v)))
        end
      end
    end
  end
  if level>0 then
    handle(format("%s},",depth))
  end
end
local function serialize(_handle,root,name,specification) 
  local tname=type(name)
  if type(specification)=="table" then
    noquotes=specification.noquotes
    hexify=specification.hexify
    handle=_handle or specification.handle or print
    reduce=specification.reduce or false
    functions=specification.functions
    compact=specification.compact
    inline=specification.inline and compact
    if functions==nil then
      functions=true
    end
    if compact==nil then
      compact=true
    end
    if inline==nil then
      inline=compact
    end
  else
    noquotes=false
    hexify=false
    handle=_handle or print
    reduce=false
    compact=true
    inline=true
    functions=true
  end
  if tname=="string" then
    if name=="return" then
      handle("return {")
    else
      handle(name.."={")
    end
  elseif tname=="number" then
    if hexify then
      handle(format("[0x%04X]={",name))
    else
      handle("["..name.."]={")
    end
  elseif tname=="boolean" then
    if name then
      handle("return {")
    else
      handle("{")
    end
  else
    handle("t={")
  end
  if root then
    if getmetatable(root) then 
      local dummy=root._w_h_a_t_e_v_e_r_
      root._w_h_a_t_e_v_e_r_=nil
    end
    if next(root) then
      do_serialize(root,name,"",0)
    end
  end
  handle("}")
end
function table.serialize(root,name,specification)
  local t,n={},0
  local function flush(s)
    n=n+1
    t[n]=s
  end
  serialize(flush,root,name,specification)
  return concat(t,"\n")
end
table.tohandle=serialize
local maxtab=2*1024
function table.tofile(filename,root,name,specification)
  local f=io.open(filename,'w')
  if f then
    if maxtab>1 then
      local t,n={},0
      local function flush(s)
        n=n+1
        t[n]=s
        if n>maxtab then
          f:write(concat(t,"\n"),"\n") 
          t,n={},0 
        end
      end
      serialize(flush,root,name,specification)
      f:write(concat(t,"\n"),"\n")
    else
      local function flush(s)
        f:write(s,"\n")
      end
      serialize(flush,root,name,specification)
    end
    f:close()
    io.flush()
  end
end
local function flattened(t,f,depth)
  if f==nil then
    f={}
    depth=0xFFFF
  elseif tonumber(f) then
    depth=f
    f={}
  elseif not depth then
    depth=0xFFFF
  end
  for k,v in next,t do
    if type(k)~="number" then
      if depth>0 and type(v)=="table" then
        flattened(v,f,depth-1)
      else
        f[k]=v
      end
    end
  end
  local n=#f
  for k=1,#t do
    local v=t[k]
    if depth>0 and type(v)=="table" then
      flattened(v,f,depth-1)
      n=#f
    else
      n=n+1
      f[n]=v
    end
  end
  return f
end
table.flattened=flattened
local function unnest(t,f) 
  if not f then     
    f={}      
  end
  for i=1,#t do
    local v=t[i]
    if type(v)=="table" then
      if type(v[1])=="table" then
        unnest(v,f)
      else
        f[#f+1]=v
      end
    else
      f[#f+1]=v
    end
  end
  return f
end
function table.unnest(t) 
  return unnest(t)
end
local function are_equal(a,b,n,m) 
  if a and b and #a==#b then
    n=n or 1
    m=m or #a
    for i=n,m do
      local ai,bi=a[i],b[i]
      if ai==bi then
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
local function identical(a,b) 
  for ka,va in next,a do
    local vb=b[ka]
    if va==vb then
    elseif type(va)=="table" and type(vb)=="table" then
      if not identical(va,vb) then
        return false
      end
    else
      return false
    end
  end
  return true
end
table.identical=identical
table.are_equal=are_equal
function table.compact(t) 
  if t then
    for k,v in next,t do
      if not next(v) then 
        t[k]=nil
      end
    end
  end
end
function table.contains(t,v)
  if t then
    for i=1,#t do
      if t[i]==v then
        return i
      end
    end
  end
  return false
end
function table.count(t)
  local n=0
  for k,v in next,t do
    n=n+1
  end
  return n
end
function table.swapped(t,s) 
  local n={}
  if s then
    for k,v in next,s do
      n[k]=v
    end
  end
  for k,v in next,t do
    n[v]=k
  end
  return n
end
function table.mirrored(t) 
  local n={}
  for k,v in next,t do
    n[v]=k
    n[k]=v
  end
  return n
end
function table.reversed(t)
  if t then
    local tt,tn={},#t
    if tn>0 then
      local ttn=0
      for i=tn,1,-1 do
        ttn=ttn+1
        tt[ttn]=t[i]
      end
    end
    return tt
  end
end
function table.reverse(t)
  if t then
    local n=#t
    for i=1,floor(n/2) do
      local j=n-i+1
      t[i],t[j]=t[j],t[i]
    end
    return t
  end
end
function table.sequenced(t,sep,simple) 
  if not t then
    return ""
  end
  local n=#t
  local s={}
  if n>0 then
    for i=1,n do
      s[i]=tostring(t[i])
    end
  else
    n=0
    for k,v in sortedhash(t) do
      if simple then
        if v==true then
          n=n+1
          s[n]=k
        elseif v and v~="" then
          n=n+1
          s[n]=k.."="..tostring(v)
        end
      else
        n=n+1
        s[n]=k.."="..tostring(v)
      end
    end
  end
  return concat(s,sep or " | ")
end
function table.print(t,...)
  if type(t)~="table" then
    print(tostring(t))
  else
    serialize(print,t,...)
  end
end
setinspector(function(v) if type(v)=="table" then serialize(print,v,"table") return true end end)
function table.sub(t,i,j)
  return { unpack(t,i,j) }
end
function table.is_empty(t)
  return not t or not next(t)
end
function table.has_one_entry(t)
  return t and not next(t,next(t))
end
function table.loweredkeys(t) 
  local l={}
  for k,v in next,t do
    l[lower(k)]=v
  end
  return l
end
function table.unique(old)
  local hash={}
  local new={}
  local n=0
  for i=1,#old do
    local oi=old[i]
    if not hash[oi] then
      n=n+1
      new[n]=oi
      hash[oi]=true
    end
  end
  return new
end
function table.sorted(t,...)
  sort(t,...)
  return t 
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-io"] = package.loaded["l-io"] or true

-- original size: 8799, stripped down to: 6325

if not modules then modules={} end modules ['l-io']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local io=io
local byte,find,gsub,format=string.byte,string.find,string.gsub,string.format
local concat=table.concat
local floor=math.floor
local type=type
if string.find(os.getenv("PATH"),";") then
  io.fileseparator,io.pathseparator="\\",";"
else
  io.fileseparator,io.pathseparator="/",":"
end
local function readall(f)
  return f:read("*all")
end
local function readall(f)
  local size=f:seek("end")
  if size==0 then
    return ""
  elseif size<1024*1024 then
    f:seek("set",0)
    return f:read('*all')
  else
    local done=f:seek("set",0)
    if size<1024*1024 then
      step=1024*1024
    elseif size>16*1024*1024 then
      step=16*1024*1024
    else
      step=floor(size/(1024*1024))*1024*1024/8
    end
    local data={}
    while true do
      local r=f:read(step)
      if not r then
        return concat(data)
      else
        data[#data+1]=r
      end
    end
  end
end
io.readall=readall
function io.loaddata(filename,textmode) 
  local f=io.open(filename,(textmode and 'r') or 'rb')
  if f then
    local data=readall(f)
    f:close()
    if #data>0 then
      return data
    end
  end
end
function io.savedata(filename,data,joiner)
  local f=io.open(filename,"wb")
  if f then
    if type(data)=="table" then
      f:write(concat(data,joiner or ""))
    elseif type(data)=="function" then
      data(f)
    else
      f:write(data or "")
    end
    f:close()
    io.flush()
    return true
  else
    return false
  end
end
function io.loadlines(filename,n) 
  local f=io.open(filename,'r')
  if not f then
  elseif n then
    local lines={}
    for i=1,n do
      local line=f:read("*lines")
      if line then
        lines[#lines+1]=line
      else
        break
      end
    end
    f:close()
    lines=concat(lines,"\n")
    if #lines>0 then
      return lines
    end
  else
    local line=f:read("*line") or ""
    f:close()
    if #line>0 then
      return line
    end
  end
end
function io.loadchunk(filename,n)
  local f=io.open(filename,'rb')
  if f then
    local data=f:read(n or 1024)
    f:close()
    if #data>0 then
      return data
    end
  end
end
function io.exists(filename)
  local f=io.open(filename)
  if f==nil then
    return false
  else
    f:close()
    return true
  end
end
function io.size(filename)
  local f=io.open(filename)
  if f==nil then
    return 0
  else
    local s=f:seek("end")
    f:close()
    return s
  end
end
function io.noflines(f)
  if type(f)=="string" then
    local f=io.open(filename)
    if f then
      local n=f and io.noflines(f) or 0
      f:close()
      return n
    else
      return 0
    end
  else
    local n=0
    for _ in f:lines() do
      n=n+1
    end
    f:seek('set',0)
    return n
  end
end
local nextchar={
  [ 4]=function(f)
    return f:read(1,1,1,1)
  end,
  [ 2]=function(f)
    return f:read(1,1)
  end,
  [ 1]=function(f)
    return f:read(1)
  end,
  [-2]=function(f)
    local a,b=f:read(1,1)
    return b,a
  end,
  [-4]=function(f)
    local a,b,c,d=f:read(1,1,1,1)
    return d,c,b,a
  end
}
function io.characters(f,n)
  if f then
    return nextchar[n or 1],f
  end
end
local nextbyte={
  [4]=function(f)
    local a,b,c,d=f:read(1,1,1,1)
    if d then
      return byte(a),byte(b),byte(c),byte(d)
    end
  end,
  [3]=function(f)
    local a,b,c=f:read(1,1,1)
    if b then
      return byte(a),byte(b),byte(c)
    end
  end,
  [2]=function(f)
    local a,b=f:read(1,1)
    if b then
      return byte(a),byte(b)
    end
  end,
  [1]=function (f)
    local a=f:read(1)
    if a then
      return byte(a)
    end
  end,
  [-2]=function (f)
    local a,b=f:read(1,1)
    if b then
      return byte(b),byte(a)
    end
  end,
  [-3]=function(f)
    local a,b,c=f:read(1,1,1)
    if b then
      return byte(c),byte(b),byte(a)
    end
  end,
  [-4]=function(f)
    local a,b,c,d=f:read(1,1,1,1)
    if d then
      return byte(d),byte(c),byte(b),byte(a)
    end
  end
}
function io.bytes(f,n)
  if f then
    return nextbyte[n or 1],f
  else
    return nil,nil
  end
end
function io.ask(question,default,options)
  while true do
    io.write(question)
    if options then
      io.write(format(" [%s]",concat(options,"|")))
    end
    if default then
      io.write(format(" [%s]",default))
    end
    io.write(format(" "))
    io.flush()
    local answer=io.read()
    answer=gsub(answer,"^%s*(.*)%s*$","%1")
    if answer=="" and default then
      return default
    elseif not options then
      return answer
    else
      for k=1,#options do
        if options[k]==answer then
          return answer
        end
      end
      local pattern="^"..answer
      for k=1,#options do
        local v=options[k]
        if find(v,pattern) then
          return v
        end
      end
    end
  end
end
local function readnumber(f,n,m)
  if m then
    f:seek("set",n)
    n=m
  end
  if n==1 then
    return byte(f:read(1))
  elseif n==2 then
    local a,b=byte(f:read(2),1,2)
    return 256*a+b
  elseif n==3 then
    local a,b,c=byte(f:read(3),1,3)
    return 256*256*a+256*b+c
  elseif n==4 then
    local a,b,c,d=byte(f:read(4),1,4)
    return 256*256*256*a+256*256*b+256*c+d
  elseif n==8 then
    local a,b=readnumber(f,4),readnumber(f,4)
    return 256*a+b
  elseif n==12 then
    local a,b,c=readnumber(f,4),readnumber(f,4),readnumber(f,4)
    return 256*256*a+256*b+c
  elseif n==-2 then
    local b,a=byte(f:read(2),1,2)
    return 256*a+b
  elseif n==-3 then
    local c,b,a=byte(f:read(3),1,3)
    return 256*256*a+256*b+c
  elseif n==-4 then
    local d,c,b,a=byte(f:read(4),1,4)
    return 256*256*256*a+256*256*b+256*c+d
  elseif n==-8 then
    local h,g,f,e,d,c,b,a=byte(f:read(8),1,8)
    return 256*256*256*256*256*256*256*a+256*256*256*256*256*256*b+256*256*256*256*256*c+256*256*256*256*d+256*256*256*e+256*256*f+256*g+h
  else
    return 0
  end
end
io.readnumber=readnumber
function io.readstring(f,n,m)
  if m then
    f:seek("set",n)
    n=m
  end
  local str=gsub(f:read(n),"\000","")
  return str
end
if not io.i_limiter then function io.i_limiter() end end 
if not io.o_limiter then function io.o_limiter() end end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-number"] = package.loaded["l-number"] or true

-- original size: 4939, stripped down to: 2830

if not modules then modules={} end modules ['l-number']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local tostring,tonumber=tostring,tonumber
local format,floor,match,rep=string.format,math.floor,string.match,string.rep
local concat,insert=table.concat,table.insert
local lpegmatch=lpeg.match
number=number or {}
local number=number
if bit32 then 
  local btest,bor=bit32.btest,bit32.bor
  function number.bit(p)
    return 2^(p-1) 
  end
  number.hasbit=btest
  number.setbit=bor
  function number.setbit(x,p) 
    return btest(x,p) and x or x+p
  end
  function number.clearbit(x,p)
    return btest(x,p) and x-p or x
  end
else
  function number.bit(p)
    return 2^(p-1) 
  end
  function number.hasbit(x,p) 
    return x%(p+p)>=p
  end
  function number.setbit(x,p)
    return (x%(p+p)>=p) and x or x+p
  end
  function number.clearbit(x,p)
    return (x%(p+p)>=p) and x-p or x
  end
end
if bit32 then
  local bextract=bit32.extract
  local t={
    "0","0","0","0","0","0","0","0",
    "0","0","0","0","0","0","0","0",
    "0","0","0","0","0","0","0","0",
    "0","0","0","0","0","0","0","0",
  }
  function number.tobitstring(b,m)
    local n=32
    for i=0,31 do
      local v=bextract(b,i)
      local k=32-i
      if v==1 then
        n=k
        t[k]="1"
      else
        t[k]="0"
      end
    end
    if m then
      m=33-m*8
      if m<1 then
        m=1
      end
      return concat(t,"",m)
    elseif n<8 then
      return concat(t)
    elseif n<16 then
      return concat(t,"",9)
    elseif n<24 then
      return concat(t,"",17)
    else
      return concat(t,"",25)
    end
  end
else
  function number.tobitstring(n,m)
    if n>0 then
      local t={}
      while n>0 do
        insert(t,1,n%2>0 and 1 or 0)
        n=floor(n/2)
      end
      local nn=8-#t%8
      if nn>0 and nn<8 then
        for i=1,nn do
          insert(t,1,0)
        end
      end
      if m then
        m=m*8-#t
        if m>0 then
          insert(t,1,rep("0",m))
        end
      end
      return concat(t)
    elseif m then
      rep("00000000",m)
    else
      return "00000000"
    end
  end
end
function number.valid(str,default)
  return tonumber(str) or default or nil
end
function number.toevenhex(n)
  local s=format("%X",n)
  if #s%2==0 then
    return s
  else
    return "0"..s
  end
end
local one=lpeg.C(1-lpeg.S('')/tonumber)^1
function number.toset(n)
  return lpegmatch(one,tostring(n))
end
local function bits(n,i,...)
  if n>0 then
    local m=n%2
    local n=floor(n/2)
    if m>0 then
      return bits(n,i+1,i,...)
    else
      return bits(n,i+1,...)
    end
  else
    return...
  end
end
function number.bits(n)
  return { bits(n,1) }
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-set"] = package.loaded["l-set"] or true

-- original size: 1923, stripped down to: 1133

if not modules then modules={} end modules ['l-set']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
set=set or {}
local nums={}
local tabs={}
local concat=table.concat
local next,type=next,type
set.create=table.tohash
function set.tonumber(t)
  if next(t) then
    local s=""
    for k,v in next,t do
      if v then
        s=s.." "..k
      end
    end
    local n=nums[s]
    if not n then
      n=#tabs+1
      tabs[n]=t
      nums[s]=n
    end
    return n
  else
    return 0
  end
end
function set.totable(n)
  if n==0 then
    return {}
  else
    return tabs[n] or {}
  end
end
function set.tolist(n)
  if n==0 or not tabs[n] then
    return ""
  else
    local t,n={},0
    for k,v in next,tabs[n] do
      if v then
        n=n+1
        t[n]=k
      end
    end
    return concat(t," ")
  end
end
function set.contains(n,s)
  if type(n)=="table" then
    return n[s]
  elseif n==0 then
    return false
  else
    local t=tabs[n]
    return t and t[s]
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-os"] = package.loaded["l-os"] or true

-- original size: 13692, stripped down to: 8406

if not modules then modules={} end modules ['l-os']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local os=os
local date,time=os.date,os.time
local find,format,gsub,upper,gmatch=string.find,string.format,string.gsub,string.upper,string.gmatch
local concat=table.concat
local random,ceil,randomseed=math.random,math.ceil,math.randomseed
local rawget,rawset,type,getmetatable,setmetatable,tonumber,tostring=rawget,rawset,type,getmetatable,setmetatable,tonumber,tostring
math.initialseed=tonumber(string.sub(string.reverse(tostring(ceil(socket and socket.gettime()*10000 or time()))),1,6))
randomseed(math.initialseed)
if not os.__getenv__ then
  os.__getenv__=os.getenv
  os.__setenv__=os.setenv
  if os.env then
    local osgetenv=os.getenv
    local ossetenv=os.setenv
    local osenv=os.env   local _=osenv.PATH 
    function os.setenv(k,v)
      if v==nil then
        v=""
      end
      local K=upper(k)
      osenv[K]=v
      if type(v)=="table" then
        v=concat(v,";") 
      end
      ossetenv(K,v)
    end
    function os.getenv(k)
      local K=upper(k)
      local v=osenv[K] or osenv[k] or osgetenv(K) or osgetenv(k)
      if v=="" then
        return nil
      else
        return v
      end
    end
  else
    local ossetenv=os.setenv
    local osgetenv=os.getenv
    local osenv={}
    function os.setenv(k,v)
      if v==nil then
        v=""
      end
      local K=upper(k)
      osenv[K]=v
    end
    function os.getenv(k)
      local K=upper(k)
      local v=osenv[K] or osgetenv(K) or osgetenv(k)
      if v=="" then
        return nil
      else
        return v
      end
    end
    local function __index(t,k)
      return os.getenv(k)
    end
    local function __newindex(t,k,v)
      os.setenv(k,v)
    end
    os.env={}
    setmetatable(os.env,{ __index=__index,__newindex=__newindex } )
  end
end
local execute,spawn,exec,iopopen,ioflush=os.execute,os.spawn or os.execute,os.exec or os.execute,io.popen,io.flush
function os.execute(...) ioflush() return execute(...) end
function os.spawn (...) ioflush() return spawn (...) end
function os.exec  (...) ioflush() return exec  (...) end
function io.popen (...) ioflush() return iopopen(...) end
function os.resultof(command)
  local handle=io.popen(command,"r")
  return handle and handle:read("*all") or ""
end
if not io.fileseparator then
  if find(os.getenv("PATH"),";") then
    io.fileseparator,io.pathseparator,os.type="\\",";",os.type or "mswin"
  else
    io.fileseparator,io.pathseparator,os.type="/",":",os.type or "unix"
  end
end
os.type=os.type or (io.pathseparator==";"    and "windows") or "unix"
os.name=os.name or (os.type=="windows" and "mswin" ) or "linux"
if os.type=="windows" then
  os.libsuffix,os.binsuffix,os.binsuffixes='dll','exe',{ 'exe','cmd','bat' }
else
  os.libsuffix,os.binsuffix,os.binsuffixes='so','',{ '' }
end
local launchers={
  windows="start %s",
  macosx="open %s",
  unix="$BROWSER %s &> /dev/null &",
}
function os.launch(str)
  os.execute(format(launchers[os.name] or launchers.unix,str))
end
if not os.times then
  function os.times()
    return {
      utime=os.gettimeofday(),
      stime=0,
      cutime=0,
      cstime=0,
    }
  end
end
os.gettimeofday=os.gettimeofday or os.clock
local startuptime=os.gettimeofday()
function os.runtime()
  return os.gettimeofday()-startuptime
end
os.resolvers=os.resolvers or {} 
local resolvers=os.resolvers
setmetatable(os,{ __index=function(t,k)
  local r=resolvers[k]
  return r and r(t,k) or nil 
end })
local name,platform=os.name or "linux",os.getenv("MTX_PLATFORM") or ""
local function guess()
  local architecture=os.resultof("uname -m") or ""
  if architecture~="" then
    return architecture
  end
  architecture=os.getenv("HOSTTYPE") or ""
  if architecture~="" then
    return architecture
  end
  return os.resultof("echo $HOSTTYPE") or ""
end
if platform~="" then
  os.platform=platform
elseif os.type=="windows" then
  function os.resolvers.platform(t,k)
    local platform,architecture="",os.getenv("PROCESSOR_ARCHITECTURE") or ""
    if find(architecture,"AMD64") then
      platform="mswin-64"
    else
      platform="mswin"
    end
    os.setenv("MTX_PLATFORM",platform)
    os.platform=platform
    return platform
  end
elseif name=="linux" then
  function os.resolvers.platform(t,k)
    local platform,architecture="",os.getenv("HOSTTYPE") or os.resultof("uname -m") or ""
    if find(architecture,"x86_64") then
      platform="linux-64"
    elseif find(architecture,"ppc") then
      platform="linux-ppc"
    else
      platform="linux"
    end
    os.setenv("MTX_PLATFORM",platform)
    os.platform=platform
    return platform
  end
elseif name=="macosx" then
  function os.resolvers.platform(t,k)
    local platform,architecture="",os.resultof("echo $HOSTTYPE") or ""
    if architecture=="" then
      platform="osx-intel"
    elseif find(architecture,"i386") then
      platform="osx-intel"
    elseif find(architecture,"x86_64") then
      platform="osx-64"
    else
      platform="osx-ppc"
    end
    os.setenv("MTX_PLATFORM",platform)
    os.platform=platform
    return platform
  end
elseif name=="sunos" then
  function os.resolvers.platform(t,k)
    local platform,architecture="",os.resultof("uname -m") or ""
    if find(architecture,"sparc") then
      platform="solaris-sparc"
    else 
      platform="solaris-intel"
    end
    os.setenv("MTX_PLATFORM",platform)
    os.platform=platform
    return platform
  end
elseif name=="freebsd" then
  function os.resolvers.platform(t,k)
    local platform,architecture="",os.resultof("uname -m") or ""
    if find(architecture,"amd64") then
      platform="freebsd-amd64"
    else
      platform="freebsd"
    end
    os.setenv("MTX_PLATFORM",platform)
    os.platform=platform
    return platform
  end
elseif name=="kfreebsd" then
  function os.resolvers.platform(t,k)
    local platform,architecture="",os.getenv("HOSTTYPE") or os.resultof("uname -m") or ""
    if find(architecture,"x86_64") then
      platform="kfreebsd-amd64"
    else
      platform="kfreebsd-i386"
    end
    os.setenv("MTX_PLATFORM",platform)
    os.platform=platform
    return platform
  end
else
  function os.resolvers.platform(t,k)
    local platform="linux"
    os.setenv("MTX_PLATFORM",platform)
    os.platform=platform
    return platform
  end
end
local t={ 8,9,"a","b" }
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
  d=d or tonumber(tonumber(date("%H")-date("!%H")))
  if delta then
    if d>0 then
      return format("+%02i:00",d)
    else
      return format("-%02i:00",-d)
    end
  else
    return 1
  end
end
local timeformat=format("%%s%s",os.timezone(true))
local dateformat="!%Y-%m-%d %H:%M:%S"
function os.fulltime(t,default)
  t=tonumber(t) or 0
  if t>0 then
  elseif default then
    return default
  else
    t=nil
  end
  return format(timeformat,date(dateformat,t))
end
local dateformat="%Y-%m-%d %H:%M:%S"
function os.localtime(t,default)
  t=tonumber(t) or 0
  if t>0 then
  elseif default then
    return default
  else
    t=nil
  end
  return date(dateformat,t)
end
function os.converttime(t,default)
  local t=tonumber(t)
  if t and t>0 then
    return date(dateformat,t)
  else
    return default or "-"
  end
end
local memory={}
local function which(filename)
  local fullname=memory[filename]
  if fullname==nil then
    local suffix=file.suffix(filename)
    local suffixes=suffix=="" and os.binsuffixes or { suffix }
    for directory in gmatch(os.getenv("PATH"),"[^"..io.pathseparator.."]+") do
      local df=file.join(directory,filename)
      for i=1,#suffixes do
        local dfs=file.addsuffix(df,suffixes[i])
        if io.exists(dfs) then
          fullname=dfs
          break
        end
      end
    end
    if not fullname then
      fullname=false
    end
    memory[filename]=fullname
  end
  return fullname
end
os.which=which
os.where=which
function os.today()
  return date("!*t") 
end
function os.now()
  return date("!%Y-%m-%d %H:%M:%S") 
end
if not os.sleep and socket then
  os.sleep=socket.sleep
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-file"] = package.loaded["l-file"] or true

-- original size: 16648, stripped down to: 9051

if not modules then modules={} end modules ['l-file']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
file=file or {}
local file=file
if not lfs then
  lfs=optionalrequire("lfs")
end
if not lfs then
  lfs={
    getcurrentdir=function()
      return "."
    end,
    attributes=function()
      return nil
    end,
    isfile=function(name)
      local f=io.open(name,'rb')
      if f then
        f:close()
        return true
      end
    end,
    isdir=function(name)
      print("you need to load lfs")
      return false
    end
  }
elseif not lfs.isfile then
  local attributes=lfs.attributes
  function lfs.isdir(name)
    return attributes(name,"mode")=="directory"
  end
  function lfs.isfile(name)
    return attributes(name,"mode")=="file"
  end
end
local insert,concat=table.insert,table.concat
local match=string.match
local lpegmatch=lpeg.match
local getcurrentdir,attributes=lfs.currentdir,lfs.attributes
local checkedsplit=string.checkedsplit
local P,R,S,C,Cs,Cp,Cc,Ct=lpeg.P,lpeg.R,lpeg.S,lpeg.C,lpeg.Cs,lpeg.Cp,lpeg.Cc,lpeg.Ct
local colon=P(":")
local period=P(".")
local periods=P("..")
local fwslash=P("/")
local bwslash=P("\\")
local slashes=S("\\/")
local noperiod=1-period
local noslashes=1-slashes
local name=noperiod^1
local suffix=period/""*(1-period-slashes)^1*-1
local pattern=C((1-(slashes^1*noslashes^1*-1))^1)*P(1) 
local function pathpart(name,default)
  return name and lpegmatch(pattern,name) or default or ""
end
local pattern=(noslashes^0*slashes)^1*C(noslashes^1)*-1
local function basename(name)
  return name and lpegmatch(pattern,name) or name
end
local pattern=(noslashes^0*slashes^1)^0*Cs((1-suffix)^1)*suffix^0
local function nameonly(name)
  return name and lpegmatch(pattern,name) or name
end
local pattern=(noslashes^0*slashes)^0*(noperiod^1*period)^1*C(noperiod^1)*-1
local function suffixonly(name)
  return name and lpegmatch(pattern,name) or ""
end
file.pathpart=pathpart
file.basename=basename
file.nameonly=nameonly
file.suffixonly=suffixonly
file.suffix=suffixonly
file.dirname=pathpart  
file.extname=suffixonly
local drive=C(R("az","AZ"))*colon
local path=C((noslashes^0*slashes)^0)
local suffix=period*C(P(1-period)^0*P(-1))
local base=C((1-suffix)^0)
local rest=C(P(1)^0)
drive=drive+Cc("")
path=path+Cc("")
base=base+Cc("")
suffix=suffix+Cc("")
local pattern_a=drive*path*base*suffix
local pattern_b=path*base*suffix
local pattern_c=C(drive*path)*C(base*suffix) 
local pattern_d=path*rest
function file.splitname(str,splitdrive)
  if not str then
  elseif splitdrive then
    return lpegmatch(pattern_a,str) 
  else
    return lpegmatch(pattern_b,str) 
  end
end
function file.splitbase(str)
  return str and lpegmatch(pattern_d,str) 
end
function file.nametotable(str,splitdrive)
  if str then
    local path,drive,subpath,name,base,suffix=lpegmatch(pattern_c,str)
    if splitdrive then
      return {
        path=path,
        drive=drive,
        subpath=subpath,
        name=name,
        base=base,
        suffix=suffix,
      }
    else
      return {
        path=path,
        name=name,
        base=base,
        suffix=suffix,
      }
    end
  end
end
local pattern=Cs(((period*(1-period-slashes)^1*-1)/""+1)^1)
function file.removesuffix(name)
  return name and lpegmatch(pattern,name)
end
local suffix=period/""*(1-period-slashes)^1*-1
local pattern=Cs((noslashes^0*slashes^1)^0*((1-suffix)^1))*Cs(suffix)
function file.addsuffix(filename,suffix,criterium)
  if not filename or not suffix or suffix=="" then
    return filename
  elseif criterium==true then
    return filename.."."..suffix
  elseif not criterium then
    local n,s=lpegmatch(pattern,filename)
    if not s or s=="" then
      return filename.."."..suffix
    else
      return filename
    end
  else
    local n,s=lpegmatch(pattern,filename)
    if s and s~="" then
      local t=type(criterium)
      if t=="table" then
        for i=1,#criterium do
          if s==criterium[i] then
            return filename
          end
        end
      elseif t=="string" then
        if s==criterium then
          return filename
        end
      end
    end
    return (n or filename).."."..suffix
  end
end
local suffix=period*(1-period-slashes)^1*-1
local pattern=Cs((1-suffix)^0)
function file.replacesuffix(name,suffix)
  if name and suffix and suffix~="" then
    return lpegmatch(pattern,name).."."..suffix
  else
    return name
  end
end
local reslasher=lpeg.replacer(P("\\"),"/")
function file.reslash(str)
  return str and lpegmatch(reslasher,str)
end
function file.is_writable(name)
  if not name then
  elseif lfs.isdir(name) then
    name=name.."/m_t_x_t_e_s_t.tmp"
    local f=io.open(name,"wb")
    if f then
      f:close()
      os.remove(name)
      return true
    end
  elseif lfs.isfile(name) then
    local f=io.open(name,"ab")
    if f then
      f:close()
      return true
    end
  else
    local f=io.open(name,"ab")
    if f then
      f:close()
      os.remove(name)
      return true
    end
  end
  return false
end
local readable=P("r")*Cc(true)
function file.is_readable(name)
  if name then
    local a=attributes(name)
    return a and lpegmatch(readable,a.permissions) or false
  else
    return false
  end
end
file.isreadable=file.is_readable 
file.iswritable=file.is_writable 
function file.size(name)
  if name then
    local a=attributes(name)
    return a and a.size or 0
  else
    return 0
  end
end
function file.splitpath(str,separator) 
  return str and checkedsplit(lpegmatch(reslasher,str),separator or io.pathseparator)
end
function file.joinpath(tab,separator) 
  return tab and concat(tab,separator or io.pathseparator) 
end
local stripper=Cs(P(fwslash)^0/""*reslasher)
local isnetwork=fwslash*fwslash*(1-fwslash)+(1-fwslash-colon)^1*colon
local isroot=fwslash^1*-1
local hasroot=fwslash^1
local deslasher=lpeg.replacer(S("\\/")^1,"/")
function file.join(...)
  local lst={... }
  local one=lst[1]
  if lpegmatch(isnetwork,one) then
    local two=lpegmatch(deslasher,concat(lst,"/",2))
    return one.."/"..two
  elseif lpegmatch(isroot,one) then
    local two=lpegmatch(deslasher,concat(lst,"/",2))
    if lpegmatch(hasroot,two) then
      return two
    else
      return "/"..two
    end
  elseif one=="" then
    return lpegmatch(stripper,concat(lst,"/",2))
  else
    return lpegmatch(deslasher,concat(lst,"/"))
  end
end
local drivespec=R("az","AZ")^1*colon
local anchors=fwslash+drivespec
local untouched=periods+(1-period)^1*P(-1)
local splitstarter=(Cs(drivespec*(bwslash/"/"+fwslash)^0)+Cc(false))*Ct(lpeg.splitat(S("/\\")^1))
local absolute=fwslash
function file.collapsepath(str,anchor)
  if not str then
    return
  end
  if anchor and not lpegmatch(anchors,str) then
    str=getcurrentdir().."/"..str
  end
  if str=="" or str=="." then
    return "."
  elseif lpegmatch(untouched,str) then
    return lpegmatch(reslasher,str)
  end
  local starter,oldelements=lpegmatch(splitstarter,str)
  local newelements={}
  local i=#oldelements
  while i>0 do
    local element=oldelements[i]
    if element=='.' then
    elseif element=='..' then
      local n=i-1
      while n>0 do
        local element=oldelements[n]
        if element~='..' and element~='.' then
          oldelements[n]='.'
          break
        else
          n=n-1
        end
       end
      if n<1 then
        insert(newelements,1,'..')
      end
    elseif element~="" then
      insert(newelements,1,element)
    end
    i=i-1
  end
  if #newelements==0 then
    return starter or "."
  elseif starter then
    return starter..concat(newelements,'/')
  elseif lpegmatch(absolute,str) then
    return "/"..concat(newelements,'/')
  else
    return concat(newelements,'/')
  end
end
local validchars=R("az","09","AZ","--","..")
local pattern_a=lpeg.replacer(1-validchars)
local pattern_a=Cs((validchars+P(1)/"-")^1)
local whatever=P("-")^0/""
local pattern_b=Cs(whatever*(1-whatever*-1)^1)
function file.robustname(str,strict)
  if str then
    str=lpegmatch(pattern_a,str) or str
    if strict then
      return lpegmatch(pattern_b,str) or str 
    else
      return str
    end
  end
end
file.readdata=io.loaddata
file.savedata=io.savedata
function file.copy(oldname,newname)
  if oldname and newname then
    local data=io.loaddata(oldname)
    if data and data~="" then
      file.savedata(newname,data)
    end
  end
end
local letter=R("az","AZ")+S("_-+")
local separator=P("://")
local qualified=period^0*fwslash+letter*colon+letter^1*separator+letter^1*fwslash
local rootbased=fwslash+letter*colon
lpeg.patterns.qualified=qualified
lpeg.patterns.rootbased=rootbased
function file.is_qualified_path(filename)
  return filename and lpegmatch(qualified,filename)~=nil
end
function file.is_rootbased_path(filename)
  return filename and lpegmatch(rootbased,filename)~=nil
end
function file.strip(name,dir)
  if name then
    local b,a=match(name,"^(.-)"..dir.."(.*)$")
    return a~="" and a or name
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-md5"] = package.loaded["l-md5"] or true

-- original size: 3760, stripped down to: 2088

if not modules then modules={} end modules ['l-md5']={
  version=1.001,
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
if not md5 then
  md5=optionalrequire("md5")
end
if not md5 then
  md5={
    sum=function(str) print("error: md5 is not loaded (sum     ignored)") return str end,
    sumhexa=function(str) print("error: md5 is not loaded (sumhexa ignored)") return str end,
  }
end
local md5,file=md5,file
local gsub,format,byte=string.gsub,string.format,string.byte
local md5sum=md5.sum
local function convert(str,fmt)
  return (gsub(md5sum(str),".",function(chr) return format(fmt,byte(chr)) end))
end
if not md5.HEX then function md5.HEX(str) return convert(str,"%02X") end end
if not md5.hex then function md5.hex(str) return convert(str,"%02x") end end
if not md5.dec then function md5.dec(str) return convert(str,"%03i") end end
function file.needsupdating(oldname,newname,threshold) 
  local oldtime=lfs.attributes(oldname,"modification")
  if oldtime then
    local newtime=lfs.attributes(newname,"modification")
    if not newtime then
      return true 
    elseif newtime>=oldtime then
      return false 
    elseif oldtime-newtime<(threshold or 1) then
      return false 
    else
      return true 
    end
  else
    return false 
  end
end
file.needs_updating=file.needsupdating
function file.syncmtimes(oldname,newname)
  local oldtime=lfs.attributes(oldname,"modification")
  if oldtime and lfs.isfile(newname) then
    lfs.touch(newname,oldtime,oldtime)
  end
end
function file.checksum(name)
  if md5 then
    local data=io.loaddata(name)
    if data then
      return md5.HEX(data)
    end
  end
  return nil
end
function file.loadchecksum(name)
  if md5 then
    local data=io.loaddata(name..".md5")
    return data and (gsub(data,"%s",""))
  end
  return nil
end
function file.savechecksum(name,checksum)
  if not checksum then checksum=file.checksum(name) end
  if checksum then
    io.savedata(name..".md5",checksum)
    return checksum
  end
  return nil
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-url"] = package.loaded["l-url"] or true

-- original size: 11806, stripped down to: 5417

if not modules then modules={} end modules ['l-url']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local char,format,byte=string.char,string.format,string.byte
local concat=table.concat
local tonumber,type=tonumber,type
local P,C,R,S,Cs,Cc,Ct,Cf,Cg,V=lpeg.P,lpeg.C,lpeg.R,lpeg.S,lpeg.Cs,lpeg.Cc,lpeg.Ct,lpeg.Cf,lpeg.Cg,lpeg.V
local lpegmatch,lpegpatterns,replacer=lpeg.match,lpeg.patterns,lpeg.replacer
url=url or {}
local url=url
local tochar=function(s) return char(tonumber(s,16)) end
local colon=P(":")
local qmark=P("?")
local hash=P("#")
local slash=P("/")
local percent=P("%")
local endofstring=P(-1)
local hexdigit=R("09","AF","af")
local plus=P("+")
local nothing=Cc("")
local escapedchar=(percent*C(hexdigit*hexdigit))/tochar
local escaped=(plus/" ")+escapedchar
local noslash=P("/")/""
local schemestr=Cs((escaped+(1-colon-slash-qmark-hash))^2)
local authoritystr=Cs((escaped+(1-   slash-qmark-hash))^0)
local pathstr=Cs((escaped+(1-      qmark-hash))^0)
local querystr=Cs(((1-         hash))^0)
local fragmentstr=Cs((escaped+(1-      endofstring))^0)
local scheme=schemestr*colon+nothing
local authority=slash*slash*authoritystr+nothing
local path=slash*pathstr+nothing
local query=qmark*querystr+nothing
local fragment=hash*fragmentstr+nothing
local validurl=scheme*authority*path*query*fragment
local parser=Ct(validurl)
lpegpatterns.url=validurl
lpegpatterns.urlsplitter=parser
local escapes={}
setmetatable(escapes,{ __index=function(t,k)
  local v=format("%%%02X",byte(k))
  t[k]=v
  return v
end })
local escaper=Cs((R("09","AZ","az")^1+P(" ")/"%%20"+S("-./_")^1+P(1)/escapes)^0) 
local unescaper=Cs((escapedchar+1)^0)
lpegpatterns.urlunescaped=escapedchar
lpegpatterns.urlescaper=escaper
lpegpatterns.urlunescaper=unescaper
local function split(str)
  return (type(str)=="string" and lpegmatch(parser,str)) or str
end
local isscheme=schemestr*colon*slash*slash 
local function hasscheme(str)
  if str then
    local scheme=lpegmatch(isscheme,str) 
    return scheme~="" and scheme or false
  else
    return false
  end
end
local rootletter=R("az","AZ")+S("_-+")
local separator=P("://")
local qualified=P(".")^0*P("/")+rootletter*P(":")+rootletter^1*separator+rootletter^1*P("/")
local rootbased=P("/")+rootletter*P(":")
local barswapper=replacer("|",":")
local backslashswapper=replacer("\\","/")
local equal=P("=")
local amp=P("&")
local key=Cs(((escapedchar+1)-equal      )^0)
local value=Cs(((escapedchar+1)-amp -endofstring)^0)
local splitquery=Cf (Ct("")*P { "sequence",
  sequence=V("pair")*(amp*V("pair"))^0,
  pair=Cg(key*equal*value),
},rawset)
local function hashed(str) 
  if str=="" then
    return {
      scheme="invalid",
      original=str,
    }
  end
  local s=split(str)
  local rawscheme=s[1]
  local rawquery=s[4]
  local somescheme=rawscheme~=""
  local somequery=rawquery~=""
  if not somescheme and not somequery then
    s={
      scheme="file",
      authority="",
      path=str,
      query="",
      fragment="",
      original=str,
      noscheme=true,
      filename=str,
    }
  else 
    local authority,path,filename=s[2],s[3]
    if authority=="" then
      filename=path
    elseif path=="" then
      filename=""
    else
      filename=authority.."/"..path
    end
    s={
      scheme=rawscheme,
      authority=authority,
      path=path,
      query=lpegmatch(unescaper,rawquery),
      queries=lpegmatch(splitquery,rawquery),
      fragment=s[5],
      original=str,
      noscheme=false,
      filename=filename,
    }
  end
  return s
end
url.split=split
url.hasscheme=hasscheme
url.hashed=hashed
function url.addscheme(str,scheme) 
  if hasscheme(str) then
    return str
  elseif not scheme then
    return "file:///"..str
  else
    return scheme..":///"..str
  end
end
function url.construct(hash) 
  local fullurl,f={},0
  local scheme,authority,path,query,fragment=hash.scheme,hash.authority,hash.path,hash.query,hash.fragment
  if scheme and scheme~="" then
    f=f+1;fullurl[f]=scheme.."://"
  end
  if authority and authority~="" then
    f=f+1;fullurl[f]=authority
  end
  if path and path~="" then
    f=f+1;fullurl[f]="/"..path
  end
  if query and query~="" then
    f=f+1;fullurl[f]="?"..query
  end
  if fragment and fragment~="" then
    f=f+1;fullurl[f]="#"..fragment
  end
  return lpegmatch(escaper,concat(fullurl))
end
local pattern=Cs(noslash*R("az","AZ")*(S(":|")/":")*noslash*P(1)^0)
function url.filename(filename)
  local spec=hashed(filename)
  local path=spec.path
  return (spec.scheme=="file" and path and lpegmatch(pattern,path)) or filename
end
local function escapestring(str)
  return lpegmatch(escaper,str)
end
url.escape=escapestring
function url.query(str)
  if type(str)=="string" then
    return lpegmatch(splitquery,str) or ""
  else
    return str
  end
end
function url.toquery(data)
  local td=type(data)
  if td=="string" then
    return #str and escape(data) or nil 
  elseif td=="table" then
    if next(data) then
      local t={}
      for k,v in next,data do
        t[#t+1]=format("%s=%s",k,escapestring(v))
      end
      return concat(t,"&")
    end
  else
  end
end
local pattern=Cs(noslash^0*(1-noslash*P(-1))^0)
function url.barepath(path)
  if not path or path=="" then
    return ""
  else
    return lpegmatch(pattern,path)
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-dir"] = package.loaded["l-dir"] or true

-- original size: 13139, stripped down to: 8196

if not modules then modules={} end modules ['l-dir']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local type,select=type,select
local find,gmatch,match,gsub=string.find,string.gmatch,string.match,string.gsub
local concat,insert,remove=table.concat,table.insert,table.remove
local lpegmatch=lpeg.match
local P,S,R,C,Cc,Cs,Ct,Cv,V=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.Cc,lpeg.Cs,lpeg.Ct,lpeg.Cv,lpeg.V
dir=dir or {}
local dir=dir
local lfs=lfs
local attributes=lfs.attributes
local walkdir=lfs.dir
local isdir=lfs.isdir
local isfile=lfs.isfile
local currentdir=lfs.currentdir
local chdir=lfs.chdir
if not isdir then
  function isdir(name)
    local a=attributes(name)
    return a and a.mode=="directory"
  end
  lfs.isdir=isdir
end
if not isfile then
  function isfile(name)
    local a=attributes(name)
    return a and a.mode=="file"
  end
  lfs.isfile=isfile
end
function dir.current()
  return (gsub(currentdir(),"\\","/"))
end
local lfsisdir=isdir
local function isdir(path)
  path=gsub(path,"[/\\]+$","")
  return lfsisdir(path)
end
lfs.isdir=isdir
local function globpattern(path,patt,recurse,action)
  if path=="/" then
    path=path.."."
  elseif not find(path,"/$") then
    path=path..'/'
  end
  if isdir(path) then 
    for name in walkdir(path) do 
      local full=path..name
      local mode=attributes(full,'mode')
      if mode=='file' then
        if find(full,patt) then
          action(full)
        end
      elseif recurse and (mode=="directory") and (name~='.') and (name~="..") then
        globpattern(full,patt,recurse,action)
      end
    end
  end
end
dir.globpattern=globpattern
local function collectpattern(path,patt,recurse,result)
  local ok,scanner
  result=result or {}
  if path=="/" then
    ok,scanner,first=xpcall(function() return walkdir(path..".") end,function() end) 
  else
    ok,scanner,first=xpcall(function() return walkdir(path)   end,function() end) 
  end
  if ok and type(scanner)=="function" then
    if not find(path,"/$") then path=path..'/' end
    for name in scanner,first do
      local full=path..name
      local attr=attributes(full)
      local mode=attr.mode
      if mode=='file' then
        if find(full,patt) then
          result[name]=attr
        end
      elseif recurse and (mode=="directory") and (name~='.') and (name~="..") then
        attr.list=collectpattern(full,patt,recurse)
        result[name]=attr
      end
    end
  end
  return result
end
dir.collectpattern=collectpattern
local pattern=Ct {
  [1]=(C(P(".")+P("/")^1)+C(R("az","AZ")*P(":")*P("/")^0)+Cc("./"))*V(2)*V(3),
  [2]=C(((1-S("*?/"))^0*P("/"))^0),
  [3]=C(P(1)^0)
}
local filter=Cs ((
  P("**")/".*"+P("*")/"[^/]*"+P("?")/"[^/]"+P(".")/"%%."+P("+")/"%%+"+P("-")/"%%-"+P(1)
)^0 )
local function glob(str,t)
  if type(t)=="function" then
    if type(str)=="table" then
      for s=1,#str do
        glob(str[s],t)
      end
    elseif isfile(str) then
      t(str)
    else
      local split=lpegmatch(pattern,str) 
      if split then
        local root,path,base=split[1],split[2],split[3]
        local recurse=find(base,"%*%*")
        local start=root..path
        local result=lpegmatch(filter,start..base)
        globpattern(start,result,recurse,t)
      end
    end
  else
    if type(str)=="table" then
      local t=t or {}
      for s=1,#str do
        glob(str[s],t)
      end
      return t
    elseif isfile(str) then
      if t then
        t[#t+1]=str
        return t
      else
        return { str }
      end
    else
      local split=lpegmatch(pattern,str) 
      if split then
        local t=t or {}
        local action=action or function(name) t[#t+1]=name end
        local root,path,base=split[1],split[2],split[3]
        local recurse=find(base,"%*%*")
        local start=root..path
        local result=lpegmatch(filter,start..base)
        globpattern(start,result,recurse,action)
        return t
      else
        return {}
      end
    end
  end
end
dir.glob=glob
local function globfiles(path,recurse,func,files) 
  if type(func)=="string" then
    local s=func
    func=function(name) return find(name,s) end
  end
  files=files or {}
  local noffiles=#files
  for name in walkdir(path) do
    if find(name,"^%.") then
    else
      local mode=attributes(name,'mode')
      if mode=="directory" then
        if recurse then
          globfiles(path.."/"..name,recurse,func,files)
        end
      elseif mode=="file" then
        if not func or func(name) then
          noffiles=noffiles+1
          files[noffiles]=path.."/"..name
        end
      end
    end
  end
  return files
end
dir.globfiles=globfiles
function dir.ls(pattern)
  return concat(glob(pattern),"\n")
end
local make_indeed=true 
local onwindows=os.type=="windows" or find(os.getenv("PATH"),";")
if onwindows then
  function dir.mkdirs(...)
    local str,pth="",""
    for i=1,select("#",...) do
      local s=select(i,...)
      if s=="" then
      elseif str=="" then
        str=s
      else
        str=str.."/"..s
      end
    end
    local first,middle,last
    local drive=false
    first,middle,last=match(str,"^(//)(//*)(.*)$")
    if first then
    else
      first,last=match(str,"^(//)/*(.-)$")
      if first then
        middle,last=match(str,"([^/]+)/+(.-)$")
        if middle then
          pth="//"..middle
        else
          pth="//"..last
          last=""
        end
      else
        first,middle,last=match(str,"^([a-zA-Z]:)(/*)(.-)$")
        if first then
          pth,drive=first..middle,true
        else
          middle,last=match(str,"^(/*)(.-)$")
          if not middle then
            last=str
          end
        end
      end
    end
    for s in gmatch(last,"[^/]+") do
      if pth=="" then
        pth=s
      elseif drive then
        pth,drive=pth..s,false
      else
        pth=pth.."/"..s
      end
      if make_indeed and not isdir(pth) then
        lfs.mkdir(pth)
      end
    end
    return pth,(isdir(pth)==true)
  end
else
  function dir.mkdirs(...)
    local str,pth="",""
    for i=1,select("#",...) do
      local s=select(i,...)
      if s and s~="" then 
        if str~="" then
          str=str.."/"..s
        else
          str=s
        end
      end
    end
    str=gsub(str,"/+","/")
    if find(str,"^/") then
      pth="/"
      for s in gmatch(str,"[^/]+") do
        local first=(pth=="/")
        if first then
          pth=pth..s
        else
          pth=pth.."/"..s
        end
        if make_indeed and not first and not isdir(pth) then
          lfs.mkdir(pth)
        end
      end
    else
      pth="."
      for s in gmatch(str,"[^/]+") do
        pth=pth.."/"..s
        if make_indeed and not isdir(pth) then
          lfs.mkdir(pth)
        end
      end
    end
    return pth,(isdir(pth)==true)
  end
end
dir.makedirs=dir.mkdirs
if onwindows then
  function dir.expandname(str) 
    local first,nothing,last=match(str,"^(//)(//*)(.*)$")
    if first then
      first=dir.current().."/" 
    end
    if not first then
      first,last=match(str,"^(//)/*(.*)$")
    end
    if not first then
      first,last=match(str,"^([a-zA-Z]:)(.*)$")
      if first and not find(last,"^/") then
        local d=currentdir()
        if chdir(first) then
          first=dir.current()
        end
        chdir(d)
      end
    end
    if not first then
      first,last=dir.current(),str
    end
    last=gsub(last,"//","/")
    last=gsub(last,"/%./","/")
    last=gsub(last,"^/*","")
    first=gsub(first,"/*$","")
    if last=="" or last=="." then
      return first
    else
      return first.."/"..last
    end
  end
else
  function dir.expandname(str) 
    if not find(str,"^/") then
      str=currentdir().."/"..str
    end
    str=gsub(str,"//","/")
    str=gsub(str,"/%./","/")
    str=gsub(str,"(.)/%.$","%1")
    return str
  end
end
file.expandname=dir.expandname 
local stack={}
function dir.push(newdir)
  insert(stack,currentdir())
  if newdir and newdir~="" then
    chdir(newdir)
  end
end
function dir.pop()
  local d=remove(stack)
  if d then
    chdir(d)
  end
  return d
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-boolean"] = package.loaded["l-boolean"] or true

-- original size: 1781, stripped down to: 1503

if not modules then modules={} end modules ['l-boolean']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local type,tonumber=type,tonumber
boolean=boolean or {}
local boolean=boolean
function boolean.tonumber(b)
  if b then return 1 else return 0 end 
end
function toboolean(str,tolerant) 
  if str==nil then
    return false
  elseif str==false then
    return false
  elseif str==true then
    return true
  elseif str=="true" then
    return true
  elseif str=="false" then
    return false
  elseif not tolerant then
    return false
  elseif str==0 then
    return false
  elseif (tonumber(str) or 0)>0 then
    return true
  else
    return str=="yes" or str=="on" or str=="t"
  end
end
string.toboolean=toboolean
function string.booleanstring(str)
  if str=="0" then
    return false
  elseif str=="1" then
    return true
  elseif str=="" then
    return false
  elseif str=="false" then
    return false
  elseif str=="true" then
    return true
  elseif (tonumber(str) or 0)>0 then
    return true
  else
    return str=="yes" or str=="on" or str=="t"
  end
end
function string.is_boolean(str,default)
  if type(str)=="string" then
    if str=="true" or str=="yes" or str=="on" or str=="t" then
      return true
    elseif str=="false" or str=="no" or str=="off" or str=="f" then
      return false
    end
  end
  return default
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-unicode"] = package.loaded["l-unicode"] or true

-- original size: 26810, stripped down to: 11943

if not modules then modules={} end modules ['l-unicode']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
utf=utf or (unicode and unicode.utf8) or {}
utf.characters=utf.characters or string.utfcharacters
utf.values=utf.values   or string.utfvalues
local type=type
local char,byte,format,sub=string.char,string.byte,string.format,string.sub
local concat=table.concat
local P,C,R,Cs,Ct,Cmt,Cc,Carg,Cp=lpeg.P,lpeg.C,lpeg.R,lpeg.Cs,lpeg.Ct,lpeg.Cmt,lpeg.Cc,lpeg.Carg,lpeg.Cp
local lpegmatch,patterns=lpeg.match,lpeg.patterns
local bytepairs=string.bytepairs
local finder=lpeg.finder
local replacer=lpeg.replacer
local utfvalues=utf.values
local utfgmatch=utf.gmatch 
local p_utftype=patterns.utftype
local p_utfoffset=patterns.utfoffset
local p_utf8char=patterns.utf8char
local p_utf8byte=patterns.utf8byte
local p_utfbom=patterns.utfbom
local p_newline=patterns.newline
local p_whitespace=patterns.whitespace
if not unicode then
  unicode={ utf=utf } 
end
if not utf.char then
  local floor,char=math.floor,string.char
  function utf.char(n)
    if n<0x80 then
      return char(n)
    elseif n<0x800 then
      return char(
        0xC0+floor(n/0x40),
        0x80+(n%0x40)
      )
    elseif n<0x10000 then
      return char(
        0xE0+floor(n/0x1000),
        0x80+(floor(n/0x40)%0x40),
        0x80+(n%0x40)
      )
    elseif n<0x200000 then
      return char(
        0xF0+floor(n/0x40000),
        0x80+(floor(n/0x1000)%0x40),
        0x80+(floor(n/0x40)%0x40),
        0x80+(n%0x40)
      )
    else
      return ""
    end
  end
end
if not utf.byte then
  local utf8byte=patterns.utf8byte
  function utf.byte(c)
    return lpegmatch(utf8byte,c)
  end
end
local utfchar,utfbyte=utf.char,utf.byte
function utf.filetype(data)
  return data and lpegmatch(p_utftype,data) or "unknown"
end
local toentities=Cs (
  (
    patterns.utf8one+(
        patterns.utf8two+patterns.utf8three+patterns.utf8four
      )/function(s) local b=utfbyte(s) if b<127 then return s else return format("&#%X;",b) end end
  )^0
)
patterns.toentities=toentities
function utf.toentities(str)
  return lpegmatch(toentities,str)
end
local one=P(1)
local two=C(1)*C(1)
local four=C(R(utfchar(0xD8),utfchar(0xFF)))*C(1)*C(1)*C(1)
local pattern=P("\254\255")*Cs((
          four/function(a,b,c,d)
                local ab=0xFF*byte(a)+byte(b)
                local cd=0xFF*byte(c)+byte(d)
                return utfchar((ab-0xD800)*0x400+(cd-0xDC00)+0x10000)
              end+two/function(a,b)
                return utfchar(byte(a)*256+byte(b))
              end+one
        )^1 )+P("\255\254")*Cs((
          four/function(b,a,d,c)
                local ab=0xFF*byte(a)+byte(b)
                local cd=0xFF*byte(c)+byte(d)
                return utfchar((ab-0xD800)*0x400+(cd-0xDC00)+0x10000)
              end+two/function(b,a)
                return utfchar(byte(a)*256+byte(b))
              end+one
        )^1 )
function string.toutf(s) 
  return lpegmatch(pattern,s) or s 
end
local validatedutf=Cs (
  (
    patterns.utf8one+patterns.utf8two+patterns.utf8three+patterns.utf8four+P(1)/"�"
  )^0
)
patterns.validatedutf=validatedutf
function utf.is_valid(str)
  return type(str)=="string" and lpegmatch(validatedutf,str) or false
end
if not utf.len then
  local n,f=0,1
  local utfcharcounter=patterns.utfbom^-1*Cmt (
    Cc(1)*patterns.utf8one^1+Cc(2)*patterns.utf8two^1+Cc(3)*patterns.utf8three^1+Cc(4)*patterns.utf8four^1,
    function(_,t,d) 
      n=n+(t-f)/d
      f=t
      return true
    end
  )^0
  function utf.len(str)
    n,f=0,1
    lpegmatch(utfcharcounter,str or "")
    return n
  end
end
utf.length=utf.len
if not utf.sub then
  local utflength=utf.length
  local b,e,n,first,last=0,0,0,0,0
  local function slide_zero(s,p)
    n=n+1
    if n>=last then
      e=p-1
    else
      return p
    end
  end
  local function slide_one(s,p)
    n=n+1
    if n==first then
      b=p
    end
    if n>=last then
      e=p-1
    else
      return p
    end
  end
  local function slide_two(s,p)
    n=n+1
    if n==first then
      b=p
    else
      return true
    end
  end
  local pattern_zero=Cmt(p_utf8char,slide_zero)^0
  local pattern_one=Cmt(p_utf8char,slide_one )^0
  local pattern_two=Cmt(p_utf8char,slide_two )^0
  function utf.sub(str,start,stop)
    if not start then
      return str
    end
    if start==0 then
      start=1
    end
    if not stop then
      if start<0 then
        local l=utflength(str) 
        start=l+start
      else
        start=start-1
      end
      b,n,first=0,0,start
      lpegmatch(pattern_two,str)
      if n>=first then
        return sub(str,b)
      else
        return ""
      end
    end
    if start<0 or stop<0 then
      local l=utf.length(str)
      if start<0 then
        start=l+start
        if start<=0 then
          start=1
        else
          start=start+1
        end
      end
      if stop<0 then
        stop=l+stop
        if stop==0 then
          stop=1
        else
          stop=stop+1
        end
      end
    end
    if start>stop then
      return ""
    elseif start>1 then
      b,e,n,first,last=0,0,0,start-1,stop
      lpegmatch(pattern_one,str)
      if n>=first and e==0 then
        e=#str
      end
      return sub(str,b,e)
    else
      b,e,n,last=1,0,0,stop
      lpegmatch(pattern_zero,str)
      if e==0 then
        e=#str
      end
      return sub(str,b,e)
    end
  end
end
function utf.remapper(mapping)
  local pattern=Cs((p_utf8char/mapping)^0)
  return function(str)
    if not str or str=="" then
      return ""
    else
      return lpegmatch(pattern,str)
    end
  end,pattern
end
function utf.replacer(t) 
  local r=replacer(t,false,false,true)
  return function(str)
    return lpegmatch(r,str)
  end
end
function utf.subtituter(t) 
  local f=finder (t)
  local r=replacer(t,false,false,true)
  return function(str)
    local i=lpegmatch(f,str)
    if not i then
      return str
    elseif i>#str then
      return str
    else
      return lpegmatch(r,str)
    end
  end
end
local utflinesplitter=p_utfbom^-1*lpeg.tsplitat(p_newline)
local utfcharsplitter_ows=p_utfbom^-1*Ct(C(p_utf8char)^0)
local utfcharsplitter_iws=p_utfbom^-1*Ct((p_whitespace^1+C(p_utf8char))^0)
local utfcharsplitter_raw=Ct(C(p_utf8char)^0)
patterns.utflinesplitter=utflinesplitter
function utf.splitlines(str)
  return lpegmatch(utflinesplitter,str or "")
end
function utf.split(str,ignorewhitespace) 
  if ignorewhitespace then
    return lpegmatch(utfcharsplitter_iws,str or "")
  else
    return lpegmatch(utfcharsplitter_ows,str or "")
  end
end
function utf.totable(str) 
  return lpegmatch(utfcharsplitter_raw,str)
end
function utf.magic(f) 
  local str=f:read(4) or ""
  local off=lpegmatch(p_utfoffset,str)
  if off<4 then
    f:seek('set',off)
  end
  return lpegmatch(p_utftype,str)
end
local function utf16_to_utf8_be(t)
  if type(t)=="string" then
    t=lpegmatch(utflinesplitter,t)
  end
  local result={} 
  for i=1,#t do
    local r,more=0,0
    for left,right in bytepairs(t[i]) do
      if right then
        local now=256*left+right
        if more>0 then
          now=(more-0xD800)*0x400+(now-0xDC00)+0x10000 
          more=0
          r=r+1
          result[r]=utfchar(now)
        elseif now>=0xD800 and now<=0xDBFF then
          more=now
        else
          r=r+1
          result[r]=utfchar(now)
        end
      end
    end
    t[i]=concat(result,"",1,r) 
  end
  return t
end
local function utf16_to_utf8_le(t)
  if type(t)=="string" then
    t=lpegmatch(utflinesplitter,t)
  end
  local result={} 
  for i=1,#t do
    local r,more=0,0
    for left,right in bytepairs(t[i]) do
      if right then
        local now=256*right+left
        if more>0 then
          now=(more-0xD800)*0x400+(now-0xDC00)+0x10000 
          more=0
          r=r+1
          result[r]=utfchar(now)
        elseif now>=0xD800 and now<=0xDBFF then
          more=now
        else
          r=r+1
          result[r]=utfchar(now)
        end
      end
    end
    t[i]=concat(result,"",1,r) 
  end
  return t
end
local function utf32_to_utf8_be(t)
  if type(t)=="string" then
    t=lpegmatch(utflinesplitter,t)
  end
  local result={} 
  for i=1,#t do
    local r,more=0,-1
    for a,b in bytepairs(t[i]) do
      if a and b then
        if more<0 then
          more=256*256*256*a+256*256*b
        else
          r=r+1
          result[t]=utfchar(more+256*a+b)
          more=-1
        end
      else
        break
      end
    end
    t[i]=concat(result,"",1,r)
  end
  return t
end
local function utf32_to_utf8_le(t)
  if type(t)=="string" then
    t=lpegmatch(utflinesplitter,t)
  end
  local result={} 
  for i=1,#t do
    local r,more=0,-1
    for a,b in bytepairs(t[i]) do
      if a and b then
        if more<0 then
          more=256*b+a
        else
          r=r+1
          result[t]=utfchar(more+256*256*256*b+256*256*a)
          more=-1
        end
      else
        break
      end
    end
    t[i]=concat(result,"",1,r)
  end
  return t
end
utf.utf32_to_utf8_be=utf32_to_utf8_be
utf.utf32_to_utf8_le=utf32_to_utf8_le
utf.utf16_to_utf8_be=utf16_to_utf8_be
utf.utf16_to_utf8_le=utf16_to_utf8_le
function utf.utf8_to_utf8(t)
  return type(t)=="string" and lpegmatch(utflinesplitter,t) or t
end
function utf.utf16_to_utf8(t,endian)
  return endian and utf16_to_utf8_be(t) or utf16_to_utf8_le(t) or t
end
function utf.utf32_to_utf8(t,endian)
  return endian and utf32_to_utf8_be(t) or utf32_to_utf8_le(t) or t
end
local function little(c)
  local b=byte(c)
  if b<0x10000 then
    return char(b%256,b/256)
  else
    b=b-0x10000
    local b1,b2=b/1024+0xD800,b%1024+0xDC00
    return char(b1%256,b1/256,b2%256,b2/256)
  end
end
local function big(c)
  local b=byte(c)
  if b<0x10000 then
    return char(b/256,b%256)
  else
    b=b-0x10000
    local b1,b2=b/1024+0xD800,b%1024+0xDC00
    return char(b1/256,b1%256,b2/256,b2%256)
  end
end
local _,l_remap=utf.remapper(little)
local _,b_remap=utf.remapper(big)
function utf.utf8_to_utf16(str,littleendian)
  if littleendian then
    return char(255,254)..lpegmatch(l_remap,str)
  else
    return char(254,255)..lpegmatch(b_remap,str)
  end
end
local pattern=Cs (
  (p_utf8byte/function(unicode     ) return format("0x%04X",unicode) end)*(p_utf8byte*Carg(1)/function(unicode,separator) return format("%s0x%04X",separator,unicode) end)^0
)
function utf.tocodes(str,separator)
  return lpegmatch(pattern,str,1,separator or " ")
end
function utf.ustring(s)
  return format("U+%05X",type(s)=="number" and s or utfbyte(s))
end
function utf.xstring(s)
  return format("0x%05X",type(s)=="number" and s or utfbyte(s))
end
local p_nany=p_utf8char/""
if utfgmatch then
  function utf.count(str,what)
    if type(what)=="string" then
      local n=0
      for _ in utfgmatch(str,what) do
        n=n+1
      end
      return n
    else 
      return #lpegmatch(Cs((P(what)/" "+p_nany)^0),str)
    end
  end
else
  local cache={}
  function utf.count(str,what)
    if type(what)=="string" then
      local p=cache[what]
      if not p then
        p=Cs((P(what)/" "+p_nany)^0)
        cache[p]=p
      end
      return #lpegmatch(p,str)
    else 
      return #lpegmatch(Cs((P(what)/" "+p_nany)^0),str)
    end
  end
end
if not utf.characters then
  function utf.characters(str)
    return gmatch(str,".[\128-\191]*")
  end
  string.utfcharacters=utf.characters
end
if not utf.values then
  local find=string.find
  local dummy=function()
  end
  function utf.values(str)
    local n=#str
    if n==0 then
      return dummy
    elseif n==1 then
      return function() return utfbyte(str) end
    else
      local p=1
      return function()
          local b,e=find(str,".[\128-\191]*",p)
          if b then
            p=e+1
            return utfbyte(sub(str,b,e))
          end
      end
    end
  end
  string.utfvalues=utf.values
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-math"] = package.loaded["l-math"] or true

-- original size: 915, stripped down to: 836

if not modules then modules={} end modules ['l-math']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local floor,sin,cos,tan=math.floor,math.sin,math.cos,math.tan
if not math.round then
  function math.round(x) return floor(x+0.5) end
end
if not math.div then
  function math.div(n,m) return floor(n/m) end
end
if not math.mod then
  function math.mod(n,m) return n%m end
end
local pipi=2*math.pi/360
if not math.sind then
  function math.sind(d) return sin(d*pipi) end
  function math.cosd(d) return cos(d*pipi) end
  function math.tand(d) return tan(d*pipi) end
end
if not math.odd then
  function math.odd (n) return n%2~=0 end
  function math.even(n) return n%2==0 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-str"] = package.loaded["util-str"] or true

-- original size: 22834, stripped down to: 12570

if not modules then modules={} end modules ['util-str']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
utilities=utilities or {}
utilities.strings=utilities.strings or {}
local strings=utilities.strings
local format,gsub,rep,sub=string.format,string.gsub,string.rep,string.sub
local load,dump=load,string.dump
local tonumber,type,tostring=tonumber,type,tostring
local unpack,concat=table.unpack,table.concat
local P,V,C,S,R,Ct,Cs,Cp,Carg,Cc=lpeg.P,lpeg.V,lpeg.C,lpeg.S,lpeg.R,lpeg.Ct,lpeg.Cs,lpeg.Cp,lpeg.Carg,lpeg.Cc
local patterns,lpegmatch=lpeg.patterns,lpeg.match
local utfchar,utfbyte=utf.char,utf.byte
local loadstripped=_LUAVERSION<5.2 and load or function(str)
  return load(dump(load(str),true)) 
end
if not number then number={} end 
local stripper=patterns.stripzeros
local function points(n)
  return (not n or n==0) and "0pt" or lpegmatch(stripper,format("%.5fpt",n/65536))
end
local function basepoints(n)
  return (not n or n==0) and "0bp" or lpegmatch(stripper,format("%.5fbp",n*(7200/7227)/65536))
end
number.points=points
number.basepoints=basepoints
local rubish=patterns.spaceortab^0*patterns.newline
local anyrubish=patterns.spaceortab+patterns.newline
local anything=patterns.anything
local stripped=(patterns.spaceortab^1/"")*patterns.newline
local leading=rubish^0/""
local trailing=(anyrubish^1*patterns.endofstring)/""
local redundant=rubish^3/"\n"
local pattern=Cs(leading*(trailing+redundant+stripped+anything)^0)
function strings.collapsecrlf(str)
  return lpegmatch(pattern,str)
end
local repeaters={} 
function strings.newrepeater(str,offset)
  offset=offset or 0
  local s=repeaters[str]
  if not s then
    s={}
    repeaters[str]=s
  end
  local t=s[offset]
  if t then
    return t
  end
  t={}
  setmetatable(t,{ __index=function(t,k)
    if not k then
      return ""
    end
    local n=k+offset
    local s=n>0 and rep(str,n) or ""
    t[k]=s
    return s
  end })
  s[offset]=t
  return t
end
local extra,tab,start=0,0,4,0
local nspaces=strings.newrepeater(" ")
string.nspaces=nspaces
local pattern=Carg(1)/function(t)
    extra,tab,start=0,t or 7,1
  end*Cs((
   Cp()*patterns.tab/function(position)
     local current=(position-start+1)+extra
     local spaces=tab-(current-1)%tab
     if spaces>0 then
       extra=extra+spaces-1
       return nspaces[spaces] 
     else
       return ""
     end
   end+patterns.newline*Cp()/function(position)
     extra,start=0,position
   end+patterns.anything
 )^1)
function strings.tabtospace(str,tab)
  return lpegmatch(pattern,str,1,tab or 7)
end
function strings.striplong(str) 
  str=gsub(str,"^%s*","")
  str=gsub(str,"[\n\r]+ *","\n")
  return str
end
function strings.nice(str)
  str=gsub(str,"[:%-+_]+"," ") 
  return str
end
local n=0
local sequenced=table.sequenced
function string.autodouble(s,sep)
  if s==nil then
    return '""'
  end
  local t=type(s)
  if t=="number" then
    return tostring(s) 
  end
  if t=="table" then
    return ('"'..sequenced(s,sep or ",")..'"')
  end
  return ('"'..tostring(s)..'"')
end
function string.autosingle(s,sep)
  if s==nil then
    return "''"
  end
  local t=type(s)
  if t=="number" then
    return tostring(s) 
  end
  if t=="table" then
    return ("'"..sequenced(s,sep or ",").."'")
  end
  return ("'"..tostring(s).."'")
end
local tracedchars={}
string.tracedchars=tracedchars
strings.tracers=tracedchars
function string.tracedchar(b)
  if type(b)=="number" then
    return tracedchars[b] or (utfchar(b).." (U+"..format('%05X',b)..")")
  else
    local c=utfbyte(b)
    return tracedchars[c] or (b.." (U+"..format('%05X',c)..")")
  end
end
function number.signed(i)
  if i>0 then
    return "+",i
  else
    return "-",-i
  end
end
local preamble=[[
local type = type
local tostring = tostring
local tonumber = tonumber
local format = string.format
local concat = table.concat
local signed = number.signed
local points = number.points
local basepoints = number.basepoints
local utfchar = utf.char
local utfbyte = utf.byte
local lpegmatch = lpeg.match
local nspaces = string.nspaces
local tracedchar = string.tracedchar
local autosingle = string.autosingle
local autodouble = string.autodouble
local sequenced = table.sequenced
]]
local template=[[
%s
%s
return function(%s) return %s end
]]
local arguments={ "a1" } 
setmetatable(arguments,{ __index=function(t,k)
    local v=t[k-1]..",a"..k
    t[k]=v
    return v
  end
})
local prefix_any=C((S("+- .")+R("09"))^0)
local prefix_tab=C((1-R("az","AZ","09","%%"))^0)
local format_s=function(f)
  n=n+1
  if f and f~="" then
    return format("format('%%%ss',a%s)",f,n)
  else 
    return format("(a%s or '')",n) 
  end
end
local format_S=function(f) 
  n=n+1
  if f and f~="" then
    return format("format('%%%ss',tostring(a%s))",f,n)
  else
    return format("tostring(a%s)",n)
  end
end
local format_q=function()
  n=n+1
  return format("(a%s and format('%%q',a%s) or '')",n,n) 
end
local format_Q=function() 
  n=n+1
  return format("format('%%q',tostring(a%s))",n)
end
local format_i=function(f)
  n=n+1
  if f and f~="" then
    return format("format('%%%si',a%s)",f,n)
  else
    return format("a%s",n)
  end
end
local format_d=format_i
local format_I=function(f)
  n=n+1
  return format("format('%%s%%%si',signed(a%s))",f,n)
end
local format_f=function(f)
  n=n+1
  return format("format('%%%sf',a%s)",f,n)
end
local format_g=function(f)
  n=n+1
  return format("format('%%%sg',a%s)",f,n)
end
local format_G=function(f)
  n=n+1
  return format("format('%%%sG',a%s)",f,n)
end
local format_e=function(f)
  n=n+1
  return format("format('%%%se',a%s)",f,n)
end
local format_E=function(f)
  n=n+1
  return format("format('%%%sE',a%s)",f,n)
end
local format_x=function(f)
  n=n+1
  return format("format('%%%sx',a%s)",f,n)
end
local format_X=function(f)
  n=n+1
  return format("format('%%%sX',a%s)",f,n)
end
local format_o=function(f)
  n=n+1
  return format("format('%%%so',a%s)",f,n)
end
local format_c=function()
  n=n+1
  return format("utfchar(a%s)",n)
end
local format_C=function()
  n=n+1
  return format("tracedchar(a%s)",n)
end
local format_r=function(f)
  n=n+1
  return format("format('%%%s.0f',a%s)",f,n)
end
local format_h=function(f)
  n=n+1
  if f=="-" then
    f=sub(f,2)
    return format("format('%%%sx',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  else
    return format("format('0x%%%sx',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  end
end
local format_H=function(f)
  n=n+1
  if f=="-" then
    f=sub(f,2)
    return format("format('%%%sX',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  else
    return format("format('0x%%%sX',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  end
end
local format_u=function(f)
  n=n+1
  if f=="-" then
    f=sub(f,2)
    return format("format('%%%sx',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  else
    return format("format('u+%%%sx',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  end
end
local format_U=function(f)
  n=n+1
  if f=="-" then
    f=sub(f,2)
    return format("format('%%%sX',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  else
    return format("format('U+%%%sX',type(a%s) == 'number' and a%s or utfbyte(a%s))",f=="" and "05" or f,n,n,n)
  end
end
local format_p=function()
  n=n+1
  return format("points(a%s)",n)
end
local format_b=function()
  n=n+1
  return format("basepoints(a%s)",n)
end
local format_t=function(f)
  n=n+1
  if f and f~="" then
    return format("concat(a%s,%q)",n,f)
  else
    return format("concat(a%s)",n)
  end
end
local format_T=function(f)
  n=n+1
  if f and f~="" then
    return format("sequenced(a%s,%q)",n,f)
  else
    return format("sequenced(a%s)",n)
  end
end
local format_l=function()
  n=n+1
  return format("(a%s and 'true' or 'false')",n)
end
local format_L=function()
  n=n+1
  return format("(a%s and 'TRUE' or 'FALSE')",n)
end
local format_N=function() 
  n=n+1
  return format("tostring(tonumber(a%s) or a%s)",n,n)
end
local format_a=function(f)
  n=n+1
  if f and f~="" then
    return format("autosingle(a%s,%q)",n,f)
  else
    return format("autosingle(a%s)",n)
  end
end
local format_A=function(f)
  n=n+1
  if f and f~="" then
    return format("autodouble(a%s,%q)",n,f)
  else
    return format("autodouble(a%s)",n)
  end
end
local format_w=function(f) 
  n=n+1
  f=tonumber(f)
  if f then 
    return format("nspaces[%s+a%s]",f,n) 
  else
    return format("nspaces[a%s]",n) 
  end
end
local format_W=function(f) 
  return format("nspaces[%s]",tonumber(f) or 0)
end
local format_rest=function(s)
  return format("%q",s) 
end
local format_extension=function(extensions,f,name)
  local extension=extensions[name] or "tostring(%s)"
  local f=tonumber(f) or 1
  if f==0 then
    return extension
  elseif f==1 then
    n=n+1
    local a="a"..n
    return format(extension,a,a) 
  elseif f<0 then
    local a="a"..(n+f+1)
    return format(extension,a,a)
  else
    local t={}
    for i=1,f do
      n=n+1
      t[#t+1]="a"..n
    end
    return format(extension,unpack(t))
  end
end
local builder=Cs { "start",
  start=(
    (
      P("%")/""*(
        V("!") 
+V("s")+V("q")+V("i")+V("d")+V("f")+V("g")+V("G")+V("e")+V("E")+V("x")+V("X")+V("o")
+V("c")+V("C")+V("S") 
+V("Q") 
+V("N")
+V("r")+V("h")+V("H")+V("u")+V("U")+V("p")+V("b")+V("t")+V("T")+V("l")+V("L")+V("I")+V("h") 
+V("w") 
+V("W") 
+V("a") 
+V("A")
+V("*") 
      )+V("*")
    )*(P(-1)+Carg(1))
  )^0,
  ["s"]=(prefix_any*P("s"))/format_s,
  ["q"]=(prefix_any*P("q"))/format_q,
  ["i"]=(prefix_any*P("i"))/format_i,
  ["d"]=(prefix_any*P("d"))/format_d,
  ["f"]=(prefix_any*P("f"))/format_f,
  ["g"]=(prefix_any*P("g"))/format_g,
  ["G"]=(prefix_any*P("G"))/format_G,
  ["e"]=(prefix_any*P("e"))/format_e,
  ["E"]=(prefix_any*P("E"))/format_E,
  ["x"]=(prefix_any*P("x"))/format_x,
  ["X"]=(prefix_any*P("X"))/format_X,
  ["o"]=(prefix_any*P("o"))/format_o,
  ["S"]=(prefix_any*P("S"))/format_S,
  ["Q"]=(prefix_any*P("Q"))/format_S,
  ["N"]=(prefix_any*P("N"))/format_N,
  ["c"]=(prefix_any*P("c"))/format_c,
  ["C"]=(prefix_any*P("C"))/format_C,
  ["r"]=(prefix_any*P("r"))/format_r,
  ["h"]=(prefix_any*P("h"))/format_h,
  ["H"]=(prefix_any*P("H"))/format_H,
  ["u"]=(prefix_any*P("u"))/format_u,
  ["U"]=(prefix_any*P("U"))/format_U,
  ["p"]=(prefix_any*P("p"))/format_p,
  ["b"]=(prefix_any*P("b"))/format_b,
  ["t"]=(prefix_tab*P("t"))/format_t,
  ["T"]=(prefix_tab*P("T"))/format_T,
  ["l"]=(prefix_tab*P("l"))/format_l,
  ["L"]=(prefix_tab*P("L"))/format_L,
  ["I"]=(prefix_any*P("I"))/format_I,
  ["w"]=(prefix_any*P("w"))/format_w,
  ["W"]=(prefix_any*P("W"))/format_W,
  ["a"]=(prefix_any*P("a"))/format_a,
  ["A"]=(prefix_any*P("A"))/format_A,
  ["*"]=Cs(((1-P("%"))^1+P("%%")/"%%%%")^1)/format_rest,
  ["!"]=Carg(2)*prefix_any*P("!")*C((1-P("!"))^1)*P("!")/format_extension,
}
local direct=Cs (
    P("%")/""*Cc([[local format = string.format return function(str) return format("%]])*(S("+- .")+R("09"))^0*S("sqidfgGeExXo")*Cc([[",str) end]])*P(-1)
  )
local function make(t,str)
  local f
  local p
  local p=lpegmatch(direct,str)
  if p then
    f=loadstripped(p)()
  else
    n=0
    p=lpegmatch(builder,str,1,"..",t._extensions_) 
    if n>0 then
      p=format(template,preamble,t._preamble_,arguments[n],p)
      f=loadstripped(p)()
    else
      f=function() return str end
    end
  end
  t[str]=f
  return f
end
local function use(t,fmt,...)
  return t[fmt](...)
end
strings.formatters={}
function strings.formatters.new()
  local t={ _extensions_={},_preamble_="",_type_="formatter" }
  setmetatable(t,{ __index=make,__call=use })
  return t
end
local formatters=strings.formatters.new() 
string.formatters=formatters 
string.formatter=function(str,...) return formatters[str](...) end 
local function add(t,name,template,preamble)
  if type(t)=="table" and t._type_=="formatter" then
    t._extensions_[name]=template or "%s"
    if preamble then
      t._preamble_=preamble.."\n"..t._preamble_ 
    end
  end
end
strings.formatters.add=add
lpeg.patterns.xmlescape=Cs((P("<")/"&lt;"+P(">")/"&gt;"+P("&")/"&amp;"+P('"')/"&quot;"+P(1))^0)
lpeg.patterns.texescape=Cs((C(S("#$%\\{}"))/"\\%1"+P(1))^0)
add(formatters,"xml",[[lpegmatch(xmlescape,%s)]],[[local xmlescape = lpeg.patterns.xmlescape]])
add(formatters,"tex",[[lpegmatch(texescape,%s)]],[[local texescape = lpeg.patterns.texescape]])


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-tab"] = package.loaded["util-tab"] or true

-- original size: 14491, stripped down to: 8512

if not modules then modules={} end modules ['util-tab']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
utilities=utilities or {}
utilities.tables=utilities.tables or {}
local tables=utilities.tables
local format,gmatch,gsub=string.format,string.gmatch,string.gsub
local concat,insert,remove=table.concat,table.insert,table.remove
local setmetatable,getmetatable,tonumber,tostring=setmetatable,getmetatable,tonumber,tostring
local type,next,rawset,tonumber,tostring,load,select=type,next,rawset,tonumber,tostring,load,select
local lpegmatch,P,Cs,Cc=lpeg.match,lpeg.P,lpeg.Cs,lpeg.Cc
local serialize,sortedkeys,sortedpairs=table.serialize,table.sortedkeys,table.sortedpairs
local formatters=string.formatters
local splitter=lpeg.tsplitat(".")
function tables.definetable(target,nofirst,nolast) 
  local composed,shortcut,t=nil,nil,{}
  local snippets=lpegmatch(splitter,target)
  for i=1,#snippets-(nolast and 1 or 0) do
    local name=snippets[i]
    if composed then
      composed=shortcut.."."..name
      shortcut=shortcut.."_"..name
      t[#t+1]=formatters["local %s = %s if not %s then %s = { } %s = %s end"](shortcut,composed,shortcut,shortcut,composed,shortcut)
    else
      composed=name
      shortcut=name
      if not nofirst then
        t[#t+1]=formatters["%s = %s or { }"](composed,composed)
      end
    end
  end
  if nolast then
    composed=shortcut.."."..snippets[#snippets]
  end
  return concat(t,"\n"),composed
end
function tables.definedtable(...)
  local t=_G
  for i=1,select("#",...) do
    local li=select(i,...)
    local tl=t[li]
    if not tl then
      tl={}
      t[li]=tl
    end
    t=tl
  end
  return t
end
function tables.accesstable(target,root)
  local t=root or _G
  for name in gmatch(target,"([^%.]+)") do
    t=t[name]
    if not t then
      return
    end
  end
  return t
end
function tables.migratetable(target,v,root)
  local t=root or _G
  local names=string.split(target,".")
  for i=1,#names-1 do
    local name=names[i]
    t[name]=t[name] or {}
    t=t[name]
    if not t then
      return
    end
  end
  t[names[#names]]=v
end
function tables.removevalue(t,value) 
  if value then
    for i=1,#t do
      if t[i]==value then
        remove(t,i)
      end
    end
  end
end
function tables.insertbeforevalue(t,value,extra)
  for i=1,#t do
    if t[i]==extra then
      remove(t,i)
    end
  end
  for i=1,#t do
    if t[i]==value then
      insert(t,i,extra)
      return
    end
  end
  insert(t,1,extra)
end
function tables.insertaftervalue(t,value,extra)
  for i=1,#t do
    if t[i]==extra then
      remove(t,i)
    end
  end
  for i=1,#t do
    if t[i]==value then
      insert(t,i+1,extra)
      return
    end
  end
  insert(t,#t+1,extra)
end
local escape=Cs(Cc('"')*((P('"')/'""'+P(1))^0)*Cc('"'))
function table.tocsv(t,specification)
  if t and #t>0 then
    local result={}
    local r={}
    specification=specification or {}
    local fields=specification.fields
    if type(fields)~="string" then
      fields=sortedkeys(t[1])
    end
    local separator=specification.separator or ","
    if specification.preamble==true then
      for f=1,#fields do
        r[f]=lpegmatch(escape,tostring(fields[f]))
      end
      result[1]=concat(r,separator)
    end
    for i=1,#t do
      local ti=t[i]
      for f=1,#fields do
        local field=ti[fields[f]]
        if type(field)=="string" then
          r[f]=lpegmatch(escape,field)
        else
          r[f]=tostring(field)
        end
      end
      result[#result+1]=concat(r,separator)
    end
    return concat(result,"\n")
  else
    return ""
  end
end
local nspaces=utilities.strings.newrepeater(" ")
local function toxml(t,d,result,step)
  for k,v in sortedpairs(t) do
    local s=nspaces[d] 
    local tk=type(k)
    local tv=type(v)
    if tv=="table" then
      if tk=="number" then
        result[#result+1]=formatters["%s<entry n='%s'>"](s,k)
        toxml(v,d+step,result,step)
        result[#result+1]=formatters["%s</entry>"](s,k)
      else
        result[#result+1]=formatters["%s<%s>"](s,k)
        toxml(v,d+step,result,step)
        result[#result+1]=formatters["%s</%s>"](s,k)
      end
    elseif tv=="string" then
      if tk=="number" then
        result[#result+1]=formatters["%s<entry n='%s'>%!xml!</entry>"](s,k,v,k)
      else
        result[#result+1]=formatters["%s<%s>%!xml!</%s>"](s,k,v,k)
      end
    elseif tk=="number" then
      result[#result+1]=formatters["%s<entry n='%s'>%S</entry>"](s,k,v,k)
    else
      result[#result+1]=formatters["%s<%s>%S</%s>"](s,k,v,k)
    end
  end
end
function table.toxml(t,specification)
  specification=specification or {}
  local name=specification.name
  local noroot=name==false
  local result=(specification.nobanner or noroot) and {} or { "<?xml version='1.0' standalone='yes' ?>" }
  local indent=specification.indent or 0
  local spaces=specification.spaces or 1
  if noroot then
    toxml(t,indent,result,spaces)
  else
    toxml({ [name or "data"]=t },indent,result,spaces)
  end
  return concat(result,"\n")
end
function tables.encapsulate(core,capsule,protect)
  if type(capsule)~="table" then
    protect=true
    capsule={}
  end
  for key,value in next,core do
    if capsule[key] then
      print(formatters["\ninvalid %s %a in %a"]("inheritance",key,core))
      os.exit()
    else
      capsule[key]=value
    end
  end
  if protect then
    for key,value in next,core do
      core[key]=nil
    end
    setmetatable(core,{
      __index=capsule,
      __newindex=function(t,key,value)
        if capsule[key] then
          print(formatters["\ninvalid %s %a' in %a"]("overload",key,core))
          os.exit()
        else
          rawset(t,key,value)
        end
      end
    } )
  end
end
local function fastserialize(t,r,outer) 
  r[#r+1]="{"
  local n=#t
  if n>0 then
    for i=1,n do
      local v=t[i]
      local tv=type(v)
      if tv=="string" then
        r[#r+1]=formatters["%q,"](v)
      elseif tv=="number" then
        r[#r+1]=formatters["%s,"](v)
      elseif tv=="table" then
        fastserialize(v,r)
      elseif tv=="boolean" then
        r[#r+1]=formatters["%S,"](v)
      end
    end
  else
    for k,v in next,t do
      local tv=type(v)
      if tv=="string" then
        r[#r+1]=formatters["[%q]=%q,"](k,v)
      elseif tv=="number" then
        r[#r+1]=formatters["[%q]=%s,"](k,v)
      elseif tv=="table" then
        r[#r+1]=formatters["[%q]="](k)
        fastserialize(v,r)
      elseif tv=="boolean" then
        r[#r+1]=formatters["[%q]=%S,"](k,v)
      end
    end
  end
  if outer then
    r[#r+1]="}"
  else
    r[#r+1]="},"
  end
  return r
end
function table.fastserialize(t,prefix) 
  return concat(fastserialize(t,{ prefix or "return" },true))
end
function table.deserialize(str)
  if not str or str=="" then
    return
  end
  local code=load(str)
  if not code then
    return
  end
  code=code()
  if not code then
    return
  end
  return code
end
function table.load(filename)
  if filename then
    local t=io.loaddata(filename)
    if t and t~="" then
      t=load(t)
      if type(t)=="function" then
        t=t()
        if type(t)=="table" then
          return t
        end
      end
    end
  end
end
function table.save(filename,t,n,...)
  io.savedata(filename,serialize(t,n==nil and true or n,...))
end
local function slowdrop(t)
  local r={}
  local l={}
  for i=1,#t do
    local ti=t[i]
    local j=0
    for k,v in next,ti do
      j=j+1
      l[j]=formatters["%s=%q"](k,v)
    end
    r[i]=formatters[" {%t},\n"](l)
  end
  return formatters["return {\n%st}"](r)
end
local function fastdrop(t)
  local r={ "return {\n" }
  for i=1,#t do
    local ti=t[i]
    r[#r+1]=" {"
    for k,v in next,ti do
      r[#r+1]=formatters["%s=%q"](k,v)
    end
    r[#r+1]="},\n"
  end
  r[#r+1]="}"
  return concat(r)
end
function table.drop(t,slow) 
  if #t==0 then
    return "return { }"
  elseif slow==true then
    return slowdrop(t) 
  else
    return fastdrop(t) 
  end
end
function table.autokey(t,k)
  local v={}
  t[k]=v
  return v
end
local selfmapper={ __index=function(t,k) t[k]=k return k end }
function table.twowaymapper(t)
  if not t then
    t={}
  else
    for i=0,#t do
      local ti=t[i]    
      if ti then
        local i=tostring(i)
        t[i]=ti   
        t[ti]=i    
      end
    end
    t[""]=t[0] or ""
  end
  setmetatable(t,selfmapper)
  return t
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-sto"] = package.loaded["util-sto"] or true

-- original size: 4432, stripped down to: 3123

if not modules then modules={} end modules ['util-sto']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local setmetatable,getmetatable,type=setmetatable,getmetatable,type
utilities=utilities or {}
utilities.storage=utilities.storage or {}
local storage=utilities.storage
function storage.mark(t)
  if not t then
    print("\nfatal error: storage cannot be marked\n")
    os.exit()
    return
  end
  local m=getmetatable(t)
  if not m then
    m={}
    setmetatable(t,m)
  end
  m.__storage__=true
  return t
end
function storage.allocate(t)
  t=t or {}
  local m=getmetatable(t)
  if not m then
    m={}
    setmetatable(t,m)
  end
  m.__storage__=true
  return t
end
function storage.marked(t)
  local m=getmetatable(t)
  return m and m.__storage__
end
function storage.checked(t)
  if not t then
    report("\nfatal error: storage has not been allocated\n")
    os.exit()
    return
  end
  return t
end
function storage.setinitializer(data,initialize)
  local m=getmetatable(data) or {}
  m.__index=function(data,k)
    m.__index=nil 
    initialize()
    return data[k]
  end
  setmetatable(data,m)
end
local keyisvalue={ __index=function(t,k)
  t[k]=k
  return k
end }
function storage.sparse(t)
  t=t or {}
  setmetatable(t,keyisvalue)
  return t
end
local function f_empty ()              return "" end 
local function f_self (t,k) t[k]=k        return k end
local function f_table (t,k) local v={} t[k]=v return v end
local function f_ignore()                   end 
local t_empty={ __index=f_empty }
local t_self={ __index=f_self  }
local t_table={ __index=f_table }
local t_ignore={ __newindex=f_ignore }
function table.setmetatableindex(t,f)
  if type(t)~="table" then
    f,t=t,{}
  end
  local m=getmetatable(t)
  if m then
    if f=="empty" then
      m.__index=f_empty
    elseif f=="key" then
      m.__index=f_self
    elseif f=="table" then
      m.__index=f_table
    else
      m.__index=f
    end
  else
    if f=="empty" then
      setmetatable(t,t_empty)
    elseif f=="key" then
      setmetatable(t,t_self)
    elseif f=="table" then
      setmetatable(t,t_table)
    else
      setmetatable(t,{ __index=f })
    end
  end
  return t
end
function table.setmetatablenewindex(t,f)
  if type(t)~="table" then
    f,t=t,{}
  end
  local m=getmetatable(t)
  if m then
    if f=="ignore" then
      m.__newindex=f_ignore
    else
      m.__newindex=f
    end
  else
    if f=="ignore" then
      setmetatable(t,t_ignore)
    else
      setmetatable(t,{ __newindex=f })
    end
  end
  return t
end
function table.setmetatablecall(t,f)
  if type(t)~="table" then
    f,t=t,{}
  end
  local m=getmetatable(t)
  if m then
    m.__call=f
  else
    setmetatable(t,{ __call=f })
  end
  return t
end
function table.setmetatablekey(t,key,value)
  local m=getmetatable(t)
  if not m then
    m={}
    setmetatable(t,m)
  end
  m[key]=value
  return t
end
function table.getmetatablekey(t,key,value)
  local m=getmetatable(t)
  return m and m[key]
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-prs"] = package.loaded["util-prs"] or true

-- original size: 16976, stripped down to: 12143

if not modules then modules={} end modules ['util-prs']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local lpeg,table,string=lpeg,table,string
local P,R,V,S,C,Ct,Cs,Carg,Cc,Cg,Cf,Cp=lpeg.P,lpeg.R,lpeg.V,lpeg.S,lpeg.C,lpeg.Ct,lpeg.Cs,lpeg.Carg,lpeg.Cc,lpeg.Cg,lpeg.Cf,lpeg.Cp
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local concat,format,gmatch,find=table.concat,string.format,string.gmatch,string.find
local tostring,type,next,rawset=tostring,type,next,rawset
utilities=utilities or {}
local parsers=utilities.parsers or {}
utilities.parsers=parsers
local patterns=parsers.patterns or {}
parsers.patterns=patterns
local setmetatableindex=table.setmetatableindex
local sortedhash=table.sortedhash
local digit=R("09")
local space=P(' ')
local equal=P("=")
local comma=P(",")
local lbrace=P("{")
local rbrace=P("}")
local lparent=P("(")
local rparent=P(")")
local period=S(".")
local punctuation=S(".,:;")
local spacer=lpegpatterns.spacer
local whitespace=lpegpatterns.whitespace
local newline=lpegpatterns.newline
local anything=lpegpatterns.anything
local endofstring=lpegpatterns.endofstring
local nobrace=1-(lbrace+rbrace )
local noparent=1-(lparent+rparent)
local escape,left,right=P("\\"),P('{'),P('}')
lpegpatterns.balanced=P {
  [1]=((escape*(left+right))+(1-(left+right))+V(2))^0,
  [2]=left*V(1)*right
}
local nestedbraces=P { lbrace*(nobrace+V(1))^0*rbrace }
local nestedparents=P { lparent*(noparent+V(1))^0*rparent }
local spaces=space^0
local argument=Cs((lbrace/"")*((nobrace+nestedbraces)^0)*(rbrace/""))
local content=(1-endofstring)^0
lpegpatterns.nestedbraces=nestedbraces 
lpegpatterns.nestedparents=nestedparents 
lpegpatterns.nested=nestedbraces 
lpegpatterns.argument=argument   
lpegpatterns.content=content    
local value=P(lbrace*C((nobrace+nestedbraces)^0)*rbrace)+C((nestedbraces+(1-comma))^0)
local key=C((1-equal-comma)^1)
local pattern_a=(space+comma)^0*(key*equal*value+key*C(""))
local pattern_c=(space+comma)^0*(key*equal*value)
local key=C((1-space-equal-comma)^1)
local pattern_b=spaces*comma^0*spaces*(key*((spaces*equal*spaces*value)+C("")))
local hash={}
local function set(key,value)
  hash[key]=value
end
local pattern_a_s=(pattern_a/set)^1
local pattern_b_s=(pattern_b/set)^1
local pattern_c_s=(pattern_c/set)^1
patterns.settings_to_hash_a=pattern_a_s
patterns.settings_to_hash_b=pattern_b_s
patterns.settings_to_hash_c=pattern_c_s
function parsers.make_settings_to_hash_pattern(set,how)
  if how=="strict" then
    return (pattern_c/set)^1
  elseif how=="tolerant" then
    return (pattern_b/set)^1
  else
    return (pattern_a/set)^1
  end
end
function parsers.settings_to_hash(str,existing)
  if str and str~="" then
    hash=existing or {}
    lpegmatch(pattern_a_s,str)
    return hash
  else
    return {}
  end
end
function parsers.settings_to_hash_tolerant(str,existing)
  if str and str~="" then
    hash=existing or {}
    lpegmatch(pattern_b_s,str)
    return hash
  else
    return {}
  end
end
function parsers.settings_to_hash_strict(str,existing)
  if str and str~="" then
    hash=existing or {}
    lpegmatch(pattern_c_s,str)
    return next(hash) and hash
  else
    return nil
  end
end
local separator=comma*space^0
local value=P(lbrace*C((nobrace+nestedbraces)^0)*rbrace)+C((nestedbraces+(1-comma))^0)
local pattern=spaces*Ct(value*(separator*value)^0)
patterns.settings_to_array=pattern
function parsers.settings_to_array(str,strict)
  if not str or str=="" then
    return {}
  elseif strict then
    if find(str,"{") then
      return lpegmatch(pattern,str)
    else
      return { str }
    end
  else
    return lpegmatch(pattern,str)
  end
end
local function set(t,v)
  t[#t+1]=v
end
local value=P(Carg(1)*value)/set
local pattern=value*(separator*value)^0*Carg(1)
function parsers.add_settings_to_array(t,str)
  return lpegmatch(pattern,str,nil,t)
end
function parsers.hash_to_string(h,separator,yes,no,strict,omit)
  if h then
    local t,tn,s={},0,table.sortedkeys(h)
    omit=omit and table.tohash(omit)
    for i=1,#s do
      local key=s[i]
      if not omit or not omit[key] then
        local value=h[key]
        if type(value)=="boolean" then
          if yes and no then
            if value then
              tn=tn+1
              t[tn]=key..'='..yes
            elseif not strict then
              tn=tn+1
              t[tn]=key..'='..no
            end
          elseif value or not strict then
            tn=tn+1
            t[tn]=key..'='..tostring(value)
          end
        else
          tn=tn+1
          t[tn]=key..'='..value
        end
      end
    end
    return concat(t,separator or ",")
  else
    return ""
  end
end
function parsers.array_to_string(a,separator)
  if a then
    return concat(a,separator or ",")
  else
    return ""
  end
end
function parsers.settings_to_set(str,t) 
  t=t or {}
  for s in gmatch(str,"[^, ]+") do 
    t[s]=true
  end
  return t
end
function parsers.simple_hash_to_string(h,separator)
  local t,tn={},0
  for k,v in sortedhash(h) do
    if v then
      tn=tn+1
      t[tn]=k
    end
  end
  return concat(t,separator or ",")
end
local value=P(lbrace*C((nobrace+nestedbraces)^0)*rbrace)+C(digit^1*lparent*(noparent+nestedparents)^1*rparent)+C((nestedbraces+(1-comma))^1)
local pattern_a=spaces*Ct(value*(separator*value)^0)
local function repeater(n,str)
  if not n then
    return str
  else
    local s=lpegmatch(pattern_a,str)
    if n==1 then
      return unpack(s)
    else
      local t,tn={},0
      for i=1,n do
        for j=1,#s do
          tn=tn+1
          t[tn]=s[j]
        end
      end
      return unpack(t)
    end
  end
end
local value=P(lbrace*C((nobrace+nestedbraces)^0)*rbrace)+(C(digit^1)/tonumber*lparent*Cs((noparent+nestedparents)^1)*rparent)/repeater+C((nestedbraces+(1-comma))^1)
local pattern_b=spaces*Ct(value*(separator*value)^0)
function parsers.settings_to_array_with_repeat(str,expand) 
  if expand then
    return lpegmatch(pattern_b,str) or {}
  else
    return lpegmatch(pattern_a,str) or {}
  end
end
local value=lbrace*C((nobrace+nestedbraces)^0)*rbrace
local pattern=Ct((space+value)^0)
function parsers.arguments_to_table(str)
  return lpegmatch(pattern,str)
end
function parsers.getparameters(self,class,parentclass,settings)
  local sc=self[class]
  if not sc then
    sc={}
    self[class]=sc
    if parentclass then
      local sp=self[parentclass]
      if not sp then
        sp={}
        self[parentclass]=sp
      end
      setmetatableindex(sc,sp)
    end
  end
  parsers.settings_to_hash(settings,sc)
end
function parsers.listitem(str)
  return gmatch(str,"[^, ]+")
end
local pattern=Cs { "start",
  start=V("one")+V("two")+V("three"),
  rest=(Cc(",")*V("thousand"))^0*(P(".")+endofstring)*anything^0,
  thousand=digit*digit*digit,
  one=digit*V("rest"),
  two=digit*digit*V("rest"),
  three=V("thousand")*V("rest"),
}
lpegpatterns.splitthousands=pattern 
function parsers.splitthousands(str)
  return lpegmatch(pattern,str) or str
end
local optionalwhitespace=whitespace^0
lpegpatterns.words=Ct((Cs((1-punctuation-whitespace)^1)+anything)^1)
lpegpatterns.sentences=Ct((optionalwhitespace*Cs((1-period)^0*period))^1)
lpegpatterns.paragraphs=Ct((optionalwhitespace*Cs((whitespace^1*endofstring/""+1-(spacer^0*newline*newline))^1))^1)
local dquote=P('"')
local equal=P('=')
local escape=P('\\')
local separator=S(' ,')
local key=C((1-equal)^1)
local value=dquote*C((1-dquote-escape*dquote)^0)*dquote
local pattern=Cf(Ct("")*Cg(key*equal*value)*separator^0,rawset)^0*P(-1)
patterns.keq_to_hash_c=pattern
function parsers.keq_to_hash(str)
  if str and str~="" then
    return lpegmatch(pattern,str)
  else
    return {}
  end
end
local defaultspecification={ separator=",",quote='"' }
function parsers.csvsplitter(specification)
  specification=specification and table.setmetatableindex(specification,defaultspecification) or defaultspecification
  local separator=specification.separator
  local quotechar=specification.quote
  local separator=S(separator~="" and separator or ",")
  local whatever=C((1-separator-newline)^0)
  if quotechar and quotechar~="" then
    local quotedata=nil
    for chr in gmatch(quotechar,".") do
      local quotechar=P(chr)
      local quoteword=quotechar*C((1-quotechar)^0)*quotechar
      if quotedata then
        quotedata=quotedata+quoteword
      else
        quotedata=quoteword
      end
    end
    whatever=quotedata+whatever
  end
  local parser=Ct((Ct(whatever*(separator*whatever)^0)*S("\n\r"))^0 )
  return function(data)
    return lpegmatch(parser,data)
  end
end
function parsers.rfc4180splitter(specification)
  specification=specification and table.setmetatableindex(specification,defaultspecification) or defaultspecification
  local separator=specification.separator 
  local quotechar=P(specification.quote) 
  local dquotechar=quotechar*quotechar  
/specification.quote
  local separator=S(separator~="" and separator or ",")
  local escaped=quotechar*Cs((dquotechar+(1-quotechar))^0)*quotechar
  local non_escaped=C((1-quotechar-newline-separator)^1)
  local field=escaped+non_escaped
  local record=Ct((field*separator^-1)^1)
  local headerline=record*Cp()
  local wholeblob=Ct((newline^-1*record)^0)
  return function(data,getheader)
    if getheader then
      local header,position=lpegmatch(headerline,data)
      local data=lpegmatch(wholeblob,data,position)
      return data,header
    else
      return lpegmatch(wholeblob,data)
    end
  end
end
local function ranger(first,last,n,action)
  if not first then
  elseif last==true then
    for i=first,n or first do
      action(i)
    end
  elseif last then
    for i=first,last do
      action(i)
    end
  else
    action(first)
  end
end
local cardinal=lpegpatterns.cardinal/tonumber
local spacers=lpegpatterns.spacer^0
local endofstring=lpegpatterns.endofstring
local stepper=spacers*(C(cardinal)*(spacers*S(":-")*spacers*(C(cardinal)+Cc(true) )+Cc(false) )*Carg(1)*Carg(2)/ranger*S(", ")^0 )^1
local stepper=spacers*(C(cardinal)*(spacers*S(":-")*spacers*(C(cardinal)+(P("*")+endofstring)*Cc(true) )+Cc(false) )*Carg(1)*Carg(2)/ranger*S(", ")^0 )^1*endofstring 
function parsers.stepper(str,n,action)
  if type(n)=="function" then
    lpegmatch(stepper,str,1,false,n or print)
  else
    lpegmatch(stepper,str,1,n,action or print)
  end
end
local pattern_math=Cs((P("%")/"\\percent "+P("^")*Cc("{")*lpegpatterns.integer*Cc("}")+P(1))^0)
local pattern_text=Cs((P("%")/"\\percent "+(P("^")/"\\high")*Cc("{")*lpegpatterns.integer*Cc("}")+P(1))^0)
patterns.unittotex=pattern
function parsers.unittotex(str,textmode)
  return lpegmatch(textmode and pattern_text or pattern_math,str)
end
local pattern=Cs((P("^")/"<sup>"*lpegpatterns.integer*Cc("</sup>")+P(1))^0)
function parsers.unittoxml(str)
  return lpegmatch(pattern,str)
end
local cache={}
local spaces=lpeg.patterns.space^0
local dummy=function() end
table.setmetatableindex(cache,function(t,k)
  local separator=P(k)
  local value=(1-separator)^0
  local pattern=spaces*C(value)*separator^0*Cp()
  t[k]=pattern
  return pattern
end)
local commalistiterator=cache[","]
function utilities.parsers.iterator(str,separator)
  local n=#str
  if n==0 then
    return dummy
  else
    local pattern=separator and cache[separator] or commalistiterator
    local p=1
    return function()
      if p<=n then
        local s,e=lpegmatch(pattern,str,p)
        if e then
          p=e
          return s
        end
      end
    end
  end
end
local function initialize(t,name)
  local source=t[name]
  if source then
    local result={}
    for k,v in next,t[name] do
      result[k]=v
    end
    return result
  else
    return {}
  end
end
local function fetch(t,name)
  return t[name] or {}
end
function process(result,more)
  for k,v in next,more do
    result[k]=v
  end
  return result
end
local name=C((1-S(", "))^1)
local parser=(Carg(1)*name/initialize)*(S(", ")^1*(Carg(1)*name/fetch))^0
local merge=Cf(parser,process)
function utilities.parsers.mergehashes(hash,list)
  return lpegmatch(merge,list,1,hash)
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-fmt"] = package.loaded["util-fmt"] or true

-- original size: 2274, stripped down to: 1781

if not modules then modules={} end modules ['util-fmt']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
utilities=utilities or {}
utilities.formatters=utilities.formatters or {}
local formatters=utilities.formatters
local concat,format=table.concat,string.format
local tostring,type=tostring,type
local strip=string.strip
local lpegmatch=lpeg.match
local stripper=lpeg.patterns.stripzeros
function formatters.stripzeros(str)
  return lpegmatch(stripper,str)
end
function formatters.formatcolumns(result,between)
  if result and #result>0 then
    between=between or "   "
    local widths,numbers={},{}
    local first=result[1]
    local n=#first
    for i=1,n do
      widths[i]=0
    end
    for i=1,#result do
      local r=result[i]
      for j=1,n do
        local rj=r[j]
        local tj=type(rj)
        if tj=="number" then
          numbers[j]=true
        end
        if tj~="string" then
          rj=tostring(rj)
          r[j]=rj
        end
        local w=#rj
        if w>widths[j] then
          widths[j]=w
        end
      end
    end
    for i=1,n do
      local w=widths[i]
      if numbers[i] then
        if w>80 then
          widths[i]="%s"..between
         else
          widths[i]="%0"..w.."i"..between
        end
      else
        if w>80 then
          widths[i]="%s"..between
         elseif w>0 then
          widths[i]="%-"..w.."s"..between
        else
          widths[i]="%s"
        end
      end
    end
    local template=strip(concat(widths))
    for i=1,#result do
      local str=format(template,unpack(result[i]))
      result[i]=strip(str)
    end
  end
  return result
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-set"] = package.loaded["trac-set"] or true

-- original size: 12365, stripped down to: 8799

if not modules then modules={} end modules ['trac-set']={ 
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local type,next,tostring=type,next,tostring
local concat=table.concat
local format,find,lower,gsub,topattern=string.format,string.find,string.lower,string.gsub,string.topattern
local is_boolean=string.is_boolean
local settings_to_hash=utilities.parsers.settings_to_hash
local allocate=utilities.storage.allocate
utilities=utilities or {}
local utilities=utilities
local setters=utilities.setters or {}
utilities.setters=setters
local data={}
local trace_initialize=false 
function setters.initialize(filename,name,values) 
  local setter=data[name]
  if setter then
    frozen=true
    local data=setter.data
    if data then
      for key,newvalue in next,values do
        local newvalue=is_boolean(newvalue,newvalue)
        local functions=data[key]
        if functions then
          local oldvalue=functions.value
          if functions.frozen then
            if trace_initialize then
              setter.report("%s: %a is %s to %a",filename,key,"frozen",oldvalue)
            end
          elseif #functions>0 and not oldvalue then
            if trace_initialize then
              setter.report("%s: %a is %s to %a",filename,key,"set",newvalue)
            end
            for i=1,#functions do
              functions[i](newvalue)
            end
            functions.value=newvalue
            functions.frozen=functions.frozen or frozen
          else
            if trace_initialize then
              setter.report("%s: %a is %s as %a",filename,key,"kept",oldvalue)
            end
          end
        else
          functions={ default=newvalue,frozen=frozen }
          data[key]=functions
          if trace_initialize then
            setter.report("%s: %a is %s to %a",filename,key,"defaulted",newvalue)
          end
        end
      end
      return true
    end
  end
end
local function set(t,what,newvalue)
  local data=t.data
  if not data.frozen then
    local done=t.done
    if type(what)=="string" then
      what=settings_to_hash(what) 
    end
    if type(what)~="table" then
      return
    end
    if not done then 
      done={}
      t.done=done
    end
    for w,value in next,what do
      if value=="" then
        value=newvalue
      elseif not value then
        value=false 
      else
        value=is_boolean(value,value)
      end
      w=topattern(w,true,true)
      for name,functions in next,data do
        if done[name] then
        elseif find(name,w) then
          done[name]=true
          for i=1,#functions do
            functions[i](value)
          end
          functions.value=value
        end
      end
    end
  end
end
local function reset(t)
  local data=t.data
  if not data.frozen then
    for name,functions in next,data do
      for i=1,#functions do
        functions[i](false)
      end
      functions.value=false
    end
  end
end
local function enable(t,what)
  set(t,what,true)
end
local function disable(t,what)
  local data=t.data
  if not what or what=="" then
    t.done={}
    reset(t)
  else
    set(t,what,false)
  end
end
function setters.register(t,what,...)
  local data=t.data
  what=lower(what)
  local functions=data[what]
  if not functions then
    functions={}
    data[what]=functions
    if trace_initialize then
      t.report("defining %a",what)
    end
  end
  local default=functions.default 
  for i=1,select("#",...) do
    local fnc=select(i,...)
    local typ=type(fnc)
    if typ=="string" then
      if trace_initialize then
        t.report("coupling %a to %a",what,fnc)
      end
      local s=fnc 
      fnc=function(value) set(t,s,value) end
    elseif typ~="function" then
      fnc=nil
    end
    if fnc then
      functions[#functions+1]=fnc
      local value=functions.value or default
      if value~=nil then
        fnc(value)
        functions.value=value
      end
    end
  end
  return false 
end
function setters.enable(t,what)
  local e=t.enable
  t.enable,t.done=enable,{}
  enable(t,what)
  t.enable,t.done=e,{}
end
function setters.disable(t,what)
  local e=t.disable
  t.disable,t.done=disable,{}
  disable(t,what)
  t.disable,t.done=e,{}
end
function setters.reset(t)
  t.done={}
  reset(t)
end
function setters.list(t) 
  local list=table.sortedkeys(t.data)
  local user,system={},{}
  for l=1,#list do
    local what=list[l]
    if find(what,"^%*") then
      system[#system+1]=what
    else
      user[#user+1]=what
    end
  end
  return user,system
end
function setters.show(t)
  local category=t.name
  local list=setters.list(t)
  t.report()
  for k=1,#list do
    local name=list[k]
    local functions=t.data[name]
    if functions then
      local value,default,modules=functions.value,functions.default,#functions
      value=value==nil and "unset" or tostring(value)
      default=default==nil and "unset" or tostring(default)
      t.report("%-50s   modules: %2i   default: %-12s   value: %-12s",name,modules,default,value)
    end
  end
  t.report()
end
local enable,disable,register,list,show=setters.enable,setters.disable,setters.register,setters.list,setters.show
function setters.report(setter,...)
  print(format("%-15s : %s\n",setter.name,format(...)))
end
local function default(setter,name)
  local d=setter.data[name]
  return d and d.default
end
local function value(setter,name)
  local d=setter.data[name]
  return d and (d.value or d.default)
end
function setters.new(name) 
  local setter 
  setter={
    data=allocate(),
    name=name,
    report=function(...) setters.report (setter,...) end,
    enable=function(...)     enable (setter,...) end,
    disable=function(...)     disable (setter,...) end,
    register=function(...)     register(setter,...) end,
    list=function(...)     list  (setter,...) end,
    show=function(...)     show  (setter,...) end,
    default=function(...) return default (setter,...) end,
    value=function(...) return value  (setter,...) end,
  }
  data[name]=setter
  return setter
end
trackers=setters.new("trackers")
directives=setters.new("directives")
experiments=setters.new("experiments")
local t_enable,t_disable=trackers  .enable,trackers  .disable
local d_enable,d_disable=directives .enable,directives .disable
local e_enable,e_disable=experiments.enable,experiments.disable
local trace_directives=false local trace_directives=false trackers.register("system.directives",function(v) trace_directives=v end)
local trace_experiments=false local trace_experiments=false trackers.register("system.experiments",function(v) trace_experiments=v end)
function directives.enable(...)
  if trace_directives then
    directives.report("enabling: % t",{...})
  end
  d_enable(...)
end
function directives.disable(...)
  if trace_directives then
    directives.report("disabling: % t",{...})
  end
  d_disable(...)
end
function experiments.enable(...)
  if trace_experiments then
    experiments.report("enabling: % t",{...})
  end
  e_enable(...)
end
function experiments.disable(...)
  if trace_experiments then
    experiments.report("disabling: % t",{...})
  end
  e_disable(...)
end
directives.register("system.nostatistics",function(v)
  if statistics then
    statistics.enable=not v
  else
  end
end)
directives.register("system.nolibraries",function(v)
  if libraries then
    libraries=nil 
  else
  end
end)
if environment then
  local engineflags=environment.engineflags
  if engineflags then
    local list=engineflags["c:trackers"] or engineflags["trackers"]
    if type(list)=="string" then
      setters.initialize("commandline flags","trackers",settings_to_hash(list))
    end
    local list=engineflags["c:directives"] or engineflags["directives"]
    if type(list)=="string" then
      setters.initialize("commandline flags","directives",settings_to_hash(list))
    end
  end
end
if texconfig then
  local function set(k,v)
    v=tonumber(v)
    if v then
      texconfig[k]=v
    end
  end
  directives.register("luatex.expanddepth",function(v) set("expand_depth",v)  end)
  directives.register("luatex.hashextra",function(v) set("hash_extra",v)   end)
  directives.register("luatex.nestsize",function(v) set("nest_size",v)   end)
  directives.register("luatex.maxinopen",function(v) set("max_in_open",v)  end)
  directives.register("luatex.maxprintline",function(v) set("max_print_line",v) end)
  directives.register("luatex.maxstrings",function(v) set("max_strings",v)  end)
  directives.register("luatex.paramsize",function(v) set("param_size",v)   end)
  directives.register("luatex.savesize",function(v) set("save_size",v)   end)
  directives.register("luatex.stacksize",function(v) set("stack_size",v)   end)
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-log"] = package.loaded["trac-log"] or true

-- original size: 21795, stripped down to: 14194

if not modules then modules={} end modules ['trac-log']={
  version=1.001,
  comment="companion to trac-log.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local write_nl,write=texio and texio.write_nl or print,texio and texio.write or io.write
local format,gmatch,find=string.format,string.gmatch,string.find
local concat,insert,remove=table.concat,table.insert,table.remove
local topattern=string.topattern
local texcount=tex and tex.count
local next,type,select=next,type,select
local utfchar=utf.char
local setmetatableindex=table.setmetatableindex
local formatters=string.formatters
logs=logs or {}
local logs=logs
local moreinfo=[[
More information about ConTeXt and the tools that come with it can be found at:
]].."\n"..[[
maillist : ntg-context@ntg.nl / http://www.ntg.nl/mailman/listinfo/ntg-context
webpage  : http://www.pragma-ade.nl / http://tex.aanhet.net
wiki     : http://contextgarden.net
]]
utilities.strings.formatters.add (
  formatters,"unichr",
  [["U+" .. format("%%05X",%s) .. " (" .. utfchar(%s) .. ")"]]
)
utilities.strings.formatters.add (
  formatters,"chruni",
  [[utfchar(%s) .. " (U+" .. format("%%05X",%s) .. ")"]]
)
local function ignore() end
setmetatableindex(logs,function(t,k) t[k]=ignore;return ignore end)
local report,subreport,status,settarget,setformats,settranslations
local direct,subdirect,writer,pushtarget,poptarget
if tex and (tex.jobname or tex.formatname) then
  local valueiskey={ __index=function(t,k) t[k]=k return k end } 
  local target="term and log"
  logs.flush=io.flush
  local formats={} setmetatable(formats,valueiskey)
  local translations={} setmetatable(translations,valueiskey)
  writer=function(...)
    write_nl(target,...)
  end
  newline=function()
    write_nl(target,"\n")
  end
  local f_one=formatters["%-15s > %s\n"]
  local f_two=formatters["%-15s >\n"]
  report=function(a,b,c,...)
    if c then
      write_nl(target,f_one(translations[a],formatters[formats[b]](c,...)))
    elseif b then
      write_nl(target,f_one(translations[a],formats[b]))
    elseif a then
      write_nl(target,f_two(translations[a]))
    else
      write_nl(target,"\n")
    end
  end
  local f_one=formatters["%-15s > %s"]
  local f_two=formatters["%-15s >"]
  direct=function(a,b,c,...)
    if c then
      return f_one(translations[a],formatters[formats[b]](c,...))
    elseif b then
      return f_one(translations[a],formats[b])
    elseif a then
      return f_two(translations[a])
    else
      return ""
    end
  end
  local f_one=formatters["%-15s > %s > %s\n"]
  local f_two=formatters["%-15s > %s >\n"]
  subreport=function(a,s,b,c,...)
    if c then
      write_nl(target,f_one(translations[a],translations[s],formatters[formats[b]](c,...)))
    elseif b then
      write_nl(target,f_one(translations[a],translations[s],formats[b]))
    elseif a then
      write_nl(target,f_two(translations[a],translations[s]))
    else
      write_nl(target,"\n")
    end
  end
  local f_one=formatters["%-15s > %s > %s"]
  local f_two=formatters["%-15s > %s >"]
  subdirect=function(a,s,b,c,...)
    if c then
      return f_one(translations[a],translations[s],formatters[formats[b]](c,...))
    elseif b then
      return f_one(translations[a],translations[s],formats[b])
    elseif a then
      return f_two(translations[a],translations[s])
    else
      return ""
    end
  end
  local f_one=formatters["%-15s : %s\n"]
  local f_two=formatters["%-15s :\n"]
  status=function(a,b,c,...)
    if c then
      write_nl(target,f_one(translations[a],formatters[formats[b]](c,...)))
    elseif b then
      write_nl(target,f_one(translations[a],formats[b]))
    elseif a then
      write_nl(target,f_two(translations[a]))
    else
      write_nl(target,"\n")
    end
  end
  local targets={
    logfile="log",
    log="log",
    file="log",
    console="term",
    terminal="term",
    both="term and log",
  }
  settarget=function(whereto)
    target=targets[whereto or "both"] or targets.both
    if target=="term" or target=="term and log" then
      logs.flush=io.flush
    else
      logs.flush=ignore
    end
  end
  local stack={}
  pushtarget=function(newtarget)
    insert(stack,target)
    settarget(newtarget)
  end
  poptarget=function()
    if #stack>0 then
      settarget(remove(stack))
    end
  end
  setformats=function(f)
    formats=f
  end
  settranslations=function(t)
    translations=t
  end
else
  logs.flush=ignore
  writer=write_nl
  newline=function()
    write_nl("\n")
  end
  local f_one=formatters["%-15s | %s"]
  local f_two=formatters["%-15s |"]
  report=function(a,b,c,...)
    if c then
      write_nl(f_one(a,formatters[b](c,...)))
    elseif b then
      write_nl(f_one(a,b))
    elseif a then
      write_nl(f_two(a))
    else
      write_nl("")
    end
  end
  local f_one=formatters["%-15s | %s | %s"]
  local f_two=formatters["%-15s | %s |"]
  subreport=function(a,sub,b,c,...)
    if c then
      write_nl(f_one(a,sub,formatters[b](c,...)))
    elseif b then
      write_nl(f_one(a,sub,b))
    elseif a then
      write_nl(f_two(a,sub))
    else
      write_nl("")
    end
  end
  local f_one=formatters["%-15s : %s\n"]
  local f_two=formatters["%-15s :\n"]
  status=function(a,b,c,...) 
    if c then
      write_nl(f_one(a,formatters[b](c,...)))
    elseif b then
      write_nl(f_one(a,b)) 
    elseif a then
      write_nl(f_two(a))
    else
      write_nl("\n")
    end
  end
  direct=ignore
  subdirect=ignore
  settarget=ignore
  pushtarget=ignore
  poptarget=ignore
  setformats=ignore
  settranslations=ignore
end
logs.report=report
logs.subreport=subreport
logs.status=status
logs.settarget=settarget
logs.pushtarget=pushtarget
logs.poptarget=poptarget
logs.setformats=setformats
logs.settranslations=settranslations
logs.direct=direct
logs.subdirect=subdirect
logs.writer=writer
logs.newline=newline
local data,states={},nil
function logs.reporter(category,subcategory)
  local logger=data[category]
  if not logger then
    local state=false
    if states==true then
      state=true
    elseif type(states)=="table" then
      for c,_ in next,states do
        if find(category,c) then
          state=true
          break
        end
      end
    end
    logger={
      reporters={},
      state=state,
    }
    data[category]=logger
  end
  local reporter=logger.reporters[subcategory or "default"]
  if not reporter then
    if subcategory then
      reporter=function(...)
        if not logger.state then
          subreport(category,subcategory,...)
        end
      end
      logger.reporters[subcategory]=reporter
    else
      local tag=category
      reporter=function(...)
        if not logger.state then
          report(category,...)
        end
      end
      logger.reporters.default=reporter
    end
  end
  return reporter
end
logs.new=logs.reporter
local ctxreport=logs.writer
function logs.setmessenger(m)
  ctxreport=m
end
function logs.messenger(category,subcategory)
  if subcategory then
    return function(...)
      ctxreport(subdirect(category,subcategory,...))
    end
  else
    return function(...)
      ctxreport(direct(category,...))
    end
  end
end
local function setblocked(category,value)
  if category==true then
    category,value="*",true
  elseif category==false then
    category,value="*",false
  elseif value==nil then
    value=true
  end
  if category=="*" then
    states=value
    for k,v in next,data do
      v.state=value
    end
  else
    states=utilities.parsers.settings_to_hash(category)
    for c,_ in next,states do
      if data[c] then
        v.state=value
      else
        c=topattern(c,true,true)
        for k,v in next,data do
          if find(k,c) then
            v.state=value
          end
        end
      end
    end
  end
end
function logs.disable(category,value)
  setblocked(category,value==nil and true or value)
end
function logs.enable(category)
  setblocked(category,false)
end
function logs.categories()
  return table.sortedkeys(data)
end
function logs.show()
  local n,c,s,max=0,0,0,0
  for category,v in table.sortedpairs(data) do
    n=n+1
    local state=v.state
    local reporters=v.reporters
    local nc=#category
    if nc>c then
      c=nc
    end
    for subcategory,_ in next,reporters do
      local ns=#subcategory
      if ns>c then
        s=ns
      end
      local m=nc+ns
      if m>max then
        max=m
      end
    end
    local subcategories=concat(table.sortedkeys(reporters),", ")
    if state==true then
      state="disabled"
    elseif state==false then
      state="enabled"
    else
      state="unknown"
    end
    report("logging","category %a, subcategories %a, state %a",category,subcategories,state)
  end
  report("logging","categories: %s, max category: %s, max subcategory: %s, max combined: %s",n,c,s,max)
end
local delayed_reporters={}
setmetatableindex(delayed_reporters,function(t,k)
  local v=logs.reporter(k.name)
  t[k]=v
  return v
end)
function utilities.setters.report(setter,...)
  delayed_reporters[setter](...)
end
directives.register("logs.blocked",function(v)
  setblocked(v,true)
end)
directives.register("logs.target",function(v)
  settarget(v)
end)
local report_pages=logs.reporter("pages") 
local real,user,sub
function logs.start_page_number()
  real,user,sub=texcount.realpageno,texcount.userpageno,texcount.subpageno
end
local timing=false
local starttime=nil
local lasttime=nil
trackers.register("pages.timing",function(v) 
  starttime=os.clock()
  timing=true
end)
function logs.stop_page_number() 
  if timing then
    local elapsed,average
    local stoptime=os.clock()
    if not lasttime or real<2 then
      elapsed=stoptime
      average=stoptime
      starttime=stoptime
    else
      elapsed=stoptime-lasttime
      average=(stoptime-starttime)/(real-1)
    end
    lasttime=stoptime
    if real<=0 then
      report_pages("flushing page, time %0.04f / %0.04f",elapsed,average)
    elseif user<=0 then
      report_pages("flushing realpage %s, time %0.04f / %0.04f",real,elapsed,average)
    elseif sub<=0 then
      report_pages("flushing realpage %s, userpage %s, time %0.04f / %0.04f",real,user,elapsed,average)
    else
      report_pages("flushing realpage %s, userpage %s, subpage %s, time %0.04f / %0.04f",real,user,sub,elapsed,average)
    end
  else
    if real<=0 then
      report_pages("flushing page")
    elseif user<=0 then
      report_pages("flushing realpage %s",real)
    elseif sub<=0 then
      report_pages("flushing realpage %s, userpage %s",real,user)
    else
      report_pages("flushing realpage %s, userpage %s, subpage %s",real,user,sub)
    end
  end
  logs.flush()
end
local report_files=logs.reporter("files")
local nesting=0
local verbose=false
local hasscheme=url.hasscheme
function logs.show_open(name)
end
function logs.show_close(name)
end
function logs.show_load(name)
end
local simple=logs.reporter("comment")
logs.simple=simple
logs.simpleline=simple
function logs.setprogram () end 
function logs.extendbanner() end 
function logs.reportlines () end 
function logs.reportbanner() end 
function logs.reportline () end 
function logs.simplelines () end 
function logs.help    () end
local Carg,C,lpegmatch=lpeg.Carg,lpeg.C,lpeg.match
local p_newline=lpeg.patterns.newline
local linewise=(
  Carg(1)*C((1-p_newline)^1)/function(t,s) t.report(s) end+Carg(1)*p_newline^2/function(t)  t.report() end+p_newline
)^1
local function reportlines(t,str)
  if str then
    lpegmatch(linewise,str,1,t)
  end
end
local function reportbanner(t)
  local banner=t.banner
  if banner then
    t.report(banner)
    t.report()
  end
end
local function reportversion(t)
  local banner=t.banner
  if banner then
    t.report(banner)
  end
end
local function reporthelp(t,...)
  local helpinfo=t.helpinfo
  if type(helpinfo)=="string" then
    reportlines(t,helpinfo)
  elseif type(helpinfo)=="table" then
    for i=1,select("#",...) do
      reportlines(t,t.helpinfo[select(i,...)])
      if i<n then
        t.report()
      end
    end
  end
end
local function reportinfo(t)
  t.report()
  reportlines(t,t.moreinfo)
end
local function reportexport(t,method)
  report(t.helpinfo)
end
local reporters={
  lines=reportlines,
  banner=reportbanner,
  version=reportversion,
  help=reporthelp,
  info=reportinfo,
  export=reportexport,
}
local exporters={
}
logs.reporters=reporters
logs.exporters=exporters
function logs.application(t)
  t.name=t.name  or "unknown"
  t.banner=t.banner
  t.moreinfo=moreinfo
  t.report=logs.reporter(t.name)
  t.help=function(...)
    reporters.banner(t)
    reporters.help(t,...)
    reporters.info(t)
  end
  t.export=function(...)
    reporters.export(t,...)
  end
  t.identify=function()
    reporters.banner(t)
  end
  t.version=function()
    reporters.version(t)
  end
  return t
end
function logs.system(whereto,process,jobname,category,...)
  local message=formatters["%s %s => %s => %s => %s\r"](os.date("%d/%m/%y %H:%m:%S"),process,jobname,category,format(...))
  for i=1,10 do
    local f=io.open(whereto,"a") 
    if f then
      f:write(message)
      f:close()
      break
    else
      sleep(0.1)
    end
  end
end
local report_system=logs.reporter("system","logs")
function logs.obsolete(old,new)
  local o=loadstring("return "..new)()
  if type(o)=="function" then
    return function(...)
      report_system("function %a is obsolete, use %a",old,new)
      loadstring(old.."="..new.." return "..old)()(...)
    end
  elseif type(o)=="table" then
    local t,m={},{}
    m.__index=function(t,k)
      report_system("table %a is obsolete, use %a",old,new)
      m.__index,m.__newindex=o,o
      return o[k]
    end
    m.__newindex=function(t,k,v)
      report_system("table %a is obsolete, use %a",old,new)
      m.__index,m.__newindex=o,o
      o[k]=v
    end
    if libraries then
      libraries.obsolete[old]=t 
    end
    setmetatable(t,m)
    return t
  end
end
if utilities then
  utilities.report=report_system
end
if tex and tex.error then
  function logs.texerrormessage(...) 
    tex.error(format(...),{})
  end
else
  function logs.texerrormessage(...)
    print(format(...))
  end
end
io.stdout:setvbuf('no')
io.stderr:setvbuf('no')


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-inf"] = package.loaded["trac-inf"] or true

-- original size: 5791, stripped down to: 4540

if not modules then modules={} end modules ['trac-inf']={
  version=1.001,
  comment="companion to trac-inf.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local type,tonumber=type,tonumber
local format,lower=string.format,string.lower
local concat=table.concat
local clock=os.gettimeofday or os.clock 
statistics=statistics or {}
local statistics=statistics
statistics.enable=true
statistics.threshold=0.01
local statusinfo,n,registered,timers={},0,{},{}
table.setmetatableindex(timers,function(t,k)
  local v={ timing=0,loadtime=0 }
  t[k]=v
  return v
end)
local function hastiming(instance)
  return instance and timers[instance]
end
local function resettiming(instance)
  timers[instance or "notimer"]={ timing=0,loadtime=0 }
end
local function starttiming(instance)
  local timer=timers[instance or "notimer"]
  local it=timer.timing or 0
  if it==0 then
    timer.starttime=clock()
    if not timer.loadtime then
      timer.loadtime=0
    end
  end
  timer.timing=it+1
end
local function stoptiming(instance,report)
  local timer=timers[instance or "notimer"]
  local it=timer.timing
  if it>1 then
    timer.timing=it-1
  else
    local starttime=timer.starttime
    if starttime then
      local stoptime=clock()
      local loadtime=stoptime-starttime
      timer.stoptime=stoptime
      timer.loadtime=timer.loadtime+loadtime
      if report then
        statistics.report("load time %0.3f",loadtime)
      end
      timer.timing=0
      return loadtime
    end
  end
  return 0
end
local function elapsed(instance)
  if type(instance)=="number" then
    return instance or 0
  else
    local timer=timers[instance or "notimer"]
    return timer and timer.loadtime or 0
  end
end
local function elapsedtime(instance)
  return format("%0.3f",elapsed(instance))
end
local function elapsedindeed(instance)
  return elapsed(instance)>statistics.threshold
end
local function elapsedseconds(instance,rest) 
  if elapsedindeed(instance) then
    return format("%0.3f seconds %s",elapsed(instance),rest or "")
  end
end
statistics.hastiming=hastiming
statistics.resettiming=resettiming
statistics.starttiming=starttiming
statistics.stoptiming=stoptiming
statistics.elapsed=elapsed
statistics.elapsedtime=elapsedtime
statistics.elapsedindeed=elapsedindeed
statistics.elapsedseconds=elapsedseconds
function statistics.register(tag,fnc)
  if statistics.enable and type(fnc)=="function" then
    local rt=registered[tag] or (#statusinfo+1)
    statusinfo[rt]={ tag,fnc }
    registered[tag]=rt
    if #tag>n then n=#tag end
  end
end
local report=logs.reporter("mkiv lua stats")
function statistics.show()
  if statistics.enable then
    local register=statistics.register
    register("luatex banner",function()
      return lower(status.banner)
    end)
    register("control sequences",function()
      return format("%s of %s + %s",status.cs_count,status.hash_size,status.hash_extra)
    end)
    register("callbacks",function()
      local total,indirect=status.callbacks or 0,status.indirect_callbacks or 0
      return format("%s direct, %s indirect, %s total",total-indirect,indirect,total)
    end)
    if jit then
      local status={ jit.status() }
      if status[1] then
        register("luajit status",function()
          return concat(status," ",2)
        end)
      end
    end
    register("current memory usage",statistics.memused)
    register("runtime",statistics.runtime)
    logs.newline() 
    for i=1,#statusinfo do
      local s=statusinfo[i]
      local r=s[2]()
      if r then
        report("%s: %s",s[1],r)
      end
    end
    statistics.enable=false
  end
end
function statistics.memused() 
  local round=math.round or math.floor
  return format("%s MB (ctx: %s MB)",round(collectgarbage("count")/1000),round(status.luastate_bytes/1000000))
end
starttiming(statistics)
function statistics.formatruntime(runtime) 
  return format("%s seconds",runtime)  
end
function statistics.runtime()
  stoptiming(statistics)
  return statistics.formatruntime(elapsedtime(statistics))
end
local report=logs.reporter("system")
function statistics.timed(action)
  starttiming("run")
  action()
  stoptiming("run")
  report("total runtime: %s",elapsedtime("run"))
end
commands=commands or {}
function commands.resettimer(name)
  resettiming(name or "whatever")
  starttiming(name or "whatever")
end
function commands.elapsedtime(name)
  stoptiming(name or "whatever")
  context(elapsedtime(name or "whatever"))
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-pro"] = package.loaded["trac-pro"] or true

-- original size: 5773, stripped down to: 3453

if not modules then modules={} end modules ['trac-pro']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local getmetatable,setmetatable,rawset,type=getmetatable,setmetatable,rawset,type
local trace_namespaces=false trackers.register("system.namespaces",function(v) trace_namespaces=v end)
local report_system=logs.reporter("system","protection")
namespaces=namespaces or {}
local namespaces=namespaces
local registered={}
local function report_index(k,name)
  if trace_namespaces then
    report_system("reference to %a in protected namespace %a: %s",k,name,debug.traceback())
  else
    report_system("reference to %a in protected namespace %a",k,name)
  end
end
local function report_newindex(k,name)
  if trace_namespaces then
    report_system("assignment to %a in protected namespace %a: %s",k,name,debug.traceback())
  else
    report_system("assignment to %a in protected namespace %a",k,name)
  end
end
local function register(name)
  local data=name=="global" and _G or _G[name]
  if not data then
    return 
  end
  registered[name]=data
  local m=getmetatable(data)
  if not m then
    m={}
    setmetatable(data,m)
  end
  local index,newindex={},{}
  m.__saved__index=m.__index
  m.__no__index=function(t,k)
    if not index[k] then
      index[k]=true
      report_index(k,name)
    end
    return nil
  end
  m.__saved__newindex=m.__newindex
  m.__no__newindex=function(t,k,v)
    if not newindex[k] then
      newindex[k]=true
      report_newindex(k,name)
    end
    rawset(t,k,v)
  end
  m.__protection__depth=0
end
local function private(name) 
  local data=registered[name]
  if not data then
    data=_G[name]
    if not data then
      data={}
      _G[name]=data
    end
    register(name)
  end
  return data
end
local function protect(name)
  local data=registered[name]
  if not data then
    return
  end
  local m=getmetatable(data)
  local pd=m.__protection__depth
  if pd>0 then
    m.__protection__depth=pd+1
  else
    m.__save_d_index,m.__saved__newindex=m.__index,m.__newindex
    m.__index,m.__newindex=m.__no__index,m.__no__newindex
    m.__protection__depth=1
  end
end
local function unprotect(name)
  local data=registered[name]
  if not data then
    return
  end
  local m=getmetatable(data)
  local pd=m.__protection__depth
  if pd>1 then
    m.__protection__depth=pd-1
  else
    m.__index,m.__newindex=m.__saved__index,m.__saved__newindex
    m.__protection__depth=0
  end
end
local function protectall()
  for name,_ in next,registered do
    if name~="global" then
      protect(name)
    end
  end
end
local function unprotectall()
  for name,_ in next,registered do
    if name~="global" then
      unprotect(name)
    end
  end
end
namespaces.register=register    
namespaces.private=private     
namespaces.protect=protect
namespaces.unprotect=unprotect
namespaces.protectall=protectall
namespaces.unprotectall=unprotectall
namespaces.private("namespaces") registered={} register("global") 
directives.register("system.protect",function(v)
  if v then
    protectall()
  else
    unprotectall()
  end
end)
directives.register("system.checkglobals",function(v)
  if v then
    report_system("enabling global namespace guard")
    protect("global")
  else
    report_system("disabling global namespace guard")
    unprotect("global")
  end
end)


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-lua"] = package.loaded["util-lua"] or true

-- original size: 12575, stripped down to: 8700

if not modules then modules={} end modules ['util-lua']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  comment="the strip code is written by Peter Cawley",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local rep,sub,byte,dump,format=string.rep,string.sub,string.byte,string.dump,string.format
local load,loadfile,type=load,loadfile,type
utilities=utilities or {}
utilities.lua=utilities.lua or {}
local luautilities=utilities.lua
local report_lua=logs.reporter("system","lua")
local tracestripping=false
local forcestupidcompile=true 
luautilities.stripcode=true 
luautilities.alwaysstripcode=false 
luautilities.nofstrippedchunks=0
luautilities.nofstrippedbytes=0
local strippedchunks={} 
luautilities.strippedchunks=strippedchunks
luautilities.suffixes={
  tma="tma",
  tmc=jit and "tmb" or "tmc",
  lua="lua",
  luc=jit and "lub" or "luc",
  lui="lui",
  luv="luv",
  luj="luj",
  tua="tua",
  tuc="tuc",
}
if jit or status.luatex_version>=74 then
  local function register(name)
    if tracestripping then
      report_lua("stripped bytecode from %a",name or "unknown")
    end
    strippedchunks[#strippedchunks+1]=name
    luautilities.nofstrippedchunks=luautilities.nofstrippedchunks+1
  end
  local function stupidcompile(luafile,lucfile,strip)
    local code=io.loaddata(luafile)
    if code and code~="" then
      code=load(code)
      if code then
        code=dump(code,strip and luautilities.stripcode or luautilities.alwaysstripcode)
        if code and code~="" then
          register(name)
          io.savedata(lucfile,code)
          return true,0
        end
      else
        report_lua("fatal error %a in file %a",1,luafile)
      end
    else
      report_lua("fatal error %a in file %a",2,luafile)
    end
    return false,0
  end
  function luautilities.loadedluacode(fullname,forcestrip,name)
    name=name or fullname
    local code=environment.loadpreprocessedfile and environment.loadpreprocessedfile(fullname) or loadfile(fullname)
    if code then
      code()
    end
    if forcestrip and luautilities.stripcode then
      if type(forcestrip)=="function" then
        forcestrip=forcestrip(fullname)
      end
      if forcestrip or luautilities.alwaysstripcode then
        register(name)
        return load(dump(code,true)),0
      else
        return code,0
      end
    elseif luautilities.alwaysstripcode then
      register(name)
      return load(dump(code,true)),0
    else
      return code,0
    end
  end
  function luautilities.strippedloadstring(code,forcestrip,name) 
    if forcestrip and luautilities.stripcode or luautilities.alwaysstripcode then
      code=load(code)
      if not code then
        report_lua("fatal error %a in file %a",3,name)
      end
      register(name)
      code=dump(code,true)
    end
    return load(code),0
  end
  function luautilities.compile(luafile,lucfile,cleanup,strip,fallback) 
    report_lua("compiling %a into %a",luafile,lucfile)
    os.remove(lucfile)
    local done=stupidcompile(luafile,lucfile,strip~=false)
    if done then
      report_lua("dumping %a into %a stripped",luafile,lucfile)
      if cleanup==true and lfs.isfile(lucfile) and lfs.isfile(luafile) then
        report_lua("removing %a",luafile)
        os.remove(luafile)
      end
    end
    return done
  end
  function luautilities.loadstripped(...)
    local l=load(...)
    if l then
      return load(dump(l,true))
    end
  end
else
  local function register(name,before,after)
    local delta=before-after
    if tracestripping then
      report_lua("bytecodes stripped from %a, # before %s, # after %s, delta %s",name,before,after,delta)
    end
    strippedchunks[#strippedchunks+1]=name
    luautilities.nofstrippedchunks=luautilities.nofstrippedchunks+1
    luautilities.nofstrippedbytes=luautilities.nofstrippedbytes+delta
    return delta
  end
  local strip_code_pc
  if _MAJORVERSION==5 and _MINORVERSION==1 then
    strip_code_pc=function(dump,name)
      local before=#dump
      local version,format,endian,int,size,ins,num=byte(dump,5,11)
      local subint
      if endian==1 then
        subint=function(dump,i,l)
          local val=0
          for n=l,1,-1 do
            val=val*256+byte(dump,i+n-1)
          end
          return val,i+l
        end
      else
        subint=function(dump,i,l)
          local val=0
          for n=1,l,1 do
            val=val*256+byte(dump,i+n-1)
          end
          return val,i+l
        end
      end
      local strip_function
      strip_function=function(dump)
        local count,offset=subint(dump,1,size)
        local stripped,dirty=rep("\0",size),offset+count
        offset=offset+count+int*2+4
        offset=offset+int+subint(dump,offset,int)*ins
        count,offset=subint(dump,offset,int)
        for n=1,count do
          local t
          t,offset=subint(dump,offset,1)
          if t==1 then
            offset=offset+1
          elseif t==4 then
            offset=offset+size+subint(dump,offset,size)
          elseif t==3 then
            offset=offset+num
          end
        end
        count,offset=subint(dump,offset,int)
        stripped=stripped..sub(dump,dirty,offset-1)
        for n=1,count do
          local proto,off=strip_function(sub(dump,offset,-1))
          stripped,offset=stripped..proto,offset+off-1
        end
        offset=offset+subint(dump,offset,int)*int+int
        count,offset=subint(dump,offset,int)
        for n=1,count do
          offset=offset+subint(dump,offset,size)+size+int*2
        end
        count,offset=subint(dump,offset,int)
        for n=1,count do
          offset=offset+subint(dump,offset,size)+size
        end
        stripped=stripped..rep("\0",int*3)
        return stripped,offset
      end
      dump=sub(dump,1,12)..strip_function(sub(dump,13,-1))
      local after=#dump
      local delta=register(name,before,after)
      return dump,delta
    end
  else
    strip_code_pc=function(dump,name)
      return dump,0
    end
  end
  function luautilities.loadedluacode(fullname,forcestrip,name)
    local code=environment.loadpreprocessedfile and environment.preprocessedloadfile(fullname) or loadfile(fullname)
    if code then
      code()
    end
    if forcestrip and luautilities.stripcode then
      if type(forcestrip)=="function" then
        forcestrip=forcestrip(fullname)
      end
      if forcestrip then
        local code,n=strip_code_pc(dump(code),name)
        return load(code),n
      elseif luautilities.alwaysstripcode then
        return load(strip_code_pc(dump(code),name))
      else
        return code,0
      end
    elseif luautilities.alwaysstripcode then
      return load(strip_code_pc(dump(code),name))
    else
      return code,0
    end
  end
  function luautilities.strippedloadstring(code,forcestrip,name) 
    local n=0
    if (forcestrip and luautilities.stripcode) or luautilities.alwaysstripcode then
      code=load(code)
      if not code then
        report_lua("fatal error in file %a",name)
      end
      code,n=strip_code_pc(dump(code),name)
    end
    return load(code),n
  end
  local function stupidcompile(luafile,lucfile,strip)
    local code=io.loaddata(luafile)
    local n=0
    if code and code~="" then
      code=load(code)
      if not code then
        report_lua("fatal error in file %a",luafile)
      end
      code=dump(code)
      if strip then
        code,n=strip_code_pc(code,luautilities.stripcode or luautilities.alwaysstripcode,luafile) 
      end
      if code and code~="" then
        io.savedata(lucfile,code)
      end
    end
    return n
  end
  local luac_normal="texluac -o %q %q"
  local luac_strip="texluac -s -o %q %q"
  function luautilities.compile(luafile,lucfile,cleanup,strip,fallback) 
    report_lua("compiling %a into %a",luafile,lucfile)
    os.remove(lucfile)
    local done=false
    if strip~=false then
      strip=true
    end
    if forcestupidcompile then
      fallback=true
    elseif strip then
      done=os.spawn(format(luac_strip,lucfile,luafile))==0
    else
      done=os.spawn(format(luac_normal,lucfile,luafile))==0
    end
    if not done and fallback then
      local n=stupidcompile(luafile,lucfile,strip)
      if n>0 then
        report_lua("%a dumped into %a (%i bytes stripped)",luafile,lucfile,n)
      else
        report_lua("%a dumped into %a (unstripped)",luafile,lucfile)
      end
      cleanup=false 
      done=true 
    end
    if done and cleanup==true and lfs.isfile(lucfile) and lfs.isfile(luafile) then
      report_lua("removing %a",luafile)
      os.remove(luafile)
    end
    return done
  end
  luautilities.loadstripped=loadstring
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-deb"] = package.loaded["util-deb"] or true

-- original size: 3708, stripped down to: 2568

if not modules then modules={} end modules ['util-deb']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local debug=require "debug"
local getinfo=debug.getinfo
local type,next,tostring=type,next,tostring
local format,find=string.format,string.find
local is_boolean=string.is_boolean
utilities=utilities or {}
local debugger=utilities.debugger or {}
utilities.debugger=debugger
local counters={}
local names={}
local report=logs.reporter("debugger")
local function hook()
  local f=getinfo(2) 
  if f then
    local n="unknown"
    if f.what=="C" then
      n=f.name or '<anonymous>'
      if not names[n] then
        names[n]=format("%42s",n)
      end
    else
      n=f.name or f.namewhat or f.what
      if not n or n=="" then
        n="?"
      end
      if not names[n] then
        names[n]=format("%42s : % 5i : %s",n,f.linedefined or 0,f.short_src or "unknown source")
      end
    end
    counters[n]=(counters[n] or 0)+1
  end
end
function debugger.showstats(printer,threshold) 
  printer=printer or report
  threshold=threshold or 0
  local total,grandtotal,functions=0,0,0
  local dataset={}
  for name,count in next,counters do
    dataset[#dataset+1]={ name,count }
  end
  table.sort(dataset,function(a,b) return a[2]==b[2] and b[1]>a[1] or a[2]>b[2] end)
  for i=1,#dataset do
    local d=dataset[i]
    local name=d[1]
    local count=d[2]
    if count>threshold and not find(name,"for generator") then 
      printer(format("%8i  %s\n",count,names[name]))
      total=total+count
    end
    grandtotal=grandtotal+count
    functions=functions+1
  end
  printer("\n")
  printer(format("functions  : % 10i\n",functions))
  printer(format("total      : % 10i\n",total))
  printer(format("grand total: % 10i\n",grandtotal))
  printer(format("threshold  : % 10i\n",threshold))
end
function debugger.savestats(filename,threshold)
  local f=io.open(filename,'w')
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
end
function traceback()
  local level=1
  while true do
    local info=debug.getinfo(level,"Sl")
    if not info then
      break
    elseif info.what=="C" then
      print(format("%3i : C function",level))
    else
      print(format("%3i : [%s]:%d",level,info.short_src,info.currentline))
    end
    level=level+1
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-mrg"] = package.loaded["util-mrg"] or true

-- original size: 7294, stripped down to: 5798

if not modules then modules={} end modules ['util-mrg']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local gsub,format=string.gsub,string.format
local concat=table.concat
local type,next=type,next
local P,R,S,V,Ct,C,Cs,Cc,Cp,Cmt,Cb,Cg=lpeg.P,lpeg.R,lpeg.S,lpeg.V,lpeg.Ct,lpeg.C,lpeg.Cs,lpeg.Cc,lpeg.Cp,lpeg.Cmt,lpeg.Cb,lpeg.Cg
local lpegmatch,patterns=lpeg.match,lpeg.patterns
utilities=utilities or {}
local merger=utilities.merger or {}
utilities.merger=merger
merger.strip_comment=true
local report=logs.reporter("system","merge")
utilities.report=report
local m_begin_merge="begin library merge"
local m_end_merge="end library merge"
local m_begin_closure="do -- create closure to overcome 200 locals limit"
local m_end_closure="end -- of closure"
local m_pattern="%c+".."%-%-%s+"..m_begin_merge.."%c+(.-)%c+".."%-%-%s+"..m_end_merge.."%c+"
local m_format="\n\n-- "..m_begin_merge.."\n%s\n".."-- "..m_end_merge.."\n\n"
local m_faked="-- ".."created merged file".."\n\n".."-- "..m_begin_merge.."\n\n".."-- "..m_end_merge.."\n\n"
local m_report=[[
-- used libraries    : %s
-- skipped libraries : %s
-- original bytes    : %s
-- stripped bytes    : %s
]]
local m_preloaded=[[package.loaded[%q] = package.loaded[%q] or true]]
local function self_fake()
  return m_faked
end
local function self_nothing()
  return ""
end
local function self_load(name)
  local data=io.loaddata(name) or ""
  if data=="" then
    report("unknown file %a",name)
  else
    report("inserting file %a",name)
  end
  return data or ""
end
local space=patterns.space
local eol=patterns.newline
local equals=P("=")^0
local open=P("[")*Cg(equals,"init")*P("[")*P("\n")^-1
local close=P("]")*C(equals)*P("]")
local closeeq=Cmt(close*Cb("init"),function(s,i,a,b) return a==b end)
local longstring=open*(1-closeeq)^0*close
local quoted=patterns.quoted
local emptyline=space^0*eol
local operator1=P("<=")+P(">=")+P("~=")+P("..")+S("/^<>=*+%%")
local operator2=S("*+/")
local operator3=S("-")
local separator=S(",;")
local ignore=(P("]")*space^1*P("=")*space^1*P("]"))/"]=["+(P("=")*space^1*P("{"))/"={"+(P("(")*space^1)/"("+(P("{")*(space+eol)^1*P("}"))/"{}"
local strings=quoted 
local longcmt=(emptyline^0*P("--")*longstring*emptyline^0)/""
local longstr=longstring
local comment=emptyline^0*P("--")*P("-")^0*(1-eol)^0*emptyline^1/"\n"
local pack=((eol+space)^0/"")*operator1*((eol+space)^0/"")+((eol+space)^0/"")*operator2*((space)^0/"")+((eol+space)^1/"")*operator3*((space)^1/"")+((space)^0/"")*separator*((space)^0/"")
local lines=emptyline^2/"\n"
local spaces=(space*space)/" "
local compact=Cs ((
  ignore+strings+longcmt+longstr+comment+pack+lines+spaces+1
)^1 )
local strip=Cs((emptyline^2/"\n"+1)^0)
local stripreturn=Cs((1-P("return")*space^1*P(1-space-eol)^1*(space+eol)^0*P(-1))^1)
function merger.compact(data)
  return lpegmatch(strip,lpegmatch(compact,data))
end
local function self_compact(data)
  local delta=0
  if merger.strip_comment then
    local before=#data
    data=lpegmatch(compact,data)
    data=lpegmatch(strip,data)
    local after=#data
    delta=before-after
    report("original size %s, compacted to %s, stripped %s",before,after,delta)
    data=format("-- original size: %s, stripped down to: %s\n\n%s",before,after,data)
  end
  return lpegmatch(stripreturn,data) or data,delta
end
local function self_save(name,data)
  if data~="" then
    io.savedata(name,data)
    report("saving %s with size %s",name,#data)
  end
end
local function self_swap(data,code)
  return data~="" and (gsub(data,m_pattern,function() return format(m_format,code) end,1)) or ""
end
local function self_libs(libs,list)
  local result,f,frozen,foundpath={},nil,false,nil
  result[#result+1]="\n"
  if type(libs)=='string' then libs={ libs } end
  if type(list)=='string' then list={ list } end
  for i=1,#libs do
    local lib=libs[i]
    for j=1,#list do
      local pth=gsub(list[j],"\\","/") 
      report("checking library path %a",pth)
      local name=pth.."/"..lib
      if lfs.isfile(name) then
        foundpath=pth
      end
    end
    if foundpath then break end
  end
  if foundpath then
    report("using library path %a",foundpath)
    local right,wrong,original,stripped={},{},0,0
    for i=1,#libs do
      local lib=libs[i]
      local fullname=foundpath.."/"..lib
      if lfs.isfile(fullname) then
        report("using library %a",fullname)
        local preloaded=file.nameonly(lib)
        local data=io.loaddata(fullname,true)
        original=original+#data
        local data,delta=self_compact(data)
        right[#right+1]=lib
        result[#result+1]=m_begin_closure
        result[#result+1]=format(m_preloaded,preloaded,preloaded)
        result[#result+1]=data
        result[#result+1]=m_end_closure
        stripped=stripped+delta
      else
        report("skipping library %a",fullname)
        wrong[#wrong+1]=lib
      end
    end
    right=#right>0 and concat(right," ") or "-"
    wrong=#wrong>0 and concat(wrong," ") or "-"
    report("used libraries: %a",right)
    report("skipped libraries: %a",wrong)
    report("original bytes: %a",original)
    report("stripped bytes: %a",stripped)
    result[#result+1]=format(m_report,right,wrong,original,stripped)
  else
    report("no valid library path found")
  end
  return concat(result,"\n\n")
end
function merger.selfcreate(libs,list,target)
  if target then
    self_save(target,self_swap(self_fake(),self_libs(libs,list)))
  end
end
function merger.selfmerge(name,libs,list,target)
  self_save(target or name,self_swap(self_load(name),self_libs(libs,list)))
end
function merger.selfclean(name)
  self_save(name,self_swap(self_load(name),self_nothing()))
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-tpl"] = package.loaded["util-tpl"] or true

-- original size: 5655, stripped down to: 3242

if not modules then modules={} end modules ['util-tpl']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
utilities.templates=utilities.templates or {}
local templates=utilities.templates
local trace_template=false trackers.register("templates.trace",function(v) trace_template=v end)
local report_template=logs.reporter("template")
local tostring=tostring
local format,sub=string.format,string.sub
local P,C,Cs,Carg,lpegmatch=lpeg.P,lpeg.C,lpeg.Cs,lpeg.Carg,lpeg.match
local replacer
local function replacekey(k,t,how,recursive)
  local v=t[k]
  if not v then
    if trace_template then
      report_template("unknown key %a",k)
    end
    return ""
  else
    v=tostring(v)
    if trace_template then
      report_template("setting key %a to value %a",k,v)
    end
    if recursive then
      return lpegmatch(replacer,v,1,t,how,recursive)
    else
      return v
    end
  end
end
local sqlescape=lpeg.replacer {
  { "'","''"  },
  { "\\","\\\\" },
  { "\r\n","\\n" },
  { "\r","\\n" },
}
local sqlquotedescape=lpeg.Cs(lpeg.Cc("'")*sqlescape*lpeg.Cc("'"))
local escapers={
  lua=function(s)
    return sub(format("%q",s),2,-2)
  end,
  sql=function(s)
    return lpegmatch(sqlescape,s)
  end,
}
local quotedescapers={
  lua=function(s)
    return format("%q",s)
  end,
  sql=function(s)
    return lpegmatch(sqlquotedescape,s)
  end,
}
lpeg.patterns.sqlescape=sqlescape
lpeg.patterns.sqlescape=sqlquotedescape
local luaescaper=escapers.lua
local quotedluaescaper=quotedescapers.lua
local function replacekeyunquoted(s,t,how,recurse) 
  local escaper=how and escapers[how] or luaescaper
  return escaper(replacekey(s,t,how,recurse))
end
local function replacekeyquoted(s,t,how,recurse) 
  local escaper=how and quotedescapers[how] or quotedluaescaper
  return escaper(replacekey(s,t,how,recurse))
end
local single=P("%") 
local double=P("%%") 
local lquoted=P("%[") 
local rquoted=P("]%") 
local lquotedq=P("%(") 
local rquotedq=P(")%") 
local escape=double/'%%'
local nosingle=single/''
local nodouble=double/''
local nolquoted=lquoted/''
local norquoted=rquoted/''
local nolquotedq=lquotedq/''
local norquotedq=rquotedq/''
local key=nosingle*((C((1-nosingle )^1)*Carg(1)*Carg(2)*Carg(3))/replacekey    )*nosingle
local quoted=nolquotedq*((C((1-norquotedq)^1)*Carg(1)*Carg(2)*Carg(3))/replacekeyquoted )*norquotedq
local unquoted=nolquoted*((C((1-norquoted )^1)*Carg(1)*Carg(2)*Carg(3))/replacekeyunquoted)*norquoted
local any=P(1)
   replacer=Cs((unquoted+quoted+escape+key+any)^0)
local function replace(str,mapping,how,recurse)
  if mapping and str then
    return lpegmatch(replacer,str,1,mapping,how or "lua",recurse or false) or str
  else
    return str
  end
end
templates.replace=replace
function templates.load(filename,mapping,how,recurse)
  local data=io.loaddata(filename) or ""
  if mapping and next(mapping) then
    return replace(data,mapping,how,recurse)
  else
    return data
  end
end
function templates.resolve(t,mapping,how,recurse)
  if not mapping then
    mapping=t
  end
  for k,v in next,t do
    t[k]=replace(v,mapping,how,recurse)
  end
  return t
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-env"] = package.loaded["util-env"] or true

-- original size: 7702, stripped down to: 4701

if not modules then modules={} end modules ['util-env']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local allocate,mark=utilities.storage.allocate,utilities.storage.mark
local format,sub,match,gsub,find=string.format,string.sub,string.match,string.gsub,string.find
local unquoted,quoted=string.unquoted,string.quoted
local concat,insert,remove=table.concat,table.insert,table.remove
environment=environment or {}
local environment=environment
os.setlocale(nil,nil) 
function os.setlocale()
end
local validengines=allocate {
  ["luatex"]=true,
  ["luajittex"]=true,
}
local basicengines=allocate {
  ["luatex"]="luatex",
  ["texlua"]="luatex",
  ["texluac"]="luatex",
  ["luajittex"]="luajittex",
  ["texluajit"]="luajittex",
}
local luaengines=allocate {
  ["lua"]=true,
  ["luajit"]=true,
}
environment.validengines=validengines
environment.basicengines=basicengines
if not arg then
elseif luaengines[file.removesuffix(arg[-1])] then
elseif validengines[file.removesuffix(arg[0])] then
  if arg[1]=="--luaonly" then
    arg[-1]=arg[0]
    arg[ 0]=arg[2]
    for k=3,#arg do
      arg[k-2]=arg[k]
    end
    remove(arg) 
    remove(arg) 
  else
  end
  local originalzero=file.basename(arg[0])
  local specialmapping={ luatools=="base" }
  if originalzero~="mtxrun" and originalzero~="mtxrun.lua" then
    arg[0]=specialmapping[originalzero] or originalzero
    insert(arg,0,"--script")
    insert(arg,0,"mtxrun")
  end
end
environment.arguments=allocate()
environment.files=allocate()
environment.sortedflags=nil
function environment.initializearguments(arg)
  local arguments,files={},{}
  environment.arguments,environment.files,environment.sortedflags=arguments,files,nil
  for index=1,#arg do
    local argument=arg[index]
    if index>0 then
      local flag,value=match(argument,"^%-+(.-)=(.-)$")
      if flag then
        flag=gsub(flag,"^c:","")
        arguments[flag]=unquoted(value or "")
      else
        flag=match(argument,"^%-+(.+)")
        if flag then
          flag=gsub(flag,"^c:","")
          arguments[flag]=true
        else
          files[#files+1]=argument
        end
      end
    end
  end
  environment.ownname=file.reslash(environment.ownname or arg[0] or 'unknown.lua')
end
function environment.setargument(name,value)
  environment.arguments[name]=value
end
function environment.getargument(name,partial)
  local arguments,sortedflags=environment.arguments,environment.sortedflags
  if arguments[name] then
    return arguments[name]
  elseif partial then
    if not sortedflags then
      sortedflags=allocate(table.sortedkeys(arguments))
      for k=1,#sortedflags do
        sortedflags[k]="^"..sortedflags[k]
      end
      environment.sortedflags=sortedflags
    end
    for k=1,#sortedflags do
      local v=sortedflags[k]
      if find(name,v) then
        return arguments[sub(v,2,#v)]
      end
    end
  end
  return nil
end
environment.argument=environment.getargument
function environment.splitarguments(separator) 
  local done,before,after=false,{},{}
  local originalarguments=environment.originalarguments
  for k=1,#originalarguments do
    local v=originalarguments[k]
    if not done and v==separator then
      done=true
    elseif done then
      after[#after+1]=v
    else
      before[#before+1]=v
    end
  end
  return before,after
end
function environment.reconstructcommandline(arg,noquote)
  arg=arg or environment.originalarguments
  if noquote and #arg==1 then
    local a=arg[1]
    a=resolvers.resolve(a)
    a=unquoted(a)
    return a
  elseif #arg>0 then
    local result={}
    for i=1,#arg do
      local a=arg[i]
      a=resolvers.resolve(a)
      a=unquoted(a)
      a=gsub(a,'"','\\"') 
      if find(a," ") then
        result[#result+1]=quoted(a)
      else
        result[#result+1]=a
      end
    end
    return concat(result," ")
  else
    return ""
  end
end
if arg then
  local newarg,instring={},false
  for index=1,#arg do
    local argument=arg[index]
    if find(argument,"^\"") then
      newarg[#newarg+1]=gsub(argument,"^\"","")
      if not find(argument,"\"$") then
        instring=true
      end
    elseif find(argument,"\"$") then
      newarg[#newarg]=newarg[#newarg].." "..gsub(argument,"\"$","")
      instring=false
    elseif instring then
      newarg[#newarg]=newarg[#newarg].." "..argument
    else
      newarg[#newarg+1]=argument
    end
  end
  for i=1,-5,-1 do
    newarg[i]=arg[i]
  end
  environment.initializearguments(newarg)
  environment.originalarguments=mark(newarg)
  environment.rawarguments=mark(arg)
  arg={} 
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["luat-env"] = package.loaded["luat-env"] or true

-- original size: 5874, stripped down to: 4184

 if not modules then modules={} end modules ['luat-env']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local rawset,rawget,loadfile,assert=rawset,rawget,loadfile,assert
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_lua=logs.reporter("resolvers","lua")
local luautilities=utilities.lua
local luasuffixes=luautilities.suffixes
environment=environment or {}
local environment=environment
local mt={
  __index=function(_,k)
    if k=="version" then
      local version=tex.toks and tex.toks.contextversiontoks
      if version and version~="" then
        rawset(environment,"version",version)
        return version
      else
        return "unknown"
      end
    elseif k=="kind" then
      local kind=tex.toks and tex.toks.contextkindtoks
      if kind and kind~="" then
        rawset(environment,"kind",kind)
        return kind
      else
        return "unknown"
      end
    elseif k=="jobname" or k=="formatname" then
      local name=tex and tex[k]
      if name or name=="" then
        rawset(environment,k,name)
        return name
      else
        return "unknown"
      end
    elseif k=="outputfilename" then
      local name=environment.jobname
      rawset(environment,k,name)
      return name
    end
  end
}
setmetatable(environment,mt)
function environment.texfile(filename)
  return resolvers.findfile(filename,'tex')
end
function environment.luafile(filename) 
  local resolved=resolvers.findfile(filename,'tex') or ""
  if resolved~="" then
    return resolved
  end
  resolved=resolvers.findfile(filename,'texmfscripts') or ""
  if resolved~="" then
    return resolved
  end
  return resolvers.findfile(filename,'luatexlibs') or ""
end
local stripindeed=false directives.register("system.compile.strip",function(v) stripindeed=v end)
local function strippable(filename)
  if stripindeed then
    local modu=modules[file.nameonly(filename)]
    return modu and modu.dataonly
  else
    return false
  end
end
function environment.luafilechunk(filename,silent) 
  filename=file.replacesuffix(filename,"lua")
  local fullname=environment.luafile(filename)
  if fullname and fullname~="" then
    local data=luautilities.loadedluacode(fullname,strippable,filename) 
    if trace_locating then
      report_lua("loading file %a %s",fullname,not data and "failed" or "succeeded")
    elseif not silent then
      texio.write("<",data and "+ " or "- ",fullname,">")
    end
    return data
  else
    if trace_locating then
      report_lua("unknown file %a",filename)
    end
    return nil
  end
end
function environment.loadluafile(filename,version)
  local lucname,luaname,chunk
  local basename=file.removesuffix(filename)
  if basename==filename then
    luaname=file.addsuffix(basename,luasuffixes.lua)
    lucname=file.addsuffix(basename,luasuffixes.luc)
  else
    luaname=basename 
    lucname=nil
  end
  local fullname=(lucname and environment.luafile(lucname)) or ""
  if fullname~="" then
    if trace_locating then
      report_lua("loading %a",fullname)
    end
    chunk=loadfile(fullname) 
  end
  if chunk then
    assert(chunk)()
    if version then
      local v=version 
      if modules and modules[filename] then
        v=modules[filename].version 
      elseif versions and versions[filename] then
        v=versions[filename]    
      end
      if v==version then
        return true
      else
        if trace_locating then
          report_lua("version mismatch for %a, lua version %a, luc version %a",filename,v,version)
        end
        environment.loadluafile(filename)
      end
    else
      return true
    end
  end
  fullname=(luaname and environment.luafile(luaname)) or ""
  if fullname~="" then
    if trace_locating then
      report_lua("loading %a",fullname)
    end
    chunk=loadfile(fullname) 
    if not chunk then
      if trace_locating then
        report_lua("unknown file %a",filename)
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

package.loaded["lxml-tab"] = package.loaded["lxml-tab"] or true

-- original size: 42495, stripped down to: 26647

if not modules then modules={} end modules ['lxml-tab']={
  version=1.001,
  comment="this module is the basis for the lxml-* ones",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local trace_entities=false trackers.register("xml.entities",function(v) trace_entities=v end)
local report_xml=logs and logs.reporter("xml","core") or function(...) print(string.format(...)) end
xml=xml or {}
local xml=xml
local concat,remove,insert=table.concat,table.remove,table.insert
local type,next,setmetatable,getmetatable,tonumber=type,next,setmetatable,getmetatable,tonumber
local lower,find,match,gsub=string.lower,string.find,string.match,string.gsub
local utfchar=utf.char
local lpegmatch=lpeg.match
local P,S,R,C,V,C,Cs=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.V,lpeg.C,lpeg.Cs
local formatters=string.formatters
xml.xmlns=xml.xmlns or {}
local check=P(false)
local parse=check
function xml.registerns(namespace,pattern) 
  check=check+C(P(lower(pattern)))/namespace
  parse=P { P(check)+1*V(1) }
end
function xml.checkns(namespace,url)
  local ns=lpegmatch(parse,lower(url))
  if ns and namespace~=ns then
    xml.xmlns[namespace]=ns
  end
end
function xml.resolvens(url)
   return lpegmatch(parse,lower(url)) or ""
end
local nsremap,resolvens=xml.xmlns,xml.resolvens
local stack={}
local top={}
local dt={}
local at={}
local xmlns={}
local errorstr=nil
local entities={}
local strip=false
local cleanup=false
local utfize=false
local resolve_predefined=false
local unify_predefined=false
local dcache={}
local hcache={}
local acache={}
local mt={}
local function initialize_mt(root)
  mt={ __index=root } 
end
function xml.setproperty(root,k,v)
  getmetatable(root).__index[k]=v
end
function xml.checkerror(top,toclose)
  return "" 
end
local function add_attribute(namespace,tag,value)
  if cleanup and #value>0 then
    value=cleanup(value) 
  end
  if tag=="xmlns" then
    xmlns[#xmlns+1]=resolvens(value)
    at[tag]=value
  elseif namespace=="" then
    at[tag]=value
  elseif namespace=="xmlns" then
    xml.checkns(tag,value)
    at["xmlns:"..tag]=value
  else
    at[namespace..":"..tag]=value
  end
end
local function add_empty(spacing,namespace,tag)
  if #spacing>0 then
    dt[#dt+1]=spacing
  end
  local resolved=namespace=="" and xmlns[#xmlns] or nsremap[namespace] or namespace
  top=stack[#stack]
  dt=top.dt
  local t={ ns=namespace or "",rn=resolved,tg=tag,at=at,dt={},__p__=top }
  dt[#dt+1]=t
  setmetatable(t,mt)
  if at.xmlns then
    remove(xmlns)
  end
  at={}
end
local function add_begin(spacing,namespace,tag)
  if #spacing>0 then
    dt[#dt+1]=spacing
  end
  local resolved=namespace=="" and xmlns[#xmlns] or nsremap[namespace] or namespace
  top={ ns=namespace or "",rn=resolved,tg=tag,at=at,dt={},__p__=stack[#stack] }
  setmetatable(top,mt)
  dt=top.dt
  stack[#stack+1]=top
  at={}
end
local function add_end(spacing,namespace,tag)
  if #spacing>0 then
    dt[#dt+1]=spacing
  end
  local toclose=remove(stack)
  top=stack[#stack]
  if #stack<1 then
    errorstr=formatters["unable to close %s %s"](tag,xml.checkerror(top,toclose) or "")
  elseif toclose.tg~=tag then 
    errorstr=formatters["unable to close %s with %s %s"](toclose.tg,tag,xml.checkerror(top,toclose) or "")
  end
  dt=top.dt
  dt[#dt+1]=toclose
  if toclose.at.xmlns then
    remove(xmlns)
  end
end
local function add_text(text)
  if cleanup and #text>0 then
    dt[#dt+1]=cleanup(text)
  else
    dt[#dt+1]=text
  end
end
local function add_special(what,spacing,text)
  if #spacing>0 then
    dt[#dt+1]=spacing
  end
  if strip and (what=="@cm@" or what=="@dt@") then
  else
    dt[#dt+1]={ special=true,ns="",tg=what,dt={ text } }
  end
end
local function set_message(txt)
  errorstr="garbage at the end of the file: "..gsub(txt,"([ \n\r\t]*)","")
end
local reported_attribute_errors={}
local function attribute_value_error(str)
  if not reported_attribute_errors[str] then
    report_xml("invalid attribute value %a",str)
    reported_attribute_errors[str]=true
    at._error_=str
  end
  return str
end
local function attribute_specification_error(str)
  if not reported_attribute_errors[str] then
    report_xml("invalid attribute specification %a",str)
    reported_attribute_errors[str]=true
    at._error_=str
  end
  return str
end
xml.placeholders={
  unknown_dec_entity=function(str) return str=="" and "&error;" or formatters["&%s;"](str) end,
  unknown_hex_entity=function(str) return formatters["&#x%s;"](str) end,
  unknown_any_entity=function(str) return formatters["&#x%s;"](str) end,
}
local placeholders=xml.placeholders
local function fromhex(s)
  local n=tonumber(s,16)
  if n then
    return utfchar(n)
  else
    return formatters["h:%s"](s),true
  end
end
local function fromdec(s)
  local n=tonumber(s)
  if n then
    return utfchar(n)
  else
    return formatters["d:%s"](s),true
  end
end
local rest=(1-P(";"))^0
local many=P(1)^0
local parsedentity=P("&")*(P("#x")*(rest/fromhex)+P("#")*(rest/fromdec))*P(";")*P(-1)+(P("#x")*(many/fromhex)+P("#")*(many/fromdec))
local predefined_unified={
  [38]="&amp;",
  [42]="&quot;",
  [47]="&apos;",
  [74]="&lt;",
  [76]="&gt;",
}
local predefined_simplified={
  [38]="&",amp="&",
  [42]='"',quot='"',
  [47]="'",apos="'",
  [74]="<",lt="<",
  [76]=">",gt=">",
}
local nofprivates=0xF0000 
local privates_u={ 
  [ [[&]] ]="&amp;",
  [ [["]] ]="&quot;",
  [ [[']] ]="&apos;",
  [ [[<]] ]="&lt;",
  [ [[>]] ]="&gt;",
}
local privates_p={}
local privates_n={
}
local escaped=utf.remapper(privates_u)
local function unescaped(s)
  local p=privates_n[s]
  if not p then
    nofprivates=nofprivates+1
    p=utfchar(nofprivates)
    privates_n[s]=p
    s="&"..s..";" 
    privates_u[p]=s
    privates_p[p]=s
  end
  return p
end
local unprivatized=utf.remapper(privates_p)
xml.privatetoken=unescaped
xml.unprivatized=unprivatized
xml.privatecodes=privates_n
local function handle_hex_entity(str)
  local h=hcache[str]
  if not h then
    local n=tonumber(str,16)
    h=unify_predefined and predefined_unified[n]
    if h then
      if trace_entities then
        report_xml("utfize, converting hex entity &#x%s; into %a",str,h)
      end
    elseif utfize then
      h=(n and utfchar(n)) or xml.unknown_hex_entity(str) or ""
      if not n then
        report_xml("utfize, ignoring hex entity &#x%s;",str)
      elseif trace_entities then
        report_xml("utfize, converting hex entity &#x%s; into %a",str,h)
      end
    else
      if trace_entities then
        report_xml("found entity &#x%s;",str)
      end
      h="&#x"..str..";"
    end
    hcache[str]=h
  end
  return h
end
local function handle_dec_entity(str)
  local d=dcache[str]
  if not d then
    local n=tonumber(str)
    d=unify_predefined and predefined_unified[n]
    if d then
      if trace_entities then
        report_xml("utfize, converting dec entity &#%s; into %a",str,d)
      end
    elseif utfize then
      d=(n and utfchar(n)) or placeholders.unknown_dec_entity(str) or ""
      if not n then
        report_xml("utfize, ignoring dec entity &#%s;",str)
      elseif trace_entities then
        report_xml("utfize, converting dec entity &#%s; into %a",str,d)
      end
    else
      if trace_entities then
        report_xml("found entity &#%s;",str)
      end
      d="&#"..str..";"
    end
    dcache[str]=d
  end
  return d
end
xml.parsedentitylpeg=parsedentity
local function handle_any_entity(str)
  if resolve then
    local a=acache[str] 
    if not a then
      a=resolve_predefined and predefined_simplified[str]
      if a then
        if trace_entities then
          report_xml("resolving entity &%s; to predefined %a",str,a)
        end
      else
        if type(resolve)=="function" then
          a=resolve(str) or entities[str]
        else
          a=entities[str]
        end
        if a then
          if type(a)=="function" then
            if trace_entities then
              report_xml("expanding entity &%s; to function call",str)
            end
            a=a(str) or ""
          end
          a=lpegmatch(parsedentity,a) or a 
          if trace_entities then
            report_xml("resolving entity &%s; to internal %a",str,a)
          end
        else
          local unknown_any_entity=placeholders.unknown_any_entity
          if unknown_any_entity then
            a=unknown_any_entity(str) or ""
          end
          if a then
            if trace_entities then
              report_xml("resolving entity &%s; to external %s",str,a)
            end
          else
            if trace_entities then
              report_xml("keeping entity &%s;",str)
            end
            if str=="" then
              a="&error;"
            else
              a="&"..str..";"
            end
          end
        end
      end
      acache[str]=a
    elseif trace_entities then
      if not acache[str] then
        report_xml("converting entity &%s; to %a",str,a)
        acache[str]=a
      end
    end
    return a
  else
    local a=acache[str]
    if not a then
      a=resolve_predefined and predefined_simplified[str]
      if a then
        acache[str]=a
        if trace_entities then
          report_xml("entity &%s; becomes %a",str,a)
        end
      elseif str=="" then
        if trace_entities then
          report_xml("invalid entity &%s;",str)
        end
        a="&error;"
        acache[str]=a
      else
        if trace_entities then
          report_xml("entity &%s; is made private",str)
        end
        a=unescaped(str)
        acache[str]=a
      end
    end
    return a
  end
end
local function handle_end_entity(chr)
  report_xml("error in entity, %a found instead of %a",chr,";")
end
local space=S(' \r\n\t')
local open=P('<')
local close=P('>')
local squote=S("'")
local dquote=S('"')
local equal=P('=')
local slash=P('/')
local colon=P(':')
local semicolon=P(';')
local ampersand=P('&')
local valid=R('az','AZ','09')+S('_-.')
local name_yes=C(valid^1)*colon*C(valid^1)
local name_nop=C(P(true))*C(valid^1)
local name=name_yes+name_nop
local utfbom=lpeg.patterns.utfbom 
local spacing=C(space^0)
local anyentitycontent=(1-open-semicolon-space-close)^0
local hexentitycontent=R("AF","af","09")^0
local decentitycontent=R("09")^0
local parsedentity=P("#")/""*(
                P("x")/""*(hexentitycontent/handle_hex_entity)+(decentitycontent/handle_dec_entity)
              )+(anyentitycontent/handle_any_entity)
local entity=ampersand/""*parsedentity*((semicolon/"")+#(P(1)/handle_end_entity))
local text_unparsed=C((1-open)^1)
local text_parsed=Cs(((1-open-ampersand)^1+entity)^1)
local somespace=space^1
local optionalspace=space^0
local value=(squote*Cs((entity+(1-squote))^0)*squote)+(dquote*Cs((entity+(1-dquote))^0)*dquote) 
local endofattributes=slash*close+close 
local whatever=space*name*optionalspace*equal
local wrongvalue=Cs(P(entity+(1-space-endofattributes))^1)/attribute_value_error
local attributevalue=value+wrongvalue
local attribute=(somespace*name*optionalspace*equal*optionalspace*attributevalue)/add_attribute
local attributes=(attribute+somespace^-1*(((1-endofattributes)^1)/attribute_specification_error))^0
local parsedtext=text_parsed/add_text
local unparsedtext=text_unparsed/add_text
local balanced=P { "["*((1-S"[]")+V(1))^0*"]" } 
local emptyelement=(spacing*open*name*attributes*optionalspace*slash*close)/add_empty
local beginelement=(spacing*open*name*attributes*optionalspace*close)/add_begin
local endelement=(spacing*open*slash*name*optionalspace*close)/add_end
local begincomment=open*P("!--")
local endcomment=P("--")*close
local begininstruction=open*P("?")
local endinstruction=P("?")*close
local begincdata=open*P("![CDATA[")
local endcdata=P("]]")*close
local someinstruction=C((1-endinstruction)^0)
local somecomment=C((1-endcomment  )^0)
local somecdata=C((1-endcdata   )^0)
local function normalentity(k,v ) entities[k]=v end
local function systementity(k,v,n) entities[k]=v end
local function publicentity(k,v,n) entities[k]=v end
local begindoctype=open*P("!DOCTYPE")
local enddoctype=close
local beginset=P("[")
local endset=P("]")
local doctypename=C((1-somespace-close)^0)
local elementdoctype=optionalspace*P("<!ELEMENT")*(1-close)^0*close
local basiccomment=begincomment*((1-endcomment)^0)*endcomment
local normalentitytype=(doctypename*somespace*value)/normalentity
local publicentitytype=(doctypename*somespace*P("PUBLIC")*somespace*value)/publicentity
local systementitytype=(doctypename*somespace*P("SYSTEM")*somespace*value*somespace*P("NDATA")*somespace*doctypename)/systementity
local entitydoctype=optionalspace*P("<!ENTITY")*somespace*(systementitytype+publicentitytype+normalentitytype)*optionalspace*close
local doctypeset=beginset*optionalspace*P(elementdoctype+entitydoctype+basiccomment+space)^0*optionalspace*endset
local definitiondoctype=doctypename*somespace*doctypeset
local publicdoctype=doctypename*somespace*P("PUBLIC")*somespace*value*somespace*value*somespace*doctypeset
local systemdoctype=doctypename*somespace*P("SYSTEM")*somespace*value*somespace*doctypeset
local simpledoctype=(1-close)^1 
local somedoctype=C((somespace*(publicdoctype+systemdoctype+definitiondoctype+simpledoctype)*optionalspace)^0)
local somedoctype=C((somespace*(publicdoctype+systemdoctype+definitiondoctype+simpledoctype)*optionalspace)^0)
local instruction=(spacing*begininstruction*someinstruction*endinstruction)/function(...) add_special("@pi@",...) end
local comment=(spacing*begincomment*somecomment*endcomment  )/function(...) add_special("@cm@",...) end
local cdata=(spacing*begincdata*somecdata*endcdata   )/function(...) add_special("@cd@",...) end
local doctype=(spacing*begindoctype*somedoctype*enddoctype  )/function(...) add_special("@dt@",...) end
local trailer=space^0*(text_unparsed/set_message)^0
local grammar_parsed_text=P { "preamble",
  preamble=utfbom^0*instruction^0*(doctype+comment+instruction)^0*V("parent")*trailer,
  parent=beginelement*V("children")^0*endelement,
  children=parsedtext+V("parent")+emptyelement+comment+cdata+instruction,
}
local grammar_unparsed_text=P { "preamble",
  preamble=utfbom^0*instruction^0*(doctype+comment+instruction)^0*V("parent")*trailer,
  parent=beginelement*V("children")^0*endelement,
  children=unparsedtext+V("parent")+emptyelement+comment+cdata+instruction,
}
local function _xmlconvert_(data,settings)
  settings=settings or {}
  strip=settings.strip_cm_and_dt
  utfize=settings.utfize_entities
  resolve=settings.resolve_entities
  resolve_predefined=settings.resolve_predefined_entities 
  unify_predefined=settings.unify_predefined_entities 
  cleanup=settings.text_cleanup
  entities=settings.entities or {}
  if utfize==nil then
    settings.utfize_entities=true
    utfize=true
  end
  if resolve_predefined==nil then
    settings.resolve_predefined_entities=true
    resolve_predefined=true
  end
  stack,top,at,xmlns,errorstr={},{},{},{},nil
  acache,hcache,dcache={},{},{} 
  reported_attribute_errors={}
  if settings.parent_root then
    mt=getmetatable(settings.parent_root)
  else
    initialize_mt(top)
  end
  stack[#stack+1]=top
  top.dt={}
  dt=top.dt
  if not data or data=="" then
    errorstr="empty xml file"
  elseif utfize or resolve then
    if lpegmatch(grammar_parsed_text,data) then
      errorstr=""
    else
      errorstr="invalid xml file - parsed text"
    end
  elseif type(data)=="string" then
    if lpegmatch(grammar_unparsed_text,data) then
      errorstr=""
    else
      errorstr="invalid xml file - unparsed text"
    end
  else
    errorstr="invalid xml file - no text at all"
  end
  local result
  if errorstr and errorstr~="" then
    result={ dt={ { ns="",tg="error",dt={ errorstr },at={},er=true } } }
    setmetatable(stack,mt)
    local errorhandler=settings.error_handler
    if errorhandler==false then
    else
      errorhandler=errorhandler or xml.errorhandler
      if errorhandler then
        local currentresource=settings.currentresource
        if currentresource and currentresource~="" then
          xml.errorhandler(formatters["load error in [%s]: %s"](currentresource,errorstr))
        else
          xml.errorhandler(formatters["load error: %s"](errorstr))
        end
      end
    end
  else
    result=stack[1]
  end
  if not settings.no_root then
    result={ special=true,ns="",tg='@rt@',dt=result.dt,at={},entities=entities,settings=settings }
    setmetatable(result,mt)
    local rdt=result.dt
    for k=1,#rdt do
      local v=rdt[k]
      if type(v)=="table" and not v.special then 
        result.ri=k 
        v.__p__=result 
        break
      end
    end
  end
  if errorstr and errorstr~="" then
    result.error=true
  end
  result.statistics={
    entities={
      decimals=dcache,
      hexadecimals=hcache,
      names=acache,
    }
  }
  strip,utfize,resolve,resolve_predefined=nil,nil,nil,nil
  unify_predefined,cleanup,entities=nil,nil,nil
  stack,top,at,xmlns,errorstr=nil,nil,nil,nil,nil
  acache,hcache,dcache=nil,nil,nil
  reported_attribute_errors,mt,errorhandler=nil,nil,nil
  return result
end
function xmlconvert(data,settings)
  local ok,result=pcall(function() return _xmlconvert_(data,settings) end)
  if ok then
    return result
  else
    return _xmlconvert_("",settings)
  end
end
xml.convert=xmlconvert
function xml.inheritedconvert(data,xmldata) 
  local settings=xmldata.settings
  if settings then
    settings.parent_root=xmldata 
  end
  local xc=xmlconvert(data,settings)
  return xc
end
function xml.is_valid(root)
  return root and root.dt and root.dt[1] and type(root.dt[1])=="table" and not root.dt[1].er
end
function xml.package(tag,attributes,data)
  local ns,tg=match(tag,"^(.-):?([^:]+)$")
  local t={ ns=ns,tg=tg,dt=data or "",at=attributes or {} }
  setmetatable(t,mt)
  return t
end
function xml.is_valid(root)
  return root and not root.error
end
xml.errorhandler=report_xml
function xml.load(filename,settings)
  local data=""
  if type(filename)=="string" then
    local f=io.open(filename,'r') 
    if f then
      data=f:read("*all") 
      f:close()
    end
  elseif filename then 
    data=filename:read("*all") 
  end
  if settings then
    settings.currentresource=filename
    local result=xmlconvert(data,settings)
    settings.currentresource=nil
    return result
  else
    return xmlconvert(data,{ currentresource=filename })
  end
end
local no_root={ no_root=true }
function xml.toxml(data)
  if type(data)=="string" then
    local root={ xmlconvert(data,no_root) }
    return (#root>1 and root) or root[1]
  else
    return data
  end
end
local function copy(old,tables)
  if old then
    tables=tables or {}
    local new={}
    if not tables[old] then
      tables[old]=new
    end
    for k,v in next,old do
      new[k]=(type(v)=="table" and (tables[v] or copy(v,tables))) or v
    end
    local mt=getmetatable(old)
    if mt then
      setmetatable(new,mt)
    end
    return new
  else
    return {}
  end
end
xml.copy=copy
function xml.checkbom(root) 
  if root.ri then
    local dt=root.dt
    for k=1,#dt do
      local v=dt[k]
      if type(v)=="table" and v.special and v.tg=="@pi@" and find(v.dt[1],"xml.*version=") then
        return
      end
    end
    insert(dt,1,{ special=true,ns="",tg="@pi@",dt={ "xml version='1.0' standalone='yes'" } } )
    insert(dt,2,"\n" )
  end
end
local function verbose_element(e,handlers) 
  local handle=handlers.handle
  local serialize=handlers.serialize
  local ens,etg,eat,edt,ern=e.ns,e.tg,e.at,e.dt,e.rn
  local ats=eat and next(eat) and {}
  if ats then
    for k,v in next,eat do
      ats[#ats+1]=formatters['%s=%q'](k,escaped(v))
    end
  end
  if ern and trace_entities and ern~=ens then
    ens=ern
  end
  if ens~="" then
    if edt and #edt>0 then
      if ats then
        handle("<",ens,":",etg," ",concat(ats," "),">")
      else
        handle("<",ens,":",etg,">")
      end
      for i=1,#edt do
        local e=edt[i]
        if type(e)=="string" then
          handle(escaped(e))
        else
          serialize(e,handlers)
        end
      end
      handle("</",ens,":",etg,">")
    else
      if ats then
        handle("<",ens,":",etg," ",concat(ats," "),"/>")
      else
        handle("<",ens,":",etg,"/>")
      end
    end
  else
    if edt and #edt>0 then
      if ats then
        handle("<",etg," ",concat(ats," "),">")
      else
        handle("<",etg,">")
      end
      for i=1,#edt do
        local e=edt[i]
        if type(e)=="string" then
          handle(escaped(e)) 
        else
          serialize(e,handlers)
        end
      end
      handle("</",etg,">")
    else
      if ats then
        handle("<",etg," ",concat(ats," "),"/>")
      else
        handle("<",etg,"/>")
      end
    end
  end
end
local function verbose_pi(e,handlers)
  handlers.handle("<?",e.dt[1],"?>")
end
local function verbose_comment(e,handlers)
  handlers.handle("<!--",e.dt[1],"-->")
end
local function verbose_cdata(e,handlers)
  handlers.handle("<![CDATA[",e.dt[1],"]]>")
end
local function verbose_doctype(e,handlers)
  handlers.handle("<!DOCTYPE ",e.dt[1],">")
end
local function verbose_root(e,handlers)
  handlers.serialize(e.dt,handlers)
end
local function verbose_text(e,handlers)
  handlers.handle(escaped(e))
end
local function verbose_document(e,handlers)
  local serialize=handlers.serialize
  local functions=handlers.functions
  for i=1,#e do
    local ei=e[i]
    if type(ei)=="string" then
      functions["@tx@"](ei,handlers)
    else
      serialize(ei,handlers)
    end
  end
end
local function serialize(e,handlers,...)
  local initialize=handlers.initialize
  local finalize=handlers.finalize
  local functions=handlers.functions
  if initialize then
    local state=initialize(...)
    if not state==true then
      return state
    end
  end
  local etg=e.tg
  if etg then
    (functions[etg] or functions["@el@"])(e,handlers)
  else
    functions["@dc@"](e,handlers) 
  end
  if finalize then
    return finalize()
  end
end
local function xserialize(e,handlers)
  local functions=handlers.functions
  local etg=e.tg
  if etg then
    (functions[etg] or functions["@el@"])(e,handlers)
  else
    functions["@dc@"](e,handlers)
  end
end
local handlers={}
local function newhandlers(settings)
  local t=table.copy(handlers[settings and settings.parent or "verbose"] or {}) 
  if settings then
    for k,v in next,settings do
      if type(v)=="table" then
        local tk=t[k] if not tk then tk={} t[k]=tk end
        for kk,vv in next,v do
          tk[kk]=vv
        end
      else
        t[k]=v
      end
    end
    if settings.name then
      handlers[settings.name]=t
    end
  end
  utilities.storage.mark(t)
  return t
end
local nofunction=function() end
function xml.sethandlersfunction(handler,name,fnc)
  handler.functions[name]=fnc or nofunction
end
function xml.gethandlersfunction(handler,name)
  return handler.functions[name]
end
function xml.gethandlers(name)
  return handlers[name]
end
newhandlers {
  name="verbose",
  initialize=false,
  finalize=false,
  serialize=xserialize,
  handle=print,
  functions={
    ["@dc@"]=verbose_document,
    ["@dt@"]=verbose_doctype,
    ["@rt@"]=verbose_root,
    ["@el@"]=verbose_element,
    ["@pi@"]=verbose_pi,
    ["@cm@"]=verbose_comment,
    ["@cd@"]=verbose_cdata,
    ["@tx@"]=verbose_text,
  }
}
local result
local xmlfilehandler=newhandlers {
  name="file",
  initialize=function(name)
    result=io.open(name,"wb")
    return result
  end,
  finalize=function()
    result:close()
    return true
  end,
  handle=function(...)
    result:write(...)
  end,
}
function xml.save(root,name)
  serialize(root,xmlfilehandler,name)
end
local result
local xmlstringhandler=newhandlers {
  name="string",
  initialize=function()
    result={}
    return result
  end,
  finalize=function()
    return concat(result)
  end,
  handle=function(...)
    result[#result+1]=concat {... }
  end,
}
local function xmltostring(root) 
  if not root then
    return ""
  elseif type(root)=="string" then
    return root
  else 
    return serialize(root,xmlstringhandler) or ""
  end
end
local function __tostring(root) 
  return (root and xmltostring(root)) or ""
end
initialize_mt=function(root) 
  mt={ __tostring=__tostring,__index=root }
end
xml.defaulthandlers=handlers
xml.newhandlers=newhandlers
xml.serialize=serialize
xml.tostring=xmltostring
local function xmlstring(e,handle)
  if not handle or (e.special and e.tg~="@rt@") then
  elseif e.tg then
    local edt=e.dt
    if edt then
      for i=1,#edt do
        xmlstring(edt[i],handle)
      end
    end
  else
    handle(e)
  end
end
xml.string=xmlstring
function xml.settings(e)
  while e do
    local s=e.settings
    if s then
      return s
    else
      e=e.__p__
    end
  end
  return nil
end
function xml.root(e)
  local r=e
  while e do
    e=e.__p__
    if e then
      r=e
    end
  end
  return r
end
function xml.parent(root)
  return root.__p__
end
function xml.body(root)
  return root.ri and root.dt[root.ri] or root 
end
function xml.name(root)
  if not root then
    return ""
  end
  local ns=root.ns
  local tg=root.tg
  if ns=="" then
    return tg
  else
    return ns..":"..tg
  end
end
function xml.erase(dt,k)
  if dt then
    if k then
      dt[k]=""
    else for k=1,#dt do
      dt[1]={ "" }
    end end
  end
end
function xml.assign(dt,k,root)
  if dt and k then
    dt[k]=type(root)=="table" and xml.body(root) or root
    return dt[k]
  else
    return xml.body(root)
  end
end
function xml.tocdata(e,wrapper) 
  local whatever=type(e)=="table" and xmltostring(e.dt) or e or ""
  if wrapper then
    whatever=formatters["<%s>%s</%s>"](wrapper,whatever,wrapper)
  end
  local t={ special=true,ns="",tg="@cd@",at={},rn="",dt={ whatever },__p__=e }
  setmetatable(t,getmetatable(e))
  e.dt={ t }
end
function xml.makestandalone(root)
  if root.ri then
    local dt=root.dt
    for k=1,#dt do
      local v=dt[k]
      if type(v)=="table" and v.special and v.tg=="@pi@" then
        local txt=v.dt[1]
        if find(txt,"xml.*version=") then
          v.dt[1]=txt.." standalone='yes'"
          break
        end
      end
    end
  end
  return root
end
function xml.kind(e)
  local dt=e and e.dt
  if dt then
    local n=#dt
    if n==1 then
      local d=dt[1]
      if d.special then
        local tg=d.tg
        if tg=="@cd@" then
          return "cdata"
        elseif tg=="@cm" then
          return "comment"
        elseif tg=="@pi@" then
          return "instruction"
        elseif tg=="@dt@" then
          return "declaration"
        end
      elseif type(d)=="string" then
        return "text"
      end
      return "element"
    elseif n>0 then
      return "mixed"
    end
  end
  return "empty"
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["lxml-lpt"] = package.loaded["lxml-lpt"] or true

-- original size: 48956, stripped down to: 30516

if not modules then modules={} end modules ['lxml-lpt']={
  version=1.001,
  comment="this module is the basis for the lxml-* ones",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local concat,remove,insert=table.concat,table.remove,table.insert
local type,next,tonumber,tostring,setmetatable,load,select=type,next,tonumber,tostring,setmetatable,load,select
local format,upper,lower,gmatch,gsub,find,rep=string.format,string.upper,string.lower,string.gmatch,string.gsub,string.find,string.rep
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local setmetatableindex=table.setmetatableindex
local formatters=string.formatters
local trace_lpath=false if trackers then trackers.register("xml.path",function(v) trace_lpath=v end) end
local trace_lparse=false if trackers then trackers.register("xml.parse",function(v) trace_lparse=v end) end
local trace_lprofile=false if trackers then trackers.register("xml.profile",function(v) trace_lpath=v trace_lparse=v trace_lprofile=v end) end
local report_lpath=logs.reporter("xml","lpath")
local xml=xml
local lpathcalls=0 function xml.lpathcalls () return lpathcalls end
local lpathcached=0 function xml.lpathcached() return lpathcached end
xml.functions=xml.functions or {} 
local functions=xml.functions
xml.expressions=xml.expressions or {} 
local expressions=xml.expressions
xml.finalizers=xml.finalizers or {} 
local finalizers=xml.finalizers
xml.specialhandler=xml.specialhandler or {}
local specialhandler=xml.specialhandler
lpegpatterns.xml=lpegpatterns.xml or {}
local xmlpatterns=lpegpatterns.xml
finalizers.xml=finalizers.xml or {}
finalizers.tex=finalizers.tex or {}
local function fallback (t,name)
  local fn=finalizers[name]
  if fn then
    t[name]=fn
  else
    report_lpath("unknown sub finalizer %a",name)
    fn=function() end
  end
  return fn
end
setmetatableindex(finalizers.xml,fallback)
setmetatableindex(finalizers.tex,fallback)
xml.defaultprotocol="xml"
local apply_axis={}
apply_axis['root']=function(list)
  local collected={}
  for l=1,#list do
    local ll=list[l]
    local rt=ll
    while ll do
      ll=ll.__p__
      if ll then
        rt=ll
      end
    end
    collected[l]=rt
  end
  return collected
end
apply_axis['self']=function(list)
  return list
end
apply_axis['child']=function(list)
  local collected,c={},0
  for l=1,#list do
    local ll=list[l]
    local dt=ll.dt
    if dt then 
      local en=0
      for k=1,#dt do
        local dk=dt[k]
        if dk.tg then
          c=c+1
          collected[c]=dk
          dk.ni=k 
          en=en+1
          dk.ei=en
        end
      end
      ll.en=en
    end
  end
  return collected
end
local function collect(list,collected,c)
  local dt=list.dt
  if dt then
    local en=0
    for k=1,#dt do
      local dk=dt[k]
      if dk.tg then
        c=c+1
        collected[c]=dk
        dk.ni=k 
        en=en+1
        dk.ei=en
        c=collect(dk,collected,c)
      end
    end
    list.en=en
  end
  return c
end
apply_axis['descendant']=function(list)
  local collected,c={},0
  for l=1,#list do
    c=collect(list[l],collected,c)
  end
  return collected
end
local function collect(list,collected,c)
  local dt=list.dt
  if dt then
    local en=0
    for k=1,#dt do
      local dk=dt[k]
      if dk.tg then
        c=c+1
        collected[c]=dk
        dk.ni=k 
        en=en+1
        dk.ei=en
        c=collect(dk,collected,c)
      end
    end
    list.en=en
  end
  return c
end
apply_axis['descendant-or-self']=function(list)
  local collected,c={},0
  for l=1,#list do
    local ll=list[l]
    if ll.special~=true then 
      c=c+1
      collected[c]=ll
    end
    c=collect(ll,collected,c)
  end
  return collected
end
apply_axis['ancestor']=function(list)
  local collected,c={},0
  for l=1,#list do
    local ll=list[l]
    while ll do
      ll=ll.__p__
      if ll then
        c=c+1
        collected[c]=ll
      end
    end
  end
  return collected
end
apply_axis['ancestor-or-self']=function(list)
  local collected,c={},0
  for l=1,#list do
    local ll=list[l]
    c=c+1
    collected[c]=ll
    while ll do
      ll=ll.__p__
      if ll then
        c=c+1
        collected[c]=ll
      end
    end
  end
  return collected
end
apply_axis['parent']=function(list)
  local collected,c={},0
  for l=1,#list do
    local pl=list[l].__p__
    if pl then
      c=c+1
      collected[c]=pl
    end
  end
  return collected
end
apply_axis['attribute']=function(list)
  return {}
end
apply_axis['namespace']=function(list)
  return {}
end
apply_axis['following']=function(list)
  return {}
end
apply_axis['preceding']=function(list)
  return {}
end
apply_axis['following-sibling']=function(list)
  local collected,c={},0
  for l=1,#list do
    local ll=list[l]
    local p=ll.__p__
    local d=p.dt
    for i=ll.ni+1,#d do
      local di=d[i]
      if type(di)=="table" then
        c=c+1
        collected[c]=di
      end
    end
  end
  return collected
end
apply_axis['preceding-sibling']=function(list)
  local collected,c={},0
  for l=1,#list do
    local ll=list[l]
    local p=ll.__p__
    local d=p.dt
    for i=1,ll.ni-1 do
      local di=d[i]
      if type(di)=="table" then
        c=c+1
        collected[c]=di
      end
    end
  end
  return collected
end
apply_axis['reverse-sibling']=function(list) 
  local collected,c={},0
  for l=1,#list do
    local ll=list[l]
    local p=ll.__p__
    local d=p.dt
    for i=ll.ni-1,1,-1 do
      local di=d[i]
      if type(di)=="table" then
        c=c+1
        collected[c]=di
      end
    end
  end
  return collected
end
apply_axis['auto-descendant-or-self']=apply_axis['descendant-or-self']
apply_axis['auto-descendant']=apply_axis['descendant']
apply_axis['auto-child']=apply_axis['child']
apply_axis['auto-self']=apply_axis['self']
apply_axis['initial-child']=apply_axis['child']
local function apply_nodes(list,directive,nodes)
  local maxn=#nodes
  if maxn==3 then 
    local nns,ntg=nodes[2],nodes[3]
    if not nns and not ntg then 
      if directive then
        return list
      else
        return {}
      end
    else
      local collected,c,m,p={},0,0,nil
      if not nns then 
        for l=1,#list do
          local ll=list[l]
          local ltg=ll.tg
          if ltg then
            if directive then
              if ntg==ltg then
                local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
                c=c+1
                collected[c],ll.mi=ll,m
              end
            elseif ntg~=ltg then
              local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
              c=c+1
              collected[c],ll.mi=ll,m
            end
          end
        end
      elseif not ntg then 
        for l=1,#list do
          local ll=list[l]
          local lns=ll.rn or ll.ns
          if lns then
            if directive then
              if lns==nns then
                local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
                c=c+1
                collected[c],ll.mi=ll,m
              end
            elseif lns~=nns then
              local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
              c=c+1
              collected[c],ll.mi=ll,m
            end
          end
        end
      else 
        for l=1,#list do
          local ll=list[l]
          local ltg=ll.tg
          if ltg then
            local lns=ll.rn or ll.ns
            local ok=ltg==ntg and lns==nns
            if directive then
              if ok then
                local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
                c=c+1
                collected[c],ll.mi=ll,m
              end
            elseif not ok then
              local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
              c=c+1
              collected[c],ll.mi=ll,m
            end
          end
        end
      end
      return collected
    end
  else
    local collected,c,m,p={},0,0,nil
    for l=1,#list do
      local ll=list[l]
      local ltg=ll.tg
      if ltg then
        local lns=ll.rn or ll.ns
        local ok=false
        for n=1,maxn,3 do
          local nns,ntg=nodes[n+1],nodes[n+2]
          ok=(not ntg or ltg==ntg) and (not nns or lns==nns)
          if ok then
            break
          end
        end
        if directive then
          if ok then
            local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
            c=c+1
            collected[c],ll.mi=ll,m
          end
        elseif not ok then
          local llp=ll.__p__;if llp~=p then p,m=llp,1 else m=m+1 end
          c=c+1
          collected[c],ll.mi=ll,m
        end
      end
    end
    return collected
  end
end
local quit_expression=false
local function apply_expression(list,expression,order)
  local collected,c={},0
  quit_expression=false
  for l=1,#list do
    local ll=list[l]
    if expression(list,ll,l,order) then 
      c=c+1
      collected[c]=ll
    end
    if quit_expression then
      break
    end
  end
  return collected
end
local P,V,C,Cs,Cc,Ct,R,S,Cg,Cb=lpeg.P,lpeg.V,lpeg.C,lpeg.Cs,lpeg.Cc,lpeg.Ct,lpeg.R,lpeg.S,lpeg.Cg,lpeg.Cb
local spaces=S(" \n\r\t\f")^0
local lp_space=S(" \n\r\t\f")
local lp_any=P(1)
local lp_noequal=P("!=")/"~="+P("<=")+P(">=")+P("==")
local lp_doequal=P("=")/"=="
local lp_or=P("|")/" or "
local lp_and=P("&")/" and "
local lp_builtin=P (
    P("text")/"(ll.dt[1] or '')"+
    P("content")/"ll.dt"+
    P("name")/"((ll.ns~='' and ll.ns..':'..ll.tg) or ll.tg)"+P("tag")/"ll.tg"+P("position")/"l"+
    P("firstindex")/"1"+P("lastindex")/"(#ll.__p__.dt or 1)"+P("firstelement")/"1"+P("lastelement")/"(ll.__p__.en or 1)"+P("first")/"1"+P("last")/"#list"+P("rootposition")/"order"+P("order")/"order"+P("element")/"(ll.ei or 1)"+P("index")/"(ll.ni or 1)"+P("match")/"(ll.mi or 1)"+
    P("ns")/"ll.ns"
  )*((spaces*P("(")*spaces*P(")"))/"")
local lp_attribute=(P("@")+P("attribute::"))/""*Cc("(ll.at and ll.at['")*((R("az","AZ")+S("-_:"))^1)*Cc("'])")
lp_fastpos_p=P("+")^0*R("09")^1*P(-1)/"l==%0"
lp_fastpos_n=P("-")*R("09")^1*P(-1)/"(%0<0 and (#list+%0==l))"
local lp_fastpos=lp_fastpos_n+lp_fastpos_p
local lp_reserved=C("and")+C("or")+C("not")+C("div")+C("mod")+C("true")+C("false")
local lp_lua_function=Cs((R("az","AZ","__")^1*(P(".")*R("az","AZ","__")^1)^1)*("("))/"%0"
local lp_function=C(R("az","AZ","__")^1)*P("(")/function(t) 
  if expressions[t] then
    return "expr."..t.."("
  else
    return "expr.error("
  end
end
local lparent=P("(")
local rparent=P(")")
local noparent=1-(lparent+rparent)
local nested=P{lparent*(noparent+V(1))^0*rparent}
local value=P(lparent*C((noparent+nested)^0)*rparent) 
local lp_child=Cc("expr.child(ll,'")*R("az","AZ","--","__")^1*Cc("')")
local lp_number=S("+-")*R("09")^1
local lp_string=Cc("'")*R("az","AZ","--","__")^1*Cc("'")
local lp_content=(P("'")*(1-P("'"))^0*P("'")+P('"')*(1-P('"'))^0*P('"'))
local cleaner
local lp_special=(C(P("name")+P("text")+P("tag")+P("count")+P("child")))*value/function(t,s)
  if expressions[t] then
    s=s and s~="" and lpegmatch(cleaner,s)
    if s and s~="" then
      return "expr."..t.."(ll,"..s..")"
    else
      return "expr."..t.."(ll)"
    end
  else
    return "expr.error("..t..")"
  end
end
local content=lp_builtin+lp_attribute+lp_special+lp_noequal+lp_doequal+lp_or+lp_and+lp_reserved+lp_lua_function+lp_function+lp_content+
  lp_child+lp_any
local converter=Cs (
  lp_fastpos+(P { lparent*(V(1))^0*rparent+content } )^0
)
cleaner=Cs ((
  lp_reserved+lp_number+lp_string+1 )^1 )
local template_e=[[
    local expr = xml.expressions
    return function(list,ll,l,order)
        return %s
    end
]]
local template_f_y=[[
    local finalizer = xml.finalizers['%s']['%s']
    return function(collection)
        return finalizer(collection,%s)
    end
]]
local template_f_n=[[
    return xml.finalizers['%s']['%s']
]]
local register_self={ kind="axis",axis="self"          } 
local register_parent={ kind="axis",axis="parent"         } 
local register_descendant={ kind="axis",axis="descendant"       } 
local register_child={ kind="axis",axis="child"          } 
local register_descendant_or_self={ kind="axis",axis="descendant-or-self"   } 
local register_root={ kind="axis",axis="root"          } 
local register_ancestor={ kind="axis",axis="ancestor"        } 
local register_ancestor_or_self={ kind="axis",axis="ancestor-or-self"    } 
local register_attribute={ kind="axis",axis="attribute"        } 
local register_namespace={ kind="axis",axis="namespace"        } 
local register_following={ kind="axis",axis="following"        } 
local register_following_sibling={ kind="axis",axis="following-sibling"    } 
local register_preceding={ kind="axis",axis="preceding"        } 
local register_preceding_sibling={ kind="axis",axis="preceding-sibling"    } 
local register_reverse_sibling={ kind="axis",axis="reverse-sibling"     } 
local register_auto_descendant_or_self={ kind="axis",axis="auto-descendant-or-self" } 
local register_auto_descendant={ kind="axis",axis="auto-descendant"     } 
local register_auto_self={ kind="axis",axis="auto-self"        } 
local register_auto_child={ kind="axis",axis="auto-child"       } 
local register_initial_child={ kind="axis",axis="initial-child"      } 
local register_all_nodes={ kind="nodes",nodetest=true,nodes={ true,false,false } }
local skip={}
local function errorrunner_e(str,cnv)
  if not skip[str] then
    report_lpath("error in expression: %s => %s",str,cnv)
    skip[str]=cnv or str
  end
  return false
end
local function errorrunner_f(str,arg)
  report_lpath("error in finalizer: %s(%s)",str,arg or "")
  return false
end
local function register_nodes(nodetest,nodes)
  return { kind="nodes",nodetest=nodetest,nodes=nodes }
end
local function register_expression(expression)
  local converted=lpegmatch(converter,expression)
  local runner=load(format(template_e,converted))
  runner=(runner and runner()) or function() errorrunner_e(expression,converted) end
  return { kind="expression",expression=expression,converted=converted,evaluator=runner }
end
local function register_finalizer(protocol,name,arguments)
  local runner
  if arguments and arguments~="" then
    runner=load(format(template_f_y,protocol or xml.defaultprotocol,name,arguments))
  else
    runner=load(format(template_f_n,protocol or xml.defaultprotocol,name))
  end
  runner=(runner and runner()) or function() errorrunner_f(name,arguments) end
  return { kind="finalizer",name=name,arguments=arguments,finalizer=runner }
end
local expression=P { "ex",
  ex="["*C((V("sq")+V("dq")+(1-S("[]"))+V("ex"))^0)*"]",
  sq="'"*(1-S("'"))^0*"'",
  dq='"'*(1-S('"'))^0*'"',
}
local arguments=P { "ar",
  ar="("*Cs((V("sq")+V("dq")+V("nq")+P(1-P(")")))^0)*")",
  nq=((1-S("),'\""))^1)/function(s) return format("%q",s) end,
  sq=P("'")*(1-P("'"))^0*P("'"),
  dq=P('"')*(1-P('"'))^0*P('"'),
}
local function register_error(str)
  return { kind="error",error=format("unparsed: %s",str) }
end
local special_1=P("*")*Cc(register_auto_descendant)*Cc(register_all_nodes) 
local special_2=P("/")*Cc(register_auto_self)
local special_3=P("")*Cc(register_auto_self)
local no_nextcolon=P(-1)+#(1-P(":")) 
local no_nextlparent=P(-1)+#(1-P("(")) 
local pathparser=Ct { "patterns",
  patterns=spaces*V("protocol")*spaces*(
               (V("special")*spaces*P(-1)                             )+(V("initial")*spaces*V("step")*spaces*(P("/")*spaces*V("step")*spaces)^0 )
              ),
  protocol=Cg(V("letters"),"protocol")*P("://")+Cg(Cc(nil),"protocol"),
  step=((V("shortcuts")+P("/")+V("axis"))*spaces*V("nodes")^0+V("error"))*spaces*V("expressions")^0*spaces*V("finalizer")^0,
  axis=V("descendant")+V("child")+V("parent")+V("self")+V("root")+V("ancestor")+V("descendant_or_self")+V("following_sibling")+V("following")+V("reverse_sibling")+V("preceding_sibling")+V("preceding")+V("ancestor_or_self")+#(1-P(-1))*Cc(register_auto_child),
  special=special_1+special_2+special_3,
  initial=(P("/")*spaces*Cc(register_initial_child))^-1,
  error=(P(1)^1)/register_error,
  shortcuts_a=V("s_descendant_or_self")+V("s_descendant")+V("s_child")+V("s_parent")+V("s_self")+V("s_root")+V("s_ancestor"),
  shortcuts=V("shortcuts_a")*(spaces*"/"*spaces*V("shortcuts_a"))^0,
  s_descendant_or_self=(P("***/")+P("/"))*Cc(register_descendant_or_self),
  s_descendant=P("**")*Cc(register_descendant),
  s_child=P("*")*no_nextcolon*Cc(register_child   ),
  s_parent=P("..")*Cc(register_parent  ),
  s_self=P("." )*Cc(register_self   ),
  s_root=P("^^")*Cc(register_root   ),
  s_ancestor=P("^")*Cc(register_ancestor ),
  descendant=P("descendant::")*Cc(register_descendant     ),
  child=P("child::")*Cc(register_child       ),
  parent=P("parent::")*Cc(register_parent       ),
  self=P("self::")*Cc(register_self        ),
  root=P('root::')*Cc(register_root        ),
  ancestor=P('ancestor::')*Cc(register_ancestor      ),
  descendant_or_self=P('descendant-or-self::')*Cc(register_descendant_or_self ),
  ancestor_or_self=P('ancestor-or-self::')*Cc(register_ancestor_or_self  ),
  following=P('following::')*Cc(register_following     ),
  following_sibling=P('following-sibling::')*Cc(register_following_sibling ),
  preceding=P('preceding::')*Cc(register_preceding     ),
  preceding_sibling=P('preceding-sibling::')*Cc(register_preceding_sibling ),
  reverse_sibling=P('reverse-sibling::')*Cc(register_reverse_sibling  ),
  nodes=(V("nodefunction")*spaces*P("(")*V("nodeset")*P(")")+V("nodetest")*V("nodeset"))/register_nodes,
  expressions=expression/register_expression,
  letters=R("az")^1,
  name=(1-S("/[]()|:*!"))^1,
  negate=P("!")*Cc(false),
  nodefunction=V("negate")+P("not")*Cc(false)+Cc(true),
  nodetest=V("negate")+Cc(true),
  nodename=(V("negate")+Cc(true))*spaces*((V("wildnodename")*P(":")*V("wildnodename"))+(Cc(false)*V("wildnodename"))),
  wildnodename=(C(V("name"))+P("*")*Cc(false))*no_nextlparent,
  nodeset=spaces*Ct(V("nodename")*(spaces*P("|")*spaces*V("nodename"))^0)*spaces,
  finalizer=(Cb("protocol")*P("/")^-1*C(V("name"))*arguments*P(-1))/register_finalizer,
}
xmlpatterns.pathparser=pathparser
local cache={}
local function nodesettostring(set,nodetest)
  local t={}
  for i=1,#set,3 do
    local directive,ns,tg=set[i],set[i+1],set[i+2]
    if not ns or ns=="" then ns="*" end
    if not tg or tg=="" then tg="*" end
    tg=(tg=="@rt@" and "[root]") or format("%s:%s",ns,tg)
    t[i]=(directive and tg) or format("not(%s)",tg)
  end
  if nodetest==false then
    return format("not(%s)",concat(t,"|"))
  else
    return concat(t,"|")
  end
end
local function tagstostring(list)
  if #list==0 then
    return "no elements"
  else
    local t={}
    for i=1,#list do
      local li=list[i]
      local ns,tg=li.ns,li.tg
      if not ns or ns=="" then ns="*" end
      if not tg or tg=="" then tg="*" end
      t[i]=(tg=="@rt@" and "[root]") or format("%s:%s",ns,tg)
    end
    return concat(t," ")
  end
end
xml.nodesettostring=nodesettostring
local lpath 
local lshowoptions={ functions=false }
local function lshow(parsed)
  if type(parsed)=="string" then
    parsed=lpath(parsed)
  end
  report_lpath("%s://%s => %s",parsed.protocol or xml.defaultprotocol,parsed.pattern,
    table.serialize(parsed,false,lshowoptions))
end
xml.lshow=lshow
local function add_comment(p,str)
  local pc=p.comment
  if not pc then
    p.comment={ str }
  else
    pc[#pc+1]=str
  end
end
lpath=function (pattern) 
  lpathcalls=lpathcalls+1
  if type(pattern)=="table" then
    return pattern
  else
    local parsed=cache[pattern]
    if parsed then
      lpathcached=lpathcached+1
    else
      parsed=lpegmatch(pathparser,pattern)
      if parsed then
        parsed.pattern=pattern
        local np=#parsed
        if np==0 then
          parsed={ pattern=pattern,register_self,state="parsing error" }
          report_lpath("parsing error in pattern: %s",pattern)
          lshow(parsed)
        else
          local pi=parsed[1]
          if pi.axis=="auto-child" then
            if false then
              add_comment(parsed,"auto-child replaced by auto-descendant-or-self")
              parsed[1]=register_auto_descendant_or_self
            else
              add_comment(parsed,"auto-child replaced by auto-descendant")
              parsed[1]=register_auto_descendant
            end
          elseif pi.axis=="initial-child" and np>1 and parsed[2].axis then
            add_comment(parsed,"initial-child removed") 
            remove(parsed,1)
          end
          local np=#parsed 
          if np>1 then
            local pnp=parsed[np]
            if pnp.kind=="nodes" and pnp.nodetest==true then
              local nodes=pnp.nodes
              if nodes[1]==true and nodes[2]==false and nodes[3]==false then
                add_comment(parsed,"redundant final wildcard filter removed")
                remove(parsed,np)
              end
            end
          end
        end
      else
        parsed={ pattern=pattern }
      end
      cache[pattern]=parsed
      if trace_lparse and not trace_lprofile then
        lshow(parsed)
      end
    end
    return parsed
  end
end
xml.lpath=lpath
local profiled={} xml.profiled=profiled
local function profiled_apply(list,parsed,nofparsed,order)
  local p=profiled[parsed.pattern]
  if p then
    p.tested=p.tested+1
  else
    p={ tested=1,matched=0,finalized=0 }
    profiled[parsed.pattern]=p
  end
  local collected=list
  for i=1,nofparsed do
    local pi=parsed[i]
    local kind=pi.kind
    if kind=="axis" then
      collected=apply_axis[pi.axis](collected)
    elseif kind=="nodes" then
      collected=apply_nodes(collected,pi.nodetest,pi.nodes)
    elseif kind=="expression" then
      collected=apply_expression(collected,pi.evaluator,order)
    elseif kind=="finalizer" then
      collected=pi.finalizer(collected) 
      p.matched=p.matched+1
      p.finalized=p.finalized+1
      return collected
    end
    if not collected or #collected==0 then
      local pn=i<nofparsed and parsed[nofparsed]
      if pn and pn.kind=="finalizer" then
        collected=pn.finalizer(collected)
        p.finalized=p.finalized+1
        return collected
      end
      return nil
    end
  end
  if collected then
    p.matched=p.matched+1
  end
  return collected
end
local function traced_apply(list,parsed,nofparsed,order)
  if trace_lparse then
    lshow(parsed)
  end
  report_lpath("collecting: %s",parsed.pattern)
  report_lpath("root tags : %s",tagstostring(list))
  report_lpath("order     : %s",order or "unset")
  local collected=list
  for i=1,nofparsed do
    local pi=parsed[i]
    local kind=pi.kind
    if kind=="axis" then
      collected=apply_axis[pi.axis](collected)
      report_lpath("% 10i : ax : %s",(collected and #collected) or 0,pi.axis)
    elseif kind=="nodes" then
      collected=apply_nodes(collected,pi.nodetest,pi.nodes)
      report_lpath("% 10i : ns : %s",(collected and #collected) or 0,nodesettostring(pi.nodes,pi.nodetest))
    elseif kind=="expression" then
      collected=apply_expression(collected,pi.evaluator,order)
      report_lpath("% 10i : ex : %s -> %s",(collected and #collected) or 0,pi.expression,pi.converted)
    elseif kind=="finalizer" then
      collected=pi.finalizer(collected)
      report_lpath("% 10i : fi : %s : %s(%s)",(type(collected)=="table" and #collected) or 0,parsed.protocol or xml.defaultprotocol,pi.name,pi.arguments or "")
      return collected
    end
    if not collected or #collected==0 then
      local pn=i<nofparsed and parsed[nofparsed]
      if pn and pn.kind=="finalizer" then
        collected=pn.finalizer(collected)
        report_lpath("% 10i : fi : %s : %s(%s)",(type(collected)=="table" and #collected) or 0,parsed.protocol or xml.defaultprotocol,pn.name,pn.arguments or "")
        return collected
      end
      return nil
    end
  end
  return collected
end
local function normal_apply(list,parsed,nofparsed,order)
  local collected=list
  for i=1,nofparsed do
    local pi=parsed[i]
    local kind=pi.kind
    if kind=="axis" then
      local axis=pi.axis
      if axis~="self" then
        collected=apply_axis[axis](collected)
      end
    elseif kind=="nodes" then
      collected=apply_nodes(collected,pi.nodetest,pi.nodes)
    elseif kind=="expression" then
      collected=apply_expression(collected,pi.evaluator,order)
    elseif kind=="finalizer" then
      return pi.finalizer(collected)
    end
    if not collected or #collected==0 then
      local pf=i<nofparsed and parsed[nofparsed].finalizer
      if pf then
        return pf(collected) 
      end
      return nil
    end
  end
  return collected
end
local function applylpath(list,pattern)
  if not list then
    return
  end
  local parsed=cache[pattern]
  if parsed then
    lpathcalls=lpathcalls+1
    lpathcached=lpathcached+1
  elseif type(pattern)=="table" then
    lpathcalls=lpathcalls+1
    parsed=pattern
  else
    parsed=lpath(pattern) or pattern
  end
  if not parsed then
    return
  end
  local nofparsed=#parsed
  if nofparsed==0 then
    return 
  end
  if not trace_lpath then
    return normal_apply ({ list },parsed,nofparsed,list.mi)
  elseif trace_lprofile then
    return profiled_apply({ list },parsed,nofparsed,list.mi)
  else
    return traced_apply ({ list },parsed,nofparsed,list.mi)
  end
end
xml.applylpath=applylpath
function xml.filter(root,pattern) 
  return applylpath(root,pattern)
end
expressions.child=function(e,pattern)
  return applylpath(e,pattern) 
end
expressions.count=function(e,pattern) 
  local collected=applylpath(e,pattern) 
  return pattern and (collected and #collected) or 0
end
expressions.oneof=function(s,...)
  for i=1,select("#",...) do
    if s==select(i,...) then
      return true
    end
  end
  return false
end
expressions.error=function(str)
  xml.errorhandler(format("unknown function in lpath expression: %s",tostring(str or "?")))
  return false
end
expressions.undefined=function(s)
  return s==nil
end
expressions.quit=function(s)
  if s or s==nil then
    quit_expression=true
  end
  return true
end
expressions.print=function(...)
  print(...)
  return true
end
expressions.contains=find
expressions.find=find
expressions.upper=upper
expressions.lower=lower
expressions.number=tonumber
expressions.boolean=toboolean
function expressions.contains(str,pattern)
  local t=type(str)
  if t=="string" then
    if find(str,pattern) then
      return true
    end
  elseif t=="table" then
    for i=1,#str do
      local d=str[i]
      if type(d)=="string" and find(d,pattern) then
        return true
      end
    end
  end
  return false
end
local function traverse(root,pattern,handle)
  local collected=applylpath(root,pattern)
  if collected then
    for c=1,#collected do
      local e=collected[c]
      local r=e.__p__
      handle(r,r.dt,e.ni)
    end
  end
end
local function selection(root,pattern,handle)
  local collected=applylpath(root,pattern)
  if collected then
    if handle then
      for c=1,#collected do
        handle(collected[c])
      end
    else
      return collected
    end
  end
end
xml.traverse=traverse      
xml.selection=selection
local function dofunction(collected,fnc,...)
  if collected then
    local f=functions[fnc]
    if f then
      for c=1,#collected do
        f(collected[c],...)
      end
    else
      report_lpath("unknown function %a",fnc)
    end
  end
end
finalizers.xml["function"]=dofunction
finalizers.tex["function"]=dofunction
expressions.text=function(e,n)
  local rdt=e.__p__.dt
  return rdt and rdt[n] or ""
end
expressions.name=function(e,n) 
  local found=false
  n=tonumber(n) or 0
  if n==0 then
    found=type(e)=="table" and e
  elseif n<0 then
    local d,k=e.__p__.dt,e.ni
    for i=k-1,1,-1 do
      local di=d[i]
      if type(di)=="table" then
        if n==-1 then
          found=di
          break
        else
          n=n+1
        end
      end
    end
  else
    local d,k=e.__p__.dt,e.ni
    for i=k+1,#d,1 do
      local di=d[i]
      if type(di)=="table" then
        if n==1 then
          found=di
          break
        else
          n=n-1
        end
      end
    end
  end
  if found then
    local ns,tg=found.rn or found.ns or "",found.tg
    if ns~="" then
      return ns..":"..tg
    else
      return tg
    end
  else
    return ""
  end
end
expressions.tag=function(e,n) 
  if not e then
    return ""
  else
    local found=false
    n=tonumber(n) or 0
    if n==0 then
      found=(type(e)=="table") and e 
    elseif n<0 then
      local d,k=e.__p__.dt,e.ni
      for i=k-1,1,-1 do
        local di=d[i]
        if type(di)=="table" then
          if n==-1 then
            found=di
            break
          else
            n=n+1
          end
        end
      end
    else
      local d,k=e.__p__.dt,e.ni
      for i=k+1,#d,1 do
        local di=d[i]
        if type(di)=="table" then
          if n==1 then
            found=di
            break
          else
            n=n-1
          end
        end
      end
    end
    return (found and found.tg) or ""
  end
end
local dummy=function() end
function xml.elements(root,pattern,reverse) 
  local collected=applylpath(root,pattern)
  if not collected then
    return dummy
  elseif reverse then
    local c=#collected+1
    return function()
      if c>1 then
        c=c-1
        local e=collected[c]
        local r=e.__p__
        return r,r.dt,e.ni
      end
    end
  else
    local n,c=#collected,0
    return function()
      if c<n then
        c=c+1
        local e=collected[c]
        local r=e.__p__
        return r,r.dt,e.ni
      end
    end
  end
end
function xml.collected(root,pattern,reverse) 
  local collected=applylpath(root,pattern)
  if not collected then
    return dummy
  elseif reverse then
    local c=#collected+1
    return function()
      if c>1 then
        c=c-1
        return collected[c]
      end
    end
  else
    local n,c=#collected,0
    return function()
      if c<n then
        c=c+1
        return collected[c]
      end
    end
  end
end
function xml.inspect(collection,pattern)
  pattern=pattern or "."
  for e in xml.collected(collection,pattern or ".") do
    report_lpath("pattern: %s\n\n%s\n",pattern,xml.tostring(e))
  end
end
local function split(e)
  local dt=e.dt
  if dt then
    for i=1,#dt do
      local dti=dt[i]
      if type(dti)=="string" then
        dti=gsub(dti,"^[\n\r]*(.-)[\n\r]*","%1")
        dti=gsub(dti,"[\n\r]+","\n\n")
        dt[i]=dti
      else
        split(dti)
      end
    end
  end
  return e
end
function xml.finalizers.paragraphs(c)
  for i=1,#c do
    split(c[i])
  end
  return c
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["lxml-mis"] = package.loaded["lxml-mis"] or true

-- original size: 3684, stripped down to: 1957

if not modules then modules={} end modules ['lxml-mis']={
  version=1.001,
  comment="this module is the basis for the lxml-* ones",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local xml,lpeg,string=xml,lpeg,string
local concat=table.concat
local type,next,tonumber,tostring,setmetatable,loadstring=type,next,tonumber,tostring,setmetatable,loadstring
local format,gsub,match=string.format,string.gsub,string.match
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local P,S,R,C,V,Cc,Cs=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.V,lpeg.Cc,lpeg.Cs
lpegpatterns.xml=lpegpatterns.xml or {}
local xmlpatterns=lpegpatterns.xml
local function xmlgsub(t,old,new) 
  local dt=t.dt
  if dt then
    for k=1,#dt do
      local v=dt[k]
      if type(v)=="string" then
        dt[k]=gsub(v,old,new)
      else
        xmlgsub(v,old,new)
      end
    end
  end
end
function xml.stripleadingspaces(dk,d,k) 
  if d and k then
    local dkm=d[k-1]
    if dkm and type(dkm)=="string" then
      local s=match(dkm,"\n(%s+)")
      xmlgsub(dk,"\n"..rep(" ",#s),"\n")
    end
  end
end
local normal=(1-S("<&>"))^0
local special=P("<")/"&lt;"+P(">")/"&gt;"+P("&")/"&amp;"
local escaped=Cs(normal*(special*normal)^0)
local normal=(1-S"&")^0
local special=P("&lt;")/"<"+P("&gt;")/">"+P("&amp;")/"&"
local unescaped=Cs(normal*(special*normal)^0)
local cleansed=Cs(((P("<")*(1-P(">"))^0*P(">"))/""+1)^0)
xmlpatterns.escaped=escaped
xmlpatterns.unescaped=unescaped
xmlpatterns.cleansed=cleansed
function xml.escaped (str) return lpegmatch(escaped,str)  end
function xml.unescaped(str) return lpegmatch(unescaped,str) end
function xml.cleansed (str) return lpegmatch(cleansed,str) end
function xml.fillin(root,pattern,str,check)
  local e=xml.first(root,pattern)
  if e then
    local n=#e.dt
    if not check or n==0 or (n==1 and e.dt[1]=="") then
      e.dt={ str }
    end
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["lxml-aux"] = package.loaded["lxml-aux"] or true

-- original size: 23804, stripped down to: 16817

if not modules then modules={} end modules ['lxml-aux']={
  version=1.001,
  comment="this module is the basis for the lxml-* ones",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local trace_manipulations=false trackers.register("lxml.manipulations",function(v) trace_manipulations=v end)
local report_xml=logs.reporter("xml")
local xml=xml
local xmlconvert,xmlcopy,xmlname=xml.convert,xml.copy,xml.name
local xmlinheritedconvert=xml.inheritedconvert
local xmlapplylpath=xml.applylpath
local xmlfilter=xml.filter
local type,setmetatable,getmetatable=type,setmetatable,getmetatable
local insert,remove,fastcopy,concat=table.insert,table.remove,table.fastcopy,table.concat
local gmatch,gsub,format,find,strip=string.gmatch,string.gsub,string.format,string.find,string.strip
local utfbyte=utf.byte
local function report(what,pattern,c,e)
  report_xml("%s element %a, root %a, position %a, index %a, pattern %a",what,xmlname(e),xmlname(e.__p__),c,e.ni,pattern)
end
local function withelements(e,handle,depth)
  if e and handle then
    local edt=e.dt
    if edt then
      depth=depth or 0
      for i=1,#edt do
        local e=edt[i]
        if type(e)=="table" then
          handle(e,depth)
          withelements(e,handle,depth+1)
        end
      end
    end
  end
end
xml.withelements=withelements
function xml.withelement(e,n,handle) 
  if e and n~=0 and handle then
    local edt=e.dt
    if edt then
      if n>0 then
        for i=1,#edt do
          local ei=edt[i]
          if type(ei)=="table" then
            if n==1 then
              handle(ei)
              return
            else
              n=n-1
            end
          end
        end
      elseif n<0 then
        for i=#edt,1,-1 do
          local ei=edt[i]
          if type(ei)=="table" then
            if n==-1 then
              handle(ei)
              return
            else
              n=n+1
            end
          end
        end
      end
    end
  end
end
function xml.each(root,pattern,handle,reverse)
  local collected=xmlapplylpath(root,pattern)
  if collected then
    if reverse then
      for c=#collected,1,-1 do
        handle(collected[c])
      end
    else
      for c=1,#collected do
        handle(collected[c])
      end
    end
    return collected
  end
end
function xml.processattributes(root,pattern,handle)
  local collected=xmlapplylpath(root,pattern)
  if collected and handle then
    for c=1,#collected do
      handle(collected[c].at)
    end
  end
  return collected
end
function xml.collect(root,pattern)
  return xmlapplylpath(root,pattern)
end
function xml.collecttexts(root,pattern,flatten) 
  local collected=xmlapplylpath(root,pattern)
  if collected and flatten then
    local xmltostring=xml.tostring
    for c=1,#collected do
      collected[c]=xmltostring(collected[c].dt)
    end
  end
  return collected or {}
end
function xml.collect_tags(root,pattern,nonamespace)
  local collected=xmlapplylpath(root,pattern)
  if collected then
    local t,n={},0
    for c=1,#collected do
      local e=collected[c]
      local ns,tg=e.ns,e.tg
      n=n+1
      if nonamespace then
        t[n]=tg
      elseif ns=="" then
        t[n]=tg
      else
        t[n]=ns..":"..tg
      end
    end
    return t
  end
end
local no_root={ no_root=true }
local function redo_ni(d)
  for k=1,#d do
    local dk=d[k]
    if type(dk)=="table" then
      dk.ni=k
    end
  end
end
local function xmltoelement(whatever,root)
  if not whatever then
    return nil
  end
  local element
  if type(whatever)=="string" then
    element=xmlinheritedconvert(whatever,root) 
  else
    element=whatever 
  end
  if element.error then
    return whatever 
  end
  if element then
  end
  return element
end
xml.toelement=xmltoelement
local function copiedelement(element,newparent)
  if type(element)=="string" then
    return element
  else
    element=xmlcopy(element).dt
    if newparent and type(element)=="table" then
      element.__p__=newparent
    end
    return element
  end
end
function xml.delete(root,pattern)
  if not pattern or pattern=="" then
    local p=root.__p__
    if p then
      if trace_manipulations then
        report('deleting',"--",c,root)
      end
      local d=p.dt
      remove(d,root.ni)
      redo_ni(d) 
    end
  else
    local collected=xmlapplylpath(root,pattern)
    if collected then
      for c=1,#collected do
        local e=collected[c]
        local p=e.__p__
        if p then
          if trace_manipulations then
            report('deleting',pattern,c,e)
          end
          local d=p.dt
          remove(d,e.ni)
          redo_ni(d) 
        end
      end
    end
  end
end
function xml.replace(root,pattern,whatever)
  local element=root and xmltoelement(whatever,root)
  local collected=element and xmlapplylpath(root,pattern)
  if collected then
    for c=1,#collected do
      local e=collected[c]
      local p=e.__p__
      if p then
        if trace_manipulations then
          report('replacing',pattern,c,e)
        end
        local d=p.dt
        d[e.ni]=copiedelement(element,p)
        redo_ni(d) 
      end
    end
  end
end
local function wrap(e,wrapper)
  local t={
    rn=e.rn,
    tg=e.tg,
    ns=e.ns,
    at=e.at,
    dt=e.dt,
    __p__=e,
  }
  setmetatable(t,getmetatable(e))
  e.rn=wrapper.rn or e.rn or ""
  e.tg=wrapper.tg or e.tg or ""
  e.ns=wrapper.ns or e.ns or ""
  e.at=fastcopy(wrapper.at)
  e.dt={ t }
end
function xml.wrap(root,pattern,whatever)
  if whatever then
    local wrapper=xmltoelement(whatever,root)
    local collected=xmlapplylpath(root,pattern)
    if collected then
      for c=1,#collected do
        local e=collected[c]
        if trace_manipulations then
          report('wrapping',pattern,c,e)
        end
        wrap(e,wrapper)
      end
    end
  else
    wrap(root,xmltoelement(pattern))
  end
end
local function inject_element(root,pattern,whatever,prepend)
  local element=root and xmltoelement(whatever,root)
  local collected=element and xmlapplylpath(root,pattern)
  local function inject_e(e)
    local r=e.__p__
    local d,k,rri=r.dt,e.ni,r.ri
    local edt=(rri and d[rri].dt) or (d and d[k] and d[k].dt)
    if edt then
      local be,af
      local cp=copiedelement(element,e)
      if prepend then
        be,af=cp,edt
      else
        be,af=edt,cp
      end
      local bn=#be
      for i=1,#af do
        bn=bn+1
        be[bn]=af[i]
      end
      if rri then
        r.dt[rri].dt=be
      else
        d[k].dt=be
      end
      redo_ni(d)
    end
  end
  if not collected then
  elseif collected.tg then
    inject_e(collected)
  else
    for c=1,#collected do
      inject_e(collected[c])
    end
  end
end
local function insert_element(root,pattern,whatever,before) 
  local element=root and xmltoelement(whatever,root)
  local collected=element and xmlapplylpath(root,pattern)
  local function insert_e(e)
    local r=e.__p__
    local d,k=r.dt,e.ni
    if not before then
      k=k+1
    end
    insert(d,k,copiedelement(element,r))
    redo_ni(d)
  end
  if not collected then
  elseif collected.tg then
    insert_e(collected)
  else
    for c=1,#collected do
      insert_e(collected[c])
    end
  end
end
xml.insert_element=insert_element
xml.insertafter=insert_element
xml.insertbefore=function(r,p,e) insert_element(r,p,e,true) end
xml.injectafter=inject_element
xml.injectbefore=function(r,p,e) inject_element(r,p,e,true) end
local function include(xmldata,pattern,attribute,recursive,loaddata)
  pattern=pattern or 'include'
  loaddata=loaddata or io.loaddata
  local collected=xmlapplylpath(xmldata,pattern)
  if collected then
    for c=1,#collected do
      local ek=collected[c]
      local name=nil
      local ekdt=ek.dt
      local ekat=ek.at
      local epdt=ek.__p__.dt
      if not attribute or attribute=="" then
        name=(type(ekdt)=="table" and ekdt[1]) or ekdt 
      end
      if not name then
        for a in gmatch(attribute or "href","([^|]+)") do
          name=ekat[a]
          if name then break end
        end
      end
      local data=(name and name~="" and loaddata(name)) or ""
      if data=="" then
        epdt[ek.ni]="" 
      elseif ekat["parse"]=="text" then
        epdt[ek.ni]=xml.escaped(data) 
      else
        local xi=xmlinheritedconvert(data,xmldata)
        if not xi then
          epdt[ek.ni]="" 
        else
          if recursive then
            include(xi,pattern,attribute,recursive,loaddata)
          end
          epdt[ek.ni]=xml.body(xi) 
        end
      end
    end
  end
end
xml.include=include
local function stripelement(e,nolines,anywhere)
  local edt=e.dt
  if edt then
    if anywhere then
      local t,n={},0
      for e=1,#edt do
        local str=edt[e]
        if type(str)~="string" then
          n=n+1
          t[n]=str
        elseif str~="" then
          if nolines then
            str=gsub(str,"%s+"," ")
          end
          str=gsub(str,"^%s*(.-)%s*$","%1")
          if str~="" then
            n=n+1
            t[n]=str
          end
        end
      end
      e.dt=t
    else
      if #edt>0 then
        local str=edt[1]
        if type(str)~="string" then
        elseif str=="" then
          remove(edt,1)
        else
          if nolines then
            str=gsub(str,"%s+"," ")
          end
          str=gsub(str,"^%s+","")
          if str=="" then
            remove(edt,1)
          else
            edt[1]=str
          end
        end
      end
      local nedt=#edt
      if nedt>0 then
        local str=edt[nedt]
        if type(str)~="string" then
        elseif str=="" then
          remove(edt)
        else
          if nolines then
            str=gsub(str,"%s+"," ")
          end
          str=gsub(str,"%s+$","")
          if str=="" then
            remove(edt)
          else
            edt[nedt]=str
          end
        end
      end
    end
  end
  return e 
end
xml.stripelement=stripelement
function xml.strip(root,pattern,nolines,anywhere) 
  local collected=xmlapplylpath(root,pattern) 
  if collected then
    for i=1,#collected do
      stripelement(collected[i],nolines,anywhere)
    end
  end
end
local function renamespace(root,oldspace,newspace) 
  local ndt=#root.dt
  for i=1,ndt or 0 do
    local e=root[i]
    if type(e)=="table" then
      if e.ns==oldspace then
        e.ns=newspace
        if e.rn then
          e.rn=newspace
        end
      end
      local edt=e.dt
      if edt then
        renamespace(edt,oldspace,newspace)
      end
    end
  end
end
xml.renamespace=renamespace
function xml.remaptag(root,pattern,newtg)
  local collected=xmlapplylpath(root,pattern)
  if collected then
    for c=1,#collected do
      collected[c].tg=newtg
    end
  end
end
function xml.remapnamespace(root,pattern,newns)
  local collected=xmlapplylpath(root,pattern)
  if collected then
    for c=1,#collected do
      collected[c].ns=newns
    end
  end
end
function xml.checknamespace(root,pattern,newns)
  local collected=xmlapplylpath(root,pattern)
  if collected then
    for c=1,#collected do
      local e=collected[c]
      if (not e.rn or e.rn=="") and e.ns=="" then
        e.rn=newns
      end
    end
  end
end
function xml.remapname(root,pattern,newtg,newns,newrn)
  local collected=xmlapplylpath(root,pattern)
  if collected then
    for c=1,#collected do
      local e=collected[c]
      e.tg,e.ns,e.rn=newtg,newns,newrn
    end
  end
end
function xml.cdatatotext(e)
  local dt=e.dt
  if #dt==1 then
    local first=dt[1]
    if first.tg=="@cd@" then
      e.dt=first.dt
    end
  else
  end
end
function xml.texttocdata(e) 
  local dt=e.dt
  local s=xml.tostring(dt) 
  e.tg="@cd@"
  e.special=true
  e.ns=""
  e.rn=""
  e.dt={ s }
  e.at=nil
end
function xml.elementtocdata(e) 
  local dt=e.dt
  local s=xml.tostring(e) 
  e.tg="@cd@"
  e.special=true
  e.ns=""
  e.rn=""
  e.dt={ s }
  e.at=nil
end
xml.builtinentities=table.tohash { "amp","quot","apos","lt","gt" } 
local entities=characters and characters.entities or nil
local builtinentities=xml.builtinentities
function xml.addentitiesdoctype(root,option) 
  if not entities then
    require("char-ent")
    entities=characters.entities
  end
  if entities and root and root.tg=="@rt@" and root.statistics then
    local list={}
    local hexify=option=="hexadecimal"
    for k,v in table.sortedhash(root.statistics.entities.names) do
      if not builtinentities[k] then
        local e=entities[k]
        if not e then
          e=format("[%s]",k)
        elseif hexify then
          e=format("&#%05X;",utfbyte(k))
        end
        list[#list+1]=format("  <!ENTITY %s %q >",k,e)
      end
    end
    local dt=root.dt
    local n=dt[1].tg=="@pi@" and 2 or 1
    if #list>0 then
      insert(dt,n,{ "\n" })
      insert(dt,n,{
        tg="@dt@",
        dt={ format("Something [\n%s\n] ",concat(list)) },
        ns="",
        special=true,
      })
      insert(dt,n,{ "\n\n" })
    else
    end
  end
end
xml.all=xml.each
xml.insert=xml.insertafter
xml.inject=xml.injectafter
xml.after=xml.insertafter
xml.before=xml.insertbefore
xml.process=xml.each
xml.obsolete=xml.obsolete or {}
local obsolete=xml.obsolete
xml.strip_whitespace=xml.strip         obsolete.strip_whitespace=xml.strip
xml.collect_elements=xml.collect        obsolete.collect_elements=xml.collect
xml.delete_element=xml.delete        obsolete.delete_element=xml.delete
xml.replace_element=xml.replace        obsolete.replace_element=xml.replacet
xml.each_element=xml.each         obsolete.each_element=xml.each
xml.process_elements=xml.process        obsolete.process_elements=xml.process
xml.insert_element_after=xml.insertafter      obsolete.insert_element_after=xml.insertafter
xml.insert_element_before=xml.insertbefore     obsolete.insert_element_before=xml.insertbefore
xml.inject_element_after=xml.injectafter      obsolete.inject_element_after=xml.injectafter
xml.inject_element_before=xml.injectbefore     obsolete.inject_element_before=xml.injectbefore
xml.process_attributes=xml.processattributes   obsolete.process_attributes=xml.processattributes
xml.collect_texts=xml.collecttexts     obsolete.collect_texts=xml.collecttexts
xml.inject_element=xml.inject        obsolete.inject_element=xml.inject
xml.remap_tag=xml.remaptag       obsolete.remap_tag=xml.remaptag
xml.remap_name=xml.remapname       obsolete.remap_name=xml.remapname
xml.remap_namespace=xml.remapnamespace    obsolete.remap_namespace=xml.remapnamespace
function xml.cdata(e)
  if e then
    local dt=e.dt
    if dt and #dt==1 then
      local first=dt[1]
      return first.tg=="@cd@" and first.dt[1] or ""
    end
  end
  return ""
end
function xml.finalizers.xml.cdata(collected)
  if collected then
    local e=collected[1]
    if e then
      local dt=e.dt
      if dt and #dt==1 then
        local first=dt[1]
        return first.tg=="@cd@" and first.dt[1] or ""
      end
    end
  end
  return ""
end
function xml.insertcomment(e,str,n) 
  table.insert(e.dt,n or 1,{
    tg="@cm@",
    ns="",
    special=true,
    at={},
    dt={ str },
  })
end
function xml.setcdata(e,str) 
  e.dt={ {
    tg="@cd@",
    ns="",
    special=true,
    at={},
    dt={ str },
  } }
end
function xml.separate(x,pattern)
  local collected=xmlapplylpath(x,pattern)
  if collected then
    for c=1,#collected do
      local e=collected[c]
      local d=e.dt
      if d==x then
        report_xml("warning: xml.separate changes root")
        x=d
      end
      local t,n={ "\n" },1
      local i,nd=1,#d
      while i<=nd do
        while i<=nd do
          local di=d[i]
          if type(di)=="string" then
            if di=="\n" or find(di,"^%s+$") then 
              i=i+1
            else
              d[i]=strip(di)
              break
            end
          else
            break
          end
        end
        if i>nd then
          break
        end
        t[n+1]="\n"
        t[n+2]=d[i]
        t[n+3]="\n"
        n=n+3
        i=i+1
      end
      t[n+1]="\n"
      setmetatable(t,getmetatable(d))
      e.dt=t
    end
  end
  return x
end
local helpers=xml.helpers or {}
xml.helpers=helpers
local function normal(e,action)
  local edt=e.dt
  if edt then
    for i=1,#edt do
      local str=edt[i]
      if type(str)=="string" and str~="" then
        edt[i]=action(str)
      end
    end
  end
end
local function recurse(e,action)
  local edt=e.dt
  if edt then
    for i=1,#edt do
      local str=edt[i]
      if type(str)~="string" then
        recurse(str,action,recursive)
      elseif str~="" then
        edt[i]=action(str)
      end
    end
  end
end
function helpers.recursetext(collected,action,recursive)
  if recursive then
    for i=1,#collected do
      recurse(collected[i],action)
    end
  else
    for i=1,#collected do
      normal(collected[i],action)
    end
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["lxml-xml"] = package.loaded["lxml-xml"] or true

-- original size: 10274, stripped down to: 7538

if not modules then modules={} end modules ['lxml-xml']={
  version=1.001,
  comment="this module is the basis for the lxml-* ones",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local concat=table.concat
local find,lower,upper=string.find,string.lower,string.upper
local xml=xml
local finalizers=xml.finalizers.xml
local xmlfilter=xml.filter 
local xmltostring=xml.tostring
local xmlserialize=xml.serialize
local xmlcollected=xml.collected
local xmlnewhandlers=xml.newhandlers
local function first(collected) 
  return collected and collected[1]
end
local function last(collected)
  return collected and collected[#collected]
end
local function all(collected)
  return collected
end
local reverse=table.reversed
local function attribute(collected,name)
  if collected and #collected>0 then
    local at=collected[1].at
    return at and at[name]
  end
end
local function att(id,name)
  local at=id.at
  return at and at[name]
end
local function count(collected)
  return collected and #collected or 0
end
local function position(collected,n)
  if not collected then
    return 0
  end
  local nc=#collected
  if nc==0 then
    return 0
  end
  n=tonumber(n) or 0
  if n<0 then
    return collected[nc+n+1]
  elseif n>0 then
    return collected[n]
  else
    return collected[1].mi or 0
  end
end
local function match(collected)
  return collected and #collected>0 and collected[1].mi or 0 
end
local function index(collected)
  return collected and #collected>0 and collected[1].ni or 0 
end
local function attributes(collected,arguments)
  if collected and #collected>0 then
    local at=collected[1].at
    if arguments then
      return at[arguments]
    elseif next(at) then
      return at 
    end
  end
end
local function chainattribute(collected,arguments) 
  if collected and #collected>0 then
    local e=collected[1]
    while e do
      local at=e.at
      if at then
        local a=at[arguments]
        if a then
          return a
        end
      else
        break 
      end
      e=e.__p__
    end
  end
  return ""
end
local function raw(collected) 
  if collected and #collected>0 then
    local e=collected[1] or collected
    return e and xmltostring(e) or "" 
  else
    return ""
  end
end
local xmltexthandler=xmlnewhandlers {
  name="string",
  initialize=function()
    result={}
    return result
  end,
  finalize=function()
    return concat(result)
  end,
  handle=function(...)
    result[#result+1]=concat {... }
  end,
  escape=false,
}
local function xmltotext(root)
  local dt=root.dt
  if not dt then
    return ""
  end
  local nt=#dt 
  if nt==0 then
    return ""
  elseif nt==1 and type(dt[1])=="string" then
    return dt[1] 
  else
    return xmlserialize(root,xmltexthandler) or ""
  end
end
local function text(collected) 
  if collected then 
    local e=collected[1] or collected 
    return e and xmltotext(e) or ""
  else
    return ""
  end
end
local function texts(collected)
  if not collected then
    return {} 
  end
  local nc=#collected
  if nc==0 then
    return {} 
  end
  local t,n={},0
  for c=1,nc do
    local e=collected[c]
    if e and e.dt then
      n=n+1
      t[n]=e.dt
    end
  end
  return t
end
local function tag(collected,n)
  if not collected then
    return
  end
  local nc=#collected
  if nc==0 then
    return
  end
  local c
  if n==0 or not n then
    c=collected[1]
  elseif n>1 then
    c=collected[n]
  else
    c=collected[nc-n+1]
  end
  return c and c.tg
end
local function name(collected,n)
  if not collected then
    return
  end
  local nc=#collected
  if nc==0 then
    return
  end
  local c
  if n==0 or not n then
    c=collected[1]
  elseif n>1 then
    c=collected[n]
  else
    c=collected[nc-n+1]
  end
  if not c then
  elseif c.ns=="" then
    return c.tg
  else
    return c.ns..":"..c.tg
  end
end
local function tags(collected,nonamespace)
  if not collected then
    return
  end
  local nc=#collected
  if nc==0 then
    return
  end
  local t,n={},0
  for c=1,nc do
    local e=collected[c]
    local ns,tg=e.ns,e.tg
    n=n+1
    if nonamespace or ns=="" then
      t[n]=tg
    else
      t[n]=ns..":"..tg
    end
  end
  return t
end
local function empty(collected,spacesonly)
  if not collected then
    return true
  end
  local nc=#collected
  if nc==0 then
    return true
  end
  for c=1,nc do
    local e=collected[c]
    if e then
      local edt=e.dt
      if edt then
        local n=#edt
        if n==1 then
          local edk=edt[1]
          local typ=type(edk)
          if typ=="table" then
            return false
          elseif edk~="" then
            return false
          elseif spacesonly and not find(edk,"%S") then
            return false
          end
        elseif n>1 then
          return false
        end
      end
    end
  end
  return true
end
finalizers.first=first
finalizers.last=last
finalizers.all=all
finalizers.reverse=reverse
finalizers.elements=all
finalizers.default=all
finalizers.attribute=attribute
finalizers.att=att
finalizers.count=count
finalizers.position=position
finalizers.match=match
finalizers.index=index
finalizers.attributes=attributes
finalizers.chainattribute=chainattribute
finalizers.text=text
finalizers.texts=texts
finalizers.tag=tag
finalizers.name=name
finalizers.tags=tags
finalizers.empty=empty
function xml.first(id,pattern)
  return first(xmlfilter(id,pattern))
end
function xml.last(id,pattern)
  return last(xmlfilter(id,pattern))
end
function xml.count(id,pattern)
  return count(xmlfilter(id,pattern))
end
function xml.attribute(id,pattern,a,default)
  return attribute(xmlfilter(id,pattern),a,default)
end
function xml.raw(id,pattern)
  if pattern then
    return raw(xmlfilter(id,pattern))
  else
    return raw(id)
  end
end
function xml.text(id,pattern) 
  if pattern then
    local collected=xmlfilter(id,pattern)
    return collected and #collected>0 and xmltotext(collected[1]) or ""
  elseif id then
    return xmltotext(id) or ""
  else
    return ""
  end
end
xml.content=text
function xml.position(id,pattern,n) 
  return position(xmlfilter(id,pattern),n)
end
function xml.match(id,pattern) 
  return match(xmlfilter(id,pattern))
end
function xml.empty(id,pattern,spacesonly)
  return empty(xmlfilter(id,pattern),spacesonly)
end
xml.all=xml.filter
xml.index=xml.position
xml.found=xml.filter
local function totable(x)
  local t={}
  for e in xmlcollected(x[1] or x,"/*") do
    t[e.tg]=xmltostring(e.dt) or ""
  end
  return next(t) and t or nil
end
xml.table=totable
finalizers.table=totable
local function textonly(e,t)
  if e then
    local edt=e.dt
    if edt then
      for i=1,#edt do
        local e=edt[i]
        if type(e)=="table" then
          textonly(e,t)
        else
          t[#t+1]=e
        end
      end
    end
  end
  return t
end
function xml.textonly(e) 
  return concat(textonly(e,{}))
end
function finalizers.lowerall(collected)
  for c=1,#collected do
    local e=collected[c]
    if not e.special then
      e.tg=lower(e.tg)
      local eat=e.at
      if eat then
        local t={}
        for k,v in next,eat do
          t[lower(k)]=v
        end
        e.at=t
      end
    end
  end
end
function finalizers.upperall(collected)
  for c=1,#collected do
    local e=collected[c]
    if not e.special then
      e.tg=upper(e.tg)
      local eat=e.at
      if eat then
        local t={}
        for k,v in next,eat do
          t[upper(k)]=v
        end
        e.at=t
      end
    end
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-xml"] = package.loaded["trac-xml"] or true

-- original size: 6351, stripped down to: 4919

if not modules then modules={} end modules ['trac-xml']={
  version=1.001,
  comment="companion to trac-log.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local formatters=string.formatters
local reporters=logs.reporters
local xmlserialize=xml.serialize
local xmlcollected=xml.collected
local xmltext=xml.text
local xmlfirst=xml.first
local function showhelp(specification,...)
  local root=xml.convert(specification.helpinfo or "")
  if not root then
    return
  end
  local xs=xml.gethandlers("string")
  xml.sethandlersfunction(xs,"short",function(e,handler) xmlserialize(e.dt,handler) end)
  xml.sethandlersfunction(xs,"ref",function(e,handler) handler.handle("--"..e.at.name) end)
  local wantedcategories=select("#",...)==0 and true or table.tohash {... }
  local nofcategories=xml.count(root,"/application/flags/category")
  local report=specification.report
  for category in xmlcollected(root,"/application/flags/category") do
    local categoryname=category.at.name or ""
    if wantedcategories==true or wantedcategories[categoryname] then
      if nofcategories>1 then
        report("%s options:",categoryname)
        report()
      end
      for subcategory in xmlcollected(category,"/subcategory") do
        for flag in xmlcollected(subcategory,"/flag") do
          local name=flag.at.name
          local value=flag.at.value
          local short=xmltext(xmlfirst(flag,"/short"))
          if value then
            report("--%-20s %s",formatters["%s=%s"](name,value),short)
          else
            report("--%-20s %s",name,short)
          end
        end
        report()
      end
    end
  end
  for category in xmlcollected(root,"/application/examples/category") do
    local title=xmltext(xmlfirst(category,"/title"))
    if title and title~="" then
      report()
      report(title)
      report()
    end
    for subcategory in xmlcollected(category,"/subcategory") do
      for example in xmlcollected(subcategory,"/example") do
        local command=xmltext(xmlfirst(example,"/command"))
        local comment=xmltext(xmlfirst(example,"/comment"))
        report(command)
      end
      report()
    end
  end
  for comment in xmlcollected(root,"/application/comments/comment") do
    local comment=xmltext(comment)
    report()
    report(comment)
    report()
  end
end
local reporthelp=reporters.help
local exporthelp=reporters.export
local function xmlfound(t)
  local helpinfo=t.helpinfo
  if type(helpinfo)=="table" then
    return false
  end
  if type(helpinfo)~="string" then
    helpinfo="Warning: no helpinfo found."
    t.helpinfo=helpinfo
    return false
  end
  if string.find(helpinfo,".xml$") then
    local ownscript=environment.ownscript
    local helpdata=false
    if ownscript then
      local helpfile=file.join(file.pathpart(ownscript),helpinfo)
      helpdata=io.loaddata(helpfile)
      if helpdata=="" then
        helpdata=false
      end
    end
    if not helpdata then
      local helpfile=resolvers.findfile(helpinfo,"tex")
      helpdata=helpfile and io.loaddata(helpfile)
    end
    if helpdata and helpdata~="" then
      helpinfo=helpdata
    else
      helpinfo=formatters["Warning: help file %a is not found."](helpinfo)
    end
  end
  t.helpinfo=helpinfo
  return string.find(t.helpinfo,"^<%?xml") and true or false
end
function reporters.help(t,...)
  if xmlfound(t) then
    showhelp(t,...)
  else
    reporthelp(t,...)
  end
end
function reporters.export(t,methods,filename)
  if not xmlfound(t) then
    return exporthelp(t)
  end
  if not methods or methods=="" then
    methods=environment.arguments["exporthelp"]
  end
  if not filename or filename=="" then
    filename=environment.files[1]
  end
  dofile(resolvers.findfile("trac-exp.lua","tex"))
  local exporters=logs.exporters
  if not exporters or not methods then
    return exporthelp(t)
  end
  if methods=="all" then
    methods=table.keys(exporters)
  elseif type(methods)=="string" then
    methods=utilities.parsers.settings_to_array(methods)
  else
    return exporthelp(t)
  end
  if type(filename)~="string" or filename=="" then
    filename=false
  elseif file.pathpart(filename)=="" then
    t.report("export file %a will not be saved on the current path (safeguard)",filename)
    return
  end
  for i=1,#methods do
    local method=methods[i]
    local exporter=exporters[method]
    if exporter then
      local result=exporter(t,method)
      if result and result~="" then
        if filename then
          local fullname=file.replacesuffix(filename,method)
          t.report("saving export in %a",fullname)
          io.savedata(fullname,result)
        else
          reporters.lines(t,result)
        end
      else
        t.report("no output from exporter %a",method)
      end
    else
      t.report("unknown exporter %a",method)
    end
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-ini"] = package.loaded["data-ini"] or true

-- original size: 7898, stripped down to: 5501

if not modules then modules={} end modules ['data-ini']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files",
}
local gsub,find,gmatch,char=string.gsub,string.find,string.gmatch,string.char
local next,type=next,type
local filedirname,filebasename,filejoin=file.dirname,file.basename,file.join
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local trace_detail=false trackers.register("resolvers.details",function(v) trace_detail=v end)
local trace_expansions=false trackers.register("resolvers.expansions",function(v) trace_expansions=v end)
local report_initialization=logs.reporter("resolvers","initialization")
local ostype,osname,ossetenv,osgetenv=os.type,os.name,os.setenv,os.getenv
resolvers=resolvers or {}
local resolvers=resolvers
texconfig.kpse_init=false
texconfig.shell_escape='t'
if not (environment and environment.default_texmfcnf) and kpse and kpse.default_texmfcnf then
  local default_texmfcnf=kpse.default_texmfcnf()
  default_texmfcnf=gsub(default_texmfcnf,"$SELFAUTOLOC","selfautoloc:")
  default_texmfcnf=gsub(default_texmfcnf,"$SELFAUTODIR","selfautodir:")
  default_texmfcnf=gsub(default_texmfcnf,"$SELFAUTOPARENT","selfautoparent:")
  default_texmfcnf=gsub(default_texmfcnf,"$HOME","home:")
  environment.default_texmfcnf=default_texmfcnf
end
kpse={ original=kpse }
setmetatable(kpse,{
  __index=function(kp,name)
    report_initialization("fatal error: kpse library is accessed (key: %s)",name)
    os.exit()
  end
} )
do
  local osfontdir=osgetenv("OSFONTDIR")
  if osfontdir and osfontdir~="" then
  elseif osname=="windows" then
    ossetenv("OSFONTDIR","c:/windows/fonts//")
  elseif osname=="macosx" then
    ossetenv("OSFONTDIR","$HOME/Library/Fonts//;/Library/Fonts//;/System/Library/Fonts//")
  end
end
do
  local homedir=osgetenv(ostype=="windows" and 'USERPROFILE' or 'HOME') or ''
  if not homedir or homedir=="" then
    homedir=char(127) 
  end
  homedir=file.collapsepath(homedir)
  ossetenv("HOME",homedir) 
  ossetenv("USERPROFILE",homedir) 
  environment.homedir=homedir
end
do
  local args=environment.originalarguments or arg 
  if not environment.ownmain then
    environment.ownmain=status and string.match(string.lower(status.banner),"this is ([%a]+)") or "luatex"
  end
  local ownbin=environment.ownbin or args[-2] or arg[-2] or args[-1] or arg[-1] or arg[0] or "luatex"
  local ownpath=environment.ownpath or os.selfdir
  ownbin=file.collapsepath(ownbin)
  ownpath=file.collapsepath(ownpath)
  if not ownpath or ownpath=="" or ownpath=="unset" then
    ownpath=args[-1] or arg[-1]
    ownpath=ownpath and filedirname(gsub(ownpath,"\\","/"))
    if not ownpath or ownpath=="" then
      ownpath=args[-0] or arg[-0]
      ownpath=ownpath and filedirname(gsub(ownpath,"\\","/"))
    end
    local binary=ownbin
    if not ownpath or ownpath=="" then
      ownpath=ownpath and filedirname(binary)
    end
    if not ownpath or ownpath=="" then
      if os.binsuffix~="" then
        binary=file.replacesuffix(binary,os.binsuffix)
      end
      local path=osgetenv("PATH")
      if path then
        for p in gmatch(path,"[^"..io.pathseparator.."]+") do
          local b=filejoin(p,binary)
          if lfs.isfile(b) then
            local olddir=lfs.currentdir()
            if lfs.chdir(p) then
              local pp=lfs.currentdir()
              if trace_locating and p~=pp then
                report_initialization("following symlink %a to %a",p,pp)
              end
              ownpath=pp
              lfs.chdir(olddir)
            else
              if trace_locating then
                report_initialization("unable to check path %a",p)
              end
              ownpath=p
            end
            break
          end
        end
      end
    end
    if not ownpath or ownpath=="" then
      ownpath="."
      report_initialization("forcing fallback to ownpath %a",ownpath)
    elseif trace_locating then
      report_initialization("using ownpath %a",ownpath)
    end
  end
  environment.ownbin=ownbin
  environment.ownpath=ownpath
end
resolvers.ownpath=environment.ownpath
function resolvers.getownpath()
  return environment.ownpath
end
do
  local ownpath=environment.ownpath or dir.current()
  if ownpath then
    ossetenv('SELFAUTOLOC',file.collapsepath(ownpath))
    ossetenv('SELFAUTODIR',file.collapsepath(ownpath.."/.."))
    ossetenv('SELFAUTOPARENT',file.collapsepath(ownpath.."/../.."))
  else
    report_initialization("error: unable to locate ownpath")
    os.exit()
  end
end
local texos=environment.texos  or osgetenv("TEXOS")
local texmfos=environment.texmfos or osgetenv('SELFAUTODIR')
if not texos or texos=="" then
  texos=file.basename(texmfos)
end
ossetenv('TEXMFOS',texmfos)   
ossetenv('TEXOS',texos)    
ossetenv('SELFAUTOSYSTEM',os.platform) 
environment.texos=texos
environment.texmfos=texmfos
local texroot=environment.texroot or osgetenv("TEXROOT")
if not texroot or texroot=="" then
  texroot=osgetenv('SELFAUTOPARENT')
  ossetenv('TEXROOT',texroot)
end
environment.texroot=file.collapsepath(texroot)
if profiler then
  directives.register("system.profile",function()
    profiler.start("luatex-profile.log")
  end)
end
if not resolvers.resolve then
  function resolvers.resolve (s) return s end
  function resolvers.unresolve(s) return s end
  function resolvers.repath  (s) return s end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-exp"] = package.loaded["data-exp"] or true

-- original size: 14643, stripped down to: 9517

if not modules then modules={} end modules ['data-exp']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files",
}
local format,find,gmatch,lower,char,sub=string.format,string.find,string.gmatch,string.lower,string.char,string.sub
local concat,sort=table.concat,table.sort
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local Ct,Cs,Cc,P,C,S=lpeg.Ct,lpeg.Cs,lpeg.Cc,lpeg.P,lpeg.C,lpeg.S
local type,next=type,next
local ostype=os.type
local collapsepath=file.collapsepath
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local trace_expansions=false trackers.register("resolvers.expansions",function(v) trace_expansions=v end)
local report_expansions=logs.reporter("resolvers","expansions")
local resolvers=resolvers
local function f_first(a,b)
  local t,n={},0
  for s in gmatch(b,"[^,]+") do
    n=n+1;t[n]=a..s
  end
  return concat(t,",")
end
local function f_second(a,b)
  local t,n={},0
  for s in gmatch(a,"[^,]+") do
    n=n+1;t[n]=s..b
  end
  return concat(t,",")
end
local function f_both(a,b)
  local t,n={},0
  for sb in gmatch(b,"[^,]+") do       
    for sa in gmatch(a,"[^,]+") do     
      n=n+1;t[n]=sa..sb
    end
  end
  return concat(t,",")
end
local left=P("{")
local right=P("}")
local var=P((1-S("{}" ))^0)
local set=P((1-S("{},"))^0)
local other=P(1)
local l_first=Cs((Cc("{")*(C(set)*left*C(var)*right/f_first)*Cc("}")+other )^0 )
local l_second=Cs((Cc("{")*(left*C(var)*right*C(set)/f_second)*Cc("}")+other )^0 )
local l_both=Cs((Cc("{")*(left*C(var)*right*left*C(var)*right/f_both)*Cc("}")+other )^0 )
local l_rest=Cs((left*var*(left/"")*var*(right/"")*var*right+other )^0 )
local stripper_1=lpeg.stripper ("{}@")
local replacer_1=lpeg.replacer { { ",}",",@}" },{ "{,","{@," },}
local function splitpathexpr(str,newlist,validate) 
  if trace_expansions then
    report_expansions("expanding variable %a",str)
  end
  local t,ok,done=newlist or {},false,false
  local n=#t
  str=lpegmatch(replacer_1,str)
  repeat
    local old=str
    repeat
      local old=str
      str=lpegmatch(l_first,str)
    until old==str
    repeat
      local old=str
      str=lpegmatch(l_second,str)
    until old==str
    repeat
      local old=str
      str=lpegmatch(l_both,str)
    until old==str
    repeat
      local old=str
      str=lpegmatch(l_rest,str)
    until old==str
  until old==str 
  str=lpegmatch(stripper_1,str)
  if validate then
    for s in gmatch(str,"[^,]+") do
      s=validate(s)
      if s then
        n=n+1
        t[n]=s
      end
    end
  else
    for s in gmatch(str,"[^,]+") do
      n=n+1
      t[n]=s
    end
  end
  if trace_expansions then
    for k=1,#t do
      report_expansions("% 4i: %s",k,t[k])
    end
  end
  return t
end
local function validate(s)
  s=collapsepath(s) 
  return s~="" and not find(s,"^!*unset/*$") and s
end
resolvers.validatedpath=validate 
function resolvers.expandedpathfromlist(pathlist)
  local newlist={}
  for k=1,#pathlist do
    splitpathexpr(pathlist[k],newlist,validate)
  end
  return newlist
end
local cleanup=lpeg.replacer {
  { "!","" },
  { "\\","/" },
}
function resolvers.cleanpath(str) 
  local doslashes=(P("\\")/"/"+1)^0
  local donegation=(P("!")/""   )^0
  local homedir=lpegmatch(Cs(donegation*doslashes),environment.homedir or "")
  if homedir=="~" or homedir=="" or not lfs.isdir(homedir) then
    if trace_expansions then
      report_expansions("no home dir set, ignoring dependent paths")
    end
    function resolvers.cleanpath(str)
      if not str or find(str,"~") then
        return "" 
      else
        return lpegmatch(cleanup,str)
      end
    end
  else
    local dohome=((P("~")+P("$HOME"))/homedir)^0
    local cleanup=Cs(donegation*dohome*doslashes)
    function resolvers.cleanpath(str)
      return str and lpegmatch(cleanup,str) or ""
    end
  end
  return resolvers.cleanpath(str)
end
local expandhome=P("~")/"$HOME" 
local dodouble=P('"')/""*(expandhome+(1-P('"')))^0*P('"')/""
local dosingle=P("'")/""*(expandhome+(1-P("'")))^0*P("'")/""
local dostring=(expandhome+1       )^0
local stripper=Cs(
  lpegpatterns.unspacer*(dosingle+dodouble+dostring)*lpegpatterns.unspacer
)
function resolvers.checkedvariable(str) 
  return type(str)=="string" and lpegmatch(stripper,str) or str
end
local cache={}
local splitter=lpeg.tsplitat(";") 
local backslashswapper=lpeg.replacer("\\","/")
local function splitconfigurationpath(str) 
  if str then
    local found=cache[str]
    if not found then
      if str=="" then
        found={}
      else
        local split=lpegmatch(splitter,lpegmatch(backslashswapper,str)) 
        found={}
        local noffound=0
        for i=1,#split do
          local s=split[i]
          if not find(s,"^{*unset}*") then
            noffound=noffound+1
            found[noffound]=s
          end
        end
        if trace_expansions then
          report_expansions("splitting path specification %a",str)
          for k=1,noffound do
            report_expansions("% 4i: %s",k,found[k])
          end
        end
        cache[str]=found
      end
    end
    return found
  end
end
resolvers.splitconfigurationpath=splitconfigurationpath
function resolvers.splitpath(str)
  if type(str)=='table' then
    return str
  else
    return splitconfigurationpath(str)
  end
end
function resolvers.joinpath(str)
  if type(str)=='table' then
    return file.joinpath(str)
  else
    return str
  end
end
local attributes,directory=lfs.attributes,lfs.dir
local weird=P(".")^1+lpeg.anywhere(S("~`!#$%^&*()={}[]:;\"\'||<>,?\n\r\t"))
local timer={}
local scanned={}
local nofscans=0
local scancache={}
local function scan(files,spec,path,n,m,r)
  local full=(path=="" and spec) or (spec..path..'/')
  local dirs={}
  local nofdirs=0
  for name in directory(full) do
    if not lpegmatch(weird,name) then
      local mode=attributes(full..name,'mode')
      if mode=='file' then
        n=n+1
        local f=files[name]
        if f then
          if type(f)=='string' then
            files[name]={ f,path }
          else
            f[#f+1]=path
          end
        else 
          files[name]=path
          local lower=lower(name)
          if name~=lower then
            files["remap:"..lower]=name
            r=r+1
          end
        end
      elseif mode=='directory' then
        m=m+1
        nofdirs=nofdirs+1
        if path~="" then
          dirs[nofdirs]=path..'/'..name
        else
          dirs[nofdirs]=name
        end
      end
    end
  end
  if nofdirs>0 then
    sort(dirs)
    for i=1,nofdirs do
      files,n,m,r=scan(files,spec,dirs[i],n,m,r)
    end
  end
  scancache[sub(full,1,-2)]=files
  return files,n,m,r
end
local fullcache={}
function resolvers.scanfiles(path,branch,usecache)
  statistics.starttiming(timer)
  local realpath=resolvers.resolve(path) 
  if usecache then
    local files=fullcache[realpath]
    if files then
      if trace_locating then
        report_expansions("using caches scan of path %a, branch %a",path,branch or path)
      end
      return files
    end
  end
  if trace_locating then
    report_expansions("scanning path %a, branch %a",path,branch or path)
  end
  local files,n,m,r=scan({},realpath..'/',"",0,0,0)
  files.__path__=path 
  files.__files__=n
  files.__directories__=m
  files.__remappings__=r
  if trace_locating then
    report_expansions("%s files found on %s directories with %s uppercase remappings",n,m,r)
  end
  if usecache then
    scanned[#scanned+1]=realpath
    fullcache[realpath]=files
  end
  nofscans=nofscans+1
  statistics.stoptiming(timer)
  return files
end
local function simplescan(files,spec,path) 
  local full=(path=="" and spec) or (spec..path..'/')
  local dirs={}
  local nofdirs=0
  for name in directory(full) do
    if not lpegmatch(weird,name) then
      local mode=attributes(full..name,'mode')
      if mode=='file' then
        if not files[name] then
          files[name]=path
        end
      elseif mode=='directory' then
        nofdirs=nofdirs+1
        if path~="" then
          dirs[nofdirs]=path..'/'..name
        else
          dirs[nofdirs]=name
        end
      end
    end
  end
  if nofdirs>0 then
    sort(dirs)
    for i=1,nofdirs do
      files=simplescan(files,spec,dirs[i])
    end
  end
  return files
end
local simplecache={}
local nofsharedscans=0
function resolvers.simplescanfiles(path,branch,usecache)
  statistics.starttiming(timer)
  local realpath=resolvers.resolve(path) 
  if usecache then
    local files=simplecache[realpath]
    if not files then
      files=scancache[realpath]
      if files then
        nofsharedscans=nofsharedscans+1
      end
    end
    if files then
      if trace_locating then
        report_expansions("using caches scan of path %a, branch %a",path,branch or path)
      end
      return files
    end
  end
  if trace_locating then
    report_expansions("scanning path %a, branch %a",path,branch or path)
  end
  local files=simplescan({},realpath..'/',"")
  if trace_locating then
    report_expansions("%s files found",table.count(files))
  end
  if usecache then
    scanned[#scanned+1]=realpath
    simplecache[realpath]=files
  end
  nofscans=nofscans+1
  statistics.stoptiming(timer)
  return files
end
function resolvers.scandata()
  table.sort(scanned)
  return {
    n=nofscans,
    shared=nofsharedscans,
    time=statistics.elapsedtime(timer),
    paths=scanned,
  }
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-env"] = package.loaded["data-env"] or true

-- original size: 8762, stripped down to: 6484

if not modules then modules={} end modules ['data-env']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files",
}
local lower,gsub=string.lower,string.gsub
local resolvers=resolvers
local allocate=utilities.storage.allocate
local setmetatableindex=table.setmetatableindex
local suffixonly=file.suffixonly
local formats=allocate()
local suffixes=allocate()
local dangerous=allocate()
local suffixmap=allocate()
resolvers.formats=formats
resolvers.suffixes=suffixes
resolvers.dangerous=dangerous
resolvers.suffixmap=suffixmap
local luasuffixes=utilities.lua.suffixes
local relations=allocate { 
  core={
    ofm={ 
      names={ "ofm","omega font metric","omega font metrics" },
      variable='OFMFONTS',
      suffixes={ 'ofm','tfm' },
    },
    ovf={ 
      names={ "ovf","omega virtual font","omega virtual fonts" },
      variable='OVFFONTS',
      suffixes={ 'ovf','vf' },
    },
    tfm={
      names={ "tfm","tex font metric","tex font metrics" },
      variable='TFMFONTS',
      suffixes={ 'tfm' },
    },
    vf={
      names={ "vf","virtual font","virtual fonts" },
      variable='VFFONTS',
      suffixes={ 'vf' },
    },
    otf={
      names={ "otf","opentype","opentype font","opentype fonts"},
      variable='OPENTYPEFONTS',
      suffixes={ 'otf' },
    },
    ttf={
      names={ "ttf","truetype","truetype font","truetype fonts","truetype collection","truetype collections","truetype dictionary","truetype dictionaries" },
      variable='TTFONTS',
      suffixes={ 'ttf','ttc','dfont' },
    },
    afm={
      names={ "afm","adobe font metric","adobe font metrics" },
      variable="AFMFONTS",
      suffixes={ "afm" },
    },
    pfb={
      names={ "pfb","type1","type 1","type1 font","type 1 font","type1 fonts","type 1 fonts" },
      variable='T1FONTS',
      suffixes={ 'pfb','pfa' },
    },
    fea={
      names={ "fea","font feature","font features","font feature file","font feature files" },
      variable='FONTFEATURES',
      suffixes={ 'fea' },
    },
    cid={
      names={ "cid","cid map","cid maps","cid file","cid files" },
      variable='FONTCIDMAPS',
      suffixes={ 'cid','cidmap' },
    },
    fmt={
      names={ "fmt","format","tex format" },
      variable='TEXFORMATS',
      suffixes={ 'fmt' },
    },
    mem={ 
      names={ 'mem',"metapost format" },
      variable='MPMEMS',
      suffixes={ 'mem' },
    },
    mp={
      names={ "mp" },
      variable='MPINPUTS',
      suffixes={ 'mp','mpvi','mpiv','mpii' },
    },
    tex={
      names={ "tex" },
      variable='TEXINPUTS',
      suffixes={ 'tex',"mkvi","mkiv","mkii" },
    },
    icc={
      names={ "icc","icc profile","icc profiles" },
      variable='ICCPROFILES',
      suffixes={ 'icc' },
    },
    texmfscripts={
      names={ "texmfscript","texmfscripts","script","scripts" },
      variable='TEXMFSCRIPTS',
      suffixes={ 'rb','pl','py' },
    },
    lua={
      names={ "lua" },
      variable='LUAINPUTS',
      suffixes={ luasuffixes.lua,luasuffixes.luc,luasuffixes.tma,luasuffixes.tmc },
    },
    lib={
      names={ "lib" },
      variable='CLUAINPUTS',
      suffixes=os.libsuffix and { os.libsuffix } or { 'dll','so' },
    },
    bib={
      names={ 'bib' },
      suffixes={ 'bib' },
    },
    bst={
      names={ 'bst' },
      suffixes={ 'bst' },
    },
    fontconfig={
      names={ 'fontconfig','fontconfig file','fontconfig files' },
      variable='FONTCONFIG_PATH',
    },
  },
  obsolete={
    enc={
      names={ "enc","enc files","enc file","encoding files","encoding file" },
      variable='ENCFONTS',
      suffixes={ 'enc' },
    },
    map={
      names={ "map","map files","map file" },
      variable='TEXFONTMAPS',
      suffixes={ 'map' },
    },
    lig={
      names={ "lig files","lig file","ligature file","ligature files" },
      variable='LIGFONTS',
      suffixes={ 'lig' },
    },
    opl={
      names={ "opl" },
      variable='OPLFONTS',
      suffixes={ 'opl' },
    },
    ovp={
      names={ "ovp" },
      variable='OVPFONTS',
      suffixes={ 'ovp' },
    },
  },
  kpse={ 
    base={
      names={ 'base',"metafont format" },
      variable='MFBASES',
      suffixes={ 'base','bas' },
    },
    cmap={
      names={ 'cmap','cmap files','cmap file' },
      variable='CMAPFONTS',
      suffixes={ 'cmap' },
    },
    cnf={
      names={ 'cnf' },
      suffixes={ 'cnf' },
    },
    web={
      names={ 'web' },
      suffixes={ 'web','ch' }
    },
    cweb={
      names={ 'cweb' },
      suffixes={ 'w','web','ch' },
    },
    gf={
      names={ 'gf' },
      suffixes={ '<resolution>gf' },
    },
    mf={
      names={ 'mf' },
      variable='MFINPUTS',
      suffixes={ 'mf' },
    },
    mft={
      names={ 'mft' },
      suffixes={ 'mft' },
    },
    pk={
      names={ 'pk' },
      suffixes={ '<resolution>pk' },
    },
  },
}
resolvers.relations=relations
function resolvers.updaterelations()
  for category,categories in next,relations do
    for name,relation in next,categories do
      local rn=relation.names
      local rv=relation.variable
      local rs=relation.suffixes
      if rn and rv then
        for i=1,#rn do
          local rni=lower(gsub(rn[i]," ",""))
          formats[rni]=rv
          if rs then
            suffixes[rni]=rs
            for i=1,#rs do
              local rsi=rs[i]
              suffixmap[rsi]=rni
            end
          end
        end
      end
      if rs then
      end
    end
  end
end
resolvers.updaterelations() 
local function simplified(t,k)
  return k and rawget(t,lower(gsub(k," ",""))) or nil
end
setmetatableindex(formats,simplified)
setmetatableindex(suffixes,simplified)
setmetatableindex(suffixmap,simplified)
function resolvers.suffixofformat(str)
  local s=suffixes[str]
  return s and s[1] or ""
end
function resolvers.suffixofformat(str)
  return suffixes[str] or {}
end
for name,format in next,formats do
  dangerous[name]=true 
end
dangerous.tex=nil
function resolvers.formatofvariable(str)
  return formats[str] or ''
end
function resolvers.formatofsuffix(str) 
  return suffixmap[suffixonly(str)] or 'tex' 
end
function resolvers.variableofformat(str)
  return formats[str] or ''
end
function resolvers.variableofformatorsuffix(str)
  local v=formats[str]
  if v then
    return v
  end
  v=suffixmap[suffixonly(str)]
  if v then
    return formats[v]
  end
  return ''
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-tmp"] = package.loaded["data-tmp"] or true

-- original size: 14308, stripped down to: 10956

if not modules then modules={} end modules ['data-tmp']={
  version=1.100,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local format,lower,gsub,concat=string.format,string.lower,string.gsub,table.concat
local serialize,serializetofile=table.serialize,table.tofile
local mkdirs,isdir,isfile=dir.mkdirs,lfs.isdir,lfs.isfile
local addsuffix,is_writable,is_readable=file.addsuffix,file.is_writable,file.is_readable
local formatters=string.formatters
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local trace_cache=false trackers.register("resolvers.cache",function(v) trace_cache=v end)
local report_caches=logs.reporter("resolvers","caches")
local report_resolvers=logs.reporter("resolvers","caching")
local resolvers=resolvers
local directive_cleanup=false directives.register("system.compile.cleanup",function(v) directive_cleanup=v end)
local directive_strip=false directives.register("system.compile.strip",function(v) directive_strip=v end)
local compile=utilities.lua.compile
function utilities.lua.compile(luafile,lucfile,cleanup,strip)
  if cleanup==nil then cleanup=directive_cleanup end
  if strip==nil then strip=directive_strip  end
  return compile(luafile,lucfile,cleanup,strip)
end
caches=caches or {}
local caches=caches
local luasuffixes=utilities.lua.suffixes
caches.base=caches.base or "luatex-cache"
caches.more=caches.more or "context"
caches.direct=false 
caches.tree=false
caches.force=true
caches.ask=false
caches.relocate=false
caches.defaults={ "TMPDIR","TEMPDIR","TMP","TEMP","HOME","HOMEPATH" }
local writable,readables,usedreadables=nil,{},{}
local function identify()
  local texmfcaches=resolvers.cleanpathlist("TEXMFCACHE")
  if texmfcaches then
    for k=1,#texmfcaches do
      local cachepath=texmfcaches[k]
      if cachepath~="" then
        cachepath=resolvers.resolve(cachepath)
        cachepath=resolvers.cleanpath(cachepath)
        cachepath=file.collapsepath(cachepath)
        local valid=isdir(cachepath)
        if valid then
          if is_readable(cachepath) then
            readables[#readables+1]=cachepath
            if not writable and is_writable(cachepath) then
              writable=cachepath
            end
          end
        elseif not writable and caches.force then
          local cacheparent=file.dirname(cachepath)
          if is_writable(cacheparent) and true then 
            if not caches.ask or io.ask(format("\nShould I create the cache path %s?",cachepath),"no",{ "yes","no" })=="yes" then
              mkdirs(cachepath)
              if isdir(cachepath) and is_writable(cachepath) then
                report_caches("path %a created",cachepath)
                writable=cachepath
                readables[#readables+1]=cachepath
              end
            end
          end
        end
      end
    end
  end
  local texmfcaches=caches.defaults
  if texmfcaches then
    for k=1,#texmfcaches do
      local cachepath=texmfcaches[k]
      cachepath=resolvers.expansion(cachepath) 
      if cachepath~="" then
        cachepath=resolvers.resolve(cachepath)
        cachepath=resolvers.cleanpath(cachepath)
        local valid=isdir(cachepath)
        if valid and is_readable(cachepath) then
          if not writable and is_writable(cachepath) then
            readables[#readables+1]=cachepath
            writable=cachepath
            break
          end
        end
      end
    end
  end
  if not writable then
    report_caches("fatal error: there is no valid writable cache path defined")
    os.exit()
  elseif #readables==0 then
    report_caches("fatal error: there is no valid readable cache path defined")
    os.exit()
  end
  writable=dir.expandname(resolvers.cleanpath(writable))
  local base,more,tree=caches.base,caches.more,caches.tree or caches.treehash() 
  if tree then
    caches.tree=tree
    writable=mkdirs(writable,base,more,tree)
    for i=1,#readables do
      readables[i]=file.join(readables[i],base,more,tree)
    end
  else
    writable=mkdirs(writable,base,more)
    for i=1,#readables do
      readables[i]=file.join(readables[i],base,more)
    end
  end
  if trace_cache then
    for i=1,#readables do
      report_caches("using readable path %a (order %s)",readables[i],i)
    end
    report_caches("using writable path %a",writable)
  end
  identify=function()
    return writable,readables
  end
  return writable,readables
end
function caches.usedpaths()
  local writable,readables=identify()
  if #readables>1 then
    local result={}
    for i=1,#readables do
      local readable=readables[i]
      if usedreadables[i] or readable==writable then
        result[#result+1]=formatters["readable: %a (order %s)"](readable,i)
      end
    end
    result[#result+1]=formatters["writable: %a"](writable)
    return result
  else
    return writable
  end
end
function caches.configfiles()
  return concat(resolvers.instance.specification,";")
end
function caches.hashed(tree)
  tree=gsub(tree,"[\\/]+$","")
  tree=lower(tree)
  local hash=md5.hex(tree)
  if trace_cache or trace_locating then
    report_caches("hashing tree %a, hash %a",tree,hash)
  end
  return hash
end
function caches.treehash()
  local tree=caches.configfiles()
  if not tree or tree=="" then
    return false
  else
    return caches.hashed(tree)
  end
end
local r_cache,w_cache={},{} 
local function getreadablepaths(...)
  local tags={... }
  local hash=concat(tags,"/")
  local done=r_cache[hash]
  if not done then
    local writable,readables=identify() 
    if #tags>0 then
      done={}
      for i=1,#readables do
        done[i]=file.join(readables[i],...)
      end
    else
      done=readables
    end
    r_cache[hash]=done
  end
  return done
end
local function getwritablepath(...)
  local tags={... }
  local hash=concat(tags,"/")
  local done=w_cache[hash]
  if not done then
    local writable,readables=identify() 
    if #tags>0 then
      done=mkdirs(writable,...)
    else
      done=writable
    end
    w_cache[hash]=done
  end
  return done
end
caches.getreadablepaths=getreadablepaths
caches.getwritablepath=getwritablepath
function caches.getfirstreadablefile(filename,...)
  local rd=getreadablepaths(...)
  for i=1,#rd do
    local path=rd[i]
    local fullname=file.join(path,filename)
    if is_readable(fullname) then
      usedreadables[i]=true
      return fullname,path
    end
  end
  return caches.setfirstwritablefile(filename,...)
end
function caches.setfirstwritablefile(filename,...)
  local wr=getwritablepath(...)
  local fullname=file.join(wr,filename)
  return fullname,wr
end
function caches.define(category,subcategory) 
  return function()
    return getwritablepath(category,subcategory)
  end
end
function caches.setluanames(path,name)
  return format("%s/%s.%s",path,name,luasuffixes.tma),format("%s/%s.%s",path,name,luasuffixes.tmc)
end
function caches.loaddata(readables,name)
  if type(readables)=="string" then
    readables={ readables }
  end
  for i=1,#readables do
    local path=readables[i]
    local tmaname,tmcname=caches.setluanames(path,name)
    local loader=false
    if isfile(tmcname) then
      loader=loadfile(tmcname)
    end
    if not loader and isfile(tmaname) then
      utilities.lua.compile(tmaname,tmcname)
      if isfile(tmcname) then
        loader=loadfile(tmcname)
      end
      if not loader then
        loader=loadfile(tmaname)
      end
    end
    if loader then
      loader=loader()
      collectgarbage("step")
      return loader
    end
  end
  return false
end
function caches.is_writable(filepath,filename)
  local tmaname,tmcname=caches.setluanames(filepath,filename)
  return is_writable(tmaname)
end
local saveoptions={ compact=true }
function caches.savedata(filepath,filename,data,raw)
  local tmaname,tmcname=caches.setluanames(filepath,filename)
  local reduce,simplify=true,true
  if raw then
    reduce,simplify=false,false
  end
  data.cache_uuid=os.uuid()
  if caches.direct then
    file.savedata(tmaname,serialize(data,true,saveoptions))
  else
    serializetofile(tmaname,data,true,saveoptions)
  end
  utilities.lua.compile(tmaname,tmcname)
end
local content_state={}
function caches.contentstate()
  return content_state or {}
end
function caches.loadcontent(cachename,dataname)
  local name=caches.hashed(cachename)
  local full,path=caches.getfirstreadablefile(addsuffix(name,luasuffixes.lua),"trees")
  local filename=file.join(path,name)
  local blob=loadfile(addsuffix(filename,luasuffixes.luc)) or loadfile(addsuffix(filename,luasuffixes.lua))
  if blob then
    local data=blob()
    if data and data.content then
      if data.type==dataname then
        if data.version==resolvers.cacheversion then
          content_state[#content_state+1]=data.uuid
          if trace_locating then
            report_resolvers("loading %a for %a from %a",dataname,cachename,filename)
          end
          return data.content
        else
          report_resolvers("skipping %a for %a from %a (version mismatch)",dataname,cachename,filename)
        end
      else
        report_resolvers("skipping %a for %a from %a (datatype mismatch)",dataname,cachename,filename)
      end
    elseif trace_locating then
      report_resolvers("skipping %a for %a from %a (no content)",dataname,cachename,filename)
    end
  elseif trace_locating then
    report_resolvers("skipping %a for %a from %a (invalid file)",dataname,cachename,filename)
  end
end
function caches.collapsecontent(content)
  for k,v in next,content do
    if type(v)=="table" and #v==1 then
      content[k]=v[1]
    end
  end
end
function caches.savecontent(cachename,dataname,content)
  local name=caches.hashed(cachename)
  local full,path=caches.setfirstwritablefile(addsuffix(name,luasuffixes.lua),"trees")
  local filename=file.join(path,name) 
  local luaname=addsuffix(filename,luasuffixes.lua)
  local lucname=addsuffix(filename,luasuffixes.luc)
  if trace_locating then
    report_resolvers("preparing %a for %a",dataname,cachename)
  end
  local data={
    type=dataname,
    root=cachename,
    version=resolvers.cacheversion,
    date=os.date("%Y-%m-%d"),
    time=os.date("%H:%M:%S"),
    content=content,
    uuid=os.uuid(),
  }
  local ok=io.savedata(luaname,serialize(data,true))
  if ok then
    if trace_locating then
      report_resolvers("category %a, cachename %a saved in %a",dataname,cachename,luaname)
    end
    if utilities.lua.compile(luaname,lucname) then
      if trace_locating then
        report_resolvers("%a compiled to %a",dataname,lucname)
      end
      return true
    else
      if trace_locating then
        report_resolvers("compiling failed for %a, deleting file %a",dataname,lucname)
      end
      os.remove(lucname)
    end
  elseif trace_locating then
    report_resolvers("unable to save %a in %a (access error)",dataname,luaname)
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-met"] = package.loaded["data-met"] or true

-- original size: 4915, stripped down to: 3942

if not modules then modules={} end modules ['data-met']={
  version=1.100,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local find,format=string.find,string.format
local sequenced=table.sequenced
local addurlscheme,urlhashed=url.addscheme,url.hashed
local trace_locating=false
trackers.register("resolvers.locating",function(v) trace_methods=v end)
trackers.register("resolvers.methods",function(v) trace_methods=v end)
local report_methods=logs.reporter("resolvers","methods")
local allocate=utilities.storage.allocate
local resolvers=resolvers
local registered={}
local function splitmethod(filename) 
  if not filename then
    return { scheme="unknown",original=filename }
  end
  if type(filename)=="table" then
    return filename 
  end
  filename=file.collapsepath(filename)
  if not find(filename,"://") then
    return { scheme="file",path=filename,original=filename,filename=filename }
  end
  local specification=url.hashed(filename)
  if not specification.scheme or specification.scheme=="" then
    return { scheme="file",path=filename,original=filename,filename=filename }
  else
    return specification
  end
end
resolvers.splitmethod=splitmethod
local function methodhandler(what,first,...) 
  local method=registered[what]
  if method then
    local how,namespace=method.how,method.namespace
    if how=="uri" or how=="url" then
      local specification=splitmethod(first)
      local scheme=specification.scheme
      local resolver=namespace and namespace[scheme]
      if resolver then
        if trace_methods then
          report_methods("resolving, method %a, how %a, handler %a, argument %a",what,how,scheme,first)
        end
        return resolver(specification,...)
      else
        resolver=namespace.default or namespace.file
        if resolver then
          if trace_methods then
            report_methods("resolving, method %a, how %a, handler %a, argument %a",what,how,"default",first)
          end
          return resolver(specification,...)
        elseif trace_methods then
          report_methods("resolving, method %a, how %a, handler %a, argument %a",what,how,"unset")
        end
      end
    elseif how=="tag" then
      local resolver=namespace and namespace[first]
      if resolver then
        if trace_methods then
          report_methods("resolving, method %a, how %a, tag %a",what,how,first)
        end
        return resolver(...)
      else
        resolver=namespace.default or namespace.file
        if resolver then
          if trace_methods then
            report_methods("resolving, method %a, how %a, tag %a",what,how,"default")
          end
          return resolver(...)
        elseif trace_methods then
          report_methods("resolving, method %a, how %a, tag %a",what,how,"unset")
        end
      end
    end
  else
    report_methods("resolving, invalid method %a")
  end
end
resolvers.methodhandler=methodhandler
function resolvers.registermethod(name,namespace,how)
  registered[name]={ how=how or "tag",namespace=namespace }
  namespace["byscheme"]=function(scheme,filename,...)
    if scheme=="file" then
      return methodhandler(name,filename,...)
    else
      return methodhandler(name,addurlscheme(filename,scheme),...)
    end
  end
end
local concatinators=allocate { notfound=file.join    } 
local locators=allocate { notfound=function() end } 
local hashers=allocate { notfound=function() end } 
local generators=allocate { notfound=function() end } 
resolvers.concatinators=concatinators
resolvers.locators=locators
resolvers.hashers=hashers
resolvers.generators=generators
local registermethod=resolvers.registermethod
registermethod("concatinators",concatinators,"tag")
registermethod("locators",locators,"uri")
registermethod("hashers",hashers,"uri")
registermethod("generators",generators,"uri")


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-res"] = package.loaded["data-res"] or true

-- original size: 60857, stripped down to: 42496

if not modules then modules={} end modules ['data-res']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files",
}
local gsub,find,lower,upper,match,gmatch=string.gsub,string.find,string.lower,string.upper,string.match,string.gmatch
local concat,insert,sortedkeys=table.concat,table.insert,table.sortedkeys
local next,type,rawget=next,type,rawget
local os=os
local P,S,R,C,Cc,Cs,Ct,Carg=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.Cc,lpeg.Cs,lpeg.Ct,lpeg.Carg
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local formatters=string.formatters
local filedirname=file.dirname
local filebasename=file.basename
local suffixonly=file.suffixonly
local filejoin=file.join
local collapsepath=file.collapsepath
local joinpath=file.joinpath
local allocate=utilities.storage.allocate
local settings_to_array=utilities.parsers.settings_to_array
local setmetatableindex=table.setmetatableindex
local luasuffixes=utilities.lua.suffixes
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local trace_detail=false trackers.register("resolvers.details",function(v) trace_detail=v end)
local trace_expansions=false trackers.register("resolvers.expansions",function(v) trace_expansions=v end)
local report_resolving=logs.reporter("resolvers","resolving")
local resolvers=resolvers
local expandedpathfromlist=resolvers.expandedpathfromlist
local checkedvariable=resolvers.checkedvariable
local splitconfigurationpath=resolvers.splitconfigurationpath
local methodhandler=resolvers.methodhandler
local initializesetter=utilities.setters.initialize
local ostype,osname,osenv,ossetenv,osgetenv=os.type,os.name,os.env,os.setenv,os.getenv
resolvers.cacheversion='1.0.1'
resolvers.configbanner=''
resolvers.homedir=environment.homedir
resolvers.criticalvars=allocate { "SELFAUTOLOC","SELFAUTODIR","SELFAUTOPARENT","TEXMFCNF","TEXMF","TEXOS" }
resolvers.luacnfname="texmfcnf.lua"
resolvers.luacnfstate="unknown"
if environment.default_texmfcnf then
  resolvers.luacnfspec=environment.default_texmfcnf
else
  resolvers.luacnfspec="{selfautoloc:,selfautodir:,selfautoparent:}{,/texmf{-local,}/web2c}"
end
resolvers.luacnfspec='home:texmf/web2c;'..resolvers.luacnfspec
local unset_variable="unset"
local formats=resolvers.formats
local suffixes=resolvers.suffixes
local dangerous=resolvers.dangerous
local suffixmap=resolvers.suffixmap
resolvers.defaultsuffixes={ "tex" } 
resolvers.instance=resolvers.instance or nil 
local   instance=resolvers.instance or nil
function resolvers.setenv(key,value,raw)
  if instance then
    instance.environment[key]=value
    ossetenv(key,raw and value or resolvers.resolve(value))
  end
end
local function getenv(key)
  local value=rawget(instance.environment,key)
  if value and value~="" then
    return value
  else
    local e=osgetenv(key)
    return e~=nil and e~="" and checkedvariable(e) or ""
  end
end
resolvers.getenv=getenv
resolvers.env=getenv
local function resolve(k)
  return instance.expansions[k]
end
local dollarstripper=lpeg.stripper("$")
local inhibitstripper=P("!")^0*Cs(P(1)^0)
local backslashswapper=lpeg.replacer("\\","/")
local somevariable=P("$")/""
local somekey=C(R("az","AZ","09","__","--")^1)
local somethingelse=P(";")*((1-S("!{}/\\"))^1*P(";")/"")+P(";")*(P(";")/"")+P(1)
local variableexpander=Cs((somevariable*(somekey/resolve)+somethingelse)^1 )
local cleaner=P("\\")/"/"+P(";")*S("!{}/\\")^0*P(";")^1/";"
local variablecleaner=Cs((cleaner+P(1))^0)
local somevariable=R("az","AZ","09","__","--")^1/resolve
local variable=(P("$")/"")*(somevariable+(P("{")/"")*somevariable*(P("}")/""))
local variableresolver=Cs((variable+P(1))^0)
local function expandedvariable(var)
  return lpegmatch(variableexpander,var) or var
end
function resolvers.newinstance() 
   if trace_locating then
    report_resolving("creating instance")
   end
  local environment,variables,expansions,order=allocate(),allocate(),allocate(),allocate()
  local newinstance={
    environment=environment,
    variables=variables,
    expansions=expansions,
    order=order,
    files=allocate(),
    setups=allocate(),
    found=allocate(),
    foundintrees=allocate(),
    hashes=allocate(),
    hashed=allocate(),
    specification=allocate(),
    lists=allocate(),
    data=allocate(),
    fakepaths=allocate(),
    remember=true,
    diskcache=true,
    renewcache=false,
    renewtree=false,
    loaderror=false,
    savelists=true,
    pattern=nil,
    force_suffixes=true,
  }
  setmetatableindex(variables,function(t,k)
    local v
    for i=1,#order do
      v=order[i][k]
      if v~=nil then
        t[k]=v
        return v
      end
    end
    if v==nil then
      v=""
    end
    t[k]=v
    return v
  end)
  setmetatableindex(environment,function(t,k)
    local v=osgetenv(k)
    if v==nil then
      v=variables[k]
    end
    if v~=nil then
      v=checkedvariable(v) or ""
    end
    v=resolvers.repath(v) 
    t[k]=v
    return v
  end)
  setmetatableindex(expansions,function(t,k)
    local v=environment[k]
    if type(v)=="string" then
      v=lpegmatch(variableresolver,v)
      v=lpegmatch(variablecleaner,v)
    end
    t[k]=v
    return v
  end)
  return newinstance
end
function resolvers.setinstance(someinstance) 
  instance=someinstance
  resolvers.instance=someinstance
  return someinstance
end
function resolvers.reset()
  return resolvers.setinstance(resolvers.newinstance())
end
local function reset_hashes()
  instance.lists={}
  instance.found={}
end
local slash=P("/")
local pathexpressionpattern=Cs (
  Cc("^")*(
    Cc("%")*S(".-")+slash^2*P(-1)/"/.*"
+slash^2/"/"+(1-slash)*P(-1)*Cc("/")+P(1)
  )^1*Cc("$") 
)
local cache={}
local function makepathexpression(str)
  if str=="." then
    return "^%./$"
  else
    local c=cache[str]
    if not c then
      c=lpegmatch(pathexpressionpattern,str)
      cache[str]=c
    end
    return c
  end
end
local function reportcriticalvariables(cnfspec)
  if trace_locating then
    for i=1,#resolvers.criticalvars do
      local k=resolvers.criticalvars[i]
      local v=resolvers.getenv(k) or "unknown" 
      report_resolving("variable %a set to %a",k,v)
    end
    report_resolving()
    if cnfspec then
      report_resolving("using configuration specification %a",type(cnfspec)=="table" and concat(cnfspec,",") or cnfspec)
    end
    report_resolving()
  end
  reportcriticalvariables=function() end
end
local function identify_configuration_files()
  local specification=instance.specification
  if #specification==0 then
    local cnfspec=getenv("TEXMFCNF")
    if cnfspec=="" then
      cnfspec=resolvers.luacnfspec
      resolvers.luacnfstate="default"
    else
      resolvers.luacnfstate="environment"
    end
    reportcriticalvariables(cnfspec)
    local cnfpaths=expandedpathfromlist(resolvers.splitpath(cnfspec))
    local luacnfname=resolvers.luacnfname
    for i=1,#cnfpaths do
      local filename=collapsepath(filejoin(cnfpaths[i],luacnfname))
      local realname=resolvers.resolve(filename)
      if lfs.isfile(realname) then
        specification[#specification+1]=filename
        if trace_locating then
          report_resolving("found configuration file %a",realname)
        end
      elseif trace_locating then
        report_resolving("unknown configuration file %a",realname)
      end
    end
    if trace_locating then
      report_resolving()
    end
  elseif trace_locating then
    report_resolving("configuration files already identified")
  end
end
local function load_configuration_files()
  local specification=instance.specification
  if #specification>0 then
    local luacnfname=resolvers.luacnfname
    for i=1,#specification do
      local filename=specification[i]
      local pathname=filedirname(filename)
      local filename=filejoin(pathname,luacnfname)
      local realname=resolvers.resolve(filename) 
      local blob=loadfile(realname)
      if blob then
        local setups=instance.setups
        local data=blob()
        local parent=data and data.parent
        if parent then
          local filename=filejoin(pathname,parent)
          local realname=resolvers.resolve(filename) 
          local blob=loadfile(realname)
          if blob then
            local parentdata=blob()
            if parentdata then
              report_resolving("loading configuration file %a",filename)
              data=table.merged(parentdata,data)
            end
          end
        end
        data=data and data.content
        if data then
          if trace_locating then
            report_resolving("loading configuration file %a",filename)
            report_resolving()
          end
          local variables=data.variables or {}
          local warning=false
          for k,v in next,data do
            local variant=type(v)
            if variant=="table" then
              initializesetter(filename,k,v)
            elseif variables[k]==nil then
              if trace_locating and not warning then
                report_resolving("variables like %a in configuration file %a should move to the 'variables' subtable",
                  k,resolvers.resolve(filename))
                warning=true
              end
              variables[k]=v
            end
          end
          setups[pathname]=variables
          if resolvers.luacnfstate=="default" then
            local cnfspec=variables["TEXMFCNF"]
            if cnfspec then
              if trace_locating then
                report_resolving("reloading configuration due to TEXMF redefinition")
              end
              resolvers.setenv("TEXMFCNF",cnfspec)
              instance.specification={}
              identify_configuration_files()
              load_configuration_files()
              resolvers.luacnfstate="configuration"
              break
            end
          end
        else
          if trace_locating then
            report_resolving("skipping configuration file %a (no content)",filename)
          end
          setups[pathname]={}
          instance.loaderror=true
        end
      elseif trace_locating then
        report_resolving("skipping configuration file %a (no valid format)",filename)
      end
      instance.order[#instance.order+1]=instance.setups[pathname]
      if instance.loaderror then
        break
      end
    end
  elseif trace_locating then
    report_resolving("warning: no lua configuration files found")
  end
end
local function load_file_databases()
  instance.loaderror,instance.files=false,allocate()
  if not instance.renewcache then
    local hashes=instance.hashes
    for k=1,#hashes do
      local hash=hashes[k]
      resolvers.hashers.byscheme(hash.type,hash.name)
      if instance.loaderror then break end
    end
  end
end
local function locate_file_databases()
  local texmfpaths=resolvers.expandedpathlist("TEXMF")
  if #texmfpaths>0 then
    for i=1,#texmfpaths do
      local path=collapsepath(texmfpaths[i])
      path=gsub(path,"/+$","") 
      local stripped=lpegmatch(inhibitstripper,path) 
      if stripped~="" then
        local runtime=stripped==path
        path=resolvers.cleanpath(path)
        local spec=resolvers.splitmethod(stripped)
        if runtime and (spec.noscheme or spec.scheme=="file") then
          stripped="tree:///"..stripped
        elseif spec.scheme=="cache" or spec.scheme=="file" then
          stripped=spec.path
        end
        if trace_locating then
          if runtime then
            report_resolving("locating list of %a (runtime) (%s)",path,stripped)
          else
            report_resolving("locating list of %a (cached)",path)
          end
        end
        methodhandler('locators',stripped)
      end
    end
    if trace_locating then
      report_resolving()
    end
  elseif trace_locating then
    report_resolving("no texmf paths are defined (using TEXMF)")
  end
end
local function generate_file_databases()
  local hashes=instance.hashes
  for k=1,#hashes do
    local hash=hashes[k]
    methodhandler('generators',hash.name)
  end
  if trace_locating then
    report_resolving()
  end
end
local function save_file_databases() 
  for i=1,#instance.hashes do
    local hash=instance.hashes[i]
    local cachename=hash.name
    if hash.cache then
      local content=instance.files[cachename]
      caches.collapsecontent(content)
      if trace_locating then
        report_resolving("saving tree %a",cachename)
      end
      caches.savecontent(cachename,"files",content)
    elseif trace_locating then
      report_resolving("not saving runtime tree %a",cachename)
    end
  end
end
function resolvers.renew(hashname)
  if hashname and hashname~="" then
    local expanded=resolvers.expansion(hashname) or ""
    if expanded~="" then
      if trace_locating then
        report_resolving("identifying tree %a from %a",expanded,hashname)
      end
      hashname=expanded
    else
      if trace_locating then
        report_resolving("identifying tree %a",hashname)
      end
    end
    local realpath=resolvers.resolve(hashname)
    if lfs.isdir(realpath) then
      if trace_locating then
        report_resolving("using path %a",realpath)
      end
      methodhandler('generators',hashname)
      local content=instance.files[hashname]
      caches.collapsecontent(content)
      if trace_locating then
        report_resolving("saving tree %a",hashname)
      end
      caches.savecontent(hashname,"files",content)
    else
      report_resolving("invalid path %a",realpath)
    end
  end
end
local function load_databases()
  locate_file_databases()
  if instance.diskcache and not instance.renewcache then
    load_file_databases()
    if instance.loaderror then
      generate_file_databases()
      save_file_databases()
    end
  else
    generate_file_databases()
    if instance.renewcache then
      save_file_databases()
    end
  end
end
function resolvers.appendhash(type,name,cache)
  if not instance.hashed[name] then
    if trace_locating then
      report_resolving("hash %a appended",name)
    end
    insert(instance.hashes,{ type=type,name=name,cache=cache } )
    instance.hashed[name]=cache
  end
end
function resolvers.prependhash(type,name,cache)
  if not instance.hashed[name] then
    if trace_locating then
      report_resolving("hash %a prepended",name)
    end
    insert(instance.hashes,1,{ type=type,name=name,cache=cache } )
    instance.hashed[name]=cache
  end
end
function resolvers.extendtexmfvariable(specification) 
  local t=resolvers.splitpath(getenv("TEXMF")) 
  insert(t,1,specification)
  local newspec=concat(t,",") 
  if instance.environment["TEXMF"] then
    instance.environment["TEXMF"]=newspec
  elseif instance.variables["TEXMF"] then
    instance.variables["TEXMF"]=newspec
  else
  end
  reset_hashes()
end
function resolvers.splitexpansions()
  local ie=instance.expansions
  for k,v in next,ie do
    local t,tn,h,p={},0,{},splitconfigurationpath(v)
    for kk=1,#p do
      local vv=p[kk]
      if vv~="" and not h[vv] then
        tn=tn+1
        t[tn]=vv
        h[vv]=true
      end
    end
    if #t>1 then
      ie[k]=t
    else
      ie[k]=t[1]
    end
  end
end
function resolvers.datastate()
  return caches.contentstate()
end
function resolvers.variable(name)
  local name=name and lpegmatch(dollarstripper,name)
  local result=name and instance.variables[name]
  return result~=nil and result or ""
end
function resolvers.expansion(name)
  local name=name and lpegmatch(dollarstripper,name)
  local result=name and instance.expansions[name]
  return result~=nil and result or ""
end
function resolvers.unexpandedpathlist(str)
  local pth=resolvers.variable(str)
  local lst=resolvers.splitpath(pth)
  return expandedpathfromlist(lst)
end
function resolvers.unexpandedpath(str)
  return joinpath(resolvers.unexpandedpathlist(str))
end
local done={}
function resolvers.resetextrapath()
  local ep=instance.extra_paths
  if not ep then
    ep,done={},{}
    instance.extra_paths=ep
  elseif #ep>0 then
    instance.lists,done={},{}
  end
end
function resolvers.registerextrapath(paths,subpaths)
  paths=settings_to_array(paths)
  subpaths=settings_to_array(subpaths)
  local ep=instance.extra_paths or {}
  local oldn=#ep
  local newn=oldn
  local nofpaths=#paths
  local nofsubpaths=#subpaths
  if nofpaths>0 then
    if nofsubpaths>0 then
      for i=1,nofpaths do
        local p=paths[i]
        for j=1,nofsubpaths do
          local s=subpaths[j]
          local ps=p.."/"..s
          if not done[ps] then
            newn=newn+1
            ep[newn]=resolvers.cleanpath(ps)
            done[ps]=true
          end
        end
      end
    else
      for i=1,nofpaths do
        local p=paths[i]
        if not done[p] then
          newn=newn+1
          ep[newn]=resolvers.cleanpath(p)
          done[p]=true
        end
      end
    end
  elseif nofsubpaths>0 then
    for i=1,oldn do
      for j=1,nofsubpaths do
        local s=subpaths[j]
        local ps=ep[i].."/"..s
        if not done[ps] then
          newn=newn+1
          ep[newn]=resolvers.cleanpath(ps)
          done[ps]=true
        end
      end
    end
  end
  if newn>0 then
    instance.extra_paths=ep 
  end
  if newn>oldn then
    instance.lists={} 
  end
end
local function made_list(instance,list)
  local ep=instance.extra_paths
  if not ep or #ep==0 then
    return list
  else
    local done,new,newn={},{},0
    for k=1,#list do
      local v=list[k]
      if not done[v] then
        if find(v,"^[%.%/]$") then
          done[v]=true
          newn=newn+1
          new[newn]=v
        else
          break
        end
      end
    end
    for k=1,#ep do
      local v=ep[k]
      if not done[v] then
        done[v]=true
        newn=newn+1
        new[newn]=v
      end
    end
    for k=1,#list do
      local v=list[k]
      if not done[v] then
        done[v]=true
        newn=newn+1
        new[newn]=v
      end
    end
    return new
  end
end
function resolvers.cleanpathlist(str)
  local t=resolvers.expandedpathlist(str)
  if t then
    for i=1,#t do
      t[i]=collapsepath(resolvers.cleanpath(t[i]))
    end
  end
  return t
end
function resolvers.expandpath(str)
  return joinpath(resolvers.expandedpathlist(str))
end
function resolvers.expandedpathlist(str)
  if not str then
    return {}
  elseif instance.savelists then
    str=lpegmatch(dollarstripper,str)
    local lists=instance.lists
    local lst=lists[str]
    if not lst then
      local l=made_list(instance,resolvers.splitpath(resolvers.expansion(str)))
      lst=expandedpathfromlist(l)
      lists[str]=lst
    end
    return lst
  else
    local lst=resolvers.splitpath(resolvers.expansion(str))
    return made_list(instance,expandedpathfromlist(lst))
  end
end
function resolvers.expandedpathlistfromvariable(str) 
  str=lpegmatch(dollarstripper,str)
  local tmp=resolvers.variableofformatorsuffix(str)
  return resolvers.expandedpathlist(tmp~="" and tmp or str)
end
function resolvers.expandpathfromvariable(str)
  return joinpath(resolvers.expandedpathlistfromvariable(str))
end
function resolvers.expandbraces(str)
    local ori=str
  local pth=expandedpathfromlist(resolvers.splitpath(ori))
  return joinpath(pth)
end
function resolvers.registerfilehash(name,content,someerror)
  if content then
    instance.files[name]=content
  else
    instance.files[name]={}
    if somerror==true then 
      instance.loaderror=someerror
    end
  end
end
local function isreadable(name)
  local readable=lfs.isfile(name) 
  if trace_detail then
    if readable then
      report_resolving("file %a is readable",name)
    else
      report_resolving("file %a is not readable",name)
    end
  end
  return readable
end
local function collect_files(names)
  local filelist,noffiles={},0
  for k=1,#names do
    local fname=names[k]
    if trace_detail then
      report_resolving("checking name %a",fname)
    end
    local bname=filebasename(fname)
    local dname=filedirname(fname)
    if dname=="" or find(dname,"^%.") then
      dname=false
    else
      dname=gsub(dname,"%*",".*")
      dname="/"..dname.."$"
    end
    local hashes=instance.hashes
    for h=1,#hashes do
      local hash=hashes[h]
      local blobpath=hash.name
      local files=blobpath and instance.files[blobpath]
      if files then
        if trace_detail then
          report_resolving("deep checking %a, base %a, pattern %a",blobpath,bname,dname)
        end
        local blobfile=files[bname]
        if not blobfile then
          local rname="remap:"..bname
          blobfile=files[rname]
          if blobfile then
            bname=files[rname]
            blobfile=files[bname]
          end
        end
        if blobfile then
          local blobroot=files.__path__ or blobpath
          if type(blobfile)=='string' then
            if not dname or find(blobfile,dname) then
              local variant=hash.type
              local search=filejoin(blobroot,blobfile,bname)
              local result=methodhandler('concatinators',hash.type,blobroot,blobfile,bname)
              if trace_detail then
                report_resolving("match: variant %a, search %a, result %a",variant,search,result)
              end
              noffiles=noffiles+1
              filelist[noffiles]={ variant,search,result }
            end
          else
            for kk=1,#blobfile do
              local vv=blobfile[kk]
              if not dname or find(vv,dname) then
                local variant=hash.type
                local search=filejoin(blobroot,vv,bname)
                local result=methodhandler('concatinators',hash.type,blobroot,vv,bname)
                if trace_detail then
                  report_resolving("match: variant %a, search %a, result %a",variant,search,result)
                end
                noffiles=noffiles+1
                filelist[noffiles]={ variant,search,result }
              end
            end
          end
        end
      elseif trace_locating then
        report_resolving("no match in %a (%s)",blobpath,bname)
      end
    end
  end
  return noffiles>0 and filelist or nil
end
local fit={}
function resolvers.registerintrees(filename,format,filetype,usedmethod,foundname)
  local foundintrees=instance.foundintrees
  if usedmethod=="direct" and filename==foundname and fit[foundname] then
  else
    local t={
      filename=filename,
      format=format~="" and format or nil,
      filetype=filetype~="" and filetype or nil,
      usedmethod=usedmethod,
      foundname=foundname,
    }
    fit[foundname]=t
    foundintrees[#foundintrees+1]=t
  end
end
local function can_be_dir(name) 
  local fakepaths=instance.fakepaths
  if not fakepaths[name] then
    if lfs.isdir(name) then
      fakepaths[name]=1 
    else
      fakepaths[name]=2 
    end
  end
  return fakepaths[name]==1
end
local preparetreepattern=Cs((P(".")/"%%."+P("-")/"%%-"+P(1))^0*Cc("$"))
local collect_instance_files
local function find_analyze(filename,askedformat,allresults)
  local filetype,wantedfiles,ext='',{},suffixonly(filename)
  wantedfiles[#wantedfiles+1]=filename
  if askedformat=="" then
    if ext=="" or not suffixmap[ext] then
      local defaultsuffixes=resolvers.defaultsuffixes
      for i=1,#defaultsuffixes do
        local forcedname=filename..'.'..defaultsuffixes[i]
        wantedfiles[#wantedfiles+1]=forcedname
        filetype=resolvers.formatofsuffix(forcedname)
        if trace_locating then
          report_resolving("forcing filetype %a",filetype)
        end
      end
    else
      filetype=resolvers.formatofsuffix(filename)
      if trace_locating then
        report_resolving("using suffix based filetype %a",filetype)
      end
    end
  else
    if ext=="" or not suffixmap[ext] then
      local format_suffixes=suffixes[askedformat]
      if format_suffixes then
        for i=1,#format_suffixes do
          wantedfiles[#wantedfiles+1]=filename.."."..format_suffixes[i]
        end
      end
    end
    filetype=askedformat
    if trace_locating then
      report_resolving("using given filetype %a",filetype)
    end
  end
  return filetype,wantedfiles
end
local function find_direct(filename,allresults)
  if not dangerous[askedformat] and isreadable(filename) then
    if trace_detail then
      report_resolving("file %a found directly",filename)
    end
    return "direct",{ filename }
  end
end
local function find_wildcard(filename,allresults)
  if find(filename,'%*') then
    if trace_locating then
      report_resolving("checking wildcard %a",filename)
    end
    local method,result=resolvers.findwildcardfiles(filename)
    if result then
      return "wildcard",result
    end
  end
end
local function find_qualified(filename,allresults,askedformat,alsostripped) 
  if not file.is_qualified_path(filename) then
    return
  end
  if trace_locating then
    report_resolving("checking qualified name %a",filename)
  end
  if isreadable(filename) then
    if trace_detail then
      report_resolving("qualified file %a found",filename)
    end
    return "qualified",{ filename }
  end
  if trace_detail then
    report_resolving("locating qualified file %a",filename)
  end
  local forcedname,suffix="",suffixonly(filename)
  if suffix=="" then 
    local format_suffixes=askedformat=="" and resolvers.defaultsuffixes or suffixes[askedformat]
    if format_suffixes then
      for i=1,#format_suffixes do
        local s=format_suffixes[i]
        forcedname=filename.."."..s
        if isreadable(forcedname) then
          if trace_locating then
            report_resolving("no suffix, forcing format filetype %a",s)
          end
          return "qualified",{ forcedname }
        end
      end
    end
  end
  if alsostripped and suffix and suffix~="" then
    local basename=filebasename(filename)
    local pattern=lpegmatch(preparetreepattern,filename)
    local savedformat=askedformat
    local format=savedformat or ""
    if format=="" then
      askedformat=resolvers.formatofsuffix(suffix)
    end
    if not format then
      askedformat="othertextfiles" 
    end
    if basename~=filename then
      local resolved=collect_instance_files(basename,askedformat,allresults)
      if #resolved==0 then
        local lowered=lower(basename)
        if filename~=lowered then
          resolved=collect_instance_files(lowered,askedformat,allresults)
        end
      end
      resolvers.format=savedformat
      if #resolved>0 then
        local result={}
        for r=1,#resolved do
          local rr=resolved[r]
          if find(rr,pattern) then
            result[#result+1]=rr
          end
        end
        if #result>0 then
          return "qualified",result
        end
      end
    end
  end
end
local function check_subpath(fname)
  if isreadable(fname) then
    if trace_detail then
      report_resolving("found %a by deep scanning",fname)
    end
    return fname
  end
end
local function find_intree(filename,filetype,wantedfiles,allresults)
  local typespec=resolvers.variableofformat(filetype)
  local pathlist=resolvers.expandedpathlist(typespec)
  local method="intree"
  if pathlist and #pathlist>0 then
    local filelist=collect_files(wantedfiles)
    local dirlist={}
    if filelist then
      for i=1,#filelist do
        dirlist[i]=filedirname(filelist[i][3]).."/" 
      end
    end
    if trace_detail then
      report_resolving("checking filename %a",filename)
    end
    local resolve=resolvers.resolve
    local result={}
    for k=1,#pathlist do
      local path=pathlist[k]
      local pathname=lpegmatch(inhibitstripper,path)
      local doscan=path==pathname 
      if not find (pathname,'//$') then
        doscan=false 
      end
      local done=false
      if filelist then
        local expression=makepathexpression(pathname)
        if trace_detail then
          report_resolving("using pattern %a for path %a",expression,pathname)
        end
        for k=1,#filelist do
          local fl=filelist[k]
          local f=fl[2]
          local d=dirlist[k]
          if find(d,expression) or find(resolve(d),expression) then
            result[#result+1]=resolve(fl[3]) 
            done=true
            if allresults then
              if trace_detail then
                report_resolving("match to %a in hash for file %a and path %a, continue scanning",expression,f,d)
              end
            else
              if trace_detail then
                report_resolving("match to %a in hash for file %a and path %a, quit scanning",expression,f,d)
              end
              break
            end
          elseif trace_detail then
            report_resolving("no match to %a in hash for file %a and path %a",expression,f,d)
          end
        end
      end
      if done then
        method="database"
      else
        method="filesystem" 
        pathname=gsub(pathname,"/+$","")
        pathname=resolve(pathname)
        local scheme=url.hasscheme(pathname)
        if not scheme or scheme=="file" then
          local pname=gsub(pathname,"%.%*$",'')
          if not find(pname,"%*") then
            if can_be_dir(pname) then
              for k=1,#wantedfiles do
                local w=wantedfiles[k]
                local fname=check_subpath(filejoin(pname,w))
                if fname then
                  result[#result+1]=fname
                  done=true
                  if not allresults then
                    break
                  end
                end
              end
              if not done and doscan then
                local files=resolvers.simplescanfiles(pname,false,true)
                for k=1,#wantedfiles do
                  local w=wantedfiles[k]
                  local subpath=files[w]
                  if not subpath or subpath=="" then
                  elseif type(subpath)=="string" then
                    local fname=check_subpath(filejoin(pname,subpath,w))
                    if fname then
                      result[#result+1]=fname
                      done=true
                      if not allresults then
                        break
                      end
                    end
                  else
                    for i=1,#subpath do
                      local sp=subpath[i]
                      if sp=="" then
                      else
                        local fname=check_subpath(filejoin(pname,sp,w))
                        if fname then
                          result[#result+1]=fname
                          done=true
                          if not allresults then
                            break
                          end
                        end
                      end
                    end
                    if done and not allresults then
                      break
                    end
                  end
                end
              end
            end
          else
          end
        end
      end
      if done and not allresults then
        break
      end
    end
    if #result>0 then
      return method,result
    end
  end
end
local function find_onpath(filename,filetype,wantedfiles,allresults)
  if trace_detail then
    report_resolving("checking filename %a, filetype %a, wanted files %a",filename,filetype,concat(wantedfiles," | "))
  end
  local result={}
  for k=1,#wantedfiles do
    local fname=wantedfiles[k]
    if fname and isreadable(fname) then
      filename=fname
      result[#result+1]=filejoin('.',fname)
      if not allresults then
        break
      end
    end
  end
  if #result>0 then
    return "onpath",result
  end
end
local function find_otherwise(filename,filetype,wantedfiles,allresults) 
  local filelist=collect_files(wantedfiles)
  local fl=filelist and filelist[1]
  if fl then
    return "otherwise",{ resolvers.resolve(fl[3]) } 
  end
end
collect_instance_files=function(filename,askedformat,allresults) 
  askedformat=askedformat or ""
  filename=collapsepath(filename)
  if allresults then
    local filetype,wantedfiles=find_analyze(filename,askedformat)
    local results={
      { find_direct  (filename,true) },
      { find_wildcard (filename,true) },
      { find_qualified(filename,true,askedformat) },
      { find_intree  (filename,filetype,wantedfiles,true) },
      { find_onpath  (filename,filetype,wantedfiles,true) },
      { find_otherwise(filename,filetype,wantedfiles,true) },
    }
    local result,status,done={},{},{}
    for k,r in next,results do
      local method,list=r[1],r[2]
      if method and list then
        for i=1,#list do
          local c=collapsepath(list[i])
          if not done[c] then
            result[#result+1]=c
            done[c]=true
          end
          status[#status+1]=formatters["%-10s: %s"](method,c)
        end
      end
    end
    if trace_detail then
      report_resolving("lookup status: %s",table.serialize(status,filename))
    end
    return result,status
  else
    local method,result,stamp,filetype,wantedfiles
    if instance.remember then
      stamp=formatters["%s--%s"](filename,askedformat)
      result=stamp and instance.found[stamp]
      if result then
        if trace_locating then
          report_resolving("remembered file %a",filename)
        end
        return result
      end
    end
    method,result=find_direct(filename)
    if not result then
      method,result=find_wildcard(filename)
      if not result then
        method,result=find_qualified(filename,false,askedformat)
        if not result then
          filetype,wantedfiles=find_analyze(filename,askedformat)
          method,result=find_intree(filename,filetype,wantedfiles)
          if not result then
            method,result=find_onpath(filename,filetype,wantedfiles)
            if not result then
              method,result=find_otherwise(filename,filetype,wantedfiles)
            end
          end
        end
      end
    end
    if result and #result>0 then
      local foundname=collapsepath(result[1])
      resolvers.registerintrees(filename,askedformat,filetype,method,foundname)
      result={ foundname }
    else
      result={} 
    end
    if stamp then
      if trace_locating then
        report_resolving("remembering file %a",filename)
      end
      instance.found[stamp]=result
    end
    return result
  end
end
local function findfiles(filename,filetype,allresults)
  local result,status=collect_instance_files(filename,filetype or "",allresults)
  if not result or #result==0 then
    local lowered=lower(filename)
    if filename~=lowered then
      result,status=collect_instance_files(lowered,filetype or "",allresults)
    end
  end
  return result or {},status
end
function resolvers.findfiles(filename,filetype)
  return findfiles(filename,filetype,true)
end
function resolvers.findfile(filename,filetype)
  return findfiles(filename,filetype,false)[1] or ""
end
function resolvers.findpath(filename,filetype)
  return filedirname(findfiles(filename,filetype,false)[1] or "")
end
local function findgivenfiles(filename,allresults)
  local bname,result=filebasename(filename),{}
  local hashes=instance.hashes
  local noffound=0
  for k=1,#hashes do
    local hash=hashes[k]
    local files=instance.files[hash.name] or {}
    local blist=files[bname]
    if not blist then
      local rname="remap:"..bname
      blist=files[rname]
      if blist then
        bname=files[rname]
        blist=files[bname]
      end
    end
    if blist then
      if type(blist)=='string' then
        local found=methodhandler('concatinators',hash.type,hash.name,blist,bname) or ""
        if found~="" then
          noffound=noffound+1
          result[noffound]=resolvers.resolve(found)
          if not allresults then
            break
          end
        end
      else
        for kk=1,#blist do
          local vv=blist[kk]
          local found=methodhandler('concatinators',hash.type,hash.name,vv,bname) or ""
          if found~="" then
            noffound=noffound+1
            result[noffound]=resolvers.resolve(found)
            if not allresults then break end
          end
        end
      end
    end
  end
  return result
end
function resolvers.findgivenfiles(filename)
  return findgivenfiles(filename,true)
end
function resolvers.findgivenfile(filename)
  return findgivenfiles(filename,false)[1] or ""
end
local function doit(path,blist,bname,tag,variant,result,allresults)
  local done=false
  if blist and variant then
    local resolve=resolvers.resolve 
    if type(blist)=='string' then
      if find(lower(blist),path) then
        local full=methodhandler('concatinators',variant,tag,blist,bname) or ""
        result[#result+1]=resolve(full)
        done=true
      end
    else
      for kk=1,#blist do
        local vv=blist[kk]
        if find(lower(vv),path) then
          local full=methodhandler('concatinators',variant,tag,vv,bname) or ""
          result[#result+1]=resolve(full)
          done=true
          if not allresults then break end
        end
      end
    end
  end
  return done
end
local makewildcard=Cs(
  (P("^")^0*P("/")*P(-1)+P(-1))/".*"+(P("^")^0*P("/")/"")^0*(P("*")/".*"+P("-")/"%%-"+P(".")/"%%."+P("?")/"."+P("\\")/"/"+P(1))^0
)
function resolvers.wildcardpattern(pattern)
  return lpegmatch(makewildcard,pattern) or pattern
end
local function findwildcardfiles(filename,allresults,result) 
  result=result or {}
  local base=filebasename(filename)
  local dirn=filedirname(filename)
  local path=lower(lpegmatch(makewildcard,dirn) or dirn)
  local name=lower(lpegmatch(makewildcard,base) or base)
  local files,done=instance.files,false
  if find(name,"%*") then
    local hashes=instance.hashes
    for k=1,#hashes do
      local hash=hashes[k]
      local hashname,hashtype=hash.name,hash.type
      for kk,hh in next,files[hashname] do
        if not find(kk,"^remap:") then
          if find(lower(kk),name) then
            if doit(path,hh,kk,hashname,hashtype,result,allresults) then done=true end
            if done and not allresults then break end
          end
        end
      end
    end
  else
    local hashes=instance.hashes
    for k=1,#hashes do
      local hash=hashes[k]
      local hashname,hashtype=hash.name,hash.type
      if doit(path,files[hashname][bname],bname,hashname,hashtype,result,allresults) then done=true end
      if done and not allresults then break end
    end
  end
  return result
end
function resolvers.findwildcardfiles(filename,result)
  return findwildcardfiles(filename,true,result)
end
function resolvers.findwildcardfile(filename)
  return findwildcardfiles(filename,false)[1] or ""
end
function resolvers.automount()
end
function resolvers.load(option)
  statistics.starttiming(instance)
  identify_configuration_files()
  load_configuration_files()
  if option~="nofiles" then
    load_databases()
    resolvers.automount()
  end
  statistics.stoptiming(instance)
  local files=instance.files
  return files and next(files) and true
end
function resolvers.loadtime()
  return statistics.elapsedtime(instance)
end
local function report(str)
  if trace_locating then
    report_resolving(str) 
  else
    print(str)
  end
end
function resolvers.dowithfilesandreport(command,files,...) 
  if files and #files>0 then
    if trace_locating then
      report('') 
    end
    if type(files)=="string" then
      files={ files }
    end
    for f=1,#files do
      local file=files[f]
      local result=command(file,...)
      if type(result)=='string' then
        report(result)
      else
        for i=1,#result do
          report(result[i]) 
        end
      end
    end
  end
end
function resolvers.showpath(str)   
  return joinpath(resolvers.expandedpathlist(resolvers.formatofvariable(str)))
end
function resolvers.registerfile(files,name,path)
  if files[name] then
    if type(files[name])=='string' then
      files[name]={ files[name],path }
    else
      files[name]=path
    end
  else
    files[name]=path
  end
end
function resolvers.dowithpath(name,func)
  local pathlist=resolvers.expandedpathlist(name)
  for i=1,#pathlist do
    func("^"..resolvers.cleanpath(pathlist[i]))
  end
end
function resolvers.dowithvariable(name,func)
  func(expandedvariable(name))
end
function resolvers.locateformat(name)
  local engine=environment.ownmain or "luatex"
  local barename=file.removesuffix(name)
  local fullname=file.addsuffix(barename,"fmt")
  local fmtname=caches.getfirstreadablefile(fullname,"formats",engine) or ""
  if fmtname=="" then
    fmtname=resolvers.findfile(fullname)
    fmtname=resolvers.cleanpath(fmtname)
  end
  if fmtname~="" then
    local barename=file.removesuffix(fmtname)
    local luaname=file.addsuffix(barename,luasuffixes.lua)
    local lucname=file.addsuffix(barename,luasuffixes.luc)
    local luiname=file.addsuffix(barename,luasuffixes.lui)
    if lfs.isfile(luiname) then
      return barename,luiname
    elseif lfs.isfile(lucname) then
      return barename,lucname
    elseif lfs.isfile(luaname) then
      return barename,luaname
    end
  end
  return nil,nil
end
function resolvers.booleanvariable(str,default)
  local b=resolvers.expansion(str)
  if b=="" then
    return default
  else
    b=toboolean(b)
    return (b==nil and default) or b
  end
end
function resolvers.dowithfilesintree(pattern,handle,before,after) 
  local instance=resolvers.instance
  local hashes=instance.hashes
  for i=1,#hashes do
    local hash=hashes[i]
    local blobtype=hash.type
    local blobpath=hash.name
    if blobpath then
      if before then
        before(blobtype,blobpath,pattern)
      end
      local files=instance.files[blobpath]
      local total,checked,done=0,0,0
      if files then
        for k,v in table.sortedhash(files) do 
          total=total+1
          if find(k,"^remap:") then
          elseif find(k,pattern) then
            if type(v)=="string" then
              checked=checked+1
              if handle(blobtype,blobpath,v,k) then
                done=done+1
              end
            else
              checked=checked+#v
              for i=1,#v do
                if handle(blobtype,blobpath,v[i],k) then
                  done=done+1
                end
              end
            end
          end
        end
      end
      if after then
        after(blobtype,blobpath,pattern,total,checked,done)
      end
    end
  end
end
resolvers.obsolete=resolvers.obsolete or {}
local obsolete=resolvers.obsolete
resolvers.find_file=resolvers.findfile  obsolete.find_file=resolvers.findfile
resolvers.find_files=resolvers.findfiles  obsolete.find_files=resolvers.findfiles


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-pre"] = package.loaded["data-pre"] or true

-- original size: 6430, stripped down to: 4219

if not modules then modules={} end modules ['data-pre']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local resolvers=resolvers
local prefixes=utilities.storage.allocate()
resolvers.prefixes=prefixes
local cleanpath,findgivenfile,expansion=resolvers.cleanpath,resolvers.findgivenfile,resolvers.expansion
local getenv=resolvers.getenv 
local P,S,R,C,Cs,lpegmatch=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.Cs,lpeg.match
local joinpath,basename,dirname=file.join,file.basename,file.dirname
local getmetatable,rawset,type=getmetatable,rawset,type
prefixes.environment=function(str)
  return cleanpath(expansion(str))
end
prefixes.relative=function(str,n) 
  if io.exists(str) then
  elseif io.exists("./"..str) then
    str="./"..str
  else
    local p="../"
    for i=1,n or 2 do
      if io.exists(p..str) then
        str=p..str
        break
      else
        p=p.."../"
      end
    end
  end
  return cleanpath(str)
end
prefixes.auto=function(str)
  local fullname=prefixes.relative(str)
  if not lfs.isfile(fullname) then
    fullname=prefixes.locate(str)
  end
  return fullname
end
prefixes.locate=function(str)
  local fullname=findgivenfile(str) or ""
  return cleanpath((fullname~="" and fullname) or str)
end
prefixes.filename=function(str)
  local fullname=findgivenfile(str) or ""
  return cleanpath(basename((fullname~="" and fullname) or str)) 
end
prefixes.pathname=function(str)
  local fullname=findgivenfile(str) or ""
  return cleanpath(dirname((fullname~="" and fullname) or str))
end
prefixes.selfautoloc=function(str)
  return cleanpath(joinpath(getenv('SELFAUTOLOC'),str))
end
prefixes.selfautoparent=function(str)
  return cleanpath(joinpath(getenv('SELFAUTOPARENT'),str))
end
prefixes.selfautodir=function(str)
  return cleanpath(joinpath(getenv('SELFAUTODIR'),str))
end
prefixes.home=function(str)
  return cleanpath(joinpath(getenv('HOME'),str))
end
local function toppath()
  local inputstack=resolvers.inputstack 
  if not inputstack then         
    return "."
  end
  local pathname=dirname(inputstack[#inputstack] or "")
  if pathname=="" then
    return "."
  else
    return pathname
  end
end
resolvers.toppath=toppath
prefixes.toppath=function(str)
  return cleanpath(joinpath(toppath(),str))
end
prefixes.env=prefixes.environment
prefixes.rel=prefixes.relative
prefixes.loc=prefixes.locate
prefixes.kpse=prefixes.locate
prefixes.full=prefixes.locate
prefixes.file=prefixes.filename
prefixes.path=prefixes.pathname
function resolvers.allprefixes(separator)
  local all=table.sortedkeys(prefixes)
  if separator then
    for i=1,#all do
      all[i]=all[i]..":"
    end
  end
  return all
end
local function _resolve_(method,target)
  local action=prefixes[method]
  if action then
    return action(target)
  else
    return method..":"..target
  end
end
local resolved,abstract={},{}
function resolvers.resetresolve(str)
  resolved,abstract={},{}
end
local pattern=Cs((C(R("az")^2)*P(":")*C((1-S(" \"\';,"))^1)/_resolve_+P(1))^0)
local function resolve(str) 
  if type(str)=="table" then
    local t={}
    for i=1,#str do
      t[i]=resolve(str[i])
    end
    return t
  else
    local res=resolved[str]
    if not res then
      res=lpegmatch(pattern,str)
      resolved[str]=res
      abstract[res]=str
    end
    return res
  end
end
local function unresolve(str)
  return abstract[str] or str
end
resolvers.resolve=resolve
resolvers.unresolve=unresolve
if type(os.uname)=="function" then
  for k,v in next,os.uname() do
    if not prefixes[k] then
      prefixes[k]=function() return v end
    end
  end
end
if os.type=="unix" then
  local pattern
  local function makepattern(t,k,v)
    if t then
      rawset(t,k,v)
    end
    local colon=P(":")
    for k,v in table.sortedpairs(prefixes) do
      if p then
        p=P(k)+p
      else
        p=P(k)
      end
    end
    pattern=Cs((p*colon+colon/";"+P(1))^0)
  end
  makepattern()
  getmetatable(prefixes).__newindex=makepattern
  function resolvers.repath(str)
    return lpegmatch(pattern,str)
  end
else 
  function resolvers.repath(str)
    return str
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-inp"] = package.loaded["data-inp"] or true

-- original size: 910, stripped down to: 823

if not modules then modules={} end modules ['data-inp']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local allocate=utilities.storage.allocate
local resolvers=resolvers
local methodhandler=resolvers.methodhandler
local registermethod=resolvers.registermethod
local finders=allocate { helpers={},notfound=function() end }
local openers=allocate { helpers={},notfound=function() end }
local loaders=allocate { helpers={},notfound=function() return false,nil,0 end }
registermethod("finders",finders,"uri")
registermethod("openers",openers,"uri")
registermethod("loaders",loaders,"uri")
resolvers.finders=finders
resolvers.openers=openers
resolvers.loaders=loaders


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-out"] = package.loaded["data-out"] or true

-- original size: 530, stripped down to: 475

if not modules then modules={} end modules ['data-out']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local allocate=utilities.storage.allocate
local resolvers=resolvers
local registermethod=resolvers.registermethod
local savers=allocate { helpers={} }
resolvers.savers=savers
registermethod("savers",savers,"uri")


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-fil"] = package.loaded["data-fil"] or true

-- original size: 3801, stripped down to: 3231

if not modules then modules={} end modules ['data-fil']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_files=logs.reporter("resolvers","files")
local resolvers=resolvers
local finders,openers,loaders,savers=resolvers.finders,resolvers.openers,resolvers.loaders,resolvers.savers
local locators,hashers,generators,concatinators=resolvers.locators,resolvers.hashers,resolvers.generators,resolvers.concatinators
local checkgarbage=utilities.garbagecollector and utilities.garbagecollector.check
function locators.file(specification)
  local name=specification.filename
  local realname=resolvers.resolve(name) 
  if realname and realname~='' and lfs.isdir(realname) then
    if trace_locating then
      report_files("file locator %a found as %a",name,realname)
    end
    resolvers.appendhash('file',name,true) 
  elseif trace_locating then
    report_files("file locator %a not found",name)
  end
end
function hashers.file(specification)
  local name=specification.filename
  local content=caches.loadcontent(name,'files')
  resolvers.registerfilehash(name,content,content==nil)
end
function generators.file(specification)
  local path=specification.filename
  local content=resolvers.scanfiles(path,false,true)
  resolvers.registerfilehash(path,content,true)
end
concatinators.file=file.join
function finders.file(specification,filetype)
  local filename=specification.filename
  local foundname=resolvers.findfile(filename,filetype)
  if foundname and foundname~="" then
    if trace_locating then
      report_files("file finder: %a found",filename)
    end
    return foundname
  else
    if trace_locating then
      report_files("file finder: %a not found",filename)
    end
    return finders.notfound()
  end
end
function openers.helpers.textopener(tag,filename,f)
  return {
    reader=function()              return f:read () end,
    close=function() logs.show_close(filename) return f:close() end,
  }
end
function openers.file(specification,filetype)
  local filename=specification.filename
  if filename and filename~="" then
    local f=io.open(filename,"r")
    if f then
      if trace_locating then
        report_files("file opener: %a opened",filename)
      end
      return openers.helpers.textopener("file",filename,f)
    end
  end
  if trace_locating then
    report_files("file opener: %a not found",filename)
  end
  return openers.notfound()
end
function loaders.file(specification,filetype)
  local filename=specification.filename
  if filename and filename~="" then
    local f=io.open(filename,"rb")
    if f then
      logs.show_load(filename)
      if trace_locating then
        report_files("file loader: %a loaded",filename)
      end
      local s=f:read("*a") 
      if checkgarbage then
        checkgarbage(#s)
      end
      f:close()
      if s then
        return true,s,#s
      end
    end
  end
  if trace_locating then
    report_files("file loader: %a not found",filename)
  end
  return loaders.notfound()
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-con"] = package.loaded["data-con"] or true

-- original size: 4940, stripped down to: 3580

if not modules then modules={} end modules ['data-con']={
  version=1.100,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local format,lower,gsub=string.format,string.lower,string.gsub
local trace_cache=false trackers.register("resolvers.cache",function(v) trace_cache=v end)
local trace_containers=false trackers.register("resolvers.containers",function(v) trace_containers=v end)
local trace_storage=false trackers.register("resolvers.storage",function(v) trace_storage=v end)
containers=containers or {}
local containers=containers
containers.usecache=true
local report_containers=logs.reporter("resolvers","containers")
local allocated={}
local mt={
  __index=function(t,k)
    if k=="writable" then
      local writable=caches.getwritablepath(t.category,t.subcategory) or { "." }
      t.writable=writable
      return writable
    elseif k=="readables" then
      local readables=caches.getreadablepaths(t.category,t.subcategory) or { "." }
      t.readables=readables
      return readables
    end
  end,
  __storage__=true
}
function containers.define(category,subcategory,version,enabled)
  if category and subcategory then
    local c=allocated[category]
    if not c then
      c={}
      allocated[category]=c
    end
    local s=c[subcategory]
    if not s then
      s={
        category=category,
        subcategory=subcategory,
        storage={},
        enabled=enabled,
        version=version or math.pi,
        trace=false,
      }
      setmetatable(s,mt)
      c[subcategory]=s
    end
    return s
  end
end
function containers.is_usable(container,name)
  return container.enabled and caches and caches.is_writable(container.writable,name)
end
function containers.is_valid(container,name)
  if name and name~="" then
    local storage=container.storage[name]
    return storage and storage.cache_version==container.version
  else
    return false
  end
end
function containers.read(container,name)
  local storage=container.storage
  local stored=storage[name]
  if not stored and container.enabled and caches and containers.usecache then
    stored=caches.loaddata(container.readables,name)
    if stored and stored.cache_version==container.version then
      if trace_cache or trace_containers then
        report_containers("action %a, category %a, name %a","load",container.subcategory,name)
      end
    else
      stored=nil
    end
    storage[name]=stored
  elseif stored then
    if trace_cache or trace_containers then
      report_containers("action %a, category %a, name %a","reuse",container.subcategory,name)
    end
  end
  return stored
end
function containers.write(container,name,data)
  if data then
    data.cache_version=container.version
    if container.enabled and caches then
      local unique,shared=data.unique,data.shared
      data.unique,data.shared=nil,nil
      caches.savedata(container.writable,name,data)
      if trace_cache or trace_containers then
        report_containers("action %a, category %a, name %a","save",container.subcategory,name)
      end
      data.unique,data.shared=unique,shared
    end
    if trace_cache or trace_containers then
      report_containers("action %a, category %a, name %a","store",container.subcategory,name)
    end
    container.storage[name]=data
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

package.loaded["data-use"] = package.loaded["data-use"] or true

-- original size: 3913, stripped down to: 2998

if not modules then modules={} end modules ['data-use']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local format,lower,gsub,find=string.format,string.lower,string.gsub,string.find
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_mounts=logs.reporter("resolvers","mounts")
local resolvers=resolvers
resolvers.automounted=resolvers.automounted or {}
function resolvers.automount(usecache)
  local mountpaths=resolvers.cleanpathlist(resolvers.expansion('TEXMFMOUNT'))
  if (not mountpaths or #mountpaths==0) and usecache then
    mountpaths=caches.getreadablepaths("mount")
  end
  if mountpaths and #mountpaths>0 then
    statistics.starttiming(resolvers.instance)
    for k=1,#mountpaths do
      local root=mountpaths[k]
      local f=io.open(root.."/url.tmi")
      if f then
        for line in f:lines() do
          if line then
            if find(line,"^[%%#%-]") then
            elseif find(line,"^zip://") then
              if trace_locating then
                report_mounts("mounting %a",line)
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
statistics.register("used config file",function() return caches.configfiles() end)
statistics.register("used cache path",function() return caches.usedpaths() end)
function statistics.savefmtstatus(texname,formatbanner,sourcefile) 
  local enginebanner=status.list().banner
  if formatbanner and enginebanner and sourcefile then
    local luvname=file.replacesuffix(texname,"luv") 
    local luvdata={
      enginebanner=enginebanner,
      formatbanner=formatbanner,
      sourcehash=md5.hex(io.loaddata(resolvers.findfile(sourcefile)) or "unknown"),
      sourcefile=sourcefile,
    }
    io.savedata(luvname,table.serialize(luvdata,true))
  end
end
function statistics.checkfmtstatus(texname)
  local enginebanner=status.list().banner
  if enginebanner and texname then
    local luvname=file.replacesuffix(texname,"luv") 
    if lfs.isfile(luvname) then
      local luv=dofile(luvname)
      if luv and luv.sourcefile then
        local sourcehash=md5.hex(io.loaddata(resolvers.findfile(luv.sourcefile)) or "unknown")
        local luvbanner=luv.enginebanner or "?"
        if luvbanner~=enginebanner then
          return format("engine mismatch (luv: %s <> bin: %s)",luvbanner,enginebanner)
        end
        local luvhash=luv.sourcehash or "?"
        if luvhash~=sourcehash then
          return format("source mismatch (luv: %s <> bin: %s)",luvhash,sourcehash)
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

package.loaded["data-zip"] = package.loaded["data-zip"] or true

-- original size: 8489, stripped down to: 6757

if not modules then modules={} end modules ['data-zip']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local format,find,match=string.format,string.find,string.match
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_zip=logs.reporter("resolvers","zip")
local resolvers=resolvers
zip=zip or {}
local zip=zip
zip.archives=zip.archives or {}
local archives=zip.archives
zip.registeredfiles=zip.registeredfiles or {}
local registeredfiles=zip.registeredfiles
local limited=false
directives.register("system.inputmode",function(v)
  if not limited then
    local i_limiter=io.i_limiter(v)
    if i_limiter then
      zip.open=i_limiter.protect(zip.open)
      limited=true
    end
  end
end)
local function validzip(str) 
  if not find(str,"^zip://") then
    return "zip:///"..str
  else
    return str
  end
end
function zip.openarchive(name)
  if not name or name=="" then
    return nil
  else
    local arch=archives[name]
    if not arch then
      local full=resolvers.findfile(name) or ""
      arch=(full~="" and zip.open(full)) or false
      archives[name]=arch
    end
    return arch
  end
end
function zip.closearchive(name)
  if not name or (name=="" and archives[name]) then
    zip.close(archives[name])
    archives[name]=nil
  end
end
function resolvers.locators.zip(specification)
  local archive=specification.filename
  local zipfile=archive and archive~="" and zip.openarchive(archive) 
  if trace_locating then
    if zipfile then
      report_zip("locator: archive %a found",archive)
    else
      report_zip("locator: archive %a not found",archive)
    end
  end
end
function resolvers.hashers.zip(specification)
  local archive=specification.filename
  if trace_locating then
    report_zip("loading file %a",archive)
  end
  resolvers.usezipfile(specification.original)
end
function resolvers.concatinators.zip(zipfile,path,name) 
  if not path or path=="" then
    return format('%s?name=%s',zipfile,name)
  else
    return format('%s?name=%s/%s',zipfile,path,name)
  end
end
function resolvers.finders.zip(specification)
  local original=specification.original
  local archive=specification.filename
  if archive then
    local query=url.query(specification.query)
    local queryname=query.name
    if queryname then
      local zfile=zip.openarchive(archive)
      if zfile then
        if trace_locating then
          report_zip("finder: archive %a found",archive)
        end
        local dfile=zfile:open(queryname)
        if dfile then
          dfile=zfile:close()
          if trace_locating then
            report_zip("finder: file %a found",queryname)
          end
          return specification.original
        elseif trace_locating then
          report_zip("finder: file %a not found",queryname)
        end
      elseif trace_locating then
        report_zip("finder: unknown archive %a",archive)
      end
    end
  end
  if trace_locating then
    report_zip("finder: %a not found",original)
  end
  return resolvers.finders.notfound()
end
function resolvers.openers.zip(specification)
  local original=specification.original
  local archive=specification.filename
  if archive then
    local query=url.query(specification.query)
    local queryname=query.name
    if queryname then
      local zfile=zip.openarchive(archive)
      if zfile then
        if trace_locating then
          report_zip("opener; archive %a opened",archive)
        end
        local dfile=zfile:open(queryname)
        if dfile then
          if trace_locating then
            report_zip("opener: file %a found",queryname)
          end
          return resolvers.openers.helpers.textopener('zip',original,dfile)
        elseif trace_locating then
          report_zip("opener: file %a not found",queryname)
        end
      elseif trace_locating then
        report_zip("opener: unknown archive %a",archive)
      end
    end
  end
  if trace_locating then
    report_zip("opener: %a not found",original)
  end
  return resolvers.openers.notfound()
end
function resolvers.loaders.zip(specification)
  local original=specification.original
  local archive=specification.filename
  if archive then
    local query=url.query(specification.query)
    local queryname=query.name
    if queryname then
      local zfile=zip.openarchive(archive)
      if zfile then
        if trace_locating then
          report_zip("loader: archive %a opened",archive)
        end
        local dfile=zfile:open(queryname)
        if dfile then
          logs.show_load(original)
          if trace_locating then
            report_zip("loader; file %a loaded",original)
          end
          local s=dfile:read("*all")
          dfile:close()
          return true,s,#s
        elseif trace_locating then
          report_zip("loader: file %a not found",queryname)
        end
      elseif trace_locating then
        report_zip("loader; unknown archive %a",archive)
      end
    end
  end
  if trace_locating then
    report_zip("loader: %a not found",original)
  end
  return resolvers.openers.notfound()
end
function resolvers.usezipfile(archive)
  local specification=resolvers.splitmethod(archive) 
  local archive=specification.filename
  if archive and not registeredfiles[archive] then
    local z=zip.openarchive(archive)
    if z then
      local instance=resolvers.instance
      local tree=url.query(specification.query).tree or ""
      if trace_locating then
        report_zip("registering: archive %a",archive)
      end
      statistics.starttiming(instance)
      resolvers.prependhash('zip',archive)
      resolvers.extendtexmfvariable(archive) 
      registeredfiles[archive]=z
      instance.files[archive]=resolvers.registerzipfile(z,tree)
      statistics.stoptiming(instance)
    elseif trace_locating then
      report_zip("registering: unknown archive %a",archive)
    end
  elseif trace_locating then
    report_zip("registering: archive %a not found",archive)
  end
end
function resolvers.registerzipfile(z,tree)
  local files,filter={},""
  if tree=="" then
    filter="^(.+)/(.-)$"
  else
    filter=format("^%s/(.+)/(.-)$",tree)
  end
  if trace_locating then
    report_zip("registering: using filter %a",filter)
  end
  local register,n=resolvers.registerfile,0
  for i in z:files() do
    local path,name=match(i.filename,filter)
    if path then
      if name and name~='' then
        register(files,name,path)
        n=n+1
      else
      end
    else
      register(files,i.filename,'')
      n=n+1
    end
  end
  report_zip("registering: %s files registered",n)
  return files
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-tre"] = package.loaded["data-tre"] or true

-- original size: 2508, stripped down to: 2074

if not modules then modules={} end modules ['data-tre']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local find,gsub,format=string.find,string.gsub,string.format
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_trees=logs.reporter("resolvers","trees")
local resolvers=resolvers
local done,found,notfound={},{},resolvers.finders.notfound
function resolvers.finders.tree(specification)
  local spec=specification.filename
  local fnd=found[spec]
  if fnd==nil then
    if spec~="" then
      local path,name=file.dirname(spec),file.basename(spec)
      if path=="" then path="." end
      local hash=done[path]
      if not hash then
        local pattern=path.."/*" 
        hash=dir.glob(pattern)
        done[path]=hash
      end
      local pattern="/"..gsub(name,"([%.%-%+])","%%%1").."$"
      for k=1,#hash do
        local v=hash[k]
        if find(v,pattern) then
          found[spec]=v
          return v
        end
      end
    end
    fnd=notfound() 
    found[spec]=fnd
  end
  return fnd
end
function resolvers.locators.tree(specification)
  local name=specification.filename
  local realname=resolvers.resolve(name) 
  if realname and realname~='' and lfs.isdir(realname) then
    if trace_locating then
      report_trees("locator %a found",realname)
    end
    resolvers.appendhash('tree',name,false) 
  elseif trace_locating then
    report_trees("locator %a not found",name)
  end
end
function resolvers.hashers.tree(specification)
  local name=specification.filename
  if trace_locating then
    report_trees("analysing %a",name)
  end
  resolvers.methodhandler("hashers",name)
  resolvers.generators.file(specification)
end
resolvers.concatinators.tree=resolvers.concatinators.file
resolvers.generators.tree=resolvers.generators.file
resolvers.openers.tree=resolvers.openers.file
resolvers.loaders.tree=resolvers.loaders.file


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-sch"] = package.loaded["data-sch"] or true

-- original size: 6202, stripped down to: 5149

if not modules then modules={} end modules ['data-sch']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local load=load
local gsub,concat,format=string.gsub,table.concat,string.format
local finders,openers,loaders=resolvers.finders,resolvers.openers,resolvers.loaders
local trace_schemes=false trackers.register("resolvers.schemes",function(v) trace_schemes=v end)
local report_schemes=logs.reporter("resolvers","schemes")
local http=require("socket.http")
local ltn12=require("ltn12")
local resolvers=resolvers
local schemes=resolvers.schemes or {}
resolvers.schemes=schemes
local cleaners={}
schemes.cleaners=cleaners
local threshold=24*60*60
directives.register("schemes.threshold",function(v) threshold=tonumber(v) or threshold end)
function cleaners.none(specification)
  return specification.original
end
function cleaners.strip(specification)
  return (gsub(specification.original,"[^%a%d%.]+","-")) 
end
function cleaners.md5(specification)
  return file.addsuffix(md5.hex(specification.original),file.suffix(specification.path))
end
local cleaner=cleaners.strip
directives.register("schemes.cleanmethod",function(v) cleaner=cleaners[v] or cleaners.strip end)
function resolvers.schemes.cleanname(specification)
  local hash=cleaner(specification)
  if trace_schemes then
    report_schemes("hashing %a to %a",specification.original,hash)
  end
  return hash
end
local cached,loaded,reused,thresholds,handlers={},{},{},{},{}
local function runcurl(name,cachename) 
  local command="curl --silent --create-dirs --output "..cachename.." "..name
  os.spawn(command)
end
local function fetch(specification)
  local original=specification.original
  local scheme=specification.scheme
  local cleanname=schemes.cleanname(specification)
  local cachename=caches.setfirstwritablefile(cleanname,"schemes")
  if not cached[original] then
    statistics.starttiming(schemes)
    if not io.exists(cachename) or (os.difftime(os.time(),lfs.attributes(cachename).modification)>(thresholds[protocol] or threshold)) then
      cached[original]=cachename
      local handler=handlers[scheme]
      if handler then
        if trace_schemes then
          report_schemes("fetching %a, protocol %a, method %a",original,scheme,"built-in")
        end
        logs.flush()
        handler(specification,cachename)
      else
        if trace_schemes then
          report_schemes("fetching %a, protocol %a, method %a",original,scheme,"curl")
        end
        logs.flush()
        runcurl(original,cachename)
      end
    end
    if io.exists(cachename) then
      cached[original]=cachename
      if trace_schemes then
        report_schemes("using cached %a, protocol %a, cachename %a",original,scheme,cachename)
      end
    else
      cached[original]=""
      if trace_schemes then
        report_schemes("using missing %a, protocol %a",original,scheme)
      end
    end
    loaded[scheme]=loaded[scheme]+1
    statistics.stoptiming(schemes)
  else
    if trace_schemes then
      report_schemes("reusing %a, protocol %a",original,scheme)
    end
    reused[scheme]=reused[scheme]+1
  end
  return cached[original]
end
local function finder(specification,filetype)
  return resolvers.methodhandler("finders",fetch(specification),filetype)
end
local opener=openers.file
local loader=loaders.file
local function install(scheme,handler,newthreshold)
  handlers [scheme]=handler
  loaded  [scheme]=0
  reused  [scheme]=0
  finders  [scheme]=finder
  openers  [scheme]=opener
  loaders  [scheme]=loader
  thresholds[scheme]=newthreshold or threshold
end
schemes.install=install
local function http_handler(specification,cachename)
  local tempname=cachename..".tmp"
  local f=io.open(tempname,"wb")
  local status,message=http.request {
    url=specification.original,
    sink=ltn12.sink.file(f)
  }
  if not status then
    os.remove(tempname)
  else
    os.remove(cachename)
    os.rename(tempname,cachename)
  end
  return cachename
end
install('http',http_handler)
install('https') 
install('ftp')
statistics.register("scheme handling time",function()
  local l,r,nl,nr={},{},0,0
  for k,v in table.sortedhash(loaded) do
    if v>0 then
      nl=nl+1
      l[nl]=k..":"..v
    end
  end
  for k,v in table.sortedhash(reused) do
    if v>0 then
      nr=nr+1
      r[nr]=k..":"..v
    end
  end
  local n=nl+nr
  if n>0 then
    l=nl>0 and concat(l) or "none"
    r=nr>0 and concat(r) or "none"
    return format("%s seconds, %s processed, threshold %s seconds, loaded: %s, reused: %s",
      statistics.elapsedtime(schemes),n,threshold,l,r)
  else
    return nil
  end
end)
local httprequest=http.request
local toquery=url.toquery
local function fetchstring(url,data)
  local q=data and toquery(data)
  if q then
    url=url.."?"..q
  end
  local reply=httprequest(url)
  return reply 
end
schemes.fetchstring=fetchstring
function schemes.fetchtable(url,data)
  local reply=fetchstring(url,data)
  if reply then
    local s=load("return "..reply)
    if s then
      return s()
    end
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-lua"] = package.loaded["data-lua"] or true

-- original size: 4861, stripped down to: 3693

if not modules then modules={} end modules ['data-lua']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local resolvers,package=resolvers,package
local gsub=string.gsub
local concat=table.concat
local addsuffix=file.addsuffix
local P,S,Cs,lpegmatch=lpeg.P,lpeg.S,lpeg.Cs,lpeg.match
local libsuffixes={ 'tex','lua' }
local clibsuffixes={ 'lib' }
local libformats={ 'TEXINPUTS','LUAINPUTS' }
local clibformats={ 'CLUAINPUTS' }
local helpers=package.helpers
trackers.register("resolvers.libraries",function(v) helpers.trace=v end)
trackers.register("resolvers.locating",function(v) helpers.trace=v end)
helpers.report=logs.reporter("resolvers","libraries")
local pattern=Cs(P("!")^0/""*(P("/")*P(-1)/"/"+P("/")^1/"/"+1)^0)
local function cleanpath(path) 
  return resolvers.resolve(lpegmatch(pattern,path))
end
helpers.cleanpath=cleanpath
local loadedaslib=helpers.loadedaslib
local loadedbylua=helpers.loadedbylua
local loadedbypath=helpers.loadedbypath
local notloaded=helpers.notloaded
local getlibpaths=package.libpaths
local getclibpaths=package.clibpaths
function helpers.libpaths(libhash)
  local libpaths={}
  for i=1,#libformats do
    local paths=resolvers.expandedpathlistfromvariable(libformats[i])
    for i=1,#paths do
      local path=cleanpath(paths[i])
      if not libhash[path] then
        libpaths[#libpaths+1]=path
        libhash[path]=true
      end
    end
  end
  return libpaths
end
function helpers.clibpaths(clibhash)
  local clibpaths={}
  for i=1,#clibformats do
    local paths=resolvers.expandedpathlistfromvariable(clibformats[i])
    for i=1,#paths do
      local path=cleanpath(paths[i])
      if not clibhash[path] then
        clibpaths[#clibpaths+1]=path
        clibhash[path]=true
      end
    end
  end
  return clibpaths
end
local function loadedbyformat(name,rawname,suffixes,islib)
  local trace=helpers.trace
  local report=helpers.report
  if trace then
    report("locating %a as %a using formats %a",rawname,name,suffixes)
  end
  for i=1,#suffixes do 
    local format=suffixes[i]
    local resolved=resolvers.findfile(name,format) or ""
    if trace then
      report("checking %a using format %a",name,format)
    end
    if resolved~="" then
      if trace then
        report("lib %a located on %a",name,resolved)
      end
      if islib then
        return true,loadedaslib(resolved,rawname)
      else
        return true,loadfile(resolved)
      end
    end
  end
end
helpers.loadedbyformat=loadedbyformat
local pattern=Cs((((1-S("\\/"))^0*(S("\\/")^1/"/"))^0*(P(".")^1/"/"+P(1))^1)*-1)
local function lualibfile(name)
  return lpegmatch(pattern,name) or name
end
helpers.lualibfile=lualibfile
function helpers.loaded(name)
  local thename=lualibfile(name)
  local luaname=addsuffix(thename,"lua")
  local libname=addsuffix(thename,os.libsuffix)
  local libpaths=getlibpaths()
  local clibpaths=getclibpaths()
  local done,result=loadedbyformat(luaname,name,libsuffixes,false)
  if done then
    return result
  end
  local done,result=loadedbyformat(libname,name,clibsuffixes,true)
  if done then
    return result
  end
  local done,result=loadedbypath(luaname,name,libpaths,false,"lua")
  if done then
    return result
  end
  local done,result=loadedbypath(luaname,name,clibpaths,false,"lua")
  if done then
    return result
  end
  local done,result=loadedbypath(libname,name,clibpaths,true,"lib")
  if done then
    return result
  end
  local done,result=loadedbylua(name)
  if done then
    return result
  end
  return notloaded(name)
end
resolvers.loadlualib=require


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-aux"] = package.loaded["data-aux"] or true

-- original size: 2394, stripped down to: 2005

if not modules then modules={} end modules ['data-aux']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local find=string.find
local type,next=type,next
local trace_locating=false trackers.register("resolvers.locating",function(v) trace_locating=v end)
local resolvers=resolvers
local report_scripts=logs.reporter("resolvers","scripts")
function resolvers.updatescript(oldname,newname) 
  local scriptpath="scripts/context/lua"
  newname=file.addsuffix(newname,"lua")
  local oldscript=resolvers.cleanpath(oldname)
  if trace_locating then
    report_scripts("to be replaced old script %a",oldscript)
  end
  local newscripts=resolvers.findfiles(newname) or {}
  if #newscripts==0 then
    if trace_locating then
      report_scripts("unable to locate new script")
    end
  else
    for i=1,#newscripts do
      local newscript=resolvers.cleanpath(newscripts[i])
      if trace_locating then
        report_scripts("checking new script %a",newscript)
      end
      if oldscript==newscript then
        if trace_locating then
          report_scripts("old and new script are the same")
        end
      elseif not find(newscript,scriptpath) then
        if trace_locating then
          report_scripts("new script should come from %a",scriptpath)
        end
      elseif not (find(oldscript,file.removesuffix(newname).."$") or find(oldscript,newname.."$")) then
        if trace_locating then
          report_scripts("invalid new script name")
        end
      else
        local newdata=io.loaddata(newscript)
        if newdata then
          if trace_locating then
            report_scripts("old script content replaced by new content")
          end
          io.savedata(oldscript,newdata)
          break
        elseif trace_locating then
          report_scripts("unable to load new script")
        end
      end
    end
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-tmf"] = package.loaded["data-tmf"] or true

-- original size: 2600, stripped down to: 1627

if not modules then modules={} end modules ['data-tmf']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local resolvers=resolvers
local report_tds=logs.reporter("resolvers","tds")
function resolvers.load_tree(tree,resolve)
  if type(tree)=="string" and tree~="" then
    local getenv,setenv=resolvers.getenv,resolvers.setenv
    local texos="texmf-"..os.platform
    local oldroot=environment.texroot
    local newroot=file.collapsepath(tree)
    local newtree=file.join(newroot,texos)
    local newpath=file.join(newtree,"bin")
    if not lfs.isdir(newtree) then
      report_tds("no %a under tree %a",texos,tree)
      os.exit()
    end
    if not lfs.isdir(newpath) then
      report_tds("no '%s/bin' under tree %a",texos,tree)
      os.exit()
    end
    local texmfos=newtree
    environment.texroot=newroot
    environment.texos=texos
    environment.texmfos=texmfos
    if resolve then
      resolvers.luacnfspec=resolvers.resolve(resolvers.luacnfspec)
    end
    setenv('SELFAUTOPARENT',newroot)
    setenv('SELFAUTODIR',newtree)
    setenv('SELFAUTOLOC',newpath)
    setenv('TEXROOT',newroot)
    setenv('TEXOS',texos)
    setenv('TEXMFOS',texmfos)
    setenv('TEXMFCNF',resolvers.luacnfspec,true) 
    setenv('PATH',newpath..io.pathseparator..getenv('PATH'))
    report_tds("changing from root %a to %a",oldroot,newroot)
    report_tds("prepending %a to PATH",newpath)
    report_tds("setting TEXMFCNF to %a",resolvers.luacnfspec)
    report_tds()
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-lst"] = package.loaded["data-lst"] or true

-- original size: 2654, stripped down to: 2301

if not modules then modules={} end modules ['data-lst']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local find,concat,upper,format=string.find,table.concat,string.upper,string.format
local fastcopy,sortedpairs=table.fastcopy,table.sortedpairs
resolvers.listers=resolvers.listers or {}
local resolvers=resolvers
local report_lists=logs.reporter("resolvers","lists")
local function tabstr(str)
  if type(str)=='table' then
    return concat(str," | ")
  else
    return str
  end
end
function resolvers.listers.variables(pattern)
  local instance=resolvers.instance
  local environment=instance.environment
  local variables=instance.variables
  local expansions=instance.expansions
  local pattern=upper(pattern or "")
  local configured={}
  local order=instance.order
  for i=1,#order do
    for k,v in next,order[i] do
      if v~=nil and configured[k]==nil then
        configured[k]=v
      end
    end
  end
  local env=fastcopy(environment)
  local var=fastcopy(variables)
  local exp=fastcopy(expansions)
  for key,value in sortedpairs(configured) do
    if key~="" and (pattern=="" or find(upper(key),pattern)) then
      report_lists(key)
      report_lists("  env: %s",tabstr(rawget(environment,key))  or "unset")
      report_lists("  var: %s",tabstr(configured[key])      or "unset")
      report_lists("  exp: %s",tabstr(expansions[key])      or "unset")
      report_lists("  res: %s",tabstr(resolvers.resolve(expansions[key])) or "unset")
    end
  end
  instance.environment=fastcopy(env)
  instance.variables=fastcopy(var)
  instance.expansions=fastcopy(exp)
end
local report_resolved=logs.reporter("system","resolved")
function resolvers.listers.configurations()
  local configurations=resolvers.instance.specification
  for i=1,#configurations do
    report_resolved("file : %s",resolvers.resolve(configurations[i]))
  end
  report_resolved("")
  local list=resolvers.expandedpathfromlist(resolvers.splitpath(resolvers.luacnfspec))
  for i=1,#list do
    local li=resolvers.resolve(list[i])
    if lfs.isdir(li) then
      report_resolved("path - %s",li)
    else
      report_resolved("path + %s",li)
    end
  end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-lib"] = package.loaded["util-lib"] or true

-- original size: 10762, stripped down to: 5269

if not modules then modules={} end modules ['util-lib']={
  version=1.001,
  comment="companion to luat-lib.mkiv",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files",
}
local gsub,find=string.gsub,string.find
local pathpart,nameonly,joinfile=file.pathpart,file.nameonly,file.join
local findfile,findfiles=resolvers and resolvers.findfile,resolvers and resolvers.findfiles
local loaded=package.loaded
local report_swiglib=logs.reporter("swiglib")
local trace_swiglib=false trackers.register("resolvers.swiglib",function(v) trace_swiglib=v end)
local done=false
local function requireswiglib(required,version)
  local library=loaded[required]
  if library==nil then
    local required_full=gsub(required,"%.","/")
    local required_path=pathpart(required_full)
    local required_base=nameonly(required_full)
    local required_name=required_base.."."..os.libsuffix
    local version=type(version)=="string" and version~="" and version or false
    local engine=environment.ownmain or false
    if trace_swiglib and not done then
      local list=resolvers.expandedpathlistfromvariable("lib")
      for i=1,#list do
        report_swiglib("tds path %i: %s",i,list[i])
      end
    end
    local function found(locate,asked_library,how,...)
      if trace_swiglib then
        report_swiglib("checking %s: %a",how,asked_library)
      end
      return locate(asked_library,...)
    end
    local function check(locate,...)
      local found=nil
      if version then
        local asked_library=joinfile(required_path,version,required_name)
        if trace_swiglib then
          report_swiglib("checking %s: %a","with version",asked_library)
        end
        found=locate(asked_library,...)
      end
      if not found or found=="" then
        local asked_library=joinfile(required_path,required_name)
        if trace_swiglib then
          report_swiglib("checking %s: %a","with version",asked_library)
        end
        found=locate(asked_library,...)
      end
      return found and found~="" and found or false
    end
    local function attempt(checkpattern)
      if trace_swiglib then
        report_swiglib("checking tds lib paths strictly")
      end
      local found=findfile and check(findfile,"lib")
      if found and (not checkpattern or find(found,checkpattern)) then
        return found
      end
      if trace_swiglib then
        report_swiglib("checking tds lib paths with wildcard")
      end
      local asked_library=joinfile(required_path,".*",required_name)
      if trace_swiglib then
        report_swiglib("checking %s: %a","latest version",asked_library)
      end
      local list=findfiles(asked_library,"lib",true)
      if list and #list>0 then
        table.sort(list)
        local found=list[#list]
        if found and (not checkpattern or find(found,checkpattern)) then
          return found
        end
      end
      if trace_swiglib then
        report_swiglib("checking clib paths")
      end
      package.extraclibpath(environment.ownpath)
      local paths=package.clibpaths()
      for i=1,#paths do
        local found=check(lfs.isfile)
        if found and (not checkpattern or find(found,checkpattern)) then
          return found
        end
      end
      return false
    end
    local found_library=nil
    if engine then
      if trace_swiglib then
        report_swiglib("attemp 1, engine %a",engine)
      end
      found_library=attempt("/"..engine.."/")
      if not found_library then
        if trace_swiglib then
          report_swiglib("attemp 2, no engine",asked_library)
        end
        found_library=attempt()
      end
    else
      found_library=attempt()
    end
    if not found_library then
      if trace_swiglib then
        report_swiglib("not found: %a",asked_library)
      end
      library=false
    else
      local path=pathpart(found_library)
      local base=nameonly(found_library)
      dir.push(path)
      if trace_swiglib then
        report_swiglib("found: %a",found_library)
      end
      library=package.loadlib(found_library,"luaopen_"..required_base)
      if type(library)=="function" then
        library=library()
      else
        library=false
      end
      dir.pop()
    end
    if not library then
      report_swiglib("unknown: %a",required)
    elseif trace_swiglib then
      report_swiglib("stored: %a",required)
    end
    loaded[required]=library
  else
    report_swiglib("reused: %a",required)
  end
  return library
end
local savedrequire=require
function require(name,version)
  if find(name,"^swiglib%.") then
    return requireswiglib(name,version)
  else
    return savedrequire(name)
  end
end
local swiglibs={}
function swiglib(name,version)
  local library=swiglibs[name]
  if not library then
    statistics.starttiming(swiglibs)
    report_swiglib("loading %a",name)
    library=requireswiglib("swiglib."..name,version)
    swiglibs[name]=library
    statistics.stoptiming(swiglibs)
  end
  return library
end
statistics.register("used swiglibs",function()
  if next(swiglibs) then
    return string.format("%s, initial load time %s seconds",table.concat(table.sortedkeys(swiglibs)," "),statistics.elapsedtime(swiglibs))
  end
end)


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["luat-sta"] = package.loaded["luat-sta"] or true

-- original size: 5703, stripped down to: 2507

if not modules then modules={} end modules ['luat-sta']={
  version=1.001,
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local gmatch,match=string.gmatch,string.match
local type=type
states=states or {}
local states=states
states.data=states.data or {}
local data=states.data
states.hash=states.hash or {}
local hash=states.hash
states.tag=states.tag   or ""
states.filename=states.filename or ""
function states.save(filename,tag)
  tag=tag or states.tag
  filename=file.addsuffix(filename or states.filename,'lus')
  io.savedata(filename,
    "-- generator : luat-sta.lua\n".."-- state tag : "..tag.."\n\n"..table.serialize(data[tag or states.tag] or {},true)
  )
end
function states.load(filename,tag)
  states.filename=filename
  states.tag=tag or "whatever"
  states.filename=file.addsuffix(states.filename,'lus')
  data[states.tag],hash[states.tag]=(io.exists(filename) and dofile(filename)) or {},{}
end
local function set_by_tag(tag,key,value,default,persistent)
  local d,h=data[tag],hash[tag]
  if d then
    if type(d)=="table" then
      local dkey,hkey=key,key
      local pre,post=match(key,"(.+)%.([^%.]+)$")
      if pre and post then
        for k in gmatch(pre,"[^%.]+") do
          local dk=d[k]
          if not dk then
            dk={}
            d[k]=dk
          elseif type(dk)=="string" then
            break
          end
          d=dk
        end
        dkey,hkey=post,key
      end
      if value==nil then
        value=default
      elseif value==false then
      elseif persistent then
        value=value or d[dkey] or default
      else
        value=value or default
      end
      d[dkey],h[hkey]=value,value
    elseif type(d)=="string" then
      data[tag],hash[tag]=value,value
    end
  end
end
local function get_by_tag(tag,key,default)
  local h=hash[tag]
  if h and h[key] then
    return h[key]
  else
    local d=data[tag]
    if d then
      for k in gmatch(key,"[^%.]+") do
        local dk=d[k]
        if dk~=nil then
          d=dk
        else
          return default
        end
      end
      if d==false then
        return false
      else
        return d or default
      end
    end
  end
end
states.set_by_tag=set_by_tag
states.get_by_tag=get_by_tag
function states.set(key,value,default,persistent)
  set_by_tag(states.tag,key,value,default,persistent)
end
function states.get(key,default)
  return get_by_tag(states.tag,key,default)
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["luat-fmt"] = package.loaded["luat-fmt"] or true

-- original size: 5951, stripped down to: 4922

if not modules then modules={} end modules ['luat-fmt']={
  version=1.001,
  comment="companion to mtxrun",
  author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
  copyright="PRAGMA ADE / ConTeXt Development Team",
  license="see context related readme files"
}
local format=string.format
local concat=table.concat
local quoted=string.quoted
local luasuffixes=utilities.lua.suffixes
local report_format=logs.reporter("resolvers","formats")
local function primaryflags() 
  local trackers=environment.argument("trackers")
  local directives=environment.argument("directives")
  local flags={}
  if trackers and trackers~="" then
    flags={ "--trackers="..quoted(trackers) }
  end
  if directives and directives~="" then
    flags={ "--directives="..quoted(directives) }
  end
  if environment.argument("jit") then
    flags={ "--jiton" }
  end
  return concat(flags," ")
end
function environment.make_format(name)
  local engine=environment.ownmain or "luatex"
  local olddir=dir.current()
  local path=caches.getwritablepath("formats",engine) or "" 
  if path~="" then
    lfs.chdir(path)
  end
  report_format("using format path %a",dir.current())
  local texsourcename=file.addsuffix(name,"mkiv")
  local fulltexsourcename=resolvers.findfile(texsourcename,"tex") or ""
  if fulltexsourcename=="" then
    texsourcename=file.addsuffix(name,"tex")
    fulltexsourcename=resolvers.findfile(texsourcename,"tex") or ""
  end
  if fulltexsourcename=="" then
    report_format("no tex source file with name %a (mkiv or tex)",name)
    lfs.chdir(olddir)
    return
  else
    report_format("using tex source file %a",fulltexsourcename)
  end
  local texsourcepath=dir.expandname(file.dirname(fulltexsourcename))
  local specificationname=file.replacesuffix(fulltexsourcename,"lus")
  local fullspecificationname=resolvers.findfile(specificationname,"tex") or ""
  if fullspecificationname=="" then
    specificationname=file.join(texsourcepath,"context.lus")
    fullspecificationname=resolvers.findfile(specificationname,"tex") or ""
  end
  if fullspecificationname=="" then
    report_format("unknown stub specification %a",specificationname)
    lfs.chdir(olddir)
    return
  end
  local specificationpath=file.dirname(fullspecificationname)
  local usedluastub=nil
  local usedlualibs=dofile(fullspecificationname)
  if type(usedlualibs)=="string" then
    usedluastub=file.join(file.dirname(fullspecificationname),usedlualibs)
  elseif type(usedlualibs)=="table" then
    report_format("using stub specification %a",fullspecificationname)
    local texbasename=file.basename(name)
    local luastubname=file.addsuffix(texbasename,luasuffixes.lua)
    local lucstubname=file.addsuffix(texbasename,luasuffixes.luc)
    report_format("creating initialization file %a",luastubname)
    utilities.merger.selfcreate(usedlualibs,specificationpath,luastubname)
    if utilities.lua.compile(luastubname,lucstubname) and lfs.isfile(lucstubname) then
      report_format("using compiled initialization file %a",lucstubname)
      usedluastub=lucstubname
    else
      report_format("using uncompiled initialization file %a",luastubname)
      usedluastub=luastubname
    end
  else
    report_format("invalid stub specification %a",fullspecificationname)
    lfs.chdir(olddir)
    return
  end
  local command=format("%s --ini %s --lua=%s %s %sdump",engine,primaryflags(),quoted(usedluastub),quoted(fulltexsourcename),os.platform=="unix" and "\\\\" or "\\")
  report_format("running command: %s\n",command)
  os.spawn(command)
  local pattern=file.removesuffix(file.basename(usedluastub)).."-*.mem"
  local mp=dir.glob(pattern)
  if mp then
    for i=1,#mp do
      local name=mp[i]
      report_format("removing related mplib format %a",file.basename(name))
      os.remove(name)
    end
  end
  lfs.chdir(olddir)
end
function environment.run_format(name,data,more)
  if name and name~="" then
    local engine=environment.ownmain or "luatex"
    local barename=file.removesuffix(name)
    local fmtname=caches.getfirstreadablefile(file.addsuffix(barename,"fmt"),"formats",engine)
    if fmtname=="" then
      fmtname=resolvers.findfile(file.addsuffix(barename,"fmt")) or ""
    end
    fmtname=resolvers.cleanpath(fmtname)
    if fmtname=="" then
      report_format("no format with name %a",name)
    else
      local barename=file.removesuffix(name) 
      local luaname=file.addsuffix(barename,"luc")
      if not lfs.isfile(luaname) then
        luaname=file.addsuffix(barename,"lua")
      end
      if not lfs.isfile(luaname) then
        report_format("using format name %a",fmtname)
        report_format("no luc/lua file with name %a",barename)
      else
        local command=format("%s %s --fmt=%s --lua=%s %s %s",engine,primaryflags(),quoted(barename),quoted(luaname),quoted(data),more~="" and quoted(more) or "")
        report_format("running command: %s",command)
        os.spawn(command)
      end
    end
  end
end


end -- of closure

-- used libraries    : l-lua.lua l-lpeg.lua l-function.lua l-string.lua l-table.lua l-io.lua l-number.lua l-set.lua l-os.lua l-file.lua l-md5.lua l-url.lua l-dir.lua l-boolean.lua l-unicode.lua l-math.lua util-str.lua util-tab.lua util-sto.lua util-prs.lua util-fmt.lua trac-set.lua trac-log.lua trac-inf.lua trac-pro.lua util-lua.lua util-deb.lua util-mrg.lua util-tpl.lua util-env.lua luat-env.lua lxml-tab.lua lxml-lpt.lua lxml-mis.lua lxml-aux.lua lxml-xml.lua trac-xml.lua data-ini.lua data-exp.lua data-env.lua data-tmp.lua data-met.lua data-res.lua data-pre.lua data-inp.lua data-out.lua data-fil.lua data-con.lua data-use.lua data-zip.lua data-tre.lua data-sch.lua data-lua.lua data-aux.lua data-tmf.lua data-lst.lua util-lib.lua luat-sta.lua luat-fmt.lua
-- skipped libraries : -
-- original bytes    : 660467
-- stripped bytes    : 242438

-- end library merge

-- We need this hack till luatex is fixed.
--
-- for k,v in pairs(arg) do print(k,v) end

if arg and (arg[0] == 'luatex' or arg[0] == 'luatex.exe') and arg[1] == "--luaonly" then
    arg[-1]=arg[0] arg[0]=arg[2] for k=3,#arg do arg[k-2]=arg[k] end arg[#arg]=nil arg[#arg]=nil
end

-- End of hack.

local format, gsub, gmatch, match, find = string.format, string.gsub, string.gmatch, string.match, string.find
local concat = table.concat

local ownname = environment and environment.ownname or arg[0] or 'mtxrun.lua'
local ownpath = gsub(match(ownname,"^(.+)[\\/].-$") or ".","\\","/")
local owntree = environment and environment.ownpath or ownpath

local ownlibs = { -- order can be made better

    'l-lua.lua',
    'l-lpeg.lua',
    'l-function.lua',
    'l-string.lua',
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

    'util-str.lua', -- code might move to l-string
    'util-tab.lua',
    'util-sto.lua',
    'util-prs.lua',
    'util-fmt.lua',

    'trac-set.lua',
    'trac-log.lua',
    'trac-inf.lua', -- was before trac-set
    'trac-pro.lua', -- not really needed
    'util-lua.lua', -- indeed here?
    'util-deb.lua',

    'util-mrg.lua',
    'util-tpl.lua',

    'util-env.lua',
    'luat-env.lua', -- can come before inf (as in mkiv)

    'lxml-tab.lua',
    'lxml-lpt.lua',
 -- 'lxml-ent.lua',
    'lxml-mis.lua',
    'lxml-aux.lua',
    'lxml-xml.lua',

    'trac-xml.lua',

    'data-ini.lua',
    'data-exp.lua',
    'data-env.lua',
    'data-tmp.lua',
    'data-met.lua',
    'data-res.lua',
    'data-pre.lua',
    'data-inp.lua',
    'data-out.lua',
    'data-fil.lua',
    'data-con.lua',
    'data-use.lua',
--  'data-tex.lua',
--  'data-bin.lua',
    'data-zip.lua',
    'data-tre.lua',
    'data-sch.lua',
    'data-lua.lua',
    'data-aux.lua', -- updater
    'data-tmf.lua',
    'data-lst.lua',

    'util-lib.lua', -- swiglib

    'luat-sta.lua',
    'luat-fmt.lua',

}

local ownlist = {
    '.',
    ownpath ,
    ownpath .. "/../sources", -- HH's development path
    owntree .. "/../../texmf-local/tex/context/base",
    owntree .. "/../../texmf-context/tex/context/base",
    owntree .. "/../../texmf-dist/tex/context/base",
    owntree .. "/../../texmf/tex/context/base",
    owntree .. "/../../../texmf-local/tex/context/base",
    owntree .. "/../../../texmf-context/tex/context/base",
    owntree .. "/../../../texmf-dist/tex/context/base",
    owntree .. "/../../../texmf/tex/context/base",
}

if ownpath == "." then table.remove(ownlist,1) end

own = {
    name = ownname,
    path = ownpath,
    tree = owntree,
    list = ownlist,
    libs = ownlibs,
}

local function locate_libs()
    for l=1,#ownlibs do
        local lib = ownlibs[l]
        for p =1,#ownlist do
            local pth = ownlist[p]
            local filename = pth .. "/" .. lib
            local found = lfs.isfile(filename)
            if found then
                package.path = package.path .. ";" .. pth .. "/?.lua" -- in case l-* does a require
                return pth
            end
        end
    end
end

local function load_libs()
    local found = locate_libs()
    if found then
        for l=1,#ownlibs do
            local filename = found .. "/" .. ownlibs[l]
            local codeblob = loadfile(filename)
            if codeblob then
                codeblob()
            end
        end
    else
        resolvers = nil
    end
end

if not resolvers then
    load_libs()
end

if not resolvers then
    print("")
    print("Mtxrun is unable to start up due to lack of libraries. You may")
    print("try to run 'lua mtxrun.lua --selfmerge' in the path where this")
    print("script is located (normally under ..../scripts/context/lua) which")
    print("will make this script library independent.")
    os.exit()
end

-- verbosity

local e_verbose = environment.arguments["verbose"]

if e_verbose then
    trackers.enable("resolvers.locating")
end

-- some common flags (also passed through environment)

local e_silent       = environment.argument("silent")
local e_noconsole    = environment.argument("noconsole")

local e_trackers     = environment.argument("trackers")
local e_directives   = environment.argument("directives")
local e_experiments  = environment.argument("experiments")

if e_silent == true then
    e_silent = "*"
end

if type(e_silent) == "string" then
    if type(e_directives) == "string" then
        e_directives = format("%s,logs.blocked={%s}",e_directives,e_silent)
    else
        e_directives = format("logs.blocked={%s}",e_silent)
    end
end

if e_noconsole then
    if type(e_directives) == "string" then
        e_directives = format("%s,logs.target=file",e_directives)
    else
        e_directives = format("logs.target=file")
    end
end

if e_trackers    then trackers   .enable(e_trackers)    end
if e_directives  then directives .enable(e_directives)  end
if e_experiments then experiments.enable(e_experiments) end

if not environment.trackers    then environment.trackers    = e_trackers    end
if not environment.directives  then environment.directives  = e_directives  end
if not environment.experiments then environment.experiments = e_experiments end

--

local instance = resolvers.reset()

local helpinfo = [[
<?xml version="1.0" ?>
<application>
 <metadata>
  <entry name="name">mtxrun</entry>
  <entry name="detail">ConTeXt TDS Runner Tool</entry>
  <entry name="version">1.31</entry>
 </metadata>
 <flags>
  <category name="basic">
   <subcategory>
    <flag name="script"><short>run an mtx script (lua prefered method) (<ref name="noquotes"/>), no script gives list</short></flag>
    <flag name="execute"><short>run a script or program (texmfstart method) (<ref name="noquotes"/>)</short></flag>
    <flag name="resolve"><short>resolve prefixed arguments</short></flag>
    <flag name="ctxlua"><short>run internally (using preloaded libs)</short></flag>
    <flag name="internal"><short>run script using built in libraries (same as <ref name="ctxlua"/>)</short></flag>
    <flag name="locate"><short>locate given filename in database (default) or system (<ref name="first"/> <ref name="all"/> <ref name="detail"/>)</short></flag>
   </subcategory>
   <subcategory>
    <flag name="autotree"><short>use texmf tree cf. env texmfstart_tree or texmfstarttree</short></flag>
    <flag name="tree" value="pathtotree"><short>use given texmf tree (default file: setuptex.tmf)</short></flag>
    <flag name="environment" value="name"><short>use given (tmf) environment file</short></flag>
    <flag name="path" value="runpath"><short>go to given path before execution</short></flag>
    <flag name="ifchanged" value="filename"><short>only execute when given file has changed (md checksum)</short></flag>
    <flag name="iftouched" value="old,new"><short>only execute when given file has changed (time stamp)</short></flag>
   </subcategory>
   <subcategory>
    <flag name="makestubs"><short>create stubs for (context related) scripts</short></flag>
    <flag name="removestubs"><short>remove stubs (context related) scripts</short></flag>
    <flag name="stubpath" value="binpath"><short>paths where stubs wil be written</short></flag>
    <flag name="windows"><short>create windows (mswin) stubs</short></flag>
    <flag name="unix"><short>create unix (linux) stubs</short></flag>
   </subcategory>
   <subcategory>
    <flag name="verbose"><short>give a bit more info</short></flag>
    <flag name="trackers" value="list"><short>enable given trackers</short></flag>
    <flag name="progname" value="str"><short>format or backend</short></flag>
   </subcategory>
   <subcategory>
    <flag name="edit"><short>launch editor with found file</short></flag>
    <flag name="launch"><short>launch files like manuals, assumes os support (<ref name="all"/>)</short></flag>
   </subcategory>
   <subcategory>
    <flag name="timedrun"><short>run a script and time its run</short></flag>
    <flag name="autogenerate"><short>regenerate databases if needed (handy when used to run context in an editor)</short></flag>
   </subcategory>
   <subcategory>
    <flag name="usekpse"><short>use kpse as fallback (when no mkiv and cache installed, often slower)</short></flag>
    <flag name="forcekpse"><short>force using kpse (handy when no mkiv and cache installed but less functionality)</short></flag>
   </subcategory>
   <subcategory>
    <flag name="prefixes"><short>show supported prefixes</short></flag>
   </subcategory>
   <subcategory>
    <flag name="generate"><short>generate file database</short></flag>
   </subcategory>
   <subcategory>
    <flag name="variables"><short>show configuration variables</short></flag>
    <flag name="configurations"><short>show configuration order</short></flag>
   </subcategory>
   <subcategory>
    <flag name="directives"><short>show (known) directives</short></flag>
    <flag name="trackers"><short>show (known) trackers</short></flag>
    <flag name="experiments"><short>show (known) experiments</short></flag>
   </subcategory>
   <subcategory>
    <flag name="expand-braces"><short>expand complex variable</short></flag>
    <flag name="expand-path"><short>expand variable (resolve paths)</short></flag>
    <flag name="expand-var"><short>expand variable (resolve references)</short></flag>
    <flag name="show-path"><short>show path expansion of ...</short></flag>
    <flag name="var-value"><short>report value of variable</short></flag>
    <flag name="find-file"><short>report file location</short></flag>
    <flag name="find-path"><short>report path of file</short></flag>
   </subcategory>
   <subcategory>
    <flag name="pattern" value="string"><short>filter variables</short></flag>
   </subcategory>
  </category>
 </flags>
</application>
]]

local application = logs.application {
    name     = "mtxrun",
    banner   = "ConTeXt TDS Runner Tool 1.31",
    helpinfo = helpinfo,
}

local report = application.report

messages = messages or { } -- for the moment

runners = runners  or { } -- global (might become local)

runners.applications = {
    ["lua"] = "luatex --luaonly",
    ["luc"] = "luatex --luaonly",
    ["pl"] = "perl",
    ["py"] = "python",
    ["rb"] = "ruby",
}

runners.suffixes = {
    'rb', 'lua', 'py', 'pl'
}

runners.registered = {
    texexec      = { 'texexec.rb',      false },  -- context mkii runner (only tool not to be luafied)
    texutil      = { 'texutil.rb',      true  },  -- old perl based index sorter for mkii (old versions need it)
    texfont      = { 'texfont.pl',      true  },  -- perl script that makes mkii font metric files
    texfind      = { 'texfind.pl',      false },  -- perltk based tex searching tool, mostly used at pragma
    texshow      = { 'texshow.pl',      false },  -- perltk based context help system, will be luafied
 -- texwork      = { 'texwork.pl',      false },  -- perltk based editing environment, only used at pragma
    makempy      = { 'makempy.pl',      true  },
    mptopdf      = { 'mptopdf.pl',      true  },
    pstopdf      = { 'pstopdf.rb',      true  },  -- converts ps (and some more) images, does some cleaning (replaced)
 -- examplex     = { 'examplex.rb',     false },
    concheck     = { 'concheck.rb',     false },
    runtools     = { 'runtools.rb',     true  },
    textools     = { 'textools.rb',     true  },
    tmftools     = { 'tmftools.rb',     true  },
    ctxtools     = { 'ctxtools.rb',     true  },
    rlxtools     = { 'rlxtools.rb',     true  },
    pdftools     = { 'pdftools.rb',     true  },
    mpstools     = { 'mpstools.rb',     true  },
 -- exatools     = { 'exatools.rb',     true  },
    xmltools     = { 'xmltools.rb',     true  },
 -- luatools     = { 'luatools.lua',    true  },
    mtxtools     = { 'mtxtools.rb',     true  },
    pdftrimwhite = { 'pdftrimwhite.pl', false },
}

runners.launchers = {
    windows = { },
    unix    = { },
}

-- like runners.libpath("framework"): looks on script's subpath

function runners.libpath(...)
    package.prepend_libpath(file.dirname(environment.ownscript),...)
    package.prepend_libpath(file.dirname(environment.ownname)  ,...)
end

function runners.prepare()
    local checkname = environment.argument("ifchanged")
    if type(checkname) == "string" and checkname ~= "" then
        local oldchecksum = file.loadchecksum(checkname)
        local newchecksum = file.checksum(checkname)
        if oldchecksum == newchecksum then
            if e_verbose then
                report("file '%s' is unchanged",checkname)
            end
            return "skip"
        elseif e_verbose then
            report("file '%s' is changed, processing started",checkname)
        end
        file.savechecksum(checkname)
    end
    local touchname = environment.argument("iftouched")
    if type(touchname) == "string" and touchname ~= "" then
        local oldname, newname = string.splitup(touchname, ",")
        if oldname and newname and oldname ~= "" and newname ~= "" then
            if not file.needs_updating(oldname,newname) then
                if e_verbose then
                    report("file '%s' and '%s' have same age",oldname,newname)
                end
                return "skip"
            elseif e_verbose then
                report("file '%s' is older than '%s'",oldname,newname)
            end
        end
    end
    local runpath = environment.argument("path")
    if type(runpath) == "string" and not lfs.chdir(runpath) then
        report("unable to change to path '%s'",runpath)
        return "error"
    end
    runners.prepare = function() end
    return "run"
end

function runners.execute_script(fullname,internal,nosplit)
    local noquote = environment.argument("noquotes")
    if fullname and fullname ~= "" then
        local state = runners.prepare()
        if state == 'error' then
            return false
        elseif state == 'skip' then
            return true
        elseif state == "run" then
            local path, name, suffix = file.splitname(fullname)
            local result = ""
            if path ~= "" then
                result = fullname
            elseif name then
                name = gsub(name,"^int[%a]*:",function()
                    internal = true
                    return ""
                end )
                name = gsub(name,"^script:","")
                if suffix == "" and runners.registered[name] and runners.registered[name][1] then
                    name = runners.registered[name][1]
                    suffix = file.suffix(name)
                end
                if suffix == "" then
                    -- loop over known suffixes
                    for _,s in pairs(runners.suffixes) do
                        result = resolvers.findfile(name .. "." .. s, 'texmfscripts')
                        if result ~= "" then
                            break
                        end
                    end
                elseif runners.applications[suffix] then
                    result = resolvers.findfile(name, 'texmfscripts')
                else
                    -- maybe look on path
                    result = resolvers.findfile(name, 'other text files')
                end
            end
            if result and result ~= "" then
                if not no_split then
                    local before, after = environment.splitarguments(fullname) -- already done
                    environment.arguments_before, environment.arguments_after = before, after
                end
                if internal then
                    arg = { } for _,v in pairs(environment.arguments_after) do arg[#arg+1] = v end
                    environment.ownscript = result
                    dofile(result)
                else
                    local binary = runners.applications[file.suffix(result)]
                    result = string.quoted(string.unquoted(result))
                 -- if string.match(result,' ') and not string.match(result,"^\".*\"$") then
                 --     result = '"' .. result .. '"'
                 -- end
                    if binary and binary ~= "" then
                        result = binary .. " " .. result
                    end
                    local command = result .. " " .. environment.reconstructcommandline(environment.arguments_after,noquote)
                    if e_verbose then
                        report()
                        report("executing: %s",command)
                        report()
                        report()
                        io.flush()
                    end
                    -- no os.exec because otherwise we get the wrong return value
                    local code = os.execute(command) -- maybe spawn
                    if code == 0 then
                        return true
                    else
                        if binary then
                            binary = file.addsuffix(binary,os.binsuffix)
                            for p in gmatch(os.getenv("PATH"),"[^"..io.pathseparator.."]+") do
                                if lfs.isfile(file.join(p,binary)) then
                                    return false
                                end
                            end
                            report()
                            report("This script needs '%s' which seems not to be installed.",binary)
                            report()
                        end
                        return false
                    end
                end
            end
        end
    end
    return false
end

function runners.execute_program(fullname)
    local noquote = environment.argument("noquotes")
    if fullname and fullname ~= "" then
        local state = runners.prepare()
        if state == 'error' then
            return false
        elseif state == 'skip' then
            return true
        elseif state == "run" then
            local before, after = environment.splitarguments(fullname)
            for k=1,#after do after[k] = resolvers.resolve(after[k]) end
            environment.initializearguments(after)
            fullname = gsub(fullname,"^bin:","")
            local command = fullname .. " " .. (environment.reconstructcommandline(after or "",noquote) or "")
            report()
            report("executing: %s",command)
            report()
            report()
            io.flush()
            local code = os.exec(command) -- (fullname,unpack(after)) does not work / maybe spawn
            return code == 0
        end
    end
    return false
end

-- the --usekpse flag will fallback (not default) on kpse (hm, we can better update mtx-stubs)

local windows_stub = '@echo off\013\010setlocal\013\010set ownpath=%%~dp0%%\013\010texlua "%%ownpath%%mtxrun.lua" --usekpse --execute %s %%*\013\010endlocal\013\010'
local unix_stub    = '#!/bin/sh\010mtxrun --usekpse --execute %s \"$@\"\010'

function runners.handle_stubs(create)
    local stubpath = environment.argument('stubpath') or '.' -- 'auto' no longer subpathssupported
    local windows  = environment.argument('windows') or environment.argument('mswin') or false
    local unix     = environment.argument('unix') or environment.argument('linux') or false
    if not windows and not unix then
        if os.platform == "unix" then
            unix = true
        else
            windows = true
        end
    end
    for _,v in pairs(runners.registered) do
        local name, doit = v[1], v[2]
        if doit then
            local base = gsub(file.basename(name), "%.(.-)$", "")
            if create then
                if windows then
                    io.savedata(file.join(stubpath,base..".bat"),format(windows_stub,name))
                    report("windows stub for '%s' created",base)
                end
                if unix then
                    io.savedata(file.join(stubpath,base),format(unix_stub,name))
                    report("unix stub for '%s' created",base)
                end
            else
                if windows and (os.remove(file.join(stubpath,base..'.bat')) or os.remove(file.join(stubpath,base..'.cmd'))) then
                    report("windows stub for '%s' removed", base)
                end
                if unix and (os.remove(file.join(stubpath,base)) or os.remove(file.join(stubpath,base..'.sh'))) then
                    report("unix stub for '%s' removed",base)
                end
            end
        end
    end
end

function runners.resolve_string(filename)
    if filename and filename ~= "" then
        runners.report_location(resolvers.resolve(filename))
    end
end

-- differs from texmfstart where locate appends .com .exe .bat ... todo

function runners.locate_file(filename) -- was given file but only searches in tree
    if filename and filename ~= "" then
        if environment.argument("first") then
            runners.report_location(resolvers.findfile(filename))
         -- resolvers.dowithfilesandreport(resolvers.findfile,filename)
        elseif environment.argument("all") then
            local result, status = resolvers.findfiles(filename)
            if status and environment.argument("detail") then
                runners.report_location(status)
            else
                runners.report_location(result)
            end
        else
            runners.report_location(resolvers.findgivenfile(filename))
         -- resolvers.dowithfilesandreport(resolvers.findgivenfile,filename)
        end
    end
end

function runners.locate_platform()
    runners.report_location(os.platform)
end

function runners.report_location(result)
    if type(result) == "table" then
        for i=1,#result do
            if i > 1 then
                io.write("\n")
            end
            io.write(result[i])
        end
    else
        io.write(result)
    end
end

function runners.edit_script(filename) -- we assume that gvim is present on most systems (todo: also in cnf file)
    local editor = os.getenv("MTXRUN_EDITOR") or os.getenv("TEXMFSTART_EDITOR") or os.getenv("EDITOR") or 'gvim'
    local rest = resolvers.resolve(filename)
    if rest ~= "" then
        local command = editor .. " " .. rest
        if e_verbose then
            report()
            report("starting editor: %s",command)
            report()
            report()
        end
        os.launch(command)
    end
end

function runners.save_script_session(filename, list)
    local t = { }
    for i=1,#list do
        local key = list[i]
        t[key] = environment.arguments[key]
    end
    io.savedata(filename,table.serialize(t,true))
end

function runners.load_script_session(filename)
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

function resolvers.launch(str)
    -- maybe we also need to test on mtxrun.launcher.suffix environment
    -- variable or on windows consult the assoc and ftype vars and such
    local launchers = runners.launchers[os.platform] if launchers then
        local suffix = file.suffix(str) if suffix then
            local runner = launchers[suffix] if runner then
                str = runner .. " " .. str
            end
        end
    end
    os.launch(str)
end

function runners.launch_file(filename)
    trackers.enable("resolvers.locating")
    local allresults = environment.arguments["all"]
    local pattern = environment.arguments["pattern"]
    if not pattern or pattern == "" then
        pattern = filename
    end
    if not pattern or pattern == "" then
        report("provide name or --pattern=")
    else
        local t = resolvers.findfiles(pattern,nil,allresults)
        if not t or #t == 0 then
            t = resolvers.findfiles("*/" .. pattern,nil,allresults)
        end
        if not t or #t == 0 then
            t = resolvers.findfiles("*/" .. pattern .. "*",nil,allresults)
        end
        if t and #t > 0 then
            if allresults then
                for _, v in pairs(t) do
                    report("launching %s", v)
                    resolvers.launch(v)
                end
            else
                report("launching %s", t[1])
                resolvers.launch(t[1])
            end
        else
            report("no match for %s", pattern)
        end
    end
end

local mtxprefixes = {
    { "^mtx%-",    "mtx-"  },
    { "^mtx%-t%-", "mtx-t-" },
}

function runners.find_mtx_script(filename)
    local function found(name)
        local path = file.dirname(name)
        if path and path ~= "" then
            return false
        else
            local fullname = own and own.path and file.join(own.path,name)
            return io.exists(fullname) and fullname
        end
    end
    filename = file.addsuffix(filename,"lua")
    local basename = file.removesuffix(file.basename(filename))
    local suffix = file.suffix(filename)
    -- qualified path, raw name
    local fullname = file.is_qualified_path(filename) and io.exists(filename) and filename
    if fullname and fullname ~= "" then
        return fullname
    end
    -- current path, raw name
    fullname = "./" .. filename
    fullname = io.exists(fullname) and fullname
    if fullname and fullname ~= "" then
        return fullname
    end
    -- mtx- prefix checking
    for i=1,#mtxprefixes do
        local mtxprefix = mtxprefixes[i]
        mtxprefix = find(filename,mtxprefix[1]) and "" or mtxprefix[2]
        -- context namespace, mtx-<filename>
        fullname = mtxprefix .. filename
        fullname = found(fullname) or resolvers.findfile(fullname)
        if fullname and fullname ~= "" then
            return fullname
        end
        -- context namespace, mtx-<filename>s
        fullname = mtxprefix .. basename .. "s" .. "." .. suffix
        fullname = found(fullname) or resolvers.findfile(fullname)
        if fullname and fullname ~= "" then
            return fullname
        end
        -- context namespace, mtx-<filename minus trailing s>
        fullname = mtxprefix .. gsub(basename,"s$","") .. "." .. suffix
        fullname = found(fullname) or resolvers.findfile(fullname)
        if fullname and fullname ~= "" then
            return fullname
        end
    end
    -- context namespace, just <filename>
    fullname = resolvers.findfile(filename)
    return fullname
end

function runners.register_arguments(...)
    local arguments = environment.arguments_after
    local passedon = { ... }
    for i=#passedon,1,-1 do
        local pi = passedon[i]
        if pi then
            table.insert(arguments,1,pi)
        end
    end
end

function runners.execute_ctx_script(filename,...)
    runners.register_arguments(...)
    local arguments = environment.arguments_after
    local fullname = runners.find_mtx_script(filename) or ""
    if file.suffix(fullname) == "cld" then
        -- handy in editors where we force --autopdf
        report("running cld script: %s",filename)
        table.insert(arguments,1,fullname)
        table.insert(arguments,"--autopdf")
        fullname = runners.find_mtx_script("context") or ""
    end
    -- retry after generate but only if --autogenerate
    if fullname == "" and environment.argument("autogenerate") then -- might become the default
        instance.renewcache = true
        trackers.enable("resolvers.locating")
        resolvers.load()
        --
        fullname = runners.find_mtx_script(filename) or ""
    end
    -- that should do it
    if fullname ~= "" then
        local state = runners.prepare()
        if state == 'error' then
            return false
        elseif state == 'skip' then
            return true
        elseif state == "run" then
            -- load and save ... kind of undocumented
            arg = { } for _,v in pairs(arguments) do arg[#arg+1] = resolvers.resolve(v) end
            environment.initializearguments(arg)
            local loadname = environment.arguments['load']
            if loadname then
                if type(loadname) ~= "string" then loadname = file.basename(fullname) end
                loadname = file.replacesuffix(loadname,"cfg")
                runners.load_script_session(loadname)
            end
            filename = environment.files[1]
            if e_verbose then
                report("using script: %s\n",fullname)
            end
            environment.ownscript = fullname
            dofile(fullname)
            local savename = environment.arguments['save']
            if savename then
                local save_list = runners.save_list
                if save_list and next(save_list) then
                    if type(savename) ~= "string" then savename = file.basename(fullname) end
                    savename = file.replacesuffix(savename,"cfg")
                    runners.save_script_session(savename,save_list)
                end
            end
            return true
        end
    else
        if filename == "" or filename == "help" then
            local context = resolvers.findfile("mtx-context.lua")
            trackers.enable("resolvers.locating")
            if context ~= "" then
                local result = dir.glob((gsub(context,"mtx%-context","mtx-*"))) -- () needed
                local valid = { }
                table.sort(result)
                for i=1,#result do
                    local scriptname = result[i]
                    local scriptbase = match(scriptname,".*mtx%-([^%-]-)%.lua")
                    if scriptbase then
                        local data = io.loaddata(scriptname)
                        local banner, version = match(data,"[\n\r]logs%.extendbanner%s*%(%s*[\"\']([^\n\r]+)%s*(%d+%.%d+)")
                        if banner then
                            valid[#valid+1] = { scriptbase, version, banner }
                        end
                    end
                end
                if #valid > 0 then
                    application.identify()
                    report("no script name given, known scripts:")
                    report()
                    for k=1,#valid do
                        local v = valid[k]
                        report("%-12s  %4s  %s",v[1],v[2],v[3])
                    end
                end
            else
                report("no script name given")
            end
        else
            filename = file.addsuffix(filename,"lua")
            if file.is_qualified_path(filename) then
                report("unknown script '%s'",filename)
            else
                report("unknown script '%s' or 'mtx-%s'",filename,filename)
            end
        end
        return false
    end
end

function runners.prefixes()
    application.identify()
    report()
    report(concat(resolvers.allprefixes(true)," "))
end

function runners.timedrun(filename) -- just for me
    if filename and filename ~= "" then
        runners.timed(function() os.execute(filename) end)
    end
end

function runners.timed(action)
    statistics.timed(action)
end

function runners.associate(filename)
    os.launch(filename)
end

function runners.gethelp(filename)
    local url = environment.argument("url")
    if url and url ~= "" then
        local command = string.gsub(environment.argument("command") or "unknown","^%s*\\*(.-)%s*$","%1")
        url = utilities.templates.replace(url,{ command = command })
        os.launch(url)
    else
        report("no --url given")
    end
end

-- this is a bit dirty ... first we store the first filename and next we
-- split the arguments so that we only see the ones meant for this script
-- ... later we will use the second half

local filename = environment.files[1] or ""
local ok      = true

local before, after = environment.splitarguments(filename)
environment.arguments_before, environment.arguments_after = before, after
environment.initializearguments(before)

instance.lsrmode  = environment.argument("lsr") or false

-- maybe the unset has to go to this level

local is_mkii_stub = runners.registered[file.removesuffix(file.basename(filename))]

local e_argument = environment.argument

if e_argument("usekpse") or e_argument("forcekpse") or is_mkii_stub then

    resolvers.load_tree(e_argument('tree'),true) -- force resolve of TEXMFCNF

    os.setenv("engine","")
    os.setenv("progname","")

    local remapper = {
        otf   = "opentype fonts",
        ttf   = "truetype fonts",
        ttc   = "truetype fonts",
        pfb   = "type1 fonts",
        other = "other text files",
    }

    local progname = e_argument("progname") or 'context'

    local function kpse_initialized()
        texconfig.kpse_init = true
        local t = os.clock()
        local k = kpse.original.new("luatex",progname)
        local dummy = k:find_file("mtxrun.lua") -- so that we're initialized
        report("kpse fallback with progname '%s' initialized in %s seconds",progname,os.clock()-t)
        kpse_initialized = function() return k end
        return k
    end

    local findfile = resolvers.findfile
    local showpath = resolvers.showpath

    if e_argument("forcekpse") then

        function resolvers.findfile(name,kind)
            return (kpse_initialized():find_file(resolvers.cleanpath(name),(kind ~= "" and (remapper[kind] or kind)) or "tex") or "") or ""
        end
        function resolvers.showpath(name)
            return (kpse_initialized():show_path(name)) or ""
        end

    elseif e_argument("usekpse") or is_mkii_stub then

        resolvers.load()

        function resolvers.findfile(name,kind)
            local found = findfile(name,kind) or ""
            if found ~= "" then
                return found
            else
                return (kpse_initialized():find_file(resolvers.cleanpath(name),(kind ~= "" and (remapper[kind] or kind)) or "tex") or "") or ""
            end
        end
        function resolvers.showpath(name)
            local found = showpath(name) or ""
            if found ~= "" then
                return found
            else
                return (kpse_initialized():show_path(name)) or ""
            end
        end

    end

    function runners.loadbase()
    end

else

    function runners.loadbase(...)
        if not resolvers.load(...) then
            report("forcing cache reload")
            instance.renewcache = true
            trackers.enable("resolvers.locating")
            if not resolvers.load(...) then
                report("the resolver databases are not present or outdated")
            end
        end
    end

    resolvers.load_tree(e_argument('tree'),e_argument("resolve"))

end

if e_argument("script") or e_argument("scripts") then

    -- run a script by loading it (using libs), pass args

    runners.loadbase()
    if is_mkii_stub then
        ok = runners.execute_script(filename,false,true)
    else
        ok = runners.execute_ctx_script(filename)
    end

elseif e_argument("selfmerge") then

    -- embed used libraries

    runners.loadbase()
    local found = locate_libs()
    if found then
        utilities.merger.selfmerge(own.name,own.libs,{ found })
    end

elseif e_argument("selfclean") then

    -- remove embedded libraries

    runners.loadbase()
    utilities.merger.selfclean(own.name)

elseif e_argument("selfupdate") then

    runners.loadbase()
    trackers.enable("resolvers.locating")
    resolvers.updatescript(own.name,"mtxrun")

elseif e_argument("ctxlua") or e_argument("internal") then

    -- run a script by loading it (using libs)

    runners.loadbase()
    ok = runners.execute_script(filename,true)

elseif e_argument("execute") then

    -- execute script

    runners.loadbase()
    ok = runners.execute_script(filename)

elseif e_argument("direct") then

    -- equals bin:

    runners.loadbase()
    ok = runners.execute_program(filename)

elseif e_argument("edit") then

    -- edit file

    runners.loadbase()
    runners.edit_script(filename)

elseif e_argument("launch") then

    runners.loadbase()
    runners.launch_file(filename)

elseif e_argument("associate") then

    runners.associate(filename)

elseif e_argument("gethelp") then

    runners.gethelp()

elseif e_argument("makestubs") then

    -- make stubs (depricated)

    runners.handle_stubs(true)

elseif e_argument("removestubs") then

    -- remove stub (depricated)

    runners.loadbase()
    runners.handle_stubs(false)

elseif e_argument("resolve") then

    -- resolve string

    runners.loadbase()
    runners.resolve_string(filename)

elseif e_argument("locate") then

    -- locate file (only database)

    runners.loadbase()
    runners.locate_file(filename)

elseif e_argument("platform") or e_argument("show-platform") then

    -- locate platform

    runners.loadbase()
    runners.locate_platform()

elseif e_argument("prefixes") then

    runners.loadbase()
    runners.prefixes()

elseif e_argument("timedrun") then

    -- locate platform

    runners.loadbase()
    runners.timedrun(filename)

elseif e_argument("variables") or e_argument("show-variables") or e_argument("expansions") or e_argument("show-expansions") then

    -- luatools: runners.execute_ctx_script("mtx-base","--expansions",filename)

    resolvers.load("nofiles")
    resolvers.listers.variables(e_argument("pattern"))

elseif e_argument("configurations") or e_argument("show-configurations") then

    -- luatools: runners.execute_ctx_script("mtx-base","--configurations",filename)

    resolvers.load("nofiles")
    resolvers.listers.configurations()

elseif e_argument("find-file") then

    -- luatools: runners.execute_ctx_script("mtx-base","--find-file",filename)

    resolvers.load()
    local e_all     = e_argument("all")
    local e_pattern = e_argument("pattern")
    local e_format  = e_argument("format")
    local finder    = e_all and resolvers.findfiles or resolvers.findfile
    if not e_pattern then
        runners.register_arguments(filename)
        environment.initializearguments(environment.arguments_after)
        resolvers.dowithfilesandreport(finder,environment.files,e_format)
    elseif type(e_pattern) == "string" then
        resolvers.dowithfilesandreport(finder,{ e_pattern },e_format)
    end

elseif e_argument("find-path") then

    -- luatools: runners.execute_ctx_script("mtx-base","--find-path",filename)

    resolvers.load()
    local path = resolvers.findpath(filename, instance.my_format)
    if e_verbose then
        report(path)
    else
        print(path)
    end

elseif e_argument("expand-braces") then

    -- luatools: runners.execute_ctx_script("mtx-base","--expand-braces",filename)

    resolvers.load("nofiles")
    runners.register_arguments(filename)
    environment.initializearguments(environment.arguments_after)
    resolvers.dowithfilesandreport(resolvers.expandbraces, environment.files)

elseif e_argument("expand-path") then

    -- luatools: runners.execute_ctx_script("mtx-base","--expand-path",filename)

    resolvers.load("nofiles")
    runners.register_arguments(filename)
    environment.initializearguments(environment.arguments_after)
    resolvers.dowithfilesandreport(resolvers.expandpath, environment.files)

elseif e_argument("expand-var") or e_argument("expand-variable") then

    -- luatools: runners.execute_ctx_script("mtx-base","--expand-var",filename)

    resolvers.load("nofiles")
    runners.register_arguments(filename)
    environment.initializearguments(environment.arguments_after)
    resolvers.dowithfilesandreport(resolvers.expansion, environment.files)

elseif e_argument("show-path") or e_argument("path-value") then

    -- luatools: runners.execute_ctx_script("mtx-base","--show-path",filename)

    resolvers.load("nofiles")
    runners.register_arguments(filename)
    environment.initializearguments(environment.arguments_after)
    resolvers.dowithfilesandreport(resolvers.showpath, environment.files)

elseif e_argument("var-value") or e_argument("show-value") then

    -- luatools: runners.execute_ctx_script("mtx-base","--show-value",filename)

    resolvers.load("nofiles")
    runners.register_arguments(filename)
    environment.initializearguments(environment.arguments_after)
    resolvers.dowithfilesandreport(resolvers.variable,environment.files)

elseif e_argument("format-path") then

    -- luatools: runners.execute_ctx_script("mtx-base","--format-path",filename)

    resolvers.load()
    report(caches.getwritablepath("format"))

elseif e_argument("pattern") then

    -- luatools

    runners.execute_ctx_script("mtx-base","--pattern='" .. e_argument("pattern") .. "'",filename)

elseif e_argument("generate") then

    -- luatools

    if filename and filename ~= "" then
        resolvers.load("nofiles")
        trackers.enable("resolvers.locating")
        resolvers.renew(filename)
    else
        instance.renewcache = true
        trackers.enable("resolvers.locating")
        resolvers.load()
    end

    e_verbose = true

elseif e_argument("make") or e_argument("ini") or e_argument("compile") then

    -- luatools: runners.execute_ctx_script("mtx-base","--make",filename)

    resolvers.load()
    trackers.enable("resolvers.locating")
    environment.make_format(filename)

elseif e_argument("run") then

    -- luatools

    runners.execute_ctx_script("mtx-base","--run",filename)

elseif e_argument("fmt") then

    -- luatools

    runners.execute_ctx_script("mtx-base","--fmt",filename)

elseif e_argument("help") and filename=='base' then

    -- luatools

    runners.execute_ctx_script("mtx-base","--help")

elseif e_argument("version") then

    application.version()

elseif e_argument("directives") then

    directives.show()

elseif e_argument("trackers") then

    trackers.show()

elseif e_argument("experiments") then

    experiments.show()

elseif e_argument("exporthelp") then

    runners.loadbase()
    application.export(e_argument("exporthelp"),filename)

elseif e_argument("help") or filename=='help' or filename == "" then

    application.help()

elseif find(filename,"^bin:") then

    runners.loadbase()
    ok = runners.execute_program(filename)

elseif is_mkii_stub then

    -- execute mkii script

    runners.loadbase()
    ok = runners.execute_script(filename,false,true)

elseif false then

    runners.loadbase()
    ok = runners.execute_ctx_script(filename)
    if not ok then
        ok = runners.execute_script(filename)
    end

elseif environment.files[1] == 'texmfcnf.lua' then -- so that we don't need to load mtx-base

    resolvers.load("nofiles")
    resolvers.listers.configurations()

else
    runners.loadbase()
    runners.execute_ctx_script("mtx-base",filename)

end

if e_verbose then
    report()
    report("runtime: %0.3f seconds",os.runtime())
end

if os.type ~= "windows" then
    texio.write("\n") -- is this still valid?
end

if ok == false then ok = 1 elseif ok == true then ok = 0 end

os.exit(ok,true) -- true forces a cleanup in 5.2+
